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

/**
 * @file dlb_sadm_model.c
 * @brief internal data structure to store Dolby-constrained Serial ADM data
 */


#include "sadm/dlb_sadm_model.h"
#include "dlb_pmd_api.h"
#include "pmd_model.h"
#include "sadm_error_helper.h"
#include "dlb_sadm_common_definitions.h"
#include "memstuff.h"
#include "idrefs.h"
#include <stdarg.h>
#include <string.h>
#include <ctype.h>


#if defined(_MSC_VER)
#  if _MSC_VER < 1900 && !defined(inline)
#    define inline __inline
#  endif
#endif

#define FLOW_ID_LEN     (36)
#define FLOW_ID_BUF_LEN (FLOW_ID_LEN + 1)

#define CHECK_SUCCESS(S) if ((S) == PMD_FAIL) return PMD_FAIL

struct dlb_sadm_model
{
    DLB_SADM_FRAME_FORMAT      frame_format;

    char error[256];
    idref_table               *irt;

    dlb_sadm_counts            limits;
    dlb_sadm_counts            counts;
    
    dlb_sadm_programme        *programmes;
    dlb_sadm_content          *contents;
    dlb_sadm_object           *objects;
    dlb_sadm_pack_format      *packfmts;
    dlb_sadm_channel_format   *chanfmts;
    dlb_sadm_block_format     *blkfmts;
    dlb_sadm_stream_format    *streamfmt;       /* N.B.: not used */
    dlb_sadm_track_format     *trackfmt;        /* N.B.: not used */
    dlb_sadm_track_uid        *track_uids;

    char                       flow_id[FLOW_ID_BUF_LEN];
};
    

/**
 * @brief helper function to copy one idref array to another
 */
static inline
void
copy_idref_array
    (dlb_sadm_idref_array *dest
    ,dlb_sadm_idref_array *src
    )
{
    assert(dest->max >= src->num);
    dest->num = src->num;
    memmove(dest->array, src->array, sizeof(dlb_sadm_idref) * src->num);
}


void
dlb_sadm_get_default_counts
    (dlb_sadm_counts *limits
    )
{
    static const size_t NUM_TRACKS = 16;

    memset(limits, 0, sizeof(*limits));

    limits->max_programme_labels   =  6;
    limits->max_programme_contents = 32;
    limits->max_content_objects    = NUM_TRACKS;
    limits->max_object_objects     = MAX_AO_AO;
    limits->max_object_track_uids  = NUM_TRACKS;
    limits->max_packfmt_chanfmts   = NUM_COMMON_PACKFMT_CHANFMT;
    limits->max_chanfmt_blkfmts    =  1 + 1; /* +1 for Fhg and NHK sADM content */

    limits->num_programmes         = 10;
    limits->num_contents           = 10;
    limits->num_objects            =  8;
    limits->num_packfmts           =  6 + NUM_COMMON_AUDIO_PACK_FORMATS;
    limits->num_chanfmts           = 24 + NUM_COMMON_AUDIO_CHANNEL_FORMATS;
    limits->num_blkfmts            = limits->num_chanfmts * limits->max_chanfmt_blkfmts;
    limits->num_track_uids         = NUM_TRACKS;

    limits->use_common_defs        = PMD_FALSE;
}


size_t
dlb_sadm_query_memory
    (dlb_sadm_counts *c
    )
{
    dlb_sadm_counts limits;

    if (NULL == c)
    {
        c = &limits;
        dlb_sadm_get_default_counts(c);
    }

    return MEMREQ(dlb_sadm_model,           1)
        +  MEMREQ(dlb_sadm_programme,       c->num_programmes)
        +  MEMREQ(dlb_sadm_programme_label, c->max_programme_labels) * c->num_programmes
        +  MEMREQ(dlb_sadm_content,         c->num_contents)
        +  MEMREQ(dlb_sadm_object,          c->num_objects)
        +  MEMREQ(dlb_sadm_pack_format,     c->num_packfmts)
        +  MEMREQ(dlb_sadm_channel_format,  c->num_chanfmts)
        +  MEMREQ(dlb_sadm_block_format,    c->num_blkfmts)
        +  MEMREQ(dlb_sadm_track_uid,       c->num_track_uids)

        +  MEMREQ(dlb_sadm_idref,           c->max_programme_contents) * c->num_programmes
        +  MEMREQ(dlb_sadm_idref,           c->max_content_objects)    * c->num_contents
        +  MEMREQ(dlb_sadm_idref,           c->max_object_objects)     * c->num_objects
        +  MEMREQ(dlb_sadm_idref,           c->max_object_track_uids)  * c->num_objects
        +  MEMREQ(dlb_sadm_idref,           c->max_packfmt_chanfmts)   * c->num_packfmts
        +  MEMREQ(dlb_sadm_idref,           c->max_chanfmt_blkfmts)    * c->num_chanfmts
        +  idref_table_query_mem(c)
        ;
}


