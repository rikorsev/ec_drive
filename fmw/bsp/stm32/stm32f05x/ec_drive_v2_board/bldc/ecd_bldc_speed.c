#include <stdint.h>

#include "stm32f0xx.h"
#include "egl_lib.h"

#define ECD_BLDC_SPEED_TIM      (TIM14)
#define MICROSECONDS_IN_MINUTE  (60000000)
#define SWITCH_PER_ROTATION     (48) 

static volatile uint16_t ecd_bldc_speed_raw = 0;

static inline void timer_init(void)
{
  TIM_TimeBaseInitTypeDef config = {0};

  /* TIM clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
  
  /* Time base configuration */
  config.TIM_Period                = 65535;
  config.TIM_Prescaler             = 16;
  config.TIM_ClockDivision         = 0;
  config.TIM_CounterMode           = TIM_CounterMode_Up;

  TIM_TimeBaseInit(ECD_BLDC_SPEED_TIM, &config);
}

static void init(void)
{
  timer_init();
}

static void start(void)
{
  /* TIM enable counter */
  TIM_Cmd(ECD_BLDC_SPEED_TIM, ENABLE);
  TIM_SetCounter(ECD_BLDC_SPEED_TIM, 0);
}

static void stop(void)
{
  /* TIM disable counter */
  TIM_Cmd(ECD_BLDC_SPEED_TIM, DISABLE);
}

static void update(void)
{
  ecd_bldc_speed_raw = TIM_GetCounter(ECD_BLDC_SPEED_TIM);
  TIM_SetCounter(ECD_BLDC_SPEED_TIM, 0);
}

static uint16_t get(void)
{
  uint32_t speed = ecd_bldc_speed_raw;
  
  if(TIM_GetFlagStatus(ECD_BLDC_SPEED_TIM, TIM_FLAG_Update) == SET)
    {
      speed = 0;
      TIM_ClearFlag(ECD_BLDC_SPEED_TIM, TIM_FLAG_Update);
    }
  
  return (uint16_t)(MICROSECONDS_IN_MINUTE / (speed * SWITCH_PER_ROTATION));
}

static inline void timer_deinit(void)
{
  TIM_DeInit(ECD_BLDC_SPEED_TIM);

  /* TIM clock disable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, DISABLE);
}

static void deinit(void)
{
  timer_deinit();
}

egl_bldc_speed_meas_t ecd_bldc_speed_impl =
  {
    .init   = init,
    .start  = start,
    .stop   = stop,
    .update = update,
    .get    = get,
    .deinit = deinit
  };
