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
 * MODULE:             Green Power Cluster
 *
 * COMPONENT:          GreenPower.c
 *
 * DESCRIPTION:        The API for the Green Power Cluster
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>
#include "dlist.h"
#include "zcl.h"
#include "zcl_common.h"
#include "GreenPower.h"
#include "GreenPower_internal.h"
#include "dbg.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef TRACE_GP_DEBUG
#define TRACE_GP_DEBUG FALSE
#endif

#define GP_DEFAULT_SECURITY_VAL                           0x06
#define ALIAS_NWK_SEQ_NUM_ADDRESS_CONFLICT                0x30
#define ZPS_NWK_CMD_ADDR_CONFLICT                          0xD
#define GP_INVALID_CLUSTER_ID                            (0xFFFF)
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE void vCLD_GPTimerClickCallback(tsZCL_CallBackEvent *psCallBackEvent);
 teZCL_Status eCLD_GPRegisterTimeServer(void);
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/* Define the attributes in the Green Power cluster server */
#ifdef GP_COMBO_BASIC_DEVICE
const tsZCL_AttributeDefinition asCLD_GreenPowerClusterAttributeDefinitionsServer[] = {

    /* server attributes */
    {E_CLD_GP_ATTR_ZGPS_MAX_SINK_TABLE_ENTRIES,      (E_ZCL_AF_RD),                           E_ZCL_UINT8,     (uint32)(&((tsCLD_GreenPower*)(0))->u8ZgpsMaxSinkTableEntries), 0},
    {E_CLD_GP_ATTR_ZGPS_SINK_TABLE,                  (E_ZCL_AF_RD),                           E_ZCL_LOSTRING,  (uint32)(&((tsCLD_GreenPower*)(0))->sSinkTable), 0},
    {E_CLD_GP_ATTR_ZGPS_COMMUNICATION_MODE,          (E_ZCL_AF_WR | E_ZCL_AF_RD),             E_ZCL_BMAP8,     (uint32)(&((tsCLD_GreenPower*)(0))->b8ZgpsCommunicationMode), 0},
    {E_CLD_GP_ATTR_ZGPS_COMMISSIONING_EXIT_MODE,     (E_ZCL_AF_WR | E_ZCL_AF_RD),             E_ZCL_BMAP8,     (uint32)(&((tsCLD_GreenPower*)(0))->b8ZgpsCommissioningExitMode), 0},

#ifdef  CLD_GP_ATTR_ZGPS_COMMISSIONING_WINDOW
    {E_CLD_GP_ATTR_ZGPS_COMMISSIONING_WINDOW,        (E_ZCL_AF_WR | E_ZCL_AF_RD),             E_ZCL_UINT16,    (uint32)(&((tsCLD_GreenPower*)(0))->u16ZgpsCommissioningWindow), 0},
#endif

    {E_CLD_GP_ATTR_ZGPS_SECURITY_LEVEL,              (E_ZCL_AF_WR | E_ZCL_AF_RD),             E_ZCL_BMAP8,     (uint32)(&((tsCLD_GreenPower*)(0))->b8ZgpsSecLevel), 0},
    {E_CLD_GP_ATTR_ZGPS_FUNCTIONALITY,               (E_ZCL_AF_RD),                           E_ZCL_BMAP24,    (uint32)(&((tsCLD_GreenPower*)(0))->b24ZgpsFunctionality), 0},
    {E_CLD_GP_ATTR_ZGPS_ACTIVE_FUNCTIONALITY,        (E_ZCL_AF_RD),                           E_ZCL_BMAP24,    (uint32)(&((tsCLD_GreenPower*)(0))->b24ZgpsActiveFunctionality), 0},


    /* Shared Attributes b/w server and client */
#ifdef  CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY_TYPE
    {E_CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY_TYPE,      (E_ZCL_AF_WR | E_ZCL_AF_RD),             E_ZCL_BMAP8,     (uint32)(&((tsCLD_GreenPower*)(0))->b8ZgpSharedSecKeyType), 0},
#endif

#ifdef  CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY
    {E_CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY,           (E_ZCL_AF_WR | E_ZCL_AF_RD),             E_ZCL_KEY_128,   (uint32)(&((tsCLD_GreenPower*)(0))->sZgpSharedSecKey), 0},
#endif

#ifdef  CLD_GP_ATTR_ZGP_LINK_KEY
    {E_CLD_GP_ATTR_ZGP_LINK_KEY,                      (E_ZCL_AF_WR | E_ZCL_AF_RD),             E_ZCL_KEY_128,   (uint32)(&((tsCLD_GreenPower*)(0))->sZgpLinkKey), 0},
#endif
    {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,              (E_ZCL_AF_RD|E_ZCL_AF_GA),                 E_ZCL_BMAP32,   (uint32)(&((tsCLD_GreenPower*)(0))->u32FeatureMap),0},   /* Mandatory  */ 
    
    {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,         (E_ZCL_AF_RD|E_ZCL_AF_GA),                                E_ZCL_UINT16,  (uint32)(&((tsCLD_GreenPower*)(0))->u16ClusterRevision),0},   /* Mandatory  */

};
#endif
/* Client Attributes */
const tsZCL_AttributeDefinition asCLD_GreenPowerClusterAttributeDefinitionsClient[] = {


    {E_CLD_GP_ATTR_ZGPP_MAX_PROXY_TABLE_ENTRIES,      (E_ZCL_AF_RD | E_ZCL_AF_CA),             E_ZCL_UINT8,     (uint32)(&((tsCLD_GreenPower*)(0))->u8ZgppMaxProxyTableEntries), 0},
    {E_CLD_GP_ATTR_ZGPP_PROXY_TABLE,                  (E_ZCL_AF_RD | E_ZCL_AF_CA),             E_ZCL_LOSTRING,  (uint32)(&((tsCLD_GreenPower*)(0))->sProxyTable), 0},
#ifdef GP_PROXY_BASIC_DEVICE
#ifdef  CLD_GP_ATTR_ZGPP_NOTIFICATION_RETRY_NUMBER
    {E_CLD_GP_ATTR_ZGPP_NOTIFICATION_RETRY_NUMBER,    (E_ZCL_AF_WR | E_ZCL_AF_RD| E_ZCL_AF_CA),             E_ZCL_UINT8,     (uint32)(&((tsCLD_GreenPower*)(0))->u8ZgppNotificationRetryNumber), 0},
#endif

#ifdef  CLD_GP_ATTR_ZGPP_NOTIFICATION_RETRY_TIMER
    {E_CLD_GP_ATTR_ZGPP_NOTIFICATION_RETRY_TIMER,     (E_ZCL_AF_WR | E_ZCL_AF_RD| E_ZCL_AF_CA),             E_ZCL_UINT8,     (uint32)(&((tsCLD_GreenPower*)(0))->u8ZgppNotificationRetryTimer), 0},
#endif

#ifdef  CLD_GP_ATTR_ZGPP_MAX_SEARCH_COUNTER
    {E_CLD_GP_ATTR_ZGPP_MAX_SEARCH_COUNTER,           (E_ZCL_AF_WR | E_ZCL_AF_RD| E_ZCL_AF_CA),             E_ZCL_UINT8,     (uint32)(&((tsCLD_GreenPower*)(0))->u8ZgppMaxSearchCounter), 0},
#endif

#ifdef  CLD_GP_ATTR_ZGPP_BLOCKED_GPD_ID
    {E_CLD_GP_ATTR_ZGPP_BLOCKED_ZGPD_ID,              (E_ZCL_AF_RD| E_ZCL_AF_CA),                           E_ZCL_LOSTRING,  (uint32)(&((tsCLD_GreenPower*)(0))->sZgppBlockedGpdID), 0},
#endif

    {E_CLD_GP_ATTR_ZGPP_FUNCTIONALITY,                (E_ZCL_AF_RD| E_ZCL_AF_CA),                           E_ZCL_BMAP24,    (uint32)(&((tsCLD_GreenPower*)(0))->b24ZgppFunctionality), 0},
    {E_CLD_GP_ATTR_ZGPP_ACTIVE_FUNCTIONALITY,         (E_ZCL_AF_RD| E_ZCL_AF_CA),                           E_ZCL_BMAP24,    (uint32)(&((tsCLD_GreenPower*)(0))->b24ZgppActiveFunctionality), 0},


    /* Shared Attributes b/w server and client */
#ifdef  CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY_TYPE
    {E_CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY_TYPE,      (E_ZCL_AF_WR | E_ZCL_AF_RD | E_ZCL_AF_CA), E_ZCL_BMAP8,   (uint32)(&((tsCLD_GreenPower*)(0))->b8ZgpSharedSecKeyType), 0},
#endif

#ifdef  CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY
    {E_CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY,           (E_ZCL_AF_WR | E_ZCL_AF_RD | E_ZCL_AF_CA), E_ZCL_KEY_128, (uint32)(&((tsCLD_GreenPower*)(0))->sZgpSharedSecKey), 0},
#endif

#ifdef  CLD_GP_ATTR_ZGP_LINK_KEY
    {E_CLD_GP_ATTR_ZGP_LINK_KEY,                      (E_ZCL_AF_WR | E_ZCL_AF_RD | E_ZCL_AF_CA), E_ZCL_KEY_128, (uint32)(&((tsCLD_GreenPower*)(0))->sZgpLinkKey), 0},
#endif
#endif
    {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,              (E_ZCL_AF_RD|E_ZCL_AF_GA),  E_ZCL_BMAP32,   (uint32)(&((tsCLD_GreenPower*)(0))->u32FeatureMap),0},   /* Mandatory  */ 


    {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,         (E_ZCL_AF_RD|E_ZCL_AF_GA),                                E_ZCL_UINT16,  (uint32)(&((tsCLD_GreenPower*)(0))->u16ClusterRevision),0},   /* Mandatory  */
};

