/*****************************************************************************
 *
 * MODULE:             JN-AN-1220 ZLO Sensor Demo
 *
 * COMPONENT:          App_LightSensor.c
 *
 * DESCRIPTION:        ZLO Demo Light Sensor -Implementation
 *
 ****************************************************************************
 *
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5168, JN5169,
 * JN5179, JN5189].
 * You, and any third parties must reproduce the copyright and warranty notice
 * and any other legend of ownership on each copy or partial copy of the
 * software.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright NXP B.V. 2016-2019. All rights reserved
 *
 ***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/


#include <jendefs.h>
#include "zps_gen.h"
#include "fsl_wwdt.h"
#include "App_Linky.h"
#include "dbg.h"
#include <string.h>
#include "app_common.h"
#include "app_main.h"
#include "fsl_os_abstraction.h"
#include "app.h"
#include "fsl_usart.h"
#include "app_uartlinky.h"
//#include "metering_sensor.h"


extern uint8_t wdt_update_count;

uint32 pow10[9]={1,10,100,1000,10000,100000,1000000,10000000,100000000};

/****************************************************************************/
/***    Local Variables                           ***/
/****************************************************************************/
uint16             u16PacketType;
uint16             u16PacketLength;
uint8              au8Command[20];
uint8              au8Value[20];
uint8              loopOK;
uint32  		   u32Timeout=0;
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifdef DEBUG_LINKY
    #define TRACE_LINKY TRUE
#else
    #define TRACE_LINKY FALSE
#endif

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
const uint8 u8MyEndpoint = LINKY_SENSOR_ENDPOINT;
tsZLO_LinkyDevice sSensor;


/* define the default reports */
tsReports asDefaultReports[ZCL_NUMBER_OF_REPORTS] = \
{\
  /*  {SE_CLUSTER_ID_SIMPLE_METERING,{0, E_ZCL_UINT32,E_CLD_SM_ATTR_ID_CURRENT_SUMMATION_DELIVERED,ZLO_MIN_REPORT_INTERVAL,ZLO_MAX_REPORT_INTERVAL,0,{LINKY_MINIMUM_REPORTABLE_CHANGE}}},
	{SE_CLUSTER_ID_SIMPLE_METERING,{0, E_ZCL_UINT32,E_CLD_SM_ATTR_ID_CURRENT_TIER_2_SUMMATION_DELIVERED,ZLO_MIN_REPORT_INTERVAL,ZLO_MAX_REPORT_INTERVAL,0,{LINKY_MINIMUM_REPORTABLE_CHANGE}}},
	{SE_CLUSTER_ID_SIMPLE_METERING,{0, E_ZCL_UINT32,E_CLD_SM_ATTR_ID_CURRENT_TIER_3_SUMMATION_DELIVERED,ZLO_MIN_REPORT_INTERVAL,ZLO_MAX_REPORT_INTERVAL,0,{LINKY_MINIMUM_REPORTABLE_CHANGE}}},
*/
};

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: eApp_ZCL_RegisterEndpoint
 *
 * DESCRIPTION:
 * Register ZLO endpoints
 *
 * PARAMETER
 * Type                        Name                  Descirption
 * tfpZCL_ZCLCallBackFunction  fptr                  Pointer to ZCL Callback function
 *
 * RETURNS:
 * teZCL_Status
 *
 ****************************************************************************/
teZCL_Status eApp_ZCL_RegisterEndpoint(tfpZCL_ZCLCallBackFunction fptr)
{
    return eZLO_RegisterSimpleMeteringEndPoint(
    											LINKY_SENSOR_ENDPOINT,
                                              fptr,
                                              &sSensor);
}


/****************************************************************************
 *
 * NAME: vAPP_ZCL_DeviceSpecific_Init
 *
 * DESCRIPTION:
 * ZCL Device Specific initialization
 *
 * PARAMETER: void
 *
 * RETURNS: void
 *
 ****************************************************************************/
