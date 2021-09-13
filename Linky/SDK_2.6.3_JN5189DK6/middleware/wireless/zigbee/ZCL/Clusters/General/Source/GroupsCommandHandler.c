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
 * MODULE:             Groups Cluster
 *
 * DESCRIPTION:
 * Message event handler functions
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>
//#include "zps_apl_aib.h"
#include "dlist.h"
#include "zcl.h"
#include "zcl_customcommand.h"

#include "Groups.h"
#include "Groups_internal.h"

#include "zcl_options.h"

#include "dbg.h"



#ifdef DEBUG_CLD_GROUPS
#define TRACE_GROUPS    TRUE
#else
#define TRACE_GROUPS    FALSE
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#if defined(CLD_GROUPS) && !defined(GROUPS_SERVER) && !defined(GROUPS_CLIENT)
#error You Must Have either GROUPS_SERVER and/or GROUPS_CLIENT defined zcl_options.h
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
#ifdef GROUPS_CLIENT
PRIVATE teZCL_Status eCLD_GroupsHandleAddGroupResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem 		*pasPayloadDefinition);

PRIVATE teZCL_Status eCLD_GroupsHandleViewGroupResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem 		*pasPayloadDefinition);

PRIVATE teZCL_Status eCLD_GroupsHandleGetGroupMembershipResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem 		*pasPayloadDefinition);

PRIVATE teZCL_Status eCLD_GroupsHandleRemoveGroupResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem 		*pasPayloadDefinition);
#endif

#ifdef GROUPS_SERVER
PRIVATE teZCL_Status eCLD_GroupsHandleAddGroupRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem 		 *pasPayloadDefinition);
#endif

#ifdef GROUPS_SERVER
PRIVATE teZCL_Status eCLD_GroupsHandleViewGroupRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem 		 *pasPayloadDefinition
                            );
#endif

#ifdef GROUPS_SERVER
PRIVATE teZCL_Status eCLD_GroupsHandleGetGroupMembershipRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem 		 *pasPayloadDefinition);
#endif

#ifdef GROUPS_SERVER
PRIVATE teZCL_Status eCLD_GroupsHandleRemoveGroupRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem 		*pasPayloadDefinition);
#endif

