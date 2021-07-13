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
 * @file pmd_tool.c
 * @brief reader/writer program from Professional Metadata library
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>

#include "pmd_tool.h"

#include "dlb_pmd_api.h"
#include "dlb_pmd_generate.h"

#include "xml.h"
#include "pmd_tool_klv.h"
#include "pcm.h"

#include "pmd_tool_build_version.h"

static const char *FRAME_RATE_NAMES[] =
{
    "23.98", "24", "25", "29.97", "30", "50", "59.94", "60", "100", "119.88", "120"
};

#define NUM_FRAME_RATE_NAMES ((sizeof FRAME_RATE_NAMES)/(sizeof FRAME_RATE_NAMES[0]))


/**
 * @brief encapsulate model and its creation
 */
typedef struct
{
    size_t         size;
    void          *mem;
    dlb_pmd_model *model;
} model;


/**
 * @brief print the PMD Tool version number to stdout
 */
void
dlb_pmd_tool_print_version(void)
{
    unsigned int epoch;
    unsigned int maj;
    unsigned int min;
    unsigned int build;
    unsigned int bs_maj;
    unsigned int bs_min;

    dlb_pmd_library_version(&epoch, &maj, &min, &build, &bs_maj, &bs_min);
    printf("%u.%u.%u.%u-%u.%u (%u)", epoch, maj, min, build, bs_maj, bs_min, PMD_TOOL_BUILD_VERSION);
}


/**
 * @brief print the PMD Tool usage instructions to stdout
 */
