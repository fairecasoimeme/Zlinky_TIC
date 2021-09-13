/*****************************************************************************
 *
 * MODULE:          JN-AN-1243 Base Device Application
 *
 * COMPONENT:       app_ntag_icode.c
 *
 * DESCRIPTION:     Application layer for NTAG (Installation Code encryption)
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
#include "ntag.h"
#include "ntag_nwk.h"
#include "app_ntag_icode.h"
//#include "app_icode.h"
#include "app_main.h"
#include "PDM_IDs.h"
#ifdef OccupancySensor
    #include "app_occupancy_buttons.h"
#endif
#ifdef LightSensor
    #include "app_light_sensor_buttons.h"
    #include "app_power_on_counter.h"
#endif
#ifdef LTOSensor
    #include "app_sensor_buttons.h"
#endif
#if (defined JENNIC_CHIP_FAMILY_JN518x)
    #include "fsl_rng.h"
    #include "fsl_reset.h"
    #include "fsl_os_abstraction.h"
#endif
#include "MicroSpecific.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifdef DEBUG_APP_NTAG
    #define TRACE_APP_NTAG      TRUE
    #define TRACE_APP_NTAG_DATA FALSE
    #define TEST_ICODE          FALSE  /* Runs test of icode encryption/decryption */
#else
    #define TRACE_APP_NTAG      FALSE
    #define TRACE_APP_NTAG_DATA FALSE
    #define TEST_ICODE          FALSE
#endif
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/* APP_NTAG States */
typedef enum
{
    E_APP_NTAG_STATE_NONE,              // 0
    E_APP_NTAG_STATE_ABSENT,            // 1
    E_APP_NTAG_STATE_PRESENT            // 2
} teAppNtagState;

