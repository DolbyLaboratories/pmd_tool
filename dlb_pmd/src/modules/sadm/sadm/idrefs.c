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

/**
 * @file idrefs.c
 * @brief simple hash table to map SADM identifiers to pointers
 *
 * We use a simple <scheme>-collision hash-table. Since we don't delete
 * entries from this table, we don't need a special value for 'was used, but
 * now isn't'
 */


#include "idrefs.h"
#include "lookup3.h"
#include "memstuff.h"
#include <string.h>
#include <stdio.h>

#if defined(_MSC_VER)
#  define snprintf _snprintf
#endif


//#define DBG
#ifdef DBG
#  define TRACE(x) printf x
#else
#  define TRACE(x)
#endif


/**
 * @brief type of entry stored and retrieved in the idref table
 */
typedef struct
{
    dlb_sadm_idref_type type;       /**< type of structure pointed to */
    dlb_sadm_id id;
    unsigned int lineno;
    dlb_pmd_bool common;            /**< true if this refers to a common definition */
    void *ptr;
} idref;


/**
 * @def REF_EQUAL(ref, ty, id)
 * @brief macro to check that a reference in the table matches the given key fields
 */
#define REF_EQUAL(ref, ty, id)                                  \
    (((ref)->type == (ty))                                      \
     && !strcmp((const char*)(ref)->id.data, (const char*)(id)))


/**
 * @brief definition of the idref table data structure
 */
struct idref_table
{
    idref *entries;
    size_t max;
    size_t num;
};


/**
 * @brief compute hash into the idref table, taking into account the
 * id string, the type of thing being referenced and the number of
 * times we've had to recompute the hash
 */
static
uint32_t
compute_hash
    (dlb_sadm_idref_type ty
    ,const char *id
    ,unsigned int iteration
    )
{
    char tmp[DLB_PMD_MAX_NAME_LENGTH + 1 + 16];
    size_t len;
    
    len = snprintf(tmp, sizeof(tmp), "%c:%u:%s", 'a'+(char)ty, iteration, id);

    /* todo: use hashbig on big-endian platforms */
    return hashlittle(tmp, len, 0);
}


/**
 * @brief determine amount of entries required
 */
static
size_t
count_entries
    (dlb_sadm_counts *limits
    )
{
    size_t count;
    size_t sz;

    count = limits->num_programmes
        + limits->num_contents
        + limits->num_objects
        + limits->num_packfmts
        + limits->num_chanfmts
        + limits->num_blkfmts
        + limits->num_track_uids;


    /* round up to next power of 2 */
    sz = 1;

    while (sz <= count) sz <<= 1;
    return sz;
}


/* -------------------------  public api ----------------------------------- */

size_t
idref_table_query_mem
    (dlb_sadm_counts *limits
    )
{
    size_t sz = 0;

    if (NULL != limits)
    {
        sz = count_entries(limits);
        return MEMREQ(idref_table, 1)
             + MEMREQ(idref, sz);
    }
    return sz;
}


dlb_pmd_success
idref_table_init
    (dlb_sadm_counts *limits
    ,void *mem
    ,idref_table **ptr
    )
{
    if (NULL != limits)
    {
        size_t sz = count_entries(limits);
        uintptr_t m = (uintptr_t)mem;
        idref_table *irt = (idref_table*)m;

        m += MEMREQ(idref_table, 1);
        irt->entries = (idref *)m;
        memset(irt->entries, '\0', sizeof(idref) * sz);
        irt->max = sz;
        irt->num = 0;
        *ptr = irt;
        return PMD_SUCCESS;
    }
    return PMD_FAIL;
}


void
idref_table_finish
    (idref_table *irt
    )
{
    (void)irt;
}


void
idref_table_reinit
    (idref_table *irt
    )
{
    memset(irt->entries, '\0', sizeof(idref) * irt->max);
    irt->num = 0;
}


