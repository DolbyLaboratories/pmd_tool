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
 * @file TestKlv.cc
 * @brief encapsulate KLV read/write testing
 */

#include "TestKlv.hh"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


namespace
{
    static const int MAX_KLV_SIZE = 1024 * 1024;
    /* minimize the number of error files written; we don't want to risk flooding
     * the disk when there are many failures */
    static const unsigned int MAX_FILES_WRITTEN = 16;
    static unsigned int files_written = 0;        
}


void TestKlv::run(TestModel& m1, const char *testname, int param, bool match)
{
    dlb_klvpmd_universal_label ul = (dlb_klvpmd_universal_label)(param & 1);
    const char *errmsg = NULL;
    TestModel m2;
    TestModel m3;
    TestKlv klv;
    
    if (klv.write(ul, m1))
    {
        errmsg = "could not write KLV";
    }
    else if (klv.read(m2))
    {
        errmsg = "could not read KLV";
    }
    else if ((dlb_pmd_success)match == dlb_pmd_equal(m1, m2, 0, 0))
    {
        errmsg = match
            ? "Model mismatch after KLV write and read"
            : "Models shouldn't match after KLV write and read, but do";
    }
    else if (klv.write(ul, m2))
    {
        errmsg = "could not write KLV2";
    }
    else if (klv.read(m3))
    {
        errmsg = "could not readk KLV2";
    }
    else if ((dlb_pmd_success)match == dlb_pmd_equal(m1, m3, 0, 0))
    {
        errmsg = match
            ? "Model mismatch after KLV2 write and read"
            : "Models shouldn't match after KLV2 write and read, but do";
    }

    if (errmsg)
    {
        std::string error = errmsg;
        if (files_written < MAX_FILES_WRITTEN)
        {
            char outfile[128];    
            snprintf(outfile, sizeof(outfile), "test_%s_%d.klv", testname, param);
            klv.dump(outfile);
            ++files_written;
            error += ": ";
            error += outfile;
        }
        throw TestModel::failure(error);
    }
}


TestKlv::TestKlv()
    : mem_(0)
    , size_(0)
{
    mem_ = new uint8_t[MAX_KLV_SIZE];
}


TestKlv::~TestKlv()
{
    delete[] mem_;
}

    
dlb_pmd_success TestKlv::write(dlb_klvpmd_universal_label ul, dlb_pmd_model *model)
{
    size_ = dlb_klvpmd_write_all(model, DLB_PMD_NO_ED2_STREAM_INDEX, mem_, MAX_KLV_SIZE, ul);
    return (0 == size_)
        ? (dlb_pmd_success)PMD_FAIL
        : (dlb_pmd_success)PMD_SUCCESS;
}

                
dlb_pmd_success TestKlv::read(dlb_pmd_model *model)
{
    if (dlb_klvpmd_read_payload(mem_, size_, model, 1, NULL, NULL))
    {
        return PMD_FAIL;
    }
    return PMD_SUCCESS;
}

    
void TestKlv::dump(const char *filename)
{
    FILE *f = fopen(filename, "wb");
    if (NULL != f)
    {
        fwrite(mem_, 1, size_, f);
        fclose(f);
    }
}
