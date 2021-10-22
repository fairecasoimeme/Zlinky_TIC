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
 * MODULE:             Smart Energy
 *
 * COMPONENT:          zcl_transmit.c
 *
 * DESCRIPTION:       ZCL transmit functions
 *
 ****************************************************************************/




/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>
#include "zcl.h"
#include "pdum_apl.h"
#include "zps_apl.h"
#include "zps_apl_af.h"
#include "zcl_common.h"

#ifdef CLD_GREENPOWER
#include "GreenPower.h"
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define MANUFACTURER_SPECIFIC_SHIFT                     (2)
#define DIRECTION_SHIFT                                 (3)
#define DISABLE_DEFAULT_RESPONSE_SHIFT                  (4)

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
#ifdef CLD_BIND_SERVER
PRIVATE teZCL_Status eZCL_BufferBindTransmissionPacket(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    uint16                      u16ClusterId,
                    tsZCL_Address              *psDestinationAddress,
                    uint16                      u16PDUSize,
                    uint8                      *pu8PDU);

PRIVATE uint8 u8ZCL_GetNumberOfFreeBindServerRecrodEntries(void);
#endif /* CLD_BIND_SERVER */
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
 ** NAME:       eZCL_TransmitDataRequest
 **
 ** DESCRIPTION:
 ** TX's a Data Request
 **
 ** PARAMETERS:                Name                     Usage
 ** PDUM_thAPduInstance        myPDUM_thAPduInstance    Buffer Instance
 ** uint16                     u16PayloadSize           Data Payload
 ** uint8                      u8SourceEndPointId       source EP Id
 ** uint8                      u8DestinationEndPointId  destination EP Id
 ** uint16                     u16ClusterId             cluster Id
 ** tsZCL_Address             *psDestinationAddress     Address structure
 **
 ** RETURN:
 ** ZPS_teStatus
 **
 ****************************************************************************/

