/************************************************************************
 * dlb_pmd
 * Copyright (c) 2023, Dolby Laboratories Inc.
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
