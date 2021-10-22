/*! *********************************************************************************
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2020 NXP
* All rights reserved.
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef _PWR_CONFIGURATION_H_
#define _PWR_CONFIGURATION_H_

/*****************************************************************************
 *                               INCLUDED HEADERS                            *
 *---------------------------------------------------------------------------*
 * Add to this section all the headers that this module needs to include.    *
 * Note that it is not a good practice to include header files into header   *
 * files, so use this section only if there is no other better solution.     *
 *---------------------------------------------------------------------------*
 *****************************************************************************/
#include "TimersManager.h"

/************************************************************************************
 *************************************************************************************
 * Module configuration constants
 *************************************************************************************
 ************************************************************************************/

/*-----------------------------------------------------------------------------
 * To enable/disable all of the code in this PWR/PWRLib files.
 *   TRUE =  1: Use Lowpower functions (Normal)
 *   FALSE = 0: Don't use Lowpower - only WFI. Will cut variables and code out. But
 *              functions still exist. Useful for debugging and test purposes
 */
#ifndef cPWR_UsePowerDownMode
#define cPWR_UsePowerDownMode            FALSE
#endif

#ifndef cPWR_FullPowerDownMode
#define cPWR_FullPowerDownMode           FALSE
#endif

#ifndef cPWR_EnableDeepSleepMode_4
#define cPWR_EnableDeepSleepMode_4     	 FALSE
#endif

#ifndef gAppDeepSleepMode_c
#define gAppDeepSleepMode_c    1
#endif

#define cPWR_NoClockGating                     0
#define cPWR_ClockGating                       1
#define cPWR_DeepSleep_RamOffOsc32kOn          2
#define cPWR_PowerDown_RamRet                  3
#define cPWR_DeepSleep_RamOffOsc32kOff         4


#ifndef gPWR_LpOscOptim
#define gPWR_LpOscOptim                        1
#endif

#ifndef gPWR_Xtal32MDeactMode
#define gPWR_Xtal32MDeactMode                  1
#endif

#ifndef gPWR_Xtal32MSwitchOffEarlier
#define gPWR_Xtal32MSwitchOffEarlier           0
#endif


#if cPWR_UsePowerDownMode
  #if cPWR_FullPowerDownMode
  #define cPWR_DeepSleepMode                     cPWR_PowerDown_RamRet
  #else
  #define cPWR_DeepSleepMode                     cPWR_ClockGating
  #endif
#else
  #define cPWR_DeepSleepMode                     cPWR_NoClockGating
#endif

#ifndef gSupportBle
/* BLE features (checks BLE status, uses BLE timers) */
#define gSupportBle (1)
#endif

/* 32k FRO clock: if using 32k FRO (gClkUseFro32K=1) then default to recalibrating it
 * on every wake (gClkRecalFro32K=1). If 32k FRO should only be calibrated on first
 * start, user must explicitly set gClkRecalFro32K=0 */
#if (defined gClkUseFro32K) && (gClkUseFro32K != 0) && !(defined gClkRecalFro32K)
#define gClkRecalFro32K (1)
#endif

/* Number of times the wakeup Reason is checked before proceeding on powerdown exit */
#ifndef PWR_WakeupReasonWaitingLoop
#define PWR_WakeupReasonWaitingLoop  1
#endif

#ifndef gPWR_BleLL_EarlyEnterDsm
#define gPWR_BleLL_EarlyEnterDsm     0
#endif

#ifndef gPWR_LpEntryStructOptim
#define gPWR_LpEntryStructOptim      1
#endif

#if 1
#define gPWR_ForceWakeTimer0_WakeupSrc  1
#define gPWR_ForceWakeTimer1_WakeupSrc  1
#define gPWR_ForceRtc_WakeupSrc         1

#else
/* flags may not be set in this file */
#if ((gTimestamp_Enabled_d == 1) && (gTimestampUseWtimer_c == 0)) || (gLoggingActive_d == 1)
/* No need for WakeTimer 0 wakeup source for Timestamping */
//#define gPWR_ForceWakeTimer0_WakeupSrc  1
#endif

#if ( /*(gTimestamp_Enabled_d == 1) && (gTimestampUseWtimer_c == 1)) ||*/ (mAppUseTickLessMode_c ==1)  )
#define gPWR_ForceWakeTimer1_WakeupSrc  1
#endif

#if (gTMR_Enabled_d == 1)
#define gPWR_ForceRtc_WakeupSrc         1
#endif
#endif

#endif /* _PWR_CONFIGURATION_H_ */

