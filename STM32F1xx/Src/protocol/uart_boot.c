// #include <stdio.h>

// #include "stm32f4xx_hal.h"
// #include "bsp_uart.h"
// #include "bsp_flash.h"

// #include "uart_boot.h"
// #include "app_handle.h"

// #define MIN(X, Y) ((X) < (Y) ? (X) : (Y))

// #define BL_PROTOCOL_VERSION 5 // The revision of the bootloader protocol
// //* Next revision needs to update

// // protocol bytes
// #define PROTO_INSYNC 0x12 // 'in sync' byte sent before status
// #define PROTO_EOC    0x20 // end of command

// // Reply bytes
// #define PROTO_OK              0x10 // INSYNC/OK      - 'ok' response
// #define PROTO_FAILED          0x11 // INSYNC/FAILED  - 'fail' response
// #define PROTO_INVALID         0x13 // INSYNC/INVALID - 'invalid' response for bad commands
// #define PROTO_BAD_SILICON_REV 0x14 // On the F4 series there is an issue with < Rev 3 silicon
// #define PROTO_RESERVED_0X15   0x15 // Reserved

// // see https://pixhawk.org/help/errata
// // Command bytes
// #define PROTO_GET_SYNC     0x21 // NOP for re-establishing sync
// #define PROTO_GET_DEVICE   0x22 // get device ID bytes
// #define PROTO_CHIP_ERASE   0x23 // erase program area and reset program address
// #define PROTO_PROG_MULTI   0x27 // write bytes at program address and increment
// #define PROTO_GET_CRC      0x29 // compute & return a CRC
// #define PROTO_GET_OTP      0x2a // read a byte from OTP at the given address
// #define PROTO_GET_SN       0x2b // read a word from UDID area ( Serial)  at the given address
// #define PROTO_GET_CHIP     0x2c // read chip version (MCU IDCODE)
// #define PROTO_SET_DELAY    0x2d // set minimum boot delay
// #define PROTO_GET_CHIP_DES 0x2e // read chip version In ASCII
// #define PROTO_BOOT         0x30 // boot the application
// #define PROTO_DEBUG        0x31 // emit debug information - format not defined
// #define PROTO_SET_BAUD     0x33 // set baud rate on uart

// #define PROTO_RESERVED_0X36 0x36 // Reserved
// #define PROTO_RESERVED_0X37 0x37 // Reserved
// #define PROTO_RESERVED_0X38 0x38 // Reserved
// #define PROTO_RESERVED_0X39 0x39 // Reserved

// #define PROTO_PROG_MULTI_MAX 64  // maximum PROG_MULTI size
// #define PROTO_READ_MULTI_MAX 255 // size of the size field

// /* argument values for PROTO_GET_DEVICE */
// #define PROTO_DEVICE_BL_REV    1 // bootloader revision
// #define PROTO_DEVICE_BOARD_ID  2 // board ID
// #define PROTO_DEVICE_BOARD_REV 3 // board revision
// #define PROTO_DEVICE_FW_SIZE   4 // size of flashable area
// #define PROTO_DEVICE_VEC_AREA  5 // contents of reserved vectors 7-10

// // State
// #define STATE_PROTO_GET_SYNC     0x1   // Have Seen NOP for re-establishing sync
// #define STATE_PROTO_GET_DEVICE   0x2   // Have Seen get device ID bytes
// #define STATE_PROTO_CHIP_ERASE   0x4   // Have Seen erase program area and reset program address
// #define STATE_PROTO_PROG_MULTI   0x8   // Have Seen write bytes at program address and increment
// #define STATE_PROTO_GET_CRC      0x10  // Have Seen compute & return a CRC
// #define STATE_PROTO_GET_OTP      0x20  // Have Seen read a byte from OTP at the given address
// #define STATE_PROTO_GET_SN       0x40  // Have Seen read a word from UDID area ( Serial)  at the given address
// #define STATE_PROTO_GET_CHIP     0x80  // Have Seen read chip version (MCU IDCODE)
// #define STATE_PROTO_GET_CHIP_DES 0x100 // Have Seen read chip version In ASCII
// #define STATE_PROTO_BOOT         0x200 // Have Seen boot the application

