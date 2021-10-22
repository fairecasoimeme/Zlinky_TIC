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
#include "fsl_device_registers.h"
#include "board.h"
#include "PWRLib.h"
#include "PWR_Configuration.h"
#include "TimersManager.h"
#include "fsl_os_abstraction.h"
#include "GPIO_Adapter.h"
#include "fsl_power.h"
#include "rom_lowpower.h"
#include "fsl_flash.h"
#include "board_utility.h"

#ifndef LpIoSet
#define LpIoSet(x, y)
#endif

#if gSupportBle
#include "ble_general.h"
#include "controller_interface.h"
#endif

#include "rom_api.h"
#include "fsl_debug_console.h"

extern uint32_t                   __base_RAM0;
extern uint32_t                   __base_RAM1;

extern uint32_t                   _end_boot_resume_stack;


#ifndef gPWR_FreqScalingWFI
#define gPWR_FreqScalingWFI 0
#endif

#if (gPWR_FreqScalingWFI > 0)
  #if (gPWR_FreqScalingWFI != 32) && (gPWR_FreqScalingWFI != 24) && (gPWR_FreqScalingWFI != 16) && (gPWR_FreqScalingWFI != 12) && (gPWR_FreqScalingWFI != 8) && (gPWR_FreqScalingWFI != 6) && (gPWR_FreqScalingWFI != 4)  && (gPWR_FreqScalingWFI != 2)
  #error "Should define a WFI frequency that is a whole divisor of 48M, 32M or 12M and greater or equal than 12M"
  #endif
#endif

#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = ".s_end_fw_retention"
static uint32_t                   _end_fw_retention;
#pragma section="HEAP"
#define heapStartAddr (uint32_t)__section_begin("HEAP")
#define heapEndAddr   (uint32_t)__section_end("HEAP")
#else
extern uint32_t                   _end_fw_retention;
extern uint32_t                   _pvHeapStart;
extern uint32_t                   _pvHeapLimit;
#define heapStartAddr (uint32_t)&_pvHeapStart
#define heapEndAddr   (uint32_t)&_pvHeapLimit
#endif

#if gSupportBle
#define BLE_BASE_ADDR 0x400A0000
#define BLE_INTSTAT_REG       *((volatile uint32_t*)(BLE_BASE_ADDR + 0x00000010))
#define BLE_RADIOPWRUPDN_REG  *((volatile uint32_t*)(BLE_BASE_ADDR + 0x00000080))
#define BLE_SLPINTSTAT_BIT    4
#endif

#define RESUME_STACK_POINTER      ((uint32_t)&_end_boot_resume_stack)
#define PWR_JUMP_BUF_SIZE         10

typedef unsigned long PWR_Register;
typedef PWR_Register PWR_jmp_buf[PWR_JUMP_BUF_SIZE];

void PWR_longjmp(PWR_jmp_buf __jmpb, int __retval);
int PWR_setjmp(PWR_jmp_buf __jmpb);

uint32_t PWRLib_u32RamBanksSpanned(uint32_t u32Start, uint32_t u32Length);

PWR_jmp_buf                   pwr_CPUContext;


#if defined(__IAR_SYSTEMS_ICC__) || defined(__GNUC__)
    #define GET_MSP(x) asm volatile("MRS %0, MSP" : "=r" (x))
#else
    #error "must define get_MSP"
#endif

/*****************************************************************************
 *                             PRIVATE MACROS                                *
 *---------------------------------------------------------------------------*
 * Add to this section all the access macros, registers mappings, bit access *
 * macros, masks, flags etc ...                                              *
 *---------------------------------------------------------------------------*
 *****************************************************************************/
#ifndef BIT
#define BIT(x)                        (1 << (x))
#endif

#ifndef KB
#define KB(x)                        ((x) << 10)
#endif

/* The RAM starts at 0x04000000 but for our purposes we only care
   about it from __base_RAM0 onwards, as the area before this is used by boot
   code data that does not have to be preserved through sleep. Thus the start
   address for bank 0 is taken as __base_RAM0 */
#define PWR_SRAM0_BASE                (0x04000000)
#define PWR_SRAM0_BANK0_BASE          PWR_SRAM0_BASE
#define PWR_SRAM0_BANK0_START_ADDR    ((uint32_t)&__base_RAM0)
#define PWR_SRAM0_BANK1_START_ADDR    (PWR_SRAM0_BANK0_BASE + KB(16))
#define PWR_SRAM0_BANK2_START_ADDR    (PWR_SRAM0_BANK1_START_ADDR + KB(16))
#define PWR_SRAM0_BANK3_START_ADDR    (PWR_SRAM0_BANK2_START_ADDR + KB(16))
#define PWR_SRAM0_BANK4_START_ADDR    (PWR_SRAM0_BANK3_START_ADDR + KB(16))
#define PWR_SRAM0_BANK5_START_ADDR    (PWR_SRAM0_BANK4_START_ADDR + KB(8))
#define PWR_SRAM0_BANK6_START_ADDR    (PWR_SRAM0_BANK5_START_ADDR + KB(8))
#define PWR_SRAM0_BANK7_START_ADDR    (PWR_SRAM0_BANK6_START_ADDR + KB(4))
#define PWR_SRAM0_BANK7_END_ADDR      (PWR_SRAM0_BANK7_START_ADDR + KB(4))
#define PWR_SRAM0_END                 PWR_SRAM0_BANK7_END_ADDR

