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
 * MODULE:             Over The Air Upgrade
 *
 * COMPONENT:          ota.c
 *
 * DESCRIPTION:        Over The Air Upgrade
 *
 *****************************************************************************/
/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>
#include "zcl.h"
#include "zcl_options.h"
#include "OTA.h"
#include "OTA_private.h"
#include "dbg.h"

#ifndef OTA_NO_CERTIFICATE
#include "eccapi.h"
#include "sbmce_cb.h"
#endif
#include "aessw_ccm.h"
#include "rnd_pub.h"
#ifndef TRACE_OTA_DEBUG
#define TRACE_OTA_DEBUG FALSE
#endif
#if (defined JENNIC_CHIP_FAMILY_JN518x)
#include "fsl_aes.h"
#ifdef WEAK
#undef WEAK
#endif
#include "rom_psector.h"
#include "rom_api.h"
#endif
#ifdef FSL_RTOS_FREE_RTOS
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#endif
#ifdef LITTLE_ENDIAN_PROCESSOR
    #include "portmacro.h"
#endif

#ifdef SOTA_ENABLED
#include "blob_utils.h"
#endif

#ifdef CLD_OTA
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
#ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED
    const tsZCL_CommandDefinition asCLD_OTAClusterCommandDefinitions[] = {
        {E_CLD_OTA_COMMAND_IMAGE_NOTIFY,                      E_ZCL_CF_TX},
        {E_CLD_OTA_COMMAND_QUERY_NEXT_IMAGE_REQUEST,          E_ZCL_CF_RX},
        {E_CLD_OTA_COMMAND_QUERY_NEXT_IMAGE_RESPONSE,         E_ZCL_CF_TX},
        {E_CLD_OTA_COMMAND_BLOCK_REQUEST,                     E_ZCL_CF_RX},
        {E_CLD_OTA_COMMAND_PAGE_REQUEST,                      E_ZCL_CF_RX},
        {E_CLD_OTA_COMMAND_BLOCK_RESPONSE,                    E_ZCL_CF_TX},
        {E_CLD_OTA_COMMAND_UPGRADE_END_REQUEST,               E_ZCL_CF_RX},
        {E_CLD_OTA_COMMAND_UPGRADE_END_RESPONSE,              E_ZCL_CF_TX},
        {E_CLD_OTA_COMMAND_QUERY_SPECIFIC_FILE_REQUEST,       E_ZCL_CF_RX},
        {E_CLD_OTA_COMMAND_QUERY_SPECIFIC_FILE_RESPONSE,      E_ZCL_CF_TX},
        {E_CLD_OTA_COMMAND_QUERY_NEXT_IMAGE_RESPONSE_ERROR,   E_ZCL_CF_TX} 
    };
#endif

const tsZCL_AttributeDefinition asOTAClusterAttributeDefinitions[] = {
#ifdef OTA_CLIENT
        /* ZigBee Cluster Library Version */
        {E_CLD_OTA_ATTR_UPGRADE_SERVER_ID,                E_ZCL_AF_RD | E_ZCL_AF_CA,    E_ZCL_IEEE_ADDR,  (uint32)(&((tsCLD_AS_Ota*)(0))->u64UgradeServerID), 0},             /* Mandatory */

    #ifdef OTA_CLD_ATTR_FILE_OFFSET
        {E_CLD_OTA_ATTR_FILE_OFFSET,                      E_ZCL_AF_RD | E_ZCL_AF_CA,    E_ZCL_UINT32,     (uint32)(&((tsCLD_AS_Ota*)(0))->u32FileOffset), 0},                   /* Optional */
    #endif

    #ifdef OTA_CLD_ATTR_CURRENT_FILE_VERSION
        {E_CLD_OTA_ATTR_CURRENT_FILE_VERSION,             E_ZCL_AF_RD | E_ZCL_AF_CA,    E_ZCL_UINT32,    (uint32)(&((tsCLD_AS_Ota*)(0))->u32CurrentFileVersion), 0},               /* Optional */
    #endif

    #ifdef OTA_CLD_ATTR_CURRENT_ZIGBEE_STACK_VERSION
        {E_CLD_OTA_ATTR_CURRENT_ZIGBEE_STACK_VERSION,     E_ZCL_AF_RD | E_ZCL_AF_CA,    E_ZCL_UINT16,    (uint32)(&((tsCLD_AS_Ota*)(0))->u16CurrentStackVersion), 0},     /* Optional */
    #endif

    #ifdef OTA_CLD_ATTR_DOWNLOADED_FILE_VERSION
        {E_CLD_OTA_ATTR_DOWNLOADED_FILE_VERSION,          E_ZCL_AF_RD | E_ZCL_AF_CA,    E_ZCL_UINT32,    (uint32)(&((tsCLD_AS_Ota*)(0))->u32DownloadedFileVersion), 0},        /* Optional */
    #endif

    #ifdef OTA_CLD_ATTR_DOWNLOADED_ZIGBEE_STACK_VERSION
        {E_CLD_OTA_ATTR_DOWNLOADED_ZIGBEE_STACK_VERSION,  E_ZCL_AF_RD | E_ZCL_AF_CA,    E_ZCL_UINT16,    (uint32)(&((tsCLD_AS_Ota*)(0))->u16DownloadedStackVersion), 0},  /* Optional */
    #endif

        {E_CLD_OTA_ATTR_IMAGE_UPGRADE_STATUS,             E_ZCL_AF_RD | E_ZCL_AF_CA,    E_ZCL_ENUM8,     (uint32)(&((tsCLD_AS_Ota*)(0))->u8ImageUpgradeStatus), 0},    /* Mandatory */

    #ifdef OTA_CLD_ATTR_MANF_ID
        {E_CLD_OTA_ATTR_MANF_ID,                          E_ZCL_AF_RD | E_ZCL_AF_CA,    E_ZCL_UINT16,    (uint32)(&((tsCLD_AS_Ota*)(0))->u16ManfId), 0},        /* Optional */
    #endif

    #ifdef OTA_CLD_ATTR_IMAGE_TYPE
        {E_CLD_OTA_ATTR_IMAGE_TYPE,                       E_ZCL_AF_RD | E_ZCL_AF_CA,    E_ZCL_UINT16,    (uint32)(&((tsCLD_AS_Ota*)(0))->u16ImageType), 0},  /* Optional */
    #endif

    #ifdef OTA_CLD_ATTR_REQUEST_DELAY
        {E_CLD_OTA_ATTR_REQUEST_DELAY,                    E_ZCL_AF_RD | E_ZCL_AF_CA,    E_ZCL_UINT16,    (uint32)(&((tsCLD_AS_Ota*)(0))->u16MinBlockRequestDelay), 0},  /* Optional */
    #endif

    #ifdef OTA_CLD_ATTR_IMAGE_STAMP
        {E_CLD_OTA_ATTR_IMAGE_STAMP,                       E_ZCL_AF_RD | E_ZCL_AF_CA,    E_ZCL_UINT32,    (uint32)(&((tsCLD_AS_Ota*)(0))->u32ImageStamp), 0},  /* Optional */
    #endif

#endif
        {E_CLD_GLOBAL_ATTR_ID_FEATURE_MAP,                (E_ZCL_AF_RD|E_ZCL_AF_GA),  E_ZCL_BMAP32,   (uint32)(&((tsCLD_AS_Ota*)(0))->u32FeatureMap),0},   /* Mandatory  */ 


        {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,           E_ZCL_AF_RD | E_ZCL_AF_GA,    E_ZCL_UINT16,    (uint32)(&((tsCLD_AS_Ota*)(0))->u16ClusterRevision),0},   /* Mandatory  */
};

