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
 * COMPONENT:          zcl_attribute.c
 *
 * DESCRIPTION:       Attribute read and write functions
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include <string.h>

#include "zcl.h"
#include "zcl_customcommand.h"
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
 ** NAME:       u16ZCL_WriteAttributeValueIntoBuffer
 **
 ** DESCRIPTION:
 ** Writes an Attribute Value into a PDU buffer
 **
 ** PARAMETERS:                  Name                    Usage
 ** tsZCL_ClusterInstance       *psClusterInstance       Cluster Structure
 ** tsZCL_AttributeDefinition   *psAttributeDefinition   Attribute Structure
 ** uint16                       u16offset               Attribute data offset
 ** PDUM_thAPduInstance          PDUM_thAPduInstance     Buffer
 **
 ** RETURN:
 ** uint16 - attribute type size
 **
 ****************************************************************************/

PUBLIC uint16 u16ZCL_WriteAttributeValueIntoBuffer(
                uint8                       u8EndPointId,
                uint16                       u16AttributeId,
                tsZCL_ClusterInstance       *psClusterInstance,
                tsZCL_AttributeDefinition   *psAttributeDefinition,
                uint16                       u16offset,
                PDUM_thAPduInstance          PDUM_thAPduInstance)
{

    void *pvAttributeValueOffset;
    uint16 u16NoOfBytes;

    pvAttributeValueOffset = pvZCL_GetAttributePointer(psAttributeDefinition,
                                                       psClusterInstance,
                                                       u16AttributeId);

    switch(psAttributeDefinition->eAttributeDataType)
    {
        case(E_ZCL_LOSTRING):
        case(E_ZCL_LCSTRING):
        {
            if(psClusterInstance->pStringCallBackFunction)
            {
                if(psClusterInstance->pStringCallBackFunction(u8EndPointId,
                                                              u16AttributeId,
                                                              psAttributeDefinition->u8AttributeFlags & E_ZCL_AF_CA,
                                                              TRUE,
                                                              PDUM_thAPduInstance,
                                                              u16offset,
                                                              &u16NoOfBytes)== E_ZCL_SUCCESS)
                {
                    return u16NoOfBytes;
                }

            }
        }
        case(E_ZCL_OSTRING):
        case(E_ZCL_CSTRING):
        {

            return u16ZCL_APduInstanceWriteStringNBO(
                                PDUM_thAPduInstance,
                                u16offset,
                                psAttributeDefinition->eAttributeDataType,
                                pvAttributeValueOffset);
        }
        default:
        {
            return u16ZCL_APduInstanceWriteNBO(
                                PDUM_thAPduInstance,
                                u16offset,
                                psAttributeDefinition->eAttributeDataType,
                                pvAttributeValueOffset);
        }
    }
}

/****************************************************************************
 **
 ** NAME:       u16ZCL_WriteAttributeValueIntoStructure
 **
 ** DESCRIPTION:
 ** Writes an Attribute Value into an Attribute Structure
 **
 ** PARAMETERS:                  Name                    Usage
 ** uint8                        u8EndPointId            EP Id
 ** uint16                       u16AttributeId          Attribute Id
 ** bool_t                       bIsManufacturerSpecific manufacturer Specific Flag
 ** bool_t                       bIsClientAttribute       whether client or server attribute
 ** tsZCL_ClusterInstance       *psClusterInstance   Cluster Structure
 ** uint16                       u16offset               Attribute data offset
 ** PDUM_thAPduInstance          PDUM_thAPduInstance     Buffer
 **
 ** RETURN:
 ** uint8 - attribute type size
 **
 ** RETURN:
 ** None
 **
 ****************************************************************************/

