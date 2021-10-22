/*! *********************************************************************************
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2017 NXP
* All rights reserved.
*
* \file
*
* LED implementation file for ARM CORTEX-M4 processor
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#include "LED.h"
#include "TimersManager.h"
#include "GPIO_Adapter.h"
#include "gpio_pins.h"
#include "pin_mux.h"
#include "fsl_os_abstraction.h"

#ifndef BOARD_USE_PWM_FOR_RGB_LED
#define BOARD_USE_PWM_FOR_RGB_LED 0
#endif

#if BOARD_USE_PWM_FOR_RGB_LED
#include "TMR_Adapter.h"
#endif


#if (gLEDSupported_d)

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/
#if gLedRgbEnabled_d
#define PWM_MAX_DUTY_CYCLE_c            LED_MAX_RGB_VALUE_c    
#define PWM_INIT_VAL_c                  0x00     /* init value */
#define PWM_FREQ                        190000u  /* 187500u */
#endif

#if BOARD_USE_PWM_FOR_RGB_LED && (!gLpmIncluded_d)

/* RGB LED - 3 PWM signals for controlling color */
/* RGB LED SET value */
#define RGB_SET_LED_RED(x)              PWM_SetChnCountVal(gRedLedPwmTimerInstance_c, gRedLedPwmTimerChannel_c, (x))
#define RGB_SET_LED_GREEN(x)            PWM_SetChnCountVal(gGreenLedPwmTimerInstance_c, gGreenLedPwmTimerChannel_c, (x))
#define RGB_SET_LED_BLUE(x)             PWM_SetChnCountVal(gBlueLedPwmTimerInstance_c, gBlueLedPwmTimerChannel_c, (x))

#endif



#if ((!BOARD_USE_PWM_FOR_RGB_LED) || (BOARD_USE_PWM_FOR_RGB_LED && gLpmIncluded_d))
#define gRGB_Threshold_c               60
#define RGB_SET_LED_RED(x)              ((x) > gRGB_Threshold_c)?(LED_GpioSet((const gpioOutputPinConfig_t*)&ledPins[gRedLedIdx_c], gLedOn_c)):(LED_GpioSet((const gpioOutputPinConfig_t*)&ledPins[gRedLedIdx_c], gLedOff_c))
#define RGB_SET_LED_GREEN(x)            ((x) > gRGB_Threshold_c)?(LED_GpioSet((const gpioOutputPinConfig_t*)&ledPins[gGreenLedIdx_c], gLedOn_c)):(LED_GpioSet((const gpioOutputPinConfig_t*)&ledPins[gGreenLedIdx_c], gLedOff_c))
#define RGB_SET_LED_BLUE(x)             ((x) > gRGB_Threshold_c)?(LED_GpioSet((const gpioOutputPinConfig_t*)&ledPins[gBlueLedIdx_c], gLedOn_c)):(LED_GpioSet((const gpioOutputPinConfig_t*)&ledPins[gBlueLedIdx_c], gLedOff_c))
#endif


#define gLedId_LastActiveId_c           (0x01 << (gLEDsOnTargetBoardCnt_c-1))
/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/*
* Name: LED_DimMode_t
* Description: RGB LED dimming modes
*/
typedef enum LED_DimMode_tag {
  gLedDimIn_c,
  gLedDimOut_c,
  gLedDimMax_c
} LED_DimMode_t;

/*
* Name: LED_RgbDimInfo_t
* Description: RGB LED dimming information structure
*/
typedef struct LED_RgbDimInfo_tag
{
  LED_DimMode_t mode;     /* dim in/out */
  uint8_t interval;       /* in seconds */
  uint16_t steps;         /* dimming steps */
  uint16_t value[3];      /* current values for RGB LED */
  uint16_t increment[3];  /* increment/decrement values for RGB LED */
  bool_t ongoing;         /* TRUE if RGB LED dimming is ongoing */
} LED_RgbDimInfo_t;

/******************************************************************************
*******************************************************************************
* Public memory definitions
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private memory definitions
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private function prototypes
*******************************************************************************
******************************************************************************/
static void LED_GpioSet(const gpioOutputPinConfig_t* pOutputConfig, LED_OpMode_t operation);
#if gTMR_Enabled_d
static void LED_FlashTimeout(void *timerId);
#endif
#if gLedRgbEnabled_d
static void LED_RgbLedInit(void);
static void LED_ToggleRgbLed(LED_t LEDNr);
#endif

#if gLedColorWheelEnabled_d && gLedRgbEnabled_d
static void LED_ColorWheelSet(uint8_t index);
#else
    #if gLEDBlipEnabled_d
    static void LED_DecrementBlip(void);
    #endif
#endif
    
