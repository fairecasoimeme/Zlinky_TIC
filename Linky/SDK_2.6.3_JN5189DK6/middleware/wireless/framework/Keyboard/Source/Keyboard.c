/*! *********************************************************************************
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2017 NXP
* All rights reserved.
*
* \file
*
* Keyboard implementation file for ARM CORTEX-M4 processor.
* The keyboard handling logic can understand one or more keys
* pressed simultaneously.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#include "EmbeddedTypes.h"
#include "TimersManager.h"
#include "GPIO_Adapter.h"
#include "gpio_pins.h"
#include "board.h"
#include "pin_mux.h"

#include "fsl_os_abstraction.h"
#include "Keyboard.h"
#if (cPWR_FullPowerDownMode)
#include "PWR_Interface.h"
#endif

#define PreventDSForDebounce (0 && cPWR_FullPowerDownMode)

#if (cPWR_FullPowerDownMode)
#define PWR_ALLOW_DEVICE_TO_SLEEP PWR_AllowDeviceToSleep()
#define PWR_DISALLOW_DEVICE_TO_SLEEP PWR_DisallowDeviceToSleep()
#else
#define PWR_ALLOW_DEVICE_TO_SLEEP
#define PWR_DISALLOW_DEVICE_TO_SLEEP
#endif

extern const kbdSwitchConfig_t kbdSwitchButtons[];

#if gKeyBoardSupported_d && gKBD_TsiElectdCount_c
#include "fsl_tsi_v4.h" /*include TSI driver*/
#endif

#include "fsl_debug_console.h"

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/
#ifndef BIT
#define BIT(x)               (1 << (x))
#endif
/*
* Hardware settings
*/

#define gKeyboard_IsrPrio_c (0x80)

/* Masks for keyboard Switches */
#define gSWITCH1_MASK_c                 0x01
#define gSWITCH2_MASK_c                 0x02
#define gSWITCH3_MASK_c                 0x04
#define gSWITCH4_MASK_c                 0x08
#if gKeyBoardSupported_d
#if gKBD_TsiElectdCount_c
#define TSI_ELECTRODE_THRESH (0x100)
typedef struct kbdTsiDescriptor_tag
{
    uint16_t elecBaseValue;
    uint8_t elecName;
    bool_t  elecValueAboveThresh;
}kbdTsiDescriptor_t; /* descriptor structure for TSI electrodes */
#endif
#endif

#if gKeyBoardSupported_d
/*
 * Name: mNoKey_c
 * Description: no key macro definition
 */
#define mNoKey_c 0xff
#endif /* gKeyBoardSupported_d */


/* Configuration check */

#if (gKeyBoardSupported_d) && (gKBD_KeysCount_c > 4)
#error "Cannot support more than 4 switches"
#endif

#if (gKeyBoardSupported_d) && (gKBD_KeysCount_c == 0)
#warning "KEYBOARD module is enabled but the pushbuttons count is ZERO"
#endif

#if (gKeyBoardSupported_d) && (!gTMR_Enabled_d)
#warning "Keyboard scan cannot operate without the TIMER platform component"
#endif

#if (gKBD_TsiElectdCount_c) && !defined(gKBD_SwTypeMapping_d)
#error "The gKBD_SwTypeMapping_d is not defined!"
#endif

#if (gKBD_TsiElectdCount_c) && !defined(gKBD_TsiElecList_d)
#error "The gKBD_TsiElecList_d is not defined!"
#endif

#define gKBD_ButtonCount_c (gKBD_KeysCount_c - gKBD_TsiElectdCount_c)


/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/
#if gKeyBoardSupported_d
/*
 * Name: KeyState_t
 * Description: enumerated data type for key states
 */
#if (gKeyEventNotificationMode_d == gKbdEventShortLongPressMode_c)

typedef enum tag_KeyState{
    mStateKeyIdle,        /* coming in for first time */
    mStateKeyDetected,    /* got a key, waiting to see if it's a long key */
    mStateKeyWaitRelease  /* got the long key, waiting for the release to go back to idle */
}KeyState_t;

#elif (gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c)

typedef enum tag_KeyState {
    mStateKeyDebounce,
    mStateKeyHoldDetection,
    mStateKeyHoldGen,
} KeyState_t;

#endif /* gKeyEventNotificationMode_d */
#endif /* gKeyBoardSupported_d*/

#if gKeyBoardSupported_d
/*
 * Name:
 * Description: switches scan result definition
 */
