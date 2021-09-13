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
 * MODULE:             ZGP proxy commissioning mode command
 *
 * COMPONENT:          GreenPowerProxyCommissioningMode.c
 *
 * DESCRIPTION:        ZGP proxy commissioning mode command send and receive functions
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"
#include "GreenPower.h"
#include "GreenPower_internal.h"

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
/***        Public Functions                                              ***/
/****************************************************************************/
#ifndef GP_DISABLE_PROXY_COMMISSION_MODE_CMD
/****************************************************************************
 **
 ** NAME:       eGP_ProxyCommissioningMode
 **
 ** DESCRIPTION:
 ** Broadcasts Proxy commission mode command and cluster move to self/pairing mode state
 **
 ** PARAMETERS:                             Name                                Usage
 ** uint8                                   u8SourceEndPointId                  Source EP Id
 ** uint8                                   u8DestEndPointId                    Destination EP Id
 ** tsZCL_Address                           sDestinationAddress                 Destination Address
 ** teGP_GreenPowerProxyCommissionMode      eGreenPowerProxyCommissionMode      Enter/Exit
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eGP_ProxyCommissioningMode(
                        uint8                               u8SourceEndPointId,
                        uint8                               u8DestEndPointId,
                        tsZCL_Address                       sDestinationAddress,
                        teGP_GreenPowerProxyCommissionMode  eGreenPowerProxyCommissionMode)
{
    tsZCL_EndPointDefinition                    *psEndPointDefinition;
    tsZCL_ClusterInstance                       *psClusterInstance;
    tsGP_GreenPowerCustomData                   *psGpCustomDataStructure;
    teZCL_Status                                eStatus;
    zbmap8                                      b8ZgpsCommExitModeAttr;
    tsGP_ZgpProxyCommissioningModeCmdPayload    sZgpProxyCommissioningModeCmdPayload = {0};
    uint16                                      u16CommissionWindow;
    uint8                                       u8TransactionSequenceNumber, u8Temp;

    /* Check Parameter range */
    if(eGreenPowerProxyCommissionMode > 0x01)
    {
        return E_ZCL_ERR_PARAMETER_RANGE;
    }

    /* Find Green Power Cluster */
    if((eStatus = eGP_FindGpCluster(u8SourceEndPointId,
                        TRUE,
                        &psEndPointDefinition,
                        &psClusterInstance,
                        &psGpCustomDataStructure)) != E_ZCL_SUCCESS)
    {
        return eStatus;
    }

    if(eGreenPowerProxyCommissionMode == E_GP_PROXY_COMMISSION_ENTER)
    {
    	/* reject the frame if the Involve TC field is set*/
    	if(bIsInvolveTC(u8SourceEndPointId))
    	{
    		return E_ZCL_FAIL;
    	}

    }
    /* Check Device Mode, Send Proxy Commission mode command only if device is operation mode
       If device is remote mode or pairing mode, don't send cmd and set device mode to pairing mode */
    if(psGpCustomDataStructure->eGreenPowerDeviceMode != E_GP_OPERATING_MODE &&
            eGreenPowerProxyCommissionMode == E_GP_PROXY_COMMISSION_ENTER)
    {
        psGpCustomDataStructure->eGreenPowerDeviceMode = E_GP_PAIRING_COMMISSION_MODE;
        psGpCustomDataStructure->u64CommissionSetAddress = 0x00;
        psGpCustomDataStructure->u16CommissionWindow = 0x00;
        psGpCustomDataStructure->bIsCommissionReplySent = 0;
#ifdef GP_COMBO_BASIC_DEVICE
        vGP_RemoveTemperorySinkTableEntries(u8SourceEndPointId);
#endif
        /* Get Green Power Exit Mode attribute value */
        if((eStatus = eZCL_ReadLocalAttributeValue(
                u8SourceEndPointId,
                GREENPOWER_CLUSTER_ID,
                TRUE,
                FALSE,
                FALSE,
                E_CLD_GP_ATTR_ZGPS_COMMISSIONING_EXIT_MODE,
                &b8ZgpsCommExitModeAttr)) != E_ZCL_SUCCESS)
        {
            return eStatus;
        }

        /* Check exit mode attribute, commission window expiration flag set or not */
        if(b8ZgpsCommExitModeAttr & GP_CMSNG_EXIT_MODE_ON_WINDOW_EXPIRE_MASK)
        {
            /* Check attribute present or not */
            if((eStatus = eZCL_ReadLocalAttributeValue(
                        u8SourceEndPointId,
                        GREENPOWER_CLUSTER_ID,
                        TRUE,
                        FALSE,
                        FALSE,
                        E_CLD_GP_ATTR_ZGPS_COMMISSIONING_WINDOW,
                        &u16CommissionWindow)) == E_ZCL_SUCCESS)
            {
                /* Set Commission Window Timeout value in custom data struct */
                psGpCustomDataStructure->u16CommissionWindow = u16CommissionWindow * 50;
            }
        }

        /* Set bIsCommissionIndGiven to FALSE */
        psGpCustomDataStructure->bIsCommissionIndGiven = FALSE;

        return eStatus;
    }

    /* send exit command if only enter command already sent */
    if((eGreenPowerProxyCommissionMode == E_GP_PROXY_COMMISSION_EXIT)&&
            (psGpCustomDataStructure->bProxyCommissionEnterCmdSent == FALSE))
    {
        psGpCustomDataStructure->eGreenPowerDeviceMode = E_GP_OPERATING_MODE;
        return eStatus;
    }
    /* Fill the proxy commission mode structure field for sending command */
    sZgpProxyCommissioningModeCmdPayload.b8Options |= eGreenPowerProxyCommissionMode;

    /* If enter only, populate exit mode field values */
    if(eGreenPowerProxyCommissionMode)
    {
        /* Get Green Power Exit Mode attribute value */
        if((eStatus = eZCL_ReadLocalAttributeValue(
                u8SourceEndPointId,
                GREENPOWER_CLUSTER_ID,
                TRUE,
                FALSE,
                FALSE,
                E_CLD_GP_ATTR_ZGPS_COMMISSIONING_EXIT_MODE,
                &b8ZgpsCommExitModeAttr)) != E_ZCL_SUCCESS)
        {
            return eStatus;
        }
        u8Temp = (b8ZgpsCommExitModeAttr << 1);

        sZgpProxyCommissioningModeCmdPayload.b8Options |=  u8Temp;
        psGpCustomDataStructure->eGreenPowerDeviceMode = E_GP_PAIRING_COMMISSION_MODE;
        psGpCustomDataStructure->bProxyCommissionEnterCmdSent = TRUE;
        psGpCustomDataStructure->bIsCommissionIndGiven = FALSE; /* Set bIsCommissionIndGiven to FALSE */
        psGpCustomDataStructure->bCommissionExitModeOnFirstPairSuccess = 0;
        psGpCustomDataStructure->bIsCommissionReplySent = 0;
#ifdef GP_COMBO_BASIC_DEVICE
        vGP_RemoveTemperorySinkTableEntries(u8SourceEndPointId);
#endif
        if(sZgpProxyCommissioningModeCmdPayload.b8Options & GP_PROXY_CMSNG_MODE_EXIT_MODE_ON_PAIRING_SUCCESS_MASK)
        {
        	psGpCustomDataStructure->bCommissionExitModeOnFirstPairSuccess = 1;

        }

       // psGpCustomDataStructure->u3CommissionReplySentCount = 0;
        /* Check exit mode attribute, commission window expiration flag set or not */
        if(b8ZgpsCommExitModeAttr & GP_CMSNG_EXIT_MODE_ON_WINDOW_EXPIRE_MASK)
        {
            /* Check attribute present or not */
            if((eStatus = eZCL_ReadLocalAttributeValue(
                        u8SourceEndPointId,
                        GREENPOWER_CLUSTER_ID,
                        TRUE,
                        FALSE,
                        FALSE,
                        E_CLD_GP_ATTR_ZGPS_COMMISSIONING_WINDOW,
                        &u16CommissionWindow)) == E_ZCL_SUCCESS)
            {
                /* Set Commission Window Attribute value to commission window field of Proxy Mode Command */
                sZgpProxyCommissioningModeCmdPayload.u16CommissioningWindow = u16CommissionWindow;

                /* Set Commission Window Timeout value in custom data struct */
                psGpCustomDataStructure->u16CommissionWindow = u16CommissionWindow * 50;
            }
        }
    }
    else
    {
        psGpCustomDataStructure->bProxyCommissionEnterCmdSent = FALSE;
        psGpCustomDataStructure->eGreenPowerDeviceMode = E_GP_OPERATING_MODE;
    }

    /* Send  Proxy Mode Command */
    return eGP_ProxyCommissioningModeSend(
                            u8SourceEndPointId,
                            u8DestEndPointId,
                            &sDestinationAddress,
                            &u8TransactionSequenceNumber,
                            &sZgpProxyCommissioningModeCmdPayload);
}


