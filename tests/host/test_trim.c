/*
 * Host-side regression test for trim() in ZLinky/Source/App_Linky.c.
 *
 * trim() is applied to values parsed from the (untrusted) TIC stream. On an
 * empty or all-whitespace value the original code computed strlen(s)-1 == -1
 * and read/wrote out of bounds. The runner extracts the REAL trim() from
 * App_Linky.c at build time and exercises it here, with the strings on the
 * heap so AddressSanitizer traps any out-of-bounds access.
 *
 * Build & run:  sh tests/host/run_trim_test.sh
 * NOT part of the firmware build.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Real function, compiled from a snippet extracted out of App_Linky.c. */
char *trim(char *s);

static int fails = 0;
#define CHECK(c, m) do { \
    if (!(c)) { printf("  FAIL: %s\n", (m)); fails++; } \
    else      { printf("  ok  : %s\n", (m)); } \
} while (0)

/* Heap copy so ASan surrounds it with red zones. */
static char *dup_heap(const char *s)
{
    size_t n = strlen(s) + 1;
    char *p = malloc(n);
    memcpy(p, s, n);
    return p;
}

int main(void)
{
    char *a = dup_heap("");
    printf("[trim] empty string (must not read before the buffer)\n");
    CHECK(strcmp(trim(a), "") == 0, "trim(\"\") == \"\"");
    free(a);

    char *b = dup_heap("   ");
    printf("[trim] all-whitespace (must not walk before the buffer)\n");
    CHECK(strcmp(trim(b), "") == 0, "trim(\"   \") == \"\"");
    free(b);

    char *c = dup_heap("  hello  ");
    printf("[trim] surrounded by spaces (no regression)\n");
    CHECK(strcmp(trim(c), "hello") == 0, "trim(\"  hello  \") == \"hello\"");
    free(c);

    char *d = dup_heap("world");
    printf("[trim] no spaces (no regression)\n");
    CHECK(strcmp(trim(d), "world") == 0, "trim(\"world\") == \"world\"");
    free(d);

    if (fails) { printf("\n%d CHECK failure(s)\n", fails); return 1; }
    printf("\nAll trim checks passed.\n");
    return 0;
}
