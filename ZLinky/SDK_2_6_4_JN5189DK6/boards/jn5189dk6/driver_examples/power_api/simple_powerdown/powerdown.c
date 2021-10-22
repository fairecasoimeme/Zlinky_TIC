/* Copyright (c) 2016, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "fsl_debug_console.h"
#include "rom_lowpower.h"
#include "rom_api.h"
#include "fsl_ctimer.h"
#include "fsl_gpio.h"
#include "fsl_gint.h"
#include "fsl_inputmux.h"
#include "fsl_power.h"
#include "fsl_wtimer.h"
#include "fsl_flash.h"
#include "fsl_rtc.h"
#include "stdlib.h"

#include "pin_mux.h"
#include <stdbool.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define SLEEP_TIME_DURATION_SEC 5

#define GINT GINT0
#define LED_PORT_0 0
#define LED_PORT_1 0
#define LED_PIN_0 0
#define LED_PIN_1 3
#define PIN_WAKE 1


#define GINT GINT0
#define LED_PORT_0 0
#define LED_PORT_1 0
#define LED_PIN_0 0
#define LED_PIN_1 3

enum
{
    TEST_INI     = 0,
    TEST_TIMER_0 = TEST_INI,
    TEST_TIMER_1,
    TEST_DIO,
    TEST_RESET,
    TEST_MAX,
} wakeup_source_e;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void BOARD_SetClockForPowerMode(void);
void BOARD_InitClocks(void);
void BOARD_InitBoard(void);

static void PrintResetCause(reset_cause_t reset_cause);
static void AppMain(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

pm_wake_source_t wakeup_src[] = {
    POWER_WAKEUPSRC_WAKE_UP_TIMER0,
    POWER_WAKEUPSRC_WAKE_UP_TIMER1,
    POWER_WAKEUPSRC_IO,
    POWER_WAKEUPSRC_SYSTEM,
};

char *pd_modes_names[] = {
    //"PM_DEEP_SLEEP",
    "PM_POWER_DOWN",
    "PM_DEEP_DOWN",
};

char *wk_source_names[] = {
    "WAKEUP_TIMER_0",
    "WAKEUP_TIMER_1",
    "WAKEUP_DIO",
    "WAKEUP_NONE",
};

const int bank_size[12] = {16, 16, 16, 16, 8, 8, 4, 4, 16, 16, 16, 16};

static volatile int interrupt_fired_status;

#define NONE_IRQ_ID 0
#define TIMER0_IRQ_ID 1
#define TIMER1_IRQ_ID 2

/*******************************************************************************
 * Code
 ******************************************************************************/

void BOARD_InitClocks(void)
{
    /* what is it for exactly */
    CLOCK_EnableAPBBridge();

    /* 32MHz clock */
    // CLOCK_EnableClock(kCLOCK_Xtal32M);      // Only required if CPU running from 32MHz crystal
    // CLOCK_EnableClock(kCLOCK_Fro32M);       // derived from 198MHz FRO

    /* 32KHz clock */
    // CLOCK_EnableClock(kCLOCK_Fro32k);         // not needed as already set up by HW
    // CLOCK_EnableClock(kCLOCK_Xtal32k);

    CLOCK_EnableClock(kCLOCK_Iocon);
    CLOCK_EnableClock(kCLOCK_InputMux);

    /* Enable GPIO for LED controls */
    CLOCK_EnableClock(kCLOCK_Gpio0);

    // CLOCK_EnableClock(kCLOCK_Fro48M);
    // CLOCK_EnableClock(kCLOCK_Rtc);

    /* INMUX and IOCON are used by many apps, enable both INMUX and IOCON clock bits here. */
    CLOCK_AttachClk(kOSC32M_to_USART_CLK);

    // Alternative setting for UART
    // CLOCK_AttachClk(FRG_CLK_to_USART_CLK);
    // CLOCK_AttachClk(kOSC32M_to_FRG_CLK);

    // CLOCK_AttachClk(kOSC32M_to_I2C_CLK);
    // CLOCK_AttachClk(kFRO48M_to_DMI_CLK);
    // CLOCK_AttachClk(kMAIN_CLK_to_DMI_CLK);
}

