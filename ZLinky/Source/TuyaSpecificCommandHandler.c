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


/****************************************************************************
 *
 * MODULE:             Basic Cluster
 *
 * DESCRIPTION:
 * Message event handler functions
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>

#include "zcl.h"
#include "zcl_customcommand.h"

#include "TuyaSpecific.h"
#include "zcl_options.h"

#include "dbg.h"
#include "TuyaSpecific_internal.h"

uint8_t au8TuyaSpecificSequenceNumber = 0;

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
/***        Public Functions                                              ***/
/****************************************************************************/
PUBLIC teZCL_Status eCLD_TuyaCommandBasicMagidId(
                uint8           u8SourceEndPointId,
                uint8           u8DestinationEndPointId,
                tsZCL_Address   *psDestinationAddress,
                uint8           *pu8TransactionSequenceNumber                )
{
	uint16_t attribute = 0xFFFE;
	uint8_t type = 0x30;
	uint8_t value =0;
	tsZCL_TxPayloadItem asPayloadDefinition[] = {
				{1,      E_ZCL_UINT16,   &attribute},
				{1,      E_ZCL_UINT8,   &type},
				{1,      E_ZCL_UINT8,   &value},
												 };

	return eZCL_CustomCommandSend(u8SourceEndPointId,
									  u8DestinationEndPointId,
									  psDestinationAddress,
									  0x0000,
									  TRUE,
									  0x0a,
									  pu8TransactionSequenceNumber,
									  asPayloadDefinition,
									  FALSE,
									  0,
									  sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem)
									 );
}

PUBLIC teZCL_Status eCLD_TuyaCommandMcuVerRspSend(
                uint8           u8SourceEndPointId,
                uint8           u8DestinationEndPointId,
                tsZCL_Address   *psDestinationAddress,
                uint8           *pu8TransactionSequenceNumber                )
{
	uint8_t version = 0x40;
    uint8_t status = 0;
    tsZCL_TxPayloadItem asPayloadDefinition[] = {
                {1,      E_ZCL_UINT8,   &status},
                {1,      E_ZCL_UINT8,   &au8TuyaSpecificSequenceNumber},
				{1,      E_ZCL_UINT8,   &version},
                                                 };
    au8TuyaSpecificSequenceNumber++;
    return eZCL_CustomCommandSend(u8SourceEndPointId,
                                      u8DestinationEndPointId,
                                      psDestinationAddress,
                                      TUYA_SPECIFIC_ID_LINKY,
                                      TRUE,
                                      E_CLD_TUYA_MCU_VERSION_RSP,
                                      pu8TransactionSequenceNumber,
                                      asPayloadDefinition,
                                      FALSE,
                                      0,
                                      sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem)
                                     );
}



PUBLIC teZCL_Status eCLD_TuyaCommandTotalPowerSend( uint16_t ApparentPower )
{
	tsZCL_Address   sAddress;
	sAddress.eAddressMode = E_ZCL_AM_SHORT;
	sAddress.uAddress.u16DestinationAddress =0x0000;

	uint8_t status=0x00;
	uint8 u8Seq = u8GetTransactionSequenceNumber();
    uint8_t datapoint = 0x09;
    uint8_t type = 0x02;
    uint8_t function = 0x00;
    uint8_t ln = 0x04;
    uint8 data[4];
    data[0]=0x00;
    data[1]=0x00;
    data[2]=ApparentPower >> 8;
    data[3]=ApparentPower  & 0xFF;
    tsZCL_TxPayloadItem asPayloadDefinition[] = {
    												{1,      E_ZCL_UINT8,   &status},
													{1,      E_ZCL_UINT8,   &au8TuyaSpecificSequenceNumber},
													{1,      E_ZCL_UINT8,   &datapoint},
													{1,      E_ZCL_UINT8,   &type},
													{1,      E_ZCL_UINT8,   &function},
													{1,      E_ZCL_UINT8,   &ln},
    												{1,      E_ZCL_UINT32,   &data},
    											};
    au8TuyaSpecificSequenceNumber++;
    return eZCL_CustomCommandSend(0x01,
                                      0x01,
									  &sAddress,
                                      TUYA_SPECIFIC_ID_LINKY,
                                      TRUE,
                                      E_CLD_TY_DATA_REPORT,
									  &u8Seq,
                                      asPayloadDefinition,
                                      FALSE,
                                      0,
                                      sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem)
                                     );
}


PUBLIC teZCL_Status eCLD_TuyaCommandActivePowerSend( uint32_t index )
{
	tsZCL_Address   sAddress;
	sAddress.eAddressMode = E_ZCL_AM_SHORT;
	sAddress.uAddress.u16DestinationAddress =0x0000;

	uint8_t status=0x00;
	uint8 u8Seq = u8GetTransactionSequenceNumber();
    uint8_t datapoint = 0x01;
    uint8_t type = 0x02;
    uint8_t function = 0x00;
    uint8_t ln = 0x04;

    uint8 data[4];
    data[0] = index >> 24;
    data[1] = index >> 16;
    data[2] = index >> 8;
    data[3] = index  & 0xFF;

    DBG_vPrintf(1,"\r\n----------------------TUYA eCLD_TuyaCommandActivePowerSend: %d",data);
    tsZCL_TxPayloadItem asPayloadDefinition[] = {
    												{1,      E_ZCL_UINT8,   &status},
													{1,      E_ZCL_UINT8,   &au8TuyaSpecificSequenceNumber},
													{1,      E_ZCL_UINT8,   &datapoint},
													{1,      E_ZCL_UINT8,   &type},
													{1,      E_ZCL_UINT8,   &function},
													{1,      E_ZCL_UINT8,   &ln},
    												{1,      E_ZCL_UINT32,   &data},
    											};
    au8TuyaSpecificSequenceNumber++;
    return eZCL_CustomCommandSend(0x01,
                                      0x01,
									  &sAddress,
                                      TUYA_SPECIFIC_ID_LINKY,
                                      TRUE,
                                      E_CLD_TY_DATA_REPORT,
									  &u8Seq,
                                      asPayloadDefinition,
                                      FALSE,
                                      0,
                                      sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem)
                                     );
}

