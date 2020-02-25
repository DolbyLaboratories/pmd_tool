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

/**
 * @file pmd_idmap.h
 * @brief data structure for maintaining maps from PMD ids to array indexes
 *
 * For convenience, XML tags can have 'id' attributes that allow them to be
 * referenced in other tags.  However, for efficiency, we'd like to translate
 * id attributes into corresponding array entries, so that we can perform
 * lookups in O(1) time.
 *
 * We implement a simple mapping function using hash tables.
 */

#ifndef PARSER_IDMAP_INC_
#define PARSER_IDMAP_INC_

#include "pmd_types.h"
#include <assert.h>

#define MAP_SIZE (4096)

typedef struct
{
    uint16_t map[MAP_SIZE];
} pmd_idmap;


static inline
void
pmd_idmap_init
   (pmd_idmap *map
   )
{
    memset(map, '\xff', sizeof(*map));
}


/**
 * @brief establish a mapping from a PMD id to an array index
 */
static inline
void
pmd_idmap_insert
    (pmd_idmap *map         /**< [in] map */
    ,uint16_t id            /**< [in] PMD id value */
    ,uint16_t array_index   /**< [in] data structure array index */
    )
{
    map->map[id] = array_index;	
}


/**
 * @brief lookup PMD id attribute to find array index
 */
static inline
pmd_bool                    /** @return 1 if found, 0 otherwise */
pmd_idmap_lookup
    (const pmd_idmap *map   /**< [in] map */
    ,uint16_t id            /**< [in] id value */
    ,uint16_t *index        /**< [out] internal array index, if found */
    )
{
    *index = map->map[id];
    return (*index != 0xffff);
}


/**
 * @brief helper for iterators
 */
static inline
pmd_bool                    /** @return 1 if found, 0 otherwise */
pmd_idmap_iterate_next
    (const pmd_idmap *map   /**< [in] map */
    ,unsigned int limit     /**< [in] max limit */
    ,unsigned int *count    /**< [in] index to look at */
    ,uint16_t *value        /**< [in] mapped value */
    )
{
    unsigned int it = *count;
    assert(limit < MAP_SIZE);
    while (it <= limit)
    {
        *value = map->map[it];
        if (0xffff != *value)
        {
            *count = it + 1;
            return 1;
        }
        ++it;
    }
    *count = it + 1;
    return 0;
}


#endif /* PMD_IDMAP_H_ */
