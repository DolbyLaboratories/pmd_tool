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
 * @file dlb_sadm_common_definitions.c
 * @brief Common definitions for the audio definition model.
 *        Based on Recommendation  ITU-R  BS.2094-1
 */


#include "sadm/dlb_sadm_model.h"
#include "dlb_sadm_common_definitions.h"
#include <string.h>
#include <stdio.h>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
#endif


static const dlb_sadm_pack_format common_packfmt[NUM_COMMON_AUDIO_PACK_FORMATS] = 
{
    {.id.data="AP_00010001",    "mono_(0+1+0)",                 .type=DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS,	.chanfmts.num=0, .chanfmts.max=1,   .chanfmts.array=NULL},
    {.id.data="AP_00010002",    "stereo_(0+2+0)",               .type=DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS,	.chanfmts.num=0, .chanfmts.max=2,   .chanfmts.array=NULL},
    {.id.data="AP_00010003",    "5.1_(0+5+0)",                  .type=DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS,	.chanfmts.num=0, .chanfmts.max=6,   .chanfmts.array=NULL},
    {.id.data="AP_00010004",    "7.1_top_(2+5+0)",              .type=DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS,	.chanfmts.num=0, .chanfmts.max=8,   .chanfmts.array=NULL},
    {.id.data="AP_00010005",    "9.1_5.1.4_(4+5+0)",            .type=DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS,	.chanfmts.num=0, .chanfmts.max=10,  .chanfmts.array=NULL},
    {.id.data="AP_00010006",    "10.1_(4+5+1)",                 .type=DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS,	.chanfmts.num=0, .chanfmts.max=11,  .chanfmts.array=NULL},
    {.id.data="AP_00010007",    "10.2_(3+7+0)",                 .type=DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS,	.chanfmts.num=0, .chanfmts.max=12,  .chanfmts.array=NULL},
    {.id.data="AP_00010008",    "13.1_(4+9+0)",                 .type=DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS,	.chanfmts.num=0, .chanfmts.max=14,  .chanfmts.array=NULL},
    {.id.data="AP_00010009",    "22.2_(9+10+3)",                .type=DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS,	.chanfmts.num=0, .chanfmts.max=24,  .chanfmts.array=NULL},
    {.id.data="AP_0001000A",    "3.0_(0+3+0)",                  .type=DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS,	.chanfmts.num=0, .chanfmts.max=3,   .chanfmts.array=NULL},
    {.id.data="AP_0001000B",    "4.0_(0+4+0)",                  .type=DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS,	.chanfmts.num=0, .chanfmts.max=4,   .chanfmts.array=NULL},
    {.id.data="AP_0001000C",    "5.0_(0+5+0)",                  .type=DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS,	.chanfmts.num=0, .chanfmts.max=5,   .chanfmts.array=NULL},
    {.id.data="AP_0001000D",    "6.1_(0+6+0)",                  .type=DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS,	.chanfmts.num=0, .chanfmts.max=7,   .chanfmts.array=NULL},
    {.id.data="AP_0001000E",    "7.1_front_(0+7+0) ",           .type=DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS,	.chanfmts.num=0, .chanfmts.max=8,   .chanfmts.array=NULL},
    {.id.data="AP_0001000F",    "7.1_back_(0+7+0)",             .type=DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS,	.chanfmts.num=0, .chanfmts.max=8,   .chanfmts.array=NULL},
    {.id.data="AP_00010011",    "Auro-3D_(9+9+0)",              .type=DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS,	.chanfmts.num=0, .chanfmts.max=19,  .chanfmts.array=NULL},
    {.id.data="AP_00010012",    "7.1side_5.1+sc_(0+7+0)",       .type=DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS,	.chanfmts.num=0, .chanfmts.max=8,   .chanfmts.array=NULL},
    {.id.data="AP_00010013",    "7.1topside_5.1.2_(2+5+0)",     .type=DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS,	.chanfmts.num=0, .chanfmts.max=8,   .chanfmts.array=NULL},
    {.id.data="AP_00010014",    "9.1screen_5.1.2+sc_(2+7+0)",   .type=DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS,	.chanfmts.num=0, .chanfmts.max=10,  .chanfmts.array=NULL},
    {.id.data="AP_00010015",    "11.1_5.1.4+sc_(4+7+0)",        .type=DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS,	.chanfmts.num=0, .chanfmts.max=12,  .chanfmts.array=NULL},
    {.id.data="AP_00010016",    "9.1_7.1.2_(2+7+0)",            .type=DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS,	.chanfmts.num=0, .chanfmts.max=10,  .chanfmts.array=NULL},
    {.id.data="AP_00010017",    "11.1_7.1.4_(4+7+0)",           .type=DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS,	.chanfmts.num=0, .chanfmts.max=12,  .chanfmts.array=NULL}
};


