/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020, Dolby Laboratories Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 **********************************************************************/

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
