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

#define ECD_BLDC_HALL_STATE_1      (ECD_BLDC_HALL_1_PIN | ECD_BLDC_HALL_3_PIN)
#define ECD_BLDC_HALL_STATE_2      (ECD_BLDC_HALL_1_PIN)
#define ECD_BLDC_HALL_STATE_3      (ECD_BLDC_HALL_1_PIN | ECD_BLDC_HALL_2_PIN)
#define ECD_BLDC_HALL_STATE_4      (ECD_BLDC_HALL_2_PIN)
#define ECD_BLDC_HALL_STATE_5      (ECD_BLDC_HALL_2_PIN | ECD_BLDC_HALL_3_PIN)
#define ECD_BLDC_HALL_STATE_6      (ECD_BLDC_HALL_3_PIN)

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
  gpio.GPIO_Speed              = GPIO_Speed_2MHz;
  gpio.GPIO_PuPd               = GPIO_PuPd_NOPULL;
  GPIO_Init(ECD_BLDC_HALL_PORT, &gpio);

  /* Set interrupt handling */
  nvic.NVIC_IRQChannel         = EXTI0_1_IRQn;
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

static egl_bldc_hall_state_t get(void)
{
  egl_bldc_hall_state_t state = EGL_BLDC_HALL_STATE_ERROR;
  uint16_t hall_gpio = GPIO_ReadInputData(ECD_BLDC_HALL_PORT) & ECD_BLDC_HALL_MASK;

  switch(hall_gpio)
    {
    case ECD_BLDC_HALL_STATE_1:
      state = EGL_BLDC_HALL_STATE_1;
      break;

    case ECD_BLDC_HALL_STATE_2:
      state = EGL_BLDC_HALL_STATE_2;
      break;

    case ECD_BLDC_HALL_STATE_3:
      state = EGL_BLDC_HALL_STATE_3;
      break;

    case ECD_BLDC_HALL_STATE_4:
      state = EGL_BLDC_HALL_STATE_4;
      break;

    case ECD_BLDC_HALL_STATE_5:
      state = EGL_BLDC_HALL_STATE_5;
      break;

    case ECD_BLDC_HALL_STATE_6:
      state = EGL_BLDC_HALL_STATE_6;
      break;
    }

  return state;
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