PUBLIC teZCL_Status eCLD_TuyaCommandPhaseSend( uint8_t DP, uint16_t Voltage, uint16_t Current,uint16_t Power)
{
	tsZCL_Address   sAddress;
	sAddress.eAddressMode = E_ZCL_AM_SHORT;
	sAddress.uAddress.u16DestinationAddress =0x0000;

	uint8_t status=0x00;
	uint8 u8Seq = u8GetTransactionSequenceNumber();
    uint8_t datapoint = DP;
    uint8_t type = 0x00;
    uint8_t function = 0x00;
    uint8_t ln = 0x08;

    uint8 voltageData[2];
    voltageData[0] = Voltage >> 8;
    voltageData[1] = Voltage  & 0xFF;

    uint8 currentData[3];
    currentData[0] = Current >> 16;
    currentData[1] = Current >> 8;
    currentData[2] = Current & 0xFF;

	uint8 powerData[3];
	powerData[0] = Power >> 16;
	powerData[1] = Power >> 8;
	powerData[2] = Power & 0xFF;

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
    												{1,      E_ZCL_UINT8,   &status},
													{1,      E_ZCL_UINT8,   &au8TuyaSpecificSequenceNumber},
													{1,      E_ZCL_UINT8,   &datapoint},
													{1,      E_ZCL_UINT8,   &type},
													{1,      E_ZCL_UINT8,   &function},
													{1,      E_ZCL_UINT8,   &ln},
    												{1,      E_ZCL_UINT16,   &voltageData},
													{1,      E_ZCL_UINT24,   &currentData},
													{1,      E_ZCL_UINT24,   &powerData},
    											};
    au8TuyaSpecificSequenceNumber++;
    return eZCL_CustomCommandSend(0x01,
                                      0x01,
									  &sAddress,
                                      TUYA_SPECIFIC_ID_LINKY,
                                      TRUE,
                                      E_CLD_TY_DATA_REPORT,
									  &u8Seq,
                                      asPayloadDefinition,
                                      FALSE,
                                      0,
                                      sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem)
                                     );
}
/****************************************************************************
 **
 ** NAME:       eCLD_TuyaSpecificCommandHandler
 **
 ** DESCRIPTION:
 ** Handles Message Cluster custom commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eCLD_TuyaSpecificCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{
    teZCL_Status eReturnStatus = E_ZCL_SUCCESS;
	tsZCL_HeaderParams sZCL_HeaderParams;
    // further error checking can only be done once we have interrogated the ZCL payload
    u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                             &sZCL_HeaderParams);

    // We don't handle messages as a client, so exit
    if(psClusterInstance->bIsServer == FALSE)
    {
        return(E_ZCL_FAIL);
    }

    /* 
     * The command is successfully qualified for all the errors , now take action 
     */
    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // SERVER
    switch(sZCL_HeaderParams.u8CommandIdentifier)
    {
    	/* tuya */
		case(E_CLD_TY_DATA_REQUEST):
		{
			//eCLD_BasicHandleResetToFactoryDefaultsCommand(pZPSevent, psEndPointDefinition, psClusterInstance);
			eReturnStatus = E_ZCL_SUCCESS;
			break;
		}

		case(E_CLD_TY_DATA_QUERY):
		{
			DBG_vPrintf(1,"\r\n----------------------TUYA E_CLD_TY_DATA_QUERY");

			eReturnStatus = E_ZCL_SUCCESS;
			break;
		}
		case(E_CLD_TUYA_MCU_VERSION_REQ):
		{
			DBG_vPrintf(1,"\r\n----------------------TUYA E_CLD_TUYA_MCU_VERSION_REQ");
			teZCL_Status e_status;
			tsZCL_Address   sAddress;
			sAddress.eAddressMode = pZPSevent->uEvent.sApsDataIndEvent.u8SrcAddrMode;
			sAddress.uAddress.u16DestinationAddress = pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr;

			e_status = eCLD_TuyaCommandMcuVerRspSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
											pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
											&sAddress,
											&sZCL_HeaderParams.u8TransactionSequenceNumber);
			DBG_vPrintf(1,"\r\n----------------------TUYA eCLD_TuyaCommandMcuVerRspSend : %d",e_status);
			eReturnStatus = E_ZCL_SUCCESS;
			break;
		}
		case(E_CLD_TUYA_MCU_SYNC_TIME):
		{
			eReturnStatus = E_ZCL_SUCCESS;
			break;
		}
		default:
		{
			eReturnStatus = E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR;
			break;
		}
    }

    // unlock and return
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif



    // delete the i/p buffer on return
    return(eReturnStatus);

}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
