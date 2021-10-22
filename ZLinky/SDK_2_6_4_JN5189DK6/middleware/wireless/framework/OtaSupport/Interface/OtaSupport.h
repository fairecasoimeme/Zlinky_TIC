/*! *********************************************************************************
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2019 NXP
* All rights reserved.
*
* \file
*
* This is the header file for the OTA Programming Support.
*
** SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef _OTA_SUPPORT_H_
#define _OTA_SUPPORT_H_

#ifdef __cplusplus
    extern "C" {
#endif

#include "Eeprom.h"
/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/

#ifndef gEnableOTAServer_d
#define gEnableOTAServer_d                 (0)
#endif

#ifndef gUpgradeImageOnCurrentDevice_d
#define gUpgradeImageOnCurrentDevice_d     (0)
#endif

#define gOtaVersion_c                      (0x01)

/* Should app require to remove the read back #define gOtaVerifyWrite_d  in app_preinclude */
#ifndef gOtaVerifyWrite_d
#define gOtaVerifyWrite_d                  (1)
#endif

#define gBootValueForTRUE_c                (0x00)
#define gBootValueForFALSE_c               (0xFF)

#define gOtaEraseNone_c           0
#define gOtaEraseAtImageStart_c   1
#define gOtaEraseBeforeChunkReq_c 2

#if gOtaEepromPostedOperations_d
#define gOtaErasePolicy_c      gOtaEraseBeforeChunkReq_c
#else
#define gOtaErasePolicy_c gOtaEraseNone_c
#endif


#if !defined CPU_JN518X
#define gBootData_ImageLength_Size_c       (0x04)
#ifndef gBootData_SectorsBitmap_Size_c
#define gBootData_SectorsBitmap_Size_c     (32)
#endif
#else
#ifndef gBootData_None_c
#define gBootData_None_c                    1
#endif
#endif

#if gBootData_None_c
#define gBootData_Image_Offset_c           (0x00)
#else
#define gBootData_StartMarker_Value_c      0xDE, 0xAD, 0xAC, 0xE5
#define gBootData_StartMarker_Offset_c     (0x00)

#if (gEepromType_d == gEepromDevice_InternalFlash_c)
#define gBootData_Marker_Size_c            (FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE)
#else
#define gBootData_Marker_Size_c            (0x00)
#endif
#define gBootData_ImageLength_Offset_c     (gBootData_StartMarker_Offset_c + gBootData_Marker_Size_c)
#define gBootData_SectorsBitmap_Offset_c   gEepromAlignAddr_d(gBootData_ImageLength_Offset_c + \
                                                              gBootData_ImageLength_Size_c)
#define gBootData_Image_Offset_c           gEepromAlignAddr_d(gBootData_SectorsBitmap_Offset_c + \
                                                              gBootData_SectorsBitmap_Size_c)
#endif

#if defined  CPU_JN518X
  #define gFlashMaxStatedSize                (0x48000) /* Half of 0x90000 */
  #if  (gEepromType_d == gEepromDevice_InternalFlash_c)
  #define gFlashParams_MaxImageLength_c      (gFlashMaxStatedSize)
  #else
  #define gFlashParams_MaxImageLength_c      (FSL_FEATURE_FLASH_PFLASH_BLOCK_SIZE - 18*FSL_FEATURE_FLASH_PFLASH_BLOCK_SECTOR_SIZE)
  #endif
#else /* CPU_JN518X */
  /* The maximum amount of MCU Flash memory */
  #if defined(FSL_FEATURE_FLASH_HAS_MULTIPLE_FLASH) && (FSL_FEATURE_FLASH_HAS_MULTIPLE_FLASH)
  #define gFlashParams_MaxImageLength_c      ((FSL_FEATURE_FLASH_PFLASH_BLOCK_SIZE * FSL_FEATURE_FLASH_PFLASH_BLOCK_COUNT) + \
                                            (FSL_FEATURE_FLASH_PFLASH_1_BLOCK_SIZE * FSL_FEATURE_FLASH_PFLASH_1_BLOCK_COUNT))
#else
#define gFlashParams_MaxImageLength_c      (FSL_FEATURE_FLASH_PFLASH_BLOCK_SIZE * FSL_FEATURE_FLASH_PFLASH_BLOCK_COUNT)


    #if FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE == 4
    #define gBootFlagUnprogrammed_c 0xFF,0xFF,0xFF,0xFF
    #elif FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE == 8
    #define gBootFlagUnprogrammed_c 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
    #else
    #error The gBootFlagUnprogrammed_c macro is not defined for this FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE.
