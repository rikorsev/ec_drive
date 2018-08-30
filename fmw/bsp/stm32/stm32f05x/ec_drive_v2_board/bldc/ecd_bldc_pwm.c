#define EGL_MODULE_NAME "BLDC:PWM"

#include "stm32f0xx.h"
#include "egl_lib.h"
#include "ecd_bsp.h"

#define ECD_BLDC_PWM_1_PORT        (GPIOA)
#define ECD_BLDC_PWM_2_PORT        (GPIOB)

#define ECD_BLDC_PWM_GPIO_RCC      (RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB)
#define ECD_BLDC_PWM_TIM_RCC       (RCC_APB2Periph_TIM1)

#define ECD_BLDC_PWM_C1P_PIN       (GPIO_Pin_8)
#define ECD_BLDC_PWM_C1P_AF_PIN    (GPIO_PinSource8)

#define ECD_BLDC_PWM_C1N_PIN       (GPIO_Pin_7)
#define ECD_BLDC_PWM_C1N_AF_PIN    (GPIO_PinSource7)

#define ECD_BLDC_PWM_C2P_PIN       (GPIO_Pin_9)
#define ECD_BLDC_PWM_C2P_AF_PIN    (GPIO_PinSource9)

#define ECD_BLDC_PWM_C2N_PIN       (GPIO_Pin_0)
#define ECD_BLDC_PWM_C2N_AF_PIN    (GPIO_PinSource0)

#define ECD_BLDC_PWM_C3P_PIN       (GPIO_Pin_10)
#define ECD_BLDC_PWM_C3P_AF_PIN    (GPIO_PinSource10)

#define ECD_BLDC_PWM_C3N_PIN       (GPIO_Pin_1)
#define ECD_BLDC_PWM_C3N_AF_PIN    (GPIO_PinSource1)

#define ECD_BLDC_PWM_TIMER         (TIM1)
#define ECD_BLDC_PWM_PERIOD        (320) /* To provide 50kHz PWM frequency (16MHz/50kHz = 320)*/
#define ECD_BLDC_PWM_DEADTIME      (4)
//#define ECD_BLDC_PWM_IRQ_PRIORITY  (0)

