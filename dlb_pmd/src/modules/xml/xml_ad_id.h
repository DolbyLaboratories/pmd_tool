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
