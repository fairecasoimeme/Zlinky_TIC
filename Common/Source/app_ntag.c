/*****************************************************************************
 *
 * MODULE:          JN-AN-1243 Base Device Application
 *
 * COMPONENT:       app_ntag.c
 *
 * DESCRIPTION:     Application layer for NTAG (simple test)
 *
 * This file is INCOMPLETE and EXPERIMENTAL.
 *
 * It is a work in progress to use the NTAG blocking functions for commissioning
 * and ota. So far it only reads and validates the commissioning data.
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
 * Copyright NXP B.V. 2017-2018. All rights reserved
 *
 ***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "dbg.h"
#include "ZTimer.h"
#include "PDM.h"
#include "bdb_DeviceCommissioning.h"
#include "nfc.h"
//#include "nfc_end.h"
#include "ntag.h"
//#include "ntag_nwk.h"
#include "nfc_nwk.h"
#include "nfc_ota.h"
#include "app_ntag.h"
#include "app_main.h"
#include "app_buttons.h"
#include "PDM_IDs.h"
#ifdef LITTLE_ENDIAN_PROCESSOR
    #include "portmacro.h"
#endif
#if (JENNIC_CHIP_FAMILY == JN518x)
    #include "fsl_rng.h"
    #include "fsl_reset.h"
    #include "fsl_os_abstraction.h"
    #include "fsl_flash.h"
    #include "fsl_wwdt.h"
    #include "rom_psector.h"
#endif
#include "MicroSpecific.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifdef DEBUG_APP_NTAG
    #define TRACE_APP_NTAG         TRUE
    #define TRACE_APP_NTAG_DATA    TRUE
    #define TRACE_APP_NTAG_VERBOSE FALSE
#else
    #define TRACE_APP_NTAG         FALSE
    #define TRACE_APP_NTAG_DATA    FALSE
    #define TRACE_APP_NTAG_VERBOSE FALSE
#endif

#define NTAG_NWK_DATA_SIZE   (8 * NTAG_BLOCK_SIZE)
#define NTAG_OTA_DATA_SIZE   (NFC_SRAM_SIZE)
#define NTAG_DATA_SIZE        MAX(NTAG_NWK_DATA_SIZE, NTAG_OTA_DATA_SIZE)
#define NTAG_TIMER_MIN_MS     1
#define NTAG_TIMER_MAX_MS     5
#define NTAG_TIMER_OTA_MS     5
#define FLASH_INACTIVE_START (0x48000)
#define FLASH_PAGE_SIZE       512
#define FLASH_PDM_SAVE       (10 * FLASH_PAGE_SIZE) /* 10 flash pages = 5kb */
#if ((defined APP_NTAG_OTA) && (!defined BUILD_OTA))
#define OTA_MAX_HEADER_SIZE                       (uint8)69
#endif
#define NFC_CMD_NWK_NCI_JOIN_WITH_KEY 0xA2

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/* APP_NTAG States */
typedef enum
{
    E_APP_NTAG_STATE_INIT_READ,
    E_APP_NTAG_STATE_HEADER_WRITE,
    E_APP_NTAG_STATE_CONFIG_WRITE,
    E_APP_NTAG_STATE_IDLE,
#if (defined APP_NTAG_NWK)
    E_APP_NTAG_NWK_STATE_READ,
    E_APP_NTAG_NWK_STATE_WRITE,
#endif
#if (defined APP_NTAG_OTA)
    E_APP_NTAG_OTA_STATE_WRITE,
    E_APP_NTAG_OTA_STATE_POLL,
    E_APP_NTAG_OTA_STATE_READ,
#endif
    E_APP_NTAG_STATE_ERROR
} teAppNtagState;

/* APP_NTAG NCI States */
typedef enum
{
    E_APP_NTAG_NCI_STATE_NONE,              // 0
    E_APP_NTAG_NCI_STATE_ABSENT,            // 1
    E_APP_NTAG_NCI_STATE_PRESENT            // 2
} teAppNtagNciState;

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE bool_t APP_bNtagStateInitRead         (uint8 *pu8TimerMs);
PRIVATE bool_t APP_bNtagStateHeaderWrite      (uint8 *pu8TimerMs);
PRIVATE bool_t APP_bNtagStateConfigWrite      (uint8 *pu8TimerMs);
PRIVATE bool_t APP_bNtagStateIdle             (uint8 *pu8TimerMs);
PRIVATE bool_t APP_bNtagCcValid               (uint8 *pu8Data);
PRIVATE void   APP_vNtagCcSet                 (uint8 *pu8Data);
PRIVATE bool_t APP_bNtagConfigValid           (uint8 *pu8Data);
PRIVATE void   APP_vNtagConfigSet             (uint8 *pu8Data);
#if (defined APP_NTAG_NWK)
PRIVATE void   APP_vNtagNwkPdmLoadInstallCode (void);
PRIVATE bool_t APP_bNtagNwkPdmLoadNci         (void);
PRIVATE bool_t APP_bNtagNwkStateRead          (uint8 *pu8TimerMs);
PRIVATE bool_t APP_bNtagNwkStateWrite         (uint8 *pu8TimerMs);
PRIVATE bool_t APP_bNtagNwkNfcNwkValid        (tsNfcNwk *psNfcNwk);
PRIVATE bool_t APP_bNtagNwkNfcNwkProcess      (tsNfcNwk *psNfcNwk);
PRIVATE void   APP_vNtagNwkNfcNwkSet          (tsNfcNwk *psNfcNwk, uint8 u8Sequence);
#if TRACE_APP_NTAG_DATA
PRIVATE void   APP_vNtagNwkNfcNwkDebug        (tsNfcNwk *psNfcNwk);
#endif // TRACE_APP_NTAG_DATA
#endif // APP_NTAG_NWK
#if (defined APP_NTAG_OTA)
PRIVATE void   APP_vNtagOtaPdmLoadCmdImageNtag(void);
PRIVATE bool_t APP_bNtagOtaStateWrite         (uint8 *pu8TimerMs);
PRIVATE bool_t APP_bNtagOtaStatePoll          (uint8 *pu8TimerMs);
PRIVATE bool_t APP_bNtagOtaStateRead          (uint8 *pu8TimerMs);
PRIVATE bool_t APP_bNtagOtaCmdImageProcess    (tsNfcOtaCmdImage *psNfcOtaCmdImage);
PRIVATE bool_t APP_bNtagOtaCmdDataProcess     (tsNfcOtaCmdData  *psNfcOtaCmdData);
PRIVATE bool_t APP_bNtagOtaWriteFlashBuffer   (void);
PRIVATE void   APP_vNtagOtaXorBuffer          (uint8 *pu8Buffer, uint32 u32Length);
PRIVATE bool_t APP_bNtagOtaImageValid         (void);
PRIVATE void   APP_vNtagOtaImageChange        (void);
PRIVATE void   APP_vNtagOtaCmdImageDebug      (tsNfcOtaCmdImage *psNfcOtaCmdImage);
#endif // APP_NTAG_OTA
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
#if ((defined APP_NTAG_OTA) && (!defined BUILD_OTA))
/* Declare these here (and use them below) so sections get created */
PUBLIC uint8 s_au8Nonce[16] __attribute__ ((section (".ro_nonce"))) =
    { 0x00, 0x00, 0x00, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x00, 0x00, 0x00, 0x00 };
PUBLIC uint8 s_au8LnkKeyArray[16] __attribute__ ((section (".ro_se_lnkKey"))) =
    { 0x5a, 0x69, 0x67, 0x42, 0x65, 0x65, 0x41, 0x6c, 0x6c, 0x69, 0x61, 0x6e, 0x63, 0x65, 0x30, 0x39 };
PUBLIC uint8 au8OtaHeader[NFC_OTA_MAX_HEADER_SIZE] __attribute__ ((section (".ro_ota_header"))) =
       {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
#else
#if (defined APP_NTAG_OTA)
/* These are declared elsewhere */
extern uint8 s_au8Nonce[16];
extern uint8 s_au8LnkKeyArray[16];
extern uint8 au8OtaHeader[NFC_OTA_MAX_HEADER_SIZE];
#endif
#endif

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE uint32                   u32AppNtagTicks;
PRIVATE uint32                   u32AppNtagMs;
PRIVATE teAppNtagState             eAppNtagState;
PRIVATE teAppNtagNciState          eAppNtagNciState;
PRIVATE bool_t                     bNtagTimer;
PRIVATE bool_t                     bNtagActive;
PRIVATE uint32                   u32AppNtagStartMs;
PRIVATE uint8                     u8Endpoint;
PRIVATE uint32                   u32Errors;
PRIVATE uint8                    au8HeaderBlock[NTAG_BLOCK_SIZE]  __attribute__((section(".bss.discard.app")));
PRIVATE uint8                    au8VersionData[NFC_VERSION_SIZE] __attribute__((section(".bss.discard.app")));
PRIVATE uint8                    au8ConfigBlock[NTAG_BLOCK_SIZE]  __attribute__((section(".bss.discard.app")));
PRIVATE bool_t                     bFactoryResetOnStop;
PRIVATE bool_t                     bResetOnStop;
PRIVATE char                      acAppNtagState[E_APP_NTAG_STATE_ERROR+1][13] =
{
    "INIT_READ",
    "HEADER_WRITE",
    "CONFIG_WRITE",
    "IDLE",
#if (defined APP_NTAG_NWK)
    "NWK_READ",
    "NWK_WRITE",
#endif
#if (defined APP_NTAG_OTA)
    "OTA_WRITE",
    "OTA_POLL",
    "OTA_READ",
#endif
    "ERROR"
};

#if ((defined APP_NTAG_NWK) || (defined APP_NTAG_OTA))
PRIVATE uint8                    au8NtagData[NTAG_DATA_SIZE] __attribute__((section(".bss.discard.app")));
#endif

#if (defined APP_NTAG_NWK)
PRIVATE tsNfcNwkInstallCode        sNfcNwkInstallCode __attribute__((section(".bss.discard.app")));
PRIVATE tsNfcNwkNci                sNfcNwkNci         __attribute__((section(".bss.discard.app")));
PRIVATE uint32                   u32NtagNwkWrite;
#endif

#if (defined APP_NTAG_OTA)
PRIVATE tsNfcOtaCmdImage           sNfcOtaCmdImageNtag;
PRIVATE uint8                    au8FlashBuffer[FLASH_PAGE_SIZE] __attribute__((section(".bss.discard.app")));
PRIVATE uint32                   u32FlashBuffer;
PRIVATE uint32                   u32AppNtagOtaMs;
PRIVATE uint32                   u32OtaPercent;
PRIVATE uint32                   u32FlashPdm;
PRIVATE bool_t                     bOtaChangeOnAbsent;
#endif

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
#if (defined APP_NTAG_NWK)
extern bool bGetInstallCode(uint8 install_code[16]);
extern void APP_vFactoryResetRecords(void); /* Should be in app_???_node.c already or an equivalent */
extern void APP_vOobcSetRunning(void);  /* Add to app_???_node.c */
#endif

/****************************************************************************
 *
 * NAME: APP_vNtagPdmLoad
 *
 * DESCRIPTION:
 * Attempts to load PDM record and initiates OOBC if recovered
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC bool_t APP_bNtagPdmLoad(void)
{
    bool_t bReturn = FALSE;
    uint32 u32StartMs;
    uint32 u32StopMs;

    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: { APP_bNtagPdmLoad()", OSA_TimeGetMsec());
    /* Note start time */
    u32StartMs = OSA_TimeGetMsec();

    /* NWK ? */
    #if (defined APP_NTAG_NWK)
    {
        /* Attempt to load install code */
        APP_vNtagNwkPdmLoadInstallCode();
        /* Attempt to load nci data */
        bReturn = APP_bNtagNwkPdmLoadNci();
    }
    #endif
    /* OTA ? */
    #if (defined APP_NTAG_OTA)
    {
        /* Attempt to load ntag data */
        APP_vNtagOtaPdmLoadCmdImageNtag();
    }
    #endif

    /* Note end time */
    u32StopMs = OSA_TimeGetMsec();
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: } APP_bNtagPdmLoad() = %d %dms", u32StopMs, bReturn, u32StopMs - u32StartMs);

    return bReturn;
}

/****************************************************************************
 *
 * NAME: APP_vNtagStart
 *
 * DESCRIPTION:
 * Starts NTAG processing
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void APP_vNtagStart(uint8 u8ApplicationEndpoint)
{
    uint32 u32StartMs;
    uint32 u32StopMs;
    bool_t   bAlreadyActive;

    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: { APP_vNtagStart(x%02x)", OSA_TimeGetMsec(), u8ApplicationEndpoint);
    /* Note current active flag */
    bAlreadyActive = bNtagActive;
    /* Note start time */
    u32StartMs = OSA_TimeGetMsec();
    /* Not already doing something ? */
    if (bNtagActive == FALSE)
    {
        /* We are now active */
        bNtagActive = TRUE;
        /* Note start time */
        u32AppNtagStartMs = u32StartMs;
        /* Note endpoint */
        u8Endpoint = u8ApplicationEndpoint;
        /* Initialise main NTAG state machine */
        NTAG_vInitialise(APP_NTAG_ADDRESS,
                         APP_NTAG_I2C_SCL,
                         APP_NTAG_I2C_FREQUENCY_HZ,
                         APP_BUTTONS_NFC_FD);
        /* Register callback */
        //NTAG_vRegCbEvent(APP_cbNtagEvent);
        /* Clear errors */
        u32Errors = 0;
        /* Flag that the timer should run */
        bNtagTimer = TRUE;
        /* Start the timer in 1ms */
        ZTIMER_eStart(u8TimerNtag, NTAG_TIMER_MIN_MS);
    }
    /* Note end time */
    u32StopMs = OSA_TimeGetMsec();
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: } APP_vNtagStart(x%02x) %s %dms Active = %d (%d)",
        u32StopMs, u8ApplicationEndpoint, acAppNtagState[eAppNtagState], u32StopMs - u32StartMs, bNtagActive, bAlreadyActive);
}

