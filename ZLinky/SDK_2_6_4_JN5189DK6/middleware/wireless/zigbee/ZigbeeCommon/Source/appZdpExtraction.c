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


 /*
 * MODULE:      Utils
 *
 * COMPONENT:   appZdpExtractions.c
 *
 * DESCRIPTION:
 *
 *****************************************************************************/

#include "appZdpExtraction.h"
#include <string.h>

/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackNwkAddressResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/
 
PUBLIC bool zps_bAplZdpUnpackNwkAddressResponse(ZPS_tsAfEvent *psZdoServerEvent , 
                                                ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool     bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8    u8SeqNum;
        uint32   u32Location = 0;
        uint32   u32LoopCounter;
        

        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;

        psReturnStruct->uZdpData.sNwkAddrRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        u32Location += PDUM_u16APduInstanceReadNBO(hAPduInst , u32Location, "l",
                &psReturnStruct->uZdpData.sNwkAddrRsp.u64IeeeAddrRemoteDev);
        APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sNwkAddrRsp.u16NwkAddrRemoteDev,hAPduInst , u32Location);

        if( PDUM_u16APduInstanceGetPayloadSize(hAPduInst ) > 12)
        {
            if( psReturnStruct->uZdpData.sNwkAddrRsp.u8Status == ZPS_E_SUCCESS)
            {
                psReturnStruct->uZdpData.sNwkAddrRsp.u8NumAssocDev = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
                psReturnStruct->uZdpData.sNwkAddrRsp.u8StartIndex = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
                if( psReturnStruct->uZdpData.sNwkAddrRsp.u8NumAssocDev !=0 )
                {
                    for (u32LoopCounter = 0; u32LoopCounter < psReturnStruct->uZdpData.sNwkAddrRsp.u8NumAssocDev; u32LoopCounter++)
                    {
                        APDU_BUF_READ16_INC( psReturnStruct->uLists.au16Data[ u32LoopCounter],hAPduInst , u32Location);
                    }
                }
                else
                {
                    psReturnStruct->uZdpData.sNwkAddrRsp.pu16NwkAddrAssocDevList = NULL;
                }
            }
        }
        else
        {
             psReturnStruct->uZdpData.sNwkAddrRsp.u8NumAssocDev = 0;
        }

    }
    return bZdp;
}
                                                
/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackIeeeAddressResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/    
 
PUBLIC bool zps_bAplZdpUnpackIeeeAddressResponse(ZPS_tsAfEvent *psZdoServerEvent , 
                                                ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool    bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8     u8SeqNum;
        uint32    u32Location = 0;
        uint32    u32LoopCounter;
    

        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;       
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;

        psReturnStruct->uZdpData.sNwkAddrRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        u32Location += PDUM_u16APduInstanceReadNBO(hAPduInst , u32Location, "l",
                &psReturnStruct->uZdpData.sNwkAddrRsp.u64IeeeAddrRemoteDev);
        APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sNwkAddrRsp.u16NwkAddrRemoteDev,hAPduInst , u32Location);

        if( PDUM_u16APduInstanceGetPayloadSize(hAPduInst ) > 12)
        {
            if( psReturnStruct->uZdpData.sNwkAddrRsp.u8Status == ZPS_E_SUCCESS)
            {
                psReturnStruct->uZdpData.sNwkAddrRsp.u8NumAssocDev = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
                psReturnStruct->uZdpData.sNwkAddrRsp.u8StartIndex = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
                if( psReturnStruct->uZdpData.sNwkAddrRsp.u8NumAssocDev !=0 )
                {
                    for (u32LoopCounter = 0; u32LoopCounter < psReturnStruct->uZdpData.sNwkAddrRsp.u8NumAssocDev; u32LoopCounter++)
                    {
                        APDU_BUF_READ16_INC( psReturnStruct->uLists.au16Data[ u32LoopCounter],hAPduInst , u32Location);
                    }
                }
                else
                {
                    psReturnStruct->uZdpData.sNwkAddrRsp.pu16NwkAddrAssocDevList = NULL;
                }
            }
        }
        else
        {
             psReturnStruct->uZdpData.sNwkAddrRsp.u8NumAssocDev = 0;
        }
    }
    return bZdp;
}

/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackNwkUpdateReq
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                                
PUBLIC bool zps_bAplZdpUnpackNwkUpdateReq(ZPS_tsAfEvent *psZdoServerEvent , 
                                          ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8 u8SeqNum;
        uint32 u32Location = 0;
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
 
        bZdp = TRUE;
           u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;

        /* Overwriting the data indication event to pass the event as ZDP event and not data indication event*/
        u32Location += PDUM_u16APduInstanceReadNBO(hAPduInst , u32Location, "wb", &psReturnStruct->uZdpData.sMgmtNwkUpdateReq);
        psReturnStruct->uZdpData.sMgmtNwkUpdateReq.u8NwkUpdateId= 0;
        psReturnStruct->uZdpData.sMgmtNwkUpdateReq.u8ScanCount = 0;
        psReturnStruct->uZdpData.sMgmtNwkUpdateReq.u16NwkManagerAddr = 0;
        
        if (5 >= psReturnStruct->uZdpData.sMgmtNwkUpdateReq.u8ScanDuration) {
            psReturnStruct->uZdpData.sMgmtNwkUpdateReq.u8ScanCount = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location];
            u32Location++;
        }
        else if ( psReturnStruct->uZdpData.sMgmtNwkUpdateReq.u8ScanDuration == 0xFE){
            psReturnStruct->uZdpData.sMgmtNwkUpdateReq.u8NwkUpdateId = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location];
            u32Location++;
        }
        else if ( psReturnStruct->uZdpData.sMgmtNwkUpdateReq.u8ScanDuration == 0xFF)
        {
            psReturnStruct->uZdpData.sMgmtNwkUpdateReq.u8NwkUpdateId = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location];
            u32Location++;
            APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sMgmtNwkUpdateReq.u16NwkManagerAddr,hAPduInst , u32Location);
        }
    }
    return bZdp;
}

/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackEnhancedNwkUpdateReq
 */
/**
 *
 * @ingroup
 *
 * @param
 * @param
 * @param
 *
 * @param
 *
 * @return
 *
 * @note
 *
 ****************************************************************************/
