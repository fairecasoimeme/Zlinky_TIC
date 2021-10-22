/*! *********************************************************************************
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2017, 2019-2020 NXP
* All rights reserved.
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef _PWR_INTERFACE_H_
#define _PWR_INTERFACE_H_

/*****************************************************************************
 *                               INCLUDED HEADERS                            *
 *---------------------------------------------------------------------------*
 * Add to this section all the headers that this module needs to include.    *
 * Note that it is not a good practice to include header files into header   *
 * files, so use this section only if there is no other better solution.     *
 *---------------------------------------------------------------------------*
 *****************************************************************************/
#include "EmbeddedTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 *                             PRIVATE MACROS                                *
 *---------------------------------------------------------------------------*
 * Add to this section all the access macros, registers mappings, bit access *
 * macros, masks, flags etc ...                                              *
 *---------------------------------------------------------------------------*
 *****************************************************************************/

/*****************************************************************************
 *                             PUBLIC MACROS                                 *
 *---------------------------------------------------------------------------*
 * Add to this section all the access macros, registers mappings, bit access *
 * macros, masks, flags etc ...                                              *
 *---------------------------------------------------------------------------*
 *****************************************************************************/
#ifndef gAllowDeviceToSleep_c
#define gAllowDeviceToSleep_c 0
#endif

/* Bitfields for use with PWR_GetDeepSleepConfig */
#define PWR_CFG_REDUCE_AO_LDO   (0x01)
#define PWR_CFG_MAINTAIN_AO_LDO (0x02)
#define PWR_CFG_RAM_ON          (0x04)
#define PWR_CFG_RAM_OFF         (0x08)
#define PWR_CFG_OSC_ON          (0x10)
#define PWR_CFG_OSC_OFF         (0x20)

/* Bitfield values that can be passed to PWR_vWakeUpConfig */
#define PWR_NTAG_FD_WAKEUP    ( 1 << 22 ) /** wakeup from NTAG field detect interrupt */
#define PWR_ANA_COMP_WAKEUP   ( 1 << 23 ) /** wakeup from analog comparator in sleep mode - Prevent deep sleep where IO ring is disabled */
#define PWR_BOD_WAKEUP        ( 1 << 24 ) /** wakeup from Brown-Out Detect interrupt */

/* Basic MACROs that provide estimated time to enter and exit lowpower */
#define PWR_SYSTEM_EXIT_LOW_POWER_DURATION_MS       2
#define PWR_SYSTEM_ENTER_LOW_POWER_DURATION_MS      2

/*****************************************************************************
 *                        PUBLIC TYPE DEFINITIONS                            *
 *---------------------------------------------------------------------------*
 * Add to this section all the data types definitions: structures, unions,    *
 * enumerations, typedefs ...                                                *
 *---------------------------------------------------------------------------*
 *****************************************************************************/

/*
 * Name: PWRLib_WakeupReason_t
 * Description: Wakeup reason UNION definitions
 */
typedef  union
{
    uint16_t AllBits;
    struct
    {
        uint8_t FromReset       :1;  /* Coming from Reset */
        uint8_t FromUSART0      :1;  /* Wakeup by UART 0 interrupt / USART1 is not wake capable */
        uint8_t FromKeyBoard    :1;  /* Wakeup by TSI/Push button interrupt */
        uint8_t FromTMR         :1;  /* Wakeup by TMR timer interrupt */
        uint8_t FromRTC_Sec     :1;  /* Wakeup by RTC 1Hz timer interrupt */
        uint8_t FromRTC_mSec    :1;  /* Wakeup by RTC 1kHz timer interrupt */
        uint8_t FromWakeTimer   :1;  /* Wakeup by Wake timer interrupt */
        uint8_t FromBLE_LLTimer :1;  /* Wakeup by BLE_LL Timer */
        uint8_t FromACmp0       :1;  /* Wakeup by Analog Comparator 0*/
        uint8_t FromACmp1       :1;  /* Wakeup by Analog Comparator 1*/
        uint8_t FromBOD         :1;  /* Wakeup by Brown-out Detect */

        uint8_t Unused          :4;  /* Unused */

        uint8_t DidPowerDown    :1;  /* set when going to power down */
    } Bits;
} PWR_WakeupReason_t;

/*
 * Name: PWRLib_StackPS_t
 * Description: Zigbee stack power state
 */
typedef enum
{
    StackPS_Running=122,
    StackPS_Sleep,
    StackPS_DeepSleep
} PWRLib_StackPS_t;


/*
 * NAME: pfPWRCallBack_t
 * DESCRIPTION: PWR callback function
 */
