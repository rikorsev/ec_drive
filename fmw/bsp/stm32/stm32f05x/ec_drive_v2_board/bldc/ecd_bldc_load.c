#define EGL_MODULE_NAME "BLDC:LOAD"

#include "stm32f0xx.h"
#include "egl_lib.h"
#include "ecd_bsp.h"

#define ECD_BLDC_LOAD_PORT              (GPIOA)
#define ECD_BLDC_LOAD_PIN               (GPIO_Pin_6)
#define ECD_BLDC_LOAD_ADC               (ADC1)
#define ECD_BLDC_LOAD_OFFSET            (2048)
#define ECD_BLDC_LOAD_NUM_OF_AVAREGIN   (100)

static int16_t ref_load = 0;

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
  config.ADC_ContinuousConvMode    = DISABLE;
  config.ADC_ExternalTrigConvEdge  = ADC_ExternalTrigConvEdge_None;
  config.ADC_ExternalTrigConv      = ADC_ExternalTrigConv_T1_TRGO;
  config.ADC_DataAlign             = ADC_DataAlign_Right;
  config.ADC_ScanDirection         = ADC_ScanDirection_Upward;
  ADC_Init(ECD_BLDC_LOAD_ADC, &config);

  /* ADC Calibration */
  ADC_GetCalibrationFactor(ECD_BLDC_LOAD_ADC);

  /* Enable the ADC peripheral */
  ADC_Cmd(ECD_BLDC_LOAD_ADC, ENABLE);     

  while(ADC_GetFlagStatus(ECD_BLDC_LOAD_ADC, ADC_FLAG_ADRDY) == RESET)
    {
      /* Wait the ADRDY flag */
    }  
}

static void update(void)
{
  /* Do nothing */
}

static int16_t get_averege(void)
{
  int16_t load = 0;
  
  for(int i = 0; i < ECD_BLDC_LOAD_NUM_OF_AVAREGIN; i++)
  {
    ADC_StartOfConversion(ECD_BLDC_LOAD_ADC);

    while(ADC_GetFlagStatus(ECD_BLDC_LOAD_ADC, ADC_FLAG_EOC) != SET)
    {
	   /* wait fot the end of convertion */
	  }
      
    load += ADC_GetConversionValue(ECD_BLDC_LOAD_ADC) - ref_load;
  }

  /* Not necessary to stop conversion as Single-shot mode is used */  
  return load / ECD_BLDC_LOAD_NUM_OF_AVAREGIN;
}

static int16_t get(void)
{
  ADC_StartOfConversion(ECD_BLDC_LOAD_ADC);

  while(ADC_GetFlagStatus(ECD_BLDC_LOAD_ADC, ADC_FLAG_EOC) != SET)
  {
    /* wait fot the end of convertion */
  }

  return ADC_GetConversionValue(ECD_BLDC_LOAD_ADC) - ECD_BLDC_LOAD_OFFSET;
}

static void start(void)
{
  /* Clear other selected files */
  ECD_BLDC_LOAD_ADC->CHSELR = 0;

  /* Sample rate = 16MHz/(12.5 + 1.5) = 16MHz/14 = 1142857 SPS */
  /* Set up BLDC LOAD MEASURMENT channel */
  ADC_ChannelConfig(ECD_BLDC_LOAD_ADC, ADC_Channel_6, ADC_SampleTime_1_5Cycles);
}

static void stop(void)
{
  /* TBD */
}

static void init(void)
{
  gpio_init();
  adc_init();

  start();
  ref_load = get_averege();
  stop();

  EGL_TRACE_INFO("Reference: %d\r\n", ref_load);
}

static void deinit(void)
{
  /* TBD */
}

egl_bldc_load_t ecd_bldc_load_impl =
  {
    .init   = init,
    .start  = start,
    .stop   = stop,
    .update = update,
    .get    = get,
    .deinit = deinit
  };
