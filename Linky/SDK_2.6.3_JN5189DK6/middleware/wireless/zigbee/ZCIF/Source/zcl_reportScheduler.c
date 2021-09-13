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
 * MODULE:             Report Cluster
 *
 * COMPONENT:          ReportScheduler.c
 *
 * DESCRIPTION:        Update list of price events as time changes
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>

#include "dlist.h"

#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_internal.h"
#include "zcl_common.h"
#include "dbg.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef BOUND_REPORT_ADDR_MODE
#define BOUND_REPORT_ADDR_MODE E_ZCL_AM_BOUND_NON_BLOCKING
#endif
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

PRIVATE void vReportSchedulerUpdate(uint32 u32UTCTime);

PRIVATE bool_t bHasTimerFired(
               uint32               u32UTCTime,
               tsZCL_ReportRecord  *psHeadReportRecord);

PRIVATE bool_t bPeriodicTimerFired(
               uint32               u32UTCTime,
               tsZCL_ReportRecord  *psHeadReportRecord);

PRIVATE bool_t bReportableDifferenceTimerFired(
               uint32               u32UTCTime,
               tsZCL_ReportRecord  *psHeadReportRecord);
PRIVATE bool_t bIsClusterBound( uint16 u16ClusterId );

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       vReportTimerClickCallback
 **
 ** DESCRIPTION:
 ** Timer Callback registered with the ZCL
 **
 ** PARAMETERS:                 Name               Usage
 ** tsZCL_CallBackEvent        *psCallBackEvent    Timer Server Callback
 **
 ** RETURN:
 ** nothing
 **
 ****************************************************************************/

PUBLIC void vReportTimerClickCallback(tsZCL_CallBackEvent *psCallBackEvent)
{
    vReportSchedulerUpdate(psCallBackEvent->uMessage.sTimerMessage.u32UTCTime);
}

/****************************************************************************
 **
 ** NAME:       vReportSchedulerUpdate
 **
 ** DESCRIPTION:
 ** Sends attribute reports - based on current time
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint32                      u32UTCTime                  Current time
 **
 ** RETURN:
 ** nothing
 **
 ****************************************************************************/