#if gTMR_Enabled_d && gLedRgbEnabled_d && gRgbLedDimmingEnabled_d
static void LED_DimmingTimeout(tmrTimerID_t tmrId);
static bool_t LED_StartRgbDimming(LED_DimMode_t mode, uint8_t interval);
#endif

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/
typedef struct ledColor_tag
{
  uint8_t redValue;
  uint8_t greenValue;
  uint8_t blueValue;
}ledColor_t;
/******************************************************************************
*******************************************************************************
* Private Memory Declarations
*******************************************************************************
******************************************************************************/
/* Serial Mode: indicates if the LED module is in serial mode or not */
static bool_t mfLedInSerialMode;
/* Flashing Mode: indicates how many LEDs are in flashing mode */
static uint8_t mLedFlashingLEDs = 0;  /* flashing LEDs */
static uint8_t mLedStartFlashingPosition = LED1;
static bool_t isInitialized = FALSE;
#if gTMR_Enabled_d
/* LED timer ID */
static tmrTimerID_t mLEDTimerID = gTmrInvalidTimerID_c;
#endif
#if gLedRgbEnabled_d
/* last positive RGB values */
static uint8_t mLedRgbLastValues[3] = {0, 0, 0};
#if gTMR_Enabled_d && gRgbLedDimmingEnabled_d
static tmrTimerID_t mRGBLedTimerID = gTmrInvalidTimerID_c;
#endif /* gTMR_Enabled_d */
#endif
#if (gLEDBlipEnabled_d) && (!gLedColorWheelEnabled_d || !gLedRgbEnabled_d)
/* array that stores the LEDs blip information*/
static uint8_t mLedBlips[gLEDsOnTargetBoardCnt_c];
#endif

#if gLedColorWheelEnabled_d && gLedRgbEnabled_d
uint8_t gColorWheelIdx = 0x00;
#if BOARD_USE_PWM_FOR_RGB_LED && (!gLpmIncluded_d)
const ledColor_t gColorWheel[] = {   {255, 0, 0}, /* red */ {255, 128, 0}, /* orange */ {255, 255, 0}, /* yellow */
                                    {128, 255, 0}, /* yellow green */ {0, 255, 0}, /* green */ { 0, 255, 255}, /* blue green */
                                    {0, 0, 255}, /* blue */ { 102, 102, 255}, /* blue violet */{255, 0, 255}, /* violet */
                                    {102, 0, 204}, /* mauves */{ 102, 0, 102}, /* mauves pink */{255, 51, 153}, /* pink */
                                };
#else
const ledColor_t gColorWheel[] = { {255, 0, 0}, /* red */   {255, 255, 0}, /* yellow */
                                  {0, 255, 0}, /* green */ { 0, 255, 255}, /* blue green */
                                  {0, 0, 255}, /* blue */  {255, 0, 255}, /* violet */
                                  {255, 255, 255}, /* white */
                                };
#endif
#endif

#if gTMR_Enabled_d && gLedRgbEnabled_d && gRgbLedDimmingEnabled_d
/*
 * Name: mRbgDimInfo
 * Description: RGB dimming info structure. Contains dimming related information.
 */
static LED_RgbDimInfo_t mRbgDimInfo;
#endif

/******************************************************************************
******************************************************************************
* Public functions
******************************************************************************
*****************************************************************************/

/******************************************************************************
* Name: LED_Init
* Description: Initialize the LED module
* Parameters: -
* Return: -
******************************************************************************/
void LED_Init
(
    void
)
{
	if (!isInitialized)
	{
		GpioOutputPinInit(ledPins, gLEDsOnTargetBoardCnt_c);

#if gLedRgbEnabled_d
		LED_RgbLedInit();
#endif

		/* allocate a timer for use in flashing LEDs */
#if gTMR_Enabled_d
		mLEDTimerID = TMR_AllocateTimer();
#endif

#if gLedRgbEnabled_d && gRgbLedDimmingEnabled_d && gTMR_Enabled_d
		/* allocate a timer for use in RGB dimming */
		mRGBLedTimerID = TMR_AllocateTimer();
		mRbgDimInfo.ongoing = FALSE;
		mRbgDimInfo.interval = gRgbLedDimDefaultInterval_c;
#endif /* gLedRgbEnabled_d && gRgbLedDimmingEnabled_d && gTMR_Enabled_d */
		isInitialized = TRUE;
	}
}

/******************************************************************************
* Name: LED_PrepareExitLowPower
* Description: Initialize the LED module
* Parameters: -
* Return: -
******************************************************************************/
void LED_PrepareExitLowPower(void)
{
	GpioOutputPinInit(ledPins, gLEDsOnTargetBoardCnt_c);
}
/******************************************************************************
* Name: Un-initialize the LED module
* Description: turns off all the LEDs and disables clock gating for LED port
* Parameters: -
* Return: -
******************************************************************************/
void LED_UnInit
(
void
)
{
    uint32_t i;

#if gTMR_Enabled_d
    /* free the timer used for flashing mode */
    TMR_FreeTimer(mLEDTimerID);
#endif
    
#if gTMR_Enabled_d && gLedRgbEnabled_d && gRgbLedDimmingEnabled_d    
    TMR_FreeTimer(mRGBLedTimerID);
#endif
    
    for( i=0; i<gLEDsOnTargetBoardCnt_c; i++ )
    {
        /* turn off LED */
        GpioSetPinOutput(ledPins + i);
    }
    isInitialized = FALSE;
}

