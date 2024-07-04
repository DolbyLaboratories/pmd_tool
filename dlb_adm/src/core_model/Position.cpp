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

#include "Position.h"
#include <math.h>
#include <map>
#include <boost/algorithm/string.hpp>

namespace DlbAdm
{
    static std::map<Position::COORDINATE, std::string> cordinatesNames = {{Position::COORDINATE::X, std::string("X")}
                                                                         ,{Position::COORDINATE::Y, std::string("Y")}
                                                                         ,{Position::COORDINATE::Z, std::string("Z")}
                                                                         ,{Position::COORDINATE::AZIMUTH, std::string("AZIMUTH")}
                                                                         ,{Position::COORDINATE::ELEVATION, std::string("ELEVATION")}
                                                                         ,{Position::COORDINATE::DISTANCE, std::string("DISTANCE")}
                                                                         };

    Position::Position()
    {
        Clear();
    }

    Position::Position(dlb_adm_float coord1, dlb_adm_float coord2, dlb_adm_float coord3, bool cartesian)
        : mCartesian(cartesian)
        , mCoordinate1(coord1)
        , mCoordinate2(coord2)
        , mCoordinate3(coord3)
    {
        // Empty
    }

    Position::Position(dlb_adm_float coord1, bool cartesian)
        : Position(coord1, 0.0, 0.0, cartesian)
    {
        // Empty
    }

    Position::Position(const Position &x)
        : mCartesian(x.mCartesian)
        , mCoordinate1(x.mCoordinate1)
        , mCoordinate2(x.mCoordinate2)
        , mCoordinate3(x.mCoordinate3)
    {
        // Empty
    }

    Position::~Position()
    {
        Clear();
    }

    Position &Position::operator=(const Position &x)
    {
        mCartesian = x.mCartesian;
        mCoordinate1 = x.mCoordinate1;
        mCoordinate2 = x.mCoordinate2;
        mCoordinate3 = x.mCoordinate3;
        return *this;
    }

    Position Position::SphericalToCartesian(const Position &pos)
    {
        static const double PI_OVER_180 = 3.1415926535897932384626433832795 / 180.0;
        double xy_plane;
        float distance, x, y, z;

        distance = pos.GetCoordinate3();
        xy_plane = fabsf(distance * cosf(pos.GetCoordinate2() * PI_OVER_180));

        x = (float)(xy_plane * (sinf(pos.GetCoordinate1()   * PI_OVER_180) * -1.0));
        y = (float)(xy_plane *  cosf(pos.GetCoordinate1()   * PI_OVER_180));
        z = (float)(distance *  sinf(pos.GetCoordinate2() * PI_OVER_180));

        return Position(x, y, z, true);
    }

    Position Position::CartesianToSpherical(const Position &pos)
    {
        float azimuth, distance;
        float elevation = 0;

        distance = sqrtf(powf(pos.GetCoordinate1(),2) + powf(pos.GetCoordinate2(),2) + powf(pos.GetCoordinate3(),2));
        distance = fmin(fmax(distance, 0.0f), 1.0f);
        azimuth = atan2f(pos.GetCoordinate1(), pos.GetCoordinate2());
        if (distance != 0)
        {
            elevation = asinf(pos.GetCoordinate3() / distance);
        }

        return Position(azimuth, elevation, distance, false);
    }

    int Position::PositionCoordinateToName(Position::COORDINATE coordinate, std::string &name)
    {
        name = cordinatesNames[coordinate];
        return DLB_ADM_STATUS_OK;
    }

    int Position::PositionNameToCoordinate(const std::string &name, Position::COORDINATE &coordinate)
    {
        std::string upName(name);
        boost::to_upper(upName);
        int status = DLB_ADM_STATUS_ERROR;
        for (auto it : cordinatesNames)
        {
            if (it.second == upName)
            {
                coordinate = it.first;
                break;
            }
        }
        return status;
    }

    void Position::Clear()
    {
        // (Re)set to spherical origin
        mCartesian = false;
        mCoordinate1 = 0.0f;
        mCoordinate2 = 0.0f;
        mCoordinate3 = 0.0f;
    }

}
