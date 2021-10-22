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
 * COMPONENT:          zcl_PDUbufferReadWriteString.c
 *
 * DESCRIPTION:       Modified Zigbee stack PDU buffer read and write functions
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

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
#define STRING_LENGTH_ILLEGAL_8     (0xFF)
#define STRING_LENGTH_ILLEGAL_16    (0xFFFF)

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

PRIVATE void vZCL_SetStringStructure(
                    teZCL_ZCLAttributeType      eAttributeDataType,
                    void                       *pvStringStruct,
                    uint16                      u16stringDataLength,
                    uint8                     **ppu8stringBuffer);

#ifdef LPSW3909
PRIVATE void vZCL_WriteStructureErrorCode(
                    teZCL_ZCLAttributeType      eAttributeDataType,
                    void                       *pvStringStruct);
#endif

PRIVATE uint16 u16ZCL_WriteStringTypeNBO(
                    uint8                      *pu8Data,
                    teZCL_ZCLAttributeType      eAttributeDataType,
                    void                       *pvStringStruct);

PRIVATE uint16 u16ZCL_ReadStringTypeNBO(
                    uint8                      *pu8Data,
                    teZCL_ZCLAttributeType      eAttributeDataType,
                    void                       *pvStringStruct,
                    uint16                      u16structSize);

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
 ** NAME:       u16ZCL_APduInstanceWriteStringNBO
 **
 ** DESCRIPTION:
 ** Writes a String Attribute Value into a PDU buffer
 **
 ** PARAMETERS:                  Name               Usage
 ** PDUM_thAPduInstance          hAPduInst          PDU buffer
 ** uint16                       u16Pos             position in buffer
 ** teZCL_ZCLAttributeType       eAttributeDataType Attribute type
 ** void                        *pvStringStructure  String Data to write
 **
 ** RETURN:
 ** uint8 - attribute type size
 **
 ****************************************************************************/

PUBLIC uint16 u16ZCL_APduInstanceWriteStringNBO(
                                    PDUM_thAPduInstance         hAPduInst,
                                    uint16                      u16Pos,
                                    teZCL_ZCLAttributeType      eAttributeDataType,
                                    void                       *pvStringStructure)
{
    uint16 u16stringSize;
    uint8 *pu8buffer;
    uint16 u16stringDataLength;
    uint16 u16stringLengthField;

    if((hAPduInst) && (pvStringStructure))
    {
        uint16 u16APduSize = PDUM_u16APduGetSize(PDUM_thAPduInstanceGetApdu(hAPduInst));
        // get start
        uint8 *pu8Start = (uint8 *)(PDUM_pvAPduInstanceGetPayload(hAPduInst)) + u16Pos;
        // get string size
        u16stringSize = u16ZCL_GetStringSizeAndBufferFromStructure(eAttributeDataType,
                                                                   pvStringStructure,
                                                                   &u16stringDataLength,
                                                                   &u16stringLengthField,
                                                                   &pu8buffer);
        // check that whole string can fit into the buffer
        if((u16stringSize == 0) || (u16stringSize>u16APduSize))
        {
            uint8 u8StringLengthSize =1;
            *pu8Start++ = 0;
            if((E_ZCL_LOSTRING == eAttributeDataType) || (E_ZCL_LCSTRING == eAttributeDataType))
            {
                u8StringLengthSize = 2;
                *pu8Start++ = 0;
            }

             return u8StringLengthSize;
        }

        // write to buffer
        if ((u16Pos + u16stringSize - 1) < u16APduSize) {

            // write type, return size
            return u16ZCL_WriteStringTypeNBO(pu8Start, eAttributeDataType, pvStringStructure);
        }
    }

    return 0;
}

/****************************************************************************
 **
 ** NAME:       u8ZCL_APduInstanceReadStringNBO
 **
 ** DESCRIPTION:
 ** Reads a String Attribute Value from a PDU buffer
 **
 ** PARAMETERS:                  Name               Usage
 ** PDUM_thAPduInstance          hAPduInst          PDU buffer
 ** uint16                       u16Pos             position in buffer
 ** teZCL_ZCLAttributeType       eAttributeDataType Attribute type
 ** void                        *pvStruct           Struct to read data into
 **
 ** RETURN:
 ** uint8 - attribute type size
 **
 ****************************************************************************/

