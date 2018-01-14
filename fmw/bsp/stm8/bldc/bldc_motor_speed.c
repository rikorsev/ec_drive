#include "stm8l15x_conf.h"

#define SWITCHINGS_PER_ROTATION (51)
#define MICROSECONDS_IN_MINUTE  ((uint32_t)60000000)

static uint16_t bldc_speed = 0;

void bldc_speed_init(void)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);  
  TIM2_TimeBaseInit(TIM2_Prescaler_16,TIM2_CounterMode_Up, 0xFFFF);
  TIM2_ITConfig(TIM2_IT_Update, ENABLE);
}

void bldc_speed_start(void)
{
  TIM2_Cmd(ENABLE);
  TIM2_SetCounter(0x0000);
}

void bldc_speed_stop(void)
{
  TIM2_Cmd(DISABLE);
  TIM2_SetCounter(0x0000);
}

void bldc_speed_update(void)
{
  bldc_speed = TIM2_GetCounter();
  TIM2_SetCounter(0x0000);
}

uint16_t bldc_speed_get(void)
{
  return bldc_speed == 0 ? 0 : (uint16_t)(MICROSECONDS_IN_MINUTE/
                     ((uint32_t)bldc_speed * SWITCHINGS_PER_ROTATION));
}

INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler, 19)
{
  bldc_speed = 0; 
  TIM2_ClearITPendingBit(TIM2_IT_Update);
}
