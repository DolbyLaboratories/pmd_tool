/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020, Dolby Laboratories Inc.
 * Copyright (c) 2020, Dolby International AB.
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

extern "C"
{
#include "dlb_wave/include/dlb_wave.h"
}

#include <stdio.h>
#include <string.h>

class DlbPmdCapture02 : public testing::Test
{
protected:
    static const size_t sampleCount = 4200;

    dlb_pmd_blob_descriptor  theBlobDescriptor;
    dlb_pmd_frame_captor    *theCaptor;
    char                    *captorMemory;
    char                    *bufferMemory;

    dlb_wave_file            theWaveFile;
    bool                     waveFileIsOpen;

    FILE                    *theBinaryFile;

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

    bool OpenWaveFile(const char *path)
    {
        int status = dlb_wave_open_read(&theWaveFile, path, nullptr);
        waveFileIsOpen = (status == 0);
        return waveFileIsOpen;
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
        bufferMemory = nullptr;

        ::memset(&theWaveFile, 0, sizeof(theWaveFile));
        waveFileIsOpen = false;

        theBinaryFile = nullptr;
    }

    virtual void TearDown()
    {
        if (theBinaryFile != nullptr)
        {
            ::fclose(theBinaryFile);
        }
        if (waveFileIsOpen)
        {
            dlb_wave_close(&theWaveFile);
            waveFileIsOpen = false;
        }
        if (theCaptor != nullptr)
        {
            (void)dlb_pmd_frame_captor_close(&theCaptor);
        }
        if (captorMemory != nullptr)
        {
            delete[] captorMemory;
            captorMemory = nullptr;
        }
        if (bufferMemory != nullptr)
        {
            delete[] bufferMemory;
            bufferMemory = nullptr;
        }
    }

};

#ifdef _WIN32
TEST_F(DlbPmdCapture02, SliceItChan0)
{
    static const char *testFileName = "D:\\data\\PMD\\PMDLIB-120\\8ch_square_1min_chan0_klv.wav";
    FILE *f = ::fopen(testFileName, "r");

    if (f != nullptr)
    {
        // TODO: sanity checks on the captured metadata set...

        ::fclose(f);

        bool opened;
        int status;

        opened = OpenWaveFile(testFileName);
        ASSERT_TRUE(waveFileIsOpen);

        size_t channelCount = dlb_wave_get_channel_count(&theWaveFile);
        size_t bitDepth = dlb_wave_get_bit_depth(&theWaveFile);
        size_t frameCount = dlb_wave_get_num_frames(&theWaveFile);
        size_t frameByteCount = (bitDepth / CHAR_BIT) * channelCount;
        size_t bufferByteCount = frameByteCount * sampleCount;
        size_t startFrame = (frameCount / 3) - 299;
        size_t readCount;

        bufferMemory = new char[bufferByteCount];
        ASSERT_NE(nullptr, bufferMemory);
        ::memset(bufferMemory, 0, bufferByteCount);

        status = dlb_wave_seek_to_frame(&theWaveFile, startFrame);
        ASSERT_EQ(0, status);
        status = dlb_wave_read_data(&theWaveFile, bufferMemory, bufferByteCount, &readCount);
        ASSERT_EQ(0, status);
        ASSERT_EQ(bufferByteCount, readCount);

        status = OpenFrameCaptor();
        ASSERT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_OK, status);

        dlb_pmd_metadata_set *pmds = nullptr;
        dlb_pmd_blob_descriptor descr;

        ::memset(&descr, 0, sizeof(descr));
        descr.number_of_samples = sampleCount;
        descr.number_of_channels = static_cast<uint16_t>(channelCount);
        descr.bit_depth = static_cast<uint8_t>(bitDepth);
        status = dlb_pmd_frame_captor_capture(&pmds, theCaptor, &descr, bufferMemory);
        ASSERT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_OK, status);
    }
}

