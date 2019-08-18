#define EGL_MODULE_NAME "ECD:SPI"

#include <assert.h>
#include <string.h>
#include "stm32f0xx.h"
#include "egl_lib.h"
#include "ecd_bsp.h"

#define ECD_SPI              (SPI1)
#define ECD_SPI_PORT1        (GPIOB)
#define ECD_SPI_PORT2        (GPIOA)
#define ECD_SPI_SCK_PIN      (GPIO_Pin_3)
#define ECD_SPI_MISO_PIN     (GPIO_Pin_4)
#define ECD_SPI_MOSI_PIN     (GPIO_Pin_5)
#define ECD_SPI_CS_PIN       (GPIO_Pin_15)
#define ECD_SPI_GPIO_RCC1    (RCC_AHBPeriph_GPIOA)
#define ECD_SPI_GPIO_RCC2    (RCC_AHBPeriph_GPIOB)
#define ECD_SPI_SCK_PIN_SRC  (GPIO_PinSource3)
#define ECD_SPI_MISO_PIN_SRC (GPIO_PinSource4)
#define ECD_SPI_MOSI_PIN_SRC (GPIO_PinSource5)
#define ECD_SPI_CS_PIN_SRC   (GPIO_PinSource15)
#define ECD_SPI_BUFF_SIZE    (128)
#define ECD_SPI_IRQ_PRIORITY (1)

static uint8_t ecd_spi_tx_dma_buff[ECD_SPI_BUFF_SIZE] = {0};
static uint8_t ecd_spi_rx_dma_buff[ECD_SPI_BUFF_SIZE] = {0};

static void gpio_init(void)
{
  GPIO_InitTypeDef config = {0};  

  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(ECD_SPI_GPIO_RCC1, ENABLE);
  RCC_AHBPeriphClockCmd(ECD_SPI_GPIO_RCC2, ENABLE);

  /* SPI pin mappings */
  GPIO_PinAFConfig(ECD_SPI_PORT1, ECD_SPI_SCK_PIN_SRC,  GPIO_AF_0);
  GPIO_PinAFConfig(ECD_SPI_PORT1, ECD_SPI_MISO_PIN_SRC, GPIO_AF_0);
  GPIO_PinAFConfig(ECD_SPI_PORT1, ECD_SPI_MOSI_PIN_SRC, GPIO_AF_0);
  GPIO_PinAFConfig(ECD_SPI_PORT2, ECD_SPI_CS_PIN_SRC,   GPIO_AF_0);

  config.GPIO_Mode  = GPIO_Mode_AF;
  config.GPIO_OType = GPIO_OType_PP;
  config.GPIO_PuPd  = GPIO_PuPd_DOWN;
  config.GPIO_Speed = GPIO_Speed_Level_3;

  /* SPI SCK pin configuration */
  config.GPIO_Pin   = ECD_SPI_SCK_PIN;
  GPIO_Init(ECD_SPI_PORT1, &config);

  /* SPI  MOSI pin configuration */
  config.GPIO_Pin   = ECD_SPI_MISO_PIN;
  GPIO_Init(ECD_SPI_PORT1, &config);

  /* SPI MISO pin configuration */
  config.GPIO_Pin   = ECD_SPI_MOSI_PIN;
  GPIO_Init(ECD_SPI_PORT1, &config);
  
  /* SPI CS pin configuration */
  config.GPIO_Pin   = ECD_SPI_CS_PIN;
  GPIO_Init(ECD_SPI_PORT2, &config);
  
}

static void spi_init(void)
{
  SPI_InitTypeDef config = {0};

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    
  /* Initializes the SPI communication */
  config.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
  config.SPI_DataSize          = SPI_DataSize_8b;
  config.SPI_CPOL              = SPI_CPOL_Low;
  config.SPI_CPHA              = SPI_CPHA_1Edge;
  config.SPI_NSS               = SPI_NSS_Hard;
  config.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  config.SPI_FirstBit          = SPI_FirstBit_MSB;
  config.SPI_CRCPolynomial     = 7;
  config.SPI_Mode              = SPI_Mode_Slave;

  SPI_Init(ECD_SPI, &config);

  SPI_RxFIFOThresholdConfig(ECD_SPI, SPI_RxFIFOThreshold_QF);

  SPI_I2S_DMACmd(ECD_SPI, SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx, ENABLE);
}

