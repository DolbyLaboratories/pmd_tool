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

#include "dlb_adm/include/dlb_adm_api.h"

#include "TestSadm.hh"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


namespace
{
    static const size_t MAX_SADM_SIZE = 100 * 1024 * 1024;
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
    else if (((dlb_pmd_success)match) == dlb_pmd_equal(m1, m2, 0, 0))
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
    else if (((dlb_pmd_success)match) == dlb_pmd_equal(m1, m3, 0, 0))
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


TestSadm::TestSadm(TestModel&)
    : sadm_(0)
    , xmlc_(0)
    , genc_(0)
    , ingc_(0)
    , smem_(0)
    , bmem_(0)
    , xmem_(0)
    , gmem_(0)
    , imem_(0)
    , size_(0)
    , used_(0)
    , error_line_(0)
{
    dlb_pmd_success success;
    size_t sz;

    size_ = MAX_SADM_SIZE;
    bmem_ = new uint8_t[size_];

#ifdef WHEN_WE_HAVE_QUERY_MEM
    int status;

    memset(&cmcnt_, 0, sizeof(cmcnt_));
    status = dlb_adm_core_model_query_memory_size(&sz, &cmcnt_);
    check_status(status, "dlb_adm_core_model_query_memory_size() failed");
    smem_ = new uint8_t[sz];
    
    memset(&ccnt_, 0, sizeof(ccnt_));
    status = dlb_adm_container_query_memory_size(&sz, &ccnt_);
    check_status(status, "dlb_adm_container_query_memory_size() failed");
    xmem_ = new uint8_t[sz];
#endif

    success = pmd_core_model_generator_query_memory_size(&sz);
    check_status(success, "pmd_core_model_generator_query_memory_size() failed");
    gmem_ = new uint8_t[sz];

    success = pmd_core_model_ingester_query_memory_size(&sz);
    check_status(success, "pmd_core_model_ingester_query_memory_size() failed");
    imem_ = new uint8_t[sz];

    if (/*!smem_ ||*/ !bmem_ || /*!xmem_ ||*/ !gmem_ || !imem_)
    {
        std::string error = "out of memory";
        throw TestModel::failure(error);
    }
}


TestSadm::~TestSadm()
{
    close_all();

    delete[] imem_;
    delete[] gmem_;
    delete[] xmem_;
    delete[] bmem_;
    delete[] smem_;
}


void TestSadm::check_status(int status, const char *msg)
{
    if (status)
    {
        std::string error = msg;
        throw TestModel::failure(error);
    }
}

    
int TestSadm::get_write_buf_(char *pos, char **buf, size_t *capacity)
{
    if (!buf)
    {
        /* writer is closing, so determine actual size used */
        used_ = pos - (char*)bmem_;
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
             "Could not read S-ADM: %s at line %u", msg, error_line_);
}


void TestSadm::close_all()
{
    if (ingc_ != nullptr)
    {
        (void)pmd_core_model_ingester_close(&ingc_);
    }
    if (genc_ != nullptr)
    {
        (void)pmd_core_model_generator_close(&genc_);
    }
    if (xmlc_ != nullptr)
    {
        (void)dlb_adm_container_close(&xmlc_);
    }
    if (sadm_ != nullptr)
    {
        (void)dlb_adm_core_model_close(&sadm_);
    }
}


dlb_pmd_success TestSadm::write(TestModel& m)
{
    // This function writes a PMD model to a S-ADM string

    dlb_pmd_success success;
    int status;

    status = dlb_adm_core_model_open(&sadm_, &cmcnt_);
    check_status(status, "dlb_adm_core_model_open() failed");

    success = pmd_core_model_generator_open(&genc_, gmem_);
    check_status(success, "pmd_core_model_generator_open() failed");
    success = pmd_core_model_generator_generate(genc_, sadm_, m);
    check_status(success, "pmd_core_model_generator_generate() failed");

    status = dlb_adm_container_open_from_core_model(&xmlc_, sadm_);
    check_status(status, "dlb_adm_container_open_from_core_model() failed");
    status = dlb_adm_container_write_xml_buffer(xmlc_, sadm_writer_get_buffer_callback, this);
    check_status(status, "dlb_adm_container_write_xml_buffer() failed");

#if 0
    dump("write.xml");
#endif

    close_all();

    return PMD_SUCCESS;
}

                
dlb_pmd_success TestSadm::read(TestModel& m)
{
    // This function reads a S-ADM string and converts it to a PMD model

    dlb_pmd_success result = PMD_SUCCESS;

    if (dlb_adm_container_open(&xmlc_, &ccnt_)                                                                  ||
        dlb_adm_container_read_xml_buffer(xmlc_, reinterpret_cast<const char *>(bmem_), used_, DLB_ADM_FALSE)   ||
        dlb_adm_core_model_open_from_xml_container(&sadm_, xmlc_)                                               ||
        pmd_core_model_ingester_open(&ingc_, imem_)                                                             ||
        pmd_core_model_ingester_ingest(ingc_, m, "TestSadm", sadm_)
        )
    {
        result = PMD_FAIL;
    }

    return result;
}

    
void TestSadm::dump(const char *filename)
{
    FILE *f = fopen(filename, "wb");
    if (NULL != f)
    {
        fwrite(bmem_, 1, used_, f);
        fclose(f);
    }
}
