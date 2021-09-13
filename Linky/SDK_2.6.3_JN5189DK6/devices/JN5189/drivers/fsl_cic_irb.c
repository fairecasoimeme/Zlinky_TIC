/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**********************************************************************/
/*** Include files ***/
/**********************************************************************/

#include "fsl_cic_irb.h"
#include "fsl_cic_irb_private.h"
/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.cic_irb"
#endif

/**********************************************************************/
/*** Macro Definitions ***/
/**********************************************************************/

/**********************************************************************/
/*** Type Definitions ***/
/**********************************************************************/

/**********************************************************************/
/*** Local Function Prototypes ***/
/**********************************************************************/
static void CIC_IRB_GenericIRQHandler(uint8_t instance);

/**********************************************************************/
/*** Exported Variables ***/
/**********************************************************************/

/**********************************************************************/
/*** Local Variables ***/
/**********************************************************************/
/* Array of CIC IRB peripheral base address. */
static CIC_IRB_Type *const s_cic_irb_Bases[FSL_FEATURE_SOC_CIC_IRB_COUNT] = CIC_IRB_BASE_PTRS;

/* Array of CIC IRB IRQ number. */
static const IRQn_Type s_cic_irb_IRQ[FSL_FEATURE_SOC_CIC_IRB_COUNT] = CIC_IRB_IRQS;

/* Array of structures containing data for each CIC IRB instance */
static cic_irb_instance_data_t instanceData[FSL_FEATURE_SOC_CIC_IRB_COUNT];

/**********************************************************************/
/*** Exported Functions ***/
/**********************************************************************/

/****************************************************************************
 *
 * NAME: CIC_IRB_GetDefaultConfig
 *
 * DESCRIPTION:
 *
 * PARAMETERS: config pointer to a configuration structure
 *
 * RETURNS: A cic_irb_status_t status code*
 * NOTES:
 *
 ****************************************************************************/
cic_irb_status_t CIC_IRB_GetDefaultConfig(cic_irb_config_t *config)
{
    cic_irb_status_t status = kCIC_IRB_Status_IllegalValue;

    if (config != NULL)
    {
        config->selectedProtocol         = kCIC_IRB_ProtocolRC5;
        config->selectedCarrierFrequency = kCIC_IRB_CarrierFrequency_36kHz;
        status                           = kCIC_IRB_Status_Ok;
    }

    return status;
}

/****************************************************************************
 *
 * NAME: CIC_IRB_Init
 *
 * DESCRIPTION: Initialize the IRB peripheral. Attaches, configures and enables
 *              source and peripheral clocks, resets peripheral and initializes
 *              instance data. The peripheral is not enabled after this.
 *
 * PARAMETERS: base CIC_IRB peripheral base address
 * 			  config pointer to a configuration structure
 *
 * RETURNS: A cic_irb_status_t status code
 *
 * NOTES:
 *
 ****************************************************************************/
