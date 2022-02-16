#pragma once

#include "main.h"

#define BOOT_COMM_OVER_TIME 1000
#define FLASH_DEFINE_NUM    0xFFFFFFFF

#define APP_INFO_ADDRESS               0x08020000 /**64k * 2  bootloader */
#define APP_INFO                       ((app_info_t *)APP_INFO_ADDRESS)
#define APP_START_ADDRESS              (0x08020200)
#define APP_START_ADDRESS_ARRAY(index) (*((volatile uint32_t *)(APP_START_ADDRESS + index)))

#define BACK_RAM_RES          (*(__IO uint32_t *)0x40024000)
#define MAGIC_NUM_UAVCAN_BOOT 0xA0A0A0A0
#define MAGIC_NUM_UART_BOOT   0x746F6F62 // "boot"

/** APP 的信息字� 头部插入 512 字节 */
typedef struct
{
    uint32_t magic;
    uint32_t board_id;
    uint32_t app_start_address;
    uint32_t app_size;
    uint32_t app_crc;
    uint32_t build_timestamp;
    uint32_t board_version;
    uint32_t firmware_version;

    char build_time[19];
    char app_git_commit[40];
    char app_name[50];
    char device_id;
} app_info_t;

void boot_jump_to_app(void);
bool judge_app_jump(void);

int  app_crc_check(void);
void boot_over_time_jump(void);