PUBLIC bool zps_bAplZdpUnpackEnhancedNwkUpdateReq(ZPS_tsAfEvent *psZdoServerEvent,
                                          ZPS_tsAfZdpEvent* psReturnStruct)
{
    bool bZdp = FALSE;
    int i;
    if( psZdoServerEvent != NULL)
    {
        uint8 u8SeqNum;
        uint32 u32Location = 0;

        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;

        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location = 1;

        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;

        memset(&psReturnStruct->uZdpData.sMgmtEnhancedUpdateReq, 0, sizeof(ZPS_tsAplZdpMgmtNwkEnhanceUpdateReq));

        psReturnStruct->uZdpData.sMgmtEnhancedUpdateReq.sZdpScanChannelList.u8ChannelPageCount =
                (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[u32Location];
        u32Location++;
        if (psReturnStruct->uZdpData.sMgmtEnhancedUpdateReq.sZdpScanChannelList.u8ChannelPageCount > MAX_ZDP_CHANNEL_PAGES)
        {
            psReturnStruct->uZdpData.sMgmtEnhancedUpdateReq.sZdpScanChannelList.u8ChannelPageCount = MAX_ZDP_CHANNEL_PAGES;
        }

        for (i=0;
                i<psReturnStruct->uZdpData.sMgmtEnhancedUpdateReq.sZdpScanChannelList.u8ChannelPageCount;
                i++)
        {
            u32Location += PDUM_u16APduInstanceReadNBO(hAPduInst , u32Location, "w",
                                    &psReturnStruct->uZdpData.sMgmtEnhancedUpdateReq.sZdpScanChannelList.pu32ChannelField[i]);
        }
        /* scan duration */
        psReturnStruct->uZdpData.sMgmtEnhancedUpdateReq.u8ScanDuration =
                        (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[u32Location];
        u32Location++;
        if (psReturnStruct->uZdpData.sMgmtEnhancedUpdateReq.u8ScanDuration <= 5 )
        {
            psReturnStruct->uZdpData.sMgmtEnhancedUpdateReq.u8ScanCount =
                                    (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[u32Location];
            u32Location++;
        }
        else if (psReturnStruct->uZdpData.sMgmtEnhancedUpdateReq.u8ScanDuration == 0xfe )
        {
            psReturnStruct->uZdpData.sMgmtEnhancedUpdateReq.u8NwkUpdateId =
                                    (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[u32Location];
            u32Location++;
        }
        else if (psReturnStruct->uZdpData.sMgmtEnhancedUpdateReq.u8ScanDuration == 0xff )
        {
            psReturnStruct->uZdpData.sMgmtEnhancedUpdateReq.u8NwkUpdateId =
                                                (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[u32Location];
            u32Location++;
            APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sMgmtEnhancedUpdateReq.u16NwkManagerAddr ,hAPduInst , u32Location);

        }
    }
    return bZdp;
}

/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackPermitJoinReq
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                              
PUBLIC bool zps_bAplZdpUnpackPermitJoinReq(ZPS_tsAfEvent *psZdoServerEvent ,
                                           ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8 u8SeqNum;
        uint32 u32Location = 0;
        

        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;

        PDUM_u16APduInstanceReadNBO( psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst , 1, "bb", &psReturnStruct->uZdpData.sPermitJoiningReq);
    }
    return bZdp;
}


/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackDevicAnnounce
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/
 
PUBLIC bool zps_bAplZdpUnpackDevicAnnounce(ZPS_tsAfEvent *psZdoServerEvent ,
                                           ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8 u8SeqNum;
        uint32 u32Location = 0;
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;

        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;

        PDUM_u16APduInstanceReadNBO(hAPduInst , 1, "hlb", &psReturnStruct->uZdpData.sDeviceAnnce);
    }
    return bZdp;
}    


/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackNodeDescResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                       
PUBLIC bool zps_bAplZdpUnpackNodeDescResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                              ZPS_tsAfZdpEvent* psReturnStruct )                                              
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32Location = 0;
        
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;

        psReturnStruct->uZdpData.sNodeDescRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location];
        u32Location++;
        APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sNodeDescRsp.u16NwkAddrOfInterest ,hAPduInst , u32Location);

        if(ZPS_E_SUCCESS == psReturnStruct->uZdpData.sNodeDescRsp.u8Status)
        {
            APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sNodeDescRsp.sNodeDescriptor.uBitUnion.u16Value,hAPduInst , u32Location);
            psReturnStruct->uZdpData.sNodeDescRsp.sNodeDescriptor.u8MacFlags = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
            APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sNodeDescRsp.sNodeDescriptor.u16ManufacturerCode,hAPduInst , u32Location);
            psReturnStruct->uZdpData.sNodeDescRsp.sNodeDescriptor.u8MaxBufferSize = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
            APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sNodeDescRsp.sNodeDescriptor.u16MaxRxSize,hAPduInst , u32Location);
            APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sNodeDescRsp.sNodeDescriptor.u16ServerMask,hAPduInst , u32Location);
            APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sNodeDescRsp.sNodeDescriptor.u16MaxTxSize,hAPduInst , u32Location);
            psReturnStruct->uZdpData.sNodeDescRsp.sNodeDescriptor.u8DescriptorCapability = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        }
        else
        {
            memset( &psReturnStruct->uZdpData.sNodeDescRsp.sNodeDescriptor,0, sizeof( psReturnStruct->uZdpData.sNodeDescRsp.sNodeDescriptor ));
        }
    }
    return bZdp;
}    


/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackPowerDescResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                              
PUBLIC bool zps_bAplZdpUnpackPowerDescResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                               ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32Location = 0;
        
        
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;

        
        psReturnStruct->uZdpData.sPowerDescRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sPowerDescRsp.u16NwkAddrOfInterest ,hAPduInst , u32Location);


        if(ZPS_E_SUCCESS == psReturnStruct->uZdpData.sPowerDescRsp.u8Status)
        {
            APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sPowerDescRsp.sPowerDescriptor.uBitUnion.u16Value,hAPduInst , u32Location);
        }
        else
        {
            memset( &psReturnStruct->uZdpData.sPowerDescRsp.sPowerDescriptor,0, sizeof( psReturnStruct->uZdpData.sPowerDescRsp.sPowerDescriptor ));
        }
    }
    return bZdp;
}

/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackSimpleDescResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                               
PUBLIC bool zps_bAplZdpUnpackSimpleDescResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                                ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32Location = 0;
        uint32     u32pos;
        uint32     u32LoopCounter;
       
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
         bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;
                    
        psReturnStruct->uZdpData.sSimpleDescRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sSimpleDescRsp.u16NwkAddrOfInterest ,hAPduInst , u32Location);
        psReturnStruct->uZdpData.sSimpleDescRsp.u8Length = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];

        if(ZPS_E_SUCCESS == psReturnStruct->uZdpData.sSimpleDescRsp.u8Status)
        {
            psReturnStruct->uZdpData.sSimpleDescRsp.sSimpleDescriptor.u8Endpoint = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
            APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sSimpleDescRsp.sSimpleDescriptor.u16ApplicationProfileId,hAPduInst , u32Location);
            APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sSimpleDescRsp.sSimpleDescriptor.u16DeviceId,hAPduInst , u32Location);
            psReturnStruct->uZdpData.sSimpleDescRsp.sSimpleDescriptor.uBitUnion.u8Value = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
            psReturnStruct->uZdpData.sSimpleDescRsp.sSimpleDescriptor.u8InClusterCount = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
 
            for (u32LoopCounter = 0; u32LoopCounter < psReturnStruct->uZdpData.sSimpleDescRsp.sSimpleDescriptor.u8InClusterCount; u32LoopCounter++)
            {
                APDU_BUF_READ16_INC( psReturnStruct->uLists.au16Data[ u32LoopCounter],hAPduInst , u32Location);
            }
            u32pos = u32LoopCounter;
            psReturnStruct->uZdpData.sSimpleDescRsp.sSimpleDescriptor.pu16InClusterList = &psReturnStruct->uLists.au16Data[0];
            psReturnStruct->uZdpData.sSimpleDescRsp.sSimpleDescriptor.u8OutClusterCount = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
            for(u32LoopCounter = 0; u32LoopCounter < psReturnStruct->uZdpData.sSimpleDescRsp.sSimpleDescriptor.u8OutClusterCount ; u32LoopCounter++)
            {
                APDU_BUF_READ16_INC( psReturnStruct->uLists.au16Data[ u32pos+ u32LoopCounter],hAPduInst , u32Location);
            }
            psReturnStruct->uZdpData.sSimpleDescRsp.sSimpleDescriptor.pu16OutClusterList = &psReturnStruct->uLists.au16Data[ u32pos];
        }
        else
        {
            memset( &psReturnStruct->uZdpData.sSimpleDescRsp.sSimpleDescriptor,0, sizeof( psReturnStruct->uZdpData.sSimpleDescRsp.sSimpleDescriptor ));
        }
    }
    return bZdp;
}    