PUBLIC teZCL_Status eZCL_TransmitDataRequest(
                        PDUM_thAPduInstance        myPDUM_thAPduInstance,
                        uint16                     u16PayloadSize,
                        uint8                      u8SourceEndPointId,
                        uint8                      u8DestinationEndPointId,
                        uint16                     u16ClusterId,
                        tsZCL_Address              *psDestinationAddress)
{
    ZPS_teStatus eZpsStatus = ZPS_E_SUCCESS;

#ifdef  CLD_BIND_SERVER
    ZPS_teStatus eZCL_Status = E_ZCL_SUCCESS;
#endif

    uint8 u8StackSecurity;
    tsZCL_ClusterInstance *psClusterInstance = NULL;
    teZCL_ZCLSendSecurity eClusterSecurityLevel;
    tsZCL_CallBackEvent sZCL_CallBackEvent;

    // set payload size
    PDUM_eAPduInstanceSetPayloadSize(myPDUM_thAPduInstance, u16PayloadSize);

    // Find the cluster's security mode - try both client and server

    (void) eZCL_SearchForClusterEntry(u8SourceEndPointId,
        u16ClusterId,
        FALSE,
        &psClusterInstance);

    if (psClusterInstance == NULL)
    {
        (void) eZCL_SearchForClusterEntry(u8SourceEndPointId,
            u16ClusterId,
            TRUE,
            &psClusterInstance);
    }
    if((u8SourceEndPointId == ZCL_GP_PROXY_ENDPOINT_ID)&&(psClusterInstance == NULL))
    {
        if(u8DestinationEndPointId == 0xFF)
        {
            uint8 i, u8EndPointCount = u8ZCL_GetNumberOfEndpointsRegistered();
            for(i =0; i< u8EndPointCount; i++)
            {
                if(eZCL_SearchForClusterEntry(u8ZCL_GetEPIdFromIndex(i),
                                  u16ClusterId,
                                  TRUE,
                                  &psClusterInstance) == E_ZCL_SUCCESS)
                {
                    //u8SourceEndPointId = i;
                    break;
                }
            }
        }
        else
        {
           (void) eZCL_SearchForClusterEntry(u8DestinationEndPointId,
                  u16ClusterId,
                  TRUE,
                  &psClusterInstance);

            //u8SourceEndPointId = u8DestinationEndPointId;
        }

        u8SourceEndPointId = ZCL_GP_PROXY_ENDPOINT_ID;
    }
    if (psClusterInstance == NULL)
    {
        // Cluster not found - e.g. when sending a default response for a cluster that doesn't exist
        // Send without APS security.
        //eClusterSecurityLevel = E_ZCL_SECURITY_NETWORK;
        eClusterSecurityLevel = psZCL_Common->eSecuritySupported;
    }
    else
    {
        eClusterSecurityLevel = (psClusterInstance->psClusterDefinition->u8ClusterControlFlags & SEND_SECURITY_MASK);
    }

    // Force the level down if the app wants to
    if(E_ZCL_SECURITY_TEMP_APPLINK != eClusterSecurityLevel)
    {
        if (psZCL_Common->eSecuritySupported < eClusterSecurityLevel)
            eClusterSecurityLevel = psZCL_Common->eSecuritySupported;
    }

    // Convert to ZPS bitmask
    switch(eClusterSecurityLevel)
    {
    case E_ZCL_SECURITY_NETWORK:
        u8StackSecurity = ZPS_E_APL_AF_SECURE_NWK;
        break;
    case E_ZCL_SECURITY_TEMP_APPLINK:
    {
        /* APS security is temporarily so return back to E_ZCL_SECURITY_NETWORK */
        if (psClusterInstance != NULL) 
        {
            psClusterInstance->psClusterDefinition->u8ClusterControlFlags &= 0xF0;
            psClusterInstance->psClusterDefinition->u8ClusterControlFlags |= E_ZCL_SECURITY_NETWORK;
        }
    }
    case E_ZCL_SECURITY_APPLINK:
    default:
        u8StackSecurity = ZPS_E_APL_AF_SECURE | ZPS_E_APL_AF_SECURE_NWK;
        break;
    }

    /* Check If cluster is OTA and Dst Address Mode is Broadcast/Group then apply NWK Sec */
    if( (u16ClusterId == 0x0019) &&
            (psDestinationAddress->eAddressMode == E_ZCL_AM_GROUP || psDestinationAddress->eAddressMode == E_ZCL_AM_BROADCAST) )
    {
        u8StackSecurity = ZPS_E_APL_AF_SECURE_NWK;
    }
#ifdef CLD_GREENPOWER
    if(psZCL_Common->u8GreenPowerMappedEpId == u8SourceEndPointId)
    {
        u8SourceEndPointId = ZCL_GP_PROXY_ENDPOINT_ID;
#ifdef GP_DISABLE_SECURITY_FOR_CERTIFICATION
        if(u8DestinationEndPointId == ZCL_GP_PROXY_ENDPOINT_ID)
        {
            u8StackSecurity = ZPS_E_APL_AF_UNSECURE;
        }
#endif
    }
#endif

    if((psZCL_Common->bDisableAPSACK) 
#if (defined CLD_WWAH )
    || ((psClusterInstance!= NULL) && !(psClusterInstance->psClusterDefinition->u8ClusterControlFlags & CLUSTER_APS_ACK_ENABLE_MASK))
#endif
       )
    {
        if(E_ZCL_AM_SHORT == psDestinationAddress->eAddressMode)
            psDestinationAddress->eAddressMode = E_ZCL_AM_SHORT_NO_ACK;
        if(E_ZCL_AM_IEEE == psDestinationAddress->eAddressMode)
            psDestinationAddress->eAddressMode = E_ZCL_AM_IEEE_NO_ACK;
        if(E_ZCL_AM_BOUND == psDestinationAddress->eAddressMode)
            psDestinationAddress->eAddressMode = E_ZCL_AM_BOUND_NO_ACK;
        if(E_ZCL_AM_BOUND_NON_BLOCKING == psDestinationAddress->eAddressMode)
            psDestinationAddress->eAddressMode = E_ZCL_AM_BOUND_NON_BLOCKING_NO_ACK;
    }

    switch(psDestinationAddress->eAddressMode)
    {
        case(E_ZCL_AM_SHORT):
        {
            eZpsStatus = ZPS_eAplAfUnicastAckDataReq
                (myPDUM_thAPduInstance,
                u16ClusterId,
                u8SourceEndPointId,
                u8DestinationEndPointId,
                psDestinationAddress->uAddress.u16DestinationAddress,
                u8StackSecurity,
                0 /* default radius */,
                &(psZCL_Common->u8ApsSequenceNumberOfLastTransmit));
            break;
        }

        case(E_ZCL_AM_IEEE):
        {
            eZpsStatus = ZPS_eAplAfUnicastIeeeAckDataReq
                (myPDUM_thAPduInstance,
                u16ClusterId,
                u8SourceEndPointId,
                u8DestinationEndPointId,
                psDestinationAddress->uAddress.u64DestinationAddress,
                u8StackSecurity,
                0 /* default radius */,
                &(psZCL_Common->u8ApsSequenceNumberOfLastTransmit));
            break;
        }

        case(E_ZCL_AM_BOUND):
        {
            eZpsStatus = ZPS_eAplAfBoundAckDataReq
                (myPDUM_thAPduInstance,
                u16ClusterId,
                u8SourceEndPointId,
                u8StackSecurity,
                0 /* default radius */,
                NULL);
            // Sending before anyone has bound to us is not an error
            // as the app may be e.g. adding prices not knowing if anyone has bound or not.
            if (eZpsStatus == ZPS_APL_APS_E_NO_BOUND_DEVICE)
            {
                // This error return only happens on pre 1v6 ZB Pro stacks.  In these stacks, the buffer was freed by the ZB Pro stack
                   eZpsStatus = ZPS_E_SUCCESS;
            }

            break;
        }

        // no ack versions of these calls

        case(E_ZCL_AM_GROUP):
        {
            uint8 u8GroupCastRadius = 0;

#ifdef CLD_GREENPOWER
            tsZCL_EndPointDefinition                    *psEndPointDefinition;
            tsZCL_ClusterInstance                       *psClusterInstance;
            tsGP_GreenPowerCustomData                   *psGpCustomDataStructure;
            teZCL_Status                                eStatus;

            /* Find Green Power Cluster */
            if((eStatus = eGP_FindGpCluster(psZCL_Common->u8GreenPowerMappedEpId,
                                TRUE,
                                &psEndPointDefinition,
                                &psClusterInstance,
                                &psGpCustomDataStructure)) == E_ZCL_SUCCESS)
            {
                u8GroupCastRadius = psGpCustomDataStructure->u8GroupCastRadius;
            }
            else
            {
                u8GroupCastRadius = 0;
            }
#endif

            eZpsStatus = ZPS_eAplAfGroupDataReq
                (myPDUM_thAPduInstance,
                u16ClusterId,
                u8SourceEndPointId,
                psDestinationAddress->uAddress.u16GroupAddress,
                u8StackSecurity,
                u8GroupCastRadius /* default radius */,
                &(psZCL_Common->u8ApsSequenceNumberOfLastTransmit));
            break;
        }

        case(E_ZCL_AM_BROADCAST):
        {
            eZpsStatus = ZPS_eAplAfBroadcastDataReq
                (myPDUM_thAPduInstance,
                u16ClusterId,
                u8SourceEndPointId,
                u8DestinationEndPointId,
                psDestinationAddress->uAddress.eBroadcastMode,
                u8StackSecurity,
                0 /* default radius */,
                &(psZCL_Common->u8ApsSequenceNumberOfLastTransmit));
            break;
        }
        case(E_ZCL_AM_NO_TRANSMIT):
        {
            // Allows price or LCEs to be added before network established
            eZpsStatus = ZPS_E_SUCCESS;

            /* As packet is not passing to stack, delete apdu instance */
            PDUM_eAPduFreeAPduInstance(myPDUM_thAPduInstance);
            break;
        }
        case E_ZCL_AM_BOUND_NO_ACK:
            eZpsStatus = ZPS_eAplAfBoundDataReq
                (myPDUM_thAPduInstance,
                u16ClusterId,
                u8SourceEndPointId,
                u8StackSecurity,
                0 /* default radius */,
                NULL);
        // Sending before anyone has bound to us is not an error
        // as the app may be e.g. adding prices not knowing if anyone has bound or not.
        if (eZpsStatus == ZPS_APL_APS_E_NO_BOUND_DEVICE)
        {
            // This error return only happens on pre 1v6 ZB Pro stacks.  In these stacks, the buffer was freed by the ZB Pro stack
            eZpsStatus = ZPS_E_SUCCESS;
        }
            break;

        case E_ZCL_AM_BOUND_NON_BLOCKING:
            eZpsStatus = ZPS_eAplAfBoundDataReqNonBlocking(myPDUM_thAPduInstance,
                                                           u16ClusterId,
                                                           u8SourceEndPointId,
                                                           u8StackSecurity,
                                                           0 /* default radius */,
                                                           TRUE);
            break;

        case E_ZCL_AM_BOUND_NON_BLOCKING_NO_ACK:
            eZpsStatus = ZPS_eAplAfBoundDataReqNonBlocking(myPDUM_thAPduInstance,
                                                           u16ClusterId,
                                                           u8SourceEndPointId,
                                                           u8StackSecurity,
                                                           0 /* default radius */,
                                                           FALSE);
            break;

        case E_ZCL_AM_SHORT_NO_ACK:
            eZpsStatus = ZPS_eAplAfUnicastDataReq
                        (myPDUM_thAPduInstance,
                        u16ClusterId,
                        u8SourceEndPointId,
                        u8DestinationEndPointId,
                        psDestinationAddress->uAddress.u16DestinationAddress,
                        u8StackSecurity,
                        0 /* default radius */,
                        &(psZCL_Common->u8ApsSequenceNumberOfLastTransmit));
            break;
        case E_ZCL_AM_IEEE_NO_ACK:
            eZpsStatus = ZPS_eAplAfUnicastIeeeDataReq
                (myPDUM_thAPduInstance,
                u16ClusterId,
                u8SourceEndPointId,
                u8DestinationEndPointId,
                psDestinationAddress->uAddress.u64DestinationAddress,
                u8StackSecurity,
                0 /* default radius */,
                &(psZCL_Common->u8ApsSequenceNumberOfLastTransmit));
            break;
        default:
        {
            PDUM_eAPduFreeAPduInstance(myPDUM_thAPduInstance);
            return(E_ZCL_ERR_PARAMETER_RANGE);
        }
    }

    // occurs after the return in the default - not a problem
    if (eZpsStatus != ZPS_E_SUCCESS)
    {

#ifdef CLD_BIND_SERVER
        /* Buffere the failed bind transmission packet */
        if(psDestinationAddress->eAddressMode == E_ZCL_AM_BOUND ||
                psDestinationAddress->eAddressMode == E_ZCL_AM_BOUND_NO_ACK)
        {
            eZCL_Status = eZCL_BufferBindTransmissionPacket(u8SourceEndPointId,
                                                u8DestinationEndPointId,
                                                u16ClusterId,
                                                psDestinationAddress,
                                                u16PayloadSize,
                                                (uint8*)PDUM_pvAPduInstanceGetPayload(myPDUM_thAPduInstance));

            /* check Status */
            if(eZCL_Status == E_ZCL_SUCCESS)
            {
                /* As we queuing the Bind Tx pkts, delete the hAPDU instance */
                PDUM_eAPduFreeAPduInstance(myPDUM_thAPduInstance);
                return E_ZCL_SUCCESS;
            }
        }
#endif /* CLD_BIND_SERVER */

        psZCL_Common->eLastZpsError = eZpsStatus;
        // fill in non-attribute specific values in callback event structure
        memset(&sZCL_CallBackEvent, 0, sizeof(tsZCL_CallBackEvent));
        sZCL_CallBackEvent.eEventType = E_ZCL_CBET_ERROR;
        sZCL_CallBackEvent.eZCL_Status = E_ZCL_ERR_ZTRANSMIT_FAIL;
        // general c/b
        psZCL_Common->pfZCLinternalCallBackFunction(&sZCL_CallBackEvent);
        PDUM_eAPduFreeAPduInstance(myPDUM_thAPduInstance);
        return E_ZCL_ERR_ZTRANSMIT_FAIL;
    }

    return E_ZCL_SUCCESS;
}