typedef uint32_t    switchScan_t;
#endif /* gKeyBoardSupported_d */

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/
#if (gKeyBoardSupported_d) && (gKBD_KeysCount_c)
kbdSwitchConfig_t kbdSwButtons[gKBD_KeysCount_c];
#if gKBD_TsiElectdCount_c
static uint8_t kbdNumberOfElectrodes = 0; /* number of installed electrodes */
/* number of defined electrodes */
static kbdTsiDescriptor_t kbdTsiElectrodes[gKBD_TsiElectdCount_c];
#endif
#endif

/******************************************************************************
*******************************************************************************
* Private prototypes
*******************************************************************************
******************************************************************************/

#if gKeyBoardSupported_d
/******************************************************************************
 * Name: KbGpioInit
 * Description: Initialize the GPIOs used by the keyboard (switches)
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
static void KbGpioInit(void);

#if gKBD_TsiElectdCount_c
#if gTMR_Enabled_d
/******************************************************************************
 * Name: TsiSwTriggerCallback
 * Description: Timer callback to start software trigger on TSI channel
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
static void TsiSwTriggerCallback(void* params);
#endif

/******************************************************************************
 * Name: KbTsiInit
 * Description: Initialize the TSI module used by the keyboard
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
static void KbTsiInit(void);

/******************************************************************************
 * Name: KBD_TSI_IrqHandler
 * Description: Interrupt handler for TSI interrupts.
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
static void KBD_TSI_IrqHandler(void);
#endif

#if gTMR_Enabled_d

/******************************************************************************
 * Name: KBD_KeySwitchPortGet
 * Description: Gets the switch port
 * Parameter(s): -
 * Return: switch port value (pressed / not pressed keys)
 ******************************************************************************/
static switchScan_t KBD_KeySwitchPortGet(void);

/******************************************************************************
 * Name: KBD_KeyCheck
 * Description: Called to check if a key is still pressed
 * Parameter(s): [IN] previousPressed - previously pressed key
 * Return: TRUE if the key passed as argument is still pressed, FALSE otherwise
 ******************************************************************************/
#if ((gKeyEventNotificationMode_d == gKbdEventShortLongPressMode_c) ||  \
    (gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c))
static bool_t KBD_KeyCheck(switchScan_t previousPressed);
#endif /* #if ((gKeyEventNotificationMode_d == gKbdEventShortLongPressMode_c) ||    \
    (gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c)) */

/******************************************************************************
 * Name: KeyScan
 * Description: scan the keyboard switches and detects key press/hold/release
 *              or short/long press
 * Parameter(s): [IN]timerId - key scan timer ID
 * Return: -
 ******************************************************************************/
static void KeyScan(uint8_t timerId);
#endif /* #if gTMR_Enabled_d */

static bool_t Switch_CheckIRQ(uint32_t idx);
#endif /* gKeyBoardSupported_d */

/******************************************************************************
*******************************************************************************
* Private memory declarations
*******************************************************************************
******************************************************************************/

#if gKeyBoardSupported_d

static bool mKbdServiceOpened = false;

#if (gKeyEventNotificationMode_d == gKbdEventPressOnly_c)
/*
 * Name: mKeyPressed
 * Description: Keys pressed mask; each bit represents a key / electrode;
 *             (i.e. bit0 -> KEY0, bit1 -> KEY1, etc)
 */
static volatile uint16_t mKeyPressed;
#endif /* gKeyEventNotificationMode_d == gKbdEventPressOnly_c */

#if (gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c)

#if gKeyBoardSupported_d
#if gTMR_Enabled_d
/*
 * Name: mKbdHoldDectionKeyCount
 * Description: counter used for key hold detection
 */
static uint16_t       mKbdHoldDectionKeyCount;

/*
 * Name: mKbdHoldGenKeyCount
 * Description: counter used for key hold generation
 */
static uint16_t       mKbdHoldGenKeyCount;
#endif /* #if gTMR_Enabled_d */
#endif /* gKeyBoardSupported_d */
#endif /* gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c */


/*
 * Name: mWakeportScan
 * Description: bit field of keys having provoked a wake-up
 */
static uint32_t mWakePortScan;

#if ( (gKeyEventNotificationMode_d == gKbdEventShortLongPressMode_c) || \
      (gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c) )

#if gKeyBoardSupported_d
#if gTMR_Enabled_d
/*
 * Name: mSwitch_SCAN
 * Description: switch scan result
 */
static uint32_t mSwitch_SCAN;

#if (gKeyEventNotificationMode_d == gKbdEventShortLongPressMode_c)
/*
 * Name: mKeyState
 * Description: key scan state variable used in the scanning state machine
 */
static uint8_t mKeyState = mStateKeyIdle;

/*
 * Name: mLongKeyCount
 * Description: self explanatory
 */
static uint32_t mLongKeyCount;
#elif (gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c)
/*
 * Name: mKeyState
 * Description: key scan state variable used in the scanning state machine
 */
