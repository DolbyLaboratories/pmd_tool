/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2018-2019 by Dolby Laboratories,
 *                Copyright (C) 2018-2019 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

/**
 * @file dlb_pmd_compare.c
 * @brief implementation of model comparison, using public APIs.
 */

#include "dlb_pmd_api.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#if defined(_MSC_VER) && !defined(INFINITY)
#define INFINITY (-logf(0.0f))
#define isinf(x) (!_finite(x))
#endif


//#define TRACE_FAILURES
#ifdef TRACE_FAILURES
#  define TRACE_FAIL(x) printf x
#else
#  define TRACE_FAIL(x) 
#endif


/**
 * Defines which components of PMD should be considered
 * during comparison.
 */
DLB_PMD_DLL_ENTRY
const uint32_t PMD_COMPARE_MASK = PMD_EQUAL_MASK_SIGNALS
                                | PMD_EQUAL_MASK_BEDS
                                | PMD_EQUAL_MASK_OBJECTS
                                | PMD_EQUAL_MASK_PRESENTATIONS
                                | PMD_EQUAL_MASK_HEADPHONES
                                | PMD_EQUAL_MASK_NUM_ED2_SYSTEM
                                | PMD_EQUAL_MASK_LOUDNESS
                                | PMD_EQUAL_MASK_EAC3
                                | PMD_EQUAL_MASK_ED2_TURNAROUNDS
                                | PMD_EQUAL_MASK_ED2_UPDATES;

/**
 * @brief helper function to provide a simple location to breakpoint
 */
static
void
print_inequality
    (unsigned int line
    )
{
    (void)line;
    TRACE_FAIL(("test fail: %u\n", line));
}


/**
 * @def NOT_EQUAL
 * @brief helper macro to simplify the code
 */
#define NOT_EQUAL { print_inequality(__LINE__); return PMD_FAIL; }
    

/**
 * @brief compare PCM signals
 */
static
dlb_pmd_success                 /** @return PMD_SUCCESS on success, 1 on failure */
signals_equal
    (const dlb_pmd_model *m1    /**< [in] first model */
    ,const dlb_pmd_model *m2    /**< [in] second model */
    )
{
    if (dlb_pmd_num_signals(m1) != dlb_pmd_num_signals(m2)) NOT_EQUAL;
    return PMD_SUCCESS;
}


/**
 * @brief compare audio beds
 */
static
dlb_pmd_success                 /** @return PMD_SUCCESS on success, 1 on failure */
beds_equal
    (const dlb_pmd_model *m1    /**< [in] first model */
    ,const dlb_pmd_model *m2    /**< [in] second model */
    ,      dlb_pmd_bool minimal /**< [in] essential checking, no names */
    )
{
#define NUM_SOURCES (256)
    dlb_pmd_source tmp1[NUM_SOURCES];
    dlb_pmd_source tmp2[NUM_SOURCES];
    dlb_pmd_source *s1;
    dlb_pmd_source *s2;
    dlb_pmd_bed_iterator be;
    dlb_pmd_bed bed1;
    dlb_pmd_bed bed2;
    unsigned int i;
    
    if (dlb_pmd_num_beds(m1) != dlb_pmd_num_beds(m2))
    {
        NOT_EQUAL;
    }

    if (dlb_pmd_bed_iterator_init(&be, m1)) NOT_EQUAL;
    while (!dlb_pmd_bed_iterator_next(&be, &bed1, NUM_SOURCES, tmp1))
    {
        if (dlb_pmd_bed_lookup(m2, bed1.id, &bed2, NUM_SOURCES, tmp2)) NOT_EQUAL;
        if (bed1.config      != bed2.config) NOT_EQUAL;
        if (bed1.bed_type    != bed2.bed_type) NOT_EQUAL;
        if (bed1.source_id   != bed2.source_id) NOT_EQUAL;
        if (bed1.num_sources != bed2.num_sources) NOT_EQUAL;
        if (!minimal && strncmp(bed1.name, bed2.name, sizeof(bed1.name))) NOT_EQUAL;

        s1 = bed1.sources;
        s2 = bed2.sources;
        for (i = 0; i != bed1.num_sources; ++i)
        {
            /* todo: we should probably allow differences in order */
            if (s1->target != s2->target) NOT_EQUAL;
            if (s1->source != s2->source) NOT_EQUAL;
            if (s1->gain != s2->gain) NOT_EQUAL;
            ++s1;
            ++s2;
        }
    }
    return PMD_SUCCESS;
}


