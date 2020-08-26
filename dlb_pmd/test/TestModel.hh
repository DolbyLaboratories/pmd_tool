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
 * @file TestModel.hh
 * @brief encapsulate control of PMD models for testing
 */

#ifndef DLB_PMD_TEST_MODEL_HH
#define DLB_PMD_TEST_MODEL_HH

extern "C"
{
#include "dlb_pmd_api.h"
#include <stdint.h>
}

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
class TestModel
{
public:

    struct failure
    {
        failure(std::string& msg): msg(msg) {}
        std::string msg;
    };
    
    TestModel();
    TestModel(dlb_pmd_model_constraints&);
    TestModel(unsigned int profile, unsigned int level);
    
    ~TestModel();

    TestModel& operator= (const TestModel&);

    enum TestType
    {
        TEST_XML,
        TEST_MDSET,
        TEST_KLV,

        TEST_PCM_PAIR_2398,
        TEST_PCM_CHAN_2398,

        TEST_PCM_PAIR_2400,
        TEST_PCM_CHAN_2400,

        TEST_PCM_PAIR_2500,
        TEST_PCM_CHAN_2500,

        TEST_PCM_PAIR_2997,
        TEST_PCM_CHAN_2997,

        TEST_PCM_PAIR_3000,
        TEST_PCM_CHAN_3000,

        TEST_PCM_PAIR_5000,
        TEST_PCM_CHAN_5000,

        TEST_PCM_PAIR_5994,
        TEST_PCM_CHAN_5994,

        TEST_PCM_PAIR_6000,
        TEST_PCM_CHAN_6000,

        TEST_PCM_PAIR_10000,
        TEST_PCM_CHAN_10000,

        TEST_PCM_PAIR_11988,
        TEST_PCM_CHAN_11988,

        TEST_PCM_PAIR_12000,
        TEST_PCM_CHAN_12000,

        NUM_PMD_TEST_TYPES,

        TEST_SADM = NUM_PMD_TEST_TYPES,

        TEST_SADM_PCM_PAIR_2398,
        TEST_SADM_PCM_CHAN_2398,
        TEST_SADM_PCM_PAIR_2400,
        TEST_SADM_PCM_CHAN_2400,
        TEST_SADM_PCM_PAIR_2500,
        TEST_SADM_PCM_CHAN_2500,
        TEST_SADM_PCM_PAIR_2997,
        TEST_SADM_PCM_CHAN_2997,
        TEST_SADM_PCM_PAIR_3000,
        TEST_SADM_PCM_CHAN_3000,

        NUM_TOTAL_TEST_TYPES
    };

    static const TestModel::TestType TEST_TYPES[];
    static const int FIRST_TEST_TYPE = (int)TEST_XML;
    static const int LAST_TEST_TYPE = NUM_PMD_TEST_TYPES - 1;

    static const unsigned int NUM_FRAME_RATE_NAMES = 11;
    static const dlb_pmd_frame_rate FRAME_RATES[NUM_FRAME_RATE_NAMES];
    static const char* FRAME_RATE_NAMES[NUM_FRAME_RATE_NAMES];    

    /**
     * @brief force PCM test on single frame
     *
     * By default PCM testing does not check updates: after the first
     * frame, the model should equal the result of all those updates.
     * It shouldn't be continually resetting to initial state and then
     * updating again.
     *
     * If we actually want to test for number of updates, then we must
     * restrict the test to one frame (rather than one second which we
     * normally test for PCM).
     */
    void pcm_single_frame() { pcm_single_frame_ = true; };

    /**
     * @brief cut down comparison to barest essentials
     *
     * When the number of objects and elements grows large, we may not
     * have room for every other payload in every frame, so we can't
     * guarantee that the models will be completely equal. In such
     * cases, we console ourselves by making sure that the beds,
     * objects and presentations are equal.  This is the minimum
     * information required to produce a desired render.
     */
    void minimal_check() { minimal_check_ = true; }


