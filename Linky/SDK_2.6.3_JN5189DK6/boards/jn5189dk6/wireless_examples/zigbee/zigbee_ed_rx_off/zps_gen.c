/****************************************************************************
 *
 *                 THIS IS A GENERATED FILE. DO NOT EDIT!
 *
 * MODULE:         ZPSConfig
 *
 * COMPONENT:      zps_gen.c
 *
 * DATE:           Mon Mar 16 11:04:49 2020
 *
 * AUTHOR:         Jennic Zigbee Protocol Stack Configuration Tool
 *
 * DESCRIPTION:    ZPS definitions
 *
 ****************************************************************************
 *
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5168, JN5179].
 * You, and any third parties must reproduce the copyright and warranty notice
 * and any other legend of ownership on each copy or partial copy of the
 * software.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright NXP B.V. 2016. All rights reserved
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <pdum_gen.h>
#include "ZQueue.h"
#include <zps_gen.h>

#include "zps_apl.h"
#include "zps_apl_aib.h"
#include "zps_apl_af.h"


/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#define COMPILE_TIME_ASSERT(pred)    switch(0){case 0:case pred:;}


#define ZPS_APL_ZDO_VSOUI_LENGTH		3
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/*** ZDP Context **************************************************/

typedef struct {
    uint8 u8ZdpSeqNum;
} zps_tsZdpContext;

/*** ZDO Context **************************************************/

typedef bool (*zps_tprAplZdoServer)(void *pvApl, void *pvServerContext, ZPS_tsAfEvent *psZdoServerEvent);

typedef struct {
    zps_tprAplZdoServer prServer;
    void *pvServerContext;
} zps_tsAplZdoServer;

typedef struct
{
    uint8               au8Key[ZPS_NWK_KEY_LENGTH];
    uint8               u8KeySeqNum;
    uint8               u8KeyType;
} zps_tsAplZdoInitSecKey;

typedef struct {
    uint64 u64InitiatorAddr;
    uint64 u64ResponderAddr;
    ZPS_tsTsvTimer sTimer;
    uint8 au8Key[ZPS_NWK_KEY_LENGTH];
} zps_tsRequestKeyRequests;


typedef struct {
    uint8 au8VsOUIBytes[ZPS_APL_ZDO_VSOUI_LENGTH];
    uint8 eNetworkState; /* ZPS_teZdoNetworkState */
    uint8 eZdoDeviceType; /* ZPS_teZdoDeviceType */
    uint8 eNwkKeyState; /* ZPS_teZdoNwkKeyState */
    uint8 u8PermitJoinTime;
    uint8 u8StackProfile;
    uint8 u8ZigbeeVersion;
    uint8 u8ScanDuration;
    bool_t bLookupTCAddr;
    const zps_tsAplZdoServer *psZdoServers;
    void (*prvZdoServersInit)(void);
    ZPS_tsTsvTimer sAuthenticationTimer;
    ZPS_tsTsvTimer sAuthenticationPollTimer;
    uint8 u8NumPollFailures;
    uint8 u8MaxNumPollFailures;
    bool_t bSecurityDisabled;
    zps_tsAplZdoInitSecKey *psInitSecKey;
    uint8 u8DevicePermissions;
    bool_t (*prvZdoReqFilter)(uint16);
    bool (*pfzps_bAplZdoBindRequestServer)(void *,
            void *,
            ZPS_tsAfEvent *);
    zps_tsRequestKeyRequests *psRequestKeyReqs;
    uint32 u32ReqKeyTimeout;
    uint8 u8MaxNumSimulRequestKeyReqs;    
    
} zps_tsZdoContext;

/**** Context for the ZDO servers data confirms and acks***********/

typedef struct {
    uint8 eState;
    uint8 u8SeqNum;
    uint8 u8ConfAck;
} zps_tsZdoServerConfAckContext;

/*** Trust Center Context *****************************************/

typedef struct
{
    uint16 u16AddrLkup;
    ZPS_teDevicePermissions eDevPermissions;
} zps_tsAplTCDeviceTable;

typedef struct
{
    zps_tsAplTCDeviceTable *asTCDeviceTable;
    uint16  u16SizeOfTCDeviceTable;
} zps_tsAplTCib;


typedef struct
{
    void (*prvTrustCenterInit)(void*);
    void (*prvTrustCenterUpdateDevice)(void*, uint64, uint64, uint8, uint16);
    void (*prvTrustCenterRequestKey)(void*, uint64, uint8, uint64);
    zps_tsAplTCib sTCib;
    bool_t bTcOverride;
    bool_t bChangeOverride;
} zps_tsTrustCenterContext;

/*** AF Context ***************************************************/

typedef struct zps_tsAplAfDynamicContext zps_tsAplAfDynamicContext;

typedef struct _zps_tsAplAfSimpleDescCont
{
    ZPS_tsAplAfSimpleDescriptor sSimpleDesc;
    const PDUM_thAPdu *phAPduInClusters;
    bool_t bEnabled;
} zps_tsAplAfSimpleDescCont;

typedef struct {
    zps_tsAplAfDynamicContext *psDynamicContext;
    ZPS_tsAplAfNodeDescriptor *psNodeDescriptor;
    ZPS_tsAplAfNodePowerDescriptor *psNodePowerDescriptor;
    uint32 u32NumSimpleDescriptors;
    zps_tsAplAfSimpleDescCont *psSimpleDescConts;
    ZPS_tsAplAfUserDescriptor *psUserDescriptor;
    void* hOverrunMsg;   
    uint8 zcp_u8FragApsAckValue;
    uint8 zcp_u8FragBlockControl;
} zps_tsAfContext;