static uint8_t mKeyState = mStateKeyDebounce;

#endif /* gKeyEventNotificationMode_d */
#endif /* #if gTMR_Enabled_d */
#endif /* gKeyBoardSupported_d */
#endif /* (gKeyEventNotificationMode_d == gKbdEventShortLongPressMode_c) || \
          (gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c) */

/*
 * Name: pressedKey
 * Description: pressed key number
 */
#if gKeyBoardSupported_d
#if gTMR_Enabled_d
static uint8_t pressedKey;
#endif
#endif

/*
 * Name: mKeyScanTimerID
 * Description: timer ID used for key scanning
 */
tmrTimerID_t mKeyScanTimerID = gTmrInvalidTimerID_c;
#if gKBD_TsiElectdCount_c
tmrTimerID_t mTsiSwTriggerTimerID = gTmrInvalidTimerID_c;
#endif

/*
 * Name: mpfKeyFunction
 * Description: pointer to the application callback function
 */
static KBDFunction_t mpfKeyFunction = NULL;

#endif /* gKeyBoardSupported_d */

/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/

#if gKeyBoardSupported_d
#if gTMR_Enabled_d
#if defined (CPU_JN518X) && PreventDSForDebounce
volatile int TMR_lock = 0;
#endif
static void KBD_TimerStart(void)
{
    KBD_DBG_LOG("");
#if defined (CPU_JN518X) && PreventDSForDebounce
    OSA_DisableIRQGlobal();
    TMR_lock++;
    if (TMR_lock == 1)
    {
        PWR_DISALLOW_DEVICE_TO_SLEEP();
    }
    OSA_EnableIRQGlobal();
#endif  /* PreventDSForDebounce */
    TMR_StartIntervalTimer(mKeyScanTimerID,
                           gKeyScanInterval_c,
                           (pfTmrCallBack_t)KeyScan,
                           (void*)((uint32_t)mKeyScanTimerID));
}

static void KBD_TimerStop(void)
{
    KBD_DBG_LOG("");
#if defined (CPU_JN518X) && PreventDSForDebounce
    OSA_DisableIRQGlobal();
    if (TMR_lock > 0)
    {
        TMR_lock--;
        if (TMR_lock == 0)
        {
            PWR_ALLOW_DEVICE_TO_SLEEP;
        }
    }
    OSA_EnableIRQGlobal();
#endif /* PreventDSForDebounce */
    TMR_StopTimer(mKeyScanTimerID);

}
#endif
/******************************************************************************
 * Name: KbGpioInit
 * Description: Initialize the GPIOs used by the keyboard (switches)
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
static void KbGpioInit(void)
{
    KBD_DBG_LOG("");

    uint8_t idx = 0;
    for(idx = 0; idx < gKBD_KeysCount_c; idx++)
    {
        if(kbdSwButtons[idx].swType == gKBDTypeGpio_c)
        {
            (void)GpioInputPinInit(kbdSwButtons[idx].config_struct.pSwGpio, 1);
        }
    }
}

#if gKBD_TsiElectdCount_c
/******************************************************************************
* Name: KbTsiInit
* Description: Initialize the TSI module used by the keyboard
* Parameter(s): -
* Return: -
******************************************************************************/
static void KbTsiInit(void)
{
    uint8_t i;
    tsi_config_t tsiConfig_normal = {0};

    TSI_GetNormalModeDefaultConfig(&tsiConfig_normal); /*use normal config */

    TSI_Init(TSI0, &tsiConfig_normal); /*init module*/

    TSI_EnableModule(TSI0, true); /* Enable module */

    kbdNumberOfElectrodes = 0;

    for (i = 0U; i < gKBD_KeysCount_c; i++) /* populate electrodes table */
    {
        if (kbdSwButtons[i].swType == gKBDTypeTsi_c)
        {
            kbdTsiElectrodes[kbdNumberOfElectrodes].elecName =
                kbdSwButtons[i].config_struct.swTsiElectrodeId;
            kbdNumberOfElectrodes++;
        }
    }

    for(i = 0U; i < kbdNumberOfElectrodes; i++)
    {
        TSI_SetMeasuredChannelNumber(TSI0,
                                     kbdTsiElectrodes[i].elecName);
        TSI_StartSoftwareTrigger(TSI0);
        while (!(TSI_GetStatusFlags(TSI0) & kTSI_EndOfScanFlag))
        {
        }
        kbdTsiElectrodes[i].elecBaseValue = TSI_GetCounter(TSI0);
        TSI_ClearStatusFlags(TSI0, kTSI_EndOfScanFlag);
    } /* Calibrate TSI. Do not touch electrodes in the process */

    OSA_InstallIntHandler(TSI0_IRQn, KBD_TSI_IrqHandler); /*install TSI interrupt handler*/

    NVIC_SetPriority(TSI0_IRQn, gKeyboard_IsrPrio_c); /* use keyboard interrupt priority */
    NVIC_EnableIRQ(TSI0_IRQn); /* enable TSI interrupt */

    TSI_EnableHardwareTriggerScan(TSI0, false); /* Enable software trigger scan */
    /* Enable module interrupts */
    TSI_EnableInterrupts(TSI0, kTSI_GlobalInterruptEnable);
    TSI_EnableInterrupts(TSI0, kTSI_EndOfScanInterruptEnable);
    TSI_ClearStatusFlags(TSI0, kTSI_EndOfScanFlag);
}
#endif

