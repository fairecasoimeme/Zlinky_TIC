/*
* Copyright 2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <string.h>
#include "dbg.h"
#include "zps_gen.h"
#include "PDM.h"
#include "PDM_IDs.h"
#include "app_common.h"
#include "zcl_options.h"
#include "zcl_common.h"
#include "app_reporting.h"
#include "Groups.h"
#include "OnOff.h"
#include "Groups_internal.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef TRACE_REPORT
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
static tsReports asSavedReports[NUMBER_OF_REPORTS];

/* define the default reports */
tsReports asDefaultReports[NUMBER_OF_REPORTS] = \
{\
    {GENERAL_CLUSTER_ID_ONOFF, {0, E_ZCL_BOOL, E_CLD_ONOFF_ATTR_ID_ONOFF, MIN_REPORT_INTERVAL,MAX_REPORT_INTERVAL,0,{0}}, 0}
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
PDM_teStatus eRestoreReports( void )
{
    /* Restore any report data that is previously saved to flash */
    uint16 u16ByteRead;
    PDM_teStatus eStatusReportReload = PDM_eReadDataFromRecord(PDM_ID_APP_REPORTS,
                                                              asSavedReports,
                                                              sizeof(asSavedReports),
                                                              &u16ByteRead);

    DBG_vPrintf(TRACE_REPORT,"eStatusReportReload = %d\r\n", eStatusReportReload);
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
void vMakeSupportedAttributesReportable(void)
{
    uint16 u16AttributeEnum;
    uint16 u16ClusterId;
    int i;


    tsZCL_AttributeReportingConfigurationRecord*    psAttributeReportingConfigurationRecord;

    DBG_vPrintf(TRACE_REPORT, "MAKE Reportable ep %d\r\n", APP_u8GetDeviceEndpoint());

    for(i=0; i<NUMBER_OF_REPORTS; i++)
    {
        u16AttributeEnum = asSavedReports[i].sAttributeReportingConfigurationRecord.u16AttributeEnum;
        u16ClusterId = asSavedReports[i].u16ClusterID;
        psAttributeReportingConfigurationRecord = &(asSavedReports[i].sAttributeReportingConfigurationRecord);
        DBG_vPrintf(TRACE_REPORT, "Cluster %04x Attribute %04x Min %d Max %d IntV %d Direct %d Change %d\r\n",
                u16ClusterId,
                u16AttributeEnum,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u16MinimumReportingInterval,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u16MaximumReportingInterval,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u16TimeoutPeriodField,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u8DirectionIsReceived,
                asSavedReports[i].sAttributeReportingConfigurationRecord.uAttributeReportableChange.zint8ReportableChange);
        eZCL_SetReportableFlag( APP_u8GetDeviceEndpoint(), u16ClusterId, TRUE, FALSE, u16AttributeEnum);
        eZCL_CreateLocalReport( APP_u8GetDeviceEndpoint(), u16ClusterId, 0, TRUE, psAttributeReportingConfigurationRecord);
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

void vLoadDefaultConfigForReportable(void)
{
    memset(asSavedReports, 0 ,sizeof(asSavedReports));
    int i;
    for (i=0; i<NUMBER_OF_REPORTS; i++)
    {
        asSavedReports[i] = asDefaultReports[i];
    }

#if TRACE_REPORT

    DBG_vPrintf(TRACE_REPORT,"\r\nLoaded Defaults Records \r\n");
    for(i=0; i <NUMBER_OF_REPORTS; i++)
    {
        DBG_vPrintf(TRACE_REPORT,"Cluster %04x Type %d Attr %04x Min %d Max %d IntV %d Direct %d Change %d\r\n",
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
 * NAME: vSaveReportableRecord
 *
 * DESCRIPTION:
 * Loads a default configuration
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void vSaveReportableRecord(  uint16 u16ClusterID,
                                    tsZCL_AttributeReportingConfigurationRecord* psAttributeReportingConfigurationRecord)
{
    int iIndex;

    if (u16ClusterID == GENERAL_CLUSTER_ID_ONOFF)
    {
        iIndex = REPORT_ONOFF_SLOT;
        DBG_vPrintf(TRACE_REPORT, "Save to report %d\r\n", iIndex);

        /*For CurrentLevel attribute in LevelControl Cluster*/
        asSavedReports[iIndex].u16ClusterID=u16ClusterID;
        memcpy( &(asSavedReports[iIndex].sAttributeReportingConfigurationRecord),
                psAttributeReportingConfigurationRecord,
                sizeof(tsZCL_AttributeReportingConfigurationRecord) );

        DBG_vPrintf(TRACE_REPORT,"Cluster %04x Type %d Attrib %04x Min %d Max %d IntV %d Direction %d Change %d\r\n",
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



}

static uint32 * vGetLastFiredUTCTimeReportAddress(tsReports * pReport, uint8 endpointId)
{
    uint32 *pLastFiredUTCTimeReport = NULL;
    tsZCL_ReportRecord *psHeadReportRecord =  (tsZCL_ReportRecord *)psDLISTgetHead(&psZCL_Common->lReportAllocList);

    do
    {
        if (psHeadReportRecord == NULL)
            break;
        /* if cluster id matches, then do check */
        if((pReport->u16ClusterID == psHeadReportRecord->psClusterInstance->psClusterDefinition->u16ClusterEnum)
            &&
           (endpointId == psHeadReportRecord->psEndPointDefinition->u8EndPointNumber))
        {
            if((psHeadReportRecord->sAttributeReportingConfigurationRecord.u16AttributeEnum == pReport->sAttributeReportingConfigurationRecord.u16AttributeEnum)
                &&
                (psHeadReportRecord->sAttributeReportingConfigurationRecord.u8DirectionIsReceived == pReport->sAttributeReportingConfigurationRecord.u8DirectionIsReceived))
            {
                pLastFiredUTCTimeReport = &psHeadReportRecord->u32LastFiredUTCTime;
                break;
            }
        }
        // get next
        psHeadReportRecord = (tsZCL_ReportRecord *)psDLISTgetNext((DNODE *)(psHeadReportRecord));
    }
    while(psHeadReportRecord!=NULL);

    return pLastFiredUTCTimeReport;
}

void vSetReportDataForMinRetention(void)
{
    int i;
    tsReports * pReport = NULL;
    uint32 *pLastFiredUTCTimeReport = NULL;
    for (i=0; i<NUMBER_OF_REPORTS; i++)
    {
        pReport = &asSavedReports[i];
        pLastFiredUTCTimeReport = vGetLastFiredUTCTimeReportAddress(pReport, APP_u8GetDeviceEndpoint());
        if (pLastFiredUTCTimeReport != NULL)
        {
            pReport->u32LastFiredUTCTime = *pLastFiredUTCTimeReport;
        }

    }
}

void vRestoreReportAfterSleep(void)
{
    int i;
    tsReports * pReport = NULL;
    uint32 *pLastFiredUTCTimeReport = NULL;
    PDM_teStatus eStatusReportReload = eRestoreReports();
    /*Load the reports from the PDM or the default ones depending on the PDM load record status*/
    if(eStatusReportReload !=PDM_E_STATUS_OK )
    {
         /*Load Defaults if the data was not correct*/
         vLoadDefaultConfigForReportable();
    }
    /*Make the reportable attributes */
    vMakeSupportedAttributesReportable();

    for (i=0; i<NUMBER_OF_REPORTS; i++)
    {
        pReport = &asSavedReports[i];
        pLastFiredUTCTimeReport = vGetLastFiredUTCTimeReportAddress(pReport, APP_u8GetDeviceEndpoint());
        if (pLastFiredUTCTimeReport != NULL)
            *pLastFiredUTCTimeReport = pReport->u32LastFiredUTCTime;
    }

}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
