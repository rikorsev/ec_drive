#include "egl_lib.h"
#include "stm32f0xx.h"

#define PORT              (GPIOA)
#define PIN               (GPIO_Pin_12)

static void init(void)
{
  GPIO_InitTypeDef config = {0};
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  config.GPIO_Pin                  = PIN;
  config.GPIO_Mode                 = GPIO_Mode_OUT;
  config.GPIO_PuPd                 = GPIO_PuPd_NOPULL;
  GPIO_Init(PORT, &config);  
}

static egl_result_t set(bool state)
{
  if(state == true)
  {
    GPIO_SetBits(PORT, PIN);
  }
  else
  {
    GPIO_ResetBits(PORT, PIN);
  }
  
  return EGL_SUCCESS;
}

static egl_pio_t int1_impl = 
{
  .init = init,
  .set  = set,
};

egl_pio_t *int1(void)
{
  return &int1_impl;
}
