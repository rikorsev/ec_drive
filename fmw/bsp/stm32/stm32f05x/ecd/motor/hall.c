#define EGL_MODULE_NAME "HALL"

#include "stm32f0xx.h"
#include "egl_lib.h"
#include "ecd_bsp.h"

#define IRQ_PRIORITY (0)
#define IRQ          (EXTI0_1_IRQn | EXTI4_15_IRQn)

#define PORT         (GPIOA)
#define PIN_1        (GPIO_Pin_0)
#define PIN_2        (GPIO_Pin_1)
#define PIN_3        (GPIO_Pin_11)

#define STATE_1      (PIN_1 | PIN_3)
#define STATE_2      (PIN_1)
#define STATE_3      (PIN_1 | PIN_2)
#define STATE_4      (PIN_2)
#define STATE_5      (PIN_2 | PIN_3)
#define STATE_6      (PIN_3)

#define MASK         ((uint16_t)0x0803)

static void init_gpio(void)
{
  /* Configure PA1,PA2 and PA11 in input mode */
  static const GPIO_InitTypeDef config = 
  {
    .GPIO_Pin   = PIN_1 | PIN_2 | PIN_3,
    .GPIO_Mode  = GPIO_Mode_IN,
    .GPIO_Speed = GPIO_Speed_2MHz,
    .GPIO_PuPd  = GPIO_PuPd_NOPULL
  };

  /* GPIOA Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  GPIO_Init(PORT, (GPIO_InitTypeDef *) &config);

}

static void init_nvic(void)
{
  /* Set interrupt handling */
  NVIC_InitTypeDef config = 
  {
    .NVIC_IRQChannel         = EXTI0_1_IRQn,
    .NVIC_IRQChannelPriority = IRQ_PRIORITY,
    .NVIC_IRQChannelCmd      = ENABLE
  };

  NVIC_Init(&config);

  config.NVIC_IRQChannel = EXTI4_15_IRQn;
  NVIC_Init(&config);
}

static void init_exti(void)
{
  static const EXTI_InitTypeDef config = 
  {
    .EXTI_Line    = HALL_1_EXTI | 
                    HALL_2_EXTI | 
                    HALL_3_EXTI,
    .EXTI_Mode    = EXTI_Mode_Interrupt,
    .EXTI_Trigger = EXTI_Trigger_Rising_Falling,
    .EXTI_LineCmd = ENABLE    
  };
    
  EXTI_Init((EXTI_InitTypeDef *)&config);  
}

static void init(void)
{
  init_gpio();
  init_nvic();
  init_exti();
}

static egl_bldc_hall_state_t get(void)
{
  egl_bldc_hall_state_t state = EGL_BLDC_HALL_STATE_ERROR;
  uint16_t hall_gpio = GPIO_ReadInputData(PORT) & MASK;

  switch(hall_gpio)
    {
    case STATE_1:
      state = EGL_BLDC_HALL_STATE_1;
      break;

    case STATE_2:
      state = EGL_BLDC_HALL_STATE_2;
      break;

    case STATE_3:
      state = EGL_BLDC_HALL_STATE_3;
      break;

    case STATE_4:
      state = EGL_BLDC_HALL_STATE_4;
      break;

    case STATE_5:
      state = EGL_BLDC_HALL_STATE_5;
      break;

    case STATE_6:
      state = EGL_BLDC_HALL_STATE_6;
      break;
    }

  return state;
}

static void deinit_exti(void)
{
  static const EXTI_InitTypeDef config = 
  {
    .EXTI_Line = HALL_1_EXTI | 
                 HALL_2_EXTI | 
                 HALL_3_EXTI,
    .EXTI_Line = DISABLE
  };

  EXTI_Init((EXTI_InitTypeDef *)&config);
}

static void deinit_nvic(void)
{
  NVIC_InitTypeDef config = 
  {
    .NVIC_IRQChannel = EXTI0_1_IRQn,
    .NVIC_IRQChannelCmd = DISABLE
  };

  NVIC_Init(&config);  

  config.NVIC_IRQChannel = EXTI4_15_IRQn;  
  NVIC_Init(&config);  
}

static void deinit(void)
{
  deinit_exti();
  deinit_nvic();
}

const egl_bldc_hall_t hall_impl =
{
  .init   = init,
  .get    = get,
  .deinit = deinit,
};

egl_bldc_hall_t* hall(void)
{
  return (egl_bldc_hall_t *) &hall_impl;
}
