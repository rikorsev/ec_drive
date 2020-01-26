#include <stdbool.h>
#include "stm32f0xx.h"
#include "egl_lib.h"

#define PORT (GPIOA)
#define PIN  (GPIO_Pin_4)

extern egl_counter_t runtime_impl;

static void init(void)
{
  GPIO_InitTypeDef config = {0};
  /* GPIOA Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  /* Configure PA4 in output pushpull mode */
  config.GPIO_Pin   = PIN;
  config.GPIO_Mode  = GPIO_Mode_OUT;
  config.GPIO_OType = GPIO_OType_PP;
  config.GPIO_Speed = GPIO_Speed_2MHz;
  config.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(PORT, &config);
}

static void on(void)
{
  GPIO_SetBits(PORT, PIN);
}

static void off(void)
{
  GPIO_ResetBits(PORT, PIN);
}

static bool state(void)
{
  return GPIO_ReadOutputDataBit(PORT, PIN) ? true : false;  
}

static void deinit(void)
{
  GPIO_DeInit(PORT);
}

static const egl_led_t led_impl =
{
  .itf =
  {
    .init   = init,
    .on     = on,
    .off    = off,
    .state  = state,
    .deinit = deinit
  },

  .ctr = &runtime_impl
};

egl_led_t* led(void)
{
  return (egl_led_t *)&led_impl;
}
