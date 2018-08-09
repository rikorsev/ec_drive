#include "stm32f0xx.h"
#include "egl_lib.h"
#include "ecd_bsp.h"


void ecd_main_timer_init(void)
{
  TIM_TimeBaseInitTypeDef tim = {0};
  NVIC_InitTypeDef nvic;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  
  /* Period 1ms */
  tim.TIM_Prescaler = 0;
  tim.TIM_CounterMode = TIM_CounterMode_Up;
  tim.TIM_Period = 16000;
  tim.TIM_ClockDivision = TIM_CKD_DIV1;
  tim.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM3, &tim);

  /* Enable the TIM2 global Interrupt */
  nvic.NVIC_IRQChannel = TIM3_IRQn;
  nvic.NVIC_IRQChannelPriority = 0;
  nvic.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&nvic);
  
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

  TIM_Cmd(TIM3, ENABLE);
}

void TIM3_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
    {
      egl_counter_update(ecd_runtime());
      TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}
