/*
* Copyright 2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


#include "app.h"
#include "pin_mux.h"
#include "board.h"
#include "fsl_power.h"
#include "DebugExceptionHandlers_jn518x.h"
#include "fsl_iocon.h"
#include "fsl_gpio.h"
#include "fsl_common.h"
#include "fsl_spifi.h"
#include <stdbool.h>

/* SPIFI Flash routines for K32W041AM Chip */
#define  K32W041AM_SPIFI_FLASH_BUSY_FLAG_MASK  0x01

gpio_pin_config_t led_config = {
    kGPIO_DigitalOutput, 0,
};

extern void OSA_TimeInit(void);

static uint32_t BOARD_SPIFI_FLASH_ReadStatusReq(void)
{
    spifi_command_t status_cmd = { 1, false, kSPIFI_DataInput,  0, kSPIFI_CommandAllSerial,    kSPIFI_CommandOpcodeOnly, 0x05};        /* Read status register */

    SPIFI_SetCommand(SPIFI, &status_cmd);
    while ((SPIFI->STAT & SPIFI_STAT_INTRQ_MASK) == 0U)
    {
    }
    return SPIFI_ReadPartialWord(SPIFI, status_cmd.dataLen);
}

static uint8_t BOARD_SPIFI_FLASH_isBusy(void)
{
    return (BOARD_SPIFI_FLASH_ReadStatusReq() & K32W041AM_SPIFI_FLASH_BUSY_FLAG_MASK) == K32W041AM_SPIFI_FLASH_BUSY_FLAG_MASK;
}

static void BOARD_SPIFI_FLASH_Config(void)
{
    if(CHIP_USING_SPIFI_DUAL_MODE())
    {
        spifi_command_t DP_cmd = {0, false, kSPIFI_DataInput,  0, kSPIFI_CommandAllSerial,    kSPIFI_CommandOpcodeOnly, 0xB9};

        spifi_config_t config = {0};

        /* Enable DIOs */
        BOARD_InitSPIFI();

        /* Initialize SPIFI base driver */
        SPIFI_GetDefaultConfig(&config);

        config.dualMode = kSPIFI_DualMode;

        SPIFI_Init(SPIFI, &config);

        while (BOARD_SPIFI_FLASH_isBusy());

        /* set SPIFI Flash in Deep power down  mode */
        SPIFI_SetCommand(SPIFI, &DP_cmd);
        SPIFI_Deinit(SPIFI);
        SYSCON->AHBCLKCTRLSET[0] = SYSCON_AHBCLKCTRLSET0_SPIFI_CLK_SET_MASK;
        BOARD_SetSpiFi_LowPowerEnter();
    }
}

void BOARD_InitClocks(void)
{
    /* what is it for exactly */
    CLOCK_EnableAPBBridge();

    /* 32MHz clock */
    CLOCK_EnableClock(kCLOCK_Fro32M);       // derived from 198MHz FRO
    CLOCK_EnableClock(kCLOCK_Xtal32M);

    /* 32KHz clock */
    CLOCK_EnableClock(kCLOCK_Fro32k);      // isnt' strictly required but put for clarity
    //CLOCK_EnableClock(kCLOCK_Xtal32k);

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
    reset_cause_t reset_cause;
    /* Security code to allow debug access */
    SYSCON->CODESECURITYPROT = 0x87654320;

    /* Configure the optimized active voltages, set up the BODs */
    POWER_Init();

    /* reset FLEXCOMM for USART - mandatory from power down 4 at least*/
    RESET_PeripheralReset(kUSART0_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kGPIO0_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kI2C0_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kADC0_RST_SHIFT_RSTn);
    /* For power optimization, we need a limited clock enabling specific to this application
     * This replaces BOARD_BootClockRUN() usually called in other application    */
    BOARD_InitClocks();
    BOARD_InitDebugConsole();
    BOARD_InitPins();

    /* Enable DMA access to RAM (assuming default configuration and MAC
     * buffers not in first block of RAM) */
    *(volatile uint32_t *)0x40001000 = 0xE000F733;
    *(volatile uint32_t *)0x40001004 = 0x109;

    /* Initialise output LED GPIOs */
    GPIO_PinInit(GPIO, APP_BOARD_GPIO_PORT, APP_BASE_BOARD_LED1_PIN, &led_config);
    GPIO_PinInit(GPIO, APP_BOARD_GPIO_PORT, APP_BASE_BOARD_LED2_PIN, &led_config);

    SYSCON -> MAINCLKSEL       = 3;  /* 32 M FRO */
    SystemCoreClockUpdate();
    OSA_TimeInit();

    reset_cause = POWER_GetResetCause();

    /* on K32W041AM chip, external flash need to be initialised and set in deep power down to avoid over consumption */
    if(reset_cause != RESET_WAKE_PD)
    {
    	BOARD_SPIFI_FLASH_Config();
    }

    /* Initialise exception handlers for debugging */
    vDebugExceptionHandlersInitialise();
}

/****************************************************************************
 *
 * NAME: hardware_init
 *
 * DESCRIPTION:
 *
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void hardware_init(void)
{
	BOARD_InitHardware();
}
