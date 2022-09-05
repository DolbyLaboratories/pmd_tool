/************************************************************************
 * dlb_adm
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

#ifndef DLB_ADM_FRAME_FORMAT_H
#define DLB_ADM_FRAME_FORMAT_H

#include "ModelEntity.h"

namespace DlbAdm
{

    class FrameFormat : public ModelEntity
    {
    public:
        FrameFormat();
        FrameFormat(
            dlb_adm_entity_id entityID,
            const std::string &type,
            const std::string &start,
            const std::string &duration,
            const std::string &flowID
        );
        FrameFormat(const FrameFormat &x);
        virtual ~FrameFormat();

        std::string GetType() const { return mType; }
        std::string GetStart() const { return mStart; }
        std::string GetDuration() const { return mDuration; }
        std::string GetFlowID() const { return mFlowID; }

        FrameFormat &operator=(const FrameFormat &x);

    private:
        std::string mType;
        std::string mStart;	// TODO: dlb_adm_time
        std::string mDuration;  // TODO: dlb_adm_time
        std::string mFlowID;
    };

}

#endif  // DLB_ADM_FRAME_FORMAT_H