/****************************************************************************
 *
 * NAME: APP_vNtagStop
 *
 * DESCRIPTION:
 * Stops NTAG processing
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void APP_vNtagStop(void)
{
    uint32 u32StartMs;
    uint32 u32StopMs;

    /* Note start time */
    u32StartMs = OSA_TimeGetMsec();
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: { APP_vNtagStop() %s %dMS", u32StartMs, acAppNtagState[eAppNtagState], u32StartMs - u32AppNtagStartMs);
    /* Note start time again (for better function timing) */
    u32StartMs = OSA_TimeGetMsec();
    /* Flag that the timer shouldn't run */
    bNtagTimer = FALSE;
    /* Deinitialise NTAG */
    NTAG_vDeinitialise();
    /* Go back to unknown nci state (so if start is called again we process the ntag) */
    eAppNtagNciState = E_APP_NTAG_NCI_STATE_NONE;
    /* Note end time */
    u32StopMs = OSA_TimeGetMsec();
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: } APP_vNtagStop() %s %dms %dMS Active = %d",
        u32StopMs, acAppNtagState[eAppNtagState], u32StopMs - u32StartMs, u32StartMs - u32AppNtagStartMs, bNtagActive);
    /* We are no longer active */
    bNtagActive = FALSE;
    /* Factory reset on stop ? */
    if (bFactoryResetOnStop)
    {
        ZPS_teStatus eStatus;

        /* Request leave */
        eStatus = ZPS_eAplZdoLeaveNetwork(0, FALSE, FALSE);
        /* Debug */
        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "ZPS_eAplZdoLeaveNetwork(0, F, F) = %d", eStatus);
        /* Leave without rejoin request rejected (probably not in a network) ? */
        if (ZPS_E_SUCCESS != eStatus)
        {
            /* Debug */
            DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_vFactoryResetRecords()");
            /* Leave failed, so just reset everything */
            APP_vFactoryResetRecords();
            /* Flag we want to reset */
            bResetOnStop = TRUE;
        }
        /* Success ? */
        else
        {
            /* Wait for leave event to reset us */
            bResetOnStop = FALSE;
        }
    }
    /* Reset on stop ? */
    if (bResetOnStop)
    {
        /* JN518x ? */
        #if (JENNIC_CHIP_FAMILY == JN518x)
        {
            /* Debug */
            DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "RESET_SystemReset()");
            /* Reset */
            MICRO_DISABLE_INTERRUPTS();
            RESET_SystemReset();
        }
        /* JN516x / JN517x ? */
        #else
        {
            /* Debug */
            DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "vAHI_SwReset()");
            /* Reset */
            vAHI_SwReset();
        }
        #endif
    }
}

/****************************************************************************
 *
 * NAME: APP_cbNtagTimer
 *
 * DESCRIPTION:
 * Timer callback function
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void APP_cbNtagTimer(void *pvParams)
{
    bool_t         bResult  = TRUE;
    uint8         u8TimerMs = 0;
    teAppNtagState eAppNtagStateStart;
    /* Note start state */
    eAppNtagStateStart = eAppNtagState;
    /* Increment tick counter and timer value */
    u32AppNtagTicks++;
    u32AppNtagMs += APP_NTAG_TICK_MS;
    /* Maintain driver NTAG state machine */
    //NTAG_vTick(APP_NTAG_TICK_MS);
    /* What state are we in ? */
    switch (eAppNtagState)
    {
        /* Handle different states ? */
        case E_APP_NTAG_STATE_INIT_READ:    bResult = APP_bNtagStateInitRead(&u8TimerMs);    break;
        case E_APP_NTAG_STATE_HEADER_WRITE: bResult = APP_bNtagStateHeaderWrite(&u8TimerMs); break;
        case E_APP_NTAG_STATE_CONFIG_WRITE: bResult = APP_bNtagStateConfigWrite(&u8TimerMs); break;
        case E_APP_NTAG_STATE_IDLE:         bResult = APP_bNtagStateIdle(&u8TimerMs);        break;
#if (defined APP_NTAG_NWK)
        case E_APP_NTAG_NWK_STATE_READ:     bResult = APP_bNtagNwkStateRead(&u8TimerMs);     break;
        case E_APP_NTAG_NWK_STATE_WRITE:    bResult = APP_bNtagNwkStateWrite(&u8TimerMs);    break;
#endif
#if (defined APP_NTAG_OTA)
        case E_APP_NTAG_OTA_STATE_WRITE:    bResult = APP_bNtagOtaStateWrite(&u8TimerMs);    break;
        case E_APP_NTAG_OTA_STATE_POLL:     bResult = APP_bNtagOtaStatePoll(&u8TimerMs);     break;
        case E_APP_NTAG_OTA_STATE_READ:     bResult = APP_bNtagOtaStateRead(&u8TimerMs);     break;
#endif
        default:                            APP_vNtagStop();                                 break;
    }
    /* Success ? */
    if (bResult)
    {
        /* Clear errors */
        u32Errors = 0;
    }
    /* Failed ? */
    else
    {
        /* Increment errors */
        u32Errors++;
        /* Debug */
        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: } APP_cbNtagTimer() u32Errors = %d %s->%s", OSA_TimeGetMsec(), u32Errors, acAppNtagState[eAppNtagStateStart], acAppNtagState[eAppNtagState]);
        /* Too many errors ? */
        if (u32Errors > 4)
        {
            /* Go to error state - MIGHT WANT TO GO IDLE INSTEAD */
            eAppNtagState = E_APP_NTAG_STATE_ERROR;
            /* Debug */
            DBG_vPrintf(TRACE_APP_NTAG, "->%s", acAppNtagState[eAppNtagState]);
            /* Stop processing */
            APP_vNtagStop();
        }
    }
    /* Want to run timer again ?  */
    if (bNtagTimer)
    {
        /* Timer invalid ? */
        if (u8TimerMs < NTAG_TIMER_MIN_MS
        ||  u8TimerMs > NTAG_TIMER_MAX_MS)
        {
            /* Use maximum for safety */
            u8TimerMs = NTAG_TIMER_MAX_MS;
        }
        /* Run timer again */
        ZTIMER_eStart(u8TimerNtag, u8TimerMs);
    }
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: APP_bNtagStateInitRead()
 *
 * DESCRIPTION:
 * Timer handler for R_INIT state
 *
 ****************************************************************************/
PRIVATE bool_t APP_bNtagStateInitRead(uint8 *pu8TimerMs)
{
    uint32 u32StartMs;
    uint32 u32StopMs;
    bool_t   bResult;

    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "APP_NTAG %d: { APP_bNtagStateInitRead()", OSA_TimeGetMsec());
    /* Note start time */
    u32StartMs = OSA_TimeGetMsec();
    /* Use minimum timer period */
    *pu8TimerMs = NTAG_TIMER_MIN_MS;
    /* Read the header */
    bResult = NTAG_bRead(0, NTAG_BLOCK_SIZE, au8HeaderBlock);
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "NTAG_bRead(0, NTAG_BLOCK_SIZE) = %d", bResult);
    /* Success ? */
    if (bResult)
    {
        /* Read the version */
        bResult = NTAG_bReadVersion(NFC_VERSION_SIZE, au8VersionData);
        /* Debug */
        DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "NTAG_bReadVersion(NFC_VERSION_SIZE) = %d", bResult);
        /* Success ? */
        if (bResult)
        {
            /* Read the configuration registers */
            bResult = NTAG_bRead(NTAG_u32Config(), NTAG_BLOCK_SIZE, au8ConfigBlock);
            /* Debug */
            DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "NTAG_bRead(CONFIG, NTAG_BLOCK_SIZE) = %d", bResult);
            /* Success ? */
            if (bResult)
            {
                /* Are the capability container bytes (in the header) invalid ? */
                if (APP_bNtagCcValid(&au8HeaderBlock[12]) == FALSE)
                {
                    /* Need to correct and write header data back to NTAG */
                    eAppNtagState = E_APP_NTAG_STATE_HEADER_WRITE;
                }
                /* Is the last ndef block in the configuration register invalid ? */
                else if (APP_bNtagConfigValid(au8ConfigBlock) == FALSE)
                {
                    /* Need to correct and write config data back to NTAG */
                    eAppNtagState = E_APP_NTAG_STATE_CONFIG_WRITE;
                }
                /* Everything is ok ? */
                else
                {
                    /* Go idle */
                    eAppNtagState = E_APP_NTAG_STATE_IDLE;
                }
            }
        }
    }
    /* Note end time */
    u32StopMs = OSA_TimeGetMsec();
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: } APP_bNtagStateInitRead() = %d %s %dms", u32StopMs, bResult, acAppNtagState[eAppNtagState], u32StopMs - u32StartMs);

    return bResult;
}

/****************************************************************************
 *
 * NAME: APP_bNtagStateHeaderWrite()
 *
 * DESCRIPTION:
 * Timer handler for W_HEADER state
 *
 ****************************************************************************/
PRIVATE bool_t APP_bNtagStateHeaderWrite(uint8 *pu8TimerMs)
{
    uint32 u32StartMs;
    uint32 u32StopMs;
    bool_t   bResult;

    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "APP_NTAG %d: { APP_bNtagStateHeaderWrite()", OSA_TimeGetMsec());
    /* Note start time */
    u32StartMs = OSA_TimeGetMsec();
    /* Set correct CC bytes */
    APP_vNtagCcSet(&au8HeaderBlock[12]);
    /* Use maximum timer period */
    *pu8TimerMs = NTAG_TIMER_MAX_MS;
    /* Write the header */
    bResult = NTAG_bWrite(0, NTAG_BLOCK_SIZE, au8HeaderBlock, NTAG_WRITE_BLOCK_EEPROM_DELAY_US);
    /* Success ? */
    if (bResult)
    {
        /* Is the last ndef block in the configuration register invalid ? */
        if (APP_bNtagConfigValid(au8ConfigBlock) == FALSE)
        {
            /* Need to correct and write config data back to NTAG */
            eAppNtagState = E_APP_NTAG_STATE_CONFIG_WRITE;
        }
        /* Everything is ok ? */
        else
        {
            /* Go idle */
            eAppNtagState = E_APP_NTAG_STATE_IDLE;
        }
    }
    /* Note end time */
    u32StopMs = OSA_TimeGetMsec();
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: } APP_bNtagStateHeaderWrite() = %d %s %dms", u32StopMs, bResult, acAppNtagState[eAppNtagState], u32StopMs - u32StartMs);

    return bResult;
}

/****************************************************************************
 *
 * NAME: APP_vNtagTimerWConfig()
 *
 * DESCRIPTION:
 * Timer handler for W_CONFIG state
 *
 ****************************************************************************/
PRIVATE bool_t APP_bNtagStateConfigWrite(uint8 *pu8TimerMs)
{
    uint32 u32StartMs;
    uint32 u32StopMs;
    bool_t   bResult;

    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "APP_NTAG %d: { APP_bNtagStateConfigWrite()", OSA_TimeGetMsec());
    /* Note start time */
    u32StartMs = OSA_TimeGetMsec();
    /* Set correct config registers */
    APP_vNtagConfigSet(au8ConfigBlock);
    /* Use maximum timer period */
    *pu8TimerMs = NTAG_TIMER_MAX_MS;
    /* Write the configuration registers */
    bResult = NTAG_bWrite(NTAG_u32Config(), NTAG_BLOCK_SIZE, au8ConfigBlock, NTAG_WRITE_BLOCK_EEPROM_DELAY_US);
    /* Success ? */
    if (bResult)
    {
        /* Go idle */
        eAppNtagState = E_APP_NTAG_STATE_IDLE;
    }
    /* Note end time */
    u32StopMs = OSA_TimeGetMsec();
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: } APP_bNtagStateConfigWrite() = %d %s %dms", u32StopMs, bResult, acAppNtagState[eAppNtagState], u32StopMs - u32StartMs);

    return bResult;
}

/****************************************************************************
 *
 * NAME: APP_vNtagTimerIdle()
 *
 * DESCRIPTION:
 * Timer handler for IDLE state
 *
 ****************************************************************************/
PRIVATE bool_t APP_bNtagStateIdle(uint8 *pu8TimerMs)
{
    uint32 u32StartMs;
    uint32 u32StopMs;
    bool_t   bResult = TRUE;
    bool_t   bFd;

    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: { APP_bNtagStateIdle()", OSA_TimeGetMsec());
    /* Note start time */
    u32StartMs = OSA_TimeGetMsec();
    /* Use minimum timer period */
    *pu8TimerMs = NTAG_TIMER_MIN_MS;
    /* Read fd */
    bFd = NTAG_bReadFd();
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "NTAG_bReadFd() = %d", bFd);
    /* Are we outside field ? */
    if (bFd == TRUE)
    {
        /* Not outside field ? */
        if (eAppNtagNciState != E_APP_NTAG_NCI_STATE_ABSENT)
        {
            /* Now outside field */
            eAppNtagNciState = E_APP_NTAG_NCI_STATE_ABSENT;
            /* Debug */
            DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "eAppNtagNciState = ABSENT");
            /* Ntag ota ? */
            #if (defined APP_NTAG_OTA)
            {
                /* Reset on absent ? */
                if (bOtaChangeOnAbsent)
                {
                    /* Debug */
                    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "bOtaChangeOnAbsent = %d", bOtaChangeOnAbsent);
                    /* Valid image to activate ? */
                    if (APP_bNtagOtaImageValid())
                    {
                        /* Run image */
                        APP_vNtagOtaImageChange();
                    }
                    /* Invalid ? */
                    else
                    {
                        /* Start again with a clean structure */
                        APP_vNtagOtaPdmLoadCmdImageNtag();
                    }
                }
            }
            #endif
            /* Ntag nwk ? */
            #if (defined APP_NTAG_NWK)
            {
                /* Clear errors */
                u32Errors = 0;
                /* Go to read ndef state */
                eAppNtagState = E_APP_NTAG_NWK_STATE_READ;
            }
            #endif
        }
    }
    /* Are we inside field ? */
    else
    {
        /* Not inside field ? */
        if (eAppNtagNciState != E_APP_NTAG_NCI_STATE_PRESENT)
        {
            /* Now inside field */
            eAppNtagNciState = E_APP_NTAG_NCI_STATE_PRESENT;
            /* Debug */
            DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "eAppNtagNciState = PRESENT");
            /* Ntag ota ? */
            #if (defined APP_NTAG_OTA)
            {
                /* Clear errors */
                u32Errors = 0;
                /* Go to write ota state */
                eAppNtagState = E_APP_NTAG_OTA_STATE_WRITE;
            }
            #endif
        }
    }
    /* Still in IDLE state ? */
    if (eAppNtagState == E_APP_NTAG_STATE_IDLE)
    {
        /* Stop processing (staying in idle state) */
        APP_vNtagStop();
    }
    /* Note end time */
    u32StopMs = OSA_TimeGetMsec();
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: } APP_bNtagStateIdle() = %d %s %dms", u32StopMs, bResult, acAppNtagState[eAppNtagState], u32StopMs - u32StartMs);

    return bResult;
}

