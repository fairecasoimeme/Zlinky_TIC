/*
* Copyright 2019-2020 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "fsl_debug_console.h"
#include "fsl_common.h"
#include "fsl_clock.h"
#include "fsl_power.h"
#include "fsl_gpio.h"
#include "fsl_flash.h"
#include "fsl_os_abstraction.h"

#if gSupportBle
#include "fsl_xcvr.h"
#endif

#include "board.h"
#include "board_utility.h"
#include "FunctionLib.h"

#include "fsl_adc.h"
#include "RNG_Interface.h"
#include "Panic.h"
#include "rom_api.h"
#include "rom_psector.h"
#include "gpio_pins.h"
#include "GPIO_Adapter.h"
#include "Keyboard.h"
#include "pin_mux.h"
#include "radio.h"

#include "fsl_spifi.h"

#if gOtaEepromPostedOperations_d
#include "OtaSupport.h"
#endif

#ifndef gLogRingPlacementOffset_c
#define gLogRingPlacementOffset_c 0
#endif

#if defined (gMWS_UseCoexistence_d) && (gMWS_UseCoexistence_d)
#include "MWS.h"
#endif

#include "TimersManager.h"

extern void BOARD_InitClocks(void);
extern void PWR_UpdateWakeupReason(void);

/*******************************************************************************
 * Code
 ******************************************************************************/

#define BLE_MACID_SZ                      6
#define MANUFACTURER_BLE_MACID_ADRESS     (const uint8_t*)(0x9fc00 + 0x100)
#define gBD_ADDR_NXP_OUI_c                 0x00, 0x60, 0x37

#define BLE_BASE_ADDR                     0x400A0000
#define BLE_DIAGCNTL_OFFSET               0x00000050
#define BLE_DIAGSTAT_OFFSET               0x00000054
#define REG_BLE_RD(offs)             (*(volatile uint32_t *)(BLE_BASE_ADDR+(offs)))
#define REG_BLE_WR(offs, val)        (*(volatile uint32_t *)(BLE_BASE_ADDR+(offs))) = (val);

#define SYSCON_CPUCTRL               (*(volatile uint32_t *)(0x40000800))

/*******************************************************************************
* Local variables
******************************************************************************/
#if defined (gMWS_UseCoexistence_d) && (gMWS_UseCoexistence_d)
static gpioOutputPinConfig_t rfActivePin = {
    .gpioPort = gpioPort_A_c,
    .gpioPin = 15,
    .outputLogic = 1,
    .slewRate = pinSlewRate_Slow_c,
    .driveStrength = pinDriveStrength_Low_c,
};
static gpioOutputPinConfig_t rfStatusPin = {
    .gpioPort = gpioPort_A_c,
    .gpioPin = 16,
    .outputLogic = 1,
    .slewRate = pinSlewRate_Slow_c,
    .driveStrength = pinDriveStrength_Low_c,

};
static  gpioInputPinConfig_t rfDenyPin = {
    .gpioPort = gpioPort_A_c,
    .gpioPin = 14,
    .pullSelect = pinPull_Disabled_c,
    .interruptModeSelect = pinInt_EitherEdge_c,
    .pinIntSelect = kPINT_PinInt2,
    .inputmux_attach_id = kINPUTMUX_GpioPort0Pin14ToPintsel,
    .is_wake_source = FALSE,
};
#endif

#if gAdcUsed_d
static adc_config_t adcConfigStruct;
static adc_conv_seq_config_t adcConvSeqConfigStruct;

/*  set if adc measurements are done */
static bool adc_measure_done = false;
#endif

#if (gKBD_KeysCount_c > 0)
const gpioInputPinConfig_t dk6_button_io_pins[] = {
    [0] = {
        .gpioPort = gpioPort_A_c,
        .gpioPin = BOARD_USER_BUTTON1_GPIO_PIN,
        .pullSelect = pinPull_Disabled_c,
        .interruptModeSelect = pinInt_FallingEdge_c,
        .pinIntSelect = kPINT_PinInt0,
#ifdef OM15082
        .inputmux_attach_id = kINPUTMUX_GpioPort0Pin19ToPintsel,
#else
        .inputmux_attach_id = kINPUTMUX_GpioPort0Pin1ToPintsel,
#endif
        .is_wake_source = TRUE,
    },
    #if (gKBD_KeysCount_c > 1)
    [1] = {
        .gpioPort = gpioPort_A_c,
        .gpioPin = BOARD_USER_BUTTON2_GPIO_PIN,
        .pullSelect = pinPull_Disabled_c,
        .interruptModeSelect = pinInt_FallingEdge_c,
        .pinIntSelect = kPINT_PinInt1,
#ifdef OM15082
        .inputmux_attach_id = kINPUTMUX_GpioPort0Pin15ToPintsel,
#else
        .inputmux_attach_id = kINPUTMUX_GpioPort0Pin5ToPintsel,
#endif
        .is_wake_source = TRUE,
    },
    #endif
    #if (gKBD_KeysCount_c > 2)
    [2] = {
        .gpioPort = gpioPort_A_c,
        .gpioPin = BOARD_USER_BUTTON3_GPIO_PIN,
        .pullSelect = pinPull_Disabled_c,
        .interruptModeSelect = pinInt_FallingEdge_c,
        .pinIntSelect = kPINT_PinInt2,
        .inputmux_attach_id = kINPUTMUX_GpioPort0Pin7ToPintsel,
        .is_wake_source = TRUE,
    },
    #endif
    #if (gKBD_KeysCount_c > 3)
    [3] = {
        .gpioPort = gpioPort_A_c,
        .gpioPin = BOARD_USER_BUTTON4_GPIO_PIN,
        .pullSelect = pinPull_Disabled_c,
        .interruptModeSelect = pinInt_FallingEdge_c,
        .pinIntSelect = kPINT_PinInt3,
        .inputmux_attach_id = kINPUTMUX_GpioPort0Pin4ToPintsel,
        .is_wake_source = TRUE,
    },
    #endif
};

const iocon_group_t dk6_buttons_io[] = {
      [0] = {
          .port = 0,
          .pin =  BOARD_USER_BUTTON1_GPIO_PIN,
          .modefunc =  IOCON_PIO_FUNC(IOCON_USER_BUTTON_MODE_FUNC)  | IOCON_MODE_PULLUP | IOCON_DIGITAL_EN,
      },
    #if (gKBD_KeysCount_c > 1)
      [1] = {
          .port = 0,
          .pin =  BOARD_USER_BUTTON2_GPIO_PIN,
          .modefunc =  IOCON_PIO_FUNC(IOCON_USER_BUTTON_MODE_FUNC)  | IOCON_MODE_PULLUP | IOCON_DIGITAL_EN,
      },
    #endif
    #if (gKBD_KeysCount_c > 2)
      [2] = {
          .port = 0,
          .pin =  BOARD_USER_BUTTON3_GPIO_PIN,
          .modefunc =  IOCON_PIO_FUNC(IOCON_USER_BUTTON_MODE_FUNC)  | IOCON_MODE_PULLUP | IOCON_DIGITAL_EN,
      },
    #endif
  #if (gKBD_KeysCount_c > 3)
      [3] = {
          .port = 0,
          .pin =  BOARD_USER_BUTTON4_GPIO_PIN,
          .modefunc =  IOCON_PIO_FUNC(IOCON_USER_BUTTON_MODE_FUNC)  | IOCON_MODE_PULLUP | IOCON_DIGITAL_EN,
      },
    #endif
 };
#endif

#if (gLEDsOnTargetBoardCnt_c > 0)
const gpioOutputPinConfig_t dk6_leds_io_pins[] = {
        [0] = { .gpioPort = gpioPort_A_c,
                .gpioPin = BOARD_LED_RED1_GPIO_PIN,
                .outputLogic = 1,                     /* negated output 0 :  means switch on */
                .slewRate = pinSlewRate_Slow_c,
                .driveStrength = pinDriveStrength_Low_c,
        },
#if (gLEDsOnTargetBoardCnt_c > 1)
        [1] = { .gpioPort = gpioPort_A_c,
                .gpioPin = BOARD_LED_RED2_GPIO_PIN,
                .outputLogic = 1,                     /* negated output 0 :  means switch on */
                .slewRate = pinSlewRate_Slow_c,
                .driveStrength = pinDriveStrength_Low_c,
        },
#endif
#if (gLEDsOnTargetBoardCnt_c > 2)
        [2] = { .gpioPort = gpioPort_A_c,
                .gpioPin = BOARD_LED_RED3_GPIO_PIN,
                .outputLogic = 1,                     /* negated output 0 :  means switch on */
                .slewRate = pinSlewRate_Slow_c,
                .driveStrength = pinDriveStrength_Low_c,
        },
#endif
#if (gLEDsOnTargetBoardCnt_c > 3)
        [3] = { .gpioPort = gpioPort_A_c,
                .gpioPin = BOARD_LED_RED4_GPIO_PIN,
                .outputLogic = 1,                     /* negated output 0 :  means switch on */
                .slewRate = pinSlewRate_Slow_c,
                .driveStrength = pinDriveStrength_Low_c,
        }
#endif
};


const iocon_group_t dk6_leds_io[] = {
#if (gLEDsOnTargetBoardCnt_c > 0)
     [0] = {
         .port = 0,
         .pin =  BOARD_LED_RED1_GPIO_PIN,
         .modefunc =  IOCON_PIO_FUNC(IOCON_LED_MODE_FUNC)  | IOCON_MODE_INACT | IOCON_DIGITAL_EN,
     },
#endif
#if (gLEDsOnTargetBoardCnt_c > 1)
     [1] = {
         .port = 0,
         .pin =  BOARD_LED_RED2_GPIO_PIN,
         .modefunc =  IOCON_PIO_FUNC(IOCON_LED_MODE_FUNC)  | IOCON_MODE_INACT | IOCON_DIGITAL_EN,
     },
#endif
#if (gLEDsOnTargetBoardCnt_c > 2)
     [2] = {
         .port = 0,
         .pin =  BOARD_LED_RED3_GPIO_PIN,
         .modefunc =  IOCON_PIO_FUNC(IOCON_LED_MODE_FUNC)  | IOCON_MODE_INACT | IOCON_DIGITAL_EN,
     },
#endif
#if (gLEDsOnTargetBoardCnt_c > 3)
     [3] = {
         .port = 0,
         .pin =  BOARD_LED_RED4_GPIO_PIN,
         .modefunc =  IOCON_PIO_FUNC(IOCON_LED_MODE_FUNC)  | IOCON_MODE_INACT | IOCON_DIGITAL_EN,
     }
#endif
};

const gpio_pin_config_t dk6_leds_gpio[] = {
#if (gLEDsOnTargetBoardCnt_c > 0)
         [0] = { .pinDirection = kGPIO_DigitalOutput, .outputLogic = 1 },
#endif
#if (gLEDsOnTargetBoardCnt_c > 1)
         [1] = { .pinDirection = kGPIO_DigitalOutput, .outputLogic = 1 },
#endif
#if (gLEDsOnTargetBoardCnt_c > 2)
         [2] = { .pinDirection = kGPIO_DigitalOutput, .outputLogic = 1 },
#endif
#if (gLEDsOnTargetBoardCnt_c > 3)
         [3] = { .pinDirection = kGPIO_DigitalOutput, .outputLogic = 1 },
#endif
};
#endif

