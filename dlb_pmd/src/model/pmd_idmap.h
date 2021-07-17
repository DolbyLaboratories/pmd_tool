/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2016-2019 by Dolby Laboratories,
 *                Copyright (C) 2016-2019 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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
