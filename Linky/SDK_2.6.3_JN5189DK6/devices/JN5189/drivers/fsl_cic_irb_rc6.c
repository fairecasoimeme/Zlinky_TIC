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

/**********************************************************************/
/*** Macro Definitions ***/
/**********************************************************************/
#define RC6_SINGLE_WIDTH_PULSE_36KHZ 16                          /* 16.13 */
#define RC6_DOUBLE_WIDTH_PULSE_36KHZ 32                          /* 32.26 */
#define RC6_TRIPLE_WIDTH_PULSE_36KHZ 48                          /* 48.39 */
#define RC6_HEX_WIDTH_PULSE_36KHZ 97                             /* 96.78 */
#define RC6_SINGLE_WIDTH_PULSE_38KHZ 17                          /* 16.89 */
#define RC6_DOUBLE_WIDTH_PULSE_38KHZ 34                          /* 33.78 */
#define RC6_TRIPLE_WIDTH_PULSE_38KHZ 51                          /* 50.67 */
#define RC6_HEX_WIDTH_PULSE_38KHZ 101                            /* 101.34 */
#define RC6_SINGLE_WIDTH_PULSE_40KHZ 18                          /* 17.78 */
#define RC6_DOUBLE_WIDTH_PULSE_40KHZ 36                          /* 35.56 */
#define RC6_TRIPLE_WIDTH_PULSE_40KHZ 53                          /* 53.34 */
#define RC6_HEX_WIDTH_PULSE_40KHZ 107                            /* 106.68 */
#define RC6_FRAME_SIZE_BITS 22                                   /* From RC-6 protocol specification */
#define RC6_FRAME_SIZE_ENVELOPES ((RC6_FRAME_SIZE_BITS * 2) - 1) /* From RC-6 protocol specification */
#define RC6_COMMAND_MASK 0xff                                    /* From RC-6 protocol specification */
#define RC6_SYSTEM_MASK 0xfF                                     /* From RC-6 protocol specification */

/**********************************************************************/
/*** Type Definitions ***/
/**********************************************************************/

/**********************************************************************/
/*** Local Function Prototypes ***/
/**********************************************************************/

/**********************************************************************/
/*** Exported Variables ***/
/**********************************************************************/

/**********************************************************************/
/*** Local Variables ***/
/**********************************************************************/

/**********************************************************************/
/*** Exported Functions ***/
/**********************************************************************/

/****************************************************************************
 *
 * NAME: CIC_IRB_RC6Initialise
 *
 * DESCRIPTION: Initialise the peripheral ready to send RC6 messages.
 *
 * PARAMETERS: base CIC_IRB peripheral base address.
 *
 * RETURNS: None
 *
 * NOTES:
 *
 ****************************************************************************/
