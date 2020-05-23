#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "esp_event.h"
#include "lwip/tcp.h"
#include "lwip/err.h"
#include "pthread.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "esp_wifi.h"
#include "esp_netif.h"
#include "math.h"
#include "esp_sleep.h"

//drivers
#include "driver/adc.h"
#include "driver/gpio.h"
#include "driver/hw_timer.h"
#include "driver/ir_tx.h"

#define AP_SSID "Shinu BSNL"
#define AP_PASS "987654321"
#define PORT 8000


#endif
