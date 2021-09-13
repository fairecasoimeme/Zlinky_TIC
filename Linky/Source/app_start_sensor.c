/****************************************************************************
 *
 * MODULE:             JN-AN-1220 ZLO Sensor Demo
 *
 * COMPONENT:          app_start_sensor.c
 *
 * DESCRIPTION:        ZLO Sensor Application Initialisation and Startup
 *
 ****************************************************************************
 *
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5168, JN5169,
 * JN5179, JN5189].
 * You, and any third parties must reproduce the copyright and warranty notice
 * and any other legend of ownership on each copy or partial copy of the
 * software.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright NXP B.V. 2016-2019. All rights reserved
 *
 ***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "fsl_wwdt.h"
#include "fsl_iocon.h"
#include "fsl_gpio.h"
#include "fsl_power.h"
#include "pin_mux.h"

#ifdef APP_LOW_POWER_API
#include "PWR_interface.h"
#else
#include "pwrm.h"
#endif

#include "pdum_nwk.h"
#include "pdum_apl.h"
#include "PDM.h"
#include "dbg.h"
#include "pdum_gen.h"
#include "zps_gen.h"
#include "zps_apl_af.h"
#include "AppApi.h"
#include "app_zlo_sensor_node.h"
#include "app_main.h"
#include "app_linky_buttons.h"
#include <string.h>
#if ((defined OM15081R1) || (defined OM15081R2))
    #ifdef OM15081R1
        #include "TSL2550.h"
    #endif
    #ifdef OM15081R2
        #include "TSL25721.h"
    #endif
#else
    #error Light Sensor driver not set!
#endif
#include "bdb_api.h"
#include "fsl_os_abstraction.h"
#include "DebugExceptionHandlers_jn518x.h"
#ifdef APP_NTAG_ICODE
#include "ntag_nwk.h"
#include "app_ntag_icode.h"
#if (defined JENNIC_CHIP_FAMILY_JN518x)
#include "fsl_power.h"
#endif
#endif
#include "app.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef DEBUG_START_UP
    #define TRACE_START FALSE
#else
    #define TRACE_START TRUE
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

PRIVATE void vInitialiseApp(void);
#ifdef SLEEP_ENABLE
    PRIVATE void vSetUpWakeUpConditions(void);
#endif
PRIVATE void vfExtendedStatusCallBack (ZPS_teExtendedStatus eExtendedStatus);
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
extern void *_stack_low_water_mark;

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
/**
 * Power manager Callbacks
 */
#ifdef APP_LOW_POWER_API
static void PreSleep(void);
static void Wakeup(void);
#else
static PWRM_DECLARE_CALLBACK_DESCRIPTOR(PreSleep);
static PWRM_DECLARE_CALLBACK_DESCRIPTOR(Wakeup);
#endif

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/


/****************************************************************************
 *
 * NAME: vAppMain
 *
 * DESCRIPTION:
 * Entry point for application from a cold start.
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void vAppMain(void)
{
    wwdt_config_t  config;

    DBG_vPrintf(TRACE_START, "\r\nSTART: vAppMain() ENTER");

    /* Initialise exception handlers for debugging */
    vDebugExceptionHandlersInitialise();

    /* Catch resets due to watchdog timer expiry. Comment out to harden code. */
    WWDT_GetDefaultConfig(&config);

    if (((PMC->RESETCAUSE) & PMC_RESETCAUSE_WDTRESET_MASK) == PMC_RESETCAUSE_WDTRESET_MASK)
    {
        DBG_vPrintf(TRACE_START, "\r\nSTART: vAppMain(), Watchdog timer has reset device!");
#ifdef HALT_ON_EXCEPTION
        /* Enable the WWDT clock */
        CLOCK_EnableClock(kCLOCK_WdtOsc);
        RESET_PeripheralReset(kWWDT_RST_SHIFT_RSTn);
        WWDT_Deinit(WWDT);
        POWER_ClearResetCause();
        while (1);
#else
        /* Clear flag */
        PMC->RESETCAUSE = PMC_RESETCAUSE_WDTRESET_MASK;
#endif
    }

    /* initialise ROM based software modules */
    #ifndef JENNIC_MAC_MiniMacShim
    u32AppApiInit(NULL, NULL, NULL, NULL, NULL, NULL);
    #endif

    DBG_vPrintf(TRACE_START, "\r\nSTART: vAppMain() -> APP_vInitResources()");
    APP_vInitResources();

    DBG_vPrintf(TRACE_START, "\r\nSTART: vAppMain() -> APP_vInitialise()");
    vInitialiseApp();

