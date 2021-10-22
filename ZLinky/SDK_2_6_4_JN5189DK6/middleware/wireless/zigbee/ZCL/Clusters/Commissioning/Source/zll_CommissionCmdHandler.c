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
 * MODULE:             Light Link Profile
 *
 * COMPONENT:          zll_CommissionCmdHandler.c
 *
 * DESCRIPTION:        ZigBee Light Link profile functions
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "zps_apl.h"
#include "zcl_heap.h"
#include "zcl.h"
#include "zcl_options.h"
#include "zll_commission.h"



#include "dbg.h"


/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#if defined(CLD_ZLL_COMMISSION) && !defined(ZLL_COMMISSION_SERVER) && !defined(ZLL_COMMISSION_CLIENT)
#error You Must Have either ZLL_COMMISSION_SERVER and/or ZLL_COMMISSION_CLIENT defined in zcl_options.h
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
#ifdef CLD_ZLL_COMMISSION

#ifdef ZLL_COMMISSION_CLIENT
PRIVATE teZCL_Status eCLD_ZllCommissionHandleScanRspCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint16                      u16Offset);
PRIVATE teZCL_Status eCLD_ZllCommissionHandleDeviceInfoRspCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint16                      u16Offset);
PRIVATE teZCL_Status eCLD_ZllCommissionHandleNwkStartRspCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint16                      u16Offset);
PRIVATE teZCL_Status eCLD_ZllCommissionHandleNwkJoinRouterRspCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint16                      u16Offset);
PRIVATE teZCL_Status eCLD_ZllCommissionHandleNwkJoinEndDeviceRspCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint16                      u16Offset);
#endif

#ifdef ZLL_COMMISSION_SERVER
PRIVATE teZCL_Status eCLD_ZllCommissionHandleScanReqCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint16                      u16Offset);
PRIVATE teZCL_Status eCLD_ZllCommissionHandleDeviceInfoReqCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint16                      u16Offset);
PRIVATE teZCL_Status eCLD_ZllCommissionHandleNwkStartReqCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint16                      u16Offset);
PRIVATE teZCL_Status eCLD_ZllCommissionHandleNwkJoinRouterReqCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint16                      u16Offset);
PRIVATE teZCL_Status eCLD_ZllCommissionHandleNwkJoinEndDeviceReqCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint16                      u16Offset);

