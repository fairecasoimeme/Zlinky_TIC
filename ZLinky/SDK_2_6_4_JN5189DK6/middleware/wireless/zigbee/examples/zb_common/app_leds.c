/*
* Copyright 2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <EmbeddedTypes.h>
#include "dbg.h"
#include "app.h"
#include "fsl_gpio.h"
#include "fsl_debug_console.h"
#include "fsl_iocon.h"
#include "fsl_inputmux.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/


/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 *
 * NAME: APP_vLedInitialise
 *
 * DESCRIPTION:
 * initialise the application leds
 *
 * PARAMETER: void
 *
 * RETURNS: bool_t
 *
 ****************************************************************************/
void APP_vLedInitialise(void)
{

    /* Define the init structure for the output LED pin*/
    gpio_pin_config_t led_config = {
            kGPIO_DigitalOutput, OFF,
    };

    GPIO_PinInit(GPIO, APP_BOARD_GPIO_PORT, APP_BASE_BOARD_LED1_PIN, &led_config);
    GPIO_PinInit(GPIO, APP_BOARD_GPIO_PORT, APP_BASE_BOARD_LED2_PIN, &led_config);

}

/****************************************************************************
 *
 * NAME: APP_vSetLed
 *
 * DESCRIPTION:
 * set the state ofthe given application led
 *
 * PARAMETER:
 *
 * RETURNS:
 *
 ****************************************************************************/
void APP_vSetLed(uint8_t u8Led, bool_t bState)
{
    uint32_t u32LedPin = 0;
    switch (u8Led)
    {
    case LED1:
    	u32LedPin = APP_BASE_BOARD_LED1_PIN;
    	bState = (bState == OFF);
        break;
    case LED2:
    	u32LedPin = APP_BASE_BOARD_LED2_PIN;
    	bState = (bState == OFF);
        break;
    default:
        u32LedPin -= 1;
        break;
    }

    if (u32LedPin != 0xffffffff)
    {
        GPIO_PinWrite(GPIO, APP_BOARD_GPIO_PORT, u32LedPin, bState);
    }
}


/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/


/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
