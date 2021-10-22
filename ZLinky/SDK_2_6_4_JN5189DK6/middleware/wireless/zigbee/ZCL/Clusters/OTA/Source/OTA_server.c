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
 * COMPONENT:          OTA_server.c
 *
 * DESCRIPTION:        Over The Air Upgrade
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "zcl.h"
#include "OTA.h"
#include "zcl_options.h"
#include "OTA_private.h"

#ifdef OTA_SERVER
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
 ** NAME:       eOTA_ServerImageNotify
 **
 ** DESCRIPTION:
 ** sends image notify command
 **
 ** PARAMETERS:                 Name                           Usage
 ** uint8                     u8SourceEndPointId            Source EP Id
 ** uint8                     u8DestinationEndPointId       Destination EP Id
 ** tsZCL_Address            *psDestinationAddress          Destination Address
 ** tsOTA_ImageNotifyCommand *psImageNotifyCommand          command payload
 **
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_ServerImageNotify(
                    uint8                     u8SourceEndpoint,
                    uint8                     u8DestinationEndpoint,
                    tsZCL_Address            *psDestinationAddress,
                    tsOTA_ImageNotifyCommand *psImageNotifyCommand)
{
    teZCL_Status eZCL_Status;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;
    uint8 u8SequenceNumber;
    if((eZCL_Status =
        eOtaFindCluster(u8SourceEndpoint,
                         &psEndPointDefinition,
                           &psClusterInstance,
                           &psCustomData,
                           TRUE))
                           == E_ZCL_SUCCESS)
    {
        if (!psClusterInstance->bIsServer)
        {
            eZCL_Status = E_ZCL_FAIL;
        }
        else
        {
            if(!OTA_IS_UNICAST(psDestinationAddress->eAddressMode,psDestinationAddress->uAddress.u16DestinationAddress))
            {
                psEndPointDefinition->bDisableDefaultResponse = TRUE;
            }
            if(psImageNotifyCommand->ePayloadType <= E_CLD_OTA_ITYPE_MDID_FVERSION_JITTER)
            {
                if(E_CLD_OTA_ITYPE_MDID_FVERSION_JITTER == psImageNotifyCommand->ePayloadType)
                {
                    tsZCL_TxPayloadItem asPayloadDefinition[] = { {1, E_ZCL_UINT8,   &psImageNotifyCommand->ePayloadType},
                                                                {1, E_ZCL_UINT8,   &psImageNotifyCommand->u8QueryJitter},
                                                                {1, E_ZCL_UINT16,   &psImageNotifyCommand->u16ManufacturerCode},
                                                                {1, E_ZCL_UINT16,   &psImageNotifyCommand->u16ImageType},
                                                                {1, E_ZCL_UINT32,   &psImageNotifyCommand->u32NewFileVersion}
                                                };
                    eZCL_Status = eZCL_CustomCommandSend(u8SourceEndpoint,
                              u8DestinationEndpoint,
                              psDestinationAddress,
                              OTA_CLUSTER_ID,
                              TRUE,
                              E_CLD_OTA_COMMAND_IMAGE_NOTIFY,
                              &u8SequenceNumber,
                              asPayloadDefinition,
                              FALSE,
                              0,
                              sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
                }

                if(E_CLD_OTA_ITYPE_MDID_JITTER == psImageNotifyCommand->ePayloadType)
                {
                    tsZCL_TxPayloadItem asPayloadDefinition[] = { {1, E_ZCL_UINT8,   &psImageNotifyCommand->ePayloadType},
                                                                {1, E_ZCL_UINT8,   &psImageNotifyCommand->u8QueryJitter},
                                                                {1, E_ZCL_UINT16,   &psImageNotifyCommand->u16ManufacturerCode},
                                                                {1, E_ZCL_UINT16,   &psImageNotifyCommand->u16ImageType}
                                                };
                    eZCL_Status = eZCL_CustomCommandSend(u8SourceEndpoint,
                              u8DestinationEndpoint,
                              psDestinationAddress,
                              OTA_CLUSTER_ID,
                              TRUE,
                              E_CLD_OTA_COMMAND_IMAGE_NOTIFY,
                              &u8SequenceNumber,
                              asPayloadDefinition,
                              FALSE,
                              0,
                              sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
                }

                if(E_CLD_OTA_MANUFACTURER_ID_AND_JITTER == psImageNotifyCommand->ePayloadType)
                {
                    tsZCL_TxPayloadItem asPayloadDefinition[] = { {1, E_ZCL_UINT8,   &psImageNotifyCommand->ePayloadType},
                                                                {1, E_ZCL_UINT8,   &psImageNotifyCommand->u8QueryJitter},
                                                                {1, E_ZCL_UINT16,   &psImageNotifyCommand->u16ManufacturerCode},

                                                };
                    eZCL_Status = eZCL_CustomCommandSend(u8SourceEndpoint,
                              u8DestinationEndpoint,
                              psDestinationAddress,
                              OTA_CLUSTER_ID,
                              TRUE,
                              E_CLD_OTA_COMMAND_IMAGE_NOTIFY,
                              &u8SequenceNumber,
                              asPayloadDefinition,
                              FALSE,
                              0,
                              sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
                }

                if(E_CLD_OTA_QUERY_JITTER == psImageNotifyCommand->ePayloadType)
                {
                    tsZCL_TxPayloadItem asPayloadDefinition[] = { {1, E_ZCL_UINT8,   &psImageNotifyCommand->ePayloadType},
                                                                  {1, E_ZCL_UINT8,   &psImageNotifyCommand->u8QueryJitter}};
                    eZCL_Status = eZCL_CustomCommandSend(u8SourceEndpoint,
                              u8DestinationEndpoint,
                              psDestinationAddress,
                              OTA_CLUSTER_ID,
                              TRUE,
                              E_CLD_OTA_COMMAND_IMAGE_NOTIFY,
                              &u8SequenceNumber,
                              asPayloadDefinition,
                              FALSE,
                              0,
                              sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
                }
            }
            else
            {
               eZCL_Status = E_ZCL_ERR_PARAMETER_RANGE;
            }
        }
    }
    return eZCL_Status;
}

/****************************************************************************
 **
 ** NAME:       eOTA_ServerQueryNextImageResponse
 **
 ** DESCRIPTION:
 ** sends query next image response command
 **
 ** PARAMETERS:                Name                           Usage
 ** uint8                     u8SourceEndPointId            Source EP Id
 ** uint8                     u8DestinationEndPointId       Destination EP Id
 ** tsZCL_Address            *psDestinationAddress          Destination Address
 ** tsOTA_QueryImageResponse *psQueryImageResponsePayload   command payload
 ** uint8                     u8TransactionSequenceNumber   transaction sequence number
 **
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/

PUBLIC  teZCL_Status eOTA_ServerQueryNextImageResponse(
                    uint8                     u8SourceEndpoint,
                    uint8                     u8DestinationEndpoint,
                    tsZCL_Address            *psDestinationAddress,
                    tsOTA_QueryImageResponse *psQueryImageResponsePayload,
                    uint8                     u8TransactionSequenceNumber)
{
    teZCL_Status eZCL_Status;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;

    if((eZCL_Status =
        eOtaFindCluster(u8SourceEndpoint,
                         &psEndPointDefinition,
                           &psClusterInstance,
                           &psCustomData,
                           TRUE))
                           == E_ZCL_SUCCESS)
    {
        if (!psClusterInstance->bIsServer)
        {
            eZCL_Status = E_ZCL_FAIL;
        }
        else
        {
            psEndPointDefinition->bDisableDefaultResponse = TRUE;
            if(psQueryImageResponsePayload->u8Status == OTA_STATUS_SUCCESS)
            {
                tsZCL_TxPayloadItem asPayloadDefinition[] ={  {1, E_ZCL_UINT8,   &psQueryImageResponsePayload->u8Status},
                                                  {1, E_ZCL_UINT16,   &psQueryImageResponsePayload->u16ManufacturerCode},
                                                  {1, E_ZCL_UINT16,   &psQueryImageResponsePayload->u16ImageType},
                                                  {1, E_ZCL_UINT32,   &psQueryImageResponsePayload->u32FileVersion},
                                                  {1, E_ZCL_UINT32,   &psQueryImageResponsePayload->u32ImageSize}
                                                };

                eZCL_Status = eZCL_CustomCommandSend(u8SourceEndpoint,
                                        u8DestinationEndpoint,
                                        psDestinationAddress,
                                        OTA_CLUSTER_ID,
                                        TRUE,
                                        E_CLD_OTA_COMMAND_QUERY_NEXT_IMAGE_RESPONSE,
                                        &u8TransactionSequenceNumber,
                                        asPayloadDefinition,
                                        FALSE,
                                        0,
                                        sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

            }
            else
            {
                tsZCL_TxPayloadItem asPayloadDefinition[] ={  {1, E_ZCL_UINT8,   &psQueryImageResponsePayload->u8Status}
                                                };

                eZCL_Status = eZCL_CustomCommandSend(u8SourceEndpoint,
                                        u8DestinationEndpoint,
                                        psDestinationAddress,
                                        OTA_CLUSTER_ID,
                                        TRUE,
                                        E_CLD_OTA_COMMAND_QUERY_NEXT_IMAGE_RESPONSE,
                                        &u8TransactionSequenceNumber,
                                        asPayloadDefinition,
                                        FALSE,
                                        0,
                                        sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
            }
        }
    }
    return eZCL_Status;
}

/****************************************************************************
 **
 ** NAME:       eOTA_ServerImageBlockResponse
 **
 ** DESCRIPTION:
 ** sends image block response command
 **
 ** PARAMETERS:                          Name                           Usage
 ** uint8                            u8SourceEndPointId            Source EP Id
 ** uint8                            u8DestinationEndPointId       Destination EP Id
 ** tsZCL_Address                   *psDestinationAddress          Destination Address
 ** tsOTA_ImageBlockResponsePayload *psImageBlockResponsePayload   command payload
 ** uint8                            u8SizeOfBlock                 Block size
 ** uint8                            u8TransactionSequenceNumber   transaction sequence number
 **
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/

PUBLIC  teZCL_Status eOTA_ServerImageBlockResponse(
                    uint8                            u8SourceEndpoint,
                    uint8                            u8DestinationEndpoint,
                    tsZCL_Address                   *psDestinationAddress,
                    tsOTA_ImageBlockResponsePayload *psImageBlockResponsePayload,
                    uint8                            u8SizeOfBlock,
                    uint8                            u8TransactionSequenceNumber)
{
    teZCL_Status eZCL_Status;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;

    if((eZCL_Status =
        eOtaFindCluster(u8SourceEndpoint,
                         &psEndPointDefinition,
                           &psClusterInstance,
                           &psCustomData,
                           TRUE))
                           == E_ZCL_SUCCESS)
    {
        if (!psClusterInstance->bIsServer)
        {
            eZCL_Status = E_ZCL_FAIL;
        }
        else
        {
            psEndPointDefinition->bDisableDefaultResponse = TRUE;
            if(psImageBlockResponsePayload->u8Status == OTA_STATUS_SUCCESS)
            {
                tsZCL_TxPayloadItem asPayloadDefinition[] = { {1, E_ZCL_UINT8,    &psImageBlockResponsePayload->u8Status},
                                                              {1, E_ZCL_UINT16,   &psImageBlockResponsePayload->uMessage.sBlockPayloadSuccess.u16ManufacturerCode},
                                                              {1, E_ZCL_UINT16,   &psImageBlockResponsePayload->uMessage.sBlockPayloadSuccess.u16ImageType},
                                                              {1, E_ZCL_UINT32,   &psImageBlockResponsePayload->uMessage.sBlockPayloadSuccess.u32FileVersion},
                                                              {1, E_ZCL_UINT32,   &psImageBlockResponsePayload->uMessage.sBlockPayloadSuccess.u32FileOffset},
                                                              {1, E_ZCL_UINT8,   &psImageBlockResponsePayload->uMessage.sBlockPayloadSuccess.u8DataSize},
                                                              {u8SizeOfBlock, E_ZCL_UINT8,  psImageBlockResponsePayload->uMessage.sBlockPayloadSuccess.pu8Data}
                                                            };

                eZCL_Status = eZCL_CustomCommandSend(u8SourceEndpoint,
                                                     u8DestinationEndpoint,
                                                     psDestinationAddress,
                                                     OTA_CLUSTER_ID,
                                                     TRUE,
                                                     E_CLD_OTA_COMMAND_BLOCK_RESPONSE,
                                                     &u8TransactionSequenceNumber,
                                                     asPayloadDefinition,
                                                     FALSE,
                                                     0,
                                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));

            }
            else if(psImageBlockResponsePayload->u8Status == OTA_STATUS_WAIT_FOR_DATA)
            {
                tsZCL_TxPayloadItem asPayloadDefinition[] = { {1, E_ZCL_UINT8,    &psImageBlockResponsePayload->u8Status},
                                                              {1, E_ZCL_UINT32,   &psImageBlockResponsePayload->uMessage.sWaitForData.u32CurrentTime},
                                                              {1, E_ZCL_UINT32,   &psImageBlockResponsePayload->uMessage.sWaitForData.u32RequestTime},
                                                              {1, E_ZCL_UINT16,   &psImageBlockResponsePayload->uMessage.sWaitForData.u16BlockRequestDelayMs}
                                                            };
                eZCL_Status = eZCL_CustomCommandSend(u8SourceEndpoint,
                                                     u8DestinationEndpoint,
                                                     psDestinationAddress,
                                                     OTA_CLUSTER_ID,
                                                     TRUE,
                                                     E_CLD_OTA_COMMAND_BLOCK_RESPONSE,
                                                     &u8TransactionSequenceNumber,
                                                     asPayloadDefinition,
                                                     FALSE,
                                                     0,
                                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
            }
            else
            {
                tsZCL_TxPayloadItem asPayloadDefinition[] = { {1, E_ZCL_UINT8,    &psImageBlockResponsePayload->u8Status}
                                                            };
                eZCL_Status = eZCL_CustomCommandSend(u8SourceEndpoint,
                                                     u8DestinationEndpoint,
                                                     psDestinationAddress,
                                                     OTA_CLUSTER_ID,
                                                     TRUE,
                                                     E_CLD_OTA_COMMAND_BLOCK_RESPONSE,
                                                     &u8TransactionSequenceNumber,
                                                     asPayloadDefinition,
                                                     FALSE,
                                                     0,
                                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
            }
        }
    }
    return eZCL_Status;
}
/****************************************************************************
 **
 ** NAME:       eOTA_ServerUpgradeEndResponse
 **
 ** DESCRIPTION:
 ** sends upgrade end response command
 **
 ** PARAMETERS:                          Name                           Usage
 ** uint8                            u8SourceEndPointId            Source EP Id
 ** uint8                            u8DestinationEndPointId       Destination EP Id
 ** tsZCL_Address                   *psDestinationAddress          Destination Address
 ** tsOTA_UpgradeEndResponsePayload *psUpgradeResponsePayload      command payload
 ** uint8                            u8TransactionSequenceNumber   transaction sequence number
 **
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_ServerUpgradeEndResponse(
                    uint8                            u8SourceEndpoint,
                    uint8                            u8DestinationEndpoint,
                    tsZCL_Address                   *psDestinationAddress,
                    tsOTA_UpgradeEndResponsePayload *psUpgradeResponsePayload,
                    uint8                            u8TransactionSequenceNumber)
{
    teZCL_Status eZCL_Status;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;

    if((eZCL_Status =
        eOtaFindCluster(u8SourceEndpoint,
                          &psEndPointDefinition,
                           &psClusterInstance,
                           &psCustomData,
                           TRUE))
                           == E_ZCL_SUCCESS)
    {
        if (!psClusterInstance->bIsServer)
        {
            eZCL_Status = E_ZCL_FAIL;
        }
        else
        {

            tsZCL_TxPayloadItem asPayloadDefinition[] = { {1, E_ZCL_UINT16,    &psUpgradeResponsePayload->u16ManufacturerCode},
                                                          {1, E_ZCL_UINT16,    &psUpgradeResponsePayload->u16ImageType},
                                                          {1, E_ZCL_UINT32,   &psUpgradeResponsePayload->u32FileVersion},
                                                          {1, E_ZCL_UINT32,   &psUpgradeResponsePayload->u32CurrentTime},
                                                          {1, E_ZCL_UINT32,   &psUpgradeResponsePayload->u32UpgradeTime}
                                                            };
            psEndPointDefinition->bDisableDefaultResponse = TRUE;
            eZCL_Status = eZCL_CustomCommandSend(u8SourceEndpoint,
                                                     u8DestinationEndpoint,
                                                     psDestinationAddress,
                                                     OTA_CLUSTER_ID,
                                                     TRUE,
                                                     E_CLD_OTA_COMMAND_UPGRADE_END_RESPONSE,
                                                     &u8TransactionSequenceNumber,
                                                     asPayloadDefinition,
                                                     FALSE,
                                                     0,
                                                     sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
        }
    }
    return eZCL_Status;

}

/****************************************************************************
 **
 ** NAME:       eOTA_ServerQuerySpecificFileResponse
 **
 ** DESCRIPTION:
 ** sends query specific file response command
 **
 ** PARAMETERS:                                  Name                                 Usage
 ** uint8                                   u8SourceEndPointId                       Source EP Id
 ** uint8                                   u8DestinationEndPointId                  Destination EP Id
 ** tsZCL_Address                          *psDestinationAddress                     Destination Address
 ** tsOTA_QuerySpecificFileResponsePayload *psQuerySpecificFileResponsePayload       command payload
 ** uint8                                   u8TransactionSequenceNumber              transaction sequence number
 **
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_ServerQuerySpecificFileResponse(
                    uint8                                   u8SourceEndpoint,
                    uint8                                   u8DestinationEndpoint,
                    tsZCL_Address                          *psDestinationAddress,
                    tsOTA_QuerySpecificFileResponsePayload *psQuerySpecificFileResponsePayload,
                    uint8                                   u8TransactionSequenceNumber)
{
    teZCL_Status eZCL_Status;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;

    if((eZCL_Status =
        eOtaFindCluster(u8SourceEndpoint,
                         &psEndPointDefinition,
                         &psClusterInstance,
                           &psCustomData,
                           TRUE))
                           == E_ZCL_SUCCESS)
    {
        if (!psClusterInstance->bIsServer)
        {
            eZCL_Status = E_ZCL_FAIL;
        }
        else
        {
            psEndPointDefinition->bDisableDefaultResponse = TRUE;
            if(psQuerySpecificFileResponsePayload->u8Status == OTA_STATUS_SUCCESS)
            {
                tsZCL_TxPayloadItem asPayloadDefinition[] = { {1, E_ZCL_UINT8,   &psQuerySpecificFileResponsePayload->u8Status},
                                                  {1, E_ZCL_UINT16,   &psQuerySpecificFileResponsePayload->u16ManufacturerCode},
                                                  {1, E_ZCL_UINT16,   &psQuerySpecificFileResponsePayload->u16ImageType},
                                                  {1, E_ZCL_UINT32,   &psQuerySpecificFileResponsePayload->u32FileVersion},
                                                  {1, E_ZCL_UINT32,   &psQuerySpecificFileResponsePayload->u32ImageSize}
                                                };
                eZCL_Status = eZCL_CustomCommandSend(u8SourceEndpoint,
                                                 u8DestinationEndpoint,
                                                 psDestinationAddress,
                                                 OTA_CLUSTER_ID,
                                                 TRUE,
                                                 E_CLD_OTA_COMMAND_QUERY_SPECIFIC_FILE_RESPONSE,
                                                 &u8TransactionSequenceNumber,
                                                 asPayloadDefinition,
                                                 FALSE,
                                                 0,
                                                 sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
            }
            else
            {
                tsZCL_TxPayloadItem asPayloadDefinition[] = { {1, E_ZCL_UINT8,   &psQuerySpecificFileResponsePayload->u8Status} };

                eZCL_Status = eZCL_CustomCommandSend(u8SourceEndpoint,
                                                 u8DestinationEndpoint,
                                                 psDestinationAddress,
                                                 OTA_CLUSTER_ID,
                                                 TRUE,
                                                 E_CLD_OTA_COMMAND_QUERY_SPECIFIC_FILE_RESPONSE,
                                                 &u8TransactionSequenceNumber,
                                                 asPayloadDefinition,
                                                 FALSE,
                                                 0,
                                                 sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem));
            }
        }
    }
    return eZCL_Status;
}
#endif
/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
