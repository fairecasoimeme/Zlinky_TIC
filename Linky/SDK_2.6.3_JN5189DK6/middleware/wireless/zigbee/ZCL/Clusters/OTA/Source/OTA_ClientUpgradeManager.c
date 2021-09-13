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
 * COMPONENT:          OTA_ClientUpgradeManager.c
 *
 * DESCRIPTION:        Over The Air Upgrade
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include <string.h>
#include "zcl_options.h"
#include "zcl.h"
#include "OTA.h"
#include "OTA_private.h"
#include "rnd_pub.h"
#include "dbg.h"
 #ifndef OTA_NO_CERTIFICATE
#include "eccapi.h"
#include "sbmce_cb.h"
#include "dbg.h"
#endif
#if (defined KSDK2)
#include "OtaSupport.h"
#endif
#include "aessw_ccm.h"
#ifdef SOTA_ENABLED
#include "blob_utils.h"
#endif
#ifndef TRACE_OTA_DEBUG
#define TRACE_OTA_DEBUG FALSE
#endif
#ifndef TRACE_INT_DECRYPT
#define TRACE_INT_DECRYPT FALSE
#endif
#ifndef TRACE_INT_FLASH
#define TRACE_INT_FLASH FALSE
#endif
#ifndef TRACE_BLOCKS
#define TRACE_BLOCKS FALSE
#endif
#ifndef TRACE_VERIF
#define TRACE_VERIF FALSE
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
#ifdef INTERNAL_ENCRYPTED
#ifndef OTA_USE_KEY_IN_PSECTOR
uint32 g_au32OtaEncCred [4];
#endif
#endif

#ifdef OTA_CLIENT
PRIVATE void vOtaUpgManClientStateIdle(
                                 void                        *pvStruct,
                                 tsZCL_EndPointDefinition    *psEndPointDefinition);
PRIVATE void vOtaUpgManClientStateDownloadInProgress(
                                 void                        *pvStruct,
                                 tsZCL_EndPointDefinition    *psEndPointDefinition);
PRIVATE void vOtaUpgManClientStateDownloadComplete(
                                 void                        *pvStruct,
                                 tsZCL_EndPointDefinition    *psEndPointDefinition);
PRIVATE void vOtaUpgManClientStateWaitForRunCommand(
                                 void                        *pvStruct,
                                 tsZCL_EndPointDefinition    *psEndPointDefinition);
PRIVATE void vOtaUpgManClientStateCountDown(
                                 void                        *pvStruct,
                                 tsZCL_EndPointDefinition    *psEndPointDefinition);
PRIVATE void vOtaAbortDownload(
                                 tsOTA_Common                *psOTA_Common,
                                 tsZCL_EndPointDefinition    *psEndPointDefinition);
PRIVATE void vOtaHandleTimedBlockRequest(
                                 tsOTA_Common                *psOTA_Common,
                                 bool_t                      *pbPollRequired,
                                 tsOTA_ImageHeader           *psOTACurrentHeader,
                                 tsZCL_EndPointDefinition    *psEndPointDefinition);
PRIVATE void vOtaHandleEndRequestRetries(
                                 tsOTA_Common                *psOTA_Common,
								 bool_t                        *pbPollRequired,
                                 tsOTA_ImageHeader           *psOTACurrentHeader,
                                 tsZCL_EndPointDefinition    *psEndPointDefinition);
PRIVATE void vOtaRequestNextBlock(
                                 uint8                        u8SrcEndpoint,
                                 uint8                        u8DstEndpoint,
                                 tsOTA_BlockRequest          *psBlock,
                                 tsZCL_EndPointDefinition    *psEndPointDefinition,
                                 tsOTA_Common                *psOTA_Common,
                                 tsZCL_Address               *psZCL_Address);
PRIVATE void vOtaHandleWaitForDataStatus(
                                 tsZCL_EndPointDefinition    *psEndPointDefinition,
                                 tsOTA_Common                *psOTA_Common,
                                 tsOTA_ImageBlockResponsePayload *psBlockResponse);
#ifdef INTERNAL_ENCRYPTED
PRIVATE void vOtaProcessInternalEncryption(
                                 uint8                       *pau8Buffer,
                                 uint32                      u32StartAddress,
                                 uint32                      u32DataSize,
                                 uint32                       u32ImageStartAddress
        );
#endif


PRIVATE bool_t bOtaHandleImageNotify( tsOTA_Common *psOTA_Common,
                                    tsZCL_Address *psZCL_Address,
                                    uint8        u8SrcEndpoint,
                                    uint8       u8DstEndpoint);

PRIVATE bool_t bOtaHandleImageQueryResponse( tsOTA_Common *psOTA_Common,
                                             tsZCL_EndPointDefinition    *psEndPointDefinition,
                                    tsZCL_Address *psZCL_Address,
                                    uint8        u8SrcEndpoint,
                                    uint8       u8DstEndpoint);

PRIVATE bool_t bOtaHandleBlockResponseIdle( tsOTA_Common *psOTA_Common,
                                             tsZCL_EndPointDefinition    *psEndPointDefinition,
                                    tsZCL_Address *psZCL_Address,
                                    uint8        u8SrcEndpoint,
                                    uint8       u8DstEndpoint);

PRIVATE bool_t bOtaHandleBlockResponseInProgress( tsOTA_Common *psOTA_Common,
                                             tsZCL_EndPointDefinition    *psEndPointDefinition,
                                    tsZCL_Address *psZCL_Address,
                                    tsOTA_ImageHeader * psOTACurrentHeader);

PRIVATE bool_t bOtaHandleBlockResponseStandardImage( tsOTA_Common *psOTA_Common,
                                             tsZCL_EndPointDefinition    *psEndPointDefinition,
                                             tsOTA_ImageBlockResponsePayload * psResponse,
                                             tsOTA_ImageHeader           *psOTACurrentHeader,
                                    tsZCL_Address *psZCL_Address);

PRIVATE bool_t bOtaHandleBlockResponseCoProcessorImage( tsOTA_Common *psOTA_Common,
                                             tsZCL_EndPointDefinition    *psEndPointDefinition,
                                             tsOTA_ImageBlockResponsePayload * psResponse,
                                             tsOTA_ImageHeader           *psOTACurrentHeader,
                                    tsZCL_Address *psZCL_Address);

PRIVATE bool_t bOtaHandleBlockResponseFileSpecificImage( tsOTA_Common *psOTA_Common,
                                             tsZCL_EndPointDefinition    *psEndPointDefinition,
                                             tsOTA_ImageBlockResponsePayload * psResponse,
                                             tsOTA_ImageHeader           *psOTACurrentHeader,
                                    tsZCL_Address *psZCL_Address);

PRIVATE bool_t bOtaHandleQuerySpecificFileResponse( tsOTA_Common *psOTA_Common,
                                             tsZCL_EndPointDefinition    *psEndPointDefinition,
                                    tsZCL_Address *psZCL_Address,
                                    uint8        u8SrcEndpoint,
                                    uint8       u8DstEndpoint);



#ifdef OTA_STRING_COMPARE
PRIVATE bool_t bOtaVerifyString( tsOTA_Common *psOTA_Common,
                                             tsZCL_EndPointDefinition    *psEndPointDefinition,
                                             uint8 * pu8ExpectedStr);
#endif

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
extern uint32 _enc_start;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 **
 ** NAME:       vOtaInitStateMachine
 **
 ** DESCRIPTION:
 ** Initialise OTA client state machine
 ** PARAMETERS:               Name                           Usage
 ** tsOTA_Common           *psCustomData                  OTA custom data
 **
 ** RETURN:
 ** None
 ****************************************************************************/
PUBLIC  void vOtaInitStateMachine(tsOTA_Common *psCustomData)
{
    psCustomData->sOTACallBackMessage.sPersistedData.u32FunctionPointer =
                (uint32)vOtaUpgManClientStateIdle;
#ifdef OTA_PAGE_REQUEST_SUPPORT
    psCustomData->sOTACallBackMessage.sPersistedData.sPageReqParams.bPageReqOn = TRUE;
#endif
#if (defined KSDK2)
#ifdef APP0
           g_u16OtaPageIndex = 0;
#else
    OTA_AlignOnReset();
#endif
#endif
}
/****************************************************************************
 **
 ** NAME:       vOtaClientUpgMgrMapStates
 **
 ** DESCRIPTION:
 ** Updates OTA client state machine
 ** PARAMETERS:                           Name                           Usage
 ** teOTA_ImageUpgradeStatus            eStatus                        status
 ** tsZCL_EndPointDefinition           *psEndPointDefinition           Endpoint definition
 ** tsOTA_Common                        *psCustomData                  OTA custom data
 **
 ** RETURN:
 ** None
 ****************************************************************************/
PUBLIC  void vOtaClientUpgMgrMapStates(
                                       teOTA_ImageUpgradeStatus  eStatus,
                                       tsZCL_EndPointDefinition *psEndPointDefinition,
                                       tsOTA_Common             *psCustomData)
{
    switch(eStatus)
    {
        case E_CLD_OTA_STATUS_RESET:
            psCustomData->sOTACallBackMessage.sPersistedData.u32FunctionPointer =
                            (uint32)vOtaUpgManClientStateIdle;
#if (defined KSDK2)
#ifdef APP0
            g_u16OtaPageIndex = 0;
#endif
#endif
            break;
        case E_CLD_OTA_STATUS_NORMAL:
            psCustomData->sOTACallBackMessage.sPersistedData.u32FunctionPointer =
                (uint32)vOtaUpgManClientStateIdle;
            psCustomData->sOTACallBackMessage.sPersistedData.u8Retry = 0;
            psCustomData->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = 0;
            psCustomData->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset = 0;
            psCustomData->sOTACallBackMessage.sPersistedData.u32Step = 0;
            psCustomData->sOTACallBackMessage.sPersistedData.bIsSpecificFile = FALSE;
            psCustomData->sOTACallBackMessage.sPersistedData.bIsNullImage = FALSE;
#ifdef OTA_CLD_ATTR_REQUEST_DELAY
            psCustomData->sOTACallBackMessage.sPersistedData.sAttributes.u16MinBlockRequestDelay = OTA_BLOCK_REQUEST_DELAY_DEF_VALUE;
            eZCL_UpdateMsTimer(psEndPointDefinition, FALSE,0);
#endif
#ifdef OTA_PAGE_REQUEST_SUPPORT
           psCustomData->sOTACallBackMessage.sPersistedData.sPageReqParams.bPageReqOn = TRUE;
           psCustomData->sOTACallBackMessage.sPersistedData.sPageReqParams.u8PageReqRetry = 0;
#endif

#if (defined KSDK2)
#ifdef APP0
           g_u16OtaPageIndex = 0;
#else
           OTA_CancelImage();
#endif
#endif
        break;
        case E_CLD_OTA_STATUS_DL_IN_PROGRESS:
            psCustomData->sOTACallBackMessage.sPersistedData.u32FunctionPointer =
                 (uint32)vOtaUpgManClientStateDownloadInProgress;
        break;
        case E_CLD_OTA_STATUS_DL_COMPLETE:
#ifdef OTA_CLD_ATTR_REQUEST_DELAY
            psCustomData->sOTACallBackMessage.sPersistedData.sAttributes.u16MinBlockRequestDelay = OTA_BLOCK_REQUEST_DELAY_DEF_VALUE;
            eZCL_UpdateMsTimer(psEndPointDefinition, FALSE,0);
#endif
            psCustomData->sOTACallBackMessage.sPersistedData.u32FunctionPointer =
                 (uint32)vOtaUpgManClientStateDownloadComplete;

        break;
        case E_CLD_OTA_STATUS_WAIT_TO_UPGRADE:
            psCustomData->sOTACallBackMessage.sPersistedData.u32FunctionPointer =
                 (uint32)vOtaUpgManClientStateWaitForRunCommand;
        break;
        case E_CLD_OTA_STATUS_COUNT_DOWN:
            psCustomData->sOTACallBackMessage.sPersistedData.u32FunctionPointer =
                 (uint32)vOtaUpgManClientStateCountDown;
        break;

        default:
            psCustomData->sOTACallBackMessage.sPersistedData.u32FunctionPointer =
                 (uint32)vOtaUpgManClientStateIdle;
#if (defined KSDK2)
#ifdef APP0
           g_u16OtaPageIndex = 0;
#else
           OTA_CancelImage();
#endif
#endif
        break;
    }

    psCustomData->sOTACallBackMessage.sPersistedData.sAttributes.u8ImageUpgradeStatus = eStatus;
    eOTA_RestoreClientData(
           psEndPointDefinition->u8EndPointNumber,
           &psCustomData->sOTACallBackMessage.sPersistedData,
           FALSE);

}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/
/****************************************************************************
 **
 ** NAME:       vOtaUpgManClientStateIdle
 **
 ** DESCRIPTION:
 ** OTA client Idle state
 ** PARAMETERS:                           Name                           Usage
 ** void                               *pvStruct                        OTA custom data
 ** tsZCL_EndPointDefinition           *psEndPointDefinition           Endpoint definition
 ** RETURN:
 ** None
 ****************************************************************************/
PRIVATE  void vOtaUpgManClientStateIdle(
                                    void                        *pvStruct,
                                    tsZCL_EndPointDefinition    *psEndPointDefinition)
{
    tsZCL_Address sZCL_Address;
    uint8 u8DstEndpoint,u8SrcEndpoint;


    bool_t bPollRequired = FALSE;
    tsOTA_Common *psOTA_Common = (tsOTA_Common *)pvStruct;
    tsOTA_ImageHeader sOTAHeader;
#ifdef APP0 /* Building with selective OTA */
    if(psOTA_Common->sOTACallBackMessage.sPersistedData.bStackDownloadActive)
    {
        sOTAHeader = sOtaGetHeader(APP_pu8GetApp1OtaHeader());
    }
    else
#endif
#ifdef SOTA_ENABLED
    if (!sOtaInitBlobOtaHeader(psOTA_Common->sOTACallBackMessage.sPersistedData.blobId, &sOTAHeader))
#endif
    {
        sOTAHeader = sOtaGetHeader(au8OtaHeader);
    }

    u8DstEndpoint = psOTA_Common->sReceiveEventAddress.u8SrcEndpoint;
    psOTA_Common->sOTACallBackMessage.sPersistedData.u8DstEndpoint = u8DstEndpoint;
    u8SrcEndpoint = psOTA_Common->sReceiveEventAddress.u8DstEndpoint;

    #if OTA_ACKS_ON == TRUE
        sZCL_Address.eAddressMode = E_ZCL_AM_SHORT;
    #else
        sZCL_Address.eAddressMode = E_ZCL_AM_SHORT_NO_ACK;
    #endif
    sZCL_Address.uAddress.u16DestinationAddress = psOTA_Common->sOTACallBackMessage.sPersistedData.u16ServerShortAddress;

    /* Only these commands can cause a state change everything else is ignored and
       we stay in the current state */
    switch(psOTA_Common->sOTACallBackMessage.eEventId)
    {
        case E_CLD_OTA_INTERNAL_COMMAND_TIMER_EXPIRED:
        {
            vOtaHandleTimedBlockRequest(psOTA_Common, &bPollRequired,&sOTAHeader,psEndPointDefinition);
            DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 1 ..\n");
        }
        break;
        case E_CLD_OTA_COMMAND_IMAGE_NOTIFY:
        {
            bPollRequired = bOtaHandleImageNotify(psOTA_Common, &sZCL_Address, u8SrcEndpoint, u8DstEndpoint);

        }
        break;
        case E_CLD_OTA_COMMAND_QUERY_NEXT_IMAGE_RESPONSE:
        {
            bPollRequired = bOtaHandleImageQueryResponse( psOTA_Common,
                                                       psEndPointDefinition,
                                                       &sZCL_Address,
                                                       u8SrcEndpoint,
                                                       u8DstEndpoint);


        }
        break;

        case E_CLD_OTA_COMMAND_BLOCK_RESPONSE:
        {
            bPollRequired = bOtaHandleBlockResponseIdle(psOTA_Common,
                                                       psEndPointDefinition,
                                                       &sZCL_Address,
                                                       u8SrcEndpoint,
                                                       u8DstEndpoint);
        }     /* end of case Block response */
        break;


        case E_CLD_OTA_COMMAND_QUERY_SPECIFIC_FILE_RESPONSE:
        {
            bPollRequired = bOtaHandleQuerySpecificFileResponse(psOTA_Common,
                                                       psEndPointDefinition,
                                                       &sZCL_Address,
                                                       u8SrcEndpoint,
                                                       u8DstEndpoint);
        }
        break;

        case E_CLD_OTA_INTERNAL_COMMAND_RCVD_DEFAULT_RESPONSE:
            /* Move to normal state */
            vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_NORMAL, psEndPointDefinition, psOTA_Common);

             /* Give a call back to the user */
            eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_OTA_DL_ABORTED,psEndPointDefinition);
            break;
        default:
            /* To stop timer events sent to state machine from previous states. */
            psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = 0;
            break;

    }


    if (bPollRequired)
    {

        /* request has been sent make sure we poll for the response too otherwise might miss it
           only valid for end device*/
        eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_POLL_REQUIRED,psEndPointDefinition);
    }
}

/****************************************************************************
 **
 ** NAME:       vOtaUpgManClientStateDownloadInProgress
 **
 ** DESCRIPTION:
 ** OTA client download in progress state
 ** PARAMETERS:                           Name                           Usage
 ** void                               *pvStruct                        OTA custom data
 ** tsZCL_EndPointDefinition           *psEndPointDefinition           Endpoint definition
 ** RETURN:
 ** None
 ****************************************************************************/

PRIVATE  void vOtaUpgManClientStateDownloadInProgress(
                                           void                        *pvStruct ,
                                           tsZCL_EndPointDefinition    *psEndPointDefinition)
{

    bool_t bPollRequired = FALSE;
    tsZCL_Address sZCL_Address;
    tsOTA_Common *psOTA_Common = (tsOTA_Common *)pvStruct;
    tsOTA_ImageHeader sOTACurrentHeader;

    #if OTA_ACKS_ON == TRUE
        sZCL_Address.eAddressMode = E_ZCL_AM_SHORT;
    #else
        sZCL_Address.eAddressMode = E_ZCL_AM_SHORT_NO_ACK;
    #endif
    sZCL_Address.uAddress.u16DestinationAddress = psOTA_Common->sOTACallBackMessage.sPersistedData.u16ServerShortAddress;

#if (OTA_MAX_CO_PROCESSOR_IMAGES != 0)
    if(psOTA_Common->sOTACallBackMessage.sPersistedData.bIsCoProcessorImage)
    {
        sOTACurrentHeader = sOtaGetHeader(&asCommonCoProcessorOTAHeader[psOTA_Common->sOTACallBackMessage.sPersistedData.u8CoProcessorOTAHeaderIndex][0]);
    }
    else
    {
        sOTACurrentHeader = sOtaGetHeader(psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header);
    }
#else
    sOTACurrentHeader = sOtaGetHeader(psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header);
#endif


    switch(psOTA_Common->sOTACallBackMessage.eEventId)
    {
        case E_CLD_OTA_INTERNAL_COMMAND_TIMER_EXPIRED:
        {
            vOtaHandleTimedBlockRequest(psOTA_Common, &bPollRequired,&sOTACurrentHeader,psEndPointDefinition);
            DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 25 ..\n");
        }
        break;

        case E_CLD_OTA_COMMAND_BLOCK_RESPONSE:
        {
            bPollRequired = bOtaHandleBlockResponseInProgress(psOTA_Common,
                                                       psEndPointDefinition,
                                                       &sZCL_Address,
                                                       &sOTACurrentHeader);
        }  /* end of case block response */
        break;

        case E_CLD_OTA_INTERNAL_COMMAND_RCVD_DEFAULT_RESPONSE:
            /* Move to normal state */
            vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_NORMAL, psEndPointDefinition, psOTA_Common);

             /* Give a call back to the user */
            eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_OTA_DL_ABORTED,psEndPointDefinition);
            break;

        default:
            break;
    }

    if(bPollRequired)
    {
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN7x)
        if(psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset > psOTA_Common->sOTACallBackMessage.sPersistedData.u32Step)
        {
            /* persisted data changed send event to the  application to save it*/
            DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 33..\n");
            eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_SAVE_CONTEXT,psEndPointDefinition);
            psOTA_Common->sOTACallBackMessage.sPersistedData.u32Step = ( (psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset + 1023 ) >
                                                                        psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset)?
                                                                    psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset + 1024 :
                                                                    (0xFFFFFFFF - psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset - 1);
        }