/******************************************************************************
* Name: LED_Operate
* Description: Basic LED operation: ON, OFF, TOGGLE.
* Param(s): -
* Return: -
******************************************************************************/
void LED_Operate
(
    LED_t led,
    LED_OpMode_t operation
)
{

#if gLedRgbEnabled_d
    if((led & LED_RGB) == LED_RGB)
    {
        switch(operation)
        {
            case gLedOn_c:
                LED_SetRgbLed(LED_RGB, PWM_MAX_DUTY_CYCLE_c, PWM_MAX_DUTY_CYCLE_c, PWM_MAX_DUTY_CYCLE_c);
                break;
            case gLedOff_c:
                LED_SetRgbLed(LED_RGB, 0, 0, 0);
                break;
            case gLedToggle_c:
                LED_ToggleRgbLed(LED_RGB);
                break;
            default:
                break;
        }
        
        led &= ~(LED_RGB);
    }
#endif /* gLedRgbEnabled_d */

#if gLEDsOnTargetBoardCnt_c > 0
    if(led & LED1)
    {
      LED_GpioSet(ledPins, operation);
      
    }
#endif
#if gLEDsOnTargetBoardCnt_c > 1
    if(led & LED2)
    {
      LED_GpioSet(ledPins + 1, operation);
    }
#endif
#if gLEDsOnTargetBoardCnt_c > 2
    if(led & LED3)
    {
        LED_GpioSet(ledPins + 2, operation);
    }
#endif
#if gLEDsOnTargetBoardCnt_c > 3
    if(led & LED4)
    {
        LED_GpioSet(ledPins + 3, operation);
    }
#endif
}

/******************************************************************************
* Name: LED_TurnOnLed
* Description: Turns ON the specified LED(s)
* Parameters: LEDNr - LED number(s) to be turned ON
* Return:
******************************************************************************/
void LED_TurnOnLed
(
    LED_t LEDNr
)
{
    LED_Operate(LEDNr, gLedOn_c);  /* turn ON LEDs */
}

/******************************************************************************
* Name: LED_TurnOffLed
* Description: Turns OFF the specified LED(s)
* Parameters: LEDNr - LED number(s) to be turned ON
* Return:
******************************************************************************/
void LED_TurnOffLed
(
    LED_t LEDNr
)
{
    LED_Operate(LEDNr, gLedOff_c);  /* turn OFF LEDs */
}

/******************************************************************************
* Name: LED_ToggleLed
* Description: Toggles the specified LED(s)
* Parameters:  LEDNr - LED number(s) to be toggled
* Return: -
******************************************************************************/
void LED_ToggleLed
(
    LED_t LEDNr
)
{
    LED_Operate(LEDNr, gLedToggle_c);  /* toggle LEDs */
}

/******************************************************************************
* Name: LED_TurnOffAllLeds
* Description: Turns OFF all LEDs
* Parameters: -
* Return: -
******************************************************************************/
void LED_TurnOffAllLeds
(
void
)
{
    LED_TurnOffLed(LED_ALL);
}

/******************************************************************************
* Name: LED_TurnOnAllLeds
* Description: Turns ON all LEDs
* Parameters: -
* Return: -
******************************************************************************/
void LED_TurnOnAllLeds
(
void
)
{
    LED_TurnOnLed(LED_ALL);
}

/******************************************************************************
* Name: LED_StopFlashingAllLeds
* Description: Stops flashing and turns OFF all LEDs
* Parameters: -
* Return: -
******************************************************************************/
void LED_StopFlashingAllLeds(void)
{
    LED_SetLed(LED_ALL, gLedOff_c);
}

#if (gLEDBlipEnabled_d) && (!gLedColorWheelEnabled_d || !gLedRgbEnabled_d)
/******************************************************************************
* Name: LED_StartBlip
* Description: Set up for blinking one or more LEDs once
* Parameters: [IN] LED_t LEDNr - LED Number (may be an OR of the list)
* Return: -
******************************************************************************/
void LED_StartBlip
(
    LED_t LEDNr
)
{
    uint8_t iLedIndex;

    /* set up for blinking one or more LEDs once */
    for(iLedIndex = 0; iLedIndex < gLEDsOnTargetBoardCnt_c; ++iLedIndex) 
    {
        if(LEDNr & (1u << iLedIndex))
        {
            mLedBlips[iLedIndex] = 2;   /* blink on, then back off */
        }
    }

    /* start flashing */
    LED_StartFlash(LEDNr);
}
#endif

