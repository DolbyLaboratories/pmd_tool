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
 * @file TestModelRandom.hh
 * @brief encapsulate control of pmd models for testing, with randomly generated
 * model
 */


#ifndef DLB_PMD_TEST_MODEL_RANDOM_HH
#define DLB_PMD_TEST_MODEL_RANDOM_HH

extern "C"
{
#include "dlb_pmd_api.h"
#include <stdint.h>
}

#include "PrngKiss.hh"
#include "TestModel.hh"
#include <string>

#ifdef _MSC_VER
#  define snprintf _snprintf
#endif

/**
 * @brief encapsulate everything relating to model construction
 *
 * This is a convenience struct for managing the creation/destruction
 * of dlb_pmd_models.
 */
class TestModelRandom: public TestModel
{
    dlb_pmd_model_constraints constraintmem_;

    static dlb_pmd_model_constraints&
    random_constraints_
        (dlb_pmd_model_constraints& con
        ,unsigned int seed
        )
    {
        dlb_pmd_metadata_count *max = &con.max;
        PrngKiss prng;
        prng.seed((uint32_t)seed);
        
        max->num_signals         = 1 + (prng.next() % DLB_PMD_MAX_SIGNALS);
        max->num_beds            = prng.next() % (DLB_PMD_MAX_AUDIO_ELEMENTS+1);
        max->num_objects         = prng.next() % (DLB_PMD_MAX_AUDIO_ELEMENTS+1);
        max->num_updates         = prng.next() % (DLB_PMD_MAX_UPDATES+1);
        max->num_presentations   = prng.next() % (DLB_PMD_MAX_PRESENTATIONS+1);
        max->num_loudness        = prng.next() % (DLB_PMD_MAX_PRESENTATIONS+1);
        max->num_iat             = prng.next() % 2;
        max->num_eac3            = prng.next() % (DLB_PMD_MAX_EAC3_ENCODING_PARAMETERS+1);
        max->num_ed2_system      = prng.next() % 2;
        max->num_ed2_turnarounds = prng.next() % (DLB_PMD_MAX_ED2_TURNAROUNDS+1);
        max->num_headphone_desc  = prng.next() % (DLB_PMD_MAX_HEADPHONE+1);

        con.max_elements         = prng.next() % (DLB_PMD_MAX_AUDIO_ELEMENTS+1);
        con.max_presentation_names = max->num_presentations
            + prng.next() % (DLB_PMD_MAX_PRESENTATIONS * DLB_PMD_MAX_PRESENTATION_NAMES+1 - max->num_presentations);
        return con;
    }
    

public:

    enum ModelType { PMD, SADM };

    TestModelRandom(unsigned int seed, ModelType type = PMD)
        : TestModel(random_constraints_(constraintmem_, seed))
    {
        generate_random(seed*7, type == SADM, true);
    }
    
    virtual ~TestModelRandom() {};
};


#endif // DLB_PMD_TEST_MODEL_RANDOM_HH

