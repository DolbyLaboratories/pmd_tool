/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020 - 2022, Dolby Laboratories Inc.
 * Copyright (c) 2022, Dolby International AB.
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

/*
 * @file xml_eidr.h
 * @brief helper function to write/parse EIDRs
 *
 * EIDR format
 *
 * [10.]5240[/]XXXX[-]XXXX[-]XXXX[-]XXXX[-]XXXX[-][C]
 *
 * X is hex digit
 *
 * C is ISO 7064 mod 37,36 check character, computed over the 20 hex digits
 *
 * IAT uses 96-bit 'compact binary' format where
 *
 * - 5240 encoded as 16-bit int
 * - 80 bits for 20 hex digits
 */


#ifndef PMD_XML_EIDR_H_
#define PMD_XML_EIDR_H_

#include "xml_hex.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define EIDR_SUB_PREFIX (5240)


/**
 * @brief write EIDR to a string
 */
static inline
void
write_eidr
    (const uint8_t *eidr  /**< EIDR byte array */
    ,char *out            /**< memory to write string, must have at least 36 chars */
    )
{
    uint16_t subprefix = *eidr++;
    subprefix = (subprefix << 8) | (*eidr++);

    out += sprintf(out, "%u/", subprefix);

    write_hex(eidr, 2, out);
    eidr += 2;
    out += 4;
    *out++= '-';

    write_hex(eidr, 2, out);
    eidr += 2;
    out += 4;
    *out++= '-';

    write_hex(eidr, 2, out);
    eidr += 2;
    out += 4;
    *out++= '-';

    write_hex(eidr, 2, out);
    eidr += 2;
    out += 4;
    *out++= '-';

    write_hex(eidr, 2, out);
    eidr += 2;
    out += 4;
    *out++= '-';

    /* how to compute modulus over 80 digit number? */
}


/**
 * @brief parse an eidr
 */
static inline
int                      /** @return 0 on failure, 1 on success */
read_eidr
    (const char *in      /**< [in] data to parse */
    ,uint8_t *eidr       /**< [in] eidr field to fill, at least 12 bytes long */
    )
{
    unsigned int checksum;
    char *endp;
    long tmp;

    if (!strncmp(in, "10.", 3))
    {
       /* ignore optional prefix */
       in += 3;
    }

    tmp = strtol(in, &endp, 10);
    if (tmp != EIDR_SUB_PREFIX || endp == in)
    {
        return 0;
    }
    *eidr++ = (EIDR_SUB_PREFIX >> 8) & 0xff;
    *eidr++ = (EIDR_SUB_PREFIX & 0xff);

    in = endp;
    if (*in == '/') ++in;  /* skip optional / */

    if (read_hex(in, 4, eidr)) return 0;
    in += 4;
    eidr += 2;
    if (*in == '-') ++in;

    if (read_hex(in, 4, eidr)) return 0;
    in += 4;
    eidr += 2;
    if (*in == '-') ++in;

    if (read_hex(in, 4, eidr)) return 0;
    in += 4;
    eidr += 2;
    if (*in == '-') ++in;

    if (read_hex(in, 4, eidr)) return 0;
    in += 4;
    eidr += 2;
    if (*in == '-') ++in;

    if (read_hex(in, 4, eidr)) return 0;
    in += 4;
    eidr += 2;
    if (*in == '-') ++in;

    if (1 == sscanf(in, "%u", &checksum))
    {
        /* todo: compute checksum */
        ;
    }

    return 1;
}


#endif /* PMD_XML_EIDR_H_ */