void vAPP_ZCL_DeviceSpecific_Init(void)
{
    /* Initialise the strings in Basic */
    memcpy(sSensor.sBasicServerCluster.au8ManufacturerName, "LiXee", CLD_BAS_MANUF_NAME_SIZE);
    memcpy(sSensor.sBasicServerCluster.au8ModelIdentifier, "ZLinky_TIC", CLD_BAS_MODEL_ID_SIZE);
    memcpy(sSensor.sBasicServerCluster.au8DateCode, "20210401", CLD_BAS_DATE_SIZE);
    memcpy(sSensor.sBasicServerCluster.au8SWBuildID, "4000-0001", CLD_BAS_SW_BUILD_SIZE);
    memcpy(sSensor.sBasicServerCluster.au8ProductURL, "LiXee.fr", CLD_BAS_URL_SIZE);
    memcpy(sSensor.sBasicServerCluster.au8ProductCode, "0001", CLD_BAS_PCODE_SIZE);

    memcpy(sSensor.sLinkyServerCluster.au8LinkyOptarif, "BASE",4);

    sSensor.sSimpleMeteringServerCluster.eMeteringDeviceType = E_CLD_SM_MDT_ELECTRIC;

}

/****************************************************************************
 *
 * NAME: vAPP_ZCL_DeviceSpecific_SetIdentifyTime
 *
 * DESCRIPTION:
 * ZCL Device Specific setting of identify time
 *
 * PARAMETER:
 * uint16 u16Time Identify time duration
 *
 * RETURNS: void
 *
 ****************************************************************************/
PUBLIC void vAPP_ZCL_DeviceSpecific_SetIdentifyTime(uint16 u16Time)
{
    sSensor.sIdentifyServerCluster.u16IdentifyTime=u16Time;
}
/****************************************************************************
 *
 * NAME: vAPP_ZCL_DeviceSpecific_UpdateIdentify
 *
 * DESCRIPTION:
 * ZCL Device Specific identify updates
 *
 * PARAMETER: void
 *
 * RETURNS: void
 *
 ****************************************************************************/
PUBLIC void vAPP_ZCL_DeviceSpecific_UpdateIdentify(void)
{
    if(sSensor.sIdentifyServerCluster.u16IdentifyTime%2)
    {
        GPIO_PinWrite(GPIO, 0, APP_BASE_BOARD_LED1_PIN, 0); // On
    }
    else
    {
        GPIO_PinWrite(GPIO, 0, APP_BASE_BOARD_LED1_PIN, 1); // Off
    }
}
/****************************************************************************
 *
 * NAME: vAPP_ZCL_DeviceSpecific_IdentifyOff
 *
 * DESCRIPTION:
 * ZCL Device Specific stop identify
 *
 * PARAMETER: void
 *
 * RETURNS: void
 *
 ****************************************************************************/
PUBLIC void vAPP_ZCL_DeviceSpecific_IdentifyOff(void)
{
    vAPP_ZCL_DeviceSpecific_SetIdentifyTime(0);
    GPIO_PinWrite(GPIO, 0, APP_BASE_BOARD_LED1_PIN, 1); // Off
}

/****************************************************************************
 *
 * NAME: APP_cbTimerLightSensorSample
 *
 * DESCRIPTION:
 * CallBack For Light sensor sampling
 *
 * PARAMETER: void
 *
 * RETURNS: void
 *
 ****************************************************************************/
PUBLIC void APP_cbTimerLinkySample(void *pvParam)
{
    /* Start Sampling timer only when you are keeping alive */
    if(APP_bPersistantPolling == TRUE)
        vAPP_LinkySensorSample();
}

