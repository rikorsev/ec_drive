#include "stm32f0xx.h"
#include "egl_lib.h"
#include "ecd_bsp.h"

#define TIMER        TIM3
#define CLOCK        RCC_APB1Periph_TIM3
#define IRQ          TIM3_IRQn
#define IRQ_PRIORITY 1

static void init_timer(void)
{
  /* Period 1ms */
  static const TIM_TimeBaseInitTypeDef config  = 
  {
    .TIM_Prescaler         = 0,
    .TIM_CounterMode       = TIM_CounterMode_Up,
    .TIM_Period            = 16000,
    .TIM_ClockDivision     = TIM_CKD_DIV1,
    .TIM_RepetitionCounter = 0
  };

  /* Switch on clock */
  RCC_APB1PeriphClockCmd(CLOCK, ENABLE);

  TIM_TimeBaseInit(TIMER, (TIM_TimeBaseInitTypeDef *) &config);
  TIM_ITConfig(TIMER, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIMER, ENABLE);
}

static void init_irq(void)
{
  static const NVIC_InitTypeDef config = 
  {
    .NVIC_IRQChannel         = IRQ,
    .NVIC_IRQChannelPriority = IRQ_PRIORITY,
    .NVIC_IRQChannelCmd      = ENABLE,
  };

  /* Enable the TIM3 global Interrupt */
  NVIC_Init((NVIC_InitTypeDef *) &config);
}

void main_timer_init(void)
{
  init_timer();
  init_irq();
}