#if gDbgUseDbgIos
#if (gDbgIoCfg_c == 1) || (gDbgIoCfg_c == 2)


#define IOCON_DBGIO_MODE_FUNC 0

const gpioOutputPinConfig_t dk6_dbg_io_pins[] = {
#if NB_DBG_IO >= 1
        [0] = { .gpioPort = 0,
                .gpioPin = IOCON_DBG_PIN,
                .outputLogic = 1,
                .slewRate = pinSlewRate_Slow_c,
                .driveStrength = pinDriveStrength_Low_c,
        },
#if NB_DBG_IO >= 2
        [1] = { .gpioPort = 0,
                .gpioPin = IOCON_DBG_PIN+1,
                .outputLogic = 1,
                .slewRate = pinSlewRate_Slow_c,
                .driveStrength = pinDriveStrength_Low_c,
        },
#if NB_DBG_IO >= 3

        [2] = { .gpioPort = 0,
                .gpioPin = IOCON_DBG_PIN+2,
                .outputLogic = 1,
                .slewRate = pinSlewRate_Slow_c,
                .driveStrength = pinDriveStrength_Low_c,
        },
#if NB_DBG_IO >= 4
        [3] = { .gpioPort = 0,
                .gpioPin = IOCON_DBG_PIN+3,
                .outputLogic = 1,
                .slewRate = pinSlewRate_Slow_c,
                .driveStrength = pinDriveStrength_Low_c,
        },
#if NB_DBG_IO >= 5
        [4] = { .gpioPort = 0,
                .gpioPin = IOCON_DBG_PIN+4,
                .outputLogic = 1,
                .slewRate = pinSlewRate_Slow_c,
                .driveStrength = pinDriveStrength_Low_c,
        },
#endif
#endif
#endif
#endif
#endif
};


const iocon_group_t dk6_dbg_io[] = {
#if NB_DBG_IO >= 1
      [0] = {
          .port = 0,
          .pin =  IOCON_DBG_PIN,
          .modefunc =  IOCON_PIO_FUNC(IOCON_DBGIO_MODE_FUNC)  | IOCON_MODE_PULLUP | IOCON_DIGITAL_EN,
      },
#if NB_DBG_IO >= 2
      [1] = {
          .port = 0,
          .pin =  IOCON_DBG_PIN+1,
          .modefunc =  IOCON_PIO_FUNC(IOCON_DBGIO_MODE_FUNC)  | IOCON_MODE_PULLUP | IOCON_DIGITAL_EN,
      },
#if NB_DBG_IO >= 3
      [2] = {
          .port = 0,
          .pin =  IOCON_DBG_PIN+2,
          .modefunc =  IOCON_PIO_FUNC(IOCON_DBGIO_MODE_FUNC)  | IOCON_MODE_PULLUP | IOCON_DIGITAL_EN,
      },
#if NB_DBG_IO >= 4
      [3] = {
          .port = 0,
          .pin =  IOCON_DBG_PIN+3,
          .modefunc =  IOCON_PIO_FUNC(IOCON_DBGIO_MODE_FUNC)  | IOCON_MODE_PULLUP | IOCON_DIGITAL_EN,
      },
#if NB_DBG_IO >= 5
      [4] = {
          .port = 0,
          .pin =  IOCON_DBG_PIN+4,
          .modefunc =  IOCON_PIO_FUNC(IOCON_DBGIO_MODE_FUNC)  | IOCON_MODE_PULLUP | IOCON_DIGITAL_EN,
      }
#endif
#endif
#endif
#endif
#endif
 };
#endif
#endif

static bool mUartPinsInit[FSL_FEATURE_SOC_USART_COUNT] = {0};

/*****************************************************************************
* Local functions
****************************************************************************/

bool BOARD_IsADCEnabled(void);

/*******************************************************************************
 * Public variables
 *******************************************************************************/

jmp_buf  *exception_buf;

/*******************************************************************************
 * Private functions
 *******************************************************************************/

#if(ADC_TEMP_SENSOR_DRIVER_EN)
static void ADC_ClockPower_Configuration(void)
{
    /* Enable ADC clock */
    CLOCK_EnableClock(kCLOCK_Adc0);

    //CLOCK_EnableClock(kCLOCK_PVT);

    /* Power on the ADC converter. */
    POWER_EnablePD(kPDRUNCFG_PD_LDO_ADC_EN);

    /* Enable the clock. */
    CLOCK_AttachClk(kXTAL32M_to_ADC_CLK);
}

static bool ADC_Configuration(ADC_Type *base)
{

    ADC_ClockPower_Configuration();

    int valid = 1;
#if 1
    /* Configure the converter. */
    adcConfigStruct.clockMode = kADC_ClockAsynchronousMode; /* Using async clock source. */
    adc_clock_src_t adc_clk_src = (adc_clock_src_t)((SYSCON->ADCCLKSEL) & SYSCON_ADCCLKSEL_SEL_MASK >> SYSCON_ADCCLKSEL_SEL_SHIFT);
    switch(adc_clk_src)
    {
        case kCLOCK_AdcXtal32M:
        {
            CLOCK_SetClkDiv(kCLOCK_DivAdcClk, 8, true);
            break;
        }
        case kCLOCK_AdcFro12M:
        {
            CLOCK_SetClkDiv(kCLOCK_DivAdcClk, 3, true);
            break;
        }
        case kCLOCK_AdcNoClock:
        default:
        {
            valid = 0;
            PRINTF("ADC configuration error: no clock selected in asynchronous mode\r\n");
            break;
        }
    }

    if (!valid)
    {
        return valid;
    }
#endif

    /* Configure the converter. */
    //adcConfigStruct.clockMode = kADC_ClockSynchronousMode; /* Using sync clock source. */
    adcConfigStruct.resolution = kADC_Resolution12bit;
#if defined(FSL_FEATURE_ADC_HAS_CTRL_BYPASSCAL) & FSL_FEATURE_ADC_HAS_CTRL_BYPASSCAL
    adcConfigStruct.enableBypassCalibration = false;
#endif/* FSL_FEATURE_ADC_HAS_CTRL_BYPASSCAL. */
    adcConfigStruct.sampleTimeNumber = 0U;
    ADC_Init(base, &adcConfigStruct);

    /* Enable conversion in Sequence A. */
    /* Channel to be used is set up at the beginning of each sequence */
    adcConvSeqConfigStruct.channelMask = 0;
    adcConvSeqConfigStruct.triggerMask = 0U;
    adcConvSeqConfigStruct.triggerPolarity = kADC_TriggerPolarityPositiveEdge;
    adcConvSeqConfigStruct.enableSingleStep = false;
    adcConvSeqConfigStruct.enableSyncBypass = false;
    adcConvSeqConfigStruct.interruptMode = kADC_InterruptForEachSequence;
    ADC_SetConvSeqAConfig(base, &adcConvSeqConfigStruct);

    return valid;
}

#endif    /* #if(ADC_TEMP_SENSOR_DRIVER_EN) */

#if gAdcUsed_d
/*
 * ISR for ADC conversion sequence A done.
 */
bool BOARD_IsADCEnabled(void)
{
    bool result = false;
    if(ADC0->STARTUP & ADC_STARTUP_ADC_ENA_MASK)
    {
        result = true;
    }
    return result;
}

/*!
 * @brief Set ADC channel.
 *
 * @param number Channel to be set.
 * @param enable Enable or disable the ADC channel.
 */
static void BOARD_ADCChannelSet(uint32_t number, bool enable)
{
    if (enable == true)
    {
        adcConvSeqConfigStruct.channelMask |= (1 << number);
    }
    else
    {
        adcConvSeqConfigStruct.channelMask &= ~(1 << number);
    }
    ADC_SetConvSeqAConfig(ADC0, &adcConvSeqConfigStruct);
    ADC_EnableConvSeqA(ADC0, true);
}
#endif

static void FaultRecovery(void)
{
    if (exception_buf != NULL)
    {
        longjmp(*exception_buf, BUS_EXCEPTION);
    }
    while(1);
}

#define LP_ANALOG_PULLDWN_CFG IOCON_PIO_FUNC(0) |\
                              IOCON_PIO_MODE(3) |\
                              IOCON_PIO_DIGIMODE(0)

#define LP_DIGITAL_PULLDWN_CFG IOCON_PIO_FUNC(0) |\
                              IOCON_PIO_MODE(3) |\
                              IOCON_PIO_DIGIMODE(1)

#define LP_DIGITAL_PULLUP_CFG IOCON_PIO_FUNC(0) |\
                              IOCON_PIO_MODE(0) |\
                              IOCON_PIO_DIGIMODE(1)

#define USART1_TX_MODE                                                  \
    IOCON_PIO_FUNC(2) |                                                 \
    /* GPIO mode */                                                     \
    IOCON_PIO_EGP_GPIO |                                                \
    /* IO is an open drain cell */                                      \
    IOCON_PIO_ECS_DI |                                                  \
    /* High speed IO for GPIO mode, IIC not */                          \
    IOCON_PIO_EHS_DI |                                                  \
    /* Input function is not inverted */                                \
    IOCON_PIO_INV_DI |                                                  \
    /* Enables digital function */                                      \
    IOCON_PIO_DIGITAL_EN |                                              \
    /* Input filter disabled */                                         \
    IOCON_PIO_INPFILT_OFF |                                             \
    /* IIC mode:Noise pulses below approximately */                     \
    /* 50ns are filtered out. GPIO mode:a 3ns filter */                 \
    IOCON_PIO_FSEL_DI |                                                 \
    /* Open drain is disabled */                                        \
    IOCON_PIO_OPENDRAIN_DI |                                            \
    /* IO_CLAMP disabled */                                             \
    IOCON_PIO_IO_CLAMP_DI


#define USART1_RX_MODE                                                  \
    IOCON_PIO_FUNC2 |                                                   \
    /* GPIO mode */                                                     \
    IOCON_PIO_EGP_GPIO |                                                \
    /* IO is an open drain cell */                                      \
    IOCON_PIO_ECS_DI |                                                  \
    /* High speed IO for GPIO mode, IIC not */                          \
    IOCON_PIO_EHS_DI |                                                  \
    /* Input function is not inverted */                                \
    IOCON_PIO_INV_DI |                                                  \
    /* Enables digital function */                                      \
    IOCON_PIO_DIGITAL_EN |                                              \
    /* Input filter disabled */                                         \
    IOCON_PIO_INPFILT_OFF |                                             \
    /* IIC mode:Noise pulses below approximately */                     \
    /* 50ns are filtered out.*/                                         \
    IOCON_PIO_FSEL_DI |                                                 \
    /* Open drain is disabled */                                        \
    IOCON_PIO_OPENDRAIN_DI |                                            \
    /* IO_CLAMP disabled */                                             \
    IOCON_PIO_IO_CLAMP_DI

#define USART0_TX_MODE                                                  \
    /* Pin is configured as USART0_TXD */                               \
    IOCON_PIO_FUNC2 |                                                   \
    /* Selects pull-up function */                                      \
    IOCON_PIO_MODE_PULLUP |                                             \
    /* Standard mode, output slew rate control is disabled */           \
    IOCON_PIO_SLEW0_STANDARD | IOCON_PIO_SLEW1_STANDARD |               \
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

