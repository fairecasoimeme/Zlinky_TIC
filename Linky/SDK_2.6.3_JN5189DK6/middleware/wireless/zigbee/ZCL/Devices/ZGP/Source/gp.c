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
 * MODULE:             Green Power Profile
 *
 * COMPONENT:          gp.c
 *
 * DESCRIPTION:        The API for the Green Power End Point Registration
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>
#include "zps_apl.h"
#include "zcl_heap.h"
#include "zcl.h"
#include "zcl_options.h"
#include "zcl_common.h"
#include "GreenPower.h"
#include "gp.h"

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

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
#ifdef GP_COMBO_BASIC_DEVICE
/****************************************************************************
 *
 * NAME: eGP_RegisterComboBasicEndPoint
 *
 * DESCRIPTION:
 * Registers a Green Power Cluster with the ZCL layer
 *
 * PARAMETERS:                    Name                            Usage
 * uint8                      u8EndPointIdentifier            Endpoint id
 * tfpZCL_ZCLCallBackFunction cbCallBack                      Pointer to endpoint callback
 * tsGP_GreenPowerDevice      psDeviceInfo                    Pointer to struct containing device info
 * uint16                     u16ProfileId                    Profile id of application
 * tsGP_TranslationTableEntry   psTranslationTableEntry              pointer of translation table
 *
 * RETURNS:
 * teZCL_Status
 *
 ****************************************************************************/
PUBLIC teZCL_Status eGP_RegisterComboBasicEndPoint(
                        uint8                               u8EndPointIdentifier,
                        tfpZCL_ZCLCallBackFunction          cbCallBack,
                        tsGP_GreenPowerDevice               *psDeviceInfo,
                        uint16                              u16ProfileId,
                        tsGP_TranslationTableEntry          *psTranslationTableEntry)
{

    /* Fill in end point details */
    psDeviceInfo->sEndPoint.u8EndPointNumber =                  u8EndPointIdentifier; //local proxy Endpoint
    psDeviceInfo->sEndPoint.u16ManufacturerCode =               GP_MANUFACTURER_CODE;
    psDeviceInfo->sEndPoint.u16ProfileEnum =                    GREENPOWER_PROFILE_ID; // Profile ID for Green Power Cluster
    psDeviceInfo->sEndPoint.bIsManufacturerSpecificProfile =    FALSE;
    psDeviceInfo->sEndPoint.u16NumberOfClusters =               sizeof(tsGP_GreenPowerClusterInstances) / sizeof(tsZCL_ClusterInstance);
    psDeviceInfo->sEndPoint.psClusterInstance =                 (tsZCL_ClusterInstance*)&psDeviceInfo->sClusterInstance;
    psDeviceInfo->sEndPoint.bDisableDefaultResponse =           ZCL_DISABLE_DEFAULT_RESPONSES;
    psDeviceInfo->sEndPoint.pCallBackFunctions =                cbCallBack;

#ifdef CLD_GREENPOWER
    /* Copy Green Power Mapped End-point in zcl_common structure */
    psZCL_Common->u8GreenPowerMappedEpId = u8EndPointIdentifier;
#endif

    /* Create Server Green Power Cluster Instance */
    if(eGP_CreateGreenPower(
            &psDeviceInfo->sClusterInstance.sGreenPowerServer,
            TRUE,
            &sCLD_GreenPowerServer,
            &psDeviceInfo->sServerGreenPowerCluster,
            &au8ServerGreenPowerClusterAttributeControlBits[0],
            &psDeviceInfo->sGreenPowerCustomDataStruct)
             != E_ZCL_SUCCESS)
    {
        return(E_ZCL_FAIL);
    }

    /* Create Client Green Power Cluster Instance */
    if(eGP_CreateGreenPower(
            &psDeviceInfo->sClusterInstance.sGreenPowerClient,
            FALSE,
            &sCLD_GreenPowerClient,
            &psDeviceInfo->sClientGreenPowerCluster,
            &au8ClientGreenPowerClusterAttributeControlBits[0],
            &psDeviceInfo->sGreenPowerCustomDataStruct)
             != E_ZCL_SUCCESS)
    {
        return(E_ZCL_FAIL);
    }

    /* Save Translation table address and No. Of Entries */
    psDeviceInfo->sGreenPowerCustomDataStruct.psZgpsTranslationTableEntry = psTranslationTableEntry;

    /* Set Default device operating mode */
    psDeviceInfo->sGreenPowerCustomDataStruct.eGreenPowerDeviceMode = E_GP_OPERATING_MODE;

    /* Set GP device type */
    psDeviceInfo->sGreenPowerCustomDataStruct.eGreenPowerDeviceType = E_GP_ZGP_COMBO_BASIC_DEVICE;

    /* Save Application Profile Id in GP custom data structure */
    psDeviceInfo->sGreenPowerCustomDataStruct.u16ProfileId = u16ProfileId;

    /* Set u16CommissionSetAddress */
    psDeviceInfo->sGreenPowerCustomDataStruct.u64CommissionSetAddress = 0x00;
    psDeviceInfo->sGreenPowerCustomDataStruct.bCommissionUnicast = 0;

    /* Register bGP_AliasPresent() with ZPS for alias address conflict */
    ZPS_vAfRegisterAliasPresentCallback(&bGP_AliasPresent);

    /* Ignore profile check for loopback */
    zps_vSetIgnoreProfileCheck();

    /* Register u8GP_SecurityProcessCallback() with ZPS for Process security of received GPDF when a callback is received from stack */
    ZPS_vRegisterSecRequestResponseCallback(&u8GP_SecurityProcessCallback);

    /* Set bitmap for Mandatory features: This will be updated in future releases */

    return eZCL_Register(&psDeviceInfo->sEndPoint);
}
#endif
#ifdef GP_PROXY_BASIC_DEVICE
/****************************************************************************
 *
 * NAME: eGP_RegisterProxyBasicEndPoint
 *
 * DESCRIPTION:
 * Registers a Green Power Cluster with the ZCL layer
 *
 * PARAMETERS:                    Name                            Usage
 * uint8                      u8EndPointIdentifier            Endpoint id
 * tfpZCL_ZCLCallBackFunction cbCallBack                      Pointer to endpoint callback
 * tsGP_GreenPowerDevice      psDeviceInfo                    Pointer to struct containing device info
 *
 * RETURNS:
 * teZCL_Status
 *
 ****************************************************************************/
