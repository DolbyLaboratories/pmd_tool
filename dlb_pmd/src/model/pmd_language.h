/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2017-2019 by Dolby Laboratories,
 *                Copyright (C) 2017-2019 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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
 
