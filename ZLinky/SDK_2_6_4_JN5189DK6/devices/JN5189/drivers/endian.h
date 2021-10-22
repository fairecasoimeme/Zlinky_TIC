/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ENDIAN_H_
#define ENDIAN_H_

#include <string.h>

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

#define htole32(A) (A)
#define htobe32(A) __builtin_bswap32(A)
#define le32toh(A) (A)
#define be32toh(A) __builtin_bswap32(A)

#define htole16(A) (A)
#define htobe16(A) __builtin_bswap16(A)
#define le16toh(A) (A)
#define be16toh(A) __builtin_bswap16(A)

#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

#define htole32(A) __builtin_bswap32(A)
#define htobe32(A) (A)
#define le32toh(A) __builtin_bswap32(A)
#define be32toh(A) (A)

#define htole16(A) __builtin_bswap16(A)
#define htobe16(A) (A)
#define le16toh(A) __builtin_bswap16(A)
#define be16toh(A) (A)

#else
#error Failed to detect byte order
#endif

static inline void htobe32b(uint32_t u32Host, uint8_t *pu8Store)
{
    uint32_t u32Be = htobe32(u32Host);
    memcpy(pu8Store, &u32Be, sizeof(uint32_t));
}

static inline void htole32b(uint32_t u32Host, uint8_t *pu8Store)
{
    uint32_t u32Le = htole32(u32Host);
    memcpy(pu8Store, &u32Le, sizeof(uint32_t));
}

static inline uint32_t be32btoh(uint8_t *pu8Store)
{
    uint32_t u32Be;
    memcpy(&u32Be, pu8Store, sizeof(uint32_t));
    return be32toh(u32Be);
}

static inline uint32_t le32btoh(uint8_t *pu8Store)
{
    uint32_t u32Le;
    memcpy(&u32Le, pu8Store, sizeof(uint32_t));
    return le32toh(u32Le);
}

static inline void htobe16b(uint16_t u16Host, uint8_t *pu8Store)
{
    uint16_t u16Be = htobe16(u16Host);
    memcpy(pu8Store, &u16Be, sizeof(uint16_t));
}

static inline void htole16b(uint16_t u16Host, uint8_t *pu8Store)
{
    uint16_t u16Le = htole16(u16Host);
    memcpy(pu8Store, &u16Le, sizeof(uint16_t));
}

static inline uint16_t be16btoh(uint8_t *pu8Store)
{
    uint16_t u16Be;
    memcpy(&u16Be, pu8Store, sizeof(uint16_t));
    return be16toh(u16Be);
}

static inline uint16_t le16btoh(uint8_t *pu8Store)
{
    uint16_t u16Le;
    memcpy(&u16Le, pu8Store, sizeof(uint16_t));
    return le16toh(u16Le);
}

#endif /* ENDIAN_H_ */
