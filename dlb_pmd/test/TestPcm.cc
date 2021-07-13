/************************************************************************
 * dlb_pmd
 * Copyright (c) 2021, Dolby Laboratories Inc.
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
 * @file TestPcm.cc
 * @brief encapsulate PCM read/write testing
 */

extern "C"
{
#include "dlb_wave/include/dlb_wave_int.h"
#include "dlb_pmd_pcm.h"
#include "frontend/pcm_vsync_timer.h"
}


#include "TestPcm.hh"
#include <string.h>
#include <stdlib.h>

namespace
{
    static const size_t BLOCK_SIZE = 256;
    static const int SAMPLE_RATE = 48000;
    static const int NUM_CHANNELS = 2;

    /**
     * @brief video frame size in 48 kHz PCM samples
     */
    static const float FRAME_SIZES[NUM_PMD_FRAMERATES] =
    {
        /*  23.97 fps */  2002.0f,
        /*  24    fps */  2000.0f,
        /*  25    fps */  1920.0f,
        /*  29.97 fps */  1601.6f,
        /*  30    fps */  1600.0f,
        /*  50    fps */   960.0f,
        /*  59.94 fps */   800.8f,
        /*  60    fps */   800.0f,
        /* 100    fps */   480.0f,
        /* 119.88 fps */   400.4f,
        /* 120    fps */   400.0f,
    };
    
    enum Smpte337mConstants
    {
        PA          = 0x6f872000,   /* IEC 958 preamble a (sync word 1) */
        PB          = 0x54e1f000,   /* IEC 958 preamble b (sync word 2) */
        PC_MASK     = 0x007f0000,   /* SMPTE preamble C data_mode & data_type mask */
        PC          = 0x003b0000,   /* preamble C (stream 0, 20-bit, KLV), bit 24 'key_flag' set */
        PC_KEY_FLAG = 0x01000000,   /* KLV 'key_flag' to indicate presence of Universal Key */
        PC_NULL     = 0x00000000,   /* preamble C (20-bit, NULL) */
    };

    static const int PMD_BLOCK_SAMPLES = 160;
    static const int GUARDBAND_SAMPLES = 32;
    static const int PREAMBLE_SAMPLES = 4;
    static const int MAX_PAIR_BITS = (PMD_BLOCK_SAMPLES*2 - PREAMBLE_SAMPLES) * 20;
    static const int MAX_CHAN_BITS = (PMD_BLOCK_SAMPLES - PREAMBLE_SAMPLES) * 20;

    /* minimize the number of error files written; we don't want to risk flooding
     * the disk when there are many failures */
    static const unsigned int MAX_FILES_WRITTEN = 16;
    static unsigned int files_written = 0;
}


void TestPcm::run(TestModel& m, const char *testname, int param,
                  bool single_frame, dlb_pmd_frame_rate fr,
                  bool single_channel, bool minimal, bool nonames, bool match,
                  bool apply_updates, bool sadm, unsigned int num_skip_samples)
{
    dlb_klvpmd_universal_label ul = (dlb_klvpmd_universal_label)(param & 1);
    TestPcm pcm(fr, single_frame);    
    const char *errmsg = NULL;
    unsigned int num_frames;
    TestModel m1;
    TestModel m2;
    TestModel m3;

    m1 = m;
    if (pcm.write(!single_channel, ul, m1, sadm))
    {
        errmsg = "could not write PCM";
    }
    else if (pcm.validate())
    {
        errmsg = "PCM isn't valid";
    }
    else if (pcm.read(m2, num_skip_samples, &num_frames))
    {
        errmsg = "Failed to read PCM file";
    }
    else if (apply_updates && num_frames && m.apply_updates(fr, num_frames-1))
    {
        errmsg = "Failed to apply updates";
    }
    else if ((dlb_pmd_success)match == dlb_pmd_equal2(m, m2, !single_frame, nonames, minimal))
    {
        errmsg = match
            ? "Model mismatch after PCM write and read"
            : "Models shouldn't match after PCM write and read, but do";
    }
    else if (pcm.write(!single_channel, ul, m2, sadm))
    {
        errmsg = "could not write PCM2";
    }
    else if (pcm.validate())
    {
        errmsg = "PCM2 isn't valid";
    }
    else if (pcm.read(m3, num_skip_samples, &num_frames))
    {
        errmsg = "Failed to read PCM2 file";
    }
    else if (apply_updates && num_frames && m.apply_updates(fr, num_frames-1))
    {
        errmsg = "Failed to apply updates again";
    }
    else if ((dlb_pmd_success)match == dlb_pmd_equal2(m, m3, !single_frame, nonames, minimal))
    {
        errmsg = match
            ? "Model mismatch after PCM2 write and read"
            : "Models shouldn't match after PCM2 write and read, but do";
    }

    if (errmsg)
    {
        std::string error = errmsg;
        char name[128];
        snprintf(name, sizeof(name), "test_%s_%u_%s_%s_%s.wav",
                 testname, param,
                 TestModel::FRAME_RATE_NAMES[(int)fr],
                 single_channel ? "channel" : "pair",
                 sadm ? "sadm" : "klv");
        pcm.dump(name);
        error += ": ";
        error += name;
        throw TestModel::failure(error);
    }
}


