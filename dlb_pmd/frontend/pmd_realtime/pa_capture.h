/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019-2020 by Dolby Laboratories,
 *                Copyright (C) 2019-2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

/**
 * @file pa_capture.h
 * @brief implementation of the CAPTURE mode of the realtime PMD tool
 */

/**
 * @brief run portaudio in capture mode
 */
dlb_pmd_success            /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
pa_capture
    (Args *args            /**< [in] command-line arguments */
    );


