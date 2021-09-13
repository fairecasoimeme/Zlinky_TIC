/*! *********************************************************************************
* Copyright 2016-2020 NXP
* All rights reserved.
*
** SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/*!
 * \file  nci.h
 *
 * \brief NCI driver for reading and writing data (interface)
 *
 * nci.h contains low level APIs for using an NFC reader to reading and writing raw
 * data to the NTAGs.
 *
 * The typical set up sequence is as follows:
 *
 * \li NCI_vInitialise() is called to initialise the hardware.
 * \li NCI_vRegCbEvent() is called to register the event callback function in the
 * application the library should call to inform the application of NCI events.
 * APP_NciCbEvent() is used in these examples.
 * \li NCI_vTick() should be called regularly (recommended every 5ms) to provide
 * processing time to the library.
 * \li APP_NciCbEvent() will be called by the library to pass events to the
 * application layer. The E_NCI_EVENT_ABSENT and E_NCI_EVENT_PRESENT events will
 * be raised when a NTAG is removed from and placed into an NFC reader's field.
 *
 * A typical sequence to read data is shown below. Data can only be read
 * when a NTAG is in the reader's field in response (indicated by the E_NCI_EVENT_PRESENT
 * event). When the callback
 * function is registered at initialisation the E_NCI_EVENT_ABSENT will be raised if
 * a NTAG is not in the field.
 *
 * \li NCI_vTick() is called by the application
 * \li APP_NciCbEvent(E_NCI_EVENT_PRESENT) is called in the application
 * \li NCI_bRead() is called by the application, the byte address and length
 * of data to be read from the NTAG is provided along with a buffer to store the read
 * data in. This call will return TRUE if the request is accepted.
 * \li NCI_vTick() continues to be called regularly by the application.
 * \li APP_NciCbEvent(E_NCI_EVENT_READ_OK) is called in the application if the read
 * is successful. The data read from the NTAG will be in the buffer provided by the call
 * to NCI_bRead(), this data pointer is also provided as a parameter to APP_NciCbEvent()
 * \li APP_NciCbEvent(E_NCI_EVENT_READ_FAIL) is called in the application if the read
 * is unsuccessful.
 * \li NCI_bEnd() should be called if the application has finished interacting with the NTAG.
 *
 * A typical sequence to write data is shown below. Data can only be written
 * when a NTAG is in the reader's field (indicated by the E_NCI_EVENT_PRESENT event).
 * Data may also be written following a successful read as shown below:
 *
 * \li NCI_vTick() is called by the application
 * \li APP_NciCbEvent(E_NCI_EVENT_READ_OK) is called in the application following a
 * successful read.
 * \li NCI_bWrite() is called by the application, the byte address and length
 * of data to be written to the NTAG is provided along with a buffer containing the data
 * to be written. This call will return TRUE if the request is accepted.
 * \li NCI_vTick() continues to be called regularly by the application.
 * \li APP_NciCbEvent(E_NCI_EVENT_WRITE_OK) is called in the application if the write
 * is successful.
 * \li APP_NciCbEvent(E_NCI_EVENT_WRITE_FAIL) is called in the application if the write
 * is unsuccessful.
 * \li NCI_bEnd() should be called if the application has finished interacting with the NTAG.
 *
 * The message sequence chart below shows a sequence of function calls for initialisation,
 * a data read and a data write:
 *
 * \msc
 * APP, NCI;
 * APP=>NCI [linecolor="black", textcolor="black", label="NCI_vInitialise()"];
 * APP=>NCI [linecolor="black", textcolor="black", label="NCI_vRegCbEvent(APP_NciCbEvent)"];
 * APP=>NCI [linecolor="green", textcolor="green", label="NCI_vTick()"];
 * APP<=NCI [linecolor="red",   textcolor="red",   label="APP_NciCbEvent(E_NCI_EVENT_ABSENT)"];
 * APP>>NCI [linecolor="red",   textcolor="red",   label="APP_NciCbEvent() returns"];
 * APP<<NCI [linecolor="green", textcolor="green", label="NCI_vTick() returns"];
 * APP:>NCI [linecolor="green", textcolor="green", label="Regular calls to NCI_vTick()"];
 * ---      [linecolor="black", textcolor="black", label="until NTAG placed in field"];
 * APP=>NCI [linecolor="green", textcolor="green", label="NCI_vTick()"];
 * APP<=NCI [linecolor="red",   textcolor="red",   label="APP_NciCbEvent(E_NCI_EVENT_PRESENT)"];
 * APP=>NCI [linecolor="blue",  textcolor="blue",  label="NCI_bRead()"];
 * APP<<NCI [linecolor="blue",  textcolor="blue",  label="NCI_bRead() returns TRUE"];
 * APP>>NCI [linecolor="red",   textcolor="red",   label="APP_NciCbEvent() returns"];
 * APP<<NCI [linecolor="green", textcolor="green", label="NCI_vTick() returns"];
 * APP:>NCI [linecolor="green", textcolor="green", label="Regular calls to NCI_vTick()"];
 * ---      [linecolor="black", textcolor="black", label="until read successful"];
 * APP=>NCI [linecolor="green", textcolor="green", label="NCI_vTick()"];
 * APP<=NCI [linecolor="red",   textcolor="red",   label="APP_NciCbEvent(E_NCI_EVENT_READ_OK)"];
 * APP=>NCI [linecolor="blue",  textcolor="blue",  label="NCI_bWrite()"];
 * APP<<NCI [linecolor="blue",  textcolor="blue",  label="NCI_bWrite() returns TRUE"];
 * APP>>NCI [linecolor="red",   textcolor="red",   label="APP_NciCbEvent() returns"];
 * APP<<NCI [linecolor="green", textcolor="green", label="NCI_vTick() returns"];
 * APP:>NCI [linecolor="green", textcolor="green", label="Regular calls to NCI_vTick()"];
 * ---      [linecolor="black", textcolor="black", label="until write successful"];
 * APP=>NCI [linecolor="green", textcolor="green", label="NCI_vTick()"];
 * APP<=NCI [linecolor="red",   textcolor="red",   label="APP_NciCbEvent(E_NCI_EVENT_WRITE_OK)"];
 * APP=>NCI [linecolor="blue",  textcolor="blue",  label="NCI_bEnd()"];
 * APP<<NCI [linecolor="blue",  textcolor="blue",  label="NCI_bEnd() returns TRUE"];
 * APP>>NCI [linecolor="red",   textcolor="red",   label="APP_NciCbEvent() returns"];
 * APP<<NCI [linecolor="green", textcolor="green", label="NCI_vTick() returns"];
 * APP:>NCI [linecolor="green", textcolor="green", label="Regular calls to NCI_vTick()"];
 * ---      [linecolor="black", textcolor="black", label="until NTAG removed from field"];
 * APP=>NCI [linecolor="green", textcolor="green", label="NCI_vTick()"];
 * APP<=NCI [linecolor="red",   textcolor="red",   label="APP_NciCbEvent(E_NCI_EVENT_ABSENT)"];
 * APP>>NCI [linecolor="red",   textcolor="red",   label="APP_NciCbEvent() returns"];
 * APP<<NCI [linecolor="green", textcolor="green", label="NCI_vTick() returns"];
 * \endmsc
 ***************************************************************************/
