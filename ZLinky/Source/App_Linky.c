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

#include "base_device.h"
//#include "linky_device.h"

#include "app_blink_led.h"



extern uint8_t wdt_update_count;

uint32 pow10[9]={1,10,100,1000,10000,100000,1000000,10000000,100000000};

/****************************************************************************/
/***    Local Variables                           ***/
/****************************************************************************/
uint16             u16PacketType;
uint16             u16PacketLength;
uint8              au8Command[128];
uint8              au8Date[128];
uint8              au8Value[256];
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
//const uint8 u8MyEndpoint = LINKY_SENSOR_ENDPOINT;
extern tsZHA_BaseDevice sBaseDevice;


/* Function to remove white spaces on both sides of a string i.e trim */

char*  trim (char *s)
{
    int i;

    while (isspace (*s)) s++;   // skip left side white spaces
    for (i = strlen (s) - 1; (isspace (s[i])); i--) ;   // skip right side white spaces
    s[i + 1] = '\0';
    return s;
}



uint8 APP_vProcessRxDataStandard ( void )
{
	uint8    u8RxByte;
	if (loopOK>=2)
		return 1;
	if (u32Timeout>100000)
		return 2;
	if ( ZQ_bQueueReceive ( &APP_msgSerialRx, &u8RxByte ) )
	{
		if( TRUE == bSL_ReadMessageStandard(     MAX_PACKET_SIZE,
										 au8Command,
										 au8Date,
										 au8Value,
										 u8RxByte
									   )
			  )
			{
				u32Timeout=0;
				if (memcmp(au8Command,"ADSC",4)==0)
				{
					DBG_vPrintf(1, "\r\nADSC : %s",au8Value);

					memcpy(sBaseDevice.sSimpleMeteringServerCluster.sMeterSerialNumber.pu8Data,au8Value,12);
					sBaseDevice.sSimpleMeteringServerCluster.sMeterSerialNumber.u8Length=12;
					loopOK++;

				}else if(memcmp(au8Command,"NGTF",4)==0)
				{
					DBG_vPrintf(1, "\r\nNGTF : %s",trim(au8Value));
					memcpy(sBaseDevice.sLinkyServerCluster.au8LinkyOptarif, au8Value,16);

				}else if(memcmp(au8Command,"LTARF",5)==0)
				{
					DBG_vPrintf(1, "\r\nLTARF : %s",trim(au8Value));
					memcpy(sBaseDevice.sLinkyServerCluster.au8LinkyLTARF, au8Value,16);

				}else if(memcmp(au8Command,"NTARF",5)==0)
				{
					DBG_vPrintf(1, "\r\nNTARF : %s",trim(au8Value));
					sBaseDevice.sLinkyServerCluster.au8LinkyNTARF = atoi(au8Value);

				}else if(memcmp(au8Command,"VTIC",4)==0)
				{
					DBG_vPrintf(1, "\r\nVTIC : %s",au8Value);
					sBaseDevice.sMeterIdentification.au8LinkyMISoftwareRevision=atoi(au8Value);

				}else if(memcmp(au8Command,"DATE",4)==0)
				{
					DBG_vPrintf(1, "\r\nDATE : %s",au8Date);
					memcpy(sBaseDevice.sLinkyServerCluster.au8LinkyDATE, au8Value,13);

				}else if(memcmp(au8Command,"EAST",4)==0)
				{
					DBG_vPrintf(1, "\r\nEAST : %s",au8Value);
					sBaseDevice.sSimpleMeteringServerCluster.u48CurrentSummationDelivered = atol(au8Value);
				}else if(memcmp(au8Command,"EASF01",7)==0)
				{
					DBG_vPrintf(1, "\r\nEASF01 : %s",au8Value);
					sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier1SummationDelivered = atol(au8Value);
				}else if(memcmp(au8Command,"EASF02",7)==0)
				{
					DBG_vPrintf(1, "\r\nEASF02 : %s",au8Value);
					sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier2SummationDelivered = atol(au8Value);
				}else if(memcmp(au8Command,"EASF03",7)==0)
				{
					DBG_vPrintf(1, "\r\nEASF03 : %s",au8Value);
					sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier3SummationDelivered = atol(au8Value);
				}else if(memcmp(au8Command,"EASF04",7)==0)
				{
					DBG_vPrintf(1, "\r\nEASF04 : %s",au8Value);
					sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier4SummationDelivered = atol(au8Value);
				}else if(memcmp(au8Command,"EASF05",7)==0)
				{
					DBG_vPrintf(1, "\r\nEASF05 : %s",au8Value);
					sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier5SummationDelivered = atol(au8Value);
				}else if(memcmp(au8Command,"EASF06",7)==0)
				{
					DBG_vPrintf(1, "\r\nEASF06 : %s",au8Value);
					sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier6SummationDelivered = atol(au8Value);
				}else if(memcmp(au8Command,"EASF07",7)==0)
				{
					DBG_vPrintf(1, "\r\nEASF07 : %s",au8Value);
					sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier7SummationDelivered = atol(au8Value);
				}else if(memcmp(au8Command,"EASF08",7)==0)
				{
					DBG_vPrintf(1, "\r\nEASF08 : %s",au8Value);
					sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier8SummationDelivered = atol(au8Value);
				}else if(memcmp(au8Command,"EASF09",7)==0)
				{
					DBG_vPrintf(1, "\r\nEASF09 : %s",au8Value);
					sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier9SummationDelivered = atol(au8Value);
				}else if(memcmp(au8Command,"EASF10",7)==0)
				{
					DBG_vPrintf(1, "\r\nEASF10 : %s",au8Value);
					sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier10SummationDelivered = atol(au8Value);
				}else if(memcmp(au8Command,"EASD01",6)==0)
				{
					DBG_vPrintf(1, "\r\nEASD01 : %s",au8Value);
					sBaseDevice.sLinkyServerCluster.u32LinkyEASD01 = atol(au8Value);
				}else if(memcmp(au8Command,"EASD02",6)==0)
				{
					DBG_vPrintf(1, "\r\nEASD02 : %s",au8Value);
					sBaseDevice.sLinkyServerCluster.u32LinkyEASD02 =  atol(au8Value);
				}else if(memcmp(au8Command,"EASD03",6)==0)
				{
					DBG_vPrintf(1, "\r\nEASD03 : %s",au8Value);
					sBaseDevice.sLinkyServerCluster.u32LinkyEASD03 =  atol(au8Value);
				}else if(memcmp(au8Command,"EASD04",6)==0)
				{
					DBG_vPrintf(1, "\r\nEASD04 : %s",au8Value);
					sBaseDevice.sLinkyServerCluster.u32LinkyEASD04 =  atol(au8Value);
				}else if(memcmp(au8Command,"EAIT",4)==0)
				{
					DBG_vPrintf(1, "\r\nEAIT : %s",au8Value);
					sBaseDevice.sSimpleMeteringServerCluster.u48CurrentSummationReceived= atol(au8Value);
					sBaseDevice.sLinkyServerCluster.au8LinkyMode |= (1 << 2) ;
				}else if(memcmp(au8Command,"ERQ1",4)==0)
				{
					DBG_vPrintf(1, "\r\nERQ1 : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.i32TotalReactivePower = atol(au8Value);
				}else if(memcmp(au8Command,"ERQ2",4)==0)
				{
					DBG_vPrintf(1, "\r\nERQ2 : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.i16ReactivePower = atol(au8Value);
				}else if(memcmp(au8Command,"ERQ3",4)==0)
				{
					DBG_vPrintf(1, "\r\nERQ3 : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.i16ReactivePowerPhB = atol(au8Value);
				}else if(memcmp(au8Command,"ERQ4",4)==0)
				{
					DBG_vPrintf(1, "\r\nERQ4 : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.i16ReactivePowerPhC = atol(au8Value);
				}else if(memcmp(au8Command,"IRMS1",5)==0)
				{
					DBG_vPrintf(1, "\r\nIRMS1 : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.u16RMSCurrent=atoi(au8Value);
				}else if(memcmp(au8Command,"IRMS2",5)==0)
				{
					DBG_vPrintf(1, "\r\nIRMS2 : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.u16RMSCurrentPhB=atoi(au8Value);
					sBaseDevice.sLinkyServerCluster.au8LinkyMode |= (1 << 1) ;
				}else if(memcmp(au8Command,"IRMS3",5)==0)
				{
					DBG_vPrintf(1, "\r\nIRMS3 : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.u16RMSCurrentPhC=atoi(au8Value);
				}else if(memcmp(au8Command,"URMS1",5)==0)
				{
					DBG_vPrintf(1, "\r\nURMS1 : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.u16RMSVoltage=atoi(au8Value);
				}else if(memcmp(au8Command,"URMS2",5)==0)
				{
					DBG_vPrintf(1, "\r\nURMS2 : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.u16RMSVoltagePhB=atoi(au8Value);
				}else if(memcmp(au8Command,"URMS3",5)==0)
				{
					DBG_vPrintf(1, "\r\nURMS3 : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.u16RMSVoltagePhC=atoi(au8Value);
				}else if(memcmp(au8Command,"PREF",4)==0)
				{
					DBG_vPrintf(1, "\r\nPREF : %s",au8Value);
					sBaseDevice.sMeterIdentification.au8LinkyMIAvailablePower=atol(au8Value);
				}else if(memcmp(au8Command,"STGE",4)==0)
				{
					DBG_vPrintf(1, "\r\nSTGE : %s",au8Value);
					memcpy(sBaseDevice.sLinkyServerCluster.au8LinkySTGE, au8Value,8);
				}else if(memcmp(au8Command,"PCOUP",5)==0)
				{
					DBG_vPrintf(1, "\r\nPCOUP : %s",au8Value);
					sBaseDevice.sMeterIdentification.au8LinkyMIPowerThreshold=atoi(au8Value);
				}else if(memcmp(au8Command,"SINSTI",6)==0)
				{
					DBG_vPrintf(1, "\r\nSINSTI : %s",au8Value);
					sBaseDevice.sLinkyServerCluster.u16LinkySINSTI = atol(au8Value);
				}else if(memcmp(au8Command,"SMAXIN-1",8)==0)
				{
					DBG_vPrintf(1, "\r\nSMAXIN-1 : %s",au8Value);
					sBaseDevice.sLinkyServerCluster.u16LinkySMAXIN_1 = atol(au8Value);
				}else if(memcmp(au8Command,"SMAXIN",6)==0)
				{
					DBG_vPrintf(1, "\r\nSMAXIN : %s",au8Value);
					sBaseDevice.sLinkyServerCluster.u16LinkySMAXIN = atol(au8Value);
				}else if(memcmp(au8Command,"CCASN-1",7)==0)
				{
					DBG_vPrintf(1, "\r\nCCASN-1 : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.i16ActivePowerPhB=atoi(au8Value);
				}else if(memcmp(au8Command,"CCASN",5)==0)
				{
					DBG_vPrintf(1, "\r\nCCASN : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.i16ActivePower=atoi(au8Value);
				}else if(memcmp(au8Command,"CCAIN-1",7)==0)
				{
					DBG_vPrintf(1, "\r\nCCAIN-1 : %s",au8Value);
					sBaseDevice.sLinkyServerCluster.u16LinkyCCAIN_1 = atol(au8Value);

				}else if(memcmp(au8Command,"CCAIN",5)==0)
				{
					DBG_vPrintf(1, "\r\nCCAIN : %s",au8Value);
					sBaseDevice.sLinkyServerCluster.u16LinkyCCAIN = atol(au8Value);
				}else if(memcmp(au8Command,"UMOY1",5)==0)
				{
					DBG_vPrintf(1, "\r\nUMOY1 : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.u16AverageRMSVoltageMeasurementPeriod=atoi(au8Value);
				}else if(memcmp(au8Command,"UMOY2",5)==0)
				{
					DBG_vPrintf(1, "\r\nUMOY2 : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.u16AverageRMSVoltageMeasurementPeriodPhB=atoi(au8Value);
				}else if(memcmp(au8Command,"UMOY3",5)==0)
				{
					DBG_vPrintf(1, "\r\nUMOY3 : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.u16AverageRMSVoltageMeasurementPeriodPhC=atoi(au8Value);
				}else if(memcmp(au8Command,"SINSTS1",7)==0)
				{
					DBG_vPrintf(1, "\r\nSINSTS1 : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.u16ApparentPower=atol(au8Value);
				}else if(memcmp(au8Command,"SINSTS2",7)==0)
				{
					DBG_vPrintf(1, "\r\nSINSTS2 : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.u16ApparentPowerPhB=atol(au8Value);
				}else if(memcmp(au8Command,"SINSTS3",7)==0)
				{
					DBG_vPrintf(1, "\r\nSINSTS3 : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.u16ApparentPowerPhC=atol(au8Value);
				}else if(memcmp(au8Command,"SINSTS",6)==0)
				{
					DBG_vPrintf(1, "\r\nSINSTS : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.u16ApparentPower=atol(au8Value);
				}else if(memcmp(au8Command,"SMAXSN-1",8)==0)
				{
					DBG_vPrintf(1, "\r\nSMAXSN-1 : %s",au8Value);
					sBaseDevice.sLinkyServerCluster.u16LinkySMAXSN_1 = atol(au8Value);
				}else if(memcmp(au8Command,"SMAXSN1-1",9)==0)
				{
					DBG_vPrintf(1, "\r\nSMAXSN1-1 : %s",au8Value);
					sBaseDevice.sLinkyServerCluster.u16LinkySMAXSN_1 =atol(au8Value);
				}else if(memcmp(au8Command,"SMAXSN2-1",9)==0)
				{
					DBG_vPrintf(1, "\r\nSMAXSN2-1 : %s",au8Value);
					sBaseDevice.sLinkyServerCluster.u16LinkySMAXSN2_1 = atol(au8Value);
				}else if(memcmp(au8Command,"SMAXSN3-1",9)==0)
				{
					DBG_vPrintf(1, "\r\nSMAXSN3-1 : %s",au8Value);
					sBaseDevice.sLinkyServerCluster.u16LinkySMAXSN3_1 = atol(au8Value);
				}else if(memcmp(au8Command,"SMAXSN1",7)==0)
				{
					DBG_vPrintf(1, "\r\nSMAXSN1 : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.i16ActivePowerMax=atoi(au8Value);
				}else if(memcmp(au8Command,"SMAXSN2",7)==0)
				{
					DBG_vPrintf(1, "\r\nSMAXSN2 : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.i16ActivePowerMaxPhB=atoi(au8Value);
				}else if(memcmp(au8Command,"SMAXSN3",7)==0)
				{
					DBG_vPrintf(1, "\r\nSMAXSN3 : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.i16ActivePowerMaxPhC=atoi(au8Value);
				}else if(memcmp(au8Command,"SMAXSN",6)==0)
				{
					DBG_vPrintf(1, "\r\nSMAXSN : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.i16ActivePowerMax=atoi(au8Value);
				}else if(memcmp(au8Command,"MSG1",4)==0)
				{
					DBG_vPrintf(1, "\r\nMSG1 : %s",trim(au8Value));
					memcpy(sBaseDevice.sLinkyServerCluster.au8LinkyMSG1, au8Value,32);
				}else if(memcmp(au8Command,"MSG2",4)==0)
				{
					DBG_vPrintf(1, "\r\nMSG2 : %s",trim(au8Value));
					memcpy(sBaseDevice.sLinkyServerCluster.au8LinkyMSG2, au8Value,16);
				}else if(memcmp(au8Command,"PRM",3)==0)
				{
					DBG_vPrintf(1, "\r\nPRM : %s",au8Value);
					memcpy(sBaseDevice.sSimpleMeteringServerCluster.sSiteId.pu8Data,au8Value,14);
					sBaseDevice.sSimpleMeteringServerCluster.sSiteId.u8Length=14;
				}else if(memcmp(au8Command,"STGE",3)==0)
				{
					DBG_vPrintf(1, "\r\nSTGE : %s",au8Value);
					memcpy(sBaseDevice.sLinkyServerCluster.au8LinkySTGE, au8Value,8);
				}else if(memcmp(au8Command,"DPM1",4)==0)
				{
					DBG_vPrintf(1, "\r\nDPM1 : %s",au8Value);
					sBaseDevice.sLinkyServerCluster.u8LinkyDPM1 = atoi(au8Value);
				}else if(memcmp(au8Command,"FPM1",4)==0)
				{
					DBG_vPrintf(1, "\r\nFPM1 : %s",au8Value);
					sBaseDevice.sLinkyServerCluster.u8LinkyFPM1 = atoi(au8Value);
				}else if(memcmp(au8Command,"DPM2",4)==0)
				{
					DBG_vPrintf(1, "\r\nDPM2 : %s",au8Value);
					sBaseDevice.sLinkyServerCluster.u8LinkyDPM2 = atoi(au8Value);
				}else if(memcmp(au8Command,"FPM2",4)==0)
				{
					DBG_vPrintf(1, "\r\nFPM2 : %s",au8Value);
					sBaseDevice.sLinkyServerCluster.u8LinkyFPM2 = atoi(au8Value);
				}else if(memcmp(au8Command,"DPM3",4)==0)
				{
					DBG_vPrintf(1, "\r\nDPM3 : %s",au8Value);
					sBaseDevice.sLinkyServerCluster.u8LinkyDPM3 = atoi(au8Value);
				}else if(memcmp(au8Command,"FPM3",4)==0)
				{
					DBG_vPrintf(1, "\r\nFPM3 : %s",au8Value);
					sBaseDevice.sLinkyServerCluster.u8LinkyFPM3 = atoi(au8Value);
				}else if(memcmp(au8Command,"RELAIS",6)==0)
				{
					DBG_vPrintf(1, "\r\nRELAIS : %s",au8Value);
					sBaseDevice.sLinkyServerCluster.u16LinkyRELAIS = atoi(au8Value);
				}else if(memcmp(au8Command,"NJOURF+1",8)==0)
				{
					DBG_vPrintf(1, "\r\nNJOURF+1 : %s",au8Value);
					sBaseDevice.sLinkyServerCluster.u8LinkyNJOURF1 = atoi(au8Value);
				}else if(memcmp(au8Command,"NJOURF",6)==0)
				{
					DBG_vPrintf(1, "\r\nNJOURF : %s",au8Value);
					sBaseDevice.sLinkyServerCluster.u8LinkyNJOURF = atoi(au8Value);
				}else if(memcmp(au8Command,"PJOURF+1",8)==0)
				{
					DBG_vPrintf(1, "\r\nPJOURF+1 : %s",au8Value);
					memcpy(sBaseDevice.sLinkyServerCluster.au8LinkyPJOURF1, au8Value,48);;
				}else if(memcmp(au8Command,"PPOINTE",7)==0)
				{
					DBG_vPrintf(1, "\r\nPPOINTE : %s",au8Value);
					memcpy(sBaseDevice.sLinkyServerCluster.au8LinkyPPOINTE, au8Value,48);
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

uint8 APP_vProcessRxDataHisto ( void )
{
	uint8    u8RxByte;

	if (loopOK>=2)
		return 1;
	if (u32Timeout>100000)
		return 2;
	if ( ZQ_bQueueReceive ( &APP_msgSerialRx, &u8RxByte ) )
	{

		if( TRUE == bSL_ReadMessageHisto(     MAX_PACKET_SIZE,
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

					memcpy(sBaseDevice.sSimpleMeteringServerCluster.sMeterSerialNumber.pu8Data,au8Value,12);
					sBaseDevice.sSimpleMeteringServerCluster.sMeterSerialNumber.u8Length=12;
					loopOK++;

				}else if(memcmp(au8Command,"MOTDETAT",8)==0)
				{
					DBG_vPrintf(1, "\r\nMOTDETAT : %s",au8Value);
				}else if(memcmp(au8Command,"BASE",4)==0)
				{
					DBG_vPrintf(1, "\r\nBASE : %s",au8Value);
					sBaseDevice.sSimpleMeteringServerCluster.u48CurrentSummationDelivered=atol(au8Value);
					sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier1SummationDelivered=atol(au8Value);
				}else if(memcmp(au8Command,"HHPHC",5)==0)
				{
					DBG_vPrintf(1, "\r\nHHPHC : %s",au8Value);
				}else if(memcmp(au8Command,"OPTARIF",7)==0)
				{
					DBG_vPrintf(1, "\r\nOPTARIF : %s",au8Value);
					memcpy(sBaseDevice.sLinkyServerCluster.au8LinkyOptarif, au8Value,4);
				}else if(memcmp(au8Command,"ISOUSC",6)==0)
				{
					DBG_vPrintf(1, "\r\nISOUSC : %s",au8Value);
					sBaseDevice.sMeterIdentification.au8LinkyMIAvailablePower=atol(au8Value);

				}else if(memcmp(au8Command,"HCHC",4)==0)
				{
					DBG_vPrintf(1, "\r\nHCHC : %s",au8Value);
					sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier1SummationDelivered=atol(au8Value);
					//DBG_vPrintf(1, "\r\nHCHC : %d",sBaseDevice.sSimpleMeteringServerCluster.u32CurrentTier2SummationDelivered);
				}else if(memcmp(au8Command,"HCHP",4)==0)
				{
					DBG_vPrintf(1, "\r\nHCHP : %s",au8Value);
					sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier2SummationDelivered=atol(au8Value);
					//DBG_vPrintf(1, "\r\nHCHP : %d",sBaseDevice.sSimpleMeteringServerCluster.u32CurrentTier1SummationDelivered);

				}else if(memcmp(au8Command,"EJPHN",5)==0)
				{
					DBG_vPrintf(1, "\r\nEJPHN : %s",au8Value);
					sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier1SummationDelivered=atol(au8Value);
				}else if(memcmp(au8Command,"EJPHPM",6)==0)
				{
					DBG_vPrintf(1, "\r\nEJPHPM : %s",au8Value);
					sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier2SummationDelivered=atol(au8Value);
				}else if(memcmp(au8Command,"BBRHCJB",7)==0)
				{
					DBG_vPrintf(1, "\r\nBBRHCJB : %s",au8Value);
					sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier1SummationDelivered=atol(au8Value);
				}else if(memcmp(au8Command,"BBRHPJB",7)==0)
				{
					DBG_vPrintf(1, "\r\nBBRHPJB : %s",au8Value);
					sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier2SummationDelivered=atol(au8Value);
				}else if(memcmp(au8Command,"BBRHCJW",7)==0)
				{
					DBG_vPrintf(1, "\r\nBBRHCJW : %s",au8Value);
					sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier3SummationDelivered=atol(au8Value);
				}else if(memcmp(au8Command,"BBRHPJW",7)==0)
				{
					DBG_vPrintf(1, "\r\nBBRHPJW : %s",au8Value);
					sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier4SummationDelivered=atol(au8Value);
				}else if(memcmp(au8Command,"BBRHCJR",7)==0)
				{
					DBG_vPrintf(1, "\r\nBBRHCJR : %s",au8Value);
					sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier5SummationDelivered=atol(au8Value);
				}else if(memcmp(au8Command,"BBRHPJR",7)==0)
				{
					DBG_vPrintf(1, "\r\nBBRHPJR : %s",au8Value);
					sBaseDevice.sSimpleMeteringServerCluster.u48CurrentTier6SummationDelivered=atol(au8Value);
				}else if(memcmp(au8Command,"IINST1",6)==0)
				{
					DBG_vPrintf(1, "\r\nIINST1 : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.u16RMSCurrent=atoi(au8Value);
					sBaseDevice.sLinkyServerCluster.au8LinkyMode |= (1 << 1) ;
				}else if(memcmp(au8Command,"IINST2",6)==0)
				{
					DBG_vPrintf(1, "\r\nIINST2 : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.u16RMSCurrentPhB=atoi(au8Value);
				}else if(memcmp(au8Command,"IINST3",6)==0)
				{
					DBG_vPrintf(1, "\r\nIINST3 : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.u16RMSCurrentPhC=atoi(au8Value);
				}else if(memcmp(au8Command,"IINST",5)==0)
				{
					DBG_vPrintf(1, "\r\nIINST : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.u16RMSCurrent=atoi(au8Value);
					DBG_vPrintf(1, "\r\nIINST : %ld",sBaseDevice.sElectricalMeasurement.u16RMSCurrent);
				}else if(memcmp(au8Command,"IMAX1",5)==0)
				{
					DBG_vPrintf(1, "\r\nIMAX1 : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.u16RMSCurrentMax=atoi(au8Value);
				}else if(memcmp(au8Command,"IMAX2",5)==0)
				{
					DBG_vPrintf(1, "\r\nIMAX2 : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.u16RMSCurrentMaxPhB=atoi(au8Value);
				}else if(memcmp(au8Command,"IMAX3",5)==0)
				{
					DBG_vPrintf(1, "\r\nIMAX3 : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.u16RMSCurrentMaxPhC=atoi(au8Value);
				}else if(memcmp(au8Command,"IMAX",4)==0)
				{
					DBG_vPrintf(1, "\r\nIMAX : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.u16RMSCurrentMax=atoi(au8Value);
				}else if(memcmp(au8Command,"PMAX",4)==0)
				{
					DBG_vPrintf(1, "\r\nPMAX : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.i16ActivePowerMax=atoi(au8Value);
				}else if(memcmp(au8Command,"PAPP",4)==0)
				{
					DBG_vPrintf(1, "\r\nPAPP : %s",au8Value);
					sBaseDevice.sElectricalMeasurement.u16ApparentPower=atoi(au8Value);

				}else if(memcmp(au8Command,"PTEC",4)==0)
				{
					DBG_vPrintf(1, "\r\nPTEC : %s",au8Value);
					memcpy(sBaseDevice.sSimpleMeteringServerCluster.sActiveRegisterTierDelivered.pu8Data,au8Value,4);
					sBaseDevice.sSimpleMeteringServerCluster.sActiveRegisterTierDelivered.u8Length=4;
				}else if(memcmp(au8Command,"ADPS",4)==0)
				{
					sBaseDevice.sLinkyServerCluster.au16LinkyADPS=atoi(au8Value);

				}else if(memcmp(au8Command,"ADIR1",5)==0)
				{
					sBaseDevice.sLinkyServerCluster.au16LinkyADIR1=atoi(au8Value);

				}else if(memcmp(au8Command,"ADIR2",5)==0)
				{
					sBaseDevice.sLinkyServerCluster.au16LinkyADIR2=atoi(au8Value);

				}else if(memcmp(au8Command,"ADIR3",5)==0)
				{
					sBaseDevice.sLinkyServerCluster.au16LinkyADIR3=atoi(au8Value);

				}else if(memcmp(au8Command,"DEMAIN",6)==0)
				{
					memcpy(sBaseDevice.sLinkyServerCluster.au8LinkyDemain, au8Value,4);

				}else if(memcmp(au8Command,"PPOT",4)==0)
				{
					sBaseDevice.sLinkyServerCluster.au8LinkyPPOT=atoi(au8Value);

				}else if(memcmp(au8Command,"PEJP",4)==0)
				{
					sBaseDevice.sLinkyServerCluster.au8LinkyPEJP=atoi(au8Value);
				}else if(memcmp(au8Command,"HHPHC",4)==0)
				{
					memcpy(sBaseDevice.sLinkyServerCluster.au8LinkyHHPHC, au8Value,1);
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
    //vStopBlinkTimer();
    UARTLINKY_vInit();
    if (u8ModeLinky==1)
	{
		UARTLINKY_vSetBaudRate ( 9600 );
		sBaseDevice.sLinkyServerCluster.au8LinkyMode = 1;
	}else if (u8ModeLinky==0)
	{
		UARTLINKY_vSetBaudRate ( 1200 );
		sBaseDevice.sLinkyServerCluster.au8LinkyMode = 0;
	}

    DBG_vPrintf(TRACE_LINKY, "\r\n\r\n\r\nUARTLINKY_vInit\r\n\r\n\r\n");

    u32Timeout=0;
    while(TRUE)
    {
    	if (u8ModeLinky == 1)
    	{

    		u8StatusLinky=APP_vProcessRxDataStandard();
    	}else if (u8ModeLinky == 0)
    	{

    		u8StatusLinky=APP_vProcessRxDataHisto();
    	}

    	if (u8StatusLinky>0)
    		break;


    	WWDT_Refresh(WWDT);
    	wdt_update_count = 0;

    }
    DBG_vPrintf(TRACE_LINKY, "\r\n au8LinkyMode : %d\r\n",sBaseDevice.sLinkyServerCluster.au8LinkyMode);
    UARTLINKY_vDeInit();

    if (u8StatusLinky == 2)
    {

    	if (u8ModeLinky==1)
    	{
    		u8ModeLinky=0;
    		DBG_vPrintf(TRACE_LINKY, "\r\nmode Historique");
    	}else if (u8ModeLinky==0)
    	{
    		u8ModeLinky=1;
    		DBG_vPrintf(TRACE_LINKY, "\r\nmode Standard");
    	}

    	//GPIO_PinWrite(GPIO, 0, APP_BASE_BOARD_LED1_PIN, 1);
    	//GPIO_PinInit(GPIO, APP_BASE_BOARD_LED1_PIN, 10, &((const gpio_pin_config_t){kGPIO_DigitalOutput, 1}));
    	GPIO_PinWrite(GPIO, APP_BOARD_GPIO_PORT, 10, 1); // ON
    	vStartAwakeTimer(5);
    	vStartBlinkTimer(250);

    	DBG_vPrintf(TRACE_LINKY, "\r\nLINKY Timeout\r\n");
    }else{
    	GPIO_PinWrite(GPIO, APP_BOARD_GPIO_PORT, 10, 1); //OFF
    }
    //u16ALSResult = random();
   // sSensor.sIlluminanceMeasurementServerCluster.u16MeasuredValue = u16ALSResult;
    DBG_vPrintf(TRACE_LINKY, "ZTIMER_eStart\r\n");
    /* Start sample timer so that you keep on sampling if KEEPALIVE_TIME is too high*/
   // ZTIMER_eStart(u8TimerLightSensorSample, ZTIMER_TIME_MSEC(1000 * LINKY_SAMPLING_TIME_IN_SECONDS));
    ZTIMER_eStart(u8TimerLinky, ZTIMER_TIME_MSEC(7000));
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
/*PUBLIC uint8 app_u8GetDeviceEndpoint( void)
{
    return LINKY_SENSOR_ENDPOINT;
}*/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
