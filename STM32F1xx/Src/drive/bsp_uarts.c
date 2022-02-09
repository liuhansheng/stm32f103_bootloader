 #include "bsp_uart1.h"
 #include "bsp_uarts.h"
 void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
 {
     if (huart->Instance == USART1)
     {
         bsp_uart1_dma_tx_cplt_cb();
     }
 }

 void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
 {
     if (huart->Instance == USART1)
     {
         bsp_uart1_dma_rx_halt_cb();
     }
 }

 void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
 {
     if (huart->Instance == USART1)
     {
         bsp_uart1_dma_rx_cplt_cb();
     }
 }