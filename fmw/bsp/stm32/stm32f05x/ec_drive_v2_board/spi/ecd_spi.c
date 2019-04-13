#include <assert.h>
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

DECLARE_RING_BUFFER(ecd_spi_tx_rb, ECD_SPI_BUFF_SIZE);
DECLARE_RING_BUFFER(ecd_spi_rx_rb, ECD_SPI_BUFF_SIZE);

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

  SPI_I2S_ITConfig(ECD_SPI, SPI_I2S_IT_RXNE, ENABLE);
}

static void nvic_init(void)
{
  NVIC_InitTypeDef config = {0};
  
  /* Configure the SPI interrupt priority */
  config.NVIC_IRQChannel         = SPI1_IRQn;
  config.NVIC_IRQChannelPriority = ECD_SPI_IRQ_PRIORITY;
  config.NVIC_IRQChannelCmd      = ENABLE;
  NVIC_Init(&config);
}

static void init(void)
{
  gpio_init();
  spi_init();
  nvic_init();
}

static egl_result_t open(void)
{
  SPI_Cmd(ECD_SPI, ENABLE);

  return EGL_SUCCESS;
}

static size_t write(void* data, size_t len)
{
  return ring_buffer_write(&ecd_spi_tx_rb, data, len);
}

static size_t read(void* data, size_t len)
{
  return ring_buffer_read(&ecd_spi_rx_rb, data, len);
}

static egl_result_t close(void)
{
  SPI_Cmd(ECD_SPI, DISABLE);

  return EGL_SUCCESS;
}

void SPI1_IRQHandler(void)
{
  uint8_t data = 0;
  
  if(SPI_I2S_GetITStatus(ECD_SPI, SPI_I2S_IT_RXNE) == SET)
    {
      egl_led_toggle(ecd_led());
      
      data = SPI_ReceiveData8(ECD_SPI);

      /* data should be writen to RX buffer otherwise buffer is overfull and it should FAULT happens */
      assert(ring_buffer_write(&ecd_spi_rx_rb, &data, sizeof(data)));

      /* If we have spmething in TX buffer, then send it */
      if(ring_buffer_get_full_size(&ecd_spi_tx_rb) > 0)
	{
	  ring_buffer_read(&ecd_spi_tx_rb, &data, sizeof(data));
	  SPI_SendData8(ECD_SPI, data); 
	}
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
