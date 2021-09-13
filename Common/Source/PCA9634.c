/****************************************************************************
 *
 * MODULE:             PCA9634 LED Driver
 *
 * DESCRIPTION:
 * A Driver for the NXP PCA9634 LED Driver
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
 * Copyright NXP B.V. 2012-2019. All rights reserved
 *
 ***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "dbg.h"
#include "app.h"
#include "SMBus.h"
#include "PCA9634.h"
#include "fsl_gpio.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#ifdef DEBUG_PCA9634
#define TRACE_PCA9634   TRUE
#else
#define TRACE_PCA9634   FALSE
#endif

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
 * NAME: bPCA9634_Init
 *
 * DESCRIPTION:
 * Initialises the PCA9634 LED Driver
 *
 * PARAMETERS: None
 *
 * RETURNS:
 * bool_t:  TRUE if the function completed successfully
 *          FALSE if there was an error
 *
 ****************************************************************************/
PUBLIC bool_t bPCA9634_Init(void)
{

    /* Define the init structure for the output enable pin*/
    gpio_pin_config_t oe_pin_config = {
        kGPIO_DigitalOutput, 0,
    };

    bool_t bOk = TRUE;

    DBG_vPrintf(TRACE_PCA9634, "\r\nPCA9634: Initialising I2C bus\n");

    /* Run bus at 100KHz */
    vSMBusInit();

    DBG_vPrintf(TRACE_PCA9634, "\r\nPCA9634: Write mode 1\n");
    bPCA9634_WriteRegister(PCA9634_MODE1, 1);

    DBG_vPrintf(TRACE_PCA9634, "\r\nPCA9634: Write mode 2\n");
#ifdef HIGH_POWER_LED
    bPCA9634_WriteRegister(PCA9634_MODE2, 0x14);
#else
    bPCA9634_WriteRegister(PCA9634_MODE2, 0x8);
#endif

    DBG_vPrintf(TRACE_PCA9634, "\r\nPCA9634: Write ledout0\n");
    bPCA9634_WriteRegister(PCA9634_LEDOUT0, 0xff);

    DBG_vPrintf(TRACE_PCA9634, "\r\nPCA9634: Set GPIO low\n");

    /* Set OE low to enable (inverted signal) */
    GPIO_PinInit(GPIO, APP_BOARD_GPIO_PORT, APP_PCA9634_OE_PIN, &oe_pin_config);

    return(bOk);
}

/****************************************************************************
 *
 * NAME: bPCA9634_SetChannelLevel
 *
 * DESCRIPTION:
 * Sets the PWM level of a particular channel. On DR1175 only channels 0-2
 * are connected to LEDs, but the others come out on a header so could be
 * used as well.
 *
 * PARAMETERS: Channel (0-7)
 *             Level (0-255)
 *
 * RETURNS:
 * bool_t:  TRUE if the function completed successfully
 *          FALSE if there was an error
 *
 * EXAMPLES:
 * bPCA9634_WriteRegister(0, 187);
 *
 ****************************************************************************/
PUBLIC bool_t bPCA9634_SetChannelLevel(uint8 u8Channel, uint8 u8Level)
{
    bool_t bReturn = FALSE;
    /* Check against available registers */
    if (u8Channel < 8)
    {
        /* Change channel to register number */
        u8Channel += PCA9634_PWM0;

        bReturn = bPCA9634_WriteRegister(u8Channel, u8Level);
    }
    return bReturn;
}

/****************************************************************************
 *
 * NAME: bPCA9634_SetGroupLevel
 *
 * DESCRIPTION:
 * Sets the PWM level of all channels.
 *
 * PARAMETERS: Level (0-255)
 *
 * RETURNS:
 * bool_t:  TRUE if the function completed successfully
 *          FALSE if there was an error
 *
 * EXAMPLES:
 * bPCA9634_WriteRegister(0, 187);
 *
 ****************************************************************************/
PUBLIC bool_t bPCA9634_SetGroupLevel(uint8 u8Level)
{
    bool_t bReturn;
    bReturn = bPCA9634_WriteRegister(PCA9634_GRPPWM, u8Level);
    return bReturn;
}

/****************************************************************************
 *
 * NAME: bPCA9634_WriteRegister
 *
 * DESCRIPTION:
 * Writes to a Register on the PCA9643
 *
 * PARAMETERS: None
 *
 * RETURNS:
 * bool_t:  TRUE if the function completed successfully
 *          FALSE if there was an error
 *
 * EXAMPLES:
 * bPCA9634_WriteRegister(PCA9634_PWM1,u8Green);
 *
 ****************************************************************************/
PUBLIC bool_t bPCA9634_WriteRegister(uint8 u8Command, uint8 u8Data)
{
    bool_t bOk = TRUE;

    DBG_vPrintf(TRACE_PCA9634, "\r\nPCA9634: Write Cmd=%02x Data=%02x\n", u8Command, u8Data);
    bOk &= bSMBusWrite(PCA9634_ADDRESS, u8Command, 1, &u8Data);

    return(bOk);

}

/****************************************************************************
 *
 * NAME: u8PCA9634_ReadRegister
 *
 * DESCRIPTION:
 * Reads from a Register on the PCA9643
 *
 * PARAMETERS: None
 *
 * RETURNS:
 * bool_t:  TRUE if the function completed successfully
 *          FALSE if there was an error
 *
 ****************************************************************************/
PUBLIC uint8 u8PCA9634_ReadRegister(uint8 u8Command)
{
    uint8 u8Data = 0;

    bSMBusRandomRead(PCA9634_ADDRESS, u8Command, 1, &u8Data);

    return(u8Data);
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
