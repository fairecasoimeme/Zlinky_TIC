/*****************************************************************************
 *
 * MODULE:             JN-AN-1244
 *
 * COMPONENT:          app_ota_client.h
 *
 * DESCRIPTION:        OTA Client Implementation
 *
 ****************************************************************************
 *
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5168, JN5169,
 * JN5179, JN5189].
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
 * Copyright NXP B.V. 2016-2018. All rights reserved
 *
 ***************************************************************************/

#ifndef APP_OTA_CLIENT_H
#define APP_OTA_CLIENT_H

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifndef OTA_DEMO_TIMINGS
#define OTA_INIT_TIME_MS 60000                           /* Time to wait after initialisation */
#define OTA_BUSY_TIME_MS RND_u32GetRand(  2000,    3000) /* Time to wait when OTA state machine is busy */
#define OTA_IDLE_TIME_MS RND_u32GetRand(900000, 1800000) /* Time to wait when OTA state machine is idle (15-30m) */
#else
#define OTA_INIT_TIME_MS RND_u32GetRand( 2000,  7000) /* Time to wait after initialisation */
#define OTA_BUSY_TIME_MS RND_u32GetRand( 2000,  3000) /* Time to wait when OTA state machine is busy */
#define OTA_IDLE_TIME_MS RND_u32GetRand( 45000, 75000) /* Time to wait when OTA state machine is idle (65-90s) */
#endif

#define MAX_SERVER_EPs 2
#define MAX_SERVER_NODES 2
#define IGNORE_COORDINATOR_AS_OTA_SERVER
#define APP_OTA_OFFSET_WRITEN_BEFORE_LINKKEY_VERIFICATION 1100

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef struct {
    bool bValid;
    uint64 u64IeeeAddrOfServer;
    uint16 u16NwkAddrOfServer;
    uint8 u8MatchLength;
    uint8 u8MatchList[MAX_SERVER_EPs];
}tsDiscovedOTAServers;


typedef enum {
    OTA_IDLE,
    OTA_FIND_SERVER,
    OTA_FIND_SERVER_WAIT,
    OTA_IEEE_LOOK_UP,
    OTA_IEEE_WAIT,
    OTA_QUERYIMAGE,
    OTA_QUERYIMAGE_WAIT,
    OTA_DL_PROGRESS
} teOTA_State;
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC void vAppInitOTA(void);
PUBLIC void vRunAppOTAStateMachine(uint32 u32TimeMs);
PUBLIC bool_t bOTADeepSleepAllowed(void);
PUBLIC void vHandleMatchDescriptor( ZPS_tsAfEvent  * psStackEvent);
PUBLIC void vHandleIeeeAddressRsp( ZPS_tsAfEvent  * psStackEvent);
PUBLIC void vHandleAppOtaClient(tsOTA_CallBackMessage *psCallBackMessage);
PUBLIC void vLoadOTAPersistedData(void);
PUBLIC void vOTAResetPersist(void);
PUBLIC teOTA_State eOTA_GetState(void);
PUBLIC bool_t bOTA_IsWaitToUpgrade(void);
/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

#endif /*APP_OTA_CLIENT_H*/