#define PWR_SRAM1_BASE              (0x04020000)
#define PWR_SRAM1_BANK0_START_ADDR  PWR_SRAM1_BASE
#define PWR_SRAM1_BANK1_START_ADDR  (PWR_SRAM1_BANK0_START_ADDR + KB(16))
#define PWR_SRAM1_BANK2_START_ADDR  (PWR_SRAM1_BANK1_START_ADDR + KB(16))
#define PWR_SRAM1_BANK3_START_ADDR  (PWR_SRAM1_BANK2_START_ADDR + KB(16))
#define PWR_SRAM1_BANK3_END_ADDR    (PWR_SRAM1_BANK3_START_ADDR + KB(16))
#define PWR_SRAM1_END               PWR_SRAM1_BANK3_END_ADDR

typedef struct {
    uint32_t start_address;
    uint32_t size;
} sram_bank_desc_t;

const sram_bank_desc_t pm_sram0_bank_desc[] =
{
    {PWR_SRAM0_BANK0_START_ADDR, KB(16)},
    {PWR_SRAM0_BANK1_START_ADDR, KB(16)},
    {PWR_SRAM0_BANK2_START_ADDR, KB(16)},
    {PWR_SRAM0_BANK3_START_ADDR, KB(16)},
    {PWR_SRAM0_BANK4_START_ADDR, KB(8)},
    {PWR_SRAM0_BANK5_START_ADDR, KB(8)},
    {PWR_SRAM0_BANK6_START_ADDR, KB(4)},
    {PWR_SRAM0_BANK7_START_ADDR, KB(4)}
};

const sram_bank_desc_t pm_sram1_bank_desc[] =
{
    {PWR_SRAM1_BANK0_START_ADDR, KB(16)},   /* Bank 8 */
    {PWR_SRAM1_BANK1_START_ADDR, KB(16)},   /* Bank 9 */
    {PWR_SRAM1_BANK2_START_ADDR, KB(16)},   /* Bank 10 */
    {PWR_SRAM1_BANK3_START_ADDR, KB(16)},   /* Bank 11 */
};


/*****************************************************************************
 *                               PRIVATE VARIABLES                           *
 *---------------------------------------------------------------------------*
 * Add to this section all the variables and constants that have local       *
 * (file) scope.                                                             *
 * Each of this declarations shall be preceded by the 'static' keyword.      *
 * These variables / constants cannot be accessed outside this module.       *
 *---------------------------------------------------------------------------*
 *****************************************************************************/
static uint8_t mPWRLib_DeepSleepMode = 0;

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

volatile PWR_WakeupReason_t PWRLib_MCU_WakeupReason;


/*****************************************************************************
 *                           PRIVATE FUNCTIONS PROTOTYPES                    *
 *---------------------------------------------------------------------------*
 * Add to this section all the functions prototypes that have local (file)   *
 * scope.                                                                    *
 * These functions cannot be accessed outside this module.                   *
 * These declarations shall be preceded by the 'static' keyword.             *
 *---------------------------------------------------------------------------*
 *****************************************************************************/

/*****************************************************************************
 *                                PRIVATE FUNCTIONS                          *
 *---------------------------------------------------------------------------*
 * Add to this section all the functions that have local (file) scope.       *
 * These functions cannot be accessed outside this module.                   *
 * These definitions shall be preceded by the 'static' keyword.              *
 *---------------------------------------------------------------------------*
*****************************************************************************/
#if ROM_API_CALL
#include "lowpower_jn5180.h"
extern void Chip_LOWPOWER_PowerCycleCpuAndFlash(LPC_LOWPOWER_T *p_lowpower_cfg);
#endif

