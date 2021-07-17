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
 * @file pmd_studio_error.h
 * @brief global state structure for PMD studio app
 */

#ifndef __PMD_STUDIO_ERROR_H__
#define __PMD_STUDIO_ERROR_H__

/* Error Handling */

#define PMD_STUDIO_ERROR_MESSAGE_SIZE 256

typedef enum {
    PMD_STUDIO_OK = 0,
    PMD_STUDIO_ERR_PA_ERROR,
    PMD_STUDIO_ERR_STREAMING,    
    PMD_STUDIO_ERR_ASSERT,
    PMD_STUDIO_ERR_MEMORY,
    PMD_STUDIO_ERR_UI,
    PMD_STUDIO_ERR_FILE,
    PMD_STUDIO_ERR_AUGMENTOR,
    PMD_STUDIO_NUM_ERROR_MESSAGES
} pmd_studio_error_code;

extern const char* pmd_studio_error_messages[];

void pmd_studio_information(const char message[]);
void pmd_studio_error(pmd_studio_error_code err, const char error_message[]);

#ifndef NDEBUG
#define pmd_studio_warning( message )         fprintf(stderr, "Warning at line %d in file %s of function %s, %s\n", __LINE__, __FILE__, __func__, message);     
#else
#define pmd_studio_warning( message )
#endif

#endif // __PMD_STUDIO_ERROR_H__