/* define the ZGP command cluster table to find out cluster id with respect to ZGPD command id */
const tsGP_ZgpCommandClusterTable asZgpCommadClusterTable[] = {
    {E_GP_IDENTIFY,               GP_GENERAL_CLUSTER_ID_IDENTIFY},
    {E_GP_OFF,                    GP_GENERAL_CLUSTER_ID_ONOFF},
    {E_GP_ON,                     GP_GENERAL_CLUSTER_ID_ONOFF},
    {E_GP_TOGGLE,                 GP_GENERAL_CLUSTER_ID_ONOFF},
    {E_GP_LEVEL_CONTROL_STOP,     GP_GENERAL_CLUSTER_ID_LEVEL_CONTROL},
    {E_GP_MOVE_UP_WITH_ON_OFF,    GP_GENERAL_CLUSTER_ID_LEVEL_CONTROL},
    {E_GP_MOVE_DOWN_WITH_ON_OFF,  GP_GENERAL_CLUSTER_ID_LEVEL_CONTROL},
    {E_GP_STEP_UP_WITH_ON_OFF,    GP_GENERAL_CLUSTER_ID_LEVEL_CONTROL},
    {E_GP_STEP_DOWN_WITH_ON_OFF,  GP_GENERAL_CLUSTER_ID_LEVEL_CONTROL},
    {E_GP_ZGPD_CMD_ID_ENUM_END,   GP_INVALID_CLUSTER_ID }
};
#ifdef GP_COMBO_BASIC_DEVICE
/* server cluser */
tsZCL_ClusterDefinition sCLD_GreenPowerServer = {
    GREENPOWER_CLUSTER_ID,
    FALSE,
    E_ZCL_SECURITY_NETWORK,
    (sizeof(asCLD_GreenPowerClusterAttributeDefinitionsServer) / sizeof(tsZCL_AttributeDefinition)),
    (tsZCL_AttributeDefinition*)asCLD_GreenPowerClusterAttributeDefinitionsServer,
    NULL
};
#endif

/* client cluser */
tsZCL_ClusterDefinition sCLD_GreenPowerClient = {
    GREENPOWER_CLUSTER_ID, //u16ClusterEnum
    FALSE, //bIsManufacturerSpecificCluster
    E_ZCL_SECURITY_NETWORK, //u8ClusterControlFlags
    (sizeof(asCLD_GreenPowerClusterAttributeDefinitionsClient) / sizeof(tsZCL_AttributeDefinition)), //u16NumberOfAttributes
    (tsZCL_AttributeDefinition*)asCLD_GreenPowerClusterAttributeDefinitionsClient, //psAttributeDefinition
    NULL //psSceneExtensionTable
};

#ifdef GP_COMBO_BASIC_DEVICE
uint8 au8ServerGreenPowerClusterAttributeControlBits[(sizeof(asCLD_GreenPowerClusterAttributeDefinitionsServer) / sizeof(tsZCL_AttributeDefinition))];
#endif
uint8 au8ClientGreenPowerClusterAttributeControlBits[(sizeof(asCLD_GreenPowerClusterAttributeDefinitionsClient) / sizeof(tsZCL_AttributeDefinition))];

/****************************************************************************
 **
 ** NAME:       eGP_CreateGreenPower
 **
 ** DESCRIPTION:
 ** Creates Green Power cluster instance either server or client
 **
 ** PARAMETERS:                         Name                                Usage
 ** tsZCL_ClusterInstance               *psClusterInstance                  Cluster structure
 ** bool_t                              bIsServer                           TRUE or FALSE
 ** tsZCL_ClusterDefinition             *psClusterDefinition                Cluster Definition Structure
 ** void                                *pvEndPointSharedStructPtr          EndPoint Shared Structure
 ** uint8                               *pu8AttributeControlBits            Attribute Control Bits
 ** tsGP_GreenPowerCustomData           *psCustomDataStructure              Custom Data Structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eGP_CreateGreenPower(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8                              *pu8AttributeControlBits,
                tsGP_GreenPowerCustomData          *psCustomDataStructure)
{
    tsGP_Common    *psGP_Common;
    uint8          u8Count = 0;
    uint8 u8NoOfEntries;
    #ifdef STRICT_PARAM_CHECK 
        /* Check pointers */
        if((psClusterInstance==NULL)            ||
           (psClusterDefinition==NULL)          ||
           (psCustomDataStructure==NULL))
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif

    // cluster data
    
    vZCL_InitializeClusterInstance(
                                   psClusterInstance, 
                                   bIsServer,
                                   psClusterDefinition,
                                   pvEndPointSharedStructPtr,
                                   pu8AttributeControlBits,
                                   psCustomDataStructure,
                                   eGP_GreenPowerCommandHandler);    

    psClusterInstance->pvEndPointCustomStructPtr = (void *)psCustomDataStructure;
    psClusterInstance->pStringCallBackFunction = u16GP_APduInstanceReadWriteLongString;
    // initialise pointer
    psGP_Common = &((tsGP_GreenPowerCustomData *)psClusterInstance->pvEndPointCustomStructPtr)->sGPCommon;

    // clear
    memset(psGP_Common, 0, sizeof(tsGP_Common));
    psGP_Common->sReceiveEventAddress.bInitialised = FALSE;

    // initialise the c/b structures
    psGP_Common->sGPCustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psGP_Common->sGPCustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = psClusterInstance->psClusterDefinition->u16ClusterEnum;
    psGP_Common->sGPCustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void *)&psGP_Common->sGreenPowerCallBackMessage;
    psGP_Common->sGPCustomCallBackEvent.psClusterInstance = psClusterInstance;

    /* Initialize list pointers */
    vDLISTinitialise(&psCustomDataStructure->lGpAllocList);
    vDLISTinitialise(&psCustomDataStructure->lGpDeAllocList);

    /* add all APDUs that are transmitted after delay  to the to free list */
    for(u8Count=0; u8Count < GP_MAX_NUMBER_BUFFERED_RECORDS; u8Count++)
    {
        vDLISTaddToTail(&psCustomDataStructure->lGpDeAllocList, (DNODE *)&psCustomDataStructure->asZgpBufferedApduRecord[u8Count]);
    }
    //initialize the aging time of duplicate table
    for(u8Count = 0; u8Count < GP_MAX_DUPLICATE_TABLE_ENTIRES; u8Count++)
    {
        psCustomDataStructure->asZgpDuplicateFilterTable[u8Count].u16AgingTime = 0;
    }

    psCustomDataStructure->u8GPDataReqHandle = ZPS_NWK_GP_BASE_HANDLE + 1;
    ((tsCLD_GreenPower*)pvEndPointSharedStructPtr)->u32FeatureMap = 0;
    ((tsCLD_GreenPower*)pvEndPointSharedStructPtr)->u16ClusterRevision = 1;

    if(bIsServer == FALSE)
    {
    	((tsCLD_GreenPower*)pvEndPointSharedStructPtr)->u8ZgppMaxProxyTableEntries = GP_NUMBER_OF_PROXY_SINK_TABLE_ENTRIES;
        ((tsCLD_GreenPower*)pvEndPointSharedStructPtr)->sProxyTable.u16MaxLength = u16GP_GetStringSizeOfProxyTable(0, &u8NoOfEntries, psCustomDataStructure);
        ((tsCLD_GreenPower*)pvEndPointSharedStructPtr)->sProxyTable.u16Length = u16GP_GetStringSizeOfProxyTable(0, &u8NoOfEntries, psCustomDataStructure);
        ((tsCLD_GreenPower*)pvEndPointSharedStructPtr)->sProxyTable.pu8Data =(uint8 *) psCustomDataStructure->asZgpsSinkProxyTable;
    }
    /* Initialize sink/Proxy table size */
#ifdef GP_COMBO_BASIC_DEVICE
    if(bIsServer == TRUE)
    {
		((tsCLD_GreenPower*)pvEndPointSharedStructPtr)->sSinkTable.u16MaxLength = u16GP_GetStringSizeOfSinkTable(0, &u8NoOfEntries, psCustomDataStructure);
		((tsCLD_GreenPower*)pvEndPointSharedStructPtr)->sSinkTable.u16Length = u16GP_GetStringSizeOfSinkTable(0, &u8NoOfEntries, psCustomDataStructure);
		((tsCLD_GreenPower*)pvEndPointSharedStructPtr)->sSinkTable.pu8Data =(uint8 *) psCustomDataStructure->asZgpsSinkProxyTable;

		((tsCLD_GreenPower *)(psClusterInstance->pvEndPointSharedStructPtr))->u8ZgpsMaxSinkTableEntries = GP_NUMBER_OF_PROXY_SINK_TABLE_ENTRIES;
    }
