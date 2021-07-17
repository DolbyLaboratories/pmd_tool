/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019-2021 by Dolby Laboratories,
 *                Copyright (C) 2019-2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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
