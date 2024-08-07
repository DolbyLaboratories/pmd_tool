/************************************************************************
 * dlb_adm
 * Copyright (c) 2020, Dolby Laboratories Inc.
 * Copyright (c) 2020, Dolby International AB.
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

#ifndef DLB_ADM_RELATIONSHIP_DESCRIPTOR_H
#define DLB_ADM_RELATIONSHIP_DESCRIPTOR_H

#include "dlb_adm/include/dlb_adm_api_types.h"
#include "EntityRelationship.h"

namespace DlbAdm
{

    struct RelationshipArity
    {
        int     minArity;
        int     maxArity;

        static const int ANY;
    };

    struct RelationshipDescriptor
    {
        DLB_ADM_ENTITY_TYPE  fromType;
        DLB_ADM_ENTITY_TYPE  toType;
        ENTITY_RELATIONSHIP  relationship;
        RelationshipArity    arity;

        bool operator<(const RelationshipDescriptor &x) const;
    };

    extern RelationshipDescriptor nullRelationshipDescriptor;

    void InitializeRelationshipIndex();

    int GetRelationshipDescriptor(RelationshipDescriptor &rd, DLB_ADM_ENTITY_TYPE f, DLB_ADM_ENTITY_TYPE t);

}

#endif /* DLB_ADM_RELATIONSHIPS_H */