#endif
    psCustomDataStructure->u16TransmitChannelTimeout = 0;
    eCLD_GPRegisterTimeServer();
    return E_ZCL_SUCCESS;
}
/****************************************************************************
 **
 ** NAME:       eCLD_GPRegisterTimeServer
 **
 ** DESCRIPTION:
 ** Timer registartion  with the ZCL
 **
 ** PARAMETERS:                 Name               Usage
 ** None
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eCLD_GPRegisterTimeServer(void)
{
#ifndef IDENTIFY_PC_BUILD
    static bool_t bTimerRegistered = FALSE;
#endif
    if(!bTimerRegistered)
    {
        // add timer click function to ZCL
        if(eZCL_TimerRegister(E_ZCL_TIMER_CLICK_MS, 0, vCLD_GPTimerClickCallback) != E_ZCL_SUCCESS)
            return(E_ZCL_FAIL);

        bTimerRegistered = TRUE;
    }

    return E_ZCL_SUCCESS;
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/
/****************************************************************************
 **
 ** NAME:       vCLD_GPTimerClickCallback
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
PRIVATE void vCLD_GPTimerClickCallback(tsZCL_CallBackEvent *psCallBackEvent)
{
	static uint16 u16OneMSCounterValue = 0;

	eGP_Update1mS(psZCL_Common->u8GreenPowerMappedEpId);


	u16OneMSCounterValue++;
	if( (u16OneMSCounterValue % 20)== 0)
	{

		eGP_Update20mS(psZCL_Common->u8GreenPowerMappedEpId);
		u16OneMSCounterValue = 0;
	}
}

/****************************************************************************
**
** NAME:       eGP_FindGpCluster
**
** DESCRIPTION:
** Search for GreenPower cluster
**
** PARAMETERS:                   Name                            Usage
** uint8                         u8SourceEndPointId              Source EP Id
** bool_t                          bIsServer                       Server or Client
** tsZCL_EndPointDefinition      **ppsEndPointDefinition         EP
** tsZCL_ClusterInstance         **ppsClusterInstance            Cluster
** tsGP_GreenPowerCustomData     **ppsOTACustomDataStructure     event data
**
** RETURN:
** teZCL_Status
**
****************************************************************************/
PUBLIC teZCL_Status eGP_FindGpCluster(
                    uint8                          u8SourceEndPoint,
                    bool_t                           bIsServer,
                    tsZCL_EndPointDefinition       **ppsEndPointDefinition,
                    tsZCL_ClusterInstance          **ppsClusterInstance,
                    tsGP_GreenPowerCustomData      **ppsGpCustomDataStructure)
{
#ifdef GP_COMBO_BASIC_DEVICE
	bIsServer = TRUE;
#else
	bIsServer = FALSE;
#endif
    /* Check EP range */
    if(u8SourceEndPoint == 0)
    {
        return(E_ZCL_ERR_EP_RANGE);
    }

    // check EP is registered and cluster is present in the device
    if(eZCL_SearchForEPentry(u8SourceEndPoint, ppsEndPointDefinition) != E_ZCL_SUCCESS)
    {
        return(E_ZCL_ERR_EP_RANGE);
    }

    // search for cluster entry
    if(eZCL_SearchForClusterEntry(u8SourceEndPoint, GREENPOWER_CLUSTER_ID, bIsServer, ppsClusterInstance) != E_ZCL_SUCCESS)
    {
        return(E_ZCL_ERR_CLUSTER_NOT_FOUND);
    }

    // initialise custom data pointer
    *ppsGpCustomDataStructure = (tsGP_GreenPowerCustomData *)(*ppsClusterInstance)->pvEndPointCustomStructPtr;

    /* Check custom data structure pointer */
    if(*ppsGpCustomDataStructure==NULL)
    {
        return(E_ZCL_ERR_CUSTOM_DATA_NULL);
    }

    return(E_ZCL_SUCCESS);
}

/****************************************************************************
**
** NAME:       u16GP_DeriveAliasSrcAddr
**
** DESCRIPTION:
** Derivation of alias source address
**
** PARAMETERS:                  Name                      Usage
** uint8                        u8ApplicationId           application id to identified type of ZGPD ID
** tuGP_ZgpdDeviceAddr          uZgpdID                   ZGPD ID
**
** RETURN:
** uint16
****************************************************************************/
PUBLIC uint16 u16GP_DeriveAliasSrcAddr(uint8 u8ApplicationId, tuGP_ZgpdDeviceAddr uZgpdID)
{
    uint16 u16AliasShortAddr;
    uint32 u32ZgpdSrcAddr = 0;

    /* check application Id */
    if(0 == u8ApplicationId)
    {
        u32ZgpdSrcAddr = uZgpdID.u32ZgpdSrcId;
    }
#ifdef GP_IEEE_ADDR_SUPPORT
    else
    {
        u32ZgpdSrcAddr =    (uint32)uZgpdID.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr;
    }
#endif

    /* If 2 LSB of the GPD ID  do not correspond to any of the reserved ZigBee short addresses (0x0000 for the ZigBee Coor-dinator,
     *  and the addresses exceeding 0xfff7, reserved for broadcasts), this value is used as Ali-as_src_addr */
    u16AliasShortAddr = (uint16)u32ZgpdSrcAddr;     /* Take lower two bytes */

    /* Alias address shouldn't be ZCORD address and NWK broadcast addresses */
    if((u16AliasShortAddr != 0) && (u16AliasShortAddr < GP_NWK_MIN_BROADCAST_ADDRESS))
    {
        return(u16AliasShortAddr);
    }
    /*if the resulting Alias_src_addr does correspond to one of the reserved ZigBee short addresses, the 2 LSBs of the GPD ID
     *  SHALL be XORed with the 3rd and 4th LSB of the GPD ID, i.e.  1st LSB XORed with 3rd LSB and 2nd LSB XORed with 4th LSB**/

    /* Logical OR with higher two bytes */
    u16AliasShortAddr ^= (uint16)(u32ZgpdSrcAddr >> 16);

    /* Alias address shouldn't be ZCORD address and NWK broadcast addresses */
    if((u16AliasShortAddr != 0) && (u16AliasShortAddr < GP_NWK_MIN_BROADCAST_ADDRESS))
    {
        return(u16AliasShortAddr);
    }

    u16AliasShortAddr = (uint16)u32ZgpdSrcAddr;
    /* if alias address is ZCORD address then return 0x0007 , or else the value of 0x0008 is subtracted from the 2 LSB*/
    if(u16AliasShortAddr == 0)
    {
      return(0x0007);
    }
    else
    {
        return(u16AliasShortAddr - 0x0008);
    }
}

/****************************************************************************
**
** NAME:       u8GP_DeriveAliasSeqNum
**
** DESCRIPTION:
** Derivation of alias sequence number
**
** PARAMETERS:                          Name                       Usage
** uint8                                u8GpdfSeqNum               GPDF Sequence number
** teGP_GreenPowerClusterCmdIds       . eGpCmdId                   GP Command id
** teGP_GreenPowerCommunicationMode     eCommunicationMode         communication mode
**
** RETURN:
** uint8
****************************************************************************/
PUBLIC uint8 u8GP_DeriveAliasSeqNum(
                    uint8                                  u8GpdfSeqNum,
                    teGP_GreenPowerClusterCmdIds           eGpCmdId,
                    teGP_GreenPowerCommunicationMode       eCommunicationMode)
{

    if(E_GP_ZGP_NOTIFICATION == eGpCmdId)
    {
        if(E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID == eCommunicationMode)
        {
        	/* The derived groupcast GP Notification command uses the exact value from the GPDF MAC header Sequence number field */
            return  u8GpdfSeqNum;
        }
        else if(E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_PRE_COMMISSION_GROUP_ID == eCommunicationMode)
        {
        	/* The commissioned groupcast GP Notification command uses the value: GPDF_MAC_header_Sequence_number - 9 (mod 256) */
            return(u8GpdfSeqNum - 9);
        }
    }
    /* The GP Commissioning Notification command uses the value: GPDF_MAC_header_Sequence_number - 12 (mod 256) */
    else if(E_GP_ZGP_COMMOSSIONING_NOTIFICATION == eGpCmdId)
    {
        return(u8GpdfSeqNum - 12);
    }
    /* The GP Tunnelling Stop command uses the value: GPDF_MAC_header_Sequence_number - 11 (mod 256); */
    else if(E_GP_ZGP_TUNNELING_STOP == eGpCmdId)
    {
        return(u8GpdfSeqNum - 11);
    }
    /* The GP Pairing Search command uses the value: GPDF_MAC_header_Sequence_number - 10 (mod 256); */
    else if(E_GP_ZGP_PAIRING_SEARCH == eGpCmdId)
    {
       return(u8GpdfSeqNum - 10);
    }

    return 0;
}

