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
 * COMPONENT:          OTA_client.c
 *
 * DESCRIPTION:        Over The Air Upgrade
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>
#include "zcl.h"
#include "zcl_options.h"
#include "OTA.h"
#include "OTA_private.h"
#include "zps_apl_af.h"
#include "dbg.h"
#if (defined JENNIC_CHIP_FAMILY_JN518x)
#include "fsl_wwdt.h"
#include "OtaSupport.h"
#ifdef WEAK
#undef WEAK
#endif
#include "rom_psector.h"
#include "rom_api.h"
#endif
#ifndef TRACE_OTA_DEBUG
#define TRACE_OTA_DEBUG FALSE
#endif
#ifndef TRACE_VERIF
#define TRACE_VERIF FALSE
#endif

#ifdef OTA_CLIENT
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
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 **
 ** NAME:       eOTA_ClientQueryNextImageRequest
 **
 ** DESCRIPTION:
 ** sends query next image request command
 **
 ** PARAMETERS:               Name                           Usage
 ** uint8                    u8SourceEndPointId            Source EP Id
 ** uint8                    u8DestinationEndPointId       Destination EP Id
 ** tsZCL_Address           *psDestinationAddress          Destination Address
 ** tsOTA_QueryImageRequest *psQueryImageRequest           command payload
 **
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_ClientQueryNextImageRequest(
                    uint8 u8SourceEndpoint,
                    uint8 u8DestinationEndpoint,
                    tsZCL_Address *psDestinationAddress,
                    tsOTA_QueryImageRequest *psQueryImageRequest)
{
    teZCL_Status eZCL_Status;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;
    uint8 u8SequenceNumber ;
    bool_t bDisableDefaultResponseState=FALSE;
//    tsZCL_TxPayloadItem asActPayloadDefinition[5] = {0};
    uint8 u8ItemsInPayload = 0;
    
    if((eZCL_Status =
        eOtaFindCluster(u8SourceEndpoint,
                         &psEndPointDefinition,
                           &psClusterInstance,
                           &psCustomData,
                           FALSE))
                           == E_ZCL_SUCCESS)
    {
        if (psClusterInstance->bIsServer)
        {
            eZCL_Status = E_ZCL_FAIL;
        }
        else
        {
                tsZCL_TxPayloadItem asPayloadDefinition[] = { {1, E_ZCL_UINT8,   &psQueryImageRequest->u8FieldControl},
                                                              {1, E_ZCL_UINT16,   &psQueryImageRequest->u16ManufacturerCode},
                                                              {1, E_ZCL_UINT16,   &psQueryImageRequest->u16ImageType},
                                                              {1, E_ZCL_UINT32,   &psQueryImageRequest->u32CurrentFileVersion},
                                                              {1, E_ZCL_UINT16,   &psQueryImageRequest->u16HardwareVersion}
                                                            };

            bDisableDefaultResponseState = psEndPointDefinition->bDisableDefaultResponse;
            psEndPointDefinition->bDisableDefaultResponse = OTA_CLIENT_DISABLE_DEFAULT_RESPONSE;

            if(psQueryImageRequest->u8FieldControl& 0x1)
            {
                u8ItemsInPayload=5;
            }
            else
            {
                u8ItemsInPayload=4;             
            }
            
            eZCL_Status = eZCL_CustomCommandSend(u8SourceEndpoint,
                             u8DestinationEndpoint,
                             psDestinationAddress,
                             OTA_CLUSTER_ID,
                             FALSE,
                             E_CLD_OTA_COMMAND_QUERY_NEXT_IMAGE_REQUEST,
                             &u8SequenceNumber,
                             asPayloadDefinition,//asActPayloadDefinition,
                             FALSE,
                             0,
                             u8ItemsInPayload);
        }
    }
    

    if(E_ZCL_SUCCESS == eZCL_Status)
    {
        eOtaClientReqSeqNoUpdate(u8SourceEndpoint,u8SequenceNumber);
    }

    psEndPointDefinition->bDisableDefaultResponse = bDisableDefaultResponseState;
    return eZCL_Status;
}