dlb_pmd_success
dlb_sadm_init
    (dlb_sadm_counts *c
    ,void *mem
    ,dlb_sadm_model **mptr
    )
{
    dlb_sadm_counts limits;
    dlb_sadm_model *m = (dlb_sadm_model *)mem;
    uintptr_t rp = (uintptr_t)(m + 1);
    dlb_sadm_programme *prog;
    dlb_sadm_content *content;
    dlb_sadm_object *obj;
    dlb_sadm_pack_format *packfmt;
    dlb_sadm_channel_format *chanfmt;
    dlb_pmd_success success;
    unsigned int i;
    size_t sz;

    if (NULL == c)
    {
        c = &limits;
        dlb_sadm_get_default_counts(c);
    }
    sz = dlb_sadm_query_memory(c);
    memset(mem, '\0', sz);
    m->limits = *c;
    *mptr = m;

    m->programmes = (dlb_sadm_programme*)rp;
    rp += MEMREQ(dlb_sadm_programme, c->num_programmes);

    m->contents = (dlb_sadm_content*)rp;
    rp += MEMREQ(dlb_sadm_content, c->num_contents);

    m->objects = (dlb_sadm_object*)rp;
    rp += MEMREQ(dlb_sadm_object, c->num_objects);
    
    m->packfmts = (dlb_sadm_pack_format*)rp;
    rp += MEMREQ(dlb_sadm_pack_format, c->num_packfmts);

    m->chanfmts = (dlb_sadm_channel_format*)rp;
    rp += MEMREQ(dlb_sadm_channel_format, c->num_chanfmts);

    m->blkfmts = (dlb_sadm_block_format*)rp;
    rp += MEMREQ(dlb_sadm_block_format, c->num_blkfmts);

    m->track_uids = (dlb_sadm_track_uid*)rp;
    rp += MEMREQ(dlb_sadm_track_uid, c->num_track_uids);
    
    /* now add programme label arrays to programmes */
    prog = m->programmes;
    for (i = 0; i != c->num_programmes; ++i, ++prog)
    {
        prog->labels = (dlb_sadm_programme_label*)rp;
        rp += MEMREQ(dlb_sadm_programme_label, c->max_programme_labels);
        prog->contents.array = (dlb_sadm_idref*)rp;
        prog->contents.max = (unsigned int)c->max_programme_contents;
        rp += MEMREQ(dlb_sadm_idref, c->max_programme_contents);
    }

    /* add object arrays to contents */
    content = m->contents;
    for (i = 0; i != c->num_contents; ++i, ++content)
    {
        content->objects.array = (dlb_sadm_idref *)rp;
        content->objects.max = (unsigned int)c->max_content_objects;
        rp += MEMREQ(dlb_sadm_idref, c->max_content_objects);
    }

    /* now add object_refs and track_uids idref arrays to objects */
    obj = m->objects;
    for (i = 0; i != c->num_objects; ++i, ++obj)
    {
        obj->object_refs.array = (dlb_sadm_idref*)rp;
        obj->object_refs.max = (unsigned int)c->max_object_objects;
        rp += MEMREQ(dlb_sadm_idref, c->max_object_objects);
        obj->track_uids.array = (dlb_sadm_idref*)rp;
        obj->track_uids.max = (unsigned int)c->max_object_track_uids;
        rp += MEMREQ(dlb_sadm_idref, c->max_object_track_uids);
    }
    
    packfmt = m->packfmts;
    for (i = 0; i != c->num_packfmts; ++i, ++packfmt)
    {
        packfmt->chanfmts.array = (dlb_sadm_idref*)rp;
        packfmt->chanfmts.max = (unsigned int)c->max_packfmt_chanfmts;
        rp += MEMREQ(dlb_sadm_idref, c->max_packfmt_chanfmts);
    }

    chanfmt = m->chanfmts;
    for (i = 0; i != c->num_chanfmts; ++i, ++chanfmt)
    {
        chanfmt->blkfmts.array = (dlb_sadm_idref*)rp;
        chanfmt->blkfmts.max = (unsigned int)c->max_chanfmt_blkfmts;
        rp += MEMREQ(dlb_sadm_idref, c->max_chanfmt_blkfmts);
    }

    success = idref_table_init(c, (void*)rp, &m->irt);
    if (PMD_SUCCESS == success && c->use_common_defs)
    {
        /* add common definitions to model */
        success = dlb_sadm_init_common_definitions(m);
    }

    m->frame_format = DLB_SADM_FRAME_FORMAT_FULL;

    return success;
}


void
dlb_sadm_finish
    (dlb_sadm_model *m
    )
{
    idref_table_finish(m->irt);
}


dlb_pmd_success
dlb_sadm_copy
    (dlb_sadm_model *dest
    ,const dlb_sadm_model *src
    )
{
    dlb_pmd_success success;
    size_t i;

    if (dest == NULL || src == NULL)
    {
        return PMD_FAIL;
    }

    // Check src counts vs. dest limits
    if (src->counts.num_programmes > dest->limits.num_programmes ||
        src->counts.num_contents   > dest->limits.num_contents   ||
        src->counts.num_objects    > dest->limits.num_objects    ||
        src->counts.num_packfmts   > dest->limits.num_packfmts   ||
        src->counts.num_blkfmts    > dest->limits.num_blkfmts    ||
        src->counts.num_track_uids > dest->limits.num_track_uids ||

        src->counts.max_programme_labels   > dest->limits.max_programme_labels   ||
        src->counts.max_programme_contents > dest->limits.max_programme_contents ||
        src->counts.max_content_objects    > dest->limits.max_content_objects    ||
        src->counts.max_object_track_uids  > dest->limits.max_object_track_uids  ||
        src->counts.max_packfmt_chanfmts   > dest->limits.max_packfmt_chanfmts   ||
        src->counts.max_chanfmt_blkfmts    > dest->limits.max_chanfmt_blkfmts
        )
    {
        return PMD_FAIL;
    }

    // Clear dest

    memset(&dest->counts, '\0', sizeof(dest->counts));
    idref_table_reinit(dest->irt);

    // Copy everything

    dest->frame_format = src->frame_format;

    // block format
    for (i = 0; i < src->counts.num_blkfmts; i++)
    {
        success = dlb_sadm_set_block_format(dest, &src->blkfmts[i], NULL);
        CHECK_SUCCESS(success);
    }

    // channel format
    for (i = 0; i < src->counts.num_chanfmts; i++)
    {
        success = dlb_sadm_set_channel_format(dest, &src->chanfmts[i], NULL);
        CHECK_SUCCESS(success);
    }

    // pack format
    for (i = 0; i < src->counts.num_packfmts; i++)
    {
        success = dlb_sadm_set_pack_format(dest, &src->packfmts[i], NULL);
        CHECK_SUCCESS(success);
    }

    // stream format
    if (src->streamfmt != NULL)
    {
        return PMD_FAIL;
    }

    // track format
    if (src->trackfmt != NULL)
    {
        return PMD_FAIL;
    }

    // track uid
    for (i = 0; i < src->counts.num_track_uids; i++)
    {
        success = dlb_sadm_set_track_uid(dest, &src->track_uids[i], NULL);
        CHECK_SUCCESS(success);
    }

    // audio object
    for (i = 0; i < src->counts.num_objects; i++)
    {
        success = dlb_sadm_set_object(dest, &src->objects[i], NULL);
        CHECK_SUCCESS(success);
    }

    // audio content
    for (i = 0; i < src->counts.num_contents; i++)
    {
        success = dlb_sadm_set_content(dest, &src->contents[i], NULL);
        CHECK_SUCCESS(success);
    }

    // audio programme
    for (i = 0; i < src->counts.num_programmes; i++)
    {
        success = dlb_sadm_set_programme(dest, &src->programmes[i], NULL);
        CHECK_SUCCESS(success);
    }

    return PMD_SUCCESS;
}


