#include "main.h"
#include "mitsubishi_ac.h"

#define LOOKUP_TABLE_MIN 80
// TEMPERATURE FOR 80 to 200 ADC RAW LOOKUP TABLE
/*
      [21.69360918, 21.98809541, 22.2798558 , 22.5689536 , 22.85544988,
       23.13940363, 23.42087187, 23.6999097 , 23.97657044, 24.25090566,
       24.52296528, 24.79279765, 25.06044958, 25.32596644, 25.58939222,
       25.85076954, 26.11013977, 26.36754304, 26.6230183 , 26.87660335,
       27.12833493, 27.37824871, 27.62637935, 27.87276057, 28.11742511,
       28.36040485, 28.60173079, 28.84143309, 29.07954111, 29.31608345,
       29.55108795, 29.78458171, 30.01659116, 30.24714205, 30.47625948,
       30.70396793, 30.93029125, 31.15525273, 31.37887508, 31.60118046,
       31.82219051, 32.04192636, 32.26040862, 32.47765744, 32.69369249,
       32.908533  , 33.12219777, 33.33470515, 33.54607309, 33.75631916,
       33.96546051, 34.17351394, 34.38049589, 34.58642241, 34.79130925,
       34.9951718 , 35.19802514, 35.39988402, 35.6007629 , 35.80067594,
       35.99963699, 36.19765967, 36.39475726, 36.59094283, 36.78622916,
       36.98062879, 37.17415401, 37.36681689, 37.55862923, 37.74960265,
       37.93974851, 38.12907798, 38.31760201, 38.50533136, 38.69227656,
       38.87844799, 39.06385582, 39.24851001, 39.4324204 , 39.61559659,
       39.79804807, 39.97978411, 40.16081385, 40.34114627, 40.52079018,
       40.69975425, 40.878047  , 41.0556768 , 41.23265188, 41.40898035,
       41.58467017, 41.75972917, 41.93416504, 42.10798537, 42.28119761,
       42.45380909, 42.62582704, 42.79725855, 42.96811062, 43.13839012,
       43.30810383, 43.47725842, 43.64586045, 43.81391638, 43.9814326 ,
       44.14841535, 44.31487083, 44.48080512, 44.6462242 , 44.81113399,
       44.97554029, 45.13944886, 45.30286533, 45.46579527, 45.62824418,
       45.79021745, 45.95172044, 46.1127584 , 46.2733365 , 46.43345987]
*/

#define AP_SSID "Shinu BSNL"
#define AP_PASS "987654321"
#define PORT 8000
#define ADC_SAMPLES 100

//macros
#define TIMING_CRITICAL(X) \
	taskENTER_CRITICAL();  \
	X;                     \
	taskEXIT_CRITICAL()

const uint8_t ON_SIGNAL[] = {0xb5, 0x8a, 0x3c, 0x9b, 0x64, 0x41, 0xbe, 0x0b, 0xf4, 0x10, 0xef};
const uint8_t OFF_SIGNAL[] = {0xb5, 0x8a, 0x3c, 0x9b, 0x64, 0x41, 0xbe, 0x0b, 0xf4, 0x00, 0xff};

//temperature related vars
static uint16_t adc_data[ADC_SAMPLES];
static uint32_t adc_reading = 0;
static const double temperature_lookup_table[120] = {
	21.694, 21.988, 22.280, 22.569, 22.855, 23.139, 23.421, 23.700,
	23.977, 24.251, 24.523, 24.793, 25.060, 25.326, 25.589, 25.851,
	26.110, 26.368, 26.623, 26.877, 27.128, 27.378, 27.626, 27.873,
	28.117, 28.360, 28.602, 28.841, 29.080, 29.316, 29.551, 29.785,
	30.017, 30.247, 30.476, 30.704, 30.930, 31.155, 31.379, 31.601,
	31.822, 32.042, 32.260, 32.478, 32.694, 32.909, 33.122, 33.335,
	33.546, 33.756, 33.965, 34.174, 34.380, 34.586, 34.791, 34.995,
	35.198, 35.400, 35.601, 35.801, 36.000, 36.198, 36.395, 36.591,
	36.786, 36.981, 37.174, 37.367, 37.559, 37.750, 37.940, 38.129,
	38.318, 38.505, 38.692, 38.878, 39.064, 39.249, 39.432, 39.616,
	39.798, 39.980, 40.161, 40.341, 40.521, 40.700, 40.878, 41.056,
	41.233, 41.409, 41.585, 41.760, 41.934, 42.108, 42.281, 42.454,
	42.626, 42.797, 42.968, 43.138, 43.308, 43.477, 43.646, 43.814,
	43.981, 44.148, 44.315, 44.481, 44.646, 44.811, 44.976, 45.139,
	45.303, 45.466, 45.628, 45.790, 45.952, 46.113, 46.273, 46.433};

static double temperature = 0.0;
static uint8_t ac_currently_on = 0;

//socket related variables
static int listening_socket;
static int accepting_socket;
static struct sockaddr_in info;
static struct sockaddr_in remote_info;