/****************************************************************************
 *
 * NAME: APP_bNtagCcValid
 *
 * DESCRIPTION:
 * Checks capability container bytes in header
 *
 * RETURNS:
 * TRUE  - if valid
 * FALSE - if invalid (and corrected)
 *
 ****************************************************************************/
PRIVATE bool_t APP_bNtagCcValid(uint8 *pu8Data)
{
    bool_t bReturn = TRUE;

    /* Invalid CC data ? */
    if (pu8Data[0] != 0xE1  /* NDEF format */
    ||  pu8Data[1] != 0x10  /* Version 1.0 */
    ||  pu8Data[2] != 0x6D  /* Data size 872 bytes (assuming lowest across supported NTAGS) */
    ||  pu8Data[3] != 0x00) /* R/W allowed */
    {
        /* Return they were not valid */
        bReturn = FALSE;
    }

    return bReturn;
}

/****************************************************************************
 *
 * NAME: APP_vNtagCcSet
 *
 * DESCRIPTION:
 * Sets capability bytes in header
 *
 ****************************************************************************/
PRIVATE void APP_vNtagCcSet(uint8 *pu8Data)
{
    pu8Data[0] = 0xE1; /* NDEF format */
    pu8Data[1] = 0x10; /* Version 1.0 */
    pu8Data[2] = 0x6D; /* Data size 872 bytes (assuming lowest across supported NTAGS) */
    pu8Data[3] = 0x00; /* R/W allowed */
}

/****************************************************************************
 *
 * NAME: APP_bNtagConfigValid
 *
 * DESCRIPTION:
 * Checks config register bytes
 *
 * RETURNS:
 * TRUE  - if valid
 * FALSE - if invalid
 *
 ****************************************************************************/
PRIVATE bool_t APP_bNtagConfigValid(uint8 *pu8Data)
{
    bool_t bReturn = TRUE;

    /* Invalid data ? */
    if (pu8Data[NFC_OFFSET_LAST_NDEF_BLOCK] != 0x37)
    {
        /* Return they were not already valid */
        bReturn = FALSE;
    }

    return bReturn;
}

/****************************************************************************
 *
 * NAME: APP_vNtagConfigSet
 *
 * DESCRIPTION:
 * Sets config register bytes
 *
 ****************************************************************************/
PRIVATE void APP_vNtagConfigSet(uint8 *pu8Data)
{
    /* Correct values */
    pu8Data[NFC_OFFSET_LAST_NDEF_BLOCK] = 0x37; /* Assuming lowest across supported NTAGs */
}

#if (defined APP_NTAG_NWK)
/****************************************************************************
 *
 * NAME: APP_vNtagNwkPdmLoadInstallCode()
 *
 * DESCRIPTION:
 * Attempts to load install code from psector (preferred) or PDM, if not found
 * a random code is generated and saved to PDM.
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void APP_vNtagNwkPdmLoadInstallCode(void)
{
    uint32 u32StartMs;
    uint32 u32StopMs;
    uint8    u8Byte;
    uint16  u16PdmRead;
    uint8    u8InstallCode = 0;

    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: { APP_vNtagNwkPdmLoadInstallCode()", OSA_TimeGetMsec());
    /* Note start time */
    u32StartMs = OSA_TimeGetMsec();
    /* Can we get an install code from the stack ? */
    if (bGetInstallCode(sNfcNwkInstallCode.au8Key))
    {
        /* Non-zero install code ? */
        for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_KEY_SIZE && u8InstallCode == 0; u8Byte++)
        {
            if (sNfcNwkInstallCode.au8Key[u8Byte] != 0)
            {
                /* Note install code came from psector */
                u8InstallCode = 1;
                /* Generate crc from install code */
                sNfcNwkInstallCode.u16Crc = ZPS_u16crc(sNfcNwkInstallCode.au8Key, NFC_NWK_PAYLOAD_KEY_SIZE);
            }
        }
    }
    /* Stack doesn't have an install code ? */
    if (u8InstallCode == 0)
    {
        /* Attempt to get install code from pdm ? */
        u16PdmRead = 0;
        (void) PDM_eReadDataFromRecord(PDM_ID_APP_NFC_ICODE,
                                       &sNfcNwkInstallCode,
                                       sizeof(tsNfcNwkInstallCode),
                                       &u16PdmRead);
        /* Install code in pdm ? */
        if (sizeof(tsNfcNwkInstallCode) == u16PdmRead)
        {
            /* Non-zero install code ? */
            for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_KEY_SIZE && u8InstallCode == 0; u8Byte++)
            {
                if (sNfcNwkInstallCode.au8Key[u8Byte] != 0)
                {
                    uint16 u16Crc;

                    /* Generate crc from install code */
                    u16Crc = ZPS_u16crc(sNfcNwkInstallCode.au8Key, NFC_NWK_PAYLOAD_KEY_SIZE);
                    /* Matching crc ? */
                    if (u16Crc == sNfcNwkInstallCode.u16Crc)
                    {
                        /* Note install code came from pdm */
                        u8InstallCode = 2;
                    }
                }
            }
        }
    }
    /* Stack and pdm don't have install code ? */
    if (u8InstallCode == 0)
    {
        /* JN518x ? */
        #if (JENNIC_CHIP_FAMILY == JN518x)
        {
            trng_config_t trng_config;

            /* Generate random install code */
            TRNG_GetDefaultConfig(&trng_config);
            TRNG_Init(RNG, &trng_config);
            for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_KEY_SIZE; u8Byte++)
            {
                TRNG_GetRandomData(RNG, &sNfcNwkInstallCode.au8Key[u8Byte], 1);
            }
            TRNG_Deinit(RNG);
        }
        /* Not JN518x ? */
        #else
        {
            uint16 u16Random;

            /* Generate random install code */
            for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_KEY_SIZE; u8Byte++)
            {
                vAHI_StartRandomNumberGenerator(E_AHI_RND_SINGLE_SHOT, E_AHI_INTS_DISABLED);
                while(!bAHI_RndNumPoll());
                u16Random = u16AHI_ReadRandomNumber();
                sNfcNwkInstallCode.au8Key[u8Byte] = (uint8)(u16Random & 0xFF);
            }
        }
        #endif
        /* Calculate CRC */
        sNfcNwkInstallCode.u16Crc = ZPS_u16crc(sNfcNwkInstallCode.au8Key, NFC_NWK_PAYLOAD_KEY_SIZE);
        /* Save new install code */
        PDM_eSaveRecordData(PDM_ID_APP_NFC_ICODE,
                            &sNfcNwkInstallCode,
                            sizeof(tsNfcNwkInstallCode));
        /* Note we generated a new install code */
        u8InstallCode = 3;
    }
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "u8InstallCode         = %d", u8InstallCode);
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "sNfcNwkInstallCode:");
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "au8Key                =");
    for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_KEY_SIZE; u8Byte++) DBG_vPrintf(TRACE_APP_NTAG, " %02x", sNfcNwkInstallCode.au8Key[u8Byte]);
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "u16Crc                = x%04x", sNfcNwkInstallCode.u16Crc);
    /* Note end time */
    u32StopMs = OSA_TimeGetMsec();
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: } APP_vNtagNwkPdmLoadInstallCode() %dms", u32StopMs, u32StopMs - u32StartMs);
}

/****************************************************************************
 *
 * NAME: APP_bNtagNwkPdmLoadNci()
 *
 * DESCRIPTION:
 * Attempts to load NCI data from PDM, if found the OOB joining process is
 * initiated.
 *
 * RETURNS:
 * TRUE  - OOB joining process initiated
 * FALSE - OOB joining process not initiated
 *
 ****************************************************************************/
PRIVATE bool_t APP_bNtagNwkPdmLoadNci(void)
{
    bool_t bReturn = FALSE;
    uint32 u32StartMs;
    uint32 u32StopMs;
    uint8    u8Byte;
    uint16  u16PdmRead;

    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: { APP_bNtagNwkPdmLoadNci()", OSA_TimeGetMsec());
    /* Note start time */
    u32StartMs = OSA_TimeGetMsec();
    /* Attempt to read network nci data */
    u16PdmRead = 0;
    PDM_eReadDataFromRecord(PDM_ID_APP_NFC_NWK_NCI,
                            &sNfcNwkNci,
                            sizeof(tsNfcNwkNci),
                            &u16PdmRead);
    /* Recovered data ? */
    if (sizeof(tsNfcNwkNci) == u16PdmRead)
    {
        uint8  au8NetworkKey[NFC_NWK_PAYLOAD_KEY_SIZE];

        /* Join with install code command ? */
        if (NFC_CMD_NWK_NCI_JOIN_WITH_CODE == sNfcNwkNci.u8Command)
        {
            uint64 u64ExtAddress;

            /* Get our extended address */
            u64ExtAddress = ZPS_u64AplZdoGetIeeeAddr();
            /* Decrypt key */
            bReturn = BDB_bOutOfBandCommissionGetKey(sNfcNwkInstallCode.au8Key,
                                                     sNfcNwkNci.au8Key,
                                                     u64ExtAddress,
                                                     au8NetworkKey,
                                                     sNfcNwkNci.au8Mic);
        }
        /* Join with key command ? */
        else if (NFC_CMD_NWK_NCI_JOIN_WITH_KEY == sNfcNwkNci.u8Command)
        {
            /* Just copy key */
            memcpy(au8NetworkKey, sNfcNwkNci.au8Key, NFC_NWK_PAYLOAD_KEY_SIZE);
            /* We got the key */
            bReturn = TRUE;
        }

        /* Success (got key) ? */
        if (bReturn)
        {
            /* Set result back to false - until we are successful */
            bReturn = FALSE;
            /* Got a valid channel ? */
            if (sNfcNwkNci.u8Channel >= 11 && sNfcNwkNci.u8Channel <= 26)
            {
                uint8                          u8BdbOob;
                BDB_tsOobWriteDataToCommission  sBdbOob;

                /* Populate BDB data */
                sBdbOob.u64PanId              = sNfcNwkNci.u64ExtPanId;
                sBdbOob.u64TrustCenterAddress = sNfcNwkNci.u64ExtAddress;
                sBdbOob.pu8NwkKey             = au8NetworkKey;
                sBdbOob.pu8InstallCode        = (uint8 *) NULL; /* Don't use until we have proper end-to-end icode implementation */
                sBdbOob.u16PanId              = sNfcNwkNci.u16PanId;
                sBdbOob.u16ShortAddress       = 0xFFFF;
                sBdbOob.u8ActiveKeySqNum      = sNfcNwkNci.u8KeySeqNum;
                sBdbOob.u8DeviceType          = ZPS_eAplZdoGetDeviceType();
                sBdbOob.u8Channel             = sNfcNwkNci.u8Channel;
                sBdbOob.u8NwkUpdateId         = 1;
                /* End device ? */
                if (sBdbOob.u8DeviceType == ZPS_ZDO_DEVICE_ENDDEVICE)
                {
                    /* Use ED settings */
                    sBdbOob.bRejoin          = TRUE;
                    sBdbOob.u8RxOnWhenIdle   = FALSE;
                }
                /* Others ? */
                else
                {
                    /* Use router settings */
                    sBdbOob.bRejoin          = FALSE;
                    sBdbOob.u8RxOnWhenIdle   = TRUE;
                }
                /* Commission device */
                u8BdbOob = BDB_u8OutOfBandCommissionStartDevice(&sBdbOob);
                /* Set app state to running for oobc */
                APP_vOobcSetRunning();
                /* Set on a network */
                sBDB.sAttrib.bbdbNodeIsOnANetwork = TRUE;
                /* Delete record */
                PDM_vDeleteDataRecord(PDM_ID_APP_NFC_NWK_NCI);
                /* Debug */
                DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "BDB_u8OutOfBandCommissionStartDevice() = %d", u8BdbOob);
                DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "u64PanId              = x%08x%08x", (uint32)(sBdbOob.u64PanId >> 32), (uint32)(sBdbOob.u64PanId & 0xFFFFFFFF));
                DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "u64TrustCenterAddress = x%08x%08x", (uint32)(sBdbOob.u64TrustCenterAddress >> 32), (uint32)(sBdbOob.u64TrustCenterAddress & 0xFFFFFFFF));
                DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "pu8NwkKey             =");
                for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_KEY_SIZE; u8Byte++) DBG_vPrintf(TRACE_APP_NTAG, " %02x", au8NetworkKey[u8Byte]);
                DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "u16PanId              = x%04x", sBdbOob.u16PanId);
                DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "u8DeviceType          = x%02x", sBdbOob.u8DeviceType);
                DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "u8Channel             = %d",    sBdbOob.u8Channel);
                DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "bRejoin               = %d",    sBdbOob.bRejoin);
                DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "u8RxOnWhenIdle        = %d",    sBdbOob.u8RxOnWhenIdle);
                /* Note we started the join process */
                bReturn = TRUE;
            }
        }
    }
    /* Note end time */
    u32StopMs = OSA_TimeGetMsec();
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: } APP_bNtagNwkPdmLoadNci() = %d %dms", u32StopMs, bReturn, u32StopMs - u32StartMs);

    return bReturn;
}

/****************************************************************************
 *
 * NAME: APP_bNtagNwkStateRead()
 *
 * DESCRIPTION:
 * Timer handler for R_NWK state
 *
 ****************************************************************************/
