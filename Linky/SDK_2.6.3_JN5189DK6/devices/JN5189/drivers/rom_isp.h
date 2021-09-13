/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ROM_ISP_H_
#define ROM_ISP_H_

#if defined __cplusplus
extern "C" {
#endif

/*!
 * @addtogroup ROM_API
 * @{
 */

/*! @file */

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include "rom_common.h"
#include "rom_secure.h"
#include <stdint.h>

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/*! @brief Each ISP extension function invalid : 0 corresponds to a NULL pointer */
#define ISP_INVALID_EXTENSION (0)

/*! @brief Each ISP command is preceded by a 'flag' byte that tell how to verify the command */
#define ISP_FLAG_HAS_CRC32 (1 << 0) /*!< CRC32 Now deprecated */
#define ISP_FLAG_SIGNED (1 << 1)    /*!< tells that command is RSA signed and authentication
                                     * is checked,
                                     * if unset, the SHA256 is computed and compared against the
                                     * one held in the message, which guarantees integrity  */
#define ISP_FLAG_HAS_NEXT_HASH (1 << 2) /*!< tells to hold the computed hash */

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/*! @brief ISP Message types
 * Only a subset of message types below is supported.
 */
typedef enum
{
    TYPE_ACKNOWLEDGE,
    TYPE_READ_REQUEST,
    TYPE_READ_RESPONSE,
    TYPE_WRITE_REQUEST,
    TYPE_WRITE_RESPONSE,
    TYPE_SET_CS_REQUEST,
    TYPE_SET_CS_RESPONSE,
    TYPE_FL_ERASE_REQUEST,
    TYPE_FL_ERASE_RESPONSE,
    TYPE_FL_PROG_REQUEST,
    TYPE_FL_PROG_RESPONSE,
    TYPE_FL_READ_REQUEST,
    TYPE_FL_READ_RESPONSE,
    TYPE_FL_SECTOR_ERASE_REQUEST,
    TYPE_FL_SECTOR_ERASE_RESPONSE,
    TYPE_FL_WRITE_SR_REQUEST,
    TYPE_FL_WRITE_SR_RESPONSE,
    TYPE_QUIT_REQUEST,
    TYPE_QUIT_RESPONSE,

    TYPE_SET_RESET_REQUEST = 20, /*!< ISP Set Reset request */
    TYPE_SET_RESET_RESPONSE,      /*!< ISP Set Reset response */
    TYPE_SET_RATE_REQUEST,
    TYPE_SET_RATE_RESPONSE,
    TYPE_PACKET_TEST_REQUEST,
    TYPE_PACKET_TEST_RESPONSE,
    TYPE_TRIG_PKT_TEST_REQUEST,
    TYPE_TRIG_PKT_TEST_RESPONSE,
    TYPE_TRIG_PKT_TEST_DATA,
    TYPE_RM_WRITE_REQUEST,
    TYPE_RM_WRITE_RESPONSE,
    TYPE_RM_READ_REQUEST,
    TYPE_RM_READ_RESPONSE,
    TYPE_FP_RUN_REQUEST,           /*!< ISP FP Run request : jump to address if ISP access level and authentication allow it */
    TYPE_FP_RUN_RESPONSE,          /*!< ISP FP Run response */
    TYPE_FL_READ_SR_REQUEST,
    TYPE_FL_READ_SR_RESPONSE,
    TYPE_FL_READ_ID_REQUEST,
    TYPE_FL_READ_ID_RESPONSE,
    TYPE_FL_SET_BAUD_REQUEST,      /*!< ISP FL Set Baud request : set UART speed */
    TYPE_FL_SET_BAUD_RESPONSE,     /*!< ISP FL Set Baud response */
    TYPE_AUTHENTICATE_REQ,
    TYPE_AUTHENTICATE_CHALLENGE,
    TYPE_AUTHENTICATE_RESP,
    TYPE_FL_SELECT_FLASH_REQUEST,
    TYPE_FL_SELECT_FLASH_RESPONSE,
    TYPE_REG_READ_REQUEST,          /*!< not implemented */
    TYPE_REG_READ_RESPONSE,         /*!< not implemented */
    TYPE_REG_WRITE_REQUEST,         /*!< not implemented */
    TYPE_REG_WRITE_RESPONSE,        /*!< not implemented */

    TYPE_GET_CHIP_ID_REQUEST,       /*!< ISP chip id request */
    TYPE_GET_CHIP_ID_RESPONSE,      /*!< ISP chip id response */
    TYPE_GET_FUSE_SECURED_REQUEST,  /*!< not implemented */
    TYPE_GET_FUSE_SECURED_RESPONSE, /*!< not implemented */

    TYPE_MEM_OPEN_REQUEST = 0x40,    /*!< ISP memory open request */
    TYPE_MEM_OPEN_RESPONSE,          /*!< ISP memory open response */
    TYPE_MEM_ERASE_REQUEST,          /*!< ISP memory erase request, applies to internal flash only */
    TYPE_MEM_ERASE_RESPONSE,         /*!< ISP memory erase response */
    TYPE_MEM_BLANK_CHECK_REQUEST,    /*!< ISP memory blank check request, applies to internal flash only */
    TYPE_MEM_BLANK_CHECK_RESPONSE,   /*!< ISP memory blank check response to request */
    TYPE_MEM_READ_REQUEST,           /*!< ISP memory read request, applies to all memory types */
    TYPE_MEM_READ_RESPONSE,          /*!< ISP memory read response */
    TYPE_MEM_WRITE_REQUEST,          /*!< ISP memory write request, applies to all memory types except EFUSE */
    TYPE_MEM_WRITE_RESPONSE,         /*!< ISP memory read response */
    TYPE_MEM_CLOSE_REQUEST,          /*!< ISP memory close request */
    TYPE_MEM_CLOSE_RESPONSE,         /*!< ISP memory close response */
    TYPE_MEM_GET_INFO_REQUEST,       /*!< ISP memory get information of memory geometry and accessibility */
    TYPE_MEM_GET_INFO_RESPONSE,     /*!< ISP memory get information response */

    TYPE_UNLOCK_ISP_REQUEST,        /*!< ISP unlock request: reset a locked device to its pristine state*/
    TYPE_UNLOCK_ISP_RESPONSE,       /*!< ISP unlock response */

    TYPE_USE_CERTIFICATE_REQUEST,   /*!< ISP Use Certificate request */
    TYPE_USE_CERTIFICATE_RESPONSE,  /*!< ISP Use Certificate response */

    TYPE_START_ENCRYPTION_REQUEST,  /*!< ISP Start Encryption request */
    TYPE_START_ENCRYPTION_RESPONSE, /*!< ISP Start Encryption response */

    TYPE_RM_WRITE_IMAGE_REQUEST = 80,
    TYPE_RM_WRITE_IMAGE_RESPONSE,

    TYPE_TST_SRV_REQUEST = 100,
    TYPE_TST_SRV_RESPONSE,
    TYPE_TST_SRV_POLL,
    TYPE_TST_SRV_POLL_RESPONSE,

    TYPE_FL_READ_JEDEC_ID_REQUEST,
    TYPE_FL_READ_JEDEC_ID_RESPONSE,

} teFlashProgCommand;

/*! @brief ISP Status types */
typedef enum
{
    ISP_OK,                       /*!< ISP operation successful */
    NOT_SUPPORTED = -1,           /*!< ISP operation not supported  */
    WRITE_FAIL = -2,              /*!< ISP write failure when writing to FLASH, PSECT,r PFLASH  */
    INVALID_RESPONSE = -3,        /*!< ISP invalid response : not used */
    CRC_ERROR = -4,               /*!< ISP command received CRC incorrect */
    ASSERT_FAIL = -5,             /*!< ISP received too long a message  */
    USER_INTERRUPT = -6,          /*!< ISP User aborted operation: not used */
    READ_FAIL = -7,               /*!< ISP Flash blank check error or Flash excessive ECC errors */
    TST_ERR = -8,                 /*!< not used */
    ISP_NOT_AUTHORISED = -9,      /*!< ISP order authentification failure */
    NO_RESPONSE = -10,            /*!< not used */
    ISP_MEM_INVALID = -11,        /*!< ISP message malformed : addressed to non existant memory */
    ISP_MEM_NOT_SUPPORTED = -12,  /*!< ISP order not supported for memory */
    ISP_MEM_NO_ACCESS = -13,      /*!< ISP access level insufficient  */
    ISP_MEM_OUT_OF_RANGE = -14,   /*!< ISP order addressing memory outisde the intended range */
    ISP_MEM_TOO_LONG = -15,       /*!< ISP buffer insufficient to read requested amount of memory */
    ISP_MEM_BAD_STATE = -16,      /*!< Memory in a state that cannot support operation
                                   * e.g. opening an errored PSECT or PFLASH,
                                   * closing a memory that was not opened */
    ISP_MEM_INVALID_MODE = -17,   /*!< ISP order is malformed : mode incorrect */

    ISP_DEFAULT_HANDLER = -128,
} ISP_STATUS_T;

typedef enum
{
    ISP_MEM_ROM,
    ISP_MEM_FLASH,
    ISP_MEM_RAM,
    ISP_MEM_SPI,
    ISP_MEM_SPIFI,                /*!< Unused SPIFI not handled by ISP */
    ISP_MEM_EFUSE
} ISP_MEMORY_TYPE_E;


/* Forward declarations */
typedef struct __MEM_INFO_T ISP_MEM_INFO_T;
typedef struct __MEM_T ISP_MEMORY_T;
typedef struct __STATE_T ISP_STATE_T;

/*! @brief ISP_MEM_FUNC_T structure of ops method pointers instantiated per memory type  */
typedef struct
{
    ISP_STATUS_T (*open)(ISP_MEMORY_T *mem, void *buffer, uint8_t access);
    ISP_STATUS_T (*close)(ISP_MEMORY_T *mem, void *buffer);
    ISP_STATUS_T (*erase)(ISP_MEMORY_T *mem, void *buffer, uint32_t address, uint32_t length, uint8_t mode);
    ISP_STATUS_T (*blank)(ISP_MEMORY_T *mem, void *buffer, uint32_t address, uint32_t length, uint8_t mode);
    ISP_STATUS_T (*read) (ISP_MEMORY_T *mem, void *buffer, uint32_t address, uint8_t *data, uint32_t *length, uint8_t mode);
    ISP_STATUS_T (*write)(ISP_MEMORY_T *mem, void *buffer, uint32_t address, const uint8_t *data, uint32_t length, uint8_t mode);
} ISP_MEM_FUNC_T;

/*! @brief ISP_MEM_INFO_T structure of memory characteristics  */
typedef struct __MEM_INFO_T
{
    uint32_t base_address;  /*!< base address of memory bank */
    uint32_t length;        /*!< total size */
    uint32_t block_size;    /*!< block size : flash page size */
    uint16_t flags;         /*!< unused */
    ISP_MEMORY_TYPE_E type; /*!< memory type : note that EFUSE bank is not a memory as such -
                             * SPIFI is unimplemented  */
    uint8_t access;        /*!< bitfield of access rights:
                            * - bit 0: Read access
                            * - bit 1: Write access
                            * - bit 2: Erase right
                            * - bit 3: Erase all right
                            * - bit 4: blank check right
                            A value of 0 denotes that access is closed */
    uint8_t auth_access;   /*!< similar to access for authenticated commands */
    ISP_MEM_FUNC_T *func;  /*!< set of function pointers of this memory type see @ ISP_MEM_FUNC_T*/
    const char *name;      /*!< name of memory bank */
} ISP_MEM_INFO_T;

typedef struct __MEM_T
{
    uint8_t index;
    uint8_t access;
    ISP_MEM_INFO_T info;
} ISP_MEMORY_T;

/*! @brief ISP_ENC_STATE_T ISP structure for ciphering options : TODO check poorly tested should we advertise this ?*/
typedef struct
{
    uint32_t mode;    /*!< 0: none - 1: AES CTR */
    uint32_t start;   /*!< start address of cipher/decipher operation */
    uint32_t end;     /*!< end address of cipher/decipher operation */
    uint32_t iv[4];  /*!< Initialization vector IV : 16 bytes */
    uint32_t key[8]; /*!< AES Key - key[4..7] unused */
} ISP_ENC_STATE_T;

/*! @brief ISP_EXTENSION_T ISP extension function pointer prototype */
typedef ISP_STATUS_T (*ISP_EXTENSION_T)(ISP_STATE_T *state,
                                        teFlashProgCommand request,
                                        uint8_t *in_data,
                                        uint16_t in_len,
                                        teFlashProgCommand *response,
                                        uint8_t *out_data,
                                        uint16_t *out_len);

typedef ISP_STATUS_T (*ISP_GET_MEMORY_T)(ISP_STATE_T *state, uint8_t index, ISP_MEM_INFO_T *memory);

/*! @brief ISP_STATE_T structure holding the context the the curent ISP command.
 *
 * Note: this context is held in RAM is the stack so is lost after ISP_Entry is exited.
 *
 */
typedef struct __STATE_T
{
    ISP_GET_MEMORY_T get_memory;     /*!< Function pointer to get_memory  */
    ISP_MEMORY_T mem;
    ISP_EXTENSION_T extension;      /*!< Function pointer to extension */
    uint32_t *buffer;               /*!< buffer holding command (in stack) */
    ISP_ENC_STATE_T enc_state;      /*!< Embedded ciphering structure see @ ISP_ENC_STATE_T*/
    IMAGE_CERT_T certificate;       /*!< Certificate used to authenticate ISP commands
                                     * it is composed of the custumer identifier and
                                     * the unlock public key found in PFLASH
                                     */
    uint8_t stored_hash[32];        /*!< SHA=256 hash storage */
    uint8_t mode;                   /*!< mode 0x00: inactive
                                     * - 0x01: Default ISP mode
                                     * - 0x7f: unlock mode
                                     * - 0x80 or higher: treated by extension function if any
                                     */
    uint8_t isp_level;             /*!< ISP level as restrained by EFUSE configuation and PFLASH parameter */
    uint16_t buffer_size;          /*!< size of buffer : normally 1024 */
    uint8_t unlock_disable;        /*!< unlock forbidden by EFUSE */
    uint8_t SWD_disable;           /*!< SWD Debug interface disabled */
    unsigned authenticated : 1;
} ISP_STATE_T;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/*!
 * @brief This function is invoked when ISP mode is requested.
 *
 * The ISP mode is requested when GPIO 5 is held down on rest or when no valid
 * image can be found in the in the internal flash.
 *
 * @param  isp_extension function pointer on extension function.
 *         ISP_INVALID_EXTENSION (0) : no extension requested is the only implemented choice
 * Note: ISP_Entry reads from vector table [13] in order to find a possible extension funcion.
 *       The boot ROM has a 0 value at that location.
 *
 *
 * @return  status 0: ISP entered successfully, otherwise error was detected (ISP disabled)
 *
 */
static inline int ISP_Entry(ISP_EXTENSION_T isp_extension)
{
    int (*p_ISP_Entry)(ISP_EXTENSION_T isp_extension);
    p_ISP_Entry = (int (*)(ISP_EXTENSION_T isp_extension))0x03002fb1U;

    return p_ISP_Entry(isp_extension);
}

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/



#if defined __cplusplus
}
#endif

#endif /* ROM_ISP_H_ */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
