/************************************************************************
 * dlb_pmd
 * Copyright (c) 2018-2019, Dolby Laboratories Inc.
 * Copyright (c) 2018-2019, Dolby International AB.
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

#ifndef PMD_APN_H
#define PMD_APN_H

/**
 * @file pmd_apn.h
 * @brief definitions for handling Audio Presentation Name (APN) payload
 *
 * Unlike element names, presentations may have multiple different
 * names, depending on the language. Moreover, names are expected to
 * be delivered at a much lower rate than the actual bed, object and
 * presentation information. This means that names must persist while
 * the model is emptied and refreshed.
 *
 * Similarly, it means that all incoming presentations must check the
 * current list of names to see if there are any belonging to it, and
 * must understand when names become stale and must be removed.
 */

#include "pmd_apd.h"
#include <stdarg.h>

//#define DEBUG_PRESENTATION_NAMES


/**
 * @brief internal type of presentation name
 */
typedef struct
{
    uint16_t idx;                           /**< index of the presentation name */
    uint16_t next;                          /**< index of next presentation in list */
    uint16_t presid;                        /**< associated presentation */
    uint16_t readcount;                     /**< number of times this name is read,
                                              * used to detect stale names */
    pmd_langcode lang;                      /**< presentation name language */
    uint8_t  text[DLB_PMD_NAME_ARRAY_SIZE]; /**< name text (Unicode) */
} pmd_apn;


/**
 * @brief the presentation name list can vary over time, so we use a freelist
 *
 * Unlike element names, there can be several presentation names, one
 * per language. Moreover, the update cycle of names is not tied to
 * the update cycle of presentations. Every video frame, for example,
 * the beds, objects and presentations will be retransmitted. This is
 * because these three kinds of thing are required to render
 * audio. Every other kind of PMD data is not mandatory for rendering,
 * and is scheduled at a lower refresh rate. e.g., if presentations
 * are refreshed every video frame, names may refresh every ten video
 * frames, or a hundred video frames depending on transmission
 * bandwidth and number of names.
 *
 * This means that the list of presentations may change half-way
 * through an update of names, which in turn means that names might
 * become stale, i.e., they refer to a previous version of the
 * presentation, or a no-longer existing presentation. The next cycle
 * of updates, the name would not be transmitted.
 *
 * We therefore need to manage how to detect when names go out or come
 * into scope. To do this, we keep a 'read count', or the number of
 * times a name has been read into the model. This is kind of like a
 * generation counter. If a name goes out of scope, eventually it will
 * not be read back in, and therefore the read count will not
 * increment.
 *
 * If we assume that the maximum read count is the current
 * 'generation', then we know that any name which has a lower read
 * count is not known to belong to the current generation. If its read
 * count is only one lower than the maximum, of course, then it may
 * not have been read during the current cycle. But if it is two
 * lower, then we know it did not appear in the previous cycle, and is
 * therefore stale.
 *
 * When a name is found to be stale, the slot it occupies (which may
 * occur at any point in the list of names) is remove from the current
 * list and put into the free list.
 */
typedef struct
{
    pmd_apn     *pool;                          /**< memory pool for name allocation */
    unsigned int num;                           /**< number of current names */
    unsigned int max;                           /**< maximum number of names */
    uint16_t     list;                          /**< list of current names */
    uint16_t     free;                          /**< list of unused slots in pool */
    uint16_t     max_readcount;                 /**< largest number of times a name has
                                                  * been read in */
} pmd_apn_list;
    

/**
 * @def PMD_APN_LIST_END
 * @brief special index marker that signifies 'no more'
 */
#define PMD_APN_LIST_END (0xffffu)


#ifdef DEBUG_PRESENTATION_NAMES
static inline
void
TRACE_LIST
    (pmd_apn_list *nl  /**< [in] name list to initialize */
    ,const char *fmt
    ,...
    )
{
    pmd_apn *name;
    char hdr[64];
    uint16_t idx;
    va_list args;

    snprintf(hdr, sizeof(hdr), "NAMELIST[%p] %s", (void*)nl, fmt);
    va_start(args, nl);
    vprintf((const char*)hdr, args);
    va_end(args);
    printf(": [");

    idx = nl->list;
    while (PMD_APN_LIST_END != idx)
    {
        name = &nl->pool[idx];
        printf("%u ", name->idx);
        idx = name->next;
    }
    printf("]\n");
}
#else
#  define TRACE_LIST(nl, ...)
#endif


/**
 * @brief initialize the model's presentation name list
 */
static inline
void
pmd_apn_list_init
    (pmd_apn_list *nl  /**< [in] name list to initialize */
    ,unsigned int maxm /**< [in] max number allowed */
    )
{
    pmd_apn *name;
    uint16_t max = maxm > 65535 ? 65535 : (uint16_t)maxm;
    uint16_t i;

    name = nl->pool;
    for (i = 0; i != max; ++i)
    {
        memset(name, '\0', sizeof(*name));
        name->idx = i;
        name->next = (i+1 == max) ? (unsigned short)PMD_APN_LIST_END : i+1;
        ++name;
    }
    nl->free = 0;
    nl->list = PMD_APN_LIST_END;
    nl->num = 0;
    nl->max = max;
    nl->max_readcount = 0;
}


