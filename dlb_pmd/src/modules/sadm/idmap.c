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
