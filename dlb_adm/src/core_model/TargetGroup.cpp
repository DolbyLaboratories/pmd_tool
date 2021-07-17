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

#include "TargetGroup.h"

namespace DlbAdm
{

    static const size_t NAME_LIMIT = 1;

    TargetGroup::TargetGroup()
        : ModelEntity()
        , mSpeakerConfig(DLB_ADM_SPEAKER_CONFIG_NONE)
        , mObjectClass(DLB_ADM_OBJECT_CLASS_NONE)
        , mIsDynamic(false)
    {
        mNameLimit = NAME_LIMIT;
    }

    TargetGroup::TargetGroup(dlb_adm_entity_id id, DLB_ADM_SPEAKER_CONFIG speakerConfig)
        : ModelEntity(id, NAME_LIMIT)
        , mSpeakerConfig(speakerConfig)
        , mObjectClass(DLB_ADM_OBJECT_CLASS_NONE)
        , mIsDynamic(false)
    {
        // Empty
    }

    TargetGroup::TargetGroup(dlb_adm_entity_id id, DLB_ADM_OBJECT_CLASS objectClass, bool isDynamic)
        : ModelEntity(id, NAME_LIMIT)
        , mSpeakerConfig(DLB_ADM_SPEAKER_CONFIG_NONE)
        , mObjectClass(objectClass)
        , mIsDynamic(isDynamic)
    {
        // Empty
    }

    TargetGroup::TargetGroup(const TargetGroup &x)
        : ModelEntity(x)
        , mSpeakerConfig(x.mSpeakerConfig)
        , mObjectClass(x.mObjectClass)
        , mIsDynamic(x.mIsDynamic)
    {
        // Empty
    }

    TargetGroup::~TargetGroup()
    {
        mSpeakerConfig = DLB_ADM_SPEAKER_CONFIG_NONE;
        mObjectClass = DLB_ADM_OBJECT_CLASS_NONE;
        mIsDynamic = false;
    }

    TargetGroup &TargetGroup::operator=(const TargetGroup &x)
    {
        (void)ModelEntity::operator=(x);
        mSpeakerConfig = x.mSpeakerConfig;
        mObjectClass = x.mObjectClass;
        mIsDynamic = x.mIsDynamic;
        return *this;
    }

}