tsZCL_ClusterDefinition sCLD_OTA = {
        OTA_CLUSTER_ID,
        FALSE,
#ifndef OTA_NO_CERTIFICATE
        E_ZCL_SECURITY_APPLINK,
#else
        E_ZCL_SECURITY_NETWORK,
#endif
        (sizeof(asOTAClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
        (tsZCL_AttributeDefinition*)asOTAClusterAttributeDefinitions,
        NULL
        #ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED        
            ,
            (sizeof(asCLD_OTAClusterCommandDefinitions) / sizeof(tsZCL_CommandDefinition)),
            (tsZCL_CommandDefinition*)asCLD_OTAClusterCommandDefinitions
        #endif        
};

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
#ifdef OTA_SERVER
tsOTA_ImageHeader sOTAHeader[OTA_MAX_CO_PROCESSOR_IMAGES];
#endif

#ifdef OTA_CLIENT
#if (OTA_MAX_CO_PROCESSOR_IMAGES != 0)
uint8  asCommonCoProcessorOTAHeader[OTA_MAX_CO_PROCESSOR_IMAGES][OTA_MAX_HEADER_SIZE] ;
bool_t bIsCoProcessorImgUpgdDependent;
#endif
#endif

tsNvmDefs  sNvmDefsStruct;

uint8 au8OtaHeader[OTA_MAX_HEADER_SIZE] __attribute__ ((section (".ro_ota_header"))) =
       {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

extern uint16 u16ImageStartSector;
/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
#ifdef OTA_UPGRADE_VOLTAGE_CHECK
bool_t bOta_LowVoltage=FALSE;
#endif
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
#if (defined JENNIC_CHIP_FAMILY_JN518x)
extern void __disable_irq(void);
#endif

#ifdef INTERNAL_ENCRYPTED
#ifndef OTA_USE_KEY_IN_PSECTOR
extern uint32 g_au32OtaEncCred [4];
#endif

 unsigned char cOTA_atoh (unsigned char data)
 { if (data > '9') 
    { data += 9;
    }
    return (data &= 0x0F);
 }
 #endif
/****************************************************************************
 **
 ** NAME:       eOTA_Create
 **
 ** DESCRIPTION:
 ** Creates OTA cluster instance on device
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsZCL_ClusterInstance       *psClusterInstance          Cluster structure
 ** bool_t                       bIsServer                  Is server instance
 ** tsZCL_ClusterDefinition     *psClusterDefinition        Cluster definition
 ** void                        *pvEndPointSharedStructPtr  shared data pointer
 ** uint8                        u8Endpoint                 Endpoint Id
 ** uint8                       *pu8AttributeControlBits    attribute Control bits
 ** tsOTA_Common                *psCustomDataStruct         custom data pointer
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_Create(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8                               u8Endpoint,
                uint8                              *pu8AttributeControlBits,
                tsOTA_Common                       *psCustomDataStruct)
{
    teZCL_Status eZCLstatus = E_ZCL_SUCCESS;
    tsReg128 sOtaUseKey = eOTA_retOtaUseKey();

    #ifdef STRICT_PARAM_CHECK 
        if((psClusterInstance==NULL)            ||
           (psClusterDefinition==NULL)         ||
           (psCustomDataStruct == NULL))
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif
    
    #ifdef INTERNAL_ENCRYPTED
    const char* sString = CLD_OTA_KEY;
    uint32 i;
    for(i = 0; i < 4; i++)
    {
        g_au32OtaEncCred[ i ] = (( cOTA_atoh ( sString [ ( i * 8 ) ] )<< 4) & 0xF0 ) | 
                                ( ( cOTA_atoh ( sString [ ( i * 8 ) + 1 ]) ) & 0x0F) |
                                ( ( cOTA_atoh ( sString [ ( i * 8 ) + 2 ]) << 12) & 0xF000) |
                                ( ( cOTA_atoh ( sString [ ( i * 8 ) + 3 ]) << 8 ) & 0xF00) |
                                ( ( cOTA_atoh ( sString [ ( i * 8 ) + 4 ]) << 20) & 0xF00000) |
                                ( ( cOTA_atoh ( sString [ ( i * 8 ) + 5 ]) << 16) & 0xF0000) |
                                ( ( cOTA_atoh ( sString [ ( i * 8 ) + 6 ]) << 28) & 0xF0000000) |
                                ( ( cOTA_atoh ( sString [ ( i * 8 ) + 7 ]) << 24) & 0xF000000);

    }
    #endif

    memset(&psCustomDataStruct->sOTACallBackMessage.uMessage,0,sizeof(psCustomDataStruct->sOTACallBackMessage.uMessage));
    psCustomDataStruct->sReceiveEventAddress.bInitialised = FALSE;
    // cluster data
    vZCL_InitializeClusterInstance(
                                   psClusterInstance, 
                                   bIsServer,
                                   psClusterDefinition,
                                   pvEndPointSharedStructPtr,
                                   pu8AttributeControlBits,
                                   NULL,
                                   eOtaCommandHandler);        

    psClusterInstance->pvEndPointCustomStructPtr = psCustomDataStruct;
    psCustomDataStruct->sOTACustomCallBackEvent.eEventType = E_ZCL_CBET_CLUSTER_CUSTOM;
    psCustomDataStruct->sOTACustomCallBackEvent.psClusterInstance = psClusterInstance;
    psCustomDataStruct->sOTACustomCallBackEvent.u8EndPoint = u8Endpoint;
    psCustomDataStruct->sOTACustomCallBackEvent.uMessage.sClusterCustomMessage.u16ClusterId = OTA_CLUSTER_ID;
    psCustomDataStruct->sOTACustomCallBackEvent.uMessage.sClusterCustomMessage.pvCustomData = (void*)&psCustomDataStruct->sOTACallBackMessage;

#ifdef OTA_CLIENT
    psCustomDataStruct->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime = 0;
    vOtaInitStateMachine(psCustomDataStruct);
    eZCLstatus = eOtaRegisterTimeServer();

    #ifdef APP0 /* Building with selective OTA */
        psCustomDataStruct->sOTACallBackMessage.sPersistedData.bStackDownloadActive = FALSE;
    #endif
    #ifdef SOTA_ENABLED
        psCustomDataStruct->sOTACallBackMessage.sPersistedData.blobId = 0x0;
    #endif
#endif
    ((tsCLD_AS_Ota*)pvEndPointSharedStructPtr)->u32FeatureMap = CLD_OTA_FEATURE_MAP;
    ((tsCLD_AS_Ota*)pvEndPointSharedStructPtr)->u16ClusterRevision = CLD_OTA_CLUSTER_REVISION;
    
#ifdef OTA_SERVER   
    #ifdef OTA_PAGE_REQUEST_SUPPORT
        eZCLstatus = eOtaRegisterTimeServer();
    #endif
#endif

    DBG_vPrintf(TRACE_OTA_DEBUG, "eFuse key 0x%08x, 0x%08x, 0x%08x, 0x%08x\n",
            sOtaUseKey.u32register0,
            sOtaUseKey.u32register1,
            sOtaUseKey.u32register2,
            sOtaUseKey.u32register3);

    return eZCLstatus;
}
/****************************************************************************
 **
 ** NAME:       eOTA_OtaOffset
 **
 ** DESCRIPTION:
 ** Calculates the offset of ota image
 **
 ** PARAMETERS:
 ** None
 ** RETURN:
 ** uint32
 **
 ****************************************************************************/
PUBLIC uint32 eOTA_OtaOffset(void)
{
    uint32 u32OtaOffset;

    u32OtaOffset = ((uint8*)&_FlsOtaHeader) - ((uint8*)&_flash_start);
    return u32OtaOffset;
}

#ifdef SOTA_ENABLED
PUBLIC void vOTA_SetBlobToDownload(uint8 u8Endpoint, uint16_t blobId)
{
    teZCL_Status eZCL_Status;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;
    if((eZCL_Status =
        eOtaFindCluster(u8Endpoint,
                         &psEndPointDefinition,
                           &psClusterInstance,
                           &psCustomData,
                           FALSE))
                           == E_ZCL_SUCCESS)
    {
        psCustomData->sOTACallBackMessage.sPersistedData.blobId  = blobId;
    }
}
#endif

#ifdef APP0
/****************************************************************************
 **
 ** NAME:       vOTA_SetApp1OtaEnable
 **
 ** DESCRIPTION:
 ** Enables downloading APP1.
 **
 ** PARAMETERS:
 ** pointer to tsOTA_Common
 ** RETURN:
 ** None
 **
 ****************************************************************************/
PUBLIC void vOTA_SetApp1OtaEnable(uint8            u8Endpoint,
                                  bool_t           bState )
{
    teZCL_Status eZCL_Status;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;
    if((eZCL_Status =
        eOtaFindCluster(u8Endpoint,
                         &psEndPointDefinition,
                           &psClusterInstance,
                           &psCustomData,
                           FALSE))
                           == E_ZCL_SUCCESS)
    {
        psCustomData->sOTACallBackMessage.sPersistedData.bStackDownloadActive  = bState;
    }
}
#endif
/****************************************************************************
 **
 ** NAME:       eOTA_retOtaUseKey
 **
 ** DESCRIPTION:
 ** provide key for ota
 **
 ** PARAMETERS:
 ** None
 ** RETURN:
 ** tsReg128
 **
 ****************************************************************************/
PUBLIC tsReg128 eOTA_retOtaUseKey(void)
{
    tsReg128 sOtaUseKey = {0};

#ifndef OTA_UNIT_TEST_FRAMEWORK

#if (defined JENNIC_CHIP_FAMILY_JN517x) || (defined JENNIC_CHIP_FAMILY_JN516x)
    uint32 *pu32KeyPtr = (uint32*)FL_INDEX_SECTOR_ENC_KEY_ADDRESS;

    sOtaUseKey.u32register0 = *pu32KeyPtr;
    sOtaUseKey.u32register1 = *(pu32KeyPtr+1);
    sOtaUseKey.u32register2 = *(pu32KeyPtr+2);
    sOtaUseKey.u32register3 = *(pu32KeyPtr+3);
#endif

#endif

    return sOtaUseKey;
}

/****************************************************************************
 **
 ** NAME:       eOTA_VerifyImage
 **
 ** DESCRIPTION:
 ** Verify downloaded image
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                     u8Endpoint                 Endpoint Id
 ** bool_t                      bIsServer                  Is server instance
 ** uint8                     u8ImageLocation            Image location
 ** bool_t                      bHeaderPresent             Is header present in image
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_VerifyImage(
                                uint8   u8Endpoint,
                                bool_t    bIsServer,
                                uint8   u8ImageLocation,
                                bool_t    bHeaderPresent)
{
    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;
    uint16 u16tagId;
    uint8 au8Certificate[OTA_SIGNING_CERT_SIZE];
     uint32 u32ImageSize,u32CurrentOffset,u32TagLength,u32FlashOffset ;
     uint8 au8Tag[OTA_TAG_HEADER_SIZE];
#ifndef OTA_NO_CERTIFICATE
    uint32 i;
    uint8 au8Signature[OTA_SIGNITURE_SIZE], au8PublicKey[22] = {0},au8Mac[8];
    uint64 u64Mac,u64Cert;
#endif
    AESSW_Block_u uHash;
    uint8 au8Header[OTA_MAX_HEADER_SIZE];
    uint16 u16HeaderSize = OTA_MIN_HEADER_SIZE;

    if((eStatus = eOtaFindCluster(u8Endpoint,
                         &psEndPointDefinition,
                           &psClusterInstance,
                           &psCustomData,
                           bIsServer))
                           == E_ZCL_SUCCESS)
    {

        vOTA_GenerateHash(psEndPointDefinition, psCustomData, bIsServer, bHeaderPresent, &uHash, u8ImageLocation);
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x) || (defined APP0)
        u32FlashOffset = (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8ImageLocation] *
                        sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION ;
#else
        u32FlashOffset = 0;
#endif
        if(bHeaderPresent)
        {
            /* First read Min Header Size */
            sNvmDefsStruct.sOtaFnTable.prReadCb(u32FlashOffset, OTA_MIN_HEADER_SIZE, au8Header);

            /* Calculate Header Size */
            vReverseMemcpy((uint8*)&u16HeaderSize,&au8Header[6],sizeof(uint16));

            /* Read Reamining Header Bytes */
            sNvmDefsStruct.sOtaFnTable.prReadCb((u32FlashOffset + OTA_MIN_HEADER_SIZE), (u16HeaderSize - OTA_MIN_HEADER_SIZE), (au8Header + OTA_MIN_HEADER_SIZE));
        }
        else
        {
            /* Server/Client */
            if(bIsServer)
            {
#ifdef OTA_SERVER
                u16HeaderSize = psCustomData->sOTACallBackMessage.au8ServerOTAHeader[7] | psCustomData->sOTACallBackMessage.au8ServerOTAHeader[6];
                memcpy(au8Header, psCustomData->sOTACallBackMessage.au8ServerOTAHeader, u16HeaderSize);
#endif
            }
            else
            {
#ifdef OTA_CLIENT
                u16HeaderSize = psCustomData->sOTACallBackMessage.sPersistedData.au8Header[7] | psCustomData->sOTACallBackMessage.sPersistedData.au8Header[6];
                memcpy(au8Header, psCustomData->sOTACallBackMessage.sPersistedData.au8Header, u16HeaderSize);
#endif
            }
        }

        vReverseMemcpy((uint8*)&u32ImageSize,&au8Header[52],sizeof(uint32));

        u32ImageSize -= OTA_SIGNITURE_SIZE;

        if(bHeaderPresent)
        {
            u32CurrentOffset = u32FlashOffset + u32ImageSize -  (2*OTA_TAG_HEADER_SIZE) - OTA_SIGNING_CERT_SIZE - OTA_MAC_ADDRESS_SIZE;
        }
        else
        {
            u32CurrentOffset = u32FlashOffset + u32ImageSize -  u16HeaderSize - (3*OTA_TAG_HEADER_SIZE) - OTA_SIGNING_CERT_SIZE - OTA_MAC_ADDRESS_SIZE;
        }
        vOtaFlashLockRead(psEndPointDefinition, psCustomData,u32CurrentOffset,OTA_TAG_HEADER_SIZE, au8Tag);
        vOTA_GetTagIdandLengh(&u16tagId,&u32TagLength,au8Tag);

        if(u16tagId == OTA_TAG_ID_EDCA_CERT &&
                u32TagLength == OTA_SIGNING_CERT_SIZE)
        {
            u32CurrentOffset += OTA_TAG_HEADER_SIZE;
            vOtaFlashLockRead(psEndPointDefinition, psCustomData,u32CurrentOffset,OTA_SIGNING_CERT_SIZE, au8Certificate);
 #ifndef OTA_NO_CERTIFICATE
            /* Construct the public key from the device's certificate */
            if( ZSE_ECQVReconstructPublicKey(au8Certificate,
                                            psCustomData->sOTACallBackMessage.au8CAPublicKey,
                                            au8PublicKey,
                                            SBMCE_iAesMmo,
                                            SBMCE_iYield,
                                            0) == MCE_SUCCESS)
            {
                DBG_vPrintf(TRACE_OTA_DEBUG, "\nPublic Key :");
                for (i=0; i < 22; i++)
                    DBG_vPrintf(TRACE_OTA_DEBUG, "%x", au8PublicKey[i]);

                u32CurrentOffset += OTA_SIGNING_CERT_SIZE;
                vOtaFlashLockRead(psEndPointDefinition, psCustomData,u32CurrentOffset,OTA_TAG_HEADER_SIZE, au8Tag);
                vOTA_GetTagIdandLengh(&u16tagId,&u32TagLength,au8Tag);

                if(u16tagId == OTA_TAG_ID_EDCA_SIGNATURE && u32TagLength == (OTA_SIGNITURE_SIZE+8))
                {
                    uint32 u32HighValue, u32LowValue;
                    u32CurrentOffset += (OTA_TAG_HEADER_SIZE);
                    vOtaFlashLockRead(psEndPointDefinition, psCustomData,u32CurrentOffset,8, au8Mac);
                    /* ZCL_OPT_2 Not sure about the change */
                    vReverseMemcpy((uint8*)&u64Cert,&au8Certificate[22],sizeof(uint64));
                    memcpy(&u64Mac,&au8Mac[0],sizeof(uint64));
                                        
                    DBG_vPrintf(TRACE_OTA_DEBUG,"mac cert %llx , mac header %llx \n", u64Cert, u64Mac);
                    if(u64Cert == u64Mac)
                    {
                        /* Mac address in certificate and signer address field is same, now give the call back
                         *  to application to verify that OTA image came from trusted source */
                        psCustomData->sOTACallBackMessage.uMessage.sSignerMacVerify.eMacVerifyStatus = E_ZCL_SUCCESS;
                        psCustomData->sOTACallBackMessage.uMessage.sSignerMacVerify.u64SignerMac = u64Mac;
                        eOtaSetEventTypeAndGiveCallBack(psCustomData, E_CLD_OTA_INTERNAL_COMMAND_VERIFY_SIGNER_ADDRESS,psEndPointDefinition);
                        if(E_ZCL_SUCCESS == psCustomData->sOTACallBackMessage.uMessage.sSignerMacVerify.eMacVerifyStatus)
                        {
                            u32CurrentOffset += 8;
                            vOtaFlashLockRead(psEndPointDefinition, psCustomData,u32CurrentOffset,OTA_SIGNITURE_SIZE, au8Signature);
                            DBG_vPrintf(TRACE_OTA_DEBUG, "\n Hash Value :");
                            for (i=0; i < AESSW_BLK_SIZE; i++)
                                DBG_vPrintf(TRACE_OTA_DEBUG, "%x", uHash.au8[i]);

                            DBG_vPrintf(TRACE_OTA_DEBUG, "\n Signature :");
                            for (i=0; i < OTA_SIGNITURE_SIZE; i++)
                                DBG_vPrintf(TRACE_OTA_DEBUG, "%x", au8Signature[i]);

                            if(ZSE_ECDSAVerify(au8PublicKey,
                                            uHash.au8,
                                            au8Signature,
                                            &au8Signature[OTA_SIGNITURE_SIZE/2],
                                           SBMCE_iYield,
                                           0) != MCE_SUCCESS)
                            {
                                DBG_vPrintf(TRACE_OTA_DEBUG, "ERR1\n");
                                eStatus = E_ZCL_FAIL;
                            }
                        }
                        else
                        {
                            DBG_vPrintf(TRACE_OTA_DEBUG, "ERR6\n");
                            eStatus = E_ZCL_FAIL;
                        }
                    }
                    else
                    {
                        DBG_vPrintf(TRACE_OTA_DEBUG, "ERR2\n");
                        eStatus = E_ZCL_FAIL;
                    }
                }
                else
                {
                    DBG_vPrintf(TRACE_OTA_DEBUG, "ERR3\n");
                    eStatus = E_ZCL_FAIL;
                }
            }
            else
            {
                DBG_vPrintf(TRACE_OTA_DEBUG, "ERR4\n");
                eStatus = E_ZCL_FAIL;
            }
#endif
        }
        else
        {
            DBG_vPrintf(TRACE_OTA_DEBUG, "ERR5\n");
            eStatus = E_ZCL_FAIL;
        }
    }
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eOtaFindCluster
 **
 ** DESCRIPTION:
 ** Search for ota cluster
 **
 ** PARAMETERS:                     Name                          Usage
 ** uint8                         u8SourceEndPointId            Source EP Id
 ** tsZCL_EndPointDefinition    **ppsEndPointDefinition         EP
 ** tsZCL_ClusterInstance       **ppsClusterInstance            Cluster
 ** tsOTA_Common                **ppsOTACustomDataStructure     event data
 ** bool_t                        bIsServer                   Is server
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC  teZCL_Status eOtaFindCluster(
                                uint8                           u8SourceEndPointId,
                                tsZCL_EndPointDefinition      **ppsEndPointDefinition,
                                tsZCL_ClusterInstance         **ppsClusterInstance,
                                tsOTA_Common                  **ppsOTACustomDataStructure,
                                bool_t                          bIsServer)
{

    if(u8SourceEndPointId > u8ZCL_GetNumberOfEndpointsRegistered())
    {
        return(E_ZCL_ERR_EP_RANGE);
    }

    // check EP is registered and cluster is present in the send device
    if(eZCL_SearchForEPentry(u8SourceEndPointId, ppsEndPointDefinition) != E_ZCL_SUCCESS)
    {
        return(E_ZCL_ERR_EP_RANGE);
    }

    if(eZCL_SearchForClusterEntry(u8SourceEndPointId, OTA_CLUSTER_ID, bIsServer, ppsClusterInstance) != E_ZCL_SUCCESS)
    {
        return(E_ZCL_ERR_CLUSTER_NOT_FOUND);
    }

    // initialise custom data pointer
    *ppsOTACustomDataStructure = (tsOTA_Common *)(*ppsClusterInstance)->pvEndPointCustomStructPtr;
    if(*ppsOTACustomDataStructure==NULL)
    {
        return(E_ZCL_ERR_PARAMETER_NULL);
    }

    return(E_ZCL_SUCCESS);
}

