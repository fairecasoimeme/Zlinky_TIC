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
 * MODULE:             Smart Energy
 *
 * COMPONENT:          zcl.c
 *
 * DESCRIPTION:       ZCL top level functions
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include <string.h>

#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_common.h"
#include "zcl_heap.h"
#include "zcl_internal.h"

#include "pdum_apl.h"
#include "zps_apl.h"
#include "zps_apl_af.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE teZCL_Status eZCL_CheckEPstructureIntegrity(
                        tsZCL_EndPointDefinition    *psEndPointDefinition);
PRIVATE teZCL_Status eZCL_CheckClusterStructureIntegrity(
                        tsZCL_EndPointDefinition    *psEndPointDefinition);
PRIVATE teZCL_Status eZCL_CheckAttributeStructureIntegrity(
                        tsZCL_ClusterDefinition     *psZCL_ClusterDefinition);
PRIVATE teZCL_Status eZCL_CreateEndpointList(
                        uint8                        u8NumberOfEndpoints);
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
// common structure for resource allocation and overall configuration
PUBLIC tsZCL_Common *psZCL_Common;

#ifdef CLD_BIND_SERVER
tsBindServerBufferedPDURecord    asBindServerBufferedPDURecord[MAX_NUM_BIND_QUEUE_BUFFERS];
#endif

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE tsZCL_Common sZCL_Common;
PRIVATE uint8 u8NumberOfRegEndpoints = 0;
/****************************************************************************/
/***        Public Functions                                              ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eZCL_CreateZCL
 **
 ** DESCRIPTION:
 ** Creates ZCL device
 **
 ** PARAMETERS:                 Name                                Usage
 ** tsZCL_Config                psZCL_Config                        Struct containing config
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC  teZCL_Status eZCL_CreateZCL(tsZCL_Config *psZCL_Config)
{

    teZCL_Status eCreateEndpointListResult, eCreateTimerResult;
    teZCL_Status eCreateOptionalManagersResult;

    #ifdef  CLD_BIND_SERVER
        uint8 u8LoopIndex = 0;
    #endif

    if((psZCL_Config == NULL) ||
       (psZCL_Config->pfZCLcallBackFunction == NULL) ||
       (psZCL_Config->hAPdu == PDUM_INVALID_HANDLE) ||
       (psZCL_Config->pu32ZCL_Heap == NULL) ||
       (psZCL_Config->u32ZCL_HeapSizeInWords == 0))
    {
        return(E_ZCL_ERR_PARAMETER_NULL);
    }

    // create ZCL common structure pointer for reference elsewhere
    psZCL_Common = &sZCL_Common;
    memset(&sZCL_Common, 0, sizeof(sZCL_Common));
    u8NumberOfRegEndpoints = 0;

    psZCL_Common->u8TransactionSequenceNumber = 0;
    psZCL_Common->pfZCLinternalCallBackFunction = psZCL_Config->pfZCLcallBackFunction;
    psZCL_Common->hZCL_APdu = psZCL_Config->hAPdu;
    psZCL_Common->u32UTCTime = 0;
    psZCL_Common->eLastZpsError = ZPS_E_SUCCESS;
    psZCL_Common->bTimeHasBeenSynchronised = FALSE;
    psZCL_Common->eSecuritySupported = E_ZCL_SECURITY_NETWORK;
    psZCL_Common->bDisableAPSACK = ZCL_DISABLE_APS_ACK;

    //initialise heap parameters
    psZCL_Common->u32HeapStart = (uint32)psZCL_Config->pu32ZCL_Heap;
    psZCL_Common->u32HeapEnd   = (uint32)(&psZCL_Config->pu32ZCL_Heap[psZCL_Config->u32ZCL_HeapSizeInWords-1]);

#ifdef PC_PLATFORM_BUILD
    psZCL_Common->bPCtestDisableAttributeChecking = TRUE;
#endif

    // Create endpoint record array
    eCreateEndpointListResult = eZCL_CreateEndpointList(psZCL_Config->u8NumberOfEndpoints);
    if(eCreateEndpointListResult != E_ZCL_SUCCESS)
    {
        return eCreateEndpointListResult;
    }

    // Create Timers
    eCreateTimerResult = eZCL_CreateTimer(psZCL_Config->u8NumberOfTimers + ZCL_INTERNAL_TIMERS);
    if(eCreateTimerResult != E_ZCL_SUCCESS)
    {
        return eCreateTimerResult;
    }

    // Create optional managers, currently reporting is only optional manager
    // Could make timers optional using this mechanism as well by moving the above
    // eZCL_CreateTimer call into eZCL_CreateOptionalManagers and adding an extra
    // parameter to eZCL_CreateOptionalManagers for the number of timers
    eCreateOptionalManagersResult = eZCL_CreateOptionalManagers(psZCL_Config->u8NumberOfReports,
                                                                psZCL_Config->u16SystemMinimumReportingInterval,
                                                                psZCL_Config->u16SystemMaximumReportingInterval);
    if(eCreateOptionalManagersResult != E_ZCL_SUCCESS)
    {
        return eCreateOptionalManagersResult;
    }

    // Register a signature creation function
    psZCL_Common->pfZCLSignatureCreationFunction = psZCL_Config->pfZCLSignatureCreationFunction;

    // Register a signature verification function
    psZCL_Common->pfZCLSignatureVerificationFunction = psZCL_Config->pfZCLSignatureVerificationFunction;


#ifdef  CLD_BIND_SERVER
    /*  Init Buffer lists */
    vDLISTinitialise(&psZCL_Common->sBindServerCustomData.lBindServerAllocList);
    vDLISTinitialise(&psZCL_Common->sBindServerCustomData.lBindServerDeAllocList);

    /* Add free buffers to Dealloc list as tail */
    for(u8LoopIndex = 0; u8LoopIndex < MAX_NUM_BIND_QUEUE_BUFFERS; u8LoopIndex++)
    {
        vDLISTaddToTail(&psZCL_Common->sBindServerCustomData.lBindServerDeAllocList,
                (DNODE *)&asBindServerBufferedPDURecord[u8LoopIndex]);
    }

