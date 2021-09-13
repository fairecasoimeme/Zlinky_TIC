/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_adc.h"
#include "fsl_clock.h"
#include "fsl_power.h"

#include "pin_mux.h"
#include "clock_config.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_ADC_BASE ADC0
#define DEMO_ADC_SAMPLE_CHANNEL_NUMBER 0U
#define DEMO_ADC_IRQ_ID ADC0_SEQA_IRQn
#define THREE_HUNDRED_MICROSECONDS 300
#define DEMO_ADC_IRQ_HANDLER_FUNC ADC0_SEQA_IRQHandler
#define DEMO_ADC_CLOCK_DIVIDER 7U

/*******************************************************************************
 * Variables
 ******************************************************************************/
static adc_result_info_t gAdcResultInfoStruct;
adc_result_info_t *volatile gAdcResultInfoPtr = &gAdcResultInfoStruct;
volatile bool gAdcConvSeqAIntFlag;
const uint32_t g_Adc_12bitFullRange = 4096U;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void ADC_Configuration(void);

/*******************************************************************************
 * Code
 ******************************************************************************/
static void ADC_ClockPower_Configuration(void)
{
    /* Enable ADC clock */
    CLOCK_EnableClock(kCLOCK_Adc0);

    /* Power on the ADC converter. */
    POWER_EnablePD(kPDRUNCFG_PD_LDO_ADC_EN);

    /* Enable LDO ADC 1v1 */
    PMC->PDRUNCFG |= PMC_PDRUNCFG_ENA_LDO_ADC_MASK;
}

/*${function:start}*/
/*!
 * @brief Main function
 */
int main(void)
{
    /* Initialize board hardware. */
    /* Security code to allow debug access */
    SYSCON->CODESECURITYPROT = 0x87654320;

    /* reset FLEXCOMM for USART */
    RESET_PeripheralReset(kFC0_RST_SHIFT_RSTn);

    /* Init the boards */
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitPins();

    /* Enable the power and clock for ADC. */
    ADC_ClockPower_Configuration();

    CLOCK_uDelay(10000);
    PRINTF("ADC burst example.\r\n");

#if !(defined(FSL_FEATURE_ADC_HAS_NO_CALIB_FUNC) && FSL_FEATURE_ADC_HAS_NO_CALIB_FUNC)
#if defined(FSL_FEATURE_ADC_HAS_CALIB_REG) && FSL_FEATURE_ADC_HAS_CALIB_REG
    /* Calibration after power up. */
    if (ADC_DoSelfCalibration(DEMO_ADC_BASE))
#else
    uint32_t frequency;

#if defined(SYSCON_ADCCLKDIV_DIV_MASK)
    frequency = CLOCK_GetFreq(DEMO_ADC_CLOCK_SOURCE) / CLOCK_GetClkDivider(kCLOCK_DivAdcClk);
#else
    frequency = CLOCK_GetFreq(DEMO_ADC_CLOCK_SOURCE);
#endif /* SYSCON_ADCCLKDIV_DIV_MASK */
    /* Calibration after power up. */
    if (ADC_DoSelfCalibration(DEMO_ADC_BASE, frequency))
#endif /* FSL_FEATURE_ADC_HAS_CALIB_REG */
    {
        PRINTF("ADC_DoSelfCalibration() Done.\r\n");
    }
    else
    {
        PRINTF("ADC_DoSelfCalibration() Failed.\r\n");
    }
#endif /* FSL_FEATURE_ADC_HAS_NO_CALIB_FUNC */

    ADC_Configuration();

    /* Enable the interrupt. */
    ADC_EnableInterrupts(DEMO_ADC_BASE, kADC_ConvSeqAInterruptEnable);
    NVIC_EnableIRQ(DEMO_ADC_IRQ_ID);

    PRINTF("Configuration Done.\r\n");

#if defined(FSL_FEATURE_ADC_HAS_CTRL_RESOL) & FSL_FEATURE_ADC_HAS_CTRL_RESOL
    PRINTF("ADC Full Range: %d\r\n", g_Adc_12bitFullRange);
#endif /* FSL_FEATURE_ADC_HAS_CTRL_RESOL */
    while (1)
    {
        GETCHAR();

        gAdcConvSeqAIntFlag = false;

        /* ADC_DoSoftwareTriggerConvSeqA(DEMO_ADC_BASE); */
        /* Enable the burst mode and start the conversion. */
        ADC_EnableConvSeqABurstMode(DEMO_ADC_BASE, true);

        while (!gAdcConvSeqAIntFlag)
        {
        }
        PRINTF("gAdcResultInfoStruct.result        = %d\r\n", gAdcResultInfoStruct.result);
        PRINTF("gAdcResultInfoStruct.channelNumber = %d\r\n", gAdcResultInfoStruct.channelNumber);
        /* PRINTF("gAdcResultInfoStruct.overrunFlag   = %d\r\n", gAdcResultInfoStruct.overrunFlag ? 1U : 0U); */
        PRINTF("\r\n");
    }
}

