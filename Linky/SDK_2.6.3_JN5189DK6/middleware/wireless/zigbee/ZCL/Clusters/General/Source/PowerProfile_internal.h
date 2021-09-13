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
 * MODULE:             Power Profile
 *
 * COMPONENT:          PowerProfile_internal.h
 *
 * DESCRIPTION:        The internal API for the Power Prfoile
 *
 *****************************************************************************/

#ifndef  POLLCONTROL_INTERNAL_H_INCLUDED
#define  POLLCONTROL_INTERNAL_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include "jendefs.h"

#include "zcl.h"
#include "PowerProfile.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC teZCL_Status eCLD_PPCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);
                    
PUBLIC teZCL_Status eCLD_PPPowerProfileReqReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_PP_PowerProfileReqPayload                 *psPayload);                    

PUBLIC teZCL_Status eCLD_PPEnergyPhasesScheduleReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_PP_EnergyPhasesSchedulePayload            *psPayload,
                    uint16                                          u16HeaderOffset);                    
#ifdef PP_CLIENT
PUBLIC teZCL_Status eCLD_PPGetOverallSchedulePriceReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber);
PUBLIC teZCL_Status eCLD_PPPowerProfileScheduleConstraintsReceive   (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_PP_PowerProfileScheduleConstraintsPayload *psPayload);
PUBLIC teZCL_Status eCLD_PPGetPowerProfilePriceExtendedReceive   (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_PP_GetPowerProfilePriceExtendedPayload    *psPayload);
PUBLIC teZCL_Status eCLD_PPGetPowerProfilePriceOrExtendedRspSend  (
                    uint8                                            u8SourceEndPointId,
                    uint8                                            u8DestinationEndPointId,
                    tsZCL_Address                                    *psDestinationAddress,
                    uint8                                            *pu8TransactionSequenceNumber,
                    teCLD_PP_ServerReceivedCommandID                 eCommandId,
                    tsCLD_PP_GetPowerProfilePriceRspPayload         *psPayload);
PUBLIC teZCL_Status eCLD_PPGetOverallSchedulePriceRspSend(
                    uint8                                             u8SourceEndPointId,
                    uint8                                             u8DestinationEndPointId,
                    tsZCL_Address                                     *psDestinationAddress,
                    uint8                                             *pu8TransactionSequenceNumber,
                    tsCLD_PP_GetOverallSchedulePriceRspPayload       *psPayload);
                
#endif

#ifdef PP_SERVER
PUBLIC teZCL_Status eCLD_PPPowerProfileStateReqReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber);
PUBLIC teZCL_Status eCLD_PPGetPowerProfilePriceOrExtendedRspReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_PP_GetPowerProfilePriceRspPayload         *psPayload);    
PUBLIC teZCL_Status eCLD_PPGetOverallSchedulePriceRspReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_PP_GetOverallSchedulePriceRspPayload      *psPayload);    
#endif
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

#endif  /* PP_INTERNAL_H_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