PRIVATE bool_t APP_bNtagNwkStateRead(uint8 *pu8TimerMs)
{
    uint32 u32StartMs;
    uint32 u32StopMs;
    bool_t   bResult;
    bool_t   bValidNfcNwk = FALSE;
    bool_t   bWriteNfcNwk = TRUE;

    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "APP_NTAG %d: { APP_bNtagNwkStateRead()", OSA_TimeGetMsec());
    /* Note start time */
    u32StartMs = OSA_TimeGetMsec();
    /* Use minimum timer period */
    *pu8TimerMs = NTAG_TIMER_MIN_MS;
    /* Read the data (enough to ensure our whole NDEF record can be read) */
    bResult = NTAG_bRead(NTAG_BLOCK_SIZE, NTAG_NWK_DATA_SIZE, au8NtagData);
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "NTAG_bRead(%d, %d) = %d", NTAG_BLOCK_SIZE, NTAG_NWK_DATA_SIZE, bResult);
    /* Success ? */
    if (bResult)
    {
        tsNfcNwk   sNfcNwk;

        /* Convert buffer to structure */
        NFC_NWK_vNfcNwkFromBuffer(&sNfcNwk, au8NtagData);
        /* Debug ? */
        #if TRACE_APP_NTAG_DATA
        {
            /* Debug */
            DBG_vPrintf(TRACE_APP_NTAG_DATA, NFC_DBG_NL "sNfcNwk(%d) READ:", sizeof(tsNfcNwk));
            /* Debug structure */
            APP_vNtagNwkNfcNwkDebug(&sNfcNwk);
        }
        #endif
        /* Validate data */
        bValidNfcNwk = APP_bNtagNwkNfcNwkValid(&sNfcNwk);
        /* Valid data ? */
        if (bValidNfcNwk)
        {
            /* Process data */
            bWriteNfcNwk = APP_bNtagNwkNfcNwkProcess(&sNfcNwk);
        }
        /* Need to write data ? */
        if (bWriteNfcNwk)
        {
            /* Set default data (incrementing sequence number) */
            APP_vNtagNwkNfcNwkSet(&sNfcNwk, sNfcNwk.sTlv.sNdef.sPayload.sNtag.u8Sequence + 1);
            /* Debug ? */
            #if TRACE_APP_NTAG_DATA
            {
                /* Debug */
                DBG_vPrintf(TRACE_APP_NTAG_DATA, NFC_DBG_NL "sNfcNwk(%d) WRITE:", sizeof(tsNfcNwk));
                /* Debug structure */
                APP_vNtagNwkNfcNwkDebug(&sNfcNwk);
            }
            #endif
            /* Convert structure to buffer */
            NFC_NWK_vNfcNwkToBuffer(&sNfcNwk, au8NtagData);
            /* Initialise write location */
            u32NtagNwkWrite = 0;
            /* Go to write ndef state */
            eAppNtagState = E_APP_NTAG_NWK_STATE_WRITE;
            /* Use maximum timer period */
            *pu8TimerMs = NTAG_TIMER_MAX_MS;
            /* Write the first block of data */
            bResult = NTAG_bWrite(u32NtagNwkWrite + NTAG_BLOCK_SIZE, NTAG_BLOCK_SIZE, &au8NtagData[u32NtagNwkWrite], NTAG_WRITE_BLOCK_EEPROM_DELAY_US);
            /* Success ? */
            if (bResult)
            {
                /* Update for next block */
                u32NtagNwkWrite += 16;
            }
        }
        else
        {
            /* Go to idle state */
            eAppNtagState = E_APP_NTAG_STATE_IDLE;
        }
    }
    /* Note end time */
    u32StopMs = OSA_TimeGetMsec();
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: } APP_bNtagNwkStateRead() = %d %s %dms bValid = %d bWrite=%d",
        u32StopMs, bResult, acAppNtagState[eAppNtagState], u32StopMs - u32StartMs, bValidNfcNwk, bWriteNfcNwk);

    return bResult;
}

/****************************************************************************
 *
 * NAME: APP_bNtagNwkStateWrite()
 *
 * DESCRIPTION:
 * Timer handler for W_NWK state
 *
 ****************************************************************************/
PRIVATE bool_t APP_bNtagNwkStateWrite(uint8 *pu8TimerMs)
{
    uint32 u32StartMs;
    uint32 u32StopMs;
    bool_t   bResult = TRUE;

    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "APP_NTAG %d: { APP_bNtagNwkStateWrite() EepromWrite = %d / %d", OSA_TimeGetMsec(), u32NtagNwkWrite, NTAG_NWK_DATA_SIZE);
    /* Note start time */
    u32StartMs = OSA_TimeGetMsec();
    /* Use minimum timer period */
    *pu8TimerMs = NTAG_TIMER_MIN_MS;
    /* No more data to write ? */
    if (u32NtagNwkWrite >= NTAG_NWK_DATA_SIZE)
    {
        /* Go to idle state */
        eAppNtagState = E_APP_NTAG_STATE_IDLE;
    }
    /* More data to write ? */
    else
    {
        /* Use maximum timer period */
        *pu8TimerMs = NTAG_TIMER_MAX_MS;
        /* Write the next block of data */
        bResult = NTAG_bWrite(u32NtagNwkWrite + NTAG_BLOCK_SIZE, NTAG_BLOCK_SIZE, &au8NtagData[u32NtagNwkWrite], NTAG_WRITE_BLOCK_EEPROM_DELAY_US);
        /* Success ? */
        if (bResult)
        {
            /* Update for next block */
            u32NtagNwkWrite += 16;
        }
    }
    /* Note end time */
    u32StopMs = OSA_TimeGetMsec();
    /* Verbose debugging ? */
    #if TRACE_APP_NTAG_VERBOSE
    {
        /* Debug all calls */
        DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "APP_NTAG %d: } APP_bNtagNwkStateWrite() = %d %s %dms", u32StopMs, bResult, acAppNtagState[eAppNtagState], u32StopMs - u32StartMs);
    }
    /* Normal debugging ? */
    #elif TRACE_APP_NTAG
    {
        /* Debug final call */
        if (eAppNtagState != E_APP_NTAG_NWK_STATE_WRITE) DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: } APP_bNtagNwkStateWrite() = %d %s %dms", u32StopMs, bResult, acAppNtagState[eAppNtagState], u32StopMs - u32StartMs);
    }
    #endif

    return bResult;
}

/****************************************************************************
 *
 * NAME: APP_bNtagNwkNfcNwkValid
 *
 * DESCRIPTION:
 * Checks NfcNwk structure
 *
 * RETURNS:
 * TRUE  - if valid
 * FALSE - if invalid
 *
 ****************************************************************************/
PRIVATE bool_t APP_bNtagNwkNfcNwkValid(tsNfcNwk *psNfcNwk)
{
    bool_t   bReturn = FALSE;

    /* Valid tlv data ? */
    if (psNfcNwk->sTlv.u8TlvType   == 0x03
    &&  psNfcNwk->sTlv.u8TlvLength == sizeof(tsNfcNwkNdef))
    {
        /* Valid ndef data ? */
        if (psNfcNwk->sTlv.sNdef.u8NdefFlags         == 0xd4
        &&  psNfcNwk->sTlv.sNdef.u8NdefTypeLength    == NFC_NWK_NDEF_TYPE_LEN
        &&  psNfcNwk->sTlv.sNdef.u8NdefPayloadLength == sizeof(tsNfcNwkPayload))
        {
            char acNdefType[NFC_NWK_NDEF_TYPE_LEN+1] = "com.nxp:JN51xx-NWK";

            /* Valid ndef type ? */
            if (memcmp(psNfcNwk->sTlv.sNdef.au8NdefType, acNdefType, NFC_NWK_NDEF_TYPE_LEN) == 0)
            {
                /* Valid version ? */
                if (psNfcNwk->sTlv.sNdef.sPayload.sNtag.u8Version == NFC_NWK_PAYLOAD_VERSION)
                {
                    ZPS_tsAplAfSimpleDescriptor   sDesc;
                    uint64                      u64ExtAddress;

                    /* Can not get simple descriptor ? */
                    if (ZPS_eAplAfGetSimpleDescriptor(u8Endpoint, &sDesc) != E_ZCL_SUCCESS)
                    {
                        /* Use unknown device id */
                        sDesc.u16DeviceId = 0xFFFF;
                    }
                    /* Get extended address */
                    u64ExtAddress = ZPS_u64AplZdoGetIeeeAddr();
                    /* Valid ntag data ? */
                    if (psNfcNwk->sTlv.sNdef.sPayload.sNtag.u16DeviceId   == sDesc.u16DeviceId
                    &&  psNfcNwk->sTlv.sNdef.sPayload.sNtag.u64ExtAddress == u64ExtAddress)
                    {
                        /* Data is valid */
                        bReturn = TRUE;
                        /* Debug */
                        DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "APP_NTAG %d: } APP_bNtagNwkNfcNwkValid() = %d", OSA_TimeGetMsec(), bReturn);
                    }
                    else
                    {
                        /* Debug */
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: } APP_bNtagNwkNfcNwkValid() = %d sNtag", OSA_TimeGetMsec(), bReturn);
                    }
                }
                else
                {
                    /* Debug */
                    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: } APP_bNtagNwkNfcNwkValid() = %d u8Version", OSA_TimeGetMsec(), bReturn);
                }
            }
            else
            {
                /* Debug */
                DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: } APP_bNtagNwkNfcNwkValid() = %d au8NdefType", OSA_TimeGetMsec(), bReturn);
            }
        }
        else
        {
            /* Debug */
            DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: } APP_bNtagNwkNfcNwkValid() = %d sNdef", OSA_TimeGetMsec(), bReturn);
        }
    }
    else
    {
        /* Debug */
        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: } APP_bNtagNwkNfcNwkValid() = %d sTlv", OSA_TimeGetMsec(), bReturn);
    }

    return bReturn;
}

/****************************************************************************
 *
 * NAME: APP_bNtagNwkNfcNwkProcess
 *
 * DESCRIPTION:
 * Processes NfcNwk structure
 *
 * RETURNS:
 * TRUE  - if needs writing
 * FALSE - if doesn't need writing
 *
 ****************************************************************************/
PRIVATE bool_t APP_bNtagNwkNfcNwkProcess(tsNfcNwk *psNfcNwk)
{
    bool_t   bWrite = FALSE;

    /* Which NCI command ? */
    switch(psNfcNwk->sTlv.sNdef.sPayload.sNci.u8Command)
    {
        /* Factory reset ? */
        case NFC_CMD_NWK_NCI_FACTORY_RESET:
        {
            /* Flag that we want to factory reset on stopping */
            bFactoryResetOnStop = TRUE;
            /* Debug */
            DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: } APP_bNtagNwkNfcNwkProcess() CMD_FACTORY_RESET OK", OSA_TimeGetMsec());
            /* Need to write data back to ntag */
            bWrite = TRUE;
        }
        break;

        /* Join with code ? */
        case NFC_CMD_NWK_NCI_JOIN_WITH_CODE:
        {
            bool_t          bKey;
            uint8         au8NetworkKey[NFC_NWK_PAYLOAD_KEY_SIZE];
            uint64        u64ExtAddress;

            /* Get our extended address */
            u64ExtAddress   = ZPS_u64AplZdoGetIeeeAddr();
            /* Attempt to decrypt network key */
            bKey = BDB_bOutOfBandCommissionGetKey(sNfcNwkInstallCode.au8Key,
                                                  psNfcNwk->sTlv.sNdef.sPayload.sNci.au8Key,
                                                  u64ExtAddress,
                                                  au8NetworkKey,
                                                  psNfcNwk->sTlv.sNdef.sPayload.sNci.au8Mic);
            /* Debug */
            DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: } APP_bNtagNwkNfcNwkProcess() CMD_JOIN_WITH_CODE", OSA_TimeGetMsec());
            /* Success ? */
            if (bKey)
            {
                PDM_teStatus   ePdmStatus;

                /* Debug */
                DBG_vPrintf(TRACE_APP_NTAG, " KEY", OSA_TimeGetMsec());
                /* Copy data into PDM structure */
                memcpy(&sNfcNwkNci, &psNfcNwk->sTlv.sNdef.sPayload.sNci, sizeof(tsNfcNwkNci));
                /* Save record */
                ePdmStatus = PDM_eSaveRecordData(PDM_ID_APP_NFC_NWK_NCI,
                                                 &sNfcNwkNci,
                                                 sizeof(tsNfcNwkNci));
                /* Success ? */
                if (ePdmStatus == PDM_E_STATUS_OK)
                {
                    /* Debug */
                    DBG_vPrintf(TRACE_APP_NTAG, " PDM OK", OSA_TimeGetMsec());
                    /* Flag we want to reset on stop */
                    bResetOnStop = TRUE;
                }
            }
            /* Need to write data back to ntag */
            bWrite = TRUE;
        }
        break;

        /* No command ? */
        case NFC_CMD_NONE:
        {
            /* Do nothing */
            ;
        }
        break;

        /* Unexpected command ? */
        default:
        {
            /* Need to write data back to ntag (to clear it) */
            bWrite = TRUE;
        }
        break;
    }

    /* Not already decided we need to write data back to ntag ? */
    if (bWrite == FALSE)
    {

        /* Non-network data incorrect ? */
        if (psNfcNwk->sTlv.sNdef.sPayload.sNtag.u8Command   != NFC_CMD_NWK_NTAG_JOIN_WITH_CODE
        ||  memcmp(psNfcNwk->sTlv.sNdef.sPayload.sNtag.au8Key, sNfcNwkInstallCode.au8Key, NFC_NWK_PAYLOAD_KEY_SIZE) != 0
        ||  psNfcNwk->sTlv.sNdef.sPayload.sNtag.u16Crc      != sNfcNwkInstallCode.u16Crc
        ||  psNfcNwk->u8TlvTerminator                       != 0xfe)
        {
            /* Need to write data back to ntag */
            bWrite = TRUE;
        }
        else
        /* Non-network data correct ? */
        {
            /* Get extended pan id */
            uint64 u64ExtPanId = ZPS_u64AplZdoGetNetworkExtendedPanId();
            /* Got an extended pan id ? */
            if (u64ExtPanId != 0)
            {
                /* Any network data incorrect ? */
                if (psNfcNwk->sTlv.sNdef.sPayload.sNtag.u16ShortAddress != ZPS_u16AplZdoGetNwkAddr()
                ||  psNfcNwk->sTlv.sNdef.sPayload.sNtag.u8Channel       != ZPS_u8AplZdoGetRadioChannel()
                ||  psNfcNwk->sTlv.sNdef.sPayload.sNtag.u16PanId        != ZPS_u16AplZdoGetNetworkPanId()
                ||  psNfcNwk->sTlv.sNdef.sPayload.sNtag.u64ExtPanId     != u64ExtPanId)
                {
                    /* Need to write data back to ntag */
                    bWrite = TRUE;
                }
            }
            /* Not got an extended pan id ? */
            else
            {
                /* Any network data incorrect ? */
                if (psNfcNwk->sTlv.sNdef.sPayload.sNtag.u16ShortAddress != 0xffff
                ||  psNfcNwk->sTlv.sNdef.sPayload.sNtag.u8Channel       != 0
                ||  psNfcNwk->sTlv.sNdef.sPayload.sNtag.u16PanId        != 0
                ||  psNfcNwk->sTlv.sNdef.sPayload.sNtag.u64ExtPanId     != u64ExtPanId)
                {
                    /* Need to write data back to ntag */
                    bWrite = TRUE;
                }
            }
        }
    }

    return bWrite;
}

