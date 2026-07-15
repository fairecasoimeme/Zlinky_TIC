/* Host-test stub for NXP jendefs.h — provides just the base types/macros
 * used by app_uartlinky.c so it can be compiled and exercised on a PC.
 * NOT used by the firmware build. */
#ifndef HOST_STUB_JENDEFS_H
#define HOST_STUB_JENDEFS_H

#include <stdint.h>

typedef uint8_t   uint8;
typedef uint16_t  uint16;
typedef uint32_t  uint32;
typedef uint64_t  uint64;
typedef int8_t    int8;
typedef int16_t   int16;
typedef int32_t   int32;

typedef uint8_t   bool_t;
typedef uint8_t   bool;

#ifndef true
#define true  (1)
#endif
#ifndef false
#define false (0)
#endif

#ifndef PUBLIC
#define PUBLIC
#endif
#ifndef PRIVATE
#define PRIVATE static
#endif

#ifndef TRUE
#define TRUE  (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

#endif /* HOST_STUB_JENDEFS_H */
