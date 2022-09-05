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

#include "ModelEntity.h"
#include "dlb_adm/src/adm_identity/AdmId.h"

namespace DlbAdm
{

    ModelEntity::~ModelEntity()
    {
        mEntityID = DLB_ADM_NULL_ENTITY_ID;
        mIsCommon = false;
        mNameLimit = 0;
        mLabelCount = 0;
    }

    dlb_adm_entity_id ModelEntity::GetEntityID() const
    {
        return mEntityID;
    }

    DLB_ADM_ENTITY_TYPE ModelEntity::GetEntityType() const
    {
        return static_cast<DLB_ADM_ENTITY_TYPE>(DLB_ADM_ID_GET_ENTITY_TYPE(mEntityID));
    }

    bool ModelEntity::GetName(EntityName &name, size_t index) const
    {
        bool ok = (index < GetNameCount());

        if (ok)
        {
            name = mNameVector[index];
        }

        return ok;
    }

    bool ModelEntity::AddName(const char *name, const char *language /*= ""*/)
    {
        bool add = (name != nullptr && name[0] != '\0');

        if (add)
        {
            add = AddName(std::string(name), std::string(language));
        }

        return add;
    }

    bool ModelEntity::AddName(const std::string &name, const std::string &language)
    {
        bool add = ((mNameLimit > 0 && GetNameCount() == 0) && (!name.empty()));

        if (add)
        {
            mNameVector.push_back(EntityName(name, language));
        }

        return add;
    }

    bool ModelEntity::operator<(const ModelEntity &x) const
    {
        return mEntityID < x.mEntityID;
    }

    static const char *audioTypeStrings[] =
    {
        "None",
        "DirectSpeakers",
        "Matrix",
        "Objects",
        "HOA",
        "Binaural"
    };

    std::string ModelEntity::TranslateAudioType(DLB_ADM_AUDIO_TYPE audioType)
    {
        if (audioType > DLB_ADM_AUDIO_TYPE_BINAURAL)
        {
            return std::string("Custom");
        }

        return std::string(audioTypeStrings[audioType]);
    }

    ModelEntity::ModelEntity()
        : mEntityID(DLB_ADM_NULL_ENTITY_ID)
        , mIsCommon(false)
        , mNameLimit(0)
        , mLabelCount(0)
        , mNameVector()
    {
        // Empty
    }

    ModelEntity::ModelEntity(dlb_adm_entity_id id, size_t nameLimit /*= 0*/, bool isCommon /*= false*/)
        : mEntityID(id)
        , mIsCommon(isCommon)
        , mNameLimit(nameLimit)
        , mLabelCount(0)
        , mNameVector()
    {
        // Empty
    }

    ModelEntity::ModelEntity(const ModelEntity &x)
        : mEntityID(x.mEntityID)
        , mIsCommon(x.mIsCommon)
        , mNameLimit(x.mNameLimit)
        , mLabelCount(x.mLabelCount)
        , mNameVector(x.mNameVector)
    {
        // Empty
    }

    ModelEntity &ModelEntity::operator=(const ModelEntity &x)
    {
        mEntityID = x.mEntityID;
        mIsCommon = x.mIsCommon;
        mNameLimit = x.mNameLimit;
        mLabelCount = x.mLabelCount;
        mNameVector = x.mNameVector;
        return *this;
    }

    bool ModelEntity::AddLabel(const char *name, const char *language /*= ""*/)
    {
        return AddLabel(std::string(name), std::string(language));
    }

    bool ModelEntity::AddLabel(const std::string &name, const std::string &language)
    {
        bool add = ((GetNameCount() < mNameLimit) && (!name.empty()));

        if (add)
        {
            mNameVector.push_back(EntityName(name, language));
            ++mLabelCount;
        }

        return add;
    }

}