/****************************************************************************
 **
 ** NAME:       eZCL_BuildTransmitAddressStructure
 **
 ** DESCRIPTION:
 ** Build address structure
 **
 ** PARAMETERS:               Name                    Usage
 ** ZPS_tsAfEvent              *pZPSevent               Zigbee stack event structure
 ** tsZCL_Address            *psZCL_Address           Address Structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eZCL_BuildTransmitAddressStructure(
                        ZPS_tsAfEvent       *pZPSevent,
                        tsZCL_Address       *psZCL_Address)
{
    #ifdef STRICT_PARAM_CHECK 
        if((pZPSevent == NULL) || (psZCL_Address == NULL))
        {
            return(E_ZCL_FAIL);
        }
    #endif
    psZCL_Address->eAddressMode = pZPSevent->uEvent.sApsDataIndEvent.u8SrcAddrMode;

    switch(psZCL_Address->eAddressMode)
    {
        case(E_ZCL_AM_SHORT):
        case(E_ZCL_AM_SHORT_NO_ACK):
        {
            psZCL_Address->uAddress.u16DestinationAddress = pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr;
            break;
        }

        // not supported yet
        case(E_ZCL_AM_IEEE):
        case(E_ZCL_AM_BOUND):
        case(E_ZCL_AM_GROUP):
        case(E_ZCL_AM_BROADCAST):
        case(E_ZCL_AM_IEEE_NO_ACK):
        case(E_ZCL_AM_BOUND_NO_ACK):
        default:
        {
            return(E_ZCL_FAIL);
            break;
        }
    }
    /* this check is required for PR lpsw2920 - ZCL should accept the APS encrypted packet
     * even it is not required and send the response as aps encrypted
     */
    if(ZPS_APL_APS_E_SECURED_LINK_KEY == pZPSevent->uEvent.sApsDataIndEvent.eSecurityStatus)
    {
        tsZCL_ClusterInstance *psClusterInstance = NULL;
        teZCL_ZCLSendSecurity eClusterSecurityLevel;
        // Find the cluster's security mode - try both client and server
        eZCL_SearchForClusterEntry(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                pZPSevent->uEvent.sApsDataIndEvent.u16ClusterId,
                FALSE,
                &psClusterInstance);

        if (psClusterInstance == NULL)
        {
            eZCL_SearchForClusterEntry(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                    pZPSevent->uEvent.sApsDataIndEvent.u16ClusterId,
                    TRUE,
                    &psClusterInstance);
        }

        if (psClusterInstance == NULL)
        {
            // Cluster not found - something wrong
            return(E_ZCL_FAIL);
        }
        else
        {
            eClusterSecurityLevel = (psClusterInstance->psClusterDefinition->u8ClusterControlFlags & SEND_SECURITY_MASK);
        }

        // Force the security level up temporarily for sending response of this request if the request wants to
        if(E_ZCL_SECURITY_NETWORK == eClusterSecurityLevel)
        {
            psClusterInstance->psClusterDefinition->u8ClusterControlFlags |= E_ZCL_SECURITY_TEMP_APPLINK;
        }
    }

    return(E_ZCL_SUCCESS);
}