/****************************************************************************
 **
 ** NAME:       u32GP_CRC32
 **
 ** DESCRIPTION:
 ** Calculates 32 bit CRC
 **
 ** PARAMETERS:                  Name                      Usage
 ** uint8                        *pu8Data                  data pointer to calculate CRC
 ** uint32                       u32Len                    length of the data
 **
 ** RETURN:
 ** uint32
 ****************************************************************************/
PUBLIC uint32 u32GP_CRC32(uint8 *pu8Data, uint32 u32Len)
{
    uint32 u32Result;
    uint8 u8Octet;
    unsigned int i,j;

    u32Result = -1;

    for (i = 0; i < u32Len; i++)
    {
        u8Octet = *(pu8Data++);
        for (j = 0; j < 8; j++)
        {
            if ((u8Octet >> 7) ^ (u32Result >> 31))
            {
                u32Result = (u32Result << 1) ^ 0x04c11db7UL;
            }
            else
            {
                u32Result = (u32Result << 1);
            }
            u8Octet <<= 1;
        }
    }

    return ~u32Result; /* The complement of the remainder */
}

/****************************************************************************
 **
 ** NAME:       bGP_IsDuplicatePkt
 **
 ** DESCRIPTION:
 ** performing Duplicate filter check
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          Source EP ID
 ** bool_t                        bIsServer                   Server cluster or client cluster
 ** uint16                      u16ShortAddr                Short Address
 ** uint8                       u8SecLevel                   Security Level
 ** uint32                      u32SeqNoOrCounter           MAC Sequence Number or Security Frame Counter
 ** uint8                       u8CommandId                 GP Command Id
 ** uint8                       u8PayloadLength             GP Command Length
 ** uint8                       *pu8Payload                 GP Command payload
 ** uint16                      u16SourceAddress            Source short address
 **
 ** RETURN:
 ** bool_t
 ****************************************************************************/
PUBLIC bool_t bGP_IsDuplicatePkt(
					uint8									u8TimeOutInSec,
                    uint8                                  u8SourceEndPointId,
                    bool_t                                   bIsServer,
                    uint8                                  u8ApplicationID,
                    tuGP_ZgpdDeviceAddr                    uZgpdDeviceAddr,
                    uint8                                  u8SecLevel,
                    uint32                                 u32SeqNoOrCounter,
                    uint8                                  u8CommandId,
                    tsGP_GreenPowerCustomData              *psGpCustomDataStructure)
{

    uint8 i, u8TempData[12], u8Index = 0;
    uint32 u32CRC = 0;

    /* If packet rxed with no security, calculate CRC over device address, counter, GP command id */
    if(u8SecLevel == 0)
    {

        if((u8ApplicationID == GP_APPL_ID_4_BYTE) || (u8ApplicationID == (GP_APPL_ID_4_BYTE + 1))) // GP_APPL_ID_4_BYTE + 1 is passed to ignore GPDF sent to extract response
        {
            memcpy(u8TempData,(uint8*)&(uZgpdDeviceAddr.u32ZgpdSrcId),4);


            u8Index += 4;
        }
#ifdef GP_IEEE_ADDR_SUPPORT
        else
        {
            memcpy(u8TempData,(uint8*)&uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr,8);

            u8Index += 8;
            u8TempData[u8Index++] = uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint;
        }
#endif


        u8TempData[u8Index++] = (uint8)u32SeqNoOrCounter;
        u8TempData[u8Index++] = u8CommandId;
        u32CRC = u32GP_CRC32(u8TempData,(uint32)(u8Index));

    }

    /* traverse through duplicate table to check entry is existed or not. If entry existed
     * returns TRUE otherwise FALSE */
    for(i = 0; i< GP_MAX_DUPLICATE_TABLE_ENTIRES; i++)
    {
        if(psGpCustomDataStructure->asZgpDuplicateFilterTable[i].u16AgingTime != 0)
        {

        	if((psGpCustomDataStructure->asZgpDuplicateFilterTable[i].u3ApplicationID ==u8ApplicationID) &&
        			((psGpCustomDataStructure->asZgpDuplicateFilterTable[i].uZgpdDeviceAddr.u32ZgpdSrcId == uZgpdDeviceAddr.u32ZgpdSrcId)||
        			(psGpCustomDataStructure->asZgpDuplicateFilterTable[i].uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr  == uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr)))

        	{
                if(psGpCustomDataStructure->asZgpDuplicateFilterTable[i].u3SecLevel)
                {
                     /* compare */
                    if(psGpCustomDataStructure->asZgpDuplicateFilterTable[i].uData.u32SecFrameCounter == u32SeqNoOrCounter)
                    {
                        return TRUE;
                    }
                }
                else
                {   /* compare */
                    if(u32CRC == psGpCustomDataStructure->asZgpDuplicateFilterTable[i].uData.u32gpdCrc)
                    {
                        return TRUE;
                    }
                }
            }
        }
    }
    /* packet is not duplicate so add this entry in duplicate filter */
    if(u8TimeOutInSec)
	{
		for(i = 0; i< GP_MAX_DUPLICATE_TABLE_ENTIRES; i++)
		 {
			/* find a free duplicate table entry */
			if(psGpCustomDataStructure->asZgpDuplicateFilterTable[i].u16AgingTime == 0)
			{
				psGpCustomDataStructure->asZgpDuplicateFilterTable[i].u16AgingTime = (u8TimeOutInSec * 50);
				psGpCustomDataStructure->asZgpDuplicateFilterTable[i].u3ApplicationID = u8ApplicationID;
				psGpCustomDataStructure->asZgpDuplicateFilterTable[i].uZgpdDeviceAddr = uZgpdDeviceAddr;
				psGpCustomDataStructure->asZgpDuplicateFilterTable[i].u3SecLevel = u8SecLevel;
				if(u8SecLevel)
				{
					psGpCustomDataStructure->asZgpDuplicateFilterTable[i].uData.u32SecFrameCounter = u32SeqNoOrCounter;
				}
				else
				{
					psGpCustomDataStructure->asZgpDuplicateFilterTable[i].uData.u32gpdCrc = u32CRC;
				}
				break;
			}
		 }
	}

    return FALSE;
}

/****************************************************************************
 **
 ** NAME:       u16GP_APduInstanceReadWriteLongString
 **
 ** DESCRIPTION:
 ** Performs either writing into APDU or updates attribute value based received command type
 **
 ** PARAMETERS:                  Name                               Usage
 ** uint8                        u8EndPoint                         End Point Id
 ** uint16                       u16AttributeId                     Attribute Id
 ** bool_t                       IsClientAttribute                  Client attribute or not
 ** bool_t                       bWriteAttributeValueIntoBuffer     Write into APDU buffer or update local attribute value
 ** PDUM_thAPduInstance          hAPduInst                          PDU buffer
 ** uint16                       u16Pos                             position in buffer
 ** uint16                       *pu16NoOfBytes                     Number of bytes to be written or read to/from APDU
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status u16GP_APduInstanceReadWriteLongString(
                    uint8                                  u8EndPoint,
                    uint16                                 u16AttributeId,
                    bool_t                                 IsClientAttribute,
                    bool_t                                 bWriteAttributeValueIntoBuffer,
                    PDUM_thAPduInstance                    hAPduInst,
                    uint16                                 u16Pos,
                    uint16                                 *pu16NoOfBytes)
{

    uint16 u16stringSize, u16APduSize;
    uint8										u8NoOfTableEntries=0;
    if(hAPduInst)
    {
#ifdef GP_COMBO_BASIC_DEVICE

        /* Is Attribute Id sink table attribute Id and attribute should be server attribute */
        if((u16AttributeId == E_CLD_GP_ATTR_ZGPS_SINK_TABLE)&&(!IsClientAttribute))
        {
            /* write into APDU buffer or read from buffer */
            if(bWriteAttributeValueIntoBuffer)
            {
                // get APDU size
                uint16 u16APduSize = PDUM_u16APduGetSize(PDUM_thAPduInstanceGetApdu(hAPduInst));

                // get string size
                u16stringSize = u16GP_GetStringSizeOfSinkTable(u8EndPoint, &u8NoOfTableEntries, NULL);

                // check that whole string can fit into the buffer
                // write to buffer
                if ((u16Pos + u16stringSize + 1) < u16APduSize)
                {
                    // get start
                    uint8 *pu8Start = (uint8 *)(PDUM_pvAPduInstanceGetPayload(hAPduInst)) + u16Pos;
                    // write type, return size
                    *pu16NoOfBytes = u16GP_ReadFromSinkTableToString(u8EndPoint, pu8Start, u16stringSize);
                    DBG_vPrintf(TRACE_GP_DEBUG, "ASinkTable size = %d  u16stringSize = %d \n",*pu16NoOfBytes, u16stringSize);
                    return E_ZCL_SUCCESS;
                }
                else
                {
                	DBG_vPrintf(TRACE_GP_DEBUG, "E_ZCL_ERR_INSUFFICIENT_SPACE ASinkTable size = %d  u16stringSize = %d \n",*pu16NoOfBytes, u16stringSize);
                	return E_ZCL_ERR_INSUFFICIENT_SPACE;
                }
            }
        }