PUBLIC uint16 u16ZCL_WriteAttributeValueIntoStructure(
                uint8                      u8EndPointId,
                uint16                     u16AttributeId,
                bool_t                     bIsManufacturerSpecific,
                bool_t                     bIsClientAttribute,
                tsZCL_ClusterInstance      *psClusterInstance,
                uint16                     u16offset,
                PDUM_thAPduInstance        hAPduInst)
{


    void *pvAttributeValueOffset;
    tsZCL_AttributeDefinition *psAttributeDefinition;

    uint16 u16typeSize;
    uint16 u16attributeIndex, u16NoOfBytes;;

    // find attribute
    if(eZCL_SearchForAttributeEntry(u8EndPointId,
                                    u16AttributeId,
                                    bIsManufacturerSpecific,
                                    bIsClientAttribute,
                                    psClusterInstance,
                                    &psAttributeDefinition,
                                    &u16attributeIndex) != E_ZCL_SUCCESS)
    {
        // clean up
        return 0;
    }
    // get address of attribute structure location
    pvAttributeValueOffset = pvZCL_GetAttributePointer(psAttributeDefinition,
                                                        psClusterInstance,
                                                        u16AttributeId);

    if( psAttributeDefinition->eAttributeDataType == E_ZCL_LOSTRING ||
        psAttributeDefinition->eAttributeDataType == E_ZCL_LCSTRING )
    {
        if(psClusterInstance->pStringCallBackFunction)
        {
            if(psClusterInstance->pStringCallBackFunction(u8EndPointId,
                                                          u16AttributeId,
                                                          psAttributeDefinition->u8AttributeFlags & E_ZCL_AF_CA,
                                                          FALSE,
                                                          hAPduInst,
                                                          u16offset,
                                                          &u16NoOfBytes) == E_ZCL_SUCCESS)
            {
                return u16NoOfBytes;
            }
        }
   }

   if( psAttributeDefinition->eAttributeDataType == E_ZCL_LOSTRING ||
       psAttributeDefinition->eAttributeDataType == E_ZCL_LCSTRING ||
       psAttributeDefinition->eAttributeDataType == E_ZCL_OSTRING ||
       psAttributeDefinition->eAttributeDataType == E_ZCL_CSTRING )
   {
       if(eZCL_GetAttributeTypeSizeFromBuffer(psAttributeDefinition->eAttributeDataType,
                                               hAPduInst,
                                               u16offset,
                                               &u16typeSize)!= E_ZCL_SUCCESS)
        {
            return 0;
        }

        return u16ZCL_APduInstanceReadStringNBO(
                                hAPduInst,
                                u16offset,
                                psAttributeDefinition->eAttributeDataType,
                                pvAttributeValueOffset);
    }

    return u16ZCL_APduInstanceReadNBO(
                                hAPduInst,
                                u16offset,
                                psAttributeDefinition->eAttributeDataType,
                                pvAttributeValueOffset);

}

/****************************************************************************
 **
 ** NAME:       u8ZCL_GetAttributeAllignToFourBytesBoundary
 **
 ** DESCRIPTION:
 ** Allign the attrubutes to four bytes boundary
 **
 ** PARAMETERS:                 Name                    Usage
 ** uint8                      u8typeSize               data type size
 **
 ** RETURN:
 ** uint8
 **
 ****************************************************************************/

PUBLIC uint8 u8ZCL_GetAttributeAllignToFourBytesBoundary(
                            uint8                   u8TypeSize)
{
    return (((u8TypeSize>2) && (u8TypeSize % 4)) ? (((u8TypeSize/4)+1)*4): u8TypeSize);
}

/****************************************************************************
 **
 ** NAME:       eZCL_GetAttributeTypeSize
 **
 ** DESCRIPTION:
 ** Gets Attribute type size
 **
 ** PARAMETERS:              Name                    Usage
 ** teZCL_ZCLAttributeType   eAttributeDataType      Attribute data type
 ** uint8                   *pu8typeSize             data type size
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
 PUBLIC const uint8 au8ConstSizeArray[258] = { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    /*0x00*/
                                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,    /*0x08*/
                                 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    /*0x10*/
                                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,    /*0x18*/
                                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,    /*0x20*/
                                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,    /*0x28*/ 
                                 0x01, 0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    /*0x30*/
                                 0x02, 0x04, 0x08, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    /*0x38*/
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,    /*0x40*/
                                 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    /*0x48*/
                                 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    /*0x50*/
                                 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    /*0x58*/
                                 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    /*0x60*/
                                 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    /*0x68*/
                                 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    /*0x70*/
                                 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    /*0x78*/
                                 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    /*0x80*/
                                 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    /*0x88*/
                                 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    /*0x90*/
                                 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    /*0x98*/
                                 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    /*0xA0*/
                                 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    /*0xA8*/
                                 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    /*0xB0*/
                                 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    /*0xB8*/
                                 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    /*0xC0*/
                                 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    /*0xC8*/
                                 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    /*0xD0*/
                                 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    /*0xD8*/
                                 0x04, 0x04, 0x04, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    /*0xE0*/
                                 0x02, 0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    /*0xE8*/
                                 0x08, E_ZCL_KEY_128_SIZE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    /*0xF0*/
                                 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, E_ZCL_SIGNATURE_SIZE, 0xFF,    /*0xF8*/
                                 0xFF, 0xFF};