#endif

    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       eZCL_SetCustomCallBackEvent
 **
 ** DESCRIPTION:
 ** sets the part of the callback that is message dependant
 **
 ** PARAMETERS:               Name                          Usage
 ** tsZCL_CallBackEvent      *psCallBackEvent               Callback message
 ** ZPS_tsAfEvent            *pZPSevent                     Zigbee Event
 ** uint8                     u8TransactionSequenceNumber   sequence number
 ** uint8                     u8EndPoint                    endpoint number
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eZCL_SetCustomCallBackEvent(
                       tsZCL_CallBackEvent           *psCallBackEvent,
                       ZPS_tsAfEvent                 *pZPSevent,
                       uint8                          u8TransactionSequenceNumber,
                       uint8                          u8EndPoint)
{
    if(psCallBackEvent==NULL)
    {
        return(E_ZCL_ERR_PARAMETER_NULL);
    }

    // fill in callback event structure
    psCallBackEvent->u8EndPoint = u8EndPoint;
    psCallBackEvent->pZPSevent = pZPSevent;
    psCallBackEvent->u8TransactionSequenceNumber = u8TransactionSequenceNumber;
    // default setting
    psCallBackEvent->eZCL_Status = E_ZCL_SUCCESS;

    return(E_ZCL_SUCCESS);
}
#ifndef COOPERATIVE
/****************************************************************************
 **
 ** NAME:       eZCL_GetMutex
 **
 ** DESCRIPTION:
 ** grabs the ZCL Mutex
 **
 ** PARAMETERS:               Name                    Usage
 ** tsZCL_EndPointDefinition *psEndPointDefinition    Pointer to endpoint structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eZCL_GetMutex(tsZCL_EndPointDefinition *psEndPointDefinition)
{
    if(psEndPointDefinition == NULL)
    {
        return(E_ZCL_FAIL);
    }

    // set up mutex call
    memset(&psZCL_Common->sZCL_CallBackEvent, 0, sizeof(tsZCL_CallBackEvent));
    psZCL_Common->sZCL_CallBackEvent.u8EndPoint = psEndPointDefinition->u8EndPointNumber;
    psZCL_Common->sZCL_CallBackEvent.eEventType = E_ZCL_CBET_LOCK_MUTEX;
    // get EP mutex
    psEndPointDefinition->pCallBackFunctions(&psZCL_Common->sZCL_CallBackEvent);

    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       eZCL_ReleaseMutex
 **
 ** DESCRIPTION:
 ** releases the ZCL Mutex
 **
 ** PARAMETERS:               Name                    Usage
 ** tsZCL_EndPointDefinition *psEndPointDefinition    Pointer to endpoint structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eZCL_ReleaseMutex(tsZCL_EndPointDefinition *psEndPointDefinition)
{
    if((psEndPointDefinition == NULL) ||
       (psZCL_Common->sZCL_CallBackEvent.u8EndPoint != psEndPointDefinition->u8EndPointNumber))
    {
        return(E_ZCL_FAIL);
    }

    // set up mutex call
    psZCL_Common->sZCL_CallBackEvent.eEventType = E_ZCL_CBET_UNLOCK_MUTEX;
    // get EP mutex
    psEndPointDefinition->pCallBackFunctions(&psZCL_Common->sZCL_CallBackEvent);

    return(E_ZCL_SUCCESS);
}

#endif
/****************************************************************************
 **
 ** NAME:       vZCL_GetInternalMutex
 **
 ** DESCRIPTION:
 ** grabs the ZCL Mutex
 **
 ** PARAMETERS:               Name                    Usage
 ** None
 **
 ** RETURN:
 ** nothing
 **
 ****************************************************************************/
#ifndef COOPERATIVE
PUBLIC void vZCL_GetInternalMutex(void)
{
    // set up mutex call
    memset(&psZCL_Common->sZCL_InternalCallBackEvent, 0, sizeof(tsZCL_CallBackEvent));
    psZCL_Common->sZCL_InternalCallBackEvent.eEventType = E_ZCL_CBET_LOCK_MUTEX;
    // get EP mutex
    psZCL_Common->pfZCLinternalCallBackFunction(&psZCL_Common->sZCL_InternalCallBackEvent);

}
#endif
/****************************************************************************
 **
 ** NAME:       vZCL_GetInternalMutex
 **
 ** DESCRIPTION:
 ** releases the ZCL Mutex
 **
 ** PARAMETERS:               Name                    Usage
 ** None
 **
 ** RETURN:
 ** nothing
 **
 ****************************************************************************/
#ifndef COOPERATIVE
PUBLIC void vZCL_ReleaseInternalMutex(void)
{

    // set up mutex call
    psZCL_Common->sZCL_InternalCallBackEvent.eEventType = E_ZCL_CBET_UNLOCK_MUTEX;
    // get EP mutex
    psZCL_Common->pfZCLinternalCallBackFunction(&psZCL_Common->sZCL_InternalCallBackEvent);

}
#endif
/****************************************************************************
 **
 ** NAME:       eZCL_Register
 **
 ** DESCRIPTION:
 ** Registers a Zigbee Endpoint with the ZCL device
 **
 ** PARAMETERS:               Name                    Usage
 ** tsZCL_EndPointDefinition *psEndPointDefinition    Pointer to endpoint structure
 **
 ** RETURN:
 ** None
 **
 ****************************************************************************/