#if defined(__IAR_SYSTEMS_ICC__)
__ramfunc
#elif  defined(__GNUC__)
__attribute__ ((section (".ramfunc")))
#endif
void PWRLib_EnterSleep_WithFlashControllerPowerDown(void)
{
#if (gPWR_FlashControllerPowerDownInWFI == 2)
    uint32_t ldoflashcore_prev_voltage = 0;
#endif

    /* Clear the DONE status bit */
    FLASH->INT_CLR_STATUS = FLASH_DONE;

    /* Power down the flash */
    FLASH->CMD = FLASH_CMD_POWERDOWN;

    /* Wait for flash power down is done before turning OFF LDO flash */
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");

#if (gPWR_FlashControllerPowerDownInWFI == 2)
    {
       uint32_t ldo_flash_core_reg       = PMC -> LDOFLASHCORE;
       uint32_t ldoflashcore_new_voltage = (p_lowpower_cfg->VOLTAGE & LOWPOWER_VOLTAGE_LDO_FLASH_CORE_MASK) >> LOWPOWER_VOLTAGE_LDO_FLASH_CORE_INDEX;

       /* Switch OFF the clock first */
       SYSCON -> CLOCK_CTRL &= ~SYSCON_CLOCK_CTRL_FLASH48MHZ_ENA_MASK;
       /* Save and set new LDO Flash Core voltage */
       if ( PMC -> CTRL & PMC_CTRL_SELLDOVOLTAGE_MASK )
       {
           ldoflashcore_prev_voltage  = (ldo_flash_core_reg & PMC_LDOFLASHCORE_VADJ_2_MASK) >> PMC_LDOFLASHCORE_VADJ_2_SHIFT;
           PMC -> LDOFLASHCORE = (ldo_flash_core_reg & (~PMC_LDOFLASHCORE_VADJ_2_MASK)) | ( ldoflashcore_new_voltage << PMC_LDOFLASHCORE_VADJ_2_SHIFT );
       }
       else
       {
           ldoflashcore_prev_voltage  = (ldo_flash_core_reg & PMC_LDOFLASHCORE_VADJ_MASK) >> PMC_LDOFLASHCORE_VADJ_SHIFT;
           PMC -> LDOFLASHCORE = (ldo_flash_core_reg & (~PMC_LDOFLASHCORE_VADJ_MASK))   | ( ldoflashcore_new_voltage << PMC_LDOFLASHCORE_VADJ_SHIFT );
       }
    }
#else
    /* switch OFF the Flash controller switch to reduce power */
      SYSCON->CLOCK_CTRL &= ~SYSCON_CLOCK_CTRL_FLASH48MHZ_ENA_MASK;
#endif

    POWER_ENTER_SLEEP();

#if (gPWR_FlashControllerPowerDownInWFI == 2)
    {
       uint32_t ldo_flash_core_reg     = PMC -> LDOFLASHCORE;
       uint32_t delay_5us              = ((p_lowpower_cfg->CFG & LOWPOWER_CFG_LDOFLASHCORE_DELAY_MASK) >> LOWPOWER_CFG_LDOFLASHCORE_DELAY_INDEX) + 4 /* 19us */;   /* 4.75 us per unit */

        /* restore LDO FLASH CORE Voltage */
       if ( PMC -> CTRL & PMC_CTRL_SELLDOVOLTAGE_MASK )
           PMC -> LDOFLASHCORE = (ldo_flash_core_reg & (~PMC_LDOFLASHCORE_VADJ_2_MASK)) | ( ldoflashcore_prev_voltage << PMC_LDOFLASHCORE_VADJ_2_SHIFT );
       else
           PMC -> LDOFLASHCORE = (ldo_flash_core_reg & (~PMC_LDOFLASHCORE_VADJ_MASK))   | ( ldoflashcore_prev_voltage << PMC_LDOFLASHCORE_VADJ_SHIFT );

       for ( int i = delay_5us*11; i!=0 ; i--)
            __asm("nop");

       /* switch ON the clock back */
       SYSCON -> CLOCK_CTRL |= SYSCON_CLOCK_CTRL_FLASH48MHZ_ENA_MASK;
    }
#else
    /* Restore Flash controller clock */
    SYSCON -> CLOCK_CTRL |= SYSCON_CLOCK_CTRL_FLASH48MHZ_ENA_MASK;
#endif



    /* Power Up the flash (WAKE UP event) */
    FLASH->EVENT = FLASH_EVENT_WAKEUP(1);

#if 0
    __ISB();
    __DSB();
#endif

   /* Wait until Flash is powered up */
   while(!(FLASH-> INT_STATUS & FLASH_DONE ));

#if 0
   __asm("nop");
   __asm("nop");
   __asm("nop");
   __asm("nop");
#endif
}

#if gSwdWakeupReconnect_c
void SWD_spin_waiting_for_connection(void)
{
    static volatile int foo = 1;

    while (foo)
    {
    }
}
#endif

/*****************************************************************************
 *                             PUBLIC FUNCTIONS                              *
 *---------------------------------------------------------------------------*
 * Add to this section all the functions that have global (project) scope.   *
 * These functions can be accessed outside this module.                      *
 * These functions shall have their declarations (prototypes) within the     *
 * interface header file and shall be preceded by the 'extern' keyword.      *
 *---------------------------------------------------------------------------*
 *****************************************************************************/
void PWRLib_Init(void)
{
}

