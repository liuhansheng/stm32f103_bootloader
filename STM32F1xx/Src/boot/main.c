#include "main.h"
#include "system.h"
#include "bsp_uart1.h"
#include "bsp_io_out.h"
#include "bsp_crc32.h"
#include "app_handler.h"
#include "uart_boot.h"
#include "bsp_flash.h"
typedef enum
{
    BOOT_TYPE_NONE = 0,
    BOOT_TYPE_UART,
    BOOT_TYPE_UAVCAN,
} boot_type_e;
void uart_receive_data(uint8_t data)
{
    printf("%c",data);
}
void test_init()
{
    bsp_uart1_init();
    while(1)
    {
       char buff[100];
       sprintf(buff,"hello_world! %d \n",HAL_GetTick());
       bsp_uart1_send(buff,strlen(buff));
       HAL_Delay(1000);
    }
}

/** 判断是哪个接口触发的 BOOT 升级 */
uint8_t get_boot_type(void)
{
    static uint8_t boot_type = BOOT_TYPE_NONE;

    if (boot_type == BOOT_TYPE_NONE)
    {
        if (boot_uart_get_entry_seq())
        {
            // uart 引起 bootloder
            boot_type = BOOT_TYPE_UART;
        }
    }

    return boot_type;
}
/** 轮询处理升级任务*/
void boot_task(boot_type_e type)
{
    switch (type)
    {
    case BOOT_TYPE_NONE:
        /* code */
        boot_over_time_jump();
        break;
    case BOOT_TYPE_UART:
        boot_uart_handle_update();
        break;
    
    default:
        break;
    }
}
int main( void ) 

{ 
    sys_init();
    bsp_crc32_init();
    bsp_uart1_init();
    bsp_io_out_config();
    bsp_flash_init();
    while(1)
    {
        boot_task(get_boot_type());
        bsp_led_flashing();
      
    }
}

/*************************** End of file ****************************/

