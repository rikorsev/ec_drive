#include "egl_lib.h"

extern egl_bldc_hall_t        ecd_bldc_hall_impl;
extern egl_bldc_pwm_t         ecd_bldc_pwm_impl;
extern egl_bldc_load_t        ecd_bldc_load_impl;
extern egl_bldc_speed_meas_t  ecd_bldc_speed_impl;

egl_bldc_t ecd_bldc_impl =
  {
    .pwm   = &ecd_bldc_pwm_impl,
    .hall  = &ecd_bldc_hall_impl,
    .speed = &ecd_bldc_speed_impl,
    .load  = &ecd_bldc_load_impl
  };

egl_bldc_t* ecd_bldc_motor(void)
{
  return &ecd_bldc_impl;
}