/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackActiveEpResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                            
PUBLIC bool zps_bAplZdpUnpackActiveEpResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                              ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32Location = 0;
        uint32     u32LoopCounter;
        
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;
        
        
        psReturnStruct->uZdpData.sActiveEpRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sActiveEpRsp.u16NwkAddrOfInterest ,hAPduInst , u32Location);
        psReturnStruct->uZdpData.sActiveEpRsp.u8ActiveEpCount = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];

        if( psReturnStruct->uZdpData.sActiveEpRsp.u8ActiveEpCount != 0)
        {
            for (u32LoopCounter = 0 ; u32LoopCounter < psReturnStruct->uZdpData.sActiveEpRsp.u8ActiveEpCount  ; u32LoopCounter++)
                psReturnStruct->uLists.au8Data[ u32LoopCounter] = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        }
        else
        {
            psReturnStruct->uZdpData.sActiveEpRsp.pu8ActiveEpList = NULL;
        }
    }
    return bZdp;
}    

/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackMatchDescResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                              
PUBLIC bool zps_bAplZdpUnpackMatchDescResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                               ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32Location = 0;
        uint32     u32LoopCounter;
        uint16     u16ShortAddress;
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;
        if( psZdoServerEvent->uEvent.sApsDataIndEvent.u8SrcAddrMode == ZPS_E_ADDR_MODE_SHORT)
        {
            u16ShortAddress = psZdoServerEvent->uEvent.sApsDataIndEvent.uSrcAddress.u16Addr;
        }
        else
        {
            u16ShortAddress = ZPS_u16NwkNibFindNwkAddr(ZPS_pvAplZdoGetNwkHandle(), psZdoServerEvent->uEvent.sApsDataIndEvent.uSrcAddress.u64Addr );
        }
        psReturnStruct->uZdpData.sMatchDescRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sMatchDescRsp.u16NwkAddrOfInterest ,hAPduInst , u32Location);
        psReturnStruct->uZdpData.sMatchDescRsp.u8MatchLength = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        if( psReturnStruct->uZdpData.sMatchDescRsp.u16NwkAddrOfInterest == ZPS_E_BROADCAST_RX_ON)
        {
            psReturnStruct->uZdpData.sMatchDescRsp.u16NwkAddrOfInterest = u16ShortAddress;
        }
        if( psReturnStruct->uZdpData.sMatchDescRsp.u8MatchLength != 0)
        {
            for (u32LoopCounter = 0 ; u32LoopCounter < psReturnStruct->uZdpData.sMatchDescRsp.u8MatchLength ; u32LoopCounter++)
                psReturnStruct->uLists.au8Data[ u32LoopCounter] = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        }
        else
        {
            psReturnStruct->uZdpData.sMatchDescRsp.pu8MatchList = NULL;
        }
    }
    return bZdp;
}    

#ifdef LEGACY_SUPPORT
/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackDiscCacheResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                               
PUBLIC bool zps_bAplZdpUnpackDiscCacheResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                               ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8     u8SeqNum;
        uint32    u32Location = 0;
        
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;
        
        psReturnStruct->uZdpData.sDiscoveryCacheRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
    }
    return bZdp;
}

/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackNodeDescStoreResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                               
PUBLIC bool zps_bAplZdpUnpackNodeDescStoreResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                                   ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8     u8SeqNum;
        uint32    u32Location = 0;
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;
        psReturnStruct->uZdpData.sDiscoveryCacheRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
    }
    return bZdp;
}    


/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackActiveEpStoreResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                               
PUBLIC bool zps_bAplZdpUnpackActiveEpStoreResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                                   ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8     u8SeqNum;
        uint32    u32Location = 0; 
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        bZdp = TRUE;
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;
         
        psReturnStruct->uZdpData.sDiscoveryCacheRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
    }
    return bZdp;
}    


/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackSimpleDescStoreResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                               
PUBLIC bool zps_bAplZdpUnpackSimpleDescStoreResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                                     ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8     u8SeqNum;
        uint32    u32Location = 0;
        
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;

        psReturnStruct->uZdpData.sDiscoveryCacheRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
    }
    return bZdp;
}    

/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackRemoveNodeCacheResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/
 
PUBLIC bool zps_bAplZdpUnpackRemoveNodeCacheResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                                     ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8     u8SeqNum;
        uint32    u32Location = 0;
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;

        psReturnStruct->uZdpData.sDiscoveryCacheRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
    }
    return bZdp;
}    

/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackBackUpSourceBindResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                                 
PUBLIC bool zps_bAplZdpUnpackBackUpSourceBindResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                                      ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8     u8SeqNum;
        uint32    u32Location = 0;
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;

        psReturnStruct->uZdpData.sDiscoveryCacheRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
    }
    return bZdp;
}    
#endif

/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackDiscStoreResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                                   
PUBLIC bool zps_bAplZdpUnpackDiscStoreResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                               ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32Location = 0;
                
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;

        psReturnStruct->uZdpData.sDiscoveryCacheRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
    }
    return bZdp;
}

/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackMgmtLeaveResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                                                                                            
PUBLIC bool zps_bAplZdpUnpackMgmtLeaveResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                               ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32Location = 0;
        
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;

        psReturnStruct->uZdpData.sDiscoveryCacheRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
    }
    return bZdp;
}

#ifdef LEGACY_SUPPORT
/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackMgmtDirectJoinResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                                   
PUBLIC bool zps_bAplZdpUnpackMgmtDirectJoinResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                                    ZPS_tsAfZdpEvent* psReturnStruct )
{
    
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32Location = 0;
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;
        psReturnStruct->uZdpData.sDiscoveryCacheRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
    }
    return bZdp;
}    

#endif
/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackMgmtPermitJoinResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                                    
PUBLIC bool zps_bAplZdpUnpackMgmtPermitJoinResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                                    ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32Location = 0;
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;

        psReturnStruct->uZdpData.sDiscoveryCacheRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
    }
    return bZdp;
}    

#ifdef LEGACY_SUPPORT
/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackEndDeviceBindResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                                    
PUBLIC bool zps_bAplZdpUnpackEndDeviceBindResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                                   ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32Location = 0;
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;

        psReturnStruct->uZdpData.sDiscoveryCacheRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
    }
    return bZdp;
}    
#endif
/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackBindResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                               
PUBLIC bool zps_bAplZdpUnpackBindResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                          ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32Location = 0;
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;

        psReturnStruct->uZdpData.sDiscoveryCacheRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
    }
    return bZdp;
}    


/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackUnBindResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                          
PUBLIC bool zps_bAplZdpUnpackUnBindResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                            ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32Location = 0;        
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;
 
        psReturnStruct->uZdpData.sDiscoveryCacheRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
    }
    return bZdp;
}    

#ifdef LEGACY_SUPPORT
/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackReplaceDeviceResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                            
PUBLIC bool zps_bAplZdpUnpackReplaceDeviceResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                                   ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32Location = 0;
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;

        psReturnStruct->uZdpData.sDiscoveryCacheRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
    }
    return bZdp;
}