/*
 * ISR for ADC conversion sequence A done.
 */
void DEMO_ADC_IRQ_HANDLER_FUNC(void)
{
    if (kADC_ConvSeqAInterruptFlag == (kADC_ConvSeqAInterruptFlag & ADC_GetStatusFlags(DEMO_ADC_BASE)))
    {
        /* Stop the burst mode and pause the conversion. */
        ADC_EnableConvSeqABurstMode(DEMO_ADC_BASE, false);

        ADC_GetChannelConversionResult(DEMO_ADC_BASE, DEMO_ADC_SAMPLE_CHANNEL_NUMBER, gAdcResultInfoPtr);
        ADC_ClearStatusFlags(DEMO_ADC_BASE, kADC_ConvSeqAInterruptFlag);
        gAdcConvSeqAIntFlag = true;
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

/*
 * Configure the ADC as normal converter in polling mode.
 */
void ADC_Configuration(void)
{
    adc_config_t adcConfigStruct;
    adc_conv_seq_config_t adcConvSeqConfigStruct;

/* Configure the converter. */
#if defined(FSL_FEATURE_ADC_HAS_CTRL_ASYNMODE) & FSL_FEATURE_ADC_HAS_CTRL_ASYNMODE
    adcConfigStruct.clockMode = kADC_ClockSynchronousMode; /* Using sync clock source. */
#endif                                                     /* FSL_FEATURE_ADC_HAS_CTRL_ASYNMODE */
    adcConfigStruct.clockDividerNumber = DEMO_ADC_CLOCK_DIVIDER;
#if defined(FSL_FEATURE_ADC_HAS_CTRL_RESOL) & FSL_FEATURE_ADC_HAS_CTRL_RESOL
    adcConfigStruct.resolution = kADC_Resolution12bit;
#endif /* FSL_FEATURE_ADC_HAS_CTRL_RESOL */
#if defined(FSL_FEATURE_ADC_HAS_CTRL_BYPASSCAL) & FSL_FEATURE_ADC_HAS_CTRL_BYPASSCAL
    adcConfigStruct.enableBypassCalibration = false;
#endif /* FSL_FEATURE_ADC_HAS_CTRL_BYPASSCAL */
#if defined(FSL_FEATURE_ADC_HAS_CTRL_TSAMP) & FSL_FEATURE_ADC_HAS_CTRL_TSAMP
    adcConfigStruct.sampleTimeNumber = 0U;
#endif /* FSL_FEATURE_ADC_HAS_CTRL_TSAMP */
#if defined(FSL_FEATURE_ADC_HAS_CTRL_LPWRMODE) & FSL_FEATURE_ADC_HAS_CTRL_LPWRMODE
    adcConfigStruct.enableLowPowerMode = false;
#endif /* FSL_FEATURE_ADC_HAS_CTRL_LPWRMODE */
#if defined(FSL_FEATURE_ADC_HAS_TRIM_REG) & FSL_FEATURE_ADC_HAS_TRIM_REG
    adcConfigStruct.voltageRange = kADC_HighVoltageRange;
#endif /* FSL_FEATURE_ADC_HAS_TRIM_REG */
    ADC_Init(DEMO_ADC_BASE, &adcConfigStruct);

#if !(defined(FSL_FEATURE_ADC_HAS_NO_INSEL) && FSL_FEATURE_ADC_HAS_NO_INSEL)
    /* Use the temperature sensor input to channel 0. */
    ADC_EnableTemperatureSensor(DEMO_ADC_BASE, false);
#endif /* FSL_FEATURE_ADC_HAS_NO_INSEL. */

    /* Enable channel DEMO_ADC_SAMPLE_CHANNEL_NUMBER's conversion in Sequence A. */
    adcConvSeqConfigStruct.channelMask =
        (1U << DEMO_ADC_SAMPLE_CHANNEL_NUMBER); /* Includes channel DEMO_ADC_SAMPLE_CHANNEL_NUMBER. */
    adcConvSeqConfigStruct.triggerMask      = 0U;
    adcConvSeqConfigStruct.triggerPolarity  = kADC_TriggerPolarityPositiveEdge;
    adcConvSeqConfigStruct.enableSingleStep = false;
    adcConvSeqConfigStruct.enableSyncBypass = false;
    adcConvSeqConfigStruct.interruptMode    = kADC_InterruptForEachSequence;
    ADC_SetConvSeqAConfig(DEMO_ADC_BASE, &adcConvSeqConfigStruct);
#if defined(FSL_FEATURE_ADC_REQUIRE_DELAY) && FSL_FEATURE_ADC_REQUIRE_DELAY
    /* A problem with the ADC requires a delay after setup, see RFT 1340 */
    CLOCK_uDelay(THREE_HUNDRED_MICROSECONDS);
#endif                                       /* FSL_FEATURE_ADC_REQUIRE_DELAY */
    ADC_EnableConvSeqA(DEMO_ADC_BASE, true); /* Enable the conversion sequence A. */
}
