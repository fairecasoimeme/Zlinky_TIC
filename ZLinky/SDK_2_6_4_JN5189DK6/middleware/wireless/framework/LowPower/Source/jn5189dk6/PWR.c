/*! *********************************************************************************
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2017, 2019-2020 NXP
* All rights reserved.
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/*****************************************************************************
 *                               INCLUDED HEADERS                            *
 *---------------------------------------------------------------------------*
 * Add to this section all the headers that this module needs to include.    *
 *---------------------------------------------------------------------------*
 *****************************************************************************/
#include "EmbeddedTypes.h"
#include "PWR_Configuration.h"
#include "PWRLib.h"
#include "PWR_Interface.h"
#include "TimersManager.h"
#include "fsl_device_registers.h"
#include "fsl_os_abstraction.h"
#include "board.h"
#include "board_utility.h"

#if gSupportBle
// TODO: Register pointers from application to call BLE/ZB functions to remove
//       the 2 following headers
#include "ble_general.h"
#include "controller_interface.h"
#include "fsl_xcvr.h"
#endif
#include "fsl_wtimer.h"
#include "fsl_fmeas.h"
#include "fsl_inputmux.h"
#include "fsl_power.h"
#include "GPIO_Adapter.h"
#include "fsl_rtc.h"
#include "Panic.h"
#if defined gLoggingActive_d && gLoggingActive_d
#include "dbg_logging.h"
#endif

#define RTOS_TICKLESS 0
#if defined USE_RTOS && (USE_RTOS != 0)
  #ifdef FSL_RTOS_FREE_RTOS
  #include "FreeRTOSConfig.h"
  #include "FreeRTOS.h"
  #include "portmacro.h"
  #undef RTOS_TICKLESS
  #if (defined configUSE_TICKLESS_IDLE && (configUSE_TICKLESS_IDLE != 0) && (cPWR_FullPowerDownMode))
      #define RTOS_TICKLESS 1
  #else
      #define RTOS_TICKLESS 0
      //  #define USE_WTIMER /* For now use RTC Wake counter */
  #endif
  #else /* not FreeRTOS */
  #error "RTOS unsupported"
  #endif
#endif

#ifndef LpIoSet
#define LpIoSet(x, y)
#endif

#if (cPWR_FullPowerDownMode)
extern void hardware_init(void);
#endif

extern void ResetMCU(void);

/*****************************************************************************
 *                             PRIVATE MACROS                                *
 *---------------------------------------------------------------------------*
 * Add to this section all the access macros, registers mappings, bit access *
 * macros, masks, flags etc ...                                              *
 *---------------------------------------------------------------------------*
 *****************************************************************************/
#ifndef BIT
#define BIT(x)                  (1 << (x))
#endif

#ifndef FMEAS_SYSCON
#if defined(FSL_FEATURE_FMEAS_USE_ASYNC_SYSCON) && (FSL_FEATURE_FMEAS_USE_ASYNC_SYSCON)
#define FMEAS_SYSCON ASYNC_SYSCON
#else
#define FMEAS_SYSCON SYSCON
#endif
#endif

#define SET_MPU_CTRL(x) (*(uint32_t*)0xe000ed94 = (uint32_t)(x))

#if !defined PWR_PREVENT_SLEEP_IF_LESS_TICKS
#if RTOS_TICKLESS
/* As the tickless mode would reject power down if the xExpectedIdleTime < 2 RTOS ticks, it is
 * recommended to also have a value <= 2 RTOS ticks when PWR_CheckIfDeviceCanGoToSleep check if
 * a low power timer expiration is too close
 */
#define PWR_PREVENT_SLEEP_IF_LESS_TICKS        MILLISECONDS_TO_TICKS32K(2*portTICK_PERIOD_MS)
#elif !defined(DUAL_MODE_APP) && gSupportBle
/* Might need to be redefined to a smaller value : 10ms is enough (1000 ticks is ~30.5 ms)*/
#define PWR_PREVENT_SLEEP_IF_LESS_TICKS        MILLISECONDS_TO_TICKS32K(10)
#else
/* Might need to be redefined to a smaller value : 1000 ti0cks is ~30.5 ms*/
#define PWR_PREVENT_SLEEP_IF_LESS_TICKS        (1000)
#endif
#endif

/* Tune FRO32K calibration
 * PWR_FRO32K_CAL_SCALE : Calibration period 4: 2^4*30.5us = 488us */
#define PWR_FRO32K_CAL_WAKEUP_END         0   /* Set to 1 to complete the Cal at the end of the SW wakeup     */
#define PWR_FRO32K_CAL_SCALE              6   /* ( 4 if PWR_FRO32K_CAL_WAKEUP_END==1 , 5 otherwise)           */
#define PWR_FRO32K_CAL_AVERAGE            5
#define FREQ32K_CAL_SHIFT                 4
#define PWR_FRO32K_CAL_INCOMPLETE_PRINTF  1   /* set to 1 to PRINTF when CAL is completed when we check it    */

#if gSupportBle
#define PWR_DEBUG   (0)
#else
#define PWR_DEBUG   (0)
#endif

#if PWR_DEBUG
#include "fsl_debug_console.h"
#endif


/*****************************************************************************
 *                               PUBLIC VARIABLES                            *
 *---------------------------------------------------------------------------*
 * Add to this section all the variables and constants that have global      *
 * (project) scope.                                                          *
 * These variables / constants can be accessed outside this module.          *
 * These variables / constants shall be preceded by the 'extern' keyword in  *
 * the interface header.                                                     *
 *---------------------------------------------------------------------------*
 *****************************************************************************/

/*****************************************************************************
 *                           PRIVATE FUNCTIONS PROTOTYPES                    *
 *---------------------------------------------------------------------------*
 * Add to this section all the functions prototypes that have local (file)   *
 * scope.                                                                    *
 * These functions cannot be accessed outside this module.                   *
 * These declarations shall be preceded by the 'static' keyword.             *
 *---------------------------------------------------------------------------*
 *****************************************************************************/
#if (cPWR_FullPowerDownMode)
static void vHandleSleepModes(uint32_t u32Mode);
#if cPWR_EnableDeepSleepMode_4
static void vHandleDeepDown(uint32_t u32Mode);
#endif

void PWR_SetWakeUpConfig(uint32_t set_msk, uint32_t clr_msk);
uint32_t PWR_GetWakeUpConfig(void);
void vSetWakeUpIoConfig(void);
uint64_t u64GetWakeupSourceConfig(uint32_t u32Mode);
#endif /* (cPWR_FullPowerDownMode) */

/*****************************************************************************
 *                        PUBLIC TYPE DEFINITIONS                            *
 *---------------------------------------------------------------------------*
 * Add to this section all the data types definitions: structures, unions,    *
 * enumerations, typedefs ...                                                *
 *---------------------------------------------------------------------------*
 *****************************************************************************/
typedef enum
{
    PWR_Run = 77,
    PWR_Sleep,
    PWR_PowerDown,
    PWR_DeepDown,
    PWR_OFF,
    PWR_Reset,
} PWR_PowerState_t;

typedef void (*pfHandleDeepSleepFunc_t)(uint32_t u32Config);
typedef struct
{
    pfHandleDeepSleepFunc_t pfFunc;
    uint32_t                u32Config;
} SleepModeTable_t;

#define NB_IRQs (NUMBER_OF_INT_VECTORS-16)
typedef struct {
    uint32_t mbasepri;
    uint32_t scb_icsr;
    uint32_t scb_aircr;
    uint32_t scb_shp[12];
    uint32_t nvic_ip[NB_IRQs];
} ARM_CM4_register_t;

/*****************************************************************************
 *                               PRIVATE VARIABLES                           *
 *---------------------------------------------------------------------------*
 * Add to this section all the variables and constants that have local       *
 * (file) scope.                                                             *
 * Each of this declarations shall be preceded by the 'static' keyword.      *
 * These variables / constants cannot be accessed outside this module.       *
 *---------------------------------------------------------------------------*
 *****************************************************************************/
