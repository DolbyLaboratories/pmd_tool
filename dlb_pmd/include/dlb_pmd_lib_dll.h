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

#ifndef DLB_PMD_LIB_DLL_H
#define DLB_PMD_LIB_DLL_H

#if defined(_WIN32) && defined(DLB_PMD_LIB_DLL)
    #ifdef DLB_PMD_DLL_EXPORT
        #define DLB_PMD_DLL_ENTRY __declspec(dllexport)
    #else
        #define DLB_PMD_DLL_ENTRY __declspec(dllimport)
    #endif
#else
    #define DLB_PMD_DLL_ENTRY
#endif

#endif /* DLB_PMD_LIB_DLL_H */
