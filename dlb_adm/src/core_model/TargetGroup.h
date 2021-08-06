/************************************************************************
 * dlb_adm
 * Copyright (c) 2021, Dolby Laboratories Inc.
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