/******************************************************************************
* Name: LED_StopFlash
* Description: Stop an LED from flashing.
* Parameters: [IN] LED_t LEDNr - LED Number (may be an OR of the list)
* Return: -
******************************************************************************/
void LED_StopFlash
(
    LED_t LEDNr
)
{
    /* leave stopped LEDs in the off state */
    LED_TurnOffLed(LEDNr);

    /* stop flashing on one or more LEDs */
    mLedFlashingLEDs &= (uint8_t)(~LEDNr);

#if gTMR_Enabled_d
    /* if ALL LEDs have stopped flashing, then stop timer */
    if(!mLedFlashingLEDs)
    {
        TMR_StopTimer(mLEDTimerID);
    }
#endif
}

/******************************************************************************
* Name: LED_SetHex
* Description: Sets a specified hex value on the LEDs
* Parameters: [IN] hexValue - the value to be set on LEDs
* Return: -
******************************************************************************/
void LED_SetHex
(
uint8_t hexValue
)
{
    LED_SetLed(LED_ALL, gLedOff_c);
    LED_SetLed(hexValue, gLedOn_c);
}
/******************************************************************************
* Name: LED_SetLed
* Description: This function can set the specified LED(s) in one of the
*              following states: On, Off, Toggle, Flashing or StopFlash
* Parameters: [IN] LEDNr - LED(s) to
*             [IN] LedState_t state - one of the possible states listed above
* Return:
******************************************************************************/
void LED_SetLed
(
    LED_t LEDNr,
    LedState_t state
)
{
    /* turning off flashing same as off state */
    if(state == gLedStopFlashing_c)
    {
        state = gLedOff_c;
    }

    #if gLedColorWheelEnabled_d && gLedRgbEnabled_d
    if(LED_RGB == LEDNr)
    {
        gColorWheelIdx = 0x00;
    }
    #endif   
    
    /* turn off serial lights if in serial mode */
    LED_StopSerialFlash();

    /* flash LED */
    if(state == gLedFlashing_c)
    {
        LED_StartFlash(LEDNr);
    }
#if (gLEDBlipEnabled_d) && (!gLedColorWheelEnabled_d || !gLedRgbEnabled_d)
    /* blink LEDs */
    else if(state == gLedBlip_c)
    {
        LED_StartBlip(LEDNr);
    }
#endif
    else
    {
        /* on, off or toggle will stop flashing on the LED. Also exits serial mode */
        if((mfLedInSerialMode) || (mLedFlashingLEDs & LEDNr))
        {
            LED_StopFlash(LEDNr);
        }
        /* Select the operation to be done on the port */
        if(state == gLedOn_c)
        {
             LED_TurnOnLed(LEDNr);
        }
        if(state == gLedOff_c)
        {
             LED_TurnOffLed(LEDNr);
        }
        if(state == gLedToggle_c)
        {
             LED_ToggleLed(LEDNr);
        }
    }
}

/******************************************************************************
* Name: LED_SetRgbLed
* Description: This function is used to control the RGB LED
* Param(s): [in]    LEDNr - led identifier
            [in]    redValue - red value  from 0 to PWM_MODULE_MAX_DUTY_CYCLE_c
            [in]    greenValue - green value from 0 to PWM_MODULE_MAX_DUTY_CYCLE_c
            [in]    blueValue - blue value from 0 to PWM_MODULE_MAX_DUTY_CYCLE_c
* Return: - None
******************************************************************************/
void LED_SetRgbLed
(
    LED_t LEDNr,
    uint16_t redValue,
    uint16_t greenValue,
    uint16_t blueValue
)
{
#if gLedRgbEnabled_d
   if(LEDNr == LED_RGB)
   {
        /* max value = PWM_MAX_DUTY_CYCLE_c */
        if((redValue > PWM_MAX_DUTY_CYCLE_c) ||
           (greenValue > PWM_MAX_DUTY_CYCLE_c) ||
           (blueValue > PWM_MAX_DUTY_CYCLE_c))
        {
            return;
        }
        
        /* keep last values */
        if(redValue || greenValue || blueValue)
        {
            mLedRgbLastValues[0] = redValue;
            mLedRgbLastValues[1] = greenValue;
            mLedRgbLastValues[2] = blueValue;
        }

#if gLED_InvertedMode_d && BOARD_USE_PWM_FOR_RGB_LED
        redValue   = PWM_MAX_DUTY_CYCLE_c - redValue;
        greenValue = PWM_MAX_DUTY_CYCLE_c - greenValue;
        blueValue  = PWM_MAX_DUTY_CYCLE_c - blueValue;
#endif

        /* set color */
        RGB_SET_LED_RED(redValue);
        RGB_SET_LED_GREEN(greenValue);
        RGB_SET_LED_BLUE(blueValue);
   }
#else
   LEDNr=LEDNr;
   redValue=redValue;
   greenValue=greenValue;
   blueValue=blueValue;
#endif
}

