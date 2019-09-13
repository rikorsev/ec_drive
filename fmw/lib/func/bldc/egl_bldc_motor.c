#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "egl_result.h"
#include "egl_bldc_motor.h"

void egl_bldc_init(egl_bldc_t *motor)
{
  /* check hall functions */
  assert(motor->hall             != NULL);
  assert(motor->hall->init       != NULL);
  assert(motor->hall->get        != NULL);
  assert(motor->hall->deinit     != NULL);

  /*check pwm functions */
  assert(motor->pwm              != NULL);
  assert(motor->pwm->init        != NULL);
  assert(motor->pwm->start       != NULL);
  assert(motor->pwm->stop        != NULL);
  assert(motor->pwm->set         != NULL);
  assert(motor->pwm->switch_wind != NULL);
  assert(motor->pwm->deinit      != NULL);
  
  /* check speed functions */
  assert(motor->speed            != NULL);
  assert(motor->speed->init      != NULL);
  assert(motor->speed->start     != NULL);
  assert(motor->speed->stop      != NULL);
  assert(motor->speed->update    != NULL);
  assert(motor->speed->get       != NULL);
  assert(motor->speed->deinit    != NULL);

  /* check load functions */
  assert(motor->load->init       != NULL);
  assert(motor->load->update     != NULL);
  assert(motor->load->get        != NULL);
  assert(motor->load->deinit     != NULL);

  /* init motor */
  motor->hall->init();
  motor->pwm->init();
  motor->load->init();
  motor->speed->init();

  motor->state  = EGL_BLDC_MOTOR_READY;
  motor->dir    = EGL_BLDC_MOTOR_DIR_CW;
  motor->power  = 0;
}

egl_result_t egl_bldc_start(egl_bldc_t *motor)
{
  egl_result_t result = EGL_FAIL;

  assert(motor             != NULL);
  assert(motor->pwm        != NULL);
  assert(motor->pwm->start != NULL);
  
  if(motor->state != EGL_BLDC_MOTOR_IN_WORK)
  {
    result = motor->pwm->start();
    if(result == EGL_SUCCESS)
    {
      motor->state = EGL_BLDC_MOTOR_IN_WORK;
      result = egl_bldc_hall_handler(motor);
      if(result == EGL_SUCCESS)
      {
        motor->speed->start();
      }
      else
      {
        motor->pwm->stop();
        motor->state = EGL_BLDC_MOTOR_ERROR;
      }
    }
  }
  
  return result;
}

egl_result_t egl_bldc_stop(egl_bldc_t *motor)
{
  egl_result_t result = EGL_FAIL;
  
  assert(motor             != NULL);
  assert(motor->pwm        != NULL);
  assert(motor->pwm->stop  != NULL);

  motor->speed->stop();
  result = motor->pwm->stop();  
  if(result == EGL_SUCCESS)
  {
    motor->state = EGL_BLDC_MOTOR_READY;
  }
  else
  {
    motor->state = EGL_BLDC_MOTOR_ERROR;
  }

  return result;
}

egl_result_t egl_bldc_hall_handler(egl_bldc_t *motor)
{
  egl_result_t result = EGL_SUCCESS;
  
  if(motor->state == EGL_BLDC_MOTOR_IN_WORK)
  {
    motor->speed->update();
    
    result = motor->pwm->switch_wind(motor->hall->get(), motor->dir);
    if(result != EGL_SUCCESS)
    {
      egl_bldc_stop(motor);
      motor->state = EGL_BLDC_MOTOR_ERROR;
    }
  }

  return result;
}


void egl_bldc_set_dir(egl_bldc_t *motor, egl_bldc_dir_t dir)
{
  assert(motor != NULL);
  
  motor->dir = dir;
}

egl_bldc_dir_t bldc_get_dir(egl_bldc_t *motor)
{
  assert(motor != NULL);
  
  return motor->dir;
}

egl_result_t egl_bldc_set_power(egl_bldc_t *motor, uint16_t power)
{
  assert(motor             != NULL);
  assert(motor->pwm        != NULL);
  assert(motor->pwm->set   != NULL);
  
  return motor->pwm->set(power);
}

uint32_t egl_bldc_get_speed(egl_bldc_t *motor)
{
  assert(motor             != NULL);
  assert(motor->speed      != NULL);
  assert(motor->speed->get != NULL);
  
  return motor->speed->get();
}

int16_t egl_bldc_get_load(egl_bldc_t *motor)
{
  assert(motor            != NULL);
  assert(motor->load      != NULL);
  assert(motor->load->get != NULL);
  
  return motor->load->get();
}

egl_bldc_state_t egl_bldc_get_state(egl_bldc_t *motor)
{
  return motor->state;
}