PUBLIC teZCL_Status eGP_RegisterProxyBasicEndPoint(
                        uint8                               u8EndPointIdentifier,
                        tfpZCL_ZCLCallBackFunction          cbCallBack,
                        tsGP_GreenPowerDevice               *psDeviceInfo)
{

    /* Fill in end point details */
    psDeviceInfo->sEndPoint.u8EndPointNumber =                  u8EndPointIdentifier; //local proxy Endpoint
    psDeviceInfo->sEndPoint.u16ManufacturerCode =               GP_MANUFACTURER_CODE;
    psDeviceInfo->sEndPoint.u16ProfileEnum =                    GREENPOWER_PROFILE_ID; // Profile ID for Green Power Cluster
    psDeviceInfo->sEndPoint.bIsManufacturerSpecificProfile =    FALSE;
    psDeviceInfo->sEndPoint.u16NumberOfClusters =               sizeof(tsGP_GreenPowerClusterInstances) / sizeof(tsZCL_ClusterInstance);
    psDeviceInfo->sEndPoint.psClusterInstance =                 (tsZCL_ClusterInstance*)&psDeviceInfo->sClusterInstance;
    psDeviceInfo->sEndPoint.bDisableDefaultResponse =           ZCL_DISABLE_DEFAULT_RESPONSES;
    psDeviceInfo->sEndPoint.pCallBackFunctions =                cbCallBack;

#ifdef CLD_GREENPOWER
    /* Copy Green Power Mapped End-point in zcl_common structure */
    psZCL_Common->u8GreenPowerMappedEpId = u8EndPointIdentifier;
#endif


    /* Create Server Green Power Cluster Instance */
   /* if(eGP_CreateGreenPower(
            &psDeviceInfo->sClusterInstance.sGreenPowerServer,
            TRUE,
            &sCLD_GreenPowerClient,
            NULL,
            NULL,
            &psDeviceInfo->sGreenPowerCustomDataStruct)
             != E_ZCL_SUCCESS)
    {
        return(E_ZCL_FAIL);
    }
*/
    /* Create Client Green Power Cluster Instance */
    if(eGP_CreateGreenPower(
            &psDeviceInfo->sClusterInstance.sGreenPowerClient,
            FALSE,
            &sCLD_GreenPowerClient,
            &psDeviceInfo->sClientGreenPowerCluster,
            &au8ClientGreenPowerClusterAttributeControlBits[0],
            &psDeviceInfo->sGreenPowerCustomDataStruct)
             != E_ZCL_SUCCESS)
    {
        return(E_ZCL_FAIL);
    }

    /* Set Default device operating mode */
    psDeviceInfo->sGreenPowerCustomDataStruct.eGreenPowerDeviceMode = E_GP_OPERATING_MODE;

    /* Set GP device type */
    psDeviceInfo->sGreenPowerCustomDataStruct.eGreenPowerDeviceType = E_GP_ZGP_PROXY_BASIC_DEVICE;

    /* Set u16CommissionSetAddress */
    psDeviceInfo->sGreenPowerCustomDataStruct.u64CommissionSetAddress = 0x00;
    psDeviceInfo->sGreenPowerCustomDataStruct.bCommissionUnicast = 0;
    /* Register bGP_AliasPresent() with ZPS for alias address conflict */
    ZPS_vAfRegisterAliasPresentCallback(&bGP_AliasPresent);

    /* Ignore profile check for loopback */
    zps_vSetIgnoreProfileCheck();

    /* Register u8GP_SecurityProcessCallback() with ZPS for Process security of received GPDF when a callback is received from stack */
    ZPS_vRegisterSecRequestResponseCallback(&u8GP_SecurityProcessCallback);

    /* Set bitmap for Mandatory features: This will be updated in future releases */

    return eZCL_Register(&psDeviceInfo->sEndPoint);
}
#endif
/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