void dlb_pmd_tool_usage
    (const Args *args
    )
{
    size_t i;

    printf("usage:\n");
    printf("    %s <opt>*\n", args == NULL ? "pmd_tool" : args->progname);
    printf("    where <opt> can be:\n");
    printf("        -v              - print tool version and exit\n");
    printf("        -i <filename>   - input filename:  .xml, .klv or .wav\n");
    printf("        -o <filename>   - output filename: .xml, .klv or .wav\n");
    printf("                          (see note below when -o is .wav)\n");
    printf("        -log <filename> - log file name for .wav input, may also be stdout or stderr\n");
    printf("                          (optional)\n");
    printf("        -chan <channel> - PCM channel to read/write SMPTE-337m encoded metadata [0-n]\n");
    printf("        -pair <pair>    - PCM pair to read/write SMPTE-337m encoded metadata [0-n]\n");
    printf("                          (use either -chan or -pair, not both!)\n");
    printf("        -fr <rate>      - video frame rate, where <rate> may be one of:\n");
    printf("                          ");
    for (i = 0; i != NUM_FRAME_RATE_NAMES; ++i)
    {
        printf("%s ", FRAME_RATE_NAMES[i]);
    }
    printf("\n");
    printf("                          (default: 25)\n");
    printf("        -unstrict-xml   - relax stringent XML field checking of the presentation config string\n");
    printf("        -skip-pcm <count> - (PCM+PMD read) simulate random access into a PCM stream by skipping\n");
    printf("                          this many samples from the start of the .wav file\n");
    printf("        -vsync <offset> - (PCM+PMD read) number of samples from the beginning of the .wav file\n");
    printf("                          where the first video frame boundary occurs\n");
    printf("        -Dolby          - use Dolby Private Universal Label instead of SMPTE 2109\n");
    printf("        -mark-pcm-blocks - insert SMPTE 337m NULL-frames at PCM block boundaries\n");
    printf("                          even when there is no data for that PCM block\n");
    printf("        -sadm           - generate serial ADM output in XML output mode\n");
    printf("                          EXPERIMENTAL!\n");
    printf("        -sadm-common    - use ADM common definitions for serial ADM\n");
    printf("                          EXPERIMENTAL!\n");
    printf("\n");
    printf("        -try-frame      - for XML input, try to fit the resulting model into one video frame,\n");
    printf("                          with the given frame rate and number of metadata channels (single\n");
    printf("                          or pair), and print the status to stdout\n");
    printf("            status - ERROR:  an error occurred trying to write the model\n");
    printf("                   - GREEN:  the model fits in a single frame\n");
    printf("                   - YELLOW: some names are dropped to fit the model in a single frame\n");
    printf("                   - RED:    the model does not fit in a single frame\n");
    printf("\n");
    printf("        -rand           - generate a random model instead of using an input\n");
    printf("                          EXPERIMENTAL!\n");
    printf("        -seed <num>     - 32-bit seed to use for pseudo-random number generation\n");
    printf("        -ns <num>       - number of signals [1-255]\n");
    printf("        -nb <num>       - number of beds [0-4095]\n");
    printf("        -no <num>       - number of objects [0-4095] \n");
    printf("        -np <num>       - number of presentations [1-511]\n");
    printf("        -nl <num>       - number of loudness payloads [0-511]\n");
    printf("                          (will be limited to the number of presentations)\n");
    printf("        -ne <num>       - number of EAC3 encoding parameters [0-255]\n");
    printf("        -nt <num>       - number of ED2 turnarounds [0-255]\n");
    printf("        -nh <num>       - number of Headphone Element Descriptions [0-255]\n");
    printf("        -gas            - restrict all names to printable ascii\n");
    printf("\n");
    printf("NOTE: if generating a random model via the -rand argument, the related arguments are\n");
    printf("      optional (they are ignored if not generating a random model).\n");
    printf("\n");
    printf("NOTE: the suffixes of the input and output files determine the kind of processing.\n");
    printf("\n");
    printf("NOTE: \"KLV\" format refers to the SMPTE 336 Key-Length-Value encoding of the metadata.\n");
    printf("\n");
    printf("NOTE: if neither -pair nor -chan are specified in PCM workflows, the default behavior\n");
    printf("      is to use the first pair of channels (equivalent to \"-pair 0\").\n");
    printf("\n");
    printf("EXAMPLES:\n");
    printf("\n");
    printf("      1. to convert PMD metadata in XML format to KLV format:\n");
    printf("            pmd_tool -i <file>.xml -o <file>.klv\n");
    printf("\n");
    printf("      2. to convert KLV back into XML format, use\n");
    printf("            pmd_tool -i <file>.klv -o <file>.xml\n");
    printf("\n");
    printf("      3. to add metadata in XML format to PCM in a .wav file:\n");
    printf("            pmd_tool -i <md>.xml -o <pcm>.wav -fr <framerate>\n");
    printf("\n");
    printf("         This will take <md>.xml as input metadata and <pcm>.wav as *input*\n");
    printf("         PCM.  It will create a new file <pcm>_klv.wav as output, where the\n");
    printf("         first two channels of the PCM have been overwritten with SMPTE 337m-\n");
    printf("         wrapped KLV metadata, at 160 sample chunks within the specified\n");
    printf("         video framerate.\n");
    printf("         NOTE: the PCM must have an even number of channels, unless you use\n");
    printf("         the -chan option.\n");
    printf("         NOTE: to read metadata from KLV format instead, change the .xml\n");
    printf("         suffix to .klv.\n");
    printf("\n");
    printf("     4. to extract metadata from PCM and dump to XML:\n");
    printf("           pmd_tool -i <pcm>.wav -o <md>.xml\n");
    printf("\n");
    printf("        If there is metadata on the first two channels of the PCM, extract\n");
    printf("        the SMPTE-337m wrapped KLV and write the equivalent data in XML format.\n");
    printf("        NOTE: to write metadata to KLV format instead, change the .xml suffix\n");
    printf("        to .klv.\n");
    printf("\n");
    printf("    5.  to copy metadata from one .wav file to another:\n");
    printf("          pmd_tool -i <pcm1>.wav -o <pcm2>.wav\n");
    printf("\n");
    printf("        Assuming that both inputs are PCM with even number of channels, and\n");
    printf("        that <pcm1>.wav has SMPTE-337m encoded metadata in first pair,\n");
    printf("        this will decode the KLV metadata and write it out again onto the\n");
    printf("        first pair of <pcm2>.wav, to produce <pcm2>_klv.wav.\n");
    printf("\n");
    printf("\n");
}


/**
 * @brief create a PMD model
 */