void BOARD_SetClockForPowerMode(void)
{
    // Comment line if some PRINTF is still done prior to this step before sleep
    CLOCK_AttachClk(kNONE_to_USART_CLK);
    // CLOCK_AttachClk(kNONE_to_FRG_CLK);

    CLOCK_DisableClock(kCLOCK_Iocon);
    CLOCK_DisableClock(kCLOCK_InputMux);
    CLOCK_DisableClock(kCLOCK_Gpio0);

    // CLOCK_DisableClock(kCLOCK_Xtal32M);     // Crystal 32MHz

    // Keep the 32K clock
    // CLOCK_DisableClock(kCLOCK_Fro32k);
    // CLOCK_DisableClock(kCLOCK_Xtal32k);

    // Do not disable APB clock - Freeze otherwise
    // CLOCK_DisableAPBBridge();
}

void BOARD_InitBoard(void)
{
    /* Security code to allow debug access */
    SYSCON->CODESECURITYPROT = 0x87654320;

    /* reset FLEXCOMM for USART - mandatory from power down 4 at least*/
    RESET_PeripheralReset(kUSART0_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kGPIO0_RST_SHIFT_RSTn);

    /* For power optimization, we need a limited clock enabling specific to this application
     * This replaces BOARD_BootClockRUN() usually called in other application    */
    BOARD_InitClocks();
    BOARD_InitDebugConsole();
    BOARD_InitPins();
}


/* For timers, we need to implement the IRQ handler to clear the interrupt and optionally the status */
void WAKE_UP_TIMER0_IRQHandler(void)
{
    // PRINTF( "-->> WAKE_UP_TIMER0_IRQHandler\n");
    interrupt_fired_status = TIMER0_IRQ_ID;

    /* clear the interrupt */
    SYSCON->WKT_INTENCLR = SYSCON_WKT_INTENCLR_WKT0_TIMEOUT_MASK;

    /* clear status */
    SYSCON->WKT_STAT = SYSCON_WKT_STAT_WKT0_TIMEOUT_MASK;
}

void WAKE_UP_TIMER1_IRQHandler(void)
{
    // PRINTF( "-->> WAKE_UP_TIMER1_IRQHandler\n");
    interrupt_fired_status = TIMER1_IRQ_ID;

    /* clear the interrupt */
    SYSCON->WKT_INTENCLR = SYSCON_WKT_INTENCLR_WKT1_TIMEOUT_MASK;

    /* clear status */
    SYSCON->WKT_STAT = SYSCON_WKT_STAT_WKT1_TIMEOUT_MASK;
}

void RTC_IRQHandler(void)
{
    // PRINTF( "-->> RTC_IRQHandler\n");

    if (RTC_GetStatusFlags(RTC) & kRTC_AlarmFlag)
    {
        RTC_ClearStatusFlags(RTC, kRTC_AlarmFlag);
    }

    if (RTC_GetStatusFlags(RTC) & kRTC_WakeupFlag)
    {
        RTC_ClearStatusFlags(RTC, kRTC_WakeupFlag);
    }
}

void gint0_callback(void)
{
    PRINTF("GINT0 event detected\r\n");
}

void update_32K_clk(void)
{
    CLOCK_EnableClock(kCLOCK_Xtal32k);
}

void update_cpu_clk(int test_id)
{
    CLOCK_EnableClock(kCLOCK_Xtal32M);

    SYSCON->MAINCLKSEL = 2;
    SystemCoreClockUpdate();
}

void stop_timers(void)
{
    /* Enable clock if not enabled */
    WTIMER_Init();

    WTIMER_StopTimer(WTIMER_TIMER0_ID);
    WTIMER_StopTimer(WTIMER_TIMER1_ID);

    /* Disable Wake timer clocks */
    WTIMER_DeInit();
}

static bool checkIrqPending()
{
    bool no_irq_pending = true;
    uint32_t pmc_wakeiocause;

    for (IRQn_Type IRQ_Type = WDT_BOD_IRQn; IRQ_Type <= BLE_WAKE_UP_TIMER_IRQn; IRQ_Type++)
    {
        bool irq_pending;
        irq_pending = NVIC_GetPendingIRQ(IRQ_Type);
        if (irq_pending)
        {
            no_irq_pending = false;
            // PRINTF("IRQ %d pending\n", IRQ_Type);
            NVIC_EnableIRQ(IRQ_Type);

            NVIC_DisableIRQ(IRQ_Type);
        }
    }

    pmc_wakeiocause = PMC->WAKEIOCAUSE;
    if (pmc_wakeiocause)
    {
        no_irq_pending = false;
        // PRINTF("WAKEIOCAUSE=%x\n", pmc_wakeiocause);
    }

    return no_irq_pending;
}

