/*****************************************************************************
 *
 * MODULE:             Application API header
 *
 * DESCRIPTION:        Select correct interface depending on chip / chip family
 *
 ****************************************************************************
 *
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5148, JN5142, JN5139].
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
 * Copyright NXP B.V. 2012. All rights reserved
 *
 ***************************************************************************/

/**
 * @defgroup g_app_sap Application MAC Service Access Point (SAP)
 */
#ifndef  APP_API_H_INCLUDED
#define  APP_API_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include <mac_sap.h>
//#include <mac_pib.h>

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/**
 * @ingroup g_app_sap
 * @brief Get Buffer routine type
 *
 * Type of Get Buffer callback routine
 */
typedef MAC_DcfmIndHdr_s * (*PR_GET_BUFFER)(void *pvParam);

/**
 * @ingroup g_app_sap
 * @brief Post routine type
 *
 * Type of Post callback routine
 */
typedef void (*PR_POST_CALLBACK)(void *pvParam, MAC_DcfmIndHdr_s *psDcfmInd);

typedef enum
{
    E_APPAPI_RADIO_TX_MODE_STD    = 0,
    E_APPAPI_RADIO_TX_MODE_PROP_1 = 1,
    E_APPAPI_RADIO_TX_MODE_PROP_2 = 2,
    E_APPAPI_RADIO_TX_MODE_RESET  = 0xff
} teAppApiRadioTxMode;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PUBLIC uint32
u32AppApiInit(PR_GET_BUFFER prMlmeGetBuffer,
              PR_POST_CALLBACK prMlmeCallback,
              void *pvMlmeParam,
              PR_GET_BUFFER prMcpsGetBuffer,
              PR_POST_CALLBACK prMcpsCallback,
              void *pvMcpsParam);

PUBLIC void *pvAppApiGetMacHandle(void);

/* MLME calls */
PUBLIC void
vAppApiMlmeRequest(MAC_MlmeReqRsp_s *psMlmeReqRsp,
                   MAC_MlmeSyncCfm_s *psMlmeSyncCfm);

PUBLIC void
vAppApiMcpsRequest(MAC_McpsReqRsp_s *psMcpsReqRsp,
                   MAC_McpsSyncCfm_s *psMcpsSyncCfm);

/* PLME calls */
PUBLIC PHY_Enum_e eAppApiPlmeSet(PHY_PibAttr_e ePhyPibAttribute,
                                 uint32 u32PhyPibValue);
PUBLIC PHY_Enum_e eAppApiPlmeGet(PHY_PibAttr_e ePhyPibAttribute,
                                 uint32 *pu32PhyPibValue);

PUBLIC void *pvAppApiGetMacAddrLocation(void);
PUBLIC void vAppApiSetMacAddrLocation(void *pvNewMacAddrLocation);
PUBLIC void  vAppApiSaveMacSettings(void);
PUBLIC void  vAppApiRestoreMacSettings(void);

PUBLIC void vAppApiEnableBeaconResponse(bool_t bEnable);

PUBLIC void vAppApiSetSecurityMode(MAC_SecutityMode_e eSecurityMode);

PUBLIC void vMAC_RestoreSettings(void);
PUBLIC bool_t bAppApi_CurrentlyScanning(void);

PUBLIC void vAppApiSetComplianceLimits(int8  i8TxMaxPower,
                                       int8  i8TxMaxPowerCh26,
                                       uint8 u8CcaThreshold);
PUBLIC void vAppApiSetRadioTxModes(teAppApiRadioTxMode eTxMode,
                                   teAppApiRadioTxMode eTxModeCh26);
PUBLIC void vAppApiSetHighPowerMode(uint8 u8ModuleID, bool_t bMode);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

#endif  /* APP_API_H_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