/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackStoreBkupBindResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                                       
PUBLIC bool zps_bAplZdpUnpackStoreBkupBindResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                                   ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32Location = 0;        
        
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;

        psReturnStruct->uZdpData.sDiscoveryCacheRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
    }
    return bZdp;
}

/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackRemoveBkupBindResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                                       
PUBLIC bool zps_bAplZdpUnpackRemoveBkupBindResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                                    ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32Location = 0;        
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;

        psReturnStruct->uZdpData.sDiscoveryCacheRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
    }
    return bZdp;
}    

#endif
/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackMgmtLqiResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                                    
PUBLIC bool zps_bAplZdpUnpackMgmtLqiResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                             ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32Location = 0;
        uint32     u32LoopCounter;
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;
        
        psReturnStruct->uZdpData.sMgmtLqiRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        psReturnStruct->uZdpData.sMgmtLqiRsp.u8NeighborTableEntries = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        psReturnStruct->uZdpData.sMgmtLqiRsp.u8StartIndex = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        psReturnStruct->uZdpData.sMgmtLqiRsp.u8NeighborTableListCount = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];

        if(ZPS_E_SUCCESS == psReturnStruct->uZdpData.sMgmtLqiRsp.u8Status)
        {
            for(u32LoopCounter = 0 ; u32LoopCounter < psReturnStruct->uZdpData.sMgmtLqiRsp.u8NeighborTableListCount ; u32LoopCounter++)
            {
                u32Location += PDUM_u16APduInstanceReadNBO(hAPduInst , u32Location, "l",
                                    &psReturnStruct->uLists.asNtList[ u32LoopCounter].u64ExtPanId);
                u32Location += PDUM_u16APduInstanceReadNBO(hAPduInst , u32Location, "l",
                                    &psReturnStruct->uLists.asNtList[ u32LoopCounter].u64ExtendedAddress);
                APDU_BUF_READ16_INC( psReturnStruct->uLists.asNtList[ u32LoopCounter].u16NwkAddr,hAPduInst , u32Location);
                psReturnStruct->uLists.asNtList[ u32LoopCounter].uAncAttrs.au8Field[0] = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
                psReturnStruct->uLists.asNtList[ u32LoopCounter].uAncAttrs.au8Field[1] = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
                psReturnStruct->uLists.asNtList[ u32LoopCounter].u8Depth = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
                psReturnStruct->uLists.asNtList[ u32LoopCounter].u8LinkQuality = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
            }
        }
        else
        {
            psReturnStruct->uZdpData.sMgmtLqiRsp.psNetworkTableList = NULL;
        }
    }
    return bZdp;
}    

/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackMgmtRtgResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                             
PUBLIC bool zps_bAplZdpUnpackMgmtRtgResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                             ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32Location = 0;
        uint32     u32LoopCounter;
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;
        
        psReturnStruct->uZdpData.sRtgRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        psReturnStruct->uZdpData.sRtgRsp.u8RoutingTableEntries = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        psReturnStruct->uZdpData.sRtgRsp.u8StartIndex = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        psReturnStruct->uZdpData.sRtgRsp.u8RoutingTableCount = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];

        if(ZPS_E_SUCCESS == psReturnStruct->uZdpData.sRtgRsp.u8Status)
        {
            for(u32LoopCounter = 0 ; u32LoopCounter < psReturnStruct->uZdpData.sRtgRsp.u8RoutingTableEntries ; u32LoopCounter++)
            {
                APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sRtgRsp.asRoutingTableList[ u32LoopCounter].u16NwkDstAddr,hAPduInst , u32Location);
                psReturnStruct->uZdpData.sRtgRsp.asRoutingTableList[ u32LoopCounter].u8Flags = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
                APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sRtgRsp.asRoutingTableList[ u32LoopCounter].u16NwkNxtHopAddr,hAPduInst , u32Location);
            }
        }
    }
    return bZdp;
}    


/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackNwkUpdateNotifyResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                         
PUBLIC bool zps_bAplZdpUnpackNwkUpdateNotifyResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                                     ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32Location = 0;
        uint32     u32LoopCounter;
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;
           
        psReturnStruct->uZdpData.sMgmtNwkUpdateNotify.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        u32Location += PDUM_u16APduInstanceReadNBO(hAPduInst , u32Location, "w",
                &psReturnStruct->uZdpData.sMgmtNwkUpdateNotify.u32ScannedChannels);
        APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sMgmtNwkUpdateNotify.u16TotalTransmissions,hAPduInst , u32Location);
        APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sMgmtNwkUpdateNotify.u16TransmissionFailures,hAPduInst , u32Location);
        psReturnStruct->uZdpData.sMgmtNwkUpdateNotify.u8ScannedChannelListCount = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];

        if(ZPS_E_SUCCESS == psReturnStruct->uZdpData.sMgmtNwkUpdateNotify.u8Status)
        {
            for(u32LoopCounter = 0 ; u32LoopCounter < psReturnStruct->uZdpData.sMgmtNwkUpdateNotify.u8ScannedChannelListCount ; u32LoopCounter++)
            {
                psReturnStruct->uLists.au8Data[ u32LoopCounter] = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
            }
        }
        else
        {
            psReturnStruct->uZdpData.sMgmtNwkUpdateNotify.pu8EnergyValuesList = NULL;
        }
    }
    return bZdp;
}

/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackNwkUnsolicitedUpdateNotify
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                         
PUBLIC bool zps_bAplZdpUnpackNwkUnsolicitedUpdateNotify(ZPS_tsAfEvent *psZdoServerEvent ,
                                                     ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32Location = 0;
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;
           
        psReturnStruct->uZdpData.sMgmtNwkUnsolicitedUpdateNotify.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        u32Location += PDUM_u16APduInstanceReadNBO(hAPduInst , u32Location, "w",
                &psReturnStruct->uZdpData.sMgmtNwkUnsolicitedUpdateNotify.u32ChannelInUse);
        APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sMgmtNwkUnsolicitedUpdateNotify.u16MACTxUnicastTotal,hAPduInst , u32Location);
        APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sMgmtNwkUnsolicitedUpdateNotify.u16MACTxUnicastFailures,hAPduInst , u32Location);
        APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sMgmtNwkUnsolicitedUpdateNotify.u16MACTxUnicastRetries,hAPduInst , u32Location);
        psReturnStruct->uZdpData.sMgmtNwkUnsolicitedUpdateNotify.u8PeriodOfTimeForResults = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
    }
    return bZdp;
}
#ifdef LEGACY_SUPPORT
/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackComplexDescResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                                         
PUBLIC bool zps_bAplZdpUnpackComplexDescResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                                 ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32Location = 0;
        uint32     u32LoopCounter;
        
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;
         psReturnStruct->uZdpData.sComplexDescRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sComplexDescRsp.u16NwkAddrOfInterest ,hAPduInst , u32Location);
        psReturnStruct->uZdpData.sComplexDescRsp.u8Length = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];


        if(ZPS_E_SUCCESS == psReturnStruct->uZdpData.sComplexDescRsp.u8Status)
        {
            psReturnStruct->uZdpData.sComplexDescRsp.sComplexDescriptor.u8XMLTag = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
            psReturnStruct->uZdpData.sComplexDescRsp.sComplexDescriptor.u8FieldCount = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];


            for(u32LoopCounter = 0 ; u32LoopCounter < psReturnStruct->uZdpData.sComplexDescRsp.sComplexDescriptor.u8FieldCount ; u32LoopCounter++)
            {
                psReturnStruct->uLists.au8Data[ u32LoopCounter] = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
            }
        }
        else
        {
            memset( &psReturnStruct->uZdpData.sComplexDescRsp.sComplexDescriptor,0, sizeof( psReturnStruct->uZdpData.sComplexDescRsp.sComplexDescriptor ));
        }
    }
    return bZdp;
}    

