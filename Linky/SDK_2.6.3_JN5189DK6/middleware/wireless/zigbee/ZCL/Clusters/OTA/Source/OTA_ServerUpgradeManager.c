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
 * MODULE:             Over The Air Upgrade
 *
 * COMPONENT:          OTA_ServerUpgradeManger.c
 *
 * DESCRIPTION:        Over The Air Upgrade
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "zcl_options.h"
#include "zcl.h"
#include "OTA.h"
#include "OTA_private.h"
#include "dbg.h"

#include <string.h>
#ifdef OTA_SERVER

#ifndef TRACE_OTA_DEBUG
#define TRACE_OTA_DEBUG FALSE
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
PRIVATE bool_t bOtaIsDeviceAuthorised(
                            uint64                     u64IeeeAddress,
                            tsOTA_Common              *psOTA_Common);
#ifdef OTA_PAGE_REQUEST_SUPPORT
PRIVATE teZCL_Status eOta_SendDefaultResponse(
                            uint16                     u16CommandId,
                            uint8                      u8Status,
                            tsZCL_ReceiveEventAddress  sDstAddress,
                            uint8                      u8TransactionSequenceNumber);
#endif
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/


/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/


/****************************************************************************
 **
 ** NAME:       vOtaUpgManServerCommandHandler
 **
 ** DESCRIPTION:
 ** Handle received command on OTA server
 ** PARAMETERS:                           Name                           Usage
 ** ZPS_tsAfEvent                      *pZPSevent                      ZPS event
 ** void                               *pvStruct                       OTA custom data
 ** tsZCL_EndPointDefinition           *psEndPointDefinition           Endpoint definition
 **
 ** RETURN:
 ** None
 ****************************************************************************/
