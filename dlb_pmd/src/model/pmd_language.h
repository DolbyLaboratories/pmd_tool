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

#ifndef PMD_LANGUAGE_H_
#define PMD_LANGUAGE_H_

/**
 * @file pmd_language.h
 *
 * routines for checking validity of ISO 639-1 or ISO 639-2 (B and T)
 * 2- and 3- letter country codes.
 */ 

#include <stdint.h>
#include "pmd_types.h"
#include "dlb_pmd_api.h"

/**
 * @brief language code, ISO 639-1 or ISO 639-2
 *
 * internally, 2-char language codes are stored as if they
 * were 3-char string arrays, and 3-char language codes as if
 * they were 4-char string arrays.  The entire array is
 * represented as a 4-byte integer.
 */
typedef uint32_t pmd_langcode;


/**
 * @brief convert an ascii string to an ISO 639-1 or ISO 639-2 code
 */
dlb_pmd_success           /** @return 0 if legal code, 1 otherwise */
pmd_decode_langcode
    (const char *str      /**< [in] string to parse */
    ,pmd_langcode *code   /**< [out] encoded string */
    );


/**
 * @brief convert internal langcode to ASCII string
 */
void
pmd_langcode_string
    (pmd_langcode code   /**< [in] language code */
    ,char (*outptr)[4]   /**< [out] character string, must be > 3 chars long */
    );


/**
 * @brief return the total number of languages
 */
unsigned int            /** @return total number of valid language codes */
pmd_langcode_count
    (void
    );


/**
 * @brief assuming all valid language codes are ordered alphabetically,
 * (ISO-639-1, ISO-639-2b/t), return the n'th language code in that order.
 */
dlb_pmd_success             /** @return 0 if successful, 1 otherwise */
pmd_langcode_select
    (unsigned int number    /**< [in] number of language in alphabetic order */
    ,char (*outptr)[4]      /**< [in] location of array to populate */
    );


#endif /* PMD_LANGUAGE_H_ */
 
