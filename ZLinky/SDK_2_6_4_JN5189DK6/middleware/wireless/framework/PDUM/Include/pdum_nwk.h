/*****************************************************************************
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2008, 2020 NXP
 * All rights reserved.
 *
 * MODULE:             PDU Manager
 *
 * COMPONENT:          pdum_nwk.h
 *
 * AUTHOR:             MRW
 *
 * DESCRIPTION:        PDU manager interface for network protocol stacks
 *                     Manages NPDUs
 *
 *
 *
 ****************************************************************************/

#ifndef PDUM_NWK_H_
#define PDUM_NWK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <jendefs.h>
#include <pdum_common.h>

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#ifndef PDUM_INLINE
#define PDUM_INLINE INLINE
#endif

#ifndef PDUM_ALWAYS_INLINE
#define PDUM_ALWAYS_INLINE ALWAYS_INLINE
#endif

#if JENNIC_CHIP_FAMILY == JN514x
// Keep PDUM size for Z01 to avoid patching
#define PDUM_NPDU_SIZE              (127)
#else
//  RAM saving lpsw2999: NPDU size can be shrunk
#define PDUM_NPDU_SIZE              (127-9)
#endif
#define PDUM_NPDU_DESCENDING(npdu)  (((pdum_tsNPdu *)(npdu))->u8Footer < ((pdum_tsNPdu *)(npdu))->u8Header)
#define PDUM_NPDU_ASCENDING(npdu)   (((pdum_tsNPdu *)(npdu))->u8Footer >= ((pdum_tsNPdu *)(npdu))->u8Header)

#define TRACE_NPDU_MAX TRUE
#define PDUM_NPDU_FREETAG (0xFF)

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/* NPDU handle */
/* internal container for an NPDU */
/* [I SP001259_sfr 3] begin */
typedef struct pdum_tsNPdu_tag {
    union {
        struct pdum_tsNPdu_tag *psNext;
        struct {
            uint8 u8Footer;
            uint8 u8ClaimedFtr;
            uint8 u8Header;
            union { /* same place for descending */
                uint8 u8Data;
                uint8 u8ClaimedHdr;
            };
        };
    };
#ifdef PDUM_PEDANTIC_CHECKS
    uint8 au8Magic1[4];
#endif
    uint8 au8PayloadStorage[PDUM_NPDU_SIZE]; /* [I SP001259_sfr 6] */
#ifdef PDUM_PEDANTIC_CHECKS
    uint8 au8Magic2[4];
#endif
    uint8 u8Tag;
} pdum_tsNPdu;
/* [I SP001259_sfr 3] end */

/* NPDU handle */
typedef pdum_tsNPdu *PDUM_thNPdu;

