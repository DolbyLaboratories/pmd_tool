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

#include "EntityName.h"

namespace DlbAdm
{

    EntityName::EntityName()
        : mName("")
        , mLanguage("")
    {
        // Empty
    }

    EntityName::EntityName(const char *name, const char *language /*= ""*/)
        : mName(name)
        , mLanguage(language)
    {
        // Empty
    }

    EntityName::EntityName(const std::string &name, const std::string &language)
        : mName(name)
        , mLanguage(language)
    {
        // Empty
    }

    EntityName::EntityName(const EntityName &x)
        : mName(x.mName)
        , mLanguage(x.mLanguage)
    {
        // Empty
    }

    EntityName::~EntityName()
    {
        // Empty
    }

    EntityName &EntityName::operator=(const EntityName &x)
    {
        mName = x.mName;
        mLanguage = x.mLanguage;
        return *this;
    }

}
