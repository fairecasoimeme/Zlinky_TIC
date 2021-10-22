/*****************************************************************************
 *
 * MODULE:             JN-AN-1243
 *
 * COMPONENT:          hardware_init.c
 *
 * DESCRIPTION:        Hardware initialisations
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
/*${header:start}*/
#include "pin_mux.h"
#include "board.h"
#include "fsl_power.h"
#include <stdbool.h>
#ifdef CPU_K32W041AMK
#include "Eeprom_User.h"
#endif

/*${header:end}*/

/*${function:start}*/

void BOARD_InitClocks(void)
{
    /* what is it for exactly */
    CLOCK_EnableAPBBridge();

    /* 32MHz clock */
    CLOCK_EnableClock(kCLOCK_Fro32M);       // derived from 198MHz FRO
    CLOCK_EnableClock(kCLOCK_Xtal32M);

    /* 32KHz clock */
    #if (gClkUseFro32K == 1)
    CLOCK_EnableClock(kCLOCK_Fro32k);      // isnt' strictly required but put for clarity
    #else
    CLOCK_EnableClock(kCLOCK_Xtal32k);
    #endif

    CLOCK_EnableClock(kCLOCK_Iocon);
    CLOCK_EnableClock(kCLOCK_InputMux);

    /* Enable GPIO for LED controls */
    CLOCK_EnableClock(kCLOCK_Gpio0);

    /* Enable ADC clock */
    CLOCK_EnableClock(kCLOCK_Adc0);
    /* Power on the ADC converter. */
    POWER_EnablePD(kPDRUNCFG_PD_LDO_ADC_EN);

    /* INMUX and IOCON are used by many apps, enable both INMUX and IOCON clock bits here. */
    CLOCK_AttachClk(kOSC32M_to_USART_CLK);

    CLOCK_EnableClock(kCLOCK_Aes);
    CLOCK_EnableClock(kCLOCK_I2c0) ;
    CLOCK_AttachClk(kOSC32M_to_I2C_CLK);

    /* WWDT clock config (32k oscillator, no division) */
    CLOCK_AttachClk(kOSC32K_to_WDT_CLK);
    CLOCK_SetClkDiv(kCLOCK_DivWdtClk, 1, true);

    /* Attach the ADC clock. */
    CLOCK_AttachClk(kXTAL32M_to_ADC_CLK);
    /* Enable LDO ADC 1v1 */
    PMC -> PDRUNCFG |= PMC_PDRUNCFG_ENA_LDO_ADC_MASK;
}

void BOARD_SetClockForPowerMode(void)
{
    // Comment line if some PRINTF is still done prior to this step before sleep
    CLOCK_AttachClk(kNONE_to_USART_CLK);
    CLOCK_DisableClock(kCLOCK_Iocon);
    CLOCK_DisableClock(kCLOCK_InputMux);
    CLOCK_DisableClock(kCLOCK_Gpio0);
    CLOCK_DisableClock(kCLOCK_Aes);
    CLOCK_DisableClock(kCLOCK_Xtal32M);
    CLOCK_DisableClock(kCLOCK_Fro48M);
    CLOCK_DisableClock(kCLOCK_I2c0);
    /* Disable LDO ADC 1v1 */
    PMC -> PDRUNCFG &= ~PMC_PDRUNCFG_ENA_LDO_ADC_MASK;
    POWER_DisablePD(kPDRUNCFG_PD_LDO_ADC_EN);
    CLOCK_DisableClock(kCLOCK_Adc0);
}


void BOARD_InitHardware(void)
{
    /* Security code to allow debug access */
    SYSCON->CODESECURITYPROT = 0x87654320;

    /* Configure the optimized active voltages, set up the BODs */
    POWER_Init();

    /* reset FLEXCOMM for USART - mandatory from power down 4 at least*/
    RESET_PeripheralReset(kUSART0_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kGPIO0_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kI2C0_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kADC0_RST_SHIFT_RSTn);

    /* enable WDTRESETENABLE bit in PMC CTRL */
    PMC->CTRL |= PMC_CTRL_WDTRESETENABLE_MASK;

    /* For power optimization, we need a limited clock enabling specific to this application
     * This replaces BOARD_BootClockRUN() usually called in other application    */
    BOARD_InitClocks();
    BOARD_InitDebugConsole();
    BOARD_InitPins();
	
   /*Put SPIFI flash to low power*/
#ifdef CPU_K32W041AMK
    _EEPROM_Init();
    _EEPROM_DeInit();
#endif

}
/*${function:end}*/