// #define STATE_ALLOWS_ERASE    (STATE_PROTO_GET_SYNC | STATE_PROTO_GET_DEVICE)
// #define STATE_ALLOWS_REBOOT   (STATE_ALLOWS_ERASE | STATE_PROTO_PROG_MULTI | STATE_PROTO_GET_CRC)
// #define SET_BL_STATE(s)       bl_state |= (s)
// #define SET_BL_FIRST_STATE(s) bl_state |= (s)

// #define FW_START     0x08020000
// #define VECTOR_START (FW_START + 512)
// #define FW_MAX_SIZE  ((1024 - 64) * 1024)

// struct boardinfo
// {
//     uint32_t board_type;
//     uint32_t board_rev;
//     uint32_t fw_size;
//     uint32_t systick_mhz;
// } __attribute__((packed));

// /*****************************************************
//     �  述：  CRC8校验子程�x^8+x^5+x^4+x^0
//     入口参数�指向数组指针，校验字节个�
//     出口参数�8位CRC校验�
// ******************************************************/
// const char crc8_table[]
//     = {0,   94,  188, 226, 97,  63,  221, 131, 194, 156, 126, 32,  163, 253, 31,  65,  157, 195, 33,  127, 252, 162, 64,  30,
//        95,  1,   227, 189, 62,  96,  130, 220, 35,  125, 159, 193, 66,  28,  254, 160, 225, 191, 93,  3,   128, 222, 60,  98,
//        190, 224, 2,   92,  223, 129, 99,  61,  124, 34,  192, 158, 29,  67,  161, 255, 70,  24,  250, 164, 39,  121, 155, 197,
//        132, 218, 56,  102, 229, 187, 89,  7,   219, 133, 103, 57,  186, 228, 6,   88,  25,  71,  165, 251, 120, 38,  196, 154,
//        101, 59,  217, 135, 4,   90,  184, 230, 167, 249, 27,  69,  198, 152, 122, 36,  248, 166, 68,  26,  153, 199, 37,  123,
//        58,  100, 134, 216, 91,  5,   231, 185, 140, 210, 48,  110, 237, 179, 81,  15,  78,  16,  242, 172, 47,  113, 147, 205,
//        17,  79,  173, 243, 112, 46,  204, 146, 211, 141, 111, 49,  178, 236, 14,  80,  175, 241, 19,  77,  206, 144, 114, 44,
//        109, 51,  209, 143, 12,  82,  176, 238, 50,  108, 142, 208, 83,  13,  239, 177, 240, 174, 76,  18,  145, 207, 45,  115,
//        202, 148, 118, 40,  171, 245, 23,  73,  8,   86,  180, 234, 105, 55,  213, 139, 87,  9,   235, 181, 54,  104, 138, 212,
//        149, 203, 41,  119, 244, 170, 72,  22,  233, 183, 85,  11,  136, 214, 52,  106, 43,  117, 151, 201, 74,  20,  246, 168,
//        116, 42,  200, 150, 21,  75,  169, 247, 182, 232, 10,  84,  215, 137, 107, 53};

// static uint8_t crc8_calc(unsigned char *p, char counter)
// {
//     uint8_t crc8 = 0;

//     for (; counter > 0; counter--)
//     {
//         crc8 = crc8_table[crc8 ^ *p];
//         p++;
//     }

//     return crc8;
// }

// static int cout(uint8_t *data, size_t len)
// {
//     int i = bsp_uart1_send(data, len);
//     while (!bsp_get_uart_dma_tx_cplt_flag()) /**  wait send all */
//         ;
//     return i;
// }

// static void sync_response(void)
// {
//     uint8_t data[] = {
//         PROTO_INSYNC, // "in sync"
//         PROTO_OK      // "OK"
//     };

//     cout(data, sizeof(data));
// }

// static void failure_response(void)
// {
//     uint8_t data[] = {
//         PROTO_INSYNC, // "in sync"
//         PROTO_FAILED  // "command failed"
//     };

//     cout(data, sizeof(data));
// }

// static void invalid_response(void)
// {
//     uint8_t data[] = {
//         PROTO_INSYNC, // "in sync"
//         PROTO_INVALID // "invalid command"

