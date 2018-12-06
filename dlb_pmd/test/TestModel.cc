/************************************************************************
 * dlb_pmd
 * Copyright (c) 2018, Dolby Laboratories Inc.
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

//#define ENABLE_TRACE
#ifdef ENABLE_TRACE
#  define TRACE printf
#else
#  define TRACE(...)
#endif


extern "C"
{
#include "dlb_pmd_api.h"
#include "dlb_pmd_generate.h"
#include "src/model/pmd_model.h"  /* not part of public API! */
}

#include "TestXml.hh"
#include "TestKlv.hh"
#include "TestPcm.hh"
#include "TestMdset.hh"

#include "TestModel.hh"

#include <cstdlib>

const TestModel::TestType TestModel::TEST_TYPES[] =
{
    TestModel::TEST_XML,
    TestModel::TEST_MDSET,
    TestModel::TEST_KLV,
    TestModel::TEST_PCM_PAIR_2398,
    TestModel::TEST_PCM_CHAN_2398,
    TestModel::TEST_PCM_PAIR_2400,
    TestModel::TEST_PCM_CHAN_2400,
    TestModel::TEST_PCM_PAIR_2500,
    TestModel::TEST_PCM_CHAN_2500,
    TestModel::TEST_PCM_PAIR_2997,
    TestModel::TEST_PCM_CHAN_2997,
    TestModel::TEST_PCM_PAIR_3000,
    TestModel::TEST_PCM_CHAN_3000,
    TestModel::TEST_PCM_PAIR_5000,
    TestModel::TEST_PCM_CHAN_5000,
    TestModel::TEST_PCM_PAIR_5994,
    TestModel::TEST_PCM_CHAN_5994,
    TestModel::TEST_PCM_PAIR_6000,
    TestModel::TEST_PCM_CHAN_6000,
    TestModel::TEST_PCM_PAIR_10000,
    TestModel::TEST_PCM_CHAN_10000,
    TestModel::TEST_PCM_PAIR_11988,
    TestModel::TEST_PCM_CHAN_11988,
    TestModel::TEST_PCM_PAIR_12000,
    TestModel::TEST_PCM_CHAN_12000,
};


const dlb_pmd_frame_rate TestModel::FRAME_RATES[] =
{
    DLB_PMD_FRAMERATE_2398,
    DLB_PMD_FRAMERATE_2400,
    DLB_PMD_FRAMERATE_2500,
    DLB_PMD_FRAMERATE_2997,
    DLB_PMD_FRAMERATE_3000,
    DLB_PMD_FRAMERATE_5000,
    DLB_PMD_FRAMERATE_5994,
    DLB_PMD_FRAMERATE_6000,        
    DLB_PMD_FRAMERATE_10000,
    DLB_PMD_FRAMERATE_11988,
    DLB_PMD_FRAMERATE_12000,                
};


/**
 * @brief frame rate strings
 */
const char *TestModel::FRAME_RATE_NAMES[] =
{
    "23.98", "24", "25", "29.97", "30", "50", "59.94", "60", "100", "119.88", "120"
};


#define NUM_FRAME_RATE_NAMES ((sizeof FRAME_RATE_NAMES)/(sizeof FRAME_RATE_NAMES[0]))



TestModel::TestModel()
    : size_(dlb_pmd_query_mem())
    , mem_(size_, 0)
    , model_(0)
    , pcm_skip_samples_(0)
    , pcm_single_frame_(false)
    , minimal_check_(false)
    , max_sig_id_(0)
    , max_track_id_(0)
    , max_element_id_(0)
    , max_pres_id_(0)
    , max_eac3_id_(0)
    , max_etd_id_(0)
{
    dlb_pmd_init(&model_, &mem_[0]);
}


TestModel::~TestModel()
{
    dlb_pmd_finish(model_);
    model_ = 0;
}


TestModel& TestModel::operator= (const TestModel& other)
{
    assert(size_ == other.size_);
    dlb_pmd_copy(model_, other.model_);
    max_sig_id_ = other.max_sig_id_;
    max_track_id_ = other.max_track_id_;
    max_element_id_ = other.max_element_id_;
    max_pres_id_ = other.max_pres_id_;
    max_eac3_id_ = other.max_eac3_id_;
    max_etd_id_ = other.max_etd_id_;
    return *this;
}
    

void TestModel::illegal_set_version(uint8_t maj, uint8_t min)
{
    model_->version_avail = 1;
    model_->version_maj = maj;
    model_->version_min = min;
}
    

