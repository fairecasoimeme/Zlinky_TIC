/*
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef _NTAG_DRIVER_INTERN_H_
#define _NTAG_DRIVER_INTERN_H_

/***********************************************************************/
/* INCLUDES                                                            */
/***********************************************************************/
#include "HAL_I2C_driver.h"
#include "ntag_driver.h"
#ifdef HAVE_NTAG_INTERRUPT
#include "HAL_ISR_driver.h"
#endif
/***********************************************************************/
/* DEFINES                                                             */
/***********************************************************************/
#define TX_START HAL_I2C_TX_RESERVED_BYTES
#define RX_START HAL_I2C_RX_RESERVED_BYTES
#define NTAG_ADDRESS_SIZE 1
/***********************************************************************/
/* TYPES                                                               */
/***********************************************************************/
struct NTAG_DEVICE
{
    NTAG_STATUS_T status;
    HAL_I2C_HANDLE_T i2cbus;
    uint8_t address;
#ifdef HAVE_NTAG_INTERRUPT
    ISR_SOURCE_T isr;
#endif
    uint8_t tx_buffer[TX_START + NTAG_I2C_BLOCK_SIZE + NTAG_ADDRESS_SIZE];
    uint8_t rx_buffer[RX_START + NTAG_I2C_BLOCK_SIZE];
};

/***********************************************************************/
/* GLOBAL VARIABLES                                                    */
/***********************************************************************/
extern struct NTAG_DEVICE ntag_device_list[];

/***********************************************************************/
/* GLOBAL FUNCTION PROTOTYPES                                          */
/***********************************************************************/
/**
 * \brief read the specified block from the selected NTAG device
 *
 *	This functions reads the specified 16-byte sized block from the selected
 *	NTAG device. Reading an invalid block will result in a failed read.
 *
 * \param	ntag	handle to identify the NTAG device instance
 * \param	bytes	array of bytes to store read data
 * \param	block	block number to read
 * \param	len 	number of bytes to be read
 *
 * \return			TRUE on failure
 */
BOOL NTAG_ReadBlock(NTAG_HANDLE_T ntag, uint8_t block, uint8_t *bytes, uint8_t len);

/**
 * \brief write the specified block to the selected NTAG device
 *
 *	This functions writes the specified 16-byte sized block to selected NTAG device.
 *	Writing to an invalid block is undefined. Writing less than a full block will write
 *	0 to the rest of the block.
 *
 *	When directly using this function care has to be taken when writing the first byte
 *	of the first block( block = 0 ) because it will modify the I2C address of the device
 *	whereas on read the first byte will contain part of the serial number.
 *
 * \param	ntag	handle to identify the NTAG device instance
 * \param	bytes	array of bytes to be written
 * \param	block	block number to write
 * \param	len 	number of bytes to be written
 *
 * \return			TRUE on failure
 */
BOOL NTAG_WriteBlock(NTAG_HANDLE_T ntag, uint8_t block, const uint8_t *bytes, uint8_t len);

#endif /* _NTAG_DRIVER_INTERN_H_ */
