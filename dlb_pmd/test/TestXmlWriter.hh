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
 * @file TestXmlWriter.hh
 * @brief utility for writing XML strings to be ingested
 */

#include "dlb_pmd_xml_string.h"
#include "TestModel.hh"
#include <cstring>
#include <cstdio>

class XMLTestWriter
{
  public:

    XMLTestWriter()
        : error_line_(0)
        , capacity_(sizeof(mem_))
        , wp_(mem_)
        , len_(0)
        , errp_(error_message_)
        , error_capacity_(sizeof(error_message_)-1)
    {
        error_message_[0] = '\0';
    }


    bool ingest(TestModel& m)
    {
        memset(error_message_, '\0', sizeof(error_message_));
        errp_ = error_message_;
        error_capacity_ = sizeof(error_message_)-1;
        return !dlb_xmlpmd_string_read(mem_, len_, m, DLB_PMD_XML_STRICT,
                                       error_callback_, this, &error_line_);
    }

    const char *error()
    {
        return error_message_;
    }
    

    friend XMLTestWriter& operator<<(XMLTestWriter& w, const char *s);

  private:
    
    static void error_callback_(const char *msg, void *arg)
    {
        char tmp[256];
        size_t len;

        XMLTestWriter *w = static_cast<XMLTestWriter*>(arg);
        snprintf(tmp, sizeof(tmp),
                 "ERROR (line %u): Could not read XML: %s\n", w->error_line_, msg);
        strncat(w->errp_, tmp, w->error_capacity_);
        len = strlen(tmp);
        w->errp_ += len;
        w->error_capacity_ -= len;
    }

    unsigned int error_line_;
    char mem_[4096];
    size_t capacity_;
    char *wp_;
    int len_;
    char error_message_[1024];
    char *errp_;
    size_t error_capacity_;
};


XMLTestWriter& operator<<(XMLTestWriter& w, const char *s)
{
    int len = snprintf(w.wp_, w.capacity_, "%s", s);
    w.wp_ += len;
    w.len_ += len;
    w.capacity_ -= len;
    return w;
}


