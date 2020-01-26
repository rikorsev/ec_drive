#include "egl_lib.h"
#include "stm32f0xx.h"

#define ECD_BLDC_INT_PORT              (GPIOA)
#define ECD_BLDC_INT_PIN               (GPIO_Pin_12)

static void init(void)
{
  GPIO_InitTypeDef config = {0};
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  config.GPIO_Pin                  = ECD_BLDC_INT_PIN;
  config.GPIO_Mode                 = GPIO_Mode_OUT;
  config.GPIO_PuPd                 = GPIO_PuPd_NOPULL;
  GPIO_Init(ECD_BLDC_INT_PORT, &config);  
}

static egl_result_t set(bool state)
{
  if(state == true)
    {
      GPIO_SetBits(ECD_BLDC_INT_PORT, ECD_BLDC_INT_PIN);
    }
  else
    {
      GPIO_ResetBits(ECD_BLDC_INT_PORT, ECD_BLDC_INT_PIN);
    }
  
  return EGL_NOT_SUPPORTED;
}

static egl_pio_t ecd_int_pin_impl = 
  {
    .init = init,
    .set  = set,
  };

egl_pio_t *ecd_int_pin(void)
{
  return &ecd_int_pin_impl;
}
