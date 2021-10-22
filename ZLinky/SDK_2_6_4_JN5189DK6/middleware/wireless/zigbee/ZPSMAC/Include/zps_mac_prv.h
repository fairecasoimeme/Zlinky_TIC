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
 * MODULE:			zps_mac_prv.h
 *
 * COMPONENT:       ZPSMAC
 *
 * DESCRIPTION:		Non public MAC API with MODULE scope
 *
 ****************************************************************************/

#ifndef  ZPS_MAC_PRV_H_INCLUDED
#define  ZPS_MAC_PRV_H_INCLUDED


/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include "mac_vs_sap.h"
#include "pdum_apl.h"


#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/






/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/



/****************************************************************************/
/***        Callbacks                                                     ***/
/****************************************************************************/
MODULE void
zps_vMacCommonMlmeCallBackHandler(void *pvParam, MAC_MlmeDcfmInd_s *psMlmeDCnfInd);






/****************************************************************************/
/***        Beacons/IEs                                                   ***/
/****************************************************************************/
MODULE uint8
zps_u8MacIEBuildEnhancedBeaconPayload(uint8 *pu8Payload,
		                              const uint8 *pu8ZBPPayload,
		                              bool_t bPANCoord,
		                              bool_t bAssociatePermit,
		                              uint16 u16ShortAddress);
MODULE uint8
zps_u8MacIEBuildJoinPayload(uint8* pu8Payload,
                            bool_t bPermitJoining,
                            bool_t bLinkQualityPresent,
                            bool_t bPercentFilterPresent,
                            uint8  u8LinkQuality,
                            uint8  u8PercentFilter);
MODULE uint8
zps_u8MacIEBuildRejoinPayload(uint8* pu8Payload,
                              uint64 u64ExtPanId,
                              uint16 u16ShortAddress);

MODULE uint8
zps_u8MacIECopyZigbeePayloadSubIE(uint8 *pu8ZBPPayload,
									  uint16 *pu16Superframe,
									  uint16* pu16ShortAddr,
									  const uint8* pu8Payload,
									  uint8 u8Len);
MODULE bool_t
zps_bMacConvertEnhancedBeacon(MAC_MlmeDcfmInd_s *psMlmeInd);





/****************************************************************************/
/***       SOC MAC Specific                                               ***/
/****************************************************************************/
MODULE 	void
zps_vSocMacShimInit( MAC_tsInitStruct *pStruct );

MODULE 	void
zps_vSocMacHandleMcpsVsReqRsp(
		void* pvMac,
		MAC_tsMcpsVsReqRsp *psMcpsVsReqRsp,
		MAC_tsMcpsVsSyncCfm *psMcpsVsSyncCfm);

MODULE 	void
zps_vSocMacHandleMlmeVsReqRsp(
		void* pvMac,
		MAC_MlmeReqRsp_s *psMlmeReqRsp,
		MAC_MlmeSyncCfm_s *psMlmeSyncCfm);



MODULE uint32
zps_eSocMacSetTxBuffers (uint8 u8MaxTxBuffers);

MODULE 	void
zps_vSocMac_InterruptHandler(void);

/****************************************************************************/
/***        ZPS SOC MAC EXPORTS (Called by ZPS MAC SHIM directly          ***/
/****************************************************************************/

MODULE void 		SOC_ZPS_vMacModifyForZgp(bool_t bStopCca, uint8 u8SeqNo);

