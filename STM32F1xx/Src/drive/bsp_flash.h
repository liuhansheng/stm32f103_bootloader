#pragma once 
 #include "bsp_flash.h"

 void bsp_flash_init(void);
 void bsp_flash_erase(uint32_t base_addr, uint32_t size);
 void bsp_flash_write_bytes(uint32_t offfset, const uint8_t *ptr, uint32_t len);