#ifndef NCI_H_
#define NCI_H_

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/
#include "global_types.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
/*! NTAG Events - Passed to application by library calling the registerd callback function */
typedef enum
{
    E_NCI_EVENT_ABSENT,     /*!< Tag has been removed from reader */
    E_NCI_EVENT_PRESENT,    /*!< Tag has been presented to reader */
    E_NCI_EVENT_READ_FAIL,  /*!< Read request failed */
    E_NCI_EVENT_READ_OK,    /*!< Read request succeeded */
    E_NCI_EVENT_WRITE_FAIL, /*!< Write request failed */
    E_NCI_EVENT_WRITE_OK,   /*!< Write request succeeded */
} teNciEvent;

/*! NCI Event Callback function */
typedef void (*tprNciCbEvent)(          /* Called when an event takes place */
        teNciEvent  eNciEvent,          /* Event raised */
        uint32      u32Address,
        uint32      u32Length,
        uint8       *pu8Data);          /* Event data (NULL if no data) */

/****************************************************************************/
/***        Exported Functions (called by application)                    ***/
/****************************************************************************/
/****************************************************************************
 *
 *  NAME:  NCI_vInitialise
 */
/*! \brief Initialises NCI data and hardware
 *
 ****************************************************************************/
void        NCI_vInitialise(
        uint8       u8Address,          /*!< Reader I2C address (0xFF for automatic detection of NPC100 or PN7120) */
        uint8       u8I2cScl,           /*!< 6x: 16   = DIO16
                                         *       others = DIO14
                                         *   7x: 4      = DIO4
                                         *       others = DIO3
                                         *   8x: 6      = I2C1 DIO6
                                         *       12     = I2C1 DIO12
                                         *       15     = I2C0 DIO15
                                         *       0xff   = I2C2 (internal NTAG)
                                         *       others = I2C0 DIO10 */
        uint32      u32FrequencyHz,     /*!< Frequency in Hz */
        uint8       u8OutputVen,        /*!< Output DIO for VEN */
        uint8       u8InputIrq          /*!< Input DIO for IRQ */
        );

/****************************************************************************
 *
 *  NAME:  NCI_vRegCbEvent
 */
/*! \brief Registers callback for NCI events
 *
 ****************************************************************************/
void        NCI_vRegCbEvent(
        tprNciCbEvent prRegCbEvent      /*!< Pointer to event callback function */
        );

/****************************************************************************
 *
 *  NAME:  NCI_vTick
 */
/*! \brief Timer function to drive NCI processing
 *
 * Should be called regularly, every 5ms is recommended.
 *
 ****************************************************************************/
void        NCI_vTick(
        uint32      u32TickMs           /*!< Time in ms since previous call */
        );