//     };

//     cout(data, sizeof(data));
// }

// static int cin_wait(int timeout)
// {
//     uint8_t  c;
//     int      w;
//     uint32_t tick_start, tick_end;
//     uint32_t tick_escape = 0;

//     if (timeout == 0)
//     {
//         return (0 == uart_buff_read(&c, 1)) ? -1 : (w = c); // NOLINT
//     }

//     tick_start = HAL_GetTick();
//     while (0 == uart_buff_read(&c, 1))
//     {
//         tick_end    = HAL_GetTick();
//         tick_escape = tick_end - tick_start;
//         if (tick_escape > timeout)
//         {
//             return -1;
//         }
//     }

//     w = c;
//     return w;
// }

// static int wait_for_eoc(int timeout)
// {
//     return (cin_wait(timeout) == PROTO_EOC);
// }

// static const uint32_t bl_proto_rev = 5;

// struct boardinfo board_info = {
//     .board_type  = 9,
//     .board_rev   = 0,
//     .fw_size     = FW_MAX_SIZE,
//     .systick_mhz = 168,
// };

// static void erase_all(void)
// {
//     bsp_flash_erase(APP_INFO_ADDRESS, 384 * 1024);
// }

// static void write(uint32_t start_addr, const void *data, uint32_t data_len)
// {
//     const uint8_t *p = data;

//     HAL_FLASH_Unlock();
//     for (uint32_t i = 0; i < (data_len / 4); i++)
//     {
//         HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FW_START + start_addr + i, *(p + 4));
//     }
//     HAL_FLASH_Lock();
// }

// static void read(uint32_t start_addr, void *data, uint32_t data_len)
// {
//     memcpy(data, (void *)(FW_START + start_addr), data_len);
// }

// static bool is_app_valid(void)
// {
//     return app_crc_check() == 0;
// }

// bool boot_uart_get_entry_seq(void)
// {
//     /* 进入boot模式的序列检�/
//     /* "boot" */
//     static uint8_t state = 0;
//     if (4 == state)
//     {
//         // 已经检测过序列�
//         return true;
//     }

//     uint8_t c = cin_wait(1);
//     if (c < 0)
//     {
//         return false;
//     }

//     switch (state)
//     {
//     case 0:
//         if ('b' == c)
//         {
//             state = 1;
//         }
//         else
//         {
//             state = 0;
//         }
//         break;

//     case 1:
//         if ('o' == c)
//         {
//             state = 2;
//         }
//         else
//         {
//             state = 0;
//         }

//     case 2:
//         if ('o' == c)
//         {
//             state = 3;
//         }
//         else
//         {
//             state = 0;
//         }
//         break;

//     case 3:
//         if ('t' == c)
//         {
//             state = 4;
//         }
//         break;
//     }

//     if (4 == state)
//     {
//         sync_response();
//         return true;
//     }

//     return false;
// }

// static volatile uint32_t bl_state = 0;
// static uint32_t          address  = 0;
// static int               arg;
// static uint8_t           flash_buffer[252];

// int boot_uart_handle_update(void)
// {
//     do
//     {
//         volatile int c;
//         c = cin_wait(0);
//         if (c < 0)
//         {
//             return 0;
//         }

//         switch (c)
//         {
//         case PROTO_GET_SYNC:
//             if (!wait_for_eoc(2))
//             {
//                 goto cmd_bad;
//             }
//             address = 0;
//             SET_BL_FIRST_STATE(STATE_PROTO_GET_SYNC);
//             break;

//         case PROTO_GET_DEVICE:
//             /* expect arg then EOC */
//             arg = cin_wait(1000);

//             if (arg < 0)
//             {
//                 goto cmd_bad;
//             }

//             if (!wait_for_eoc(2))
//             {
//                 goto cmd_bad;
//             }

//             switch (arg)
//             {
//             case PROTO_DEVICE_BL_REV:
//                 cout((uint8_t *)&bl_proto_rev, sizeof(bl_proto_rev));
//                 break;

//             case PROTO_DEVICE_BOARD_ID:
//                 cout((uint8_t *)&board_info.board_type, sizeof(board_info.board_type));
//                 break;

