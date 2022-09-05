/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020 - 2022, Dolby Laboratories Inc.
 * Copyright (c) 2022, Dolby International AB.
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
 * @file TestXml.cc
 * @brief encapsulate XML read/write testing
 */

extern "C"
{
#include "dlb_pmd_xml.h"
#include "dlb_pmd_xml_string.h"
#include "XmlSchema.hh"
}


#include "TestXml.hh"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


namespace
{
    static const int MAX_XML_SIZE = 100 * 1024 * 1024;
    /* minimize the number of error files written; we don't want to risk flooding
     * the disk when there are many failures */
    static const unsigned int MAX_FILES_WRITTEN = 16;
    static unsigned int files_written = 0;
}


void TestXml::run(TestModel& m1, const char *testname, int param, bool match)
{
    const char *errmsg = NULL;
    TestModel m2;
    TestModel m3;
    TestXml xml;

    if (xml.write(m1))
    {
        errmsg = "could not write XML";
    }
    else if (xml.validate())
    {
        errmsg = "XML doesn't match schema";
    }
    else if (xml.read(m2))
    {
        errmsg = xml.error_message_;
    }
    else if ((dlb_pmd_success)match == dlb_pmd_equal(m1, m2, 0, 0))
    {
        errmsg = match
            ? "Model mismatch after XML write and read"
            : "Models shouldn't match after XML write and read, but do";
    }
    else if (xml.write(m2))
    {
        errmsg = "could not write XML 2";        
    }
    else if (xml.validate())
    {
        errmsg = "XML 2 doesn't match schema";
    }
    else if (xml.read(m3))
    {
        errmsg = xml.error_message_;
    }
    else if ((dlb_pmd_success)match == dlb_pmd_equal(m1, m3, 0, 0))
    {
        errmsg = match
            ? "Model mismatch after XML2 write and read"
            : "Models shouldn't match after XML2 write and read, but do";
    }
    
    if (errmsg)
    {
        std::string error = errmsg;
        if (files_written < MAX_FILES_WRITTEN)
        {
            char outfile[128];    
            snprintf(outfile, sizeof(outfile), "test_%s_%d.xml", testname, param);
            xml.dump(outfile);
            error += ": ";
            error += outfile;
            ++files_written;            
        }
        throw TestModel::failure(error);
    }
}


TestXml::TestXml()
    : mem_(0)
    , size_(0)
    , error_line_(0)
{
    size_= MAX_XML_SIZE;
    mem_ = new uint8_t[size_];
}


TestXml::~TestXml()
{
    delete[] mem_;
}

    
int TestXml::get_write_buf_(char *pos, char **buf, size_t *capacity)
{
    if (!buf)
    {
        /* writer is closing, so determine actual size used */
        size_ = pos - (char*)mem_;
    }
    else if (!pos)
    {
        *buf = (char*)mem_;
        *capacity = size_;
        return 1;
    }
    return 0;
}


void TestXml::error_callback_(const char *msg)
{
    snprintf(error_message_, sizeof(error_message_),
             "Could not read XML: %s at line %u", msg, error_line_);
}


dlb_pmd_success TestXml::write(TestModel& m)
{
    size_ = MAX_XML_SIZE;
    if (dlb_xmlpmd_write(xml_writer_get_buffer_callback, 0, this, m))
    {
        return PMD_FAIL;
    }
    return PMD_SUCCESS;
}

                
dlb_pmd_success TestXml::validate()
{
    if (XmlSchema::test(mem_, size_))
    {
        return PMD_SUCCESS;
    }
    return PMD_FAIL;
}


dlb_pmd_success TestXml::read(TestModel& m)
{
    return dlb_xmlpmd_string_read((const char*)mem_, size_, m, DLB_PMD_XML_STRICT,
                                  error_callback, this, &error_line_)
        ? (dlb_pmd_success)PMD_FAIL
        : (dlb_pmd_success)PMD_SUCCESS;
}

    
void TestXml::dump(const char *filename)
{
    FILE *f = fopen(filename, "wb");
    if (NULL != f)
    {
        fwrite(mem_, 1, size_, f);
        fclose(f);
    }
}
