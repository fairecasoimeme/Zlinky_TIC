/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_spifi.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define IS_WORD_ALIGNED(x) ((((uint32_t)(x) & 0x3) == 0))

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.spifi"
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Array of SPIFI peripheral base address. */
static SPIFI_Type *const s_spifiBases[] = SPIFI_BASE_PTRS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/* Array of SPIFI clock name. */
static const clock_ip_name_t s_spifiClock[] = SPIFI_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

#if !(defined(FSL_FEATURE_SPIFI_HAS_NO_RESET) && FSL_FEATURE_SPIFI_HAS_NO_RESET)
static const reset_ip_name_t s_spifiResets[] = SPIFI_RSTS;
#endif

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * brief Get the SPIFI instance from peripheral base address.
 *
 * param base SPIFI peripheral base address.
 * return SPIFI instance.
 */
uint32_t SPIFI_GetInstance(SPIFI_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_spifiBases); instance++)
    {
        if (s_spifiBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_spifiBases));

    return instance;
}

/*!
 * brief Get SPIFI default configure settings.
 *
 * param config  SPIFI config structure pointer.
 */
void SPIFI_GetDefaultConfig(spifi_config_t *config)
{
    /* Initializes the configure structure to zero. */
    memset(config, 0, sizeof(*config));

    config->timeout              = 0xFFFFU;
    config->csHighTime           = 0xFU;
    config->disablePrefetch      = false;
    config->disableCachePrefech  = false;
    config->isFeedbackClock      = true;
    config->spiMode              = kSPIFI_SPISckLow;
    config->isReadFullClockCycle = true;
    config->dualMode             = kSPIFI_QuadMode;
}

/*!
 * brief Initializes the SPIFI with the user configuration structure.
 *
 * This function configures the SPIFI module with the user-defined configuration.
 *
 * param base     SPIFI peripheral base address.
 * param config   The pointer to the configuration structure.
 */
