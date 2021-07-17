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

#ifndef DLB_ADM_ELEMENT_GROUP_H
#define DLB_ADM_ELEMENT_GROUP_H

#include "ModelEntity.h"
#include "Gain.h"

namespace DlbAdm
{

    class ElementGroup : public ModelEntity
    {
    public:
        ElementGroup();
        ElementGroup(dlb_adm_entity_id id, const Gain &gain);
        ElementGroup(dlb_adm_entity_id id, float gainValue, Gain::GAIN_UNIT gainUnit = Gain::GAIN_UNIT::LINEAR);
        ElementGroup(const ElementGroup &x);
        virtual ~ElementGroup();

        ElementGroup &operator=(const ElementGroup &x);

        Gain GetGain() const { return mGain; }

        virtual bool AddLabel(const char *name, const char *language = "");

        virtual bool AddLabel(const std::string &name, const std::string &language);

    private:
        Gain mGain;
    };

}

#endif // DLB_ADM_ELEMENT_GROUP_H