uint8_t mLPMFlag               = gAllowDeviceToSleep_c;
#if (cPWR_FullPowerDownMode)
static bool_t isInitialized           = FALSE;
static uint32_t pwr_wakeup_io         = 0;
static bool_t mPreventEnterLowPower   = FALSE;

#if gClkUseFro32K && !gPWR_UseAlgoTimeBaseDriftCompensate
/* When using FRO32K without compensation, keep active AO LDO voltage in sleep */
#define PWR_CFG_AO_LDO_BUILD (PWR_CFG_MAINTAIN_AO_LDO)
#else
/* Allow more power saving by decreasing the Always ON LDO voltage in power down mode */
#define PWR_CFG_AO_LDO_BUILD (PWR_CFG_REDUCE_AO_LDO)
#endif

const SleepModeTable_t maHandleDeepSleepTable[] =
{
                                                                                    /* Mode | Osc | RAM | AO LDO | PWRM mapping              */
    {vHandleSleepModes, PWR_CFG_AO_LDO_BUILD | PWR_CFG_OSC_ON | PWR_CFG_RAM_ON},    /* 1    | On  | On  | Build  | E_AHI_SLEEP_OSCON_RAMON   */
    {vHandleSleepModes, PWR_CFG_AO_LDO_BUILD | PWR_CFG_OSC_ON | PWR_CFG_RAM_OFF},   /* 2    | On  | Off | Build  | E_AHI_SLEEP_OSCON_RAMOFF  */
    {vHandleSleepModes, PWR_CFG_REDUCE_AO_LDO | PWR_CFG_OSC_ON | PWR_CFG_RAM_ON},   /* 3    | On  | On  | Reduce | -                         */
#if cPWR_EnableDeepSleepMode_4
    {vHandleDeepDown,   PWR_CFG_REDUCE_AO_LDO | PWR_CFG_OSC_OFF | PWR_CFG_RAM_OFF}, /* 4    | Off | Off | Reduce | -                         */
#else
    {(pfHandleDeepSleepFunc_t)0, 0},
#endif
    {vHandleSleepModes, PWR_CFG_REDUCE_AO_LDO | PWR_CFG_OSC_OFF | PWR_CFG_RAM_ON},  /* 5    | Off | On  | Reduce | E_AHI_SLEEP_OSCOFF_RAMON  */
    {vHandleSleepModes, PWR_CFG_REDUCE_AO_LDO | PWR_CFG_OSC_OFF | PWR_CFG_RAM_OFF}, /* 6    | Off | Off | Reduce | E_AHI_SLEEP_OSCOFF_RAMOFF */
};
static ARM_CM4_register_t  cm4_misc_regs;
static pfPWRCallBack_t gpfPWR_LowPowerEnterCb;
static pfPWRCallBack_t gpfPWR_LowPowerExitCb;

static PWR_clock_32k_hk_t mHk32k = {
    .freq32k          = 32768,
    .freq32k_16thHz  = (32768 << FREQ32K_CAL_SHIFT), /* expressed in 16th of Hz: (1<<19) */
    .ppm_32k          = -0x6000,                      /* initialization of the 32k clock calibration in part per miliion */
};

static pm_power_config_t      pwrm_sleep_config;
static uint32_t               pwrm_force_retention;

/* Flag to say if BLE is active. Managed by application, but default depends
 * on build configuration.
 */
#if gSupportBle
static volatile bool_t        bBLE_Active = TRUE;
#else
static volatile bool_t        bBLE_Active = FALSE;
#endif

/*****************************************************************************
 *                             PRIVATE FUNCTIONS                             *
 *---------------------------------------------------------------------------*
 * Add to this section all the functions that have local (file) scope.       *
 * These functions cannot be accessed outside this module.                   *
 * These definitions shall be preceded by the 'static' keyword.              *
 *---------------------------------------------------------------------------*
 *****************************************************************************/
static void Save_CM4_registers(ARM_CM4_register_t * reg_store)
{
#if defined(USE_RTOS) && (USE_RTOS)
    reg_store->mbasepri = __get_BASEPRI();
    for(int i=0;i<12;i++)
    {
        reg_store->scb_shp[i] = SCB->SHP[i];
    }
    reg_store->scb_icsr = SCB->ICSR;
    reg_store->scb_aircr = SCB->AIRCR;
#endif
    for(int i=0;i<NB_IRQs;i++)
    {
        reg_store->nvic_ip[i] = NVIC->IP[i];
    }
    /* clear PendSysTick bit in ICSR, if set */
    SCB->ICSR |= SCB_ICSR_PENDSVCLR_Msk;

}

static void Restore_CM4_registers(ARM_CM4_register_t * reg_store)
{
#if (defined(USE_RTOS) && (USE_RTOS))
    __set_BASEPRI(reg_store->mbasepri);
    __DSB();
    __ISB();
    for(int i=0;i<12;i++)
    {
        SCB->SHP[i] = reg_store->scb_shp[i];
    }
    SCB->ICSR = reg_store->scb_icsr;
    SCB->AIRCR = reg_store->scb_aircr;
#endif
    for(int i=0;i<NB_IRQs;i++)
    {
        NVIC->IP[i] = reg_store->nvic_ip[i];
    }
}

#if PWR_DEBUG
#if defined DBG_PWR && (DBG_PWR > 0)

/* The macros and BLE register access functions below are copied from reg_blecore.h */

#define BLE_BASE_ADDR 0x400A0000

#define BLE_DEEPSLCNTL_OFFSET 0x00000030

#define REG_BLE_RD(offs)             (*(volatile uint32_t *)(BLE_BASE_ADDR+(offs)))

static inline uint32_t ble_deepslcntl_get(void)
{
    return REG_BLE_RD(BLE_DEEPSLCNTL_OFFSET);
}

#define BLE_DEEPSLSTAT_OFFSET 0x00000038

static inline uint32_t ble_deepslstat_get(void)
{
    return REG_BLE_RD(BLE_DEEPSLSTAT_OFFSET);
}

#define BLE_DEEPSLWKUP_OFFSET 0x00000034

static inline uint32_t ble_deepslwkup_get(void)
{
    return REG_BLE_RD(BLE_DEEPSLWKUP_OFFSET);
}

#define BLE_EXTWKUPDSB_POS            31
#define BLE_DEEP_SLEEP_STAT_POS       15
#define BLE_SOFT_WAKEUP_REQ_POS       4
#define BLE_DEEP_SLEEP_CORR_EN_POS    3
#define BLE_DEEP_SLEEP_ON_POS         2
#define BLE_RADIO_SLEEP_EN_POS        1
#define BLE_OSC_SLEEP_EN_POS          0

#define BLE_ENBPRESET_OFFSET 0x0000003C

static inline uint32_t ble_enbpreset_get(void)
{
    return REG_BLE_RD(BLE_ENBPRESET_OFFSET);
}


#if CHECK_PENDING_IRQ
static bool checkIrqPending() __keep_unused
{
    bool no_irq_pending = true;
    uint32_t     pmc_wakeiocause;

    for (IRQn_Type IRQ_Type = WDT_BOD_IRQn; IRQ_Type<=BLE_WAKE_UP_TIMER_IRQn ; IRQ_Type++)
    {
        bool irq_pending;
        irq_pending = NVIC_GetPendingIRQ(IRQ_Type);
        if (irq_pending)
        {
            no_irq_pending = false;
            PWR_DBG_LOG("IRQ=%d Pending", IRQ_Type);

            //NVIC_EnableIRQ(IRQ_Type);
            //NVIC_DisableIRQ(IRQ_Type);
        }
    }

    if ( no_irq_pending )
    {
        PWR_DBG_LOG("No IRQ Pending");
    }

    pmc_wakeiocause = PMC->WAKEIOCAUSE;
    if (pmc_wakeiocause)
    {
        no_irq_pending = false;
        PWR_DBG_LOG("WAKEIOCAUSE=%x", pmc_wakeiocause);
    }
    return no_irq_pending;
}
#endif