static const unsigned int common_packfmt_chanfmt[NUM_COMMON_AUDIO_PACK_FORMATS][NUM_COMMON_PACKFMT_CHANFMT] =
{
    { 2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},             //AP_00010001
    { 0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},             //AP_00010002
    { 0,  1,  2,  3,  4,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},             //AP_00010003
    { 0,  1,  2,  3,  4,  5, 12, 14,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},             //AP_00010004
    { 0,  1,  2,  3,  4,  5, 12, 14, 15, 17,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},             //AP_00010005
    { 0,  1,  2,  3,  4,  5, 12, 14, 15, 17, 20,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},             //AP_00010006
    { 2,  0,  1, 33, 34,  9, 10, 27, 28, 39, 31, 32,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},             //AP_00010007
    { 0,  1,  2,  3,  9, 10, 27, 28, 33, 34, 13, 14, 35, 36,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},             //AP_00010008
    {23, 24,  2, 31, 27, 28,  0,  1,  8, 32,  9, 10, 33, 34, 11, 12, 29, 30, 18, 29, 16, 20, 21, 22},             //AP_00010009
    { 0,  1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},             //AP_0001000A
    { 0,  1,  2,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},             //AP_0001000B
    { 0,  1,  2,  4,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},             //AP_0001000C
    { 0,  1,  2,  3,  4,  5,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},             //AP_0001000D
    { 0,  1,  2,  3,  4,  5, 37, 38,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},             //AP_0001000E
    { 0,  1,  2,  3,  9, 10, 27, 28,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},             //AP_0001000F
    { 0,  1,  2,  3,  4,  5,  9, 10, 25, 26, 12, 14, 13, 15, 17, 18, 19, 29, 30,  0,  0,  0,  0,  0},             //AP_00010011
    { 0,  1,  2,  3,  4,  5, 35, 36,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},             //AP_00010012
    { 0,  1,  2,  3,  4,  5, 18, 19,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},             //AP_00010013
    { 0,  1,  2,  3,  4,  5, 18, 19, 35, 36,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},             //AP_00010014
    { 0,  1,  2,  3,  4,  5, 12, 14, 15, 17, 35, 36,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},             //AP_00010015
    { 0,  1,  2,  3,  9, 10, 27, 28, 18, 19,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},             //AP_00010016
    { 0,  1,  2,  3,  9, 10, 27, 28, 33, 34, 29, 30,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}              //AP_00010017
};


