#pragma once
#include "main.h"
#include "bsp_uarts.h"
extern  UART_HandleTypeDef huart1;
void bsp_uart1_init(void);

void bsp_uart1_dma_tx_cplt_cb(void);
bool bsp_get_uart_dma_tx_cplt_flag(void);

void bsp_uart1_dma_rx_halt_cb(void);
void bsp_uart1_dma_rx_cplt_cb(void);
void bsp_uart1_rx_idle_cb(void);
int  uart_buff_read(void *data, uint32_t len);

uint8_t bsp_uart1_send(uint8_t *data, uint16_t len);