/*** APS Context **************************************************/

typedef struct
{
    uint8 u8Type;
    uint8 u8ParamLength;
} ZPS_tsAplApsmeDcfmIndHdr;

typedef struct
{
    uint8 u8Type;
    uint8 u8ParamLength;
} ZPS_tsAplApsdeDcfmIndHdr;

typedef struct {
    ZPS_tuAddress uDstAddr;
    PDUM_thAPduInstance hAPduInst;
    uint8 *pu8SeqCounter;
    uint16 u16ProfileId;
    uint16 u16ClusterId;
    uint8 u8DstEndpoint;
    uint8 u8SrcEndpoint;
    uint8 u8Radius;
    uint8 eDstAddrMode;
    uint8 eTxOptions;
} ZPS_tsAplApsdeReqData;

typedef union
{
    ZPS_tsAplApsdeReqData  sReqData;
} ZPS_tuAplApsdeReqRspParam;

typedef struct
{
    uint8                 u8Type;
    uint8                 u8ParamLength;
    uint16                u16Pad;
    ZPS_tuAplApsdeReqRspParam uParam;
} ZPS_tsAplApsdeReqRsp;

typedef struct
{
    struct {
        uint32 u6Reserved       : 6;
        uint32 u2Fragmentation  : 2;
        uint32 u24Padding       : 24;
    } sEFC;
    uint8 u8BlockNum;
    uint8 u8Ack;
} zps_tsExtendedFrameControlField;

typedef union {
    struct {
        uint8   u8DstEndpoint;
        uint16  u16ClusterId;
        uint16  u16ProfileId;
        uint8   u8SrcEndpoint;
        uint8   u8ApsCounter;
    } sUnicast;

    struct {
            uint16  u16GroupAddr;
            uint16  u16ClusterId;
            uint16  u16ProfileId;
            uint8   u8SrcEndpoint;
            uint8   u8ApsCounter;
        } sGroup;
} zps_tuApsAddressingField;

typedef struct {
    uint16    *psDuplicateTableSrcAddr;
    uint32    *psDuplicateTableHash;
    uint8     *psDuplicateTableApsCnt;
    uint8     u8TableIndex;
} zps_tsApsDuplicateTable;

typedef struct zps_tsMsgRecord_tag {
    struct zps_tsMsgRecord_tag *psNext;
    ZPS_tsAplApsdeReqRsp sApsdeReqRsp;
    ZPS_tsTsvTimer sAckTimer;       /* ack timer */
    uint8       u8ReTryCnt;
    uint8       u8ApsCount;
} zps_tsMsgRecord;

typedef struct zps_tsDcfmRecord_tag{
    union {
        uint16 u16DstAddr;
        uint64 u64DstAddr;
    };
    uint8   u8Handle;
    uint8   u8SrcEp;
    uint8   u8DstEp;
    uint8   u8DstAddrMode;
    uint8   u8SeqNum;
} zps_tsDcfmRecord;

typedef struct zps_tsDcfmRecordPool_tag{
    zps_tsDcfmRecord *psDcfmRecords;
    uint8 u8NextHandle;
    uint8 u8NumRecords;
} zps_tsDcfmRecordPool;

typedef struct zps_tsFragmentTransmit_tag {
    enum {
        ZPS_FRAG_TX_STATE_IDLE,
        ZPS_FRAG_TX_STATE_SENDING,
        ZPS_FRAG_TX_STATE_RESENDING,
        ZPS_FRAG_TX_STATE_WAIT_FOR_ACK
    }eState;
    PDUM_thAPduInstance hAPduInst;
    uint16  u16DstAddress;
    uint16  u16ProfileId;
    uint16  u16ClusterId;
    uint8   u8DstEndpoint;
    uint8   u8SrcEndpoint;
    uint8   u8Radius;
    uint8   u8SeqNum;

    ZPS_tsTsvTimer sAckTimer;
    uint8   u8CurrentBlock;
    uint8   u8SentBlocksInWindow;
    uint8   u8MinBlockNumber;
    uint8   u8MaxBlockNumber;
    uint8   u8TotalBlocksToSend;
    uint8   u8RetryCount;
    uint8   u8AckedBlocksInWindow;
    uint8   u8WindowSize;
    uint8   u8BlockSize;
    bool_t  bSecure;
} zps_tsFragmentTransmit;

typedef struct zps_tsfragTxPool_tag {
    zps_tsFragmentTransmit *psFragTxRecords;
    uint8   u8NumRecords;
} zps_tsFragTxPool;

typedef struct zps_tsFragmentReceive_tag {
    enum {
        ZPS_FRAG_RX_STATE_IDLE,
        ZPS_FRAG_RX_STATE_RECEIVING,
        ZPS_FRAG_RX_STATE_PERSISTING
    }eState;
    PDUM_thAPduInstance hAPduInst;
    uint16  u16SrcAddress;
    uint16  u16ProfileId;
    uint16  u16ClusterId;
    uint8   u8DstEndpoint;
    uint8   u8SrcEndpoint;
    uint8   u8SeqNum;

    ZPS_tsTsvTimer  sWindowTimer;
    PDUM_thNPdu     hNPduPrevious;
    uint16  u16ReceivedBytes;
    uint8   u8TotalBlocksToReceive;
    uint8   u8ReceivedBlocksInWindow;
    uint8   u8MinBlockNumber;
    uint8   u8MaxBlockNumber;
    uint8   u8HighestUnAckedBlock;
    uint8   u8WindowSize;
    uint8   u8BlockSize;
    uint8   u8PreviousBlock;
} zps_tsFragmentReceive;