/**
 * @brief add a new presentation name to the list
 */
static inline
pmd_apn*                /** @return new name struct to populate, or NULL if no more */
pmd_apn_list_add
    (pmd_apn_list *nl   /**< [in] name list to add presentation name to */
    )
{
    pmd_apn *name;
    
    if (nl->num == nl->max)
    {
        return NULL;
    }
    
    name = &nl->pool[nl->free];
    nl->free = name->next;
    name->next = nl->list;
    nl->list = name->idx;
    nl->num += 1;
    name->readcount = nl->max_readcount - 1;

    TRACE_LIST(nl, "add %u", name->idx);
    return name;
}


/**
 * @brief look up a presentation name by its index
 */
static inline
const pmd_apn*                /** @return name structure */
pmd_apn_list_lookup
    (const pmd_apn_list *nl   /**< [in] name list */
    ,uint16_t idx             /**< [in] name index */
    )
{
    return &nl->pool[idx];
}


/**
 * @brief search for a name with the given presentation id and language
 */
static inline
pmd_apn*                        /** @return name or NULL if not found */
pmd_apn_list_find
    (pmd_apn_list *nl           /**< [in] name list to search */
    ,unsigned int presid        /**< [in] required presentation id */
    ,pmd_langcode lang          /**< [in] required presentation name language */
    )
{
    pmd_apn *name;
    uint16_t idx = nl->list;

    while (PMD_APN_LIST_END != idx)
    {
        name = &nl->pool[idx];
        if (name->presid == presid && name->lang == lang)
        {
            return name;
        }
        idx = name->next;
    }
    return NULL;
}


/**
 * @brief mark a name as recently read
 *
 * We keep track of the number of times a name has been marked to determine
 * how 'up to date' it is.
 */
static inline
void
pmd_apn_list_mark
    (pmd_apn_list *nl   /**< [in] name list */
    ,pmd_apn *name      /**< [in] name to mark afresh */
    )
{
    name->readcount += 1;
    if (name->readcount > nl->max_readcount)
    {
        nl->max_readcount = name->readcount;
    }
}


/**
 * @brief remove a name from the namelist and return to the freelist
 */
static inline
void
pmd_apn_list_remove
    (pmd_apn_list *nl      /**< [in] name list */
    ,pmd_apn      *prev    /**< [in] previous name in list, or NULL for beginning */
    ,pmd_apn      *name    /**< [in] name to remove and return to freelist */
    )
{
    if (NULL == prev)
    {
        nl->list = name->next;
    }
    else
    {
        prev->next = name->next;
    }
    name->next = nl->free;
    name->lang = 0;
    nl->free = name->idx;
    nl->num -= 1;
    TRACE_LIST(nl, "remove %u", name->idx);
}


/**
 * @brief find all names associated with the given presentation, and add
 * them to the presentation structure
 */
static inline
void
pmd_apn_list_isolate
    (pmd_apn_list *nl       /**< [in] namelist */
    ,pmd_apd *pres          /**< [in] presentation to match */
    )
{
    pmd_apn *prev = NULL;
    pmd_apn *name;
    uint16_t idx = nl->list;

    pres->num_names = 0;
    while (PMD_APN_LIST_END != idx)
    {
        name = &nl->pool[idx];
        idx = name->next;
        if (name->presid == pres->id)
        {
            if ((nl->max_readcount - name->readcount) < 2)
            {
                pres->names[pres->num_names] = name->idx;
                pres->num_names += 1;
                prev = name;
            }
            else
            {
                pmd_apn_list_remove(nl, prev, name);
            }
        }
        else
        {
            prev = name;
        }
    }
}


/**
 * @brief type of name list iterator
 */
typedef struct
{
    pmd_apn_list *nl;         /**< namelist being iterated over */
    uint16_t idx;             /**< current index into namelist pool */
} pmd_apn_list_iterator;
    

/**
 * @brief initialize namelist iterator
 */
static inline
void
pmd_apn_list_iterator_init
    (pmd_apn_list_iterator *it  /**< [in] iterator being initialized */
    ,pmd_apn_list *nl           /**< [in] name list being iterated over */
    )
{
    it->nl = nl;
    it->idx = nl->list;
}


/**
 * @brief read the current entry of the iterator
 */
static inline
pmd_apn*                          /** @return current name, or NULL if none */
pmd_apn_list_iterator_get
    (pmd_apn_list_iterator *it    /**< [in] iterator being queried */
    )
{
    if (it->idx == PMD_APN_LIST_END)
    {
        return NULL;
    }
    
    return &it->nl->pool[it->idx];
}


/**
 * @brief move to the next name in the list
 */
static inline
void
pmd_apn_list_iterator_next
    (pmd_apn_list_iterator *it   /**< [in] iterator */
    )
{
    if (it->idx != PMD_APN_LIST_END)
    {
        pmd_apn *name = &it->nl->pool[it->idx];
        it->idx = name->next;
    }
}


/**
 * @brief has the iterator finished?
 */
static inline
pmd_bool                         /** @return 1 if it has finished, 0 otherwise */
pmd_apn_list_iterator_done
    (pmd_apn_list_iterator *it   /**< [in] iterator */
    )
{
    return it->idx == PMD_APN_LIST_END;
}



#endif /* PMD_APN_H */
