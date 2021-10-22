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
 * COMPONENT:          zcl_command.c
 *
 * DESCRIPTION:       ZCL command header construction functions
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
#include "zcl_internal.h"

#include "pdum_apl.h"
#include "zps_apl.h"
#include "zps_apl_af.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define FRAME_TYPE_SHIFT                                (0)
#define MANUFACTURER_SPECIFIC_SHIFT                     (2)
#define DIRECTION_SHIFT                                 (3)
#define DISABLE_DEFAULT_RESPONSE_SHIFT                  (4)

#define APDU_FRAME_CONTROL_POSITION                     (0)

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE void vZCL_BuildFrameControlField(
                uint8                 *pu8FrameFrameControlField,
                eFrameTypeSubfield     eFrameType,
                bool_t                 bManufacturerSpecific,
                bool_t                 bDirection, 
                bool_t                 bDisableDefaultResponse);



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
 ** NAME:       u16ZCL_WriteCommandHeader
 **
 ** DESCRIPTION:
 ** Builds ZCL command Frame Header
 **
 ** PARAMETERS:                 Name                        Usage
 ** PDUM_thAPduInstance         PDUM_thAPduInstance         Zigbee Buffer
 ** eFrameTypeSubfield          eFrameType                  Frame Type
 ** bool_t                      bManufacturerSpecific       Flag to Indicate Manf. Specific
 ** uint16                      u16ManufacturerCode         Manf. Code, if specific
 ** bool_t                      bDirection                  From Client/Server
 ** bool_t                      bDisableDefaultResponse     Disable Def Response
 ** uint8                       u8TransactionSequenceNumber ZCL Transaction Seq Number
 ** uint8                       u8CommandIdentifier         Command Id
 **
 ** RETURN:
 ** uint16 - Size Of Command Header, In Bytes
 **
 ****************************************************************************/

PUBLIC uint16 u16ZCL_WriteCommandHeader(
                    PDUM_thAPduInstance         PDUM_thAPduInstance,
                    eFrameTypeSubfield          eFrameType, 
                    bool_t                      bManufacturerSpecific,
                    uint16                      u16ManufacturerCode,
                    bool_t                      bDirection, 
                    bool_t                      bDisableDefaultResponse,
                    uint8                       u8TransactionSequenceNumber,
                    uint8                       u8CommandIdentifier)
{
    uint8 u8FrameControlField;
    uint16 u16offset;

    // build FC
    vZCL_BuildFrameControlField(&u8FrameControlField, eFrameType, bManufacturerSpecific, bDirection, bDisableDefaultResponse);
    u16offset = u16ZCL_APduInstanceWriteNBO(PDUM_thAPduInstance, APDU_FRAME_CONTROL_POSITION, E_ZCL_UINT8, &u8FrameControlField);

    // complete rest of command
    if(bManufacturerSpecific)
    {
        u16offset += u16ZCL_APduInstanceWriteNBO(PDUM_thAPduInstance, u16offset, E_ZCL_UINT16, &u16ManufacturerCode);
    }
    // add TSN
    u16offset += u16ZCL_APduInstanceWriteNBO(PDUM_thAPduInstance, u16offset, E_ZCL_UINT8, &u8TransactionSequenceNumber);
    // add CID
    u16offset += u16ZCL_APduInstanceWriteNBO(PDUM_thAPduInstance, u16offset, E_ZCL_UINT8, &u8CommandIdentifier);

    return(u16offset);

}

/****************************************************************************
 **
 ** NAME:       u8ZCL_ReadCommandHeader
 **
 ** DESCRIPTION:
 ** Builds ZCL command Frame Header
 **
 ** PARAMETERS:                 Name                         Usage
 ** PDUM_thAPduInstance         PDUM_thAPduInstance          Zigbee Buffer
 ** eFrameTypeSubfield          peFrameType                  Frame Type
 ** bool_t                      pbManufacturerSpecific       Flag to Indicate Manf. Specific
 ** uint16                      pu16ManufacturerCode         Manf. Code, if specific
 ** bool_t                      pbDirection                  From Client/Server
 ** bool_t                      pbDisableDefaultResponse     Disable Def Response
 ** uint8                       pu8TransactionSequenceNumber ZCL Transaction Seq Number
 ** uint8                       pu8CommandIdentifier         Command Id
 **
 ** RETURN:
 ** uint16 - Size Of Command Header, In Bytes
 **
 ** RETURN:
 ** None
 **
 ****************************************************************************/

PUBLIC uint16 u16ZCL_ReadCommandHeader(
                    PDUM_thAPduInstance     PDUM_thAPduInstance,
                    tsZCL_HeaderParams      *psZCL_HeaderParams)

