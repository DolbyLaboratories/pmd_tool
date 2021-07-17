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

#ifndef PMD_SMPTE_2109_H
#define PMD_SMPTE_2109_H

/**
 * @file pmd_smpte_2109.h
 * @brief SMPTE 2109 specific information
 */

#include <stdint.h>
#include <string.h>


/**
 * @def PMD_MAX_DYNAMIC_TAGS
 * @brief max number of dynamic tags supported
 */
#define PMD_MAX_DYNAMIC_TAGS (32)


/**
 * @brief indicate a local tag remapping
 */
typedef struct
{
    uint16_t local_tag;               /**< local tag to be remapped */
    uint8_t  universal_label[16];     /**< 16-byte universal label to which we remap */
} pmd_dynamic_tag;


/**
 * @brief SMPTE-2109 related information
 */
typedef struct
{
    /* SMPTE 2109 container config */
    uint16_t sample_offset;          /**< global 'starting sample' */
    unsigned int num_dynamic_tags;   /**< number of local-tag reassignments */
    pmd_dynamic_tag dynamic_tags[PMD_MAX_DYNAMIC_TAGS];  /**< local-tag reassignments */

} pmd_smpte2109;

    
/**
 * @brief helper function to initialize SMPTE 2109 specific information
 */
static inline
void
pmd_smpte2109_init
    (pmd_smpte2109 *smpte2109
    )
{
    smpte2109->sample_offset = 0;
    smpte2109->num_dynamic_tags = 0;
    memset(smpte2109->dynamic_tags, '\0', sizeof(smpte2109->dynamic_tags));
}


#endif /* PMD_SMPTE_2109_H */
