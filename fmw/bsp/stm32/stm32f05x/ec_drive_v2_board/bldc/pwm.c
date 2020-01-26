#define EGL_MODULE_NAME "BLDC:PWM"

#include "stm32f0xx.h"
#include "egl_lib.h"
#include "ecd_bsp.h"

#define PORT_1        (GPIOA)
#define PORT_2        (GPIOB)

#define GPIO_RCC      (RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB)
#define TIM_RCC       (RCC_APB2Periph_TIM1)

#define C1P_PIN       (GPIO_Pin_8)
#define C1N_PIN       (GPIO_Pin_7)
#define C2P_PIN       (GPIO_Pin_9)
#define C2N_PIN       (GPIO_Pin_0)
#define C3P_PIN       (GPIO_Pin_10)
#define C3N_PIN       (GPIO_Pin_1)

#define C1P_AF_PIN    (GPIO_PinSource8)
#define C1N_AF_PIN    (GPIO_PinSource7)
#define C2P_AF_PIN    (GPIO_PinSource9)
#define C2N_AF_PIN    (GPIO_PinSource0)
#define C3P_AF_PIN    (GPIO_PinSource10)
#define C3N_AF_PIN    (GPIO_PinSource1)


#define TIMER         (TIM1)
#define PERIOD        (320) /* To provide 50kHz PWM frequency (16MHz/50kHz = 320)*/
#define DEADTIME      (4)

static void init_gpio(void)
{
  static GPIO_InitTypeDef config = 
  {
    .GPIO_Pin   = C1P_PIN | C1N_PIN | C2P_PIN | C3P_PIN,
    .GPIO_Mode  = GPIO_Mode_AF,
    .GPIO_Speed = GPIO_Speed_10MHz,
    .GPIO_OType = GPIO_OType_PP,
    .GPIO_PuPd  = GPIO_PuPd_NOPULL
  };

  /* GPIOA and GPIOB clocks enable */
  RCC_AHBPeriphClockCmd(GPIO_RCC, ENABLE);

  /* GPIOA Configuration: Channel 1, 2, 1N and 3 as alternate function push-pull */
  GPIO_Init(PORT_1, &config);

  /* GPIOB Configuration: Channel 2N and 3N as alternate function push-pull */
  config.GPIO_Pin = C3N_PIN | C2N_PIN;
  GPIO_Init(PORT_2, &config);
  
  /* Connect TIM pins to AF2 */
  GPIO_PinAFConfig(PORT_1, C1P_AF_PIN, GPIO_AF_2);
  GPIO_PinAFConfig(PORT_1, C1N_AF_PIN, GPIO_AF_2);
  GPIO_PinAFConfig(PORT_1, C2P_AF_PIN, GPIO_AF_2);
  GPIO_PinAFConfig(PORT_2, C2N_AF_PIN, GPIO_AF_2);
  GPIO_PinAFConfig(PORT_1, C3P_AF_PIN, GPIO_AF_2);
  GPIO_PinAFConfig(PORT_2, C3N_AF_PIN, GPIO_AF_2);
}

static void init_timer(void)
{
  static const TIM_TimeBaseInitTypeDef config_base =
  {
    .TIM_Prescaler         = 0,
    .TIM_CounterMode       = TIM_CounterMode_Up,
    .TIM_Period            = PERIOD,
    .TIM_ClockDivision     = 0,
    .TIM_RepetitionCounter = 0
  };
  static const TIM_OCInitTypeDef config_oc = 
  {
    .TIM_OCMode            = TIM_OCMode_Inactive,
    .TIM_OutputState       = TIM_OutputState_Disable,
    .TIM_OutputNState      = TIM_OutputNState_Disable,
    .TIM_Pulse             = 0,
    .TIM_OCPolarity        = TIM_OCPolarity_High,
    .TIM_OCNPolarity       = TIM_OCNPolarity_High,
    .TIM_OCIdleState       = TIM_OCIdleState_Reset,
    .TIM_OCNIdleState      = TIM_OCNIdleState_Reset
  };
  static const TIM_BDTRInitTypeDef config_bdtr = 
  {
    .TIM_OSSRState         = TIM_OSSRState_Enable,
    .TIM_OSSIState         = TIM_OSSIState_Disable,
    .TIM_LOCKLevel         = TIM_LOCKLevel_1,
    .TIM_DeadTime          = DEADTIME,
    .TIM_Break             = TIM_Break_Disable,
    .TIM_BreakPolarity     = TIM_BreakPolarity_Low,
    .TIM_AutomaticOutput   = TIM_AutomaticOutput_Enable
  };

  /* TIM1 clock enable */
  RCC_APB2PeriphClockCmd(TIM_RCC, ENABLE);

  /* Time Base configuration. PWM Frequency 50kHz*/
  TIM_TimeBaseInit(TIMER, (TIM_TimeBaseInitTypeDef *) &config_base);

  /* Channel 1, 2,3 and 4 Configuration in PWM mode */
  TIM_OC1Init(TIMER, (TIM_OCInitTypeDef *) &config_oc);
  TIM_OC2Init(TIMER, (TIM_OCInitTypeDef *) &config_oc);
  TIM_OC3Init(TIMER, (TIM_OCInitTypeDef *) &config_oc);
  TIM_CCPreloadControl(TIMER, ENABLE);


  /* Automatic Output enable, Break, dead time and lock configuration */
  TIM_BDTRConfig(TIMER, (TIM_BDTRInitTypeDef *) &config_bdtr);

}

