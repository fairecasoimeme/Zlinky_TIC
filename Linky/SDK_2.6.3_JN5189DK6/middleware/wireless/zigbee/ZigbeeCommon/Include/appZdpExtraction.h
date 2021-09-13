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
 * MODULE:      Utils
 *
 * COMPONENT:   appZdpExtractions.h
 *
 * DESCRIPTION:
 *
 *****************************************************************************/

#ifndef APPZDPEXTRACTION_H_
#define APPZDPEXTRACTION_H_

#include <jendefs.h>
#include "zps_apl_zdp.h"
#include "zps_apl_af.h"
#include "pdum_apl.h"
#include "pdum_nwk.h"

typedef struct {
    union {
        ZPS_tsAplZdpDeviceAnnceReq sDeviceAnnce;
        ZPS_tsAplZdpMgmtNwkUpdateReq sMgmtNwkUpdateReq;
        ZPS_tsAplZdpMgmtNwkEnhanceUpdateReq sMgmtEnhancedUpdateReq;
        ZPS_tsAplZdpMgmtPermitJoiningReq sPermitJoiningReq;
        ZPS_tsAplZdpDiscoveryCacheRsp sDiscoveryCacheRsp;
        ZPS_tsAplZdpDiscoveryStoreRsp sDiscoveryStoreRsp;
        ZPS_tsAplZdpNodeDescStoreRsp sNodeDescStoreRsp;
        ZPS_tsAplZdpActiveEpStoreRsp sActiveEpStoreRsp;
        ZPS_tsAplZdpSimpleDescStoreRsp sSimpleDescStoreRsp;
        ZPS_tsAplZdpRemoveNodeCacheRsp sRemoveNodeCacheRsp;
        ZPS_tsAplZdpEndDeviceBindRsp sEndDeviceBindRsp;
        ZPS_tsAplZdpBindRsp sBindRsp;
        ZPS_tsAplZdpUnbindRsp sUnbindRsp;
        ZPS_tsAplZdpReplaceDeviceRsp sReplaceDeviceRsp;
        ZPS_tsAplZdpStoreBkupBindEntryRsp sStoreBkupBindEntryRsp;
        ZPS_tsAplZdpRemoveBkupBindEntryRsp sRemoveBkupBindEntryRsp;
        ZPS_tsAplZdpBackupSourceBindRsp sBackupSourceBindRsp;
        ZPS_tsAplZdpMgmtLeaveRsp sMgmtLeaveRsp;
        ZPS_tsAplZdpMgmtDirectJoinRsp sMgmtDirectJoinRsp;
        ZPS_tsAplZdpMgmtPermitJoiningRsp sPermitJoiningRsp;
        ZPS_tsAplZdpNodeDescRsp sNodeDescRsp;
        ZPS_tsAplZdpPowerDescRsp sPowerDescRsp;
        ZPS_tsAplZdpSimpleDescRsp sSimpleDescRsp;
        ZPS_tsAplZdpNwkAddrRsp sNwkAddrRsp;
        ZPS_tsAplZdpIeeeAddrRsp sIeeeAddrRsp;
        ZPS_tsAplZdpUserDescConf sUserDescConf;
        ZPS_tsAplZdpSystemServerDiscoveryRsp sSystemServerDiscoveryRsp;
        ZPS_tsAplZdpPowerDescStoreRsp sPowerDescStoreRsp;
        ZPS_tsAplZdpUserDescRsp sUserDescRsp;
        ZPS_tsAplZdpActiveEpRsp sActiveEpRsp;
        ZPS_tsAplZdpMatchDescRsp sMatchDescRsp;
        ZPS_tsAplZdpComplexDescRsp sComplexDescRsp;
        ZPS_tsAplZdpFindNodeCacheRsp sFindNodeCacheRsp;
        ZPS_tsAplZdpExtendedSimpleDescRsp sExtendedSimpleDescRsp;
        ZPS_tsAplZdpExtendedActiveEpRsp sExtendedActiveEpRsp;
        ZPS_tsAplZdpBindRegisterRsp sBindRegisterRsp;
        ZPS_tsAplZdpBackupBindTableRsp sBackupBindTableRsp;
        ZPS_tsAplZdpRecoverBindTableRsp sRecoverBindTableRsp;
        ZPS_tsAplZdpRecoverSourceBindRsp sRecoverSourceBindRsp;
        ZPS_tsAplZdpMgmtNwkDiscRsp sMgmtNwkDiscRsp;
        ZPS_tsAplZdpMgmtLqiRsp sMgmtLqiRsp;
        ZPS_tsAplZdpMgmtRtgRsp sRtgRsp;
        ZPS_tsAplZdpMgmtBindRsp sMgmtBindRsp;
        ZPS_tsAplZdpMgmtCacheRsp sMgmtCacheRsp;
        ZPS_tsAplZdpMgmtNwkUpdateNotify sMgmtNwkUpdateNotify;
        ZPS_tsAplZdpMgmtNwkUnSolictedUpdateNotify sMgmtNwkUnsolicitedUpdateNotify;
    }uZdpData;
    union {
        ZPS_tsAplZdpBindingTableEntry asBindingTable[5];
        ZPS_tsAplZdpNetworkDescr asNwkDescTable[5];
        ZPS_tsAplZdpNtListEntry asNtList[2];
        ZPS_tsAplDiscoveryCache aDiscCache[5];
        uint16 au16Data[34];
        uint8 au8Data[77];
        uint64 au64Data[9];
    }uLists;
    uint16 u16ClusterId;
    uint8 u8SequNumber;
}ZPS_tsAfZdpEvent;