MODULE void 		SOC_ZPS_vSetMacAddrLocation(void* pu64MacAddress);
MODULE void* 	    SOC_ZPS_pvGetMacAddrLocation(void);
MODULE void         SOC_ZPS_vRestoreMacState (void);
/* ZPS MAC PIB Get Functions */
MODULE	uint16		SOC_ZPS_u16MacPibGetCoordShortAddress(void);
MODULE	uint16		SOC_ZPS_u16MacPibGetMaxFrameTotalWaitTime(void);
MODULE	uint16		SOC_ZPS_u16MacPibGetTransactionPersistenceTime(void);
MODULE	uint8		SOC_ZPS_u8MacPibGetMaxFrameRetries(void);
MODULE	uint8		SOC_ZPS_u8MacPibGetResponseWaitTime(void);
MODULE	uint16		SOC_ZPS_u16MacPibGetPanId(void);
MODULE	uint16		SOC_ZPS_u16MacPibGetShortAddr(void);
MODULE	uint8		SOC_ZPS_u8MacPibGetMinBe(void);
MODULE	uint8		SOC_ZPS_u8MacPibGetMaxBe(void);
MODULE	uint8		SOC_ZPS_u8MacPibGetMaxCsmaBackoffs(void);
MODULE	bool_t		SOC_ZPS_bMacPibGetRxOnWhenIdle(void);
MODULE	tsExtAddr	SOC_ZPS_sMacPibGetCoordExtAddr(void);
MODULE	uint32		SOC_ZPS_u32MacPibGetMacFrameCounter(void);
MODULE	bool_t		SOC_ZPS_bMacPibGetAssociationPermit(void);
MODULE	uint8		SOC_ZPS_u8MacPibGetBeaconPayloadLength(void);
MODULE	uint8		SOC_ZPS_u8MacPibGetBeaconPayload(uint8 *pu8Payload);
MODULE	uint8		SOC_ZPS_u8MacPibGetBsn(void);
MODULE	uint8		SOC_ZPS_u8MacPibGetDsn(void);
MODULE	bool_t		SOC_ZPS_bMacPibGetPanCoordinator(void);
MODULE	uint8		SOC_ZPS_u8MacPibGetBeaconRequestLqiThreshold(void);
MODULE	uint8		SOC_ZPS_u8MacPibGetMaxMaxConcurrentAssocResponses(void);
MODULE	tsExtAddr 	SOC_ZPS_sMacPibGetExtAddr(void);
MODULE	uint8		SOC_ZPS_u8MacPibGetEBsn(void);
MODULE	bool_t		SOC_ZPS_bMacPibGetEBFilteringEnable(void);
MODULE	uint8		SOC_ZPS_u8MacPibGetEBR_PayloadLength(void);
MODULE	uint8		SOC_ZPS_u8MacPibGetEBR_Payload(uint8 *pu8EBRPayload);

/* ZPS MAC PIB Set Functions */
MODULE	void		SOC_ZPS_vMacPibSetCoordShortAddress(uint16 u16CoordShortAddr);
MODULE	void		SOC_ZPS_vMacPibSetMaxFrameTotalWaitTime(uint16 u16MaxFrameTotalWaitTime);
MODULE	void		SOC_ZPS_vMacPibSetTransactionPersistenceTime(uint16 u16TransactionPersistenceTime);
MODULE	void		SOC_ZPS_vMacPibSetMaxFrameRetries(uint8 u8MaxFrameRetries);
MODULE	void		SOC_ZPS_vMacPibSetResponseWaitTime(uint8 u8ResponseWaitTime);
MODULE	void		SOC_ZPS_vMacPibSetPanId(uint16 u16PanID);
MODULE	void		SOC_ZPS_vMacPibSetShortAddr(uint16 u16ShortAddr);
MODULE	void		SOC_ZPS_vMacPibSetMinBe(uint8 u8MinBe);
MODULE	void		SOC_ZPS_vMacPibSetMaxBe(uint8 u8MaxBe);
MODULE	void		SOC_ZPS_vMacPibSetMaxCsmaBackoffs(uint8 u8MaxCsmaBackoffs);
MODULE	void 		SOC_ZPS_vMacPibSetRxOnWhenIdle(bool_t bNewState, bool_t bInReset);
MODULE	void		SOC_ZPS_vMacPibSetCoordExtAddr(tsExtAddr sCoordExtAddr);
MODULE	void		SOC_ZPS_vMacPibSetMacFrameCounter(uint32 u32MacFrameCounter);
MODULE	void		SOC_ZPS_vMacPibSetAssociationPermit(bool_t bAssociatePermit);
MODULE	void		SOC_ZPS_vMacPibSetBeaconPayloadLength(uint8 u8BeaconPayloadLen);
MODULE	void		SOC_ZPS_vMacPibSetBeaconPayload(const uint8* pu8Payload, uint8 u8Len);
MODULE	void		SOC_ZPS_vMacPibSetBsn(uint8 u8Bsn);
MODULE	void		SOC_ZPS_vMacPibSetDsn(uint8 u8Dsn);
MODULE	void		SOC_ZPS_vMacPibSetPanCoordinator(bool_t bPanCoordinator);
MODULE	void		SOC_ZPS_vMacPibSetBeaconRequestLqiThreshold(uint8 u8LqiThreshold);
MODULE	void		SOC_ZPS_vMacPibSetMaxMaxConcurrentAssocResponses(uint8 u8Max);
MODULE	void 		SOC_ZPS_vMacPibSetExtAddr(tsExtAddr *psExtAddr);
MODULE  void 		SOC_ZPS_vMacPibSetRxInCca(bool_t bEnable);

