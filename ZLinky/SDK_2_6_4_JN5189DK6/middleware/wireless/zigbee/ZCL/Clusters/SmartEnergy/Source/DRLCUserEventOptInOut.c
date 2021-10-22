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
 * DESCRIPTION:
 * User event opt in / out
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
#include "DRLC.h"
#include "DRLC_internal.h"


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

/****************************************************************************
 **
 ** NAME:       eSE_DRLCSetEventUserOption
 **
 ** DESCRIPTION:
 ** Allows user to opt-in or out from participating in an event
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint32                      u32IssuerId                 Event Id
 ** uint8                       u8EndPointId                Endpoint
 ** teSE_DRLCUserEventOption    eEventOption                Opt-in or out
 **
 ** RETURN:
 ** teSE_DRLCStatus
 **
 ****************************************************************************/

PUBLIC  teSE_DRLCStatus eSE_DRLCSetEventUserOption(
                            uint32                       u32IssuerId,
                            uint8                        u8EndPointId,
                            teSE_DRLCUserEventOption     eEventOption)
{

    teSE_DRLCEventList eEventList;
    tsSE_DRLCLoadControlEventRecord *psLoadControlEventRecord;
    tsSE_DRLCCustomDataStructure *psDRLCCustomDataStructure;

    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;

    uint8 u8DRLCClusterReturn;

    // error check via EP number
    u8DRLCClusterReturn = eSE_DRLCFindDRLCCluster(u8EndPointId, FALSE, &psEndPointDefinition, &psClusterInstance, &psDRLCCustomDataStructure);
    if(u8DRLCClusterReturn != E_ZCL_SUCCESS)
    {
        return u8DRLCClusterReturn;
    }

    // get mutex call
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // check each list to find the event
    psLoadControlEventRecord = psCheckToSeeIfEventExists(psDRLCCustomDataStructure, u32IssuerId, &eEventList);
    if((psLoadControlEventRecord==NULL) || (eEventList== E_SE_DRLC_EVENT_LIST_CANCELLED))
    {
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return E_SE_DRLC_NOT_FOUND;
    }

    if((eEventList== E_SE_DRLC_EVENT_LIST_ACTIVE) && (eEventOption != psLoadControlEventRecord->eEventOption))
    {
        // increment state change count
        // bad day - wrap round protection
        if(psLoadControlEventRecord->u8EventOptionChangeCount==255)
        {
            psLoadControlEventRecord->u8EventOptionChangeCount=1;
        }
        else
        {
            psLoadControlEventRecord->u8EventOptionChangeCount++;
        }

        // send event to indicate user changes, whilst the event is active
        if(eEventOption == E_SE_DRLC_EVENT_USER_OPT_OUT)
        {
            psLoadControlEventRecord->sReportEvent.u8EventStatus = E_SE_DRLC_USER_CHOSEN_OPT_OUT;
        }
        else
        {
            psLoadControlEventRecord->sReportEvent.u8EventStatus = E_SE_DRLC_USER_CHOSEN_OPT_IN;
        }

        /* User can set opt in/out through call back event which will provided when LCE become active */
        // issuer Id set whenevent added to system, time added when sent
        /* Send event only when Current UTC time more than Event Start Time */
        if(psLoadControlEventRecord->sLoadControlEvent.u32StartTime < u32ZCL_GetUTCTime())
            eSE_DRLCGenerateReportEventStatusMessage(psClusterInstance,&psLoadControlEventRecord->sReportEvent);
    }
    psLoadControlEventRecord->eEventOption = eEventOption;

    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    return(E_ZCL_SUCCESS);
}


/****************************************************************************
 **
 ** NAME:       eSE_DRLCSetEventUserData
 **
 ** DESCRIPTION:
 ** Allows user to set event data.
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint32                      u32IssuerId                 Event Id
 ** uint8                       u8EndPointId                Endpoint
 ** teSE_DRLCUserEventSet       eUserEventSetID             User Event Set ID
 ** uint16                      u16EventData                Event Data
 **
 ** RETURN:
 ** teSE_DRLCStatus
 **
 ****************************************************************************/

PUBLIC  teSE_DRLCStatus eSE_DRLCSetEventUserData(
                            uint32                       u32IssuerId,
                            uint8                        u8EndPointId,
                            teSE_DRLCUserEventSet        eUserEventSetID,
                            uint16                       u16EventData)
{

    teSE_DRLCEventList eEventList;
    tsSE_DRLCLoadControlEventRecord *psLoadControlEventRecord;
    tsSE_DRLCCustomDataStructure *psDRLCCustomDataStructure;

    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;

    uint8 u8DRLCClusterReturn;

    if(eUserEventSetID >= E_SE_DRLC_USER_EVENT_ENUM_END)
    {
        return E_ZCL_ERR_INVALID_VALUE;
    }

    // error check via EP number
    u8DRLCClusterReturn = eSE_DRLCFindDRLCCluster(u8EndPointId, FALSE, &psEndPointDefinition, &psClusterInstance, &psDRLCCustomDataStructure);
    if(u8DRLCClusterReturn != E_ZCL_SUCCESS)
    {
        return u8DRLCClusterReturn;
    }

    // get mutex call
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // check each list to find the event
    psLoadControlEventRecord = psCheckToSeeIfEventExists(psDRLCCustomDataStructure, u32IssuerId, &eEventList);
    if((psLoadControlEventRecord==NULL) || (eEventList!= E_SE_DRLC_EVENT_LIST_ACTIVE))
    {
        #ifndef COOPERATIVE
            eZCL_ReleaseMutex(psEndPointDefinition);
        #endif

        return E_SE_DRLC_NOT_FOUND;
    }

    switch(eUserEventSetID)
    {
        case(E_SE_DRLC_CRITICALITY_LEVEL_APPLIED):
        {
            psLoadControlEventRecord->sReportEvent.u8CriticalityLevelApplied = (uint8)u16EventData;
            break;
        }

        case(E_SE_DRLC_COOLING_TEMPERATURE_SET_POINT_APPLIED):
        {
            psLoadControlEventRecord->sReportEvent.u16CoolingTemperatureSetPointApplied = u16EventData;
            break;
        }

        case(E_SE_DRLC_HEATING_TEMPERATURE_SET_POINT_APPLIED):
        {
            psLoadControlEventRecord->sReportEvent.u16HeatingTemperatureSetPointApplied = u16EventData;
            break;
        }

        case(E_SE_DRLC_AVERAGE_LOAD_ADJUSTMENT_PERCENTAGE_APPLIED):
        {
            psLoadControlEventRecord->sReportEvent.u8AverageLoadAdjustmentPercentageApplied = (uint8)u16EventData;
            break;
        }

        case(E_SE_DRLC_DUTY_CYCLE_APPLIED):
        {
            psLoadControlEventRecord->sReportEvent.u8DutyCycleApplied = (uint8)u16EventData;
            break;
        }

        default:
        {
            // checked on the way in
            break;
        }
    }

    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    return(E_ZCL_SUCCESS);
}

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