#ifdef OTA_CLIENT
/****************************************************************************
 **
 ** NAME:       eOTA_RestoreClientData
 **
 ** DESCRIPTION:
 ** Updates client attributes
 **
 ** PARAMETERS:                     Name                          Usage
 ** uint8                         u8Endpoint                 Source EP Id
 ** tsOTA_PersistedData          *psOTAData                  persisted data structure pointer
 ** bool_t                        bReset                     Is reset require
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_RestoreClientData(
                                   uint8                u8Endpoint,
                                   tsOTA_PersistedData *psOTAData,
                                   bool_t               bReset)
{
    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;

#if (defined OTA_CLD_ATTR_CURRENT_ZIGBEE_STACK_VERSION) || (defined OTA_CLD_ATTR_IMAGE_TYPE ) || (defined OTA_CLD_ATTR_MANF_ID)
    uint16 u16Value = E_ZCL_SUCCESS;
#endif
#if (defined OTA_CLD_ATTR_CURRENT_FILE_VERSION)
    uint32 u32Value = E_ZCL_SUCCESS;
#endif

    /* check pointer */
    if(psOTAData == NULL)
        return E_ZCL_ERR_PARAMETER_NULL;

    if((eStatus = eOtaFindCluster(u8Endpoint,
                         &psEndPointDefinition,
                           &psClusterInstance,
                           &psCustomData,
                           FALSE))
                           == E_ZCL_SUCCESS)
    {
        if(bReset)
        {
            memcpy(&psCustomData->sOTACallBackMessage.sPersistedData,psOTAData, sizeof(tsOTA_PersistedData) );
            // This is to cover the scenario where we have upgraded with PD held
            // The function pointer's need to be re-initialised.
            //if(psCustomData->sOTACallBackMessage.sPersistedData.sAttributes.u8ImageUpgradeStatus ==
            //            E_CLD_OTA_STATUS_NORMAL)
            //{
            //    vOtaInitStateMachine(psCustomData);
            //}
                /*Initialise the OTA function pointers*/
#if (defined KSDK2)
            if ( psCustomData->sOTACallBackMessage.sPersistedData.sAttributes.u8ImageUpgradeStatus == E_CLD_OTA_STATUS_DL_IN_PROGRESS)
            {
                if( sOtaGetHeader( &psCustomData->sOTACallBackMessage.sPersistedData.au8Header[0]).u32TotalImage != 0 )
                {
                    OTA_StartImage( ( sOtaGetHeader( &psCustomData->sOTACallBackMessage.sPersistedData.au8Header[0]).u32TotalImage )
                                                    - OTA_MIN_HEADER_SIZE - OTA_TAG_HEADER_SIZE );
                }
            }

            if(psCustomData->sOTACallBackMessage.sPersistedData.sAttributes.u8ImageUpgradeStatus ==  E_CLD_OTA_STATUS_NORMAL)
            {
                psCustomData->sOTACallBackMessage.sPersistedData.sAttributes.u8ImageUpgradeStatus = E_CLD_OTA_STATUS_RESET;
            }
#endif
            vOtaClientUpgMgrMapStates(  psCustomData->sOTACallBackMessage.sPersistedData.sAttributes.u8ImageUpgradeStatus, //teOTA_ImageUpgradeStatus  eStatus,
                                        psEndPointDefinition,                                                              //tsZCL_EndPointDefinition *psEndPointDefinition,
                                        psCustomData);                                                                     //tsOTA_Common *psCustomData);
        }
        eZCL_SetLocalAttributeValue(E_CLD_OTA_ATTR_UPGRADE_SERVER_ID,FALSE,TRUE,
                             psEndPointDefinition,
                             psClusterInstance,
                             &psCustomData->sOTACallBackMessage.sPersistedData.sAttributes.u64UgradeServerID);
#ifdef OTA_CLD_ATTR_FILE_OFFSET
        eZCL_SetLocalAttributeValue(E_CLD_OTA_ATTR_FILE_OFFSET,FALSE,TRUE,
                             psEndPointDefinition,
                             psClusterInstance,
                             &psCustomData->sOTACallBackMessage.sPersistedData.sAttributes.u32FileOffset);
#endif
#ifdef OTA_CLD_ATTR_CURRENT_FILE_VERSION
        u32Value = sOtaGetHeader(&au8OtaHeader[0]).u32FileVersion;
        psCustomData->sOTACallBackMessage.sPersistedData.sAttributes.u32CurrentFileVersion = u32Value;
        eZCL_SetLocalAttributeValue(E_CLD_OTA_ATTR_CURRENT_FILE_VERSION,FALSE,TRUE,
                             psEndPointDefinition,
                             psClusterInstance,
                             &u32Value);
#endif
#ifdef OTA_CLD_ATTR_CURRENT_ZIGBEE_STACK_VERSION
        u16Value = sOtaGetHeader(&au8OtaHeader[0]).u16StackVersion;
        psCustomData->sOTACallBackMessage.sPersistedData.sAttributes.u16CurrentStackVersion = u16Value;
        eZCL_SetLocalAttributeValue(E_CLD_OTA_ATTR_CURRENT_ZIGBEE_STACK_VERSION,FALSE,TRUE,
                             psEndPointDefinition,
                             psClusterInstance,
                             &u16Value);
#endif
#ifdef OTA_CLD_ATTR_DOWNLOADED_FILE_VERSION
        eZCL_SetLocalAttributeValue(E_CLD_OTA_ATTR_DOWNLOADED_FILE_VERSION,FALSE,TRUE,
                             psEndPointDefinition,
                             psClusterInstance,
                             &psCustomData->sOTACallBackMessage.sPersistedData.sAttributes.u32DownloadedFileVersion);

#endif

#ifdef OTA_CLD_ATTR_DOWNLOADED_ZIGBEE_STACK_VERSION
        eZCL_SetLocalAttributeValue(E_CLD_OTA_ATTR_DOWNLOADED_ZIGBEE_STACK_VERSION,FALSE,TRUE,
                             psEndPointDefinition,
                             psClusterInstance,
                             &psCustomData->sOTACallBackMessage.sPersistedData.sAttributes.u16DownloadedStackVersion);

#endif
        eZCL_SetLocalAttributeValue(E_CLD_OTA_ATTR_IMAGE_UPGRADE_STATUS,FALSE,TRUE,
                             psEndPointDefinition,
                             psClusterInstance,
                             &psCustomData->sOTACallBackMessage.sPersistedData.sAttributes.u8ImageUpgradeStatus);

#ifdef OTA_CLD_ATTR_MANF_ID
        u16Value = sOtaGetHeader(&au8OtaHeader[0]).u16ManufacturerCode;
        psCustomData->sOTACallBackMessage.sPersistedData.sAttributes.u16ManfId = u16Value;
        eZCL_SetLocalAttributeValue(E_CLD_OTA_ATTR_MANF_ID,FALSE,TRUE,
                             psEndPointDefinition,
                             psClusterInstance,
                             &psCustomData->sOTACallBackMessage.sPersistedData.sAttributes.u16ManfId);
#endif

#ifdef OTA_CLD_ATTR_IMAGE_TYPE
    if(psCustomData->sOTACallBackMessage.sPersistedData.sAttributes.u8ImageUpgradeStatus == E_CLD_OTA_STATUS_NORMAL)
    {
        u16Value = 0xFFFF;
        eZCL_SetLocalAttributeValue(E_CLD_OTA_ATTR_IMAGE_TYPE,FALSE,TRUE,
                             psEndPointDefinition,
                             psClusterInstance,
                             &u16Value);
    }
    else
    {
        u16Value = sOtaGetHeader(&au8OtaHeader[0]).u16ImageType;
        psCustomData->sOTACallBackMessage.sPersistedData.sAttributes.u16ImageType = u16Value;
        eZCL_SetLocalAttributeValue(E_CLD_OTA_ATTR_IMAGE_TYPE,FALSE,TRUE,
                             psEndPointDefinition,
                             psClusterInstance,
                             &psCustomData->sOTACallBackMessage.sPersistedData.sAttributes.u16ImageType);
    }
#endif
    }
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eOTA_UpdateClientAttributes
 **
 ** DESCRIPTION:
 ** Updates client attributes
 **
 ** PARAMETERS:                     Name                          Usage
 ** uint8                         u8Endpoint                 Source EP Id
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC teZCL_Status eOTA_UpdateClientAttributes(uint8 u8Endpoint, uint32 u32ImageStamp)
{
    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;
    tsCLD_AS_Ota sOta;

    sOta.u64UgradeServerID = 0xffffffffffffffffLL;

#ifdef OTA_CLD_ATTR_FILE_OFFSET
    sOta.u32FileOffset = 0xffffffff;
#endif

#ifdef OTA_CLD_ATTR_CURRENT_FILE_VERSION
    sOta.u32CurrentFileVersion = 0xffffffff;
#endif

#ifdef OTA_CLD_ATTR_CURRENT_ZIGBEE_STACK_VERSION
    sOta.u16CurrentStackVersion = 0xffff;
#endif

#ifdef OTA_CLD_ATTR_DOWNLOADED_FILE_VERSION
    sOta.u32DownloadedFileVersion = 0xffffffff;
#endif

#ifdef OTA_CLD_ATTR_DOWNLOADED_ZIGBEE_STACK_VERSION
    sOta.u16DownloadedStackVersion = 0xffff;
#endif

    sOta.u8ImageUpgradeStatus = 0x00;

#ifdef OTA_CLD_ATTR_MANF_ID
    sOta.u16ManfId = CLD_OTA_MANF_ID_VALUE;
#endif

#ifdef OTA_CLD_ATTR_IMAGE_TYPE
    sOta.u16ImageType = 0xffff;
#endif

#ifdef OTA_CLD_ATTR_IMAGE_STAMP
    sOta.u32ImageStamp = u32ImageStamp;
#endif

#ifdef OTA_CLD_ATTR_REQUEST_DELAY
    sOta.u16MinBlockRequestDelay = OTA_BLOCK_REQUEST_DELAY_DEF_VALUE;
#endif

    if((eStatus = eOtaFindCluster(u8Endpoint,
                         &psEndPointDefinition,
                           &psClusterInstance,
                           &psCustomData,
                           FALSE))
                           == E_ZCL_SUCCESS)
    {

        eZCL_SetLocalAttributeValue(E_CLD_OTA_ATTR_UPGRADE_SERVER_ID,FALSE,TRUE,
                             psEndPointDefinition,
                             psClusterInstance,
                             &sOta.u64UgradeServerID);
#ifdef OTA_CLD_ATTR_FILE_OFFSET
        eZCL_SetLocalAttributeValue(E_CLD_OTA_ATTR_FILE_OFFSET,FALSE,TRUE,
                             psEndPointDefinition,
                             psClusterInstance,
                             &sOta.u32FileOffset);
#endif
#ifdef OTA_CLD_ATTR_CURRENT_FILE_VERSION
        sOta.u32CurrentFileVersion = sOtaGetHeader(&au8OtaHeader[0]).u32FileVersion;
        eZCL_SetLocalAttributeValue(E_CLD_OTA_ATTR_CURRENT_FILE_VERSION,FALSE,TRUE,
                             psEndPointDefinition,
                             psClusterInstance,
                             &sOta.u32CurrentFileVersion);
#endif
#ifdef OTA_CLD_ATTR_CURRENT_ZIGBEE_STACK_VERSION
        sOta.u16CurrentStackVersion = sOtaGetHeader(&au8OtaHeader[0]).u16StackVersion;
        eZCL_SetLocalAttributeValue(E_CLD_OTA_ATTR_CURRENT_ZIGBEE_STACK_VERSION,FALSE,TRUE,
                             psEndPointDefinition,
                             psClusterInstance,
                             &sOta.u16CurrentStackVersion);
#endif
#ifdef OTA_CLD_ATTR_DOWNLOADED_FILE_VERSION
        eZCL_SetLocalAttributeValue(E_CLD_OTA_ATTR_DOWNLOADED_FILE_VERSION,FALSE,TRUE,
                             psEndPointDefinition,
                             psClusterInstance,
                             &sOta.u32DownloadedFileVersion);

#endif
#ifdef OTA_CLD_ATTR_DOWNLOADED_ZIGBEE_STACK_VERSION
        eZCL_SetLocalAttributeValue(E_CLD_OTA_ATTR_DOWNLOADED_ZIGBEE_STACK_VERSION,FALSE,TRUE,
                             psEndPointDefinition,
                             psClusterInstance,
                             &sOta.u16DownloadedStackVersion);

#endif

        eZCL_SetLocalAttributeValue(E_CLD_OTA_ATTR_IMAGE_UPGRADE_STATUS,FALSE,TRUE,
                             psEndPointDefinition,
                             psClusterInstance,
                             &sOta.u8ImageUpgradeStatus);

#ifdef OTA_CLD_ATTR_MANF_ID
        eZCL_SetLocalAttributeValue(E_CLD_OTA_ATTR_MANF_ID,FALSE,TRUE,
                             psEndPointDefinition,
                             psClusterInstance,
                             &sOta.u16ManfId);

#endif

#ifdef OTA_CLD_ATTR_IMAGE_TYPE
        eZCL_SetLocalAttributeValue(E_CLD_OTA_ATTR_IMAGE_TYPE,FALSE,TRUE,
                             psEndPointDefinition,
                             psClusterInstance,
                             &sOta.u16ImageType);
#endif

#ifdef OTA_CLD_ATTR_IMAGE_STAMP
        eZCL_SetLocalAttributeValue(E_CLD_OTA_ATTR_IMAGE_STAMP,FALSE,TRUE,
                             psEndPointDefinition,
                             psClusterInstance,
                             &sOta.u32ImageStamp);
#endif

#ifdef OTA_CLD_ATTR_REQUEST_DELAY
        eZCL_SetLocalAttributeValue(E_CLD_OTA_ATTR_REQUEST_DELAY,FALSE,TRUE,
                             psEndPointDefinition,
                             psClusterInstance,
                             &sOta.u16MinBlockRequestDelay);
#endif
    }
    return eStatus;
}
#endif /* #ifdef OTA_CLIENT */
/****************************************************************************
 **
 ** NAME:       eOtaSetEventTypeAndGiveCallBack
 **
 ** DESCRIPTION:
 ** set event type in callback message and give callback to application
 ** PARAMETERS:                         Name                          Usage
 ** tsOTA_Common                     *psOTA_Common                OTA custom data structure
 ** teOTA_UpgradeClusterEvents        eEventType                  event type
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
PUBLIC  teZCL_Status eOtaSetEventTypeAndGiveCallBack(
                       tsOTA_Common                     *psOTA_Common,
                       teOTA_UpgradeClusterEvents        eEventType,
                       tsZCL_EndPointDefinition         *psEndPointDefinition)
{
    teZCL_Status eStatus = E_ZCL_SUCCESS;
    if(psOTA_Common == NULL)
    {
        eStatus = E_ZCL_ERR_PARAMETER_NULL;
    }
    else
    {
        psOTA_Common->sOTACallBackMessage.eEventId = eEventType;
        psEndPointDefinition->pCallBackFunctions(&psOTA_Common->sOTACustomCallBackEvent);
    }
    return(eStatus);
}
/****************************************************************************
 **
 ** NAME:       eOtaRegisterTimeServer
 **
 ** DESCRIPTION:
 ** Registers OTA Time Server with ZCL
 **
 ** PARAMETERS:                         Name                          Usage
 ** none
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eOtaRegisterTimeServer(void)
{
    static bool_t bOTATimerRegistered = FALSE;
    if(!bOTATimerRegistered)
    {
       // add timer click function to ZCL
       if(eZCL_TimerRegister(E_ZCL_TIMER_CLICK_MS, 0, vOtaTimerClickCallback)!= E_ZCL_SUCCESS)
       {
          return(E_ZCL_FAIL);
       }
      bOTATimerRegistered = TRUE;
    }

    return(E_ZCL_SUCCESS);
}
/****************************************************************************
 **
 ** NAME:       vOtaTimerClickCallback
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

PUBLIC  void vOtaTimerClickCallback(tsZCL_CallBackEvent *psCallBackEvent)
{
#if (defined(OTA_SERVER) && defined(OTA_PAGE_REQUEST_SUPPORT)) || defined(OTA_CLIENT)
    int i;

    uint8 u8NumberOfendpoints;

    u8NumberOfendpoints = u8ZCL_GetNumberOfEndpointsRegistered();
    
    // find price clusters on each EP - if any
    for(i=1; i<(u8NumberOfendpoints+1); i++)
    {
        // deliver time to any EP-server/client
        eOtaTimeUpdate(i, TRUE, psCallBackEvent);
        eOtaTimeUpdate(i, FALSE, psCallBackEvent);
    }
#endif
}
/****************************************************************************
 **
 ** NAME:       eOtaTimeUpdate
 **
 ** DESCRIPTION:
 ** Update ota parameters on Timer Callback registered with the ZCL
 **
 ** PARAMETERS:                 Name               Usage
 ** uint8                  u8SourceEndPointId     Source EP Id
 ** bool_t                 bIsServer              Is server instance
 ** tsZCL_CallBackEvent   *psCallBackEvent        callback event
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eOtaTimeUpdate(
                                  uint8                u8SourceEndPointId,
                                  bool_t               bIsServer,
                                  tsZCL_CallBackEvent *psCallBackEvent)
{
    teZCL_Status eStatus = E_ZCL_SUCCESS;

#if defined(OTA_CLIENT) || (defined(OTA_SERVER) && defined(OTA_PAGE_REQUEST_SUPPORT))
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;
#endif

#ifdef OTA_CLIENT
    uint32 u32UTCTime = psCallBackEvent->uMessage.sTimerMessage.u32UTCTime;
#endif
#ifdef OTA_UPGRADE_VOLTAGE_CHECK
            /* Check if Volatge was OK */
    if( bOta_LowVoltage )
    {
        return E_ZCL_SUCCESS;
    }
#endif

    if(!bIsServer )
    {
#ifdef OTA_CLIENT
        if((eStatus = eOtaFindCluster(u8SourceEndPointId,
                         &psEndPointDefinition,
                           &psClusterInstance,
                           &psCustomData,
                           FALSE))
                           == E_ZCL_SUCCESS)
        {
#ifdef OTA_CLD_ATTR_REQUEST_DELAY
            if((psCustomData->sOTACallBackMessage.sPersistedData.bWaitForBlockReq == TRUE)&&
                    (E_ZCL_CBET_TIMER_MS == psCallBackEvent->eEventType))
            {
                psCustomData->sOTACallBackMessage.sPersistedData.bWaitForBlockReq = FALSE;
                psCustomData->sOTACallBackMessage.eEventId = E_CLD_OTA_INTERNAL_COMMAND_TIMER_EXPIRED;
                psCustomData->sOTACustomCallBackEvent.psClusterInstance = psClusterInstance;
                ((pFuncptr)(psCustomData->sOTACallBackMessage.sPersistedData.u32FunctionPointer))(psCustomData, psEndPointDefinition);
                return E_ZCL_SUCCESS;
            }
#endif

            if(((psCustomData->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime <=
                u32UTCTime)&& (psCustomData->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime != 0))
#ifdef OTA_CLD_ATTR_REQUEST_DELAY
                &&
                (psCustomData->sOTACallBackMessage.sPersistedData.bWaitForBlockReq != TRUE)    
#endif                
                )
            {
                psCustomData->sOTACallBackMessage.eEventId = E_CLD_OTA_INTERNAL_COMMAND_TIMER_EXPIRED;
                psCustomData->sOTACustomCallBackEvent.psClusterInstance = psClusterInstance;
                ((pFuncptr)(psCustomData->sOTACallBackMessage.sPersistedData.u32FunctionPointer))(psCustomData, psEndPointDefinition);
                DBG_vPrintf(TRACE_OTA_DEBUG,"Timer called for EP %d %d %d\n", u8SourceEndPointId,u32UTCTime, psCustomData->sOTACallBackMessage.sPersistedData.u32RequestBlockRequestTime);
            }
        }
#endif

    }
#if (defined OTA_SERVER && defined OTA_PAGE_REQUEST_SUPPORT)
    else
    {
        if((eStatus = eOtaFindCluster(u8SourceEndPointId,
                    &psEndPointDefinition,
                    &psClusterInstance,
                    &psCustomData,
                    TRUE))
                    == E_ZCL_SUCCESS)
        {
            if((psCustomData->sOTACallBackMessage.sPageReqServerParams.bPageReqRespSpacing == TRUE)&&
               (E_ZCL_CBET_TIMER_MS == psCallBackEvent->eEventType))
            {
                psCustomData->sOTACallBackMessage.sPageReqServerParams.bPageReqRespSpacing = FALSE;
                vOtaHandleTimedPageRequest(u8SourceEndPointId);
            }
        }
    }