/****************************************************************************
 **
 ** NAME:       eOTA_ClientImageBlockRequest
 **
 ** DESCRIPTION:
 ** sends image block request command
 **
 ** PARAMETERS:               Name                           Usage
 ** uint8                    u8SourceEndPointId            Source EP Id
 ** uint8                    u8DestinationEndPointId       Destination EP Id
 ** tsZCL_Address           *psDestinationAddress          Destination Address
 ** tsOTA_BlockRequest      *psOtaBlockRequest             command payload
 **
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_ClientImageBlockRequest(
                    uint8 u8SourceEndpoint,
                    uint8 u8DestinationEndpoint,
                    tsZCL_Address *psDestinationAddress,
                    tsOTA_BlockRequest *psOtaBlockRequest)
{
    teZCL_Status eZCL_Status;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;
    uint8 u8SequenceNumber;
    bool_t bDisableDefaultResponseState=FALSE;

    uint8 u8ItemsInPayload = 0;
    
    if((eZCL_Status =
        eOtaFindCluster(u8SourceEndpoint,
                           &psEndPointDefinition,
                           &psClusterInstance,
                           &psCustomData,
                           FALSE))
                           == E_ZCL_SUCCESS)
    {
        if (psClusterInstance->bIsServer)
        {
            eZCL_Status = E_ZCL_FAIL;
            DBG_vPrintf(TRACE_OTA_DEBUG,"ERROR:SERVER node trying to do request\n");
        }
        else
        {
            bDisableDefaultResponseState = psEndPointDefinition->bDisableDefaultResponse;
            psEndPointDefinition->bDisableDefaultResponse = OTA_CLIENT_DISABLE_DEFAULT_RESPONSE;

            tsZCL_TxPayloadItem asPayloadDefinition[] = { {1, E_ZCL_UINT8,   &psOtaBlockRequest->u8FieldControl},
                                  {1, E_ZCL_UINT16,   &psOtaBlockRequest->u16ManufactureCode},
                                  {1, E_ZCL_UINT16,   &psOtaBlockRequest->u16ImageType},
                                  {1, E_ZCL_UINT32,   &psOtaBlockRequest->u32FileVersion},
                                  {1, E_ZCL_UINT32,   &psOtaBlockRequest->u32FileOffset},
                                  {1, E_ZCL_UINT8,    &psOtaBlockRequest->u8MaxDataSize},
                                  {1, E_ZCL_UINT64,   &psOtaBlockRequest->u64RequestNodeAddress},
                                  {1, E_ZCL_UINT16,   &psOtaBlockRequest->u16BlockRequestDelay}
                                };
            
            if((psOtaBlockRequest->u8FieldControl& 0x1)&&(psOtaBlockRequest->u8FieldControl& 0x2))
            {
                u8ItemsInPayload=8;                                
            }
            else if(psOtaBlockRequest->u8FieldControl& 0x1)
            {
                u8ItemsInPayload=7;                             
            }
            else if(psOtaBlockRequest->u8FieldControl& 0x2)
            {
                tsZCL_TxPayloadItem asActPayloadDefinition[] = { 
                                                  {1, E_ZCL_UINT16,   &psOtaBlockRequest->u16BlockRequestDelay}
                                                };
                u8ItemsInPayload=7;
                memcpy(&asPayloadDefinition[6],asActPayloadDefinition,sizeof(asActPayloadDefinition));
            }
            else
            {
                u8ItemsInPayload=6;                                          
            }
            
            eZCL_Status = eZCL_CustomCommandSend(u8SourceEndpoint,
                            u8DestinationEndpoint,
                            psDestinationAddress,
                            OTA_CLUSTER_ID,
                            FALSE,
                            E_CLD_OTA_COMMAND_BLOCK_REQUEST,
                            &u8SequenceNumber,
                            asPayloadDefinition,
                            FALSE,
                            0,
                            u8ItemsInPayload);

            if(eZCL_Status != E_ZCL_SUCCESS)
            {
                DBG_vPrintf(TRACE_OTA_DEBUG,"ERROR: %x status returned %x stack error\n", eZCL_Status , eZCL_GetLastZpsError());
            }
        }
    }
    if(E_ZCL_SUCCESS == eZCL_Status)
    {
        eOtaClientReqSeqNoUpdate(u8SourceEndpoint,u8SequenceNumber);
    }

    psEndPointDefinition->bDisableDefaultResponse = bDisableDefaultResponseState;
    return eZCL_Status;
}

/****************************************************************************
 **
 ** NAME:       eOTA_ClientImagePageRequest
 **
 ** DESCRIPTION:
 ** sends image page request command
 **
 ** PARAMETERS:               Name                           Usage
 ** uint8                    u8SourceEndPointId            Source EP Id
 ** uint8                    u8DestinationEndPointId       Destination EP Id
 ** tsZCL_Address           *psDestinationAddress          Destination Address
 ** tsOTA_ImagePageRequest  *psOtaPageRequest              command payload
 **
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_ClientImagePageRequest(
                    uint8 u8SourceEndpoint,
                    uint8 u8DestinationEndpoint,
                    tsZCL_Address *psDestinationAddress,
                    tsOTA_ImagePageRequest *psOtaPageRequest)
{
    teZCL_Status eZCL_Status;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;
    uint8 u8SequenceNumber;
    bool_t bDisableDefaultResponseState=FALSE;

    uint8 u8ItemsInPayload = 0;

    if((eZCL_Status =
        eOtaFindCluster(u8SourceEndpoint,
                           &psEndPointDefinition,
                           &psClusterInstance,
                           &psCustomData,
                           FALSE))
                           == E_ZCL_SUCCESS)
    {
        if (psClusterInstance->bIsServer)
        {
            eZCL_Status = E_ZCL_FAIL;
        }
        else
        {
            bDisableDefaultResponseState = psEndPointDefinition->bDisableDefaultResponse;
            psEndPointDefinition->bDisableDefaultResponse = OTA_CLIENT_DISABLE_DEFAULT_RESPONSE;
            tsZCL_TxPayloadItem asPayloadDefinition[] = { {1, E_ZCL_UINT8,   &psOtaPageRequest->u8FieldControl},
                                              {1, E_ZCL_UINT16,   &psOtaPageRequest->u16ManufactureCode},
                                              {1, E_ZCL_UINT16,   &psOtaPageRequest->u16ImageType},
                                              {1, E_ZCL_UINT32,   &psOtaPageRequest->u32FileVersion},
                                              {1, E_ZCL_UINT32,   &psOtaPageRequest->u32FileOffset},
                                              {1, E_ZCL_UINT8,    &psOtaPageRequest->u8MaxDataSize},
                                              {1, E_ZCL_UINT16,   &psOtaPageRequest->u16PageSize},
                                              {1, E_ZCL_UINT16,   &psOtaPageRequest->u16ResponseSpacing},
                                              {1, E_ZCL_UINT64,   &psOtaPageRequest->u64RequestNodeAddress},
                                            };

            if(psOtaPageRequest->u8FieldControl& 0x1)
            {
                u8ItemsInPayload = 9;
            }
            else
            {
                u8ItemsInPayload = 8;
            }
            eZCL_Status = eZCL_CustomCommandSend(u8SourceEndpoint,
                                     u8DestinationEndpoint,
                                     psDestinationAddress,
                                     OTA_CLUSTER_ID,
                                     FALSE,
                                     E_CLD_OTA_COMMAND_PAGE_REQUEST,
                                     &u8SequenceNumber,
                                     asPayloadDefinition,//asActPayloadDefinition,
                                     FALSE,
                                     0,
                                     u8ItemsInPayload);
        }
    }
    if(E_ZCL_SUCCESS == eZCL_Status)
    {
        eOtaClientReqSeqNoUpdate(u8SourceEndpoint,u8SequenceNumber);
    }

    psEndPointDefinition->bDisableDefaultResponse = bDisableDefaultResponseState;
    return eZCL_Status;
}

/****************************************************************************
 **
 ** NAME:       eOTA_ClientUpgradeEndRequest
 **
 ** DESCRIPTION:
 ** sends upgrade end request command
 **
 ** PARAMETERS:                         Name                           Usage
 ** uint8                           u8SourceEndPointId            Source EP Id
 ** uint8                           u8DestinationEndPointId       Destination EP Id
 ** tsZCL_Address                  *psDestinationAddress          Destination Address
 ** tsOTA_UpgradeEndRequestPayload *psUpgradeEndRequestPayload    Command Payload
 **
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_ClientUpgradeEndRequest(
                    uint8 u8SourceEndpoint,
                    uint8 u8DestinationEndpoint,
                    tsZCL_Address *psDestinationAddress,
                    tsOTA_UpgradeEndRequestPayload *psUpgradeEndRequestPayload)
{
    teZCL_Status eZCL_Status;
    tsZCL_ClusterInstance *psClusterInstance = NULL;
    tsZCL_EndPointDefinition *psEndPointDefinition = NULL;
    tsOTA_Common *psCustomData = NULL;
    uint8 u8SequenceNumber;
    bool_t bDisableDefaultResponseState=FALSE;

    tsZCL_TxPayloadItem asPayloadDefinition[] = { {1, E_ZCL_UINT8,   &psUpgradeEndRequestPayload->u8Status},
                                                  {1, E_ZCL_UINT16,   &psUpgradeEndRequestPayload->u16ManufacturerCode},
                                                  {1, E_ZCL_UINT16,   &psUpgradeEndRequestPayload->u16ImageType},
                                                  {1, E_ZCL_UINT32,   &psUpgradeEndRequestPayload->u32FileVersion},
                                                };

    if((eZCL_Status =
        eOtaFindCluster(u8SourceEndpoint,
                         &psEndPointDefinition,
                         &psClusterInstance,
                           &psCustomData,
                           FALSE))
                           == E_ZCL_SUCCESS)
    {
        if (psClusterInstance->bIsServer)
        {
            eZCL_Status = E_ZCL_FAIL;
        }
        else
        {
            /* get and save default response state */
            bDisableDefaultResponseState = psEndPointDefinition->bDisableDefaultResponse;
            psEndPointDefinition->bDisableDefaultResponse = OTA_CLIENT_DISABLE_DEFAULT_RESPONSE;

            eZCL_Status = eZCL_CustomCommandSend(u8SourceEndpoint,
                                                 u8DestinationEndpoint,
                                                 psDestinationAddress,
                                                 OTA_CLUSTER_ID,
                                                 FALSE,
                                                 E_CLD_OTA_COMMAND_UPGRADE_END_REQUEST,
                                                 &u8SequenceNumber,
                                                 asPayloadDefinition,
                                                 FALSE,
                                                 0,
                                                 sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
            /* restore default response setting */
            psEndPointDefinition->bDisableDefaultResponse = bDisableDefaultResponseState;
        }
    }

    if(E_ZCL_SUCCESS == eZCL_Status)
    {
        eOtaClientReqSeqNoUpdate(u8SourceEndpoint, u8SequenceNumber);
    }

    return eZCL_Status;
}

