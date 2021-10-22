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
 ** NAME:       eCLD_PPPowerProfileRequestSend 
 **
 ** DESCRIPTION:
 ** Builds and sends a Power Profile Request
 **
 ** PARAMETERS:                                           Name                               Usage
 ** uint8                                                 u8SourceEndPointId                 Source EP Id
 ** uint8                                                 u8DestinationEndPointId            Destination EP Id
 ** tsZCL_Address                                         *psDestinationAddress              Destination Address
 ** uint8                                                 *pu8TransactionSequenceNumber      Sequence number Pointer
 ** teCLD_PP_ServerReceivedCommandID                      eCommandId                         Command ID
 ** tsCLD_PP_PowerProfileReqPayload                   *psPayload                         Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_PPPowerProfileRequestSend (
                                        uint8                                                       u8SourceEndPointId,
                                        uint8                                                       u8DestinationEndPointId,
                                        tsZCL_Address                                               *psDestinationAddress,
                                        uint8                                                       *pu8TransactionSequenceNumber,
                                        teCLD_PP_ServerGeneratedCommandID                           eCommandId,
                                        bool_t                                                        bServer,
                                        tsCLD_PP_PowerProfileReqPayload                             *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
    
        {1,                              E_ZCL_UINT8,                   &psPayload->u8PowerProfileId},
                                                 };
    // handle sequence number get a new sequence number
    if(bServer && pu8TransactionSequenceNumber != NULL)
        *pu8TransactionSequenceNumber = u8GetTransactionSequenceNumber();
        
    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                  u8DestinationEndPointId,
                                  psDestinationAddress,
                                  GENERAL_CLUSTER_ID_PP,
                                  bServer,
                                  eCommandId,
                                  pu8TransactionSequenceNumber,
                                  asPayloadDefinition,
                                  FALSE,
                                  0,
                                  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem)
                                 );

}