cic_irb_status_t CIC_IRB_Init(CIC_IRB_Type *base, cic_irb_config_t *config)
{
    uint8_t instance        = 0;
    cic_irb_status_t status = kCIC_IRB_Status_Ok;

    /* Get and check instance of IR peripheral from base address */
    instance = CIC_IRB_GetInstance(base);
    if (config == NULL || instance >= FSL_FEATURE_SOC_CIC_IRB_COUNT)
    {
        status = kCIC_IRB_Status_IllegalValue;
    }

    /* Only continue if instance found */
    if (status == kCIC_IRB_Status_Ok)
    {
        /* Select 48MHz as the carrier clock. */
        CLOCK_AttachClk(kFRO48M_to_IR_CLK);

        /* Set clock divisor and disable the IR_divider halt. This defaults to on at reset */
        CLOCK_SetClkDiv(kCLOCK_DivIrClk, 1, false);

        /* Reset IR Blaster peripheral */
        RESET_PeripheralReset(kIRB_RST_SHIFT_RSTn);

        /* Enable the peripheral clock for the IR Blaster */
        CLOCK_EnableClock(kCLOCK_Ir);

        /* Check protocol and parameters */
        switch (config->selectedProtocol)
        {
            case kCIC_IRB_ProtocolRC5:
            case kCIC_IRB_ProtocolRC6:
                if (config->selectedCarrierFrequency == kCIC_IRB_CarrierFrequency_36kHz ||
                    config->selectedCarrierFrequency == kCIC_IRB_CarrierFrequency_38kHz ||
                    config->selectedCarrierFrequency == kCIC_IRB_CarrierFrequency_40kHz)
                {
                    instanceData[instance].carrierFrequency = config->selectedCarrierFrequency;
                }
                else
                {
                    status = kCIC_IRB_Status_IllegalValue;
                }
                break;

            case kCIC_IRB_ProtocolSIRC:
                if (config->selectedCarrierFrequency == kCIC_IRB_CarrierFrequency_40kHz)
                {
                    instanceData[instance].carrierFrequency = kCIC_IRB_CarrierFrequency_40kHz;
                }
                else
                {
                    status = kCIC_IRB_Status_IllegalValue;
                }
                break;

            case kCIC_IRB_ProtocolRCMM:
                if (config->selectedCarrierFrequency == kCIC_IRB_CarrierFrequency_36kHz)
                {
                    instanceData[instance].carrierFrequency = kCIC_IRB_CarrierFrequency_36kHz;
                }
                else
                {
                    status = kCIC_IRB_Status_IllegalValue;
                }
                break;

            default:
                status = kCIC_IRB_Status_IllegalProtocol;
        }
    }

    /* Initialise instance data if no error */
    if (status == kCIC_IRB_Status_Ok)
    {
        instanceData[instance].selectedProtocol = config->selectedProtocol;
        instanceData[instance].busy             = false;
        instanceData[instance].enabled          = false;
    }

    return status;
}

/****************************************************************************
 *
 * NAME: CIC_IRB_Enable
 *
 * DESCRIPTION: Enable the IRB peripheral doing protocol specific
 *              initializations. The interrupts are enabled after this call.
 *
 * PARAMETERS: base CIC_IRB peripheral base address
 *
 * RETURNS: A cic_irb_status_t status code
 *
 * NOTES:
 *
 ****************************************************************************/
cic_irb_status_t CIC_IRB_Enable(CIC_IRB_Type *base)
{
    uint8_t instance        = 0;
    cic_irb_status_t status = kCIC_IRB_Status_Ok;

    /* Get and check instance of IR peripheral from base address */
    instance = CIC_IRB_GetInstance(base);
    if (instance >= FSL_FEATURE_SOC_CIC_IRB_COUNT)
    {
        status = kCIC_IRB_Status_IllegalValue;
    }

    if (status == kCIC_IRB_Status_Ok && instanceData[instance].enabled == true)
    {
        /* Bale out if already enabled */
        status = kCIC_IRB_Status_AlreadyEnabled;
    }

    if (status == kCIC_IRB_Status_Ok)
    {
        switch (instanceData[instance].selectedProtocol)
        {
            case kCIC_IRB_ProtocolRC5:
                CIC_IRB_RC5Initialise(base);
                NVIC_EnableIRQ(s_cic_irb_IRQ[instance]);
                break;

            case kCIC_IRB_ProtocolRC6:
                CIC_IRB_RC6Initialise(base);
                NVIC_EnableIRQ(s_cic_irb_IRQ[instance]);
                break;

            case kCIC_IRB_ProtocolSIRC:
                CIC_IRB_SIRCInitialise(base);
                NVIC_EnableIRQ(s_cic_irb_IRQ[instance]);
                break;

            case kCIC_IRB_ProtocolRCMM:
                CIC_IRB_RCMMInitialise(base);
                NVIC_EnableIRQ(s_cic_irb_IRQ[instance]);
                break;

            default:
                status = kCIC_IRB_Status_IllegalProtocol;
                break;
        }

        /* Set state to enabled if no error */
        if (status == kCIC_IRB_Status_Ok)
        {
            instanceData[instance].enabled = true;
        }
    }

    return status;
}

