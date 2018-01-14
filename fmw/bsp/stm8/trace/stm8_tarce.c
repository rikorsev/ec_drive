#include <yfuns.h>
#include <stdlib.h>
#include <assert.h>

#include "stm8l15x_conf.h"
#include "ringbuffer.h"

typedef enum
{
  TRACE_READY,
  TRACE_BUSY
}trace_state_t;

#define STM8_TRACE_UART_BAUT_RATE     (9600)
#define STM8_TRACE_BUFFER_SIZE        (512)
static trace_state_t state = TRACE_READY;
static uint8_t ring_buffer[STM8_TRACE_BUFFER_SIZE] = {0};
static ring_buffer_t rb_trace = 
{
  .buff = ring_buffer,
  .size = STM8_TRACE_BUFFER_SIZE,
  .idx_in = 0,
  .idx_out = 0
};

void uart1_tracer_init(void)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);

  USART_Init(USART1, STM8_TRACE_UART_BAUT_RATE, USART_WordLength_8b,
             USART_StopBits_1, USART_Parity_No,
             (USART_Mode_TypeDef)(USART_Mode_Tx | USART_Mode_Rx));
  ITC_SetSoftwarePriority(USART1_TX_IRQn, ITC_PriorityLevel_2);
  USART_ITConfig(USART1,USART_IT_TC,ENABLE);
  USART_Cmd(USART1,ENABLE);
}

void uart1_tracer_flush(void)
{
   char data;
   
   if(state != TRACE_BUSY)
   {
       if( 0 != ring_buffer_read(&rb_trace, &data, sizeof(data)))
       {
         state = TRACE_BUSY;
         USART_SendData8(USART1, data);
       }
   }
}


size_t __write(int handle, const unsigned char* buffer, size_t size)
{

  if (handle != _LLIO_STDOUT && handle != _LLIO_STDERR)
  {
    return _LLIO_ERROR;
  }

  if (0 == ring_buffer_write(&rb_trace, (unsigned char*)buffer, size))
  {
    return _LLIO_ERROR;
  
  }

  //uart1_tracer_flush();
  
   return size;
}
       
INTERRUPT_HANDLER(USART1_TX_TIM5_UPD_OVF_TRG_BRK_IRQHandler, 27)
{
  char data;
  
   if( 0 != ring_buffer_read(&rb_trace, &data, sizeof(data)))
   {
      USART_SendData8(USART1, data);
   }
   else
   {
     state = TRACE_READY;
   }
  
  USART_ClearITPendingBit(USART1, USART_IT_TC);
}