PUBLIC uint16 u16ZCL_APduInstanceReadStringNBO(
                                    PDUM_thAPduInstance         hAPduInst,
                                    uint16                      u16Pos,
                                    teZCL_ZCLAttributeType      eAttributeDataType,
                                    void                        *pvStringStructure)
{
    uint16 u16stringSize, u16structSize;
    uint16 u16stringDataLength;
    uint8 *pu8stringData;
    uint16 u16stringDataLengthField;

    if((hAPduInst) && (pvStringStructure)) {
        uint16 u16APduSize = PDUM_u16APduInstanceGetPayloadSize(hAPduInst);

        // get string size
        u16stringSize = u16ZCL_GetStringAttributeTypeSizeFromBuffer(eAttributeDataType,
                                                                    &u16stringDataLength,
                                                                    &u16stringDataLengthField,
                                                                    &pu8stringData,
                                                                    (uint8 *)(PDUM_pvAPduInstanceGetPayload(hAPduInst)) + u16Pos);

        // check structure we are writing into is valid
        u16structSize = u16ZCL_GetStringStructureSize(eAttributeDataType, pvStringStructure);


        // if the string in the pdu is longer than the buffer - truncate the string (SE profile messaging test  13.4.3)
        #ifdef LPSW3909
        if(u16structSize == 0)
        {
            // write error code into buffer
            vZCL_WriteStructureErrorCode(eAttributeDataType, pvStringStructure);
            return 0;
        }
        #endif

        // read from buffer
        if ((u16Pos + u16stringSize - 1) < u16APduSize) {
            // get start
            uint8 *pu8Start = (uint8 *)(PDUM_pvAPduInstanceGetPayload(hAPduInst)) + u16Pos;
            // write type, return size
            return u16ZCL_ReadStringTypeNBO(pu8Start, eAttributeDataType, pvStringStructure, u16structSize);
        }
    }

    return 0;
}

/****************************************************************************
 **
 ** NAME:       u16ZCL_WriteStringTypeNBO
 **
 ** DESCRIPTION:
 ** Writes a string data type in NBO into a buffer
 **
 ** PARAMETERS:                  Name               Usage
 ** uint8                       *pu8Data            buffer
 ** teZCL_ZCLAttributeType       eAttributeDataType Attribute type
 ** void                        *pvData             data to write
 **
 ** RETURN:
 ** uint8 - attribute type size
 **
 ****************************************************************************/

PUBLIC uint16 u16ZCL_WriteStringTypeNBO(
                    uint8                      *pu8Data,
                    teZCL_ZCLAttributeType      eAttributeDataType,
                    void                       *pvStringStruct)
{
    int i;
    uint8 *pu8Start = pu8Data;
    uint8 *pu8stringData;
    uint16 u16stringDataLength;
    uint16 u16stringLengthField;

    // initial checks
    if(!pu8Data || (((uint8*)pvStringStruct)==NULL) )
    {
        return 0;
    }

    u16ZCL_GetStringSizeAndBufferFromStructure(eAttributeDataType, pvStringStruct, &u16stringDataLength, &u16stringLengthField, &pu8stringData);

    // write in length of string
    switch(eAttributeDataType)
    {
        case(E_ZCL_OSTRING):
        case(E_ZCL_CSTRING):
        {
            *pu8Data++ = (uint8)u16stringLengthField;
            break;
        }
        case(E_ZCL_LOSTRING):
        case(E_ZCL_LCSTRING):
        {
            *pu8Data++ = (uint8)u16stringLengthField;
            *pu8Data++ = (u16stringLengthField >> 8);
            break;
        }

        // unrecognised case
        default:
        {
            return 0;
            break;
        }
    }

    // write string into message - any string errors return u16stringDataLength as zero
    for(i=0;i<u16stringDataLength;i++)
    {
        *pu8Data++ = pu8stringData[i];
    }

    return (uint16)(pu8Data - pu8Start);
}

