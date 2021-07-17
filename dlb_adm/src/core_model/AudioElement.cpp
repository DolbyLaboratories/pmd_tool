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

#include "AudioElement.h"
#include "core_model_defs.h"

namespace DlbAdm
{

    AudioElement::AudioElement()
        : ModelEntity()
        , mGain()
    {
        mNameLimit = DEFAULT_NAME_LIMIT;
    }

    AudioElement::AudioElement(const AudioElement &x)
        : ModelEntity(x)
        , mGain(x.mGain)
    {
        // Empty
    }

    AudioElement::AudioElement(dlb_adm_entity_id id, const Gain &gain)
        : ModelEntity(id)
        , mGain(gain)
    {
        mNameLimit = DEFAULT_NAME_LIMIT;
    }

    AudioElement::AudioElement(dlb_adm_entity_id id, float gainValue /*= 1.0f*/, Gain::GAIN_UNIT gainUnit /*= Gain::GAIN_UNIT::LINEAR*/)
        : ModelEntity(id)
        , mGain(gainValue, gainUnit)
    {
        mNameLimit = DEFAULT_NAME_LIMIT;
    }

    AudioElement::~AudioElement()
    {
        // Empty for now
    }

    AudioElement &AudioElement::operator=(const AudioElement &x)
    {
        (void)ModelEntity::operator=(x);
        mGain = x.mGain;
        return *this;
    }

    bool AudioElement::AddLabel(const char *name, const char *language)
    {
        return ModelEntity::AddLabel(name, language);
    }

    bool AudioElement::AddLabel(const std::string &name, const std::string &language)
    {
        return ModelEntity::AddLabel(name, language);
    }

}
