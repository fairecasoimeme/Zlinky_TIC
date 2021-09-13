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
 * MODULE:             IAS Zone Cluster
 *
 * COMPONENT:          IASZone.c
 *
 * DESCRIPTION:        IAS ZOne cluster definition
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>

#include "zps_apl.h"
#include "zps_apl_aib.h"

#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_options.h"
#include "string.h"
#include "dbg.h"
#include "IASZone.h"
#include "IASZone_internal.h"

#ifdef DEBUG_CLD_IASZONE
#define TRACE_IASZONE    TRUE
#else
#define TRACE_IASZONE    FALSE
#endif

#ifdef CLD_IASZONE

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#if !defined(IASZONE_SERVER) && !defined(IASZONE_CLIENT)
#error You Must Have either IASZONE_SERVER and/or IASZONE_CLIENT defined in zcl_options.h
#endif

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


#ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED
const tsZCL_CommandDefinition asCLD_IASZoneClusterCommandDefinitions[] = {

    /* Server Receives and client sends */
    {E_CLD_IASZONE_CMD_ZONE_ENROLL_RESP,                   E_ZCL_CF_RX}, /* Mandatory */

    #ifdef CLD_IASZONE_CMD_INITIATE_NORMAL_OPERATION_MODE    
    {E_CLD_IASZONE_CMD_INITIATE_NORMAL_OPERATION_MODE,     E_ZCL_CF_RX}, /* Optional */
    #endif
    
    #ifdef CLD_IASZONE_CMD_INITIATE_TEST_MODE
    {E_CLD_IASZONE_CMD_INITIATE_TEST_MODE,                 E_ZCL_CF_RX}, /* Optional */
    #endif


    /* Server Sends and client receives */
    {E_CLD_IASZONE_CMD_ZONE_STATUS_CHANGE_NOTIFICATION,    E_ZCL_CF_TX}, /* Mandatory */
    {E_CLD_IASZONE_CMD_ZONE_ENROLL_REQUEST,                E_ZCL_CF_TX}, /* Mandatory */
  

};
#endif

const tsZCL_AttributeDefinition asCLD_IASZoneClusterAttributeDefinitions[] = {
#ifdef IASZONE_SERVER    
        { E_CLD_IASZONE_ATTR_ID_ZONE_STATE,                         E_ZCL_AF_RD,                E_ZCL_ENUM8,        (uint32)(&((tsCLD_IASZone*)(0))->e8ZoneState),0},       /* Mandatory */
        { E_CLD_IASZONE_ATTR_ID_ZONE_TYPE,                          E_ZCL_AF_RD,                E_ZCL_ENUM16,       (uint32)(&((tsCLD_IASZone*)(0))->e16ZoneType),0},       /* Mandatory */
        { E_CLD_IASZONE_ATTR_ID_ZONE_STATUS,                        E_ZCL_AF_RD,                E_ZCL_BMAP16,       (uint32)(&((tsCLD_IASZone*)(0))->b16ZoneStatus),0},     /* Mandatory */
        { E_CLD_IASZONE_ATTR_ID_IAS_CIE_ADDRESS,                    (E_ZCL_AF_RD|E_ZCL_AF_WR),  E_ZCL_IEEE_ADDR,    (uint32)(&((tsCLD_IASZone*)(0))->u64IASCIEAddress), 0}, /* Mandatory */
        { E_CLD_IASZONE_ATTR_ID_ZONE_ID,                            E_ZCL_AF_RD,                E_ZCL_UINT8,        (uint32)(&((tsCLD_IASZone*)(0))->u8ZoneId),0},          /* Mandatory */

	#ifdef CLD_IASZONE_ATTR_NUMBER_OF_ZONE_SENSITIVITY_LEVELS
        {E_CLD_IASZONE_ATTR_ID_NUMBER_OF_ZONE_SENSITIVITY_LEVELS,    E_ZCL_AF_RD,                E_ZCL_UINT8,    (uint32)(&((tsCLD_IASZone*)(0))->u8NumberOfZoneSensitivityLevels),0}, /* Optional */
    #endif

    #ifdef CLD_IASZONE_ATTR_CURRENT_ZONE_SENSITIVITY_LEVEL
        {E_CLD_IASZONE_ATTR_ID_CURRENT_ZONE_SENSITIVITY_LEVEL,       (E_ZCL_AF_RD|E_ZCL_AF_WR),   E_ZCL_UINT8,    (uint32)(&((tsCLD_IASZone*)(0))->u8CurrentZoneSensitivityLevel),0},   /* Optional */
    #endif
#endif    
        {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,                          (E_ZCL_AF_RD|E_ZCL_AF_GA),    E_ZCL_BMAP32,       (uint32)(&((tsCLD_IASZone*)(0))->u32FeatureMap),0},   /* Mandatory  */ 

        {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,                     (E_ZCL_AF_RD|E_ZCL_AF_GA),    E_ZCL_UINT16,       (uint32)(&((tsCLD_IASZone*)(0))->u16ClusterRevision),0},   /* Mandatory  */
};


