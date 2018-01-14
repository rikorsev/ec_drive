#include <stdio.h>
#include "stm8l15x_conf.h"

#include "bldc_motor.h"
#include "stm8_bldc_motor.h"

#include "stm8_trace.h"

#define ms                     250

#define LEDS_PORT                     GPIOC
#define GREEN_LED_PIN              GPIO_Pin_4

volatile static uint32_t delay_ms_count = 0;

void clock_init(void)
{
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
  CLK_SYSCLKSourceSwitchCmd(ENABLE);
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI); 

  while (CLK_GetSYSCLKSource() != CLK_SYSCLKSource_HSI)
  { 

  }

}

void timer_init(void)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);
  TIM4_TimeBaseInit(TIM4_Prescaler_64,  ms);
  ITC_SetSoftwarePriority(TIM4_UPD_OVF_TRG_IRQn, ITC_PriorityLevel_2);
  TIM4_ITConfig(TIM4_IT_Update, ENABLE);
  TIM4_Cmd(ENABLE);
}

void gpio_init(void)
{
  GPIO_Init(LEDS_PORT, GREEN_LED_PIN, GPIO_Mode_Out_PP_High_Slow);
}

void delay_ms(uint32_t delay)
{
  delay_ms_count = delay;

  while(delay_ms_count)
    {

    }
}

void blink(void)
{
  delay_ms(1000);
  //GPIO_ToggleBits(LEDS_PORT, GREEN_LED_PIN);
  printf("tick\r\n");
}

int main(void)
{
  /* init section */
  clock_init(); /* let's try without, external clock */
  timer_init();
  gpio_init();

  uart1_tracer_init();
  bldc_init(stm8_bldc_motor_get());
  
  enableInterrupts();
  
  delay_ms(1000);
  
  bldc_set_power(stm8_bldc_motor_get(), 16);
  bldc_start(stm8_bldc_motor_get());

  printf("EC drive 0.1\r\n");

  while(1)
    {
      /* infinit loop */
      blink();
      printf("Hello, world!\r\n");
    }

#ifdef _SDCC_
#pragma save
#pragma disable_warning 126
#endif // _SDCC_
  return 0;
#ifdef _SDCC_
#pragma restore
#endif // _SDCC_
}

INTERRUPT_HANDLER(TIM4_UPD_OVF_TRG_IRQHandler, 25)
{
  delay_ms_count--;
  
  uart1_tracer_flush();

  TIM4_ClearITPendingBit(TIM4_IT_Update);
}

/**
  * @brief TRAP Interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER_TRAP(TRAP_IRQHandler)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  while(1)
  {

  }
}

/**
  * @brief Top Level Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TLI_IRQHandler, 0)

{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  while(1)
  {
    
  }
}
