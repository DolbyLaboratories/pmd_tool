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

#ifndef PMD_STUDIO_LIMITS_H_
#define PMD_STUDIO_LIMITS_H_

/**
 * @file pmd_studio_limits.h
 * @brief define ranges of things that can be defined
 */


#define MAX_AUDIO_OBJECTS (4)
#define INIT_AUDIO_OBJECTS (1)
#define MAX_AUDIO_BEDS (4)
#define INIT_AUDIO_BEDS (4)
#define MAX_AUDIO_SIGNALS (32)
#define MAX_AUDIO_PRESENTATIONS (4)
#define INIT_AUDIO_PRESENTATIONS (1)
#define INIT_AUDIO_OUTPUTS (1)
#define MAX_AUDIO_OUTPUTS (8)
#define MAX_CHANNELS (32)
#define MAX_INPUT_CHANNELS MAX_CHANNELS
#define MAX_OUTPUT_CHANNELS MAX_CHANNELS
#define MAX_CONFIG_CHANNELS (16)

#if MAX_OUTPUT_CHANNELS < MAX_CONFIG_CHANNELS
#error Number of output channels must exceed largest configuration channel count
#endif

#define MAX_METADATA_OUTPUTS (4)
#define INIT_METADATA_OUTPUTS (1)
#define MAX_LABEL_LENGTH (32)


#endif /* PMD_STUDIO_LIMITS_H_ */
