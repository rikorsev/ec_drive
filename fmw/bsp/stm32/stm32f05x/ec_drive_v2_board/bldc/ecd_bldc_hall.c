#define EGL_MODULE_NAME "BLDC:HALL"

#include "stm32f0xx.h"
#include "egl_lib.h"
#include "ecd_bsp.h"

#define ECD_BLDC_HALL_IRQ_PRIORITY (0)
#define ECD_BLDC_HALL_IRQ          (EXTI0_1_IRQn | EXTI4_15_IRQn)

#define ECD_BLDC_HALL_PORT         (GPIOA)
#define ECD_BLDC_HALL_1_PIN        (GPIO_Pin_0)
#define ECD_BLDC_HALL_2_PIN        (GPIO_Pin_1)
#define ECD_BLDC_HALL_3_PIN        (GPIO_Pin_11)

#define ECD_BLDC_HALL_1_EXTI       (EXTI_Line0)
#define ECD_BLDC_HALL_2_EXTI       (EXTI_Line1)
#define ECD_BLDC_HALL_3_EXTI       (EXTI_Line11)

#define ECD_BLDC_HALL_MASK         ((uint16_t)0x0803)

static void init(void)
{
  GPIO_InitTypeDef gpio        = {0};
  NVIC_InitTypeDef nvic        = {0};
  EXTI_InitTypeDef exti        = {0};
  
  /* GPIOA Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  /* Configure PA1,PA2 and PA11 in input mode */
  gpio.GPIO_Pin                = ECD_BLDC_HALL_1_PIN | \
                                 ECD_BLDC_HALL_2_PIN | \
                                 ECD_BLDC_HALL_3_PIN;
  gpio.GPIO_Mode               = GPIO_Mode_IN;
  gpio.GPIO_OType              = GPIO_OType_PP;
  gpio.GPIO_Speed              = GPIO_Mode_IN;
  gpio.GPIO_PuPd               = GPIO_PuPd_NOPULL;
  GPIO_Init(ECD_BLDC_HALL_PORT, &gpio);

  /* Set interrupt handling */
  nvic.NVIC_IRQChannel         = EXTI0_1_IRQn;//ECD_BLDC_HALL_IRQ;
  nvic.NVIC_IRQChannelPriority = ECD_BLDC_HALL_IRQ_PRIORITY;
  nvic.NVIC_IRQChannelCmd      = ENABLE;
  NVIC_Init(&nvic);

  nvic.NVIC_IRQChannel         = EXTI4_15_IRQn;
  NVIC_Init(&nvic);
  
  exti.EXTI_Line               = ECD_BLDC_HALL_1_EXTI;
  exti.EXTI_Mode               = EXTI_Mode_Interrupt;
  exti.EXTI_Trigger            = EXTI_Trigger_Rising_Falling; 
  exti.EXTI_LineCmd            = ENABLE;
  EXTI_Init(&exti);

  exti.EXTI_Line               = ECD_BLDC_HALL_2_EXTI;
  EXTI_Init(&exti);

  exti.EXTI_Line               = ECD_BLDC_HALL_3_EXTI;
  EXTI_Init(&exti);
}

static uint16_t get(void)
{
  return GPIO_ReadInputData(ECD_BLDC_HALL_PORT);// & ECD_BLDC_HALL_MASK;
}

static void deinit(void)
{
  EXTI_InitTypeDef exti        = {0};
  NVIC_InitTypeDef nvic        = {0};
  
  exti.EXTI_Line               = ECD_BLDC_HALL_1_EXTI | \
                                 ECD_BLDC_HALL_2_EXTI | \
                                 ECD_BLDC_HALL_3_EXTI;
  exti.EXTI_LineCmd            = DISABLE;
  EXTI_Init(&exti);

  /* Set interrupt handling */
  nvic.NVIC_IRQChannel         = ECD_BLDC_HALL_IRQ;
  nvic.NVIC_IRQChannelCmd      = DISABLE;
  NVIC_Init(&nvic);
}

void EXTI0_1_IRQHandler(void)
{
  if(EXTI_GetITStatus(ECD_BLDC_HALL_1_EXTI) == SET)
    {
      EXTI_ClearITPendingBit(ECD_BLDC_HALL_1_EXTI);
      egl_led_toggle(ecd_led());
      EGL_TRACE_INFO("1: 0x%x -> 0x%x\r\n", get(), get() & ECD_BLDC_HALL_MASK);
    }

  if(EXTI_GetITStatus(ECD_BLDC_HALL_2_EXTI) == SET)
    {
      EXTI_ClearITPendingBit(ECD_BLDC_HALL_2_EXTI);
      egl_led_toggle(ecd_led());
      EGL_TRACE_INFO("2: 0x%x -> 0x%x\r\n", get(), get() & ECD_BLDC_HALL_MASK);
    }
}

void EXTI4_15_IRQHandler(void)\
{
  if(EXTI_GetITStatus(ECD_BLDC_HALL_3_EXTI) == SET)
    {
      EXTI_ClearITPendingBit(ECD_BLDC_HALL_3_EXTI);
      egl_led_toggle(ecd_led());
      EGL_TRACE_INFO("3: 0x%x -> 0x%x\r\n", get(), get() & ECD_BLDC_HALL_MASK);
    }
}

egl_bldc_hall_t ecd_bldc_hall_impl =
  {
    .init   = init,
    .get    = get,
    .deinit = deinit,
  };

egl_bldc_hall_t* ecd_bldc_hall(void)
{
  return &ecd_bldc_hall_impl;
}
