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

#include "sadm/dlb_sadm_model.h"
#include "sadm/dlb_sadm_writer.h"
#include "dlb_pmd_sadm.h"
#include "sadm/memstuff.h"
#include "pmd_sadm_generator.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/**
 * @brief implementation of the abstract dlb_pmd_sadm_writer type
 */
struct dlb_pmd_sadm_writer
{
    const dlb_pmd_model *pmd;
    dlb_sadm_model *sadm;
    pmd_sadm_generator *gen;
};


/** ------------------------- public api ------------------------------ */


size_t
dlb_pmd_sadm_writer_query_mem
    (dlb_pmd_model_constraints *limits
    )
{
    dlb_sadm_counts sc;
    size_t sz;

    compute_sadm_limits(limits, &sc);

    sz =  ALIGN_TO_MPTR(sizeof(dlb_pmd_sadm_writer))
        + ALIGN_TO_MPTR(dlb_sadm_query_memory(&sc))
        + ALIGN_TO_MPTR(pmd_sadm_generator_query_mem());
    
    return sz;
}


dlb_pmd_success
dlb_pmd_sadm_writer_init
    (dlb_pmd_sadm_writer **wptr
    ,dlb_pmd_model_constraints *limits
    ,void *mem
    )
{
    uintptr_t mc = (uintptr_t)mem;
    dlb_pmd_sadm_writer *w;
    dlb_sadm_counts sc;
    
    w = (dlb_pmd_sadm_writer *)mc;
    mc += ALIGN_TO_MPTR(sizeof(dlb_pmd_sadm_writer));

    compute_sadm_limits(limits, &sc);
    if (dlb_sadm_init(&sc, (void*)mc, &w->sadm))
    {
        return PMD_FAIL;
    }
    mc += ALIGN_TO_MPTR(dlb_sadm_query_memory(&sc));
    
    if (pmd_sadm_generator_init((void*)mc, &w->gen))
    {
        return PMD_FAIL;
    }

    *wptr = w;
    return PMD_SUCCESS;
}


void
dlb_pmd_sadm_writer_finish
    (dlb_pmd_sadm_writer *w
    )
{
    pmd_sadm_generator_finish(w->gen);
    dlb_sadm_finish(w->sadm);
}


dlb_pmd_success
dlb_pmd_sadm_writer_write
   (dlb_pmd_sadm_writer *w
   ,const dlb_pmd_model *model
   ,dlb_xmlpmd_get_buffer gb
   ,unsigned int indent
   ,void *cbarg
   )
{
    w->pmd = model;

    return pmd_sadm_generator_generate(w->gen, model, w->sadm)
        || dlb_sadm_write(gb, indent, cbarg, w->sadm);
}


const dlb_sadm_model *
dlb_pmd_sadm_writer_get_sadm_model
   (dlb_pmd_sadm_writer *w
    )
{
    const dlb_sadm_model *p = NULL;

    if (w != NULL)
    {
        p = w->sadm;
    }

    return p;
}
