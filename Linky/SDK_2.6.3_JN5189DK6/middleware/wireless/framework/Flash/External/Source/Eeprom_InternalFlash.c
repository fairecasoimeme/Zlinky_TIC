/*! *********************************************************************************
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2017 NXP
* All rights reserved.
*
* \file
*
* This is the Source file for the EEPROM emulated inside the MCU's FLASH
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#include "Eeprom.h"

#if gEepromType_d == gEepromDevice_InternalFlash_c

#include "Flash_Adapter.h"
#include "FunctionLib.h"
#include "fsl_debug_console.h"
/******************************************************************************
*******************************************************************************
* Private Macros
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private Prototypes
*******************************************************************************
******************************************************************************/
#if ( gEepromParams_bufferedWrite_c == 0)
#ifndef CPU_JN518X
#if (gFlashEraseDuringWrite == 0)
static ee_err_t EEPROM_PrepareForWrite(uint32_t NoOfBytes, uint32_t Addr);
#endif
#endif
#else
static ee_err_t EEPROM_FixWriteDataInFlash( uint16_t      PageIndex);
#endif
/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private Memory Declarations
*******************************************************************************
******************************************************************************/
#if ( gEepromParams_bufferedWrite_c == 0)
#ifndef CPU_JN518X
static uint8_t mEepromEraseBitmap[32];
#endif
#if (gEepromParams_WriteAlignment_c > 1)
static uint8_t  mWriteBuff[gEepromParams_WriteAlignment_c];
static uint8_t  mWriteBuffLevel=0;
static uint32_t mWriteBuffAddr=0;
#endif
#else
/* This supports only continous writes.
 */
 /* variable keeps track of how much is currently in the buffer */
static uint32_t    mLocalIndex    = 0;
/* buffer to hold data for fixed size writes */
       uint8_t     gEePromStandaloneBuffer [gEePromParams_BufferSize_c];
/* determines how many pages have been written to */
static uint16_t    mEepromPageIndex  = 0;
/* keeps track of how far the flash has been written to only valid for continous sequential writes */
static uint32_t    mWriteBuffAddr = 0;
       uint32_t    mCurrentWriteOffset = 0;

#endif

/******************************************************************************
*******************************************************************************
* Public Memory
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************
******************************************************************************/

/*****************************************************************************
*  EEPROM_Init
*
*  Initializes the EEPROM peripheral
*
*****************************************************************************/
ee_err_t EEPROM_Init(void)
{
#if ( gEepromParams_bufferedWrite_c == 0)
#ifndef CPU_JN518X
    FLib_MemSet (mEepromEraseBitmap, 0x00, 32);
#endif
    NV_Init();
#else
    FLib_MemSet (gEePromStandaloneBuffer, 0xFF, gEePromParams_BufferSize_c);
    mLocalIndex = 0;
    mEepromPageIndex = 0;
	mWriteBuffAddr = gEepromParams_StartOffset_c;
	mCurrentWriteOffset = gEepromParams_StartOffset_c;
#endif
    return ee_ok;
}

/******************************************************************************
* NAME: EEPROM_DeInit
* DESCRIPTION: De-Initializes the EEPROM peripheral
* PARAMETERS: None
* RETURN: ee_ok - if the EEPROM has been de-initialized successfully
*         ee_error - otherwise
*******************************************************************************/
ee_err_t EEPROM_DeInit(void)
{
    /* Empty for now */
    return ee_ok;
}

#if ( gEepromParams_bufferedWrite_c == 0)
/*****************************************************************************
*  EEPROM_ChipErase
*
*  Erase all memory to 0xFF
*
*****************************************************************************/
ee_err_t EEPROM_ChipErase(void)
{
    uint32_t i, endAddr;
    
    endAddr = gEepromParams_StartOffset_c + gEepromParams_TotalSize_c;

    for( i=gEepromParams_StartOffset_c; i<endAddr; i+=gEepromParams_SectorSize_c )
    {
        if( ee_ok != EEPROM_EraseBlock(i, gEepromParams_SectorSize_c) )
        {
            return ee_error;
        }
    }

    return ee_ok;
}
#endif

