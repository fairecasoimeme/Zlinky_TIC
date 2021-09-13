/*! *********************************************************************************
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2019 NXP
* All rights reserved.
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "EmbeddedTypes.h"
#include "assert.h"
#include "stddef.h"
#include "flash_header.h"
#include "rom_psector.h"
#include "blob_utils.h"
#include "FunctionLib.h"
#include "PWR_Interface.h"

#ifdef SOTA_DEBUG_FWK_ENABLED
#include "fsl_debug_console.h"
#endif

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
#define SOTA_CALL_RESET_ISR(blobAddr,eResetMode) ((void (*)(BlobResetMode_t))(*(uint32_t *)((blobAddr) + 4)))(eResetMode);
#define SOTA_CALL_DATA_USAGE(blobAddr,eResetMode,prCallback) ((void (*)(BlobResetMode_t,void (*)(uint32_t,uint32_t)))(*(uint32_t *)((blobAddr) + 8)))(eResetMode,prCallback);

#define SOTA_SSBL_BASE_ADDR    0x0
#define SOTA_BLOB_ADDR_UNKNOWN 0xff
#define SOTA_EMPTY_BLOB_SIZE   0

#define SOTA_SSBL_SECTION_SIZE 8192

#define SOTA_SSBL_POSITION_0 0

#define SOTA_JUMP_TABLE_INDEX (13)
#define SOTA_JUMP_TABLE_OFFSET (SOTA_JUMP_TABLE_INDEX * sizeof(uint32_t))

#ifdef SOTA_DEBUG_FWK_ENABLED
#define SOTA_FWK_PRINTF PRINTF
#else
#define SOTA_FWK_PRINTF(...)
#endif

#define SOTA_GET_MAJOR_VERSION(blobVersion) (blobVersion >> 16)
#define SOTA_GET_MINOR_VERSION(blobVersion) (blobVersion & 0x0000FFFF)

/************************************************************************************
*************************************************************************************
* Private types
*************************************************************************************
************************************************************************************/

typedef struct {
    uint32_t blob_position_in_flash;
	uint32_t blob_start_address;
	uint32_t blob_size;
    Sota_BlobUtilsFunction blobFunctionGetZgOtaHeader;
} blob_desc_t;


/************************************************************************************
*************************************************************************************
* Private variables
*********************************************************************************/

static blob_desc_t blob_desc[] = {
    {SOTA_SSBL_POSITION_0, SOTA_SSBL_BASE_ADDR, SOTA_SSBL_SECTION_SIZE, NULL},
    {SOTA_BLOB_POSITION_1, SOTA_BLOB_ADDR_UNKNOWN, SOTA_EMPTY_BLOB_SIZE, NULL},
    {SOTA_BLOB_POSITION_2, SOTA_BLOB_ADDR_UNKNOWN, SOTA_EMPTY_BLOB_SIZE, NULL},
    {SOTA_BLOB_POSITION_3, SOTA_BLOB_ADDR_UNKNOWN, SOTA_EMPTY_BLOB_SIZE, NULL},
    {SOTA_BLOB_POSITION_4, SOTA_BLOB_ADDR_UNKNOWN, SOTA_EMPTY_BLOB_SIZE, NULL},
    {SOTA_BLOB_POSITION_5, SOTA_BLOB_ADDR_UNKNOWN, SOTA_EMPTY_BLOB_SIZE, NULL},
    {SOTA_BLOB_POSITION_6, SOTA_BLOB_ADDR_UNKNOWN, SOTA_EMPTY_BLOB_SIZE, NULL},
    {SOTA_BLOB_POSITION_7, SOTA_BLOB_ADDR_UNKNOWN, SOTA_EMPTY_BLOB_SIZE, NULL},
    {SOTA_BLOB_POSITION_8, SOTA_BLOB_ADDR_UNKNOWN, SOTA_EMPTY_BLOB_SIZE, NULL}
};

static bool_t blobUtilsInitialized = FALSE;

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
static blob_desc_t * Sota_GetBlobDesc(uint32_t blobFlashPosition)
{
    uint8_t i;
    for (i=0; i< (sizeof(blob_desc)/sizeof(blob_desc_t)); i++)
    {
        if (blobFlashPosition == blob_desc[i].blob_position_in_flash
            && blobFlashPosition != SOTA_SSBL_POSITION_0)
        {
            return &blob_desc[i];
        }
    }
    return NULL;
}

