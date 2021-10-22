/*****************************************************************************
 *
 * MODULE:          JN-AN-1243 Base Device application
 *
 * COMPONENT:       app_nci_icode.c
 *
 * DESCRIPTION:     Base Device - Application layer for NCI (Installation Code encryption)
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
#include "nci.h"
#include "nci_nwk.h"
#ifdef APP_NCI_OTA
#include "nci_ota.h"
#include "OTA.h"
#include "fsl_flash.h"
#endif
#include "app_nci_icode.h"
//#include "app_icode.h"
#include "app_main.h"
#ifdef LITTLE_ENDIAN_PROCESSOR
    #include "portmacro.h"
#endif
#if ((defined APP_NCI_LED1_PIN) || (defined APP_NCI_LED1_PIN))
#if (JENNIC_CHIP_FAMILY == JN518x)
#include "fsl_gpio.h"
#include "fsl_iocon.h"
#include "fsl_os_abstraction.h"
#endif
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifdef DEBUG_APP_NCI
    #define TRACE_APP_NCI          TRUE
    #define TRACE_APP_NCI_VERBOSE FALSE
#else
    #define TRACE_APP_NCI         FALSE
    #define TRACE_APP_NCI_VERBOSE FALSE
#endif

#define FLASH_INACTIVE_START    (0x48000)
#define FLASH_MAGIC_NUMBER   (0x04015fe0)
#define FLASH_MAGIC_OFFSET             6

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/* APP_NCI States */
typedef enum
{
    E_APP_NCI_STATE_NONE,           // 0
    E_APP_NCI_STATE_ABSENT,         // 1
    E_APP_NCI_STATE_PRESENT         // 2
} teAppNciState;

/* APP_NCI Modes */
typedef enum
{
    E_APP_NCI_MODE_APP,             // 0
    E_APP_NCI_MODE_NWK,             // 1
    E_APP_NCI_MODE_OTA              // 2
} teAppNciMode;

/* APP_NCI LED states */
typedef enum
{
    E_APP_NCI_LED_ABSENT,             // 0
    E_APP_NCI_LED_PRESENT_BUSY,       // 1
    E_APP_NCI_LED_PRESENT_OK,         // 2
    E_APP_NCI_LED_PRESENT_FAIL        // 3
} teAppNciLed;

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
PRIVATE bool_t APP_bNciNtagCmdJoinWithCode(void);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/
/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
PRIVATE uint32                      u32AppNciTicks;
PRIVATE uint32                      u32AppNciMs;
PRIVATE teAppNciState               eAppNciState;
PRIVATE teAppNciMode                eAppNciMode;
PRIVATE tsNfcNwkPayload             sNfcNwkPayload;
PRIVATE uint8                      u8Endpoint;
PRIVATE uint32                    u32NfcNwkAddress;
PRIVATE bool_t                      bNciTimer;
PRIVATE uint8                      u8Sequence;
PRIVATE teAppNciLed                 eAppNciLed;
PRIVATE uint8                      u8NciCommand;
#ifdef NCI_OTA_H_
PRIVATE uint8                    au8Sram[NFC_SRAM_SIZE];
PRIVATE tsNfcOtaCmdImage           sNfcOtaCmdImageNtag;
PRIVATE tsNfcOtaCmdImage           sNfcOtaCmdImageNci;
PRIVATE tsNfcOtaCmdData            sNfcOtaCmdData;
PRIVATE uint32                    u32FlashOffset = 0xFFFFFFFF;
PRIVATE uint32                    u32OtaNfcStartMsOsa;
PRIVATE uint32                    u32OtaNfcPercent;
#endif

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: APP_vNciStart
 *
 * DESCRIPTION:
 * Starts NCI processing
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void APP_vNciStart(uint8 u8ApplicationEndpoint)
{
    /* Debug */
    DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "%d: APP_vNciStart(x%02x) ICODE", u32AppNciMs, u8ApplicationEndpoint);
    /* Save the endpoint for later */
    u8Endpoint = u8ApplicationEndpoint;
    /* Initialise LEDs */
    #if ((defined APP_NCI_LED1_PIN) && (JENNIC_CHIP_FAMILY == JN518x))
    {
        gpio_pin_config_t output_config = {kGPIO_DigitalOutput, 0};
        /* Disable pullup */
        //IOCON_PinMuxSet(IOCON, 0, APP_NCI_LED1_PIN, IOCON_FUNC0 | IOCON_DIGITAL_EN | IOCON_GPIO_MODE | IOCON_MODE_INACT);
        /* Set IRQ pin as Input and VEN as output */
        GPIO_PinInit(GPIO, 0, APP_NCI_LED1_PIN, &output_config);
    }
    #endif
    #if ((defined APP_NCI_LED2_PIN) && (JENNIC_CHIP_FAMILY == JN518x))
    {
        gpio_pin_config_t output_config = {kGPIO_DigitalOutput, 0};
        /* Disable pullup */
        //IOCON_PinMuxSet(IOCON, 0, APP_NCI_LED2_PIN, IOCON_FUNC0 | IOCON_DIGITAL_EN | IOCON_GPIO_MODE | IOCON_MODE_INACT);
        /* Set IRQ pin as Input and VEN as output */
        GPIO_PinInit(GPIO, 0, APP_NCI_LED2_PIN, &output_config);
    }
    #endif
    /* Initialise main NTAG state machine */
    NCI_vInitialise(APP_NCI_ADDRESS,
                     APP_NCI_I2C_SCL,
                     APP_NCI_I2C_FREQUENCY_HZ,
                     APP_NCI_VEN_PIN,
                     APP_NCI_IRQ_PIN);
    /* Set state and mode */
    eAppNciState = E_APP_NCI_STATE_NONE;
    eAppNciMode  = E_APP_NCI_MODE_APP;
    /* Register callback */
    NCI_vRegCbEvent(APP_cbNciEvent);
    /* Set default NCI command (starts and stops timer as appropriate) */
    APP_bNciCommand(NFC_CMD_NWK_NCI_JOIN_WITH_CODE);
}

/****************************************************************************
 *
 * NAME: APP_vNciStop
 *
 * DESCRIPTION:
 * Stops NCI processing
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void APP_vNciStop(void)
{
    /* Debug */
    DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "%d: APP_vNciStop()", u32AppNciMs);
    /* Are we in network mode ? */
    if (eAppNciMode == E_APP_NCI_MODE_NWK)
    {
        /* Stop network processing */
        (void) NCI_NWK_eStop();
        /* Go to application mode */
        eAppNciMode = E_APP_NCI_MODE_APP;
        /* Register callback */
        NCI_vRegCbEvent(APP_cbNciEvent);
    }
