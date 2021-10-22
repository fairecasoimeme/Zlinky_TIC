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
 * MODULE:              ZLL Commissoning Cluster
 *
 * COMPONENT:          zll_utility.h
 *
 * DESCRIPTION:        Header for  Cluster
 *
 *****************************************************************************/

#ifndef ZLL_UTILITY_H
#define ZLL_UTILITY_H

#include <jendefs.h>
#include "dlist.h"
#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_options.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/* Cluster ID's */
#define ZLL_CLUSTER_ID_UTILITY    0x1000
#define TL_MAX_GROUP_RECORDS          21
#define ZLL_MAX_ENDPOINT_RECORDS       8

#ifndef NUM_ENDPOINT_RECORDS
    #define NUM_ENDPOINT_RECORDS       1
#endif

#ifndef NUM_GROUP_RECORDS
    #define NUM_GROUP_RECORDS          4  
#endif

/****************************************************************************/
/*              Cluster - Optional Attributes                */
/*                                                                          */
/* Add the following #define's to your zcl_options.h file to add optional   */
/* attributes.                               */
/****************************************************************************/

/* There are no attributes */

/* End of optional attributes */

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef struct {
    uint16 u16NwkAddr;
    uint16 u16ProfileId;
    uint16 u16DeviceId;
    uint8 u8Endpoint;
    uint8 u8Version;
} tsZllEndpointInfoRecord;

typedef struct {
    uint8 u8NumRecords;
    uint8 u8CurrentLight;
    tsZllEndpointInfoRecord asEndpointRecords[NUM_ENDPOINT_RECORDS];
    uint8 au8PingCount[NUM_ENDPOINT_RECORDS];
} tsZllEndpointInfoTable;

typedef struct {
    uint16 u16GroupId;
    uint8 u8GroupType;
} tsZllGroupInfoRecord;

typedef struct {
    uint8 u8NumRecords;
    tsZllGroupInfoRecord asGroupRecords[NUM_GROUP_RECORDS];
} tsZllGroupInfoTable;

#if 1
/* Attribute ID's */
typedef enum 
{
    E_CLD_OH = 0x00,
    // there are no attributes
} teCLD_ZllUtility_ClusterID;

/* Identify Cluster */
typedef struct
{
    zbmap32     u32FeatureMap;
    zuint16     u16ClusterRevision;
} tsCLD_ZllUtility;


#endif

/* Command codes */
typedef enum 
{
    E_CLD_UTILITY_CMD_ENDPOINT_INFO = 0x40,          /* Mandatory (received by client) 0x40 */
    E_CLD_UTILITY_CMD_GET_GROUP_ID_REQ_RSP,          /* Mandatory (received by server) 0x41 */
    E_CLD_UTILITY_CMD_GET_ENDPOINT_LIST_REQ_RSP,     /* Mandatory (received by server) 0x42 */
} teCLD_ZllUtility_Command;

/* Endpoint Information command payload */
typedef struct
{
    uint64  u64IEEEAddr;
    uint16  u16NwkAddr;
    uint8   u8Endpoint;
    uint16  u16ProfileID;
    uint16  u16DeviceID;
    uint8   u8Version;
} tsCLD_ZllUtility_EndpointInformationCommandPayload;


/* Get Group identifiers request command payload */
typedef struct
{
    uint8   u8StartIndex;
} tsCLD_ZllUtility_GetGroupIdReqCommandPayload;

typedef struct {
    uint16  u16GroupID;
    uint8   u8GroupType;
} tsCLD_ZllGroupIdRecord;

/* Get Group identifiers Response command payload */
typedef struct
{
    uint8   u8Total;
    uint8   u8StartIndex;
    uint8   u8Count;
    tsCLD_ZllGroupIdRecord asGroupRecords[TL_MAX_GROUP_RECORDS];
} tsCLD_ZllUtility_GetGroupIdRspCommandPayload;

/* Get Endpoint List request command payload */
typedef struct
{
    uint8   u8StartIndex;
} tsCLD_ZllUtility_GetEndpointListReqCommandPayload;

typedef struct {
    uint16  u16NwkAddr;
    uint8   u8Endpoint;
    uint16  u16ProfileID;
    uint16  u16DeviceID;
    uint8   u8Version;
} tsCLD_ZllEndpointlistRecord;

/* Get Endpoint List Response command payload */
typedef struct
{
    uint8   u8Total;
    uint8   u8StartIndex;
    uint8   u8Count;
    tsCLD_ZllEndpointlistRecord asEndpointRecords[ZLL_MAX_ENDPOINT_RECORDS];
} tsCLD_ZllUtility_GetEndpointListRspCommandPayload;






/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/



PUBLIC teZCL_Status eCLD_ZllUtilityCommandEndpointInformationCommandSend(
                    uint8 u8SrcEndpoint,
                    uint8 u8DstEndpoint,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    tsCLD_ZllUtility_EndpointInformationCommandPayload *psPayload);

PUBLIC teZCL_Status eCLD_ZllUtilityCommandGetGroupIdReqCommandSend(
                    uint8   u8Srcendpoint,
                    uint8   u8DstEndpoint,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    uint8                       u8StartIndex);

PUBLIC teZCL_Status eCLD_ZllUtilityCommandGetGroupIdRspCommandSend(
                    uint8 u8SrcEndpoint,
                    uint8 u8DstEndpoint,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    uint8                       u8StartIndex);

PUBLIC teZCL_Status eCLD_ZllUtilityCommandGetEndpointListReqCommandSend(

                    uint8 u8SrcEndpoint,
                    uint8 u8DstEndpoint,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    uint8                       u8StartIndex);

PUBLIC teZCL_Status eCLD_ZllUtilityCommandGetEndpointListRspCommandSend(
                    uint8                       u8SrcEndpoint,
                    uint8                       u8DstEndpoint,
                    tsZCL_Address               *psDestinationAddress,
                    uint8                       *pu8TransactionSequenceNumber,
                    uint8                       u8StartIndex);

PUBLIC teZCL_Status eCLD_ZllUtilityCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);


/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/


extern const tsZCL_AttributeDefinition asCLD_ZllUtilityClusterAttributeDefinitions[];
extern tsZCL_ClusterDefinition sCLD_ZllUtility;
extern uint8 au8ZllUtilityAttributeControlBits[];

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* ZLL_UTILITY_H */
