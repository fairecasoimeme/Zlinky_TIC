/*! *********************************************************************************
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2017 NXP
* All rights reserved.
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */


/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
********************************************************************************** */

#ifndef CPU_HYPNOS_cm33
#include "fsl_gpio.h"
#if !defined (FSL_FEATURE_SOC_SYSCON_COUNT) || (FSL_FEATURE_SOC_SYSCON_COUNT < 1)
#include "fsl_port.h"
#else
#include "fsl_iocon.h"
#include "fsl_inputmux.h"
#if defined (FSL_FEATURE_SOC_PINT_COUNT) &&  (FSL_FEATURE_SOC_PINT_COUNT > 0)
#include "fsl_pint.h"
#endif
#endif
#endif
#include "fsl_os_abstraction.h"

#include "GPIO_Adapter.h"
#include "FunctionLib.h"
#include "fsl_debug_console.h"


/*! *********************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
********************************************************************************** */
#define gGpio_FoundPortIsr_c  0x01
#define gGpio_FoundSimilar_c  0x02

#ifndef BIT
#define BIT(x) (1<<(x))
#endif

/*! *********************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
********************************************************************************** */


/*! *********************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
********************************************************************************** */
#if !defined (CPU_HYPNOS_cm33)
void Gpio_CommonIsr(void);
static gpioStatus_t Gpio_InstallPortISR(IRQn_Type irqId, uint32_t nvicPrio);
#if defined (FSL_FEATURE_SOC_PINT_COUNT)  && (FSL_FEATURE_SOC_PINT_COUNT > 0)
    pinInterrupt_t GpioGetInterruptType(gpioPort_t gpioPort, uint8_t gpioPin);
    void GpioSetInterruptType(gpioPort_t gpioPort, uint8_t gpioPin, pinInterrupt_t int_mode);
#elif (FSL_FEATURE_SOC_INTMUX_COUNT <= 0) && (FSL_FEATURE_SOC_SYSCON_COUNT > 0)
    void           GpioSetInterruptType(GPIO_Type *base, uint8_t gpioPin, pinInterrupt_t int_mode);
    pinInterrupt_t GpioGetInterruptType(GPIO_Type *base, gpioPort_t gpioPort, uint8_t gpioPin);
#endif
#endif
/*! *********************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
********************************************************************************** */

#if defined(CPU_JN518X)
/* bit field of IO sources for the wakeup */
uint32_t gpio_wakeiocause             = 0;
uint32_t gpio_wakeupsrc               = 0;
#endif


#if !defined (CPU_HYPNOS_cm33)

#if defined (FSL_FEATURE_SOC_LPC_GPIO_COUNT) &&  (FSL_FEATURE_SOC_LPC_GPIO_COUNT > 0)
static GPIO_Type *const maGpioBases[] = GPIO_BASE_PTRS;
static const uint8_t gpioPinsPerPort[gpioPort_Invalid_c] =
{
    FSL_FEATURE_GPIO_PIO_COUNT, /* gpioPort_A_c */
};

#if defined (FSL_FEATURE_SOC_PINT_COUNT) &&  (FSL_FEATURE_SOC_PINT_COUNT > 0)
static gpioIsr_t mGpioIsr[gGpioMaxIsrEntries_c];
static uint16_t mGpioIsrCount = 0;
static IRQn_Type maPortIrqId[] = PINT_IRQS;
#endif

#else
#if (FSL_FEATURE_SOC_INTMUX_COUNT <= 0) && (FSL_FEATURE_SOC_SYSCON_COUNT > 0)
typedef GPIO_Type PORT_Type;
#define PORT_BASE_PTRS GPIO_BASE_PTRS
#define PORT_IRQS GPIO_IRQS

static const uint8_t gpioPinsPerPort[gpioPort_Invalid_c] =
{
    32, /* gpioPort_A_c */
    3,  /* gpioPort_B_c */
    0,  /* gpioPort_C_c */
    0,  /* gpioPort_D_c */
    0,  /* gpioPort_E_c */
};

#endif

static gpioIsr_t mGpioIsr[gGpioMaxIsrEntries_c];
static uint16_t mGpioIsrCount = 0;
static PORT_Type *const maPortBases[] = PORT_BASE_PTRS;
static GPIO_Type *const maGpioBases[] = GPIO_BASE_PTRS;
static IRQn_Type maPortIrqId[] = PORT_IRQS;
#endif
#endif

/*! *********************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
********************************************************************************** */

#if !defined FSL_FEATURE_SOC_SYSCON_COUNT || (FSL_FEATURE_SOC_SYSCON_COUNT < 1)
/* Section for Kinetis boards only ( exclude LPC and QN ) */