static const dlb_sadm_channel_format common_chanfmt[NUM_COMMON_AUDIO_CHANNEL_FORMATS] = 
{
    {.id.data="AC_00010001",    "FrontLeft",            .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010002",    "FrontRight",           .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010003",    "FrontCentre",          .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010004",    "LowFrequencyEffects",  .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010005",    "SurroundLeft",         .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010006",    "SurroundRight",        .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010007",    "FrontLeftOfCentre",    .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010008",    "FrontRightOfCentre",   .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010009",    "BackCentre",           .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_0001000A",    "SideLeft",             .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_0001000B",    "SideRight",            .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_0001000C",    "TopCentre",            .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_0001000D",    "TopFrontLeft",         .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_0001000E",    "TopFrontCentre",       .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_0001000F",    "TopFrontRight",        .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010010",    "TopSurroundLeft",      .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010011",    "TopBackCentre",        .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010012",    "TopSurroundRight",     .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010013",    "TopSideLeft",          .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010014",    "TopSideRight",         .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010015",    "BottomFrontCentre",    .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010016",    "BottomFrontLeftMid",   .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010017",    "BottomFrontRightMid",  .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010018",    "FrontLeftWide",        .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010019",    "FrontRightWide",       .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_0001001A",    "BackLeftMidDiffuse",   .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_0001001B",    "BackRightMidDiffuse",  .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_0001001C",    "BackLeftMid",          .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_0001001D",    "BackRightMid",         .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_0001001E",    "TopBackLeftMid",       .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_0001001F",    "TopBackRightMid",      .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010020",    "LowFrequencyEffectsL", .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010021",    "LowFrequencyEffectsR", .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010022",    "TopFrontLeftMid",      .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010023",    "TopFrontRightMid",     .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010024",    "FrontLeftScreen",      .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010025",    "FrontRightScreen",     .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010026",    "FrontLeftMid",         .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010027",    "FrontRightMid",        .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL},
    {.id.data="AC_00010028",    "UpperTopBackCentre",   .blkfmts.num=0, .blkfmts.max=1, .blkfmts.array=NULL}
};


static const dlb_sadm_block_format common_blkfmt[NUM_COMMON_AUDIO_CHANNEL_FORMATS] = 
{  /* ID, speaker_label, gain, cartesian, x, y, z */
    {.id.data="AB_00010001_00000001", "M+030",          0, 1, -1, 1, 0},        //FrontLeft
    {.id.data="AB_00010002_00000001", "M-030",          0, 1,  1, 1, 0},        //FrontRight
    {.id.data="AB_00010003_00000001", "M+000",          0, 1,  0, 1, 0},        //FrontCentre
    {.id.data="AB_00010004_00000001", "LFE",            0, 1, -1, 1,-1},        //LowFrequencyEffects
    {.id.data="AB_00010005_00000001", "M+110",          0, 1, -1, 0, 0},        //SurroundLeft
    {.id.data="AB_00010006_00000001", "M-110",          0, 1,  1, 0, 0},        //SurroundRight
    {.id.data="AB_00010007_00000001", "M+022",          0, 1,  0, 1, 0},        //FrontLeftOfCentre
    {.id.data="AB_00010008_00000001", "M-022",          0, 1,  0, 1, 0},        //FrontRightOfCentre
    {.id.data="AB_00010009_00000001", "M+180",          0, 1,  0, 1, 0},        //BackCentre
    {.id.data="AB_0001000A_00000001", "M+090",          0, 1, -1,-1, 0},        //SideLeft
    {.id.data="AB_0001000B_00000001", "M-090",          0, 1,  1,-1, 0},        //SideRight
    {.id.data="AB_0001000C_00000001", "T+000",          0, 1,  0, 1, 0},        //TopCentre
    {.id.data="AB_0001000D_00000001", "U+030",          0, 1,  0, 1, 0},        //TopFrontLeft
    {.id.data="AB_0001000E_00000001", "U-000",          0, 1,  0, 1, 0},        //TopFrontCentre
    {.id.data="AB_0001000F_00000001", "U-030",          0, 1,  0, 1, 0},        //TopFrontRight
    {.id.data="AB_00010010_00000001", "U+110",          0, 1,  0, 1, 0},        //TopSurroundLeft
    {.id.data="AB_00010011_00000001", "U+180",          0, 1,  0, 1, 0},        //TopBackCentre
    {.id.data="AB_00010012_00000001", "U-110",          0, 1,  0, 1, 0},        //TopSurroundRight
    {.id.data="AB_00010013_00000001", "U+090",          0, 1,  0, 1, 0},        //TopSideLeft
    {.id.data="AB_00010014_00000001", "U-090",          0, 1,  0, 1, 0},        //TopSideRight
    {.id.data="AB_00010015_00000001", "B+000",          0, 1,  0, 1, 0},        //BottomFrontCentre
    {.id.data="AB_00010016_00000001", "B+045",          0, 1,  0, 1, 0},        //BottomFrontLeftMid
    {.id.data="AB_00010017_00000001", "B-045",          0, 1,  0, 1, 0},        //BottomFrontRightMid
    {.id.data="AB_00010018_00000001", "M+060",          0, 1,  0, 1, 0},        //FrontLeftWide
    {.id.data="AB_00010019_00000001", "M-060",          0, 1,  0, 1, 0},        //FrontRightWide
    {.id.data="AB_0001001A_00000001", "M+135_Diff",     0, 1,  0, 1, 0},        //BackLeftMidDiffuse
    {.id.data="AB_0001001B_00000001", "M-135_Diff",     0, 1,  0, 1, 0},        //BackRightMidDiffuse
    {.id.data="AB_0001001C_00000001", "M+135",          0, 1, -1,-1, 0},        //BackLeftMid
    {.id.data="AB_0001001D_00000001", "M-135",          0, 1,  1,-1, 0},        //BackRightMid
    {.id.data="AB_0001001E_00000001", "U+135",          0, 1, -1, 0, 1},        //TopBackLeftMid
    {.id.data="AB_0001001F_00000001", "U-135",          0, 1,  1, 0, 1},        //TopBackRightMid
    {.id.data="AB_00010020_00000001", "LFE1",           0, 1, -1, 1, 0},        //LowFrequencyEffectsL
    {.id.data="AB_00010021_00000001", "LFE2",           0, 1,  1, 1, 0},        //LowFrequencyEffectsR
    {.id.data="AB_00010022_00000001", "U+045",          0, 1, -1, 1, 1},        //TopFrontLeftMid
    {.id.data="AB_00010023_00000001", "U-045",          0, 1,  1, 1, 1},        //TopFrontRightMid
    {.id.data="AB_00010024_00000001", "M+SC",           0, 1,  0, 1, 0},        //FrontLeftScreen
    {.id.data="AB_00010025_00000001", "M-SC",           0, 1,  0, 1, 0},        //FrontRightScreen
    {.id.data="AB_00010026_00000001", "M+045",          0, 1,  0, 1, 0},        //FrontLeftMid
    {.id.data="AB_00010027_00000001", "M-045",          0, 1,  0, 1, 0},        //FrontRightMid
    {.id.data="AB_00010028_00000001", "UH+180",         0, 1,  0, 1, 0},        //UpperTopBackCentre
};

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif


