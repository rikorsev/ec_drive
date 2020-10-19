#include <assert.h>
#include "stm32f0xx.h"
#include "egl_lib.h"
#include "dbg.h"

#define USART            (USART2)
#define IRQ_USART        (USART2_IRQn)
#define PORT             (GPIOA)
#define TX               (GPIO_Pin_2)
#define TX_AF            (GPIO_PinSource2)
#define DEFAULT_SPEED    (115200)
#define BUFF_SIZE        (1024)
#define CLOCK_GPIO       (RCC_AHBPeriph_GPIOA)
#define CLOCK_UART       (RCC_APB1Periph_USART2)
#define IRQ_PRIORITY     (2)
#define TX_CHUNKS_NUMBER (16)
#define CLOCK_DMA        (RCC_AHBPeriph_DMA1)
#define DMA_TX           (DMA1_Channel4)
#define IRQ_DMA          (DMA1_Channel4_5_IRQn)

EGL_DECLARE_RINGBUF(tx_buff, BUFF_SIZE);

EGL_DECLARE_CHUNKS(tx_chunks, TX_CHUNKS_NUMBER);

static void init_gpio(void)
{
  static const GPIO_InitTypeDef config = 
  {
    .GPIO_Pin   = TX,
    .GPIO_Mode  = GPIO_Mode_AF,
    .GPIO_Speed = GPIO_Speed_10MHz,
    .GPIO_OType = GPIO_OType_PP,
    .GPIO_PuPd  = GPIO_PuPd_UP
  };
  
  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(CLOCK_GPIO, ENABLE);

  /* Connect PXx to USARTx_Tx */
  GPIO_PinAFConfig(PORT, TX_AF, GPIO_AF_1);
  
  /* Configure USART Tx as alternate function push-pull */
  GPIO_Init(PORT, (GPIO_InitTypeDef *) &config);
}

static void init_uart(void)
{
  /* USARTx configured as follow:
  - BaudRate    = 115200 baud  
  - Word Length = 8 Bits
  - Stop Bit    = 1 Stop Bit
  - Parity      = No Parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  */

  static const USART_InitTypeDef config = 
  {
    .USART_BaudRate            = DEFAULT_SPEED,
    .USART_WordLength          = USART_WordLength_8b,
    .USART_StopBits            = USART_StopBits_1,
    .USART_Parity              = USART_Parity_No,
    .USART_HardwareFlowControl = USART_HardwareFlowControl_None,
    .USART_Mode                = USART_Mode_Rx | USART_Mode_Tx
  };

  /* Enable USART clock */
  RCC_APB1PeriphClockCmd(CLOCK_UART, ENABLE); 

  /* USART configuration */
  USART_Init(USART, (USART_InitTypeDef *) &config);

  /* Disable DMA */
  USART_DMACmd(USART, USART_DMAReq_Tx, ENABLE);
}

static void init_irq(void)
{
  NVIC_InitTypeDef config = 
  {
    .NVIC_IRQChannel         = IRQ_USART,
    .NVIC_IRQChannelPriority = IRQ_PRIORITY,
    .NVIC_IRQChannelCmd      = ENABLE
  };
  
  /* Enable the USART Interrupt */
  NVIC_Init(&config);

  config.NVIC_IRQChannel = IRQ_DMA;

  /* Enable the DMA Interrupt */
  NVIC_Init(&config);
}

static void init_dma(void)
{
  RCC_AHBPeriphClockCmd(CLOCK_DMA, ENABLE);

  /* Enable DMA SPI TX Transfer complete interrupt */
  DMA_ITConfig(DMA_TX, DMA_IT_TC, ENABLE);
}

static void init(void)
{
  static char tx_buffer[BUFF_SIZE] = {0};

  init_gpio();
  init_uart();
  init_dma();
  init_irq();

  /* Init chunks */
  assert(egl_chunk_init(&tx_chunks, tx_buffer, sizeof(tx_buffer)) == EGL_SUCCESS);
}

static egl_result_t open(void)
{
  /* Enable USART */
  USART_Cmd(USART, ENABLE);


  return EGL_SUCCESS;
}

static size_t write_polling(void* buff, size_t len)
{
  int i;
  uint8_t* data_ptr = (uint8_t *)buff;
  uint8_t data = 0;

  /* flush data from ringbuffer if any */
  while(egl_ringbuf_get_fill_size(&tx_buff) > 0)
  {
    (void)egl_ringbuf_read(&tx_buff, &data, 1);

    while(USART_GetFlagStatus(USART, USART_FLAG_TXE) != SET)
    {
      /* Do nothing */
    }

    USART_SendData(USART, (uint16_t)data);
  }
  
  for(i = 0; i < len; i++)
  {    
    
    while(USART_GetFlagStatus(USART, USART_FLAG_TXE) != SET)
    {
      /* Do nothing */
    }

    USART_SendData(USART, (uint16_t)(data_ptr[i]));
  }
  
  return len;
}

static size_t write_interrupt(void* buff, size_t len)
{
  len = egl_ringbuf_write(&tx_buff, buff, len);
  uint8_t data = 0;
   
  if(USART_GetFlagStatus(USART, USART_FLAG_TC) == SET && egl_ringbuf_is_empty(&tx_buff) == false)
  {
    USART_ITConfig(USART, USART_IT_TXE, ENABLE);

    (void)egl_ringbuf_read(&tx_buff, &data, 1);
    USART_SendData(USART, (uint16_t)data);
  }
  
  return len;
}

