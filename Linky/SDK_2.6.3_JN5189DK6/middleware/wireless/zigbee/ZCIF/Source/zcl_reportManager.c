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
 * MODULE:             Zigbee Demand Response And Load Control Cluster
 *
 * DESCRIPTION:        DRLC table manager
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>
#include <stdlib.h>

#include "zcl_common.h"
#include "dlist.h"

#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_internal.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE teZCL_Status eZCLCheckMatch(
                         tsZCL_AttributeReportingConfigurationRecord    *psAttributeReportingRecord1,
                         tsZCL_AttributeReportingConfigurationRecord    *psAttributeReportingRecord2);

PRIVATE teZCL_Status eZCLCheckToSeeIfReportExists(
                         tsZCL_AttributeReportingConfigurationRecord    *psAttributeReportingRecord,
                         tsZCL_ReportRecord                             **ppsHeadReportRecord,
                         uint16                                         u16ReportClusterId,
                         uint8                                          u8ReportEndpointId);

PRIVATE teZCL_Status eZCLCheckReportingConfigurationRecord(
                         tsZCL_AttributeReportingConfigurationRecord    *psAttributeReportingRecord);


/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eZCL_CreateReportManager
 **
 ** DESCRIPTION:
 ** Creates Report Manager
 **
 ** PARAMETERS:         Name                                    Usage
 ** uint8               u8NumberOfReports                       No. Of Reports
 ** uint16              u16SystemMinimumReportingInterval       Min System Reporting Interval
 ** uint16              u16SystemMaximumReportingInterval       Max System Reporting Interval
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC  teZCL_Status eZCL_CreateReportManager(
                uint8       u8NumberOfReports,
                uint16      u16SystemMinimumReportingInterval,
                uint16      u16SystemMaximumReportingInterval)
{
    int i;

    // create report record array
    vZCL_HeapAlloc(psZCL_Common->psReportRecord,
                tsZCL_ReportRecord,
                (sizeof(tsZCL_ReportRecord))*u8NumberOfReports,
                TRUE,
                "Report_Record");

    if (psZCL_Common->psReportRecord == NULL)
    {
        return E_ZCL_ERR_HEAP_FAIL;
    }

    /* initialise lists */
    vDLISTinitialise(&psZCL_Common->lReportAllocList);
    vDLISTinitialise(&psZCL_Common->lReportDeAllocList);

    psZCL_Common->u8NumberOfReports = u8NumberOfReports;
    psZCL_Common->u16SystemMinimumReportingInterval = u16SystemMinimumReportingInterval;
    psZCL_Common->u16SystemMaximumReportingInterval = u16SystemMaximumReportingInterval;

    // add timer click function to ZCL
    if(eZCL_TimerRegister(E_ZCL_TIMER_CLICK_MS, 0, vReportTimerClickCallback)!= E_ZCL_SUCCESS)
    {
        return(E_ZCL_FAIL);
    }

    // initialise structure
    for(i=0;i<u8NumberOfReports;i++)
    {
        /* add all header slots to the to free list */
        vDLISTaddToHead(&psZCL_Common->lReportDeAllocList, (DNODE *)&psZCL_Common->psReportRecord[i]);
        // initialise
    }

    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
NAME:       eZCL_CreateLocalReport
Description: This function allows a report to be configured on a local device.


 PARAMETERS:
  Type             Name                           Usage
  uint8            u8SourceEndPointId             Local source EP Id
  uint16           u16ClusterId                   Cluster Id
  bool_t           bManufacturerSpecific,
  bool_t           bIsServerAttribute,
  tsZCL_AttributeReportingConfigurationRecord
                   *psAttributeReportingConfigurationRecord
                                                  Pointer to the report to be configured.
                                                  Note that CreateLocalReport configures only one attribute
                                                  at a time and so this parameter is a pointer to a single
                                                  configuration record not an array.
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/


PUBLIC  teZCL_Status eZCL_CreateLocalReport(
                    uint8                       u8SourceEndPointId,
                    uint16                      u16ClusterId,
                    bool_t                      bManufacturerSpecific,
                    bool_t                      bIsServerAttribute,
                    tsZCL_AttributeReportingConfigurationRecord     *psAttributeReportingConfigurationRecord)

{
    tsZCL_ClusterInstance        *psClusterInstance;
    teZCL_Status eStatus;
    tsZCL_AttributeDefinition   *psAttributeDefinition;
    uint16 u16attributeIndex;
    uint8 u8EndPointIndex = 0;

    // parameter checks
    if(psAttributeReportingConfigurationRecord == NULL)
    {
        return(E_ZCL_ERR_PARAMETER_NULL);
    }

    if(eZCL_SearchForEPIndex(u8SourceEndPointId,&u8EndPointIndex) != E_ZCL_SUCCESS)
    {
        return E_ZCL_ERR_EP_UNKNOWN;
    }
    
    eStatus = eZCL_SearchForClusterEntry(u8SourceEndPointId, u16ClusterId, bIsServerAttribute, &psClusterInstance);
    if(eStatus != E_ZCL_SUCCESS)
    {
        return(eStatus);
    }

    // find attribute definition
    eStatus = eZCL_SearchForAttributeEntry(
            u8SourceEndPointId,
            psAttributeReportingConfigurationRecord->u16AttributeEnum,
            bManufacturerSpecific,
            !bIsServerAttribute,
            psClusterInstance,
            &psAttributeDefinition,
            &u16attributeIndex);
            
    if(eStatus != E_ZCL_SUCCESS)
    {
        return(eStatus);
    }


    eStatus = eZCLAddReport(
        psZCL_Common->psZCL_EndPointRecord[u8EndPointIndex].psEndPointDefinition,
        psClusterInstance,
        psAttributeDefinition,
        psAttributeReportingConfigurationRecord);

    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eZCLAddReport
 **
 ** DESCRIPTION:
 ** Add Report
 **
 ** PARAMETERS:                     Name                        Usage
 ** tsZCL_EndPointDefinition        *psEndPointDefinition       EP structure
 ** tsZCL_ClusterInstance           *psClusterInstance          Cluster structure
 **  tsZCL_AttributeDefinition    *psAttributeDefinition,
 ** tsZCL_AttributeReportingConfigurationRecord *psAttributeReportingRecord
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC  teZCL_Status eZCLAddReport(
                                tsZCL_EndPointDefinition     *psEndPointDefinition,
                                tsZCL_ClusterInstance        *psClusterInstance,
                                tsZCL_AttributeDefinition    *psAttributeDefinition,
                                tsZCL_AttributeReportingConfigurationRecord    *psAttributeReportingRecord)
{

    tsZCL_ReportRecord *psHeadReportRecord, *psHeadReportRecordAlloc;
    bool_t bGroupingInsertPointFound;
    teZCL_Status eStatus;
    uint8 *pu8PointerToStringStorage;
    bool_t bDoesEntryExist = FALSE;
    
    #ifdef STRICT_PARAM_CHECK
        if(
           (psEndPointDefinition==NULL)         ||
           (psClusterInstance==NULL)            ||
           (psAttributeDefinition==NULL)        ||
           (psAttributeReportingRecord==NULL))
        {
            return(E_ZCL_ERR_PARAMETER_NULL);
        }
    #endif

    // check report is 'correct' before we add or compare
    eStatus = eZCLCheckReportingConfigurationRecord(psAttributeReportingRecord);
    if(eStatus != E_ZCL_SUCCESS)
    {
        return(eStatus);
    }

    /* If entry is presented in record list then remove it */
    if(eZCLCheckToSeeIfReportExists(psAttributeReportingRecord, &psHeadReportRecord,
            psClusterInstance->psClusterDefinition->u16ClusterEnum,psEndPointDefinition->u8EndPointNumber) == E_ZCL_SUCCESS)
    {
        /* As this entry will be pointing to different structure preserve the heap allocated string storage address */
        pu8PointerToStringStorage = pu8ZCL_GetPointerToStringStorage(psAttributeDefinition->eAttributeDataType,psHeadReportRecord);
        
        psDLISTremove( &psZCL_Common->lReportAllocList, (DNODE *)psHeadReportRecord);
        vDLISTaddToTail(&psZCL_Common->lReportDeAllocList, (DNODE *)psHeadReportRecord);

        bDoesEntryExist = TRUE;
    }

    // we need to get a list item from the dealloc list
    psHeadReportRecord = (tsZCL_ReportRecord *)psDLISTgetHead(&psZCL_Common->lReportDeAllocList);
    
    if(psHeadReportRecord==NULL)
    {
        return E_ZCL_ERR_NO_REPORT_ENTRIES;
    }

    psDLISTremove(&psZCL_Common->lReportDeAllocList, (DNODE *)psHeadReportRecord);

    // set fired time to 'now'
    psHeadReportRecord->u32LastFiredUTCTime = u32ZCL_GetUTCTime();
    // add attribute 'now' value

    if(bDoesEntryExist)
    {
        //in case the attribute is string data type restore the string pointer and assign the maximum length 
        vZCL_SetPointerToStringStorage(psAttributeDefinition->eAttributeDataType,psHeadReportRecord,pu8PointerToStringStorage);
    }else
    {
        //in case the attribute is string data type allocate the space for it if entry does not exist
        vZCL_AllocateSpaceForStringReports(psAttributeDefinition->eAttributeDataType,psHeadReportRecord);
    }

    // copy structure
    psHeadReportRecord->psEndPointDefinition = psEndPointDefinition;
    psHeadReportRecord->psClusterInstance = psClusterInstance;
    psHeadReportRecord->psAttributeDefinition = psAttributeDefinition;

    memcpy(&psHeadReportRecord->sAttributeReportingConfigurationRecord, psAttributeReportingRecord, sizeof(tsZCL_AttributeReportingConfigurationRecord));

    // store new value
    eZCL_StoreChangeAttributeValue(psClusterInstance, psAttributeDefinition, psHeadReportRecord);

    // get head to start search
    psHeadReportRecordAlloc = (tsZCL_ReportRecord *)psDLISTgetHead(&psZCL_Common->lReportAllocList);

    // add in ascending reporting time order
    while((psHeadReportRecordAlloc!=NULL) &&
          (psHeadReportRecordAlloc->sAttributeReportingConfigurationRecord.u16MaximumReportingInterval <
           psAttributeReportingRecord->u16MaximumReportingInterval)
    )
    {
        // get next
        psHeadReportRecordAlloc = (tsZCL_ReportRecord *)psDLISTgetNext((DNODE *)psHeadReportRecordAlloc);
    }

    // at this point the record is either null or has a time >= to u16MaximumReportingInterval

    // check for time groupings
    bGroupingInsertPointFound=FALSE;
    while((psHeadReportRecordAlloc!=NULL)                            &&
          (psHeadReportRecordAlloc->sAttributeReportingConfigurationRecord.u16MaximumReportingInterval ==
           psAttributeReportingRecord->u16MaximumReportingInterval)  &&
          (bGroupingInsertPointFound==FALSE)

    )
    {
       if(
            (psClusterInstance->psClusterDefinition->u16ClusterEnum ==  psHeadReportRecord->psClusterInstance->psClusterDefinition->u16ClusterEnum) &&
            (psClusterInstance->bIsServer == psHeadReportRecord->psClusterInstance->bIsServer)
       )
       {
           bGroupingInsertPointFound=TRUE;
       }
       else
       {
           // get next
           psHeadReportRecordAlloc = (tsZCL_ReportRecord *)psDLISTgetNext((DNODE *)psHeadReportRecordAlloc);
       }
    }

    // at this point the record is either null or has a time >= to u16MaximumReportingInterval,
    // if not null we insert it before the last record reference we have.

    if(psHeadReportRecordAlloc==NULL)
    {
        // add to tail
        vDLISTaddToTail(&psZCL_Common->lReportAllocList, (DNODE *)psHeadReportRecord);
    }
    else
    {
        if(bGroupingInsertPointFound==TRUE)
        {
            vDLISTinsertBefore(&psZCL_Common->lReportAllocList, (DNODE *)psHeadReportRecordAlloc, (DNODE *)psHeadReportRecord);
        }
        else
        {
            vDLISTinsertAfter(&psZCL_Common->lReportAllocList, (DNODE *)psHeadReportRecordAlloc, (DNODE *)psHeadReportRecord);
        }
    }

    return(E_ZCL_SUCCESS);

}

/****************************************************************************
 **
 ** NAME:       eZCLCheckToSeeIfReportExists
 **
 ** DESCRIPTION:
 ** Check if report exists
 **
 ** PARAMETERS:                     Name                        Usage
 ** tsZCL_AttributeReportingConfigurationRecord *psAttributeReportingRecord
 ** tsZCL_ReportRecord            **ppsHeadReportRecord
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PRIVATE  teZCL_Status eZCLCheckToSeeIfReportExists(
                         tsZCL_AttributeReportingConfigurationRecord    *psAttributeReportingRecord,
                         tsZCL_ReportRecord                             **ppsHeadReportRecord,
                         uint16 u16ReportClusterId,
                         uint8 u8ReportEndpointId)
{

    *ppsHeadReportRecord =  (tsZCL_ReportRecord *)psDLISTgetHead(&psZCL_Common->lReportAllocList);

    // check
    while(*ppsHeadReportRecord!=NULL)
    {
        /* if cluster id is is matches, then do check */
        if((u16ReportClusterId == (*ppsHeadReportRecord)->psClusterInstance->psClusterDefinition->u16ClusterEnum)&&
           (u8ReportEndpointId == (*ppsHeadReportRecord)->psEndPointDefinition->u8EndPointNumber))
        {
            if(eZCLCheckMatch(&((*ppsHeadReportRecord)->sAttributeReportingConfigurationRecord),
                                  psAttributeReportingRecord) == E_ZCL_SUCCESS)
            {
                return(E_ZCL_SUCCESS);
            }
        }
        // get next
        *ppsHeadReportRecord = (tsZCL_ReportRecord *)psDLISTgetNext((DNODE *)(*ppsHeadReportRecord));
    }

    return(E_ZCL_FAIL);

}

