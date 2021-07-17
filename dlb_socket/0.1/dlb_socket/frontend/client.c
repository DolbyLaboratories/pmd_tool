/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019 by Dolby Laboratories,
 *                Copyright (C) 2019 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/
/**
 * @file client.c
 * @brief simple command-line tool to test http client library
 */

#include "dlb_http_client.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef enum
{
    REQ_NONE,
    REQ_GET,
    REQ_POST
} req;


typedef struct
{
    const char *infile;
    const char *outfile;
    const char *url;
    int chunked;
    req r;
    
} Args;


static
int
parse_args
    (Args *args
    ,int argc
    ,const char **argv
    )
{
    const char *progname;
    
    args->infile = NULL;
    args->outfile = NULL;
    args->url = NULL;
    args->chunked = 0;
    args->r = REQ_NONE;
    
    progname = *argv;
    --argc;
    ++argv;

    if (argc)
    {
        if (0 == strncmp(*argv, "get", 4))
        {
            args->r = REQ_GET;
        }
        else if (0 == strncmp(*argv, "post", 5))
        {
            args->r = REQ_POST;
        }
        else
        {
            printf("Error: test mode neither get nor post\n");
            goto usage;
        }
        
        --argc;
        ++argv;
        if (!argc)
        {
            printf("Error: no URL specified\n");
            goto usage;
        }
        args->url = *argv;
        --argc;
        ++argv;

        while (argc)
        {
            if (0 == strncmp(*argv, "-i", 3))
            {
                --argc;
                ++argv;
                if (!argc)
                {
                    printf("Error: no input file specified\n");
                    goto usage;
                }
                args->infile = *argv;
            }
            else if (0 == strncmp(*argv, "-o", 3))
            {
                --argc;
                ++argv;
                if (!argc)
                {
                    printf("Error: no output file specified\n");
                    goto usage;
                }
                args->outfile = *argv;
            }
            else if (0 == strncmp(*argv, "-chunked", 9))
            {
                args->chunked = 1;
            }
            else
            {
                printf("Error: unrecognized option \"%s\"\n", *argv);
                goto usage;
            }
            
            --argc;
            ++argv;
        }
    }

    if (args->infile == NULL && args->r == REQ_POST)
    {
        printf("Error: attempting a POST with no input\n");
        goto usage;
    }
    if (args->outfile == NULL)
    {
        printf("Error: nowhere to store output\n");
        goto usage;
    }
    return 0;

  usage:
    printf("usage: %s <reqtype> <url> <options*>\n", progname);
    printf("    where\n");
    printf("       <reqtype> can be:\n");
    printf("           get - initiate HTTP GET request\n");
    printf("           post - initiate HTTP POST request\n");
    printf("       <url> is a standard http request of form:\n");
    printf("           http://<address>[:<port>?]/<filename>\n");
    printf("       <option> can be:\n");
    printf("           -i <input file> : file to post\n");
    printf("           -o <output file>: file to dump response\n");
    return 1;
}

     
static
void
got_data
    (void *arg
    ,char *buffer
    ,size_t length
    )
{
    fwrite(buffer, 1, length, (FILE*)arg);
}


int
main
    (int argc
    ,const char **argv
    )
{
    dlb_http_client client;
    size_t content_length;
    Args args;
    FILE *f;
    int err;

    if (parse_args(&args, argc, argv))
    {
        return -1;
    }

    content_length = 0;
    if (!args.chunked)
    {
        f = fopen(args.infile, "rb");
        fseek(f, 0, SEEK_END);
        content_length = ftell(f);
        fclose(f);
    }

    switch (args.r)
    {
        case REQ_GET:
            if (dlb_http_client_open(&client, args.url, &err))
            {
                printf("failed to open URL \"%s\": %d\n", args.url, err);
            }
            else if (dlb_http_client_get(&client, NULL, NULL, &err))
            {
                printf("failed to send GET to \"%s\": %d\n", args.url, err);
                dlb_http_client_close(&client);
            }
            else 
            {
                f = fopen(args.outfile, "wb");
                if (NULL != f)
                {
                    if (dlb_http_client_read(&client, got_data, f))
                    {
                        printf("failed reading response\n");
                    }
                    fclose(f);
                }
            }
            break;
        case REQ_POST:
            if (dlb_http_client_open(&client, args.url, &err))
            {
                printf("failed to open URL \"%s\": %d\n", args.url, err);
            }
            else if (dlb_http_client_post(&client, NULL, content_length, &err))
            {
                printf("failed to send POST req to \"%s\": %d\n", args.url, err);
                dlb_http_client_close(&client);
            }
            else 
            {
                /* send rest of post */
                FILE *f = fopen(args.infile, "rb");
                char buffer[4096];
                ssize_t len;

                while (!feof(f))
                {
                    len = fread(buffer, 1, sizeof(buffer), f);
                    if (dlb_http_client_send(&client, buffer, len, &err))
                    {
                        printf("failed sending data: %d\n", err);
                        break;
                    }
                }
                fclose(f);

                f = fopen(args.outfile, "wb");
                if (NULL != f)
                {
                    if (dlb_http_client_read(&client, got_data, f))
                    {
                        printf("failed reading response\n");
                    }
                    fclose(f);
                }
            }
            break;
        default:
            abort();
    }
    

    return 0;
}