#ifdef GROUPS_SERVER
PRIVATE teZCL_Status eCLD_GroupsHandleRemoveAllGroupsRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance);
#endif



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
 ** NAME:       eCLD_GroupsCommandHandler
 **
 ** DESCRIPTION:
 ** Handles Message Cluster custom commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_GroupsCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{

	tsZCL_HeaderParams sZCL_HeaderParams;
    tsCLD_GroupsCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_GroupsCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Store return address details */
    eZCL_SetReceiveEventAddressStructure(pZPSevent, &psCommon->sReceiveEventAddress);

    // further error checking can only be done once we have interrogated the ZCL payload
    u16ZCL_ReadCommandHeader(pZPSevent->uEvent.sApsDataIndEvent.hAPduInst,
                             &sZCL_HeaderParams);


    // get EP mutex
    #ifndef COOPERATIVE
        eZCL_GetMutex(psEndPointDefinition);
    #endif


    // fill in callback event structure
    eZCL_SetCustomCallBackEvent(&psCommon->sCustomCallBackEvent, pZPSevent, sZCL_HeaderParams.u8TransactionSequenceNumber, psEndPointDefinition->u8EndPointNumber);

    /* Fill in message */
    psCommon->sCallBackMessage.u8CommandId = sZCL_HeaderParams.u8CommandIdentifier;

#ifdef GROUPS_SERVER
    /* Handle messages appropriate for the cluster type (Client/Server) */
    if(psClusterInstance->bIsServer == TRUE)
    {
    	uint16 u16ActualQuantity;
    	  tsZCL_RxPayloadItem asPayloadDefinition[] = {
    	    {1, &u16ActualQuantity, E_ZCL_UINT16,   0},
    	    {1, &u16ActualQuantity, E_ZCL_CSTRING,  0},
    	                                                };

        // SERVER
        switch(sZCL_HeaderParams.u8CommandIdentifier)
        {

            case (E_CLD_GROUPS_CMD_ADD_GROUP):
            {
                eCLD_GroupsHandleAddGroupRequest(pZPSevent, psEndPointDefinition, psClusterInstance, asPayloadDefinition);
                break;
            }

            case (E_CLD_GROUPS_CMD_VIEW_GROUP):
            {
                eCLD_GroupsHandleViewGroupRequest(pZPSevent, psEndPointDefinition, psClusterInstance, asPayloadDefinition);
                break;
            }

            case (E_CLD_GROUPS_CMD_GET_GROUP_MEMBERSHIP):
            {
            	asPayloadDefinition[0].eType =E_ZCL_UINT8;
            	asPayloadDefinition[1].eType =E_ZCL_UINT16;
                eCLD_GroupsHandleGetGroupMembershipRequest(pZPSevent, psEndPointDefinition, psClusterInstance, asPayloadDefinition);
                break;
            }

            case (E_CLD_GROUPS_CMD_REMOVE_GROUP):
            {
                eCLD_GroupsHandleRemoveGroupRequest(pZPSevent, psEndPointDefinition, psClusterInstance, asPayloadDefinition);
                break;
            }

            case (E_CLD_GROUPS_CMD_REMOVE_ALL_GROUPS):
            {
                eCLD_GroupsHandleRemoveAllGroupsRequest(pZPSevent, psEndPointDefinition, psClusterInstance);
                break;
            }

            case (E_CLD_GROUPS_CMD_ADD_GROUP_IF_IDENTIFYING):
            {
            	eCLD_GroupsHandleAddGroupRequest(pZPSevent, psEndPointDefinition, psClusterInstance, asPayloadDefinition);
                break;
            }

            default:
            {
                // unlock
                #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

                return(E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR);
                break;
            }
        }

    }
#endif

#ifdef GROUPS_CLIENT

    /* Handle messages appropriate for the cluster type (Client/Server) */
    if(psClusterInstance->bIsServer == FALSE)
    {
    	uint16 u16ActualQuantity;
    	tsZCL_RxPayloadItem asPayloadDefinition[] = {
    	            {1,   &u16ActualQuantity,    E_ZCL_ENUM8,    0},
    	            {1,   &u16ActualQuantity,    E_ZCL_UINT16,   0},
    	            {1,   &u16ActualQuantity,     E_ZCL_CSTRING,  0},
    	};
        // CLIENT
        switch(sZCL_HeaderParams.u8CommandIdentifier)
        {

            case (E_CLD_GROUPS_CMD_ADD_GROUP):
            {
                eCLD_GroupsHandleAddGroupResponse(pZPSevent, psEndPointDefinition, psClusterInstance, asPayloadDefinition);
                break;
            }

            case (E_CLD_GROUPS_CMD_VIEW_GROUP):
            {
                eCLD_GroupsHandleViewGroupResponse(pZPSevent, psEndPointDefinition, psClusterInstance, asPayloadDefinition);
                break;
            }

            case (E_CLD_GROUPS_CMD_GET_GROUP_MEMBERSHIP):
            {
                eCLD_GroupsHandleGetGroupMembershipResponse(pZPSevent, psEndPointDefinition, psClusterInstance, asPayloadDefinition);
                break;
            }

            case (E_CLD_GROUPS_CMD_REMOVE_GROUP):
            {
                eCLD_GroupsHandleRemoveGroupResponse(pZPSevent, psEndPointDefinition, psClusterInstance, asPayloadDefinition);
                break;
            }

            default:
            {
                // unlock
                #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif

                return(E_ZCL_ERR_CUSTOM_COMMAND_HANDLER_NULL_OR_RETURNED_ERROR);
                break;
            }
        }
    }
#endif

    // unlock
    #ifndef COOPERATIVE
        eZCL_ReleaseMutex(psEndPointDefinition);
    #endif


    // delete the i/p buffer on return
    return(E_ZCL_SUCCESS);

}