/******************************************************************************
* Name: KBD_Deinit
* Description: -
* Parameter(s): -
* Return: -
******************************************************************************/
void KBD_Deinit(void)
{
    KBD_DBG_LOG("");
    uint8_t i;
    TMR_FreeTimer(mKeyScanTimerID);

    for( i=0; i<gKBD_KeysCount_c; i++ )
    {
        if(kbdSwButtons[i].swType == gKBDTypeGpio_c)
        {
            GpioUninstallIsr( kbdSwButtons[i].config_struct.pSwGpio );
        }
    }
    BOARD_UnInitButtons();
#if gKBD_TsiElectdCount_c
    TMR_FreeTimer(mTsiSwTriggerTimerID); /* free software trigger timer */

    TSI_DisableInterrupts(TSI0, kTSI_GlobalInterruptEnable);
    TSI_DisableInterrupts(TSI0, kTSI_EndOfScanInterruptEnable);
    TSI_ClearStatusFlags(TSI0, kTSI_EndOfScanFlag);

    NVIC_DisableIRQ(TSI0_IRQn); /* disable interrupt from NVIC */

    TSI_EnableModule(TSI0, false);
    TSI_Deinit(TSI0);
#endif
}

static uint32_t GetPressedButtonsMask(void)
{
   uint32_t res = 0;
   int i;
    for( i=0; i<gKBD_KeysCount_c; i++ )
    {
       switch (kbdSwButtons[i].swType) {

       case gKBDTypeGpio_c:
       {
          if(GpioReadPinInput(kbdSwButtons[i].config_struct.pSwGpio) == 0)
          {
             res |= BIT(i);
          }
       }
       break;

#if gKBD_TsiElectdCount_c
       case gKBDTypeTsi_c:
        {
           int j;
           for(j = 0; j < kbdNumberOfElectrodes; j++)
            {
                if(kbdTsiElectrodes[j].elecName ==
                   kbdSwButtons[i].config_struct.swTsiElectrodeId)
                {
                    if(kbdTsiElectrodes[j].elecValueAboveThresh)
                    {
                        res |= BIT(i);
                    }
                }
            }
        }
        break;
#endif
        default:
           break;
       } /* switch */
    }
    return res;
}

#if gTMR_Enabled_d

/******************************************************************************
* Name: KBD_KeySwitchPortGet
* Description: Gets the switch port
* mWakePortScan is the mask of IO that have triggered a wake up. Is cleared on exit.
* Parameter(s): -
* Return: switch port value (pressed / not pressed keys)
******************************************************************************/
static switchScan_t KBD_KeySwitchPortGet(void)
{
    uint32_t portScan;
    pressedKey = mNoKey_c;

    portScan = mWakePortScan;
    portScan |= GetPressedButtonsMask();
    for(int i=0; i<gKBD_KeysCount_c; i++ )
    {
        if((portScan & BIT(i)) && (pressedKey == mNoKey_c)) /*first detected key*/
        {
            pressedKey = i;
        }
    }
    mWakePortScan = 0; /* clear after event consumed */
    KBD_DBG_LOG("portScan=%x pressedKey=%d", portScan, pressedKey);

    return portScan;
}

/******************************************************************************
 * Name: KBD_KeyCheck
 * Description: Called to check if a key is still pressed
 * Parameter(s): [IN] previousPressed - previously pressed key
 * Return: TRUE if the key passed as argument is still pressed, FALSE otherwise
 ******************************************************************************/
#if ((gKeyEventNotificationMode_d == gKbdEventShortLongPressMode_c) ||  \
    (gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c))
