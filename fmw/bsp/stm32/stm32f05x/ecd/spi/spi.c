#define EGL_MODULE_NAME "SPI"

#include <assert.h>
#include <string.h>
#include "stm32f0xx.h"
#include "egl_lib.h"
#include "ecd_bsp.h"

#define SPI              (SPI1)
#define PORT1            (GPIOB)
#define PORT2            (GPIOA)
#define SCK              (GPIO_Pin_3)
#define MISO             (GPIO_Pin_4)
#define MOSI             (GPIO_Pin_5)
#define CS               (GPIO_Pin_15)
#define CLOCK_A          (RCC_AHBPeriph_GPIOA)
#define CLOCK_B          (RCC_AHBPeriph_GPIOB)
#define CLOCK_SPI        (RCC_APB2Periph_SPI1)
#define SCK_AF           (GPIO_PinSource3)
#define MISO_AF          (GPIO_PinSource4)
#define MOSI_AF          (GPIO_PinSource5)
#define CS_AF            (GPIO_PinSource15)
#define BUFF_SIZE        (128)
#define IRQ_PRIORITY     (1)
#define DMA_RX           (DMA1_Channel2)
#define DMA_TX           (DMA1_Channel3)
#define CLOCK_DMA        (RCC_AHBPeriph_DMA1)
#define IRQ              (DMA1_Channel2_3_IRQn)
#define SPI_FIFO_SIZE    (4)
#define TX_CHUNKS_NUMBER (4)
#define RX_CHUNKS_NUMBER (4)

/* declare ring buffers */
EGL_DECLARE_RINGBUF(tx_rbuff, BUFF_SIZE);
EGL_DECLARE_RINGBUF(rx_rbuff, BUFF_SIZE);

EGL_DECLARE_CHUNKS(tx_chunks, TX_CHUNKS_NUMBER);
EGL_DECLARE_CHUNKS(rx_chunks, RX_CHUNKS_NUMBER);


static void gpio_init(void)
{
  static GPIO_InitTypeDef config = 
  {
    .GPIO_Pin   = SCK | MISO | MOSI,
    .GPIO_Mode  = GPIO_Mode_AF,
    .GPIO_OType = GPIO_OType_PP,
    .GPIO_PuPd  = GPIO_PuPd_DOWN,
    .GPIO_Speed = GPIO_Speed_50MHz,
  };  

  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(CLOCK_A, ENABLE);
  RCC_AHBPeriphClockCmd(CLOCK_B, ENABLE);

  /* SPI pin mappings */
  GPIO_PinAFConfig(PORT1, SCK_AF,  GPIO_AF_0);
  GPIO_PinAFConfig(PORT1, MISO_AF, GPIO_AF_0);
  GPIO_PinAFConfig(PORT1, MOSI_AF, GPIO_AF_0);
  GPIO_PinAFConfig(PORT2, CS_AF,   GPIO_AF_0);

  /* SPI SCK, MOSI, MISO pins configuration */
  GPIO_Init(PORT1, &config);
 
  /* SPI CS pin configuration */
  config.GPIO_Pin = CS;
  GPIO_Init(PORT2, &config);
}

static void spi_init(void)
{
  static const SPI_InitTypeDef config = 
  {
    .SPI_Direction         = SPI_Direction_2Lines_FullDuplex,
    .SPI_DataSize          = SPI_DataSize_8b,
    .SPI_CPOL              = SPI_CPOL_Low,
    .SPI_CPHA              = SPI_CPHA_1Edge,
    .SPI_NSS               = SPI_NSS_Hard,
    .SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4,
    .SPI_FirstBit          = SPI_FirstBit_MSB,
    .SPI_CRCPolynomial     = 7,
    .SPI_Mode              = SPI_Mode_Slave
  };

  RCC_APB2PeriphClockCmd(CLOCK_SPI, ENABLE);
    
  /* Initializes the SPI communication */
  SPI_Init(SPI, (SPI_InitTypeDef *) &config);
  SPI_RxFIFOThresholdConfig(SPI, SPI_RxFIFOThreshold_QF);
  SPI_I2S_DMACmd(SPI, SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx, ENABLE);
}

static void nvic_init(void)
{
  static const NVIC_InitTypeDef config = 
  {
    .NVIC_IRQChannel         = IRQ,
    .NVIC_IRQChannelPriority = IRQ_PRIORITY,
    .NVIC_IRQChannelCmd      = ENABLE
  };

  /* Configure DMA interrupt */
  NVIC_Init( (NVIC_InitTypeDef *) &config);
}

