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

#ifndef DLB_ADM_CONTENT_GROUP_H
#define DLB_ADM_CONTENT_GROUP_H

#include "ModelEntity.h"

namespace DlbAdm
{

    class ContentGroup : public ModelEntity
    {
    public:
        ContentGroup();
        ContentGroup(dlb_adm_entity_id id, DLB_ADM_CONTENT_KIND contentKind);
        ContentGroup(const ContentGroup &x);
        virtual ~ContentGroup();

        ContentGroup &operator=(const ContentGroup &x);

        DLB_ADM_CONTENT_KIND GetContentKind() const { return mContentKind; }

        virtual bool AddLabel(const char *name, const char *language = "");

        virtual bool AddLabel(const std::string &name, const std::string &language);

    private:
        DLB_ADM_CONTENT_KIND mContentKind;
    };

}

#endif  // DLB_ADM_CONTENT_GROUP_H
