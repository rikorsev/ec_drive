#define EGL_MODULE_NAME "BLDC:LOAD"

#include "stm32f0xx.h"
#include "egl_lib.h"
#include "ecd_bsp.h"

#define ECD_BLDC_LOAD_PORT         (GPIOA)
#define ECD_BLDC_LOAD_PIN          (GPIO_Pin_6)
#define ECD_BLDC_LOAD_ADC          (ADC1)
#define ECD_BLDC_LOAD_BUFF_SIZE    (1000)
#define ECD_BLDC_LOAD_ADC_ADDR     ((uint32_t)0x40012440)
#define ECD_BLDC_LOAD_IRQ_PRIO     (1)
#define ECD_BLDC_LOAD_OFFSET       (2048)

static uint16_t ecd_bldc_load_buff[ECD_BLDC_LOAD_BUFF_SIZE] = {0};
static uint16_t ecd_bldc_load = 0;

static void gpio_init(void)
{
  GPIO_InitTypeDef config = {0};
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  config.GPIO_Pin                  = ECD_BLDC_LOAD_PIN;
  config.GPIO_Mode                 = GPIO_Mode_AN;
  config.GPIO_PuPd                 = GPIO_PuPd_NOPULL ;
  GPIO_Init(ECD_BLDC_LOAD_PORT, &config);  
}

static void adc_init(void)
{
  ADC_InitTypeDef config = {0};

  /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  config.ADC_Resolution            = ADC_Resolution_12b;
  config.ADC_ContinuousConvMode    = ENABLE;
  //config.ADC_ContinuousConvMode    = DISABLE;
  config.ADC_ExternalTrigConvEdge  = ADC_ExternalTrigConvEdge_None;
  config.ADC_ExternalTrigConv      = ADC_ExternalTrigConv_T1_TRGO;
  config.ADC_DataAlign             = ADC_DataAlign_Right;
  config.ADC_ScanDirection         = ADC_ScanDirection_Upward;
  ADC_Init(ECD_BLDC_LOAD_ADC, &config);

  /* Sample rate = 16MHz/(12.5 + 1.5) = 16MHz/14 = 1142857 SPS */
  ADC_ChannelConfig(ECD_BLDC_LOAD_ADC, ADC_Channel_6 , ADC_SampleTime_1_5Cycles);

  /* ADC Calibration */
  ADC_GetCalibrationFactor(ECD_BLDC_LOAD_ADC);

  /* Enable the ADC peripheral */
  ADC_Cmd(ECD_BLDC_LOAD_ADC, ENABLE);     

  while(ADC_GetFlagStatus(ECD_BLDC_LOAD_ADC, ADC_FLAG_ADRDY) == RESET)
    {
      /* Wait the ADRDY flag */
    }  

  /* ADC DMA request in circular mode */
  ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);

  /* Enable DMA */
  ADC_DMACmd(ECD_BLDC_LOAD_ADC, ENABLE);
  
  /* ADC1 regular Software Start Conv */ 
  ADC_StartOfConversion(ECD_BLDC_LOAD_ADC);
}

static void dma_init(void)
{
  DMA_InitTypeDef config = {0};
  
  /* DMA1 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);  

  /* DMA1 Channel1 Config */
  //DMA_DeInit(DMA1_Channel1);
  config.DMA_PeripheralBaseAddr       = ECD_BLDC_LOAD_ADC_ADDR;
  config.DMA_MemoryBaseAddr           = (uint32_t)ecd_bldc_load_buff;
  config.DMA_DIR                      = DMA_DIR_PeripheralSRC;
  config.DMA_BufferSize               = ECD_BLDC_LOAD_BUFF_SIZE;
  config.DMA_PeripheralInc            = DMA_PeripheralInc_Disable;
  config.DMA_MemoryInc                = DMA_MemoryInc_Enable;
  config.DMA_PeripheralDataSize       = DMA_PeripheralDataSize_HalfWord;
  config.DMA_MemoryDataSize           = DMA_MemoryDataSize_HalfWord;
  config.DMA_Mode                     = DMA_Mode_Circular;
  config.DMA_Priority                 = DMA_Priority_High;
  config.DMA_M2M                      = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &config);

  DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1, ENABLE);
  
  /* DMA1 Channel1 enable */
  DMA_Cmd(DMA1_Channel1, ENABLE);
}

static void irq_init(void)
{
  NVIC_InitTypeDef config = {0};
  
  config.NVIC_IRQChannel         = DMA1_Channel1_IRQn;
  config.NVIC_IRQChannelPriority = ECD_BLDC_LOAD_IRQ_PRIO;
  config.NVIC_IRQChannelCmd      = ENABLE;
  NVIC_Init(&config);
}

static void init(void)
{
  gpio_init();
  dma_init();
  //  irq_init();
  adc_init();
}

//static void update(void)
//{
//  uint32_t load = 0;
  
//  for(int i = 0; i < ECD_BLDC_LOAD_BUFF_SIZE; i++)
//    {
//      load += ecd_bldc_load_buff[i];
//    }

  /* Calc average load */
//  ecd_bldc_load = ECD_BLDC_LOAD_OFFSET - (uint16_t)(load / ECD_BLDC_LOAD_BUFF_SIZE);
//}

static void update(void)
{
  static i = 0;
  EGL_TRACE_DEBUG("%d: %d\r\n", i, ((int16_t)ecd_bldc_load_buff[i++] - ECD_BLDC_LOAD_OFFSET));
}

//static int16_t get(void)
//{
//  uint32_t load = 0;
//  
//  for(int i = 0; i < ECD_BLDC_LOAD_BUFF_SIZE; i++)
//    {
//      load += ecd_bldc_load_buff[i];
//    }
//
//  EGL_TRACE_DEBUG("Total: %d\r\n", load);
//
//  /* Get average load */
//  load /= ECD_BLDC_LOAD_BUFF_SIZE;
//
//  /* Get load module */
//  if(load > ECD_BLDC_LOAD_OFFSET)
//    {
//      load = load - ECD_BLDC_LOAD_OFFSET;
//    }
//  else
//    {
//      load = ECD_BLDC_LOAD_OFFSET - load;
//    }
//
//  
//  return (uint16_t)load;
//}

static int16_t get(void)
{
  ADC_StartOfConversion(ECD_BLDC_LOAD_ADC);

  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != SET)
    {
      /* wait fot the end of convertion */
    }

  return ECD_BLDC_LOAD_OFFSET - ADC_GetConversionValue(ADC1);
}

static void deinit(void)
{
  /* TBD */
}

egl_bldc_load_t ecd_bldc_load_impl =
  {
    .init   = init,
    .update = update,
    .get    = get,
    .deinit = deinit
  };

void DMA1_Channel1_IRQHandler(void)
{
  if(DMA_GetITStatus(DMA1_IT_TC1) == SET)
    {
      egl_led_on(ecd_led());
      DMA_ClearITPendingBit(DMA1_IT_TC1);
      //egl_led_toggle(ecd_led());
      //update();
      egl_led_off(ecd_led());
    }
}
