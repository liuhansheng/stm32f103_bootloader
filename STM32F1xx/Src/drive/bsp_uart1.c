#include "bsp_uart1.h"
#include "lwrb/lwrb.h"
UART_HandleTypeDef huart1;
static uint8_t uart1_tx_buff_data[256];
static uint8_t uart1_rx_dma_buffer[256];
static lwrb_t usart_tx_buff;
static size_t usart_dma_tx_len;
void uart_config(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart1);
    /** TX send finish */
    __HAL_DMA_ENABLE_IT(huart1.hdmatx, DMA_IT_TC);


    /** RX send IT */
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    __HAL_DMA_ENABLE_IT(huart1.hdmarx, DMA_IT_TC);
    __HAL_DMA_ENABLE_IT(huart1.hdmarx, DMA_IT_HT);
    HAL_UART_Receive_DMA(&huart1, uart1_rx_dma_buffer, sizeof(uart1_rx_dma_buffer));
}
void bsp_uart1_init(void)
{
    lwrb_init(&usart_tx_buff, uart1_tx_buff_data, sizeof(uart1_tx_buff_data));
    uart_config();
}


static usart_rx_handler_cb usart_rx_handler;
void bsp_uart1_install_rx_install(usart_rx_handler_cb cb)
{
      usart_rx_handler = cb;
}
static void usart_process_data(const uint8_t *data, size_t len)
{
    const uint8_t *d = data;

    for (; len > 0; --len, ++d)
    {
        if (usart_rx_handler)
        {
            (*usart_rx_handler)(*d);
        }
    }
}
static void usart_rx_check(void)
{
    static size_t old_pos;

    /* Calculate current position in buffer */
    const size_t pos = ARRAY_LEN(uart1_rx_dma_buffer) - __HAL_DMA_GET_COUNTER(huart1.hdmarx);

    if (pos != old_pos)
    {
        /* Check change in received data */
        if (pos > old_pos)
        {
            /* Current position is over previous one */
            /* We are in "linear" mode */
            /* Process data directly by subtracting "pointers" */
            usart_process_data(&uart1_rx_dma_buffer[old_pos], pos - old_pos);
        }
        else
        {
            /* We are in "overflow" mode */
            /* First process data to the end of buffer */
            usart_process_data(&uart1_rx_dma_buffer[old_pos], ARRAY_LEN(uart1_rx_dma_buffer) - old_pos);

            /* Check and continue with beginning of buffer */
            if (pos > 0)
            {
                usart_process_data(&uart1_rx_dma_buffer[0], pos);
            }
        }
    }

    old_pos = pos; /* Save current position as old */
}
void bsp_uart1_rx_entry(void)
{
    usart_rx_check();
    HAL_UART_Receive_DMA(&huart1, uart1_rx_dma_buffer, sizeof(uart1_rx_dma_buffer));
}
void usart_start_tx_dma(void)
{
    if (usart_dma_tx_len > 0)
    {
        return;
    }
    //__disable_irq();
    if (usart_dma_tx_len == 0 && (usart_dma_tx_len = lwrb_get_linear_block_read_length(&usart_tx_buff)) > 0)
    {
        /* Limit maximal size to transmit at a time */
        if (usart_dma_tx_len > 128)
        {
            usart_dma_tx_len = 128;
        }

        /* Clear all flags */
        __HAL_DMA_CLEAR_FLAG(huart1.hdmatx, __HAL_DMA_GET_TC_FLAG_INDEX(&huart1));
        __HAL_DMA_CLEAR_FLAG(huart1.hdmatx, __HAL_DMA_GET_HT_FLAG_INDEX(&huart1));
        __HAL_DMA_CLEAR_FLAG(huart1.hdmatx, __HAL_DMA_GET_GI_FLAG_INDEX(&huart1));
        __HAL_DMA_CLEAR_FLAG(huart1.hdmatx, __HAL_DMA_GET_TE_FLAG_INDEX(&huart1));

        /* Configure DMA */
        HAL_UART_Transmit_DMA(&huart1, (uint8_t *)lwrb_get_linear_block_read_address(&usart_tx_buff), usart_dma_tx_len);
    }
    //__enable_irq();
}
uint8_t bsp_uart1_send(uint8_t *data, uint16_t len)
{
    size_t length = 0;
    if(lwrb_get_free(&usart_tx_buff) >= len)
    {
        length = lwrb_write(&usart_tx_buff,data,len);
        usart_start_tx_dma();
    }
    return length;
}

void bsp_uart1_dma_tx_cplt_cb(void)
{
    /* Data sent, ignore these */
    lwrb_skip(&usart_tx_buff, usart_dma_tx_len);

    usart_dma_tx_len = 0;

    /* Try to send more data */
    usart_start_tx_dma();
}

void bsp_uart1_dma_rx_halt_cb(void)
{
    bsp_uart1_rx_entry();
}
void bsp_uart1_dma_rx_cplt_cb(void)
{
     bsp_uart1_rx_entry();
}
void bsp_uart1_rx_idle_cb(void)
{
     bsp_uart1_rx_entry();
}