static void setup_dma_read(void *data, size_t len)
{
  /* config dma rx channel */
  static DMA_InitTypeDef config = 
  {
    .DMA_PeripheralBaseAddr = (uint32_t)(&SPI->DR),
    .DMA_DIR                = DMA_DIR_PeripheralSRC,
    .DMA_PeripheralInc      = DMA_PeripheralInc_Disable,
    .DMA_MemoryInc          = DMA_MemoryInc_Enable,
    .DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
    .DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte,
    .DMA_Mode               = DMA_Mode_Normal,
    .DMA_Priority           = DMA_Priority_VeryHigh,
    .DMA_M2M                = DMA_M2M_Disable,
  };  
 
  EGL_TRACE_INFO("DMA: Len: %d\r\n", len);

  if (len > 0)
  {
    /* check if dma is not in error state */
    assert(DMA_GetFlagStatus(DMA1_FLAG_TE2) == RESET);

    /* Set up buffer to transmission */
    config.DMA_MemoryBaseAddr = (uint32_t)data;
    config.DMA_BufferSize     = len;

    DMA_Cmd(DMA_RX, DISABLE);
    DMA_Init(DMA_RX, &config);
    DMA_Cmd(DMA_RX, ENABLE);
  }
}

void dma_init(void)
{
  RCC_AHBPeriphClockCmd(CLOCK_DMA, ENABLE);

  /* Enable DMA SPI TX Transfer complete interrupt */
  DMA_ITConfig(DMA_TX, DMA_IT_TC, ENABLE);

  /* TBD: currently commented, let's see if I need it, if not then I will remove it*/
  //DMA_ITConfig(DMA_RX, DMA_IT_TC, ENABLE);
}

static void init(void)
{
  static char tx_buffer[BUFF_SIZE] = {0};
  static char rx_buffer[BUFF_SIZE] = {0};

  gpio_init();
  spi_init();
  dma_init();
  nvic_init();

  assert(egl_chunk_init(&tx_chunks, tx_buffer, sizeof(tx_buffer)) == EGL_SUCCESS);
  assert(egl_chunk_init(&rx_chunks, rx_buffer, sizeof(rx_buffer)) == EGL_SUCCESS);

}

static egl_result_t open(void)
{
  SPI_Cmd(SPI, ENABLE);

  /* start reading data trough dma */
  setup_dma_read(egl_ringbuf_get_in_ptr(&rx_rbuff),
                 egl_ringbuf_get_cont_free_size(&rx_rbuff));

  return EGL_SUCCESS;
}

static egl_result_t setup_dma_write(void *data, size_t len)
{
  /* config dma tx channel */
  static DMA_InitTypeDef config = 
  {
    .DMA_PeripheralBaseAddr = (uint32_t)(&SPI->DR),
    .DMA_DIR                = DMA_DIR_PeripheralDST,
    .DMA_PeripheralInc      = DMA_PeripheralInc_Disable,
    .DMA_MemoryInc          = DMA_MemoryInc_Enable,
    .DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
    .DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte,
    .DMA_Mode               = DMA_Mode_Normal,
    .DMA_Priority           = DMA_Priority_VeryHigh,
    .DMA_M2M                = DMA_M2M_Disable,
  };

  assert(data);

  if( len < SPI_FIFO_SIZE )
  {
    return EGL_INVALID_PARAM;
  }

  /* check if dma in error state */
  assert(DMA_GetFlagStatus(DMA1_FLAG_TE3) == RESET);

  /* Set up dma transmission */
  config.DMA_MemoryBaseAddr = (uint32_t)data;
  config.DMA_BufferSize     = len;

  /* Start dma transmission */
  DMA_Cmd(DMA_TX, DISABLE);
  DMA_Init(DMA_TX, &config);

  /* Wait till current data transfer will be finished */
  while(SPI_I2S_GetFlagStatus(SPI, SPI_I2S_FLAG_BSY) == SET)
  {
    /* Do nothing */
  }

  DMA_Cmd(DMA_TX, ENABLE);

  return EGL_SUCCESS;
}

static size_t write(void *data, size_t len)
{
  egl_result_t result = egl_chunk_write(&tx_chunks, data, len);
  if(result != EGL_SUCCESS)
  {
    EGL_TRACE_ERROR("Fail to write to chunk. Result %s", EGL_RESULT());
    return 0;
  }

  /* If DMA transmission not started, then srart it */
  if(DMA_GetCurrDataCounter(DMA_TX) == 0)
  {
    egl_chunk_t *chunk = egl_chunk_in_previous_get(&tx_chunks);
    setup_dma_write(chunk->buf, chunk->size);
  }

  /* Notify that board has new data */
  egl_pio_set(int1(), true);  

  return len;
}

// static size_t write(void *data, size_t len)
// {
//   size_t cont_full_size = 0;

//   // egl_pio_set(int2(), true);

//   len = egl_ringbuf_write(&tx_rbuff, data, len);

//   /* If DMA transmission not started, then srart it */
//   if(DMA_GetCurrDataCounter(DMA_TX) == 0)
//   {
//     cont_full_size = egl_ringbuf_get_cont_full_size(&tx_rbuff);

//     if(EGL_SUCCESS == setup_dma_write(egl_ringbuf_get_out_ptr(&tx_rbuff), cont_full_size))
//     {
//       assert(cont_full_size == egl_ringbuf_reserve_for_read(&tx_rbuff, cont_full_size));
//     }
//   }