#ifdef NCI_OTA_H_
    /* Are we in network mode ? */
    else if (eAppNciMode == E_APP_NCI_MODE_OTA)
    {
        /* Stop ota processing */
        (void) NCI_OTA_eStop();
        /* Go to application mode */
        eAppNciMode = E_APP_NCI_MODE_APP;
        /* Register callback */
        NCI_vRegCbEvent(APP_cbNciEvent);
    }
#endif
}

/****************************************************************************
 *
 * NAME: APP_vNciCommand
 *
 * DESCRIPTION:
 * Stops NCI processing
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC bool_t APP_bNciCommand(uint8 u8NciCommandNew)
{
    bool_t bReturn = TRUE;
    /* Which command to use ? */
    switch (u8NciCommandNew)
    {
#ifdef NCI_OTA_H_
        /* OTA ? */
        case NFC_CMD_OTA_IMAGE:
#endif
        /* Join with code ? */
        case NFC_CMD_NWK_NCI_JOIN_WITH_CODE:
        /* Factory reset (with leave) ? */
        case NFC_CMD_NWK_NCI_FACTORY_RESET:
        {
            /* Save command */
            u8NciCommand = u8NciCommandNew;
            /* Timer not running ? */
            if (bNciTimer == FALSE)
            {
                /* Start timer ok ? */
                if (ZTIMER_eStart(u8TimerNci, APP_NCI_TICK_MS) == E_ZTIMER_OK)
                {
                    /* Flag timer should be running */
                    bNciTimer = TRUE;
                }
            }
        }
        break;

        /* None (disabled) ? */
        case NFC_CMD_NONE:
        {
            /* Null command */
            u8NciCommand = NFC_CMD_NONE;
            /* Timer running ? */
            if (bNciTimer == TRUE)
            {
                /* Flag timer should not be running (allow to expire) */
                bNciTimer = FALSE;
            }
        }
        break;

        /* Others ? */
        default:
        {
            /* Return fail */
            bReturn = FALSE;
        }
        break;
    }

    return bReturn;
}

/****************************************************************************
 *
 * NAME: APP_cbNciTimer
 *
 * DESCRIPTION:
 * Timer callback function
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void APP_cbNciTimer(void *pvParams)
{
    uint8 u8Byte;

    /* Restart the timer */
    if (bNciTimer) ZTIMER_eStart(u8TimerNci, APP_NCI_TICK_MS);
    /* Debug */
    /*if (u32AppNciTicks % 100 == 0) DBG_vPrintf(TRACE_APP_NCI, "."); */
    /* Increment tick counter and timer value */
    u32AppNciTicks++;
    u32AppNciMs += APP_NCI_TICK_MS;
