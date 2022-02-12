#include "main.h"
#include "bsp_io_out.h"

#define OUTPUT_TYPE_PP 0
#define OUTPUT_TYPE_OD 1

typedef struct
{
    GPIO_TypeDef *gpio;  // GPIO address
    uint16_t      pin;   // pin num
    uint8_t       state; // IO state
    uint8_t       type;
} gpio_define_t;

static gpio_define_t bsp_do_map[] = {
    {LED_R_GPIO_Port, LED_R_Pin, 1, OUTPUT_TYPE_PP},   // DO_LED_BLUE
    {LED_G_GPIO_Port, LED_G_Pin, 1, OUTPUT_TYPE_PP}, // DO_LED_GREEN
    {LED_B_GPIO_Port, LED_B_Pin, 1, OUTPUT_TYPE_PP},     // DO_LED_RED
};


void bsp_io_out_config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Speed            = GPIO_SPEED_FREQ_HIGH;
    for (uint16_t i = 0; i < ARRAY_LEN(bsp_do_map); i++)
    {
        if (bsp_do_map[i].type == OUTPUT_TYPE_PP)
        {
            GPIO_InitStruct.Pull = GPIO_PULLUP;
            GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        }
        else
        {
            GPIO_InitStruct.Pull = GPIO_PULLUP;
            GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
        }

        GPIO_InitStruct.Pin = bsp_do_map[i].pin;
        HAL_GPIO_Init(bsp_do_map[i].gpio, &GPIO_InitStruct);
        HAL_GPIO_WritePin(bsp_do_map[i].gpio, bsp_do_map[i].pin, bsp_do_map[i].state);
    }
}

int bsp_write_do(uint8_t n, uint8_t state)
{
    gpio_define_t *io;

    if (n < ARRAY_LEN(bsp_do_map))
    {
        io        = bsp_do_map + n;
        io->state = state;
        HAL_GPIO_WritePin(io->gpio, io->pin, (GPIO_PinState)state);

        return REC_OK;
    }

    return ERR_DATA_OVERWRITE;
}

void bsp_write_do_toggle(uint8_t n)
{
    gpio_define_t *io;

    if (n < ARRAY_LEN(bsp_do_map))
    {
        io        = bsp_do_map + n;
        io->state = !io->state;
        HAL_GPIO_WritePin(io->gpio, io->pin, (GPIO_PinState)io->state);
    }
}

void bsp_led_flashing(void)
{
    static uint32_t tick = 0;

    if (HAL_GetTick() > tick + 50)
    {
        tick = HAL_GetTick();
        bsp_write_do_toggle(DO_LED_BLUE); // System Running
    }
}