/****************************************************************************
 **
 ** NAME:       u16ZCL_ReadStringTypeNBO
 **
 ** DESCRIPTION:
 ** Reads a string data type in NBO from a buffer
 **
 ** PARAMETERS:                  Name               Usage
 ** uint8                       *pu8Data            buffer
 ** teZCL_ZCLAttributeType       eAttributeDataType Attribute type
 ** void                        *pvData             data to write
 ** uint16                      u16structSize       maximum bytes to write
 **
 ** RETURN:
 ** uint8 - attribute type size
 **
 ****************************************************************************/

PRIVATE uint16 u16ZCL_ReadStringTypeNBO(
                    uint8                      *pu8Data,
                    teZCL_ZCLAttributeType      eAttributeDataType,
                    void                       *pvStringStruct,
                    uint16                      u16structSize)
{
    int i;
    uint8 *pu8stringData, *pu8stringBuffer = 0;

    uint16 u16length;
    uint16 u16stringDataLength;
    uint16 u16stringDataLengthField;

    // initial checks
    if(!pu8Data || (((uint8*)pvStringStruct)==NULL))
    {
        return 0;
    }

    u16length = u16ZCL_GetStringAttributeTypeSizeFromBuffer(eAttributeDataType, &u16stringDataLength, &u16stringDataLengthField, &pu8stringData, pu8Data);

    // if the string in the pdu is longer than the buffer - truncate the string (SE profile messaging test  13.4.3)
    if (u16stringDataLength > u16structSize)
    {
        u16stringDataLength = u16structSize;
        u16stringDataLengthField = u16structSize;
    }

    // set structure
    vZCL_SetStringStructure(eAttributeDataType, pvStringStruct, u16stringDataLengthField, &pu8stringBuffer);

    // read string
    if ( pu8stringBuffer != NULL )
    {
        for(i=0;i<u16stringDataLength; i++)
        {
            pu8stringBuffer[i] = *pu8stringData++;
        }
    }

    // length = data length + number of bytes used to represent the string length in the message
    // Return the number of bytes in the PDU NOT the number of bytes actually written
    // because we need to know were we are in the PDU.
    return u16length;
}

/****************************************************************************
 **
 ** NAME:       u16ZCL_GetStringSizeAndBufferFromStructure
 **
 ** DESCRIPTION:
 ** Gets the string data size and buffer from the user structure
 **
 ** PARAMETERS:                  Name                   Usage
 ** teZCL_ZCLAttributeType       eAttributeDataType     Attribute type
 ** void                        *pvStringStruct         Stringdata to write
 ** uint16                      *pu16stringDataLength   length of string alone
 ** uint16                      *pu16stringLengthField  value of the string length field
 ** uint8                      **ppu8stringData         Buffer contained in structure
 **
 ** RETURN:
 ** uint8 - string size to be written to buffer - length field + data
 **
 ****************************************************************************/

