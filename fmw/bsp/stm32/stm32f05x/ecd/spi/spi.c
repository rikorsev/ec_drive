#define EGL_MODULE_NAME "SPI"

#include <assert.h>
#include <string.h>
#include "stm32f0xx.h"
#include "egl_lib.h"
#include "ecd_bsp.h"

#define SPI          (SPI1)
#define PORT1        (GPIOB)
#define PORT2        (GPIOA)
#define SCK          (GPIO_Pin_3)
#define MISO         (GPIO_Pin_4)
#define MOSI         (GPIO_Pin_5)
#define CS           (GPIO_Pin_15)
#define CLOCK_A      (RCC_AHBPeriph_GPIOA)
#define CLOCK_B      (RCC_AHBPeriph_GPIOB)
#define CLOCK_SPI    (RCC_APB2Periph_SPI1)
#define SCK_AF       (GPIO_PinSource3)
#define MISO_AF      (GPIO_PinSource4)
#define MOSI_AF      (GPIO_PinSource5)
#define CS_AF        (GPIO_PinSource15)
#define BUFF_SIZE    (128)
#define IRQ_PRIORITY (1)
#define DMA_RX       (DMA1_Channel2)
#define DMA_TX       (DMA1_Channel3)
#define CLOCK_DMA    (RCC_AHBPeriph_DMA1)
#define IRQ          (DMA1_Channel2_3_IRQn)

static uint8_t tx_buff[BUFF_SIZE] = {0};
static uint8_t rx_buff[BUFF_SIZE] = {0};

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

static void setup_dma_read(size_t offset)
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

  /* Set up buffer to transmission */
  config.DMA_MemoryBaseAddr = (uint32_t)rx_buff + offset;
  config.DMA_BufferSize     = BUFF_SIZE - offset;

  DMA_Cmd(DMA_RX, DISABLE);
  DMA_Init(DMA_RX, &config);
  DMA_Cmd(DMA_RX, ENABLE);
}

void dma_init(void)
{
  RCC_AHBPeriphClockCmd(CLOCK_DMA, ENABLE);

  /* Enable DMA SPI TX Transfer complete interrupt */
  DMA_ITConfig(DMA_TX, DMA_IT_TC, ENABLE);

}

static void init(void)
{
  gpio_init();
  spi_init();
  dma_init();
  nvic_init();
}

static egl_result_t open(void)
{
  SPI_Cmd(SPI, ENABLE);

  /* start reading data trough dma */
  setup_dma_read(0);

  return EGL_SUCCESS;
}

static size_t write(void* data, size_t len)
{
  /* config dma tx channel */
  static DMA_InitTypeDef config = 
  {
    .DMA_PeripheralBaseAddr = (uint32_t)(&SPI->DR),
    .DMA_MemoryBaseAddr     = (uint32_t)tx_buff,
    .DMA_DIR                = DMA_DIR_PeripheralDST,
    .DMA_PeripheralInc      = DMA_PeripheralInc_Disable,
    .DMA_MemoryInc          = DMA_MemoryInc_Enable,
    .DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
    .DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte,
    .DMA_Mode               = DMA_Mode_Normal,
    .DMA_Priority           = DMA_Priority_VeryHigh,
    .DMA_M2M                = DMA_M2M_Disable,
  };

  /* check if dma in error state */
  assert(DMA_GetFlagStatus(DMA1_FLAG_TE3) == RESET);

  /* check if data length less then buffer */
  if(len > BUFF_SIZE)
  {
    len = BUFF_SIZE;
  }
  
  /* copy data to buffer */
  memcpy(tx_buff, data, len);

  /* Set up dma transmission */
  config.DMA_BufferSize = len;
  DMA_Init(DMA_TX, &config);
  DMA_Cmd(DMA_TX, ENABLE);

  /* Notify that board has new data */
  egl_pio_set(int1(), true);  

  return len;
}

static size_t read(void* data, size_t len)
{
  uint16_t current_read      = 0;
  static size_t read_in_idx  = 0; /* index of last recived data in buffer */
  static size_t read_out_idx = 0; /* index of last read data in buffer */
  
  /* 
    check CS pin, if it is in low state then transmission in process
    and we should try to read data laiter 
  */
  if(GPIO_ReadInputDataBit(PORT2, CS) == false)
  {
    return 0;
  }

  /* Get amount of bytes that came to buffer */  
  current_read = BUFF_SIZE - DMA_GetCurrDataCounter(DMA_RX);

  read_in_idx += current_read;

  /* check if in index not out of boundary */
  assert(read_in_idx < BUFF_SIZE);

  /* truncate length if it is greater then buffer has */
  if(read_in_idx < len + read_out_idx)
  {
    len = read_in_idx - read_out_idx;
  }

  /* copy data */
  memcpy(data, rx_buff + read_out_idx, len);
  read_out_idx += len;

  /* if all data in buffer has been read, then reset indexes */
  if(read_out_idx >= read_in_idx)
  {
    read_in_idx = 0;
    read_out_idx = 0;
  }

  /* if come data has been read trough dma, restart dma reading once again */
  if(current_read > 0)
  {
    setup_dma_read(read_in_idx);  
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

void spi_irq(void)
{
  egl_pio_set(int1(), false);
  DMA_ClearITPendingBit(DMA1_IT_TC3);
  DMA_Cmd(DMA_TX, DISABLE);
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