PUBLIC bool zps_bAplZdpUnpackNwkAddressResponse(ZPS_tsAfEvent *psZdoServerEvent, 
                                                ZPS_tsAfZdpEvent* psReturnStruct);
                                                
PUBLIC bool zps_bAplZdpUnpackIeeeAddressResponse(ZPS_tsAfEvent *psZdoServerEvent, 
                                                ZPS_tsAfZdpEvent* psReturnStruct);
                                                
PUBLIC bool zps_bAplZdpUnpackNwkUpdateReq(ZPS_tsAfEvent *psZdoServerEvent, 
                                          ZPS_tsAfZdpEvent* psReturnStruct);

PUBLIC bool zps_bAplZdpUnpackEnhancedNwkUpdateReq(ZPS_tsAfEvent *psZdoServerEvent,
                                          ZPS_tsAfZdpEvent* psReturnStruct);
                                          
PUBLIC bool zps_bAplZdpUnpackPermitJoinReq(ZPS_tsAfEvent *psZdoServerEvent,
                                           ZPS_tsAfZdpEvent* psReturnStruct);
                                           
PUBLIC bool zps_bAplZdpUnpackDevicAnnounce(ZPS_tsAfEvent *psZdoServerEvent,
                                           ZPS_tsAfZdpEvent* psReturnStruct);
                                           
PUBLIC bool zps_bAplZdpUnpackNodeDescResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                              ZPS_tsAfZdpEvent* psReturnStruct);                                              
                                              
PUBLIC bool zps_bAplZdpUnpackPowerDescResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                               ZPS_tsAfZdpEvent* psReturnStruct);
                                               
PUBLIC bool zps_bAplZdpUnpackSimpleDescResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                                ZPS_tsAfZdpEvent* psReturnStruct);
                                                
PUBLIC bool zps_bAplZdpUnpackActiveEpResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                              ZPS_tsAfZdpEvent* psReturnStruct);
                                              
PUBLIC bool zps_bAplZdpUnpackMatchDescResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                               ZPS_tsAfZdpEvent* psReturnStruct);
                                               
PUBLIC bool zps_bAplZdpUnpackDiscCacheResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                               ZPS_tsAfZdpEvent* psReturnStruct);
                                               
PUBLIC bool zps_bAplZdpUnpackDiscStoreResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                               ZPS_tsAfZdpEvent* psReturnStruct);
                                               
PUBLIC bool zps_bAplZdpUnpackNodeDescStoreResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                                   ZPS_tsAfZdpEvent* psReturnStruct);
                                                   
PUBLIC bool zps_bAplZdpUnpackActiveEpStoreResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                                   ZPS_tsAfZdpEvent* psReturnStruct);
                                                   
PUBLIC bool zps_bAplZdpUnpackSimpleDescStoreResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                                     ZPS_tsAfZdpEvent* psReturnStruct);
                                                     
PUBLIC bool zps_bAplZdpUnpackRemoveNodeCacheResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                                     ZPS_tsAfZdpEvent* psReturnStruct);
                                                     
PUBLIC bool zps_bAplZdpUnpackBackUpSourceBindResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                                      ZPS_tsAfZdpEvent* psReturnStruct);
                                                                                                            
PUBLIC bool zps_bAplZdpUnpackMgmtLeaveResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                               ZPS_tsAfZdpEvent* psReturnStruct);
                                               
PUBLIC bool zps_bAplZdpUnpackMgmtDirectJoinResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                                    ZPS_tsAfZdpEvent* psReturnStruct);
                                                    