static
dlb_pmd_bool
idref_array_eq
    (const dlb_sadm_idref_array *a1
    ,const dlb_sadm_idref_array *a2
    )
{
    size_t i;

    if (a1->num != a2->num)
    {
        return PMD_FALSE;
    }

    for (i = 0; i < a1->num; i++)
    {
        if (!idref_equal(a1->array[i], a2->array[i]))
        {
            return PMD_FALSE;
        }
    }

    return PMD_TRUE;
}


static inline
dlb_pmd_bool
sadm_id_eq
    (const dlb_sadm_id *id1
    ,const dlb_sadm_id *id2
    )
{
    return strncmp((const char *)id1->data, (const char *)id2->data, sizeof(id1->data)) == 0;
}


static inline
dlb_pmd_bool
sadm_name_eq
    (const dlb_sadm_name *n1
    ,const dlb_sadm_name *n2
    )
{
    return strncmp((const char *)n1->data, (const char *)n2->data, sizeof(n1->data)) == 0;
}


static inline
dlb_pmd_bool
language_eq
    (const char *l1
    ,const char *l2
    )
{
    return strncmp(l1, l2, 4) == 0;     /* TODO: symbolic constant */
}


static
dlb_pmd_bool
programme_label_eq
    (const dlb_sadm_programme_label *l1
    ,const dlb_sadm_programme_label *l2
    )
{
    return
        language_eq(l1->language, l2->language) &&
        sadm_name_eq(&l1->name, &l2->name);
}


static
dlb_pmd_bool
programme_eq
    (const dlb_sadm_programme *p1
    ,const dlb_sadm_programme *p2
    )
{
    if (sadm_id_eq    (&p1->id,           &p2->id)       &&
        sadm_name_eq  (&p1->name,         &p2->name)     &&
        language_eq   ( p1->language,      p2->language) &&
        idref_array_eq(&p1->contents,     &p2->contents) &&
                        p1->num_labels ==  p2->num_labels)
    {
        size_t i;

        for (i = 0; i < p1->num_labels; i++)
        {
            if (!programme_label_eq(&p1->labels[i], &p2->labels[i]))
            {
                return PMD_FALSE;
            }
        }
        return PMD_TRUE;
    }
    return PMD_FALSE;
}


static
dlb_pmd_bool
content_label_eq
    (const dlb_sadm_content_label *l1
    ,const dlb_sadm_content_label *l2
    )
{
    return
        language_eq(l1->language, l2->language) &&
        sadm_name_eq(&l1->name, &l2->name);
}


static
dlb_pmd_bool
content_eq
    (const dlb_sadm_content *c1
    ,const dlb_sadm_content *c2
    )
{
    return
        sadm_id_eq(&c1->id, &c2->id) &&
        sadm_name_eq(&c1->name, &c2->name) &&
        c1->dialogue_value == c2->dialogue_value &&
        c1->type == c2->type &&
        content_label_eq(&c1->label, &c2->label) &&
        idref_array_eq(&c1->objects, &c2->objects);
}


static
dlb_pmd_bool
object_eq
    (const dlb_sadm_object *o1
    ,const dlb_sadm_object *o2
    )
{
    return
        sadm_id_eq(&o1->id, &o2->id) &&
        sadm_name_eq(&o1->name, &o2->name) &&
        o1->gain == o2->gain &&     /* TODO: strict equality between floats might be too restrictive */
        idref_equal(o1->pack_format, o2->pack_format) &&
        idref_array_eq(&o1->track_uids, &o2->track_uids);
}


static
dlb_pmd_bool
track_uid_eq
    (const dlb_sadm_track_uid *t1
    ,const dlb_sadm_track_uid *t2
    )
{
    return
        sadm_id_eq(&t1->id, &t2->id) &&
        idref_equal(t1->chanfmt, t2->chanfmt) &&
        idref_equal(t1->packfmt, t2->packfmt) &&
        t1->channel_idx == t2->channel_idx;
}


static
dlb_pmd_bool
pack_format_eq
    (const dlb_sadm_pack_format *p1
    ,const dlb_sadm_pack_format *p2
    )
{
    return
        sadm_id_eq(&p1->id, &p2->id) &&
        sadm_name_eq(&p1->name, &p2->name) &&
        p1->type == p2->type &&
        idref_array_eq(&p1->chanfmts, &p2->chanfmts);
}


static
dlb_pmd_bool
channel_format_eq
    (const dlb_sadm_channel_format *c1
    ,const dlb_sadm_channel_format *c2
    )
{
    return
        sadm_id_eq(&c1->id, &c2->id) &&
        sadm_name_eq(&c1->name, &c2->name) &&
        idref_array_eq(&c1->blkfmts, &c2->blkfmts);
}


static
dlb_pmd_bool
block_format_eq
    (const dlb_sadm_block_format *b1
    ,const dlb_sadm_block_format *b2
    )
{
    return
        sadm_id_eq(&b1->id, &b2->id) &&
        strncmp((const char *)b1->speaker_label, (const char *)b2->speaker_label, sizeof(b1->speaker_label)) == 0 &&
        b1->gain == b2->gain &&
        b1->cartesian_coordinates == b2->cartesian_coordinates &&
        b1->azimuth_or_x == b2->azimuth_or_x &&
        b1->elevation_or_y == b2->elevation_or_y &&
        b1->azimuth_or_x == b2->distance_or_z;
}


