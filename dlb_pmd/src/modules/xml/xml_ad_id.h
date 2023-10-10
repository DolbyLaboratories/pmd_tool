/************************************************************************
 * dlb_pmd
 * Copyright (c) 2023, Dolby Laboratories Inc.
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

/**
 * @file xml_ad+id.h
 * @brief helper functions to write/read Ad-ID codes
 */

#ifndef PMD_XML_AD_ID_H_
#define PMD_XML_AD_ID_H_

#include <string.h>

/**
 * Ad-ID codes
 *
 * All AD-ID codes are 11 characters except HD or 3D codes which have an H or D in 12th Character
 *
 * - 4-character prefix identifying company
 * - 7-character code, can by any combination of letters or numbers
 */

/**
 * @brief write Ad-ID to a string
 */
static inline
void
write_ad_id
    (const uint8_t *adid     /**< Ad-ID byte array */
    ,char *out               /**< memory to write string, must have at least 11 chars */
    )
{
    strcpy(out, (char*)adid);
}


/**
 * @brief read Ad-ID to a string
 */
static inline
int                      /** @return 0 on failure, 1 on success */
read_ad_id
    (const char *in      /**< [in] data to parse */
    ,uint8_t *adid       /**< [in] Ad-ID field to fill, at least 11 bytes long */
    )
{
    unsigned int i;
    for (i = 0; i != 11; ++i)
    {
        char c = in[i];
    if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')))
    {
        return 0;
    }
    }
    memcpy(adid, in, 11);
    return 1;
}


#endif /* PMD_XML_AD_ID_H_ */
