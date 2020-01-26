#include "stm32f0xx.h"
#include "egl_lib.h"

#define PORT        (GPIOA)
#define PIN         (GPIO_Pin_6)
#define CTL_ADC     (ADC1)

static inline void gpio_init(void)
{
  static const GPIO_InitTypeDef config = 
  {
    .GPIO_Pin = PIN,
    .GPIO_Mode = GPIO_Mode_AN,
    .GPIO_PuPd = GPIO_PuPd_NOPULL
  };
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  GPIO_Init(PORT, (GPIO_InitTypeDef *) &config);  
}

static inline void acd_init(void)
{
  /* ADC already inited in "bldc.c" module, so it is unnecessary to init it there. Any way I should reorganize it in a future*/ 
}

static void init(void)
{
  gpio_init();
  acd_init();
}

static egl_result_t open(void)
{
  /* Do nothing */

  return EGL_SUCCESS;
}

static size_t read(void* data, size_t len)
{
  /* Clear other selected files */
  CTL_ADC->CHSELR = 0;

  /* Set up MAN SPEED CTL ADC channel */
  ADC_ChannelConfig(CTL_ADC, ADC_Channel_5, ADC_SampleTime_1_5Cycles);  
  
  ADC_StartOfConversion(CTL_ADC);

  while(ADC_GetFlagStatus(CTL_ADC, ADC_FLAG_EOC) != SET)
  {
      /* wait fot the end of convertion */
  }

  *(uint16_t *)data = ADC_GetConversionValue(CTL_ADC);

  /* Not necessary to stop conversion as Single-shot mode is used */
  
  return sizeof(uint16_t);
}

static egl_result_t close(void)
{
  /* Do nothing */

  return EGL_SUCCESS;
}

static void deinit(void)
{
  /* TBD */ 
}

static egl_interface_t man_ctl_impl = 
{
  .init   = init,
  .open   = open,
  .write  = NULL,
  .ioctl  = NULL,
  .read   = read,
  .close  = close,
  .deinit = deinit
};

egl_interface_t *man_ctl(void)
{
  return &man_ctl_impl;
}