/******************************************************************************
* Name: LED_RgbDimOut
* Description: Dim out the RGB LED
* Param(s): None    
* Return: TRUE if no dimining is ongoing and the command has been accepted,
*         FALSE otherwise
******************************************************************************/
bool_t LED_RgbDimOut(void)
{
#if gTMR_Enabled_d && gLedRgbEnabled_d && gRgbLedDimmingEnabled_d  
  return LED_StartRgbDimming(gLedDimOut_c, mRbgDimInfo.interval);
#else
  return FALSE;
#endif
}

/******************************************************************************
* Name: LED_RgbDimIn
* Description: Dim in the RGB LED
* Param(s): None    
* Return: TRUE if no dimining is ongoing and the command has been accepted,
*         FALSE otherwise
******************************************************************************/
bool_t LED_RgbDimIn(void)
{
#if gTMR_Enabled_d && gLedRgbEnabled_d && gRgbLedDimmingEnabled_d
  return LED_StartRgbDimming(gLedDimIn_c, mRbgDimInfo.interval);
#else
  return FALSE;  
#endif
}

/******************************************************************************
* Name: LED_RgbSetDimInterval
* Description: Dim (fade) in the RGB LED
* Param(s): dim_interval - the dimming interval
* Return: None
******************************************************************************/
void LED_RgbSetDimInterval(uint8_t dim_interval)
{
#if gTMR_Enabled_d && gLedRgbEnabled_d && gRgbLedDimmingEnabled_d  
  if(dim_interval > gRgbLedDimMaxInterval_c)
  {
    return;
  }  
  mRbgDimInfo.interval = dim_interval;
#else
  (void)dim_interval;
#endif  
}

 /******************************************************************************
* Name: LED_StartFlashWithPeriod
* Description: Starts flashing one or more LEDs
* Parameters: [IN] LED_t LEDNr          - LED Number (may be an OR of the list)
              [IN] uint16_t periodMs    - LED flashing period in milliseconds
* Return: -
******************************************************************************/
void LED_StartFlashWithPeriod
(
    LED_t LEDNr,
    uint16_t periodMs
)
{
    /* indicate which LEDs are flashing */
    mLedFlashingLEDs |= LEDNr;

#if gTMR_Enabled_d
    /* start the timer */
    if(!TMR_IsTimerActive(mLEDTimerID))
    {
        TMR_StartIntervalTimer(mLEDTimerID, periodMs, (pfTmrCallBack_t)LED_FlashTimeout, (void*)((uint32_t)mLEDTimerID));
    }
#else
    #warning "The TIMER component is not enabled and therefore the LED flashing function is disabled"
#endif
}
/******************************************************************************
* Name: LED_StartSerialFlashWithPeriod
* Description: starts serial flashing LEDs
* Parameters: [IN] LEDStartPosition - LED start position
*             [IN] periodMs - flashing period in milliseconds
* Return: -
******************************************************************************/
void LED_StartSerialFlashWithPeriod
(
    uint8_t ledStartPosition,
    uint16_t periodMs
)
{
    /* indicate going into flashing mode (must be done first) */
    LED_StartFlashWithPeriod(ledStartPosition, periodMs);

#if gLedRgbEnabled_d
    if(ledStartPosition > gLedId_LastActiveId_c)
    {
        ledStartPosition = LED_RGB;
    }
#else
    if(ledStartPosition > gLedId_LastActiveId_c)
    {
        ledStartPosition = LED1;
    }
#endif
    mLedStartFlashingPosition   = ledStartPosition;

    /* set up for serial lights */
    LED_TurnOffAllLeds();
    LED_TurnOnLed(mLedStartFlashingPosition);
    mLedFlashingLEDs = mLedStartFlashingPosition | (uint16_t)((uint16_t)mLedStartFlashingPosition << 1); /* toggle these to on first flash */

    /* indicate in serial flash mode */
    mfLedInSerialMode = TRUE;
}
/******************************************************************************
* Name: LED_StopSerialFlash
* Description: Stop serial flashing mode, if serial flash mode is active.
*              Turns all LEDs off.
* Param(s): -
* Return: -
******************************************************************************/
void LED_StopSerialFlash
(
    void
)
{
    if(mfLedInSerialMode)
    {
        /* stop flashing timer */
        (void)TMR_StopTimer(mLEDTimerID);

        /* stop flashing on one or more LEDs */
        mLedFlashingLEDs = 0;
        mfLedInSerialMode = FALSE;

        /* leave stopped LEDs in the off state */
        LED_TurnOffAllLeds();

        mLedStartFlashingPosition = LED1;
    }
}
/******************************************************************************
* Name: LED_StartColorWheel
* Description: starts color wheel
* Parameters: [IN] LEDNr - LED Number (may be an OR of the list)
*             [IN] periodMs - period to the next color in milliseconds
* Return: -
******************************************************************************/
void LED_StartColorWheel
(
    LED_t LEDNr,
    uint16_t periodMs
)
{
#if gLedColorWheelEnabled_d && gLedRgbEnabled_d
    LED_ColorWheelSet(LEDNr);
    LED_StartFlashWithPeriod(LEDNr, periodMs);
#else
    LEDNr=LEDNr;
    periodMs=periodMs;
#endif

}
/******************************************************************************
*******************************************************************************
* Private functions
*******************************************************************************
******************************************************************************/


