/************************************************************************
 * dlb_adm
 * Copyright (c) 2020-2022, Dolby Laboratories Inc.
 * Copyright (c) 2020-2022, Dolby International AB.
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

#ifndef DLB_ADM_ENTITY_DESCRIPTOR_H
#define DLB_ADM_ENTITY_DESCRIPTOR_H

#include "dlb_adm/include/dlb_adm_api_types.h"

#include <string>
#include <functional>

namespace DlbAdm
{

    struct EntityDescriptor
    {
        std::string          name;              // Must be unique
        DLB_ADM_ENTITY_TYPE  entityType;
        bool                 xmlTypeComposite;
        bool                 hasADMIdOrRef;
        bool                 isReference;
        DLB_ADM_TAG          distinguishedTag;  // ID or value
    };

    extern EntityDescriptor nullEntityDescriptor;

    void InitializeEntityIndex();

    typedef std::function<bool(const EntityDescriptor &d)> const& EntityNameDisambiguationFn;

    int GetEntityDescriptor(EntityDescriptor &d, const std::string &name, EntityNameDisambiguationFn disambiguator = nullptr);

    int GetEntityDescriptor(EntityDescriptor &d, DLB_ADM_ENTITY_TYPE eType);    // Returns "not unique" for multiple results

    int GetEntityDescriptor(EntityDescriptor &d, DLB_ADM_ENTITY_TYPE eType, bool isReference);

}

#endif