/****************************************************************************
 *
 * NAME: APP_vNtagNwkNfcNwkSet()
 *
 * DESCRIPTION:
 * Sets nfcnwk structure (sNtag.u8Sequence should be set on calling)
 *
 ****************************************************************************/
PRIVATE void APP_vNtagNwkNfcNwkSet(tsNfcNwk *psNfcNwk, uint8 u8Sequence)
{
    char                        acNdefType[NFC_NWK_NDEF_TYPE_LEN+1] = "com.nxp:JN51xx-NWK";
    ZPS_tsAplAfSimpleDescriptor  sDesc;

    /* Can not get simple descriptor ? */
    if (ZPS_eAplAfGetSimpleDescriptor(u8Endpoint, &sDesc) != E_ZCL_SUCCESS)
    {
        /* Use unknown device id */
        sDesc.u16DeviceId = 0xFFFF;
    }
    /* tlv */
    psNfcNwk->sTlv.u8TlvType   = 0x03;
    psNfcNwk->sTlv.u8TlvLength = sizeof(tsNfcNwkNdef);
    /* ndef */
    psNfcNwk->sTlv.sNdef.u8NdefFlags                        = 0xd4;
    psNfcNwk->sTlv.sNdef.u8NdefTypeLength                   = NFC_NWK_NDEF_TYPE_LEN;
    psNfcNwk->sTlv.sNdef.u8NdefPayloadLength                = sizeof(tsNfcNwkPayload);
    memcpy(psNfcNwk->sTlv.sNdef.au8NdefType,                  acNdefType, NFC_NWK_NDEF_TYPE_LEN);
    /* ntag */
    psNfcNwk->sTlv.sNdef.sPayload.sNtag.u8Version           = NFC_NWK_PAYLOAD_VERSION;
    psNfcNwk->sTlv.sNdef.sPayload.sNtag.u8Command           = NFC_CMD_NWK_NTAG_JOIN_WITH_CODE;
    psNfcNwk->sTlv.sNdef.sPayload.sNtag.u8Sequence          = u8Sequence;
    psNfcNwk->sTlv.sNdef.sPayload.sNtag.u16DeviceId         = sDesc.u16DeviceId;
    psNfcNwk->sTlv.sNdef.sPayload.sNtag.u64ExtAddress       = ZPS_u64AplZdoGetIeeeAddr();
    psNfcNwk->sTlv.sNdef.sPayload.sNtag.u64ExtPanId         = ZPS_u64AplZdoGetNetworkExtendedPanId();
    if (psNfcNwk->sTlv.sNdef.sPayload.sNtag.u64ExtPanId != 0)
    {
        psNfcNwk->sTlv.sNdef.sPayload.sNtag.u16ShortAddress = ZPS_u16AplZdoGetNwkAddr();
        psNfcNwk->sTlv.sNdef.sPayload.sNtag.u8Channel       = ZPS_u8AplZdoGetRadioChannel();
        psNfcNwk->sTlv.sNdef.sPayload.sNtag.u16PanId        = ZPS_u16AplZdoGetNetworkPanId();
    }
    else
    {
        psNfcNwk->sTlv.sNdef.sPayload.sNtag.u16ShortAddress = 0xffff;
        psNfcNwk->sTlv.sNdef.sPayload.sNtag.u8Channel       = 0;
        psNfcNwk->sTlv.sNdef.sPayload.sNtag.u16PanId        = 0;
    }

    memcpy(psNfcNwk->sTlv.sNdef.sPayload.sNtag.au8Key,        sNfcNwkInstallCode.au8Key, NFC_NWK_PAYLOAD_KEY_SIZE);
    psNfcNwk->sTlv.sNdef.sPayload.sNtag.u16Crc              = sNfcNwkInstallCode.u16Crc;
    /* nci */
    memset(&psNfcNwk->sTlv.sNdef.sPayload.sNci, 0, sizeof(tsNfcNwkNci));
    /* tlv terminator */
    psNfcNwk->u8TlvTerminator                               = 0xfe;
}

/****************************************************************************
 *
 * NAME: APP_vNtagNwkNfcNwkDebug
 *
 * DESCRIPTION:
 * Debugs NfcNwk structure
 *
 ****************************************************************************/
#if TRACE_APP_NTAG_DATA
PRIVATE void APP_vNtagNwkNfcNwkDebug(tsNfcNwk *psNfcNwk)
{
    uint8 u8Byte;

    /* tlv */
    DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "u8TlvType             = x%02x", psNfcNwk->sTlv.u8TlvType);
    DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "u8TlvLength           = %d",    psNfcNwk->sTlv.u8TlvLength);
    /* ndef */
    DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "u8NdefFlags           = x%02x", psNfcNwk->sTlv.sNdef.u8NdefFlags);
    DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "u8NdefTypeLength      = %d",    psNfcNwk->sTlv.sNdef.u8NdefTypeLength);
    DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "u8NdefPayloadLength   = %d",    psNfcNwk->sTlv.sNdef.u8NdefPayloadLength);
    DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "au8NdefType           = ");
    for (u8Byte = 0; u8Byte < NFC_NWK_NDEF_TYPE_LEN; u8Byte++) DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, "%c", psNfcNwk->sTlv.sNdef.au8NdefType[u8Byte]);
    /* ntag */
    DBG_vPrintf(TRACE_APP_NTAG_DATA, NFC_DBG_NL "sNtag.u8Version       = %d",    psNfcNwk->sTlv.sNdef.sPayload.sNtag.u8Version);
    DBG_vPrintf(TRACE_APP_NTAG_DATA, NFC_DBG_NL "sNtag.u8Command       = x%02x", psNfcNwk->sTlv.sNdef.sPayload.sNtag.u8Command);
    DBG_vPrintf(TRACE_APP_NTAG_DATA, NFC_DBG_NL "sNtag.u8Sequence      = %d",    psNfcNwk->sTlv.sNdef.sPayload.sNtag.u8Sequence);
    DBG_vPrintf(TRACE_APP_NTAG_DATA, NFC_DBG_NL "sNtag.u16DeviceId     = x%04x", psNfcNwk->sTlv.sNdef.sPayload.sNtag.u16DeviceId);
    DBG_vPrintf(TRACE_APP_NTAG_DATA, NFC_DBG_NL "sNtag.u64ExtAddress   = x%08x%08x", (uint32)(psNfcNwk->sTlv.sNdef.sPayload.sNtag.u64ExtAddress >> 32), (uint32)(psNfcNwk->sTlv.sNdef.sPayload.sNtag.u64ExtAddress & 0xFFFFFFFF));
    DBG_vPrintf(TRACE_APP_NTAG_DATA, NFC_DBG_NL "sNtag.u16ShortAddress = x%04x", psNfcNwk->sTlv.sNdef.sPayload.sNtag.u16ShortAddress);
    DBG_vPrintf(TRACE_APP_NTAG_DATA, NFC_DBG_NL "sNtag.u8Channel       = %d",    psNfcNwk->sTlv.sNdef.sPayload.sNtag.u8Channel);
    DBG_vPrintf(TRACE_APP_NTAG_DATA, NFC_DBG_NL "sNtag.u16PanId        = x%04x", psNfcNwk->sTlv.sNdef.sPayload.sNtag.u16PanId);
    DBG_vPrintf(TRACE_APP_NTAG_DATA, NFC_DBG_NL "sNtag.u64ExtPanId     = x%08x%08x", (uint32)(psNfcNwk->sTlv.sNdef.sPayload.sNtag.u64ExtPanId >> 32), (uint32)(psNfcNwk->sTlv.sNdef.sPayload.sNtag.u64ExtPanId & 0xFFFFFFFF));
    DBG_vPrintf(TRACE_APP_NTAG_DATA, NFC_DBG_NL "sNtag.au8Key          =");
    for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_KEY_SIZE; u8Byte++) DBG_vPrintf(TRACE_APP_NTAG_DATA, " %02x", psNfcNwk->sTlv.sNdef.sPayload.sNtag.au8Key[u8Byte]);
    DBG_vPrintf(TRACE_APP_NTAG_DATA, NFC_DBG_NL "sNtag.u16Crc          = x%04x", psNfcNwk->sTlv.sNdef.sPayload.sNtag.u16Crc);
    /* nci */
    DBG_vPrintf(TRACE_APP_NTAG_DATA, NFC_DBG_NL "sNci.u8Command        = x%02x", psNfcNwk->sTlv.sNdef.sPayload.sNci.u8Command);
    DBG_vPrintf(TRACE_APP_NTAG_DATA, NFC_DBG_NL "sNci.u8Sequence       = %d",    psNfcNwk->sTlv.sNdef.sPayload.sNci.u8Sequence);
    DBG_vPrintf(TRACE_APP_NTAG_DATA, NFC_DBG_NL "sNci.u16DeviceId      = x%04x", psNfcNwk->sTlv.sNdef.sPayload.sNci.u16DeviceId);
    DBG_vPrintf(TRACE_APP_NTAG_DATA, NFC_DBG_NL "sNci.u64ExtAddress    = x%08x%08x", (uint32)(psNfcNwk->sTlv.sNdef.sPayload.sNci.u64ExtAddress >> 32), (uint32)(psNfcNwk->sTlv.sNdef.sPayload.sNci.u64ExtAddress & 0xFFFFFFFF));
    DBG_vPrintf(TRACE_APP_NTAG_DATA, NFC_DBG_NL "sNci.u16ShortAddress  = x%04x", psNfcNwk->sTlv.sNdef.sPayload.sNci.u16ShortAddress);
    DBG_vPrintf(TRACE_APP_NTAG_DATA, NFC_DBG_NL "sNci.u8Channel        = %d",    psNfcNwk->sTlv.sNdef.sPayload.sNci.u8Channel);
    DBG_vPrintf(TRACE_APP_NTAG_DATA, NFC_DBG_NL "sNci.u16PanId         = x%04x", psNfcNwk->sTlv.sNdef.sPayload.sNci.u16PanId);
    DBG_vPrintf(TRACE_APP_NTAG_DATA, NFC_DBG_NL "sNci.u64ExtPanId      = x%08x%08x", (uint32)(psNfcNwk->sTlv.sNdef.sPayload.sNci.u64ExtPanId >> 32), (uint32)(psNfcNwk->sTlv.sNdef.sPayload.sNci.u64ExtPanId & 0xFFFFFFFF));
    DBG_vPrintf(TRACE_APP_NTAG_DATA, NFC_DBG_NL "sNci.au8Key           =");
    for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_KEY_SIZE; u8Byte++) DBG_vPrintf(TRACE_APP_NTAG_DATA, " %02x", psNfcNwk->sTlv.sNdef.sPayload.sNci.au8Key[u8Byte]);
    DBG_vPrintf(TRACE_APP_NTAG_DATA, NFC_DBG_NL "sNci.au8Mic           =");
    for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_MIC_SIZE; u8Byte++) DBG_vPrintf(TRACE_APP_NTAG_DATA, " %02x", psNfcNwk->sTlv.sNdef.sPayload.sNci.au8Mic[u8Byte]);
    DBG_vPrintf(TRACE_APP_NTAG_DATA, NFC_DBG_NL "sNci.u8KeySeqNum      = x%02x", psNfcNwk->sTlv.sNdef.sPayload.sNci.u8KeySeqNum);
    /* tlv terminator */
    DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "u8TlvTerminator       = x%02x", psNfcNwk->u8TlvTerminator);
}
#endif /* TRACE_APP_NTAG_DATA */
#endif /* APP_NTAG_NWK */