//   if(len > 0)
//   {
//     /* Notify that board has new data */
//     egl_pio_set(int1(), true);  
//   }

//   // egl_pio_set(int2(), false);

//   return len;
// }

static size_t read(void* data, size_t len)
{
  /* Get amount of bytes that came to buffer */ 
  uint16_t counter = DMA_GetCurrDataCounter(DMA_RX);
  size_t free      = egl_ringbuf_get_cont_free_size(&rx_rbuff);
  size_t recived   = 0;
  size_t full      = 0;
  
  /*
    Check CS pin, if it is in low state then transmission in process 
    and we should wait till current transmission will be finished 
  */
  while(GPIO_ReadInputDataBit(PORT2, CS) == false)
  {
    /* Do nothing */
  }

  /* Check that we got from DMA not more then free buffer size */
  assert(free >= counter);

  /* Calculate how many bytes we have revived fromm DMA */
  recived = free - counter;

  /* If come data has been read trough dma, restart dma reading once again */
  if(recived > 0 || egl_ringbuf_is_empty(&rx_rbuff) == false)
  {
    /* Mark data which we recive form dma as written */
    assert(egl_ringbuf_reserve_for_write(&rx_rbuff, recived) == recived);

    /* Get number of fulled bytes in ringbuffer */ 
    full = egl_ringbuf_get_full_size(&rx_rbuff);

    /* If requested number of bytes more then buffer has, then truncate it */
    len = len > full ? full : len;

    /* Then read data from buffer */
    len = egl_ringbuf_read(&rx_rbuff, data, len);

    /* If buffer is empty, reset buffer. It will give more continioua buffer space to write */
    if(egl_ringbuf_is_empty(&rx_rbuff) == true)
    {
      egl_ringbuf_reset(&rx_rbuff);
    }

    /* And set up new DMA transfer with free space */
    setup_dma_read(egl_ringbuf_get_in_ptr(&rx_rbuff),
                   egl_ringbuf_get_cont_free_size(&rx_rbuff));
  }
  else
  {
    len = 0;
  }

  return len;
}

static egl_result_t close(void)
{
  SPI_Cmd(SPI,    DISABLE);
  DMA_Cmd(DMA_TX, DISABLE);
  DMA_Cmd(DMA_RX, DISABLE);

  return EGL_SUCCESS;
}

// void spi_dma_tx_irq(void)
// {
//   size_t cont_full_size = egl_ringbuf_get_cont_full_size(&tx_rbuff);
//   //size_t full_len = egl_ringbuf_get_full_size(&tx_rbuff);

//   //EGL_TRACE_DEBUG("Write len: %d, full size: %d\r\n", write_len, full_len);
//   /* If we have something more to transmit,
//      Thent setup new DMA transfer to write */
//   if(cont_full_size > 0)
//   {
//     egl_pio_set(int2(), true);
//     if(EGL_SUCCESS == setup_dma_write(egl_ringbuf_get_out_ptr(&tx_rbuff), cont_full_size))
//     {
//       assert(cont_full_size == egl_ringbuf_reserve_for_read(&tx_rbuff, cont_full_size));
//     }
//     egl_pio_set(int2(), false);
//     //EGL_TRACE_DEBUG("Reserved: %d, Left: %d\r\n", write_len, egl_ringbuf_get_cont_full_size(&tx_rbuff));
//   }
//   /* Else notify that transmission has been finished */
//   else
//   {
//     egl_ringbuf_reset(&tx_rbuff);
//     egl_pio_set(int1(), false);
//     DMA_Cmd(DMA_TX, DISABLE);
//   }
// }

void spi_dma_tx_irq(void)
{
  /* Clear current chunk */
  egl_chunk_t *chunk = egl_chunk_out_current_get(&tx_chunks);
  chunk->size = 0;

  /* Check next chunk */
  chunk = egl_chunk_out_next_get(&tx_chunks);

  /*If it contains some data then set up new DMA transmission */
  if(chunk->size != 0)
  {
    setup_dma_write(chunk->buf, chunk->size);
  }
  /* Else notify that transmission has been finished */
  else
  {
    egl_pio_set(int1(), false);
    DMA_Cmd(DMA_TX, DISABLE);
  }

  /* Increment chunk number */
  egl_chunk_out_index_inc(&tx_chunks);
}

void spi_dma_rx_irq(void)
{
  setup_dma_read(egl_ringbuf_get_in_ptr(&rx_rbuff),
                 egl_ringbuf_get_cont_free_size(&rx_rbuff));
}

static egl_interface_t spi_impl = 
{
  .init   = init,
  .open   = open,
  .write  = write,
  .ioctl  = NULL,
  .read   = read,
  .close  = close,
  .deinit = NULL
};

egl_interface_t *spi(void)
{
  return &spi_impl;
}
