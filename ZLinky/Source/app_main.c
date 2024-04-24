/*****************************************************************************
 *
 * MODULE:             JN-AN-1243
 *
 * COMPONENT:          app_main.c
 *
 * DESCRIPTION:
 *
 *****************************************************************************
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
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <stdint.h>
#include "jendefs.h"
#include "app.h"
#include "fsl_wwdt.h"
#include "fsl_iocon.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "ZQueue.h"
#include "ZTimer.h"
#include "portmacro.h"
#include "zps_apl_af.h"
#include "mac_vs_sap.h"
#include "dbg.h"
#include "app_main.h"
#include "app_buttons.h"
#include "app_events.h"
#include "app_zcl_task.h"
#include "PDM.h"
#include "app_router_node.h"
#ifdef APP_NTAG_NWK
#include "app_ntag.h"
#endif

#ifdef APP_LOW_POWER_API
#include "PWR_interface.h"
#else
#include "pwrm.h"
#endif

#include "app_leds.h"
#include "temp_sensor_drv.h"
#include "radio.h"
#include "PDM.h"
#include "PDM_IDs.h"
#include "fsl_os_abstraction.h"
#if (ZIGBEE_USE_FRAMEWORK != 0)
#include "SecLib.h"
#include "RNG_Interface.h"
#include "MemManager.h"
#include "TimersManager.h"
#endif

#include "app_uartlinky.h"
#include "app_blink_led.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef DEBUG_APP
#define TRACE_APP   FALSE
#else
#define TRACE_APP   TRUE
#endif

#define APP_QUEUE_SIZE               8
#define BDB_QUEUE_SIZE               2

#define APP_NUM_STD_TMRS             3

#if (defined APP_NTAG_NWK)
#define APP_NUM_NCI_TMRS            1 /* NTAG: Added timer */
#else
#define APP_NUM_NCI_TMRS            0
#endif

#if (defined APP_NTAG_NWK)
#define TIMER_QUEUE_SIZE             9
#else
#define TIMER_QUEUE_SIZE             8
#endif
#define MLME_QUEQUE_SIZE             8
#define MCPS_QUEUE_SIZE             16
#define MCPS_DCFM_QUEUE_SIZE         8

#define WDT_CLK_FREQ CLOCK_GetFreq(kCLOCK_WdtOsc)

PUBLIC uint8 u8TimerPowerOn;

#ifdef CLD_GREENPOWER
    PUBLIC uint8 u8GPTimerTick;
    #define APP_NUM_GP_TMRS             1
    #define GP_TIMER_QUEUE_SIZE         2
#else
    #define APP_NUM_GP_TMRS             0
#endif

#define APP_ZTIMER_STORAGE           ( APP_NUM_STD_TMRS + APP_NUM_NCI_TMRS + APP_NUM_GP_TMRS)

#define APP_WATCHDOG_STACK_DUMP FALSE

/* Interrupt priorities */
#define APP_BASE_INTERRUPT_PRIORITY (5)
#define APP_WATCHDOG_PRIOIRTY       (1)

/* Temperature ADC defines */
#define APP_ADC_BASE                       ADC0
#define APP_ADC_TEMPERATURE_SENSOR_CHANNEL 7U
#define APP_ADC_TEMPERATURE_DELAY_US       300
#define APP_ADC_TEMPERATURE_SAMPLES        8
#define TRACE_MAIN_RADIO                   TRUE
#define RADIO_TEMP_UPDATE                  TRUE

#define RX_QUEUE_SIZE                8

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
PUBLIC uint8 u8TimerButtonScan;
PUBLIC uint8 u8TimerZCL;
PUBLIC uint8 u8TimerLinky;
PUBLIC uint8 u8TimerBlink;
PUBLIC uint8 u8TimerLedON;
PUBLIC uint8 u8TimerAwake;


#if (defined APP_NTAG_NWK)
PUBLIC uint8 u8TimerNtag;
#endif

