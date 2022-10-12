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

#include "dlb_pmd_model_combo.h"
#include "pmd_model.h"
#include "pmd_apn.h"
#include "dlb_pmd_api.h"
#include "dlb_pmd/src/modules/sadm/pmd_core_model_ingester.h"
#include "dlb_pmd/src/modules/sadm/pmd_core_model_generator.h"
#include "dlb_adm/include/dlb_adm_api.h"

#include <string.h>

#ifdef NDEBUG
#define FAILURE PMD_FAIL
#else
static dlb_pmd_success ret_fail()
{
    return PMD_FAIL;    // Put a breakpoint here
}
#define FAILURE ret_fail()
#endif

#define CHECK_STATUS_SUCCESS(s) if ((s) != DLB_ADM_STATUS_OK) return FAILURE
#define CHECK_SUCCESS(s)        if ((s) != PMD_SUCCESS) return FAILURE

/* Possibly this should be moved into pmd_model.[hc] */
static
void
dlb_pmd_reset_write_state
    (dlb_pmd_model  *model
    )
{
    pmd_apn_list_iterator_init(&model->write_state.apni, &model->apn_list);
}

typedef enum
{
    COMBO_STATE_UNKNOWN,
    COMBO_STATE_PMD_MODEL_PRIMARY,
    COMBO_STATE_CORE_MODEL_PRIMARY
} COMBO_STATE;

struct dlb_pmd_model_combo
{
    dlb_pmd_model       *pmd_model;
    dlb_adm_core_model  *core_model;

    COMBO_STATE          combo_state;
    dlb_pmd_bool         existing_pmd_model;
    dlb_pmd_bool         existing_core_model;
    dlb_pmd_bool         use_adm_common_defs;

    dlb_pmd_bool         mallocated;
    void                *combo_model_memory;
    void                *converter_memory;
    void                *pmd_model_memory;
};

static
dlb_pmd_success
get_sizes
    (dlb_pmd_model      *existing_pmd_model
    ,dlb_adm_core_model *existing_core_model
    ,size_t             *converter_sz
    ,size_t             *pmd_model_sz
    )
{
    size_t sz1;
    size_t sz2;

    (void)existing_core_model;  /* not relevant in this version */

    *converter_sz = 0;
    *pmd_model_sz = 0;

    if (pmd_core_model_generator_query_memory_size(&sz1) ||
        pmd_core_model_ingester_query_memory_size(&sz2))
    {
        return FAILURE;
    }
    else
    {
        if (sz1 > sz2)
        {
            *converter_sz = sz1;
        }
        else
        {
            *converter_sz = sz2;
        }
    }

    if (existing_pmd_model == NULL)
    {
        *pmd_model_sz = dlb_pmd_query_mem();
    }

    return PMD_SUCCESS;
}

size_t
dlb_pmd_model_combo_query_mem
    (dlb_pmd_model          *existing_pmd_model
    ,dlb_adm_core_model     *existing_core_model
    )
{
    size_t model_combo_sz = sizeof(dlb_pmd_model_combo);
    size_t converter_sz;
    size_t pmd_model_sz;
    size_t total_sz;

    if (get_sizes(existing_pmd_model, existing_core_model, &converter_sz, &pmd_model_sz))
    {
        total_sz = 0;
    } 
    else
    {
        total_sz = converter_sz + pmd_model_sz + model_combo_sz;
    }

    return total_sz;
}

static
dlb_pmd_success
pmd_model_is_empty
    (dlb_pmd_model  *pmd_model
    ,dlb_pmd_bool   *is_empty
    )
{
    dlb_pmd_metadata_count counts;
    dlb_pmd_bool empty = PMD_TRUE;
    dlb_pmd_success success;

    if ((pmd_model == NULL) || (is_empty == NULL))
    {
        return FAILURE;
    }

    success = dlb_pmd_count_entities(pmd_model, &counts);
    CHECK_SUCCESS(success);
    if ((counts.num_signals       > 0) ||
        (counts.num_beds          > 0) ||
        (counts.num_objects       > 0) ||
        (counts.num_updates       > 0) ||
        (counts.num_presentations > 0)
        /* we could continue, but that is probably sufficient... */
        )
    {
        empty = PMD_FALSE;
    }

    *is_empty = empty;

    return PMD_SUCCESS;
}