PRIVATE void vReportSchedulerUpdate(uint32 u32UTCTime)
{
    tsZCL_ReportRecord *psHeadReportRecord, *psTransmitReportRecord, *psHeadRecord;

    tsZCL_CallBackEvent sZCL_CallBackEvent;
    bool_t bIsManufacturerSpecific;

    tsZCL_ClusterInstance       *psClusterInstance;
    tsZCL_EndPointDefinition    *psEndPointDefinition;
    tsZCL_AttributeDefinition   *psAttributeDefinition;

    uint8 u8clusterRecordSize;

    PDUM_thAPduInstance myPDUM_thAPduInstance;

    uint16 u16outputOffset, u16responseBufferSize, u16typeSize;

    bool_t bBufferAllocated;

    tsZCL_AttributeReportingConfigurationRecord *psAttributeReportingRecord;

    tsZCL_Address sZCL_Address;

    bool_t bSentReportRequestEvent = FALSE;

    // get first resource
    psHeadRecord = (tsZCL_ReportRecord *)psDLISTgetHead(&psZCL_Common->lReportAllocList);
    if(psHeadRecord==NULL)
    {
        return;
    }

    while(psHeadRecord)
    {
        psHeadReportRecord = psHeadRecord;

        // indicate no buffer ready, initially
        bBufferAllocated=FALSE;

        // search - first report will automatically pass through this
        if(bHasTimerFired(u32UTCTime, psHeadReportRecord))
        {
            if ((ZPS_u8NwkManagerState() == ZPS_ZDO_ST_ACTIVE) &&
                ( bIsClusterBound(psHeadReportRecord->psClusterInstance->psClusterDefinition->u16ClusterEnum) ))
            {
                // store reference address of first report
                psAttributeReportingRecord= &psHeadReportRecord->sAttributeReportingConfigurationRecord;
                psClusterInstance = psHeadReportRecord->psClusterInstance;
                psEndPointDefinition = psHeadReportRecord->psEndPointDefinition;
                psAttributeDefinition = psHeadReportRecord->psAttributeDefinition;

                // check whether timer entry has fired
                if(psAttributeReportingRecord->u8DirectionIsReceived == 0)
                {
                    // issue a report
                    if(bBufferAllocated==FALSE)
                    {
                        // get buffer to write the response in
                        myPDUM_thAPduInstance = hZCL_AllocateAPduInstance();
                        // no buffers - return
                        if(myPDUM_thAPduInstance == PDUM_INVALID_HANDLE)
                        {
                            return;
                        }
                        bBufferAllocated = TRUE;
                        // build manufacturer specific flag by or-ing all the possible options
                        bIsManufacturerSpecific = FALSE;
                        // get attribute flag and set bool flag
                        if(psAttributeDefinition->u8AttributeFlags & E_ZCL_AF_MS)
                        {
                            bIsManufacturerSpecific = TRUE;
                        }
                        // build command packet for response
                        u16outputOffset = u16ZCL_WriteCommandHeader(myPDUM_thAPduInstance,
                                                               eFRAME_TYPE_COMMAND_ACTS_ACCROSS_ENTIRE_PROFILE,
                                                               bIsManufacturerSpecific,
                                                               psEndPointDefinition->u16ManufacturerCode,
                                                               psClusterInstance->bIsServer,
                                                               psEndPointDefinition->bDisableDefaultResponse,
                                                               u8GetTransactionSequenceNumber(),
                                                               E_ZCL_REPORT_ATTRIBUTES);
                    }

                    // size of outgoing buffer
                    u16responseBufferSize = PDUM_u16APduGetSize(psZCL_Common->hZCL_APdu);

                    eZCL_GetAttributeTypeSizeFromStructure(psHeadReportRecord->psAttributeDefinition, pvZCL_GetAttributePointer(psHeadReportRecord->psAttributeDefinition,psClusterInstance, psAttributeReportingRecord->u16AttributeEnum), &u16typeSize);
                    u8clusterRecordSize = 2 + 1 + u16typeSize;

                    // clear TX state flags
                    psTransmitReportRecord = NULL;

                    // write attributes into the outgoing buffer - loop round the attributes in the list
                    while(psHeadReportRecord != NULL)
                    {
                        if(bHasTimerFired(u32UTCTime, psHeadReportRecord)                               &&
                         ((u16outputOffset+u8clusterRecordSize) <= u16responseBufferSize)               &&
                          (psClusterInstance->psClusterDefinition->u16ClusterEnum ==
                           psHeadReportRecord->psClusterInstance->psClusterDefinition->u16ClusterEnum)  &&
                          (psHeadReportRecord->psClusterInstance->bIsServer == psClusterInstance->bIsServer) &&
                           bZCL_CheckManufacturerSpecificAttributeFlagMatch(psHeadReportRecord->psAttributeDefinition, bIsManufacturerSpecific) &&
                          (psEndPointDefinition == psHeadReportRecord->psEndPointDefinition))
                        {
                            // store reference address of report
                            psAttributeReportingRecord= &psHeadReportRecord->sAttributeReportingConfigurationRecord;
                            psAttributeDefinition = psHeadReportRecord->psAttributeDefinition;
                            // both time periods could coincide - if they are multiples
                            // check if the attribute is using periodic reporting
                            if(bPeriodicTimerFired(u32UTCTime, psHeadReportRecord) ||
                              // check if the attribute is using change based reporting
                              (bReportableDifferenceTimerFired(u32UTCTime, psHeadReportRecord) &&
                              (eZCL_IndicateReportableChange(psHeadReportRecord)== E_ZCL_SUCCESS))
                            )
                            {
                                if (!bSentReportRequestEvent)
                                {
                                    // allow the device to update the attributes - express style
                                    sZCL_CallBackEvent.pZPSevent = NULL;
                                    sZCL_CallBackEvent.u8TransactionSequenceNumber = 0;
                                    sZCL_CallBackEvent.u8EndPoint = psEndPointDefinition->u8EndPointNumber;
                                    sZCL_CallBackEvent.eEventType = E_ZCL_CBET_REPORT_REQUEST;
                                    sZCL_CallBackEvent.psClusterInstance = psClusterInstance;
                                    sZCL_CallBackEvent.eZCL_Status = E_ZCL_SUCCESS;
                                    psEndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);

                                    bSentReportRequestEvent = TRUE;
                                }
                                // store last time
                                psHeadReportRecord->u32LastFiredUTCTime = u32UTCTime;

                                // Store last sent value - this was being stored in eZCL_LogAttributeChangeAndIndicateReportableChange - renamed to eZCL_IndicateReportableChange()
                                // but this is called before the E_ZCL_CBET_REPORT_REQUEST and the app may update the att value in the callback.
                                eZCL_StoreChangeAttributeValue(psClusterInstance, psAttributeDefinition, psHeadReportRecord);

                                // store present record entry
                                psTransmitReportRecord = psHeadReportRecord;

                                // write Attribute Id into outgoing buffer
                                u16outputOffset += u16ZCL_APduInstanceWriteNBO(
                                myPDUM_thAPduInstance, u16outputOffset, E_ZCL_ATTRIBUTE_ID, &psAttributeReportingRecord->u16AttributeEnum);
                                // write type
                                u16outputOffset += u16ZCL_APduInstanceWriteNBO(
                                                    myPDUM_thAPduInstance,
                                                    u16outputOffset,
                                                    E_ZCL_UINT8,
                                                   &psAttributeReportingRecord->eAttributeDataType);
                                // value - structure base address in cluster struct, offset in attribute structure
                                u16outputOffset += u16ZCL_WriteAttributeValueIntoBuffer(psEndPointDefinition->u8EndPointNumber,
                                                    psAttributeReportingRecord->u16AttributeEnum,
                                                    psClusterInstance,
                                                    psHeadReportRecord->psAttributeDefinition,
                                                    u16outputOffset,
                                                    myPDUM_thAPduInstance);
                            }
                         }
                        // get next list member
                        psHeadReportRecord = (tsZCL_ReportRecord *)psDLISTgetNext((DNODE *)psHeadReportRecord);
                    }

                    // if there is something to TX
                    if(psTransmitReportRecord!=NULL)
                    {
                        // transmit request
                        sZCL_Address.eAddressMode = BOUND_REPORT_ADDR_MODE;
                        if(eZCL_TransmitDataRequest(myPDUM_thAPduInstance,
                                                    u16outputOffset,
                                                    psEndPointDefinition->u8EndPointNumber,
                                                    0, // No dest EP for bound
                                                    psClusterInstance->psClusterDefinition->u16ClusterEnum,
                                                    &sZCL_Address) != E_ZCL_SUCCESS)
                        {
                            return;
                        }
                    }
                    else
                    {
                        // free buffer and return
                        PDUM_eAPduFreeAPduInstance(myPDUM_thAPduInstance);
                    }
                }
                else
                {
                    if(psAttributeReportingRecord->u16TimeoutPeriodField == REPORTS_OF_ATTRIBUTE_NOT_SUBJECT_TO_TIMEOUT)
                    {
                        psHeadRecord = (tsZCL_ReportRecord *)psDLISTgetNext((DNODE *)psHeadRecord);
                        continue;
                    }

                    // may be a problem with reporting - use c/b to inform the user
                    sZCL_CallBackEvent.pZPSevent = 0;
                    sZCL_CallBackEvent.u8TransactionSequenceNumber = 0;
                    sZCL_CallBackEvent.u8EndPoint = psEndPointDefinition->u8EndPointNumber;
                    sZCL_CallBackEvent.psClusterInstance = psClusterInstance;
                    sZCL_CallBackEvent.eEventType = E_ZCL_CBET_REPORT_TIMEOUT;
                    sZCL_CallBackEvent.eZCL_Status = E_ZCL_SUCCESS;
                    memcpy(&sZCL_CallBackEvent.uMessage.sAttributeReportingConfigurationRecord,
                           &psHeadReportRecord->sAttributeReportingConfigurationRecord,
                            sizeof(tsZCL_AttributeReportingConfigurationRecord));
                    // call user
                    psEndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);
                }

                bBufferAllocated=FALSE;
            }  // if zdo active

        }   // if timer fored
        /* go the next record */
        // get next list member
        psHeadRecord = (tsZCL_ReportRecord *)psDLISTgetNext((DNODE *)psHeadRecord);
    }
}

