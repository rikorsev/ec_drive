#include "stm8l15x_conf.h"
#include "bldc_motor.h"
#include "bldc_motor_hall.h"

#define BLDC_MOTOR_MAX_POWER                                  ((uint16_t)0x0135)
#define BLDC_MOTOR_DEFAULT_POWER                              ((uint16_t)0x0000) //??
#define BLDC_MOTOR_DEADTIME                                   (4)

/*timer 1 settings: PWM frequency 52 kHz*/
#define BLDC_MOTOR_TIM1_PERIOD                                (320)
#define BLDC_MOTOR_TIM1_PRESCALER                             (0)
#define BLDC_MOTOR_TIM1_REPTETION_COUNTER                     (0)

/*gpio settings */
#define BLDC_MOTOR_DRIVER_PORT1                               (GPIOD)
#define BLDC_MOTOR_DRIVER_PINS1                               (GPIO_Pin_2 | \
                                                               GPIO_Pin_4 | \
                                                               GPIO_Pin_5 | \
                                                               GPIO_Pin_7)

#define BLDC_MOTOR_DRIVER_PORT2                               (GPIOE)
#define BLDC_MOTOR_DRIVER_PINS2                               (GPIO_Pin_1 | \
                                                               GPIO_Pin_2)

#define BLDC_LEDS_PORT                                        (GPIOC)
#define BLDC_LEDS_PINS                                        (GPIO_Pin_5 | \
                                                               GPIO_Pin_6 | \
                                                               GPIO_Pin_7)

#define BLDC_LEDS_SHIFT                                       (5)

void bldc_pwm_init(void)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, ENABLE);  

  /*Init PWM GPIO*/
  /* TIM1 Channels 1, 2, 3 and 1N configuration: PD2, PD4, PD5 and PD7 */
  GPIO_Init(BLDC_MOTOR_DRIVER_PORT1, BLDC_MOTOR_DRIVER_PINS1, 
	    GPIO_Mode_Out_PP_Low_Fast);

  /* TIM1 Channels 2N and 3N configuration: PE1 and PE2 */
  GPIO_Init(BLDC_MOTOR_DRIVER_PORT2, BLDC_MOTOR_DRIVER_PINS2, 
	    GPIO_Mode_Out_PP_Low_Fast);

  /* TIM1 Break input pin configuration */
  GPIO_Init(GPIOD, GPIO_Pin_6, GPIO_Mode_In_FL_No_IT);

  /* Hall leds init */
  GPIO_Init(BLDC_LEDS_PORT, BLDC_LEDS_PINS, GPIO_Mode_Out_PP_High_Fast);
    
  /*Init PWM timers*/
  /* Time Base configuration */
  TIM1_TimeBaseInit(BLDC_MOTOR_TIM1_PRESCALER, TIM1_CounterMode_Up, 
                    BLDC_MOTOR_TIM1_PERIOD, 
                    BLDC_MOTOR_TIM1_REPTETION_COUNTER);

  /* Channels 1, 2 and 3 Configuration in TIMING mode */
  TIM1_OC1Init(TIM1_OCMode_PWM1, TIM1_OutputState_Disable, 
               TIM1_OutputNState_Disable, BLDC_MOTOR_DEFAULT_POWER,
               TIM1_OCPolarity_Low, TIM1_OCNPolarity_Low, 
               TIM1_OCIdleState_Reset, TIM1_OCNIdleState_Reset);
    
  TIM1_OC2Init(TIM1_OCMode_PWM1, TIM1_OutputState_Disable, 
               TIM1_OutputNState_Disable, BLDC_MOTOR_DEFAULT_POWER,
               TIM1_OCPolarity_Low, TIM1_OCNPolarity_Low, 
               TIM1_OCIdleState_Reset, TIM1_OCNIdleState_Reset);
    
  TIM1_OC3Init(TIM1_OCMode_PWM1, TIM1_OutputState_Disable, 
               TIM1_OutputNState_Disable, BLDC_MOTOR_DEFAULT_POWER,
               TIM1_OCPolarity_Low, TIM1_OCNPolarity_Low, 
               TIM1_OCIdleState_Reset, TIM1_OCNIdleState_Reset);

  /* Automatic Output Enable, Break, dead time and lock configuration*/
  TIM1_BDTRConfig(TIM1_OSSIState_Disable, TIM1_LockLevel_Off, 
                  BLDC_MOTOR_DEADTIME, TIM1_BreakState_Disable, 
                  TIM1_BreakPolarity_Low, TIM1_AutomaticOutput_Disable);

  TIM1_CCPreloadControl(ENABLE);
  TIM1_ITConfig(TIM1_IT_COM, ENABLE);
  
  /* Main Output Enable */
  TIM1_CtrlPWMOutputs(ENABLE);
  TIM1->BKR |= 0x08;
}

