/****************************************************************************
 *
 * Copyright 2020-2021 NXP.
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
 * MODULE:          Zigbee Protocol Stack MAC shim layer
 *
 * COMPONENT:       mac_vs_sap.h
 *
 * DESCRIPTION:     MAC shim layer to provide PDU manager i/f to NWK layer
 *
 ****************************************************************************/

/**
 * @defgroup g_mac_vs_data MAC layer Service Access Point (SAP)
 */

#ifndef _mac_vs_data_h_
#define _mac_vs_data_h_

#ifdef __cplusplus
extern "C" {
#endif

/***********************/
/**** INCLUDE FILES ****/
/***********************/

#include "jendefs.h"
#include <pdum_nwk.h>
#include "mac_sap.h"
#include "mac_pib.h"
#include "AppApi.h"
#include "tsv_pub.h"
#ifdef ZPS_SHIM_TYPE_SERIAL
#include "SMAC_Common.h"
#endif

#include "zps_mac_if_table.h"
#include "tsv_pub.h"
/************************/
/**** MACROS/DEFINES ****/
/************************/

#define MAC_MCPS_VS_TYPE_OFFSET 				0x80

#if 1 /*ZPS_SHIM_TYPE_SERIAL*/
#define MAX_SCAN_CHANNEL_COUNT					27
#else
#define MAX_SCAN_CHANNEL_COUNT					MAC_MAX_SCAN_CHANNELS
#endif



#define MAC_MAX_ZBP_BEACON_PAYLOAD_LEN 			15
#define MAC_MAX_ENHANCED_BEACON_BYTES			40

#ifdef MAX_TIMERS
#undef MAX_TIMERS
#endif

#define MAX_TIMERS								8		/* Defined as enum private to MiniMAC in mini_internal.h Need to make
														the same in the Serial MAC Shim or we generate link warning. */




#define TIMER_INTERVAL 							64
#define u32SymbolsToTicks(A)   					(((A) + TIMER_INTERVAL - 1) / TIMER_INTERVAL)




#define MAC_PAGE_FROM_MASK(X)					((uint8)(X >> 27))
#define MAC_CHANNELMASK_FROM_MASK(X)			((uint32)(X & 0x07FFFFFF))


#define MAC_CHAN_MASK_PAGE_0                    (0u)
#define MAC_CHAN_MASK_PAGE_28                   (28u)
#define MAC_CHAN_MASK_PAGE_29                   (29u)
#define MAC_CHAN_MASK_PAGE_30                   (30u)
#define MAC_CHAN_MASK_PAGE_31                   (31u)

#define MAC_PAGE_MASK							(0x1F << 27)
#define MAC_PAGE_MASK_0 						(0	<< 27)
#define MAC_PAGE_MASK_28 						(28 << 27)
#define MAC_PAGE_MASK_29						(29 << 27)
#define MAC_PAGE_MASK_30 						(30 << 27)
#define MAC_PAGE_MASK_31 						(31 << 27)


#define MAC_ID_DCFMIND(X) 						(((MAC_DcfmIndHdr_s*) X )->u16MacID)
#define MAC_ID_REQRSP(X) 						(((MAC_ReqRspHdr_s*) X )->u16MacID)


#define EXCEPTION_CODE_NONE						0x0000000
#define EXCEPTION_CODE_BAD_HANDLE				0x0000001
#define EXCEPTION_CODE_UNINITIALSED_MAC			0x0008009
#define EXCEPTION_CODE_SERIALMAC_NO_SYNC		0x000000A

#ifndef BIT
#define BIT(X)		(1 << (X))
#endif

#define MIN_TX_BUFFERS (5)

#define ZPS_NWK_INTERPAN_HANDLE     0xF5
#define ZPS_NWK_GP_BASE_HANDLE      0xE0
#define ZPS_NWK_GP_MAX_HANDLE       0xF0

/**********************/
/**** MCPS Request ****/
/**********************/

/**
 * @defgroup g_mac_sap_mcps_req MCPS Request objects
 * @ingroup g_mac_sap_mcps
 *
 * These are passed in a call to MAC_vHandleMcpsReqRsp.
 * Confirms to a Request will either be passed back synchronously on the function return,
 * or a special 'deferred' confirm will come back asynchronously via the
 * Deferred Confirm/Indication callback.
 * Responses have no effective confirmation of sending as they are in response to
 * an Indication; this will be indicated in the synchronous Confirm passed back.
 */

/**
 * @defgroup g_mac_sap_mcps_req_15_4 MCPS Request 802.15.4 specification parameters
 * @ingroup g_mac_sap_mcps_req
 *
 * @{
 */

/**
 * @brief Structure for MCPS-DATA.request
 *
 * Data transmit request. Use type MAC_MCPS_REQ_DATA
 */
typedef struct
{
    MAC_Addr_s  sSrcAddr;    /**< Source address */
    MAC_Addr_s  sDstAddr;    /**< Destination address */
    uint8       u8TxOptions; /**< Transmit options */
    uint8       u8Handle;    /**< MAC handle */
    uint8       u8Free;      /**< Whether NPDU is freed or not */
    PDUM_thNPdu hNPdu;       /**< NPDU handle */
} MAC_tsMcpsVsReqData;

/**
 * @brief Structure for MCPS-PURGE.request
 *
 * Purge request. Use type MAC_MCPS_REQ_PURGE
 */
typedef struct
{
    uint8 u8Handle;    /**< Handle of request to purge from queue */
} MAC_tsMcpsVsReqPurge;

/* @} */

/**********************/
/**** MCPS Confirm ****/
/**********************/

/**
 * @defgroup g_mac_sap_mcps_cfm MCPS Confirm objects
 * @ingroup g_mac_sap_mcps
 *
 * These come back synchronously as a returned parameter in the Request/Response call.
 * They can also be deferred and asynchronously posted via the Deferred Confirm/Indication callback.
 */

/**
 * @defgroup g_mac_sap_mcps_cfm_15_4 MCPS Confirm 802.15.4 specification parameters
 * @ingroup g_mac_sap_mcps_cfm
 *
 * @{
 */

/**
 * @brief Structure for MCPS-DATA.confirm
 *
 * Data transmit confirm. Use type MAC_MCPS_CFM_DATA
 */
typedef struct
{
    uint8 u8Status; /**< Status of request @sa MAC_Enum_e */
    uint8 u8Handle; /**< Handle matching associated request */
    uint16 u16MacId;/**< DFCM interface source */
    uint32 u32Timestamp; /**< SFD TX time    */
} MAC_tsMcpsVsCfmData;

/**
 * @brief Structure for MCPS-PURGE.confirm
 *
 * Data transmit confirm. Use type MAC_MCPS_CFM_PURGE
 */
typedef struct
{
    uint8 u8Status; /**< Status of request @sa MAC_Enum_e */
    uint8 u8Handle; /**< Handle matching associated request */
} MAC_tsMcpsVsCfmPurge;

/* @} */
typedef struct
{
    tsTxFrameFormat sTxFrame;
    TSV_Timer_s sTimer;
    uint8 u8HigherLayerRetryCount;
} tsHigherLayerTxFrame;

/*************************/
/**** MCPS Indication ****/
/*************************/
typedef int (*Appfnptr) (const char *pcFormat, ...);
/**
 * @defgroup g_mac_sap_mcps_ind MCPS Indication Object
 * @ingroup g_mac_sap_mcps
 *
 * These are sent asynchronously via the registered Deferred Confirm/Indication callback
 */

/**
 * @defgroup g_mac_sap_mcps_ind_15_4 MCPS Indication 802.15.4 specification parameters
 * @ingroup g_mac_sap_mcps_ind
 *
 * @{
 */

/**
 * @brief Structure for MCPS-DATA.indication
 *
 * Data received indication. Uses type MAC_MCPS_IND_DATA
 */
typedef struct
{
    MAC_Addr_s  sSrcAddr;       /**< Source address */
    MAC_Addr_s  sDstAddr;       /**< Destination address */
    uint8       u8LinkQuality;  /**< Link quality of received frame */
    uint8       u8SecurityUse;  /**< True if security was used */
    uint8       u8AclEntry;     /**< Security suite used */
    uint8       u8SeqNum;       /**< sequence number */
    uint32      u32ArrivalTime; /**< Arrival time    */
    PDUM_thNPdu hNPdu;          /**< NPDU handle */
    bool_t      bFramePending;  /*Frame pending set*/
} MAC_tsMcpsVsIndData;

/* @} */

/*****************************************/
/**** MCPS Request/Response Interface ****/
/*****************************************/

/**
 * @defgroup g_mac_sap_mcps_req_rsp_if MCPS Request/Response interface
 * @ingroup g_mac_sap_mcps g_mac_VS
 *
 * The interface for the client to issue an MCPS Request or Response
 * is via a function call to MAC_vHandleMcpsReqRsp.
 * @li Request/Response parameters are passed in via psMcpsReqRsp
 * @li Synchronous Confirm parameters are passed out via psMcpsSyncCfm
 * @li Deferred Confirms are posted back asynchronously via the
 *     Deferred Confirm/Indication callback.
 * @note There are currently no MCPS Responses specified
 *
 * @{
 */

/**
 * @brief MAC MCPS Request/Response enumeration.
 *
 * Enumeration of MAC MCPS Request/Response
 * @note Must not exceed 256 entries
 */
typedef enum
{
    MAC_MCPS_VS_REQ_DATA = MAC_MCPS_VS_TYPE_OFFSET,  /**< Use with MAC_tsMcpsVsReqRsp */
    MAC_MCPS_VS_REQ_PURGE,        /**< Use with MAC_tsMcpsVsReqPurge */
} MAC_teMcpsVsReqRspType;

#define NUM_MAC_MCPS_VS_REQ 1

/**
 * @brief MCPS Request/Response Parameter union
 *
 * Union of all the possible MCPS Requests and Responses
 * @note There are no Responses currently specified
 */
typedef union
{
    MAC_tsMcpsVsReqData  sVsReqData;   /**< Data request */
    MAC_tsMcpsVsReqPurge sVsReqPurge;  /**< Purge request */
} MAC_tuMcpsVsReqRspParam;

/**
 * @brief MCPS Request/Response object
 *
 * The object passed to MAC_vHandleMcpsReqRsp containing the request/response
 */
typedef struct
{
    uint8                   u8Type;          /**< Request type (@sa MAC_teMcpsReqRspVsType) */
    uint8                   u8ParamLength;   /**< Parameter length in following union */
    uint16                  u16Pad;          /**< Padding to force alignment */
    MAC_tuMcpsVsReqRspParam uParam;          /**< Union of all possible Requests */
} MAC_tsMcpsVsReqRsp;

/**
 * @brief MCPS Synchronouse Confirm Parameter union
 *
 * Union of all the possible MCPS Synchronous Confirms
 */
typedef union
{
    MAC_tsMcpsVsCfmData  sVsCfmData;
    MAC_tsMcpsVsCfmData  sVsCfmPurge;
} MAC_tuMcpsVsSyncCfmParam;

/**
 * @brief MCPS Synchronous Confirm
 *
 * The object returned by MAC_vHandleMcpsReqRsp containing the synchronous confirm.
 * The confirm type is implied as corresponding to the request
 * @note All Confirms may also be sent asynchronously via the registered Deferred Confirm/Indication callback;
 * this is notified by returning MAC_MCPS_CFM_DEFERRED.
 */
typedef struct
{
    uint8                    u8Status;        /**< Confirm status (@sa MAC_teMcpsSyncCfmVsStatus) */
    uint8                    u8ParamLength;   /**< Parameter length in following union */
    uint16                   u16Pad;          /**< Padding to force alignment */
    MAC_tuMcpsVsSyncCfmParam uParam;          /**< Union of all possible Confirms */
} MAC_tsMcpsVsSyncCfm;

/* @} */

/****************************************************/
/**** MCPS Deferred Confirm/Indication Interface ****/
/****************************************************/

/**
 * @defgroup g_mac_sap_mcps_dcfm_ind_if MCPS Deferred Confirm/Indication Interface
 * @ingroup g_mac_sap_mcps g_mac_VS
 *
 * The interface for the client to receive an MCPS Deferred Confirm or Indication
 * is via a function callback to the function registered using MAC_vMcpsDcfmIndRegister
 *
 * @{
 */

/**
 * @brief Deferred Confirm/Indication type
 *
 * Indicates the type of deferred confirm or indication
 * @note NB Must not exceed 256 entries
 */
typedef enum
{
    MAC_MCPS_VS_DCFM_DATA = MAC_MCPS_VS_TYPE_OFFSET,
    MAC_MCPS_VS_DCFM_PURGE,
    MAC_MCPS_VS_IND_DATA
} MAC_teMcpsVsDcfmIndType;

#define NUM_MAC_MCPS_VS_DCFM_IND 3

/**
 * @brief MCPS Deferred Confirm/Indication Parameter union
 *
 * Union of all the possible MCPS Deferred Confirms or Indications
 */
typedef union
{
    MAC_tsMcpsVsCfmData  sVsDcfmData;   /**< Deferred transmit data confirm */
    MAC_tsMcpsVsCfmPurge sVsDcfmPurge;  /**< Deferred purge confirm */
    MAC_tsMcpsVsIndData  sVsIndData;    /**< Received data indication */
} MAC_tuMcpsVsDcfmIndParam;

/**
 * @brief MCPS Deferred Confirm/Indication
 *
 * The object passed in the MCPS Deferred Confirm/Indication callback
 */
typedef struct
{
    uint8                    u8Type;          /**< Indication type (@sa MAC_teMcpsVsDcfmIndType) */
    uint8                    u8ParamLength;   /**< Parameter length in following union */
    uint16                   u16Pad;          /**< Padding to force alignment */
    MAC_tuMcpsVsDcfmIndParam uParam;          /**< Union of all possible Indications */
} MAC_tsMcpsVsDcfmInd;

/* @} */

/**
 * @defgroup g_mac_sap_gen Generic headers
 * @ingroup g_mac_sap
 *
 * Generic headers which abstract the parameter interfaces to the function calls.
 * The headers reflect the common structure at the head of the derived structures
 * for MLME/MCPS
 *
 * @{
 */
typedef struct
{
    uint8          u8Status;                /**< Status of scan request @sa MAC_Enum_e */
    uint8          u8ScanType;              /**< Scan type */
    uint8          u8ResultListSize;        /**< Size of scan results list */
    uint8          u8Pad;                   /**< Padding to show alignment */
    uint32         u32UnscannedChannels;    /**< Bitmap of unscanned channels */
    uint8          au8EnergyDetect[MAX_SCAN_CHANNEL_COUNT];
} MAC_tsMlmeVsCfmScan;


typedef struct
{
    MAC_PanDescr_s   sPANdescriptor;                                      /**< PAN descriptor */
    uint8            u8BSN;                                               /**< Beacon sequence number */
    uint8            au8BeaconPayload[MAC_MAX_ZBP_BEACON_PAYLOAD_LEN];    /**< Beacon payload */
} MAC_tsMlmeVsIndBeacon;

typedef union
{
    MAC_tsMlmeVsCfmScan         sVsDcfmScan;
    MAC_MlmeCfmAssociate_s      sDcfmAssociate;
    MAC_MlmeCfmDisassociate_s   sDcfmDisassociate;
    MAC_MlmeCfmPoll_s           sDcfmPoll;
    MAC_MlmeCfmRxEnable_s       sDcfmRxEnable;
    MAC_MlmeIndAssociate_s      sIndAssociate;
    MAC_MlmeIndDisassociate_s   sIndDisassociate;
    MAC_MlmeIndGts_s            sIndGts;
    MAC_tsMlmeVsIndBeacon       sVsIndBeacon;
    MAC_MlmeIndSyncLoss_s       sIndSyncLoss;
    MAC_MlmeIndCommStatus_s     sIndCommStatus;
    MAC_MlmeIndOrphan_s         sIndOrphan;
    MAC_MlmeIndDutyCycleMode_s	sIndDutyCycle;
} MAC_tuMlmeVsDcfmIndParam;

typedef struct
{
    uint8                    u8Type;          /**< Deferred Confirm/Indication type @sa MAC_MlmeDcfmIndType_e */
    uint8                    u8ParamLength;   /**< Parameter length in following union */
    uint16                   u16Pad;          /**< Padding to force alignment */
    MAC_tuMlmeVsDcfmIndParam uParam;          /**< Union of all possible Deferred Confirms/Indications */
} MAC_tsMlmeVsDcfmInd;




typedef void (*PR_MAC_MCPS_CALL)(void *, MAC_tsMcpsVsReqRsp *psMcpsVsReqRsp,
										 MAC_tsMcpsVsSyncCfm *psMcpsVsSyncCfm);

typedef void (*PR_MAC_MLME_CALL)(void *, MAC_MlmeReqRsp_s *psMlmeReqRsp,
										 MAC_MlmeSyncCfm_s *psMlmeSyncCfm);


typedef void (*pfnMacVsDataRequest) (uint16 u16Pan, uint16 u16Short);

typedef void (*pfnMacEmptyMcpsFrameWithFpSet) (void);

typedef void (*PR_MAC_CALLBACK)(uint8 u8Status, uint8 u8Channel);

typedef struct
{
    PR_POST_CALLBACK prMlmeCallback;
    PR_POST_CALLBACK prMcpsCallback;
    void *pfGetVsBuffer;
    void *pfVsCallback;
    void *pvApl;
	bool_t bInitTimer;
} MAC_tsInitStruct;


/****************************************************************************/
/***        Exported Functions	                                          ***/
/****************************************************************************/

PUBLIC void			ZPS_vMacShimInit( MAC_tsInitStruct *pStruct );

PUBLIC void 		ZPS_vMacRegisterDataRequest(pfnMacVsDataRequest pfn);

PUBLIC void 		ZPS_vMacRegisterEmptyMcpsFrameWithFpSetCb(pfnMacEmptyMcpsFrameWithFpSet pfn);

PUBLIC void 		ZPS_vMacHandleMcpsVsReqRsp(void* pvMac,
						MAC_tsMcpsVsReqRsp *psMcpsVsReqRsp,
						MAC_tsMcpsVsSyncCfm *psMcpsVsSyncCfm);

PUBLIC void 		ZPS_vMacHandleMlmeVsReqRsp(void* pvMac,
						MAC_MlmeReqRsp_s *psMlmeReqRsp,
						MAC_MlmeSyncCfm_s *psMlmeSyncCfm);

PUBLIC void 		ZPS_vMacTask(void);

#ifdef MAC_FILTER_INCOMING_DATA
PUBLIC void         ZPS_vMacSetFilterStorage (uint16* pu16FilterTable,uint8* pu8LinkQualityTable, uint16 u16MaxTableNumAddrs);
PUBLIC uint32       ZPS_u32MacFilterAddAccept(uint16 u16Addr, uint8 u8LinkQuality);
#endif /* MAC_FILTER_INCOMING_DATA */

PUBLIC void 		ZPS_vMacModifyForZgp(bool_t bStopCca, uint8 u8SeqNo);

PUBLIC void 		ZPS_vNwkHandleMcpsDcfmInd (void *pvNwk,
										MAC_DcfmIndHdr_s *psDcfmIndHdr);

PUBLIC uint32 		ZPS_u32MacSetTxBuffers(uint8 u8MaxTxBuffers);

PUBLIC void 		ZPS_vMacRestoreInterfaceState(void);
PUBLIC void         ZPS_vRestoreMacState (void);
/****************************************************************************/
/***        Multimac Extensions                                           ***/
/****************************************************************************/
PUBLIC bool_t		ZPS_bMacIsMultiMac(void);

PUBLIC uint8 		ZPS_u8MultiMacInitAuxChannel(void);

PUBLIC uint8		ZPS_u8MultiMacSelectAuxChannel(uint8 u8EDScanDuration,
												   uint8 u8EDThreshold,
												   PR_MAC_CALLBACK prCallback);



/****************************************************************************/
/***        Interrupt Handlers                                            ***/
/****************************************************************************/
PUBLIC void 		ZPS_vPhyInterruptHandler(void);
PUBLIC void 		ZPS_vMacInterruptHandler(void);

/****************************************************************************/
/***       Phy PLME 	                                                  ***/
/****************************************************************************/
PUBLIC PHY_Enum_e 	ZPS_eMacPlmeGet(PHY_PibAttr_e ePhyPibAttribute, uint32 *pu32PhyPibValue);
PUBLIC PHY_Enum_e 	ZPS_eMacPlmeSet(PHY_PibAttr_e ePhyPibAttribute, uint32 u32PhyPibValue);



/****************************************************************************/
/***        MAC Interface Table                                           ***/
/****************************************************************************/

typedef uint16 (*PR_MAC_ID_BY_ADDR)(MAC_Addr_s*);


/****************************************************************************/
/***       Antenna setting Get                                                          ***/
/****************************************************************************/

PUBLIC bool_t       ZPS_bSetAntennaInput (uint8 uInput);
/****************************************************************************/
/***       PIB Get		                                                  ***/
/****************************************************************************/
PUBLIC	uint16		ZPS_u16MacPibGetCoordShortAddress(void);
PUBLIC	uint16		ZPS_u16MacPibGetMaxFrameTotalWaitTime(void);
PUBLIC	uint16		ZPS_u16MacPibGetTransactionPersistenceTime(void);
PUBLIC	uint8		ZPS_u8MacPibGetMaxFrameRetries(void);
PUBLIC	uint8		ZPS_u8MacPibGetResponseWaitTime(void);
PUBLIC	uint16		ZPS_u16MacPibGetPanId(void);
PUBLIC	uint16		ZPS_u16MacPibGetShortAddr(void);
PUBLIC	uint8		ZPS_u8MacPibGetMinBe(void);
PUBLIC	uint8		ZPS_u8MacPibGetMaxBe(void);
PUBLIC	uint8		ZPS_u8MacPibGetMaxCsmaBackoffs(void);
PUBLIC	bool_t		ZPS_bMacPibGetRxOnWhenIdle(void);
PUBLIC	tsExtAddr	ZPS_sMacPibGetCoordExtAddr(void);
PUBLIC	uint32		ZPS_u32MacPibGetMacFrameCounter(void);
PUBLIC	bool_t		ZPS_bMacPibGetAssociationPermit(void);
PUBLIC	uint8		ZPS_u8MacPibGetBeaconPayloadLength(void);
PUBLIC	uint8		ZPS_u8MacPibGetBeaconPayload(uint8 *pu8Payload);
PUBLIC	uint8		ZPS_u8MacPibGetBsn(void);
PUBLIC	uint8		ZPS_u8MacPibGetDsn(void);
PUBLIC	bool_t		ZPS_bMacPibGetPanCoordinator(void);
PUBLIC	uint8		ZPS_u8MacPibGetBeaconRequestLqiThreshold(void);
PUBLIC	uint8		ZPS_u8MacPibGetMaxMaxConcurrentAssocResponses(void);
PUBLIC	tsExtAddr 	ZPS_sMacPibGetExtAddr(void);
PUBLIC	uint8		ZPS_u8MacPibGetEBsn(void);
PUBLIC	bool_t		ZPS_bMacPibGetEBFilteringEnable(void);
PUBLIC	uint8		ZPS_u8MacPibGetEBR_PayloadLength(void);
PUBLIC	uint8		ZPS_u8MacPibGetEBR_Payload(uint8 *pu8EBRPayload);
PUBLIC 	uint8		ZPS_u8MacPibGetDutyCycleMode(void);
PUBLIC 	uint16		ZPS_u16MacPibGetDutyCycleMeasurement(void);
PUBLIC	bool_t 		ZPS_bMacPibGetOverwriteTXPowerIE(void);
PUBLIC  uint8 		ZPS_u8MacPibGetProtoInfo(uint8 *pu8ProtoInfo);
PUBLIC	uint16 		ZPS_u16MacPibGetDutyCyleRegulated(uint8 u8MacIntfIndex);
PUBLIC	uint16 		ZPS_u16MacPibGetDutyCyleWarningThreshold(uint8 u8MacIntfIndex);
PUBLIC	uint16 		ZPS_u16MacPibGetDutyCyleCriticalThreshold(uint8 u8MacIntfIndex);

/****************************************************************************/
/***       PIB Set		                                                  ***/
/****************************************************************************/

PUBLIC	void		ZPS_vMacPibSetCoordShortAddress(uint16 u16CoordShortAddr);
PUBLIC	void		ZPS_vMacPibSetMaxFrameTotalWaitTime(uint16 u16DefaultWaitTime, uint16 u16SubGigWaitTime);
PUBLIC	void		ZPS_vMacPibSetTransactionPersistenceTime(uint16 u16DefaultTranPersisTime);
PUBLIC	void		ZPS_vMacPibSetMaxFrameRetries(uint8 u8MaxFrameRetries);
PUBLIC	void		ZPS_vMacPibSetResponseWaitTime(uint8 u8ResponseWaitTime);
PUBLIC	void		ZPS_vMacPibSetPanId(uint16 u16PanID);
PUBLIC	void		ZPS_vMacPibSetShortAddr(uint16 u16ShortAddr);
PUBLIC	void		ZPS_vMacPibSetMinBe(uint8 u8MinBe);
PUBLIC	void		ZPS_vMacPibSetMaxBe(uint8 u8MaxBe);
PUBLIC	void		ZPS_vMacPibSetMaxCsmaBackoffs(uint8 u8MaxCsmaBackoffs);
PUBLIC	void 		ZPS_vMacPibSetRxOnWhenIdle(bool_t bNewState, bool_t bInReset);
PUBLIC	void		ZPS_vMacPibSetCoordExtAddr(tsExtAddr sCoordExtAddr);
PUBLIC	void		ZPS_vMacPibSetMacFrameCounter(uint32 u32MacFrameCounter);
PUBLIC	void		ZPS_vMacPibSetAssociationPermit(bool_t bAssociatePermit);
PUBLIC	void		ZPS_vMacPibSetBsn(uint8 u8Bsn);
PUBLIC	void		ZPS_vMacPibSetDsn(uint8 u8Dsn);
PUBLIC	void		ZPS_vMacPibSetPanCoordinator(bool_t bPanCoordinator);
PUBLIC	void		ZPS_vMacPibSetBeaconRequestLqiThreshold(uint8 u8LqiThreshold);
PUBLIC	void		ZPS_vMacPibSetMaxMaxConcurrentAssocResponses(uint8 u8Max);
PUBLIC	void 		ZPS_vMacPibSetExtAddr(tsExtAddr *psExtAddr);
PUBLIC  void 		ZPS_vMacPibSetRxInCca(bool_t bEnable);
PUBLIC	void		ZPS_vMacPibSetEBsn(uint8 u8EBsn);
PUBLIC	void		ZPS_vMacPibSetEBFilteringEnable(bool_t bEBFilteringEnabled);
PUBLIC	void		ZPS_vMacPibSetEBR_PayloadLength(uint8 u8EBRPayloadLen);
PUBLIC	void		ZPS_vMacPibSetEBR_Payload(const uint8* pu8EBRPayload, uint8 u8EBRPayloadLen);
PUBLIC	void 		ZPS_vMacPibSetOverwriteTXPowerIE(bool_t bOverwrite);
PUBLIC	void 		ZPS_vMacPibSetMIBIEEE_Expiry(uint16 u16Minutes);
PUBLIC	void 		ZPS_vMacPibSetDutyCyleRegulated(uint16 u16HundredsOfPerc);
PUBLIC	void 		ZPS_vMacPibSetDutyCyleWarningThreshold(uint16 u16HundredsOfPerc);
PUBLIC	void 		ZPS_vMacPibSetDutyCyleCriticalThreshold(uint16 u16HundredsOfPerc);
PUBLIC  void 		ZPS_vMacPibSetLBTState(bool_t bEnabled);
PUBLIC  void 		ZPS_vMacSetVsOUIBytes(uint8 *pu8VsOUIBytes);
PUBLIC	void 		ZPS_vMacSetEBRPayloadJoin(bool_t bPermitJoin);
PUBLIC	void 		ZPS_vMacSetEBRPayloadReJoin(uint64 u64ExtPanId, uint16 u16ShortAddress);
PUBLIC  void        ZPS_vMacBeaconRequest(bool_t bEnable);
PUBLIC  void 		ZPS_vMacPibSetRxOnWhenIdleForInterface(uint8 u8MacIntfID, bool_t bNewState, bool_t bInReset);

/****************************************************************************/
/***       Beacons		                                                  ***/
/****************************************************************************/

PUBLIC bool_t		ZPS_bMacGetEnhancedMode(void);
PUBLIC void			ZPS_vMacSetEnhancedMode(bool_t bMode, bool_t bPermitJoining);
PUBLIC void			ZPS_vMacPibSetEBFilteringEnable(bool_t bEBFilteringEnabled);
PUBLIC void 		ZPS_vMacPibSetBeaconPayload(const uint8* pu8Payload, uint8 u8Len);
PUBLIC uint8 		ZPS_u8MacPibGetBeaconPayload(uint8 *pu8Payload);
PUBLIC void 		ZPS_vMacPibSetBeaconPayloadLength(uint8 u8BeaconPayloadLen);
PUBLIC uint8 		ZPS_u8MacPibGetBeaconPayloadLength(void);



/****************************************************************************/
/***      Channel Masks / Radios                                          ***/
/****************************************************************************/


PUBLIC MAC_teRadioType ZPS_eMacRadioTypeFromPage(uint8 u8Page);
PUBLIC MAC_teRadioType ZPS_eMacRadioTypeFromChannelMask(uint32 u32ChannelMask);
PUBLIC teMacStatus  ZPS_eMacValidateChannelMask(uint32 u32Mask);
PUBLIC uint8		ZPS_u8MacGetChannelOffsetFromPage(uint8 u8Page);
PUBLIC uint8		ZPS_u8MacGetChannelOffsetFromMask(uint32 u32ChannelMask);
PUBLIC uint8 		ZPS_u8MacGetChannelNumFromMask(uint32 u32ChannelMask);
PUBLIC uint32 		ZPS_u32MacGetChannelMaskFromNum(uint8 u8RadioType, uint8 u8ChannelNum);
PUBLIC uint8 		ZPS_u8MacGetNumChannelsFromMask(uint32 *pu32ChannelMaskList);
PUBLIC uint8		ZPS_u8MacGetLowChanForPage(uint8 u8Page);
PUBLIC uint8		ZPS_u8MacGetHighChanForPage(uint8 u8Page);

/****************************************************************************/
/***       TX POWER API		                                              ***/
/****************************************************************************/
PUBLIC  uint8 		ZPS_u8MacGetTXPower(uint64 u64IEEEAddress, MAC_TxPower_s *psMacTxPower);
PUBLIC  uint8		ZPS_u8MacSetTXPower(uint64 u64IEEEAddress, MAC_TxPower_s *psMacTxPower);
PUBLIC  uint8 		ZPS_u8MacDeleteTXPower(uint64 u64IEEEAddress);
PUBLIC  uint8		ZPS_u8MacClearTXPowerTable(void);

/****************************************************************************/
/***        MIB IEEE API                                                  ***/
/****************************************************************************/
PUBLIC uint8		ZPS_u8MacMibIeeeSetPolicy(uint8 u8Policy);
PUBLIC uint8		ZPS_u8MacMibIeeeGetPolicy(uint8 *pu8Policy);
PUBLIC uint8		ZPS_u8MacMibIeeeGetCount(uint8 *pu8Count);
PUBLIC uint8		ZPS_u8MacMibIeeeAddDevice(uint64 u64Address, uint8* pu8Count);
PUBLIC uint8		ZPS_u8MacMibIeeeSetTable(uint8 u8Index, uint8 u8Count, uint64 *pu64Address);
PUBLIC uint8		ZPS_u8MacMibIeeeGetTable(uint8 u8Index, uint8 u8Count, uint64 *pu64Address);

/****************************************************************************/
/***        MLME RESET                                                    ***/
/****************************************************************************/
PUBLIC bool_t       ZPS_bMlmeResetFromHigherLayer ( uint8 u8Value );
#ifdef __cplusplus
};
#endif

#endif /* _mac_vs_data_h_ */

/* End of file mac_sap.h *******************************************/