#else
        if(psOTA_Common->sOTACallBackMessage.sPersistedData.u32CurrentFlashOffset % 1024 == 0)
        {
            eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_SAVE_CONTEXT,psEndPointDefinition);
        }
#endif
        /* request has been sent make sure we poll for the response too otherwise might miss it
            only valid for end device*/
        eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_POLL_REQUIRED,psEndPointDefinition);
    }
}

/****************************************************************************
 **
 ** NAME:       vOtaUpgManClientStateDownloadComplete
 **
 ** DESCRIPTION:
 ** OTA client download complete state
 ** PARAMETERS:                           Name                           Usage
 ** void                               *pvStruct                        OTA custom data
 ** tsZCL_EndPointDefinition           *psEndPointDefinition           Endpoint definition
 ** RETURN:
 ** None
 ****************************************************************************/
PRIVATE  void vOtaUpgManClientStateDownloadComplete(
                                  void                        *pvStruct ,
                                  tsZCL_EndPointDefinition    *psEndPointDefinition)
{
    tsOTA_Common *psOTA_Common = (tsOTA_Common *)pvStruct;
    bool_t bPollRequired       = FALSE;
    bool_t bCountDown          = FALSE;
    bool_t bWaitToUpgrade      = FALSE;

    tsOTA_ImageHeader sOTAHeader;
    tsOTA_ImageHeader sOTACurrentHeader;

    memset(&sOTAHeader,0,sizeof(tsOTA_ImageHeader));
    memset(&sOTACurrentHeader,0,sizeof(tsOTA_ImageHeader));

#ifdef APP0 /* Building with selective OTA */
     if(psOTA_Common->sOTACallBackMessage.sPersistedData.bStackDownloadActive)
     {
         sOTACurrentHeader = sOtaGetHeader(APP_pu8GetApp1OtaHeader());
     }
     else
#endif
#ifdef SOTA_ENABLED
    if (!sOtaInitBlobOtaHeader(psOTA_Common->sOTACallBackMessage.sPersistedData.blobId, &sOTAHeader))
#endif
     {
         sOTACurrentHeader = sOtaGetHeader(au8OtaHeader);
     }

#if (OTA_MAX_CO_PROCESSOR_IMAGES != 0)
    if(psOTA_Common->sOTACallBackMessage.sPersistedData.bIsCoProcessorImage)
    {
        sOTACurrentHeader = sOtaGetHeader(&asCommonCoProcessorOTAHeader[psOTA_Common->sOTACallBackMessage.sPersistedData.u8CoProcessorOTAHeaderIndex][0]);
        sOTAHeader.u32FileVersion = psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32DownloadedFileVersion;
        sOTAHeader.u16ImageType = sOTACurrentHeader.u16ImageType;
        sOTAHeader.u16ManufacturerCode = sOTACurrentHeader.u16ManufacturerCode;
    }
    else
    {
        sOTAHeader = sOtaGetHeader( psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header);
    }
#else
    sOTAHeader = sOtaGetHeader(psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header);
#endif
    switch(psOTA_Common->sOTACallBackMessage.eEventId)
    {
        case E_CLD_OTA_INTERNAL_COMMAND_TIMER_EXPIRED:
        {
            vOtaHandleEndRequestRetries(psOTA_Common, &bPollRequired,&sOTAHeader,psEndPointDefinition);
            DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 25 ..\n");
        }
        break;
        case  E_CLD_OTA_COMMAND_UPGRADE_END_RESPONSE:
        {
            tsOTA_UpgradeEndResponsePayload sReceivedPayload = psOTA_Common->sOTACallBackMessage.uMessage.sUpgradeResponsePayload;
            DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 35 ..\n");
            if(psOTA_Common->sOTACallBackMessage.sPersistedData.bIsSpecificFile)
            {
                sOTAHeader.u16ImageType = psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16ImageType;
                sOTAHeader.u16ManufacturerCode = psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16ManfId;
                sOTAHeader.u32FileVersion = psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32DownloadedFileVersion;
            }
            // is the response for me
            if(((sReceivedPayload.u16ImageType == sOTAHeader.u16ImageType)||(sReceivedPayload.u16ImageType == 0xffff)) &&
               ((sReceivedPayload.u16ManufacturerCode == sOTAHeader.u16ManufacturerCode)||(sReceivedPayload.u16ManufacturerCode == 0xffff)) &&
               ((sReceivedPayload.u32FileVersion == sOTAHeader.u32FileVersion) ||(sReceivedPayload.u32FileVersion  == 0xffffffff)))
            {
                DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 34 ..\n");
                if((sReceivedPayload.u32CurrentTime == 0)&&(sReceivedPayload.u32UpgradeTime != 0xffffffff))
                {
                     psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime() + sReceivedPayload.u32UpgradeTime;
                     bCountDown = TRUE;
                }
                else if(sReceivedPayload.u32UpgradeTime == 0xffffffff )
                {
                    bWaitToUpgrade = TRUE;
                }
                else
                {
                    if(sReceivedPayload.u32UpgradeTime == 0)
                    {
                        psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime();
                    }
                    else
                    {
                        psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = sReceivedPayload.u32UpgradeTime;
                    }
                    bCountDown = TRUE;
                }
            }

            /* check wait to upgrade or count down */
            if(bCountDown || bWaitToUpgrade)
            {
                psOTA_Common->sOTACallBackMessage.uMessage.sUpgradeDowngradeVerify.eUpgradeDowngradeStatus = E_ZCL_SUCCESS;
                psOTA_Common->sOTACallBackMessage.uMessage.sUpgradeDowngradeVerify.u32CurrentImageVersion = sOTACurrentHeader.u32FileVersion;
                psOTA_Common->sOTACallBackMessage.uMessage.sUpgradeDowngradeVerify.u32DownloadImageVersion = sOTAHeader.u32FileVersion;
                eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_SWITCH_TO_UPGRADE_DOWNGRADE,psEndPointDefinition);

                /* check callback even status */
                if(psOTA_Common->sOTACallBackMessage.uMessage.sUpgradeDowngradeVerify.eUpgradeDowngradeStatus != E_ZCL_SUCCESS)
                {
                    /* persisted data changed send event to the  application to save it*/
                    eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_SAVE_CONTEXT,psEndPointDefinition);
                    vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_NORMAL, psEndPointDefinition, psOTA_Common);
                    return;
                }
            }

            if(bCountDown)
            {
                vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_COUNT_DOWN,psEndPointDefinition,psOTA_Common);
            }
            else if(bWaitToUpgrade)
            {
                vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_WAIT_TO_UPGRADE,psEndPointDefinition,psOTA_Common);
                psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime() + 60*60;// Poll the server if required.
            }
        }
        break;
        case E_CLD_OTA_INTERNAL_COMMAND_RCVD_DEFAULT_RESPONSE:
            /* Move to normal state */
            vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_NORMAL, psEndPointDefinition, psOTA_Common);

             /* Give a call back to the user */
            eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_OTA_DL_ABORTED,psEndPointDefinition);
            break;
        default:
        return;
    }
    /* persisted data changed send event to the  application to save it*/
    eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_SAVE_CONTEXT,psEndPointDefinition);
    if (bPollRequired)
    {  /* request has been sent make sure we poll for the response too otherwise might miss it
          only valid for end device*/
        eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_POLL_REQUIRED,psEndPointDefinition);
    }
}

/****************************************************************************
 **
 ** NAME:       vOtaUpgManClientStateWaitForRunCommand
 **
 ** DESCRIPTION:
 ** OTA client wait for run command state
 ** PARAMETERS:                           Name                           Usage
 ** void                               *pvStruct                        OTA custom data
 ** tsZCL_EndPointDefinition           *psEndPointDefinition           Endpoint definition
 ** RETURN:
 ** None
 ****************************************************************************/

PRIVATE  void vOtaUpgManClientStateWaitForRunCommand(
                                     void                        *pvStruct,
                                     tsZCL_EndPointDefinition    *psEndPointDefinition)
{
    tsOTA_Common *psOTA_Common = (tsOTA_Common *)pvStruct;
    tsOTA_ImageHeader sOTAHeader;
    bool_t bCountDown =  FALSE;
#if (OTA_MAX_CO_PROCESSOR_IMAGES != 0)
    if(psOTA_Common->sOTACallBackMessage.sPersistedData.bIsCoProcessorImage)
    {
        sOTAHeader = sOtaGetHeader(&asCommonCoProcessorOTAHeader[psOTA_Common->sOTACallBackMessage.sPersistedData.u8CoProcessorOTAHeaderIndex][0]);
        sOTAHeader.u32FileVersion = psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32DownloadedFileVersion;
    }
    else
    {
        sOTAHeader = sOtaGetHeader(psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header);
    }
#else
    sOTAHeader = sOtaGetHeader(psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header);
#endif
    if(psOTA_Common->sOTACallBackMessage.sPersistedData.bIsSpecificFile)
    {
        sOTAHeader.u16ImageType = psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16ImageType;
        sOTAHeader.u16ManufacturerCode = psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16ManfId;
        sOTAHeader.u32FileVersion = psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32DownloadedFileVersion;
    }
    switch(psOTA_Common->sOTACallBackMessage.eEventId)
    {
        case E_CLD_OTA_INTERNAL_COMMAND_TIMER_EXPIRED:
        {
            tsOTA_UpgradeEndRequestPayload sEndRequest;
            tsZCL_Address sZCL_Address;
    #if (OTA_ACKS_ON == TRUE)
                sZCL_Address.eAddressMode = E_ZCL_AM_SHORT;
    #else
                sZCL_Address.eAddressMode = E_ZCL_AM_SHORT_NO_ACK;
    #endif
            sZCL_Address.uAddress.u16DestinationAddress = psOTA_Common->sOTACallBackMessage.sPersistedData.u16ServerShortAddress;
            
            sEndRequest.u8Status = OTA_STATUS_SUCCESS;
            sEndRequest.u16ImageType = sOTAHeader.u16ImageType;
            sEndRequest.u16ManufacturerCode = sOTAHeader.u16ManufacturerCode;
            sEndRequest.u32FileVersion = sOTAHeader.u32FileVersion;

            eOTA_ClientUpgradeEndRequest(psEndPointDefinition->u8EndPointNumber,
                                         psOTA_Common->sOTACallBackMessage.sPersistedData.u8DstEndpoint,
                                         &sZCL_Address,
                                         &sEndRequest );
            psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime() + 60*60;// Poll the server if required.
        }
        break;

        case E_CLD_OTA_COMMAND_UPGRADE_END_RESPONSE:
        {
            tsOTA_UpgradeEndResponsePayload sReceivedPayload = psOTA_Common->sOTACallBackMessage.uMessage.sUpgradeResponsePayload;
            DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 36 ..\n");
            // is the response for me
            if(((sReceivedPayload.u16ImageType == sOTAHeader.u16ImageType)||(sReceivedPayload.u16ImageType == 0xffff)) &&
               ((sReceivedPayload.u16ManufacturerCode == sOTAHeader.u16ManufacturerCode)||(sReceivedPayload.u16ManufacturerCode == 0xffff)) &&
               ((sReceivedPayload.u32FileVersion == sOTAHeader.u32FileVersion) ||(sReceivedPayload.u32FileVersion  == 0xffffffff)))
            {
                DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 37 ..\n");
                if((sReceivedPayload.u32CurrentTime == 0)&&(sReceivedPayload.u32UpgradeTime != 0xffffffff))
                {
                    psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime() + sReceivedPayload.u32UpgradeTime;
                    bCountDown = TRUE;
                }
                else if(sReceivedPayload.u32UpgradeTime == 0xffffffff )
                {   /* remain in same state and wait for next end response */
                    psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime() + 60*60;// Poll the server if required.
                }
                else
                {
                    if(sReceivedPayload.u32UpgradeTime == 0)
                    {
                        psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime();
                    }
                    else
                    {
                        psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = sReceivedPayload.u32UpgradeTime;
                    }
                    bCountDown = TRUE;
                }
            }
            if(bCountDown)
            {
               vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_COUNT_DOWN,psEndPointDefinition,psOTA_Common);
            }
        }
        break;
        case E_CLD_OTA_INTERNAL_COMMAND_RCVD_DEFAULT_RESPONSE:
            /* Move to normal state */
            vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_NORMAL, psEndPointDefinition, psOTA_Common);

             /* Give a call back to the user */
            eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_OTA_DL_ABORTED,psEndPointDefinition);
        break;
        default:
            break;

    }

    /* persisted data changed send event to the  application to save it*/
    eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_SAVE_CONTEXT,psEndPointDefinition);
}
/****************************************************************************
 **
 ** NAME:       vOtaUpgManClientStateCountDown
 **
 ** DESCRIPTION:
 ** OTA client count down state
 ** PARAMETERS:                           Name                           Usage
 ** void                               *pvStruct                        OTA custom data
 ** tsZCL_EndPointDefinition           *psEndPointDefinition           Endpoint definition
 ** RETURN:
 ** None
 ****************************************************************************/

PRIVATE  void vOtaUpgManClientStateCountDown(
                                           void                        *pvStruct,
                                           tsZCL_EndPointDefinition    *psEndPointDefinition)
{
     tsOTA_Common *psOTA_Common = (tsOTA_Common *)pvStruct;
     if(psOTA_Common->sOTACallBackMessage.eEventId == E_CLD_OTA_INTERNAL_COMMAND_TIMER_EXPIRED)
     {
         /* check own image or co-processor image , for own image reboot from new image */
         if((psOTA_Common->sOTACallBackMessage.sPersistedData.bIsCoProcessorImage)
#if (OTA_MAX_CO_PROCESSOR_IMAGES != 0)
                 ||(bIsCoProcessorImgUpgdDependent)
#endif
         )
         {
             /* send call back event to application, switch to new image for co-processor */
             eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_CO_PROCESSOR_SWITCH_TO_NEW_IMAGE,psEndPointDefinition);
         }
         else if(psOTA_Common->sOTACallBackMessage.sPersistedData.bIsSpecificFile)
         {
             /* send call back event to application, switch to new image for co-processor */
             eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_SPECIFIC_FILE_USE_NEW_FILE,psEndPointDefinition);
         }
         else
         {
             eOTA_ClientSwitchToNewImage(psEndPointDefinition->u8EndPointNumber);
         }
         /* change state to normal */
         vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_NORMAL,psEndPointDefinition,psOTA_Common);
     }
}
/****************************************************************************
 **
 ** NAME:       vOtaAbortDownload
 **
 ** DESCRIPTION:
 ** Aborts the download
 ** PARAMETERS:                           Name                           Usage
 ** tsOTA_Common                       *psOTA_Common                   OTA custom data
 ** tsZCL_EndPointDefinition           *psEndPointDefinition           Endpoint definition
 ** RETURN:
 ** None
 ****************************************************************************/
PRIVATE  void vOtaAbortDownload(
                                tsOTA_Common             *psOTA_Common,
                                tsZCL_EndPointDefinition *psEndPointDefinition)
{
    //Image is sent bigger than what we can support.
    tsOTA_ImageHeader sOTAHeader ;
    tsOTA_UpgradeEndRequestPayload sEndRequest;
    tsZCL_Address sZCL_Address;

    #if OTA_ACKS_ON == TRUE
        sZCL_Address.eAddressMode = E_ZCL_AM_SHORT;
    #else
        sZCL_Address.eAddressMode = E_ZCL_AM_SHORT_NO_ACK;
    #endif
    sZCL_Address.uAddress.u16DestinationAddress = psOTA_Common->sOTACallBackMessage.sPersistedData.u16ServerShortAddress;

#if (OTA_MAX_CO_PROCESSOR_IMAGES != 0)
    if(psOTA_Common->sOTACallBackMessage.sPersistedData.bIsCoProcessorImage)
    {
        sOTAHeader = sOtaGetHeader(&asCommonCoProcessorOTAHeader[psOTA_Common->sOTACallBackMessage.sPersistedData.u8CoProcessorOTAHeaderIndex][0]);
    }
    else
    {
        sOTAHeader = sOtaGetHeader(psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header);
    }
#else
    sOTAHeader = sOtaGetHeader(psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header);
#endif
    sEndRequest.u8Status = OTA_STATUS_ABORT;
    if(psOTA_Common->sOTACallBackMessage.sPersistedData.bIsSpecificFile)
    {
        sEndRequest.u16ImageType = sOTAHeader.u16ImageType;
        sEndRequest.u16ManufacturerCode = sOTAHeader.u16ManufacturerCode;
    }
    else
    {
        sEndRequest.u16ImageType = psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16ImageType;
        sEndRequest.u16ManufacturerCode = psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16ManfId;
    }
    sEndRequest.u32FileVersion = psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32DownloadedFileVersion;
    eOTA_ClientUpgradeEndRequest(psOTA_Common->sReceiveEventAddress.u8DstEndpoint,
                    psOTA_Common->sReceiveEventAddress.u8SrcEndpoint,
                    &sZCL_Address,
                    &sEndRequest);
    psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset = 0;
    vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_NORMAL,psEndPointDefinition,psOTA_Common);
    eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_OTA_DL_ABORTED,psEndPointDefinition);
    /* persisted data changed send event to the  application to save it*/
    eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_SAVE_CONTEXT,psEndPointDefinition);   
#ifdef APP0
    u32LocalIndex = 0;
    memset (u8StandaloneBuffer, 0xFF, NVM_BYTES_PER_SEGMENT);
#endif
}
/****************************************************************************
 **
 ** NAME:       vOtaRequestNextBlock
 **
 ** DESCRIPTION:
 ** Request the next image block request from server
 ** PARAMETERS:                           Name                           Usage
 ** uint8                               u8SrcEndpoint                  Source endpoint
 ** uint8                               u8DstEndpoint                  Destination endpoint
 ** tsOTA_BlockRequest                 *psBlock                        payload
 ** tsZCL_EndPointDefinition           *psEndPointDefinition           Endpoint definition
 ** tsOTA_Common                       *psOTA_Common                   OTA custom data
 ** tsZCL_Address                      *psZCL_Address                  Address of server
 ** RETURN:
 ** None
 ****************************************************************************/
PRIVATE  void vOtaRequestNextBlock(
                           uint8                     u8SrcEndpoint,
                           uint8                     u8DstEndpoint,
                           tsOTA_BlockRequest       *psBlock,
                           tsZCL_EndPointDefinition *psEndPointDefinition,
                           tsOTA_Common             *psOTA_Common,
                           tsZCL_Address            *psZCL_Address)

