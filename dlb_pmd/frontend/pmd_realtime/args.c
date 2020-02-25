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
 * @file args.h
 * @brief code to parse command-line arguments and populate the command-line
 * args structure
 */


#include "args.h"
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <limits.h>


#ifdef _MSC_VER
#  define NUL_FILE "nul"
#else
#  define NUL_FILE "/dev/null"
#endif


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


dlb_pmd_success
chkargs 
    (Args *args
    ,int argc
    ,const char **argv
    )
{
    struct stat file_info;

    args->mode          = MODE_LIST;
    args->file_in       = NULL;
    args->file_out      = NULL;
    args->md_file_in    = NULL;
    args->md_file_out   = NULL;
    args->server_service= NULL;
    args->server_port   = 0;
    args->in_device     = NO_DEVICE;
    args->channel_count = 2;
    args->out_device    = NO_DEVICE;
    args->time          = 0;
    args->interleaved   = 1;
    args->frame_size    = 1024;

    args->rate          = DLB_PMD_FRAMERATE_2500;
    args->ul            = DLB_PMD_KLV_UL_DOLBY;
    args->pmd_chan      = NO_CHAN;
    args->subframe_mode = 0;
    args->mark_pcm_blocks = 0;
    args->skip_pcm_samples = 0;
    args->vsync         = 0;
    args->sadm          = 0;

    args->desired_input_latency = 0;
    args->desired_output_latency = 0;
    args->loop_playback = 0;
    args->filter_md = 1;

    --argc;
    ++argv;

    if (argc == 0)
    {
        /* this is just 'list devices' */
        return 0;
    }

    /* parse test mode */
    if (0 == strncmp(*argv, "list", 5))
    {
        args->mode = MODE_LIST;
    }
    else if (0 == strncmp(*argv, "play", 5))
    {
        args->mode = MODE_PLAY;
        if (argc == 1)
        {
            printf("ERROR: mode %s requires input filename\n", *argv);
            goto error;
        }
        --argc;
        ++argv;
        if (stat(*argv, &file_info) != 0)
        {
            printf("ERROR: input file does not exist \"%s\"\n", *argv);
            goto error;
        }
        args->file_in= *argv;
    }
    else if (0 == strncmp(*argv, "listen", 7))
    {
        args->mode = MODE_CAPTURE;
    }
    else if (0 == strncmp(*argv, "capture", 8))
    {
        args->mode = MODE_CAPTURE;
        if (argc == 1)
        {
            printf("ERROR: mode %s requires input filename\n", *argv);
            goto error;
        }
        --argc;
        ++argv;
        args->file_out = *argv;
        if (!strcmp(args->file_out, NUL_FILE))
        {
            args->file_out = NULL;
        }
    }
    else if (0 == strncmp(*argv, "pipe", 5))
    {
        args->mode = MODE_PIPE;
    }
    else
    {
        printf("ERROR: unknown test mode \"%s\"\n", *argv);
        goto error;
    }

    --argc;
    ++argv;

    while (argc)
    {
        const char *arg= *argv;
        if (0 == strncmp(arg, "-id", 4))
        {
            --argc;
            ++argv;
            if (!parse_uint_arg(argc, argv, arg, 0, INT_MAX, &args->in_device))
            {
                goto error;
            }
        }
        else if (0 == strncmp(arg, "-od", 4))
        {
            --argc;
            ++argv;
            if (!parse_uint_arg(argc, argv, arg, 0, INT_MAX, &args->out_device))
            {
                goto error;
            }
        }
        else if (0 == strncmp(arg, "-cc", 4))
        {
            --argc;
            ++argv;
            if (!parse_uint_arg(argc, argv, arg, 1, 128, &args->channel_count))
            {
                goto error;
            }
        }
        else if (0 == strncmp(arg, "-mdi", 5))
        {
            --argc;
            ++argv;
            if (!argc)
            {
                printf("ERROR, -mdi requires an input filename\n");
                goto error;
            }
            if (stat(*argv, &file_info) != 0)
            {
                printf("ERROR: -mdi input metadata file does not exist\n");
                goto error;
            }
            args->md_file_in = *argv;
        }
        else if (0 == strncmp(arg, "-mdo", 5))
        {
            --argc;
            ++argv;
            if (!argc)
            {
                printf("ERROR, -mdo requires an input filename\n");
                goto error;
            }
            args->md_file_out = *argv;
        }
        else if (0 == strncmp(arg, "-t", 3))
        {
            --argc;
            ++argv;
            if (!parse_uint_arg(argc, argv, arg, 1, INT_MAX, &args->time))
            {
                goto error;
            }
        }
        else if (0 == strncmp(arg, "-ni", 4))
        {
            printf("Selecting non-interleaved mode\n");
            args->interleaved = 0;
        }
        else if (0 == strncmp(arg, "-fs", 4))
        {
            --argc;
            ++argv;
            if (!parse_uint_arg(argc, argv, arg, 1, 4096, &args->frame_size))
            {
                goto error;
            }
        }
        else if (0 == strncmp(arg, "-chan", 6))
        {
            --argc;
            ++argv;
            if (!parse_uint_arg(argc, argv, arg, 0, 4096, &args->pmd_chan))
            {
                goto error;
            }
            args->subframe_mode = 1;
        }
        else if (0 == strncmp(arg, "-pair", 6))
        {
            --argc;
            ++argv;
            if (!parse_uint_arg(argc, argv, arg, 0, 4096, &args->pmd_chan))
            {
                goto error;
            }
            args->pmd_chan *= 2;
            args->subframe_mode = 0;
        }
        else if (0 == strncmp(arg, "-loop", 6))
        {
            args->loop_playback = 1;
        }
        else if (0 == strncmp(arg, "-allmd", 7))
        {
            args->filter_md = 0;
        }
        else if (0 == strncmp(arg, "-mark-pcm-blocks", 18))
        {
            args->mark_pcm_blocks = 1;
        }
        else if (0 == strncmp(arg, "-skip-pcm", 10))
        {
            --argc;
            ++argv;
            if (!parse_uint_arg(argc, argv, arg, 1, 4096, &args->skip_pcm_samples))
            {
                goto error;
            }
        }
        else if (0 == strncmp(arg, "-vsync", 7))
        {
            --argc;
            ++argv;
            if (!parse_uint_arg(argc, argv, arg, 1, 4096, &args->vsync))
            {
                goto error;
            }
        }
        else if (0 == strncmp(arg, "-dil", 5))
        {
            --argc;
            ++argv;
            if (!argc)
            {
                printf("ERROR: -dil requires an argument\n");
                goto error;
            }
            if (0 == strncmp(*argv, "hi", 3))
            {
                args->desired_input_latency = 0;
            }
            else if (0 == strncmp(*argv, "lo", 3))
            {
                args->desired_input_latency = -1;
            }
            else if (!parse_uint_arg(argc, argv, arg, 1, INT_MAX,
                                     (unsigned int*)&args->desired_input_latency))
            {
                goto error;
            }
        }
        else if (0 == strncmp(arg, "-dol", 5))
        {
            --argc;
            ++argv;
            if (!argc)
            {
                printf("ERROR: -dol requires an argument\n");
                goto error;
            }
            if (0 == strncmp(*argv, "hi", 3))
            {
                args->desired_output_latency = 0;
            }
            else if (0 == strncmp(*argv, "lo", 3))
            {
                args->desired_output_latency = -1;
            }
            else if (!parse_uint_arg(argc, argv, arg, 1, INT_MAX,
                                     (unsigned int*)&args->desired_output_latency))
            {
                goto error;
            }
        }
        else if (0 == strncmp(arg, "-listen", 8))
        {
            unsigned int tmp;
            --argc;
            ++argv;
            if (!argc)
            {
                printf("-listen requires two arguments\n");
                goto error;
            }
            if (!parse_uint_arg(argc, argv, arg, 1024, 65536, &tmp))
            {
                goto error;
            }
            args->server_port = (uint16_t)tmp;
            --argc;
            ++argv;
            if (!argc)
            {
                printf("-listen requires two arguments\n");
                goto error;
            }
            args->server_service = *argv;
        }
        else if (0 == strncmp(arg, "-sadm", 6))
        {
            args->sadm = 1;
        }
        else
        {
            printf("Error: unrecognised arg: \"%s\"\n", *argv);
            goto error;
        }
        --argc;
        ++argv;
    }

    if (args->mode == MODE_PLAY && args->out_device == NO_DEVICE)
    {
        printf("Error: mode 'play' requires an output device\n");
        goto error;
    }

    if (args->mode == MODE_CAPTURE && args->in_device == NO_DEVICE)
    {
        printf("Error: mode 'capture' requires an input device\n");
        goto error;
    }

    if (args->mode == MODE_PIPE)
    {
        if (args->in_device == NO_DEVICE)
        {
            printf("Error: mode 'pipe' requires an input device\n");
            goto error;
        }
        if (args->out_device == NO_DEVICE)
        {
            printf("Error: mode 'pipe' requires an output device\n");
            goto error;
        }
#ifdef todo
        if (args->in_device == args->out_device)
        {
            printf("Error: testing duplex mode: pass input from device to its own output\n");
            args->mode = MODE_PIPE_DUPLEX;
        }
#endif
    }
    return PMD_SUCCESS;
    
  error:
    printf("usage: <prog> <mode> [<opts>*]\n");
    printf("  where: <mode>:\n");
    printf("       play <filename>    : stream an filename to output device\n");
    printf("       listen             : listen to an input device, but do not record\n");
    printf("       capture <filename> : record from an input device to filename\n");
    printf("                             note: if <filename> is %s, the mode defaults\n", NUL_FILE);
    printf("                             to 'listen'\n");
    printf("       pipe               : stream from an input device to an output device\n");
    printf("       list               : enumerate audio devices\n");
    printf("\n");
    printf("  and: <opts>:\n");
    printf("      -id <devno>         : input device number, 0 - max dev\n");
    printf("      -od <devno>         : output device number, 0 - max dev\n");
    printf("      -mdi <filename>     : input PMD XML metadata file\n");
    printf("      -mdo <filename>     : output PMD XML metadata file\n");
    printf("                            OR HTTP output url\n");
    printf("      -listen <p> <s>     : listen for HTTP POST requests containing XML\n");
    printf("                            PMD metadata, on localhost port <p> and request\n");
    printf("                            <s>.  i.e., HTTP clients will need to POST to URL\n");
    printf("                            of form \"http://<address>/<s>\"\n");
    printf("      -ni                 : device is non-interleaved\n");
    printf("      -cc <n>             : set <n> channels\n");
    printf("      -fs <sample sets>   : non-interleaved chunk size in sample sets\n");

    printf("      -chan <pcm chan number>: channel in PCM to look for/write "
                                           "subframe-mode (single-channel) SMPTE-337m\n");
    printf("      -pair <pcm pair number>: 1st channel of pair in PCM to look for/write "
                                           "frame-mode (two-channel) SMPTE-337m\n");
    printf("      -mark-pcm-blocks: insert SMPTE 337m NULL-frames at PCM block boundaries\n");
    printf("            even when there is not data for that PCM block.\n");
    printf("      -loop               : in play mode, loop input file, don't terminate\n");
    printf("      -sadm               : prefer sADM metadata format in streams\n");
    printf("      -allmd              : dump metadata every frame, even if same as last frame\n");
    printf("      -skip-pcm <count>: simulate random access into PCM stream by skipping this\n");
    printf("            many samples from start of .wav file when decoding PCM+PMD\n");
    printf("      -vsync <offset>: (PCM+PMD read only) samples from beginning of .wav file where\n");
    printf("            first video frame boundary occurs\n");
    
    printf("      -dil hi|lo|<int> : portaudio desired input latency\n");
    printf("                     hi - use portaudio's default high input latency (default)\n");
    printf("                     lo - use portaudio's default low input latench\n");
    printf("                      <int> - specify desired latency in milliseconds\n");
    printf("                          note that this is only a suggestion to portaudio\n");

    printf("      -dol hi|lo|<int> : portaudio desired output latency\n");
    printf("                     hi - use portaudio's default high output latency (default)\n");
    printf("                     lo - use portaudio's default low output latench\n");
    printf("                      <int> - specify desired latency in milliseconds\n");
    printf("                          note that this is only a suggestion to portaudio\n");

    return PMD_FAIL;
}
