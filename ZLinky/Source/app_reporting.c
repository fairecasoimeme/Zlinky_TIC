/*****************************************************************************
 *
 * MODULE:             JN-AN-1243
 *
 * COMPONENT:          app_reporting.c
 *
 * DESCRIPTION:        Base Device application - reporting functionality
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
 * Copyright NXP B.V. 2016-2018. All rights reserved
 *
 ***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include <string.h>
#include "dbg.h"
#include "zps_gen.h"
#include "PDM.h"
#include "PDM_IDs.h"
#include "app_common.h"
#include "zcl_options.h"
#include "zcl_common.h"
#include "app_reporting.h"
#ifdef CLD_GROUPS
#include "Groups.h"
#include "OnOff.h"
#include "Groups_internal.h"
#endif
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef DEBUG_REPORT
    #define TRACE_REPORT   FALSE
#else
    #define TRACE_REPORT   TRUE
#endif
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/


/*There are just two attributes at this point - OnOff and CurrentLevel */

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/


/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
/*Just Two reports for time being*/
PRIVATE tsReports asSavedReports[ZCL_NUMBER_OF_REPORTS];

/* define the default reports */
tsReports asDefaultReports[ZCL_NUMBER_OF_REPORTS] = \
{\
   // {GENERAL_CLUSTER_ID_ONOFF, {0, E_ZCL_BOOL, E_CLD_ONOFF_ATTR_ID_ONOFF, MIN_REPORT_INTERVAL,MAX_REPORT_INTERVAL,0,{0}}}
};


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/


/****************************************************************************
 *
 * NAME: eRestoreReports
 *
 * DESCRIPTION:
 * Loads the reporting information from the EEPROM/PDM
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC PDM_teStatus eRestoreReports( void )
{
    /* Restore any report data that is previously saved to flash */
    uint16 u16ByteRead;
    PDM_teStatus eStatusReportReload = PDM_eReadDataFromRecord(PDM_ID_APP_REPORTS,
                                                              asSavedReports,
                                                              sizeof(asSavedReports),
                                                              &u16ByteRead);

    DBG_vPrintf(TRACE_REPORT,"\r\neStatusReportReload = %d", eStatusReportReload);
    /* Restore any application data previously saved to flash */

    return  (eStatusReportReload);
}

/****************************************************************************
 *
 * NAME: vMakeSupportedAttributesReportable
 *
 * DESCRIPTION:
 * Makes the attributes reportable for On Off and Level control
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void vMakeSupportedAttributesReportable(void)
{
    uint16 u16AttributeEnum;
    uint16 u16ClusterId;
    int i;


    tsZCL_AttributeReportingConfigurationRecord*    psAttributeReportingConfigurationRecord;

    DBG_vPrintf(TRACE_REPORT, "\r\nMAKE Reportable ep %d", ZLINKY_APPLICATION_ENDPOINT);

    for(i=0; i<ZCL_NUMBER_OF_REPORTS; i++)
    {
        u16AttributeEnum = asSavedReports[i].sAttributeReportingConfigurationRecord.u16AttributeEnum;
        u16ClusterId = asSavedReports[i].u16ClusterID;
        psAttributeReportingConfigurationRecord = &(asSavedReports[i].sAttributeReportingConfigurationRecord);
        DBG_vPrintf(TRACE_REPORT, "\r\nCluster %04x Attribute %04x Min %d Max %d IntV %d Direct %d Change %d",
                u16ClusterId,
                u16AttributeEnum,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u16MinimumReportingInterval,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u16MaximumReportingInterval,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u16TimeoutPeriodField,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u8DirectionIsReceived,
                asSavedReports[i].sAttributeReportingConfigurationRecord.uAttributeReportableChange.zint8ReportableChange);
        eZCL_SetReportableFlag( ZLINKY_APPLICATION_ENDPOINT, u16ClusterId, TRUE, FALSE, u16AttributeEnum);
        eZCL_CreateLocalReport( ZLINKY_APPLICATION_ENDPOINT, u16ClusterId, 0, TRUE, psAttributeReportingConfigurationRecord);
    }
}

/****************************************************************************
 *
 * NAME: vLoadDefaultConfigForReportable
 *
 * DESCRIPTION:
 * Loads a default configuration
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/

PUBLIC void vLoadDefaultConfigForReportable(void)
{
    memset(asSavedReports, 0 ,sizeof(asSavedReports));
    int i;
    for (i=0; i<ZCL_NUMBER_OF_REPORTS; i++)
    {
        asSavedReports[i] = asDefaultReports[i];
    }

#if TRACE_REPORT

    DBG_vPrintf(TRACE_REPORT,"\r\nLoaded Defaults Records");
    for(i=0; i <ZCL_NUMBER_OF_REPORTS; i++)
    {
        DBG_vPrintf(TRACE_REPORT, "\r\nCluster %04x Type %d Attr %04x Min %d Max %d IntV %d Direct %d Change %d",
                asSavedReports[i].u16ClusterID,
                asSavedReports[i].sAttributeReportingConfigurationRecord.eAttributeDataType,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u16AttributeEnum,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u16MinimumReportingInterval,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u16MaximumReportingInterval,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u16TimeoutPeriodField,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u8DirectionIsReceived,
                asSavedReports[i].sAttributeReportingConfigurationRecord.uAttributeReportableChange.zuint8ReportableChange);
    }
#endif


    /*Save this Records*/
    PDM_eSaveRecordData(PDM_ID_APP_REPORTS,
                        asSavedReports,
                        sizeof(asSavedReports));
}