static void debug_low_power(void)
{
#if CHECK_PENDING_IRQ
    uint32_t val = ble_deepslcntl_get();

    uint32_t lldeepsleep = ( val & BIT(BLE_DEEP_SLEEP_STAT_POS) );

    //PWR_DBG_LOG("llDS=%d, DUR=%d, WKUP=%d", lldeepsleep, ble_deepslstat_get(), ble_deepslwkup_get());

    if ( lldeepsleep )
    {
        checkIrqPending();
    }
#else
    PWR_DBG_LOG("llDS=%d, DUR=%d, WKUP=%d", ble_deepslcntl_get(), ble_deepslstat_get(), ble_deepslwkup_get());
#endif
    /*
     * PWR_DBG_LOG("ENPB=0x%x", ble_enbpreset_get());
     * ENBPRESET is not restored yet so no use logging
     */
}
#endif
/* Default implementation of the WakeupTimer IRQ handler is weak:
 * here redefine body where an IO is lowered on entry and raised on exit
 * */
void BLE_WAKE_UP_TIMER_IRQHandler(void)
{
    /* BLE_WAKE_UP_TIMER_IRQ should have been cleared*/
    /* raise assert */
    assert(0);
}
#endif /* PWR_DEBUG */


#if gClkUseFro32K

/* suppose 32MHz crystal is running and 32K running */
void PWR_Start32KCalibration(void)
{
    INPUTMUX_Init(INPUTMUX);

    /* Setup to measure the selected target */
    INPUTMUX_AttachSignal(INPUTMUX, 1U, kINPUTMUX_Xtal32MhzToFreqmeas);
    INPUTMUX_AttachSignal(INPUTMUX, 0U, kINPUTMUX_32KhzOscToFreqmeas);

    /* Temporary fix for RFT1366 : JN518x Frequency measure does not work 32kHz
       if used for target clock */
    SYSCON->MODEMCTRL |= SYSCON_MODEMCTRL_BLE_LP_OSC32K_EN(1);

    CLOCK_EnableClock(kCLOCK_Fmeas);

    /* Start a measurement cycle and wait for it to complete. If the target
       clock is not running, the measurement cycle will remain active
       forever, so a timeout may be necessary if the target clock can stop */
    FMEAS_StartMeasureWithScale(FMEAS_SYSCON, PWR_FRO32K_CAL_SCALE);
}

// Return the result in unit of 1/(2^freq_scale)th of Hertz for higher accuracy
uint32_t PWR_Complete32KCalibration(uint8_t u8Shift)
{
    uint32_t        freqComp    = 0U;
    uint32_t        refCount    = 0U;
    uint32_t        targetCount = 0U;
    uint32_t        freqRef     = CLOCK_GetFreq(kCLOCK_Xtal32M);

    if (FMEAS_IsMeasureComplete(FMEAS_SYSCON))
    {
        /* Get computed frequency */
        FMEAS_GetCountWithScale(FMEAS_SYSCON, PWR_FRO32K_CAL_SCALE, &refCount, &targetCount);
        freqComp = (uint32_t)(((((uint64_t)freqRef)*refCount)<<u8Shift) / targetCount);

        /* Disable the clocks if disable previously */
        CLOCK_DisableClock(kCLOCK_Fmeas);
    }

    return freqComp;
}

void Update32kFrequency(uint32_t freq)
{
    if (freq != 0UL)
    {
        PWR_clock_32k_hk_t *hk = PWR_GetHk32kHandle();

        uint32_t val = hk->freq32k_16thHz;
        val = ((val << PWR_FRO32K_CAL_AVERAGE) - val + freq) >> PWR_FRO32K_CAL_AVERAGE ;
        hk->freq32k_16thHz = val;
        hk->freq32k = (val >> FREQ32K_CAL_SHIFT);

        PWR_DBG_LOG("freq32k=%d freq32k_16thHz=%d", hk->freq32k, hk->freq32k_16thHz);

    }
    else
    {
        LpIoSet(4, 1);
        LpIoSet(4, 0);
        LpIoSet(4, 1);
        LpIoSet(4, 0);

#if PWR_FRO32K_CAL_INCOMPLETE_PRINTF
        PWR_DBG_LOG("32K cal incomplete");
#endif
    }
}


#endif


static void RelinquishXtal32MCtrl(void)
{

#if (gPWR_SmartFrequencyScaling == 2)
        BOARD_CpuClockUpdate48Mz();
#else
        BOARD_CpuClockUpdate32MhzFro();
#endif

    /* Prevent disabling the 32M XO at wakeup - see artf587675
     * XTAL32MCTRL is set to 3 or 1 from POR/PD by SW, need to set back to zero    */
#if gSupportBle
    /*   1-0 does not work, do not go properly in lowpower ,  go to lowpower only when wakup
          1-1 does not work,   not wakeup,
        0-1 or 0-0 works        */

#if (gPWR_Xtal32MDeactMode > 0)
    SYSCON->XTAL32MCTRL =  SYSCON_XTAL32MCTRL_DEACTIVATE_PMC_CTRL(0)
                        | SYSCON_XTAL32MCTRL_DEACTIVATE_BLE_CTRL(0);

#if gPWR_Xtal32MSwitchOffEarlier
    /* not retained - usefull only for lowpower entry
        can be done safely because CPU running on FRO */
    ASYNC_SYSCON->XTAL32MLDOCTRL &= ~(ASYNC_SYSCON_XTAL32MLDOCTRL_ENABLE_MASK | 0x1 /* XO LDO bypass - connected directly analog */  );
    ASYNC_SYSCON->XTAL32MCTRL    &= ~(ASYNC_SYSCON_XTAL32MCTRL_XO32M_TO_MCU_ENABLE_MASK
                                    | ASYNC_SYSCON_XTAL32MCTRL_XO_STANDALONE_ENABLE_MASK
                                    | ASYNC_SYSCON_XTAL32MCTRL_XO_ENABLE_MASK);
#endif

#else // gPWR_Xtal32MDeactMode not defined or =0
    SYSCON->XTAL32MCTRL =  SYSCON_XTAL32MCTRL_DEACTIVATE_PMC_CTRL(0)
                        | SYSCON_XTAL32MCTRL_DEACTIVATE_BLE_CTRL(1);
#endif

#else
    SYSCON->XTAL32MCTRL = SYSCON_XTAL32MCTRL_DEACTIVATE_PMC_CTRL(0);
#endif
}
int error_status;
/*---------------------------------------------------------------------------
 * Name: PWR_HandleDeepSleepMode
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
static void vHandleSleepModes(uint32_t mode)
{
#if gSupportBle
    uint8_t   power_down_mode = 0xff;
#endif
    WTIMER_status_t InterruptStatus1;

    /* TODO: Note from pwrm.c is to not disable interrupts "Keep commented
        - DEEP DOWN 1 fails with 4470 Application if interrupts are masked" */
    OSA_DisableIRQGlobal();

    PWR_ClearWakeupReason();

#if gSupportBle
    if (mode & PWR_CFG_RAM_OFF)
    {
        power_down_mode = kPmPowerDown1;
    }
    else
    {
        SYSCON->MODEMCTRL |= SYSCON_MODEMCTRL_BLE_LP_OSC32K_EN_MASK;
        //PWR_DBG_LOG("BLE Sleep");

        LpIoSet(0, 0);

#if !gPWR_BleLL_EarlyEnterDsm  /* Can be skipped as done in CheckIfDeviceCanGoToSleepExt() */
        /* Check if LL still allows lowpower and go to lowpower if so */
        power_down_mode = BLE_sleep();
#endif

        LpIoSet(0, 1);
    }
    if (power_down_mode < kPmPowerDown0)
    {
        PWRLib_MCU_Enter_Sleep();
    }
    else