/**
 * @def PLUS_MINUS_EQUAL(x,y,epsilon)
 * @brief equality plus or minus some e value
 */
#define PLUS_MINUS_EQUAL(x,y,e) (((x)<((y)+(e))) && ((x)>((y)-(e))))


/**
 * @brief compare audio object elements
 */
static
dlb_pmd_success                 /** @return PMD_SUCCESS on success, 1 on failure */
objects_equal
    (const dlb_pmd_model *m1    /**< [in] first model */
    ,const dlb_pmd_model *m2    /**< [in] second model */
    ,      dlb_pmd_bool minimal /**< [in] essential checking, no names */
    )
{
    dlb_pmd_object_iterator oe;
    dlb_pmd_object obj1;
    dlb_pmd_object obj2;
    
    if (dlb_pmd_num_objects(m1) != dlb_pmd_num_objects(m2))
    {
        NOT_EQUAL;
    }

    dlb_pmd_object_iterator_init(&oe, m1);
    while (!dlb_pmd_object_iterator_next(&oe, &obj1))
    {
        if (dlb_pmd_object_lookup(m2, obj1.id, &obj2)) NOT_EQUAL;
        if (obj1.id != obj2.id) NOT_EQUAL;
        if (obj1.object_class != obj2.object_class) NOT_EQUAL;
        if (obj1.dynamic_updates != obj2.dynamic_updates) NOT_EQUAL;
        if (!minimal && strncmp(obj1.name, obj2.name, sizeof(obj1.name))) NOT_EQUAL;
        
        if (!PLUS_MINUS_EQUAL(obj1.x, obj2.x, 0.01)) NOT_EQUAL;
        if (!PLUS_MINUS_EQUAL(obj1.y, obj2.y, 0.01)) NOT_EQUAL;
        if (!PLUS_MINUS_EQUAL(obj1.z, obj2.z, 0.01)) NOT_EQUAL;                
        if (!PLUS_MINUS_EQUAL(obj1.size, obj2.size, 1/62.0)) NOT_EQUAL;
        if (isinf(obj1.source_gain) != isinf(obj2.source_gain)) NOT_EQUAL;
        if (0 == isinf(obj1.source_gain))
        {
            if (!PLUS_MINUS_EQUAL(obj1.source_gain, obj2.source_gain, 0.17)) NOT_EQUAL;
        }
        if (obj1.size_3d != obj2.size_3d) NOT_EQUAL;
        if (obj1.diverge != obj2.diverge) NOT_EQUAL;
        if (obj1.source != obj2.source) NOT_EQUAL;
    }
    return PMD_SUCCESS;
}


/**
 * @brief compare audio presentations
 */
static
dlb_pmd_success                 /** @return PMD_SUCCESS on success, 1 on failure */
presentations_equal
    (const dlb_pmd_model *m1    /**< [in] first model */
    ,const dlb_pmd_model *m2    /**< [in] second model */
    ,      dlb_pmd_bool minimal /**< [in] essential checking, no names */
    )
{
#define NUM_ELEMENTS (4096)
    dlb_pmd_element_id elements1[NUM_ELEMENTS];
    dlb_pmd_element_id elements2[NUM_ELEMENTS];
    dlb_pmd_presentation_iterator pe;
    dlb_pmd_presentation p1;
    dlb_pmd_presentation p2;
    unsigned int i;
    unsigned int j;
    
    if (dlb_pmd_num_presentations(m1) != dlb_pmd_num_presentations(m2))
    {
        NOT_EQUAL;
    }

    dlb_pmd_presentation_iterator_init(&pe, m1);
    while (!dlb_pmd_presentation_iterator_next(&pe, &p1, NUM_ELEMENTS, elements1))
    {
        if (dlb_pmd_presentation_lookup(m2, p1.id, &p2, NUM_ELEMENTS, elements2)) NOT_EQUAL;
        if (p1.config         != p2.config) NOT_EQUAL;
        if (strcmp(p1.audio_language, p2.audio_language)) NOT_EQUAL;
        if (p1.num_elements   != p2.num_elements) NOT_EQUAL;
        if (!minimal)
        {
            if (p1.num_names != p2.num_names) NOT_EQUAL;
            for (i = 0; i != p1.num_names; ++i)
            {
                /* The order is not important, but we assume no duplicates.
                 * TODO: make this more efficient by storing names in
                 * a known order
                 */
                dlb_pmd_presentation_name *n1 = &p1.names[i];
                dlb_pmd_presentation_name *n2 = &p2.names[0];
                for (j = 0; j != p2.num_names; ++j, ++n2)
                {
                    if (!strcmp(n1->language, n2->language) &&
                        !strcmp(n1->text, n2->text))
                    {
                        break;
                    }
                }
                if (j == p2.num_names)
                {
                    NOT_EQUAL;
                }
            }
        }

        for (i = 0; i != p1.num_elements; ++i)
        {
            /* elements are inserted in order */
            if (p1.elements[i] != p2.elements[i]) NOT_EQUAL;
        }
    }
    return PMD_SUCCESS;
}


