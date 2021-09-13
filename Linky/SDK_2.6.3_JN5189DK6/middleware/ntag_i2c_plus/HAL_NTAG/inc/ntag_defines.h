/*
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef NTAG_DEFINES_H
#define NTAG_DEFINES_H
/** @file ntag_defines.h
 * \brief Constants used by the NTAG I2C tag driver.
 */

#define NTAG_MAX_WRITE_DELAY_MS 10

#define NTAG_I2C_BLOCK_SIZE 0x10

#ifndef NULL
#define NULL 0
#endif

//----------------------------------------------------------------------
///
/// memory addresses as seen from i2c interface
#define NTAG_MEM_ADRR_I2C_ADDRESS 0x00
#define NTAG_MEM_BLOCK_START_USER_MEMORY 0x01
#define NTAG_MEM_ADDR_START_USER_MEMORY NTAG_MEM_BLOCK_START_USER_MEMORY *NTAG_I2C_BLOCK_SIZE

#define NTAG_MEM_BLOCK_CONFIGURATION_2k 0x7A
#define NTAG_MEM_ADDR_CONFIGURATION_2k NTAG_MEM_BLOCK_CONFIGURATION_2k *NTAG_I2C_BLOCK_SIZE
#define NTAG_MEM_BLOCK_CONFIGURATION_1k 0x3A
#define NTAG_MEM_ADDR_CONFIGURATION_1k NTAG_MEM_BLOCK_CONFIGURATION_1k *NTAG_I2C_BLOCK_SIZE

#define NTAG_MEM_ADDR_DEFAULT_REGS NTAG_MEM_BLOCK_DEFAULT_REGS *NTAG_I2C_BLOCK_SIZE
#define NTAG_MEM_BLOCK_SESSION_REGS 0xFE

#define NTAG_MEM_BLOCK_START_SRAM 0xF8
#define NTAG_MEM_ADDR_START_SRAM NTAG_MEM_BLOCK_START_SRAM *NTAG_I2C_BLOCK_SIZE

#define NTAG_MEM_SRAM_BLOCKS 4
#define NTAG_MEM_SRAM_SIZE NTAG_MEM_SRAM_BLOCKS *NTAG_I2C_BLOCK_SIZE

//----------------------------------------------------------------------
///
///  byte offset in session and configuration
#define NTAG_MEM_OFFSET_NC_REG 0x00
#define NTAG_MEM_OFFSET_LAST_NDEF_BLOCK 0x01
#define NTAG_MEM_OFFSET_SRAM_MIRROR_BLOCK 0x02
#define NTAG_MEM_OFFSET_WDT_LS 0x03
#define NTAG_MEM_OFFSET_WDT_MS 0x04
#define NTAG_MEM_OFFSET_I2C_CLOCK_STR 0x05
#define NTAG_MEM_OFFSET_REG_LOCK 0x06
#define NTAG_MEM_OFFSET_NS_REG 0x06

//----------------------------------------------------------------------
///
///  memory bit masks
#define NTAG_NC_REG_MASK_I2C_RST_ON_OFF 0x80
#define NTAG_NC_REG_MASK_PTHRU_ON_OFF 0x40
#define NTAG_NC_REG_MASK_FD_OFF 0x30
#define NTAG_NC_REG_MASK_FD_ON 0x0C
#define NTAG_NC_REG_MASK_SRAM_MIRROR_ON_OFF 0x02
#define NTAG_NC_REG_MASK_TRANSFER_DIR 0x01

#define NTAG_REG_LOCK_MASK_CONF_BYTES_ACCESS_I2C 0x02
#define NTAG_REG_LOCK_MASK_CONF_BYTES_ACCESS_RF 0x01

#define NTAG_NS_REG_MASK_NDEF_DATA_READ 0x80
#define NTAG_NS_REG_MASK_I2C_LOCKED 0x40
#define NTAG_NS_REG_MASK_RF_LOCKED 0x20
#define NTAG_NS_REG_MASK_SRAM_I2C_READY 0x10
#define NTAG_NS_REG_MASK_SRAM_RF_READY 0x08
#define NTAG_NS_REG_MASK_EEPROM_WR_ERR 0x04
#define NTAG_NS_REG_MASK_EEPROM_WR_BUSY 0x02
#define NTAG_NS_REG_MASK_RF_FIELD_PRESENT 0x01

//----------------------------------------------------------------------
///
/// error codes

#define NTAG_ERR_OK 0x00
#define NTAG_ERR_COMMUNICATION -0x01
#define NTAG_ERR_BUFF_OVERFLOW -0x02
#define NTAG_ERR_INIT_FAILED -0x03
#define NTAG_ERR_INVALID_PARAM -0x09

#endif // NTAG_DEFINES_H
