#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "stm32f1xx_hal.h"
#include "lwrb/lwrb.h"
typedef void (* usart_rx_handler_cb)(const uint8_t data);