{
    teZCL_Status eStatus;
#ifdef OTA_CLD_ATTR_REQUEST_DELAY
    psBlock->u8FieldControl |= 0x02;
    psBlock->u16BlockRequestDelay = psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16MinBlockRequestDelay;
#endif
    eZCL_SetLocalAttributeValue(E_CLD_OTA_ATTR_FILE_OFFSET,FALSE,TRUE,
                         psEndPointDefinition,
                         psOTA_Common->sOTACustomCallBackEvent.psClusterInstance,
                         &psBlock->u32FileOffset);
#ifdef OTA_PAGE_REQUEST_SUPPORT
    if((psOTA_Common->sOTACallBackMessage.sPersistedData.sPageReqParams.bPageReqOn)&&(!psOTA_Common->sOTACallBackMessage.sPersistedData.bIsSpecificFile))
    {
        tsOTA_ImagePageRequest sPageReq;
        sPageReq.u8FieldControl = 0;
        sPageReq.u16ManufactureCode = psBlock->u16ManufactureCode;
        sPageReq.u16ImageType = psBlock->u16ImageType;
        sPageReq.u32FileVersion = psBlock->u32FileVersion;
        sPageReq.u32FileOffset = psBlock->u32FileOffset;
        sPageReq.u8MaxDataSize = psBlock->u8MaxDataSize;
        sPageReq.u16PageSize = OTA_PAGE_REQ_PAGE_SIZE;
        sPageReq.u16ResponseSpacing = OTA_PAGE_REQ_RESPONSE_SPACING;
        psOTA_Common->sOTACallBackMessage.sPersistedData.sPageReqParams.u32PageReqFileOffset = psBlock->u32FileOffset;
        eStatus = eOTA_ClientImagePageRequest(u8SrcEndpoint,u8DstEndpoint,psZCL_Address,&sPageReq);
        DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 90 ..%x\n",eStatus);
        return;
    }
#endif
    DBG_vPrintf(TRACE_BLOCKS, "BLOCK REQ -> %08x\n", psBlock->u32FileOffset);
    eStatus = eOTA_ClientImageBlockRequest(u8SrcEndpoint,u8DstEndpoint,psZCL_Address,psBlock);
    DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 50 ..%x\n",eStatus);

}
/****************************************************************************
 **
 ** NAME:       vOtaHandleTimedBlockRequest
 **
 ** DESCRIPTION:
 ** Handle retries of image block request after timeout
 ** PARAMETERS:                           Name                           Usage
 ** tsOTA_Common                       *psOTA_Common                   OTA custom data
 ** bool_t                             *pbProceed                      result
 ** tsOTA_ImageHeader                  *psOTACurrentHeader             image header
 ** tsZCL_EndPointDefinition           *psEndPointDefinition           Endpoint definition
 ** RETURN:
 ** None
 ****************************************************************************/

PRIVATE  void vOtaHandleTimedBlockRequest(
                                       tsOTA_Common             *psOTA_Common,
                                       bool_t                   *pbPollRequired,
                                       tsOTA_ImageHeader        *psOTACurrentHeader,
                                       tsZCL_EndPointDefinition *psEndPointDefinition)
{
    tsOTA_BlockRequest sBlock;
    tsZCL_Address sZCLAddress;


    #if OTA_ACKS_ON == TRUE
        sZCLAddress.eAddressMode = E_ZCL_AM_SHORT;
    #else
        sZCLAddress.eAddressMode = E_ZCL_AM_SHORT_NO_ACK;
    #endif

    sZCLAddress.uAddress.u16DestinationAddress = psOTA_Common->sOTACallBackMessage.sPersistedData.u16ServerShortAddress;

#ifdef OTA_CLD_ATTR_REQUEST_DELAY
    if((psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16MinBlockRequestDelay != 0)&&
            (psOTA_Common->sOTACallBackMessage.sPersistedData.bWaitForBlockReq == TRUE))
    {
        eZCL_UpdateMsTimer(psEndPointDefinition, TRUE,(uint32)psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16MinBlockRequestDelay*1000);
        return;
    }
#endif
#ifdef OTA_PAGE_REQUEST_SUPPORT
    if(psOTA_Common->sOTACallBackMessage.sPersistedData.sPageReqParams.bPageReqOn)
    {
        if((psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset <
          (psOTA_Common->sOTACallBackMessage.sPersistedData.sPageReqParams.u32PageReqFileOffset + OTA_PAGE_REQ_PAGE_SIZE))&&
          (0 == psOTA_Common->sOTACallBackMessage.sPersistedData.u8Retry))
        {
            psOTA_Common->sOTACallBackMessage.sPersistedData.u8Retry++;
            return;
        }
        else if((psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset ==
                (psOTA_Common->sOTACallBackMessage.sPersistedData.sPageReqParams.u32PageReqFileOffset + OTA_PAGE_REQ_PAGE_SIZE))&&
                (0 == psOTA_Common->sOTACallBackMessage.sPersistedData.u8Retry))
        {
            psOTA_Common->sOTACallBackMessage.sPersistedData.sPageReqParams.u8PageReqRetry = 0;
            DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 111 ..\n");
        }
        if((psOTA_Common->sOTACallBackMessage.sPersistedData.u8Retry > CLD_OTA_MAX_BLOCK_PAGE_REQ_RETRIES)
                ||(psOTA_Common->sOTACallBackMessage.sPersistedData.sPageReqParams.u8PageReqRetry >
                CLD_OTA_MAX_BLOCK_PAGE_REQ_RETRIES))
        {
            psOTA_Common->sOTACallBackMessage.sPersistedData.sPageReqParams.bPageReqOn = FALSE;
            psOTA_Common->sOTACallBackMessage.sPersistedData.u8Retry = 0;
            psOTA_Common->sOTACallBackMessage.sPersistedData.sPageReqParams.u8PageReqRetry = 0;
            DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 112 ..\n");
        }
    }
#endif
    if(psOTA_Common->sOTACallBackMessage.sPersistedData.u8Retry <=CLD_OTA_MAX_BLOCK_PAGE_REQ_RETRIES)
    {
        DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 113 .. %d\n" , psOTA_Common->sOTACallBackMessage.sPersistedData.u8Retry);
        psOTA_Common->sOTACallBackMessage.sPersistedData.u8Retry++;
#ifdef OTA_PAGE_REQUEST_SUPPORT
        if(psOTA_Common->sOTACallBackMessage.sPersistedData.sPageReqParams.bPageReqOn)
        {
            psOTA_Common->sOTACallBackMessage.sPersistedData.sPageReqParams.u8PageReqRetry++;
        }
#endif
        sBlock.u32FileOffset = psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset;
        if(psOTA_Common->sOTACallBackMessage.sPersistedData.bIsSpecificFile)
        {
            sBlock.u8FieldControl = 1;
            sBlock.u16ManufactureCode = psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16ManfId;
            sBlock.u16ImageType = psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16ImageType;
#ifndef OTA_UNIT_TEST_FRAMEWORK
            sBlock.u64RequestNodeAddress = ZPS_u64AplZdoGetIeeeAddr();
#endif
        }
        else
        {
            sBlock.u8FieldControl = 0;
            sBlock.u16ManufactureCode = psOTACurrentHeader->u16ManufacturerCode;
            sBlock.u16ImageType = psOTACurrentHeader->u16ImageType;
        }
        sBlock.u8MaxDataSize = OTA_MAX_BLOCK_SIZE;
        sBlock.u32FileVersion = psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32DownloadedFileVersion;

#ifndef OTA_TIME_INTERVAL_BETWEEN_REQUESTS
        psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime()+OTA_TIME_INTERVAL_BETWEEN_RETRIES+1;
#else
        psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime()+OTA_TIME_INTERVAL_BETWEEN_REQUESTS+1;
#endif
        if(psOTA_Common->sOTACallBackMessage.sPersistedData.bWaitForNextBlockReq == FALSE)
        {
            vOtaRequestNextBlock(psEndPointDefinition->u8EndPointNumber,
                             psOTA_Common->sOTACallBackMessage.sPersistedData.u8DstEndpoint,
                             &sBlock,
                             psEndPointDefinition,
                             psOTA_Common,
                             &sZCLAddress);
        }
                         *pbPollRequired = TRUE;

    }
    else
    {
        DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 114 ..\n");
        vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_NORMAL,psEndPointDefinition,psOTA_Common);
        *pbPollRequired = FALSE;
    }

}
/****************************************************************************
 **
 ** NAME:       vOtaHandleEndRequestRetries
 **
 ** DESCRIPTION:
 ** Handle retries of Upgrade end request after timeout
 ** PARAMETERS:                           Name                           Usage
 ** tsOTA_Common                       *psOTA_Common                   OTA custom data
 ** bool_t                             *pbPollRequired                      result
 ** tsOTA_ImageHeader                  *psOTACurrentHeader             image header
 ** tsZCL_EndPointDefinition           *psEndPointDefinition           Endpoint definition
 ** RETURN:
 ** None
 ****************************************************************************/

PRIVATE  void vOtaHandleEndRequestRetries(
                                        tsOTA_Common             *psOTA_Common,
										bool_t                     *pbPollRequired,
                                        tsOTA_ImageHeader        *psOTACurrentHeader,
                                        tsZCL_EndPointDefinition *psEndPointDefinition)
{
    tsOTA_UpgradeEndRequestPayload sEndRequest;
    tsZCL_Address sZCL_Address;

    #if OTA_ACKS_ON == TRUE
        sZCL_Address.eAddressMode = E_ZCL_AM_SHORT;
    #else
        sZCL_Address.eAddressMode = E_ZCL_AM_SHORT_NO_ACK;
    #endif
    sZCL_Address.uAddress.u16DestinationAddress = psOTA_Common->sOTACallBackMessage.sPersistedData.u16ServerShortAddress;

    if(psOTA_Common->sOTACallBackMessage.sPersistedData.u8Retry <=CLD_OTA_MAX_BLOCK_PAGE_REQ_RETRIES)
    {
        psOTA_Common->sOTACallBackMessage.sPersistedData.u8Retry++;
        sEndRequest.u32FileVersion = psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32DownloadedFileVersion;
        if(psOTA_Common->sOTACallBackMessage.sPersistedData.bIsSpecificFile)
        {
            sEndRequest.u16ImageType = psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16ImageType;
            sEndRequest.u16ManufacturerCode = psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16ManfId;
        }
        else
        {
            sEndRequest.u16ImageType = psOTACurrentHeader->u16ImageType;
            sEndRequest.u16ManufacturerCode = psOTACurrentHeader->u16ManufacturerCode;
        }
        sEndRequest.u8Status = OTA_STATUS_SUCCESS;
        psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime()+OTA_TIME_INTERVAL_BETWEEN_END_REQUEST_RETRIES+1;

        eOTA_ClientUpgradeEndRequest(psOTA_Common->sReceiveEventAddress.u8DstEndpoint,
                                     psOTA_Common->sReceiveEventAddress.u8SrcEndpoint,
                                     &sZCL_Address,
                                     &sEndRequest );
        *pbPollRequired = TRUE;
    }
    else
    {
        if(psOTA_Common->sOTACallBackMessage.sPersistedData.bIsSpecificFile)
        {
            /* send call back event to application, that upgrade end response is not received */
              eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_SPECIFIC_FILE_NO_UPGRADE_END_RESPONSE,psEndPointDefinition);
        }
        vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_NORMAL,psEndPointDefinition,psOTA_Common);
        *pbPollRequired = FALSE;
    }
}
/****************************************************************************
 **
 ** NAME:       vOtaHandleWaitForDataStatus
 **
 ** DESCRIPTION:
 ** Handle wait for data status
 ** PARAMETERS:                           Name                           Usage
 ** tsZCL_EndPointDefinition           *psEndPointDefinition           Endpoint definition
 ** tsOTA_Common                       *psOTA_Common                   OTA custom data
 ** tsOTA_ImageBlockResponsePayload    *psBlockResponse                Image block response payload
 ** RETURN:
 ** None
 ****************************************************************************/
PRIVATE  void vOtaHandleWaitForDataStatus(
                                                    tsZCL_EndPointDefinition        *psEndPointDefinition,
                                                    tsOTA_Common                    *psOTA_Common,
                                                    tsOTA_ImageBlockResponsePayload *psBlockResponse)
{
#ifdef OTA_CLD_ATTR_REQUEST_DELAY
    if(psBlockResponse->uMessage.sWaitForData.u16BlockRequestDelayMs == 0)
    {
        /* Disable rate limiting feature */ 
        psOTA_Common->sOTACallBackMessage.sPersistedData.bWaitForBlockReq = FALSE;
    }
    else
    {
        /* Enable the rate limiting feature */
        psOTA_Common->sOTACallBackMessage.sPersistedData.bWaitForBlockReq = TRUE;
    }
    
    /* Enter only if Disabling OTA block request delay after being enabled or If it is requested to enable*/
    if((((psOTA_Common->sOTACallBackMessage.sPersistedData.bWaitForBlockReq == FALSE) && (psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16MinBlockRequestDelay != 0)) ||
          (psOTA_Common->sOTACallBackMessage.sPersistedData.bWaitForBlockReq == TRUE)))
    {
        /* Clamp the Maximum Block Request Delay to the Maximum supported at client */
        if(psBlockResponse->uMessage.sWaitForData.u16BlockRequestDelayMs <= OTA_BLOCK_REQUEST_DELAY_MAX_VALUE)
            psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16MinBlockRequestDelay = psBlockResponse->uMessage.sWaitForData.u16BlockRequestDelayMs;
        else
            psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16MinBlockRequestDelay = OTA_BLOCK_REQUEST_DELAY_MAX_VALUE;
            
        
        tsZCL_ClusterInstance *psClusterInstance;
        tsZCL_EndPointDefinition *psEndPointDefinition1;
        tsOTA_Common *psOtaCustomData;
         
        if(eOtaFindCluster(psEndPointDefinition->u8EndPointNumber,
                          &psEndPointDefinition1,
                          &psClusterInstance,
                          &psOtaCustomData,
                          FALSE) == E_ZCL_SUCCESS)
        {
            teZCL_Status eStatus = eZCL_SetLocalAttributeValue(E_CLD_OTA_ATTR_REQUEST_DELAY,
                                                               FALSE,
                                                               TRUE,
                                                               psEndPointDefinition1,
                                                               psClusterInstance,
                                                               &psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16MinBlockRequestDelay);

            DBG_vPrintf(TRACE_OTA_DEBUG, "\n\reZCL_SetLocalAttributeValue: %x\n", eStatus);
        }
        eOTA_RestoreClientData(psEndPointDefinition->u8EndPointNumber,
                               &psOTA_Common->sOTACallBackMessage.sPersistedData,
                               FALSE);

        if(psBlockResponse->uMessage.sWaitForData.u32CurrentTime >= psBlockResponse->uMessage.sWaitForData.u32RequestTime)
        {
            if(psOTA_Common->sOTACallBackMessage.sPersistedData.bWaitForBlockReq)
                eZCL_UpdateMsTimer(psEndPointDefinition, TRUE,(uint32)psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16MinBlockRequestDelay*1000);
            else 
                eZCL_UpdateMsTimer(psEndPointDefinition, TRUE,(uint32)OTA_MIN_TIMER_MS_RESOLUTION);
        }
        else if(psOTA_Common->sOTACallBackMessage.sPersistedData.bWaitForBlockReq)
        {
            uint32 u32MinBlockRequestDelay = ( psBlockResponse->uMessage.sWaitForData.u32RequestTime - psBlockResponse->uMessage.sWaitForData.u32CurrentTime);
            eZCL_UpdateMsTimer(psEndPointDefinition, TRUE,(u32MinBlockRequestDelay*1000));
        }        
    }
#endif
    psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = 0;
    if(psBlockResponse->uMessage.sWaitForData.u32CurrentTime == 0)
    {
        psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime()+ psBlockResponse->uMessage.sWaitForData.u32RequestTime;
    }
    else
    {
        psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime() +
                                                                                      ( psBlockResponse->uMessage.sWaitForData.u32RequestTime -
                                                                                        psBlockResponse->uMessage.sWaitForData.u32CurrentTime);
    }
    psOTA_Common->sOTACallBackMessage.sPersistedData.u8Retry = 0;
}


#ifdef INTERNAL_ENCRYPTED
PRIVATE void vOtaProcessInternalEncryption(
         uint8                       *pau8Buffer,
         uint32                      u32StartAddress,
         uint32                      u32DataSize,
         uint32                      u32ImageStartAddress)
{
    int i;
    uint32 u32UnEncryptedToWrite;
    uint32 u32EncryptedToWrite;
    uint32 u32EncryptionStart = ((uint32)(&(_enc_start))) - (uint32)(&(_flash_start));
    u32EncryptionStart += u32ImageStartAddress;           // adjust to flash area we are writing to
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
    /*Flash offset */
    uint32 u32IVLocation = u32ImageStartAddress + 0x10;
#else
    /*Flash offset */
    uint32 u32IVLocation = u32ImageStartAddress + 0x150;
#endif

    DBG_vPrintf(TRACE_INT_DECRYPT, "Flash Addr %08x u32EncryptionStart %08x %08x \n",
            u32StartAddress, u32EncryptionStart, u32ImageStartAddress);

    if ( u32StartAddress < u32EncryptionStart)
    {
        /* There is unencrypted data first */

        if ( ( u32StartAddress + u32DataSize) < u32EncryptionStart )
        {
            /* all of it unencrypted */
            u32UnEncryptedToWrite = u32DataSize;
            u32EncryptedToWrite = 0;
        }
        else
        {
            /* partially un encrypted */
            u32EncryptedToWrite = u32DataSize;
            u32UnEncryptedToWrite = 0;

            while ( ( u32StartAddress + u32UnEncryptedToWrite) < u32EncryptionStart ) {
                u32UnEncryptedToWrite += 16;
                u32EncryptedToWrite -= 16;
            }

        }
    }
    else
    {
        u32UnEncryptedToWrite = 0;
        u32EncryptedToWrite = u32DataSize;
    }

    DBG_vPrintf(TRACE_INT_DECRYPT, "Start at %08x u32UnEncryptedToWrite %d u32EncryptedToWrite %d\n",
            u32StartAddress,
            u32UnEncryptedToWrite,
            u32EncryptedToWrite);

    if (u32EncryptedToWrite > 0)
    {
        DBG_vPrintf(TRACE_INT_DECRYPT, "Decrypt %d bytes\n", u32EncryptedToWrite);
        uint8 au8Iv[0x10];
        uint8 au8DataOut[0x10];
        tsReg128 sKey;
        uint32 u32IVCount;
        /* Must have the full IV otherwise can't get here */
        if ( u32StartAddress <= u32IVLocation ) {
            /*
             * In this case we have not yet written the IV to flash,
             * it's still in the buffer we are processing
             * so get it from there
             */
            DBG_vPrintf(TRACE_INT_DECRYPT, "Get from buffer -> ");
            for(i=0;i<16;i++)
            {
                au8Iv[i] = pau8Buffer[(u32IVLocation - u32StartAddress)+i];
                DBG_vPrintf(TRACE_INT_DECRYPT, "%02x ", au8Iv[i]);
            }
            DBG_vPrintf(TRACE_INT_DECRYPT, "\n");
        }
        else
        {

#if (defined APP0)
                /* still in the flash write buffer */
                if ( (u32StartAddress - u32ImageStartAddress ) >= 512 )
                {
#endif

                /*Get the downloaded IV from Flash */
                DBG_vPrintf(TRACE_INT_DECRYPT, "READ FROM FLASH \n");
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x) || (defined APP0)
                bAHI_FullFlashRead(u32IVLocation,0x10,au8Iv);
#else
                OTA_PullImageChunk(au8Iv, 0x10, &u32IVLocation);
#endif

#if (defined APP0)
                }
                else
                {
                    uint32 u32LocInBuffer = ( u32IVLocation - u32ImageStartAddress );
                    DBG_vPrintf(TRACE_INT_DECRYPT, "Internal buffer 512 bytes \n");
                    for(i=0;i<16;i++)
                    {
                        au8Iv [ i ]  = u8StandaloneBuffer [ u32LocInBuffer + i ];
                    }
                }

#endif
            for(i=0;i<16;i++)
            {
                DBG_vPrintf(TRACE_INT_DECRYPT, "%02x ", au8Iv[i]);
            }
        }

        /* current write to address */
        uint32 u32BlockCount = u32StartAddress;
        u32BlockCount += u32UnEncryptedToWrite;
        /* take off encryption start -> gives encrypted bytes written so far */
        u32BlockCount -= u32EncryptionStart;
        /* convert no of byte to no of blocks */
        u32BlockCount = (u32BlockCount>>4);

        #ifndef LITTLE_ENDIAN_PROCESSOR
            u32IVCount = (((uint32)au8Iv[12])<<24) | (((uint32)au8Iv[13])<<16) | (((uint32)au8Iv[14])<<8) | (au8Iv[15]);
        #else
            memcpy((uint8*)&u32IVCount,&au8Iv[12],sizeof(uint32));
        #endif

        DBG_vPrintf(TRACE_INT_DECRYPT, "u32BlockCount = %08x  IV Count %08x\n", u32BlockCount, u32IVCount);
        u32IVCount += u32BlockCount;

        au8Iv[12] = (uint8)((u32IVCount >> 24) &  0xff);
        au8Iv[13] = (uint8)((u32IVCount >> 16) &  0xff);
        au8Iv[14] = (uint8)((u32IVCount >> 8) &  0xff);
        au8Iv[15] = (uint8)(u32IVCount &  0xff);

        DBG_vPrintf(TRACE_INT_DECRYPT,"The Plain IV :0x%02x 0x%02x 0x%02x 0x%02x\n",
                 au8Iv[12], au8Iv[13], au8Iv[14], au8Iv[15]);

#if (JENNIC_CHIP_FAMILY_JN516x) || (JENNIC_CHIP_FAMILY_JN517x)
        /*Get the EFUSE keys*/
        uint32 *pu32KeyPtr = (uint32*)FL_INDEX_SECTOR_ENC_KEY_ADDRESS;
#else
#ifdef OTA_USE_KEY_IN_PSECTOR
#else
    uint32 *pu32KeyPtr = g_au32OtaEncCred;    
#endif        
#endif
        sKey.u32register0 = *pu32KeyPtr;
        sKey.u32register1 = *(pu32KeyPtr+1);
        sKey.u32register2 = *(pu32KeyPtr+2);
        sKey.u32register3 = *(pu32KeyPtr+3);

        DBG_vPrintf(TRACE_INT_DECRYPT,"The Key is :\n");
        DBG_vPrintf(TRACE_INT_DECRYPT,"sKey.u32register0: 0x%08x ",sKey.u32register0);
        DBG_vPrintf(TRACE_INT_DECRYPT,"sKey.u32register1: 0x%08x ",sKey.u32register1);
        DBG_vPrintf(TRACE_INT_DECRYPT,"sKey.u32register2: 0x%08x ",sKey.u32register2);
        DBG_vPrintf(TRACE_INT_DECRYPT,"sKey.u32register3: 0x%08x\n",sKey.u32register3);
       
        uint32 u32Offset = u32UnEncryptedToWrite;
        while (u32EncryptedToWrite > 0)
       {
           /*Encrypt the IV*/
           DBG_vPrintf(TRACE_INT_DECRYPT, "TO DO -> %d at offset %d\n", u32EncryptedToWrite, u32Offset );
           vOTA_EncodeString(&sKey,au8Iv,au8DataOut);
           
           /* Decrypt a block of the buffer */
           for(i=0;i<16;i++)
           {
               pau8Buffer[u32Offset+i] = pau8Buffer[u32Offset+i] ^ au8DataOut[i];
           }

           /* increment the IV for the next block  */
           u32IVCount++;
           
           au8Iv[12] = (uint8)((u32IVCount >> 24) &  0xff);
           au8Iv[13] = (uint8)((u32IVCount >> 16) &  0xff);
           au8Iv[14] = (uint8)((u32IVCount >> 8) &  0xff);
           au8Iv[15] = (uint8)(u32IVCount &  0xff);           
           
           u32EncryptedToWrite -= 16;   /* 1 block of 16 bytes done */
           u32Offset += 16;             /* increment the buffer offset */

       }

    }
}
#endif

