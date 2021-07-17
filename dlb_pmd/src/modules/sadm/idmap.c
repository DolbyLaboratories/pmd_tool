/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019-2020 by Dolby Laboratories,
 *                Copyright (C) 2019-2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#include "sadm/memstuff.h"
#include "sadm/lookup3.h"
#include "idmap.h"

#include <string.h>
#include <stdio.h>


/**
 * @brief an instance of an association beween an id reference and a PMD id
 */
typedef struct
{
    uintptr_t key;
    uintptr_t val;
} idmapping;


/**
 * @brief type of map
 */
struct idmap
{
    idmapping *pool;
    idmapping **map;
    size_t num;
    size_t max;
};


static inline
size_t
next_power_of_2
    (size_t n
    )
{
    size_t ret = 1;
    while (ret <= n) ret <<= 1;
    return ret;
}


/**
 * @brief compute hash into the idref table, taking into account the
 * id string, the type of thing being referenced and the number of
 * times we've had to recompute the hash
 */
static inline
uint32_t
compute_hash
    (idmap *map
    ,uintptr_t key
    ,unsigned int iteration
    )
{
    char tmp[sizeof(uintptr_t)*2+1];
    size_t len;

    memset(tmp, '\0', sizeof(tmp));
    memmove(tmp, &key, sizeof(key));
    memmove(&tmp[sizeof(key)], &iteration, sizeof(iteration));
    
    len = sizeof(key) + sizeof(iteration);
    /* todo: use hashbig on big-endian platforms */
    return hashlittle(tmp, len, 0) & (map->max-1);
}


size_t
idmap_query_memory
    (size_t max
    )
{
    size_t sz = next_power_of_2(max);
    return MEMREQ(idmap, 1)
        + MEMREQ(idmapping, sz)
        + MEMREQ(idmapping*, sz);
}


void
idmap_init
    (size_t max
    ,void *mem
    ,idmap **mapptr
    )
{
    uintptr_t mc = (uintptr_t)mem;
    idmap *map = (idmap*)mc;
    size_t sz = next_power_of_2(max);

    map->num = 0;
    map->max = max;

    mc += MEMREQ(idmap, 1);
    map->pool = (idmapping *)mc;
    mc += MEMREQ(idmapping, sz);
    map->map = (idmapping**)mc;
    *mapptr = map;

    memset(map->pool, '\0', MEMREQ(idmapping, sz));
    memset(map->map, '\0', MEMREQ(idmapping*, sz));
}


dlb_pmd_success
idmap_assoc
    (idmap *map
    ,uintptr_t key
    ,uintptr_t val
    )
{
    unsigned int i;

    for (i = 0; i != map->max; ++i)
    {
        unsigned int idx = compute_hash(map, key, i);
        idmapping *mapping = map->map[idx];
        if (!mapping)
        {
            if (map->num >= map->max)
            {
                return PMD_FAIL;
            }
            mapping = &map->pool[map->num];
            mapping->key = key;
            mapping->val = val;
            map->map[idx] = mapping;
            map->num += 1;
            return PMD_SUCCESS;
        }
        else if (mapping->key == key)
        {
            mapping->val = val;
            return PMD_SUCCESS;
        }
    }
    return PMD_FAIL;
}


dlb_pmd_success
idmap_lookup
    (idmap *map
    ,uintptr_t key
    ,uintptr_t *val
    )
{
    unsigned int i;

    for (i = 0; i != map->num; ++i)
    {
        unsigned int idx = compute_hash(map, key, i);
        idmapping *mapping = map->map[idx];
        if (!mapping)
        {
            return PMD_FAIL;
        }
        else if (mapping->key == key)
        {
            *val = mapping->val;
            return PMD_SUCCESS;
        }
    }
    return PMD_FAIL;
}