/****************************************************************************
 **
 ** NAME:       eGP_ProxyCommissioningModeSend
 **
 ** DESCRIPTION:
 ** Sends Proxy commissioning mode command
 **
 ** PARAMETERS:         Name                           Usage
 ** uint8               u8SourceEndPointId             Source EP Id
 ** uint8               u8DestinationEndPointId        Destination EP Id
 ** tsZCL_Address      *psDestinationAddress           Destination Address
 ** uint8              *pu8TransactionSequenceNumber   Sequence number Pointer
 ** tsGP_ZgpProxyCommissioningModeCmdPayload  *psZgpProxyCommissioningModeCmdPayload  command payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eGP_ProxyCommissioningModeSend(
                    uint8                                       u8SourceEndPointId,
                    uint8                                       u8DestinationEndPointId,
                    tsZCL_Address                               *psDestinationAddress,
                    uint8                                       *pu8TransactionSequenceNumber,
                    tsGP_ZgpProxyCommissioningModeCmdPayload    *psZgpProxyCommissioningModeCmdPayload)
{

    zbmap8 b8Options;
    uint8 u8ItemsInPayload;

    tsZCL_TxPayloadItem asPayloadDefinition[] = {
     {1,  E_ZCL_BMAP8,   &psZgpProxyCommissioningModeCmdPayload->b8Options},
     {1,  E_ZCL_UINT16,  &psZgpProxyCommissioningModeCmdPayload->u16CommissioningWindow},
     {1,  E_ZCL_UINT8,   &psZgpProxyCommissioningModeCmdPayload->u8Channel}
    };


    b8Options = psZgpProxyCommissioningModeCmdPayload->b8Options;
    u8ItemsInPayload = 1; //  mandatory items in command payload
    /* check if proxy commissioning mode command has action field as ENTER and commissioning window is present*/
    if((b8Options & GP_PROXY_CMSNG_MODE_ACTION_FIELD_MASK)&&(b8Options & GP_PROXY_CMSNG_MODE_EXIT_MODE_ON_WINDOW_EXPIRE_MASK))
    {
        u8ItemsInPayload += 1; // increase commissioning window optional field
    }
    /* check if channel field is present in command payload */
    if(b8Options & GP_PROXY_CMSNG_MODE_CHANNEL_PRESENT_MASK)
    {
        u8ItemsInPayload += 1; // increase channel optional field
        /* if commissioning window is not present and channel field is present then channel field
         * would be at second position */
        if(u8ItemsInPayload == 2)
        {
            asPayloadDefinition[1].eType = E_ZCL_UINT8;
            asPayloadDefinition[1].pvData = &psZgpProxyCommissioningModeCmdPayload->u8Channel;
        }
    }

    return eZCL_CustomCommandSend(u8SourceEndPointId,
                              u8DestinationEndPointId,
                              psDestinationAddress,
                              GREENPOWER_CLUSTER_ID,
                              TRUE,
                              E_GP_ZGP_PROXY_COMMOSSIONING_MODE,
                              pu8TransactionSequenceNumber,
                              asPayloadDefinition,
                              FALSE,
                              0,
                              u8ItemsInPayload);

}