void SPIFI_Init(SPIFI_Type *base, const spifi_config_t *config)
{
    assert(config);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Enable the SAI clock */
    CLOCK_EnableClock(s_spifiClock[SPIFI_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

#if !(defined(FSL_FEATURE_SPIFI_HAS_NO_RESET) && FSL_FEATURE_SPIFI_HAS_NO_RESET)
    RESET_PeripheralReset(s_spifiResets[SPIFI_GetInstance(base)]);
#endif

    /* Reset the Command register */
    SPIFI_ResetCommand(base);

    /* Set time delay parameter */
    base->CTRL = SPIFI_CTRL_TIMEOUT(config->timeout) | SPIFI_CTRL_CSHIGH(config->csHighTime) |
                 SPIFI_CTRL_D_PRFTCH_DIS(config->disablePrefetch) | SPIFI_CTRL_MODE3(config->spiMode) |
                 SPIFI_CTRL_PRFTCH_DIS(config->disableCachePrefech) | SPIFI_CTRL_DUAL(config->dualMode) |
                 SPIFI_CTRL_RFCLK(config->isReadFullClockCycle) | SPIFI_CTRL_FBCLK(config->isFeedbackClock);
}

/*!
 * brief Deinitializes the SPIFI regions.
 *
 * param base     SPIFI peripheral base address.
 */
void SPIFI_Deinit(SPIFI_Type *base)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Enable the SAI clock */
    CLOCK_DisableClock(s_spifiClock[SPIFI_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Set SPIFI flash command.
 *
 * param base     SPIFI peripheral base address.
 * param cmd      SPIFI command structure pointer.
 */
void SPIFI_SetCommand(SPIFI_Type *base, const spifi_command_t *cmd)
{
    /* If SPIFI in memory mode, call reset function to abort memory mode */
    if (SPIFI_GetStatusFlag(base) & SPIFI_STAT_MCINIT_MASK)
    {
        SPIFI_ResetCommand(base);
    }

    /* Wait for other command finished */
    while (SPIFI_GetStatusFlag(base) & SPIFI_STAT_CMD_MASK)
    {
    }

    base->CMD = SPIFI_CMD_DATALEN(cmd->dataLen) | SPIFI_CMD_POLL(cmd->isPollMode) | SPIFI_CMD_DOUT(cmd->direction) |
                SPIFI_CMD_INTLEN(cmd->intermediateBytes) | SPIFI_CMD_FIELDFORM(cmd->format) |
                SPIFI_CMD_FRAMEFORM(cmd->type) | SPIFI_CMD_OPCODE(cmd->opcode);
}

/*!
 * brief Set SPIFI flash AHB read command.
 *
 * Call this function means SPIFI enters to memory mode, while users need to use command, a SPIFI_ResetCommand shall
 * be called.
 *
 * param base     SPIFI peripheral base address.
 * param cmd      SPIFI command structure pointer.
 */
void SPIFI_SetMemoryCommand(SPIFI_Type *base, const spifi_command_t *cmd)
{
    /* Wait for the CMD flag be 0 */
    while (SPIFI_GetStatusFlag(base) & SPIFI_STAT_CMD_MASK)
    {
    }

    base->MCMD = SPIFI_MCMD_POLL(0U) | SPIFI_MCMD_DOUT(0U) | SPIFI_MCMD_INTLEN(cmd->intermediateBytes) |
                 SPIFI_MCMD_FIELDFORM(cmd->format) | SPIFI_MCMD_FRAMEFORM(cmd->type) | SPIFI_MCMD_OPCODE(cmd->opcode);

    /* Wait for the command written */
    while ((base->STAT & SPIFI_STAT_MCINIT_MASK) == 0)
    {
    }
}

void SPIFI_WriteBuffer(SPIFI_Type *base, uint8_t *buf, size_t size_to_write)
{
    size_t remaining_bytes;
#ifdef CPU_JN518X
    remaining_bytes = size_to_write & 0x3;
#else
    remaining_bytes = 0;
#endif
    uint32_t data;
    uint8_t *src = &buf[0];

    /* let end point to the last whole word of the buffer */
    uint8_t *end = &buf[size_to_write-remaining_bytes];

    if (IS_WORD_ALIGNED(buf))
    {
        uint32_t * p_src_w32 = (uint32_t*)src;
        uint32_t * p_end_w32 = (uint32_t*)end;

        while (p_src_w32 < p_end_w32)
        {
            data = *p_src_w32++;
            SPIFI_WriteData(SPIFI, data);
        }
    }
    else
    {
        while (src < end)
        {
            data = (uint32_t)*src++;
            data |= ((uint32_t)*src++) << 8;
            data |= ((uint32_t)*src++) << 16;
            data |= ((uint32_t)*src++) << 24;
            SPIFI_WriteData(SPIFI, data);
        }
    }
#ifdef CPU_JN518X
    src = end;
    end += remaining_bytes;
    while (src < end)
    {
        SPIFI_Write8bitWord(base, *src++);
    }
#endif
}

#ifdef CPU_JN518X
/* The JN518X SPIFI data register will throw a hard fault if more data is
 * passed to it than there is remaining to be sent. Must keep track of how
 * many bytes are needed and perform 1- 2- or 3-byte writes if needed */
void SPIFI_WritePartialWord(SPIFI_Type *base, uint32_t data, uint8_t partial_write_width)
{
    switch (partial_write_width)
    {
        case 0:
        default:
            base->DATA = data;
            break;
        case 1:
            SPIFI_Write8bitWord(base, (uint8_t)data);
            break;

       case 2:
            SPIFI_Write16bitWord(base, (uint16_t)data);
            break;

        case 3:
        {
            __IO uint8_t *reg8 = (__IO uint8_t *) &base->DATA;
            *reg8 = (uint8_t)data;
            *reg8 = (uint8_t)(data >> 8);
            *reg8 = (uint8_t)(data >> 16);
        }
        break;
    }
}

uint32_t SPIFI_ReadPartialWord(SPIFI_Type *base,  uint8_t partial_read_width)
{
    uint32_t data;
    switch (partial_read_width)
    {
    case 0:
    default:
            data = base->DATA;
            break;
    case 1:
        data = SPIFI_Read8bitWord(base);
        break;

    case 2:
        data = SPIFI_Read16bitWord(base);
        break;

    case 3:
        {
            __IO uint8_t *reg8 = (__IO uint8_t *) &base->DATA;
            data = *reg8;
            data |= (*reg8 << 8);
            data |= (*reg8 << 16);
        }
        break;
    }
    return data;
}
#endif    

/*!
 * brief Write a halfword data in address of SPIFI.
 *
 * Users can write a halfword data into SPIFI address.
 *
 * param base     SPIFI peripheral base address.
 * param data     Data need be write.
 */
void SPIFI_WriteDataHalfword(SPIFI_Type *base, uint16_t data)
{
    volatile uint8_t *dataReg = ((volatile uint8_t *)(&(base->DATA)));

    *dataReg = (data & 0xFFU);
    dataReg++;
    *dataReg = ((data >> 8U) & 0xFFU);
}

/*!
 * brief Read a halfword data from serial flash.
 *
 * param base     SPIFI peripheral base address.
 * return Data input from flash.
 */
uint16_t SPIFI_ReadDataHalfword(SPIFI_Type *base)
{
    uint16_t val              = 0;
    volatile uint8_t *dataReg = ((volatile uint8_t *)(&(base->DATA)));

    val = ((*dataReg) | (uint16_t)((uint16_t)(*(dataReg + 1U)) << 8U));

    return val;
}
