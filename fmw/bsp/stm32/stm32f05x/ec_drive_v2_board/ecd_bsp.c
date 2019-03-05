#include "egl_lib.h"
#include "ecd_bsp.h"

#define CRC_POLY                  (0x1021)
#define CRC_START_VAL             (0xFFFF)

void ecd_bsp_init(void)
{
  ecd_clock_init();
  ecd_main_timer_init();
  
  egl_counter_init(ecd_runtime());
  egl_led_init    (ecd_led());
  egl_itf_init    (ecd_dbg_usart());
  egl_itf_init    (ecd_man_pwm_ctl());
  egl_itf_init    (ecd_spi());
  egl_bldc_init   (ecd_bldc_motor());
  egl_crc_init    (ecd_crc(), CRC_POLY, CRC_START_VAL);
}