static void init(void)
{
  init_gpio();
  init_timer();
} 

static egl_result_t start(void)
{
  /* TIM1 counter enable */
  TIM_Cmd(TIMER, ENABLE);

  /* Main Output Enable */
  TIM_CtrlPWMOutputs(TIMER, ENABLE);
  
  return EGL_SUCCESS;
}

static egl_result_t stop(void)
{
  /* TIM1 counter disable */
  TIM_Cmd(TIMER, DISABLE);

  /* Main Output disable */
  TIM_CtrlPWMOutputs(TIMER, DISABLE);
 
  return EGL_SUCCESS;
}

static egl_result_t set(uint16_t power)
{
  if( PERIOD < power )
  {
    return EGL_OUT_OF_BOUNDARY;
  }
  
  TIM_SetCompare1(TIMER, power);
  TIM_SetCompare2(TIMER, power);
  TIM_SetCompare3(TIMER, power);

  return EGL_SUCCESS;
}

#define TIMER_DISABLE_ALL_OUTPUTS()  TIMER->CCER  &= ~0x5555

/* Channel 1 macro */
#define TIMER_CH1_MODE_RESET()       TIMER->CCMR1 &= ~0x0070
#define TIMER_CH1_MODE_PWM()         TIMER->CCMR1 |=  0x0060
#define TIMER_CH1_MODE_ACTIVE()      TIMER->CCMR1 |=  0x0050
#define TIMER_CH1_MODE_INACTIVE()    TIMER->CCMR1 |=  0x0040
#define TIMER_CH1_POS_ENABLE()       TIMER->CCER  |=  0x0001
#define TIMER_CH1_NEG_ENABLE()       TIMER->CCER  |=  0x0004

/* Channel 2 macro */
#define TIMER_CH2_MODE_RESET()       TIMER->CCMR1 &= ~0x7000
#define TIMER_CH2_MODE_PWM()         TIMER->CCMR1 |=  0x6000
#define TIMER_CH2_MODE_ACTIVE()      TIMER->CCMR1 |=  0x5000
#define TIMER_CH2_MODE_INACTIVE()    TIMER->CCMR1 |=  0x4000     
#define TIMER_CH2_POS_ENABLE()       TIMER->CCER  |=  0x0010
#define TIMER_CH2_NEG_ENABLE()       TIMER->CCER  |=  0x0040

/* Channel 3 macro */
#define TIMER_CH3_MODE_RESET()       TIMER->CCMR2 &= ~0x0070
#define TIMER_CH3_MODE_PWM()         TIMER->CCMR2 |=  0x0060
#define TIMER_CH3_MODE_ACTIVE()      TIMER->CCMR2 |=  0x0050
#define TIMER_CH3_MODE_INACTIVE()    TIMER->CCMR2 |=  0x0040
#define TIMER_CH3_POS_ENABLE()       TIMER->CCER  |=  0x0100
#define TIMER_CH3_NEG_ENABLE()       TIMER->CCER  |=  0x0400

#define TIMER_EVT_BREAK()            TIMER->EGR |= 0x0080
#define TIMER_EVT_COM()              TIMER->EGR |= 0x0020