TestPcm::TestPcm(dlb_pmd_frame_rate fr, bool check_updates)
    : fr_(fr)
    , ispair_(true)
    , num_samples_(0)
    , mem_(0)
#ifdef PA_POS
    , pa_positions_()
#endif
{
    if (check_updates)
    {
        /* updates expire after the first frame, so if we want to check
         * that update payloads are correct, we must limit ourselves to one frame
         * of PCM
         */
        num_samples_ = VF_SPACING[fr][0];
    }
    else
    {
        unsigned int frame_cycle_length = (unsigned int)(FRAME_SIZES[fr] * VF_CYCLE);
        num_samples_ = (SAMPLE_RATE / frame_cycle_length) * frame_cycle_length;
    }
    mem_ = new uint32_t[num_samples_ * NUM_CHANNELS];
    ::memset(mem_, '\0', num_samples_ * NUM_CHANNELS * sizeof(uint32_t));
}


TestPcm::~TestPcm()
{
    delete[] mem_;
}

    
dlb_pmd_success TestPcm::write(bool ispair, dlb_klvpmd_universal_label ul, dlb_pmd_model *m,
                               bool sadm)
{
    dlb_pmd_model_constraints limits;
    dlb_pcmpmd_augmentor *aug;
    uint32_t *channeldata;
    unsigned int chan = ispair ? 0 : 1;
    vsync_timer vt;
    size_t video_sync;
    size_t sz;
    char *mem;
    size_t num_samples = num_samples_;
    size_t read;
    
    dlb_pmd_get_constraints(m, &limits);
    sz = dlb_pcmpmd_augmentor_query_mem2(sadm, &limits);
    mem = new char[sz];

    ispair_ = ispair;
    /* note, for testing, we always want to mark the 160-sample blocks with NULL databursts */
    dlb_pcmpmd_augmentor_init2(&aug, m, mem, fr_, ul, 1, NUM_CHANNELS, NUM_CHANNELS, ispair, chan,
                               sadm);
    vsync_timer_init(&vt, fr_, 0);

    video_sync = 0;
    channeldata = mem_;
    while (num_samples)
    {
        read = num_samples < BLOCK_SIZE ? num_samples : BLOCK_SIZE;
        video_sync = vsync_timer_add_samples(&vt, read);            
        dlb_pcmpmd_augment(aug, channeldata, read, video_sync);
        channeldata += NUM_CHANNELS * read;
        num_samples -= read;
    }
    
    dlb_pcmpmd_augmentor_finish(aug);
    delete[] mem;
    return PMD_SUCCESS;
}

                
bool TestPcm::verify_preamble_(uint32_t pa, uint32_t pb, uint32_t pc, uint32_t pd,
                               unsigned int maxbits)
{
    if (pa != PA)
    {
        return false;
    }
    if (pb != PB)
    {
        return false;
    }
    if (((pc & PC_MASK) != PC) || ((pd>>12) > maxbits))
    {
        /* if PcPd is not a full KLV data burst, it may be a NULL data burst
         * because we enable the 'mark all blocks' option */
        if (pc != 0 || pd != 0)
        {
            return false;
        }
    }

    return true;
}


#ifdef PA_POS
void TestPcm::record_pa_positions_()
{
    uint32_t *pcm = mem_;
    uint16_t sample;
    size_t stride = 1;
    unsigned int max_bits = MAX_CHAN_BITS;
    size_t preamble_samples = PREAMBLE_SAMPLES;

    if (ispair_)
    {
        stride *= 2;
        max_bits *= 2;
        preamble_samples /= 2;
    }

    for (sample = 0; sample < num_samples_ - preamble_samples; ++sample, pcm += stride)
    {
        if (verify_preamble_(pcm[0], pcm[1], pcm[2], pcm[3], max_bits))
        {
            pa_positions_.push_back(sample);
        }
    }
}
#endif


