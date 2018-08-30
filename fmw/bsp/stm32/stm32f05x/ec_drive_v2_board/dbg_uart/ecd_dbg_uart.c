#include <assert.h>
#include "stm32f0xx.h"
#include "egl_lib.h"
#include "ecd_dbg_uart.h"

#define ECD_DBG_USART                (USART2)
#define ECD_DBG_USART_PORT           (GPIOA)
#define ECD_DBG_USART_TX_PIN         (GPIO_Pin_2)
#define ECD_DBG_USART_RX_PIN         (GPIO_Pin_3)
#define ECD_DBG_USART_TX_PIN_SRC     (GPIO_PinSource2)
#define ECD_DBG_USART_RX_PIN_SRC     (GPIO_PinSource3)
#define ECD_DBG_USART_DEFAULT_SPEED  (115200)
#define ECD_DBG_USART_BUFF_SIZE      (512)
#define ECD_DBG_USART_GPIO_RCC       (RCC_AHBPeriph_GPIOA)
#define ECD_DBG_USART_RCC            (RCC_APB1Periph_USART2)
#define ECD_DBG_USART_IRQ_PRIORITY   (1)

DECLARE_RING_BUFFER(ecd_dbg_uart_tx_rb, ECD_DBG_USART_BUFF_SIZE);

static void init(void)
{
  USART_InitTypeDef usart;
  GPIO_InitTypeDef  gpio;
  NVIC_InitTypeDef  nvic;
  
  /* USARTx configured as follow:
  - BaudRate = 9600 baud  
  - Word Length = 8 Bits
  - Stop Bit = 1 Stop Bit
  - Parity = No Parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  */
  
  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(ECD_DBG_USART_GPIO_RCC, ENABLE);

  /* Enable USART clock */
  RCC_APB1PeriphClockCmd(ECD_DBG_USART_RCC, ENABLE); 

  /* Connect PXx to USARTx_Rx */
  GPIO_PinAFConfig(ECD_DBG_USART_PORT, ECD_DBG_USART_RX_PIN_SRC, GPIO_AF_1);

  /* Connect PXx to USARTx_Tx */
  GPIO_PinAFConfig(ECD_DBG_USART_PORT, ECD_DBG_USART_TX_PIN_SRC, GPIO_AF_1);
  
  /* Configure USART Tx as alternate function push-pull */
  gpio.GPIO_Pin                    = ECD_DBG_USART_TX_PIN;
  gpio.GPIO_Mode                   = GPIO_Mode_AF;
  gpio.GPIO_Speed                  = GPIO_Speed_10MHz;
  gpio.GPIO_OType                  = GPIO_OType_PP;
  gpio.GPIO_PuPd                   = GPIO_PuPd_UP;
  GPIO_Init(ECD_DBG_USART_PORT, &gpio);

  gpio.GPIO_Pin                    = ECD_DBG_USART_RX_PIN;
  GPIO_Init(ECD_DBG_USART_PORT, &gpio);
  
  usart.USART_BaudRate             = ECD_DBG_USART_DEFAULT_SPEED;
  usart.USART_WordLength           = USART_WordLength_8b;
  usart.USART_StopBits             = USART_StopBits_1;
  usart.USART_Parity               = USART_Parity_No;
  usart.USART_HardwareFlowControl  = USART_HardwareFlowControl_None;
  usart.USART_Mode                 = USART_Mode_Rx | USART_Mode_Tx;
  
  /* USART configuration */
  USART_Init(ECD_DBG_USART, &usart);

  /* Enable the USART Interrupt */
  nvic.NVIC_IRQChannel             = USART2_IRQn;
  nvic.NVIC_IRQChannelPriority     = ECD_DBG_USART_IRQ_PRIORITY;
  nvic.NVIC_IRQChannelCmd          = ENABLE;
  NVIC_Init(&nvic);
}

static egl_itf_status_t open(void)
{
  /* Enable USART */
  USART_Cmd(ECD_DBG_USART, ENABLE);

  return EGL_ITF_SUCCESS;
}

static size_t write_polling(void* buff, size_t len)
{
  int i;
  uint8_t* data_ptr = (uint8_t *)buff;
  uint8_t data = 0;

  /* flush data from ringbuffer if any */
  while(ring_buffer_get_full_size(&ecd_dbg_uart_tx_rb) > 0)
    {
      (void)ring_buffer_read(&ecd_dbg_uart_tx_rb, &data, 1);

      while(USART_GetFlagStatus(ECD_DBG_USART, USART_FLAG_TXE) != SET)
	{
	  /* Do nothing */
	}

      USART_SendData(ECD_DBG_USART, (uint16_t)data);
    }
  
  for(i = 0; i < len; i++)
    {    
    
      while(USART_GetFlagStatus(ECD_DBG_USART, USART_FLAG_TXE) != SET)
	{
	  /* Do nothing */
	}

      USART_SendData(ECD_DBG_USART, (uint16_t)(data_ptr[i]));
    }
  
  return len;
}

static size_t write_interrupt(void* buff, size_t len)
{
  len = ring_buffer_write(&ecd_dbg_uart_tx_rb, buff, len);
  
  if(USART_GetFlagStatus(ECD_DBG_USART, USART_FLAG_TC) == SET)
    {
      USART_ITConfig(ECD_DBG_USART, USART_IT_TXE, ENABLE);
    }
  
  return len;
}

static egl_itf_status_t ioctl(uint8_t opcode, void* data, size_t len)
{
  switch(opcode)
    {
      
    case ECD_DBU_UART_WRITE_INTERRUPT_IOCTL:
      ecd_dbg_usart()->write = write_interrupt;
      break;
      
    case ECD_DBG_UART_WRITE_POLLING_IOCTL:
      ecd_dbg_usart()->write = write_polling;
      break;
      
    }

  return EGL_ITF_SUCCESS;
}

static egl_itf_status_t close(void)
{
  /* Disable USART */
  USART_Cmd(ECD_DBG_USART, DISABLE);

  return EGL_ITF_SUCCESS;
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
  egl_itf_status_t result = EGL_ITF_SUCCESS;

  switch (file)
    {
    case 1: /* stdout */
    case 2: /* stderr */
      result = egl_itf_write(ecd_dbg_usart(), ptr, (size_t *)&len);
      assert(result == EGL_ITF_SUCCESS);
      break;
    default:
      return -1;
    }
  return len;
}
