#include <stdint.h>

#include "stm32f0xx.h"
#include "egl_lib.h"

#define TIMER                   (TIM14)
#define MICROSECONDS_IN_MINUTE  (60000000)
#define SWITCH_PER_ROTATION     (48) 

static volatile uint16_t speed_raw = 0;

static inline void timer_init(void)
{
  static const TIM_TimeBaseInitTypeDef config = 
  {
    .TIM_Period = 65535,
    .TIM_Prescaler = 16,
    .TIM_ClockDivision = 0,
    .TIM_CounterMode = TIM_CounterMode_Up
  };

  /* TIM clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
  
  /* Time base configuration */
  TIM_TimeBaseInit(TIMER, (TIM_TimeBaseInitTypeDef *) &config);
}

static void init(void)
{
  timer_init();
}

static void start(void)
{
  /* TIM enable counter */
  TIM_Cmd(TIMER, ENABLE);
  TIM_SetCounter(TIMER, 0);
}

static void stop(void)
{
  /* TIM disable counter */
  TIM_Cmd(TIMER, DISABLE);
}

static void update(void)
{
  speed_raw = TIM_GetCounter(TIMER);
  TIM_SetCounter(TIMER, 0);
}

static uint16_t get(void)
{
  uint32_t speed = speed_raw;
  
  if(TIM_GetFlagStatus(TIMER, TIM_FLAG_Update) == SET)
  {
    speed = 0;
    TIM_ClearFlag(TIMER, TIM_FLAG_Update);
  }
  
  return (uint16_t)(MICROSECONDS_IN_MINUTE / (speed * SWITCH_PER_ROTATION));
}

static inline void timer_deinit(void)
{
  TIM_DeInit(TIMER);

  /* TIM clock disable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, DISABLE);
}

static void deinit(void)
{
  timer_deinit();
}

const egl_bldc_speed_meas_t speed_impl =
{
  .init   = init,
  .start  = start,
  .stop   = stop,
  .update = update,
  .get    = get,
  .deinit = deinit
};