/****************************************************************************
 **
 ** NAME:       eCLD_PPEnergyPhasesScheduleSend    
 **
 ** DESCRIPTION:
 ** Builds and sends a Energy Phase Schedule Command
 ** PARAMETERS:                                           Name                               Usage
 ** uint8                                                 u8SourceEndPointId                 Source EP Id
 ** uint8                                                 u8DestinationEndPointId            Destination EP Id
 ** tsZCL_Address                                         *psDestinationAddress              Destination Address
 ** uint8                                                 *pu8TransactionSequenceNumber      Sequence number Pointer
 ** teCLD_PP_ServerReceivedCommandID                      eCommandId                         Command Id     
 ** bool_t                                                  bServer                            Is Server Or not
 ** tsCLD_PP_EnergyPhasesSchedulePayload                  *psPayload                         Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_PPEnergyPhasesScheduleSend (
                                        uint8                                                       u8SourceEndPointId,
                                        uint8                                                       u8DestinationEndPointId,
                                        tsZCL_Address                                               *psDestinationAddress,
                                        uint8                                                       *pu8TransactionSequenceNumber,
                                        teCLD_PP_ServerReceivedCommandID                            eCommandId,
                                        bool_t                                                        bServer,
                                        tsCLD_PP_EnergyPhasesSchedulePayload                        *psPayload)
{
    teZCL_Status    eReturnValue;
    tsZCL_ClusterInstance       *psClusterInstance;
    tsZCL_EndPointDefinition    *psEndPointDefinition;
    uint16 u16offset,u16PayloadSize;
    PDUM_thAPduInstance myPDUM_thAPduInstance = 0;
    uint8 i;
                                                 
    eReturnValue = eZCL_SearchForClusterEntry(u8SourceEndPointId, GENERAL_CLUSTER_ID_PP, bServer, &psClusterInstance);
    
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

    // handle sequence number pass present value back to user
    if(eCommandId != E_CLD_PP_CMD_ENERGY_PHASES_SCHEDULE_RSP && pu8TransactionSequenceNumber != NULL)
        *pu8TransactionSequenceNumber = u8GetTransactionSequenceNumber();

    // write command header
    u16offset = u16ZCL_WriteCommandHeader(myPDUM_thAPduInstance,
                                       eFRAME_TYPE_COMMAND_IS_SPECIFIC_TO_A_CLUSTER,
                                       FALSE,
                                       0,
                                       bServer,
                                       psEndPointDefinition->bDisableDefaultResponse,
                                       (( pu8TransactionSequenceNumber != NULL )? *pu8TransactionSequenceNumber : u8GetTransactionSequenceNumber()),
                                       eCommandId);
    
    /* Payload Size = 1 bytes for Profile Id + 1 byte for NumOfPhases + 3 bytes (1 byte Phase Id + 2 bytes Schedule Time )* NumofPhases */ 
    u16PayloadSize = (2 + (psPayload->u8NumOfScheduledPhases * 3));
    u16PayloadSize +=  u16offset;
    
    // check buffer size
    if(PDUM_u16APduGetSize(psZCL_Common->hZCL_APdu) < u16PayloadSize)
    {
        // free buffer and return
        PDUM_eAPduFreeAPduInstance(myPDUM_thAPduInstance);
        return(E_ZCL_ERR_ZBUFFER_FAIL);
    }
    
    u16offset += u16ZCL_APduInstanceWriteNBO(
        myPDUM_thAPduInstance, u16offset, E_ZCL_UINT8, &psPayload->u8PowerProfileId);
    u16offset += u16ZCL_APduInstanceWriteNBO(
        myPDUM_thAPduInstance, u16offset, E_ZCL_UINT8, &psPayload->u8NumOfScheduledPhases);
        
    for(i=0; i<psPayload->u8NumOfScheduledPhases; i++)
    {
        // setting up a report
        u16offset += u16ZCL_APduInstanceWriteNBO(
        myPDUM_thAPduInstance, u16offset, E_ZCL_UINT8,  &psPayload->psEnergyPhaseDelay[i].u8EnergyPhaseId);
        u16offset += u16ZCL_APduInstanceWriteNBO(
        myPDUM_thAPduInstance, u16offset, E_ZCL_UINT16, &psPayload->psEnergyPhaseDelay[i].u16ScheduleTime);
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

#ifdef PP_CLIENT
/****************************************************************************
 **
 ** NAME:       eCLD_PPGetOverallSchedulePriceReceive 
 **
 ** DESCRIPTION:
 ** handles rx of Get Overall Price Command
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
PUBLIC  teZCL_Status eCLD_PPGetOverallSchedulePriceReceive (
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
 ** NAME:       eCLD_PPPowerProfileScheduleConstraintsReceive    
 **
 ** DESCRIPTION:
 ** handles rx of Power Profile Request Command
 **
 ** PARAMETERS:                                      Name                           Usage
 ** ZPS_tsAfEvent                                    *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition                         *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance                            *psClusterInstance             Cluster structure
 ** uint8                                            *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_PP_PowerProfileScheduleConstraintsPayload  *psPayload                     Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_PPPowerProfileScheduleConstraintsReceive   (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_PP_PowerProfileScheduleConstraintsPayload *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
    {1,                 &u16ActualQuantity,             E_ZCL_UINT8,               &psPayload->u8PowerProfileId},
    {1,                 &u16ActualQuantity,             E_ZCL_UINT16,              &psPayload->u16StartAfter},
    {1,                 &u16ActualQuantity,             E_ZCL_UINT16,              &psPayload->u16StopBefore},
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT);

}

/****************************************************************************
 **
 ** NAME:       eCLD_PPGetPowerProfilePriceExtendedReceive    
 **
 ** DESCRIPTION:
 ** handles rx of Power Profile Request Command
 **
 ** PARAMETERS:                                      Name                           Usage
 ** ZPS_tsAfEvent                                    *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition                         *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance                            *psClusterInstance             Cluster structure
 ** uint8                                            *pu8TransactionSequenceNumber  Sequence number Pointer
 ** tsCLD_PP_GetPowerProfilePriceExtendedPayload     *psPayload                     Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_PPGetPowerProfilePriceExtendedReceive   (
                    ZPS_tsAfEvent                                   *pZPSevent,
                    uint8                                           *pu8TransactionSequenceNumber,
                    tsCLD_PP_GetPowerProfilePriceExtendedPayload    *psPayload)
{

    uint16 u16ActualQuantity;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
    {1,                 &u16ActualQuantity,             E_ZCL_BMAP8,               &psPayload->u8Options},
    {1,                 &u16ActualQuantity,             E_ZCL_UINT8,               &psPayload->u8PowerProfileId},
    {1,                 &u16ActualQuantity,             E_ZCL_UINT16,              &psPayload->u16PowerProfileStartTime},
                                                };

    return eZCL_CustomCommandReceive(pZPSevent,
                                     pu8TransactionSequenceNumber,
                                     asPayloadDefinition,
                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                     E_ZCL_ACCEPT_EXACT);

}

/****************************************************************************
 **
 ** NAME:       eCLD_PPPowerProfileStateReqSend 
 **
 ** DESCRIPTION:
 ** Builds and sends a Power Profile State Request
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
PUBLIC  teZCL_Status eCLD_PPPowerProfileStateReqSend  (
                                        uint8                                                       u8SourceEndPointId,
                                        uint8                                                       u8DestinationEndPointId,
                                        tsZCL_Address                                               *psDestinationAddress,
                                        uint8                                                       *pu8TransactionSequenceNumber)
{

                                                 
        return eZCL_CustomCommandSend(u8SourceEndPointId,
                                      u8DestinationEndPointId,
                                      psDestinationAddress,
                                      GENERAL_CLUSTER_ID_PP,
                                      FALSE,
                                      E_CLD_PP_CMD_POWER_PROFILE_STATE_REQ,
                                      pu8TransactionSequenceNumber,
                                      0,
                                      FALSE,
                                      0,
                                      0
                                     );

}

/****************************************************************************
 **
 ** NAME:       eCLD_PPGetPowerProfilePriceOrExtendedRspSend  
 **
 ** DESCRIPTION:
 ** Builds and sends a Power Profile price Response Command
 ** PARAMETERS:                                           Name                               Usage
 ** uint8                                                 u8SourceEndPointId                 Source EP Id
 ** uint8                                                 u8DestinationEndPointId            Destination EP Id
 ** tsZCL_Address                                         *psDestinationAddress              Destination Address
 ** uint8                                                 *pu8TransactionSequenceNumber      Sequence number Pointer
 ** teCLD_PP_ServerReceivedCommandID                      eCommandId                         Command Id 
 ** tsCLD_PP_GetPowerProfilePriceRspPayload              *psPayload                         Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_PPGetPowerProfilePriceOrExtendedRspSend  (
                                        uint8                                                       u8SourceEndPointId,
                                        uint8                                                       u8DestinationEndPointId,
                                        tsZCL_Address                                               *psDestinationAddress,
                                        uint8                                                       *pu8TransactionSequenceNumber,
                                        teCLD_PP_ServerReceivedCommandID                            eCommandId,
                                        tsCLD_PP_GetPowerProfilePriceRspPayload                    *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
    
        {1,                              E_ZCL_UINT8,                   &psPayload->u8PowerProfileId},
        {1,                              E_ZCL_UINT16,                  &psPayload->u16Currency},
        {1,                              E_ZCL_UINT32,                  &psPayload->u32Price},
        {1,                              E_ZCL_UINT8,                   &psPayload->u8PriceTrailingDigits},
                                                 };
                                                 
        return eZCL_CustomCommandSend(u8SourceEndPointId,
                                      u8DestinationEndPointId,
                                      psDestinationAddress,
                                      GENERAL_CLUSTER_ID_PP,
                                      FALSE,
                                      eCommandId,
                                      pu8TransactionSequenceNumber,
                                      asPayloadDefinition,
                                      FALSE,
                                      0,
                                      sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem)
                                     );

}

/****************************************************************************
 **
 ** NAME:       eCLD_PPGetOverallSchedulePriceRspSend   
 **
 ** DESCRIPTION:
 ** Builds and sends a Power Profile price Response Command
 ** PARAMETERS:                                           Name                               Usage
 ** uint8                                                 u8SourceEndPointId                 Source EP Id
 ** uint8                                                 u8DestinationEndPointId            Destination EP Id
 ** tsZCL_Address                                         *psDestinationAddress              Destination Address
 ** uint8                                                 *pu8TransactionSequenceNumber      Sequence number Pointer
 ** tsCLD_PP_GetOverallSchedulePriceRspPayload           *psPayload                         Payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_PPGetOverallSchedulePriceRspSend(
                                        uint8                                                       u8SourceEndPointId,
                                        uint8                                                       u8DestinationEndPointId,
                                        tsZCL_Address                                               *psDestinationAddress,
                                        uint8                                                       *pu8TransactionSequenceNumber,
                                        tsCLD_PP_GetOverallSchedulePriceRspPayload                 *psPayload)
{

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
    
        {1,                              E_ZCL_UINT16,                  &psPayload->u16Currency},
        {1,                              E_ZCL_UINT32,                  &psPayload->u32Price},
        {1,                              E_ZCL_UINT8,                   &psPayload->u8PriceTrailingDigits},
                                                 };
                                                 
        return eZCL_CustomCommandSend(u8SourceEndPointId,
                                      u8DestinationEndPointId,
                                      psDestinationAddress,
                                      GENERAL_CLUSTER_ID_PP,
                                      FALSE,
                                      E_CLD_PP_CMD_GET_OVERALL_SCHEDULE_PRICE_RSP,
                                      pu8TransactionSequenceNumber,
                                      asPayloadDefinition,
                                      FALSE,
                                      0,
                                      sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem)
                                     );

}

#endif
/****************************************************************************/
/***        Private Functions                                             ***/
/****************************************************************************/
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