PRIVATE teZCL_Status eCLD_ZllCommissionHandleDeviceIdentifyReqCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint16                      u16Offset);
PRIVATE teZCL_Status eCLD_ZllCommissionHandleFactoryResetReqCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint16                      u16Offset);
PRIVATE teZCL_Status eCLD_ZllCommissionHandleNwkUpdateReqCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint16                      u16Offset);
#endif

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:     eCLD_ZllCommissionCommandHandler
 **
 ** DESCRIPTION:
 ** sets address, based on Mode
 **
 ** PARAMETERS:                 Name                      Usage
 ** ZPS_tsAfEvent               *pZPSevent                Zigbee stack event structure
 ** tsZCL_EndPointDefinition    *psEndPointDefinition,    pointer to endpoint definition
 ** tsZCL_ClusterInstance       *psClusterInstance,       pointer to cluster instance
 **
 ** RETURN:
 ** status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ZllCommissionCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{
	tsZCL_HeaderParams sZCL_HeaderParams;
    uint16 u16Offset;
    tsCLD_ZllCommissionCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ZllCommissionCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    // further error checking can only be done once we have interrogated the ZCL payload
    u16Offset = u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsInterPanDataIndEvent.hAPduInst,
                             &sZCL_HeaderParams);

    if (sZCL_HeaderParams.u8CommandIdentifier >= E_CLD_UTILITY_CMD_ENDPOINT_INFO)
    {
        return eCLD_ZllUtilityCommandHandler(pZPSevent,
                psEndPointDefinition,
                psClusterInstance);
    }



    /* Inter Pan */
    psCommon->sRxInterPanAddr.bInitialised = TRUE;
    psCommon->sRxInterPanAddr.sSrcAddr.eMode  = ZPS_E_AM_INTERPAN_IEEE;
    psCommon->sRxInterPanAddr.sSrcAddr.u16PanId = pZPSevent->uEvent.sApsInterPanDataIndEvent.u16SrcPan;
    psCommon->sRxInterPanAddr.sSrcAddr.uAddress.u64Addr = pZPSevent->uEvent.sApsInterPanDataIndEvent.u64SrcAddress;
    psCommon->sRxInterPanAddr.sDstAddr = pZPSevent->uEvent.sApsInterPanDataIndEvent.sDstAddr;

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psCommon->sCustomCallBackEvent, pZPSevent, sZCL_HeaderParams.u8TransactionSequenceNumber, psEndPointDefinition->u8EndPointNumber);

    /* Fill in message */
    psCommon->sCallBackMessage.u8CommandId = sZCL_HeaderParams.u8CommandIdentifier;

    /* Handle messages appropriate for the cluster type (Client/Server) */
    if(psClusterInstance->bIsServer)
    {
#ifdef ZLL_COMMISSION_SERVER
        // SERVER
        switch(sZCL_HeaderParams.u8CommandIdentifier)
        {
            case E_CLD_COMMISSION_CMD_SCAN_REQ:
                eCLD_ZllCommissionHandleScanReqCommand(pZPSevent, psEndPointDefinition, psClusterInstance, u16Offset);
                break;

            case E_CLD_COMMISSION_CMD_DEVICE_INFO_REQ:
                eCLD_ZllCommissionHandleDeviceInfoReqCommand(pZPSevent, psEndPointDefinition, psClusterInstance, u16Offset);
                break;

            case E_CLD_COMMISSION_CMD_IDENTIFY_REQ:
                eCLD_ZllCommissionHandleDeviceIdentifyReqCommand(pZPSevent, psEndPointDefinition, psClusterInstance, u16Offset);
                break;

            case E_CLD_COMMISSION_CMD_FACTORY_RESET_REQ:
                eCLD_ZllCommissionHandleFactoryResetReqCommand(pZPSevent, psEndPointDefinition, psClusterInstance, u16Offset);
                break;

            case E_CLD_COMMISSION_CMD_NETWORK_START_REQ:
                eCLD_ZllCommissionHandleNwkStartReqCommand(pZPSevent, psEndPointDefinition, psClusterInstance, u16Offset);
                break;

            case E_CLD_COMMISSION_CMD_NETWORK_JOIN_ROUTER_REQ:
                eCLD_ZllCommissionHandleNwkJoinRouterReqCommand(pZPSevent, psEndPointDefinition, psClusterInstance, u16Offset);
                break;

            case E_CLD_COMMISSION_CMD_NETWORK_JOIN_END_DEVICE_REQ:
                eCLD_ZllCommissionHandleNwkJoinEndDeviceReqCommand(pZPSevent, psEndPointDefinition, psClusterInstance, u16Offset);
                break;

            case E_CLD_COMMISSION_CMD_NETWORK_UPDATE_REQ:
                eCLD_ZllCommissionHandleNwkUpdateReqCommand(pZPSevent, psEndPointDefinition, psClusterInstance, u16Offset);
                break;

            default:
            {
                // unlock
                //DBG_vPrintf(TRUE, "Zll unhandled server %d\n", u8CommandIdentifier);
                #ifndef COOPERATIVE
                    eZCL_ReleaseMutex(psEndPointDefinition);
                #endif

                return(E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR);
                break;
            }
        }
#endif
    }
    else
    {
#ifdef ZLL_COMMISSION_CLIENT
        // CLIENT
        switch(sZCL_HeaderParams.u8CommandIdentifier)
        {
            case E_CLD_COMMISSION_CMD_SCAN_RSP:
                eCLD_ZllCommissionHandleScanRspCommand(pZPSevent, psEndPointDefinition, psClusterInstance, u16Offset);
                break;
            case E_CLD_COMMISSION_CMD_DEVICE_INFO_RSP:
                eCLD_ZllCommissionHandleDeviceInfoRspCommand(pZPSevent, psEndPointDefinition, psClusterInstance, u16Offset);
                break;
            case E_CLD_COMMISSION_CMD_NETWORK_START_RSP:
                eCLD_ZllCommissionHandleNwkStartRspCommand(pZPSevent, psEndPointDefinition, psClusterInstance, u16Offset);
                break;
            case E_CLD_COMMISSION_CMD_NETWORK_JOIN_ROUTER_RSP:
                eCLD_ZllCommissionHandleNwkJoinRouterRspCommand(pZPSevent, psEndPointDefinition, psClusterInstance, u16Offset);
                break;
            case E_CLD_COMMISSION_CMD_NETWORK_JOIN_END_DEVICE_RSP:
                eCLD_ZllCommissionHandleNwkJoinEndDeviceRspCommand(pZPSevent, psEndPointDefinition, psClusterInstance, u16Offset);
                break;

            default:
            {
                // unlock
                //DBG_vPrintf(TRUE, "Zll unhandled client %d\n", u8CommandIdentifier);
                #ifndef COOPERATIVE
                    eZCL_ReleaseMutex(psEndPointDefinition);
                #endif

                return(E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR);
                break;
            }
        }
#endif
    }


    // unlock
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    // delete the i/p buffer on return
    return(E_ZCL_SUCCESS);
}