PUBLIC  void vOtaUpgManServerCommandHandler(
                                    ZPS_tsAfEvent               *pZPSevent,
                                    void                        *pvStruct,
                                    tsZCL_EndPointDefinition    *psEndPointDefinition)
{
    tsOTA_Common *psOTA_Common = (tsOTA_Common *)pvStruct;
    uint8 i, u8FoundLocation = 0;
    bool_t bProceed = FALSE;
    tsZCL_Address sZCL_Address;
    uint32 u32FlashOffset;
    tsOTA_ImageHeader sOTAHeader;

#if OTA_ACKS_ON == TRUE
    sZCL_Address.eAddressMode = E_ZCL_AM_SHORT;
    sZCL_Address.uAddress.u16DestinationAddress = psOTA_Common->sReceiveEventAddress.uSrcAddress.u16Addr;
#else
    sZCL_Address.eAddressMode = E_ZCL_AM_SHORT_NO_ACK;
    sZCL_Address.uAddress.u16DestinationAddress = psOTA_Common->sReceiveEventAddress.uSrcAddress.u16Addr;
#endif

#ifdef OTA_PAGE_REQUEST_SUPPORT
    if(NULL == pZPSevent)
    {
        sZCL_Address.eAddressMode = E_ZCL_AM_SHORT_NO_ACK;
    }
#endif
    switch(psOTA_Common->sOTACallBackMessage.eEventId)
    {
        case E_CLD_OTA_COMMAND_QUERY_SPECIFIC_FILE_REQUEST:
        {
            tsOTA_QuerySpecificFileResponsePayload sQSpFileRsp;
            tsOTA_QuerySpecificFileRequestPayload *psQSpFileRequest = &psOTA_Common->sOTACallBackMessage.uMessage.sQuerySpFileRequestPayload;

            bool_t bAllowed = bOtaIsDeviceAuthorised(psOTA_Common->sReceiveEventAddress.uDstAddress.u64Addr, psOTA_Common);
            DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 80 ..\n");
            // Is this device allowed
            if(bAllowed)
            {
                DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 81 ..\n");
                for(i = 0; i< OTA_TOTAL_ACTIVE_IMAGES_ON_SERVER; i++)
                {
                    {
                        eOTA_GetOtaHeader(psEndPointDefinition->u8EndPointNumber, i, &sOTAHeader);
                        // Check details
                        if(sOTAHeader.u16ManufacturerCode == psQSpFileRequest->u16ManufacturerCode &&
                                sOTAHeader.u16ImageType == psQSpFileRequest->u16ImageType &&
                                sOTAHeader.u32FileVersion == psQSpFileRequest->u32FileVersion &&
                                sOTAHeader.u16HeaderControlField&0x2 &&
                                sOTAHeader.u64UpgradeFileDest == psQSpFileRequest->u64RequestNodeAddress &&
                                sOTAHeader.u16StackVersion == psQSpFileRequest->u16CurrentZibgeeStackVersion)
                        {
                            u8FoundLocation = i;
                            bProceed = TRUE;
                            break;
                        }
                    }
                }
                if(bProceed)
                {
                    sQSpFileRsp.u8Status = OTA_STATUS_SUCCESS;
                    sQSpFileRsp.u16ManufacturerCode = psQSpFileRequest->u16ManufacturerCode;
                    sQSpFileRsp.u16ImageType = psQSpFileRequest->u16ImageType;
                    sQSpFileRsp.u32FileVersion = sOTAHeader.u32FileVersion;
                    sQSpFileRsp.u32ImageSize = sOTAHeader.u32TotalImage;
                    eOTA_ServerQuerySpecificFileResponse(psOTA_Common->sReceiveEventAddress.u8DstEndpoint,
                                            psOTA_Common->sReceiveEventAddress.u8SrcEndpoint,
                                            &sZCL_Address,
                                            &sQSpFileRsp,
                                            psOTA_Common->sOTACustomCallBackEvent.u8TransactionSequenceNumber);
                    psOTA_Common->sOTACallBackMessage.aServerPrams[u8FoundLocation].u32CurrentTime =u32ZCL_GetUTCTime();


                }
                else
                {
                    sQSpFileRsp.u8Status = OTA_STATUS_NO_IMAGE_AVAILABLE;
                    eOTA_ServerQuerySpecificFileResponse(psOTA_Common->sReceiveEventAddress.u8DstEndpoint,
                                        psOTA_Common->sReceiveEventAddress.u8SrcEndpoint,
                                        &sZCL_Address,
                                        &sQSpFileRsp,
                                        psOTA_Common->sOTACustomCallBackEvent.u8TransactionSequenceNumber);
                }

            }
            else
            {
                sQSpFileRsp.u8Status = OTA_STATUS_NOT_AUTHORISED;
                eOTA_ServerQuerySpecificFileResponse(psOTA_Common->sReceiveEventAddress.u8DstEndpoint,
                    psOTA_Common->sReceiveEventAddress.u8SrcEndpoint,
                    &sZCL_Address,
                    &sQSpFileRsp,
                    psOTA_Common->sOTACustomCallBackEvent.u8TransactionSequenceNumber);
            }
        }
        break;

        case E_CLD_OTA_COMMAND_UPGRADE_END_REQUEST:
        {
            tsOTA_UpgradeEndResponsePayload sEndResponse;
            tsOTA_UpgradeEndRequestPayload *psEndRequest = &psOTA_Common->sOTACallBackMessage.uMessage.sUpgradeEndRequestPayload;
            if(psEndRequest->u8Status != OTA_STATUS_SUCCESS)
            {
                eZCL_SendDefaultResponse(pZPSevent, OTA_STATUS_SUCCESS);
            }
            else
            {
                for(i = 0; i< OTA_TOTAL_ACTIVE_IMAGES_ON_SERVER; i++)
                {
                    {
                        eOTA_GetOtaHeader(psEndPointDefinition->u8EndPointNumber, i, &sOTAHeader);
                        if(sOTAHeader.u16ManufacturerCode == psEndRequest->u16ManufacturerCode &&
                            sOTAHeader.u16ImageType == psEndRequest->u16ImageType &&
                            sOTAHeader.u32FileVersion == psEndRequest->u32FileVersion)
                        {
                            u8FoundLocation = i;
                            bProceed = TRUE;
                            break;
                        }
                    }
                }
                if(bProceed)
                {
#ifdef OTA_SERVER_ABORT_UPGRADE_TEST_HARNESS
                    eZCL_SendDefaultResponse(pZPSevent, OTA_STATUS_ABORT);
                    return;
#endif
                    sEndResponse.u16ImageType = psEndRequest->u16ImageType;
                    sEndResponse.u16ManufacturerCode = psEndRequest->u16ManufacturerCode ;
                    sEndResponse.u32FileVersion = psEndRequest->u32FileVersion;

                    // If the UpgradeTime or Request Time is Zero, we send present UTC Time as Upgrade Time
                    if( psOTA_Common->sOTACallBackMessage.aServerPrams[u8FoundLocation].u32RequestOrUpgradeTime == 0 )
                        sEndResponse.u32UpgradeTime = u32ZCL_GetUTCTime();
                    else
                        sEndResponse.u32UpgradeTime = psOTA_Common->sOTACallBackMessage.aServerPrams[u8FoundLocation].u32RequestOrUpgradeTime;

                    sEndResponse.u32CurrentTime =  psOTA_Common->sOTACallBackMessage.aServerPrams[u8FoundLocation].u32CurrentTime; //=u32ZCL_GetUTCTime();

                    eOTA_ServerUpgradeEndResponse(psOTA_Common->sReceiveEventAddress.u8DstEndpoint,
                                            psOTA_Common->sReceiveEventAddress.u8SrcEndpoint,
                                            &sZCL_Address,
                                            &sEndResponse,
                                            psOTA_Common->sOTACustomCallBackEvent.u8TransactionSequenceNumber);
                }
            }
        }
        break;
        case E_CLD_OTA_COMMAND_BLOCK_REQUEST:
        {
            uint8 au8Data[OTA_MAX_BLOCK_SIZE];
            tsOTA_ImageBlockResponsePayload sBlockResponse;
            tsOTA_BlockRequest *psBlockRequest = &psOTA_Common->sOTACallBackMessage.uMessage.sBlockRequestPayload;
            DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 38 ..\n");
            for(i = 0; i< OTA_TOTAL_ACTIVE_IMAGES_ON_SERVER; i++)
            {
                {
                    eOTA_GetOtaHeader(psEndPointDefinition->u8EndPointNumber, i, &sOTAHeader);
                    if(sOTAHeader.u16ManufacturerCode == psBlockRequest->u16ManufactureCode &&
                        sOTAHeader.u16ImageType == psBlockRequest->u16ImageType &&
                        sOTAHeader.u32FileVersion == psBlockRequest->u32FileVersion)
                    {
                        u8FoundLocation = i;
                        bProceed = TRUE;
                        break;
                    }
                }
            }

            if(bProceed)
            {
#ifdef OTA_SERVER_ABORT_DOWNLOAD_TEST_HARNESS
                static bool_t bPKTNotSend = TRUE;

                if(bPKTNotSend)
                {
                    sBlockResponse.u8Status = OTA_STATUS_ABORT;

                    eOTA_ServerImageBlockResponse(psOTA_Common->sReceiveEventAddress.u8DstEndpoint,
                        psOTA_Common->sReceiveEventAddress.u8SrcEndpoint,
                        &sZCL_Address,
                        &sBlockResponse,
                        0,
                        psOTA_Common->sOTACustomCallBackEvent.u8TransactionSequenceNumber);

                    bPKTNotSend = FALSE;
                    return;
                }
#endif
                /* check if image is on our flash,
                 * send response otherwise give a call back to application */
                if(u8FoundLocation < OTA_MAX_IMAGES_PER_ENDPOINT)
                {
                    if(psBlockRequest->u32FileOffset >= sOTAHeader.u32TotalImage)
                    {
                        DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 37 ..\n");
#ifndef OTA_PAGE_REQUEST_SUPPORT
                        eZCL_SendDefaultResponse(pZPSevent, OTA_MALFORMED_COMMAND);

#else
                        if(NULL == pZPSevent)
                        {
                            psOTA_Common->sOTACallBackMessage.sPageReqServerParams.bPageReqRespSpacing = FALSE;
                            eZCL_UpdateMsTimer(psEndPointDefinition, FALSE,0);
                            eOta_SendDefaultResponse(E_CLD_OTA_COMMAND_PAGE_REQUEST,OTA_MALFORMED_COMMAND,psOTA_Common->sReceiveEventAddress, psOTA_Common->sOTACustomCallBackEvent.u8TransactionSequenceNumber);
                        }
                        else
                        {
                            eZCL_SendDefaultResponse(pZPSevent, OTA_MALFORMED_COMMAND);
                        }
#endif
                    }
                    else
                    {
                        if((psOTA_Common->sOTACallBackMessage.sWaitForDataParams.bInitialized)&&
                          (psOTA_Common->sOTACallBackMessage.sWaitForDataParams.u16ClientAddress == sZCL_Address.uAddress.u16DestinationAddress))
                        {
                            DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 99 ..\n" );
                            sBlockResponse.u8Status = OTA_STATUS_WAIT_FOR_DATA;
                            sBlockResponse.uMessage.sWaitForData.u32RequestTime = psOTA_Common->sOTACallBackMessage.sWaitForDataParams.sWaitForDataPyld.u32RequestTime;
                            sBlockResponse.uMessage.sWaitForData.u32CurrentTime = psOTA_Common->sOTACallBackMessage.sWaitForDataParams.sWaitForDataPyld.u32CurrentTime;
                            sBlockResponse.uMessage.sWaitForData.u16BlockRequestDelayMs = psOTA_Common->sOTACallBackMessage.sWaitForDataParams.sWaitForDataPyld.u16BlockRequestDelayMs;
                            eOTA_ServerImageBlockResponse(psOTA_Common->sReceiveEventAddress.u8DstEndpoint,
                                                                            psOTA_Common->sReceiveEventAddress.u8SrcEndpoint,
                                                                            &sZCL_Address,
                                                                            &sBlockResponse,
                                                                            0,
                                                                            psOTA_Common->sOTACustomCallBackEvent.u8TransactionSequenceNumber);
                            psOTA_Common->sOTACallBackMessage.sWaitForDataParams.bInitialized = FALSE;


                        }
                        else
                        {
                            teZCL_Status eZCL_Status;
                            uint32 u32Value = sOTAHeader.u32TotalImage - psBlockRequest->u32FileOffset;

                            sBlockResponse.u8Status = OTA_STATUS_SUCCESS;
                            sBlockResponse.uMessage.sBlockPayloadSuccess.u16ImageType = psBlockRequest->u16ImageType;
                            sBlockResponse.uMessage.sBlockPayloadSuccess.u16ManufacturerCode = psBlockRequest->u16ManufactureCode;
                            sBlockResponse.uMessage.sBlockPayloadSuccess.u32FileOffset = psBlockRequest->u32FileOffset;
                            sBlockResponse.uMessage.sBlockPayloadSuccess.u32FileVersion = psBlockRequest->u32FileVersion;

                            if(OTA_MAX_BLOCK_SIZE > psBlockRequest->u8MaxDataSize)
                            {
                                sBlockResponse.uMessage.sBlockPayloadSuccess.u8DataSize = psBlockRequest->u8MaxDataSize;
                            }
                            else
                            {
                                sBlockResponse.uMessage.sBlockPayloadSuccess.u8DataSize = OTA_MAX_BLOCK_SIZE;

                            }
                            if(u32Value < sBlockResponse.uMessage.sBlockPayloadSuccess.u8DataSize)
                            {
                                sBlockResponse.uMessage.sBlockPayloadSuccess.u8DataSize = (uint8) u32Value;
#ifdef OTA_PAGE_REQUEST_SUPPORT
                                 psOTA_Common->sOTACallBackMessage.sPageReqServerParams.sPageReq.u16PageSize = psOTA_Common->sOTACallBackMessage.sPageReqServerParams.u16DataSent + (uint16)u32Value;
#endif
                            }

                            u32FlashOffset = ( (psOTA_Common->sOTACallBackMessage.u8ImageStartSector[u8FoundLocation] *
                                          sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION) + psBlockRequest->u32FileOffset;
                            vOtaFlashLockRead(psEndPointDefinition, psOTA_Common,u32FlashOffset,sBlockResponse.uMessage.sBlockPayloadSuccess.u8DataSize, au8Data);

                            sBlockResponse.uMessage.sBlockPayloadSuccess.pu8Data = au8Data;

                            eZCL_Status = eOTA_ServerImageBlockResponse(psOTA_Common->sReceiveEventAddress.u8DstEndpoint,
                                            psOTA_Common->sReceiveEventAddress.u8SrcEndpoint,
                                            &sZCL_Address,
                                            &sBlockResponse,
                                            sBlockResponse.uMessage.sBlockPayloadSuccess.u8DataSize,
                                            psOTA_Common->sOTACustomCallBackEvent.u8TransactionSequenceNumber);
#ifdef OTA_PAGE_REQUEST_SUPPORT
                            if((E_ZCL_SUCCESS == eZCL_Status)&&(NULL == pZPSevent))
                            {
                                psOTA_Common->sOTACallBackMessage.sPageReqServerParams.u16DataSent +=  sBlockResponse.uMessage.sBlockPayloadSuccess.u8DataSize;
                                psOTA_Common->sOTACallBackMessage.sPageReqServerParams.u8TransactionNumber++;
                                if(psOTA_Common->sOTACallBackMessage.sPageReqServerParams.sPageReq.u16PageSize <= psOTA_Common->sOTACallBackMessage.sPageReqServerParams.u16DataSent)
                                {
                                    psOTA_Common->sOTACallBackMessage.sPageReqServerParams.u16DataSent = 0;
                                    psOTA_Common->sOTACallBackMessage.sPageReqServerParams.sPageReq.u16PageSize = 0;
                                    psOTA_Common->sOTACallBackMessage.sPageReqServerParams.bPageReqRespSpacing = FALSE;
                                    eZCL_UpdateMsTimer(psEndPointDefinition, FALSE,0);
                                }
                            }
#endif
                            DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 39 ..%x %x\n",eZCL_Status,sBlockResponse.uMessage.sBlockPayloadSuccess.u8DataSize );
                            psOTA_Common->sOTACallBackMessage.aServerPrams[u8FoundLocation].u32CurrentTime =u32ZCL_GetUTCTime();
                            psOTA_Common->sOTACallBackMessage.aServerPrams[u8FoundLocation].u8DataSize = sBlockResponse.uMessage.sBlockPayloadSuccess.u8DataSize;
                            psOTA_Common->sOTACallBackMessage.aServerPrams[u8FoundLocation].pu8Data = sBlockResponse.uMessage.sBlockPayloadSuccess.pu8Data;
                        }
                    }
                }
#if OTA_MAX_CO_PROCESSOR_IMAGES != 0
                else
                {
                    /* call back to the application */
                    psOTA_Common->sOTACallBackMessage.eEventId = E_CLD_OTA_INTERNAL_COMMAND_CO_PROCESSOR_IMAGE_BLOCK_REQUEST;
                    psEndPointDefinition->pCallBackFunctions(&psOTA_Common->sOTACustomCallBackEvent);
                }
#endif
            }
            else
            {
#ifndef OTA_PAGE_REQUEST_SUPPORT
                eZCL_SendDefaultResponse(pZPSevent, OTA_STATUS_NO_IMAGE_AVAILABLE);

#else
                if(NULL == pZPSevent)
                {
                    psOTA_Common->sOTACallBackMessage.sPageReqServerParams.bPageReqRespSpacing = FALSE;
                    eZCL_UpdateMsTimer(psEndPointDefinition, FALSE,0);
                    eOta_SendDefaultResponse(E_CLD_OTA_COMMAND_PAGE_REQUEST,OTA_STATUS_NO_IMAGE_AVAILABLE,psOTA_Common->sReceiveEventAddress, psOTA_Common->sOTACustomCallBackEvent.u8TransactionSequenceNumber);
                }
                else
                {
                    eZCL_SendDefaultResponse(pZPSevent, OTA_STATUS_NO_IMAGE_AVAILABLE);
                }
#endif
            }
        }
        break;
        case E_CLD_OTA_COMMAND_QUERY_NEXT_IMAGE_REQUEST:
        {
            tsOTA_QueryImageResponse sImageResponse;
            tsOTA_QueryImageRequest *psImageRequest = &psOTA_Common->sOTACallBackMessage.uMessage.sQueryImagePayload;

            bool_t bAllowed = bOtaIsDeviceAuthorised(psOTA_Common->sReceiveEventAddress.uDstAddress.u64Addr, psOTA_Common);
            DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 40 ..\n");
            // Is this device allowed
            if(bAllowed)
            {

                DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 41 ..\n");
                for(i = 0; i< OTA_TOTAL_ACTIVE_IMAGES_ON_SERVER; i++)
                {
                    {
                        eOTA_GetOtaHeader(psEndPointDefinition->u8EndPointNumber, i, &sOTAHeader);
                        // Check details
                        if(psImageRequest->u16ManufacturerCode == sOTAHeader.u16ManufacturerCode &&
                                psImageRequest->u16ImageType == sOTAHeader.u16ImageType)
                        {

                            u8FoundLocation = i;
#ifdef OTA_CLD_HARDWARE_VERSIONS_PRESENT
                            /* check HW version present in the header */
                            if(psImageRequest->u8FieldControl == 0x01)
                            {
                                if(psImageRequest->u16HardwareVersion >= sOTAHeader.u16MinimumHwVersion ||
                                        psImageRequest->u16HardwareVersion <= sOTAHeader.u16MaxHwVersion)
                                {
                                    bProceed = TRUE;
                                }
                                else
                                {
                                    bProceed = FALSE;
                                }
                            }
                            else
                            {
                                bProceed = TRUE;
                            }
#else
                            bProceed = TRUE;
#endif
                        }
                    }
                    if(bProceed)
                    {
                        break;
                    }
                }
                if(bProceed)
                {
                    sImageResponse.u8Status = OTA_STATUS_SUCCESS;
                    sImageResponse.u16ManufacturerCode = psImageRequest->u16ManufacturerCode;
                    sImageResponse.u16ImageType = psImageRequest->u16ImageType;
                    sImageResponse.u32FileVersion = sOTAHeader.u32FileVersion;
                    sImageResponse.u32ImageSize = sOTAHeader.u32TotalImage;
                    eOTA_ServerQueryNextImageResponse(psOTA_Common->sReceiveEventAddress.u8DstEndpoint,
                                            psOTA_Common->sReceiveEventAddress.u8SrcEndpoint,
                                            &sZCL_Address,
                                            &sImageResponse,
                                            psOTA_Common->sOTACustomCallBackEvent.u8TransactionSequenceNumber);
                    psOTA_Common->sOTACallBackMessage.aServerPrams[u8FoundLocation].u32CurrentTime =u32ZCL_GetUTCTime();


                }
                else
                {
                    sImageResponse.u8Status = OTA_STATUS_NO_IMAGE_AVAILABLE;
                    eOTA_ServerQueryNextImageResponse(psOTA_Common->sReceiveEventAddress.u8DstEndpoint,
                                        psOTA_Common->sReceiveEventAddress.u8SrcEndpoint,
                                        &sZCL_Address,
                                        &sImageResponse,
                                        psOTA_Common->sOTACustomCallBackEvent.u8TransactionSequenceNumber);
                }

            }
            else
            {
                sImageResponse.u8Status = OTA_STATUS_NOT_AUTHORISED;
                eOTA_ServerQueryNextImageResponse(psOTA_Common->sReceiveEventAddress.u8DstEndpoint,
                    psOTA_Common->sReceiveEventAddress.u8SrcEndpoint,
                    &sZCL_Address,
                    &sImageResponse,
                    psOTA_Common->sOTACustomCallBackEvent.u8TransactionSequenceNumber);
            }
        }
        break;
        default:
            break;
    }
}
/****************************************************************************
 **
 ** NAME:       bOtaIsDeviceAuthorised
 **
 ** DESCRIPTION:
 ** verify if client device is authorised
 ** PARAMETERS:                           Name                           Usage
 ** uint64                           u64IeeeAddress                   IEEE address of client
 ** tsOTA_Common                    *psOTA_Common                     OTA custom data
 ** RETURN:
 ** bool_t
 ****************************************************************************/
PRIVATE  bool_t bOtaIsDeviceAuthorised(
                                   uint64         u64IeeeAddress,
                                   tsOTA_Common  *psOTA_Common)
{
    uint8 i;
    if(psOTA_Common->sOTACallBackMessage.sAuthStruct.eState == E_CLD_OTA_STATE_ALLOW_ALL)
    {
        return TRUE;
    }
    else
    {
        for(i = 0; i < psOTA_Common->sOTACallBackMessage.sAuthStruct.u8Size; i++)
        {
            if(u64IeeeAddress ==  psOTA_Common->sOTACallBackMessage.sAuthStruct.pu64WhiteList[i])
            {
               return TRUE;
            }
        }
        return FALSE;
    }
}
#ifdef OTA_PAGE_REQUEST_SUPPORT
/****************************************************************************
 **
 ** NAME:       vOtaHandleTimedPageRequest
 **
 ** DESCRIPTION:
 ** Handle page request after timeout
 ** PARAMETERS:                           Name                           Usage
 ** uint8                               u8SourceEndPointId             source endpoint id
 ** RETURN:
 ** None
 ****************************************************************************/
PUBLIC  void vOtaHandleTimedPageRequest(uint8 u8SourceEndPointId)
{
    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;

    if((eStatus = eOtaFindCluster(u8SourceEndPointId,
                           &psEndPointDefinition,
                           &psClusterInstance,
                           &psCustomData,
                           TRUE))
                           == E_ZCL_SUCCESS)
    {
        if(psCustomData->sOTACallBackMessage.sPageReqServerParams.sPageReq.u16PageSize > psCustomData->sOTACallBackMessage.sPageReqServerParams.u16DataSent)
        {

            tsOTA_Common *psOTA_Common =((tsOTA_Common *)psClusterInstance->pvEndPointCustomStructPtr);
                // get EP mutex
            #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif



            psOTA_Common->sOTACallBackMessage.uMessage.sBlockRequestPayload.u8FieldControl = psCustomData->sOTACallBackMessage.sPageReqServerParams.sPageReq.u8FieldControl;
            psOTA_Common->sOTACallBackMessage.uMessage.sBlockRequestPayload.u16ManufactureCode = psCustomData->sOTACallBackMessage.sPageReqServerParams.sPageReq.u16ManufactureCode;
            psOTA_Common->sOTACallBackMessage.uMessage.sBlockRequestPayload.u16ImageType = psCustomData->sOTACallBackMessage.sPageReqServerParams.sPageReq.u16ImageType;
            psOTA_Common->sOTACallBackMessage.uMessage.sBlockRequestPayload.u32FileVersion = psCustomData->sOTACallBackMessage.sPageReqServerParams.sPageReq.u32FileVersion;
            psOTA_Common->sOTACallBackMessage.uMessage.sBlockRequestPayload.u32FileOffset = psCustomData->sOTACallBackMessage.sPageReqServerParams.sPageReq.u32FileOffset + psCustomData->sOTACallBackMessage.sPageReqServerParams.u16DataSent;

            if(OTA_MAX_BLOCK_SIZE > psCustomData->sOTACallBackMessage.sPageReqServerParams.sPageReq.u8MaxDataSize)
            {
                psOTA_Common->sOTACallBackMessage.uMessage.sBlockRequestPayload.u8MaxDataSize = psCustomData->sOTACallBackMessage.sPageReqServerParams.sPageReq.u8MaxDataSize;
            }
            else
            {
                psOTA_Common->sOTACallBackMessage.uMessage.sBlockRequestPayload.u8MaxDataSize = OTA_MAX_BLOCK_SIZE;

            }
            if((psCustomData->sOTACallBackMessage.sPageReqServerParams.sPageReq.u16PageSize -
                    psCustomData->sOTACallBackMessage.sPageReqServerParams.u16DataSent) <
                    psOTA_Common->sOTACallBackMessage.uMessage.sBlockRequestPayload.u8MaxDataSize)
            {
                psOTA_Common->sOTACallBackMessage.uMessage.sBlockRequestPayload.u8MaxDataSize =
                    psCustomData->sOTACallBackMessage.sPageReqServerParams.sPageReq.u16PageSize -
                      psCustomData->sOTACallBackMessage.sPageReqServerParams.u16DataSent;
            }

            if(psCustomData->sOTACallBackMessage.sPageReqServerParams.sPageReq.u8FieldControl & 0x1)
            {
                psOTA_Common->sOTACallBackMessage.uMessage.sBlockRequestPayload.u64RequestNodeAddress = psCustomData->sOTACallBackMessage.sPageReqServerParams.sPageReq.u64RequestNodeAddress;
            }
            memcpy(&psOTA_Common->sReceiveEventAddress, &psCustomData->sOTACallBackMessage.sPageReqServerParams.sReceiveEventAddress, sizeof(tsZCL_ReceiveEventAddress));
            psOTA_Common->sOTACallBackMessage.eEventId = E_CLD_OTA_COMMAND_BLOCK_REQUEST;
            psOTA_Common->sOTACustomCallBackEvent.u8TransactionSequenceNumber = psOTA_Common->sOTACallBackMessage.sPageReqServerParams.u8TransactionNumber;
            vOtaUpgManServerCommandHandler(NULL,psOTA_Common, psEndPointDefinition);
            psCustomData->sOTACallBackMessage.sPageReqServerParams.bPageReqRespSpacing = TRUE;
            // release mutex
            #ifndef COOPERATIVE
                eZCL_ReleaseMutex(psEndPointDefinition);
            #endif

        }
        else
        {
            DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 555...%d .\n",psCustomData->sOTACallBackMessage.sPageReqServerParams.u16DataSent);
        }
    }
}

/****************************************************************************
 **
 ** NAME:       eOta_SendDefaultResponse
 **
 ** DESCRIPTION:
 ** send default response
 ** PARAMETERS:                           Name                           Usage
 ** uint16                            u16CommandId                   Command id
 ** uint8                             u8Status                       status to send
 ** tsZCL_ReceiveEventAddress         sDstAddress                    address
 ** uint8                             u8TransactionSequenceNumber    Transaction Sequence Number
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
PRIVATE  teZCL_Status eOta_SendDefaultResponse(
                                          uint16                     u16CommandId,
                                          uint8                      u8Status,
                                          tsZCL_ReceiveEventAddress  sDstAddress,
                                          uint8                      u8TransactionSequenceNumber)
{
    PDUM_thAPduInstance myPDUM_thAPduInstance;
     uint16 u16offset, u16payloadSize;
    // build command packet
    tsZCL_Address sZCL_Address;
#if OTA_ACKS_ON ==  TRUE
    sZCL_Address.eAddressMode = E_ZCL_AM_SHORT;
    sZCL_Address.uAddress.u16DestinationAddress = sDstAddress.uSrcAddress.u16Addr;
#else
    sZCL_Address.eAddressMode = E_ZCL_AM_SHORT_NO_ACK;
    sZCL_Address.uAddress.u16DestinationAddress = sDstAddress.uSrcAddress.u16Addr;
#endif
    // get buffer
    myPDUM_thAPduInstance = hZCL_AllocateAPduInstance();

    if(myPDUM_thAPduInstance == PDUM_INVALID_HANDLE)
    {
        return(E_ZCL_ERR_ZBUFFER_FAIL);
    }

    // write command header - using old values where appropriate
    u16offset = u16ZCL_WriteCommandHeader(
                                       myPDUM_thAPduInstance,
                                       eFRAME_TYPE_COMMAND_ACTS_ACCROSS_ENTIRE_PROFILE,
                                       FALSE,
                                       0,
                                       TRUE,
                                       TRUE,
                                       u8TransactionSequenceNumber,
                                       E_ZCL_DEFAULT_RESPONSE);

    // calculate payload size
    u16payloadSize = u16offset + 2;

    // write payload - the command ID of the incoming command that caused the response followed by the error code
    u16offset +=  u16ZCL_APduInstanceWriteNBO(myPDUM_thAPduInstance, u16offset, E_ZCL_UINT8, &u16CommandId);
    u16offset +=  u16ZCL_APduInstanceWriteNBO(myPDUM_thAPduInstance, u16offset, E_ZCL_UINT8, &u8Status);

    // transmit request - reverse event parameters - src and dst
    if (eZCL_TransmitDataRequest(myPDUM_thAPduInstance,
                                u16payloadSize,
                                sDstAddress.u8DstEndpoint,
                                sDstAddress.u8SrcEndpoint,
                                OTA_CLUSTER_ID,
                                &sZCL_Address) != E_ZCL_SUCCESS)
    {
        return(E_ZCL_ERR_ZTRANSMIT_FAIL);
    }
    return(E_ZCL_SUCCESS);
}
#endif  /*#ifdef OTA_PAGE_REQUEST_SUPPORT*/
#endif
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