static uint16_t Sota_ExtractBlobId(uint32_t blob_start_address)
{
    uint32_t u32BootBlockOffset;
    IMG_HEADER_T *psImgHdr;
    BOOT_BLOCK_T *psBootBlock;

    psImgHdr = (IMG_HEADER_T *)blob_start_address;
    u32BootBlockOffset = psImgHdr->bootBlockOffset;
    psBootBlock = (BOOT_BLOCK_T *)(u32BootBlockOffset + blob_start_address);

    return (uint16_t) psBootBlock->img_type;
}

static blob_desc_t * Sota_GetBlobDescGivenABlobId(uint16_t blobId)
{
    uint8_t i;
    uint16_t blobIdFound;
    blob_desc_t * current_blob;

    for (i=0; i< (sizeof(blob_desc)/sizeof(blob_desc_t)); i++)
    {
        current_blob = &blob_desc[i];
        if (current_blob != NULL && current_blob->blob_size != SOTA_EMPTY_BLOB_SIZE)
        {
            /* Get the blob ID from the boot block */
            blobIdFound = Sota_ExtractBlobId(current_blob->blob_start_address);

            if (blobId == blobIdFound)
                return &blob_desc[i];
        }
    }
    return NULL;
}

static uint32_t * Sota_FindJumpTable(uint32_t blobFlashPosition, Sota_BlobUtilsFunction blobFunctionGetZgOtaHeader)
{
    uint32_t *pu32BlobJumpTable = NULL;

#ifndef NDEBUG
    uint32_t u32BlobTargetAddress;
    uint32_t u32BootBlockOffset;
    IMG_HEADER_T *psImgHdr;
    BOOT_BLOCK_T *psBootBlock;
#endif
    blob_desc_t * current_blob;
    current_blob = Sota_GetBlobDesc(blobFlashPosition);
    if (current_blob != NULL )
    {
        uint32_t u32BlobBaseAddress;
        u32BlobBaseAddress = current_blob->blob_start_address;
#ifndef NDEBUG
        /* u32BlobTargetAddress must be equal to u32BlobBaseAddress */
        psImgHdr = (IMG_HEADER_T *)u32BlobBaseAddress;
        u32BootBlockOffset = psImgHdr->bootBlockOffset;
        psBootBlock = (BOOT_BLOCK_T *)(u32BootBlockOffset + u32BlobBaseAddress);
        u32BlobTargetAddress = psBootBlock->target_addr;
        assert(u32BlobTargetAddress == u32BlobBaseAddress);
#endif

        pu32BlobJumpTable = (uint32_t *) *((uint32_t *) (u32BlobBaseAddress + SOTA_JUMP_TABLE_OFFSET));
        current_blob->blobFunctionGetZgOtaHeader = blobFunctionGetZgOtaHeader;
    }

    return pu32BlobJumpTable;
}

static void Sota_InitBlobDescTab()
{
    uint8_t i;
    /* Read in the psector the image directory and fill the local tab with blob info */
    psector_page_data_t mPage0;
    psector_page_data_t *pPage0 = &mPage0;
    psector_page_state_t page_state;
    do
    {
        page_state = psector_ReadData(PSECTOR_PAGE0_PART, 0, 0, sizeof(psector_page_t), pPage0);
        if (page_state < PAGE_STATE_DEGRADED)
        {
            break;
        }
        else
        {
            blob_desc_t * currentBlob = NULL;
            image_directory_entry_t * currentEntry = pPage0->page0_v3.img_directory;
            for (i=0; i<IMG_DIRECTORY_MAX_SIZE; i++)
            {
                currentBlob = Sota_GetBlobDesc(currentEntry->img_type);
                if (currentBlob != NULL)
                {
                    currentBlob->blob_start_address = currentEntry->img_base_addr;
                    currentBlob->blob_size = currentEntry->img_nb_pages*512;
                }
                currentEntry++;
            }
        }

    } while (0);
}

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief  Initializes the dependencyTable with the address of the jumptable
*
* \param[in/out] dependencyTable  jump table dependency
* \param[in]     tableSize        jump table dependency table size
*
********************************************************************************** */
void Sota_InitDependencyTable (sotaDependencyTableElement_t dependencyTable[], uint32_t tableSize)
{
    uint32_t i;
    sotaDependencyTableElement_t *pDependencyTableElement;

    if (!blobUtilsInitialized)
    {
        Sota_InitBlobDescTab();
        blobUtilsInitialized = TRUE;
    }

    for (i = 0; i < tableSize; i++)
    {
        pDependencyTableElement = &dependencyTable[i];
        *pDependencyTableElement->pu32BlobJumpTable = Sota_FindJumpTable(pDependencyTableElement->blobFlashPosition, pDependencyTableElement->blobFunctionGetZgOtaHeader);
    }
}