/****************************************************************************
 **
 ** NAME:       bHasTimerFired
 **
 ** DESCRIPTION:
 ** Determines whether the timer entry has 'fired'
 **
 ** PARAMETERS:                 Name                  Usage
 ** uint32                      u32UTCTime            Current time
 ** tsZCL_ReportRecord         *psHeadReportRecord    Report Record
 **
 ** RETURN:
 ** TRUE/FALSE
 **
 ****************************************************************************/

PRIVATE bool_t bHasTimerFired(
               uint32               u32UTCTime,
               tsZCL_ReportRecord  *psHeadReportRecord)
{
    return(bPeriodicTimerFired(u32UTCTime, psHeadReportRecord) ||
           bReportableDifferenceTimerFired(u32UTCTime, psHeadReportRecord));
}

/****************************************************************************
 **
 ** NAME:       bPeriodicTimerFired
 **
 ** DESCRIPTION:
 ** Determines whether the timer entry has 'fired'
 **
 ** PARAMETERS:                 Name                  Usage
 ** uint32                      u32UTCTime            Current time
 ** tsZCL_ReportRecord         *psHeadReportRecord    Report Record
 **
 ** RETURN:
 ** TRUE/FALSE
 **
 ****************************************************************************/

PRIVATE bool_t bPeriodicTimerFired(
               uint32               u32UTCTime,
               tsZCL_ReportRecord  *psHeadReportRecord)
{

    uint16 u16ReportingInterval;

    // not triggered
    if((psHeadReportRecord->sAttributeReportingConfigurationRecord.u16MaximumReportingInterval == REPORTING_MAXIMUM_TURNED_OFF) ||
       (psHeadReportRecord->sAttributeReportingConfigurationRecord.u16MaximumReportingInterval == REPORTING_MAXIMUM_PERIODIC_TURNED_OFF)
    )
    {
        return(FALSE);
    }

    u16ReportingInterval = psHeadReportRecord->sAttributeReportingConfigurationRecord.u16MaximumReportingInterval;

    if((((u32UTCTime - psHeadReportRecord->u32LastFiredUTCTime)/u16ReportingInterval)!=0) &&
       (((u32UTCTime - psHeadReportRecord->u32LastFiredUTCTime)%u16ReportingInterval)==0)
    )
    {
        return(TRUE);
    }

    return(FALSE);
}