dlb_pmd_success
dlb_pmd_model_combo_init
    (dlb_pmd_model_combo   **model_combo
    ,dlb_pmd_model          *existing_pmd_model
    ,dlb_adm_core_model     *existing_core_model
    ,dlb_pmd_bool            use_adm_common_defs
    ,void                   *memory
    )
{
    dlb_pmd_model_combo *mc;
    dlb_pmd_bool         mallocate = (memory == NULL);
    size_t               model_combo_sz = sizeof(dlb_pmd_model_combo);
    size_t               converter_sz;
    size_t               pmd_model_sz;
    uint8_t             *p;

    if (model_combo == NULL)
    {
        return FAILURE;
    }
    *model_combo = NULL;

    if (get_sizes(existing_pmd_model, existing_core_model, &converter_sz, &pmd_model_sz))
    {
        return FAILURE;
    }

    if (mallocate)
    {
        size_t total_sz = model_combo_sz + converter_sz + pmd_model_sz;

        memory = malloc(total_sz);
        if (memory == NULL)
        {
            return FAILURE;
        }
    }

    mc = (dlb_pmd_model_combo *)memory;
    memset(mc, 0, model_combo_sz);
    p = ((uint8_t *)mc) + model_combo_sz;
    mc->converter_memory = p;
    p += converter_sz;

    mc->mallocated = mallocate;
    mc->combo_model_memory = memory;
    mc->use_adm_common_defs = use_adm_common_defs;

    if (existing_pmd_model == NULL)
    {
        mc->pmd_model_memory = p;
        dlb_pmd_init(&mc->pmd_model, mc->pmd_model_memory);
    }
    else
    {
        mc->pmd_model = existing_pmd_model;
        mc->existing_pmd_model = PMD_TRUE;
    }

    if (existing_core_model == NULL)
    {
        dlb_adm_core_model_counts counts;
        int status;

        memset(&counts, 0, sizeof(counts));
        status = dlb_adm_core_model_open(&mc->core_model, &counts);
        if (status != DLB_ADM_STATUS_OK)
        {
            return FAILURE;
        }
    }
    else
    {
        mc->core_model = existing_core_model;
        mc->existing_core_model = PMD_TRUE;
    }

    if ((mc->existing_pmd_model) || (mc->existing_core_model))
    {
        dlb_pmd_bool     pmd_model_has_content;
        dlb_pmd_bool     core_model_has_content;
        dlb_pmd_success  success;

        success = dlb_pmd_model_combo_has_content(mc, &pmd_model_has_content, &core_model_has_content);
        CHECK_SUCCESS(success);
        if ((pmd_model_has_content ? 1 : 0) ^ (core_model_has_content ? 1 : 0)) /* one or the other, but not both, nor neither */
        {
            if (mc->existing_pmd_model && pmd_model_has_content)
            {
                mc->combo_state = COMBO_STATE_PMD_MODEL_PRIMARY;
            }
            else if (mc->existing_core_model && core_model_has_content)
            {
                mc->combo_state = COMBO_STATE_CORE_MODEL_PRIMARY;
            }
        }
    }

    *model_combo = mc;

    return PMD_SUCCESS;
}

dlb_pmd_success
dlb_pmd_model_combo_get_readable_pmd_model
    (const dlb_pmd_model_combo  *model_combo
    ,const dlb_pmd_model       **pmd_model
    ,dlb_pmd_bool                reset_write_state
    )
{
    if (model_combo == NULL || pmd_model == NULL)
    {
        return FAILURE;
    }

    if (reset_write_state)
    {
        dlb_pmd_reset_write_state(model_combo->pmd_model);
    }

    *pmd_model = model_combo->pmd_model;

    return PMD_SUCCESS;
}

