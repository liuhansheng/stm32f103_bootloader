#pragma once

#include "stdint.h"

void     bsp_crc32_init(void);
uint32_t bsp_crc32_check(uint32_t *data, uint32_t len);