static bool_t KBD_KeyCheck(switchScan_t previousPressed)
{
    bool_t pressed = FALSE;
    uint32_t portScan;
    uint8_t key = mNoKey_c;

    previousPressed=previousPressed; /* avoid compiler warnings */

    portScan = GetPressedButtonsMask();

    for(int i=0; i<gKBD_KeysCount_c; i++)
    {
        if((portScan & (1<<i)) != 0)
        {
            key = i;
            break;
        }
    }
    /* Check if the switch is still pressed */
    if (pressedKey == key)
    {
        pressed = TRUE;
    }
    mWakePortScan = 0;
    KBD_DBG_LOG("previousPressed=%d still_pressed=%d key=%x", pressedKey, pressed, key);
    return pressed;
}
#endif /*#if ((gKeyEventNotificationMode_d == gKbdEventShortLongPressMode_c) || \
    (gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c))*/


/******************************************************************************
 * Name: KeyScan
 * Description: scan the keyboard switches and detects key press/hold/release
 *              or short/long press
 * Parameter(s): [IN]timerId - key scan timer ID
 * Return: -
 ******************************************************************************/
#if (gKeyEventNotificationMode_d == 0)
static void KeyScan(uint8_t timerId)
{
    if(KBD_KeySwitchPortGet())
    {
        mpfKeyFunction(BIT(pressedKey)); /* bits 0..3 are for keyboard */
    }
}
#elif (gKeyEventNotificationMode_d == gKbdEventPressOnly_c)
static void KeyScan(uint8_t timerId)
{
    if(KBD_KeySwitchPortGet())
    {
        KBD_TimerStop();
        mpfKeyFunction(BIT(pressedKey)); /* bits 0..3 are for keyboard */
    }
}

#elif (gKeyEventNotificationMode_d == gKbdEventShortLongPressMode_c)
static void KeyScan(uint8_t timerId)
{
    uint8_t keyBase;
    uint32_t portScan;

#if DBG_KBD
    uint8_t prev_key_state = mKeyState;
#endif
    switch(mKeyState)
    {
    /* got a fresh key */
    case mStateKeyIdle:
        PWR_DISALLOW_DEVICE_TO_SLEEP;
        mSwitch_SCAN = KBD_KeySwitchPortGet();
        if(mSwitch_SCAN != 0)
        {
            mKeyState = mStateKeyDetected;
            mLongKeyCount = 0;
        }
        else
        {
            KBD_TimerStop();
            PWR_ALLOW_DEVICE_TO_SLEEP;

        }
        break;

    /* a key was detected. Has it been released or still being pressed? */
    case mStateKeyDetected:
        keyBase = 0;  /* assume no key */

        /* Check to see if the key is still pressed. Ignore other pressed keys */
        if( KBD_KeyCheck(mSwitch_SCAN) )
        {
            mLongKeyCount++;
        }
        else
        {
            /* short key press */
            keyBase = gKBD_EventPB1_c;

            /* validate long key count*/
            if(mLongKeyCount >= gKbdLongKeyIterations_c)
            {
                keyBase = gKBD_EventLongPB1_c;
            }
            if(mLongKeyCount >= gKbdVeryLongKeyIterations_c)
            {
                keyBase = gKBD_EventVeryLongPB1_c;
            }
        }

        if(keyBase)
        {
            /* if a key was pressed, send it */
            if(pressedKey != mNoKey_c)
            {
                mpfKeyFunction(keyBase + pressedKey);
            }

            /* whether we sent a key or not, wait to go back to keyboard  */
            mKeyState = mStateKeyWaitRelease;
        }
        break;

    /* got the long key, waiting for the release to go back to idle */
    case mStateKeyWaitRelease:
        /* wait for the release before going back to idle */
        portScan = GetPressedButtonsMask();

        if((mSwitch_SCAN == 0) || (portScan == 0))
        {
            mKeyState = mStateKeyIdle;
            KBD_TimerStop();
            PWR_ALLOW_DEVICE_TO_SLEEP;
        }
        break;
    default:
        break;
    }
#if DBG_KBD
    KBD_DBG_LOG("mKeyState: prev=%x, new=%x mSwitchScan=%x", prev_key_state, mKeyState, mSwitch_SCAN);
#endif

}

