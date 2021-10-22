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
 * COMPONENT:          zll_commission.c
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


/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE void vWriteU16ToPayLoad(uint16 u16Data, uint8 *pu8Buffer);
PRIVATE void vWriteU32ToPayLoad(uint32 u32Data, uint8 *pu8Buffer);
PRIVATE void vWriteU64ToPayLoad(uint64 u64Data, uint8 *pu8Buffer);
PRIVATE void vWriteU8ArrayToPayLoad(uint8 *pu8Data, uint8 *pu8Buffer, uint8 u8Num);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
const tsZCL_AttributeDefinition asCLD_ZllCommissionClusterAttributeDefinitions[] = {
    {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,       (E_ZCL_AF_RD|E_ZCL_AF_GA),         E_ZCL_BMAP32,   (uint32)(&((tsCLD_ZllCommission*)(0))->u32FeatureMap),0},   /* Mandatory  */ 

    {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,  (E_ZCL_AF_RD|E_ZCL_AF_GA),        E_ZCL_UINT16,    (uint32)(&((tsCLD_ZllCommission*)(0))->u16ClusterRevision),0},   /* Mandatory  */
};

tsZCL_ClusterDefinition sCLD_ZllCommission = {
        COMMISSIONING_CLUSTER_ID_TL_COMMISSIONING,
        FALSE,
        E_ZCL_SECURITY_NETWORK,
        (sizeof(asCLD_ZllCommissionClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
        (tsZCL_AttributeDefinition*)asCLD_ZllCommissionClusterAttributeDefinitions,
        NULL
};

uint8 au8ZllCommissionAttributeControlBits[(sizeof(asCLD_ZllCommissionClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 **
 ** NAME:     eCLD_ZllCommissionCreateCommission
 **
 ** DESCRIPTION:
 ** Constructor
 **
 ** PARAMETERS:                 Name                      Usage
 ** tsZCL_ClusterInstance       *psClusterInstance,       pointer to cluster instance
 ** bool_t                      bIsServer,
 ** tsZCL_ClusterDefinition     *psClusterDefinition,
 ** void                        *pvSharedStructPtr,
 ** uint8                       *pu8AttributeControlBits,
 ** tsCLD_ZllCommissionCustomDataStructure  *psCustomDataStructure
 **
 ** RETURN:
 ** status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ZllCommissionCreateCommission(
                    tsZCL_ClusterInstance              *psClusterInstance,
                    bool_t                              bIsServer,
                    tsZCL_ClusterDefinition            *psClusterDefinition,
                    void                               *pvSharedStructPtr,
                    uint8                              *pu8AttributeControlBits,
                    tsCLD_ZllCommissionCustomDataStructure  *psCustomDataStructure)
{
    #ifdef STRICT_PARAM_CHECK 
        /* Parameter check */
        if((psClusterInstance==NULL) || (psCustomDataStructure==NULL))
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif

    // cluster data
    vZCL_InitializeClusterInstance(
                                   psClusterInstance, 
                                   bIsServer,
                                   psClusterDefinition,
                                   pvSharedStructPtr,
                                   pu8AttributeControlBits,
                                   psCustomDataStructure,
                                   eCLD_ZllCommissionCommandHandler);      

    psCustomDataStructure->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = psClusterDefinition->u16ClusterEnum;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void *)&psCustomDataStructure->sCallBackMessage;
    psCustomDataStructure->sCustomCallBackEvent.psClusterInstance = psClusterInstance;
    
    /* attributes to default */
    ((tsCLD_ZllCommission*)pvSharedStructPtr)->u32FeatureMap = CLD_ZLL_COMMISSION_FEATURE_MAP;
    ((tsCLD_ZllCommission*)pvSharedStructPtr)->u16ClusterRevision = CLD_ZLL_COMMISSION_CLUSTER_REVISION;

    //  register timer

    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:     eCLD_ZllCommissionCommandScanReqCommandSend
 **
 ** DESCRIPTION:
 **
 **
 ** PARAMETERS:                 Name                      Usage
 ** ZPS_tsInterPanAddress       *psDestinationAddress
 ** uint8                       *pu8TransactionSequenceNumber
 ** tsCLD_ZllCommission_ScanReqCommandPayload  *psPayload
 **
 ** RETURN:
 ** status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ZllCommissionCommandScanReqCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_ScanReqCommandPayload  *psPayload)
{
    uint8 au8Payload[7];
    /*
     * This is an Inter Pan Client Message
     */

    vWriteU32ToPayLoad( psPayload->u32TransactionId, au8Payload);
    au8Payload[4] = psPayload->u8ZigbeeInfo;
    au8Payload[5] = psPayload->u8ZllInfo;

    return eZCL_TransmitInterPanRequest(
            eTX_FROM_CLIENT|eTX_CLUSTER_SPECIFIC|eTX_DISABLE_DEFAULT_RSP,
            E_CLD_COMMISSION_CMD_SCAN_REQ,
            6,
            COMMISSIONING_CLUSTER_ID_TL_COMMISSIONING,
            ZLL_PROFILE_ID,
            0,
            psDestinationAddress,
            au8Payload,
            pu8TransactionSequenceNumber);


}

/****************************************************************************
 **
 ** NAME:     eCLD_ZllCommissionCommandScanRspCommandSend
 **
 ** DESCRIPTION:
 **
 **
 ** PARAMETERS:                 Name                      Usage
 ** ZPS_tsInterPanAddress       *psDestinationAddress
 ** uint8                       *pu8TransactionSequenceNumber
 ** tsCLD_ZllCommission_ScanRspCommandPayload  *psPayload
 **
 ** RETURN:
 ** status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ZllCommissionCommandScanRspCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_ScanRspCommandPayload *psPayload)
{
    uint8 au8Payload[37];
    uint8 u8PayloadSize;
    /*
     * This is an Inter Pan Server Message
     */

    vWriteU32ToPayLoad( psPayload->u32TransactionId, au8Payload);
    au8Payload[4] = psPayload->u8RSSICorrection;
    au8Payload[5] = psPayload->u8ZigbeeInfo;
    au8Payload[6] = psPayload->u8ZllInfo;
    vWriteU16ToPayLoad( psPayload->u16KeyMask, &au8Payload[7]);
    vWriteU32ToPayLoad( psPayload->u32ResponseId, &au8Payload[9]);
    vWriteU64ToPayLoad( psPayload->u64ExtPanId, &au8Payload[13]);
    au8Payload[21] = psPayload->u8NwkUpdateId;
    au8Payload[22] = psPayload->u8LogicalChannel;
    vWriteU16ToPayLoad( psPayload->u16PanId, &au8Payload[23]);
    vWriteU16ToPayLoad( psPayload->u16NwkAddr, &au8Payload[25]);
    au8Payload[27] = psPayload->u8NumberSubDevices;
    au8Payload[28] = psPayload->u8TotalGroupIds;
    u8PayloadSize = 29;
    if (psPayload->u8NumberSubDevices == 1) {
        au8Payload[29] = psPayload->u8Endpoint;
        vWriteU16ToPayLoad( psPayload->u16ProfileId, &au8Payload[30]);
        vWriteU16ToPayLoad( psPayload->u16DeviceId, &au8Payload[32]);
        au8Payload[34] = psPayload->u8Version;
        au8Payload[35] = psPayload->u8GroupIdCount;
        u8PayloadSize = 36;
    }

    return eZCL_TransmitInterPanRequest(
            eTX_FROM_SERVER|eTX_CLUSTER_SPECIFIC|eTX_DISABLE_DEFAULT_RSP,
            E_CLD_COMMISSION_CMD_SCAN_RSP,
            u8PayloadSize,
            COMMISSIONING_CLUSTER_ID_TL_COMMISSIONING,
            ZLL_PROFILE_ID,
            0,
            psDestinationAddress,
            au8Payload,
            pu8TransactionSequenceNumber);
}

/****************************************************************************
 **
 ** NAME:     eCLD_ZllCommissionCommandDeviceInfoReqCommandSend
 **
 ** DESCRIPTION:
 **
 **
 ** PARAMETERS:                 Name                      Usage
 ** ZPS_tsInterPanAddress       *psDestinationAddress
 ** uint8                       *pu8TransactionSequenceNumber
 ** tsCLD_ZllCommission_DeviceInfoReqCommandPayload  *psPayload
 **
 ** RETURN:
 ** status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ZllCommissionCommandDeviceInfoReqCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_DeviceInfoReqCommandPayload *psPayload)
{
    uint8 au8Payload[6];
    /*
     * This is an Inter Pan Client Message
     */

    vWriteU32ToPayLoad( psPayload->u32TransactionId, au8Payload);
    au8Payload[4] = psPayload->u8StartIndex;


    return eZCL_TransmitInterPanRequest(
            eTX_FROM_CLIENT|eTX_CLUSTER_SPECIFIC|eTX_DISABLE_DEFAULT_RSP,
            E_CLD_COMMISSION_CMD_DEVICE_INFO_REQ,
            5,
            COMMISSIONING_CLUSTER_ID_TL_COMMISSIONING,
            ZLL_PROFILE_ID,
            0,
            psDestinationAddress,
            au8Payload,
            pu8TransactionSequenceNumber);
}

PUBLIC  teZCL_Status eCLD_ZllCommissionCommandDeviceInfoRspCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_DeviceInfoRspCommandPayload *psPayload)
{
    uint8 au8Payload[8+(TL_MAX_DEVICE_RECORDS*16)];
    uint8 u8Pos;
    int i;
    /*
     * This is an Inter Pan Server Message
     */

    if (psPayload->u8DeviceInfoRecordCount > TL_MAX_DEVICE_RECORDS) {
        // device count too big or Null pointer
        DBG_vPrintf(1, "Too big or Count %d Max 5d\n", psPayload->u8DeviceInfoRecordCount, TL_MAX_DEVICE_RECORDS);
        return 1;  // TODO error code
    }

    vWriteU32ToPayLoad( psPayload->u32TransactionId, au8Payload);
    au8Payload[4] = psPayload->u8NumberSubDevices;
    au8Payload[5] = psPayload->u8StartIndex;
    au8Payload[6] = psPayload->u8DeviceInfoRecordCount;

    u8Pos = 7;
    for (i=0;  i<psPayload->u8DeviceInfoRecordCount; i++) {
        vWriteU64ToPayLoad( psPayload->asDeviceRecords[i].u64IEEEAddr, &au8Payload[u8Pos]);
        u8Pos += 8;
        au8Payload[u8Pos++] = psPayload->asDeviceRecords[i].u8Endpoint;
        vWriteU16ToPayLoad( psPayload->asDeviceRecords[i].u16ProfileId, &au8Payload[u8Pos]);
        u8Pos += 2;
        vWriteU16ToPayLoad( psPayload->asDeviceRecords[i].u16DeviceId, &au8Payload[u8Pos]);
        u8Pos += 2;
        au8Payload[u8Pos++] = psPayload->asDeviceRecords[i].u8Version;
        au8Payload[u8Pos++] = psPayload->asDeviceRecords[i].u8NumberGroupIds;
        au8Payload[u8Pos++] = psPayload->asDeviceRecords[i].u8Sort;
    }



    return eZCL_TransmitInterPanRequest(
            eTX_FROM_SERVER|eTX_CLUSTER_SPECIFIC|eTX_DISABLE_DEFAULT_RSP,
            E_CLD_COMMISSION_CMD_DEVICE_INFO_RSP,
            u8Pos,
            COMMISSIONING_CLUSTER_ID_TL_COMMISSIONING,
            ZLL_PROFILE_ID,
            0,
            psDestinationAddress,
            au8Payload,
            pu8TransactionSequenceNumber);
}

/****************************************************************************
 **
 ** NAME:     eCLD_ZllCommissionCommandDeviceIndentifyReqCommandSend
 **
 ** DESCRIPTION:
 **
 **
 ** PARAMETERS:                 Name                      Usage
 ** ZPS_tsInterPanAddress       *psDestinationAddress
 ** uint8                       *pu8TransactionSequenceNumber
 ** tsCLD_ZllCommission_IdentifyReqCommandPayload  *psPayload
 **
 ** RETURN:
 ** status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ZllCommissionCommandDeviceIndentifyReqCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_IdentifyReqCommandPayload *psPayload)
{
    uint8 au8Payload[7];
    /*
     * This is an Inter Pan Client Message
     */

    vWriteU32ToPayLoad( psPayload->u32TransactionId, au8Payload);
    vWriteU16ToPayLoad( psPayload->u16Duration, &au8Payload[4]);


    return eZCL_TransmitInterPanRequest(
            eTX_FROM_CLIENT|eTX_CLUSTER_SPECIFIC|eTX_DISABLE_DEFAULT_RSP,
            E_CLD_COMMISSION_CMD_IDENTIFY_REQ,
            6,
            COMMISSIONING_CLUSTER_ID_TL_COMMISSIONING,
            ZLL_PROFILE_ID,
            0,
            psDestinationAddress,
            au8Payload,
            pu8TransactionSequenceNumber);
}

