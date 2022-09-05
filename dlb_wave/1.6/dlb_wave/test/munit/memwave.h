/************************************************************************
 * dlb_wave
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