/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:     eCLD_ZllCommissionHandleScanRspCommand
 **
 ** DESCRIPTION:
 ** sets address, based on Mode
 **
 ** PARAMETERS:                 Name                      Usage
 ** ZPS_tsAfEvent               *pZPSevent                Zigbee stack event structure
 ** tsZCL_EndPointDefinition    *psEndPointDefinition,    pointer to endpoint definition
 ** tsZCL_ClusterInstance       *psClusterInstance,       pointer to cluster instance
 ** uint16                      u16Offset                 start of Zll data in apdu
 **
 ** RETURN:
 ** status
 **
 ****************************************************************************/
#ifdef ZLL_COMMISSION_CLIENT
PRIVATE  teZCL_Status eCLD_ZllCommissionHandleScanRspCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint16                      u16Offset)
{

    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsCLD_ZllCommission_ScanRspCommandPayload sPayload;
    uint16 u16ActualQuantity;
    //uint8 u8Id;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {1, &u16ActualQuantity, E_ZCL_UINT32,  &sPayload.u32TransactionId},
        {1, &u16ActualQuantity, E_ZCL_UINT8,  &sPayload.u8RSSICorrection},
        {1, &u16ActualQuantity, E_ZCL_UINT8,  &sPayload.u8ZigbeeInfo},
        {1, &u16ActualQuantity, E_ZCL_UINT8,  &sPayload.u8ZllInfo},
        {1, &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16KeyMask},
        {1, &u16ActualQuantity, E_ZCL_UINT32,  &sPayload.u32ResponseId},
        {1, &u16ActualQuantity, E_ZCL_UINT64,  &sPayload.u64ExtPanId},
        {1, &u16ActualQuantity, E_ZCL_UINT8,  &sPayload.u8NwkUpdateId},
        {1, &u16ActualQuantity, E_ZCL_UINT8,  &sPayload.u8LogicalChannel},
        {1, &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16PanId},
        {1, &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16NwkAddr},
        {1, &u16ActualQuantity, E_ZCL_UINT8,  &sPayload.u8NumberSubDevices},
        {1, &u16ActualQuantity, E_ZCL_UINT8,  &sPayload.u8TotalGroupIds},
        {1, &u16ActualQuantity, E_ZCL_UINT8,  &sPayload.u8Endpoint},
        {1, &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16ProfileId},
        {1, &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16DeviceId},
        {1, &u16ActualQuantity, E_ZCL_UINT8,  &sPayload.u8Version},
        {1, &u16ActualQuantity, E_ZCL_UINT8,  &sPayload.u8GroupIdCount},

    };

    eStatus = eZCL_CustomCommandInterPanReceive(pZPSevent,
                              asPayloadDefinition,
                              sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                              u16Offset,
                              E_ZCL_ACCEPT_EXACT);

    if (eStatus != E_ZCL_SUCCESS) {
        return eStatus;
    }

    tsCLD_ZllCommissionCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ZllCommissionCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psScanRspPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}

