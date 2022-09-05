/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020 - 2022, Dolby Laboratories Inc.
 * Copyright (c) 2022, Dolby International AB.
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

#ifndef _MIX_MATRIX_H_
#define _MIX_MATRIX_H_

#include "pmd_studio_error.h"

#define MAX_COMP_MIX_MATRIX_SIZE (MAX_INPUT_CHANNELS * MAX_OUTPUT_CHANNELS)

typedef float pmd_studio_mix_matrix[][MAX_OUTPUT_CHANNELS];
typedef float pmd_studio_mix_matrix_array[MAX_INPUT_CHANNELS][MAX_OUTPUT_CHANNELS];

struct pmd_studio_comp_mix_matrix
{
    unsigned int size;
    unsigned int input[MAX_COMP_MIX_MATRIX_SIZE];
    unsigned int output[MAX_COMP_MIX_MATRIX_SIZE];
    float coef[MAX_COMP_MIX_MATRIX_SIZE];
};

inline
void
pmd_studio_mix_matrix_reset
    (pmd_studio_mix_matrix mix_matrix)
{
    unsigned int i,j;
    for (i = 0 ; i < MAX_INPUT_CHANNELS ; i++)
    {
        for (j = 0 ; j < MAX_OUTPUT_CHANNELS ; j++)
        {
            mix_matrix[i][j] = 0.0;
        }
    }
}

inline
void
pmd_studio_mix_matrix_unity
    (pmd_studio_mix_matrix mix_matrix)
{
    unsigned int i,j;
    for (i = 0 ; i < MAX_INPUT_CHANNELS ; i++)
    {
        for (j = 0 ; j < MAX_OUTPUT_CHANNELS ; j++)
        {
            if (i == j)
            {
                mix_matrix[i][j] = 1.0;
            }
            else
            {
                mix_matrix[i][j] = 0.0;
            }
        }
    }
}

inline
void
pmd_studio_mix_matrix_print
    (pmd_studio_mix_matrix mix_matrix)
{
    unsigned int i,j;

    printf("Outputs\tMix Matrix\n\t==== ======\n");
    printf("|\tInputs---->\n|\n|\nV\n   ");
    for (j = 0 ; j < MAX_INPUT_CHANNELS ; j++)
    {
        printf("%5u",j + 1);
    }
    printf("\n");
    for (i = 0 ; i < (MAX_INPUT_CHANNELS * 5) + 3 ; i++)
    {
        printf("-");
    }
    printf("\n");
    for (i = 0 ; i < MAX_OUTPUT_CHANNELS ; i++)
    {
        printf("%2u| ",i + 1);
        for (j = 0 ; j < MAX_INPUT_CHANNELS ; j++)
        {
            printf("%2.2f ", mix_matrix[j][i]);
        }
        printf("\n");
    }
}

inline
void
compress_mix_matrix(pmd_studio_mix_matrix_array mix_matrix, struct pmd_studio_comp_mix_matrix *comp_mix_matrix, unsigned int input_channels, unsigned int output_channels)
{
    unsigned int i,j;

    comp_mix_matrix->size = 0;

    if ((input_channels > MAX_INPUT_CHANNELS) ||
        (input_channels == 0) ||
        (output_channels > MAX_OUTPUT_CHANNELS) ||
        (output_channels == 0))
    {
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "pmd_studio_device compress_mix_matrix - Invalid number of channels");
    }

    for (i = 0 ; i < input_channels ; i++)
    {
        for (j = 0 ; j < output_channels ; j++)
        {
            if (mix_matrix[i][j] != 0.0)
            {
                comp_mix_matrix->input[comp_mix_matrix->size] = i;
                comp_mix_matrix->output[comp_mix_matrix->size] = j;
                comp_mix_matrix->coef[comp_mix_matrix->size++] = mix_matrix[i][j];
                if (comp_mix_matrix->size == MAX_COMP_MIX_MATRIX_SIZE)
                {
                    break;
                }
            }
            if (comp_mix_matrix->size == MAX_COMP_MIX_MATRIX_SIZE)
            {
                break;
            }
        }
        if (comp_mix_matrix->size == MAX_COMP_MIX_MATRIX_SIZE)
        {
            break;
        }
    }
}

#endif // _MIX_MATRIX_H_