/*! *********************************************************************************
* \brief
*
* \param[in]
*
* \return  logical level of the input pin
*
********************************************************************************** */
uint32_t GpioGetPinPCR(gpioPort_t gpioPort, uint8_t gpioPin)
{
#ifndef CPU_HYPNOS_cm33
    PORT_Type *base = maPortBases[gpioPort];
    return (uint32_t)base->PCR[gpioPin];
#else
    return 0;
#endif
}
/*! *********************************************************************************
* \brief
*
* \param[in]
*
* \return  logical level of the input pin
*
********************************************************************************** */
void GpioSetPinPCR(gpioPort_t gpioPort, uint8_t gpioPin, uint32_t pcr)
{
#ifndef CPU_HYPNOS_cm33
    PORT_Type *base = maPortBases[gpioPort];
    base->PCR[gpioPin] = pcr;
#endif
}
/*! *********************************************************************************
* \brief
*
* \param[in]
*
* \return  logical level of the input pin
*
********************************************************************************** */
void GpioSetPinPCR_ISF_Preserved(gpioPort_t gpioPort, uint8_t gpioPin, uint32_t pcr)
{
#ifndef CPU_HYPNOS_cm33
    PORT_Type *base = maPortBases[gpioPort];
    base->PCR[gpioPin] = pcr & (~PORT_PCR_ISF_MASK);
#endif
}

void GpioSetSlewRate(gpioPort_t gpioPort, uint8_t gpioPin, pinSlewRate_t slewRate)
{
#ifndef CPU_HYPNOS_cm33
    PORT_Type *base = maPortBases[gpioPort];

    if (slewRate < pinSlewRate_Invalid_c)
    {
        base->PCR[gpioPin] &= ~PORT_PCR_SRE_MASK;
        base->PCR[gpioPin] |= PORT_PCR_SRE(slewRate);
    }
#endif
}

void GpioSetOpenDrain(gpioPort_t gpioPort, uint8_t gpioPin, bool_t enable)
{
#ifndef CPU_HYPNOS_cm33
#if defined(FSL_FEATURE_PORT_HAS_OPEN_DRAIN) && FSL_FEATURE_PORT_HAS_OPEN_DRAIN
    PORT_Type *base = maPortBases[gpioPort];

    if (enable)
    {
        base->PCR[gpioPin] |= PORT_PCR_ODE_MASK;
    }
    else
    {
        base->PCR[gpioPin] &= ~PORT_PCR_ODE_MASK;
    }
#endif /* FSL_FEATURE_PORT_HAS_OPEN_DRAIN */
#endif
}

/*! *********************************************************************************
* \brief
*
* \param[in]
*
* \return  logical level of the input pin
*
********************************************************************************** */

void GpioSetPassiveFilter(gpioPort_t gpioPort, uint8_t gpioPin, bool_t enable)
{
#ifndef CPU_HYPNOS_cm33
    PORT_Type *base = maPortBases[gpioPort];

    if (enable)
    {
        base->PCR[gpioPin] |= PORT_PCR_PFE_MASK;
    }
    else
    {
        base->PCR[gpioPin] &= ~PORT_PCR_PFE_MASK;
    }
#endif
}
#endif /* ! FSL_FEATURE_SOC_SYSCON_COUNT */

#if defined (FSL_FEATURE_SOC_PINT_COUNT)  && (FSL_FEATURE_SOC_PINT_COUNT > 0)
pinInterrupt_t GpioGetInterruptType(gpioPort_t gpioPort, uint8_t gpioPin)
{
    pinInterrupt_t res = pinInt_Invalid_c;
    do {
        int i;
        if (gpioPort >= gpioPort_Invalid_c) break;
        if (gpioPin >= gpioPinsPerPort[gpioPort]) break;

        INPUTMUX_Init(INPUTMUX);

        /* Read to which of the FSL_FEATURE_PINT_NUMBER_OF_CONNECTED_OUTPUTS (4)
         *  PIN Interrupts the PIO is multiplexed to by Input Mux
         */
        for (i = 0; i < FSL_FEATURE_PINT_NUMBER_OF_CONNECTED_OUTPUTS; i++)
        {
            if ((uint8_t)INPUTMUX->PINTSEL[i] == gpioPin) break;
        }
        if (i >=  FSL_FEATURE_PINT_NUMBER_OF_CONNECTED_OUTPUTS)
        {
            res = pinInt_Disabled_c;
            break;
        }
        pint_pin_enable_t enable_cfg;
        pint_cb_t pint_cb;
        /* Read from PINT IENR, IENF, SIENF, CIENF to determine the interrupt policy */
        PINT_PinInterruptGetConfig(PINT, (pint_pin_int_t)i, &enable_cfg, &pint_cb);

    } while (0);

    INPUTMUX_Deinit(INPUTMUX);

    return res;
}
void GpioSetInterruptType(gpioPort_t gpioPort, uint8_t gpioPin, pinInterrupt_t int_mode)
{
    do {
        int i;
        if (gpioPort >= gpioPort_Invalid_c) break;
        if (gpioPin >= gpioPinsPerPort[gpioPort]) break;
        if ((int_mode != pinInt_Disabled_c) &&
            (int_mode != pinInt_LogicZero_c) &&
            (int_mode != pinInt_RisingEdge_c) &&
            (int_mode != pinInt_FallingEdge_c) &&
            (int_mode != pinInt_EitherEdge_c) &&
            (int_mode != pinInt_LogicOne_c)) break;

        INPUTMUX_Init(INPUTMUX);
        /* Read to which of the FSL_FEATURE_PINT_NUMBER_OF_CONNECTED_OUTPUTS (4)
         *  PIN Interrupts the PIO is multiplexed to by Input Mux
         */
         pint_pin_enable_t prev_enable_cfg; /* pinInterrupt_t made to match pint_pin_enable_t on LPC PINT */
         pint_cb_t pint_cb;
         for (i = 0; i < FSL_FEATURE_PINT_NUMBER_OF_CONNECTED_OUTPUTS; i++)
         {
             if ((uint8_t)INPUTMUX->PINTSEL[i] == gpioPin) break;
         }
         if (i >= FSL_FEATURE_PINT_NUMBER_OF_CONNECTED_OUTPUTS) break;

         PINT_PinInterruptGetConfig(PINT, (pint_pin_int_t)i, &prev_enable_cfg, &pint_cb);
         if (prev_enable_cfg != (pint_pin_enable_t)int_mode)
         {
             PINT_PinInterruptConfig(PINT, (pint_pin_int_t)i, (pint_pin_enable_t)int_mode, pint_cb);
         }
    } while (0);
    INPUTMUX_Deinit(INPUTMUX);
}
#endif

