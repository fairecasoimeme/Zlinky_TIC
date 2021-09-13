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
#define SIRC_COMMAND_MASK 0x7f             /* From SIRC protocol specification */
#define SIRC_12_20BIT_ADDRESS_MASK 0x3f    /* From SIRC protocol specification */
#define SIRC_15BIT_ADDRESS_MASK 0xff       /* From SIRC protocol specification */
#define SIRC_20BIT_EXTENDED_BITS_MASK 0xff /* From SIRC protocol specification */
#define SIRC_START_BIT_PULSE_WIDTH 96      /* 96.00 */
#define SIRC_LONG_PULSE_WIDTH 48           /* 48.00 */
#define SIRC_SHORT_PULSE_WIDTH 24          /* 24.00 */
#define SIRC_COMMAND_LENGTH 7              /* From SIRC protocol specification */
#define SIRC_12_20BIT_ADDRESS_LENGTH 5     /* From SIRC protocol specification */
#define SIRC_15BIT_ADDRESS_LENGTH 8        /* From SIRC protocol specification */
#define SIRC_EXTENDED_BITS_LENGTH 8        /* From SIRC protocol specification */

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
 * NAME: CIC_IRB_SIRCInitialise
 *
 * DESCRIPTION: Initialise the peripheral ready to send SIRC messages.
 *
 * PARAMETERS: base CIC_IRB peripheral base address.
 *
 * RETURNS: None
 *
 * NOTES:
 *
 ****************************************************************************/
void CIC_IRB_SIRCInitialise(CIC_IRB_Type *base)
{
    uint32_t ctuValue                     = 0;
    cic_irb_instance_data_t *instanceData = NULL;

    /* Get instance data for the current instance */
    instanceData = CIC_IRB_GetInstanceData(base);

    /* ensure that the peripheral is disabled before setting up carrier */
    base->CMD |= CIC_IRB_CMD_DIS(1);

    /* Set the entry level of the waveform, i.e. first envelope */
    instanceData->startLevel = true;

    /* Set the CTU widths */
    ctuValue = CTU_40KHZ;

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
 * NAME: CIC_IRB_SendSIRCPacket
 *
 * DESCRIPTION: Send a SIRC packet via the IRB peripheral. The peripheral
 *              instance must have been initialised and enabled and not busy.
 *
 * PARAMETERS: base CIC_IRB peripheral base address
 * 	           version the version of the protocol to use, varies the packet length
 * 	           command the 7 bit command to go into the message
 * 	           address the 5 or 8 bit address to go into the message
 *			   extendedBits 8 bits of extra data in 20 bit message
 *
 * RETURNS: A cic_irb_status_t status code
 *
 * NOTES:
 *
 ****************************************************************************/
cic_irb_status_t CIC_IRB_SendSIRCPacket(
    CIC_IRB_Type *base, cic_irb_sirc_version_t version, uint8_t command, uint8_t address, uint8_t extendedBits)
{
    int8_t envelopeCount                  = 0;
    uint8_t i                             = 0;
    uint8_t bitMask                       = 0;
    uint8_t addressLengthBits             = 0;
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

    /* Bale out if SIRC protocol not selected */
    if (status == kCIC_IRB_Status_Ok && instanceData->selectedProtocol != kCIC_IRB_ProtocolSIRC)
    {
        status = kCIC_IRB_Status_IllegalProtocol;
    }

    /* Continue if no error so far */
    if (status == kCIC_IRB_Status_Ok)
    {
        memset((void *)instanceData->envelopes, 0, sizeof(uint16_t) * ENVELOPE_ARRAY_SIZE);

        /* Build the frame representing the message */

        /* Start bit */
        instanceData->envelopes[envelopeCount] = SIRC_START_BIT_PULSE_WIDTH;
        envelopeCount++;
        instanceData->envelopes[envelopeCount] = SIRC_SHORT_PULSE_WIDTH;
        envelopeCount++;

        /* Command, always 7 bits */
        command &= SIRC_COMMAND_MASK;
        bitMask = 0x1;
        for (i = 0; i < SIRC_COMMAND_LENGTH; i++)
        {
            /* Add high pulse representing data bit */
            if (command & bitMask)
            {
                instanceData->envelopes[envelopeCount] = SIRC_LONG_PULSE_WIDTH;
                envelopeCount++;
            }
            else
            {
                instanceData->envelopes[envelopeCount] = SIRC_SHORT_PULSE_WIDTH;
                envelopeCount++;
            }

            /* Add subsequent separating low fixed width pulse */
            instanceData->envelopes[envelopeCount] = SIRC_SHORT_PULSE_WIDTH;
            envelopeCount++;
            bitMask <<= 1;
        }

        /* Address, various lengths */
        bitMask = 0x1;
        if (version == kCIC_IRB_SircVersion_15Bit)
        {
            addressLengthBits = SIRC_15BIT_ADDRESS_LENGTH;
            address &= SIRC_15BIT_ADDRESS_MASK;
        }
        else
        {
            addressLengthBits = SIRC_12_20BIT_ADDRESS_LENGTH;
            address &= SIRC_12_20BIT_ADDRESS_MASK;
        }
        for (i = 0; i < addressLengthBits; i++)
        {
            /* Add high pulse representing data bit */
            if (address & bitMask)
            {
                instanceData->envelopes[envelopeCount] = SIRC_LONG_PULSE_WIDTH;
                envelopeCount++;
            }
            else
            {
                instanceData->envelopes[envelopeCount] = SIRC_SHORT_PULSE_WIDTH;
                envelopeCount++;
            }

            /* Add subsequent separating low fixed width pulse */
            instanceData->envelopes[envelopeCount] = SIRC_SHORT_PULSE_WIDTH;
            envelopeCount++;
            bitMask <<= 1;
        }

        /* Extended bits, 8 bits, 20 bit message only */
        if (version == kCIC_IRB_SircVersion_20Bit)
        {
            extendedBits &= SIRC_20BIT_EXTENDED_BITS_MASK;
            bitMask = 0x1;
            for (i = 0; i < SIRC_EXTENDED_BITS_LENGTH; i++)
            {
                /* Add high pulse representing data bit */
                if (extendedBits & bitMask)
                {
                    instanceData->envelopes[envelopeCount] = SIRC_LONG_PULSE_WIDTH;
                    envelopeCount++;
                }
                else
                {
                    instanceData->envelopes[envelopeCount] = SIRC_SHORT_PULSE_WIDTH;
                    envelopeCount++;
                }

                /* Add subsequent separating low fixed width pulse */
                instanceData->envelopes[envelopeCount] = SIRC_SHORT_PULSE_WIDTH;
                envelopeCount++;
                bitMask <<= 1;
            }
        }

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
