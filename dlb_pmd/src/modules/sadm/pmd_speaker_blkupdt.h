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

#ifndef DLB_PMD_SPEAKER_BLKFMT_H
#define DLB_PMD_SPEAKER_BLKFMT_H

#include "dlb_pmd_types.h"

#define SPEAKER_LABEL_SIZE (8)

#if DLB_PMD_USE_ALT_SPKRS
#define PMD_SPEAKER_LS_R (PMD_SPEAKER_RESERVED_LAST + 2)
#define PMD_SPEAKER_RS_R (PMD_SPEAKER_LS_R + 1)
#endif

 /**
  * @brief type of BlockUpdate information used for channels in a bed
  */
typedef struct
{
    char name[DLB_PMD_NAME_ARRAY_SIZE]; /**< audio channel format name */
    char label[SPEAKER_LABEL_SIZE];     /**< Dolby S-ADM channel id */
    float x;                            /**< speaker's cartesian x coordinate */
    float y;                            /**< speaker's cartesian y coordinate */
    float z;                            /**< speaker's cartesian z coordinate */
} speaker_blkupdt;

/**
 * @brief S-ADM speaker BlockUpdate information for formats using Lss/Rss
 */
static speaker_blkupdt SPEAKER_BLKUPDT[] =
{
    /* L */    { "RoomCentricLeft",              "RC_L",   -1.0f,  1.0f,    0.0f },
    /* R */    { "RoomCentricRight",             "RC_R",    1.0f,  1.0f,    0.0f },
    /* C */    { "RoomCentricCenter",            "RC_C",    0.0f,  1.0f,    0.0f },
    /* Lfe */  { "RoomCentricLFE",               "RC_LFE", -1.0f,  1.0f,   -1.0f },
    /* Lss */  { "RoomCentricLeftSideSurround",  "RC_Lss", -1.0f,  0.0f,    0.0f },
    /* Rss */  { "RoomCentricRightSideSurround", "RC_Rss",  1.0f,  0.0f,    0.0f },
    /* Lrs */  { "RoomCentricLeftRearSurround",  "RC_Lrs", -1.0f, -1.0f,    0.0f },
    /* Rrs */  { "RoomCentricRightRearSurround", "RC_Rrs",  1.0f, -1.0f,    0.0f },
    /* Ltf */  { "RoomCentricLeftTopFront",      "RC_Ltf", -1.0f,  1.0f,    1.0f },
    /* Rtf */  { "RoomCentricRightTopFront",     "RC_Rtf",  1.0f,  1.0f,    1.0f },
    /* Ltm */  { "RoomCentricLeftTopMiddle",     "RC_Ltm", -1.0f,  0.0f,    1.0f },
    /* Rtm */  { "RoomCentricRightTopMiddle",    "RC_Rtm",  1.0f,  0.0f,    1.0f },
    /* Ltr */  { "RoomCentricLeftTopRear",       "RC_Ltr", -1.0f, -1.0f,    1.0f },
    /* Rtr */  { "RoomCentricRightTopRear",      "RC_Rtr",  1.0f, -1.0f,    1.0f },
    /* Lw */   { "RoomCentricLeftFrontWide",     "RC_Lfw", -1.0f,  0.677f,  0.0f },
    /* Rw */   { "RoomCentricRightFrontWide",    "RC_Rfw",  1.0f,  0.677f,  0.0f },
};

#if DLB_PMD_USE_ALT_SPKRS
/**
 * @brief S-ADM speaker BlockUpdate information for Ls/Rs
 */
static speaker_blkupdt SPEAKER_BLKUPDT_LS_RS[] =
{
    /* Ls */   { "RoomCentricLeftSurround",  "RC_Ls", -1.0f,  -1.0f,  0.0f },
    /* Rs */   { "RoomCentricRightSurround", "RC_Rs",  1.0f,  -1.0f,  0.0f },
};
#endif

/**
 * @brief find the block update information for a speaker position
 * 
 * #alt_spkrs true means we are using Ls/Rs instead of Lss/Rss
 */
static
const speaker_blkupdt *
find_speaker_blkupdt
    (dlb_pmd_speaker    *spkr
    ,dlb_pmd_bool        alt_spkrs
    )
{
    const speaker_blkupdt *sb = &SPEAKER_BLKUPDT[*spkr - 1];

#if DLB_PMD_USE_ALT_SPKRS
    if (!alt_spkrs)
    {
        /* !alt_spkrs means 2.0 - 5.1.4 (i.e., no rear surrounds)
         * when no rear surrounds, Ls and Rs take the Lrs and Rrs
         * speaker positions, plus shorter names
         */
        switch (*spkr)
        {
        case PMD_SPEAKER_LS:
            sb = &SPEAKER_BLKUPDT_LS_RS[0];
            *spkr = (dlb_pmd_speaker)PMD_SPEAKER_LS_R;
            break;
        case PMD_SPEAKER_RS:
            sb = &SPEAKER_BLKUPDT_LS_RS[1];
            *spkr = (dlb_pmd_speaker)PMD_SPEAKER_RS_R;
            break;
        default:
            break;
        }
    }
#else
    (void)alt_spkrs;
#endif

    return sb;
}

#endif  /* DLB_PMD_SPEAKER_BLKFMT_H */
