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

#include "ContentGroup.h"
#include "core_model_defs.h"

namespace DlbAdm
{

    ContentGroup::ContentGroup()
        : ModelEntity()
        , mContentKind(DLB_ADM_CONTENT_KIND_UNKNOWN)
    {
        mNameLimit = DEFAULT_NAME_LIMIT;
    }

    ContentGroup::ContentGroup(dlb_adm_entity_id id, DLB_ADM_CONTENT_KIND contentKind)
        : ModelEntity(id, DEFAULT_NAME_LIMIT)
        , mContentKind(contentKind)
    {
        // Empty
    }

    ContentGroup::ContentGroup(const ContentGroup &x)
        : ModelEntity(x)
        , mContentKind(x.mContentKind)
    {
        // Empty
    }

    ContentGroup::~ContentGroup()
    {
        mContentKind = DLB_ADM_CONTENT_KIND_UNKNOWN;
    }

    ContentGroup &ContentGroup::operator=(const ContentGroup &x)
    {
        (void)ModelEntity::operator=(x);
        mContentKind = x.mContentKind;
        return *this;
    }

    bool ContentGroup::AddLabel(const char *name, const char *language)
    {
        return ModelEntity::AddLabel(name, language);
    }

    bool ContentGroup::AddLabel(const std::string & name, const std::string & language)
    {
        return ModelEntity::AddLabel(name, language);
    }

}