static uint32_t PWRLib_SetRamBanksRetention(void)
{
    uint32_t bank_mask = 0;
    register uint32_t stack_top;
    GET_MSP(stack_top);

    if ( bank_mask == 0 )
    {
        /*  retrieve HEAP limits */

        /* Set retention for all data from start of SRAM0 up to retention limit */
        /* Assess the RAM banks to be held in sleep mode based on _end_fw_retention
         * linker script symbol.
         * */
        bank_mask |= PWRLib_u32RamBanksSpanned(pm_sram0_bank_desc[0].start_address,
                                               (uint32_t)&_end_fw_retention - pm_sram0_bank_desc[0].start_address);

        /*
         * If the retention policy is to conserve RAM from bottom to end_of_fw_retention.
         */
        int32_t heap_sz = (heapEndAddr - heapStartAddr);

        if (heap_sz > 0)
        {
            bank_mask |= PWRLib_u32RamBanksSpanned(heapStartAddr, heap_sz);
        }
        /* SRAM0 bank 7 must be held in retention no matter how other data are mapped because the 32 bytes of
         * boot persistent data need to be preserved and it dwells at the top of bank7 @0x04015fe0.
         * Consequently the remainder of SRAM0 bank7 will be retained.
         * The top of the stack of the Startup Task (with RTOS) or of the bare metal thread,
         * is usually anchored at the bottom of the boot interface data @0x04015fe0 to grow downwards.
         * However, the implementer may chose to place this stack at the border of the preceding bank (bank6),
         * indeed only the top of the Startup task need to be retained. In that case, there would be a benefit
         * to place hand-picked data in the ~3.5kbyte space between the top of this stack and the bottom of the
         * boot persistent data.
         * The SP position at Power Down time need to be evaluated at run-time. So retention mask may be amended
         * later.
         * */
        bank_mask |= PWRLib_u32RamBanksSpanned(PWR_SRAM0_END-32, 32);

        /* PM_CFG_SRAM_ALL_RETENTION can be used to set all SRAM banks in retention */
#if gLoggingActive_d
        /* Usually the log ring is occupying the remainder of the available space in SRAM1 till
         * top. Anyhow it needs to be held in retention
         * */
        uint32_t log_ring_addr;
        uint32_t log_ring_sz;
        DbgLogGetStartAddrAndSize(&log_ring_addr, &log_ring_sz);

        if (log_ring_sz!= 0)
        {
            bank_mask |= PWRLib_u32RamBanksSpanned(log_ring_addr, log_ring_sz);
        }
#endif
        /* Need to keep the startup task's stack in retention down to current stack position */
        /* The most part of the retained mask has already been set at PWR_Init call but the stack
         * pointer might have gone past the bottom of the original bank */
        if (stack_top < PWR_SRAM0_BANK7_START_ADDR)
        {
            bank_mask |= PWRLib_u32RamBanksSpanned(stack_top, PWR_SRAM0_END-stack_top);
        }
    }
    return bank_mask;
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_SetDeepSleepMode
 * Description:
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWRLib_SetDeepSleepMode(uint8_t lpMode)
{
    mPWRLib_DeepSleepMode = lpMode;
#if gSupportBle
    if (lpMode == cPWR_DeepSleep_RamOffOsc32kOn ||
         lpMode == cPWR_PowerDown_RamRet ||
         lpMode == cPWR_DeepSleep_RamOffOsc32kOff)
        BLE_enable_sleep();
    else
        BLE_disable_sleep();
#endif
}

/*---------------------------------------------------------------------------
 * Name: PWRLib_GetDeepSleepMode
 * Description:
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
uint8_t PWRLib_GetDeepSleepMode(void)
{
    return mPWRLib_DeepSleepMode;
}

#define RXPWRUP_TIME(x) (141 - (x))
#define RTRIP_DELAY(x) (64 - (x))
#define TXPWRUP_TIME(x) (80 - x)

#if defined (gPWR_FreqScalingWFI) && (gPWR_FreqScalingWFI > 0 )

static void RadioTimingTweakForFrequencyScaling(void)
{
#define TXPWRDN_VAL 15 /* independent from frequency */
#if (gPWR_FreqScalingWFI == 24 ) || (gPWR_FreqScalingWFI == 32 )
    /* For WFI frequency scaling above 24MHz the radio timing settings do not
     * require adjustment  from original value
     * */
#define TX_PATH_ANTICIPATED (0U)
#define RX_PATH_ANTICIPATED (0U)
#elif (gPWR_FreqScalingWFI == 16)
#define TX_PATH_ANTICIPATED (4U)
#define RX_PATH_ANTICIPATED (2U)
#elif (gPWR_FreqScalingWFI == 12)
#define TX_PATH_ANTICIPATED (3U)
#define RX_PATH_ANTICIPATED (4U)
#elif (gPWR_FreqScalingWFI == 8)
#define TX_PATH_ANTICIPATED (3U)
#define RX_PATH_ANTICIPATED (8U)
#elif (gPWR_FreqScalingWFI == 6)
#define TX_PATH_ANTICIPATED (3U)
#define RX_PATH_ANTICIPATED (10U)
#elif (gPWR_FreqScalingWFI == 4)
#define TX_PATH_ANTICIPATED (3U)
#define RX_PATH_ANTICIPATED (16U)
#elif (gPWR_FreqScalingWFI == 2)
#define TX_PATH_ANTICIPATED (4U)
#define RX_PATH_ANTICIPATED (30U)
#endif
#if (gPWR_FreqScalingWFI < 24 )
    uint32_t val = (TXPWRUP_TIME(TX_PATH_ANTICIPATED) | (TXPWRDN_VAL << 8) |
            (RXPWRUP_TIME(RX_PATH_ANTICIPATED) << 16) | (RTRIP_DELAY(RX_PATH_ANTICIPATED) << 24));
    /* no need to change if WFI frequency is above 24 MHz */
    BLE_RADIOPWRUPDN_REG = val;
    //DbgLogAdd(__FUNCTION__ , "RADIOPWRUPDN=%x", 1, val);
#endif
}

static void SetAhbClkDiv(uint8_t div)
{
    SYSCON->AHBCLKDIV = SYSCON_AHBCLKDIV_REQFLAG_MASK | SYSCON_AHBCLKDIV_DIV((div-1));
#if 0
    /* might Need to wait if SysTick is maintained in WFI and need to be reprogrammed */
    while (SYSCON->AHBCLKDIV & SYSCON_AHBCLKDIV_REQFLAG_MASK)
    {
        __NOP();
    }
#endif
}

static void ChangeFrequencyBeforeWfi(void)
{
#if (gPWR_FreqScalingWFI == 12)
    /* FRO 12MHz */
    SYSCON -> MAINCLKSEL = SYSCON_MAINCLKSEL_SEL(BOARD_MAINCLK_FRO12M);
#elif (gPWR_FreqScalingWFI == 32)
    /* XTAL 32MHz */
    SYSCON -> MAINCLKSEL = SYSCON_MAINCLKSEL_SEL(BOARD_MAINCLK_XTAL32M);
#elif (gPWR_FreqScalingWFI == 16) || (gPWR_FreqScalingWFI == 8) ||  (gPWR_FreqScalingWFI == 4) ||  (gPWR_FreqScalingWFI == 2)
    SYSCON -> MAINCLKSEL = SYSCON_MAINCLKSEL_SEL(BOARD_MAINCLK_XTAL32M);
    SetAhbClkDiv(32/gPWR_FreqScalingWFI);
#elif (gPWR_FreqScalingWFI == 24) || (gPWR_FreqScalingWFI == 6)
    SYSCON -> MAINCLKSEL = SYSCON_MAINCLKSEL_SEL(BOARD_MAINCLK_FRO48M);
    SetAhbClkDiv(48/gPWR_FreqScalingWFI);
#endif
}

