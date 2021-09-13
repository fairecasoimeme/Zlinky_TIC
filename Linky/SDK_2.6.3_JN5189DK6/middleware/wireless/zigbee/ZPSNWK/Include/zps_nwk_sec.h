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


/*****************************************************************************
 *
 * MODULE:             ZPS NWK
 *
 * COMPONENT:          zps_nwk_sec.h
 *
 * DESCRIPTION:        ZPS NWK Security defines
 *
 *****************************************************************************/

/**
 * @defgroup g_zps_nwk ZPS Network Layer infrastructure
 */

#ifndef _zps_nwk_sec_h_
#define _zps_nwk_sec_h_

#ifdef __cplusplus
extern "C" {
#endif

/***********************/
/**** INCLUDE FILES ****/
/***********************/
#include "jendefs.h"
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
#include "AHI_AES.h"
#endif
#include "aessw_ccm.h"
#include "pdum_nwk.h"

/************************/
/**** MACROS/DEFINES ****/
/************************/

#define ZPS_SEC_KEY_LENGTH                  16

/* Security Levels */
#define ZPS_NWK_SEC_SECURITY_LEVEL_0        0       /* No Security */
#define ZPS_NWK_SEC_SECURITY_LEVEL_1        1       /* MIC-32 */
#define ZPS_NWK_SEC_SECURITY_LEVEL_2        2       /* MIC-64 */
#define ZPS_NWK_SEC_SECURITY_LEVEL_3        3       /* MIC-128 */
#define ZPS_NWK_SEC_SECURITY_LEVEL_4        4       /* ENC */
#define ZPS_NWK_SEC_SECURITY_LEVEL_5        5       /* ENC-MIC-32 */
#define ZPS_NWK_SEC_SECURITY_LEVEL_6        6       /* ENC-MIC-64 */
#define ZPS_NWK_SEC_SECURITY_LEVEL_7        7       /* ENC-MIC-128 */

#define ZPS_NWK_SEC_SECURITY_LEVEL_MASK     0x07

/* Key Identifiers */
#define ZPS_NWK_SEC_DATA_KEY                0
#define ZPS_NWK_SEC_NETWORK_KEY             1
#define ZPS_NWK_SEC_KEY_TRANSPORT_KEY       2
#define ZPS_NWK_SEC_KEY_KOAD_KEY            3
#define ZPS_NWK_SEC_KEY_INVALID_KEY         0xFF

extern uint32 ZPS_u32NwkFcSaveCountBitShift(void);

/**************************/
/**** TYPE DEFINITIONS ****/
/**************************/

typedef struct
{
    uint8 u8SecCtrl;
    uint32 u32FrameCounter;
    uint64 u64SrcAddr;
    uint8 u8KeySeqNum;
} ZPS_tsSecAuxHdr;

typedef void (*pfnCommandCallback) (uint8 u8CommandId, uint8 *pu8Buffer, uint8 u8Size);
/****************************/
/**** EXPORTED VARIABLES ****/
/****************************/


/****************************/
/**** EXPORTED FUNCTIONS ****/
/****************************/
#ifdef ZPS_NWK_OPT_FS_DYNAMIC_SEC_LVL
PUBLIC uint8 ZPS_u8NwkSecGetMicLen( uint8 u8SecurityLevel );
#endif
PUBLIC bool_t ZPS_bNwkSecFindMatSet( void *pvNwk, uint8 u8ActiveKSN, uint8 *u8SecMatIndex );
PUBLIC bool_t ZPS_bNwkSecCheckFC( void *pvNwk, uint64 u64SrcAddr, uint32 u32RecFC);
PUBLIC bool_t ZPS_bNwkSecHaveNetworkKey( void *pvNwk );
PUBLIC bool_t ZPS_bNwkSecCheckMatSet( void *pvNwk, uint8 u8ActiveKSN );
PUBLIC void ZPS_vNwkSecResetDevice(void *pvNwk,uint64 u64DevAddr);
PUBLIC bool_t ZPS_bSecEncrypt(void *pvNwk,
                              uint8 *pu8CurrHdr,
                              uint32 u32OutgoingFrameCounter,
                              bool_t bExtNonce,
                              AESSW_Block_u *puKey,
                              uint8 u8KeyId,
                              uint8 u8KeySeqNum,
                              PDUM_thNPdu hNPdu);
PUBLIC bool_t ZPS_bSecDecrypt(void *pvNwk,
                              ZPS_tsSecAuxHdr *psAuxSecHdr,
                              uint8 *pu8AuxSecHdr,
                              AESSW_Block_u *puKey,
                              PDUM_thNPdu hNPdu);

PUBLIC void ZPS_vNwkResetOutgoingFrameCounter(void *pvNwk);

PUBLIC void ZPS_vRegisterCommandCallback(void* pvFnCallback);
PUBLIC void ZPS_vSetBufferCommandCallback(uint8 *pu8Buffer);
#ifdef __cplusplus
};
#endif

#endif /* _zps_nwk_sec_h_ */

/* End of file **************************************************************/
