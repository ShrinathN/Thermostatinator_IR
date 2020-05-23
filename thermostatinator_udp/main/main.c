#include "main.h"
#include "mitsubishi_ac.h"

#define AP_SSID "Shinu BSNL"
#define AP_PASS "987654321"
#define PORT 8080

void sock_stuff()
{
	uint8_t buff[100];
	ESP_LOGI("TAG", "Runnning!\n");
	int s = socket(AF_INET, SOCK_DGRAM, 0);
	uint8_t len;
	struct sockaddr_in info;
	info.sin_addr.s_addr = htonl(INADDR_ANY);
	info.sin_family = AF_INET;
	info.sin_port = htons(PORT);
	socklen_t siz = sizeof(info);
	bind(s, (struct sockaddr*)&info, sizeof(struct sockaddr_in));

	while(1)
	{
		len = recvfrom(s, buff, 100, MSG_WAITALL, (struct sockaddr*)&info, &siz);
		taskENTER_CRITICAL();
		MAC_TransmitConfigStruct(buff, len);
		taskEXIT_CRITICAL();
		bzero(buff, 100);
	}
}

void event_han(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	if (event_base == WIFI_EVENT)
	{
		if (event_id == WIFI_EVENT_STA_START)
		{
			esp_wifi_connect();
		}
	}
	else if (event_base == IP_EVENT)
	{
		if (event_id == IP_EVENT_STA_GOT_IP)
		{
			ip_event_got_ip_t * data = (ip_event_got_ip_t *)event_data;
			ESP_LOGI("hi", "IP"IPSTR, IP2STR(&data->ip_info.ip));
			xTaskCreate(sock_stuff, "sock_stuff", 2048, NULL, 10, NULL);
		}
	}
}

void app_main()
{
	tcpip_adapter_init();
	esp_event_loop_create_default();
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	esp_wifi_init(&cfg);

	esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, event_han, NULL);
	esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, event_han, NULL);

	MAC_GPIO_Init();

	wifi_config_t wifi_config = {
		.sta = {
			.ssid = AP_SSID,
			.password = AP_PASS},};

	esp_wifi_set_mode(WIFI_MODE_STA);
	esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
	esp_wifi_start();
}
