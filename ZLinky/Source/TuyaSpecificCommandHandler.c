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
#include "app_common.h"
#include "TuyaSpecific.h"
#include "zcl_options.h"
#include "App_Linky.h"
#include "dbg.h"
#include "TuyaSpecific_internal.h"
#include "PDM_IDs.h"
extern tsLinkyParams sLinkyParams;

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


PUBLIC teZCL_Status eCLD_TuyaCommandRspSend(
                uint8           u8SourceEndPointId,
                uint8           u8DestinationEndPointId,
                tsZCL_Address   *psDestinationAddress,
                uint8           *pu8TransactionSequenceNumber,
				PDUM_thAPduInstance PduInst
				)
{
	teZCL_Status e_status;
	// get payload size
	uint16 u16PayloadSize;
	u16PayloadSize = PDUM_u16APduInstanceGetPayloadSize(PduInst);

	uint8 *pu8PayloadData = (uint8 *)(PDUM_pvAPduInstanceGetPayload(PduInst));

	static uint8 u8ResponseStatus = 0x00;
	static uint8 u8seq;
	uint8 tmp = pu8PayloadData[4];
	u8seq= tmp + 1;

	tsZCL_TxPayloadItem asPayloadDefinition[u16PayloadSize-3];
	uint8 u8ItemCount = 0;
	asPayloadDefinition[u8ItemCount++] = (tsZCL_TxPayloadItem){1, E_ZCL_UINT8, &u8ResponseStatus};
	asPayloadDefinition[u8ItemCount++] = (tsZCL_TxPayloadItem){1, E_ZCL_UINT8, &u8seq};

	//DP
	uint8_t DP = pu8PayloadData[5];
	DBG_vPrintf(TRUE, "DP : %02X -", DP );

	//type
	uint8_t type = pu8PayloadData[6];
	DBG_vPrintf(TRUE, "type : %02X -", type );
	//size
	uint16_t size = ZNC_RTN_U16(pu8PayloadData, 7);
	DBG_vPrintf(TRUE, "size : %04X -", size );

	for (int i = 5; i < u16PayloadSize; i++) {
		asPayloadDefinition[u8ItemCount++] = (tsZCL_TxPayloadItem){1, E_ZCL_UINT8, &pu8PayloadData[i]};

	}

	switch (DP)
	{
		case 0x11:
			sLinkyParams.bHighPowerAlarm = (bool_t)pu8PayloadData[10];
			sLinkyParams.u16HighPowerValue = ZNC_RTN_U16(pu8PayloadData,11);
			break;
		case 0x12:
			sLinkyParams.bOverCurrentAlarm = (bool_t)pu8PayloadData[10];
			sLinkyParams.u16OverCurrentValue = ZNC_RTN_U16(pu8PayloadData, 11);

			sLinkyParams.bOverVoltageAlarm = (bool_t)pu8PayloadData[14];
			sLinkyParams.u16OverVoltageValue = ZNC_RTN_U16(pu8PayloadData, 15);

			sLinkyParams.bUnderVoltageAlarm = (bool_t)pu8PayloadData[18];
			sLinkyParams.u16UnderVoltageValue = ZNC_RTN_U16(pu8PayloadData, 19);
			break;

	}

	PDM_eSaveRecordDataInIdleTask(PDM_ID_APP_LINKY_PARAM,
											&sLinkyParams,
											sizeof(tsLinkyParams));

	au8TuyaSpecificSequenceNumber++;
    e_status = eZCL_CustomCommandSend(u8SourceEndPointId,
                                      u8DestinationEndPointId,
                                      psDestinationAddress,
                                      TUYA_SPECIFIC_ID_LINKY,
                                      TRUE,
									  E_CLD_TY_DATA_REPORT,
                                      pu8TransactionSequenceNumber,
                                      asPayloadDefinition,
                                      FALSE,
                                      0,
                                      sizeof(asPayloadDefinition) / sizeof(tsZCL_TxPayloadItem)
                                     );

    return e_status;
}