/****************************************************************************
 **
 ** NAME:       bOtaHandleImageNotify
 **
 ** DESCRIPTION:
 ** Handle Image Notify Commands
 ** PARAMETERS:                           Name                           Usage
 ** tsZCL_EndPointDefinition           *psEndPointDefinition           Endpoint definition
 ** tsOTA_Common                       *psOTA_Common                   OTA custom data
 **
 ** RETURN: True if a poll will be reqired
 ** None
 ****************************************************************************/
PRIVATE bool_t bOtaHandleImageNotify( tsOTA_Common *psOTA_Common,
                                    tsZCL_Address *psZCL_Address,
                                    uint8        u8SrcEndpoint,
                                    uint8       u8DstEndpoint)
{
    tsOTA_ImageNotifyCommand sReceivedFrame;
    tsOTA_QueryImageRequest sQueryImage;
    tsOTA_ImageHeader sOTAHeader;
    uint8 u8LoopCount;
    bool_t bSendReqForAllImages = FALSE;
    bool_t bPollRequired = FALSE;


#ifdef APP0 /* Building with selective OTA */
    if(psOTA_Common->sOTACallBackMessage.sPersistedData.bStackDownloadActive)
    {
        sOTAHeader = sOtaGetHeader(APP_pu8GetApp1OtaHeader());
    }
    else
#endif
#ifdef SOTA_ENABLED
    if (!sOtaInitBlobOtaHeader(psOTA_Common->sOTACallBackMessage.sPersistedData.blobId, &sOTAHeader))
#endif
    {
        sOTAHeader = sOtaGetHeader(au8OtaHeader);
    }

    sReceivedFrame = psOTA_Common->sOTACallBackMessage.uMessage.sImageNotifyPayload;

    /* check Notify payload type */
    if((sReceivedFrame.ePayloadType == E_CLD_OTA_QUERY_JITTER) ||
            (sReceivedFrame.ePayloadType == E_CLD_OTA_MANUFACTURER_ID_AND_JITTER && sReceivedFrame.u16ManufacturerCode == 0xFFFF))
    {
        /* Send Next Image Req command for all OTA images (own image plus co-processor images) */
        bSendReqForAllImages = TRUE;
    }
    else if(sReceivedFrame.ePayloadType == E_CLD_OTA_MANUFACTURER_ID_AND_JITTER)
    {
        /* check Manufacturer ID is matching with all OTA headers */
        /* Manufacturer ID is same for all images */
        if(sOTAHeader.u16ManufacturerCode == sReceivedFrame.u16ManufacturerCode)
        {
            /* Send Next Image Req command for all OTA images (own image plus co-processor images) */
            bSendReqForAllImages = TRUE;
        }
    }
    else if(sReceivedFrame.ePayloadType == E_CLD_OTA_ITYPE_MDID_JITTER)
    {
        /* check manufacturer ID */
        if(sOTAHeader.u16ManufacturerCode == sReceivedFrame.u16ManufacturerCode ||
                sReceivedFrame.u16ManufacturerCode == 0xFFFF)
        {
            if(sReceivedFrame.u16ImageType == 0xFFFF)
            {
                /* Send Next Image Req command for all OTA images (own image plus co-processor images) */
                bSendReqForAllImages = TRUE;
            }
            else
            {
                /* check for match image type */
                for(u8LoopCount = 0; u8LoopCount < OTA_TOTAL_ACTIVE_IMAGES; u8LoopCount++)
                {
                    /* get the OTA header */
                    if(u8LoopCount == 0) /* Own Header */
                    {
                    #ifdef APP0 /* Building with selective OTA */
                        if(psOTA_Common->sOTACallBackMessage.sPersistedData.bStackDownloadActive)
                        {
                            sOTAHeader = sOtaGetHeader(APP_pu8GetApp1OtaHeader());
                        }
                        else
                    #endif
                    #ifdef SOTA_ENABLED
                        if (!sOtaInitBlobOtaHeader(psOTA_Common->sOTACallBackMessage.sPersistedData.blobId, &sOTAHeader))
                    #endif
                        {
                            sOTAHeader = sOtaGetHeader(au8OtaHeader);
                        }

                    }
#if (OTA_MAX_CO_PROCESSOR_IMAGES != 0)
                    else
                    {
                        /* CoProcessor Headers */
                        sOTAHeader = sOtaGetHeader(&asCommonCoProcessorOTAHeader[u8LoopCount-1][0]);
                    }
#endif
                    if(sReceivedFrame.u16ImageType == sOTAHeader.u16ImageType)
                    {
                        uint8 u8RandomValue = (uint8)RND_u32GetRand(0,sReceivedFrame.u8QueryJitter);
                        bool_t bIsUnicast = OTA_IS_UNICAST(psOTA_Common->sReceiveEventAddress.u8DstAddrMode,psOTA_Common->sReceiveEventAddress.uDstAddress.u16Addr);

                        /* Send Query Image Request command */
                        if( bIsUnicast || (!bIsUnicast && u8RandomValue <= sReceivedFrame.u8QueryJitter) )
                        {
#ifdef OTA_CLD_HARDWARE_VERSIONS_PRESENT
                            sQueryImage.u16HardwareVersion = sOTAHeader.u16MinimumHwVersion;
#endif
                            sQueryImage.u16ImageType = sOTAHeader.u16ImageType;
                            sQueryImage.u16ManufacturerCode = sOTAHeader.u16ManufacturerCode;
                            sQueryImage.u32CurrentFileVersion = sOTAHeader.u32FileVersion;
#ifdef OTA_CLD_HARDWARE_VERSIONS_PRESENT
                            sQueryImage.u8FieldControl = 1;
#else
                            sQueryImage.u8FieldControl = 0;
#endif
                            eOTA_ClientQueryNextImageRequest(u8SrcEndpoint,u8DstEndpoint,psZCL_Address,&sQueryImage);
                            bPollRequired = TRUE;
                        }
                        break;
                    }
                }
            }
        }
    }
    else if(sReceivedFrame.ePayloadType == E_CLD_OTA_ITYPE_MDID_FVERSION_JITTER)
    {
        /* check manufacturer ID */
        if(sOTAHeader.u16ManufacturerCode == sReceivedFrame.u16ManufacturerCode ||
                sReceivedFrame.u16ManufacturerCode == 0xFFFF)
        {
            if(sReceivedFrame.u16ImageType == 0xFFFF && sReceivedFrame.u32NewFileVersion == 0xFFFFFFFF)
            {
                /* Send Next Image Req command for all OTA images (own image plus co-processor images) */
                bSendReqForAllImages = TRUE;
            }
            else if(sReceivedFrame.u16ImageType != 0xFFFF && sReceivedFrame.u32NewFileVersion != 0xFFFFFFFF)
            {
                /* check for match image type */
                for(u8LoopCount = 0; u8LoopCount < OTA_TOTAL_ACTIVE_IMAGES; u8LoopCount++)
                {
                    /* get the OTA header */
                    if(u8LoopCount == 0) /* Own Header */
                    {
                    #ifdef APP0 /* Building with selective OTA */
                        if(psOTA_Common->sOTACallBackMessage.sPersistedData.bStackDownloadActive)
                        {
                            sOTAHeader = sOtaGetHeader(APP_pu8GetApp1OtaHeader());
                        }
                        else
                    #endif
                    #ifdef SOTA_ENABLED
                        if (!sOtaInitBlobOtaHeader(psOTA_Common->sOTACallBackMessage.sPersistedData.blobId, &sOTAHeader))
                    #endif
                        {
                            sOTAHeader = sOtaGetHeader(au8OtaHeader);
                        }

                    }
#if (OTA_MAX_CO_PROCESSOR_IMAGES != 0)
                    else
                    {
                        /* CoProcessor Headers */
                        sOTAHeader = sOtaGetHeader(&asCommonCoProcessorOTAHeader[u8LoopCount-1][0]);
                    }
#endif

                    if(sReceivedFrame.u16ImageType == sOTAHeader.u16ImageType)
                    {
                        uint8 u8RandomValue = (uint8)RND_u32GetRand(0,sReceivedFrame.u8QueryJitter);
                        bool_t bIsUnicast = OTA_IS_UNICAST(psOTA_Common->sReceiveEventAddress.u8DstAddrMode,psOTA_Common->sReceiveEventAddress.uDstAddress.u16Addr);

                        /* Send Query Image Request command */
                        if( bIsUnicast || (!bIsUnicast && u8RandomValue <= sReceivedFrame.u8QueryJitter) )
                        {
#ifdef OTA_CLD_HARDWARE_VERSIONS_PRESENT
                            sQueryImage.u16HardwareVersion = sOTAHeader.u16MinimumHwVersion;
#endif
                            sQueryImage.u16ImageType = sOTAHeader.u16ImageType;
                            sQueryImage.u16ManufacturerCode = sOTAHeader.u16ManufacturerCode;
                            sQueryImage.u32CurrentFileVersion = sOTAHeader.u32FileVersion;
#ifdef OTA_CLD_HARDWARE_VERSIONS_PRESENT
                            sQueryImage.u8FieldControl = 1;
#else
                            sQueryImage.u8FieldControl = 0;
#endif
                            eOTA_ClientQueryNextImageRequest(u8SrcEndpoint,u8DstEndpoint,psZCL_Address,&sQueryImage);
                            bPollRequired = TRUE;
                        }
                        break;
                    }
                }
            }
        }
    }

    /* check we need to send next image req for images */
    if(bSendReqForAllImages)
    {
        uint8 u8NumOfImages = OTA_TOTAL_ACTIVE_IMAGES;
#if (OTA_MAX_CO_PROCESSOR_IMAGES != 0)
        if(bIsCoProcessorImgUpgdDependent)
        {
            u8NumOfImages = 1;
        }
#endif
        for(u8LoopCount = 0; u8LoopCount < u8NumOfImages; u8LoopCount++)
        {
            uint8 u8RandomValue = (uint8)RND_u32GetRand(0,sReceivedFrame.u8QueryJitter);
            bool_t bIsUnicast = OTA_IS_UNICAST(psOTA_Common->sReceiveEventAddress.u8DstAddrMode,psOTA_Common->sReceiveEventAddress.uDstAddress.u16Addr);

            /* get the OTA header */
            if(u8LoopCount == 0) /* Own Header */
            {
            #ifdef APP0 /* Building with selective OTA */
                if(psOTA_Common->sOTACallBackMessage.sPersistedData.bStackDownloadActive)
                {
                    sOTAHeader = sOtaGetHeader(APP_pu8GetApp1OtaHeader());
                }
                else
            #endif
            #ifdef SOTA_ENABLED
                if (!sOtaInitBlobOtaHeader(psOTA_Common->sOTACallBackMessage.sPersistedData.blobId, &sOTAHeader))
            #endif
                {
                     sOTAHeader = sOtaGetHeader(au8OtaHeader);
                }

            }
#if (OTA_MAX_CO_PROCESSOR_IMAGES != 0)
            else
            {
                /* CoProcessor Headers */
                sOTAHeader = sOtaGetHeader(&asCommonCoProcessorOTAHeader[u8LoopCount-1][0]);
            }
#endif

            /* Send Query Image Request command */
            if( bIsUnicast || (!bIsUnicast && u8RandomValue <= sReceivedFrame.u8QueryJitter) )
            {
#ifdef OTA_CLD_HARDWARE_VERSIONS_PRESENT
                sQueryImage.u16HardwareVersion = sOTAHeader.u16MinimumHwVersion;
#endif
                sQueryImage.u16ImageType = sOTAHeader.u16ImageType;
                sQueryImage.u16ManufacturerCode = sOTAHeader.u16ManufacturerCode;
                sQueryImage.u32CurrentFileVersion = sOTAHeader.u32FileVersion;
#ifdef OTA_CLD_HARDWARE_VERSIONS_PRESENT
                sQueryImage.u8FieldControl = 1;
#else
                sQueryImage.u8FieldControl = 0;
#endif
                eOTA_ClientQueryNextImageRequest(u8SrcEndpoint,u8DstEndpoint,psZCL_Address,&sQueryImage);
                bPollRequired = TRUE;
            }
        }
    }
    return bPollRequired;
}

/****************************************************************************
 **
 ** NAME:       bOtaHandleImageQueryResponse
 **
 ** DESCRIPTION:
 ** Handle Image Query Response
 ** PARAMETERS:                           Name                           Usage
 ** tsZCL_EndPointDefinition           *psEndPointDefinition           Endpoint definition
 ** tsOTA_Common                       *psOTA_Common                   OTA custom data
 **
 ** RETURN: True if a poll will be reqired
 ** None
 ****************************************************************************/
