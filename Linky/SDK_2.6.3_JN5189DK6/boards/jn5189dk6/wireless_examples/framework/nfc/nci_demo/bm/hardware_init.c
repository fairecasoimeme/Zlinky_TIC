/*
* Copyright 2020 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "pin_mux.h"
#include "board.h"
#include <stdbool.h>
#include "fsl_power.h"


void BOARD_InitClocks(void)
{
    /* Already set to mainCLK by default */
    CLOCK_AttachClk(kMAIN_CLK_to_ASYNC_APB);

    /* 32MHz clock */
    CLOCK_EnableClock(kCLOCK_Xtal32M);      // Only required if CPU running from 32MHz crystal
    CLOCK_EnableClock(kCLOCK_Fro32M);       // derived from 198MHz FRO

    /* 32KHz clock */
#if !gLoggingActive_d
    CLOCK_EnableClock(CLOCK_32k_source);
#endif

    CLOCK_EnableClock(kCLOCK_Iocon);
    CLOCK_EnableClock(kCLOCK_InputMux);

    /* Enable GPIO for LED controls */
    CLOCK_EnableClock(kCLOCK_Gpio0);

    /* INMUX and IOCON are used by many apps, enable both INMUX and IOCON clock bits here. */
    CLOCK_AttachClk(kOSC32M_to_USART_CLK);
    CLOCK_EnableClock(kCLOCK_Usart0);
    CLOCK_EnableClock(kCLOCK_Usart1);

    CLOCK_EnableClock(kCLOCK_I2c0) ;
    CLOCK_AttachClk(kOSC32M_to_I2C_CLK);

}

void BOARD_SetClockForPowerMode(void)
{
    // Comment line if some PRINTF is still done prior to this step before sleep
    CLOCK_AttachClk(kNONE_to_USART_CLK);
    CLOCK_DisableClock(kCLOCK_Iocon);
    CLOCK_DisableClock(kCLOCK_InputMux);
    CLOCK_DisableClock(kCLOCK_Gpio0);
    CLOCK_DisableClock(kCLOCK_I2c0);
    CLOCK_DisableClock(kCLOCK_Usart0);
    CLOCK_DisableClock(kCLOCK_Usart1);
}

void hardware_init(void)
{
    BOARD_common_hw_init();
}
/*${function:end}*/
