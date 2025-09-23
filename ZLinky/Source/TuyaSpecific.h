/*
 * LixeeCluster.h
 *
 *  Created on: 11 March 2024
 *      Author: akila
 */

#ifndef LINKY_SOURCE_TUYASPECIFIC_H_
#define LINKY_SOURCE_TUYASPECIFIC_H_

#include <jendefs.h>
#include "zcl_options.h"
#include "zcl.h"



/* Command codes */
typedef enum
{
	/* TUYA COMMAND */
    E_CLD_TY_DATA_REQUEST    = 0x00,
	E_CLD_TY_DATA_RESPONSE,
	E_CLD_TY_DATA_REPORT,
	E_CLD_TY_DATA_QUERY,
	E_CLD_TUYA_MCU_VERSION_REQ = 0x10,
	E_CLD_TUYA_MCU_VERSION_RSP,
	E_CLD_TUYA_MCU_OTA_NOTIFY,
	E_CLD_TUYA_OTA_BLOCK_DATA_REQ,
	E_CLD_TUYA_OTA_BLOCK_DATA_RSP,
	E_CLD_TUYA_MCU_OTA_RESULT,
	E_CLD_TUYA_MCU_SYNC_TIME = 0x24,
} teCLD_Tuya_Command;




/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/* Cluster ID's */
#define TUYA_SPECIFIC_ID_LINKY                        0xEF00

typedef enum
{
	 E_CLD_TUYASPECIFIC_ATTR_ID_VERSION                = 0x0000, /* Mandatory */


} teCLD_TuyaSpecific_ClusterID;

typedef struct
{

	zuint8                      u8ApplicationVersion;

} tsCLD_TuyaSpecific;

/* Definition of TuyaSpecific Callback Event Structure */
typedef struct
{
    uint8                           u8CommandId;
} tsCLD_TuyaSpecificCallBackMessage;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC teZCL_Status eCLD_TuyaSpecificCreateLinky(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8                              *pu8AttributeControlBits);

PUBLIC teZCL_Status eCLD_TuyaCommandRspSend(
                uint8           u8SourceEndPointId,
                uint8           u8DestinationEndPointId,
                tsZCL_Address   *psDestinationAddress,
                uint8           *pu8TransactionSequenceNumber ,
				PDUM_thAPduInstance PduInst);

PUBLIC teZCL_Status eCLD_TuyaCommandMcuVerRspSend(
                uint8           u8SourceEndPointId,
                uint8           u8DestinationEndPointId,
                tsZCL_Address   *psDestinationAddress,
                uint8           *pu8TransactionSequenceNumber                );

PUBLIC teZCL_Status eCLD_TuyaCommandBasicMagidId(
                uint8           u8SourceEndPointId,
                uint8           u8DestinationEndPointId,
                tsZCL_Address   *psDestinationAddress,
                uint8           *pu8TransactionSequenceNumber                );

PUBLIC void SendTuyaReportCommand(uint8_t DP);

PUBLIC teZCL_Status eCLD_TuyaCommandPhaseSend( uint8_t DP,uint16_t Voltage, uint16_t Current,int16_t Power);

PUBLIC teZCL_Status eCLD_TuyaCommandUniversalSend(uint8_t type,  uint8_t datapoint, uint8_t* payload_data, uint16_t payload_size);

extern tsZCL_ClusterDefinition sCLD_TuyaSpecific;
extern const tsZCL_AttributeDefinition asCLD_TuyaSpecificLinkyClusterAttributeDefinitions[];
#endif /* LINKY_SOURCE_TUYASPECIFIC_H_ */

