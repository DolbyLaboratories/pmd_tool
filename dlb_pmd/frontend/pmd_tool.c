/************************************************************************
 * dlb_pmd
 * Copyright (c) 2018, Dolby Laboratories Inc.
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
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>
#include <limits.h>

#ifdef _MSC_VER
#  include <windows.h>
#  if !defined(inline)
#    define inline __inline
#  endif
#else
#  include <unistd.h>
#endif

#include "dlb_pmd_api.h"
#include "dlb_pmd_klv.h"
#include "dlb_pmd_generate.h"

#include "./xml.h"
#include "./klv.h"
#include "./pcm.h"

static const char *FRAME_RATE_NAMES[] =
{
    "23.98", "24", "25", "29.97", "30", "50", "59.94", "60", "100", "119.88", "120"
};

#define NUM_FRAME_RATE_NAMES ((sizeof FRAME_RATE_NAMES)/(sizeof FRAME_RATE_NAMES[0]))


#define PMD_TOOL_VERSION "1.2"


/**
 * @def NO_CHAN
 * @brief symbolic constant meaning channel hasn't been specified on command line
 */
#define NO_CHAN (~0u)


/**
 * @brief known filename suffixes
 *
 * We use input/output file suffix types to guide the type of processing
 * we'd like done.
 */
typedef enum
{
    SUFFIX_NONE = 0,
    SUFFIX_UNKNOWN,
    SUFFIX_XML,
    SUFFIX_KLV,
    SUFFIX_WAV,

    MIN_SUFFIX = SUFFIX_XML,
    MAX_SUFFIX = SUFFIX_WAV,

} suffix_type;
    

/**
 * @brief result of command-line argument parsing
 */
typedef struct
{
    const char *progname;
    const char *in;
    const char *out;

    dlb_pmd_frame_rate rate;
    unsigned int chan;
    dlb_pmd_bool s337m_pair;
    dlb_pmd_bool mark_pcm_blocks;
    unsigned int skip_pcm_samples;
    unsigned int vsync;
    suffix_type insuffix;
    suffix_type outsuffix;
    suffix_type mdsuffix;
    dlb_klvpmd_universal_label ul;

    dlb_pmd_bool generate_random;
    unsigned int random_seed;
    dlb_pmd_metadata_count random_counts;
} Args;


/**
 * @brief encapsulate model and its creation
 */
typedef struct
{
    size_t size;
    void *mem;
    dlb_pmd_model *model;
} model;
    

/**
 * @brief create a PMD model
 */
static inline
int
model_init
    (model *m
    )
{
    m->size = dlb_pmd_query_mem();
    m->mem = malloc(m->size);
    if (NULL == m->mem)
    {
        printf("could not allocate memory\n");
        return 0;
    }
    dlb_pmd_init(&m->model, m->mem);
    return 1;
}


/**
 * @brief destroy a PMD model
 */
static inline
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
suffix_type                   /** @return suffix type */
read_file_suffix
    (const char *filename     /**< [in] filename */
    )
{
    size_t len = strlen(filename);
    const char *ptr = filename + len - 1;

    while (*ptr != '\\' && *ptr != '/' && *ptr != '.' && ptr != filename)
    {
        --ptr;
    }
    
    if (*ptr != '.')
    {
        printf("ERROR: no suffix\n");
        return SUFFIX_NONE;
    }

    ++ptr;
    if (0 == strncmp(ptr, "xml", 4))
    {
        return SUFFIX_XML;
    }
    if (0 == strncmp(ptr, "klv", 4))
    {
        return SUFFIX_KLV;
    }
    if (0 == strncmp(ptr, "wav", 4))
    {
        return SUFFIX_WAV;
    }

    return SUFFIX_UNKNOWN;
}


/**
 * @brief check supported suffix
 */
static inline
int
check_suffixes
    (Args *args
    )
{
    if (!args->generate_random)
    {
        args->insuffix = read_file_suffix(args->in);
        if (MIN_SUFFIX > args->insuffix || MAX_SUFFIX < args->insuffix)
        {
            printf("ERROR: unsupported input file type\n");
            return 0;
        }
    }
    args->outsuffix = read_file_suffix(args->out);
    if (MIN_SUFFIX > args->outsuffix || MAX_SUFFIX < args->outsuffix)
    {
        printf("ERROR: unsupported output file type\n");
        return 0;
    }
    return 1;
}


/**
 * @brief parse video frame rate command-line option
 */