//    #warning PermitJoin off
//    ZPS_eAplZdoPermitJoining(0);
    /* Which mode are we in ? */
    switch (eAppNciMode)
    {
        /* Network ? */
        case E_APP_NCI_MODE_NWK:
        {
            teNciNwkStatus eNciNwkStatus;

            /* Maintain network NTAG state machine */
            eNciNwkStatus = NCI_NWK_eTick(APP_NCI_TICK_MS);
            /* Finished reading ntag data ? */
            if (E_NCI_NWK_READ_FAIL == eNciNwkStatus
            ||  E_NCI_NWK_READ_OK   == eNciNwkStatus)
            {
                bool_t bWrite     = FALSE;

                /* Debug */
                DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "%d: APP_cbNciTimer()", u32AppNciMs);
                DBG_vPrintf(TRACE_APP_NCI, ", NCI_NWK_eTick() = %d <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<", eNciNwkStatus);
                /* Read ok ? */
                if (E_NCI_NWK_READ_OK == eNciNwkStatus)
                {
                    /* Is the read data valid ? */
                    if (sNfcNwkPayload.sNtag.u8Version == NFC_NWK_PAYLOAD_VERSION)
                    {
                        /* Debug */
                        DBG_vPrintf(TRACE_APP_NCI, ", VALID");
                        /* Debug data */
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "    sNfcNwkPayload (Read)");
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "        sNtag");
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u8Version       = %d",    sNfcNwkPayload.sNtag.u8Version);
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u8Command       = x%02x", sNfcNwkPayload.sNtag.u8Command);
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u8Sequence      = %d",    sNfcNwkPayload.sNtag.u8Sequence);
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u16DeviceId     = x%04x", sNfcNwkPayload.sNtag.u16DeviceId);
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u64ExtAddress   = %08x:%08x", (uint32)(sNfcNwkPayload.sNtag.u64ExtAddress >> 32), (uint32)(sNfcNwkPayload.sNtag.u64ExtAddress & 0xFFFFFFFF));
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u16ShortAddress = x%04x", sNfcNwkPayload.sNtag.u16ShortAddress);
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u8Channel       = %d",    sNfcNwkPayload.sNtag.u8Channel);
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u16PanId        = x%04x", sNfcNwkPayload.sNtag.u16PanId);
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u64ExtPanId     = %08x:%08x", (uint32)(sNfcNwkPayload.sNtag.u64ExtPanId >> 32), (uint32)(sNfcNwkPayload.sNtag.u64ExtPanId & 0xFFFFFFFF));
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            au8Key          =");
                        for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_KEY_SIZE; u8Byte++) DBG_vPrintf(TRACE_APP_NCI_VERBOSE, " %02x", sNfcNwkPayload.sNtag.au8Key[u8Byte]);
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u16Crc          = x%04x", sNfcNwkPayload.sNtag.u16Crc);
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "        sNci");
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u8Command       = x%02x", sNfcNwkPayload.sNci.u8Command);
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u8Sequence      = %d",    sNfcNwkPayload.sNci.u8Sequence);
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u16DeviceId     = x%04x", sNfcNwkPayload.sNci.u16DeviceId);
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u64ExtAddress   = %08x:%08x", (uint32)(sNfcNwkPayload.sNci.u64ExtAddress >> 32), (uint32)(sNfcNwkPayload.sNci.u64ExtAddress & 0xFFFFFFFF));
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u16ShortAddress = x%04x", sNfcNwkPayload.sNci.u16ShortAddress);
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u8Channel       = %d",    sNfcNwkPayload.sNci.u8Channel);
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u16PanId        = x%04x", sNfcNwkPayload.sNci.u16PanId);
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u64ExtPanId     = %08x:%08x", (uint32)(sNfcNwkPayload.sNci.u64ExtPanId >> 32), (uint32)(sNfcNwkPayload.sNci.u64ExtPanId & 0xFFFFFFFF));
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            au8Key          =");
                        for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_KEY_SIZE; u8Byte++) DBG_vPrintf(TRACE_APP_NCI_VERBOSE, " %02x", sNfcNwkPayload.sNci.au8Key[u8Byte]);
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            au8Mic          =");
                        for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_MIC_SIZE; u8Byte++) DBG_vPrintf(TRACE_APP_NCI_VERBOSE, " %02x", sNfcNwkPayload.sNci.au8Mic[u8Byte]);
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u8KeySeqNum     = x%02x", sNfcNwkPayload.sNci.u8KeySeqNum);

                        /* Which command ? */
                        switch (sNfcNwkPayload.sNtag.u8Command)
                        {
                            case NFC_CMD_NWK_NTAG_JOIN_WITH_CODE: bWrite = APP_bNciNtagCmdJoinWithCode(); break;
                            default:                                                                      break;
                        }

                        /* Want to write data ? */
                        if (bWrite)
                        {
                            /* Debug data */
                            DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "    sNfcNwkPayload (Write)");
                            DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "        sNtag");
                            DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u8Version       = %d",    sNfcNwkPayload.sNtag.u8Version);
                            DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u8Command       = x%02x", sNfcNwkPayload.sNtag.u8Command);
                            DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u8Sequence      = %d",    sNfcNwkPayload.sNtag.u8Sequence);
                            DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u16DeviceId     = x%04x", sNfcNwkPayload.sNtag.u16DeviceId);
                            DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u64ExtAddress   = %08x:%08x", (uint32)(sNfcNwkPayload.sNtag.u64ExtAddress >> 32), (uint32)(sNfcNwkPayload.sNtag.u64ExtAddress & 0xFFFFFFFF));
                            DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u16ShortAddress = x%04x", sNfcNwkPayload.sNtag.u16ShortAddress);
                            DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u8Channel       = %d",    sNfcNwkPayload.sNtag.u8Channel);
                            DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u16PanId        = x%04x", sNfcNwkPayload.sNtag.u16PanId);
                            DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u64ExtPanId     = %08x:%08x", (uint32)(sNfcNwkPayload.sNtag.u64ExtPanId >> 32), (uint32)(sNfcNwkPayload.sNtag.u64ExtPanId & 0xFFFFFFFF));
                            DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            au8Key          =");
                            for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_KEY_SIZE; u8Byte++) DBG_vPrintf(TRACE_APP_NCI_VERBOSE, " %02x", sNfcNwkPayload.sNtag.au8Key[u8Byte]);
                            DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u16Crc          = x%04x", sNfcNwkPayload.sNtag.u16Crc);
                            DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "        sNci");
                            DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u8Command       = x%02x", sNfcNwkPayload.sNci.u8Command);
                            DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u8Sequence      = %d",    sNfcNwkPayload.sNci.u8Sequence);
                            DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u16DeviceId     = x%04x", sNfcNwkPayload.sNci.u16DeviceId);
                            DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u64ExtAddress   = %08x:%08x", (uint32)(sNfcNwkPayload.sNci.u64ExtAddress >> 32), (uint32)(sNfcNwkPayload.sNci.u64ExtAddress & 0xFFFFFFFF));
                            DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u16ShortAddress = x%04x", sNfcNwkPayload.sNci.u16ShortAddress);
                            DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u8Channel       = %d",    sNfcNwkPayload.sNci.u8Channel);
                            DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u16PanId        = x%04x", sNfcNwkPayload.sNci.u16PanId);
                            DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u64ExtPanId     = %08x:%08x", (uint32)(sNfcNwkPayload.sNci.u64ExtPanId >> 32), (uint32)(sNfcNwkPayload.sNci.u64ExtPanId & 0xFFFFFFFF));
                            DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            au8Key          =");
                            for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_KEY_SIZE; u8Byte++) DBG_vPrintf(TRACE_APP_NCI_VERBOSE, " %02x", sNfcNwkPayload.sNci.au8Key[u8Byte]);
                            DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            au8Mic          =");
                            for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_MIC_SIZE; u8Byte++) DBG_vPrintf(TRACE_APP_NCI_VERBOSE, " %02x", sNfcNwkPayload.sNci.au8Mic[u8Byte]);
                            DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "            u8KeySeqNum     = x%02x", sNfcNwkPayload.sNci.u8KeySeqNum);
                            /* Start the write back to the same address */
                            eNciNwkStatus = NCI_NWK_eWrite(&u32NfcNwkAddress, &sNfcNwkPayload);
                        }
                        else
                        {
                        /* Debug */
                            DBG_vPrintf(TRACE_APP_NCI, ", !WRITE");
                            /* Failed LED state */
                            eAppNciLed = E_APP_NCI_LED_PRESENT_FAIL;
                        }
                    }
                    /* Invalid data ? */
                    else
                    {
                        /* Debug */
                        DBG_vPrintf(TRACE_APP_NCI, ", INVALID");
                        /* Failed LED state */
                        eAppNciLed = E_APP_NCI_LED_PRESENT_FAIL;
                    }
                }
                /* Failed to read ? */
                else
                {
                    /* Debug */
                    DBG_vPrintf(TRACE_APP_NCI, ", FAILED");
                    /* Failed LED state */
                    eAppNciLed = E_APP_NCI_LED_PRESENT_FAIL;
                }

                /* Not writing ? */
                if (bWrite == FALSE)
                {
                    /* Stop network processing */
                    eNciNwkStatus = NCI_NWK_eStop();
                    /* Go to application mode */
                    eAppNciMode = E_APP_NCI_MODE_APP;
                    /* Register callback */
                    NCI_vRegCbEvent(APP_cbNciEvent);
                }
            }
            /* Finished writing nci data or wnet idle unexpectedly ? */
            else if (E_NCI_NWK_WRITE_FAIL == eNciNwkStatus
            ||       E_NCI_NWK_WRITE_OK   == eNciNwkStatus
            ||       E_NCI_NWK_IDLE       == eNciNwkStatus)
            {
                /* Debug */
                DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "%d: APP_cbNtagTimer()", u32AppNciMs);
                DBG_vPrintf(TRACE_APP_NCI, ", NCI_NWK_eTick() = %d >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>", eNciNwkStatus);
                /* Write ok ? */
                if (E_NCI_NWK_WRITE_OK == eNciNwkStatus)
                {
                    /* Set LED state */
                    eAppNciLed = E_APP_NCI_LED_PRESENT_OK;
                }
                /* Write failed ? */
                else
                {
                    /* Set LED state */
                    eAppNciLed = E_APP_NCI_LED_PRESENT_FAIL;
                }
                /* Stop network processing */
                eNciNwkStatus = NCI_NWK_eStop();
                /* Go to application mode */
                eAppNciMode = E_APP_NCI_MODE_APP;
                /* Register callback */
                NCI_vRegCbEvent(APP_cbNciEvent);
            }
        }
        break;