PUBLIC uint8 u8TimerPowerOn;

PUBLIC tszQueue APP_msgBdbEvents;
PUBLIC tszQueue APP_msgAppEvents;
PUBLIC tszQueue APP_msgSerialRx;

uint8  au8AtRxBuffer [ RX_QUEUE_SIZE ];
PUBLIC uint8 u8ModeLinky=0;
PUBLIC uint8 u8OldStatusLinky=0;
PUBLIC zuint48 u48StartTuyaTotalConsumption=0;

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE ZTIMER_tsTimer asTimers[APP_ZTIMER_STORAGE + BDB_ZTIMER_STORAGE];
#if (ZIGBEE_USE_FRAMEWORK == 0)
PRIVATE APP_tsEvent         asAppEvent[APP_QUEUE_SIZE];
PRIVATE BDB_tsZpsAfEvent    asBdbEvent[BDB_QUEUE_SIZE];
PRIVATE MAC_tsMcpsVsDcfmInd asMacMcpsDcfmInd[MCPS_QUEUE_SIZE];
PRIVATE MAC_tsMlmeVsDcfmInd asMacMlmeVsDcfmInd[MLME_QUEQUE_SIZE];
PRIVATE zps_tsTimeEvent     asTimeEvent[TIMER_QUEUE_SIZE];
PRIVATE MAC_tsMcpsVsCfmData asMacMcpsDcfm[MCPS_DCFM_QUEUE_SIZE];
#endif

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
extern void zps_taskZPS(void);
extern void vAppMain(void);
extern void OSA_TimeInit(void);
extern const uint8_t gUseRtos_c;

PUBLIC uint8_t wdt_update_count = 0;

gpio_pin_config_t led_config = {
    kGPIO_DigitalOutput, 0,
};

#ifdef CLD_GREENPOWER
PUBLIC tszQueue APP_msgGPZCLTimerEvents;
#if (ZIGBEE_USE_FRAMEWORK == 0)
uint8 au8GPZCLEvent[ GP_TIMER_QUEUE_SIZE];
#endif
uint8 u8GPZCLTimerEvent;
#endif

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
#if APP_WATCHDOG_STACK_DUMP
extern void ExceptionUnwindStack(uint32_t * pt);
#endif
void System_IRQHandler(void)
{
#ifdef WATCHDOG_ALLOWED
    uint32_t wdtStatus = WWDT_GetStatusFlags(WWDT);
    /* The chip should reset before this happens. For this interrupt to occur,
     * it means that the WD timeout flag has been set but the reset has not occurred  */
    if (wdtStatus & kWWDT_TimeoutFlag)
    {
        /* A watchdog feed didn't occur prior to window timeout */
        /* Stop WDT */
        WWDT_Disable(WWDT);
        WWDT_ClearStatusFlags(WWDT, kWWDT_TimeoutFlag);
        DBG_vPrintf(TRUE, "Watchdog timeout flag\r\n");
#if APP_WATCHDOG_STACK_DUMP
        /* 0x4015f28  0x32fc2    (Placed on stack by stack dump function)
         * 0x4015f2c  0xfffffff9 (Placed on stack by stack dump function)
         * 0x4015f30  0x0        R0
         * 0x4015f34  0x114ed    R1
         * 0x4015f38  0x0        R2
         * 0x4015f3c  0x40082100 R3
         * 0x4015f40  0x0        R12
         * 0x4015f44  0xe4d      Link Register (R14) (return address at time of exception)
         * 0x4015f48  0xe4c      Return address (PC at time of exception)
         * 0x4015f4c  0x41000000 PSR */
        ExceptionUnwindStack((uint32_t *) __get_MSP());
#endif
    }

    /* Handle warning interrupt */
    if (wdtStatus & kWWDT_WarningFlag)
    {
        /* A watchdog feed didn't occur prior to warning timeout */
        WWDT_ClearStatusFlags(WWDT, kWWDT_WarningFlag);
#if APP_WATCHDOG_STACK_DUMP
        if (wdt_update_count < 7)
#else
        if (wdt_update_count < 8)
#endif
        {
            /* Feed only for the first 8 warnings then allow for a WDT reset to occur */
            wdt_update_count++;
            WWDT_Refresh(WWDT);
            DBG_vPrintf(TRUE,"Watchdog warning flag %d\r\n", wdt_update_count);
        }
#if APP_WATCHDOG_STACK_DUMP
        else
        {
             DBG_vPrintf(TRUE,"Watchdog last warning %d\r\n", wdt_update_count);
             /* 0x4015f28  0x32fc2    (Placed on stack by stack dump function)
              * 0x4015f2c  0xfffffff9 (Placed on stack by stack dump function)
              * 0x4015f30  0x0        R0
              * 0x4015f34  0x114ed    R1
              * 0x4015f38  0x0        R2
              * 0x4015f3c  0x40082100 R3
              * 0x4015f40  0x0        R12
              * 0x4015f44  0xe4d      Link Register (R14) (return address at time of exception)
              * 0x4015f48  0xe4c      Return address (PC at time of exception)
              * 0x4015f4c  0x41000000 PSR */
             ExceptionUnwindStack((uint32_t *) __get_MSP());
        }
#endif
    }
#endif
}

