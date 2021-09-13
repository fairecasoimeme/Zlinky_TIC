/*! *********************************************************************************
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2019 NXP
* All rights reserved.
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************/

#ifndef _BLOB_UTILS_H_
#define _BLOB_UTILS_H_

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "EmbeddedTypes.h"

/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/

#define SOTA_BLOB_POSITION_1 1
#define SOTA_BLOB_POSITION_2 2
#define SOTA_BLOB_POSITION_3 3
#define SOTA_BLOB_POSITION_4 4
#define SOTA_BLOB_POSITION_5 5
#define SOTA_BLOB_POSITION_6 6
#define SOTA_BLOB_POSITION_7 7
#define SOTA_BLOB_POSITION_8 8

#define SOTA_MAX_BLOB_NB_WITH_SSBL 9 /* SSBL included */
#define SOTA_MAX_BLOB_NB_WITHOUT_SSBL (SOTA_MAX_BLOB_NB_WITH_SSBL-1) /* SSBL not included */

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

typedef uint8_t * (*Sota_BlobUtilsFunction)(void);

typedef struct sotaDependencyTableElement_tag
{
    uint32_t   **pu32BlobJumpTable;
    uint32_t    blobFlashPosition;
    Sota_BlobUtilsFunction blobFunctionGetZgOtaHeader;
} sotaDependencyTableElement_t;


typedef struct {
    uint16_t blobId;
    uint32_t blobVersion;
} sotaCompatibilityListElem_t;

typedef enum
{
    E_RESET_COLD,
    E_RESET_WARM
} BlobResetMode_t;

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief  Initializes the dependencyTable with the address of the jumptable
*
* \param[in/out] dependencyTable  jump table dependency
* \param[in] 	 tableSize        jump table dependency table size
*
********************************************************************************** */
void Sota_InitDependencyTable (sotaDependencyTableElement_t dependencyTable[], uint32_t tableSize);

/*! *********************************************************************************
* \brief  This function is in charge of calling the ResetISR function of each slave blob
*
********************************************************************************** */
void Sota_ResetAllSlaveBlobs(BlobResetMode_t eResetMode);

/*! *********************************************************************************
* \brief  This function returns true if it is a known blob and fills the blobVersion
*         given as an arg. Otherwise it returns false
*
* \param[in]     blobId       blob Id targeted
* \param[in/out] blobVersion  the blobVersion if filled
*
* \return true or false
********************************************************************************** */
bool_t Sota_GetBlobVersion (uint16_t blobId, uint32_t *blobVersion);

/*! *********************************************************************************
* \brief  This function returns true if it is a known blob and fills the u32BlobSectionSize
*         given as an arg. Otherwise it returns false
*
* \param[in]     blobId              blob Id targeted
* \param[in/out] u32BlobSectionSize  the section size if filled
*
* \return true or false
********************************************************************************** */
bool_t Sota_GetBlobSectionSize (uint16_t blobId, uint32_t *u32BlobSectionSize);

/*! *********************************************************************************
* \brief  Returns the number of blobs composing the global application
*         The SSBL is not included
*
* \return the number of blob without the SSBL
********************************************************************************** */
uint8_t Sota_GetNumberOfBlobs ();

/*! *********************************************************************************
* \brief  Fills the blobId table with all blob ID composing the application including the
* 		  SSBL.
*         Returns the number of blob ID written
*
* \param[in/out] blobIdTab        list of blobId composing the application
* \param[in] 	 blobIdTabSize    max size of the table given as arg
*
* \return the number of blob ID writen in the tab
********************************************************************************** */
uint8_t Sota_GetAllBlobIds (uint16_t *blobIdTab, uint8_t blobIdTabSize);

/*! *********************************************************************************
* \brief  Returns the blob function to get the targeted blob Zigbee OTA header
*
* \param[in] 	 blobId    targeted blob ID
*
* \return function pointer
********************************************************************************** */
Sota_BlobUtilsFunction Sota_GetZbOtaHeaderFunction(uint16_t blobId);

/*! *********************************************************************************
* \brief  Check a compatibility list for a given blob ID
*         Returns False if the compatibility list does not match
*
* \param[in] blobId                         targeted blob ID
* \param[in] pCompatibilityListFirstElemt   pointer to the first element of the compatibility list
* \param[in] compatibilityListLen           number of element
*
* \return true or false
*
********************************************************************************** */
bool_t Sota_CheckCompatibilityList(uint16_t blobId, uint8_t *pCompatibilityListFirstElemt, uint32_t compatibilityListLen);

/*! *********************************************************************************
* \brief  Compare the current blob version with a notified blob version
*          - If the major versions are not equal => return false
*          - Otherwise minor versions are compared => return true only if the minor version of the notified blob
*            version is greater than the version of the current blob
*
* \param[in]     currentBlobVersion    current version of the blob Id targeted
* \param[in]     blobIdTabSize         notified blob version
*
* \return true or false
*
********************************************************************************** */
bool_t Sota_CompareBlobUpdateVersion(uint32_t currentBlobVersion, uint32_t notifiedBlobVersion);

/*! *********************************************************************************
* \brief  Set the RAM retention bitmap for all of the slave blobs
*
* \param[in]     eResetMode    Reset mode used when waking (cold, warm...)

*
********************************************************************************** */
void Sota_SetRamRetention(BlobResetMode_t eResetMode);

#endif /* _BLOB_UTILS_H_ */