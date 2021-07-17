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