static void init(void)
{
  GPIO_InitTypeDef         gpio;
  TIM_TimeBaseInitTypeDef  timer;
  TIM_OCInitTypeDef        tim_oc;
  TIM_BDTRInitTypeDef      bdtr;
  //NVIC_InitTypeDef         nvic;
  
  /* GPIOA and GPIOB clocks enable */
  RCC_AHBPeriphClockCmd(ECD_BLDC_PWM_GPIO_RCC, ENABLE);

  /* TIM1 clock enable */
  RCC_APB2PeriphClockCmd(ECD_BLDC_PWM_TIM_RCC, ENABLE);

    /* GPIOA Configuration: Channel 1, 2, 1N and 3 as alternate function push-pull */
  gpio.GPIO_Pin               = ECD_BLDC_PWM_C1P_PIN | \
                                ECD_BLDC_PWM_C1N_PIN | \
                                ECD_BLDC_PWM_C2P_PIN | \
                                ECD_BLDC_PWM_C3P_PIN;
  gpio.GPIO_Mode              = GPIO_Mode_AF;
  gpio.GPIO_Speed             = GPIO_Speed_50MHz;
  gpio.GPIO_OType             = GPIO_OType_PP;
  gpio.GPIO_PuPd              = GPIO_PuPd_NOPULL;
  GPIO_Init(ECD_BLDC_PWM_1_PORT, &gpio);

  /* GPIOB Configuration: Channel 2N and 3N as alternate function push-pull */
  gpio.GPIO_Pin = ECD_BLDC_PWM_C3N_PIN | ECD_BLDC_PWM_C2N_PIN;
  GPIO_Init(ECD_BLDC_PWM_2_PORT, &gpio);

 /* Connect TIM pins to AF2 */
  GPIO_PinAFConfig(ECD_BLDC_PWM_1_PORT, ECD_BLDC_PWM_C1P_AF_PIN, GPIO_AF_2);
  GPIO_PinAFConfig(ECD_BLDC_PWM_1_PORT, ECD_BLDC_PWM_C1N_AF_PIN, GPIO_AF_2);
  GPIO_PinAFConfig(ECD_BLDC_PWM_1_PORT, ECD_BLDC_PWM_C2P_AF_PIN, GPIO_AF_2);
  GPIO_PinAFConfig(ECD_BLDC_PWM_2_PORT, ECD_BLDC_PWM_C2N_AF_PIN, GPIO_AF_2);
  GPIO_PinAFConfig(ECD_BLDC_PWM_1_PORT, ECD_BLDC_PWM_C3P_AF_PIN, GPIO_AF_2);
  GPIO_PinAFConfig(ECD_BLDC_PWM_2_PORT, ECD_BLDC_PWM_C3N_AF_PIN, GPIO_AF_2);

   /* Time Base configuration. PWM Frequency 50kHz*/
  timer.TIM_Prescaler         = 0;
  timer.TIM_CounterMode       = TIM_CounterMode_Up;
  timer.TIM_Period            = ECD_BLDC_PWM_PERIOD;
  timer.TIM_ClockDivision     = 0;
  timer.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(ECD_BLDC_PWM_TIMER, &timer);

  /* Channel 1, 2,3 and 4 Configuration in PWM mode */
  tim_oc.TIM_OCMode           = TIM_OCMode_PWM1;
  tim_oc.TIM_OutputState      = TIM_OutputState_Disable;
  tim_oc.TIM_OutputNState     = TIM_OutputNState_Disable;
  tim_oc.TIM_Pulse            = 0;
  tim_oc.TIM_OCPolarity       = TIM_OCPolarity_High;
  tim_oc.TIM_OCNPolarity      = TIM_OCNPolarity_High;
  tim_oc.TIM_OCIdleState      = TIM_OCIdleState_Reset;
  tim_oc.TIM_OCNIdleState     = TIM_OCNIdleState_Reset;

  TIM_OC1Init(ECD_BLDC_PWM_TIMER, &tim_oc);
  TIM_OC2Init(ECD_BLDC_PWM_TIMER, &tim_oc);
  TIM_OC3Init(ECD_BLDC_PWM_TIMER, &tim_oc);

  /* Automatic Output enable, Break, dead time and lock configuration*/
  bdtr.TIM_OSSRState          = TIM_OSSRState_Enable;
  bdtr.TIM_OSSIState          = TIM_OSSIState_Disable;
  bdtr.TIM_LOCKLevel          = TIM_LOCKLevel_1;
  bdtr.TIM_DeadTime           = ECD_BLDC_PWM_DEADTIME;
  bdtr.TIM_Break              = TIM_Break_Disable;
  bdtr.TIM_BreakPolarity      = TIM_BreakPolarity_Low;
  bdtr.TIM_AutomaticOutput    = TIM_AutomaticOutput_Enable;

  TIM_BDTRConfig(ECD_BLDC_PWM_TIMER, &bdtr);

  TIM_CCPreloadControl(ECD_BLDC_PWM_TIMER, ENABLE);

  /* Enable the TIM1 Trigger and commutation interrupt */
  //nvic.NVIC_IRQChannel             = TIM1_BRK_UP_TRG_COM_IRQn;
  //nvic.NVIC_IRQChannelPriority     = ECD_BLDC_PWM_IRQ_PRIORITY;
  //nvic.NVIC_IRQChannelCmd          = ENABLE;
  //NVIC_Init(&nvic);   
} 

static bool start(void)
{
  /* Enable COM IRQ */
  //TIM_ITConfig(ECD_BLDC_PWM_TIMER, TIM_IT_COM, ENABLE);

  /* TIM1 counter enable */
  TIM_Cmd(ECD_BLDC_PWM_TIMER, ENABLE);

  /* perform switching of windings before start */
  //egl_bldc_hall_handler(ecd_bldc_motor());
  
  /* Main Output Enable */
  TIM_CtrlPWMOutputs(ECD_BLDC_PWM_TIMER, ENABLE);
  
  return true;
}

static bool stop(void)
{
    /* Disable COM IRQ */
  //TIM_ITConfig(ECD_BLDC_PWM_TIMER, TIM_IT_COM, DISABLE);

  /* TIM1 counter disable */
  TIM_Cmd(ECD_BLDC_PWM_TIMER, DISABLE);

  /* Main Output disable */
  TIM_CtrlPWMOutputs(ECD_BLDC_PWM_TIMER, DISABLE);
 
  return true;
}

static void set(uint16_t power)
{
  if( ECD_BLDC_PWM_PERIOD < power)
    {
      power = ECD_BLDC_PWM_PERIOD;
    }
  
  TIM_SetCompare1(ECD_BLDC_PWM_TIMER, power);
  TIM_SetCompare2(ECD_BLDC_PWM_TIMER, power);
  TIM_SetCompare3(ECD_BLDC_PWM_TIMER, power);
}

