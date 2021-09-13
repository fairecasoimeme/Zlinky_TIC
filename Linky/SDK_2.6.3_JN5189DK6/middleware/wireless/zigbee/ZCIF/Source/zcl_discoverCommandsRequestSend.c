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
 * MODULE:             Discover Commands Send
 *
 * COMPONENT:          ZCL
 *
 * DESCRIPTION:       Commands Discovery
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


/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE teZCL_Status  eZCL_SendDiscoverCommandGenericRequest(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    uint16                      u16ClusterId,
                    uint8                       u8Command,                    
                    bool_t                      bDirectionIsServerToClient,
                    tsZCL_Address              *psDestinationAddress,
                    uint8                      *pu8TransactionSequenceNumber,
                    uint8                       u8CommandId,
                    bool_t                      bIsManufacturerSpecific,
                    uint16                      u16ManufacturerCode,
                    uint8                       u8MaximumNumberOfIdentifiers);
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
 ** NAME:       eZCL_SendDiscoverCommandReceivedRequest
 **
 ** DESCRIPTION:
 ** Sends an command discovery request for a cluster on a remote node.
 **
 ** PARAMETERS:      Name                           Usage
 ** uint8            u8SourceEndPointId             Source EP Id
 ** uint8            u8DestinationEndPointId        Destination EP Id
 ** uint16           u16ClusterId                   Cluster Id
 ** bool_t           bDirectionIsServerToClient     Direction
 ** tsZCL_Address   *psDestinationAddress           Destination Address
 ** uint8           *pu8TransactionSequenceNumber   Sequence number Pointer
 ** uint16           u8CommandId                    Attribute Id to start discovery
 ** bool_t           bIsManufacturerSpecific        Attribute manufacturer specific
 ** uint16           u16ManufacturerCode            Manufacturer code
 ** uint8            u8MaximumNumberOfIdentifiers   max number of commands to return
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
 PUBLIC teZCL_Status  eZCL_SendDiscoverCommandReceivedRequest(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    uint16                      u16ClusterId,                
                    bool_t                      bDirectionIsServerToClient,
                    tsZCL_Address              *psDestinationAddress,
                    uint8                      *pu8TransactionSequenceNumber,
                    uint8                       u8CommandId,                    
                    bool_t                      bIsManufacturerSpecific,
                    uint16                      u16ManufacturerCode,
                    uint8                       u8MaximumNumberOfIdentifiers)
{
    return eZCL_SendDiscoverCommandGenericRequest(
                    u8SourceEndPointId,
                    u8DestinationEndPointId,
                    u16ClusterId,
                    E_ZCL_DISCOVER_COMMANDS_RECEIVED,
                    bDirectionIsServerToClient,
                    psDestinationAddress,
                    pu8TransactionSequenceNumber,
                    u8CommandId,
                    bIsManufacturerSpecific,
                    u16ManufacturerCode,
                    u8MaximumNumberOfIdentifiers);
}
/****************************************************************************
 **
 ** NAME:       eZCL_SendDiscoverCommandGeneratedRequest
 **
 ** DESCRIPTION:
 ** Sends an command discovery request for a cluster on a remote node.
 **
 ** PARAMETERS:      Name                           Usage
 ** uint8            u8SourceEndPointId             Source EP Id
 ** uint8            u8DestinationEndPointId        Destination EP Id
 ** uint16           u16ClusterId                   Cluster Id
 ** bool_t           bDirectionIsServerToClient     Direction
 ** tsZCL_Address   *psDestinationAddress           Destination Address
 ** uint8           *pu8TransactionSequenceNumber   Sequence number Pointer
 ** uint16           u8CommandId                    Attribute Id to start discovery
 ** bool_t           bIsManufacturerSpecific        Attribute manufacturer specific
 ** uint16           u16ManufacturerCode            Manufacturer code
 ** uint8            u8MaximumNumberOfIdentifiers   max number of commands to return
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
 PUBLIC teZCL_Status  eZCL_SendDiscoverCommandGeneratedRequest(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    uint16                      u16ClusterId,                
                    bool_t                      bDirectionIsServerToClient,
                    tsZCL_Address              *psDestinationAddress,
                    uint8                      *pu8TransactionSequenceNumber,
                    uint8                       u8CommandId,
                    bool_t                      bIsManufacturerSpecific,
                    uint16                      u16ManufacturerCode,
                    uint8                       u8MaximumNumberOfIdentifiers)
{
    return eZCL_SendDiscoverCommandGenericRequest(
                    u8SourceEndPointId,
                    u8DestinationEndPointId,
                    u16ClusterId,
                    E_ZCL_DISCOVER_COMMANDS_GENERATED,
                    bDirectionIsServerToClient,
                    psDestinationAddress,
                    pu8TransactionSequenceNumber,
                    u8CommandId,
                    bIsManufacturerSpecific,
                    u16ManufacturerCode,
                    u8MaximumNumberOfIdentifiers);
}
/****************************************************************************/
/***        Local Functions                                              ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eZCL_SendDiscoverCommandGenericRequest
 **
 ** DESCRIPTION:
 ** Sends an command discovery request for a cluster on a remote node.
 **
 ** PARAMETERS:      Name                           Usage
 ** uint8            u8SourceEndPointId             Source EP Id
 ** uint8            u8DestinationEndPointId        Destination EP Id
 ** uint16           u16ClusterId                   Cluster Id
 ** uint8            u8Command                      Received or generated   
 ** bool_t           bDirectionIsServerToClient     Direction
 ** tsZCL_Address   *psDestinationAddress           Destination Address
 ** uint8           *pu8TransactionSequenceNumber   Sequence number Pointer
 ** uint16           u8CommandId                    Attribute Id to start discovery
 ** bool_t           bIsManufacturerSpecific        Attribute manufacturer specific
 ** uint16           u16ManufacturerCode            Manufacturer code
 ** uint8            u8MaximumNumberOfIdentifiers   max number of commands to return
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE teZCL_Status  eZCL_SendDiscoverCommandGenericRequest(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8DestinationEndPointId,
                    uint16                      u16ClusterId,
                    uint8                       u8Command,                    
                    bool_t                      bDirectionIsServerToClient,
                    tsZCL_Address              *psDestinationAddress,
                    uint8                      *pu8TransactionSequenceNumber,
                    uint8                       u8CommandId,
                    bool_t                      bIsManufacturerSpecific,
                    uint16                      u16ManufacturerCode,
                    uint8                       u8MaximumNumberOfIdentifiers)
{

    uint16 u16offset;
    uint16 u16payloadSize;

    teZCL_Status eReturnValue;
    tsZCL_EndPointDefinition    *psZCL_EndPointDefinition;

    PDUM_thAPduInstance myPDUM_thAPduInstance;
    uint16 u16TempManufacturerCode = u16ManufacturerCode;

    // parameter checks
	#ifdef STRICT_PARAM_CHECK	
        if((psDestinationAddress == NULL) || (pu8TransactionSequenceNumber == NULL))
        {
            return(E_ZCL_ERR_PARAMETER_NULL);
        }



        if(u8MaximumNumberOfIdentifiers == 0)
        {
            return(E_ZCL_ERR_ATTRIBUTES_0);
        }
    #endif
    // check EP is registered and cluster is present in the send device
    eReturnValue = eZCL_SearchForEPentry(u8SourceEndPointId, &psZCL_EndPointDefinition);
    if(eReturnValue != E_ZCL_SUCCESS)
    {
        return(eReturnValue);
    }
    u16ManufacturerCode = psZCL_EndPointDefinition->u16ManufacturerCode;
    // check EP is registered and cluster is present in the send device for manufacturere specefic
    eReturnValue = eZCL_SearchForEPentryAndCheckManufacturerId(u8SourceEndPointId, bIsManufacturerSpecific, u16ManufacturerCode, &psZCL_EndPointDefinition);
    if(eReturnValue != E_ZCL_SUCCESS)
    {
        return(eReturnValue);
    }


    // build command packet
    // get buffer
    myPDUM_thAPduInstance = hZCL_AllocateAPduInstance();

    if(myPDUM_thAPduInstance == PDUM_INVALID_HANDLE)
    {
        return(E_ZCL_ERR_ZBUFFER_FAIL);
    }

    // handle sequence number pass present value back to user
    *pu8TransactionSequenceNumber = u8GetTransactionSequenceNumber();
    u16ManufacturerCode = u16TempManufacturerCode;
    // write command header
    u16offset = u16ZCL_WriteCommandHeader(myPDUM_thAPduInstance,
                                       eFRAME_TYPE_COMMAND_ACTS_ACCROSS_ENTIRE_PROFILE,
                                       bIsManufacturerSpecific,
                                       u16ManufacturerCode,
                                       bDirectionIsServerToClient,
                                       psZCL_EndPointDefinition->bDisableDefaultResponse,
                                       *pu8TransactionSequenceNumber,
                                       u8Command);

    // calculate payload size - Two more bytes to follow CommandId and the Identifier
    u16payloadSize = u16offset + 2;
    // check buffer size
    if(PDUM_u16APduGetSize(psZCL_Common->hZCL_APdu) < u16payloadSize)
    {
        // free buffer and return
        PDUM_eAPduFreeAPduInstance(myPDUM_thAPduInstance);
        return(E_ZCL_ERR_ZBUFFER_FAIL);
    }

    // write payload
    u16offset += u16ZCL_APduInstanceWriteNBO(myPDUM_thAPduInstance, u16offset, E_ZCL_UINT8/*E_ZCL_COMMAND_ID - TBD*/, &u8CommandId);
    u16offset += u16ZCL_APduInstanceWriteNBO(myPDUM_thAPduInstance, u16offset, E_ZCL_UINT8, &u8MaximumNumberOfIdentifiers);

    // transmit the request
    if(eZCL_TransmitDataRequest(myPDUM_thAPduInstance,
                                u16payloadSize,
                                u8SourceEndPointId,
                                u8DestinationEndPointId,
                                u16ClusterId,
                                psDestinationAddress) != E_ZCL_SUCCESS)
    {
        return(E_ZCL_ERR_ZTRANSMIT_FAIL);
    }

    return(E_ZCL_SUCCESS);
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
