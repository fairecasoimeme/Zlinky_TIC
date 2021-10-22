/*! *********************************************************************************
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2017 NXP
* All rights reserved.
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
#include "TimersManager.h"
#include "fsl_wtimer.h"
#include "fsl_device_registers.h"
#include "fsl_common.h"
#include "fsl_os_abstraction.h"

#include "fsl_debug_console.h"
#include <assert.h>
#include "TMR_Adapter.h"

#ifndef DISABLE_TMR_ADAPTER
//#include "board.h"

#ifdef FSL_RTOS_FREE_RTOS
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "portmacro.h"
#include "task.h"
#endif

#if gTimerMgrUseFtm_c
  #include "fsl_ftm.h"
#elif gTimerMgrUseLpcRtc_c
  #include "fsl_rtc.h"
#elif gTimerMgrUseRtcFrc_c
  #include "fsl_rtc.h"
  #include "clock_config.h"
#elif gTimerMgrUseCtimer_c
  #include "fsl_ctimer.h"
#else
   #include "fsl_tpm.h"
#endif

#include "fsl_fmeas.h"
#include "fsl_inputmux.h"
#if defined(cPWR_UsePowerDownMode) && (cPWR_UsePowerDownMode == 1)
#include "PWR_Interface.h"
#endif

#if defined mAppUseTickLessMode_c && (mAppUseTickLessMode_c != 0)
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "portmacro.h"
#endif

#if (defined(gSystickUseWtimer1ForSleepDuration) && gSystickUseWtimer1ForSleepDuration) && (defined(gTimestampUseWtimer_c) && gTimestampUseWtimer_c)
#error "It is recommended to disable gTimestampUseWtimer_c if gSystickUseWtimer1ForSleepDuration is enabled to save power otherwise gSystickUseWtimer1ForSleepDuration should not be defined"
#endif

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
extern void PWR_DisallowDeviceToSleep(void);
extern void PWR_AllowDeviceToSleep(void);

#ifndef FMEAS_SYSCON
#if defined(FSL_FEATURE_FMEAS_USE_ASYNC_SYSCON) && (FSL_FEATURE_FMEAS_USE_ASYNC_SYSCON)
#define FMEAS_SYSCON ASYNC_SYSCON
#else
#define FMEAS_SYSCON SYSCON
#endif
#endif


/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
#if gTimerMgrUseFtm_c
static const IRQn_Type mFtmIrqId[] = FTM_IRQS;
static FTM_Type * mFtmBase[] = FTM_BASE_PTRS;
static const ftm_config_t mFtmConfig = {
    .prescale = kFTM_Prescale_Divide_128,
    .bdmMode = kFTM_BdmMode_0,
    .pwmSyncMode = kFTM_SoftwareTrigger,
    .reloadPoints = 0,
    .faultMode = kFTM_Fault_Disable,
    .faultFilterValue = 0,
    .deadTimePrescale = kFTM_Deadtime_Prescale_1,
    .deadTimeValue = 0,
    .extTriggers = 0,
    .chnlInitState = 0,
    .chnlPolarity = 0,
    .useGlobalTimeBase = 0
};

#elif gTimerMgrUseLpcRtc_c
static const IRQn_Type  mRtcFrIrqId = RTC_IRQn;

#elif gTimerMgrUseRtcFrc_c
static const IRQn_Type  mRtcFrIrqId = RTC_FR_IRQn;
static       RTC_Type   *mRtcBase[] = RTC_BASE_PTRS;

#elif gTimerMgrUseCtimer_c
static const IRQn_Type          mCtimerIrqId[]  = CTIMER_IRQS;
static CTIMER_Type              *mCtimerBase[]  = CTIMER_BASE_PTRS;
static const ctimer_config_t    mCtimerConfig[FSL_FEATURE_SOC_CTIMER_COUNT]
                                                = {{.mode =  kCTIMER_TimerMode,
                                                    .input = kCTIMER_Capture_0,
                                                    .prescale = 0},
                                                   {.mode =  kCTIMER_TimerMode,
                                                    .input = kCTIMER_Capture_0,
                                                    .prescale = 0},
#if(FSL_FEATURE_SOC_CTIMER_COUNT > 2)
                                                   {.mode =  kCTIMER_TimerMode,
                                                    .input = kCTIMER_Capture_0,
                                                    .prescale = 0},
                                                   {.mode =  kCTIMER_TimerMode,
                                                    .input = kCTIMER_Capture_0,
                                                    .prescale = 0}
#endif
};
static ctimer_match_config_t mCtimerMatchConfig = {.enableCounterReset = true,
                                                   .enableCounterStop = true,
                                                   .matchValue = 0xff,
                                                   .outControl = kCTIMER_Output_NoAction,
                                                   .outPinInitState = false,
                                                   .enableInterrupt = true};

/*! @brief List of Timer Match channel interrupts,
 * this can be accessed using the channel number as index*/
static const ctimer_interrupt_enable_t ctimer_match_ch_interrupts[] = {
    kCTIMER_Match0InterruptEnable,
    kCTIMER_Match1InterruptEnable,
    kCTIMER_Match2InterruptEnable,
    kCTIMER_Match3InterruptEnable,
};

#ifndef ENABLE_RAM_VECTOR_TABLE
ctimer_callback_t cTimerCallbacks[2] = {StackTimer_ISR_withParam, NULL};
#endif

#else
static const IRQn_Type mTpmIrqId[] = TPM_IRQS;
static TPM_Type * mTpmBase[] = TPM_BASE_PTRS;
static const tpm_config_t mTpmConfig = {
    .prescale = kTPM_Prescale_Divide_128,
    .useGlobalTimeBase = 0,
    .enableDoze = 0,
    .enableDebugMode = 0,
    .enableReloadOnTrigger = 0,
    .enableStopOnOverflow = 0,
    .enableStartOnTrigger = 0,
    .triggerSelect = kTPM_Trigger_Select_0
};
#endif

#ifdef CPU_JN518X
#if 0
#if (BOARD_TARGET_CPU_FREQ != BOARD_MAINCLK_XTAL32M) && !gClkUseFro32K
#define CoreFreqVariable_c  1
#else
#define CoreFreqVariable_c 0
#endif
#if defined mAppUseTickLessMode_c && (mAppUseTickLessMode_c != 0)
typedef struct {
    uint32_t sleep_start_ts;
    bool     intercept_next_timer_irq;
    uint32_t initial_core_freq;
    uint32_t MaximumPossibleSuppressedTicks;
    uint32_t TimerCountsForOneTick;
    uint32_t StoppedTimerCompensation;
    uint32_t suppressed_ticks_time_msec;
    int32_t  original_tmr_value;
    uint32_t origin_systick_ts;
#if CoreFreqVariable_c
    uint32_t systick_residual_count;
    uint32_t estimated_core_freq;
#endif
    bool cal_ongoing;
} lp_tickless_systick_t;

static lp_tickless_systick_t SysTick_lp_ctx;
#endif
#endif
#endif

#define MILLION 1000000UL
#define MHZ MILLION

static void ConfigureIntHandler(IRQn_Type irqId, void (*cb)(void))
{
    /* Overwrite old ISR */
    OSA_InstallIntHandler(irqId, cb);
    /* set interrupt priority */
    NVIC_SetPriority(irqId, gStackTimer_IsrPrio_c >> (8 - __NVIC_PRIO_BITS));
    NVIC_ClearPendingIRQ(irqId);
    NVIC_EnableIRQ(irqId);
}



#if gTimestampUseWtimer_c
void Timestamp_Init(void)
{
    WTIMER_status_t     timer_status;
    /*
     * WTIMER_Init tests if SYSCON->AHBCLKCTRLS[0] has SYSCON_AHBCLKCTRLSET0_WAKE_UP_TIMERS_CLK_SET_MASK set
     * and WKT_CLK_SEL is 0. Does nothing if both are already set.
     */
    WTIMER_Init();

    /* Check wake timers */
    timer_status = WTIMER_GetStatusFlags(WTIMER_TIMER0_ID);
    if ( timer_status != WTIMER_STATUS_RUNNING )
    {
        /* was not running yet : start free running count */
        WTIMER_StartTimerLarge(WTIMER_TIMER0_ID, WTIMER0_MAX_VALUE);
    }
}

void Timestamp_Deinit(void)
{
    WTIMER_StopTimer(WTIMER_TIMER0_ID);
    /* Attempt to stop WTIMER but doesn't if WKT1 timer is still running */
    WTIMER_DeInit();

}

uint32_t Timestamp_GetCounter32bit(void)
{
    /* WTIMER0 is a 41 bit decrementing counter but if we read only 32 bit its max value is ~0UL.
     */
    return (~0UL- WTIMER_ReadTimer(WTIMER_TIMER0_ID));
}

uint64_t Timestamp_GetCounter64bit(void)
{

    /* WTIMER0 is a 41 bit decrementing counter: if we read its full range
     * its max value is ((1<<41)-1 ).
     */
    return WTIMER0_MAX_VALUE - WTIMER_ReadTimerLarge(WTIMER_TIMER0_ID);
}



uint64_t Timestamp_Get_uSec(void)
{
    uint64_t cnt = Timestamp_GetCounter64bit();
    uint64_t usec = TICKS32kHz_TO_USEC(cnt);
    /* multiplying 2^41 * 15625 is less than 2^55 so less than 2^64 */
    return usec;

}

uint64_t Timestamp_Get_mSec(void)
{
    uint64_t cnt = Timestamp_GetCounter64bit();
    uint64_t msec = TICKS32kHz_TO_MSEC(cnt);
    return msec;
}



