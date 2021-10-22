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
 * COMPONENT:          zcl_customcommand.h
 *
 * DESCRIPTION:       internal-ish API ZCL API calls needed to write custom commands
 *
 ****************************************************************************/

#ifndef  ZCL_CUSTOM_COMMANDS_H_INCLUDED
#define  ZCL_CUSTOM_COMMANDS_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include "jendefs.h"
#include "zcl.h"
#include "pdum_apl.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/* Security modes for data requests */
typedef enum {
    eTX_FROM_CLIENT             = 0,
    eTX_CLUSTER_SPECIFIC        = 0x01,
    eTX_MANUFACTURER_SPECIFIC   = 0x02,
    eTX_FROM_SERVER             = 0x04,
    eTX_DISABLE_DEFAULT_RSP     = 0x08
} eInterPanTxOptions;

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef struct
{
    bool_t                      bInitialised;
    uint8                       u8DstAddrMode;
    uint8                       u8DstEndpoint;
    uint8                       u8SrcAddrMode;
    uint8                       u8SrcEndpoint;
    ZPS_tuAddress               uDstAddress;
    ZPS_tuAddress               uSrcAddress;
} tsZCL_ReceiveEventAddress;

typedef struct
{
    bool_t                bInitialised;
    ZPS_tsInterPanAddress sSrcAddr;
    ZPS_tsInterPanAddress sDstAddr;
} tsZCL_ReceiveEventAddressInterPan;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC teZCL_Status  eZCL_SearchForEPentry(
                 uint8                      u8endpointId,
                 tsZCL_EndPointDefinition **ppsZCL_EndPointDefinition);

PUBLIC teZCL_Status eZCL_SearchForEPentryAndCheckManufacturerId(
                 uint8                       u8endpointId,
                 bool_t                      bIsManufacturerSpecific,
                 uint16                      u16ManufacturerCode,
                 tsZCL_EndPointDefinition  **ppsZCL_EndPointDefinition);

PUBLIC teZCL_Status  eZCL_SearchForClusterEntry(
                 uint8                      u8EndpointId,
                 uint16                     u16ClusterEnum,
                 bool_t                     bDirection,
                 tsZCL_ClusterInstance    **ppsClusterInstance);

PUBLIC teZCL_Status eZCL_FindCluster(
                uint16                      u16ClusterId,
                uint8                       u8SourceEndPointId,
                bool_t                      bIsServer,
                tsZCL_EndPointDefinition  **ppsEndPointDefinition,
                tsZCL_ClusterInstance     **ppsClusterInstance,
                void                      **ppsCustomDataStructure);

PUBLIC uint16 u16ZCL_WriteCommandHeader(
                PDUM_thAPduInstance         PDUM_thAPduInstance,
                eFrameTypeSubfield          eFrameType,
                bool_t                      bManufacturerSpecific,
                uint16                      u16ManufacturerCode,
                bool_t                      bDirection,
                bool_t                      bDisableDefaultResponse,
                uint8                       u8TransactionSequenceNumber,
                uint8                       u8CommandIdentifier);

PUBLIC uint16 u16ZCL_ReadCommandHeader(PDUM_thAPduInstance  PDUM_thAPduInstance,
                                       tsZCL_HeaderParams   *psZCL_HeaderParams);


PUBLIC PDUM_thAPduInstance hZCL_AllocateAPduInstance(void);

PUBLIC teZCL_Status eZCL_BuildTransmitAddressStructure(
                ZPS_tsAfEvent             *pZPSevent,
                tsZCL_Address             *psZCL_Address);

PUBLIC teZCL_Status eZCL_TransmitDataRequest(
                PDUM_thAPduInstance        myPDUM_thAPduInstance,
                uint16                     u16PayloadSize,
                uint8                      u8SourceEndPointId,
                uint8                      u8DestinationEndPointId,
                uint16                     u16ClusterId,
                tsZCL_Address             *psDestinationAddress);

PUBLIC teZCL_Status eZCL_SendDefaultResponse(
                ZPS_tsAfEvent             *pZPSevent,
                teZCL_CommandStatus        eZCLcommandStatus);

PUBLIC teZCL_Status eZCL_GetAttributeTypeSizeFromBuffer(
                teZCL_ZCLAttributeType      eAttributeDataType,
                PDUM_thAPduInstance         hAPduInst,
                uint16                      u16inputOffset,
                uint16                     *pu16typeSize);