dlb_pmd_success
dlb_pmd_model_combo_ensure_readable_pmd_model
    (dlb_pmd_model_combo    *model_combo
    ,const dlb_pmd_model   **pmd_model
    ,dlb_pmd_bool            reset_write_state
    )
{
    DLB_PMD_MODEL_COMBO_STATE pmd_model_state;
    DLB_PMD_MODEL_COMBO_STATE core_model_state;
    dlb_pmd_bool convert = PMD_FALSE;
    dlb_pmd_success success;

    success = dlb_pmd_model_combo_get_state(model_combo, &pmd_model_state, &core_model_state);
    CHECK_SUCCESS(success);

    switch (pmd_model_state)
    {
    case DLB_PMD_MODEL_COMBO_STATE_IS_PRIMARY:
    case DLB_PMD_MODEL_COMBO_STATE_IS_CONVERTED:
    case DLB_PMD_MODEL_COMBO_STATE_HAS_CONTENT:
        success = dlb_pmd_model_combo_get_readable_pmd_model(model_combo, pmd_model, reset_write_state);
        CHECK_SUCCESS(success);
        break;

    case DLB_PMD_MODEL_COMBO_STATE_IS_EMPTY:
        switch (core_model_state)
        {
        case DLB_PMD_MODEL_COMBO_STATE_IS_PRIMARY:
        case DLB_PMD_MODEL_COMBO_STATE_HAS_CONTENT:
            convert = PMD_TRUE;
            break;

        case DLB_PMD_MODEL_COMBO_STATE_IS_EMPTY:
            success = dlb_pmd_model_combo_get_readable_pmd_model(model_combo, pmd_model, reset_write_state);
            CHECK_SUCCESS(success);
            break;

        default:
            return FAILURE;
        }
        break;

    default:
        return FAILURE;
    }

    if (convert)
    {
        success = dlb_pmd_model_combo_convert_to_pmd_model(model_combo, "Converted from Serial ADM", pmd_model);
        CHECK_SUCCESS(success);
    }

    return success;
}

dlb_pmd_success
dlb_pmd_model_combo_get_writable_pmd_model
    (dlb_pmd_model_combo    *model_combo
    ,dlb_pmd_model         **pmd_model
    ,dlb_pmd_bool            reset_write_state
    )
{
    int status;

    if ((model_combo == NULL) || (pmd_model == NULL))
    {
        return FAILURE;
    }
    *pmd_model = NULL;

    switch (model_combo->combo_state)
    {
    case COMBO_STATE_UNKNOWN:
        model_combo->combo_state = COMBO_STATE_PMD_MODEL_PRIMARY;
        break;

    case COMBO_STATE_PMD_MODEL_PRIMARY:
        break;

    case COMBO_STATE_CORE_MODEL_PRIMARY:
        {
            pmd_core_model_ingester *ingester;
            status = pmd_core_model_ingester_open(&ingester, model_combo->converter_memory);
            CHECK_STATUS_SUCCESS(status);
            /* the call to ingest() clears the PMD model */
            status = pmd_core_model_ingester_ingest(ingester, model_combo->pmd_model, "Converted from Serial ADM", model_combo->core_model);
            CHECK_STATUS_SUCCESS(status);
            status = pmd_core_model_ingester_close(&ingester);
            CHECK_STATUS_SUCCESS(status);

            reset_write_state = PMD_TRUE;
            
            model_combo->existing_pmd_model = PMD_TRUE;
            model_combo->combo_state = COMBO_STATE_PMD_MODEL_PRIMARY;
        }
        break;
                    
    default:
        return FAILURE;
    }

    status = dlb_adm_core_model_clear(model_combo->core_model);
    CHECK_STATUS_SUCCESS(status);
    if (reset_write_state)
    {
        dlb_pmd_reset_write_state(model_combo->pmd_model);
    }

    *pmd_model = model_combo->pmd_model;

    return PMD_SUCCESS;
}