#endif

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
void StackTimer_Init(void (*cb)(void))
{
    TMR_DBG_LOG("cb=%x", cb);
    IRQn_Type irqId;
#if gTimerMgrUseFtm_c
    FTM_Type *ftmBaseAddr = (FTM_Type*)mFtmBase[gStackTimerInstance_c];

    FTM_Init(ftmBaseAddr, &mFtmConfig);
    FTM_StopTimer(ftmBaseAddr);
    ftmBaseAddr->MOD = 0xFFFF;
    /* Configure channel to Software compare; output pin not used */
    FTM_SetupOutputCompare(ftmBaseAddr, (ftm_chnl_t)gStackTimerChannel_c, kFTM_NoOutputSignal, 0x01);

    /* Install ISR */
    irqId = mFtmIrqId[gStackTimerInstance_c];
    FTM_EnableInterrupts(ftmBaseAddr, kFTM_TimeOverflowInterruptEnable |  (1 << gStackTimerChannel_c));
#elif gTimerMgrUseLpcRtc_c
    /* RTC time counter has to be stopped before setting the date & time in the TSR register */
    /* RTC clocks dividers */
    CLOCK_SetClkDiv(kCLOCK_DivRtcClk, 32, false);
    CLOCK_SetClkDiv(kCLOCK_DivRtc1HzClk, 1, true);

    RTC_StopTimer(RTC);
    RTC_Init(RTC);

    /* Enable RTC interrupt */
#if gTimestamp_Enabled_d && !gTimestampUseWtimer_c
    RTC_EnableInterrupts(RTC, kRTC_WakeupInterruptEnable | kRTC_AlarmInterruptEnable);
#else
    RTC_EnableInterrupts(RTC, kRTC_WakeupInterruptEnable);
#endif
    irqId = mRtcFrIrqId;
#elif gTimerMgrUseRtcFrc_c
    RTC_Init(RTC);
    /* Enable RTC free running interrupt */
    RTC_EnableInterrupts(RTC, kRTC_FreeRunningInterruptEnable);
    irqId = mRtcFrIrqId;
#elif gTimerMgrUseCtimer_c
    CTIMER_Type *ctimerBaseAddr = mCtimerBase[gStackTimerInstance_c];

    CTIMER_Init(ctimerBaseAddr, &mCtimerConfig[gStackTimerInstance_c]);
    CTIMER_StopTimer(ctimerBaseAddr);

    /* Configure channel to Software compare; output pin not used */
    CTIMER_SetupMatch(ctimerBaseAddr, (ctimer_match_t)gStackTimerChannel_c, &mCtimerMatchConfig);

    /* Install ISR */
    irqId = mCtimerIrqId[gStackTimerInstance_c];
    CTIMER_EnableInterrupts(ctimerBaseAddr, ctimer_match_ch_interrupts[gStackTimerChannel_c]);
#ifndef ENABLE_RAM_VECTOR_TABLE
    CTIMER_RegisterCallBack(ctimerBaseAddr, &cTimerCallbacks[0], kCTIMER_SingleCallback);
#endif

#else
    TPM_Type *tpmBaseAddr = (TPM_Type*)mTpmBase[gStackTimerInstance_c];

    TPM_Init(tpmBaseAddr, &mTpmConfig);
    TPM_StopTimer(tpmBaseAddr);

    /* Set the timer to be in free-running mode */
    tpmBaseAddr->MOD = 0xFFFF;
    /* Configure channel to Software compare; output pin not used */
    TPM_SetupOutputCompare(tpmBaseAddr, (tpm_chnl_t)gStackTimerChannel_c, kTPM_NoOutputSignal, 0x01);

    /* Install ISR */
    irqId = mTpmIrqId[gStackTimerInstance_c];
    TPM_EnableInterrupts(tpmBaseAddr, kTPM_TimeOverflowInterruptEnable | (1 << gStackTimerChannel_c));
#endif

    ConfigureIntHandler( irqId, cb);
}

/*************************************************************************************/
int StackTimer_ReInit(void (*cb)(void))
{

    uint32_t result = 0;

#if gTimerMgrUseLpcRtc_c
    ConfigureIntHandler( mRtcFrIrqId, cb);
#else
    /* If the timer used for TMR is not conserved during power down
     * need to restore all
     */
    StackTimer_Init(cb);
#endif
    TMR_DBG_LOG("cb=%x res=%x", cb, result);
    return result;
}

/*************************************************************************************/
void StackTimer_Enable(void)
{
    TMR_DBG_LOG("");
#if gTimerMgrUseFtm_c
    FTM_StartTimer(mFtmBase[gStackTimerInstance_c], kFTM_SystemClock);
#elif gTimerMgrUseLpcRtc_c
    RTC_StartTimer(RTC);
#elif gTimerMgrUseRtcFrc_c
    RTC_FreeRunningEnable(mRtcBase[0], true);
#elif gTimerMgrUseCtimer_c
    CTIMER_StartTimer(mCtimerBase[gStackTimerInstance_c]);
#else
    TPM_StartTimer(mTpmBase[gStackTimerInstance_c], kTPM_SystemClock);
#endif
}

/*************************************************************************************/
void StackTimer_Disable(void)
{
    TMR_DBG_LOG("");
#if gTimerMgrUseFtm_c
    FTM_StopTimer(mFtmBase[gStackTimerInstance_c]);
#elif gTimerMgrUseLpcRtc_c
    RTC_StopTimer(RTC);
#elif gTimerMgrUseRtcFrc_c
    RTC_FreeRunningEnable(mRtcBase[0], false);
#elif gTimerMgrUseCtimer_c
    CTIMER_StopTimer(mCtimerBase[gStackTimerInstance_c]);
#else
    TPM_StopTimer(mTpmBase[gStackTimerInstance_c]);
#endif
}

/*************************************************************************************/
uint32_t StackTimer_GetInputFrequency(void)
{
    uint32_t prescaller = 0;
    uint32_t refClk     = 0;
    uint32_t result     = 0;
#if gTimerMgrUseFtm_c
    refClk = BOARD_GetFtmClock(gStackTimerInstance_c);
    prescaller = mFtmConfig.prescale;
    result = refClk / (1 << prescaller);
#elif gTimerMgrUseLpcRtc_c
    (void)prescaller; /* unused variables  */
    (void)refClk;     /* suppress warnings */
    result = 1000;    /* The high-resolution RTC timer uses a 1kHz clock */
#elif gTimerMgrUseRtcFrc_c

    (void)prescaller; /* unused variables  */
    (void)refClk;     /* suppress warnings */
  #if (defined(BOARD_XTAL1_CLK_HZ) && (BOARD_XTAL1_CLK_HZ == CLK_XTAL_32KHZ))
    result = CLOCK_GetFreq(kCLOCK_32KClk);  //32,768Khz crystal is used
  #else
    if( RTC->CTRL | RTC_CTRL_CAL_EN_MASK) // is calibration enabled ?
    {
        /* result = 32000 +- ( (32768-32000)*(calculated_ppm / ppm_for_32_000))
         *        = 32000 +- ( 768 * calculated_ppm / 0x6000 )
         *        = 32000 +- (3 * calculated_ppm / 0x60)
         */
        if (RTC->CAL & RTC_CAL_DIR_MASK) //backward calibration
        {
            result = 32000U - ((3 * (RTC->CAL & RTC_CAL_PPM_MASK)) / 0x60);
        }
        else
        {
            result = 32000U + ((3 * (RTC->CAL & RTC_CAL_PPM_MASK)) / 0x60);
        }
    }
    else
    {
        result = CLOCK_GetFreq(kCLOCK_32KClk);  //32,000Khz internal RCO is used
    }
  #endif
#elif gTimerMgrUseCtimer_c
    refClk = BOARD_GetCtimerClock(mCtimerBase[gStackTimerInstance_c]);
    prescaller = mCtimerConfig[gStackTimerInstance_c].prescale;
    result = refClk / (prescaller + 1);
#else
    refClk = BOARD_GetTpmClock(gStackTimerInstance_c);
    prescaller = mTpmConfig.prescale;
    result = refClk / (1 << prescaller);
#endif
    TMR_DBG_LOG("%d", result);
    return result;
}

/*************************************************************************************/
uint32_t StackTimer_GetCounterValue(void)
{
    uint32_t counter_value = 0;
#if gTimerMgrUseFtm_c
    counter_value = mFtmBase[gStackTimerInstance_c]->CNT;
#elif gTimerMgrUseLpcRtc_c
    counter_value = RTC_GetWakeupCount(RTC);
#elif gTimerMgrUseRtcFrc_c
    counter_value = RTC_GetFreeRunningCount(mRtcBase[0]);
#elif gTimerMgrUseCtimer_c
    counter_value = mCtimerBase[gStackTimerInstance_c]->TC;
#else
    counter_value = mTpmBase[gStackTimerInstance_c]->CNT;
#endif
    TMR_DBG_LOG("%d", counter_value);

    return counter_value;
}

/*************************************************************************************/
void StackTimer_SetOffsetTicks(uint32_t offset)
{
#if gTimerMgrUseFtm_c
    FTM_SetupOutputCompare(mFtmBase[gStackTimerInstance_c], (ftm_chnl_t)gStackTimerChannel_c, kFTM_NoOutputSignal, offset);
#elif gTimerMgrUseLpcRtc_c
    RTC_SetWakeupCount(RTC, (uint16_t)offset);
#elif gTimerMgrUseRtcFrc_c
    RTC_SetFreeRunningInterruptThreshold(mRtcBase[0], offset);
#elif gTimerMgrUseCtimer_c
    mCtimerMatchConfig.matchValue = offset;
    CTIMER_SetupMatch(mCtimerBase[gStackTimerInstance_c], (ctimer_match_t)gStackTimerInstance_c, &mCtimerMatchConfig);
#else
    TPM_SetupOutputCompare(mTpmBase[gStackTimerInstance_c], (tpm_chnl_t)gStackTimerChannel_c, kTPM_NoOutputSignal, offset);
#endif
    TMR_DBG_LOG("%d", offset);

}

