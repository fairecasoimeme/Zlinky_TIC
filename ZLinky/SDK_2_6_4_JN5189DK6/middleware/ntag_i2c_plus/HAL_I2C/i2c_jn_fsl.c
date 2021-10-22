/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "HAL_I2C_jn_fsl.h"
#include "fsl_ntag.h"

/* NTAG power up time, when its VDD line is supplied */
#define NTAG_POWER_UP_DELAY_US            300

int16_t I2C_InitDevice(uint32_t bitrate, clock_name_t input_clock, I2C_Type *instance)
{
    i2c_master_config_t masterConfig;

    /* Switch on NTAG, configure IO pads */
    NTAG_SetState(kNTAG_StateActive);
    /* Wait for NTAG to be ready */
    CLOCK_uDelay(NTAG_POWER_UP_DELAY_US);
    /* Reset NTAG I2C */
    RESET_PeripheralReset(kFC6_RST_SHIFT_RSTn);
    I2C_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate_Bps = bitrate;

    I2C_MasterInit(instance, &masterConfig, CLOCK_GetFreq(input_clock));

    return instance->STAT;
}

int16_t TransmitPoll(I2C_Type *instance, uint8_t address, uint8_t *bytes, size_t len)
{
    uint8_t status;
    i2c_master_transfer_t xfer;

    memset(&xfer, 0, sizeof(xfer));

    xfer.slaveAddress = address;
    xfer.direction = kI2C_Write;
    xfer.data = bytes;
    xfer.dataSize = len;
    xfer.flags = kI2C_TransferDefaultFlag;

    status = I2C_MasterTransferBlocking(instance, &xfer);

    return status;
}

int16_t ReceivePoll(I2C_Type *instance, uint8_t address, uint8_t *bytes, size_t len)
{
    uint8_t status;
    i2c_master_transfer_t xfer;

    memset(&xfer, 0, sizeof(xfer));

    xfer.slaveAddress = address;
    xfer.direction = kI2C_Read;
    xfer.data = bytes;
    xfer.dataSize = len;
    xfer.flags = kI2C_TransferDefaultFlag;
    
    status = I2C_MasterTransferBlocking(instance, &xfer);

    return status;
}

int16_t I2C_CloseDevice(I2C_Type *instance)
{
    /* Disable I2C master */
    I2C_MasterDeinit(instance);

    /* Switch off NTAG, configure IO pads */
    NTAG_SetState(kNTAG_StateInactive);

    return instance->STAT;
}