#endif
    {
        RelinquishXtal32MCtrl();

        error_status = 0;

#if gLoggingActive_d && 0
        static int count = 0;
        if (count ==2)
        {
            DbgLogDump(FALSE);
            count=0;
        }
        else
        {
            count ++;
        }
#endif

#if gClkUseFro32K && gClkRecalFro32K && !PWR_FRO32K_CAL_WAKEUP_END
        uint32_t freq = PWR_Complete32KCalibration(FREQ32K_CAL_SHIFT);
        Update32kFrequency(freq);
#endif

        /* Going to go to sleep */
        /* Call any registered pre-sleep callback */
        if(gpfPWR_LowPowerEnterCb != NULL)
        {
            gpfPWR_LowPowerEnterCb();
        }

       /* XCVR_Deinit(); no longer required since Radio v2053 */
        SET_MPU_CTRL(0);

        if (mode & PWR_CFG_OSC_OFF)
        {
            CLOCK_DisableClock(kCLOCK_Fro32k);
            CLOCK_DisableClock(kCLOCK_Xtal32k);
        }

        if (mode & PWR_CFG_RAM_ON)
        {
            /* Useless to save CM4 registers if RAM is not to be retained */
            Save_CM4_registers(&cm4_misc_regs);

#if gSupportBle
#ifdef PWR_DisableLateBleIsolateModem
            /* do it now if PWR_DisableLateBleIsolateModem */
            BLE_IsolateModem();
#endif
            LpIoSet(4, 0);
            LpIoSet(4, 1);

            /* Clear ble wake up IRQs that could be pending */
            NVIC_ClearPendingIRQ(BLE_WAKE_UP_TIMER_IRQn);
#endif
            /* Go to sleep */
            PWRLib_EnterPowerDownModeRamOn(mode);

            /* At this point, have woken up again */

            /* Change CPU clock to appropriate clock source to speed up initialization
             * and update Flash wait states */

            /* Initialise hardware */
            hardware_init();
            LpIoSet(1, 0);

            Restore_CM4_registers(&cm4_misc_regs); /* Restore IRQs */

            BOARD_DbgDiagEnable();

#if gClkUseFro32K && gClkRecalFro32K
            PWR_Start32KCalibration();
#endif

#if gSupportBle
            TMR_ReInit();
#endif

#if defined(PWR_DEBUG) && (PWR_DEBUG > 0)
            debug_low_power();
#endif
            LpIoSet(1, 1);

            if (gpfPWR_LowPowerExitCb != NULL)
            {
                gpfPWR_LowPowerExitCb();
            }

#if defined gLoggingActive_d && gLoggingActive_d
            /* Can not dump before gpfPWR_LowPowerExitCb() so check the error_status now to dump the trace */
            if ( error_status < 0 )
            {
                DbgLogDump(TRUE);
            }
            else if ( error_status > 0 )
            {
                DbgLogDump(FALSE);
            }
#endif

            LpIoSet(1, 0);
#if defined(gPWR_BleWakeupTimeOptimDisabled) && (gPWR_BleWakeupTimeOptimDisabled == 1)
            PWR_UpdateWakeupReason();
#endif
            LpIoSet(1, 1);

#if gClkUseFro32K && gClkRecalFro32K && PWR_FRO32K_CAL_WAKEUP_END
            uint32_t freq = PWR_Complete32KCalibration(FREQ32K_CAL_SHIFT);
            Update32kFrequency(freq);
#endif

            /* Check if WTIMER enabled and, if so, check running status */
            if (0 != (SYSCON->AHBCLKCTRLS[0] & SYSCON_AHBCLKCTRLSET0_WAKE_UP_TIMERS_CLK_SET_MASK))
            {
                InterruptStatus1 = WTIMER_GetStatusFlags(WTIMER_TIMER1_ID);

                if (   (InterruptStatus1 == WTIMER_STATUS_RUNNING)
                    || (InterruptStatus1 == WTIMER_STATUS_EXPIRED) )
                {
                    WTIMER_EnableInterrupts(WTIMER_TIMER1_ID);
                }
            }
            LpIoSet(1, 0);
        }
        else
        {
            pwrlib_pd_cfg_t pd_cfg;
            memset(&pd_cfg, 0x0, sizeof(pwrlib_pd_cfg_t));

            /* get the IO wakeup configuration */
            vSetWakeUpIoConfig();

            if (mode & PWR_CFG_MAINTAIN_AO_LDO)
            {
                pd_cfg.sleep_cfg |= PM_CFG_KEEP_AO_VOLTAGE;
            }

            pd_cfg.wakeup_io = PWR_GetWakeUpConfig();
            pd_cfg.wakeup_src = u64GetWakeupSourceConfig(mode);

            /* Mode with RAM off; will not return here afterwards */
            if (   (pd_cfg.wakeup_src & POWER_WAKEUPSRC_ANA_COMP)
                || (mode & PWR_CFG_OSC_ON)
                || ( SYSCON->RETENTIONCTRL & SYSCON_RETENTIONCTRL_IOCLAMP_MASK )
               )
            {
                /* if Analog comparator wakeup is requested or if oscillator
                   is to keep running, forbid deep down mode */
                PWRLib_EnterPowerDownModeRamOff(&pd_cfg);
            }
            else
            {
                PWRLib_EnterDeepDownMode(&pd_cfg);
            }
        }
    }

    OSA_EnableIRQGlobal();

}

#if cPWR_EnableDeepSleepMode_4
static void vHandleDeepDown(uint32_t u32Mode)
{
    PWR_EnterDeepDown();
}
#endif

/*---------------------------------------------------------------------------
 * Name: PWR_HandleDeepSleep
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
static PWR_WakeupReason_t PWR_HandleDeepSleep(void)
{
    uint8_t lpMode;
    PWR_ClearWakeupReason();

    lpMode = PWRLib_GetDeepSleepMode();

    if(lpMode)
    {
        SleepModeTable_t const *psEntry = &maHandleDeepSleepTable[lpMode - 1];
        if (psEntry->pfFunc)
        {
            psEntry->pfFunc(psEntry->u32Config);
        }
    }

    return PWRLib_MCU_WakeupReason;
}


#endif /* #if (cPWR_FullPowerDownMode)*/

PWR_clock_32k_hk_t *PWR_GetHk32kHandle(void)
{
#if (cPWR_FullPowerDownMode)
    return &mHk32k;
#else
    return NULL;
#endif
}

/*---------------------------------------------------------------------------
 * Name: PWR_CheckForAndEnterNewPowerState
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
PWR_WakeupReason_t PWR_CheckForAndEnterNewPowerState(PWR_PowerState_t NewPowerState)
{
    PWR_WakeupReason_t ReturnValue;

    ReturnValue.AllBits = 0;

    if (NewPowerState == PWR_Run)
    {
        /* ReturnValue = 0; */
    }
    else if(NewPowerState == PWR_Sleep)
    {
        PWR_EnterSleep();
    }
#if (cPWR_FullPowerDownMode)
    else if(NewPowerState == PWR_PowerDown)
    {
        ReturnValue = PWR_EnterPowerDown();
    }
    else if(NewPowerState == PWR_DeepDown)
    {
        /* Never returns */
        PWR_EnterDeepDown();
    }
    else if(NewPowerState == PWR_OFF)
    {
        /* Never returns */
        PWR_EnterPowerOff();
    }
#else
    else if(NewPowerState == PWR_PowerDown)
    {
        PWR_EnterSleep();
    }
    else if(NewPowerState == PWR_DeepDown)
    {
        PWR_EnterSleep();
    }
    else if(NewPowerState == PWR_OFF)
    {
        PWR_EnterSleep();
    }
#endif
    else if(NewPowerState == PWR_Reset)
    {
        /* Never returns */
        PWR_SystemReset();
    }
    else
    {
        /* ReturnValue = FALSE; */
    }

    return ReturnValue;
}


/*****************************************************************************
 *                             PUBLIC FUNCTIONS                              *
 *---------------------------------------------------------------------------*
 * Add to this section all the functions that have global (project) scope.   *
 * These functions can be accessed outside this module.                      *
 * These functions shall have their declarations (prototypes) within the     *
 * interface header file and shall be preceded by the 'extern' keyword.      *
 *---------------------------------------------------------------------------*
 *****************************************************************************/