/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackUserDescResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                             
PUBLIC bool zps_bAplZdpUnpackUserDescResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                              ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32Location = 0;
        uint32     u32LoopCounter;
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;
        
        psReturnStruct->uZdpData.sUserDescRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sUserDescRsp.u16NwkAddrOfInterest ,hAPduInst , u32Location);
        psReturnStruct->uZdpData.sUserDescRsp.u8Length = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];

        for (u32LoopCounter = 0; u32LoopCounter < psReturnStruct->uZdpData.sUserDescRsp.u8Length; u32LoopCounter++)
        {
            psReturnStruct->uZdpData.sUserDescRsp.szUserDescriptor[ u32LoopCounter] = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        }
    }
    return bZdp;
}    
#endif

/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackUserDescConfirmResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                              
PUBLIC bool zps_bAplZdpUnpackUserDescConfirmResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                                     ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32Location = 0;        
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;

        PDUM_u16APduInstanceReadNBO(hAPduInst , u32Location, "bh",
                    &psReturnStruct->uZdpData.sUserDescConf);
    }
    return bZdp;
}    


/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackSystemServerDiscResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                                     
PUBLIC bool zps_bAplZdpUnpackSystemServerDiscResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                                      ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32Location = 0;
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;

        
        PDUM_u16APduInstanceReadNBO(hAPduInst , u32Location, "bh",
                &psReturnStruct->uZdpData.sUserDescConf);
    }
    return bZdp;
}    

#ifdef LEGACY_SUPPORT
/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackBkupBindTableResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                                  
PUBLIC bool zps_bAplZdpUnpackBkupBindTableResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                                   ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32Location = 0;        
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;

        PDUM_u16APduInstanceReadNBO(hAPduInst , u32Location, "bh",
                &psReturnStruct->uZdpData.sUserDescConf);
    }
    return bZdp;
}


/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackPowerDescStoreResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                                       
PUBLIC bool zps_bAplZdpUnpackPowerDescStoreResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                                    ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8       u8SeqNum;
        uint32      u32Location = 0;
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;

        
        PDUM_u16APduInstanceReadNBO(hAPduInst , u32Location, "blh",
                &psReturnStruct->uZdpData.sPowerDescStoreRsp);
    }
    return bZdp;
}    


/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackFindNodeCacheResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                                
PUBLIC bool zps_bAplZdpUnpackFindNodeCacheResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                                   ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32Location = 0; 
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;
        
        PDUM_u16APduInstanceReadNBO(hAPduInst , u32Location, "hhl",
                &psReturnStruct->uZdpData.sFindNodeCacheRsp);
    }
    return bZdp;
}    


/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackExtendedSimpleDescResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                           
PUBLIC bool zps_bAplZdpUnpackExtendedSimpleDescResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                                        ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32pos;
        uint32     u32Location = 0;
        uint32     u32LoopCounter;
        
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;
       
        psReturnStruct->uZdpData.sExtendedSimpleDescRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sExtendedSimpleDescRsp.u16NwkAddr,hAPduInst , u32Location);
        psReturnStruct->uZdpData.sExtendedSimpleDescRsp.u8EndPoint = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        psReturnStruct->uZdpData.sExtendedSimpleDescRsp.u8AppInputClusterCount = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        psReturnStruct->uZdpData.sExtendedSimpleDescRsp.u8AppOutputClusterCount = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        psReturnStruct->uZdpData.sExtendedSimpleDescRsp.u8StartIndex = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];

        if(ZPS_E_SUCCESS == psReturnStruct->uZdpData.sExtendedSimpleDescRsp.u8Status)
        {
            for(u32LoopCounter = 0 ; u32LoopCounter < psReturnStruct->uZdpData.sExtendedSimpleDescRsp.u8AppInputClusterCount ; u32LoopCounter++)
            {
                APDU_BUF_READ16_INC( psReturnStruct->uLists.au16Data[ u32LoopCounter],hAPduInst , u32Location);
            }
            u32pos = u32LoopCounter;
            for(u32LoopCounter = 0 ; u32LoopCounter < psReturnStruct->uZdpData.sExtendedSimpleDescRsp.u8AppOutputClusterCount ; u32LoopCounter++)
            {
                APDU_BUF_READ16_INC( psReturnStruct->uLists.au16Data[ u32pos+ u32LoopCounter],hAPduInst , u32Location);
            }
        }
        else
        {
            psReturnStruct->uZdpData.sExtendedSimpleDescRsp.pu16AppClusterList = NULL;
        }
    }
    return bZdp;
}

/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackExtendedActiveEndpointResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                                        
PUBLIC bool zps_bAplZdpUnpackExtendedActiveEndpointResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                                            ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8     u8SeqNum;
        uint32    u32Location = 0;
        uint32    u32LoopCounter;
        
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;
        
        psReturnStruct->uZdpData.sExtendedActiveEpRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sExtendedActiveEpRsp.u16NwkAddr,hAPduInst , u32Location);
        psReturnStruct->uZdpData.sExtendedActiveEpRsp.u8ActiveEpCount = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        psReturnStruct->uZdpData.sExtendedActiveEpRsp.u8StartIndex = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];

        if(ZPS_E_SUCCESS == psReturnStruct->uZdpData.sExtendedActiveEpRsp.u8Status)
        {
            for(u32LoopCounter = 0 ; u32LoopCounter < psReturnStruct->uZdpData.sExtendedActiveEpRsp.u8ActiveEpCount ; u32LoopCounter++)
            {
                psReturnStruct->uLists.au8Data[ u32LoopCounter] = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
            }
        }
        else
        {
            psReturnStruct->uZdpData.sExtendedActiveEpRsp.pu8ActiveEpList = NULL;
        }
    }
    return bZdp;
}                                                            