#define USART0_RX_MODE                                                  \
    /* Pin is configured as USART0_RXD */                               \
    IOCON_PIO_FUNC2 |                                                   \
    /* Selects pull-up function */                                      \
    IOCON_PIO_MODE_PULLUP |                                             \
    /* Standard mode, output slew rate control is disabled */           \
    IOCON_PIO_SLEW0_STANDARD |  IOCON_PIO_SLEW1_STANDARD |              \
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

const iocon_group_t usart1_io_cfg[] = {
    [0] = {
        .port = 0,
#ifdef USART1_FTDI
        .pin =  0,  /* USART 1 Tx */
        .modefunc =  USART0_TX_MODE,
#else
        .pin =  IOCON_USART1_TX_PIN,  /* USART 1 Tx */
        .modefunc =  USART1_TX_MODE,
#endif
    },
    [1] = {
        .port = 0,
#ifdef USART1_FTDI
        .pin =  1,   /* USART 1 Rx */
        .modefunc = USART0_RX_MODE,
#else
        .pin =  IOCON_USART1_RX_PIN,   /* USART 1 Rx */
        .modefunc = USART1_RX_MODE,
#endif
    },
};

const iocon_group_t usart0_io_cfg[] = {
    [0] = {
        .port = 0,
        .pin =  IOCON_USART0_TX_PIN,  /* USART 0 Tx */
        .modefunc =  USART0_TX_MODE,
    },
    [1] = {
        .port = 0,
        .pin =  IOCON_USART0_RX_PIN,   /* USART 0 Rx */
        .modefunc = USART0_RX_MODE,
    },
#if gUartHwFlowControl_d
    [2] = {
        .port = 0,
        .pin =  6,  /* USART 0 RTS */
        .modefunc =  USART0_TX_MODE,
    },
    [3] = {
        .port = 0,
        .pin =  7,   /* USART 0 CTS */
        .modefunc = USART0_RX_MODE,
    },
#endif

};

static iocon_group_t* BOARD_Get_UART_cfg(uint32_t uartInstance, uint32_t *elementCnt)
{
    iocon_group_t *pUsartCfg = NULL;

    switch (uartInstance)
    {
        case 0:
            pUsartCfg = (iocon_group_t *)usart0_io_cfg;
            *elementCnt = sizeof(usart0_io_cfg)/sizeof(iocon_group_t);
            break;

        case 1:
            pUsartCfg = (iocon_group_t *)usart1_io_cfg;
            *elementCnt = sizeof(usart1_io_cfg)/sizeof(iocon_group_t);
            break;

        default:
            break;
    }

    return pUsartCfg;
}
/*****************************************************************************
 * Public functions
 ****************************************************************************/
void BOARD_Init_UART_Pins(uint32_t uartInstance)
{
    uint32_t elementCnt = 0;
    iocon_group_t *pUsartCfg = BOARD_Get_UART_cfg(uartInstance, &elementCnt);

    if (NULL != pUsartCfg)
    {
         mUartPinsInit[uartInstance] = true;

        /* Enables the clock for the I/O controller block. 0: Disable. 1: Enable.: 0x01u */
        CLOCK_EnableClock(kCLOCK_Iocon);

        for (int i = 0; i < elementCnt; i++)
        {
            /* Initialize GPIO functionality on each pin : all inputs */
            IOCON_PinMuxSet(IOCON,
                            pUsartCfg[i].port,
                            pUsartCfg[i].pin,
                            pUsartCfg[i].modefunc);
        }
    }
}
void BOARD_DeInit_UART_Pins(uint32_t uartInstance)
{
    uint32_t elementCnt = 0;
    iocon_group_t *pUsartCfg = BOARD_Get_UART_cfg(uartInstance, &elementCnt);

    const gpio_pin_config_t pin_config = {
        .pinDirection = kGPIO_DigitalInput,
        .outputLogic = 0U
    };

    if (NULL != pUsartCfg)
    {
         mUartPinsInit[uartInstance] = false;

        /* Enables the clock for the I/O controller block. 0: Disable. 1: Enable.: 0x01u */
        CLOCK_EnableClock(kCLOCK_Iocon);
        /* Enables the clock for the GPIO0 module */
        CLOCK_EnableClock(kCLOCK_Gpio0);
        for (int i = 0; i < elementCnt; i++)
        {
            /* Initialize GPIO functionality on each pin : all inputs */
            GPIO_PinInit(GPIO, pUsartCfg[i].port, pUsartCfg[i].pin, &pin_config);

            IOCON_PinMuxSet(IOCON,
                            pUsartCfg[i].port,
                            pUsartCfg[i].pin,
                            LP_ANALOG_PULLDWN_CFG);
        }
    }
}

#if defined (gMWS_UseCoexistence_d) && (gMWS_UseCoexistence_d)
void BOARD_GetCoexIoCfg(void **rfDeny, void **rfActive, void **rfStatus)
{
    *rfDeny   = &rfDenyPin;
    *rfActive = &rfActivePin;
    *rfStatus = &rfStatusPin;
}
#else
void BOARD_InitCoexOutPin(void)
{
    /* Set coexistence pins to Low state */
    gpioOutputPinConfig_t outPin = {gpioPort_A_c, 0, 0, pinSlewRate_Slow_c, pinDriveStrength_Low_c};

    outPin.gpioPin = 15;
    GpioOutputPinInit(&outPin, 1);
    GpioClearPinOutput(&outPin);

    outPin.gpioPin = 16;
    GpioOutputPinInit(&outPin, 1);
    GpioClearPinOutput(&outPin);
}
#endif

#if gUartDebugConsole_d
/* Initialize debug console. */
status_t BOARD_InitDebugConsole(void)
{
    status_t result;

    uint32_t uartClkSrcFreq = BOARD_DEBUG_UART_CLK_FREQ;

    result = DbgConsole_Init(DEBUG_SERIAL_INTERFACE_INSTANCE, BOARD_DEBUG_UART_BAUDRATE, BOARD_DEBUG_UART_TYPE, uartClkSrcFreq);
    BOARD_Init_UART_Pins(DEBUG_SERIAL_INTERFACE_INSTANCE);

    return result;
}

/* DeInitialize debug console. */
status_t BOARD_DeinitDebugConsole(void)
{
    status_t result;
    result = DbgConsole_Deinit();
    return result;
}

void BOARD_SetDEBUG_UART_LowPower(void)
{
    BOARD_DeInit_UART_Pins(DEBUG_SERIAL_INTERFACE_INSTANCE);
    BOARD_DeinitDebugConsole();
}
#endif

#if gAdcUsed_d
uint8_t BOARD_GetBatteryLevel(void)
{
    static uint8_t    battery_lvl = 0;
    bool do_measure   = false;
    adc_result_info_t adcResultInfoStruct;

    ADC_DBG_LOG("");

    /*  do battery measurement if ADC is enabled or if battery level has never been measured */
    if( BOARD_IsADCEnabled() )
    {
        ADC_DBG_LOG("ADC not enabled");

        BOARD_CheckADCReady();

        do_measure = true;
    }
    else if(battery_lvl == 0)
    {
        ADC_DBG_LOG("ADC not initialised");

        BOARD_InitAdc();

        CLOCK_uDelay(ADC_WAIT_TIME_US);

        do_measure = true;
    }

    if(do_measure)
    {
        BOARD_ADCChannelSet(ADC_BAT_LEVEL_CHANNEL, true);

        ADC_DoSoftwareTriggerConvSeqA(ADC0);
        while (!ADC_GetChannelConversionResult(ADC0, ADC_BAT_LEVEL_CHANNEL, &adcResultInfoStruct))
        {
        }

        BOARD_ADCChannelSet(ADC_BAT_LEVEL_CHANNEL, false);

        /* battery_lvl = ADCoutputData/4095*ADCFullScale/ADCFrontEndGain/FullVoltageRange*100*/
        uint32_t voltageValMv = ADC_TO_MV(adcResultInfoStruct.result);
        battery_lvl  = (uint8_t)ADC_MV_TO_PERCENT(voltageValMv);
    }

    //PRINTF("BOARD_GetBatteryLevel =%d \r\n", battery_lvl);
    ADC_DBG_LOG("Battery level=%d", battery_lvl);

    return battery_lvl;
}

/* Store time when ADC is initialised, then enabling could be done at least 230us later */
#if gTimestampUseWtimer_c
static uint64_t ADCInit_time = 0;

static void BOARD_StoreADCInitTime(uint64_t time)
{
    ADC_DBG_LOG("ADCInit_time: %d", (uint32_t )(0xFFFFFFFF & time));
    ADCInit_time = time;
}
#endif


void BOARD_InitAdc(void)
{
    ADC_DBG_LOG("");

    LpIoSet(1, 0);

    if (!BOARD_IsADCEnabled())
    {
        ADC_DBG_LOG("ADC not enabled");
        ADC_Configuration(ADC0);

        /* Enabling ADC should be done at least 230us later */

#if gTimestampUseWtimer_c
        /* Store ADC Init Time */
        BOARD_StoreADCInitTime(Timestamp_Get_uSec());
#endif
        /* ADC just initialized, measurements should be done before going to low power */
        adc_measure_done = false;
    }

    LpIoSet(1, 1);

}

/* periodical ADC initialisation , one over gAppADCMeasureCounter_c times */
void BOARD_ADCWakeupInit(void)
{
     static uint32_t adc_count = 0;

    /* init ADC for measurement one over gAppADCMeasureCounter_c wakeup times*/
    if(adc_count == 0 )
    {
        ADC_DBG_LOG("Init ADC, adc_count : %d", adc_count);
        BOARD_InitAdc();
        adc_count = gAppADCMeasureCounter_c;
    }
    else
    {
        ADC_DBG_LOG("adc_count : %d", adc_count);
        adc_count--;
    }
}

/* Verify that ADC init has been done at least ADC_WAIT_TIME_US earlier */
void BOARD_CheckADCReady(void)
{
#if gTimestampUseWtimer_c
    uint64_t time;

    time = Timestamp_Get_uSec() - ADCInit_time ;

    if (time < ADC_WAIT_TIME_US)
    {
        CLOCK_uDelay(ADC_WAIT_TIME_US - time);
    }
#else
    /* when no time stamp, wait for ADC_WAIT_TIME_US */
    CLOCK_uDelay(ADC_WAIT_TIME_US);
#endif
}

void BOARD_EnableAdc(void)
{
    LpIoSet(1, 0);
    /* Make sure 230us has elapsed since BOARD_InitAdc() has been called
     * ADC requires a delay after setup, see RFT 1340 */
    //CLOCK_uDelay(ADC_WAIT_TIME_US);
    ADC_DBG_LOG("");
    ADC_EnableConvSeqA(ADC0, true); /* Enable the conversion sequence A. */

    LpIoSet(1, 1);
}

/*  Do ADC measurements before going to low power */
void BOARD_ADCMeasure(void)
{
    int32_t temperature;

    /* check ADC is initialized and measurements not already done */
    if( BOARD_IsADCEnabled() && (adc_measure_done == false) )
    {
        adc_measure_done = true;
        BOARD_GetBatteryLevel();
        temperature = BOARD_GetTemperature();
#if gSupportBle
        XCVR_TemperatureUpdate(temperature);
#else
        vRadio_Temp_Update((int16_t) (2*temperature));
#endif
    }
}