/****************************************************************************
 **
 ** NAME:       eGP_ProxyCommissioningModeReceive
 **
 ** DESCRIPTION:
 ** Handles Proxy commissioning mode command
 **
 ** PARAMETERS:               Name                          Usage
 ** ZPS_tsAfEvent            *pZPSevent                     Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition          EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance             Cluster structure
 ** uint16                    u16Offset,                    offset
 ** tsGP_ZgpProxyCommissioningModeCmdPayload *psZgpProxyCommissioningModeCmdPayload  command payload
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eGP_ProxyCommissioningModeReceive(
                    ZPS_tsAfEvent                               *pZPSevent,
                    uint16                                      u16Offset,
                    tsGP_ZgpProxyCommissioningModeCmdPayload    *psZgpProxyCommissioningModeCmdPayload)
{

    uint8 u8TransactionSequenceNumber;
    uint16 u16ActualQuantity;
    zbmap8 b8Options;
    uint8 u8ItemsInPayload = 0;

    tsZCL_RxPayloadItem asPayloadDefinition[] = {
     {1, &u16ActualQuantity, E_ZCL_BMAP8,   &psZgpProxyCommissioningModeCmdPayload->b8Options},
     {1, &u16ActualQuantity, E_ZCL_UINT16,  &psZgpProxyCommissioningModeCmdPayload->u16CommissioningWindow},
     {1, &u16ActualQuantity, E_ZCL_UINT8,   &psZgpProxyCommissioningModeCmdPayload->u8Channel}
    };
    /* Read Options field from received command */
    u16ZCL_APduInstanceReadNBO(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                               u16Offset,
                               E_ZCL_BMAP8,
                               &b8Options);
    u8ItemsInPayload = 1; //  mandatory items in command payload
    /* check if proxy commissioning mode command has action field as ENTER and commissioning window is present*/
    if((b8Options & GP_PROXY_CMSNG_MODE_ACTION_FIELD_MASK)&&(b8Options & GP_PROXY_CMSNG_MODE_EXIT_MODE_ON_WINDOW_EXPIRE_MASK))
    {
        u8ItemsInPayload += 1; // increase commissioning window optional field
    }
    /* check if channel field is present in command payload */
    if(b8Options & GP_PROXY_CMSNG_MODE_CHANNEL_PRESENT_MASK)
    {
        u8ItemsInPayload += 1; // increase channel optional field
        /* if commissioning window is not present and channel field is present then channel field
         * would be at second position */
        if(u8ItemsInPayload == 2)
        {
            asPayloadDefinition[1].eType = E_ZCL_UINT8;
            asPayloadDefinition[1].pvDestination = &psZgpProxyCommissioningModeCmdPayload->u8Channel;
        }
    }

    return eZCL_CustomCommandReceive(pZPSevent,
                              &u8TransactionSequenceNumber,
                              asPayloadDefinition,
                              u8ItemsInPayload,
                              E_ZCL_ACCEPT_EXACT|E_ZCL_DISABLE_DEFAULT_RESPONSE);


}
#endif
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
