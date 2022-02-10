#pragma once

#include <stdbool.h>
#include <stdint.h>

#define OUTPUT_TYPE_PP 0
#define OUTPUT_TYPE_OD 1

#define LED_G_Pin GPIO_PIN_0
#define LED_G_GPIO_Port GPIOB
#define LED_B_Pin GPIO_PIN_1
#define LED_B_GPIO_Port GPIOB
#define LED_R_Pin GPIO_PIN_5
#define LED_R_GPIO_Port GPIOB

typedef enum
{
    DO_LED_RED = 0,
    DO_LED_GREEN = 1,
    DO_LED_BLUE = 2,
} do_map_enum;

#define REC_OK             (0)
#define ERR_DATA_OVERWRITE (-1)
#define ERR_OPERATION      (-2)

void bsp_io_out_config(void);
int  bsp_write_do(uint8_t n, uint8_t state);
void bsp_led_flashing(void);
void bsp_write_do_toggle(uint8_t n);