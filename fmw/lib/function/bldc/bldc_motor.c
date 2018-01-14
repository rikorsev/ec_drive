#include <stdint.h>
#include <stdio.h>
//#include <assert.h> /* assert use vprintf function, as _putchar is not defined I can't use it */
#include <stdbool.h>

#include "bldc_motor.h"

/* motor_interface_t motor_tension_itf =  */
/*   { */
/*     motor_tension_init, */
/*     motor_tension_start, */
/*     motor_tension_stop */
/*   }; */

void bldc_init(bldc_t *motor)
{
  /* check if all function pointers are set */
  /* check pwm functions */
  /* assert(motor->pwm.init && */
  /* 	 motor->pwm.start && */
  /* 	 motor->pwm.stop && */
  /* 	 motor->pwm.set && */
  /* 	 motor->pwm.switch_wind); */

  /* check hall functions */
  /* assert(motor->hall.init && */
  /* 	 motor->hall.get); */

  /* check speed functions */
  /* assert(motor->speed.init && */
  /* 	 motor->speed.start && */
  /* 	 motor->speed.stop && */
  /* 	 motor->speed.update && */
  /* 	 motor->speed.get); */

  /* check load functions */
  /* assert(motor->load.init && */
  /* 	 motor->load.update && */
  /* 	 motor->load.get); */

  /* init motor */
  motor->pwm.init();
  motor->hall.init();
  motor->speed.init();
  motor->load.init();
  motor->state = BLDC_MOTOR_READY;
  motor->dir = BLDC_MOTOR_DIR_CW;
  motor->power = 0;
}

void bldc_hall_handler(bldc_t *motor)
{
  //printf("0x%x\r\n", motor->hall.get());
  if(motor->state == BLDC_MOTOR_IN_WORK)
  {
    motor->speed.update();
    if(false == motor->pwm.switch_wind(motor->hall.get(), motor->dir))
      {
	//motor->state = BLDC_MOTOR_HALL_SENSOR_ERROR;
        //printf("E:0x%x\r\n", motor->hall.get());
      }
   }  
}

bool bldc_start(bldc_t *motor)
{

  if(motor->state == BLDC_MOTOR_IN_WORK)
    {
      return true;
    }

  motor->state = BLDC_MOTOR_IN_WORK;
  //motor->pwm.set(motor->power);
  bldc_hall_handler(motor);
  motor->pwm.start();
  motor->speed.start();
  
  return true;
}

bool bldc_stop(bldc_t *motor)
{
  motor->state = BLDC_MOTOR_READY;
  motor->pwm.stop();
  motor->speed.stop();
  
  return true;
}

void bldc_set_dir(bldc_t *motor, bldc_dir_t dir)
{
  motor->dir = dir;
}

bldc_dir_t bldc_get_dir(bldc_t *motor)
{
  return motor->dir;
}

void bldc_set_power(bldc_t *motor, uint16_t power)
{
  motor->pwm.set(power);
}

uint32_t bldc_get_speed(bldc_t *motor)
{
  return motor->speed.get();
}

uint16_t bldc_get_load(bldc_t *motor)
{
  return motor->load.get();
}

bldc_state_t bldc_get_state(bldc_t *motor)
{
  return motor->state;
}