void AppInit(void)
{
#if 0
    volatile int go_on = 1;
    while (go_on) {};
#endif

    BOARD_InitBoard();
    /* Configure the optimized active voltages, set up the BODs */
    // POWER_Init();

    /* Enable LEDs. */
    GPIO_PinInit(GPIO, LED_PORT_0, LED_PIN_0, &((const gpio_pin_config_t){kGPIO_DigitalOutput, 0}));
    GPIO_PinInit(GPIO, LED_PORT_1, LED_PIN_1, &((const gpio_pin_config_t){kGPIO_DigitalOutput, 0}));

    checkIrqPending();

    // if (checkIrqPending()) PRINTF("No IRQ pending\r\n");

    /* stop timers in case some were running and the wakeup source is something else (case of Multiple wakeup source)
     * Need to be done after IRQ checking */
    stop_timers();

/* Change CPU clock to 32MHz */
#if 0
    CLOCK_EnableClock(kCLOCK_Xtal32M);
    SYSCON -> MAINCLKSEL       = 2;
    SystemCoreClockUpdate();
#endif
}

/* On Warm Start (memory held), WarmMain() function is called instead of main() */
void WarmMain(void)
{
    AppInit();
    PRINTF("\nWarm Boot!!\r\n");
    AppMain();
}

/* Cold start, memory lost */
int main(void)
{
    AppInit();
    if (POWER_GetResetCause() == PMC_RESETCAUSE_WAKEUPPWDNRESET_MASK)
        PRINTF("\r\n!!Awake!!\r\n\r\n");
    PRINTF("\n\nPower API demo V1.0\r\nUse power measurement view in MCUXpresso\r\n");

    /* ES2: move to middle ram to have enough stack for rom debug */
    BOOT_SetResumeStackPointer(0x04008000);

    AppMain();
    return 0;
}

void AppMain(void)
{
    pm_power_config_t config = {0};
    pm_power_mode_t mode;

    PrintResetCause(POWER_GetResetCause());
    PRINTF("SystemCoreClock=%d\r\n", SystemCoreClock);

    while (1)
    {
        uint32_t wake_source;
        uint32_t sleep_time;
        bool ret;

        PRINTF("Select Mode\r\n");
        for (mode = PM_POWER_DOWN; mode <= PM_DEEP_DOWN; mode++)
        {
            PRINTF("%d - %s\r\n", mode, pd_modes_names[mode]);
        }
        while (mode > PM_DEEP_DOWN)
        {
            SCANF("%d", &mode);
        }
        PRINTF("Powerdown mode %s selected\r\n", pd_modes_names[mode]);

        if ((mode != PM_DEEP_DOWN))
        {
            /* Keep All SRAM banks for warm boot */
            config.pm_config |= PM_CFG_SRAM_ALL_RETENTION;

            PRINTF("Select wakeup source\r\n");
            for (wake_source = TEST_INI; wake_source < TEST_MAX; wake_source++)
            {
                PRINTF("%d - %s\r\n", wake_source, wk_source_names[wake_source]);
            }
            while (wake_source >= TEST_MAX)
            {
                SCANF("%d", &wake_source);
            }
            PRINTF("Wakeup source %s selected\r\n", wk_source_names[wake_source]);
        }
        else
        {
            wake_source = TEST_RESET;
            PRINTF("After sleep, please press the reset button to exit deep down mode.\r\n");
        }

        if ((wake_source == TEST_TIMER_0) || (wake_source == TEST_TIMER_1))
        {
            PRINTF("Input sleep time in seconds\r\n");
            SCANF("%d", &sleep_time);
            PRINTF("%d seconds sleeping chosen\r\n", sleep_time);
        }

        config.pm_wakeup_src = wakeup_src[wake_source];

        update_32K_clk();

        PRINTF("Press any key to sleep\r\n");
        GETCHAR();
        switch (wake_source)
        {
            case TEST_DIO:
                // No polarity for Power down and deep down modes
                config.pm_wakeup_io = 1 << BOARD_SW1_GPIO_PIN;
                PRINTF("Wake up on pin %d, please press the BP1 button to wakeup.\r\n", BOARD_SW1_GPIO_PIN);
                break;

            case TEST_TIMER_0:
                WTIMER_Init();
                WTIMER_EnableInterrupts(WTIMER_TIMER0_ID);
                WTIMER_StartTimer(WTIMER_TIMER0_ID, sleep_time * 32768U);
                break;

            case TEST_TIMER_1:
                WTIMER_Init();
                WTIMER_EnableInterrupts(WTIMER_TIMER1_ID);
                WTIMER_StartTimer(WTIMER_TIMER1_ID, sleep_time * 32768U);
                break;
        }

        /* Clear interrupt fired status */
        interrupt_fired_status = NONE_IRQ_ID;

        PRINTF("Sleeping...\r\n");

        /* Set HW for power down modes. Note : this could be done selectively depending on the power down mode */
        switch (mode)
        {
            case PM_POWER_DOWN:
            case PM_DEEP_DOWN:
                // BOARD_SetPinsForPowerMode();
                break;
        }

        __disable_irq();

        ret = POWER_EnterPowerMode(mode, &config);

        if (ret == false)
        {
            PRINTF("\r\n!!ERROR WHEN GOING TO SLEEP!!\r\n\r\n");
            __enable_irq();

            if (checkIrqPending())
                PRINTF("No IRQ pending\n");
            // CheckTestResults(false, true);
            // PrintTestStatus( test_id, true);
            continue;
        }

        /* Restore for DEEP_SLEEP only */
        FLASH_Init(FLASH);
        /* TODO : from here DEEP SLEEP 1 only */
        BOARD_InitClocks();
        BOARD_InitDebugConsole();

        /* Enable LEDs again. */
        GPIO_PinInit(GPIO, LED_PORT_0, LED_PIN_0, &((const gpio_pin_config_t){kGPIO_DigitalOutput, 0}));
        GPIO_PinInit(
            GPIO, LED_PORT_1, LED_PIN_1,
            &((const gpio_pin_config_t){kGPIO_DigitalOutput,
                                        0})); // Switch ON the LED0 too, it could have be turned OFF in power down mode

        // PRINTF("\r\n!!Awake!!\r\n\r\n");
        // PRINTF("PDRUNCFG=0x%x\n", PMC->PDRUNCFG);

        __enable_irq();

        switch (interrupt_fired_status)
        {
            case 0:
                // PRINTF("No IRQ fired !!\r\n");
                break;
            case TIMER0_IRQ_ID:
                PRINTF("WAKE_UP_TIMER0 fired !!\r\n");
                break;
            case TIMER1_IRQ_ID:
                PRINTF("WAKE_UP_TIMER1 fired !!\r\n");
                break;
            default:
                PRINTF("ERROR : Unknow IRQ Id !!\r\n");
                break;
        }

        // if (checkIrqPending()) PRINTF("No IRQ pending\n");
        // PrintResetCause(POWER_GetResetCause());
    }
}