static void RestoreCoreFrequencyAfterSleep(uint32_t mainclksel, uint32_t ahbdiv)
{
    /* FRO 48MHz or 32MHz - the flash wait states were not changed during WFI */
    SYSCON->MAINCLKSEL       = mainclksel;

    /* Revert the Main Clock divisor to 1 */
    SetAhbClkDiv(ahbdiv+1);
}
#endif

/*---------------------------------------------------------------------------
 * Name: PWRLib_MCUEnter_Sleep
 * Description: Puts the processor into Sleep .

                Mode of operation details:
                 - ARM core enters Sleep Mode
                 - ARM core is clock gated (HCLK = OFF)
                 - peripherals are functional
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/

extern uint8_t mLPMFlag;
void PWRLib_MCU_Enter_Sleep(void)
{
    //PWR_DBG_LOG("");

    LpIoSet(0, 0);

#if defined (gPWR_FreqScalingWFI) && (gPWR_FreqScalingWFI != 0 )
    uint32_t save_MAINCLKSEL = SYSCON -> MAINCLKSEL;
    uint32_t save_AHBCLKDIV = SYSCON->AHBCLKDIV;
#if (gPWR_FreqScalingWFI < 24 )
    uint32_t save_radio_timing = BLE_RADIOPWRUPDN_REG;
    RadioTimingTweakForFrequencyScaling();
#endif
    ChangeFrequencyBeforeWfi();
#endif

#if gPWR_FlashControllerPowerDownInWFI
    if ( mLPMFlag==0 )
    {
#if ROM_API_CALL
        memset (&p_lowpower_cfg, 0, sizeof(LPC_LOWPOWER_T));

        p_lowpower_cfg.CFG   |= LOWPOWER_CFG_WFI_NOT_WFE_MASK;
        LpIoSet(3, 1);
        Chip_LOWPOWER_PowerCycleCpuAndFlash(&p_lowpower_cfg);
#else
        LpIoSet(3, 1);

        PWRLib_EnterSleep_WithFlashControllerPowerDown();
#endif
        LpIoSet(3, 0);
    }
    else
    {
        POWER_EnterSleep();
    }

#else
    POWER_EnterSleep();
#endif

#if defined gPWR_FreqScalingWFI && (gPWR_FreqScalingWFI == 4)
    SetAhbClkDiv(32/8);
#endif

#if defined gPWR_FreqScalingWFI && (gPWR_FreqScalingWFI > 0)
  #if gPWR_LdoVoltDynamic

    /* Need to increase LDO voltage before increasing CPU clock */
    POWER_ApplyLdoActiveVoltage_1V1();

    /* wait for LDO voltage to reach the voltage setting */
    /* This is still run at reduced frequency!
     *  CLOCK_uDelay is not usable at very slow clocks < 8MHz because the number of
     * instructions that is unrolls is huge. At 2MHz the 10us delay is only 20 CPU cycles.
     * DelayUsecMHz is merely a 4 cycle NOP loop. This yields a relatively good accuracy.
     * */
    //DelayUsecMHz(10U, gPWR_FreqScalingWFI );
    DelayLoopN( (10U * gPWR_FreqScalingWFI) / 4 );
  #endif

    RestoreCoreFrequencyAfterSleep(save_MAINCLKSEL, save_AHBCLKDIV);
  #if (gPWR_FreqScalingWFI < 24 )
    BLE_RADIOPWRUPDN_REG = save_radio_timing;
  #endif

  #if gPWR_LdoVoltDynamic
    POWER_ApplyLdoActiveVoltage_1V0();
  #endif
#endif

    LpIoSet(0, 1);
}
#if gSwdWakeupReconnect_c
bool swd_PD_deny = false;
void PWR_SWD_SetPowerDownAllowDeny(bool against)
{
    swd_PD_deny = against;
}

bool PWR_SWD_GetPowerDownDeny(void)
{
    return swd_PD_deny;
}
#endif
extern int error_status;
/*---------------------------------------------------------------------------
 * Name: PWRLib_UpdateWakeupReason
 * Description: Updates wakeup reason when exiting deep sleep
 * Parameters: -
 * Return: -
 *---------------------------------------------------------------------------*/