/****************************************************************************
 *
 *  NAME:  NCI_bRead
 */
/*! \brief Requests read of data from NTAG in reader's field
 *
 * If the request is successful the final outcome of the read request is
 * indicated by a call to the NCI event callback function, along with the
 * data if successful.
 *
 * \retval TRUE  Request accepted
 * \retval FALSE Request failed
 *
 ****************************************************************************/
bool_t      NCI_bRead(
        uint32      u32ReadAddress,     /*!< Byte address of data to read */
        uint32      u32ReadLength,      /*!< Number of bytes to read */
        uint8       *pu8ReadData        /*!< Buffer to read data into */
        );

/****************************************************************************
 *
 *  NAME:  NCI_bReadVersion
 */
/*! \brief Requests read of version data from NTAG in reader's field
 *
 * If the request is successful the final outcome of the read request is
 * indicated by a call to the NCI event callback function, along with the
 * data if successful.
 *
 * The version information is always 8 bytes in size the buffer and length
 * needs to be set appropriately.
 *
 * \retval TRUE  Request accepted
 * \retval FALSE Request failed
 *
 ****************************************************************************/
bool_t NCI_bReadVersion(
        uint32    u32ReadLength, /*!< Number of bytes to read (minimum 8) */
        uint8    *pu8ReadData    /*!< Buffer to read data into */
        );

/****************************************************************************
 *
 *  NAME:  NCI_bWrite
 */
/*! \brief Requests write of data to NTAG in reader's field
 *
 * If the request is successful the final outcome of the write request is
 * indicated by a call to the NCI event callback function, along with the
 * data if successful.
 *
 * \retval TRUE  Request accepted
 * \retval FALSE Request failed
 *
 ****************************************************************************/
bool_t NCI_bWrite(
        uint32    u32WriteAddress,  /*!< Byte address of write */
        uint32    u32WriteLength,   /*!< Number of bytes to write */
        uint8    *pu8WriteData      /*!< Buffer to write data from */
        );

/****************************************************************************
 *
 *  NAME:  NCI_bEnd
 */
/*! \brief Ends NCI processing
 *
 * This function should be called when the reader has finished processing
 * data in the NTAG in the reader's field.
 *
 * \retval TRUE  Request accepted
 * \retval FALSE Request failed
 *
 ****************************************************************************/
bool_t      NCI_bEnd(void);

/****************************************************************************
 *
 *  NAME:  NCI_u32Ntag
 */
/*! \brief Returns the model of the NTAG in the reader's field.
 *
 * The NCI_bReadVersion() function must have been called previously in
 * order to detect the NTAG model.
 *
 * \retval NFC_NTAG_UNKNOWN  Unknown NTAG model
 * \retval NFC_NTAG_NT3H1101 NT3H1101 NTAG model
 * \retval NFC_NTAG_NT3H1201 NT3H1201 NTAG model
 * \retval NFC_NTAG_NT3H2111 NT3H2111 NTAG model
 * \retval NFC_NTAG_NT3H2211 NT3H2211 NTAG model
 *
 ****************************************************************************/
uint32      NCI_u32Ntag(void);

/****************************************************************************
 *
 *  NAME:  NCI_u32Config
 */
/*! \brief Returns the byte address of the configuration registers of the NTAG in the reader's field
 *
 * The NCI_bReadVersion() function must have been called previously in
 * order to determine the address.
 *
 * \returns Byte address of configuration registers
 *
 ****************************************************************************/
uint32      NCI_u32Config(void);

/****************************************************************************
 *
 *  NAME:  NCI_u32Session
 */
/*! \brief Returns the byte address of the session registers of the NTAG in the reader's field
 *
 * The NCI_bReadVersion() function must have been called previously in
 * order to determine the address.
 *
 * \returns Byte address of session registers
 *
 ****************************************************************************/
uint32      NCI_u32Session(void);

/****************************************************************************
 *
 *  NAME:  NCI_u32Sram
 */
/*! \brief Returns the byte address of the SRAM data of the NTAG in the reader's field
 *
 * The NCI_bReadVersion() function must have been called previously in
 * order to determine the address.
 *
 * \returns Byte address of the SRAM data
 *
 ****************************************************************************/
uint32      NCI_u32Sram(void);

/****************************************************************************
 *
 *  NAME:  NCI_pu8Header
 */
/*! \brief Returns the 16 header bytes of the memory of the NTAG in the reader's field
 *
 * The NCI_bRead() function must have been called previously with a read of
 * these first 16 bytes from address 0.
 *
 * \returns Pointer to header data
 *
 ****************************************************************************/
uint8      *NCI_pu8Header(void);

/****************************************************************************
 *
 *  NAME:  NCI_pu8Version
 */
/*! \brief Returns the 8 version information bytes for the NTAG in the reader's field
 *
 * The NCI_bReadVersion() function must have been called previously.
 *
 * \returns Pointer to version data
 *
 ****************************************************************************/
uint8      *NCI_pu8Version(void);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#endif /* NCI_H_ */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
