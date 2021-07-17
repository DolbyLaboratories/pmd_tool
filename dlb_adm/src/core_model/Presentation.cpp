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

#include "Presentation.h"
#include "core_model_defs.h"

namespace DlbAdm
{

     Presentation::Presentation()
         : ModelEntity()
     {
         mNameLimit = DEFAULT_NAME_LIMIT;
     }

     Presentation::Presentation(dlb_adm_entity_id id)
         : ModelEntity(id, DEFAULT_NAME_LIMIT)
     {
         // Empty
     }

     Presentation::Presentation(const Presentation &x)
         : ModelEntity(x)
     {
         // Empty
     }

     Presentation::~Presentation()
     {
         // Empty
     }

     Presentation &Presentation::operator=(const Presentation &x)
     {
         (void)ModelEntity::operator=(x);
         return *this;
     }

     bool Presentation::AddLabel(const char *name, const char *language /*= ""*/)
     {
         return ModelEntity::AddLabel(name, language);
     }

     bool Presentation::AddLabel(const std::string &name, const std::string &language)
     {
         return ModelEntity::AddLabel(name, language);
     }

 }
