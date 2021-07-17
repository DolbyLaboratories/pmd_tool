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
