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
extern tsLinkyParams sLinkyParams;

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
		case 0x1d:
		{

			teZCL_Status e_status;
			int32 totalApparentPower =0;

			uint32_t totalInjectPower = (uint32_t)sBaseDevice.sLinkyServerCluster.u16LinkySINSTI;

			if (totalInjectPower > 0)
			{
				totalApparentPower = sBaseDevice.sElectricalMeasurement.u16ApparentPower +
									sBaseDevice.sElectricalMeasurement.u16ApparentPowerPhB +
									sBaseDevice.sElectricalMeasurement.u16ApparentPowerPhC -
									totalInjectPower;

			}else{

				totalApparentPower = sBaseDevice.sElectricalMeasurement.u16ApparentPower +
										sBaseDevice.sElectricalMeasurement.u16ApparentPowerPhB +
										sBaseDevice.sElectricalMeasurement.u16ApparentPowerPhC ;

				if ((sBaseDevice.sElectricalMeasurement.u16ApparentPower == 0) && (sBaseDevice.sElectricalMeasurement.u16RMSCurrent > 0))
				{
					totalApparentPower = (int32)totalApparentPower - (int32)(sBaseDevice.sElectricalMeasurement.u16RMSCurrent * sBaseDevice.sElectricalMeasurement.u16RMSVoltage);
				}
				if ((sBaseDevice.sElectricalMeasurement.u16ApparentPowerPhB == 0) && (sBaseDevice.sElectricalMeasurement.u16RMSCurrentPhB > 0))
				{
					totalApparentPower = (int32)totalApparentPower - (int32)(sBaseDevice.sElectricalMeasurement.u16RMSCurrentPhB * sBaseDevice.sElectricalMeasurement.u16RMSVoltagePhB);
				}
				if ((sBaseDevice.sElectricalMeasurement.u16ApparentPowerPhC == 0) && (sBaseDevice.sElectricalMeasurement.u16RMSCurrentPhC > 0))
				{
					totalApparentPower = (int32)totalApparentPower - (int32)(sBaseDevice.sElectricalMeasurement.u16RMSCurrentPhC * sBaseDevice.sElectricalMeasurement.u16RMSVoltagePhC);
				}
			}

			uint8_t payload_data[] = {(totalApparentPower >> 24) & 0xFF,(totalApparentPower >> 16) & 0xFF,(totalApparentPower >> 8) & 0xFF,totalApparentPower  & 0xFF};
			uint16_t payload_size = sizeof(payload_data);
			e_status = eCLD_TuyaCommandUniversalSend(0x02,DP, payload_data ,payload_size);

		}
		break;
		case 0x17:
		{

			teZCL_Status e_status;
			uint32 TotalIndexDelivered =0;

			TotalIndexDelivered = (uint32_t)sBaseDevice.sSimpleMeteringServerCluster.u48CurrentSummationReceived / 10;

			uint8_t payload_data[] = {(TotalIndexDelivered >> 24) & 0xFF,(TotalIndexDelivered >> 16) & 0xFF,(TotalIndexDelivered >> 8) & 0xFF,TotalIndexDelivered  & 0xFF};
			uint16_t payload_size = sizeof(payload_data);
			e_status = eCLD_TuyaCommandUniversalSend(0x02,DP, payload_data ,payload_size);
		}
		break;
		case 0x32:
		{

			teZCL_Status e_status;
			uint8_t payload_data[] = {0x00,0x00,0x00,0x64};
			uint16_t payload_size = sizeof(payload_data);
			e_status = eCLD_TuyaCommandUniversalSend(0x02,DP, payload_data ,payload_size);

		}
		break;
		case 0x20:
		{

			teZCL_Status e_status;
			uint8_t payload_data[] = {0x00,0x00,0x13,0x88};
			uint16_t payload_size = sizeof(payload_data);
			e_status = eCLD_TuyaCommandUniversalSend(0x02,DP, payload_data ,payload_size);

		}
		break;
		case 0x65:
		{

			teZCL_Status e_status;
			uint8_t payload_data[] = {0x00};
			uint16_t payload_size = sizeof(payload_data);
			e_status = eCLD_TuyaCommandUniversalSend(0x01,DP, payload_data ,payload_size);

		}
		break;
		case 0x66:
		{

			teZCL_Status e_status;
			uint8_t payload_data[] = {0x00,0x00,0x00,0x30};
			uint16_t payload_size = sizeof(payload_data);
			e_status = eCLD_TuyaCommandUniversalSend(0x02,DP, payload_data ,payload_size);

		}
		break;
		case 0x67:
		{

			teZCL_Status e_status;
			int32_t tmp = (int32)(sBaseDevice.sElectricalMeasurement.u16RMSVoltage *10);
			uint8_t payload_data[] = {(tmp >> 24) & 0xFF,(tmp >> 16) & 0xFF,(tmp >> 8) & 0xFF,tmp  & 0xFF};
			uint16_t payload_size = sizeof(payload_data);
			e_status = eCLD_TuyaCommandUniversalSend(0x02,DP, payload_data ,payload_size);

		}
		break;
		case 0x68:
		{

			teZCL_Status e_status;
			int32_t tmp = (int32)(sBaseDevice.sElectricalMeasurement.u16RMSCurrent *1000);
			uint8_t payload_data[] = {(tmp >> 24) & 0xFF,(tmp >> 16) & 0xFF,(tmp >> 8) & 0xFF,tmp  & 0xFF};
			uint16_t payload_size = sizeof(payload_data);
			e_status = eCLD_TuyaCommandUniversalSend(0x02,DP, payload_data ,payload_size);

		}
		break;
		case 0x69:
		{

			teZCL_Status e_status;
			int32_t tmp = (int32)(sBaseDevice.sElectricalMeasurement.u16ApparentPower);
			uint8_t payload_data[] = {(tmp >> 24) & 0xFF,(tmp >> 16) & 0xFF,(tmp >> 8) & 0xFF,tmp  & 0xFF};
			uint16_t payload_size = sizeof(payload_data);
			e_status = eCLD_TuyaCommandUniversalSend(0x02,DP, payload_data ,payload_size);

		}
		break;
		case 0x6a:
		{

			teZCL_Status e_status;

			uint8_t payload_data[] = {0x00,0x00,0x00,0x00};
			uint16_t payload_size = sizeof(payload_data);
			e_status = eCLD_TuyaCommandUniversalSend(0x02,DP, payload_data ,payload_size);

		}
		break;
		case 0x6c:
		{

			teZCL_Status e_status;
			uint8_t payload_data[] = {0x00,0x00,0x00,0x64};
			uint16_t payload_size = sizeof(payload_data);
			e_status = eCLD_TuyaCommandUniversalSend(0x02,DP, payload_data ,payload_size);

		}
		break;
		case 0x6d:
		{

			teZCL_Status e_status;
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


			uint8_t payload_data[] = {(totalConsumption >> 24) & 0xFF,(totalConsumption >> 16) & 0xFF,(totalConsumption >> 8) & 0xFF,totalConsumption  & 0xFF};
			uint16_t payload_size = sizeof(payload_data);
			e_status = eCLD_TuyaCommandUniversalSend(0x02,DP, payload_data ,payload_size);

		}
		break;
		case 0x6e:
		{

			teZCL_Status e_status;
			uint8_t payload_data[] = {0x00,0x00,0x00,0x00};
			uint16_t payload_size = sizeof(payload_data);
			e_status = eCLD_TuyaCommandUniversalSend(0x02,DP, payload_data ,payload_size);

		}
		break;
		case 0x70:
		{

			teZCL_Status e_status;
			int32_t tmp = (int32)(sBaseDevice.sElectricalMeasurement.u16RMSVoltagePhB *10);
			uint8_t payload_data[] = {(tmp >> 24) & 0xFF,(tmp >> 16) & 0xFF,(tmp >> 8) & 0xFF,tmp  & 0xFF};
			uint16_t payload_size = sizeof(payload_data);
			e_status = eCLD_TuyaCommandUniversalSend(0x02,DP, payload_data ,payload_size);

		}
		break;
		case 0x71:
		{

			teZCL_Status e_status;
			int32_t tmp = (int32)(sBaseDevice.sElectricalMeasurement.u16RMSCurrentPhB *1000);
			uint8_t payload_data[] = {(tmp >> 24) & 0xFF,(tmp >> 16) & 0xFF,(tmp >> 8) & 0xFF,tmp  & 0xFF};
			uint16_t payload_size = sizeof(payload_data);
			e_status = eCLD_TuyaCommandUniversalSend(0x02,DP, payload_data ,payload_size);
		}
		break;
		case 0x72:
		{

			teZCL_Status e_status;
			int32_t tmp = (int32)(sBaseDevice.sElectricalMeasurement.u16ApparentPowerPhB);
			uint8_t payload_data[] = {(tmp >> 24) & 0xFF,(tmp >> 16) & 0xFF,(tmp >> 8) & 0xFF,tmp  & 0xFF};
			uint16_t payload_size = sizeof(payload_data);
			e_status = eCLD_TuyaCommandUniversalSend(0x02,DP, payload_data ,payload_size);

		}
		break;
		case 0x75:
		{

			teZCL_Status e_status;
			uint8_t payload_data[] = {0x00,0x00,0x00,0x64};
			uint16_t payload_size = sizeof(payload_data);
			e_status = eCLD_TuyaCommandUniversalSend(0x02,DP, payload_data ,payload_size);

		}
		break;
		case 0x79:
		{

			teZCL_Status e_status;
			int32_t tmp = (int32)(sBaseDevice.sElectricalMeasurement.u16RMSVoltagePhC *10);
			uint8_t payload_data[] = {(tmp >> 24) & 0xFF,(tmp >> 16) & 0xFF,(tmp >> 8) & 0xFF,tmp  & 0xFF};
			uint16_t payload_size = sizeof(payload_data);
			e_status = eCLD_TuyaCommandUniversalSend(0x02,DP, payload_data ,payload_size);

		}
		break;
		case 0x7a:
		{

			teZCL_Status e_status;
			int32_t tmp = (int32)(sBaseDevice.sElectricalMeasurement.u16RMSCurrentPhC *1000);
			uint8_t payload_data[] = {(tmp >> 24) & 0xFF,(tmp >> 16) & 0xFF,(tmp >> 8) & 0xFF,tmp  & 0xFF};
			uint16_t payload_size = sizeof(payload_data);
			e_status = eCLD_TuyaCommandUniversalSend(0x02,DP, payload_data ,payload_size);

		}
		break;
		case 0x7b:
		{

			teZCL_Status e_status;
			int32_t tmp = (int32)(sBaseDevice.sElectricalMeasurement.u16ApparentPowerPhC);
			uint8_t payload_data[] = {(tmp >> 24) & 0xFF,(tmp >> 16) & 0xFF,(tmp >> 8) & 0xFF,tmp  & 0xFF};
			uint16_t payload_size = sizeof(payload_data);
			//e_status = eCLD_TuyaCommandUniversalSend(0x02,DP, payload_data ,payload_size);

		}
		break;
		case 0x7e:
		{

			teZCL_Status e_status;
			uint8_t payload_data[] = {0x00,0x00,0x00,0x64};
			uint16_t payload_size = sizeof(payload_data);
			e_status = eCLD_TuyaCommandUniversalSend(0x02,DP, payload_data ,payload_size);

		}
		break;
		case 0x13:
		{

			teZCL_Status e_status;
			uint8_t payload_data[] = {0x64, 0x69, 0x6b, 0x32, 0x34, 0x30, 0x31, 0x30, 0x30, 0x30, 0x31, 0x00};
			uint16_t payload_size = sizeof(payload_data);
			e_status = eCLD_TuyaCommandUniversalSend(0x03,DP, payload_data ,payload_size);

		}
		break;
		case 0x11:
		{

			teZCL_Status e_status;
			uint8_t payload_data[] = {
				0x07, sLinkyParams.bHighPowerAlarm, ((sLinkyParams.u16HighPowerValue >> 8) & 0xFF),(sLinkyParams.u16HighPowerValue & 0xFF)
			};
			uint16_t payload_size = sizeof(payload_data);
			e_status = eCLD_TuyaCommandUniversalSend(0x00,DP, payload_data ,payload_size);

		}
		break;
		case 0x12:
		{

			teZCL_Status e_status;
			uint8_t payload_data[] = {
			    0x01, sLinkyParams.bOverCurrentAlarm, ((sLinkyParams.u16OverCurrentValue >> 8) & 0xFF),(sLinkyParams.u16OverCurrentValue & 0xFF),
			    0x03, sLinkyParams.bOverVoltageAlarm, ((sLinkyParams.u16OverVoltageValue >> 8) & 0xFF),(sLinkyParams.u16OverVoltageValue & 0xFF),  // bytes 9-12
			    0x04, sLinkyParams.bUnderVoltageAlarm, ((sLinkyParams.u16UnderVoltageValue >> 8) & 0xFF), (sLinkyParams.u16UnderVoltageValue & 0xFF) // bytes 13-16
			};
			uint16_t payload_size = sizeof(payload_data);
			e_status = eCLD_TuyaCommandUniversalSend(0x00,DP, payload_data ,payload_size);

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


			uint8_t payload_data[] = {(totalConsumption >> 24) & 0xFF,(totalConsumption >> 16) & 0xFF,(totalConsumption >> 8) & 0xFF,totalConsumption  & 0xFF};
			uint16_t payload_size = sizeof(payload_data);
			eCLD_TuyaCommandUniversalSend(0x02,DP, payload_data ,payload_size);
			DBG_vPrintf(1,"\r\n----------------------TUYA SendTuyaReportCommand u48CurrentSummationDelivered: %ld",totalConsumption);
		}
		break;
		case 0x06:
		{
			uint16_t voltage=0;
			uint16_t Current=0;
			int16_t Power=0;
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
				if ((sBaseDevice.sElectricalMeasurement.u16RMSVoltage>0) && (Power >0))
					Current = (Power*1000) / sBaseDevice.sElectricalMeasurement.u16RMSVoltage  ;
			}
		    eCLD_TuyaCommandPhaseSend(0x06,voltage,Current,Power);

			DBG_vPrintf(1,"\r\n----------------------TUYA eCLD_TuyaCommandPhase1Send Voltage : %d - Current : %d - Power : %d",voltage,Current,Power);

		}
		break;
		case 0x07:
		{
			uint16_t voltage=0;
			uint16_t Current=0;
			int16_t Power=0;
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
				if ((sBaseDevice.sElectricalMeasurement.u16RMSVoltagePhB>0) && (Power >0))
					Current = (Power*1000) / sBaseDevice.sElectricalMeasurement.u16RMSVoltagePhB  ;
			}
			eCLD_TuyaCommandPhaseSend(0x07,voltage,Current,Power);

		}
		break;
		case 0x08:
		{
			uint16_t voltage=0;
			uint16_t Current=0;
			int16_t Power=0;
			if ((sBaseDevice.sLinkyServerCluster.au8LinkyMode == 0) || (sBaseDevice.sLinkyServerCluster.au8LinkyMode == 2) )
			{
				voltage=0;
				Current = sBaseDevice.sElectricalMeasurement.u16RMSCurrentPhC * 1000;
				Power = 0;
			}else{
				voltage = sBaseDevice.sElectricalMeasurement.u16RMSVoltagePhC*10;
				Current = sBaseDevice.sElectricalMeasurement.u16RMSCurrentPhC * 1000;
				Power = sBaseDevice.sElectricalMeasurement.u16ApparentPowerPhC;
				if ((sBaseDevice.sElectricalMeasurement.u16RMSVoltagePhC>0) && (Power >0))
					Current = (Power*1000) / sBaseDevice.sElectricalMeasurement.u16RMSVoltagePhC  ;

			}
			eCLD_TuyaCommandPhaseSend(0x08,voltage,Current,Power);

		}
		break;


	}
}






