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
 * MODULE:             Over the Air upgrade
 *
 * COMPONENT:          ota_private.h
 *
 * DESCRIPTION:        Header for Over the air upgrade cluster
 *
 *****************************************************************************/

#ifndef OTA_H_PRIVATE
#define OTA_H_PRIVATE

#if defined __cplusplus
extern "C" {
#endif

#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"
#include "OTA.h"
#if !(defined JENNIC_CHIP_FAMILY_JN516x) && !(defined JENNIC_CHIP_FAMILY_JN517x) && !(defined APP0)
#include "OtaSupport.h"
#endif
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/


#ifdef OTA_PC_BUILD
#define OTA_FLS_VALIDITY_OFFSET 105

#ifdef OTA_UNIT_TEST_FRAMEWORK
extern tsOTA_ImageHeader _FlsOtaHeader;
extern uint8 FlsPrivateKey[];
extern uint8 FlsZcCert[];
extern uint8 _FlsLinkKey[];
extern uint16 u16ImageStartSector;
extern uint32   _flash_start;
#else
#define FlsStart 0
#define _FlsOtaHeader 0
#define FlsPrivateKey 0
#define FlsZcCert 0
#define _FlsLinkKey 0
#define u16ImageStartSector 0
#endif

#else
#define OTA_FLS_VALIDITY_OFFSET 0
#endif

#ifdef OTA_UNIT_TEST_FRAMEWORk
extern uint32 u32Registers;
#define OTA_EFUSE_BANK1_31_0        (&u32Registers)
#define OTA_EFUSE_BANK1_63_32       (&u32Registers)
#define OTA_EFUSE_BANK1_95_64       (&u32Registers)
#define OTA_EFUSE_BANK1_127_96      (&u32Registers)

#else
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (JENNIC_CHIP_FAMILY_JN517x)
#define OTA_EFUSE_BANK1_31_0        (0x02000000 + 0x0070)
#define OTA_EFUSE_BANK1_63_32       (0x02000000 + 0x0074)
#define OTA_EFUSE_BANK1_95_64       (0x02000000 + 0x0078)
#define OTA_EFUSE_BANK1_127_96      (0x02000000 + 0x007C)
#endif
#endif

#define OTA_IS_ZCL_UNICAST(ZCL_ADDRESS)   ( (((ZCL_ADDRESS).eAddressMode == ZPS_E_ADDR_MODE_SHORT)&&((ADDRESS).u16DestinationAddress < 0xFFF8)\
    && ((ZCL_ADDRESS).eAddressMode != ZPS_E_ADDR_MODE_GROUP)) ? TRUE : FALSE)

#define OTA_IS_UNICAST(ADDRESS_MODE,ADDRESS)   ( (((ADDRESS_MODE) == E_ZCL_AM_SHORT)&&((ADDRESS) < 0xFFF8)\
    && ((ADDRESS_MODE) != E_ZCL_AM_GROUP)) ? TRUE : FALSE)
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef void(*pFuncptr)(void *, tsZCL_EndPointDefinition    *);

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC teZCL_Status eOtaCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);
PUBLIC teZCL_Status eOtaRegisterTimeServer(void);
PUBLIC void vOtaTimerClickCallback(
                    tsZCL_CallBackEvent         *psCallBackEvent);
PUBLIC teZCL_Status eOtaTimeUpdate(
                    uint8                        u8SourceEndPointId,
                    bool_t                       bIsServer,
                    tsZCL_CallBackEvent         *psCallBackEvent);
PUBLIC void vOtaFlashInitHw (
                    uint8                        u8FlashType,
                    void                        *pvFlashTable);
PUBLIC void vOtaFlashErase(
                    uint8                        u8Sector);
PUBLIC void vOtaFlashWrite(
                    uint32                       u32FlashByteLocation,
                    uint16                       u16Len,
                    uint8                       *pu8Data);
PUBLIC void vOtaFlashRead(
                    uint32                       u32FlashByteLocation,
                    uint16                       u16Len,
                    uint8                       *pu8Data);
PUBLIC teZCL_Status eOtaFindCluster(
                    uint8                        u8SourceEndPointId,
                    tsZCL_EndPointDefinition   **ppsEndPointDefinition,
                    tsZCL_ClusterInstance      **ppsClusterInstance,
                    tsOTA_Common               **ppsOTACustomDataStructure,
                    bool_t                       bIsServer);
PUBLIC  teZCL_Status eOtaSetEventTypeAndGiveCallBack(
                       tsOTA_Common                     *psOTA_Common,
                       teOTA_UpgradeClusterEvents        eEventType,
                       tsZCL_EndPointDefinition         *psEndPointDefinition);