PUBLIC teZCL_Status eZCL_GetAttributeTypeSize(
                teZCL_ZCLAttributeType     eAttributeDataType,
                uint8                     *pu8typeSize) 
{
    *pu8typeSize = au8ConstSizeArray[eAttributeDataType];
    if(*pu8typeSize == 0xFF)
    {
        *pu8typeSize = 0;
        return(E_ZCL_ERR_ATTRIBUTE_TYPE_UNSUPPORTED);
    }
    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       eZCL_GetAttributeTypeSizeFromBuffer
 **
 ** DESCRIPTION:
 ** Gets Attribute type size
 **
 ** PARAMETERS:              Name                    Usage
 ** teZCL_ZCLAttributeType   eAttributeDataType      Attribute data type
 ** PDUM_thAPduInstance      hAPduInst               PDU buffer
 ** uint16                   u16inputOffset          position in buffer
 ** uint8                   *pu8typeSize             data type size
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eZCL_GetAttributeTypeSizeFromBuffer(
                            teZCL_ZCLAttributeType  eAttributeDataType,
                            PDUM_thAPduInstance     hAPduInst,
                            uint16                  u16inputOffset,
                            uint16                 *pu16typeSize)
{
    uint8 u8typeSize;
    uint16 u16stringDataLength;
    uint8 *pu8stringData;
    uint8 u8lengthfield;
    uint16 u16stringDataLengthField;

    teZCL_Status eGetAttributeTypeSizeReturn;

    uint16 u16APduSize;

    switch(eAttributeDataType)
    {
        // add attribute types with handlers here which are variable length
        case(E_ZCL_OSTRING):
        case(E_ZCL_CSTRING):
        case(E_ZCL_LOSTRING):
        case(E_ZCL_LCSTRING):
        {
            // check buffer dimensions before read
            u16APduSize = PDUM_u16APduGetSize(PDUM_thAPduInstanceGetApdu(hAPduInst));
            // get string length size so we can perform proper boundary checking
            if(eZCL_GetStringLengthFieldSize(eAttributeDataType, &u8lengthfield) != E_ZCL_SUCCESS)
            {
                // game over
                *pu16typeSize = 0;
                return(E_ZCL_FAIL);
            }

            if((hAPduInst==NULL) || ((u16inputOffset+u8lengthfield) > u16APduSize ))
            {
                // game over
                *pu16typeSize = 0;
                return(E_ZCL_FAIL);
            }

            *pu16typeSize = u16ZCL_GetStringAttributeTypeSizeFromBuffer(eAttributeDataType, &u16stringDataLength, &u16stringDataLengthField,&pu8stringData,   (unsigned char*)(PDUM_pvAPduInstanceGetPayload(hAPduInst)) + u16inputOffset);
            // pre-empt an over-read
            if(((u16inputOffset+u8lengthfield)== u16APduSize) && (*pu16typeSize!=0))
            {
                return(E_ZCL_FAIL);
            }
            return(E_ZCL_SUCCESS);
        }
        default:
        {
            eGetAttributeTypeSizeReturn = eZCL_GetAttributeTypeSize(eAttributeDataType, &u8typeSize);
            *pu16typeSize = u8typeSize;
            return eGetAttributeTypeSizeReturn;
        }
    }
}
/****************************************************************************
 **
 ** NAME:       eZCL_GetAttributeTypeSizeFromStructure
 **
 ** DESCRIPTION:
 ** Gets Attribute type size
 **
 ** PARAMETERS:                 Name                    Usage
 ** tsZCL_AttributeDefinition  *psAttributeDefinition   Attribute data type struct
 ** void                       *pvAttributeBuffer       Pointer to cluster data
 ** uint8                      *pu8typeSize             data type size
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eZCL_GetAttributeTypeSizeFromStructure(
                            tsZCL_AttributeDefinition  *psAttributeDefinition,
                            void                       *pvAttributeStructure,
                            uint16                     *pu16typeSize)
{
    uint8 u8typeSize;
    uint16 u16stringDataLength;
    uint8 *pu8stringData;
    uint16 u16stringLengthField;
    teZCL_Status eGetAttributeTypeSizeReturn;

    switch(psAttributeDefinition->eAttributeDataType)
    {
        // add attribute types with handlers here which are variable length
        case(E_ZCL_OSTRING):
        case(E_ZCL_CSTRING):
        case(E_ZCL_LOSTRING):
        case(E_ZCL_LCSTRING):
        {
            // get address of attribute structure location
            if(eZCL_CheckUserStringStructureRead(psAttributeDefinition->eAttributeDataType,pvAttributeStructure) != E_ZCL_SUCCESS)
            {
                return E_ZCL_FAIL;
            }
            // get string size
            *pu16typeSize =
             u16ZCL_GetStringSizeAndBufferFromStructure(psAttributeDefinition->eAttributeDataType, pvAttributeStructure, &u16stringDataLength, &u16stringLengthField, &pu8stringData);
            return(E_ZCL_SUCCESS);
        }
        default:
        {
            eGetAttributeTypeSizeReturn = eZCL_GetAttributeTypeSize(psAttributeDefinition->eAttributeDataType, &u8typeSize);
            *pu16typeSize = u8typeSize;
            return eGetAttributeTypeSizeReturn;
        }
    }
}
/****************************************************************************
 **
 ** NAME:       bZCL_CheckManufacturerSpecificAttributeFlagMatch
 **
 ** DESCRIPTION:
 **
 **
 ** PARAMETERS:                 Name                    Usage
 ** tsZCL_AttributeDefinition  *psAttributeDefinition   Attribute data type struct
 ** bool_t                      bManufacturerSpecificAttributeFlag
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC bool_t bZCL_CheckManufacturerSpecificAttributeFlagMatch(
                    tsZCL_AttributeDefinition  *psAttributeDefinition,
                    bool_t                      bManufacturerSpecificAttributeFlag)
{

    bool_t bLocalManufacturerSpecificAttributeFlag = FALSE;

    if(psAttributeDefinition==NULL)
    {
        return(FALSE);
    }
    // get attribute flag and set bool flag
    if(psAttributeDefinition->u8AttributeFlags & E_ZCL_AF_MS)
    {
        bLocalManufacturerSpecificAttributeFlag = TRUE;
    }
    // compare
    if(bLocalManufacturerSpecificAttributeFlag == bManufacturerSpecificAttributeFlag)
    {
        return(TRUE);
    }
    return(FALSE);
}

/****************************************************************************
 **
 ** NAME:       bZCL_CheckStringFitsInStructure
 **
 ** DESCRIPTION:
 ** Checks a string in a PDU will fit into a structure if it is written
 **
 ** PARAMETERS:                  Name                    Usage
 ** uint8                        u8EndPointId            EP Id
 ** uint16                       u16AttributeId          Attribute Id
 ** bool_t                       bIsManufacturerSpecific
 ** bool_t                       bIsClientAttribute
 ** tsZCL_ClusterInstance       *psClusterInstance   Cluster Structure
 ** uint16                       u16offset               Attribute data offset
 ** PDUM_thAPduInstance          PDUM_thAPduInstance     Buffer
 **
 ** RETURN:
 ** bool_t                       TRUE if string will fit
 **
 ****************************************************************************/

PUBLIC bool_t bZCL_CheckStringFitsInStructure(
                                            uint8                      u8EndPointId,
                                            uint16                     u16AttributeId,
                                            bool_t                     bIsManufacturerSpecific,
                                            bool_t                     bIsClientAttribute,
                                            tsZCL_ClusterInstance     *psClusterInstance,
                                            uint16                     u16offset,
                                            PDUM_thAPduInstance        hAPduInst)
{
    void *pvAttributeValueOffset;
    tsZCL_AttributeDefinition *psAttributeDefinition;
    uint16 u16typeSize;
    uint16 u16attributeIndex;

    // find attribute
    if(eZCL_SearchForAttributeEntry(u8EndPointId, u16AttributeId, bIsManufacturerSpecific,bIsClientAttribute, psClusterInstance, &psAttributeDefinition, &u16attributeIndex) != E_ZCL_SUCCESS)
    {
        return FALSE;
    }

    // get address of attribute structure location
    pvAttributeValueOffset = pvZCL_GetAttributePointer(psAttributeDefinition,
                                                       psClusterInstance,
                                                       u16AttributeId);

    switch(psAttributeDefinition->eAttributeDataType)
    {
        case(E_ZCL_OSTRING):
        case(E_ZCL_CSTRING):
        case(E_ZCL_LOSTRING):
        case(E_ZCL_LCSTRING):
        {
            // Get size from struct
            uint16 u16structSize = u16ZCL_GetStringStructureSize(psAttributeDefinition->eAttributeDataType,
                                                                 pvAttributeValueOffset);

            // Get size from buffer
            if(eZCL_GetAttributeTypeSizeFromBuffer(psAttributeDefinition->eAttributeDataType,
                                                   hAPduInst,
                                                   u16offset,
                                                   &u16typeSize)!= E_ZCL_SUCCESS)
                return FALSE;
            // Type size includes the string length bytes
            u16typeSize--;
            if (psAttributeDefinition->eAttributeDataType == E_ZCL_LOSTRING ||
                psAttributeDefinition->eAttributeDataType == E_ZCL_LCSTRING)
            {
                u16typeSize--;
            }
            if((u16structSize == 0) || (u16typeSize > u16structSize))
                return FALSE;
            break;
        }
        default:
            break;
    }
    // Non string types will fit
    return TRUE;
}
/****************************************************************************
 **
 ** NAME:       bZCL_AttributeTypeIsSigned
 **
 ** DESCRIPTION:
 ** Checks an attribute type is signed or unsigned
 **
 ** PARAMETERS:                  Name                    Usage
 ** teZCL_ZCLAttributeType  eAttributeDataType
 **
 ** RETURN:
 ** bool_t                       TRUE if its a signed type
 **
 ****************************************************************************/
PUBLIC bool_t bZCL_AttributeTypeIsSigned(
                teZCL_ZCLAttributeType     eAttributeDataType)
{
    if(eAttributeDataType >= E_ZCL_INT8 && eAttributeDataType <= E_ZCL_INT64)
    {
        return TRUE;
    }
    return FALSE;
}

/****************************************************************************
 **
 ** NAME:       bZCL_CheckAttributeInvalid
 **
 ** DESCRIPTION:
 ** checks to see if an attribute is an invalid value as specified in ZCL 2.5.2
 **
 ** PARAMETERS:                  Name                    Usage
 ** teZCL_ZCLAttributeType      eAttributeDataType      Attribute type
 ** void                        *pvAttribute            pointer to attribute
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status bZCL_CheckAttributeInvalid(
                teZCL_ZCLAttributeType     eAttributeDataType,
                void                      *pvAttribute)
{
    tuZCL_AttributeReportable uAttributeReportable;
    void *pvReportableChange;
    uint8 u8typeSize;

    eZCL_GetAttributeReportableEntry(&pvReportableChange, eAttributeDataType, &uAttributeReportable);

    vZCL_MarkAttributeInvalid(eAttributeDataType, pvReportableChange);

    eZCL_GetAttributeTypeSize(eAttributeDataType, &u8typeSize);

    if(memcmp(pvAttribute, pvReportableChange, u8typeSize)==0)
    {
        return(TRUE);
    }
    return(FALSE);
}

/****************************************************************************
 **
 ** NAME:       vZCL_MarkAttributeInvalid
 **
 ** DESCRIPTION:
 ** Sets an attribute to its invalid value as specified in ZCL 2.5.2
 **
 ** PARAMETERS:                  Name                    Usage
 ** teZCL_ZCLAttributeType      eAttributeDataType      Attribute type
 ** void                        *pvAttribute            pointer to attribute
 **
 ** RETURN:
 ** void
 **
 ****************************************************************************/

PUBLIC void vZCL_MarkAttributeInvalid(
                teZCL_ZCLAttributeType     eAttributeDataType,
                void                      *pvAttribute)
{

    uint8 au8SignedValue[]   = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8 au8UnsignedValue[8];
    uint8 u8typeSize;
    
    memset(au8UnsignedValue,0xFF,8);

    #ifdef STRICT_PARAM_CHECK
        /* Parameter checks */
        if(pvAttribute == NULL)
        {
            return;
        }
    #endif
    
    eZCL_GetAttributeTypeSize(eAttributeDataType, &u8typeSize);

    switch(eAttributeDataType)
    {
    case E_ZCL_FLOAT_SEMI:
    case E_ZCL_FLOAT_SINGLE:
    case E_ZCL_FLOAT_DOUBLE:
        au8UnsignedValue[0] = 0x7F;
    /* 0xf's */
    case E_ZCL_BOOL:
    case E_ZCL_UINT8:
    case E_ZCL_ENUM8:
    case E_ZCL_UINT16:
    case E_ZCL_ENUM16:
    case E_ZCL_CLUSTER_ID:
    case E_ZCL_ATTRIBUTE_ID:
    case E_ZCL_UINT24:
    case E_ZCL_UINT32:
    case E_ZCL_TOD:
    case E_ZCL_DATE:
    case E_ZCL_UTCT:
    case E_ZCL_BACNET_OID:
    case E_ZCL_UINT40:
    case E_ZCL_UINT48:
    case E_ZCL_UINT56:
    case E_ZCL_UINT64:
    case E_ZCL_IEEE_ADDR:
        memcpy(pvAttribute, au8UnsignedValue, u8typeSize);
        break;

        /* Big negative numbers */
    case E_ZCL_INT8:
    case E_ZCL_INT16:
    case E_ZCL_INT24:
    case E_ZCL_INT32:
    case E_ZCL_INT40:
    case E_ZCL_INT48:
    case E_ZCL_INT56:
    case E_ZCL_INT64:
        memcpy(pvAttribute, au8SignedValue, u8typeSize);
        break;

    /* Strings (0xf's) */
    case E_ZCL_OSTRING:
    	((tsZCL_OctetString*)pvAttribute)->u8Length = 0xFF;
        break;

    case E_ZCL_CSTRING:
    	((tsZCL_CharacterString*)pvAttribute)->u8Length = 0xFF;
        break;

    case E_ZCL_LOSTRING:
        ((tsZCL_LongOctetString*)pvAttribute)->u16Length = 0xFFFF;
        break;

    case E_ZCL_LCSTRING:
        ((tsZCL_LongCharacterString*)pvAttribute)->u16Length = 0xFFFF;
        break;

    case E_ZCL_ARRAY:
        ((tsZCL_Array*)pvAttribute)->u16Length = 0xFFFF;
        break;

    case E_ZCL_STRUCT:
        ((tsZCL_Structure*)pvAttribute)->u16Length = 0xFFFF;
        break;

    default:
        break;
    }
}
/****************************************************************************
 **
 ** NAME:       bZCL_CheckAttributeDirectionFlagMatch
 **
 ** DESCRIPTION:
 **
 **
 ** PARAMETERS:                 Name                    Usage
 ** tsZCL_AttributeDefinition  *psAttributeDefinition   Attribute data type struct
 ** bool_t                      bIsServer               Server Instance
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC bool_t bZCL_CheckAttributeDirectionFlagMatch(
                    tsZCL_AttributeDefinition  *psAttributeDefinition,
                    bool_t                      bIsServer)
{
    if(psAttributeDefinition==NULL)
    {
        return(FALSE);
    }
    // get attribute flag and set bool flag
    if( (psAttributeDefinition->u8AttributeFlags & E_ZCL_AF_GA) ||
       (bIsServer && !(psAttributeDefinition->u8AttributeFlags & E_ZCL_AF_CA)) ||
       (!bIsServer && (psAttributeDefinition->u8AttributeFlags & E_ZCL_AF_CA)) 
       )
    {
        return TRUE;
    }

    return(FALSE);
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/