PRIVATE  teZCL_Status eCLD_ZllCommissionHandleDeviceInfoRspCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint16                      u16Offset){

    tsCLD_ZllCommission_DeviceInfoRspCommandPayload sPayload;
    int i;

    u16Offset += PDUM_u16APduInstanceReadNBO(pZPSevent->uEvent.sApsInterPanDataIndEvent.hAPduInst, u16Offset, "w",
                        &sPayload.u32TransactionId);
    u16Offset += PDUM_u16APduInstanceReadNBO(pZPSevent->uEvent.sApsInterPanDataIndEvent.hAPduInst, u16Offset, "b",
                            &sPayload.u8NumberSubDevices);
    u16Offset += PDUM_u16APduInstanceReadNBO(pZPSevent->uEvent.sApsInterPanDataIndEvent.hAPduInst, u16Offset, "b",
                            &sPayload.u8StartIndex);
    u16Offset += PDUM_u16APduInstanceReadNBO(pZPSevent->uEvent.sApsInterPanDataIndEvent.hAPduInst, u16Offset, "b",
                            &sPayload.u8DeviceInfoRecordCount);
    for (i=0; i<TL_MAX_DEVICE_RECORDS && i<sPayload.u8DeviceInfoRecordCount; i++) {
        u16Offset += PDUM_u16APduInstanceReadNBO(pZPSevent->uEvent.sApsInterPanDataIndEvent.hAPduInst, u16Offset, "l",
                                    &sPayload.asDeviceRecords[i].u64IEEEAddr);
        u16Offset += PDUM_u16APduInstanceReadNBO(pZPSevent->uEvent.sApsInterPanDataIndEvent.hAPduInst, u16Offset, "b",
                                            &sPayload.asDeviceRecords[i].u8Endpoint);
        u16Offset += PDUM_u16APduInstanceReadNBO(pZPSevent->uEvent.sApsInterPanDataIndEvent.hAPduInst, u16Offset, "h",
                                            &sPayload.asDeviceRecords[i].u16ProfileId);
        u16Offset += PDUM_u16APduInstanceReadNBO(pZPSevent->uEvent.sApsInterPanDataIndEvent.hAPduInst, u16Offset, "h",
                                            &sPayload.asDeviceRecords[i].u16DeviceId);
        u16Offset += PDUM_u16APduInstanceReadNBO(pZPSevent->uEvent.sApsInterPanDataIndEvent.hAPduInst, u16Offset, "b",
                                            &sPayload.asDeviceRecords[i].u8Version);
        u16Offset += PDUM_u16APduInstanceReadNBO(pZPSevent->uEvent.sApsInterPanDataIndEvent.hAPduInst, u16Offset, "b",
                                            &sPayload.asDeviceRecords[i].u8NumberGroupIds);
        u16Offset += PDUM_u16APduInstanceReadNBO(pZPSevent->uEvent.sApsInterPanDataIndEvent.hAPduInst, u16Offset, "b",
                                            &sPayload.asDeviceRecords[i].u8Sort);
    }

    tsCLD_ZllCommissionCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ZllCommissionCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psDeviceInfoRspPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);


    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:    eCLD_ZllCommissionHandleNwkStartRspCommand
 **
 ** DESCRIPTION:
 ** sets address, based on Mode
 **
 ** PARAMETERS:                 Name                      Usage
 ** ZPS_tsAfEvent               *pZPSevent                Zigbee stack event structure
 ** tsZCL_EndPointDefinition    *psEndPointDefinition,    pointer to endpoint definition
 ** tsZCL_ClusterInstance       *psClusterInstance,       pointer to cluster instance
 ** uint16                      u16Offset                 start of Zll data in apdu
 **
 ** RETURN:
 ** status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ZllCommissionHandleNwkStartRspCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint16                      u16Offset){

    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsCLD_ZllCommission_NetworkStartRspCommandPayload sPayload;

    uint16 u16ActualQuantity;


    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {1, &u16ActualQuantity, E_ZCL_UINT32, &sPayload.u32TransactionId},
        {1, &u16ActualQuantity, E_ZCL_UINT8,  &sPayload.u8Status},
        {1, &u16ActualQuantity, E_ZCL_UINT64, &sPayload.u64ExtPanId},
        {1, &u16ActualQuantity, E_ZCL_UINT8,  &sPayload.u8NwkUpdateId},
        {1, &u16ActualQuantity, E_ZCL_UINT8,  &sPayload.u8LogicalChannel},
        {1, &u16ActualQuantity, E_ZCL_UINT16, &sPayload.u16PanId}
    };

    eStatus = eZCL_CustomCommandInterPanReceive(pZPSevent,
                              asPayloadDefinition,
                              sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                              u16Offset,
                              E_ZCL_ACCEPT_EXACT);

    if (eStatus != E_ZCL_SUCCESS) {
        return eStatus;
    }

    tsCLD_ZllCommissionCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ZllCommissionCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psNwkStartRspPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:    eCLD_ZllCommissionHandleNwkJoinRouterRspCommand
 **
 ** DESCRIPTION:
 ** sets address, based on Mode
 **
 ** PARAMETERS:                 Name                      Usage
 ** ZPS_tsAfEvent               *pZPSevent                Zigbee stack event structure
 ** tsZCL_EndPointDefinition    *psEndPointDefinition,    pointer to endpoint definition
 ** tsZCL_ClusterInstance       *psClusterInstance,       pointer to cluster instance
 ** uint16                      u16Offset                 start of Zll data in apdu
 **
 ** RETURN:
 ** status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ZllCommissionHandleNwkJoinRouterRspCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint16                      u16Offset){

    tsCLD_ZllCommission_NetworkJoinRouterRspCommandPayload sPayload;


    u16Offset += PDUM_u16APduInstanceReadNBO(pZPSevent->uEvent.sApsInterPanDataIndEvent.hAPduInst, u16Offset, "w",
                        &sPayload.u32TransactionId);
    u16Offset += PDUM_u16APduInstanceReadNBO(pZPSevent->uEvent.sApsInterPanDataIndEvent.hAPduInst, u16Offset, "b",
                            &sPayload.u8Status);

    tsCLD_ZllCommissionCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ZllCommissionCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psNwkJoinRouterRspPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:    eCLD_ZllCommissionHandleNwkJoinEndDeviceRspCommand
 **
 ** DESCRIPTION:
 ** sets address, based on Mode
 **
 ** PARAMETERS:                 Name                      Usage
 ** ZPS_tsAfEvent               *pZPSevent                Zigbee stack event structure
 ** tsZCL_EndPointDefinition    *psEndPointDefinition,    pointer to endpoint definition
 ** tsZCL_ClusterInstance       *psClusterInstance,       pointer to cluster instance
 ** uint16                      u16Offset                 start of Zll data in apdu
 **
 ** RETURN:
 ** status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ZllCommissionHandleNwkJoinEndDeviceRspCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint16                      u16Offset){

    tsCLD_ZllCommission_NetworkJoinEndDeviceRspCommandPayload sPayload;

    u16Offset += PDUM_u16APduInstanceReadNBO(pZPSevent->uEvent.sApsInterPanDataIndEvent.hAPduInst, u16Offset, "w",
                        &sPayload.u32TransactionId);
    u16Offset += PDUM_u16APduInstanceReadNBO(pZPSevent->uEvent.sApsInterPanDataIndEvent.hAPduInst, u16Offset, "b",
                            &sPayload.u8Status);

    tsCLD_ZllCommissionCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ZllCommissionCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psNwkJoinEndDeviceRspPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    return E_ZCL_SUCCESS;
}