#if (defined APP_NTAG_OTA)
/****************************************************************************
 *
 * NAME: APP_vNtagOtaPdmLoadCmdImageNtag()
 *
 * DESCRIPTION:
 * Attempts to load cmd image structure from PDM
 * - if found validates it against running software
 * - not found or invalid builds new structure
 * - not written back here (only stored in pdm while download is in progress)
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void APP_vNtagOtaPdmLoadCmdImageNtag(void)
{
    uint32         u32StartMs;
    uint32         u32StopMs;
    bool_t           bResult;
//    uint8          au8Flash[NFC_OTA_MAX_HEADER_SIZE];
    tsNfcOtaZbHeader sNfcOtaZbHeader;
    uint16         u16PdmRead;
    uint8           u8NfcOtaCmdImageNtag = 0;
    PDM_teStatus     ePdmStatus;

    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: { APP_vNtagOtaPdmLoadCmdImage()", OSA_TimeGetMsec());
    /* Note start time */
    u32StartMs = OSA_TimeGetMsec();

    /* Nonce in flash is incorrect ? */
    uint8 au8CmpNonce[]  = { 0x00, 0x00, 0x00, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x00, 0x00, 0x00, 0x00 };
    if (0 != memcmp(au8CmpNonce, s_au8Nonce, 16))
    {
        /* Debug */
        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG_OTA: Nonce Invalid");
    }
    /* Nonce in flash is correct ? */
    else
    {
        /* Debug */
        DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "APP_NTAG_OTA: Nonce Valid");

        /* Link key in flash is incorrect ? */
        uint8 au8CmpLnkKey[] = { 0x5a, 0x69, 0x67, 0x42, 0x65, 0x65, 0x41, 0x6c, 0x6c, 0x69, 0x61, 0x6e, 0x63, 0x65, 0x30, 0x39 };
        if (0 != memcmp(au8CmpLnkKey, s_au8LnkKeyArray, 16))
        {
            /* Debug */
            DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG_OTA: Link Key Invalid");
        }
        /* Link key in flash is correct ? */
        else
        {
            /* Debug */
            DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "APP_NTAG_OTA: Link Key Valid");

            /* Attempt to  convert data from flash into OTA header (without image offset or checksum) ? */
            bResult = NFC_OTA_bZbHeaderFromBuffer(&sNfcOtaZbHeader, (uint32 *) NULL, (uint32 *) NULL, au8OtaHeader, NFC_OTA_MAX_HEADER_SIZE);
            /* Debug */
            DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "NFC_OTA_bZbHeaderFromBuffer() = %d", bResult);
            /* Success ? */
            if (bResult)
            {
                /* Attempt to read ota data */
                memset(&sNfcOtaCmdImageNtag, 0, sizeof(tsNfcOtaCmdImage));
                u16PdmRead = 0;
                ePdmStatus = PDM_eReadDataFromRecord(PDM_ID_APP_NFC_OTA,
                                                     &sNfcOtaCmdImageNtag,
                                                     sizeof(tsNfcOtaCmdImage),
                                                     &u16PdmRead);
                /* Debug */
                DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "PDM_eReadDataFromRecord(x%04x / PDM_ID_APP_NFC_OTA) = %d, %d, msec=%d", PDM_ID_APP_NFC_OTA, ePdmStatus, u16PdmRead, OSA_TimeGetMsec());
                /* Did not read data ? */
                if (sizeof(tsNfcOtaCmdImage) != u16PdmRead)
                {
                    /* Result is not valid data */
                    bResult = FALSE;
                }
                /* Read data ? */
                else
                {
                    /* Is the data we read invalid for our active image ? */
                    if (sNfcOtaCmdImageNtag.u8Cmd                != NFC_CMD_OTA_IMAGE
                    &&  sNfcOtaCmdImageNtag.u32FileIdentifier    != sNfcOtaZbHeader.u32FileIdentifier
                    &&  sNfcOtaCmdImageNtag.u16ManufacturerCode  != sNfcOtaZbHeader.u16ManufacturerCode
                    &&  sNfcOtaCmdImageNtag.u16ImageType         != sNfcOtaZbHeader.u16ImageType
                    &&  sNfcOtaCmdImageNtag.u16StackVersion      != sNfcOtaZbHeader.u16StackVersion
                    &&  memcmp(sNfcOtaCmdImageNtag.stHeaderString,  sNfcOtaZbHeader.stHeaderString, NFC_OTA_STRING_SIZE) != 0
                    &&  sNfcOtaCmdImageNtag.u32FileVersionActive != sNfcOtaZbHeader.u32FileVersion)
                    {
                        /* Result is not valid data */
                        bResult = FALSE;
                    }
                    /* Data is valid for our active image ? */
                    else
                    {
                        /* Is the inactive data invalid ? */
                        if (sNfcOtaCmdImageNtag.u32FileVersionInactive == sNfcOtaCmdImageNtag.u32FileVersionActive
                        ||  sNfcOtaCmdImageNtag.u32SavedImageInactive  >= sNfcOtaCmdImageNtag.u32TotalImageInactive)
                        {
                            /* Result is not valid data */
                            bResult = FALSE;
                        }
                        /* Data is valid */
                        else
                        {
                            /* Note we retrieved valid data from pdm */
                            u8NfcOtaCmdImageNtag = 2;
                        }
                    }
                }
                /* Don't have valid data in PDM ? */
                if (bResult == FALSE)
                {
                    /* Note we retrieved valid data from flash */
                    u8NfcOtaCmdImageNtag = 1;
                    /* Set flags and command */
                    sNfcOtaCmdImageNtag.u8Cmd                  = NFC_CMD_OTA_IMAGE;
                    sNfcOtaCmdImageNtag.u8Flags                = 0;
                    /* Copy the data into our packed structure */
                    sNfcOtaCmdImageNtag.u32FileIdentifier      = sNfcOtaZbHeader.u32FileIdentifier;
                    sNfcOtaCmdImageNtag.u16ManufacturerCode    = sNfcOtaZbHeader.u16ManufacturerCode;
                    sNfcOtaCmdImageNtag.u16ImageType           = sNfcOtaZbHeader.u16ImageType;
                    sNfcOtaCmdImageNtag.u16StackVersion        = sNfcOtaZbHeader.u16StackVersion;
                    memcpy(sNfcOtaCmdImageNtag.stHeaderString,   sNfcOtaZbHeader.stHeaderString, NFC_OTA_STRING_SIZE);
                    sNfcOtaCmdImageNtag.u32FileVersionActive   = sNfcOtaZbHeader.u32FileVersion;
                    /* Zero partial download info in our structure */
                    sNfcOtaCmdImageNtag.u32FileVersionInactive = 0;
                    sNfcOtaCmdImageNtag.u32TotalImageInactive  = 0;
                    sNfcOtaCmdImageNtag.u32SavedImageInactive  = 0;
                    sNfcOtaCmdImageNtag.u32OffsetImageInactive = 0xFFFFFFFF;
                }
            }
        }
    }
    /* Debugging ? */
    #if TRACE_APP_NTAG
    {
        /* Debug */
        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "u8NfcOtaCmdImageNtag = %d", u8NfcOtaCmdImageNtag);
        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "sNfcOtaCmdImageNtag:");
        APP_vNtagOtaCmdImageDebug(&sNfcOtaCmdImageNtag);
    }
    #endif
    /* Note end time */
    u32StopMs = OSA_TimeGetMsec();
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: } APP_vNtagOtaPdmLoadCmdImage() %dms", u32StopMs, u32StopMs - u32StartMs);
}

/****************************************************************************
 *
 * NAME: APP_bNtagOtaStateWrite()
 *
 * DESCRIPTION:
 * Timer handler for W_OTA state
 *
 ****************************************************************************/
PRIVATE bool_t APP_bNtagOtaStateWrite(uint8 *pu8TimerMs)
{
    uint32 u32StartMs;
    uint32 u32StopMs;
    bool_t   bResult = TRUE;
    bool_t   bFd;

    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "APP_NTAG %d: { APP_bNtagOtaStateWrite()", OSA_TimeGetMsec());
    /* Note start time */
    u32StartMs = OSA_TimeGetMsec();
    /* Use minimum timer period */
    *pu8TimerMs = NTAG_TIMER_OTA_MS;
    /* Read fd */
    bFd = NTAG_bReadFd();
    /* Are we outside field ? */
    if (bFd == TRUE)
    {
        /* Go idle */
        eAppNtagState = E_APP_NTAG_STATE_IDLE;
        /* Use minimum timer period */
        *pu8TimerMs = NTAG_TIMER_MIN_MS;
    }
    /* Still inside field */
    else
    {
        /* Attempt to read session NS register */
        bResult = NTAG_bReadReg((NTAG_u32Session() + NFC_OFFSET_NS_REG), 1, au8NtagData);
        /* Debug */
        DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "NTAG_bReadReg(NS_REG, 1) = %d {NS_REG = x%x}", bResult, au8NtagData[0]);
        /* Success ? */
        if (bResult)
        {
            /* Has the last NDEF block been read ? */
            if (au8NtagData[0] & NFC_MASK_NS_REG_NDEF_DATA_READ)
            {
                /* Set write reg mask: pthru and dir bits */
                au8NtagData[0] = 0x41;
                /* Set PTHRU on, direction out */
                au8NtagData[1] = 0x40;
                /* Attempt to write session NC register use as return value */
                bResult = NTAG_bWriteReg((NTAG_u32Session() + NFC_OFFSET_NC_REG), 2, au8NtagData);
                /* Debug */
                DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "NTAG_WriteReg(NC_REG, 2) = %d {NC_REG = x%x, x%x}", bResult, au8NtagData[0], au8NtagData[1]);
                /* Success ? */
                if (bResult)
                {
                    /* Convert ntag image structure to data */
                    NFC_OTA_vCmdImageToBuffer(&sNfcOtaCmdImageNtag, au8NtagData);
                    /* Attempt to write sram */
                    bResult = NTAG_bWrite(NTAG_u32Sram(), NFC_SRAM_SIZE, au8NtagData, NTAG_WRITE_BLOCK_SRAM_DELAY_US);
                    /* Debug */
                    DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "NTAG_bWrite(SRAM, %d) = %d", NFC_SRAM_SIZE, bResult);
                    /* Success ? */
                    if (bResult)
                    {
                        /* Go to poll state */
                        eAppNtagState = E_APP_NTAG_OTA_STATE_POLL;
                    }
                }
            }
        }
    }
    /* Note end time */
    u32StopMs = OSA_TimeGetMsec();
    /* Verbose debugging ? */
    #if TRACE_APP_NTAG_VERBOSE
    {
        /* Debug all calls */
        DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "APP_NTAG %d: } APP_bNtagOtaStateWrite() = %d %s %dms", u32StopMs, bResult, acAppNtagState[eAppNtagState], u32StopMs - u32StartMs);
    }
    /* Normal debugging ? */
    #elif TRACE_APP_NTAG
    {
        /* Debug final call */
        if (eAppNtagState != E_APP_NTAG_OTA_STATE_WRITE) DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: } APP_bNtagOtaStateWrite() = %d %s %dms", u32StopMs, bResult, acAppNtagState[eAppNtagState], u32StopMs - u32StartMs);
    }
    #endif

    return bResult;
}

/****************************************************************************
 *
 * NAME: APP_bNtagOtaStatePoll()
 *
 * DESCRIPTION:
 * Timer handler for P_OTA state
 *
 ****************************************************************************/
PRIVATE bool_t APP_bNtagOtaStatePoll(uint8 *pu8TimerMs)
{
    uint32 u32StartMs;
    uint32 u32StopMs;
    bool_t   bResult = TRUE;
    bool_t   bFd;

    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "APP_NTAG %d: { APP_bNtagOtaStatePoll()", OSA_TimeGetMsec());
    /* Note start time */
    u32StartMs = OSA_TimeGetMsec();
    /* Use minimum timer period */
    *pu8TimerMs = NTAG_TIMER_OTA_MS;
    /* Read fd */
    bFd = NTAG_bReadFd();
    /* Are we outside field ? */
    if (bFd == TRUE)
    {
        /* Go idle */
        eAppNtagState = E_APP_NTAG_STATE_IDLE;
        /* Use minimum timer period */
        *pu8TimerMs = NTAG_TIMER_MIN_MS;
    }
    /* Still inside field */
    else
    {
        /* Attempt to read session NS register */
        bResult = NTAG_bReadReg((NTAG_u32Session() + NFC_OFFSET_NS_REG), 1, au8NtagData);
        /* Debug */
        DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "NTAG_bReadReg(NS_REG, 1) = %d {NS_REG = x%x}", bResult, au8NtagData[0]);
        /* Success ? */
        if (bResult)
        {
            /* Has the sram data been read by the RF side ? */
            if ((au8NtagData[0] & NFC_MASK_NS_REG_SRAM_RF_READY) == 0)
            {
                /* Set write reg mask: pthru and dir bits */
                au8NtagData[0] = 0x41;
                /* Set PTHRU on, direction in */
                au8NtagData[1] = 0x41;
                /* Attempt to write session NC register use as return value */
                bResult = NTAG_bWriteReg((NTAG_u32Session() + NFC_OFFSET_NC_REG), 2, au8NtagData);
                /* Debug */
                DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "NTAG_WriteReg(NC_REG, 2) = %d {NC_REG = x%x, x%x}", bResult, au8NtagData[0], au8NtagData[1]);
                /* Success ? */
                if (bResult)
                {
                    /* Go to read state */
                    eAppNtagState = E_APP_NTAG_OTA_STATE_READ;
                }
            }
        }
    }
    /* Note end time */
    u32StopMs = OSA_TimeGetMsec();
    /* Verbose debugging ? */
    #if TRACE_APP_NTAG_VERBOSE
    {
        /* Debug all calls */
        DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "APP_NTAG %d: } APP_bNtagOtaStatePoll() = %d %s %dms", u32StopMs, bResult, acAppNtagState[eAppNtagState], u32StopMs - u32StartMs);
    }
    /* Normal debugging ? */
    #elif TRACE_APP_NTAG
    {
        /* Debug final call */
        if (eAppNtagState != E_APP_NTAG_OTA_STATE_POLL) DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: } APP_bNtagOtaStatePoll() = %d %s %dms", u32StopMs, bResult, acAppNtagState[eAppNtagState], u32StopMs - u32StartMs);
    }
    #endif

    return bResult;
}

/****************************************************************************
 *
 * NAME: APP_bNtagOtaStateRead()
 *
 * DESCRIPTION:
 * Timer handler for R_OTA state
 *
 ****************************************************************************/
PRIVATE bool_t APP_bNtagOtaStateRead(uint8 *pu8TimerMs)
{
    uint32 u32StartMs;
    uint32 u32StopMs;
    bool_t   bResult = TRUE;
    bool_t   bDownload = FALSE;
    bool_t   bFd;

    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "APP_NTAG %d: { APP_bNtagOtaStateRead()", OSA_TimeGetMsec());
    /* Note start time */
    u32StartMs = OSA_TimeGetMsec();
    /* Use minimum timer period */
    *pu8TimerMs = NTAG_TIMER_OTA_MS;
    /* Read fd */
    bFd = NTAG_bReadFd();
    /* Are we outside field ? */
    if (bFd == TRUE)
    {
        /* Go idle */
        eAppNtagState = E_APP_NTAG_STATE_IDLE;
        /* Use minimum timer period */
        *pu8TimerMs = NTAG_TIMER_MIN_MS;
        /* Debug */
        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "OTA_READ -> IDLE (FD)");
    }
    /* Still inside field */
    else
    {
        /* Attempt to read session NS register */
        bResult = NTAG_bReadReg((NTAG_u32Session() + NFC_OFFSET_NS_REG), 1, au8NtagData);
        /* Debug */
        DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "NTAG_bReadReg(NS_REG, 1) = %d {NS_REG = x%x}", bResult, au8NtagData[0]);
        /* Success ? */
        if (bResult)
        {
            /* Has the sram data been written by the RF side ? */
            if ((au8NtagData[0] & NFC_MASK_NS_REG_SRAM_I2C_READY) != 0)
            {
                memset(au8NtagData, 0xaa, NFC_SRAM_SIZE);
                /* Attempt to read sram */
                bResult = NTAG_bRead(NTAG_u32Sram(), NFC_SRAM_SIZE, au8NtagData);
                /* Debug */
                DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "NTAG_bRead(SRAM, %d) = %d", NFC_SRAM_SIZE, bResult);
                /* Success ? */
                if (bResult)
                {
                    /* Is this an ota image command ? */
                    if ((au8NtagData[0] & 0x7F) == NFC_CMD_OTA_IMAGE)
                    {
                        tsNfcOtaCmdImage sNfcOtaCmdImageNci;

                        /* Convert to structure */
                        NFC_OTA_vCmdImageFromBuffer(&sNfcOtaCmdImageNci, au8NtagData);
                        /* Process structure */
                        bDownload = APP_bNtagOtaCmdImageProcess(&sNfcOtaCmdImageNci);
                        /* Debug */
                        if (bDownload == FALSE) DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "OTA_READ -> IDLE (Image)");
                    }
                    /* Is this ota data ? */
                    else if ((au8NtagData[0] & 0x7F) == NFC_CMD_OTA_DATA)
                    {
                        tsNfcOtaCmdData sNfcOtaCmdData;

                        /* Convert to structure */
                        NFC_OTA_vCmdDataFromBuffer(&sNfcOtaCmdData, au8NtagData);
                        /* Process structure */
                        bDownload = APP_bNtagOtaCmdDataProcess(&sNfcOtaCmdData);
                        /* Debug */
                        if (bDownload == FALSE) DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "OTA_READ -> IDLE (Data)");
                    }
                    /* Is this not ota data ? */
                    else
                    {
                        /* Debug */
                        if (bDownload == FALSE) DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "OTA_READ -> IDLE (Cmd = x%02x)", au8NtagData[0]);
                    }
                    /* Not downloading ? */
                    if (bDownload == FALSE)
                    {
                        /* Go to idle state */
                        eAppNtagState = E_APP_NTAG_STATE_IDLE;
                        /* Use minimum timer period */
                        *pu8TimerMs = NTAG_TIMER_MIN_MS;
                    }
                }
            }
        }
    }
    /* Note end time */
    u32StopMs = OSA_TimeGetMsec();
    /* Verbose debugging ? */
    #if TRACE_APP_NTAG_VERBOSE
    {
        /* Debug all calls */
        DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "APP_NTAG %d: } APP_bNtagOtaStateRead() = %d %s %dms", u32StopMs, bResult, acAppNtagState[eAppNtagState], u32StopMs - u32StartMs);
    }
    /* Normal debugging ? */
    #elif TRACE_APP_NTAG
    {
        /* Debug final call */
        if (eAppNtagState != E_APP_NTAG_OTA_STATE_READ) DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: } APP_bNtagOtaStateRead() = %d %s %dms", u32StopMs, bResult, acAppNtagState[eAppNtagState], u32StopMs - u32StartMs);
    }
    #endif

    return bResult;
}

