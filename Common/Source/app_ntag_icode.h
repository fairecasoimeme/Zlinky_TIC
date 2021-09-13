/*****************************************************************************
 *
 * MODULE:          JN-AN-1243 Base Device Application
 *
 * COMPONENT:       app_ntag_icode.h
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
/*!
 * \file  app_ntag_icode.h
 *
 * \brief ZigBee 3.0 NTAG network commissioning
 *
 * app_ntag_icode.h contains application APIs that can be used to commission ZigBee 3.0
 * devices into a network using NFC to write network information into an NTAG. The
 * code in app_ntag_icode.h makes use of libNTAG to interact with the NTAG.
 *
 * The changes required to add these features to ZigBee 3.0 applications are
 * described in the sections on the individual functions and have already been made
 * to the ZigBee 3.0 Application Notes. The changes in the source code are
 * all wrapped by \#ifdef APP_NTAG_ICODE and the NTAG functionality can be disabled
 * in the makefiles or on the command line by setting APP_NTAG_ICODE = 0.
 *
 ***************************************************************************/
#ifndef APP_NTAG_ICODE_H_
#define APP_NTAG_ICODE_H_

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include <ntag.h>

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define APP_NTAG_ADDRESS          0xFFU  /*!< I2C Address (0xFF for automatic detection) */
#if (defined JENNIC_CHIP_FAMILY_JN516x)
#define APP_NTAG_I2C_SCL          14  /*!< JN516x use standard DIO14/15 for I2C */
#elif (defined JENNIC_CHIP_FAMILY_JN517x)
#define APP_NTAG_I2C_SCL          3   /*!< JN517x use standard DIO3/2 for I2C */
#elif (defined JENNIC_CHIP_FAMILY_JN518x)
//#define APP_NTAG_I2C_SCL          10  /*!< JN518x use DIO10/11 on I2C0 */
#define APP_NTAG_I2C_SCL          0xff  /*!< JN518x use internal on I2C2 */
#else
#error JENNIC_CHIP_FAMILY not set!
#endif
#define APP_NTAG_I2C_FREQUENCY_HZ 100000 /*!< I2C frequency value in Hz */
#if (defined JENNIC_CHIP_FAMILY_JN518x)
#define APP_NTAG_TICK_MS          ZTIMER_TIME_MSEC(5) /*!< Interval of tick timer in ms */
#else
#define APP_NTAG_TICK_MS          5      /*!< Interval of tick timer in ms */
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 *
 *  NAME:  APP_bNtagPdmLoad
 */
/*! \brief Reads NTAG PDM records during initialisation and initiates out of
 * band commissioning if an appropriate record is found.
 *
 * This function should be called during initialisation of the application
 * before starting the stack.
 *
 * Two PDM records are used by app_ntag.h, with the IDs defined in PDM_IDs.h
 * which are used in the following ways:
 *
 * \li \b PDM_ID_APP_NFC_ICODE stores the installation code and its CRC in a
 * tsNfcNwkInstallCode structure. If the record cannot be read from the PDM
 * a random installation code is generated and its CRC calculated which is
 * then stored in the PDM.
 *
 * \li \b PDM_ID_APP_NFC_NWK_NCI stores network commissioning data written to the
 * NTAG during NFC commissioning in a tsNfcNwkNci structure. If a valid
 * NFC_NWK_NCI_CMD_JOIN_WITH_CODE command is present in the record read
 * from the PDM the BDB_u8OutOfBandCommissionStartDevice() is called to
 * commission the device into the network. An APP_vOobcSetRunning() function,
 * which must be present in the application, is called to allow the
 * application to take appropriate actions to move to a running state.
 *
 * \retval TRUE  BDB_u8OutOfBandCommissionStartDevice() has been called and the
 * stack should \b not be started by the application calling BDB_vStart().
 * \retval FALSE BDB_u8OutOfBandCommissionStartDevice() has \b not been called and the
 * stack may be started by the application calling BDB_vStart() if appropriate.
 *
 ****************************************************************************/
