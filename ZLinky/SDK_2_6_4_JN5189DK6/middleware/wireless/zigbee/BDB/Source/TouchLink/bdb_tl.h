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


/*###############################################################################
#
# MODULE:      BDB
#
# COMPONENT:   bdb_tl.h
#
# DESCRIPTION: BDB Touchlink API 
#              
#
###############################################################################*/

#ifndef BDB_TL_INCLUDED
#define BDB_TL_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include "jendefs.h"
#if (JENNIC_CHIP_FAMILY==JN518x)
   #include "fsl_reset.h"
#endif
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#define ADJUST_POWER   TRUE

#if JENNIC_CHIP==JN5169
#define TX_POWER_NORMAL     ((uint32)(8))
#define TX_POWER_LOW        ((uint32)(0))
#else
#define TX_POWER_NORMAL     ((uint32)(3))
#define TX_POWER_LOW        ((uint32)(-9))
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/



/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC void BDB_vTlInit(void);
PUBLIC bool_t BDB_bTlTouchLinkInProgress( void);
PUBLIC bool_t bIsTlStarted(void);
PUBLIC void BDB_vTlStateMachine( tsBDB_ZCLEvent *psEvent);

PUBLIC uint8 BDB_u8TlEncryptKey( uint8* au8InData,
                                  uint8* au8OutData,
                                  uint32 u32TransId,
                                  uint32 u32ResponseId,
                                  uint8 u8KeyIndex);
PUBLIC uint8 BDB_eTlDecryptKey( uint8* au8InData,
                                  uint8* au8OutData,
                                  uint32 u32TransId,
                                  uint32 u32ResponseId,
                                  uint8 u8KeyIndex);
PUBLIC bool_t BDB_bTlIsKeySupported(uint8 u8KeyIndex);
PUBLIC uint8 BDB_u8TlGetRandomPrimary(void);
PUBLIC uint8 BDB_u8TlNewUpdateID(uint8 u8ID1, uint8 u8ID2 );
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
PUBLIC void vECB_Decrypt(uint8* au8Key,
                         uint8* au8InData,
                         uint8* au8OutData);
#endif
PUBLIC void BDB_vTlTimerCb(void *pvParam);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
extern PUBLIC uint8 u8TimerBdbTl;

#if defined __cplusplus
}
#endif

#endif  /* BDB_TL_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/