/*---------------------------------------------------------------------------
 * Name: PWR_Init
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWR_Init(void)
{
#if (cPWR_FullPowerDownMode)
    if (!isInitialized)
    {
    #if gClkUseFro32K /* Using 32k FRO */
        #if gClkRecalFro32K /* Will recalibrate 32k FRO on each warm start */
        // TODO MCB-539: parallelize FRO32K calibration to reduce the cold boot time
        uint32_t freq;
        PWR_clock_32k_hk_t *hk = PWR_GetHk32kHandle();

        PWR_Start32KCalibration();
        do {
           freq = PWR_Complete32KCalibration(FREQ32K_CAL_SHIFT);
        }  while (!freq);

        hk->freq32k_16thHz = freq;
        hk->freq32k = freq >> FREQ32K_CAL_SHIFT;
        PWR_DBG_LOG("freq=%d", hk->freq32k);

        #else /* no gClkRecalFro32K */
        /* Does selected sleep mode require 32kHz oscillator? */
        if (0 != (PWR_GetDeepSleepConfig() & PWR_CFG_OSC_ON))
        {
            bool            fmeas_clk_enable;
            bool            mdm_clk_enable;
            uint32_t        freqComp;

            /* Check if XTAL32K has been enabled by application, otherwise
             * enable the FRO32K and calibrate it - the XTAL32K may not be
             * present on the board */
            if (   (0 == (SYSCON->OSC32CLKSEL & SYSCON_OSC32CLKSEL_SEL32KHZ_MASK))
                || (0 == (PMC->PDRUNCFG & (1UL << kPDRUNCFG_PD_XTAL32K_EN)))
               )
            {
                /* Enable FRO32k */
                CLOCK_EnableClock(kCLOCK_Fro32k);

                /* Enable 32MHz XTAL if not running - FRO32K already enable if we are here*/
                if ( !(ASYNC_SYSCON->XTAL32MCTRL & ASYNC_SYSCON_XTAL32MCTRL_XO32M_TO_MCU_ENABLE_MASK) )
                {
                    CLOCK_EnableClock(kCLOCK_Xtal32M);
                }

                /* Fmeas clock gets enabled by the generic calibration code, so note if we
                   should disable it again afterwards */
                fmeas_clk_enable = CLOCK_IsClockEnable(kCLOCK_Fmeas);

                /* RFT1366 requires BLE LP clock to be used for 32kHz measurement. It gets
                   enabled within the generic calibration code, so note if we should
                   disable it again afterwards */
                mdm_clk_enable = SYSCON->MODEMCTRL & SYSCON_MODEMCTRL_BLE_LP_OSC32K_EN_MASK;

                /* Call Low Power function to start calibration */
                PWR_Start32KCalibration();

                /* Call Low Power function to wait for end of calibration */
                do
                {
                    freqComp = PWR_Complete32KCalibration(FREQ32K_CAL_SHIFT);
                } while (0 == freqComp);
                /* freqComp is returned in 16th of Hz */
                /* Disable the clocks if disable previously */
                if ( !fmeas_clk_enable )
                {
                    CLOCK_DisableClock(kCLOCK_Fmeas);
                }

                if ( !mdm_clk_enable )
                {
                    SYSCON->MODEMCTRL &= ~SYSCON_MODEMCTRL_BLE_LP_OSC32K_EN(1);
                }

                mHk32k.freq32k_16thHz = freqComp;
                mHk32k.freq32k = freqComp >> FREQ32K_CAL_SHIFT;
            }
            else
            {
                /* Case of the Xtal32k */
                mHk32k.freq32k_16thHz = 32768 << FREQ32K_CAL_SHIFT;
                mHk32k.freq32k = 32768;
            }
        }
        #endif /* gClkRecalFro32K */
    #else
        /* If using XTAL 32k, set calibration value to expected value */
        /* Case of the Xtal32k */
        mHk32k.freq32k_16thHz = 32768 << FREQ32K_CAL_SHIFT;
        mHk32k.freq32k = 32768;
    #endif  /* gClkUseFro32K */

        pwrm_force_retention = 0;
        memset(&pwrm_sleep_config, 0x0, sizeof(pm_power_config_t));

        PWRLib_Init();
        isInitialized = TRUE;
    }
#endif
}


/*---------------------------------------------------------------------------
 * Name: PWR_SystemReset
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWR_SystemReset(void)
{
    ResetMCU();
}


/*---------------------------------------------------------------------------
 * Name: PWR_AllowDeviceToSleep
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWR_AllowDeviceToSleep(void)
{
#if (cPWR_FullPowerDownMode)
    OSA_InterruptDisable();

    if(mLPMFlag > 0)
    {
        mLPMFlag--;
    }
    PWR_DBG_LOG("mLPMFlag=%d", mLPMFlag);

    OSA_InterruptEnable();
#endif /* (cPWR_FullPowerDownMode) */
}

/*---------------------------------------------------------------------------
 * Name: PWR_DisallowDeviceToSleep
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWR_DisallowDeviceToSleep(void)
{
#if (cPWR_FullPowerDownMode)
    uint8_t prot;

    OSA_InterruptDisable();

    prot = mLPMFlag + 1;

    if(prot != 0)
    {
        mLPMFlag++;
    }
    PWR_DBG_LOG("mLPMFlag=%d", mLPMFlag);

    OSA_InterruptEnable();
#endif /* (cPWR_FullPowerDownMode) */
}

#if (cPWR_FullPowerDownMode)
static int PWR_CheckSocTimers(void)
{
    int reason_debug = 0;

    do
    {
        /* Check if WTIMER enabled and, if so, check running status */
        if (0 != (SYSCON->AHBCLKCTRLS[0] & SYSCON_AHBCLKCTRL0_WAKE_UP_TIMERS_MASK))
        {
            bool dealine_too_close = FALSE;
            WTIMER_status_t InterruptStatus[2];
            uint32_t        timer_count[2];
            for (int i = 0; i < 2; i++)
            {
                InterruptStatus[i] = WTIMER_GetStatusFlags((WTIMER_timer_id_t)i);
                timer_count[i]     = WTIMER_ReadTimer((WTIMER_timer_id_t)i);

                if (   (timer_count[i] < PWR_PREVENT_SLEEP_IF_LESS_TICKS)
                    && (InterruptStatus[i] == WTIMER_STATUS_RUNNING)
                   )
                {
                    dealine_too_close = TRUE;
                    reason_debug = -2;
                    break; /* for */
                }
            }
            if (dealine_too_close)
            {
                reason_debug = -3;
                break; /* do .. while (0) */
            }

            /* The goal is to prevent to sleep if there is a wtimer pending IRQ that could have fired just
            * after having disabled global interrupts.
            * In this case do not allow to sleep because otherwise the device will go to sleep
            * and would probability never wake up if there is no other wake event programmed
            */
            if (NVIC_GetPendingIRQ(WAKE_UP_TIMER1_IRQn) || NVIC_GetPendingIRQ(WAKE_UP_TIMER0_IRQn))
            {
                reason_debug = -4;
                break;
            }

            /* MCUZIGBEE-946: If pwrm wake timer is not running and sleep mode
             * is 32k OSC ON, prevent sleep.
             * MCUZIGBEE-2469: However, checking for a running wake timer 1 in
             * OSC ON mode is not necessary or desirable when BLE is running,
             * as it sleeps with the 32k OSC ON but no wake timer.
             */
            if ( (FALSE == bBLE_Active)
                && (0 != (PWR_GetDeepSleepConfig() & PWR_CFG_OSC_ON))
                && (InterruptStatus[1] != WTIMER_STATUS_RUNNING)
               )
            {
                reason_debug = -5;
                break;
            }
        }
        if  (0 != (SYSCON->AHBCLKCTRLS[0] & SYSCON_AHBCLKCTRL0_RTC_MASK))
        {
            uint32_t rtc_ctrl = RTC->CTRL;
            if (rtc_ctrl & (RTC_CTRL_ALARM1HZ_MASK | RTC_CTRL_WAKE1KHZ_MASK))
            {
                /* Alarm or wake interrupt have already fired " prevent sleep */
                reason_debug = -6;
                break;
            }
            if (rtc_ctrl &  RTC_CTRL_WAKEDPD_EN_MASK)
            {
                /* 1kHz clock timer is running */
                /* Check if the deadline is not too close to go to sleep */
                uint32_t wake_countdown = RTC->WAKE;
                if ((wake_countdown > 0) && (MILLISECONDS_TO_TICKS32K(wake_countdown) < PWR_PREVENT_SLEEP_IF_LESS_TICKS))
                {
                    reason_debug = -7;
                    break;
                }
            }
        }
    }
    while (0);

    return reason_debug;
}
#endif

