#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "egl_bldc_motor.h"

void egl_bldc_init(egl_bldc_t *motor)
{
  /* check hall functions */
  assert(motor->hall->init      != NULL);
  assert(motor->hall->get       != NULL);
  assert(motor->hall->deinit    != NULL);

  /*check pwm functions */
  assert(motor->pwm.init        != NULL);
  assert(motor->pwm.start       != NULL);
  assert(motor->pwm.stop        != NULL);
  assert(motor->pwm.set         != NULL);
  assert(motor->pwm.switch_wind != NULL);

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
  motor->hall->init();
  motor->pwm->init();

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
	motor->state = EGL_BLDC_MOTOR_ERROR;
      }
   }  
}
/* TBD: return motor status instead bool */
bool egl_bldc_start(egl_bldc_t *motor)
{
  bool result = false;

  /* TBD: add asserts */
  
  if(motor->state != EGL_BLDC_MOTOR_IN_WORK)
    {
      if(motor->pwm->start() == true)
	{
	  egl_bldc_hall_handler(motor);
	  //motor->speed->start();
	  motor->state = EGL_BLDC_MOTOR_IN_WORK;
	  result = true;
	}
    }
  
  return result;
}

/* TBD: return motor status instead bool */
bool egl_bldc_stop(egl_bldc_t *motor)
{
  bool result = false;

  /* TBD: add asserts */
  
  if(motor->pwm->stop() == true)
    {
      //motor->speed->stop();
      motor->state = EGL_BLDC_MOTOR_READY;
      result = true;
    }
  
  return result;
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