PUBLIC tsOTA_ImageHeader sOtaGetHeader(
                    uint8                       *pu8HeaderBytes);

#ifdef SOTA_ENABLED
PUBLIC bool_t sOtaInitBlobOtaHeader(
                    uint16                       u16BlobId,
                    tsOTA_ImageHeader            *pBlobOtaHeader);
#endif

PUBLIC void vOtaUpgManServerCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    void                        *pvStruct,
                    tsZCL_EndPointDefinition    *psEndPointDefinition);
PUBLIC void vOtaFlashLockRead(
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsOTA_Common                *psCustomData,
                    uint32                       u32FlashByteLocation,
                    uint16                       u16Len,
                    uint8                       *pu8Data);
PUBLIC void vOtaFlashLockWrite(
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsOTA_Common                *psCustomData,
                    uint32                       u32FlashByteLocation,
                    uint16                       u16Len,
                    uint8                       *pu8Data);
PUBLIC  void vOtaFlashLockErase(
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsOTA_Common                *psCustomData,
                    uint8                        u8Sector);

PUBLIC bool_t bOtaIsImageValid( tsOTA_Common*    psOTA_Common,
                              uint8*           pu8Data );
PUBLIC void vOtaSwitchLoads(void);
PUBLIC uint32 eOTA_OtaOffset(void);
PUBLIC tsReg128 eOTA_retOtaUseKey(void);

#ifdef OTA_CLIENT
PUBLIC void vOtaInitStateMachine(
                    tsOTA_Common                *psCustomData);
PUBLIC void vOtaClientUpgMgrMapStates(
                    teOTA_ImageUpgradeStatus     eStatus,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsOTA_Common                *psCustomData);
PUBLIC teZCL_Status eOtaReceivedImageNotify(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint16                       u16Offset);
PUBLIC teZCL_Status eOtaReceivedQueryNextImageResponse(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint16                       u16Offset);
PUBLIC teZCL_Status eOtaReceivedBlockResponse(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint16 u16Offset);
PUBLIC teZCL_Status eOtaReceivedUpgradeEndResponse(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint16                       u16Offset);
PUBLIC teZCL_Status eOtaQuerySpecificFileResponse(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint16                       u16Offset);
PUBLIC teZCL_Status eOtaClientReqSeqNoUpdate(
                    uint8                       u8SourceEndPointId,
                    uint8                       u8RequestTransSeqNo);

PUBLIC void vOtaReceivedDefaultResponse(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);


#endif
#ifdef OTA_SERVER
PUBLIC void vOtaServerUpgManagerInit(pFunc *pFuncPtr);
PUBLIC teZCL_Status eOtaReceivedQueryNextImage(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint16                       u16Offset);
PUBLIC teZCL_Status eOtaReceivedBlockRequest(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint16                       u16Offset);

PUBLIC teZCL_Status eOtaReceivedUpgradeEndRequest(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint16                       u16Offset);

PUBLIC teZCL_Status eOtaReceivedUpgradeEndRequest(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint16                       u16Offset);
PUBLIC teZCL_Status eOtaReceivedQuerySpecificFileRequest(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint16                       u16Offset);
PUBLIC teZCL_Status eOtaReceivedPageRequest(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance,
                    uint16                       u16Offset);
#ifdef OTA_PAGE_REQUEST_SUPPORT
PUBLIC void vOtaHandleTimedPageRequest(uint8 u8SourceEndPointId);
#endif
PUBLIC teZCL_Status eOTA_GetOtaHeader(
                    uint8                        u8Endpoint,
                    uint8                        u8ImageIndex,
                    tsOTA_ImageHeader           *psOTAHeader);
#endif
PUBLIC void vOTA_GetTagIdandLengh(uint16 *pu16TagId,uint32 *pu32TagLength,uint8 * pu8Tag);
/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/




#ifdef OTA_CLIENT
extern const tsZCL_AttributeDefinition asOTAClusterAttributeDefinitions[];
extern uint8 au8OtaHeader[OTA_MAX_HEADER_SIZE] ;
#if (OTA_MAX_CO_PROCESSOR_IMAGES != 0)
extern uint8 asCommonCoProcessorOTAHeader[OTA_MAX_CO_PROCESSOR_IMAGES][OTA_MAX_HEADER_SIZE] ;
extern bool_t bIsCoProcessorImgUpgdDependent;
#endif
#endif


/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

#endif /* OTA_H_PRIVATE */
