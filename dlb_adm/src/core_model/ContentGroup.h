/************************************************************************
 * dlb_adm
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

#ifndef DLB_ADM_CONTENT_GROUP_H
#define DLB_ADM_CONTENT_GROUP_H

#include "ModelEntity.h"
#include "LoudnessMetadata.h"

namespace DlbAdm
{

    class ContentGroup : public ModelEntity
    {
    public:
        ContentGroup();
        ContentGroup(dlb_adm_entity_id id, DLB_ADM_CONTENT_KIND contentKind);
        ContentGroup(dlb_adm_entity_id id, DLB_ADM_CONTENT_KIND contentKind, const LoudnessMetadata &loudness);
        ContentGroup(dlb_adm_entity_id id, DLB_ADM_CONTENT_KIND contentKind, const dlb_adm_data_loudness &loudness);
        ContentGroup(const ContentGroup &x);
        virtual ~ContentGroup();

        ContentGroup &operator=(const ContentGroup &x);

        DLB_ADM_CONTENT_KIND GetContentKind() const { return mContentKind; }

        LoudnessMetadata GetLoudnessMetadata() const { return mLoudness; }

        virtual bool AddLabel(const char *name, const char *language = "");

        virtual bool AddLabel(const std::string &name, const std::string &language);

    private:
        DLB_ADM_CONTENT_KIND mContentKind;
        LoudnessMetadata mLoudness;
    };

}

#endif  // DLB_ADM_CONTENT_GROUP_H
