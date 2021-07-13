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

/**
 * @file TestPcm.hh
 * @brief encapsulate PCM read/write testing
 */

#include "dlb_pmd_api.h"
#include "TestModel.hh"

#include <string>

// If defined, record Pa positions (primarily useful for debugging)
//#define PA_POS
#ifdef PA_POS
#include <vector>
#endif


class TestPcm
{
public:

    static void run(TestModel& m1, const char *testname, int param,
                    bool single_frame, dlb_pmd_frame_rate fr, bool single_channel,
                    bool minimal, bool nonames, bool match, bool apply_updates,
                    bool sadm, unsigned int num_skip_samples);

private:

    TestPcm(dlb_pmd_frame_rate fr, bool check_updates);
    ~TestPcm();

    dlb_pmd_success write(bool ispair, dlb_klvpmd_universal_label ul, dlb_pmd_model *m, bool sadm);
    dlb_pmd_success validate();
    dlb_pmd_success read(dlb_pmd_model *m, unsigned int num_skip_samples, unsigned int *num_frames);
    void dump(const char *filename);

    bool verify_preamble_(uint32_t pa, uint32_t pb, uint32_t pc, uint32_t pd, unsigned int maxbits);

    dlb_pmd_frame_rate fr_;
    bool ispair_;
    size_t num_samples_;
    uint32_t *mem_;

#ifdef PA_POS
    void record_pa_positions_();

    std::vector<uint16_t> pa_positions_;
#endif
};

    