PUBLIC  teZCL_Status eZCL_Register(tsZCL_EndPointDefinition *psEndPointDefinition)
{
    teZCL_Status eEPstructureIntegrity;
    uint8 u8EndPointIndex = 0;

    // parameter check
    if(psEndPointDefinition == NULL)
    {
        return(E_ZCL_ERR_PARAMETER_NULL);
    }

    // basic structure integrity
    eEPstructureIntegrity = eZCL_CheckEPstructureIntegrity(psEndPointDefinition);
    if(eEPstructureIntegrity != E_ZCL_SUCCESS)
    {
        return(eEPstructureIntegrity);
    }

    /* Check if endpoint already exist in list or not, if yes use the old endpoint index
       to point to new endpoint definition (considering something has changed) */
    if((eZCL_SearchForEPIndex(psEndPointDefinition->u8EndPointNumber,&u8EndPointIndex) == E_ZCL_SUCCESS) &&
        (psZCL_Common->psZCL_EndPointRecord[u8EndPointIndex].bRegistered == TRUE))
    {
        psZCL_Common->psZCL_EndPointRecord[u8EndPointIndex].psEndPointDefinition = psEndPointDefinition;
    }
    else
    {

        // store cluster reference
        if (u8NumberOfRegEndpoints < psZCL_Common->u8NumberOfEndpoints)
        {
            psZCL_Common->psZCL_EndPointRecord[u8NumberOfRegEndpoints].bRegistered = TRUE;
            psZCL_Common->psZCL_EndPointRecord[u8NumberOfRegEndpoints].psEndPointDefinition = psEndPointDefinition;

            //Increment number of registered endpoints
            u8NumberOfRegEndpoints++;
        }
    }
    
    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       u8ZCL_GetNumberOfEndpointsRegistered
 **
 ** DESCRIPTION:
 ** Returns number of EP's Registered with the ZCL
 **
 ** PARAMETERS:               Name                    Usage
 ** none
 **
 ** RETURN:
 ** uint8 number of EP's
 **
 ****************************************************************************/

PUBLIC uint8 u8ZCL_GetNumberOfEndpointsRegistered(void)
{
    uint8 u8NumberOfEndpoints;

    // get ZCL mutex
    #ifndef COOPERATIVE
       vZCL_GetInternalMutex();
    #endif

    u8NumberOfEndpoints = psZCL_Common->u8NumberOfEndpoints;
#ifndef COOPERATIVE
    vZCL_ReleaseInternalMutex();
#endif
    return(u8NumberOfEndpoints);

}

/****************************************************************************
 **
 ** NAME:       eZCL_CreateEndpointList
 **
 ** DESCRIPTION:
 ** creates internal EP database
 **
 ** PARAMETERS:     Name                    Usage
 ** uint8           u8NumberOfEndpoints     Number of Endpoints
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PRIVATE  teZCL_Status eZCL_CreateEndpointList(uint8 u8NumberOfEndpoints)
{
    int i;

    // create endpoint record array
    vZCL_HeapAlloc(psZCL_Common->psZCL_EndPointRecord, tsZCL_EndPointRecord, (sizeof(tsZCL_EndPointRecord))*u8NumberOfEndpoints, TRUE, "EP_Record");
    if (psZCL_Common->psZCL_EndPointRecord == NULL)
    {
        return E_ZCL_ERR_HEAP_FAIL;
    }

    // initialise structure
    for(i=0;i<u8NumberOfEndpoints;i++)
    {
        psZCL_Common->psZCL_EndPointRecord[i].bRegistered = FALSE;
    }

    psZCL_Common->u8NumberOfEndpoints = u8NumberOfEndpoints;

    return(E_ZCL_SUCCESS);

}

/****************************************************************************
 **
 ** NAME:       eZCL_CheckEPstructureIntegrity
 **
 ** DESCRIPTION:
 ** checks validity of a user EP structure
 **
 ** PARAMETERS:               Name                    Usage
 ** tsZCL_EndPointDefinition *psEndPointDefinition    Pointer to endpoint structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PRIVATE  teZCL_Status eZCL_CheckEPstructureIntegrity(tsZCL_EndPointDefinition *psEndPointDefinition)
{
    // structure integrity
    if((psEndPointDefinition->u8EndPointNumber == 0) ||
       (psEndPointDefinition->u8EndPointNumber > MAX_ENDPOINT_ID)
       )
    {
        return(E_ZCL_ERR_EP_RANGE);
    }

    if(psEndPointDefinition->u16NumberOfClusters == 0)
    {
        return(E_ZCL_ERR_CLUSTER_0);
    }

    if(psEndPointDefinition->pCallBackFunctions == NULL)
    {
        return(E_ZCL_ERR_CALLBACK_NULL);
    }

    // check cluster structure
    return eZCL_CheckClusterStructureIntegrity(psEndPointDefinition);

}

/****************************************************************************
 **
 ** NAME:       eZCL_CheckClusterStructureIntegrity
 **
 ** DESCRIPTION:
 ** checks validity of a user Cluster structure
 **
 ** PARAMETERS:               Name                    Usage
 ** tsZCL_EndPointDefinition *psEndPointDefinition    Pointer to endpoint structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PRIVATE  teZCL_Status eZCL_CheckClusterStructureIntegrity(tsZCL_EndPointDefinition *psEndPointDefinition)
{
    int i, j;
    teZCL_Status eReturnValue;
    uint8 u8typeSize;

    int iKeyClusterCount;

    uint16 u16previousAttributeEnum = 0;
    uint16 u16AttributeEnum = 0,u16Index;

    if(psEndPointDefinition->psClusterInstance == NULL)
    {
        return(E_ZCL_ERR_CLUSTER_NULL);
    }

    iKeyClusterCount = 0;

    for(i=0; i<psEndPointDefinition->u16NumberOfClusters; i++)
    {
        /* we can support a cluster without any attributes. most of client
         * cluster does not have attributes*/

       /* if((psEndPointDefinition->psClusterInstance[i].pvEndPointSharedStructPtr != NULL) &&
           (psEndPointDefinition->psClusterInstance[i].pu8AttributeControlBits == NULL))
        {
            return(E_ZCL_ERR_ATTRIBUTES_NULL);
        }*/

        if(psEndPointDefinition->psClusterInstance[i].psClusterDefinition->u16ClusterEnum == SE_CLUSTER_ID_KEY_ESTABLISHMENT)
        {
            iKeyClusterCount++;
            if(iKeyClusterCount > 1)
            {
                return(E_ZCL_ERR_KEY_ESTABLISHMENT_MORE_THAN_ONE_CLUSTER);
            }
        }

        if(psEndPointDefinition->psClusterInstance[i].psClusterDefinition   == NULL)
        {
            return(E_ZCL_ERR_CLUSTER_NULL);
        }

        if((psEndPointDefinition->psClusterInstance[i].psClusterDefinition->u8ClusterControlFlags  & SEND_SECURITY_MASK) >= E_ZCL_SECURITY_ENUM_END)
        {
            return(E_ZCL_ERR_SECURITY_RANGE);
        }

        if(((psEndPointDefinition->bIsManufacturerSpecificProfile == FALSE) &&
            (psEndPointDefinition->psClusterInstance[i].psClusterDefinition->u16ClusterEnum > MAX_STD_CLUSTER_ID) &&
            (psEndPointDefinition->psClusterInstance[i].psClusterDefinition->u16ClusterEnum < MIN_MAN_CLUSTER_ID))
        )
        {
            return(E_ZCL_ERR_CLUSTER_ID_RANGE);
        }

        // if cluster in manufacturer specific - Id should be in a certain range
        if(((psEndPointDefinition->psClusterInstance[i].psClusterDefinition->bIsManufacturerSpecificCluster  == TRUE)) &&
           (psEndPointDefinition->psClusterInstance[i].psClusterDefinition->u16ClusterEnum < MIN_MAN_CLUSTER_ID)
        )
        {
            return(E_ZCL_ERR_MANUFACTURER_SPECIFIC);
        }
        
        eReturnValue = eZCL_CheckAttributeStructureIntegrity(psEndPointDefinition->psClusterInstance[i].psClusterDefinition);
        if(eReturnValue != E_ZCL_SUCCESS)
        {
            return(eReturnValue);
        }

        if(psEndPointDefinition->psClusterInstance[i].psClusterDefinition->u16NumberOfAttributes != 0)
        {
            u16AttributeEnum = psEndPointDefinition->psClusterInstance[i].psClusterDefinition->psAttributeDefinition[0].u16AttributeEnum;
        }
        u16Index = 0;
        // clear attribute status table and check attribute definition order - smallest first
        for(j=0; j<psEndPointDefinition->psClusterInstance[i].psClusterDefinition->u16NumberOfAttributes; j++)
        {
#ifdef PC_PLATFORM_BUILD
            if(psZCL_Common->bPCtestDisableAttributeChecking==FALSE)
#endif
            {
                // check attribute type is supported
                if(eZCL_GetAttributeTypeSize(psEndPointDefinition->psClusterInstance[i].psClusterDefinition->psAttributeDefinition[u16Index].eAttributeDataType, &u8typeSize)!= E_ZCL_SUCCESS)
                {
                    return(E_ZCL_ERR_ATTRIBUTE_TYPE_UNSUPPORTED);
                }
            }

            // allow floats in a cluster definition without the float library being included as long as the attribute isn't reportable,
            // and reporting isn't enabled
#if defined ZCL_CONFIGURE_ATTRIBUTE_REPORTING_SERVER_SUPPORTED || defined ZCL_CONFIGURE_ATTRIBUTE_REPORTING_CLIENT_SUPPORTED
#ifndef  ZCL_ENABLE_FLOAT
            if(
               ((psEndPointDefinition->psClusterInstance[i].psClusterDefinition->psAttributeDefinition[u16Index].eAttributeDataType == E_ZCL_FLOAT_SEMI) ||
                (psEndPointDefinition->psClusterInstance[i].psClusterDefinition->psAttributeDefinition[u16Index].eAttributeDataType == E_ZCL_FLOAT_SINGLE) ||
                (psEndPointDefinition->psClusterInstance[i].psClusterDefinition->psAttributeDefinition[u16Index].eAttributeDataType == E_ZCL_FLOAT_DOUBLE)) &&
                (ZCL_IS_BIT_SET(uint8,psEndPointDefinition->psClusterInstance[i].pu8AttributeControlBits[j],E_ZCL_ACF_RP))
            )
            {
                // cluster not valid
                return(E_ZCL_ERR_ATTRIBUTE_TYPE_UNSUPPORTED);
            }
#endif  // Float not enabled
#endif // Reporting enabled

            if(j==0)
            {
                u16previousAttributeEnum = u16AttributeEnum;
                //psEndPointDefinition->psClusterInstance[i].psClusterDefinition->psAttributeDefinition[0].u16AttributeEnum;
            }
            else
            {
                //u16AttributeEnum = u16TempAttrId;
                //psEndPointDefinition->psClusterInstance[i].psClusterDefinition->psAttributeDefinition[j].u16AttributeEnum;
                // spot sequence and replication errors
                if(u16previousAttributeEnum >= u16AttributeEnum)
                {
                    if (((psEndPointDefinition->psClusterInstance[i].psClusterDefinition->psAttributeDefinition[u16Index].u8AttributeFlags & E_ZCL_AF_MS) !=
                        (psEndPointDefinition->psClusterInstance[i].psClusterDefinition->psAttributeDefinition[u16Index-1].u8AttributeFlags & E_ZCL_AF_MS))||
                        ((psEndPointDefinition->psClusterInstance[i].psClusterDefinition->psAttributeDefinition[u16Index].u8AttributeFlags & E_ZCL_AF_CA) !=
                        (psEndPointDefinition->psClusterInstance[i].psClusterDefinition->psAttributeDefinition[u16Index-1].u8AttributeFlags & E_ZCL_AF_CA)))
                    {
                        // Manufacturer specific and non manuf specific with the incremental sequence break is allowed
                        // client attribute and server attribute with the incremental sequence break is allowed
                    }
                    else
                    {
                        return(E_ZCL_ERR_ATTRIBUTE_ID_ORDER);
                    }
                }
                // update previous
                u16previousAttributeEnum = u16AttributeEnum;
            }

            // if cluster in manufacturer specific - so should all attributes
            if((psEndPointDefinition->psClusterInstance[i].psClusterDefinition->bIsManufacturerSpecificCluster  == TRUE) &&
               ((psEndPointDefinition->psClusterInstance[i].psClusterDefinition->psAttributeDefinition[u16Index].u8AttributeFlags & E_ZCL_AF_MS) != E_ZCL_AF_MS)
            )
            {
                return(E_ZCL_ERR_MANUFACTURER_SPECIFIC);
            }
            ZCL_BIT_CLEAR(uint8, psEndPointDefinition->psClusterInstance[i].pu8AttributeControlBits[j],E_ZCL_ACF_RS);

            if((psEndPointDefinition->psClusterInstance[i].psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeArrayLength != 0)&&
              ((u16AttributeEnum - psEndPointDefinition->psClusterInstance[i].psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeEnum) < psEndPointDefinition->psClusterInstance[i].psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeArrayLength))
            {
                u16AttributeEnum++;
            }
            else
            {
                u16Index++;
                u16AttributeEnum = psEndPointDefinition->psClusterInstance[i].psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeEnum;
            }
        }
    }

    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       eZCL_CheckAttributeStructureIntegrity
 **
 ** DESCRIPTION:
 ** checks validity of a user Attribute structure
 **
 ** PARAMETERS:               Name                     Usage
 ** tsZCL_ClusterDefinition  *psZCL_ClusterDefinition  Pointer to cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PRIVATE  teZCL_Status eZCL_CheckAttributeStructureIntegrity(tsZCL_ClusterDefinition *psZCL_ClusterDefinition)
{
    uint32 i;
    uint16 u16AttributeID = 0, u16Index = 0;

    if(psZCL_ClusterDefinition->u16NumberOfAttributes != 0)
    {
        u16AttributeID = psZCL_ClusterDefinition->psAttributeDefinition[u16Index].u16AttributeEnum;
    }

    for(i=0; i< psZCL_ClusterDefinition->u16NumberOfAttributes; i++)
    {

        // should have at least one flag set
        if(psZCL_ClusterDefinition->psAttributeDefinition[u16Index].u8AttributeFlags == 0)
        {
            return(E_ZCL_ERR_ATTRIBUTES_ACCESS);
        }
        if((psZCL_ClusterDefinition->psAttributeDefinition[u16Index].u16AttributeArrayLength != 0)&&
          ((u16AttributeID - psZCL_ClusterDefinition->psAttributeDefinition[u16Index].u16AttributeEnum) < psZCL_ClusterDefinition->psAttributeDefinition[u16Index].u16AttributeArrayLength))
        {
            u16AttributeID++;
        }
        else
        {
            u16Index++;
            u16AttributeID = psZCL_ClusterDefinition->psAttributeDefinition[u16Index].u16AttributeEnum;
        }
    }
    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       eZCL_SearchForEPIndex
 **
 ** DESCRIPTION:
 ** Searches For An Endpoint Index In The EndpointRecord
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8endpointId                EP Id
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eZCL_SearchForEPIndex(
                        uint8                       u8endpointId,
                        uint8                       *pu8EndpointIndex)
{
    uint8 i;

    if(pu8EndpointIndex == NULL)
        return E_ZCL_FAIL;

    for(i=0; i<psZCL_Common->u8NumberOfEndpoints; i++)
    {
        if(psZCL_Common->psZCL_EndPointRecord[i].psEndPointDefinition->u8EndPointNumber == u8endpointId)
        {
            *pu8EndpointIndex = i;
            return E_ZCL_SUCCESS;
        }
    }
    return E_ZCL_FAIL;
}
/****************************************************************************
 **
 ** NAME:       u8ZCL_GetEPIdFromIndex
 **
 ** DESCRIPTION:
 ** Returns An Endpoint enum based on the Endpoint Index in endpoint list
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8EndpointIndex             EP Index
 **
 ** RETURN:
 ** uint8
 **
 ****************************************************************************/

PUBLIC uint8 u8ZCL_GetEPIdFromIndex(
                        uint8                       u8EndpointIndex)
{
    if(u8EndpointIndex < psZCL_Common->u8NumberOfEndpoints)
        return(psZCL_Common->psZCL_EndPointRecord[u8EndpointIndex].psEndPointDefinition->u8EndPointNumber);
    else
        return 0;
}
/****************************************************************************
 **
 ** NAME:       eZCL_GetLastZpsError
 **
 ** DESCRIPTION:
 ** Returns the last Zigbee Pro Stack error code that has been set, normally during a transmit.
 **
 ** PARAMETERS:            Name                           Usage
 **
 ** RETURN:
 ** ZPS_teStatus
 **
 ****************************************************************************/
PUBLIC ZPS_teStatus eZCL_GetLastZpsError(void)
{
    return psZCL_Common->eLastZpsError;
}

/****************************************************************************
 **
 ** NAME:       bZCL_GetTimeHasBeenSynchronised
 **
 ** DESCRIPTION:
 ** Returns TRUE only if the ZCL UTC time has been synchronised with a
 ** call to vZCL_SetUTCTime()
 **
 ** PARAMETERS:            Name                           Usage
 **
 ** RETURN:
 ** bool_t True if time has been synchronised
 **
 ****************************************************************************/

PUBLIC bool_t bZCL_GetTimeHasBeenSynchronised(void)
{
    return psZCL_Common->bTimeHasBeenSynchronised;
}


/****************************************************************************
 **
 ** NAME:       vZCL_ClearTimeHasBeenSynchronised
 **
 ** DESCRIPTION:
 ** Clears the time has been synchronised flag.
 ** PARAMETERS:            Name                           Usage
 **
 ** RETURN:
 ** void
 **
 ****************************************************************************/

PUBLIC void vZCL_ClearTimeHasBeenSynchronised(void)
{
    psZCL_Common->bTimeHasBeenSynchronised = FALSE;
}


/****************************************************************************
 **
 ** NAME:       eZCL_SetSupportedSecurity
 **
 ** DESCRIPTION:
 ** Sets the security level that is supported in the stack.
 **
 ** The default is E_ZCL_SECURITY_APPLINK.  Can be overridden by app to
 ** E_ZCL_SECURITY_NETWORK, which means that all clusters are sent with only NWK security.
 ** This is useful for testing with a stack with no APS security or for easier sniffing of SE
 ** clusters.  It is not possible to send completely unsecure frames in a ZB Pro compliant stack
 ** so E_ZCL_SECURITY_NONE has been removed.
 **
 ** PARAMETERS:            Name                           Usage
 ** teZCL_ZCLSendSecurity  eSecuritySupported             The security level to set.
 **
 ** RETURN:
 ** E_ZCL_SUCCESS, E_ZCL_ERR_PARAMETER_RANGE
 **
 ****************************************************************************/
PUBLIC teZCL_Status eZCL_SetSupportedSecurity(teZCL_ZCLSendSecurity  eSecuritySupported)
{
    if (eSecuritySupported >= E_ZCL_SECURITY_ENUM_END)
        return (E_ZCL_ERR_PARAMETER_RANGE);
    psZCL_Common->eSecuritySupported = eSecuritySupported;
    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       u8ZCL_GetApsSequenceNumberOfLastTransmit
 **
 ** DESCRIPTION:
 ** Get the APS sequence number that the last transmit was sent with.
 **
 ** PARAMETERS:      Name                           Usage
 **
 ** RETURN:
 ** uint8  last sequence number
 **
 ****************************************************************************/
PUBLIC uint8 u8ZCL_GetApsSequenceNumberOfLastTransmit(void)
{
    return psZCL_Common->u8ApsSequenceNumberOfLastTransmit;
}

/****************************************************************************
 **
 ** NAME:       eZCL_ReportAllAttributes
 **
 ** DESCRIPTION:
 ** Sends the list of all enabled attributes with in the cluster
 **
 ** PARAMETERS:                 Name                           Usage
 ** tsZCL_Address          psDestinationAddress    Destination address
 ** uint16                 u6ClusterID             Cluster Id, whose attribute
 **                                                need to be reported
 ** uint8                  u8SrcEndPoint           Source End Point
 ** uint8                  u8DestEndPoint          Destination End Point
 ** PDUM_thAPduInstance    hAPduInst               Application PDU Instance
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eZCL_ReportAllAttributes(
                    tsZCL_Address              *psDestinationAddress,
                    uint16                      u16ClusterID,
                    uint8                       u8SrcEndPoint,
                    uint8                       u8DestEndPoint,
                    PDUM_thAPduInstance         hAPduInst)
{
    tsZCL_ClusterInstance *psClusterInst;
    teZCL_Status eStatus;
    uint16 u16LoopCntr, u16Index = 0, u16AttributeID;
    volatile uint16 u16Offset = 0;
    eFrameTypeSubfield eFrameType = eFRAME_TYPE_COMMAND_ACTS_ACCROSS_ENTIRE_PROFILE;

    uint8 u8Seq = u8GetTransactionSequenceNumber();

    //Point to cluster instance
    if((eStatus = eZCL_SearchForClusterEntry(
              u8SrcEndPoint,
              u16ClusterID,
              TRUE,
              &psClusterInst)) != E_ZCL_SUCCESS)
    {
        return eStatus;
    }

    if (hAPduInst == PDUM_INVALID_HANDLE)
    {
      eStatus = E_ZCL_ERR_ZBUFFER_FAIL;
    }
    else
    {
        // Write command header
        u16Offset = u16ZCL_WriteCommandHeader(
              hAPduInst, eFrameType,
              FALSE, 0,   //Not mfr.specific
              TRUE,             //From server to client
              TRUE,             //Disable default response
              u8Seq, E_ZCL_REPORT_ATTRIBUTES);

        u16AttributeID = psClusterInst->psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeEnum;
        //Form payload
        for(u16LoopCntr = 0; u16LoopCntr < psClusterInst->psClusterDefinition->u16NumberOfAttributes; u16LoopCntr++)
        {

            if(ZCL_IS_BIT_CLEAR(uint8,psClusterInst->pu8AttributeControlBits[u16Index],E_ZCL_ACF_RP) ||
               ZCL_IS_BIT_SET(uint8,psClusterInst->psClusterDefinition->psAttributeDefinition[u16Index].u8AttributeFlags,E_ZCL_AF_MS))
            {
                u16Index++;
                u16AttributeID = psClusterInst->psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeEnum;
                continue;
            }

        
              //Write attribute ID
              u16Offset += PDUM_u16APduInstanceWriteNBO(hAPduInst,
                          u16Offset, "h",
                          u16AttributeID);

              //Write attribute data type
              u16Offset += PDUM_u16APduInstanceWriteNBO(hAPduInst,
                          u16Offset, "b",
                          psClusterInst->psClusterDefinition->psAttributeDefinition[u16Index].eAttributeDataType);

              //Write attribute value
              u16Offset += u16ZCL_WriteAttributeValueIntoBuffer(u8SrcEndPoint,
                          u16AttributeID,
                          psClusterInst,
                          &psClusterInst->psClusterDefinition->psAttributeDefinition[u16Index],
                          u16Offset,
                          hAPduInst);

              if((psClusterInst->psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeArrayLength != 0)&&
                ((u16AttributeID - psClusterInst->psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeEnum) < psClusterInst->psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeArrayLength))
              {
                  u16AttributeID++;
              }
              else
              {
                  u16Index++;
                  u16AttributeID = psClusterInst->psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeEnum;
              }
        }

        //Send command
        eStatus = eZCL_TransmitDataRequest(
                    hAPduInst,
                    u16Offset,
                    u8SrcEndPoint,
                    u8DestEndPoint,
                    u16ClusterID,
                    psDestinationAddress);
    }

    return eStatus;
}
/****************************************************************************
 **
 ** NAME:       eZCL_OverrideClusterControlFlags
 **
 ** DESCRIPTION:
 ** Overrides cluster control flags
 **
 ** PARAMETERS:                   Name                           Usage
 ** uint8                       u8SrcEndpoint                source EP
 ** uint16                      u16ClusterId                 Cluster Id
 ** bool_t                        bIsServerClusterInstance     Is server
 ** uint8                       u8ClusterControlFlags        control flags
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eZCL_OverrideClusterControlFlags(
                    uint8                       u8SrcEndpoint,
                    uint16                      u16ClusterId,
					bool_t                        bIsServerClusterInstance,
                    uint8                       u8ClusterControlFlags)
{
    teZCL_Status eZCL_Status;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    void *psCustomDataStructure;
    /* Find cluster Entry */
    eZCL_Status = eZCL_FindCluster(u16ClusterId,
                                u8SrcEndpoint,
                                bIsServerClusterInstance,
                                &psEndPointDefinition,
                                &psClusterInstance,
                                &psCustomDataStructure);

    /* check cluster find status */
    if(eZCL_Status != E_ZCL_SUCCESS)
    {
        return eZCL_Status;
    }

    psClusterInstance->psClusterDefinition->u8ClusterControlFlags = u8ClusterControlFlags;
    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       eZCL_OverrideClusterControlFlags
 **
 ** DESCRIPTION:
 ** Get cluster control flags and returns FF is there is an error
 **
 ** PARAMETERS:                   Name                           Usage
 ** uint8                       u8SrcEndpoint                source EP
 ** uint16                      u16ClusterId                 Cluster Id
 ** bool_t                        bIsServerClusterInstance     Is server
 **
 ** RETURN:
 ** uint8
 **
 ****************************************************************************/
PUBLIC  uint8 u8ZCL_GetClusterControlFlags(
                    uint8                       u8SrcEndpoint,
                    uint16                      u16ClusterId,
					bool_t                        bIsServerClusterInstance)
{
    teZCL_Status eZCL_Status;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    void *psCustomDataStructure;
    /* Find cluster Entry */
    eZCL_Status = eZCL_FindCluster(u16ClusterId,
                                u8SrcEndpoint,
                                bIsServerClusterInstance,
                                &psEndPointDefinition,
                                &psClusterInstance,
                                &psCustomDataStructure);

    /* check cluster find status */
    if(eZCL_Status != E_ZCL_SUCCESS)
    {
        return 0xFF;
    }

    return psClusterInstance->psClusterDefinition->u8ClusterControlFlags;
}

/****************************************************************************
 **
 ** NAME:       eZCL_SetAPSACKClusterControlFlagsBit
 **
 ** DESCRIPTION:
 ** Set or Clears the APS ACK cluster control flags bit
 **
 ** PARAMETERS:                   Name                           Usage
 ** uint8                       u8SrcEndpoint                source EP
 ** uint16                      u16ClusterId                 Cluster Id
 ** bool_t                        bIsServerClusterInstance     Is server
 ** bool_t                        bSet                         Set or Clears
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eZCL_SetAPSACKClusterControlFlagsBit(
                    uint8                       u8SrcEndpoint,
                    uint16                      u16ClusterId,
					bool_t                        bIsServerClusterInstance,
					bool_t                        bSet)
{
    teZCL_Status eZCL_Status;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    void *psCustomDataStructure;
    /* Find cluster Entry */
    eZCL_Status = eZCL_FindCluster(u16ClusterId,
                                u8SrcEndpoint,
                                bIsServerClusterInstance,
                                &psEndPointDefinition,
                                &psClusterInstance,
                                &psCustomDataStructure);

    /* check cluster find status */
    if(eZCL_Status != E_ZCL_SUCCESS)
    {
        return eZCL_Status;
    }

    if(bSet)
    {
         psClusterInstance->psClusterDefinition->u8ClusterControlFlags |= CLUSTER_APS_ACK_ENABLE_MASK;
    }
    else
    {
        psClusterInstance->psClusterDefinition->u8ClusterControlFlags  &= ~(CLUSTER_APS_ACK_ENABLE_MASK);
    }
    
    return(E_ZCL_SUCCESS);
}
/****************************************************************************
 **
 ** NAME:       vReverseMemcpy
 **
 ** DESCRIPTION:
 ** Does reverse memcopy (Conversion from little endian to big endian)
 **
 ** PARAMETERS:                   Name                           Usage
 ** uint8*                       pvOutData                       Destination
 ** uint8*                      pu8InData                       InputData
 ** uint8                       u8NumOfBytes                    Number Of Bytes
 **
 ** RETURN:
 ** None
 **
 ****************************************************************************/
PUBLIC void vReverseMemcpy(uint8 *pu8OutData, uint8 *pu8InData, uint8 u8NumOfBytes)
{
    uint8 i;

    if(!pu8InData || !pu8OutData || u8NumOfBytes == 0)
    {
        return;
    }
    uint8 *pu8Temp = (uint8*)pu8InData;

    #ifndef LITTLE_ENDIAN_PROCESSOR
        pu8Temp += (u8ZCL_GetAttributeAllignToFourBytesBoundary(u8NumOfBytes) - 1);
    #endif

    for(i=0;i<u8NumOfBytes;i++)
    {
        #ifndef LITTLE_ENDIAN_PROCESSOR
            *pu8OutData++ = *pu8Temp--;
        #else
            *pu8OutData++ = *pu8Temp++;
        #endif
    }
  
    return;
}

PUBLIC void vZCL_InitializeClusterInstance(
                tsZCL_ClusterInstance                              *psClusterInstance,
                bool_t                                             bIsServer,
                tsZCL_ClusterDefinition                            *psClusterDefinition,
                void                                               *pvEndPointSharedStructPtr,
                uint8                                              *pu8AttributeControlBits,
                void                                               *psCustomDataStructure,
                tfpZCL_ZCLCustomcallCallBackFunction               pCustomcallCallBackFunction)
{
    psClusterInstance->bIsServer = bIsServer;
    psClusterInstance->psClusterDefinition = psClusterDefinition;
    psClusterInstance->pvEndPointSharedStructPtr = pvEndPointSharedStructPtr;
    psClusterInstance->pu8AttributeControlBits = pu8AttributeControlBits;
    psClusterInstance->pvEndPointCustomStructPtr = psCustomDataStructure;
    psClusterInstance->pCustomcallCallBackFunction = (tfpZCL_ZCLCustomcallCallBackFunction)pCustomcallCallBackFunction;
}

/****************************************************************************
 **
 ** NAME:       vZCL_DisableAPSACK
 **
 ** DESCRIPTION:
 ** Disables the APS ACK from ZCL transmits function or pass as requested
 **
 ** PARAMETERS:                 Name                          Usage
 ** bool_t                      bDisableAPSACK                Disables APS ACK  
 ** RETURN:
 ** None
 **
 ****************************************************************************/

PUBLIC void vZCL_DisableAPSACK(
                                bool_t       bDisableAPSACK)
{
    psZCL_Common->bDisableAPSACK = bDisableAPSACK;
}

/****************************************************************************
 **
 ** NAME:       eZCL_ReportAttribute
 **
 ** DESCRIPTION:
 ** Sends the report of attributes with in the cluster
 **
 ** PARAMETERS:            Name                    Usage
 ** tsZCL_Address          psDestinationAddress    Destination address
 ** uint16                 u6ClusterID             Cluster Id
 ** uint16                 u16AttributeID          Attribute which needs to be reported
 ** uint8                  u8SrcEndPoint           Source End Point
 ** uint8                  u8DestEndPoint          Destination End Point
 ** PDUM_thAPduInstance    hAPduInst               Application PDU Instance
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eZCL_ReportAttribute(
                    tsZCL_Address               *psDestinationAddress,
                    uint16                      u16ClusterID,
                    uint16                      u16AttributeID,
                    uint8                       u8SrcEndPoint,
                    uint8                       u8DestEndPoint,
                    PDUM_thAPduInstance         hAPduInst)
{
    tsZCL_ClusterInstance *psClusterInst;
    teZCL_Status eStatus;
    uint16 u16Index = 0;
    volatile uint16 u16Offset = 0;
    eFrameTypeSubfield eFrameType = eFRAME_TYPE_COMMAND_ACTS_ACCROSS_ENTIRE_PROFILE;

    uint8 u8Seq = u8GetTransactionSequenceNumber();

    //Point to cluster instance
    if((eStatus = eZCL_SearchForClusterEntry(
              u8SrcEndPoint,
              u16ClusterID,
              TRUE,
              &psClusterInst)) != E_ZCL_SUCCESS)
    {
        return eStatus;
    }

    if (hAPduInst == PDUM_INVALID_HANDLE)
    {
        eStatus = E_ZCL_ERR_ZBUFFER_FAIL;
    }
    else
    {
        // Write command header
        u16Offset = u16ZCL_WriteCommandHeader(
              hAPduInst, eFrameType,
              FALSE, 0, //Not mfr.specific
              TRUE, //From server to client
              FALSE, // TRUE //Disable default response
              u8Seq, E_ZCL_REPORT_ATTRIBUTES);


        for(u16Index = 0; u16Index < psClusterInst->psClusterDefinition->u16NumberOfAttributes; u16Index++)
        {
            if(u16AttributeID == psClusterInst->psClusterDefinition->psAttributeDefinition[u16Index].u16AttributeEnum)
            {
                //Form payload
                if(ZCL_IS_BIT_SET(uint8,psClusterInst->psClusterDefinition->psAttributeDefinition[u16Index].u8AttributeFlags,E_ZCL_AF_RP) &&
                    ZCL_IS_BIT_CLEAR(uint8,psClusterInst->psClusterDefinition->psAttributeDefinition[u16Index].u8AttributeFlags,E_ZCL_AF_MS))
                {
                    //Write attribute ID
                    u16Offset += PDUM_u16APduInstanceWriteNBO(hAPduInst,
                              u16Offset, "h",
                              u16AttributeID);

                    //Write attribute data type
                    u16Offset += PDUM_u16APduInstanceWriteNBO(hAPduInst,
                              u16Offset, "b",
                              psClusterInst->psClusterDefinition->psAttributeDefinition[u16Index].eAttributeDataType);

                    //Write attribute value
                    u16Offset += u16ZCL_WriteAttributeValueIntoBuffer(u8SrcEndPoint,
                              u16AttributeID,
                              psClusterInst,
                              &psClusterInst->psClusterDefinition->psAttributeDefinition[u16Index],
                              u16Offset,
                              hAPduInst);
                              
                    //Send command
                    eStatus = eZCL_TransmitDataRequest(
                                hAPduInst,
                                u16Offset,
                                u8SrcEndPoint,
                                u8DestEndPoint,
                                u16ClusterID,
                                psDestinationAddress);                                  

                }
                else
                {
                    return E_ZCL_ERR_ATTRIBUTE_NOT_REPORTABLE;
                }                           
                break;
            }else if(u16Index == (psClusterInst->psClusterDefinition->u16NumberOfAttributes - 1)) // Indicates that attribute is not present as list has exhausted
            {
                return E_ZCL_ERR_ATTRIBUTE_NOT_FOUND;
            }
        }

    }

    return eStatus;
}
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
