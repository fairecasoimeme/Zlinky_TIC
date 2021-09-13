/****************************************************************************
 *
 * Copyright 2020 NXP.
 *
 * NXP Confidential. 
 * 
 * This software is owned or controlled by NXP and may only be used strictly 
 * in accordance with the applicable license terms.  
 * By expressly accepting such terms or by downloading, installing, activating 
 * and/or otherwise using the software, you are agreeing that you have read, 
 * and that you agree to comply with and are bound by, such license terms.  
 * If you do not agree to be bound by the applicable license terms, 
 * then you may not retain, install, activate or otherwise use the software. 
 * 
 *
 ****************************************************************************/




#ifndef PORTMACRO_JN518x_H
#define PORTMACRO_JN518x_H

#include "MicroSpecific.h"
#include "dbg.h"
#include "aessw_ccm.h"
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
/* Imported functions                                        */
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
/* Local functions                                        */
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
/* Imported variables                                        */
/*-----------------------------------------------------------*/



PUBLIC uint8 ZPS_eEnterCriticalSection(void* hMutex, uint32* psIntStore);
PUBLIC uint8 ZPS_eExitCriticalSection(void* hMutex, uint32* psIntStore);
PUBLIC uint8 ZPS_u8GrabMutexLock(void* hMutex, uint32* psIntStore);
PUBLIC uint8 ZPS_u8ReleaseMutexLock(void* hMutex, uint32* psIntStore);
PUBLIC uint32 u32Reverse(uint32 u32InWord);
PUBLIC void vACI_OptimisedCcmStar(
    bool_t         bEncrypt,
    uint8          u8M,
    uint8          u8alength,
    uint8          u8mlength,
    tuAES_Block   *puNonce,
    uint8         *pau8authenticationData,
    uint8         *pau8Data,
    uint8         *pau8checksumData,
    bool_t        *pbChecksumVerify);
PUBLIC bool_t bACI_WriteKey(tsReg128 *psKeyData);
PUBLIC void vSwipeEndian(AESSW_Block_u *puBlock, tsReg128 *psReg, bool_t bBlockToReg);
#endif /* PORTMACRO_JN518x_H */