pinInterrupt_t GpioGetPinInterrupt(gpioPort_t gpioPort, uint8_t gpioPin)
{
    uint32_t result = pinInt_Disabled_c;
#if defined (FSL_FEATURE_SOC_PINT_COUNT)  && (FSL_FEATURE_SOC_PINT_COUNT > 0)
    result = GpioGetInterruptType(gpioPort, gpioPin);
#else
    PORT_Type *base = maPortBases[gpioPort];
#if (FSL_FEATURE_SOC_INTMUX_COUNT <= 0) && (FSL_FEATURE_SOC_SYSCON_COUNT > 0)
    result = GpioGetInterruptType( base, gpioPort, gpioPin);
#else
    result = (base->PCR[gpioPin] & PORT_PCR_IRQC_MASK) >> PORT_PCR_IRQC_SHIFT;
#endif
#endif
    return (pinInterrupt_t)result;
}

void GpioSetPinInterrupt(gpioPort_t gpioPort, uint8_t gpioPin, pinInterrupt_t int_mode)
{
#if defined (FSL_FEATURE_SOC_PINT_COUNT)  && (FSL_FEATURE_SOC_PINT_COUNT > 0)
    GpioSetInterruptType(gpioPort, gpioPin, int_mode);
#else
#if (FSL_FEATURE_SOC_INTMUX_COUNT <= 0) && (FSL_FEATURE_SOC_SYSCON_COUNT > 0)
    GpioSetInterruptType(maPortBases[gpioPort], gpioPin, int_mode);
    GPIO_EnableInterrupt(maPortBases[gpioPort], (1 << gpioPin));
#else
    PORT_SetPinInterruptConfig(maPortBases[gpioPort], gpioPin, (port_interrupt_t)(int_mode));
#endif
#endif
}



/*! *********************************************************************************
* \brief
*
* \param[in]
*
* \return  logical level of the input pin
*
********************************************************************************** */
void GpioSetPinPullMode(gpioPort_t gpioPort, uint8_t gpioPin, pinPullSelect_t pullSelect)
{
#ifdef  CPU_HYPNOS_cm33
        return;
#else
  #if (FSL_FEATURE_SOC_INTMUX_COUNT <= 0) && (FSL_FEATURE_SOC_SYSCON_COUNT > 0)
    IOCON_PullSet(IOCON, gpioPort, gpioPin, pullSelect);
  #else
    uint32_t pcr;
    PORT_Type *base = maPortBases[gpioPort];
    pcr = base->PCR[gpioPin];
    pcr &= ~(PORT_PCR_PS_MASK | PORT_PCR_PE_MASK);
    if (pullSelect < pinPull_Disabled_c)
    {
        pcr |= pullSelect | PORT_PCR_PE_MASK;
    }
    base->PCR[gpioPin] = pcr;
  #endif
#endif

}
/*! *********************************************************************************
* \brief
*
* \param[in]
*
* \return  logical level of the input pin
*
********************************************************************************** */

void GpioSetPinMux(gpioPort_t gpioPort, uint8_t gpioPin, pinMux_t pinMux)
{
#ifdef  CPU_HYPNOS_cm33
    return;
#else
  #if (FSL_FEATURE_SOC_INTMUX_COUNT <= 0) && (FSL_FEATURE_SOC_SYSCON_COUNT > 0)
    IOCON_FuncSet(IOCON, gpioPort, gpioPin, pinMux);
  #else
    PORT_SetPinMux(maPortBases[gpioPort], (uint32_t)gpioPin, (port_mux_t)pinMux);
  #endif
#endif
}

/*! *********************************************************************************
* \brief
*
* \param[in]
*
* \return  logical level of the input pin
*
********************************************************************************** */

