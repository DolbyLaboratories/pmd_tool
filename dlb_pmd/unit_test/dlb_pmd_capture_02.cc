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

// TODO...
