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
 * @file TestMdset.cc
 * @brief encapsulate Metadata set generation/ingestion testing
 */

extern "C"
{
#include "dlb_pmd_klv.h"
}


#include "TestMdset.hh"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


void TestMdset::run(TestModel& m1, const char *testname, int param, bool match)
{
    dlb_pmd_metadata_set *mdset;
    size_t sz = dlb_pmd_metadata_set_query_memory(m1);
    void *mem = malloc(sz);
    std::string error;

    (void)param;
    (void)testname;

    TestModel m2;
    if (dlb_pmd_create_metadata_set(m1, mem, &mdset))
    {
        error = "Could not create Metadata Set";
        throw TestModel::failure(error);
    }
    else if (dlb_pmd_ingest_metadata_set(m2, mdset))
    {
        error = "Could not ingest Metadata Set";
        throw TestModel::failure(error);
    }
    else if ((dlb_pmd_success)match == dlb_pmd_equal(m1, m2, 0, 0))
    {
        if (match)
            error = "Model mismatch after Metadata set creation and ingestion";
        else
            error = "Models shouldn't match after Metadata set creation and ingestions, but do";
        throw TestModel::failure(error);
    }

    free(mem);
}