void GpioSetPinMux_ISF_Preserved(gpioPort_t gpioPort, uint8_t gpioPin, pinMux_t pinMux)
{
#ifdef CPU_HYPNOS_cm33
#else
#if (FSL_FEATURE_SOC_INTMUX_COUNT <= 0) && (FSL_FEATURE_SOC_SYSCON_COUNT > 0)
    // on QN9080 ISF is not changed when setting pin mux
    GpioSetPinMux(gpioPort, gpioPin, pinMux);
#else
    PORT_Type *base = maPortBases[gpioPort];
    base->PCR[gpioPin] = (base->PCR[gpioPin] & ~(PORT_PCR_MUX_MASK | PORT_PCR_ISF_MASK)) | PORT_PCR_MUX(pinMux);
#endif
#endif
}
/*! *********************************************************************************
* \brief
*
* \param[in]  pInputConfig             pointer to a gpioInputPinConfig_t structure
*
* \return  logical level of the input pin
*
********************************************************************************** */

void GpioClearPinIntFlag(const gpioInputPinConfig_t* pInputConfig)
{
#ifdef CPU_HYPNOS_cm33
    return;
#else
#if defined (FSL_FEATURE_SOC_PINT_COUNT) && (FSL_FEATURE_SOC_PINT_COUNT > 0)
    PINT_PinInterruptClrStatus(PINT, pInputConfig->pinIntSelect);
#elif (FSL_FEATURE_SOC_INTMUX_COUNT <= 0) && (FSL_FEATURE_SOC_SYSCON_COUNT > 0)
    GPIO_ClearPinsInterruptFlags(maPortBases[pInputConfig->gpioPort], (1 << pInputConfig->gpioPin));
  #else
    PORT_ClearPinsInterruptFlags(maPortBases[pInputConfig->gpioPort], 1<<pInputConfig->gpioPin);
  #endif
#endif
}

#if defined(CPU_JN518X)
uint32_t GpioGetIoWakeupSource(void)
{
    return gpio_wakeupsrc;
}

bool_t GpioIsPinCauseOfWakeup(const gpioInputPinConfig_t* pInputConfig)
{
    bool_t res = FALSE;
#if defined (CPU_JN518X)
    uint32_t pin_bit =  BIT(pInputConfig->gpioPin);
    if (gpio_wakeiocause & pin_bit)
    {
        gpio_wakeiocause &= ~pin_bit;
        res = TRUE;
    }
#endif
    return res;
}

void GpioNotifyIoWakeupSource(uint32_t wkup_io_src)
{
#if defined(CPU_JN518X)
    gpio_wakeiocause = wkup_io_src;
#endif
}
#endif

/*! *********************************************************************************
* \brief
*
* \param[in]  pInputConfig             pointer to a gpioInputPinConfig_t structure
*
* \return  logical level of the input pin
*
********************************************************************************** */

uint32_t GpioIsPinIntPending(const gpioInputPinConfig_t* pInputConfig)
{
#ifdef CPU_HYPNOS_cm33
    return 0;
#else
#if defined (FSL_FEATURE_SOC_PINT_COUNT) && (FSL_FEATURE_SOC_PINT_COUNT > 0)
    return PINT_PinInterruptGetStatus(PINT, pInputConfig->pinIntSelect);
#else
    uint32_t isfr;
  #if (FSL_FEATURE_SOC_INTMUX_COUNT <= 0) && (FSL_FEATURE_SOC_SYSCON_COUNT > 0)
    isfr = GPIO_GetPinsInterruptFlags(maPortBases[pInputConfig->gpioPort]);
  #else
    isfr = PORT_GetPinsInterruptFlags(maPortBases[pInputConfig->gpioPort]);
  #endif

    return ((isfr >> pInputConfig->gpioPin) & 0x01U);
#endif
}

/*! *********************************************************************************
* \brief  Set the logical level of the output pin
*
* \param[in]  pOutputConfig             pointer to a gpioOutputPinConfig_t structure
*
* \return  logical level of the input pin
*
********************************************************************************** */

void GpioSetPinOutput(const gpioOutputPinConfig_t* pOutputConfig)
{
#if defined (FSL_FEATURE_SOC_LPC_GPIO_COUNT) && (FSL_FEATURE_SOC_LPC_GPIO_COUNT > 0)
    GPIO_PortSet(maGpioBases[pOutputConfig->gpioPort], pOutputConfig->gpioPort, (1<< pOutputConfig->gpioPin));
#else
    #ifndef CPU_HYPNOS_cm33
    GPIO_SetPinsOutput(maGpioBases[pOutputConfig->gpioPort], (1<< pOutputConfig->gpioPin));
    #endif
#endif
}
/*! *********************************************************************************
* \brief  Clear the logical level of the output pin
*
* \param[in]  pOutputConfig             pointer to a gpioOutputPinConfig_t structure
*
* \return  logical level of the input pin
*
********************************************************************************** */

void GpioClearPinOutput(const gpioOutputPinConfig_t* pOutputConfig)
{
#if defined (FSL_FEATURE_SOC_LPC_GPIO_COUNT) && (FSL_FEATURE_SOC_LPC_GPIO_COUNT > 0)
    GPIO_PortClear(maGpioBases[pOutputConfig->gpioPort], pOutputConfig->gpioPort, (1<< pOutputConfig->gpioPin));
#else
    #ifndef CPU_HYPNOS_cm33
    GPIO_ClearPinsOutput(maGpioBases[pOutputConfig->gpioPort], (1<< pOutputConfig->gpioPin));
    #endif
#endif
}
/*! *********************************************************************************
* \brief  Toggle the logical level of the output pin
*
* \param[in]  pOutputConfig             pointer to a gpioOutputPinConfig_t structure
*
* \return  logical level of the input pin
*
********************************************************************************** */