PRIVATE bool_t bOtaHandleImageQueryResponse( tsOTA_Common *psOTA_Common,
                                             tsZCL_EndPointDefinition    *psEndPointDefinition,
                                             tsZCL_Address *psZCL_Address,
                                             uint8        u8SrcEndpoint,
                                             uint8       u8DstEndpoint)
{
    tsOTA_ImageHeader sOTAHeader;
    bool_t bPollRequired = FALSE;
    uint8 u8LoopCount;
    tsOTA_QueryImageResponse sImageResponse;
    uint32 u32MaxImage =
           sNvmDefsStruct.u32SectorSize * psOTA_Common->sOTACallBackMessage.u8MaxNumberOfSectors * OTA_SECTOR_CONVERTION;

    teZCL_Status   eQueryNextImgRspErrStatus = OTA_STATUS_SUCCESS;

#if (OTA_MAX_CO_PROCESSOR_IMAGES != 0)
    /* Initiliaze No.of downloadable images */
    if(!psOTA_Common->sOTACallBackMessage.sPersistedData.u8NumOfDownloadableImages)
          psOTA_Common->sOTACallBackMessage.sPersistedData.u8NumOfDownloadableImages = (OTA_MAX_CO_PROCESSOR_IMAGES+1);
#endif
    sImageResponse = psOTA_Common->sOTACallBackMessage.uMessage.sQueryImageResponsePayload;

    /* check response status */
    if(sImageResponse.u8Status == OTA_STATUS_SUCCESS)
    {
        /* we should know for which image (own or co-processor image) we are sending block request */
        for(u8LoopCount = 0; u8LoopCount < OTA_TOTAL_ACTIVE_IMAGES; u8LoopCount++)
        {
            /* get the OTA header */
            if(u8LoopCount == 0) /* Own Header */
            {
    #ifdef APP0 /* Building with selective OTA */
             if(psOTA_Common->sOTACallBackMessage.sPersistedData.bStackDownloadActive)
             {
                 sOTAHeader = sOtaGetHeader(APP_pu8GetApp1OtaHeader());
             }
             else
    #endif
    #ifdef SOTA_ENABLED
            if (!sOtaInitBlobOtaHeader(psOTA_Common->sOTACallBackMessage.sPersistedData.blobId, &sOTAHeader))
    #endif
             {
                 sOTAHeader = sOtaGetHeader(&au8OtaHeader[0]);
             }

            }
#if (OTA_MAX_CO_PROCESSOR_IMAGES != 0)
            else
            {
                /* CoProcessor Headers */
                sOTAHeader = sOtaGetHeader(&asCommonCoProcessorOTAHeader[u8LoopCount-1][0]);
            }
#endif

            if(sImageResponse.u16ManufacturerCode == sOTAHeader.u16ManufacturerCode &&
                    sImageResponse.u16ImageType == sOTAHeader.u16ImageType)
            {
                /* SE1.1.1 approved test spec, client has ability to download lower version of image */
                psOTA_Common->sOTACallBackMessage.uMessage.sImageVersionVerify.eImageVersionVerifyStatus = E_ZCL_SUCCESS;
                psOTA_Common->sOTACallBackMessage.uMessage.sImageVersionVerify.u32CurrentImageVersion = sOTAHeader.u32FileVersion;
                psOTA_Common->sOTACallBackMessage.uMessage.sImageVersionVerify.u32NotifiedImageVersion = sImageResponse.u32FileVersion;
                eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_VERIFY_IMAGE_VERSION,psEndPointDefinition);

                if(psOTA_Common->sOTACallBackMessage.uMessage.sImageVersionVerify.eImageVersionVerifyStatus == E_ZCL_SUCCESS)
                {

#ifdef SOTA_ENABLED
                    uint32_t u32BlobSectionSize = 0;
                    bool_t startImgResult = FALSE;
                    if (Sota_GetBlobSectionSize(sOTAHeader.u16ImageType, &u32BlobSectionSize))
                    {
                        startImgResult = (OTA_StartImageWithMaxSize((sImageResponse.u32ImageSize - OTA_MIN_HEADER_SIZE - OTA_TAG_HEADER_SIZE ), u32BlobSectionSize) == gOtaSuccess_c);
                    }
                    else
                    {
                        startImgResult = (OTA_StartImage((sImageResponse.u32ImageSize - OTA_MIN_HEADER_SIZE - OTA_TAG_HEADER_SIZE )) == gOtaSuccess_c);
                    }

                    if ((startImgResult && (u32MaxImage >= sImageResponse.u32ImageSize))||(u8LoopCount != 0))
#else
                    if( (
#if (defined KSDK2)
#ifndef APP0
                            gOtaSuccess_c == OTA_StartImage((sImageResponse.u32ImageSize - OTA_MIN_HEADER_SIZE - OTA_TAG_HEADER_SIZE )) &&
#endif
#endif
                            (u32MaxImage >= sImageResponse.u32ImageSize))||(u8LoopCount != 0))
#endif /*SOTA_ENABLED */
                    {
                        //Optional to send our extended address so not sending
                        tsOTA_BlockRequest sBlock;
                        sBlock.u8FieldControl = 0;
                        sBlock.u16ManufactureCode = sImageResponse.u16ManufacturerCode;
                        sBlock.u16ImageType = sImageResponse.u16ImageType;
                        sBlock.u32FileOffset = 0;
                        sBlock.u8MaxDataSize = OTA_MAX_BLOCK_SIZE;
                        sBlock.u32FileVersion = sImageResponse.u32FileVersion;
                        psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32DownloadedFileVersion = sImageResponse.u32FileVersion;
                        vOtaRequestNextBlock(u8SrcEndpoint,u8DstEndpoint,&sBlock, psEndPointDefinition,psOTA_Common, psZCL_Address);
                        psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset = 0;

                        /* Check whether received image null or not */
                        if( ((sImageResponse.u32ImageSize >= (OTA_MIN_HEADER_SIZE + OTA_TAG_HEADER_SIZE + 110))&&
                                (sImageResponse.u32ImageSize <= (OTA_MAX_HEADER_SIZE + OTA_TAG_HEADER_SIZE +110))) ||
                             ((sImageResponse.u32ImageSize >= (OTA_MIN_HEADER_SIZE + OTA_TAG_HEADER_SIZE))&&
                                (sImageResponse.u32ImageSize <= (OTA_MAX_HEADER_SIZE + OTA_TAG_HEADER_SIZE))) )
                        {
                            psOTA_Common->sOTACallBackMessage.sPersistedData.bIsNullImage = TRUE;
                        }

                        bPollRequired = TRUE;
                        psOTA_Common->sOTACallBackMessage.sPersistedData.u8Retry++;
#ifndef OTA_TIME_INTERVAL_BETWEEN_REQUESTS
                        psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime()+OTA_TIME_INTERVAL_BETWEEN_RETRIES+1;
#else
                        psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime()+OTA_TIME_INTERVAL_BETWEEN_REQUESTS+1;
#endif
                        DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 13 ..\n");
                        break;
                    }
                    else
                    {
                        eQueryNextImgRspErrStatus = E_ZCL_ERR_INVALID_IMAGE_SIZE;
                    }
                }
                else
                {
                    eQueryNextImgRspErrStatus = E_ZCL_ERR_INVALID_IMAGE_VERSION;
                }
                if(OTA_STATUS_SUCCESS != eQueryNextImgRspErrStatus)
                {
                    psOTA_Common->sOTACallBackMessage.eEventId = E_CLD_OTA_COMMAND_QUERY_NEXT_IMAGE_RESPONSE_ERROR;
                    psOTA_Common->sOTACallBackMessage.uMessage.eQueryNextImgRspErrStatus = eQueryNextImgRspErrStatus;
                    psEndPointDefinition->pCallBackFunctions(&psOTA_Common->sOTACustomCallBackEvent);
                    eQueryNextImgRspErrStatus = OTA_STATUS_SUCCESS;
                }
            }
        }

#if OTA_MAX_CO_PROCESSOR_IMAGES != 0
        /* if loop count is zero means, we are requesting for own image */
        if((u8LoopCount) && (OTA_TOTAL_ACTIVE_IMAGES > u8LoopCount))
        {
            psOTA_Common->sOTACallBackMessage.sPersistedData.bIsCoProcessorImage = TRUE;
            psOTA_Common->sOTACallBackMessage.sPersistedData.u32CoProcessorImageSize = sImageResponse.u32ImageSize;
            psOTA_Common->sOTACallBackMessage.sPersistedData.u8CoProcessorOTAHeaderIndex = u8LoopCount-1;
            /* change the state to download in progress */
            vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_DL_IN_PROGRESS,psEndPointDefinition,psOTA_Common);
        }
        else
        {
            psOTA_Common->sOTACallBackMessage.sPersistedData.bIsCoProcessorImage = FALSE;
        }
#else
        psOTA_Common->sOTACallBackMessage.sPersistedData.bIsCoProcessorImage = FALSE;
#endif
    }
    return bPollRequired;
}

/****************************************************************************
 **
 ** NAME:       bOtaHandleBlockResponseIdle
 **
 ** DESCRIPTION:
 ** Handle Block Response from idle state (reveive the header)
 ** PARAMETERS:                           Name                           Usage
 ** tsZCL_EndPointDefinition           *psEndPointDefinition           Endpoint definition
 ** tsOTA_Common                       *psOTA_Common                   OTA custom data
 **
 ** RETURN: True if a poll will be reqired
 ** None
 ****************************************************************************/
PRIVATE bool_t bOtaHandleBlockResponseIdle( tsOTA_Common *psOTA_Common,
                                             tsZCL_EndPointDefinition    *psEndPointDefinition,
                                    tsZCL_Address *psZCL_Address,
                                    uint8        u8SrcEndpoint,
                                    uint8       u8DstEndpoint)
{
    tsOTA_ImageHeader sOTAHeader;
    bool_t bPollRequired = FALSE;
    bool_t bProceed = FALSE;
    uint32 u32MinOverFlow;
    uint32 u32FileOverflow;
    uint32 u32FieldId;
    uint32 u32TagLength;
    uint32 u32TagIdOverflow;
    uint32 u32DataOverflow;
    uint32 i;
    uint32 u32TotalImage = 0;
    uint32 u32TempChkLength;
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x) || (defined APP0)
    uint8 u8NextImageLocation = 0;
#endif
    uint16 u16FieldLength = 0, u16TagId;
    tsOTA_ImageBlockResponsePayload sBlockResponse;

    #ifdef APP0 /* Building with selective OTA */
        if(psOTA_Common->sOTACallBackMessage.sPersistedData.bStackDownloadActive)
        {
            sOTAHeader = sOtaGetHeader(APP_pu8GetApp1OtaHeader());
        }
        else
    #endif
    #ifdef SOTA_ENABLED
        if (!sOtaInitBlobOtaHeader(psOTA_Common->sOTACallBackMessage.sPersistedData.blobId, &sOTAHeader))
    #endif
        {
             sOTAHeader = sOtaGetHeader(au8OtaHeader);
        }
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x) || (defined APP0)
    u8NextImageLocation = psOTA_Common->sOTACallBackMessage.u8NextFreeImageLocation;
#endif
    sBlockResponse = psOTA_Common->sOTACallBackMessage.uMessage.sImageBlockResponsePayload;
#ifdef OTA_RESET_RETRY_UNCONDITIONAL
    psOTA_Common->sOTACallBackMessage.sPersistedData.u8Retry = 0;
