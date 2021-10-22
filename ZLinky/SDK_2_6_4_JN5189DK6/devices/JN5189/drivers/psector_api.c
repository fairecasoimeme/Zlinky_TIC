/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "fsl_debug_console.h"
#include "FunctionLib.h"
#include "rom_psector.h"
#include "psector_api.h"

#define BLE_MACID_SZ 6
#define MANUFACTURER_BLE_MACID_ADRESS (const uint8_t *)(0x9fc00 + 0x100)
#define gBD_ADDR_NXP_OUI_c 0x00, 0x60, 0x37

static psector_page_data_t *mPage0Hdl = NULL;
#if PRELOAD_PFLASH
static psector_page_data_t *mPFlashHdl = NULL;
#endif
psector_page_data_t *psector_GetPage0Contents(void)
{
    static psector_page_data_t mPage0;
    psector_page_data_t *page = &mPage0;

    psector_page_state_t page_state;
    do
    {
        page_state = psector_ReadData(PSECTOR_PAGE0_PART, 0, 0, sizeof(psector_page_t), page);
        if (page_state < PAGE_STATE_DEGRADED)
        {
            page = NULL;
            break;
        }
    } while (0);

    return page;
}

#if PRELOAD_PFLASH
psector_page_data_t *psector_GetPFlashContents(void)
{
    static psector_page_data_t mpFlash;
    psector_page_data_t *page = &mpFlash;

    psector_page_state_t page_state;
    do
    {
        page_state = psector_ReadData(PSECTOR_PFLASH_PART, 0, 0, sizeof(psector_page_t), page);
        if (page_state < PAGE_STATE_DEGRADED)
        {
            page = NULL;
            break;
        }
    } while (0);

    return page;
}
#endif

psector_page_data_t *psector_GetPage0Handle(void)
{
    psector_page_data_t *hdl = NULL;
    ;
    do
    {
        if (mPage0Hdl != NULL)
        {
            hdl = mPage0Hdl;
            break;
        }

        if ((mPage0Hdl = psector_GetPage0Contents()) != NULL)
        {
            hdl = mPage0Hdl;
            break;
        }
    } while (0);
    return hdl;
}

int psector_CommitPageUpdates(psector_page_data_t *page, psector_partition_id_t id)
{
    int status = -1;
    do
    {
        if (!page)
            break;

        page->hdr.version++;
        page->hdr.checksum = psector_CalculateChecksum((psector_page_t *)page);

        if (psector_WriteUpdatePage(id, (psector_page_t *)page) != WRITE_OK)
        {
            break;
        }
        status = 0;
    } while (0);

    return status;
}

int psector_SetOtaEntry(image_directory_entry_t *ota_entry, bool commit)
{
    int res = -1;

    do
    {
        mPage0Hdl = psector_GetPage0Handle();
        if (mPage0Hdl == NULL)
            break;

        mPage0Hdl->page0_v3.ota_entry.img_base_addr = ota_entry->img_base_addr;
        mPage0Hdl->page0_v3.ota_entry.img_nb_pages  = ota_entry->img_nb_pages;
        mPage0Hdl->page0_v3.ota_entry.flags         = ota_entry->flags;
        mPage0Hdl->page0_v3.ota_entry.img_type      = ota_entry->img_type;

        if (commit)
        {
            if (psector_CommitPageUpdates(mPage0Hdl, PSECTOR_PAGE0_PART) < 0)
                break;
        }
        res = 0;

    } while (0);
    return res;
}

bool psector_SetPreferredApp(uint8_t preferred_app,
                             bool commit_now,
                             pfImgValidationCb imgValCb,
                             void *imgValidationArgs)
{
    bool res = false;
    do
    {
        uint32_t image_addr = IMAGE_INVALID_ADDR;

        mPage0Hdl = psector_GetPage0Handle();
        if (mPage0Hdl == NULL)
        {
            res = false;
            break;
        }

        if (preferred_app == mPage0Hdl->page0_v3.preferred_app_index)
        {
            res = false; /* do nothing : already set so do not reset */
            break;
        }
        /* Try to find an entry in the img directory */
        image_directory_entry_t *dir_entry          = NULL;
        image_directory_entry_t *dir_entry_iterator = NULL;
        for (int i = 0; i < IMG_DIRECTORY_MAX_SIZE; i++)
        {
            dir_entry_iterator = mPage0Hdl->page0_v3.img_directory + i;
            if (dir_entry_iterator->img_type == preferred_app)
            {
                dir_entry = dir_entry_iterator;
                break;
            }
        }
        if (dir_entry && (dir_entry->flags & IMG_FLAG_BOOTABLE))
        {
            /* Do a quick image validation : pass no certificate pointer */
            if (imgValCb != NULL)
                image_addr = (*imgValCb)(dir_entry->img_base_addr, imgValidationArgs);
        }
        if (image_addr == IMAGE_INVALID_ADDR)
            break;

        mPage0Hdl->page0_v3.preferred_app_index = preferred_app;
        if (commit_now)
        {
            if (psector_CommitPageUpdates(mPage0Hdl, PSECTOR_PAGE0_PART) == 0)
                res = true;
        }
        res = true;
    } while (0);
    return res;
}