/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackRecoverBindTableResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                              
PUBLIC bool zps_bAplZdpUnpackRecoverBindTableResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                                      ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8     u8SeqNum;
        uint32    u32Location = 0;
        uint32    u32LoopCounter;
        
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
                        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;

        psReturnStruct->uZdpData.sRecoverBindTableRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sRecoverBindTableRsp.u16StartIndex,hAPduInst , u32Location);
        APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sRecoverBindTableRsp.u16BindingTableEntries,hAPduInst , u32Location);
        APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sRecoverBindTableRsp.u16BindingTableListCount ,hAPduInst , u32Location);

        if(ZPS_E_SUCCESS == psReturnStruct->uZdpData.sRecoverBindTableRsp.u8Status)
        {
            for(u32LoopCounter = 0 ; u32LoopCounter < psReturnStruct->uZdpData.sRecoverBindTableRsp.u16BindingTableListCount ; u32LoopCounter++)
            {
                u32Location += PDUM_u16APduInstanceReadNBO(hAPduInst , u32Location, "l",
                                    &psReturnStruct->uZdpData.sRecoverBindTableRsp.sBindingTableList.u64SourceAddress);
                psReturnStruct->uLists.asBindingTable[ u32LoopCounter].u8SourceEndpoint= (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
                APDU_BUF_READ16_INC( psReturnStruct->uLists.asBindingTable[ u32LoopCounter].u16ClusterId,hAPduInst , u32Location);
                psReturnStruct->uLists.asBindingTable[ u32LoopCounter].u8DstAddrMode= (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];

                if( psReturnStruct->uLists.asBindingTable[ u32LoopCounter].u8DstAddrMode == 0x3)
                {
                    u32Location += PDUM_u16APduInstanceReadNBO(hAPduInst , u32Location, "l",
                                        &psReturnStruct->uLists.asBindingTable[ u32LoopCounter].uDstAddress.u64Addr );
                    psReturnStruct->uLists.asBindingTable[ u32LoopCounter].u8DstEndPoint = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
                }
                else
                {
                    u32Location += PDUM_u16APduInstanceReadNBO(hAPduInst , u32Location, "l",
                                        &psReturnStruct->uLists.asBindingTable[ u32LoopCounter].uDstAddress.u16Addr );
                }

            }
            psReturnStruct->uZdpData.sRecoverBindTableRsp.sBindingTableList.psBindingTableEntryForSpSrcAddr = &psReturnStruct->uLists.asBindingTable[0];
        }
        else
        {
            memset( &psReturnStruct->uZdpData.sRecoverBindTableRsp.sBindingTableList ,0, sizeof( psReturnStruct->uZdpData.sRecoverBindTableRsp.sBindingTableList ));
        }
    }
    return bZdp;
}


/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackRecoverSourceBindResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                                      
PUBLIC bool zps_bAplZdpUnpackRecoverSourceBindResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                                       ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8     u8SeqNum;
        uint32    u32Location = 0;
        uint32    u32LoopCounter;
            
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;

        
        psReturnStruct->uZdpData.sRecoverSourceBindRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sRecoverSourceBindRsp.u16StartIndex,hAPduInst , u32Location);
        APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sRecoverSourceBindRsp.u16SourceTableEntries,hAPduInst , u32Location);
        APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sRecoverSourceBindRsp.u16SourceTableListCount ,hAPduInst , u32Location);

        if(ZPS_E_SUCCESS == psReturnStruct->uZdpData.sRecoverSourceBindRsp.u8Status)
        {
            for(u32LoopCounter = 0 ; u32LoopCounter < psReturnStruct->uZdpData.sRecoverSourceBindRsp.u16SourceTableListCount ; u32LoopCounter++)
            {
                u32Location += PDUM_u16APduInstanceReadNBO(hAPduInst , u32Location, "l",
                                    &psReturnStruct->uLists.au64Data[ u32LoopCounter]);

            }
        }
        else
        {
            psReturnStruct->uZdpData.sRecoverSourceBindRsp.pu64RcSourceTableList = NULL;
        }
    }
    return bZdp;
}    


/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackBindRegisterResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                                   
PUBLIC bool zps_bAplZdpUnpackBindRegisterResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                                  ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8     u8SeqNum;
        uint32    u32Location = 0;
        uint32    u32LoopCounter;
        
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;
        
        psReturnStruct->uZdpData.sBindRegisterRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sBindRegisterRsp.u16BindingTableEntries,hAPduInst , u32Location);
        APDU_BUF_READ16_INC( psReturnStruct->uZdpData.sBindRegisterRsp.u16BindingTableListCount ,hAPduInst , u32Location);

        if(ZPS_E_SUCCESS == psReturnStruct->uZdpData.sBindRegisterRsp.u8Status)
        {
            for(u32LoopCounter = 0 ; u32LoopCounter < psReturnStruct->uZdpData.sBindRegisterRsp.u16BindingTableListCount ; u32LoopCounter++)
            {
                u32Location += PDUM_u16APduInstanceReadNBO(hAPduInst , u32Location, "l",
                                    &psReturnStruct->uZdpData.sBindRegisterRsp.sBindingTableList.u64SourceAddress);
                psReturnStruct->uLists.asBindingTable[ u32LoopCounter].u8SourceEndpoint = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
                APDU_BUF_READ16_INC( psReturnStruct->uLists.asBindingTable[ u32LoopCounter].u16ClusterId,hAPduInst , u32Location);
                psReturnStruct->uLists.asBindingTable[ u32LoopCounter].u8DstAddrMode = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
                if( psReturnStruct->uLists.asBindingTable[ u32LoopCounter].u8DstAddrMode == 0x3)
                {
                    u32Location += PDUM_u16APduInstanceReadNBO(hAPduInst , u32Location, "l",
                                        &psReturnStruct->uLists.asBindingTable[ u32LoopCounter].uDstAddress.u64Addr );
                    psReturnStruct->uLists.asBindingTable[ u32LoopCounter].u8DstEndPoint = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
                }
                else
                {
                    u32Location += PDUM_u16APduInstanceReadNBO(hAPduInst , u32Location, "l",
                                        &psReturnStruct->uLists.asBindingTable[ u32LoopCounter].uDstAddress.u16Addr );
                }

            }
            psReturnStruct->uZdpData.sBindRegisterRsp.sBindingTableList.psBindingTableEntryForSpSrcAddr = &psReturnStruct->uLists.asBindingTable[0];
        }
        else
        {
            memset( &psReturnStruct->uZdpData.sBindRegisterRsp.sBindingTableList ,0, sizeof( psReturnStruct->uZdpData.sBindRegisterRsp.sBindingTableList ));
        }
    }
    return bZdp;
}

/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackMgmtNwkDiscResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                                  
PUBLIC bool zps_bAplZdpUnpackMgmtNwkDiscResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                                 ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8SeqNum;
        uint32     u32Location = 0;
        uint32     u32LoopCounter;
                
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;
        
        psReturnStruct->uZdpData.sMgmtNwkDiscRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        psReturnStruct->uZdpData.sMgmtNwkDiscRsp.u8NetworkCount = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        psReturnStruct->uZdpData.sMgmtNwkDiscRsp.u8StartIndex = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        psReturnStruct->uZdpData.sMgmtNwkDiscRsp.u8NetworkListCount = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];

        if(ZPS_E_SUCCESS == psReturnStruct->uZdpData.sMgmtNwkDiscRsp.u8Status)
        {
            for(u32LoopCounter = 0 ; u32LoopCounter < psReturnStruct->uZdpData.sMgmtNwkDiscRsp.u8NetworkListCount ; u32LoopCounter++)
            {
                u32Location += PDUM_u16APduInstanceReadNBO(hAPduInst , u32Location, "l",
                                    &psReturnStruct->uLists.asNwkDescTable[ u32LoopCounter].u64ExtPanId);
                psReturnStruct->uLists.asNwkDescTable[ u32LoopCounter].u8LogicalChan = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
                psReturnStruct->uLists.asNwkDescTable[ u32LoopCounter].u8StackProfile = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
                psReturnStruct->uLists.asNwkDescTable[ u32LoopCounter].u8ZigBeeVersion = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
                psReturnStruct->uLists.asNwkDescTable[ u32LoopCounter].u8PermitJoining = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
                psReturnStruct->uLists.asNwkDescTable[ u32LoopCounter].u8RouterCapacity = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
                psReturnStruct->uLists.asNwkDescTable[ u32LoopCounter].u8EndDeviceCapacity = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
            }
        }
        else
        {
            psReturnStruct->uZdpData.sMgmtNwkDiscRsp.psZdpNetworkDescrList = NULL;
        }
    }
    return bZdp;
}    
#endif

