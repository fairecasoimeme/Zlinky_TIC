/*
 *
 */
#include "dbg.h"
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_adc.h"
#include "fsl_clock.h"
#include "voltage_drv.h"


#define ADC_Battery_SENSOR_CHANNEL 6

// Comment the define if you want to use specific calibration values or want to debug
#define LOAD_CAL_FROM_FLASH


void Config_ADC(uint32_t delay_value)
{

	adc_conv_seq_config_t adcConvSeqConfigStruct;
	adc_config_t adcConfigStruct;

	adcConfigStruct.clockMode = kADC_ClockSynchronousMode; /* Using sync clock source. */
	adcConfigStruct.clockDividerNumber = 7;
	adcConfigStruct.resolution = kADC_Resolution12bit;
	adcConfigStruct.sampleTimeNumber = 0U;

	ADC_Init(ADC0, &adcConfigStruct);

	adcConvSeqConfigStruct.channelMask = (1U << ADC_Battery_SENSOR_CHANNEL);
	adcConvSeqConfigStruct.triggerMask = 0U;
	adcConvSeqConfigStruct.triggerPolarity = kADC_TriggerPolarityPositiveEdge;
	adcConvSeqConfigStruct.enableSingleStep = false;
	adcConvSeqConfigStruct.enableSyncBypass = false;
	adcConvSeqConfigStruct.interruptMode = kADC_InterruptForEachSequence;
	ADC_SetConvSeqAConfig(ADC0, &adcConvSeqConfigStruct);

	CLOCK_uDelay(delay_value);

	ADC_EnableConvSeqA(ADC0, true); /* Enable the conversion sequence A. */


}

uint16_t Get_BattVolt(void)
{

	Config_ADC(300);

	adc_result_info_t adcResultInfoStruct;
	uint32_t estimated_sum = 0;
	uint16_t voltage = 0;

	for (int i=0;i<8;i++)
	{
		ADC_DoSoftwareTriggerConvSeqA(ADC0);

		while (!ADC_GetChannelConversionResult(ADC0, ADC_Battery_SENSOR_CHANNEL, &adcResultInfoStruct))
		{
		}
		estimated_sum += adcResultInfoStruct.result;
	}
	ADC_Deinit(ADC0);

	voltage = (uint16_t)(estimated_sum / 8);
	voltage = (voltage / 1.14);

	return(voltage);
}
