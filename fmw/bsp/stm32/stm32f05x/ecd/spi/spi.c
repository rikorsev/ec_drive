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
#define BUFF_SIZE        (128)
#define DMA_RX           (DMA1_Channel2)
#define DMA_TX           (DMA1_Channel3)

/* declare ring buffers */
EGL_DECLARE_RINGBUF(tx_rbuff, BUFF_SIZE);
EGL_DECLARE_RINGBUF(rx_rbuff, BUFF_SIZE);

uint8_t dma_tx_buffer[BUFF_SIZE] = {0};

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
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

  /* SPI pin mappings */
  GPIO_PinAFConfig(PORT1, GPIO_PinSource3, GPIO_AF_0);
  GPIO_PinAFConfig(PORT1, GPIO_PinSource4, GPIO_AF_0);
  GPIO_PinAFConfig(PORT1, GPIO_PinSource5, GPIO_AF_0);
  GPIO_PinAFConfig(PORT2, GPIO_PinSource15,   GPIO_AF_0);

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

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    
  /* Initializes the SPI communication */
  SPI_Init(SPI, (SPI_InitTypeDef *) &config);
  SPI_RxFIFOThresholdConfig(SPI, SPI_RxFIFOThreshold_QF);
  SPI_I2S_DMACmd(SPI, SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx, ENABLE);
}

static void nvic_init(void)
{
  /* Configure DMA interrupt */
  NVIC_InitTypeDef config = 
  {
    .NVIC_IRQChannel         = DMA1_Channel2_3_IRQn,
    .NVIC_IRQChannelPriority = 1,
    .NVIC_IRQChannelCmd      = ENABLE
  };

  NVIC_Init(&config);

  /* Config EXTI interrupt */
  config.NVIC_IRQChannel         = EXTI4_15_IRQn;
  config.NVIC_IRQChannelPriority = 2;

  NVIC_Init(&config);
}

static void exti_init(void)
{
  static const EXTI_InitTypeDef config = 
  {
    .EXTI_Line    = CS,
    .EXTI_Mode    = EXTI_Mode_Interrupt,
    .EXTI_Trigger = EXTI_Trigger_Rising,
    .EXTI_LineCmd = ENABLE
  };

  EXTI_Init(&config);
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
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  /* Enable DMA SPI TX Transfer complete interrupt */
  DMA_ITConfig(DMA_TX, DMA_IT_TC, ENABLE);
}

static void init(void)
{
  gpio_init();
  spi_init();
  dma_init();
  nvic_init();
  exti_init();
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

  /* check if dma in error state */
  assert(DMA_GetFlagStatus(DMA1_FLAG_TE3) == RESET);

  /* Set up dma transmission */
  config.DMA_MemoryBaseAddr = (uint32_t)data;
  config.DMA_BufferSize     = len;

  /* Start dma transmission */
  DMA_Cmd(DMA_TX, DISABLE);
  DMA_Init(DMA_TX, &config);

  /* Wait till the end of current transmisstion */ 
  while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == 0)
  {
    /* Do nothing */
  }

  /* Start DMA */
  DMA_Cmd(DMA_TX, ENABLE);

  return EGL_SUCCESS;
}

static size_t write(void *data, size_t data_len)
{
  /* If DMA transmission not started, then srart it */
  if(DMA_GetCurrDataCounter(DMA_TX) == 0)
  {
    data_len = MIN(BUFF_SIZE, data_len);
    memcpy(dma_tx_buffer, data, data_len);
    assert(EGL_SUCCESS == setup_dma_write(dma_tx_buffer, data_len));
  }
  else
  { 
    data_len = egl_ringbuf_write(&tx_rbuff, data, data_len);
  }
    
  if(data_len > 0)
  {
    /* Notify that controller has new data */
    egl_pio_set(int1(), true);
  }

  return data_len;
}

static size_t read(void *data, size_t len)
{
  /* Get amount of bytes that came to buffer */ 
  uint16_t counter = DMA_GetCurrDataCounter(DMA_RX);
  size_t free      = egl_ringbuf_get_cont_free_size(&rx_rbuff);
  size_t recived   = 0;
  size_t fill      = 0;
  
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

    /* Get number of filled bytes in ringbuffer */ 
    fill = egl_ringbuf_get_fill_size(&rx_rbuff);

    /* If requested number of bytes more then buffer has, then truncate it */
    len = len > fill ? fill : len;

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

void spi_dma_tx_irq(void)
{
  size_t fill = egl_ringbuf_get_fill_size(&tx_rbuff);
  
  assert(DMA_GetCurrDataCounter(DMA_TX) == 0);

  if(fill > 0)
  {
    size_t write_len = egl_ringbuf_read(&tx_rbuff, dma_tx_buffer, fill);

    assert(EGL_SUCCESS == setup_dma_write(dma_tx_buffer, write_len));
  }
  else
  {
    egl_pio_set(int1(), false);
    DMA_Cmd(DMA_TX, DISABLE);
  }
}

void spi_exti_irq(void)
{
  /* Start DMA */
  DMA_Cmd(DMA_TX, ENABLE);
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
