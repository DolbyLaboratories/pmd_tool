/************************************************************************
 * dlb_xmllib
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

/** @file xml_trace.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dlb_xml/include/dlb_xml.h"

#define MAXLINE 4096
struct context 
{
    FILE *fp;
    char line[MAXLINE];
    int indent;
};

static 
inline
void 
indent(int indent)
{
    int i;
    for (i = 0; i < indent; i++) 
    {
        printf("  ");
    }
}

/**
 * @brief Loads next line from a file
 *
 * @param p_context: Passed through context pointer, to load a line
 */
static char *line_callback(void *p_context)
{
    if (feof(((struct context *)p_context)->fp)) 
    {
        return NULL;
    }
    
    return fgets(((struct context *)p_context)->line, MAXLINE, ((struct context *)p_context)->fp);
}

/**
 * @brief "Element" callback for XML parser
 *
 * @param p_context: Passed through context pointer
 * @param tag: Tag string (name of the element)
 * @param text: Text enclosed inside the element's open and close tags, or NULL on open tag
 */
static int element_callback(void *p_context, char *tag, char *text)
{
    if (text == NULL) /* Open tag */
    {
        indent(((struct context *)p_context)->indent);
        printf("OPEN('%s')\n", tag);
        
        ((struct context *)p_context)->indent++;
    } 
    else /* Close tag */
    { 
        ((struct context *)p_context)->indent--;
        
        indent(((struct context *)p_context)->indent);
        printf("CLOSE('%s', '%s')\n", tag, text);
    }
    
    return 0;
}

/**
 * @brief "Attribute" callback for XML parser
 *
 * @param p_context: Passed through context pointer
 * @param tag: Tag string (name of the element)
 * @param attribute: Attribute string (name of the attribute)
 * @param value: Text enclosed inside the attribute's quotes
 */
static int attribute_callback(void *p_context, char *tag, char *attribute, char *value)
{
    indent(((struct context *)p_context)->indent);
    printf("ATTR('%s', '%s', '%s')\n", tag, attribute, value);
    
    return 0;
}

/**
 * @brief Simple application to trace callbacks by the XML parser
 */
int main(int argc, const char **argv) 
{
    int status;
    dlb_xml_version version;
    struct context context = {0,};
    
    dlb_xml_query_version(&version);
    printf("\n"
        "Simple XML parser, version %d.%d.%d\n"
        "\n"
        "Unpublished work.  Copyright 2003-2013 Dolby Laboratories, Inc. and\n"
        "Dolby Laboratories Licensing Corporation.  All Rights Reserved.\n"
        "\n"
        "USE OF THIS SOFTWARE IS SUBJECT TO A LEGAL AGREEMENT BETWEEN YOU AND DOLBY\n"
        "LABORATORIES. DO NOT USE THIS SOFTWARE UNLESS YOU AGREE TO THE TERMS AND\n"
        "CONDITIONS IN THE AGREEMENT.  BY USING THIS SOFTWARE, YOU ACKNOWLEDGE THAT\n"
        "YOU HAVE READ THE AGREEMENT AND THAT YOU AGREE TO BE BOUND BY ITS TERMS.\n"
        "\n", 
        version.version_major, 
        version.version_minor, 
        version.version_update);
    
    if (argc < 2) 
    {
        fprintf(stderr, "USAGE: %s <filename.xml>\n", argv[0]);
        return 1;
    }
    
    context.fp = fopen(argv[1], "r");
    if (context.fp == NULL) 
    {
        fprintf(stderr, "Failed to open '%s'!\n", argv[1]);
        return 1;
    }
    
    status = dlb_xml_parse(&context, &line_callback, &element_callback, &attribute_callback);
    printf("XML parsing: %s\n", (status != DLB_XML_SUCCESS) ? "ERROR" : "SUCCESS");
    
    fclose(context.fp);
    
    return (status != DLB_XML_SUCCESS) ? 1 : 0;
}

