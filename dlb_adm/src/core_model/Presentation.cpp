/************************************************************************
 * dlb_adm
 * Copyright (c) 2021, Dolby Laboratories Inc.
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