#elif (gKeyEventNotificationMode_d == gKbdEventPressHoldReleaseMode_c)
static void KeyScan(uint8_t timerId)
{
    uint32_t portScan, i;

    switch(mKeyState)
    {
    case mStateKeyDebounce:
        mSwitch_SCAN = KBD_KeySwitchPortGet();
        if(mSwitch_SCAN != 0)
        {
            mKeyState =  mStateKeyHoldDetection;
            mKbdHoldDectionKeyCount = 0;
            //Generate press event indication
            if(pressedKey != mNoKey_c)
            {
                mpfKeyFunction(gKBD_EventPressPB1_c + pressedKey);
            }
        }
        else
        {
            KBD_TimerStop();
        }
        break;
    case mStateKeyHoldDetection:
        if( KBD_KeyCheck(mSwitch_SCAN) )
        {
            mKbdHoldDectionKeyCount++;
            if (mKbdHoldDectionKeyCount >= gKbdFirstHoldDetectIterations_c)
            {
                //first hold event detected - generate hold event
                if(pressedKey != mNoKey_c)
                {
                    mpfKeyFunction(gKBD_EventHoldPB1_c + pressedKey);
                }
                mKbdHoldGenKeyCount = 0;
                mKeyState = mStateKeyHoldGen;
            }
        }
        else
        {
           portScan = GetPressedButtonsMask();

            if((mSwitch_SCAN == 0) || (portScan == 0))
            {
                if(pressedKey != mNoKey_c)
                {
                    mpfKeyFunction(gKBD_EventReleasePB1_c + pressedKey);
                    mKeyState = mStateKeyDebounce;
                    KBD_TimerStop();
                }
            }
        }
        break;
    case mStateKeyHoldGen:
        if( KBD_KeyCheck(mSwitch_SCAN) )
        {
            mKbdHoldGenKeyCount++;
            if(mKbdHoldGenKeyCount >= gKbdHoldDetectIterations_c)
            {
                mKbdHoldGenKeyCount = 0;
                if(pressedKey != mNoKey_c)
                {
                    mpfKeyFunction(gKBD_EventHoldPB1_c + pressedKey);
                }
            }
        }
        else
        {
           portScan = GetPressedButtonsMask();

            if((mSwitch_SCAN == 0) || (portScan == 0))
            {
                if(pressedKey != mNoKey_c)
                {
                    mpfKeyFunction(gKBD_EventReleasePB1_c + pressedKey);

                    mKeyState = mStateKeyDebounce;
                    KBD_TimerStop();
                }
            }
        }
        break;
    default:
        break;
    }
}
#endif /* gKeyEventNotificationMode_d */
#endif /* gTMR_Enabled_d */

static bool_t Switch_CheckIRQ(uint32_t idx)
{
    bool_t status = FALSE;
    const gpioInputPinConfig_t * switchPin;
    switchPin = (const gpioInputPinConfig_t *)kbdSwButtons[idx].config_struct.pSwGpio;
    do {
        if(GpioIsPinIntPending(switchPin))
        {
            /* set the local variable to mark that the interrupt is caused by one of the keyboard switches */
            GpioClearPinIntFlag(switchPin);
            status = TRUE;
            break;
        }
#if defined (CPU_JN518X)
        if (GpioIsPinCauseOfWakeup(switchPin))
        {
            status = TRUE;
            break;
        }
#endif
    } while (0);
    KBD_DBG_LOG("idx=%x, status=%d", idx, status);

    return status;
}
#endif /* gKeyBoardSupported_d */

#if gKeyBoardSupported_d

static void ProgramKeyScan(uint32_t wake_bitfield)
{
    KBD_DBG_LOG("wake_bitfield=%x", wake_bitfield);

#if (gKeyEventNotificationMode_d)
   #if gTMR_Enabled_d
    KBD_TimerStart();
    if (wake_bitfield)
    {
       KeyScan(mKeyScanTimerID);
    }
   #else
   #error gKeyEventNotificationMode_d needs gTMR_Enabled_d to be set
   #endif
#else
        KeyScan(mKeyScanTimerID);
#endif
}
#endif    /* #if gKeyBoardSupported_d */

/******************************************************************************
*******************************************************************************
* Public functions
*******************************************************************************/

#if gKeyBoardSupported_d

/******************************************************************************
 * Name: KBD_Init
 * Description: Initializes the keyboard module internal variables
 * Parameter(s): [IN] pfCallBackAdr - pointer to application callback function
 * Return: -
 * Notes: It the TIMER platform component is enabled, TMR_Init() function MUST
 *        be called before KBD_Init() function
 ******************************************************************************/