/**
 * @brief compare headphone element descriptions
 */
static
dlb_pmd_success                 /** @return PMD_SUCCESS on success, 1 on failure */
headphones_equal
    (const dlb_pmd_model *m1    /**< [in] first model */
    ,const dlb_pmd_model *m2    /**< [in] second model */
    )
{
    dlb_pmd_hed_iterator he;
    dlb_pmd_headphone h1;
    dlb_pmd_headphone h2;
    dlb_pmd_object obj;
    
    if (dlb_pmd_num_headphone_element_desc(m1) != dlb_pmd_num_headphone_element_desc(m2))
    {
        NOT_EQUAL;
    }

    dlb_pmd_hed_iterator_init(&he, m1);
    while (!dlb_pmd_hed_iterator_next(&he, &h1))
    {
        if (dlb_pmd_hed_lookup(m2, h1.audio_element_id, &h2)) NOT_EQUAL;
        if (h1.head_tracking_enabled != h2.head_tracking_enabled) NOT_EQUAL;
        if (h1.render_mode != h2.render_mode) NOT_EQUAL;

        if (dlb_pmd_object_lookup(m1, h1.audio_element_id, &obj))
        {
            /* not an object */
            if (h1.channel_mask != h2.channel_mask) NOT_EQUAL;
        }
    }
    return PMD_SUCCESS;
}


/**
 * @brief compare title
 */
static
dlb_pmd_success                 /** @return PMD_SUCCESS on success, 1 on failure */
title_equal
    (const dlb_pmd_model *m1    /**< [in] first model */
    ,const dlb_pmd_model *m2    /**< [in] second model */
    )
{
    const char *title1;
    const char *title2;
    
    if (dlb_pmd_title(m1, &title1)) NOT_EQUAL;
    if (dlb_pmd_title(m2, &title2)) NOT_EQUAL;
    if (strcmp(title1, title2)) NOT_EQUAL;
    return PMD_SUCCESS;
}


/**
 * @brief compare presentation loudness
 */
