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

#ifndef DLB_ADM_MODEL_ENTITY_H
#define DLB_ADM_MODEL_ENTITY_H

#include "dlb_adm/include/dlb_adm_api_types.h"
#include "EntityName.h"

#include <vector>
#include <memory>

namespace DlbAdm
{

    class ModelEntity
    {
    public:
        virtual ~ModelEntity();

        dlb_adm_entity_id GetEntityID() const;

        DLB_ADM_ENTITY_TYPE GetEntityType() const;

        size_t GetNameCount() const { return mNameVector.size(); }

        size_t GetNameLimit() const { return mNameLimit; }

        size_t GetLabelCount() const { return mLabelCount; }

        bool GetName(EntityName &name, size_t index) const;

        bool AddName(const char *name, const char *language = "");

        bool AddName(const std::string &name, const std::string &language);

        bool HasName() const { return (GetNameCount() > GetLabelCount()); }

        bool operator<(const ModelEntity &x) const;

        static std::string TranslateAudioType(DLB_ADM_AUDIO_TYPE audioType);

    protected:
        typedef std::vector<EntityName> EntityNameVector;

        ModelEntity();
        explicit ModelEntity(dlb_adm_entity_id id, size_t nameLimit = 0);
        ModelEntity(const ModelEntity &x);

        ModelEntity &operator=(const ModelEntity &x);

        virtual bool AddLabel(const char *name, const char *language = "");

        virtual bool AddLabel(const std::string &name, const std::string &language);

        dlb_adm_entity_id mEntityID;
        size_t mNameLimit;
        size_t mLabelCount;

    private:
        EntityNameVector mNameVector;
    };

    typedef std::shared_ptr<const ModelEntity> ConstModelEntityPtr;

}

#endif /* DLB_ADM_MODEL_ENTITY_H */
