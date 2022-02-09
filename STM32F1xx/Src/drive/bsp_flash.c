#include "main.h"
#include "bsp_flash.h"
#define PAGE_ADDR(x) (0x08000000 + 0x800 * (x))

static uint32_t flash_page_addr[256];
void bsp_flash_init(void)
{
  for (uint16_t i = 0;i < 256 ; i++)
  {
      flash_page_addr[i] = PAGE_ADDR(i);
  }
}
void bsp_flash_erase(uint32_t base_addr, uint32_t size)
{
    int8_t page_num = -1, Nbpage = 0;

    for (uint8_t i = 0; i < ARRAY_LEN(flash_page_addr) - 1; i++)
    {
        if (page_num < 0 && base_addr >= flash_page_addr[i] && base_addr < flash_page_addr[i + 1])
        {
            page_num = i;
        }

        if (page_num > 0 && page_num < ARRAY_LEN(flash_page_addr))
        {
            if (flash_page_addr[i] - flash_page_addr[page_num] < size)
            {
                Nbpage++;
            }
        }
    }

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef flash_erase;
    flash_erase.TypeErase    = FLASH_TYPEERASE_PAGES;
    flash_erase.NbPages      = Nbpage;
    flash_erase.PageAddress  = flash_page_addr[page_num];
    flash_erase.Banks        = FLASH_BANK_1;

    // 调用擦除函数
    uint32_t PageError = 0;
    HAL_FLASHEx_Erase(&flash_erase, &PageError);

    HAL_FLASH_Lock();
}

void bsp_flash_write_bytes(uint32_t offfset, const uint8_t *ptr, uint32_t len)
{
    HAL_FLASH_Unlock();

    for (uint32_t i = 0; i < len / 4; i++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, offfset + (i), ptr[i]);
    }

    HAL_FLASH_Lock();
}