typedef struct zps_tsfragRxPool_tag {
    zps_tsFragmentReceive *psFragRxRecords;
    uint8   u8NumRecords;
    uint8   u8PersistanceTime;
} zps_tsFragRxPool;

typedef struct zps_tsApsPollTimer {
    ZPS_tsTsvTimer sPollTimer;
    uint16 u16PollInterval;
    uint8 u8PollActive;
} zps_tsApsPollTimer;

typedef struct zps_tsApsmeCmdContainer {
    struct zps_tsApsmeCmdContainer *psNext; /* must be first element of struct */
    ZPS_tsNwkNldeReqRsp sNldeReqRsp;
    ZPS_tsTsvTimer sTimer;
    PDUM_thNPdu hNPduCopy;
    uint8 u8Retries;
} zps_tsApsmeCmdContainer;

typedef struct {
    zps_tsApsmeCmdContainer *psFreeList;
    zps_tsApsmeCmdContainer *psSubmittedList;
} zps_tsApsmeCmdMgr;

typedef struct {
    void* pvParam;
    ZPS_tsAplApsdeDcfmIndHdr *psApsdeDcfmIndHdr;
}zps_tsLoopbackDataContext;

typedef struct {
    /* APSDE */
    void *pvParam;
    ZPS_tsAplApsdeDcfmIndHdr *(*prpsGetApsdeBuf)(void *);
    void (*prvPostApsdeDcfmInd)(void *, ZPS_tsAplApsdeDcfmIndHdr *);
    /* APSME */
    void *pvApsmeParam;
    ZPS_tsAplApsmeDcfmIndHdr *(*prpsGetApsmeBuf)(void *);
    void (*prvPostApsmeDcfmInd)(void *, ZPS_tsAplApsmeDcfmIndHdr *);

    zps_tsApsDuplicateTable *psApsDuplicateTable;
    zps_tsMsgRecord  *psSyncMsgPool;
    uint8 u8ApsDuplicateTableSize;
    uint8 u8SeqNum;
    uint8 u8SyncMsgPoolSize;
    uint8 u8MaxFragBlockSize;
    zps_tsDcfmRecordPool sDcfmRecordPool;
    zps_tsFragRxPool sFragRxPool;
    zps_tsFragTxPool sFragTxPool;
    ZPS_teStatus (*preStartFragmentTransmission)(void *, ZPS_tsAplApsdeReqRsp *, uint16, uint8);
    void (*prvHandleExtendedDataAck)(void *, ZPS_tsNwkNldeDcfmInd *, zps_tuApsAddressingField *, zps_tsExtendedFrameControlField *);
    void (*prvHandleDataFragmentReceive)(void *, ZPS_tsAplApsdeDcfmIndHdr *);
    zps_tsApsmeCmdMgr sApsmeCmdMgr;
    zps_tsApsPollTimer sApsPollTimer;
    zps_tsLoopbackDataContext sLoopbackContext;
    ZPS_tsTsvTimer sLoopbackTimer;
} zps_tsApsContext;

/*** APL Context **************************************************/

typedef struct {
    void *pvNwk;
    const void *pvNwkTableSizes;
    const void *pvNwkTables;
    
    ZPS_tsNwkNib *psNib;
    ZPS_tsAplAib *psAib;
    
    void* hZpsMutex;
    void* hDefaultStackEventMsg;
    void* hMcpsDcfmIndMsg;
    void* hMlmeDcfmIndMsg;
    void* hTimeEventMsg;
    void* hMcpsDcfmMsg;
    /* sub-layer contexts */
    zps_tsZdpContext sZdpContext;
    zps_tsZdoContext sZdoContext;
    zps_tsAfContext  sAfContext;
    zps_tsApsContext sApsContext;

    /* trust center context if present */
    zps_tsTrustCenterContext *psTrustCenterContext;

} zps_tsApl;

/*** NIB Defaults **************************************************/

typedef struct
{
    uint32 u32VsOldRouteExpiryTime;
    uint8  u8MaxRouters;
    uint8  u8MaxChildren;
    uint8  u8MaxDepth;
    uint8  u8PassiveAckTimeout;
    uint8  u8MaxBroadcastRetries;
    uint8  u8MaxSourceRoute;
    uint8  u8NetworkBroadcastDeliveryTime;
    uint8  u8UniqueAddr;
    uint8  u8AddrAlloc;
    uint8  u8UseTreeRouting;
    uint8  u8SymLink;
    uint8  u8UseMulticast;
    uint8  u8LinkStatusPeriod;
    uint8  u8RouterAgeLimit;
    uint8  u8RouteDiscoveryRetriesPermitted;
    uint8  u8VsFormEdThreshold;
    uint8  u8SecurityLevel;
    uint8  u8AllFresh;
    uint8  u8SecureAllFrames;
    uint8  u8VsTxFailThreshold;
    uint8  u8VsMaxOutgoingCost;
    uint8  u8VsLeaveRejoin;
    uint8  u8ZedTimeout;
    uint8  u8ZedTimeoutDefault;
    uint16 u16VerifyLinkCostTransmitRate; 					

} zps_tsNwkNibInitialValues;


/****************************************************************************/
/***        External Dependencies                                         ***/
/****************************************************************************/

