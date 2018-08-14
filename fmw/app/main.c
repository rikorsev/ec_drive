#define EGL_MODULE_NAME "MAIN"

#include <stdbool.h>
#include <stdio.h>

#include "stm32f0xx.h"
#include "egl_lib.h"
#include "ecd_bsp.h"

int main(void)
{
  ecd_bsp_init();

  egl_itf_open(ecd_dbg_usart());
  egl_trace_init(EGL_TRACE_LEVEL_DEBUG, ms, NULL, 0);

  EGL_TRACE_INFO("EC Drive v0.1\r\n");

  egl_bldc_start(ecd_bldc_motor());
  
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  while (1)
  {}
}
#endif