/****************************************************************************
 *
 * NAME: main_task
 *
 * DESCRIPTION:
 * Main  execution loop
 *
 * RETURNS:
 * Never
 *
 ****************************************************************************/
void main_task (uint32_t parameter)
{
    /* e.g. osaEventFlags_t ev; */
    static uint8_t initialized = FALSE;

    if(!initialized)
    {
        /* place initialization code here... */
        /*myEventId = OSA_EventCreate(TRUE);*/
        initialized = TRUE;
#if (ZIGBEE_USE_FRAMEWORK != 0)
        SecLib_Init();
		if(gRngSuccess_d != RNG_Init())
		{
			DBG_vPrintf(TRUE,"Failed to init RNG\n");
		}
        MEM_Init();
        TMR_Init();
#endif
        vAppMain();
    }
    while(1)
    {
         /* place event handler code here... */
        DBG_vPrintf(FALSE, "APP: Entering zps_taskZPS\n");
        zps_taskZPS();

        DBG_vPrintf(FALSE, "APP: Entering bdb_taskBDB\n");
        bdb_taskBDB();

        DBG_vPrintf(FALSE, "APP: Entering ZTIMER_vTask\n");
        ZTIMER_vTask();

        DBG_vPrintf(FALSE, "APP: Entering APP_taskRouter\n");
        APP_taskRouter();


#ifdef WATCHDOG_ALLOWED
        /* Re-load the watch-dog timer. Execution must return through the idle
         * task before the CPU is suspended by the power manager. This ensures
         * that at least one task / ISR has executed with in the watchdog period
         * otherwise the system will be reset.
         */
        WWDT_Refresh(WWDT);
        wdt_update_count = 0;
#endif
        /*
         * suspends CPU operation when the system is idle or puts the device to
         * sleep if there are no activities in progress
         */
#ifdef APP_LOW_POWER_API
        (void) PWR_EnterLowPower();
#else
        PWRM_vManagePower();
#endif
        if(!gUseRtos_c)
        {
            break;
        }
    }
}