/****************************************************************************
 **
 ** NAME:     eCLD_ZllCommissionCommandFactoryResetReqCommandSend
 **
 ** DESCRIPTION:
 **
 **
 ** PARAMETERS:                 Name                      Usage
 ** ZPS_tsInterPanAddress       *psDestinationAddress
 ** uint8                       *pu8TransactionSequenceNumber
 ** tsCLD_ZllCommission_FactoryResetReqCommandPayload  *psPayload
 **
 ** RETURN:
 ** status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ZllCommissionCommandFactoryResetReqCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_FactoryResetReqCommandPayload *psPayload)
{
    uint8 au8Payload[5];
    /*
     * This is an Inter Pan Client Message
     */

    vWriteU32ToPayLoad( psPayload->u32TransactionId, au8Payload);

    return eZCL_TransmitInterPanRequest(
            eTX_FROM_CLIENT|eTX_CLUSTER_SPECIFIC|eTX_DISABLE_DEFAULT_RSP,
            E_CLD_COMMISSION_CMD_FACTORY_RESET_REQ,
            4,
            COMMISSIONING_CLUSTER_ID_TL_COMMISSIONING,
            ZLL_PROFILE_ID,
            0,
            psDestinationAddress,
            au8Payload,
            pu8TransactionSequenceNumber);
}

