/*****************************************************************************
 *
 * MODULE:          JN-AN-1243 Base Device application
 *
 * COMPONENT:       app_nci_icode.h
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
/*!
 * \file  app_nci_icode.h
 *
 * \brief ZigBee 3.0 NCI network commissioning
 *
 * app_nci_icode.h contains application APIs that can be used to operate an attached NCI
 * (reader) to write ZigBee 3.0 network data into other devices fitted with
 * NTAGs to commission them into a network. OTA over NFC features are also provided.
 *
 * The changes required to add these features to ZigBee 3.0 applications are
 * described in the sections on the individual functions and have already been made
 * to the Coordinator device in the JN-AN-1243 ZigBee 3.0 Application Note.
 *
 * The changes in the source code are all wrapped by \#ifdef APP_NCI_ICODE. The NCI
 * functionality is disabled by default in the Coordinator but can be enabled
 * in the makefile or on the command line by setting APP_NCI_ICODE = 1.
 *
 * Similarly the changes in the source code for OTA over NFC are all wrapped
 * by \#ifdef APP_NCI_OTA. The NCI functionality is disabled by default in the
 * Coordinator but can be enabled in the makefile or on the command line by
 * setting APP_NCI_OTA = 1.
 ***************************************************************************/
#ifndef APP_NCI_ICODE_H_
#define APP_NCI_ICODE_H_

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include <nci.h>
#include "OTA.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define APP_NCI_ADDRESS          0xFFU           /*!< I2C Address (0xFF for automatic detection) */
#if (JENNIC_CHIP_FAMILY == JN516x)
#define APP_NCI_I2C_SCL          14              /*!< JN516x use standard DIO14/15 for I2C */
#elif (JENNIC_CHIP_FAMILY == JN517x)
#define APP_NCI_I2C_SCL          3               /*!< JN517x use standard DIO3/2 for I2C */
#elif (JENNIC_CHIP_FAMILY == JN518x)
#define APP_NCI_I2C_SCL          10              /*!< JN518x use DIO10/11 on I2C0 */
#else
#error JENNIC_CHIP_FAMILY not set!
#endif
#define APP_NCI_I2C_FREQUENCY_HZ 400000          /*!< I2C frequency in Hz */
#if (JENNIC_CHIP_FAMILY == JN518x)
#define APP_NCI_TICK_MS          ZTIMER_TIME_MSEC(5) /*!< Interval of tick timer in ms */
#else
#define APP_NCI_TICK_MS          5               /*!< Interval of tick timer in ms */
#endif
#if (JENNIC_CHIP_FAMILY == JN518x)
#define APP_NCI_IRQ_PIN          18              /*!< Sets DIO connected to IRQ pin */
#define APP_NCI_VEN_PIN           7              /*!< Sets DIO connected to VEN pin */
#define APP_NCI_LED1_PIN         16              /*!< Sets DIO for commissioning mode LED */
#define APP_NCI_LED2_PIN          6              /*!< Sets DIO for decommissioning mode LED */
#elif (JENNIC_CHIP_FAMILY == JN517x)
#define APP_NCI_IRQ_PIN          18              /*!< Sets DIO connected to IRQ pin */
#define APP_NCI_VEN_PIN          15              /*!< Sets DIO connected to VEN pin */
#else
#define APP_NCI_IRQ_PIN          17              /*!< Sets DIO connected to IRQ pin */
#define APP_NCI_VEN_PIN           0              /*!< Sets DIO connected to VEN pin */
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 *
 *  NAME:  APP_vNciStart
 */
/*! \brief Starts NCI processing
 *
 * This function should be called during initialisation after the call to BDB_vStart().
 *
 * APP_vNtagStart() starts the u8TimerNci ZTIMER which must be present in the
 * application (usually in app_main.c). This timer runs continuously in order to
 * monitor for and react to NTAGs being placed in the NCI field.
 *
 ****************************************************************************/
PUBLIC void APP_vNciStart(
    uint8 u8ApplicationEndpoint /*!< Main application endpoint, used to determine the Device ID to be written into presented NTAGs */
    );

/****************************************************************************
 *
 *  NAME:  APP_vNciStop
 */
/*! \brief Stops the NCI running
 *
 * This function may be called to abort NCI processing. The current ZigBee 3.0
 * application notes do not make use of this function.
 *
 ****************************************************************************/
PUBLIC void APP_vNciStop(void);

/****************************************************************************
 *
 *  NAME:  APP_vNciCommand
 */
/*! \brief Sets command to issue from NCI side when NTAG is presented
 *
 * Starts and stops the ZTIMER as appropriate. Can use the following commands
 * from nfc_nwk.h:
 *
 * NFC_CMD_NONE
 * NFC_CMD_NWK_NCI_FACTORY_RESET
 * NFC_CMD_NWK_NCI_JOIN_WITH_CODE (default set by APP_vNciStart())
 * NFC_CMD_OTA_IMAGE
 *
 * Returns TRUE for valid command, FALSE for invalid command
 *
 ****************************************************************************/
PUBLIC bool_t APP_bNciCommand(uint8 u8NciCommandNew);

/****************************************************************************
 *
 *  NAME:  APP_cbNciTimer
 */
/*! \brief ZTIMER callback function
 *
 * This is the callback function used by the NCI ZTIMER and drives the
 * processing of NCI data.
 *
 * \warning This function should not be called directly by the
 * application code.
 *
 ****************************************************************************/
PUBLIC void APP_cbNciTimer(void *pvParams);

/****************************************************************************
 *
 *  NAME:  APP_cbNciEvent()
 */
/*! \brief NCI event callback function
 *
 * This is the callback function used by the NCI library to pass events and
 * data from the NCI to the application.
 *
 * This function initiates the reading of data from NTAGs when they are
 * presented to the NCI indicated by the E_NCI_EVENT_PRESENT event.
 *
 * \warning This function should not be called directly by the
 * application code.
 *
 ****************************************************************************/
PUBLIC void APP_cbNciEvent(
       teNciEvent     eNciEvent, /*!< Event raised */
       uint32       u32Address,  /*!< Byte address in NTAG of data relating to the event */
       uint32       u32Length,   /*!< Length of data relating to the event */
       uint8        *pu8Data     /*!< Pointer to data relating to the event */
       );

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#endif /* APP_NCI_ICODE_H_ */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