/****************************************************************************/
/***        Private Functions                                             ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eCLD_GroupsHandleAddGroupRequest
 **
 ** DESCRIPTION:
 ** Handles add group request
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
 #ifdef GROUPS_SERVER
PRIVATE  teZCL_Status eCLD_GroupsHandleAddGroupRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem 		*pasPayloadDefinition
                            )
{

    uint8 u8Status, u8TransactionSequenceNumber;
    uint8 au8GroupName[CLD_GROUPS_MAX_GROUP_NAME_LENGTH];
    tsCLD_Groups_AddGroupRequestPayload sPayload;
    tsCLD_Groups_AddGroupResponsePayload sResponse;
    tsCLD_GroupsCustomDataStructure *psCommon;
    tsZCL_Address sZCL_Address;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_GroupsCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Point to storage for received string */
    sPayload.sGroupName.pu8Data = au8GroupName;
    sPayload.sGroupName.u8MaxLength = sizeof(au8GroupName);

    /* Receive the command */
    pasPayloadDefinition[0].pvDestination = &sPayload.u16GroupId;
    pasPayloadDefinition[1].pvDestination = &sPayload.sGroupName;
    u8Status = eZCL_CustomCommandReceive(pZPSevent,
                                         &u8TransactionSequenceNumber,
                                         pasPayloadDefinition,
                                         2,
                                         E_ZCL_ACCEPT_EXACT|E_ZCL_DISABLE_DEFAULT_RESPONSE);

    if(u8Status != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_GROUPS, "\nErr:eCLD_GroupsCommandAddGroupRequestReceive:%d", u8Status);
        return u8Status;
    }

    DBG_vPrintf(TRACE_GROUPS, "\nAdd Group:%04x ", sPayload.u16GroupId);
#ifdef DEBUG_CLD_GROUPS
    {
        int x;
        for(x = 0; x < sPayload.sGroupName.u8Length; x++)
        {
            DBG_vPrintf(TRACE_GROUPS, "%c", sPayload.sGroupName.pu8Data[x]);
        }
    }
#endif

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psAddGroupRequestPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);
    /* Exit if we're not in identify mode */
    if((psCommon->bIdentifying == FALSE) &&(psCommon->sCallBackMessage.u8CommandId == E_CLD_GROUPS_CMD_ADD_GROUP_IF_IDENTIFYING))
    {
        /* For Unicast AddGroupIfIdentifying, send DefaultResponse-SUCCESS
           to pass docs-15-0306_G-TC-02S step 18a 
           ToDo: Clarify in working group as SUCCESS doesn't seem right */
        if(bZCL_SendCommandResponse( pZPSevent))
        {
            eZCL_SendDefaultResponse(pZPSevent,E_ZCL_SUCCESS);
        }
        return E_ZCL_SUCCESS;
    }

    u8Status = eCLD_GroupsAddGroup(psEndPointDefinition,
                                  psClusterInstance,
                                  &sPayload);
    
    if(psCommon->sCallBackMessage.u8CommandId == E_CLD_GROUPS_CMD_ADD_GROUP_IF_IDENTIFYING)
    {
        /* For Unicast AddGroupIfIdentifying, send DefaultResponse-SUCCESS
           to pass docs-15-0306_G-TC-02S step 18d 
           ToDo: Clarify in working group as SUCCESS doesn't seem right 
           What happens if group wasn't added successfully? GroupsTable full Groups already exist */
        if(bZCL_SendCommandResponse( pZPSevent))
        {
            eZCL_SendDefaultResponse(pZPSevent,E_ZCL_SUCCESS);
        }
        return E_ZCL_SUCCESS;
    }
    
#ifdef DEBUG_CLD_GROUPS
    if(u8Status != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_GROUPS, "\nErr:%d", u8Status);
    }