uint8 APP_vProcessRxData ( void )
{
	uint8    u8RxByte;

	if (loopOK>=2)
		return 1;
	if (u32Timeout>100000)
		return 2;
	if ( ZQ_bQueueReceive ( &APP_msgSerialRx, &u8RxByte ) )
	{

		if( TRUE == bSL_ReadMessage(     MAX_PACKET_SIZE,
		                                 au8Command,
										 au8Value,
		                                 u8RxByte
		                               )
		      )
		    {
				u32Timeout=0;
				if (memcmp(au8Command,"ADCO",4)==0)
				{
					DBG_vPrintf(1, "\r\nADCO : %s",au8Value);

					memcpy(sSensor.sSimpleMeteringServerCluster.sMeterSerialNumber.pu8Data,au8Value,12);
					sSensor.sSimpleMeteringServerCluster.sMeterSerialNumber.u8Length=12;
					loopOK++;

				}else if(memcmp(au8Command,"MOTDETAT",8)==0)
				{
					DBG_vPrintf(1, "\r\nMOTDETAT : %s",au8Value);
				}else if(memcmp(au8Command,"BASE",4)==0)
				{
					DBG_vPrintf(1, "\r\nBASE : %s",au8Value);
					sSensor.sSimpleMeteringServerCluster.u32CurrentSummationDelivered=atol(au8Value);
				}else if(memcmp(au8Command,"HHPHC",5)==0)
				{
					DBG_vPrintf(1, "\r\nHHPHC : %s",au8Value);
				}else if(memcmp(au8Command,"OPTARIF",7)==0)
				{
					DBG_vPrintf(1, "\r\nOPTARIF : %s",au8Value);
					memcpy(sSensor.sLinkyServerCluster.au8LinkyOptarif, au8Value,4);
				}else if(memcmp(au8Command,"ISOUSC",6)==0)
				{
					DBG_vPrintf(1, "\r\nISOUSC : %s",au8Value);
					sSensor.sMeterIdentification.au16LinkyMIAvailablePower=atol(au8Value);

				}else if(memcmp(au8Command,"HCHC",4)==0)
				{
					DBG_vPrintf(1, "\r\nHCHC : %s",au8Value);
					sSensor.sSimpleMeteringServerCluster.u32CurrentTier2SummationDelivered=atol(au8Value);
					//DBG_vPrintf(1, "\r\nHCHC : %d",sSensor.sSimpleMeteringServerCluster.u32CurrentTier2SummationDelivered);
				}else if(memcmp(au8Command,"HCHP",4)==0)
				{
					DBG_vPrintf(1, "\r\nHCHP : %s",au8Value);
					sSensor.sSimpleMeteringServerCluster.u32CurrentTier1SummationDelivered=atol(au8Value);
					//DBG_vPrintf(1, "\r\nHCHP : %d",sSensor.sSimpleMeteringServerCluster.u32CurrentTier1SummationDelivered);
					/*uint32 tmp=0;
					int ln=0;
					int i=0;
					while (au8Value[i]>0)
					{
						ln=i;
						i++;
					}
					for(i=0;i<=ln;i++)
					{
						tmp+=(au8Value[ln-i]-48) * pow10[i] ;
					}

					DBG_vPrintf(1, "\r\nHCHP : %d",tmp);
					sSensor.sSimpleMeteringServerCluster.u32CurrentTier2SummationDelivered= tmp;
					DBG_vPrintf(1, "\r\nHCHP : %d",sSensor.sSimpleMeteringServerCluster.u32CurrentTier2SummationDelivered);*/
				}else if(memcmp(au8Command,"EJPHN",5)==0)
				{
					DBG_vPrintf(1, "\r\nEJPHN : %s",au8Value);
					sSensor.sSimpleMeteringServerCluster.u32CurrentTier1SummationDelivered=atol(au8Value);
				}else if(memcmp(au8Command,"EJPHPM",6)==0)
				{
					DBG_vPrintf(1, "\r\nEJPHPM : %s",au8Value);
					sSensor.sSimpleMeteringServerCluster.u32CurrentTier2SummationDelivered=atol(au8Value);
				}else if(memcmp(au8Command,"BBRHCJB",7)==0)
				{
					DBG_vPrintf(1, "\r\nBBRHCJB : %s",au8Value);
					sSensor.sSimpleMeteringServerCluster.u32CurrentTier1SummationDelivered=atol(au8Value);
				}else if(memcmp(au8Command,"BBRHPJB",7)==0)
				{
					DBG_vPrintf(1, "\r\nBBRHPJB : %s",au8Value);
					sSensor.sSimpleMeteringServerCluster.u32CurrentTier2SummationDelivered=atol(au8Value);
				}else if(memcmp(au8Command,"BBRHCJW",7)==0)
				{
					DBG_vPrintf(1, "\r\nBBRHCJW : %s",au8Value);
					sSensor.sSimpleMeteringServerCluster.u32CurrentTier3SummationDelivered=atol(au8Value);
				}else if(memcmp(au8Command,"BBRHPJW",7)==0)
				{
					DBG_vPrintf(1, "\r\nBBRHPJW : %s",au8Value);
					sSensor.sSimpleMeteringServerCluster.u32CurrentTier4SummationDelivered=atol(au8Value);
				}else if(memcmp(au8Command,"BBRHCJR",7)==0)
				{
					DBG_vPrintf(1, "\r\nBBRHCJR : %s",au8Value);
					sSensor.sSimpleMeteringServerCluster.u32CurrentTier5SummationDelivered=atol(au8Value);
				}else if(memcmp(au8Command,"BBRHPJR",7)==0)
				{
					DBG_vPrintf(1, "\r\nBBRHPJR : %s",au8Value);
					sSensor.sSimpleMeteringServerCluster.u32CurrentTier6SummationDelivered=atol(au8Value);
				}else if(memcmp(au8Command,"IINST",5)==0)
				{
					DBG_vPrintf(1, "\r\nIINST : %s",au8Value);
					sSensor.sElectricalMeasurement.u16RMSCurrent=atoi(au8Value);
					//DBG_vPrintf(1, "\r\nIINST : %ld",sSensor.sElectricalMeasurement.u16RMSCurrent);
				}else if(memcmp(au8Command,"IINST1",6)==0)
				{
					DBG_vPrintf(1, "\r\nIINST1 : %s",au8Value);
					sSensor.sElectricalMeasurement.u16RMSCurrent=atoi(au8Value);
				}else if(memcmp(au8Command,"IINST2",6)==0)
				{
					DBG_vPrintf(1, "\r\nIINST2 : %s",au8Value);
					sSensor.sElectricalMeasurement.u16RMSCurrentPhB=atoi(au8Value);
				}else if(memcmp(au8Command,"IINST3",6)==0)
				{
					DBG_vPrintf(1, "\r\nIINST3 : %s",au8Value);
					sSensor.sElectricalMeasurement.u16RMSCurrentPhC=atoi(au8Value);
				}else if(memcmp(au8Command,"IMAX",4)==0)
				{
					DBG_vPrintf(1, "\r\nIMAX : %s",au8Value);
					sSensor.sElectricalMeasurement.u16RMSCurrentMax=atoi(au8Value);
				}else if(memcmp(au8Command,"IMAX1",5)==0)
				{
					DBG_vPrintf(1, "\r\nIMAX1 : %s",au8Value);
					sSensor.sElectricalMeasurement.u16RMSCurrentMax=atoi(au8Value);
				}else if(memcmp(au8Command,"IMAX2",5)==0)
				{
					DBG_vPrintf(1, "\r\nIMAX2 : %s",au8Value);
					sSensor.sElectricalMeasurement.u16RMSCurrentMaxPhB=atoi(au8Value);
				}else if(memcmp(au8Command,"IMAX3",5)==0)
				{
					DBG_vPrintf(1, "\r\nIMAX3 : %s",au8Value);
					sSensor.sElectricalMeasurement.u16RMSCurrentMaxPhC=atoi(au8Value);
				}else if(memcmp(au8Command,"PMAX",4)==0)
				{
					DBG_vPrintf(1, "\r\nPMAX : %s",au8Value);
					sSensor.sElectricalMeasurement.i16ActivePowerMax=atoi(au8Value);
				}else if(memcmp(au8Command,"PAPP",4)==0)
				{
					DBG_vPrintf(1, "\r\nPAPP : %s",au8Value);
					sSensor.sElectricalMeasurement.u16ApparentPower=atoi(au8Value);

				}else if(memcmp(au8Command,"PTEC",4)==0)
				{
					DBG_vPrintf(1, "\r\nPTEC : %s",au8Value);
					memcpy(sSensor.sSimpleMeteringServerCluster.sActiveRegisterTierDelivered.pu8Data,au8Value,4);
					sSensor.sSimpleMeteringServerCluster.sActiveRegisterTierDelivered.u8Length=4;
				}else if(memcmp(au8Command,"ADPS",4)==0)
				{
					sSensor.sLinkyServerCluster.au16LinkyADPS=atoi(au8Value);

				}else if(memcmp(au8Command,"DEMAIN",6)==0)
				{
					memcpy(sSensor.sLinkyServerCluster.au8LinkyDemain, au8Value,4);

				}else if(memcmp(au8Command,"PPOT",4)==0)
				{
					sSensor.sLinkyServerCluster.au8LinkyPPOT=atoi(au8Value);

				}else if(memcmp(au8Command,"PEJP",4)==0)
				{
					sSensor.sLinkyServerCluster.au8LinkyPEJP=atoi(au8Value);
				}else if(memcmp(au8Command,"HHPHC",4)==0)
				{
					sSensor.sLinkyServerCluster.au8LinkyHHPHC=au8Value;
				}
		    }
	}else{

		u32Timeout++;
	}

#ifdef WATCHDOG_ALLOWED
	/* Kick the watchdog */
	WWDT_Refresh(WWDT);
#endif

	return 0;
}

