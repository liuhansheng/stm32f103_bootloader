
#include "main.h"
#include "stm32f1xx_it.h"
#include "bsp_uart1.h"

void NMI_Handler(void)
{

  while (1)
  {
  }
}


void HardFault_Handler(void)
{

  while (1)
  {

  }
}


void MemManage_Handler(void)
{

  while (1)
  {

  }
}

void BusFault_Handler(void)
{

  while (1)
  {

  }
}

void UsageFault_Handler(void)
{
  while (1)
  {

  }
}


void SVC_Handler(void)
{
}


void DebugMon_Handler(void)
{

}


 void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{

  HAL_IncTick();
}

void DMA1_Channel6_IRQHandler(void)
{
    // HAL_DMA_IRQHandler(huart2.hdmarx);
}

void DMA1_Channel7_IRQHandler(void)
{
    // HAL_DMA_IRQHandler(huart2.hdmatx);
}
/******************************************************************************/
/******************************************************************************/
void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);

    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET)
    {
       /** Clear idle line interrupt flag. */
       __HAL_UART_CLEAR_IDLEFLAG(&huart1);

       /** 闲时函数回调 */
       bsp_uart1_rx_idle_cb();
    }
}

void DMA1_Channel5_IRQHandler(void)
{
    HAL_DMA_IRQHandler(huart1.hdmarx);
}

void DMA1_Channel4_IRQHandler(void)
{
    HAL_DMA_IRQHandler(huart1.hdmatx);
}
/******************************************************************************/
/******************************************************************************/