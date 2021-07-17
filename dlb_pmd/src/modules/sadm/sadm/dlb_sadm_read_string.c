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


#include "sadm/dlb_sadm_reader.h"
#include "sadm/dlb_sadm_string.h"

#include <string.h>
#include <stdlib.h>

/**
 * @brief helper struct to walk through buffer line-by-line
 */
typedef struct
{
    dlb_sadm_error_callback cberr;   /**< client error callback */
    void *cbarg;                     /**< client's error callback user parameter */
    const char *pos;                 /**< current position in string */
    const char *end;                 /**< end of string */
    unsigned int lineno;             /**< line number */
    char line[4096];
} xml_buf;


/**
 * @brief helper callback to read user-supplied buffer
 */
static
char *                   /** @return start of next line, or NULL */
line_callback
    (void *arg           /**< [in] client argument */
    )
{
    xml_buf *xbuf = (xml_buf *)arg;
    size_t size = xbuf->end - xbuf->pos;
    const char *next;
    
    if (xbuf->pos == xbuf->end)
    {
        return NULL;
    }
    else
    {
        memset(xbuf->line, '\0', sizeof(xbuf->line));
        next = memchr(xbuf->pos, '\n', size);
        if (NULL == next)
        {
            next = memchr(xbuf->pos, '\r', size);
        }
        if (NULL == next)
        {
            next = xbuf->end;
        }
        else 
        {
            if (*next == '\n' || *next == '\r') ++next;
            if (*next == '\n') ++next;
        }

        memcpy(xbuf->line, xbuf->pos, next - xbuf->pos);
        xbuf->pos = next;
        xbuf->lineno += 1;
        return xbuf->line;
    }
}


/**
 * @brief error handler callback
 */
static
void
error_callback
    (const char *msg
    ,void *cbarg
    )
{
    xml_buf *xbuf = (xml_buf *)cbarg;
    if (NULL != xbuf->cberr)
    {
        xbuf->cberr(msg, xbuf->cbarg);
    }
}


dlb_pmd_success
dlb_sadm_string_read
   (const char              *data
   ,size_t                   size
   ,dlb_sadm_model          *model
   ,dlb_sadm_error_callback  err
   ,void                    *arg
   ,unsigned int            *errline
   )
{
    dlb_sadm_reader *reader;
    dlb_sadm_counts limits;
    void *mem;
    xml_buf xbuf;
    size_t sz;
            
    xbuf.cberr  = err;
    xbuf.cbarg  = arg;
    xbuf.pos    = data;
    xbuf.end    = data + size;
    xbuf.lineno = 1;

    if (dlb_sadm_model_limits(model, &limits))
    {
        return PMD_FAIL;
    }
    
    sz = dlb_sadm_reader_query_memory(&limits);
    mem = malloc(sz);
    if (!mem)
    {
        return PMD_FAIL;
    }

    if (dlb_sadm_reader_init(&limits, mem, &reader))
    {
        free(mem);
        return PMD_FAIL;
    }

    if (dlb_sadm_reader_read(reader, line_callback, error_callback, &xbuf, model))
    {
        if (errline)
        {
            *errline = xbuf.lineno;
        }
        return PMD_FAIL;
    }

    dlb_sadm_reader_finish(reader);
    free(mem);
    return PMD_SUCCESS;
}