/****************************************************************************
 *
 * NAME: CIC_IRB_IsBusy
 *
 * DESCRIPTION: Determine if the IRB peripheral instance is in the process
 *              of sending the previous message.
 *
 * PARAMETERS: base CIC_IRB peripheral base address
 *             isBusy pointer to bool for result
 *
 * RETURNS: A cic_irb_status_t status code
 *
 * NOTES:
 *
 ****************************************************************************/
cic_irb_status_t CIC_IRB_IsBusy(CIC_IRB_Type *base, bool *isBusy)
{
    uint8_t instance        = 0;
    cic_irb_status_t status = kCIC_IRB_Status_IllegalValue;

    /* Get and check instance of IR peripheral from base address */
    instance = CIC_IRB_GetInstance(base);
    if (instance < FSL_FEATURE_SOC_CIC_IRB_COUNT && isBusy != NULL)
    {
        /* Set isBusy value */
        *isBusy = instanceData[instance].busy;
        status  = kCIC_IRB_Status_Ok;
    }

    return status;
}

/****************************************************************************
 *
 * NAME: CIC_IRB_Disable
 *
 * DESCRIPTION: Disable the IRB peripheral such that it can be enabled again
 *              without doing another init. This stops interrupts and
 *              any part sent message is abandoned.
 *
 * PARAMETERS: base CIC_IRB peripheral base address
 *
 * RETURNS: A cic_irb_status_t status code
 *
 * NOTES:
 *
 ****************************************************************************/
cic_irb_status_t CIC_IRB_Disable(CIC_IRB_Type *base)
{
    uint8_t instance        = 0;
    cic_irb_status_t status = kCIC_IRB_Status_IllegalValue;

    /* Get and check instance of IR peripheral from base address */
    instance = CIC_IRB_GetInstance(base);
    if (instance < FSL_FEATURE_SOC_CIC_IRB_COUNT)
    {
        /* Disable further interrupts */
        NVIC_DisableIRQ(s_cic_irb_IRQ[instance]);

        /* Disable peripheral */
        base->CMD |= CIC_IRB_CMD_DIS(1);

        /* Reset FIFO */
        base->CMD |= CIC_IRB_CMD_FIFO_RST(1);

        /* Reset instance flags */
        instanceData[instance].enabled = false;
        instanceData[instance].busy    = false;

        status = kCIC_IRB_Status_Ok;
    }

    return status;
}

/****************************************************************************
 *
 * NAME: CIC_IRB_DeInit
 *
 * DESCRIPTION: De-initialises the IRB peripheral instance. The clock is
 *              stopped.
 *
 * PARAMETERS: base CIC_IRB peripheral base address
 *
 * RETURNS: A cic_irb_status_t status code
 *
 * NOTES:
 *
 ****************************************************************************/
cic_irb_status_t CIC_IRB_DeInit(CIC_IRB_Type *base)
{
    cic_irb_status_t status = kCIC_IRB_Status_IllegalValue;

    if (base != NULL)
    {
        CIC_IRB_Disable(base);
        CLOCK_DisableClock(kCLOCK_Ir);
        status = kCIC_IRB_Status_Ok;
    }

    return status;
}

/**********************************************************************/
/*** Private Functions ***/
/**********************************************************************/

/****************************************************************************
 *
 * NAME: CIC_IRB_GetInstance
 *
 * DESCRIPTION: Get the CIC_IRB instance from peripheral base address.
 *
 * PARAMETERS: base CIC_IRB peripheral base address.
 *
 * RETURNS: CIC_IRB instance number.
 *
 * NOTES: Returns FSL_FEATURE_SOC_CIC_IRB_COUNT if instance for base not found.
 *
 ****************************************************************************/
uint8_t CIC_IRB_GetInstance(CIC_IRB_Type *base)
{
    uint8_t instance = 0;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < FSL_FEATURE_SOC_CIC_IRB_COUNT; instance++)
    {
        if (s_cic_irb_Bases[instance] == base)
        {
            break;
        }
    }

    if (instance == FSL_FEATURE_SOC_CIC_IRB_COUNT)
    {
        instance = 0;
    }

    return instance;
}

