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
 * MODULE:             Scenes Cluster
 *
 * COMPONENT:          ScenesClusterManagement.h
 *
 * DESCRIPTION:        Scene cluster management functions
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>

#include "zps_apl.h"
#include "zps_apl_aib.h"

#include "zcl.h"
#include "zcl_internal.h"
#include "zcl_customcommand.h"



#include "Scenes.h"
#include "Scenes_internal.h"

#include "zcl_options.h"

#include "pdum_apl.h"
#include "zps_apl.h"
#include "zps_apl_af.h"

#include "string.h"

#if ((defined CLD_COLOUR_CONTROL) && (defined COLOUR_CONTROL_SERVER))
#include "ColourControl.h"
#include "LevelControl.h"
#include "OnOff.h"
#endif

#include "dbg.h"

#ifdef DEBUG_CLD_SCENES
#define TRACE_SCENES    TRUE
#else
#define TRACE_SCENES    FALSE
#endif

#ifdef SCENES_SERVER
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef union
{
    uint8   u8;
    uint16  u16;
    uint32  u32;
    uint64  u64;
    int8    i8;
    int16   i16;
    int32   i32;
    int64   i64;
} tuCLD_ScenesAttribute;

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE teZCL_Status eCLD_ScenesUpdateAttribute(
                                void *pvAttribute,
                                void *pvExtension,
                                uint16 u16Type,
                                uint16 u16Length,
                                uint32 u32Steps,
                                uint16 u16Enum);

#ifdef DEBUG_CLD_SCENES
PRIVATE void vCLD_ScenesShowValues(tuCLD_ScenesAttribute *puCurrent,
                                   tuCLD_ScenesAttribute *puTarget,
                                   tuCLD_ScenesAttribute *puNow,
                                   uint16 u16Size,
								   bool_t bIsSigned);
