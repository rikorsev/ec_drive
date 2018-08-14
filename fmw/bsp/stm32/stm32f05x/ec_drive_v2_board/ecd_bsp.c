#include "egl_lib.h"
#include "ecd_bsp.h"

void ecd_bsp_init(void)
{
  ecd_clock_init();
  ecd_main_timer_init();
  
  egl_counter_init(ecd_runtime());
  egl_led_init    (ecd_led());
  egl_itf_init    (ecd_dbg_usart());
  egl_bldc_init   (ecd_bldc_motor());
}