/****************************************************************************
 *
 * NAME: APP_bNtagOtaCmdImageProcess
 *
 * DESCRIPTION:
 * Processes CmdImage structure
 *
 * RETURNS:
 * TRUE  - if downloading
 * FALSE - if not downloading
 *
 ****************************************************************************/
PRIVATE bool_t APP_bNtagOtaCmdImageProcess(tsNfcOtaCmdImage *psNfcOtaCmdImage)
{
    bool_t bDownload = FALSE;

    /* Is there more data to read ? */
    if (psNfcOtaCmdImage->u8Cmd & NFC_CMD_OTA_FLAG_MORE)
    {
        /* Is this a valid image to download ? */
        if (sNfcOtaCmdImageNtag.u32FileIdentifier   == psNfcOtaCmdImage->u32FileIdentifier
        &&  sNfcOtaCmdImageNtag.u16ManufacturerCode == psNfcOtaCmdImage->u16ManufacturerCode
        &&  sNfcOtaCmdImageNtag.u16ImageType        == psNfcOtaCmdImage->u16ImageType
        &&  sNfcOtaCmdImageNtag.u16StackVersion     == psNfcOtaCmdImage->u16StackVersion
        &&  memcmp(sNfcOtaCmdImageNtag.stHeaderString, psNfcOtaCmdImage->stHeaderString, NFC_OTA_STRING_SIZE) == 0
        &&  0                                       <  psNfcOtaCmdImage->u32TotalImageInactive)
        {
            /* Is this a continuation of the image we are already downloading ? */
            if (sNfcOtaCmdImageNtag.u32FileVersionInactive == psNfcOtaCmdImage->u32FileVersionInactive
            &&  sNfcOtaCmdImageNtag.u32TotalImageInactive  == psNfcOtaCmdImage->u32TotalImageInactive
            &&  sNfcOtaCmdImageNtag.u32SavedImageInactive   > 0
            &&  sNfcOtaCmdImageNtag.u32SavedImageInactive   < psNfcOtaCmdImage->u32TotalImageInactive)
            {
                /* Going to download */
                bDownload = TRUE;
            }
            /* Is this a different image to the image we are currently running ? */
            else if (sNfcOtaCmdImageNtag.u32FileVersionActive != psNfcOtaCmdImage->u32FileVersionInactive)
            {
                /* Going to download */
                bDownload = TRUE;
                /* Note our inactive version */
                sNfcOtaCmdImageNtag.u32FileVersionInactive = psNfcOtaCmdImage->u32FileVersionInactive;
                sNfcOtaCmdImageNtag.u32TotalImageInactive  = psNfcOtaCmdImage->u32TotalImageInactive;
                sNfcOtaCmdImageNtag.u32SavedImageInactive  = 0;
            }
        }
    }
    /* Debugging ? */
    #if TRACE_APP_NTAG
    {
        /* Debug */
        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: } APP_bNtagOtaCmdImageProcess() = %d", OSA_TimeGetMsec(), bDownload);
        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "sNfcOtaCmdImageNci:");
        APP_vNtagOtaCmdImageDebug(psNfcOtaCmdImage);
    }
    #endif
    /* Download ? */
    if (bDownload)
    {
        /* Reset buffer */
        memset(au8FlashBuffer, 0xFF, FLASH_PAGE_SIZE);
        /* Reset amount of data in buffer */
        u32FlashBuffer = 0;
        /* Invalidate percentage */
        u32OtaPercent = 0xFFFFFFFF;
        /* Zero amount of data saved to flash */
        u32FlashPdm = 0;
        /* Note time of ota start */
        u32AppNtagOtaMs = OSA_TimeGetMsec();
    }

    return bDownload;
}

/****************************************************************************
 *
 * NAME: APP_bNtagOtaCmdDataProcess
 *
 * DESCRIPTION:
 * Processes CmdData structure
 *
 * RETURNS:
 * TRUE  - if downloading
 * FALSE - if not downloading
 *
 ****************************************************************************/
PRIVATE bool_t APP_bNtagOtaCmdDataProcess(tsNfcOtaCmdData *psNfcOtaCmdData)
{
    bool_t   bDownload = TRUE;
    uint32 u32Copy;
    uint32 u32Progress;
    uint32 u32Percent;
    uint32 u32StopMs;

    /* Will this packet overflow the buffer ? */
    if (u32FlashBuffer + NFC_OTA_CMD_DATA_DATA_SIZE > FLASH_PAGE_SIZE)
    {
        /* Calculate how much data to transfer */
        u32Copy = FLASH_PAGE_SIZE - u32FlashBuffer;
    }
    else
    {
        /* Transfer all the data */
        u32Copy = NFC_OTA_CMD_DATA_DATA_SIZE;
    }
    /* Transfer the data */
    memcpy(&au8FlashBuffer[u32FlashBuffer], psNfcOtaCmdData->au8Data, u32Copy);
    /* Update amount of data in buffer */
    u32FlashBuffer += u32Copy;
    /* Got an almost full buffer and not calculated the offset yet ? */
    if (u32FlashBuffer >= FLASH_PAGE_SIZE - NFC_OTA_CMD_DATA_DATA_SIZE
    &&  sNfcOtaCmdImageNtag.u32OffsetImageInactive == 0xFFFFFFFF)
    {
        bool_t           bResult;
        tsNfcOtaZbHeader sNfcOtaZbHeader;
        uint32         u32ImageOffset;

        /* Attempt to  convert data from flash buffer into OTA header (and find image offset, no checksum) ? */
        bResult = NFC_OTA_bZbHeaderFromBuffer(&sNfcOtaZbHeader, &u32ImageOffset, (uint32 *) NULL, au8FlashBuffer, u32FlashBuffer);
        /* Success ? */
        if (bResult)
        {
            /* Expected data in ZB OTA header and valid image offset ? */
            if (sNfcOtaZbHeader.u32FileIdentifier      == sNfcOtaCmdImageNtag.u32FileIdentifier
            &&  sNfcOtaZbHeader.u16ManufacturerCode    == sNfcOtaCmdImageNtag.u16ManufacturerCode
            &&  sNfcOtaZbHeader.u16ImageType           == sNfcOtaCmdImageNtag.u16ImageType
            &&  sNfcOtaZbHeader.u16StackVersion        == sNfcOtaCmdImageNtag.u16StackVersion
            &&  memcmp(sNfcOtaZbHeader.stHeaderString,    sNfcOtaCmdImageNtag.stHeaderString, NFC_OTA_STRING_SIZE) == 0
            &&  sNfcOtaZbHeader.u32FileVersion         == sNfcOtaCmdImageNtag.u32FileVersionInactive
            &&  sNfcOtaZbHeader.u32TotalImage          == sNfcOtaCmdImageNtag.u32TotalImageInactive
            &&  u32ImageOffset                         != 0xFFFFFFFF)
            {
                uint32 u32Byte;

                /* Note the image offset */
                sNfcOtaCmdImageNtag.u32OffsetImageInactive = u32ImageOffset;
                /* Pretend we've saved this much data */
                sNfcOtaCmdImageNtag.u32SavedImageInactive = sNfcOtaCmdImageNtag.u32OffsetImageInactive;
                /* Shuffle the image up in the buffer */
                for (u32Byte = sNfcOtaCmdImageNtag.u32OffsetImageInactive;
                     u32Byte < u32FlashBuffer;
                     u32Byte++)
                {
                    /* Move byte */
                    au8FlashBuffer[u32Byte - sNfcOtaCmdImageNtag.u32OffsetImageInactive] = au8FlashBuffer[u32Byte];
                }
                /* Reduce bytes in flash buffer */
                u32FlashBuffer -= sNfcOtaCmdImageNtag.u32OffsetImageInactive;
            }
        }
    }
    /* Got a full buffer or this is the last data packet ? */
    if ((u32FlashBuffer == FLASH_PAGE_SIZE)
    ||  (psNfcOtaCmdData->u8Cmd & NFC_CMD_OTA_FLAG_MORE) == 0)
    {
        /* Write buffer to flash */
        APP_bNtagOtaWriteFlashBuffer();
        /* Update end time */
        u32StopMs = OSA_TimeGetMsec();
    }
    /* Is there more data to go in the buffer ? */
    if (u32Copy < NFC_OTA_CMD_DATA_DATA_SIZE)
    {
        uint32 u32Tail;

        /* Calculate how much needs to go in the flash buffer */
        u32Tail = NFC_OTA_CMD_DATA_DATA_SIZE - u32Copy;
        if (u32FlashBuffer + u32Tail <= sizeof (au8FlashBuffer))
        {
            /* Transfer the data */
            memcpy(&au8FlashBuffer[u32FlashBuffer], &psNfcOtaCmdData->au8Data[u32Copy], u32Tail);
            /* Update amount of data in buffer */
            u32FlashBuffer += u32Tail;
        }
        /* No more data ? */
        if ((psNfcOtaCmdData->u8Cmd & NFC_CMD_OTA_FLAG_MORE) == 0)
        {
            /* Write buffer to flash */
            APP_bNtagOtaWriteFlashBuffer();
            /* Update end time */
            u32StopMs = OSA_TimeGetMsec();
        }
    }
    /* Note end time */
    u32StopMs = OSA_TimeGetMsec();
    /* Calculate progress */
    u32Progress = psNfcOtaCmdData->u32Offset + NFC_OTA_CMD_DATA_DATA_SIZE;
    /* Calculate percentage */
    u32Percent = ((u32Progress * 100) / sNfcOtaCmdImageNtag.u32TotalImageInactive);
    /* Is the percentage different to the last reported percentage and on a 5% boundary
     * or this is the last packet ? */
    if ((u32Percent != u32OtaPercent && (u32Percent % 10) == 0)
    ||  (psNfcOtaCmdData->u8Cmd & NFC_CMD_OTA_FLAG_MORE)  == 0)
    {
        uint32 u32OtaS;

        /* Remember reported percentage */
        u32OtaPercent = u32Percent;
        /* Calculate number of seconds since we started */
        u32OtaS = (u32StopMs - u32AppNtagOtaMs) / 1000;
        /* Debug */
        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: u32OtaNfcPercent = %d%% %ds", u32StopMs, u32OtaPercent, u32OtaS);
        /* Got some seconds ? */
        if (u32OtaS > 0)
        {
            /* Debug */
            DBG_vPrintf(TRACE_APP_NTAG, " %db/s", u32Progress/u32OtaS);
        }
    }
    /* No more data to read or download is complete ? */
    if ((psNfcOtaCmdData->u8Cmd & NFC_CMD_OTA_FLAG_MORE) == 0
    ||   u32Progress >= sNfcOtaCmdImageNtag.u32TotalImageInactive)
    {
        /* Stop downloading */
        bDownload = FALSE;
        /* Delete progress */
        PDM_vDeleteDataRecord(PDM_ID_APP_NFC_OTA);
        /* Reset on absent */
        bOtaChangeOnAbsent = TRUE;
    }

    return bDownload;
}

/****************************************************************************
 *
 * NAME: void APP_vNtagOtaWriteBuffer
 *
 * DESCRIPTION:
 * Writes buffer to flash
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE bool_t APP_bNtagOtaWriteFlashBuffer(void)
{
    bool_t bReturn = FALSE;

    /* Got a valid offset ? */
    if (sNfcOtaCmdImageNtag.u32OffsetImageInactive != 0xFFFFFFFF)
    {
        uint32 u32FlashAddress;

        /* Calculate flash address */
        u32FlashAddress = FLASH_INACTIVE_START + sNfcOtaCmdImageNtag.u32SavedImageInactive - sNfcOtaCmdImageNtag.u32OffsetImageInactive;
        /* Address is on a page boundary ? */
        if (u32FlashAddress % FLASH_PAGE_SIZE == 0)
        {
            uint32 u32FlashPage;
            bool_t   bFlash;

            /* Calculate page */
            u32FlashPage = u32FlashAddress / FLASH_PAGE_SIZE;
            /* First page ? */
            if (sNfcOtaCmdImageNtag.u32SavedImageInactive == sNfcOtaCmdImageNtag.u32OffsetImageInactive)
            {
                /* Invalidate data before writing to flash */
                APP_vNtagOtaXorBuffer(au8FlashBuffer, NFC_OTA_BOOTLOADER_HEADER_SIZE);
            }
            /* Debug */
            DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "NFC_OTA_bFlashWritePage(%d) {x%x}", u32FlashPage, u32FlashAddress);
            /* Write page to flash */
            bFlash = NFC_OTA_bFlashWritePage(au8FlashBuffer, u32FlashPage);
            /* Debug */
            DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, " = %d", bFlash);
            /* Success */
            if (bFlash)
            {
                /* Update saved image */
                sNfcOtaCmdImageNtag.u32SavedImageInactive += FLASH_PAGE_SIZE;
                /* Update amount of data saved to flash since last pdm write */
                u32FlashPdm += FLASH_PAGE_SIZE;
                /* Write progress to PDM ? */
                if (u32FlashPdm >= FLASH_PDM_SAVE)
                {
                    PDM_teStatus ePdmStatus;

                    /* Save progress */
                    ePdmStatus = PDM_eSaveRecordData(PDM_ID_APP_NFC_OTA,
                                                     &sNfcOtaCmdImageNtag,
                                                     sizeof(tsNfcOtaCmdImage));
                    /* Debug */
                    DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "PDM_eSaveRecordData(x%04x / PDM_ID_APP_NFC_OTA) = %d, u32SavedImageInactive=%d", PDM_ID_APP_NFC_OTA, ePdmStatus, sNfcOtaCmdImageNtag.u32SavedImageInactive);
                    /* Reset amount of data saved to flash since last pdm write */
                    u32FlashPdm = 0;
                }
                /* Success */
                bReturn = TRUE;
            }
            /* Reset buffer */
            memset(au8FlashBuffer, 0xFF, FLASH_PAGE_SIZE);
            /* Reset amount of data in buffer */
            u32FlashBuffer = 0;
        }
        /* Not on page boundary ? */
        else
        {
            /* Something is wrong! */
            ;
        }
    }
    /* Not got offset ? */
    else
    {
        /* Something is wrong */
        ;
    }

    return bReturn;
}

