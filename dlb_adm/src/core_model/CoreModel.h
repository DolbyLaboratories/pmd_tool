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

#ifndef DLB_ADM_CORE_MODEL_CLASS_H
#define DLB_ADM_CORE_MODEL_CLASS_H

#include <boost/core/noncopyable.hpp>
#include <functional>
#include <memory>

#include "ModelEntity.h"

namespace DlbAdm
{

    class ModelEntity;
    class Presentation;
    class ContentGroup;
    class ElementGroup;
    class AudioElement;
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
    };

}

#endif  // DLB_ADM_CORE_MODEL_CLASS_H
