#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "stm32f1xx_hal.h"
#include "lwrb/lwrb.h"
#define ARRAY_LEN(x) (sizeof(x)/sizeof((x)[0]))
#define MIN(x,y) ((x) < (y)) ? (x) : (y)
typedef void (* usart_rx_handler_cb)(const uint8_t data);