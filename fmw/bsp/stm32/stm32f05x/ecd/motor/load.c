#define EGL_MODULE_NAME "LOAD"

#include "stm32f0xx.h"
#include "egl_lib.h"
#include "ecd_bsp.h"

#define PORT              (GPIOA)
#define PIN               (GPIO_Pin_6)
#define LOAD_ADC          (ADC1)
#define OFFSET            (2048)
#define NUM_OF_AVAREGIN   (100)

static int16_t ref_load = 0;

static void gpio_init(void)
{
  static const GPIO_InitTypeDef config = 
  {
    .GPIO_Pin  = PIN,
    .GPIO_Mode = GPIO_Mode_AN,
    .GPIO_PuPd = GPIO_PuPd_NOPULL
  };
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  GPIO_Init(PORT, (GPIO_InitTypeDef *) &config);  
}

static void adc_init(void)
{
  static const ADC_InitTypeDef config = 
  {
    .ADC_Resolution           = ADC_Resolution_12b,
    .ADC_ContinuousConvMode   = DISABLE,
    .ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None,
    .ADC_ExternalTrigConv     = ADC_ExternalTrigConv_T1_TRGO,
    .ADC_DataAlign            = ADC_DataAlign_Right,
    .ADC_ScanDirection        = ADC_ScanDirection_Upward
  };

  /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  ADC_Init(LOAD_ADC, (ADC_InitTypeDef *)&config);

  /* ADC Calibration */
  ADC_GetCalibrationFactor(LOAD_ADC);

  /* Enable the ADC peripheral */
  ADC_Cmd(LOAD_ADC, ENABLE);     

  while(ADC_GetFlagStatus(LOAD_ADC, ADC_FLAG_ADRDY) == RESET)
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
  
  for(int i = 0; i < NUM_OF_AVAREGIN; i++)
  {
    ADC_StartOfConversion(LOAD_ADC);

    while(ADC_GetFlagStatus(LOAD_ADC, ADC_FLAG_EOC) != SET)
    {
	   /* wait fot the end of convertion */
	  }
      
    load += ADC_GetConversionValue(LOAD_ADC) - ref_load;
  }

  /* Not necessary to stop conversion as Single-shot mode is used */  
  return load / NUM_OF_AVAREGIN;
}

static int16_t get(void)
{
  ADC_StartOfConversion(LOAD_ADC);

  while(ADC_GetFlagStatus(LOAD_ADC, ADC_FLAG_EOC) != SET)
  {
    /* wait fot the end of convertion */
  }

  return ADC_GetConversionValue(LOAD_ADC) - OFFSET;
}

static void start(void)
{
  /* Clear other selected files */
  LOAD_ADC->CHSELR = 0;

  /* Sample rate = 16MHz/(12.5 + 1.5) = 16MHz/14 = 1142857 SPS */
  /* Set up BLDC LOAD MEASURMENT channel */
  ADC_ChannelConfig(LOAD_ADC, ADC_Channel_6, ADC_SampleTime_1_5Cycles);
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

const egl_bldc_load_t load_impl =
{
  .init   = init,
  .start  = start,
  .stop   = stop,
  .update = update,
  .get    = get,
  .deinit = deinit
};
