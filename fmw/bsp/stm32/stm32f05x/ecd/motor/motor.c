#include "egl_lib.h"

extern egl_bldc_hall_t        hall_impl;
extern egl_bldc_pwm_t         pwm_impl;
extern egl_bldc_load_t        load_impl;
extern egl_bldc_speed_meas_t  speed_impl;

static const egl_bldc_t bldc_motor_impl =
{
  .pwm   = &pwm_impl,
  .hall  = &hall_impl,
  .speed = &speed_impl,
  .load  = &load_impl
};

egl_bldc_t* motor(void)
{
  return (egl_bldc_t *) &bldc_motor_impl;
}