/*---------------------------------------------------------------------------
 * Name: PWR_CheckIfDeviceCanGoToSleep
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t PWR_CheckIfDeviceCanGoToSleep(void)
{
    bool_t returnValue = FALSE;
#if (cPWR_FullPowerDownMode)
    uint8_t  pwr_mode = 0xff;
    bool_t tmr_all_off = FALSE;
    int reason_debug = 0;
    do {
        if (PWR_GetDeepSleepMode() == 0)
        {
            reason_debug = -8;
            break;
        }
        /* Check if device can sleep for Application */
        if (mLPMFlag > 0 || mPreventEnterLowPower)
        {
            reason_debug = -1;
            break;
        }
        /* Check if Timers are all OFF */
#if (!cPWR_DiscardRunningTimerForPowerDown) && gSupportBle
        tmr_all_off = TMR_AreAllTimersOff();
        if (!tmr_all_off) break;
#endif

#if gSupportBle
        /* Check first if ble can go to sleep because
         * it could start a calibration
         */
        if (PWR_GetDeepSleepConfig() & PWR_CFG_RAM_ON)
        {
            /* Check if BLE LL can sleep */
            pwr_mode = (uint8_t)BLE_get_sleep_mode();
            if (pwr_mode < kPmPowerDown0) break;
        }
#endif

       reason_debug = PWR_CheckSocTimers();
       if ( reason_debug ) break;

       /* All conditions successfully passed */
       returnValue = TRUE;

    } while (0);

    PWR_DBG_LOG("mLPMFlag=%d TimerOff=%d ble pwr_mode=%d", mLPMFlag, tmr_all_off, pwr_mode);
    PWR_DBG_LOG("reason_debug=%d", reason_debug);

    NOT_USED(pwr_mode);
    NOT_USED(tmr_all_off);
    NOT_USED(reason_debug);
#endif
    return returnValue;
}

int PWR_CheckIfDeviceCanGoToSleepExt(void)
{
    int    reason_debug = 0;

#if (cPWR_FullPowerDownMode)
    bool_t tmr_all_off  = FALSE;
    do {
        if (PWR_GetDeepSleepMode() == 0)
        {
            reason_debug = -8;
            break;
        }
        /* Check if device can sleep for Application */
        if (mLPMFlag > 0 || mPreventEnterLowPower)
        {
            reason_debug = -1;
            break;
        }

        /* legacy mode, call this first */
#if gSupportBle && !gPWR_BleLL_EarlyEnterDsm
        /* Check first if ble can go to sleep because
         * it could start a calibration
         */
        if (PWR_GetDeepSleepConfig() & PWR_CFG_RAM_ON)
        {
            /* Check if BLE LL can sleep  (return 2 (kPmPowerDown0) or above if so )*/
            LpIoSet(1, 1);
            reason_debug = (uint8_t)BLE_get_sleep_mode();
            LpIoSet(1, 0);
            if (reason_debug < kPmPowerDown0)
            {
                 break;
            }
        }
        else
        {
            /* Ram Off */
            reason_debug =  kPmPowerDown1;   // -kPmPowerDown1
            break;
        }
#endif

        /* Check if Timers are all OFF */
#if (!cPWR_DiscardRunningTimerForPowerDown) && gSupportBle
        tmr_all_off = TMR_AreAllTimersOff();
        if (!tmr_all_off) break;
#endif

       int ret = PWR_CheckSocTimers();
       if ( ret != 0 )     // do not overide reason_debug if ret is 0
       {
          /* if timer prevents lowpower, needs to negative the returned value */
          reason_debug = ret;
          break;
       }

        /* with gPWR_BleLL_EarlyEnterDsm, Ble LL will go to lowpower if possible
            ->  shall be the last check before the lowpower entry     */
#if gSupportBle && gPWR_BleLL_EarlyEnterDsm
        /* Check first if ble can go to sleep because
         * it could start a calibration
         */
        if (PWR_GetDeepSleepConfig() & PWR_CFG_RAM_ON)
        {
            /* Check if BLE LL can sleep and for sleep if So (return 2 or above)*/
            LpIoSet(1, 1);
            reason_debug = (uint8_t)BLE_sleep();
            LpIoSet(1, 0);
        }
        else
        {
            /* Ram Off */
            reason_debug = kPmPowerDown1;
        }
#endif

       /* if failing to enter sleep, code has already broken above
            turn the pwr_mode inverse so the value is negative to allow sleep
            kPmActive,     // CPU is executing
            kPmSleep,      // CPU clock is gated
            kPmPowerDown0, // Power is shut down except for always on domain, 32k clock and selected wakeup source
            kPmPowerDown1  // Power is shut down except for always on domain and selected wakeup source
        */


    } while (0);

    //PWR_DBG_LOG("mLPMFlag=%d TimerOff=%d", mLPMFlag, tmr_all_off);
    PWR_DBG_LOG("reason_debug=%d", reason_debug);

    NOT_USED(tmr_all_off);
#endif
    return reason_debug;
}

/*---------------------------------------------------------------------------
 * Name: PWR_ChangeDeepSleepMode
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
bool_t PWR_ChangeDeepSleepMode(uint8_t dsMode)
{
    bool_t result = TRUE;
    PWR_DBG_LOG("dsMode=%d", dsMode);

#if (cPWR_FullPowerDownMode)
    if ((dsMode > sizeof(maHandleDeepSleepTable)/sizeof(SleepModeTable_t)) ||
        (maHandleDeepSleepTable[dsMode - 1].pfFunc == (pfHandleDeepSleepFunc_t)0))
    {
        result = FALSE;
    }
    else
    {
        PWRLib_SetDeepSleepMode(dsMode);
    }
#endif /* (cPWR_FullPowerDownMode) */

    return result;
}

/*---------------------------------------------------------------------------
 * Name: PWR_GetDeepSleepMode
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PWR_GetDeepSleepMode(void)
{
    uint8_t result = 0;
#if (cPWR_FullPowerDownMode)
    result = PWRLib_GetDeepSleepMode();
#endif
    return result;
}

/*---------------------------------------------------------------------------
 * Name: PWR_GetDeepSleepConfig
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint32_t PWR_GetDeepSleepConfig(void)
{
    uint32_t result = 0;
#if (cPWR_FullPowerDownMode)
    uint8_t  lpMode;
    lpMode = PWRLib_GetDeepSleepMode();

    if (lpMode)
    {
        SleepModeTable_t const *psEntry = &maHandleDeepSleepTable[lpMode - 1];
        if (psEntry->pfFunc)
        {
            result = psEntry->u32Config;
        }
    }
#endif
    return result;
}

/*---------------------------------------------------------------------------
 * Name: PWR_EnterLowPower
 * Description: - Main entry power to switch to low power mode - checks if power
 * down mode is allowed and enter in power down. If not allowed, will fall back
 * to sleep mode (WFI)
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
extern PWR_WakeupReason_t PWR_EnterLowPower(void)
{
    PWR_DBG_LOG("");
    PWR_WakeupReason_t ReturnValue;

    ReturnValue.AllBits = 0;

    OSA_InterruptDisable();

#if (cPWR_FullPowerDownMode)
    if(PWR_CheckIfDeviceCanGoToSleep())
    {
        ReturnValue = PWR_EnterPowerDown();
    }
    else
#endif
    {
        PWRLib_MCU_Enter_Sleep();
    }

    OSA_InterruptEnable();

    return ReturnValue;
}

/*---------------------------------------------------------------------------
 * Name: PWR_EnterSleep
 * Description: - Enter the chip in WFI state - CPU clock is switched OFF
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWR_EnterSleep(void)
{
    PWR_DBG_LOG("");

    OSA_DisableIRQGlobal();

    PWRLib_MCU_Enter_Sleep();

    OSA_EnableIRQGlobal();
}

/*---------------------------------------------------------------------------
 * Name: PWR_EnterPowerDown
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
PWR_WakeupReason_t PWR_EnterPowerDown(void)
{
    //PWR_DBG_LOG("");
    PWR_WakeupReason_t ReturnValue;
    ReturnValue.AllBits = 0;
#if (cPWR_FullPowerDownMode)
    OSA_InterruptDisable();

    ReturnValue = PWR_HandleDeepSleep();

    OSA_InterruptEnable();
#endif
    return ReturnValue;
}

/*---------------------------------------------------------------------------
 * Name: PWR_EnterDeepDown
 * Description: - All the chip is OFF - only wakeup by IO or NTAG
 * Parameters: -
 * Return: - never return
 *---------------------------------------------------------------------------*/