#endif
  #endif
#endif
#ifndef gInvalidateHeaderLength
#define gInvalidateHeaderLength          (336)
#endif


#ifndef gEepromParams_ResetBitmap
#define gEepromParams_ResetBitmap        0
#endif

#define DBG_LEVEL_NONE       0
#define DBG_LEVEL_WARNING    1
#define DBG_LEVEL_INFO       2
#define DBG_LEVEL_DEBUG      3
#define DBG_LEVEL_VERBOSE    4


#define OTA_DBG_LVL    DBG_LEVEL_WARNING

#if (OTA_DBG_LVL >= DBG_LEVEL_VERBOSE)
#define OTA_VERBOSE_TRACE  PRINTF
#else
#define OTA_VERBOSE_TRACE(...)
#endif
#if (OTA_DBG_LVL >= DBG_LEVEL_DEBUG)
#define OTA_DEBUG_TRACE  PRINTF
#else
#define OTA_DEBUG_TRACE(...)
#endif

#if (OTA_DBG_LVL >= DBG_LEVEL_INFO)
#define OTA_INFO_TRACE  PRINTF
#else
#define OTA_INFO_TRACE(...)
#endif

#if (OTA_DBG_LVL >= DBG_LEVEL_WARNING)
#define OTA_WARNING_TRACE  PRINTF
#else
#define OTA_WARNING_TRACE(...)
#endif


/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
typedef enum {
  gOtaSuccess_c = 0,
  gOtaNoImage_c,
  gOtaUpdated_c,
  gOtaError_c,
  gOtaCrcError_c,
  gOtaInvalidParam_c,
  gOtaInvalidOperation_c,
  gOtaExternalFlashError_c,
  gOtaInternalFlashError_c,
  gOtaImageTooLarge_c
} otaResult_t;


/* OTA server - operating modes */
typedef enum {
  gUpgradeImageOnCurrentDevice_c = 0,
  gUseExternalMemoryForOtaUpdate_c,
  gDoNotUseExternalMemoryForOtaUpdate_c,
  gOtaTestingProcess
}otaMode_t;

typedef enum{
    gOtaImageAuthFail_c,
    gOtaImageAuthPass_c,
}otaImageAuthResult_t;

typedef otaResult_t (*pfOTA_Callback_t)(uint8_t* pBuffer, uint16_t len);

typedef struct otaServer_AppCB_tag{
    pfOTA_Callback_t otaServerImgNotifyCnf;
    pfOTA_Callback_t otaServerSetModeCnf;
    pfOTA_Callback_t otaServerQueryImageCnf;
    pfOTA_Callback_t otaServerPushChunkCnf;
    pfOTA_Callback_t otaServerCancelImgCnf;
    pfOTA_Callback_t otaServerAbortProcessCnf;
    pfOTA_Callback_t otaServerSetFileVersPoliciesCnf;
    pfOTA_Callback_t otaServerClientInfoCnf;
}otaServer_AppCB_t;

/*! Structure containing the boot flags. Likely to be perfectly packed anyway; GCC
    would warn about this so add pragma to repress warning */
#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpacked"
#endif
typedef PACKED_STRUCT
{
    uint8_t  newBootImageAvailable[FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE];
    uint8_t  bootProcessCompleted[FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE];
    uint8_t  version[FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE];
    uint8_t  internalStorageAddr [FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE];

}bootInfo_t;
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

typedef void (*ota_op_completion_cb_t)(uint32_t);

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
extern uint16_t gBootFlagsSectorBitNo;
/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief  Registers the OTA component to FSCI
*
* \param[in] fsciInterface  The Id of the FSCI interface used by the OTAP Server
* \param[in] pCB            Pointer to a table of callback function.
*
* \return  error code.
*
********************************************************************************** */
otaResult_t OTA_RegisterToFsci(uint32_t fsciInterface, otaServer_AppCB_t *pCB);

/*! *********************************************************************************
* \brief  Starts the process of writing a new image to the external EEPROM.
*
* \param[in] length: the length of the image to be written in the EEPROM
*
* \return
*  - gOtaInvalidParam_c: the intended length is bigger than the FLASH capacity
*  - gOtaInvalidOperation_c: the process is already started (can be cancelled)
*  - gOtaEepromError_c: can not detect external EEPROM
*
********************************************************************************** */
otaResult_t OTA_StartImage(uint32_t length);