/****************************************************************************
 **
 ** NAME:       eOTA_ClientQuerySpecificFileRequest
 **
 ** DESCRIPTION:
 ** sends Query Specific File request command
 **
 ** PARAMETERS:                         Name                           Usage
 ** uint8                           u8SourceEndPointId            Source EP Id
 ** uint8                           u8DestinationEndPointId       Destination EP Id
 ** tsZCL_Address                  *psDestinationAddress          Destination Address
 ** tsOTA_QuerySpecificFileRequestPayload *psQuerySpecificFileRequestPayload    Command Payload
 **
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_ClientQuerySpecificFileRequest(
                    uint8 u8SourceEndpoint,
                    uint8 u8DestinationEndpoint,
                    tsZCL_Address *psDestinationAddress,
                    tsOTA_QuerySpecificFileRequestPayload *psQuerySpecificFileRequestPayload)
{
    teZCL_Status eZCL_Status;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;
    uint8 u8SequenceNumber;
    bool_t bDisableDefaultResponseState=FALSE;

    tsZCL_TxPayloadItem asPayloadDefinition[] = { {1, E_ZCL_UINT64,   &psQuerySpecificFileRequestPayload->u64RequestNodeAddress},
                                                  {1, E_ZCL_UINT16,   &psQuerySpecificFileRequestPayload->u16ManufacturerCode},
                                                  {1, E_ZCL_UINT16,   &psQuerySpecificFileRequestPayload->u16ImageType},
                                                  {1, E_ZCL_UINT32,   &psQuerySpecificFileRequestPayload->u32FileVersion},
                                                  {1, E_ZCL_UINT16,   &psQuerySpecificFileRequestPayload->u16CurrentZibgeeStackVersion}
                                                };

    if((eZCL_Status =
        eOtaFindCluster(u8SourceEndpoint,
                         &psEndPointDefinition,
                         &psClusterInstance,
                           &psCustomData,
                           FALSE))
                           == E_ZCL_SUCCESS)
    {
        if (psClusterInstance->bIsServer)
        {
            eZCL_Status = E_ZCL_FAIL;
        }
        else
        {
            bDisableDefaultResponseState = psEndPointDefinition->bDisableDefaultResponse;
            psEndPointDefinition->bDisableDefaultResponse = OTA_CLIENT_DISABLE_DEFAULT_RESPONSE;

            eZCL_Status = eZCL_CustomCommandSend(u8SourceEndpoint,
                                                 u8DestinationEndpoint,
                                                 psDestinationAddress,
                                                 OTA_CLUSTER_ID,
                                                 FALSE,
                                                 E_CLD_OTA_COMMAND_QUERY_SPECIFIC_FILE_REQUEST,
                                                 &u8SequenceNumber,
                                                 asPayloadDefinition,
                                                 FALSE,
                                                 0,
                                                 sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
        }
    }
    if(E_ZCL_SUCCESS == eZCL_Status)
    {
        eOtaClientReqSeqNoUpdate(u8SourceEndpoint,u8SequenceNumber);
    }

    psEndPointDefinition->bDisableDefaultResponse = bDisableDefaultResponseState;
    return eZCL_Status;
}
/****************************************************************************
 **
 ** NAME:       eOTA_SpecificFileUpgradeEndRequest
 **
 ** DESCRIPTION:
 ** sends Query Specific File Upgrade End request command
 **
 ** PARAMETERS:                         Name                           Usage
 ** uint8                           u8SourceEndPointId            Source EP Id
 ** uint8                           u8Status                      Status
 **
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_SpecificFileUpgradeEndRequest(uint8 u8SourceEndPointId, uint8 u8Status)
{
    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;
    tsOTA_UpgradeEndRequestPayload sEndRequest;
    tsZCL_Address sZCL_Address;

    #if OTA_ACKS_ON == TRUE
        sZCL_Address.eAddressMode = E_ZCL_AM_SHORT;
    #else
        sZCL_Address.eAddressMode = E_ZCL_AM_SHORT_NO_ACK;
    #endif
    
    if((eStatus = eOtaFindCluster(u8SourceEndPointId,
                                  &psEndPointDefinition,
                                  &psClusterInstance,
                                  &psCustomData,
                                  FALSE))
                                  == E_ZCL_SUCCESS)
    {
        sZCL_Address.uAddress.u16DestinationAddress = psCustomData->sOTACallBackMessage.sPersistedData.u16ServerShortAddress;
        if(!psCustomData->sOTACallBackMessage.sPersistedData.bIsSpecificFile)
        {
            return(E_ZCL_FAIL);
        }

        sEndRequest.u32FileVersion = psCustomData->sOTACallBackMessage.sPersistedData.sAttributes.u32DownloadedFileVersion;

        sEndRequest.u16ImageType = psCustomData->sOTACallBackMessage.sPersistedData.sAttributes.u16ImageType;

        sEndRequest.u16ManufacturerCode = psCustomData->sOTACallBackMessage.sPersistedData.sAttributes.u16ManfId;
        sEndRequest.u8Status = u8Status;
        if(u8Status == OTA_STATUS_SUCCESS)
        {
             /* change state to dowmload complete */
            vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_DL_COMPLETE, psEndPointDefinition, psCustomData);
        }
        else
        {
            /* change state to normal */
            vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_NORMAL, psEndPointDefinition, psCustomData);
        }
         /* Send block end request */
        eStatus = eOTA_ClientUpgradeEndRequest(psCustomData->sReceiveEventAddress.u8DstEndpoint,
                                     psCustomData->sReceiveEventAddress.u8SrcEndpoint,
                                     &sZCL_Address,
                                     &sEndRequest);
        if(u8Status == OTA_STATUS_SUCCESS)
        {  /* if it is a success case then enable retries and wait for response */
            psCustomData->sOTACallBackMessage.sPersistedData.u8Retry = 0;
            psCustomData->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime()+ OTA_TIME_INTERVAL_BETWEEN_END_REQUEST_RETRIES+1;
        }
        else
        {
            psCustomData->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = 0;
        }
        /* request has been sent make sure we poll for the response too otherwise might miss it
           only valid for end device*/
        eOtaSetEventTypeAndGiveCallBack(psCustomData, E_CLD_OTA_INTERNAL_COMMAND_POLL_REQUIRED,psEndPointDefinition);
    }
    return eStatus;
}
#if (OTA_MAX_CO_PROCESSOR_IMAGES != 0)
/****************************************************************************
 **
 ** NAME:       eOTA_CoProcessorUpgradeEndRequest
 **
 ** DESCRIPTION:
 ** sends upgrade end request command for the co-processor
 **
 ** PARAMETERS:                         Name                           Usage
 ** uint8                           u8SourceEndPointId            Source EP Id
 ** uint8                           u8Status                      status
 **
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_CoProcessorUpgradeEndRequest(uint8 u8SourceEndPointId, uint8 u8Status)
{
    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;
    tsOTA_UpgradeEndRequestPayload sEndRequest;
    tsOTA_ImageHeader sOTAHeader ;
    tsZCL_Address sZCL_Address;


    if((eStatus = eOtaFindCluster(u8SourceEndPointId,
                                  &psEndPointDefinition,
                                  &psClusterInstance,
                                  &psCustomData,
                                  FALSE))
                                  == E_ZCL_SUCCESS)
    {

        if(!psCustomData->sOTACallBackMessage.sPersistedData.bIsCoProcessorImage)
        {
            return(E_ZCL_FAIL);
        }
        sOTAHeader = sOtaGetHeader(&asCommonCoProcessorOTAHeader[psCustomData->sOTACallBackMessage.sPersistedData.u8CoProcessorOTAHeaderIndex][0]);
        sEndRequest.u32FileVersion = psCustomData->sOTACallBackMessage.sPersistedData.sAttributes.u32DownloadedFileVersion;

        sEndRequest.u16ImageType = sOTAHeader.u16ImageType;

        sEndRequest.u16ManufacturerCode = sOTAHeader.u16ManufacturerCode;
        sEndRequest.u8Status = u8Status;
        if(u8Status == OTA_STATUS_SUCCESS)
        {
            /* Decrement downloadable images */
            psCustomData->sOTACallBackMessage.sPersistedData.u8NumOfDownloadableImages--;

            /* Check Any images are pedning for downloading */
            if((!psCustomData->sOTACallBackMessage.sPersistedData.u8NumOfDownloadableImages)||(!bIsCoProcessorImgUpgdDependent))
            {
               sEndRequest.u8Status = OTA_STATUS_SUCCESS;

               /* change state to dowmload complete */
               vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_DL_COMPLETE, psEndPointDefinition, psCustomData);
            }
            else
            {
                sEndRequest.u8Status = OTA_REQUIRE_MORE_IMAGE;
            }
        }
        else
        {
            /* change state to normal */
            vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_NORMAL, psEndPointDefinition, psCustomData);
        }


    #if OTA_ACKS_ON == TRUE
        sZCL_Address.eAddressMode = E_ZCL_AM_SHORT;
    #else
        sZCL_Address.eAddressMode = E_ZCL_AM_SHORT_NO_ACK;
    #endif
        sZCL_Address.uAddress.u16DestinationAddress = psCustomData->sOTACallBackMessage.sPersistedData.u16ServerShortAddress;
        
         /* Send block end request */
        eStatus = eOTA_ClientUpgradeEndRequest(psCustomData->sReceiveEventAddress.u8DstEndpoint,
                                     psCustomData->sReceiveEventAddress.u8SrcEndpoint,
                                     &sZCL_Address,
                                     &sEndRequest);
        if(sEndRequest.u8Status == OTA_STATUS_SUCCESS)
        {  /* if it is a success case then enable retries and wait for response */
            psCustomData->sOTACallBackMessage.sPersistedData.u8Retry = 0;
            psCustomData->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime()+ OTA_TIME_INTERVAL_BETWEEN_END_REQUEST_RETRIES+1;
        }
        else if(sEndRequest.u8Status == OTA_REQUIRE_MORE_IMAGE)
        {
            /* Give a call back to the user for requesting remaining images */
            eOtaSetEventTypeAndGiveCallBack(psCustomData, E_CLD_OTA_INTERNAL_COMMAND_REQUEST_QUERY_NEXT_IMAGES,psEndPointDefinition);

            /* Change the state to Normal State */
            vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_NORMAL,psEndPointDefinition,psCustomData);
        }
        else
        {
            psCustomData->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = 0;
        }
        /* request has been sent make sure we poll for the response too otherwise might miss it
            only valid for end device*/
        eOtaSetEventTypeAndGiveCallBack(psCustomData, E_CLD_OTA_INTERNAL_COMMAND_POLL_REQUIRED,psEndPointDefinition);
    }
    return eStatus;
}
#endif
/****************************************************************************
 **
 ** NAME:       eOTA_ClientSwitchToNewImage
 **
 ** DESCRIPTION:
 ** Client switch to new downloaded upgrade image
 **
 ** PARAMETERS:                         Name                           Usage
 ** uint8                           u8SourceEndPointId            Source EP Id
 **
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_ClientSwitchToNewImage(uint8 u8SourceEndPointId)
{
    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psOTA_Common;


    if((eStatus = eOtaFindCluster(u8SourceEndPointId,
                                  &psEndPointDefinition,
                                  &psClusterInstance,
                                  &psOTA_Common,
                                  FALSE))
                                  == E_ZCL_SUCCESS)
    {
        if(!psOTA_Common->sOTACallBackMessage.sPersistedData.bIsNullImage)
        {
            uint8 au8MagicNumbers[12] = {0};
            uint32 u32Offset;
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x) || (defined APP0)
            uint8 u8NextFreeImageLocation = psOTA_Common->sOTACallBackMessage.u8NextFreeImageLocation;
#endif
            uint8 au8Data[16] = {0};

            vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_NORMAL,psEndPointDefinition,psOTA_Common);
            vOTA_SetImageValidityFlag(psOTA_Common->sOTACallBackMessage.u8NextFreeImageLocation,psOTA_Common,TRUE, psEndPointDefinition);
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
            /* Read Magic Numbers */
            u32Offset =  (psOTA_Common->sOTACallBackMessage.u8ImageStartSector[u8NextFreeImageLocation] * sNvmDefsStruct.u32SectorSize) + OTA_FLS_VALIDITY_OFFSET;

            vOtaFlashLockRead(psEndPointDefinition, psOTA_Common,u32Offset,OTA_FLS_MAGIC_NUMBER_LENGTH, au8MagicNumbers);
