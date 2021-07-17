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

#ifndef DLB_ADM_TARGET_GROUP_H
#define DLB_ADM_TARGET_GROUP_H

#include "ModelEntity.h"

namespace DlbAdm
{

    class TargetGroup : public ModelEntity
    {
    public:
        TargetGroup();
        TargetGroup(dlb_adm_entity_id id, DLB_ADM_SPEAKER_CONFIG speakerConfig);
        TargetGroup(dlb_adm_entity_id id, DLB_ADM_OBJECT_CLASS objectClass, bool isDynamic);
        TargetGroup(const TargetGroup &x);
        virtual ~TargetGroup();

        TargetGroup &operator=(const TargetGroup &x);

        bool IsBed() const { return mSpeakerConfig != DLB_ADM_SPEAKER_CONFIG_NONE; }

        bool IsObject() const { return mObjectClass != DLB_ADM_OBJECT_CLASS_NONE; }

        bool IsDynamic() const { return IsObject() && mIsDynamic; }

        DLB_ADM_SPEAKER_CONFIG GetSpeakerConfig() const { return mSpeakerConfig; }

        DLB_ADM_OBJECT_CLASS GetObjectClass() const { return mObjectClass; }

    private:
        DLB_ADM_SPEAKER_CONFIG mSpeakerConfig;
        DLB_ADM_OBJECT_CLASS mObjectClass;
        bool mIsDynamic;
    };

}

#endif // DLB_ADM_TARGET_GROUP_H