PUBLIC teZCL_Status eZCL_GetAttributeTypeSizeFromStructure(
                tsZCL_AttributeDefinition  *psAttributeDefinition,
                void                       *pvAttributeStructure,
                uint16                     *pu16typeSize);

PUBLIC uint16 u16ZCL_WriteAttributeValueIntoBuffer(
                uint8                      u8EndPointId,
                uint16                     u16AttributeId,
                tsZCL_ClusterInstance      *psClusterInstance,
                tsZCL_AttributeDefinition  *psAttributeDefinition,
                uint16                      u16offset,
                PDUM_thAPduInstance         PDUM_thAPduInstance);

PUBLIC uint16 u16ZCL_WriteAttributeValueIntoStructure(
                uint8                       u8EndPointId,
                uint16                      u16AttributeId,
                bool_t                      bIsManufacturerSpecific,
                bool_t                      bIsClientAttribute,
                tsZCL_ClusterInstance       *psClusterInstance,
                uint16                      u16offset,
                PDUM_thAPduInstance         PDUM_thAPduInstance);

PUBLIC uint16 u16ZCL_APduInstanceWriteNBO(
                PDUM_thAPduInstance         hAPduInst,
                uint16                      u16Pos,
                teZCL_ZCLAttributeType      eAttributeDataType,
                void                       *pvData);

PUBLIC uint16 u16ZCL_WriteTypeNBO(
                uint8                 *pu8Data,
                teZCL_ZCLAttributeType eAttributeDataType,
                void                  *pvData);

PUBLIC uint16 u16ZCL_APduInstanceReadNBO(
                PDUM_thAPduInstance         hAPduInst,
                uint16                      u16Pos,
                teZCL_ZCLAttributeType      eAttributeDataType,
                void                       *pvStruct);

PUBLIC uint16 u16ZCL_ReadTypeNBO(
                uint8                 *pu8Data,
                teZCL_ZCLAttributeType eAttributeDataType,
                uint8                 *pu8Struct);

PUBLIC uint16 u16ZCL_APduInstanceWriteStringNBO(
                PDUM_thAPduInstance         hAPduInst,
                uint16                      u16Pos,
                teZCL_ZCLAttributeType      eAttributeDataType,
                void                       *pvStringStructure);

PUBLIC uint16 u16ZCL_APduInstanceReadStringNBO(
                PDUM_thAPduInstance         hAPduInst,
                uint16                      u16Pos,
                teZCL_ZCLAttributeType      eAttributeDataType,
                void                       *pvStringStructure);

PUBLIC uint16 u16ZCL_GetStringAttributeTypeSizeFromBuffer(
                teZCL_ZCLAttributeType      eAttributeDataType,
                uint16                     *pu16stringDataLength,
                uint16                     *pu16stringDataLengthField,
                uint8                     **ppu8stringData,
                uint8                      *pu8stringLengthPosition);

PUBLIC uint16 u16ZCL_GetStringSizeAndBufferFromStructure(
                teZCL_ZCLAttributeType      eAttributeDataType,
                void                       *pvStringStruct,
                uint16                     *pu16stringDataLength,
                uint16                     *pu16stringLengthField,
                uint8                     **ppu8stringData);

PUBLIC uint8 u8GetTransactionSequenceNumber(void);

PUBLIC teZCL_Status eZCL_GetLocalAttributeValue(
                uint16                      u16AttributeId,
                bool_t                      bManufacturerSpecific,
                bool_t                      bIsClientAttribute,
                tsZCL_EndPointDefinition   *psEndPointDefinition,
                tsZCL_ClusterInstance      *psClusterInstance,
                void                       *pvAttributeValue);
PUBLIC teZCL_Status eZCL_SetLocalAttributeValue(
                    uint16                      u16AttributeId,
                    bool_t                      bManufacturerSpecific,
                    bool_t                      bIsClientAttribute,
                    tsZCL_EndPointDefinition   *psEndPointDefinition,
                    tsZCL_ClusterInstance      *psClusterInstance,
                    void                       *pvAttributeValue);
PUBLIC teZCL_Status eZCL_SetReceiveEventAddressStructure(
                ZPS_tsAfEvent              *pZPSevent,
                tsZCL_ReceiveEventAddress  *psReceiveEventAddress);