static
dlb_pmd_success                 /** @return PMD_SUCCESS on success, 1 on failure */
loudness_equal
    (const dlb_pmd_model *m1    /**< [in] first model */
    ,const dlb_pmd_model *m2    /**< [in] second model */
    )
{
    dlb_pmd_loudness_iterator le;
    dlb_pmd_loudness loud1;
    dlb_pmd_loudness loud2;
    
    if (dlb_pmd_num_loudness(m1) != dlb_pmd_num_loudness(m2))
    {
        NOT_EQUAL;
    }

    dlb_pmd_loudness_iterator_init(&le, m1);
    while (!dlb_pmd_loudness_iterator_next(&le, &loud1))
    {
        if (dlb_pmd_loudness_lookup(m2, loud1.presid, &loud2)) NOT_EQUAL;
        if (loud1.presid != loud2.presid) NOT_EQUAL;
        if (loud1.loud_prac_type != loud2.loud_prac_type) NOT_EQUAL;
        if (loud1.b_loudcorr_gating != loud2.b_loudcorr_gating) NOT_EQUAL;
        if (loud1.b_loudcorr_gating)
        {
            if (loud1.loudcorr_gating != loud2.loudcorr_gating) NOT_EQUAL;
        }
        if (loud1.loudcorr_type != loud2.loudcorr_type) NOT_EQUAL;
        if (loud1.b_loudrelgat != loud2.b_loudrelgat) NOT_EQUAL;
        if (loud1.b_loudrelgat)
        {
            if (loud1.loudrelgat != loud2.loudrelgat) NOT_EQUAL;
        }
        if (loud1.b_loudspchgat != loud2.b_loudspchgat) NOT_EQUAL;
        if (loud1.b_loudspchgat)
        {
            if (loud1.loudspchgat != loud2.loudspchgat) NOT_EQUAL;
            if (loud1.loudspch_gating != loud2.loudspch_gating) NOT_EQUAL;
        }
        if (loud1.b_loudstrm3s != loud2.b_loudstrm3s) NOT_EQUAL;
        if (loud1.b_loudstrm3s)
        {
            if (loud1.loudstrm3s != loud2.loudstrm3s) NOT_EQUAL;
        }
        if (loud1.b_max_loudstrm3s != loud2.b_max_loudstrm3s) NOT_EQUAL;
        if (loud1.b_max_loudstrm3s)
        {
            if (loud1.max_loudstrm3s != loud2.max_loudstrm3s) NOT_EQUAL;
        }
        if (loud1.b_truepk != loud2.b_truepk) NOT_EQUAL;
        if (loud1.b_truepk)
        {
            if (loud1.truepk != loud2.truepk) NOT_EQUAL;
        }
        if (loud1.b_max_truepk != loud2.b_max_truepk) NOT_EQUAL;
        if (loud1.b_max_truepk)
        {
            if (loud1.max_truepk != loud2.max_truepk) NOT_EQUAL;
        }
        if (loud1.b_prgmbndy != loud2.b_prgmbndy) NOT_EQUAL;
        if (loud1.b_prgmbndy)
        {
            if (loud1.prgmbndy != loud2.prgmbndy) NOT_EQUAL;
            if (loud1.b_prgmbndy_offset != loud2.b_prgmbndy_offset) NOT_EQUAL;
            if (loud1.b_prgmbndy_offset)
            {
                if (loud1.prgmbndy_offset != loud2.prgmbndy_offset) NOT_EQUAL;
            }
        }
        if (loud1.b_lra != loud2.b_lra) NOT_EQUAL;
        if (loud1.b_lra)
        {
            if (loud1.lra != loud2.lra) NOT_EQUAL;
            if (loud1.lra_prac_type != loud2.lra_prac_type) NOT_EQUAL;
        }
        if (loud1.b_loudmntry != loud2.b_loudmntry) NOT_EQUAL;
        if (loud1.b_loudmntry)
        {
            if (loud1.loudmntry != loud2.loudmntry) NOT_EQUAL;
        }
        if (loud1.b_max_loudmntry)
        {
            if (loud1.max_loudmntry != loud2.max_loudmntry) NOT_EQUAL;
        }
        if (loud1.extension.size != loud2.extension.size) NOT_EQUAL;
        if (loud1.extension.size)
        {
            if (memcmp(loud1.extension.data, loud2.extension.data,
                       (loud1.extension.size + 7)/8))
            {
                NOT_EQUAL;
            }
        }
    }
    return PMD_SUCCESS;
}


/**
 * @brief compare Identity and Timing
 */
static
dlb_pmd_success                 /** @return PMD_SUCCESS on success, 1 on failure */
iat_equal
    (const dlb_pmd_model *m1    /**< [in] first model */
    ,const dlb_pmd_model *m2    /**< [in] second model */
    )
{
    dlb_pmd_identity_and_timing iat1;
    dlb_pmd_identity_and_timing iat2;
    
    if (dlb_pmd_num_iat(m1) != dlb_pmd_num_iat(m2))
    {
        NOT_EQUAL;
    }

    if (dlb_pmd_num_iat(m1))
    {
        if (dlb_pmd_iat_lookup(m1, &iat1)) NOT_EQUAL;
        if (dlb_pmd_iat_lookup(m2, &iat2)) NOT_EQUAL;
        if (memcmp(&iat1, &iat2, sizeof(iat1))) NOT_EQUAL;
    }
    return PMD_SUCCESS;
}


/**
 * @brief compare EAC3 encoding parameters
 */
