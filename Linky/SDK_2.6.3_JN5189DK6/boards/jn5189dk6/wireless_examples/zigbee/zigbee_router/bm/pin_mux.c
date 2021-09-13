/*
* Copyright 2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

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

#define SPIFI_FAST_IO_MODE                                              \
    IOCON_PIO_FUNC(IOCON_QSPI_MODE_FUNC) |                              \
    /* No addition pin function */                                      \
    /* Fast mode, slew rate control is enabled */                       \
    IOCON_PIO_SLEW0(1) | IOCON_PIO_SLEW1(1) |                           \
    /* Input function is not inverted */                                \
    IOCON_PIO_INV_DI |                                                  \
    /* Enables digital function */                                      \
    IOCON_PIO_DIGITAL_EN |                                              \
    /* Input filter disabled */                                         \
    IOCON_PIO_INPFILT_OFF |                                             \
    /* Open drain is disabled */                                        \
    IOCON_PIO_OPENDRAIN_DI |                                            \
    /* SSEL is disabled */                                              \
    IOCON_PIO_SSEL_DI

#define LP_DIGITAL_PULLUP_CFG IOCON_PIO_FUNC(0) |\
                              IOCON_PIO_MODE(0) |\
                              IOCON_PIO_DIGIMODE(1)

#define SPIFI_FAST_IO_MODE_INACT   (SPIFI_FAST_IO_MODE | IOCON_PIO_MODE(2))
#define SPIFI_FAST_IO_MODE_PULLUP  (SPIFI_FAST_IO_MODE | IOCON_PIO_MODE(0))

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/


/*****************************************************************************
 * Local Prototypes
 ****************************************************************************/
static const iocon_group_t sfifi_io_cfg[] = {
    [0] = {
        .port = 0,
        .pin =  16,               /* SPIFI Chip Select */
        .modefunc = SPIFI_FAST_IO_MODE_PULLUP,
    },
    [1] = {
        .port = 0,
        .pin =  17,                 /*SPIFI DIO3 */
        .modefunc = SPIFI_FAST_IO_MODE_INACT,
    },
    [2] = {
        .port = 0,
        .pin =  18,                 /*SPIFI CLK */
        .modefunc = SPIFI_FAST_IO_MODE_INACT,
    },
    [3] = {
        .port = 0,
        .pin =  19,                 /*SPIFI DIO0 */
        .modefunc =SPIFI_FAST_IO_MODE_INACT,
    },
    [4] = {
        .port = 0,
        .pin =  20,                 /*SPIFI DIO2 */
        .modefunc = SPIFI_FAST_IO_MODE_INACT,
    },
    [5] = {
        .port = 0,
        .pin =  21,                 /*SPIFI DIO1 */
        .modefunc = SPIFI_FAST_IO_MODE_INACT,
    },
};

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

    /* Debugger signals (SWCLK, SWDIO) - need to turn it OFF to reduce power consumption in power modes*/
    IOCON_PinMuxSet(IOCON, 0, 12, IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_DIGITAL_EN);
    IOCON_PinMuxSet(IOCON, 0, 13, IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_DIGITAL_EN);

#ifndef ENABLE_SUBG_IF
    /* I2C0  */
    IOCON_PinMuxSet(IOCON, 0, 10, IOCON_FUNC5 | IOCON_DIGITAL_EN | IOCON_STDI2C_EN);  /* I2C0_SCL */
    IOCON_PinMuxSet(IOCON, 0, 11, IOCON_FUNC5 | IOCON_DIGITAL_EN | IOCON_STDI2C_EN);  /* I2C0_SDA */
#else
    /* USART1 RX/TX pin */
    IOCON_PinMuxSet(IOCON, 0, 10, IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_GPIO_MODE | IOCON_INPFILT_OFF | IOCON_DIGITAL_EN);
    IOCON_PinMuxSet(IOCON, 0, 11, IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_GPIO_MODE | IOCON_INPFILT_OFF | IOCON_DIGITAL_EN);

    /* MSCL, MSDA, RST */
    IOCON_PinMuxSet(IOCON, 0, 18, IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGITAL_EN);
    IOCON_PinMuxSet(IOCON, 0, 4, IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGITAL_EN);
    IOCON_PinMuxSet(IOCON, 0, 6, IOCON_FUNC0 |  IOCON_MODE_INACT | IOCON_DIGITAL_EN);
#endif

}

void BOARD_SetPinsForPowerMode(void)
{
    for ( int i=0; i<22; i++)
    {
        /* configure GPIOs to Input mode */
        GPIO_PinInit(GPIO, 0, i, &((const gpio_pin_config_t){kGPIO_DigitalInput, 1}));
        IOCON_PinMuxSet(IOCON, 0, i, IOCON_FUNC0 |  IOCON_MODE_INACT | IOCON_DIGITAL_EN);
    }
}

void BOARD_InitSPIFI( void )
{
    uint32_t divisor;

    /* Enables the clock for the I/O controller block. 0: Disable. 1: Enable.: 0x01u */
    CLOCK_EnableClock(kCLOCK_Iocon);
    for (int i = 0; i < 6; i++)
    {
        if ((i==1 || i==4) && CHIP_USING_SPIFI_DUAL_MODE())
            continue;
        IOCON_PinMuxSet(IOCON,
                        sfifi_io_cfg[i].port,
                        sfifi_io_cfg[i].pin,
                        sfifi_io_cfg[i].modefunc);
    }
    RESET_SetPeripheralReset(kSPIFI_RST_SHIFT_RSTn);
    /* Enable clock to block and set divider */
    CLOCK_AttachClk(kMAIN_CLK_to_SPIFI);
    divisor = CLOCK_GetSpifiClkFreq() / BOARD_SPIFI_CLK_RATE;
    CLOCK_SetClkDiv(kCLOCK_DivSpifiClk, divisor ? divisor : 1, false);
    RESET_ClearPeripheralReset(kSPIFI_RST_SHIFT_RSTn);

}

void BOARD_SetSpiFi_LowPowerEnter(void)
{
    /* Enables the clock for the I/O controller block. 0: Disable. 1: Enable.: 0x01u */
    CLOCK_EnableClock(kCLOCK_Iocon);
    /* Enables the clock for the GPIO0 module */
    CLOCK_EnableClock(kCLOCK_Gpio0);
    /* Turn all pins into analog inputs */
    for (int i = 0; i < 6; i++)
    {
        if ((i==1 || i==4) && CHIP_USING_SPIFI_DUAL_MODE())
            continue;
        /* Initialize GPIO functionality on each pin : all inputs */
        GPIO_PinInit(GPIO,
                     sfifi_io_cfg[i].port,
                     sfifi_io_cfg[i].pin,
                     &((const gpio_pin_config_t){kGPIO_DigitalInput, 1}));
        IOCON_PinMuxSet(IOCON,
                        sfifi_io_cfg[i].port,
                        sfifi_io_cfg[i].pin,
                        LP_DIGITAL_PULLUP_CFG);
    }
}