#else

            /* Find the offset of the block header */
            uint32 u32AbsOffset = 0;

#ifdef APP0 /* Building with selective OTA */
            if(psOTA_Common->sOTACallBackMessage.sPersistedData.bStackDownloadActive)
            {
                u32AbsOffset = OTA_APP1_SHADOW_FLASH_OFFSET ;
            }
            else
            {
                u32AbsOffset = (psOTA_Common->sOTACallBackMessage.u8ImageStartSector[u8NextFreeImageLocation]  * sNvmDefsStruct.u32SectorSize * OTA_SECTOR_CONVERTION ) ;
            }
#endif
            /* Read header marker and offset to boot block */
            vOtaFlashLockRead(psEndPointDefinition, psOTA_Common, (u32AbsOffset + OTA_BOOTLOADER_HEADER_MARKER_OFFSET), 8, au8Data);
            u32Offset = (au8Data[4] ) & 0xFF;
            u32Offset |= (au8Data[5]  & 0xFF) << 8;
            u32Offset |= (au8Data[6]  & 0xFF) << 16;
            u32Offset |= (au8Data[7] & 0xFF) << 24;

#ifdef APP0 /* Building with selective OTA */
            if ( u32Offset < u32AbsOffset )
            {
#endif
                u32Offset +=  u32AbsOffset;
                vOtaFlashLockRead(psEndPointDefinition, psOTA_Common, u32Offset, OTA_BOOTLOADER_BLOCK_LENGTH, au8MagicNumbers);
#ifdef APP0 /* Building with selective OTA */
            }
#endif
#endif
            if(bOtaIsImageValid(psOTA_Common, au8MagicNumbers)
#ifdef SOTA_ENABLED
                /* TODO add a test to check the compatibility list inside the blob */
#endif
              )
            {
                /* persisted data changed send event to the  application to save it*/
                eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_SAVE_CONTEXT,psEndPointDefinition);

                eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_RESET_TO_UPGRADE,psEndPointDefinition);

                /* Invalidate current image and validate upgrade image */
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
                /* Invalidate Internal Flash Header */
                bAHI_FlashInit(E_FL_CHIP_INTERNAL, NULL); /* Pass Internal Flash */
                /* Erase Internal Flash Header */
                DBG_vPrintf(TRACE_VERIF, "DELETE HEADER\n");
                bAHI_FullFlashProgram(0x00,16,au8Data);