#if (defined APP_NTAG_ICODE)
    DBG_vPrintf(TRACE_START, "\r\nSTART: vAppMain() -> APP_vNtagPdmLoad()");
    /* Didn't start BDB using PDM data ? */
    if (FALSE == APP_bNtagPdmLoad())
#endif
    {
        DBG_vPrintf(TRACE_START, "\r\nSTART: vAppMain() -> BDB_vStart()\n");
        BDB_vStart();
        if (FALSE == APP_bNodeIsInRunningState()) BDB_eNsStartNwkSteering();
    }

#ifdef APP_NTAG_ICODE
#if (defined JENNIC_CHIP_FAMILY_JN518x)
    /* Not waking from deep sleep ? */
    if (0 == (POWER_GetResetCause() & RESET_WAKE_DEEP_PD))
#else
    /* Not waking from deep sleep ? */
    if (0 == (u16AHI_PowerStatus() & (1 << 11)))
#endif
    {
        DBG_vPrintf(TRACE_START, "\r\nSTART: vAppMain() -> APP_vNtagStart()");
        APP_vNtagStart(LIGHTSENSOR_SENSOR_ENDPOINT);
    }
#endif

    /*switch to XTAL 32M for better timing accuracy*/
    SYSCON -> MAINCLKSEL    =  2;  /* 32 M XTAL */
    SystemCoreClockUpdate();
    OSA_TimeInit();
	
    DBG_vPrintf(TRACE_START, "\r\nSTART: vAppMain() EXIT");
}

/****************************************************************************
 *
 * NAME: vAppRegisterPWRCallbacks
 *
 * DESCRIPTION:
 * Power manager callback.
 * Called to allow the application to register  sleep and wake callbacks.
 *
 * PARAMETERS:      Name            RW  Usage
 *
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void vAppRegisterPWRCallbacks(void)
{
#ifdef APP_LOW_POWER_API
    PWR_RegisterLowPowerEnterCallback(PreSleep);
    PWR_RegisterLowPowerExitCallback(Wakeup);
#else
    PWRM_vRegisterPreSleepCallback(PreSleep);
    PWRM_vRegisterWakeupCallback(Wakeup);
#endif
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: vInitialiseApp
 *
 * DESCRIPTION:
 * Initialises Zigbee stack, hardware and application.
 *
 *
 * RETURNS:
 * void
 ****************************************************************************/
PRIVATE void vInitialiseApp(void)
{

    /*
     * Initialise JenOS modules. Initialise Power Manager even on non-sleeping nodes
     * as it allows the device to doze when in the idle task.
     * Parameter options: E_AHI_SLEEP_OSCON_RAMON or E_AHI_SLEEP_DEEP or ...
     */
#ifdef APP_LOW_POWER_API
    (void) PWR_ChangeDeepSleepMode(PWR_E_SLEEP_OSCON_RAMON);
    PWR_Init();
    PWR_vForceRadioRetention(TRUE);
#else
    PWRM_vInit(E_AHI_SLEEP_OSCON_RAMON);
    PWRM_vForceRadioRetention(TRUE);
#endif

    PDM_eInitialise(1200, 63, NULL);

    /* Initialise Protocol Data Unit Manager */
    PDUM_vInit();

    ZPS_vExtendedStatusSetCallback(vfExtendedStatusCallBack);

    /* Initialise application */
    APP_vInitialiseNode();
}

/****************************************************************************
 *
 * NAME: vfExtendedStatusCallBack
 *
 * DESCRIPTION:
 *
 * ZPS extended error callback .
 *
 * PARAMETERS:      Name            RW  Usage
 *
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vfExtendedStatusCallBack (ZPS_teExtendedStatus eExtendedStatus)
{
    DBG_vPrintf(TRACE_START, "\r\nSTART: vfExtendedStatusCallBack(%x)", eExtendedStatus);
}

/****************************************************************************
 *
 * NAME: vSetUpWakeUpConditions
 *
 * DESCRIPTION:
 *
 * Set up the wake up inputs while going to sleep.
 *
 * PARAMETERS:      Name            RW  Usage
 *
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void vSetUpWakeUpConditions(void)
{
   /*
    * Set the DIO with the right edges for wake up
    * */
#ifdef APP_LOW_POWER_API
    PWR_vWakeUpConfig(APP_BUTTONS_DIO_MASK);
#else
    PWRM_vWakeUpConfig(APP_BUTTONS_DIO_MASK);
