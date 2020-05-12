#include "mitsubishi_ac.h"

void MAC_GPIO_Init()
{
	gpio_config_t gp;
	gp.mode = GPIO_MODE_OUTPUT;
	gp.pin_bit_mask = 1 << MAC_PIN;
	gp.pull_down_en = 0;
	gp.pull_up_en = 0;
	ESP_ERROR_CHECK(gpio_config(&gp));
	ESP_ERROR_CHECK(gpio_set_level(MAC_PIN, 1));
	MAC_PIN_LOW;
}

void MAC_Preamble()
{
	uint8_t counter;
	//MAC_HEADER
	for (counter = 0; counter < 121; counter++)
	{
		MAC_PIN_HIGH;
		MAC_DELAY_US(MAC_ON_DELAY);
		MAC_PIN_LOW;
		MAC_DELAY_US(MAC_OFF_DELAY);
	}

	MAC_DELAY_US(MAC_HEADER_SPACE);
}

void MAC_TransmitConfigStruct(uint8_t *ptr, uint8_t len)
{
	uint8_t byte_counter, bit_counter, timing_counter;
	MAC_Preamble();
	for (byte_counter = 0; byte_counter < len; byte_counter++)
	{
		for (bit_counter = 0; bit_counter < 8; bit_counter++)
		{
			//generating bit mark
			//15 * 26us
			for (timing_counter = 0; timing_counter < 15; timing_counter++)
			{
				MAC_PIN_HIGH;
				MAC_DELAY_US(MAC_ON_DELAY);
				MAC_PIN_LOW;
				MAC_DELAY_US(MAC_OFF_DELAY);
			}
			//have to send data MSB first
			if ((ptr[byte_counter] >> (7 - bit_counter)) & 1) //if 1
			{
				MAC_DELAY_US(MAC_ONE - MAC_OFF_DELAY);
			}
			else
			{
				MAC_DELAY_US(MAC_ZERO - MAC_OFF_DELAY);
			}
		}
	}

	//generating end bit
	for (timing_counter = 0; timing_counter < 15; timing_counter++)
	{
		MAC_PIN_HIGH;
		MAC_DELAY_US(MAC_ON_DELAY);
		MAC_PIN_LOW;
		MAC_DELAY_US(MAC_OFF_DELAY);
	}
}
