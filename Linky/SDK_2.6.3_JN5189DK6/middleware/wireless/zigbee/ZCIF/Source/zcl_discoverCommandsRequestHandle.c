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
 * COMPONENT:          ZCL
 *
 * DESCRIPTION:       Attribute Discovery
 *
 * $ $
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_common.h"
#include "zcl_internal.h"

#include "pdum_apl.h"
#include "zps_apl.h"
#include "zps_apl_af.h"

#ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE teZCL_Status eZCL_SearchForClosestCommandEntry(
                        uint8                       u8CommandEnum,
                        bool_t                      bManufacturerSpecific,
                        tsZCL_ClusterInstance      *psClusterInstance,
                        uint8                      *pu8FirstCommandId,
                        uint8                      *pu8CommandIndex,
                        uint8                       u8Command,
                        bool_t                      bDirection);

PRIVATE  void vZCL_HandleCommandsDiscoverRequest(
                         ZPS_tsAfEvent              *pZPSevent,
                         tsZCL_EndPointDefinition   *psZCL_EndPointDefinition,
                         tsZCL_ClusterInstance      *psClusterInstance,
                         uint8 u8Command);
                         
PRIVATE bool_t bValidateCommandWithCommandsFlags(uint8 u8Flag,uint8 u8Command,bool_t bDirection);                         
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Public Functions                                              ***/
/****************************************************************************/
/****************************************************************************
 **
 ** NAME:       vZCL_HandleDiscoverCommandsReceivedRequest
 **
 ** DESCRIPTION:
 ** Handles an attribute discovery request
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee Stack Event
 ** tsZCL_EndPointDefinition *psZCL_EndPointDefinition  EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** None
 **
 ****************************************************************************/

PUBLIC  void vZCL_HandleDiscoverCommandsReceivedRequest(
                                             ZPS_tsAfEvent              *pZPSevent,
                                             tsZCL_EndPointDefinition   *psZCL_EndPointDefinition,
                                             tsZCL_ClusterInstance      *psClusterInstance)
{
    vZCL_HandleCommandsDiscoverRequest(
                                        pZPSevent,
                                        psZCL_EndPointDefinition,
                                        psClusterInstance,
                                        E_ZCL_DISCOVER_COMMANDS_RECEIVED_RESPONSE);
}
/****************************************************************************
 **
 ** NAME:       vZCL_HandleDiscoverCommandsGeneratedRequest
 **
 ** DESCRIPTION:
 ** Handles an attribute discovery request
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee Stack Event
 ** tsZCL_EndPointDefinition *psZCL_EndPointDefinition  EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** None
 **
 ****************************************************************************/

PUBLIC  void vZCL_HandleDiscoverCommandsGeneratedRequest(
                                             ZPS_tsAfEvent              *pZPSevent,
                                             tsZCL_EndPointDefinition   *psZCL_EndPointDefinition,
                                             tsZCL_ClusterInstance      *psClusterInstance)
{
    vZCL_HandleCommandsDiscoverRequest(
                                        pZPSevent,
                                        psZCL_EndPointDefinition,
                                        psClusterInstance,
                                        E_ZCL_DISCOVER_COMMANDS_GENERATED_RESPONSE);
}

/****************************************************************************
 **
 ** NAME:       bValidateCommandWithCommandsFlags
 **
 ** DESCRIPTION:
 ** Validates the Command with the comand flags 
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                      u8Flag                       Comand Flag in Cluster Definition
 ** uint8                      u8Command                    Comand 
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE bool_t bValidateCommandWithCommandsFlags(uint8 u8Flag,uint8 u8Command,bool_t bDirection)
{
    /*client has asked server responding  */
    if(FALSE == bDirection)
    {
        if(
           (
               ( E_ZCL_CF_RX ==  (u8Flag & E_ZCL_CF_RX )) &&
               (E_ZCL_DISCOVER_COMMANDS_RECEIVED_RESPONSE == u8Command  ) 
           ) 
            ||
           ( 
               ( E_ZCL_CF_TX == (u8Flag & E_ZCL_CF_TX )) &&
               ( E_ZCL_DISCOVER_COMMANDS_GENERATED_RESPONSE == u8Command  ) 
           ) 
          )
       
        {
            return TRUE;
        }
    }
     /*Server has asked , client responding */
    if(TRUE == bDirection)
    {
        if(
           (
               ( E_ZCL_CF_TX ==  (u8Flag & E_ZCL_CF_TX )) &&
               (E_ZCL_DISCOVER_COMMANDS_RECEIVED_RESPONSE == u8Command  ) 
           ) 
            ||
           ( 
               ( E_ZCL_CF_RX == (u8Flag & E_ZCL_CF_RX )) &&
               ( E_ZCL_DISCOVER_COMMANDS_GENERATED_RESPONSE == u8Command  ) 
           ) 
          )
       
        {
            return TRUE;
        }
    }    
    return FALSE;
}
/****************************************************************************
 **
 ** NAME:       vZCL_HandleCommandsDiscoverRequest
 **
 ** DESCRIPTION:
 ** Handles an attribute discovery request
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee Stack Event
 ** tsZCL_EndPointDefinition *psZCL_EndPointDefinition  EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** None
 **
 ****************************************************************************/