PUBLIC ZPS_teStatus zps_eStartFragmentedTransmission(void *, ZPS_tsAplApsdeReqRsp *, uint16 , uint8);
PUBLIC void zps_vHandleExtendedDataAckFrame(void *, ZPS_tsNwkNldeDcfmInd *, zps_tuApsAddressingField *, zps_tsExtendedFrameControlField * );
PUBLIC void zps_vHandleApsdeDataFragIndNotSupported(void *pvApl, ZPS_tsAplApsdeDcfmIndHdr *);
PUBLIC void zps_vHandleApsdeDataFragInd(void *, ZPS_tsAplApsdeDcfmIndHdr *psApsdeDcfmInd);
bool_t g_pbZpsMutex = FALSE;
PUBLIC void* zps_vGetZpsMutex(void);
extern PUBLIC bool_t APP_bMultimaskJoinCallBack(void *);
PRIVATE  bool_t    g_bIgnoreBroadcast [ 1 ] =  {TRUE,};
PRIVATE  uint32    g_u32MacTxUcastAvgRetry [ 1 ] =  {1,};
PRIVATE  uint32    g_u32MacTxUcastAccRetry [ 1 ]; 
PRIVATE  uint32    g_u32MacTxUcastFail [ 1 ]; 
PRIVATE  uint32    g_u32MacTxUcast [ 1 ]; 
PRIVATE  uint32    g_u32MacCcaFail [ 1 ]; 
PRIVATE  uint32    g_u32ApsRetry   [ 1 ]; 
PUBLIC   uint32    g_u32ChannelMaskList [ 1 ] = {     0x07fff800UL,
}; 
zps_tsAplAfMMServerContext	s_sMultiMaskServer ={     ZPS_E_MULTIMASK_STATE_IDLE,     1,     0,     APP_bMultimaskJoinCallBack};
    /* ... ZPS_MULTIMASK_SUPPORT */
    /* The MAC Interface Table (default values) */ 
PRIVATE  MAC_tsMacInterface    g_sMacInterface [ 1 ]= 
{
        { 0, 0x7, E_MAC_FREQ_2400, E_MAC_TYPE_SOC  } , 
};
PRIVATE MAC_tsMacInterfaceTable g_asMacInterfaceTable =  
{ 
  &s_sMultiMaskServer,		 /* ZPS_MULTIMASK_SUPPORT ...*/ 
  g_sMacInterface,
  &g_u32ChannelMaskList [0],
  g_u32MacTxUcastAvgRetry,
  g_u32MacTxUcastAccRetry,
  g_u32MacTxUcastFail,
  g_u32MacTxUcast,
  g_u32MacCcaFail,
  g_u32ApsRetry,
  g_bIgnoreBroadcast,
  1
};
PUBLIC uint8 u8MaxZpsConfigEp = 1 ;
PUBLIC uint8 au8EpMapPresent[1] = { 1  }; 
PUBLIC uint8 u8ZpsConfigStackProfileId = 2;
PUBLIC const uint32 g_u32ApsFcSaveCountBitShift = 4;
PUBLIC const uint32 g_u32NwkFcSaveCountBitShift = 4;
ZPS_tsAfZgpGreenPowerContext *g_psGreenPowerContext = NULL;

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

PUBLIC void vZdoServersInit(void);
PUBLIC bool zps_bAplZdoDefaultServer(void *, void *, ZPS_tsAfEvent *);
PUBLIC void zps_vAplZdoDefaultServerInit(void *, PDUM_thAPdu );
PUBLIC bool zps_bAplZdoZdoClient(void *, void *, ZPS_tsAfEvent *);
PUBLIC void zps_vAplZdoZdoClientInit(void *, PDUM_thAPdu );
PUBLIC bool zps_bAplZdoDeviceAnnceServer(void *, void *, ZPS_tsAfEvent *);
PUBLIC void zps_vAplZdoDeviceAnnceServerInit(void *);
PUBLIC bool zps_bAplZdoActiveEpServer(void *, void *, ZPS_tsAfEvent *);
PUBLIC void zps_vAplZdoActiveEpServerInit(void *, PDUM_thAPdu );
PUBLIC bool zps_bAplZdoNwkAddrServer(void *, void *, ZPS_tsAfEvent *);
PUBLIC void zps_vAplZdoNwkAddrServerInit(void *, PDUM_thAPdu );
PUBLIC bool zps_bAplZdoIeeeAddrServer(void *, void *, ZPS_tsAfEvent *);
PUBLIC void zps_vAplZdoIeeeAddrServerInit(void *, PDUM_thAPdu );
PUBLIC bool zps_bAplZdoSystemServerDiscoveryServer(void *, void *, ZPS_tsAfEvent *);
PUBLIC void zps_vAplZdoSystemServerDiscoveryServerInit(void *, PDUM_thAPdu );
PUBLIC bool zps_bAplZdoNodeDescServer(void *, void *, ZPS_tsAfEvent *);
PUBLIC void zps_vAplZdoNodeDescServerInit(void *, PDUM_thAPdu );
PUBLIC bool zps_bAplZdoPowerDescServer(void *, void *, ZPS_tsAfEvent *);
PUBLIC void zps_vAplZdoPowerDescServerInit(void *, PDUM_thAPdu );
PUBLIC bool zps_bAplZdoMatchDescServer(void *, void *, ZPS_tsAfEvent *);
PUBLIC void zps_vAplZdoMatchDescServerInit(void *, PDUM_thAPdu );
PUBLIC bool zps_bAplZdoSimpleDescServer(void *, void *, ZPS_tsAfEvent *);
PUBLIC void zps_vAplZdoSimpleDescServerInit(void *, PDUM_thAPdu );
PUBLIC bool zps_bAplZdoMgmtLqiServer(void *, void *, ZPS_tsAfEvent *);
PUBLIC void zps_vAplZdoMgmtLqiServerInit(void *, PDUM_thAPdu );
PUBLIC bool zps_bAplZdoMgmtLeaveServer(void *, void *, ZPS_tsAfEvent *);
PUBLIC void zps_vAplZdoMgmtLeaveServerInit(void *, PDUM_thAPdu );
PUBLIC bool zps_bAplZdoMgmtNWKUpdateServer(void *, void *, ZPS_tsAfEvent *);
PUBLIC void zps_vAplZdoMgmtNWKUpdateServerInit(void *, PDUM_thAPdu , void *);
PUBLIC bool zps_bAplZdoBindUnbindServer(void *, void *, ZPS_tsAfEvent *);
PUBLIC void zps_vAplZdoBindUnbindServerInit(void *, PDUM_thAPdu );
PUBLIC bool zps_bAplZdoBindRequestServer(void *, void *, ZPS_tsAfEvent *);
PUBLIC void zps_vAplZdoBindRequestServerInit(void *, uint8, uint8, zps_tsZdoServerConfAckContext* );
PUBLIC bool zps_bAplZdoMgmtBindServer(void *, void *, ZPS_tsAfEvent *);
PUBLIC void zps_vAplZdoMgmtBindServerInit(void *, PDUM_thAPdu );

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