/****************************************************************************
 *
 * NAME: vRestoreDefaultRecord
 *
 * DESCRIPTION:
 * Restores a default configuration
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void vRestoreDefaultRecord(  uint8                                        u8EndPointID,
                                    uint16                                       u16ClusterID,
                                    tsZCL_AttributeReportingConfigurationRecord* psAttributeReportingConfigurationRecord)
{
    uint8 u8Index = 0;

    eZCL_CreateLocalReport( u8EndPointID, u16ClusterID, 0, TRUE, &(asDefaultReports[u8Index].sAttributeReportingConfigurationRecord));

    DBG_vPrintf(TRACE_REPORT, "Restore to report %d\n", u8Index);

    memcpy( &(asSavedReports[u8Index].sAttributeReportingConfigurationRecord),
            &(asDefaultReports[u8Index].sAttributeReportingConfigurationRecord),
            sizeof(tsZCL_AttributeReportingConfigurationRecord) );

    DBG_vPrintf(TRACE_REPORT,"Restore reportable Cluster %04x Type %d Attrib %04x Min %d Max %d IntV %d Direction %d Change %d\n",
            asSavedReports[u8Index].u16ClusterID,
            asSavedReports[u8Index].sAttributeReportingConfigurationRecord.eAttributeDataType,
            asSavedReports[u8Index].sAttributeReportingConfigurationRecord.u16AttributeEnum,
            asSavedReports[u8Index].sAttributeReportingConfigurationRecord.u16MinimumReportingInterval,
            asSavedReports[u8Index].sAttributeReportingConfigurationRecord.u16MaximumReportingInterval,
            asSavedReports[u8Index].sAttributeReportingConfigurationRecord.u16TimeoutPeriodField,
            asSavedReports[u8Index].sAttributeReportingConfigurationRecord.u8DirectionIsReceived,
            asSavedReports[u8Index].sAttributeReportingConfigurationRecord.uAttributeReportableChange.zuint8ReportableChange    );

    /*Save this Records*/
    PDM_eSaveRecordData(PDM_ID_APP_REPORTS,
                        asSavedReports,
                        sizeof(asSavedReports));

}
/****************************************************************************
 *
 * NAME: vSaveReportableRecord
 *
 * DESCRIPTION:
 * Loads a default configuration
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void vSaveReportableRecord(  uint16 u16ClusterID,
                                    tsZCL_AttributeReportingConfigurationRecord* psAttributeReportingConfigurationRecord)
{
    int iIndex;
    int i;
	for (i=0; i<ZCL_NUMBER_OF_REPORTS; i++)
	{
		DBG_vPrintf(TRACE_REPORT,"\r\ncount i : %d Cluster : %04x attr : %04x",i,u16ClusterID,psAttributeReportingConfigurationRecord->u16AttributeEnum);
		if  ((asSavedReports[i].u16ClusterID==u16ClusterID) && (asSavedReports[i].sAttributeReportingConfigurationRecord.u16AttributeEnum == psAttributeReportingConfigurationRecord->u16AttributeEnum))
		{
			iIndex=i;
			break;
		}
		if  (asSavedReports[i].u16ClusterID==0)
		{
			iIndex=i;
			break;
		}
	}

	DBG_vPrintf(TRACE_REPORT, "\r\nSave to report %d", iIndex);

	/*For CurrentLevel attribute in LevelControl Cluster*/
	asSavedReports[iIndex].u16ClusterID=u16ClusterID;
	memcpy( &(asSavedReports[iIndex].sAttributeReportingConfigurationRecord),
			psAttributeReportingConfigurationRecord,
			sizeof(tsZCL_AttributeReportingConfigurationRecord) );

	asSavedReports[iIndex].sAttributeReportingConfigurationRecord.u16TimeoutPeriodField=0;
	DBG_vPrintf(TRACE_REPORT,"\r\nCluster %04x Type %d Attrib %04x Min %d Max %d IntV %d Direction %d Change %d",
			asSavedReports[iIndex].u16ClusterID,
			asSavedReports[iIndex].sAttributeReportingConfigurationRecord.eAttributeDataType,
			asSavedReports[iIndex].sAttributeReportingConfigurationRecord.u16AttributeEnum,
			asSavedReports[iIndex].sAttributeReportingConfigurationRecord.u16MinimumReportingInterval,
			asSavedReports[iIndex].sAttributeReportingConfigurationRecord.u16MaximumReportingInterval,
			asSavedReports[iIndex].sAttributeReportingConfigurationRecord.u16TimeoutPeriodField,
			asSavedReports[iIndex].sAttributeReportingConfigurationRecord.u8DirectionIsReceived,
			asSavedReports[iIndex].sAttributeReportingConfigurationRecord.uAttributeReportableChange.zuint8ReportableChange );

	/*Save this Records*/
	PDM_eSaveRecordData(PDM_ID_APP_REPORTS,
						asSavedReports,
						sizeof(asSavedReports));

}


