#include "main.h"
#include "system.h"
#include "bsp_uart1.h"
#define OUTPUT_TYPE_PP 0
#define OUTPUT_TYPE_OD 1
typedef enum
{
    DO_LED_RED = 0,
    DO_LED_GREEN = 1,
    DO_LED_BLUE = 2,
} do_map_enum;
#define ARRAY_LEN(x) (sizeof(x) / sizeof((x)[0]))
typedef struct
{
    GPIO_TypeDef *gpio;  // GPIO address
    uint16_t      pin;   // pin num
    uint8_t       state; // IO state
    uint8_t       type;
} GPIO_DEFINE_TYPE;

// IO MAP
static GPIO_DEFINE_TYPE bsp_do_map[] = {
    {GPIOB, GPIO_PIN_5, 1, OUTPUT_TYPE_PP}, // DO_LED_RED
    {GPIOB, GPIO_PIN_0, 1, OUTPUT_TYPE_PP}, // DO_LED_GREEN
    {GPIOB, GPIO_PIN_1, 1, OUTPUT_TYPE_PP}, // DO_LED_BLUE
};
void bsp_write_do(uint8_t n, uint8_t state)
{
    GPIO_DEFINE_TYPE *io;

    if (n < ARRAY_LEN(bsp_do_map))
    {
        io        = bsp_do_map + n;
        io->state = state;
        HAL_GPIO_WritePin(io->gpio, io->pin, (GPIO_PinState)state);
    }

}
void bsp_write_do_toggle(uint8_t n)
{
    GPIO_DEFINE_TYPE *io;

    if (n < ARRAY_LEN(bsp_do_map))
    {
        io        = bsp_do_map + n;
        io->state = !io->state;
        HAL_GPIO_WritePin(io->gpio, io->pin, (GPIO_PinState)io->state);
        return;
    }
}
void LED_Init(void)
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
    }
    for (uint16_t i = 0; i < ARRAY_LEN(bsp_do_map); i++)
    {
        HAL_GPIO_WritePin(bsp_do_map[i].gpio, bsp_do_map[i].pin, bsp_do_map[i].state);
    }
}

void vTaskLED1(void * pvParameters)
{ 
  LED_Init();
  while(1)
  {
    bsp_write_do_toggle(DO_LED_RED);
    printf("hello world %d !\r\n",HAL_GetTick());
    HAL_Delay(1000); 
  }
}
void uart_receive_data(uint8_t data)
{
    printf("%c",data);
}
void test_init()
{
    bsp_uart1_init();
    bsp_uart1_install_rx_install(uart_receive_data);
    while(1)
    {
        char buff[100];
        sprintf(buff,"hello_world! %d \n",HAL_GetTick());
        bsp_uart1_send(buff,strlen(buff));
        HAL_Delay(1000);
    }
}
int main( void ) 
{ 
    sys_init();
    test_init();
    //vTaskLED1(NULL);
    //while(1);
}

/*************************** End of file ****************************/