void BOARD_DeInitAdc(void)
{
    ADC_DBG_LOG("");
    /* Power off the ADC converter. */
    POWER_DisablePD(kPDRUNCFG_PD_LDO_ADC_EN);
    ADC_Deinit(ADC0);
}
#endif

uint32_t BOARD_GetUsartClock(int8_t instance)
{
    /* set a default */
    uint32_t ret = BOARD_DEFAULT_UART_CLK_FREQ;
    do {
    if (instance >= FSL_FEATURE_SOC_USART_COUNT) break;
#if gUartDebugConsole_d
        if (instance == DEBUG_SERIAL_INTERFACE_INSTANCE)
        {
            ret = BOARD_DEBUG_UART_CLK_FREQ;
            break;
        }
#endif
#if gUartAppConsole_d
        if (instance == APP_SERIAL_INTERFACE_INSTANCE)
        {
            ret = BOARD_APP_UART_CLK_FREQ;
        }
#endif
    } while (0);
    return ret;
}

uint32_t BOARD_GetCtimerClock(CTIMER_Type *timer)
{
    return CLOCK_GetApbCLkFreq();
}

uint16_t BOARD_GetPotentiometerLevel(void)
{
    uint32_t random = 0;
    RNG_GetRandomNo(&random);

    return (uint16_t)(random & 0x0FFF);
}

void BOARD_SetFaultBehaviour(void)
{
    /* In the absence of a proper setting in SHCSR, all faults go to a HardFault
     * Since we want a finer discrimination for BusFaults in order to catch Faults when
     * accessing the Flash on pages in error, lets set  BUSFAULTENA.
     * The other may turnout usefull later
     */
    SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk ;
    // SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk ;
    // SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk ;

}

__attribute__((section(".after_vectors")))  void BusFault_Handler(void)
{
    /* Retrieve the stack pointer */
    /* At this point LR contains the EXC_RETURN whose value tells
     * which stack pointer to restore : MSP Main Stack pointer or
     * PSP Process Stack pointer*/
    asm volatile (
        "TST   lr, #0x4\n"
        "ITE   EQ\n"
        "MRSEQ r0, MSP\n"
        "MRSNE r0, PSP\n"
        :
        :
        : "r0"
    );

    /* Set the exception return address to recovery function
     * Force execution of the Fault Recovery function and patch the LR to be reloaded
     * in the stack frame.
     * As the exception fired SP got decreased by 8 32 bit words as it stored:
     * PSR, PC, LR, R13, R3-R0
     * From the stack bottom LR is to be found at offset 6 * 4 i.e. 24, still here we have
     * gone deeper in the stack by 1 more word becasue r7 gets  pushed on the stack too
     *  thence the 28 offset
     * */
    asm volatile (
        "MOV   r1, %0\n"
        "STR   r1, [r0, #28]\n"
        :
        : "r"(FaultRecovery)
        : "r0", "r1"
    );

}

uint32_t BOARD_GetSpiClock(uint32_t instance)
{
    panic(0,0,0,0);
    return 0;
}


#if gDbgUseLLDiagPort
/* BOARD_DIAG_PORT_MODE 32 bit word is used to set up to 4 DBG_FUNC modes at a time.
 * It is used to set the Link Layer DIAGCTRL register telling it to output debug
 * information of IOs.
 * The value of each 8-bit tranche composing this 32-bit word is the configuration corresponds
 * to a DIAG mode: DIAG0 and DIAG2 can be muxed towards PIO0..PIO7 port whereas DIAG1..DIAG3
 * are routed to PIO8..PIO15 if configured.
 * The IOCON configuration allows the selection.
 * Beware PIO[8:9] are USART0 so might be lost
 * Beware PIO[10:11] are USART1 so might be lost too, also these 2 are I2C specific IOs
 * Beware PIO[12:13] are SWD so is very painful to lose
 * For each PIO in the IOCON, DBG_FUNC is coded on 4 bits however only values 1 and 2 make sense for
 * SW control of LL diagnostic traces. The DIAG port consists of 16 outputs, so let's consider a 2 bit code to say:
 *   - 0 means not a debug pin,
 *   - 1 routes DIAG0 signals to PIO [0..7] and DIAG1 ones to PIO[8..15]
 *   - 2 routes DIAG2 signals to PIO [0..7] and DIAG3 ones to PIO[8..15]
 *   Combining 16 of these 2 bit codes fits in a single 32bit word.
 * In order to mux to PIO[0..7] signals from either DIAG0 or DIAG2, configure DIAG_PIN_DBGCFG(pin, 1 or 2)
 * likewise the muxing to PIO[8..15] from DIAG1 or DIAG3 can be configured using  DIAG_PIN_DBGCFG(pin, 1 or 2)
 * Note that in order to preserve the debug UART functionality you may wish to prevent routing of LL DIAG signals
 * to pins 8 and 9. This is done using by configuring DIAG_PIN_DBGCFG(8, 0) since USART0 Tx is pin 8 with pin mode #2.
 * We could move USART0 to pin18 and 19 if we configured them in mode #5 thus potentially freeing pin 8 and 9 for LL DIAG.
 * The same kind of issue exists for pins 10 - 11 that are USART1's default.
 * Be warned that SWD interface makes use of pins 12 and 13. Setting the pinmux to make them belong to the DIAG port,
 * prevents the use of a debugger.
 * If we need to manually set a mix of groups
 * Diag 0 & Diag 2 -
 * Use the DIAG_PIN_DBGCFG macro below to select the pins that are to be directed to the DIAG port.
 * DIAG_PIN_DBGCFG(pin, 1) maps a Diag0 or Diag1 signal to IO pin
 * DIAG_PIN_DBGCFG(pin, 2) to map a Diag2 or Diag3 signal to IO the IO pin
 * For instance:
 *        GPIO 0:3    DIAGCNTL 0x03 TX,RX, sync win, sync found
 *        GPIO 4:6    DIAGCNTL 0x0B  exchange memory accesses
 *        GPIO 7      DIAGCNTL 0x03  ble_error_irq
 *        GPIO 11:12  DIAGCNTL 0x07 625us, prefetch
 *        GPIO 15     DIAGCNTL 0x1F event_in_process
 * This results in BOARD_DIAG_PORT_MODE defined as  0x9F8B8783
 *
 * #define DIAG_IOCON_SETTING \
 *       (DIAG_PIN_DBGCFG(0, 1) |\
 *        DIAG_PIN_DBGCFG(1, 1) |\
 *        DIAG_PIN_DBGCFG(2, 1) |\
 *        DIAG_PIN_DBGCFG(3, 1) |\
 *        DIAG_PIN_DBGCFG(4, 2) |\
 *        DIAG_PIN_DBGCFG(5, 2) |\
 *        DIAG_PIN_DBGCFG(6, 2) |\
 *        DIAG_PIN_DBGCFG(7, 1) |\
 *        DIAG_PIN_DBGCFG(8, 0) |\
 *        DIAG_PIN_DBGCFG(9, 0) |\
 *        DIAG_PIN_DBGCFG(10, 0) |\
 *      DIAG_PIN_DBGCFG(11, 1) |\ <- the breaks the USART1
 *      DIAG_PIN_DBGCFG(12, 1) |\ <- note that this setting ruins the SWD usage
 *        DIAG_PIN_DBGCFG(15, 2))
 *
**/

#define DIAG_PIN_DBGCFG(pin, cfg) (((cfg) & 0x3) << (pin*2))


#define DIAG_IOCON_SETTING \
       (DIAG_PIN_DBGCFG(0, 1) |\
        DIAG_PIN_DBGCFG(1, 1) |\
        DIAG_PIN_DBGCFG(2, 1) |\
        DIAG_PIN_DBGCFG(3, 1) |\
        DIAG_PIN_DBGCFG(4, 1) |\
        DIAG_PIN_DBGCFG(5, 1) |\
        DIAG_PIN_DBGCFG(6, 1) |\
        DIAG_PIN_DBGCFG(7, 1) |\
        DIAG_PIN_DBGCFG(8, 0) |\
        DIAG_PIN_DBGCFG(9, 0) |\
        DIAG_PIN_DBGCFG(10, 0) |\
        DIAG_PIN_DBGCFG(11, 0) |\
        DIAG_PIN_DBGCFG(12, 0) |\
        DIAG_PIN_DBGCFG(13, 0) |\
        DIAG_PIN_DBGCFG(14, 1) |\
        DIAG_PIN_DBGCFG(15, 1))

void BOARD_DbgDiagIoConf(void)
{
    int i;
    uint32_t diag_iocon_val = DIAG_IOCON_SETTING;

    uint32_t dbg_func;
    uint32_t val;

    /* PIO[0..7] settings */
    if (!(BOARD_DIAG_PORT_MODE & gDbgLLDiagPort0Msk))
        diag_iocon_val &= 0xffff0000;
    /* PIO[8..15] settings */
    if (!(BOARD_DIAG_PORT_MODE & gDbgLLDiagPort1Msk))
        diag_iocon_val &= 0x0000ffff;

    for (i = 0; i < 16; i++)
    {
        dbg_func = diag_iocon_val & 0x3;
        if (dbg_func == 1 || dbg_func == 2)
        {
            /* pins 10 and 11 are I2C capable and have a special programming */
            val = (i == 10 || i == 11) ? IOCON_I2C_CFG(dbg_func) : IOCON_CFG(dbg_func);
            IOCON -> PIO[0][i] = val;
        }
        else
        {
            val = IOCON->PIO[0][i];
            /* pins 10 and 11 are I2C capable and have a special programming */
            if (i == 10 || i == 11)
                val &= ~(uint32_t)IOCON_PIO_I2C_DBG_MODE_MASK;
            else
                val &= ~(uint32_t)IOCON_PIO_DBG_MODE_MASK;
            IOCON->PIO[0][i] = val;
        }
        diag_iocon_val >>= 2;
    }
}

void BOARD_DbgDiagEnable(void)
{
    REG_BLE_WR(BLE_DIAGCNTL_OFFSET, BOARD_DIAG_PORT_MODE );
}
#else
void BOARD_DbgDiagIoConf(void)
{
}
void BOARD_DbgDiagEnable(void)
{
}
#endif

void BOARD_InitPMod_SPI_I2C( void )
{
    panic(0,0,0,0);
}

void BOARD_InitSPI( void )
{
    panic(0,0,0,0);
}


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

#define SPIFI_FAST_IO_MODE_INACT   (SPIFI_FAST_IO_MODE | IOCON_PIO_MODE(2))
#define SPIFI_FAST_IO_MODE_PULLUP  (SPIFI_FAST_IO_MODE | IOCON_PIO_MODE(0))


