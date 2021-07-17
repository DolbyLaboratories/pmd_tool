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

#include "ElementGroup.h"
#include "core_model_defs.h"

namespace DlbAdm
{

    ElementGroup::ElementGroup()
        : ModelEntity()
        , mGain()
    {
        mNameLimit = DEFAULT_NAME_LIMIT;
    }

    ElementGroup::ElementGroup(const ElementGroup &x)
        : ModelEntity(x)
        , mGain(x.mGain)
    {
        // Empty
    }

    ElementGroup::ElementGroup(dlb_adm_entity_id id, const Gain &gain)
        : ModelEntity(id)
        , mGain(gain)
    {
        mNameLimit = DEFAULT_NAME_LIMIT;
    }

    ElementGroup::ElementGroup(dlb_adm_entity_id id, float gainValue, Gain::GAIN_UNIT gainUnit /*= Gain::GAIN_UNIT::LINEAR*/)
        : ModelEntity(id)
        , mGain(gainValue, gainUnit)
    {
        mNameLimit = DEFAULT_NAME_LIMIT;
    }

    ElementGroup::~ElementGroup()
    {
        // Empty for now
    }

    ElementGroup &ElementGroup::operator=(const ElementGroup &x)
    {
        (void)ModelEntity::operator=(x);
        mGain = x.mGain;
        return *this;
    }

    bool ElementGroup::AddLabel(const char *name, const char *language)
    {
        return ModelEntity::AddLabel(name, language);
    }

    bool ElementGroup::AddLabel(const std::string &name, const std::string &language)
    {
        return ModelEntity::AddLabel(name, language);
    }

}
