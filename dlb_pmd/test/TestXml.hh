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
 * @file TestXml.hh
 * @brief encapsulate XML read/write testing
 */

extern "C"
{
#include "dlb_pmd_api.h"
}

#include "TestModel.hh"


class TestXml
{
public:

    static void run(TestModel& m, const char *testname, int param, bool match);

private:

    TestXml();
    ~TestXml();

    dlb_pmd_success write(TestModel&);
    dlb_pmd_success validate();
    dlb_pmd_success read(TestModel&);
    
    void dump(const char *filename);

    int get_write_buf_(char *pos, char **buf, size_t *capacity);
    void error_callback_(const char *msg);

    static void error_callback(const char *msg, void *arg)
    {
        TestXml *test = static_cast<TestXml*>(arg);
        test->error_callback_(msg);
    }

    static int xml_writer_get_buffer_callback
        (void *arg
        ,char *pos
        ,char **buf
        ,size_t *capacity
        )
    {
        TestXml *test = static_cast<TestXml*>(arg);
        return test->get_write_buf_(pos, buf, capacity);
    }

    uint8_t *mem_;
    size_t size_;
    char error_message_[256];
    unsigned int error_line_;
};

    