dlb_pmd_success
dlb_pmd_model_combo_get_readable_core_model
    (const dlb_pmd_model_combo  *model_combo
    ,const dlb_adm_core_model  **core_model
    )
{
    if (model_combo == NULL || core_model == NULL)
    {
        return FAILURE;
    }
    *core_model = model_combo->core_model;

    return PMD_SUCCESS;
}

dlb_pmd_success
dlb_pmd_model_combo_ensure_readable_core_model
    (dlb_pmd_model_combo        *model_combo
    ,const dlb_adm_core_model  **core_model
    )
{
    DLB_PMD_MODEL_COMBO_STATE pmd_model_state;
    DLB_PMD_MODEL_COMBO_STATE core_model_state;
    dlb_pmd_bool convert = PMD_FALSE;
    dlb_pmd_success success;

    success = dlb_pmd_model_combo_get_state(model_combo, &pmd_model_state, &core_model_state);
    CHECK_SUCCESS(success);
    switch (core_model_state)
    {
    case DLB_PMD_MODEL_COMBO_STATE_IS_PRIMARY:
    case DLB_PMD_MODEL_COMBO_STATE_IS_CONVERTED:
    case DLB_PMD_MODEL_COMBO_STATE_HAS_CONTENT:
        success = dlb_pmd_model_combo_get_readable_core_model(model_combo, core_model);
        CHECK_SUCCESS(success);
        break;

    case DLB_PMD_MODEL_COMBO_STATE_IS_EMPTY:
        switch (pmd_model_state)
        {
        case DLB_PMD_MODEL_COMBO_STATE_IS_PRIMARY:
        case DLB_PMD_MODEL_COMBO_STATE_HAS_CONTENT:
            convert = PMD_TRUE;
            break;

        case DLB_PMD_MODEL_COMBO_STATE_IS_EMPTY:
            success = dlb_pmd_model_combo_get_readable_core_model(model_combo, core_model);
            CHECK_SUCCESS(success);
            break;

        default:
            return FAILURE;
        }
        break;

    default:
        return FAILURE;
    }

    if (convert)
    {
        success = dlb_pmd_model_combo_convert_to_core_model(model_combo, core_model);
        CHECK_SUCCESS(success);
    }

    return success;
}

dlb_pmd_success
dlb_pmd_model_combo_get_writable_core_model
    (dlb_pmd_model_combo        *model_combo
    ,dlb_adm_core_model        **core_model
    )
{
    dlb_pmd_success success;

    if (model_combo == NULL || core_model == NULL)
    {
        return FAILURE;
    }
    *core_model = NULL;

    switch (model_combo->combo_state)
    {
    case COMBO_STATE_UNKNOWN:
        model_combo->combo_state = COMBO_STATE_CORE_MODEL_PRIMARY;
        break;

    case COMBO_STATE_CORE_MODEL_PRIMARY:
        break;

    default:
        return FAILURE;
    }

    success = dlb_pmd_reset(model_combo->pmd_model);
    CHECK_SUCCESS(success);

    *core_model = model_combo->core_model;

    return PMD_SUCCESS;
}

dlb_pmd_success
dlb_pmd_model_combo_convert_to_pmd_model
    (dlb_pmd_model_combo        *model_combo
    ,const char                 *title
    ,const dlb_pmd_model       **pmd_model
    )
{
    pmd_core_model_ingester *ingester;
    dlb_pmd_success success;

    if ((model_combo == NULL) || (pmd_model == NULL))
    {
        return FAILURE;
    }
    *pmd_model = NULL;

    switch (model_combo->combo_state)
    {
    case COMBO_STATE_UNKNOWN:
        model_combo->combo_state = COMBO_STATE_CORE_MODEL_PRIMARY;
        break;

    case COMBO_STATE_CORE_MODEL_PRIMARY:
        break;

    default:
        return FAILURE;
    }

    if (title == NULL)
    {
        title = "Converted from Serial ADM";
    }

    success = pmd_core_model_ingester_open(&ingester, model_combo->converter_memory);
    CHECK_SUCCESS(success);
    /* the call to ingest() clears the PMD model */
    success = pmd_core_model_ingester_ingest(ingester, model_combo->pmd_model, title, model_combo->core_model);
    CHECK_SUCCESS(success);
    success = pmd_core_model_ingester_close(&ingester);
    CHECK_SUCCESS(success);

    dlb_pmd_reset_write_state(model_combo->pmd_model);

    *pmd_model = model_combo->pmd_model;

    return success;
}

