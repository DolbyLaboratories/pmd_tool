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