void CIC_IRB_RC6Initialise(CIC_IRB_Type *base)
{
    uint32_t ctuValue                     = 0;
    cic_irb_instance_data_t *instanceData = NULL;

    /* Get instance data for the current instance */
    instanceData = CIC_IRB_GetInstanceData(base);

    /* ensure that the peripheral is disabled before setting up carrier */
    base->CMD |= CIC_IRB_CMD_DIS(1);

    /* Set the entry level of the waveform, i.e. first envelope */
    instanceData->startLevel = true;

    /* Set the envelope widths */
    switch (instanceData->carrierFrequency)
    {
        case kCIC_IRB_CarrierFrequency_36kHz:
            instanceData->RCxSinglePulseEnvelopeWidth = RC6_SINGLE_WIDTH_PULSE_36KHZ;
            instanceData->RCxDoublePulseEnvelopeWidth = RC6_DOUBLE_WIDTH_PULSE_36KHZ;
            instanceData->RC6TriplePulseEnvelopeWidth = RC6_TRIPLE_WIDTH_PULSE_36KHZ;
            instanceData->RC6HexPulseEnvelopeWidth    = RC6_HEX_WIDTH_PULSE_36KHZ;
            ctuValue                                  = CTU_36KHZ;
            break;

        case kCIC_IRB_CarrierFrequency_38kHz:
            instanceData->RCxSinglePulseEnvelopeWidth = RC6_SINGLE_WIDTH_PULSE_38KHZ;
            instanceData->RCxDoublePulseEnvelopeWidth = RC6_DOUBLE_WIDTH_PULSE_38KHZ;
            instanceData->RC6TriplePulseEnvelopeWidth = RC6_TRIPLE_WIDTH_PULSE_38KHZ;
            instanceData->RC6HexPulseEnvelopeWidth    = RC6_HEX_WIDTH_PULSE_38KHZ;
            ctuValue                                  = CTU_38KHZ;
            break;

        case kCIC_IRB_CarrierFrequency_40kHz:
            instanceData->RCxSinglePulseEnvelopeWidth = RC6_SINGLE_WIDTH_PULSE_40KHZ;
            instanceData->RCxDoublePulseEnvelopeWidth = RC6_DOUBLE_WIDTH_PULSE_40KHZ;
            instanceData->RC6TriplePulseEnvelopeWidth = RC6_TRIPLE_WIDTH_PULSE_40KHZ;
            instanceData->RC6HexPulseEnvelopeWidth    = RC6_HEX_WIDTH_PULSE_40KHZ;
            ctuValue                                  = CTU_40KHZ;
            break;

        default:
            break;
    }

    /* Set the CTU period and the mark space ratio. */
    base->CARRIER = 0;
    base->CARRIER |= CIC_IRB_CARRIER_CTU(ctuValue);
    base->CARRIER |= CIC_IRB_CARRIER_CLOW(DUTY_CYCLE_33_PERCENT_CARRIER_LOW);
    base->CARRIER |= CIC_IRB_CARRIER_CHIGH(DUTY_CYCLE_33_PERCENT_CARRIER_HIGH);
    base->CONF |= CIC_IRB_CONF_CAR_INI(1);

    /* Enable the peripheral */
    base->CMD |= CIC_IRB_CMD_ENA(1);

    /* Enable the Envelope start and last interrupts */
    base->INT_ENA |= CIC_IRB_INT_ENA_ENV_START_ENA(1);
    base->INT_ENA |= CIC_IRB_INT_ENA_ENV_LAST_ENA(1);
}

/****************************************************************************
 *
 * NAME: CIC_IRB_SendRC6Packet
 *
 * DESCRIPTION: Send a RC-5 packet via the IRB peripheral. The peripheral
 *              instance must have been initialised and enabled and not busy.
 *
 * PARAMETERS: base CIC_IRB peripheral base address
 * 	           toggle the state of the toggle bit to encode
 * 	           field the 3 bit field to go into the message
 * 	           address the 8 bit address to go into the message
 * 	           command the 8 bit command to go into the message
 *
 * RETURNS: A cic_irb_status_t status code
 *
 * NOTES:
 *
 ****************************************************************************/