/*****************************************************************************
*  EEPROM_EraseSector4K
*
*  Erase 4K of memory to 0xFF
*
*****************************************************************************/
ee_err_t EEPROM_EraseBlock(uint32_t Addr, uint32_t size)
{
    ee_err_t status;
#if ( gEepromParams_bufferedWrite_c == 0)
    if( size != gEepromParams_SectorSize_c )
    {
        status = ee_error;
    }
    else if( NV_FlashEraseSector(gEepromParams_StartOffset_c + Addr, size) != kStatus_FLASH_Success )
    {
        status = ee_error;
    }
    else
    {
        status = ee_ok;
    }
#else
	int iAPIreturn;
	status = ee_ok;
	if( size != gEepromParams_SectorSize_c )
    {
        status = ee_error;
    }
	uint32_t pageIndex = Addr/gEepromParams_SectorSize_c;
    iAPIreturn = FLASH_ErasePages(FLASH, pageIndex, 1);
    if(iAPIreturn != FLASH_DONE)
    {
        return(ee_error);
    }
    /* perform a blank check */
    iAPIreturn =
    FLASH_BlankCheck(
        FLASH,
        (uint8_t *)(pageIndex * gEepromParams_SectorSize_c),
        (uint8_t *)((pageIndex * gEepromParams_SectorSize_c) + (gEepromParams_SectorSize_c-1))
    );
	if(iAPIreturn != FLASH_DONE)
    {
        return(ee_error);
    }
	
#endif
    
    return status;
}

/*****************************************************************************
*  EEPROM_WriteData
*
*  Writes a data buffer into EEPROM, at a given address
*
*****************************************************************************/
ee_err_t EEPROM_WriteData(uint32_t NoOfBytes, uint32_t Addr, uint8_t *Outbuf)
{
#if ( gEepromParams_bufferedWrite_c == 0)
#ifndef CPU_JN518X

    /* If Erase is done during write, no need to erase here */
#if (gFlashEraseDuringWrite == 0)
    if( ee_ok != EEPROM_PrepareForWrite(NoOfBytes, Addr) )
    {
        return ee_error;
    }
#endif
#endif
#if (gEepromParams_WriteAlignment_c > 1)
    if( mWriteBuffLevel )
    {
        if( Addr >= mWriteBuffAddr &&
            Addr <  mWriteBuffAddr + gEepromParams_WriteAlignment_c )
        {
            uint32_t offset = Addr - mWriteBuffAddr;
            uint32_t size = gEepromParams_WriteAlignment_c - offset;

            FLib_MemCpy(&mWriteBuff[offset], Outbuf, size);
            Addr += size;
            Outbuf += size;
            NoOfBytes -= size;
        }
        else
        {
            FLib_MemSet(&mWriteBuff[mWriteBuffLevel], 0xFF, gEepromParams_WriteAlignment_c - mWriteBuffLevel);
        }
        
        NV_FlashProgramUnaligned(gEepromParams_StartOffset_c + mWriteBuffAddr, sizeof(mWriteBuff), mWriteBuff);
        mWriteBuffLevel = 0;
    }

    if( Addr & (gEepromParams_WriteAlignment_c-1) )
        return ee_not_aligned;

    /* Store unaligned bytes for later processing */
    mWriteBuffLevel = NoOfBytes % gEepromParams_WriteAlignment_c;
    NoOfBytes -= mWriteBuffLevel;
    mWriteBuffAddr = Addr + NoOfBytes;
    FLib_MemCpy(mWriteBuff, &Outbuf[NoOfBytes], mWriteBuffLevel);
#endif

    /* Write data to FLASH */
    if( NV_FlashProgramUnaligned(gEepromParams_StartOffset_c + Addr, NoOfBytes, Outbuf) != kStatus_FLASH_Success )
    {
        return ee_error;
    }
#else
    uint32_t effectiveAddress = Addr + mCurrentWriteOffset;
    if ( mEepromPageIndex == 0   )
    {
        /* You need to know the initial page to write to, If writing consecutively the EEPROM_FixWriteDataInFlash
		   will increment the right page.
         */
        mEepromPageIndex = ( effectiveAddress  / gEePromParams_BufferSize_c );
		
    }
	/* sanity check for raw write and not sequential write the effective address will be higher */
	if ( ( effectiveAddress / gEePromParams_BufferSize_c ) != mEepromPageIndex )
	{
		mEepromPageIndex = ( effectiveAddress / gEePromParams_BufferSize_c );
	}
	/* Realign if a reset has happened */
	if ( ( ((Addr + mCurrentWriteOffset) % gEePromParams_BufferSize_c )  == 0) && 
          mWriteBuffAddr < effectiveAddress )
    {
	    mWriteBuffAddr = effectiveAddress;
	}
	
    while ( NoOfBytes > 0 )
    {
        while ( (mLocalIndex <= gEePromParams_BufferSize_c- 1) && ( NoOfBytes > 0 ) )
        {
            gEePromStandaloneBuffer [ mLocalIndex++] = *Outbuf++;
            NoOfBytes--;
        }
        if (  mLocalIndex >= gEePromParams_BufferSize_c- 1  )
        {            
			if ( EEPROM_FixWriteDataInFlash (mEepromPageIndex) != ee_ok )
            {
				return ee_error;
            }
        }
    }

    	
#endif
    
    return ee_ok;    
}

