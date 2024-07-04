/************************************************************************
 * dlb_pmd
 * Copyright (c) 2019-2023, Dolby Laboratories Inc.
 * Copyright (c) 2019-2023, Dolby International AB.
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

/** Constants */

#define MAX_AUDIO_OBJECTS (10)
#define MAX_AUDIO_BEDS (4)
#define MAX_STUDIO_AUDIO_SIGNALS (32)
#define MAX_AUDIO_PRESENTATIONS (10)
#define MAX_AUDIO_OUTPUTS (8)
#define MAX_BED_SOURCES (16)

#define MAX_METADATA_OUTPUTS (4)
#define INIT_METADATA_OUTPUTS (1)
#define MAX_LABEL_LENGTH (256)
#define NUM_VIDEO_FRAME_RATE_CADENCE (5)

/* Typedefs and enums */

typedef enum
{
    PMD_OUTPUT_MODE,
    SADM_OUTPUT_MODE,
    NUM_METADATA_FORMATS
} pmd_studio_metadata_format;

static const char pmd_studio_metadata_format_names[][MAX_LABEL_LENGTH] = 
{
    "PMD",
    "S-ADM"
};

typedef enum
{
    FPS_25,
    FPS_2997,
    FPS_30,
    FPS_50,
    FPS_5994,
    FPS_60,
    NUM_VIDEO_FRAME_RATES,
    INVALID_FRAME_RATE
} pmd_studio_video_frame_rate;

static const float pmd_studio_video_frame_rate_floats[NUM_VIDEO_FRAME_RATES] =
{ 25.0, 29.97, 30.0, 50.0, 59.94, 60};

static const unsigned int pmd_studio_video_frame_rate_max_frames[NUM_VIDEO_FRAME_RATES] = 
{ 1920, 1602, 1600, 960, 801, 800};

static const unsigned int pmd_studio_video_frame_rate_cadence[NUM_VIDEO_FRAME_RATES][NUM_VIDEO_FRAME_RATE_CADENCE] = 
{ { 1920, 1920, 1920, 1920, 1920 },
  { 1602, 1601, 1602, 1601, 1602 },
  { 1600, 1600, 1600, 1600, 1600 },
  { 960,  960,  960,  960,  960  },
  { 801,  801,  801,  801,  800, },
  { 800,  800,  800,  800,  800  } };


#endif /* PMD_STUDIO_LIMITS_H_ */