/* [I SP001259_sfr 159] begin */
typedef enum {
    PDUM_E_DESCENDING  = 0,
    PDUM_E_ASCENDING = 1
} PDUM_teDirection;
/* [I SP001259_sfr 159] end */

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC PDUM_teStatus PDUM_eNPduAllocate(PDUM_thNPdu *phNPdu); /* [I SP001259_sfr 102] */
PUBLIC PDUM_teStatus PDUM_eNPduAllocateFromISR (PDUM_thNPdu *phNPdu);
PUBLIC PDUM_teStatus PDUM_eNPduClone(PDUM_thNPdu *phNPdu, PDUM_thNPdu hNPdu); /* [I SP001259_sfr 102] */
PUBLIC PDUM_teStatus PDUM_eNPduInitDescending(PDUM_thNPdu phNPdu); /* [I SP001259_sfr 108] */
PUBLIC PDUM_teStatus PDUM_eNPduInitAscending(PDUM_thNPdu phNPdu, uint8 u8PayloadSize); /* [I SP001259_sfr 114] */
PUBLIC PDUM_teStatus PDUM_eNPduFree(PDUM_thNPdu hNPdu); /* [I SP001259_sfr 119] */
PUBLIC PDUM_teStatus PDUM_eNPduFreeFromISR(PDUM_thNPdu hNPdu);
PUBLIC PDUM_teStatus PDUM_eNPduClaimHeader(PDUM_thNPdu hNPdu, uint8 u8HeaderSize); /* [I SP001259_sfr 125] */
PUBLIC PDUM_teStatus PDUM_eNPduClaimFooter(PDUM_thNPdu hNPdu, uint8 u8FooterSize); /* [I SP001259_sfr 133] */
PUBLIC PDUM_teStatus PDUM_eNPduClaimData(PDUM_thNPdu hNPdu, uint8 u8Size);
PUBLIC PDUM_teStatus PDUM_eNPduAscend(PDUM_thNPdu hNPdu); /* [I SP001259_sfr 144] */
PUBLIC PDUM_teStatus PDUM_eNPduDescend(PDUM_thNPdu hNPdu); /* [I SP001259_sfr 149] */
PUBLIC PDUM_teStatus PDUM_eNPduPrependHeaderNBO(PDUM_thNPdu hNPdu, const char *szFormat, ...);
PUBLIC PDUM_teStatus PDUM_eNPduAppendFooterNBO(PDUM_thNPdu hNPdu, const char *szFormat, ...);
PUBLIC PDUM_teStatus PDUM_eNPduAppendDataNBO(PDUM_thNPdu hNPdu, const char *szFormat, ...);
PUBLIC PDUM_teStatus PDUM_eNPduReadHeaderNBO(void *pvStruct, const char *szFormat, PDUM_thNPdu hNPdu);
PUBLIC PDUM_teStatus PDUM_eNPduReadFooterNBO(void *pvStruct, const char *szFormat, PDUM_thNPdu hNPdu);
PUBLIC PDUM_teStatus PDUM_eNPduWriteToBuffer(PDUM_thNPdu hNPdu, uint8 *pu8Buffer);
PUBLIC PDUM_teStatus PDUM_eNPduWriteFromBuffer(PDUM_thNPdu hNPdu, uint8 *pu8Buffer);
PUBLIC PDUM_teStatus PDUM_eNPduWriteDataToBuffer(PDUM_thNPdu hNPdu, uint8 *pu8Buffer);
PUBLIC PDUM_teStatus PDUM_eNPduWriteDataFromBuffer(PDUM_thNPdu hNPdu, uint8 *pu8Buffer);

#ifdef DBG_ENABLE
PUBLIC void PDUM_vDBGPrintNPdu(PDUM_thNPdu hNPdu);
#else
#define PDUM_vDBGPrintNPdu(npdu)
#endif

#ifdef PDUM_PEDANTIC_CHECKS
PUBLIC void vCheckNPduPoolValid(void);
#endif

#if TRACE_NPDU_MAX
PUBLIC uint8 PDUM_u8GetMaxNpduUse(void);
PUBLIC uint8 PDUM_u8GetNpduUse(void);
PUBLIC uint8 PDUM_u8GetNpduPool(void);
#endif

/****************************************************************************/
/***        Inlined Functions                                             ***/
/****************************************************************************/

/* [I SP001259_sfr 155] begin */
PDUM_INLINE void *PDUM_pvNPduGetHeader(PDUM_thNPdu hNPdu) PDUM_ALWAYS_INLINE;
PDUM_INLINE void *PDUM_pvNPduGetHeader(PDUM_thNPdu hNPdu)
{
#ifdef PDUM_PEDANTIC_CHECKS
    vCheckNPduPoolValid();
#endif
    if (PDUM_NPDU_ASCENDING(hNPdu))
    {
        return &((pdum_tsNPdu *)hNPdu)->au8PayloadStorage[((pdum_tsNPdu *)hNPdu)->u8Header];
    }
    return &((pdum_tsNPdu *)hNPdu)->au8PayloadStorage[((pdum_tsNPdu *)hNPdu)->u8ClaimedHdr];
}
/* [I SP001259_sfr 155] end */

