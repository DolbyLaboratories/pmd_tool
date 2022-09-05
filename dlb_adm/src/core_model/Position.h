/************************************************************************
 * dlb_adm
 * Copyright (c) 2020 - 2022, Dolby Laboratories Inc.
 * Copyright (c) 2022, Dolby International AB.
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

#ifndef DLB_ADM_POSITION_H
#define DLB_ADM_POSITION_H

#include "dlb_adm/include/dlb_adm_api_types.h"
#include <string>

namespace DlbAdm
{

    class Position
    {
    public:
        enum class COORDINATE
        {
            X,
            Y,
            Z,
            AZIMUTH,
            ELEVATION,
            DISTANCE
        };

        Position();
        Position(dlb_adm_float coord1, bool cartesian);
        Position(dlb_adm_float coord1, dlb_adm_float coord2, dlb_adm_float coord3, bool cartesian);
        Position(const Position &x);
        ~Position();

        Position &operator=(const Position &x);

        bool IsCartesian() const { return mCartesian; }

        dlb_adm_float GetCoordinate1() const { return mCoordinate1; }
        dlb_adm_float GetCoordinate2() const { return mCoordinate2; }
        dlb_adm_float GetCoordinate3() const { return mCoordinate3; }

        static Position SphericalToCartesian(const Position &pos);

        static Position CartesianToSpherical(const Position &pos);

        static int PositionCoordinateToName(Position::COORDINATE coordinate, std::string &name);

        static int PositionNameToCoordinate(const std::string &name, Position::COORDINATE &coordinate);

        void Clear();

    private:
        bool mCartesian;
        dlb_adm_float mCoordinate1;    // X or azimuth
        dlb_adm_float mCoordinate2;    // Y or elevation
        dlb_adm_float mCoordinate3;    // Z or distance
    };

}

#endif  // DLB_ADM_POSITION_H