PRIVATE  void vZCL_HandleCommandsDiscoverRequest(
                                             ZPS_tsAfEvent              *pZPSevent,
                                             tsZCL_EndPointDefinition   *psZCL_EndPointDefinition,
                                             tsZCL_ClusterInstance      *psClusterInstance,
                                             uint8 u8Command)
{
    uint16 i, j;

    uint16 u16inputOffset, u16outputOffset, u16BufferOverflowFlagOffset, u16Index = 0;

	tsZCL_HeaderParams sZCL_HeaderParams;

    bool_t bDiscoveryComplete;
    uint8 u8CommandId,u8TempCommandId;
    uint8 u8FirstCommandId;
    

    tsZCL_Address sZCL_Address;

    uint8 u8MaximumNumberOfIdentifiers;
    uint16 u16payloadSize;
    uint16 u16responseBufferSize;


    PDUM_thAPduInstance myPDUM_thAPduInstance;

    uint8 u8FirstCommandIndex=0;
    bool_t bFoundCommand;

    // build command packet for response
    // read incoming request
    u16inputOffset = u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                                              &sZCL_HeaderParams);

    /* Manufacture Specefic with Wild card */
    if ( sZCL_HeaderParams.bManufacturerSpecific && (0xFFFF == sZCL_HeaderParams.u16ManufacturerCode) )
    {
    	sZCL_HeaderParams.u16ManufacturerCode= psZCL_EndPointDefinition->u16ManufacturerCode;
    }
    // payload investigation
    u16payloadSize = PDUM_u16APduInstanceGetPayloadSize(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst);
    // size of outgoing buffer
    u16responseBufferSize = u16ZCL_GetTxPayloadSize(pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr);

    // size sanity checks
    if((u16payloadSize < u16inputOffset) || ((u16payloadSize - u16inputOffset)!=2))
    {
        // send response if possible
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_MALFORMED_COMMAND);
        return;
    }

    // get buffer to write the response in
    myPDUM_thAPduInstance = hZCL_AllocateAPduInstance();
    // no buffers - return. we can make this descision a bit more exotic using the incoming buffer to store the
    // outgoing command but for the moment we are implementing the KISS principle.
    if(myPDUM_thAPduInstance == PDUM_INVALID_HANDLE)
    {
        return;
    }

    if(u16responseBufferSize < (u16inputOffset+2))
    {
        // send response if possible
        eZCL_SendDefaultResponse(pZPSevent, E_ZCL_CMDS_SOFTWARE_FAILURE);
        // free buffer and return
        PDUM_eAPduFreeAPduInstance(myPDUM_thAPduInstance);
        return;
    }
    
    // modify and write back
    u16outputOffset = u16ZCL_WriteCommandHeader(myPDUM_thAPduInstance,
											sZCL_HeaderParams.eFrameType,
											sZCL_HeaderParams.bManufacturerSpecific,
											sZCL_HeaderParams.u16ManufacturerCode,
                                           !sZCL_HeaderParams.bDirection,
                                           psZCL_EndPointDefinition->bDisableDefaultResponse,
                                           sZCL_HeaderParams.u8TransactionSequenceNumber,
                                           u8Command);

    // read input attribute Id
    u16inputOffset += u16ZCL_APduInstanceReadNBO(
                        pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16inputOffset, E_ZCL_UINT8, &u8CommandId);
    u16inputOffset += u16ZCL_APduInstanceReadNBO(
                        pZPSevent->uEvent.sApsDataIndEvent.hAPduInst, u16inputOffset, E_ZCL_UINT8, &u8MaximumNumberOfIdentifiers);

    u16BufferOverflowFlagOffset = u16outputOffset;
    // parse the cluster, read each attribute from the device and write into the outgoing buffer
    i=0;
    j=0;
    // step over discovery complete flag slot
    u16outputOffset++;

    bFoundCommand = FALSE;
    if(eZCL_SearchForClosestCommandEntry(u8CommandId, sZCL_HeaderParams.bManufacturerSpecific, psClusterInstance, &u8FirstCommandId, &u8FirstCommandIndex,u8Command,sZCL_HeaderParams.bDirection) == E_ZCL_SUCCESS)
    {
        bFoundCommand = TRUE;
        j = u8FirstCommandIndex;
        u16Index=u8FirstCommandIndex;

        u8TempCommandId = u8FirstCommandId;

        while((j < psClusterInstance->psClusterDefinition->u8NumberOfCommands) &&
              (i < u8MaximumNumberOfIdentifiers)                               &&
              (u16responseBufferSize >= (u16outputOffset +2)) )
        {
            // increment loop count
            j++;
            if( TRUE == bValidateCommandWithCommandsFlags(psClusterInstance->psClusterDefinition->psCommandDefinition[u16Index].u8CommandFlags,u8Command,sZCL_HeaderParams.bDirection))              
            {
                if(bZCL_CheckManufacturerSpecificCommandFlagMatch(&psClusterInstance->psClusterDefinition->psCommandDefinition[u16Index], sZCL_HeaderParams.bManufacturerSpecific))
                {
                    // write Attribute Id and type into outgoing buffer
                    u16outputOffset += u16ZCL_APduInstanceWriteNBO(myPDUM_thAPduInstance, u16outputOffset, E_ZCL_UINT8,&u8TempCommandId);
                    i++;
                }
            }
            u16Index++;
            u8TempCommandId = psClusterInstance->psClusterDefinition->psCommandDefinition[u16Index].u8CommandEnum;                
        }
    }

    // we need to check if there are any valid attributes left to discover, this must take into account the MS flag
    bDiscoveryComplete = TRUE;
    while(bFoundCommand && (j < psClusterInstance->psClusterDefinition->u8NumberOfCommands) && (bDiscoveryComplete != FALSE))
    {
        // increment loop count
        j++;
        if( TRUE == bValidateCommandWithCommandsFlags(psClusterInstance->psClusterDefinition->psCommandDefinition[u16Index].u8CommandFlags,u8Command,sZCL_HeaderParams.bDirection))              
        {
            if(bZCL_CheckManufacturerSpecificCommandFlagMatch(&psClusterInstance->psClusterDefinition->psCommandDefinition[u16Index], sZCL_HeaderParams.bManufacturerSpecific))
            {
                bDiscoveryComplete = FALSE;
            }        
        }
        u16Index++;
    }

    // write discovery result in first byte
    u16ZCL_APduInstanceWriteNBO(myPDUM_thAPduInstance, u16BufferOverflowFlagOffset, E_ZCL_BOOL, &bDiscoveryComplete);

    // build address structure
    eZCL_BuildTransmitAddressStructure(pZPSevent, &sZCL_Address);
    // transmit request
    eZCL_TransmitDataRequest(myPDUM_thAPduInstance,
                                u16outputOffset,
                                pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                pZPSevent->uEvent.sApsDataIndEvent.u16ClusterId,
                                &sZCL_Address);
}