static
int
model_init
    (model          *m
    ,const Args     *args
    )
{
    dlb_pmd_model_constraints c;

    dlb_pmd_max_constraints(&c, args->sadm_common);
    m->size = dlb_pmd_query_mem_constrained(&c);
    m->mem  = malloc(m->size);
    if (NULL == m->mem)
    {
        fprintf(stderr, "could not allocate memory\n");
        return 0;
    }
    dlb_pmd_init_constrained(&m->model, &c, m->mem);
    return 1;
}


/**
 * @brief destroy a PMD model
 */
static
void
model_finish
    (model *m
    )
{
    dlb_pmd_finish(m->model);
    free(m->mem);
}


/**
 * @brief helper function to determine filename type
 *
 * We use the filename suffixes to determine what kind of operation
 * we want to achieve.  For instanece, if the input filename is a.xml
 * and the output b.klv, we want to translate metadata in XML format
 * into the serialized KLV format.
 */
static
mode                          /** @return file's mode */
read_file_mode
    (const char *filename     /**< [in] filename */
    )
{
    size_t      len = strlen(filename);
    const char *ptr = filename + len - 1;

    while ((*ptr != '\\') && (*ptr != '/') && (*ptr != '.') && (ptr != filename))
    {
        --ptr;
    }

    if (*ptr != '.')
    {
        fprintf(stderr, "ERROR: no suffix\n");
        return MODE_NONE;
    }

    ++ptr;

    if (0 == strncmp(ptr, "xml", 4))
    {
        return MODE_XML;
    }
    if (0 == strncmp(ptr, "klv", 4))
    {
        return MODE_KLV;
    }
    if (0 == strncmp(ptr, "wav", 4))
    {
        return MODE_WAV;
    }

    return MODE_UNKNOWN;
}


/**
 * @brief check supported suffix
 */
static
int
check_modes
    (Args *args
    )
{
    if (args->inmode != MODE_PRNG)
    {
        args->inmode =
            read_file_mode
                (args->in
                );
        if ((MIN_MODE > args->inmode) || (MAX_MODE < args->inmode))
        {
            fprintf(stderr, "ERROR: unsupported input file type\n");
            return 0;
        }
    }
    args->outmode =
        read_file_mode
            (args->out
            );
    if ((MIN_MODE > args->outmode) || (MAX_MODE < args->outmode))
    {
        fprintf(stderr, "ERROR: unsupported output file type\n");
        return 0;
    }
    return 1;
}


/**
 * @brief parse video frame rate command-line option
 */
static
int                        /** @return 1 on success, 0 on failure */
parse_frame_rate
    (      Args *args      /**< [in] argument structure to populate */
    ,const char *arg       /**< [in] option argument to parse */
    )
{
    int i;

    for (i = 0; i != NUM_FRAME_RATE_NAMES; ++i)
    {
        if (!strcmp(arg, FRAME_RATE_NAMES[i]))
        {
            args->rate = i;
            return 1;
        }
    }
    return 0;
}


/**
 * @brief parse argument of -pair option
 */
static
int                        /** @return 1 on success, 0 on failure */
parse_pair_number
    (      Args         *args     /**< [in] argument structure to populate */
    ,const char         *arg      /**< [in] option argument to parse */
    ,      dlb_pmd_bool  is_pair  /**< [in] 0 for single channel, 1 for pair */
    )
{
    long int  tmp;
    char     *endp;

    tmp = strtol(arg, &endp, 0);
    if ((endp == arg) || (tmp < 0) || (tmp > INT_MAX))
    {
        fprintf(stderr, "Error: '%s' requires a non-negative integer, not '%s'\n",
                is_pair ? "-pair" : "-chan", arg);
        return 0;
    }
    args->chan = (unsigned int)tmp * (is_pair ? 2 : 1);
    return 1;
}


/**
 * @brief parse an unsigned integer valued argument
 */