#ifdef NCI_OTA_H_
        /* OTA ? */
        case E_APP_NCI_MODE_OTA:
        {
            teNciOtaStatus eNciOtaStatus;

            /* Maintain ota NTAG state machine */
            eNciOtaStatus = NCI_OTA_eTick(APP_NCI_TICK_MS);
            /* Finished reading ntag data ? */
            if (E_NCI_OTA_READ_FAIL == eNciOtaStatus
            ||  E_NCI_OTA_READ_OK   == eNciOtaStatus)
            {
                /* Debug */
                DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "%d: APP_cbNciTimer()", u32AppNciMs);
                DBG_vPrintf(TRACE_APP_NCI, ", NCI_OTA_eTick() = %d <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<", eNciOtaStatus);
                /* Read ok ? */
                if (E_NCI_OTA_READ_OK == eNciOtaStatus)
                {
                    /* Convert buffer to CmdImage */
                    NFC_OTA_vCmdImageFromBuffer(&sNfcOtaCmdImageNtag, au8Sram);
                    /* Debug */
                    DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNfcOtaCmdImageNtag.u8Cmd                       = x%02x", sNfcOtaCmdImageNtag.u8Cmd);
                    /* Is this an image command ? */
                    if (sNfcOtaCmdImageNtag.u8Cmd == NFC_CMD_OTA_IMAGE)
                    {
                        bool_t  bResult;
                        uint8 au8Flash[NFC_OTA_MAX_HEADER_SIZE];
                        tsNfcOtaZbHeader sNfcOtaZbHeader;

                        /* Debugging ? */
                        #if TRACE_APP_NCI
                        {
                            char acHeaderString[NFC_OTA_STRING_SIZE+1];

                            /* Copy string and terminate */
                            memcpy(acHeaderString, sNfcOtaCmdImageNtag.stHeaderString, NFC_OTA_STRING_SIZE);
                            acHeaderString[NFC_OTA_STRING_SIZE] = '\0';
                            /* Debug */
                            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNfcOtaCmdImageNtag.u8Flags                = x%02x",       sNfcOtaCmdImageNtag.u8Flags               );
                            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNfcOtaCmdImageNtag.u32FileIdentifier      = x%08x",       sNfcOtaCmdImageNtag.u32FileIdentifier     );
                            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNfcOtaCmdImageNtag.u16ManufacturerCode    = x%04x",       sNfcOtaCmdImageNtag.u16ManufacturerCode   );
                            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNfcOtaCmdImageNtag.u16ImageType           = x%04x",       sNfcOtaCmdImageNtag.u16ImageType          );
                            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNfcOtaCmdImageNtag.u16StackVersion        = x%04x",       sNfcOtaCmdImageNtag.u16StackVersion       );
                            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNfcOtaCmdImageNtag.stHeaderString         = %s",          acHeaderString                            );
                            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNfcOtaCmdImageNtag.u32FileVersionActive   = x%08x",       sNfcOtaCmdImageNtag.u32FileVersionActive  );
                            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNfcOtaCmdImageNtag.u32FileVersionInactive = x%08x",       sNfcOtaCmdImageNtag.u32FileVersionInactive);
                            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNfcOtaCmdImageNtag.u32TotalImageInactive  = x%08x / d%d", sNfcOtaCmdImageNtag.u32TotalImageInactive,  sNfcOtaCmdImageNtag.u32TotalImageInactive );
                            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNfcOtaCmdImageNtag.u32SavedImageInactive  = x%08x / d%d", sNfcOtaCmdImageNtag.u32SavedImageInactive,  sNfcOtaCmdImageNtag.u32SavedImageInactive );
                            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNfcOtaCmdImageNtag.u32OffsetImageInactive = x%08x / d%d", sNfcOtaCmdImageNtag.u32OffsetImageInactive, sNfcOtaCmdImageNtag.u32OffsetImageInactive);
                        }
                        #endif
                        /* Zero NCI image */
                        memset((uint8 *) &sNfcOtaCmdImageNci, 0, sizeof(tsNfcOtaCmdImage));
                        /* Set basic command */
                        sNfcOtaCmdImageNci.u8Cmd = NFC_CMD_OTA_IMAGE;
                        /* Set null flash data pointer */
                        u32FlashOffset = 0xFFFFFFFF;
                        /* Attempt to read flash where inactive OTA header should be */
                        bResult = NFC_OTA_bFlashReadBytes(au8Flash, FLASH_INACTIVE_START, NFC_OTA_MAX_HEADER_SIZE);
                        /* Debug */
                        DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    NFC_OTA_bFlashReadBytes(x%x, %d) = %d", FLASH_INACTIVE_START, NFC_OTA_MAX_HEADER_SIZE, bResult);
                        /* Success ? */
                        if (bResult)
                        {
                            /* Attempt to  convert data from flash into OTA header (without image offset or checksum) ? */
                            bResult = NFC_OTA_bZbHeaderFromBuffer(&sNfcOtaZbHeader, (uint32 *) NULL, (uint32 *) NULL, au8Flash, NFC_OTA_MAX_HEADER_SIZE);
                            /* Debug */
                            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    NFC_OTA_bHeaderFromBuffer() = %d", bResult);
                            /* Success ? */
                            if (bResult)
                            {
                                /* Do we have matching data in flash (with a different file version) ? */
                                if (NFC_OTA_FILE_IDENTIFIER                  == sNfcOtaZbHeader.u32FileIdentifier
                                &&  sNfcOtaCmdImageNtag.u32FileIdentifier    == sNfcOtaZbHeader.u32FileIdentifier
                                &&  sNfcOtaCmdImageNtag.u16ManufacturerCode  == sNfcOtaZbHeader.u16ManufacturerCode
                                &&  sNfcOtaCmdImageNtag.u16ImageType         == sNfcOtaZbHeader.u16ImageType
                                &&  sNfcOtaCmdImageNtag.u16StackVersion      == sNfcOtaZbHeader.u16StackVersion
                                &&  memcmp(sNfcOtaCmdImageNtag.stHeaderString,  sNfcOtaZbHeader.stHeaderString, NFC_OTA_STRING_SIZE) == 0
                                &&  sNfcOtaCmdImageNtag.u32FileVersionActive != sNfcOtaZbHeader.u32FileVersion)
                                {
                                    /* Is the device already downloading this image ? */
                                    if (sNfcOtaCmdImageNtag.u32FileVersionInactive == sNfcOtaZbHeader.u32FileVersion
                                    &&  sNfcOtaCmdImageNtag.u32TotalImageInactive  == sNfcOtaZbHeader.u32TotalImage
                                    &&  sNfcOtaCmdImageNtag.u32SavedImageInactive  >  0
                                    &&  sNfcOtaCmdImageNtag.u32SavedImageInactive  <  sNfcOtaZbHeader.u32TotalImage)
                                    {
                                        /* Flag we will send more data */
                                        sNfcOtaCmdImageNci.u8Cmd |= NFC_CMD_OTA_FLAG_MORE;
                                        /* Download from where the device has got up to already */
                                        u32FlashOffset = sNfcOtaCmdImageNtag.u32SavedImageInactive;
                                    }
                                    /* Device is not already downloading our image ? */
                                    else
                                    {
                                        /* Flag we will send more data */
                                        sNfcOtaCmdImageNci.u8Cmd |= NFC_CMD_OTA_FLAG_MORE;
                                        /* Download from beginning */
                                        u32FlashOffset = 0;
                                    }
                                }
                            }
                        }
                        /* Going to transfer the image ? */
                        if (u32FlashOffset != 0xFFFFFFFF)
                        {
                            /* Transfer data from flash */
                            sNfcOtaCmdImageNci.u32FileIdentifier      = sNfcOtaZbHeader.u32FileIdentifier;
                            sNfcOtaCmdImageNci.u16ManufacturerCode    = sNfcOtaZbHeader.u16ManufacturerCode;
                            sNfcOtaCmdImageNci.u16ImageType           = sNfcOtaZbHeader.u16ImageType;
                            sNfcOtaCmdImageNci.u16StackVersion        = sNfcOtaZbHeader.u16StackVersion;
                            memcpy(sNfcOtaCmdImageNci.stHeaderString,   sNfcOtaZbHeader.stHeaderString, NFC_OTA_STRING_SIZE);
                            sNfcOtaCmdImageNci.u32FileVersionInactive = sNfcOtaZbHeader.u32FileVersion;
                            sNfcOtaCmdImageNci.u32TotalImageInactive  = sNfcOtaZbHeader.u32TotalImage;
                            sNfcOtaCmdImageNci.u32SavedImageInactive  = sNfcOtaZbHeader.u32TotalImage;
                            sNfcOtaCmdImageNci.u32OffsetImageInactive = 0xFFFFFFFF;
                            /* Reset progress values */
                            u32OtaNfcStartMsOsa = OSA_TimeGetMsec();
                            /* Invalidate percentage */
                            u32OtaNfcPercent = 0xFFFFFFFF;
                        }
                        /* Debugging ? */
                        #if TRACE_APP_NCI
                        {
                            char acHeaderString[NFC_OTA_STRING_SIZE+1];

                            /* Copy string and terminate */
                            memcpy(acHeaderString, sNfcOtaCmdImageNci.stHeaderString, NFC_OTA_STRING_SIZE);
                            acHeaderString[NFC_OTA_STRING_SIZE] = '\0';
                            /* Debug */
                            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNfcOtaCmdImageNci.u8Cmd                  = x%02x",       sNfcOtaCmdImageNci.u8Cmd                 );
                            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNfcOtaCmdImageNci.u8Flags                = x%02x",       sNfcOtaCmdImageNci.u8Flags               );
                            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNfcOtaCmdImageNci.u32FileIdentifier      = x%08x",       sNfcOtaCmdImageNci.u32FileIdentifier     );
                            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNfcOtaCmdImageNci.u16ManufacturerCode    = x%04x",       sNfcOtaCmdImageNci.u16ManufacturerCode   );
                            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNfcOtaCmdImageNci.u16ImageType           = x%04x",       sNfcOtaCmdImageNci.u16ImageType          );
                            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNfcOtaCmdImageNci.u16StackVersion        = x%04x",       sNfcOtaCmdImageNci.u16StackVersion       );
                            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNfcOtaCmdImageNci.stHeaderString         = %s",          acHeaderString                           );
                            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNfcOtaCmdImageNci.u32FileVersionActive   = x%08x",       sNfcOtaCmdImageNci.u32FileVersionActive  );
                            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNfcOtaCmdImageNci.u32FileVersionInactive = x%08x",       sNfcOtaCmdImageNci.u32FileVersionInactive);
                            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNfcOtaCmdImageNci.u32TotalImageInactive  = x%08x / d%d", sNfcOtaCmdImageNci.u32TotalImageInactive,  sNfcOtaCmdImageNci.u32TotalImageInactive);
                            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNfcOtaCmdImageNci.u32SavedImageInactive  = x%08x / d%d", sNfcOtaCmdImageNci.u32SavedImageInactive,  sNfcOtaCmdImageNci.u32SavedImageInactive);
                            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNfcOtaCmdImageNci.u32OffsetImageInactive = x%08x / d%d", sNfcOtaCmdImageNci.u32OffsetImageInactive, sNfcOtaCmdImageNci.u32OffsetImageInactive);
                        }
                        #endif
                        /* Convert CmdImage to buffer */
                        NFC_OTA_vCmdImageToBuffer(&sNfcOtaCmdImageNci, au8Sram);
                        /* Write NCI header back to NTAG */
                        eNciOtaStatus = NCI_OTA_eWrite(au8Sram);
                    }
                    /* Unrecognised command ? */
                    else
                    {
                        /* Debug */
                        DBG_vPrintf(TRACE_APP_NCI, ", FAILED (unrecognised command)");
                        /* Failed LED state */
                        eAppNciLed = E_APP_NCI_LED_PRESENT_FAIL;
                        /* Stop ota processing */
                        eNciOtaStatus = NCI_OTA_eStop();
                        /* Go to application mode */
                        eAppNciMode = E_APP_NCI_MODE_APP;
                        /* Register callback */
                        NCI_vRegCbEvent(APP_cbNciEvent);
                    }
                }
                /* Failed to read ? */
                else
                {
                    /* Debug */
                    DBG_vPrintf(TRACE_APP_NCI, ", FAILED");
                    /* Failed LED state */
                    eAppNciLed = E_APP_NCI_LED_PRESENT_FAIL;
                    /* Stop ota processing */
                    eNciOtaStatus = NCI_OTA_eStop();
                    /* Go to application mode */
                    eAppNciMode = E_APP_NCI_MODE_APP;
                    /* Register callback */
                    NCI_vRegCbEvent(APP_cbNciEvent);
                }
            }
            /* Finished writing ntag data ? */
            else if (E_NCI_OTA_WRITE_FAIL == eNciOtaStatus
            ||       E_NCI_OTA_WRITE_OK   == eNciOtaStatus)
            {
                /* Write ok ? */
                if (E_NCI_OTA_WRITE_OK == eNciOtaStatus)
                {
                    /* No more data to write ? */
                    if (u32FlashOffset >= sNfcOtaCmdImageNci.u32TotalImageInactive)
                    {
                        /* Debug */
                        DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "%d: APP_cbNciTimer()", u32AppNciMs);
                        DBG_vPrintf(TRACE_APP_NCI, ", NCI_OTA_eTick() = %d", eNciOtaStatus);
                        /* Debug */
                        DBG_vPrintf(TRACE_APP_NCI, " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SUCCESS OsaMs = %d", (OSA_TimeGetMsec() - u32OtaNfcStartMsOsa));
                        /* OK LED state */
                        eAppNciLed = E_APP_NCI_LED_PRESENT_OK;
                        /* Stop network processing */
                        eNciOtaStatus = NCI_OTA_eStop();
                        /* Go to application mode */
                        eAppNciMode = E_APP_NCI_MODE_APP;
                        /* Register callback */
                        NCI_vRegCbEvent(APP_cbNciEvent);
                    }
                    /* More data to write */
                    else
                    {
                        uint32 u32Percent;
                        uint32 u32Copy;

                        /* Calculate percentage */
                        u32Percent = ((u32FlashOffset * 100) / sNfcOtaCmdImageNci.u32TotalImageInactive);
                        /* Debug */
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, NFC_DBG_NL "%d: APP_cbNciTimer()", u32AppNciMs);
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, ", NCI_OTA_eTick() = %d", eNciOtaStatus);
                        /* Debug */
                        DBG_vPrintf(TRACE_APP_NCI_VERBOSE, ", WRITING %d%%", u32Percent);
                        /* Is the percentage different to the last reported percentage and on a 5% boundary ? */
                        if (u32Percent != u32OtaNfcPercent && (u32Percent % 5) == 0)
                        {
                            /* Note reported percent */
                            u32OtaNfcPercent = u32Percent;
                            /* Debug */
                            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "u32OtaNfcPercent = %d%%", u32OtaNfcPercent);
                        }
                        /* Calculate the total data left to transfer */
                        u32Copy = sNfcOtaCmdImageNci.u32TotalImageInactive - u32FlashOffset;
                        /* Too much for one command ? */
                        if (u32Copy > NFC_OTA_CMD_DATA_DATA_SIZE)
                        {
                            /* Limit to amount we can copy */
                            u32Copy = NFC_OTA_CMD_DATA_DATA_SIZE;
                        }
                        /* Build data command */
                        sNfcOtaCmdData.u8Cmd     = NFC_CMD_OTA_DATA;
                        sNfcOtaCmdData.u32Offset = u32FlashOffset;
                        /* Can read data from flash ? */
                        if (NFC_OTA_bFlashReadBytes(sNfcOtaCmdData.au8Data, (FLASH_INACTIVE_START + u32FlashOffset), u32Copy))
                        {
                            /* Increment flash offset */
                            u32FlashOffset += u32Copy;
                            /* Not last packet ? */
                            if (u32FlashOffset < sNfcOtaCmdImageNci.u32TotalImageInactive)
                            {
                                /* Set more data flag */
                                sNfcOtaCmdData.u8Cmd |= NFC_CMD_OTA_FLAG_MORE;
                            }
                            /* Convert buffer from CmdData */
                            NFC_OTA_vCmdDataToBuffer(&sNfcOtaCmdData, au8Sram);
                            /* Write NCI data back to NTAG */
                            eNciOtaStatus = NCI_OTA_eWrite(au8Sram);
                        }
                        /* Couldn't read data from flash ? */
                        else
                        {
                            /* Debug */
                            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "%d: APP_cbNciTimer()", u32AppNciMs);
                            DBG_vPrintf(TRACE_APP_NCI, ", NCI_OTA_eTick() = %d", eNciOtaStatus);
                            /* Debug */
                            DBG_vPrintf(TRACE_APP_NCI, " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>> FAILED (FLASH)");
                            /* Failed LED state */
                            eAppNciLed = E_APP_NCI_LED_PRESENT_FAIL;
                            /* Stop network processing */
                            eNciOtaStatus = NCI_OTA_eStop();
                            /* Go to application mode */
                            eAppNciMode = E_APP_NCI_MODE_APP;
                            /* Register callback */
                            NCI_vRegCbEvent(APP_cbNciEvent);
                        }
                    }
                }
                /* Failed to write ? */
                else
                {
                    /* Debug */
                    DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "%d: APP_cbNciTimer()", u32AppNciMs);
                    DBG_vPrintf(TRACE_APP_NCI, ", NCI_OTA_eTick() = %d", eNciOtaStatus);
                    /* Debug */
                    DBG_vPrintf(TRACE_APP_NCI, " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>> FAILED (NCI)");
                    /* Failed LED state */
                    eAppNciLed = E_APP_NCI_LED_PRESENT_FAIL;
                    /* Stop network processing */
                    eNciOtaStatus = NCI_OTA_eStop();
                    /* Go to application mode */
                    eAppNciMode = E_APP_NCI_MODE_APP;
                    /* Register callback */
                    NCI_vRegCbEvent(APP_cbNciEvent);
                }
            }
            /* Went idle unexpectedly ? */
            else if (E_NCI_OTA_IDLE == eNciOtaStatus)
            {
                /* Debug */
                DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "%d: APP_cbNtagTimer()", u32AppNciMs);
                DBG_vPrintf(TRACE_APP_NCI, ", NCI_OTA_eTick() = %d >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>", eNciOtaStatus);
                /* Set LED state */
                eAppNciLed = E_APP_NCI_LED_PRESENT_FAIL;
                /* Stop network processing */
                eNciOtaStatus = NCI_OTA_eStop();
                /* Go to application mode */
                eAppNciMode = E_APP_NCI_MODE_APP;
                /* Register callback */
                NCI_vRegCbEvent(APP_cbNciEvent);
            }
        }
        break;