/****************************************************************************
 **
 ** NAME:       eZCL_SearchForClosestCommandEntry
 **
 ** DESCRIPTION:
 ** Searches For An Attribute Definition In The Endpoint
 ** assumes EP has already been validiated
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint16                      u16AttributeEnum            Attribute Id
 ** bool_t                      bIsManufacturerSpecific     Attribute manufacturer specific
 ** tsZCL_ClusterInstance      *psClusterInstance           Cluster Structure
 ** tsZCL_AttributeDefinition **ppsAttributeDefinition      Attribute Structure
 ** uint16                     *pu16attributeIndex          Index of attribute definition
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PRIVATE teZCL_Status eZCL_SearchForClosestCommandEntry(
                        uint8                       u8CommandEnum,
                        bool_t                      bManufacturerSpecific,
                        tsZCL_ClusterInstance      *psClusterInstance,
                        uint8                      *pu8FirstCommandId,
                        uint8                      *pu8CommandIndex,
                        uint8                       u8Command,
                        bool_t                      bDirection)
{

    tsZCL_CommandDefinition  *psCommandDefinition;
    uint8 u8TempCommandId;
    if (psClusterInstance == NULL)
    {
        return(E_ZCL_ERR_CLUSTER_NOT_FOUND);
    }

    psCommandDefinition = psClusterInstance->psClusterDefinition->psCommandDefinition;
    u8TempCommandId = psCommandDefinition->u8CommandEnum;
    // not found
    for(*pu8CommandIndex=0; *pu8CommandIndex < psClusterInstance->psClusterDefinition->u8NumberOfCommands; (*pu8CommandIndex)++)
    {
        if((u8TempCommandId >= u8CommandEnum) && bZCL_CheckManufacturerSpecificCommandFlagMatch(psCommandDefinition, bManufacturerSpecific))
        {
            *pu8FirstCommandId = u8TempCommandId;
            return(E_ZCL_SUCCESS);
        }
        psCommandDefinition++;
        if( TRUE == bValidateCommandWithCommandsFlags(psCommandDefinition->u8CommandFlags,u8Command,bDirection))                        
        {
            u8TempCommandId = psCommandDefinition->u8CommandEnum;
        }
        
    }
    return(E_ZCL_ERR_CLUSTER_COMMAND_NOT_FOUND);
}
#endif
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