/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackMgmtBindResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                                 
PUBLIC bool zps_bAplZdpUnpackMgmtBindResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                              ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8      u8value;
        uint8      u8SeqNum;
        uint32     u32Location = 0;
        uint32     u32LoopCounter;
        
        
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;
        
        psReturnStruct->uZdpData.sMgmtBindRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        u8value = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        psReturnStruct->uZdpData.sMgmtBindRsp.u16BindingTableEntries = u8value;
        u8value = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        psReturnStruct->uZdpData.sMgmtBindRsp.u16StartIndex = u8value;
        u8value = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        psReturnStruct->uZdpData.sMgmtBindRsp.u16BindingTableListCount = u8value;
        if(ZPS_E_SUCCESS == psReturnStruct->uZdpData.sMgmtBindRsp.u8Status)
        {
            for(u32LoopCounter = 0 ; u32LoopCounter < psReturnStruct->uZdpData.sMgmtBindRsp.u16BindingTableListCount ; u32LoopCounter++)
            {
                u32Location += PDUM_u16APduInstanceReadNBO(hAPduInst , u32Location, "l",
                                    &psReturnStruct->uZdpData.sMgmtBindRsp.sBindingTableList.u64SourceAddress);
                psReturnStruct->uLists.asBindingTable[ u32LoopCounter].u8SourceEndpoint = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
                APDU_BUF_READ16_INC( psReturnStruct->uLists.asBindingTable[ u32LoopCounter].u16ClusterId,hAPduInst , u32Location);
                psReturnStruct->uLists.asBindingTable[ u32LoopCounter].u8DstAddrMode = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];

                if( psReturnStruct->uLists.asBindingTable[ u32LoopCounter].u8DstAddrMode == 0x3)
                {
                    u32Location += PDUM_u16APduInstanceReadNBO(hAPduInst , u32Location, "l",
                                        &psReturnStruct->uLists.asBindingTable[ u32LoopCounter].uDstAddress.u64Addr );
                    psReturnStruct->uLists.asBindingTable[ u32LoopCounter].u8DstEndPoint = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
                }
                else
                {
                    u32Location += PDUM_u16APduInstanceReadNBO(hAPduInst , u32Location, "l",
                                        &psReturnStruct->uLists.asBindingTable[ u32LoopCounter].uDstAddress.u16Addr );
                }

            }
        }
        else
        {
            psReturnStruct->uZdpData.sMgmtBindRsp.sBindingTableList.psBindingTableEntryForSpSrcAddr = NULL;
            psReturnStruct->uZdpData.sMgmtBindRsp.sBindingTableList.u64SourceAddress = 0;
        }
    }
    return bZdp;
}    

#ifdef LEGACY_SUPPORT
/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackMgmtCacheResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/                                              
PUBLIC bool zps_bAplZdpUnpackMgmtCacheResponse(ZPS_tsAfEvent *psZdoServerEvent ,
                                               ZPS_tsAfZdpEvent* psReturnStruct )
                                               
{
    bool bZdp = FALSE;
    if( psZdoServerEvent != NULL)
    {
        uint8     u8SeqNum;
        uint32    u32Location = 0; 
        uint32    u32LoopCounter;
        
        
        PDUM_thAPduInstance hAPduInst = psZdoServerEvent->uEvent.sApsDataIndEvent.hAPduInst;
        uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
        
        bZdp = TRUE;
        u8SeqNum = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[0];
        u32Location++;
        
        psReturnStruct->u8SequNumber = u8SeqNum;
        psReturnStruct->u16ClusterId = u16ClusterId;
     
        psReturnStruct->uZdpData.sMgmtCacheRsp.u8Status = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        psReturnStruct->uZdpData.sMgmtCacheRsp.u8DiscoveryCacheEntries = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        psReturnStruct->uZdpData.sMgmtCacheRsp.u8StartIndex = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];
        psReturnStruct->uZdpData.sMgmtCacheRsp.u8DiscoveryCacheListCount = (( pdum_tsAPduInstance* )hAPduInst )->au8Storage[ u32Location++ ];

        if(ZPS_E_SUCCESS == psReturnStruct->uZdpData.sMgmtCacheRsp.u8Status)
        {
            for(u32LoopCounter = 0 ; u32LoopCounter < psReturnStruct->uZdpData.sMgmtCacheRsp.u8DiscoveryCacheListCount ; u32LoopCounter++)
            {
                u32Location += PDUM_u16APduInstanceReadNBO(hAPduInst , u32Location, "l",
                                    &psReturnStruct->uLists.aDiscCache[ u32LoopCounter].u64ExtendedAddress);
                APDU_BUF_READ16_INC( psReturnStruct->uLists.aDiscCache[ u32LoopCounter].u16NwkAddress,hAPduInst , u32Location);
            }
        }
        else
        {
            psReturnStruct->uZdpData.sMgmtCacheRsp.psDiscoveryCacheList = NULL;
        }
    }
    return bZdp;
}

#endif
/****************************************************************************
 *
 * NAME:       zps_bAplZdpUnpackResponse
 */
/**
 * 
 *
 * @ingroup 
 *
 * @param 
 * @param 
 * @param 
 *                      
 * @param
 *
 * @return 
 *
 * @note
 *
 ****************************************************************************/    
