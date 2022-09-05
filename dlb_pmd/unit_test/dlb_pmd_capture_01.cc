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

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "gtest/gtest.h"

#include "dlb_pmd/include/dlb_pmd_capture.h"
#include "dlb_pmd/include/dlb_pmd_pcm.h"

#include <string.h>

class DlbPmdCapture01 : public testing::Test
{
protected:

    dlb_pmd_blob_descriptor  theBlobDescriptor;
    dlb_pmd_frame_captor    *theCaptor;
    char                    *captorMemory;

    bool AllocateCaptorMemory(size_t sz)
    {
        bool allocated = false;

        if (captorMemory == nullptr)
        {
            captorMemory = new char[sz];
            allocated = (captorMemory != nullptr);
        }

        return allocated;
    }

    bool AllocateCaptorMemory()
    {
        size_t sz;
        int status = dlb_pmd_frame_captor_query_memory_size(&sz);
        return (status == DLB_PMD_FRAME_CAPTOR_STATUS_OK && AllocateCaptorMemory(sz));
    }

    int OpenFrameCaptor()
    {
        bool ok;
        int status;
        size_t sz;

        status = dlb_pmd_frame_captor_query_memory_size(&sz);
        if (status != DLB_PMD_FRAME_CAPTOR_STATUS_OK)
        {
            return status;
        }

        ok = AllocateCaptorMemory(sz);
        if (!ok)
        {
            return DLB_PMD_FRAME_CAPTOR_STATUS_OUT_OF_MEMORY;
        }

        status = dlb_pmd_frame_captor_open(&theCaptor, captorMemory);
        return status;
    }

    virtual void SetUp()
    {
        ::memset(&theBlobDescriptor, 0, sizeof(theBlobDescriptor));
        theBlobDescriptor.number_of_samples = 5000;
        theBlobDescriptor.number_of_channels = 16;
        theBlobDescriptor.bit_depth = 24;
        theBlobDescriptor.big_endian = 0;

        theCaptor = nullptr;
        captorMemory = nullptr;
    }

    virtual void TearDown()
    {
        if (theCaptor != nullptr)
        {
            (void)dlb_pmd_frame_captor_close(&theCaptor);
        }
        if (captorMemory != nullptr)
        {
            delete[] captorMemory;
            captorMemory = nullptr;
        }
    }

};

TEST(pmd_capture_test, QueryMem)
{
    int status;
    size_t sz;

    status = dlb_pmd_frame_captor_query_memory_size(nullptr);
    EXPECT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_NULL_POINTER, status);
    status = dlb_pmd_frame_captor_query_memory_size(&sz);
    EXPECT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_OK, status);
    EXPECT_LT(0u, sz);
}

TEST_F(DlbPmdCapture01, OpenCloseBasic)
{
    dlb_pmd_frame_captor *null_captor = nullptr;
    int status;

    status = dlb_pmd_frame_captor_open(nullptr, nullptr);
    EXPECT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_NULL_POINTER, status);

    status = dlb_pmd_frame_captor_close(nullptr);
    EXPECT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_NULL_POINTER, status);
    status = dlb_pmd_frame_captor_close(&null_captor);
    EXPECT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_NULL_POINTER, status);

    status = dlb_pmd_frame_captor_open(&theCaptor, nullptr);
    EXPECT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_OK, status);
    status = dlb_pmd_frame_captor_close(&theCaptor);
    EXPECT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_OK, status);
    EXPECT_EQ(static_cast<dlb_pmd_frame_captor *>(nullptr), theCaptor);
    status = dlb_pmd_frame_captor_close(&theCaptor);
    EXPECT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_NULL_POINTER, status);

    bool ok = AllocateCaptorMemory();
    ASSERT_TRUE(ok);

    status = dlb_pmd_frame_captor_open(&theCaptor, captorMemory);
    EXPECT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_OK, status);
    status = dlb_pmd_frame_captor_close(&theCaptor);
    EXPECT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_OK, status);
    EXPECT_EQ(static_cast<dlb_pmd_frame_captor *>(nullptr), theCaptor);
    status = dlb_pmd_frame_captor_close(&theCaptor);
    EXPECT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_NULL_POINTER, status);
};

TEST_F(DlbPmdCapture01, CaptureBadArgs)
{
    dlb_pmd_metadata_set *metadata_set = nullptr;
    dlb_pmd_blob_descriptor descriptor;
    char buffer[128];
    int status;

    status = dlb_pmd_frame_captor_open(&theCaptor, nullptr);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), status);
    ::memset(&descriptor, 0, sizeof(descriptor));

    status = dlb_pmd_frame_captor_capture(nullptr, nullptr, nullptr, nullptr);
    EXPECT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_NULL_POINTER, status);
    status = dlb_pmd_frame_captor_capture(&metadata_set, nullptr, nullptr, nullptr);
    EXPECT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_NULL_POINTER, status);
    status = dlb_pmd_frame_captor_capture(&metadata_set, theCaptor, nullptr, nullptr);
    EXPECT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_NULL_POINTER, status);

    status = dlb_pmd_frame_captor_capture(&metadata_set, theCaptor, &theBlobDescriptor, nullptr);
    EXPECT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_NULL_POINTER, status);
    status = dlb_pmd_frame_captor_capture(&metadata_set, theCaptor, &descriptor, buffer);
    EXPECT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_ERROR, status);

    descriptor = theBlobDescriptor;
    descriptor.number_of_samples = static_cast<uint16_t>(dlb_pcmpmd_min_frame_size(DLB_PMD_FRAMERATE_LAST) - 1);
    status = dlb_pmd_frame_captor_capture(&metadata_set, theCaptor, &descriptor, buffer);
    EXPECT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_ERROR, status);

    descriptor = theBlobDescriptor;
    descriptor.number_of_channels = 0;
    status = dlb_pmd_frame_captor_capture(&metadata_set, theCaptor, &descriptor, buffer);
    EXPECT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_ERROR, status);

    descriptor = theBlobDescriptor;
    descriptor.bit_depth = 99;
    status = dlb_pmd_frame_captor_capture(&metadata_set, theCaptor, &descriptor, buffer);
    EXPECT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_ERROR, status);
}
