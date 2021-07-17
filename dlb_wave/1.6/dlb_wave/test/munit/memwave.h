/******************************************************************************  
 * This program is protected under international and U.S. copyright laws as  
 * an unpublished work. This program is confidential and proprietary to the  
 * copyright owners. Reproduction or disclosure, in whole or in part, or the  
 * production of derivative works therefrom without the express permission of  
 * the copyright owners is prohibited.  
 *  
 *                  Copyright (C) 2015 by Dolby Laboratories.  
 *                            All rights reserved.  
 ******************************************************************************/

#ifndef MEMWAVE_H
#define MEMWAVE_H

#include "dlb_wave/include/dlb_wave.h"

/* Searches for the data chunk of a wave file stored in memory (p_riff).
 * riff_size is the length of p_riff in octets. The base pointer of the data
 * section is stored in pp_data and the size of the section is recorded in
 * p_data_size. All pointer arguments must be valid. The function returns NULL
 * on success, otherwise it returns an error string indicating what went
 * wrong. */
const char *
memwave_find_data
    (const unsigned char  *p_riff
    ,size_t                riff_size
    ,const unsigned char **pp_data
    ,size_t               *p_data_size
    );

/* Searches for the format chunk of a wave file stored in memory (p_riff) and
 * parses the chunk into a dlb_wave_format structure (p_fmt). If the format is
 * floating point, a non-zero value will be stored in *b_is_float, otherwise a
 * zero will be stored in *b_is_float. riff_size is the length of p_riff in
 * octets. All pointer arguments must be valid. The function returns NULL on
 * success, otherwise it returns an error string indicating what went
 * wrong. */
const char *
memwave_load_format
    (const unsigned char  *p_riff
    ,size_t                riff_size
    ,dlb_wave_format      *p_fmt
    ,int                  *b_is_float
    );

#endif /* MEMWAVE_H */
