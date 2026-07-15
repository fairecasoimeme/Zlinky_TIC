/*
 * Host-side test for iFindReportSlot() in ZLinky/Source/app_reporting.c.
 *
 * vSaveReportableRecord() used to index asSavedReports[] with an int that was
 * left uninitialised when the table was full and no slot matched, producing an
 * out-of-bounds write that was then persisted to flash. The search is now a
 * helper that returns -1 in that case, and the caller drops the record.
 *
 * The runner extracts the REAL helper and compiles it against a minimal struct
 * carrying only the two fields the helper reads.
 *
 * Build & run:  sh tests/host/run_report_test.sh
 * NOT part of the firmware build.
 */
#include <stdio.h>
#include "report_types.h"

int iFindReportSlot(uint16 u16ClusterID, uint16 u16AttributeEnum);

/* The table the helper searches (declared extern in report_types.h). */
tsReports_min asSavedReports[ZCL_NUMBER_OF_REPORTS];

static int fails = 0;
#define CHECK(c, m) do { \
    if (!(c)) { printf("  FAIL: %s\n", (m)); fails++; } \
    else      { printf("  ok  : %s\n", (m)); } \
} while (0)

static void clear_table(void)
{
    for (int i = 0; i < ZCL_NUMBER_OF_REPORTS; i++) {
        asSavedReports[i].u16ClusterID = 0;
        asSavedReports[i].sAttributeReportingConfigurationRecord.u16AttributeEnum = 0;
    }
}

static void set_slot(int i, uint16 cluster, uint16 attr)
{
    asSavedReports[i].u16ClusterID = cluster;
    asSavedReports[i].sAttributeReportingConfigurationRecord.u16AttributeEnum = attr;
}

int main(void)
{
    printf("[report-slot] empty / partial table\n");
    clear_table();
    CHECK(iFindReportSlot(0x0702, 0x0000) == 0, "empty table -> first free slot 0");

    clear_table();
    set_slot(0, 0x0702, 0x0000);
    CHECK(iFindReportSlot(0x0702, 0x0001) == 1, "distinct attribute -> next free slot 1");
    CHECK(iFindReportSlot(0x0702, 0x0000) == 0, "existing (cluster,attr) -> its slot 0");

    printf("[report-slot] full table\n");
    clear_table();
    for (int i = 0; i < ZCL_NUMBER_OF_REPORTS; i++)
        set_slot(i, 0x0702, (uint16)(i + 1));   /* every slot used, attrs 1..N */
    CHECK(iFindReportSlot(0x0702, 0xABCD) == -1,
          "full table, no match -> -1 (guard prevents the out-of-bounds write)");
    CHECK(iFindReportSlot(0x0702, 5) == 4, "full table, matching attribute -> its slot");

    if (fails) { printf("\n%d CHECK failure(s)\n", fails); return 1; }
    printf("\nAll report-slot checks passed.\n");
    return 0;
}
