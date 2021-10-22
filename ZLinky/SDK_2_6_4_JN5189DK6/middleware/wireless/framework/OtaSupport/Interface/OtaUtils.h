/*! *********************************************************************************
* Copyright 2020 NXP
* All rights reserved.
*
* \file
*
* This is the header file for the OTA Programming Support.
*
** SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef _OTA_UTILS_H_
#define _OTA_UTILS_H_

/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
********************************************************************************** */

#include "EmbeddedTypes.h"
#include "psector_api.h"
#include "rom_secure.h"

/*! *********************************************************************************
*************************************************************************************
* Public MACRO
*************************************************************************************
********************************************************************************** */

#define OTA_UTILS_IMAGE_INVALID_ADDR              (0xFFFFFFFF)

/*! *********************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
********************************************************************************** */

typedef enum {
  gOtaUtilsSuccess_c = 0,
  gOtaUtilsReadError_c,
  gOtaUtilsInvalidKey_c,
  gOtaUtilsError_c,
} otaUtilsResult_t;

typedef struct {
    const uint8_t *pSoftKeyAes;
} sOtaUtilsSoftwareKey;

typedef enum {
    eCipherKeyNone,
    eEfuseKey,
    eSoftwareKey,
} eEncryptionKeyType;

/*
 * Function pointer to read from the flash driver
 */
typedef otaUtilsResult_t (*OtaUtils_EEPROM_ReadData)(uint16_t nbBytes,
                                                     uint32_t address,
                                                     uint8_t *pInbuf);

/*
 * Function allowing to read data
 * Note: a OtaUtils_EEPROM_ReadData function is required to access flash drivers
 */
typedef otaUtilsResult_t (*OtaUtils_ReadBytes)(uint16_t nbBytes,
                                               uint32_t address,
                                               uint8_t *pInbuf,
                                               void *pParam,
                                               OtaUtils_EEPROM_ReadData eepromFunction);

/*! *********************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
********************************************************************************** */

/*! *********************************************************************************
* \brief  Allows to read bytes from the Internal Flash
*
* \param[in] nbBytesToRead: number of bytes to read
* \param[in] address: offset/address to read from the internal flash
* \param[in/out] pOutbuf: buffer to fill
* \param[in] pParam: optional param
* \param[in] pFunctionEepromRead: Eeprom function
*
* \return    otaUtilsResult_t
*
********************************************************************************** */
otaUtilsResult_t OtaUtils_ReadFromInternalFlash(uint16_t nbBytesToRead,
                                                uint32_t address,
                                                uint8_t *pOutbuf,
                                                void * pParam,
                                                OtaUtils_EEPROM_ReadData pFunctionEepromRead);

/*! *********************************************************************************
* \brief  Allows to read bytes from an encrypted flash using Efuse key
*
* \param[in] nbBytesToRead: number of bytes to read
* \param[in] address: offset/address to read from the internal flash
* \param[in/out] pOutbuf: buffer to fill
* \param[in] pParam: optional param
* \param[in] pFunctionEepromRead: Eeprom function
*
* \return    otaUtilsResult_t
*
********************************************************************************** */
otaUtilsResult_t OtaUtils_ReadFromEncryptedExtFlashEfuseKey(uint16_t nbBytesToRead,
                                                            uint32_t address,
                                                            uint8_t *pOutbuf,
                                                            void *pParam,
                                                            OtaUtils_EEPROM_ReadData pFunctionEepromRead);

/*! *********************************************************************************
* \brief  Allows to read bytes from an encrypted flash using Software key
*
* \param[in] nbBytesToRead: number of bytes to read
* \param[in] address: offset/address to read from the internal flash
* \param[in/out] pOutbuf: buffer to fill
* \param[in] pParam: the software key (format: sOtaUtilsSoftwareKey)
* \param[in] pFunctionEepromRead: Eeprom function
*
* \return    otaUtilsResult_t
*
********************************************************************************** */
otaUtilsResult_t OtaUtils_ReadFromEncryptedExtFlashSoftwareKey(uint16_t nbBytesToRead,
                                                            uint32_t address,
                                                            uint8_t *pOutbuf,
                                                            void *pParam,
                                                            OtaUtils_EEPROM_ReadData pFunctionEepromRead);

/*! *********************************************************************************
* \brief  Allows to read bytes from an unencrypted external flash
*
* \param[in] nbBytesToRead: number of bytes to read
* \param[in] address: offset/address to read from the internal flash
* \param[in/out] pOutbuf: buffer to fill
* \param[in] pParam: the software key (format: sOtaUtilsSoftwareKey)
* \param[in] pFunctionEepromRead: Eeprom function
*
* \return    otaUtilsResult_t
*
********************************************************************************** */
otaUtilsResult_t OtaUtils_ReadFromUnencryptedExtFlash(uint16_t nbBytesToRead,
                                                            uint32_t address,
                                                            uint8_t *pOutbuf,
                                                            void *pParam,
                                                            OtaUtils_EEPROM_ReadData pFunctionEepromRead);

/*! *********************************************************************************
* \brief  Allows validate an image
*
* \param[in] pFunctionRead: function pointer to read data
*            can be OtaUtils_ReadFromInternalFlash/OtaUtils_ReadFromEncryptedExtFlashEfuseKey
*            /OtaUtils_ReadFromEncryptedExtFlashSoftwareKey or others
* \param[in] pFunctionEepromRead: function pointer to data from a memory driver
* \param[in] imgAddr: address/offset from where to validate the image
* \param[in] minValidAddr: min valid address/offset
* \param[in] pRootCert: root certificate uses to authenticate the image
* \param[in] inOtaCheck: the image to check is coming from an OTA
* \param[in] isRemappable: image support remap or not
*
* \return  target addr of the image or IMAGE_INVALID_ADDR if not valid
*
********************************************************************************** */
uint32_t OtaUtils_ValidateImage(OtaUtils_ReadBytes pFunctionRead,
                                void *pReadFunctionParam,
                                OtaUtils_EEPROM_ReadData pFunctionEepromRead,
                                uint32_t imgAddr,
                                uint32_t minValidAddr,
                                const IMAGE_CERT_T * pRootCert,
                                bool_t inOtaCheck, bool_t isRemappable);

/*! *********************************************************************************
* \brief  Allows to reconstruct the device root certificate
*
* \param[in/out] pCert: certificate to be filled
* \param[in] pPage0 content
* \param[in] pFlashPage content or NULL
*
* \return otaUtilsResult_t
*
********************************************************************************** */
otaUtilsResult_t OtaUtils_ReconstructRootCert(IMAGE_CERT_T *pCert, const psector_page_data_t* pPage0, const psector_page_data_t* pFlashPage);



#endif /* _OTA_UTILS_H_ */