PUBLIC teZCL_Status eZCL_TransmitInterPanRequest(
                        eInterPanTxOptions        eTxOptions,
                        uint8                     u8Command,
                        uint8                     u8PayloadSize,
                        uint16                    u16ClusterId,
                        uint16                    u16Profile,
                        uint16                    u16ManufSpecific,
                        ZPS_tsInterPanAddress     *psDestinationAddress,
                        uint8                     *pu8Payload,
                        uint8                     *pu8SeqNum)
{
    ZPS_teStatus eZpsStatus = ZPS_E_SUCCESS;
    PDUM_thAPduInstance hAPduInst;
    uint16 u16Pos = 0;
    int i;
    uint8 *pu8Start;
    uint8 u8FCtrl = 0;

    hAPduInst = hZCL_AllocateAPduInstance();

    if(hAPduInst == PDUM_INVALID_HANDLE)
    {
        return(E_ZCL_ERR_ZBUFFER_FAIL);
    }

    pu8Start = (uint8 *)(PDUM_pvAPduInstanceGetPayload(hAPduInst));

    if (eTxOptions & eTX_CLUSTER_SPECIFIC) {
       u8FCtrl |= eFRAME_TYPE_COMMAND_IS_SPECIFIC_TO_A_CLUSTER;
    }
    if (eTxOptions & eTX_MANUFACTURER_SPECIFIC) {
       u8FCtrl |= (1 << MANUFACTURER_SPECIFIC_SHIFT);
    }
    if (eTxOptions & eTX_FROM_SERVER) {
       u8FCtrl |= (1 << DIRECTION_SHIFT);
    } else {
        // Client side message, so need new TSN
        *pu8SeqNum = u8GetTransactionSequenceNumber();
    }
    if (eTxOptions & eTX_DISABLE_DEFAULT_RSP) {
       u8FCtrl |= (1 << DISABLE_DEFAULT_RESPONSE_SHIFT);
    }
    *pu8Start++ = u8FCtrl;
    u16Pos++;
    if (eTxOptions & eTX_MANUFACTURER_SPECIFIC) {
       *pu8Start++ = (uint8)(u16ManufSpecific>>8);
       *pu8Start++ = (uint8)u16ManufSpecific;
       u16Pos += 2;
    }

    // TODOTransaction Seq No
    *pu8Start++ = *pu8SeqNum;
    u16Pos++;

    *pu8Start++ = u8Command;
    u16Pos++;

    for (i=0; i<u8PayloadSize; i++) {
        *pu8Start++ = pu8Payload[i];
    }

    u16Pos += u8PayloadSize;

    PDUM_eAPduInstanceSetPayloadSize(hAPduInst, u16Pos);

    eZpsStatus = ZPS_eAplAfInterPanDataReq( hAPduInst,
                               u16ClusterId,
                               u16Profile,
                               psDestinationAddress,
                               0);


    return eZpsStatus;
}

