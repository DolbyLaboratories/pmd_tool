/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019 by Dolby Laboratories,
 *                Copyright (C) 2019 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

/**
 * @file idmap.h
 * @brief simple mapping between uintptr_t types
 */


#include "dlb_pmd_api.h"


/**
 * @brief type of map
 */
typedef struct idmap idmap;


size_t
idmap_query_memory
    (size_t max
    );


void
idmap_init
    (size_t max
    ,void *mem
    ,idmap **mapptr
    );


dlb_pmd_success
idmap_assoc
    (idmap *map
    ,uintptr_t key
    ,uintptr_t val
    );


dlb_pmd_success
idmap_lookup
    (idmap *map
    ,uintptr_t key
    ,uintptr_t *val
    );