PRIVATE ZPS_tsAplApsmeBindingTableStoreEntry s_bindingTableStorage[16];
PRIVATE ZPS_tsAplApsmeBindingTable s_bindingTable = { s_bindingTableStorage, 16 };
PRIVATE ZPS_tsAplApsmeBindingTableType s_bindingTables = { NULL, &s_bindingTable };
PRIVATE ZPS_tsAplApsmeGroupTableEntry s_groupTableStorage[16];
PRIVATE ZPS_tsAplApsmeAIBGroupTable s_groupTable = { s_groupTableStorage, 16 };
PRIVATE ZPS_tsAPdmGroupTableEntry s_groupTablePdmStorage[16];
PUBLIC ZPS_tsPdmGroupTable s_groupPdmTable = { s_groupTablePdmStorage, 16 };
PRIVATE ZPS_tsAplApsKeyDescriptorEntry s_keyPairTableStorage[4] = {
    { 0, 0xFFFF, 0, { }  },
    { 0, 0xFFFF, 0, { }  },
    { 0, 0xFFFF, 0, { }  },
    { 0, 0xFFFF, 0, { }  },
};
ZPS_tsAplApsKeyDescriptorEntry  *psAplDefaultDistributedAPSLinkKey = &s_keyPairTableStorage[2];
ZPS_tsAplApsKeyDescriptorEntry  *psAplDefaultGlobalAPSLinkKey = &s_keyPairTableStorage[3];
PRIVATE uint32 au32IncomingFrameCounter[4];
PRIVATE ZPS_tsAplApsKeyDescriptorTable s_keyPairTable = { s_keyPairTableStorage, 1 };

PRIVATE ZPS_tsAplAib s_sAplAib = {
    0,
    0x0000000000000000ULL,
    FALSE,
    FALSE,
    FALSE,
    0,
    0x02,
    0x0a,
    0,
    0,
    0,
    0x08,
    &s_bindingTables,
    &s_groupTable,
    &s_keyPairTable,
    &s_keyPairTableStorage[1],
    FALSE,
    FALSE,
    0x1770,
    au32IncomingFrameCounter,
    g_u32ChannelMaskList
};
PRIVATE uint8 s_sDefaultServerContext[4] __attribute__ ((aligned (4)));
PRIVATE uint8 s_sZdoClientContext[8] __attribute__ ((aligned (4)));
PRIVATE uint8 s_sDeviceAnnceServerContext[0] __attribute__ ((aligned (4)));
PRIVATE uint8 s_sActiveEpServerContext[4] __attribute__ ((aligned (4)));
PRIVATE uint8 s_sNwkAddrServerContext[4] __attribute__ ((aligned (4)));
PRIVATE uint8 s_sIeeeAddrServerContext[4] __attribute__ ((aligned (4)));
PRIVATE uint8 s_sSystemServerDiscoveryServerContext[4] __attribute__ ((aligned (4)));
PRIVATE uint8 s_sNodeDescServerContext[4] __attribute__ ((aligned (4)));
PRIVATE uint8 s_sPowerDescServerContext[4] __attribute__ ((aligned (4)));
PRIVATE uint8 s_sMatchDescServerContext[4] __attribute__ ((aligned (4)));
PRIVATE uint8 s_sSimpleDescServerContext[4] __attribute__ ((aligned (4)));
PRIVATE uint8 s_sMgmtLqiServerContext[4] __attribute__ ((aligned (4)));
PRIVATE uint8 s_sMgmtLeaveServerContext[40] __attribute__ ((aligned (4)));
PRIVATE uint8 s_sMgmtNWKUpdateServerContext[88] __attribute__ ((aligned (4)));
PRIVATE uint8 s_sBindUnbindServerContext[48] __attribute__ ((aligned (4)));
PRIVATE uint8 s_sBindRequestServerContext[84] __attribute__ ((aligned (4)));
PRIVATE zps_tsZdoServerConfAckContext s_sBindRequestServerAcksDcfmContext[10];
PRIVATE uint8 s_sMgmtBindServerContext[4] __attribute__ ((aligned (4)));