{
    uint8 u8FrameControlField;
    uint16 u16offset;

//    if((&(psZCL_HeaderParams->eFrameType)==NULL) || (&(psZCL_HeaderParams->bManufacturerSpecific)==NULL) || (&(psZCL_HeaderParams->u16ManufacturerCode)==NULL) || 
//       (&(psZCL_HeaderParams->bDirection)==NULL) || (&(psZCL_HeaderParams->bDisableDefaultResponse)==NULL) || (&(psZCL_HeaderParams->u8TransactionSequenceNumber)==NULL) || (&(psZCL_HeaderParams->u8CommandIdentifier)==NULL))
	if(psZCL_HeaderParams == NULL)
    {
        return(0xffff);
    }

    // read FC
    u16offset = u16ZCL_APduInstanceReadNBO(PDUM_thAPduInstance, APDU_FRAME_CONTROL_POSITION, E_ZCL_UINT8, &u8FrameControlField);
    psZCL_HeaderParams->eFrameType = (u8FrameControlField >> FRAME_TYPE_SHIFT) & 0x03;
    psZCL_HeaderParams->bManufacturerSpecific = (u8FrameControlField >> MANUFACTURER_SPECIFIC_SHIFT) & 0x1;
    psZCL_HeaderParams->bDirection = (u8FrameControlField >> DIRECTION_SHIFT) & 0x1;
    psZCL_HeaderParams->bDisableDefaultResponse = (u8FrameControlField >> DISABLE_DEFAULT_RESPONSE_SHIFT) & 0x1;

    // read rest of command
    if(psZCL_HeaderParams->bManufacturerSpecific)
    {
        u16offset += u16ZCL_APduInstanceReadNBO(PDUM_thAPduInstance, u16offset, E_ZCL_UINT16, &(psZCL_HeaderParams->u16ManufacturerCode));
    }
    // TSN
    u16offset += u16ZCL_APduInstanceReadNBO(PDUM_thAPduInstance, u16offset, E_ZCL_UINT8, &(psZCL_HeaderParams->u8TransactionSequenceNumber));
    // CID
    u16offset += u16ZCL_APduInstanceReadNBO(PDUM_thAPduInstance, u16offset, E_ZCL_UINT8, &(psZCL_HeaderParams->u8CommandIdentifier));

    return(u16offset);

}

/****************************************************************************
 **
 ** NAME:       u8GetTransactionSequenceNumber
 **
 ** DESCRIPTION:
 ** Obtains a transaction sequence number and internally incremants the
 ** value for the next request
 ** PARAMETERS:                 Name                        Usage
 ** None
 **
 ** RETURN:
 ** u8TransactionSequenceNumber
 **
 ****************************************************************************/

PUBLIC uint8 u8GetTransactionSequenceNumber(void)
{
    uint8 u8TransactionSequenceNumber;
 
    #ifndef COOPERATIVE
    // get ZCL mutex
        vZCL_GetInternalMutex();
    #endif

    // read sequence number
    u8TransactionSequenceNumber = psZCL_Common->u8TransactionSequenceNumber;

    // increment the transaction counter for next time
    psZCL_Common->u8TransactionSequenceNumber++;
    #ifndef COOPERATIVE
        vZCL_ReleaseInternalMutex();
    #endif
    // pass present value back to user
    return(u8TransactionSequenceNumber);
}
    
/****************************************************************************
 **
 ** NAME:       vZCL_BuildFrameControlField
 **
 ** DESCRIPTION:
 ** Builds ZCL command Frame Control Field
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       *pu8FrameFrameControlField  ptr to FCF byte
 ** eFrameTypeSubfield          eFrameType                  Frame Type
 ** bool_t                      bManufacturerSpecific       Flag to Indicate Manf. Specific
 ** bool_t                      bDirection                  From Client/Server
 ** bool_t                      bDisableDefaultResponse     Disable Def Response
 **
 ** RETURN:
 ** None
 **
 ****************************************************************************/

PRIVATE void vZCL_BuildFrameControlField(
                uint8                 *pu8FrameFrameControlField,
                eFrameTypeSubfield     eFrameType,
                bool_t                 bManufacturerSpecific,
                bool_t                 bDirection, 
                bool_t                 bDisableDefaultResponse)
{
    *pu8FrameFrameControlField = 0;

    *pu8FrameFrameControlField |= (eFrameType << FRAME_TYPE_SHIFT);

    if(bManufacturerSpecific)
    {
        *pu8FrameFrameControlField |= (1 << MANUFACTURER_SPECIFIC_SHIFT);
    }

    if(bDirection)
    {
        *pu8FrameFrameControlField |= (1 << DIRECTION_SHIFT);
    }

    if(bDisableDefaultResponse)
    {
        *pu8FrameFrameControlField |= (1 << DISABLE_DEFAULT_RESPONSE_SHIFT);
    }

}
/****************************************************************************
 **
 ** NAME:       u16ZCL_GetTxPayloadSize
 **
 ** DESCRIPTION:
 ** Returns the maximum Payload size possible through ZCL
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint16                      u16DestAddr                 Destination short address
 **
 ** RETURN:
 ** None
 **
 ****************************************************************************/
PUBLIC uint16 u16ZCL_GetTxPayloadSize(uint16 u16DestAddr)
{
        /* If fragmentation is supported APDU should be taken as reference */
    if(ZPS_bAplDoesDeviceSupportFragmentation(ZPS_pvAplZdoGetAplHandle()))
    {
        return(PDUM_u16APduGetSize(psZCL_Common->hZCL_APdu));
    }
    else /* If fragmentation is not supported find the actual payload size from network layer */
    {
        return(ZPS_u8AplGetMaxPayloadSize(ZPS_pvAplZdoGetAplHandle(),u16DestAddr));
    }
}
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
