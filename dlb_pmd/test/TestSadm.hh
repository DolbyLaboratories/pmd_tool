/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020, Dolby Laboratories Inc.
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
 * @file TestSadm.hh
 * @brief encapsulate serial ADM XML read/write testing
 */

extern "C"
{
#include "dlb_pmd_api.h"
#include "sadm/dlb_sadm_model.h"
}

#include "TestModel.hh"


class TestSadm
{
public:

    static void run(TestModel& m, const char *testname, int param, bool match);

private:

    TestSadm(TestModel&);
    ~TestSadm();

    dlb_pmd_success write(TestModel&);
    dlb_pmd_success read(TestModel&);
    
    void dump(const char *filename);

    int get_write_buf_(char *pos, char **buf, size_t *capacity);
    void error_callback_(const char *msg);

    static void error_callback(const char *msg, void *arg)
    {
        TestSadm *test = static_cast<TestSadm*>(arg);
        test->error_callback_(msg);
    }

    static int sadm_writer_get_buffer_callback
        (void *arg
        ,char *pos
        ,char **buf
        ,size_t *capacity
        )
    {
        TestSadm *test = static_cast<TestSadm*>(arg);
        return test->get_write_buf_(pos, buf, capacity);
    }

    dlb_sadm_model *sadm_;
    uint8_t *smem_;
    uint8_t *bmem_;
    size_t size_;
    char error_message_[256];
    unsigned int error_line_;
};

    