TEST_F(DlbPmdCapture02, SliceItPair2)
{
    static const char *testFileName = "D:\\data\\PMD\\PMDLIB-120\\8ch_square_1min_pair2_klv.wav";
    FILE *f = ::fopen(testFileName, "r");

    if (f != nullptr)
    {
        // TODO: sanity checks on the captured metadata set...

        ::fclose(f);

        bool opened;
        int status;

        opened = OpenWaveFile(testFileName);
        ASSERT_TRUE(waveFileIsOpen);

        size_t channelCount = dlb_wave_get_channel_count(&theWaveFile);
        size_t bitDepth = dlb_wave_get_bit_depth(&theWaveFile);
        size_t frameCount = dlb_wave_get_num_frames(&theWaveFile);
        size_t frameByteCount = (bitDepth / CHAR_BIT) * channelCount;
        size_t bufferByteCount = frameByteCount * sampleCount;
        size_t startFrame = (frameCount / 2) + 2;
        size_t readCount;

        bufferMemory = new char[bufferByteCount];
        ASSERT_NE(nullptr, bufferMemory);
        ::memset(bufferMemory, 0, bufferByteCount);

        status = dlb_wave_seek_to_frame(&theWaveFile, startFrame);
        ASSERT_EQ(0, status);
        status = dlb_wave_read_data(&theWaveFile, bufferMemory, bufferByteCount, &readCount);
        ASSERT_EQ(0, status);
        ASSERT_EQ(bufferByteCount, readCount);

        status = OpenFrameCaptor();
        ASSERT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_OK, status);

        dlb_pmd_metadata_set *pmds = nullptr;
        dlb_pmd_blob_descriptor descr;

        ::memset(&descr, 0, sizeof(descr));
        descr.number_of_samples = sampleCount;
        descr.number_of_channels = static_cast<uint16_t>(channelCount);
        descr.bit_depth = static_cast<uint8_t>(bitDepth);
        status = dlb_pmd_frame_captor_capture(&pmds, theCaptor, &descr, bufferMemory);
        ASSERT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_OK, status);
    }
}

TEST_F(DlbPmdCapture02, SliceItChan7)
{
    static const char *testFileName = "D:\\data\\PMD\\PMDLIB-120\\8ch_square_1min_chan7_klv.wav";
    FILE *f = ::fopen(testFileName, "r");

    if (f != nullptr)
    {
        // TODO: sanity checks on the captured metadata set...

        ::fclose(f);

        bool opened;
        int status;

        opened = OpenWaveFile(testFileName);
        ASSERT_TRUE(waveFileIsOpen);

        size_t channelCount = dlb_wave_get_channel_count(&theWaveFile);
        size_t bitDepth = dlb_wave_get_bit_depth(&theWaveFile);
        size_t frameCount = dlb_wave_get_num_frames(&theWaveFile);
        size_t frameByteCount = (bitDepth / CHAR_BIT) * channelCount;
        size_t bufferByteCount = frameByteCount * sampleCount;
        size_t startFrame = ((frameCount * 2) / 3) - 42;
        size_t readCount;

        bufferMemory = new char[bufferByteCount];
        ASSERT_NE(nullptr, bufferMemory);
        ::memset(bufferMemory, 0, bufferByteCount);

        status = dlb_wave_seek_to_frame(&theWaveFile, startFrame);
        ASSERT_EQ(0, status);
        status = dlb_wave_read_data(&theWaveFile, bufferMemory, bufferByteCount, &readCount);
        ASSERT_EQ(0, status);
        ASSERT_EQ(bufferByteCount, readCount);

        status = OpenFrameCaptor();
        ASSERT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_OK, status);

        dlb_pmd_metadata_set *pmds = nullptr;
        dlb_pmd_blob_descriptor descr;

        ::memset(&descr, 0, sizeof(descr));
        descr.number_of_samples = sampleCount;
        descr.number_of_channels = static_cast<uint16_t>(channelCount);
        descr.bit_depth = static_cast<uint8_t>(bitDepth);
        status = dlb_pmd_frame_captor_capture(&pmds, theCaptor, &descr, bufferMemory);
        ASSERT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_OK, status);
    }
}

