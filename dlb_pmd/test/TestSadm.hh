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
 * @file TestSadm.hh
 * @brief encapsulate serial ADM XML read/write testing
 */


#include "TestModel.hh"
#include "dlb_pmd/src/modules/sadm/pmd_core_model_generator.h"
#include "dlb_pmd/src/modules/sadm/pmd_core_model_ingester.h"
#include "dlb_adm/include/dlb_adm_api_types.h"


class TestSadm
{
public:

    static void run(TestModel& m, const char *testname, int param, bool match);

private:

    TestSadm(TestModel&);
    ~TestSadm();

    void check_status(int status, const char *msg);

    dlb_pmd_success write(TestModel&);
    dlb_pmd_success read(TestModel&);
    
    void dump(const char *filename);

    int get_write_buf_(char *pos, char **buf, size_t *capacity);
    void error_callback_(const char *msg);

    void close_all();

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
        TestSadm *test = reinterpret_cast<TestSadm*>(arg);
        return test->get_write_buf_(pos, buf, capacity);
    }

    dlb_adm_core_model_counts cmcnt_;
    dlb_adm_container_counts ccnt_;
    dlb_adm_core_model *sadm_;
    dlb_adm_xml_container *xmlc_;
    pmd_core_model_generator *genc_;
    pmd_core_model_ingester *ingc_;
    uint8_t *smem_;
    uint8_t *bmem_;
    uint8_t *xmem_;
    uint8_t *gmem_;
    uint8_t *imem_;
    size_t size_;
    size_t used_;
    char error_message_[256];
    unsigned int error_line_;
};
