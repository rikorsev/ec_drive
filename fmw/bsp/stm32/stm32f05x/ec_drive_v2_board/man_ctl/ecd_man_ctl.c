
#include "stm32f0xx.h"
#include "egl_lib.h"

#define ECD_MAN_PWM_CTL_PORT        (GPIOA)
#define ECD_MAN_PWM_CTL_PIN         (GPIO_Pin_6)
#define ECD_MAN_PWM_CTL_ADC         (ADC1)

static inline void gpio_init(void)
{
  GPIO_InitTypeDef config = {0};
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  config.GPIO_Pin                  = ECD_MAN_PWM_CTL_PIN;
  config.GPIO_Mode                 = GPIO_Mode_AN;
  config.GPIO_PuPd                 = GPIO_PuPd_NOPULL;
  GPIO_Init(ECD_MAN_PWM_CTL_PORT, &config);  
}

static inline void acd_init(void)
{
  /* ADC already inited in "ecd_bldc_load.c" module, so it is unnecessary to init it there. Any way I should reorganize it in a future*/ 
}

static void init(void)
{
  gpio_init();
  acd_init();
}

static egl_itf_status_t open(void)
{
  /* Do nothing */

  return EGL_ITF_SUCCESS;
}

static size_t read(void* data, size_t len)
{
  /* Clear other selected files */
  ECD_MAN_PWM_CTL_ADC->CHSELR = 0;

  /* Set up MAN SPEED CTL ADC channel */
  ADC_ChannelConfig(ECD_MAN_PWM_CTL_ADC, ADC_Channel_5, ADC_SampleTime_1_5Cycles);  
  
  ADC_StartOfConversion(ECD_MAN_PWM_CTL_ADC);

  while(ADC_GetFlagStatus(ECD_MAN_PWM_CTL_ADC, ADC_FLAG_EOC) != SET)
    {
      /* wait fot the end of convertion */
    }

  *(uint16_t *)data = ADC_GetConversionValue(ECD_MAN_PWM_CTL_ADC);

  /* Not necessary to stop conversion as Single-shot mode is used */
  
  return sizeof(uint16_t);
}

static egl_itf_status_t close(void)
{
  /* Do nothing */

  return EGL_ITF_SUCCESS;
}

static void deinit(void)
{
  /* TBD */ 
}

static egl_interface_t ecd_man_pwm_ctl_impl = 
{
  .init   = init,
  .open   = open,
  .write  = NULL,
  .ioctl  = NULL,
  .read   = read,
  .close  = close,
  .deinit = deinit
};

egl_interface_t *ecd_man_pwm_ctl(void)
{
  return &ecd_man_pwm_ctl_impl;
}