/* APP_NTAG Modes */
typedef enum
{
    E_APP_NTAG_MODE_APP,                // 0
    E_APP_NTAG_MODE_NWK                 // 1
} teAppNtagMode;

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE bool_t APP_bNtagNciCmdFactoryReset(void);
PRIVATE bool_t APP_bNtagNciCmdJoinWithCode(void);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE uint32                   u32AppNtagTicks;
PRIVATE uint32                   u32AppNtagMs;
PRIVATE teAppNtagState             eAppNtagState;
PRIVATE teAppNtagMode              eAppNtagMode;
PRIVATE tsNfcNwkInstallCode        sNfcNwkInstallCodePdm;
PRIVATE tsNfcNwkNci                sNfcNwkNciPdm;
PRIVATE tsNfcNwkPayload            sNfcNwkPayloadRead;
PRIVATE tsNfcNwkPayload            sNfcNwkPayloadWrite;
PRIVATE uint32                   u32NfcNwkAddress;
PRIVATE bool_t                     bNtagTimer;
PRIVATE bool_t                     bFactoryReset;
PRIVATE bool_t                     bReset;
PRIVATE bool_t                     bActive;
PRIVATE bool_t                     bRestart;
PRIVATE uint8                     u8Endpoint;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
extern void APP_vFactoryResetRecords(void); /* Should be in app_???_node.c already or an equivalent */
extern void APP_vOobcSetRunning(void);  /* Add to app_???_node.c */

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
    PDM_teStatus   ePdmStatus;
    uint16       u16PdmRead;
    uint8         u8Byte;
    uint8        au8NetworkKey[NFC_NWK_PAYLOAD_KEY_SIZE];

    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "%d: APP_vNtagPdmLoad(), msec=%d", u32AppNtagMs, OSA_TimeGetMsec());

    /* Attempt to read installation code data */
    u16PdmRead = 0;
    ePdmStatus = PDM_eReadDataFromRecord(PDM_ID_APP_NFC_ICODE,
                                         &sNfcNwkInstallCodePdm,
                                         sizeof(tsNfcNwkInstallCode),
                                         &u16PdmRead);
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "    PDM_eReadDataFromRecord(x%04x / PDM_ID_APP_NFC_ICODE) = %d, %d, msec=%d", PDM_ID_APP_NFC_ICODE, ePdmStatus, u16PdmRead, OSA_TimeGetMsec());
    /* Did not recover data ? */
    if (sizeof(tsNfcNwkInstallCode) != u16PdmRead)
    {
        /* JN518x ? */
        #if (defined JENNIC_CHIP_FAMILY_JN518x)
        {
            trng_config_t trng_config;

            /* Initialise RNG module */
            DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "    TRNG_GetDefaultConfig()");
            TRNG_GetDefaultConfig(&trng_config);
            DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "    TRNG_Init() msec=%d", OSA_TimeGetMsec());
            TRNG_Init(RNG, &trng_config);
            /* Generate random install code */
            for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_KEY_SIZE; u8Byte++)
            {
                /* Generate random install code */
                TRNG_GetRandomData(RNG, &sNfcNwkInstallCodePdm.au8Key[u8Byte], 1);
            }
            /* De-initialise */
            DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "    TRNG_Deinit() msec=%d", OSA_TimeGetMsec());
            TRNG_Deinit(RNG);
        }
        #else
        {
            uint16 u16Random;

            /* Generate random install code */
            for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_KEY_SIZE; u8Byte++)
            {
                vAHI_StartRandomNumberGenerator(E_AHI_RND_SINGLE_SHOT, E_AHI_INTS_DISABLED);
                while(!bAHI_RndNumPoll());
                u16Random = u16AHI_ReadRandomNumber();
                sNfcNwkInstallCodePdm.au8Key[u8Byte] = (uint8)(u16Random & 0xFF);
            }
        }
        #endif
        /* Application encryption ? */
        #ifdef APP_ICODE_H_
        {
            /* Calculate CRC */
            sNfcNwkInstallCodePdm.u16Crc = APP_u16InstallCodeCrc(sNfcNwkInstallCodePdm.au8Key, NFC_NWK_PAYLOAD_KEY_SIZE);
        }
        /* Stack encryption ? */
        #else
        {
            /* Calculate CRC */
            sNfcNwkInstallCodePdm.u16Crc = ZPS_u16crc(sNfcNwkInstallCodePdm.au8Key, NFC_NWK_PAYLOAD_KEY_SIZE);
        }
        #endif
        /* Save new install code */
        ePdmStatus = PDM_eSaveRecordData(PDM_ID_APP_NFC_ICODE,
                                         &sNfcNwkInstallCodePdm,
                                         sizeof(tsNfcNwkInstallCode));
        /* Debug */
        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "    PDM_eSaveRecordData(x%04x / PDM_ID_APP_NFC_ICODE) = %d, msec=%d", PDM_ID_APP_NFC_ICODE, ePdmStatus, OSA_TimeGetMsec());
    }
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "    sNfcNwkInstallCodePdm.au8Key =");
    for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_KEY_SIZE; u8Byte++) DBG_vPrintf(TRACE_APP_NTAG, " %02x", sNfcNwkInstallCodePdm.au8Key[u8Byte]);
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "    sNfcNwkInstallCodePdm.u16Crc = x%04x", sNfcNwkInstallCodePdm.u16Crc);

    /* Attempt to read network nci data */
    u16PdmRead = 0;
    ePdmStatus = PDM_eReadDataFromRecord(PDM_ID_APP_NFC_NWK_NCI,
                                         &sNfcNwkNciPdm,
                                         sizeof(tsNfcNwkNci),
                                         &u16PdmRead);
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "    PDM_eReadDataFromRecord(x%04x / PDM_ID_APP_NFC_NWK_NCI) = %d, %d, msec=%d", PDM_ID_APP_NFC_NWK_NCI, ePdmStatus, u16PdmRead, OSA_TimeGetMsec());
    /* Recovered data ? */
    if (sizeof(tsNfcNwkNci) == u16PdmRead)
    {
        /* Join with install code command ? */
        if (NFC_NWK_NCI_CMD_JOIN_WITH_CODE == sNfcNwkNciPdm.u8Command)
        {
            uint64 u64ExtAddress;

            /* Get our extended address */
            u64ExtAddress   = ZPS_u64AplZdoGetIeeeAddr();
            /* Application decryption ? */
            #ifdef APP_ICODE_H_
            {
                /* Get decrypted key */
                bReturn = APP_bInstallCode(FALSE,
                                           /* Inputs */
                                           sNfcNwkInstallCodePdm.au8Key,   /* uint8*    pu8InstallCode, */
                                           sNfcNwkNciPdm.au8Key,           /* uint8*    pu8EncKey, */
                                           u64ExtAddress,                  /* uint64    u64ExtAddress, */
                                           /* Outputs */
                                           au8NetworkKey,                  /* uint8*    pu8DecKey, */
                                           sNfcNwkNciPdm.au8Mic);          /* uint8*    pu8Mic */
            }
            /* Stack encryption ? */
            #else
            {
                bReturn = BDB_bOutOfBandCommissionGetKey(sNfcNwkInstallCodePdm.au8Key,
                                                         sNfcNwkNciPdm.au8Key,
                                                         u64ExtAddress,
                                                         au8NetworkKey,
                                                         sNfcNwkNciPdm.au8Mic);
            }
            #endif
            /* Success ? */
            if (bReturn)
            {
                /* Set result back to false - until we are successful */
                bReturn = FALSE;
                /* Got a valid channel ? */
                if (sNfcNwkNciPdm.u8Channel >= 11 && sNfcNwkNciPdm.u8Channel <= 26)
                {
                    uint8 u8BdbOob;
                    BDB_tsOobWriteDataToCommission sBdbOob;

                    /* Populate BDB data */
                    sBdbOob.u64PanId              = sNfcNwkNciPdm.u64ExtPanId;
                    sBdbOob.u64TrustCenterAddress = sNfcNwkNciPdm.u64ExtAddress;
                    sBdbOob.pu8NwkKey             = au8NetworkKey;
                    sBdbOob.pu8InstallCode        = (uint8 *) NULL; /* Don't use until we have proper end-to-end icode implementation */
                    sBdbOob.u16PanId              = sNfcNwkNciPdm.u16PanId;
                    sBdbOob.u16ShortAddress       = 0xFFFF;
                    sBdbOob.u8ActiveKeySqNum      = sNfcNwkNciPdm.u8KeySeqNum;
                    sBdbOob.u8DeviceType          = ZPS_eAplZdoGetDeviceType();
                    sBdbOob.u8Channel             = sNfcNwkNciPdm.u8Channel;
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
#ifndef SLEEPY_ZED
                    /* Set app state to running for oobc */
                    //APP_vOobcSetRunning();
#endif
                    /* Set on a network */
                    sBDB.sAttrib.bbdbNodeIsOnANetwork = TRUE;
                    /* Debug */
                    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "    BDB_u8OutOfBandCommissionStartDevice() = %d", u8BdbOob);
                    /* Clear record (can we delete it instead ?) */
                    memset(&sNfcNwkNciPdm, 0, sizeof(sNfcNwkNciPdm));
                    /* Save empty record */
                    ePdmStatus = PDM_eSaveRecordData(PDM_ID_APP_NFC_NWK_NCI,
                                                     &sNfcNwkNciPdm,
                                                     sizeof(tsNfcNwkNci));
                    /* Debug */
                    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "    PDM_eSaveRecordData(x%04x / PDM_ID_APP_NFC_NWK_NCI) = %d", PDM_ID_APP_NFC_NWK_NCI, ePdmStatus);
                    /* Note we started the join process */
                    bReturn = TRUE;
                }
            }
        }
    }
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "%d: APP_vNtagPdmLoad() EXIT msec=%d", u32AppNtagMs, OSA_TimeGetMsec());

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
    ZPS_tsAplAfSimpleDescriptor sDesc;

    /* Icode debug ? */
    #if TEST_ICODE
    {
        uint8                             au8InCode[16] = {0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87, 0x78, 0x69, 0x5a, 0x4b, 0x3c, 0x2d, 0x1e, 0x0f};
        uint8                             au8NwkKey[16] = {0xff, 0x00, 0xee, 0x11, 0xdd, 0x22, 0xcc, 0x33, 0xbb, 0x44, 0xaa, 0x55, 0x99, 0x2d, 0x1e, 0x0f};
        uint64                             u64ExtAddr = 0x0123456789abcdef;
        uint8                             au8EncKey[16];
        uint8                             au8EncMic[4];
        uint8                             au8DecKey[16];

        /* Encrypt key */
        (void) APP_bInstallCode(TRUE,         /* Encrypt */
                                /* Inputs */
                                au8InCode,    /* uint8*    pu8InstallCode, */
                                au8NwkKey,    /* uint8*    pu8Input, */
                                u64ExtAddr,   /* uint64    u64ExtAddress, */
                                /* Outputs */
                                au8EncKey,    /* uint8*    pu8DecKey, */
                                au8EncMic);   /* uint8*    pu8Mic */
        /* Decrypt key */
        (void) APP_bInstallCode(FALSE,
                                /* Inputs */
                                au8InCode,    /* uint8*    pu8InstallCode, */
                                au8EncKey,    /* uint8*    pu8EncKey, */
                                u64ExtAddr,   /* uint64    u64ExtAddress, */
                                /* Outputs */
                                au8DecKey,    /* uint8*    pu8DecKey, */
                                au8EncMic);   /* uint8*    pu8Mic */
    }
    #endif

    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "%d: APP_vNtagStart(x%02x) ICODE ENTER bActive=%d, msec=%d", u32AppNtagMs, u8ApplicationEndpoint, bActive, OSA_TimeGetMsec());
    /* Already active ? */
    if (bActive)
    {
        /* Set restart flag */
        bRestart = TRUE;
    }
    /* Not already active ? */
    else
    {
        /* Flag we are active */
        bActive = TRUE;
        /* Note endpoint */
        u8Endpoint = u8ApplicationEndpoint;
        /* Set ntag data for writing */
        sNfcNwkPayloadWrite.sNtag.u8Version       = NFC_NWK_PAYLOAD_VERSION;
        sNfcNwkPayloadWrite.sNtag.u8Command       = NFC_NWK_NTAG_CMD_JOIN_WITH_CODE;
        sNfcNwkPayloadWrite.sNtag.u8Sequence      = 0;
        sNfcNwkPayloadWrite.sNtag.u16DeviceId     = 0xffff;
        sNfcNwkPayloadWrite.sNtag.u64ExtAddress   = ZPS_u64AplZdoGetIeeeAddr();
        sNfcNwkPayloadWrite.sNtag.u64ExtPanId     = ZPS_u64AplZdoGetNetworkExtendedPanId();
        memcpy(sNfcNwkPayloadWrite.sNtag.au8Key, sNfcNwkInstallCodePdm.au8Key, NFC_NWK_PAYLOAD_KEY_SIZE);
        sNfcNwkPayloadWrite.sNtag.u16Crc          = sNfcNwkInstallCodePdm.u16Crc;
        /* Has an extended PAN ID been set (we are in a network) ? */
        if (sNfcNwkPayloadWrite.sNtag.u64ExtPanId != 0)
        {
            /* Set other network identifiers */
            sNfcNwkPayloadWrite.sNtag.u16ShortAddress = ZPS_u16AplZdoGetNwkAddr();
            sNfcNwkPayloadWrite.sNtag.u8Channel       = ZPS_u8AplZdoGetRadioChannel();
            sNfcNwkPayloadWrite.sNtag.u16PanId        = ZPS_u16AplZdoGetNetworkPanId();
        }
        else
        {
            /* Invalidate other network identifiers */
            sNfcNwkPayloadWrite.sNtag.u16ShortAddress = 0xffff;
            sNfcNwkPayloadWrite.sNtag.u8Channel       = 0;
            sNfcNwkPayloadWrite.sNtag.u16PanId        = 0;
        }
        /* Can we get the simple descriptor for the passed in endpoint */
        if (ZPS_eAplAfGetSimpleDescriptor(u8ApplicationEndpoint, &sDesc) == E_ZCL_SUCCESS)
        {
            /* Overwrite with correct id */
            sNfcNwkPayloadWrite.sNtag.u16DeviceId = sDesc.u16DeviceId;
        }
        /* Clear nci data for writing */
        memset(&sNfcNwkPayloadRead.sNci, 0, sizeof(tsNfcNwkNci));
        /* Initialise main NTAG state machine */
        NTAG_vInitialise(APP_NTAG_ADDRESS,
                         APP_NTAG_I2C_SCL,
                         APP_NTAG_I2C_FREQUENCY_HZ,
                         APP_BUTTONS_NFC_FD);
        /* Set state and mode */
        eAppNtagState = E_APP_NTAG_STATE_NONE;
        eAppNtagMode  = E_APP_NTAG_MODE_APP;
        /* Register callback */
        NTAG_vRegCbEvent(APP_cbNtagEvent);
        /* Flag that the timer should run */
        bNtagTimer = TRUE;
        /* Start the timer */
        ZTIMER_eStart(u8TimerNtag, APP_NTAG_TICK_MS);
    }
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "%d: APP_vNtagStart(x%02x) ICODE EXIT bActive=%d, msec=%d", u32AppNtagMs, u8ApplicationEndpoint, bActive, OSA_TimeGetMsec());
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
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "%d: APP_vNtagStop() ENTER bActive=%d, msec=%d", u32AppNtagMs, bActive, OSA_TimeGetMsec());
    /* Active ? */
    if (bActive)
    {
        /* Not active */
        bActive = FALSE;
        /* Are we in network mode ? */
        if (eAppNtagMode == E_APP_NTAG_MODE_NWK)
        {
            /* Stop network processing */
            (void) NTAG_NWK_eStop();
            /* Go to application mode */
            eAppNtagMode = E_APP_NTAG_MODE_APP;
            /* Register callback */
            NTAG_vRegCbEvent(APP_cbNtagEvent);
        }
        /* Flag that the timer shouldn't run */
        bNtagTimer = FALSE;
        /* Deinitialise NTAG */
        NTAG_vDeinitialise();
    }
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "%d: APP_vNtagStop() EXIT bActive=%d, msec=%d", u32AppNtagMs, bActive, OSA_TimeGetMsec());
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
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, ".");
    /* Increment tick counter and timer value */
    u32AppNtagTicks++;
    u32AppNtagMs += APP_NTAG_TICK_MS;
    /* Which mode are we in ? */
    switch (eAppNtagMode)
    {
        /* Network ? */
        case E_APP_NTAG_MODE_NWK:
        {
            teNtagNwkStatus eNtagNwkStatus;

            /* Maintain network NTAG state machine */
            eNtagNwkStatus = NTAG_NWK_eTick(APP_NTAG_TICK_MS);
            /* Finished reading ntag data ? */
            if (E_NTAG_NWK_READ_FAIL == eNtagNwkStatus
            ||  E_NTAG_NWK_READ_OK   == eNtagNwkStatus)
            {
                bool_t bValid     = FALSE;

                /* Debug */
                DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "%d: APP_cbNtagTimer()", u32AppNtagMs);
                DBG_vPrintf(TRACE_APP_NTAG, ", NTAG_NWK_eTick() = %d, msec=%d", eNtagNwkStatus, OSA_TimeGetMsec());
                /* Read ok ? */
                if (E_NTAG_NWK_READ_OK == eNtagNwkStatus)
                {
                    /* Debug */
                    DBG_vPrintf(TRACE_APP_NTAG, " (READ_OK), u32Ntag = %d, NTAG_NWK_bCcValid() = %d", NTAG_u32Ntag(), NTAG_NWK_bCcValid());
                    /* Debug data */
                    #if TRACE_APP_NTAG_DATA
                    {
                        uint8 u8Byte;

                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "    sNfcNwkPayloadRead");
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "        sNtag");
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u8Version       = %d",    sNfcNwkPayloadRead.sNtag.u8Version);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u8Command       = x%02x", sNfcNwkPayloadRead.sNtag.u8Command);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u8Sequence      = %d",    sNfcNwkPayloadRead.sNtag.u8Sequence);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u16DeviceId     = x%04x", sNfcNwkPayloadRead.sNtag.u16DeviceId);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u64ExtAddress   = %08x:%08x", (uint32)(sNfcNwkPayloadRead.sNtag.u64ExtAddress >> 32), (uint32)(sNfcNwkPayloadRead.sNtag.u64ExtAddress & 0xFFFFFFFF));
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u16ShortAddress = x%04x", sNfcNwkPayloadRead.sNtag.u16ShortAddress);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u8Channel       = %d",    sNfcNwkPayloadRead.sNtag.u8Channel);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u16PanId        = x%04x", sNfcNwkPayloadRead.sNtag.u16PanId);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u64ExtPanId     = %08x:%08x", (uint32)(sNfcNwkPayloadRead.sNtag.u64ExtPanId >> 32), (uint32)(sNfcNwkPayloadRead.sNtag.u64ExtPanId & 0xFFFFFFFF));
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            au8Key          =");
                        for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_KEY_SIZE; u8Byte++) DBG_vPrintf(TRACE_APP_NTAG, " %02x", sNfcNwkPayloadRead.sNtag.au8Key[u8Byte]);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u16Crc          = x%04x", sNfcNwkPayloadRead.sNtag.u16Crc);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "        sNci");
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u8Command       = x%02x", sNfcNwkPayloadRead.sNci.u8Command);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u8Sequence      = %d",    sNfcNwkPayloadRead.sNci.u8Sequence);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u16DeviceId     = x%04x", sNfcNwkPayloadRead.sNci.u16DeviceId);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u64ExtAddress   = %08x:%08x", (uint32)(sNfcNwkPayloadRead.sNci.u64ExtAddress >> 32), (uint32)(sNfcNwkPayloadRead.sNci.u64ExtAddress & 0xFFFFFFFF));
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u16ShortAddress = x%04x", sNfcNwkPayloadRead.sNci.u16ShortAddress);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u8Channel       = %d",    sNfcNwkPayloadRead.sNci.u8Channel);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u16PanId        = x%04x", sNfcNwkPayloadRead.sNci.u16PanId);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u64ExtPanId     = %08x:%08x", (uint32)(sNfcNwkPayloadRead.sNci.u64ExtPanId >> 32), (uint32)(sNfcNwkPayloadRead.sNci.u64ExtPanId & 0xFFFFFFFF));
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            au8Key          =");
                        for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_KEY_SIZE; u8Byte++) DBG_vPrintf(TRACE_APP_NTAG, " %02x", sNfcNwkPayloadRead.sNci.au8Key[u8Byte]);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            au8Mic          =");
                        for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_MIC_SIZE; u8Byte++) DBG_vPrintf(TRACE_APP_NTAG, " %02x", sNfcNwkPayloadRead.sNci.au8Mic[u8Byte]);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u8KeySeqNum     = x%02x", sNfcNwkPayloadRead.sNci.u8KeySeqNum);
                    }
                    #endif
                    /* Is the read data valid ?
                       and capability container valid ?*/
                    if (sNfcNwkPayloadRead.sNtag.u8Version     == sNfcNwkPayloadWrite.sNtag.u8Version
                    && (sNfcNwkPayloadRead.sNtag.u16DeviceId   == sNfcNwkPayloadWrite.sNtag.u16DeviceId || sNfcNwkPayloadRead.sNci.u8Command != NFC_NWK_CMD_NONE) /* Treat as valid if RPi messes up endianess of device id */
                    &&  sNfcNwkPayloadRead.sNtag.u64ExtAddress == sNfcNwkPayloadWrite.sNtag.u64ExtAddress
                    &&  NTAG_NWK_bCcValid())
                    {
                        /* Note data was valid */
                        bValid = TRUE;
                        /* Debug */
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "    VALID");

                        /* Which command ? */
                        switch (sNfcNwkPayloadRead.sNci.u8Command)
                        {
                            case NFC_NWK_NCI_CMD_FACTORY_RESET:  bFactoryReset = APP_bNtagNciCmdFactoryReset(); break;
                            case NFC_NWK_NCI_CMD_JOIN_WITH_CODE: bReset        = APP_bNtagNciCmdJoinWithCode(); break;
                            default:                                                                            break;
                        }

                        /* Transfer the sequence number to the write structure */
                        sNfcNwkPayloadWrite.sNtag.u8Sequence = sNfcNwkPayloadRead.sNtag.u8Sequence;
                    }
                    /* Invalid data ? */
                    else
                    {
                        /* Debug */
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "    INVALID");
                    }
                }
                /* Failed to read ? */
                else
                {
                    /* Debug */
                    DBG_vPrintf(TRACE_APP_NTAG, " (READ_FAIL), FAILED");
                }

                /* Was the data invalid or data we want to write is different to the data we read ? */
                if (bValid == FALSE || memcmp(&sNfcNwkPayloadRead, &sNfcNwkPayloadWrite, sizeof(tsNfcNwkPayload)) != 0)
                {
                    /* Increment sequence number */
                    sNfcNwkPayloadWrite.sNtag.u8Sequence++;
                    /* Debug data */
                    #if TRACE_APP_NTAG_DATA
                    {
                        uint8 u8Byte;

                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "    sNfcNwkPayloadWrite");
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "        sNtag");
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u8Version       = %d",    sNfcNwkPayloadWrite.sNtag.u8Version);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u8Command       = x%02x", sNfcNwkPayloadWrite.sNtag.u8Command);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u8Sequence      = %d",    sNfcNwkPayloadWrite.sNtag.u8Sequence);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u16DeviceId     = x%04x", sNfcNwkPayloadWrite.sNtag.u16DeviceId);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u64ExtAddress   = %08x:%08x", (uint32)(sNfcNwkPayloadWrite.sNtag.u64ExtAddress >> 32), (uint32)(sNfcNwkPayloadWrite.sNtag.u64ExtAddress & 0xFFFFFFFF));
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u16ShortAddress = x%04x", sNfcNwkPayloadWrite.sNtag.u16ShortAddress);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u8Channel       = %d",    sNfcNwkPayloadWrite.sNtag.u8Channel);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u16PanId        = x%04x", sNfcNwkPayloadWrite.sNtag.u16PanId);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u64ExtPanId     = %08x:%08x", (uint32)(sNfcNwkPayloadWrite.sNtag.u64ExtPanId >> 32), (uint32)(sNfcNwkPayloadWrite.sNtag.u64ExtPanId & 0xFFFFFFFF));
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            au8Key          =");
                        for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_KEY_SIZE; u8Byte++) DBG_vPrintf(TRACE_APP_NTAG, " %02x", sNfcNwkPayloadWrite.sNtag.au8Key[u8Byte]);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u16Crc          = x%04x", sNfcNwkPayloadWrite.sNtag.u16Crc);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "        sNci");
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u8Command       = x%02x", sNfcNwkPayloadWrite.sNci.u8Command);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u8Sequence      = %d",    sNfcNwkPayloadWrite.sNci.u8Sequence);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u16DeviceId     = x%04x", sNfcNwkPayloadWrite.sNci.u16DeviceId);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u64ExtAddress   = %08x:%08x", (uint32)(sNfcNwkPayloadWrite.sNci.u64ExtAddress >> 32), (uint32)(sNfcNwkPayloadWrite.sNci.u64ExtAddress & 0xFFFFFFFF));
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u16ShortAddress = x%04x", sNfcNwkPayloadWrite.sNci.u16ShortAddress);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u8Channel       = %d",    sNfcNwkPayloadWrite.sNci.u8Channel);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u16PanId        = x%04x", sNfcNwkPayloadWrite.sNci.u16PanId);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u64ExtPanId     = %08x:%08x", (uint32)(sNfcNwkPayloadWrite.sNci.u64ExtPanId >> 32), (uint32)(sNfcNwkPayloadWrite.sNci.u64ExtPanId & 0xFFFFFFFF));
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            au8Key          =");
                        for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_KEY_SIZE; u8Byte++) DBG_vPrintf(TRACE_APP_NTAG, " %02x", sNfcNwkPayloadWrite.sNci.au8Key[u8Byte]);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            au8Mic          =");
                        for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_MIC_SIZE; u8Byte++) DBG_vPrintf(TRACE_APP_NTAG, " %02x", sNfcNwkPayloadWrite.sNci.au8Mic[u8Byte]);
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "            u8KeySeqNum     = x%02x", sNfcNwkPayloadWrite.sNci.u8KeySeqNum);
                    }
                    #endif

                    /* Was the read data not valid ? */
                    if (bValid != TRUE)
                    {
                        /* Write to zero address (for default location and to add a terminator tlv) */
                        u32NfcNwkAddress = 0;
                    }
                    /* Start the write */
                    eNtagNwkStatus = NTAG_NWK_eWrite(&u32NfcNwkAddress, &sNfcNwkPayloadWrite);
                }
                /* Not writing ? */
                else
                {
                    /* Stop ntag processing */
                    APP_vNtagStop();
                    /* Update status */
                    eNtagNwkStatus = NTAG_NWK_eStatus();
                    /* Need to factory reset ? */
                    if (bFactoryReset)
                    {
                        ZPS_teStatus eStatus;

                        /* Request leave */
                        eStatus = ZPS_eAplZdoLeaveNetwork(0, FALSE, FALSE);
                            /* Debug */
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "    ZPS_eAplZdoLeaveNetwork(0, F, F) = %d", eStatus);
                            /* Leave without rejoin request rejected (probably not in a network) ? */
                        if (ZPS_E_SUCCESS != eStatus)
                        {
                            /* Leave failed,so just reset everything */
                            APP_vFactoryResetRecords();
                            /* JN518x ? */
                            #if (defined JENNIC_CHIP_FAMILY_JN518x)
                            {
                                /* Reset */
                                MICRO_DISABLE_INTERRUPTS();
                                RESET_SystemReset();
                            }
                            /* JN516x / JN517x ? */
                            #else
                            {
                                /* Reset */
                                vAHI_SwReset();
                            }
                            #endif
                        }
                    }
                    /* Need to reset ? */
                    else if (bReset)
                    {
                        /* JN518x ? */
                        #if (defined JENNIC_CHIP_FAMILY_JN518x)
                        {
                            /* Reset */
                            MICRO_DISABLE_INTERRUPTS();
                            RESET_SystemReset();
                        }
                        /* JN516x / JN517x ? */
                        #else
                        {
                            /* Reset */
                            vAHI_SwReset();
                        }
                        #endif
                    }
                    /* Not resetting ? */
                    else
                    {
                        /* Need to restart ? */
                        if (bRestart)
                        {
                            DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "%d: APP_cbNtagTimer() RESTART (read)", u32AppNtagMs);
                            /* Clear restart flag */
                            bRestart = FALSE;
                            /* Restart */
                            APP_vNtagStart(u8Endpoint);
                        }
                    }
                }
                DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "%d: APP_cbNtagTimer() EXIT", u32AppNtagMs);
                DBG_vPrintf(TRACE_APP_NTAG, " msec=%d", OSA_TimeGetMsec());
            }
            /* Finished writing ntag data or went idle unexpectedly ? */
            else if (E_NTAG_NWK_WRITE_FAIL == eNtagNwkStatus
            ||       E_NTAG_NWK_WRITE_OK   == eNtagNwkStatus
            ||       E_NTAG_NWK_IDLE       == eNtagNwkStatus)
            {
                /* Debug */
                DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "%d: APP_cbNtagTimer()", u32AppNtagMs);
                DBG_vPrintf(TRACE_APP_NTAG, ", NTAG_NWK_eTick() = %d, msec=%d", eNtagNwkStatus, OSA_TimeGetMsec());
                /* Debug */
                if      (E_NTAG_NWK_WRITE_OK   == eNtagNwkStatus) DBG_vPrintf(TRACE_APP_NTAG, " (WRITE_OK), u32Ntag = %d", NTAG_u32Ntag());
                else if (E_NTAG_NWK_WRITE_FAIL == eNtagNwkStatus) DBG_vPrintf(TRACE_APP_NTAG, " (WRITE_FAIL)");
                else if (E_NTAG_NWK_IDLE       == eNtagNwkStatus) DBG_vPrintf(TRACE_APP_NTAG, " (IDLE)");
                /* Stop ntag processing */
                APP_vNtagStop();
                /* Update status */
                eNtagNwkStatus = NTAG_NWK_eStatus();
                /* Need to factory reset ? */
                if (bFactoryReset)
                {
                    ZPS_teStatus eStatus;

                    /* Request leave */
                    eStatus = ZPS_eAplZdoLeaveNetwork(0, FALSE, FALSE);
                    /* Debug */
                    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "    ZPS_eAplZdoLeaveNetwork(0, F, F) = %d", eStatus);
                    /* Leave without rejoin request rejected (probably not in a network) ? */
                    if (ZPS_E_SUCCESS != eStatus)
                        {
                        /* Leave failed, so just reset everything */
                        APP_vFactoryResetRecords();
                            /* JN518x ? */
                            #if (defined JENNIC_CHIP_FAMILY_JN518x)
                            {
                                /* Reset */
                                MICRO_DISABLE_INTERRUPTS();
                                RESET_SystemReset();
                            }
                            /* JN516x / JN517x ? */
                            #else
                            {
                                /* Reset */
                                vAHI_SwReset();
                            }
                            #endif
                        }
                    }
                /* Need to reset ? */
                else if (bReset)
                {
                    /* JN518x ? */
                    #if (defined JENNIC_CHIP_FAMILY_JN518x)
                    {
                        /* Reset */
                        MICRO_DISABLE_INTERRUPTS();
                        RESET_SystemReset();
                    }
                    /* JN516x / JN517x ? */
                    #else
                    {
                        /* Reset */
                        vAHI_SwReset();
                    }
                    #endif
                }
                /* Not resetting ? */
                else
                {
                    /* Need to restart ? */
                    if (bRestart)
                    {
                        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "%d: APP_cbNtagTimer() RESTART (write)", u32AppNtagMs);
                        /* Clear restart flag */
                        bRestart = FALSE;
                        /* Restart */
                        APP_vNtagStart(u8Endpoint);
                    }
                }
                DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "%d: APP_cbNtagTimer() EXIT", u32AppNtagMs);
                DBG_vPrintf(TRACE_APP_NTAG, " msec=%d", OSA_TimeGetMsec());
            }
        }
        break;

        /* Others (application mode) ? */
        default:
        {
            /* Maintain driver NTAG state machine */
            NTAG_vTick(APP_NTAG_TICK_MS);
        }
        break;
    }
    /* Restart the timer */
    if (bNtagTimer) ZTIMER_eStart(u8TimerNtag, APP_NTAG_TICK_MS);
}