PUBLIC uint16 u16ZCL_GetStringSizeAndBufferFromStructure(
                        teZCL_ZCLAttributeType eAttributeDataType,
                        void                   *pvStringStruct,
                        uint16                 *pu16stringDataLength,
                        uint16                 *pu16stringLengthField,
                        uint8                 **ppu8stringData)
{

    uint16 u16length, u16MaxLength = 0,u16ActualLength = 0,u16IllegalStringLength = 0;
    *pu16stringDataLength=0;
    *ppu8stringData=NULL;

    #ifdef STRICT_PARAM_CHECK
        // initial checks
        if((((uint8*)pvStringStruct)==NULL) || (pu16stringDataLength==NULL) || (ppu8stringData==NULL))
        {
            return 0;
        }
        
        if (eAttributeDataType < E_ZCL_OSTRING || eAttributeDataType > E_ZCL_LCSTRING)
        {
            *pu16stringLengthField = STRING_LENGTH_ILLEGAL_8;
            u16length=1;
            *pu16stringDataLength=0;
            return 0;
        }
    #endif
    switch(eAttributeDataType)
    {
        case(E_ZCL_OSTRING):
        {
            // unpack structure
            *pu16stringDataLength = ((tsZCL_OctetString *)pvStringStruct)->u8Length;
            u16MaxLength = ((tsZCL_OctetString *)pvStringStruct)->u8MaxLength;
            *ppu8stringData = ((tsZCL_OctetString *)pvStringStruct)->pu8Data;
            u16ActualLength = 1;
            u16IllegalStringLength = STRING_LENGTH_ILLEGAL_8;
            break;
        }
        case(E_ZCL_CSTRING):
        {
            // unpack structure
            *pu16stringDataLength = ((tsZCL_CharacterString *)pvStringStruct)->u8Length;
            u16MaxLength = ((tsZCL_CharacterString *)pvStringStruct)->u8MaxLength;
            *ppu8stringData = ((tsZCL_CharacterString *)pvStringStruct)->pu8Data;
            u16ActualLength = 1;
            u16IllegalStringLength = STRING_LENGTH_ILLEGAL_8;
            break;
        }
        case(E_ZCL_LOSTRING):
        {
            // unpack structure
            *pu16stringDataLength = ((tsZCL_LongOctetString *)pvStringStruct)->u16Length;
            u16MaxLength = ((tsZCL_LongCharacterString *)pvStringStruct)->u16MaxLength;
            *ppu8stringData = ((tsZCL_LongOctetString *)pvStringStruct)->pu8Data;
            u16ActualLength = 2;
            u16IllegalStringLength = STRING_LENGTH_ILLEGAL_16;
            break;
        }
        case(E_ZCL_LCSTRING):
        {
            // unpack structure
            *pu16stringDataLength = ((tsZCL_LongCharacterString *)pvStringStruct)->u16Length;
            u16MaxLength = ((tsZCL_LongCharacterString *)pvStringStruct)->u16MaxLength;
            *ppu8stringData = ((tsZCL_LongCharacterString *)pvStringStruct)->pu8Data;
            u16ActualLength = 2;
            u16IllegalStringLength = STRING_LENGTH_ILLEGAL_16;
            break;
        }
        
        default:
            break;
    }
 
    // interpret length wrt error code
    *pu16stringLengthField = *pu16stringDataLength;
    u16length = (*pu16stringDataLength)+u16ActualLength;
    // detect errors
    // string specified as illegal length or structure dimensions are wacked
    if((*pu16stringDataLength==u16IllegalStringLength) || (*pu16stringDataLength >u16MaxLength))
    {
        *pu16stringLengthField = u16IllegalStringLength;
        *pu16stringDataLength = 0;
        u16length=u16ActualLength;
    }
        
    // invalid structure checks
    if((u16length==0) || (*ppu8stringData==NULL))
    {
       u16length=0;
       *pu16stringDataLength = 0;
       return 0;
    }

    return u16length;
}

/****************************************************************************
 **
 ** NAME:       u8ZCL_GetStringAttributeTypeSizeFromBuffer
 **
 ** DESCRIPTION:
 ** Gets Attribute type size
 **
 ** PARAMETERS:              Name                       Usage
 ** teZCL_ZCLAttributeType   eAttributeDataType         Attribute data type
 ** uint16                  *pu16stringDataLength       length of string alone
 ** uint16                  *pu16stringDataLengthField  Value of the string length field
 ** uint8                  **ppu8stringData             Buffer contained in structure
 ** uint8                   *pu8stringLengthPosition    string length position in buffer
 ** RETURN:
 ** uint16 u16stringSize
 **
 ****************************************************************************/

