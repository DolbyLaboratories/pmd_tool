/************************************************************************
 * dlb_pmd
 * Copyright (c) 2021, Dolby Laboratories Inc.
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
 * @file pmd_tool.h
 * @brief reader/writer library from Professional Metadata library
 */

#ifndef PMD_TOOL_H
#define PMD_TOOL_H

#include "dlb_pmd/include/dlb_pmd_lib_dll.h"
#include "dlb_pmd/include/dlb_pmd_types.h"
#include "dlb_pmd/include/dlb_pmd_klv.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief list of input/output modes
 *
 * We use input/output modes, (mostly based on file suffix types) to
 * guide the type of processing we'd like done.
 */
typedef enum
{
    MODE_NONE = 0,
    MODE_UNKNOWN,
    MODE_XML,
    MIN_MODE = MODE_XML,
    MODE_KLV,
    MODE_WAV,
    MAX_MODE = MODE_WAV,
    MODE_PRNG,
} mode;


/**
 * @brief result of command-line argument parsing
 */
typedef struct
{
    const char *progname;
    const char *in;
    const char *out;
    const char *logname;

    dlb_pmd_frame_rate          rate;
    unsigned int                chan;
    dlb_pmd_bool                s337m_pair;
    dlb_pmd_bool                sadm_out;
    dlb_pmd_bool                sadm_common;
    dlb_pmd_bool                mark_pcm_blocks;
    unsigned int                skip_pcm_samples;
    unsigned int                vsync;
    mode                        inmode;
    mode                        outmode;
    mode                        mdmode;
    dlb_klvpmd_universal_label  ul;

    dlb_pmd_bool                try_frame;
    dlb_pmd_bool                strict_xml;
    unsigned int                random_seed;
    dlb_pmd_bool                generate_ascii_strings;
    dlb_pmd_metadata_count      random_counts;
} Args;


/**
 * @brief print the PMD Tool version number to stdout
 */
DLB_PMD_DLL_ENTRY
void
dlb_pmd_tool_print_version
    (void
    );


/**
 * @brief print the PMD Tool usage instructions to stdout
 */
DLB_PMD_DLL_ENTRY
void
dlb_pmd_tool_usage
    (const Args *args   /**< [in] control arguments -- may be NULL */
    );


/**
 * @brief parse command-line arguments
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_bool
dlb_pmd_tool_parse_cmdline_args
    (      Args  *args
    ,      int    argc
    ,const char **argv
    );


/**
 * @brief process files according to the arguments
 */
DLB_PMD_DLL_ENTRY
int
dlb_pmd_tool_process
    (const Args *args
    );


#ifdef __cplusplus
}
#endif

#endif