/* ZDO Servers */
PRIVATE const zps_tsAplZdoServer s_asAplZdoServers[18] = {
    { zps_bAplZdoZdoClient, s_sZdoClientContext },
    { zps_bAplZdoDeviceAnnceServer, s_sDeviceAnnceServerContext },
    { zps_bAplZdoActiveEpServer, s_sActiveEpServerContext },
    { zps_bAplZdoNwkAddrServer, s_sNwkAddrServerContext },
    { zps_bAplZdoIeeeAddrServer, s_sIeeeAddrServerContext },
    { zps_bAplZdoSystemServerDiscoveryServer, s_sSystemServerDiscoveryServerContext },
    { zps_bAplZdoNodeDescServer, s_sNodeDescServerContext },
    { zps_bAplZdoPowerDescServer, s_sPowerDescServerContext },
    { zps_bAplZdoMatchDescServer, s_sMatchDescServerContext },
    { zps_bAplZdoSimpleDescServer, s_sSimpleDescServerContext },
    { zps_bAplZdoMgmtLqiServer, s_sMgmtLqiServerContext },
    { zps_bAplZdoMgmtLeaveServer, s_sMgmtLeaveServerContext },
    { zps_bAplZdoMgmtNWKUpdateServer, s_sMgmtNWKUpdateServerContext },
    { zps_bAplZdoBindUnbindServer, s_sBindUnbindServerContext },
    { zps_bAplZdoBindRequestServer, s_sBindRequestServerContext },
    { zps_bAplZdoMgmtBindServer, s_sMgmtBindServerContext },
    { zps_bAplZdoDefaultServer, s_sDefaultServerContext },
    { NULL, NULL }
};

/* Simple Descriptors */
PRIVATE const uint16 s_au16Endpoint0InputClusterList[83] = { 0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001a, 0x001b, 0x001c, 0x001d, 0x001e, 0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002a, 0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x8000, 0x8001, 0x8002, 0x8003, 0x8004, 0x8005, 0x8006, 0x8010, 0x8011, 0x8012, 0x8014, 0x8015, 0x8016, 0x8017, 0x8018, 0x8019, 0x801a, 0x801b, 0x801c, 0x801d, 0x801e, 0x8020, 0x8021, 0x8022, 0x8023, 0x8024, 0x8025, 0x8026, 0x8027, 0x8028, 0x8029, 0x802a, 0x8030, 0x8031, 0x8032, 0x8033, 0x8034, 0x8035, 0x8036, 0x8037, 0x8038, };
PRIVATE const PDUM_thAPdu s_ahEndpoint0InputClusterAPdus[83] = { apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, apduZDP, };
PRIVATE uint8 s_au8Endpoint0InputClusterDiscFlags[11] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

PRIVATE const uint16 s_au16Endpoint0OutputClusterList[83] = { 0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001a, 0x001b, 0x001c, 0x001d, 0x001e, 0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002a, 0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x8000, 0x8001, 0x8002, 0x8003, 0x8004, 0x8005, 0x8006, 0x8010, 0x8011, 0x8012, 0x8014, 0x8015, 0x8016, 0x8017, 0x8018, 0x8019, 0x801a, 0x801b, 0x801c, 0x801d, 0x801e, 0x8020, 0x8021, 0x8022, 0x8023, 0x8024, 0x8025, 0x8026, 0x8027, 0x8028, 0x8029, 0x802a, 0x8030, 0x8031, 0x8032, 0x8033, 0x8034, 0x8035, 0x8036, 0x8037, 0x8038, };
PRIVATE uint8 s_au8Endpoint0OutputClusterDiscFlags[11] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

PRIVATE const uint16 s_au16Endpoint1InputClusterList[6] = { 0x0000, 0x0004, 0x0003, 0x0006, 0xffff, 0x0019, };
PRIVATE const PDUM_thAPdu s_ahEndpoint1InputClusterAPdus[6] = { apduZCL, apduZCL, apduZCL, apduZCL, apduZCL, apduZCL, };
PRIVATE uint8 s_au8Endpoint1InputClusterDiscFlags[1] = { 0x0f };

PRIVATE const uint16 s_au16Endpoint1OutputClusterList[5] = { 0x0000, 0x0004, 0x0003, 0x0006, 0x0019, };
PRIVATE uint8 s_au8Endpoint1OutputClusterDiscFlags[1] = { 0x1f };

PUBLIC void APP_vGenCallback(uint8 u8Endpoint, ZPS_tsAfEvent *psStackEvent);
tszQueue zps_msgMlmeDcfmInd;
tszQueue zps_msgMcpsDcfmInd;
tszQueue zps_TimeEvents;
tszQueue zps_msgMcpsDcfm;
PRIVATE zps_tsAplAfSimpleDescCont s_asSimpleDescConts[2] = {
    {
        {
            0x0000,
            0,
            0,
            0,
            83,
            83,
            s_au16Endpoint0InputClusterList,
            s_au16Endpoint0OutputClusterList,
            s_au8Endpoint0InputClusterDiscFlags,
            s_au8Endpoint0OutputClusterDiscFlags,
        },
        s_ahEndpoint0InputClusterAPdus,
        1
    },
    {
        {
            0x0104,
            256,
            1,
            1,
            6,
            5,
            s_au16Endpoint1InputClusterList,
            s_au16Endpoint1OutputClusterList,
            s_au8Endpoint1InputClusterDiscFlags,
            s_au8Endpoint1OutputClusterDiscFlags,
        },
        s_ahEndpoint1InputClusterAPdus,
        1
    },
};

/* Node Descriptor */
PRIVATE ZPS_tsAplAfNodeDescriptor s_sNodeDescriptor = {
    2,
    FALSE,
    FALSE,
    0,
    0x08,
    0,
    0x80,
    0x1037,
    0x7f,
    0x0064,
    0x2c00,
    0x0064,
    0x00};

/* Node Power Descriptor */
PRIVATE ZPS_tsAplAfNodePowerDescriptor s_sNodePowerDescriptor = {
    0xC,   
    0x4,
    0x4,
    0x0};