/****************************************************************************
 **
 ** NAME:       bHasTimerFired
 **
 ** DESCRIPTION:
 ** Determines whether the timer entry has 'fired'
 **
 ** PARAMETERS:                 Name                  Usage
 ** uint32                      u32UTCTime            Current time
 ** tsZCL_ReportRecord         *psHeadReportRecord    Report Record
 **
 ** RETURN:
 ** TRUE/FALSE
 **
 ****************************************************************************/

PRIVATE bool_t bReportableDifferenceTimerFired(
               uint32               u32UTCTime,
               tsZCL_ReportRecord  *psHeadReportRecord)
{

    uint16 u16ReportingInterval;

    // not triggered
    if((psHeadReportRecord->sAttributeReportingConfigurationRecord.u16MaximumReportingInterval == REPORTING_MAXIMUM_TURNED_OFF) ||
       (psHeadReportRecord->sAttributeReportingConfigurationRecord.u16MinimumReportingInterval == REPORTING_MINIMUM_NOT_SET)
    )
    {
        // all off
        return(FALSE);
    }

    u16ReportingInterval = psHeadReportRecord->sAttributeReportingConfigurationRecord.u16MinimumReportingInterval;

    // always triggered
    if(u16ReportingInterval == REPORTING_MINIMUM_LIMIT_NONE)
    {
        return(TRUE);
    }

    // is timing satisfied
    if((u32UTCTime - psHeadReportRecord->u32LastFiredUTCTime) >= u16ReportingInterval)
    {
        return(TRUE);
    }

    return(FALSE);
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/
PRIVATE bool_t bIsClusterBound( uint16 u16ClusterId )
{
    uint32   j = 0;

    ZPS_tsAplAib * psAplAib  = ZPS_psAplAibGetAib();

    for( j = 0 ; j < psAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].u32SizeOfBindingTable ; j++ )
    {
        if ( u16ClusterId == psAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].pvAplApsmeBindingTableEntryForSpSrcAddr[j].u16ClusterId )
        {
            return TRUE;
        }
    }
    return FALSE;

}
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