#endif

        /* Others (application mode) ? */
        default:
        {
            /* Maintain driver NTAG state machine */
            NCI_vTick(APP_NCI_TICK_MS);
        }
        break;
    }

    /* LED handling ? */
    #if ((defined APP_NCI_LED1_PIN) || (defined APP_NCI_LED1_PIN))
    #if (JENNIC_CHIP_FAMILY == JN518x)
    {
        /* Assume both LEDs will be off */
        uint8 u8Led1 = 0;
        uint8 u8Led2 = 0;
        /* Which command ? */
        switch (u8NciCommand)
        {
#ifdef NCI_OTA_H_
            /* Ota ? */
            case NFC_CMD_OTA_IMAGE:
            {
                /* Which led state ? */
                switch (eAppNciLed)
                {
                    /* Present ok (finished) ? */
                    case E_APP_NCI_LED_PRESENT_OK:
                    {
                        /* Led 1 and 2 on */
                        u8Led1 = u8Led2 = 1;
                    }
                    break;
                    /* Present busy ? */
                    case E_APP_NCI_LED_PRESENT_BUSY:
                    {
                        /* Led 1 and 2 flash fast */
                        if (u32AppNciTicks & 0x8) u8Led1 = u8Led2 = 1;
                    }
                    break;
                    /* Absent ? */
                    case E_APP_NCI_LED_ABSENT:
                    {
                        /* Led 1 and 2 flash slow */
                        if (u32AppNciTicks & 0x80) u8Led1 = u8Led2 = 1;
                    }
                    /* Default (including PRESENT_FAIL) - do nothing */
                    default: break;
                }
            }
            break;
#endif

            /* Join with code ? */
            case NFC_CMD_NWK_NCI_JOIN_WITH_CODE:
            {
                /* Which led state ? */
                switch (eAppNciLed)
                {
                    /* Present ok (finished) ? */
                    case E_APP_NCI_LED_PRESENT_OK:
                    {
                        /* Led 1 on */
                        u8Led1 = 1;
                    }
                    break;
                    /* Present busy ? */
                    case E_APP_NCI_LED_PRESENT_BUSY:
                    {
                        /* Led 1 flash fast */
                        if (u32AppNciTicks & 0x8) u8Led1 = 1;
                    }
                    break;
                    /* Absent ? */
                    case E_APP_NCI_LED_ABSENT:
                    {
                        /* Led 1 flash slow */
                        if (u32AppNciTicks & 0x80) u8Led1 = 1;
                    }
                    /* Default (including PRESENT_FAIL) - do nothing */
                    default: break;
                }
            }
            break;

            /* Factory reset (with leave) ? */
            case NFC_CMD_NWK_NCI_FACTORY_RESET:
            {
                /* Which led state ? */
                switch (eAppNciLed)
                {
                    /* Present ok (finished) ? */
                    case E_APP_NCI_LED_PRESENT_OK:
                    {
                        /* Led 2 on */
                        u8Led2 = 1;
                    }
                    break;
                    /* Present busy ? */
                    case E_APP_NCI_LED_PRESENT_BUSY:
                    {
                        /* Led 2 flash fast */
                        if (u32AppNciTicks & 0x8) u8Led2 = 1;
                    }
                    break;
                    /* Absent ? */
                    case E_APP_NCI_LED_ABSENT:
                    {
                        /* Led 2 flash slow */
                        if (u32AppNciTicks & 0x80) u8Led2 = 1;
                    }
                    /* Default (including PRESENT_FAIL) - do nothing */
                    default: break;
                }
            }
            break;

            /* Default - do nothing */
            default: break;
        }
        /* Set LED states */
        #if (defined APP_NCI_LED1_PIN)
        {
            GPIO_PinWrite(GPIO, 0, APP_NCI_LED1_PIN, u8Led1);
        }
        #endif
        #if (defined APP_NCI_LED2_PIN)
        {
            GPIO_PinWrite(GPIO, 0, APP_NCI_LED2_PIN, u8Led2);
        }
        #endif
    }
    #endif
    #endif

}

