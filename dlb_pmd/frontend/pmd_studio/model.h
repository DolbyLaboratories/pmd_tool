/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019-2021 by Dolby Laboratories,
 *                Copyright (C) 2019-2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

/**
 * @file model.h
 * @brief helper data struct to encapsulate use of dlb_pmd_model
 */

#ifndef __MODEL_H__
#define __MODEL_H__

#include "dlb_pmd_types.h"


/**
 * @brief encapsulate model and its creation
 */
typedef struct
{
    size_t               size;
    void                *mem;
    dlb_pmd_model_combo *model;
} model;


/**
 * @brief create a PMD model
 */
dlb_pmd_success
model_init
    (model *m
    );


/**
 * @brief destroy a PMD model
 */
void
model_finish
    (model *m
    );


/**
 * @brief populate a model from an XML file
 */
dlb_pmd_success
model_populate
    (model      *m
    ,const char *filename
    );


/**
 * @brief dump an XML representation of the model
 */
dlb_pmd_success
model_dump
    (model      *m
    ,const char *filename
    );


#endif /* __MODEL_H__ */