/************************************************************************************
* \brief  Starts the process of writing a new image to the external EEPROM.
*
* \param[in] length: the length of the image to be written in the EEPROM
* \param[in] maxAllowedArchSize: the size of the section where the image will be written
*
* \return
*  - gOtaInvalidParam_c: the intended length is bigger than the FLASH capacity
*  - gOtaInvalidParam_c: the intended length is bigger than the FLASH capacity
*  - gOtaInvalidOperation_c: the process is already started (can be cancelled)
*  - gOtaEepromError_c: can not detect external EEPROM
************************************************************************************/
otaResult_t OTA_StartImageWithMaxSize(uint32_t length, uint32_t maxAllowedArchSize);

/*! *********************************************************************************
* \brief  Places the next image chunk into the external FLASH. The CRC will not be computed.
*
* \param[in] pData          pointer to the data chunk
* \param[in] length         the length of the data chunk
* \param[in] pImageLength   if it is not null and the function call is successful,
*                           it will be filled with the current length of the image
* \param[in] pImageOffset   if it is not null contains the current offset of the image
*
* \return
*  - gOtaInvalidParam_c: pData is NULL or the resulting image would be bigger than the
*       final image length specified with OTA_StartImage()
*  - gOtaInvalidOperation_c: the process is not started
*
********************************************************************************** */
otaResult_t OTA_PushImageChunk(uint8_t* pData, uint16_t length, uint32_t* pImageLength, uint32_t *pImageOffset);

/*! *********************************************************************************
* \brief  Blocking call to OTA_PushImageChunk.
*         This function will first call OTA_MakeHeadRoomForNextBlock
*         to erase a block before writting pData in flash
*
* \param[in] pData          pointer to the data chunk
* \param[in] length         the length of the data chunk
* \param[in] pImageLength   if it is not null and the function call is successful,
*                           it will be filled with the current length of the image
* \param[in] pImageOffset   if it is not null contains the current offset of the image
*
* \return
*  - gOtaInvalidParam_c: pData is NULL or the resulting image would be bigger than the
*       final image length specified with OTA_StartImage()
*  - gOtaInvalidOperation_c: the process is not started
*
********************************************************************************** */
otaResult_t OTA_PushImageChunkBlocking(uint8_t* pData, uint16_t length, uint32_t* pImageLength, uint32_t *pImageOffset);

/*! *********************************************************************************
* \brief  Finishes the writing of a new image to the permanent storage.
*  It will write the image header (signature and length) and footer (sector copy bitmap).
*
* \param[in] bitmap   pointer to a  byte array indicating the sector erase pattern for the
*       internal FLASH before the image update.
*
* \return
*  - gOtaInvalidOperation_c: the process is not started,
*  - gOtaEepromError_c: error while trying to write the EEPROM
*
********************************************************************************** */
otaResult_t OTA_CommitImage(uint8_t* pBitmap);

/*! *********************************************************************************
* \brief  Cancels the process of writing a new image to the external EEPROM.
*
********************************************************************************** */
void OTA_CancelImage(void);

/*! *********************************************************************************
* \brief  Resets the Current Eeprom Address to its initial value
*
********************************************************************************** */
void OTA_ResetCurrentEepromAddress(void);

/*! *********************************************************************************
* \brief  After reception of OTA image set image flag
*
* Does something different for CPU_JN518X
* *
************************************************************************************ */
void OTA_SetNewImageFlag(void);

/*! *********************************************************************************
* \brief  Initializes the image storage (external memory or internal flash)
*
* \return  error code.
*
********************************************************************************** */
otaResult_t OTA_InitExternalMemory(void);

/*! *********************************************************************************
* \brief  De-initializes the image storage (external memory or internal flash)
*
* \return  error code.
*
********************************************************************************** */
otaResult_t OTA_DeInitExternalMemory(void);

/*! *********************************************************************************
* \brief  Read from the image storage (external memory or internal flash)
*
* \param[in] pData    pointer to the data chunk
* \param[in] length   the length of the data chunk
* \param[in] address  image storage address
*
* \return  error code.
*
********************************************************************************** */
otaResult_t OTA_ReadExternalMemory(uint8_t* pData, uint16_t length, uint32_t address);

/*! *********************************************************************************
* \brief  Write into the image storage (external memory or internal flash)
*
* \param[in] pData    pointer to the data chunk
* \param[in] length   the length of the data chunk
* \param[in] address  image storage address
*
* \return  error code.
*
********************************************************************************** */
otaResult_t OTA_WriteExternalMemory(uint8_t* pData, uint16_t length, uint32_t address);

/*! *********************************************************************************
* \brief  Compute CRC over a data chunk.
*
* \param[in] pData        pointer to the data chunk
* \param[in] length       the length of the data chunk
* \param[in] crcValueOld  current CRC value
*
* \return  computed CRC.
*
********************************************************************************** */
uint16_t OTA_CrcCompute(uint8_t *pData, uint16_t lenData, uint16_t crcValueOld);

