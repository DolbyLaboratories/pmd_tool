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
 * @file TestKlv.hh
 * @brief encapsulate KLV read/write testing
 */

extern "C"
{
#include "dlb_pmd_api.h"
#include "dlb_pmd_klv.h"
}

#include "TestModel.hh"


class TestKlv
{
public:

    static void run(TestModel& m, const char *testname, int param, bool match);

private:

    TestKlv();
    ~TestKlv();

    dlb_pmd_success write(dlb_klvpmd_universal_label ul, dlb_pmd_model *m);
    dlb_pmd_success read(dlb_pmd_model *m);
    void dump(const char *filename);

    uint8_t *mem_;
    size_t size_;
};