#endif


/****************************************************************************
 **
 ** NAME:    eCLD_ZllCommissionHandleScanReqCommand
 **
 ** DESCRIPTION:
 ** sets address, based on Mode
 **
 ** PARAMETERS:                 Name                      Usage
 ** ZPS_tsAfEvent               *pZPSevent                Zigbee stack event structure
 ** tsZCL_EndPointDefinition    *psEndPointDefinition,    pointer to endpoint definition
 ** tsZCL_ClusterInstance       *psClusterInstance,       pointer to cluster instance
 ** uint16                      u16Offset                 start of Zll data in apdu
 **
 ** RETURN:
 ** status
 **
 ****************************************************************************/
#ifdef ZLL_COMMISSION_SERVER
PRIVATE  teZCL_Status eCLD_ZllCommissionHandleScanReqCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint16                      u16Offset)
{

    tsCLD_ZllCommission_ScanReqCommandPayload sPayload;

    u16Offset += PDUM_u16APduInstanceReadNBO(pZPSevent->uEvent.sApsInterPanDataIndEvent.hAPduInst, u16Offset, "w",
                        &sPayload.u32TransactionId);
    u16Offset += PDUM_u16APduInstanceReadNBO(pZPSevent->uEvent.sApsInterPanDataIndEvent.hAPduInst, u16Offset, "b",
                            &sPayload.u8ZigbeeInfo);
    u16Offset += PDUM_u16APduInstanceReadNBO(pZPSevent->uEvent.sApsInterPanDataIndEvent.hAPduInst, u16Offset, "b",
                                &sPayload.u8ZllInfo);

    tsCLD_ZllCommissionCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ZllCommissionCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psScanReqPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:     eCLD_ZllCommissionHandleDeviceInfoReqCommand
 **
 ** DESCRIPTION:
 ** sets address, based on Mode
 **
 ** PARAMETERS:                 Name                      Usage
 ** ZPS_tsAfEvent               *pZPSevent                Zigbee stack event structure
 ** tsZCL_EndPointDefinition    *psEndPointDefinition,    pointer to endpoint definition
 ** tsZCL_ClusterInstance       *psClusterInstance,       pointer to cluster instance
 ** uint16                      u16Offset                 start of Zll data in apdu
 **
 ** RETURN:
 ** status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ZllCommissionHandleDeviceInfoReqCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint16                      u16Offset)
{
    tsCLD_ZllCommission_DeviceInfoReqCommandPayload sPayload;

    u16Offset += PDUM_u16APduInstanceReadNBO(pZPSevent->uEvent.sApsInterPanDataIndEvent.hAPduInst, u16Offset, "w",
                        &sPayload.u32TransactionId);
    u16Offset += PDUM_u16APduInstanceReadNBO(pZPSevent->uEvent.sApsInterPanDataIndEvent.hAPduInst, u16Offset, "b",
                            &sPayload.u8StartIndex);

    tsCLD_ZllCommissionCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ZllCommissionCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psDeviceInfoReqPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}


