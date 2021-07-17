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
 * @file TestMdset.hh
 * @brief encapsulate metadata set generation/ingestion testing
 */

extern "C"
{
#include "dlb_pmd_api.h"
}

#include "TestModel.hh"
#include <string>


class TestMdset
{
public:
    
    static void run(TestModel& m, const char *testname, int param, bool match);
};

    
