/*
 * TuyaSpecific.c
 *
 *  Created on: 11 March. 2024
 *      Author: akila
 */


/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>
#include "zcl.h"
#include "TuyaSpecific.h"
#include "TuyaSpecific_internal.h"
#include "zcl_options.h"

#include "base_device.h"
#include "App_Linky.h"
#include "dbg.h"
#include "app_main.h"

extern tsZHA_BaseDevice sBaseDevice;

const tsZCL_AttributeDefinition asCLD_TuyaSpecificLinkyClusterAttributeDefinitions[] = {

	{E_CLD_TUYASPECIFIC_ATTR_ID_VERSION,             E_ZCL_AF_RD,                E_ZCL_UINT8,    (uint32)(&((tsCLD_TuyaSpecific*)(0))->u8ApplicationVersion),0},  /* Mandatory */


};

tsZCL_ClusterDefinition sCLD_TuyaSpecific = {
        TUYA_SPECIFIC_ID_LINKY,
        FALSE,
        E_ZCL_SECURITY_NETWORK,
        (sizeof(asCLD_TuyaSpecificLinkyClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
        (tsZCL_AttributeDefinition*)asCLD_TuyaSpecificLinkyClusterAttributeDefinitions,
        NULL
};


/****************************************************************************
 *
 * NAME:       eCLD_TuyaSpecificCreateLinky
 *
 * DESCRIPTION:
 * Creates a TuyaSpecific cluster
 *
 * PARAMETERS:  Name                         Usage
 *              psClusterInstance            Cluster structure
 *              bIsServer                    Server/Client Flag
 *              psClusterDefinition          Cluster Definitation
 *              pvEndPointSharedStructPtr    EndPoint Shared Structure Pointer
 *              pu8AttributeControlBits      Attribute Control Bits
 * RETURN:
 * teZCL_Status
 *
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_TuyaSpecificCreateLinky(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8                              *pu8AttributeControlBits)
{
    #ifdef STRICT_PARAM_CHECK
        /* Parameter check */
        if((psClusterInstance==NULL) ||
           (psClusterDefinition==NULL)  )
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif

    /* Create an instance of a basic cluster */
    vZCL_InitializeClusterInstance(
           psClusterInstance,
           bIsServer,
           psClusterDefinition,
           pvEndPointSharedStructPtr,
           pu8AttributeControlBits,
           NULL,
		   eCLD_TuyaSpecificCommandHandler);

    if(pvEndPointSharedStructPtr  != NULL)
    {
    	((tsCLD_TuyaSpecific*)pvEndPointSharedStructPtr)->u8ApplicationVersion = 0x00;
    }

    return E_ZCL_SUCCESS;

}


PUBLIC void SendTuyaReportCommand(uint8_t DP)
{
	switch(DP)
	{
		case 0x09:
		{

			teZCL_Status e_status;
			uint32 totalApparentPower =0;
			/*totalApparentPower = sBaseDevice.sElectricalMeasurement.u16ApparentPower +
						sBaseDevice.sElectricalMeasurement.u16ApparentPowerPhB +
						sBaseDevice.sElectricalMeasurement.u16ApparentPowerPhC;*/

			totalApparentPower = sBaseDevice.sElectricalMeasurement.u32TotalApparentPower;

			e_status = eCLD_TuyaCommandTotalPowerSend(totalApparentPower);
		}
		break;
		case 0x01:
		{
			uint32_t totalConsumption;
			if ((sBaseDevice.sLinkyServerCluster.au8LinkyMode == 0) || (sBaseDevice.sLinkyServerCluster.au8LinkyMode == 2) )
			{
				totalConsumption =         	((uint32_t)sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier1SummationDelivered / 10) +
											((uint32_t)sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier2SummationDelivered / 10) +
											((uint32_t)sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier3SummationDelivered / 10) +
											((uint32_t)sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier4SummationDelivered / 10) +
											((uint32_t)sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier5SummationDelivered / 10) +
											((uint32_t)sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier6SummationDelivered / 10) +
											((uint32_t)sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier7SummationDelivered / 10) +
											((uint32_t)sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier8SummationDelivered / 10) +
											((uint32_t)sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier9SummationDelivered / 10) +
											((uint32_t)sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier10SummationDelivered / 10)

											;
			}else{
				totalConsumption =  (uint32_t)sBaseDevice.sSimpleMeteringServerCluster.u48CurrentSummationDelivered / 10;
			}
			DBG_vPrintf(1,"\r\n----------------------TUYA SendTuyaReportCommand u48StartTotalConsumption: %ld",u48StartTuyaTotalConsumption);
			if (u48StartTuyaTotalConsumption >0)
			{
				totalConsumption = totalConsumption - (u48StartTuyaTotalConsumption/ 10);
			}

			eCLD_TuyaCommandActivePowerSend(totalConsumption);
			DBG_vPrintf(1,"\r\n----------------------TUYA SendTuyaReportCommand u48CurrentSummationDelivered: %ld",totalConsumption);
		}
		break;
		case 0x06:
		{
			uint16_t voltage;
			uint16_t Current;
			uint16_t Power;
			if ((sBaseDevice.sLinkyServerCluster.au8LinkyMode == 0) || (sBaseDevice.sLinkyServerCluster.au8LinkyMode == 2) )
			{
				voltage=0;
				Current = sBaseDevice.sElectricalMeasurement.u16RMSCurrent * 1000;
				Power = 0;
			}else{
				voltage = sBaseDevice.sElectricalMeasurement.u16RMSVoltage*10;
				Current = sBaseDevice.sElectricalMeasurement.u16RMSCurrent * 1000;
				Power = sBaseDevice.sElectricalMeasurement.u16ApparentPower;
				//calcul de l'intensité'
				if (voltage>0)
					Current = (Power*1000) / sBaseDevice.sElectricalMeasurement.u16RMSVoltage  ;
			}
		    eCLD_TuyaCommandPhaseSend(0x06,voltage,Current,Power);
			DBG_vPrintf(1,"\r\n----------------------TUYA eCLD_TuyaCommandPhase1Send Voltage : %d - Current : %d - Power : %d",voltage,Current,Power);

		}
		break;
		case 0x07:
		{
			uint16_t voltage;
			uint16_t Current;
			uint16_t Power;
			if ((sBaseDevice.sLinkyServerCluster.au8LinkyMode == 0) || (sBaseDevice.sLinkyServerCluster.au8LinkyMode == 2) )
			{
				voltage=0;
				Current = sBaseDevice.sElectricalMeasurement.u16RMSCurrentPhB * 1000;
				Power = 0;
			}else{
				voltage = sBaseDevice.sElectricalMeasurement.u16RMSVoltagePhB*10;
				Current = sBaseDevice.sElectricalMeasurement.u16RMSCurrentPhB * 1000;
				Power = sBaseDevice.sElectricalMeasurement.u16ApparentPowerPhB;
				//calcul de l'intensité'
				if (voltage>0)
					Current = (Power*1000) / sBaseDevice.sElectricalMeasurement.u16RMSVoltage  ;
			}
			eCLD_TuyaCommandPhaseSend(0x07,voltage,Current,Power);

		}
		break;
		case 0x08:
		{
			uint16_t voltage;
			uint16_t Current;
			uint16_t Power;
			if ((sBaseDevice.sLinkyServerCluster.au8LinkyMode == 0) || (sBaseDevice.sLinkyServerCluster.au8LinkyMode == 2) )
			{
				voltage=0;
				Current = sBaseDevice.sElectricalMeasurement.u16RMSCurrentPhC * 1000;
				Power = 0;
			}else{
				voltage = sBaseDevice.sElectricalMeasurement.u16RMSVoltagePhC*10;
				Current = sBaseDevice.sElectricalMeasurement.u16RMSCurrentPhC * 1000;
				Power = sBaseDevice.sElectricalMeasurement.u16ApparentPowerPhC;
				if (voltage>0)
					Current = (Power*1000) / sBaseDevice.sElectricalMeasurement.u16RMSVoltage  ;
			}
			eCLD_TuyaCommandPhaseSend(0x08,voltage,Current,Power);

		}
		break;

	}
}