/* APSDE duplicate table */
PRIVATE uint16 s_au16ApsDuplicateTableAddrs[3];
PRIVATE uint32 s_au32ApsDuplicateTableHash[3];
PRIVATE uint8 s_au8ApsDuplicateTableSeqCnts[3];
PRIVATE zps_tsApsDuplicateTable s_sApsDuplicateTable = { s_au16ApsDuplicateTableAddrs, s_au32ApsDuplicateTableHash, s_au8ApsDuplicateTableSeqCnts, 0 };

/* APSDE sync msg pool */
PRIVATE zps_tsMsgRecord s_asApsSyncMsgPool[4];

/* APSDE dcfm record pool */
PRIVATE zps_tsDcfmRecord s_asApsDcfmRecordPool[4];

/* APSME Command Manager Command Containers */
PRIVATE zps_tsApsmeCmdContainer s_sApsmeCmdContainer_4 = { NULL, {}, {}, NULL, 0 };
PRIVATE zps_tsApsmeCmdContainer s_sApsmeCmdContainer_3 = { &s_sApsmeCmdContainer_4, {}, {}, NULL, 0 };
PRIVATE zps_tsApsmeCmdContainer s_sApsmeCmdContainer_2 = { &s_sApsmeCmdContainer_3, {}, {}, NULL, 0 };
PRIVATE zps_tsApsmeCmdContainer s_sApsmeCmdContainer_1 = { &s_sApsmeCmdContainer_2, {}, {}, NULL, 0 };

/* Network Layer Context */
PRIVATE uint8                   s_sNwkContext[1904] __attribute__ ((aligned (4)));
PRIVATE ZPS_tsNwkDiscNtEntry    s_asNwkNtDisc[16];
PRIVATE ZPS_tsNwkActvNtEntry    s_asNwkNtActv[2];
PRIVATE ZPS_tsNwkBtr            s_asNwkBtt[9];
PRIVATE ZPS_tsNwkSecMaterialSet s_asNwkSecMatSet[2];
PRIVATE uint32                  s_asNwkInFCSet[2];
PRIVATE uint16                  s_au16NwkAddrMapNwk[20];
PRIVATE uint16                  s_au16NwkAddrMapLookup[20];
PRIVATE uint64                  s_au64NwkAddrMapExt[20];
#ifdef ZPS_FRQAG
PRIVATE uint32                  s_au32RxPacketCount[2];
PRIVATE uint32                  s_au32TxPacketCount[2];
#endif
PRIVATE uint32                  s_au32ZedTimeoutCount[2];
PRIVATE uint8                  s_au8KeepAliveFlags[2];

PRIVATE const zps_tsNwkNibInitialValues s_sNibInitialValues =
{
    600,
    05,
    7,
    15,
    1,
    2,
    11,
    18,
    0,
    2,
    0,
    1,
    0,
    15,
    3,
    3,
    255,
    5,
    TRUE,
    TRUE,
    5,
    4,
    0,
    6,
    8
    ,0/* u16VerifyLinkCostTransmitRate */
};


PRIVATE const ZPS_tsNwkNibTblSize     s_sNwkTblSize = {
    2,
    0,
    0,
    16,
    16,
    0,
    9,
    2,
    sizeof(s_sNibInitialValues),
    2,
    16
};

PRIVATE const ZPS_tsNwkNibTbl s_sNwkTbl = {
    s_asNwkNtDisc,
    s_asNwkNtActv,
    NULL,
    NULL,
    s_asNwkBtt,
    NULL,
    s_asNwkSecMatSet,
    (ZPS_tsNwkNibInitialValues*)&s_sNibInitialValues,
    s_au16NwkAddrMapNwk,
    s_au16NwkAddrMapLookup,
    s_asNwkInFCSet,
#if (defined JENNIC_CHIP_FAMILY_JN516x) || (defined JENNIC_CHIP_FAMILY_JN517x)
    0,
#endif
    s_au64NwkAddrMapExt,
    s_au32ZedTimeoutCount,
    s_au8KeepAliveFlags,
#ifdef ZPS_FRQAG
    s_au32RxPacketCount,
    s_au32TxPacketCount,
#else
    NULL,
    NULL,
#endif    
};

/* Application Layer Context */
#ifdef WWAH_SUPPORT
PRIVATE ZPS_tsAplAibExtensions s_sAibExtension = { 
  0  };

const void *zps_g_pvAib_extensions = &s_sAibExtension;
#endif
PRIVATE zps_tsRequestKeyRequests s_asRequestKeyRequests[1];
PRIVATE zps_tsApl s_sApl = {
    s_sNwkContext,
    &s_sNwkTblSize,
    &s_sNwkTbl,
    NULL,
    &s_sAplAib,
    zps_vGetZpsMutex,
    &APP_vGenCallback,
    &zps_msgMcpsDcfmInd,
    &zps_msgMlmeDcfmInd,
    &zps_TimeEvents,
    &zps_msgMcpsDcfm,
    { 0 },
    {
         { 0x1B, 0x19, 0x4A },
        0,
        ZPS_ZDO_DEVICE_ENDDEVICE,
        ZPS_ZDO_PRECONFIGURED_LINK_KEY,
        0x00,
        2,
        2,
        3,
        FALSE,
        s_asAplZdoServers,
        vZdoServersInit,
        { /* timer struct */},
        { /* timer struct */},
        0,
        3,
        0,
        NULL,
        0,
        NULL,
        NULL,
        s_asRequestKeyRequests,
        937500,
        1
    },
    {
        NULL,
        &s_sNodeDescriptor,
        &s_sNodePowerDescriptor,
        2,
        s_asSimpleDescConts,
        NULL,
        NULL,
        0xff,
        0x00
    },
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        &s_sApsDuplicateTable,
        s_asApsSyncMsgPool,
        0x03,
        0,
        4,
        0,
        { s_asApsDcfmRecordPool, 1, 4 },
        { NULL, 0, 0 },
        { NULL, 0 },
        NULL,
        NULL,
        zps_vHandleApsdeDataFragIndNotSupported,
        { &s_sApsmeCmdContainer_1, NULL },
        { { /* Timer */}, 200, 0 },
        { NULL, NULL },
        { /* Timer */}
    },
    NULL
};

