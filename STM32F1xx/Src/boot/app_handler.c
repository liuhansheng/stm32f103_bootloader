#include "app_handler.h"
#include "bsp_crc32.h"

#define __IO volatile
typedef void (*pFunction)(void);

pFunction Jump_To_Application;
uint32_t  JumpAddress;

void boot_jump_to_app(void)
{
  if ((*(__IO uint32_t *)APP_START_ADDRESS) != FLASH_DEFINE_NUM)
  {
      SCB->VTOR    = APP_START_ADDRESS;
      //BACK_RAM_RES = 0;
      HAL_DeInit();
      HAL_RCC_DeInit();

      // 关闭所有中�
      for (int i = NonMaskableInt_IRQn; i <= DMA2_Channel4_5_IRQn; i++)
      {
          //HAL_NVIC_DisableIRQ(i);
          //HAL_NVIC_ClearPendingIRQ(i);
      }

       // 需复位systick才能正常跳转
      {
          SysTick->LOAD = 0;
          SysTick->VAL  = 0;
          SysTick->CTRL = 0;
      }

      {
          __set_BASEPRI(0);
          __set_FAULTMASK(0);
          __set_CONTROL(0);
          __set_PSP(*((volatile unsigned long int *)APP_START_ADDRESS));
          __set_MSP(*(__IO uint32_t *)APP_START_ADDRESS);
          __ISB();
      }

      {
          __disable_irq();

          /* Jump to user application */
          JumpAddress         = *(__IO uint32_t *)(APP_START_ADDRESS + 4);
          Jump_To_Application = (pFunction)JumpAddress;

          /* Initialize user application's Stack Pointer */
          Jump_To_Application();
      }
  }
}

bool judge_app_jump(void)
{
  if (BACK_RAM_RES == MAGIC_NUM_UAVCAN_BOOT || BACK_RAM_RES == MAGIC_NUM_UART_BOOT)
  {
      return false;
  }

  // 不通过上位机直接下载时, 直接仿真时会出现这种情况
  if (APP_INFO->magic == FLASH_DEFINE_NUM && APP_START_ADDRESS_ARRAY(0) != FLASH_DEFINE_NUM)
  {
      return true;
  }

  if (APP_INFO->app_size == 0 || APP_INFO->app_size == FLASH_DEFINE_NUM)
  {
      return false;
  }

  // 程序是否完整
  if ((APP_INFO->app_crc != FLASH_DEFINE_NUM) && (APP_INFO->app_crc == APP_START_ADDRESS_ARRAY(APP_INFO->app_size)))
  {
      // 检查程序是否可以跳�
      return true;
  }

  return false;
}

/** 检查APP CRC 是否正确 */
int app_crc_check(void)
{
  const uint32_t crc = bsp_crc32_check((uint32_t *)APP_START_ADDRESS, (APP_INFO->app_size + 4) / 4);
  if ((crc == 0) && (APP_INFO->app_crc == APP_START_ADDRESS_ARRAY(APP_INFO->app_size)))
  {
      return 0;
  }

  return -1;
}

volatile uint32_t rcc_csr_bk_data __attribute__((section(".noinit")));

void rcc_csr_backup(void)
{
  rcc_csr_bk_data = RCC->CSR;
  RCC->CSR |= RCC_CSR_RMVF;
}

uint32_t get_boot_src(void)
{
  return rcc_csr_bk_data;
}

static bool is_app_to_boot(void)
{
  return (((rcc_csr_bk_data & RCC_CSR_SFTRSTF) == RCC_CSR_SFTRSTF) && ((rcc_csr_bk_data & RCC_CSR_PORRSTF) != RCC_CSR_PORRSTF));
}

void boot_over_time_jump(void)
{
  if (!is_app_to_boot())
  {
      if (HAL_GetTick() > BOOT_COMM_OVER_TIME)
      {
          if (judge_app_jump())
          {
              // jump app
              boot_jump_to_app();
          }
      }
  }
}