bool TestModel::populate(dlb_pmd_element_id *obj)
{
    dlb_pmd_element_id bed_20  = new_elid();
    dlb_pmd_element_id bed_51  = new_elid();
    dlb_pmd_element_id bed_512 = new_elid();
    dlb_pmd_element_id bed_514 = new_elid();
    dlb_pmd_element_id d1      = new_elid();
    dlb_pmd_element_id d2      = new_elid();
    dlb_pmd_element_id vds     = new_elid();
    dlb_pmd_element_id o1      = new_elid();
    dlb_pmd_element_id o2      = new_elid();
    dlb_pmd_element_id o3      = new_elid();
    dlb_pmd_presentation_id p1 = new_presid();
    dlb_pmd_presentation_id p2 = new_presid();
    dlb_pmd_presentation_id p3 = new_presid();
    dlb_pmd_presentation_id p4 = new_presid();

    if (   dlb_pmd_add_signals(model_, 16)
        || dlb_pmd_add_bed(model_, bed_20,  NULL, DLB_PMD_SPEAKER_CONFIG_2_0,   1, 0)
        || dlb_pmd_add_bed(model_, bed_51,  NULL, DLB_PMD_SPEAKER_CONFIG_5_1,   1, 0)
        || dlb_pmd_add_bed(model_, bed_512, NULL, DLB_PMD_SPEAKER_CONFIG_5_1_2, 1, 0)
        || dlb_pmd_add_bed(model_, bed_514, NULL, DLB_PMD_SPEAKER_CONFIG_5_1_4, 1, 0)

        || dlb_pmd_add_generic_obj2(model_, d1,  NULL, 11, 0.5f, 0.0f, 0.0f)
        || dlb_pmd_add_generic_obj2(model_, d2,  NULL, 12, 0.5f, 0.0f, 0.0f)
        || dlb_pmd_add_generic_obj2(model_, vds, NULL, 13, 0.5f, 0.0f, 0.0f)
        || dlb_pmd_add_generic_obj(model_, o1, NULL, 14, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0, 1, 0)
        || dlb_pmd_add_generic_obj(model_, o2, NULL, 15, 0.5f, 0.5f, 0.5f, -1.0f, 0.5f, 1, 0, 0)
        || dlb_pmd_add_generic_obj(model_, o3, NULL, 16, 1.0f, 1.0f, 1.0f, -2.0f, 1.0f, 0, 0, 1)
        || dlb_pmd_add_presentation2(model_, p1, "eng", "TESTPRES1", "eng",
                                     DLB_PMD_SPEAKER_CONFIG_2_0,   1, bed_20)
        || dlb_pmd_add_presentation2(model_, p2, "eng", "TESTPRES2", "eng",
                                     DLB_PMD_SPEAKER_CONFIG_5_1,   2, bed_51, d1)
        || dlb_pmd_add_presentation2(model_, p3, "eng", "TESTPRES3", "eng",
                                     DLB_PMD_SPEAKER_CONFIG_5_1_2, 3, bed_512, d2, vds)
        || dlb_pmd_add_presentation2(model_, p4, "eng", "TESTPRES4", "eng",
                                     DLB_PMD_SPEAKER_CONFIG_5_1_4, 5, bed_514, d1, o1, o2, o3)
           /* object 1 has dynamic updates enabled */
        || dlb_pmd_add_update(model_, o1, 1, 0.1f, 0.1f, 0.1f)
        || dlb_pmd_add_update(model_, o1, 2, 0.2f, 0.2f, 0.2f)
        || dlb_pmd_add_update(model_, o1, 3, 0.3f, 0.3f, 0.3f)
        || dlb_pmd_add_update(model_, o1, 4, 0.4f, 0.4f, 0.4f)
        || dlb_pmd_add_update(model_, o1, 5, 0.5f, 0.5f, 0.5f)
        || dlb_pmd_add_update(model_, o1, 6, 0.6f, 0.6f, 0.6f)
        || dlb_pmd_add_update(model_, o1, 7, 0.7f, 0.7f, 0.7f)
        || dlb_pmd_add_update(model_, o1, 8, 0.8f, 0.8f, 0.8f)
        || dlb_pmd_add_update(model_, o1, 9, 0.9f, 0.9f, 0.9f)
        || dlb_pmd_add_update(model_, o1, 10, 1.0f, 1.0f, 1.0f)
      )
    {
        return true;
    }
    *obj = d1;
    return false;
}