#endif

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
bool_t bFlag_SaveScenesNVM = FALSE;

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Public Functions                                              ***/
/****************************************************************************/
#ifdef SCENES_SERVER
/****************************************************************************
 **
 ** NAME:       eCLD_ScenesSaveClusters
 **
 ** DESCRIPTION:
 ** Saves all clusters on an endpoint to a buffer
 **
 ** PARAMETERS:                 Name                           Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition
 ** uint16                      u16MaxLength
 ** uint16                      *pu16Length
 ** uint8                       *pu8Buffer
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ScenesSaveClusters(tsZCL_EndPointDefinition *psEndPointDefinition,
                                                   uint16 u16MaxLength,
                                                   uint16 *pu16Length,
                                                   uint8 *pu8Buffer,
                                                   bool_t bIsTempory )
{

    int iCluster, iAttribute;
    uint8 *pu8WritePtr, *pu8Length, *pu8BufferStart, *pu8BufferEnd;
    uint16 u16AttributeIndex;
    uint16 u16AttributeType;
    uint8 u8AttributeLength;
    void *pvAttribute;
    teZCL_Status eStatus;
    tsZCL_AttributeDefinition   *psAttributeDefinition;
    tsZCL_ClusterInstance       *psClusterInstance;
    tsZCL_CallBackEvent         sZCL_CallBackEvent;

    #if ((defined CLD_COLOUR_CONTROL) && (defined COLOUR_CONTROL_SERVER))
        uint16 u16InvalidValue = 0x0000;
    #endif

    #ifdef STRICT_PARAM_CHECK
        if((psEndPointDefinition == NULL)   ||
           (pu16Length == NULL)             ||
           (pu8Buffer == NULL))
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif
    
    *pu16Length = 0;
    pu8WritePtr = pu8Buffer;
    bFlag_SaveScenesNVM = FALSE;

    /* Make a note of buffer start */
    pu8BufferStart = pu8Buffer;

    /* Work out where the end of the buffer is */
    pu8BufferEnd = pu8Buffer + u16MaxLength;

    DBG_vPrintf(TRACE_SCENES, "\nNum clusters = %d", psEndPointDefinition->u16NumberOfClusters);

    psClusterInstance = psEndPointDefinition->psClusterInstance;


    /* For each cluster on the endpoint */
    for(iCluster = 0; iCluster < psEndPointDefinition->u16NumberOfClusters; iCluster++)
    {

        /* Save cluster ID */
        DBG_vPrintf(TRACE_SCENES, "\nC%04x (%d Attributes)", psClusterInstance->psClusterDefinition->u16ClusterEnum,
                                                             psClusterInstance->psClusterDefinition->u16NumberOfAttributes);

        /* If there are scene extensions in this cluster, save them */
        if(psClusterInstance->psClusterDefinition->psSceneExtensionTable != NULL)
        {

            /* Need at least 3 bytes to store cluster ID and data length */
            if((pu8BufferEnd - pu8WritePtr) < 3)
            {
                return E_ZCL_ERR_INSUFFICIENT_SPACE;
            }

            /* Write in cluster Id */
            *pu8WritePtr++ = (uint8)psClusterInstance->psClusterDefinition->u16ClusterEnum;
            *pu8WritePtr++ = (uint8)(psClusterInstance->psClusterDefinition->u16ClusterEnum >> 8);

            /* Make a note of where the length byte will go then skip over it */
            pu8Length = pu8WritePtr;
            *pu8Length = 0;
            pu8WritePtr++;


            /* For each attribute in the scene extension table */
            for(iAttribute = 0; iAttribute < psClusterInstance->psClusterDefinition->psSceneExtensionTable->u16NumberOfAttributes; iAttribute++)
            {

                /* Find attribute definition */
                eStatus = eZCL_SearchForAttributeEntry(psEndPointDefinition->u8EndPointNumber,
                                                       psClusterInstance->psClusterDefinition->psSceneExtensionTable->au16Attributes[iAttribute].u16Attribute,
                                                       psClusterInstance->psClusterDefinition->bIsManufacturerSpecificCluster,
                                                       FALSE,
                                                       psClusterInstance,
                                                       &psAttributeDefinition,
                                                       &u16AttributeIndex);
                if(eStatus == E_ZCL_SUCCESS)
                {

                    pvAttribute         = pvZCL_GetAttributePointer(psAttributeDefinition, psClusterInstance, psClusterInstance->psClusterDefinition->psSceneExtensionTable->au16Attributes[iAttribute].u16Attribute);
                    u16AttributeType    = psAttributeDefinition->eAttributeDataType;

                    DBG_vPrintf(TRACE_SCENES, "\tAttrId%04x Type%02d", psAttributeDefinition->u16AttributeEnum,
                                                                       psAttributeDefinition->eAttributeDataType);

                    /* Find attribute data type length */
                    eStatus = eZCL_GetAttributeTypeSize(psAttributeDefinition->eAttributeDataType, &u8AttributeLength);

                    if(eStatus != E_ZCL_SUCCESS)
                    {
                        DBG_vPrintf(TRACE_SCENES, " Failed to get attribute length");
                        return eStatus;
                    }


                    DBG_vPrintf(TRACE_SCENES, " Len%d", u8AttributeLength);

                    /* If there's not enough room to store the attribute, exit */
                    if((pu8BufferEnd - pu8WritePtr) < u8AttributeLength)
                    {
                        DBG_vPrintf(TRACE_SCENES, " Not enough room to store attribute");
                        return E_ZCL_ERR_INSUFFICIENT_SPACE;
                    }

#ifdef DEBUG_CLD_SCENES
                    {
                        int n;

                        DBG_vPrintf(TRACE_SCENES, " Len%d", u8AttributeLength);

                        for(n = 0; n < u8AttributeLength; n++)
                        {
                            DBG_vPrintf(TRACE_SCENES, " %02x", *(uint8*)(pvAttribute + n));
                        }
                    }
#endif

                    /* Only one byte available for length of data in a cluster so make sure we don't overflow */
                    if(((uint16)*pu8Length + u8AttributeLength) > 0x00ff)
                    {
                        DBG_vPrintf(TRACE_SCENES, "\nLength field would overflow");
                        return E_ZCL_ERR_INSUFFICIENT_SPACE;
                    }

#if ((defined CLD_COLOUR_CONTROL) && (defined COLOUR_CONTROL_SERVER))
                    /* Filtering Colour Control Cluster Extension Fields based on Colour Mode
                     * |--------------------------------------------------------------------------------------------------------------|
                     * |             |                       Extension field values after filtering                                   |
                     * | Colour Mode |------------------------------------------------------------------------------------------------|
                     * |             | CurrentX | CurrentY | Hue | Saturation | LoopActive | LoopDirection | LoopTime |ColorTempMired |
                     * |----------------------------------------------------------------------------------------------|---------------|
                     * | XY          | *        | *        | 0   | 0          | 0          | 0             | 0        | FFFF          |
                     * | HSV         | 0        | 0        | *   | *          | *          | *             | *        | FFFF          |
                     * | Temperature | 0        | 0        | 0   | 0          | 0          | 0             | 0        | *             |
                     * |--------------------------------------------------------------------------------------------------------------|
                     *  Note1: '*' in above indicates the value of attributes at present. */
                     
                     /* HSV mode */
                        if((psClusterInstance->psClusterDefinition->u16ClusterEnum == LIGHTING_CLUSTER_ID_COLOUR_CONTROL) &&
                           (((tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr)->u8ColourMode == E_CLD_COLOURCONTROL_COLOURMODE_CURRENT_HUE_AND_CURRENT_SATURATION) &&
                           ((psAttributeDefinition->u16AttributeEnum == E_CLD_COLOURCONTROL_ATTR_CURRENT_X) ||
                            (psAttributeDefinition->u16AttributeEnum == E_CLD_COLOURCONTROL_ATTR_CURRENT_Y) ||
                            (psAttributeDefinition->u16AttributeEnum == E_CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED)))
                        {
                            if(psAttributeDefinition->u16AttributeEnum == E_CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED)
                            {
                                u16InvalidValue = 0xFFFF;
                            }
                            else
                            {
                                u16InvalidValue = 0;
                            }
                            pvAttribute = (void *) &u16InvalidValue;

                        }

                    /* X and Y mode */
                        if((psClusterInstance->psClusterDefinition->u16ClusterEnum == LIGHTING_CLUSTER_ID_COLOUR_CONTROL) &&
                           (((tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr)->u8ColourMode == E_CLD_COLOURCONTROL_COLOURMODE_CURRENT_X_AND_CURRENT_Y) &&
                           ((psAttributeDefinition->u16AttributeEnum == E_CLD_COLOURCONTROL_ATTR_ENHANCED_CURRENT_HUE) ||
                            (psAttributeDefinition->u16AttributeEnum == E_CLD_COLOURCONTROL_ATTR_CURRENT_SATURATION) ||
                            (psAttributeDefinition->u16AttributeEnum == E_CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_ACTIVE) ||
                            (psAttributeDefinition->u16AttributeEnum == E_CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_DIRECTION) ||
                            (psAttributeDefinition->u16AttributeEnum == E_CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_TIME) ||
                            (psAttributeDefinition->u16AttributeEnum == E_CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED)))
                        {
                            if(psAttributeDefinition->u16AttributeEnum == E_CLD_COLOURCONTROL_ATTR_COLOUR_TEMPERATURE_MIRED)
                            {
                                u16InvalidValue = 0xFFFF;
                            }
                            else
                            {
                                u16InvalidValue = 0;
                            }
                            pvAttribute = (void *) &u16InvalidValue;
                        }
                        
                    /* Temperature mode */                        
                        if((psClusterInstance->psClusterDefinition->u16ClusterEnum == LIGHTING_CLUSTER_ID_COLOUR_CONTROL) &&
                           (((tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr)->u8ColourMode == E_CLD_COLOURCONTROL_COLOURMODE_COLOUR_TEMPERATURE) &&
                           ((psAttributeDefinition->u16AttributeEnum == E_CLD_COLOURCONTROL_ATTR_ENHANCED_CURRENT_HUE) ||
                            (psAttributeDefinition->u16AttributeEnum == E_CLD_COLOURCONTROL_ATTR_CURRENT_SATURATION) ||
                            (psAttributeDefinition->u16AttributeEnum == E_CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_ACTIVE) ||
                            (psAttributeDefinition->u16AttributeEnum == E_CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_DIRECTION) ||
                            (psAttributeDefinition->u16AttributeEnum == E_CLD_COLOURCONTROL_ATTR_COLOUR_LOOP_TIME) ||
                            (psAttributeDefinition->u16AttributeEnum == E_CLD_COLOURCONTROL_ATTR_CURRENT_X) ||
                            (psAttributeDefinition->u16AttributeEnum == E_CLD_COLOURCONTROL_ATTR_CURRENT_Y)))
                        {
                            u16InvalidValue = 0;
                            pvAttribute = (void *) &u16InvalidValue;
                        }
#endif
                    /* Compare attributes in SceneTableEntry with attribute value to be written;
                       Update bFlag_SaveScenesNVM towards reducing EEPROM wear out. */
                    if(!bFlag_SaveScenesNVM)
                    {
                        switch(u8AttributeLength)
                        {
                            case 1:
                                if(*((uint8 *)(pu8WritePtr)) != *((uint8 *)(pvAttribute)))
                                {
                                    bFlag_SaveScenesNVM = TRUE;
                                }
                                break;
                            case 2:
                                if((*((uint8 *)(pu8WritePtr)) != *((uint8 *)(pvAttribute+1))) ||
                                   (*((uint8 *)(pu8WritePtr+1)) != *((uint8 *)(pvAttribute))))
                                {
                                    bFlag_SaveScenesNVM = TRUE;
                                }
                                break;
                            default:
                                DBG_vPrintf(TRACE_SCENES,"\n Attributes for SceneTableExtension are no more than 2B in size, so far!");
                                break;
                        }
                    }

                    /* Copy attribute data into buffer */
                    pu8WritePtr += u16ZCL_WriteTypeNBO(
                                        pu8WritePtr,
                                        u16AttributeType,
                                        pvAttribute);

                    /* Update length */
                    *pu8Length += u8AttributeLength;

                }
                /* For unsupported attributes that have to be present but ignored */
                else if (eStatus ==E_ZCL_ERR_ATTRIBUTE_NOT_FOUND)
                {
                  uint16 u16DefaultValue = 0x0000;
                  
                  /* Find attribute data type length */                  
                  eStatus = eZCL_GetAttributeTypeSize(psClusterInstance->psClusterDefinition->psSceneExtensionTable->au16Attributes[iAttribute].eAttributeDataType, &u8AttributeLength);
                  
                  if(eStatus != E_ZCL_SUCCESS)
                  {
                    DBG_vPrintf(TRACE_SCENES, " Failed to get attribute length");
                    return eStatus;
                  }
                  
                  /* If there's not enough room to store the attribute, exit */
                  if((pu8BufferEnd - pu8WritePtr) < u8AttributeLength)
                  {
                    DBG_vPrintf(TRACE_SCENES, " Not enough room to store attribute");
                    return E_ZCL_ERR_INSUFFICIENT_SPACE;
                  }
                  /* Only one byte available for length of data in a cluster so make sure we don't overflow */
                  if(((uint16)*pu8Length + u8AttributeLength) > 0x00ff)
                  {
                    DBG_vPrintf(TRACE_SCENES, "\nLength field would overflow");
                    return E_ZCL_ERR_INSUFFICIENT_SPACE;
                  }
                  
                  /* Copy default values into buffer for unsupported attributes */
                  /* No concern for endianness since only default values of 0 are written */
                  
                  memcpy(pu8WritePtr,(void*) &u16DefaultValue, u8AttributeLength);
                  
                  pu8WritePtr += u8AttributeLength;
                  
                  /* Update length */
                  *pu8Length += u8AttributeLength;                           
                }
                
            }

            /* If a scene event handler was registered, call it */
            if ((psClusterInstance->psClusterDefinition->psSceneExtensionTable->pSceneEventHandler != NULL)
                  &&  ( bIsTempory == FALSE))
            {
                psClusterInstance->psClusterDefinition->psSceneExtensionTable->pSceneEventHandler(E_ZCL_SCENE_EVENT_SAVE, psEndPointDefinition, psClusterInstance);
            }

        }

        psClusterInstance++;

    }

    *pu16Length = (pu8WritePtr - pu8BufferStart);

    DBG_vPrintf(TRACE_SCENES, "\nTotal length %d bytes", *pu16Length);
    if ( bIsTempory == FALSE)
    {
        /* Inform the application that the cluster has just been updated */
        sZCL_CallBackEvent.u8EndPoint = psEndPointDefinition->u8EndPointNumber;
        sZCL_CallBackEvent.psClusterInstance = psEndPointDefinition->psClusterInstance;
        sZCL_CallBackEvent.pZPSevent = NULL;
        sZCL_CallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_UPDATE;

        // call user for every cluster
        psEndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);
    }
    return E_ZCL_SUCCESS;

}


/****************************************************************************
 **
 ** NAME:       eCLD_ScenesRestoreClusters
 **
 ** DESCRIPTION:
 ** Saves all clusters on an endpoint to a buffer
 **
 ** PARAMETERS:                 Name                           Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition
 ** uint16                      u16Length
 ** uint8                       *pu8Buffer
 ** uint32                      u32RemainingSeconds
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ScenesRestoreClusters(tsZCL_EndPointDefinition *psEndPointDefinition,
                                                   uint16 u16Length,
                                                   uint8 *pu8Buffer,
                                                   uint32 u32RemainingTime)
{

    int iAttribute;
    uint8 *pu8ReadPtr, *pu8BufferEnd;
    uint16 u16BytesWritten;
    uint16 u16ClusterId;
    uint8  u8Length;
    //uint32 u32AttributeOffset;
    uint16 u16AttributeIndex;
    uint16 u16AttributeType;
    uint16 u16AttributeEnum;
    uint8  u8AttributeLength;
    void *pvAttribute;
    teZCL_Status eStatus;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_CallBackEvent sZCL_CallBackEvent;
    tsZCL_AttributeDefinition *psAttributeDefinition;
    
    #ifdef STRICT_PARAM_CHECK   
        if( (psEndPointDefinition == NULL)   ||
            (pu8Buffer == NULL))
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif
    
    /* If no data in extensions list, exit, not much we can do! */
    if(u16Length < 3)
    {
        return E_ZCL_SUCCESS;
    }

    /* Make a note of where the end of the scene extension data is */
    pu8BufferEnd = pu8Buffer + u16Length;

    /* Point to scene extensions */
    pu8ReadPtr = pu8Buffer;


    do
    {

        /* Get cluster id */
        pu8ReadPtr += u16ZCL_ReadTypeNBO(pu8ReadPtr, E_ZCL_UINT16, (uint8*)&u16ClusterId);

        /* Get scene extension length */
        u8Length = *pu8ReadPtr++;

        DBG_vPrintf(TRACE_SCENES, "\nCluster %04x Length %d", u16ClusterId, u8Length);

        /* Not written any bytes yet */
        u16BytesWritten = 0;

        /* If there is some scene extension data for this cluster */
        if(u8Length > 0)
        {
            /* Find cluster */
            eStatus = eZCL_SearchForClusterEntry(psEndPointDefinition->u8EndPointNumber,
                                                 u16ClusterId,
                                                 TRUE,
                                                 &psClusterInstance);
            /* If we found the cluster and it has a valid scene extension table */
            if((eStatus == E_ZCL_SUCCESS) && (psClusterInstance->psClusterDefinition->psSceneExtensionTable != NULL))
            {

                // get EP mutex
                #ifndef COOPERATIVE
                    eZCL_GetMutex(psEndPointDefinition);
                #endif



                /* For each attribute listed in the scene extension table */
                for(iAttribute = 0; iAttribute < psClusterInstance->psClusterDefinition->psSceneExtensionTable->u16NumberOfAttributes; iAttribute++)
                {

                    /* Search for attribute definition */
                    eStatus = eZCL_SearchForAttributeEntry(psEndPointDefinition->u8EndPointNumber,
                                                           psClusterInstance->psClusterDefinition->psSceneExtensionTable->au16Attributes[iAttribute].u16Attribute,
                                                           psClusterInstance->psClusterDefinition->bIsManufacturerSpecificCluster,
                                                           FALSE,
                                                           psClusterInstance,
                                                           &psAttributeDefinition,
                                                           &u16AttributeIndex);
                    if(eStatus == E_ZCL_SUCCESS)
                    {
                        /* Get attribute details */
                        u16AttributeEnum   = psAttributeDefinition->u16AttributeEnum;
                        u16AttributeType   = psAttributeDefinition->eAttributeDataType;
                        //u32AttributeOffset = psAttributeDefinition->u32OffsetFromStructBase;
                        pvAttribute        = pvZCL_GetAttributePointer(psAttributeDefinition, psClusterInstance, psClusterInstance->psClusterDefinition->psSceneExtensionTable->au16Attributes[iAttribute].u16Attribute);

                        /* Find out how big the attribute is then copy it to a temporary variable */
                        eStatus = eZCL_GetAttributeTypeSize(psAttributeDefinition->eAttributeDataType, &u8AttributeLength);
                        if(eStatus != E_ZCL_SUCCESS)
                        {
                            DBG_vPrintf(TRACE_SCENES, "\nError: eZCL_GetAttributeTypeSizeFromStructure returned %d", eStatus);
                            return eStatus;
                        }

                        eStatus = eCLD_ScenesUpdateAttribute(pvAttribute,
                                                             (void*)pu8ReadPtr,
                                                             u16AttributeType,
                                                             (uint16)u8AttributeLength,
                                                             u32RemainingTime,
                                                             u16AttributeEnum);
                        if(eStatus != E_ZCL_SUCCESS)
                        {
                            return eStatus;
                        }

                        pu8ReadPtr += u8AttributeLength;


                        /* If no more scene extension data for this cluster, exit loop */
                        u16BytesWritten += u8AttributeLength;
                        if(u16BytesWritten >= u8Length)
                        {
                            break;
                        }

                    }

                }

                        // release EP
                        #ifndef COOPERATIVE
                            eZCL_ReleaseMutex(psEndPointDefinition);
                        #endif


                /* If a scene event handler was registered, call it */
                if(psClusterInstance->psClusterDefinition->psSceneExtensionTable->pSceneEventHandler != NULL)
                        {
                    psClusterInstance->psClusterDefinition->psSceneExtensionTable->pSceneEventHandler(E_ZCL_SCENE_EVENT_RECALL, psEndPointDefinition, psClusterInstance);
                        }
                else
                {
                    /* Inform the application that the cluster has just been updated */
                    sZCL_CallBackEvent.u8EndPoint = psEndPointDefinition->u8EndPointNumber;
                    sZCL_CallBackEvent.psClusterInstance = psClusterInstance;
                    sZCL_CallBackEvent.pZPSevent = NULL;
                    sZCL_CallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_UPDATE;

                    // call user for every cluster
                    psEndPointDefinition->pCallBackFunctions(&sZCL_CallBackEvent);
                    }

                }

            }

    } while (pu8ReadPtr < pu8BufferEnd);


    return E_ZCL_SUCCESS;

}



