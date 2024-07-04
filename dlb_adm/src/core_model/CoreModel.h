/************************************************************************
 * dlb_adm
 * Copyright (c) 2020-2023, Dolby Laboratories Inc.
 * Copyright (c) 2020-2023, Dolby International AB.
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

#ifndef DLB_ADM_CORE_MODEL_CLASS_H
#define DLB_ADM_CORE_MODEL_CLASS_H

#include <boost/core/noncopyable.hpp>
#include <boost/interprocess/managed_heap_memory.hpp>
#include <functional>
#include <memory>
#include <set>

#include "ModelEntity.h"

namespace DlbAdm
{

    class ModelEntity;
    class Presentation;
    class ContentGroup;
    class ElementGroup;
    class AudioElement;
    class AlternativeValueSet;
    class ElementGroup;
    class AudioTrack;
    class TargetGroup;
    class Target;
    class SourceGroup;
    class Source;
    class BlockUpdate;
    class FrameFormat;
    struct PresentationRecord;
    struct ElementRecord;
    struct SourceRecord;
    struct UpdateRecord;
    class CoreModelData;
    class ComplementaryElement;
    class AudioObjectInteraction;

    class CoreModel : public boost::noncopyable
    {
    public:
        CoreModel();
        ~CoreModel();

        typedef std::function<int (const ModelEntity *e)> const& EntityCallbackFn;
        typedef std::function<bool(const ModelEntity *e)> const& EntityFilterFn;

        typedef std::function<int(const PresentationRecord &r)> const& PresentationCallbackFn;
        typedef std::function<int(const ElementRecord &r)> const& ElementCallbackFn;
        typedef std::function<int(const SourceRecord &r)> const& SourceCallbackFn;

        // Add model entities

        bool AddEntity(const Presentation &presentation);

        bool AddEntity(const ContentGroup &contentGroup);

        bool AddEntity(const ElementGroup &elementGroup);

        bool AddEntity(const AudioElement &audioElement);

        bool AddEntity(const AudioObjectInteraction &audioObjInteraction);

        bool AddEntity(const ComplementaryElement &compElement);

        bool AddEntity(const AlternativeValueSet &altValSet);

        bool AddEntity(const AudioTrack &audioTrack);

        bool AddEntity(const TargetGroup &targetGroup);

        bool AddEntity(const Target &target);

        bool AddEntity(const SourceGroup &sourceGroup);

        bool AddEntity(const Source &source);

        bool AddEntity(const BlockUpdate &update);

        bool AddEntity(const FrameFormat &frameFormat);

        // Add table records

        bool AddRecord(const PresentationRecord &record);

        bool AddRecord(const ElementRecord &record);

        bool AddRecord(const SourceRecord &record);

        bool AddRecord(const UpdateRecord &record);

        // Queries

        bool GetEntity(dlb_adm_entity_id entityID, const ModelEntity **e) const;

        template <class T>
        int GetEntity(dlb_adm_entity_id entityID, const T **e) const
        {
            int status = DLB_ADM_STATUS_ERROR;

            if (e != nullptr)
            {
                const ModelEntity *base = nullptr;
                bool gotBase = GetEntity(entityID, &base);

                if (gotBase)
                {
                    *e = dynamic_cast<const T *>(base);
                    if (*e != nullptr)
                    {
                        status = DLB_ADM_STATUS_OK;
                    }
                }
                else
                {
                    status = DLB_ADM_STATUS_NOT_FOUND;
                }
            }

            return status;
        }

        int ForEach(DLB_ADM_ENTITY_TYPE entityType, EntityCallbackFn callbackFn, EntityFilterFn filterFn = nullptr) const;

        size_t Count(DLB_ADM_ENTITY_TYPE entityType, EntityFilterFn filterFn = nullptr) const;

        int ForEach(PresentationCallbackFn callbackFn) const;

        int ForEach(ElementCallbackFn callbackFn) const;

        int ForEach(dlb_adm_entity_id audioElementID, ElementCallbackFn callbackFn) const;

        int ForEach(dlb_adm_entity_id presentationID, PresentationCallbackFn callbackFn) const;

        int ForEach(SourceCallbackFn callbackFn) const;

        bool GetSource(SourceRecord &record, dlb_adm_entity_id audioTrackID) const;

        bool GetBlockUpdate(UpdateRecord &record, dlb_adm_entity_id targetID) const;    // Return the first update for the target

        // Identity

        int GetEntityId(dlb_adm_entity_id &entityID, DLB_ADM_ENTITY_TYPE entityType, DLB_ADM_AUDIO_TYPE audioType = DLB_ADM_AUDIO_TYPE_NONE);

        int GetSubcomponentID(dlb_adm_entity_id &entityID, dlb_adm_entity_id parentID);

        // Miscellaneous

        void Clear();

        bool IsEmpty() const;

        void AddProfile(DLB_ADM_PROFILE profile) { mCoreModelProfiles.insert(profile); }

        bool HasProfile(DLB_ADM_PROFILE profile) const { 
			return mCoreModelProfiles.count(profile); 
		}

        const std::set<DLB_ADM_PROFILE> & GetProfiles() const { return mCoreModelProfiles; };

    private:
        template <class T>
        bool AddModelEntity(const T &entity);

        template <typename RecordT, typename TableT>
        bool AddModelRecord(const RecordT &record, TableT &table);

        template <typename IndexT, typename CallbackT>
        int ForEachRecord(IndexT &index, CallbackT callbackFn) const;

        bool Validate(const PresentationRecord &record);

        bool Validate(const ElementRecord &record);

        bool Validate(const SourceRecord &record);

        bool Validate(const UpdateRecord &record);

        std::unique_ptr<CoreModelData> mCoreModelData;

        /* From BS.2125-1 specifcation: "the flow is constrained by the most constrained parts of each profile" */
        std::set<DLB_ADM_PROFILE> mCoreModelProfiles;
        std::shared_ptr<boost::interprocess::managed_heap_memory> mSharedMemory;
    };

}

#endif  // DLB_ADM_CORE_MODEL_CLASS_H