/****************************************************************************
 **
 ** NAME:    eCLD_ZllCommissionHandleNwkStartReqCommand
 **
 ** DESCRIPTION:
 ** sets address, based on Mode
 **
 ** PARAMETERS:                 Name                      Usage
 ** ZPS_tsAfEvent               *pZPSevent                Zigbee stack event structure
 ** tsZCL_EndPointDefinition    *psEndPointDefinition,    pointer to endpoint definition
 ** tsZCL_ClusterInstance       *psClusterInstance,       pointer to cluster instance
 ** uint16                      u16Offset                 start of Zll data in apdu
 **
 ** RETURN:
 ** status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ZllCommissionHandleNwkStartReqCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint16                      u16Offset)
{
    tsCLD_ZllCommission_NetworkStartReqCommandPayload sPayload;
    teZCL_Status eStatus = E_ZCL_SUCCESS;
    uint16 u16ActualQuantity;


    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {1,  &u16ActualQuantity, E_ZCL_UINT32,  &sPayload.u32TransactionId},
        {1,  &u16ActualQuantity, E_ZCL_UINT64,  &sPayload.u64ExtPanId},
        {1,  &u16ActualQuantity, E_ZCL_UINT8,   &sPayload.u8KeyIndex},
        {16, &u16ActualQuantity, E_ZCL_UINT8,   &sPayload.au8NwkKey},
        {1,  &u16ActualQuantity, E_ZCL_UINT8,   &sPayload.u8LogicalChannel},
        {1,  &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16PanId},
        {1,  &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16NwkAddr},
        {1,  &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16GroupIdBegin},
        {1,  &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16GroupIdEnd},
        {1,  &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16FreeNwkAddrBegin},
        {1,  &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16FreeNwkAddrEnd},
        {1,  &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16FreeGroupIdBegin},
        {1,  &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16FreeGroupIdEnd},
        {1,  &u16ActualQuantity, E_ZCL_UINT64,  &sPayload.u64InitiatorIEEEAddr},
        {1,  &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16InitiatorNwkAddr}
    };

    eStatus = eZCL_CustomCommandInterPanReceive(pZPSevent,
                              asPayloadDefinition,
                              sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                              u16Offset,
                              E_ZCL_ACCEPT_EXACT);

    if (eStatus != E_ZCL_SUCCESS) {
        return eStatus;
    }

    tsCLD_ZllCommissionCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ZllCommissionCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psNwkStartReqPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:     eCLD_ZllCommissionHandleNwkJoinRouterReqCommand
 **
 ** DESCRIPTION:
 ** sets address, based on Mode
 **
 ** PARAMETERS:                 Name                      Usage
 ** ZPS_tsAfEvent               *pZPSevent                Zigbee stack event structure
 ** tsZCL_EndPointDefinition    *psEndPointDefinition,    pointer to endpoint definition
 ** tsZCL_ClusterInstance       *psClusterInstance,       pointer to cluster instance
 ** uint16                      u16Offset                 start of Zll data in apdu
 **
 ** RETURN:
 ** status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ZllCommissionHandleNwkJoinRouterReqCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint16                      u16Offset)
{
    tsCLD_ZllCommission_NetworkJoinRouterReqCommandPayload sPayload;
    uint16 u16ActualQuantity;
    teZCL_Status eStatus = E_ZCL_SUCCESS;


    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {1,  &u16ActualQuantity, E_ZCL_UINT32,  &sPayload.u32TransactionId},
        {1,  &u16ActualQuantity, E_ZCL_UINT64,  &sPayload.u64ExtPanId},
        {1,  &u16ActualQuantity, E_ZCL_UINT8,   &sPayload.u8KeyIndex},
        {16, &u16ActualQuantity, E_ZCL_UINT8,   &sPayload.au8NwkKey},
        {1,  &u16ActualQuantity, E_ZCL_UINT8,   &sPayload.u8NwkUpdateId},
        {1,  &u16ActualQuantity, E_ZCL_UINT8,   &sPayload.u8LogicalChannel},
        {1,  &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16PanId},
        {1,  &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16NwkAddr},
        {1,  &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16GroupIdBegin},
        {1,  &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16GroupIdEnd},
        {1,  &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16FreeNwkAddrBegin},
        {1,  &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16FreeNwkAddrEnd},
        {1,  &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16FreeGroupIdBegin},
        {1,  &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16FreeGroupIdEnd}
    };

    eStatus = eZCL_CustomCommandInterPanReceive(pZPSevent,
                              asPayloadDefinition,
                              sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                              u16Offset,
                              E_ZCL_ACCEPT_EXACT);

    if (eStatus != E_ZCL_SUCCESS) {
        return eStatus;
    }

    tsCLD_ZllCommissionCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ZllCommissionCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psNwkJoinRouterReqPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}


/****************************************************************************
 **
 ** NAME:    eCLD_ZllCommissionHandleNwkJoinEndDeviceReqCommand
 **
 ** DESCRIPTION:
 ** sets address, based on Mode
 **
 ** PARAMETERS:                 Name                      Usage
 ** ZPS_tsAfEvent               *pZPSevent                Zigbee stack event structure
 ** tsZCL_EndPointDefinition    *psEndPointDefinition,    pointer to endpoint definition
 ** tsZCL_ClusterInstance       *psClusterInstance,       pointer to cluster instance
 ** uint16                      u16Offset                 start of Zll data in apdu
 **
 ** RETURN:
 ** status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ZllCommissionHandleNwkJoinEndDeviceReqCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint16                      u16Offset)
{
    tsCLD_ZllCommission_NetworkJoinEndDeviceReqCommandPayload sPayload;
    uint16 u16ActualQuantity;
    teZCL_Status eStatus = E_ZCL_SUCCESS;


    tsZCL_RxPayloadItem asPayloadDefinition[] = {
        {1,  &u16ActualQuantity, E_ZCL_UINT32,  &sPayload.u32TransactionId},
        {1,  &u16ActualQuantity, E_ZCL_UINT64,  &sPayload.u64ExtPanId},
        {1,  &u16ActualQuantity, E_ZCL_UINT8,   &sPayload.u8KeyIndex},
        {16, &u16ActualQuantity, E_ZCL_UINT8,   &sPayload.au8NwkKey},
        {1,  &u16ActualQuantity, E_ZCL_UINT8,   &sPayload.u8NwkUpdateId},
        {1,  &u16ActualQuantity, E_ZCL_UINT8,   &sPayload.u8LogicalChannel},
        {1,  &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16PanId},
        {1,  &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16NwkAddr},
        {1,  &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16GroupIdBegin},
        {1,  &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16GroupIdEnd},
        {1,  &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16FreeNwkAddrBegin},
        {1,  &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16FreeNwkAddrEnd},
        {1,  &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16FreeGroupIdBegin},
        {1,  &u16ActualQuantity, E_ZCL_UINT16,  &sPayload.u16FreeGroupIdEnd}
    };

    eStatus = eZCL_CustomCommandInterPanReceive(pZPSevent,
                              asPayloadDefinition,
                              sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                              u16Offset,
                              E_ZCL_ACCEPT_EXACT);

    if (eStatus != E_ZCL_SUCCESS) {
        return eStatus;
    }

    tsCLD_ZllCommissionCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ZllCommissionCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psNwkJoinEndDeviceReqPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eCLD_ZllCommissionHandleDeviceIdentifyReqCommand
 **
 ** DESCRIPTION:
 ** sets address, based on Mode
 **
 ** PARAMETERS:                 Name                      Usage
 ** ZPS_tsAfEvent               *pZPSevent                Zigbee stack event structure
 ** tsZCL_EndPointDefinition    *psEndPointDefinition,    pointer to endpoint definition
 ** tsZCL_ClusterInstance       *psClusterInstance,       pointer to cluster instance
 ** uint16                      u16Offset                 start of Zll data in apdu
 **
 ** RETURN:
 ** status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ZllCommissionHandleDeviceIdentifyReqCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint16                      u16Offset)
{
    tsCLD_ZllCommission_IdentifyReqCommandPayload sPayload;

    u16Offset += PDUM_u16APduInstanceReadNBO(pZPSevent->uEvent.sApsInterPanDataIndEvent.hAPduInst, u16Offset, "w",
                        &sPayload.u32TransactionId);
    u16Offset += PDUM_u16APduInstanceReadNBO(pZPSevent->uEvent.sApsInterPanDataIndEvent.hAPduInst, u16Offset, "h",
                            &sPayload.u16Duration);

    tsCLD_ZllCommissionCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_ZllCommissionCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psIdentifyReqPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}


/****************************************************************************
 **
 ** NAME:  eCLD_ZllCommissionHandleFactoryResetReqCommand
 **
 ** DESCRIPTION:
 ** sets address, based on Mode
 **
 ** PARAMETERS:                 Name                      Usage
 ** ZPS_tsAfEvent               *pZPSevent                Zigbee stack event structure
 ** tsZCL_EndPointDefinition    *psEndPointDefinition,    pointer to endpoint definition
 ** tsZCL_ClusterInstance       *psClusterInstance,       pointer to cluster instance
 ** uint16                      u16Offset                 start of Zll data in apdu
 **
 ** RETURN:
 ** status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ZllCommissionHandleFactoryResetReqCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint16                      u16Offset)
{
    tsCLD_ZllCommission_FactoryResetReqCommandPayload sPayload;

    u16Offset += PDUM_u16APduInstanceReadNBO(pZPSevent->uEvent.sApsInterPanDataIndEvent.hAPduInst, u16Offset, "w",
                        &sPayload.u32TransactionId);

   tsCLD_ZllCommissionCustomDataStructure *psCommon;

   /* Get pointer to custom data structure */
   psCommon = (tsCLD_ZllCommissionCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

   /* Message data for callback */
   psCommon->sCallBackMessage.uMessage.psFactoryResetPayload = &sPayload;

   /* call callback */
   psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}