/****************************************************************************
 **
 ** NAME:       vCLD_ScenesUpdateSceneValid
 **
 ** DESCRIPTION:
 ** Checks if a scene is valid and if the attribute in the table match
 ** the attributes on the outputs
 **
 ** PARAMETERS:                 Name                           Usage
 ** tsZCL_EndPointDefinition    *psEndPointDefinition

 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  void vCLD_ScenesUpdateSceneValid(
                            tsZCL_EndPointDefinition    *psEndPointDefinition)
{
    tsZCL_ClusterInstance       *psSceneClusterInstance;
    tsCLD_Scenes*                psScenesSharedStruct;
    tsCLD_ScenesTableEntry *     psTableEntry;
    teZCL_Status                 eStatus;
    uint8                       au8SceneData[CLD_SCENES_MAX_SCENE_STORAGE_BYTES];
    uint16                      u16DataLength;


#ifdef STRICT_PARAM_CHECK
    if (psEndPointDefinition == NULL)
    {
        return;
    }
#endif


    /* Find scenes cluster */
    eStatus = eZCL_SearchForClusterEntry( psEndPointDefinition->u8EndPointNumber,
                                          GENERAL_CLUSTER_ID_SCENES,
                                          TRUE,
                                          &psSceneClusterInstance);

    if (eStatus != E_ZCL_SUCCESS)
    {
        return;
    }

    psScenesSharedStruct = (tsCLD_Scenes*)psSceneClusterInstance->pvEndPointSharedStructPtr;

    DBG_vPrintf(TRACE_SCENES, "SCN: scene %d group %04x Valid %d\n", psScenesSharedStruct->u8CurrentScene,
                                                                     psScenesSharedStruct->u16CurrentGroup,
                                                                     psScenesSharedStruct->bSceneValid);

    if (psScenesSharedStruct->bSceneValid)
    {
        psTableEntry = NULL;
        eStatus = eCLD_ScenesFindActiveScene( psEndPointDefinition,
                                              psSceneClusterInstance,
                                              &psTableEntry);
        if (eStatus != E_ZCL_SUCCESS)
        {
            DBG_vPrintf(TRACE_SCENES, "no scene table entry  St %02x\n", eStatus );
            return;
        }

        if (psTableEntry->bInTransit == FALSE)
        {
            int index;

            /* create dummy entry based on current state of attributes */
            memset(au8SceneData, 0, CLD_SCENES_MAX_SCENE_STORAGE_BYTES);
            eCLD_ScenesSaveClusters( psEndPointDefinition,
                                     CLD_SCENES_MAX_SCENE_STORAGE_BYTES,
                                     &u16DataLength,
                                     au8SceneData,
                                     TRUE);

            for (index=0; index<u16DataLength; index++)
            {
                /* copmare the saved table to the one just created */
                if ( psTableEntry->au8SceneData[index] != au8SceneData[index] )
                {
                    DBG_vPrintf(TRACE_SCENES, "idx = %d table %02x  Temp %02x\n", index, psTableEntry->au8SceneData[index], au8SceneData[index]);
                    /* some thing changed so set Vaild and Active to False */
                    psScenesSharedStruct->bSceneValid = FALSE;
                    psTableEntry->bActive = FALSE;
                    break;
                }
            }

        }else { DBG_vPrintf(TRACE_SCENES, "in transist leave alone\n"); }

    }
    return ;

}


