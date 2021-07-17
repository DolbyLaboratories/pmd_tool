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

#ifndef DLB_ADM_API_PVT_H
#define DLB_ADM_API_PVT_H

#include "dlb_adm/include/dlb_adm_data_types.h"

#ifdef __cplusplus
extern "C" {
#endif

const char *
dlb_adm_get_common_defs_path
    (void
    );

DLB_ADM_OBJECT_CLASS
dlb_adm_translate_content_kind
    (DLB_ADM_CONTENT_KIND contentKind
    );

#ifdef __cplusplus
}
#endif

#endif /* DLB_ADM_API_H */
