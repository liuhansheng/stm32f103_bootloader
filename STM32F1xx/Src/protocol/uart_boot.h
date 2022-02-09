#pragma once

#include <stdbool.h>
#include <stdint.h>

#pragma pack(push)
#pragma pack(1)
typedef struct
{
    uint32_t magic_num;
    uint32_t board_id;
    uint32_t start_address;
    uint32_t app_len;
    uint32_t crc32;
    uint32_t build_timestamp;
    uint32_t board_version;
    uint32_t fw_version;
    char     strftime[19];
    char     commit_id[40];
    char     app_name[50];
} header_t;
#pragma pack(pop)

#define HEADER_MAGIC_NUM 0xA0A0A0A0
#define BOARD_ID         0x10

int  boot_uart_handle_update(void);
bool boot_uart_get_entry_seq(void);