PUBLIC bool zps_bAplZdpUnpackMgmtPermitJoinResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                                    ZPS_tsAfZdpEvent* psReturnStruct);
                                                    
PUBLIC bool zps_bAplZdpUnpackEndDeviceBindResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                                   ZPS_tsAfZdpEvent* psReturnStruct);
                                                   
PUBLIC bool zps_bAplZdpUnpackBindResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                          ZPS_tsAfZdpEvent* psReturnStruct);
                                          
PUBLIC bool zps_bAplZdpUnpackUnBindResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                            ZPS_tsAfZdpEvent* psReturnStruct);
                                            
PUBLIC bool zps_bAplZdpUnpackReplaceDeviceResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                                   ZPS_tsAfZdpEvent* psReturnStruct);
                                                   
PUBLIC bool zps_bAplZdpUnpackStoreBkupBindResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                                   ZPS_tsAfZdpEvent* psReturnStruct);
                                                   
PUBLIC bool zps_bAplZdpUnpackRemoveBkupBindResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                                    ZPS_tsAfZdpEvent* psReturnStruct);
                                                    
PUBLIC bool zps_bAplZdpUnpackMgmtLqiResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                             ZPS_tsAfZdpEvent* psReturnStruct);
                                             
PUBLIC bool zps_bAplZdpUnpackMgmtRtgResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                             ZPS_tsAfZdpEvent* psReturnStruct);
                                             
PUBLIC bool zps_bAplZdpUnpackNwkUpdateNotifyResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                                     ZPS_tsAfZdpEvent* psReturnStruct);

PUBLIC bool zps_bAplZdpUnpackNwkUnsolicitedUpdateNotify(ZPS_tsAfEvent *psZdoServerEvent,
                                                     ZPS_tsAfZdpEvent* psReturnStruct );
                                                     
PUBLIC bool zps_bAplZdpUnpackComplexDescResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                                 ZPS_tsAfZdpEvent* psReturnStruct);
                                                 
PUBLIC bool zps_bAplZdpUnpackUserDescResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                              ZPS_tsAfZdpEvent* psReturnStruct);
                                              
PUBLIC bool zps_bAplZdpUnpackUserDescConfirmResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                                     ZPS_tsAfZdpEvent* psReturnStruct);
                                                     
PUBLIC bool zps_bAplZdpUnpackSystemServerDiscResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                                      ZPS_tsAfZdpEvent* psReturnStruct);
                                                      
PUBLIC bool zps_bAplZdpUnpackBkupBindTableResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                                   ZPS_tsAfZdpEvent* psReturnStruct);
                                                   
PUBLIC bool zps_bAplZdpUnpackPowerDescStoreResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                                    ZPS_tsAfZdpEvent* psReturnStruct);
                                                    
PUBLIC bool zps_bAplZdpUnpackFindNodeCacheResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                                   ZPS_tsAfZdpEvent* psReturnStruct);
                                                   
PUBLIC bool zps_bAplZdpUnpackExtendedSimpleDescResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                                        ZPS_tsAfZdpEvent* psReturnStruct);
                                                        
PUBLIC bool zps_bAplZdpUnpackExtendedActiveEndpointResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                                            ZPS_tsAfZdpEvent* psReturnStruct);
                                                            
PUBLIC bool zps_bAplZdpUnpackBindRegisterResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                                  ZPS_tsAfZdpEvent* psReturnStruct);
                                                  
PUBLIC bool zps_bAplZdpUnpackRecoverBindTableResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                                      ZPS_tsAfZdpEvent* psReturnStruct);
                                                      
PUBLIC bool zps_bAplZdpUnpackRecoverSourceBindResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                                       ZPS_tsAfZdpEvent* psReturnStruct);                                               

                                                  
PUBLIC bool zps_bAplZdpUnpackMgmtNwkDiscResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                                 ZPS_tsAfZdpEvent* psReturnStruct);
                                                 
PUBLIC bool zps_bAplZdpUnpackMgmtBindResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                              ZPS_tsAfZdpEvent* psReturnStruct);
                                              
PUBLIC bool zps_bAplZdpUnpackMgmtCacheResponse(ZPS_tsAfEvent *psZdoServerEvent,
                                               ZPS_tsAfZdpEvent* psReturnStruct);
                                               
PUBLIC bool zps_bAplZdpUnpackResponse (ZPS_tsAfEvent *psZdoServerEvent,
                                       ZPS_tsAfZdpEvent* psReturnStruct);


#endif /* APPZDPEXTRACTION_H_ */
