#include "egl_lib.h"
#include "stm32f0xx.h"

#define PORT              (GPIOA)
#define PIN               (GPIO_Pin_3)

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

static egl_result_t get(void)
{
  uint8_t state = GPIO_ReadOutputDataBit(PORT, PIN);

  return state == Bit_SET ? EGL_SET : EGL_RESET;
}

static egl_pio_t int2_impl = 
{
  .init = init,
  .set  = set,
  .get  = get
};

egl_pio_t *int2(void)
{
  return &int2_impl;
}
