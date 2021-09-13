/*****************************************************************************
 *
 * Copyright Jennic Ltd. 2008 All rights reserved
 * Copyright 2019 NXP.
 *
 * NXP Confidential. This software is owned or controlled by NXP and may only
 * be used strictly in accordance with the applicable license terms.  By
 * expressly accepting such terms or by downloading, installing, activating
 * and/or otherwise using the software, you are agreeing that you have read,
 * and that you agree to comply with and are bound by, such license terms.
 * If you do not agree to be bound by the applicable license terms, then you
 * may not retain, install, activate or otherwise use the software.
 *
 ****************************************************************************/

#ifndef PWRM_H_
#define PWRM_H_

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "PWR_Interface.h"

/* Sleep modes */
typedef enum
{
    E_AHI_SLEEP_OSCON_RAMON   = PWR_E_SLEEP_OSCON_RAMON,   /*32Khz Osc on and Ram On*/
    E_AHI_SLEEP_OSCON_RAMOFF  = PWR_E_SLEEP_OSCON_RAMOFF,  /*32Khz Osc on and Ram off*/
    E_AHI_SLEEP_OSCOFF_RAMON  = PWR_E_SLEEP_OSCOFF_RAMON,  /*32Khz Osc off and Ram on*/
    E_AHI_SLEEP_OSCOFF_RAMOFF = PWR_E_SLEEP_OSCOFF_RAMOFF, /*32Khz Osc off and Ram off*/
    E_AHI_SLEEP_INVALID       = PWR_E_SLEEP_INVALID
} teAHI_SleepMode;

/*
 * Define E_AHI_SLEEP_DEEP to E_AHI_SLEEP_OSCOFF_RAMOFF for backwards
 * compatibility. if no IO wakeup is requested except PAD reset pin, then PWRM will desactivate
 * the IO ring for power saving
 */
#define E_AHI_SLEEP_DEEP       E_AHI_SLEEP_OSCOFF_RAMOFF

#define PWRM_NTAG_FD_WAKEUP    ( 1 << 22 )
#define PWRM_ANA_COMP_WAKEUP   ( 1 << 23 )       /** wakeup from analog comparator in sleep mode - Prevent deep sleep where IO ring is disabled */
#define PWRM_BOD_WAKEUP        ( 1 << 24 )       /** wakeup from Brown-Out Detect interrupt */

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#ifndef PWRM_INLINE
#define PWRM_INLINE INLINE
#endif

#ifndef PWRM_ALWAYS_INLINE
#define PWRM_ALWAYS_INLINE ALWAYS_INLINE
#endif

#define PWRM_CALLBACK(name) void pwrm_##name(void)

#define PWRM_DECLARE_CALLBACK_DESCRIPTOR(name) PWRM_CALLBACK(name)

#define PWRM_vRegisterWakeupCallback(name) PWR_RegisterLowPowerExitCallback(pwrm_##name)
#define PWRM_vRegisterPreSleepCallback(name) PWR_RegisterLowPowerEnterCallback(pwrm_##name)

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef struct _sWakeTimerEvent {
    struct _sWakeTimerEvent  *psNext;
    uint32 u32TickDelta;
    void (*prCallbackfn)(void);
    uint8 u8Status;
} pwrm_tsWakeTimerEvent;

typedef enum {
    PWRM_E_OK,
    PWRM_E_ACTIVITY_OVERFLOW,
    PWRM_E_ACTIVITY_UNDERFLOW,
    PWRM_E_TIMER_RUNNING,
    PWRM_E_TIMER_FREE,
    PWRM_E_TIMER_INVALID,
    PWRM_E_IO_INVALID,
    PWRM_E_MODE_INVALID,
} PWRM_teStatus;

typedef teAHI_SleepMode PWRM_tePowerMode;

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/**
 * Initialize the Power Manager Framework:
 * If ePowerMode is E_AHI_SLEEP_OSCON_RAMON or E_AHI_SLEEP_OSCON_RAMOFF:
 * - Calibrate the 32KHz clock source if the main 32K clock source is FRO32K by calling
 * PWRM_GetFro32KCalibrationValue();  and OSCON modes are selected
 *   Application shall have selected the proper 32K clock source before calling this function
 * - Enable the Wake timer clocks and enable the PWRM wake timer interrupts
 */
PUBLIC PWRM_teStatus PWRM_vInit(PWRM_tePowerMode ePowerMode);
PUBLIC void PWRM_vManagePower(void);

/*
 * In E_AHI_SLEEP_OSCON_RAMOFF, due to lack retention, all PWRM timer expiring after a wakeup from
 * sleep mode will not have the callback called. Typical scenerio happens when the application
 * uses the second wake timer, if the second timer expires before the PWRM timer programmed by
 * the PWRM_eScheduleActivity function, the callback in PWRM_eScheduleActivity() will not be called
 */

PUBLIC PWRM_teStatus PWRM_eScheduleActivity(
        pwrm_tsWakeTimerEvent *psWake,
        uint32 u32TimeMs,
        void (*prCallbackfn)(void));

PUBLIC PWRM_teStatus PWRM_eRemoveActivity(
        pwrm_tsWakeTimerEvent *psWake);

PUBLIC void vAppRegisterPWRMCallbacks(void);