static
int                          /** @return 1 on success, 0 otherwise */
parse_uint_arg
   (      int            argc
   ,const char         **argv
   ,const char          *opt
   ,      unsigned int   min
   ,      unsigned int   max
   ,      unsigned int  *value
   )
{
    long int  tmp;
    char     *endp;

    if (!argc)
    {
        fprintf(stderr, "ERROR: option %s expects an integral argument\n", opt);
        return 0;
    }
    tmp = strtol(*argv, &endp, 0);
    if ((endp == *argv) || (tmp < (long)min) ||(tmp > (long)max))
    {
        fprintf(stderr, "ERROR: option %s requires a positive integer "
                "between %u and %u inclusive, not \"%s\"\n", opt, min, max, *argv);
        return 0;
    }
    *value = (unsigned int)tmp;
    return 1;
}


/**
 * @brief parse command-line arguments
 */
dlb_pmd_bool
dlb_pmd_tool_parse_cmdline_args
    (      Args  *args
    ,      int    argc
    ,const char **argv
    )
{
    struct stat  info;
    dlb_pmd_bool has_chan_or_pair = PMD_FALSE;

    memset(args, 0, sizeof(*args));

    args->progname               = NULL;
    args->in                     = NULL;
    args->out                    = NULL;
    args->logname                = NULL;
    args->inmode                 = MODE_UNKNOWN;
    args->outmode                = MODE_UNKNOWN;
    args->progname               = *argv;
    args->rate                   = DLB_PMD_FRAMERATE_2500;
    args->chan                   = 0;
    args->s337m_pair             = 1;
    args->sadm_out               = 0;
    args->sadm_common            = 0;
    args->mark_pcm_blocks        = 0;
    args->skip_pcm_samples       = 0;
    args->vsync                  = 0;
    args->ul                     = DLB_PMD_KLV_UL_ST2109;
    args->try_frame              = 0;
    args->strict_xml             = 1;
    args->random_seed            = 0;
    args->generate_ascii_strings = 0;

    args->random_counts.num_signals         = PMD_GENERATE_RANDOM_NUMBER;
    args->random_counts.num_beds            = PMD_GENERATE_RANDOM_NUMBER;
    args->random_counts.num_objects         = PMD_GENERATE_RANDOM_NUMBER;
    args->random_counts.num_presentations   = PMD_GENERATE_RANDOM_NUMBER;
    args->random_counts.num_loudness        = PMD_GENERATE_RANDOM_NUMBER;
    args->random_counts.num_iat             = PMD_GENERATE_RANDOM_NUMBER;
    args->random_counts.num_eac3            = PMD_GENERATE_RANDOM_NUMBER;
    args->random_counts.num_ed2_turnarounds = PMD_GENERATE_RANDOM_NUMBER;
    args->random_counts.num_headphone_desc  = PMD_GENERATE_RANDOM_NUMBER;

    --argc;
    ++argv;

    while (argc)
    {
        const char *opt = *argv;

        if (0 == strncmp(opt, "-v", 3) || 0 == strncmp(opt, "--version", 10))
        {
            printf("PMD TOOL version ");
            dlb_pmd_tool_print_version();
            printf("\n");
            exit(0);
        }
        else if (0 == strncmp(opt, "-i", 3))
        {
            --argc;
            ++argv;
            if (!argc)
            {
                fprintf(stderr, "ERROR: option -i expects a filename parameter\n");
                goto error;
            }
            args->in = *argv;
        }
        else if (0 == strncmp(opt, "-o", 3))
        {
            --argc;
            ++argv;
            if (!argc)
            {
                fprintf(stderr, "ERROR: option -o expects a filename parameter\n");
                goto error;
            }
            args->out = *argv;
        }
        else if (0 == strncmp(opt, "-log", 5))
        {
            --argc;
            ++argv;
            if (!argc)
            {
                fprintf(stderr, "ERROR: option -log expects a filename parameter\n");
                goto error;
            }
            args->logname = *argv;
        }
        else if (0 == strncmp(opt, "-fr", 4))
        {
            --argc;
            ++argv;
            if (!argc)
            {
                fprintf(stderr, "ERROR: option -fr expects a frame rate parameter\n");
                goto error;
            }
            if (!parse_frame_rate(args, *argv))
            {
                fprintf(stderr, "ERROR: unknown frame rate\n");
                goto error;
            }
        }
        else if (0 == strncmp(opt, "-pair", 6))
        {
            --argc;
            ++argv;
            if (!argc)
            {
                fprintf(stderr, "ERROR: option -pair expects a pair number\n");
                goto error;
            }
            if (has_chan_or_pair)
            {
                fprintf(stderr, "ERROR: option -chan or -pair already seen\n");
                goto error;
            }
            if (!parse_pair_number(args, *argv, 1))
            {
                goto error;
            }
            has_chan_or_pair = PMD_TRUE;
            args->s337m_pair = 1;
        }
        else if (0 == strncmp(opt, "-chan", 6))
        {
            --argc;
            ++argv;
            if (!argc)
            {
                fprintf(stderr, "ERROR: option -chan expects a channel number\n");
                goto error;
            }
            if (has_chan_or_pair)
            {
                fprintf(stderr, "ERROR: option -chan or -pair already seen\n");
                goto error;
            }
            if (!parse_pair_number(args, *argv, 0))
            {
                goto error;
            }
            has_chan_or_pair = PMD_TRUE;
            args->s337m_pair = 0;
        }
        else if (0 == strncmp(opt, "-Dolby", 8))
        {
            args->ul = DLB_PMD_KLV_UL_DOLBY;
        }
        else if (0 == strncmp(opt, "-sadm", 6))
        {
            args->sadm_out = 1;
        }
        else if (0 == strncmp(opt, "-sadm-common", 13))
        {
            args->sadm_common = 1;
        }
        else if (0 == strncmp(opt, "-mark-pcm-blocks", 18))
        {
            args->mark_pcm_blocks = 1;
        }
        else if (0 == strncmp(opt, "-skip-pcm", 10))
        {
            --argc;
            ++argv;
            if (!parse_uint_arg(argc, argv, opt, 1, 4096, &args->skip_pcm_samples))
            {
                goto error;
            }
        }
        else if (0 == strncmp(opt, "-vsync", 7))
        {
            --argc;
            ++argv;
            if (!parse_uint_arg(argc, argv, opt, 1, 4096, &args->vsync))
            {
                goto error;
            }
        }
        else if (0 == strncmp(opt, "-rand", 6))
        {
            args->inmode = MODE_PRNG;
        }
        else if (0 == strncmp(opt, "-seed", 6))
        {
            --argc;
            ++argv;
            if (!parse_uint_arg(argc, argv, opt, 0, ~0u, &args->random_seed))
            {
                goto error;
            }
        }
        else if (0 == strncmp(opt, "-ns", 4))
        {
            --argc;
            ++argv;
            if (!parse_uint_arg(argc, argv, opt, 1, 255, &args->random_counts.num_signals))
            {
                goto error;
            }
        }
        else if (0 == strncmp(opt, "-nb", 4))
        {
            --argc;
            ++argv;
            if (!parse_uint_arg(argc, argv, opt, 0, 4095, &args->random_counts.num_beds))
            {
                goto error;
            }
        }
        else if (0 == strncmp(opt, "-no", 4))
        {
            --argc;
            ++argv;
            if (!parse_uint_arg(argc, argv, opt, 0, 4095, &args->random_counts.num_objects))
            {
                goto error;
            }
        }
        else if (0 == strncmp(opt, "-np", 4))
        {
            --argc;
            ++argv;
            if (!parse_uint_arg(argc, argv, opt, 1, 511, &args->random_counts.num_presentations))
            {
                goto error;
            }
        }
        else if (0 == strncmp(opt, "-nl", 4))
        {
            --argc;
            ++argv;
            if (!parse_uint_arg(argc, argv, opt, 0, 511, &args->random_counts.num_loudness))
            {
                goto error;
            }
        }
        else if (0 == strncmp(opt, "-ni", 4))
        {
            --argc;
            ++argv;
            if (!parse_uint_arg(argc, argv, opt, 0, 1, &args->random_counts.num_iat))
            {
                goto error;
            }
        }
        else if (0 == strncmp(opt, "-ne", 4))
        {
            --argc;
            ++argv;
            if (!parse_uint_arg(argc, argv, opt, 0, 255, &args->random_counts.num_eac3))
            {
                goto error;
            }
        }
        else if (0 == strncmp(opt, "-nt", 4))
        {
            --argc;
            ++argv;
            if (!parse_uint_arg(argc, argv, opt, 0, 255, &args->random_counts.num_ed2_turnarounds))
            {
                goto error;
            }
        }
        else if (0 == strncmp(opt, "-nh", 4))
        {
            --argc;
            ++argv;
            if (!parse_uint_arg(argc, argv, opt, 0, 255, &args->random_counts.num_headphone_desc))
            {
                goto error;
            }
        }
        else if (0 == strncmp(opt, "-gas", 4))
        {
            args->generate_ascii_strings = 1;
        }
        else if (0 == strncmp(opt, "-unstrict-xml", 14))
        {
            args->strict_xml = 0;
        }
        else if (0 == strncmp(opt, "-try-frame", 4))
        {
            args->try_frame = 1;
        }
        else
        {
            fprintf(stderr, "ERROR: unknown filename options %s\n", opt);
            goto error;
        }

        --argc;
        ++argv;
    }

    if ((NULL == args->in && MODE_PRNG != args->inmode) || NULL == args->out)
    {
        fprintf(stderr, "ERROR: not enough arguments\n");
        goto error;
    }

    if (MODE_PRNG == args->inmode)
    {
        if (   (args->random_counts.num_beds != PMD_GENERATE_RANDOM_NUMBER)
            && (args->random_counts.num_beds != PMD_GENERATE_RANDOM_NUMBER))
        {
            if ((args->random_counts.num_beds + args->random_counts.num_objects) == 0)
            {
                fprintf(stderr, "ERROR: attempting to generate a model with no elements\n");
                goto error;
            }
            if ((args->random_counts.num_beds + args->random_counts.num_objects) > 4095)
            {
                fprintf(stderr, "ERROR: attempting to generate a model with too many elements\n");
                goto error;
            }
        }
    }
    else if (stat(args->in, &info))
    {
        fprintf(stderr, "ERROR: input file is not accessible '%s'\n", args->in);
        return PMD_FALSE;
    }

    if (!check_modes(args))
    {
        return PMD_FALSE;
    }
    return PMD_TRUE;

  error:
    return PMD_FALSE;
}