static void PrintResetCause(reset_cause_t reset_cause)
{
    uint32_t reset_cause_cnt = 0; /* Reset causes counter */

    PRINTF("Reset Cause : ");
    if (reset_cause == 0)
    {
        PRINTF("NO RESET (CPU CONTEXT PRESERVED)\r\n");
        return;
    }

    if (reset_cause & PMC_RESETCAUSE_POR_MASK)
    {
        reset_cause_cnt++;
        PRINTF("   POWER ON RESET\r\n");
    }

    if (reset_cause & PMC_RESETCAUSE_PADRESET_MASK)
    {
        reset_cause_cnt++;
        PRINTF("   HARDWARE RESET (pin RSTN)\r\n");
    }

    if (reset_cause & PMC_RESETCAUSE_BODRESET_MASK)
    {
        reset_cause_cnt++;
        PRINTF("   BROWN OUT DETECTORS\r\n");
    }

    if (reset_cause & PMC_RESETCAUSE_SYSTEMRESET_MASK)
    {
        reset_cause_cnt++;
        PRINTF("   SYSTEM RESET REQUEST (from CPU)\r\n");
    }

    if (reset_cause & PMC_RESETCAUSE_WDTRESET_MASK)
    {
        reset_cause_cnt++;
        PRINTF("   WATCHDOG TIMER\r\n");
    }

    if (reset_cause & PMC_RESETCAUSE_WAKEUPIORESET_MASK)
    {
        reset_cause_cnt++;
        PRINTF("   WAKE UP I/O (from DEEP POWER DOWN)\r\n");
    }

    if (reset_cause & PMC_RESETCAUSE_WAKEUPPWDNRESET_MASK)
    {
        reset_cause_cnt++;
        PRINTF("   POWER DOWN MODE\r\n");
    }

    if (reset_cause_cnt > 1)
    {
        PRINTF("   :: WARNING :: multiple reset causes detected.\r\n");
    }
}