void bldc_pwm_start(void)
{
  TIM1_Cmd(ENABLE);
  TIM1_CtrlPWMOutputs(ENABLE);
  TIM1_GenerateEvent(TIM1_EventSource_COM); 
}

void bldc_pwm_stop(void)
{
  TIM1_CtrlPWMOutputs(DISABLE);

  GPIOD->ODR &= 0x4B;
  GPIOE->ODR &= 0xF9;
  TIM1->CCER2 = 0x00;
  TIM1->CCER2 &= 0xF0;

  TIM1_Cmd(DISABLE);
}

void bldc_pwm_set(uint16_t power)
{
  if( BLDC_MOTOR_MAX_POWER < power)
    {
      power = BLDC_MOTOR_MAX_POWER;
    }
  
  TIM1_SetCompare1(power);
  TIM1_SetCompare2(power);
  TIM1_SetCompare3(power);
}

bool bldc_pwm_switch(uint8_t hall_sensors, bldc_dir_t dir)
{
  uint8_t tmp1;
  uint8_t tmp2;
  uint8_t hall_leds = 7; 
  
  //motor_tension_update_speed();
  
  tmp1=0x00;
  tmp2=0x00;
  GPIOD->ODR &= 0x4B;
  GPIOE->ODR &= 0xF9;
  
  /*Start of dead time*/
  TIM1->BKR &= (uint8_t)(~TIM1_BKR_MOE);

  if(dir)
    {
      /* Direction CCW*/
      switch(hall_sensors)
	{
	case HALL_STATE_1:
	  tmp1 = TIM1_CCER1_CC1E|TIM1_CCER1_CC1NE;
	  GPIOE->ODR |= 0x02;
	  hall_leds = 1;
	  break;
  
	case HALL_STATE_2:
	  tmp1 = TIM1_CCER1_CC1E|TIM1_CCER1_CC1NE;                       
	  GPIOE->ODR |= 0x04;
	  hall_leds = 2;
          break;
  
	case HALL_STATE_3:
	  tmp1 = TIM1_CCER1_CC2E|TIM1_CCER1_CC2NE;                       
	  GPIOE->ODR |= 0x04;
	  hall_leds = 3;
          break;
  
	case HALL_STATE_4:
	  tmp1 = TIM1_CCER1_CC2E|TIM1_CCER1_CC2NE;
	  GPIOD->ODR |= 0x80;
	  hall_leds = 4;
          break;
  
	case HALL_STATE_5:
	  tmp2 = TIM1_CCER2_CC3NE|TIM1_CCER2_CC3E;
	  GPIOD->ODR |= 0x80;
	  hall_leds = 5;
          break;
  
	case HALL_STATE_6:
	  tmp2 = TIM1_CCER2_CC3NE|TIM1_CCER2_CC3E;
	  GPIOE->ODR |= 0x02;
	  hall_leds = 6;
          break;
  
	default:
	  return FALSE;
	}
    }
  else
    {
      /* Direction CW  */
      switch(hall_sensors)
	{
	case HALL_STATE_6:
	  tmp1 = TIM1_CCER1_CC2E|TIM1_CCER1_CC2NE;   
	  GPIOE->ODR |= 0x04;
	  hall_leds = 6;
	  break;

	case HALL_STATE_5: 
	  tmp1 = TIM1_CCER1_CC1E|TIM1_CCER1_CC1NE;
	  GPIOE->ODR |= 0x04;
	  hall_leds = 5;
	  break;
	  
	case HALL_STATE_4:
	  tmp1 = TIM1_CCER1_CC1E|TIM1_CCER1_CC1NE;
	  GPIOE->ODR |= 0x02;
	  hall_leds = 4;
	  break;

	case HALL_STATE_3:
	  tmp2 = TIM1_CCER2_CC3NE|TIM1_CCER2_CC3E;
	  GPIOE->ODR |= 0x02;   
	  hall_leds = 3;
	  break;   

	case HALL_STATE_2:
	  tmp2 = TIM1_CCER2_CC3NE|TIM1_CCER2_CC3E;
	  GPIOD->ODR |= 0x80;
	  hall_leds = 2;
	  break;

	case HALL_STATE_1:
	  tmp1 = TIM1_CCER1_CC2E|TIM1_CCER1_CC2NE;  
	  GPIOD->ODR |= 0x80;
	  hall_leds = 1;
	  break;

	default:
	  return FALSE;
	}
    }
  TIM1->CCER1 = tmp1;
  TIM1->CCER2 = tmp2;
  TIM1->EGR = TIM1_EventSource_COM;

  /* update hall leds */
  BLDC_LEDS_PORT->ODR &= ~BLDC_LEDS_PINS;
  BLDC_LEDS_PORT->ODR |= (hall_leds << BLDC_LEDS_SHIFT);
  
  return TRUE;
}

INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_COM_IRQHandler, 23)
{
  /*End of dead time*/
  TIM1->BKR |= TIM1_BKR_MOE;
  TIM1_ClearITPendingBit(TIM1_IT_COM);
}
