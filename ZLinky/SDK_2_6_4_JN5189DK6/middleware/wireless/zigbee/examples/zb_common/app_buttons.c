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
#include "ZQueue.h"
#include "ZTimer.h"
#include "app.h"
#include "app_common.h"
#include "app_buttons.h"
#include "fsl_iocon.h"
#include "fsl_gpio.h"
#include "fsl_gint.h"
#include "app_main.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef TRACE_APP_BUTTON
    #define TRACE_APP_BUTTON            TRUE
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
void gint_callback(void);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
extern tszQueue APP_msgAppEvents;

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
static uint8_t       s_u8ButtonDebounce[APP_BUTTONS_NUM];
static uint8_t       s_u8ButtonState[APP_BUTTONS_NUM];
static const uint8_t s_u8ButtonDIOLine[APP_BUTTONS_NUM] =
{
      APP_BOARD_SW0_PIN
};

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 *
 * NAME: APP_bButtonInitialise
 *
 * DESCRIPTION:
 * Button Initialization
 *
 * PARAMETER: void
 *
 * RETURNS: bool_t
 *
 ****************************************************************************/
bool_t APP_bButtonInitialise(void)
{
    uint8_t u8Button;
    gpio_pin_config_t switch_config = {kGPIO_DigitalInput,};

    /* Initialise arrays */
    memset(s_u8ButtonDebounce, 0xFF, sizeof(s_u8ButtonDebounce));
    memset(s_u8ButtonState,    0x00, sizeof(s_u8ButtonState));

    /* Loop through buttons */
    for (u8Button = 0; u8Button < APP_BUTTONS_NUM; u8Button++)
    {
        /* Valid pin ? */
        if (s_u8ButtonDIOLine[u8Button] < APP_NUMBER_OF_GPIO)
        {
#if APP_BUTTON_INVERT_MASK
            /* Inverted operation ? */
            if (APP_BUTTON_INVERT_MASK & (1 << s_u8ButtonDIOLine[u8Button]))
            {
                /* Configure io mux for pull down operation */
                IOCON_PinMuxSet(IOCON, 0, s_u8ButtonDIOLine[u8Button], IOCON_FUNC0 | IOCON_MODE_PULLDOWN | IOCON_DIGITAL_EN);
            }
            /* Normal operation ? */
            else
#endif
            {
                /* Configure io mux for pull up operation */
                IOCON_PinMuxSet(IOCON, 0, s_u8ButtonDIOLine[u8Button], IOCON_FUNC0 | IOCON_MODE_PULLUP | IOCON_DIGITAL_EN);
            }
            /* Initialise GPIO use */
            GPIO_PinInit(GPIO, APP_BOARD_GPIO_PORT, s_u8ButtonDIOLine[u8Button], &switch_config);
        }
    }

    /* Initialize GINT0 */
    GINT_Init(GINT0);
    /* Setup GINT0 for edge trigger, "OR" mode */
    GINT_SetCtrl(GINT0, kGINT_CombineOr, kGINT_TrigEdge, gint_callback);
    /* Select pins & polarity for GINT0 */
    GINT_ConfigPins(GINT0, DEMO_GINT0_PORT, DEMO_GINT0_POL_MASK, DEMO_GINT0_ENA_MASK);
    /* Enable callback(s) */
    GINT_EnableCallback(GINT0);


    /* If we came out of deep sleep; perform appropriate action as well based
       on button press.*/
    APP_cbTimerButtonScan(NULL);

    uint32_t u32Buttons = APP_u32GetSwitchIOState();

    if (u32Buttons != APP_BUTTONS_DIO_MASK)
    {
        return TRUE;
    }
    return FALSE;
}

/****************************************************************************
 *
 * NAME: APP_u32GetSwitchIOState
 *
 * DESCRIPTION:
 * reads the IO state of any switch inputs, flips the bits of any switches that are
 * active high
 *
 * PARAMETER:
 *
 * RETURNS:
 *
 ****************************************************************************/
uint32_t APP_u32GetSwitchIOState(void)
{
    uint32_t u32Return = -0xffffffff;
    #if (!defined DONGLE)
    {
        uint32_t u32ReadPinsInput;

        /* Read all GPIOs */
        u32ReadPinsInput = GPIO_PortRead(GPIO, APP_BOARD_GPIO_PORT);
        /* Apply mask and inversions */
        u32Return = (u32ReadPinsInput & APP_BUTTONS_DIO_MASK) ^ APP_BUTTON_INVERT_MASK;
    }
    #endif
    //DBG_vPrintf(TRACE_APP_BUTTON, "APP_u32GetSwitchIOState()=0x%08x\r\n", u32Return);

    return u32Return;
}