#endif
}

/****************************************************************************
 *
 * NAME: PreSleep
 *
 * DESCRIPTION:
 *
 * PreSleep call back by the power manager before the controller put into sleep.
 *
 * PARAMETERS:      Name            RW  Usage
 *
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
#ifdef APP_LOW_POWER_API
static void PreSleep(void)
#else
PWRM_CALLBACK(PreSleep)
#endif
{
    DBG_vPrintf(TRACE_START, "\r\nSTART: PWRM_CALLBACK(PreSleep), OSA_TimeGetMsec()=%d", OSA_TimeGetMsec());

    /* Set up wake up input */
    vSetUpWakeUpConditions();

    /* Save the MAC settings (will get lost though if we don't preserve RAM) */
    vAppApiSaveMacSettings();

    /* Put ZTimer module to sleep (stop tick timer) */
    ZTIMER_vSleep();

#if 0
    /* Turn off white LED */
    GPIO_PinWrite(GPIO, 0, 6, 0);
#endif

    /* Turn off LEDs */
    GPIO_PinWrite(GPIO, 0, APP_BASE_BOARD_LED1_PIN, 1); // Off

    /* Turn off debug */
    DbgConsole_Deinit();

    /* Minimize GPIO power consumption */
    BOARD_SetPinsForPowerMode();
}

/****************************************************************************
 *
 * NAME: Wakeup
 *
 * DESCRIPTION:
 *
 * Wakeup call back by  power manager after the controller wakes up from sleep.
 *
 ****************************************************************************/
#ifdef APP_LOW_POWER_API
static void Wakeup(void)
#else
PWRM_CALLBACK(Wakeup)
#endif
{

    APP_vSetUpHardware();
//    DBG_vPrintf(TRACE_START, "\r\nSTART: PWRM_CALLBACK(Wakeup), powerStatus = 0x%04x, OSA_TimeGetMsec()=%d", PMC->RESETCAUSE, OSA_TimeGetMsec());

    /* Initialise RGB LED */
    GPIO_PinWrite(GPIO, 0, APP_BASE_BOARD_LED1_PIN, 1); // Off

    /* Always initialise the light sensor and the RGB Led
     * the iic interface runs slower (100kHz) than that used
     * by the RGB driver (400KHz)
     * */
#ifdef OM15081R1
//    DBG_vPrintf(TRACE_START, "\r\nSTART: PWRM_CALLBACK(Wakeup) -> bTSL2550_Init() =");
/*    bool_t bStatus=*/
    bTSL2550_Init();
//    DBG_vPrintf(TRACE_START, " %d", bStatus);
#endif
#ifdef OM15081R2
//    DBG_vPrintf(TRACE_START, "\r\nSTART: PWRM_CALLBACK(Wakeup) -> bTSL25721_Init() =");
/*    bool_t bStatus=*/
    bTSL25721_Init();
//    DBG_vPrintf(TRACE_START, " %d", bStatus);
#endif

    /* If the power status is OK and RAM held while sleeping, restore the MAC settings */
    if((PMC->RESETCAUSE & ( PMC_RESETCAUSE_WAKEUPIORESET_MASK |
            PMC_RESETCAUSE_WAKEUPPWDNRESET_MASK ) ))
    {
//        DBG_vPrintf(TRACE_START, "\r\nSTART: PWRM_CALLBACK(Wakeup) -> vMAC_RestoreSettings()");
        /* Restore Mac settings (turns radio on) */

        vAppApiRestoreMacSettings();

        /* Define HIGH_POWER_ENABLE to enable high power module */
        #ifdef HIGH_POWER_ENABLE
            vAHI_HighPowerModuleEnable(TRUE, TRUE);
        #endif

        ZTIMER_vWake();

        /* Initialise buttons */
        APP_bButtonInitialise();
    }
}

/****************************************************************************
 *
 * NAME: hardware_init
 *
 * DESCRIPTION:
 *
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void hardware_init(void)
{
    static bool_t bColdStarted = FALSE;

    if (FALSE == bColdStarted)
    {
        APP_vSetUpHardware();
#ifdef APP_LOW_POWER_API
        PWR_vColdStart();
#else
        PWRM_vColdStart();
#endif
        bColdStarted = TRUE;
    }

#if 0
    /* Turn on white LED */
    gpio_pin_config_t output_config = {kGPIO_DigitalOutput, 0};
    GPIO_PinInit(GPIO, 0, 6, &output_config);
    GPIO_PinWrite(GPIO, 0, 6, 1);
#endif
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