/**
 * @brief generate pseudo-random model
 */
static
int                           /** @return 0 on success, 1 on failure */
prng_read
    (Args          *args      /**< [in]  control arguments */
    ,dlb_pmd_model *model     /**< [out] PMD model to read */
    )
{
    if (dlb_pmd_generate_random(model, &args->random_counts, args->random_seed,
                                args->generate_ascii_strings, args->sadm_out))
    {
        fprintf(stderr, "Failed to generate random model: %s\n", dlb_pmd_error(model));
        return 1;
    }
    return 0;
}


/**
 * @brief read file in chosen format
 */
static
int                           /** @return 0 on success, 1 on failure */
read_input
    (const Args    *args      /**< [in]  control arguments */
    ,dlb_pmd_model *model     /**< [out] PMD model to read */
    )

{
    int                  result = 1;
    dlb_pmd_bool         try_it = PMD_FALSE;
    dlb_pmd_bool         sadm   = args->sadm_out;   /* TODO: is this correct? */
    dlb_pmd_frame_rate   rate   = args->rate;
    dlb_pmd_bool         ispair = args->s337m_pair;
    unsigned int         chan   = args->chan;
    unsigned int         vsync  = args->vsync;
    unsigned int         skip   = args->skip_pcm_samples;
    const char          *in     = args->in;

    switch (args->inmode)
    {
    case MODE_XML:
        result = xml_read(in, model, args->strict_xml);
        try_it = args->try_frame;
        break;
    case MODE_KLV:
        result = klv_read(in, model);
        break;
    case MODE_WAV:
        result = pcm_read(in, args->logname, rate, chan, ispair, vsync, skip, model);
        break;
    case MODE_PRNG:
        result = prng_read((Args *)args, model);    /* TODO: it would be nice to keep args const... */
        break;
    default:
        break;
    }

    if (try_it)
    {
        unsigned int n_chan = 2;
        unsigned int n_samp = dlb_pcmpmd_min_frame_size(rate);
        uint32_t *buf = malloc(n_chan * n_samp * sizeof(uint32_t));
        size_t sz = dlb_pcmpmd_augmentor_model_try_frame_query_mem(model, sadm);
        uint8_t *mem = malloc(sz);

        if (buf != NULL && mem != NULL)
        {
            dlb_pcmpmd_write_status write_status = dlb_pcmpmd_augmentor_model_try_frame(model, mem, buf, n_chan, n_samp, rate, ispair, sadm);
            switch (write_status)
            {
            case DLB_PCMPMD_WRITE_STATUS_ERROR:
                printf("Try frame status is ERROR\n");
                break;
            case DLB_PCMPMD_WRITE_STATUS_GREEN:
                printf("Try frame status is GREEN\n");
                break;
            case DLB_PCMPMD_WRITE_STATUS_YELLOW:
                printf("Try frame status is YELLOW\n");
                break;
            case DLB_PCMPMD_WRITE_STATUS_RED:
                printf("Try frame status is RED\n");
                break;
            default:
                printf("Try frame status is UNKNOWN\n");
                break;
            }
            free(mem);
            free(buf);
        }
    }

    return result;
}


