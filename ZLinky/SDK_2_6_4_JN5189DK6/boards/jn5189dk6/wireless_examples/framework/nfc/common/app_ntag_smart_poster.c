/*
* Copyright 2020 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/************************************************************************************
 *************************************************************************************
 * Include
 *************************************************************************************
 ************************************************************************************/

#include "app_ntag_smart_poster.h"
#include "ntag_defines.h"
#include "ntag_driver.h"
#include "FunctionLib.h"



/************************************************************************************
 *************************************************************************************
 * Private macros
 *************************************************************************************
 ************************************************************************************/
#define NFC_NDEF_RECORD_HEADER_SMART_POSTER 0xD1 /* MB=1, ME=1, TNF=1 */
#define NFC_NDEF_RECORD_HEADER_TITLE 0x91 /* SR=1, ME=1, TNF=1 */
#define NFC_NDEF_RECORD_HEADER_URI 0x51 /* MB=1, SR=1, TNF=1 */

#define NFC_NXP_URI {'n','x','p','.','c','o','m'}
#define NFC_NXP_URI_LEN 7
#define NFC_NXP_PAYLOAD_TITLE {'N','X','P',' ','N','T','A','G',' ','D','E','M','O'}
#define NFC_NXP_PAYLOAD_TITLE_LEN 13
#define NFC_NDEF_SMART_POSTER_RECORD_NAME {'S','p'}
#define NFC_NDEF_SMART_POSTER_RECORD_NAME_LEN 2
#define NFC_NDEF_URI_RECORD_NAME 'U'
#define NFC_NDEF_URI_RECORD_NAME_LEN 1
#define NFC_NDEF_URI_HTTPS_ABBREVIATION 0x02
#define NFC_NDEF_TEXT_RECORD_TYPE 'T'
#define NFC_NDEF_TEXT_RECORD_TYPE_LEN 1
#define NFC_NDEF_ISO_LANGUAGE_CODE_US_ENGLISH {'e','n','-','U','S'}
#define NFC_NDEF_ISO_LANGUAGE_CODE_US_ENGLISH_LEN 5

/************************************************************************************
 *************************************************************************************
 * Private type definitions
 *************************************************************************************
 ************************************************************************************/
typedef PACKED_STRUCT
{
    uint8_t headerNdef;
    uint8_t recordNameLen;
    uint8_t textPayloadLen;
    uint8_t recordType;
    uint8_t isoLanguageCodeLen;
    uint8_t isoLanguageCode[NFC_NDEF_ISO_LANGUAGE_CODE_US_ENGLISH_LEN];
    uint8_t textLen[NFC_NXP_PAYLOAD_TITLE_LEN];
} NfcNdefTitle_t; 

typedef PACKED_STRUCT
{
    uint8_t headerNdef;
    uint8_t recordNameLen;
    uint8_t uriPayloadLen;
    uint8_t recordType;
    uint8_t abbreviation;
    uint8_t uri[NFC_NXP_URI_LEN];
} NfcNdefUri_t; 

typedef PACKED_STRUCT
{
    NfcNdefTitle_t title;
    NfcNdefUri_t uri;
} NfcSmartPosterPayload_t; 
       
typedef PACKED_STRUCT
{
    uint8_t headerNdef;
    uint8_t recordNameLen;
    uint8_t payloadLen;
    uint8_t recordName[NFC_NDEF_SMART_POSTER_RECORD_NAME_LEN];
    NfcSmartPosterPayload_t payload;
} NfcSmartPosterNdef_t; 

/************************************************************************************
 *************************************************************************************
 * Private memory declarations
 *************************************************************************************
 ************************************************************************************/
static const NfcNdefUri_t smartPosterUri = {NFC_NDEF_RECORD_HEADER_URI,
                                        NFC_NDEF_URI_RECORD_NAME_LEN, 
                                        sizeof(smartPosterUri.uri) + sizeof(smartPosterUri.abbreviation),
                                        NFC_NDEF_URI_RECORD_NAME,
                                        NFC_NDEF_URI_HTTPS_ABBREVIATION,
                                        NFC_NXP_URI};
static const NfcNdefTitle_t smartPosterTitle = {NFC_NDEF_RECORD_HEADER_TITLE,
                                            NFC_NDEF_TEXT_RECORD_TYPE_LEN,
                                            sizeof(smartPosterTitle.isoLanguageCodeLen) + sizeof(smartPosterTitle.isoLanguageCode) + sizeof(smartPosterTitle.textLen),
                                            NFC_NDEF_TEXT_RECORD_TYPE,
                                            NFC_NDEF_ISO_LANGUAGE_CODE_US_ENGLISH_LEN,
                                            NFC_NDEF_ISO_LANGUAGE_CODE_US_ENGLISH,
                                            NFC_NXP_PAYLOAD_TITLE};
static const NfcSmartPosterNdef_t smartPoster = { NFC_NDEF_RECORD_HEADER_SMART_POSTER,
                                                NFC_NDEF_SMART_POSTER_RECORD_NAME_LEN,
                                                sizeof(smartPosterTitle)+sizeof(smartPosterUri),
                                                NFC_NDEF_SMART_POSTER_RECORD_NAME,
                                                {smartPosterTitle, smartPosterUri}};

/************************************************************************************
 *************************************************************************************
 * Private functions prototypes
 *************************************************************************************
 ************************************************************************************/

/************************************************************************************
 *************************************************************************************
 * Public functions
 *************************************************************************************
 ************************************************************************************/

bool_t app_ntag_smart_poster_eepromWrite(NTAG_HANDLE_T ntagHandleInstance)
{
    bool_t result = FALSE;
    uint32_t ndefSize = sizeof(smartPoster);
    uint32_t addrToWrite = NTAG_I2C_BLOCK_SIZE;
    uint8_t buf[4];
    uint8_t terminatorTLV = 0xFE;
    do
    {
        if (ndefSize < 0xFF)
        {
            buf[0] = 0x3;
            buf[1] = ndefSize;
            if (NTAG_WriteBytes(ntagHandleInstance, addrToWrite, buf, 2))
                break;
            addrToWrite += 2;
        }
        else if (ndefSize <= 0xFFFF)
        {
            buf[0] = 0x3;
            buf[1] = 0xFF;
            buf[2] = (uint8_t) (ndefSize & 0x0000FF00) >> 8;
            buf[3] = (uint8_t)(ndefSize & 0x000000FF);
            if (NTAG_WriteBytes(ntagHandleInstance, addrToWrite, buf, 4))
                break;
            addrToWrite += 4;
        }
        if (NTAG_WriteBytes(ntagHandleInstance, addrToWrite, (uint8_t *)&smartPoster, ndefSize))
            break;
        addrToWrite += ndefSize;
        if (NTAG_WriteBytes(ntagHandleInstance, addrToWrite, &terminatorTLV, 1))
            break;
        result = TRUE;
    } while (0);
    return result;
}

uint16_t app_ntag_smart_poster_getSize()
{
    return sizeof(smartPoster);
}