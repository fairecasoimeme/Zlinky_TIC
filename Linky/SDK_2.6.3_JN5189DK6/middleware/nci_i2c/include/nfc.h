/*! *********************************************************************************
* Copyright 2017-2020 NXP
* All rights reserved.
*
** SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef NFC_H_
#define NFC_H_

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/* NFC data sizes */
#define NFC_HEADER_SIZE                   16  /*!< Size of NTAG header data (stored at byte address 0) */
#define NFC_VERSION_SIZE                   8  /*!< Size of NTAG version data */
#define NFC_SRAM_SIZE                     64  /*!< Size of NTAG sram data */

/* Register offset defines */
#define NFC_OFFSET_NC_REG                  0  /*!< Offset of NC_REG register */
#define NFC_OFFSET_LAST_NDEF_BLOCK         1  /*!< Offset of LAST_NDEF_BLOCK register */
#define NFC_OFFSET_NS_REG                  6  /*!< Offset of NS_REG register */

/* NC Register bits */
#define NFC_MASK_NC_REG_PTHRU_ON_OFF    0x40  /*!< NC_REG PTHRU_ON_OFF bit */
#define NFC_MASK_NC_REG_PTHRU_DIR       0x01  /*!< NC_REG DIR bit */

/* NS Register bits */
#define NFC_MASK_NS_REG_NDEF_DATA_READ  0x80  /*!< NS_REG NDEF_DATA_READ bit */
#define NFC_MASK_NS_REG_I2C_LOCKED      0x40  /*!< NS_REG I2C_LOCKED bit */
#define NFC_MASK_NS_REG_SRAM_I2C_READY  0x10  /*!< NS_REG SRAM_I2C_READY bit */
#define NFC_MASK_NS_REG_SRAM_RF_READY   0x08  /*!< NS_REG SRAM_RF_READY bit */

/* NTAG part number defines */
#define NFC_NTAG_UNKNOWN                   0  /*!< Unknown NTAG model */
#define NFC_NTAG_NT3H1101              31101  /*!< NT3H1101 NTAG model */
#define NFC_NTAG_NT3H1201              31201  /*!< NT3H1201 NTAG model */
#define NFC_NTAG_NT3H2111              32111  /*!< NT3H2111 NTAG model */
#define NFC_NTAG_NT3H2211              32211  /*!< NT3H2211 NTAG model */

/* NFC commands (these commands need renaming to make them all NFC_CMD with the NWK bits later as now used in other places to NWK modules) */
/* NFC generic/pseudo commands */
#define NFC_CMD_NONE                    0x00  /*!< Null command value */
/* NFC_OTA commands */
#define NFC_CMD_OTA_DATA                0x02  /*!< OTA data command */
#define NFC_CMD_OTA_IMAGE               0x03  /*!< OTA image command */
#define NFC_CMD_OTA_FLAG_MORE           0x80  /*!< OTA more commands to follow flag */
/* NFC_NWK NTAG commands */
#define NFC_CMD_NWK_NTAG_JOIN_WITH_CODE 0x41  /*!< NTAG_NWK request to NCI to join using installation code encrypted key */
/* NFC_NWK NCI commands */
#define NFC_CMD_NWK_NCI_FACTORY_RESET   0xA0  /*!< NCI_NWK request to NTAG to perform a factory reset */
#define NFC_CMD_NWK_NCI_JOIN_WITH_CODE  0xA1  /*!< NCI_NWK request to NTAG to join using installation code encrypted key */

/* Newline string for debugging */
#if (JENNIC_CHIP_FAMILY == JN518x)
#define NFC_DBG_NL                    "\n\r"  /*!< New line for debug */
#else
#define NFC_DBG_NL                      "\n"  /*!< New line for debug */
#endif

#endif /* NFC_H_ */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
