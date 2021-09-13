/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_INPUTMUX_CONNECTIONS_
#define _FSL_INPUTMUX_CONNECTIONS_

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.inputmux_connections"
#endif

/*!
 * @addtogroup inputmux_driver
 * @{
 */

#include <stddef.h>
#include "fsl_device_registers.h"

/*! @brief Periphinmux IDs */
#define PINTSEL_PMUX_ID (offsetof(INPUTMUX_Type, PINTSEL))           /*!< 0xC0U */
#define DMA_ITRIG_PMUX_ID (offsetof(INPUTMUX_Type, DMA_ITRIG_INMUX)) /*!< 0xE0U */
#define DMA_OTRIG_PMUX_ID (offsetof(INPUTMUX_Type, DMA_OTRIG_INMUX)) /*!< 0x160U */
#define FREQMEAS_PMUX_ID (offsetof(INPUTMUX_Type, FREQMEAS_REF))     /*!< 0x180U */
#define PMUX_SHIFT 20U                                               /*!< 20U */

#define INPUTMUX_GpioPortPinToPintsel(port, pin) ((pin) + (PINTSEL_PMUX_ID << PMUX_SHIFT))

/*! @brief INPUTMUX connections type */
typedef enum _inputmux_connection_t
{
    kINPUTMUX_ClkInToFreqmeas     = 0U + (FREQMEAS_PMUX_ID << PMUX_SHIFT), /*!< Clock Input to Frequency measure. */
    kINPUTMUX_Xtal32MhzToFreqmeas = 1U + (FREQMEAS_PMUX_ID << PMUX_SHIFT), /*!< XTAL 32MHZ to Frequency measure. */
    kINPUTMUX_Fro1MhzToFreqmeas   = 2U + (FREQMEAS_PMUX_ID << PMUX_SHIFT), /*!< Fro 1MHz to Frequency measure. */
    kINPUTMUX_32KhzOscToFreqmeas  = 3U + (FREQMEAS_PMUX_ID << PMUX_SHIFT), /*!< 32KHz OSC to Frequency measure. */
    kINPUTMUX_MainClkToFreqmeas   = 4U + (FREQMEAS_PMUX_ID << PMUX_SHIFT), /*!< Main Clock to Frequency measure. */
    kINPUTMUX_GpioPort0Pin4ToFreqmeas =
        5U + (FREQMEAS_PMUX_ID << PMUX_SHIFT), /*!< GPIO PORT 0 Pin 4 to Frequency measure. */
    kINPUTMUX_GpioPort0Pin20ToFreqmeas =
        6U + (FREQMEAS_PMUX_ID << PMUX_SHIFT), /*!< GPIO Port 0 Pin 20 to Frequency measure. */
    kINPUTMUX_GpioPort0Pin16ToFreqmeas =
        7U + (FREQMEAS_PMUX_ID << PMUX_SHIFT), /*!< GPIO Port 0 Pin 16 to Frequency measure. */
    kINPUTMUX_GpioPort0Pin15ToFreqmeas =
        8U + (FREQMEAS_PMUX_ID << PMUX_SHIFT), /*!< GPIO Port 0 Pin 15 to Frequency measure. */
    /*!< Pin Interrupt. */
    kINPUTMUX_GpioPort0Pin0ToPintsel  = INPUTMUX_GpioPortPinToPintsel(0, 0U),  /*!< Port 0 Pin 0 to PINT select */
    kINPUTMUX_GpioPort0Pin1ToPintsel  = INPUTMUX_GpioPortPinToPintsel(0, 1U),  /*!< Port 0 Pin 1 to PINT select */
    kINPUTMUX_GpioPort0Pin2ToPintsel  = INPUTMUX_GpioPortPinToPintsel(0, 2U),  /*!< Port 0 Pin 2 to PINT select */
    kINPUTMUX_GpioPort0Pin3ToPintsel  = INPUTMUX_GpioPortPinToPintsel(0, 3U),  /*!< Port 0 Pin 3 to PINT select */
    kINPUTMUX_GpioPort0Pin4ToPintsel  = INPUTMUX_GpioPortPinToPintsel(0, 4U),  /*!< Port 0 Pin 4 to PINT select */
    kINPUTMUX_GpioPort0Pin5ToPintsel  = INPUTMUX_GpioPortPinToPintsel(0, 5U),  /*!< Port 0 Pin 5 to PINT select */
    kINPUTMUX_GpioPort0Pin6ToPintsel  = INPUTMUX_GpioPortPinToPintsel(0, 6U),  /*!< Port 0 Pin 6 to PINT select */
    kINPUTMUX_GpioPort0Pin7ToPintsel  = INPUTMUX_GpioPortPinToPintsel(0, 7U),  /*!< Port 0 Pin 7 to PINT select */
    kINPUTMUX_GpioPort0Pin8ToPintsel  = INPUTMUX_GpioPortPinToPintsel(0, 8U),  /*!< Port 0 Pin 8 to PINT select */
    kINPUTMUX_GpioPort0Pin9ToPintsel  = INPUTMUX_GpioPortPinToPintsel(0, 9U),  /*!< Port 0 Pin 9 to PINT select */
    kINPUTMUX_GpioPort0Pin10ToPintsel = INPUTMUX_GpioPortPinToPintsel(0, 10U), /*!< Port 0 Pin 10 to PINT select */
    kINPUTMUX_GpioPort0Pin11ToPintsel = INPUTMUX_GpioPortPinToPintsel(0, 11U), /*!< Port 0 Pin 11 to PINT select */
    kINPUTMUX_GpioPort0Pin12ToPintsel = INPUTMUX_GpioPortPinToPintsel(0, 12U), /*!< Port 0 Pin 12 to PINT select */
    kINPUTMUX_GpioPort0Pin13ToPintsel = INPUTMUX_GpioPortPinToPintsel(0, 13U), /*!< Port 0 Pin 13 to PINT select */
    kINPUTMUX_GpioPort0Pin14ToPintsel = INPUTMUX_GpioPortPinToPintsel(0, 14U), /*!< Port 0 Pin 14 to PINT select */
    kINPUTMUX_GpioPort0Pin15ToPintsel = INPUTMUX_GpioPortPinToPintsel(0, 15U), /*!< Port 0 Pin 15 to PINT select */
    kINPUTMUX_GpioPort0Pin16ToPintsel = INPUTMUX_GpioPortPinToPintsel(0, 16U), /*!< Port 0 Pin 16 to PINT select */
    kINPUTMUX_GpioPort0Pin17ToPintsel = INPUTMUX_GpioPortPinToPintsel(0, 17U), /*!< Port 0 Pin 17 to PINT select */
    kINPUTMUX_GpioPort0Pin18ToPintsel = INPUTMUX_GpioPortPinToPintsel(0, 18U), /*!< Port 0 Pin 18 to PINT select */
    kINPUTMUX_GpioPort0Pin19ToPintsel = INPUTMUX_GpioPortPinToPintsel(0, 19U), /*!< Port 0 Pin 19 to PINT select */
    kINPUTMUX_GpioPort0Pin20ToPintsel = INPUTMUX_GpioPortPinToPintsel(0, 20U), /*!< Port 0 Pin 20 to PINT select */
    kINPUTMUX_GpioPort0Pin21ToPintsel = INPUTMUX_GpioPortPinToPintsel(0, 21U), /*!< Port 0 Pin 21 to PINT select */
    /*!< DMA ITRIG. */
    kINPUTMUX_Adc0SeqaIrqToDma = 0U + (DMA_ITRIG_PMUX_ID << PMUX_SHIFT),  /*!< ADC Interrupt (Sequence A) */
    kINPUTMUX_Adc0SeqbIrqToDma = 1U + (DMA_ITRIG_PMUX_ID << PMUX_SHIFT),  /*!< ADC Interrupt (Sequence B) */
    kINPUTMUX_Ctimer0M0ToDma   = 2U + (DMA_ITRIG_PMUX_ID << PMUX_SHIFT),  /*!< Timer CT32B0 Match 0 DMA request */
    kINPUTMUX_Ctimer0M1ToDma   = 3U + (DMA_ITRIG_PMUX_ID << PMUX_SHIFT),  /*!< Timer CT32B0 Match 1 DMA request */
    kINPUTMUX_Ctimer1M0ToDma   = 4U + (DMA_ITRIG_PMUX_ID << PMUX_SHIFT),  /*!< Timer CT32B1 Match 0 DMA request */
    kINPUTMUX_Ctimer1M1ToDma   = 5U + (DMA_ITRIG_PMUX_ID << PMUX_SHIFT),  /*!< Timer CT32B1 Match 1 DMA request */
    kINPUTMUX_PinInt0ToDma     = 6U + (DMA_ITRIG_PMUX_ID << PMUX_SHIFT),  /*!< Pin interrupt 0 */
    kINPUTMUX_PinInt1ToDma     = 7U + (DMA_ITRIG_PMUX_ID << PMUX_SHIFT),  /*!< Pin interrupt 1 */
    kINPUTMUX_PinInt2ToDma     = 8U + (DMA_ITRIG_PMUX_ID << PMUX_SHIFT),  /*!< Pin interrupt 2 */
    kINPUTMUX_PinInt3ToDma     = 9U + (DMA_ITRIG_PMUX_ID << PMUX_SHIFT),  /*!< Pin interrupt 3 */
    kINPUTMUX_AesRxToDma       = 10U + (DMA_ITRIG_PMUX_ID << PMUX_SHIFT), /*!< AES RX */
    kINPUTMUX_AesTxToDma       = 11U + (DMA_ITRIG_PMUX_ID << PMUX_SHIFT), /*!< AES TX */
    kINPUTMUX_HashRxToDma      = 12U + (DMA_ITRIG_PMUX_ID << PMUX_SHIFT), /*!< Hash RX */
    kINPUTMUX_HashTxToDma      = 13U + (DMA_ITRIG_PMUX_ID << PMUX_SHIFT), /*!< Hash TX */
    kINPUTMUX_Otrig0ToDma      = 14U + (DMA_ITRIG_PMUX_ID << PMUX_SHIFT), /*!< DMA output trigger 0 */
    kINPUTMUX_Otrig1ToDma      = 15U + (DMA_ITRIG_PMUX_ID << PMUX_SHIFT), /*!< DMA output trigger 1 */
    kINPUTMUX_Otrig2ToDma      = 16U + (DMA_ITRIG_PMUX_ID << PMUX_SHIFT), /*!< DMA output trigger 2 */
    kINPUTMUX_Otrig3ToDma      = 17U + (DMA_ITRIG_PMUX_ID << PMUX_SHIFT), /*!< DMA output trigger 3 */
    /*!< DMA OTRIG. */
    kINPUTMUX_DmaUsart0RxTrigoutToTriginChannels   = 0U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),  /*!< USART 0 RX */
    kINPUTMUX_DmaUsart0TxTrigoutToTriginChannels   = 1U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),  /*!< USART 0 TX */
    kINPUTMUX_DmaUsart1RxTrigoutToTriginChannels   = 2U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),  /*!< USART 1 RX  */
    kINPUTMUX_DmaUsart1TxTrigoutToTriginChannels   = 3U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),  /*!< USART 1 TX */
    kINPUTMUX_DmaI2c0SlvaeTrigoutToTriginChannels  = 4U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),  /*!< I2C 0 Slave */
    kINPUTMUX_DmaI2c0MasterTrigoutToTriginChannels = 5U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),  /*!< I2C 0 Master */
    kINPUTMUX_DmaI2c1SlvaeTrigoutToTriginChannels  = 6U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),  /*!< I2C 1 Slave */
    kINPUTMUX_DmaI2c1MasterTrigoutToTriginChannels = 7U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),  /*!< I2C 1 Master */
    kINPUTMUX_DmaSpi0RxTrigoutToTriginChannels     = 8U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),  /*!< SPI 0 RX  */
    kINPUTMUX_DmaSpi0TxTrigoutToTriginChannels     = 9U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),  /*!< SPI 0 TX */
    kINPUTMUX_DmaSpi1RxTrigoutToTriginChannels     = 10U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT), /*!< SPI 1 RX */
    kINPUTMUX_DmaSpi1TxTrigoutToTriginChannels     = 11U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT), /*!< SPI 1 TX */
    kINPUTMUX_DmaSpifi0TrigoutToTriginChannels     = 12U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT), /*!< SPIFI */
    kINPUTMUX_DmaI2c2SlaveTrigoutToTriginChannels  = 13U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT), /*!< I2C 2 Slave  */
    kINPUTMUX_DmaI2c2MasterTrigoutToTriginChannels = 14U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT), /*!< I2C 2 Master */
    kINPUTMUX_DmaDmic0Ch0TrigoutToTriginChannels   = 15U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT), /*!< DMIC Channel 0 */
    kINPUTMUX_DmaDmic0Ch1TrigoutToTriginChannels   = 16U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT), /*!< DMIC Channel 1 */
    kINPUTMUX_DmaHash0RxTrigoutToTriginChannels    = 17U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT), /*!< Hash RX */
    kINPUTMUX_DmaHash0TxTrigoutToTriginChannels    = 18U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT), /*!< Hash TX */
} inputmux_connection_t;

/*@}*/

#endif /* _FSL_INPUTMUX_CONNECTIONS_ */