/*! *********************************************************************************
* \brief  This function is in charge of calling the ResetISR function of each slave blob
*
********************************************************************************** */
void Sota_ResetAllSlaveBlobs(BlobResetMode_t eResetMode)
{
    uint8_t i;
    /* Reset each "slave" blob except the blob0 as it is the blob manager */
    blob_desc_t * current_blob;
    for (i=1; i< (sizeof(blob_desc)/sizeof(blob_desc_t)); i++)
    {
        current_blob = &blob_desc[i];
        if (current_blob != NULL
            && current_blob->blob_position_in_flash != SOTA_SSBL_POSITION_0
            && current_blob->blob_position_in_flash != SOTA_BLOB_POSITION_1
            && current_blob->blob_size != SOTA_EMPTY_BLOB_SIZE)
        {
            SOTA_CALL_RESET_ISR(current_blob->blob_start_address, eResetMode);
        }
    }
}

/*! *********************************************************************************
* \brief  This function returns true if it is a known blob and fills the blobVersion
*         given as an arg. Otherwise it returns false
*
* \param[in]     blobId       blob Id targeted
* \param[in/out] blobVersion  the blobVersion if filled
*
* \return true or false
********************************************************************************** */
bool_t Sota_GetBlobVersion (uint16_t blobId, uint32_t *blobVersion)
{
	bool_t ret = FALSE;
    uint32_t u32BootBlockOffset;
    IMG_HEADER_T *psImgHdr;
    BOOT_BLOCK_T *psBootBlock;

    blob_desc_t * current_blob;
    current_blob = Sota_GetBlobDescGivenABlobId(blobId);
    if (current_blob != NULL)
    {
    	/* u32BlobTargetAddress must be equal to u32BlobBaseAddress */
    	psImgHdr = (IMG_HEADER_T *)current_blob->blob_start_address;
    	u32BootBlockOffset = psImgHdr->bootBlockOffset;
    	psBootBlock = (BOOT_BLOCK_T *)(u32BootBlockOffset + current_blob->blob_start_address);

    	*blobVersion = psBootBlock->version;
    	ret = TRUE;
    }
    return ret;
}

/*! *********************************************************************************
* \brief  This function returns true if it is a known blob and fills the u32BlobSectionSize
*         given as an arg. Otherwise it returns false
*
* \param[in]     blobId              blob Id targeted
* \param[in/out] u32BlobSectionSize  the section size if filled
*
* \return true or false
********************************************************************************** */
bool_t Sota_GetBlobSectionSize (uint16_t blobId, uint32_t *u32BlobSectionSize)
{
    bool_t status;
    blob_desc_t * current_blob;
    current_blob = Sota_GetBlobDescGivenABlobId(blobId);

    if (current_blob != NULL)
    {
    	*u32BlobSectionSize = current_blob->blob_size;
        status = TRUE;
    }
    else
    {
    	*u32BlobSectionSize = 0;
        status = FALSE;
    }

    return status;
}

/*! *********************************************************************************
* \brief  Returns the number of blobs composing the global application
*         The SSBL is not included
*
* \return the number of blob without the SSBL
********************************************************************************** */
uint8_t Sota_GetNumberOfBlobs ()
{
    uint8_t numberOfBlob = 0;
    uint8_t i;
    blob_desc_t * current_blob;
    for (i=0 ; i< (sizeof(blob_desc)/sizeof(blob_desc_t)); i++)
    {
        current_blob = &blob_desc[i];
        if (current_blob != NULL
            && current_blob->blob_position_in_flash != SOTA_SSBL_POSITION_0
            && current_blob->blob_size != SOTA_EMPTY_BLOB_SIZE)
        {
            numberOfBlob++;
        }
    }
    return numberOfBlob;
}

/*! *********************************************************************************
* \brief  Fills the blobId table with all blob ID composing the application including the
*         SSBL.
*         Returns the number of blob ID written
*
* \param[in/out] blobIdTab        list of blobId composing the application
* \param[in]     blobIdTabSize    max size of the table given as arg
*
* \return the number of blob ID writen in the tab
********************************************************************************** */
uint8_t Sota_GetAllBlobIds (uint16_t *blobIdTab, uint8_t blobIdTabSize)
{
    uint8_t numberOfBlob = 0;
    uint8_t i;
    uint8_t j = 0;
    blob_desc_t * current_blob;
    for (i=0 ; i< (sizeof(blob_desc)/sizeof(blob_desc_t)); i++)
    {
        current_blob = &blob_desc[i];
        if (current_blob->blob_size != SOTA_EMPTY_BLOB_SIZE)
        {
            blobIdTab[j++] = Sota_ExtractBlobId(current_blob->blob_start_address);
            numberOfBlob++;
        }
    }
    return numberOfBlob;
}