dlb_pmd_success TestPcm::validate()
{
    /* The validator simply looks for occurrences of the SMPTE 337m preambles
     * at locations it expects:
     *  - At each video frame boundary, after the guardband
     *  - first block is 160-samples less guardband (32)
     *  - last block in frame is also 160-samples less guardband (32)
     *      (which means that the total guardband around the video sync point
     *      is 64 samples)
     *  - otherwise, every 160-sample block, until no more can be fitted into
     *    the video frame.
     */
    static const int NC = NUM_CHANNELS;

    unsigned int num_frames = (unsigned int)(num_samples_ / FRAME_SIZES[fr_]);
    size_t num_blocks;

#ifdef PA_POS
    record_pa_positions_();
#endif

    uint32_t *pcm = mem_;
    int vfpos = 0;    
    while (num_frames)
    {
        uint32_t *b = pcm + GUARDBAND_SAMPLES * NUM_CHANNELS;
        size_t frame_length = VF_SPACING[fr_][vfpos];
        num_blocks = frame_length / PMD_BLOCK_SAMPLES;

        bool block_0 = true;
        while (num_blocks)
        {
            if (ispair_)
            {
                if (!verify_preamble_(b[0], b[1], b[NC], b[NC+1], MAX_PAIR_BITS))
                {
                    return PMD_FAIL;
                }
            }
            else
            {
                /* in our tests, we always write single-channel PMD on the
                 * second channel of the pair */
                if (!verify_preamble_(b[1], b[NC+1], b[NC*2+1], b[NC*3+1], MAX_CHAN_BITS))
                {
                    return PMD_FAIL;
                }
            }

            b += PMD_BLOCK_SAMPLES * NC;
            if (block_0)
            {
                /* first block has GUARDBAND fewer samples */
                b -= GUARDBAND_SAMPLES * NC;
            }

            --num_blocks;
            block_0 = false;
        }

        pcm += frame_length * NUM_CHANNELS;
        vfpos = (vfpos + 1) % VF_CYCLE;
        --num_frames;
    }
    return PMD_SUCCESS;
}


dlb_pmd_success TestPcm::read(dlb_pmd_model *m, unsigned int num_skip_samples,
                              unsigned int *num_frames)
{
    dlb_pmd_success success;
    dlb_pmd_payload_set_status status;
    dlb_pmd_model_constraints limits;
    dlb_pcmpmd_extractor *ext;
    unsigned int chan = ispair_ ? 0 : 1;
    uint32_t *channeldata;
    size_t num_samples;
    size_t read;
    size_t sz;
    char *mem;
    
    dlb_pmd_get_constraints(m, &limits);
    sz = dlb_pcmpmd_extractor_query_mem2(1, &limits);
    mem = new char[sz];
    channeldata = mem_;
    num_samples = num_samples_;

    success = dlb_pmd_initialize_payload_set_status(&status, NULL, 0);
    if (success != PMD_SUCCESS)
    {
        return success;
    }
    dlb_pcmpmd_extractor_init2(&ext, mem, fr_, chan, NUM_CHANNELS, ispair_, m, &status, 1);

    channeldata = mem_;

    *num_frames = (unsigned int)((float)num_samples / FRAME_SIZES[fr_]);

    /* testing random access by skipping samples */
    channeldata += NUM_CHANNELS * (num_skip_samples);
    num_samples -= num_skip_samples;

    while (num_samples)
    {
        read = num_samples < BLOCK_SIZE ? num_samples : BLOCK_SIZE;
        dlb_pcmpmd_extract2(ext, channeldata, read, NULL, NULL, NULL);
        channeldata += NUM_CHANNELS * read;
        num_samples -= read;
    }
    
    dlb_pcmpmd_extractor_finish(ext);

    delete[] mem;
    return PMD_SUCCESS;
}

    
void TestPcm::dump(const char *filename)
{
    static const int BIT_DEPTH = 32;
    dlb_buffer buffer;
    void *ppdata[2];
    dlb_wave_file out;
    size_t num_samples = num_samples_;
    int res;

    if (files_written < MAX_FILES_WRITTEN)
    {
        res = dlb_wave_open_write(&out, filename, 0,
                                  SAMPLE_RATE, NUM_CHANNELS, 0, BIT_DEPTH);
        if (DLB_RIFF_OK != res)
        {
            printf("ERROR: could not create dummy test .wav file \"%s\"\n", filename);
            abort();
        }
    
        res = dlb_wave_begin_data(&out);
        if (DLB_RIFF_OK != res)
        {
            printf("ERROR: could not begin writing \"%s\"\n", filename);
            dlb_wave_close(&out);
            abort();
        }
        
        /* now set up silent dlb_buffer for writing */
        buffer.data_type = DLB_BUFFER_INT_LEFT;
        buffer.nchannel = NUM_CHANNELS;
        buffer.nstride = NUM_CHANNELS;
        buffer.ppdata = ppdata;
        uint32_t *channeldata = mem_;
        
        /* generate streams consisting of a integral number of frames */
        while (num_samples)
        {
            ppdata[0] = &channeldata[0];
            ppdata[1] = &channeldata[1];
            
            size_t blk = num_samples < BLOCK_SIZE ? num_samples : BLOCK_SIZE;
            res = dlb_wave_int_write(&out, &buffer, blk);
            if (res)
            {
                printf("ERROR: could not write \"%s\"\n", filename);
                dlb_wave_close(&out);
                abort();
            }
            channeldata += blk * NUM_CHANNELS;
            num_samples -= blk;
        }
        
        dlb_wave_end_data(&out);
        dlb_wave_close(&out);
        ++files_written;
    }
}

