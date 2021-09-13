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
#define RC5_SINGLE_WIDTH_PULSE_36KHZ 32                          /* 32.04 */
#define RC5_DOUBLE_WIDTH_PULSE_36KHZ 64                          /* 64.07 */
#define RC5_SINGLE_WIDTH_PULSE_38KHZ 34                          /* 33.79 */
#define RC5_DOUBLE_WIDTH_PULSE_38KHZ 68                          /* 67.57 */
#define RC5_SINGLE_WIDTH_PULSE_40KHZ 36                          /* 35.56 */
#define RC5_DOUBLE_WIDTH_PULSE_40KHZ 71                          /* 71.12 */
#define RC5_FRAME_SIZE_BITS 14                                   /* From RC-5 protocol specification */
#define RC5_FRAME_SIZE_ENVELOPES ((RC5_FRAME_SIZE_BITS * 2) - 1) /* From RC-5 protocol specification */
#define RC5_ADDRESS_MASK 0x1F                                    /* From RC-5 protocol specification */
#define RC5_COMMAND_MASK 0x3f                                    /* From RC-5 protocol specification */

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
 * NAME: CIC_IRB_RC5Initialise
 *
 * DESCRIPTION: Initialise the peripheral ready to send RC5 messages.
 *
 * PARAMETERS: base CIC_IRB peripheral base address.
 *
 * RETURNS: None
 *
 * NOTES:
 *
 ****************************************************************************/
void CIC_IRB_RC5Initialise(CIC_IRB_Type *base)
{
    uint16_t ctuValue                     = 0;
    cic_irb_instance_data_t *instanceData = NULL;

    /* Get instance data for the current instance */
    instanceData = CIC_IRB_GetInstanceData(base);

    /* ensure that the peripheral is disabled before setting up carrier */
    base->CMD |= CIC_IRB_CMD_DIS(1);

    /* Set the entry level of the waveform, i.e. first envelope */
    instanceData->startLevel = false;

    /* Set the envelope widths */
    switch (instanceData->carrierFrequency)
    {
        case kCIC_IRB_CarrierFrequency_36kHz:
            instanceData->RCxSinglePulseEnvelopeWidth = RC5_SINGLE_WIDTH_PULSE_36KHZ;
            instanceData->RCxDoublePulseEnvelopeWidth = RC5_DOUBLE_WIDTH_PULSE_36KHZ;
            ctuValue                                  = CTU_36KHZ;
            break;

        case kCIC_IRB_CarrierFrequency_38kHz:
            instanceData->RCxSinglePulseEnvelopeWidth = RC5_SINGLE_WIDTH_PULSE_38KHZ;
            instanceData->RCxDoublePulseEnvelopeWidth = RC5_DOUBLE_WIDTH_PULSE_38KHZ;
            ctuValue                                  = CTU_38KHZ;
            break;

        case kCIC_IRB_CarrierFrequency_40kHz:
            instanceData->RCxSinglePulseEnvelopeWidth = RC5_SINGLE_WIDTH_PULSE_40KHZ;
            instanceData->RCxDoublePulseEnvelopeWidth = RC5_DOUBLE_WIDTH_PULSE_40KHZ;
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
 * NAME: CIC_IRB_SendRC5Packet
 *
 * DESCRIPTION: Send a RC-5 packet via the IRB peripheral. The peripheral
 *              instance must have been initialised and enabled and not busy.
 *
 * PARAMETERS: base CIC_IRB peripheral base address
 * 	           toggle the state of the toggle bit to encode
 * 	           address the 5 bit address to go into the message
 * 	           command the 7 bit command to go into the message
 *
 * RETURNS: A cic_irb_status_t status code
 *
 * NOTES:
 *
 ****************************************************************************/
cic_irb_status_t CIC_IRB_SendRC5Packet(CIC_IRB_Type *base, bool toggle, uint8_t address, uint8_t command)
{
    uint32_t packetBitfield               = 0;
    uint8_t fieldBit                      = 0;
    uint8_t previousEnvelopeLevel         = 1;
    int8_t envelopeCount                  = -1;
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

    /* Bale out if RC5 protocol not selected */
    if (status == kCIC_IRB_Status_Ok && instanceData->selectedProtocol != kCIC_IRB_ProtocolRC5)
    {
        status = kCIC_IRB_Status_IllegalProtocol;
    }

    /* Continue if no error so far */
    if (status == kCIC_IRB_Status_Ok)
    {
        /* The field bit depends on the value of the command in RC5.
         * If the command > 63 then field bit is 0, else field bit is 1 */
        if (command > RC5_COMMAND_MASK)
        {
            /* If the command is > 63 then only the lower 6 bits are put into the command field */
            command &= RC5_COMMAND_MASK;
            fieldBit = 0;
        }
        else
        {
            fieldBit = 1;
        }

        memset((void *)instanceData->envelopes, 0, sizeof(uint16_t) * ENVELOPE_ARRAY_SIZE);

        /* Build the frame representing the message */

        /* Start bit is always 1 */
        packetBitfield = 1; /* Start bit is always 1 */

        /* Add in the Field bit */
        packetBitfield <<= 1;
        packetBitfield |= fieldBit;

        /* Add toggle bit */
        packetBitfield <<= 1;
        packetBitfield |= toggle;

        /* Add 5 bits of address */
        packetBitfield <<= 5;
        packetBitfield |= address & RC5_ADDRESS_MASK;

        /* Add 6 bits of Command */
        packetBitfield <<= 6;
        packetBitfield |= command;

        /* Convert bitfield to envelopes */
        CIC_IRB_RCxAppendEnvelopes(packetBitfield, RC5_FRAME_SIZE_BITS, instanceData, &previousEnvelopeLevel,
                                   &envelopeCount, false);

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