/****************************************************************************
 *
 * NAME: void APP_vNtagOtaXorBuffer
 *
 * DESCRIPTION:
 * Xors each byte with 0xFB to validate/invalidate data
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void APP_vNtagOtaXorBuffer(uint8           *pu8Buffer,         /* Data buffer */
                                   uint32           u32Length)         /* Length of data */
{
    uint32 u32Byte;

    for (u32Byte = 0; u32Byte < u32Length; u32Byte++)
    {
        pu8Buffer[u32Byte] ^= 0xFB;
    }
}

/****************************************************************************
 *
 * NAME: APP_bNtagOtaImageValid
 *
 * DESCRIPTION:
 * Checks inactive image for validity
 *
 * RETURNS:
 * Validity
 *
 ****************************************************************************/
PRIVATE bool_t APP_bNtagOtaImageValid(void)
{
    bool_t          bReturn = FALSE;
    bool_t          bResult;
    uint32        u32ReadChecksum;
    uint32        u32CalcChecksum = -1;
    uint32        u32Byte;
    uint32        u32Bytes;
    uint32        u32FlashAddress = FLASH_INACTIVE_START;
    uint32        u32StartMs;
    uint32        u32StopMs;

    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: { APP_bNtagOtaImageValid()", OSA_TimeGetMsec());
    /* Note start time */
    u32StartMs = OSA_TimeGetMsec();
    /* Copy the first page back into the flash buffer */
    bResult = NFC_OTA_bFlashReadBytes(au8FlashBuffer, u32FlashAddress, FLASH_PAGE_SIZE);
    /* Read data ? */
    if (bResult)
    {
        tsNfcOtaZbHeader sNfcOtaZbHeader;

        /* Debug */
        DBG_vPrintf(TRACE_APP_NTAG, " FLASH-OK");
        /* Validate data */
        APP_vNtagOtaXorBuffer(au8FlashBuffer, NFC_OTA_BOOTLOADER_HEADER_SIZE);
        /* Extract header and checksum */
        bResult = NFC_OTA_bZbHeaderFromBuffer(&sNfcOtaZbHeader,                               /* OTA header structure to populate */
                                            (uint32 *) NULL,                                /* OTA image offset for .ota header (NULL to skip) */
                                            &u32ReadChecksum,                               /* OTA checksum for .bin header (NULL to skip) */
                                            &au8FlashBuffer[NFC_OTA_BIN_HEADER_OFFSET],     /* Data buffer */
                                            (FLASH_PAGE_SIZE - NFC_OTA_BIN_HEADER_OFFSET)); /* Length of data */
        /* Does the image match what we were expecting ? */
        if (NFC_OTA_FILE_IDENTIFIER                    == sNfcOtaZbHeader.u32FileIdentifier
        &&  sNfcOtaCmdImageNtag.u32FileIdentifier      == sNfcOtaZbHeader.u32FileIdentifier
        &&  sNfcOtaCmdImageNtag.u16ManufacturerCode    == sNfcOtaZbHeader.u16ManufacturerCode
        &&  sNfcOtaCmdImageNtag.u16ImageType           == sNfcOtaZbHeader.u16ImageType
        &&  sNfcOtaCmdImageNtag.u16StackVersion        == sNfcOtaZbHeader.u16StackVersion
        &&  sNfcOtaCmdImageNtag.u32FileVersionInactive == sNfcOtaZbHeader.u32FileVersion
        &&  memcmp(sNfcOtaCmdImageNtag.stHeaderString,    sNfcOtaZbHeader.stHeaderString, NFC_OTA_STRING_SIZE) == 0
        &&  sNfcOtaCmdImageNtag.u32TotalImageInactive  == sNfcOtaZbHeader.u32TotalImage
        &&  sNfcOtaCmdImageNtag.u32SavedImageInactive  >= sNfcOtaZbHeader.u32TotalImage)
        {
            /* Debug */
            DBG_vPrintf(TRACE_APP_NTAG, " IMAGE-OK");
            /* Zero checksum in buffer for calculating calculated checksum */
            memset(&au8FlashBuffer[NFC_OTA_BIN_HEADER_OFFSET+NFC_OTA_BIN_CHECKSUM_OFFSET], 0, sizeof(uint32));
            /* Calculate number of bytes in image */
            u32Bytes = sNfcOtaCmdImageNtag.u32TotalImageInactive - sNfcOtaCmdImageNtag.u32OffsetImageInactive;
            /* Start at first byte */
            u32Byte = 0;
            /* Loop through image (a page at a time) */
            while (u32Byte < u32Bytes)
            {
                uint32 u32PageByte;

                /* Loop through bytes in page */
                for (u32PageByte = 0; u32PageByte < FLASH_PAGE_SIZE; u32PageByte++, u32Byte++)
                {
                    /* Byte within image - update checksum (ignore otherwise) */
                    if (u32Byte < u32Bytes) ZPS_vRunningCRC32(au8FlashBuffer[u32PageByte], &u32CalcChecksum);
                }
                /* Need to read another page ? */
                if (u32Byte < u32Bytes)
                {
#ifdef WATCHDOG_ALLOWED
                    /* Refresh watchdog */
                    WWDT_Refresh(WWDT);
#endif
                    /* Increment flash address */
                    u32FlashAddress += FLASH_PAGE_SIZE;
                    /* Copy the first page back into the flash buffer */
                    bResult = NFC_OTA_bFlashReadBytes(au8FlashBuffer, u32FlashAddress, FLASH_PAGE_SIZE);
                }
            }
            /* Finalise checksum */
            ZPS_vFinalCrc32(&u32CalcChecksum);
            /* Checksums match ? */
            if (u32ReadChecksum == u32CalcChecksum)
            {
                /* Debug */
                DBG_vPrintf(TRACE_APP_NTAG, " CRC-OK");
                /* Success */
                bReturn = TRUE;
            }
            else
            {
                /* Debug */
                DBG_vPrintf(TRACE_APP_NTAG, " CRC-BAD");
            }
        }
        /* Unexpected image */
        else
        {
            /* Debug */
            DBG_vPrintf(TRACE_APP_NTAG, " IMAGE-BAD");
        }
    }
    /* Flash not read ? */
    else
    {
        /* Debug */
        DBG_vPrintf(TRACE_APP_NTAG, " FLASH-BAD");
    }
    /* Note end time */
    u32StopMs = OSA_TimeGetMsec();
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: } APP_bNtagOtaImageValid() = %d %dms", u32StopMs, bReturn, u32StopMs - u32StartMs);

    return bReturn;
}

/****************************************************************************
 *
 * NAME: void APP_vNtagOtaImageChange
 *
 * DESCRIPTION:
 * Swaps to run inactive image
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE void APP_vNtagOtaImageChange(void)
{
    uint32 u32FlashAddress;
    uint32 u32FlashPage;
    bool_t   bFlash;
    uint32 u32StartMs;
    uint32 u32StopMs;

    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "APP_NTAG %d: { APP_vNtagOtaImageChange()", OSA_TimeGetMsec());
    /* Note start time */
    u32StartMs = OSA_TimeGetMsec();
    /* Flash address */
    u32FlashAddress = FLASH_INACTIVE_START;
    /* Get page */
    u32FlashPage = u32FlashAddress / FLASH_PAGE_SIZE;
    /* Copy the first page back into the flash buffer */
    bFlash = NFC_OTA_bFlashReadBytes(au8FlashBuffer, u32FlashAddress, FLASH_PAGE_SIZE);
    /* Validate data before writing back to flash */
    APP_vNtagOtaXorBuffer(au8FlashBuffer, NFC_OTA_BOOTLOADER_HEADER_SIZE);
    /* Write page to flash */
    bFlash = NFC_OTA_bFlashWritePage(au8FlashBuffer, u32FlashPage);
    /* First sector now validated ? */
    if (bFlash)
    {
        /* Debug */
        DBG_vPrintf(TRACE_APP_NTAG, " FLASH-OK");
        /* Swap images */
        psector_SetEscoreImageData(FLASH_INACTIVE_START, 0);
        __disable_irq();
        SYSCON->MEMORYREMAP &= ~( SYSCON_MEMORYREMAP_MAP_MASK << SYSCON_MEMORYREMAP_MAP_SHIFT);
        PMC->CTRL &= ~(PMC_CTRL_LPMODE_MASK << PMC_CTRL_LPMODE_SHIFT);
        /* JN518x ? */
        #if (JENNIC_CHIP_FAMILY == JN518x)
        {
            /* Debug */
            DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "RESET_SystemReset()");
            /* Reset */
            MICRO_DISABLE_INTERRUPTS();
            RESET_SystemReset();
        }
        /* JN516x / JN517x ? */
        #else
        {
            /* Debug */
            DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "vAHI_SwReset()");
            /* Reset */
            vAHI_SwReset();
        }
        #endif
    }
    /* Flash not ok ? */
    else
    {
        /* Debug */
        DBG_vPrintf(TRACE_APP_NTAG, " FLASH-BAD");
    }
    /* Note end time */
    u32StopMs = OSA_TimeGetMsec();
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG_VERBOSE, NFC_DBG_NL "APP_NTAG %d: } APP_vNtagOtaImageChange() %dms", u32StopMs, u32StopMs - u32StartMs);
}

#if (TRACE_APP_NTAG||TRACE_APP_NTAG_VERBOSE)
/****************************************************************************
 *
 * NAME: APP_vNtagOtaCmdImageDebug
 *
 * DESCRIPTION:
 * Debugs NfcNwk structure
 *
 ****************************************************************************/
PRIVATE void APP_vNtagOtaCmdImageDebug(tsNfcOtaCmdImage *psNfcOtaCmdImage)
{
    char acHeaderString[NFC_OTA_STRING_SIZE+1];

    /* Copy string and terminate */
    memcpy(acHeaderString, psNfcOtaCmdImage->stHeaderString, NFC_OTA_STRING_SIZE);
    acHeaderString[NFC_OTA_STRING_SIZE] = '\0';
    /* Debug */
    DBG_vPrintf((TRACE_APP_NTAG||TRACE_APP_NTAG_VERBOSE), NFC_DBG_NL "u8Cmd                  = x%02x",       psNfcOtaCmdImage->u8Cmd                 );
    DBG_vPrintf((TRACE_APP_NTAG||TRACE_APP_NTAG_VERBOSE), NFC_DBG_NL "u8Flags                = x%02x",       psNfcOtaCmdImage->u8Flags               );
    DBG_vPrintf((TRACE_APP_NTAG||TRACE_APP_NTAG_VERBOSE), NFC_DBG_NL "u32FileIdentifier      = x%08x",       psNfcOtaCmdImage->u32FileIdentifier     );
    DBG_vPrintf((TRACE_APP_NTAG||TRACE_APP_NTAG_VERBOSE), NFC_DBG_NL "u16ManufacturerCode    = x%04x",       psNfcOtaCmdImage->u16ManufacturerCode   );
    DBG_vPrintf((TRACE_APP_NTAG||TRACE_APP_NTAG_VERBOSE), NFC_DBG_NL "u16ImageType           = x%04x",       psNfcOtaCmdImage->u16ImageType          );
    DBG_vPrintf((TRACE_APP_NTAG||TRACE_APP_NTAG_VERBOSE), NFC_DBG_NL "u16StackVersion        = x%04x",       psNfcOtaCmdImage->u16StackVersion       );
    DBG_vPrintf((TRACE_APP_NTAG||TRACE_APP_NTAG_VERBOSE), NFC_DBG_NL "stHeaderString         = %s",          acHeaderString                          );
    DBG_vPrintf((TRACE_APP_NTAG||TRACE_APP_NTAG_VERBOSE), NFC_DBG_NL "u32FileVersionActive   = x%08x",       psNfcOtaCmdImage->u32FileVersionActive  );
    DBG_vPrintf((TRACE_APP_NTAG||TRACE_APP_NTAG_VERBOSE), NFC_DBG_NL "u32FileVersionInactive = x%08x",       psNfcOtaCmdImage->u32FileVersionInactive);
    DBG_vPrintf((TRACE_APP_NTAG||TRACE_APP_NTAG_VERBOSE), NFC_DBG_NL "u32TotalImageInactive  = x%08x / d%d", psNfcOtaCmdImage->u32TotalImageInactive,  psNfcOtaCmdImage->u32TotalImageInactive);
    DBG_vPrintf((TRACE_APP_NTAG||TRACE_APP_NTAG_VERBOSE), NFC_DBG_NL "u32SavedImageInactive  = x%08x / d%d", psNfcOtaCmdImage->u32SavedImageInactive,  psNfcOtaCmdImage->u32SavedImageInactive);
    DBG_vPrintf((TRACE_APP_NTAG||TRACE_APP_NTAG_VERBOSE), NFC_DBG_NL "u32OffsetImageInactive = x%08x / d%d", psNfcOtaCmdImage->u32OffsetImageInactive, psNfcOtaCmdImage->u32OffsetImageInactive);
}
#endif /* (TRACE_APP_NTAG||TRACE_APP_NTAG_VERBOSE) */
#endif /* APP_NTAG_OTA */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