PUBLIC bool zps_bAplZdpUnpackResponse (ZPS_tsAfEvent *psZdoServerEvent ,ZPS_tsAfZdpEvent* psReturnStruct )
{
    bool bZdp = FALSE;
    uint16 u16ClusterId = psZdoServerEvent->uEvent.sApsDataIndEvent.u16ClusterId;
    switch(u16ClusterId)
    {
        case ZPS_ZDP_MGMT_NWK_UPDATE_REQ_CLUSTER_ID:
            bZdp = zps_bAplZdpUnpackNwkUpdateReq( psZdoServerEvent , psReturnStruct );
            break;
        case ZPS_ZDP_MGMT_NWK_ENHANCE_UPDATE_REQ_CLUSTER_ID:
            bZdp = zps_bAplZdpUnpackEnhancedNwkUpdateReq(psZdoServerEvent , psReturnStruct );
            break;
        case ZPS_ZDP_MGMT_PERMIT_JOINING_REQ_CLUSTER_ID:
            bZdp = zps_bAplZdpUnpackPermitJoinReq( psZdoServerEvent , psReturnStruct );
            break;
        case ZPS_ZDP_DEVICE_ANNCE_REQ_CLUSTER_ID:
            bZdp = zps_bAplZdpUnpackDevicAnnounce( psZdoServerEvent , psReturnStruct );
            break;
        case ZPS_ZDP_NWK_ADDR_RSP_CLUSTER_ID:
            bZdp = zps_bAplZdpUnpackNwkAddressResponse( psZdoServerEvent , psReturnStruct );
            break;
            
        case ZPS_ZDP_IEEE_ADDR_RSP_CLUSTER_ID:
            bZdp = zps_bAplZdpUnpackIeeeAddressResponse( psZdoServerEvent , psReturnStruct );
            break;

        case ZPS_ZDP_NODE_DESC_RSP_CLUSTER_ID:
            bZdp = zps_bAplZdpUnpackNodeDescResponse( psZdoServerEvent , psReturnStruct );
            break;

        case ZPS_ZDP_POWER_DESC_RSP_CLUSTER_ID:
            bZdp = zps_bAplZdpUnpackPowerDescResponse( psZdoServerEvent , psReturnStruct );
            break;

        case ZPS_ZDP_SIMPLE_DESC_RSP_CLUSTER_ID:
            bZdp = zps_bAplZdpUnpackSimpleDescResponse( psZdoServerEvent , psReturnStruct );
            break;

        case ZPS_ZDP_ACTIVE_EP_RSP_CLUSTER_ID:
            bZdp = zps_bAplZdpUnpackActiveEpResponse( psZdoServerEvent , psReturnStruct );
            break;

        case ZPS_ZDP_MATCH_DESC_RSP_CLUSTER_ID:
            bZdp = zps_bAplZdpUnpackMatchDescResponse( psZdoServerEvent , psReturnStruct );
            break;
#ifdef LEGACY_SUPPORT
        /*Structures are the same size and should align in the union */
        case ZPS_ZDP_DISCOVERY_CACHE_RSP_CLUSTER_ID:
            bZdp = zps_bAplZdpUnpackDiscCacheResponse( psZdoServerEvent , psReturnStruct );
            break;
#endif

#ifdef LEGACY_SUPPORT
        case ZPS_ZDP_DISCOVERY_STORE_RSP_CLUSTER_ID:
        case ZPS_ZDP_NODE_DESC_STORE_RSP_CLUSTER_ID:
        case ZPS_ZDP_ACTIVE_EP_STORE_RSP_CLUSTER_ID:
        case ZPS_ZDP_SIMPLE_DESC_STORE_RSP_CLUSTER_ID:
        case ZPS_ZDP_REMOVE_NODE_CACHE_RSP_CLUSTER_ID:
        case ZPS_ZDP_BKUP_SOURCE_BIND_RSP_CLUSTER_ID:        
        case ZPS_ZDP_MGMT_DIRECT_JOIN_RSP_CLUSTER_ID:        
        case ZPS_ZDP_REPLACE_DEVICE_RSP_CLUSTER_ID:
        case ZPS_ZDP_STORE_BKUP_BIND_RSP_CLUSTER_ID:
        case ZPS_ZDP_REMOVE_BKUP_BIND_RSP_CLUSTER_ID:
#endif
        case ZPS_ZDP_END_DEVICE_BIND_RSP_CLUSTER_ID:
        case ZPS_ZDP_MGMT_LEAVE_RSP_CLUSTER_ID:
        case ZPS_ZDP_BIND_RSP_CLUSTER_ID:
        case ZPS_ZDP_UNBIND_RSP_CLUSTER_ID:
        case ZPS_ZDP_MGMT_PERMIT_JOINING_RSP_CLUSTER_ID:
            bZdp = zps_bAplZdpUnpackDiscStoreResponse( psZdoServerEvent , psReturnStruct );
            break;

        case ZPS_ZDP_MGMT_LQI_RSP_CLUSTER_ID:
             bZdp = zps_bAplZdpUnpackMgmtLqiResponse( psZdoServerEvent , psReturnStruct );
            break;

        case ZPS_ZDP_MGMT_RTG_RSP_CLUSTER_ID:
            bZdp = zps_bAplZdpUnpackMgmtRtgResponse( psZdoServerEvent , psReturnStruct );
            break;

        case ZPS_ZDP_MGMT_NWK_UPDATE_NOTIFY_CLUSTER_ID:
        case ZPS_ZDP_MGMT_NWK_ENHANCE_UPDATE_NOTIFY_CLUSTER_ID:
            bZdp = zps_bAplZdpUnpackNwkUpdateNotifyResponse( psZdoServerEvent , psReturnStruct );
            break;

        case ZPS_ZDP_MGMT_NWK_UNSOLICITED_ENHANCE_UPDATE_NOTIFY_CLUSTER_ID:
        	bZdp = zps_bAplZdpUnpackNwkUnsolicitedUpdateNotify( psZdoServerEvent , psReturnStruct );
            break;
#ifdef LEGACY_SUPPORT
        case ZPS_ZDP_COMPLEX_DESC_RSP_CLUSTER_ID:
            bZdp = zps_bAplZdpUnpackComplexDescResponse( psZdoServerEvent , psReturnStruct );
            break;

        case ZPS_ZDP_USER_DESC_RSP_CLUSTER_ID:
            bZdp = zps_bAplZdpUnpackUserDescResponse( psZdoServerEvent , psReturnStruct );
            break;

            /*Structures are the same size and should align in the union */
        case ZPS_ZDP_USER_DESC_CONF_RSP_CLUSTER_ID:
        case ZPS_ZDP_BKUP_BIND_TABLE_RSP_CLUSTER_ID:
#endif
        case ZPS_ZDP_SYSTEM_SERVER_DISCOVERY_RSP_CLUSTER_ID:
            bZdp = zps_bAplZdpUnpackUserDescConfirmResponse( psZdoServerEvent , psReturnStruct );
            break;

#ifdef LEGACY_SUPPORT
        case ZPS_ZDP_POWER_DESC_STORE_RSP_CLUSTER_ID:
            bZdp = zps_bAplZdpUnpackPowerDescStoreResponse( psZdoServerEvent , psReturnStruct );
            break;

        case ZPS_ZDP_FIND_NODE_CACHE_RSP_CLUSTER_ID:
            bZdp = zps_bAplZdpUnpackFindNodeCacheResponse( psZdoServerEvent , psReturnStruct );
            break;

        case ZPS_ZDP_EXTENDED_SIMPLE_DESC_RSP_CLUSTER_ID:
            bZdp = zps_bAplZdpUnpackExtendedSimpleDescResponse( psZdoServerEvent , psReturnStruct );
            break;

        case ZPS_ZDP_EXTENDED_ACTIVE_EP_RSP_CLUSTER_ID:
            bZdp = zps_bAplZdpUnpackExtendedActiveEndpointResponse( psZdoServerEvent , psReturnStruct );
            break;
            
        case ZPS_ZDP_BIND_REGISTER_RSP_CLUSTER_ID:
            bZdp = zps_bAplZdpUnpackBindRegisterResponse( psZdoServerEvent , psReturnStruct );
            break;

        case ZPS_ZDP_RECOVER_BIND_TABLE_RSP_CLUSTER_ID:
            bZdp = zps_bAplZdpUnpackRecoverBindTableResponse( psZdoServerEvent , psReturnStruct );
            break;


        case ZPS_ZDP_RECOVER_SOURCE_BIND_RSP_CLUSTER_ID:
            bZdp = zps_bAplZdpUnpackRecoverSourceBindResponse( psZdoServerEvent , psReturnStruct );
            break;

        case ZPS_ZDP_MGMT_NWK_DISC_RSP_CLUSTER_ID:
            bZdp = zps_bAplZdpUnpackMgmtNwkDiscResponse( psZdoServerEvent , psReturnStruct );
            break;
#endif
        case ZPS_ZDP_MGMT_BIND_RSP_CLUSTER_ID:
              bZdp = zps_bAplZdpUnpackMgmtBindResponse( psZdoServerEvent , psReturnStruct );
            break;
#ifdef LEGACY_SUPPORT
        case ZPS_ZDP_MGMT_CACHE_RSP_CLUSTER_ID:
             bZdp = zps_bAplZdpUnpackMgmtCacheResponse( psZdoServerEvent , psReturnStruct );
            break;
#endif
        default:
            bZdp = FALSE;
            break;
    }
   
    return bZdp;
}