/* TX POWER API */
MODULE  uint8 		SOC_ZPS_u8MacGetTXPower(uint64 u64IEEEAddress, MAC_TxPower_s *psMacTxPower);
MODULE uint8		SOC_ZPS_u8MacSetTXPower(uint64 u64IEEEAddress, MAC_TxPower_s *psMacTxPower);
MODULE uint8 		SOC_ZPS_u8MacDeleteTXPower(uint64 u64IEEEAddress);
MODULE uint8		SOC_ZPS_u8MacClearTXPowerTable(void);

/* MIB IEEE API */
MODULE uint8		SOC_ZPS_u8MacMibIeeeSetPolicy(uint8 u8Policy);
MODULE uint8		SOC_ZPS_u8MacMibIeeeGetPolicy(uint8 *pu8Policy);
MODULE uint8		SOC_ZPS_u8MacMibIeeeGetCount(uint8 *pu8Count);
MODULE uint8		SOC_ZPS_u8MacMibIeeeAddDevice(uint64 u64Address, uint8* pu8Count);
MODULE uint8		SOC_ZPS_u8MacMibIeeeSetTable(uint8 u8Index, uint8 u8Count, uint64 *pu64Address);
MODULE uint8		SOC_ZPS_u8MacMibIeeeGetTable(uint8 u8Index, uint8 u8Count, uint64 *pu64Address);

/* PHY PLME */
MODULE PHY_Enum_e 	SOC_ZPS_eMacPlmeGet(PHY_PibAttr_e ePhyPibAttribute, uint32 *pu32PhyPibValue);
MODULE PHY_Enum_e 	SOC_ZPS_eMacPlmeSet(PHY_PibAttr_e ePhyPibAttribute, uint32 u32PhyPibValue);


/****************************************************************************/
/***       Serial MAC Specific                                            ***/
/****************************************************************************/
MODULE void zps_vSerialMacShimInit(	MAC_tsInitStruct *pStruct );

MODULE void
zps_vSerialMacHandleMcpsVsReqRsp(
		void *pvMac,
		MAC_tsMcpsVsReqRsp *psMcpsVsReqRsp,
		MAC_tsMcpsVsSyncCfm *psMcpsVsSyncCfm);

MODULE void
zps_vSerialMacHandleMlmeVsReqRsp(void *pvMac,
		MAC_MlmeReqRsp_s *psMlmeReqRsp,
		MAC_MlmeSyncCfm_s *psMlmeSyncCfm);


MODULE 	void
zps_vSerialMac_InterruptHandler(void);
MODULE void SERIAL_ZPS_vMacModifyForZgp(bool_t bStopCca, uint8 u8SeqNo);
MODULE void SERIAL_ZPS_vNwkHandleMcpsDcfmInd (void *pvNwk,
										MAC_DcfmIndHdr_s *psDcfmIndHdr);

MODULE void 		SERIAL_ZPS_vSetMacAddrLocation(void* pu64MacAddress);
MODULE void* 	    SERIAL_ZPS_pvGetMacAddrLocation(void);


/* ZPS MAC PIB Get Functions */
MODULE	uint16		SERIAL_ZPS_u16MacPibGetCoordShortAddress(void);
MODULE	uint16		SERIAL_ZPS_u16MacPibGetMaxFrameTotalWaitTime(void);
MODULE	uint16		SERIAL_ZPS_u16MacPibGetTransactionPersistenceTime(void);
MODULE	uint8		SERIAL_ZPS_u8MacPibGetMaxFrameRetries(void);
MODULE	uint8		SERIAL_ZPS_u8MacPibGetResponseWaitTime(void);
MODULE	uint16		SERIAL_ZPS_u16MacPibGetPanId(void);
MODULE	uint16		SERIAL_ZPS_u16MacPibGetShortAddr(void);
MODULE	uint8		SERIAL_ZPS_u8MacPibGetMinBe(void);
MODULE	uint8		SERIAL_ZPS_u8MacPibGetMaxBe(void);
MODULE	uint8		SERIAL_ZPS_u8MacPibGetMaxCsmaBackoffs(void);
MODULE	bool_t		SERIAL_ZPS_bMacPibGetRxOnWhenIdle(void);
MODULE	tsExtAddr	SERIAL_ZPS_sMacPibGetCoordExtAddr(void);
MODULE	uint32		SERIAL_ZPS_u32MacPibGetMacFrameCounter(void);
MODULE	bool_t		SERIAL_ZPS_bMacPibGetAssociationPermit(void);
MODULE	uint8		SERIAL_ZPS_u8MacPibGetBeaconPayloadLength(void);
MODULE	uint8		SERIAL_ZPS_u8MacPibGetBeaconPayload(uint8 *pu8Payload);
MODULE	uint8		SERIAL_ZPS_u8MacPibGetBsn(void);
MODULE	uint8		SERIAL_ZPS_u8MacPibGetDsn(void);
MODULE	bool_t		SERIAL_ZPS_bMacPibGetPanCoordinator(void);
MODULE	uint8		SERIAL_ZPS_u8MacPibGetBeaconRequestLqiThreshold(void);
MODULE	uint8		SERIAL_ZPS_u8MacPibGetMaxMaxConcurrentAssocResponses(void);
MODULE	tsExtAddr 	SERIAL_ZPS_sMacPibGetExtAddr(void);
MODULE	uint8		SERIAL_ZPS_u8MacPibGetEBsn(void);
MODULE	bool_t		SERIAL_ZPS_bMacPibGetEBFilteringEnable(void);
MODULE	uint8		SERIAL_ZPS_u8MacPibGetEBR_PayloadLength(void);
MODULE	uint8		SERIAL_ZPS_u8MacPibGetEBR_Payload(uint8 *pu8EBRPayload);
MODULE 	uint8		SERIAL_ZPS_u8MacPibGetDutyCycleMode(void);
MODULE 	uint16		SERIAL_ZPS_u16MacPibGetDutyCycleMeasurement(void);
MODULE	bool_t 		SERIAL_ZPS_bMacPibGetOverwriteTXPowerIE(void);
PUBLIC  uint8 		SERIAL_ZPS_bMacPibGetProtocolInfo(uint8 *pu8ProtoInfo);
PUBLIC  uint16 		SERIAL_ZPS_u16MacPibGetDutyCyleRegulated(void);
PUBLIC  uint16		SERIAL_ZPS_u16MacPibGetDutyCyleWarningThreshold(void);
PUBLIC  uint16		SERIAL_ZPS_u16MacPibGetDutyCyleCriticalThreshold(void);