dlb_pmd_success
idref_table_insert
    (idref_table *irt
    ,const unsigned char *id
    ,dlb_sadm_idref_type ty
    ,unsigned int lineno
    ,void *ptr
    ,void **result              /* F---ing *seriously*, Andrew? */
    )
{
    if (irt->num < irt->max)
    {
        unsigned int i;
        
        for (i = 0; i != irt->max; ++i)
        {
            uint32_t hash = compute_hash(ty, (const char*)id, i);
            uint32_t idx = hash & (irt->max-1);
            idref *ref = &irt->entries[idx];

            TRACE(("IDREF:  hash(id=%s,ty=%u) to entry %u (%s/%u/%p)\n",
                   id, ty, idx, (const char*)ref->id.data, ref->type, ref->ptr));
            if (ref->ptr == NULL || REF_EQUAL(ref, ty, id))
            {
                irt->num += (ref->ptr == NULL);
                if (ref->ptr == NULL || ptr != NULL)
                {
                    ref->type = ty;
                    ref->ptr = ptr ? ptr : FORWARD_REFERENCE;
                    ref->lineno = lineno;
                    ref->common = PMD_FALSE;
                    memmove(ref->id.data, id, sizeof(ref->id.data));
                    TRACE(("IDREF: insert (id=%s,ty=%u) %s at entry %u\n",
                           id, ty, ptr ? "definition" : "forward ref", idx));
                }
                if (result)
                {
                    *result = ref;
                }
                return PMD_SUCCESS;
            }
        }
    }
    if (result)
    {
        *result = NULL;
    }
    return PMD_FAIL;
}


dlb_pmd_success
idref_table_lookup
    (idref_table *irt
    ,const unsigned char *id      
    ,dlb_sadm_idref_type ty
    ,dlb_sadm_idref *idrefp
    ,void **ptr
    )
{
    unsigned int i;

    for (i = 0; i != irt->num; ++i)
    {
        uint32_t hash = compute_hash(ty, (const char*)id, i);
        uint32_t idx = hash & (irt->max - 1);
        idref *ref = &irt->entries[idx];

        if (ref->ptr == NULL || REF_EQUAL(ref, ty, id))
        {
            if (idrefp != NULL)
            {
                *idrefp = ref;
            }
            *ptr = ref->ptr;
            return (ref->ptr == NULL || ref->ptr == FORWARD_REFERENCE)
                ? PMD_FAIL
                : PMD_SUCCESS;
        }
    }
    return PMD_FAIL;
}


dlb_pmd_bool
idref_is_null
    (const dlb_sadm_idref i
    )
{
    return 0 == i;
}


dlb_pmd_bool
idref_is_common_def
    (const dlb_sadm_idref i
    )
{
    dlb_pmd_bool is_common = PMD_FALSE;
    idref *iref = (idref*)i;

    if (0 != iref)
    {
        is_common = iref->common;
    }

    return is_common;
}


dlb_pmd_success
idref_set_is_common_def
    (dlb_sadm_idref i
    ,dlb_pmd_bool is_common
    )
{
    dlb_pmd_success success = PMD_FAIL;
    idref *iref = (idref*)i;

    if (0 != iref)
    {
        iref->common = is_common;
        success = PMD_SUCCESS;
    }

    return success;
}


dlb_pmd_success
idref_unpack
    (dlb_sadm_idref i
    ,dlb_sadm_idref_type ty
    ,void **rawptr
    )
{
    idref *iref = (idref*)i;
    if (NULL == iref || iref->type != ty)
    {
        return PMD_FAIL;
    }
    *rawptr = iref->ptr;
    return PMD_SUCCESS;
}


dlb_pmd_success
idref_name
    (dlb_sadm_idref i
    ,const char **name
    )
{
    idref *iref = (idref*)i;
    *name = (const char*)iref->id.data;
    return PMD_SUCCESS;
}


dlb_pmd_success
idref_defined
    (dlb_sadm_idref i
    )
{
    idref *iref = (idref*)i;
    if (iref->ptr == NULL || iref->ptr == FORWARD_REFERENCE)
    {
        return PMD_FAIL;
    }
    return PMD_SUCCESS;
}


dlb_pmd_bool
idref_equal
    (dlb_sadm_idref i1
    ,dlb_sadm_idref i2
    )
{
    idref *r1 = (idref*)i1;
    idref *r2 = (idref*)i2;

    if (r1->type == r2->type && strncmp((const char *)r1->id.data, (const char *)r2->id.data, sizeof(r1->id.data)) == 0)
    {
        return PMD_TRUE;
    }
    return PMD_FALSE;
}


size_t
idref_table_get_undefined_references
    (idref_table *irt
    ,dlb_sadm_undefined_ref *undef
    ,size_t capacity
    )
{
    idref *iref = irt->entries;
    unsigned int i;
    size_t count = 0;

    for (i = 0; i != irt->max; ++i, ++iref)
    {
        if (iref->ptr == FORWARD_REFERENCE)
        {
            TRACE(("---- found forward reference at entry %u\n", i));
            if (count < capacity)
            {
                undef[count].id = iref->id.data;
                undef[count].type = iref->type;
                undef[count].lineno = iref->lineno;
            }
            ++count;
        }
    }
    return count;
}