PUBLIC void vSendImmediateReport(uint16 cluster, uint16 attribute)
{
	PDUM_thAPduInstance myPDUM_thAPduInstance;
	tsZCL_Address sDestinationAddress;
	teZCL_Status eStatus;

	DBG_vPrintf(1, "\r\nREPORT: vSendImmediateReport()");

	/* get buffer to write the response in*/
	myPDUM_thAPduInstance = hZCL_AllocateAPduInstance();
	/* no buffers - return*/
	if(myPDUM_thAPduInstance == PDUM_INVALID_HANDLE)
	{
		DBG_vPrintf(1, "\r\nREPORT: hZCL_AllocateAPduInstance() == PDUM_INVALID_HANDLE, ERROR!");
	}

	/* Got buffer ? */
	else
	{
		DBG_vPrintf(1, "\r\nmyPDUM_thAPduInstance : %d",myPDUM_thAPduInstance);
		/* Set the address mode to send to all bound device and don't wait for an ACK*/
		sDestinationAddress.eAddressMode = E_ZCL_AM_BOUND_NO_ACK; //MJL: was E_ZCL_AM_BOUND_NO_ACK;
		sDestinationAddress.uAddress.u16DestinationAddress = 0;

		/* Attempt to send */
		eStatus = eZCL_ReportAttribute(    &sDestinationAddress,
											cluster,
											attribute,
											1,
										    1,
										    myPDUM_thAPduInstance);

		DBG_vPrintf(1, "\r\nREPORT: eZCL_ReportAllAttributes() = 0x%02x _ cluster :%d / attr :%d", eStatus,cluster,attribute);
		/* Sent the report with all attributes ? */
		if (E_ZCL_SUCCESS == eStatus)
		{
			DBG_vPrintf(1, ", SUCCESS");
		}
		else
		{
			DBG_vPrintf(1, ", FAILED");
			/* free buffer for failed send */
			PDUM_eAPduFreeAPduInstance(myPDUM_thAPduInstance);
		}
	}
}

/* TUYA */

PUBLIC void vSendTuyaSpecificReport(uint16 cluster, uint16 attribute)
{
	PDUM_thAPduInstance myPDUM_thAPduInstance;
	tsZCL_Address sDestinationAddress;
	teZCL_Status eStatus;

	DBG_vPrintf(1, "\r\nREPORT: vSendImmediateReport()");

	/* get buffer to write the response in*/
	myPDUM_thAPduInstance = hZCL_AllocateAPduInstance();
	/* no buffers - return*/
	if(myPDUM_thAPduInstance == PDUM_INVALID_HANDLE)
	{
		DBG_vPrintf(1, "\r\nREPORT: hZCL_AllocateAPduInstance() == PDUM_INVALID_HANDLE, ERROR!");
	}

	/* Got buffer ? */
	else
	{
		DBG_vPrintf(1, "\r\nmyPDUM_thAPduInstance : %d",myPDUM_thAPduInstance);
		/* Set the address mode to send to all bound device and don't wait for an ACK*/
		sDestinationAddress.eAddressMode = E_ZCL_AM_SHORT_NO_ACK;
		sDestinationAddress.uAddress.u16DestinationAddress = 0;

		/* Attempt to send */
		eStatus = eZCL_ReportAttribute(    &sDestinationAddress,
											cluster,
											attribute,
											1,
										    1,
										    myPDUM_thAPduInstance);

		DBG_vPrintf(1, "\r\nREPORT: eZCL_ReportAllAttributes() = 0x%02x", eStatus);
		/* Sent the report with all attributes ? */
		if (E_ZCL_SUCCESS == eStatus)
		{
			DBG_vPrintf(1, ", SUCCESS");
		}
		else
		{
			DBG_vPrintf(1, ", FAILED");
			/* free buffer for failed send */
			PDUM_eAPduFreeAPduInstance(myPDUM_thAPduInstance);
		}
	}
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