dlb_pmd_success
dlb_pmd_model_combo_convert_to_core_model
    (dlb_pmd_model_combo        *model_combo
    ,const dlb_adm_core_model  **core_model
    )
{
    pmd_core_model_generator *generator = NULL;
    dlb_adm_xml_container    *container = NULL;
    dlb_adm_container_counts  counts;
    dlb_pmd_success success;
    int status;

    if ((model_combo == NULL) || (core_model == NULL))
    {
        return FAILURE;
    }
    *core_model = NULL;

    switch (model_combo->combo_state)
    {
    case COMBO_STATE_UNKNOWN:
        model_combo->combo_state = COMBO_STATE_PMD_MODEL_PRIMARY;
        break;

    case COMBO_STATE_PMD_MODEL_PRIMARY:
        break;

    default:
        return FAILURE;
    }

    status = dlb_adm_core_model_clear(model_combo->core_model);
    CHECK_STATUS_SUCCESS(status);
    status = dlb_adm_core_model_add_profile(model_combo->core_model, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE);
    CHECK_STATUS_SUCCESS(status);
    status = dlb_adm_container_open(&container, &counts);
    CHECK_STATUS_SUCCESS(status);
    status = dlb_adm_container_load_common_definitions(container);
    CHECK_STATUS_SUCCESS(status);
    status = dlb_adm_core_model_ingest_common_definitions_container(model_combo->core_model, container);
    CHECK_STATUS_SUCCESS(status);

    success = pmd_core_model_generator_open(&generator, model_combo->converter_memory);
    CHECK_SUCCESS(success);
    success = pmd_core_model_generator_generate(generator, model_combo->core_model, model_combo->pmd_model);
    CHECK_SUCCESS(success);
    success = pmd_core_model_generator_close(&generator);
    CHECK_SUCCESS(success);

    *core_model = model_combo->core_model;

    return success;
}

dlb_pmd_success
dlb_pmd_model_combo_has_content
    (dlb_pmd_model_combo    *model_combo
    ,dlb_pmd_bool           *pmd_model_has_content
    ,dlb_pmd_bool           *core_model_has_content
    )
{
    dlb_pmd_bool     is_empty;
    dlb_pmd_success  success;

    if ((model_combo == NULL) || ((pmd_model_has_content == NULL) && (core_model_has_content == NULL)))
    {
        return FAILURE;
    }

    if (pmd_model_has_content != NULL)
    {
        success = pmd_model_is_empty(model_combo->pmd_model, &is_empty);
        CHECK_SUCCESS(success);
        *pmd_model_has_content = !is_empty;
    }

    if (core_model_has_content != NULL)
    {
        int status = dlb_adm_core_model_is_empty(model_combo->core_model, &is_empty);
        CHECK_STATUS_SUCCESS(status);
        *core_model_has_content = !is_empty;
    }

    return PMD_SUCCESS;
}