/******************************************************************************
* Name: LED_GpioSet
* Description: GPIO LED operation: ON, OFF, TOGGLE.
* Param(s): -
* Return: -
******************************************************************************/
static void LED_GpioSet
(
    const gpioOutputPinConfig_t* pOutputConfig,
    LED_OpMode_t operation
)
{
    switch(operation)
    {
        case gLedOn_c:
#if gLED_InvertedMode_d
            GpioSetPinOutput(pOutputConfig);
#else
            GpioClearPinOutput(pOutputConfig);
#endif
            break;
        case gLedOff_c:
#if gLED_InvertedMode_d
            GpioClearPinOutput(pOutputConfig);
#else
            GpioSetPinOutput(pOutputConfig);
#endif
            break;
        case gLedToggle_c:
            GpioTogglePinOutput(pOutputConfig);
            break;
        default:
            break;
    }
}


#if gLedRgbEnabled_d
/******************************************************************************
* Name: LED_ToggleRgbLed
* Description: This function is used to toggle the RGB LED
* Param(s): [in]    LEDNr - led identifier
* Return: - None
******************************************************************************/
static void LED_ToggleRgbLed
(
   LED_t LEDNr
)
{
   if(LEDNr == LED_RGB)
   {
     uint8_t redValue = 0, greenValue = 0, blueValue = 0;

#if BOARD_USE_PWM_FOR_RGB_LED && (!gLpmIncluded_d)
     redValue   = PWM_GetChnCountVal(gRedLedPwmTimerInstance_c, gRedLedPwmTimerChannel_c);
     greenValue = PWM_GetChnCountVal(gGreenLedPwmTimerInstance_c, gGreenLedPwmTimerChannel_c);
     blueValue  = PWM_GetChnCountVal(gBlueLedPwmTimerInstance_c, gBlueLedPwmTimerChannel_c);
#else
     redValue   = !GpioReadOutputPin((const gpioOutputPinConfig_t*)&ledPins[gRedLedIdx_c]);
     greenValue = !GpioReadOutputPin((const gpioOutputPinConfig_t*)&ledPins[gGreenLedIdx_c]);
     blueValue  = !GpioReadOutputPin((const gpioOutputPinConfig_t*)&ledPins[gBlueLedIdx_c]);
#endif

#if gLED_InvertedMode_d
#if BOARD_USE_PWM_FOR_RGB_LED
     redValue   = LED_MAX_RGB_VALUE_c - redValue;
     greenValue = LED_MAX_RGB_VALUE_c - greenValue;
     blueValue  = LED_MAX_RGB_VALUE_c - blueValue;
#else
     redValue   = !redValue;
     greenValue = !greenValue;
     blueValue  = !blueValue;
#endif
#endif

     if(redValue || greenValue || blueValue)
     {
#if !BOARD_USE_PWM_FOR_RGB_LED
        redValue   += gRGB_Threshold_c;
        greenValue += gRGB_Threshold_c;
        blueValue  += gRGB_Threshold_c;
#endif
        mLedRgbLastValues[0] = redValue;
        mLedRgbLastValues[1] = greenValue;
        mLedRgbLastValues[2] = blueValue;
        LED_SetRgbLed(LEDNr, 0, 0, 0);
     }
     else
     {
        if((!mLedRgbLastValues[0]) && (!mLedRgbLastValues[1]) && (!mLedRgbLastValues[2]))
        {
            mLedRgbLastValues[0] = mLedRgbLastValues[1]  = mLedRgbLastValues[2] = LED_MAX_RGB_VALUE_c;
        }
        LED_SetRgbLed(LEDNr, mLedRgbLastValues[0], mLedRgbLastValues[1], mLedRgbLastValues[2]);
     }
   }
}
#endif

#if gLedRgbEnabled_d
/******************************************************************************
* Name: LED_RgbLedInit
* Description: Initialize the RGB LED.
* Parameters: -
* Return: -
******************************************************************************/
static void LED_RgbLedInit(void)
{
#if BOARD_USE_PWM_FOR_RGB_LED && (!gLpmIncluded_d)
  
    tmr_adapter_pwm_param_t pwm_config;
    
    /* Ungate PWM module */
    PWM_Init(gRedLedPwmTimerInstance_c);
    PWM_Init(gGreenLedPwmTimerInstance_c);
    PWM_Init(gBlueLedPwmTimerInstance_c);

    /* PWM pin mux on RGB LED */
    BOARD_InitRGB();

    /*Configure PWM Module */
    pwm_config.initValue = PWM_INIT_VAL_c;
    pwm_config.frequency = PWM_FREQ;
    PWM_StartEdgeAlignedLowTrue(gRedLedPwmTimerInstance_c, &pwm_config, gRedLedPwmTimerChannel_c);
    PWM_StartEdgeAlignedLowTrue(gGreenLedPwmTimerInstance_c, &pwm_config, gGreenLedPwmTimerChannel_c);
    PWM_StartEdgeAlignedLowTrue(gBlueLedPwmTimerInstance_c, &pwm_config, gBlueLedPwmTimerChannel_c);
    
#endif
}
#endif