/*****************************************************************************
*  EEPROM_ReadData
*
*  Reads a data buffer from EEPROM, from a given address
*
*****************************************************************************/
ee_err_t EEPROM_ReadData(uint16_t NoOfBytes, uint32_t Addr, uint8_t *inbuf)
{
#if ( gEepromParams_bufferedWrite_c == 0)
  FLib_MemCpy(inbuf, (void*)(gEepromParams_StartOffset_c + Addr), NoOfBytes);

#if (gEepromParams_WriteAlignment_c > 1)
  uint32_t i;

  /* Copy data from write buffer if needed */
  for( i=0; i<mWriteBuffLevel; i++ )
  {
      if( (mWriteBuffAddr + i) >= Addr &&
          (mWriteBuffAddr + i) <  (Addr + NoOfBytes) )
      {
          inbuf[mWriteBuffAddr - Addr + i] = mWriteBuff[i];
      }
  }
#endif
#else
    uint32_t i;
    int iAPIreturn;
    uint32_t effectiveAddress = mCurrentWriteOffset + Addr;
	
    iAPIreturn =
        FLASH_BlankCheck(
            FLASH,
            (uint8_t *)(effectiveAddress),
            (uint8_t *)(effectiveAddress + ( NoOfBytes - 1) )
        );
       
	if(iAPIreturn != FLASH_DONE)
    {
		FLib_MemCpy(inbuf, (void*)(effectiveAddress), NoOfBytes);
    }

    /* Copy data from write buffer if needed */
    for( i=0; i < mLocalIndex  ; i++ )
    { 
		if( (( mWriteBuffAddr + i ) >= effectiveAddress) &&
		     ( mWriteBuffAddr + i) <  (effectiveAddress + NoOfBytes) ) 
        {
    	    inbuf[(mWriteBuffAddr - effectiveAddress)+ i] = gEePromStandaloneBuffer[i];
        }
    }
#endif
  
  return ee_ok;
}

/*****************************************************************************
*  EEPROM_ReadStatusReq
*
*
*****************************************************************************/
uint8_t EEPROM_isBusy(void)
{
  return FALSE;
}

/******************************************************************************
*******************************************************************************
* Private Functions
*******************************************************************************
******************************************************************************/
#define IS_PAGE_ALIGNED(x) (((uint32_t)(x) & (FLASH_PAGE_SIZE-1)) == 0)

ee_err_t EEPROM_EraseArea(uint32_t *Addr, int32_t *size, bool non_blocking)
{
    ee_err_t status = ee_error;
#ifdef CPU_JN518X
    int32_t remain_sz = (int32_t)*size;
    uint32_t erase_addr = *Addr;

    EEPROM_DBG_LOG("");

    do {
        if (!IS_PAGE_ALIGNED(erase_addr))
        {
            status = ee_error;
            break;
        }
        if (remain_sz == 0)
        {
            status = ee_error;
            break;
        }
        if (FLASH_Erase(FLASH, (uint8_t *)erase_addr,
                                    (uint8_t *)(erase_addr + remain_sz - 1)) != FLASH_DONE)
        {
            status = ee_error;
            break;
        }
        erase_addr += remain_sz;
        remain_sz = 0;
        status = ee_ok;
    } while (0);

    *Addr = erase_addr;
    *size = 0;
#endif
    return status;
}

ee_err_t EEPROM_EraseNextBlock(uint32_t Addr, uint32_t size)
{
    ee_err_t status = ee_error;
    uint8_t *erase_start;
    uint8_t *erase_end;

    EEPROM_DBG_LOG("");

    do {
        erase_start = (uint8_t*)Addr;
        size = (size >= FLASH_PAGE_SIZE*121 ) ? FLASH_PAGE_SIZE*121 : size;
        erase_end = erase_start + (size - 1);
        if (FLASH_Erase(FLASH, (uint8_t *)erase_start, erase_end) != FLASH_DONE)
        {
            status = ee_error;
            break;
        }

        status = ee_ok;
    } while (0);
    return status;
}