/****************************************************************************
 **
 ** NAME:     eCLD_ZllCommissionCommandNetworkStartReqCommandSend
 **
 ** DESCRIPTION:
 **
 **
 ** PARAMETERS:                 Name                      Usage
 ** ZPS_tsInterPanAddress       *psDestinationAddress
 ** uint8                       *pu8TransactionSequenceNumber
 ** tsCLD_ZllCommission_NetworkStartReqCommandPayload  *psPayload
 **
 ** RETURN:
 ** status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ZllCommissionCommandNetworkStartReqCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_NetworkStartReqCommandPayload *psPayload)
{
    uint8 au8Payload[56];
    /*
     * This is an Inter Pan Client Message
     */

    vWriteU32ToPayLoad( psPayload->u32TransactionId, au8Payload);
    vWriteU64ToPayLoad( psPayload->u64ExtPanId, &au8Payload[4]);
    au8Payload[12] = psPayload->u8KeyIndex;
    vWriteU8ArrayToPayLoad( psPayload->au8NwkKey, &au8Payload[13], sizeof(psPayload->au8NwkKey));
    au8Payload[29] = psPayload->u8LogicalChannel;
    vWriteU16ToPayLoad( psPayload->u16PanId, &au8Payload[30]);
    vWriteU16ToPayLoad( psPayload->u16NwkAddr, &au8Payload[32]);
    vWriteU16ToPayLoad( psPayload->u16GroupIdBegin, &au8Payload[34]);
    vWriteU16ToPayLoad( psPayload->u16GroupIdEnd, &au8Payload[36]);
    vWriteU16ToPayLoad( psPayload->u16FreeNwkAddrBegin, &au8Payload[38]);
    vWriteU16ToPayLoad( psPayload->u16FreeNwkAddrEnd, &au8Payload[40]);
    vWriteU16ToPayLoad( psPayload->u16FreeGroupIdBegin, &au8Payload[42]);
    vWriteU16ToPayLoad( psPayload->u16FreeGroupIdEnd, &au8Payload[44]);
    vWriteU64ToPayLoad( psPayload->u64InitiatorIEEEAddr, &au8Payload[46]);
    vWriteU16ToPayLoad( psPayload->u16InitiatorNwkAddr, &au8Payload[54]);

    return eZCL_TransmitInterPanRequest(
            eTX_FROM_CLIENT|eTX_CLUSTER_SPECIFIC|eTX_DISABLE_DEFAULT_RSP,
            E_CLD_COMMISSION_CMD_NETWORK_START_REQ,
            56,
            COMMISSIONING_CLUSTER_ID_TL_COMMISSIONING,
            ZLL_PROFILE_ID,
            0,
            psDestinationAddress,
            au8Payload,
            pu8TransactionSequenceNumber);
}