PUBLIC uint16 u16ZCL_GetStringAttributeTypeSizeFromBuffer(
                        teZCL_ZCLAttributeType      eAttributeDataType,
                        uint16                     *pu16stringDataLength,
                        uint16                     *pu16stringDataLengthField,
                        uint8                     **ppu8stringData,
                        uint8                      *pu8stringLengthPosition)
{

    uint16 u16stringSize;
#ifdef STRICT_PARAM_CHECK
    // initial checks
    if((pu16stringDataLength==NULL) || (ppu8stringData==NULL) || (pu8stringLengthPosition==NULL))
    {
        return 0;
    }
    
    if (eAttributeDataType < E_ZCL_OSTRING || eAttributeDataType > E_ZCL_LCSTRING)
    {
        u16stringSize = 0;
        *pu16stringDataLength =0;
        *pu16stringDataLengthField = 0;
        *ppu8stringData = NULL;
    }
#endif

    if (eAttributeDataType == E_ZCL_OSTRING || eAttributeDataType == E_ZCL_CSTRING)
    {
        u16stringSize = *pu8stringLengthPosition +1;
        *pu16stringDataLengthField = *pu8stringLengthPosition;
        *pu16stringDataLength = *pu8stringLengthPosition;
        *ppu8stringData = &pu8stringLengthPosition[1];
        if((*pu16stringDataLengthField==STRING_LENGTH_ILLEGAL_8) || (*pu16stringDataLengthField==0))
        {
            u16stringSize=1;
            *pu16stringDataLength =0;
            *ppu8stringData = NULL;
        }
    }
    else
    {
        u16stringSize  = *pu8stringLengthPosition;
        u16stringSize |= *(pu8stringLengthPosition+1) << 8;
        *pu16stringDataLength = u16stringSize;
        *pu16stringDataLengthField = *pu16stringDataLength;
        u16stringSize +=2;
        *ppu8stringData = &pu8stringLengthPosition[2];
        if((*pu16stringDataLengthField==STRING_LENGTH_ILLEGAL_16) || (*pu16stringDataLengthField==0))
        {
            u16stringSize=2;
            *pu16stringDataLength =0;
            *ppu8stringData = NULL;
        }
    }

    return(u16stringSize);
}

/****************************************************************************
 **
 ** NAME:       vZCL_SetStringStructure
 **
 ** DESCRIPTION:
 ** Sets the User string structure when a string is being written into it
 **
 ** PARAMETERS:              Name                    Usage
 ** teZCL_ZCLAttributeType   eAttributeDataType      Attribute type
 ** void                    *pvStringStruct          String struct to write
 ** uint16                   u16stringDataLength     Length of string alone
 ** uint8                  **ppu8stringBuffer        String Buffer
 **
 ** RETURN:
 ** nothing
 **
 ****************************************************************************/

PRIVATE void vZCL_SetStringStructure(
                        teZCL_ZCLAttributeType      eAttributeDataType,
                        void                       *pvStringStruct,
                        uint16                      u16stringDataLength,
                        uint8                     **ppu8stringBuffer)
{

    tsZCL_OctetString *psZCL_OctetString;
    tsZCL_CharacterString *psZCL_CharacterString;
    tsZCL_LongOctetString *psZCL_LongOctetString;
    tsZCL_LongCharacterString *psZCL_LongCharacterString;

    switch(eAttributeDataType)
    {
        // strings - length determined in actual string
        case(E_ZCL_OSTRING):
        {
            psZCL_OctetString = ((tsZCL_OctetString *)pvStringStruct);
            psZCL_OctetString->u8Length = (uint8)u16stringDataLength;
            *ppu8stringBuffer = psZCL_OctetString->pu8Data;
            break;
        }

        case(E_ZCL_CSTRING):
        {
            psZCL_CharacterString = ((tsZCL_CharacterString *)pvStringStruct);
            psZCL_CharacterString->u8Length = (uint8)u16stringDataLength;
            *ppu8stringBuffer = psZCL_CharacterString->pu8Data;
            break;
        }
        case(E_ZCL_LOSTRING):
        {
            psZCL_LongOctetString = ((tsZCL_LongOctetString *)pvStringStruct);
            psZCL_LongOctetString->u16Length = u16stringDataLength;
            *ppu8stringBuffer = psZCL_LongOctetString->pu8Data;
            break;
        }

        case(E_ZCL_LCSTRING):
        {
            psZCL_LongCharacterString = ((tsZCL_LongCharacterString *)pvStringStruct);
            psZCL_LongCharacterString->u16Length = u16stringDataLength;
            *ppu8stringBuffer = psZCL_LongCharacterString->pu8Data;
            break;
        }

        default:
        {
            // unrecognised
            break;
        }
    }
}