#endif
    return eStatus;
}
#ifdef OTA_CLIENT
/****************************************************************************
 **
 ** NAME:       eOTA_SetServerAddress
 **
 ** DESCRIPTION:
 ** Updates server address
 ** PARAMETERS:             Name               Usage
 ** uint8                u8Endpoint           Endpoint
 ** uint64               u64IeeeAddress       IEEE address of server
 ** uint16               u16ShortAddress      short address of server
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_SetServerAddress(
                                          uint8     u8Endpoint,
                                          uint64    u64IeeeAddress,
                                          uint16    u16ShortAddress)
{
    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;
    if((eStatus = eOtaFindCluster(u8Endpoint,
                         &psEndPointDefinition,
                           &psClusterInstance,
                           &psCustomData,
                           FALSE))
                           == E_ZCL_SUCCESS)
    {

        psCustomData->sOTACallBackMessage.sPersistedData.u16ServerShortAddress = u16ShortAddress;
        psCustomData->sOTACallBackMessage.sPersistedData.sAttributes.u64UgradeServerID = u64IeeeAddress;
        eZCL_SetLocalAttributeValue(E_CLD_OTA_ATTR_UPGRADE_SERVER_ID,FALSE,TRUE,
                         psEndPointDefinition,
                         psCustomData->sOTACustomCallBackEvent.psClusterInstance,
                         &u64IeeeAddress);
    }
    return eStatus;
}
#endif

#ifdef OTA_SERVER
/****************************************************************************
 **
 ** NAME:       eOTA_SetServerParams
 **
 ** DESCRIPTION:
 ** Updates server parameters
 ** PARAMETERS:             Name                Usage
 ** uint8                u8Endpoint           Endpoint
 ** uint8                u8ImageIndex         Image index
 ** tsCLD_PR_Ota         psOTAData            ota data
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_SetServerParams(
                                            uint8          u8Endpoint,
                                            uint8          u8ImageIndex,
                                            tsCLD_PR_Ota  *psOTAData)
{
    teZCL_Status eStatus;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;
    if((eStatus = eOtaFindCluster(u8Endpoint,
                         &psEndPointDefinition,
                           &psClusterInstance,
                           &psCustomData,
                           TRUE))
                           == E_ZCL_SUCCESS)
    {
        psCustomData->sOTACallBackMessage.aServerPrams[u8ImageIndex].pu8Data                 = psOTAData->pu8Data;
        psCustomData->sOTACallBackMessage.aServerPrams[u8ImageIndex].u32CurrentTime          = psOTAData->u32CurrentTime;
        psCustomData->sOTACallBackMessage.aServerPrams[u8ImageIndex].u32RequestOrUpgradeTime = psOTAData->u32RequestOrUpgradeTime;
        psCustomData->sOTACallBackMessage.aServerPrams[u8ImageIndex].u8QueryJitter           = psOTAData->u8QueryJitter;
        psCustomData->sOTACallBackMessage.aServerPrams[u8ImageIndex].u8DataSize              = psOTAData->u8DataSize;
    }
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eOTA_GetServerData
 **
 ** DESCRIPTION:
 ** get server parameters
 ** PARAMETERS:             Name                Usage
 ** uint8                u8Endpoint           Endpoint
 ** uint8                u8ImageIndex         Image index
 ** tsCLD_PR_Ota         psOTAData            ota data
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_GetServerData(
                                          uint8            u8Endpoint,
                                          uint8            u8ImageIndex,
                                          tsCLD_PR_Ota    *psOTAData)
{
    teZCL_Status eStatus;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;

    if( (( eStatus = eOtaFindCluster(u8Endpoint,
                         &psEndPointDefinition,
                           &psClusterInstance,
                           &psCustomData,
                           TRUE ))
                           == E_ZCL_SUCCESS) &&
       ( u8ImageIndex < (OTA_MAX_IMAGES_PER_ENDPOINT + OTA_MAX_CO_PROCESSOR_IMAGES ) ) )
    {
        memcpy (psOTAData, &psCustomData->sOTACallBackMessage.aServerPrams[u8ImageIndex], sizeof(tsCLD_PR_Ota));
    }
    else
    {
        eStatus = E_ZCL_FAIL;
    }
    return eStatus;
}
/****************************************************************************
 **
 ** NAME:       eOTA_SetServerAuthorisation
 **
 ** DESCRIPTION:
 ** Set server Authorisation for clients
 ** PARAMETERS:                     Name                Usage
 ** uint8                        u8Endpoint           Endpoint
 ** eOTA_AuthorisationState      eState               state
 ** uint64                      *pu64WhiteList        list of clients
 ** uint8                        u8Size               size of list
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_SetServerAuthorisation(
                             uint8                   u8Endpoint,
                             eOTA_AuthorisationState eState,
                             uint64                 *pu64WhiteList,
                             uint8                   u8Size)
{
    teZCL_Status eStatus;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;
    if((eStatus = eOtaFindCluster(u8Endpoint,
                         &psEndPointDefinition,
                           &psClusterInstance,
                           &psCustomData,
                           TRUE))
                           == E_ZCL_SUCCESS)
    {
        psCustomData->sOTACallBackMessage.sAuthStruct.eState = eState;
        psCustomData->sOTACallBackMessage.sAuthStruct.pu64WhiteList = pu64WhiteList;
        psCustomData->sOTACallBackMessage.sAuthStruct.u8Size = u8Size;
    }
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eOTA_NewImageLoaded
 **
 ** DESCRIPTION:
 ** update OTA structures and index with new image
 ** PARAMETERS:                     Name                             Usage
 ** uint8                        u8Endpoint                        Endpoint
 ** bool_t                         bIsImageOnCoProcessorMedia        Is image on co processor media
 ** tsOTA_CoProcessorOTAHeader  *psOTA_CoProcessorOTAHeader        co processor ota header
 **
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_NewImageLoaded(
                                  uint8                       u8Endpoint,
                                  bool_t                        bIsImageOnCoProcessorMedia,
                                  tsOTA_CoProcessorOTAHeader *psOTA_CoProcessorOTAHeader)
{
    teZCL_Status eStatus;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;
    uint8 u8CurrentSector, i, aHeader[OTA_MAX_HEADER_SIZE];
    uint32 u32FlashOffset, u32HeaderType;
    uint16 u16HeaderLength;
    tsOTA_ImageHeader sImageHeader;
    /* validate parameters */
    if(bIsImageOnCoProcessorMedia && (psOTA_CoProcessorOTAHeader == NULL))
    {
        return E_ZCL_ERR_PARAMETER_NULL;
    }

    if((eStatus = eOtaFindCluster(u8Endpoint,
                         &psEndPointDefinition,
                           &psClusterInstance,
                           &psCustomData,
                           TRUE))
                           == E_ZCL_SUCCESS)
    {
        /* check images loaded on coprocessor media */
        if(bIsImageOnCoProcessorMedia)
        {
#if (OTA_MAX_CO_PROCESSOR_IMAGES != 0)
            /* copy OTA CoProcessor Headers to asCommonCoProcessorOTAHeader array */
            for(i = 0; i < OTA_MAX_CO_PROCESSOR_IMAGES; i++)
            {
                /* Check OTA identifier is valid or not and min header length */
                if(psOTA_CoProcessorOTAHeader->sOTA_ImageHeader[i].u32FileIdentifier == OTA_FILE_IDENTIFIER &&
                        psOTA_CoProcessorOTAHeader->sOTA_ImageHeader[i].u16HeaderLength <= OTA_MAX_HEADER_SIZE )
                {
                    memcpy(&sOTAHeader[i], &psOTA_CoProcessorOTAHeader->sOTA_ImageHeader[i],
                            sizeof(tsOTA_ImageHeader));

                    psCustomData->sOTACallBackMessage.aServerPrams[OTA_MAX_IMAGES_PER_ENDPOINT+i].u8QueryJitter = (uint8)RND_u32GetRand(0,100);
                    psCustomData->sOTACallBackMessage.aServerPrams[OTA_MAX_IMAGES_PER_ENDPOINT+i].u32CurrentTime = u32ZCL_GetUTCTime();
                    psCustomData->sOTACallBackMessage.aServerPrams[OTA_MAX_IMAGES_PER_ENDPOINT+i].u32RequestOrUpgradeTime = 0xffffffff;
                    psCustomData->sOTACallBackMessage.aServerPrams[OTA_MAX_IMAGES_PER_ENDPOINT+i].u8DataSize = 0;
                }
                else
                {
                    return E_ZCL_FAIL;
                }
            }
#endif
            return E_ZCL_SUCCESS;
        }
        eStatus = E_ZCL_FAIL;
        for(i = 0 ;i < OTA_MAX_IMAGES_PER_ENDPOINT; i++)
        {
            {
                DBG_vPrintf(TRACE_OTA_DEBUG, "OTA Client Image Found at %d\n",i);
                u8CurrentSector = psCustomData->sOTACallBackMessage.u8ImageStartSector[i];
                u32FlashOffset = u8CurrentSector * sNvmDefsStruct.u32SectorSize * OTA_SECTOR_CONVERTION;
                //Get the image header installed and check against client requirement
                //first read the initial 7 bytes to see how big the header is
                vOtaFlashLockRead(psEndPointDefinition, psCustomData,u32FlashOffset,OTA_MAX_HEADER_SIZE, aHeader);

                
                vReverseMemcpy((uint8*)&u16HeaderLength,&aHeader[6],sizeof(uint16));
                vReverseMemcpy((uint8*)&u32HeaderType,&aHeader[0],sizeof(uint32));
                if(u32HeaderType == OTA_FILE_IDENTIFIER &&
                    u16HeaderLength <= OTA_MAX_HEADER_SIZE )
                {
                    DBG_vPrintf(TRACE_OTA_DEBUG, "OTA Client Image Found File Identifer Matched\n");
                    vOtaFlashLockRead(psEndPointDefinition, psCustomData,u32FlashOffset,u16HeaderLength, aHeader);
                    sImageHeader = sOtaGetHeader(aHeader);
                    if ((sImageHeader.u16HeaderControlField & OTA_HDR_DEV_SPECF_FILE)&&(u16HeaderLength < (OTA_MIN_HEADER_SIZE+8)))
                    {
                        continue;
                    }
                    psCustomData->sOTACallBackMessage.aServerPrams[i].u8QueryJitter = (uint8)RND_u32GetRand(0,100);
                    psCustomData->sOTACallBackMessage.aServerPrams[i].u32CurrentTime = u32ZCL_GetUTCTime();
                    psCustomData->sOTACallBackMessage.aServerPrams[i].u32RequestOrUpgradeTime = 0xffffffff;
                    psCustomData->sOTACallBackMessage.aServerPrams[i].u8DataSize = 0;

                    eStatus = E_ZCL_SUCCESS;
                }
                else
                {
                    continue;
                }
            }
        }
    }
    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eOTA_FlashWriteNewImageBlock
 **
 ** DESCRIPTION: Upgrade OTA Server/Client Image by writing into allocated flash
 **
 ** PARAMETERS:                     Name                             Usage
 ** uint8                        u8Endpoint                        Endpoint
 ** uint8                        u8ImageIndex                      Image index
 ** bool_t                         bIsServerImage                    Is server image
 ** uint8                       *pu8UpgradeBlockData               Upgrade block data
 ** uint8                        u8UpgradeBlockDataLength          Upgrade block data length
 ** uint32                       u32FileOffSet                     file offset
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_FlashWriteNewImageBlock(
                                   uint8     u8Endpoint,
                                   uint8     u8ImageIndex,
                                   bool_t      bIsServerImage,
                                   uint8    *pu8UpgradeBlockData,
                                   uint8     u8UpgradeBlockDataLength,
                                   uint32    u32FileOffSet)
{
    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;
    uint32 u32FlashOffset,u32OTAFileIdentifier;
    uint16 u16OTAHeaderLength;
    uint8 u8ServerOTAHeaderRemBytesWrite, u8ServerOTAHeaderOffset = 0;

    /* Validate parameters */
    if(u8ImageIndex >= OTA_MAX_IMAGES_PER_ENDPOINT)
    {
        return E_ZCL_ERR_EP_RANGE;
    }

    if(pu8UpgradeBlockData == NULL)
    {
        return E_ZCL_ERR_PARAMETER_NULL;
    }

    /* Find OTA server cluster */
    if((eStatus = eOtaFindCluster(u8Endpoint,
                         &psEndPointDefinition,
                           &psClusterInstance,
                           &psCustomData,
                           TRUE))
                           == E_ZCL_SUCCESS)
    {

        /* Is upgrading for server */
        if(bIsServerImage)
        {
            /* Is server OTA header is copied*/
            if(!psCustomData->sOTACallBackMessage.bIsOTAHeaderCopied)
            {
                /* bcoz server upgrading, write upgrade data from tag data (OTA Header + Tag Id(0) + Tag length )*/
                if((u32FileOffSet + u8UpgradeBlockDataLength) <= OTA_MIN_HEADER_SIZE)
                {
                    memcpy(&psCustomData->sOTACallBackMessage.au8ServerOTAHeader[u32FileOffSet], pu8UpgradeBlockData, u8UpgradeBlockDataLength);
                }
                else
                {
                    uint8 u8DataOverFlow = OTA_MIN_HEADER_SIZE - (uint8)u32FileOffSet;
                    memcpy(&psCustomData->sOTACallBackMessage.au8ServerOTAHeader[u32FileOffSet], pu8UpgradeBlockData, u8DataOverFlow);

                    /* Get OTA header length */
                    vReverseMemcpy((uint8*)&u16OTAHeaderLength,&psCustomData->sOTACallBackMessage.au8ServerOTAHeader[6],sizeof(uint16));
                    vReverseMemcpy((uint8*)&u32OTAFileIdentifier,&psCustomData->sOTACallBackMessage.au8ServerOTAHeader[0],sizeof(uint32));

                    /* Is valid OTA identifier */
                    if(u32OTAFileIdentifier == OTA_FILE_IDENTIFIER &&
                            u16OTAHeaderLength <= OTA_MAX_HEADER_SIZE)
                    {
                        if( (u32FileOffSet + u8UpgradeBlockDataLength) <= (uint32)(u16OTAHeaderLength + OTA_TAG_HEADER_SIZE))
                        {
                            uint8 u8HeaderOverFlow = ((uint8)u32FileOffSet + u8UpgradeBlockDataLength - OTA_MIN_HEADER_SIZE);
                            memcpy(&psCustomData->sOTACallBackMessage.au8ServerOTAHeader[OTA_MIN_HEADER_SIZE], pu8UpgradeBlockData+u8DataOverFlow, u8HeaderOverFlow);
                        }
                        else
                        {
                            /* estimate how many bytes left for writing OTA header */
                            u8ServerOTAHeaderRemBytesWrite = (uint8)((u16OTAHeaderLength + OTA_TAG_HEADER_SIZE) - u32FileOffSet);

                            /* write remaining bytes */
                            memcpy(&psCustomData->sOTACallBackMessage.au8ServerOTAHeader[u32FileOffSet],
                                        pu8UpgradeBlockData, u8ServerOTAHeaderRemBytesWrite);

#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x) || (defined APP0)
                            /* Get Starting Image Offset */

                            u32FlashOffset =
                               (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8ImageIndex] *
                                    sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;
#else
                            u32FlashOffset = 0;
#endif
                            /* Now start writing into flash */
                            vOtaFlashLockWrite(psEndPointDefinition, psCustomData,u32FlashOffset,
                                    (u8UpgradeBlockDataLength-u8ServerOTAHeaderRemBytesWrite),
                                    (pu8UpgradeBlockData+u8ServerOTAHeaderRemBytesWrite));

                            /* Set the flag, OTA header written to callback message */
                            psCustomData->sOTACallBackMessage.bIsOTAHeaderCopied = TRUE;
                        }
                    }
                    else
                    {
                        return E_ZCL_FAIL;
                    }
                }
                return eStatus;
            }
            else
            {
                /* Get OTA header length */
                vReverseMemcpy((uint8*)&u16OTAHeaderLength,&psCustomData->sOTACallBackMessage.au8ServerOTAHeader[6],sizeof(uint16));
                u8ServerOTAHeaderOffset = u16OTAHeaderLength + OTA_TAG_HEADER_SIZE;
            }
        }

        /* Should n't write more than total allocated flash size */
        if( (u32FileOffSet + u8UpgradeBlockDataLength) <= (psCustomData->sOTACallBackMessage.u8MaxNumberOfSectors * sNvmDefsStruct.u32SectorSize) )
        {
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x) || (defined APP0)
            /* Get Starting Image Offset */
            u32FlashOffset =
                   (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8ImageIndex] *
                    sNvmDefsStruct.u32SectorSize)* OTA_SECTOR_CONVERTION;
#else
            u32FlashOffset = 0;
#endif
            /* Write client image to flash to specified offset */
            u32FlashOffset += (u32FileOffSet-u8ServerOTAHeaderOffset);
            vOtaFlashLockWrite(psEndPointDefinition, psCustomData,u32FlashOffset, u8UpgradeBlockDataLength, pu8UpgradeBlockData);
        }
        else
        {
            return E_ZCL_FAIL;
        }
    }
    return eStatus;
}