/****************************************************************************
 *
 * NAME: CIC_IRB_GetInstanceData
 *
 * DESCRIPTION: Get a pointer to the data structure containing context
 *              information for a peripheral instance
 *
 * PARAMETERS: base CIC_IRB peripheral base address.
 *
 * RETURNS: Pointer to the instance data structure
 *
 * NOTES: Returns NULL if an instance not found for base
 *
 ****************************************************************************/
cic_irb_instance_data_t *CIC_IRB_GetInstanceData(CIC_IRB_Type *base)
{
    uint8_t instance = CIC_IRB_GetInstance(base);

    return (instance < FSL_FEATURE_SOC_CIC_IRB_COUNT ? &instanceData[instance] : NULL);
}

/****************************************************************************
 *
 * NAME: CIC_IRB_RCxAppendEnvelopes
 *
 * DESCRIPTION: Take a bit pattern that comprises part of a RCx message and
 *              encode this in envelopes using the appropriate type of
 *              Manchester encoding. The envelopes can be appended to
 *              an array of previously encoded envelopes or can be a complete
 *              message.
 *
 * PARAMETERS: bitPattern the binary data to encode into the message
 *             bitCount the number of bits in bitPattern
 *             instanceData pointer to the peripheral instance data structure
 *             previousEnvelopeLevel the level of the last envelope of the preceding part of the encoded message
 *             envelope pointer to array to add this bit pattern's encoded envelopes
 *             ManchesterEncodingIEEE802_3 type of Manchester encoding, true for IEEE 802.3, false for Thomas
 *
 * RETURNS: None
 *
 * NOTES:
 *
 ****************************************************************************/
void CIC_IRB_RCxAppendEnvelopes(uint32_t bitPattern,
                                uint8_t bitCount,
                                cic_irb_instance_data_t *instanceData,
                                uint8_t *previousEnvelopeLevel,
                                int8_t *envelope,
                                bool ManchesterEncodingIEEE802_3)
{
    uint8_t i       = 0;
    uint32_t mask   = 1 << (bitCount - 1);
    uint8_t nextBit = 0;

    while (i < bitCount)
    {
        /* Pull out next bit from msb to lsb */
        nextBit = ((bitPattern & mask) > 0 ? 1 : 0);

        /* Check for Manchester coding type */
        if (ManchesterEncodingIEEE802_3)
        {
            /* IEEE 802.3 Manchester encoding */
            if (*previousEnvelopeLevel == 1)
            {
                if (nextBit == 1)
                {
                    instanceData->envelopes[*envelope] = instanceData->RCxDoublePulseEnvelopeWidth;

                    (*envelope)++;
                    instanceData->envelopes[*envelope] = instanceData->RCxSinglePulseEnvelopeWidth;

                    *previousEnvelopeLevel = 0;
                }
                else
                {
                    (*envelope)++;
                    instanceData->envelopes[*envelope] = instanceData->RCxSinglePulseEnvelopeWidth;

                    (*envelope)++;
                    instanceData->envelopes[*envelope] = instanceData->RCxSinglePulseEnvelopeWidth;
                }
            }
            else
            {
                if (nextBit == 1)
                {
                    (*envelope)++;
                    instanceData->envelopes[*envelope] = instanceData->RCxSinglePulseEnvelopeWidth;

                    (*envelope)++;
                    instanceData->envelopes[*envelope] = instanceData->RCxSinglePulseEnvelopeWidth;
                }
                else
                {
                    instanceData->envelopes[*envelope] = instanceData->RCxDoublePulseEnvelopeWidth;

                    (*envelope)++;
                    instanceData->envelopes[*envelope] = instanceData->RCxSinglePulseEnvelopeWidth;

                    *previousEnvelopeLevel = 1;
                }
            }
        }
        else
        {
            /* Thomas Manchester encoding */
            if (*previousEnvelopeLevel == 1)
            {
                if (nextBit == 0)
                {
                    instanceData->envelopes[*envelope] = instanceData->RCxDoublePulseEnvelopeWidth;

                    (*envelope)++;
                    instanceData->envelopes[*envelope] = instanceData->RCxSinglePulseEnvelopeWidth;

                    *previousEnvelopeLevel = 0;
                }
                else
                {
                    (*envelope)++;
                    instanceData->envelopes[*envelope] = instanceData->RCxSinglePulseEnvelopeWidth;

                    (*envelope)++;
                    instanceData->envelopes[*envelope] = instanceData->RCxSinglePulseEnvelopeWidth;
                }
            }
            else
            {
                if (nextBit == 0)
                {
                    (*envelope)++;
                    instanceData->envelopes[*envelope] = instanceData->RCxSinglePulseEnvelopeWidth;

                    (*envelope)++;
                    instanceData->envelopes[*envelope] = instanceData->RCxSinglePulseEnvelopeWidth;
                }
                else
                {
                    instanceData->envelopes[*envelope] = instanceData->RCxDoublePulseEnvelopeWidth;

                    (*envelope)++;
                    instanceData->envelopes[*envelope] = instanceData->RCxSinglePulseEnvelopeWidth;

                    *previousEnvelopeLevel = 1;
                }
            }
        }

        i++;
        mask >>= 1;
    }
}