#endif

        /* Is Attribute Id proxy table attribute Id and attribute should be client attribute */

        if((u16AttributeId == E_CLD_GP_ATTR_ZGPP_PROXY_TABLE)&&(IsClientAttribute))
        {
            /* write into APDU buffer or read from buffer */
            if(bWriteAttributeValueIntoBuffer)
            {
            	/* write into APDU */
                // get APDU size
                u16APduSize = PDUM_u16APduGetSize(PDUM_thAPduInstanceGetApdu(hAPduInst));

                // get string size
                u16stringSize = u16GP_GetStringSizeOfProxyTable(u8EndPoint, &u8NoOfTableEntries, NULL);


                // check that whole string can fit into the buffer
                // write to buffer
                if ((u16Pos + u16stringSize + 1) < u16APduSize)
                {
                    // get start
                    uint8 *pu8Start = (uint8 *)(PDUM_pvAPduInstanceGetPayload(hAPduInst)) + u16Pos;
                    // write type, return size
                    *pu16NoOfBytes = u16GP_ReadProxyTableToString(u8EndPoint, pu8Start, u16stringSize);
                    DBG_vPrintf(TRACE_GP_DEBUG, "Proxy table  size = %d \n",*pu16NoOfBytes);
                    return E_ZCL_SUCCESS;
                }
                else
                {
                	return E_ZCL_ERR_INSUFFICIENT_SPACE;
                }
            }
            /* Proxy and Sink table are read only, no need to suppot write functionality*/

        }

    }

    return E_ZCL_FAIL;
}

/****************************************************************************
 **
 ** NAME:       bGP_AliasPresent
 **
 ** DESCRIPTION:
 ** Verifying device announce address with ZGPD alias address. This is called from
 ** stack to check for address conflict
 **
 ** PARAMETERS:                    Name                           Usage
 ** uint16                         u16Address                     16-bit NWK address of device
 ** uint64                         u64IEEEAddr                    64-bit IEEE Address of device
 ** uint8                          u8AliasSeq                     Rxed APS sequece number in device announce
 **
 **
 ** RETURN:
 ** bool_t
 ****************************************************************************/
PUBLIC bool_t bGP_AliasPresent(uint16 u16Address, uint64 u64IEEEAddr, uint8 u8AliasSeq)
{
    tsZCL_EndPointDefinition        *psEndPointDefinition;
    tsZCL_ClusterInstance           *psClusterInstance;
    tsGP_GreenPowerCustomData       *psGpCustomDataStructure;
    tsGP_ZgppProxySinkTable             *psZgppSinkProxyTable;
    teZCL_Status                    eStatus;
    uint8                           u8NumberOfendpoints, i, j;
    bool_t                            bIsGpClusterFound = FALSE, bAliasMatched = FALSE;

    u8NumberOfendpoints = u8ZCL_GetNumberOfEndpointsRegistered();

    // find GP clusters on each EP - if any
    for(i=0; i<u8NumberOfendpoints; i++)
    {
       /* As local GP end point is not known find GP end point */
        if((eStatus = eGP_FindGpCluster(
                            u8ZCL_GetEPIdFromIndex(i),
                            TRUE,
                            &psEndPointDefinition,
                            &psClusterInstance,
                            &psGpCustomDataStructure)) == E_ZCL_SUCCESS)
        {
            bIsGpClusterFound = TRUE;
            break;
        }
    }

    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    /* If GP cluster found then verify alias address */
    /* Check only if IEEE address is other than 0xFFFFFFFFFFFFFFFF */
    if(bIsGpClusterFound && u64IEEEAddr != 0xFFFFFFFFFFFFFFFFULL)
    {

    	psZgppSinkProxyTable = psGpCustomDataStructure->asZgpsSinkProxyTable;

        /* Traverse Sink/Proxy table */
        for(i = 0; i < GP_NUMBER_OF_PROXY_SINK_TABLE_ENTRIES; i++)
        {
        	  uint16 u16AliasAddr;
            /* 1. check any of sink address list member matched with passed NWK address,if yes then update the mapping of
             * 	        sink IEEE address and sink nwk addr
            * 2. check any of alias address matched with passed NWK address
            * 3. Check if assigned address present, if yes check for conflict
            * 4. Check conflict for derived alias address */

            if(psZgppSinkProxyTable[i].b16Options & GP_PROXY_TABLE_ASSIGNED_ALIAS_MASK)
            {
                u16AliasAddr = psZgppSinkProxyTable[i].u16ZgpdAssignedAlias;
                /* compare */
                if(u16AliasAddr == u16Address)
                {
                    bAliasMatched = TRUE;
                    break;
                }
            }

            if(psZgppSinkProxyTable[i].b16Options & GP_PROXY_TABLE_COMMISSION_GROUP_GPS_MASK)
             {
                for(j = 0; j < psZgppSinkProxyTable[i].u8SinkGroupListEntries; j++)
                {
                	u16AliasAddr = psZgppSinkProxyTable[i].asSinkGroupList[j].u16Alias ;
                    if(u16AliasAddr == u16Address)
                    {
                        bAliasMatched = TRUE;
                        break;
                    }
                    else if((psZgppSinkProxyTable[i].asSinkGroupList[j].u16Alias == 0xFFFF)&&(psZgppSinkProxyTable[i].asSinkGroupList[j].u16SinkGroup != 0))
                    {
                        /* Get derived address for this device */
                        u16AliasAddr = u16GP_DeriveAliasSrcAddr(
                                (uint8)(psZgppSinkProxyTable[i].b16Options & GP_APPLICATION_ID_MASK),
                                psZgppSinkProxyTable[i].uZgpdDeviceAddr);
                        /* compare */
                        if(u16AliasAddr == u16Address)
                        {
                            bAliasMatched = TRUE;
                            break;
                        }
                    }
                }
            }

            if(psZgppSinkProxyTable[i].b16Options & GP_PROXY_TABLE_DERIVED_GROUP_GPS_MASK)
            {
                /* Get derived address for this device */
                u16AliasAddr = u16GP_DeriveAliasSrcAddr(
                        (uint8)(psZgppSinkProxyTable[i].b16Options & GP_APPLICATION_ID_MASK),
                        psZgppSinkProxyTable[i].uZgpdDeviceAddr);
                /* compare */
                if(u16AliasAddr == u16Address)
                {
                    bAliasMatched = TRUE;
                    break;
                }
            }
        }
    }

    if(bAliasMatched)
    {
    	uint64 u64SelfIEEEAddress = ZPS_u64AplZdoGetIeeeAddr();
    	tsGP_ZgpDataIndication              sZgpDataIndication;
    	memset(&sZgpDataIndication,0,sizeof(sZgpDataIndication));
    	sZgpDataIndication.u16NwkShortAddr =u16Address;

    	if(u64SelfIEEEAddress == u64IEEEAddr)
    	{
    		DBG_vPrintf(TRACE_GP_DEBUG, "u64SelfIEEEAddress == u64IEEEAddr \n");
    		/* Use the u8Status to indicate that self is to change the address and send device announce with its address*/
    		sZgpDataIndication.u8Status = 1;
    	}
    	DBG_vPrintf(TRACE_GP_DEBUG, "bGP_AliasPresent bAliasMatched u8SelfIEEEAddress = 0x%16llx, u64IEEEAddr = 0x%16llx\n",
    			u64SelfIEEEAddress, u64IEEEAddr);
    	eGp_BufferTransmissionPacket(
    	                    &sZgpDataIndication,
    	                    E_GP_DEV_ANCE,
    	                    psGpCustomDataStructure);

		// vGP_SendDeviceAnnounce(u16Address);
		 return TRUE;
    }
    // release mutex
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

    return FALSE;
}

/****************************************************************************
 **
 ** NAME:       u8GP_SecurityProcessCallback
 **
 ** DESCRIPTION:
 ** Process security of received GPDF when a callback is received from stack
 **
 ** PARAMETERS:                    Name                           Usage
 ** ZPS_tsAfZgpSecReq              psSec                      pointer of security request structure
 ** uint8                         pu8SecIndex                pointer to index of security table
 **
 **
 ** RETURN:
 ** uint8
 ****************************************************************************/
