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

#include "ContentGroup.h"
#include "core_model_defs.h"

namespace DlbAdm
{

    ContentGroup::ContentGroup()
        : ModelEntity()
        , mContentKind(DLB_ADM_CONTENT_KIND_UNKNOWN)
        , mLoudness()
    {
        mNameLimit = DEFAULT_NAME_LIMIT;
    }

    ContentGroup::ContentGroup(dlb_adm_entity_id id, DLB_ADM_CONTENT_KIND contentKind)
        : ModelEntity(id, DEFAULT_NAME_LIMIT)
        , mContentKind(contentKind)
        , mLoudness()
    {
        // Empty
    }

    ContentGroup::ContentGroup(dlb_adm_entity_id id, DLB_ADM_CONTENT_KIND contentKind, const LoudnessMetadata &loudness)
        : ModelEntity(id, DEFAULT_NAME_LIMIT)
        , mContentKind(contentKind)
        , mLoudness(loudness)
    {
        // Empty
    }

    ContentGroup::ContentGroup(dlb_adm_entity_id id, DLB_ADM_CONTENT_KIND contentKind, const dlb_adm_data_loudness &loudness)
        : ModelEntity(id, DEFAULT_NAME_LIMIT)
        , mContentKind(contentKind)
        , mLoudness(loudness.loudness_value, loudness.loudness_type)
    {
        // Empty
    }

    ContentGroup::ContentGroup(const ContentGroup &x)
        : ModelEntity(x)
        , mContentKind(x.mContentKind)
        , mLoudness(x.GetLoudnessMetadata())
    {
        // Empty
    }

    ContentGroup::~ContentGroup()
    {
        mContentKind = DLB_ADM_CONTENT_KIND_UNKNOWN;
    }

    ContentGroup &ContentGroup::operator=(const ContentGroup &x)
    {
        (void)ModelEntity::operator=(x);
        mContentKind = x.mContentKind;
        mLoudness = x.mLoudness;
        return *this;
    }

    bool ContentGroup::AddLabel(const char *name, const char *language)
    {
        return ModelEntity::AddLabel(name, language);
    }

    bool ContentGroup::AddLabel(const std::string & name, const std::string & language)
    {
        return ModelEntity::AddLabel(name, language);
    }

}