/****************************************************************************
 *
 * NAME: CIC_IRB_LoadAndSendFifo
 *
 * DESCRIPTION: Load the FIFO with the first part of a series of envelopes to
 *              send. If a message's complete set of envelopes fits within the
 *              FIFO size then the whole message will be sent. If not then the
 *              maximum number of envelopes is loaded into the FIFO that is
 *              possible. Subsequent FIFO loads for the remaining envelopes are
 *              loaded into the FIFO in the interrupt handler which is triggered
 *              when the FIFO becomes empty.
 *
 * PARAMETERS: base CIC_IRB peripheral base address.
 *
 * RETURNS: None
 *
 * NOTES:
 *
 ****************************************************************************/
void CIC_IRB_LoadAndSendFifo(CIC_IRB_Type *base)
{
    uint8_t i                     = 0;
    uint8_t instance              = 0;
    uint8_t numberEnvelopesToSend = 0;

    /* Get instance of IR peripheral from base address */
    instance = CIC_IRB_GetInstance(base);

    /* Configure the entry level of the waveform, i.e. first envelope */
    base->CONF &= ~CIC_IRB_CONF_ENV_INI_MASK;
    base->CONF |= CIC_IRB_CONF_ENV_INI(instanceData[instance].startLevel);

    /* Calculate how many envelopes to send in first FIFO load */
    if (instanceData[instance].envelopeCount > CIC_IRB_FIFO_SIZE)
    {
        /* More envelopes to send than FIFO size so send FIFO size envelopes */
        numberEnvelopesToSend = CIC_IRB_FIFO_SIZE;
        instanceData[instance].envelopes[numberEnvelopesToSend - 1] |= INV_ENV_INT_BIT;
        instanceData[instance].nextEnvelopeToSend = CIC_IRB_FIFO_SIZE;
    }
    else
    {
        /* All the envelopes will fit in a single FIFO load so send them all */
        numberEnvelopesToSend = instanceData[instance].envelopeCount;
        instanceData[instance].envelopes[numberEnvelopesToSend - 1] |= INV_ENV_LAST_BIT;
    }

    /* Load the FIFO with the calculated number of envelopes to send */
    for (i = 0; i < numberEnvelopesToSend; i++)
    {
        base->FIFO_IN = instanceData[instance].envelopes[i];
    }

    /* Start transmission, set IRB_START */
    base->CMD |= CIC_IRB_CMD_START_MASK;
}

/****************************************************************************
 *
 * NAME: CIC_IRB_DriverIRQHandler
 *
 * DESCRIPTION: The interrupt handler for IRB peripheral instance 0. This
 *              calls the generic interrupt handler where the work is done.
 *
 * PARAMETERS: None
 *
 * RETURNS: None
 *
 * NOTES: For hardware with more than 1 IRB peripheral implement further
 *        interrupt handlers and they can then call the generic handler.
 *
 ****************************************************************************/
