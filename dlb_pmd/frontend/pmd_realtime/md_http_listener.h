/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019-2020 by Dolby Laboratories,
 *                Copyright (C) 2019-2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

/**
 * @file md_http_listener.h
 * @brief abstraction of code to listen to incoming metadata via HTTP on a socket
 */

#include "dlb_http_server.h"
#include "pmd_os.h"
#include "dlb_pmd_xml.h"
#include "model_queue.h"
#include <stdarg.h>

#if defined(_MSC_VER) && !defined(snprintf)
#  define snprintf _snprintf
#endif

/* uncomment following for debug tracing */
//#define MHL_DEBUGGING
#ifdef MHL_DEBUGGING
#  define MHL_TRACE(x) printf x
#else 
#  define MHL_TRACE(x)
#endif

/* uncomment following to dump received XML */
//#define MHL_DUMP

#ifdef MHL_DUMP

typedef struct
{
    unsigned int count;
    FILE *f;
} mhl_debug_dumper;

#define MHL_DEBUG_DUMPER mhl_debug_dumper dumper;


/**
 * @brief open file for dumping, if #MHL_DUMP defined
 */
static inline
void
MHL_DUMP_OPEN
    (mhl_debug_dumper *dumper
    ,dlb_http_request *request
    )
{
    char name[64];
    
    snprintf(name, sizeof(name), "got_%u.xml", dumper->count++);
    dumper->f = fopen(name, "wb");
    fwrite(request->body, 1, request->buflen, dumper->f);
    printf("READING INCOMING HTTP %u.......\n", dumper->count-1);
}


static inline
void
MHL_DUMP_WRITE
    (mhl_debug_dumper *dumper
    ,char *data
    ,ssize_t sz
    )
{
    fwrite(data, 1, sz, dumper->f);
}


static inline
void
MHL_DUMP_CLOSE
    (mhl_debug_dumper *dumper
    )
{
    fclose(dumper->f);
}

#else
#  define MHL_DEBUG_DUMPER
#  define MHL_DUMP_OPEN(dumper, request)
#  define MHL_DUMP_WRITE(dumper, data, sz)
#  define MHL_DUMP_CLOSE(dumper)
#endif


/**
 * @brief type of http listener abstraction
 */
typedef struct
{
    const char *servicename;          /**< name of service */
    uint16_t port;                    /**< requested localhost port to listen to */
    model_queue *pending;             /**< model queue to populate with incoming XML */
    dlb_http_request *request;        /**< current request */
    dlb_http_server http_server;      /**< http server abstraction */
    pmd_thread thread;                /**< thread handle for server */

    char readbuf[4096];               /**< current input buffer */
    char *pos;                        /**< current position in string */
    char *end;                        /**< end of string */
    unsigned int lineno;              /**< current XML line number */

    char error[4096];                 /**< error message */
    char *ewp;                        /**< current error write pointer */
    MHL_DEBUG_DUMPER                  /**< debug dumper (if enabled) */

} md_http_listener;


/**
 * @brief helper callback to read another line from file
 */
static
char *                   /** @return start of next line, or NULL */
mhl_line_callback
    (void *arg           /**< [in] client argument */
    )
{
    md_http_listener *mhl = (md_http_listener *)arg;
    char *line = mhl->pos;
    char *next = NULL;
    ssize_t sz;
    int err;
    size_t size;

    if (mhl->pos < mhl->end)
    {
        size = mhl->end - mhl->pos;
        next = memchr(mhl->pos, '\n', size);
        if (NULL == next)
        {
            next = memchr(mhl->pos, '\r', size);
        }
    }

    if (NULL == next)
    {
        /* try to read more from socket.  Move fragment of last line to beginning
         * of buffer
         */
        char *wpos;
        size_t capacity;
        size_t keeplen = mhl->end - mhl->pos;
        memmove(mhl->readbuf, mhl->pos, keeplen);

        wpos = mhl->readbuf + keeplen;
        capacity = sizeof(mhl->readbuf) - keeplen;
        if (dlb_http_request_read(mhl->request, wpos, capacity, &sz, &err))
        {
            /* no more data, try sending last bit, if any */
            if (!keeplen)
            {
                return NULL;
            }
            mhl->lineno += 1;
            return mhl->pos;
        }
        else
        {
            MHL_DUMP_WRITE(&mhl->dumper, wpos, sz);
        }
        
        mhl->pos = mhl->readbuf;
        mhl->end = wpos + sz;
        line = mhl->pos;

        size = mhl->end - mhl->pos;
        next = memchr(mhl->pos, '\n', size);        
        if (NULL == next)
        {
            next = memchr(mhl->pos, '\r', size);
        }
        if (NULL == next)
        {
            /* try sending everything in one go */
            if (keeplen + sz == 0)
            {
                return NULL;
            }
            mhl->lineno += 1;
            return mhl->pos;
        }
    }

    if (*next == '\n' || *next == '\r') ++next;
    if (*next == '\n') ++next;

    next[-1] = '\0';
    mhl->pos = next;
    mhl->lineno += 1;

    MHL_TRACE(("------------ parse \"%s\"....\n", line));
    return line;
}


/**
 * @brief error handler callback
 */