/****************************************************************************
 *
 * NAME: APP_cbNtagEvent
 *
 * DESCRIPTION:
 * Called when a tag event takes place
 *
 * RETURNS:
 * None
 *
 ****************************************************************************/
PUBLIC  void        APP_cbNtagEvent(    /* Called when an event takes place */
        teNtagEvent eNtagEvent,         /* Event raised */
        uint32      u32Address,
        uint32      u32Length,
        uint8       *pu8Data)           /* Event data (NULL if no data) */
{
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "%d: APP_cbNtagEvent(%d, %d, %d)", u32AppNtagMs, eNtagEvent, u32Address, u32Length);
    /* Which event ? */
    switch (eNtagEvent)
    {
        /* Absent ? */
        case E_NTAG_EVENT_ABSENT:
        {
            /* Not already absent ? */
            if (E_APP_NTAG_STATE_ABSENT != eAppNtagState)
            {
                /* Debug */
                DBG_vPrintf(TRACE_APP_NTAG, ", eAppNtagState = ABSENT");
                /* Go to absent state */
                eAppNtagState = E_APP_NTAG_STATE_ABSENT;
                /* Not in NWK mode ? */
                if (E_APP_NTAG_MODE_NWK != eAppNtagMode)
                {
                    /* Debug */
                    DBG_vPrintf(TRACE_APP_NTAG, ", eAppNtagMode = NWK");
                    /* Go to network mode */
                    eAppNtagMode = E_APP_NTAG_MODE_NWK;
                    /* Start reading ntag network data */
                    (void) NTAG_NWK_eRead(&u32NfcNwkAddress, &sNfcNwkPayloadRead);
                }
            }
        }
        break;

        /* Present ? */
        case E_NTAG_EVENT_PRESENT:
        {
            /* Not already present ? */
            if (E_APP_NTAG_STATE_PRESENT != eAppNtagState)
            {
                /* Debug */
                DBG_vPrintf(TRACE_APP_NTAG, ", eAppNtagState = PRESENT");
                /* Go to present state */
                eAppNtagState = E_APP_NTAG_STATE_PRESENT;
                /* Stop ntag processing */
                APP_vNtagStop();
            }
        }
        break;

        /* Others ? */
        default:
        {
            /* Do nothing */
            ;
        }
        break;
    }
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/
/****************************************************************************
 *
 * NAME: APP_bNtagNciCmdJoinWithCode
 *
 * DESCRIPTION:
 * Handle join with code command from nci
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE bool_t APP_bNtagNciCmdFactoryReset(void)
{
    bool_t          bResult = TRUE;

    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "%d: bNtagNciCmdFactoryReset()", u32AppNtagMs);
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "        bResult        = %d", bResult);

    return bResult;
}

/****************************************************************************
 *
 * NAME: APP_bNtagNciCmdJoinWithCode
 *
 * DESCRIPTION:
 * Handle join with code command from nci
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE bool_t APP_bNtagNciCmdJoinWithCode(void)
{
    bool_t          bResult = FALSE;
    uint8          u8Byte;
    uint8         au8NetworkKey[NFC_NWK_PAYLOAD_KEY_SIZE];
    uint64        u64ExtAddress;


    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "%d: bNtagNciCmdJoinWithCode()", u32AppNtagMs);

    /* Get our extended address */
    u64ExtAddress   = ZPS_u64AplZdoGetIeeeAddr();
    /* Application decryption ? */
    #ifdef APP_ICODE_H_
    {
        /* Get decrypted key */
        bResult = APP_bInstallCode(FALSE,
                                   /* Inputs */
                                   sNfcNwkInstallCodePdm.au8Key,        /* uint8*    pu8InstallCode, */
                                   sNfcNwkPayloadRead.sNci.au8Key,      /* uint8*    pu8EncKey, */
                                   u64ExtAddress,                       /* uint64    u64ExtAddress, */
                                   /* Outputs */
                                   au8NetworkKey,                       /* uint8*    pu8DecKey, */
                                   sNfcNwkPayloadRead.sNci.au8Mic);     /* uint8*    pu8Mic */
    }
    /* Stack encryption ? */
    #else
    {
        bResult = BDB_bOutOfBandCommissionGetKey(sNfcNwkInstallCodePdm.au8Key,
                                                 sNfcNwkPayloadRead.sNci.au8Key,
                                                 u64ExtAddress,
                                                 au8NetworkKey,
                                                 sNfcNwkPayloadRead.sNci.au8Mic);
    }
    #endif
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "        GetKey()       = %d", bResult);
    /* Success ? */
    if (bResult)
    {
        PDM_teStatus   ePdmStatus;

        /* Debug */
        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "        au8NetworkKey  =");
        for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_KEY_SIZE; u8Byte++) DBG_vPrintf(TRACE_APP_NTAG, " %02x", au8NetworkKey[u8Byte]);
        /* Copy data into PDM structure */
        memcpy(&sNfcNwkNciPdm, &sNfcNwkPayloadRead.sNci, sizeof(tsNfcNwkNci));
        /* Save record */
        ePdmStatus = PDM_eSaveRecordData(PDM_ID_APP_NFC_NWK_NCI,
                                         &sNfcNwkNciPdm,
                                         sizeof(sNfcNwkNciPdm));
        /* Debug */
        DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "        PDM_eSaveRecordData(PDM_ID_APP_NFC_NWK_NCI) = %d", ePdmStatus);
        /* Failed ? */
        if (ePdmStatus != PDM_E_STATUS_OK)
        {
            /* Set result to failed */
            bResult = FALSE;
        }
    }
    /* Debug */
    DBG_vPrintf(TRACE_APP_NTAG, NFC_DBG_NL "        bResult        = %d", bResult);

    return bResult;
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