/* [I SP001259_sfr 130] begin */
PDUM_INLINE uint8 PDUM_u8NPduGetHeaderSize(PDUM_thNPdu hNPdu) PDUM_ALWAYS_INLINE;
PDUM_INLINE uint8 PDUM_u8NPduGetHeaderSize(PDUM_thNPdu hNPdu)
{
#ifdef PDUM_PEDANTIC_CHECKS
    vCheckNPduPoolValid();
#endif
    if (PDUM_NPDU_ASCENDING(hNPdu))
    {
        return ((pdum_tsNPdu *)hNPdu)->u8Data - ((pdum_tsNPdu *)hNPdu)->u8Header;
    }
    return ((pdum_tsNPdu *)hNPdu)->u8Header - ((pdum_tsNPdu *)hNPdu)->u8ClaimedHdr;
}
/* [I SP001259_sfr 130] end */

/* [I SP001259_sfr ???] begin */
PDUM_INLINE void *PDUM_pvNPduGetData(PDUM_thNPdu hNPdu) PDUM_ALWAYS_INLINE;
PDUM_INLINE void *PDUM_pvNPduGetData(PDUM_thNPdu hNPdu)
{
#ifdef PDUM_PEDANTIC_CHECKS
    vCheckNPduPoolValid();
#endif
    if (PDUM_NPDU_ASCENDING(hNPdu))
    {
        return &((pdum_tsNPdu *)hNPdu)->au8PayloadStorage[((pdum_tsNPdu *)hNPdu)->u8ClaimedHdr];
    }
    return NULL;
}
/* [I SP001259_sfr ???] end */

/* [I SP001259_sfr 156] begin */
PDUM_INLINE void *PDUM_pvNPduGetFooter(PDUM_thNPdu hNPdu) PDUM_ALWAYS_INLINE;
PDUM_INLINE void *PDUM_pvNPduGetFooter(PDUM_thNPdu hNPdu)
{
#ifdef PDUM_PEDANTIC_CHECKS
    vCheckNPduPoolValid();
#endif
    if (PDUM_NPDU_ASCENDING(hNPdu))
    {
        return &((pdum_tsNPdu *)hNPdu)->au8PayloadStorage[((pdum_tsNPdu *)hNPdu)->u8ClaimedFtr];
    }
    return &((pdum_tsNPdu *)hNPdu)->au8PayloadStorage[((pdum_tsNPdu *)hNPdu)->u8Footer];
}
/* [I SP001259_sfr 156] end */

/* [I SP001259_sfr 138] begin */
PDUM_INLINE uint8 PDUM_u8NPduGetFooterSize(PDUM_thNPdu hNPdu) PDUM_ALWAYS_INLINE;
PDUM_INLINE uint8 PDUM_u8NPduGetFooterSize(PDUM_thNPdu hNPdu)
{
#ifdef PDUM_PEDANTIC_CHECKS
    vCheckNPduPoolValid();
#endif
    if (PDUM_NPDU_ASCENDING(hNPdu))
    {
        return ((pdum_tsNPdu *)hNPdu)->u8Footer - ((pdum_tsNPdu *)hNPdu)->u8ClaimedFtr;
    }
    return ((pdum_tsNPdu *)hNPdu)->u8ClaimedFtr - ((pdum_tsNPdu *)hNPdu)->u8Footer;
}
/* [I SP001259_sfr 138] end */

#if 0
// not sure we need these?
PDUM_INLINE uint8 PDUM_u8NPduReadDataByte(PDUM_thNPdu hNPdu, uint8 u8Offset) PDUM_ALWAYS_INLINE;
PDUM_INLINE uint8 PDUM_u8NPduReadDataByte(PDUM_thNPdu hNPdu, uint8 u8Offset)
{
    if (PDUM_NPDU_ASCENDING(hNPdu))
    {
        return ((pdum_tsNPdu *)hNPdu)->au8PayloadStorage[((pdum_tsNPdu *)hNPdu)->u8ClaimedHdr + u8Offset];
    }
    return 0;
    return ((pdum_tsNPdu *)hNPdu)->au8PayloadStorage[
        ((pdum_tsNPdu *)hNPdu)->u8Header + u8Offset > PDUM_NPDU_SIZE ?
        ((pdum_tsNPdu *)hNPdu)->u8Header + u8Offset - PDUM_NPDU_SIZE :
        ((pdum_tsNPdu *)hNPdu)->u8Header + u8Offset];
}