/**
 * @brief helper function to generate pcm+pmd output .wav file name
 */
static
void
generate_output_filename
    (const char         *filename        /**< [in]  input .wav filename */
    ,      dlb_pmd_bool  sadm            /**< [in]  is it a sadm .wav file? */
    ,      char         *outfile         /**< [out] array to store generated filename */
    ,      size_t        outfile_size    /**< [in]  size of #outfile array */
    )
{
          size_t  len = strlen(filename);
    const char   *ptr = filename + len - 1;

    if (len > outfile_size)
    {
        fprintf(stderr, "input PCM filename too long\n");
        abort();
    }

    while ((*ptr != '\\') && (*ptr != '/') && (*ptr != '.') && (ptr != filename))
    {
        --ptr;
    }

    if (*ptr != '.')
    {
        sprintf(outfile, "%s_%s.wav", filename, sadm ? "sadm" : "klv");
    }
    else
    {
        strncpy(outfile, filename, ptr - filename);
        outfile += (ptr - filename);
        sprintf(outfile, "_%s.wav", sadm ? "sadm" : "klv");
    }
}


/**
 * @brief write to chosen output format
 */
static
int                           /** @return 0 on success, 1 on failure */
write_output
    (const Args    *args      /**< [in]  control arguments */
    ,dlb_pmd_model *model     /**< [out] PMD model to write */
    )
{
          char                         pcm_out[256];
          dlb_klvpmd_universal_label   ul           = args->ul;
          dlb_pmd_frame_rate           rate         = args->rate;
          unsigned int                 chan         = args->chan;
          dlb_pmd_bool                 ispair       = args->s337m_pair;
          dlb_pmd_bool                 mark         = args->mark_pcm_blocks;
          dlb_pmd_bool                 sadm         = args->sadm_out;
    const char                        *out          = args->out;

    generate_output_filename(out, sadm, pcm_out, sizeof(pcm_out));

    switch (args->outmode)
    {
    case MODE_XML:   return xml_write(out, model, sadm);
    case MODE_KLV:   return klv_write(out, model, ul);
    case MODE_WAV:   return pcm_write(out, pcm_out, rate, chan, ispair, ul, mark, sadm, model);
    case MODE_PRNG:  abort();  /* not possible */
    default:         return 1;
    }
}


/**
 * @brief process files according to the arguments
 */
int
dlb_pmd_tool_process
    (const Args *args
    )
{
    int res = 0;
    model m;

    if (model_init(&m, args))
    {
        res = read_input(args, m.model)
           || write_output(args, m.model);
        model_finish(&m);
    }

    return res;
}