void GpioTogglePinOutput(const gpioOutputPinConfig_t* pOutputConfig)
{

#if defined (FSL_FEATURE_SOC_LPC_GPIO_COUNT) && (FSL_FEATURE_SOC_LPC_GPIO_COUNT > 0)
    GPIO_PortToggle(maGpioBases[pOutputConfig->gpioPort], pOutputConfig->gpioPort, (1<< pOutputConfig->gpioPin));
#else
    #ifndef CPU_HYPNOS_cm33
    GPIO_TogglePinsOutput(maGpioBases[pOutputConfig->gpioPort], (1<< pOutputConfig->gpioPin));
    #endif
#endif
}

/*! *********************************************************************************
* \brief  Reads the logical level of the output pin
*
* \param[in]  pOutputConfig             pointer to a gpioOutputPinConfig_t structure
*
* \return  logical level of the input pin
*
********************************************************************************** */
uint32_t GpioReadOutputPin(const gpioOutputPinConfig_t* pOutputConfig)
{
#if !defined (CPU_HYPNOS_cm33) && !defined (FSL_FEATURE_SOC_LPC_GPIO_COUNT)
    return GPIO_ReadPinInput(maGpioBases[pOutputConfig->gpioPort], pOutputConfig->gpioPin);
#else
    return 0;
#endif
}

/*! *********************************************************************************
* \brief  Reads the logical level of the input pin
*
* \param[in]  pInputConfig             pointer to a gpioInputPinConfig_t structure
*
* \return  logical level of the input pin
*
********************************************************************************** */

uint32_t GpioReadPinInput(const gpioInputPinConfig_t* pInputConfig)
{
#if defined (FSL_FEATURE_SOC_LPC_GPIO_COUNT) && (FSL_FEATURE_SOC_LPC_GPIO_COUNT > 0)
    return GPIO_PinRead(GPIO, pInputConfig->gpioPort, pInputConfig->gpioPin);
#else
    #ifndef CPU_HYPNOS_cm33
    return GPIO_ReadPinInput(maGpioBases[pInputConfig->gpioPort], pInputConfig->gpioPin);
    #else
    return 0;
    #endif
#endif
}

/*! *********************************************************************************
* \brief  Set Pin drive strength
********************************************************************************** */
void GpioSetDriveStrength(gpioPort_t gpioPort, uint8_t gpioPin, pinDriveStrength_t pinDriveStrength)
{
#if defined (FSL_FEATURE_SOC_LPC_GPIO_COUNT) && (FSL_FEATURE_SOC_LPC_GPIO_COUNT > 0)
    return;
#else
#if (FSL_FEATURE_SOC_INTMUX_COUNT <= 0) && (FSL_FEATURE_SOC_SYSCON_COUNT > 0)
    IOCON_DriveSet(IOCON, gpioPort, (uint32_t)gpioPin, pinDriveStrength);
#else
  #if defined(FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH) && FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
    PORT_SetPinDriveStrength(maPortBases[gpioPort], (uint32_t)gpioPin, pinDriveStrength);
  #endif
#endif
#endif
}

/*! *********************************************************************************
* \brief  Initializes input pins
*
* \param[in]  pInputConfig             table of input pins configuration structures
* \param[in]  noOfElements             number of elements of the table
*
* \return  install status
*
********************************************************************************** */

