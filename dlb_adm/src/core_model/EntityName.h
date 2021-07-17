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

#ifndef DLB_ADM_ENTITY_NAME_H
#define DLB_ADM_ENTITY_NAME_H

#include <string>

namespace DlbAdm
{

    class EntityName
    {
    public:
        EntityName();
        explicit EntityName(const char *name, const char *language = "");
        EntityName(const std::string &name, const std::string &language);
        EntityName(const EntityName &x);
        ~EntityName();

        EntityName &operator=(const EntityName &x);

        bool HasLanguage() const { return !mLanguage.empty(); }

        std::string GetName() const { return mName; }

        std::string GetLanguage() const { return mLanguage; }

    private:
        std::string mName;
        std::string mLanguage;
    };

}

#endif // DLB_ADM_ENTITY_NAME_H