ee_err_t EEPROM_SectorAlignmentAfterReset(uint32_t Addr)
{
    /* empty implementation for now */
    return ee_error;
}

/*****************************************************************************
*  EEPROM_WriteData
*
*  Writes a data buffer into the External Memory, at a given address
*
*****************************************************************************/
#if ( gEepromParams_bufferedWrite_c == 0)
#ifndef CPU_JN518X
#if (gFlashEraseDuringWrite == 0)
static ee_err_t EEPROM_PrepareForWrite(uint32_t NoOfBytes, uint32_t Addr)
{
    uint32_t i;
    uint32_t startBlk, endBlk;
    
    /* Obtain the first and last block that need to be erased */
    startBlk = Addr / gEepromParams_SectorSize_c;
    endBlk   = (Addr + NoOfBytes) / gEepromParams_SectorSize_c;
    
    if( (Addr + NoOfBytes) & gEepromParams_SectorSize_c )
    {
        endBlk++;
    }
    
    /* Check if the block was previously erased */
    for(i = startBlk; i <= endBlk; i++)
    {
        if ( (mEepromEraseBitmap[i/8] & (1U << (i%8) ) ) == 0)
        {
            if (EEPROM_EraseBlock(i * gEepromParams_SectorSize_c, gEepromParams_SectorSize_c) != ee_ok)
            {
                return ee_error;
            }
            mEepromEraseBitmap[i/8] |= 1U << (i%8);
        }
    }
    
    return ee_ok;
}
#endif
#endif
#endif
#if ( gEepromParams_bufferedWrite_c == 1)
/*****************************************************************************
*  OTA_iFixWriteDataInNVM
*
*  writes fixed data buffer into flash
*
*****************************************************************************/
static ee_err_t EEPROM_FixWriteDataInFlash(
    uint16_t      PageIndex)
{
    int iAPIreturn;
	 /* Erase Current Data In Specified Page */
#if (gFlashEraseDuringWrite == 0) 
    iAPIreturn =
            FLASH_ErasePages(FLASH, PageIndex, 1);

            if(iAPIreturn != FLASH_DONE)
            {
                return ee_error;
            }
#endif
    /* perform a blank check */
    iAPIreturn =
    FLASH_BlankCheck(
        FLASH,
        (uint8_t *)(PageIndex * gEePromParams_BufferSize_c),
        (uint8_t *)((PageIndex * gEePromParams_BufferSize_c) + (gEePromParams_BufferSize_c-1))
    );
    if(iAPIreturn != FLASH_DONE)
    {
        return(ee_error);
    }

    // program data
    iAPIreturn =
    FLASH_Program(
            FLASH,
            (uint32_t *)(PageIndex * gEePromParams_BufferSize_c),
            (void *)gEePromStandaloneBuffer,
             gEePromParams_BufferSize_c);

    if(iAPIreturn != FLASH_DONE)
    {
        return(ee_error);
    }

    mLocalIndex = 0;
    memset (gEePromStandaloneBuffer, 0xFF, gEePromParams_BufferSize_c);
	
	/* Only makes sense to keep track of offset written to writing sequentiually. If not writing sequentially
	we want to make sure that we calculate the write page index everytime so needs to be set back to 0 */
	
	if( mWriteBuffAddr < ( ((  PageIndex + 1) * gEePromParams_BufferSize_c)))
	{
	   mWriteBuffAddr = ( ((  PageIndex + 1) * gEePromParams_BufferSize_c));
	   mEepromPageIndex++;
	}
    return(ee_ok);
}

/*****************************************************************************
*  EEPROM_FlashWriteBufferedTailEndBytes
*
*  writes fixed data buffer into flash
*
*****************************************************************************/
bool    EEPROM_FlashWriteBufferedTailEndBytes (uint32_t    Addr)
{
    int iAPIreturn;
	/* Here Addr is absolute address so no need to adjust */
    if ( mEepromPageIndex == 0 )
    {
        mEepromPageIndex = Addr / gEePromParams_BufferSize_c;
    }
#if (gFlashEraseDuringWrite == 0) 
    /* Erase Current Data In Specified Page */
    iAPIreturn =
    FLASH_ErasePages(FLASH, mEepromPageIndex, 1);
    if(iAPIreturn != FLASH_DONE)
    {
        return(ee_error);
    }
#endif
    if ( EEPROM_FixWriteDataInFlash ( mEepromPageIndex ) == ee_ok)
    {
        mEepromPageIndex = 0;
        return ee_ok;
    }
    return ee_error;
}


#endif
#endif /* gEepromDevice_InternalFlash_c */