void temperature_correcting_task(void *arg)
{
	const char *TAG = "temperature_correcting_task";
	ESP_LOGI(TAG, "Starting Task!\n");
	while (1)
	{
		if (temperature > 28)
		{
			ESP_LOGI(TAG, "Turning on AC!\n");
			if (ac_currently_on == 0)
			{
				TIMING_CRITICAL(MAC_TransmitConfigStruct((uint8_t *)ON_SIGNAL, 11));
				ac_currently_on = 1;
			}
		}
		else
		{
			ESP_LOGI(TAG, "Turning off AC!\n");
			if (ac_currently_on == 1)
			{
				TIMING_CRITICAL(MAC_TransmitConfigStruct((uint8_t *)OFF_SIGNAL, 11));
				ac_currently_on = 0;
			}
		}
		vTaskDelay((60 * 1000) / portTICK_RATE_MS);
	}
}

static uint32_t next_event;

void EXPERIMENTAL_TASK(void *arg)
{
	const char *TAG = "EXPERIMENTAL_TASK";
	while (1)
	{
		ESP_LOGI(TAG, "Turning On AC\n");
		TIMING_CRITICAL(MAC_TransmitConfigStruct((uint8_t *)ON_SIGNAL, sizeof(ON_SIGNAL))); //turn on
		vTaskDelay((5 * 60 * 1000) / portTICK_RATE_MS);										//for 5 minutes
		ESP_LOGI(TAG, "Turning Off AC\n");
		TIMING_CRITICAL(MAC_TransmitConfigStruct((uint8_t *)OFF_SIGNAL, sizeof(OFF_SIGNAL))); //turn off
		vTaskDelay((10 * 60 * 1000) / portTICK_RATE_MS);									  //for 30 minutes
	}
}

void temperature_reading_task(void *arg)
{
	const char *TAG = "temperature_reading_task";
	ESP_LOGI(TAG, "Starting task!!\n");
	uint8_t counter;
	adc_config_t adc;
	adc.clk_div = 16; //160MHz / 16 = 10MHz
	adc.mode = ADC_READ_TOUT_MODE;
	adc_init(&adc);
	while (1)
	{

		adc_read_fast(adc_data, ADC_SAMPLES);
		for (counter = 0, adc_reading = 0; counter < ADC_SAMPLES; counter++)
		{
			adc_reading += (uint32_t)adc_data[counter];
		}
		adc_reading /= ADC_SAMPLES;
		temperature = temperature_lookup_table[adc_reading - LOOKUP_TABLE_MIN];
		ESP_LOGI(TAG, "Temperature -> %d -> %f!!\n", adc_reading, temperature);
		ESP_LOGI(TAG, "NEXT EVENT AT %d\n", next_event);
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

void acceptor_socket_task(void *arg)
{
	const char *TAG = "acceptor_socket_task";
	char buffer[100];
	ESP_LOGI(TAG, "Running!\n");
	int return_status;
	while (1)
	{
		return_status = send(accepting_socket, &adc_reading, sizeof(adc_reading), 0);
		recv(accepting_socket, buffer, sizeof(buffer), MSG_DONTWAIT);
		if (return_status < 0)
		{
			ESP_LOGI(TAG, "Error in sending! Exiting!\n");
			shutdown(accepting_socket, 2);
			shutdown(listening_socket, 2);
			vTaskDelete(NULL);
		}
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

void listening_socket_task(void *arg)
{
	const char *TAG = "listening_socket_task";
	ESP_LOGI(TAG, "Starting task!\n");
	char one = 1;
	socklen_t siz = sizeof(info);
	listening_socket = socket(AF_INET, SOCK_STREAM, 0);
	info.sin_addr.s_addr = htonl(INADDR_ANY);
	info.sin_port = htons(PORT);
	info.sin_family = AF_INET;
	setsockopt(listening_socket, SOL_SOCKET, SO_REUSEPORT, &one, sizeof(one));
	bind(listening_socket, (struct sockaddr *)&info, sizeof(info));
	while (1)
	{
		listen(listening_socket, 1);
		accepting_socket = accept(listening_socket, (struct sockaddr *)&remote_info, &siz);
		xTaskCreate(acceptor_socket_task, "acceptor_socket_task", 2048, NULL, 10, NULL);
	}
}

void evt_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	const char *TAG = "evt_handler";
	ESP_LOGI(TAG, "Event Handler called!");
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
	{
		esp_wifi_connect();
	}
	else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
	{
		xTaskCreate(listening_socket_task, "listening_socket_task", 2048, NULL, 10, NULL);
	}
}

void app_main()
{
	static const char *TAG = "APP_MAIN";
	ESP_LOGI(TAG, "MAIN APP STARTED!\n");
	//gpio init
	//setting status LED
	gpio_config_t gp;
	gp.mode = GPIO_MODE_OUTPUT_OD;
	gp.pin_bit_mask = 1 << GPIO_NUM_16;
	gp.pull_down_en = 0;
	gp.pull_up_en = 0;
	ESP_ERROR_CHECK(gpio_config(&gp));
	ESP_ERROR_CHECK(gpio_set_level(GPIO_NUM_16, 0));

	//initializing IR LED
	MAC_GPIO_Init();

	tcpip_adapter_init();
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, evt_handler, NULL));
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, evt_handler, NULL));

	wifi_config_t wifi_config = {
		.sta = {
			.ssid = AP_SSID,
			.password = AP_PASS},
	};

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());

	// xTaskCreate(temperature_reading_task, "temperature_reading_task", 2048, NULL, 10, NULL);
	xTaskCreate(EXPERIMENTAL_TASK, "EXPERIMENTAL_TASK", 2048, NULL, 10, NULL);
	// xTaskCreate(temperature_correcting_task, "temperature_correcting_task", 2048, NULL, 10, NULL);
}
