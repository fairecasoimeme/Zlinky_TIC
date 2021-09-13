/*
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "HAL_I2C_kinetis_fsl.h"
//---------------------------------------------------------------------
int16_t I2C_InitDevice(uint32_t bitrate, clock_name_t input_clock, I2C_Type *instance)
{
    i2c_master_config_t masterConfig;
    I2C_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate_Bps = bitrate;
    uint32_t sourceClock;
    sourceClock = CLOCK_GetFreq(input_clock);

    I2C_MasterInit(instance, &masterConfig, sourceClock);

    return instance->S;
}

//---------------------------------------------------------------------
int16_t TransmitPoll(I2C_Type *instance, uint8_t address, uint8_t *bytes, size_t len)
{
    uint8_t status;
    i2c_master_transfer_t xfer;

    memset(&xfer, 0, sizeof(xfer));

    xfer.slaveAddress = address;
    xfer.direction = kI2C_Write;
    xfer.data = bytes;
    xfer.dataSize = len;

    status = I2C_MasterTransferBlocking(instance, &xfer);

    return status;
}

//---------------------------------------------------------------------
int16_t ReceivePoll(I2C_Type *instance, uint8_t address, uint8_t *bytes, size_t len)
{
    uint8_t status;
    i2c_master_transfer_t xfer;

    memset(&xfer, 0, sizeof(xfer));

    xfer.slaveAddress = address;
    xfer.direction = kI2C_Read;
    xfer.data = bytes;
    xfer.dataSize = len;

    status = I2C_MasterTransferBlocking(instance, &xfer);

    return status;
}
