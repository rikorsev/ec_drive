#include <assert.h>
#include "stm32f0xx.h"
#include "egl_lib.h"
#include "ecd_dbg_uart.h"

#define USART          (USART2)
#define PORT           (GPIOA)
#define TX_PIN         (GPIO_Pin_2)
#define TX_PIN_SRC     (GPIO_PinSource2)
#define DEFAULT_SPEED  (115200)
#define BUFF_SIZE      (512)
#define GPIO_RCC       (RCC_AHBPeriph_GPIOA)
#define RCC            (RCC_APB1Periph_USART2)
#define IRQ_PRIORITY   (1)

DECLARE_RING_BUFFER(tx_buff, BUFF_SIZE);

static void init_gpio(void)
{
  static const GPIO_InitTypeDef config = 
  {
    .GPIO_Pin   = ECD_DBG_USART_TX_PIN,
    .GPIO_Mode  = GPIO_Mode_AF,
    .GPIO_Speed = GPIO_Speed_10MHz,
    .GPIO_OType = GPIO_OType_PP,
    .GPIO_PuPd  = GPIO_PuPd_UP
  };
  
  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(GPIO_RCC, ENABLE);

  /* Connect PXx to USARTx_Tx */
  GPIO_PinAFConfig(PORT, TX_PIN_SRC, GPIO_AF_1);
  
  /* Configure USART Tx as alternate function push-pull */
  GPIO_Init(ECD_DBG_USART_PORT, &gpio);
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
  RCC_APB1PeriphClockCmd(ECD_DBG_USART_RCC, ENABLE); 

  /* USART configuration */
  USART_Init(USART, (USART_InitTypeDef *) &usart);
}

static void init_irq(void)
{
  static const NVIC_InitTypeDef config = 
  {
    .NVIC_IRQChannel         = USART2_IRQn,
    .NVIC_IRQChannelPriority = ECD_DBG_USART_IRQ_PRIORITY,
    .NVIC_IRQChannelCmd      = ENABLE
  };
  
  /* Enable the USART Interrupt */
  NVIC_Init( (NVIC_InitTypeDef *)  &config);
}

static void init(void)
{
  init_gpio();
  init_uart();
  init_irq();
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
  while(ring_buffer_get_full_size(&tx_buff) > 0)
  {
    (void)ring_buffer_read(&tx_buff, &data, 1);

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
  len = ring_buffer_write(&tx_buff, buff, len);
  
  if(USART_GetFlagStatus(USART, USART_FLAG_TC) == SET)
  {
    USART_ITConfig(USART, USART_IT_TXE, ENABLE);
  }
  
  return len;
}

static egl_result_t ioctl(uint8_t opcode, void* data, size_t len)
{
  switch(opcode)
  {
      
    case ECD_DBU_UART_WRITE_INTERRUPT_IOCTL:
      dbg()->write = write_interrupt;
      break;
      
    case ECD_DBG_UART_WRITE_POLLING_IOCTL:
      dbg()->write = write_polling;
      break;
  }

  return EGL_SUCCESS;
}

static egl_result_t close(void)
{
  /* Disable USART */
  USART_Cmd(USART, DISABLE);

  return EGL_SUCCESS;
}

static void deinit(void)
{
  NVIC_InitTypeDef  nvic;
  
  /* Disable GPIO clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, DISABLE);

  /* Disable USART clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE); 

  /* Disable the USART Interrupt */
  nvic.NVIC_IRQChannel             = USART2_IRQn;
  nvic.NVIC_IRQChannelPriority     = 0;
  nvic.NVIC_IRQChannelCmd          = DISABLE;
  NVIC_Init(&nvic);
}

void USART2_IRQHandler(void)
{
  uint8_t data = 0;
  
  if(USART_GetITStatus(ECD_DBG_USART, USART_IT_TXE) == SET)
    {
      /* if ringbuffer not empty */ 
      if(ring_buffer_get_full_size(&ecd_dbg_uart_tx_rb) > 0)
  {
    (void)ring_buffer_read(&ecd_dbg_uart_tx_rb, &data, 1);
    USART_SendData(ECD_DBG_USART, (uint16_t)data);
  }
      else
  {
    /* Disable interrupt at the end of transmission */
    USART_ITConfig(ECD_DBG_USART, USART_IT_TXE, DISABLE);
  }
    } 
}

static egl_interface_t ecd_dbg_usart_impl = 
{
  .init   = init,
  .open   = open,
  .write  = write_interrupt,
  .ioctl  = ioctl,
  .read   = NULL,
  .close  = close,
  .deinit = deinit
};

egl_interface_t *ecd_dbg_usart(void)
{
  return &ecd_dbg_usart_impl;
}

int _write(int file, char *ptr, int len)
{
  size_t to_write = len;
  size_t offset = 0;

  switch (file)
    {
    case 1: /* stdout */
    case 2: /* stderr */
      while(offset < len)
      {
        to_write = len - offset;
        assert(egl_itf_write(ecd_dbg_usart(), ptr + offset, (size_t *)&to_write) == EGL_SUCCESS);
        offset += to_write;
      }
    break;
    
    default:
      return -1;
    }
  return len;
}