/* ZPS MAC PIB Set Functions */
MODULE	void		SERIAL_ZPS_vMacPibSetCoordShortAddress(uint16 u16CoordShortAddr);
MODULE	void		SERIAL_ZPS_vMacPibSetMaxFrameTotalWaitTime(uint16 u16MaxFrameTotalWaitTime);
MODULE	void		SERIAL_ZPS_vMacPibSetTransactionPersistenceTime(uint16 u16TransactionPersistenceTime);
MODULE	void		SERIAL_ZPS_vMacPibSetMaxFrameRetries(uint8 u8MaxFrameRetries);
MODULE	void		SERIAL_ZPS_vMacPibSetResponseWaitTime(uint8 u8ResponseWaitTime);
MODULE	void		SERIAL_ZPS_vMacPibSetPanId(uint16 u16PanID);
MODULE	void		SERIAL_ZPS_vMacPibSetShortAddr(uint16 u16ShortAddr);
MODULE	void		SERIAL_ZPS_vMacPibSetMinBe(uint8 u8MinBe);
MODULE	void		SERIAL_ZPS_vMacPibSetMaxBe(uint8 u8MaxBe);
MODULE	void		SERIAL_ZPS_vMacPibSetMaxCsmaBackoffs(uint8 u8MaxCsmaBackoffs);
MODULE	void 		SERIAL_ZPS_vMacPibSetRxOnWhenIdle(bool_t bNewState, bool_t bInReset);
MODULE	void		SERIAL_ZPS_vMacPibSetCoordExtAddr(tsExtAddr sCoordExtAddr);
MODULE	void		SERIAL_ZPS_vMacPibSetMacFrameCounter(uint32 u32MacFrameCounter);
MODULE	void		SERIAL_ZPS_vMacPibSetAssociationPermit(bool_t bAssociatePermit);
MODULE  void 		SERIAL_ZPS_vMacSetEnhancedBeaconPayload(void);
MODULE	void		SERIAL_ZPS_vMacPibSetBeaconPayloadLength(uint8 u8BeaconPayloadLen);
MODULE	void		SERIAL_ZPS_vMacPibSetBeaconPayload(const uint8* pu8Payload, uint8 u8Len);
MODULE	void		SERIAL_ZPS_vMacPibSetBsn(uint8 u8Bsn);
MODULE	void		SERIAL_ZPS_vMacPibSetDsn(uint8 u8Dsn);
MODULE	void		SERIAL_ZPS_vMacPibSetPanCoordinator(bool_t bPanCoordinator);
MODULE	void		SERIAL_ZPS_vMacPibSetBeaconRequestLqiThreshold(uint8 u8LqiThreshold);
MODULE	void		SERIAL_ZPS_vMacPibSetMaxMaxConcurrentAssocResponses(uint8 u8Max);
MODULE	void 		SERIAL_ZPS_vMacPibSetExtAddr(tsExtAddr *psExtAddr);
MODULE  void 		SERIAL_ZPS_vMacPibSetRxInCca(bool_t bEnable);
MODULE	void		SERIAL_ZPS_vMacPibSetEBsn(uint8 u8EBsn);
MODULE	void		SERIAL_ZPS_vMacPibSetEBFilteringEnable(bool_t bEBFilteringEnabled);
MODULE	void 		SERIAL_ZPS_vMacSetEBRPayloadJoin(bool_t bPermitJoin);
MODULE	void 		SERIAL_ZPS_vMacSetEBRPayloadReJoin(uint64 u64ExtPanId, uint16 u16ShortAddress);
MODULE	void 		SERIAL_ZPS_vMacSetEBRPayloadClear(void);
MODULE	void		SERIAL_ZPS_vMacPibSetEBR_PayloadLength(uint8 u8EBRPayloadLen);
MODULE	void		SERIAL_ZPS_vMacPibSetEBR_Payload(const uint8* pu8EBRPayload, uint8 u8EBRPayloadLen);
MODULE	void 		SERIAL_ZPS_vMacPibSetOverwriteTXPowerIE(bool_t bOverwrite);
MODULE  void 		SERIAL_ZPS_vMacPibSetLBTState(bool_t bEnabled);
MODULE  void 		SERIAL_ZPS_vMacPibSetMIBIEEE_Expiry(uint16 u16Minutes);
MODULE	void 		SERIAL_ZPS_vMacPibSetDutyCyleRegulated(uint16 u16HundredsOfPerc);
MODULE	void 		SERIAL_ZPS_vMacPibSetDutyCyleWarningThreshold(uint16 u16HundredsOfPerc);
PUBLIC	void 		SERIAL_ZPS_vMacPibSetDutyCyleCriticalThreshold(uint16 u16HundredsOfPerc);
MODULE  void 		SERIAL_ZPS_vMacSetVsOUIBytes(uint8 *puint8VsOUI);