void KBD_Init( KBDFunction_t pfCallBackAdr )
{
    KBD_DBG_LOG("");
    uint32_t i = 0, k = 0;
    uint8_t kbdPins = gKBD_ButtonCount_c;
    uint8_t tsiPins = gKBD_TsiElectdCount_c;
#if gKBD_TsiElectdCount_c
    uint32_t keyMap = gKBD_SwTypeMapping_d;
    uint8_t tsiElectrodes[] = gKBD_TsiElecList_d;
    uint8_t j = 0;
    uint8_t bit = 0;
#endif

    /* if no valid pointer provided, return */
    if(NULL != pfCallBackAdr)
    {
        /* store the pointer to callback function provided by the application */
        mpfKeyFunction = pfCallBackAdr;

#if gTMR_Enabled_d
        /* timer is used to determine short or long key press */
        mKeyScanTimerID = TMR_AllocateTimer();
#if gKBD_TsiElectdCount_c
        mTsiSwTriggerTimerID = TMR_AllocateTimer();
#endif
#endif /* #if gTMR_Enabled_d */

        /*  */
        while( kbdPins || tsiPins )
        {
#if gKBD_TsiElectdCount_c
            if( keyMap & BIT(bit) )
            {
                if( tsiPins )
                {
                    kbdSwButtons[i].swType = gKBDTypeTsi_c;
                    kbdSwButtons[i].config_struct.swTsiElectrodeId = tsiElectrodes[j++];
                    tsiPins--;
                    i++;
                }
            }
            else
            {
                if(kbdPins)
                {
#endif
                    kbdSwButtons[i].swType = gKBDTypeGpio_c;
                    kbdSwButtons[i].config_struct.pSwGpio = &switchPins[k++];
                    kbdPins--;
                    i++;
#if gKBD_TsiElectdCount_c
                }
            }
            bit++;
#endif
        }

        /* initialize all the GPIO pins for keyboard */
        KbGpioInit();
#if gKBD_TsiElectdCount_c
        KbTsiInit();
#if gTMR_Enabled_d
        TMR_StartLowPowerTimer(mTsiSwTriggerTimerID,
                gTmrIntervalTimer_c,
                gKBD_TsiTriggerTime_c,
                TsiSwTriggerCallback,
                NULL);
#endif
#endif

        for( i=0; i<gKBD_KeysCount_c; i++ )
        {
            if(kbdSwButtons[i].swType == gKBDTypeGpio_c)
            {
                GpioInstallIsr(Switch_Press_ISR,
                               gGpioIsrPrioLow_c,
                               gKeyboard_IsrPrio_c,
                               kbdSwButtons[i].config_struct.pSwGpio);
            }
        }
    }
    mKbdServiceOpened = true;
}



void KBD_ShutOff(void)
{
    if (mKbdServiceOpened)
    {
        KBD_Deinit();
        mKbdServiceOpened = false;
    }
}

#endif /* gKeyBoardSupported_d */

/******************************************************************************
 * Name: KBD_IsWakeUpSource
 * Description:
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
#if gKeyBoardSupported_d
#if !defined (CPU_JN518X)
bool_t KBD_IsWakeUpSource(void)
{
    uint32_t i;
    bool_t status = FALSE;

    for (i=0; i<(gKBD_KeysCount_c - gKBD_TsiElectdCount_c); i++)
    {
        if ( GpioIsPinIntPending(switchPins + i) )
        {
            status = TRUE;
            break;
        }
    }
    return status;
}
#endif /* !defined (CPU_JN518X)  */
#endif /* gKeyBoardSupported_d */
/******************************************************************************
 * Name: KBD_PrepareEnterLowPower
 * Description:
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
#if gKeyBoardSupported_d
void KBD_PrepareEnterLowPower(void)
{
#if !defined(CPU_JN518X)
#if gKBD_TsiElectdCount_c
#if gTMR_Enabled_d
    TMR_StopTimer(mTsiSwTriggerTimerID);
#endif
    TSI_DisableInterrupts(TSI0, kTSI_GlobalInterruptEnable);
    TSI_DisableInterrupts(TSI0, kTSI_EndOfScanInterruptEnable);
    TSI_ClearStatusFlags(TSI0, kTSI_EndOfScanFlag);

    NVIC_DisableIRQ(TSI0_IRQn); /* disable interrupt from NVIC */
    NVIC_ClearPendingIRQ(TSI0_IRQn); /* clear interrupt flag from NVIC */
    TSI_EnableModule(TSI0, false);
  #endif /* gKBD_TsiElectdCount_c */
#else
    /* For the CPU_JN518X family stop the KeyScan timer */
    KBD_TimerStop();
    /* schedule TMR_Task to effectively stop running timer */
    TMR_MakeTMRThreadReady();