void PWR_EnterDeepDown(void)
{
    PWR_DBG_LOG("");
#if (cPWR_FullPowerDownMode)
    /* Never returns */
    pwrlib_pd_cfg_t pd_cfg;

    OSA_InterruptDisable();

    /* get the IO wakeup configuration */
    vSetWakeUpIoConfig();

    memset( &pd_cfg, 0x0, sizeof(pwrlib_pd_cfg_t) );
    pd_cfg.wakeup_io = PWR_GetWakeUpConfig();

    /* call any registered pre-sleep callbacks */
    if(gpfPWR_LowPowerEnterCb != NULL)
    {
        gpfPWR_LowPowerEnterCb();
    }

    PWRLib_EnterDeepDownMode(&pd_cfg);
#endif
}

/*---------------------------------------------------------------------------
 * Name: PWR_EnterPowerOff
 * Description: - All the chip is OFF - only wakeup by HW reset
 * Parameters: -
 * Return: - never return
 *---------------------------------------------------------------------------*/
void PWR_EnterPowerOff(void)
{
    PWR_DBG_LOG("");
#if (cPWR_FullPowerDownMode)
    /* Never returns */
    pwrlib_pd_cfg_t pd_cfg;

    OSA_InterruptDisable();

    memset( &pd_cfg, 0x0, sizeof(pwrlib_pd_cfg_t) );

    /* call any registered pre-sleep callbacks */
    if(gpfPWR_LowPowerEnterCb != NULL)
    {
        gpfPWR_LowPowerEnterCb();
    }

    PWRLib_EnterDeepDownMode(&pd_cfg);
#endif
}

/*---------------------------------------------------------------------------
 * Name: PWR_RegisterLowPowerEnterCallback
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWR_RegisterLowPowerEnterCallback(pfPWRCallBack_t lowPowerEnterCallback)
{
#if (cPWR_FullPowerDownMode)
    gpfPWR_LowPowerEnterCb = lowPowerEnterCallback;
#endif /* (cPWR_FullPowerDownMode) */
}

/*---------------------------------------------------------------------------
 * Name: PWR_RegisterLowPowerExitCallback
 * Description: -
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWR_RegisterLowPowerExitCallback(pfPWRCallBack_t lowPowerExitCallback)
{
#if (cPWR_FullPowerDownMode)
    gpfPWR_LowPowerExitCb = lowPowerExitCallback;
#endif /* (cPWR_FullPowerDownMode) */
}


/*---------------------------------------------------------------------------
 * Name: PWR_PreventEnterLowPower
 * Description: Forced prevention of entering low-power
 * Parameters: prevent - if TRUE prevents the entering to low-power
 * Return: -
 *---------------------------------------------------------------------------*/
void PWR_PreventEnterLowPower(bool_t prevent)
{
#if (cPWR_FullPowerDownMode)
    mPreventEnterLowPower = prevent;
#endif
}

/*---------------------------------------------------------------------------
 * Name: PWR_IndicateBLEActive
 * Description: Indicate BLE active. This information is used to determine if
 * wake timer needs to be running when going to a power down mode with the 32k
 * clock running: if BLE is active then wake timer is not needed to be
 * running, otherwise it is.
 * Parameters: bBLE_ActiveIndication - TRUE if BLE is active, FALSE if not
 * Return: -
 *---------------------------------------------------------------------------*/
void PWR_IndicateBLEActive(bool_t bBLE_ActiveIndication)
{
#if (cPWR_FullPowerDownMode)
    bBLE_Active = bBLE_ActiveIndication;
#endif
}

void PWR_Start32kCounter(void)
{
    Timestamp_Init();
}

uint32_t PWR_Get32kTimestamp(void)
{
    return Timestamp_GetCounter32bit();
}

#if (cPWR_FullPowerDownMode)
/*---------------------------------------------------------------------------
 * Name: PWR_SetWakeUpConfig
 * Description:
 * Parameters:
 * Return: -
 *---------------------------------------------------------------------------*/
void PWR_SetWakeUpConfig(uint32_t set_msk, uint32_t clr_msk)
{
    pwr_wakeup_io &= ~clr_msk;
    pwr_wakeup_io |= set_msk;
    //PWR_DBG_LOG("wakesrc=%x", pwr_wakeup_io);
}

/*---------------------------------------------------------------------------
 * Name: PWR_GetWakeUpConfig
 * Description:
 * Parameters:
 * Return: -
 *---------------------------------------------------------------------------*/
uint32_t PWR_GetWakeUpConfig(void)
{
    return pwr_wakeup_io;
}

/*---------------------------------------------------------------------------
 * Name: vSetWakeUpIoConfig
 * Description:
 * Parameters:
 * Return: -
 *---------------------------------------------------------------------------*/
void vSetWakeUpIoConfig(void)
{
    uint32_t wkup_src;

    /* Get GPIO wake-up sources from framework GPIO */
    wkup_src = GpioGetIoWakeupSource();
#if gSwdWakeupReconnect_c
    wkup_src |= BIT(SWDIO_GPIO_PIN);
#endif
    /* Add wake-up sources registered directly (backwards compatibility) */
    wkup_src |= pwrm_sleep_config.pm_wakeup_io;

    /* Write to LP configuration store */
    PWR_SetWakeUpConfig(wkup_src, (uint32_t)(BIT(22)-1));
}

/* Added from PWRM */
uint64_t u64GetWakeupSourceConfig(uint32_t u32Mode)
{
    /* Check for WTIMER block enabled, and 32k clock running in sleep */
    if (0 != (u32Mode & PWR_CFG_OSC_ON))
    {
        if (0 != (SYSCON->AHBCLKCTRLS[0] & SYSCON_AHBCLKCTRLSET0_WAKE_UP_TIMERS_CLK_SET(1)))
        {
            WTIMER_status_t     timer_status;
            /* Check wake timers */
            timer_status = WTIMER_GetStatusFlags(WTIMER_TIMER1_ID);
            if ( timer_status == WTIMER_STATUS_RUNNING )
            {
                pwrm_sleep_config.pm_wakeup_src |= POWER_WAKEUPSRC_WAKE_UP_TIMER1;
            }
            else
            {
                pwrm_sleep_config.pm_wakeup_src &= ~(uint64_t)POWER_WAKEUPSRC_WAKE_UP_TIMER1;
            }

            timer_status = WTIMER_GetStatusFlags(WTIMER_TIMER0_ID);
            if ( timer_status == WTIMER_STATUS_RUNNING )
            {
                pwrm_sleep_config.pm_wakeup_src |= POWER_WAKEUPSRC_WAKE_UP_TIMER0;
            }
            else
            {
                pwrm_sleep_config.pm_wakeup_src &= ~(uint64_t)POWER_WAKEUPSRC_WAKE_UP_TIMER0;
            }
        }
        else
        {
            /* No wake timers enabled */
            pwrm_sleep_config.pm_wakeup_src &= ~(uint64_t)(POWER_WAKEUPSRC_WAKE_UP_TIMER0
                                                 | POWER_WAKEUPSRC_WAKE_UP_TIMER1);
        }

        if (0 != (SYSCON->AHBCLKCTRLS[0] & SYSCON_AHBCLKCTRL0_RTC(1)))
        {
            if (RTC->CTRL & RTC_CTRL_RTC_EN_MASK)
            {
                pwrm_sleep_config.pm_wakeup_src |=  POWER_WAKEUPSRC_RTC;
            }
            else
            {
                pwrm_sleep_config.pm_wakeup_src &=  ~(uint64_t)POWER_WAKEUPSRC_RTC;
            }
        }
    }
    return pwrm_sleep_config.pm_wakeup_src;
}

