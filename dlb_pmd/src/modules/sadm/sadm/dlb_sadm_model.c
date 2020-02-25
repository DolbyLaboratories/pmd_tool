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
 * @file dlb_sadm_model.c
 * @brief internal data structure to store Dolby-constrained Serial ADM data
 */


#include "sadm/dlb_sadm_model.h"
#include "dlb_pmd_api.h"
#include "pmd_model.h"
#include "sadm_error_helper.h"
#include "memstuff.h"
#include "idrefs.h"
#include <stdarg.h>


#if defined(_MSC_VER)
#  if _MSC_VER < 1900 && !defined(inline)
#    define inline __inline
#  endif
#endif


struct dlb_sadm_model
{
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
    dlb_sadm_track_uid        *track_uids;
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
    limits->max_programme_labels   =  6;
    limits->max_programme_contents = 32;
    limits->max_object_track_uids  = 16;
    limits->max_packfmt_chanfmts   = 16;
    limits->max_chanfmt_blkfmts    =  1;

    limits->num_programmes         = 10;
    limits->num_contents           = 10;
    limits->num_objects            =  8;
    limits->num_packfmts           =  6;
    limits->num_chanfmts           = 24;
    limits->num_blkfmts            = limits->num_chanfmts * limits->max_chanfmt_blkfmts;
    limits->num_track_uids         = 16;
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
        +  MEMREQ(dlb_sadm_idref,           c->max_object_track_uids) * c->num_objects
        +  MEMREQ(dlb_sadm_idref,           c->max_packfmt_chanfmts) * c->num_packfmts
        +  MEMREQ(dlb_sadm_idref,           c->max_chanfmt_blkfmts) * c->num_chanfmts
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
    dlb_sadm_object *obj;
    dlb_sadm_pack_format *packfmt;
    dlb_sadm_channel_format *chanfmt;
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

    /* now add pack format and track_uid idref arrays to objects */
    obj = m->objects;
    for (i = 0; i != c->num_objects; ++i, ++obj)
    {
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

    return idref_table_init(c, (void*)rp, &m->irt);
}


void
dlb_sadm_finish
    (dlb_sadm_model *m
    )
{
    idref_table_finish(m->irt);
}


void
dlb_sadm_reinit
    (dlb_sadm_model *m
    )
{
    memset(&m->counts, '\0', sizeof(m->counts));
    idref_table_reinit(m->irt);
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
dlb_sadm_model_limits
    (const dlb_sadm_model *model
    ,dlb_sadm_counts* limits
    )
{
    memmove(limits, &model->limits, sizeof(*limits));
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_sadm_model_counts
    (const dlb_sadm_model *model
    ,dlb_sadm_counts*counts
    )
{
    memmove(counts, &model->counts, sizeof(*counts));
    return PMD_SUCCESS;
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
    
    if (idref_table_lookup(model->irt, p->id.data, DLB_SADM_PROGRAMME, (void**)&pptr))
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
    
    if (idref_table_lookup(model->irt, content->id.data, DLB_SADM_CONTENT, (void**)&cptr))
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
    
    memmove(cptr, content, sizeof(*content));
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
    ,dlb_sadm_content *c
    )
{
    void *rawp;

    (void)model;    
    if (idref_unpack(idref, DLB_SADM_CONTENT, &rawp))
    {
        return PMD_FAIL;
    }
    memmove(c, rawp, sizeof(*c));
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
    
    if (idref_table_lookup(model->irt, o->id.data, DLB_SADM_OBJECT, (void**)&optr))
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
    optr->pack_format = o->pack_format;
    optr->gain = o->gain;
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
    object->track_uids = *track_uids;

    if (o->track_uids.num > object->track_uids.max)
    {
        return PMD_FAIL;
    }

    copy_idref_array(&object->track_uids, &o->track_uids);
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
        object->track_uids = *track_uids;
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
    
    if (idref_table_lookup(model->irt, p->id.data, DLB_SADM_PACKFMT, (void**)&pptr))
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
    
    if (idref_table_lookup(model->irt, chanfmt->id.data, DLB_SADM_CHANFMT, (void**)&cptr))
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
    
    if (idref_table_lookup(model->irt, b->id.data, DLB_SADM_BLOCKFMT, (void**)&bptr))
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
    
    if (idref_table_lookup(model->irt, u->id.data, DLB_SADM_TRACKUID, (void**)&uptr))
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