/****************************************************************************
 **
 ** NAME:       eZCL_CheckUserStringStructureRead
 **
 ** DESCRIPTION:
 ** Checks user string structure validity before writing a string into buffer
 **
 ** PARAMETERS:              Name                    Usage
 ** teZCL_ZCLAttributeType   eAttributeDataType      Attribute type
 ** void                    *pvStringStruct          String struct to write
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eZCL_CheckUserStringStructureRead(
                        teZCL_ZCLAttributeType      eAttributeDataType,
                        void                       *pvStringStruct)
{

    tsZCL_OctetString *psZCL_OctetString;
    tsZCL_CharacterString *psZCL_CharacterString;
    tsZCL_LongOctetString *psZCL_LongOctetString;
    tsZCL_LongCharacterString *psZCL_LongCharacterString;

    switch(eAttributeDataType)
    {
        // strings - length determined in actual string
        case(E_ZCL_OSTRING):
        {
            psZCL_OctetString = ((tsZCL_OctetString *)pvStringStruct);
            // check paramaters passed in
            if(((psZCL_OctetString->u8MaxLength < psZCL_OctetString->u8Length) ||
                (psZCL_OctetString->pu8Data == NULL) )                         &&
                (psZCL_OctetString->u8Length != STRING_LENGTH_ILLEGAL_8)
            )
            {
                return(E_ZCL_FAIL);
            }
            break;
        }

        case(E_ZCL_CSTRING):
        {
            psZCL_CharacterString = ((tsZCL_CharacterString *)pvStringStruct);
            // check paramaters passed in
            if(((psZCL_CharacterString->u8MaxLength < psZCL_CharacterString->u8Length) ||
                (psZCL_CharacterString->pu8Data == NULL))                              &&
                (psZCL_CharacterString->u8Length != STRING_LENGTH_ILLEGAL_8)
            )
            {
                return(E_ZCL_FAIL);
            }
            break;
        }
        case(E_ZCL_LOSTRING):
        {
            psZCL_LongOctetString = ((tsZCL_LongOctetString *)pvStringStruct);
            // check paramaters passed in
            if(((psZCL_LongOctetString->u16MaxLength < psZCL_LongOctetString->u16Length) ||
                (psZCL_LongOctetString->pu8Data == NULL))                              &&
                (psZCL_LongOctetString->u16Length != STRING_LENGTH_ILLEGAL_16)
            )
            {
                return(E_ZCL_FAIL);
            }
            break;
        }

        case(E_ZCL_LCSTRING):
        {
            psZCL_LongCharacterString = ((tsZCL_LongCharacterString *)pvStringStruct);
            // check paramaters passed in
            if(((psZCL_LongCharacterString->u16MaxLength < psZCL_LongCharacterString->u16Length) ||
               (psZCL_LongCharacterString->pu8Data == NULL))                              &&
                (psZCL_LongCharacterString->u16Length != STRING_LENGTH_ILLEGAL_16)
            )
            {
                return(E_ZCL_FAIL);
            }
            break;
        }

        default:
        {
            // unrecognised
            return(E_ZCL_FAIL);
            break;
        }
    }

    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       u16ZCL_GetStringStructureSize
 **
 ** DESCRIPTION:
 ** Gets string structure size
 **
 ** PARAMETERS:              Name                    Usage
 ** teZCL_ZCLAttributeType   eAttributeDataType      Attribute type
 ** uint16                   u16Length               Length of data to write
 ** void                    *pvStringStruct          String struct to write
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC uint16 u16ZCL_GetStringStructureSize(
                        teZCL_ZCLAttributeType      eAttributeDataType,
                        void                       *pvStringStruct)
{

    tsZCL_OctetString *psZCL_OctetString;
    tsZCL_CharacterString *psZCL_CharacterString;
    tsZCL_LongOctetString *psZCL_LongOctetString;
    tsZCL_LongCharacterString *psZCL_LongCharacterString;

    switch(eAttributeDataType)
    {
        case(E_ZCL_OSTRING):
        {
            psZCL_OctetString = ((tsZCL_OctetString *)pvStringStruct);
            if(psZCL_OctetString->pu8Data == NULL)
            {
                return 0;
            }
            return(psZCL_OctetString->u8MaxLength);
            break;
        }

        case(E_ZCL_CSTRING):
        {
            psZCL_CharacterString = ((tsZCL_CharacterString *)pvStringStruct);
            if(psZCL_CharacterString->pu8Data == NULL)
            {
                return 0;
            }
            return(psZCL_CharacterString->u8MaxLength);
            break;
        }
        case(E_ZCL_LOSTRING):
        {
            psZCL_LongOctetString = ((tsZCL_LongOctetString *)pvStringStruct);
            if(psZCL_LongOctetString->pu8Data == NULL)
            {
                return 0;
            }
            return(psZCL_LongOctetString->u16MaxLength);
            break;
        }

        case(E_ZCL_LCSTRING):
        {
            psZCL_LongCharacterString = ((tsZCL_LongCharacterString *)pvStringStruct);
            if(psZCL_LongCharacterString->pu8Data == NULL)
            {
                return 0;
            }
            return(psZCL_LongCharacterString->u16MaxLength);
            break;
        }

        default:
        {
            // unrecognised
            return(0);
            break;
        }
    }
}


/****************************************************************************
 **
 ** NAME:       eZCL_GetStringLengthFieldSize
 **
 ** DESCRIPTION:
 ** Gets the number of bytes that make up the string length field
 **
 ** PARAMETERS:              Name                    Usage
 ** teZCL_ZCLAttributeType   eAttributeDataType      Attribute type
 ** void                    *pu8typeSize             String length
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC teZCL_Status eZCL_GetStringLengthFieldSize(
                            teZCL_ZCLAttributeType  eAttributeDataType,
                            uint8                  *pu8typeSize)
{

    if(pu8typeSize==NULL)
    {
        return(E_ZCL_FAIL);
    }

    // get length size
    switch(eAttributeDataType)
    {
        // add attribute types with handlers here which are variable length
        case(E_ZCL_OSTRING):
        case(E_ZCL_CSTRING):
        {
            *pu8typeSize=1;
            break;
        }
        case(E_ZCL_LOSTRING):
        case(E_ZCL_LCSTRING):
        {
            *pu8typeSize=2;
            break;
        }
        default:
        {
            *pu8typeSize=0;
            return(E_ZCL_FAIL);
            break;
        }
    }

    return(E_ZCL_SUCCESS);
}

/****************************************************************************
 **
 ** NAME:       vZCL_WriteStructureErrorCode
 **
 ** DESCRIPTION:
 ** writes type dependant length error code into string structure
 **
 ** PARAMETERS:              Name                    Usage
 ** teZCL_ZCLAttributeType   eAttributeDataType      Attribute type
 ** void                    *pvStringStruct          String structure
 **
 ** RETURN:
 ** nothing
 **
 ****************************************************************************/