/****************************************************************************
 **
 ** NAME:       eZCLFindReportEntryByAttributeIdAndDirection
 **
 ** DESCRIPTION:
 ** find report by u16AttributeEnum
 **
 ** PARAMETERS:           Name                        Usage
 ** uint8                 u8DirectionIsReceived
 ** uint16                u16AttributeEnum
 ** tsZCL_ReportRecord  **ppsAttributeReportingRecord
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC  teZCL_Status eZCLFindReportEntryByAttributeIdAndDirection(
                         uint8                                u8EndPoint,
                         uint16                               u16ClusterID,
                         uint8                                u8DirectionIsReceived,
                         uint16                               u16AttributeEnum,
                         tsZCL_AttributeReportingConfigurationRecord    **ppsAttributeReportingRecord)
{

    tsZCL_ReportRecord *psHeadReportRecord;

    // check
    if(ppsAttributeReportingRecord ==NULL)
    {
        return(E_ZCL_ERR_PARAMETER_NULL);
    }

    psHeadReportRecord =  (tsZCL_ReportRecord *)psDLISTgetHead(&psZCL_Common->lReportAllocList);

    // check
    while(psHeadReportRecord!=NULL)
    {
        if((psHeadReportRecord->sAttributeReportingConfigurationRecord.u8DirectionIsReceived == u8DirectionIsReceived) &&
           (psHeadReportRecord->sAttributeReportingConfigurationRecord.u16AttributeEnum == u16AttributeEnum)&&
           (psHeadReportRecord->psClusterInstance->psClusterDefinition->u16ClusterEnum == u16ClusterID)
        )
        {
            *ppsAttributeReportingRecord = &psHeadReportRecord->sAttributeReportingConfigurationRecord;
            return(E_ZCL_SUCCESS);
        }
        // get next
        psHeadReportRecord = (tsZCL_ReportRecord *)psDLISTgetNext((DNODE *)psHeadReportRecord);
    }

    return(E_ZCL_FAIL);

}

/****************************************************************************
 **
 ** NAME:       eZCL_GetNumberOfFreeReportRecordEntries
 **
 ** DESCRIPTION:
 ** gets number of free table entries
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                      *pu8NumberOfEntries          Number of free entries
 **
 ** RETURN:
 ** teSE_PriceStatus
 **
 ****************************************************************************/