#endif

    if ( !bZCL_SendCommandResponse( pZPSevent) )
    {
        /* bound, group cast, or broadcast, so don't send a response */
        return E_ZCL_SUCCESS;
    }

    switch(u8Status)
    {

    case E_ZCL_SUCCESS:
        sResponse.eStatus = E_CLD_GROUPS_CMD_STATUS_SUCCESS;
        break;

    case E_CLD_GROUPS_DUPLICATE_EXISTS:
        sResponse.eStatus = E_CLD_GROUPS_CMD_STATUS_DUPLICATE_EXISTS;
        break;

    case E_ZCL_ERR_INSUFFICIENT_SPACE:
        sResponse.eStatus = E_CLD_GROUPS_CMD_STATUS_INSUFFICIENT_SPACE;
        break;
        
    case E_CLD_GROUPS_CMD_STATUS_INVALID_FIELD:
        sResponse.eStatus = E_CLD_GROUPS_CMD_STATUS_INVALID_FIELD;
    break;

    default:
        sResponse.eStatus = E_CLD_GROUPS_CMD_STATUS_FAILURE;
        break;

    }

    // build address structure
    eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);

    sResponse.u16GroupId = sPayload.u16GroupId;
    u8Status = eCLD_GroupsCommandAddGroupResponseSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                     pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                     &sZCL_Address,
                                                     &u8TransactionSequenceNumber,
                                                     &sResponse);
    return u8Status;
}
#endif

/****************************************************************************
 **
 ** NAME:       eCLD_GroupsHandleViewGroupRequest
 **
 ** DESCRIPTION:
 ** Handles view group request
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
#ifdef GROUPS_SERVER
PRIVATE  teZCL_Status eCLD_GroupsHandleViewGroupRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem 		*pasPayloadDefinition)
{

    teZCL_Status eStatus;
    uint8 au8NoGroupName[1] = {0};
    tsCLD_GroupTableEntry *psGroupTableEntry = NULL;
    tsCLD_Groups_ViewGroupRequestPayload sPayload;
    tsCLD_Groups_ViewGroupResponsePayload sResponse;
    uint8   u8TransactionSequenceNumber;
    tsCLD_GroupsCustomDataStructure *psCommon;
    tsZCL_Address sZCL_Address;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_GroupsCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    pasPayloadDefinition[0].pvDestination = &sPayload.u16GroupId;
    eStatus =eZCL_CustomCommandReceive(pZPSevent,
            &u8TransactionSequenceNumber,
            pasPayloadDefinition,
            1,
            E_ZCL_ACCEPT_EXACT|E_ZCL_DISABLE_DEFAULT_RESPONSE);

    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_GROUPS, "\nErr:HandleViewGroupRequestReceive:%d", eStatus);
        return eStatus;
    }

    DBG_vPrintf(TRACE_GROUPS, "\nView Group:%04x", sPayload.u16GroupId);

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psViewGroupRequestPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    /* Initialise group name string */
    sResponse.sGroupName.u8Length = 0;
    sResponse.sGroupName.pu8Data = au8NoGroupName;

    eStatus = eCLD_GroupsFindGroup(psEndPointDefinition,
                                   psClusterInstance,
                                   sPayload.u16GroupId,
                                   &psGroupTableEntry);
    if(eStatus == E_ZCL_SUCCESS)
    {
        sResponse.eStatus = E_CLD_GROUPS_CMD_STATUS_SUCCESS;

        /* If a group exists and has text associated with it */
        if(psGroupTableEntry != NULL)
        {
            DBG_vPrintf(TRACE_GROUPS, "\nString len is:%d %s", strlen((char*)psGroupTableEntry->au8GroupName), psGroupTableEntry->au8GroupName);
            sResponse.sGroupName.u8Length = strlen((char*)psGroupTableEntry->au8GroupName);
            sResponse.sGroupName.pu8Data = psGroupTableEntry->au8GroupName;
        }
    }
    else
    {
        sResponse.eStatus = E_CLD_GROUPS_CMD_STATUS_NOT_FOUND;
    }

    if ( !bZCL_SendCommandResponse( pZPSevent) )
    {
        /* bound, group cast, or broadcast, so don't send a response */
        return eStatus;
    }

    sResponse.u16GroupId = sPayload.u16GroupId;
    sResponse.sGroupName.u8MaxLength = CLD_GROUPS_MAX_GROUP_NAME_LENGTH;

    // build address structure
    eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);

    eStatus = eCLD_GroupsCommandViewGroupResponseSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                     pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                     &sZCL_Address,
                                                     &u8TransactionSequenceNumber,
                                                     &sResponse);

    return eStatus;
}
#endif

