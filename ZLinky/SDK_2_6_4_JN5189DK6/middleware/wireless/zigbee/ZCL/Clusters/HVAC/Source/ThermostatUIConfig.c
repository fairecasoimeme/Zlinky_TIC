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
 * MODULE:             ThermostatUIConfig Cluster
 *
 * COMPONENT:          ThermostatUIConfig.c
 *
 * DESCRIPTION:        ThermostatUIConfig cluster definition
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
#include "ThermostatUIConfig.h"

#include "dbg.h"

#ifdef DEBUG_CLD_THERMOSTAT_UI_CONFIG
#define TRACE_THERMOSTAT_UI_CONFIG    TRUE
#else
#define TRACE_THERMOSTAT_UI_CONFIG    FALSE
#endif



/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#if defined(CLD_THERMOSTAT_UI_CONFIG) && !defined(THERMOSTAT_UI_CONFIG_SERVER) && !defined(THERMOSTAT_UI_CONFIG_CLIENT)
#error You Must Have either THERMOSTAT_UI_CONFIG_SERVER and/or THERMOSTAT_UI_CONFIG_CLIENT defined in zcl_options.h
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
#ifdef CLD_THERMOSTAT_UI_CONFIG
/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
const tsZCL_AttributeDefinition asCLD_ThermostatUIConfigClusterAttributeDefinitions[] = {
#ifdef THERMOSTAT_UI_CONFIG_SERVER
        {E_CLD_THERMOSTAT_UI_CONFIG_ATTR_ID_TEMPERATURE_DISPLAY_MODE,    (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_ENUM8,    (uint32)(&((tsCLD_ThermostatUIConfig*)(0))->eTemperatureDisplayMode),   0},   /* Mandatory */

        {E_CLD_THERMOSTAT_UI_CONFIG_ATTR_ID_KEYPAD_LOCKOUT,              (E_ZCL_AF_RD|E_ZCL_AF_WR),      E_ZCL_ENUM8,    (uint32)(&((tsCLD_ThermostatUIConfig*)(0))->eKeypadLockout),            0},   /* Mandatory */
#endif        
        {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,                               (E_ZCL_AF_RD|E_ZCL_AF_GA),     E_ZCL_BMAP32,   (uint32)(&((tsCLD_ThermostatUIConfig*)(0))->u32FeatureMap),0},   /* Mandatory  */ 
        
        {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,                          (E_ZCL_AF_RD|E_ZCL_AF_GA),      E_ZCL_UINT16,   (uint32)(&((tsCLD_ThermostatUIConfig*)(0))->u16ClusterRevision),        0},   /* Mandatory  */
};

tsZCL_ClusterDefinition sCLD_ThermostatUIConfig = {
        HVAC_CLUSTER_ID_THERMOSTAT_UI_CONFIG,
        FALSE,
        E_ZCL_SECURITY_NETWORK,
        (sizeof(asCLD_ThermostatUIConfigClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
        (tsZCL_AttributeDefinition*)asCLD_ThermostatUIConfigClusterAttributeDefinitions,
        NULL
};

uint8 au8ThermostatUIConfigAttributeControlBits[(sizeof(asCLD_ThermostatUIConfigClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 **
 ** NAME:       eCLD_ThermostatUIConfigCreateThermostatUIConfig
 **
 ** DESCRIPTION:
 ** Creates a thermostat cluster
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ThermostatUIConfigCreateThermostatUIConfig(
                tsZCL_ClusterInstance                   *psClusterInstance,
                bool_t                                  bIsServer,
                tsZCL_ClusterDefinition                 *psClusterDefinition,
                void                                    *pvEndPointSharedStructPtr,
                uint8                                   *pu8AttributeControlBits)
{


    #ifdef STRICT_PARAM_CHECK 
        /* Parameter check */
        if((psClusterInstance==NULL) ||
           (psClusterDefinition==NULL)  )
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif

    /* Create an instance of a temperature measurement cluster */
    vZCL_InitializeClusterInstance(
                                   psClusterInstance, 
                                   bIsServer,
                                   psClusterDefinition,
                                   pvEndPointSharedStructPtr,
                                   pu8AttributeControlBits,
                                   NULL,
                                   NULL);      
        if(pvEndPointSharedStructPtr != NULL)
        {
        #ifdef THERMOSTAT_UI_CONFIG_SERVER    
            /* Initialise attributes */
            ((tsCLD_ThermostatUIConfig*)pvEndPointSharedStructPtr)->eTemperatureDisplayMode = E_CLD_THERMOSTAT_UI_CONFIG_TEMPERATURE_DISPLAY_MODE_CELSIUS;
            ((tsCLD_ThermostatUIConfig*)pvEndPointSharedStructPtr)->eKeypadLockout = E_CLD_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_NO_LOCKOUT;
        #endif    
            ((tsCLD_ThermostatUIConfig*)pvEndPointSharedStructPtr)->u32FeatureMap = CLD_THERMOSTAT_UI_CONFIG_FEATURE_MAP;
            
            ((tsCLD_ThermostatUIConfig*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_THERMOSTAT_UI_CONFIG_CLUSTER_REVISION;
        }

    return E_ZCL_SUCCESS;

}
#ifdef THERMOSTAT_UI_CONFIG_SERVER

/****************************************************************************
 **
 ** NAME:       eCLD_ThermostatUIConfigConvertTemp
 **
 ** DESCRIPTION:
 ** Used by cluster to convert from celsius to fahrenheit and vice-versa
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          EndPoint Id
 ** bool_t                        bConvertFToC                Is conversion from C to F
 ** int16                       *pi16Temperature            Pointer to temperature Value
 **
 ** RETURN:
 ** teZCL_Status
 ** 
 ****************************************************************************/

PUBLIC  teZCL_Status eCLD_ThermostatUIConfigConvertTemp(
                                            uint8               u8SourceEndPointId,
											bool_t                bConvertCToF,
                                            int16               *pi16Temperature)  
{
    teZCL_Status eStatus;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    void *psCustomDataStructure;
    int32  Temp;

    eStatus = eZCL_FindCluster(HVAC_CLUSTER_ID_THERMOSTAT_UI_CONFIG, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&psCustomDataStructure);
    if(eStatus != E_ZCL_SUCCESS)
    {
        return eStatus;
    }
        
    if(pi16Temperature == NULL)
        return E_ZCL_ERR_PARAMETER_NULL;
    
    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif

    
    if(bConvertCToF)
    {
        Temp = (int16)*pi16Temperature;
        *pi16Temperature = (uint8) (((Temp*9*2 + 250)/ (5*100) + 64));
    }else{
        Temp = (uint8)*pi16Temperature;
        *pi16Temperature = (int16) (((Temp - 64)*5*1000 + 90) / (10*2*9));
    }
    
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    
    return eStatus;

}

#endif /*#define THERMOSTAT_UI_CONFIG_SERVER */
/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/


#endif /*#define  CLD_THERMOSTAT_UI_CONFIG */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

