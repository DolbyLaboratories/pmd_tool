/************************************************************************
 * dlb_adm
 * Copyright (c) 2025, Dolby Laboratories Inc.
 * Copyright (c) 2025, Dolby International AB.
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

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "gtest/gtest.h"
#include "TestUtilities.h"
#include "dlb_adm/include/dlb_adm_api.h"
#include "dlb_adm/src/core_model/Presentation.h"
#include "dlb_adm/src/core_model/ContentGroup.h"
#include "dlb_adm/src/core_model/LoudnessMetadata.h"

#include <cstring>
#include <fstream>
#include <math.h>
#include <cmath>

using namespace DlbAdm;

static inline void CheckStatus(int x)
{
    if (x != DLB_ADM_STATUS_OK)
    {
        throw false;
    }
}

static inline void CheckTrue(bool x)
{
    if (!x)
    {
        throw false;
    }
}

namespace DlbAdmTest
{

    bool CheckNames(dlb_adm_core_model &model, dlb_adm_entity_id id, dlb_adm_data_names &names)
    {
        bool good = false;

        try
        {
            CoreModel &coreModel = model.GetCoreModel();
            const ModelEntity *entity;
            dlb_adm_bool hasName;
            int status;

            good = coreModel.GetEntity(id, &entity);
            CheckTrue(good);
            status = ::dlb_adm_core_model_has_name(&hasName, &names);
            CheckStatus(status);
            good = !(static_cast<dlb_adm_bool>(entity->HasName()) ^ hasName);   // Same number of names (0 or 1)?
            CheckTrue(good);
            good = (entity->GetLabelCount() == names.label_count);              // Same number of labels (0+)?
            CheckTrue(good);

            EntityName entityName;
            std::string dataName;
            std::string dataLang;
            size_t i = 0;

            if (hasName)                    // Are the names the same?
            {
                good = entity->GetName(entityName, i);
                CheckTrue(good);
                dataName = names.names[i];
                dataLang = names.langs[i];
                good = ((entityName.GetName() == dataName) && (entityName.GetLanguage() == dataLang));
                CheckTrue(good);
                ++i;
            }

            while (i < names.name_count)    // Are the labels the same?
            {
                good = entity->GetName(entityName, i);
                CheckTrue(good);
                dataName = names.names[i];
                dataLang = names.langs[i];
                good = ((entityName.GetName() == dataName) && (entityName.GetLanguage() == dataLang));
                CheckTrue(good);
                ++i;
            }
        }
        catch (bool x)
        {
            good = x;
        }
        catch (...)
        {
            good = false;
        }

        return good;
    }

    template
    <typename T>
    static bool GetLoudnessMetadata(const ModelEntity *entity, LoudnessMetadata &outLoudness)
    {
        bool ok = false;

        const T *specificPtr = dynamic_cast<const T *>(entity);
        if(specificPtr != nullptr)
        {
            outLoudness = specificPtr->GetLoudnessMetadata();
            ok = true;
        }

        return ok;
    }

    bool CheckLoudnessMetadata(dlb_adm_core_model &model, const dlb_adm_entity_id id, const dlb_adm_data_loudness &source_loudness, DLB_ADM_ENTITY_TYPE type)
    {
        bool good = false;

        try
        {
            CoreModel &coreModel = model.GetCoreModel();
            const ModelEntity *entity;

            good = coreModel.GetEntity(id, &entity);
            CheckTrue(good);

            LoudnessMetadata entityLoudness;

            switch (type)
            {
            case DLB_ADM_ENTITY_TYPE_PROGRAMME:
                good = GetLoudnessMetadata<Presentation>(entity, entityLoudness);
                break;

            case DLB_ADM_ENTITY_TYPE_CONTENT:
                good = GetLoudnessMetadata<ContentGroup>(entity, entityLoudness);
                break;

            default:
                good = false;
                break;
            }
            CheckTrue(good);

            good = entityLoudness.GetLoudnessType() == source_loudness.loudness_type;
            CheckTrue(good);

            good = std::fabs( entityLoudness.GetLoudnessValue() - source_loudness.loudness_value) < 0.00001f; // float comparision....
            CheckTrue(good);
        }
        catch (bool x)
        {
            good = x;
        }
        catch (...)
        {
            good = false;
        }

        return good;
    }

    bool CompareFiles(const char *fname1, const char *fname2)
    {
        std::ifstream ifs1(fname1);
        std::ifstream ifs2(fname2);
        bool eq = ifs1.good() && ifs2.good();

        if (eq)
        {
            std::string line1;
            std::string line2;
            bool got1 = !std::getline(ifs1, line1).eof();
            bool got2 = !std::getline(ifs2, line2).eof();

            while (got1 && got2)
            {
                if (!(line1 == line2))
                {
                    eq = false;
                    break;
                }
                got1 = !std::getline(ifs1, line1).eof();
                got2 = !std::getline(ifs2, line2).eof();
            }
            if (eq && (got1 || got2))
            {
                eq = false; // they should end at the same time
            }
        }

        return eq;
    }

}