/* PWM C2P, High C1N */
static inline void swich_cw_stage_1(void)
{
  /*  Channel1 configuration */
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_OCMode_Active);
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_CCx_Disable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_CCxN_Enable);

  /*  Channel2 configuration */
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_OCMode_PWM1);
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_CCx_Enable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_CCxN_Enable);

  /*  Channel3 configuration */
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_CCx_Disable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_CCxN_Disable);
}

/* PWM C3P, High C1N */
static inline void swich_cw_stage_2(void)
{
  /*  Channel1 configuration */
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_OCMode_Active);
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_CCx_Disable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_CCxN_Enable);

  /*  Channel2 configuration */
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_CCx_Disable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_CCxN_Disable);

  /*  Channel3 configuration */
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_OCMode_PWM1);
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_CCx_Enable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_CCxN_Enable);
}

/* PWM C3P, High C2N */
static inline void swich_cw_stage_3(void)
{
  /*  Channel1 configuration */
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_CCx_Disable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_CCxN_Disable);

  /*  Channel2 configuration */
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_OCMode_Active);
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_CCx_Disable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_CCxN_Enable);

  /*  Channel3 configuration */
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_OCMode_PWM1);
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_CCx_Enable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_CCxN_Enable);
}

/* PWM C1P, High C2N */
static inline void swich_cw_stage_4(void)
{
  /*  Channel1 configuration */
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_OCMode_PWM1);
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_CCx_Enable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_CCxN_Enable);

  /*  Channel2 configuration */
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_OCMode_Active);
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_CCx_Disable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_CCxN_Enable);

  /*  Channel3 configuration */
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_CCx_Disable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_CCxN_Disable);
}

/* PWM C1P, High C3N */
static inline void swich_cw_stage_5(void)
{
  /*  Channel1 configuration */
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_OCMode_PWM1);
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_CCx_Enable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_CCxN_Enable);

  /*  Channel2 configuration */
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_CCx_Disable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_CCxN_Disable);

  /*  Channel3 configuration */
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_OCMode_Active);
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_CCx_Disable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_CCxN_Enable);
}

/* PWM C2P, High C3N */
static inline void swich_cw_stage_6(void)
{
  /*  Channel1 configuration */
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_CCx_Disable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_CCxN_Disable);

  /*  Channel2 configuration */
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_OCMode_PWM1);
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_CCx_Enable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_CCxN_Enable);

  /*  Channel3 configuration */
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_OCMode_Active);
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_CCx_Disable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_CCxN_Enable);
}

/* PWM C1P, High C2N */
static inline void swich_ccw_stage_1(void)
{
  /*  Channel1 configuration */
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_OCMode_PWM1);
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_CCx_Enable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_CCxN_Enable);

  /*  Channel2 configuration */
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_OCMode_Active);
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_CCx_Disable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_CCxN_Enable);

  /*  Channel3 configuration */
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_CCx_Disable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_CCxN_Disable);
}

/* PWM C1P, High C3N */
static inline void swich_ccw_stage_2(void)
{
  /*  Channel1 configuration */
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_OCMode_PWM1);
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_CCx_Enable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_CCxN_Enable);

  /*  Channel2 configuration */
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_CCx_Disable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_CCxN_Disable);

  /*  Channel3 configuration */
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_OCMode_Active);
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_CCx_Disable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_CCxN_Enable);
}

/* PWM C2P, High C3N */
static inline void swich_ccw_stage_3(void)
{
  /*  Channel1 configuration */
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_CCx_Disable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_CCxN_Disable);

  /*  Channel2 configuration */
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_OCMode_PWM1);
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_CCx_Enable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_CCxN_Enable);

  /*  Channel3 configuration */
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_OCMode_Active);
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_CCx_Disable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_CCxN_Enable);
}

/* PWM C2P, High C1N */
static inline void swich_ccw_stage_4(void)
{
  /*  Channel1 configuration */
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_OCMode_Active);
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_CCx_Disable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_CCxN_Enable);

  /*  Channel2 configuration */
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_OCMode_PWM1);
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_CCx_Enable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_CCxN_Enable);

  /*  Channel3 configuration */
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_CCx_Disable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_CCxN_Disable);
}
/* PWM C3P, High C1N */
static inline void swich_ccw_stage_5(void)
{
  /*  Channel1 configuration */
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_OCMode_Active);
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_CCx_Disable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_CCxN_Enable);

  /*  Channel2 configuration */
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_CCx_Disable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_CCxN_Disable);

  /*  Channel3 configuration */
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_OCMode_PWM1);
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_CCx_Enable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_CCxN_Enable);
}