/*! *********************************************************************************
* \brief  Returns the blob function to get the targeted blob Zigbee OTA header
*
* \param[in]     blobId    targeted blob ID
*
* \return function pointer
********************************************************************************** */
Sota_BlobUtilsFunction Sota_GetZbOtaHeaderFunction(uint16_t blobId)
{
    Sota_BlobUtilsFunction function = NULL;
    blob_desc_t * current_blob = NULL;

    current_blob = Sota_GetBlobDescGivenABlobId(blobId);


    if (current_blob != NULL)
    {
        function = current_blob->blobFunctionGetZgOtaHeader;
    }

    return function;
}

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
bool_t Sota_CheckCompatibilityList(uint16_t blobId, uint8_t *pCompatibilityListFirstElemt, uint32_t compatibilityListLen)
{
        sotaCompatibilityListElem_t compatibilityListElem;
        uint8_t *pIterator = pCompatibilityListFirstElemt;
        uint32_t blob_version;
        bool_t checkerStatus = FALSE;
        SOTA_FWK_PRINTF("Handle compatibility list (%d) - length: %d\r\n",__LINE__,compatibilityListLen);

        for (int i = 0; i < compatibilityListLen; i++)
        {
            FLib_MemCpy((uint8_t *) &compatibilityListElem.blobId,pIterator, sizeof(compatibilityListElem.blobId));
            pIterator += sizeof(uint16_t);
            FLib_MemCpy((uint8_t *) &compatibilityListElem.blobVersion, pIterator, sizeof(compatibilityListElem.blobVersion));
            pIterator += sizeof(uint32_t);

            SOTA_FWK_PRINTF("Id: %.2x, Version: %.4x\r\n",
                    compatibilityListElem.blobId,
                    compatibilityListElem.blobVersion);

            if (compatibilityListElem.blobId == blobId)
            {
                break;
            }
            if (FALSE == Sota_GetBlobVersion(compatibilityListElem.blobId, &blob_version))
            {
                SOTA_FWK_PRINTF("Unknown blob - Id: %.2x, Version: %.4x \r\n",
                        compatibilityListElem.blobId,
                        compatibilityListElem.blobVersion);
                break;
            }
            if (blob_version < compatibilityListElem.blobVersion)
            {
                SOTA_FWK_PRINTF("Blob must be upgraded first - ID %.2x, Version: %.4x / current_blob version: %.4x\r\n",
                        compatibilityListElem.blobId,
                        compatibilityListElem.blobVersion,
                        blob_version);
                break;
            }

            if ((i+1) == compatibilityListLen)
            {
                checkerStatus = TRUE;
            }
        }
        return checkerStatus;
}

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
bool_t Sota_CompareBlobUpdateVersion(uint32_t currentBlobVersion, uint32_t notifiedBlobVersion)
{
    return (SOTA_GET_MAJOR_VERSION(currentBlobVersion) == SOTA_GET_MAJOR_VERSION(notifiedBlobVersion)
        && SOTA_GET_MINOR_VERSION(notifiedBlobVersion) > SOTA_GET_MINOR_VERSION(currentBlobVersion));

}

/*! *********************************************************************************
* \brief  This function is called in order to prepare the RAM retention for all of
*         the slave blobs (SSBL is not saved, and master blob is configured 
*         automatically by the LowPower module)
*
********************************************************************************** */
void Sota_SetRamRetention(BlobResetMode_t eResetMode)
{
    uint8_t i;
    
    /* Initialise forced RAM retention in LowPower module*/
    PWR_vForceRamRetention(0);

    /* Get  each "slave" blob */
    blob_desc_t * current_blob;
    for (i= 0 ; i < (sizeof(blob_desc)/sizeof(blob_desc_t)); i++)
    {
        /* Select each valid slave blob */
        current_blob = &blob_desc[i];
        if (   (current_blob != NULL)
            && (current_blob->blob_position_in_flash != SOTA_SSBL_POSITION_0)
            && (current_blob->blob_position_in_flash != SOTA_BLOB_POSITION_1)
            && (current_blob->blob_size != SOTA_EMPTY_BLOB_SIZE))
        {
            /* Obtain RAM size to be retained, with blob calling LowPower function
             * directly to update the forced RAM retention banks */
            SOTA_CALL_DATA_USAGE(current_blob->blob_start_address,
                                 eResetMode,
                                 PWR_vAddRamRetention);
        }
    }
}
