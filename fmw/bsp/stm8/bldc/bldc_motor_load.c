#include "stm8l15x_conf.h"

#define ADC_CURRENT_MESURMENT_CHANNEL   ADC_Channel_4

static uint16_t bldc_load = 0;

void bldc_load_init(void)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);
  ADC_Init(ADC1, ADC_ConversionMode_Single, ADC_Resolution_12Bit, 
                                                               ADC_Prescaler_2);

  ADC_SamplingTimeConfig(ADC1, ADC_Group_SlowChannels,
                                                    ADC_SamplingTime_384Cycles);
  
  ADC_SamplingTimeConfig(ADC1, ADC_Group_FastChannels,
                                                    ADC_SamplingTime_384Cycles);

  ADC_Cmd(ADC1, ENABLE);
  ADC_ChannelCmd(ADC1, ADC_CURRENT_MESURMENT_CHANNEL, ENABLE);

  /* Enable End of conversion ADC1 Interrupt */
  ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
}

void bldc_load_update(void)
{
  ADC_SoftwareStartConv(ADC1);
}

uint16_t bldc_load_get(void)
{
  return bldc_load;
}

INTERRUPT_HANDLER(ADC1_COMP_IRQHandler, 18)
{
  bldc_load = ADC_GetConversionValue(ADC1);

  ADC_ClearITPendingBit(ADC1,ADC_IT_EOC); 
}
