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



#ifndef PORTMACRO_JN517x_H
#define PORTMACRO_JN517x_H

#include "MicroSpecific.h"
#include "AHI_AES.h"
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
PUBLIC void vSwipeEndian(AESSW_Block_u *puBlock, tsReg128 *psReg, bool_t bBlockToReg);
PUBLIC uint32 u32Reverse(uint32 u32InWord);
#endif /* PORTMACRO_JN517x_H */