typedef void ( *pfPWRCallBack_t ) (void);

/*
 * NAME: tePWR_SleepMode
 * DESCRIPTION: Enumerations for power modes to pass to PWR_ChangeDeepSleepMode
 */
typedef enum
{
    PWR_E_SLEEP_OSCON_RAMON   = 1,  /*32Khz Osc on and Ram On*/
	PWR_E_SLEEP_OSCON_RAMOFF  = 2,  /*32Khz Osc on and Ram off*/
	PWR_E_SLEEP_OSCOFF_RAMON  = 5,  /*32Khz Osc off and Ram on*/
	PWR_E_SLEEP_OSCOFF_RAMOFF = 6,  /*32Khz Osc off and Ram off*/
	PWR_E_SLEEP_INVALID,
} tePWR_SleepMode;

/*
 * NAME: PWR_tsWakeTimerEvent
 * DESCRIPTION: Wake timer struct for use with PWR_eScheduleActivity
 */
typedef struct _sWakeTimerEvt
{
    struct _sWakeTimerEvt *psNext;
    uint32_t u32TickDelta;
    void (*prCallbackfn)(void);
    uint8_t u8Status;
} PWR_tsWakeTimerEvent;

/*
 * NAME: PWR_teStatus
 * DESCRIPTION: Return codes for ex-PWRM functions
 */
typedef enum {
    PWR_E_OK,
    PWR_E_ACTIVITY_OVERFLOW,
    PWR_E_ACTIVITY_UNDERFLOW,
    PWR_E_TIMER_RUNNING,
    PWR_E_TIMER_FREE,
    PWR_E_TIMER_INVALID,
    PWR_E_IO_INVALID,
    PWR_E_MODE_INVALID,
} PWR_teStatus;

/* @brief:  lp_32k_dyn must match the clock_32k_hk_t structure */
typedef struct  {
    uint32_t freq32k;          /*!< 32k clock actual calculated frequency in Hz */
    uint32_t freq32k_16thHz;  /*!< 32k clock actual calculated frequency in 16th of Hz */
    int32_t  ppm_32k;          /*!< the result of 32k clock software calibration in part per million */
} PWR_clock_32k_hk_t;

/*****************************************************************************
 *                        PUBLIC VARIABLES                            *
 *---------------------------------------------------------------------------*
 * Add to this section all the data types definitions: stuctures, unions,    *
 * enumerations, typedefs ...                                                *
 *---------------------------------------------------------------------------*
 *****************************************************************************/

/*****************************************************************************
 *                            PUBLIC FUNCTIONS                               *
 *---------------------------------------------------------------------------*
 * Add to this section all the global functions prototype preceded           *
 * by the keyword 'extern'                                                   *
 *---------------------------------------------------------------------------*
 *****************************************************************************/

/*---------------------------------------------------------------------------
 * Name: PWR_Init
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
extern void PWR_Init(void);

/*---------------------------------------------------------------------------
 * Name: PWR_ChangeDeepSleepMode
 * Description: - Indicate to lowpower mode the BLE mode.
 * - PWR_HandleDeepSleepMode_1 : to be used in advertising mode
 * - PWR_HandleDeepSleepMode_3 : to be used in connected mode
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
extern bool_t PWR_ChangeDeepSleepMode(uint8_t dsMode);

/*---------------------------------------------------------------------------
 * Name: PWR_GetDeepSleepMode
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PWR_GetDeepSleepMode(void);

/*---------------------------------------------------------------------------
 * Name: PWR_GetDeepSleepConfig
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint32_t PWR_GetDeepSleepConfig(void);

/*---------------------------------------------------------------------------
 * Name: PWR_AllowDeviceToSleep
 * Description: - This function is used to reset the global variable which
 *                permits(on SET)/restricts(On RESET)the device to enter
 *                low power state.
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
extern void PWR_AllowDeviceToSleep(void);

/*---------------------------------------------------------------------------
 * Name: PWR_DisallowDeviceToSleep
 * Description: - This function is used to reset the global variable which
 *                permits(on SET)/restricts(On RESET)the device to enter
 *                low power state.
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
extern void PWR_DisallowDeviceToSleep(void);

/*---------------------------------------------------------------------------
 * Name: PWR_DisallowXcvrToSleep
 * Description: - This function is used to reset the global variable which
 *                permits(on SET)/restricts(On RESET)the device to enter
 *                low power state.
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWR_DisallowXcvrToSleep(void);

/*---------------------------------------------------------------------------
 * Name: PWR_AllowXcvrToSleep
 * Description: - This function is used to reset the global variable which
 *                permits(on SET)/restricts(On RESET)the device to enter
 *                low power state.
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWR_AllowXcvrToSleep(void);

/*---------------------------------------------------------------------------
 * Name: PWR_CheckIfDeviceCanGoToSleep
 * Description: - This function is used to  Check  the Permission flag to go to
 *                low power mode
 * Parameters: -
 * Return: - TRUE  : If the device is allowed to go to the LPM else FALSE
 *---------------------------------------------------------------------------*/