/****************************************************************************
 **
 ** NAME:       eOTA_ServerSwitchToNewImage
 **
 ** DESCRIPTION: Server switching to new image.
 **
 ** PARAMETERS:                     Name                             Usage
 ** uint8                        u8Endpoint                        Endpoint
 ** uint8                        u8ImageIndex                      Image index
 ** RETURN: teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_ServerSwitchToNewImage(
                                           uint8        u8Endpoint,
                                           uint8        u8ImageIndex)
{
    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;
    tsOTA_ImageHeader sOTA_ImageHeader;
    uint8 au8Data[16] = {0};
    uint32 u32Offset;


    uint32 u32OtaOffset = eOTA_OtaOffset();
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)    
    uint32 u32CustomerSettings = *((uint32*)FL_INDEX_SECTOR_CUSTOMER_SETTINGS);
#endif

#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
    bool_t bEncExternalFlash = (u32CustomerSettings & FL_INDEX_SECTOR_ENC_EXT_FLASH)?FALSE:TRUE;
#else
     bool_t bEncExternalFlash = FALSE;
#endif 
    
#ifndef OTA_UNIT_TEST_FRAMEWORK
    tsReg128 sOtaUseKey = eOTA_retOtaUseKey();
#endif
#ifdef OTA_ACCEPT_ONLY_SIGNED_IMAGES
    uint16 u16OTAHeaderLength = 0;
#endif

    /* Validate parameters */
    if(u8ImageIndex >= OTA_MAX_IMAGES_PER_ENDPOINT)
    {
        return E_ZCL_ERR_EP_RANGE;
    }

    /* Find OTA server cluster */
    if((eStatus = eOtaFindCluster(u8Endpoint,
                         &psEndPointDefinition,
                           &psClusterInstance,
                           &psCustomData,
                           TRUE))
                           == E_ZCL_SUCCESS)
    {
            /* get upgraded ota header */
            sOTA_ImageHeader = sOtaGetHeader(psCustomData->sOTACallBackMessage.au8ServerOTAHeader);

            /* File version should be more than previous image file version */
            /* Image type & manufacturer code should match */
            if(sOTA_ImageHeader.u32FileVersion > sOtaGetHeader(&au8OtaHeader[0]).u32FileVersion &&
                    sOTA_ImageHeader.u16ManufacturerCode == sOtaGetHeader(&au8OtaHeader[0]).u16ManufacturerCode &&
                    sOTA_ImageHeader.u16ImageType == sOtaGetHeader(&au8OtaHeader[0]).u16ImageType)
            {
#ifndef OTA_UNIT_TEST_FRAMEWORK
                /* copy serialization & custom data from active server image */

                uint8 *pu8Start = (uint8*)&_flash_start;
                uint8 *pu8LinkKey =    (uint8*)&_FlsLinkKey;
                uint8 *pu8Cert =       (uint8*)&FlsZcCert;
#ifdef COPY_PRIVATE_KEY                
                uint8 *pu8PrvKey =     (uint8*)&FlsPrivateKey;
#endif                
#ifdef OTA_COPY_MAC_ADDRESS
                uint8 *pu8MacAddress = (uint8*)&_FlsMACAddress;
#endif
#ifdef OTA_MAINTAIN_CUSTOM_SERIALISATION_DATA
                uint8 *pu8CustData =   (uint8*)&FlsCustomDatabeg;
                uint8 *pu8CustDataEnd = (uint8*)&FlsCustomDataEnd;
                uint8 u8SizeOfCustomData = pu8CustDataEnd - pu8CustData;
#endif

                uint8 au8ivector[OTA_AES_BLOCK_SIZE], au8DataOut[OTA_AES_BLOCK_SIZE];

                uint8 u8Loop;
                uint32 u32TempOffset;

                /* Verify Image */
#ifdef OTA_ACCEPT_ONLY_SIGNED_IMAGES
                /* Get OTA header length */
                vReverseMemcpy(&u16OTAHeaderLength,&psCustomData->sOTACallBackMessage.au8ServerOTAHeader[6],sizeof(uint16));

                /* for own image */
                eStatus = eOTA_VerifyImage(psEndPointDefinition->u8EndPointNumber,
                                           TRUE,
                                           u8ImageIndex,
                                           FALSE);

#else
                eStatus = E_ZCL_SUCCESS;
#endif

#ifdef OTA_ACCEPT_ONLY_SIGNED_IMAGES
                /* check verification status */
                if(eStatus != E_ZCL_SUCCESS)
                {
                    return E_ZCL_FAIL;
                }
#endif

                u32Offset = pu8Cert - pu8LinkKey;
                if(u32Offset > 0)
                {
                    uint8 au8TempBuffer[50]; /* for copying data from active image */

                    // copy Link Key
                    DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 31 ..\n");

                    // Check if the Device is Encrypted
                    /* Check if the device is Encrypted.  If yes the device is encrypted, we have to take care of IV vector as well
                     * If the IV vector in the Upgrade Image is different we cannot just copy the Serialization Data
                     * Directly. We need to Decrypt existing data and encrypt it again with the new IV vector values.
                     */

                    if (bEncExternalFlash)
                    {

                        DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 201 ..\n");
#ifdef OTA_COPY_MAC_ADDRESS
                        /* Copy Mac Address from from Internal flash and encrypt it. Then copy to external flash */
                        memcpy(au8TempBuffer, pu8MacAddress, 8);

                        /* Read IV vector from external flash */
                        u32TempOffset = (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8ImageIndex] * sNvmDefsStruct.u32SectorSize);
                        u32TempOffset += OTA_IV_LOCATION;
                        vOtaFlashLockRead(psEndPointDefinition, psCustomData, u32TempOffset,OTA_AES_BLOCK_SIZE, au8ivector);

                        au8ivector[15] = au8ivector[15]+((OTA_6X_MAC_OFFSET - OTA_ENC_OFFSET)/OTA_AES_BLOCK_SIZE);

                        //bACI_ECBencodeStripe(&sOtaUseKey,TRUE,(tsReg128 *)au8ivector,(tsReg128 *)au8DataOut);
                        vOTA_EncodeString(&sOtaUseKey,au8ivector,au8DataOut);

                        for(u8Loop=0;u8Loop<16;u8Loop++)
                              au8TempBuffer[u8Loop] = (au8TempBuffer[u8Loop] ^ au8DataOut[u8Loop]) ;

                        u32Offset = OTA_6X_MAC_OFFSET;
                        u32Offset += (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8ImageIndex] * sNvmDefsStruct.u32SectorSize);
                        vOtaFlashLockWrite(psEndPointDefinition, psCustomData,u32Offset,16, au8TempBuffer);
#endif
                        /*
                        * We read the IV data from the image presently in execution in the Device
                        */

                        u32TempOffset = (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8ImageIndex] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;
                        u32TempOffset += OTA_IV_LOCATION;
                        vOtaFlashLockRead(psEndPointDefinition, psCustomData, u32TempOffset,OTA_AES_BLOCK_SIZE, au8ivector);

                        au8ivector[15] = au8ivector[15]+((u32OtaOffset - OTA_ENC_OFFSET)/OTA_AES_BLOCK_SIZE) + (80 / OTA_AES_BLOCK_SIZE);
                        //bACI_ECBencodeStripe(&sOtaUseKey,TRUE,(tsReg128 *)au8ivector,(tsReg128 *)au8DataOut);
                        vOTA_EncodeString(&sOtaUseKey,au8ivector,au8DataOut);
                        // For Copying Link Key

                        memcpy(au8TempBuffer, pu8LinkKey, 16);

                        for(u8Loop=0;u8Loop<16;u8Loop++)
                              au8TempBuffer[u8Loop] = (au8TempBuffer[u8Loop] ^ au8DataOut[u8Loop]);

                        u32Offset = pu8LinkKey - pu8Start;
                        u32Offset += (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8ImageIndex] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;
                        vOtaFlashLockWrite(psEndPointDefinition, psCustomData,u32Offset,16, au8TempBuffer);

                        // For Copying Certificate

                        memcpy(au8TempBuffer, pu8Cert, 48);

                        for(u8Loop=0;u8Loop<48;u8Loop++)
                        {
                            if((u8Loop % OTA_AES_BLOCK_SIZE) == 0 )
                            {
                                au8ivector[15]++;
                                //bACI_ECBencodeStripe(&sOtaUseKey,TRUE,(tsReg128 *)au8ivector,(tsReg128 *)au8DataOut);
                                vOTA_EncodeString(&sOtaUseKey,au8ivector,au8DataOut);
                            }
                            au8TempBuffer[u8Loop] = (au8TempBuffer[u8Loop] ^ au8DataOut[(u8Loop %16)]);
                        }

                        u32Offset = pu8Cert - pu8Start;
                        u32Offset += (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8ImageIndex] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;
                        vOtaFlashLockWrite(psEndPointDefinition, psCustomData,u32Offset,48, au8TempBuffer);

                        // For Copying Private Key
#ifdef COPY_PRIVATE_KEY                        

                        memcpy(au8TempBuffer, pu8PrvKey, 21);

                        for(u8Loop=0;u8Loop<21;u8Loop++)
                        {
                            if((u8Loop % OTA_AES_BLOCK_SIZE) == 0 )
                            {
                                au8ivector[15]++;
                                bACI_ECBencodeStripe(&sOtaUseKey,TRUE,(tsReg128 *)au8ivector,(tsReg128 *)au8DataOut);
                            }

                            au8TempBuffer[u8Loop] = (au8TempBuffer[u8Loop] ^ au8DataOut[(u8Loop %16)]);
                        }
                        u32Offset = pu8PrvKey - pu8Start;
                        u32Offset += (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8ImageIndex] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;
                        vOtaFlashLockWrite(psEndPointDefinition, psCustomData,u32Offset,21, au8TempBuffer);
#endif
                    }
                    else
                    {

                        DBG_vPrintf(TRACE_OTA_DEBUG, "OTA 202 ..\n");
#ifdef OTA_COPY_MAC_ADDRESS
                        memcpy(au8TempBuffer, pu8MacAddress, 8);
                        u32Offset = pu8MacAddress - pu8Start;
                        u32Offset += (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8ImageIndex] * sNvmDefsStruct.u32SectorSize);
                        vOtaFlashLockWrite(psEndPointDefinition, psCustomData,u32Offset,8, au8TempBuffer);
#endif

                            /* read link key from active image */
                        /* Read From Internal Flash */
                        memcpy(au8TempBuffer, pu8LinkKey, 16);

                         /* copy link key to new upgrade image */
                        u32Offset = pu8LinkKey - pu8Start;
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x) || (defined APP0)
                        u32Offset += (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8ImageIndex] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;
#endif
                        vOtaFlashLockWrite(psEndPointDefinition, psCustomData,u32Offset,16, au8TempBuffer);

                        /* Read From Internal Flash */
                        memcpy(au8TempBuffer, pu8Cert, 48);

                        /* copy certificate to new upgrade image */
                        u32Offset = pu8Cert - pu8Start;
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x) || (defined APP0)
                        u32Offset += (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8ImageIndex] * sNvmDefsStruct.u32SectorSize);
#endif
                        vOtaFlashLockWrite(psEndPointDefinition, psCustomData,u32Offset,48, au8TempBuffer);
#ifdef COPY_PRIVATE_KEY

                        /* Read From Internal Flash */
                        memcpy(au8TempBuffer, pu8PrvKey, 21);

                        /* copy private key to new upgrade image */
                        u32Offset = pu8PrvKey - pu8Start;
                        u32Offset += (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8ImageIndex] * sNvmDefsStruct.u32SectorSize);
                        vOtaFlashLockWrite(psEndPointDefinition, psCustomData,u32Offset,21, au8TempBuffer);
#endif                        
                    }


#ifdef OTA_MAINTAIN_CUSTOM_SERIALISATION_DATA
                    if(u8SizeOfCustomData > 0)
                    {

                        uint8 u8NoOf16BytesBlocks = u8SizeOfCustomData/16;
                        uint8 u8LoopCount,u8IntLoopCount;
                        uint32 u32ReadOffset = pu8CustData - pu8Start;
                        uint32 u32WriteOffset = pu8CustData - pu8Start;


                        if (bEncExternalFlash)
                        {

                            u32TempOffset = (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8ImageIndex] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;
                            u32TempOffset += OTA_IV_LOCATION;
                            vOtaFlashLockRead(psEndPointDefinition, psCustomData, u32TempOffset,OTA_AES_BLOCK_SIZE, au8ivector);
                            au8ivector[15] = au8ivector[15]+((u32OtaOffset - OTA_ENC_OFFSET)/OTA_AES_BLOCK_SIZE) + (80 / OTA_AES_BLOCK_SIZE) + ( (pu8CustData - pu8LinkKey) / OTA_AES_BLOCK_SIZE);
                            bACI_ECBencodeStripe(&sOtaUseKey,TRUE,(tsReg128 *)au8ivector,(tsReg128 *)au8DataOut);

                            u32WriteOffset += (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8ImageIndex] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;


                            for(u8LoopCount = 0; u8LoopCount < u8NoOf16BytesBlocks; u8LoopCount++)
                            {

                                memcpy(au8TempBuffer, (pu8CustData+(16*u8LoopCount)), 16);
                                for(u8IntLoopCount=0; u8IntLoopCount < 16;u8IntLoopCount++)
                                    au8TempBuffer[u8IntLoopCount] = (au8TempBuffer[u8IntLoopCount] ^ au8DataOut[(u8IntLoopCount)]);

                                vOtaFlashLockWrite(psEndPointDefinition, psCustomData, (u32WriteOffset + (16*u8LoopCount)), 16, au8TempBuffer);

                                au8ivector[15]++;

                                bACI_ECBencodeStripe(&sOtaUseKey,TRUE,(tsReg128 *)au8ivector,(tsReg128 *)au8DataOut);
                            }

                            /* copy remaining data */
                            u8NoOf16BytesBlocks = u8SizeOfCustomData%16;

                            memcpy(au8TempBuffer, (pu8CustData+(16*u8LoopCount)), u8NoOf16BytesBlocks);
                            for(u8LoopCount = 0; u8LoopCount < u8NoOf16BytesBlocks; u8LoopCount++)
                                au8TempBuffer[u8LoopCount] = (au8TempBuffer[u8LoopCount] ^ au8DataOut[(u8LoopCount)]);
                            vOtaFlashLockWrite(psEndPointDefinition, psCustomData, (u32WriteOffset + (16*u8LoopCount)), u8NoOf16BytesBlocks, au8TempBuffer);
                        }
                        else
                        {

                            u32WriteOffset += (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8ImageIndex] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;


                            for(u8LoopCount = 0; u8LoopCount < u8NoOf16BytesBlocks; u8LoopCount++)
                            {

                                memcpy(au8TempBuffer, pu8CustData+(16*u8LoopCount), 16);
                                vOtaFlashLockWrite(psEndPointDefinition, psCustomData, (u32WriteOffset + (16*u8LoopCount)), 16, au8TempBuffer);

                            }

                            /* copy remaining data */
                            u8NoOf16BytesBlocks = u8SizeOfCustomData%16;

                            memcpy(au8TempBuffer, pu8CustData+(16*u8LoopCount), 16);
                            vOtaFlashLockWrite(psEndPointDefinition, psCustomData, (u32WriteOffset + (16*u8LoopCount)), u8NoOf16BytesBlocks, au8TempBuffer);
                        }
                    }
#endif
                }
#endif
            }
            else
            {
                return E_ZCL_FAIL;
            }
            /* validate New image */
            vOTA_SetImageValidityFlag(u8ImageIndex,psCustomData,TRUE, psEndPointDefinition);
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
            /* Invalidate Internal Flash Header */
            bAHI_FlashInit(E_FL_CHIP_INTERNAL, NULL); /* Pass Internal Flash */
             /* Erase Internal Flash Header */
            bAHI_FullFlashProgram(0x00,16,au8Data);
#else
            u32Offset = (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8ImageIndex] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;
            /* Read header marker and offset to boot block */
            vOtaFlashLockRead(psEndPointDefinition, psCustomData, (u32Offset + OTA_BOOTLOADER_HEADER_MARKER_OFFSET), 8, au8Data);

#if 0
            __disable_irq();
            vOtaFlashValidatInvalidatImage(0, psCustomData, psEndPointDefinition);
#endif
#ifdef APP0
            u32Offset = ( psCustomData->sOTACallBackMessage.u8ImageStartSector[u8ImageIndex] *
                    sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION ;
             psector_SetEscoreImageData(   u32Offset , 0);
            __disable_irq();
            SYSCON->MEMORYREMAP &= ~( SYSCON_MEMORYREMAP_MAP_MASK << SYSCON_MEMORYREMAP_MAP_SHIFT);
            PMC->CTRL &= ~(PMC_CTRL_LPMODE_MASK << PMC_CTRL_LPMODE_SHIFT);
#else
            /* Get image identifier from header */
            OTA_CommitImage(NULL);
            OTA_SetNewImageFlag();
#endif

#endif
            /* switch to new image */
            vOtaSwitchLoads();
        }



    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       eOTA_InvalidateStoredImage
 **
 ** DESCRIPTION: Invalidating OTA Client Image. This allow server stops serving image to client in this image index.
 ** Application should call this function before upgrading client image.
 **
 ** PARAMETERS:                     Name                             Usage
 ** uint8                        u8Endpoint                        Endpoint
 ** uint8                        u8ImageIndex                      Image index
 ** RETURN: teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_InvalidateStoredImage(
                                                uint8     u8Endpoint,
                                                uint8     u8ImageIndex)
{
    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;
    uint8 au8Temp[4] = {0};
    uint32 u32FlashOffset,u32HeaderType;


    /* Validate parameters */
    if(u8ImageIndex >= OTA_MAX_IMAGES_PER_ENDPOINT)
    {
        return E_ZCL_ERR_EP_RANGE;
    }

    /* Find OTA server cluster */
    if((eStatus = eOtaFindCluster(u8Endpoint,
                         &psEndPointDefinition,
                           &psClusterInstance,
                           &psCustomData,
                           TRUE))
                           == E_ZCL_SUCCESS)
    {

        u32FlashOffset = psCustomData->sOTACallBackMessage.u8ImageStartSector[u8ImageIndex] * sNvmDefsStruct.u32SectorSize * OTA_SECTOR_CONVERTION;

        vOtaFlashLockRead(psEndPointDefinition, psCustomData,u32FlashOffset,4, au8Temp);
        vReverseMemcpy((uint8*)&u32HeaderType,&au8Temp[0],sizeof(uint32));
        if(u32HeaderType == OTA_FILE_IDENTIFIER)
        {
            //Invalidate first 4 byte of file idetifier
            memset(au8Temp,0,4);
#if (defined KSDK2)
#ifdef APP0
           g_u16OtaPageIndex = 0;
#else
           OTA_CancelImage();
#endif
#endif
            vOtaFlashLockWrite(psEndPointDefinition, psCustomData,u32FlashOffset,4, au8Temp);
#if (defined gEepromParams_bufferedWrite_c) || (defined APP0)
#if (defined APP0)
            bFlashWriteBufferedTailEndBytes(u32FlashOffset);
#else
            OTA_CommitImage(NULL);
#endif
#endif
        }
    }
    return eStatus;
}


/****************************************************************************
 **
 ** NAME:       eOTA_EraseFlashSectorsForNewImage
 **
 ** DESCRIPTION: Erasing Old Image
 **
 ** PARAMETERS:                     Name                             Usage
 ** uint8                        u8Endpoint                        Endpoint
 ** uint8                        u8ImageIndex                      Image index
 ** RETURN: teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_EraseFlashSectorsForNewImage(
                                                    uint8     u8Endpoint,
                                                    uint8     u8ImageIndex)
{
    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;
    uint8 i;

    /* Validate parameters */
    if(u8ImageIndex >= OTA_MAX_IMAGES_PER_ENDPOINT)
    {
        return E_ZCL_ERR_EP_RANGE;
    }

    /* Find OTA server cluster */
    if((eStatus = eOtaFindCluster(u8Endpoint,
                         &psEndPointDefinition,
                           &psClusterInstance,
                           &psCustomData,
                           TRUE))
                           == E_ZCL_SUCCESS)
    {

        /* Erase Flahs for upgrading New Image */
        for(i=0; i< psCustomData->sOTACallBackMessage.u8MaxNumberOfSectors; i++)
        {
            DBG_vPrintf(TRACE_OTA_DEBUG, "Erasing flash location start %d, shadow bank %d \n", psCustomData->sOTACallBackMessage.u8ImageStartSector[u8ImageIndex]+i,u8ImageIndex );
            vOtaFlashLockErase(psEndPointDefinition, psCustomData, (uint8)(psCustomData->sOTACallBackMessage.u8ImageStartSector[u8ImageIndex]+i));
        }
    }
    return eStatus;
}
/****************************************************************************
 **
 ** NAME:       eOTA_SetWaitForDataParams
 **
 ** DESCRIPTION:
 ** set wait for data parameters
 ** PARAMETERS:                     Name                             Usage
 ** uint8                        u8Endpoint                        Endpoint
 ** uint16                       u16ClientAddress                  Client address
 ** tsOTA_WaitForData           *sWaitForDataParams                wait for data paramters
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_SetWaitForDataParams(
                                                uint8              u8Endpoint,
                                                uint16             u16ClientAddress,
                                                tsOTA_WaitForData *sWaitForDataParams)
{
    teZCL_Status eStatus;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;
    if((eStatus = eOtaFindCluster(u8Endpoint,
                         &psEndPointDefinition,
                           &psClusterInstance,
                           &psCustomData,
                           TRUE))
                           == E_ZCL_SUCCESS)
    {
        psCustomData->sOTACallBackMessage.sWaitForDataParams.u16ClientAddress = u16ClientAddress;
        memcpy(&psCustomData->sOTACallBackMessage.sWaitForDataParams.sWaitForDataPyld,sWaitForDataParams, sizeof(tsOTA_WaitForData));
        psCustomData->sOTACallBackMessage.sWaitForDataParams.bInitialized = TRUE;
    }
    return eStatus;
}
#endif


/****************************************************************************
 **
 ** NAME:       vOTA_FlashInit
 **
 ** DESCRIPTION:
 ** initialize flash
 ** PARAMETERS:                     Name                             Usage
 ** void                        *pvFlashTable                     Flash table
 ** tsNvmDefs                   *psNvmStruct                      Nvm structure
 ** RETURN:
 ** None
 ****************************************************************************/