/****************************************************************************
 **
 ** NAME:     eCLD_ZllCommissionCommandNetworkStartRspCommandSend
 **
 ** DESCRIPTION:
 **
 **
 ** PARAMETERS:                 Name                      Usage
 ** ZPS_tsInterPanAddress       *psDestinationAddress
 ** uint8                       *pu8TransactionSequenceNumber
 ** tsCLD_ZllCommission_NetworkStartRspCommandPayload  *psPayload
 **
 ** RETURN:
 ** status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ZllCommissionCommandNetworkStartRspCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_NetworkStartRspCommandPayload *psPayload)
{
    uint8 au8Payload[18];
    /*
     * This is an Inter Pan Server Message
     */

    vWriteU32ToPayLoad( psPayload->u32TransactionId, au8Payload);
    au8Payload[4] = psPayload->u8Status;
    vWriteU64ToPayLoad( psPayload->u64ExtPanId, &au8Payload[5]);
    au8Payload[13] = psPayload->u8NwkUpdateId;
    au8Payload[14] = psPayload->u8LogicalChannel;
    vWriteU16ToPayLoad( psPayload->u16PanId, &au8Payload[15]);

    return eZCL_TransmitInterPanRequest(
            eTX_FROM_SERVER|eTX_CLUSTER_SPECIFIC|eTX_DISABLE_DEFAULT_RSP,
            E_CLD_COMMISSION_CMD_NETWORK_START_RSP,
            17,
            COMMISSIONING_CLUSTER_ID_TL_COMMISSIONING,
            ZLL_PROFILE_ID,
            0,
            psDestinationAddress,
            au8Payload,
            pu8TransactionSequenceNumber);
}