#if (gLEDBlipEnabled_d) && (!gLedColorWheelEnabled_d || !gLedRgbEnabled_d)
/******************************************************************************
* Name: LED_DecrementBlip
* Description: Decrements the blink count
* Param(s): -
* Return: -
******************************************************************************/
static void LED_DecrementBlip(void)
{
    uint8_t iLedIndex;

    for(iLedIndex = 0; iLedIndex < gLEDsOnTargetBoardCnt_c; ++iLedIndex)
    {
        if(mLedBlips[iLedIndex])
        {
            --mLedBlips[iLedIndex];
            if(!mLedBlips[iLedIndex])
            {
                mLedFlashingLEDs &= ~(1u << iLedIndex);
            }
        }
    }

    /* done, stop the timer */
    if(!mLedFlashingLEDs)
    {
        (void)TMR_StopTimer(mLEDTimerID);
    }
}
#endif

#if gTMR_Enabled_d
/******************************************************************************
* Name: LED_FlashTimeout
* Description: timer callback function that is called each time the timer
*              expires
* Param(s): [IN] timerId - the timer ID
* Return: -
******************************************************************************/
static void LED_FlashTimeout
(
    void *timerId
)
{
    (void) timerId;
#if gLedColorWheelEnabled_d && gLedRgbEnabled_d
    if(!gColorWheelIdx)
    {
        LED_ToggleLed(mLedFlashingLEDs);
    }
    else
    {
        LED_ColorWheelSet(mLedFlashingLEDs & LED_RGB);
        LED_ToggleLed(mLedFlashingLEDs & (~LED_RGB));
    }
#else

    LED_ToggleLed(mLedFlashingLEDs);
#if gLEDBlipEnabled_d
   /* decrement blips */
   LED_DecrementBlip();
#endif
#endif

    /* if serial lights, move on to next light */
    if(mfLedInSerialMode)
    {
        mLedFlashingLEDs = mLedFlashingLEDs << 1;
        if(mLedFlashingLEDs & (gLedId_LastActiveId_c << 1))  /* wrap around */
        {
            mLedFlashingLEDs &= LED_ALL;
            mLedFlashingLEDs |= mLedStartFlashingPosition;
        }
    }
}

#if gLedColorWheelEnabled_d && gLedRgbEnabled_d
/******************************************************************************
* Name: LED_ColorWheelSet
* Description: Color wheel callback
* Param(s): -
* Return: -
******************************************************************************/
static void LED_ColorWheelSet( LED_t LEDNr)
{

    if(gColorWheelIdx >= NumberOfElements(gColorWheel))
    {
        gColorWheelIdx = 0;
    }
    LED_SetRgbLed(LEDNr, gColorWheel[gColorWheelIdx].redValue,
                    gColorWheel[gColorWheelIdx].greenValue, gColorWheel[gColorWheelIdx].blueValue);

    gColorWheelIdx++;
}
#endif /* gLedColorWheelEnabled_d && gLedRgbEnabled_d */

