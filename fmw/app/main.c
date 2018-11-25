#define EGL_MODULE_NAME "MAIN"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "stm32f0xx.h"
#include "egl_lib.h"
#include "ecd_bsp.h"

#define BLDC_LOAD_MAMPS_PER_DIGIT (12)

int32_t convert_to_mamps(int32_t raw)
{
  return raw * BLDC_LOAD_MAMPS_PER_DIGIT;
}

int main(void)
{
  int16_t load = 0;
  
  ecd_bsp_init();

  egl_itf_open(ecd_dbg_usart());
  egl_trace_init(EGL_TRACE_LEVEL_DEBUG, ms, NULL, 0);

  EGL_TRACE_INFO("EC Drive v0.1\r\n");

  egl_led_on(ecd_led());
  
  //egl_bldc_set_dir(ecd_bldc_motor(), EGL_BLDC_MOTOR_DIR_CCW);
  egl_bldc_set_power(ecd_bldc_motor(), 64);
  egl_bldc_start(ecd_bldc_motor());
  
  while(1)
  {
    egl_delay(ms, 100);
    load = egl_bldc_get_load(ecd_bldc_motor());
    EGL_TRACE_INFO("Load: %d mA (%d)\r\n", convert_to_mamps(load), load);    
  }

  return 0;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  __assert_func((const char *)file, line, NULL, NULL);
}
#endif

void __assert_func(const char * file, int line , const char * func, const char *expr)
{
  /* Disable interrupts */
  __disable_irq();

  /* Stop motor if running */
  egl_bldc_stop(ecd_bldc_motor());
  
  /* Swich to polling mode */
  egl_itf_ioctl(ecd_dbg_usart(), ECD_DBG_UART_WRITE_POLLING_IOCTL, NULL, 0);

  /* Trace fail message */
  EGL_TRACE_FAIL("Critical fail! file: %s, line: %d, func: %s, expr: %s\r\n",
                                                     file, line, func, expr);
  
  while (1)
    {
      /* Do nothing */
    }
}
