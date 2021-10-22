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
#define RCMM_START_BIT_ON_WIDTH 15       /* 15.02 */
#define RCMM_START_BIT_OFF_WIDTH 10      /* 10.01 */
#define RCMM_BIT_ON_WIDTH 6              /* 6.01 */
#define RCMM_00_BITS_OFF_WIDTH 10        /* 10.01 */
#define RCMM_01_BITS_OFF_WIDTH 16        /* 16.01 */
#define RCMM_10_BITS_OFF_WIDTH 22        /* 22.02 */
#define RCMM_11_BITS_OFF_WIDTH 28        /* 28.03 */
#define RCMM_1MS_SIGNAL_FREE_TIME 36     /* 36.04 */
#define RCMM_3_36MS_SIGNAL_FREE_TIME 121 /* 121.08 */
#define RCMM_12_BITS_MODE_MASK 0x3       /* From RCMM protocol */
#define RCMM_ADDRESS_MASK 0x3            /* From RCMM protocol */
#define RCMM_12_BIT_DATA_MASK 0xff       /* From RCMM protocol */
#define RCMM_14_BITS_MODE_MASK 0xf       /* From RCMM protocol */
#define RCMM_24_BIT_DATA_MASK 0xfffff    /* From RCMM protocol */
#define RCMM_OEM_MODE_MASK 0x3f          /* From RCMM protocol */
#define RCMM_OEM_CUSTOMER_ID_MASK 0x3f   /* From RCMM protocol */
#define RCMM_OEM_DATA_MASK 0xfff         /* From RCMM protocol */

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
static const uint8_t pulsePositionCodingValue[4] = {RCMM_00_BITS_OFF_WIDTH, RCMM_01_BITS_OFF_WIDTH,
                                                    RCMM_10_BITS_OFF_WIDTH, RCMM_11_BITS_OFF_WIDTH};

/**********************************************************************/
/*** Exported Functions ***/
/**********************************************************************/

/****************************************************************************
 *
 * NAME: CIC_IRB_RCMMInitialise
 *
 * DESCRIPTION: Initialise the peripheral ready to send RCMM messages.
 *
 * PARAMETERS: base CIC_IRB peripheral base address.
 *
 * RETURNS: None
 *
 * NOTES:
 *
 ****************************************************************************/
void CIC_IRB_RCMMInitialise(CIC_IRB_Type *base)
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
    ctuValue = CTU_36KHZ;

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
 * NAME: CIC_IRB_SendRCMMPacket
 *
 * DESCRIPTION: Send a RCMM packet via the IRB peripheral. The peripheral
 *              instance must have been initialised and enabled and not busy.
 *
 * PARAMETERS: base CIC_IRB peripheral base address
 * 				mode The RC-MM message mode. See RC-MM documentation
 * 				modeBits The mode numerical value, varying number of bits
 * 				address RC-MM address data field in some modes
 * 				customerId RC-MM customer if in some modes
 * 				data the RC-MM data field, varying length depending on mode
 * 				signalFreeTime Silent period after packet transmission
 *
 * RETURNS: A cic_irb_status_t status code
 *
 * NOTES:
 *
 ****************************************************************************/