cic_irb_status_t CIC_IRB_SendRC6Packet(CIC_IRB_Type *base, bool toggle, uint8_t field, uint8_t address, uint8_t command)
{
    int8_t envelopeCount                  = 0;
    uint8_t nextBit                       = 0;
    uint8_t previousEnvelopeLevel         = 0;
    cic_irb_instance_data_t *instanceData = NULL;
    cic_irb_status_t status               = kCIC_IRB_Status_Ok;

    /* Get and check instance data for the current instance */
    instanceData = CIC_IRB_GetInstanceData(base);
    if (instanceData == NULL)
    {
        status = kCIC_IRB_Status_IllegalValue;
    }

    /* Bale out if already sending a message */
    if (status == kCIC_IRB_Status_Ok && instanceData->busy == true)
    {
        status = kCIC_IRB_Status_Busy;
    }

    /* Bale out if not enabled */
    if (status == kCIC_IRB_Status_Ok && instanceData->enabled == false)
    {
        status = kCIC_IRB_Status_NotEnabled;
    }

    /* Bale out if RC6 protocol not selected */
    if (status == kCIC_IRB_Status_Ok && instanceData->selectedProtocol != kCIC_IRB_ProtocolRC6)
    {
        status = kCIC_IRB_Status_IllegalProtocol;
    }

    /* Continue if no error so far */
    if (status == kCIC_IRB_Status_Ok)
    {
        memset((void *)instanceData->envelopes, 0, sizeof(uint16_t) * ENVELOPE_ARRAY_SIZE);

        /* Build the frame representing the message */

        /* First start bit */
        instanceData->envelopes[envelopeCount] = instanceData->RC6HexPulseEnvelopeWidth;

        envelopeCount++;
        instanceData->envelopes[envelopeCount] = instanceData->RCxDoublePulseEnvelopeWidth;

        /* Prepend second start bit to field */
        field |= 0x8;

        /* Add envelopes for second start bit and field */
        CIC_IRB_RCxAppendEnvelopes(field, 4, instanceData, &previousEnvelopeLevel, &envelopeCount, true);

        /* Add envelopes for toggle bit. The width of these are variable. */
        if (previousEnvelopeLevel == 1)
        {
            /* Previous envelope high */
            if (toggle)
            {
                instanceData->envelopes[envelopeCount] = instanceData->RC6TriplePulseEnvelopeWidth;

                envelopeCount++;
                instanceData->envelopes[envelopeCount] = instanceData->RCxDoublePulseEnvelopeWidth;

                previousEnvelopeLevel = 0;
            }
            else
            {
                envelopeCount++;
                instanceData->envelopes[envelopeCount] = instanceData->RCxDoublePulseEnvelopeWidth;

                envelopeCount++;
                instanceData->envelopes[envelopeCount] = instanceData->RCxDoublePulseEnvelopeWidth;
            }
        }
        else
        {
            /* Previous envelope low */
            if (toggle)
            {
                envelopeCount++;
                instanceData->envelopes[envelopeCount] = instanceData->RCxDoublePulseEnvelopeWidth;

                envelopeCount++;
                instanceData->envelopes[envelopeCount] = instanceData->RCxDoublePulseEnvelopeWidth;
            }
            else
            {
                instanceData->envelopes[envelopeCount] = instanceData->RC6TriplePulseEnvelopeWidth;

                envelopeCount++;
                instanceData->envelopes[envelopeCount] = instanceData->RCxDoublePulseEnvelopeWidth;

                previousEnvelopeLevel = 0;
            }
        }

        /* Add envelope for first bit of address. The width of these are variable depending on toggle bit. */
        nextBit = ((address & 0x80) > 0 ? 1 : 0);
        if (previousEnvelopeLevel == 1)
        {
            /* Previous envelope high */
            if (nextBit == 1)
            {
                instanceData->envelopes[envelopeCount] = instanceData->RC6TriplePulseEnvelopeWidth;

                envelopeCount++;
                instanceData->envelopes[envelopeCount] = instanceData->RCxSinglePulseEnvelopeWidth;

                previousEnvelopeLevel = 0;
            }
            else
            {
                envelopeCount++;
                instanceData->envelopes[envelopeCount] = instanceData->RCxSinglePulseEnvelopeWidth;

                envelopeCount++;
                instanceData->envelopes[envelopeCount] = instanceData->RCxSinglePulseEnvelopeWidth;
            }
        }
        else
        {
            /* Previous envelope low */
            if (nextBit == 1)
            {
                envelopeCount++;
                instanceData->envelopes[envelopeCount] = instanceData->RCxSinglePulseEnvelopeWidth;

                envelopeCount++;
                instanceData->envelopes[envelopeCount] = instanceData->RCxSinglePulseEnvelopeWidth;
            }
            else
            {
                instanceData->envelopes[envelopeCount] = instanceData->RC6TriplePulseEnvelopeWidth;

                envelopeCount++;
                instanceData->envelopes[envelopeCount] = instanceData->RCxSinglePulseEnvelopeWidth;

                previousEnvelopeLevel = 1;
            }
        }

        /* Add envelopes for remainder of address */
        CIC_IRB_RCxAppendEnvelopes(address, 7, instanceData, &previousEnvelopeLevel, &envelopeCount, true);

        /* Add envelopes for command */
        CIC_IRB_RCxAppendEnvelopes(command, 8, instanceData, &previousEnvelopeLevel, &envelopeCount, true);

        envelopeCount++;
        instanceData->envelopeCount = envelopeCount;

        /* Send first FIFO load of entries to the FIFO. Remainder if more than one FIFO load are done in the interrupt
         * handler. */
        CIC_IRB_LoadAndSendFifo(base);

        /* Set instance busy flag */
        instanceData->busy = true;
    }

    return status;
}

/**********************************************************************/
/*** END OF FILE ***/
/**********************************************************************/