#if gLedRgbEnabled_d && gRgbLedDimmingEnabled_d
/******************************************************************************
* Name: LED_DimmingTimeout
* Description: Callback function of the interval timer used by the RGB LED 
*              dimmer. It increases/decreases the RGB values accordingly
*              to dimming information stored in mRbgDimInfo.
* Param(s): [in]    tmrId - timer ID
* Return: - None
******************************************************************************/
static void LED_DimmingTimeout(tmrTimerID_t tmrId)
{
  uint16_t redValue;
  uint16_t greenValue;
  uint16_t blueValue;
  
  if(tmrId != mRGBLedTimerID)
  {    
    return;
  }   
  
  if(gLedDimOut_c == mRbgDimInfo.mode) 
  {     
    if(mRbgDimInfo.value[0] >= mRbgDimInfo.increment[0])
    {
      redValue = mRbgDimInfo.value[0] - mRbgDimInfo.increment[0];
    }       
    if(mRbgDimInfo.value[1] >= mRbgDimInfo.increment[1])
    {
      greenValue = mRbgDimInfo.value[1] - mRbgDimInfo.increment[1];
    }        
    if(mRbgDimInfo.value[2] >= mRbgDimInfo.increment[2])
    {
      blueValue = mRbgDimInfo.value[2] - mRbgDimInfo.increment[2];
    }    
  }
  
  if(gLedDimIn_c == mRbgDimInfo.mode)
  {    
    if((mRbgDimInfo.value[0] < mLedRgbLastValues[0]) || (mLedRgbLastValues[0]==0))
    {
      redValue = mRbgDimInfo.value[0] + mRbgDimInfo.increment[0];
    }
    if((mRbgDimInfo.value[1] < mLedRgbLastValues[1]) || (mLedRgbLastValues[1]==0))
    {
      greenValue = mRbgDimInfo.value[1] + mRbgDimInfo.increment[1];
    }        
    if((mRbgDimInfo.value[2] < mLedRgbLastValues[2]) || (mLedRgbLastValues[2]==0))
    {
      blueValue = mRbgDimInfo.value[2] + mRbgDimInfo.increment[2];
    }
  }
    
  if(--mRbgDimInfo.steps == 0)
  {
    if(gLedDimOut_c == mRbgDimInfo.mode)
    {
     /* Turn off the RGB LED (due to potential residual value over PWM channel, 
      * if the initial value is not an integer multiple of dimming steps) 
      */
      RGB_SET_LED_RED(0); RGB_SET_LED_GREEN(0); RGB_SET_LED_BLUE(0);
    }
    else
    {
      /* Set RGB LED to last values (due to potential residual value over PWM channel, 
      * if the initial value is not an integer multiple of dimming steps) 
      */
      RGB_SET_LED_RED(mLedRgbLastValues[0]); 
      RGB_SET_LED_GREEN(mLedRgbLastValues[1]);
      RGB_SET_LED_BLUE(mLedRgbLastValues[2]);
    }
    
    /* Stop the timer */
    TMR_StopTimer(mRGBLedTimerID);
    
    /* Clear the ongoing flag */
    OSA_InterruptDisable();
    mRbgDimInfo.ongoing = FALSE;
    OSA_InterruptEnable();
  }
  else
  {
    /* store the current RGB LED color */
    mRbgDimInfo.value[0] = redValue; 
    mRbgDimInfo.value[1] = greenValue; 
    mRbgDimInfo.value[2] = blueValue;
    
    /* set the current RGB LED color */
    RGB_SET_LED_RED(redValue); 
    RGB_SET_LED_GREEN(greenValue); 
    RGB_SET_LED_BLUE(blueValue);
  }
}

/******************************************************************************
* Name: LED_StartRgbDimming
* Description: Starts RGB LED dimming
* Param(s): [in]    mode - dimm in/out
*                   interval - dimming interval, in seconds
* Return: TRUE if no dimining is ongoing and the command has been accepted,
*         FALSE otherwise
******************************************************************************/
static bool_t LED_StartRgbDimming(LED_DimMode_t mode, uint8_t interval)
{    
  if(mode > gLedDimOut_c)
  {
    return FALSE;
  }  
  
  OSA_InterruptDisable();
  if(!mRbgDimInfo.ongoing)
  {
      mRbgDimInfo.ongoing = TRUE;
      OSA_InterruptEnable();
  }
  else
  {
    OSA_InterruptEnable();
    return FALSE;
  } 
      
  /* populate dimming info structure */
  mRbgDimInfo.interval = interval;
  mRbgDimInfo.mode = mode;
  if(mode == gLedDimIn_c)
  {
    mRbgDimInfo.value[0] = mRbgDimInfo.value[1] = mRbgDimInfo.value[2] = 0;
  }
  else
  {
    mRbgDimInfo.value[0] = mLedRgbLastValues[0];
    mRbgDimInfo.value[1] = mLedRgbLastValues[1];
    mRbgDimInfo.value[2] = mLedRgbLastValues[2];
  }
  mRbgDimInfo.steps = (uint16_t)(mRbgDimInfo.interval * TmrSeconds(1))/(uint16_t)(gRgbLedDimTimerTimeout_c);
  mRbgDimInfo.steps /= 3; /* adjust the steps number taking into account processing overhead */
  mRbgDimInfo.increment[0] = mLedRgbLastValues[0]/mRbgDimInfo.steps;
  mRbgDimInfo.increment[1] = mLedRgbLastValues[1]/mRbgDimInfo.steps;
  mRbgDimInfo.increment[2] = mLedRgbLastValues[2]/mRbgDimInfo.steps;        
  
  /* start the timer used for dimming */
  if(!TMR_IsTimerActive(mRGBLedTimerID))
  {
      TMR_StartIntervalTimer(mRGBLedTimerID, gRgbLedDimTimerTimeout_c, (pfTmrCallBack_t)LED_DimmingTimeout, (void*)((uint32_t)mRGBLedTimerID));          
  }
  
  return TRUE;
}
#endif /* gLedRgbEnabled_d && gRgbLedDimmingEnabled_d */
#endif /* gTMR_Enabled_d */
#endif /* gLEDSupported_d */
