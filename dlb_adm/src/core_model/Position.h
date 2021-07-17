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

#ifndef DLB_ADM_POSITION_H
#define DLB_ADM_POSITION_H

#include "dlb_adm/include/dlb_adm_api_types.h"

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
        Position(dlb_adm_float coord1, dlb_adm_float coord2, dlb_adm_float coord3, bool cartesian);
        Position(const Position &x);
        ~Position();

        Position &operator=(const Position &x);

        bool IsCartesian() const { return mCartesian; }

        dlb_adm_float GetCoordinate1() const { return mCoordinate1; }
        dlb_adm_float GetCoordinate2() const { return mCoordinate2; }
        dlb_adm_float GetCoordinate3() const { return mCoordinate3; }

        void Clear();

    private:
        bool mCartesian;
        dlb_adm_float mCoordinate1;    // X or azimuth
        dlb_adm_float mCoordinate2;    // Y or elevation
        dlb_adm_float mCoordinate3;    // Z or distance
    };

}

#endif  // DLB_ADM_POSITION_H