/*
 * Shall be called after hardware_init(), after the hardware is initialised
 * from reset or a cold start (sleep with RAM off). No need to call it after a
 * warm start (sleep with RAM on).
 * PWRM_vColdStart() does the following actions :
 * - call vAppRegisterPWRMCallbacks
 * - reset the wake timer IP if the reset cause if different from the power down reset
 * - Stop the PWRM timer if the reset cause is the power down reset
 * - call the post wakeup callbacks
 * PWRM_vColdStart() does not call the PWRM_eScheduleActivity() callback since the RAM was OFF
 */
PUBLIC void PWRM_vColdStart(void);

/**
 *  Obsolete function replaced by PWRM_vWakeUpConfig() to support more wakeup sources
 *  kept for backward functionality
 */
PUBLIC PWRM_teStatus PWRM_vWakeUpIO(uint32_t io_mask);

/**
 *  Configure the wakeup sources
 *  Replace function PWRM_vWakeUpIO() with more wakeup sources
 *  BIT22 for NTAG field detect wakeup
 *  Enable the Digital IO wake up sources in sleep modes and/or NTAG field detect (PWRM_NTAG_FD_WAKEUP)
 *  In sleep mode, the chip will wake up if the level of one IO level ID specified in the io_mask from 0 to 21
 *  is changed.
 *  Range from 0 to 21 is for DIO0 to DIO21 wakeup. PWRM_NTAG_FD_WAKEUP in pwrm_config is for NTAG field detect wakeup.
 *  For DIO wakeup from 0 to 21, no IO wakeup callback is called. The User shall check the IO status
 *  in the post wakeup callback if needed.
 *  For NTAG field detect wakeup when PWRM_NTAG_FD_WAKEUP (1<<22) in pwrm_config has been set,
 *  the application shall check and enable the NTAG field detect interrupt handler during wakeup,
 *  For Analog Comparator wakeup when PWRM_ANA_COMP_WAKEUP (1<<23) in pwrm_config has been set,
 *  the application shall enable the Analog Comparator through the dedicated AnaComp API. Note that because
 *  the Analog comparator shall remain ON, expect additional power increase due to the analog comparator power domain
 *  leakage and because the device can not go to deep down mode (IO ring can not be disabled if they are about to be)
 *  For Brown-Out Detect wakeup when PWRM_BOD_WAKEUP (1<<24) in pwrm_config has been set,
 *  the application shall enable the wake from the BOD interrupt
 *
 *  PWRM_vWakeUpConfig() does not apply for doze mode. In doze mode, the Application
 *  shall program the resume of the Execution from IO through the fsl_gint.h API
 *
 *  PWRM_vWakeUpConfig() will return PWRM_E_IO_INVALID if a bit set in the bit field
 *  is invalid (any bit above bit 24 is set)
 *
 *  To clear the wakeup sources, call this function with argument set to 0.
 *  For all other argument values, the new configuration is added to any
 *  existing configuration. Hence:
 *    PWRM_vWakeUpConfig(PWRM_NTAG_FD_WAKEUP);
 *    PWRM_vWakeUpConfig(PWRM_ANA_COMP_WAKEUP);
 *  is equivalent to:
 *    PWRM_vWakeUpConfig(PWRM_NTAG_FD_WAKEUP | PWRM_ANA_COMP_WAKEUP);
 *
 *  return PWRM_teStatus
 *   */
PUBLIC PWRM_teStatus PWRM_vWakeUpConfig(uint32_t pwrm_config);

/**
 * Force some RAM banks to be held in retention when going to sleep mode in sleep modes
 * E_AHI_SLEEP_OSCON_RAMON and E_AHI_SLEEP_OSCOFF_RAMON.
 * In sleep modes E_AHI_SLEEP_OSCON_RAMON and E_AHI_SLEEP_OSCOFF_RAMON,
 * PWRM already keeps the required RAM banks in retention to hold the content of the
 * the application data memory : data, bss, heap, stack.
 * However, It never holds the SRAM1 (bank8 to bank11).
 * Consequently, if the application desires to keep additional RAM banks in retention,
 * call this API with u32RetainBitmap being a bitfield of the bank0 to bank11 t0 be held
 * in retention as the same as the defines in fsl_power.h PM_CFG_SRAM_BANKX_RET.
 * Call API with  u32RetainBitmap set to 0 to clear the RAM banks to be forced in retention
 *
 * Configuration is cleared after a call to PWRM_vInit(). Application shall call the API again if
 * PWRM_vInit() is called.
 */
PUBLIC void PWRM_vForceRamRetention(uint32_t u32RetainBitmap);

/**
 * Force radio registers to be retained through sleep. Enabled if bRetain is
 * TRUE, disabled if bRetain is FALSE.
 * Configuration is cleared after a call to PWRM_vInit(). Application should
 * call the API again if PWRM_vInit() is called.
 */
PUBLIC void PWRM_vForceRadioRetention(bool_t bRetain);

/**
 * Return number of outstanding actions that need to complete before sleep 
 * is possible 
 */
PUBLIC uint16 PWRM_u16GetActivityCount(void);

PUBLIC PWRM_teStatus PWRM_eStartActivity(void);
PUBLIC PWRM_teStatus PWRM_eFinishActivity(void);

/****************************************************************************/
/***        Private Functions                                             ***/
/****************************************************************************/

#endif /*PWRMAN_H_*/

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