/****************************************************************************
 *
 * NAME: vAPP_LinkySensorSample
 *
 * DESCRIPTION:
 * Linky sensor sampling
 *
 * PARAMETER: void
 *
 * RETURNS: void
 *
 ****************************************************************************/
PUBLIC void vAPP_LinkySensorSample(void)
{
	uint8 u8StatusLinky;
    loopOK=0;

    UARTLINKY_vInit();
    UARTLINKY_vSetBaudRate ( 1200 );

    DBG_vPrintf(TRACE_LINKY, "\r\n\r\n\r\nUARTLINKY_vInit\r\n\r\n\r\n");
    GPIO_PinWrite(GPIO, 0, APP_BASE_BOARD_LED1_PIN, 0); // OFF

    u32Timeout=0;
    while(TRUE)
    {
    	u8StatusLinky=APP_vProcessRxData();
    	if (u8StatusLinky>0)
    		break;

    	WWDT_Refresh(WWDT);
    	wdt_update_count = 0;

    }

    UARTLINKY_vDeInit();

    if (u8StatusLinky == 2)
    {
    	GPIO_PinInit(GPIO, 0, APP_BASE_BOARD_LED1_PIN, &((const gpio_pin_config_t){kGPIO_DigitalInput, 1}));
    	//GPIO_PinWrite(GPIO, 0, APP_BASE_BOARD_LED1_PIN, 1); // Off
    	DBG_vPrintf(TRACE_LINKY, "\r\nLINKY Timeout\r\n");
    }else{
    	GPIO_PinInit(GPIO, 0, APP_BASE_BOARD_LED1_PIN, &((const gpio_pin_config_t){kGPIO_DigitalOutput, 0}));
    	GPIO_PinWrite(GPIO, 0, APP_BASE_BOARD_LED1_PIN, 1); //ON
    }
    //u16ALSResult = random();
   // sSensor.sIlluminanceMeasurementServerCluster.u16MeasuredValue = u16ALSResult;
    DBG_vPrintf(TRACE_LINKY, "ZTIMER_eStart\r\n");
    /* Start sample timer so that you keep on sampling if KEEPALIVE_TIME is too high*/
    ZTIMER_eStart(u8TimerLightSensorSample, ZTIMER_TIME_MSEC(1000 * LINKY_SAMPLING_TIME_IN_SECONDS));
}

/****************************************************************************
 *
 * NAME: app_u8GetDeviceEndpoint
 *
 * DESCRIPTION:
 * Returns the application endpoint
 *
 * PARAMETER: void
 *
 * RETURNS: void
 *
 ****************************************************************************/
PUBLIC uint8 app_u8GetDeviceEndpoint( void)
{
    return LINKY_SENSOR_ENDPOINT;
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