static
void
mhl_error_callback
    (const char *msg
    ,void *cbarg
    )
{
    md_http_listener *mhl = (md_http_listener *)cbarg;
    char *end = &mhl->error[sizeof(mhl->error)];
    if (mhl->ewp < end)
    {
        size_t space = end - mhl->ewp;
        mhl->ewp += snprintf(mhl->ewp, space, "%s\n", msg);
    }
}


static inline
void
mhl_request_error
    (md_http_listener *mhl
    ,dlb_http_request *request
    ,char *return_code
    ,const char *fmt
    ,...
    )
{
    va_list va_args;
    int len;

    request->return_code = return_code;
    snprintf(request->content_type, sizeof(request->content_type), "text/plain");
    va_start(va_args, fmt);
    
    len = vsnprintf(mhl->error, sizeof(mhl->error), fmt, va_args);
    request->response_body = mhl->error;
    request->response_body_size = len;
    va_end(va_args);
}

/**
 * @brief HTTP server request processor
 */
static
int                                   /** @return 0 if server generates error, 1 otherwise */
process_http_request
    (void *arg                        /**< [in] client arg, md_http_listener */
    ,dlb_http_request *request        /**< [in.out] HTTP request to process */
    )
{
    md_http_listener *mhl = (md_http_listener *)arg;
    dlb_pmd_model *model;
    char *svcname;

    if (strcmp("POST", request->method))
    {
        mhl_request_error(mhl, request, 
                          "405 Method Not Allowed",
                          "Unrecognized method: \"%s\", expected \"POST\"\n",
                          request->method);
        return 1;
    }
    if (strcmp("application/xml; charset=utf-8", request->content_type))
    {
        /* only accept incoming XML */
        mhl_request_error(mhl, request,
                          "415 Unsupported Media Type",
                          "Unrecognized content type: \"%s\", expected \"application/xml\"\n",
                          request->content_type);
        return 1;
    }

    svcname = request->uri;
    if (svcname[0] == '/') ++svcname;
    if (strcmp(mhl->servicename, svcname))
    {
        /* only process accepted service */
        mhl_request_error(mhl, request,
                          "404 Not Found",
                          "Unrecognized service: \"%s\", expected \"%s%s\"\n",
                          request->uri,
                          mhl->servicename[0] == '/' ? "" : "/",
                          mhl->servicename);
        return 1;
    }

    /* parse rest of header buffer, and incrementally download the rest as
     * needed
     */
    mhl->request = request;
    mhl->lineno = 1;
    mhl->pos = request->body;
    mhl->end = request->buf + request->buflen;
    
    MHL_DUMP_OPEN(&mhl->dumper, request);

    model = model_queue_back(mhl->pending);
    if (model)
    {
        dlb_pmd_reset(model);
        if (dlb_xmlpmd_parse(mhl_line_callback, mhl_error_callback, mhl, model,
                             !DLB_PMD_XML_STRICT))
        {
            request->return_code = "400 Bad Request";
            /* error is stored in mhl->readbuf */
            snprintf(request->content_type, sizeof(request->content_type), "text/plain");
            request->response_body = mhl->error;
            request->response_body_size = strlen(mhl->error);
        }
        else
        {
            /* todo: check to see if there is more data */
            request->return_code = "200 OK";
            model_queue_push(mhl->pending);
        }
    }
    
    MHL_DUMP_CLOSE(&mhl->dumper);
    return 1;
}


/**
 * @brief listener thread
 */
static
void *
listener_thread
    (void *arg
    )
{
    md_http_listener *mhl = (md_http_listener *)arg;
    int err;

    (void)dlb_http_server_run(&mhl->http_server, mhl->port, process_http_request, mhl, &err);
    return NULL;
}


/**
 * @brief set up HTTP listener
 *
 * @note this only listens for insecure http:// POST messages
 */
static inline
dlb_pmd_success
md_http_listener_init
    (Args *args                    /**< [in] command-line arguments */
    ,model_queue *pending          /**< [in] model queue to populate */
    ,md_http_listener **mhlptr     /**< [out] metadata listener */
    )
{
    md_http_listener *mhl = (md_http_listener*)malloc(sizeof(md_http_listener));
    *mhlptr = mhl;
    if (args->server_port)
    {
        mhl->servicename = args->server_service;
        mhl->port = (uint16_t)args->server_port;
        mhl->pending = pending;
        mhl->ewp = mhl->error;
        mhl->error[0] = '\0';
        
        if (   pmd_thread_init(&mhl->thread, listener_thread, mhl, PMD_THREAD_PRIORITY_NORMAL, 0, 0)
            || pmd_thread_start(&mhl->thread))
        {
            printf("Failed to create HTTP listener thread\n");
            return PMD_FAIL;
        }
    }
    return PMD_SUCCESS;
}


/**
 * @brief shut down HTTP client
 */
static inline
void
md_http_listener_finish
    (md_http_listener *mhl       /**< [in] metadata listener to finish */
    )
{
    if (mhl->port)
    {
        void *ignore;
        dlb_http_server_stop(&mhl->http_server);
        (void)pmd_thread_join(&mhl->thread, &ignore);
        (void)ignore;
    }
    free(mhl);
}