/****************************************************************************
 *
 * NAME: APP_cbNciEvent
 *
 * DESCRIPTION:
 * Called when a tag event takes place
 *
 * RETURNS:
 * None
 *
 ****************************************************************************/
PUBLIC  void        APP_cbNciEvent( /* Called when an event takes place */
        teNciEvent  eNciEvent,          /* Event raised */
        uint32      u32Address,
        uint32      u32Length,
        uint8       *pu8Data)           /* Event data (NULL if no data) */
{
    /* Debug */
    DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "%d: APP_cbNciEvent(%d, %d, %d)", u32AppNciMs, eNciEvent, u32Address, u32Length);
    /* Which event ? */
    switch (eNciEvent)
    {
        /* Present ? */
        case E_NCI_EVENT_PRESENT:
        {
            /* Not already present ? */
            if (E_APP_NCI_STATE_PRESENT != eAppNciState)
            {
                /* Debug */
                DBG_vPrintf(TRACE_APP_NCI, ", eAppNciState = PRESENT");
                /* Go to present state */
                eAppNciState = E_APP_NCI_STATE_PRESENT;
                /* Set LED state */
                eAppNciLed = E_APP_NCI_LED_PRESENT_BUSY;
                /* Commissioning or decommissioning ? */
                if (u8NciCommand == NFC_CMD_NWK_NCI_JOIN_WITH_CODE
                ||  u8NciCommand == NFC_CMD_NWK_NCI_FACTORY_RESET)
                {
                    /* Not in NWK mode ? */
                    if (E_APP_NCI_MODE_NWK != eAppNciMode)
                    {
                        /* Debug */
                        DBG_vPrintf(TRACE_APP_NCI, ", eAppNciMode = NWK");
                        /* Go to network mode */
                        eAppNciMode = E_APP_NCI_MODE_NWK;
                        /* Start reading nci network data */
                        NCI_NWK_eRead(&u32NfcNwkAddress, &sNfcNwkPayload);
                    }
                }
#ifdef NCI_OTA_H_
                /* OTA ? */
                else if (u8NciCommand == NFC_CMD_OTA_IMAGE)
                {
                    /* Not in OTA mode ? */
                    if (E_APP_NCI_MODE_OTA != eAppNciMode)
                    {
                        /* Debug */
                        DBG_vPrintf(TRACE_APP_NCI, ", eAppNciMode = OTA");
                        /* Go to network mode */
                        eAppNciMode = E_APP_NCI_MODE_OTA;
                        /* Start reading nci network data */
                        NCI_OTA_eRead(au8Sram);
                    }
                }
#endif
            }
        }
        break;

        /* Absent ? */
        case E_NCI_EVENT_ABSENT:
        {
            /* Not already absent ? */
            if (E_APP_NCI_STATE_ABSENT != eAppNciState)
            {
                /* Debug */
                DBG_vPrintf(TRACE_APP_NCI, ", eAppNciState = ABSENT");
                /* Go to absent state */
                eAppNciState = E_APP_NCI_STATE_ABSENT;
                /* Set LED state */
                eAppNciLed = E_APP_NCI_LED_ABSENT;
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
 * NAME: APP_bNciNtagCmdJoinWithCode
 *
 * DESCRIPTION:
 * Handle join with code command from ntag
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE bool_t APP_bNciNtagCmdJoinWithCode(void)
{
    bool_t  bWrite = FALSE;
    uint64 u64ExtPanId;
    uint8   u8ActiveKeySeqNumber = 0;

    /* Debug */
    DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "%d: bNciNtagCmdJoinWithCode()", u32AppNciMs);
    /* Get extended PAN ID */
    u64ExtPanId = ZPS_u64AplZdoGetNetworkExtendedPanId();
    /* Debug */
    DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    u64ExtPanId    = %08x:%08x", (uint32)(u64ExtPanId >> 32), (uint32)(u64ExtPanId & 0xFFFFFFFF));

    /* Got a valid extended PAN ID (in a network) ? */
    if (u64ExtPanId != 0)
    {
        /* Issuing join with icode command ? */
        if (u8NciCommand == NFC_CMD_NWK_NCI_JOIN_WITH_CODE)
        {
            uint8                              u8Byte;
            uint16                            u16Crc;
            uint8                            *pu8NetworkKey;

            /* Get pointer to network key */
            pu8NetworkKey = (uint8*)ZPS_pvNwkSecGetNetworkKey(ZPS_pvAplZdoGetNwkHandle());
            /* Debug */
            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    pu8NetworkKey           =");
            for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_KEY_SIZE; u8Byte++) DBG_vPrintf(TRACE_APP_NCI, " %02x", pu8NetworkKey[u8Byte]);
            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNtag.u64ExtAddr        = %08x:%08x", (uint32)(sNfcNwkPayload.sNtag.u64ExtAddress >> 32), (uint32)(sNfcNwkPayload.sNtag.u64ExtAddress & 0xFFFFFFFF));
            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNtag.pu8InstallCode    =");
            for (u8Byte = 0; u8Byte < NFC_NWK_PAYLOAD_KEY_SIZE; u8Byte++) DBG_vPrintf(TRACE_APP_NCI, " %02x", sNfcNwkPayload.sNtag.au8Key[u8Byte]);
            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    sNtag.u16Crc            = x%04x", sNfcNwkPayload.sNtag.u16Crc);
            /* Application encryption ? */
            #ifdef APP_ICODE_H_
            {
                /* Recalculate CRC */
                u16Crc = APP_u16InstallCodeCrc(sNfcNwkPayload.sNtag.au8Key, NFC_NWK_PAYLOAD_KEY_SIZE);
            }
            /* Stack encryption ? */
            #else
            {
                /* Recalculate CRC */
                u16Crc = ZPS_u16crc(sNfcNwkPayload.sNtag.au8Key, NFC_NWK_PAYLOAD_KEY_SIZE);
            }
            #endif
            /* Debug */
            DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    u16Crc                 = x%04x", u16Crc);
            /* Do the CRCs match ? */
            if (sNfcNwkPayload.sNtag.u16Crc == u16Crc)
            {
                /* Application encryption ? */
                #ifdef APP_ICODE_H_
                {
                    /* Generate encrypted network key */
                    bWrite = APP_bInstallCode(TRUE,                               /* bool_t      bEncrypt, */
                                              /* Inputs */
                                              sNfcNwkPayload.sNtag.au8Key,        /* uint8*    pu8InstallCode, */
                                              pu8NetworkKey,                      /* uint8*    pu8Input, */
                                              sNfcNwkPayload.sNtag.u64ExtAddress, /* uint64    u64ExtAddress, */
                                              /* Outputs */
                                              sNfcNwkPayload.sNci.au8Key,         /* uint8*    pu8Output, */
                                              sNfcNwkPayload.sNci.au8Mic);        /* uint8*    pu8Mic */
                }
                /* Stack encryption ? */
                #else
                {
                    BDB_teStatus                       eBdbStatus;
                    BDB_tsOobWriteDataToAuthenticate   sAuthenticate;
                    uint8                            au8DataEncrypted[64];
                    uint16                           u16DataSize;

                    /* Populate data to authenticate */
                    sAuthenticate.u64ExtAddr     = sNfcNwkPayload.sNtag.u64ExtAddress;
                    sAuthenticate.pu8InstallCode = sNfcNwkPayload.sNtag.au8Key;
                    /* Generate encrypted network key */
                    eBdbStatus = BDB_eOutOfBandCommissionGetDataEncrypted(&sAuthenticate,
                                                                          au8DataEncrypted,
                                                                          &u16DataSize);
                    /* Success ? */
                    if (BDB_E_SUCCESS == eBdbStatus)
                    {
                        ZPS_tsNwkNib                *psNib;

                        /* Transfer data into structure */
                        memcpy(sNfcNwkPayload.sNci.au8Key, &au8DataEncrypted[ 8], NFC_NWK_PAYLOAD_KEY_SIZE);
                        memcpy(sNfcNwkPayload.sNci.au8Mic, &au8DataEncrypted[24], NFC_NWK_PAYLOAD_MIC_SIZE);
                        /* Note active key sequence number */
                        psNib = ZPS_psAplZdoGetNib();
                        u8ActiveKeySeqNumber = psNib->sPersist.u8ActiveKeySeqNumber;
                        /* Write data back to NTAG */
                        bWrite = TRUE;
                    }
                }
                #endif
                /* Success ? */
                if (TRUE == bWrite)
                {
                    /* Clear ntag installation code data for writing */
                    memset(sNfcNwkPayload.sNtag.au8Key, 0, sizeof(sNfcNwkPayload.sNtag.au8Key));
                    sNfcNwkPayload.sNtag.u16Crc = 0;
                    /* Set nci command data for writing */
                    sNfcNwkPayload.sNci.u8Command = NFC_CMD_NWK_NCI_JOIN_WITH_CODE;
                }
            }
        }
        /* Issuing factory reset (with leave) command ? */
        else if (u8NciCommand == NFC_CMD_NWK_NCI_FACTORY_RESET)
        {
            /* Clear key and mic */
            memset(sNfcNwkPayload.sNci.au8Key, 0, NFC_NWK_PAYLOAD_KEY_SIZE);
            memset(sNfcNwkPayload.sNci.au8Mic, 0, NFC_NWK_PAYLOAD_MIC_SIZE);
            /* Set nci command data for writing */
            sNfcNwkPayload.sNci.u8Command = NFC_CMD_NWK_NCI_FACTORY_RESET;
            /* Write data back to the NTAG */
            bWrite = TRUE;
        }
    }

    /* Want to write back to tag ? */
    if (TRUE == bWrite)
    {
        ZPS_tsAplAfSimpleDescriptor   sDesc;

        /* Set common nci data for writing */
        sNfcNwkPayload.sNci.u8Sequence      = ++u8Sequence;
        sNfcNwkPayload.sNci.u16DeviceId     = 0xffff;
        sNfcNwkPayload.sNci.u64ExtAddress   = ZPS_u64AplZdoGetIeeeAddr();
        sNfcNwkPayload.sNci.u64ExtPanId     = u64ExtPanId;
        sNfcNwkPayload.sNci.u16ShortAddress = ZPS_u16AplZdoGetNwkAddr();
        sNfcNwkPayload.sNci.u8Channel       = ZPS_u8AplZdoGetRadioChannel();
        sNfcNwkPayload.sNci.u16PanId        = ZPS_u16AplZdoGetNetworkPanId();
        sNfcNwkPayload.sNci.u8KeySeqNum     = u8ActiveKeySeqNumber;
        /* Can we get the simple descriptor for the passed in endpoint */
        if (ZPS_eAplAfGetSimpleDescriptor(u8Endpoint, &sDesc) == E_ZCL_SUCCESS)
        {
            /* Overwrite with correct id */
            sNfcNwkPayload.sNci.u16DeviceId = sDesc.u16DeviceId;
        }
    }

    /* Debug */
    DBG_vPrintf(TRACE_APP_NCI, NFC_DBG_NL "    bWrite                  = %d", bWrite);

    return bWrite;
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
