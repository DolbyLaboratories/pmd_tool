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

#include "Position.h"

namespace DlbAdm
{

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

    void Position::Clear()
    {
        // (Re)set to spherical origin
        mCartesian = false;
        mCoordinate1 = 0.0f;
        mCoordinate2 = 0.0f;
        mCoordinate3 = 0.0f;
    }

}
