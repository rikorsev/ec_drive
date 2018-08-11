#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "egl_bldc_motor.h"

void egl_bldc_init(egl_bldc_t *motor)
{
  /* check if all function pointers are set */
  /* check pwm functions */
  /* assert(motor->pwm.init && */
  /* 	 motor->pwm.start && */
  /* 	 motor->pwm.stop && */
  /* 	 motor->pwm.set && */
  /* 	 motor->pwm.switch_wind); */

  /* check hall functions */
  assert(motor->hall->init   != NULL);
  assert(motor->hall->get    != NULL);
  assert(motor->hall->deinit != NULL);

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
  //motor->pwm.init();
  motor->hall->init();
  //motor->speed.init();
  //motor->load.init();
  motor->state  = EGL_BLDC_MOTOR_READY;
  motor->dir    = EGL_BLDC_MOTOR_DIR_CW;
  motor->power  = 0;
}

void egl_bldc_hall_handler(egl_bldc_t *motor)
{
  if(motor->state == EGL_BLDC_MOTOR_IN_WORK)
  {
    motor->speed->update();
    if(false == motor->pwm->switch_wind(motor->hall->get(), motor->dir))
      {
	motor->state = EGL_BLDC_MOTOR_HALL_SENSOR_ERROR;
      }
   }  
}

bool egl_bldc_start(egl_bldc_t *motor)
{

  if(motor->state == EGL_BLDC_MOTOR_IN_WORK)
    {
      return true;
    }

  //motor->pwm.set(motor->power);
  egl_bldc_hall_handler(motor);
  motor->pwm->start();
  motor->speed->start();
  motor->state = EGL_BLDC_MOTOR_IN_WORK;
  
  return true;
}

bool egl_bldc_stop(egl_bldc_t *motor)
{
  motor->state = EGL_BLDC_MOTOR_READY;
  motor->pwm->stop();
  motor->speed->stop();
  
  return true;
}

void egl_bldc_set_dir(egl_bldc_t *motor, egl_bldc_dir_t dir)
{
  motor->dir = dir;
}

egl_bldc_dir_t bldc_get_dir(egl_bldc_t *motor)
{
  return motor->dir;
}

void egl_bldc_set_power(egl_bldc_t *motor, uint16_t power)
{
  motor->pwm->set(power);
}

uint32_t egl_bldc_get_speed(egl_bldc_t *motor)
{
  return motor->speed->get();
}

uint16_t egl_bldc_get_load(egl_bldc_t *motor)
{
  return motor->load->get();
}

egl_bldc_state_t egl_bldc_get_state(egl_bldc_t *motor)
{
  return motor->state;
}