void TestModel::generate_random(unsigned int seed)
{
    dlb_pmd_metadata_count count;

    memset(&count, '\0', sizeof(count));
    count.num_signals        = PMD_GENERATE_RANDOM_NUMBER;
    count.num_beds           = PMD_GENERATE_RANDOM_NUMBER;
    count.num_objects        = PMD_GENERATE_RANDOM_NUMBER;
    count.num_presentations  = PMD_GENERATE_RANDOM_NUMBER;
    count.num_loudness       = PMD_GENERATE_RANDOM_NUMBER;
    count.num_iat            = PMD_GENERATE_RANDOM_NUMBER;
    count.num_eac3           = PMD_GENERATE_RANDOM_NUMBER;
    count.num_ed2_turnarounds= PMD_GENERATE_RANDOM_NUMBER;
    count.num_headphone_desc = PMD_GENERATE_RANDOM_NUMBER;

    if (dlb_pmd_generate_random(model_, &count, seed))
    {
        printf("Failed to generate random model: %s\n", dlb_pmd_error(model_));
    }
}



bool TestModel::apply_updates()
{
    return dlb_pmd_apply_updates(model_) == 0 ? false : true;
}


void TestModel::set_ed2_system(const TestModel& other)
{
    model_->esd_present = other.model_->esd_present;
    model_->esd = other.model_->esd;
}


void TestModel::test_(TestType type, const char *n, int p, bool m, bool au)
{
    switch (type)
    {
        case TEST_XML:            test_xml_(n, p, m); break;
        case TEST_MDSET:          test_mdset_(n, p, m); break; 
        case TEST_KLV:            test_klv_(n, p, m); break;   

        case TEST_PCM_PAIR_2398:  test_pcm_(n, p, 0, false, m, au); break;  
        case TEST_PCM_PAIR_2400:  test_pcm_(n, p, 1, false, m, au); break;  
        case TEST_PCM_PAIR_2500:  test_pcm_(n, p, 2, false, m, au); break;  
        case TEST_PCM_PAIR_2997:  test_pcm_(n, p, 3, false, m, au); break;  
        case TEST_PCM_PAIR_3000:  test_pcm_(n, p, 4, false, m, au); break;  
        case TEST_PCM_PAIR_5000:  test_pcm_(n, p, 5, false, m, au); break;  
        case TEST_PCM_PAIR_5994:  test_pcm_(n, p, 6, false, m, au); break;  
        case TEST_PCM_PAIR_6000:  test_pcm_(n, p, 7, false, m, au); break;  
        case TEST_PCM_PAIR_10000: test_pcm_(n, p, 8, false, m, au); break;  
        case TEST_PCM_PAIR_11988: test_pcm_(n, p, 9, false, m, au); break;  
        case TEST_PCM_PAIR_12000: test_pcm_(n, p,10, false, m, au); break;  

        case TEST_PCM_CHAN_2398:  test_pcm_(n, p, 0, true, m, au); break;  
        case TEST_PCM_CHAN_2400:  test_pcm_(n, p, 1, true, m, au); break;  
        case TEST_PCM_CHAN_2500:  test_pcm_(n, p, 2, true, m, au); break;  
        case TEST_PCM_CHAN_2997:  test_pcm_(n, p, 3, true, m, au); break;  
        case TEST_PCM_CHAN_3000:  test_pcm_(n, p, 4, true, m, au); break;  
        case TEST_PCM_CHAN_5000:  test_pcm_(n, p, 5, true, m, au); break;  
        case TEST_PCM_CHAN_5994:  test_pcm_(n, p, 6, true, m, au); break;  
        case TEST_PCM_CHAN_6000:  test_pcm_(n, p, 7, true, m, au); break;  
        case TEST_PCM_CHAN_10000: test_pcm_(n, p, 8, true, m, au); break;  
        case TEST_PCM_CHAN_11988: test_pcm_(n, p, 9, true, m, au); break;  
        case TEST_PCM_CHAN_12000: test_pcm_(n, p,10, true, m, au); break; 

        default: abort();
    }
}


void TestModel::test_xml_(const char *testname, int param, bool match)
{
    TestXml::run(*this, testname, param, match);
}


void TestModel::test_klv_(const char *testname, int param, bool match)
{
    TestKlv::run(*this, testname, param, match);
}



void TestModel::test_mdset_(const char *testname, int param, bool match)
{
    TestMdset::run(*this, testname, param, match);
}


void TestModel::test_pcm_(const char *testname, int param, int fr_idx, bool single_channel,
                          bool match, bool apply_updates)
{
    TestPcm::run(*this, testname, param, pcm_single_frame_,
                 FRAME_RATES[fr_idx], single_channel, minimal_check_, match,
                 apply_updates,
                 /* don't try random access testing when we only have one frame of data! */
                 pcm_single_frame_ ? 0 : pcm_skip_samples_);
}


