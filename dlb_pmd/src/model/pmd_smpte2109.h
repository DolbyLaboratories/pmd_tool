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