/****************************************************************************
 **
 ** NAME:       eCLD_GroupsHandleGetGroupMembershipRequest
 **
 ** DESCRIPTION:
 ** Handles view group request
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
#ifdef GROUPS_SERVER
PRIVATE  teZCL_Status eCLD_GroupsHandleGetGroupMembershipRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem 		*pasPayloadDefinition)
{

    teZCL_Status eStatus;
    tsCLD_Groups_GetGroupMembershipRequestPayload sPayload;
    tsCLD_Groups_GetGroupMembershipResponsePayload sResponse;
    uint8   u8TransactionSequenceNumber;
    tsCLD_GroupsCustomDataStructure *psCommon;
    tsZCL_Address sZCL_Address;
    int16 ai16RequestList[CLD_GROUPS_MAX_NUMBER_OF_GROUPS];
    int16 ai16ListOfGroups[CLD_GROUPS_MAX_NUMBER_OF_GROUPS];

    sPayload.pi16GroupList = ai16RequestList;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_GroupsCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    pasPayloadDefinition[0].pvDestination = &sPayload.u8GroupCount;
    pasPayloadDefinition[1].pvDestination = sPayload.pi16GroupList;
    pasPayloadDefinition[1].u16MaxQuantity = CLD_GROUPS_MAX_NUMBER_OF_GROUPS;
    eStatus = eZCL_CustomCommandReceive(pZPSevent,
            &u8TransactionSequenceNumber,
            pasPayloadDefinition,
            2,
            E_ZCL_ACCEPT_LESS|E_ZCL_DISABLE_DEFAULT_RESPONSE);

    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_GROUPS, "\nErr: eCLD_GroupsCommandGetGroupMembershipRequestReceive:%d", eStatus);
        return eStatus;
    }

    DBG_vPrintf(TRACE_GROUPS, "\nGetGroupMembershipRequest: ");
#ifdef DEBUG_CLD_GROUPS
    {
        int x;
        for(x = 0; x < sPayload.u8GroupCount; x++)
        {
            DBG_vPrintf(TRACE_GROUPS, "0x%04x ", ai16RequestList[x]);
        }
    }
#endif

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psGetGroupMembershipRequestPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    // build address structure
    eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);

    /* If group count is zero, return a list of all groups we're in */
    if(sPayload.u8GroupCount == 0)
    {
        /* Get list of groups */
        sResponse.u8GroupCount = iCLD_GroupsListGroups(psEndPointDefinition, ai16ListOfGroups);
        sResponse.u8Capacity = CLD_GROUPS_MAX_NUMBER_OF_GROUPS - sResponse.u8GroupCount;
        sResponse.pi16GroupList = ai16ListOfGroups;

        DBG_vPrintf(TRACE_GROUPS, " GroupCount=%d Capacity=%d", sResponse.u8GroupCount, sResponse.u8Capacity);


        eStatus = eCLD_GroupsCommandGetGroupMembershipResponseSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                                   pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                                   &sZCL_Address,
                                                                   &u8TransactionSequenceNumber,
                                                                   &sResponse);

    }
    else
    {
        /* Get list of groups */
        sResponse.u8GroupCount = iCLD_GroupsListMatchingGroups(psEndPointDefinition,
                                                               sPayload.pi16GroupList,
                                                               sPayload.u8GroupCount,
                                                               ai16ListOfGroups);
        sResponse.u8Capacity = CLD_GROUPS_MAX_NUMBER_OF_GROUPS - iCLD_GroupsCountGroups(psEndPointDefinition);
        sResponse.pi16GroupList = ai16ListOfGroups;

        DBG_vPrintf(TRACE_GROUPS, " GroupCount=%d Capacity=%d", sResponse.u8GroupCount, sResponse.u8Capacity);

        eStatus = eCLD_GroupsCommandGetGroupMembershipResponseSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                                   pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                                   &sZCL_Address,
                                                                   &u8TransactionSequenceNumber,
                                                                   &sResponse);
    }

    return eStatus;
}
#endif