const iocon_group_t sfifi_io_cfg[] = {
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

static void _BOARD_InitSPIFIPins(void)
{
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
}

static void _BOARD_DeInitSPIFIPins(void)
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


void BOARD_InitSPIFI( void )
{
    uint32_t divisor;

    _BOARD_InitSPIFIPins();
    RESET_SetPeripheralReset(kSPIFI_RST_SHIFT_RSTn);
    /* Enable clock to block and set divider */
    CLOCK_AttachClk(kMAIN_CLK_to_SPIFI);
    divisor = CLOCK_GetSpifiClkFreq() / BOARD_SPIFI_CLK_RATE;
    CLOCK_SetClkDiv(kCLOCK_DivSpifiClk, divisor ? divisor : 1, false);
    RESET_ClearPeripheralReset(kSPIFI_RST_SHIFT_RSTn);

}


/* SPIFI Flash routines for K32W041AM Chip */

#define  K32W041AM_SPIFI_FLASH_BUSY_FLAG_MASK  0x01

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


#if gDbgUseDbgIos
void BOARD_InitDbgIo(void)
{
    const gpioOutputPinConfig_t *pinArray = &dk6_dbg_io_pins[0];
    const iocon_group_t * p_iocon = &dk6_dbg_io[0];
    uint32_t nb_dbg_io = sizeof(dk6_dbg_io)/sizeof(iocon_group_t);
    gpio_pin_config_t dk6_output = { .pinDirection = kGPIO_DigitalOutput};
    for (uint32_t i = 0; i < nb_dbg_io; i++)
    {
        dk6_output.outputLogic = pinArray->outputLogic;
        IOCON_PinMuxSet(IOCON,
                      p_iocon->port,
                      p_iocon->pin,
                      p_iocon->modefunc);

        GPIO_PinInit(GPIO, pinArray->gpioPort, pinArray->gpioPin, &dk6_output);
        pinArray++;
        p_iocon++;
    }
    GpioOutputPinInit(dk6_dbg_io_pins, nb_dbg_io);

    // Toggle all Dbg IOs

    IODBG_CLR_MASK(IODBG_ALL_PIN_MASK);
    IODBG_SET_MASK(IODBG_ALL_PIN_MASK);
    IODBG_CLR_MASK(IODBG_ALL_PIN_MASK);

    /* To output RFTX and RFRX on 20 & 21 */
//    IOCON -> PIO[0][20] = 0x85;
//    IOCON -> PIO[0][21] = 0x85;

     /* To output Clock on 17 */
//    SYSCON -> CLKOUTSEL = SYSCON_CLKOUTSEL_SEL(0); // MAINCLK output to CLK_OUT
//    SYSCON -> CLKOUTDIV = SYSCON_CLKOUTDIV_DIV(3) | SYSCON_CLKOUTDIV_HALT(0);
//    IOCON -> PIO[0][17] =  0x3BD;
}
#endif
#if (gKBD_KeysCount_c > 0)

void BOARD_UnInitButtons(void)
{
    INPUTMUX_Init(INPUTMUX);
    for (int i = 0; i < sizeof(dk6_button_io_pins)/sizeof(gpioInputPinConfig_t); i++)
    {
        INPUTMUX_AttachSignal(INPUTMUX, dk6_button_io_pins[i].pinIntSelect,
                              (inputmux_connection_t)INPUTMUX_GpioPortPinToPintsel(0, 0x1f));
        PINT_PinInterruptConfig(PINT, dk6_button_io_pins[i].pinIntSelect, (pint_pin_enable_t)pinInt_Disabled_c, NULL);
    }
    INPUTMUX_Deinit(INPUTMUX); /* Can be gated now because used only for changes */
}

void BOARD_SetButtons_LowPowerEnter(void)
{
    const iocon_group_t *pinArray = &dk6_buttons_io[0];
    int nb_buttons = sizeof(dk6_buttons_io)/sizeof(iocon_group_t);
    /* Enables the clock for the I/O controller block. 0: Disable. 1: Enable.: 0x01u */
    CLOCK_EnableClock(kCLOCK_Iocon);
    /* Enables the clock for the GPIO0 module */
    CLOCK_EnableClock(kCLOCK_Gpio0);
    for (int i = 0; i < nb_buttons; i++)
    {
        IOCON_PinMuxSet(IOCON, pinArray->port, pinArray->pin, IOCON_FUNC0|IOCON_MODE_PULLUP | IOCON_DIGITAL_EN);
        GPIO_PinInit(GPIO, pinArray->port, pinArray->pin, &((const gpio_pin_config_t){kGPIO_DigitalInput, 1}));
        pinArray++;
    }
}
#endif


#if (gLEDsOnTargetBoardCnt_c > 0)
void BOARD_SetLEDs_LowPower(void)
{
    const iocon_group_t *pinArray = &dk6_leds_io[0];
    uint32_t nb_leds = sizeof(dk6_leds_io)/sizeof(iocon_group_t);
    /* Enables the clock for the I/O controller block. 0: Disable. 1: Enable.: 0x01u */
    CLOCK_EnableClock(kCLOCK_Iocon);
    /* Enables the clock for the GPIO0 module */
    CLOCK_EnableClock(kCLOCK_Gpio0);
    for (uint32_t i = 0; i < nb_leds; i++)
    {
        GPIO_PinInit(GPIO, pinArray->port, pinArray->pin, &((const gpio_pin_config_t){kGPIO_DigitalInput, 0}));
        IOCON_PinMuxSet(IOCON, pinArray->port, pinArray->pin,
                        IOCON_PIO_FUNC(0)
                        | IOCON_PIO_MODE(3)     /* Pull Down resistor enabled */
                        | IOCON_PIO_DIGIMODE(0) /* Analog not Digital */
        );
        pinArray++;
    }
}
#endif


#if gDbgUseDbgIos

void force_all_dbg_io_high(void)
{
    IODBG_SET_MASK(IODBG_ALL_PIN_MASK);
}
/* Function that gives a pattern to be easily recognized when called */
void toggle_io_debug(void)
{
    IODBG_CLR_MASK(IODBG_ALL_PIN_MASK);
    IODBG_SET_MASK(IODBG_ALL_PIN_MASK);

    /* IO0 denotes CPU active (except IRQ) : toggle it once more */
    /* IO1 denotes power down */
    /* IO2 denotes LL IRQ : clear it*/
    /* IO3 LL IRQ in sleep if LL irq down,  with error if LL irq is up : clear it */
    /* IO4 denotes wakeup let it high */

    IODBG_CLR(0);    IODBG_SET(0);

    IODBG_CLR(2);

    IODBG_CLR(3);

}
#endif


#if (gPWR_SmartFrequencyScaling > 0) && defined(gPWR_BleWakeupTimeOptimDisabled) && (gPWR_BleWakeupTimeOptimDisabled == 1)
#error gPWR_SmartFrequencyScaling requires gPWR_BleWakeupTimeOptimDisabled to be unset
#endif

void BOARD_common_hw_init(void)
{
    reset_cause_t reset_cause;

#if (gPWR_SmartFrequencyScaling > 0) || (gPWR_CpuClk_48MHz == 1)
    /* Temporary switch to FRO48M to speed up initialization until LL wakeup complete */
    BOARD_CpuClockUpdate48Mz();
#else
    /* Switch to FRO32M to speed up initialization
        should already be done in start up file */
    SYSCON -> MAINCLKSEL = BOARD_MAINCLK_FRO32M;
#endif

    SYSCON_CPUCTRL &= ~BIT(3); /* power optimization */

    /* MODEM master priority = 3 - highest */
    SYSCON -> AHBMATPRIO = 0x00000300;

    /* Security code to allow debug access */
    SYSCON->CODESECURITYPROT = 0x87654320;

    BOARD_SetFaultBehaviour();

#if gOTA_externalFlash_d
    /* Allows to write in the qspi in each 256 blocks */
    SYSCON->MEMORYREMAP = (SYSCON->MEMORYREMAP & 0xfffff) | 0xe400000;
#endif

    /* Enable APB bridge access */
    CLOCK_EnableAPBBridge();
    CLOCK_EnableClock(CLOCK_32k_source);  // Done in BOARD_InitClocks()

#if gLoggingActive_d
    /* Need to start 32k clock before starting Logging service because of WTIMER timestamp */
    DbgLogInit(RAM2_BASE+gLogRingPlacementOffset_c, RAM2_SIZE-gLogRingPlacementOffset_c);
#elif gTimestampUseWtimer_c
    Timestamp_Init(); /* does nothing if already started */
#endif

#if gDbgUseDbgIos
    BOARD_InitDbgIo();
    //force_all_dbg_io_high();
    toggle_io_debug();
#endif

#if gDbgUseLLDiagPort
    BOARD_DbgDiagIoConf();
    BOARD_DbgDiagEnable();
#endif

    /* check if we come from power down */
    reset_cause = POWER_GetResetCause();
    if(reset_cause  != RESET_WAKE_PD)
    {
        INIT_DBG_LOG("Reset Cause=%x PMC reset cause:%x", reset_cause, PMC->RESETCAUSE);
    }

    /* Set up LDO 32M before CLOCK_XtalBasicTrim()/BOARD_tcxo32M_compensation_run()
        requires 20us for LDO to stabilize */
    CLOCK_SetXtal32M_LDO();

    /* Configure the optimized active voltages, set up the BODs */
    /* POWER_Init must be called regardless of cold and warm boot */
    /* POWER_SetTrimDefaultActiveVoltage will be called only in case of cold boot */
    POWER_Init();

#ifdef FOR_BOD_DEBUG
    /* Set up the BODs
     * Note : This function shall be safe in term of LDO voltage - no Voltage scaling yet */
    POWER_BodSetUp();
#endif

    /* TODO : Trim and bias the XTAL32M on Radio : do we need to be sure XTAL is ready first ? */
    LpIoSet(1, 0);

#if gXo32M_Trim_c
    BOARD_tcxo32M_compensation_run(0, 0);
#else
    CLOCK_XtalBasicTrim();
#endif

#if gXo32k_Trim_c
    BOARD_tcxo32k_compensation_run(0, 0);
#endif
#if !defined(DUAL_MODE_APP) && (defined(gSupportBle) && (gSupportBle == 1))
    vRadio_SkipXTALInit();
    vRadio_DisableZBRadio();
#endif
    vRadio_ActivateXtal32MRadioBiasing();

#if defined(cPWR_FullPowerDownMode) && ( cPWR_FullPowerDownMode == 1 )
#if !defined(gPWR_BleWakeupTimeOptimDisabled) || (gPWR_BleWakeupTimeOptimDisabled == 0)
    static uint8_t ram_was_retained = 0;
    LpIoSet(1, 1);
    if ( (reset_cause == RESET_WAKE_PD) && (ram_was_retained == 1) )
    {
        /* Call in advance the UpdateWakeupReason in Advance, it will call the LL ISR to speed up the wakeup time */
        PWR_UpdateWakeupReason();
    }
    else
    {
        /* set flag to 1 for next time, it will be reset to zero if ram not retained */
        ram_was_retained = 1;
    }
    LpIoSet(1, 0);
#endif

#if (gPWR_SmartFrequencyScaling > 0) // CPU frequency was set to 48Mhz
#if gPWR_CpuClk_48MHz
    /* nothing to do,  already at 48Mhz */
#else
    /* reduce to 32Mhz to reduce power consumption */
    BOARD_CpuClockUpdate32MhzXtal();
#endif


#if gPWR_LdoVoltDynamic
    /* now revert to 1.0V since the peak current is done */
    POWER_ApplyLdoActiveVoltage(PM_LDO_VOLT_1_0V);
#endif
#endif  // (gPWR_SmartFrequencyScaling > 0)
#endif  /* cPWR_FullPowerDownMode */

#ifdef FOR_BOD_DEBUG
    /* need to wait for 27us for the LDO BOD core to set up (Done during POWER_BodSetUp()) - so do it after XTAL trimming/biasing */
    POWER_BodActivate();
#endif

    /* It is mandatory to reset FLEXCOM peripherals because FLEXCOM is not reset on wakeup from power down
     * Except if FLEXCOM is retained during power down mode
     * No need to reset other blocks, these are already reset by Hardware */
    if ( (reset_cause == RESET_WAKE_PD) )
    {
        if ((!(PMC->PDSLEEPCFG & PMC_PDSLEEPCFG_PDEN_PD_COMM0_MASK) ||
         (DEBUG_SERIAL_INTERFACE_INSTANCE == 0)))
        {
            RESET_PeripheralReset(kUSART0_RST_SHIFT_RSTn);
            RESET_PeripheralReset(kUSART1_RST_SHIFT_RSTn);
            RESET_PeripheralReset(kI2C0_RST_SHIFT_RSTn);
            RESET_PeripheralReset(kSPI0_RST_SHIFT_RSTn);
        }
#ifdef DUAL_MODE_APP
        RESET_PeripheralReset(kGPIO0_RST_SHIFT_RSTn);
        RESET_PeripheralReset(kADC0_RST_SHIFT_RSTn);
#endif
    }

    /* For power optimization, we need a limited clock enabling specific to this application
     * This replaces BOARD_BootClockRUN() usually called in other application    */
    BOARD_InitClocks();

#if gUartDebugConsole_d
    BOARD_InitDebugConsole();
#endif

#if gDbgUseRfDiagIos
    BOARD_DbgRfDiagIoInit();
#endif
    /* remove comment to keep Debug port IOs on wake up from power down
     * Note : this will disable the button after power down   */
#if (BOARD_DIAG_PORT_MODE & gDbgLLDiagPort0Msk)
    if ((reset_cause == RESET_WAKE_PD))
    {
#if gKeyBoardSupported_d
        KBD_ShutOff();
#endif
    }
#endif

#if (gPWR_SmartFrequencyScaling == 0)    // CPU clock was 32Mhz FRO
#if gPWR_CpuClk_48MHz
    /* nothing to do,  already at 48Mhz */
#else
    /* Switch to target frequency 32MHz XTAL set by application
     * Configure correctly the Flash wait states  */
    BOARD_CpuClockUpdate32MhzXtal();
#endif

#if gPWR_LdoVoltDynamic
    /* now revert to 1.0V since the current peak is done */
    POWER_ApplyLdoActiveVoltage(PM_LDO_VOLT_1_0V);
#endif
#endif

    /* Always call this in case CPU frequency was updated during/before hardware_init() */
    SystemCoreClockUpdate();

    OSA_TimeInit();

    /* on K32W041AM chip, external flash need to be initialised and set in deep power down to avoid over consumption */
    if(reset_cause  != RESET_WAKE_PD)
    {
        BOARD_SPIFI_FLASH_Config();
    }
    LpIoSet(1, 1);
}


#if gUartAppConsole_d

void BOARD_InitHostInterface(void)
{
    BOARD_Init_UART_Pins(APP_SERIAL_INTERFACE_INSTANCE);
}

void BOARD_SetHostIf_UART_LowPower(void)
{
    /* Reconfigure USART RX/TX pins */
     BOARD_DeInit_UART_Pins(APP_SERIAL_INTERFACE_INSTANCE);
}

#endif /* gUartAppConsole_d */

void BOARD_BatteryLevelPinInit(void)
{
    /* Set up the Analog input */
    IOCON_PinMuxSet(IOCON, 0, gADC0BatLevelInputPin, IOCON_ANALOG_EN);
}

void BOARD_SetPinsForPowerMode(void)
{
#if gUartDebugConsole_d
    BOARD_SetDEBUG_UART_LowPower();
#endif
#if gUartAppConsole_d
    BOARD_SetHostIf_UART_LowPower();
#endif
    /* If the pins have been de-init above this code will not execute, it covers the case where pins
       have been init outside debug or app console */
    for (uint32_t i = 0; i < FSL_FEATURE_SOC_USART_COUNT; i++)
    {
        if (mUartPinsInit[i])
        {
            BOARD_DeInit_UART_Pins(i);
        }
    }

#if (gLEDsOnTargetBoardCnt_c > 0)
    BOARD_SetLEDs_LowPower();           /* Set outputs as inputs to stop current output */
#endif
#if (gKBD_KeysCount_c > 0)
    KBD_PrepareEnterLowPower();
    BOARD_SetButtons_LowPowerEnter();        /* already inputs could do nothing */
#endif

    /* The pin mux settings below are there to compensate for a bug in the IO output configurations */
    /* Force LED outputs to become pulled-down inputs */
    IOCON_PinMuxSet(IOCON, 0, 0, IOCON_PIO_FUNC(0)| IOCON_PIO_MODE(3) | IOCON_PIO_DIGIMODE(0) );
    IOCON_PinMuxSet(IOCON, 0, 3, IOCON_PIO_FUNC(0)| IOCON_PIO_MODE(3) | IOCON_PIO_DIGIMODE(0) );
    /* Force UART1 Tx line to be set as input and pulled down */
    IOCON_PinMuxSet(IOCON, 0, 10, IOCON_PIO_FUNC(0)| IOCON_PIO_MODE(3) | IOCON_PIO_DIGIMODE(0) );

#if gDbgUseRfDiagIos
    /* force RF debug Ios to be set as input and pulled down */
    IOCON_PinMuxSet(IOCON, 0, 14, IOCON_PIO_FUNC(0)| IOCON_PIO_MODE(3) | IOCON_PIO_DIGIMODE(0) );
    IOCON_PinMuxSet(IOCON, 0, 15, IOCON_PIO_FUNC(0)| IOCON_PIO_MODE(3) | IOCON_PIO_DIGIMODE(0) );
#endif

}

void BOARD_SetSpiFi_LowPowerEnter(void)
{
    _BOARD_DeInitSPIFIPins();
}

void board_specific_action_on_idle(void)
{

#if gOtaEepromPostedOperations_d
    /* Checks if any Eeprom Write transactions are pending and resume them if the
     * status is no busy */
    OTA_TransactionResume();
#endif
#if ((defined gTcxo32k_ModeEn_c) && (gTcxo32k_ModeEn_c != 0))
    BOARD_tcxo32k_compensation_run(2, 0);
#endif
#if ((defined gTcxo32M_ModeEn_c) && (gTcxo32M_ModeEn_c != 0))
    BOARD_tcxo32M_compensation_run(2, 10); /* 2 degrees - wait for 10us */
#endif
}
#if 0
#define NumberOfElements(x) (sizeof(x)/sizeof((x)[0]))
typedef struct {
    const char * reg_name;
    const volatile uint32_t *reg_addr;
} reg_desc_t;

static const reg_desc_t RtcDesc[] = {
    {"CTRL",  &RTC->CTRL },
    {"MATCH", &RTC->MATCH },
    {"COUNT", &RTC->COUNT },
    {"WAKE",  &RTC->WAKE },
};

void dump_rtc(void)
{
    PRINTF("RTC\r\n");
    for (int i = 0; i < NumberOfElements(RtcDesc); i++)
     {
         PRINTF("\t%s = %x\r\n", RtcDesc[i].reg_name, *RtcDesc[i].reg_addr);
     }
}
static const reg_desc_t Async_SysconDesc[] = {
    {"ASYNCPRESETCTRL",        &ASYNC_SYSCON->ASYNCPRESETCTRL },
    {"ASYNCAPBCLKCTRL",        &ASYNC_SYSCON->ASYNCAPBCLKCTRL },
    {"ASYNCAPBCLKSELA",        &ASYNC_SYSCON->ASYNCAPBCLKSELA },
    {"ASYNCAPBCLKDIV",         &ASYNC_SYSCON->ASYNCAPBCLKDIV },
    {"ASYNCCLKOVERRIDE",       &ASYNC_SYSCON->ASYNCCLKOVERRIDE },
    {"TEMPSENSORCTRL",         &ASYNC_SYSCON->TEMPSENSORCTRL },
    {"NFCTAGPADSCTRL",         &ASYNC_SYSCON->NFCTAGPADSCTRL },
    {"XTAL32MLDOCTRL",         &ASYNC_SYSCON->XTAL32MLDOCTRL },
    {"XTAL32MCTRL",            &ASYNC_SYSCON->XTAL32MCTRL },
    {"ANALOGID",               &ASYNC_SYSCON->ANALOGID },
    {"RADIOSTATUS",            &ASYNC_SYSCON->RADIOSTATUS },
    {"DIGITALSTATUS",          &ASYNC_SYSCON->DIGITALSTATUS },
    {"DCBUSCTRL",              &ASYNC_SYSCON->DCBUSCTRL },
    {"FREQMECTRL",             &ASYNC_SYSCON->FREQMECTRL },
    {"NFCTAGINTSTATUS",        &ASYNC_SYSCON->NFCTAGINTSTATUS },
    {"NFCTAG_VDD",             &ASYNC_SYSCON->NFCTAG_VDD },
};

void dump_async_syscon(void)
{
    PRINTF("ASYNC_SYSCON\r\n");
    for (int i = 0; i < NumberOfElements(Async_SysconDesc); i++)
     {
         PRINTF("\t%s = %x\r\n", Async_SysconDesc[i].reg_name, *Async_SysconDesc[i].reg_addr);
     }
}



static const reg_desc_t SysconDesc[] = {
    {"MEMORYREMAP",       &SYSCON->MEMORYREMAP },
    {"AHBMATPRIO",        &SYSCON->AHBMATPRIO },
    {"BUFFERINGAHB2VPB0", &SYSCON->BUFFERINGAHB2VPB[0] },
    {"BUFFERINGAHB2VPB1", &SYSCON->BUFFERINGAHB2VPB[1] },
    {"SYSTCKCAL",         &SYSCON->SYSTCKCAL },
    {"NMISRC",            &SYSCON->NMISRC },
    {"ASYNCAPBCTRL",      &SYSCON->ASYNCAPBCTRL },
    {"PRESETCTRL0",       &SYSCON->PRESETCTRL[0] },
    {"PRESETCTRL1",       &SYSCON->PRESETCTRL[1] },
    {"AHBCLKCTRL0",       &SYSCON->AHBCLKCTRL[0] },
    {"AHBCLKCTRL1",       &SYSCON->AHBCLKCTRL[1] },
    {"MAINCLKSEL",        &SYSCON->MAINCLKSEL },
    {"OSC32CLKSEL",       &SYSCON->OSC32CLKSEL },
    {"CLKOUTSEL",         &SYSCON->CLKOUTSEL },
    {"SPIFICLKSEL",       &SYSCON->SPIFICLKSEL },
    {"ADCCLKSEL",         &SYSCON->ADCCLKSEL },
    {"USARTCLKSEL",       &SYSCON->USARTCLKSEL },
    {"I2CCLKSEL",         &SYSCON->I2CCLKSEL },
    {"SPICLKSEL",         &SYSCON->SPICLKSEL },
    {"IRCLKSEL",          &SYSCON->IRCLKSEL },
    {"PWMCLKSEL",         &SYSCON->PWMCLKSEL },
    {"WDTCLKSEL",         &SYSCON->WDTCLKSEL },
    {"MODEMCLKSEL",       &SYSCON->MODEMCLKSEL },
    {"FRGCLKSEL",         &SYSCON->FRGCLKSEL },
    {"DMICCLKSEL",        &SYSCON->DMICCLKSEL },
    {"WKTCLKSEL",         &SYSCON->WKTCLKSEL },
    {"ISO7816CLKSEL",     &SYSCON->ISO7816CLKSEL },
    {"SYSTICKCLKDIV",     &SYSCON->SYSTICKCLKDIV },
    {"TRACECLKDIV",       &SYSCON->TRACECLKDIV },
    {"WDTCLKDIV",         &SYSCON->WDTCLKDIV },
    {"IRCLKDIV",          &SYSCON->IRCLKDIV },
    {"AHBCLKDIV",         &SYSCON->AHBCLKDIV },
    {"CLKOUTDIV",         &SYSCON->CLKOUTDIV },
    {"SPIFICLKDIV",       &SYSCON->SPIFICLKDIV },
    {"ADCCLKDIV",         &SYSCON->ADCCLKDIV },
    {"RTCCLKDIV",         &SYSCON->RTCCLKDIV },
    {"ISO7816CLKDIV",&SYSCON->ISO7816CLKDIV },
    {"FRGCTRL",&SYSCON->FRGCTRL },
    {"DMICCLKDIV",&SYSCON->DMICCLKDIV },
    {"RTC1HZCLKDIV",&SYSCON->RTC1HZCLKDIV },
    {"CLOCKGENUPDATELOCKOUT",&SYSCON->CLOCKGENUPDATELOCKOUT },
    {"EFUSECLKCTRL",&SYSCON->EFUSECLKCTRL },
    {"RNGCTRL",&SYSCON->RNGCTRL },
    {"RNGCLKCTRL",&SYSCON->RNGCLKCTRL },
    {"SRAMCTRL",&SYSCON->SRAMCTRL },
    {"SRAMCTRL0",&SYSCON->SRAMCTRL0 },
    {"SRAMCTRL1",&SYSCON->SRAMCTRL1 },
    {"ROMCTRL",&SYSCON->ROMCTRL },
    {"MODEMCTRL",&SYSCON->MODEMCTRL },
    {"MODEMSTATUS",&SYSCON->MODEMSTATUS },
    {"XTAL32KCAP",&SYSCON->XTAL32KCAP },
    {"XTAL32MCTRL",&SYSCON->XTAL32MCTRL },
    {"STARTER0",&SYSCON->STARTER[0] },
    {"STARTER1",&SYSCON->STARTER[1] },
    {"RETENTIONCTRL",&SYSCON->RETENTIONCTRL },
    {"POWERDOWNSAFETY",&SYSCON->POWERDOWNSAFETY },
    {"MAINCLKSAFETY",&SYSCON->MAINCLKSAFETY },
    {"HARDWARESLEEP",&SYSCON->HARDWARESLEEP },
    {"CPUCTRL",&SYSCON->CPUCTRL },
    {"CPBOOT",&SYSCON->CPBOOT },
    {"CPSTACK",&SYSCON->CPSTACK },
    {"CPSTAT",&SYSCON->CPSTAT },
    {"GPIOSECIN",&SYSCON->GPIOSECIN },
    {"GPIOSECOUT",&SYSCON->GPIOSECOUT },
    {"GPIOSECDIR",&SYSCON->GPIOSECDIR },
    {"ANACTRL_CTRL",&SYSCON->ANACTRL_CTRL },
    {"ANACTRL_VAL",&SYSCON->ANACTRL_VAL },
    {"ANACTRL_STAT",&SYSCON->ANACTRL_STAT },
    {"ANACTRL_INTENSET",&SYSCON->ANACTRL_INTENSET },
    {"ANACTRL_INTSTAT",&SYSCON->ANACTRL_INTSTAT },
    {"CLOCK_CTRL",&SYSCON->CLOCK_CTRL },
    {"WKT_CTRL",&SYSCON->WKT_CTRL },
    {"WKT_LOAD_WKT0_LSB",&SYSCON->WKT_LOAD_WKT0_LSB },
    {"WKT_LOAD_WKT0_MSB",&SYSCON->WKT_LOAD_WKT0_MSB },
    {"WKT_LOAD_WKT1",&SYSCON->WKT_LOAD_WKT1 },
    {"WKT_VAL_WKT0_LSB",&SYSCON->WKT_VAL_WKT0_LSB },
    {"WKT_VAL_WKT0_MSB",&SYSCON->WKT_VAL_WKT0_MSB },
    {"WKT_VAL_WKT1",&SYSCON->WKT_VAL_WKT1 },
    {"WKT_STAT",&SYSCON->WKT_STAT },
    {"WKT_INTENSET",&SYSCON->WKT_INTENSET },
    {"WKT_INTSTAT",&SYSCON->WKT_INTSTAT },
    {"AUTOCLKGATEOVERRIDE",&SYSCON->AUTOCLKGATEOVERRIDE },
    {"GPIOPSYNC",&SYSCON->GPIOPSYNC },
    {"INVERTMAINCLK",&SYSCON->INVERTMAINCLK },
    {"DIEID",&SYSCON->DIEID },
    {"CPUCFG",&SYSCON->CPUCFG },
    {"CONFIGLOCKOUT",&SYSCON->CONFIGLOCKOUT },
};

void dump_syscon(void)
{
    PRINTF("SYSCON\r\n");
    for (int i = 0; i < NumberOfElements(SysconDesc); i++)
    {
        PRINTF("\t%s = %x\r\n", SysconDesc[i].reg_name, *SysconDesc[i].reg_addr);
    }

}

const reg_desc_t PmcDesc[] = {
    {"CTRL",         &PMC->CTRL  },
    {"DCDC0",        &PMC->DCDC0 },
    {"DCDC1",        &PMC->DCDC1 },
    {"BIAS",         &PMC->BIAS },
    {"LDOPMU",       &PMC->LDOPMU },
    {"LDOMEM",       &PMC->LDOMEM  },
    {"LDOCORE",      &PMC->LDOCORE },
    {"LDOFLASHNV",   &PMC->LDOFLASHNV },
    {"LDOFLASHCORE", &PMC->LDOFLASHCORE },
    {"LDOADC",       &PMC->LDOADC },
    {"BODVBAT",      &PMC->BODVBAT },
    {"BODMEM",       &PMC->BODMEM },
    {"BODCORE",      &PMC->BODCORE },
    {"FRO192M",      &PMC->FRO192M },
    {"FRO1M",        &PMC->FRO1M },
    {"FRO32K",       &PMC->FRO32K },
    {"XTAL32K",      &PMC->XTAL32K },
    {"ANAMUXCOMP",   &PMC->ANAMUXCOMP  },
    {"PWRSWACK",     &PMC->PWRSWACK },
    {"DPDWKSRC",     &PMC->DPDWKSRC },
    {"STATUSPWR",    &PMC->STATUSPWR },
    {"STATUSCLK",    &PMC->STATUSCLK },
    {"RESETCAUSE",   &PMC->RESETCAUSE },
    {"AOREG0",       &PMC->AOREG0 },
    {"AOREG1",       &PMC->AOREG1 },
    {"AOREG2",       &PMC->AOREG2 },
    {"FUSEOVW",      &PMC->FUSEOVW },
    {"DUMMYCTRL",    &PMC->DUMMYCTRL },
    {"DUMMYSTATUS",  &PMC->DUMMYSTATUS },
    {"DPDCTRL",      &PMC->DPDCTRL },
    {"PIOPORCAP",    &PMC->PIOPORCAP },
    {"PIORESCAP",    &PMC->PIORESCAP },
    {"TIMEOUTEVENTS",&PMC->TIMEOUTEVENTS },
    {"TIMEOUT",      &PMC->TIMEOUT },
    {"PDSLEEPCFG",   &PMC->PDSLEEPCFG },
    {"PDRUNCFG",     &PMC->PDRUNCFG },
    {"WAKEIOCAUSE",  &PMC->WAKEIOCAUSE },
    {"EFUSE0",       &PMC->EFUSE0 },
    {"EFUSE1",       &PMC->EFUSE1 },
    {"EFUSE2",       &PMC->EFUSE2 },
    {"CTRLNORST",    &PMC->CTRLNORST },
};

void dump_pmc(void)
{
    PRINTF("PMC\r\n");
    for (int i = 0; i < NumberOfElements(PmcDesc); i++)
    {
        PRINTF("\t%s = %x\r\n", PmcDesc[i].reg_name, *PmcDesc[i].reg_addr);
    }
}

const reg_desc_t BleDpDesc[] = {
    {"DP_TOP_SYSTEM_CTRL",  &BLE_DP_TOP->DP_TOP_SYSTEM_CTRL },
    {"PROP_MODE_CTRL",      &BLE_DP_TOP->PROP_MODE_CTRL },
    {"ACCESS_ADDRESS",      &BLE_DP_TOP->ACCESS_ADDRESS },
    {"ANT_PDU_DATA0",       &BLE_DP_TOP->ANT_PDU_DATA0 },
    {"ANT_PDU_DATA1",       &BLE_DP_TOP->ANT_PDU_DATA1 },
    {"ANT_PDU_DATA2",       &BLE_DP_TOP->ANT_PDU_DATA2 },
    {"ANT_PDU_DATA3",       &BLE_DP_TOP->ANT_PDU_DATA3 },
    {"ANT_PDU_DATA4",       &BLE_DP_TOP->ANT_PDU_DATA4 },
    {"ANT_PDU_DATA5",       &BLE_DP_TOP->ANT_PDU_DATA5 },
    {"ANT_PDU_DATA6",       &BLE_DP_TOP->ANT_PDU_DATA6 },
    {"ANT_PDU_DATA7",       &BLE_DP_TOP->ANT_PDU_DATA7 },
    {"CRC_SEED",            &BLE_DP_TOP->CRC_SEED },
    {"DP_FUNCTION_CTRL",    &BLE_DP_TOP->DP_FUNCTION_CTRL },
    {"DP_TEST_CTRL",        &BLE_DP_TOP->DP_TEST_CTRL },
    {"BLE_DP_STATUS1",      &BLE_DP_TOP->BLE_DP_STATUS1 },
    {"BLE_DP_STATUS2",      &BLE_DP_TOP->BLE_DP_STATUS2 },
    {"BLE_DP_STATUS3",      &BLE_DP_TOP->BLE_DP_STATUS3 },
    {"BLE_DP_STATUS4",      &BLE_DP_TOP->BLE_DP_STATUS4 },
    {"RX_FRONT_END_CTRL1",  &BLE_DP_TOP->RX_FRONT_END_CTRL1 },
    {"RX_FRONT_END_CTRL2",  &BLE_DP_TOP->RX_FRONT_END_CTRL2 },
    {"FREQ_DOMAIN_CTRL1",   &BLE_DP_TOP->FREQ_DOMAIN_CTRL1 },
    {"FREQ_DOMAIN_CTRL2",   &BLE_DP_TOP->FREQ_DOMAIN_CTRL2 },
    {"FREQ_DOMAIN_CTRL3",   &BLE_DP_TOP->FREQ_DOMAIN_CTRL3 },
    {"FREQ_DOMAIN_CTRL4",   &BLE_DP_TOP->FREQ_DOMAIN_CTRL4 },
    {"FREQ_DOMAIN_CTRL5",   &BLE_DP_TOP->FREQ_DOMAIN_CTRL5 },
    {"FREQ_DOMAIN_CTRL6",   &BLE_DP_TOP->FREQ_DOMAIN_CTRL6 },
    {"HP_MODE_CTRL1",       &BLE_DP_TOP->HP_MODE_CTRL1},
    {"HP_MODE_CTRL2",       &BLE_DP_TOP->HP_MODE_CTRL2 },
    {"FREQ_DOMAIN_STATUS1", &BLE_DP_TOP->FREQ_DOMAIN_STATUS1 },
    {"FREQ_DOMAIN_STATUS2", &BLE_DP_TOP->FREQ_DOMAIN_STATUS2 },
    {"DP_AA_ERROR_CTRL",    &BLE_DP_TOP->DP_AA_ERROR_CTRL },
    {"DP_INT",              &BLE_DP_TOP->DP_INT },
    {"DP_AA_ERROR_TH",      &BLE_DP_TOP->DP_AA_ERROR_TH },
    {"DP_ANTENNA_CTRL",     &BLE_DP_TOP->DP_ANTENNA_CTRL },
    {"ANTENNA_MAP01",       &BLE_DP_TOP->ANTENNA_MAP01 },
    {"ANTENNA_MAP23",       &BLE_DP_TOP->ANTENNA_MAP23 },
    {"ANTENNA_MAP45",       &BLE_DP_TOP->ANTENNA_MAP45 },
    {"ANTENNA_MAP67",       &BLE_DP_TOP->ANTENNA_MAP67 },
    {"LL_EM_BASE_ADDRESS",  &BLE_DP_TOP->LL_EM_BASE_ADDRESS },
    {"RX_EARLY_EOP",        &BLE_DP_TOP->RX_EARLY_EOP },
    {"ANT_DIVERSITY",       &BLE_DP_TOP->ANT_DIVERSITY },
    {"TX_M_TEST_CTRL",      &BLE_DP_TOP->TX_M_TEST_CTRL },
};
void dump_BleDp(void)
{
    PRINTF("BLE_TOP_DP\r\n");
    for (int i = 0; i < NumberOfElements(BleDpDesc); i++)
    {
        PRINTF("\t%s = %x\r\n", BleDpDesc[i].reg_name, *BleDpDesc[i].reg_addr);
    }
}
#endif

static psector_page_data_t  * mPage0Hdl = NULL;
#if PRELOAD_PFLASH
static psector_page_data_t  * mPFlashHdl = NULL;
#endif
psector_page_data_t * psector_GetPage0Contents(void)
{
    static psector_page_data_t  mPage0;
    psector_page_data_t *  page = &mPage0;

    psector_page_state_t page_state;
    do {
        page_state = psector_ReadData(PSECTOR_PAGE0_PART, 0, 0, sizeof(psector_page_t), page);
        if (page_state < PAGE_STATE_DEGRADED)
        {
            page = NULL;
            panic(0,0,0,0);
            break;
        }
    } while (0);

    return page;
}

#if PRELOAD_PFLASH
psector_page_data_t * psector_GetPFlashContents(void)
{
    static psector_page_data_t  mpFlash;
    psector_page_data_t *  page = &mpFlash;

    psector_page_state_t page_state;
    do {
        page_state = psector_ReadData(PSECTOR_PFLASH_PART, 0, 0, sizeof(psector_page_t), page);
        if (page_state < PAGE_STATE_DEGRADED)
        {
            page = NULL;
            panic(0,0,0,0);
            break;
        }
    } while (0);

    return page;
}
#endif

psector_page_data_t  *psector_GetPage0Handle(void)
{
    psector_page_data_t  * hdl = NULL;;
    do {
        if (mPage0Hdl != NULL)
        {
            hdl = mPage0Hdl;
            break;
        }

        if ((mPage0Hdl = psector_GetPage0Contents()) != NULL)
        {
            hdl = mPage0Hdl;
            break;
        }
    } while (0);
    return hdl;
}

int psector_CommitPageUpdates(psector_page_data_t * page, psector_partition_id_t id)
{
    int status = -1;
    do {
        if (!page) break;

        page->hdr.version ++;
        page->hdr.checksum = psector_CalculateChecksum((psector_page_t *)page);

        if(psector_WriteUpdatePage(id, (psector_page_t *)page) != WRITE_OK)
        {
            break;
        }
        status = 0;
    } while (0);


    return status;
}

int psector_SetOtaEntry(image_directory_entry_t * ota_entry, bool commit)
{
    int res = -1;

    do {
        mPage0Hdl = psector_GetPage0Handle();
        if (mPage0Hdl == NULL)
            break;

        mPage0Hdl->page0_v3.ota_entry.img_base_addr = ota_entry->img_base_addr;
        mPage0Hdl->page0_v3.ota_entry.img_nb_pages  = ota_entry->img_nb_pages;
        mPage0Hdl->page0_v3.ota_entry.flags         = ota_entry->flags;
        mPage0Hdl->page0_v3.ota_entry.img_type      = ota_entry->img_type;

        if (commit)
        {
            if (psector_CommitPageUpdates(mPage0Hdl, PSECTOR_PAGE0_PART) < 0)
                break;
        }
        res =  0;

    } while (0);
    return res;
}

bool psector_SetPreferredApp(uint8_t preferred_app,
                             bool commit_now,
                             pfImgValidationCb  imgValCb,
                             void * imgValidationArgs )
{
    bool res = false;
    do {
        uint32_t image_addr = IMAGE_INVALID_ADDR;

        mPage0Hdl = psector_GetPage0Handle();
        if (mPage0Hdl == NULL)
        {
            res = false;
            break;
        }

        if (preferred_app == mPage0Hdl->page0_v3.preferred_app_index)
        {
            res = false;  /* do nothing : already set so do not reset */
            break;
        }
        /* Try to find an entry in the img directory */
        image_directory_entry_t * dir_entry = NULL;
        image_directory_entry_t * dir_entry_iterator = NULL;
        for (int i = 0; i < IMG_DIRECTORY_MAX_SIZE; i++)
        {
            dir_entry_iterator = mPage0Hdl->page0_v3.img_directory+i;
            if (dir_entry_iterator->img_type == preferred_app)
            {
                dir_entry = dir_entry_iterator;
                break;
            }
        }
        if (dir_entry && (dir_entry->flags & IMG_FLAG_BOOTABLE))
        {
            /* Do a quick image validation : pass no certificate pointer */
            if (imgValCb != NULL)
                image_addr = (* imgValCb )(dir_entry->img_base_addr,
                                           imgValidationArgs);
        }
        if (image_addr == IMAGE_INVALID_ADDR) break;

        mPage0Hdl->page0_v3.preferred_app_index = preferred_app;
        if (commit_now)
        {
            if (psector_CommitPageUpdates(mPage0Hdl, PSECTOR_PAGE0_PART) != 0)
                break;
        }
        res = true;
    } while (0);
    return res;
}

/*
 * BOARD_Get_BLE_MAC_Id tries to find a valid BLE MAC address by looking successively into
 * the PFLASH, if an address is founnd it exists. Otherwise if keeps looking for one into the
 * CONFIG page (page N-2), if that fails too then it generates a random address whose OUI is
 * forced (3 MSB bytes of the address).
 *
 */
#if !defined CPU_JN518X_REV || (CPU_JN518X_REV > 1)

int psector_WriteBleMacAddress( uint8_t * src_mac_address)
{
    int res = -1;
    psector_page_data_t page; /* Stored in the stack temporarily */

    do {
        psector_page_state_t pg_state;

        pg_state = psector_ReadData(PSECTOR_PFLASH_PART,
                                    0,
                                    0,
                                    sizeof(psector_page_data_t),
                                    &page);
        if (pg_state < PAGE_STATE_DEGRADED) break;

        uint8_t * dst = (uint8_t*)&page.pFlash.ble_mac_id;

        for (int i = 0; i < sizeof(uint64_t); i++)
        {
            dst[i] = src_mac_address[i];
        }
        if (psector_CommitPageUpdates(&page,  PSECTOR_PFLASH_PART) < 0)
            break;

        res = 0;

    } while (0);

    return res;

}

int BOARD_Get_BLE_MAC_Id(uint8_t mac_addr[BLE_MACID_SZ])
{
    int res = -1;
    uint8_t buf[8];

    FLib_MemSet((uint8_t*)mac_addr, 0, BLE_MACID_SZ);

    do {
        /* Try to read form pFlash Customer MAC address */
        psector_page_state_t pg_state;
#define CUSTOMER_BLE_MACID_OFFSET  ((size_t)&((psector_page_data_t*)0)->pFlash.ble_mac_id)
        pg_state = psector_ReadData(PSECTOR_PFLASH_PART, 0, CUSTOMER_BLE_MACID_OFFSET, sizeof(uint64_t), buf);
        if (pg_state > 1)
        {
            if (!FLib_MemCmpToVal((uint8_t *)buf, 0, sizeof(uint64_t)))
            {
                res = 0;
                break;
            }
        }
        /* If unset by Customer continue to read Manufacturer's */

        FLib_MemCpy((uint8_t *)buf, MANUFACTURER_BLE_MACID_ADRESS, sizeof(uint64_t));
        if (!FLib_MemCmpToVal(buf, 0, sizeof(uint64_t)))
        {
            res = 1;
            FLib_MemCpy((uint8_t *)mac_addr, (uint8_t *)&buf[2], BLE_MACID_SZ);
            break;
        }
        else
        {
            uint8_t randomNb[4];
            const uint8_t oui[3] = { gBD_ADDR_NXP_OUI_c };
            RNG_GetRandomNo((uint32_t*)&randomNb[0]);
            buf[0] = buf[1] = 0;
            FLib_MemCpy(&buf[2], (void *)oui, 3);
            FLib_MemCpy(&buf[5], (void *)&randomNb[0], 3);

            if (psector_WriteBleMacAddress(&buf[0]) < 0)
            {
                res = -1;
                break;
            }
            res = 2;
            break;
        }
    } while (0);
    if (res >= 0)
    {
        for (int i = 0; i < BLE_MACID_SZ; i++)
        {
            mac_addr[i] = buf[BLE_MACID_SZ+1-i];
        }
    }
    return res;
}

void BOARD_DisplayMCUUid(uint8_t mac_id[BLE_MACID_SZ])
{
    PRINTF("Device_MAC_ID = ");
    for (int i=0; i < BLE_MACID_SZ-1; i++)
    {
        PRINTF("%02x:", mac_id[BLE_MACID_SZ - 1 - i]);
    }
    PRINTF("%02x\r\n", mac_id[0]);
}

void BOARD_GetMCUUid(uint8_t* aOutUid16B, uint8_t* pOutLen)
{
    uint8_t mac_id[BD_ADDR_SIZE] = { 0 };

    if ( BOARD_Get_BLE_MAC_Id(mac_id) >= 0)
    {
        static int already_displayed = 0;
        if (!already_displayed)
        {
            BOARD_DisplayMCUUid(mac_id);
            already_displayed = 1;
        }

        *pOutLen = BD_ADDR_SIZE;
        FLib_MemCpy(aOutUid16B, mac_id, BD_ADDR_SIZE);
    }
}
#endif
