/************************************************************************
 * dlb_adm
 * Copyright (c) 2023, Dolby Laboratories Inc.
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
