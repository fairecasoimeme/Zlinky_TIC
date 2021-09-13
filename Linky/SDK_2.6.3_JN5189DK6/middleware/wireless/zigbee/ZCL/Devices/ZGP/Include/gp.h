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
 * COMPONENT:          gp.h
 *
 * DESCRIPTION:        The API for the Green Power End Point Registration
 *
 *****************************************************************************/

#ifndef COMBOMIN_H
#define COMBOMIN_H

#if defined __cplusplus
extern "C" {
#endif

#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"
#include "GreenPower.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef GP_MANUFACTURER_CODE
#define GP_MANUFACTURER_CODE                                        (0)
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

    /* Holds cluster instances */
typedef struct
{
#ifdef GP_COMBO_BASIC_DEVICE
    tsZCL_ClusterInstance                  sGreenPowerServer;
#endif
    tsZCL_ClusterInstance                  sGreenPowerClient;
} tsGP_GreenPowerClusterInstances __attribute__ ((aligned(4)));


/* Holds everything required to create an instance of an Green Power device */
typedef struct
{
    tsZCL_EndPointDefinition               sEndPoint;

    /* Cluster instances */
    tsGP_GreenPowerClusterInstances        sClusterInstance;
    /* Holds the attributes for the Green Power cluster server */
#ifdef GP_COMBO_BASIC_DEVICE
    tsCLD_GreenPower                       sServerGreenPowerCluster;
#endif
   /* Holds the attributes for the Green Power cluster Client */
    tsCLD_GreenPower                       sClientGreenPowerCluster;

    /*Event Address, Custom call back event, Custom call back message*/
    tsGP_GreenPowerCustomData              sGreenPowerCustomDataStruct;
} tsGP_GreenPowerDevice;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC teZCL_Status eGP_RegisterComboBasicEndPoint(
                        uint8                               u8EndPointIdentifier,
                        tfpZCL_ZCLCallBackFunction          cbCallBack,
                        tsGP_GreenPowerDevice               *psDeviceInfo,
                        uint16                              u16ProfileId,
                        tsGP_TranslationTableEntry          *psTranslationTableEntry);

PUBLIC teZCL_Status eGP_RegisterProxyBasicEndPoint(
                        uint8                               u8EndPointIdentifier,
                        tfpZCL_ZCLCallBackFunction          cbCallBack,
                        tsGP_GreenPowerDevice               *psDeviceInfo);
/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/
extern const tsZCL_AttributeDefinition  asCLD_GreenPowerClusterAttributeDefinitionsClient[];
extern tsZCL_ClusterDefinition          sCLD_GreenPowerClient;
extern uint8                            au8ClientGreenPowerClusterAttributeControlBits[];

#ifdef GP_COMBO_BASIC_DEVICE
extern const tsZCL_AttributeDefinition  asCLD_GreenPowerClusterAttributeDefinitionsServer[];
extern tsZCL_ClusterDefinition          sCLD_GreenPowerServer;

extern uint8                            au8ServerGreenPowerClusterAttributeControlBits[];

#endif
/****************************************************************************/
#if defined __cplusplus
}
#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* COMBOMIN_H */
