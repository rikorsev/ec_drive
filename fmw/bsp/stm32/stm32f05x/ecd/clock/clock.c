#include <assert.h>
#include "stm32f0xx.h"

void clock_init(void)
{
  RCC_HSEConfig(RCC_HSE_ON);

  if(RCC_WaitForHSEStartUp() != SUCCESS)
  {
    assert(0);
  }

  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);
}
