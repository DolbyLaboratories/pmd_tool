/************************************************************************
 * dlb_pmd
 * Copyright (c) 2019-2020, Dolby Laboratories Inc.
 * Copyright (c) 2019-2020, Dolby International AB.
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