#ifdef LPSW3909
PRIVATE void vZCL_WriteStructureErrorCode(
                        teZCL_ZCLAttributeType      eAttributeDataType,
                        void                       *pvStringStruct)
{

    tsZCL_OctetString *psZCL_OctetString;
    tsZCL_CharacterString *psZCL_CharacterString;
    tsZCL_LongOctetString *psZCL_LongOctetString;
    tsZCL_LongCharacterString *psZCL_LongCharacterString;

    switch(eAttributeDataType)
    {
        // strings - length determined in actual string
        case(E_ZCL_OSTRING):
        {
            psZCL_OctetString = ((tsZCL_OctetString *)pvStringStruct);
            psZCL_OctetString->u8Length=STRING_LENGTH_ILLEGAL_8;
            break;
        }

        case(E_ZCL_CSTRING):
        {
            psZCL_CharacterString = ((tsZCL_CharacterString *)pvStringStruct);
            psZCL_CharacterString->u8Length=STRING_LENGTH_ILLEGAL_8;
            break;
        }

        case(E_ZCL_LOSTRING):
        {
            psZCL_LongOctetString = ((tsZCL_LongOctetString *)pvStringStruct);
            psZCL_LongOctetString->u16Length=STRING_LENGTH_ILLEGAL_16;
            break;
        }

        case(E_ZCL_LCSTRING):
        {
            psZCL_LongCharacterString = ((tsZCL_LongCharacterString *)pvStringStruct);
            psZCL_LongCharacterString->u16Length=STRING_LENGTH_ILLEGAL_16;
            break;
        }

        default:
        {
            // unrecognised
            break;
        }
    }

}
#endif
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