#endif
}
/******************************************************************************
 * Name: KBD_PrepareExitLowPower
 * Description:
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
void KBD_PrepareExitLowPower(void)
{
#if gKBD_TsiElectdCount_c
#if gTMR_Enabled_d
    TMR_StartLowPowerTimer(mTsiSwTriggerTimerID,
            gTmrIntervalTimer_c,
            gKBD_TsiTriggerTime_c,
            TsiSwTriggerCallback,
            NULL);
#endif
    TSI_EnableModule(TSI0, true);
    TSI_ClearStatusFlags(TSI0, kTSI_EndOfScanFlag);
    TSI_EnableInterrupts(TSI0, kTSI_GlobalInterruptEnable);
    TSI_EnableInterrupts(TSI0, kTSI_EndOfScanInterruptEnable);
    NVIC_EnableIRQ(TSI0_IRQn);
#else
#if defined (CPU_JN518X)
    do {
        if (!mKbdServiceOpened) break;
        KbGpioInit();

        mWakePortScan = 0;
        for(int i=0; i<gKBD_KeysCount_c; i++ )
        {
            if(kbdSwButtons[i].swType == gKBDTypeGpio_c)
            {
                if( Switch_CheckIRQ(i) )
                {
                    mWakePortScan |= BIT(i);
                }
                GpioInstallIsr(Switch_Press_ISR,
                               gGpioIsrPrioLow_c,
                               gKeyboard_IsrPrio_c,
                               kbdSwButtons[i].config_struct.pSwGpio);
            }
        }
        if (!mWakePortScan) break;

        for (int i = 0; i < gKBD_KeysCount_c; i++)
        {
            if (mWakePortScan & BIT(i))
            {
                KBD_DBG_LOG("Detect Key %d", i);
            }
        }
        ProgramKeyScan(mWakePortScan);
    } while (0);
  #endif /* defined (CPU_JN518X)*/
#endif /* gKBD_TsiElectdCount_c */
}
#endif /* gKeyBoardSupported_d */
/******************************************************************************
 * Name: KBD_SwitchPressOnWakeUp
 * Description:
 * Parameter(s): -
 * Return: -
 ******************************************************************************/
#if gKeyBoardSupported_d
#if !defined(CPU_JN518X)
void KBD_SwitchPressedOnWakeUp(void)
{
#if gTMR_Enabled_d
    KBD_TimerStart();
#endif
}
#endif
#endif /* gKeyBoardSupported_d */

/******************************************************************************
 * Name: Switch_Press_ISR
 * Description: Keyboard (switches) interrupt handler
 * Parameter(s): [IN] mask - mask corresponding to pin
 * Return: -
 ******************************************************************************/
#if gKeyBoardSupported_d
#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".isr_handler"
#endif
void Switch_Press_ISR(void)
{
    uint32_t i = 0;
    uint8_t kbi_irq = 0;

    for(i=0; i<gKBD_KeysCount_c; i++)
    {
        if(kbdSwButtons[i].swType == gKBDTypeGpio_c)
        {
            if( Switch_CheckIRQ(i) )
            {
                kbi_irq++;
            }
        }
    }

    if(kbi_irq)
    {
        ProgramKeyScan(0);
    }
}
#endif /* gKeyBoardSupported_d */

#if gKeyBoardSupported_d
#if gKBD_TsiElectdCount_c
/* timer callback for tsi software trigger */
static void TsiSwTriggerCallback(void* params)
{
    static uint8_t mActiveTsiElectrode = 0;
    if(kbdNumberOfElectrodes == 0)
    {
        TMR_StopTimer(mTsiSwTriggerTimerID); /*no installed electrodes. stop timer*/
    }
    if(!TSI_IsScanInProgress(TSI0)) /*if scan is not in progress start new*/
    {
        mActiveTsiElectrode = (mActiveTsiElectrode + 1) % kbdNumberOfElectrodes;

        TSI_SetMeasuredChannelNumber(TSI0, kbdTsiElectrodes[mActiveTsiElectrode].elecName);
        TSI_StartSoftwareTrigger(TSI0);
    }
}
/* Interrupt handler for TSI */
void KBD_TSI_IrqHandler(void)
{
    uint8_t i = 0;
    for(i = 0; i < kbdNumberOfElectrodes; i++)
    {
        if (TSI_GetMeasuredChannelNumber(TSI0) == kbdTsiElectrodes[i].elecName)
        {
            if (TSI_GetCounter(TSI0) >
                (uint16_t)(kbdTsiElectrodes[i].elecBaseValue) + TSI_ELECTRODE_THRESH)
            {
                kbdTsiElectrodes[i].elecValueAboveThresh = TRUE;
#if gTMR_Enabled_d
                if(!TMR_IsTimerActive(mKeyScanTimerID))
                {
                    KBD_TimerStart();
                }
#endif
            }
            else
            {
                kbdTsiElectrodes[i].elecValueAboveThresh = FALSE;
            }
            break;
        }
    }

    /* Clear flags */
    TSI_ClearStatusFlags(TSI0, kTSI_EndOfScanFlag);
    TSI_ClearStatusFlags(TSI0, kTSI_OutOfRangeFlag);
}
#endif /* #if gKBD_TsiElectdCount_c */
#endif /* #if gKeyBoardSupported_d */