#endif
    DBG_vPrintf(TRACE_OTA_DEBUG, "Got Block Idle ..\n");

    /* check if status is Abort */
    if(sBlockResponse.u8Status == OTA_STATUS_ABORT)
    {
        /* Stop OTA Download and inform user, DOWN LOAD aborted */
        eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_OTA_DL_ABORTED,psEndPointDefinition);
        vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_NORMAL, psEndPointDefinition, psOTA_Common);
        eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_SAVE_CONTEXT,psEndPointDefinition);
        return bPollRequired;
    }

    if(sBlockResponse.u8Status == OTA_STATUS_WAIT_FOR_DATA)
    {
        psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime =
            u32ZCL_GetUTCTime() + (sBlockResponse.uMessage.sWaitForData.u32RequestTime - sBlockResponse.uMessage.sWaitForData.u32CurrentTime);
    }


    eOTA_RestoreClientData(psEndPointDefinition->u8EndPointNumber,
                           &psOTA_Common->sOTACallBackMessage.sPersistedData,
                           FALSE);

    DBG_vPrintf(TRACE_BLOCKS, "BLOCK idle -> Status %02x Block File Offset %08x Persist File offset %08x\n",
            sBlockResponse.u8Status,
            sBlockResponse.uMessage.sBlockPayloadSuccess.u32FileOffset,
            psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset);
    DBG_vPrintf(TRACE_BLOCKS, "BLOCK idle -> Flash Offset %08x\n",
            psOTA_Common->sOTACallBackMessage.sPersistedData.u32CurrentFlashOffset);

    if ((sBlockResponse.u8Status == OTA_STATUS_SUCCESS) &&
       (sBlockResponse.uMessage.sBlockPayloadSuccess.u32FileOffset == psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset)
#ifdef OTA_CHECK_TRANSACTION_SEQ_NUM
       &&((psOTA_Common->sOTACustomCallBackEvent.u8TransactionSequenceNumber == psOTA_Common->sOTACallBackMessage.sPersistedData.u8RequestTransSeqNo)
#ifdef OTA_PAGE_REQUEST_SUPPORT
       ||(psOTA_Common->sOTACallBackMessage.sPersistedData.sPageReqParams.bPageReqOn)
#endif
       )
#endif
       )
    {
        /* rcvd block status success and
         * rcvd file off equal to persisted file offset
         */
        psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = 0;
        bPollRequired = TRUE;
        DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 14 ..\n");

        if((sBlockResponse.uMessage.sBlockPayloadSuccess.u32FileOffset +
            sBlockResponse.uMessage.sBlockPayloadSuccess.u8DataSize) > OTA_MIN_HEADER_SIZE)
        {
            /* rvcd file offset plus rcvd data length greater than OTA_MIN_HEADER_SIZE */
            DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 15 ..\n");
            if(sBlockResponse.uMessage.sBlockPayloadSuccess.u32FileOffset <= OTA_MIN_HEADER_SIZE)
            {
                u32MinOverFlow = (sBlockResponse.uMessage.sBlockPayloadSuccess.u32FileOffset +
                                    sBlockResponse.uMessage.sBlockPayloadSuccess.u8DataSize) -
                                    OTA_MIN_HEADER_SIZE;

                u32MinOverFlow = sBlockResponse.uMessage.sBlockPayloadSuccess.u8DataSize -
                            u32MinOverFlow;
                for(i=0;i < u32MinOverFlow;i++)
                {
                    psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header[sBlockResponse.uMessage.sBlockPayloadSuccess.u32FileOffset + i] =
                           sBlockResponse.uMessage.sBlockPayloadSuccess.pu8Data[i];

                }
                psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset += u32MinOverFlow;
                DBG_vPrintf(TRACE_INT_FLASH, "OFFSET1 -> %08x added %d\n", psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset, u32MinOverFlow);
            }
            else
            {
                u32MinOverFlow = 0;
            }

            vReverseMemcpy( (uint8*)&u32FieldId, &psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header[0], sizeof(uint32));
            vReverseMemcpy( (uint8*)&u16FieldLength, &psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header[6], sizeof(uint16));

            if(  (u32FieldId == OTA_FILE_IDENTIFIER)  &&
                 (u16FieldLength <= OTA_MAX_HEADER_SIZE)  )
            {
                /* field Id is equal to OTA_FILE_IDENTIFIER and
                 * field length less than or equal to OTA_MAX_HEADER_SIZE
                 */
                if((sBlockResponse.uMessage.sBlockPayloadSuccess.u32FileOffset +
                    sBlockResponse.uMessage.sBlockPayloadSuccess.u8DataSize) > u16FieldLength)
                {
                    /* Rcvd file off set plus rcvd data size greater than field length */
                    if(sBlockResponse.uMessage.sBlockPayloadSuccess.u32FileOffset <= u16FieldLength)
                    {
                        DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 17 ..\n");
                        u32FileOverflow = (sBlockResponse.uMessage.sBlockPayloadSuccess.u32FileOffset +
                                            sBlockResponse.uMessage.sBlockPayloadSuccess.u8DataSize) -
                                            u16FieldLength;

                        u32FileOverflow = sBlockResponse.uMessage.sBlockPayloadSuccess.u8DataSize -
                                        u32FileOverflow ;
                        for(i = 0;i < u32FileOverflow;i++)
                        {
                            psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header[sBlockResponse.uMessage.sBlockPayloadSuccess.u32FileOffset + i] =
                                                                                         sBlockResponse.uMessage.sBlockPayloadSuccess.pu8Data[i];

                        }
                        psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset += u32FileOverflow - u32MinOverFlow;
                        DBG_vPrintf(TRACE_INT_FLASH, "OFFSET2 -> %08x, %d minus %d added %d\n",
                                psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset,
                                u32FileOverflow,
                                u32MinOverFlow,
                                (u32FileOverflow - u32MinOverFlow));
                    }
                    else
                    {
                        u32FileOverflow = 0;
                    }
                    if(psOTA_Common->sOTACallBackMessage.sPersistedData.bIsNullImage)
                    {
                        u32TempChkLength = (uint32)(u16FieldLength + OTA_TAG_HEADER_SIZE - 1);
                    }
                    else
                    {
                        u32TempChkLength = (uint32)(u16FieldLength + OTA_TAG_HEADER_SIZE);
                    }
                    if((sBlockResponse.uMessage.sBlockPayloadSuccess.u32FileOffset +
                         sBlockResponse.uMessage.sBlockPayloadSuccess.u8DataSize) > u32TempChkLength)
                    {
                        /* Rcvd file off plus rcvd data size greater than Temp check lenght */
                        if(sBlockResponse.uMessage.sBlockPayloadSuccess.u32FileOffset <= (uint32)(u16FieldLength+OTA_TAG_HEADER_SIZE))
                        {
                            DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 18 ..\n");
                            u32TagIdOverflow = (sBlockResponse.uMessage.sBlockPayloadSuccess.u32FileOffset +
                                sBlockResponse.uMessage.sBlockPayloadSuccess.u8DataSize)-u16FieldLength - OTA_TAG_HEADER_SIZE;
                            u32TagIdOverflow = sBlockResponse.uMessage.sBlockPayloadSuccess.u8DataSize -
                                u32TagIdOverflow - u32FileOverflow;
                            for(i = 0;i < u32TagIdOverflow;i++)
                            {
                                psOTA_Common->sOTACallBackMessage.sPersistedData.u8PassiveTag[
                                         (psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset - u16FieldLength) + i] =
                                          sBlockResponse.uMessage.sBlockPayloadSuccess.pu8Data[u32FileOverflow+i];

                            }
                            psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset += u32TagIdOverflow;
                            DBG_vPrintf(TRACE_INT_FLASH, "OFFSET3 -> %08x, added %d\n",
                                    psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset,
                                                                    u32TagIdOverflow);
                        }
                        else
                        {
                            u32TagIdOverflow = 0;
                        }
#ifdef OTA_STRING_COMPARE
                        /*OTA Header is downloaded now compare string */
                        DBG_vPrintf(TRACE_OTA_DEBUG,"\nStarting Image Identification\n");
                        if ( FALSE == bOtaVerifyString( psOTA_Common,
                                                       psEndPointDefinition,
                                                       (uint8*)&(sOTAHeader.stHeaderString) )
                           )
                        {
                            return FALSE;
                        }

                        /*String compare Done !!*/
#endif




                        memcpy(psOTA_Common->sOTACallBackMessage.sPersistedData.u8ActiveTag,
                            psOTA_Common->sOTACallBackMessage.sPersistedData.u8PassiveTag, OTA_TAG_HEADER_SIZE);

                        vOTA_GetTagIdandLengh(&u16TagId,&u32TagLength,&psOTA_Common->sOTACallBackMessage.sPersistedData.u8ActiveTag[0]);
                        vReverseMemcpy((uint8*)&u32TotalImage,&psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header[52],sizeof(uint32));

                        // is impossible
                        if((u32TagLength+OTA_TAG_HEADER_SIZE+u16FieldLength) >
                            u32TotalImage)
                        {
                            vOtaAbortDownload(psOTA_Common, psEndPointDefinition);
                            return FALSE;
                        }

                        if(u16TagId == OTA_TAG_ID_UPGRADE_IMAGE)
                        {
                            uint32 u32WriteValue=0;
                            int i;
                            for(i=0; i<OTA_MAX_IMAGES_PER_ENDPOINT; i++)
                            {
                                DBG_vPrintf(TRACE_INT_FLASH, "start sectors %d\n", psOTA_Common->sOTACallBackMessage.u8ImageStartSector[i]);
                            }
                            uint32 u32StartLocation = 0;
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x) || (defined APP0)
#ifdef APP0 /* Building with selective OTA */
                            if(psOTA_Common->sOTACallBackMessage.sPersistedData.bStackDownloadActive)
                            {
                                u32StartLocation = OTA_APP1_SHADOW_FLASH_OFFSET;
                            }
                            else
#endif
                            {
                                u32StartLocation = psOTA_Common->sOTACallBackMessage.u8ImageStartSector[u8NextImageLocation]
                                                                                                           * sNvmDefsStruct.u32SectorSize * OTA_SECTOR_CONVERTION;
                            }
#endif

                            DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 19 ..\n");
                            u32DataOverflow = (sBlockResponse.uMessage.sBlockPayloadSuccess.u32FileOffset +
                                                 sBlockResponse.uMessage.sBlockPayloadSuccess.u8DataSize) -
                                                 (u16FieldLength + OTA_TAG_HEADER_SIZE);
                            DBG_vPrintf(TRACE_INT_FLASH, "WRITE1 -> u32FileOffset %d\n", sBlockResponse.uMessage.sBlockPayloadSuccess.u32FileOffset);
                            DBG_vPrintf(TRACE_INT_FLASH, "WRITE1 -> u8DataSize %d\n", sBlockResponse.uMessage.sBlockPayloadSuccess.u8DataSize);
                            DBG_vPrintf(TRACE_INT_FLASH, "WRITE1 -> u16FieldLength %d\n", u16FieldLength);
                            DBG_vPrintf(TRACE_INT_FLASH, "WRITE1 -> OTA_TAG_HEADER_SIZE %d\n", OTA_TAG_HEADER_SIZE);
                            DBG_vPrintf(TRACE_INT_FLASH, "WRITE1 -> u32DataOverflow %d\n", u32DataOverflow);
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x) || (defined APP0)
#ifdef APP0 /* Building with selective OTA */
                            if(psOTA_Common->sOTACallBackMessage.sPersistedData.bStackDownloadActive)
                            {
                                uint32 u32PageLocation = OTA_APP1_SHADOW_FLASH_OFFSET/NVM_BYTES_PER_SEGMENT;
                                for(i=0; i< (OTA_APP1_MAX_SIZE/NVM_BYTES_PER_SEGMENT); i++)
                                {
                                    DBG_vPrintf(TRACE_OTA_DEBUG, "Erasing flash location start %d, shadow bank %d \n",
                                        u32PageLocation+i,u32PageLocation );
                                    bAHI_FlashErasePage( (u32PageLocation + i) );
                                }
                            }
                            else
#endif
                            {
                                for(i=0; i< psOTA_Common->sOTACallBackMessage.u8MaxNumberOfSectors; i++)
                                {
                                    DBG_vPrintf(TRACE_OTA_DEBUG, "Erasing flash location start %d, shadow bank %d \n",
                                             psOTA_Common->sOTACallBackMessage.u8ImageStartSector[u8NextImageLocation]+i,u8NextImageLocation );
                                    vOtaFlashLockErase(psEndPointDefinition, psOTA_Common, (uint8)(psOTA_Common->sOTACallBackMessage.u8ImageStartSector[
                                                                                                              u8NextImageLocation] + i ) );
                                }
                            }

#if APP0
                            g_bOtaFirstImagePage = TRUE;
#endif
#endif
                            if( !psOTA_Common->sOTACallBackMessage.sPersistedData.bIsNullImage)
                            {  /* if image is for own device copy from  OTA_FLS_MAGIC_NUMBER_LENGTH onwards */
                                if(u32DataOverflow > (OTA_FLS_MAGIC_NUMBER_LENGTH + OTA_REMOVE_BOOTLOADER_SPECIFIC_BYTES_LENGTH) )
                                {
#if !(defined OTA_INTERNAL_STORAGE) && ((defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x))
                                    u32WriteValue = u32DataOverflow - OTA_FLS_MAGIC_NUMBER_LENGTH;

                                    vOtaFlashLockWrite(psEndPointDefinition,
                                                        psOTA_Common,
                                                        u32StartLocation + OTA_FLS_MAGIC_NUMBER_LENGTH,
                                                         (uint16)u32WriteValue,
                                                         (sBlockResponse.uMessage.sBlockPayloadSuccess.pu8Data + ( u32TagIdOverflow + u32FileOverflow +
                                                                                                                 OTA_FLS_MAGIC_NUMBER_LENGTH ) ));
#else
                                    // internal
                                    uint32 u32Temp;
                                    u32WriteValue = u32DataOverflow - OTA_FLS_MAGIC_NUMBER_LENGTH - OTA_REMOVE_BOOTLOADER_SPECIFIC_BYTES_LENGTH;
                                    DBG_vPrintf(TRACE_INT_FLASH, "TO write is %d\n", u32WriteValue);
                                    u32Temp = (u32WriteValue % 16);
                                    u32WriteValue -= u32Temp;
                                    u32DataOverflow -= u32Temp;
                                    DBG_vPrintf(TRACE_INT_FLASH, "To Adjusted write is %d   Left Over %d\n", u32WriteValue, u32Temp);


#ifdef INTERNAL_ENCRYPTED

                                    vOtaProcessInternalEncryption(
                                            sBlockResponse.uMessage.sBlockPayloadSuccess.pu8Data + (u32TagIdOverflow + u32FileOverflow +
                                                    OTA_FLS_MAGIC_NUMBER_LENGTH + OTA_REMOVE_BOOTLOADER_SPECIFIC_BYTES_LENGTH ),
                                            u32StartLocation + OTA_FLS_MAGIC_NUMBER_LENGTH + OTA_REMOVE_BOOTLOADER_SPECIFIC_BYTES_LENGTH,
                                            u32WriteValue,
                                            u32StartLocation );

#endif  // end of #ifdef INTERNAL_ENCRYPTED


                                    DBG_vPrintf(TRACE_INT_FLASH, "WRITE1 -> start %08x, Magic %08x Length %d u32TagIdOverflow %d u32FileOverflow %d OTA_FLS_MAGIC_NUMBER_LENGTH %d \n",
                                            u32StartLocation,
                                            OTA_FLS_MAGIC_NUMBER_LENGTH,
                                            u32WriteValue,
                                            u32TagIdOverflow,
                                            u32FileOverflow,
                                            OTA_FLS_MAGIC_NUMBER_LENGTH);
                                    if ( u32WriteValue > 0)
                                    {
                                        vOtaFlashLockWrite(psEndPointDefinition,
                                                        psOTA_Common,
                                                        u32StartLocation + OTA_FLS_MAGIC_NUMBER_LENGTH + OTA_REMOVE_BOOTLOADER_SPECIFIC_BYTES_LENGTH,
                                                        (uint16)u32WriteValue,
                                                        (sBlockResponse.uMessage.sBlockPayloadSuccess.pu8Data + (u32TagIdOverflow + u32FileOverflow +
                                                                OTA_FLS_MAGIC_NUMBER_LENGTH + OTA_REMOVE_BOOTLOADER_SPECIFIC_BYTES_LENGTH )));
                                    }
                                #if ((defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x))
                                    psOTA_Common->sOTACallBackMessage.sPersistedData.u8Buf[0] =   *(sBlockResponse.uMessage.sBlockPayloadSuccess.pu8Data +
                                                                                                                     (u32TagIdOverflow + u32FileOverflow +
                                                                                                                             OTA_FLS_MAGIC_NUMBER_LENGTH)  );
                                    psOTA_Common->sOTACallBackMessage.sPersistedData.u8Buf[1] = *(sBlockResponse.uMessage.sBlockPayloadSuccess.pu8Data +
                                                                                                                   (u32TagIdOverflow+u32FileOverflow   +
                                                                                                                           OTA_FLS_MAGIC_NUMBER_LENGTH + 1 ) );
                                    psOTA_Common->sOTACallBackMessage.sPersistedData.u8Buf[2] = *(sBlockResponse.uMessage.sBlockPayloadSuccess.pu8Data +
                                                                                                                   (u32TagIdOverflow + u32FileOverflow +
                                                                                                                           OTA_FLS_MAGIC_NUMBER_LENGTH + 2) );
                                    psOTA_Common->sOTACallBackMessage.sPersistedData.u8Buf[3] = *(sBlockResponse.uMessage.sBlockPayloadSuccess.pu8Data +
                                                                                                                  ( u32TagIdOverflow + u32FileOverflow +
                                                                                                                           OTA_FLS_MAGIC_NUMBER_LENGTH + 3) );      
                                #endif   
#endif
                                }
#if !(defined OTA_INTERNAL_STORAGE) && ((defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x))
                                psOTA_Common->sOTACallBackMessage.sPersistedData.u32CurrentFlashOffset = u32StartLocation + u32DataOverflow;
                                psOTA_Common->sOTACallBackMessage.sPersistedData.u32TagDataWritten = u32DataOverflow;

#else
                                psOTA_Common->sOTACallBackMessage.sPersistedData.u32CurrentFlashOffset = u32StartLocation + u32WriteValue +
                                                                                                                  OTA_FLS_MAGIC_NUMBER_LENGTH +
                                                                                                                  OTA_REMOVE_BOOTLOADER_SPECIFIC_BYTES_LENGTH;
                                psOTA_Common->sOTACallBackMessage.sPersistedData.u32TagDataWritten = u32DataOverflow;
                                DBG_vPrintf(TRACE_INT_FLASH, "start loc %08x DataOverflow %d\n", u32StartLocation, u32DataOverflow);

                                DBG_vPrintf(TRACE_INT_FLASH, "WRITE1 -> Set flash offset %08x, Tag Data written %d\n",
                                        psOTA_Common->sOTACallBackMessage.sPersistedData.u32CurrentFlashOffset,
                                        psOTA_Common->sOTACallBackMessage.sPersistedData.u32TagDataWritten);

                                DBG_vPrintf(TRACE_INT_FLASH, "Saving -> u32TagIdOverflow %d u32FileOverflow %d\n", u32TagIdOverflow, u32FileOverflow );
#endif
                            }
                            else
                            {
                                DBG_vPrintf(TRACE_INT_FLASH, "WRITE2 -> start %08x, u32DataOverflow %d <u32FileOverflow %d u32TagIdOverflow %d>\n",
                                                                                    u32StartLocation,
                                                                                    u32DataOverflow,
                                                                                    u32TagIdOverflow,
                                                                                    u32FileOverflow
                                                                                    );
                                vOtaFlashLockWrite(psEndPointDefinition, psOTA_Common,
                                                    u32StartLocation,
                                                    (uint16)u32DataOverflow,
                                                    (sBlockResponse.uMessage.sBlockPayloadSuccess.pu8Data + (u32TagIdOverflow + u32FileOverflow ) ));

                                psOTA_Common->sOTACallBackMessage.sPersistedData.u32CurrentFlashOffset = u32StartLocation + u32DataOverflow;
                                psOTA_Common->sOTACallBackMessage.sPersistedData.u32TagDataWritten = 0;
                            }
                            psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16DownloadedStackVersion =
                                psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header[19]<< 8 |
                                psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header[20];
                            psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset += u32DataOverflow;
                            DBG_vPrintf(TRACE_INT_FLASH, "OFFSET4 -> %08x, added %d\n",
                                    psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset,
                                    u32DataOverflow);
                            bProceed = TRUE;
                        }
                        else
                        {
                            //Image should always follow the header.
                            vOtaAbortDownload(psOTA_Common, psEndPointDefinition);
                            return FALSE;
                        }
                    }
                    else
                    {
                        /* Rcvd file off plus rcvd data size less or equal to Temp check lenght */
                        uint32 u32Iterations = sBlockResponse.uMessage.sBlockPayloadSuccess.u32FileOffset +
                            sBlockResponse.uMessage.sBlockPayloadSuccess.u8DataSize - u16FieldLength;
                        DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 21 ..\n");
                        for(i=0;i < u32Iterations;i++)
                        {
                            psOTA_Common->sOTACallBackMessage.sPersistedData.u8PassiveTag[i] =
                                    sBlockResponse.uMessage.sBlockPayloadSuccess.pu8Data[u32FileOverflow+i];
                        }
                        psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset += u32Iterations;
                        DBG_vPrintf(TRACE_INT_FLASH, "OFFSET5 -> %08x, added %d\n",
                                psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset,
                                                                    u32Iterations);
                    }
                }
                else
                {
                    /* Rcvd file off set plus rcvd data size less than or equal to field length */
                    DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 20 ..\n");
                    u32DataOverflow = (sBlockResponse.uMessage.sBlockPayloadSuccess.u32FileOffset +
                            sBlockResponse.uMessage.sBlockPayloadSuccess.u8DataSize) -
                            OTA_MIN_HEADER_SIZE;
                    DBG_vPrintf(TRACE_INT_FLASH, "WRITE12 -> u32DataOverflow %d\n", u32DataOverflow);
                    for(i=0;i < u32DataOverflow;i++)
                    {
                        psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header[psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset + i] =
                            sBlockResponse.uMessage.sBlockPayloadSuccess.pu8Data[u32MinOverFlow+i];
                    }
                    psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset += u32DataOverflow;
                    DBG_vPrintf(TRACE_INT_FLASH, "OFFSET6 -> %08x, added %d\n",
                            psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset,
                                        u32DataOverflow);
                }
            }
            else
            {
                /* field Id is equal to OTA_FILE_IDENTIFIER and
                 * field length greater than OTA_MAX_HEADER_SIZE
                 */
                DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 22 ..\n");
                vOtaAbortDownload(psOTA_Common, psEndPointDefinition);
                return FALSE;
            }
        }
        else
        {

            /* rvcd file offset plus rcvd data length less than or equal to OTA_MIN_HEADER_SIZE */
            DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 23 ..\n");
            for(i=0; i < sBlockResponse.uMessage.sBlockPayloadSuccess.u8DataSize; i++)
            {
                psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header[psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset + i] =
                        sBlockResponse.uMessage.sBlockPayloadSuccess.pu8Data[i];
            }
            psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset += sBlockResponse.uMessage.sBlockPayloadSuccess.u8DataSize;
            DBG_vPrintf(TRACE_INT_FLASH, "OFFSET7 -> %08x, added %d\n",
                    psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset,
                                                            sBlockResponse.uMessage.sBlockPayloadSuccess.u8DataSize);
        }

        psOTA_Common->sOTACallBackMessage.sPersistedData.u8Retry = 0;

        if ( (psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset < u32TotalImage) ||
             (psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset < (OTA_MIN_HEADER_SIZE + OTA_TAG_HEADER_SIZE)))
        {
            /* file offset less then total image size OR
             * file offset lesss than (OTA_MIN_HEADER_SIZE + OTA_TAG_HEADER_SIZE)
             */
#ifndef OTA_TIME_INTERVAL_BETWEEN_REQUESTS
#ifdef OTA_CLD_ATTR_REQUEST_DELAY
            if(psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16MinBlockRequestDelay > 0)
                psOTA_Common->sOTACallBackMessage.sPersistedData.bWaitForBlockReq = TRUE;
#endif
            vOtaHandleTimedBlockRequest(psOTA_Common, &bPollRequired,&sOTAHeader,psEndPointDefinition);
            psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime()+ 2*OTA_TIME_INTERVAL_BETWEEN_RETRIES;
#else
            psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime()+ 2*OTA_TIME_INTERVAL_BETWEEN_REQUESTS;
#endif
        }
    }/* end of rcvd block status success and rcvd file off equal to persisted file offset */
    else if((sBlockResponse.u8Status == OTA_STATUS_WAIT_FOR_DATA) &&
            (psOTA_Common->sOTACustomCallBackEvent.u8TransactionSequenceNumber == psOTA_Common->sOTACallBackMessage.sPersistedData.u8RequestTransSeqNo))
    {
        bPollRequired = TRUE;
        vOtaHandleWaitForDataStatus(psEndPointDefinition, psOTA_Common, &sBlockResponse);
    }
    else
    {
        DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 24 ..\n");
        // No more processing done because either invalid image or error status . do nothing.
        return FALSE;
    }

    if(bProceed)
    {
        if(!psOTA_Common->sOTACallBackMessage.sPersistedData.bIsNullImage)
        {
            vOtaClientUpgMgrMapStates( E_CLD_OTA_STATUS_DL_IN_PROGRESS, psEndPointDefinition, psOTA_Common);
        }
        else
        {
            /* Null Image */
            if( psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset < u32TotalImage)
            {
                vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_DL_IN_PROGRESS,psEndPointDefinition,psOTA_Common);
            }
            else
            {
                tsOTA_UpgradeEndRequestPayload sEndRequest;


                bPollRequired = FALSE;

#ifdef OTA_ACCEPT_ONLY_SIGNED_IMAGES
// Download Complete
                teZCL_Status eStatus;
                eStatus = eOTA_VerifyImage(psEndPointDefinition->u8EndPointNumber,
                                    FALSE,
                                    psOTA_Common->sOTACallBackMessage.u8NextFreeImageLocation,
                                    FALSE);
#endif /* OTA_ACCEPT_ONLY_SIGNED_IMAGES */

                vReverseMemcpy((uint8*)&sEndRequest.u16ImageType,&psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header[12],sizeof(uint16));
                vReverseMemcpy((uint8*)&sEndRequest.u16ManufacturerCode,&psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header[10],sizeof(uint16));
                vReverseMemcpy((uint8*)&sEndRequest.u32FileVersion,&psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header[14],sizeof(uint32));

#ifdef OTA_ACCEPT_ONLY_SIGNED_IMAGES
                /* check status */
                if(eStatus)
                {
                    sEndRequest.u8Status = OTA_STATUS_IMAGE_INVALID;
                }
                else
                {
                    sEndRequest.u8Status = E_ZCL_SUCCESS;
                }
#else
                sEndRequest.u8Status = E_ZCL_SUCCESS;
#endif
                /* change state to normal if status is success */
                if(sEndRequest.u8Status == E_ZCL_SUCCESS)
                {
#if (OTA_MAX_CO_PROCESSOR_IMAGES != 0)
                    /* Decrement downloadable images */
                    psOTA_Common->sOTACallBackMessage.sPersistedData.u8NumOfDownloadableImages--;

                    /* Check Any images are pending for downloading */
                    if((!psOTA_Common->sOTACallBackMessage.sPersistedData.u8NumOfDownloadableImages)||(!bIsCoProcessorImgUpgdDependent))
                    {
                       sEndRequest.u8Status = OTA_STATUS_SUCCESS;
                       /* change state to download complete */
                       vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_DL_COMPLETE, psEndPointDefinition, psOTA_Common);
                       /* if it is a success case then enable retries and wait for response */
                       psOTA_Common->sOTACallBackMessage.sPersistedData.u8Retry = 0;
                       psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime()+
                                                                                                     OTA_TIME_INTERVAL_BETWEEN_END_REQUEST_RETRIES + 1;
                    }
                    else
                    {
                        sEndRequest.u8Status = OTA_REQUIRE_MORE_IMAGE;
                        /* Change the state to Normal State */
                        vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_NORMAL,psEndPointDefinition,psOTA_Common);
                    }
#else
                    vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_DL_COMPLETE, psEndPointDefinition, psOTA_Common);
#endif
                }
#ifdef OTA_ACCEPT_ONLY_SIGNED_IMAGES
                else
                {
                    /* Move to normal state */
                    vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_NORMAL, psEndPointDefinition, psOTA_Common);

                    /* Give a call back to the user */
                    eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_OTA_DL_ABORTED,psEndPointDefinition);
                }
#endif

                /* Send block end request */
                eOTA_ClientUpgradeEndRequest(psOTA_Common->sReceiveEventAddress.u8DstEndpoint,
                                psOTA_Common->sReceiveEventAddress.u8SrcEndpoint,
                                psZCL_Address,
                                &sEndRequest);
#if (OTA_MAX_CO_PROCESSOR_IMAGES != 0)
                if(sEndRequest.u8Status == OTA_REQUIRE_MORE_IMAGE)
                {
                    /* Give a call back to the user for requesting remaining images */
                    eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_REQUEST_QUERY_NEXT_IMAGES,psEndPointDefinition);
                }
#endif
            }
        }
    }  /* end of if (bProceed) */

    return bPollRequired;
}

/****************************************************************************
 **
 ** NAME:       bOtaHandleQuerySpecificFileResponse
 **
 ** DESCRIPTION:
 ** Handle Query Specific File Response
 ** PARAMETERS:                           Name                           Usage
 ** tsZCL_EndPointDefinition           *psEndPointDefinition           Endpoint definition
 ** tsOTA_Common                       *psOTA_Common                   OTA custom data
 **
 ** RETURN: True if a poll will be reqired
 ** None
 ****************************************************************************/
PRIVATE bool_t bOtaHandleQuerySpecificFileResponse( tsOTA_Common *psOTA_Common,
                                             tsZCL_EndPointDefinition    *psEndPointDefinition,
                                    tsZCL_Address *psZCL_Address,
                                    uint8        u8SrcEndpoint,
                                    uint8       u8DstEndpoint)
{
    bool_t bPollRequired = FALSE;
    tsOTA_BlockRequest sBlockSp;
    tsOTA_QuerySpecificFileResponsePayload sQSpFileResponse;
    sQSpFileResponse = psOTA_Common->sOTACallBackMessage.uMessage.sQuerySpFileResponsePayload;

    DBG_vPrintf(TRACE_OTA_DEBUG,"In Event Handler for Query Specific File Response");
    if(sQSpFileResponse.u8Status == OTA_STATUS_SUCCESS)
    {
        sBlockSp.u8FieldControl = 1;
        sBlockSp.u16ManufactureCode = sQSpFileResponse.u16ManufacturerCode;
        sBlockSp.u16ImageType = sQSpFileResponse.u16ImageType;
        sBlockSp.u32FileOffset = 0;
        sBlockSp.u8MaxDataSize = OTA_MAX_BLOCK_SIZE;
        sBlockSp.u32FileVersion = sQSpFileResponse.u32FileVersion;
#ifndef OTA_UNIT_TEST_FRAMEWORK
        sBlockSp.u64RequestNodeAddress = ZPS_u64AplZdoGetIeeeAddr();
#endif
        psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32DownloadedFileVersion = sQSpFileResponse.u32FileVersion;
        psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16ImageType = sQSpFileResponse.u16ImageType;
        psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16ManfId = sQSpFileResponse.u16ManufacturerCode;
        psOTA_Common->sOTACallBackMessage.sPersistedData.bIsSpecificFile = TRUE;
        vOtaRequestNextBlock(u8SrcEndpoint,u8DstEndpoint,&sBlockSp, psEndPointDefinition,psOTA_Common, psZCL_Address);
        psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset = 0;
        bPollRequired = TRUE;

        psOTA_Common->sOTACallBackMessage.sPersistedData.u32SpecificFileSize = sQSpFileResponse.u32ImageSize;
        /* change the state to download in progress */
        vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_DL_IN_PROGRESS,psEndPointDefinition,psOTA_Common);
    }
    return bPollRequired;
}