/****************************************************************************/
/***        Private Functions                                             ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eCLD_ScenesUpdateAttribute
 **
 ** DESCRIPTION:
 ** Update Scene Attribute
 **
 ** PARAMETERS:                 Name                           Usage
 ** void                        *pvAttribute
 ** void                        *pvExtension
 ** uint16                      u16Type
 ** tsZCL_EndPointDefinition    *psEndPointDefinition
 ** uint16                      u16Length
 ** uint32                      u32Steps
 ** uint16                      u16Enum
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ScenesUpdateAttribute(
                                void *pvAttribute,
                                void *pvExtension,
                                uint16 u16Type,
                                uint16 u16Length,
                                uint32 u32Steps,
                                uint16 u16Enum)
{

    int64 i64Step;
    uint64 u64Step;
#ifdef DEBUG_CLD_SCENES
    tuCLD_ScenesAttribute uOriginal;
#endif
    tuCLD_ScenesAttribute uCurrent;
    tuCLD_ScenesAttribute uTarget;
    uint8 au8RealAttributeLength[] = {sizeof(zuint8),
                                      sizeof(zuint16),
                                      sizeof(zuint24),
                                      sizeof(zuint32),
                                      sizeof(zuint40),
                                      sizeof(zuint48),
                                      sizeof(zuint56),
                                      sizeof(uint64),
                                      };

    /* Parameter checks */
    if((pvAttribute == NULL)    ||
       (pvExtension == NULL)    ||
       (u16Length == 0))
    {
        return E_ZCL_ERR_PARAMETER_NULL;
    }

    DBG_vPrintf(TRACE_SCENES, "\nUpdate Attribute Enum=%04d Type=%02d Len=%02d Steps=%02d", u16Enum, u16Type, u16Length, u32Steps);

    /* Can only deal with values up to 64 bits in length */
    if(u16Length > sizeof(uint64))
    {
        DBG_vPrintf(TRACE_SCENES, " Invalid value");
        return E_ZCL_ERR_INVALID_VALUE;
    }

    uCurrent.u64 = (uint64)0LL;
    uTarget.u64 = (uint64)0LL;

    /* Get current attribute value into union */
    memcpy(&uCurrent.u8, pvAttribute, au8RealAttributeLength[u16Length - 1]);

    /* Get the new target attribute value into union */
    u16ZCL_ReadTypeNBO(pvExtension, u16Type, (uint8*)&uTarget.u8);