static
dlb_pmd_success                 /** @return PMD_SUCCESS on success, 1 on failure */
eac3_equal
    (const dlb_pmd_model *m1    /**< [in] first model */
    ,const dlb_pmd_model *m2    /**< [in] second model */
    )
{
    dlb_pmd_eac3_iterator ee;
    dlb_pmd_eac3 eep1;
    dlb_pmd_eac3 eep2;
    
    if (dlb_pmd_num_eac3(m1) != dlb_pmd_num_eac3(m2))
    {
        NOT_EQUAL;
    }

    dlb_pmd_eac3_iterator_init(&ee, m1);
    while (!dlb_pmd_eac3_iterator_next(&ee, &eep1))
    {
        if (dlb_pmd_eac3_lookup(m2, eep1.id, &eep2)) NOT_EQUAL;
        if (memcmp(&eep1, &eep2, sizeof(eep1))) NOT_EQUAL;
    }
    return PMD_SUCCESS;
}


/**
 * @brief compare ED2 system information
 */
static
dlb_pmd_success                 /** @return PMD_SUCCESS on success, 1 on failure */
ed2_system_equal
    (const dlb_pmd_model *m1    /**< [in] first model */
    ,const dlb_pmd_model *m2    /**< [in] second model */
    )
{
    unsigned int c1 = dlb_pmd_num_ed2_system(m1);
    unsigned int c2 = dlb_pmd_num_ed2_system(m2);
    
    if (c1 != c2) NOT_EQUAL;
    if (c1)
    {
        dlb_pmd_ed2_system e1;
        dlb_pmd_ed2_system e2;

        if (dlb_pmd_ed2_system_lookup(m1, &e1)) NOT_EQUAL;
        if (dlb_pmd_ed2_system_lookup(m2, &e2)) NOT_EQUAL;
        if (memcmp(&e1, &e2, sizeof(e1))) NOT_EQUAL;
    }
    return PMD_SUCCESS;
}


/**
 * @brief compare ED2 turnarounds
 */
static
dlb_pmd_success                 /** @return PMD_SUCCESS on success, 1 on failure */
ed2_turnarounds_equal
    (const dlb_pmd_model *m1    /**< [in] first model */
    ,const dlb_pmd_model *m2    /**< [in] second model */
    )
{
    dlb_pmd_ed2_turnaround_iterator ee;
    dlb_pmd_ed2_turnaround etd1;
    dlb_pmd_ed2_turnaround etd2;
    unsigned int i;
    
    if (dlb_pmd_num_ed2_turnarounds(m1) != dlb_pmd_num_ed2_turnarounds(m2))
    {
        NOT_EQUAL;
    }

    if (dlb_pmd_ed2_turnaround_iterator_init(&ee, m1)) NOT_EQUAL;
    while (!dlb_pmd_ed2_turnaround_iterator_next(&ee, &etd1))
    {
        if (dlb_pmd_ed2_turnaround_lookup(m2, etd1.id, &etd2)) NOT_EQUAL;
        if (etd1.ed2_presentations != etd2.ed2_presentations) NOT_EQUAL;
        if (etd1.ed2_presentations)
        {
            dlb_pmd_turnaround *t1 = etd1.ed2_turnarounds;
            dlb_pmd_turnaround *t2 = etd2.ed2_turnarounds;
            
            if (etd1.ed2_framerate != etd2.ed2_framerate) NOT_EQUAL;
            for (i = 0; i != etd1.ed2_presentations; ++i)
            {
                if (memcmp(t1, t2, sizeof(*t1))) NOT_EQUAL;
                ++t1;
                ++t2;
            }
        }
        if (etd1.de_presentations != etd2.de_presentations) NOT_EQUAL;
        if (etd1.de_presentations)
        {
            dlb_pmd_turnaround *t1 = etd1.de_turnarounds;
            dlb_pmd_turnaround *t2 = etd2.de_turnarounds;

            if (etd1.de_framerate != etd2.de_framerate) NOT_EQUAL;
            if (etd1.pgm_config != etd2.pgm_config) NOT_EQUAL;
            for (i = 0; i != etd1.de_presentations; ++i)
            {
                if (memcmp(t1, t2, sizeof(*t1))) NOT_EQUAL;
                ++t1;
                ++t2;
            }
        }
    }
    return PMD_SUCCESS;
}


/**
 * @brief compare ED2 turnarounds
 */
