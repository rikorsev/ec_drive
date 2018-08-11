#include "egl_lib.h"


extern egl_bldc_hall_t ecd_bldc_hall_impl;

egl_bldc_t ecd_blds_impl =
  {
    .pwm   = NULL,
    .hall  = &ecd_bldc_hall_impl,
    .speed = NULL,
    .load  = NULL
  };

egl_bldc_t* ecd_bldc_motor(void)
{
  return &ecd_blds_impl;
}
