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

#include "sadm/dlb_sadm_reader.h"
#include "dlb_pmd_sadm.h"
#include "pmd_sadm_ingester.h"
#include "pmd_error_helper.h"
#include "sadm/memstuff.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


struct dlb_pmd_sadm_reader
{
    dlb_pmd_model *pmd;
    dlb_sadm_model *sadm;
    dlb_sadm_reader *sreader;
    pmd_sadm_ingester *ing;
};


/** ------------------------------ public API ------------------------- */


dlb_pmd_bool
dlb_xmlpmd_is_sadm
    (const char *buffer
    ,size_t length
    )
{
    return dlb_sadm_reader_check_xml(buffer, length);
}


size_t
dlb_pmd_sadm_reader_query_mem
    (dlb_pmd_model_constraints *limits
    )
{
    dlb_sadm_counts sc;
    size_t sz;
    
    compute_sadm_limits(limits, &sc);
    
    sz =  ALIGN_TO_MPTR(sizeof(dlb_pmd_sadm_reader))
        + ALIGN_TO_MPTR(dlb_sadm_query_memory(&sc))
        + ALIGN_TO_MPTR(dlb_sadm_reader_query_memory(&sc))
        + ALIGN_TO_MPTR(pmd_sadm_ingester_query_mem(&sc));

    return sz;
}


dlb_pmd_success
dlb_pmd_sadm_reader_init
    (dlb_pmd_sadm_reader **rdr
    ,dlb_pmd_model_constraints *limits
    ,void *mem
    )
{
    uintptr_t mc = (uintptr_t)mem;
    dlb_pmd_sadm_reader *r;
    dlb_sadm_counts sc;

    r = (dlb_pmd_sadm_reader*)mc;
    mc += ALIGN_TO_MPTR(sizeof(dlb_pmd_sadm_reader));

    compute_sadm_limits(limits, &sc);
    if (dlb_sadm_init(&sc, (void*)mc, &r->sadm))
    {
        return PMD_FAIL;
    }
    mc += ALIGN_TO_MPTR(dlb_sadm_query_memory(&sc));

    if (dlb_sadm_reader_init(&sc, (void*)mc, &r->sreader))
    {
        return PMD_FAIL;
    }
    mc += ALIGN_TO_MPTR(dlb_sadm_reader_query_memory(&sc));

    if (pmd_sadm_ingester_init(&r->ing, (void*)mc, r->sadm))
    {
        return PMD_FAIL;
    }
    *rdr = r;
    return PMD_SUCCESS;
}


void
dlb_pmd_sadm_reader_finish
    (dlb_pmd_sadm_reader *rdr
    )
{
    pmd_sadm_ingester_finish(rdr->ing);
    dlb_sadm_reader_finish(rdr->sreader);
    dlb_sadm_finish(rdr->sadm);
}


dlb_pmd_success
dlb_pmd_sadm_reader_read
    (dlb_pmd_sadm_reader *rdr
    ,dlb_pmd_model *model
    ,const char *title
    ,dlb_xmlpmd_line_callback lcb
    ,dlb_xmlpmd_error_callback ecb
    ,void *cbarg
    )
{
    rdr->pmd = model;

    dlb_sadm_reinit(rdr->sadm);

    if (dlb_sadm_reader_read(rdr->sreader, lcb, ecb, cbarg, rdr->sadm))
    {
        error(rdr->pmd, "sADM read error: %s", dlb_sadm_error(rdr->sadm));
        return PMD_FAIL;
    }

    if (pmd_sadm_ingester_ingest(rdr->ing, title, model))
    {
        error(rdr->pmd, "sADM ingest error: %s", dlb_sadm_error(rdr->sadm));
        return PMD_FAIL;
    }
    return PMD_SUCCESS;
}


const dlb_sadm_model *
dlb_pmd_sadm_reader_get_sadm_model
    (dlb_pmd_sadm_reader *rdr
    )
{
    const dlb_sadm_model *p = NULL;

    if (rdr != NULL)
    {
        p = rdr->sadm;
    }

    return p;
}

