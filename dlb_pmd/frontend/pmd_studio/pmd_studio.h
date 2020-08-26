/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020, Dolby Laboratories Inc.
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
 * @file pmd_studio.h
 * @brief global state structure for PMD studio app
 */

#ifndef __PMD_STUDIO_H__
#define __PMD_STUDIO_H__

#include <math.h>
#include <stdio.h>
#include "portaudio.h"
#include "dlb_pmd_api.h"
#include "ui.h"
#include "model.h"


/**
 * @brief abstract pmd_studio type
 */ 
typedef struct pmd_studio pmd_studio;
typedef float pmd_studio_mix_matrix[][MAX_OUTPUT_CHANNELS];
typedef float pmd_studio_mix_matrix_array[MAX_INPUT_CHANNELS][MAX_OUTPUT_CHANNELS];

#include "pmd_studio_audio_beds.h"
#include "pmd_studio_audio_objects.h"
#include "pmd_studio_audio_presentations.h"
#include "pmd_studio_audio_outputs.h"
#include "pmd_studio_device.h"

/* Conversion function get context back from pmd_studio but without exposing top level structure */
pmd_studio_audio_presentations *pmd_studio_get_presentations(pmd_studio *studio);
pmd_studio_audio_beds *pmd_studio_get_beds(pmd_studio *studio);
pmd_studio_audio_objects *pmd_studio_get_objects(pmd_studio *studio);
pmd_studio_outputs *pmd_studio_get_outputs(pmd_studio *studio);
pmd_studio_device *pmd_studio_get_device(pmd_studio *studio);

/* Global definitions */
const char *pmd_studio_speaker_config_strings[NUM_PMD_SPEAKER_CONFIGS];

extern const unsigned int pmd_studio_speaker_config_num_channels[];

/* Mix Matrix type and methods */
        
static inline
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

static inline
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

static inline
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


#define PMD_STUDIO_M3DB (0.7071f)
#define PMD_STUDIO_M6DB (0.5f)

/* Error Handling */

typedef enum {
	PMD_STUDIO_OK = 0,
	PMD_STUDIO_ERR_PA_ERROR,
	PMD_STUDIO_ERR_ASSERT,
	PMD_STUDIO_ERR_MEMORY,
    PMD_STUDIO_ERR_UI,
	PMD_STUDIO_NUM_ERROR_MESSAGES
} pmd_studio_error_code;

extern const char* pmd_studio_error_messages[];


static inline
void
pmd_studio_error
	(pmd_studio_error_code err,
	const char error_message[]
	)
{

#ifndef NDEBUG
	if (err == PMD_STUDIO_ERR_PA_ERROR)
	{
		fprintf(stderr, "%d,%s,%s,%s\n", err, error_message, pmd_studio_error_messages[err], Pa_GetErrorText(err));		
	}
	if (err < PMD_STUDIO_NUM_ERROR_MESSAGES)
	{
		fprintf(stderr, "%d,%s,%s\n", err, error_message, pmd_studio_error_messages[err]);
	}
	else
	{
		fprintf(stderr, "%d,%s\n", err, error_message);
	}
	exit(err);
#else
    (void)err; // suppress warnings
    (void)error_message;
#endif
}

#ifndef NDEBUG
#define pmd_studio_warning( message )         fprintf(stderr, "Warning at line %d in file %s of function %s, %s\n", __LINE__, __FILE__, __func__, message);     
#else
#define pmd_studio_warning( message )
#endif


/* Global helper functions */

static inline
void
add_grid_title
    (uiGrid *gbed
    ,const char *txt
    ,int left
    ,int top
    )
{
    uiGridAppend(gbed, uiControl(uiNewLabel(txt)), left, top, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
}

static inline
float
gain_from_db(
	float db
	)
{
    return (float)(pow(10.0, db / 20.0));
}


/**
 * @brief reinitialize studio model to initial state
 */
void
pmd_studio_reset
    (pmd_studio *studio
    );

/**
 * @brief read file to populate studio's model
 */
void
pmd_studio_open_file
    (pmd_studio *studio
    ,const char *filename
    );


/**
 * @brief write studio's model to file
 */
void
pmd_studio_save_file
    (pmd_studio *studio
    ,const char *filename
    ,dlb_pmd_bool sadm
    );


/**
 * @brief populate UI defs after reading a model from file
 */
void
pmd_studio_import
    (pmd_studio *studio
    );


/**
 * @brief rebuild model based on pmd_studio stored information
 */
void
pmd_studio_update_model
    (pmd_studio *studio
    );

dlb_pmd_model
*pmd_studio_get_model
    (pmd_studio *studio
    );


#endif /* __PMD_STUDIO_H__ */