const void *zps_g_pvApl = &s_sApl;

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/
PUBLIC void *ZPS_pvAplZdoGetAplHandle(void)
{
    return (void*)zps_g_pvApl;
}

PUBLIC uint32 ZPS_u32ApsFcSaveCountBitShift(void)
{
    return g_u32ApsFcSaveCountBitShift;
}

PUBLIC ZPS_tsAplApsKeyDescriptorEntry** ZPS_psAplDefaultDistributedAPSLinkKey(void)
{
    return &psAplDefaultDistributedAPSLinkKey;
}

PUBLIC ZPS_tsAplApsKeyDescriptorEntry** ZPS_psAplDefaultGlobalAPSLinkKey(void)
{
    return &psAplDefaultGlobalAPSLinkKey;
}

PUBLIC ZPS_tsPdmGroupTable* ZPS_ps_groupPdmTable(void)
{
    return &s_groupPdmTable;
}

PUBLIC uint8* ZPS_pu8MaxZpsConfigEp(void) 
{
    return &u8MaxZpsConfigEp;
}

PUBLIC uint8* ZPS_pau8EpMapPresent(void)
{
    return &au8EpMapPresent[0];
}

PUBLIC uint8 ZPS_u8ConfigStackProfileId(void)
{
    return u8ZpsConfigStackProfileId;
}

PUBLIC uint32 ZPS_u32NwkFcSaveCountBitShift(void)
{
    return g_u32NwkFcSaveCountBitShift;
}

/****************************************************************************
 *
 * NAME: ZPS_psMacIFTGetTable
 *
 * DESCRIPTION:
 * Obtain the pointer to the Mac inteface table 
 *
 * PARAMETERS 			Name  	   		RW  	Usage 
 *
 * RETURNS:
 * Address of Mac interface table if successful, NULL otherwise 
 *
 ****************************************************************************/ 
 PUBLIC MAC_tsMacInterfaceTable* ZPS_psMacIFTGetTable(void) 
 {
	 return &g_asMacInterfaceTable; 
 }

/****************************************************************************
 *
 * NAME: ZPS_psMacIFTGetInterface 
 *
 * DESCRIPTION:
 * Get the Mac interface entry from the MAC interface table for the specified
 * Mac ID
 *
 * PARAMETERS 	Name  		RW  	Usage
 *				u8MacID		R		The Mac Id for the interface
 * RETURNS:
 * Address of Mac interface structure if found, NULL otherwise
 *
 ****************************************************************************/ 
 PUBLIC MAC_tsMacInterface* ZPS_psMacIFTGetInterface(uint8 u8MacID)
 { 
	 MAC_tsMacInterface *pRet = NULL;
 	 if(u8MacID < g_asMacInterfaceTable.u8NumInterfaces) 
 	 {
 		 pRet = &g_asMacInterfaceTable.psMacInterfaces[u8MacID];
 	 }
 	 return pRet;
 }

/* ZDO Server Initialisation */
PUBLIC void vZdoServersInit(void)
{
    zps_vAplZdoDefaultServerInit(&s_sDefaultServerContext, apduZDP);
    zps_vAplZdoZdoClientInit(&s_sZdoClientContext, apduZDP);
    zps_vAplZdoDeviceAnnceServerInit(&s_sDeviceAnnceServerContext);
    zps_vAplZdoActiveEpServerInit(&s_sActiveEpServerContext, apduZDP);
    zps_vAplZdoNwkAddrServerInit(&s_sNwkAddrServerContext, apduZDP);
    zps_vAplZdoIeeeAddrServerInit(&s_sIeeeAddrServerContext, apduZDP);
    zps_vAplZdoSystemServerDiscoveryServerInit(&s_sSystemServerDiscoveryServerContext, apduZDP);
    zps_vAplZdoNodeDescServerInit(&s_sNodeDescServerContext, apduZDP);
    zps_vAplZdoPowerDescServerInit(&s_sPowerDescServerContext, apduZDP);
    zps_vAplZdoMatchDescServerInit(&s_sMatchDescServerContext, apduZDP);
    zps_vAplZdoSimpleDescServerInit(&s_sSimpleDescServerContext, apduZDP);
    zps_vAplZdoMgmtLqiServerInit(&s_sMgmtLqiServerContext, apduZDP);
    zps_vAplZdoMgmtLeaveServerInit(&s_sMgmtLeaveServerContext, apduZDP);
    zps_vAplZdoMgmtNWKUpdateServerInit(&s_sMgmtNWKUpdateServerContext, apduZDP, &s_sApl);
    zps_vAplZdoBindUnbindServerInit(&s_sBindUnbindServerContext, apduZDP);
    zps_vAplZdoBindRequestServerInit(&s_sBindRequestServerContext, 1, 10, s_sBindRequestServerAcksDcfmContext);
    zps_vAplZdoMgmtBindServerInit(&s_sMgmtBindServerContext, apduZDP);
}

PUBLIC void* ZPS_vGetGpContext(void)
{

    return g_psGreenPowerContext;
}

PUBLIC void* zps_vGetZpsMutex(void)
{

    return &g_pbZpsMutex;
}


PUBLIC void ZPS_vGetOptionalFeatures(void)
{
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