#ifdef CLD_BIND_SERVER
/****************************************************************************
 **
 ** NAME:       eZCL_BufferBindTransmissionPacket
 **
 ** DESCRIPTION:
 ** Buffer PDU Info for Bind Transmission
 **
 ** PARAMETERS:               Name                        Usage
 ** uint8                   u8SourceEndPointId        Source End Point
 ** uint8                   u8DestinationEndPointId   Destination End Point
 ** uint16                  u16ClusterId              Cluster Id
 ** tsZCL_Address          *psDestinationAddress      Destination Address
 ** uint8                   u8PDUSize                 Size of PDU
 ** uint8                  *pu8PDU                    Pointer to PDU
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status eZCL_BufferBindTransmissionPacket(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    uint16                      u16ClusterId,
                    tsZCL_Address              *psDestinationAddress,
                    uint16                      u16PDUSize,
                    uint8                      *pu8PDU)
{
    tsBindServerBufferedPDURecord     *psBindServerBufferedPDURecord;
    uint8                               u8NoOfBuffersRequired = 1;
    uint8                               u8NoOfBufferedCopied = 0;

    /* check how many buffers are required */
    u8NoOfBuffersRequired = (u16PDUSize/MAX_PDU_BIND_QUEUE_PAYLOAD_SIZE) +
                                    (u16PDUSize%MAX_PDU_BIND_QUEUE_PAYLOAD_SIZE != 0);

    /* check no.of free entries available aganist required */
    if(u8NoOfBuffersRequired > u8ZCL_GetNumberOfFreeBindServerRecrodEntries())
    {
        return E_ZCL_ERR_ZTRANSMIT_FAIL;
    }


    /* Allocate buffer for Saving PDU */
    psBindServerBufferedPDURecord = (tsBindServerBufferedPDURecord *)psDLISTgetHead(&psZCL_Common->sBindServerCustomData.lBindServerDeAllocList);

    /* Copy Buffered PDU info which will be used for scheduling transmission */
    psBindServerBufferedPDURecord->sBindServerBufferedPDUInfo.u8SourceEndPointId = u8SourceEndPointId;
    psBindServerBufferedPDURecord->sBindServerBufferedPDUInfo.u8DestinationEndPointId = u8DestinationEndPointId;
    psBindServerBufferedPDURecord->sBindServerBufferedPDUInfo.u16ClusterId = u16ClusterId;
    psBindServerBufferedPDURecord->sBindServerBufferedPDUInfo.eAddressMode = psDestinationAddress->eAddressMode;
    psBindServerBufferedPDURecord->sBindServerBufferedPDUInfo.u16PDUSize = u16PDUSize;

    while(u8NoOfBufferedCopied < u8NoOfBuffersRequired)
    {
        if(++u8NoOfBufferedCopied == u8NoOfBuffersRequired)
        {
            memcpy(psBindServerBufferedPDURecord->sBindServerBufferedPDUInfo.au8PDU,
                    pu8PDU, (u16PDUSize%MAX_PDU_BIND_QUEUE_PAYLOAD_SIZE));
        }
        else
        {
            memcpy(psBindServerBufferedPDURecord->sBindServerBufferedPDUInfo.au8PDU,
                    pu8PDU, MAX_PDU_BIND_QUEUE_PAYLOAD_SIZE);

            /* Increment the pointer */
            pu8PDU = pu8PDU + MAX_PDU_BIND_QUEUE_PAYLOAD_SIZE;
        }

        /* Remove from the dealloc list */
        psDLISTremove(&psZCL_Common->sBindServerCustomData.lBindServerDeAllocList,
                (DNODE *)psBindServerBufferedPDURecord);

        /* Add to the Alloclist as tail */
        vDLISTaddToTail(&psZCL_Common->sBindServerCustomData.lBindServerAllocList,
                            (DNODE *)psBindServerBufferedPDURecord);

        /* Allocate buffer for Saving PDU */
        psBindServerBufferedPDURecord = (tsBindServerBufferedPDURecord *)psDLISTgetHead(&psZCL_Common->sBindServerCustomData.lBindServerDeAllocList);
    }

    return E_ZCL_SUCCESS;
}