/****************************************************************************
 *
 * NAME: APP_vSetUpHardware
 *
 * DESCRIPTION:
 * Set up interrupts
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void APP_vSetUpHardware(void)
{
    wwdt_config_t config;
    uint32_t wdtFreq;
    /* Enable DMA access to RAM (assuming default configuration and MAC
     * buffers not in first block of RAM) */
    *(volatile uint32 *)0x40001000 = 0xE000F733;
    *(volatile uint32 *)0x40001004 = 0x109;
    /* Board pin, clock, debug console init */
    BOARD_InitHardware();

    /* Enable ASYNC bus access */
    CLOCK_EnableAPBBridge();

    /* Start crystal */
    CLOCK_EnableClock(kCLOCK_Xtal32M);
    CLOCK_EnableClock(kCLOCK_Xtal32k);

    /* Initialise output LED GPIOs */
    GPIO_PinInit(GPIO, APP_BOARD_GPIO_PORT, APP_BASE_BOARD_LED1_PIN, &led_config);

#ifdef WATCHDOG_ALLOWED
    /* The WDT divides the input frequency into it by 4 */
    wdtFreq = WDT_CLK_FREQ/4 ;
    NVIC_EnableIRQ(WDT_BOD_IRQn);
    WWDT_GetDefaultConfig(&config);
    /* Replace default config values where required */
    /*
     * Set watchdog feed time constant to approximately 1s - 8 warnings
     * Set watchdog warning time to 512 ticks after feed time constant
     * Set watchdog window time to 1s
     */
    config.timeoutValue = wdtFreq * 1;
    config.warningValue = 512;
    config.windowValue = wdtFreq * 1;
    /* Configure WWDT to reset on timeout */
    config.enableWatchdogReset = true;
    config.clockFreq_Hz = WDT_CLK_FREQ;

    WWDT_Init(WWDT, &config);
    /* First feed starts the watchdog */
    WWDT_Refresh(WWDT);
#endif

    /* Initialise interrupt priorities */
    int   iAppInterrupt;
    for (iAppInterrupt = NotAvail_IRQn; iAppInterrupt < BLE_WAKE_UP_TIMER_IRQn; iAppInterrupt ++)
    {
        NVIC_SetPriority(iAppInterrupt, APP_BASE_INTERRUPT_PRIORITY);
    }
    NVIC_SetPriority(WDT_BOD_IRQn, APP_WATCHDOG_PRIOIRTY);

    SYSCON -> MAINCLKSEL       = 3;  /* 32 M FRO */
    SystemCoreClockUpdate();
    OSA_TimeInit();
}


/****************************************************************************
 *
 * NAME: APP_vInitResources
 *
 * DESCRIPTION:
 * Initialise resources (timers, queue's etc)
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void APP_vInitResources(void)
{
    /* Initialise the Z timer module */
    ZTIMER_eInit(asTimers, sizeof(asTimers) / sizeof(ZTIMER_tsTimer));

    /* Create Z timers */
    ZTIMER_eOpen(&u8TimerPowerOn,    APP_cbTimerPowerCount, NULL, ZTIMER_FLAG_PREVENT_SLEEP);
    ZTIMER_eOpen(&u8TimerButtonScan, APP_cbTimerButtonScan, NULL, ZTIMER_FLAG_PREVENT_SLEEP);
    ZTIMER_eOpen(&u8TimerZCL,        APP_cbTimerZclTick,    NULL, ZTIMER_FLAG_PREVENT_SLEEP);
    ZTIMER_eOpen(&u8TimerLinky,      APP_cbTimerLinky,      NULL, ZTIMER_FLAG_PREVENT_SLEEP);
    ZTIMER_eOpen(&u8TimerBlink,      vAPP_cbBlinkLED,      NULL, ZTIMER_FLAG_PREVENT_SLEEP);
    ZTIMER_eOpen(&u8TimerLedON,      vAPP_cbLedON,         NULL, ZTIMER_FLAG_PREVENT_SLEEP);
    ZTIMER_eOpen(&u8TimerAwake,      vAPP_cbTimerAwake,      NULL, ZTIMER_FLAG_PREVENT_SLEEP);



#if (defined APP_NTAG_NWK)
    ZTIMER_eOpen(&u8TimerNtag,       APP_cbNtagTimer,       NULL, ZTIMER_FLAG_PREVENT_SLEEP);