/****************************************************************************
 **
 ** NAME:     eCLD_ZllCommissionCommandNetworkJoinRouterReqCommandSend
 **
 ** DESCRIPTION:
 **
 **
 ** PARAMETERS:                 Name                      Usage
 ** ZPS_tsInterPanAddress       *psDestinationAddress
 ** uint8                       *pu8TransactionSequenceNumber
 ** tsCLD_ZllCommission_NetworkJoinRouterReqCommandPayload  *psPayload
 **
 ** RETURN:
 ** status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ZllCommissionCommandNetworkJoinRouterReqCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_NetworkJoinRouterReqCommandPayload *psPayload)
{
    uint8 au8Payload[48];
    /*
     * This is an Inter Pan Client Message
     */

    vWriteU32ToPayLoad( psPayload->u32TransactionId, au8Payload);
    vWriteU64ToPayLoad( psPayload->u64ExtPanId, &au8Payload[4]);
    au8Payload[12] = psPayload->u8KeyIndex;
    vWriteU8ArrayToPayLoad( psPayload->au8NwkKey, &au8Payload[13], sizeof(psPayload->au8NwkKey));
    au8Payload[29] = psPayload->u8NwkUpdateId;
    au8Payload[30] = psPayload->u8LogicalChannel;
    vWriteU16ToPayLoad( psPayload->u16PanId, &au8Payload[31]);
    vWriteU16ToPayLoad( psPayload->u16NwkAddr, &au8Payload[33]);
    vWriteU16ToPayLoad( psPayload->u16GroupIdBegin, &au8Payload[35]);
    vWriteU16ToPayLoad( psPayload->u16GroupIdEnd, &au8Payload[37]);
    vWriteU16ToPayLoad( psPayload->u16FreeNwkAddrBegin, &au8Payload[39]);
    vWriteU16ToPayLoad( psPayload->u16FreeNwkAddrEnd, &au8Payload[41]);
    vWriteU16ToPayLoad( psPayload->u16FreeGroupIdBegin, &au8Payload[43]);
    vWriteU16ToPayLoad( psPayload->u16FreeGroupIdEnd, &au8Payload[45]);


    return eZCL_TransmitInterPanRequest(
            eTX_FROM_CLIENT|eTX_CLUSTER_SPECIFIC|eTX_DISABLE_DEFAULT_RSP,
            E_CLD_COMMISSION_CMD_NETWORK_JOIN_ROUTER_REQ,
            47,
            COMMISSIONING_CLUSTER_ID_TL_COMMISSIONING,
            ZLL_PROFILE_ID,
            0,
            psDestinationAddress,
            au8Payload,
            pu8TransactionSequenceNumber);
}

