#include "stm8l15x_conf.h"

#include "bldc_motor.h"
#include "bldc_motor_pwm.h"
#include "bldc_motor_hall.h"
#include "bldc_motor_load.h"
#include "bldc_motor_speed.h"

static bldc_t stm8_bldc_motor =
  {
    .pwm.init = bldc_pwm_init,
    .pwm.start = bldc_pwm_start,
    .pwm.stop = bldc_pwm_stop,
    .pwm.set = bldc_pwm_set,
    .pwm.switch_wind = bldc_pwm_switch,

    .hall.init = bldc_hall_init,
    .hall.get = bldc_hall_get,

    .load.init = bldc_load_init,
    .load.update = bldc_load_update,
    .load.get = bldc_load_get,

    .speed.init = bldc_speed_init,
    .speed.start = bldc_speed_start,
    .speed.stop = bldc_speed_stop,
    .speed.update = bldc_speed_update,
    .speed.get = bldc_speed_get
  };
  
//void stm8_bldc_motor_init(void)
//{
//  bldc_init(&stm8_bldc_motor);
//}

bldc_t *stm8_bldc_motor_get(void)
{
  return &stm8_bldc_motor;
}

INTERRUPT_HANDLER(EXTI3_IRQHandler, 11)
{
  bldc_hall_handler(&stm8_bldc_motor);
  
  /*clear pending bit*/
  EXTI->SR1 = (uint8_t) EXTI_IT_Pin3;
  GPIOC->ODR ^= GPIO_Pin_4;
}

INTERRUPT_HANDLER(EXTI4_IRQHandler, 12)
{
  bldc_hall_handler(&stm8_bldc_motor);
  
   /*clear pending bit*/
  EXTI->SR1 = (uint8_t) EXTI_IT_Pin4;
  GPIOC->ODR ^= GPIO_Pin_4;
}

INTERRUPT_HANDLER(EXTI5_IRQHandler, 13)
{
  bldc_hall_handler(&stm8_bldc_motor);
  
  /*clear pending bit*/
  EXTI->SR1 = (uint8_t) EXTI_IT_Pin5;
  GPIOC->ODR ^= GPIO_Pin_4;
}
