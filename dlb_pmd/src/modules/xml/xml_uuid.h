/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2017-2020 by Dolby Laboratories,
 *                Copyright (C) 2017-2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef PMD_XML_UUID_H_
#define PMD_XML_UUID_H_

#include "xml_hex.h"

/**
 * @file xml_uuid.h
 * @brief helper function to write/parse UUIDs
 *
 * A UUID is 32-hex digits written in the following format:
 *
 *  xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx (8-4-4-4-12)
 */


/**
 * @brief write UUID to a string
 */
static inline
void
write_uuid
    (const uint8_t *uuid  /**< UUID byte array */
    ,char *out            /**< memory to write string, must have at least 36 chars */
    )
{
    write_hex(uuid, 4, out);
    uuid += 4;
    out += 8;
    *out++= '-';

    write_hex(uuid, 2, out);
    uuid += 2;
    out += 4;
    *out++ = '-';

    write_hex(uuid, 2, out);
    uuid += 2;
    out += 4;
    *out++ = '-';

    write_hex(uuid, 2, out);
    uuid += 2;
    out += 4;
    *out++ = '-';

    write_hex(uuid, 6, out);
}


/**
 * @brief parse a UUID
 */
static inline
int                    /** @return 0 on failure, 1 on success */
read_uuid
    (const char *in    /**< [in] data to parse */
    ,uint8_t *uuid     /**< [in] uuid field to fill, at least 16 bytes long */
    )
{
    if (read_hex(in, 8, uuid)) return 0;
    in += 8;
    uuid += 4;
    if (*in != '-') return 0;
    ++in;

    if (read_hex(in, 4, uuid)) return 0;
    in += 4;
    uuid += 2;
    if (*in != '-') return 0;
    ++in;

    if (read_hex(in, 4, uuid)) return 0;
    in += 4;
    uuid += 2;
    if (*in != '-') return 0;
    ++in;

    if (read_hex(in, 4, uuid)) return 0;
    in += 4;
    uuid += 2;
    if (*in != '-') return 0;
    ++in;

    if (read_hex(in, 12, uuid)) return 0;
    return 1;
}


#endif /* PMD_XML_UUID_H_ */
