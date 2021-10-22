/*! *********************************************************************************
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2017 NXP
* All rights reserved.
*
* \file
*
* TMR export interface file for ARM CORTEX processor
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef _TMR_ADAPTER_H_
#define _TMR_ADAPTER_H_


/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
***********************************************************************************/
#include "EmbeddedTypes.h"
#include "fsl_device_registers.h"
#include "board.h"



/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/

#ifndef gTimestampUseWtimer_c
#if defined gLoggingActive_d && gTimerMgrLowPowerTimers != 0
#define gTimestampUseWtimer_c           (1)
#else
#if defined mAppUseTickLessMode_c && (mAppUseTickLessMode_c > 0)
#define gTimestampUseWtimer_c           (1)
#endif
#endif
#endif

#if defined mAppUseTickLessMode_c && (mAppUseTickLessMode_c > 0)
#define gTimerMgrLowPowerTimers 1
#endif

#ifdef CPU_JN518X
#if gTimestamp_Enabled_d || gTimerMgrLowPowerTimers
/* */
#ifndef gTimerMgrUseCtimer_c
#define gTimerMgrUseCtimer_c	        (0)
#endif
#ifndef gTimerMgrUseLpcRtc_c
#define gTimerMgrUseLpcRtc_c	        (1)
#endif
#else
#ifndef gTimerMgrUseCtimer_c
#define gTimerMgrUseCtimer_c	        (1)
#endif
#ifndef gTimerMgrUseLpcRtc_c
#define gTimerMgrUseLpcRtc_c	        (0)
#endif
#endif
#ifndef gTimestampUseWtimer_c
#define gTimestampUseWtimer_c           (1)
#endif
#endif


/* Defines which timer resource can be used */
#if (defined (FSL_FEATURE_SOC_CTIMER_COUNT) && (FSL_FEATURE_SOC_CTIMER_COUNT > 0))
#ifndef gTimerMgrUseCtimer_c
#define gTimerMgrUseCtimer_c	(1)
#endif
#endif

#if(defined (FSL_FEATURE_SOC_LPC_RTC_COUNT) && (FSL_FEATURE_SOC_LPC_RTC_COUNT > 0))
#ifndef gTimerMgrUseLpcRtc_c
#define gTimerMgrUseLpcRtc_c	(1)
#endif
#endif

#if(defined (FSL_FEATURE_RTC_HAS_FRC) && (FSL_FEATURE_RTC_HAS_FRC > 0))
#ifndef gTimerMgrUseRtcFrc_c
#define gTimerMgrUseRtcFrc_c	(1)
#endif
#endif

#if(defined (FSL_FEATURE_SOC_FTM_COUNT) && (FSL_FEATURE_SOC_FTM_COUNT > 0))
#ifndef gTimerMgrUseFtm_c
#define gTimerMgrUseFtm_c       (1)
#endif
#endif



#ifndef gStackTimerInstance_c
#define gStackTimerInstance_c  0
#endif

#ifndef gStackTimerChannel_c
#define gStackTimerChannel_c   0
#endif

#ifndef gLptmrInstance_c
#define gLptmrInstance_c       0
#endif

#ifndef gTmrRtcInstance_c
#define gTmrRtcInstance_c      0
#endif

#ifndef gTmrPitInstance_c
#define gTmrPitInstance_c      0
#endif

#define gStackTimer_IsrPrio_c (0x80)

#if gTimerMgrUseLpcRtc_c
#define gStackTimerMaxCountValue_c  0xffff
#else
#define gStackTimerMaxCountValue_c  0xffffffff
#endif
#ifdef CPU_JN518X
#define RTC1KHZTICKS_TO_MILLISECONDS(x)    (((x) *125)>>7)
#define MILLISECONDS_TO_RTC1KHZTICKS(x)   (((x)<<7)/125)
#define TICKS32K_TO_MILLISECONDS(x)   (((((uint64_t)x)*125))>>12) /* mult by 1000 divided by 32768*/
#define MILLISECONDS_TO_TICKS32K(x)   (((((uint64_t)x)<<12))/125)
#endif

/************************************************************************************
*************************************************************************************
* Public types
*************************************************************************************
************************************************************************************/
typedef struct _tmr_adapter_pwm_param_type
{
    uint32_t frequency;
    uint32_t initValue;
}tmr_adapter_pwm_param_t;


/*
 * \brief   Platofrm specific timer ticks type definition
 */
#if FSL_FEATURE_SOC_CTIMER_COUNT || FSL_FEATURE_RTC_HAS_FRC
    typedef uint32_t tmrTimerTicks_t;
#else
    typedef uint16_t tmrTimerTicks_t;
#endif
/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/
void StackTimer_Init(void (*cb)(void));
int  StackTimer_ReInit(void (*cb)(void));
void StackTimer_Enable(void);
void StackTimer_Disable(void);
uint32_t StackTimer_ClearIntFlag(void);
uint32_t StackTimer_GetInputFrequency(void);
uint32_t StackTimer_GetCounterValue(void);
void StackTimer_SetOffsetTicks(uint32_t offset);
void StackTimer_ConsumeIrq(void);

/* FreeRTOS SysTick suppressed tick mode related functions */
void StackTimer_PrePowerDownWakeCounterSet(uint32_t expected_idle_time_ms);

void StackTimer_LowPowerWakeTimerStart(void);      /*< Start the wake timer either RTC 1kHz or WTIMER */
void SysTick_Configure(void);     /*< Setup the Core frequency, the tick duration etc ... */
void SysTickTimeStampLastTick(void);             /*< Remember reference time of previous SysTick interrupt occurrence */
void SysTick_StopAndReadRemainingValue(void);
void SysTick_RestartAfterSleepAbort(void);
uint32_t SysTick_RestartAfterSleep(uint32_t now);

uint32_t PWR_GetTotalSleepDuration32kTicks(uint32_t now);
void StackTimer_ReprogramDeadline(uint32_t sleep_duration_ticks);


/* Uncomment line below if you intend to measure
 * the drift between SysTick count and 32k timer */
//#define PostStepTickAssess 1

void SystickCheckDrift(void);



#ifndef ENABLE_RAM_VECTOR_TABLE
void StackTimer_ISR_withParam(uint32_t param);
#endif

void PWM_Init(uint8_t instance);
void PWM_SetChnCountVal(uint8_t instance, uint8_t channel, tmrTimerTicks_t val);
tmrTimerTicks_t PWM_GetChnCountVal(uint8_t instance, uint8_t channel);
#if !defined(FSL_FEATURE_SOC_CTIMER_COUNT) || (FSL_FEATURE_SOC_CTIMER_COUNT == 0)
void PWM_StartEdgeAlignedLowTrue(uint8_t instance, tmr_adapter_pwm_param_t *param, uint8_t channel);
#endif
#endif /* _TMR_ADAPTER_H_ */
