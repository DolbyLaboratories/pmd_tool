/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2020 by Dolby Laboratories,
 *                Copyright (C) 2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef DLB_ADM_SOURCE_H
#define DLB_ADM_SOURCE_H

#include "ModelEntity.h"
#include "core_model_defs.h"

namespace DlbAdm
{

    class Source : public ModelEntity
    {
    public:
        Source();
        Source(dlb_adm_entity_id entityID, ChannelNumber channelNumber, SourceGroupID sourceGroupID = DEFAULT_SOURCE_GROUP_ID);
        Source(const Source &x);
        virtual ~Source();

        Source &operator=(const Source &x);

        SourceGroupID GetSourceGroupID() const { return mSourceGroupID; }
        ChannelNumber GetChannelNumber() const { return mChannelNumber; }

    private:
        SourceGroupID mSourceGroupID;
        ChannelNumber mChannelNumber;
    };

}

#endif  // DLB_ADM_SOURCE_H