/*************************************************************************************/
uint32_t StackTimer_ClearIntFlag(void)
{
    uint32_t flags = 0;
#if gTimerMgrUseFtm_c
    flags = FTM_GetStatusFlags(mFtmBase[gStackTimerInstance_c]);
    FTM_ClearStatusFlags(mFtmBase[gStackTimerInstance_c], flags);
#elif gTimerMgrUseLpcRtc_c
    flags = RTC_GetStatusFlags(RTC);
    if(flags & kRTC_WakeupFlag)
        RTC_ClearStatusFlags(RTC, kRTC_WakeupFlag);
    if(flags & kRTC_AlarmFlag)
        RTC_ClearStatusFlags(RTC, kRTC_AlarmFlag);
#elif gTimerMgrUseRtcFrc_c
    flags = RTC_STATUS_FREE_RUNNING_INT_MASK;
    RTC_ClearStatusFlags(mRtcBase[0], flags);
#elif gTimerMgrUseCtimer_c
    flags = CTIMER_GetStatusFlags(mCtimerBase[gStackTimerInstance_c]);
    CTIMER_ClearStatusFlags(mCtimerBase[gStackTimerInstance_c], flags);
#else
    flags = TPM_GetStatusFlags(mTpmBase[gStackTimerInstance_c]);
    TPM_ClearStatusFlags(mTpmBase[gStackTimerInstance_c], flags);
#endif
    TMR_DBG_LOG("flags=%x", flags);
    return flags;
}

#if gTimerMgrUseLpcRtc_c

#ifdef CPU_JN518X

#if 0
#if defined mAppUseTickLessMode_c &&  (mAppUseTickLessMode_c != 0)

#if CoreFreqVariable_c
static void Systick_EstimateCoreClockFreq(void);
static uint32_t SysTick_GetCoreFreqTickReloadValue(void);
#endif


/* A fiddle factor to estimate the number of SysTick counts that would have
   occurred while the SysTick counter is stopped during tickless idle
   calculations. TODO tune */
#define MISSED_COUNTS_FACTOR            ( 45UL )


#if CoreFreqVariable_c
#define SysTickGetCoreFreq()  SysTick_lp_ctx.estimated_core_freq
#else
#define SysTickGetCoreFreq()  SysTick_lp_ctx.initial_core_freq
#endif

/*! *********************************************************************************
* \brief  This function sets the RTC WAKE deadline
*
* \param[in]  expected_suppressed_ticks duration for which FreeRTOS expects SysTick ticks to be suppressed (expressed in msec)
*
* \remarks  There is a need to trim the wake delay and advance it if the OS Expected Idle Time is to exceed its orig
*
********************************************************************************** */
void StackTimer_PrePowerDownWakeCounterSet(uint32_t expected_suppressed_ticks)
{
    uint32_t expected_idle_time_ms;
    /* Make sure the SysTick reload value does not overflow the counter. */
    if( expected_suppressed_ticks > SysTick_lp_ctx.MaximumPossibleSuppressedTicks )
    {
        expected_suppressed_ticks = SysTick_lp_ctx.MaximumPossibleSuppressedTicks;
    }
    expected_idle_time_ms = expected_suppressed_ticks * portTICK_PERIOD_MS;

    SysTick_lp_ctx.original_tmr_value = -1L; /* no TMR running on account of TimerManager */
    SysTick_lp_ctx.suppressed_ticks_time_msec = 0;

    if (expected_idle_time_ms > 0xffff)
        expected_idle_time_ms = 0xffff;

    int32_t rtc_ms_to_ticks32k = MILLISECONDS_TO_TICKS32K(expected_idle_time_ms);

    /* TMR_MGR: Get next timer manager expiry time if any */
    /* OSA_InterruptDisable(); useless since called from masked section already
     * However this code must be called from a critical section
     */
    uint32_t rtc_ctrl = RTC->CTRL & (RTC_CTRL_RTC1KHZ_EN_MASK | RTC_CTRL_RTC_EN_MASK) ;
    if (rtc_ctrl == (RTC_CTRL_RTC1KHZ_EN_MASK | RTC_CTRL_RTC_EN_MASK))
    {
        /* If RTC 1kHz counter is already running it means some timeout
         * is already expected so let it run */
        SysTick_lp_ctx.original_tmr_value = (int32_t)RTC_GetWakeupCount(RTC) << 5;
    }
    /* OSA_InterruptEnable(); ditto */
    /* TMR_MGR: Update RTC Threshold only if RTOS needs to wakeup earlier */
    if((int32_t)rtc_ms_to_ticks32k < SysTick_lp_ctx.original_tmr_value || SysTick_lp_ctx.original_tmr_value <= 0)
    {
        SysTick_lp_ctx.suppressed_ticks_time_msec =  expected_idle_time_ms;
        SysTick_lp_ctx.intercept_next_timer_irq = true;
    }
    else
    {
        SysTick_lp_ctx.intercept_next_timer_irq = false;
    }
#if CoreFreqVariable_c
    if (SysTick_lp_ctx.cal_ongoing)
    {
        Systick_EstimateCoreClockFreq();
        SYSTICK_DBG_LOG("CoreFreq is %d",  SysTick_lp_ctx.estimated_core_freq);

    }
#endif
    TMR_DBG_LOG("expected_idle_time_ms=%d tmrMgrExpiryCnt=%d", expected_idle_time_ms, SysTick_lp_ctx.original_tmr_value);

}

