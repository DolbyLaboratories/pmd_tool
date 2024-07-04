/************************************************************************
 * dlb_adm
 * Copyright (c) 2021, Dolby Laboratories Inc.
 * Copyright (c) 2021, Dolby International AB.
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

#include "SourceGroup.h"

namespace DlbAdm
{

    static const size_t NAME_LIMIT = 1;

    SourceGroup::SourceGroup()
        : ModelEntity()
        , mSourceGroupID(UNKNOWN_SOURCE_GROUP_ID)
        , mSignalCount(0)
        , mTrackCount(0)
    {
        mNameLimit = NAME_LIMIT;
    }

    SourceGroup::SourceGroup(dlb_adm_entity_id entityID, SourceGroupID sourceGroupID, dlb_adm_uint signalCount, dlb_adm_uint trackCount)
        : ModelEntity(entityID, NAME_LIMIT)
        , mSourceGroupID(sourceGroupID)
        , mSignalCount(signalCount)
        , mTrackCount(trackCount)
    {
        // Empty
    }

    SourceGroup::SourceGroup(const SourceGroup &x)
        : ModelEntity(x)
        , mSourceGroupID(x.mSourceGroupID)
        , mSignalCount(x.mSignalCount)
        , mTrackCount(x.mTrackCount)
    {
        // Empty
    }

    SourceGroup::~SourceGroup()
    {
        mSourceGroupID = UNKNOWN_SOURCE_GROUP_ID;
        mSignalCount = 0;
        mTrackCount = 0;
    }

    SourceGroup &SourceGroup::operator=(const SourceGroup &x)
    {
        (void)ModelEntity::operator=(x);
        mSourceGroupID = x.mSourceGroupID;
        mSignalCount = x.mSignalCount;
        mTrackCount = x.mTrackCount;
        return *this;
    }

}