static void nvic_init(void)
{
  NVIC_InitTypeDef config = {0};

  /* Configure DMA interrupt */
  config.NVIC_IRQChannel         = DMA1_Channel2_3_IRQn;
  config.NVIC_IRQChannelPriority = ECD_SPI_IRQ_PRIORITY;
  config.NVIC_IRQChannelCmd      = ENABLE;
  NVIC_Init(&config);

}

static void setup_dma_read(size_t offset)
{
  /* config dma rx channel */
  static DMA_InitTypeDef config = 
  {
    .DMA_PeripheralBaseAddr = (uint32_t)(&ECD_SPI->DR),
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
  config.DMA_MemoryBaseAddr = (uint32_t)ecd_spi_rx_dma_buff + offset;
  config.DMA_BufferSize     = ECD_SPI_BUFF_SIZE - offset;

  egl_pio_set(ecd_int2_pin(), false);
  DMA_Cmd(DMA1_Channel2, DISABLE);
  DMA_Init(DMA1_Channel2, &config);
  DMA_Cmd(DMA1_Channel2, ENABLE);
  egl_pio_set(ecd_int2_pin(), true);  

}

void dma_init(void)
{
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  /* Enable DMA SPI TX Transfer complete interrupt */
  DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);

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
  SPI_Cmd(ECD_SPI, ENABLE);

  /* start reading data trough dma */
  setup_dma_read(0);

  return EGL_SUCCESS;
}

static size_t write(void* data, size_t len)
{
  /* config dma tx channel */
  static DMA_InitTypeDef config = 
  {
    .DMA_PeripheralBaseAddr = (uint32_t)(&ECD_SPI->DR),
    .DMA_MemoryBaseAddr     = (uint32_t)ecd_spi_tx_dma_buff,
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
  if(len > ECD_SPI_BUFF_SIZE)
  {
    len = ECD_SPI_BUFF_SIZE;
  }
  
  /* copy data to buffer */
  memcpy(ecd_spi_tx_dma_buff, data, len);

  /* Set up dma transmission */
  config.DMA_BufferSize = len;
  DMA_Init(DMA1_Channel3, &config);
  DMA_Cmd(DMA1_Channel3, ENABLE);

  egl_pio_set(ecd_int_pin(), true);  

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
  if(GPIO_ReadInputDataBit(ECD_SPI_PORT2, ECD_SPI_CS_PIN) == false)
  {
    return 0;
  }

  /* Get amount of bytes that came to buffer */  
  current_read = ECD_SPI_BUFF_SIZE - DMA_GetCurrDataCounter(DMA1_Channel2);

  read_in_idx += current_read;

  /* check if in index not out of boundary */
  assert(read_in_idx < ECD_SPI_BUFF_SIZE);

  /* truncate length if it is greater then buffer has */
  if(read_in_idx < len + read_out_idx)
  {
    len = read_in_idx - read_out_idx;
  }

  /* copy data */
  memcpy(data, ecd_spi_rx_dma_buff + read_out_idx, len);
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
  SPI_Cmd(ECD_SPI,       DISABLE);
  DMA_Cmd(DMA1_Channel3, DISABLE);
  DMA_Cmd(DMA1_Channel2, DISABLE);

  return EGL_SUCCESS;
}

void DMA1_Channel2_3_IRQHandler(void)
{
  if(DMA_GetITStatus(DMA1_IT_TC3) == SET)
  {
    egl_pio_set(ecd_int_pin(), false);
    DMA_ClearITPendingBit(DMA1_IT_TC3);
    DMA_Cmd(DMA1_Channel3, DISABLE);
  }
}

static egl_interface_t ecd_spi_impl = 
{
  .init   = init,
  .open   = open,
  .write  = write,
  .ioctl  = NULL,
  .read   = read,
  .close  = close,
  .deinit = NULL
};

egl_interface_t *ecd_spi(void)
{
  return &ecd_spi_impl;
}