void PWR_UpdateWakeupReason(void)
{
    /* GPIO */
    uint32_t wakeup_io_source = POWER_GetIoWakeStatus();
    if (wakeup_io_source)
    {
#if gSwdWakeupReconnect_c
        if (wakeup_io_source == BIT(SWDIO_GPIO_PIN))
        {
            wakeup_io_source &= BIT(SWDIO_GPIO_PIN);
            SWD_spin_waiting_for_connection();
            PWR_SWD_SetPowerDownAllowDeny(true);
        }
#endif
        PWRLib_MCU_WakeupReason.Bits.FromKeyBoard = 1U;
        /* notify the GPIO adaptor so the user callback could be called */
        GpioNotifyIoWakeupSource(wakeup_io_source);
        PWR_DBG_LOG("WakeIOSource=%x", wakeup_io_source);
    }

    /* WTIMER */
    if (NVIC_GetPendingIRQ(WAKE_UP_TIMER0_IRQn) ||
        NVIC_GetPendingIRQ(WAKE_UP_TIMER1_IRQn))
    {
        PWR_DBG_LOG("WAKE_UP_TIMERx_IRQn");
        PWRLib_MCU_WakeupReason.Bits.FromWakeTimer = 1U;
    }

    /* RTC Interrupt */
    if (NVIC_GetPendingIRQ(RTC_IRQn))
    {
        /* RTC Second Interrupt */
        if (RTC->CTRL & RTC_CTRL_ALARM1HZ_MASK)
        {
            PWR_DBG_LOG("RTC_IRQn 1s");
            PWRLib_MCU_WakeupReason.Bits.FromRTC_Sec = 1U;
        }
        /* RTC Millisecond Interrupt */
        if (RTC->CTRL & RTC_CTRL_WAKE1KHZ_MASK)
        {
            PWR_DBG_LOG("RTC_IRQn 1ms");
            PWRLib_MCU_WakeupReason.Bits.FromRTC_mSec = 1U;
        }
    }

    /* Analog comparator 0 */
    if (NVIC_GetPendingIRQ(ANA_COMP_IRQn))
    {
        PWR_DBG_LOG("ANA_COMP_IRQn");
        PWRLib_MCU_WakeupReason.Bits.FromACmp0 = 1U;
    }

    /* USART 0 */
    if (NVIC_GetPendingIRQ(FLEXCOMM0_IRQn))
    {
        PWR_DBG_LOG("FLEXCOMM0_IRQn");
        PWRLib_MCU_WakeupReason.Bits.FromUSART0 = 1U;
    }

    /* BOD */
    if (NVIC_GetPendingIRQ(WDT_BOD_IRQn))
    {
        PWR_DBG_LOG("WDT_BOD_IRQn");
        PWRLib_MCU_WakeupReason.Bits.FromBOD = 1U;
    }

#if gSupportBle
    /* BLE */

#if 1
    int ret = -1;
    if(PWRLib_MCU_WakeupReason.Bits.DidPowerDown == 1U)
    {
        if ((PWRLib_MCU_WakeupReason.AllBits & ~0x8000U) == 0)
        {
            // No  wakeup reason, Suppose the LL is waking up, just need to wait
            PWRLib_MCU_WakeupReason.Bits.FromBLE_LLTimer = 1U;
            LpIoSet(4, 0);
            LpIoSet(4, 1);
        }
        else
        {
            /* there is already a wakeup reason, just check that LL wake event does not happen at the same time */
            if ( !NVIC_GetPendingIRQ( BLE_LL_ALL_IRQn) && !NVIC_GetPendingIRQ( BLE_WAKE_UP_TIMER_IRQn) )
            {
                LpIoSet(2, 1);
                 /* Force LL to be active if not */
extern int BLE_SetActive(void);
                ret = BLE_SetActive();
                LpIoSet(2, 0);

                /*    0: SW flag shows LL has not been to DSM
                      1: LL is being waking up,
                      2: LL is still asleep */
                if ( ret == 1 )
                {
                    PWRLib_MCU_WakeupReason.Bits.FromBLE_LLTimer = 1U;
                }

                if ( ret < 1 )
                {
                    PWR_DBG_LOG("err: ret=%d ", ret);
                    error_status = -1;
                }
            }
        }
    }

    int count_down = 5000;
    LpIoSet(3, 1);
    while (!NVIC_GetPendingIRQ( BLE_LL_ALL_IRQn) && (count_down>0))
    {
        LpIoSet(3, 0);
        LpIoSet(3, 1);
        count_down--;
    }
    LpIoSet(3, 0);

    /* Enable BLE Interrupt */
    NVIC_SetPriority(BLE_LL_ALL_IRQn, 0x01U);
    NVIC_EnableIRQ(BLE_LL_ALL_IRQn);

    NVIC_ClearPendingIRQ(BLE_WAKE_UP_TIMER_IRQn);

    if (count_down !=0)
    {
extern void BLE_LL_ALL_IRQHandler(void);
        for(int i=3; i>0; i--)
        {
            BLE_LL_ALL_IRQHandler();
        }
    }
    else
    {
        PWR_DBG_LOG("err: %d / %d", ret, count_down);
        error_status = -2;
    }



#else
    int ret = -1;
    if (  /* NVIC_GetPendingIRQ( BLE_LL_ALL_IRQn) ||*/ NVIC_GetPendingIRQ( BLE_WAKE_UP_TIMER_IRQn))
    {
        PWR_DBG_LOG("BLE_WAKE_UP_TIMER_IRQn");
        NVIC_ClearPendingIRQ(BLE_WAKE_UP_TIMER_IRQn);
        PWRLib_MCU_WakeupReason.Bits.FromBLE_LLTimer = 1U;
    }
    else
    {
        /* Force LL to be active if not */
        ret = BLE_SetActive();

        /*    0: SW flag shows LL has not been to DSM
              1: LL is being waking up,
              2: LL is still asleep */
        if ( ret < 1 )
        {
            PWR_DBG_LOG("err: ret=%d ", ret);
            error_status = -1;
        }
    }
#if 0
    if (NVIC_GetPendingIRQ( BLE_WAKE_UP_TIMER_IRQn))
    {
        PWR_DBG_LOG("BLE_WAKE_UP_TIMER_IRQn");
        PWRLib_MCU_WakeupReason.Bits.FromBLE_LLTimer = 1U;
        NVIC_ClearPendingIRQ(BLE_WAKE_UP_TIMER_IRQn);
    }
#endif
    /* Enable BLE Interrupt */
    NVIC_SetPriority(BLE_LL_ALL_IRQn, 0x01U);
    NVIC_EnableIRQ(BLE_LL_ALL_IRQn);

    /* wakeup of BLE LL shall always happen here
          in Case of Async wakeup, this will take longer but we can wait */
    LpIoSet(3, 1);
    int count_down = 5000;
    while (!NVIC_GetPendingIRQ( BLE_LL_ALL_IRQn) && (count_down>0))
    {
        LpIoSet(3, 0);
        LpIoSet(3, 1);
        count_down--;
    }
    LpIoSet(3, 0);

    if (count_down !=0)
    {
extern void BLE_LL_ALL_IRQHandler(void);
        BLE_LL_ALL_IRQHandler();
    }
    else
    {
        PWR_DBG_LOG("err: %d / %d", ret, count_down);
        error_status = -2;
    }

    /* We may have peeked the NVIC Pending IRQ too early:
       The duration of the application Wakeup call back participates to that delay but
       60us at least are required after the end of hardware_init for the IRQ to raise.
       Keep the DidPowerDown bit so that the caller may know whether specific action is to be
       undertaken.
     */
    if(PWRLib_MCU_WakeupReason.Bits.DidPowerDown == 1U)
    {
        if ((PWRLib_MCU_WakeupReason.AllBits & ~0x8000U) == 0)
        {
            /* most of the time, LL has not woken up yet so we usually see no wakeup reason when LL timer expires */
            //error_status = +3;
        }
    }
#endif
#endif
}