/****************************************************************************
 **
 ** NAME:       bOtaHandleBlockResponseInProgress
 **
 ** DESCRIPTION:
 ** Handle Query Specific File Response
 ** PARAMETERS:                           Name                           Usage
 ** tsZCL_EndPointDefinition           *psEndPointDefinition           Endpoint definition
 ** tsOTA_Common                       *psOTA_Common                   OTA custom data
 **
 ** RETURN: True if a poll will be reqired
 ** None
 ****************************************************************************/
PRIVATE bool_t bOtaHandleBlockResponseInProgress( tsOTA_Common *psOTA_Common,
                                             tsZCL_EndPointDefinition    *psEndPointDefinition,
                                    tsZCL_Address *psZCL_Address,
                                    tsOTA_ImageHeader * psOTACurrentHeader)
{
    bool_t bPollRequired = FALSE;
    //bool_t bPKTProcessed = FALSE;
    //bool_t bRequestMore = TRUE;
    //uint32  u32CurrOverflow;
    //uint32  u32TotalSize;

    tsOTA_ImageBlockResponsePayload sResponse = psOTA_Common->sOTACallBackMessage.uMessage.sImageBlockResponsePayload;
#ifdef  OTA_RESET_RETRY_UNCONDITIONAL
    psOTA_Common->sOTACallBackMessage.sPersistedData.u8Retry = 0;
#endif


    /* check if status is Abort */
    if(sResponse.u8Status == OTA_STATUS_ABORT)
    {
        /* Stop OTA Download and inform user, DOWN LOAD ID aborted */
        eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_OTA_DL_ABORTED,psEndPointDefinition);
        vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_NORMAL, psEndPointDefinition, psOTA_Common);
        eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_SAVE_CONTEXT,psEndPointDefinition);
        return FALSE;
    }

    if(sResponse.u8Status == OTA_STATUS_WAIT_FOR_DATA)
    {
        psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime =
            u32ZCL_GetUTCTime() + (sResponse.uMessage.sWaitForData.u32RequestTime - sResponse.uMessage.sWaitForData.u32CurrentTime);
    }

    DBG_vPrintf(TRACE_BLOCKS, "\nBLOCK actv -> Status %02x Block File Offset %08x Persist File offset %08x\n",
                        sResponse.u8Status,
                        sResponse.uMessage.sBlockPayloadSuccess.u32FileOffset,
                        psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset);
    DBG_vPrintf(TRACE_BLOCKS, "BLOCK actv -> Flash Offset %08x\n",
                        psOTA_Common->sOTACallBackMessage.sPersistedData.u32CurrentFlashOffset);

    if ( ( sResponse.u8Status == OTA_STATUS_SUCCESS ) &&
        ( !psOTA_Common->sOTACallBackMessage.sPersistedData.bIsCoProcessorImage) &&
        ( !psOTA_Common->sOTACallBackMessage.sPersistedData.bIsSpecificFile) )
    {
        /* standard client image, no co processor not file specific */
        bPollRequired = bOtaHandleBlockResponseStandardImage( psOTA_Common,
                                              psEndPointDefinition,
                                              &sResponse,
                                              psOTACurrentHeader,
                                              psZCL_Address);

    }    /* end of standard client image, no co processor not file specific */
    else if ( (sResponse.u8Status == OTA_STATUS_SUCCESS ) &&
              (psOTA_Common->sOTACallBackMessage.sPersistedData.bIsCoProcessorImage) )
    {
        /* Co processor image */
        bPollRequired = bOtaHandleBlockResponseCoProcessorImage( psOTA_Common,
                                                                 psEndPointDefinition,
                                                                 &sResponse,
                                                                 psOTACurrentHeader,
                                                                 psZCL_Address);
    }   /* End of Co processor image */
    else if ( (sResponse.u8Status == OTA_STATUS_SUCCESS ) &&
              (psOTA_Common->sOTACallBackMessage.sPersistedData.bIsSpecificFile) )
    {
        /* file specificic image */
        bPollRequired = bOtaHandleBlockResponseFileSpecificImage( psOTA_Common,
                                                                  psEndPointDefinition,
                                                                  &sResponse,
                                                                  psOTACurrentHeader,
                                                                  psZCL_Address);
    }     /* End of file specifici image */
    else if(sResponse.u8Status == OTA_STATUS_WAIT_FOR_DATA)
    {
#ifdef OTA_CHECK_TRANSACTION_SEQ_NUM
        if(psOTA_Common->sOTACustomCallBackEvent.u8TransactionSequenceNumber == psOTA_Common->sOTACallBackMessage.sPersistedData.u8RequestTransSeqNo)
#endif
        {
            vOtaHandleWaitForDataStatus(psEndPointDefinition, psOTA_Common, &sResponse);
        }
    }
    else
    {
        DBG_vPrintf(TRACE_OTA_DEBUG, "r4\n");
        vOtaAbortDownload(psOTA_Common, psEndPointDefinition);
        if(psOTA_Common->sOTACallBackMessage.sPersistedData.bIsCoProcessorImage)
        {
            eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_CO_PROCESSOR_DL_ABORT,psEndPointDefinition);
        }
        else if(psOTA_Common->sOTACallBackMessage.sPersistedData.bIsSpecificFile)
        {
            eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_SPECIFIC_FILE_DL_ABORT,psEndPointDefinition);
        }
    }
    return bPollRequired;
}

#ifdef OTA_STRING_COMPARE
/****************************************************************************
 **
 ** NAME:       bOtaVerifyString
 **
 ** DESCRIPTION:
 ** Handle wait for data status
 ** PARAMETERS:                           Name                           Usage
 ** tsZCL_EndPointDefinition           *psEndPointDefinition           Endpoint definition
 ** tsOTA_Common                       *psOTA_Common                   OTA custom data
 **
 ** RETURN:
 ** True -> strings match
 ** False -> strings don't match
 ****************************************************************************/
PRIVATE bool_t bOtaVerifyString( tsOTA_Common *psOTA_Common,
                                             tsZCL_EndPointDefinition    *psEndPointDefinition,
                                             uint8 * pu8ExpectedStr)
{
    int i;
    psOTA_Common->sOTACallBackMessage.uMessage.sImageIdentification.eStatus = E_ZCL_SUCCESS;
    psOTA_Common->sOTACallBackMessage.uMessage.sImageIdentification.pu8Current = pu8ExpectedStr;
    psOTA_Common->sOTACallBackMessage.uMessage.sImageIdentification.puNotified =
            &(psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header[20]);

    for (i=0;i<32;i++)
    {
        if ( psOTA_Common->sOTACallBackMessage.uMessage.sImageIdentification.pu8Current[i] !=
             psOTA_Common->sOTACallBackMessage.uMessage.sImageIdentification.puNotified[i])
        {
            DBG_vPrintf(TRACE_OTA_DEBUG, "Mismatch\n\n\n");
            psOTA_Common->sOTACallBackMessage.uMessage.sImageIdentification.eStatus = E_ZCL_FAIL;
            break;
        }
    }

    eOtaSetEventTypeAndGiveCallBack( psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_VERIFY_STRING, psEndPointDefinition);


    if(psOTA_Common->sOTACallBackMessage.uMessage.sImageIdentification.eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_OTA_DEBUG, "String not OK ...Aborting Download\n\n\n");
        psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16ImageType =
                psOTA_Common->sOTACallBackMessage.uMessage.sImageBlockResponsePayload.uMessage.sBlockPayloadSuccess.u16ImageType;
        psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16ManfId =
                psOTA_Common->sOTACallBackMessage.uMessage.sImageBlockResponsePayload.uMessage.sBlockPayloadSuccess.u16ManufacturerCode;
        vOtaAbortDownload(psOTA_Common, psEndPointDefinition);
        return FALSE;
    }
    return TRUE;
}
#endif

/****************************************************************************
 **
 ** NAME:       bOtaHandleBlockResponseStandardImage
 **
 ** DESCRIPTION:
 ** Handle wait for data status
 ** PARAMETERS:                           Name                           Usage
 ** tsZCL_EndPointDefinition           *psEndPointDefinition           Endpoint definition
 ** tsOTA_Common                       *psOTA_Common                   OTA custom data
 **
 ** RETURN:
 ** True -> strings match
 ** False -> strings don't match
 ****************************************************************************/
PRIVATE bool_t bOtaHandleBlockResponseStandardImage( tsOTA_Common *psOTA_Common,
                                             tsZCL_EndPointDefinition    *psEndPointDefinition,
                                             tsOTA_ImageBlockResponsePayload * psResponse,
                                             tsOTA_ImageHeader           *psOTACurrentHeader,
                                    tsZCL_Address *psZCL_Address)
{
    bool_t    bPKTProcessed   = FALSE;
    bool_t    bPollRequired   = FALSE;
    bool_t    bRequestMore    = TRUE;
    uint32    u32CurrOverflow = 0;
    uint32    u32TotalSize    = 0;
    uint8     u8DataSize      = 0;
    /* rcvd file offset equal persisted file offset */
    uint16    u16TagId;
    uint32    u32TagLength;

#ifdef OTA_CHECK_TRANSACTION_SEQ_NUM
    if((psOTA_Common->sOTACustomCallBackEvent.u8TransactionSequenceNumber == psOTA_Common->sOTACallBackMessage.sPersistedData.u8RequestTransSeqNo)
#ifdef OTA_PAGE_REQUEST_SUPPORT
       ||(psOTA_Common->sOTACallBackMessage.sPersistedData.sPageReqParams.bPageReqOn)
#endif
       )
#endif
    {
        uint16 u16HdrSize;
        vReverseMemcpy((uint8*)&u16HdrSize,&psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header[6],sizeof(uint16));

        vReverseMemcpy((uint8*)&u32TotalSize,&psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header[52],sizeof(uint32));
        DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 26 ..\n");
        //psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = 0;

        if ( ( psResponse->uMessage.sBlockPayloadSuccess.u32FileOffset >= u32TotalSize) ||
             ( psResponse->uMessage.sBlockPayloadSuccess.u8DataSize > OTA_MAX_BLOCK_SIZE) )
        {
            /* rcvd file offset plus rcvd data size greater than total size OR
             * rcvd data size greater than max block size
             */
            vOtaAbortDownload(psOTA_Common, psEndPointDefinition);
            DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 27 ..\n");
            return FALSE;
        }
        else if ( psResponse->uMessage.sBlockPayloadSuccess.u32FileOffset ==
                 psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset )
        {

            u8DataSize = psResponse->uMessage.sBlockPayloadSuccess.u8DataSize;
            DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 28 ..\n");

            /* Set Client Image Upgrade Status Attribute as Download in Progress */
            psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u8ImageUpgradeStatus = E_CLD_OTA_STATUS_DL_IN_PROGRESS;
            eOTA_RestoreClientData(psEndPointDefinition->u8EndPointNumber,
                                   &psOTA_Common->sOTACallBackMessage.sPersistedData,
                                   FALSE);

            psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = 0;
            vOTA_GetTagIdandLengh( &u16TagId,
                                   &u32TagLength,
                                   &psOTA_Common->sOTACallBackMessage.sPersistedData.u8ActiveTag[0]);
            bPKTProcessed = TRUE;
            DBG_vPrintf(TRACE_INT_FLASH, "TAG -> Id %04x Length %d Tag written %d\n",
                    u16TagId,
                    u32TagLength,
                    psOTA_Common->sOTACallBackMessage.sPersistedData.u32TagDataWritten);

            if((u32TotalSize-OTA_TAG_HEADER_SIZE-u16HdrSize) != u32TagLength)
            {
                /* we expected only the image and got image+something else can't allow this*/
                vOtaAbortDownload(psOTA_Common, psEndPointDefinition);
                return FALSE;
            }

            if ( (psOTA_Common->sOTACallBackMessage.sPersistedData.u32TagDataWritten + u8DataSize) <= u32TagLength )
            {
                /* Tag data written plus new data less than or equal tag length */
                if ( u16TagId == OTA_TAG_ID_UPGRADE_IMAGE )
                {
                    /* u16TagId == OTA_TAG_ID_UPGRADE_IMAGE */
                    uint32 u32ValueToWrite, u32DataOverflow, u32Temp, u32ValueToIgnore;

                    /* for own image handling */
                    if( psOTA_Common->sOTACallBackMessage.sPersistedData.u32TagDataWritten > OTA_FLS_MAGIC_NUMBER_LENGTH)
                    {
                        /* u32TagDataWritten > OTA_FLS_MAGIC_NUMBER_LENGTH */
#if (defined OTA_INTERNAL_STORAGE) || (defined KSDK2)
                        if (u8DataSize < 16) {
                            DBG_vPrintf(TRACE_INT_FLASH, "WRITE3 -> Make up data length tto 16\n");
                            u8DataSize = 16;
                        } else {
                            u8DataSize -= (u8DataSize%16);
                        }

#ifdef INTERNAL_ENCRYPTED
                        uint32 u32StartLocation = 0;
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x) || (defined APP0)
                        #ifdef APP0 /* Building with selective OTA */
                            if(psOTA_Common->sOTACallBackMessage.sPersistedData.bStackDownloadActive)
                            {
                                u32StartLocation = OTA_APP1_SHADOW_FLASH_OFFSET;
                            }
                            else
                       #endif
                           {
                               u32StartLocation = psOTA_Common->sOTACallBackMessage.u8ImageStartSector[psOTA_Common->sOTACallBackMessage.u8NextFreeImageLocation]
                                                                     * sNvmDefsStruct.u32SectorSize* OTA_SECTOR_CONVERTION;
                           }
#endif
                        vOtaProcessInternalEncryption( psResponse->uMessage.sBlockPayloadSuccess.pu8Data,
                                psOTA_Common->sOTACallBackMessage.sPersistedData.u32CurrentFlashOffset,
                                (uint32)u8DataSize,
                                u32StartLocation);


#endif    /* of INTERNAL_ENCRYPTED */

#endif
                        DBG_vPrintf(TRACE_INT_FLASH, "\nWRITE3 -> persit Loc %08x, u8DataSize %d \n",
                                psOTA_Common->sOTACallBackMessage.sPersistedData.u32CurrentFlashOffset,
                                        u8DataSize);
                        vOtaFlashLockWrite( psEndPointDefinition, psOTA_Common,
                                            psOTA_Common->sOTACallBackMessage.sPersistedData.u32CurrentFlashOffset,
                                            u8DataSize,
                                            psResponse->uMessage.sBlockPayloadSuccess.pu8Data);
                        psOTA_Common->sOTACallBackMessage.sPersistedData.u32CurrentFlashOffset += u8DataSize;
                        psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset += u8DataSize;
                        psOTA_Common->sOTACallBackMessage.sPersistedData.u32TagDataWritten += u8DataSize;
                    }   /* End of  u32TagDataWritten > OTA_FLS_MAGIC_NUMBER_LENGTH */
                    else
                    {
                        /* Tag data written less than or equal to Magic number */
                        if( ( psOTA_Common->sOTACallBackMessage.sPersistedData.u32TagDataWritten + u8DataSize )
                                    > (OTA_FLS_MAGIC_NUMBER_LENGTH + OTA_REMOVE_BOOTLOADER_SPECIFIC_BYTES_LENGTH))
                        {
                            /* u32TagDataWritten + u8DataSize > OTA_FLS_MAGIC_NUMBER_LENGTH */
                            u32ValueToWrite = psOTA_Common->sOTACallBackMessage.sPersistedData.u32TagDataWritten + u8DataSize
                                    - OTA_FLS_MAGIC_NUMBER_LENGTH - OTA_REMOVE_BOOTLOADER_SPECIFIC_BYTES_LENGTH;
                            u32ValueToIgnore = u8DataSize - u32ValueToWrite;

                            DBG_vPrintf(TRACE_INT_FLASH, "TO write is %d\n", u32ValueToWrite);
                            u32Temp = (u32ValueToWrite % 16);
                            u32ValueToWrite -= u32Temp;
                            u32DataOverflow = u8DataSize - u32Temp;
                            DBG_vPrintf(TRACE_INT_FLASH, "To Adjusted write is %d   Left Over %d\n", u32ValueToWrite, u32Temp);

                            if(u32ValueToWrite > 0)
                            {
                                vOtaFlashLockWrite(psEndPointDefinition, psOTA_Common,
                                    psOTA_Common->sOTACallBackMessage.sPersistedData.u32CurrentFlashOffset,
                                    (uint16)u32ValueToWrite,
                                     psResponse->uMessage.sBlockPayloadSuccess.pu8Data+u32ValueToIgnore);
                               /* Increase the flash offset with Value to Write */
                                psOTA_Common->sOTACallBackMessage.sPersistedData.u32CurrentFlashOffset += u32ValueToWrite;
                            }
                            

                            psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset += u32DataOverflow;
                            psOTA_Common->sOTACallBackMessage.sPersistedData.u32TagDataWritten +=  u32DataOverflow;
                            DBG_vPrintf(TRACE_INT_FLASH, "WRITE4 -> persit Loc %08x, u32ValueToWrite %d \n",
                                                psOTA_Common->sOTACallBackMessage.sPersistedData.u32CurrentFlashOffset,
                                                u32ValueToWrite);
#if ((defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x))
                            psOTA_Common->sOTACallBackMessage.sPersistedData.u8Buf[0] = *(psResponse->uMessage.sBlockPayloadSuccess.pu8Data +
                                                                                                                          (u32ValueToIgnore -
                                                                                                 OTA_REMOVE_BOOTLOADER_SPECIFIC_BYTES_LENGTH)  );
                            psOTA_Common->sOTACallBackMessage.sPersistedData.u8Buf[1] = *(psResponse->uMessage.sBlockPayloadSuccess.pu8Data +
                                                                                                                         ((u32ValueToIgnore -
                                                                                               OTA_REMOVE_BOOTLOADER_SPECIFIC_BYTES_LENGTH) + 1)  );
                            psOTA_Common->sOTACallBackMessage.sPersistedData.u8Buf[2] = *(psResponse->uMessage.sBlockPayloadSuccess.pu8Data +
                                                                                                                         ((u32ValueToIgnore -
                                                                                               OTA_REMOVE_BOOTLOADER_SPECIFIC_BYTES_LENGTH) + 2) );
                            psOTA_Common->sOTACallBackMessage.sPersistedData.u8Buf[3] = *(psResponse->uMessage.sBlockPayloadSuccess.pu8Data +
                                                                                                                         ((u32ValueToIgnore -
                                                                                               OTA_REMOVE_BOOTLOADER_SPECIFIC_BYTES_LENGTH) + 3)  );

#endif
                        }    /* End of u32TagDataWritten + u8DataSize > OTA_FLS_MAGIC_NUMBER_LENGTH */
                        else
                        {
                            /* u32TagDataWritten + u8DataSize <= OTA_FLS_MAGIC_NUMBER_LENGTH */
                            psOTA_Common->sOTACallBackMessage.sPersistedData.u32CurrentFlashOffset += u8DataSize;
                        }
                    }
                }  /* End of u16TagId == OTA_TAG_ID_UPGRADE_IMAGE */
                else
                {
                    /* u16TagId != OTA_TAG_ID_UPGRADE_IMAGE */
                    /* should never happen since we look at tags only for the image and nothing else. */
                    vOtaAbortDownload(psOTA_Common, psEndPointDefinition);
                    return FALSE;
                }

                DBG_vPrintf(TRACE_INT_FLASH, "OFFSET8 -> %08x, added %d\n",
                        psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset,
                                u8DataSize);
            }     /* End of Tag data written plus new data less than or equal tag length */
            else
            {
                /* Tag data written plus new data greater than tag length */
#if TRACE_OTA_DEBUG == TRUE
                uint32 i;
#endif
                DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 29 ..\n");
                /* we are anything but the image now so just write it fully*/
                u32CurrOverflow = psOTA_Common->sOTACallBackMessage.sPersistedData.u32TagDataWritten +
                                        u8DataSize - u32TagLength;
                u32CurrOverflow = u8DataSize - u32CurrOverflow;

                if (u32CurrOverflow > 0 )
                {
                    DBG_vPrintf(TRACE_INT_FLASH, "WRITE5 -> persit Loc %08x, u32CurrOverflow %d \n",
                                   psOTA_Common->sOTACallBackMessage.sPersistedData.u32CurrentFlashOffset,
                                   u32CurrOverflow);
                    vOtaFlashLockWrite(psEndPointDefinition, psOTA_Common,
                                       psOTA_Common->sOTACallBackMessage.sPersistedData.u32CurrentFlashOffset,
                                       (uint16)u32CurrOverflow,
                                       psResponse->uMessage.sBlockPayloadSuccess.pu8Data);
                    psOTA_Common->sOTACallBackMessage.sPersistedData.u32CurrentFlashOffset += u32CurrOverflow;
                    psOTA_Common->sOTACallBackMessage.sPersistedData.u32TagDataWritten += u32CurrOverflow;
                    psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset += u32CurrOverflow;
                    DBG_vPrintf(TRACE_INT_FLASH, "OFFSET9 -> %08x, added %d\n",
                            psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset,
                                        u32CurrOverflow);
                    u8DataSize -= (uint8)u32CurrOverflow;
               }

                DBG_vPrintf(TRACE_INT_FLASH, "WRITE6 -> persit Loc %08x, u8DataSize %d \n",
                                      psOTA_Common->sOTACallBackMessage.sPersistedData.u32CurrentFlashOffset,
                                      u8DataSize);
                psOTA_Common->sOTACallBackMessage.uMessage.sBlockResponseEvent.u8Status = OTA_STATUS_SUCCESS;
                            psOTA_Common->sOTACallBackMessage.uMessage.sBlockResponseEvent.u8DataSize = u8DataSize;
                            psOTA_Common->sOTACallBackMessage.uMessage.sBlockResponseEvent.pu8Data = psResponse->uMessage.sBlockPayloadSuccess.pu8Data +
                                                                                                     u32CurrOverflow;
                           
                            eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_BLOCK_RESPONSE_TAG_OTHER_THAN_UPGRADE_IMAGE,psEndPointDefinition); 
                            psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset += u8DataSize;
                            psOTA_Common->sOTACallBackMessage.sPersistedData.u32CurrentFlashOffset += u8DataSize;
                            DBG_vPrintf(TRACE_INT_FLASH, "OFFSET10 -> %08x, added %d\n",
                                       psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset,
                                       u8DataSize);
               DBG_vPrintf(TRACE_OTA_DEBUG,"\nData received :\n");
#if TRACE_OTA_DEBUG == TRUE
               for (i=0;i<u8DataSize; i++)
                   DBG_vPrintf(TRACE_OTA_DEBUG,"%x", psResponse->uMessage.sBlockPayloadSuccess.pu8Data[i]);
#endif
            }    /* End of Tag data written plus new data greater than tag length */

            if ( psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset >= u32TotalSize )
            {
                /* sAttributes.u32FileOffset == u32TotalSize */
                DBG_vPrintf(TRACE_INT_FLASH, "u32FileOffset == u32TotalSize\n");
                if ( psOTA_Common->sOTACallBackMessage.sPersistedData.bIsNullImage )
                {
                    /* Null image */
                    tsOTA_UpgradeEndRequestPayload sEndRequest;
#ifdef OTA_ACCEPT_ONLY_SIGNED_IMAGES
                    teZCL_Status eStatus;
                    uint16 u16HdrLength ;
                    vReverseMemcpy((uint8*)&u16HdrLength,&psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header[6],sizeof(uint16));
                    // Download Complete
                    eStatus = eOTA_VerifyImage(psEndPointDefinition->u8EndPointNumber,
                                        FALSE,
                                        psOTA_Common->sOTACallBackMessage.u8NextFreeImageLocation,
                                        FALSE);
#else

#endif /* OTA_ACCEPT_ONLY_SIGNED_IMAGES */

                    vReverseMemcpy((uint8*)&sEndRequest.u16ImageType,&psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header[12],sizeof(uint16));
                    vReverseMemcpy((uint8*)&sEndRequest.u16ManufacturerCode,&psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header[10],sizeof(uint16));
                    vReverseMemcpy((uint8*)&sEndRequest.u32FileVersion,&psOTA_Common->sOTACallBackMessage.sPersistedData.au8Header[14],sizeof(uint32));
#ifdef OTA_ACCEPT_ONLY_SIGNED_IMAGES
                    /* check status */
                    if(eStatus)
                    {
                        sEndRequest.u8Status = OTA_STATUS_IMAGE_INVALID;
                    }
                    else
                    {
                        sEndRequest.u8Status = E_ZCL_SUCCESS;
                    }
#else
                    sEndRequest.u8Status = E_ZCL_SUCCESS;
#endif

                    /* change state to normal if status is success */
                    if( sEndRequest.u8Status == E_ZCL_SUCCESS )
                    {
#if (OTA_MAX_CO_PROCESSOR_IMAGES != 0)
                        /* Decrement downloadable images */
                        psOTA_Common->sOTACallBackMessage.sPersistedData.u8NumOfDownloadableImages--;

                        /* Check Any images are pedning for downloading */
                        if( ( !psOTA_Common->sOTACallBackMessage.sPersistedData.u8NumOfDownloadableImages) ||
                            ( !bIsCoProcessorImgUpgdDependent ) )
                        {
                           sEndRequest.u8Status = OTA_STATUS_SUCCESS;
                           /* change state to dowmload complete */
                           vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_DL_COMPLETE, psEndPointDefinition, psOTA_Common);
                           /* if it is a success case then enable retries and wait for response */
                           psOTA_Common->sOTACallBackMessage.sPersistedData.u8Retry = 0;
                           psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime()+
                                                                                                         OTA_TIME_INTERVAL_BETWEEN_END_REQUEST_RETRIES + 1;
                        }
                        else
                        {
                            sEndRequest.u8Status = OTA_REQUIRE_MORE_IMAGE;
                            /* Change the state to Normal State */
                            vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_NORMAL,psEndPointDefinition,psOTA_Common);
                        }
#else
                        vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_DL_COMPLETE, psEndPointDefinition, psOTA_Common);
#endif

                    }     /* End of if( sEndRequest.u8Status == E_ZCL_SUCCESS ) */

