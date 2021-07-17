/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2020-2021 by Dolby Laboratories,
 *                Copyright (C) 2020-2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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