/****************************************************************************
 **
 ** NAME:     eCLD_ZllCommissionCommandNetworkJoinRouterRspCommandSend
 **
 ** DESCRIPTION:
 **
 **
 ** PARAMETERS:                 Name                      Usage
 ** ZPS_tsInterPanAddress       *psDestinationAddress
 ** uint8                       *pu8TransactionSequenceNumber
 ** tsCLD_ZllCommission_NetworkJoinRouterRspCommandPayload  *psPayload
 **
 ** RETURN:
 ** status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ZllCommissionCommandNetworkJoinRouterRspCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_NetworkJoinRouterRspCommandPayload *psPayload)
{
    uint8 au8Payload[6];
    /*
     * This is an Inter Pan Server Message
     */

    vWriteU32ToPayLoad( psPayload->u32TransactionId, au8Payload);
    au8Payload[4] = psPayload->u8Status;

    return eZCL_TransmitInterPanRequest(
            eTX_FROM_SERVER|eTX_CLUSTER_SPECIFIC|eTX_DISABLE_DEFAULT_RSP,
            E_CLD_COMMISSION_CMD_NETWORK_JOIN_ROUTER_RSP,
            5,
            COMMISSIONING_CLUSTER_ID_TL_COMMISSIONING,
            ZLL_PROFILE_ID,
            0,
            psDestinationAddress,
            au8Payload,
            pu8TransactionSequenceNumber);
}