PUBLIC uint8 u8GP_SecurityProcessCallback(ZPS_tsAfZgpSecReq *psSec, uint8 *pu8SecIndex)
{

	ZPS_tsAfZgpGpstEntry     *psZgpGpstEntry;
    uint8 i;
    uint8 u8Status;
    if(psSec->u8SecurityLevel == E_GP_NO_SECURITY)
    {
        return ((uint8)ZPS_E_PASS_UNPROCESSED);
    }

    DBG_vPrintf(TRACE_GP_DEBUG, "\nSecurity Request callback u8KeyType = %d\n", psSec->u8KeyType);

    psZgpGpstEntry = ZPS_psZgpFindGpstEntry((uint32)psSec->u8ApplicationId, psSec->uGpId, pu8SecIndex);

    /* GP security table entry not found for this GPD then add a entry in security table */
    if(psZgpGpstEntry == NULL)
    {
    	u8Status =eGP_AddDeviceSecurity(psSec);
        if(u8Status != ZPS_E_MATCH)
        {
            DBG_vPrintf(TRACE_GP_DEBUG, " eGP_AddDeviceSecurity  failed = %d\n", u8Status);
            return((uint8)u8Status);
        }

        psZgpGpstEntry = ZPS_psZgpFindGpstEntry((uint32)psSec->u8ApplicationId, psSec->uGpId, pu8SecIndex);
    }
    else if(psSec->u8KeyType && psZgpGpstEntry->u8KeyType < E_GP_OUT_OF_THE_BOX_ZGPD_KEY)
    {
    	DBG_vPrintf(TRACE_GP_DEBUG, " eGP_AddDeviceSecurity  failed1 = %d %d \n", psZgpGpstEntry->u8KeyType, psSec->u8KeyType);
        return bGP_CheckFrameToBeProcessed();
    }
    else if(!psSec->u8KeyType && psZgpGpstEntry->u8KeyType > E_GP_NWK_KEY_DERIVED_ZGPD_GROUP_KEY)
    {
    	DBG_vPrintf(TRACE_GP_DEBUG, " eGP_AddDeviceSecurity  failed2 = %d %d \n", psZgpGpstEntry->u8KeyType, psSec->u8KeyType);
        return bGP_CheckFrameToBeProcessed();
    }

#if 1
    
    for(i=0;i<AESSW_BLK_SIZE;i++)
    DBG_vPrintf(TRACE_GP_DEBUG, " 0x%x,", psZgpGpstEntry->uSecurityKey.au8[i]);
    DBG_vPrintf(TRACE_GP_DEBUG, "\nGPD ID 0x%08x\n", psZgpGpstEntry->uGpId.u32SrcId);
    DBG_vPrintf(TRACE_GP_DEBUG, "GPST security level %d\n", psZgpGpstEntry->u8SecurityLevel);
    DBG_vPrintf(TRACE_GP_DEBUG, "GPST key type %d\n", psZgpGpstEntry->u8KeyType);
    DBG_vPrintf(TRACE_GP_DEBUG, "GPST valid %d\n", psZgpGpstEntry->bValid);
#endif

    if(psSec->u8SecurityLevel > E_GP_NO_SECURITY)
    {
        psZgpGpstEntry->u32Counter = psSec->u32FrameCounter;
    }

    return((uint8)ZPS_E_MATCH );
}
/****************************************************************************
 **
 ** NAME:       vGP_CallbackForPersistData
 **
 ** DESCRIPTION:
 ** call persist attribute data call back message to application
 **
 ** PARAMETERS:                    Name                           Usage
 ** teGP_ZgpsPersistChange 			eGPChangeCause
 **
 ** RETURN:
 ** None
 ****************************************************************************/
PUBLIC void vGP_CallbackForPersistData(void)
{

    tsZCL_EndPointDefinition        *psEndPointDefinition;
    tsZCL_ClusterInstance           *psClusterInstance;
    tsGP_GreenPowerCustomData       *psGpCustomDataStructure;
    uint8                           u8NumberOfendpoints, i;

    u8NumberOfendpoints = u8ZCL_GetNumberOfEndpointsRegistered();

    // find GP clusters on each EP - if any
    for(i=0; i<u8NumberOfendpoints; i++)
    {
       /* As local GP end point is not known find GP end point */

        if(eGP_FindGpCluster(u8ZCL_GetEPIdFromIndex(i),
#ifdef GP_COMBO_BASIC_DEVICE
                             TRUE,
#else
                             FALSE,
#endif
                             &psEndPointDefinition,
                             &psClusterInstance,
                             &psGpCustomDataStructure) == E_ZCL_SUCCESS)
        {

            psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_PERSIST_SINK_PROXY_TABLE;

            psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.uMessage.psZgpsProxySinkTable =
            		psGpCustomDataStructure->asZgpsSinkProxyTable;
            psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);
			return;
        }
    }
}


/****************************************************************************
 **
 ** NAME:       vGP_RestorePersistedData
 **
 ** DESCRIPTION:
 ** Restore persisted attribute data during initialisation
 **
 ** PARAMETERS:                    Name                           Usage
 ** tsGP_ZgppProxySinkTable        *psZgpsProxySinkTable              persisted data pointer passed by application
 ** teGP_ResetToDefaultConfig  eSetToDefault                 if application want to set attribute data to default value then set TRUE
 ** RETURN:
 ** None
 ****************************************************************************/