#if 0
TEST_F(DlbPmdCapture02, SliceItSadm)
{
    // TODO: test signal needs to be regenerated
    static const char *testFileName = "D:\\data\\PMD\\PMDLIB-120\\8ch_square_1min_sadm.wav";
    FILE *f = ::fopen(testFileName, "r");

    if (f != nullptr)
    {
        // TODO: sanity checks on the captured metadata set...

        ::fclose(f);

        bool opened;
        int status;

        opened = OpenWaveFile(testFileName);
        ASSERT_TRUE(waveFileIsOpen);

        size_t channelCount = dlb_wave_get_channel_count(&theWaveFile);
        size_t bitDepth = dlb_wave_get_bit_depth(&theWaveFile);
        size_t frameCount = dlb_wave_get_num_frames(&theWaveFile);
        size_t frameByteCount = (bitDepth / CHAR_BIT) * channelCount;
        size_t bufferByteCount = frameByteCount * sampleCount;
        size_t startFrame = (frameCount / 3) - 299;
        size_t readCount;

        bufferMemory = new char[bufferByteCount];
        ASSERT_NE(nullptr, bufferMemory);
        ::memset(bufferMemory, 0, bufferByteCount);

        status = dlb_wave_seek_to_frame(&theWaveFile, startFrame);
        ASSERT_EQ(0, status);
        status = dlb_wave_read_data(&theWaveFile, bufferMemory, bufferByteCount, &readCount);
        ASSERT_EQ(0, status);
        ASSERT_EQ(bufferByteCount, readCount);

        status = OpenFrameCaptor();
        ASSERT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_OK, status);

        dlb_pmd_metadata_set *pmds = nullptr;
        dlb_pmd_blob_descriptor descr;

        ::memset(&descr, 0, sizeof(descr));
        descr.number_of_samples = sampleCount;
        descr.number_of_channels = static_cast<uint16_t>(channelCount);
        descr.bit_depth = static_cast<uint8_t>(bitDepth);
        status = dlb_pmd_frame_captor_capture(&pmds, theCaptor, &descr, bufferMemory);
        ASSERT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_OK, status);
    }
}
#endif

TEST_F(DlbPmdCapture02, SliceItPmdRaw)
{
    static const char *testFileName = "D:\\data\\PMD\\PMDLIB-120\\pmd.raw";

    theBinaryFile = ::fopen(testFileName, "rb");
    if (theBinaryFile != nullptr)
    {
        // TODO: sanity checks on the captured metadata set...

        size_t channelCount = 2;
        size_t bitDepth = 32;
        size_t frameByteCount = (bitDepth / CHAR_BIT) * channelCount;
        size_t bufferByteCount = frameByteCount * sampleCount;
        size_t readCount;
        int status;

        bufferMemory = new char[bufferByteCount];
        ASSERT_NE(nullptr, bufferMemory);
        ::memset(bufferMemory, 0, bufferByteCount);

        readCount = ::fread(bufferMemory, 1, bufferByteCount, theBinaryFile);
        ASSERT_EQ(bufferByteCount, readCount);

        status = OpenFrameCaptor();
        ASSERT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_OK, status);

        dlb_pmd_metadata_set *pmds = nullptr;
        dlb_pmd_blob_descriptor descr;

        ::memset(&descr, 0, sizeof(descr));
        descr.number_of_samples = sampleCount;
        descr.number_of_channels = static_cast<uint16_t>(channelCount);
        descr.bit_depth = static_cast<uint8_t>(bitDepth);
        descr.big_endian = PMD_TRUE;
        status = dlb_pmd_frame_captor_capture(&pmds, theCaptor, &descr, bufferMemory);
        ASSERT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_OK, status);
    }
}

TEST_F(DlbPmdCapture02, SliceItSadmRaw)
{
    static const char *testFileName = "D:\\data\\PMD\\PMDLIB-120\\sadm.raw";

    theBinaryFile = ::fopen(testFileName, "rb");
    if (theBinaryFile != nullptr)
    {
        // BIG FAT NOTE: the SMPTE 337M pC values in the test input file are not correct
        // for our current S-ADM parsing, so we won't get any actual decodes!

        size_t channelCount = 2;
        size_t bitDepth = 32;
        size_t frameByteCount = (bitDepth / CHAR_BIT) * channelCount;
        size_t bufferByteCount = frameByteCount * sampleCount;
        size_t readCount;
        int status;

        bufferMemory = new char[bufferByteCount];
        ASSERT_NE(nullptr, bufferMemory);
        ::memset(bufferMemory, 0, bufferByteCount);

        readCount = ::fread(bufferMemory, 1, bufferByteCount, theBinaryFile);
        ASSERT_EQ(bufferByteCount, readCount);

        status = OpenFrameCaptor();
        ASSERT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_OK, status);

        dlb_pmd_metadata_set *pmds = nullptr;
        dlb_pmd_blob_descriptor descr;

        ::memset(&descr, 0, sizeof(descr));
        descr.number_of_samples = sampleCount;
        descr.number_of_channels = static_cast<uint16_t>(channelCount);
        descr.bit_depth = static_cast<uint8_t>(bitDepth);
        descr.big_endian = PMD_TRUE;
        status = dlb_pmd_frame_captor_capture(&pmds, theCaptor, &descr, bufferMemory);
        ASSERT_EQ(DLB_PMD_FRAME_CAPTOR_STATUS_OK, status);
    }
}
#endif
