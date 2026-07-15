/* Host-test stub for the ZigBee queue API used by the UART ISR.
 * NOT used by the firmware build. */
#ifndef HOST_STUB_ZQUEUE_H
#define HOST_STUB_ZQUEUE_H
#include "jendefs.h"

typedef struct { int dummy; } tszQueue;
extern tszQueue APP_msgSerialRx;

bool ZQ_bQueueSend(void *pQueue, const void *pvItemToQueue);

#endif
