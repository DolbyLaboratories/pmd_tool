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