/*! *********************************************************************************
* \brief  This function is called in order to erase the image storage
*         (external memory or internal flash)
*
* \return  computed CRC.
*
********************************************************************************** */
otaResult_t OTA_EraseExternalMemory(void);

/*! *********************************************************************************
* \brief  This function is called in order to erase a block of memory (flash sector)
*
* \return  error code.
*
********************************************************************************** */
otaResult_t OTA_EraseBlock(uint32_t address);

/*! *********************************************************************************
* \brief  This function sends a request for a new image chunk over the serial interface
*
* \param[in] offset  image offset
* \param[in] len     requested chunk length
* \param[in] devId   Id of the device
*
********************************************************************************** */
void OTA_ImageChunkReq(uint32_t offset, uint16_t len, uint16_t devId);

/*! *********************************************************************************
* \brief  This function sends over the serial interface a query for a specific image
*
* \param[in] devId          Id of the device
* \param[in] manufacturer   The manufacturer code
* \param[in] imgType        Type of the image
* \param[in] fileVersion    Minimum version of the image
*
********************************************************************************** */
void OTA_QueryImageReq(uint16_t devId, uint16_t manufacturer, uint16_t imgType, uint32_t fileVersion);

/*! *********************************************************************************
* \brief
*
* \param[in] pClientAddr    Pointer to the client address (16 bytes)
* \param[in] offset
* \param[in] devId          Id of the device
*
********************************************************************************** */
void OTA_ClientInfoCnf(uint8_t* pClientAddr, uint32_t offset, uint16_t devId);

/*! *********************************************************************************
* \brief        Image validation + authenfication if a root certificate is found
 *              and authentication level > 0
*
*  Return:
                 Authenticate pass or not, gOtaImageAuthPass_c/gOtaImageAuthFail_c
********************************************************************************** */
otaImageAuthResult_t OTA_ImageAuthenticate();

/*! *********************************************************************************

* \brief        Erase enough flash in order ot store next block erasing blocks or sectors
*               Recommendation: Always call this function with the same "size" arg value
*               to improve erase efficiency.
* \param[in]        size number of bytes that need to be erased
* \param[in]        cb callback function to be called on completion
* \param[in]        param parameter callback function to be called on completion
*
*
* \return           Erase success or not.
********************************************************************************** */
otaResult_t OTA_MakeHeadRoomForNextBlock(uint32_t size, ota_op_completion_cb_t cb, uint32_t param);

/*****************************************************************************
*  OTA_PullImageChunk
*
*  Read image chunk from external memory
*****************************************************************************/
otaResult_t OTA_PullImageChunk(uint8_t* pData, uint16_t length, uint32_t *pImageOffset);

#ifdef gEepromParams_bufferedWrite_c
/*****************************************************************************
*  OTA_InvalidateImageHeader
*
*  Validate shadow flash header
*****************************************************************************/
otaResult_t OTA_InvalidateImageHeader( bool_t bCurrentImage, uint32_t Offset );
/*****************************************************************************
*  OTA_SetInvalidateFlag
*
*  Sets Internal flag to automatically validate bytes on read without changing
*  them in flash.
*****************************************************************************/
void OTA_SetInvalidateFlag ( bool_t bInvalidateFlag );
#endif

/*****************************************************************************
*  OTA_ClientInit
*
*  Sets OTA Client variables to their initial state.
*
*****************************************************************************/
otaResult_t OTA_ClientInit(void);

/*****************************************************************************
*  OTA_TransactionResume
*
*  Called each time the platform gets 'idle' is order to restart flashing pending OTA
*  blocks to the flash.
*
*****************************************************************************/
int OTA_TransactionResume(void);
/*****************************************************************************
*  OTA_AlignOnReset
*
*  Called each time on wake, reset or abort of OTA to align internal parameters
*  
*
*****************************************************************************/
void OTA_AlignOnReset(void);
#if defined gLoggingActive_d && (gLoggingActive_d > 0)
#include "dbg_logging.h"
#ifndef DBG_OTA
#define DBG_OTA 0
#endif
#define OTA_DBG_LOG(fmt, ...)   if (DBG_OTA) do { DbgLogAdd(__FUNCTION__ , fmt, VA_NUM_ARGS(__VA_ARGS__), ##__VA_ARGS__); } while (0);
#else
#define OTA_DBG_LOG(...)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _OTA_SUPPORT_H_ */
