/*****************************************************************************
 *
 * MODULE:             JN-AN-1243
 *
 * COMPONENT:          app_buttons.c
 *
 * DESCRIPTION:        board led driver
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
 * Copyright NXP B.V. 2017-2018. All rights reserved
 *
 ***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include "dbg.h"
#include "app.h"
#if (defined OM15081)
#include "fsl_debug_console.h"
#include "fsl_pwm.h"
#include "fsl_iocon.h"
#include "fsl_inputmux.h"
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef DEBUG_APP_LED
    #define TRACE_APP_LED            FALSE
#else
    #define TRACE_APP_LED            TRUE
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
#if (defined OM15081)
PRIVATE void APP_vInitWhiteLed( void);
PRIVATE void APP_vWhiteLedOon(void);
PRIVATE void APP_vWhiteLedOff(void);
#endif

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
#if (defined OM15081)
PRIVATE pwm_setup_t pwmChan;
PRIVATE uint16_t u16CurValue;
#endif
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
 * RETURNS: bool
 *
 ****************************************************************************/
PUBLIC void APP_vLedInitialise(void)
{

#if (defined OM15082)
    /* Define the init structure for the output LED pin*/
    gpio_pin_config_t led_config = {
        kGPIO_DigitalOutput, OFF,
    };

    GPIO_PinInit(GPIO, APP_BOARD_GPIO_PORT, APP_BOARD_LED1_PIN, &led_config);
    GPIO_PinInit(GPIO, APP_BOARD_GPIO_PORT, APP_BOARD_LED2_PIN, &led_config);
    GPIO_PinInit(GPIO, APP_BOARD_GPIO_PORT, APP_BOARD_LED3_PIN, &led_config);

    GPIO_PinInit(GPIO, APP_BOARD_GPIO_PORT, APP_BASE_BOARD_LED1_PIN, &led_config);
    GPIO_PinInit(GPIO, APP_BOARD_GPIO_PORT, APP_BASE_BOARD_LED2_PIN, &led_config);

#endif

#if (defined DONGLE)
// TODO Dobngle leds
#endif

#if (defined OM15081)
    /* Define the init structure for the output LED pin*/
    gpio_pin_config_t led_config = {
            kGPIO_DigitalOutput, OFF,
    };

    APP_vInitWhiteLed();
    APP_vWhiteLedOff();

    GPIO_PinInit(GPIO, APP_BOARD_GPIO_PORT, APP_BASE_BOARD_LED1_PIN, &led_config);

#endif

#if (defined OM5578)
    /* Define the init structure for the output LED pin*/
    gpio_pin_config_t led_config = {
            kGPIO_DigitalOutput, OFF,
    };

    GPIO_PinInit(GPIO, APP_BOARD_GPIO_PORT, APP_BASE_BOARD_LED1_PIN, &led_config);
    GPIO_PinInit(GPIO, APP_BOARD_GPIO_PORT, APP_BASE_BOARD_LED2_PIN, &led_config);
#endif
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
PUBLIC void APP_vSetLed(uint u8Led, bool_t bState)
{
    uint32 u32LedPin = 0;

#if (defined OM15082)
    switch (u8Led)
    {
    case LED1:
        u32LedPin = APP_BOARD_LED1_PIN;
        break;
    case LED2:
        u32LedPin = APP_BOARD_LED2_PIN;
        break;
    case LED3:
        u32LedPin = APP_BOARD_LED3_PIN;
        break;
    case LED4:
            u32LedPin = APP_BASE_BOARD_LED1_PIN;
            bState = ( bState==OFF );
            break;
    case LED5:
            u32LedPin = APP_BASE_BOARD_LED1_PIN;
            bState = ( bState==OFF );
            break;
    default:
        u32LedPin -= 1;
        break;
    }

    if (u32LedPin != 0xffffffff)
    {
        GPIO_PinWrite(GPIO, APP_BOARD_GPIO_PORT, u32LedPin, bState);
    }
#endif

#if (defined OM15081)
    switch (u8Led)
    {
    case LED1:
        (bState == ON) ? APP_vWhiteLedOon(): APP_vWhiteLedOff();
        u32LedPin -= 1;
        break;

    case LED2:
            u32LedPin = APP_BASE_BOARD_LED1_PIN;
            bState = ( bState==OFF );
            break;
    case LED3:
            u32LedPin = APP_BASE_BOARD_LED1_PIN;
            bState = ( bState==OFF );
            break;
    default:
        u32LedPin -= 1;
        break;
    }
    if (u32LedPin != 0xffffffff)
    {
        GPIO_PinWrite(GPIO, APP_BOARD_GPIO_PORT, u32LedPin, bState);
    }
#endif

#if (defined OM5578)
    switch (u8Led)
    {
    case LED1:
            u32LedPin = APP_BASE_BOARD_LED1_PIN;
            bState = ( bState==OFF );
            break;
    case LED2:
            u32LedPin = APP_BASE_BOARD_LED1_PIN;
            bState = ( bState==OFF );
            break;
    default:
        u32LedPin -= 1;
        break;
    }
    if (u32LedPin != 0xffffffff)
    {
        GPIO_PinWrite(GPIO, APP_BOARD_GPIO_PORT, u32LedPin, bState);
    }
#endif

#if (defined DONGLE)
    switch (u8Led)
        {
    case LED1:
        u32LedPin = APP_BASE_BOARD_LED1_PIN;
        break;
    case LED2:
        u32LedPin = APP_BASE_BOARD_LED1_PIN;
        break;
    default:
        u32LedPin -= 1;
        break;
    }

    if (u32LedPin != 0xffffffff)
    {
        GPIO_PinWrite(GPIO, APP_BOARD_GPIO_PORT, u32LedPin, bState);
    }
#endif

}


/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/
#if (defined OM15081)
/****************************************************************************
 *
 * NAME: APP_vInitWhiteLed
 *
 * DESCRIPTION:
 * initialise the pwm channels to drive the white led on OM15081 expansion voard
 *
 * PARAMETER:
 *
 * RETURNS:
 *
 ****************************************************************************/
PRIVATE void APP_vInitWhiteLed( void)
{
    status_t pwm_status;
    pwm_config_t pwmConfig;
    uint32_t pwmClockFrq;
    uint32_t pwmChanClk;


    PRINTF("DriverBulb_vInit\n");


     /* Configure PWM pin */
    IOCON_PinMuxSet(IOCON, 0, 6, IOCON_FUNC4 | IOCON_MODE_INACT | IOCON_DIGITAL_EN);

    /* Check default configuration configuration */
    PWM_GetDefaultConfig(&pwmConfig);
    pwmConfig.clk_sel = kCLOCK_PWMOsc32Mclk;
    pwmClockFrq = CLOCK_GetFreq(kCLOCK_Fro32M);

    /* PWM Init with user clock select */
    pwm_status = PWM_Init(PWM, &pwmConfig);
    if(pwm_status != kStatus_Success)
    {
        PRINTF("PWM_Channel init failed! \n");
        while(1);
    }

    /* Set up PWM channel to generate PWM of 600Hz starting with PWM off */
    pwmChan.pol_ctrl = kPWM_SetHighOnMatchLowOnPeriod;
    pwmChan.dis_out_level = kPWM_SetLow;
    pwmChan.prescaler_val = 0;
    pwmChanClk = pwmClockFrq / (1 + pwmChan.prescaler_val);
    pwmChan.period_val = USEC_TO_COUNT(1666, pwmChanClk);
    pwmChan.comp_val = ((255+1) * pwmChan.period_val) >> 8;


    u16CurValue = pwmChan.comp_val;

    if(PWM_SetupPwm (PWM, 6, &pwmChan) !=  kStatus_Success)
    {
        PRINTF("PWM chan setup failed\n");
        while(1);
    }

    /* Clear interrupt status for PWM channel */
    PWM_ClearStatusFlags(PWM, 6);

    /* Start the PWM generation channel */
    PWM_StartTimer(PWM, 6);
}

/****************************************************************************
 *
 * NAME: APP_vWhiteLedOn
 *
 * DESCRIPTION:
 * turns on the white led on OM15081 expansion board
 *
 * PARAMETER:
 *
 * RETURNS:
 *
 ****************************************************************************/
PRIVATE void APP_vWhiteLedOon(void)
{
    //PRINTF("DriverBulb_vOn\n");

    pwmChan.comp_val = u16CurValue;

    if(PWM_SetupPwm (PWM, 6, &pwmChan) !=  kStatus_Success)
    {
        PRINTF("PWM chan setup failed\n");
        while(1);
    }

}

/****************************************************************************
 *
 * NAME: APP_vWhiteLedOff
 *
 * DESCRIPTION:
 * turns off the white led on OM15081 expansion board
 *
 * PARAMETER:
 *
 * RETURNS:
 *
 ****************************************************************************/
PRIVATE void APP_vWhiteLedOff(void)
{
    //PRINTF("DriverBulb_vOff\r\n");

    pwmChan.comp_val = 0;

    if(PWM_SetupPwm (PWM, 6, &pwmChan) !=  kStatus_Success)
    {
        PRINTF("PWM chan setup failed\n");
        while(1);
    }
}

#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