/****************************************************************************
 *
 * NAME: gint_callback
 *
 * DESCRIPTION:
 *callback from the group io interrupt
 *
 * PARAMETER:
 *
 * RETURNS:
 *
 ****************************************************************************/
void gint_callback(void)
{
    /* Take action for gint event */;
    uint32_t u32IOStatus = APP_u32GetSwitchIOState();
    //DBG_vPrintf(TRACE_APP_BUTTON, "GINT Pin Interrupt event detected %x\r\n", u32IOStatus);

    if (u32IOStatus != APP_BUTTONS_DIO_MASK)
    {
        /* disable edge detection until scan complete */
        GINT_DisableCallback(GINT0);
        /* Run timer */
        //DBG_vPrintf(TRACE_APP_BUTTON, ";g");
        ZTIMER_eStop(u8TimerButtonScan);
        ZTIMER_eStart(u8TimerButtonScan, ZTIMER_TIME_MSEC(10));
    }
}


/****************************************************************************
 *
 * NAME: APP_cbTimerButtonScan
 *
 * DESCRIPTION:
 * Timer callback to debounce the button presses
 *
 * PARAMETER:
 *
 * RETURNS:
 *
 ****************************************************************************/
void APP_cbTimerButtonScan(void *pvParam)
{
    /*
     * The DIO changed status register is reset here before the scan is performed.
     * This avoids a race condition between finishing a scan and re-enabling the
     * DIO to interrupt on a falling edge.
     */

    uint8_t u8AllReleased = 0xff;
    unsigned int i;
    uint32_t u32DIOState = APP_u32GetSwitchIOState();

    for (i = 0; i < APP_BUTTONS_NUM; i++)
    {
        uint8_t u8Button = (uint8_t) ((u32DIOState >> s_u8ButtonDIOLine[i]) & 1);

        s_u8ButtonDebounce[i] <<= 1;
        s_u8ButtonDebounce[i] |= u8Button;
        u8AllReleased &= s_u8ButtonDebounce[i];

        if (0 == s_u8ButtonDebounce[i] && !s_u8ButtonState[i])
        {
            s_u8ButtonState[i] = TRUE;

            /*
             * button consistently depressed for 8 scan periods
             * so post message to application task to indicate
             * a button down event
             */
            APP_tsEvent sButtonEvent;
            sButtonEvent.eType = APP_E_EVENT_BUTTON_DOWN;
            sButtonEvent.uEvent.sButton.u8Button = i;

            DBG_vPrintf(TRACE_APP_BUTTON, "Button DN=%d\r\n", i);

            if(ZQ_bQueueSend(&APP_msgAppEvents, &sButtonEvent) == FALSE)
            {
                DBG_vPrintf(TRACE_APP_BUTTON, "Button: Failed to post Event %d \r\n", sButtonEvent.eType);
            }
        }
        else if (0xff == s_u8ButtonDebounce[i] && s_u8ButtonState[i] != FALSE)
        {
            s_u8ButtonState[i] = FALSE;

            /*
             * button consistently released for 8 scan periods
             * so post message to application task to indicate
             * a button up event
             */
            APP_tsEvent sButtonEvent;
            sButtonEvent.eType = APP_E_EVENT_BUTTON_UP;
            sButtonEvent.uEvent.sButton.u8Button = i;

            DBG_vPrintf(TRACE_APP_BUTTON, "Button UP=%i\r\n", i);

            if(ZQ_bQueueSend(&APP_msgAppEvents, &sButtonEvent) == FALSE)
            {
                DBG_vPrintf(TRACE_APP_BUTTON, "Button: Failed to post Event %d \r\n", sButtonEvent.eType);
            }
        }
    }

    if (0xff == u8AllReleased)
    {
        /*
         * all buttons high so set dio to interrupt on change
         */
        DBG_vPrintf(TRACE_APP_BUTTON, "ALL UP\r\n", i);
        GINT_EnableCallback(GINT0);
        //DBG_vPrintf(TRACE_APP_BUTTON, ":t");
        ZTIMER_eStop(u8TimerButtonScan);
    }
    else
    {
        /*
         * one or more buttons is still depressed so continue scanning
         */
        //DBG_vPrintf(TRACE_APP_BUTTON, ";t");
        ZTIMER_eStop(u8TimerButtonScan);
        ZTIMER_eStart(u8TimerButtonScan, ZTIMER_TIME_MSEC(10));
    }
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
