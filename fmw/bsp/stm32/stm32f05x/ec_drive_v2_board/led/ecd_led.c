#include <stdbool.h>
#include "stm32f0xx.h"
#include "egl_lib.h"

#define ECD_LED_PORT (GPIOA)
#define ECD_LED_PIN  (GPIO_Pin_4)

extern egl_counter_t ecd_runtime;

static void init(void)
{
  GPIO_InitTypeDef gpio = {0};
  /* GPIOA Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  /* Configure PD0 and PD2 in output pushpull mode */
  gpio.GPIO_Pin = ECD_LED_PIN;
  gpio.GPIO_Mode = GPIO_Mode_OUT;
  gpio.GPIO_OType = GPIO_OType_PP;
  gpio.GPIO_Speed = GPIO_Speed_2MHz;
  gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &gpio);
}

static void on(void)
{
  GPIO_SetBits(ECD_LED_PORT, ECD_LED_PIN);
}

static void off(void)
{
  GPIO_ResetBits(ECD_LED_PORT, ECD_LED_PIN);
}

static bool state(void)
{
  return GPIO_ReadOutputDataBit(ECD_LED_PORT, ECD_LED_PIN) ? true : false;  
}

static void deinit(void)
{
  GPIO_DeInit(ECD_LED_PORT);
}

static const egl_led_t ecd_led =
  {
    .itf =
    {
      .init   = init,
      .on     = on,
      .off    = off,
      .state  = state,
      .deinit = deinit
    },

    .ctr = &ecd_runtime
  };

egl_led_t* ecd_led_get(void)
{
  return (egl_led_t *)&ecd_led;
}
