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
 * MODULE:             Power Profile Cluster
 *
 * COMPONENT:          PowerProfileServerCommands.c
 *
 * DESCRIPTION:        Power Profile Cluster definition
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_internal.h"
#include "pdum_apl.h"

#include "PowerProfile.h"
#include "PowerProfile_internal.h"

#include "dbg.h"


#ifdef DEBUG_CLD_PP
#define TRACE_PP    TRUE
#else
#define TRACE_PP    FALSE
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Public Functions                                              ***/
/****************************************************************************/
/****************************************************************************
 **
 ** NAME:       eCLD_PPPowerProfileReqReceive 
 **
 ** DESCRIPTION:
 ** handles rx of Power Profile Request
 **
 ** PARAMETERS:                                      Name                           Usage
 ** ZPS_tsAfEvent                                    *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition                         *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance                            *psClusterInstance             Cluster structure
 ** uint8                                            *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_PP_PowerProfileReqPayload              *psPayload                     Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_PPPowerProfileReqReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_PP_PowerProfileReqPayload                 *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
    {1,                 &u16ActualQuantity,             E_ZCL_UINT8,               &psPayload->u8PowerProfileId},
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT);

}

/****************************************************************************
 **
 ** NAME:       eCLD_PPEnergyPhasesScheduleReceive 
 **
 ** DESCRIPTION:
 ** handles rx of Power Profile Request
 **
 ** PARAMETERS:                                      Name                           Usage
 ** ZPS_tsAfEvent                                    *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition                         *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance                            *psClusterInstance             Cluster structure
 ** uint8                                            *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_PP_EnergyPhasesSchedulePayload             *psPayload                     Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_PPEnergyPhasesScheduleReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_PP_EnergyPhasesSchedulePayload            *psPayload,
                    uint16                                          u16HeaderOffset)
{

    teZCL_Status eStatus;
    uint16 u16ActualQuantity;
    int i;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
    {1,                 &u16ActualQuantity,             E_ZCL_UINT8,               &psPayload->u8PowerProfileId},
    {1,                 &u16ActualQuantity,             E_ZCL_UINT8,               &psPayload->u8NumOfScheduledPhases},
                                                };

    eStatus  =  eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_LESS);
    
    if(eStatus == E_ZCL_SUCCESS)
    {
        u16ActualQuantity = PDUM_u16APduInstanceGetPayloadSize(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst);
        
        /* 1 byte for Profile Id + 1 byte for schedule phases + structure size (sizeof(tsCLD_PP_EnergyPhaseDelay))) */
        u16HeaderOffset += 2; 
        for( i = 0 ; i < psPayload->u8NumOfScheduledPhases ; i++ ) 
        {
            u16HeaderOffset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16HeaderOffset, E_ZCL_UINT8,  &psPayload->psEnergyPhaseDelay[i].u8EnergyPhaseId);
            u16HeaderOffset += u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16HeaderOffset, E_ZCL_UINT16, &psPayload->psEnergyPhaseDelay[i].u16ScheduleTime);
        }
        
        if((u16HeaderOffset - u16ActualQuantity ) == 0){
            return E_ZCL_SUCCESS;
        }
    }
    return eStatus;                                  

}