PUBLIC void vGP_RestorePersistedData( tsGP_ZgppProxySinkTable                   *psZgpsProxySinkTable,
									teGP_ResetToDefaultConfig                 eSetToDefault)
{
    tsZCL_EndPointDefinition        *psEndPointDefinition;
    tsZCL_ClusterInstance           *psClusterInstance;
    tsGP_GreenPowerCustomData       *psGpCustomDataStructure;
    uint8                           u8NumberOfendpoints, i;
#ifdef GP_COMBO_BASIC_DEVICE
    uint8                           j;
#endif

    u8NumberOfendpoints = u8ZCL_GetNumberOfEndpointsRegistered();

    // find GP clusters on each EP - if any
    for(i=0; i<u8NumberOfendpoints; i++)
    {
       /* As local GP end point is not known find GP end point */
        if(eGP_FindGpCluster(u8ZCL_GetEPIdFromIndex(i),
#ifdef GP_COMBO_BASIC_DEVICE
                             TRUE,
#else
                             FALSE,
#endif
                             &psEndPointDefinition,
                             &psClusterInstance,
                             &psGpCustomDataStructure) == E_ZCL_SUCCESS)
        {
            // get EP mutex
            #ifndef COOPERATIVE
                eZCL_GetMutex(psEndPointDefinition);
            #endif

            // server attributes set
			if(eSetToDefault & E_GP_DEFAULT_PROXY_SINK_TABLE_VALUE)
            {

				DBG_vPrintf(TRACE_GP_DEBUG, "Resetting sink table\n");
				if(psZgpsProxySinkTable != NULL)
				{
					memset(psZgpsProxySinkTable, 0, (sizeof(tsGP_ZgppProxySinkTable) *GP_NUMBER_OF_PROXY_SINK_TABLE_ENTRIES) );
				}
				else
				{
					memset(psGpCustomDataStructure->asZgpsSinkProxyTable,0,
							sizeof(tsGP_ZgppProxySinkTable) * GP_NUMBER_OF_PROXY_SINK_TABLE_ENTRIES);
				}
#ifdef GP_COMBO_BASIC_DEVICE
				for(j = 0; j< GP_NUMBER_OF_TRANSLATION_TABLE_ENTRIES; j++)
				{
					psGpCustomDataStructure->psZgpsTranslationTableEntry[j].psGpToZclCmdInfo = NULL;
				}
#endif
			}
            if(eSetToDefault & E_GP_DEFAULT_ATTRIBUTE_VALUE)
			{    // server attributes set default value

				DBG_vPrintf(TRACE_GP_DEBUG, "Resetting attribute values\n");

#ifdef GP_COMBO_BASIC_DEVICE
				((tsCLD_GreenPower *)(psClusterInstance->pvEndPointSharedStructPtr))->b8ZgpsCommunicationMode =  E_GP_GROUP_FORWARD_ZGP_NOTIFICATION_TO_DGROUP_ID;
				((tsCLD_GreenPower *)(psClusterInstance->pvEndPointSharedStructPtr))->b8ZgpsCommissioningExitMode = GP_CMSNG_EXIT_MODE_ON_PAIRING_SUCCESS_MASK;

#ifdef  CLD_GP_ATTR_ZGPS_COMMISSIONING_WINDOW
				((tsCLD_GreenPower *)(psClusterInstance->pvEndPointSharedStructPtr))->u16ZgpsCommissioningWindow = GP_COMMISSION_WINDOW_DURATION;
#endif

				((tsCLD_GreenPower *)(psClusterInstance->pvEndPointSharedStructPtr))->b8ZgpsSecLevel = GP_DEFAULT_SECURITY_VAL;
				((tsCLD_GreenPower *)(psClusterInstance->pvEndPointSharedStructPtr))->b24ZgpsActiveFunctionality = GP_ZGP_ACTIVE_FEATURES_ATTR_DEFAULT_VALUE;
				/* ZGP Server features */

				((tsCLD_GreenPower *)(psClusterInstance->pvEndPointSharedStructPtr))->b24ZgpsFunctionality = GP_FEATURE_ZGPS_FEATURE |
																										GP_FEATURE_ZGPS_DIRECT_COMM |
																										GP_FEATURE_ZGPS_DERIVED_GC_COMM |
																										GP_FEATURE_ZGPS_COMMISSION_GC_COMM |
																										GP_FEATURE_ZGPP_LIGHTWEIGHT_UNICAST_COMM|
																										GP_FEATURE_ZGPS_PROXIMITY_CMSNG |
																										GP_FEATURE_ZGPS_MULTI_HOP_CMSNG |
																										GP_FEATURE_ZGPS_CT_BASED_CMSNG|
																										GP_FEATURE_ZGPS_ZGPD_SEC_LVL_0B00 |
																										GP_FEATURE_ZGPS_ZGPD_SEC_LVL_0B10 |
																										GP_FEATURE_ZGPS_ZGPD_SEC_LVL_0B11 |
																										GP_FEATURE_ZGPS_TRANSLATION_TABLE ;
#ifdef GP_IEEE_ADDR_SUPPORT
				((tsCLD_GreenPower *)(psClusterInstance->pvEndPointSharedStructPtr))->b24ZgpsFunctionality |= GP_FEATURE_ZGPS_ZGPD_IEEE_ADDR;
#endif

#ifdef  CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY_TYPE
				((tsCLD_GreenPower *)(psClusterInstance->pvEndPointSharedStructPtr))->b8ZgpSharedSecKeyType = E_GP_NO_KEY;
#endif

#ifdef CLD_GP_ATTR_ZGP_LINK_KEY
				memcpy(((tsCLD_GreenPower *)(psClusterInstance->pvEndPointSharedStructPtr))->sZgpLinkKey.au8Key,"ZigBeeAlliance09",16);
#endif
#endif


#ifdef GP_PROXY_BASIC_DEVICE
#ifdef  CLD_GP_ATTR_ZGPP_NOTIFICATION_RETRY_NUMBER
			((tsCLD_GreenPower *)(psClusterInstance->pvEndPointSharedStructPtr))->u8ZgppNotificationRetryNumber = 2;
#endif

#ifdef  CLD_GP_ATTR_ZGPP_NOTIFICATION_RETRY_TIMER
			((tsCLD_GreenPower *)(psClusterInstance->pvEndPointSharedStructPtr))->u8ZgppNotificationRetryTimer = 0x64;
#endif

#ifdef  CLD_GP_ATTR_ZGPP_MAX_SEARCH_COUNTER
			((tsCLD_GreenPower *)(psClusterInstance->pvEndPointSharedStructPtr))->u8ZgppMaxSearchCounter = 0x0A;
#endif

#ifdef  CLD_GP_ATTR_ZGPP_BLOCKED_GPD_ID
			((tsCLD_GreenPower *)(psClusterInstance->pvEndPointSharedStructPtr))->sZgppBlockedGpdID.u16Length = 0;
#endif

			((tsCLD_GreenPower *)(psClusterInstance->pvEndPointSharedStructPtr))->b24ZgppFunctionality = GP_FEATURE_ZGPP_FEATURE |
																										GP_FEATURE_ZGPP_DIRECT_COMM |
																										GP_FEATURE_ZGPP_DERIVED_GC_COMM |
																										GP_FEATURE_ZGPP_COMMISSION_GC_COMM |
																										GP_FEATURE_ZGPP_LIGHTWEIGHT_UNICAST_COMM|
																										GP_FEATURE_ZGPP_GP_COMMISSIONING |
																										GP_FEATURE_ZGPP_CT_BASED_CMSNG |
																										 GP_FEATURE_ZGPP_ZGPD_SEC_LVL_0B00 |
																										 GP_FEATURE_ZGPP_ZGPD_SEC_LVL_0B10 |
																										 GP_FEATURE_ZGPP_ZGPD_SEC_LVL_0B11;
#ifdef GP_IEEE_ADDR_SUPPORT
			((tsCLD_GreenPower *)(psClusterInstance->pvEndPointSharedStructPtr))->b24ZgppFunctionality |= GP_FEATURE_ZGPP_ZGPD_IEEE_ADDR;
#endif
			((tsCLD_GreenPower *)(psClusterInstance->pvEndPointSharedStructPtr))->b24ZgppActiveFunctionality = GP_ZGP_ACTIVE_FEATURES_ATTR_DEFAULT_VALUE;
#ifdef CLD_GP_ATTR_ZGP_LINK_KEY
				memcpy(((tsCLD_GreenPower *)(psClusterInstance->pvEndPointSharedStructPtr))->sZgpLinkKey.au8Key,"ZigBeeAlliance09",16);
#endif
#endif
			}
			// release mutex
			#ifndef COOPERATIVE
				eZCL_ReleaseMutex(psEndPointDefinition);
			#endif

			return;
        }
	}
}
/****************************************************************************
 **
 ** NAME:       bGP_CheckFrameToBeProcessed
 **
 ** DESCRIPTION:
 **
 **
 ** PARAMETERS:                    Name                           Usage
 ** ZPS_tsAfZgpSecReq            *psSec
 **
 ** RETURN:
 ** None
 ****************************************************************************/
PUBLIC ZPS_teAfSecActions bGP_CheckFrameToBeProcessed(void)
{
    tsZCL_EndPointDefinition        *psEndPointDefinition;
    tsZCL_ClusterInstance           *psClusterInstance;
    tsGP_GreenPowerCustomData       *psGpCustomDataStructure;
    uint8                           u8NumberOfendpoints, i;
    bool_t                            bIsGpClusterFound = FALSE;
    bool_t                            bIsServer;
    teZCL_Status                    eStatus;
#ifdef GP_COMBO_BASIC_DEVICE
    bIsServer= TRUE;
#else
    bIsServer= 	FALSE;
#endif
    u8NumberOfendpoints = u8ZCL_GetNumberOfEndpointsRegistered();
//@tbd check for different end points
    // find GP clusters on each EP - if any
    for(i=0; i<u8NumberOfendpoints; i++)
    {
       /* As local GP end point is not known find GP end point */
        if((eStatus = eGP_FindGpCluster(
                                u8ZCL_GetEPIdFromIndex(i),
                                bIsServer,
                                &psEndPointDefinition,
                                &psClusterInstance,
                                &psGpCustomDataStructure)) == E_ZCL_SUCCESS)
        {
            bIsGpClusterFound = TRUE;
            break;
        }
    }

    if(!bIsGpClusterFound)
    {
        return ZPS_E_DROP_FRAME;
    }

	if((psGpCustomDataStructure->eGreenPowerDeviceMode == E_GP_OPERATING_MODE))
	{
		return ZPS_E_DROP_FRAME;
	}
	else
	{
		return ZPS_E_PASS_UNPROCESSED;
	}

}
/****************************************************************************
 **
 ** NAME:       eGP_AddDeviceSecurity
 **
 ** DESCRIPTION:
 ** Restore persisted attribute data during initialisation
 **
 ** PARAMETERS:                    Name                           Usage
 ** ZPS_tsAfZgpSecReq            *psSec
 **
 ** RETURN:
 ** None
 ****************************************************************************/