PUBLIC  void vOTA_FlashInit(
                                  void        *pvFlashTable,
                                  tsNvmDefs   *psNvmStruct)
{
    vOtaFlashInitHw(psNvmStruct->u8FlashDeviceType, NULL);
    sNvmDefsStruct.u8FlashDeviceType = psNvmStruct->u8FlashDeviceType;
    sNvmDefsStruct.u32SectorSize = psNvmStruct->u32SectorSize;
    if(psNvmStruct->u8FlashDeviceType != 4)
    {
        sNvmDefsStruct.sOtaFnTable.prEraseCb = vOtaFlashErase;
        sNvmDefsStruct.sOtaFnTable.prInitHwCb = vOtaFlashInitHw;
        sNvmDefsStruct.sOtaFnTable.prReadCb = vOtaFlashRead;
        sNvmDefsStruct.sOtaFnTable.prWriteCb = vOtaFlashWrite;
    }
    else
    {
        sNvmDefsStruct.sOtaFnTable = psNvmStruct->sOtaFnTable;
    }
}
/****************************************************************************
 **
 ** NAME:       sOtaGetHeader
 **
 ** DESCRIPTION:
 ** provide ota header
 ** PARAMETERS:                     Name                             Usage
 ** uint8                       *pu8HeaderBytes                   header pointer to update
 ** RETURN:
 ** tsOTA_ImageHeader
 ****************************************************************************/
PUBLIC  tsOTA_ImageHeader sOtaGetHeader(uint8 *pu8HeaderBytes)
{
    tsOTA_ImageHeader sTempStruct;
    memset(&sTempStruct, 0, sizeof(sTempStruct));
    
    vReverseMemcpy((uint8*)&sTempStruct.u32FileIdentifier,&pu8HeaderBytes[0],sizeof(uint32));
    vReverseMemcpy((uint8*)&sTempStruct.u16HeaderVersion,&pu8HeaderBytes[4],sizeof(uint16));
    vReverseMemcpy((uint8*)&sTempStruct.u16HeaderControlField,&pu8HeaderBytes[8],sizeof(uint16));
    vReverseMemcpy((uint8*)&sTempStruct.u16ManufacturerCode,&pu8HeaderBytes[10],sizeof(uint16));
    vReverseMemcpy((uint8*)&sTempStruct.u16ImageType,&pu8HeaderBytes[12],sizeof(uint16));
    vReverseMemcpy((uint8*)&sTempStruct.u32FileVersion,&pu8HeaderBytes[14],sizeof(uint32));
    vReverseMemcpy((uint8*)&sTempStruct.u16StackVersion,&pu8HeaderBytes[18],sizeof(uint16));

    memcpy(&sTempStruct.stHeaderString[0],&pu8HeaderBytes[20],OTA_HEADER_STRING_SIZE);
    vReverseMemcpy((uint8*)&sTempStruct.u32TotalImage,&pu8HeaderBytes[52],sizeof(uint32));

    if(sTempStruct.u16HeaderControlField & OTA_HDR_SECURITY_CRED_VER_PRESENT)
    {
        sTempStruct.u8SecurityCredVersion = pu8HeaderBytes[56];
        if( sTempStruct.u16HeaderControlField & OTA_HDR_DEV_SPECF_FILE )
        {
            /* ZCL_OPT_2 Not sure about the change */
            vReverseMemcpy((uint8*)&sTempStruct.u64UpgradeFileDest,&pu8HeaderBytes[57],sizeof(uint64));
            
             if(sTempStruct.u16HeaderControlField & OTA_HDR_HW_VER_PRESENT)
             {
                 vReverseMemcpy((uint8*)&sTempStruct.u16MinimumHwVersion,&pu8HeaderBytes[65],sizeof(uint16));
                 vReverseMemcpy((uint8*)&sTempStruct.u16MaxHwVersion,&pu8HeaderBytes[67],sizeof(uint16));
             }
        }
    }
    else if((sTempStruct.u16HeaderControlField & OTA_HDR_DEV_SPECF_FILE ))
    {
        vReverseMemcpy((uint8*)&sTempStruct.u64UpgradeFileDest,&pu8HeaderBytes[56],sizeof(uint64));
        if(sTempStruct.u16HeaderControlField & OTA_HDR_HW_VER_PRESENT)
        {
            vReverseMemcpy((uint8*)&sTempStruct.u16MinimumHwVersion,&pu8HeaderBytes[64],sizeof(uint16));
            vReverseMemcpy((uint8*)&sTempStruct.u16MaxHwVersion,&pu8HeaderBytes[66],sizeof(uint16));
        }
    }
    else if(sTempStruct.u16HeaderControlField & OTA_HDR_HW_VER_PRESENT)
    {
        vReverseMemcpy((uint8*)&sTempStruct.u16MinimumHwVersion,&pu8HeaderBytes[56],sizeof(uint16));
        vReverseMemcpy((uint8*)&sTempStruct.u16MaxHwVersion,&pu8HeaderBytes[58],sizeof(uint16));
    }
    return sTempStruct;
}

#ifdef SOTA_ENABLED
/****************************************************************************
 **
 ** NAME:       sOtaInitBlobOtaHeader
 **
 ** DESCRIPTION:
 ** provide ota header
 ** PARAMETERS:                     Name                             Usage
 ** uint16_t                      u16BlobId                         targeted blob id
 ** tsOTA_ImageHeader          *pBlobOtaHeader                   header pointer to update
 ** RETURN:
 ** tsOTA_ImageHeader
 ****************************************************************************/
PUBLIC bool_t sOtaInitBlobOtaHeader(uint16 u16BlobId, tsOTA_ImageHeader *pBlobOtaHeader)
{
    bool_t result = FALSE;
    Sota_BlobUtilsFunction blobFunctionGetZgOtaHeader = NULL;
    blobFunctionGetZgOtaHeader = Sota_GetZbOtaHeaderFunction(u16BlobId);
    if (blobFunctionGetZgOtaHeader != NULL)
    {
        *pBlobOtaHeader = sOtaGetHeader(blobFunctionGetZgOtaHeader());
        result = TRUE;
    }
    return result;
}
#endif

/****************************************************************************
 **
 ** NAME:       eOTA_AllocateEndpointOTASpace
 **
 ** DESCRIPTION:
 ** Allocate space for OTA upgrade images
 ** PARAMETERS:                     Name                             Usage
 ** uint8                         u8Endpoint                       Endpoint Id
 ** uint8                        *pu8Data                          Start sectors
 ** uint8                         u8NumberOfImages                 Number of images
 ** uint8                         u8MaxSectorsPerImage             Max sectors per image
 ** bool_t                        bIsServer                        Is server
 ** uint8                        *pu8CAPublicKey                   CA public key
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_AllocateEndpointOTASpace(
                                              uint8     u8Endpoint,
                                              uint8    *pu8Data,
                                              uint8     u8NumberOfImages,
                                              uint8     u8MaxSectorsPerImage,
                                              bool_t    bIsServer,
                                              uint8    *pu8CAPublicKey)
{
    teZCL_Status eStatus;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;
    uint8 i;
#ifdef OTA_CLIENT
    tsOTA_ImageHeader sCurrent;
#endif

    if(u8NumberOfImages != OTA_MAX_IMAGES_PER_ENDPOINT)
    {
        return E_ZCL_ERR_INVALID_VALUE;
    }
    if((eStatus = eOtaFindCluster(u8Endpoint,
                         &psEndPointDefinition,
                           &psClusterInstance,
                           &psCustomData,
                           bIsServer)) == E_ZCL_SUCCESS)
    {
        if(pu8CAPublicKey != NULL)
        {
            memcpy(psCustomData->sOTACallBackMessage.au8CAPublicKey, pu8CAPublicKey, 22);
        }

        psCustomData->sOTACallBackMessage.u8MaxNumberOfSectors = u8MaxSectorsPerImage;
        for(i = 0; i<u8NumberOfImages; i++)
        {
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x) || (defined APP0) || defined OTA_SERVER
            psCustomData->sOTACallBackMessage.u8ImageStartSector[i] = pu8Data[i];
#endif

#ifdef OTA_CLIENT
            /* Read Starting Flash Magic Numbers */

            sCurrent = sOtaGetHeader(&au8OtaHeader[0]);

            /* Check Valid Embedded OTA header present or not */
            if(sCurrent.u32FileIdentifier == OTA_FILE_IDENTIFIER && sCurrent.u16HeaderLength <= OTA_MAX_HEADER_SIZE)
            {
                psCustomData->sOTACallBackMessage.u8NextFreeImageLocation = 0;

                DBG_vPrintf(TRACE_OTA_DEBUG, "OTA Next Image Location 0x%02x ", psCustomData->sOTACallBackMessage.u8NextFreeImageLocation);
            }
            else
            {
                return E_ZCL_FAIL;
            }
#endif /* OTA_CLIENT */
        }
    }

    return eStatus;
}

/****************************************************************************
 **
 ** NAME:       bOtaIsImageValid
 **
 ** DESCRIPTION:
 ** Validates the image
 ** PARAMETERS:                     Name                             Usage
 ** uint8                        *pu8Data                          magic number bytes
 ** RETURN:
 ** bool
 ****************************************************************************/
PUBLIC bool_t bOtaIsImageValid( tsOTA_Common*    psOTA_Common,
                              uint8*           pu8Data )
{
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)

    #ifndef LITTLE_ENDIAN_PROCESSOR  /*JN517x*/
        uint8 au8Values[OTA_FLS_MAGIC_NUMBER_LENGTH] = {0x12,0x34,0x56,0x78,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88};
    #else /*JN516x*/
        uint8 au8Values[OTA_FLS_MAGIC_NUMBER_LENGTH] = {0x78,0x56,0x34,0x12,0x44,0x33,0x22,0x11,0x88,0x77,0x66,0x55};
    #endif
    return ((memcmp(pu8Data, au8Values, OTA_FLS_MAGIC_NUMBER_LENGTH) == 0)? TRUE : FALSE);

#else
    /*JN518x */
    uint8 au8Values[OTA_BOOTLOADER_BLOCK_LENGTH] = {0xbb ,0x10 ,0x01 ,0xbb ,0x00 ,0x00 ,0x00 ,0x00  };
#ifdef APP0 /* Building with selective OTA */
    if(psOTA_Common->sOTACallBackMessage.sPersistedData.bStackDownloadActive)
    {
        au8Values[4] = 0x01;
    }
#endif

    /* Special case for first byte as can increase based on image identifier */
    if (   (pu8Data[0] >= au8Values[0])
#ifdef SOTA_ENABLED
        && (memcmp(&pu8Data[1], &au8Values[1], OTA_BOOTLOADER_BLOCK_LENGTH - 5) == 0)
#else
        && (memcmp(&pu8Data[1], &au8Values[1], OTA_BOOTLOADER_BLOCK_LENGTH - 1) == 0)
#endif
       )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }

#endif
}

/****************************************************************************
 **
 ** NAME:       vOTA_SetImageValidityFlag
 **
 ** DESCRIPTION:
 ** Set image validity flag
 ** PARAMETERS:                     Name                             Usage
 ** uint8                        u8Location                        Image location
 ** tsOTA_Common                *psCustomData                      OTA custom data
 ** bool_t                         bSet                              is set
 ** tsZCL_EndPointDefinition    *psEndPointDefinition              endpoint definition
 ** RETURN:
 ** None
 ****************************************************************************/
PUBLIC  void vOTA_SetImageValidityFlag(
                                uint8                        u8Location,
                                tsOTA_Common                *psCustomData,
                                bool_t                         bSet,
                                tsZCL_EndPointDefinition    *psEndPointDefinition)
{
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
    if(bSet)
    {
        /*JN516x or JN517x*/
#if !(defined OTA_INTERNAL_STORAGE) || !(defined KSDK2)
        #ifndef LITTLE_ENDIAN_PROCESSOR  /*JN517x*/
            uint8 au8Values[OTA_FLS_MAGIC_NUMBER_LENGTH] = {0x12,0x34,0x56,0x78,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88}; // TODO fill the magic value
        #else /*JN516x*/
                uint8 au8Values[OTA_FLS_MAGIC_NUMBER_LENGTH] = {0x78,0x56,0x34,0x12,0x44,0x33,0x22,0x11,0x88,0x77,0x66,0x55}; // TODO fill the magic value
        #endif

        uint32     u32Offset =  (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8Location] * sNvmDefsStruct.u32SectorSize)
                                + OTA_FLS_VALIDITY_OFFSET;

        vOtaFlashLockWrite(psEndPointDefinition, psCustomData, u32Offset,OTA_FLS_MAGIC_NUMBER_LENGTH, au8Values);

#else /*External flash JN516x or JN517x*/
        #ifndef LITTLE_ENDIAN_PROCESSOR  /*JN517x*/
            uint8 au8Values[OTA_FLS_MAGIC_NUMBER_LENGTH + 4] = {0x12,0x34,0x56,0x78,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88}; // TODO fill the magic value
        #else /*JN516x*/
            uint8 au8Values[OTA_FLS_MAGIC_NUMBER_LENGTH + 4] = {0x78,0x56,0x34,0x12,0x44,0x33,0x22,0x11,0x88,0x77,0x66,0x55}; // TODO fill the magic value
        #endif
       
        uint32     u32Offset =  (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8Location] * sNvmDefsStruct.u32SectorSize)
                                + OTA_FLS_VALIDITY_OFFSET;

        au8Values[12] = psCustomData->sOTACallBackMessage.sPersistedData.u8Buf[0];
        au8Values[13] = psCustomData->sOTACallBackMessage.sPersistedData.u8Buf[1];
        au8Values[14] = psCustomData->sOTACallBackMessage.sPersistedData.u8Buf[2];
        au8Values[15] = psCustomData->sOTACallBackMessage.sPersistedData.u8Buf[3];
        
        DBG_vPrintf(TRACE_OTA_DEBUG, "Magic write u32Offset %08x", u32Offset);
        vOtaFlashLockWrite(psEndPointDefinition, psCustomData, u32Offset,OTA_FLS_MAGIC_NUMBER_LENGTH+4, au8Values);
#endif /*External flash JN516x or JN517x */
    }
    else
    {
        /*JN516x or JN517x */
        uint8 au8Values[OTA_FLS_MAGIC_NUMBER_LENGTH] = {0,0,0,0,0,0,0,0,0,0,0,0};

        uint32 u32Offset =  (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8Location] * sNvmDefsStruct.u32SectorSize)
                                + OTA_FLS_VALIDITY_OFFSET;

        /* clear Flash cookie/magic number */
        vOtaFlashLockWrite(psEndPointDefinition, psCustomData, u32Offset,OTA_FLS_MAGIC_NUMBER_LENGTH, au8Values);

        /* Clear flash status byte which is located at 13th byte */
        vOtaFlashLockWrite(psEndPointDefinition, psCustomData, (u32Offset+13), 0x01, au8Values);
    }
#else /* end of JN516x OR JN517x */
    /*JN518x*/
#if (defined OTA_INTERNAL_STORAGE) || (defined KSDK2)
#ifdef APP0 /* Building with selective OTA */
    /* The first page should be invalid at this point, we should now validate it*/
    (void)(bSet);
    __disable_irq();
    uint32 u32StartLocation = 0;
     if(psCustomData->sOTACallBackMessage.sPersistedData.bStackDownloadActive)
     {
         u32StartLocation = OTA_APP1_SHADOW_FLASH_OFFSET;
     }
     else
     {
         u32StartLocation = ( psCustomData->sOTACallBackMessage.u8ImageStartSector[u8Location] *
                 sNvmDefsStruct.u32SectorSize * OTA_SECTOR_CONVERTION);
     }

    vOtaFlashValidatInvalidatImage(  u32StartLocation,
                                      psCustomData,
                                      psEndPointDefinition );
    __enable_irq();
#endif
#endif
#endif
}