#ifdef OTA_ACCEPT_ONLY_SIGNED_IMAGES
                    else
                    {
                        /* Move to normal state */
                        vOtaClientUpgMgrMapStates(E_CLD_OTA_STATUS_NORMAL, psEndPointDefinition, psOTA_Common);

                        /* Give a call back to the user */
                        eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_OTA_DL_ABORTED);
                    }
#endif

                    /* Send block end request */
                    DBG_vPrintf(TRACE_VERIF, "Send  Upgrade End Response\n");
                    eOTA_ClientUpgradeEndRequest(psOTA_Common->sReceiveEventAddress.u8DstEndpoint,
                                        psOTA_Common->sReceiveEventAddress.u8SrcEndpoint,
                                        psZCL_Address,
                                        &sEndRequest);
#if (OTA_MAX_CO_PROCESSOR_IMAGES != 0)
                    if(sEndRequest.u8Status == OTA_REQUIRE_MORE_IMAGE)
                    {
                        /* Give a call back to the user for requesting remaining images */
                        eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_REQUEST_QUERY_NEXT_IMAGES,psEndPointDefinition);
                    }
#endif
                    return FALSE;
                }    /* End of Null image */
#if (defined gEepromParams_bufferedWrite_c) || (defined APP0)
#if (defined APP0)
                /* Any pending bytes in the internal buffer now needs to be written to flash since download is finished
                 * the flash offset has been moved on so we need to realign it back ot where we should have previously written */
                bFlashWriteBufferedTailEndBytes((psOTA_Common->sOTACallBackMessage.sPersistedData.u32CurrentFlashOffset - u8DataSize));
#else
                OTA_CommitImage(NULL);
#endif
#endif
                bRequestMore = FALSE;
                DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 30 ..\n");
#ifdef OTA_ACCEPT_ONLY_SIGNED_IMAGES
                // Download Complete

                /* Generate callback to the user, he will run image verification in lower priority */
                /* Give a call back to the user */
                eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_OTA_START_IMAGE_VERIFICATION_IN_LOW_PRIORITY,psEndPointDefinition);
                psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = 0;
                return FALSE;
#else
                eOTA_HandleImageVerification(psOTA_Common->sReceiveEventAddress.u8DstEndpoint,
                                             psOTA_Common->sReceiveEventAddress.u8SrcEndpoint,
                                             E_ZCL_SUCCESS);
#endif
            }   /* End of sAttributes.u32FileOffset == u32TotalSize */
        }   /* End of rcvd file offset equal persisted file offset */

        if ( bRequestMore && ( bPKTProcessed
#ifdef OTA_PAGE_REQUEST_SUPPORT
             || psOTA_Common->sOTACallBackMessage.sPersistedData.sPageReqParams.bPageReqOn
#endif
           ))
        {
            psOTA_Common->sOTACallBackMessage.sPersistedData.u8Retry = 0;
            DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 32 ..\n");
#ifndef OTA_TIME_INTERVAL_BETWEEN_REQUESTS
#ifdef OTA_CLD_ATTR_REQUEST_DELAY
            if(psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16MinBlockRequestDelay > 0)
                psOTA_Common->sOTACallBackMessage.sPersistedData.bWaitForBlockReq = TRUE;
#endif
            vOtaHandleTimedBlockRequest(psOTA_Common, &bPollRequired, psOTACurrentHeader, psEndPointDefinition);
            psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime()+ OTA_TIME_INTERVAL_BETWEEN_RETRIES+1;
#else
            psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime()+OTA_TIME_INTERVAL_BETWEEN_REQUESTS+1;
#endif
        }
    }

    return bPollRequired;
}

/****************************************************************************
 **
 ** NAME:       bOtaHandleBlockResponseCoProcessorImage
 **
 ** DESCRIPTION:
 ** Handle wait for data status
 ** PARAMETERS:                           Name                           Usage
 ** tsZCL_EndPointDefinition           *psEndPointDefinition           Endpoint definition
 ** tsOTA_Common                       *psOTA_Common                   OTA custom data
 **
 ** RETURN:
 ** True -> strings match
 ** False -> strings don't match
 ****************************************************************************/
PRIVATE bool_t bOtaHandleBlockResponseCoProcessorImage( tsOTA_Common *psOTA_Common,
                                             tsZCL_EndPointDefinition    *psEndPointDefinition,
                                             tsOTA_ImageBlockResponsePayload * psResponse,
                                             tsOTA_ImageHeader           *psOTACurrentHeader,
                                    tsZCL_Address *psZCL_Address)
{
    bool_t bPollRequired = FALSE;
    bool_t bRequestMore = TRUE;


#ifdef OTA_CHECK_TRANSACTION_SEQ_NUM
    if((psOTA_Common->sOTACustomCallBackEvent.u8TransactionSequenceNumber == psOTA_Common->sOTACallBackMessage.sPersistedData.u8RequestTransSeqNo)
#ifdef OTA_PAGE_REQUEST_SUPPORT
       ||(psOTA_Common->sOTACallBackMessage.sPersistedData.sPageReqParams.bPageReqOn)
#endif
       )
#endif
    {
        if((psResponse->uMessage.sBlockPayloadSuccess.u32FileOffset +
                psResponse->uMessage.sBlockPayloadSuccess.u8DataSize > psOTA_Common->sOTACallBackMessage.sPersistedData.u32CoProcessorImageSize) ||
            (psResponse->uMessage.sBlockPayloadSuccess.u8DataSize > OTA_MAX_BLOCK_SIZE))
        {
            vOtaAbortDownload(psOTA_Common, psEndPointDefinition);
            eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_CO_PROCESSOR_DL_ABORT,psEndPointDefinition);
            DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 27 ..\n");
            return FALSE;
        }
        else if(psResponse->uMessage.sBlockPayloadSuccess.u32FileOffset ==
            psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset)
        {
            psOTA_Common->sOTACallBackMessage.eEventId = E_CLD_OTA_INTERNAL_COMMAND_CO_PROCESSOR_BLOCK_RESPONSE;
            psOTA_Common->sOTACallBackMessage.uMessage.sImageBlockResponsePayload.uMessage.sBlockPayloadSuccess = psResponse->uMessage.sBlockPayloadSuccess;
            psOTA_Common->sOTACallBackMessage.uMessage.sImageBlockResponsePayload.u8Status = OTA_STATUS_SUCCESS;
            psEndPointDefinition->pCallBackFunctions(&psOTA_Common->sOTACustomCallBackEvent);
            psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset += psResponse->uMessage.sBlockPayloadSuccess.u8DataSize;
            DBG_vPrintf(TRACE_INT_FLASH, "OFFSET11 -> %08x, added %d\n",
                    psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset,
                                                   psResponse->uMessage.sBlockPayloadSuccess.u8DataSize);
            /* check if download complete */
            if(psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset == psOTA_Common->sOTACallBackMessage.sPersistedData.u32CoProcessorImageSize)
            {
                /* send a call back event if download is complete */
                eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_CO_PROCESSOR_IMAGE_DL_COMPLETE,psEndPointDefinition);
                psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = 0;
                bPollRequired = TRUE;
                bRequestMore = FALSE;
            }
        }
        if(bRequestMore)
        {
            psOTA_Common->sOTACallBackMessage.sPersistedData.u8Retry = 0;
            DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 32 ..\n");
#ifndef OTA_TIME_INTERVAL_BETWEEN_REQUESTS
#ifdef OTA_CLD_ATTR_REQUEST_DELAY
            if(psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16MinBlockRequestDelay > 0)
                psOTA_Common->sOTACallBackMessage.sPersistedData.bWaitForBlockReq = TRUE;
#endif
            vOtaHandleTimedBlockRequest(psOTA_Common, &bPollRequired, psOTACurrentHeader, psEndPointDefinition);
            psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime()+ OTA_TIME_INTERVAL_BETWEEN_RETRIES+1;
#else
            psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime()+OTA_TIME_INTERVAL_BETWEEN_REQUESTS+1;
#endif
        }
    }
return bPollRequired;
}

/****************************************************************************
 **
 ** NAME:       bOtaHandleBlockResponseFileSpecificImage
 **
 ** DESCRIPTION:
 ** Handle wait for data status
 ** PARAMETERS:                           Name                           Usage
 ** tsZCL_EndPointDefinition           *psEndPointDefinition           Endpoint definition
 ** tsOTA_ImageBlockResponsePayload    *psResponse                     payload of block response
 **
 ** RETURN:
 ** True -> poll required
 ** False -> poll not requried
 ****************************************************************************/
PRIVATE bool_t bOtaHandleBlockResponseFileSpecificImage( tsOTA_Common *psOTA_Common,
                                             tsZCL_EndPointDefinition    *psEndPointDefinition,
                                             tsOTA_ImageBlockResponsePayload * psResponse,
                                             tsOTA_ImageHeader           *psOTACurrentHeader,
                                    tsZCL_Address *psZCL_Address)
{
    bool_t bPollRequired = FALSE;
    bool_t bRequestMore = TRUE;

#ifdef OTA_CHECK_TRANSACTION_SEQ_NUM
    if((psOTA_Common->sOTACustomCallBackEvent.u8TransactionSequenceNumber == psOTA_Common->sOTACallBackMessage.sPersistedData.u8RequestTransSeqNo)
#ifdef OTA_PAGE_REQUEST_SUPPORT
        ||(psOTA_Common->sOTACallBackMessage.sPersistedData.sPageReqParams.bPageReqOn)
#endif
        )
#endif
    {
        if((psResponse->uMessage.sBlockPayloadSuccess.u32FileOffset+
            psResponse->uMessage.sBlockPayloadSuccess.u8DataSize > psOTA_Common->sOTACallBackMessage.sPersistedData.u32SpecificFileSize) ||
            (psResponse->uMessage.sBlockPayloadSuccess.u8DataSize > OTA_MAX_BLOCK_SIZE))
        {
            vOtaAbortDownload(psOTA_Common, psEndPointDefinition);
            eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_SPECIFIC_FILE_DL_ABORT,psEndPointDefinition);
            DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 82 ..\n");
            return FALSE;
        }
        else if(psResponse->uMessage.sBlockPayloadSuccess.u32FileOffset ==
            psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset)
        {
            psOTA_Common->sOTACallBackMessage.eEventId = E_CLD_OTA_INTERNAL_COMMAND_SPECIFIC_FILE_BLOCK_RESPONSE;
            psOTA_Common->sOTACallBackMessage.uMessage.sImageBlockResponsePayload.uMessage.sBlockPayloadSuccess = psResponse->uMessage.sBlockPayloadSuccess;
            psOTA_Common->sOTACallBackMessage.uMessage.sImageBlockResponsePayload.u8Status = OTA_STATUS_SUCCESS;
            psEndPointDefinition->pCallBackFunctions(&psOTA_Common->sOTACustomCallBackEvent);
            psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset += psResponse->uMessage.sBlockPayloadSuccess.u8DataSize;
            DBG_vPrintf(TRACE_INT_FLASH, "OFFSET12 -> %08x, added %d\n",
                    psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset,
                                    psResponse->uMessage.sBlockPayloadSuccess.u8DataSize);
            /* check if download complete */
            if(psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset == psOTA_Common->sOTACallBackMessage.sPersistedData.u32SpecificFileSize)
            {
                /* send a call back event if download is complete */
                eOtaSetEventTypeAndGiveCallBack(psOTA_Common, E_CLD_OTA_INTERNAL_COMMAND_SPECIFIC_FILE_DL_COMPLETE,psEndPointDefinition);
                psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = 0;
                bPollRequired = TRUE;
                bRequestMore = FALSE;
            }
        }
        if(bRequestMore)
        {
            psOTA_Common->sOTACallBackMessage.sPersistedData.u8Retry = 0;
            DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 83 ..\n");

#ifndef OTA_TIME_INTERVAL_BETWEEN_REQUESTS
#ifdef OTA_CLD_ATTR_REQUEST_DELAY
            if(psOTA_Common->sOTACallBackMessage.sPersistedData.sAttributes.u16MinBlockRequestDelay > 0)
                psOTA_Common->sOTACallBackMessage.sPersistedData.bWaitForBlockReq = TRUE;
#endif
            vOtaHandleTimedBlockRequest(psOTA_Common, &bPollRequired, psOTACurrentHeader, psEndPointDefinition);
            psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime()+ OTA_TIME_INTERVAL_BETWEEN_RETRIES + 1;
#else
            psOTA_Common->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = u32ZCL_GetUTCTime()+OTA_TIME_INTERVAL_BETWEEN_REQUESTS + 1;
#endif
        }
    }
    return bPollRequired;
}


#endif /* #ifdef OTA_CLIENT */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
