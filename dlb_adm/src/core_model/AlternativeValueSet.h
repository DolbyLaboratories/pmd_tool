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

#ifndef DLB_ADM_ALTERNATIVE_VALUE_SET_H
#define DLB_ADM_ALTERNATIVE_VALUE_SET_H

#include <boost/optional.hpp>

#include "dlb_adm/include/dlb_adm_api_types.h"
#include "ModelEntity.h"
#include "Position.h"
#include "Gain.h"

namespace DlbAdm
{

    class AlternativeValueSet : public ModelEntity
    {
    public:

        AlternativeValueSet();

        explicit AlternativeValueSet
            ( dlb_adm_entity_id id
            , const boost::optional<Position> position = boost::none
            , const boost::optional<Gain> gain = boost::none
            );

        AlternativeValueSet(const dlb_adm_data_alt_value_set &avs);
        AlternativeValueSet(const AlternativeValueSet &x);
        virtual ~AlternativeValueSet();

        bool HasPositionOffset() const { return mPositionOffset.has_value(); }
        bool HasGain()           const { return mGain.has_value(); }

        /** 
         * @param position variable to fill
         * @return DLB_ADM_STATUS_NOT_FOUND if mPostionOffset == std::nullopt, DLB_ADM_STATUS_OK otherwise
         */
        int GetPositionOffset(Position & position) const;

        /** 
         * @param gain variable to fill
         * @return DLB_ADM_STATUS_NOT_FOUND if mGain == std::nullopt, DLB_ADM_STATUS_OK otherwise
         */
        int GetGain(Gain & gain) const;

        dlb_adm_entity_id GetParentId() const;

        AlternativeValueSet &operator=(const AlternativeValueSet &x);

        virtual bool AddLabel(const char *name, const char *language = "");

        virtual bool AddLabel(const std::string &name, const std::string &language);

    private:
        boost::optional<Position> mPositionOffset;
        boost::optional<Gain>     mGain;
    };

}

#endif  // DLB_ADM_ALTERNATIVE_VALUE_SET_H
