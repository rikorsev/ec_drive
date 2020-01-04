#include "egl_lib.h"
#include "stm32f0xx.h"

#define ECD_BLDC_INT_PORT              (GPIOA)
#define ECD_BLDC_INT_PIN               (GPIO_Pin_3)

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
  
  return EGL_SUCCESS;
}

static egl_result_t get(void)
{
  uint8_t state = GPIO_ReadOutputDataBit(ECD_BLDC_INT_PORT, ECD_BLDC_INT_PIN);

  return state == Bit_SET ? EGL_SET : EGL_RESET;
}

static egl_pio_t ecd_int2_pin_impl = 
{
  .init = init,
  .set  = set,
  .get  = get
};

egl_pio_t *ecd_int2_pin(void)
{
  return &ecd_int2_pin_impl;
}
