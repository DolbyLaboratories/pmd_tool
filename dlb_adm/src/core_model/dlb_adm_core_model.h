/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2020-2021 by Dolby Laboratories,
 *                Copyright (C) 2020-2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef DLB_ADM_CORE_MODEL_H
#define DLB_ADM_CORE_MODEL_H

#include "CoreModel.h"

struct dlb_adm_core_model : public boost::noncopyable
{
public:
    explicit dlb_adm_core_model(const dlb_adm_core_model_counts *counts);
    ~dlb_adm_core_model();

    DlbAdm::CoreModel &GetCoreModel();

    const DlbAdm::CoreModel &GetCoreModel() const;

private:
    DlbAdm::CoreModel mCoreModel;
};

#endif /* DLB_ADM_CORE_MODEL_H */
