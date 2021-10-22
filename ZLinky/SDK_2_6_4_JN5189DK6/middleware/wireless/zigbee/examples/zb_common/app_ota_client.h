/*
* Copyright 2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


#ifndef APP_OTA_CLIENT_H
#define APP_OTA_CLIENT_H

#include "OTA.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef OTA_DEMO_TIMINGS
#define OTA_INIT_TIME_MS RND_u32GetRand(  2000,    7000) /* Time to wait after initialisation */
#define OTA_BUSY_TIME_MS RND_u32GetRand(  2000,    3000) /* Time to wait when OTA state machine is busy */
#define OTA_IDLE_TIME_MS RND_u32GetRand(900000, 1800000) /* Time to wait when OTA state machine is idle (15-30m) */
#else
#define OTA_INIT_TIME_MS RND_u32GetRand(  2000,    7000) /* Time to wait after initialisation */
#define OTA_BUSY_TIME_MS RND_u32GetRand(  2000,    3000) /* Time to wait when OTA state machine is busy */
#define OTA_IDLE_TIME_MS RND_u32GetRand(30000,  60000) /* Time to wait when OTA state machine is idle (30s-1m) */
#endif

#define MAX_SERVER_EPs 2
#define MAX_SERVER_NODES 2
#define IGNORE_COORDINATOR_AS_OTA_SERVER
#define APP_OTA_OFFSET_WRITEN_BEFORE_LINKKEY_VERIFICATION 1100

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef struct {
    bool_t bValid;
    uint64_t u64IeeeAddrOfServer;
    uint16_t u16NwkAddrOfServer;
    uint8_t u8MatchLength;
    uint8_t u8MatchList[MAX_SERVER_EPs];
}tsDiscovedOTAServers;

typedef struct {
    bool_t        bValid;
    uint64_t      u64IeeeAddrOfServer;
    uint16_t    u16NwkAddrOfServer;
    uint8_t       u8OTAserverEP;
}tsPdmOtaApp;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
void   vAppInitOTA(void);
void   vRunAppOTAStateMachine(uint32_t u32TimeMs);
bool_t bOTADeepSleepAllowed(void);
bool_t bOTASleepAllowed(void);
void   vHandleMatchDescriptor( ZPS_tsAfEvent  * psStackEvent);
void   vHandleIeeeAddressRsp( ZPS_tsAfEvent  * psStackEvent);
void   vHandleAppOtaClient(tsOTA_CallBackMessage *psCallBackMessage);
void   vLoadOTAPersistedData(void);
void   vSetOTAPersistedDatForMinRetention(void);
void   vOTAResetPersist(void);
void   vResetOTADiscovery(void);
bool_t   bInitialiseOTAClusterAndAttributes (void);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/
#if (defined SLEEP_MIN_RETENTION) && (defined CLD_OTA) && (defined OTA_CLIENT)
extern uint32_t U32UTCTimeBeforeSleep;
#endif

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

#endif /*APP_OTA_CLIENT_H*/