#if (FSL_FEATURE_SOC_CIC_IRB_COUNT > 0U)
void CIC_IRB_DriverIRQHandler(void)
{
    CIC_IRB_GenericIRQHandler(0); /* 0 is the instance number */
}
#endif

/****************************************************************************
*
* NAME: CIC_IRB_GenericIRQHandler
*
* DESCRIPTION: The generic IRB peripheral interrupt handler that is called by
*              the peripheral specific interrupt handler.
*
*              Two interrupt types are handled:

*              1. ENVELOPE START is triggered when the last envelope of the
*              previous part of the message has been sent. The interrupt then
*              fills the FIFO with the next part of the message, setting the
*              'LAST' bit if required in the last envelope word which causes
*              the ENVELOPE LAST interrupt when the FIFO is emptied.
*              2. ENVELOPE LAST interrupt which occurs when the final envelope
*              bit is emptied from the FIFO. This resets the busy flag that has
*              been set when starting transmitting.
*
* PARAMETERS: None
*
* RETURNS: None
*
* NOTES:
* There are three inputs as global variables to the function:
*  envelopes    		The array carrying the envelope data to load into the FIFO
*  envelopeCount		The number of envelopes comprising this message
*  nextEnvelopeToSend   The next envelope to send if there are more to send
*
****************************************************************************/
static void CIC_IRB_GenericIRQHandler(uint8_t instance)
{
    uint8_t i                     = 0;
    uint8_t numberEnvelopesToSend = 0;
    CIC_IRB_Type *base            = s_cic_irb_Bases[instance];

    /* Check for an ENVELOPE START interrupt */
    if ((base->INT_STATUS & CIC_IRB_INT_STATUS_ENV_START_INT_MASK) == CIC_IRB_INT_STATUS_ENV_START_INT_MASK)
    {
        /* Calculate how many envelopes to send in the next FIFO load */
        if (instanceData[instance].envelopeCount - instanceData[instance].nextEnvelopeToSend > CIC_IRB_FIFO_SIZE)
        {
            /* More remaining envelopes to send than FIFO size so send FIFO size envelopes */
            numberEnvelopesToSend = CIC_IRB_FIFO_SIZE;
            instanceData[instance].envelopes[instanceData[instance].nextEnvelopeToSend + numberEnvelopesToSend - 1] |=
                INV_ENV_INT_BIT;
        }
        else
        {
            /* All the remaining envelopes will fit in another single FIFO load so send them all */
            numberEnvelopesToSend = instanceData[instance].envelopeCount - instanceData[instance].nextEnvelopeToSend;
            instanceData[instance].envelopes[instanceData[instance].nextEnvelopeToSend + numberEnvelopesToSend - 1] |=
                INV_ENV_LAST_BIT;
        }

        /* Load the FIFO with the calculated number of envelopes to send */
        for (i = instanceData[instance].nextEnvelopeToSend;
             i < instanceData[instance].nextEnvelopeToSend + numberEnvelopesToSend; i++)
        {
            base->FIFO_IN = instanceData[instance].envelopes[i];
        }

        /* Update position of next envelope to send in next FIFO load */
        instanceData[instance].nextEnvelopeToSend += numberEnvelopesToSend;

        /* Start transmission, set IRB_START */
        base->CMD |= CIC_IRB_CMD_START_MASK;

        /* Clear the interrupt bit */
        base->INT_CLR |= CIC_IRB_INT_CLR_ENV_START_CLR_MASK;
    }

    /* Check for the ENVELOPE LAST interrupt */
    if ((base->INT_STATUS & CIC_IRB_INT_STATUS_ENV_LAST_INT_MASK) == CIC_IRB_INT_STATUS_ENV_LAST_INT_MASK)
    {
        instanceData[instance].busy = false;

        /* Clear the interrupt bit */
        base->INT_CLR |= CIC_IRB_INT_CLR_ENV_LAST_CLR_MASK;
    }
}

/**********************************************************************/
/*** END OF FILE ***/
/**********************************************************************/