bool_t GpioInputPinInit(const gpioInputPinConfig_t* pInputConfig, uint32_t noOfElements )
{

#ifdef CPU_HYPNOS_cm33
    return TRUE;
#else
    gpio_pin_config_t gpioConfig;

#if defined (FSL_FEATURE_SOC_PINT_COUNT) &&  (FSL_FEATURE_SOC_PINT_COUNT > 0)
    static uint8_t gpio_int_nb = 0;
    int inputmux_in_use = 0;
#endif
    gpioConfig.pinDirection = kGPIO_DigitalInput; //default pin direction
    bool_t ret = FALSE;
    do {
        if(pInputConfig == NULL)
            break;

        for( ;noOfElements > 0; noOfElements--)
        {
#if (FSL_FEATURE_SOC_INTMUX_COUNT <= 0) && (FSL_FEATURE_SOC_SYSCON_COUNT > 0)
            if (pInputConfig->gpioPin >= gpioPinsPerPort[pInputConfig->gpioPort]) break;
#else
            if (pInputConfig->gpioPin >= NumberOfElements(((PORT_Type*)0)->PCR)) break;
#endif
            if (pInputConfig->gpioPort >= FSL_FEATURE_SOC_GPIO_COUNT) break;
            if (pInputConfig->pullSelect >= pinPull_Invalid_c) break;
            if (pInputConfig->interruptModeSelect >= pinInt_Invalid_c) break;
            GpioSetPinPullMode(pInputConfig->gpioPort,
                               pInputConfig->gpioPin,
                               pInputConfig->pullSelect);
            GpioSetPinMux(pInputConfig->gpioPort, pInputConfig->gpioPin, pinMux_Gpio_c);
            //set Pin direction = kGPIO_DigitalInput
#if defined (FSL_FEATURE_SOC_LPC_GPIO_COUNT) && (FSL_FEATURE_SOC_LPC_GPIO_COUNT > 0)
            GPIO_PinInit(maGpioBases[pInputConfig->gpioPort],
                         pInputConfig->gpioPort,
                         pInputConfig->gpioPin,
                         &gpioConfig);
#else
            GPIO_PinInit(maGpioBases[pInputConfig->gpioPort],
                         pInputConfig->gpioPin,
                         &gpioConfig);
#endif
            if (pInputConfig->interruptModeSelect != pinInt_Disabled_c)
            {
#if defined (FSL_FEATURE_SOC_PINT_COUNT) &&  (FSL_FEATURE_SOC_PINT_COUNT > 0)
                if (inputmux_in_use == 0)
                {
                    INPUTMUX_Init(INPUTMUX);
                }
                if (gpio_int_nb == 0)
                {
                    PINT_Init(PINT); /* Only for first GPIO */
                }
                /* Bind Gpio pin to PINT select */
                INPUTMUX_AttachSignal(INPUTMUX,
                                      pInputConfig->pinIntSelect,
                                      (inputmux_connection_t)INPUTMUX_GpioPortPinToPintsel(pInputConfig->gpioPort,
                                                                    pInputConfig->gpioPin));
                gpio_int_nb++;
                inputmux_in_use ++;
#endif
                GpioSetPinInterrupt(pInputConfig->gpioPort,
                                    pInputConfig->gpioPin,
                                    pInputConfig->interruptModeSelect);
#if defined(CPU_JN518X)
                if (pInputConfig->is_wake_source)
                {
                    gpio_wakeupsrc |= BIT(pInputConfig->gpioPin);
                }
#endif
            }
            pInputConfig++;

        }
        ret = (noOfElements == 0);
    } while (0);
#if defined (FSL_FEATURE_SOC_PINT_COUNT) &&  (FSL_FEATURE_SOC_PINT_COUNT > 0)
    if (inputmux_in_use)
    {
        /* Turn-off clock to inputmux to save power. Clock is only needed to make changes */
        INPUTMUX_Deinit(INPUTMUX);
    }
#endif
    return ret;
#endif
}

/*! *********************************************************************************
* \brief  Initializes output pins
*
* \param[in]  pOutputConfig   table of output pins configuration structures
* \param[in]  noOfElements    number of elements of the table
* \return  install status
*
********************************************************************************** */

bool_t GpioOutputPinInit(const gpioOutputPinConfig_t* pOutputConfig, uint32_t noOfElements )
{
#if defined (CPU_HYPNOS_cm33)
    return TRUE;
#else
    gpio_pin_config_t gpioConfig;
    gpioConfig.pinDirection = kGPIO_DigitalOutput; //pin direction = out
    bool_t ret = FALSE;
    do {
        if(pOutputConfig == NULL) break;

        for(; noOfElements > 0; noOfElements--)
        {
            if (pOutputConfig->gpioPort >= FSL_FEATURE_SOC_GPIO_COUNT) break;
#if (FSL_FEATURE_SOC_INTMUX_COUNT <= 0) && (FSL_FEATURE_SOC_SYSCON_COUNT > 0)
            if (pOutputConfig->gpioPin >= gpioPinsPerPort[pOutputConfig->gpioPort]) break;
#else
            if (pOutputConfig->gpioPin >= NumberOfElements(((PORT_Type*)0)->PCR)) break;
            if (pOutputConfig->slewRate >= pinSlewRate_Invalid_c) break;
#endif
            if (pOutputConfig->driveStrength >= pinDriveStrength_Invalid_c) break;

            GpioSetPinMux(pOutputConfig->gpioPort,
                          pOutputConfig->gpioPin,
                          pinMux_Gpio_c);
            GpioSetDriveStrength(pOutputConfig->gpioPort,
                                 pOutputConfig->gpioPin,
                                 pOutputConfig->driveStrength);
#if !((FSL_FEATURE_SOC_INTMUX_COUNT <= 0) && (FSL_FEATURE_SOC_SYSCON_COUNT > 0))
            GpioSetSlewRate(pOutputConfig->gpioPort,
                            pOutputConfig->gpioPin,
                            pOutputConfig->slewRate);
#endif
            gpioConfig.outputLogic = pOutputConfig->outputLogic;
            GPIO_PinInit(maGpioBases[pOutputConfig->gpioPort],
                         pOutputConfig->gpioPort,
                         pOutputConfig->gpioPin,
                         &gpioConfig);

            pOutputConfig++;
        }
        ret = (noOfElements == 0);
    } while (0);
    return ret;
#endif
}