//             case PROTO_DEVICE_BOARD_REV:
//                 cout((uint8_t *)&board_info.board_rev, sizeof(board_info.board_rev));
//                 break;

//             case PROTO_DEVICE_FW_SIZE:
//                 if (0 != address)
//                 {
//                     board_info.fw_size = address;
//                 }
//                 else
//                 {
//                     board_info.fw_size = FW_MAX_SIZE;
//                 }
//                 cout((uint8_t *)&board_info.fw_size, sizeof(board_info.fw_size));
//                 break;

//             case PROTO_DEVICE_VEC_AREA:
//                 for (unsigned p = 7; p <= 10; p++)
//                 {
//                     uint32_t bytes;
//                     read(p * 4, &bytes, 4);
//                     cout((uint8_t *)&bytes, sizeof(bytes));
//                 }
//                 break;

//             default:
//                 goto cmd_bad;
//             }

//             SET_BL_STATE(STATE_PROTO_GET_DEVICE);
//             break;

//         case PROTO_CHIP_ERASE:
//             /* expect EOC */
//             if (!wait_for_eoc(2))
//             {
//                 goto cmd_bad;
//             }
//             if ((bl_state & STATE_ALLOWS_ERASE) != STATE_ALLOWS_ERASE)
//             {
//                 goto cmd_bad;
//             }
//             erase_all();
//             address            = 0;
//             board_info.fw_size = (1024 - 64) * 1024;
//             SET_BL_STATE(STATE_PROTO_CHIP_ERASE);
//             break;

//         case PROTO_PROG_MULTI: // program bytes
//             // expect count
//             arg = cin_wait(50);

//             if (arg < 0)
//             {
//                 goto cmd_bad;
//             }

//             // sanity-check arguments
//             if (arg % 4)
//             {
//                 goto cmd_bad;
//             }

//             if ((address + arg) > board_info.fw_size)
//             {
//                 goto cmd_bad;
//             }

//             if ((unsigned int)arg > sizeof(flash_buffer))
//             {
//                 goto cmd_bad;
//             }

//             for (int i = 0; i < arg; i++)
//             {
//                 c = cin_wait(1000);

//                 if (c < 0)
//                 {
//                     goto cmd_bad;
//                 }

//                 flash_buffer[i] = c;
//             }

//             c = cin_wait(1000);
//             if (c < 0)
//             {
//                 goto cmd_bad;
//             }
//             if (crc8_calc(flash_buffer, arg) != c)
//             {
//                 goto cmd_bad;
//             }

//             if (!wait_for_eoc(200))
//             {
//                 goto cmd_bad;
//             }

//             write(address, flash_buffer, arg);
//             address += arg;
//             SET_BL_STATE(STATE_PROTO_PROG_MULTI);
//             break;

//         case PROTO_GET_CRC:
//             // expect EOC
//             if (!wait_for_eoc(2))
//             {
//                 goto cmd_bad;
//             }

//             // compute CRC of the programmed area
//             uint32_t sum = 0;
//             sum          = app_crc_check();
//             cout((uint8_t *)&sum, 4);
//             SET_BL_STATE(STATE_PROTO_GET_CRC);
//             break;

//         case PROTO_BOOT:
//             // expect EOC
//             if (!wait_for_eoc(1000))
//             {
//                 goto cmd_bad;
//             }
//             if (/* first_word == 0xffffffff ||*/ (bl_state & STATE_ALLOWS_REBOOT) != STATE_ALLOWS_REBOOT)
//             {
//                 goto cmd_bad;
//             }

//             if (!is_app_valid())
//             {
//                 goto cmd_fail;
//             }
//             sync_response();

//             /* 直接进入app */
//             boot_jump_to_app();
//             return 0;

//         default:
//             continue;
//         }

//         // send the sync response for this command
//         sync_response();
//         continue;

//     cmd_bad:
//         // send an 'invalid' response but don't kill the timeout - could be garbage
//         invalid_response();
//         bl_state = 0;
//         continue;

//     cmd_fail:
//         // send a 'command failed' response but don't kill the timeout - could be garbage
//         failure_response();
//         continue;
//     } while (0);

//     return 0;
// }