extern bool_t PWR_CheckIfDeviceCanGoToSleep(void);

/*---------------------------------------------------------------------------
 * Name: PWR_CheckIfDeviceCanGoToSleepExt
 * Description: - Same than PWR_CheckIfDeviceCanGoToSleep but int8_t
 * Parameters: -
 * Return: - int : >0 error codeTRUE
              = 0 : need to stay active (no WFI)
              = -1 : sleep :WFI only
              <-1  : lowpower
 *---------------------------------------------------------------------------*/
extern int PWR_CheckIfDeviceCanGoToSleepExt(void);

/*---------------------------------------------------------------------------
 * Name: PWR_EnterLowPower
 * Description: - Main entry power to switch to low power mode - checks if power
 * down mode is allowed and enter in power down. If not allowed, will fall back
 * to sleep mode (WFI)
 * This function shall be usually called from the idle task.
 * You can set up the power down mode to use with API PWR_ChangeDeepSleepMode()
 * Parameters: -
 * Return: - wakeup reason
 *---------------------------------------------------------------------------*/
extern PWR_WakeupReason_t PWR_EnterLowPower(void);

/*---------------------------------------------------------------------------
 * Name: PWR_EnterSleep
 * Description: - Enter the chip in WFI state - CPU clock is switched OFF
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
extern void PWR_EnterSleep(void);

/*---------------------------------------------------------------------------
 * Name: PWR_EnterPowerDown
 * Description: - force entering in power down mode without any checks
 * Parameters: -
 * Return: - wakeup reason
 *---------------------------------------------------------------------------*/
extern PWR_WakeupReason_t PWR_EnterPowerDown(void);

/*---------------------------------------------------------------------------
 * Name: PWR_EnterDeepDown
 * Description: - All the chip is OFF - only wakeup by IO or NTAG
 * On wakeup, check the wakeup source using POWER_GetResetCause() API in fsl_power.h
 * and the IO wakeup source with POWER_GetIoWakeStatus() API
 * Parameters: -
 * Return: - never return
 *---------------------------------------------------------------------------*/
extern void PWR_EnterDeepDown(void);

/*---------------------------------------------------------------------------
 * Name: PWR_EnterPowerOff
 * Description: - All the chip is OFF - only wakeup by HW reset
 * Parameters: -
 * Return: - never return
 *---------------------------------------------------------------------------*/
extern void PWR_EnterPowerOff(void);

/*---------------------------------------------------------------------------
 * Name: PWR_SystemReset
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
extern void PWR_SystemReset(void);

/*---------------------------------------------------------------------------
 * Name: PWR_RegisterLowPowerEnterCallback
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWR_RegisterLowPowerEnterCallback(pfPWRCallBack_t lowPowerEnterCallback);

/*---------------------------------------------------------------------------
 * Name: PWR_RegisterLowPowerExitCallback
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWR_RegisterLowPowerExitCallback(pfPWRCallBack_t lowPowerExitCallback);

/*---------------------------------------------------------------------------
 * Name: PWR_EnterSleep
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWR_EnterSleep(void);

/*---------------------------------------------------------------------------
 * Name: PWR_PreventEnterLowPower
 * Description: Forced prevention of entering low-power
 * Parameters: prevent - if TRUE prevents the entering to low-power
 * Return: -
 *---------------------------------------------------------------------------*/
void PWR_PreventEnterLowPower(bool_t prevent);

/*---------------------------------------------------------------------------
 * Name: PWR_GetWakeupReason
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
PWR_WakeupReason_t PWR_GetWakeupReason(void);

/*---------------------------------------------------------------------------
 * Name: PWR_ClearWakeupReason
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWR_ClearWakeupReason(void);

/*---------------------------------------------------------------------------
 * Name: PWR_Start32KCalibration
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWR_Start32KCalibration(void);

/*---------------------------------------------------------------------------
 * Name: PWR_Complete32KCalibration
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint32_t PWR_Complete32KCalibration(uint8_t u8Shift);

/*---------------------------------------------------------------------------
 * Name: PWR_GetHk32kHandle
 * Description: return handle on 32k housekeeping structure.
 * Parameters: -
 * Return: pointer on structure
 *---------------------------------------------------------------------------*/
