#include "stm8l15x_conf.h"

#define BLDC_HALL_PORT                   (GPIOE)
#define BLDC_HALL_PINS                   (GPIO_Pin_3 |GPIO_Pin_4 | GPIO_Pin_5)

/* #define MOTOR_TENSION_GET_HALL_SENSORS_STATE()                                 \ */
/*                                              ((motor_hall_states_t)(GPIOE->IDR&\ */
/*                                                MOTOR_TENSION_HALL_SENSORS_PINS)) */
                                               
void bldc_hall_init(void)
{
  /*Hall Sensors pins*/
  GPIO_Init(BLDC_HALL_PORT, BLDC_HALL_PINS, 
	    GPIO_Mode_In_FL_IT);
  
  /*External interrupt Hall Sensor 1*/
  EXTI_SetPinSensitivity(EXTI_Pin_3, EXTI_Trigger_Rising_Falling);
  
  /*External interrupt Hall Sensor 2*/
  EXTI_SetPinSensitivity(EXTI_Pin_4, EXTI_Trigger_Rising_Falling);
  
  /*External interrupt Hall Sensor 3*/
  EXTI_SetPinSensitivity(EXTI_Pin_5, EXTI_Trigger_Rising_Falling);

  /*Set priority for hall sensors interrupt*/
  ITC_SetSoftwarePriority(EXTI0_IRQn, ITC_PriorityLevel_3);
  ITC_SetSoftwarePriority(EXTI1_IRQn, ITC_PriorityLevel_3);
  ITC_SetSoftwarePriority(EXTI2_IRQn, ITC_PriorityLevel_3);

}

uint8_t bldc_hall_get(void)
{
  return BLDC_HALL_PORT->IDR & BLDC_HALL_PINS;
}
