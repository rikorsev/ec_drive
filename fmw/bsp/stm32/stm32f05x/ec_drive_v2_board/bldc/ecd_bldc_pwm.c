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

static void init(void)
{
  GPIO_InitTypeDef         gpio;
  TIM_TimeBaseInitTypeDef  timer;
  TIM_OCInitTypeDef        tim_oc;
  TIM_BDTRInitTypeDef      bdtr;
  
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
  gpio.GPIO_Speed             = GPIO_Speed_10MHz;
  gpio.GPIO_OType             = GPIO_OType_PP;
  //gpio.GPIO_PuPd              = GPIO_PuPd_DOWN;
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
  tim_oc.TIM_OCMode           = TIM_OCMode_Inactive;
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

  /* Automatic Output enable, Break, dead time and lock configuration */
  bdtr.TIM_OSSRState          = TIM_OSSRState_Enable;
  bdtr.TIM_OSSIState          = TIM_OSSIState_Disable;
  bdtr.TIM_LOCKLevel          = TIM_LOCKLevel_1;
  bdtr.TIM_DeadTime           = ECD_BLDC_PWM_DEADTIME;
  bdtr.TIM_Break              = TIM_Break_Disable;
  bdtr.TIM_BreakPolarity      = TIM_BreakPolarity_Low;
  bdtr.TIM_AutomaticOutput    = TIM_AutomaticOutput_Enable;

  TIM_BDTRConfig(ECD_BLDC_PWM_TIMER, &bdtr);

  TIM_CCPreloadControl(ECD_BLDC_PWM_TIMER, ENABLE);
} 

static egl_result_t bool start(void)
{
  /* TIM1 counter enable */
  TIM_Cmd(ECD_BLDC_PWM_TIMER, ENABLE);

  /* Main Output Enable */
  TIM_CtrlPWMOutputs(ECD_BLDC_PWM_TIMER, ENABLE);
  
  return EGL_SUCCESS;
}

static egl_result_t stop(void)
{
  /* TIM1 counter disable */
  TIM_Cmd(ECD_BLDC_PWM_TIMER, DISABLE);

  /* Main Output disable */
  TIM_CtrlPWMOutputs(ECD_BLDC_PWM_TIMER, DISABLE);
 
  return EGL_SUCCESS;
}

static egl_result_t set(uint16_t power)
{
  if( ECD_BLDC_PWM_PERIOD < power)
    {
      return EGL_OUT_OF_BOUNDARY;
    }
  
  TIM_SetCompare1(ECD_BLDC_PWM_TIMER, power);
  TIM_SetCompare2(ECD_BLDC_PWM_TIMER, power);
  TIM_SetCompare3(ECD_BLDC_PWM_TIMER, power);

  return EGL_SUCCESS;
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
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_OCMode_Inactive);
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
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_OCMode_Inactive);
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
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_OCMode_Inactive);
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
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_OCMode_Inactive);
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
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_OCMode_Inactive);
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
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_OCMode_Inactive);
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
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_OCMode_Inactive);
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
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_OCMode_Inactive);
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
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_OCMode_Inactive);
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
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_3, TIM_OCMode_Inactive);
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
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_2, TIM_OCMode_Inactive);
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
  TIM_SelectOCxM (ECD_BLDC_PWM_TIMER, TIM_Channel_1, TIM_OCMode_Inactive);
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

static const void (* bldc_switc[2][6])(void) =
{
  {
    swich_cw_stage_1,
    swich_cw_stage_2,
    swich_cw_stage_3,
    swich_cw_stage_4,
    swich_cw_stage_5,
    swich_cw_stage_6
  },
  {
    swich_ccw_stage_1,
    swich_ccw_stage_2,
    swich_ccw_stage_3,
    swich_ccw_stage_4,
    swich_ccw_stage_5,
    swich_ccw_stage_6
  }
};

static egl_result_t switch_wind(egl_bldc_hall_state_t hall, egl_bldc_dir_t dir)
{
  if(hall > EGL_BLDC_HALL_STATE_6)
    {
      EGL_TRACE_ERROR("Unknow hall state 0x%x\r\n", hall);
      return EGL_INVALID_PARAM;
    }

  if(dir > EGL_BLDC_MOTOR_DIR_CCW)
    {
      EGL_TRACE_ERROR("Unknow direction 0x%x\r\n", dir);
      return EGL_INVALID_PARAM;
    }

  bldc_switc[dir][hall]();

  TIM_GenerateEvent(ECD_BLDC_PWM_TIMER, TIM_EventSource_Break);
  TIM_GenerateEvent(ECD_BLDC_PWM_TIMER, TIM_EventSource_COM);
  
  return EGL_SUCCESS;
}

static void deinit(void)
{
  /* TBD */
}

egl_bldc_pwm_t ecd_bldc_pwm_impl =
  {
    .init        = init,
    .start       = start,
    .stop        = stop,
    .set         = set,
    .switch_wind = switch_wind,
    .deinit      = deinit
  };
