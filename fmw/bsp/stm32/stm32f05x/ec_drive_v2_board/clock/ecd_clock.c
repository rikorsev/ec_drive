#include <assert.h>
#include "stm32f0xx.h"

void ecd_clock_init(void)
{
  /* RCC_HSICmd(ENABLE); */

  /* while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) != SET)     */
  /*   { */
  /*     /\* do nothing *\/ */
  /*   } */
    
  /* RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI); */

  RCC_HSEConfig(RCC_HSE_ON);

  if(RCC_WaitForHSEStartUp() != SUCCESS)
    {
      assert(0);
    }

  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);
}