void PWR_ClearWakeupReason(void)
{
    PWRLib_MCU_WakeupReason.AllBits = 0UL;
}


PWR_WakeupReason_t PWR_GetWakeupReason(void)
{
    return PWRLib_MCU_WakeupReason;
}


void WarmMain(void)
{
    /* Interrupts should be disabled */
    __disable_irq();
#if defined(USE_RTOS) && (USE_RTOS)
    /* Switch to the psp stack */
    asm volatile("movs r0, #2");
    asm volatile("msr CONTROL, r0");
 #endif
    // Restore context
    PWR_longjmp(pwr_CPUContext, TRUE);
}

uint32_t lowpower_power_cfg[POWER_LOWPOWER_STRUCTURE_SIZE];
void *lowpower_power_cfg_p = NULL;

void PWR_ResetPowerDownModeConfig(void)
{
    /* set to NULL to be recalculate next time */
    lowpower_power_cfg_p = NULL;
}

void PWR_SetWakeUpConfig(uint32_t set_msk, uint32_t clr_msk);
uint32_t PWR_GetWakeUpConfig(void);
void vSetWakeUpIoConfig(void);

static void PWRLib_LLDsmComplete(void)
{
#if gSupportBle
#ifndef PWR_DisableLateBleIsolateModem
    BLE_IsolateModem();
#endif
#endif

#if gPWR_LdoVoltDynamic
    /* Apply default LDO voltage */
    POWER_ApplyLdoActiveVoltage_1V1();
#endif

#if gSupportBle
#ifndef gPWR_LpOscOptim
    /* Waiting for MODEMSTATUS.BLE_LP_OSC_EN bit to drop is apparently not necessary,
     * this will happen anyway. SYSCON are retained during powerdown
     * waiting is around 30us
     * */
    BLE_WaitForLpOscReady();
#endif
#endif
}

void PWRLib_EnterPowerDownModeRamOn(uint32_t mode)
{
    //PWR_DBG_LOG("");
    pm_power_config_t power_config;
    uint32_t pwrlib_pm_ram_config;

#if !defined(gPWR_LpEntryStructOptim) || (gPWR_LpEntryStructOptim == 0)
    lowpower_power_cfg_p = NULL;
#endif

    if ( lowpower_power_cfg_p == NULL )
    {
        pwrlib_pd_cfg_t pd_cfg;
        lowpower_power_cfg_p    = (void*)&lowpower_power_cfg;

        memset(&pd_cfg, 0x0, sizeof(pwrlib_pd_cfg_t));

        /* get the IO wakeup configuration */
        vSetWakeUpIoConfig();

        if (mode & PWR_CFG_MAINTAIN_AO_LDO)
        {
            pd_cfg.sleep_cfg |= PM_CFG_KEEP_AO_VOLTAGE;
        }

        pd_cfg.wakeup_io  = PWR_GetWakeUpConfig();
        pd_cfg.wakeup_src = u64GetWakeupSourceConfig(mode);
        pd_cfg.sleep_cfg  |= PWR_u32GetRamRetention();

#if defined(gPWR_ForceWakeTimer0_WakeupSrc) && (gPWR_ForceWakeTimer0_WakeupSrc == 1)
        pd_cfg.wakeup_src |= POWER_WAKEUPSRC_WAKE_UP_TIMER0;
#endif
#if defined(gPWR_ForceWakeTimer1_WakeupSrc) && (gPWR_ForceWakeTimer1_WakeupSrc == 1)
        pd_cfg.wakeup_src |= POWER_WAKEUPSRC_WAKE_UP_TIMER1;
#endif
#if defined(gPWR_ForceRtc_WakeupSrc) && (gPWR_ForceRtc_WakeupSrc == 1)
        pd_cfg.wakeup_src |= POWER_WAKEUPSRC_RTC;
#endif

        power_config.pm_wakeup_src = pd_cfg.wakeup_src;

        pwrlib_pm_ram_config = PWRLib_SetRamBanksRetention();

        pd_cfg.sleep_cfg |= pwrlib_pm_ram_config;

#if gSupportBle
        power_config.pm_wakeup_src |= (POWER_WAKEUPSRC_BLE_OSC_EN
                                       | POWER_WAKEUPSRC_RTC ); /* POWER_WAKEUPSRC_BLE_WAKE_TIMER */

        /* BLE always assumes RFP retention registers retained */
#if (gPWR_Xtal32MDeactMode == 2)
        power_config.pm_config = pd_cfg.sleep_cfg | PM_CFG_RADIO_RET;
#else
        power_config.pm_config = pd_cfg.sleep_cfg | PM_CFG_XTAL32M_AUTOSTART  | PM_CFG_RADIO_RET;
#endif

#else
        power_config.pm_config = pd_cfg.sleep_cfg | PM_CFG_XTAL32M_AUTOSTART;
#endif

        power_config.pm_wakeup_io           = pd_cfg.wakeup_io & (BIT(22)-1) ;
        if (power_config.pm_wakeup_io)
        {
            power_config.pm_wakeup_src        |=  POWER_WAKEUPSRC_IO;
        }

        POWER_GetPowerDownConfig(&power_config, lowpower_power_cfg_p);
        POWER_RegisterPowerDownEntryHookFunction( PWRLib_LLDsmComplete );
    }

    BOOT_SetResumeStackPointer(RESUME_STACK_POINTER);

    /* on application warm start, prevent the application from corrupting the current stack */
    SYSCON->CPSTACK = RESUME_STACK_POINTER;

    /* Set low power flag in wakeup source */
    PWRLib_MCU_WakeupReason.Bits.DidPowerDown = 1;

    if ( 0 == PWR_setjmp(pwr_CPUContext))
    {
        POWER_GoToPowerDown( lowpower_power_cfg_p );
    }
}