PUBLIC teZCL_Status eZCL_BuildClientTransmitAddressStructure(
                tsZCL_Address              *psZCL_Address,
                tsZCL_ReceiveEventAddress  *psReceiveEventAddress);
#ifndef COOPERATIVE
PUBLIC teZCL_Status eZCL_GetMutex(
                tsZCL_EndPointDefinition   *psEndPointDefinition);

PUBLIC teZCL_Status eZCL_ReleaseMutex(
                tsZCL_EndPointDefinition   *psEndPointDefinition);
#endif
PUBLIC teZCL_Status eZCL_SetCustomCallBackEvent(
                tsZCL_CallBackEvent        *psKECCallBackEvent,
                ZPS_tsAfEvent              *pZPSevent,
                uint8                       u8TransactionSequenceNumber,
                uint8                       u8EndPoint);

PUBLIC bool_t bZCL_AttributeTypeIsSigned(
                teZCL_ZCLAttributeType      eAttributeDataType);

PUBLIC void vZCL_MarkAttributeInvalid(
                teZCL_ZCLAttributeType      eAttributeDataType,
                void                       *pvAttribute);

PUBLIC teZCL_Status bZCL_CheckAttributeInvalid(
                teZCL_ZCLAttributeType      eAttributeDataType,
                void                       *pvAttribute);

PUBLIC teZCL_Status eZCL_CustomCommandSend(
                uint8                       u8SourceEndPointId,
                uint8                       u8DestinationEndPointId,
                tsZCL_Address              *psDestinationAddress,
                uint16                      u16ClusterId,
                bool_t                      bDirection,
                uint8                       u8CommandId,
                uint8                      *pu8TransactionSequenceNumber,
                tsZCL_TxPayloadItem        *psPayloadDefinition,
                bool_t                      bIsManufacturerSpecific,
                uint16                      u16ManufacturerCode,
                uint8                       u8ItemsInPayload);


PUBLIC teZCL_Status eZCL_CustomCommandReceive(
                ZPS_tsAfEvent              *pZPSevent,
                uint8                      *pu8TransactionSequenceNumber,
                tsZCL_RxPayloadItem        *psPayloadDefinition,
                uint8                       u8ItemsInPayload,
                uint8                       u8Flags);
PUBLIC teZCL_Status eZCL_CustomCommandDRLCReceive(
                ZPS_tsAfEvent              *pZPSevent,
                tsZCL_EndPointDefinition   *psEndPointDefinition,
                uint8                      *pu8TransactionSequenceNumber,
                tsZCL_RxPayloadItem        *psPayloadDefinition,
                uint8                       u8ItemsInPayload,
                uint8                       u8Flags);

PUBLIC teZCL_Status eZCL_CustomCommandInterPanReceive(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_RxPayloadItem         *psPayloadDefinition,
                    uint8                       u8ItemsInPayload,
                    uint16                      u16Offset,
                    uint8                       u8Flags);

PUBLIC uint8 u8ZCL_GetApsSequenceNumberOfLastTransmit();


PUBLIC teZCL_Status eZCL_TransmitInterPanRequest(
                        eInterPanTxOptions        eTxOption,
                        uint8                     u8Command,
                        uint8                     u8PayloadSize,
                        uint16                    u16ClusterId,
                        uint16                    u16Profile,
                        uint16                    u16ManufSpecific,
                        ZPS_tsInterPanAddress     *psDestinationAddress,
                        uint8                     *pu6Payload,
                        uint8                     *pu8SeqNum);

PUBLIC bool_t bZCL_SendCommandResponse( ZPS_tsAfEvent *pZPSevent);

PUBLIC teZCL_Status eZCL_SearchForAttributeEntry(
                        uint8                       u8EndpointId,
                        uint16                      u16AttributeEnum,
                        bool_t                      bIsManufacturerSpecific,
                        bool_t                      bIsClientAttribute,
                        tsZCL_ClusterInstance      *psClusterInstance,
                        tsZCL_AttributeDefinition **ppsAttributeDefinition,
                        uint16                     *pu16attributeIndex);

PUBLIC  void vZCL_SetDefaultReporting(
                    tsZCL_ClusterInstance       *psClusterInstance);                        
#if defined __cplusplus
}
#endif

#endif  /* ZCL_CUSTOM_COMMANDS_H_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