#ifdef PP_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_PPPowerProfileSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Power Profile Notification/Response Command
 **
 ** PARAMETERS:                                           Name                               Usage
 ** uint8                                                 u8SourceEndPointId                 Source EP Id
 ** uint8                                                 u8DestinationEndPointId            Destination EP Id
 ** tsZCL_Address                                         *psDestinationAddress              Destination Address
 ** uint8                                                 *pu8TransactionSequenceNumber      Sequence number Pointer
 ** teCLD_PP_ServerGeneratedCommandID                     eCommandId                         Command ID
 ** tsCLD_PP_PowerProfilePayload                          *psPayload                         Payload
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_PPPowerProfileSend(
                                        uint8                                                       u8SourceEndPointId,
                                        uint8                                                       u8DestinationEndPointId,
                                        tsZCL_Address                                               *psDestinationAddress,
                                        uint8                                                       *pu8TransactionSequenceNumber,
                                        teCLD_PP_ServerGeneratedCommandID                           eCommandId,
                                        tsCLD_PP_PowerProfilePayload                                *psPayload)
{

    teZCL_Status    eReturnValue;
    tsZCL_ClusterInstance       *psClusterInstance;
    tsZCL_EndPointDefinition    *psEndPointDefinition;
    uint16 u16offset,u16PayloadSize;
    PDUM_thAPduInstance myPDUM_thAPduInstance = 0;
    uint8 i;
                                                 
    eReturnValue = eZCL_SearchForClusterEntry(u8SourceEndPointId, GENERAL_CLUSTER_ID_PP, TRUE, &psClusterInstance);
    
    if(eReturnValue != E_ZCL_SUCCESS)
    {
        return(eReturnValue);
    }
    
     /* Check end point is registered and cluster is present in the send device */
    if(eZCL_SearchForEPentry(u8SourceEndPointId, &psEndPointDefinition) != E_ZCL_SUCCESS)
    {
        return(E_ZCL_ERR_EP_UNKNOWN);
    }
    
    // get buffer
    myPDUM_thAPduInstance = hZCL_AllocateAPduInstance();

    if(myPDUM_thAPduInstance == PDUM_INVALID_HANDLE)
    {
        return(E_ZCL_ERR_ZBUFFER_FAIL);
    }
        
    // write command header
    u16offset = u16ZCL_WriteCommandHeader(myPDUM_thAPduInstance,
                                       eFRAME_TYPE_COMMAND_IS_SPECIFIC_TO_A_CLUSTER,
                                       FALSE,
                                       0,
                                       TRUE,
                                       psEndPointDefinition->bDisableDefaultResponse,
                                       *pu8TransactionSequenceNumber,
                                       eCommandId);
    
    /* Payload Size = 1 byte TotalProfileNum + 1 bytes for Profile Id + 1 byte for NumOfPhases + 10 bytes (1 byte Phase Id +
                      1 Byte for MacroPhaseId + 2 bytes Expected Duration + 2 bytes PeakPower + 2 bytes
                      Energy + 2 bytes Max Activation Delay )* NumofPhases */ 
    if(psPayload->u8TotalProfileNum !=  0)
    {
        u16PayloadSize = (3 + (psPayload->u8NumOfTransferredPhases * 10));
    }else{
        u16PayloadSize = 3;
    }
    
    u16PayloadSize +=  u16offset;
    
    // check buffer size
    if(PDUM_u16APduGetSize(psZCL_Common->hZCL_APdu) < u16PayloadSize)
    {
        // free buffer and return
        PDUM_eAPduFreeAPduInstance(myPDUM_thAPduInstance);
        return(E_ZCL_ERR_ZBUFFER_FAIL);
    }
    
    u16offset += u16ZCL_APduInstanceWriteNBO(
        myPDUM_thAPduInstance, u16offset, E_ZCL_UINT8, &psPayload->u8TotalProfileNum);
    u16offset += u16ZCL_APduInstanceWriteNBO(
        myPDUM_thAPduInstance, u16offset, E_ZCL_UINT8, &psPayload->u8PowerProfileId);
    u16offset += u16ZCL_APduInstanceWriteNBO(
        myPDUM_thAPduInstance, u16offset, E_ZCL_UINT8, &psPayload->u8NumOfTransferredPhases);
        
    for(i=0; i<psPayload->u8NumOfTransferredPhases; i++)
    {
        // setting up a Payload
        u16offset += u16ZCL_APduInstanceWriteNBO(
        myPDUM_thAPduInstance, u16offset, E_ZCL_UINT8, &psPayload->psEnergyPhaseInfo[i].u8EnergyPhaseId);
        u16offset += u16ZCL_APduInstanceWriteNBO(
        myPDUM_thAPduInstance, u16offset, E_ZCL_UINT8, &psPayload->psEnergyPhaseInfo[i].u8MacroPhaseId);
        u16offset += u16ZCL_APduInstanceWriteNBO(
        myPDUM_thAPduInstance, u16offset, E_ZCL_UINT16, &psPayload->psEnergyPhaseInfo[i].u16ExpectedDuration);
        u16offset += u16ZCL_APduInstanceWriteNBO(
        myPDUM_thAPduInstance, u16offset, E_ZCL_UINT16, &psPayload->psEnergyPhaseInfo[i].u16PeakPower);
        u16offset += u16ZCL_APduInstanceWriteNBO(
        myPDUM_thAPduInstance, u16offset, E_ZCL_UINT16, &psPayload->psEnergyPhaseInfo[i].u16Energy);
        u16offset += u16ZCL_APduInstanceWriteNBO(
        myPDUM_thAPduInstance, u16offset, E_ZCL_UINT16, &psPayload->psEnergyPhaseInfo[i].u16MaxActivationDelay);
    }

    // transmit the request
    if(eZCL_TransmitDataRequest(myPDUM_thAPduInstance,
                                u16offset,
                                u8SourceEndPointId,
                                u8DestinationEndPointId,
                                GENERAL_CLUSTER_ID_PP,
                                psDestinationAddress) != E_ZCL_SUCCESS)
    {
        return(E_ZCL_ERR_ZTRANSMIT_FAIL);
    }

    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       eCLD_PPPowerProfileStateSend
 **
 ** DESCRIPTION:
 ** Builds and sends a Power Profile State Notification/Response Command
 **
 ** PARAMETERS:                                           Name                               Usage
 ** uint8                                                 u8SourceEndPointId                 Source EP Id
 ** uint8                                                 u8DestinationEndPointId            Destination EP Id
 ** tsZCL_Address                                         *psDestinationAddress              Destination Address
 ** uint8                                                 *pu8TransactionSequenceNumber      Sequence number Pointer
 ** teCLD_PP_ServerGeneratedCommandID                     eCommandId                         Command ID
 ** tsCLD_PP_PowerProfileStatePayload                     *psPayload                         Payload
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_PPPowerProfileStateSend(
                                        uint8                                                       u8SourceEndPointId,
                                        uint8                                                       u8DestinationEndPointId,
                                        tsZCL_Address                                               *psDestinationAddress,
                                        uint8                                                       *pu8TransactionSequenceNumber,
                                        teCLD_PP_ServerGeneratedCommandID                           eCommandId,
                                        tsCLD_PP_PowerProfileStatePayload                           *psPayload)
{

    teZCL_Status    eReturnValue;
    tsZCL_ClusterInstance       *psClusterInstance;
    tsZCL_EndPointDefinition    *psEndPointDefinition;
    uint16 u16offset,u16PayloadSize;
    PDUM_thAPduInstance myPDUM_thAPduInstance = 0;
    uint8 i;
                                                 
    eReturnValue = eZCL_SearchForClusterEntry(u8SourceEndPointId, GENERAL_CLUSTER_ID_PP, TRUE, &psClusterInstance);
    
    if(eReturnValue != E_ZCL_SUCCESS)
    {
        return(eReturnValue);
    }
    
     /* Check end point is registered and cluster is present in the send device */
    if(eZCL_SearchForEPentry(u8SourceEndPointId, &psEndPointDefinition) != E_ZCL_SUCCESS)
    {
        return(E_ZCL_ERR_EP_UNKNOWN);
    }
    
    // get buffer
    myPDUM_thAPduInstance = hZCL_AllocateAPduInstance();

    if(myPDUM_thAPduInstance == PDUM_INVALID_HANDLE)
    {
        return(E_ZCL_ERR_ZBUFFER_FAIL);
    }

     
    // write command header
    u16offset = u16ZCL_WriteCommandHeader(myPDUM_thAPduInstance,
                                       eFRAME_TYPE_COMMAND_IS_SPECIFIC_TO_A_CLUSTER,
                                       FALSE,
                                       0,
                                       TRUE,
                                       psEndPointDefinition->bDisableDefaultResponse,
                                       *pu8TransactionSequenceNumber,
                                       eCommandId);
    
    /* Payload Size = 1 byte TotalProfileCount +  4 bytes (1 byte power Id +
                      1 Byte for EnergyPhaseId + 1byte RemoteControlable + 1 byte State  )* TotalProfileCount */ 
    u16PayloadSize = (1 + (psPayload->u8PowerProfileCount * 4));
    u16PayloadSize +=  u16offset;
    
    // check buffer size
    if(PDUM_u16APduGetSize(psZCL_Common->hZCL_APdu) < u16PayloadSize)
    {
        // free buffer and return
        PDUM_eAPduFreeAPduInstance(myPDUM_thAPduInstance);
        return(E_ZCL_ERR_ZBUFFER_FAIL);
    }
    
    u16offset += u16ZCL_APduInstanceWriteNBO(
        myPDUM_thAPduInstance, u16offset, E_ZCL_UINT8, &psPayload->u8PowerProfileCount);
        
    for(i=0; i<psPayload->u8PowerProfileCount; i++)
    {
        // setting up a Payload
        u16offset += u16ZCL_APduInstanceWriteNBO(
        myPDUM_thAPduInstance, u16offset, E_ZCL_UINT8, &psPayload->psPowerProfileRecord[i].u8PowerProfileId);
        u16offset += u16ZCL_APduInstanceWriteNBO(
        myPDUM_thAPduInstance, u16offset, E_ZCL_UINT8, &psPayload->psPowerProfileRecord[i].u8EnergyPhaseId);
        u16offset += u16ZCL_APduInstanceWriteNBO(
        myPDUM_thAPduInstance, u16offset, E_ZCL_BOOL,  &psPayload->psPowerProfileRecord[i].bPowerProfileRemoteControl);     
        u16offset += u16ZCL_APduInstanceWriteNBO(
        myPDUM_thAPduInstance, u16offset, E_ZCL_ENUM8, &psPayload->psPowerProfileRecord[i].u8PowerProfileState);        
    }

    // transmit the request
    if(eZCL_TransmitDataRequest(myPDUM_thAPduInstance,
                                u16offset,
                                u8SourceEndPointId,
                                u8DestinationEndPointId,
                                GENERAL_CLUSTER_ID_PP,
                                psDestinationAddress) != E_ZCL_SUCCESS)
    {
        return(E_ZCL_ERR_ZTRANSMIT_FAIL);
    }

    return(E_ZCL_SUCCESS);
}

#ifdef CLD_PP_CMD_GET_OVERALL_SCHEDULE_PRICE
/****************************************************************************
 **
 ** NAME:       eCLD_PPGetOverallSchedulePriceSend  
 **
 ** DESCRIPTION:
 ** Builds and sends a Power Profile Request
 **
 ** PARAMETERS:                                           Name                               Usage
 ** uint8                                                 u8SourceEndPointId                 Source EP Id
 ** uint8                                                 u8DestinationEndPointId            Destination EP Id
 ** tsZCL_Address                                         *psDestinationAddress              Destination Address
 ** uint8                                                 *pu8TransactionSequenceNumber      Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_PPGetOverallSchedulePriceSend  (
                                        uint8                                                       u8SourceEndPointId,
                                        uint8                                                       u8DestinationEndPointId,
                                        tsZCL_Address                                               *psDestinationAddress,
                                        uint8                                                       *pu8TransactionSequenceNumber)
{
    // handle sequence number get a new sequence number
    if(pu8TransactionSequenceNumber != NULL)
        *pu8TransactionSequenceNumber = u8GetTransactionSequenceNumber();
                                             
    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_PP,
                                  TRUE,
                                  E_CLD_PP_CMD_GET_OVERALL_SCHEDULE_PRICE,
                                  pu8TransactionSequenceNumber,
                                  0,
                                  FALSE,
                                  0,
                                  0
                                 );

}
#endif

/****************************************************************************
 **
 ** NAME:       eCLD_PPPowerProfileScheduleConstraintsSend  
 **
 ** DESCRIPTION:
 ** Builds and sends a Power Profile Schedule Constraints  Response /Notification
 **
 ** PARAMETERS:                                           Name                               Usage
 ** uint8                                                 u8SourceEndPointId                 Source EP Id
 ** uint8                                                 u8DestinationEndPointId            Destination EP Id
 ** tsZCL_Address                                         *psDestinationAddress              Destination Address
 ** uint8                                                 *pu8TransactionSequenceNumber      Sequence number Pointer
 ** teCLD_PP_ServerReceivedCommandID                      eCommandId                         Command ID
 ** tsCLD_PP_PowerProfileScheduleConstraintsPayload       *psPayload                         Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_PPPowerProfileScheduleConstraintsSend (
                                        uint8                                                       u8SourceEndPointId,
                                        uint8                                                       u8DestinationEndPointId,
                                        tsZCL_Address                                               *psDestinationAddress,
                                        uint8                                                       *pu8TransactionSequenceNumber,
                                        teCLD_PP_ServerGeneratedCommandID                           eCommandId,
                                        tsCLD_PP_PowerProfileScheduleConstraintsPayload             *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
    
        {1,                              E_ZCL_UINT8,                   &psPayload->u8PowerProfileId},
        {1,                              E_ZCL_UINT16,                  &psPayload->u16StartAfter},
        {1,                              E_ZCL_UINT16,                  &psPayload->u16StopBefore},
                                                 };
                                                 
        return eZCL_CustomCommandSend(u8SourceEndPointId,
                                      u8DestinationEndPointId,
                                      psDestinationAddress,
                                      GENERAL_CLUSTER_ID_PP,
                                      TRUE,
                                      eCommandId,
                                      pu8TransactionSequenceNumber,
                                      asPayloadDefinition,
                                      FALSE,
                                      0,
                                      sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem)
                                     );

}

#ifdef CLD_PP_CMD_GET_POWER_PROFILE_PRICE_EXTENDED
/****************************************************************************
 **
 ** NAME:       eCLD_PPGetPowerProfilePriceExtendedSend   
 **
 ** DESCRIPTION:
 ** Builds and sends a Get Power Profile Schedule Price Extended  Command
 **
 ** PARAMETERS:                                           Name                               Usage
 ** uint8                                                 u8SourceEndPointId                 Source EP Id
 ** uint8                                                 u8DestinationEndPointId            Destination EP Id
 ** tsZCL_Address                                         *psDestinationAddress              Destination Address
 ** uint8                                                 *pu8TransactionSequenceNumber      Sequence number Pointer
 ** tsCLD_PP_GetPowerProfilePriceExtendedPayload          *psPayload                         Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_PPGetPowerProfilePriceExtendedSend  (
                                        uint8                                                       u8SourceEndPointId,
                                        uint8                                                       u8DestinationEndPointId,
                                        tsZCL_Address                                               *psDestinationAddress,
                                        uint8                                                       *pu8TransactionSequenceNumber,
                                        tsCLD_PP_GetPowerProfilePriceExtendedPayload                *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
    
        {1,                              E_ZCL_BMAP8,                   &psPayload->u8Options},
        {1,                              E_ZCL_UINT8,                   &psPayload->u8PowerProfileId},
        {1,                              E_ZCL_UINT16,                  &psPayload->u16PowerProfileStartTime},
                                                 };
    // handle sequence number get a new sequence number
    if(pu8TransactionSequenceNumber != NULL)
        *pu8TransactionSequenceNumber = u8GetTransactionSequenceNumber();
        
    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_PP,
                                  TRUE,
                                  E_CLD_PP_CMD_GET_POWER_PROFILE_PRICE_EXTENDED,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem)
                                 );

}
#endif
/****************************************************************************
 **
 ** NAME:       eCLD_PPPowerProfileStateReqReceive 
 **
 ** DESCRIPTION:
 ** handles rx of Power Profile State Request
 **
 ** PARAMETERS:                                      Name                           Usage
 ** ZPS_tsAfEvent                                    *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition                         *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance                            *psClusterInstance             Cluster structure
 ** uint8                                            *pu8TransactionSequenceNumber  Sequence number Pointer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_PPPowerProfileStateReqReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber)
{

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     0,
                                     0,
                                     E_ZCL_ACCEPT_EXACT);

}

/****************************************************************************
 **
 ** NAME:       eCLD_PPGetPowerProfilePriceOrExtendedRspReceive  
 **
 ** DESCRIPTION:
 ** handles rx of Power Profile Request
 **
 ** PARAMETERS:                                      Name                           Usage
 ** ZPS_tsAfEvent                                    *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition                         *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance                            *psClusterInstance             Cluster structure
 ** uint8                                            *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_PP_GetPowerProfilePriceRspPayload         *psPayload                     Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_PPGetPowerProfilePriceOrExtendedRspReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_PP_GetPowerProfilePriceRspPayload         *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
    {1,                 &u16ActualQuantity,             E_ZCL_UINT8,               &psPayload->u8PowerProfileId},
    {1,                 &u16ActualQuantity,             E_ZCL_UINT16,              &psPayload->u16Currency},
    {1,                 &u16ActualQuantity,             E_ZCL_UINT32,              &psPayload->u32Price},
    {1,                 &u16ActualQuantity,             E_ZCL_UINT8,               &psPayload->u8PriceTrailingDigits},
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT);

}
/****************************************************************************
 **
 ** NAME:       eCLD_PPGetOverallSchedulePriceRspReceive   
 **
 ** DESCRIPTION:
 ** handles rx of Power Profile Request
 **
 ** PARAMETERS:                                      Name                           Usage
 ** ZPS_tsAfEvent                                    *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition                         *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance                            *psClusterInstance             Cluster structure
 ** uint8                                            *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_PP_GetOverallSchedulePriceRspPayload      *psPayload                     Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_PPGetOverallSchedulePriceRspReceive (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_PP_GetOverallSchedulePriceRspPayload      *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
    {1,                 &u16ActualQuantity,             E_ZCL_UINT16,              &psPayload->u16Currency},
    {1,                 &u16ActualQuantity,             E_ZCL_UINT32,              &psPayload->u32Price},
    {1,                 &u16ActualQuantity,             E_ZCL_UINT8,               &psPayload->u8PriceTrailingDigits},
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT);

}

#endif
/****************************************************************************/
/***        Private Functions                                             ***/
/****************************************************************************/
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
