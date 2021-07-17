/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2020-2021 by Dolby Laboratories,
 *                Copyright (C) 2020-2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#include "ModelEntity.h"
#include "dlb_adm/src/adm_identity/AdmId.h"

namespace DlbAdm
{

    ModelEntity::~ModelEntity()
    {
        mEntityID = DLB_ADM_NULL_ENTITY_ID;
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
        , mNameLimit(0)
        , mLabelCount(0)
        , mNameVector()
    {
        // Empty
    }

    ModelEntity::ModelEntity(dlb_adm_entity_id id, size_t nameLimit /*= 0*/)
        : mEntityID(id)
        , mNameLimit(nameLimit)
        , mLabelCount(0)
        , mNameVector()
    {
        // Empty
    }

    ModelEntity::ModelEntity(const ModelEntity &x)
        : mEntityID(x.mEntityID)
        , mNameLimit(x.mNameLimit)
        , mLabelCount(x.mLabelCount)
        , mNameVector(x.mNameVector)
    {
        // Empty
    }

    ModelEntity &ModelEntity::operator=(const ModelEntity &x)
    {
        mEntityID = x.mEntityID;
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