/*! *********************************************************************************
* \brief  Install the callback for the specified Pin Definition
*
* \param[in]  cb              The callback function to be installed
* \param[in]  priority        The priority used by the GPIO_IrqAdapter
* \param[in]  nvicPriority    The priority to be set in NVIC. Only most significant bits are used!
* \param[in]  pinDef          The KSDK pin definition
*
* \return  install status
*
********************************************************************************** */
gpioStatus_t GpioInstallIsr( pfGpioIsrCb_t cb,
                             uint8_t priority,
                             uint8_t nvicPriority,
                             const gpioInputPinConfig_t* pInputConfig )
{
#ifdef CPU_HYPNOS_cm33
    return gpio_success;
#else
    uint32_t i;
    uint8_t  found   = 0;
    uint8_t  pos     = mGpioIsrCount;
    gpioPort_t portId = pInputConfig->gpioPort;

#if defined (FSL_FEATURE_SOC_PINT_COUNT)  && (FSL_FEATURE_SOC_PINT_COUNT > 0)
    IRQn_Type irqNo  = maPortIrqId[pInputConfig->pinIntSelect];
    uint32_t pinMask = (1 << pInputConfig->gpioPin);
#else
    IRQn_Type irqNo  = maPortIrqId[portId];
    uint32_t pinMask = (1 << pInputConfig->gpioPin);
#endif

    /* Parse already installed Gpio interrupts to check if already in place */
    for( i=0; i<mGpioIsrCount; i++ )
    {
#if !(defined (FSL_FEATURE_SOC_PINT_COUNT)  && (FSL_FEATURE_SOC_PINT_COUNT > 0))
        /* search for port ISR already installed */
        if( mGpioIsr[i].port == portId )
        {
            found |= gGpio_FoundPortIsr_c;
        }
        /* search for insert position */
        if( (pos == mGpioIsrCount) && (mGpioIsr[i].prio >= priority) )
        {
            pos = i;
        }
#endif
#if (defined (FSL_FEATURE_SOC_PINT_COUNT)  && (FSL_FEATURE_SOC_PINT_COUNT > 0))
        /* search for an entry with the same callback installed for the same port with the same priority */
        if( (mGpioIsr[i].callback == cb) &&
            (mGpioIsr[i].port == portId) &&
            (mGpioIsr[i].prio == priority) &&
            (mGpioIsr[i].pinMask == pinMask))
        {
            pos = i;
            found |= gGpio_FoundSimilar_c;
            break;
        }
#endif
    }

    if( found & gGpio_FoundSimilar_c )
    {
        /* found the same ISR installed for the same port, but other pins */
        mGpioIsr[pos].pinMask |= pinMask;
    }
    else
    {
        if( mGpioIsrCount >= gGpioMaxIsrEntries_c )
        {
            return gpio_outOfMemory;
        }
        OSA_InterruptDisable();

        if( pos != mGpioIsrCount )
        {
           /* Shift all entries to the left, to obtain a sorted list */
            for( i=mGpioIsrCount; i>pos; i-- )
            {
                mGpioIsr[i] = mGpioIsr[i-1];
            }
        }

        /* install new callback */
        mGpioIsr[pos].callback = cb;
        mGpioIsr[pos].prio     = priority;
        mGpioIsr[pos].port     = portId;
        mGpioIsr[pos].pinMask  = pinMask;
        mGpioIsr[pos].irqId    = irqNo;
#if defined (FSL_FEATURE_SOC_PINT_COUNT)  && (FSL_FEATURE_SOC_PINT_COUNT > 0)
        mGpioIsr[pos].pintPinSelect = pInputConfig->pinIntSelect;
        pint_pin_enable_t enable_cfg;   /* pinInterrupt_t made to match pint_pin_enable_t on LPC PINT */
        pint_cb_t pint_cb = NULL;
        PINT_PinInterruptGetConfig(PINT,
                                   (pint_pin_int_t)i,
                                   &enable_cfg,
                                   &pint_cb);
        PINT_PinInterruptConfig(PINT,
                                pInputConfig->pinIntSelect,
                                (pint_pin_enable_t)pInputConfig->interruptModeSelect,
                                (pint_cb_t)cb);
#endif

        mGpioIsrCount++;
        OSA_InterruptEnable();
    }

    if( found )
    {
        /* The PORT ISR was already installed. Update NVIC priority if higher than the old one! */
        nvicPriority = nvicPriority >> (8 - __NVIC_PRIO_BITS);
        i = NVIC_GetPriority(irqNo);
        if( i > nvicPriority )
        {
            NVIC_SetPriority(irqNo, nvicPriority);
        }

#if    cPWR_FullPowerDownMode
        OSA_InstallIntHandler(irqNo, Gpio_CommonIsr);
        NVIC_EnableIRQ(irqNo);
#endif
        return gpio_success;
    }
    else
    {
        /* Install common PORT ISR */
        return Gpio_InstallPortISR(irqNo, nvicPriority);
    }
#endif
#endif
}

/*! *********************************************************************************
* \brief  Uninstall the callback for the specified Pin Definition
*
* \param[in]  pinDef    The KSDK pin definition
*
* \return  uninstall status
*
********************************************************************************** */

