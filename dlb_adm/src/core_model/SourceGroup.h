/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2021 by Dolby Laboratories,
 *                Copyright (C) 2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef DLB_ADM_SOURCE_GROUP_H
#define DLB_ADM_SOURCE_GROUP_H

#include "ModelEntity.h"
#include "core_model_defs.h"

namespace DlbAdm
{

    class SourceGroup : public ModelEntity
    {
    public:
        SourceGroup();
        explicit SourceGroup
        (
            dlb_adm_entity_id    entityID,
            SourceGroupID        sourceGroupID = DEFAULT_SOURCE_GROUP_ID,
            dlb_adm_uint         signalCount = 0,
            dlb_adm_uint         trackCount = 0
        );
        SourceGroup(const SourceGroup &x);
        virtual ~SourceGroup();

        SourceGroup &operator=(const SourceGroup &x);

        SourceGroupID GetSourceGroupID() const { return mSourceGroupID; }
        dlb_adm_uint GetSignalCount() const { return mSignalCount; }
        dlb_adm_uint GetTrackCount() const { return mTrackCount; }

        void SetSignalCount(dlb_adm_uint signalCount) { mSignalCount = signalCount; }
        void SetTrackCount(dlb_adm_uint trackCount) { mTrackCount = trackCount; }

    private:
        SourceGroupID mSourceGroupID;
        dlb_adm_uint mSignalCount;
        dlb_adm_uint mTrackCount;
    };

}

#endif  // DLB_ADM_SOURCE_H
