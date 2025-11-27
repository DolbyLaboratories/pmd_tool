/************************************************************************
 * dlb_adm
 * Copyright (c) 2020-2025, Dolby Laboratories Inc.
 * Copyright (c) 2020-2025, Dolby International AB.
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

#include "CoreModel.h"

#include "ModelEntityContainer.h"
#include "PresentationTable.h"
#include "ElementTable.h"
#include "SourceTable.h"
#include "UpdateTable.h"

#include "Presentation.h"
#include "ContentGroup.h"
#include "ElementGroup.h"
#include "AudioElement.h"
#include "AlternativeValueSet.h"
#include "AudioTrack.h"
#include "TargetGroup.h"
#include "Target.h"
#include "SourceGroup.h"
#include "Source.h"
#include "BlockUpdate.h"
#include "FrameFormat.h"
#include "ComplementaryElement.h"
#include "AudioObjectInteraction.h"
#include "DolbyeInfo.h"
#include "DolbyeEncoderParameters.h"
#include "DolbyeProgram.h"
#include "ProfileDescriptor.h"

#include "dlb_adm/src/adm_identity/AdmIdTranslator.h"
#include "dlb_adm/src/adm_identity/AdmIdSequenceMap.h"

#include <inttypes.h>

namespace DlbAdm
{
    using namespace boost::interprocess;
    class CoreModelData
    {
    public:
        CoreModelData(std::shared_ptr<managed_heap_memory> memory)
        {
            mMemory = memory;
            mModelEntityContainer = mMemory->construct<ModelEntityContainer>("ModelEntityContainer")(ModelEntityContainer::ctor_args_list(), mMemory->get_allocator<ModelEntityRecord>());
            mPresentationTable = mMemory->construct<PresentationTable>("PresentationTable")(PresentationTable::ctor_args_list(), mMemory->get_allocator<PresentationRecord>());
            mElementTable = mMemory->construct<ElementTable>("ElementTable")(ElementTable::ctor_args_list(), mMemory->get_allocator<ElementRecord>());
            mSourceTable = mMemory->construct<SourceTable>("SourceTable")(SourceTable::ctor_args_list(), mMemory->get_allocator<SourceRecord>());
            mUpdateTable = mMemory->construct<UpdateTable>("UpdateTable")(UpdateTable::ctor_args_list(), mMemory->get_allocator<UpdateRecord>());
            mSequenceMap = std::unique_ptr<AdmIdSequenceMap>(new AdmIdSequenceMap(*mMemory));
        }

        ~CoreModelData()
        {
            Clear();
        }

        ModelEntityContainer &GetModelEntityContainer() { return *mModelEntityContainer; }
        PresentationTable &GetPresentationTable() { return *mPresentationTable; }
        ElementTable &GetElementTable() { return *mElementTable; }
        SourceTable &GetSourceTable() { return *mSourceTable; }
        UpdateTable &GetUpdateTable() { return *mUpdateTable; }
        AdmIdSequenceMap &GetSequenceMap() { return *mSequenceMap; }

        void Clear();

        bool IsEmpty() const;

    private:

        void DeallocataModelEntities();
        template <class T>
        bool RemoveModelEntity(const DlbAdm::ModelEntity &entity);

        ModelEntityContainer *mModelEntityContainer;
        PresentationTable *mPresentationTable;
        ElementTable *mElementTable;
        SourceTable *mSourceTable;
        UpdateTable *mUpdateTable;
        std::unique_ptr<AdmIdSequenceMap> mSequenceMap;
        std::shared_ptr<boost::interprocess::managed_heap_memory> mMemory;
    };

    CoreModel::CoreModel()
        :mCoreModelProfiles()
    {
        mSharedMemory = std::shared_ptr<managed_heap_memory>(new managed_heap_memory(160000));
        mCoreModelData = std::unique_ptr<CoreModelData>(new CoreModelData(mSharedMemory));
    }

    CoreModel::~CoreModel()
    {
    }

    template<class T>
    bool CoreModelData::RemoveModelEntity(const DlbAdm::ModelEntity &entity)
    {
        char name[32];
        snprintf(name, 32, "%" PRIX64, entity.GetEntityID());
        return mMemory->destroy<T>(name);
    }

    void CoreModelData::DeallocataModelEntities()
    {
        ModelEntityContainer_PKIndex &index = mModelEntityContainer->get<ModelEntityContainer_PK>();
        for (auto it = index.begin(); it != index.end();)
        {
            switch (it->GetReference().GetEntityType())
            {
            case DLB_ADM_ENTITY_TYPE_FRAME_FORMAT:
                RemoveModelEntity<FrameFormat>(it->GetReference());
                break;

            case DLB_ADM_ENTITY_TYPE_TRANSPORT_TRACK_FORMAT:
                RemoveModelEntity<SourceGroup>(it->GetReference());
                break;

            case DLB_ADM_ENTITY_TYPE_PROGRAMME:
                RemoveModelEntity<Presentation>(it->GetReference());
                break;

            case DLB_ADM_ENTITY_TYPE_CONTENT:
                RemoveModelEntity<ContentGroup>(it->GetReference());
                break;

            case DLB_ADM_ENTITY_TYPE_OBJECT:
                if (dynamic_cast<const AudioElement*>(it->GetPointer()) != nullptr)
                {
                    RemoveModelEntity<AudioElement>(it->GetReference()); // For Objects
                }
                else
                {
                    RemoveModelEntity<ElementGroup>(it->GetReference()); // For Beds
                }
                break;

            case DLB_ADM_ENTITY_TYPE_PACK_FORMAT:
                RemoveModelEntity<TargetGroup>(it->GetReference());
                break;

            case DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT:
                RemoveModelEntity<Target>(it->GetReference());
                break;

            case DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT:
                RemoveModelEntity<BlockUpdate>(it->GetReference());
                break;

            case DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET:
                RemoveModelEntity<AlternativeValueSet>(it->GetReference());
                break;

            case DLB_ADM_ENTITY_TYPE_TRACK_UID:
                RemoveModelEntity<AudioTrack>(it->GetReference());
                break;

            case DLB_ADM_ENTITY_TYPE_AUDIO_TRACK:
                RemoveModelEntity<Source>(it->GetReference());
                break;

            case DLB_ADM_ENTITY_TYPE_OBJECT_INTERACTION:
                RemoveModelEntity<AudioObjectInteraction>(it->GetReference());
                break;
                
            case DLB_ADM_ENTITY_TYPE_COMPLEMENTARY_OBJECT_REF:
                RemoveModelEntity<ComplementaryElement>(it->GetReference());
                break;

            case DLB_ADM_ENTITY_TYPE_PROFILE_LIST_SPECIFICATION:
                RemoveModelEntity<ProfileDescriptor>(it->GetReference());
                break;

            case DLB_ADM_ENTITY_TYPE_DOLBY_E:
                RemoveModelEntity<DolbyeInfo>(it->GetReference());
                break;

            case DLB_ADM_ENTITY_TYPE_AC3_PROGRAM:
                RemoveModelEntity<DolbyeProgram>(it->GetReference());
                break;

            case DLB_ADM_ENTITY_TYPE_ENCODE_PARAMETERS:
                RemoveModelEntity<DolbyeEncoderParameters>(it->GetReference());
                break;

            default:
                assert(0);
                break;
            }
            ++it;
        }
    }

    void CoreModelData::Clear()
    {
        mUpdateTable->clear();
        mSourceTable->clear();
        mPresentationTable->clear();
        mElementTable->clear();
        mSequenceMap->Clear();

        DeallocataModelEntities();
        mModelEntityContainer->clear();
    }

    bool CoreModelData::IsEmpty() const
    {
        return
            mModelEntityContainer->empty()   &&
            mPresentationTable->empty()      &&
            mElementTable->empty()           &&
            mSourceTable->empty()            &&
            mUpdateTable->empty()            &&
            mSequenceMap->IsEmpty();
    }

    template<class T>
    bool CoreModel::AddModelEntity(const T &entity)
    {
        EntityName eName;
        entity.GetName(eName, 0);
        char name[32];
        snprintf(name, 32, "%" PRIX64, entity.GetEntityID());
        std::pair<T* , managed_heap_memory::size_type> ret = mSharedMemory->find<T>(name);
        if (ret.first != nullptr)
        {
            return 0; // Reject duplacates
        }

        ConstModelEntityPtr p = mSharedMemory->construct<T>(name)(entity);
        ModelEntityRecord r(p);
        auto result = mCoreModelData->GetModelEntityContainer().insert(r);
        return result.second;
    }

    bool CoreModel::AddEntity(const Presentation &presentation)
    {
        return AddModelEntity(presentation);
    }

    bool CoreModel::AddEntity(const ContentGroup &contentGroup)
    {
        return AddModelEntity(contentGroup);
    }

    bool CoreModel::AddEntity(const ElementGroup &elementGroup)
    {
        return AddModelEntity(elementGroup);
    }

    bool CoreModel::AddEntity(const AudioElement &audioElement)
    {
        return AddModelEntity(audioElement);
    }

    bool CoreModel::AddEntity(const AudioObjectInteraction &audioObjInteration)
    {
        return AddModelEntity(audioObjInteration);
    }

    bool CoreModel::AddEntity(const ComplementaryElement &element)
    {
        return AddModelEntity(element);
    }

    bool CoreModel::AddEntity(const AlternativeValueSet &altValSet)
    {
        return AddModelEntity(altValSet);
    }

    bool CoreModel::AddEntity(const AudioTrack &audioTrack)
    {
        return AddModelEntity(audioTrack);
    }

    bool CoreModel::AddEntity(const TargetGroup &targetGroup)
    {
        return AddModelEntity(targetGroup);
    }

    bool CoreModel::AddEntity(const Target &target)
    {
        return AddModelEntity(target);
    }

    bool CoreModel::AddEntity(const SourceGroup &sourceGroup)
    {
        return AddModelEntity(sourceGroup);
    }

    bool CoreModel::AddEntity(const Source &source)
    {
        return AddModelEntity(source);
    }

    bool CoreModel::AddEntity(const BlockUpdate &update)
    {
        return AddModelEntity(update);
    }

    bool CoreModel::AddEntity(const FrameFormat &frameFormat)
    {
        return AddModelEntity(frameFormat);
    }

    bool CoreModel::AddEntity(const DolbyeInfo &info)
    {
        return AddModelEntity(info);
    }

    bool CoreModel::AddEntity(const DolbyeProgram &info)
    {
        return AddModelEntity(info);
    }

    bool CoreModel::AddEntity(const DolbyeEncoderParameters &info)
    {
        return AddModelEntity(info);
    }

    bool CoreModel::AddEntity(const ProfileDescriptor &profile)
    {
        return AddModelEntity(profile);
    }


    template <typename RecordT, typename TableT>
    bool CoreModel::AddModelRecord(const RecordT &record, TableT &table)
    {
        bool inserted = false;

        if (Validate(record))
        {
            auto result = table.insert(record);
            inserted = result.second;
        }

        return inserted;
    }

    bool CoreModel::AddRecord(const PresentationRecord &record)
    {
        return AddModelRecord(record, mCoreModelData->GetPresentationTable());
    }

    bool CoreModel::AddRecord(const ElementRecord &record)
    {
        return AddModelRecord(record, mCoreModelData->GetElementTable());
    }

    bool CoreModel::AddRecord(const SourceRecord &record)
    {
        return AddModelRecord(record, mCoreModelData->GetSourceTable());
    }

    bool CoreModel::AddRecord(const UpdateRecord &record)
    {
        return AddModelRecord(record, mCoreModelData->GetUpdateTable());
    }

    bool CoreModel::GetEntity(dlb_adm_entity_id entityID, const ModelEntity **e) const
    {
        bool found = false;

        if (e != nullptr)
        {
            ModelEntityContainer_PKIndex &index = mCoreModelData->GetModelEntityContainer().get<ModelEntityContainer_PK>();
            ModelEntityContainer_PKIndex::iterator it = index.find(entityID, ModelEntityIdCompare());

            found = (it != index.end());
            if (found)
            {
                *e = it->GetPointer();
            }
            else
            {
                *e = nullptr;
            }
        }

        return found;
    }

    int CoreModel::ForEach(DLB_ADM_ENTITY_TYPE entityType, EntityCallbackFn callbackFn, EntityFilterFn filterFn) const
    {
        ModelEntityContainer_PKIndex &index = mCoreModelData->GetModelEntityContainer().get<ModelEntityContainer_PK>();
        auto itPair = index.equal_range(entityType, ModelEntityTypeCompare());
        ModelEntityContainer_PKIndex::iterator it = itPair.first;
        int status = DLB_ADM_STATUS_OK;

        if (filterFn != nullptr)
        {
            while (it != itPair.second)
            {
                const ModelEntity *p = (it->GetPointer());

                if (filterFn(p))
                {
                    status = callbackFn(p);
                    if (status != DLB_ADM_STATUS_OK)
                    {
                        break;
                    }
                }
                ++it;
            }
        }
        else
        {
            while (it != itPair.second)
            {
                status = callbackFn(it->GetPointer());
                if (status != DLB_ADM_STATUS_OK)
                {
                    break;
                }
                ++it;
            }
        }

        return status;
    }

    size_t CoreModel::Count(DLB_ADM_ENTITY_TYPE entityType, EntityFilterFn filterFn) const
    {
        size_t count = 0;
        EntityCallbackFn f = [&](const ModelEntity *)
        {
            ++count;
            return static_cast<int>(DLB_ADM_STATUS_OK);
        };
        int status = ForEach(entityType, f, filterFn);

        if (status != DLB_ADM_STATUS_OK)
        {
            throw status;
        }

        return count;
    }

    template <typename IndexT, typename CallbackT>
    int CoreModel::ForEachRecord(IndexT &index, CallbackT callbackFn) const
    {
        auto it = index.begin();
        int status = DLB_ADM_STATUS_OK;

        while (it != index.end())
        {
            status = callbackFn(*it);
            if (status != DLB_ADM_STATUS_OK)
            {
                break;
            }
            ++it;
        }

        return status;
    }

    int CoreModel::ForEach(PresentationCallbackFn callbackFn) const
    {
        return ForEachRecord(mCoreModelData->GetPresentationTable().get<PresentationTable_PK>(), callbackFn);
    }

    int CoreModel::ForEach(ElementCallbackFn callbackFn) const
    {
        return ForEachRecord(mCoreModelData->GetElementTable().get<ElementTable_PK>(), callbackFn);
    }

    int CoreModel::ForEach(dlb_adm_entity_id audioElementID, ElementCallbackFn callbackFn) const
    {
        ElementTable_PKIndex &index = mCoreModelData->GetElementTable().get<ElementTable_PK>();
        auto itPair = index.equal_range(audioElementID, AudioElementIdCompare());
        ElementTable_PKIndex::iterator it = itPair.first;
        int status = DLB_ADM_STATUS_OK;

        while (it != itPair.second)
        {
            status = callbackFn(*it);
            if (status != DLB_ADM_STATUS_OK)
            {
                break;
            }
            ++it;
        }

        return status;
    }

    int CoreModel::ForEach(dlb_adm_entity_id presentationID, PresentationCallbackFn callbackFn) const
    {
        PresentationTable_PKIndex &index = mCoreModelData->GetPresentationTable().get<PresentationTable_PK>();
        auto itPair = index.equal_range(presentationID, PresentationIdCompare());
        PresentationTable_PKIndex::iterator it = itPair.first;
        int status = DLB_ADM_STATUS_OK;

        while (it != itPair.second)
        {
            status = callbackFn(*it);
            if (status != DLB_ADM_STATUS_OK)
            {
                break;
            }
            ++it;
        }

        return status;
    }

    int CoreModel::ForEach(SourceCallbackFn callbackFn) const
    {
        return ForEachRecord(mCoreModelData->GetSourceTable().get<SourceTable_PK>(), callbackFn);
    }

    bool CoreModel::GetSource(SourceRecord &record, dlb_adm_entity_id audioTrackID) const
    {
        SourceTable_AudioTrackIndex &index = mCoreModelData->GetSourceTable().get<SourceTable_AudioTrack>();
        SourceTable_AudioTrackIndex::iterator it = index.find(audioTrackID);
        bool found = (it != index.end());

        if (found)
        {
            record = *it;
        }
        else
        {
            record = {};
        }

        return found;
    }

    bool CoreModel::GetBlockUpdate(UpdateRecord &record, dlb_adm_entity_id targetID) const
    {
        UpdateTable_PKIndex &index = mCoreModelData->GetUpdateTable().get<UpdateTable_PK>();
        auto itPair = index.equal_range(targetID, TargetIdCompare());
        bool found = itPair.first != itPair.second;

        if (found)
        {
            record = *(itPair.first);
        }

        return found;
    }

    int CoreModel::GetEntityId(dlb_adm_entity_id &entityID, DLB_ADM_ENTITY_TYPE entityType, DLB_ADM_AUDIO_TYPE audioType)
    {
        AdmIdTranslator translator;
        int status = DLB_ADM_STATUS_OK;

        entityID = DLB_ADM_NULL_ENTITY_ID;
        if (translator.IsGenericEntityType(entityType))
        {
            entityID = translator.ConstructGenericId(entityType, mCoreModelData->GetSequenceMap().GetSequenceNumber(entityType));
        }
        else
        {
            switch (entityType)
            {
            case DLB_ADM_ENTITY_TYPE_FRAME_FORMAT:
            {
                char buffer[ADM_ID_MAX_LEN + 1];
                uint32_t seq = mCoreModelData->GetSequenceMap().GetSequenceNumber(entityType);

                snprintf(buffer, sizeof(buffer), "FF_%08x", seq);
                entityID = translator.Translate(buffer);
                if (entityID == DLB_ADM_NULL_ENTITY_ID)
                {
                    status = DLB_ADM_STATUS_ERROR;
                }
                break;
            }

            case DLB_ADM_ENTITY_TYPE_TRANSPORT_TRACK_FORMAT:
            case DLB_ADM_ENTITY_TYPE_PROGRAMME:
            case DLB_ADM_ENTITY_TYPE_CONTENT:
            case DLB_ADM_ENTITY_TYPE_OBJECT:
                entityID = translator.ConstructUntypedId(entityType, mCoreModelData->GetSequenceMap().GetSequenceNumber(entityType));
                break;

            case DLB_ADM_ENTITY_TYPE_PACK_FORMAT:
            case DLB_ADM_ENTITY_TYPE_STREAM_FORMAT:
            case DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT:
                entityID = translator.ConstructTypedId(entityType, audioType, mCoreModelData->GetSequenceMap().GetSequenceNumber(entityType));
                break;

            case DLB_ADM_ENTITY_TYPE_TRACK_FORMAT:
            case DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT:
            case DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET:
            default:
                status = DLB_ADM_STATUS_ERROR;
                break;
            }
        }

        return status;
    }

    int CoreModel::GetSubcomponentID(dlb_adm_entity_id &entityID, dlb_adm_entity_id parentID)
    {
        AdmIdTranslator translator;
        DLB_ADM_ENTITY_TYPE entityType = translator.GetEntityType(parentID);
        int status = DLB_ADM_STATUS_OK;

        entityID = DLB_ADM_NULL_ENTITY_ID;
        switch (entityType)
        {
        case DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT:
        case DLB_ADM_ENTITY_TYPE_OBJECT:
            entityID = translator.ConstructSubcomponentId(parentID, mCoreModelData->GetSequenceMap().GetSubcomponentNumber(parentID));
            break;

        default:
            status = DLB_ADM_STATUS_ERROR;
            break;
        }

        return status;
    }

    void CoreModel::Clear()
    {
        mCoreModelData->Clear();
        mCoreModelProfiles.clear();
    }

    bool CoreModel::IsEmpty() const
    {
        return mCoreModelData->IsEmpty() && mCoreModelProfiles.empty();
    }

    bool CoreModel::Validate(const PresentationRecord &record)
    {
        const ModelEntity *ptr;

        bool altValGood =
            ( record.altValueSetID == DLB_ADM_NULL_ENTITY_ID
            ||  (  record.presentationID != DLB_ADM_NULL_ENTITY_ID
                && GetEntity(record.altValueSetID, &ptr)
                && AdmIdTranslator().SubcomponentIdReferencesComponent(record.audioElementID, record.altValueSetID)
                )
            );

        bool good =
            record.Validate() &&
            (record.presentationID == DLB_ADM_NULL_ENTITY_ID || GetEntity(record.presentationID, &ptr)) &&
            GetEntity(record.contentGroupID, &ptr) &&
            (record.elementGroupID == DLB_ADM_NULL_ENTITY_ID || GetEntity(record.elementGroupID, &ptr)) &&
            (record.complementaryRefID == DLB_ADM_NULL_ENTITY_ID || GetEntity(record.complementaryRefID, &ptr)) &&
            GetEntity(record.audioElementID, &ptr) &&
            altValGood;

        return good;
    }

    bool CoreModel::Validate(const ElementRecord &record)
    {
        const ModelEntity *ptr;
        bool good =
            record.Validate() &&
            GetEntity(record.audioElementID, &ptr) &&
            GetEntity(record.targetGroupID,  &ptr) &&
            GetEntity(record.targetID,       &ptr) &&
            GetEntity(record.audioTrackID,   &ptr);

        return good;
    }

    bool CoreModel::Validate(const SourceRecord &record)
    {
        const ModelEntity *ptr;
        bool good =
            record.Validate() &&
            GetEntity(record.sourceGroupID, &ptr) &&
            GetEntity(record.sourceID,      &ptr) &&
            GetEntity(record.audioTrackID,  &ptr);

        return good;
    }

    bool CoreModel::Validate(const UpdateRecord &record)
    {
        const ModelEntity *ptr;
        bool good = record.Validate();

        if (good)
        {
            good = GetEntity(record.updateID, &ptr);
        }

        return good;
    }

}
