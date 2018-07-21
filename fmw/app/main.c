#include "stm32f0xx.h"

static volatile uint32_t runtime = 0;

#define LED_PIN  (GPIO_Pin_4)

void gpio_init(void)
{
  GPIO_InitTypeDef gpio = {0};
  /* GPIOA Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  /* Configure PD0 and PD2 in output pushpull mode */
  gpio.GPIO_Pin = LED_PIN;
  gpio.GPIO_Mode = GPIO_Mode_OUT;
  gpio.GPIO_OType = GPIO_OType_PP;
  gpio.GPIO_Speed = GPIO_Speed_2MHz;
  gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &gpio);

}

void timer_init(void)
{
  TIM_TimeBaseInitTypeDef tim = {0};
  NVIC_InitTypeDef nvic;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  
  /* Period 1ms */
  tim.TIM_Prescaler = 0;
  tim.TIM_CounterMode = TIM_CounterMode_Up;
  tim.TIM_Period = 8000;
  tim.TIM_ClockDivision = TIM_CKD_DIV1;
  tim.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM3, &tim);

  /* Enable the TIM2 global Interrupt */
  nvic.NVIC_IRQChannel = TIM3_IRQn;
  nvic.NVIC_IRQChannelPriority = 0;
  nvic.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&nvic);
  
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

  TIM_Cmd(TIM3, ENABLE);
}

void clk_init(void)
{
  RCC_HSICmd(ENABLE);

  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) != SET)    
    {
      /* do nothing */
    }
    
  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
}

void delay_ms(uint32_t delay)
{
  uint32_t target_runtime = runtime + delay;

  while(target_runtime != runtime)
    {
      /* do nothing */
    }
}

int main(void)
{
  clk_init();
  timer_init();
  gpio_init();
  
  while(1)
  {
    GPIO_SetBits(GPIOA, LED_PIN);
    delay_ms(500);
    GPIO_ResetBits(GPIOA, LED_PIN);
    delay_ms(500);
  }

  return 0;
}

void TIM3_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
    {
      runtime++;
      TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
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