static
dlb_pmd_success                 /** @return PMD_SUCCESS on success, 1 on failure */
updates_equal
    (const dlb_pmd_model *m1    /**< [in] first model */
    ,const dlb_pmd_model *m2    /**< [in] second model */
    )
{
    dlb_pmd_update_iterator ui1;
    dlb_pmd_update u1;
    dlb_pmd_update u2;
    
    if (dlb_pmd_num_updates(m1) != dlb_pmd_num_updates(m2))
    {
        NOT_EQUAL;
    }

    if (dlb_pmd_update_iterator_init(&ui1, m1)) NOT_EQUAL;
    while (!dlb_pmd_update_iterator_next(&ui1, &u1))
    {
        if (dlb_pmd_update_lookup(m2, u1.sample_offset, u1.id, &u2)) NOT_EQUAL;
        if (!PLUS_MINUS_EQUAL(u1.x, u2.x, 0.01)) NOT_EQUAL;
        if (!PLUS_MINUS_EQUAL(u1.y, u2.y, 0.01)) NOT_EQUAL;
        if (!PLUS_MINUS_EQUAL(u1.z, u2.z, 0.01)) NOT_EQUAL;
    }
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_equal2
    (const dlb_pmd_model *m1
    ,const dlb_pmd_model *m2
    ,      dlb_pmd_bool ignore_updates
    ,      dlb_pmd_bool ignore_names
    ,      dlb_pmd_bool minimal
    )
{
    if (minimal)
    {
        ignore_names = 1;
        ignore_updates = 1;
    }

    if (   signals_equal(m1, m2)
        || beds_equal(m1, m2, ignore_names)
        || objects_equal(m1, m2, ignore_names)
        || presentations_equal(m1, m2, ignore_names)
        || headphones_equal(m1, m2)
       )
    {
        return PMD_FAIL;
    }
    
    if (!minimal)
    {
        if (   (dlb_pmd_num_ed2_system(m1) && title_equal(m1, m2))
            || loudness_equal(m1, m2)
            || iat_equal(m1, m2)
            || eac3_equal(m1, m2)
            || ed2_system_equal(m1, m2)
            || ed2_turnarounds_equal(m1, m2)
           )
        {
            return PMD_FAIL;
        }
    }
    if (!ignore_updates)
    {
        if (updates_equal(m1, m2))
        {
            return PMD_FAIL;
        }
    }
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_equal3
    (const dlb_pmd_model *m1
    ,const dlb_pmd_model *m2
    ,      dlb_pmd_bool ignore_names
    ,      uint32_t components_to_check
    )
{
    if (   ( (components_to_check & PMD_EQUAL_MASK_SIGNALS)         && signals_equal(m1, m2))
        || ( (components_to_check & PMD_EQUAL_MASK_BEDS)            && beds_equal(m1, m2, ignore_names))
        || ( (components_to_check & PMD_EQUAL_MASK_OBJECTS)         && objects_equal(m1, m2, ignore_names))
        || ( (components_to_check & PMD_EQUAL_MASK_PRESENTATIONS)   && presentations_equal(m1, m2, ignore_names))
        || ( (components_to_check & PMD_EQUAL_MASK_HEADPHONES)      && headphones_equal(m1, m2))
        || ( (components_to_check & PMD_EQUAL_MASK_NUM_ED2_SYSTEM)  && (dlb_pmd_num_ed2_system(m1) && title_equal(m1, m2)))
        || ( (components_to_check & PMD_EQUAL_MASK_LOUDNESS)        && loudness_equal(m1, m2))
        || ( (components_to_check & PMD_EQUAL_MASK_IAT)             && iat_equal(m1, m2))
        || ( (components_to_check & PMD_EQUAL_MASK_EAC3)            && eac3_equal(m1, m2))
        || ( (components_to_check & PMD_EQUAL_MASK_ED2_SYSTEM)      && ed2_system_equal(m1, m2))
        || ( (components_to_check & PMD_EQUAL_MASK_ED2_TURNAROUNDS) && ed2_turnarounds_equal(m1, m2))
        || ( (components_to_check & PMD_EQUAL_MASK_ED2_UPDATES)     && updates_equal(m1, m2))
       )
    {
        return PMD_FAIL;
    }

    return PMD_SUCCESS;
}

