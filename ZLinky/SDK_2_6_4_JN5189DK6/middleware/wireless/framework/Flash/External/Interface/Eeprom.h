/*! *********************************************************************************
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2017 NXP
* All rights reserved.
*
* \file
*
* This is the header file for the Eeprom driver.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef _EEPROM_H_
#define _EEPROM_H_

/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
********************************************************************************** */
#include "EmbeddedTypes.h"
#include "board.h"

#ifdef __cplusplus
    extern "C" {
#endif

/*! *********************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
********************************************************************************** */


/* List of the EEPROM devices used on each of the FSL development boards */
#define gEepromDevice_None_c          0
#define gEepromDevice_InternalFlash_c 1
#define gEepromDevice_AT45DB161E_c    2 /* TWR-KW2x  */
#define gEepromDevice_AT26DF081A_c    3 /* TWR-MEM   */
#define gEepromDevice_AT45DB021E_c    4 /* FRDM-KW40 */
#define gEepromDevice_AT45DB041E_c    5 /* FRDM-KW41 */
#define gEepromDevice_MX25R3235F_c    6 /* FRDM-K32W */
#define gEepromDevice_MX25R8035F_c    7 /* DK6-K32W061 */

/* Default EEPROM type used */
#ifndef gEepromType_d
#define gEepromType_d gEepromDevice_None_c
#endif

#ifndef gEepromParams_bufferedWrite_c
#define gEepromParams_bufferedWrite_c         0
#endif

/* Characteristics of the EEPROM device */
#if (gEepromType_d == gEepromDevice_AT26DF081A_c)
  #define gEepromParams_TotalSize_c           0x00100000U /* 1 MBytes */
  #define gEepromParams_SectorSize_c          (4*1024)    /* 4 KBytes */

#elif (gEepromType_d == gEepromDevice_AT45DB161E_c)
  #define gEepromParams_TotalSize_c           0x00200000U /* 2 MBytes */
  #define gEepromParams_SectorSize_c          (4*1024)    /* 4 KBytes */

#elif (gEepromType_d == gEepromDevice_AT45DB021E_c)
  #define gEepromParams_TotalSize_c           0x00040000U /* 256 KBytes */
  #define gEepromParams_SectorSize_c          (2*1024)    /* 2 KBytes */

#elif (gEepromType_d == gEepromDevice_AT45DB041E_c)
  #define gEepromParams_TotalSize_c           0x00080000U /* 512 KBytes */
  #define gEepromParams_SectorSize_c          (2*1024)    /* 2 KBytes */

#elif (gEepromType_d == gEepromDevice_MX25R3235F_c)
  #define gEepromParams_TotalSize_c           0x00400000U /* 4 MBytes */
  #define gEepromParams_SectorSize_c          (4*1024)    /* 4 KBytes */

#elif (gEepromType_d == gEepromDevice_MX25R8035F_c)
  #define gEepromParams_TotalSize_c           0x00100000U /* 1 MBytes */
  #define gEepromParams_SectorSize_c          (4*1024)    /* 4 KBytes */
  #define gEepromParams_StartOffset_c         (0UL)

#elif (gEepromType_d == gEepromDevice_InternalFlash_c)

#if defined(__CC_ARM)

  extern uint32_t Image$$INT_STORAGE$$Base[];
  extern uint32_t Image$$INT_STORAGE$$Length[];

  #define gEepromParams_StartOffset_c          ((uint32_t)Image$$INT_STORAGE$$Base)
  #define gEepromParams_TotalSize_c            ((uint32_t)Image$$INT_STORAGE$$Length)
  #define gEepromParams_SectorSize_c           ((uint32_t)FSL_FEATURE_FLASH_PAGE_SIZE_BYTES)

#else /* defined(__CC_ARM) */

  extern uint32_t INT_STORAGE_SIZE[];
  extern uint32_t INT_STORAGE_END[];
  extern uint32_t INT_STORAGE_SECTOR_SIZE[];
  #define gEepromParams_StartOffset_c          ((uint32_t)INT_STORAGE_END)
  #define gEepromParams_TotalSize_c            ((uint32_t)INT_STORAGE_SIZE)
  #define gEepromParams_SectorSize_c           ((uint32_t)INT_STORAGE_SECTOR_SIZE)

#endif /* defined(__CC_ARM) */

#else
  #define gEepromParams_TotalSize_c            0x00000U
  #define gEepromParams_SectorSize_c           0
#endif

#ifndef gEepromParams_WriteAlignment_c
#define gEepromParams_WriteAlignment_c         1 /* bytes */
#endif

#if (gEepromParams_WriteAlignment_c > 1)
#define gEepromAlignAddr_d(x) ((((x)+gEepromParams_WriteAlignment_c-1)/gEepromParams_WriteAlignment_c)*gEepromParams_WriteAlignment_c)
#else
#define gEepromAlignAddr_d(x) (x)
#endif

#if ( gEepromParams_bufferedWrite_c == 1)

#ifndef gEepromParams_CurrentOffset
#define gEepromParams_CurrentOffset           0
#endif

#ifndef gEePromParams_BufferSize_c
#define gEePromParams_BufferSize_c           512
#endif

#endif /*defined gEepromParams_bufferedWrite_c */
/*! *********************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
********************************************************************************** */
typedef enum
{
  ee_ok,
  ee_too_big,
  ee_not_aligned,
  ee_busy,
  ee_error
} ee_err_t;


/*! *********************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
********************************************************************************** */

/******************************************************************************
* NAME: EEPROM_Init
* DESCRIPTION: Initializes the EEPROM peripheral
* PARAMETERS: None
* RETURN: ee_ok - if the EEPROM is initialized successfully
*         ee_error - otherwise
******************************************************************************/
ee_err_t EEPROM_Init
(
  void /*IN: No Input Parameters*/
);

/******************************************************************************
* NAME: EEPROM_DeInit
* DESCRIPTION: De-Initializes the EEPROM peripheral
* PARAMETERS: None
* RETURN: ee_ok - if the EEPROM has been de-initialized successfully
*         ee_error - otherwise
******************************************************************************/
ee_err_t EEPROM_DeInit
(
  void /*IN: No Input Parameters*/
);

/******************************************************************************
* NAME: EEPROM_ReadData
* DESCRIPTION: Reads the specified number of bytes from EEPROM memory
* PARAMETERS: [IN] NoOfBytes - number of bytes to be read
*             [IN] Addr - EEPROM address to start reading from
*             [OUT] inbuf - a pointer to a memory location where the
*                           data read out from EEPROM will be stored
* RETURN: ee_ok - if the read operation completed successfully
*         ee_too_big - if the provided address is out of range
*         ee_error - otherwise
******************************************************************************/
ee_err_t EEPROM_ReadData
(
  uint16_t NoOfBytes,/* IN: No of bytes to read */
  uint32_t Addr,         /* IN: EEPROM address to start reading from */
  uint8_t  *inbuf         /* OUT:Pointer to read buffer */
);

/******************************************************************************
* NAME: EEPROM_WriteData
* DESCRIPTION: Writes the specified number of bytes to EEPROM memory
* PARAMETERS: [IN] NoOfBytes - number of bytes to be written.
*             [IN] Addr - EEPROM address to start writing at.
*             [IN] Outbuf - a pointer to a memory location where the
*                           data to be written is stored.
* RETURN: ee_ok - if the write operation completed successfully
*         ee_too_big - if the provided address is out of range
*         ee_error - if the write operation fails
******************************************************************************/
ee_err_t EEPROM_WriteData
(
  uint32_t  NoOfBytes,/* IN: No of bytes to write */
  uint32_t Addr,         /* IN: EEPROM address to start writing at. */
  uint8_t  *Outbuf     /* IN: Pointer to data to write to EEPROM  */
);

/*****************************************************************************
*  EEPROM_WritePendingData
*
*  Writes pending data buffer into EEPROM
*
*****************************************************************************/
void EEPROM_WritePendingData(void);

/******************************************************************************
* NAME: EEPROM_isBusy
* DESCRIPTION: This function tests if the EEPROM is busy
* PARAMETERS: None
* RETURN: TRUE if EEPROM is busy or FALSE if EEPROM is ready(idle)
******************************************************************************/
uint8_t EEPROM_isBusy
(
  void /*IN: No Input Parameters*/
);


/******************************************************************************
* NAME: EEPROM_EraseBlock
* DESCRIPTION:  This function erase a block of 4Kbytes/32kbytes or 64 kbytes of memory
* PARAMETERS: IN Addr: address of block to erase (must be at least a 4k multiple)
*             IN size: size of block (can be 4k, 32k or 64k)
* RETURN: ee_busy - the EEPROM is busy
*         ee_too_big - the specified address is out of range
*         ee_ok - the block erase operation completed successfully
******************************************************************************/
ee_err_t EEPROM_EraseBlock
(
  uint32_t Addr, /* IN: EEPROM address within the 4Kbyte block to be erased. */
  uint32_t size
);

/******************************************************************************
* NAME: EEPROM_EraseArea
* DESCRIPTION:  This function erases as many block/sectors as necessary to erase
*               the expected number of bytes
* PARAMETERS: IN,OUT *Addr: pointer on address of first block to erase, incremented
*                           as blocks get erased or skipped if already erased
*             IN,OUT *size: pointer on number of bytes to erase, decremented as blocks get erased
*             IN     non_blocking stop loop if EEPROM status found busy
* RETURN: ee_busy - the EEPROM is busy
*         ee_too_big - the specified address is out of range
*         ee_ok - the block erase operation completed successfully
******************************************************************************/
ee_err_t EEPROM_EraseArea
(
		uint32_t *Addr,
		int32_t *size,
		bool non_blocking
);

/******************************************************************************
* NAME: EEPROM_EraseArea
* DESCRIPTION:  This function erases the next block form the next block aligned address
*
* PARAMETERS: IN Addr: current address not necessarily block aligned
*             IN size: number of bytes to erase
* RETURN: ee_busy - the EEPROM is busy
*         ee_too_big - the specified address is out of range
*         ee_ok - the block erase operation completed successfully
******************************************************************************/
ee_err_t EEPROM_EraseNextBlock
(
    uint32_t Addr,
    uint32_t size
);


/******************************************************************************
* NAME: EEPROM_ChipErase
* DESCRIPTION:  This function erase the entire EEPROM memory
* PARAMETERS: None
* RETURN: ee_busy - the EEPROM is busy
*         ee_ok - the block erase operation completed successfully
******************************************************************************/
ee_err_t EEPROM_ChipErase
(
  void /*IN: No Input Parameters*/
);

/******************************************************************************
* NAME: EEPROM_TransactionQueuePurge
* DESCRIPTION:  This function clears the pending transactions.
* PARAMETERS: None
* RETURN: number of purged transactions
******************************************************************************/
int EEPROM_TransactionQueuePurge(void);

/******************************************************************************
* NAME: EEPROM_SectorAlignmentAfterReset
* DESCRIPTION:  This function restores the necesary bytes of a sector, and erase
*               bytes that are no more necessary. This function can be called if
 *              we want to go back in a sector and that gFlashEraseDuringWrite is
 *              not enabled.
* PARAMETERS: Addr
* RETURN: ee_ok - the operation completed successfully
 *        other error codes
******************************************************************************/
ee_err_t EEPROM_SectorAlignmentAfterReset(uint32_t Addr);

#if ( gEepromParams_bufferedWrite_c == 1)
/******************************************************************************
* NAME: EEPROM_FlashWriteBufferedTailEndBytes
* DESCRIPTION:  This function writes into flash anything which is pending from 
* unaligned buffered write.
* PARAMETERS: None
* RETURN: ee_busy - the EEPROM is busy         
*         ee_ok - the block erase operation completed successfully
******************************************************************************/
bool    EEPROM_FlashWriteBufferedTailEndBytes (uint32_t    Addr);
extern uint32_t mCurrentWriteOffset;
#endif

/******************************************************************************
* NAME: EEPROM_GetTotalSize
* DESCRIPTION:  This function clears the pending transactions.
* PARAMETERS: None
* RETURN: number of purged transactions
******************************************************************************/
static inline size_t EEPROM_GetTotalSize(void)
{
	return (size_t)gEepromParams_TotalSize_c;
}

#if defined gLoggingActive_d && (gLoggingActive_d > 0)
#include "dbg_logging.h"
#ifndef DBG_EEPROM
#define DBG_EEPROM 0
#endif
#define EEPROM_DBG_LOG(fmt, ...)   if (DBG_EEPROM) do { DbgLogAdd(__FUNCTION__ , fmt, VA_NUM_ARGS(__VA_ARGS__), ##__VA_ARGS__); } while (0);
#else
#define EEPROM_DBG_LOG(...)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _EEPROM_H_ */