/****************************************************************************
 **
 ** NAME:     eCLD_ZllCommissionCommandNetworkJoinEndDeviceReqCommandSend
 **
 ** DESCRIPTION:
 **
 **
 ** PARAMETERS:                 Name                      Usage
 ** ZPS_tsInterPanAddress       *psDestinationAddress
 ** uint8                       *pu8TransactionSequenceNumber
 ** tsCLD_ZllCommission_NetworkJoinEndDeviceReqCommandPayload  *psPayload
 **
 ** RETURN:
 ** status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ZllCommissionCommandNetworkJoinEndDeviceReqCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_NetworkJoinEndDeviceReqCommandPayload *psPayload)
{
    uint8 au8Payload[48];
    /*
     * This is an Inter Pan Client Message
     */

    vWriteU32ToPayLoad( psPayload->u32TransactionId, au8Payload);
    vWriteU64ToPayLoad( psPayload->u64ExtPanId, &au8Payload[4]);
    au8Payload[12] = psPayload->u8KeyIndex;
    vWriteU8ArrayToPayLoad( psPayload->au8NwkKey, &au8Payload[13], sizeof(psPayload->au8NwkKey));
    au8Payload[29] = psPayload->u8NwkUpdateId;
    au8Payload[30] = psPayload->u8LogicalChannel;
    vWriteU16ToPayLoad( psPayload->u16PanId, &au8Payload[31]);
    vWriteU16ToPayLoad( psPayload->u16NwkAddr, &au8Payload[33]);
    vWriteU16ToPayLoad( psPayload->u16GroupIdBegin, &au8Payload[35]);
    vWriteU16ToPayLoad( psPayload->u16GroupIdEnd, &au8Payload[37]);
    vWriteU16ToPayLoad( psPayload->u16FreeNwkAddrBegin, &au8Payload[39]);
    vWriteU16ToPayLoad( psPayload->u16FreeNwkAddrEnd, &au8Payload[41]);
    vWriteU16ToPayLoad( psPayload->u16FreeGroupIdBegin, &au8Payload[43]);
    vWriteU16ToPayLoad( psPayload->u16FreeGroupIdEnd, &au8Payload[45]);


    return eZCL_TransmitInterPanRequest(
            eTX_FROM_CLIENT|eTX_CLUSTER_SPECIFIC|eTX_DISABLE_DEFAULT_RSP,
            E_CLD_COMMISSION_CMD_NETWORK_JOIN_END_DEVICE_REQ,
            47,
                           COMMISSIONING_CLUSTER_ID_TL_COMMISSIONING,
            ZLL_PROFILE_ID,
                           0,
            psDestinationAddress,
            au8Payload,
            pu8TransactionSequenceNumber);
}

/****************************************************************************
 **
 ** NAME:     eCLD_ZllCommissionCommandNetworkJoinEndDeviceRspCommandSend
 **
 ** DESCRIPTION:
 **
 **
 ** PARAMETERS:                 Name                      Usage
 ** ZPS_tsInterPanAddress       *psDestinationAddress
 ** uint8                       *pu8TransactionSequenceNumber
 ** tsCLD_ZllCommission_NetworkJoinEndDeviceRspCommandPayload  *psPayload
 **
 ** RETURN:
 ** status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ZllCommissionCommandNetworkJoinEndDeviceRspCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_NetworkJoinEndDeviceRspCommandPayload *psPayload)
{
    uint8 au8Payload[6];
    /*
     * This is an Inter Pan Server Message
     */

    vWriteU32ToPayLoad( psPayload->u32TransactionId, au8Payload);
    au8Payload[4] = psPayload->u8Status;

    return eZCL_TransmitInterPanRequest(
            eTX_FROM_SERVER|eTX_CLUSTER_SPECIFIC|eTX_DISABLE_DEFAULT_RSP,
            E_CLD_COMMISSION_CMD_NETWORK_JOIN_END_DEVICE_RSP,
            5,
                          COMMISSIONING_CLUSTER_ID_TL_COMMISSIONING,
            ZLL_PROFILE_ID,
                          0,
            psDestinationAddress,
            au8Payload,
            pu8TransactionSequenceNumber);
}

/****************************************************************************
 **
 ** NAME:     eCLD_ZllCommissionCommandNetworkUpdateReqCommandSend
 **
 ** DESCRIPTION:
 **
 **
 ** PARAMETERS:                 Name                      Usage
 ** ZPS_tsInterPanAddress       *psDestinationAddress
 ** uint8                       *pu8TransactionSequenceNumber
 ** tsCLD_ZllCommission_NetworkUpdateReqCommandPayload  *psPayload
 **
 ** RETURN:
 ** status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ZllCommissionCommandNetworkUpdateReqCommandSend(
                    ZPS_tsInterPanAddress       *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllCommission_NetworkUpdateReqCommandPayload *psPayload)
{
    uint8 au8Payload[19];
    /*
     * This is an Inter Pan Client Message
     */

    vWriteU32ToPayLoad( psPayload->u32TransactionId, au8Payload);
    vWriteU64ToPayLoad( psPayload->u64ExtPanId, &au8Payload[4]);
    au8Payload[12] = psPayload->u8NwkUpdateId;

    au8Payload[13] = psPayload->u8LogicalChannel;
    vWriteU16ToPayLoad( psPayload->u16PanId, &au8Payload[14]);
    vWriteU16ToPayLoad( psPayload->u16NwkAddr, &au8Payload[16]);

    return eZCL_TransmitInterPanRequest(
            eTX_FROM_CLIENT|eTX_CLUSTER_SPECIFIC|eTX_DISABLE_DEFAULT_RSP,
            E_CLD_COMMISSION_CMD_NETWORK_UPDATE_REQ,
            18,
                              COMMISSIONING_CLUSTER_ID_TL_COMMISSIONING,
            ZLL_PROFILE_ID,
                              0,
            psDestinationAddress,
            au8Payload,
            pu8TransactionSequenceNumber);
}