/* PWM C2P, High C1N */
static inline void swich_cw_stage_1(void)
{
  TIMER_DISABLE_ALL_OUTPUTS();

  /* Channel1 configuration */  
  TIMER_CH1_MODE_RESET();
  TIMER_CH1_MODE_ACTIVE();
  TIMER_CH1_NEG_ENABLE();

  /* Channel2 configuration */
  TIMER_CH2_MODE_RESET();
  TIMER_CH2_MODE_PWM();
  TIMER_CH2_POS_ENABLE();
  TIMER_CH2_NEG_ENABLE();

  /*  Channel3 configuration */
  TIMER_CH3_MODE_RESET();
  TIMER_CH3_MODE_INACTIVE();
}

/* PWM C3P, High C1N */
static inline void swich_cw_stage_2(void)
{
  TIMER_DISABLE_ALL_OUTPUTS();

  /* Channel1 configuration */  
  TIMER_CH1_MODE_RESET();
  TIMER_CH1_MODE_ACTIVE();
  TIMER_CH1_NEG_ENABLE();

  /* Channel2 configuration */
  TIMER_CH2_MODE_RESET();
  TIMER_CH2_MODE_INACTIVE();

  /* Channel3 configuration */
  TIMER_CH3_MODE_RESET();
  TIMER_CH3_MODE_PWM();
  TIMER_CH3_POS_ENABLE();
  TIMER_CH3_NEG_ENABLE();
}

/* PWM C3P, High C2N */
static inline void swich_cw_stage_3(void)
{
  TIMER_DISABLE_ALL_OUTPUTS();

  /* Channel1 configuration */
  TIMER_CH1_MODE_RESET();
  TIMER_CH1_MODE_INACTIVE();

  /* Channel2 configuration */  
  TIMER_CH2_MODE_RESET();
  TIMER_CH2_MODE_ACTIVE();
  TIMER_CH2_NEG_ENABLE();

  /* Channel3 configuration */
  TIMER_CH3_MODE_RESET();
  TIMER_CH3_MODE_PWM();
  TIMER_CH3_POS_ENABLE();
  TIMER_CH3_NEG_ENABLE();
}

/* PWM C1P, High C2N */
static inline void swich_cw_stage_4(void)
{
  TIMER_DISABLE_ALL_OUTPUTS();

  /* Channel1 configuration */
  TIMER_CH1_MODE_RESET();
  TIMER_CH1_MODE_PWM();
  TIMER_CH1_POS_ENABLE();
  TIMER_CH1_NEG_ENABLE();

  /* Channel2 configuration */  
  TIMER_CH2_MODE_RESET();
  TIMER_CH2_MODE_ACTIVE();
  TIMER_CH2_NEG_ENABLE();

  /* Channel3 configuration */
  TIMER_CH3_MODE_RESET();
  TIMER_CH3_MODE_INACTIVE();
}

/* PWM C1P, High C3N */
static inline void swich_cw_stage_5(void)
{
  TIMER_DISABLE_ALL_OUTPUTS();

  /* Channel1 configuration */
  TIMER_CH1_MODE_RESET();
  TIMER_CH1_MODE_PWM();
  TIMER_CH1_POS_ENABLE();
  TIMER_CH1_NEG_ENABLE();

  /* Channel2 configuration */
  TIMER_CH2_MODE_RESET();
  TIMER_CH2_MODE_INACTIVE();

  /* Channel3 configuration */  
  TIMER_CH3_MODE_RESET();
  TIMER_CH3_MODE_ACTIVE();
  TIMER_CH3_NEG_ENABLE();

}

/* PWM C2P, High C3N */
static inline void swich_cw_stage_6(void)
{
  TIMER_DISABLE_ALL_OUTPUTS();

  /* Channel1 configuration */
  TIMER_CH1_MODE_RESET();
  TIMER_CH1_MODE_INACTIVE();

  /* Channel2 configuration */
  TIMER_CH2_MODE_RESET();
  TIMER_CH2_MODE_PWM();
  TIMER_CH2_POS_ENABLE();
  TIMER_CH2_NEG_ENABLE();

  /* Channel3 configuration */  
  TIMER_CH3_MODE_RESET();
  TIMER_CH3_MODE_ACTIVE();
  TIMER_CH3_NEG_ENABLE();
}