/****************************************************************************
 **
 ** NAME:       eCLD_GroupsHandleRemoveGroupRequest
 **
 ** DESCRIPTION:
 ** Handles remove group request
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint64                   u64SrcIEEEAddress        IEEE address of source or all FF
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
#ifdef GROUPS_SERVER
PRIVATE  teZCL_Status eCLD_GroupsHandleRemoveGroupRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem 		*pasPayloadDefinition)
{

    teZCL_Status eStatus;
    tsCLD_Groups_RemoveGroupRequestPayload sPayload;
    tsCLD_Groups_RemoveGroupResponsePayload sResponse;
    uint8   u8TransactionSequenceNumber;
    tsCLD_GroupsCustomDataStructure *psCommon;
    tsZCL_Address sZCL_Address;
    uint64 u64SrcIEEEAddress = (uint64)0xffffffffffffffffLL;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_GroupsCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    pasPayloadDefinition[0].pvDestination = &sPayload.u16GroupId;
    eStatus = eZCL_CustomCommandReceive(pZPSevent,
            &u8TransactionSequenceNumber,
            pasPayloadDefinition,
            1,
            E_ZCL_ACCEPT_EXACT|E_ZCL_DISABLE_DEFAULT_RESPONSE);

    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_GROUPS, "\nErr: eCLD_GroupsCommandRemoveGroupRequestReceive:%d", eStatus);
        return eStatus;
    }

    DBG_vPrintf(TRACE_GROUPS, "\nRemoveGroupRequest-Group:%04x", sPayload.u16GroupId);

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psRemoveGroupRequestPayload = &sPayload;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);


    #if (defined CLD_SCENES) && (defined SCENES_SERVER) && (defined CLD_SCENES_ATTR_LAST_CONFIGURED_BY) 
        if(pZPSevent->uEvent.sApsDataIndEvent.u8SrcAddrMode == E_ZCL_AM_IEEE)
        {
            u64SrcIEEEAddress = pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u64Addr;
        }
        else
        {
            u64SrcIEEEAddress = ZPS_u64NwkNibFindExtAddr(ZPS_pvNwkGetHandle(), pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr);
            if(u64SrcIEEEAddress == ZPS_NWK_NULL_EXT_ADDR)
            {
                u64SrcIEEEAddress = (uint64)0xffffffffffffffffLL;
            }
        }
    #endif
    eStatus = eCLD_GroupsRemoveGroup(psEndPointDefinition,
                                     psClusterInstance,
                                     &sPayload,
                                     u64SrcIEEEAddress);

    DBG_vPrintf(TRACE_GROUPS, "\nRemoveGroupStatus:%d", eStatus);

    if ( !bZCL_SendCommandResponse( pZPSevent) )
    {
        /* bound, group cast, or broadcast, so don't send a response */
        return eStatus;
    }

    switch(eStatus)
    {

    case E_ZCL_SUCCESS:
        sResponse.eStatus = E_CLD_GROUPS_CMD_STATUS_SUCCESS;
        break;

    case E_ZCL_FAIL:
        sResponse.eStatus = E_CLD_GROUPS_CMD_STATUS_NOT_FOUND;
        break;

    default:
        sResponse.eStatus = E_CLD_GROUPS_CMD_STATUS_FAILURE;
        break;

    }

    // build address structure
    eZCL_BuildClientTransmitAddressStructure(&sZCL_Address, &psCommon->sReceiveEventAddress);

    sResponse.u16GroupId = sPayload.u16GroupId;
    eStatus = eCLD_GroupsCommandRemoveGroupResponseSend(pZPSevent->uEvent.sApsDataIndEvent.u8DstEndpoint,
                                                        pZPSevent->uEvent.sApsDataIndEvent.u8SrcEndpoint,
                                                        &sZCL_Address,
                                                        &u8TransactionSequenceNumber,
                                                        &sResponse);
    return eStatus;
}
#endif