cic_irb_status_t CIC_IRB_SendRCMMPacket(CIC_IRB_Type *base,
                                        cic_irb_rcmm_mode_t mode,
                                        uint8_t modeBits,
                                        uint8_t address,
                                        uint8_t customerId,
                                        uint32_t data,
                                        cic_irb_rcmm_signal_free_time_t signalFreeTime)
{
    int8_t envelopeCount                  = 0;
    uint8_t i                             = 0;
    uint32_t bitMask                      = 0;
    uint8_t bitPair                       = 0;
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
    if (status == kCIC_IRB_Status_Ok && instanceData->selectedProtocol != kCIC_IRB_ProtocolRCMM)
    {
        status = kCIC_IRB_Status_IllegalProtocol;
    }

    /* Check mode and modeBits values */
    if (status == kCIC_IRB_Status_Ok)
    {
        switch (mode)
        {
            case kCIC_IRB_RcmmMode_12Bit:
            case kCIC_IRB_RcmmMode_24Bit:
                /* Mode 0b00 indicates 24 bit mode so cannot be used for 12 bit mode */
                /* Mode 0b0000 indicates oem bit mode so cannot be used for 24 bit mode */
                if (modeBits == 0)
                {
                    status = kCIC_IRB_Status_IllegalValue;
                }
                break;

            case kCIC_IRB_RcmmMode_Oem:
                /* Mode 0b000011 indicates oem bit mode so cannot be any other value */
                if (modeBits != 0x3)
                {
                    status = kCIC_IRB_Status_IllegalValue;
                }
                break;

            default:
                status = kCIC_IRB_Status_IllegalProtocol;
        }
    }

    /* Check signalFreeTime value */
    if (status == kCIC_IRB_Status_Ok)
    {
        if (signalFreeTime != kCIC_IRB_RcmmSignalFreeTime_1ms && signalFreeTime != kCIC_IRB_RcmmSignalFreeTime_3_36ms)
        {
            status = kCIC_IRB_Status_IllegalValue;
        }
    }

    /* Continue if no error so far */
    if (status == kCIC_IRB_Status_Ok)
    {
        memset((void *)instanceData->envelopes, 0, sizeof(uint16_t) * ENVELOPE_ARRAY_SIZE);
        instanceData->envelopeCount = envelopeCount;

        /* Start bit */
        instanceData->envelopes[envelopeCount] = RCMM_START_BIT_ON_WIDTH;
        envelopeCount++;
        instanceData->envelopes[envelopeCount] = RCMM_START_BIT_OFF_WIDTH;
        envelopeCount++;

        /* On bit for first 2 bits of mode (there may only be 2 bits to mode depending on protocol) */
        instanceData->envelopes[envelopeCount] = RCMM_BIT_ON_WIDTH;
        envelopeCount++;

        switch (mode)
        {
            case kCIC_IRB_RcmmMode_12Bit:
                /* Off bit for mode */
                instanceData->envelopes[envelopeCount] = pulsePositionCodingValue[modeBits & RCMM_12_BITS_MODE_MASK];
                envelopeCount++;

                /* On bit for address */
                instanceData->envelopes[envelopeCount] = RCMM_BIT_ON_WIDTH;
                envelopeCount++;

                /* Off bit for address */
                instanceData->envelopes[envelopeCount] = pulsePositionCodingValue[address & RCMM_ADDRESS_MASK];
                envelopeCount++;

                /* Data, 4 bit pairs */
                data &= RCMM_12_BIT_DATA_MASK;
                bitMask = 0xC0;
                for (i = 0; i < 4; i++)
                {
                    bitPair = data & bitMask;
                    bitPair >>= 6 - (i * 2);

                    /* On bit for data */
                    instanceData->envelopes[envelopeCount] = RCMM_BIT_ON_WIDTH;
                    envelopeCount++;

                    /* Off bit for data */
                    instanceData->envelopes[envelopeCount] = pulsePositionCodingValue[bitPair];
                    envelopeCount++;
                    bitMask >>= 2;
                }
                break;

            case kCIC_IRB_RcmmMode_24Bit:
                /* Off bit for first half of mode */
                instanceData->envelopes[envelopeCount] =
                    pulsePositionCodingValue[(modeBits & RCMM_14_BITS_MODE_MASK) >> 2];
                envelopeCount++;

                /* On bit for second half of mode */
                instanceData->envelopes[envelopeCount] = RCMM_BIT_ON_WIDTH;
                envelopeCount++;

                /* Off bit for second half of mode */
                instanceData->envelopes[envelopeCount] =
                    pulsePositionCodingValue[(modeBits & RCMM_14_BITS_MODE_MASK) & 0x3];
                envelopeCount++;

                /* Data, 10 bit pairs */
                data &= RCMM_24_BIT_DATA_MASK;
                bitMask = 0xC0000;
                for (i = 0; i < 10; i++)
                {
                    bitPair = data & bitMask;
                    bitPair >>= 18 - (i * 2);

                    /* On bit for data */
                    instanceData->envelopes[envelopeCount] = RCMM_BIT_ON_WIDTH;
                    envelopeCount++;

                    /* Off bit for data */
                    instanceData->envelopes[envelopeCount] = pulsePositionCodingValue[bitPair];
                    envelopeCount++;
                    bitMask >>= 2;
                }
                break;

            case kCIC_IRB_RcmmMode_Oem:
                modeBits &= RCMM_OEM_MODE_MASK;

                /* Off bit for first bit pair of mode */
                instanceData->envelopes[envelopeCount] = pulsePositionCodingValue[(modeBits & 0x30) >> 4];
                envelopeCount++;

                /* On bit for second bit pair of mode */
                instanceData->envelopes[envelopeCount] = RCMM_BIT_ON_WIDTH;
                envelopeCount++;

                /* Off bit for second bit pair of mode */
                instanceData->envelopes[envelopeCount] = pulsePositionCodingValue[(modeBits & 0xC) >> 2];
                envelopeCount++;

                /* On bit for third bit pair of mode */
                instanceData->envelopes[envelopeCount] = RCMM_BIT_ON_WIDTH;
                envelopeCount++;

                /* Off bit for third bit pair of mode */
                instanceData->envelopes[envelopeCount] = pulsePositionCodingValue[modeBits & 0x3];
                envelopeCount++;

                /* Customer ID, 3 bit pairs */
                customerId &= RCMM_OEM_CUSTOMER_ID_MASK;
                bitMask = 0x30;
                for (i = 0; i < 3; i++)
                {
                    bitPair = customerId & bitMask;
                    bitPair >>= 4 - (i * 2);

                    /* On bit for data */
                    instanceData->envelopes[envelopeCount] = RCMM_BIT_ON_WIDTH;
                    envelopeCount++;

                    /* Off bit for data */
                    instanceData->envelopes[envelopeCount] = pulsePositionCodingValue[bitPair];
                    envelopeCount++;
                    bitMask >>= 2;
                }

                /* Data, 6 bit pairs */
                data &= RCMM_OEM_DATA_MASK;
                bitMask = 0xC00;
                for (i = 0; i < 6; i++)
                {
                    bitPair = data & bitMask;
                    bitPair >>= 10 - (i * 2);

                    /* On bit for data */
                    instanceData->envelopes[envelopeCount] = RCMM_BIT_ON_WIDTH;
                    envelopeCount++;

                    /* Off bit for data */
                    instanceData->envelopes[envelopeCount] = pulsePositionCodingValue[bitPair];
                    envelopeCount++;
                    bitMask >>= 2;
                }
                break;
        }

        /* Add signal free time, which is an extension of the previous already off envelope */
        switch (signalFreeTime)
        {
            case kCIC_IRB_RcmmSignalFreeTime_1ms:
                instanceData->envelopes[envelopeCount - 1] += RCMM_1MS_SIGNAL_FREE_TIME;
                break;

            case kCIC_IRB_RcmmSignalFreeTime_3_36ms:
                instanceData->envelopes[envelopeCount - 1] += RCMM_3_36MS_SIGNAL_FREE_TIME;
                break;
        }
        /* Do not increment envelopeCount here, previous envelope extended, not added */

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