PUBLIC ZPS_teAfSecActions eGP_AddDeviceSecurity(ZPS_tsAfZgpSecReq *psSec)
{
    tsZCL_EndPointDefinition        *psEndPointDefinition;
    tsZCL_ClusterInstance           *psClusterInstance;
    tsGP_GreenPowerCustomData       *psGpCustomDataStructure;
    uint8                           u8NumberOfendpoints, i;
    uint8                           u8SecLevel;
    bool_t                            bIsGpClusterFound = FALSE;
    bool_t                            bIsServer;
    teZCL_Status                    eStatus;
    teGP_GreenPowerSecKeyType       eZgpSecKeyType = E_GP_NO_KEY;
    ZPS_tsAfZgpGpstEntry            sZgpGpstEntry;
    tuGP_ZgpdDeviceAddr             uZgpdDeviceAddr;
    bool_t                            bIsTableEntryPresent = FALSE;
    tsGP_ZgppProxySinkTable         *psZgppProxySinkTable;
    AESSW_Block_u                   uSecurityKey;
#ifdef GP_COMBO_BASIC_DEVICE
    bIsServer= TRUE;
#else
    bIsServer= 	FALSE;
#endif
    u8NumberOfendpoints = u8ZCL_GetNumberOfEndpointsRegistered();
//@tbd check for different end points
    // find GP clusters on each EP - if any
    for(i=0; i<u8NumberOfendpoints; i++)
    {
       /* As local GP end point is not known find GP end point */
        if((eStatus = eGP_FindGpCluster(
                                u8ZCL_GetEPIdFromIndex(i),
                                bIsServer,
                                &psEndPointDefinition,
                                &psClusterInstance,
                                &psGpCustomDataStructure)) == E_ZCL_SUCCESS)
        {
            bIsGpClusterFound = TRUE;
            break;
        }
    }

    if(!bIsGpClusterFound)
    {
        return ZPS_E_DROP_FRAME;
    }

    if( psSec->u8ApplicationId == GP_APPL_ID_4_BYTE)
    {
    	uZgpdDeviceAddr.u32ZgpdSrcId = psSec->uGpId.u32SrcId;
    }
#ifdef GP_IEEE_ADDR_SUPPORT
    else
    {
    	uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr = psSec->uGpId.u64Address;
    	uZgpdDeviceAddr.sZgpdDeviceAddrAppId2.u8EndPoint = psSec->u8Endpoint; // do not care for endpoint. May have to revisit this
    }
#endif
    bIsTableEntryPresent =  bGP_IsProxyTableEntryPresent(
   	     		psEndPointDefinition->u8EndPointNumber,
   	     	    bIsServer,
   	     		(uint8)psSec->u8ApplicationId,
   	     		&uZgpdDeviceAddr,
   	     		&psZgppProxySinkTable);


	if(bIsTableEntryPresent&&(psZgppProxySinkTable->b16Options & GP_PROXY_TABLE_SECURITY_USE_MASK) && (psSec->u8KeyType))
	{
		u8SecLevel = psZgppProxySinkTable->b8SecOptions & GP_SECURITY_LEVEL_MASK;
		if(u8SecLevel != psSec->u8SecurityLevel)
		{
			if((psGpCustomDataStructure->eGreenPowerDeviceMode == E_GP_OPERATING_MODE))
			{
				return ZPS_E_DROP_FRAME;
			}
			else
			{
				return ZPS_E_PASS_UNPROCESSED;
			}
		}
		eZgpSecKeyType =( psZgppProxySinkTable->b8SecOptions & GP_SECURITY_KEY_TYPE_MASK)>> 2;
	}
	else
	{
		bIsTableEntryPresent = FALSE;
	}



    /* populate key */
    if((!bIsTableEntryPresent) && (psSec->u8KeyType == 0))
    {
#ifdef CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY_TYPE
        // get EP mutex
        #ifndef COOPERATIVE
            eZCL_GetMutex(psEndPointDefinition);
        #endif

        /* Get Green Power communication Mode attribute value */
        eZCL_ReadLocalAttributeValue(
                  psEndPointDefinition->u8EndPointNumber,
                  GREENPOWER_CLUSTER_ID,
                  bIsServer,
                  FALSE,
                  (!bIsServer),
                  E_CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY_TYPE,
                  (uint8*)&eZgpSecKeyType);

        // release mutex
        #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


#else
       // fill in callback event structure
        eZCL_SetCustomCallBackEvent(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent,
                                    0, 0, psEndPointDefinition->u8EndPointNumber);
        /* Set Event Type */
        psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_SHARED_SECURITY_KEY_TYPE_IS_NOT_ENABLED;
        /* Give a call back event to app */
        psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);
        return ZPS_E_DROP_FRAME;
#endif
    }
    else if((bIsTableEntryPresent == FALSE ) && (psSec->u8KeyType == 1))
    {
    	return ZPS_E_PASS_UNPROCESSED;
    }

    if(bGP_GetGPDKey(
    		(uint8)psSec->u8ApplicationId,
    		bIsTableEntryPresent,
    		&uZgpdDeviceAddr,
    		psZgppProxySinkTable,
    	    eZgpSecKeyType,
    	    &uSecurityKey,
    	    psEndPointDefinition,
    	    psGpCustomDataStructure) == FALSE)
		{
    		return ZPS_E_DROP_FRAME;
		}


    sZgpGpstEntry.uSecurityKey = uSecurityKey;
    sZgpGpstEntry.uGpId = psSec->uGpId;
    sZgpGpstEntry.u32Counter = psSec->u32FrameCounter;
    sZgpGpstEntry.u8SecurityLevel = psSec->u8SecurityLevel;
    sZgpGpstEntry.u8KeyType = (uint8)eZgpSecKeyType;
    sZgpGpstEntry.bValid = TRUE;

   if( ZPS_sZgpAddDeviceSecurity((uint32)psSec->u8ApplicationId, &sZgpGpstEntry))
   {

	   return ZPS_E_MATCH;
   }
   else
   {
	   return ZPS_E_DROP_FRAME;
   }
}

/****************************************************************************
 **
 ** NAME:       bGP_GetGPDKey
 **
 ** DESCRIPTION:
 ** Gets key
 **
 ** PARAMETERS:                    Name                           Usage
 ** uint8                                u8AppID,
 ** bool_t                                 bIsTableEntryPresent,
 ** tuGP_ZgpdDeviceAddr                  *puZgpdDeviceAddr,
 ** tsGP_ZgppProxySinkTable              *psZgppProxySinkTable,
 ** teGP_GreenPowerSecKeyType            eZgpSecKeyType,
 ** AESSW_Block_u                        *puSecurityKey,
 ** tsZCL_EndPointDefinition             *psEndPointDefinition,
 ** tsGP_GreenPowerCustomData            *psGpCustomDataStructure
 **
 ** RETURN:
 ** None
 ****************************************************************************/
bool_t  bGP_GetGPDKey(
		uint8                                u8AppID,
		bool_t                                 bIsTableEntryPresent,
		tuGP_ZgpdDeviceAddr                  *puZgpdDeviceAddr,
		tsGP_ZgppProxySinkTable              *psZgppProxySinkTable,
		teGP_GreenPowerSecKeyType            eZgpSecKeyType,
		AESSW_Block_u                        *puSecurityKey,
	    tsZCL_EndPointDefinition             *psEndPointDefinition,
	    tsGP_GreenPowerCustomData            *psGpCustomDataStructure)
{
	bool_t bIsServer = TRUE;

#ifdef GP_PROXY_BASIC_DEVICE
	bIsServer = FALSE;
#endif

#ifdef CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY
    AESSW_Block_u                   uInKey;
#endif

	if((eZgpSecKeyType == E_GP_ZGPD_GROUP_KEY)||(eZgpSecKeyType == E_GP_NWK_KEY_DERIVED_ZGPD_GROUP_KEY))
	{
#ifdef CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY
	/* Get shared key value */
	eZCL_ReadLocalAttributeValue(
			  psEndPointDefinition->u8EndPointNumber,
			  GREENPOWER_CLUSTER_ID,
			  bIsServer,
			  FALSE,
			  (!bIsServer),
			  E_CLD_GP_ATTR_ZGP_SHARED_SECURITY_KEY,
			  puSecurityKey);


	if(eZgpSecKeyType == E_GP_NWK_KEY_DERIVED_ZGPD_GROUP_KEY)
	{
		memcpy(uInKey.au8, puSecurityKey, E_ZCL_KEY_128_SIZE);

		/* Get the derived individual key */
		ZPS_vZgpTransformKey(ZPS_E_ZGP_NWK_KEY_DERIVED_GRP_KEY,
				u8AppID,
				puZgpdDeviceAddr->u32ZgpdSrcId,
			    puZgpdDeviceAddr->sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr,
				&uInKey,
				puSecurityKey);
	}
#else
	// fill in callback event structure
	eZCL_SetCustomCallBackEvent(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent,
								  0, 0, psEndPointDefinition->u8EndPointNumber);
	 /* Set Event Type */
	psGpCustomDataStructure->sGPCommon.sGreenPowerCallBackMessage.eEventType = E_GP_SHARED_SECURITY_KEY_IS_NOT_ENABLED;
	 /* Give a call back event to app */
	psEndPointDefinition->pCallBackFunctions(&psGpCustomDataStructure->sGPCommon.sGPCustomCallBackEvent);
	return FALSE;
#endif
	}
	/*else if(eZgpSecKeyType == E_GP_DERIVED_INDIVIDUAL_ZGPD_KEY)
	{

		ZPS_vZgpTransformKey(ZPS_E_ZGP_DERIVED_INDIVIDUAL_KEY,
				u8AppID,
				puZgpdDeviceAddr->u32ZgpdSrcId,
				puZgpdDeviceAddr->sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr,
				NULL,
				puSecurityKey);
	}*/
	else if(eZgpSecKeyType == E_GP_ZIGBEE_NWK_KEY)
	{
		ZPS_vZgpTransformKey(ZPS_E_ZGP_ZIGBEE_NWK_KEY,
				u8AppID,
				puZgpdDeviceAddr->u32ZgpdSrcId,
				puZgpdDeviceAddr->sZgpdDeviceAddrAppId2.u64ZgpdIEEEAddr,
				NULL,
				puSecurityKey);
	}
	else if(((eZgpSecKeyType == E_GP_OUT_OF_THE_BOX_ZGPD_KEY) || (eZgpSecKeyType == E_GP_DERIVED_INDIVIDUAL_ZGPD_KEY))&&bIsTableEntryPresent)
	{
		memcpy(puSecurityKey->au8, psZgppProxySinkTable->sZgpdKey.au8Key, E_ZCL_KEY_128_SIZE);
		 DBG_vPrintf(TRACE_GP_DEBUG, " psZgppProxySinkTable->sZgpdKey.au8Key= 0x%x 0x5x 0x%x 0x%x\n ",
				 psZgppProxySinkTable->sZgpdKey.au8Key[0],  psZgppProxySinkTable->sZgpdKey.au8Key[1],  psZgppProxySinkTable->sZgpdKey.au8Key[14], psZgppProxySinkTable->sZgpdKey.au8Key[15]);
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