#ifdef OTA_CLIENT
#if (OTA_MAX_CO_PROCESSOR_IMAGES != 0)
/****************************************************************************
 **
 ** NAME:       vOTA_UpdateCoProcessorOTAHeader
 **
 ** DESCRIPTION:
 ** updates Co Processor OTA header
 ** PARAMETERS:                        Name                             Usage
 ** tsOTA_CoProcessorOTAHeader   *psOTA_CoProcessorOTAHeader           Co Processor OTA header
 ** bool_t                        bIsCoProcessorImageUpgradeDependent  Is CoProcessor Image Upgrade Dependent
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_UpdateCoProcessorOTAHeader(
                                tsOTA_CoProcessorOTAHeader *psOTA_CoProcessorOTAHeader,
                                bool_t                      bIsCoProcessorImageUpgradeDependent)
{
    uint8 u8LoopCount = 0;
    uint8 u8HeaderBytesWritten;

    /* validate parameters */
    if((psOTA_CoProcessorOTAHeader == NULL)||(bIsCoProcessorImageUpgradeDependent > 1))
    {
        return E_ZCL_ERR_PARAMETER_NULL;
    }


    /* copy OTA CoProcessor Headers to asCommonCoProcessorOTAHeader array */
    for(u8LoopCount = 0; u8LoopCount < OTA_MAX_CO_PROCESSOR_IMAGES; u8LoopCount++)
    {
        u8HeaderBytesWritten = 0;

        /* As the OTA header comes in big endian format, to compatible with existing code copy little endian format with individual fields */
        /* copy File Identifier in Little Endian format */
        u8HeaderBytesWritten += u16ZCL_WriteTypeNBO(&asCommonCoProcessorOTAHeader[u8LoopCount][u8HeaderBytesWritten], E_ZCL_UINT32,
                (uint8*)&psOTA_CoProcessorOTAHeader->sOTA_ImageHeader[u8LoopCount].u32FileIdentifier);

        /* Copy Header Version */
        u8HeaderBytesWritten += u16ZCL_WriteTypeNBO(&asCommonCoProcessorOTAHeader[u8LoopCount][u8HeaderBytesWritten], E_ZCL_UINT16,
                        (uint8*)&psOTA_CoProcessorOTAHeader->sOTA_ImageHeader[u8LoopCount].u16HeaderVersion);

        /* Copy Header Length */
        u8HeaderBytesWritten += u16ZCL_WriteTypeNBO(&asCommonCoProcessorOTAHeader[u8LoopCount][u8HeaderBytesWritten], E_ZCL_UINT16,
                        (uint8*)&psOTA_CoProcessorOTAHeader->sOTA_ImageHeader[u8LoopCount].u16HeaderLength);

        /* Copy Header Control Field */
        u8HeaderBytesWritten += u16ZCL_WriteTypeNBO(&asCommonCoProcessorOTAHeader[u8LoopCount][u8HeaderBytesWritten], E_ZCL_UINT16,
                        (uint8*)&psOTA_CoProcessorOTAHeader->sOTA_ImageHeader[u8LoopCount].u16HeaderControlField);

        /* Copy Header Manufacturer code  */
        u8HeaderBytesWritten += u16ZCL_WriteTypeNBO(&asCommonCoProcessorOTAHeader[u8LoopCount][u8HeaderBytesWritten], E_ZCL_UINT16,
                        (uint8*)&psOTA_CoProcessorOTAHeader->sOTA_ImageHeader[u8LoopCount].u16ManufacturerCode);

        /* Copy Header Image Type  */
        u8HeaderBytesWritten += u16ZCL_WriteTypeNBO(&asCommonCoProcessorOTAHeader[u8LoopCount][u8HeaderBytesWritten], E_ZCL_UINT16,
                        (uint8*)&psOTA_CoProcessorOTAHeader->sOTA_ImageHeader[u8LoopCount].u16ImageType);

        /* Copy File Version  */
        u8HeaderBytesWritten += u16ZCL_WriteTypeNBO(&asCommonCoProcessorOTAHeader[u8LoopCount][u8HeaderBytesWritten], E_ZCL_UINT32,
                        (uint8*)&psOTA_CoProcessorOTAHeader->sOTA_ImageHeader[u8LoopCount].u32FileVersion);

        /* Copy Stack Version Type  */
        u8HeaderBytesWritten += u16ZCL_WriteTypeNBO(&asCommonCoProcessorOTAHeader[u8LoopCount][u8HeaderBytesWritten], E_ZCL_UINT16,
                        (uint8*)&psOTA_CoProcessorOTAHeader->sOTA_ImageHeader[u8LoopCount].u16StackVersion);

        /* Copy Head String  is not required as it is in little endian format */
        u8HeaderBytesWritten += OTA_HEADER_STRING_SIZE;

        /* Copy total image size field */
        u8HeaderBytesWritten += u16ZCL_WriteTypeNBO(&asCommonCoProcessorOTAHeader[u8LoopCount][u8HeaderBytesWritten], E_ZCL_UINT32,
                        (uint8*)&psOTA_CoProcessorOTAHeader->sOTA_ImageHeader[u8LoopCount].u32TotalImage);

        /* Copy Security credential field */
        u8HeaderBytesWritten += u16ZCL_WriteTypeNBO(&asCommonCoProcessorOTAHeader[u8LoopCount][u8HeaderBytesWritten], E_ZCL_UINT8,
                        (uint8*)&psOTA_CoProcessorOTAHeader->sOTA_ImageHeader[u8LoopCount].u8SecurityCredVersion);

        /* Copy upgrade destination field */
        u8HeaderBytesWritten += u16ZCL_WriteTypeNBO(&asCommonCoProcessorOTAHeader[u8LoopCount][u8HeaderBytesWritten], E_ZCL_UINT64,
                        (uint8*)&psOTA_CoProcessorOTAHeader->sOTA_ImageHeader[u8LoopCount].u64UpgradeFileDest);

        /* Copy Minimum Hardware Version field */
        u8HeaderBytesWritten += u16ZCL_WriteTypeNBO(&asCommonCoProcessorOTAHeader[u8LoopCount][u8HeaderBytesWritten], E_ZCL_UINT16,
                        (uint8*)&psOTA_CoProcessorOTAHeader->sOTA_ImageHeader[u8LoopCount].u16MinimumHwVersion);

        /* Copy Maximum Hardware Version field */
        u8HeaderBytesWritten += u16ZCL_WriteTypeNBO(&asCommonCoProcessorOTAHeader[u8LoopCount][u8HeaderBytesWritten], E_ZCL_UINT16,
                        (uint8*)&psOTA_CoProcessorOTAHeader->sOTA_ImageHeader[u8LoopCount].u16MaxHwVersion);
    }
    bIsCoProcessorImgUpgdDependent = bIsCoProcessorImageUpgradeDependent;
    return E_ZCL_SUCCESS;
}
#endif
#endif /* OTA_CLIENT */

/****************************************************************************
 **
 ** NAME:       vOTA_GenerateHash
 **
 ** DESCRIPTION:
 ** calculates hash of the image
 ** PARAMETERS:                        Name                             Usage
 ** tsZCL_EndPointDefinition   *psEndPointDefinition         Endpoint definition
 ** tsOTA_Common               *psCustomData                 OTA custom data
 ** bool_t                        bIsServer                    Is server
 ** bool_t                        bHeaderPresent               Is header present
 ** AESSW_Block_u              *puHash                       Hash
 ** uint8                       u8ImageLocation              Image location
 ** RETURN:
 ** None
 ****************************************************************************/
PUBLIC  void vOTA_GenerateHash(
                                tsZCL_EndPointDefinition *psEndPointDefinition,
                                tsOTA_Common             *psCustomData,
                                bool_t                      bIsServer,
                                bool_t                      bHeaderPresent,
                                AESSW_Block_u            *puHash,
                                uint8                     u8ImageLocation)
{
    AESSW_Block_u uBuf;
    uint32 u32ImageSize, u32TotalImageBlocks,u32FlashOffset,i,u32TagLength;
    int iBytes;
    uint8 u8BlockElements, au8Tag[OTA_TAG_HEADER_SIZE];
    uint8 j;
    uint8 au8Buffer[OTA_MAX_HEADER_SIZE+OTA_TAG_HEADER_SIZE+OTA_FLS_MAGIC_NUMBER_LENGTH];
    uint8 u8Length;
    uint16 u16HeaderSize = OTA_MIN_HEADER_SIZE;

    memset(uBuf.au8, 0, AESSW_BLK_SIZE);
    memset(puHash->au8, 0, AESSW_BLK_SIZE);

    psCustomData->sOTACallBackMessage.eEventId = E_CLD_OTA_INTERNAL_COMMAND_LOCK_FLASH_MUTEX;
    psEndPointDefinition->pCallBackFunctions(&psCustomData->sOTACustomCallBackEvent);

    /* If header is present in flash, read it from flash otherwise from custom struct */
    if(bHeaderPresent)
    {
        u32FlashOffset = (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8ImageLocation] *
                    sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;

        /* First read Min Header Size */
        sNvmDefsStruct.sOtaFnTable.prReadCb(u32FlashOffset, OTA_MIN_HEADER_SIZE, au8Buffer);

        /* Calculate Header Size */
        vReverseMemcpy((uint8*)&u16HeaderSize,&au8Buffer[6],sizeof(uint16));

        /* Read Reamining Header Bytes */
        sNvmDefsStruct.sOtaFnTable.prReadCb((u32FlashOffset+OTA_MIN_HEADER_SIZE), (u16HeaderSize - OTA_MIN_HEADER_SIZE), (au8Buffer+OTA_MIN_HEADER_SIZE));
    }
    else
    {
        /* Server/Client */
        if(bIsServer)
        {
#ifdef OTA_SERVER
            vReverseMemcpy((uint8*)&u16HeaderSize,&psCustomData->sOTACallBackMessage.au8ServerOTAHeader[6],sizeof(uint16));
            memcpy(au8Buffer, psCustomData->sOTACallBackMessage.au8ServerOTAHeader, u16HeaderSize);
#endif
        }
        else
        {
#ifdef OTA_CLIENT
            vReverseMemcpy((uint8*)&u16HeaderSize,&psCustomData->sOTACallBackMessage.sPersistedData.au8Header[6],sizeof(uint16));
            memcpy(au8Buffer, psCustomData->sOTACallBackMessage.sPersistedData.au8Header, u16HeaderSize);
#endif
        }
    }

    vReverseMemcpy((uint8*)&u32ImageSize,&au8Buffer[52],sizeof(uint32));
    u32ImageSize -= OTA_SIGNITURE_SIZE;
    u32TagLength = u32ImageSize - u16HeaderSize - (3*OTA_TAG_HEADER_SIZE) - 0x30 - 8;

    DBG_vPrintf(TRACE_OTA_DEBUG, "\n u32ImageSize %d u32TagLength %d \n", u32ImageSize, u32TagLength);
    au8Tag[0] = 0;
    au8Tag[1] = 0;
    
    vReverseMemcpy(&au8Tag[2],(uint8*)&u32TagLength,sizeof(uint32));

    /* Copy Tag Id */
    memcpy(au8Buffer+u16HeaderSize, au8Tag, OTA_TAG_HEADER_SIZE);

    if(bHeaderPresent)
    {

        u32FlashOffset = ( (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8ImageLocation] *
                        sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION) + u16HeaderSize + OTA_TAG_HEADER_SIZE;

        u32TotalImageBlocks = (u16HeaderSize+OTA_TAG_HEADER_SIZE) / AESSW_BLK_SIZE;
        u8Length = (u16HeaderSize+OTA_TAG_HEADER_SIZE);
    }
    else
    {
        if(u32TagLength)
        {
            
            #ifndef LITTLE_ENDIAN_PROCESSOR 
                uint8 au8MagicFlashNumbers[OTA_FLS_MAGIC_NUMBER_LENGTH] = {0x12,0x34,0x56,0x78,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88}; // TODO fill the magic value
            #else
              #if (defined JENNIC_CHIP_FAMILY_JN517x)
                    uint8 au8MagicFlashNumbers[OTA_FLS_MAGIC_NUMBER_LENGTH] = {0x78,0x56,0x34,0x12,0x44,0x33,0x22,0x11,0x88,0x77,0x66,0x55}; // TODO fill the magic value
              #endif
            #endif
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
            /* Copy  Magic Number */
            memcpy(au8Buffer+u16HeaderSize+OTA_TAG_HEADER_SIZE, au8MagicFlashNumbers, OTA_FLS_MAGIC_NUMBER_LENGTH);
#endif
            u32FlashOffset = (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8ImageLocation] *
                                    sNvmDefsStruct.u32SectorSize * OTA_SECTOR_CONVERTION) + OTA_FLS_MAGIC_NUMBER_LENGTH;

            u32TotalImageBlocks = (u16HeaderSize+ OTA_TAG_HEADER_SIZE + OTA_FLS_MAGIC_NUMBER_LENGTH) / AESSW_BLK_SIZE;
            u8Length = (u16HeaderSize+ OTA_TAG_HEADER_SIZE + OTA_FLS_MAGIC_NUMBER_LENGTH);

        }
        else
        {
            u32FlashOffset = (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8ImageLocation] *
                                    sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION;

            u32TotalImageBlocks = (u16HeaderSize + OTA_TAG_HEADER_SIZE) / AESSW_BLK_SIZE;
            u8Length = (u16HeaderSize + OTA_TAG_HEADER_SIZE);
        }
    }

    for(i=0; i < u32TotalImageBlocks; i++)
    {
        memcpy(uBuf.au8, (au8Buffer+(i*AESSW_BLK_SIZE)), AESSW_BLK_SIZE);
        AESSW_vMMOBlockUpdate(puHash, &uBuf);
        for(j=0; j<AESSW_BLK_SIZE; j++)
            DBG_vPrintf(TRACE_OTA_DEBUG, "%02x", uBuf.au8[j]);
        DBG_vPrintf(TRACE_OTA_DEBUG, "\n");
        u32ImageSize -= AESSW_BLK_SIZE ;
    }

    iBytes = u8Length % AESSW_BLK_SIZE;
    u8BlockElements = AESSW_BLK_SIZE - iBytes;
    memcpy(uBuf.au8, (au8Buffer+(i*AESSW_BLK_SIZE)), iBytes);

    /* Read Remaining bytes from flash */
    sNvmDefsStruct.sOtaFnTable.prReadCb(u32FlashOffset, u8BlockElements, (uBuf.au8+iBytes));
    AESSW_vMMOBlockUpdate(puHash, &uBuf);
    for(j=0; j<AESSW_BLK_SIZE; j++)
        DBG_vPrintf(TRACE_OTA_DEBUG, "%02x", uBuf.au8[j]);
    DBG_vPrintf(TRACE_OTA_DEBUG, "\n");
    u32FlashOffset += u8BlockElements;
    u32ImageSize -= AESSW_BLK_SIZE ;
    u32TotalImageBlocks = u32ImageSize/AESSW_BLK_SIZE;

    for(i=0; i < u32TotalImageBlocks; i++)
    {
        sNvmDefsStruct.sOtaFnTable.prReadCb(u32FlashOffset,AESSW_BLK_SIZE, uBuf.au8);
        AESSW_vMMOBlockUpdate(puHash, &uBuf);
        u32FlashOffset += AESSW_BLK_SIZE;
    }

    iBytes = u32ImageSize % AESSW_BLK_SIZE;
    sNvmDefsStruct.sOtaFnTable.prReadCb(u32FlashOffset,iBytes, uBuf.au8);
    vReverseMemcpy((uint8*)&u32ImageSize,&au8Buffer[52],sizeof(uint32));
    AESSW_vMMOFinalUpdate(puHash, uBuf.au8, (u32ImageSize - OTA_SIGNITURE_SIZE), iBytes);
    for(j=0; j<AESSW_BLK_SIZE; j++)
        DBG_vPrintf(TRACE_OTA_DEBUG, "%02x", uBuf.au8[j]);
    DBG_vPrintf(TRACE_OTA_DEBUG, "\n");

    psCustomData->sOTACallBackMessage.eEventId = E_CLD_OTA_INTERNAL_COMMAND_FREE_FLASH_MUTEX;
    psEndPointDefinition->pCallBackFunctions(&psCustomData->sOTACustomCallBackEvent);
}

/****************************************************************************
 **
 ** NAME:       vOtaFlashLockRead
 **
 ** DESCRIPTION:
 ** Reads Flash data
 ** PARAMETERS:
 ** RETURN:
 ** None
 ****************************************************************************/
PUBLIC  void vOtaFlashLockRead(
                               tsZCL_EndPointDefinition *psEndPointDefinition,
                               tsOTA_Common             *psCustomData,
                               uint32                    u32FlashByteLocation,
                               uint16                    u16Len,
                               uint8                    *pu8Data)
{
    psCustomData->sOTACallBackMessage.eEventId = E_CLD_OTA_INTERNAL_COMMAND_LOCK_FLASH_MUTEX;
    psEndPointDefinition->pCallBackFunctions(&psCustomData->sOTACustomCallBackEvent);
    //DBG_vPrintf(TRUE, "FLASH Read %d bytes at %08x\n", u16Len, u32FlashByteLocation);
    sNvmDefsStruct.sOtaFnTable.prReadCb(u32FlashByteLocation,u16Len, pu8Data );
    psCustomData->sOTACallBackMessage.eEventId = E_CLD_OTA_INTERNAL_COMMAND_FREE_FLASH_MUTEX;
    psEndPointDefinition->pCallBackFunctions(&psCustomData->sOTACustomCallBackEvent);
}

/****************************************************************************
 **
 ** NAME:       vOtaFlashLockWrite
 **
 ** DESCRIPTION:
 ** Write data in flash
 ** PARAMETERS:
 ** RETURN:
 ** None
 ****************************************************************************/
PUBLIC  void vOtaFlashLockWrite(
                                  tsZCL_EndPointDefinition *psEndPointDefinition,
                                  tsOTA_Common             *psCustomData,
                                  uint32                    u32FlashByteLocation,
                                  uint16                    u16Len,
                                  uint8                    *pu8Data)
{

    psCustomData->sOTACallBackMessage.eEventId = E_CLD_OTA_INTERNAL_COMMAND_LOCK_FLASH_MUTEX;
    psEndPointDefinition->pCallBackFunctions(&psCustomData->sOTACustomCallBackEvent);
   //DBG_vPrintf(TRUE, "FLASH Write %d bytes at %08x \n", u16Len, u32FlashByteLocation);
    sNvmDefsStruct.sOtaFnTable.prWriteCb(u32FlashByteLocation, u16Len, pu8Data);
    psCustomData->sOTACallBackMessage.eEventId = E_CLD_OTA_INTERNAL_COMMAND_FREE_FLASH_MUTEX;
    psEndPointDefinition->pCallBackFunctions(&psCustomData->sOTACustomCallBackEvent);
}

/****************************************************************************
 **
 ** NAME:       vOtaFlashLockErase
 **
 ** DESCRIPTION:
 ** Erase the flash
 ** PARAMETERS:
 ** RETURN:
 ** None
 ****************************************************************************/
PUBLIC  void vOtaFlashLockErase(
                                    tsZCL_EndPointDefinition *psEndPointDefinition,
                                    tsOTA_Common             *psCustomData,
                                    uint8                     u8Sector)
{
    psCustomData->sOTACallBackMessage.eEventId = E_CLD_OTA_INTERNAL_COMMAND_LOCK_FLASH_MUTEX;
    psEndPointDefinition->pCallBackFunctions(&psCustomData->sOTACustomCallBackEvent);
    sNvmDefsStruct.sOtaFnTable.prEraseCb(u8Sector);
    psCustomData->sOTACallBackMessage.eEventId = E_CLD_OTA_INTERNAL_COMMAND_FREE_FLASH_MUTEX;
    psEndPointDefinition->pCallBackFunctions(&psCustomData->sOTACustomCallBackEvent);
}

/****************************************************************************
 **
 ** NAME:       bOtaIsSerializationDataValid
 **
 ** DESCRIPTION:
 ** validates serialiazation data after copying
 ** PARAMETERS:
 ** RETURN:
 ** bool
 ****************************************************************************/
