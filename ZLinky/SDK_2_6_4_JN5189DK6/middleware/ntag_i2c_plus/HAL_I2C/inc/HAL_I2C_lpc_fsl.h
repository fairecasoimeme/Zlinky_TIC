/*
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HAL_I2C_FRDM_FSL_H
#define HAL_I2C_FRDM_FSL_H

#include "fsl_i2c.h"
#include "ntag_defines.h"

/* Set up hardware for FRDM K82F */
int16_t I2C_InitDevice(uint32_t bitrate, clock_name_t input_clock, I2C_Type *instance);

/* send and receive functions */
int16_t ReceivePoll(I2C_Type *instance, uint8_t address, uint8_t *bytes, size_t len);
int16_t TransmitPoll(I2C_Type *instance, uint8_t address, uint8_t *bytes, size_t len);

#endif /* HAL_I2C_FRDM_FSL_H */
