#define EGL_MODULE_NAME "MAIN"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "stm32f0xx.h"
#include "egl_lib.h"
#include "ecd_bsp.h"

#define BLDC_LOAD_MAMPS_PER_DIGIT (12)
#define NUM_OF_APROXIMATIONS      (100)
#define CRC_EXPECTED              (0xD71C)

static const uint8_t test_data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};

static int32_t convert_to_mamps(int32_t raw)
{
  return raw * BLDC_LOAD_MAMPS_PER_DIGIT;
}

static void calc_aprox_motor_params(uint16_t pwm)
{
  int32_t load = 0;
  uint32_t speed = 0;
  
  for(int i = 0; i < NUM_OF_APROXIMATIONS; i++)
    {
      egl_delay(ms, 100);

      /* Get current motor load and speed */
      load += egl_bldc_get_load(ecd_bldc_motor());
      speed += egl_bldc_get_speed(ecd_bldc_motor());
    }
  
  load /= NUM_OF_APROXIMATIONS;
  speed /= NUM_OF_APROXIMATIONS;
  
  EGL_TRACE_INFO("PWM: %d, Load: %d mA, Speed: %d rpm\r\n", pwm, convert_to_mamps(load), speed);
}

int main(void)
{
  uint16_t crc = 0;
  ecd_bsp_init();

  egl_itf_open(ecd_dbg_usart());
  egl_trace_init(EGL_TRACE_LEVEL_DEBUG, ms, NULL, 0);
  
  EGL_TRACE_INFO("EC Drive v0.1\r\n");

  egl_crc_init(egl_crc16_ccitt(), 0, 0xFFFF);
  egl_itf_open(ecd_spi());
  egl_led_on(ecd_led());

  //  crc = egl_crc16_calc(ecd_crc(), test_data, sizeof(test_data));

  //EGL_TRACE_INFO("CRC calc: 0x%04x, exp: 0x%04x\r\n", crc, CRC_EXPECTED);

  crc = egl_crc16_calc(egl_crc16_ccitt(), test_data, sizeof(test_data));

  EGL_TRACE_INFO("CRC calc: 0x%04x, exp: 0x%04x\r\n", crc, CRC_EXPECTED);
  
  while(1)
  {
    
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