/****************************************************************************
 **
 ** NAME:     vWriteU16ToPayLoad
 **
 ** DESCRIPTION:
 **
 ** PARAMETERS:                 Name                      Usage
 ** uint16                      u16Data
 ** uint8                       *pu8Buffer
 **
 ** RETURN:
 ** void
 **
 ****************************************************************************/
PRIVATE void vWriteU16ToPayLoad(uint16 u16Data, uint8 *pu8Buffer) {
    pu8Buffer[0] = (uint8)(u16Data & 0xff);
    pu8Buffer[1] = (uint8)(u16Data >> 8);
    }

/****************************************************************************
 **
 ** NAME:     vWriteU32ToPayLoad
 **
 ** DESCRIPTION:
 **
 ** PARAMETERS:                 Name                      Usage
 ** uint32                      u32Data
 ** uint8                       *pu8Buffer
 **
 ** RETURN:
 ** void
 **
 ****************************************************************************/
PRIVATE void vWriteU32ToPayLoad(uint32 u32Data, uint8 *pu8Buffer) {
    pu8Buffer[0] = (uint8)(u32Data & 0xff);
    pu8Buffer[1] = (uint8)((u32Data >> 8) & 0xff);
    pu8Buffer[2] = (uint8)((u32Data >> 16) & 0xff);
    pu8Buffer[3] = (uint8)(u32Data >> 24);
}

/****************************************************************************
 **
 ** NAME:     vWriteU32ToPayLoad
 **
 ** DESCRIPTION:
 **
 ** PARAMETERS:                 Name                      Usage
 ** uint64                      u64Data
 ** uint8                       *pu8Buffer
 **
 ** RETURN:
 ** void
 **
 ****************************************************************************/
PRIVATE void vWriteU64ToPayLoad(uint64 u64Data, uint8 *pu8Buffer) {
    pu8Buffer[0] = (uint8)(u64Data & 0xff);
    pu8Buffer[1] = (uint8)((u64Data >> 8) & 0xff);
    pu8Buffer[2] = (uint8)((u64Data >> 16) & 0xff);
    pu8Buffer[3] = (uint8)((u64Data >> 24) & 0xff);
    pu8Buffer[4] = (uint8)((u64Data >> 32) & 0xff);
    pu8Buffer[5] = (uint8)((u64Data >> 40) & 0xff);
    pu8Buffer[6] = (uint8)((u64Data >> 48) & 0xff);
    pu8Buffer[7] = (uint8)(u64Data >> 56);
}

/****************************************************************************
 **
 ** NAME:     vWriteU32ToPayLoad
 **
 ** DESCRIPTION:
 **
 ** PARAMETERS:                 Name                      Usage
 ** uint8                       *pu8Data
 ** uint8                       *pu8Buffer
 ** uint8                       u8Num
 **
 ** RETURN:
 ** void
 **
 ****************************************************************************/
PRIVATE void vWriteU8ArrayToPayLoad(uint8 *pu8Data, uint8 *pu8Buffer, uint8 u8Num) {
    int i;

    for (i=0; i<u8Num; i++) {
        pu8Buffer[i] = pu8Data[i];
}
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

