/*****************************************************************************
 *
 * MODULE:             JN-AN-1243
 *
 * COMPONENT:          pin_mux.c
 *
 * DESCRIPTION:        Pin multiplexing
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
#include "fsl_common.h"
#include "fsl_iocon.h"
#include "fsl_gpio.h"
#include "pin_mux.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/



#ifndef BOARD_USECLKINSRC
#define BOARD_USECLKINSRC   (0)
#endif

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Local Prototypes
 ****************************************************************************/


 /*****************************************************************************
 * Private functions
 ****************************************************************************/



/*******************************************************************************
 * Code
 ******************************************************************************/

void BOARD_InitPins(void)
{
	/* USART0 RX/TX pin */
	    IOCON_PinMuxSet(IOCON, 0, 8, IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_DIGITAL_EN);
	    IOCON_PinMuxSet(IOCON, 0, 9, IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_DIGITAL_EN);

	    /* USART1 RX pin */
	    IOCON_PinMuxSet(IOCON, 0, 1, IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_DIGITAL_EN);

	    /* Debugger signals (SWCLK, SWDIO) - need to turn it OFF to reduce power consumption in power modes*/
	    IOCON_PinMuxSet(IOCON, 0, 12, IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_DIGITAL_EN);

	    //LED Linky
	    IOCON_PinMuxSet(IOCON, 0, 10, IOCON_FUNC0 | IOCON_DIGITAL_EN | IOCON_GPIO_MODE | IOCON_MODE_PULLDOWN);

	    /* I2C0  */
	    //IOCON_PinMuxSet(IOCON, 0, 10, IOCON_FUNC5 | IOCON_DIGITAL_EN | IOCON_STDI2C_EN);  /* I2C0_SCL */
	    //IOCON_PinMuxSet(IOCON, 0, 11, IOCON_FUNC5 | IOCON_DIGITAL_EN | IOCON_STDI2C_EN);  /* I2C0_SDA */

}

void BOARD_SetPinsForPowerMode(void)
{
    for ( int i=0; i<22; i++)
    {
        /* configure GPIOs to Input mode */
        /*GPIO_PinInit(GPIO, 0, i, &((const gpio_pin_config_t){kGPIO_DigitalInput, 1}));
        IOCON_PinMuxSet(IOCON, 0, i, IOCON_FUNC0 |  IOCON_MODE_INACT | IOCON_DIGITAL_EN);*/
    	if (i !=10)
		{
			/* configure GPIOs to Input mode */
			GPIO_PinInit(GPIO, 0, i, &((const gpio_pin_config_t){kGPIO_DigitalInput, 1}));
			IOCON_PinMuxSet(IOCON, 0, i, IOCON_FUNC0 |  IOCON_MODE_PULLUP | IOCON_DIGITAL_EN);
		}
    }
}