dlb_pmd_bool
dlb_sadm_eq
    (const dlb_sadm_model *m1
    ,const dlb_sadm_model *m2
    )
{
    size_t i;

    if (m1 == m2)           /* This includes both NULL */
    {
        return PMD_TRUE;
    }

    if (m1 == NULL || m2 == NULL)
    {
        return PMD_FALSE;
    }

    // Check metadata frame format
    if (m1->frame_format != m2->frame_format)
    {
        return PMD_FALSE;
    }

    // Check counts
    if (m1->counts.num_programmes != m2->counts.num_programmes ||
        m1->counts.num_contents   != m2->counts.num_contents   ||
        m1->counts.num_objects    != m2->counts.num_objects    ||
        m1->counts.num_packfmts   != m2->counts.num_packfmts   ||
        m1->counts.num_chanfmts   != m2->counts.num_chanfmts   ||
        m1->counts.num_blkfmts    != m2->counts.num_blkfmts    ||
        m1->counts.num_track_uids != m2->counts.num_track_uids
        )
    {
        return PMD_FALSE;
    }

    // Check lists of entities

    // audio programme
    for (i = 0; i < m1->counts.num_programmes; i++)
    {
        if (!programme_eq(&m1->programmes[i], &m2->programmes[i]))
        {
            return PMD_FALSE;
        }
    }

    // audio content
    for (i = 0; i < m1->counts.num_contents; i++)
    {
        if (!content_eq(&m1->contents[i], &m2->contents[i]))
        {
            return PMD_FALSE;
        }
    }

    // audio object
    for (i = 0; i < m1->counts.num_objects; i++)
    {
        if (!object_eq(&m1->objects[i], &m2->objects[i]))
        {
            return PMD_FALSE;
        }
    }

    // track uid
    for (i = 0; i < m1->counts.num_track_uids; i++)
    {
        if (!track_uid_eq(&m1->track_uids[i], &m2->track_uids[i]))
        {
            return PMD_FALSE;
        }
    }

    // track format
    // TODO: not in use?

    // stream format
    // TODO: not in use?

    // pack format
    for (i = 0; i < m1->counts.num_packfmts; i++)
    {
        if (!pack_format_eq(&m1->packfmts[i], &m2->packfmts[i]))
        {
            return PMD_FALSE;
        }
    }

    // channel format
    for (i = 0; i < m1->counts.num_chanfmts; i++)
    {
        if (!channel_format_eq(&m1->chanfmts[i], &m2->chanfmts[i]))
        {
            return PMD_FALSE;
        }
    }

    // block format
    for (i = 0; i < m1->counts.num_blkfmts; i++)
    {
        if (!block_format_eq(&m1->blkfmts[i], &m2->blkfmts[i]))
        {
            return PMD_FALSE;
        }
    }

    return PMD_TRUE;
}

void
dlb_sadm_reinit
    (dlb_sadm_model *m
    )
{
    memset(&m->counts, '\0', sizeof(m->counts));
    idref_table_reinit(m->irt);
    if (m->limits.use_common_defs)
    {
        dlb_sadm_init_common_definitions(m);
    }
}


void
dlb_sadm_set_error
    (const dlb_sadm_model *model
    ,const char *fmt
    ,...
    )
{
    dlb_sadm_model *m = (dlb_sadm_model*)model;
    va_list args;

    va_start(args, fmt);
    (void)vsnprintf(m->error, sizeof(m->error), fmt, args);
    va_end(args);
}


void
dlb_sadm_error_reset
    (const dlb_sadm_model *model
    )
{
    dlb_sadm_model *m = (dlb_sadm_model*)model;
    m->error[0] = '\0';
}


dlb_pmd_success
dlb_sadm_model_frame_format
    (const dlb_sadm_model *model
    ,DLB_SADM_FRAME_FORMAT *frame_format
    )
{
    if (model == NULL || frame_format == NULL)
    {
        return PMD_FAIL;
    }

    *frame_format = model->frame_format;

    return PMD_SUCCESS;
}