gpioStatus_t GpioUninstallIsr( const gpioInputPinConfig_t* pInputConfig )
{
#ifndef CPU_HYPNOS_cm33
    IRQn_Type irqNo;
    uint32_t  i, j;
    gpioPort_t portId = pInputConfig->gpioPort;
#if defined (FSL_FEATURE_SOC_PINT_COUNT)  && (FSL_FEATURE_SOC_PINT_COUNT > 0)
    irqNo  = maPortIrqId[pInputConfig->pinIntSelect];
    uint32_t pinMask = (1 << pInputConfig->pinIntSelect);
#else
    irqNo  = maPortIrqId[portId];
    uint32_t pinMask = (1 << pInputConfig->gpioPin);
#endif
    for( i=0; i<mGpioIsrCount; i++ )
    {
        if( (mGpioIsr[i].port == portId) && (mGpioIsr[i].pinMask & pinMask) )
        {
            OSA_InterruptDisable();
            /* uninstall ISR only for specified pins */
            mGpioIsr[i].pinMask &= ~pinMask;
            /* if no more pins are active, uninstall handler function */
            if( !mGpioIsr[i].pinMask )
            {
                irqNo = mGpioIsr[i].irqId;
                mGpioIsr[i].callback = NULL;

                /* Shift next entries to the left */
                for( j=i; j<mGpioIsrCount-1; j++ )
                {
                    mGpioIsr[j] = mGpioIsr[j+1];
                }
                mGpioIsrCount--;

                /* Search for other ISR installed for the same IRQ */
                for( j=0; j<mGpioIsrCount; j++ )
                {
                    if( irqNo == mGpioIsr[j].irqId )
                    {
                        irqNo = NotAvail_IRQn;
                        break;
                    }
                }

                /* If no other ISR was installed for this IRQ, disable IRQ in NVIC */
                if( irqNo != NotAvail_IRQn )
                {
                    NVIC_DisableIRQ(irqNo);
                }
            }
            OSA_InterruptEnable();
            return gpio_success;
        }
    }

    return gpio_notFound;
#else
    return gpio_success;
#endif
}

/*! *********************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
********************************************************************************* */

#ifndef CPU_HYPNOS_cm33
/*! *********************************************************************************
* \brief  Common GPIO ISR.
*
********************************************************************************** */
void Gpio_CommonIsr(void)
{
    uint32_t i;
    uint32_t flags;

    IRQn_Type irqId = (IRQn_Type)(__get_IPSR() - 16);
#if defined(FSL_FEATURE_SOC_INTMUX_COUNT) && FSL_FEATURE_SOC_INTMUX_COUNT
#define INSTANCE FSL_FEATURE_FLASH_CURRENT_CORE_ID
    const IRQn_Type s_intmuxIRQNumber[][FSL_FEATURE_INTMUX_CHANNEL_COUNT] = INTMUX_IRQS;
    if ((irqId >= s_intmuxIRQNumber[INSTANCE][0]) &&
        (irqId <= s_intmuxIRQNumber[INSTANCE][FSL_FEATURE_INTMUX_CHANNEL_COUNT-1]))
    {
        irqId += FSL_FEATURE_INTMUX_IRQ_START_INDEX;
    }
#endif

    /* Search for the highest priority ISR installed for the current PORT */
    for( i=0; i<mGpioIsrCount; i++ )
    {
#if defined (FSL_FEATURE_SOC_PINT_COUNT)  && (FSL_FEATURE_SOC_PINT_COUNT > 0)
        flags = PINT_PinInterruptGetStatusAll(PINT);
#else
#if (FSL_FEATURE_SOC_INTMUX_COUNT <= 0) && (FSL_FEATURE_SOC_SYSCON_COUNT > 0)
        flags = GPIO_GetPinsInterruptFlags(maPortBases[mGpioIsr[i].port]);
#else
        flags = PORT_GetPinsInterruptFlags(maPortBases[mGpioIsr[i].port]);
#endif
#endif
        if( ((1<<mGpioIsr[i].pintPinSelect) & flags) &&
            (mGpioIsr[i].irqId == irqId) )
        {
            mGpioIsr[i].callback();
            /* If other lower priority IRSs need to run, this common ISR will run again! */
            return;
        }
    }
}

/*! *********************************************************************************
* \brief  Install the Gpio_CommonIsr() ISR for the specified IRQ
*
* \param[in]  irqId    The CMSIS irq Id
* \param[in]  nvicPrio The priority to be set in NVIC
*
* \return  install status
*
********************************************************************************** */
static gpioStatus_t Gpio_InstallPortISR(IRQn_Type irqId, uint32_t nvicPrio)
{
    if( irqId != NotAvail_IRQn )
    {
        OSA_InstallIntHandler(irqId, Gpio_CommonIsr);

        /* Enable IRQ in NVIC and set priority */
        NVIC_ClearPendingIRQ(irqId);
        NVIC_EnableIRQ(irqId);
        NVIC_SetPriority(irqId, nvicPrio >> (8 - __NVIC_PRIO_BITS));
    }
    return gpio_success;
}
#endif /* #ifndef CPU_HYPNOS_cm33 */