static inline
int                        /** @return 1 on success, 0 on failure */
parse_frame_rate
    (Args *args            /**< [in] argument structure to populate */
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
static inline
int                        /** @return 1 on success, 0 on failure */
parse_pair_number
    (Args *args            /**< [in] argument structure to populate */
    ,const char *arg       /**< [in] option argument to parse */
    ,dlb_pmd_bool is_pair  /**< [in] 0 for single channel, 1 for pair */
    )
{
    long int tmp;
    char *endp;

    tmp = strtol(arg, &endp, 0);
    if (endp == arg || tmp < 0 || tmp > INT_MAX)
    {
        printf("Error: '%s' requires a positive integer, not '%s'\n",
               is_pair ? "-pair" : "-chan", arg);
        return 0;
    }
    args->chan = (unsigned int)tmp * (is_pair ? 2 : 1);
    return 1;
}


/**
 * @brief parse an unsigned integer valued argument
 */
static inline
int                          /** @return 1 on success, 0 otherwise */
parse_uint_arg
   (int argc                 
   ,const char **argv
   ,const char *opt
   ,unsigned int min
   ,unsigned int max
   ,unsigned int *value
   )
{
    long int tmp;
    char *endp;
    
    if (!argc)
    {
        printf("ERROR: option %s expects an integral argument\n", opt);
        return 0;
    }
    tmp = strtol(*argv, &endp, 0);
    if (endp == *argv || tmp < (long)min || tmp > (long)max)
    {
        printf("ERROR: option %s requires a positive integer "
               "between %u and %u inclusing, not \"%s\"\n", opt, min, max, *argv);
        return 0;
    }
    *value = (unsigned int)tmp;
    return 1;
}
    

/**
 * @brief parse command-line arguments
 */
static
int
chkargs
    (Args *args
    ,int argc
    ,const char **argv
    )
{
    struct stat info;
    unsigned int i;
    
    args->in = NULL;
    args->out = NULL;
    args->insuffix = SUFFIX_UNKNOWN;
    args->outsuffix = SUFFIX_UNKNOWN;
    args->progname = *argv;
    args->rate = DLB_PMD_FRAMERATE_2500;
    args->chan = NO_CHAN;
    args->s337m_pair = 1;
    args->mark_pcm_blocks = 0;
    args->skip_pcm_samples = 0;
    args->vsync = 0;
    args->ul = DLB_PMD_KLV_UL_DOLBY;
    args->generate_random = 0;
    args->random_seed = 0;

    memset(&args->random_counts, '\0', sizeof(args->random_counts));
    args->random_counts.num_signals        = PMD_GENERATE_RANDOM_NUMBER;
    args->random_counts.num_beds           = PMD_GENERATE_RANDOM_NUMBER;
    args->random_counts.num_objects        = PMD_GENERATE_RANDOM_NUMBER;
    args->random_counts.num_presentations  = PMD_GENERATE_RANDOM_NUMBER;
    args->random_counts.num_loudness       = PMD_GENERATE_RANDOM_NUMBER;
    args->random_counts.num_iat            = PMD_GENERATE_RANDOM_NUMBER;
    args->random_counts.num_eac3           = PMD_GENERATE_RANDOM_NUMBER;
    args->random_counts.num_ed2_turnarounds= PMD_GENERATE_RANDOM_NUMBER;
    args->random_counts.num_headphone_desc = PMD_GENERATE_RANDOM_NUMBER;

    --argc;
    ++argv;

    while (argc)
    {
        const char *opt = *argv;
        
        if (0 == strncmp(opt, "-v", 3))
        {
            printf("PMD TOOL Version " PMD_TOOL_VERSION "\n");
            exit(0);
        }
        else if (0 == strncmp(opt, "-i", 3))
        {
            --argc;
            ++argv;
            if (!argc)
            {
                printf("ERROR: option -i expects a filename parameter\n");
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
                printf("ERROR: option -o expects a filename parameter\n");
                goto error;
            }
            args->out = *argv;
        }
        else if (0 == strncmp(opt, "-fr", 4))
        {
            --argc;
            ++argv;
            if (!argc)
            {
                printf("ERROR: option -fr expects a frame rate parameter\n");
                goto error;
            }
            if (!parse_frame_rate(args, *argv))
            {
                printf("ERROR: unknown frame rate\n");
                goto error;
            }
        }
        else if (0 == strncmp(opt, "-pair", 6))
        {
            --argc;
            ++argv;
            if (!argc)
            {
                printf("ERROR: option -pair expects a pair number\n");
                goto error;
            }
            if (!parse_pair_number(args, *argv, 1))
            {
                goto error;
            }
            args->s337m_pair = 1;
        }
        else if (0 == strncmp(opt, "-chan", 6))
        {
            --argc;
            ++argv;
            if (!argc)
            {
                printf("ERROR: option -pair expects a pair number\n");
                goto error;
            }
            if (!parse_pair_number(args, *argv, 0))
            {
                goto error;
            }
            args->s337m_pair = 0;
        }
        else if (0 == strncmp(opt, "-st2109", 8))
        {
            args->ul = DLB_PMD_KLV_UL_ST2109;
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
            args->generate_random = 1;
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
            if (!parse_uint_arg(argc, argv, opt, 1, 255, &args->random_counts.num_eac3))
            {
                goto error;
            }
        }
        else if (0 == strncmp(opt, "-nt", 4))
        {
            --argc;
            ++argv;
            if (!parse_uint_arg(argc, argv, opt, 1, 255, &args->random_counts.num_ed2_turnarounds))
            {
                goto error;
            }
        }        
        else if (0 == strncmp(opt, "-nh", 4))
        {
            --argc;
            ++argv;
            if (!parse_uint_arg(argc, argv, opt, 1, 255, &args->random_counts.num_headphone_desc))
            {
                goto error;
            }
        }        
        else
        {
            printf("ERROR: unknown filename options %s\n", opt);
            goto error;
        }

        --argc;
        ++argv;
    }

    if ((NULL == args->in && !args->generate_random) || NULL == args->out)
    {
        printf("ERROR: not enough arguments\n");
        goto error;
    }

    if (args->generate_random)
    {
        if (args->random_counts.num_beds != PMD_GENERATE_RANDOM_NUMBER &&
            args->random_counts.num_beds != PMD_GENERATE_RANDOM_NUMBER)
        {
            if (args->random_counts.num_beds + args->random_counts.num_objects == 0)
            {
                printf("ERROR: attempting to generate a model with no elements\n");
                goto error;
            }
            if (args->random_counts.num_beds + args->random_counts.num_objects > 4095)
            {
                printf("ERROR: attempting to generate a model with too many elements\n");
                goto error;
            }
        }
    }
    else if (stat(args->in, &info))
    {
        printf("ERROR: input file is not accessible '%s'\n", args->in);
        return -1;
    }

    if (!check_suffixes(args))
    {
        return 0;
    }
    return 1;

  error:
    printf("usage:\n");
    printf("    %s <opt>*\n", args->progname);
    printf("    where <opt> can be:\n");
    printf("        -v            - print tool version and exit\n");
    printf("        -i <filename> - input filename: .xml, .klv, or .wav\n");
    printf("        -o <filename> - output filename:  .xml, .klv, or .wav\n");
    printf("               see note below when -o is .wav\n");
    printf("        -rand         - generate a random model instead of using an input\n");
    printf("                        EXPERIMENTAL!\n");
    printf("        -seed <num>   - if generating a random model, the 32-bit seed to use\n");
    printf("        -ns <num>     - if generating a random model, the number of signals [1-255]\n");
    printf("        -nb <num>     - if generating a random model, the number of beds [0 - 4095]\n");
    printf("        -no <num>     - if generating a random model, the number of objects [0 - 4095] \n");
    printf("        -np <num>     - if generating a random model, the number of presentations [1-511]\n");
    printf("        -nl <num>     - if generating a random model, the number of loudness payloads [1-511]\n");
    printf("                           (will be rounded down to number of presentations, if it is lower)\n");
    printf("        -ne <num>     - if generating a random model, the number of EAC3 encoding parameters\n");
    printf("        -nt <num>     - if generating a random model, the number of ED2 turnarounds\n");
    printf("        -nh <num>     - if generating a random model, the number of Headphone Element Desriptions\n");
    printf("        -fr <rate> - PCM frame rate, where <rate> may be one of\n");
    printf("             ");
    for (i = 0; i != NUM_FRAME_RATE_NAMES; ++i)
    {
        printf("%s ", FRAME_RATE_NAMES[i]);
    }
    printf("\n");
    printf("       -pair <pcm pair number>: pair in PCM to look for/write SMPTE-337m encoded PMD\n");
    printf("       -chan <pcm chan number>: channel in PCM to look for/write SMPTE-337m encoded PMD\n");
    printf("       -st2109: use SMPTE 2109 Audio Metadata Universal Label instead of Dolby Private\n");
    printf("       -mark-pcm-blocks: insert SMPTE 337m NULL-frames at PCM block boundaries\n");
    printf("            even when there is not data for that PCM block.\n");
    printf("       -skip-pcm <count>: simulate random access into PCM stream by skipping this\n");
    printf("            many samples from start of .wav file when decoding PCM+PMD\n");
    printf("       -vsync <offset>: (PCM+PMD read only) samples from beginning of .wav file where\n");
    printf("            first video frame boundary occurs\n");
    printf("\n");
    printf("NOTE:  if generating a random model via the -rand argument,\n");
    printf("          the -seed, -ns, -nb, -no, -np, -nl, -ne, -nt and -nh arguments are optional\n");
    printf("          (These arguments are ignored if not generating a random model\n");
    printf("       Note also that only signals, beds, objects and presentations are\n");
    printf("       currently generated\n");
    printf("\n");
    printf("NOTE: \"KLV\" format refers to the SMPTE 336 Key-Length-Value encoding of the\n");
    printf("      PMD metadata\n");
    printf("\n");
    printf("NOTE: the suffix of the input and output file determine the kind of processing\n");
    printf("\n");
    printf("NOTE: if neither -pair nor -chan are specified in PCM workflows, the default\n");
    printf("      is to use final pair of channels in PCM\n");
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
    printf("         last two channels of the PCM have been overwritten with SMPTE 337m-\n");
    printf("         wrapped KLV metadata, at 160 sample chunks within the specified\n");
    printf("         video framerate.\n");
    printf("         NOTE that the PCM must have an even number of channels.\n");
    printf("         NOTE also that you could read metadata from KLV format instead by\n");
    printf("         changing the .xml suffix to .klv\n");
    printf("\n");
    printf("     4. to extract metadata from PCM and dump to XML:\n");
    printf("           pmd_tool -i <pcm>.wav -o <md>.xml\n");
    printf("\n");
    printf("         If there is metadata on last two channels of the PCM (and the number\n");
    printf("         of channels is even), then extract the SMPTE-337m wrapped KLV and\n");
    printf("         dump it to XML format.\n");
    printf("         NOTE also that you could write metadata to KLV format instead, by\n");
    printf("         replacing the .xml suffix with .klv\n");
    printf("\n\n");
    return 0;
}


/**
 * @brief read file in chosen format
 */
static
int                           /** @return 0 on success, 1 on failure */
read_input
    (Args *args               /**< [in] command-line arguments */
    ,dlb_pmd_model *model     /**< [out] PMD model to read */
    )

{
    dlb_pmd_frame_rate rate = args->rate;
    dlb_pmd_bool ispair = args->s337m_pair;
    unsigned int chan = args->chan;
    unsigned int vsync = args->vsync;
    unsigned int skip = args->skip_pcm_samples;
    const char *in = args->in;

    if (args->generate_random)
    {
        if (dlb_pmd_generate_random(model, &args->random_counts, args->random_seed))
        {
            printf("Failed to generate random model: %s\n", dlb_pmd_error(model));
            return 1;
        }
        return 0;
    }
    else
    {
        switch (args->insuffix)
        {
        case SUFFIX_XML:   return xml_read(in, model);
        case SUFFIX_KLV:   return klv_read(in, model);
        case SUFFIX_WAV:   return pcm_read(in, rate, chan, ispair, vsync, skip, model);
        default:           return 1;
        }
    }
}


/**
 * @brief helper function to generate pcm+pmd output .wav file name
 */
static inline
void
generate_output_filename
    (const char  *filename        /**< [in] input .wav filename */
    ,      char  *outfile         /**< [out] array to store generated filename */
    ,      size_t outfile_size    /**< [in] size of #outfile array */
    )
{
    size_t len = strlen(filename);
    const char *ptr = filename + len - 1;

    if (len > outfile_size)
    {
        printf("input PCM filename too long\n");
        abort();
    }

    while (*ptr != '\\' && *ptr != '/' && *ptr != '.' && ptr != filename)
    {
        --ptr;
    }
    
    if (*ptr != '.')
    {
        sprintf(outfile, "%s_klv.wav", filename);
    }
    else
    {
        strncpy(outfile, filename, ptr - filename);
        outfile += (ptr - filename);
        sprintf(outfile, "_klv.wav");
    }
}


/**
 * @brief write to chosen output format
 */
static inline
int                           /** @return 0 on success, 1 on failure */
write_output
    (Args *args               /**< [in] command-line arguments */
    ,dlb_pmd_model *model     /**< [in] PMD model to write */
    )
{
    char pcm_out[256];
    dlb_klvpmd_universal_label ul = args->ul;
    dlb_pmd_frame_rate rate = args->rate;
    unsigned int chan = args->chan;
    dlb_pmd_bool ispair = args->s337m_pair;
    dlb_pmd_bool mark = args->mark_pcm_blocks;
    const char *out = args->out;

    generate_output_filename(out, pcm_out, sizeof(pcm_out));

    switch (args->outsuffix)
    {
    case SUFFIX_XML:   return xml_write(out, model);
    case SUFFIX_KLV:   return klv_write(out, model, ul);
    case SUFFIX_WAV:   return pcm_write(out, pcm_out, rate, chan, ispair, ul, mark, model);
    default:           return 1;
    }
}


int
main
    (int argc
    ,const char *argv[]
    )
{
    int res = 0;
    Args args;
    model m;

    if (!chkargs(&args, argc, argv))
    {
        return -1;
    }
    
    if (model_init(&m))
    {
        res = read_input(&args, m.model)
           || write_output(&args, m.model);
        model_finish(&m);
    }
    return res;
}
