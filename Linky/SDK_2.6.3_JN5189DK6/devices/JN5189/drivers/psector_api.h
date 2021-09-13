/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PSECTOR_API_H_
#define _PSECTOR_API_H_

#include "rom_psector.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#ifndef IMAGE_INVALID_ADDR
#define IMAGE_INVALID_ADDR ~0UL
#endif

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * API
 ******************************************************************************/

/*! *********************************************************************************
 * @brief      Write new BLE MAC address to PFLASH.
 *
 * @return -1 if failure. 0 if success
 */
int psector_WriteBleMacAddress(uint8_t *src_mac_address);

/*! *********************************************************************************
 * @brief      Reads PSECT PAGE0 contents to  RAM buffer and return pointer to that buffer if done.
 *
 * @return psector_page_data_t pointer, NULL if unsuccessful. Should be kept by user in order to
 * access the data faster.
 */
psector_page_data_t *psector_GetPage0Handle(void);

/*! *********************************************************************************
 * @brief      Computes the checksum of the PSECT to be updates and perform write to the update page.
 * @param[in] page : pointer in RAM on page to be written to update PSECT.
 * @param[in] id   : PAGE0(0) or PFLASH(1).
 *
 * @return -1 if Write to update page fails, 0 otherwise
 */
int psector_CommitPageUpdates(psector_page_data_t *page, psector_partition_id_t id);

/*! *********************************************************************************
 * @brief      Computes the checksum of the PSECT to be updates and perform write to the update page.
 * @param[in] ota_entry : ota_entry structure to update PSECT.
 * @param[in] commit   : set to TRUE if no other updates are to be added to the RAM cache.
 *                       if set write to UpdatePage is triggered to PSECT.
 *
 * @return -1 if Write to update page fails, 0 otherwise
 */
int psector_SetOtaEntry(image_directory_entry_t *ota_entry, bool commit);

/*! *********************************************************************************
 * @brief      Computes the checksum of the PSECT to be updates and perform write to the update page.
 * @param[in] page : pointer in RAM on page to be written to update PSECT.
 * @param[in] id   : PAGE0(0) or PFLASH(1).
 *
 * @return -1 if Write to update page fails, 0 otherwise
 */
int psector_CommitPageUpdates(psector_page_data_t *page, psector_partition_id_t id);

/*! *********************************************************************************
 * @brief        pfImgValidationCb callback function pointer for image validation
 *               function.
 * Note this callback has a void* argument that must match a structure coresponding
 * to the actual implementation.
 */
typedef uint32_t (*pfImgValidationCb)(uint32_t image_addr, void *imgValidationArgs);

/*! *********************************************************************************
 * @brief        Set index of application to load for SSBL.
 * @param[in] preferred_app : application to be started by SSBL
 * @param[in] commit        : if set updates to PSECT update page complete,
 *                            application will compute the Update page checksum and verifications
 * @param[in] imgValCb      : Image validation callback function may perform authentication or not
 *                            if NULL, this operation is skipped.
 *                           OTA_ImageValidate is an example in OTASupport.c
 * @param[in] imgValidationArgs: void * pointer used to pass arguments to imgValCb
 * @return           TRUE if success, FALSE otherwise.
 ********************************************************************************** */
bool psector_SetPreferredApp(uint8_t preferred_app,
                             bool commit_now,
                             pfImgValidationCb imgValCb,
                             void *imgValidationArgs);

/*!
 * @brief This function returns the ZB password found in PSECT
 *
 * @param  password:   pointer on 16 octet string storage in RAM to receive the password read.
 *
 * @return none
 */
void psector_GetPassword(uint8_t *password);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _BOARD_H_ */
