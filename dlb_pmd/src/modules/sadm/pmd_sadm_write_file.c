/************************************************************************
 * dlb_pmd
 * Copyright (c) 2021, Dolby Laboratories Inc.
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


#include "dlb_pmd_sadm_file.h"
#include "sadm/dlb_sadm_file.h"
#include "sadm/dlb_sadm_model.h"
#include "pmd_sadm_generator.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/** ------------------------------ public API ------------------------- */


dlb_pmd_success
dlb_pmd_sadm_file_write
   (const char    *filename
   ,const dlb_pmd_model *model
   )
{
    dlb_pmd_model_constraints limits;
    dlb_pmd_success res = PMD_FAIL;
    dlb_sadm_counts sc;
    dlb_sadm_model *sm;
    pmd_sadm_generator *generator;
    size_t sz;
    void *smem;
    void *cmem;

    dlb_pmd_get_constraints(model, &limits);
    compute_sadm_limits(&limits, &sc);
    sz = dlb_sadm_query_memory(&sc);
    smem = malloc(sz);
    if (NULL == smem)                                        goto done0;
    if (dlb_sadm_init(&sc, (void*)smem, &sm))                goto done1;

    sz = pmd_sadm_generator_query_mem();
    cmem = malloc(sz);
    if (NULL == cmem)                                        goto done2;
    if (pmd_sadm_generator_init(cmem, &generator))           goto done3;
    if (pmd_sadm_generator_generate(generator, model, sm))   goto done4;
    if (dlb_sadm_file_write(filename, sm))                   goto done4;

    res = PMD_SUCCESS;

  done4: pmd_sadm_generator_finish(generator);
  done3: free(cmem);
  done2: dlb_sadm_finish(sm);
  done1: free(smem);
  done0: return res;
}