#endif
#ifdef CLD_GREENPOWER
    ZTIMER_eOpen(&u8GPTimerTick,     APP_cbTimerGPZclTick,  NULL, ZTIMER_FLAG_PREVENT_SLEEP);
#endif

    /* Create all the queues */
    #if (ZIGBEE_USE_FRAMEWORK == 0)
        ZQ_vQueueCreate(&APP_msgAppEvents,        APP_QUEUE_SIZE,       sizeof(APP_tsEvent),         (uint8*)asAppEvent);
        ZQ_vQueueCreate(&APP_msgBdbEvents,        BDB_QUEUE_SIZE,       sizeof(BDB_tsZpsAfEvent),    (uint8*)asBdbEvent);
        ZQ_vQueueCreate(&zps_msgMlmeDcfmInd,      MLME_QUEQUE_SIZE,     sizeof(MAC_tsMlmeVsDcfmInd), (uint8*)asMacMlmeVsDcfmInd);
        ZQ_vQueueCreate(&zps_msgMcpsDcfmInd,      MCPS_QUEUE_SIZE,      sizeof(MAC_tsMcpsVsDcfmInd), (uint8*)asMacMcpsDcfmInd);
        ZQ_vQueueCreate(&zps_msgMcpsDcfm,         MCPS_DCFM_QUEUE_SIZE, sizeof(MAC_tsMcpsVsCfmData), (uint8*)asMacMcpsDcfm);
        ZQ_vQueueCreate(&zps_TimeEvents,          TIMER_QUEUE_SIZE,     sizeof(zps_tsTimeEvent),     (uint8*)asTimeEvent);
        #ifdef CLD_GREENPOWER
        ZQ_vQueueCreate(&APP_msgGPZCLTimerEvents, GP_TIMER_QUEUE_SIZE,  sizeof(uint8),               (uint8*)au8GPZCLEvent);
        #endif
    #else
        ZQ_vQueueCreate(&APP_msgAppEvents,        APP_QUEUE_SIZE,       sizeof(APP_tsEvent),         NULL);
        ZQ_vQueueCreate(&APP_msgBdbEvents,        BDB_QUEUE_SIZE,       sizeof(BDB_tsZpsAfEvent),    NULL);
        ZQ_vQueueCreate(&zps_msgMlmeDcfmInd,      MLME_QUEQUE_SIZE,     sizeof(MAC_tsMlmeVsDcfmInd), NULL);
        ZQ_vQueueCreate(&zps_msgMcpsDcfmInd,      MCPS_QUEUE_SIZE,      sizeof(MAC_tsMcpsVsDcfmInd), NULL);
        ZQ_vQueueCreate(&zps_msgMcpsDcfm,         MCPS_DCFM_QUEUE_SIZE, sizeof(MAC_tsMcpsVsCfmData), NULL);
        ZQ_vQueueCreate(&zps_TimeEvents,          TIMER_QUEUE_SIZE,     sizeof(zps_tsTimeEvent),     NULL);
        #ifdef CLD_GREENPOWER
        ZQ_vQueueCreate(&APP_msgGPZCLTimerEvents, GP_TIMER_QUEUE_SIZE,  sizeof(uint8),               NULL);
        #endif
    #endif
        ZQ_vQueueCreate ( &APP_msgSerialRx,       RX_QUEUE_SIZE,          sizeof ( uint8 ),         (uint8*)au8AtRxBuffer );
}

/****************************************************************************
 *
 * NAME: APP_vRadioTempUpdate
 *
 * DESCRIPTION:
 * Callback function from radio driver requesting write of calibration
 * settings to PDM
 *
 * RETURNS:
 * TRUE  - successful
 * FALSE - failed
 *
 ****************************************************************************/