/****************************************************************************
 **
 ** NAME:       vZCL_BindServerTransmissionTimerCallback
 **
 ** DESCRIPTION:
 ** Transmitting Buffered Packets for Bind transmission mode
 **
 ** PARAMETERS:               Name                    Usage

 ** RETURN:
 ** void
 **
 ****************************************************************************/
PUBLIC void vZCL_BindServerTransmissionTimerCallback(void)
{
    tsBindServerBufferedPDURecord     *psBindServerBufferedPDURecord;
    PDUM_thAPduInstance             myPDUM_thAPduInstance;
    tsZCL_Address                    sDestinationAddress;
    uint8                           u8NoOfBuffersRequiredToCopyPDU = 0;
    uint8                           u8NoOfBufferedCopied = 0;
    uint16                          u16PDUSize = 0;
    uint16                          u16ClusterId;
    uint8                           u8SrcEndPointId, u8DstEndPointId;
    uint8                           *pu8PDU = NULL;

    /* Get Head Pointer of Alloc List */
    psBindServerBufferedPDURecord = (tsBindServerBufferedPDURecord *)psDLISTgetHead(&psZCL_Common->sBindServerCustomData.lBindServerAllocList);

    /* Check Pointer */
    if(psBindServerBufferedPDURecord != NULL)
    {
        /* Allocate APDU */
        myPDUM_thAPduInstance = hZCL_AllocateAPduInstance();

        /* Check PDU Handle */
        if(myPDUM_thAPduInstance == PDUM_INVALID_HANDLE)
        {
            return;
        }

        /* collect tx parameters from 1st buffer */
        u16PDUSize = psBindServerBufferedPDURecord->sBindServerBufferedPDUInfo.u16PDUSize;
        u16ClusterId = psBindServerBufferedPDURecord->sBindServerBufferedPDUInfo.u16ClusterId;
        u8SrcEndPointId = psBindServerBufferedPDURecord->sBindServerBufferedPDUInfo.u8SourceEndPointId;
        u8DstEndPointId = psBindServerBufferedPDURecord->sBindServerBufferedPDUInfo.u8DestinationEndPointId;
        sDestinationAddress.eAddressMode = psBindServerBufferedPDURecord->sBindServerBufferedPDUInfo.eAddressMode;

        /* caliculate in how many buffers, the PDU is saved */
        u8NoOfBuffersRequiredToCopyPDU = (u16PDUSize/MAX_PDU_BIND_QUEUE_PAYLOAD_SIZE) +
                                    (u16PDUSize%MAX_PDU_BIND_QUEUE_PAYLOAD_SIZE != 0);

        pu8PDU = (uint8*)PDUM_pvAPduInstanceGetPayload(myPDUM_thAPduInstance);

        while(u8NoOfBufferedCopied < u8NoOfBuffersRequiredToCopyPDU)
        {
            /* Copy Buffered PDU to APDU */
            if(++u8NoOfBufferedCopied == u8NoOfBuffersRequiredToCopyPDU)
            {
                memcpy(pu8PDU, psBindServerBufferedPDURecord->sBindServerBufferedPDUInfo.au8PDU,
                         (u16PDUSize%MAX_PDU_BIND_QUEUE_PAYLOAD_SIZE));
            }
            else
            {
                memcpy(pu8PDU, psBindServerBufferedPDURecord->sBindServerBufferedPDUInfo.au8PDU,
                        MAX_PDU_BIND_QUEUE_PAYLOAD_SIZE);

                pu8PDU = pu8PDU + MAX_PDU_BIND_QUEUE_PAYLOAD_SIZE;
            }

            /* Before Transmission delete from alloc list and to the dealloc list */
            psDLISTremove(&psZCL_Common->sBindServerCustomData.lBindServerAllocList,
                    (DNODE *)psBindServerBufferedPDURecord);

            // add to free list
            vDLISTaddToTail(&psZCL_Common->sBindServerCustomData.lBindServerDeAllocList,
                    (DNODE *)psBindServerBufferedPDURecord);

            /* get the next pointer */
            psBindServerBufferedPDURecord = (tsBindServerBufferedPDURecord *)psDLISTgetHead(&psZCL_Common->sBindServerCustomData.lBindServerAllocList);
        }

        /* Transmit Buffered Pkt */
        eZCL_TransmitDataRequest(
                            myPDUM_thAPduInstance,
                            u16PDUSize,
                            u8SrcEndPointId,
                            u8DstEndPointId,
                            u16ClusterId,
                            &sDestinationAddress);
    }
    else
    {
        return; /* No Buffers in the Queue */
    }
}


/****************************************************************************
 **
 ** NAME:       u8ZCL_GetNumberOfFreeBindServerRecrodEntries
 **
 ** DESCRIPTION:
 ** Retunrs no.of free entries
 **
 ** PARAMETERS:               Name                    Usage
 ** RETURN:
 ** uint8
 **
 ****************************************************************************/
PRIVATE uint8 u8ZCL_GetNumberOfFreeBindServerRecrodEntries(void)
{
    tsBindServerBufferedPDURecord     *psBindServerBufferedPDURecord;
    uint8                               ucCount = 0;

    psBindServerBufferedPDURecord = (tsBindServerBufferedPDURecord *)psDLISTgetHead(&psZCL_Common->sBindServerCustomData.lBindServerDeAllocList);

    while(psBindServerBufferedPDURecord)
    {
        psBindServerBufferedPDURecord = (tsBindServerBufferedPDURecord *)psDLISTgetNext((DNODE *)psBindServerBufferedPDURecord);
        ucCount++;
    }

    return ucCount;
}
#endif /* CLD_BIND_SERVER */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