PUBLIC  bool_t bOtaIsSerializationDataValid(
                                  tsZCL_EndPointDefinition *psEndPointDefinition,
                                  tsOTA_Common             *psCustomData,
                                  uint8                     u8UpgradeImageIndex)
{
    uint8 au8ReadBuffer1[50] = {0}; /* Temp Buffer reading */
    uint8 au8ReadBuffer2[50] = {0}; /* Temp Buffer reading */


    uint8 *pu8Start = (uint8*)&_flash_start;
    uint8 *pu8LinkKey =    (uint8*)&_FlsLinkKey;
#ifndef OTA_NO_CERTIFICATE
    uint8 *pu8Cert =       (uint8*)&FlsZcCert;
    uint8 *pu8PrvKey =     (uint8*)&FlsPrivateKey;
#endif
#ifdef OTA_COPY_MAC_ADDRESS
    uint8 *pu8MacAddress = (uint8*)&_FlsMACAddress;
#endif

#ifdef OTA_MAINTAIN_CUSTOM_SERIALISATION_DATA
    uint8 *pu8CustData =   (uint8*)&FlsCustomDatabeg;
    uint8 *pu8CustDataEnd = (uint8*)&FlsCustomDataEnd;
    uint8 u8SizeOfCustomData = pu8CustDataEnd - pu8CustData;
#endif

#ifndef OTA_UNIT_TEST_FRAMEWORK

#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
    uint8 au8ivector[OTA_AES_BLOCK_SIZE], au8DataOut[OTA_AES_BLOCK_SIZE],u8Loop;
     tsReg128 sOtaUseKey =  eOTA_retOtaUseKey();
    uint32 u32OtaOffset = eOTA_OtaOffset();
#endif
    uint32 u32Offset;
#if (defined OTA_INTERNAL_STORAGE) || (defined KSDK2)
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
    bool_t bEncExternalFlash = FALSE;
#endif
#else
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
    uint32 u32CustomerSettings = *((uint32*)FL_INDEX_SECTOR_CUSTOMER_SETTINGS);
    bool_t bEncExternalFlash = (u32CustomerSettings & FL_INDEX_SECTOR_ENC_EXT_FLASH)?FALSE:TRUE;
#endif
#endif


#ifdef OTA_COPY_MAC_ADDRESS
    /* Read MAC Address From Internal Flash */
    memcpy(au8ReadBuffer1, pu8MacAddress, 8);

    if(bEncExternalFlash)
    {
        /* Encrypt MAC address and read MAC address from upgrade image then compare */
        /* Read IV vector from external flash */
        u32Offset = (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8UpgradeImageIndex] * sNvmDefsStruct.u32SectorSize);
        u32Offset += OTA_IV_LOCATION;
        vOtaFlashLockRead(psEndPointDefinition, psCustomData, u32Offset,OTA_AES_BLOCK_SIZE, au8ivector);

        au8ivector[15] = au8ivector[15]+((OTA_6X_MAC_OFFSET - OTA_ENC_OFFSET)/OTA_AES_BLOCK_SIZE);

        //bACI_ECBencodeStripe(&sOtaUseKey,TRUE,(tsReg128 *)au8ivector,(tsReg128 *)au8DataOut);
        vOTA_EncodeString(&sOtaUseKey, au8ivector,au8DataOut);

        for(u8Loop=0;u8Loop<16;u8Loop++)
            au8ReadBuffer1[u8Loop] = (au8ReadBuffer1[u8Loop] ^ au8DataOut[u8Loop]) ;
    }
    u32Offset = OTA_6X_MAC_OFFSET;

    u32Offset += (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8UpgradeImageIndex] * sNvmDefsStruct.u32SectorSize);
    vOtaFlashLockRead(psEndPointDefinition, psCustomData,u32Offset,16, au8ReadBuffer2);

    /* Comapre MAC Address */
    if(memcmp(au8ReadBuffer1, au8ReadBuffer2, 8) != 0)
        return FALSE;


#endif

    /* Link Keys Comparison */
    memcpy(au8ReadBuffer1, pu8LinkKey, 16);
#if !(defined OTA_INTERNAL_STORAGE) && !(defined KSDK2)
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
    if(bEncExternalFlash)
    {
        u32Offset = (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8UpgradeImageIndex] * sNvmDefsStruct.u32SectorSize);
        u32Offset += OTA_IV_LOCATION;
        vOtaFlashLockRead(psEndPointDefinition, psCustomData, u32Offset,OTA_AES_BLOCK_SIZE, au8ivector);

        au8ivector[15] = au8ivector[15]+((u32OtaOffset - OTA_ENC_OFFSET)/OTA_AES_BLOCK_SIZE) + (80 / OTA_AES_BLOCK_SIZE);
        //bACI_ECBencodeStripe(&sOtaUseKey,TRUE,(tsReg128 *)au8ivector,(tsReg128 *)au8DataOut);
        vOTA_EncodeString(&sOtaUseKey, au8ivector,au8DataOut);

        for(u8Loop=0;u8Loop<16;u8Loop++)
            au8ReadBuffer1[u8Loop] = (au8ReadBuffer1[u8Loop] ^ au8DataOut[u8Loop]);
    }
#endif
#endif
    u32Offset = pu8LinkKey - pu8Start;

#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x) || (defined APP0)
#ifdef APP0 /* Building with selective OTA */
    if(psCustomData->sOTACallBackMessage.sPersistedData.bStackDownloadActive)
    {
        u32Offset += OTA_APP1_SHADOW_FLASH_OFFSET;
    }
    else
#endif
    {
        u32Offset +=  ( (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8UpgradeImageIndex] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION );
    }
#endif
    vOtaFlashLockRead(psEndPointDefinition, psCustomData,u32Offset,16, au8ReadBuffer2);

    /* Compare Link key */
    if(memcmp(au8ReadBuffer1, au8ReadBuffer2, 16) != 0)
        return FALSE;

#ifndef OTA_NO_CERTIFICATE
    /* Certificates Comparison */
    memcpy(au8ReadBuffer1, pu8Cert, 48);

    if(bEncExternalFlash)
    {
        for(u8Loop=0;u8Loop<48;u8Loop++)
        {
            if((u8Loop % OTA_AES_BLOCK_SIZE) == 0 )
            {
                au8ivector[15]++;
                bACI_ECBencodeStripe(&sOtaUseKey,TRUE,(tsReg128 *)au8ivector,(tsReg128 *)au8DataOut);
            }
            au8ReadBuffer1[u8Loop] = au8ReadBuffer1[u8Loop] ^ au8DataOut[(u8Loop %16)] ;
        }
    }

    u32Offset = pu8Cert - pu8Start;
    u32Offset += ( ( psCustomData->sOTACallBackMessage.u8ImageStartSector[u8UpgradeImageIndex] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION );
    vOtaFlashLockRead(psEndPointDefinition, psCustomData,u32Offset,48, au8ReadBuffer2);

    /* Compare Certificates */
    if(memcmp(au8ReadBuffer1, au8ReadBuffer2, 48) != 0)
        return FALSE;

    /* Private key Comparison */
    memcpy(au8ReadBuffer1, pu8PrvKey, 21);

    if(bEncExternalFlash)
    {
        for(u8Loop=0;u8Loop<21;u8Loop++)
        {
            if((u8Loop % OTA_AES_BLOCK_SIZE) == 0 )
            {
                au8ivector[15]++;
                bACI_ECBencodeStripe(&sOtaUseKey,TRUE,(tsReg128 *)au8ivector,(tsReg128 *)au8DataOut);
            }

            au8ReadBuffer1[u8Loop] = (au8ReadBuffer1[u8Loop] ^ au8DataOut[(u8Loop %16)]) ;
        }
    }

    u32Offset = pu8PrvKey - pu8Start;

    u32Offset +=  ( ( psCustomData->sOTACallBackMessage.u8ImageStartSector[u8UpgradeImageIndex] * sNvmDefsStruct.u32SectorSize ) * OTA_SECTOR_CONVERTION );

    vOtaFlashLockRead(psEndPointDefinition, psCustomData,u32Offset,21, au8ReadBuffer2);

    /* Compare Private Keys */
    if(memcmp(au8ReadBuffer1, au8ReadBuffer2, 21) != 0)
        return FALSE;
#endif
#ifdef OTA_MAINTAIN_CUSTOM_SERIALISATION_DATA
    if(u8SizeOfCustomData > 0)
    {
        uint8 u8NoOf16BytesBlocks = u8SizeOfCustomData/16;

        uint8 u8LoopCount,u8IntLoopCount;
        uint32 u32ReadOffset = pu8CustData - pu8Start;

        u32Offset = ( ( psCustomData->sOTACallBackMessage.u8ImageStartSector[u8UpgradeImageIndex] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION );

        u32Offset += OTA_IV_LOCATION;
        vOtaFlashLockRead(psEndPointDefinition, psCustomData, u32Offset,OTA_AES_BLOCK_SIZE, au8ivector);

        au8ivector[15] = au8ivector[15]+((u32OtaOffset - OTA_ENC_OFFSET)/OTA_AES_BLOCK_SIZE) + (80 / OTA_AES_BLOCK_SIZE) + ( (pu8CustData - pu8LinkKey) / OTA_AES_BLOCK_SIZE);

        bACI_ECBencodeStripe(&sOtaUseKey,TRUE,(tsReg128 *)au8ivector,(tsReg128 *)au8DataOut);

        for(u8LoopCount = 0; u8LoopCount < u8NoOf16BytesBlocks; u8LoopCount++)
        {
            memcpy(au8ReadBuffer1, (pu8CustData+(16*u8LoopCount)), 16);

            if(bEncExternalFlash)
            {
                for(u8IntLoopCount=0; u8IntLoopCount < 16;u8IntLoopCount++)
                {
                    au8ReadBuffer1[u8IntLoopCount] = (au8ReadBuffer1[u8IntLoopCount] ^ au8DataOut[(u8IntLoopCount)]);
                }
            }

            vOtaFlashLockRead(psEndPointDefinition, psCustomData, (u32ReadOffset + (16*u8LoopCount)), 16, au8ReadBuffer2);

            /* Compare Custom Data */
            if(memcmp(au8ReadBuffer1, au8ReadBuffer2, 16) != 0)
                return FALSE;

            au8ivector[15]++;
            bACI_ECBencodeStripe(&sOtaUseKey,TRUE,(tsReg128 *)au8ivector,(tsReg128 *)au8DataOut);
        }

        /* read remaining data */
        u8NoOf16BytesBlocks = u8SizeOfCustomData%16;
        memcpy(au8ReadBuffer1, (pu8CustData+(16*u8LoopCount)), u8NoOf16BytesBlocks);

        if(bEncExternalFlash)
        {
            for(u8LoopCount = 0; u8LoopCount < u8NoOf16BytesBlocks; u8LoopCount++)
            {
                au8ReadBuffer1[u8LoopCount] = (au8ReadBuffer1[u8LoopCount] ^ au8DataOut[(u8LoopCount)]);
            }
        }

        vOtaFlashLockRead(psEndPointDefinition, psCustomData, (u32ReadOffset + (16*u8LoopCount)), u8NoOf16BytesBlocks, au8ReadBuffer2);

        /* Compare Custom data */
        if(memcmp(au8ReadBuffer1, au8ReadBuffer2, u8NoOf16BytesBlocks) != 0)
            return FALSE;
    }
#endif
#endif
    return TRUE;
}

#ifdef OTA_CLIENT
/****************************************************************************
 **
 ** NAME:       eOtaClientReqSeqNoUpdate
 **
 ** DESCRIPTION:
 ** Update ZCL transaction number sent in request sequence
 ** PARAMETERS:
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
PUBLIC  teZCL_Status eOtaClientReqSeqNoUpdate(
                                            uint8 u8SourceEndPointId,
                                            uint8 u8RequestTransSeqNo)
{
    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;

    if((eStatus = eOtaFindCluster(u8SourceEndPointId,
                     &psEndPointDefinition,
                       &psClusterInstance,
                       &psCustomData,
                       FALSE))
                       == E_ZCL_SUCCESS)
    {
        psCustomData->sOTACallBackMessage.sPersistedData.u8RequestTransSeqNo = u8RequestTransSeqNo;
    }

    return eStatus;
}
#endif/* OTA_CLIENT */

/****************************************************************************
 **
 ** NAME:       eOTA_GetCurrentOtaHeader
 **
 ** DESCRIPTION:
 ** Get current image OTA header
 ** PARAMETERS:
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC  teZCL_Status eOTA_GetCurrentOtaHeader(
                                                uint8              u8Endpoint,
                                                bool_t             bIsServer,
                                                tsOTA_ImageHeader *psOTAHeader)
{
    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;

    /* validate parameters */
    if(psOTAHeader == NULL)
    {
        return E_ZCL_ERR_PARAMETER_NULL;
    }

    if((eStatus = eOtaFindCluster(u8Endpoint,
                         &psEndPointDefinition,
                         &psClusterInstance,
                         &psCustomData,
                         bIsServer))
                         == E_ZCL_SUCCESS)
    {
        #ifdef APP0 /* Building with selective OTA */
             if(psCustomData->sOTACallBackMessage.sPersistedData.bStackDownloadActive)
             {
                 *psOTAHeader = sOtaGetHeader(APP_pu8GetApp1OtaHeader());
             }
             else
        #endif
        #ifdef SOTA_ENABLED
            if (!sOtaInitBlobOtaHeader(psCustomData->sOTACallBackMessage.sPersistedData.blobId, psOTAHeader))
        #endif
             {
                 *psOTAHeader = sOtaGetHeader(au8OtaHeader);
             }

    }
    return eStatus;
}

#ifdef OTA_SERVER
/****************************************************************************
 **
 ** NAME:       eOTA_GetOtaHeader
 **
 ** DESCRIPTION:
 ** provide OTA header of specified image
 ** PARAMETERS:
 ** RETURN:
 ** teZCL_Status
 ****************************************************************************/
PUBLIC  teZCL_Status eOTA_GetOtaHeader(
                                                uint8              u8Endpoint,
                                                uint8              u8ImageIndex,
                                                tsOTA_ImageHeader *psOTAHeader)
{
    teZCL_Status eStatus = E_ZCL_SUCCESS;
    tsZCL_ClusterInstance *psClusterInstance;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsOTA_Common *psCustomData;
    uint8 aHeader[OTA_MAX_HEADER_SIZE];
    uint16 u16HeaderLength;
    uint32 u32HeaderType, u32FlashOffset = 0;
    /* validate parameters */
    if(psOTAHeader == NULL)
    {
        return E_ZCL_ERR_PARAMETER_NULL;
    }

    if((eStatus = eOtaFindCluster(u8Endpoint,
                         &psEndPointDefinition,
                         &psClusterInstance,
                         &psCustomData,
                         TRUE))
                         == E_ZCL_SUCCESS)
    {

       if(u8ImageIndex < OTA_MAX_IMAGES_PER_ENDPOINT)
       {

#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x) || (defined APP0) || (defined OTA_SERVER)
#ifdef APP0 /* Building with selective OTA */
            if(psOTA_Common->sOTACallBackMessage.sPersistedData.bStackDownloadActive)
            {
                u32FlashOffset = OTA_APP1_SHADOW_FLASH_OFFSET;
            }
            else
#endif
            {
                u32FlashOffset =  ( (psCustomData->sOTACallBackMessage.u8ImageStartSector[u8ImageIndex] * sNvmDefsStruct.u32SectorSize) * OTA_SECTOR_CONVERTION );
            }
#endif
           /* Get the image header installed and check against client requirement
             first read the initial 8 bytes to see how big the header is */
           vOtaFlashLockRead(psEndPointDefinition, psCustomData,u32FlashOffset,OTA_MAX_HEADER_SIZE, aHeader);
           vReverseMemcpy((uint8*)&u16HeaderLength,&aHeader[6],sizeof(uint16));
           vReverseMemcpy((uint8*)&u32HeaderType,&aHeader[0],sizeof(uint32));
           if(u32HeaderType == OTA_FILE_IDENTIFIER &&
               u16HeaderLength <= OTA_MAX_HEADER_SIZE )
           {
               DBG_vPrintf(TRACE_OTA_DEBUG, "OTA Client Image Found File Identifer Matched2\n");
               *psOTAHeader = sOtaGetHeader(aHeader);
           }
           else
           {
               return E_ZCL_FAIL;
           }
       }
       else
       {
           *psOTAHeader = sOTAHeader[u8ImageIndex - OTA_MAX_IMAGES_PER_ENDPOINT];
       }
    }
    return eStatus;
}
#endif


PUBLIC void vOTA_GetTagIdandLengh(uint16 *pu16TagId,uint32 *pu32TagLength,uint8 * pu8Tag)
{
    vReverseMemcpy((uint8*)pu16TagId,&pu8Tag[0],sizeof(uint16));
    vReverseMemcpy((uint8*)pu32TagLength,&pu8Tag[2],sizeof(uint32));
}

PUBLIC void vOTA_EncodeString(tsReg128 *psKey, uint8 *pu8Iv,uint8 * pu8DataOut)
{
    tsReg128 sIv,sDataOut;
    #if (JENNIC_CHIP_FAMILY == JN517x) && (defined LITTLE_ENDIAN_PROCESSOR)
        AESSW_Block_u uTempData;
        memcpy(&uTempData,pu8Iv,sizeof(AESSW_Block_u));
        vSwipeEndian(&uTempData,&sIv,TRUE);
    #else
        memcpy(&sIv,pu8Iv,0x10);
    #endif

    memcpy(&sDataOut,pu8DataOut,0x10);

#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
    bACI_ECBencodeStripe(psKey,
                         TRUE,
                         &sIv,
                         &sDataOut);
#else
    AES_SetKey(AES0, (uint8*)psKey, AESSW_BLK_SIZE);
    AES_EncryptEcb(AES0, (uint8*)&sIv, (uint8*)&sDataOut, AESSW_BLK_SIZE);
#endif

    #if (JENNIC_CHIP_FAMILY == JN517x) && (defined LITTLE_ENDIAN_PROCESSOR)
        vSwipeEndian(&uTempData,&sDataOut,FALSE);
        memcpy(pu8DataOut,&uTempData,sizeof(AESSW_Block_u));

    #else
        memcpy(pu8DataOut,&sDataOut,0x10);
    #endif    
}
#endif /* CLD_OTA */
/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