/****************************************************************************
 **
 ** NAME:       eCLD_GroupsHandleRemoveAllGroupsRequest
 **
 ** DESCRIPTION:
 ** Handles remove all groups request
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 ** uint64                   u64SrcIEEEAddress        IEEE address of source or all FF
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
#ifdef GROUPS_SERVER
PRIVATE  teZCL_Status eCLD_GroupsHandleRemoveAllGroupsRequest(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance)
{

    teZCL_Status eStatus;
    //uint8  u8TransactionSequenceNumber;
    uint64 u64SrcIEEEAddress = (uint64)0xffffffffffffffffLL;
    tsCLD_GroupsCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_GroupsCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    eStatus = eZCL_CustomCommandReceive(pZPSevent,
            &psCommon->sCustomCallBackEvent.u8TransactionSequenceNumber,
            0,
            0,
            E_ZCL_ACCEPT_EXACT);

    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_GROUPS, "\nErr: eCLD_GroupsCommandRemoveAllGroupRequestReceive:%d", eStatus);
        return eStatus;
    }

    DBG_vPrintf(TRACE_GROUPS, "\nRemoveAllGroupsRequest");

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    #if (defined CLD_SCENES) && (defined SCENES_SERVER) && (defined CLD_SCENES_ATTR_LAST_CONFIGURED_BY) 
        if(pZPSevent->uEvent.sApsDataIndEvent.u8SrcAddrMode == E_ZCL_AM_IEEE)
        {
            u64SrcIEEEAddress = pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u64Addr;
        }
        else
        {
            u64SrcIEEEAddress = ZPS_u64NwkNibFindExtAddr(ZPS_pvNwkGetHandle(), pZPSevent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr);
            if(u64SrcIEEEAddress == ZPS_NWK_NULL_EXT_ADDR)
            {
                u64SrcIEEEAddress = (uint64)0xffffffffffffffffLL;
            }
        }
    #endif

    eStatus = eCLD_GroupsRemoveAllGroups(psEndPointDefinition,
                                         psClusterInstance,
                                         u64SrcIEEEAddress);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_GROUPS, "\nFailed to remove all groups:%d", eStatus);
    }

    return eStatus;
}
#endif



#ifdef GROUPS_CLIENT
/****************************************************************************
 **
 ** NAME:       eCLD_GroupsHandleAddGroupResponse
 **
 ** DESCRIPTION:
 ** Handles add group request
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_GroupsHandleAddGroupResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem 		*pasPayloadDefinition)
{

    teZCL_Status eStatus;
    tsCLD_Groups_AddGroupResponsePayload sResponse;
    tsCLD_GroupsCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_GroupsCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    pasPayloadDefinition[0].pvDestination = &sResponse.eStatus;
    pasPayloadDefinition[1].pvDestination = &sResponse.u16GroupId;
    eStatus = eZCL_CustomCommandReceive(pZPSevent,
            &psCommon->sCustomCallBackEvent.u8TransactionSequenceNumber,
            pasPayloadDefinition,
            2,
            E_ZCL_ACCEPT_EXACT);

    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_GROUPS, "\nErr: eCLD_GroupsHandleAddGroupResponseReceive:%d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psAddGroupResponsePayload = &sResponse;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}


/****************************************************************************
 **
 ** NAME:       eCLD_GroupsHandleViewGroupResponse
 **
 ** DESCRIPTION:
 ** Handles view group request
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_GroupsHandleViewGroupResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem 		*pasPayloadDefinition)
{

    teZCL_Status eStatus;
    tsCLD_Groups_ViewGroupResponsePayload sResponse;
    uint8 au8GroupName[CLD_GROUPS_MAX_GROUP_NAME_LENGTH];
    tsCLD_GroupsCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_GroupsCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Point to group name storage */
    sResponse.sGroupName.u8MaxLength = CLD_GROUPS_MAX_GROUP_NAME_LENGTH;
    sResponse.sGroupName.pu8Data = au8GroupName;

    /* Receive the command */
    pasPayloadDefinition[0].pvDestination = &sResponse.eStatus;
    pasPayloadDefinition[1].pvDestination = &sResponse.u16GroupId;
    pasPayloadDefinition[2].pvDestination = &sResponse.sGroupName;
    eStatus = eZCL_CustomCommandReceive(pZPSevent,
            &psCommon->sCustomCallBackEvent.u8TransactionSequenceNumber,
            pasPayloadDefinition,
            3,
            E_ZCL_ACCEPT_EXACT);

    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_GROUPS, "\nErr: eCLD_GroupsCommandViewGroupResponseReceive:%d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psViewGroupResponsePayload = &sResponse;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}