dlb_pmd_success
dlb_sadm_init_common_definitions
    (dlb_sadm_model *model
    )
{
    unsigned int i;
    dlb_sadm_idref idref;
    dlb_sadm_idref chanfmt_idref;
    dlb_sadm_block_format current_blkfmt;
    dlb_sadm_channel_format current_chanfmt;
    dlb_sadm_pack_format current_pacfmt;
    dlb_sadm_track_uid silent_track_uid;
    const char *silent_track_id = "ATU_00000000";
    dlb_sadm_idref blkfmt_array[1];
    dlb_sadm_idref chanfmt_array[NUM_COMMON_PACKFMT_CHANFMT];
    dlb_sadm_counts limits;

    for (i = 0; i < NUM_COMMON_AUDIO_CHANNEL_FORMATS; ++i)
    {
        current_blkfmt = common_blkfmt[i];
        current_chanfmt = common_chanfmt[i];
        current_chanfmt.blkfmts.array = blkfmt_array;
        /* Add audioBlockFormat */
        if (   dlb_sadm_lookup_reference(model, current_blkfmt.id.data , DLB_SADM_BLOCKFMT, 0, &idref)
            || dlb_sadm_idref_defined(idref))
        {
            if (   dlb_sadm_set_block_format(model, &current_blkfmt, NULL)
                || dlb_sadm_idref_set_is_common_def(idref, PMD_TRUE))
            {
                return PMD_FAIL;
            }
        }
        else
        {
            dlb_sadm_set_error(model, "common block format \"%s\" has already been defined!\n", current_blkfmt.id.data);
            return PMD_FAIL;
        }

        /* Add audioChannelFormat and assign audioBlockFormat */
        if (   dlb_sadm_lookup_reference(model, current_chanfmt.id.data , DLB_SADM_CHANFMT, 0, &chanfmt_idref)
            || dlb_sadm_idref_defined(chanfmt_idref))
        {
            /* There is only one block format per channel format in common definitions*/
            current_chanfmt.blkfmts.array[0] = idref;
            current_chanfmt.blkfmts.num++;
            if (   dlb_sadm_set_channel_format(model, &current_chanfmt, NULL)
                || dlb_sadm_idref_set_is_common_def(chanfmt_idref, PMD_TRUE))
            {
                return PMD_FAIL;
            }
        }
        else
        {
            dlb_sadm_set_error(model, "common channel format \"%s\" has already been defined!\n", current_chanfmt.id.data);
            return PMD_FAIL;
        }
    }
    
    /* Add audioPackFormats */
    for (i = 0; i < NUM_COMMON_AUDIO_PACK_FORMATS; ++i)
    {
        current_pacfmt = common_packfmt[i];
        current_pacfmt.chanfmts.array = chanfmt_array;

        dlb_sadm_model_limits(model, &limits);
        /* Skip audio pack formats which exceeds limits */
        if (current_pacfmt.chanfmts.max <= limits.max_packfmt_chanfmts)
        {
            if (   dlb_sadm_lookup_reference(model, common_packfmt[i].id.data, DLB_SADM_PACKFMT, 0, &idref)
                || dlb_sadm_idref_defined(idref))
            {

                // Fill in pack format
                for (; current_pacfmt.chanfmts.num < current_pacfmt.chanfmts.max; current_pacfmt.chanfmts.num++)
                {
                    unsigned int idx = common_packfmt_chanfmt[i][current_pacfmt.chanfmts.num];
                    if (dlb_sadm_lookup_reference(model, common_chanfmt[idx].id.data , DLB_SADM_CHANFMT, 0, &current_pacfmt.chanfmts.array[current_pacfmt.chanfmts.num]))
                    {
                        dlb_sadm_set_error(model, "common pack format \"%s\" can not resolve reference \"%s\"\n", current_pacfmt.id.data, common_chanfmt[idx].id.data);
                        return PMD_FAIL;
                    }
                }

                if (   dlb_sadm_set_pack_format(model, &current_pacfmt, NULL)
                    || dlb_sadm_idref_set_is_common_def(idref, PMD_TRUE))
                {
                    return PMD_FAIL;
                }
            }
            else
            {
                dlb_sadm_set_error(model, "common pack format \"%s\" has already been defined!\n", current_pacfmt.id.data);
                return PMD_FAIL;
            }
        }
        // TODO: Add some warning about skipped audio pack formats
    }

    /* Add ATU_00000000 - silent channel */
    if (   dlb_sadm_lookup_reference(model, (const unsigned char *)silent_track_id, DLB_SADM_TRACKUID, 0, &idref)
        || dlb_sadm_idref_defined(idref))
    {
        memset(&silent_track_uid, '\0', sizeof(silent_track_uid));
        memmove(silent_track_uid.id.data, silent_track_id, sizeof(silent_track_uid.id.data));
        /* TODO: the silent channel number needs to be configurable */
        silent_track_uid.channel_idx = SADM_COMMON_DEFAULT_SILENT_CHANNEL;

        if (   dlb_sadm_set_track_uid(model, &silent_track_uid, NULL)
            || dlb_sadm_idref_set_is_common_def(idref, PMD_TRUE))
        {
            return PMD_FAIL;
        }
    }
    else
    {
        dlb_sadm_set_error(model, "common Track UID \"%s\" has already been defined\n", silent_track_id);
        return PMD_FAIL;
    }

    return PMD_SUCCESS;
}