PUBLIC teZCL_Status eCLD_TuyaCommandMcuVerRspSend(
                uint8           u8SourceEndPointId,
                uint8           u8DestinationEndPointId,
                tsZCL_Address   *psDestinationAddress,
                uint8           *pu8TransactionSequenceNumber                )
{
	uint8_t version = 0x49;
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


/*PUBLIC teZCL_Status eCLD_TuyaCommandUniversalSend(uint8_t type,  uint8_t datapoint, uint8_t* payload_data, uint16_t payload_size)
{
    tsZCL_Address sAddress;
    sAddress.eAddressMode = E_ZCL_AM_SHORT;
    sAddress.uAddress.u16DestinationAddress = 0x0000;


    uint8_t status = 0x00;
    uint8 u8Seq = u8GetTransactionSequenceNumber();

    // Convertir la longueur en big-endian (2 bytes)
	uint8_t ln_bytes[2];
	ln_bytes[0] = (payload_size >> 8) & 0xFF;  // byte de poids fort
	ln_bytes[1] = payload_size & 0xFF;


    // Allouer dynamiquement le tableau de définition du payload
    uint8_t num_items = 5 + (payload_size > 0 ? 1 : 0); // 5 éléments fixes + 1 pour les données si présentes
    tsZCL_TxPayloadItem* asPayloadDefinition = malloc(num_items * sizeof(tsZCL_TxPayloadItem));

    if (asPayloadDefinition == NULL) {
        return E_ZCL_ERR_INSUFFICIENT_SPACE;
    }

    // Remplir les éléments fixes
    asPayloadDefinition[0] = (tsZCL_TxPayloadItem){1, E_ZCL_UINT8, &status};
    asPayloadDefinition[1] = (tsZCL_TxPayloadItem){1, E_ZCL_UINT8, &au8TuyaSpecificSequenceNumber};
    asPayloadDefinition[2] = (tsZCL_TxPayloadItem){1, E_ZCL_UINT8, &datapoint};
    asPayloadDefinition[3] = (tsZCL_TxPayloadItem){1, E_ZCL_UINT8, &type};
    asPayloadDefinition[4] = (tsZCL_TxPayloadItem){2, E_ZCL_UINT8, ln_bytes};

    // Ajouter les données si présentes
    if (payload_size > 0 && payload_data != NULL) {
        asPayloadDefinition[5] = (tsZCL_TxPayloadItem){payload_size, E_ZCL_UINT8, payload_data};
    }

    au8TuyaSpecificSequenceNumber++;
    teZCL_Status result;
    result = eZCL_CustomCommandSend(0x01,
                                               0x01,
                                               &sAddress,
                                               TUYA_SPECIFIC_ID_LINKY,
                                               TRUE,
                                               E_CLD_TY_DATA_REPORT,
                                               &u8Seq,
                                               asPayloadDefinition,
                                               FALSE,
                                               0,
                                               num_items);
    DBG_vPrintf(TRUE, "result : %d\n", result);
    // Libérer la mémoire allouée
    free(asPayloadDefinition);

    return result;
}*/

PUBLIC teZCL_Status eCLD_TuyaCommandUniversalSend(uint8_t type, uint8_t datapoint, uint8_t* payload_data, uint16_t payload_size)
{
    tsZCL_Address sAddress;
    sAddress.eAddressMode = E_ZCL_AM_SHORT;
    sAddress.uAddress.u16DestinationAddress = 0x0000;

    // Variables statiques pour éviter les problèmes de pointeurs
    static uint8_t s_status = 0x00;
    static uint8_t s_sequence;
    static uint8_t s_datapoint;
    static uint8_t s_type;
    static uint8_t s_ln_bytes[2];

    // Copier les valeurs dans les variables statiques
    s_sequence = au8TuyaSpecificSequenceNumber;
    s_datapoint = datapoint;
    s_type = type;
    s_ln_bytes[0] = (payload_size >> 8) & 0xFF;  // byte de poids fort
    s_ln_bytes[1] = payload_size & 0xFF;         // byte de poids faible

    // Incrémenter AVANT utilisation pour éviter les race conditions
    au8TuyaSpecificSequenceNumber++;

    // Utiliser un tableau statique au lieu de malloc
    static tsZCL_TxPayloadItem asPayloadDefinition[6]; // Taille maximale possible

    // Remplir les éléments fixes avec les variables statiques
    asPayloadDefinition[0] = (tsZCL_TxPayloadItem){1, E_ZCL_UINT8, &s_status};
    asPayloadDefinition[1] = (tsZCL_TxPayloadItem){1, E_ZCL_UINT8, &s_sequence};
    asPayloadDefinition[2] = (tsZCL_TxPayloadItem){1, E_ZCL_UINT8, &s_datapoint};
    asPayloadDefinition[3] = (tsZCL_TxPayloadItem){1, E_ZCL_UINT8, &s_type};
    asPayloadDefinition[4] = (tsZCL_TxPayloadItem){2, E_ZCL_UINT8, s_ln_bytes};

    uint8_t num_items = 5;

    // Ajouter les données si présentes
    if (payload_size > 0 && payload_data != NULL)
    {
        // Vérifier la taille pour éviter les débordements
        if (payload_size > 255)
        {
            DBG_vPrintf(TRUE, "ERROR: Payload too large: %d", payload_size);
            return E_ZCL_ERR_INSUFFICIENT_SPACE;
        }

        asPayloadDefinition[5] = (tsZCL_TxPayloadItem){payload_size, E_ZCL_UINT8, payload_data};
        num_items = 6;
    }

    uint8 u8Seq = u8GetTransactionSequenceNumber();


    teZCL_Status result = eZCL_CustomCommandSend(
        0x01,                           // u8SourceEndPointId
        0x01,                           // u8DestinationEndPointId
        &sAddress,                      // psDestinationAddress
        TUYA_SPECIFIC_ID_LINKY,         // u16ClusterId
        TRUE,                           // bDirection
        E_CLD_TY_DATA_REPORT,           // u8CommandId
        &u8Seq,                         // pu8TransactionSequenceNumber
        asPayloadDefinition,            // psPayloadDefinition
        FALSE,                          // bIsManufacturerSpecific
        0,                              // u16ManufacturerCode
        num_items                       // u8ItemsInPayload
    );

    DBG_vPrintf(TRUE, "eZCL_CustomCommandSend result: %d", result);

    return result;
}

PUBLIC teZCL_Status eCLD_TuyaCommandPhaseSend( uint8_t DP, uint16_t Voltage, uint16_t Current, int16 Power)
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

	powerData[0] = (Power >> 16) & 0xFF;
	powerData[1] = (Power >> 8) & 0xFF;
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
			DBG_vPrintf(1,"\r\n----------------------TUYA E_CLD_TY_DATA_REQUEST");

			tsZCL_Address   sAddress;
			sAddress.eAddressMode = pZPSevent->uEvent.sApsDataIndEvent.u8SrcAddrMode;
			sAddress.uAddress.u16DestinationAddress = pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr;

			eCLD_TuyaCommandRspSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
											pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
											&sAddress,
											&sZCL_HeaderParams.u8TransactionSequenceNumber,
											pZPSevent->uEvent.sApsDataIndEvent.hAPduInst);

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
