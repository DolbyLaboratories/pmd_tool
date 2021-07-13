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
 * @file TestSadm.cc
 * @brief encapsulate serial ADM XML read/write testing
 */

extern "C"
{
#include "dlb_pmd_sadm_string.h"
#include "src/modules/sadm/pmd_sadm_limits.h"
}


#include "TestSadm.hh"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


namespace
{
    static const int MAX_SADM_SIZE = 100 * 1024 * 1024;
    /* minimize the number of error files written; we don't want to risk flooding
     * the disk when there are many failures */
    static const unsigned int MAX_FILES_WRITTEN = 16;
    static unsigned int files_written = 0;
}


void TestSadm::run(TestModel& m1, const char *testname, int param, bool match)
{
    const char *errmsg = NULL;
    TestModel m2;
    TestModel m3;
    TestSadm sadm(m1);

    if (sadm.write(m1))
    {
        errmsg = "could not write SADM";
    }
    else if (sadm.read(m2))
    {
        errmsg = sadm.error_message_;
    }
    else if ((dlb_pmd_success)match == dlb_pmd_equal(m1, m2, 0, 0))
    {
        errmsg = match
            ? "Model mismatch after SADM write and read"
            : "Models shouldn't match after SADM write and read, but do";
    }
    else if (sadm.write(m2))
    {
        errmsg = "could not write SADM 2";        
    }
    else if (sadm.read(m3))
    {
        errmsg = sadm.error_message_;
    }
    else if ((dlb_pmd_success)match == dlb_pmd_equal(m1, m3, 0, 0))
    {
        errmsg = match
            ? "Model mismatch after SADM2 write and read"
            : "Models shouldn't match after SADM2 write and read, but do";
    }
    
    if (errmsg)
    {
        std::string error = errmsg;
        if (files_written < MAX_FILES_WRITTEN)
        {
            char outfile[128];    
            snprintf(outfile, sizeof(outfile), "test_%s_%d_sadm.xml", testname, param);
            sadm.dump(outfile);
            error += ": ";
            error += outfile;
            ++files_written;            
        }
        throw TestModel::failure(error);
    }
}


TestSadm::TestSadm(TestModel& m)
    : sadm_(0)
    , smem_(0)
    , bmem_(0)
    , size_(0)
    , error_line_(0)
{
    dlb_pmd_model_constraints limits;
    dlb_sadm_counts sc;
    size_t sz;
    
    dlb_pmd_get_constraints(m, &limits);
    compute_sadm_limits(&limits, &sc);
    sz = dlb_sadm_query_memory(&sc);

    size_= MAX_SADM_SIZE;
    bmem_ = new uint8_t[size_];
    smem_ = new uint8_t[sz];

    if (dlb_sadm_init(&sc, smem_, &sadm_))
    {
        std::string error = "could not init sADM model";
        throw TestModel::failure(error);
    }
}


TestSadm::~TestSadm()
{
    dlb_sadm_finish(sadm_);
    sadm_ = 0;
    delete[] bmem_;
    delete[] smem_;
}

    
int TestSadm::get_write_buf_(char *pos, char **buf, size_t *capacity)
{
    if (!buf)
    {
        /* writer is closing, so determine actual size used */
        size_ = pos - (char*)bmem_;
    }
    else if (!pos)
    {
        *buf = (char*)bmem_;
        *capacity = size_;
        return 1;
    }
    return 0;
}


void TestSadm::error_callback_(const char *msg)
{
    snprintf(error_message_, sizeof(error_message_),
             "Could not read SADM: %s at line %u", msg, error_line_);
}


dlb_pmd_success TestSadm::write(TestModel& m)
{
    dlb_pmd_model_constraints limits;
    dlb_pmd_sadm_writer *w;
    uint8_t *mem;
    size_t wsz;

    dlb_pmd_get_constraints(m, &limits);
    wsz = dlb_pmd_sadm_writer_query_mem(&limits);
    mem = new uint8_t[wsz];
    size_ = MAX_SADM_SIZE;

    dlb_pmd_success res
        =  dlb_pmd_sadm_writer_init(&w, &limits, mem)
        || dlb_pmd_sadm_string_write(w, m, (char*)bmem_, &size_);
    
    dlb_pmd_sadm_writer_finish(w);
    delete[] mem;
    return res;
}

                
dlb_pmd_success TestSadm::read(TestModel& m)
{
    dlb_pmd_model_constraints limits;
    dlb_pmd_sadm_reader *r;    
    dlb_pmd_success res;
    uint8_t *mem;
    size_t rsz;

    dlb_pmd_get_constraints(m, &limits);
    rsz = dlb_pmd_sadm_reader_query_mem(&limits);
    mem = new uint8_t[rsz];
    res =  dlb_pmd_sadm_reader_init(&r, &limits, mem)
        || dlb_pmd_sadm_string_read(r, "TestSadm", (char*)bmem_, size_, m, error_callback, this, &error_line_)
        ;
        
    dlb_pmd_sadm_reader_finish(r);
    delete[] mem;
    return res;
}

    
void TestSadm::dump(const char *filename)
{
    FILE *f = fopen(filename, "wb");
    if (NULL != f)
    {
        fwrite(bmem_, 1, size_, f);
        fclose(f);
    }
}