tsZCL_ClusterDefinition sCLD_IASZone = {
        SECURITY_AND_SAFETY_CLUSTER_ID_IASZONE,
        FALSE,
        E_ZCL_SECURITY_NETWORK,
        (sizeof(asCLD_IASZoneClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
        (tsZCL_AttributeDefinition*)asCLD_IASZoneClusterAttributeDefinitions,
        NULL
        #ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED        
            ,
            (sizeof(asCLD_IASZoneClusterCommandDefinitions) / sizeof(tsZCL_CommandDefinition)),
            (tsZCL_CommandDefinition*)asCLD_IASZoneClusterCommandDefinitions 
        #endif         
};

uint8 au8IASZoneAttributeControlBits[(sizeof(asCLD_IASZoneClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eCLD_IASZoneCreateIASZone
 **
 ** DESCRIPTION:
 ** Creates IAS Zone cluster
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASZoneCreateIASZone(
        tsZCL_ClusterInstance                   *psClusterInstance,
        bool_t                                  bIsServer,
        tsZCL_ClusterDefinition                 *psClusterDefinition,
        void                                    *pvEndPointSharedStructPtr,
        uint8                                   *pu8AttributeControlBits,
        tsCLD_IASZone_CustomDataStructure       *psCustomDataStructure)
{

    #ifdef STRICT_PARAM_CHECK 
        /* Parameter check */
        if((psClusterInstance==NULL) || (psCustomDataStructure==NULL))
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif

    /* Zero everything */
    memset(pvEndPointSharedStructPtr, 0, sizeof(tsCLD_IASZone));
    memset(psCustomDataStructure, 0, sizeof(tsCLD_IASZone_CustomDataStructure));

    // cluster data
    vZCL_InitializeClusterInstance(
                                   psClusterInstance, 
                                   bIsServer,
                                   psClusterDefinition,
                                   pvEndPointSharedStructPtr,
                                   pu8AttributeControlBits,
                                   psCustomDataStructure,
                                   eCLD_IASZoneCommandHandler);     
    
    psCustomDataStructure->sCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = psClusterDefinition->u16ClusterEnum;
    psCustomDataStructure->sCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void *)&psCustomDataStructure->sCallBackMessage;
    psCustomDataStructure->sCustomCallBackEvent.psClusterInstance = psClusterInstance; 

        /*Load default values to the attributes*/
        if(pvEndPointSharedStructPtr != NULL)
        {
        #ifdef IASZONE_SERVER    
            ((tsCLD_IASZone*)pvEndPointSharedStructPtr)->u8ZoneId = 0xFF;

            #ifdef CLD_IASZONE_ATTR_NUMBER_OF_ZONE_SENSITIVITY_LEVELS
                ((tsCLD_IASZone*)pvEndPointSharedStructPtr)->u8NumberOfZoneSensitivityLevels = CLD_IASZONE_NUMBER_OF_ZONE_SENSITIVITY_LEVELS;
            #endif

            #ifdef CLD_IASZONE_ATTR_CURRENT_ZONE_SENSITIVITY_LEVEL
                ((tsCLD_IASZone*)pvEndPointSharedStructPtr)->u8CurrentZoneSensitivityLevel = CLD_IASZONE_CURRENT_ZONE_SENSITIVITY_LEVEL;
            #endif
        #endif    
            ((tsCLD_IASZone*)pvEndPointSharedStructPtr)->u32FeatureMap = CLD_IASZONE_FEATURE_MAP;
                
            ((tsCLD_IASZone*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_IASZONE_CLUSTER_REVISION;
        }
    /* As this cluster has reportable attributes enable default reporting */
    vZCL_SetDefaultReporting(psClusterInstance);   
	
    return E_ZCL_SUCCESS;
}

#ifdef IASZONE_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_IASZoneUpdate
 **
 ** DESCRIPTION:
 ** Updates the cluster every second.
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                   u8SourceEndPoint             End Point
 ** uint16                  u16StatusBitMask             Mask Bit
 ** bool_t                  bStatusState                 Set or Reset
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASZoneUpdate (uint8   u8SourceEndPoint)
{
    teZCL_Status eStatus = E_ZCL_SUCCESS;

    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_IASZone_CustomDataStructure *pCustomDataStructure;
    tsZCL_CallBackEvent sZCL_CallBackEvent;
    
    zbmap16 b16ZoneStatus;

    /* Find pointers to cluster */
    eStatus = eZCL_FindCluster(SECURITY_AND_SAFETY_CLUSTER_ID_IASZONE, u8SourceEndPoint, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&pCustomDataStructure);
    if(eStatus != E_ZCL_SUCCESS)
    {
        return eStatus;
    }
    
    if( NULL == pCustomDataStructure)
    {
        return E_ZCL_ERR_CUSTOM_DATA_NULL;
    }
    
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    
    /*Subtract the time Loaded for Warning or Stobe at 50msec */
    eStatus = eZCL_ReadLocalAttributeValue(
                     u8SourceEndPoint,                      //uint8                      u8SrcEndpoint,
                     SECURITY_AND_SAFETY_CLUSTER_ID_IASZONE, //uint16                     u16ClusterId,
                     TRUE,                                  //bool_t                       bIsServerClusterInstance,
                     FALSE,                                 //bool_t                       bManufacturerSpecific,
                     FALSE,                                 //bool_t                     bIsClientAttribute,
                     E_CLD_IASZONE_ATTR_ID_ZONE_STATUS,     //uint16                     u16AttributeId,
                     &b16ZoneStatus
                    );                     
    /* Warning Mode is Set*/
    if( ZCL_IS_BIT_SET(zbmap16,b16ZoneStatus,CLD_IASZONE_STATUS_MASK_TEST)  )
    {
        /*Test Mode is still on */
        if(pCustomDataStructure->sTestMode.u8TestModeDuration > 0)
        {
            pCustomDataStructure->sTestMode.u8TestModeDuration--;

        }
        else
        {
            tsCLD_IASZone_TestModeUpdate sTestModeUpdate;
            /*Update the status */
            eCLD_IASZoneUpdateZoneStatus(psEndPointDefinition->u8EndPointNumber,CLD_IASZONE_STATUS_MASK_TEST,CLD_IASZONE_STATUS_MASK_RESET);
            

            sTestModeUpdate.e8TestMode= E_CLD_IASZONE_TEST_MODE_ENDED;
            
            pCustomDataStructure->sCallBackMessage.uMessage.psTestModeUpdate=&sTestModeUpdate; 

            /* Generate a callback to let the user know that an update event occurred */        
            pCustomDataStructure->sCallBackMessage.u8CommandId=E_CLD_IASZONE_TEST_MODE_ENDED;
            
            sZCL_CallBackEvent.u8EndPoint           = psEndPointDefinition->u8EndPointNumber;
            sZCL_CallBackEvent.psClusterInstance    = psClusterInstance;
            sZCL_CallBackEvent.pZPSevent            = NULL;
            sZCL_CallBackEvent.eEventType           = E_ZCL_CBET_CLUSTER_UPDATE;
            psEndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);
 
        }
    }

    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif
 
    return eStatus;

}

/****************************************************************************
 **
 ** NAME:       eCLD_IASZoneUpdateZoneStatusAttribute
 **
 ** DESCRIPTION:
 ** Writes Status bit.
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                   u8SourceEndPoint             End Point
 ** uint16                  u16StatusBitMask             Mask Bit
 ** bool_t                  bStatusState                 Set or Reset
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASZoneUpdateZoneStatusAttribute (
                                uint8                             u8SourceEndPoint, 
                                zbmap16                           b16ZoneStatus)
{

    teZCL_Status eStatus;

    eStatus = eZCL_WriteLocalAttributeValue(
                 u8SourceEndPoint,                      //uint8                      u8SrcEndpoint,
                 SECURITY_AND_SAFETY_CLUSTER_ID_IASZONE, //uint16                     u16ClusterId,
                 TRUE,                                  //bool_t                       bIsServerClusterInstance,
                 FALSE,                                 //bool_t                       bManufacturerSpecific,
                 FALSE,                                 //bool_t                     bIsClientAttribute,
                 E_CLD_IASZONE_ATTR_ID_ZONE_STATUS,     //uint16                     u16AttributeId,
                 &b16ZoneStatus                         //void                      *pvAttributeValue
                 );

    return eStatus;
}
/****************************************************************************
 **
 ** NAME:       eCLD_IASZoneUpdateZoneStatus
 **
 ** DESCRIPTION:
 ** Writes Status bit and sens out notification to tthe bound devices.
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                   u8SourceEndPoint             End Point
 ** uint16                  u16StatusBitMask             Mask Bit
 ** bool_t                  bStatusState                 Set or Reset
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASZoneUpdateZoneStatus (
                                uint8                             u8SourceEndPoint, 
                                uint16                            u16StatusBitMask, 
                                bool_t                            bStatusState)
{

    uint8 u8TransactionSequenceNumber;
    teZCL_Status eStatus;
    zbmap16   b16ZoneStatus;
    tsZCL_Address sDestinationAddress;
    tsCLD_IASZone_StatusChangeNotificationPayload sPayload;
    
    eStatus = eZCL_ReadLocalAttributeValue(
                     u8SourceEndPoint,                      //uint8                      u8SrcEndpoint,
                     SECURITY_AND_SAFETY_CLUSTER_ID_IASZONE, //uint16                     u16ClusterId,
                     TRUE,                                  //bool_t                       bIsServerClusterInstance,
                     FALSE,                                 //bool_t                       bManufacturerSpecific,
                     FALSE,                                 //bool_t                     bIsClientAttribute,
                     E_CLD_IASZONE_ATTR_ID_ZONE_STATUS,     //uint16                     u16AttributeId,
                     &b16ZoneStatus                         //void                      *pvAttributeValue
                     );
    if(E_ZCL_SUCCESS == eStatus)
    {    
        bStatusState ? ZCL_BIT_SET(zbmap16,b16ZoneStatus,u16StatusBitMask): ZCL_BIT_CLEAR(zbmap16,b16ZoneStatus,u16StatusBitMask);                
        eStatus = eZCL_WriteLocalAttributeValue(
                     u8SourceEndPoint,                      //uint8                      u8SrcEndpoint,
                     SECURITY_AND_SAFETY_CLUSTER_ID_IASZONE, //uint16                     u16ClusterId,
                     TRUE,                                  //bool_t                       bIsServerClusterInstance,
                     FALSE,                                 //bool_t                       bManufacturerSpecific,
                     FALSE,                                 //bool_t                     bIsClientAttribute,
                     E_CLD_IASZONE_ATTR_ID_ZONE_STATUS,     //uint16                     u16AttributeId,
                     &b16ZoneStatus                         //void                      *pvAttributeValue
                     );
        if(E_ZCL_SUCCESS == eStatus)
        {
           
            sPayload.b16ZoneStatus = b16ZoneStatus;
            sPayload.b8ExtendedStatus = 0;
            eStatus = eZCL_ReadLocalAttributeValue(
                     u8SourceEndPoint,                      //uint8                      u8SrcEndpoint,
                     SECURITY_AND_SAFETY_CLUSTER_ID_IASZONE, //uint16                     u16ClusterId,
                     TRUE,                                  //bool_t                       bIsServerClusterInstance,
                     FALSE,                                 //bool_t                       bManufacturerSpecific,
                     FALSE,                                 //bool_t                     bIsClientAttribute,
                     E_CLD_IASZONE_ATTR_ID_ZONE_ID,         //uint16                     u16AttributeId,
                     &sPayload.u8ZoneId                     //void                      *pvAttributeValue
                     );
                     
            sPayload.u16Delay = 0;
            #ifdef CLD_IASZONE_BOUND_TX_WITH_APS_ACK_DISABLED
                sDestinationAddress.eAddressMode = E_ZCL_AM_BOUND_NO_ACK;
            #else
                sDestinationAddress.eAddressMode = E_ZCL_AM_BOUND;
            #endif  
            /*Send Out a status Chnage Notification */
            eStatus = eCLD_IASZoneStatusChangeNotificationSend (
                                u8SourceEndPoint,            //uint8                             u8SourceEndPointId,
                                0xff,                        //uint8                             u8DestinationEndPointId, //Doesn't mattter
                                &sDestinationAddress,        //tsZCL_Address                     *psDestinationAddress,
                                &u8TransactionSequenceNumber,//uint8                             *pu8TransactionSequenceNumber,
                                &sPayload                    
                                );
        }
    }

    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_IASZoneUpdateZoneState
 **
 ** DESCRIPTION:
 ** Sets the Zone State
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                   u8SourceEndPointId           End Point
 ** teCLD_IASZone_State     eZoneState                   ZoneState
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASZoneUpdateZoneState (
                                uint8                             u8SourceEndPointId,
                                teCLD_IASZone_State               eZoneState)
{

    teZCL_Status eStatus;
    
               
    eStatus = eZCL_WriteLocalAttributeValue(
                     u8SourceEndPointId,                    //uint8                      u8SrcEndpoint,
                     SECURITY_AND_SAFETY_CLUSTER_ID_IASZONE, //uint16                     u16ClusterId,
                     TRUE,                                  //bool_t                       bIsServerClusterInstance,
                     FALSE,                                 //bool_t                       bManufacturerSpecific,
                     FALSE,                                 //bool_t                     bIsClientAttribute,
                     E_CLD_IASZONE_ATTR_ID_ZONE_STATE,      //uint16                     u16AttributeId,
                     &eZoneState                            //void                      *pvAttributeValue
                     );

    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eCLD_IASZoneUpdateZoneType
 **
 ** DESCRIPTION:
 ** Sets Zone Type
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                   u8SourceEndPointId           End Point
 ** teCLD_IASZone_Type      eIASZoneType                 Zone Type
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASZoneUpdateZoneType (
                                uint8                             u8SourceEndPointId,
                                teCLD_IASZone_Type                eIASZoneType)
{

    teZCL_Status eStatus;
    
               
    eStatus = eZCL_WriteLocalAttributeValue(
                     u8SourceEndPointId,                    //uint8                      u8SrcEndpoint,
                     SECURITY_AND_SAFETY_CLUSTER_ID_IASZONE, //uint16                     u16ClusterId,
                     TRUE,                                  //bool_t                       bIsServerClusterInstance,
                     FALSE,                                 //bool_t                       bManufacturerSpecific,
                     FALSE,                                 //bool_t                     bIsClientAttribute,
                     E_CLD_IASZONE_ATTR_ID_ZONE_TYPE,       //uint16                     u16AttributeId,
                     &eIASZoneType                          //void                      *pvAttributeValue
                     );

    return eStatus;
}
/****************************************************************************
 **
 ** NAME:       eCLD_IASZoneUpdateZoneID
 **
 ** DESCRIPTION:
 ** Sets Zone ID
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                   u8SourceEndPointId           End Point
 ** uint8                   u8IASZoneId                  Zone ID
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASZoneUpdateZoneID (
                                uint8                             u8SourceEndPointId,
                                uint8                             u8IASZoneId)
{

    teZCL_Status eStatus;
    
    eStatus = eZCL_WriteLocalAttributeValue(
                     u8SourceEndPointId,                    //uint8                      u8SrcEndpoint,
                     SECURITY_AND_SAFETY_CLUSTER_ID_IASZONE, //uint16                     u16ClusterId,
                     TRUE,                                  //bool_t                       bIsServerClusterInstance,
                     FALSE,                                 //bool_t                       bManufacturerSpecific,
                     FALSE,                                 //bool_t                     bIsClientAttribute,
                     E_CLD_IASZONE_ATTR_ID_ZONE_ID,         //uint16                     u16AttributeId,
                     &u8IASZoneId                           //void                      *pvAttributeValue
                     );

    return eStatus;
}   
      
/****************************************************************************
 **
 ** NAME:       eCLD_IASZoneUpdateCIEAddress
 **
 ** DESCRIPTION:
 ** Sets CIE IEEE Address
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                   u8SourceEndPointId             End Point
 ** uint64                  u64CIEAddress                 Zone Type
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_IASZoneUpdateCIEAddress (
                                uint8                             u8SourceEndPointId,
                                uint64                            u64CIEAddress )
{

    teZCL_Status eStatus;
    
    eStatus = eZCL_WriteLocalAttributeValue(
                     u8SourceEndPointId,                    //uint8                      u8SrcEndpoint,
                     SECURITY_AND_SAFETY_CLUSTER_ID_IASZONE, //uint16                     u16ClusterId,
                     TRUE,                                  //bool_t                       bIsServerClusterInstance,
                     FALSE,                                 //bool_t                       bManufacturerSpecific,
                     FALSE,                                 //bool_t                     bIsClientAttribute,
                     E_CLD_IASZONE_ATTR_ID_IAS_CIE_ADDRESS, //uint16                     u16AttributeId,
                     &u64CIEAddress                         //void                      *pvAttributeValue
                     );

    return eStatus;
}         
#endif

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

#endif          // ifdef CLD_IASZONE

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

