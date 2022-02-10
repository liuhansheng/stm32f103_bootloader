#include "main.h"
#include "bsp_crc32.h"
static CRC_HandleTypeDef hcrc32;

void bsp_crc32_init(void)
{
    __HAL_RCC_CRC_CLK_ENABLE();

    hcrc32.Instance = CRC;
    HAL_CRC_Init(&hcrc32);
}

uint32_t bsp_crc32_check(uint32_t *data, uint32_t len)
{
    return HAL_CRC_Calculate(&hcrc32, data, len);
}