PUBLIC  teZCL_Status eZCL_GetNumberOfFreeReportRecordEntries(uint8 *pu8NumberOfEntries)
{

    tsZCL_ReportRecord *psAttributeReportingRecord;

    if(pu8NumberOfEntries==NULL)
    {
        return(E_ZCL_ERR_PARAMETER_NULL);
    }

    *pu8NumberOfEntries = 0;
    // search through dealloc list
    psAttributeReportingRecord = (tsZCL_ReportRecord *)psDLISTgetHead(&psZCL_Common->lReportDeAllocList);
    // search list to count
    while(psAttributeReportingRecord!=NULL)
    {
        (*pu8NumberOfEntries)++;
        psAttributeReportingRecord = (tsZCL_ReportRecord *)psDLISTgetNext((DNODE *)psAttributeReportingRecord);
    }

    return(E_ZCL_SUCCESS);
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eZCLCheckReportingConfigurationRecord
 **
 ** DESCRIPTION:
 ** Check Record is valid
 **
 ** PARAMETERS:                                     Name                        Usage
 ** tsZCL_AttributeReportingConfigurationRecord    *psAttributeReportingRecord
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PRIVATE  teZCL_Status eZCLCheckReportingConfigurationRecord(
             tsZCL_AttributeReportingConfigurationRecord    *psAttributeReportingRecord)
{
    void *pvReportableChange;

    // error checks

    if((psAttributeReportingRecord->u16MaximumReportingInterval != REPORTING_MAXIMUM_PERIODIC_TURNED_OFF ) &&
       (psAttributeReportingRecord->u16MaximumReportingInterval < psZCL_Common->u16SystemMaximumReportingInterval)
    )
    {
        return(E_ZCL_ERR_INVALID_VALUE);
    }

    if((psAttributeReportingRecord->u16MinimumReportingInterval != REPORTING_MINIMUM_LIMIT_NONE ) &&
       (psAttributeReportingRecord->u16MinimumReportingInterval < psZCL_Common->u16SystemMinimumReportingInterval)
    )
    {
        return(E_ZCL_ERR_INVALID_VALUE);
    }

    // system override config

    if(psAttributeReportingRecord->u16MinimumReportingInterval == REPORTING_MINIMUM_LIMIT_NONE)
    {
        psAttributeReportingRecord->u16MinimumReportingInterval = psZCL_Common->u16SystemMinimumReportingInterval;
    }

    // clear reserved bits
    if(psAttributeReportingRecord->u8DirectionIsReceived > 1)
    {
        psAttributeReportingRecord->u8DirectionIsReceived &= 0x01;
    }

    // check to see if attribute is reportable
    if(eZCL_DoesAttributeHaveReportableChange(psAttributeReportingRecord->eAttributeDataType)==E_ZCL_SUCCESS)
    {
        eZCL_GetAttributeReportableEntry(
        &pvReportableChange, psAttributeReportingRecord->eAttributeDataType, &psAttributeReportingRecord->uAttributeReportableChange);

        // check to see if reportable change is enabled
        if(bZCL_CheckAttributeInvalid(psAttributeReportingRecord->eAttributeDataType, pvReportableChange) == TRUE)
        {
            // faf
            if(psAttributeReportingRecord->u16MinimumReportingInterval != REPORTING_MINIMUM_NOT_SET )
            {
                psAttributeReportingRecord->u16MinimumReportingInterval = REPORTING_MINIMUM_NOT_SET;
            }
        }
    }
    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       eZCLCheckMatch
 **
 ** DESCRIPTION:
 ** Check Match
 **
 ** PARAMETERS:                                     Name                        Usage
 ** tsZCL_AttributeReportingConfigurationRecord    *psAttributeReportingRecord1
 ** tsZCL_AttributeReportingConfigurationRecord    *psAttributeReportingRecord2
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PRIVATE  teZCL_Status eZCLCheckMatch(
                         tsZCL_AttributeReportingConfigurationRecord    *psAttributeReportingRecord1,
                         tsZCL_AttributeReportingConfigurationRecord    *psAttributeReportingRecord2)
{

    if((psAttributeReportingRecord1->u16AttributeEnum != psAttributeReportingRecord2->u16AttributeEnum) ||
       (psAttributeReportingRecord1->u8DirectionIsReceived != psAttributeReportingRecord2->u8DirectionIsReceived)

    )
    {
        return(E_ZCL_FAIL);
    }
    return(E_ZCL_SUCCESS);
}
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       vZCL_SetDefaultReporting
 **
 ** DESCRIPTION:
 ** Set the reportable flag for an attribute which is mandatory reportable
 ** based on whether attribute flag has been set to reportable or not 
 **
 ** PARAMETERS:                 Name                           Usage
 ** tsZCL_ClusterInstance       *psClusterInstance             Cluster Instance
 **
 ** RETURN:
 ** None
 **
 **
 ****************************************************************************/
PUBLIC void vZCL_SetDefaultReporting(
                    tsZCL_ClusterInstance       *psClusterInstance)
{
    int i;
    #ifdef STRICT_PARAM_CHECK 
        /* Parameter check */
        if((psClusterInstance==NULL) || 
           (psClusterInstance->psClusterDefinition==NULL)  ||
           (psClusterInstance->pu8AttributeControlBits==NULL))
        {
            return;
        }
    #endif
    
    for (i=0; i<psClusterInstance->psClusterDefinition->u16NumberOfAttributes; i++)
    {
        if(ZCL_IS_BIT_SET(uint8,psClusterInstance->psClusterDefinition->psAttributeDefinition[i].u8AttributeFlags,E_ZCL_AF_RP))
        {
            ZCL_BIT_SET(uint8,psClusterInstance->pu8AttributeControlBits[i],E_ZCL_ACF_RP);
        }
    }
}

/****************************************************************************
 **
 ** NAME:       eZCL_SetReportableFlag
 **
 ** DESCRIPTION:
 ** Set the reportable flag for an attribute at run time.  RP flag in e.g. SE is never set by default.
 ** Only needed on a server.  A server will not allow reports to be configured without this flag being set.
 **
 ** PARAMETERS:      Name                           Usage
 ** u8SrcEndPoint                Source End Point - the bit will be set in the attribute definition for all end points
 **                                             but this EP is used to search for the definition and check the cluster is on this EP.
 ** u6ClusterID                  Cluster Id of the attribute.
 ** bIsServerClusterInstance     Is server attribute
 ** bManufacturerSpecific        Is manufacturer specific attribute
 ** u16AttributeId               The attribute to set the RP bit on.
 **
 ** RETURN:
 ** teZCL_Status
 **
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eZCL_SetReportableFlag(
                    uint8                       u8SrcEndPoint,
                    uint16                      u6ClusterID,
					bool_t                        bIsServerClusterInstance,
					bool_t                        bManufacturerSpecific,
                    uint16                      u16AttributeId)
{
    tsZCL_ClusterInstance *psClusterInst;
    teZCL_Status eStatus;
    int i;
    uint16 u16Index = 0, u16TempAttrId = 0;


    //Point to cluster instance
    if((eStatus = eZCL_SearchForClusterEntry(
              u8SrcEndPoint,
              u6ClusterID,
              bIsServerClusterInstance,
              &psClusterInst)) != E_ZCL_SUCCESS)
    {
        return eStatus;
    }
    if(psClusterInst->psClusterDefinition->u16NumberOfAttributes)
    {
        u16TempAttrId = psClusterInst->psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeEnum;
    }
    for (i=0; i<psClusterInst->psClusterDefinition->u16NumberOfAttributes; i++)
    {
        if((u16TempAttrId == u16AttributeId) &&
                    bZCL_CheckManufacturerSpecificAttributeFlagMatch(&psClusterInst->psClusterDefinition->psAttributeDefinition[u16Index], bManufacturerSpecific))
        {
            if(bZCL_CheckAttributeDirectionFlagMatch(&psClusterInst->psClusterDefinition->psAttributeDefinition[u16Index],bIsServerClusterInstance))
            {
                ZCL_BIT_SET(uint8,psClusterInst->pu8AttributeControlBits[i],E_ZCL_ACF_RP);
                return E_ZCL_SUCCESS;
            }
        }

        if((psClusterInst->psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeArrayLength != 0)&&
        ((u16TempAttrId - psClusterInst->psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeEnum) < psClusterInst->psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeArrayLength))
        {
            u16TempAttrId++;
        }
        else
        {
            u16Index++;
            u16TempAttrId = psClusterInst->psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeEnum;
        }
    }
    return E_ZCL_ERR_ATTRIBUTE_NOT_FOUND;
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