void PWRLib_EnterPowerDownModeRamOff(pwrlib_pd_cfg_t *pd_cfg)
{
    PWR_DBG_LOG("");
    pm_power_config_t power_config;

    power_config.pm_wakeup_src = pd_cfg->wakeup_src;
    power_config.pm_config = pd_cfg->sleep_cfg | PM_CFG_XTAL32M_AUTOSTART;
    power_config.pm_wakeup_io           = pd_cfg->wakeup_io & (BIT(22)-1) ;
    if (power_config.pm_wakeup_io)
    {
        power_config.pm_wakeup_src         |=  POWER_WAKEUPSRC_IO;
    }

    POWER_EnterPowerMode( PM_POWER_DOWN, &power_config );
}

void PWRLib_EnterDeepDownMode(pwrlib_pd_cfg_t *pd_cfg)
{
    PWR_DBG_LOG("");
    pm_power_config_t power_config;

    power_config.pm_wakeup_src = pd_cfg->wakeup_src;
    power_config.pm_config = pd_cfg->sleep_cfg | PM_CFG_XTAL32M_AUTOSTART;
    power_config.pm_wakeup_io           = pd_cfg->wakeup_io & (BIT(22)-1) ;
    if (power_config.pm_wakeup_io)
    {
        power_config.pm_wakeup_src         |=  POWER_WAKEUPSRC_IO;
    }

#if gPWR_LdoVoltDynamic
    /* Apply default LDO voltage */
    POWER_ApplyLdoActiveVoltage_1V1();
#endif

    POWER_EnterPowerMode( PM_DEEP_DOWN, &power_config );
}

uint32_t PWRLib_u32RamBanksSpanned(uint32_t u32Start, uint32_t u32Length)
{
    uint32_t u32RamBanks = 0;
    uint32_t u32End = u32Start + u32Length - 1U;
    int iStartBank = 0;
    int iEndBank = 0;

    /* Bounds checking: returns 0 if any of these tests fail */
    if (   (u32Start  >= PWR_SRAM0_BASE)
        && (u32Start  <  PWR_SRAM0_END)
        && (u32End    >= PWR_SRAM0_BASE)
        && (u32End    <  PWR_SRAM0_END)
        && (u32Length >  0U)
       )
    {
        /* Assess the RAM bank(s) spanned by the specified RAM range.
           Loops result in iStartBank and iEndBank having values between
           1 and 8, for banks 0 to 7 */
        iStartBank = 1U;

        while ((iStartBank < sizeof(pm_sram0_bank_desc)/sizeof(sram_bank_desc_t))
                && (pm_sram0_bank_desc[iStartBank].start_address <= u32Start))
        {
            iStartBank++;
        }

        iEndBank = iStartBank;
        while ((iEndBank < sizeof(pm_sram0_bank_desc)/sizeof(sram_bank_desc_t))
                && (pm_sram0_bank_desc[iEndBank].start_address <= u32End))
        {
            iEndBank++;
        }


    }
    else if (   (u32Start  >= PWR_SRAM1_BASE)
                && (u32Start  <  PWR_SRAM1_END)
                && (u32End    >= PWR_SRAM1_BASE)
                && (u32End    <  PWR_SRAM1_END)
                && (u32Length >  0U)
       )
    {
        /* Assess the RAM bank(s) spanned by the specified RAM range,
           if they belong to SRAM1
           Loops result in iStartBank and iEndBank having values between
           1 and 4 */
        iStartBank = 1U;

        while ((iStartBank < sizeof(pm_sram1_bank_desc)/sizeof(sram_bank_desc_t))
                && (pm_sram1_bank_desc[iStartBank].start_address <= u32Start))
        {
            iStartBank++;
        }

        iEndBank = iStartBank;
        while ((iEndBank < sizeof(pm_sram1_bank_desc)/sizeof(sram_bank_desc_t))
                && (pm_sram1_bank_desc[iEndBank].start_address <= u32End))
        {
            iEndBank++;
        }

        /* The shift for the mask starts after SRAM0 */
        iEndBank += sizeof(pm_sram0_bank_desc)/sizeof(sram_bank_desc_t);
        iStartBank += sizeof(pm_sram0_bank_desc)/sizeof(sram_bank_desc_t);

    }
    if (iStartBank != 0)
    {
        /* Create bitmask with bits (iEndBank-1):(iStartBank-1) set */
        u32RamBanks = (1U << iEndBank) - 1U;
        u32RamBanks ^= (1U << (iStartBank - 1U)) - 1U;
    }

    return u32RamBanks;
}