PWR_teStatus PWR_vWakeUpIO(uint32_t io_mask)
{
    PWR_teStatus status;

    //if ( !(E_AHI_SLEEP_DEEP == s_ePowerMode) )
    if ( io_mask == 0 )
    {
        /* remove io wake up source */
        pwrm_sleep_config.pm_wakeup_io   = 0;
        pwrm_sleep_config.pm_wakeup_src &= ~(uint64_t)POWER_WAKEUPSRC_IO;

        status = PWR_E_OK;
    }
    else if ( (io_mask & (~((1<<23)-1))) !=0 )
    {
        /* bit beyond 23 is set */
        status = PWR_E_IO_INVALID;
    }
    else
    {
        pwrm_sleep_config.pm_wakeup_io   = io_mask;
        pwrm_sleep_config.pm_wakeup_src |= POWER_WAKEUPSRC_IO;

        status = PWR_E_OK;
    }

    return status;
}

PWR_teStatus PWR_vWakeUpConfig(uint32_t pwrm_config)
{
    PWR_teStatus status;

    if ( pwrm_config == 0 )
    {
        /* remove io wake up source */
        pwrm_sleep_config.pm_wakeup_io   = 0;
        pwrm_sleep_config.pm_wakeup_src &= ~ (uint64_t)(POWER_WAKEUPSRC_IO
                                              | POWER_WAKEUPSRC_NFCTAG
                                              | PWR_ANA_COMP_WAKEUP
                                              | PWR_BOD_WAKEUP);

        status = PWR_E_OK;
    }
    else if ( (pwrm_config & (~((PWR_BOD_WAKEUP<<1)-1))) !=0 )
    {
        /* bit beyond PWRM_BOD_WAKEUP is set */
        status = PWR_E_IO_INVALID;
    }
    else
    {
        uint32_t io_cfg = pwrm_config & ( PWR_NTAG_FD_WAKEUP-1 );
        if ( io_cfg )
        {
            pwrm_sleep_config.pm_wakeup_io   = io_cfg;
            pwrm_sleep_config.pm_wakeup_src |= POWER_WAKEUPSRC_IO;
        }
        if ( pwrm_config & PWR_NTAG_FD_WAKEUP )
        {
            pwrm_sleep_config.pm_wakeup_src |= POWER_WAKEUPSRC_NFCTAG;
        }
        if ( pwrm_config & PWR_ANA_COMP_WAKEUP )
        {
            pwrm_sleep_config.pm_wakeup_src |= POWER_WAKEUPSRC_ANA_COMP;
        }
        if ( pwrm_config & PWR_BOD_WAKEUP )
        {
            pwrm_sleep_config.pm_wakeup_src |= POWER_WAKEUPSRC_SYSTEM;
        }

        status = PWR_E_OK;
    }

    return status;
}


void PWR_vForceRamRetention(uint32_t u32RetainBitmap)
{
    pwrm_force_retention &= ~(PM_CFG_SRAM_ALL_RETENTION << PM_CFG_SRAM_BANK_BIT_BASE);
    pwrm_force_retention |= (u32RetainBitmap & PM_CFG_SRAM_ALL_RETENTION) << PM_CFG_SRAM_BANK_BIT_BASE;
}

void PWR_vAddRamRetention(uint32_t u32Start, uint32_t u32Length)
{
    uint32_t u32RamBanks;

    u32RamBanks = PWRLib_u32RamBanksSpanned(u32Start, u32Length);
    pwrm_force_retention |= (u32RamBanks & PM_CFG_SRAM_ALL_RETENTION) << PM_CFG_SRAM_BANK_BIT_BASE;
}

uint32_t PWR_u32GetRamRetention(void)
{
    return pwrm_force_retention;
}

void PWR_vForceRadioRetention(bool_t bRetain)
{
    if (bRetain)
    {
        pwrm_force_retention |= PM_CFG_RADIO_RET;
    }
    else
    {
        pwrm_force_retention &= ~PM_CFG_RADIO_RET;
    }
}
static PWR_teStatus PWR_eTmrStatusToPwrStatus(TMR_teActivityStatus tmrStatus)
{
    PWR_teStatus status = PWR_E_TIMER_INVALID;
    switch(tmrStatus)
    {
        case TMR_E_ACTIVITY_OK:
            status = PWR_E_OK;
            break;
        case TMR_E_ACTIVITY_RUNNING:
            status = PWR_E_TIMER_RUNNING;
            break;
        case TMR_E_ACTIVITY_FREE:
            status = PWR_E_TIMER_FREE;
            break;
        default:
            break;
    }
    return status;
}
PWR_teStatus PWR_eRemoveActivity(PWR_tsWakeTimerEvent *psWake)
{
    PWR_teStatus status = PWR_E_TIMER_INVALID;
    TMR_teActivityStatus tmrStatus = TMR_eRemoveActivity((TMR_tsActivityWakeTimerEvent *)psWake);
    status = PWR_eTmrStatusToPwrStatus(tmrStatus);
    return status;
}

PWR_teStatus PWR_eScheduleActivity(PWR_tsWakeTimerEvent *psWake,
                                   uint32_t u32TimeMs,
                                   void (*prCallbackfn)(void))
{
    uint64_t              u64AdjustedTicks;
    uint32_t u32Ticks;
    uint32_t freqHz = mHk32k.freq32k;
    u64AdjustedTicks = u32TimeMs;
    u64AdjustedTicks = u64AdjustedTicks * freqHz / 1000;

    if (u64AdjustedTicks > 0xffffffff)
    {
        /* Overflowed, so limit to maximum uint32 value */
        u32Ticks = 0xffffffff;
    }
    else
    {
        u32Ticks = (uint32_t)u64AdjustedTicks;
    }
    return PWR_eScheduleActivity32kTicks(psWake, u32Ticks, prCallbackfn);
}

PWR_teStatus PWR_eScheduleActivity32kTicks(PWR_tsWakeTimerEvent *psWake,
                                   uint32_t u32Ticks,
                                   void (*prCallbackfn)(void))
{
    PWR_teStatus status = PWR_E_TIMER_INVALID;
    /* Ensure that sleep mode keeps 32k oscillator running */
    if (PWR_GetDeepSleepConfig() & PWR_CFG_OSC_ON)
    {
        TMR_teActivityStatus tmrStatus = TMR_eScheduleActivity32kTicks((TMR_tsActivityWakeTimerEvent *)psWake, u32Ticks, prCallbackfn);
        status = PWR_eTmrStatusToPwrStatus(tmrStatus);
    }
    return status;
}


void PWR_vColdStart(void)
{
    reset_cause_t reset_cause;

    vAppRegisterPWRCallbacks();

    reset_cause = POWER_GetResetCause();

    /* if coming from power down mode, stop the timer if running and clear interrupts (No WAKE Cb to call in cold start)
     * if coming from other reset cause, reset the Wakeup timer
     *Do not reset the Wake timer if coming from power down mode */
    if ( reset_cause == RESET_WAKE_PD )
    {
        // TODO: need to check whether the timer has been correctly initialized previously (AHBCLK ON)
        WTIMER_StopTimer(WTIMER_TIMER1_ID);
    }
    else
    {
        RESET_SetPeripheralReset(kWKT_RST_SHIFT_RSTn);
        RESET_ClearPeripheralReset(kWKT_RST_SHIFT_RSTn);
    }

    if ( (reset_cause == RESET_WAKE_DEEP_PD) || (reset_cause == RESET_WAKE_PD) )
    {
        // Call any registered callbacks -
        if (gpfPWR_LowPowerExitCb != NULL)
        {
            gpfPWR_LowPowerExitCb();
        }
    }
}
#endif