extern PWR_clock_32k_hk_t *PWR_GetHk32kHandle(void);

/*---------------------------------------------------------------------------
 * Name: PWR_IndicateBLEActive
 * Description: Indicate BLE active. This information is used to determine if
 * wake timer needs to be running when going to a power down mode with the 32k
 * clock running: if BLE is active then wake timer is not needed to be
 * running, otherwise it is.
 * Parameters: bBLE_ActiveIndication - TRUE if BLE is active, FALSE if not
 * Return: -
 *---------------------------------------------------------------------------*/
extern void PWR_IndicateBLEActive(bool_t bBLE_ActiveIndication);

#if defined gLoggingActive_d && (gLoggingActive_d > 0)
#include "dbg_logging.h"
#ifndef DBG_PWR
#define DBG_PWR 0
#endif
#define PWR_DBG_LOG(fmt, ...)   if (DBG_PWR) do { DbgLogAdd(__FUNCTION__ , fmt, VA_NUM_ARGS(__VA_ARGS__), ##__VA_ARGS__); } while (0);
#else
#define PWR_DBG_LOG(...)
#endif

/* Added from PWRM --> */

/**
 * In E_AHI_SLEEP_OSCON_RAMOFF, due to lack retention, all PWRM timer expiring after a wakeup from
 * sleep mode will not have the callback called. Typical scenerio happens when the application
 * uses the second wake timer, if the second timer expires before the PWRM timer programmed by
 * the PWRM_eScheduleActivity function, the callback in PWRM_eScheduleActivity() will not be called
 */

PWR_teStatus PWR_eScheduleActivity(
		PWR_tsWakeTimerEvent *psWake,
        uint32_t u32TimeMs,
        void (*prCallbackfn)(void));

PWR_teStatus PWR_eScheduleActivity32kTicks(
        PWR_tsWakeTimerEvent *psWake,
        uint32_t u32Ticks,
        void (*prCallbackfn)(void));

PWR_teStatus PWR_eRemoveActivity(
        PWR_tsWakeTimerEvent *psWake);

void vAppRegisterPWRCallbacks(void);

/**
 * Shall be called from hardware_init() after the hardware is initiliazed.
 * PWR_vColdStart() does the following actions :
 * - call vAppRegisterPWRMCallbacks
 * - reset the wake timer IP if the reset cause if different from the power down reset
 * - Stop the PWRM timer if the reset cause is the power down reset
 * - call the post wakeup callbacks
 * PWR_vColdStart() does not call the PWR_eScheduleActivity() callback since the RAM was OFF
 */
void PWR_vColdStart(void);

/**
 *  Obsolete function replaced by PWRM_vWakeUpConfig() to support more wakeup sources
 *  kept for backward functionality
 */
PWR_teStatus PWR_vWakeUpIO(uint32_t io_mask);

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
PWR_teStatus PWR_vWakeUpConfig(uint32_t pwr_config);

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
void PWR_vForceRamRetention(uint32_t u32RetainBitmap);

/**
 * Force some RAM banks to be held in retention when going to sleep mode in sleep modes
 * E_AHI_SLEEP_OSCON_RAMON and E_AHI_SLEEP_OSCOFF_RAMON.
 * Unlike PWR_vForceRamRetention, which uses a bitmap of banks to retain and replaces any
 * previous configuration, this function takes the start and length of a region and adds
 * the banks that this uses to the existing configuration. As such, it can be called
 * multiple times for different regions, accumulating the total set of banks needed as it
 * goes.
 */
void PWR_vAddRamRetention(uint32_t u32Start, uint32_t u32Length);

/**
 * Get current RAM retention setting.
 */
uint32_t PWR_u32GetRamRetention(void);

/**
 * Force radio registers to be retained through sleep. Enabled if bRetain is
 * TRUE, disabled if bRetain is FALSE.
 * Configuration is cleared after a call to PWRM_vInit(). Application should
 * call the API again if PWRM_vInit() is called.
 */
void PWR_vForceRadioRetention(bool_t bRetain);

/**
 * Reset Lowpower Mode config
 * to be called when lower configuration to go to powerdown with RAM On has changed
 */
void PWR_ResetPowerDownModeConfig(void);

/* <-- Added from PWRM */

void PWR_Start32kCounter(void);
uint32_t PWR_Get32kTimestamp(void);



/* <--Added for freeRTOS tickless mode */

#ifdef __cplusplus
}
#endif

#endif /* _PWR_INTERFACE_H_ */