/* TX POWER API */
MODULE  uint8 		SERIAL_ZPS_u8MacGetTXPower(uint64 u64IEEEAddress, MAC_TxPower_s *psMacTxPower);
MODULE uint8		SERIAL_ZPS_u8MacSetTXPower(uint64 u64IEEEAddress, MAC_TxPower_s *psMacTxPower);
MODULE uint8 		SERIAL_ZPS_u8MacDeleteTXPower(uint64 u64IEEEAddress);
MODULE uint8		SERIAL_ZPS_u8MacClearTXPowerTable(void);

/* MIB IEEE API */
MODULE uint8		SERIAL_ZPS_u8MacMibIeeeSetPolicy(uint8 u8Policy);
MODULE uint8		SERIAL_ZPS_u8MacMibIeeeGetPolicy(uint8 *pu8Policy);
MODULE uint8		SERIAL_ZPS_u8MacMibIeeeGetCount(uint8 *pu8Count);
MODULE uint8		SERIAL_ZPS_u8MacMibIeeeAddDevice(uint64 u64Address, uint8* pu8Count);
MODULE uint8		SERIAL_ZPS_u8MacMibIeeeSetTable(uint8 u8Index, uint8 u8Count, uint64 *pu64Address);
MODULE uint8		SERIAL_ZPS_u8MacMibIeeeGetTable(uint8 u8Index, uint8 u8Count, uint64 *pu64Address);

/* PHY PLME */
MODULE PHY_Enum_e 	SERIAL_ZPS_eMacPlmeGet(PHY_PibAttr_e ePhyPibAttribute, uint32 *pu32PhyPibValue);
MODULE PHY_Enum_e 	SERIAL_ZPS_eMacPlmeSet(PHY_PibAttr_e ePhyPibAttribute, uint32 u32PhyPibValue);
PUBLIC void         SERIAL_ZPS_vSerialMacResetCallback( uint8 u8ResetReason , uint8 u8Length, uint8* pu8Buffer);

/* Antenna Input Setting */
MODULE bool_t       SERIAL_ZPS_bSetAntennaInput(uint8 u8Input);

/* MLME reset from higher layer */
PUBLIC bool_t SERIAL_ZPS_bMlmeResetFromHigherLayer (uint8 u8Value);
/****************************************************************************/
/***        Multimac Extensions                                           ***/
/****************************************************************************/
MODULE void 		zps_vMultiMacMlmeCallBackHandler(void *pvParam, MAC_MlmeDcfmInd_s *psMlmeDcnfInd);



#if defined __cplusplus
}
#endif


#endif  /* ZPS_MAC_PRV_H_INCLUDED */



/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