PUBLIC void APP_vRadioTempUpdate(bool_t bLoadCalibration)
{
#if RADIO_TEMP_UPDATE
    bool_t bStatus = TRUE;

    /* Debug */
    DBG_vPrintf(TRACE_MAIN_RADIO, "\r\n%d: APP_vRadioTempUpdate(%d)",OSA_TimeGetMsec(), bLoadCalibration);
    /* Need to load calibration data ? */
    if (bLoadCalibration)
    {
        /* Read temperature and adc calibration parameter once. Store ADC calibration values into ADC register */
        bStatus = load_calibration_param_from_flash(APP_ADC_BASE);
        /* Debug */
        DBG_vPrintf(TRACE_MAIN_RADIO, "\r\n%d: load_calibration_param_from_flash() = %d", OSA_TimeGetMsec(), bStatus);
    }
    /* OK to read temperature */
    if (bStatus)
    {
        /* Temp in 128th degree */
        int32 i32Temp128th;
        /* Read temperature (in 128th degree) */
        bStatus = get_temperature(APP_ADC_BASE, APP_ADC_TEMPERATURE_SENSOR_CHANNEL, APP_ADC_TEMPERATURE_DELAY_US, APP_ADC_TEMPERATURE_SAMPLES, &i32Temp128th);
        /* Debug */
        DBG_vPrintf(TRACE_MAIN_RADIO, "\r\n%d: get_temperature() = %d", OSA_TimeGetMsec(), bStatus);
        /* Success ? */
        if (bStatus)
        {
            /* Temp in half degree */
            int16 i16Temp2th = (int16)(i32Temp128th / 64);
            /* Debug */
            DBG_vPrintf(TRACE_MAIN_RADIO, ", i32Temp128th = %d, i16Temp2th = %d", i32Temp128th, i16Temp2th);
            /* Pass to radio driver */
            vRadio_Temp_Update(i16Temp2th);
        }
    }
#endif
}

#if RADIO_TEMP_UPDATE
/****************************************************************************
 *
 * NAME: bRadioCB_WriteNVM
 *
 * DESCRIPTION:
 * Callback function from radio driver requesting write of calibration
 * settings to PDM
 *
 * RETURNS:
 * TRUE  - successful
 * FALSE - failed
 *
 ****************************************************************************/
PUBLIC bool_t bRadioCB_WriteNVM(uint8 *pu8DataBlock, uint16 u16DataLength)
{
    bool_t bReturn = FALSE;
    PDM_teStatus eStatus;

    eStatus = PDM_eSaveRecordData(PDM_ID_RADIO_SETTINGS, (void *)pu8DataBlock, u16DataLength);

    if (PDM_E_STATUS_OK == eStatus)
    {
        bReturn = TRUE;
    }

    /* Debug */
    DBG_vPrintf(TRACE_MAIN_RADIO, "\r\n%d: bRadioCB_WriteNVM(%d) = %d, eStatus = %d", OSA_TimeGetMsec(), u16DataLength, bReturn, eStatus);

    return bReturn;
}

/****************************************************************************
 *
 * NAME: u16RadioCB_ReadNVM
 *
 * DESCRIPTION:
 * Callback function from radio driver requesting read of calibration
 * settings from PDM
 *
 * RETURNS:
 * Number of bytes read
 *
 ****************************************************************************/
PUBLIC uint16 u16RadioCB_ReadNVM(uint8 *pu8DataBlock, uint16 u16DataLength)
{
    PDM_teStatus eStatus;
    uint16 u16BytesRead = 0;

    eStatus = PDM_eReadDataFromRecord(PDM_ID_RADIO_SETTINGS, (void *)pu8DataBlock, u16DataLength, &u16BytesRead);

    /* Debug */
    DBG_vPrintf(TRACE_MAIN_RADIO, "\r\n%d: u16RadioCB_ReadNVM(%d) = %d, eStatus = %d",OSA_TimeGetMsec(), u16DataLength, u16BytesRead, eStatus);

    return u16BytesRead;
}
#endif
/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