static uint32_t SysTickGetElapsedCyclesSinceLastTick(void)
{
    uint32_t val;

#if 0
    /* Peek SysTick count so as to improve accuracy */
    uint32_t cvr = SysTick->VAL & SysTick_VAL_CURRENT_Msk;
    uint32_t rvr = SysTick->LOAD;
    bool flag = (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0;
    if (flag)
    {
        cvr = SysTick->VAL & SysTick_VAL_CURRENT_Msk;
        val = rvr + (rvr - cvr);
    }
    else
    {
        val = (rvr - cvr); /* number of ticks of core clock */
        flag = (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0;
        if (flag)
        {
            val += rvr;
        }
    }
#else
    /* Peek SysTick count so as to improve accuracy */
    val = (SysTick->LOAD - (SysTick->VAL & SysTick_VAL_CURRENT_Msk)); /* number of ticks of core clock */
#endif
    //TMR_DBG_LOG("elapsed ticks %d", val);

    return val;
}

void SysTick_StopAndReadRemainingValue(void)
{
    uint32_t core_ticks_to_32k_ticks = 0;
    uint32_t val;
    uint32_t core_freq;
    core_freq = SysTickGetCoreFreq();
    /* Stop the SysTick momentarily.  The time the SysTick is stopped for
     * is accounted for as best it can be, but using the tickless mode will
     * inevitably result in some tiny drift of the time maintained by the
     * kernel with respect to calendar time.
     */
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

    val = SysTickGetElapsedCyclesSinceLastTick();
    SysTick_lp_ctx.sleep_start_ts = Timestamp_GetCounter32bit();

    core_ticks_to_32k_ticks =  (uint32_t)(((uint64_t)(val) * 32768) / core_freq);

    TMR_DBG_LOG("elapsed core ticks=%d residual_count=%d", val, core_ticks_to_32k_ticks);
    (void) core_ticks_to_32k_ticks;
#if CoreFreqVariable_c
    /* Store elapsed time since last tick converted in 32k ticks */
    SysTick_lp_ctx.systick_residual_count = core_ticks_to_32k_ticks;
#endif
}


void SysTick_StartFroCalibration(void);

void SysTickSetCal(bool on_noff)
{
    if (on_noff)
    {
        SysTick_lp_ctx.cal_ongoing = true;
        //PWR_DisallowDeviceToSleep();
        SysTick_StartFroCalibration();
    }
    else
    {
        SysTick_lp_ctx.cal_ongoing = false;
        //PWR_AllowDeviceToSleep();
    }
}
#if CoreFreqVariable_c

/* suppose 32MHz crystal is running and FRO48M running */
void SysTick_StartFroCalibration(void)
{
    INPUTMUX_Init(INPUTMUX);

    /* Setup to measure the selected target */
    INPUTMUX_AttachSignal(INPUTMUX, 1U, kINPUTMUX_Xtal32MhzToFreqmeas);
    INPUTMUX_AttachSignal(INPUTMUX, 0U, kINPUTMUX_MainClkToFreqmeas);

    CLOCK_EnableClock(kCLOCK_Fmeas);

    /* Start a measurement cycle and wait for it to complete. If the target
       clock is not running, the measurement cycle will remain active
       forever, so a timeout may be necessary if the target clock can stop */
    FMEAS_StartMeasureWithScale(FMEAS_SYSCON, 17);
}

uint32_t SysTick_CompleteFroCalibration(void)
{
    uint32_t        freqComp    = 0U;
    uint32_t        refCount    = 0U;
    uint32_t        targetCount = 0U;
    uint32_t        freqRef     = CLOCK_GetFreq(kCLOCK_Xtal32M);

    if (FMEAS_IsMeasureComplete(FMEAS_SYSCON))
    {
        /* Wait 2^17 counts of the reference clock : 4096us */
        /* Get computed frequency */
        FMEAS_GetCountWithScale(FMEAS_SYSCON, 17, &refCount, &targetCount);
        freqComp = (uint32_t)(((((uint64_t)freqRef)*refCount)) / targetCount);

        /* Disable the clocks if disable previously */
        CLOCK_DisableClock(kCLOCK_Fmeas);
    }
    return freqComp;
}


void Systick_EstimateCoreClockFreq(void)
{
    if (SysTick_lp_ctx.cal_ongoing)
    {
        uint32_t core_freq = SysTick_CompleteFroCalibration();
        if (core_freq != 0)
        {
            SysTick_lp_ctx.estimated_core_freq = (uint32_t)core_freq;
            SysTick_GetCoreFreqTickReloadValue(); /* Sets TickCountForOneTick */
            TMR_DBG_LOG("core_freq=%d", SysTick_lp_ctx.estimated_core_freq);

            SysTickSetCal(false);
        }
    }
}

uint32_t SysTick_GetCoreFreqTickReloadValue(void)
{
    uint32_t reload_value;
#if CoreFreqVariable_c
    TMR_DBG_LOG("core_freq=%d", SysTick_lp_ctx.estimated_core_freq);
    reload_value = (SysTick_lp_ctx.estimated_core_freq / configTICK_RATE_HZ);

//    if (reload_value > SysTick_lp_ctx.StoppedTimerCompensation)
//        reload_value -= SysTick_lp_ctx.StoppedTimerCompensation;

#else
    reload_value = CLOCK_GetFreq(kCLOCK_CoreSysClk) / configTICK_RATE_HZ;
#endif
    SysTick_lp_ctx.TimerCountsForOneTick = reload_value;
    return reload_value;
}
#endif


void SysTick_RestartAfterSleepAbort(void)
{
    TMR_DBG_LOG("");

    /* Restart from whatever is left in the count register to complete
       this tick period. */
    SysTick->LOAD = SysTick->VAL;

    /* Restart SysTick. */
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

    /* Reset the reload register to the value required for normal tick
       periods. */
    SysTick->LOAD = SysTick_lp_ctx.TimerCountsForOneTick - 1UL;

}

uint32_t  SysTick_RestartAfterSleep(uint32_t now)
{
    uint32_t nb_system_ticks;
    uint32_t remainder = SysTick_lp_ctx.TimerCountsForOneTick;
    uint32_t core_freq; /* won't change in his function */
    uint32_t elapsed_32k_ticks;

    core_freq = SysTickGetCoreFreq();

    uint32_t systick_cnt = OSA_GetTickCount();
    uint64_t prev_tick_ts = ((uint64_t)systick_cnt * 32768) / configTICK_RATE_HZ;

    prev_tick_ts += SysTick_lp_ctx.origin_systick_ts;
    if (now > (uint32_t)prev_tick_ts)
    {
        uint32_t systick_ts;
        elapsed_32k_ticks = (now - (uint32_t)prev_tick_ts);
        nb_system_ticks = (elapsed_32k_ticks * configTICK_RATE_HZ) / 32768;
        systick_ts = (systick_cnt + nb_system_ticks) * 32768 / configTICK_RATE_HZ;

        remainder -= (((now - systick_ts)*core_freq) / 32768);

        TMR_DBG_LOG("systick_cnt=%d cur_tick=%d nb_ticks=%d", systick_cnt, (uint32_t)prev_tick_ts, nb_system_ticks);
    }
    else
    {
        /* Can only less than one tick */
        nb_system_ticks = 0;
        elapsed_32k_ticks = (uint32_t)prev_tick_ts - now;

        /* Make the next SysTick duration a bit longer to adapt */
        remainder += (elapsed_32k_ticks * core_freq) / 32768;
        TMR_DBG_LOG("systick_cnt=%d  cur_tick=%d adv_remainder=%d", systick_cnt, (uint32_t)prev_tick_ts, remainder);
    }
    SysTick->VAL = 0U;
    /* First reload with adjusted counter value : the first count down will be with this value*/
    SysTick->LOAD = (remainder-1);
    /* Enable SysTick IRQ and SysTick Timer */
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
    /* Then prepare reload value for next time  */
    SysTick->LOAD = (SysTick_lp_ctx.TimerCountsForOneTick-1);
    return nb_system_ticks;
}


void StackTimer_ReprogramDeadline(uint32_t sleep_duration_ticks)
{
    if (SysTick_lp_ctx.intercept_next_timer_irq)
    {
        /* Silently consume the wake timer interrupt and do not propagate event to TMR_Task */
        StackTimer_ConsumeIrq();
        /*
         * If we have not changed the RTC wake for the tickless mode,
         * let the counter live as is. It has kept counting down.
         * Otherwise we need to reprogram it with the remaining time
         */
        if (SysTick_lp_ctx.original_tmr_value != -1)
        {
            int32_t programmed_cnt_32k_ticks = SysTick_lp_ctx.original_tmr_value;
            /* If there was a TimersManager running before Sleep, restart it */
            /* sleep_duration is in 32kHz ticks */
            programmed_cnt_32k_ticks -= sleep_duration_ticks;
            if (programmed_cnt_32k_ticks > 0)
            {
                uint32_t val = programmed_cnt_32k_ticks >> 5;
                /* val is the number of 1024Hz ticks */
                TMR_DBG_LOG("Reprogram TMR cnt=%d", val);
                RTC_SetWakeupCount(RTC, (uint16_t)val);
            }
            else
            {
                /* Timer Manager has a deadline too close to RTOS tick
                 * Let RTC interrupt fire since time already elapsed
                 */
                TMR_DBG_LOG("late expiry");
            }
        }
    }
}


uint32_t PWR_GetTotalSleepDuration32kTicks(uint32_t start_of_sleep)
{
    uint32_t ticks;
    OSA_InterruptDisable();
#ifdef gTimestampUseWtimer_c
    ticks = Timestamp_GetCounter32bit();
    if (ticks > start_of_sleep)
    {
        ticks = (ticks - start_of_sleep);
    }
    else
    {
        ticks = (~0UL - start_of_sleep) + ticks + 1;
    }
    /* time_delta is a number of 32kHz ticks: convert to milliseconds */
#else
    /* the counter is counting down so previous value is greater.
     * already expressed in 1kHz ticks */
    ticks = RTC_GetWakeupCount(RTC);
    if (ticks > start_of_sleep)
    {
        ticks = (ticks - start_of_sleep);
    }
    else
    {
        ticks = 0x10000 - start_of_sleep + ticks;
    }
    ticks <<=5;
#endif
    OSA_InterruptEnable();

    TMR_DBG_LOG("ticks32k=%d", ticks);
    return (uint32_t)ticks;
}


void SysTick_Configure(void)
{
    /* Calculate the constants required to configure the tick interrupt. */
    SysTick_lp_ctx.initial_core_freq = CLOCK_GetFreq(kCLOCK_CoreSysClk);
#if CoreFreqVariable_c
    SysTick_lp_ctx.estimated_core_freq = SysTick_lp_ctx.initial_core_freq;
    SysTick_lp_ctx.TimerCountsForOneTick = ( SysTick_lp_ctx.estimated_core_freq / configTICK_RATE_HZ );
    SysTickSetCal(true);
#else
    SysTick_lp_ctx.TimerCountsForOneTick = ( SysTick_lp_ctx.initial_core_freq / configTICK_RATE_HZ );
    SysTick_lp_ctx.cal_ongoing = false;
#endif
#if gTimerMgrUseLpcRtc_c
    SysTick_lp_ctx.MaximumPossibleSuppressedTicks = (((1 << 16) - 1) / 1024) * configTICK_RATE_HZ;
#elif gTimerMgrUseWtimer_c
    SysTick_lp_ctx.MaximumPossibleSuppressedTicks = (((1U << 28) - 1) / 32768) *  configTICK_RATE_HZ;
#else
    #error "Should define either gTimerMgrUseLpcRtc_c or gTimerMgrUseWtimer_c"
#endif
    SysTick_lp_ctx.StoppedTimerCompensation = MISSED_COUNTS_FACTOR;

    /* Stop and clear the SysTick. */
    SysTick->CTRL = 0UL;
    SysTick->VAL = 0UL;
    SysTick_lp_ctx.origin_systick_ts = Timestamp_GetCounter32bit();

    /* Configure SysTick to interrupt at the requested rate. */
    SysTick_Config( SysTick_lp_ctx.TimerCountsForOneTick );
}
#endif

/*---------------------------------------------------------------------------
* Name: RTC_SetWakeupTimeMs
* Description: -
* Parameters: wakeupTimeMs: New wakeup time in milliseconds
* Return: -
*---------------------------------------------------------------------------*/
static void RTC_SetWakeupTimeMs (uint32_t wakeupTimeMs)
{
    TMR_DBG_LOG("timer ms=%d", wakeupTimeMs);
    /* Since our 32kHz clock is 32768Hz, its divider by 32 yields a 1.024kHz clock
     * instead of 1kHz so adjust the count value */
    /* Count down of 1kHz clock */
    wakeupTimeMs = MILLISECONDS_TO_RTC1KHZTICKS(wakeupTimeMs);
    if (wakeupTimeMs > 0xffff)
        wakeupTimeMs = 0xffff;
    RTC_SetWakeupCount(RTC, (uint16_t)wakeupTimeMs);
}



static void RTC_WakeupStart(void)
{
    TMR_DBG_LOG("");
    uint32_t enabled_interrupts = RTC_GetEnabledInterrupts(RTC);
    enabled_interrupts |= kRTC_WakeupInterruptEnable;
    RTC_EnableInterrupts(RTC, enabled_interrupts);
}

bool SysTickFreqCalOngoing(void)
{
    return SysTick_lp_ctx.cal_ongoing;
}

void StackTimer_LowPowerWakeTimerStart(void)
{
    /* No need to store previous value of RTC IRQ priority: already done by Save_CM4_registers
     * that saves the Interrupt priorities for all IRQs */
    if (NVIC_GetPendingIRQ(RTC_IRQn))
    {
        RTC_ClearStatusFlags(RTC, kRTC_WakeupFlag);
        NVIC_ClearPendingIRQ(RTC_IRQn);
    }
    NVIC_SetPriority(RTC_IRQn, 0);  /* Force RTC IRQ to raise its priority required ??? */
    /* Configure RTC 1kHz wakeup */
    if(SysTick_lp_ctx.suppressed_ticks_time_msec > 0)
    {
        /* If suppressed_ticks_time_msec is 0 it means the TimersManager original counter is still running */
        RTC_SetWakeupTimeMs(SysTick_lp_ctx.suppressed_ticks_time_msec);
    }
    RTC_WakeupStart();
}



void SystickCheckDrift(void)
{
#if PostStepTickAssess
  #if defined SYSTICK_DBG && (SYSTICK_DBG != 0) || gLogginActive_d

    #if defined SYSTICK_DBG && (SYSTICK_DBG != 0)
    static int32_t prev_delta_ticks = -1;
    #endif
    uint32_t cnt;
    uint32_t os_msec;
    uint32_t cnt_msec;
    int32_t delta;

    OSA_InterruptDisable();
    /* Peek both time reference values under critical section */
    cnt = Timestamp_GetCounter32bit() - SysTick_lp_ctx.origin_systick_ts;
    os_msec = OSA_TimeGetMsec();
    OSA_InterruptEnable();

    cnt_msec = TICKS32K_TO_MILLISECONDS(cnt);

    if (cnt_msec > os_msec)
    {
        delta = cnt_msec - os_msec;
    }
    else
    {
        delta = os_msec - cnt_msec;
    }
    TMR_DBG_LOG("os_msec=%d delta=%d",os_msec, delta);
#if defined SYSTICK_DBG && (SYSTICK_DBG != 0)
    uint32_t sec = RTC_GetTimeSeconds(RTC);

    int32_t nb_TICK = delta / portTICK_PERIOD_MS;
    do {
        if (nb_TICK > prev_delta_ticks)
        {
            SYSTICK_DBG_LOG("WARN:sec=%d os_msec=%d cnt_msec=%d delta=%d freq=%d", sec, os_msec, cnt_msec, delta, SysTickGetCoreFreq());
            prev_delta_ticks = nb_TICK;
        }
      #if defined MAX_NB_TICK_DRIFT
        /* We want to assert if this happened */
        if (nb_TICK > MAX_NB_TICK_DRIFT)
        {
#if defined gLoggingActive_d && gLoggingActive_d
            DbgLogDump(true);
#endif
            break;
        }
#endif
    } while(0);
    #endif /* SYSTICK_DBG */

  #endif /* defined SYSTICK_DBG && (SYSTICK_DBG != 0) || gLogginActive_d */
#endif
}

#endif /* defined(cPWR_FullPowerDownMode) && (cPWR_FullPowerDownMode) && (configUSE_TICKLESS_IDLE != 0) */

#endif /* CPU_JN518X */


#ifndef ENABLE_RAM_VECTOR_TABLE
void RTC_IRQHandler(void)
{
    StackTimer_ISR_withParam(0);
}
#endif




void StackTimer_ConsumeIrq(void)
{
    if (NVIC_GetPendingIRQ(RTC_IRQn))
    {
        RTC_ClearStatusFlags(RTC, kRTC_WakeupFlag);
        NVIC_ClearPendingIRQ(RTC_IRQn);
    }
}

#endif

/*************************************************************************************/
/*                                       PWM                                         */
/*************************************************************************************/
#if (FSL_FEATURE_SOC_PWM_COUNT < 1)
void PWM_Init(uint8_t instance)
{
#if gTimerMgrUseFtm_c
    ftm_config_t config;
    FTM_GetDefaultConfig(&config);
    FTM_Init(mFtmBase[instance], &config);
    /* Enable TPM compatibility. Free running counter and synchronization compatible with TPM */
    mFtmBase[instance]->MODE &= ~(FTM_MODE_FTMEN_MASK);
    FTM_StartTimer(mFtmBase[instance], kFTM_SystemClock);
#elif gTimerMgrUseRtcFrc_c
    // do nothing, RTC has no PWM
#elif gTimerMgrUseCtimer_c
    ctimer_config_t config;
    CTIMER_GetDefaultConfig(&config);
    CTIMER_Init(mCtimerBase[gStackTimerInstance_c], &config);
    CTIMER_StartTimer(mCtimerBase[gStackTimerInstance_c]);
#else
    tpm_config_t config;
    TPM_GetDefaultConfig(&config);
    TPM_Init(mTpmBase[instance], &config);
    TPM_StartTimer(mTpmBase[instance], kTPM_SystemClock);
#endif
}

/*************************************************************************************/
void PWM_SetChnCountVal(uint8_t instance, uint8_t channel, tmrTimerTicks_t val)
{
#if gTimerMgrUseFtm_c
    mFtmBase[instance]->CONTROLS[channel].CnV = val;
#elif gTimerMgrUseRtcFrc_c
    // do nothing, RTC has no PWM
#elif gTimerMgrUseCtimer_c
    mCtimerBase[gStackTimerInstance_c]->MR[channel] = val;
#else
    mTpmBase[instance]->CONTROLS[channel].CnV = val;
#endif
}

/*************************************************************************************/
tmrTimerTicks_t PWM_GetChnCountVal(uint8_t instance, uint8_t channel)
{
    tmrTimerTicks_t value = 0;
#if gTimerMgrUseFtm_c
    value = mFtmBase[instance]->CONTROLS[channel].CnV;
#elif gTimerMgrUseRtcFrc_c
    // do nothing, RTC has no PWM
#elif gTimerMgrUseCtimer_c
    value =  mCtimerBase[gStackTimerInstance_c]->MR[channel];
#else
    value = mTpmBase[instance]->CONTROLS[channel].CnV;
#endif
    return value;
}

#if !defined(gTimerMgrUseCtimer_c) || (gTimerMgrUseCtimer_c == 0)
/* For QN9080 CTimer pwm output always starts low and rises on compare match */
/*************************************************************************************/
void PWM_StartEdgeAlignedLowTrue(uint8_t instance, tmr_adapter_pwm_param_t *param, uint8_t channel)
{
#if gTimerMgrUseFtm_c
    ftm_chnl_pwm_signal_param_t pwmChannelConfig = {
        .chnlNumber = (ftm_chnl_t)channel,
        .level = kFTM_LowTrue,
        .dutyCyclePercent = param->initValue,
        .firstEdgeDelayPercent = 0
    };

    FTM_SetupPwm(mFtmBase[instance], &pwmChannelConfig, 1, kFTM_EdgeAlignedPwm, param->frequency, BOARD_GetFtmClock(instance));
#else
    tpm_chnl_pwm_signal_param_t pwmChannelConfig = {
        .chnlNumber = (tpm_chnl_t)channel,
        .level = kTPM_LowTrue,
        .dutyCyclePercent = param->initValue,
#if defined(FSL_FEATURE_TPM_HAS_COMBINE) && FSL_FEATURE_TPM_HAS_COMBINE
        .firstEdgeDelayPercent = 0
#endif
    };

    TPM_SetupPwm(mTpmBase[instance], &pwmChannelConfig, 1, kTPM_EdgeAlignedPwm, param->frequency, BOARD_GetTpmClock(instance));
#endif
}
#endif
#endif
#endif /* DISABLE_TMR_ADAPTER */

static TMR_tsActivityWakeTimerEvent  *psNextActivity = NULL;
static volatile bool_t rejectWtimer1Irq = FALSE;
static bool_t scheduleActivityAlreadyCalled = FALSE;

static void TMR_vUtilSubstractValue(uint32_t *pCurrentValue, uint32_t substractValue)
{
    if (*pCurrentValue > substractValue)
    {
        *pCurrentValue -= substractValue;
    }
    else
    {
        *pCurrentValue = 0;
    }

}

TMR_teActivityStatus TMR_eScheduleActivity32kTicks(TMR_tsActivityWakeTimerEvent *psTmr,
                                                    uint32_t u32Ticks,
                                                    void (*prCallbackfn)(void))
{
    return TMR_eScheduleActivity32kTicksAndGetCurrentTimestampValue(psTmr, u32Ticks, prCallbackfn, NULL);
}

TMR_teActivityStatus TMR_eScheduleActivity32kTicksAndGetCurrentTimestampValue(TMR_tsActivityWakeTimerEvent *psTmr,
                                                    uint32_t u32Ticks,
                                                    void (*prCallbackfn)(void),
                                                    uint32_t *pCurrentTimestampValue)
{
    uint32_t              u32CurrentCount;
    TMR_tsActivityWakeTimerEvent *psCurrentNode, *psNextNode;
    TMR_teActivityStatus status = TMR_E_ACTIVITY_INVALID;
    NVIC_DisableIRQ(WAKE_UP_TIMER1_IRQn);
    do
    {
        if (TMR_E_ACTIVITY_RUNNING == psTmr->u8Status)
        {
            status = TMR_E_ACTIVITY_RUNNING;
            break;
        }
        psTmr->psNext = NULL;
        psTmr->prCallbackfn = prCallbackfn;
        psTmr->u8Status = TMR_E_ACTIVITY_RUNNING;

        /* Enable WTIMER if needed. Do not clear flag or stop the timer in
            * case a timer was already running */
        if (0 == (SYSCON->AHBCLKCTRLS[0] & SYSCON_AHBCLKCTRLSET0_WAKE_UP_TIMERS_CLK_SET_MASK))
        {
            WTIMER_Init();
        }
        TMR_SCHEDULE_ACT_LOG("[L%d] 0x%x u32Ticks=%d", __LINE__, psTmr, u32Ticks);

        /* Save the value of the currentTimestamp that would be loaded in wtimer1
         * Can be changed later in the function depending on the timer list
         */
        if (pCurrentTimestampValue != NULL)
            *pCurrentTimestampValue = u32Ticks;

        if (psNextActivity == NULL)
        {
            // List currently empty, add at the head
            psNextActivity           = psTmr;
            psTmr->u32TickDelta = u32Ticks;

            /* We can have a wakeup from lowpower Ram off in BLE (osTaskDelay() in RAM off) ,
                In this case, Either the timer is still running, or has expired,
                stop the timer in this case, the application shall handle what to do directly    */
            if ( WTIMER_GetStatusFlags(WTIMER_TIMER1_ID) != WTIMER_STATUS_NOT_RUNNING )
            {
                WTIMER_StopTimer(WTIMER_TIMER1_ID);
            }
            assert(WTIMER_GetStatusFlags(WTIMER_TIMER1_ID) == WTIMER_STATUS_NOT_RUNNING);
            TMR_SCHEDULE_ACT_LOG("[L%d] 0x%x u32Ticks=%d", __LINE__, psNextActivity, u32Ticks);
            WTIMER_StartTimerUnsafe(WTIMER_TIMER1_ID, u32Ticks);
            status = TMR_E_ACTIVITY_OK;
            break;
        }
        /* If we are here it means that wtimer1 is already running
         * Make sure that the register has been correclty updated
         * If TMR_eScheduleActivity32kTicks has just been called and has started a wtimer1,
         * this one may not be running yet. A wait for status to become running will be done prior to
         * the call to WTIMER_StopTimer.
         */
        if (WTIMER_GetStatusFlags(WTIMER_TIMER1_ID) == WTIMER_STATUS_NOT_RUNNING)
        {
            u32CurrentCount = psNextActivity->u32TickDelta;
        }
        else
        {
            /* catch time remaining on current timer */
            u32CurrentCount = WTIMER_ReadTimerSafe(WTIMER_TIMER1_ID);
        }
        TMR_SCHEDULE_ACT_LOG("[%d] u32CurrentCount=%d", __LINE__, u32CurrentCount);

        /* In case there is a pending IRQ, or if u32CurrentCount <= 4,
        * we can assume that the timer has fired because the execution
        * time to restart it will take at least 3 ticks
        */
        if (NVIC_GetPendingIRQ(WAKE_UP_TIMER1_IRQn) || u32CurrentCount <= 4)
        {
            u32CurrentCount = 0;
        }

        if (u32Ticks < u32CurrentCount)
        {
            // Inserting at head of queue
            // Adjust time remaining on interrupted timer
            psTmr->u32TickDelta       = u32Ticks;
            psNextActivity->u32TickDelta   = u32CurrentCount - u32Ticks;
            TMR_SCHEDULE_ACT_LOG("[L%d] 0x%d u32TickDelta=%d", __LINE__, psNextActivity, psNextActivity->u32TickDelta);
            psTmr->psNext             = psNextActivity;
            psNextActivity                 = psTmr;
            TMR_SCHEDULE_ACT_LOG("[L%d] 0x%d u32Ticks=%d", __LINE__, psNextActivity, u32Ticks);
            /* wait for status to become running  (cf. previous comment) */
            while (WTIMER_GetStatusFlags(WTIMER_TIMER1_ID) == WTIMER_STATUS_NOT_RUNNING)
            {
                __asm volatile("nop");
            }
            /* stop the running timer */
            WTIMER_StopTimer(WTIMER_TIMER1_ID);
            /* Restart it with the new value */
            WTIMER_StartTimerUnsafe(WTIMER_TIMER1_ID, u32Ticks);
            status = TMR_E_ACTIVITY_OK;
            break;
        }

        if (pCurrentTimestampValue != NULL)
            *pCurrentTimestampValue = u32CurrentCount;

        // Find where in the list to insert new wake point
        for (psCurrentNode = psNextActivity; psCurrentNode != NULL;
                    psCurrentNode = psCurrentNode->psNext)
        {
            if (psCurrentNode == psNextActivity)
            {
                TMR_vUtilSubstractValue(&u32Ticks, u32CurrentCount);
            }
            else
            {
                TMR_SCHEDULE_ACT_LOG("[L%d] 0x%d u32TickDelta=%d", __LINE__, psCurrentNode, psCurrentNode->u32TickDelta);
                TMR_vUtilSubstractValue(&u32Ticks, psCurrentNode->u32TickDelta);
            }

            if (psCurrentNode->psNext == NULL)
            {
                // Add at end of list
                psTmr->u32TickDelta   = u32Ticks;
                TMR_SCHEDULE_ACT_LOG("[L%d] 0x%d u32TickDelta=%d", __LINE__, psTmr, psTmr->u32TickDelta);
                psTmr->psNext         = NULL;
                psCurrentNode->psNext  = psTmr;
                status = TMR_E_ACTIVITY_OK;
                break;
            }
            else
            {
                psNextNode = psCurrentNode->psNext;
                if (u32Ticks < psNextNode->u32TickDelta)
                {
                    // Insert in to middle of list
                    // adjust delta time of event after insertion point
                    psTmr->u32TickDelta       = u32Ticks;
                    TMR_SCHEDULE_ACT_LOG("[L%d] 0x%d u32TickDelta=%d", __LINE__, psTmr, psTmr->u32TickDelta);
                    TMR_vUtilSubstractValue(&psNextNode->u32TickDelta, u32Ticks);
                    psTmr->psNext = psNextNode;
                    psCurrentNode->psNext      = psTmr;
                    status = TMR_E_ACTIVITY_OK;
                    break;
                }
            }
        }
    } while(0);
    WTIMER_EnableInterrupts(WTIMER_TIMER1_ID);
    if (!scheduleActivityAlreadyCalled)
    {
        NVIC_SetPriority(WAKE_UP_TIMER1_IRQn, gStackTimer_IsrPrio_c >> (8 - __NVIC_PRIO_BITS));
        scheduleActivityAlreadyCalled = TRUE;
    }
    return status;
}

static TMR_teActivityStatus TMR_eRemoveActivityFunction(TMR_tsActivityWakeTimerEvent *psTmr, bool_t blocking)
{
    TMR_tsActivityWakeTimerEvent *psCurrentNode = NULL;
    TMR_tsActivityWakeTimerEvent *psPreviousNode = NULL;
    uint32_t remainingTime = 0;
    TMR_teActivityStatus status = TMR_E_ACTIVITY_INVALID;
    OSA_InterruptDisable();
    if (psTmr->u8Status != TMR_E_ACTIVITY_RUNNING)
    {
        /* Nothing to do if timer is not running */
        status = TMR_E_ACTIVITY_OK;
    }
    else
    {
        for (psCurrentNode = psNextActivity; psCurrentNode != NULL;
                    psCurrentNode = psCurrentNode->psNext)
        {
            if (psCurrentNode == psTmr)
            {
                /* Is it the first element ? */
                if (psCurrentNode == psNextActivity)
                {
                   /* If we are here it means that wtimer1 is already running
                   * Make sure that the register has been correclty updated
                   * If TMR_eScheduleActivity32kTicks has just been called and has started a wtimer1,
                   * this one may not be running yet, wait for status to become running now.
                   */
                    while (WTIMER_GetStatusFlags(WTIMER_TIMER1_ID) == WTIMER_STATUS_NOT_RUNNING)
                    {
                        __asm volatile("nop");
                    }
                    /* Get the remain time if needed */
                    if (WTIMER_GetStatusFlags(WTIMER_TIMER1_ID) != WTIMER_STATUS_EXPIRED
                            /* We do not need to read the value if next element is empty */
                            && psCurrentNode->psNext != NULL)
                    {
                        remainingTime = WTIMER_ReadTimerSafe(WTIMER_TIMER1_ID);
                        TMR_SCHEDULE_ACT_LOG("[L%d] 0x%x remainingTime=%d", __LINE__, psCurrentNode, remainingTime);
                    }

                    /*
                     * When 1 < remainingTime < 4, there is an issue the wtimer hardware. Even if
                     * WTIMER_StopTimer is called, an interrupt will fire 4 (32k ticks) later. As
                     * a workaround this unwanted IRQ would be rejected.
                     */
                    if (remainingTime > 1 && remainingTime < 4)
                    {
                        rejectWtimer1Irq = TRUE;
                    }

                    if (!blocking && psCurrentNode->psNext == NULL)
                    {
                        /* Assume that WTIMER_StartTimerUnsafe would be called later at least
                         * 2 (32k ticks) after this call
                         */
                        WTIMER_StopTimerUnsafe(WTIMER_TIMER1_ID);
                    }
                    else
                    {
                        WTIMER_StopTimer(WTIMER_TIMER1_ID);
                    }
                    psNextActivity->u8Status = TMR_E_ACTIVITY_FREE;
                    /* Restart the next one if needed */
                    psNextActivity = psCurrentNode->psNext;
                    if (psNextActivity != NULL)
                    {
                        psNextActivity->u32TickDelta += remainingTime;
                        TMR_SCHEDULE_ACT_LOG("[L%d] 0x%x u32TickDelta=%d", __LINE__, psNextActivity, psNextActivity->u32TickDelta);
                        WTIMER_StartTimerUnsafe(WTIMER_TIMER1_ID, psNextActivity->u32TickDelta);
                    }
                }
                else if (psPreviousNode != NULL)
                {
                    psPreviousNode->psNext = psCurrentNode->psNext;
                    if (psCurrentNode->psNext != NULL)
                    {
                        psCurrentNode->psNext->u32TickDelta += psCurrentNode->u32TickDelta;
                        TMR_SCHEDULE_ACT_LOG("[L%d] 0x%x deltaTick=%d", __LINE__, psCurrentNode->psNext, psCurrentNode->psNext->u32TickDelta);
                    }
                }
                psCurrentNode->u8Status = TMR_E_ACTIVITY_FREE;
                status = TMR_E_ACTIVITY_OK;
                break;
            }
            psPreviousNode = psCurrentNode;
        }
    }
    OSA_InterruptEnable();
    return status;
}

TMR_teActivityStatus TMR_eRemoveActivity(TMR_tsActivityWakeTimerEvent *psTmr)
{
    return TMR_eRemoveActivityFunction(psTmr, TRUE);
}

TMR_teActivityStatus TMR_eRemoveActivityUnsafe(TMR_tsActivityWakeTimerEvent *psTmr)
{
    return TMR_eRemoveActivityFunction(psTmr, FALSE);
}

static void TMR_vActivityInterruptCallback(void)
{
    WTIMER_StopTimer(WTIMER_TIMER1_ID);
    if (psNextActivity != NULL)
    {
        TMR_SCHEDULE_ACT_LOG("");

        // If there is a callback function call it
        if (psNextActivity->prCallbackfn != NULL)
        {
            psNextActivity->prCallbackfn();
        }

        // Free up the timer for future use
        psNextActivity->u8Status  = TMR_E_ACTIVITY_FREE;
        // Next wake event in the list
        psNextActivity            = psNextActivity->psNext;

        if (psNextActivity != NULL)
        {
            TMR_SCHEDULE_ACT_LOG("[L%d] 0x%x psNextActivity->u32TickDelta=%d", __LINE__, psNextActivity, psNextActivity->u32TickDelta);
            // Start the timer for the next tmr event
            WTIMER_StartTimerUnsafe(WTIMER_TIMER1_ID, psNextActivity->u32TickDelta);
        }
    }
}

/* Wake timer 1 interrupt handler */
void WAKE_UP_TIMER1_IRQHandler(void)
{
    if (!rejectWtimer1Irq)
    {
        TMR_vActivityInterruptCallback();
    }
    else
    {
        TMR_SCHEDULE_ACT_LOG("[L%d] IRQ rejected", __LINE__);
        rejectWtimer1Irq = FALSE;
        WTIMER_ClearStatusFlags(WTIMER_TIMER1_ID);
    }
}


#ifdef FSL_RTOS_FREE_RTOS
//static tmrlp_tickless_systick_t SysTick_lp_ctx;

typedef struct {
#if 0
    uint32_t sleep_start_ts;
    bool     intercept_next_timer_irq;
    uint32_t initial_core_freq;
    uint32_t MaximumPossibleSuppressedTicks;
    uint32_t TimerCountsForOneTick;
    uint32_t StoppedTimerCompensation;
    uint32_t suppressed_ticks_time_msec;
    int32_t  original_tmr_value;
    uint32_t origin_systick_ts;
#if CoreFreqVariable_c
    uint32_t systick_residual_count;
    uint32_t estimated_core_freq;
#endif
#endif
    bool        cal_ongoing;
    uint32_t    core_clock_freq;
    uint32_t    TimerCountsForOneSysTick;
} tmr_fro_cal_t;

static tmr_fro_cal_t tmr_fro_cal_ctx;


static uint32_t idle_time_carry_ticks = 0;
static TMR_tsActivityWakeTimerEvent osTimerEvent;
#if gTimestampUseWtimer_c
static uint32_t origin_systick_32k_ts = 0;
#endif


void tickless_PreProcessing(tmrlp_tickless_systick_t * p_lp_ctx, uint32_t xExpectedIdleTime)
{
    uint32_t core_freq = tmr_fro_cal_ctx.core_clock_freq;
    uint32_t systick_cvr = 0;
    uint32_t systick_rvr = 0;
    uint32_t val = 0;
    uint32_t count_for_one_tick;
    uint32_t systick_residual_count_32kTicks = 0;
    uint32_t xExpectedIdleTime_32k = WTIMER1_MAX_VALUE;
    TMR_teActivityStatus status;

    /* Set p_lp_ctx default values */
    p_lp_ctx->osSleep_activity_scheduled = FALSE;
    p_lp_ctx->idle_tick_jump = 0;
    p_lp_ctx->rtosSystickExpectedIdleTime = xExpectedIdleTime;
    p_lp_ctx->sysTick_CSR = SysTick->CTRL;

    /* Value to be programmed in LOAD at next SYSTICK restore */
    /* Nb Core ticks to countdown */
    count_for_one_tick          =  tmr_fro_cal_ctx.TimerCountsForOneSysTick;
    p_lp_ctx->sysTick_RV        =  (count_for_one_tick -1UL) & SysTick_LOAD_RELOAD_Msk;

#if !defined(gSystickUseWtimer1ForSleepDuration) || !gSystickUseWtimer1ForSleepDuration
    /* Get the initial ts in 32k that would be used to calculate the sleep duration */
    p_lp_ctx->sleep_start_ts_32kTicks = Timestamp_GetCounter32bit();
#endif

    /* Disable SysTick counter and interrupt */
    SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);

    /* clear PendSysTick bit in ICSR, if set */
    SCB->ICSR |= SCB_ICSR_PENDSVCLR_Msk;

    systick_cvr = SysTick->VAL & SysTick_VAL_CURRENT_Msk;
    systick_rvr = SysTick->LOAD;
    val = (systick_rvr - systick_cvr);
#if 0
    if (SysTick->CTRL & (SysTick_CTRL_COUNTFLAG_Msk))
    {
        val += systick_rvr;
    }
#endif
    SYSTICK_DBG_LOG("cvr=%d rvr=%d val=%d", systick_cvr, systick_rvr, val);

    systick_residual_count_32kTicks = (uint32_t)(((uint64_t)val << 15) / core_freq);

    SYSTICK_DBG_LOG("residual_count_32k_tick=%d", systick_residual_count_32kTicks);

    /* Get the compensation value that would be used to calculate the sleep duration */
    p_lp_ctx->compensation_32kticks = idle_time_carry_ticks + systick_residual_count_32kTicks;

    assert(TICKS32K_TO_MILLISECONDS(p_lp_ctx->compensation_32kticks) < 2*portTICK_PERIOD_MS);


#define SUPPRESS_TICK_MAX_DURATION              (TICKS32K_TO_MILLISECONDS(WTIMER1_MAX_VALUE)/portTICK_PERIOD_MS)
    /* Is there an OS event to schedule ?
     * The expected Idle Time is calculated by FreeRTOS by using:
     * xExpectedIdleTime = xNextTaskUnblockTime - xTickCount
     * If there is no os event to schedule xNextTaskUnblockTime=0xffffffff
     */
    if ((~0)-xExpectedIdleTime == xTaskGetTickCount() || xExpectedIdleTime > SUPPRESS_TICK_MAX_DURATION)
    {
        xExpectedIdleTime = SUPPRESS_TICK_MAX_DURATION;
    }

#if defined(cPWR_UsePowerDownMode) && (cPWR_UsePowerDownMode == 1)
    /* don t schedule wakeup due to OS event if device goes to lowpower RAM off and lowpower duration is equal to the max (meaning infinite) */
    if ( (PWR_GetDeepSleepConfig() & PWR_CFG_RAM_ON ) || ( xExpectedIdleTime != SUPPRESS_TICK_MAX_DURATION))
#endif
    {
        /* Convert the Idle time to 32k ticks */
        xExpectedIdleTime_32k = MILLISECONDS_TO_TICKS32K(xExpectedIdleTime*portTICK_PERIOD_MS);

        /* Add the necessary margin to take into account:
         * - the systck residual count calculated before going to sleep
         * - the carry calculated at the last wake up
         * - the duration to exit from the tickless mode
         */
        xExpectedIdleTime_32k = xExpectedIdleTime_32k - p_lp_ctx->compensation_32kticks - p_lp_ctx->exitTicklessDuration32kTick;

        SYSTICK_DBG_LOG("expected os_sleep duration = %d 32k tick", xExpectedIdleTime_32k);

#if !defined(gSystickUseWtimer1ForSleepDuration) || (gSystickUseWtimer1ForSleepDuration == 0)
        /* Schedule a low power timer event to process the next OS activity */
        status = TMR_eScheduleActivity32kTicks(&osTimerEvent, xExpectedIdleTime_32k, NULL);
#else
        /* Schedule a low power timer event to process the next OS activity */
        status = TMR_eScheduleActivity32kTicksAndGetCurrentTimestampValue(&osTimerEvent, xExpectedIdleTime_32k, NULL, &p_lp_ctx->sleep_start_ts_32kTicks);
#endif
        assert(status == TMR_E_ACTIVITY_OK);
        (void) status;

        p_lp_ctx->osSleep_activity_scheduled = TRUE;
    }
}

static uint32_t TMR_GetTotalSleepDuration32kTicks(uint32_t start_of_sleep)
{
    uint32_t ticks;
    OSA_InterruptDisable();
#if defined(gSystickUseWtimer1ForSleepDuration) && gSystickUseWtimer1ForSleepDuration
    if (WTIMER_GetStatusFlags(WTIMER_TIMER1_ID) == WTIMER_STATUS_NOT_RUNNING || start_of_sleep <= 4)
    {
        /* If the timer is not in running state or the start_of_sleep value is very small
         * we can assume that we did not sleep
         */
        ticks = 0;
    }
    else
    {
        uint32_t now = WTIMER_ReadTimerSafe(WTIMER_TIMER1_ID);
        if (WTIMER_GetStatusFlags(WTIMER_TIMER1_ID) == WTIMER_STATUS_RUNNING)
        {
            if (start_of_sleep >= now)
            {
                ticks = (start_of_sleep - now);
            }
            else
            {
                /* could happens when two calls to WTIMER_ReadTimerSafe are done too close */
                ticks = 0;
            }
        }
        else
        {
            /* Expired status: We always assume that wtimer1 has wrapped */
            ticks = (WTIMER1_MAX_VALUE - now) + start_of_sleep + 1;
        }
    }
#elif defined(gTimestampUseWtimer_c) && gTimestampUseWtimer_c
    ticks = Timestamp_GetCounter32bit();
    if (ticks > start_of_sleep)
    {
        ticks = (ticks - start_of_sleep);
    }
    else
    {
        ticks = (~0UL - start_of_sleep) + ticks + 1;
    }
    /* time_delta is a number of 32kHz ticks: convert to milliseconds */
#else
    /* the counter is counting down so previous value is greater.
     * already expressed in 1kHz ticks */
    ticks = RTC_GetWakeupCount(RTC);
    if (ticks > start_of_sleep)
    {
        ticks = (ticks - start_of_sleep);
    }
    else
    {
        ticks = 0x10000 - start_of_sleep + ticks;
    }
    ticks <<=5;
#endif
    OSA_InterruptEnable();

    TMR_DBG_LOG("ticks32k=%d", ticks);
    return (uint32_t)ticks;
}


void tickless_PostProcessing(tmrlp_tickless_systick_t * p_lp_ctx)
{
    uint32_t sleep_duration_32k_ticks = 0;
    uint32_t comp_plus_dur = 0;
    int32_t tick_jump_diff = 0;

    /* Restart SysTick so it runs from SysTick->LOAD
       again, then set SysTick->LOAD back to its standard
       value. The critical section is used to ensure the tick interrupt
       can only execute once in the case that the reload register is near
       zero.
    */
    SysTick->VAL  = 0UL; /* Current value is reset */
    SysTick->LOAD = tmr_fro_cal_ctx.TimerCountsForOneSysTick; /* Set reload value first */
    SysTick->CTRL = p_lp_ctx->sysTick_CSR; /* Systick restart now */

    sleep_duration_32k_ticks = TMR_GetTotalSleepDuration32kTicks(p_lp_ctx->sleep_start_ts_32kTicks);

    configASSERT( sleep_duration_32k_ticks < WTIMER1_MAX_VALUE );

    comp_plus_dur            = p_lp_ctx->compensation_32kticks + sleep_duration_32k_ticks;

    p_lp_ctx->idle_tick_jump = TICKS32K_TO_MILLISECONDS(comp_plus_dur) / portTICK_PERIOD_MS;
    idle_time_carry_ticks    = comp_plus_dur - MILLISECONDS_TO_TICKS32K(p_lp_ctx->idle_tick_jump*portTICK_PERIOD_MS);

    SYSTICK_DBG_LOG("compensation=%d carry=%d idle=%d", comp_plus_dur, idle_time_carry_ticks, p_lp_ctx->idle_tick_jump);
    SYSTICK_DBG_LOG("sleep_duration = %d", sleep_duration_32k_ticks);

    tick_jump_diff = p_lp_ctx->rtosSystickExpectedIdleTime - p_lp_ctx->idle_tick_jump;
    /* Acceptable tick diff +1 */
    if (tick_jump_diff == 1)
    {
        p_lp_ctx->idle_tick_jump = p_lp_ctx->rtosSystickExpectedIdleTime;
        SYSTICK_DBG_LOG("warning tick_jump_diff = %d", tick_jump_diff);
    }

    /* Update the OS time ticks. */
    vTaskStepTick( p_lp_ctx->idle_tick_jump );

    if (p_lp_ctx->osSleep_activity_scheduled)
    {
        /* Remove the os timer activity */
        TMR_eRemoveActivityUnsafe(&osTimerEvent);
    }
}


/* suppose 32MHz crystal is running and FRO48M running */
void tickless_StartFroCalibration(void)
{
    INPUTMUX_Init(INPUTMUX);

    /* Setup to measure the selected target */
    INPUTMUX_AttachSignal(INPUTMUX, 1U, kINPUTMUX_Xtal32MhzToFreqmeas);
    INPUTMUX_AttachSignal(INPUTMUX, 0U, kINPUTMUX_MainClkToFreqmeas);

    CLOCK_EnableClock(kCLOCK_Fmeas);

    /* Start a measurement cycle and wait for it to complete. If the target
       clock is not running, the measurement cycle will remain active
       forever, so a timeout may be necessary if the target clock can stop */
    FMEAS_StartMeasureWithScale(FMEAS_SYSCON, 17);

    tmr_fro_cal_ctx.cal_ongoing = true;
}

static uint32_t tickless_GetCoreFreqTickReloadValue(void)
{
    uint32_t reload_value;

    assert( tmr_fro_cal_ctx.core_clock_freq != 0 );
    SYSTICK_DBG_LOG("core_freq=%d", tmr_fro_cal_ctx.core_clock_freq);

    reload_value = (tmr_fro_cal_ctx.core_clock_freq / configTICK_RATE_HZ);

//    if (reload_value > SysTick_lp_ctx.StoppedTimerCompensation)
//        reload_value -= SysTick_lp_ctx.StoppedTimerCompensation;

    return reload_value;
}

static uint32_t tickless_CompleteFroCalibration(void)
{
    uint32_t        freqComp    = 0U;
    uint32_t        refCount    = 0U;
    uint32_t        targetCount = 0U;
    uint32_t        freqRef     = CLOCK_GetFreq(kCLOCK_Xtal32M);

    if (FMEAS_IsMeasureComplete(FMEAS_SYSCON))
    {
        /* Wait 2^17 counts of the reference clock : 4096us */
        /* Get computed frequency */
        FMEAS_GetCountWithScale(FMEAS_SYSCON, 17, &refCount, &targetCount);
        freqComp = (uint32_t)(((((uint64_t)freqRef)*refCount)) / targetCount);

        /* Disable the clocks */
        CLOCK_DisableClock(kCLOCK_Fmeas);
    }
    return freqComp;
}

void tickless_init(void)
{
    tmr_fro_cal_ctx.cal_ongoing                 = FALSE;

    /* give a first frequency value - this may be refined by calibration */
    tmr_fro_cal_ctx.core_clock_freq             = CLOCK_GetFreq(kCLOCK_CoreSysClk);
    tmr_fro_cal_ctx.TimerCountsForOneSysTick    = tickless_GetCoreFreqTickReloadValue(); /* Sets TickCountForOneTick */
}

bool_t tickless_EstimateCoreClockFreq(void)
{
    bool_t ret = FALSE;
    if (tmr_fro_cal_ctx.cal_ongoing)
    {
        uint32_t core_freq = tickless_CompleteFroCalibration();
        if (core_freq == 0)
        {
            ret = TRUE;
        }
        else
        {
            tmr_fro_cal_ctx.core_clock_freq             = (uint32_t)core_freq;
            tmr_fro_cal_ctx.TimerCountsForOneSysTick    = tickless_GetCoreFreqTickReloadValue(); /* Sets TickCountForOneTick */
            SYSTICK_DBG_LOG("core_freq=%d", tmr_fro_cal_ctx.core_clock_freq);

            tmr_fro_cal_ctx.cal_ongoing = false;
            ret = FALSE;
        }
    }
    return ret;
}

void tickless_SystickCheckDriftInit(void)
{
#if gTimestampUseWtimer_c
    origin_systick_32k_ts = Timestamp_GetCounter32bit();
#endif
}

void tickless_SystickCheckDrift(void)
{
#if gTimestampUseWtimer_c
    static int32_t prev_delta_ticks = -1;

    uint32_t cnt = Timestamp_GetCounter32bit() - origin_systick_32k_ts;
    uint32_t os_msec = OSA_TimeGetMsec();
    uint32_t cnt_msec = TICKS32K_TO_MILLISECONDS(cnt);
    uint32_t delta = 0;

    if (cnt_msec > os_msec)
    {
        delta = cnt_msec-os_msec;
    }
    else
    {
        delta = os_msec-cnt_msec;
    }

    SYSTICK_DBG_LOG("os_msec=%d delta=%d cnt_msec=%d", os_msec, delta, cnt_msec);

    int32_t nb_TICK = delta / portTICK_PERIOD_MS;
    #define MAX_NB_TICK_DRIFT 10
#if 0
#define PRINT_NB_TICK(x) PRINTF("%d.", x)
#else
#define PRINT_NB_TICK(...)
#endif
    do {
        if (nb_TICK > prev_delta_ticks)
        {
            PRINTF("\r\nd=%d cnt_msec=%d\r\n", delta, cnt_msec);
            prev_delta_ticks = nb_TICK;
        }
        PRINT_NB_TICK(nb_TICK);
#if 0
        if (nb_TICK > MAX_NB_TICK_DRIFT)
        {
#if defined gLoggingActive_d && gLoggingActive_d
            DbgLogDump(true);
#endif
            break;
        }
#endif
    } while(0);
#endif
}
#endif