#else

                
#ifdef APP0
                if(!psOTA_Common->sOTACallBackMessage.sPersistedData.bStackDownloadActive)
                {
                    u32Offset = ( psOTA_Common->sOTACallBackMessage.u8ImageStartSector[u8NextFreeImageLocation] *
                             sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION ;

                    psector_SetEscoreImageData(   u32Offset , 0 );
                    __disable_irq();
                    SYSCON->MEMORYREMAP &= ~( SYSCON_MEMORYREMAP_MAP_MASK << SYSCON_MEMORYREMAP_MAP_SHIFT);
                    PMC->CTRL &= ~(PMC_CTRL_LPMODE_MASK << PMC_CTRL_LPMODE_SHIFT);
                }
                else
                {
                    __disable_irq();
                    vOtaFlashValidatInvalidatImage(OTA_APP1_ACTIVE_FLASH_OFFSET, psOTA_Common, psEndPointDefinition);
                }
#else
                /* Indicate new image is available */
                OTA_SetNewImageFlag();
#endif
#endif

                vOtaSwitchLoads();
            }
            else
            {
                DBG_vPrintf(TRACE_VERIF, "Image header check FAILED\n");
                eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_FAILED_VALIDATING_UPGRADE_IMAGE,psEndPointDefinition);
            }
        }
    }
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eOTA_HandleImageVerification
 **
 ** DESCRIPTION:
 ** After image verification according to status provided it copies serialization data
 ** and sends the upgrade end request
 **
 ** PARAMETERS:                         Name                           Usage
 ** uint8                           u8SourceEndPointId            Source EP Id
 ** uint8                           u8DstEndpoint                 Destination EP Id
 ** teZCL_Status                    eImageVerificationStatus      image verification status
 **
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_HandleImageVerification(uint8 u8SourceEndpoint,
                                                                  uint8 u8DstEndpoint,
                                                                  teZCL_Status eImageVerificationStatus)
{
    teZCL_Status eStatus, eImageCrcCheckStatus=E_ZCL_SUCCESS;
    uint32 u32Offset,u32Id;
    bool_t bValid = FALSE;
#ifndef OTA_IGNORE_CRC_CHECK /*CRC checks Required*/

#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
    /*JN516x OR JN517x */
#ifndef LITTLE_ENDIAN_PROCESSOR  /*JN516x*/
    uint8 au8MagicNumber[ OTA_FLS_MAGIC_NUMBER_LENGTH + 4 ] = {0x12,0x34,0x56,0x78,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88, 0x08, 0x01, 0x00, 0x00};
#else /*JN517x*/
    uint8 au8MagicNumber[ OTA_FLS_MAGIC_NUMBER_LENGTH + 4 ] = {0x78,0x56,0x34,0x12,0x44,0x33,0x22,0x11,0x88,0x77,0x66,0x55,0x08, 0x01, 0x00, 0x00};
#endif
    uint8 *pu8Start       = (uint8*)&_flash_start;
    uint8 *pu8LinkKey     = (uint8*)&_FlsLinkKey;
    uint8 *pu8Cert        = (uint8*)&_FlsLinkKey + 16;
#endif
    uint32 u32NewImageTmpOffset =  0;
    uint32 u32NewImageEndOffset =  0;
    uint32 u32ReceivedCrc       =  0;
    uint32 u32CalculatedCrc     = -1;
    uint32 i                    =  0;
    uint32 u32TotalImageSize    =  0;
    uint32 u32ImageSize         =  0;

#if JENNIC_CHIP_FAMILY == JN516x
    uint32 u32IvCounter         =  0;
#endif

    uint32 u32CrcHextetCnt     =  0;
    uint16 u16OtaHeaderSize    =  0;
#endif

#ifndef OTA_NO_CERTIFICATE
    uint8 *pu8Cert        = (uint8*)&FlsZcCert;
    uint8 *pu8PrvKey      = (uint8*)&FlsPrivateKey;
#else

#ifdef COPY_PRIVATE_KEY     
    uint8 *pu8PrvKey      = (uint8*)&_FlsLinkKey + 16;
#endif    
#endif

#ifdef OTA_COPY_MAC_ADDRESS
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
    /* JN516x or JN517x */
    uint8 *pu8MacAddress = (uint8*)&_FlsMACAddress;
#endif
#endif

#ifdef OTA_MAINTAIN_CUSTOM_SERIALISATION_DATA
    uint8 *pu8CustData       = (uint8*)&FlsCustomDatabeg;
    uint8 *pu8CustDataEnd    = (uint8*)&FlsCustomDataEnd;
    uint8 u8SizeOfCustomData = pu8CustDataEnd - pu8CustData;
#endif
    uint8 au8Value[OTA_AES_BLOCK_SIZE];
    tsOTA_UpgradeEndRequestPayload sEndRequest;

#ifndef KSDK2
#if  !(defined OTA_INTERNAL_STORAGE) && ((defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x))
    uint32 u32CustomerSettings = *((uint32*)FL_INDEX_SECTOR_CUSTOMER_SETTINGS);
    bool_t bEncExternalFlash = (u32CustomerSettings & FL_INDEX_SECTOR_ENC_EXT_FLASH)?FALSE:TRUE;
#endif
#endif
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psOTA_Common;
    tsZCL_Address sZCL_Address;


    /* These arrays will be used for the Decryption of the Encrypted data on flash
       and is used in different cases. */
#ifndef OTA_UNIT_TEST_FRAMEWORK
#ifndef KSDK2
#if !(defined OTA_INTERNAL_STORAGE) && ((defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x))
    uint8 au8ivector[OTA_AES_BLOCK_SIZE], au8DataOut[OTA_AES_BLOCK_SIZE],u8Loop;
    uint32 u32TempOffset;
    tsReg128 sOtaUseKey =  eOTA_retOtaUseKey();
#endif
#endif
#endif
    uint32 u32OtaOffset = eOTA_OtaOffset();

    if((eStatus = eOtaFindCluster(u8SourceEndpoint,
                                  &psEndPointDefinition,
                                  &psClusterInstance,
                                  &psOTA_Common,
                                  FALSE))
                                  == E_ZCL_SUCCESS)
    {
        vReverseMemcpy((uint8*)&sEndRequest.u16ImageType,&psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header[12],sizeof(uint16));
        vReverseMemcpy((uint8*)&sEndRequest.u16ManufacturerCode,&psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header[10],sizeof(uint16));
        vReverseMemcpy((uint8*)&sEndRequest.u32FileVersion,&psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header[14],sizeof(uint32));

        u32Offset = u32OtaOffset;

#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x) || (defined APP0)
#ifdef APP0
       if(psOTA_Common->sOTACallBackMessage.sPersistedData.bStackDownloadActive)
       {
           u32Offset += OTA_APP1_SHADOW_FLASH_OFFSET;
       }
       else
#endif
       {
           u32Offset +=  (psOTA_Common->sOTACallBackMessage.u8ImageStartSector[psOTA_Common->sOTACallBackMessage.u8NextFreeImageLocation] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;
       }

#endif
        vOtaFlashLockRead(psEndPointDefinition, psOTA_Common, u32Offset,OTA_AES_BLOCK_SIZE, au8Value);
#ifndef OTA_UNIT_TEST_FRAMEWORK
#ifndef KSDK2
#if !(defined OTA_INTERNAL_STORAGE) && ((defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x))
        if (bEncExternalFlash)
        {   // Read IV of Old Image
            u32TempOffset = (psOTA_Common->sOTACallBackMessage.u8ImageStartSector[psOTA_Common->sOTACallBackMessage.u8NextFreeImageLocation] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;
            u32TempOffset += OTA_IV_LOCATION;
            /* Read the IV */
            vOtaFlashLockRead(psEndPointDefinition, psOTA_Common, u32TempOffset,OTA_AES_BLOCK_SIZE, au8ivector);
            au8ivector[15] = au8ivector[15] + (uint8)((u32OtaOffset - OTA_ENC_OFFSET)/OTA_AES_BLOCK_SIZE);
            //bACI_ECBencodeStripe(&sOtaUseKey,TRUE,(tsReg128 *)au8ivector,(tsReg128 *)au8DataOut);
            vOTA_EncodeString(&sOtaUseKey, au8ivector,au8DataOut);

            for(u8Loop=0;u8Loop<OTA_AES_BLOCK_SIZE;u8Loop++)
            {
                au8Value[u8Loop] = au8DataOut[u8Loop]^au8Value[u8Loop];
            }
        }
#endif
#endif
#endif
        vReverseMemcpy((uint8*)&u32Id,&au8Value[0],sizeof(uint32));

        if(u32Id != OTA_FILE_IDENTIFIER)
        {
            DBG_vPrintf(TRACE_OTA_DEBUG, "OTA FILE IDENTIFIER NOT MATCHED \n");
        }

        /* CRC check */
#ifndef OTA_IGNORE_CRC_CHECK /* CRC check is required */

#define TRACE_OTA_CRC_DEBUG 0

#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x) || (defined APP0)
#ifdef APP0
       if(psOTA_Common->sOTACallBackMessage.sPersistedData.bStackDownloadActive)
       {
           u32NewImageTmpOffset = OTA_APP1_SHADOW_FLASH_OFFSET;
       }
       else
#endif
       {
           u32NewImageTmpOffset = (psOTA_Common->sOTACallBackMessage.u8ImageStartSector[psOTA_Common->sOTACallBackMessage.u8NextFreeImageLocation] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;
       }
#endif
        /* Let's find out NewImageEndOffset = ImageSize(=u32TotalImageSize - HeaderLen + 6B) + ImageStartOffset */
        
        //vReverseMemcpy((uint8*)&u32NewImageEndOffset,&psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header[52],sizeof(uint32));
        vReverseMemcpy((uint8*)&u16OtaHeaderSize,&psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header[6],sizeof(uint16));
        vReverseMemcpy((uint8*)&u32TotalImageSize,&psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header[52],sizeof(uint32));
        u32ImageSize = u32TotalImageSize - u16OtaHeaderSize - OTA_TAG_HEADER_SIZE;
        u32NewImageEndOffset = u32ImageSize + u32NewImageTmpOffset;
        DBG_vPrintf(TRACE_OTA_CRC_DEBUG,"HeaderSize = %04x, TotalImageSize = %08x, EndOffset = %08x \n",u16OtaHeaderSize,u32TotalImageSize, u32NewImageEndOffset);

        /* If bEncExternalFlash, let's read IV from external flash */
        #if JENNIC_CHIP_FAMILY == JN516x
        #ifndef OTA_INTERNAL_STORAGE
            if (bEncExternalFlash)
            {
                u32TempOffset = (psOTA_Common->sOTACallBackMessage.u8ImageStartSector[psOTA_Common->sOTACallBackMessage.u8NextFreeImageLocation] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;
                u32TempOffset += OTA_IV_LOCATION;
                vOtaFlashLockRead(psEndPointDefinition, psOTA_Common, u32TempOffset,OTA_AES_BLOCK_SIZE, au8ivector);
                u32IvCounter = 0;
            }
        #endif
        #endif

        /* Process a Hextet(16 Bytes) at a time */
        for(u32CrcHextetCnt = 0; u32NewImageTmpOffset < u32NewImageEndOffset ; u32NewImageTmpOffset += 16, u32CrcHextetCnt++)
        {

#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
            /*JN516x OR JN517x*/
            if(0 == u32CrcHextetCnt)
            {
                /* Only  True for JN516x or JN517x
                 * Initial Hextet will be all FFs.
                 * Later, during vOTA_SetImageValidityFlag, MagicNumber will replace FFs.
                 * During this CRC calculated, It's assumed that magic number is in place. */ 
                memcpy(au8Value,au8MagicNumber,OTA_AES_BLOCK_SIZE);

            }
            else
            {
#endif
                vOtaFlashLockRead(psEndPointDefinition, psOTA_Common, u32NewImageTmpOffset,OTA_AES_BLOCK_SIZE, au8Value);
#ifdef APP0
                /*JN518x specific only */
                if(u32CrcHextetCnt < (BOOTLOADER_HEADER_LENGTH/16)) /* first 336 bytes are hashed */
                {
                     uint8 k = 0;
                     while ( k < OTA_AES_BLOCK_SIZE)
                     {
                         au8Value[k++] ^=  0xFB;
                     }
                }
#endif
                /* If bEncExternalFlash, Increment IV */
                #if JENNIC_CHIP_FAMILY == JN516x
                #ifndef OTA_INTERNAL_STORAGE
                    if (bEncExternalFlash)
                    {
                        /* Ignore the MagicNumber(0th) and IV (1st)Hextets */
                        if(u32CrcHextetCnt > 1)
                        {
                            DBG_vPrintf(TRACE_OTA_CRC_DEBUG,"_%08x : ",u32NewImageTmpOffset);
                            memcpy(&au8ivector[12], (uint8*)&u32IvCounter,sizeof(uint32));
                            u32IvCounter++;
                            vOTA_EncodeString(&sOtaUseKey, au8ivector,au8DataOut);
                            for(u8Loop=0;u8Loop<OTA_AES_BLOCK_SIZE;u8Loop++)
                            {
                                DBG_vPrintf(TRACE_OTA_CRC_DEBUG,"%02x ", au8Value[u8Loop]);
                                au8Value[u8Loop] = au8DataOut[u8Loop]^au8Value[u8Loop];
                            }
                            DBG_vPrintf(TRACE_OTA_CRC_DEBUG,"\n");
                        }
                        
                    }
                #endif
                #endif

#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)    /*end of the else block*/
            }
#endif
            /* Calculate  CRC of the Flash read hextet */
            DBG_vPrintf(TRACE_OTA_CRC_DEBUG," %08x : ",u32NewImageTmpOffset);
            for(i=0 ; i<OTA_AES_BLOCK_SIZE ; i++)
            {
                /* During CRC calculation, consider 0x00 instead of CRC in internal OTA header */
                if( (u32NewImageTmpOffset + i) >= u32Offset + 69 && (u32NewImageTmpOffset + i) <= (u32Offset + 72) )
                {
                    u32ReceivedCrc |= au8Value[i] << (8*((u32NewImageTmpOffset+i) - ( u32Offset + 69 )));
                    DBG_vPrintf(TRACE_OTA_CRC_DEBUG,"<%02x- [%08x]", au8Value[i],u32ReceivedCrc);
                    au8Value[i] = 0x00;
                }

                if( (u32NewImageTmpOffset + i + 1) > u32NewImageEndOffset)
                {
                    // Shouldn't do anything for these extra bytes
                    DBG_vPrintf(1|TRACE_OTA_CRC_DEBUG,"Caution! Unpadded Extra [%02x]\n",au8Value[i]);
                }
                else
                {
                    DBG_vPrintf(TRACE_OTA_CRC_DEBUG,"%02x ", au8Value[i]);
                    ZPS_vRunningCRC32(au8Value[i],&u32CalculatedCrc);
                }
            }
            DBG_vPrintf(TRACE_OTA_CRC_DEBUG,"\n");
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
            /* Reset watchdog as CRC calculation takes a while */
            vAHI_WatchdogRestart();
#else
            WWDT_Refresh(WWDT);
#endif
        }
        ZPS_vFinalCrc32 (&u32CalculatedCrc);


        /* Compare Crc values */
        if( u32ReceivedCrc != u32CalculatedCrc)
        {
            eImageCrcCheckStatus = E_ZCL_FAIL;
            
            DBG_vPrintf(1|TRACE_OTA_CRC_DEBUG, " u32CalculatedCrc = %08x \n",u32CalculatedCrc);
            DBG_vPrintf(1|TRACE_OTA_CRC_DEBUG, " u32ReceivedCrc   = %08x \n",u32ReceivedCrc);
        }
        else
        {
            #ifdef OTA_CLD_ATTR_IMAGE_STAMP
                eZCL_WriteLocalAttributeValue(u8SourceEndpoint,             //uint8                      u8SrcEndpoint,
                                              OTA_CLUSTER_ID,               //uint16                     u16ClusterId,
                                              FALSE,                        //bool_t                       bIsServerClusterInstance,
                                              FALSE,                        //bool_t                       bManufacturerSpecific,
                                              TRUE,                         //bool_t                     bIsClientAttribute,
                                              E_CLD_OTA_ATTR_IMAGE_STAMP,   //uint16                     u16AttributeId,
                                              &u32CalculatedCrc             //void                      *pvAttributeValue
                                              );
            #endif
        }
#endif


        if( (u32Id == OTA_FILE_IDENTIFIER) && \
            (eImageVerificationStatus == E_ZCL_SUCCESS) && \
            (eImageCrcCheckStatus == E_ZCL_SUCCESS))
        {
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
            /* copy of serializetion is only for own image */
            u32Offset = pu8Cert - pu8LinkKey;

            if(u32Offset > 0)
            {

#ifndef OTA_UNIT_TEST_FRAMEWORK
                uint8 au8TempBuffer[50]; /* for copying data from active image */

                /* Check if the device is Encrypted.  If yes the device is encrypted, we have to take care of IV vector as well
                 * If the IV vector of the Client and the Upgrade Image is different we cannot just copy the Serialization Data
                 * Directly. We need to Decrypt existing data and encrypt it again with the new IV vector values.
                 */

                if (bEncExternalFlash)
                {

                    DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 200 ..\n");
#ifdef OTA_COPY_MAC_ADDRESS
                    /* Copy Mac Address from from Internal flash and encrypt it. Then copy to external flash */
                    memcpy(au8TempBuffer, pu8MacAddress, 8);

                    /* Read IV vector from external flash */
                    u32TempOffset = (psOTA_Common->sOTACallBackMessage.u8ImageStartSector[psOTA_Common->sOTACallBackMessage.u8NextFreeImageLocation] * sNvmDefsStruct.u32SectorSize);
                    u32TempOffset += OTA_IV_LOCATION;
                    vOtaFlashLockRead(psEndPointDefinition, psOTA_Common, u32TempOffset,OTA_AES_BLOCK_SIZE, au8ivector);

                    au8ivector[15] = au8ivector[15]+((OTA_6X_MAC_OFFSET - OTA_ENC_OFFSET)/OTA_AES_BLOCK_SIZE);
                    bACI_ECBencodeStripe(&sOtaUseKey,TRUE,(tsReg128 *)au8ivector,(tsReg128 *)au8DataOut);

                    for(u8Loop=0;u8Loop<16;u8Loop++)
                              au8TempBuffer[u8Loop] = (au8TempBuffer[u8Loop] ^ au8DataOut[u8Loop]) ;

                    u32Offset = OTA_6X_MAC_OFFSET;
                    u32Offset += (psOTA_Common->sOTACallBackMessage.u8ImageStartSector[psOTA_Common->sOTACallBackMessage.u8NextFreeImageLocation] * sNvmDefsStruct.u32SectorSize);
                    DBG_vPrintf(TRACE_VERIF, "Copy MAC TO %08x\n", u32Offset);
                    vOtaFlashLockWrite(psEndPointDefinition, psOTA_Common,u32Offset,16, au8TempBuffer);
#endif
                    /*
                     * We read the IV data from the image presently in execution in the Device
                     */
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x) || (defined APP0)
#ifdef APP0
                    if(psOTA_Common->sOTACallBackMessage.sPersistedData.bStackDownloadActive)
                    {
                        u32TempOffset = OTA_APP1_SHADOW_FLASH_OFFSET;
                    }
                    else
#endif
                    {
                        u32TempOffset = (psOTA_Common->sOTACallBackMessage.u8ImageStartSector[psOTA_Common->sOTACallBackMessage.u8NextFreeImageLocation] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;
                    }

#else
                    u32TempOffset = 0;
#endif

                    u32TempOffset += OTA_IV_LOCATION;
                    vOtaFlashLockRead(psEndPointDefinition, psOTA_Common, u32TempOffset,OTA_AES_BLOCK_SIZE, au8ivector);

                    au8ivector[15] = au8ivector[15]+((u32OtaOffset - OTA_ENC_OFFSET)/OTA_AES_BLOCK_SIZE) + (80 / OTA_AES_BLOCK_SIZE);
                    //bACI_ECBencodeStripe(&sOtaUseKey,TRUE,(tsReg128 *)au8ivector,(tsReg128 *)au8DataOut);
                    vOTA_EncodeString(&sOtaUseKey,au8ivector,au8DataOut);

                    // For Copying Link Key
                    memcpy(au8TempBuffer, pu8LinkKey, 16);

                    for(u8Loop=0;u8Loop<16;u8Loop++)
                              au8TempBuffer[u8Loop] = (au8TempBuffer[u8Loop] ^ au8DataOut[u8Loop]);

                    u32Offset = pu8LinkKey - pu8Start;
#ifdef APP0
                    if(psOTA_Common->sOTACallBackMessage.sPersistedData.bStackDownloadActive)
                    {
                        u32Offset += OTA_APP1_SHADOW_FLASH_OFFSET;
                    }
                    else
#endif
                    {
                        u32Offset += (psOTA_Common->sOTACallBackMessage.u8ImageStartSector[psOTA_Common->sOTACallBackMessage.u8NextFreeImageLocation] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;
                    }

                    DBG_vPrintf(TRACE_VERIF, "Copy key to %08x\n", u32Offset);
                    vOtaFlashLockWrite(psEndPointDefinition, psOTA_Common,u32Offset,16, au8TempBuffer);

                    // For Copying Certificate

                    memcpy(au8TempBuffer, pu8Cert, 48);
                    for(u8Loop=0;u8Loop<48;u8Loop++)
                    {
                        if((u8Loop % OTA_AES_BLOCK_SIZE) == 0 )
                        {
                            au8ivector[15]++;
                            //bACI_ECBencodeStripe(&sOtaUseKey,TRUE,(tsReg128 *)au8ivector,(tsReg128 *)au8DataOut);
                            vOTA_EncodeString(&sOtaUseKey,au8ivector,au8DataOut);
                        }

                        au8TempBuffer[u8Loop] = au8TempBuffer[u8Loop] ^ au8DataOut[(u8Loop %16)] ;
                    }
                    u32Offset = pu8Cert - pu8Start;
#ifdef APP0
                    if(psOTA_Common->sOTACallBackMessage.sPersistedData.bStackDownloadActive)
                    {
                        u32Offset += OTA_APP1_SHADOW_FLASH_OFFSET;
                    }
                    else
#endif
                    {
                        u32Offset += (psOTA_Common->sOTACallBackMessage.u8ImageStartSector[psOTA_Common->sOTACallBackMessage.u8NextFreeImageLocation] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;
                    }


                    vOtaFlashLockWrite(psEndPointDefinition, psOTA_Common,u32Offset,48, au8TempBuffer);

                    // For Copying Private Key
#ifdef COPY_PRIVATE_KEY                     

                    memcpy(au8TempBuffer, pu8PrvKey, 21);
                    for(u8Loop=0;u8Loop<21;u8Loop++)
                    {
                        if((u8Loop % OTA_AES_BLOCK_SIZE) == 0 )
                        {
                            au8ivector[15]++;
                            bACI_ECBencodeStripe(&sOtaUseKey,TRUE,(tsReg128 *)au8ivector,(tsReg128 *)au8DataOut);
                        }

                        au8TempBuffer[u8Loop] = (au8TempBuffer[u8Loop] ^ au8DataOut[(u8Loop %16)]) ;
                    }
                    u32Offset = pu8PrvKey - pu8Start;
                    u32Offset += (psOTA_Common->sOTACallBackMessage.u8ImageStartSector[psOTA_Common->sOTACallBackMessage.u8NextFreeImageLocation] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;
                    vOtaFlashLockWrite(psEndPointDefinition, psOTA_Common,u32Offset,21, au8TempBuffer);
#endif                    
                }
                else
                {
                    DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 31 ..\n");

#ifdef OTA_COPY_MAC_ADDRESS
#if !(defined OTA_INTERNAL_STORAGE) && ((defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x))
#ifndef KSDK2
                    memcpy(au8TempBuffer, pu8MacAddress, 8);
                    u32Offset = pu8MacAddress - pu8Start;
                    u32Offset += (psOTA_Common->sOTACallBackMessage.u8ImageStartSector[psOTA_Common->sOTACallBackMessage.u8NextFreeImageLocation] * sNvmDefsStruct.u32SectorSize);
                    DBG_vPrintf(TRACE_VERIF, "Copy MAC2 TO %08x\n", u32Offset);
                    vOtaFlashLockWrite(psEndPointDefinition, psOTA_Common,u32Offset,8, au8TempBuffer);
#endif
#else
                    memset(au8TempBuffer, 0, 16);
                    memcpy(au8TempBuffer, pu8MacAddress, 8);
                    u32Offset = pu8MacAddress - pu8Start;
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x) || (defined APP0)
                    u32Offset += (psOTA_Common->sOTACallBackMessage.u8ImageStartSector[psOTA_Common->sOTACallBackMessage.u8NextFreeImageLocation] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;
#endif
                    DBG_vPrintf(TRACE_VERIF, "Copy MAC2 TO %08x 16 bytes\n", u32Offset);
                    vOtaFlashLockWrite(psEndPointDefinition, psOTA_Common, u32Offset, 16, au8TempBuffer);
#endif
#endif
                    // copy Link Key
                    /* read link key from active image */

                    memcpy(au8TempBuffer, pu8LinkKey, 16);
                    /* copy link key to new upgrade image */
                    u32Offset = pu8LinkKey - pu8Start;
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x) || (defined APP0)
#ifdef APP0
                    if(psOTA_Common->sOTACallBackMessage.sPersistedData.bStackDownloadActive)
                    {
                        u32Offset += OTA_APP1_SHADOW_FLASH_OFFSET;
                    }
                    else
#endif
                    {
                         u32Offset += (psOTA_Common->sOTACallBackMessage.u8ImageStartSector[psOTA_Common->sOTACallBackMessage.u8NextFreeImageLocation] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;
                    }

#endif
                    DBG_vPrintf(TRACE_VERIF, "Copy Key2 to %08x\n", u32Offset);
                    vOtaFlashLockWrite(psEndPointDefinition, psOTA_Common,u32Offset,16, au8TempBuffer);
#ifndef OTA_NO_CERTIFICATE
                    // copy certs Key
                    /* read certificates from active image */

                    memcpy(au8TempBuffer, pu8Cert, 48);
                    /* copy certificate to new upgrade image */
                    u32Offset = pu8Cert - pu8Start;
                    u32Offset += (psOTA_Common->sOTACallBackMessage.u8ImageStartSector[psOTA_Common->sOTACallBackMessage.u8NextFreeImageLocation] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;
                    DBG_vPrintf(TRACE_VERIF, "READ4 -> 48 from %08x\n", u32Offset);
                    vOtaFlashLockWrite(psEndPointDefinition, psOTA_Common,u32Offset,48, au8TempBuffer);

                    // copy private Key
                    /* read private key from active image */

                    memcpy(au8TempBuffer, pu8PrvKey, 21);
                    /* copy private key to new upgrade image */
                    u32Offset = pu8PrvKey - pu8Start;
                    u32Offset += (psOTA_Common->sOTACallBackMessage.u8ImageStartSector[psOTA_Common->sOTACallBackMessage.u8NextFreeImageLocation] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;
                    DBG_vPrintf(TRACE_VERIF, "VERIF1 -> write 21 bytes to %08x\n", u32Offset);
                    vOtaFlashLockWrite(psEndPointDefinition, psOTA_Common,u32Offset,21, au8TempBuffer);
#endif
                }
#endif


#ifdef OTA_MAINTAIN_CUSTOM_SERIALISATION_DATA
                if(u8SizeOfCustomData > 0)
                {
                    uint8 u8NoOf16BytesBlocks = u8SizeOfCustomData/16;

                    uint8 u8LoopCount,u8IntLoopCount;
                    uint32 u32ReadOffset = pu8CustData - pu8Start;
                    uint32 u32WriteOffset = pu8CustData - pu8Start;
                    // Check if Device is Encrypted
                    if (bEncExternalFlash)
                    {

                        u32TempOffset = (psOTA_Common->sOTACallBackMessage.u8ImageStartSector[psOTA_Common->sOTACallBackMessage.u8NextFreeImageLocation] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;
                        u32TempOffset += OTA_IV_LOCATION;
                        vOtaFlashLockRead(psEndPointDefinition, psOTA_Common, u32TempOffset,OTA_AES_BLOCK_SIZE, au8ivector);

                        au8ivector[15] = au8ivector[15]+((u32OtaOffset - OTA_ENC_OFFSET)/OTA_AES_BLOCK_SIZE) + (80 / OTA_AES_BLOCK_SIZE) + ( (pu8CustData - pu8LinkKey) / OTA_AES_BLOCK_SIZE);

                        bACI_ECBencodeStripe(&sOtaUseKey,TRUE,(tsReg128 *)au8ivector,(tsReg128 *)au8DataOut);
                        u32WriteOffset += (psOTA_Common->sOTACallBackMessage.u8ImageStartSector[psOTA_Common->sOTACallBackMessage.u8NextFreeImageLocation] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;

                        for(u8LoopCount = 0; u8LoopCount < u8NoOf16BytesBlocks; u8LoopCount++)
                        {

                            memcpy(au8TempBuffer, (pu8CustData+(16*u8LoopCount)), 16);

                            for(u8IntLoopCount=0; u8IntLoopCount < 16;u8IntLoopCount++)
                            {
                                 au8TempBuffer[u8IntLoopCount] = (au8TempBuffer[u8IntLoopCount] ^ au8DataOut[(u8IntLoopCount)]);
                            }

                            vOtaFlashLockWrite(psEndPointDefinition, psOTA_Common, (u32WriteOffset + (16*u8LoopCount)), 16, au8TempBuffer);

                            au8ivector[15]++;
                            bACI_ECBencodeStripe(&sOtaUseKey,TRUE,(tsReg128 *)au8ivector,(tsReg128 *)au8DataOut);
                        }

                        /* copy remaining data */
                        u8NoOf16BytesBlocks = u8SizeOfCustomData%16;
                        memcpy(au8TempBuffer, (pu8CustData+(16*u8LoopCount)), u8NoOf16BytesBlocks);
                        for(u8LoopCount = 0; u8LoopCount < u8NoOf16BytesBlocks; u8LoopCount++)
                        {
                            au8TempBuffer[u8LoopCount] = (au8TempBuffer[u8LoopCount] ^ au8DataOut[(u8LoopCount)]);
                        }

                        vOtaFlashLockWrite(psEndPointDefinition, psOTA_Common, (u32WriteOffset + (16*u8LoopCount)), u8NoOf16BytesBlocks, au8TempBuffer);
                    }
                    else
                    {
                        u32WriteOffset += (psOTA_Common->sOTACallBackMessage.u8ImageStartSector[psOTA_Common->sOTACallBackMessage.u8NextFreeImageLocation] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;

                        for(u8LoopCount = 0; u8LoopCount < u8NoOf16BytesBlocks; u8LoopCount++)
                        {

                            memcpy(au8TempBuffer, (pu8CustData+(16*u8LoopCount)), 16);
                            vOtaFlashLockWrite(psEndPointDefinition, psOTA_Common, (u32WriteOffset + (16*u8LoopCount)), 16, au8TempBuffer);

                        }

                        /* copy remaining data */
                        u8NoOf16BytesBlocks = u8SizeOfCustomData%16;
                        memcpy(au8TempBuffer, (pu8CustData+(16*u8LoopCount)), 16);
                        vOtaFlashLockWrite(psEndPointDefinition, psOTA_Common, (u32WriteOffset + (16*u8LoopCount)), u8NoOf16BytesBlocks, au8TempBuffer);
                    }
                }
#endif
            }
#endif /* Only valid for JN516x OR JN517x */
            //vOTA_SetImageValidityFlag(psOTA_Common->sOTACallBackMessage.u8CurrentActiveImageLocation,psOTA_Common,FALSE, psEndPointDefinition);
            bValid = bOtaIsSerializationDataValid( psEndPointDefinition, psOTA_Common,
                                                   psOTA_Common->sOTACallBackMessage.u8NextFreeImageLocation);

            /* Check Status, if it is not true, there is no upgrade from new image */
            if(bValid)
            {
#if (OTA_MAX_CO_PROCESSOR_IMAGES != 0)
                /* Decrement downloadable images */
                psOTA_Common->sOTACallBackMessage.sPersistedData.u8NumOfDownloadableImages--;
                if((!psOTA_Common->sOTACallBackMessage.sPersistedData.u8NumOfDownloadableImages)||(!bIsCoProcessorImgUpgdDependent))
                {
                    sEndRequest.u8Status = OTA_STATUS_SUCCESS;
                }
                else
                {
                    sEndRequest.u8Status = OTA_REQUIRE_MORE_IMAGE;
                }
#else
                sEndRequest.u8Status = OTA_STATUS_SUCCESS;
#endif
                DBG_vPrintf(TRACE_VERIF, "IMAGE IS VALID\n");
            }
            else
            {
                /* Set Status as Invalid Image */
                /* even though signature verification is passed but serialization copy is failed */
                sEndRequest.u8Status = OTA_STATUS_IMAGE_INVALID;

                /* Give a callback to the user, serialization copying failed */
                eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_FAILED_COPYING_SERIALIZATION_DATA,psEndPointDefinition);

                vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_NORMAL,psEndPointDefinition,psOTA_Common);
                DBG_vPrintf(TRACE_VERIF, "IMAGE IS NOT VALID\n");
                /* Give a call back to the user */
                eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_OTA_DL_ABORTED,psEndPointDefinition);
            }
        }
        else
        {
            sEndRequest.u8Status = OTA_STATUS_IMAGE_INVALID;
            vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_NORMAL,psEndPointDefinition,psOTA_Common);
            //vOTA_SetImageValidityFlag(psOTA_Common->sOTACallBackMessage.u8NextFreeImageLocation,psOTA_Common,FALSE, psEndPointDefinition);

            /* Give a call back to the user */
            eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_OTA_DL_ABORTED,psEndPointDefinition);
        }
        #if OTA_ACKS_ON == TRUE
                sZCL_Address.eAddressMode = E_ZCL_AM_SHORT;
        #else
                sZCL_Address.eAddressMode = E_ZCL_AM_SHORT_NO_ACK;
        #endif
        sZCL_Address.uAddress.u16DestinationAddress = psOTA_Common->sOTACallBackMessage.sPersistedData.u16ServerShortAddress;

        eOTA_ClientUpgradeEndRequest(u8SourceEndpoint,
                                     u8DstEndpoint,
                                     &sZCL_Address,
                                     &sEndRequest);
        if(sEndRequest.u8Status == OTA_STATUS_SUCCESS)
        {  /* if it is a success case then enable retries and wait for response */
            psOTA_Common->sOTACallBackMessage.sPersistedData.u8Retry = 0;
            psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime()+ OTA_TIME_INTERVAL_BETWEEN_END_REQUEST_RETRIES+1;
            vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_DL_COMPLETE,psEndPointDefinition,psOTA_Common);
        }
#if (OTA_MAX_CO_PROCESSOR_IMAGES != 0)
        else if(sEndRequest.u8Status == OTA_REQUIRE_MORE_IMAGE)
        {
            /* Give a call back to the user for requesting remaining images */
            eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_REQUEST_QUERY_NEXT_IMAGES,psEndPointDefinition);

            /* Change the state to Normal State */
            vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_NORMAL,psEndPointDefinition,psOTA_Common);
        }
#endif
        else
        {
             psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = 0;
        }
        /* request has been sent make sure we poll for the response too otherwise might miss it
           only valid for end device*/
        eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_POLL_REQUIRED,psEndPointDefinition);
    }
    else
    {
        return E_ZCL_FAIL;
    }
    return E_ZCL_SUCCESS;
}
#endif
/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