/****************************************************************************
 **
 ** NAME:   teZCL_Status eCLD_ZllCommissionHandleNwkUpdateReqCommand
 **
 ** DESCRIPTION:
 ** sets address, based on Mode
 **
 ** PARAMETERS:                 Name                      Usage
 ** ZPS_tsAfEvent               *pZPSevent                Zigbee stack event structure
 ** tsZCL_EndPointDefinition    *psEndPointDefinition,    pointer to endpoint definition
 ** tsZCL_ClusterInstance       *psClusterInstance,       pointer to cluster instance
 ** uint16                      u16Offset                 start of Zll data in apdu
 **
 ** RETURN:
 ** status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ZllCommissionHandleNwkUpdateReqCommand(
        ZPS_tsAfEvent               *pZPSevent,
        tsZCL_EndPointDefinition    *psEndPointDefinition,
        tsZCL_ClusterInstance       *psClusterInstance,
        uint16                      u16Offset)
{

tsCLD_ZllCommission_NetworkUpdateReqCommandPayload sPayload;
uint16 u16ActualQuantity;
teZCL_Status eStatus = E_ZCL_SUCCESS;


   tsZCL_RxPayloadItem asPayloadDefinition[] = {
       {1,  &u16ActualQuantity, E_ZCL_UINT32,  &sPayload.u32TransactionId},
       {1,  &u16ActualQuantity, E_ZCL_UINT64,  &sPayload.u64ExtPanId},
       {1,  &u16ActualQuantity, E_ZCL_UINT8,   &sPayload.u8NwkUpdateId},
       {1,  &u16ActualQuantity, E_ZCL_UINT8,   &sPayload.u8LogicalChannel},
       {1,  &u16ActualQuantity, E_ZCL_UINT16,   &sPayload.u16PanId},
       {1,  &u16ActualQuantity, E_ZCL_UINT16,   &sPayload.u16NwkAddr}

   };

   eStatus = eZCL_CustomCommandInterPanReceive(pZPSevent,
                                 asPayloadDefinition,
                                 sizeof(asPayloadDefinition) / sizeof(tsZCL_RxPayloadItem),
                                 u16Offset,
                                 E_ZCL_ACCEPT_EXACT);


   if (eStatus != E_ZCL_SUCCESS) {
       return eStatus;
   }

   tsCLD_ZllCommissionCustomDataStructure *psCommon;

   /* Get pointer to custom data structure */
   psCommon = (tsCLD_ZllCommissionCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

   /* Message data for callback */
   psCommon->sCallBackMessage.uMessage.psNwkUpdateReqPayload = &sPayload;

   /* call callback */
   psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

   return E_ZCL_SUCCESS;
}

#endif
#endif /* CLD_ZLL_COMMISSION */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