dlb_pmd_success
dlb_pmd_model_combo_get_state
    (dlb_pmd_model_combo        *model_combo
    ,DLB_PMD_MODEL_COMBO_STATE  *pmd_model_state
    ,DLB_PMD_MODEL_COMBO_STATE  *core_model_state
    )
{
    dlb_pmd_bool                 pmd_model_has_content;
    dlb_pmd_bool                 core_model_has_content;
    DLB_PMD_MODEL_COMBO_STATE    p_state;
    DLB_PMD_MODEL_COMBO_STATE    c_state;
    dlb_pmd_success              success;

    if ((model_combo == NULL) || ((pmd_model_state == NULL) && (core_model_state == NULL)))
    {
        return FAILURE;
    }

    success = dlb_pmd_model_combo_has_content(model_combo, &pmd_model_has_content, &core_model_has_content);
    CHECK_SUCCESS(success);

    switch (model_combo->combo_state)
    {
    case COMBO_STATE_UNKNOWN:
        p_state = (pmd_model_has_content  ? DLB_PMD_MODEL_COMBO_STATE_HAS_CONTENT : DLB_PMD_MODEL_COMBO_STATE_IS_EMPTY);
        c_state = (core_model_has_content ? DLB_PMD_MODEL_COMBO_STATE_HAS_CONTENT : DLB_PMD_MODEL_COMBO_STATE_IS_EMPTY);
        break;

    case COMBO_STATE_PMD_MODEL_PRIMARY:
        p_state = DLB_PMD_MODEL_COMBO_STATE_IS_PRIMARY;
        c_state = (core_model_has_content ? DLB_PMD_MODEL_COMBO_STATE_IS_CONVERTED : DLB_PMD_MODEL_COMBO_STATE_IS_EMPTY);
        break;

    case COMBO_STATE_CORE_MODEL_PRIMARY:
        p_state = (pmd_model_has_content ? DLB_PMD_MODEL_COMBO_STATE_IS_CONVERTED : DLB_PMD_MODEL_COMBO_STATE_IS_EMPTY);
        c_state = DLB_PMD_MODEL_COMBO_STATE_IS_PRIMARY;
        break;

    default:
        return FAILURE;
    }

    if (pmd_model_state != NULL)
    {
        *pmd_model_state = p_state;
    }

    if (core_model_state != NULL)
    {
        *core_model_state = c_state;
    }

    return PMD_SUCCESS;
}

dlb_pmd_success
dlb_pmd_model_combo_use_adm_common_defs
    (const dlb_pmd_model_combo  *model_combo
    ,dlb_pmd_bool               *use_common_defs
    )
{
    if ((model_combo == NULL) || (use_common_defs == NULL))
    {
        return FAILURE;
    }

    *use_common_defs = model_combo->use_adm_common_defs;

    return PMD_SUCCESS;
}

dlb_pmd_success
dlb_pmd_model_combo_clear
    (dlb_pmd_model_combo    *model_combo
    )
{
    if (model_combo == NULL)
    {
        return PMD_FAIL;
    }

    if (dlb_pmd_reset(model_combo->pmd_model) || dlb_adm_core_model_clear(model_combo->core_model))
    {
        return PMD_FAIL;
    }

    model_combo->combo_state = COMBO_STATE_UNKNOWN;

    return PMD_SUCCESS;
}

dlb_pmd_success
dlb_pmd_model_combo_destroy
    (dlb_pmd_model_combo   **model_combo
    )
{
    dlb_pmd_success success = PMD_FAIL;

    if ((model_combo != NULL) && (*model_combo != NULL))
    {
        dlb_pmd_model_combo *mc = *model_combo;

        if ((mc->pmd_model != NULL) && (!mc->existing_pmd_model))
        {
            dlb_pmd_finish(mc->pmd_model);
        }

        if ((mc->core_model != NULL) && (!mc->existing_core_model))
        {
            (void)dlb_adm_core_model_close(&mc->core_model);
        }

        if (mc->mallocated)
        {
            void *mallocated_memory = mc->combo_model_memory;

            memset(mc, 0, sizeof(*mc));
            free(mallocated_memory);
        }
        else
        {
            memset(mc, 0, sizeof(*mc));
        }

        *model_combo = NULL;
        success = PMD_SUCCESS;
    }

    return success;
}
