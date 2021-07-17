/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2018-2020 by Dolby Laboratories,
 *                Copyright (C) 2018-2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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

    