static void setup_dma_tx(void *data, size_t len)
{
  /* config dma tx channel */
  static DMA_InitTypeDef config = 
  {
    .DMA_PeripheralBaseAddr = (uint32_t)(&USART->TDR),
    .DMA_DIR                = DMA_DIR_PeripheralDST,
    .DMA_PeripheralInc      = DMA_PeripheralInc_Disable,
    .DMA_MemoryInc          = DMA_MemoryInc_Enable,
    .DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
    .DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte,
    .DMA_Mode               = DMA_Mode_Normal,
    .DMA_Priority           = DMA_Priority_High,
    .DMA_M2M                = DMA_M2M_Disable,
  };

  /* check if dma in error state */
  assert(DMA_GetFlagStatus(DMA1_FLAG_TE4) == RESET);

  /* Set up dma transmission */
  config.DMA_MemoryBaseAddr = (uint32_t)data;
  config.DMA_BufferSize     = len;

  /* Start dma transmission */
  DMA_Cmd(DMA_TX, DISABLE);
  DMA_Init(DMA_TX, &config);
  DMA_Cmd(DMA_TX, ENABLE);
}

static size_t write_dma(void* buff, size_t len)
{

  /* Truncete data if necessary */
  len = len > tx_chunks.size ? tx_chunks.size : len;

  /* Write data to tx chunks */
  egl_result_t result = egl_chunk_write(&tx_chunks, buff, len);
  if(result != EGL_SUCCESS)
  {
    return 0;
  }

  /* If DMA transmission not started, then srart it */
  if(DMA_GetCurrDataCounter(DMA_TX) == 0)
  {
    egl_chunk_t *chunk = egl_chunk_in_previous_get(&tx_chunks);

    assert(chunk != NULL);

    setup_dma_tx(chunk->buf, chunk->size);
  }

  return len;
}

static egl_result_t ioctl(uint8_t opcode, void* data, size_t len)
{
  egl_result_t result = EGL_SUCCESS;

  switch(opcode)
  { 
    case DBG_WRITE_INTERRUPT_IOCTL:
      dbg()->write = write_interrupt;
      break;
      
    case DBG_WRITE_POLLING_IOCTL:
      dbg()->write = write_polling;
      break;

    case DBG_WRITE_DMA_IOCTL:
      dbg()->write = write_dma;
      break;

    default:
      result = EGL_NOT_SUPPORTED;
  }

  return result;
}

static egl_result_t close(void)
{
  /* Disable USART */
  USART_Cmd(USART, DISABLE);

  return EGL_SUCCESS;
}

static void deinit(void)
{
  NVIC_InitTypeDef  config;
  
  /* Disable USART clock */
  RCC_APB1PeriphClockCmd(CLOCK_UART, DISABLE); 

  /* Disable the USART Interrupt */
  config.NVIC_IRQChannel             = IRQ_USART;
  config.NVIC_IRQChannelPriority     = 0;
  config.NVIC_IRQChannelCmd          = DISABLE;
  NVIC_Init(&config);

  /* Disable the DMA Interrupt */
  config.NVIC_IRQChannel             = IRQ_DMA;
  NVIC_Init(&config);

  /* Disable DMA */
  USART_DMACmd(USART, USART_DMAReq_Tx, DISABLE);
}

void dbg_irq(void)
{
  uint8_t data = 0;

  /* if ringbuffer not empty */ 
  if(egl_ringbuf_is_empty(&tx_buff) == false)
  {
    (void)egl_ringbuf_read(&tx_buff, &data, 1);
    USART_SendData(USART, (uint16_t)data);
  }
  else
  {
    /* Disable interrupt at the end of transmission */
    USART_ITConfig(USART, USART_IT_TXE, DISABLE);
  }
}

void dbg_dma_irq(void)
{
  egl_chunk_t *chunk = egl_chunk_out_current_get(&tx_chunks);

  assert(chunk != NULL);

  /* Clear current chunk */
  chunk->size = 0;

  /* Check next chunk */
  chunk = egl_chunk_out_next_get(&tx_chunks);

  assert(chunk != NULL);

  /*If it contains some data then set up new DMA transmission */
  if(chunk->size != 0)
  {
    setup_dma_tx(chunk->buf, chunk->size);
  }
  /* Else notify that transmission has been finished */
  else
  {
    DMA_Cmd(DMA_TX, DISABLE);
  }

  /* Increment chunk number */
  egl_chunk_out_index_inc(&tx_chunks);
}


static egl_interface_t dbg_impl = 
{
  .init   = init,
  .open   = open,
  .write  = write_interrupt,
  .ioctl  = ioctl,
  .read   = NULL,
  .close  = close,
  .deinit = deinit
};

egl_interface_t *dbg(void)
{
  return &dbg_impl;
}

int _write(int file, char *ptr, int len)
{
  size_t to_write = len;
  size_t offset = 0;

  switch (file)
  {
    case 1: /* stdout */
    case 2: /* stderr */
      //while(offset < len)
      //{
        to_write = len - offset;
        assert(egl_itf_write(dbg(), ptr + offset, (size_t *)&to_write) == EGL_SUCCESS);
        offset += to_write;
      //}
    break;
    
    default:
      return -1;
  }

  return len;
}