/****************************************************************************
 **
 ** NAME:       eCLD_GroupsHandleGetGroupMembershipResponse
 **
 ** DESCRIPTION:
 ** Handles get group membership request
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_GroupsHandleGetGroupMembershipResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem 		*pasPayloadDefinition)
{

    teZCL_Status eStatus;
    tsCLD_Groups_GetGroupMembershipResponsePayload sResponse;
    int16 ai16ListOfGroups[CLD_GROUPS_MAX_NUMBER_OF_GROUPS];
    tsCLD_GroupsCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_GroupsCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Point to group group id storage */
    sResponse.pi16GroupList = ai16ListOfGroups;

    /* Receive the command */
    pasPayloadDefinition[0].pvDestination = &sResponse.u8Capacity;
	pasPayloadDefinition[1].pvDestination = &sResponse.u8GroupCount;
	pasPayloadDefinition[2].pvDestination = sResponse.pi16GroupList;

	pasPayloadDefinition[1].eType = E_ZCL_UINT8;
	pasPayloadDefinition[2].eType = E_ZCL_UINT16;
	pasPayloadDefinition[2].u16MaxQuantity = CLD_GROUPS_MAX_NUMBER_OF_GROUPS;

    eStatus = eZCL_CustomCommandReceive(pZPSevent,
            &psCommon->sCustomCallBackEvent.u8TransactionSequenceNumber,
            pasPayloadDefinition,
            3,
            E_ZCL_ACCEPT_LESS);

    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_GROUPS, "\nErr: eCLD_GroupsCommandGetGroupMembershipResponseReceive:%d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psGetGroupMembershipResponsePayload = &sResponse;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}


/****************************************************************************
 **
 ** NAME:       eCLD_GroupsHandleRemoveGroupResponse
 **
 ** DESCRIPTION:
 ** Handles add group request
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_GroupsHandleRemoveGroupResponse(
                            ZPS_tsAfEvent               *pZPSevent,
                            tsZCL_EndPointDefinition    *psEndPointDefinition,
                            tsZCL_ClusterInstance       *psClusterInstance,
                            tsZCL_RxPayloadItem 		*pasPayloadDefinition)
{

    teZCL_Status eStatus;
    tsCLD_Groups_RemoveGroupResponsePayload sResponse;
    tsCLD_GroupsCustomDataStructure *psCommon;

    /* Get pointer to custom data structure */
    psCommon = (tsCLD_GroupsCustomDataStructure*)psClusterInstance->pvEndPointCustomStructPtr;

    /* Receive the command */
    pasPayloadDefinition[0].pvDestination = &sResponse.eStatus;
    pasPayloadDefinition[1].pvDestination = &sResponse.u16GroupId;
    eStatus =eZCL_CustomCommandReceive(pZPSevent,
            &psCommon->sCustomCallBackEvent.u8TransactionSequenceNumber,
            pasPayloadDefinition,
            2,
            E_ZCL_ACCEPT_EXACT);

    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_GROUPS, "\nErr: eCLD_GroupsCommandRemoveGroupResponseReceive:%d", eStatus);
        return eStatus;
    }

    /* Message data for callback */
    psCommon->sCallBackMessage.uMessage.psRemoveGroupResponsePayload = &sResponse;

    /* call callback */
    psEndPointDefinition->pCallBackFunctions(&psCommon->sCustomCallBackEvent);

    return E_ZCL_SUCCESS;
}
#endif

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
