#ifndef MITSUBISHI_AC_H
#define MITSUBISHI_AC_H

#include "main.h"

//CONSTANTS
//time delays in us
#define MAC_HEADER 3146
#define MAC_HEADER_SPACE 1582
#define MAC_BIT 400
#define MAC_ONE 390
#define MAC_ZERO 1184

#define MAC_ON_DELAY 12
#define MAC_OFF_DELAY 13
#define MAC_PIN GPIO_NUM_4

//macro
#define MAC_DELAY_US(X) ets_delay_us(X)
#define MAC_PIN_LOW gpio_set_level(MAC_PIN, 0)
#define MAC_PIN_HIGH gpio_set_level(MAC_PIN, 1)

//public function declarations
void MAC_GPIO_Init();
void MAC_Preamble();
void MAC_TransmitConfigStruct(uint8_t *ptr, uint8_t len);

#endif