/* PWM C3P, High C2N */
static inline void swich_ccw_stage_6(void)
{
  /*  Channel1 configuration */
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_CCx_Disable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_CCxN_Disable);

  /*  Channel2 configuration */
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_OCMode_Active);
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_CCx_Disable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_CCxN_Enable);

  /*  Channel3 configuration */
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_OCMode_PWM1);
  TIM_CCxCmd     (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_CCx_Enable);
  TIM_CCxNCmd    (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_CCxN_Enable);
}

static inline bool switch_wind_cw(egl_bldc_hall_state_t hall)
{
  bool result = true;
  
  switch(hall)
    {
    case EGL_BLDC_HALL_STATE_1:
      EGL_TRACE_INFO("CW: Stage 1\r\n");
      swich_cw_stage_1();
      break;
  
    case EGL_BLDC_HALL_STATE_2:
      EGL_TRACE_INFO("CW: Stage 2\r\n");
      swich_cw_stage_2();
      break;

    case EGL_BLDC_HALL_STATE_3:
      EGL_TRACE_INFO("CW: Stage 3\r\n");
      swich_cw_stage_3();
      break;

    case EGL_BLDC_HALL_STATE_4:
      EGL_TRACE_INFO("CW: Stage 4\r\n");
      swich_cw_stage_4();
      break;
       
    case EGL_BLDC_HALL_STATE_5:
      EGL_TRACE_INFO("CW: Stage 5\r\n");
      swich_cw_stage_5();
      break;

    case EGL_BLDC_HALL_STATE_6:
      EGL_TRACE_INFO("CW: Stage 6\r\n");
      swich_cw_stage_6();
      break;

    default:
      EGL_TRACE_ERROR("Unknow hall state 0x%x\r\n", hall);
      result = false;
    }

  return result;
}

static inline bool switch_wind_ccw(egl_bldc_hall_state_t hall)
{
  bool result = true;
  
  switch(hall)
    {
    case EGL_BLDC_HALL_STATE_1:
      EGL_TRACE_INFO("CCW: Stage 1\r\n");
      swich_ccw_stage_1();
      break;
  
    case EGL_BLDC_HALL_STATE_2:
      EGL_TRACE_INFO("CCW: Stage 2\r\n");
      swich_ccw_stage_2();
      break;

    case EGL_BLDC_HALL_STATE_3:
      EGL_TRACE_INFO("CCW: Stage 3\r\n");
      swich_ccw_stage_3();
      break;

    case EGL_BLDC_HALL_STATE_4:
      EGL_TRACE_INFO("CCW: Stage 4\r\n");
      swich_ccw_stage_4();
      break;
       
    case EGL_BLDC_HALL_STATE_5:
      EGL_TRACE_INFO("CCW: Stage 5\r\n");
      swich_ccw_stage_5();
      break;

    case EGL_BLDC_HALL_STATE_6:
      EGL_TRACE_INFO("CCW: Stage 6\r\n");
      swich_ccw_stage_6();
      break;

    default:
      EGL_TRACE_ERROR("Unknow hall state 0x%x\r\n", hall);
      result = false;
    }

  return result;
}

static bool switch_wind(egl_bldc_hall_state_t hall, egl_bldc_dir_t dir)
{
  bool result = true;

  switch(dir)
    {
    case EGL_BLDC_MOTOR_DIR_CW:
      result = switch_wind_cw(hall);
      break;

    case EGL_BLDC_MOTOR_DIR_CCW:
      result = switch_wind_ccw(hall);
      break;

    default:
      EGL_TRACE_ERROR("Unknow direction 0x%x\r\n", dir);
      result = false;
    }

  if(result != false)
    {
      TIM_GenerateEvent(ECD_BLDC_PWM_TIMER, TIM_EventSource_Break);
      TIM_GenerateEvent(ECD_BLDC_PWM_TIMER, TIM_EventSource_COM);
    }
  
  return result;
}


static void deinit(void)
{
  
}

//void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
//{
//  if(TIM_GetITStatus(ECD_BLDC_PWM_TIMER, TIM_IT_COM) == SET)
//    {
//      TIM_ClearITPendingBit(ECD_BLDC_PWM_TIMER, TIM_IT_COM);
//    }
//}

egl_bldc_pwm_t ecd_bldc_pwm_impl =
  {
    .init        = init,
    .start       = start,
    .stop        = stop,
    .set         = set,
    .switch_wind = switch_wind,
    .deinit      = deinit
  };
