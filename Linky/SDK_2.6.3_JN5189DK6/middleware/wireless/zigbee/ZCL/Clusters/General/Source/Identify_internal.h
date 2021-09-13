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
 * MODULE:             Groups Cluster
 *
 * COMPONENT:          Groups_internal.h
 *
 * DESCRIPTION:        The internal API for the Groups Cluster
 *
 *****************************************************************************/

#ifndef  IDENTIFY_INTERNAL_H_INCLUDED
#define  IDENTIFY_INTERNAL_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include "jendefs.h"

#include "zcl.h"
#include "Identify.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_IdentifyCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);

#ifdef IDENTIFY_SERVER 
PUBLIC teZCL_Status eCLD_IdentifyCommandIdentifyRequestReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_Identify_IdentifyRequestPayload *psPayload);

PUBLIC teZCL_Status eCLD_IdentifyCommandIdentifyQueryRequestReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber);

PUBLIC teZCL_Status eCLD_IdentifyCommandIdentifyQueryResponseSend(
                    uint8           u8SourceEndPointId,
                    uint8           u8DestinationEndPointId,
                    tsZCL_Address   *psDestinationAddress,
                    uint8           *pu8TransactionSequenceNumber,
                    tsCLD_Identify_IdentifyQueryResponsePayload *psPayload);

#ifdef CLD_IDENTIFY_CMD_TRIGGER_EFFECT
PUBLIC teZCL_Status eCLD_IdentifyCommandTriggerEffectRequestReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_Identify_TriggerEffectRequestPayload *psPayload);
#endif 
 
#ifdef CLD_IDENTIFY_ATTR_COMMISSION_STATE
PUBLIC teZCL_Status eCLD_IdentifyUpdateCommissionStateCommandReceive(
                    ZPS_tsAfEvent                               *pZPSevent,
                    uint8                                       *pu8TransactionSequenceNumber,
                    tsCLD_Identify_UpdateCommissionStatePayload *psPayload);
#endif 
#endif /* IDENTIFY_SERVER */                    

#ifdef IDENTIFY_CLIENT
PUBLIC teZCL_Status eCLD_IdentifyCommandIdentifyQueryResponseReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_Identify_IdentifyQueryResponsePayload *psPayload);

#endif /* IDENTIFY_CLIENT */ 

#ifdef CLD_IDENTIFY_CMD_EZ_MODE_INVOKE                  
PUBLIC teZCL_Status eCLD_IdentifyEZModeInvokeCommandReceive(
                    ZPS_tsAfEvent                               *pZPSevent,
                    uint8                                       *pu8TransactionSequenceNumber,
                    tsCLD_Identify_EZModeInvokePayload          *psPayload);                
#endif
         
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

#endif  /* IDENTIFY_INTERNAL_H_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