#ifdef DEBUG_CLD_SCENES
    uOriginal.u64 = uCurrent.u64;
#endif
    /* If attribute is already at the target value, exit */
    if(uTarget.u64 == uCurrent.u64)
    {
#ifdef DEBUG_CLD_SCENES
        vCLD_ScenesShowValues(&uOriginal, &uTarget, &uCurrent, u16Length, bZCL_AttributeTypeIsSigned(u16Type));
#endif
        return E_ZCL_SUCCESS;
    }

    /* Do we need to do signed arithmetic ? */
    if(bZCL_AttributeTypeIsSigned(u16Type))
    {
        if(u32Steps == 0)
        {
            uCurrent.i64 = uTarget.i64;
        }
        else
        {
            /* Do we need to increment or decrement the current value */
            if(uTarget.i64 > uCurrent.i64)
            {
                /* Increment required */
                i64Step = (uTarget.i64 - uCurrent.i64) / (int64)u32Steps;
                uCurrent.i64 += i64Step;
            }
            else
            {
                /* Decrement required */
                i64Step = (uCurrent.i64 - uTarget.i64) / (int64)u32Steps;
                uCurrent.i64 -= i64Step;
            }
        }
    }
    else
    {
        if(u32Steps == 0)
        {
            uCurrent.u64 = uTarget.u64;
        }
        else
        {
            /* Do we need to increment or decrement the current value */
            if(uTarget.u64 > uCurrent.u64)
            {
                /* Increment required */
                u64Step = (uTarget.u64 - uCurrent.u64) / (uint64)u32Steps;
                uCurrent.u64 += u64Step;
            }
            else
            {
                /* Decrement required */
                u64Step = (uCurrent.u64 - uTarget.u64) / (uint64)u32Steps;
                uCurrent.u64 -= u64Step;
            }
        }
    }