dlb_pmd_success
dlb_sadm_model_get_coordinate_print_precision
    (const dlb_sadm_model *model
    ,int *precision
    )
{
    if (model == NULL || precision == NULL)
    {
        return PMD_FAIL;
    }
    *precision = DLB_PMD_DEFAULT_COORDINATE_PRECISION;

    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_sadm_model_limits
    (const dlb_sadm_model *model
    ,dlb_sadm_counts *limits
    )
{
    memmove(limits, &model->limits, sizeof(*limits));
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_sadm_model_counts
    (const dlb_sadm_model *model
    ,dlb_sadm_counts *counts
    )
{
    memmove(counts, &model->counts, sizeof(*counts));
    return PMD_SUCCESS;
}


dlb_pmd_bool
dlb_sadm_idref_is_null
    (const dlb_sadm_idref i
    )
{
    return idref_is_null(i);
}

dlb_pmd_bool dlb_sadm_idref_is_common_def(const dlb_sadm_idref i)
{
    return idref_is_common_def(i);
}

dlb_pmd_success dlb_sadm_idref_set_is_common_def(dlb_sadm_idref i, dlb_pmd_bool is_common)
{
    return idref_set_is_common_def(i, is_common);
}


dlb_pmd_success
dlb_sadm_set_programme
    (dlb_sadm_model *model
    ,dlb_sadm_programme *p
    ,dlb_sadm_idref *idref
    )
{
    dlb_sadm_programme *pptr;
    dlb_sadm_counts *c = &model->counts;
    dlb_pmd_bool is_new = 0;
    
    if (idref_table_lookup(model->irt, p->id.data, DLB_SADM_PROGRAMME, NULL, (void**)&pptr))
    {
        /* not already in model, so this is a new allocation */
        if (c->num_programmes == model->limits.num_programmes)
        {
            dlb_sadm_set_error(model, "Too many audio programmes");
            return PMD_FAIL;
        }
    
        pptr = &model->programmes[c->num_programmes];
        is_new = 1;
    }
    
    memmove(&pptr->id, &p->id, sizeof(p->id));
    memmove(&pptr->name, &p->name, sizeof(p->name));
    memmove(pptr->language, p->language, sizeof(p->language));

    if (p->contents.num > pptr->contents.max)
    {
        dlb_sadm_set_error(model, "Too many contents for programme");
        return PMD_FAIL;
    }

    copy_idref_array(&pptr->contents, &p->contents);
    pptr->num_labels = p->num_labels;
    memmove(pptr->labels, p->labels, p->num_labels * sizeof(p->labels[0]));
    if (idref_table_insert(model->irt, p->id.data, DLB_SADM_PROGRAMME, 0, pptr, idref))
    {
        return PMD_FAIL;
    }
    c->num_programmes += is_new;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_sadm_programme_iterator_init
    (dlb_sadm_programme_iterator *it
    ,const dlb_sadm_model *model
    )
{
    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, it);

    it->model = model;
    it->pos   = 0;
    it->count = (unsigned int)model->counts.num_programmes;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_sadm_programme_iterator_next
   (dlb_sadm_programme_iterator *it
   ,dlb_sadm_programme *prog
   ,dlb_sadm_idref_array *contents
   ,dlb_sadm_programme_label *labels
   ,unsigned int num_labels
   )
{
    const dlb_sadm_model *model;
    dlb_sadm_programme *p;

    if (!it) return PMD_FAIL;
    CHECK_PTRARG(it->model, prog);

    if (it->pos < it->count)
    {
        model = it->model;
        p = &model->programmes[it->pos++];

        memmove(prog, p, sizeof(*p));
        prog->labels = labels;
        prog->num_labels = p->num_labels;
        if (p->num_labels > num_labels)
        {
            prog->num_labels = num_labels;
        }
        memmove(labels, p->labels, sizeof(dlb_sadm_programme_label) * prog->num_labels);
        prog->contents = *contents;
        copy_idref_array(&prog->contents, &p->contents);
        return PMD_SUCCESS;
    }
    return PMD_FAIL;
}


dlb_pmd_success
dlb_sadm_set_content
    (dlb_sadm_model *model
    ,dlb_sadm_content *content
    ,dlb_sadm_idref *idref
    )
{
    dlb_sadm_content *cptr;
    dlb_sadm_counts *c = &model->counts;
    dlb_pmd_bool is_new = 0;
    
    if (idref_table_lookup(model->irt, content->id.data, DLB_SADM_CONTENT, NULL, (void**)&cptr))
    {
        /* not already in model, so this is a new allocation */
        if (c->num_contents == model->limits.num_contents)
        {
            dlb_sadm_set_error(model, "Too many audio contents");
            return PMD_FAIL;
        }
        
        cptr = &model->contents[c->num_contents];
        is_new = 1;
    }
    
    memmove(&cptr->id, &content->id, sizeof(cptr->id));
    memmove(&cptr->name, &content->name, sizeof(cptr->name));
    cptr->dialogue_value = content->dialogue_value;
    cptr->type = content->type;
    memmove(&cptr->label.language, &content->label.language, sizeof(cptr->label.language));
    memmove(&cptr->label.name, &content->label.name, sizeof(cptr->label.name));

    if (content->objects.num > cptr->objects.max)
    {
        dlb_sadm_set_error(model, "Too many objects for content");
        return PMD_FAIL;
    }

    copy_idref_array(&cptr->objects, &content->objects);
    if (idref_table_insert(model->irt, content->id.data, DLB_SADM_CONTENT, 0, cptr, idref))
    {
        return PMD_FAIL;
    }

    c->num_contents += is_new;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_sadm_content_lookup
    (const dlb_sadm_model *model
    ,dlb_sadm_idref idref
    ,dlb_sadm_content *content
    ,dlb_sadm_idref_array *objects
    )
{
    dlb_sadm_content *c;
    void *rawp;

    (void)model;    
    if (idref_unpack(idref, DLB_SADM_CONTENT, &rawp))
    {
        return PMD_FAIL;
    }

    c = (dlb_sadm_content *)rawp;
    memmove(content, c, sizeof(*c));
    content->objects = *objects;

    if (c->objects.num > content->objects.max)
    {
        return PMD_FAIL;
    }

    copy_idref_array(&content->objects, &c->objects);
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_sadm_content_iterator_init
    (dlb_sadm_content_iterator *it
    ,const dlb_sadm_model *model
    )
{
    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, it);

    it->model = model;
    it->pos   = 0;
    it->count = (unsigned int)model->counts.num_contents;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_sadm_content_iterator_next
   (dlb_sadm_content_iterator *it
   ,dlb_sadm_content *content
   )
{
    const dlb_sadm_model *model;
    dlb_sadm_content *c;

    if (!it) return PMD_FAIL;
    CHECK_PTRARG(it->model, content);

    if (it->pos < it->count)
    {
        model = it->model;
        c = &model->contents[it->pos++];

        memmove(content, c, sizeof(*c));
        return PMD_SUCCESS;
    }
    return PMD_FAIL;
}


dlb_pmd_success
dlb_sadm_set_object
    (dlb_sadm_model *model
    ,dlb_sadm_object *o
    ,dlb_sadm_idref *idref
    )
{
    dlb_sadm_object *optr;
    dlb_sadm_counts *c = &model->counts;    
    dlb_pmd_bool is_new = 0;
    
    if (idref_table_lookup(model->irt, o->id.data, DLB_SADM_OBJECT, NULL, (void**)&optr))
    {
        /* not already in model, so this is a new allocation */
        if (c->num_objects == model->limits.num_objects)
        {
            dlb_sadm_set_error(model, "Too many audio objects");
            return PMD_FAIL;
        }
        
        optr = &model->objects[c->num_objects];
        is_new = 1;
    }

    memmove(&optr->id, &o->id, sizeof(o->id));
    memmove(&optr->name, &o->name, sizeof(o->name));
    optr->gain = o->gain;
    optr->pack_format = o->pack_format;
    copy_idref_array(&optr->object_refs, &o->object_refs);
    copy_idref_array(&optr->track_uids, &o->track_uids);

    if (idref_table_insert(model->irt, o->id.data, DLB_SADM_OBJECT, 0, optr, idref))
    {
        return PMD_FAIL;
    }

    model->counts.num_objects += is_new;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_sadm_object_lookup
    (const dlb_sadm_model *model
    ,dlb_sadm_idref idref
    ,dlb_sadm_object *object
    ,dlb_sadm_idref_array *object_refs
    ,dlb_sadm_idref_array *track_uids
    )
{
    dlb_sadm_object *o;
    void *rawp;
    
    (void)model;
    if (idref_unpack(idref, DLB_SADM_OBJECT, &rawp))
    {
        return PMD_FAIL;
    }

    o = (dlb_sadm_object*)rawp;
    memmove(object, o, sizeof(*o));
    object->object_refs = *object_refs;
    object->track_uids = *track_uids;

    if (o->object_refs.num > object->object_refs.max ||
        o->track_uids.num  > object->track_uids.max)
    {
        return PMD_FAIL;
    }

    copy_idref_array(&object->object_refs, &o->object_refs);
    copy_idref_array(&object->track_uids,  &o->track_uids);
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_sadm_object_iterator_init
    (dlb_sadm_object_iterator *it
    ,const dlb_sadm_model *model
    )
{
    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, it);

    it->model = model;
    it->pos   = 0;
    it->count = (unsigned int)model->counts.num_objects;
    return PMD_SUCCESS;
}

  
dlb_pmd_success
dlb_sadm_object_iterator_next
   (dlb_sadm_object_iterator *it
   ,dlb_sadm_object *object
   ,dlb_sadm_idref_array *object_refs
   ,dlb_sadm_idref_array *track_uids
   )
{
    const dlb_sadm_model *model;
    dlb_sadm_object *o;

    if (!it) return PMD_FAIL;
    CHECK_PTRARG(it->model, object);

    if (it->pos < it->count)
    {
        model = it->model;
        o = &model->objects[it->pos++];

        memmove(object, o, sizeof(*o));
        object->object_refs = *object_refs;
        object->track_uids = *track_uids;
        copy_idref_array(&object->object_refs, &o->object_refs);
        copy_idref_array(&object->track_uids, &o->track_uids);
        return PMD_SUCCESS;
    }
    return PMD_FAIL;
}


dlb_pmd_success
dlb_sadm_set_pack_format
    (dlb_sadm_model *model
    ,dlb_sadm_pack_format *p
    ,dlb_sadm_idref *idref
    )
{
    dlb_sadm_pack_format *pptr;
    dlb_sadm_counts *c = &model->counts;
    dlb_pmd_bool is_new = 0;
    
    if (idref_table_lookup(model->irt, p->id.data, DLB_SADM_PACKFMT, NULL, (void**)&pptr))
    {
        /* not already in model, so this is a new allocation */
        if (c->num_packfmts == model->limits.num_packfmts)
        {
            dlb_sadm_set_error(model, "Too many audio pack formats");
            return PMD_FAIL;
        }
        
        pptr = &model->packfmts[c->num_packfmts];
        is_new = 1;
    }

    memmove(&pptr->id, &p->id, sizeof(p->id));
    memmove(&pptr->name, &p->name, sizeof(p->name));
    pptr->type = p->type;
    copy_idref_array(&pptr->chanfmts, &p->chanfmts);
    if (idref_table_insert(model->irt, p->id.data, DLB_SADM_PACKFMT, 0, pptr, idref))
    {
        return PMD_FAIL;
    }
    c->num_packfmts += is_new;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_sadm_pack_format_lookup
    (const dlb_sadm_model *model
    ,dlb_sadm_idref idref
    ,dlb_sadm_pack_format *packfmt
    ,dlb_sadm_idref_array *chanfmts
    )
{
    dlb_sadm_pack_format *pf;
    void *rawp;
    
    (void)model;
    if (idref_unpack(idref, DLB_SADM_PACKFMT, &rawp))
    {
        return PMD_FAIL;
    }

    pf = (dlb_sadm_pack_format*)rawp;
    memmove(packfmt, pf, sizeof(*pf));
    packfmt->chanfmts = *chanfmts;

    if (pf->chanfmts.num > packfmt->chanfmts.max)
    {
        return PMD_FAIL;
    }

    copy_idref_array(&packfmt->chanfmts, &pf->chanfmts);
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_sadm_pack_format_is_common_def
    (const dlb_sadm_model *model
    ,dlb_sadm_pack_format *p
    ,dlb_pmd_bool *is_common_def
    )
{
    dlb_sadm_pack_format *pptr;
    dlb_sadm_idref idref;

    if (model == NULL || p == NULL || is_common_def == NULL)
    {
        return PMD_FAIL;
    }

    if (idref_table_lookup(model->irt, p->id.data, DLB_SADM_PACKFMT, &idref, (void**)&pptr))
    {
        return PMD_FAIL;
    }

    *is_common_def = dlb_sadm_idref_is_common_def(idref);

    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_sadm_pack_format_iterator_init
    (dlb_sadm_pack_format_iterator *it
    ,const dlb_sadm_model *model
    )
{
    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, it);

    it->model = model;
    it->pos   = 0;
    it->count = (unsigned int)model->counts.num_packfmts;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_sadm_pack_format_iterator_next
   (dlb_sadm_pack_format_iterator *it
   ,dlb_sadm_pack_format *packfmt
   ,dlb_sadm_idref_array *chanfmts
   )
{
    const dlb_sadm_model *model;
    dlb_sadm_pack_format *p;

    if (!it) return PMD_FAIL;
    CHECK_PTRARG(it->model, packfmt);
    CHECK_PTRARG(it->model, chanfmts);

    if (it->pos < it->count)
    {
        model = it->model;
        p = &model->packfmts[it->pos++];

        memmove(packfmt, p, sizeof(*p));
        packfmt->chanfmts = *chanfmts;

        if (p->chanfmts.num > packfmt->chanfmts.max)
        {
            return PMD_FAIL;
        }

        copy_idref_array(&packfmt->chanfmts, &p->chanfmts);
        return PMD_SUCCESS;
    }
    return PMD_FAIL;
}


dlb_pmd_success
dlb_sadm_set_channel_format
    (dlb_sadm_model *model
    ,dlb_sadm_channel_format *chanfmt
    ,dlb_sadm_idref *idref
    )
{
    dlb_sadm_channel_format *cptr;
    dlb_sadm_counts *c = &model->counts;
    dlb_pmd_bool is_new = 0;
    
    if (idref_table_lookup(model->irt, chanfmt->id.data, DLB_SADM_CHANFMT, NULL, (void**)&cptr))
    {
        /* not already in model, so this is a new allocation */
        if (c->num_chanfmts == model->limits.num_chanfmts)
        {
            dlb_sadm_set_error(model, "Too many audio channel formats");
            return PMD_FAIL;
        }
        
        cptr = &model->chanfmts[c->num_chanfmts];
        is_new = 1;
    }

    memmove(&cptr->id, &chanfmt->id, sizeof(chanfmt->id));
    memmove(&cptr->name, &chanfmt->name, sizeof(chanfmt->name));
    copy_idref_array(&cptr->blkfmts, &chanfmt->blkfmts);
    if (idref_table_insert(model->irt, chanfmt->id.data, DLB_SADM_CHANFMT, 0, cptr, idref))
    {
        return PMD_FAIL;
    }
    c->num_chanfmts += is_new;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_sadm_channel_format_lookup
    (const dlb_sadm_model *model
    ,dlb_sadm_idref idref
    ,dlb_sadm_channel_format *chanfmt
    ,dlb_sadm_idref_array *blkfmts
    )
{
    dlb_sadm_channel_format *cf;
    void *rawp;
    
    (void)model;
    if (idref_unpack(idref, DLB_SADM_CHANFMT, &rawp))
    {
        return PMD_FAIL;
    }

    cf = (dlb_sadm_channel_format*)rawp;
    memmove(chanfmt, cf, sizeof(*cf));
    chanfmt->blkfmts = *blkfmts;

    if (cf->blkfmts.num > chanfmt->blkfmts.max)
    {
        return PMD_FAIL;
    }

    copy_idref_array(&chanfmt->blkfmts, &cf->blkfmts);
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_sadm_channel_format_is_common_def
    (const dlb_sadm_model *model
    ,dlb_sadm_channel_format *p
    ,dlb_pmd_bool *is_common_def
    )
{
    dlb_sadm_channel_format *pptr;
    dlb_sadm_idref idref;

    if (model == NULL || p == NULL || is_common_def == NULL)
    {
        return PMD_FAIL;
    }

    if (idref_table_lookup(model->irt, p->id.data, DLB_SADM_CHANFMT, &idref, (void**)&pptr))
    {
        return PMD_FAIL;
    }

    *is_common_def = dlb_sadm_idref_is_common_def(idref);

    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_sadm_channel_format_iterator_init
    (dlb_sadm_channel_format_iterator *it
    ,const dlb_sadm_model *model
    )
{
    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, it);

    it->model = model;
    it->pos   = 0;
    it->count = (unsigned int)model->counts.num_chanfmts;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_sadm_channel_format_iterator_next
   (dlb_sadm_channel_format_iterator *it
   ,dlb_sadm_channel_format *chanfmt
   ,dlb_sadm_idref_array *blkfmts
   )
{
    const dlb_sadm_model *model;
    dlb_sadm_channel_format *cf;

    if (!it) return PMD_FAIL;
    CHECK_PTRARG(it->model, chanfmt);
    CHECK_PTRARG(it->model, blkfmts);

    if (it->pos < it->count)
    {
        model = it->model;
        cf = &model->chanfmts[it->pos++];

        memmove(chanfmt, cf, sizeof(*cf));
        chanfmt->blkfmts = *blkfmts;

        if (cf->blkfmts.num > chanfmt->blkfmts.max)
        {
            return PMD_FAIL;
        }

        copy_idref_array(&chanfmt->blkfmts, &cf->blkfmts);
        return PMD_SUCCESS;
    }
    return PMD_FAIL;
}


dlb_pmd_success
dlb_sadm_set_block_format
    (dlb_sadm_model *model
    ,dlb_sadm_block_format *b
    ,dlb_sadm_idref *idref
    )
{
    dlb_sadm_block_format *bptr;
    dlb_sadm_counts *c = &model->counts;
    dlb_pmd_bool is_new = 0;
    
    if (idref_table_lookup(model->irt, b->id.data, DLB_SADM_BLOCKFMT, NULL, (void**)&bptr))
    {
        /* not already in model, so this is a new allocation */
        if (c->num_blkfmts == model->limits.num_blkfmts)
        {
            dlb_sadm_set_error(model, "Too many audio block formats");
            return PMD_FAIL;
        }
    
        bptr = &model->blkfmts[c->num_blkfmts];
        is_new = 1;
    }

    memmove(bptr, b, sizeof(*b));
    if (idref_table_insert(model->irt, b->id.data, DLB_SADM_BLOCKFMT, 0, bptr, idref))
    {
        return PMD_FAIL;
    }

    c->num_blkfmts += is_new;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_sadm_block_format_lookup
    (const dlb_sadm_model *model
    ,dlb_sadm_idref idref
    ,dlb_sadm_block_format *blkfmt
    )
{
    void *rawp;
    
    (void)model;
    if (idref_unpack(idref, DLB_SADM_BLOCKFMT, &rawp))
    {
        return PMD_FAIL;
    }
    memmove(blkfmt, rawp, sizeof(*blkfmt));
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_sadm_set_track_uid
    (dlb_sadm_model *model
    ,dlb_sadm_track_uid *u
    ,dlb_sadm_idref *idref
    )
{
    dlb_sadm_track_uid *uptr;
    dlb_sadm_counts *c = &model->counts;
    dlb_pmd_bool is_new = 0;
    
    if (idref_table_lookup(model->irt, u->id.data, DLB_SADM_TRACKUID, NULL, (void**)&uptr))
    {
        /* not already in model, so this is a new allocation */
        if (c->num_track_uids == model->limits.num_track_uids)
        {
            dlb_sadm_set_error(model, "Too many audio track UIDs");
            return PMD_FAIL;
        }
    
        uptr = &model->track_uids[c->num_track_uids];
        is_new = 1;
    }

    memmove(uptr, u, sizeof(*u));
    if (idref_table_insert(model->irt, u->id.data, DLB_SADM_TRACKUID, 0, uptr, idref))
    {
        return PMD_FAIL;
    }
    c->num_track_uids += is_new;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_sadm_track_uid_lookup
    (const dlb_sadm_model *model
    ,dlb_sadm_idref idref
    ,dlb_sadm_track_uid *u
    )
{
    void *rawp;
    
    (void)model;
    if (idref_unpack(idref, DLB_SADM_TRACKUID, &rawp))
    {
        return PMD_FAIL;
    }
    if (u)
    {
        if (rawp == FORWARD_REFERENCE)
        {
            memset(u, 0, sizeof(*u));
            return PMD_FAIL;
        }
        else
        {
            memmove(u, rawp, sizeof(*u));
        }
    }

    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_sadm_track_uid_is_common_def
    (const dlb_sadm_model *model
    ,dlb_sadm_track_uid *p
    ,dlb_pmd_bool *is_common_def
    )
{
    dlb_sadm_track_uid *pptr;
    dlb_sadm_idref idref;

    if (model == NULL || p == NULL || is_common_def == NULL)
    {
        return PMD_FAIL;
    }

    if (idref_table_lookup(model->irt, p->id.data, DLB_SADM_TRACKUID, &idref, (void**)&pptr))
    {
        return PMD_FAIL;
    }

    *is_common_def = dlb_sadm_idref_is_common_def(idref);

    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_sadm_track_uid_iterator_init
    (dlb_sadm_track_uid_iterator *it
    ,const dlb_sadm_model *model
    )
{
    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, it);

    it->model = model;
    it->pos   = 0;
    it->count = (unsigned int)model->counts.num_track_uids;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_sadm_track_uid_iterator_next
   (dlb_sadm_track_uid_iterator *it
   ,dlb_sadm_track_uid *track
   )
{
    const dlb_sadm_model *model;
    dlb_sadm_track_uid *u;

    if (!it) return PMD_FAIL;
    CHECK_PTRARG(it->model, track);

    if (it->pos < it->count)
    {
        model = it->model;
        u = &model->track_uids[it->pos++];

        memmove(track, u, sizeof(*u));
        return PMD_SUCCESS;
    }
    return PMD_FAIL;
}

static
dlb_pmd_bool
validate_uuid_string
    (const char *s
    )
{
    size_t i, j, k;

    for (i = 0; i < 8; i++)
    {
        if (!isxdigit(s[i]))
        {
            return PMD_FALSE;
        }
    }

    if (s[i++] != '-')
    {
        return PMD_FALSE;
    }

    for (k = 0; k < 3; k++)
    {
        for (j = 0; j < 4; j++)
        {
            if (!isxdigit(s[i++]))
            {
                return PMD_FALSE;
            }
        }

        if (s[i++] != '-')
        {
            return PMD_FALSE;
        }
    }

    for (j = 0; j < 12; j++)
    {
        if (!isxdigit(s[i++]))
        {
            return PMD_FALSE;
        }
    }

    return PMD_TRUE;
}

dlb_pmd_success dlb_sadm_set_flow_id(dlb_sadm_model *model, const char *uuid, size_t sz)
{
    dlb_pmd_success success = PMD_FAIL;

    if (model != NULL)
    {
        if (uuid == NULL || uuid[0] == '\0')
        {
            memset(model->flow_id, 0, sizeof(model->flow_id));
            success = PMD_SUCCESS;
        }
        else if (uuid != NULL && sz >= FLOW_ID_LEN && validate_uuid_string(uuid))
        {
            strncpy(model->flow_id, uuid, FLOW_ID_LEN);
            model->flow_id[FLOW_ID_LEN] = '\0';
            success = PMD_SUCCESS;
        }
    }

    return success;
}

dlb_pmd_success dlb_sadm_get_flow_id(const dlb_sadm_model *model, char *uuid, size_t sz)
{
    dlb_pmd_success success = PMD_FAIL;

    if (model != NULL && uuid != NULL && sz >= sizeof(model->flow_id))
    {
        memcpy(uuid, model->flow_id, sizeof(model->flow_id));
        success = PMD_SUCCESS;
    }

    return success;
}


const char *
dlb_sadm_error
    (const dlb_sadm_model *model
    )
{
    return model->error;
}


dlb_pmd_success
dlb_sadm_lookup_reference
    (const dlb_sadm_model *model
    ,const unsigned char *id
    ,dlb_sadm_idref_type type
    ,unsigned int lineno
    ,dlb_sadm_idref *ref
    )
{
    return idref_table_insert(model->irt, id, type, lineno, NULL, ref);
}


dlb_pmd_success
dlb_sadm_idref_defined
    (dlb_sadm_idref *ref
    )
{
    return idref_defined(ref);
}


size_t
dlb_sadm_get_undefined_references
    (const dlb_sadm_model *m
    ,dlb_sadm_undefined_ref *undef
    ,size_t capacity
    )
{
    return idref_table_get_undefined_references(m->irt, undef, capacity);
}