    /**
     * @brief ignore name checking in model comparison
     *
     * Because names are low-priority payloads, they may not all occur
     * after model read/write, especially in high-frame-rate PCM.
     * This flag simply allows us to compare everything but names.
     */
    void ignore_name_checking()  { ignore_name_check_ = true; }


    /**
     * @brief enable random-access PCM+PMD testing
     *
     * We want to make sure that our PCM PMD extractor works when it
     * is presented with an arbitrary PCM stream, perhaps one that
     * has been captured live. This means that we cannot allow the
     * algorithm to rely on being presented with the very first sample
     * that the PCM augmentor produced.
     *
     * This function tells the test to start extracting #s samples
     * from the beginning of the stream.
     *
     * This value is ignored when we are not doing PCM+PMD testing,
     * and when we are only testing single frame decode.  (If we try
     * to extract from a random access point inside such a stream, we
     * won't get any data!)
     *
     * This value is rounded modulo 4096 (which is larger than the
     * largest supported frame size), allowing a max of 2 frames to be
     * skipped dependending on the frame rate.
     */
    void skip_pcm_samples(unsigned int s) { pcm_skip_samples_ = s % 4096; }

    /**
     * @brief run a test. The models must match if and only if
     * the #match parameter is true.  If the #apply_updates flag
     * is true, the match is performed after the original model
     * has had all of its updates applied.
     */
    void test(TestType type, const char *testname, int param, bool apply_updates=true)
    {
        test_(type, testname, param, true, apply_updates);
    }
    
    /**
     * @brief run a test. The models must not match
     */
    void negtest(TestType type, const char *testname, int param, bool apply_updates=true)
    {
        test_(type, testname, param, false, apply_updates);
    }
    
    operator dlb_pmd_model*() const { return model_; }
    dlb_pmd_element_id      new_elid()   { return (dlb_pmd_element_id)++max_element_id_; }
    dlb_pmd_presentation_id new_presid() { return (dlb_pmd_presentation_id)++max_pres_id_; }
    unsigned int            new_eac3id() { return ++max_eac3_id_; }
    unsigned int            new_etdid()  { return ++max_etd_id_; }
    
    /**
     * @brief set the model version
     *
     * Normally it should be impossible to set the version of PMD
     * bitstream, because it is baked into the code (and has to be).
     *
     * However, to test the veracity of version checking, we provide
     * an illegal version setter function.
     */
    void illegal_set_version(uint8_t maj, uint8_t min);

    /**
     * @brief populate the model with a fixed test set
     *
     * @param obj returns the id of the first object
     */
    bool populate (dlb_pmd_element_id *obj);

    /**
     * @brief set the ED2 system info to that of another
     */
    void set_ed2_system (const TestModel& other);
    
    /**
     * @brief apply updates to the model
     */
    bool apply_updates(dlb_pmd_frame_rate fr, unsigned int num_frames = 1);

    /**
     * @brief generate random model
     */
    void generate_random(unsigned int seed, bool sadm = false,
                         bool prune_unused_signals = false);

private:

    void test_(TestType type, const char *testname, int param, bool match, bool apply_updates);

    void test_xml_  (const char *testname, int param, bool match);
    void test_mdset_(const char *testname, int param, bool match);
    void test_klv_  (const char *testname, int param, bool match);
    void test_sadm_ (const char *testname, int param, bool match);
    void test_pcm_  (const char *testname, int param, int fr_idx, bool single_channel, bool match, bool apply_updates, bool sadm);

    size_t size_;
    std::string mem_;
    dlb_pmd_model *model_;
    unsigned int pcm_skip_samples_;
    bool pcm_single_frame_;
    bool minimal_check_;
    bool ignore_name_check_;
    unsigned int max_sig_id_;
    unsigned int max_track_id_;
    unsigned int max_element_id_;
    unsigned int max_pres_id_;
    unsigned int max_eac3_id_;
    unsigned int max_etd_id_;
};


#endif // DLB_PMD_TEST_MODEL_HH