#ifdef DEBUG_CLD_SCENES
    vCLD_ScenesShowValues(&uOriginal, &uTarget, &uCurrent, u16Length, bZCL_AttributeTypeIsSigned(u16Type));
#endif

    /* Write back new attribute value from union */
    memcpy(pvAttribute, &uCurrent, au8RealAttributeLength[u16Length - 1]);


    return E_ZCL_SUCCESS;

}

#ifdef DEBUG_CLD_SCENES
PRIVATE void vCLD_ScenesShowValues(tuCLD_ScenesAttribute *puCurrent,
                                   tuCLD_ScenesAttribute *puTarget,
                                   tuCLD_ScenesAttribute *puNow,
                                   uint16 u16Size,
								   bool_t bIsSigned)
{

    if(bIsSigned == TRUE)
    {

        DBG_vPrintf(TRACE_SCENES, "Signed ");

        switch(u16Size)
        {

        case 1:
            DBG_vPrintf(TRACE_SCENES, "Current:%i Target:%i Now:%i", puCurrent->i8, puTarget->i8, puNow->i8);
            break;

        case 2:
            DBG_vPrintf(TRACE_SCENES, "Current:%i Target:%i Now:%i", puCurrent->i16, puTarget->i16, puNow->i16);
            break;

        case 3:
        case 4:
            DBG_vPrintf(TRACE_SCENES, "Current:%i Target:%i Now:%i", puCurrent->i32, puTarget->i32, puNow->i32);
            break;

        case 5:
        case 6:
        case 7:
        case 8:
            DBG_vPrintf(TRACE_SCENES, "Current:%lli Target:%lli Now:%lli", puCurrent->i64, puTarget->i64, puNow->i64);
            break;

        }

    }
    else
    {

        DBG_vPrintf(TRACE_SCENES, "Unsigned ");

        switch(u16Size)
        {

        case 1:
            DBG_vPrintf(TRACE_SCENES, "Current:%i Target:%i Now:%i", puCurrent->u8, puTarget->u8, puNow->u8);
            break;

        case 2:
            DBG_vPrintf(TRACE_SCENES, "Current:%i Target:%i Now:%i", puCurrent->u16, puTarget->u16, puNow->u16);
            break;

        case 3:
        case 4:
            DBG_vPrintf(TRACE_SCENES, "Current:%i Target:%i Now:%i", puCurrent->u32, puTarget->u32, puNow->u32);
            break;

        case 5:
        case 6:
        case 7:
        case 8:
            DBG_vPrintf(TRACE_SCENES, "Current:%lli Target:%lli Now:%lli", puCurrent->u64, puTarget->u64, puNow->u64);
            break;

        }

    }

}
#endif
#endif
#endif /* SCENES_SERVER */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
