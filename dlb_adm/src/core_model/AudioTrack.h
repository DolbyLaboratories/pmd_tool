/************************************************************************
 * dlb_adm
 * Copyright (c) 2023, Dolby Laboratories Inc.
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

#ifndef DLB_ADM_AUDIO_TRACK_H
#define DLB_ADM_AUDIO_TRACK_H

#include "ModelEntity.h"
#include "core_model_defs.h"

namespace DlbAdm
{

    class AudioTrack : public ModelEntity
    {
    public:
        AudioTrack();
        explicit AudioTrack(dlb_adm_entity_id id, SampleRate sampleRate = UNKNOWN_SAMPLE_RATE, BitDepth bitDepth = UNKNOWN_BIT_DEPTH);
        AudioTrack(const AudioTrack &x);
        virtual ~AudioTrack();

        AudioTrack &operator=(const AudioTrack &x);

        SampleRate GetSampleRate() const { return mSampleRate; }

        BitDepth GetBitDepth() const { return mBitDepth; }

    private:
        SampleRate mSampleRate;
        BitDepth mBitDepth;
    };

}

#endif // DLB_ADM_AUDIO_TRACK_H