PUBLIC bool_t APP_bNtagPdmLoad(void);

/****************************************************************************
 *
 *  NAME:  APP_vNtagStart
 */
/*! \brief Starts processing data in the NTAG
 *
 * This function should be called in the following circumstances:
 *
 * \li At initialisation: this allows device information to be written into
 * an NTAG at startup and also to read and react to data that may have been
 * written into the NTAG whilst the device was powered down.
 *
 * \li On field detect (FD) input change: The FD input from the NTAG is raised
 * and lowered when the NTAG is placed into or removed from an NFC reader's RF
 * field. When placed into a field this status is simply tracked, when removed
 * from a field the process of reading and writing data to the NTAG will be
 * initiated. It is assumed that the standard button handling code (in
 * app_buttons.h) is used to generate interupts and app_ntag_icode.c uses the
 * APP_BUTTONS_NFC_FD define to determine the FD input DIO line.
 *
 * \li On successfully joining a network: to write the current network
 * data into the NTAG.
 *
 * APP_vNtagStart() starts the u8TimerNtag ZTIMER which must be present in the
 * application (usually in app_main.c). This timer is configured to prevent
 * the device sleeping whilst the ZTIMER is running. At the end of NTAG processing
 * the ZTIMER is allowed to expire and thus allow sleeping again.
 *
 ****************************************************************************/
PUBLIC void APP_vNtagStart(
    uint8 u8ApplicationEndpoint /*!< Main application endpoint, used to determine the Device ID to be written into the NTAG */
    );

/****************************************************************************
 *
 *  NAME:  APP_vNtagStop
 */
/*! \brief Stops processing data in the NTAG
 *
 * This function may be called to abort NTAG processing. The current ZigBee 3.0
 * application notes do not make use of this function.
 *
 ****************************************************************************/
PUBLIC void APP_vNtagStop(void);

/****************************************************************************
 *
 *  NAME:  APP_cbNtagTimer
 */
/*! \brief ZTIMER callback function
 *
 * This is the callback function used by the NTAG ZTIMER and drives the
 * processing of NTAG data.
 *
 * \warning This function should not be called directly by the
 * application code.
 *
 * If a valid command has been read from the NTAG during processing it will
 * initiate the following actions when NTAG processing is complete:
 *
 * \li NFC_NWK_NCI_CMD_FACTORY_RESET: The function ZPS_eAplZdoLeaveNetwork()
 * is called so the device cleanly leaves the network. If the ZPS_eAplZdoLeaveNetwork()
 * function fails then APP_vFactoryResetRecords() is called, and must be present
 * in the application, to reset the device to its factory state then the device
 * is restarted.
 *
 * \li NFC_NWK_NCI_CMD_JOIN_WITH_CODE: The commissioning data read from the
 * NTAG is written into the PDM PDM_ID_APP_NFC_NWK_NCI record and the device
 * is restarted. The data then gets picked up and applied by the call to
 * APP_bNtagPdmLoad() during initialisation.
 *
 ****************************************************************************/
PUBLIC void APP_cbNtagTimer(void *pvParams);

/****************************************************************************
 *
 *  NAME:  APP_cbNtagEvent()
 */
/*! \brief NTAG event callback function
 *
 * This is the callback function used by the NTAG library to pass events and
 * data from the NTAG to the application.
 *
 * This function initiates the reading of the NTAG data when the NTAG is
 * removed from an NCI field indicated by the E_NTAG_EVENT_ABSENT event.
 *
 * \warning This function should not be called directly by the
 * application code.
 *
 ****************************************************************************/
PUBLIC void APP_cbNtagEvent(
       teNtagEvent  eNtagEvent, /*!< Event raised */
       uint32       u32Address, /*!< Byte address in NTAG of data relating to the event */
       uint32       u32Length,  /*!< Length of data relating to the event */
       uint8        *pu8Data    /*!< Pointer to data relating to the event */
       );

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#endif /* APP_NTAG_ICODE_H_ */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