/* PWM C1P, High C2N */
static inline void swich_ccw_stage_1(void)
{
  TIMER_DISABLE_ALL_OUTPUTS();

  /* Channel1 configuration */
  TIMER_CH1_MODE_RESET();
  TIMER_CH1_MODE_PWM();
  TIMER_CH1_POS_ENABLE();
  TIMER_CH1_NEG_ENABLE();  

  /* Channel2 configuration */  
  TIMER_CH2_MODE_RESET();
  TIMER_CH2_MODE_ACTIVE();
  TIMER_CH2_NEG_ENABLE();

  /* Channel3 configuration */
  TIMER_CH3_MODE_RESET();
  TIMER_CH3_MODE_INACTIVE();
}

/* PWM C1P, High C3N */
static inline void swich_ccw_stage_2(void)
{
  TIMER_DISABLE_ALL_OUTPUTS();

  /* Channel1 configuration */
  TIMER_CH1_MODE_RESET();
  TIMER_CH1_MODE_PWM();
  TIMER_CH1_POS_ENABLE();
  TIMER_CH1_NEG_ENABLE();  

  /* Channel2 configuration */
  TIMER_CH2_MODE_RESET();
  TIMER_CH2_MODE_INACTIVE();

  /* Channel3 configuration */  
  TIMER_CH3_MODE_RESET();
  TIMER_CH3_MODE_ACTIVE();
  TIMER_CH3_NEG_ENABLE();
}

/* PWM C2P, High C3N */
static inline void swich_ccw_stage_3(void)
{
  TIMER_DISABLE_ALL_OUTPUTS();

  /* Channel1 configuration */
  TIMER_CH1_MODE_RESET();
  TIMER_CH1_MODE_INACTIVE();

  /* Channel2 configuration */
  TIMER_CH2_MODE_RESET();
  TIMER_CH2_MODE_PWM();
  TIMER_CH2_POS_ENABLE();
  TIMER_CH2_NEG_ENABLE();  

  /* Channel3 configuration */  
  TIMER_CH3_MODE_RESET();
  TIMER_CH3_MODE_ACTIVE();
  TIMER_CH3_NEG_ENABLE();
}

/* PWM C2P, High C1N */
static inline void swich_ccw_stage_4(void)
{
  TIMER_DISABLE_ALL_OUTPUTS();

  /* Channel1 configuration */  
  TIMER_CH1_MODE_RESET();
  TIMER_CH1_MODE_ACTIVE();
  TIMER_CH1_NEG_ENABLE();

  /* Channel2 configuration */
  TIMER_CH2_MODE_RESET();
  TIMER_CH2_MODE_PWM();
  TIMER_CH2_POS_ENABLE();
  TIMER_CH2_NEG_ENABLE();

  /* Channel3 configuration */
  TIMER_CH3_MODE_RESET();
  TIMER_CH3_MODE_INACTIVE();  
}
/* PWM C3P, High C1N */
static inline void swich_ccw_stage_5(void)
{
  TIMER_DISABLE_ALL_OUTPUTS();

  /* Channel1 configuration */  
  TIMER_CH1_MODE_RESET();
  TIMER_CH1_MODE_ACTIVE();
  TIMER_CH1_NEG_ENABLE();

  /* Channel2 configuration */
  TIMER_CH2_MODE_RESET();
  TIMER_CH2_MODE_INACTIVE(); 

  /* Channel3 configuration */
  TIMER_CH3_MODE_RESET();
  TIMER_CH3_MODE_PWM();
  TIMER_CH3_POS_ENABLE();
  TIMER_CH3_NEG_ENABLE();
}

/* PWM C3P, High C2N */
static inline void swich_ccw_stage_6(void)
{
  TIMER_DISABLE_ALL_OUTPUTS();

  /* Channel1 configuration */
  TIMER_CH1_MODE_RESET();
  TIMER_CH1_MODE_INACTIVE(); 

  /* Channel2 configuration */  
  TIMER_CH2_MODE_RESET();
  TIMER_CH2_MODE_ACTIVE();
  TIMER_CH2_NEG_ENABLE();

  /* Channel3 configuration */
  TIMER_CH3_MODE_RESET();
  TIMER_CH3_MODE_PWM();
  TIMER_CH3_POS_ENABLE();
  TIMER_CH3_NEG_ENABLE();
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

  TIMER_EVT_BREAK();
  TIMER_EVT_COM();

  return EGL_SUCCESS;
}

static void deinit(void)
{
  /* TBD */
}

const egl_bldc_pwm_t pwm_impl =
{
  .init        = init,
  .start       = start,
  .stop        = stop,
  .set         = set,
  .switch_wind = switch_wind,
  .deinit      = deinit
};