PDUM_INLINE void PDUM_vNPduWriteDataByte(PDUM_thNPdu hNPdu, uint8 u8Offset, uint8 u8Value) PDUM_ALWAYS_INLINE;
PDUM_INLINE void PDUM_vNPduWriteDataByte(PDUM_thNPdu hNPdu, uint8 u8Offset, uint8 u8Value)
{
    if (PDUM_NPDU_ASCENDING(hNPdu))
    {
        ((pdum_tsNPdu *)hNPdu)->au8PayloadStorage[((pdum_tsNPdu *)hNPdu)->u8ClaimedHdr + u8Offset] = u8Value;
    }
    else
    {
        ((pdum_tsNPdu *)hNPdu)->au8PayloadStorage[
            ((pdum_tsNPdu *)hNPdu)->u8Header + u8Offset > PDUM_NPDU_SIZE ?
            ((pdum_tsNPdu *)hNPdu)->u8Header + u8Offset - PDUM_NPDU_SIZE :
            ((pdum_tsNPdu *)hNPdu)->u8Header + u8Offset] = u8Value;
    }
}
#endif

/* [I SP001259_sfr 141] begin */
PDUM_INLINE uint8 PDUM_u8NPduGetDataSize(PDUM_thNPdu hNPdu) PDUM_ALWAYS_INLINE;
PDUM_INLINE uint8 PDUM_u8NPduGetDataSize(PDUM_thNPdu hNPdu)
{
#ifdef PDUM_PEDANTIC_CHECKS
    vCheckNPduPoolValid();
#endif
    if (PDUM_NPDU_ASCENDING(hNPdu))
    {
        return ((pdum_tsNPdu *)hNPdu)->u8ClaimedFtr - ((pdum_tsNPdu *)hNPdu)->u8Data;
    }
    else
    {
        return ((pdum_tsNPdu *)hNPdu)->u8Footer + (PDUM_NPDU_SIZE - ((pdum_tsNPdu *)hNPdu)->u8Header);
    }
}
/* [I SP001259_sfr 141] end */

/* [I SP001259_sfr 158] begin */
PDUM_INLINE PDUM_teDirection PDUM_eNPduGetDirection(PDUM_thNPdu hNPdu) PDUM_ALWAYS_INLINE;
PDUM_INLINE PDUM_teDirection PDUM_eNPduGetDirection(PDUM_thNPdu hNPdu)
{
#ifdef PDUM_PEDANTIC_CHECKS
    vCheckNPduPoolValid();
#endif
    return PDUM_NPDU_ASCENDING(hNPdu) ? PDUM_E_ASCENDING : PDUM_E_DESCENDING; /* [I SP001259_sfr 159] */
}
/* [I SP001259_sfr 158] end */

/* [I SP001259_sfr ???] begin */
PDUM_INLINE bool_t PDUM_bNPduNoHdrOrFtr(PDUM_thNPdu hNPdu) PDUM_ALWAYS_INLINE;
PDUM_INLINE bool_t PDUM_bNPduNoHdrOrFtr(PDUM_thNPdu hNPdu)
{
#ifdef PDUM_PEDANTIC_CHECKS
    vCheckNPduPoolValid();
#endif
    return ((((pdum_tsNPdu *)hNPdu)->u8Footer == ((pdum_tsNPdu *)hNPdu)->u8ClaimedFtr) &&
            (((pdum_tsNPdu *)hNPdu)->u8Header == ((pdum_tsNPdu *)hNPdu)->u8ClaimedHdr));
}
/* [I SP001259_sfr ???] end */

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

#ifdef __cplusplus
};
#endif

#endif /* PDUM_NWK_H_ */
