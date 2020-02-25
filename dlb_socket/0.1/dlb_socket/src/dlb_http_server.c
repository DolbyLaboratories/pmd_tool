/************************************************************************
 * dlb_socket
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

#include "dlb_http_server.h"
#include <string.h>
#include <stdio.h>

/**
 * @file dlb_http_server.c
 * @brief a very basic simple cross-platform HTTP server
 */

#if defined(_MSC_VER) && !defined(snprintf)
#  define snprintf _snprintf
#endif

/* uncomment for debug tracing */
//#define DBG_TRACE
#ifdef DBG_TRACE
#  define TRACE(x) printf x
#else
#  define TRACE(x)
#endif


/**
 * @def MIN(x,y)
 * @brief simply macro to determine minimum of two comparable values
 */
#define MIN(x,y) ((x) < (y) ? (x) : (y))


/**
 * @brief parse HTTP request header
 */
static
int                               /** @return 1 on success, 0 on failure */
parse_http_request_header
     (dlb_http_request *request
     )
{
    dlb_socket_res res;
    char *ct;
    int err;
    
    res = dlb_socket_read(request->socket, request->buf, sizeof(request->buf),
                          &request->buflen, DLB_SOCKET_BLOCKING, &err);
    if (DLB_SOCKET_OK == res)
    {
        TRACE(("HTTP-SERVER: received request:\n%s\n\n", request->buf));

        /* extract method and uri from request */
        request->body = strstr(request->buf, "\r\n\r\n");
        if (NULL == request->body)
        {
            TRACE(("HTTP-SERVER: end-of-header not found\n"));
            request->return_code = "400 Bad Request";
            return 0;
        }
        /* skip the end of header */
        request->body += 4;
    
        sscanf(request->buf, "%s %s", request->method, request->uri);
        TRACE(("HTTP-SERVER: method: %s\n", request->method));
        TRACE(("HTTP-SERVER: uri: %s\n", request->uri));

        request->content_type[0] = '\0';
        request->chunked_transfer = 0;

        ct = strstr(request->buf, "Content-Type:");
        if (ct)
        {
            char *end;

            ct += 13;                                /* skip "Content-Type:" text */
            while (*ct == ' ' || *ct == '\t') ++ct;  /* skip optional whitespace */
            end = strstr(ct, "\r\n");
            while (end > ct && (end[-1] == ' ' || end[-1] == '\t')) --end; /* skip trailing ws */
            memset(request->content_type, '\0', sizeof(request->content_type));
            strncpy(request->content_type, ct,
                    MIN(sizeof(request->content_type), (size_t)(end - ct)));
            TRACE(("HTTP-SERVER: content type: \"%s\"\n", request->content_type));
        }

        ct = strstr(request->buf, "Transfer-Encoding:");
        if (ct)
        {
            char *end;

            ct += 18; /* skip 'Transfer-Encoding:' text */
            while (*ct == ' ' || *ct == '\t') ++ct;  /* skip optional whitespace */
            end = strstr(ct, "\r\n");
            while (end > ct && (end[-1] == ' ' || end[-1] == '\t')) --end; /* skip trailing ws */
            if (0 == strcmp(ct, "chunked"))
            {
                TRACE(("HTTP-SERVER: transfer-encoding chunked\n"));
                request->chunked_transfer = 1;
                request->chunk_left = 0;
            }
        }
        
        if (request->chunked_transfer && request->body)
        {
            if (1 != sscanf(request->body, "%x\r\n", &request->chunk_left))
            {
                TRACE(("HTTP-SERVER: could not find 1st chunk length\n"));
                return 0;
            }
            ct = strstr(request->body, "\r\n");
            request->body = ct + 2;
            /* TODO: handle case where multiple chunks can live in the same
             * socket read buffer as the http request.
             */
            request->chunk_left -= (&request->buf[request->buflen] - request->body);
        }
        return 1;
    }
    return 0;
}


dlb_socket_res
dlb_http_request_read
    (dlb_http_request *request
    ,char *buffer
    ,size_t capacity
    ,ssize_t *read
    ,int *err
    )
{
    dlb_socket_res res;
    ssize_t sz;

    res = dlb_socket_read(request->socket, buffer, capacity, &sz, DLB_SOCKET_BLOCKING, err);
    if (DLB_SOCKET_OK != res)
    {
        return res;
    }
    
    if (request->chunked_transfer)
    {
        size_t remaining = sz;
        char *ct = buffer;
        while (request->chunk_left < sz && request->chunk_left)
        {
            /* try and read next chunk */
            char *next;
            size_t hdrsize;

            ct += request->chunk_left;
            if (1 != sscanf(ct, "%x\r\n", &request->chunk_left))
            {
                /* bad chunk */
                return DLB_SOCKET_LIBRARY_ERR;
            }
            next = strstr(ct, "\r\n") + 2;
            hdrsize = next - ct;
            remaining = buffer + sz - next;
            memmove(ct, next, remaining);
            sz -= hdrsize;
        }
        request->chunk_left -= remaining;
        *read = sz;
    }
    else
    {
        *read = sz;
    }
    return DLB_SOCKET_OK;
}



/**
 * @brief package up the client's response and compose an HTTP request message
 */
static
void
send_response
    (dlb_http_request *request
    ,int *err
    )
{
    dlb_socket_res res;
    char response[1024];
    char content_type_str[128];
    ssize_t len;

    content_type_str[0] = '\0';
    if (request->content_type[0])
    {
        snprintf(content_type_str, sizeof(content_type_str),
                 "Content-Type: %s\r\n", request->content_type);
    }
    
    len = snprintf(response, sizeof(response),
                   "HTTP/1.1 %s\r\n"
                   "Server: dlb_http_server\r\n"
                   "Content-Length: %u\r\n"
                   "%s\r\n",
                   request->return_code,
                   request->response_body_size,
                   content_type_str);

    TRACE(("HTTP-SERVER: sending response:\n%s", response));

    res = dlb_socket_stream_write(request->socket, response, len, err);
    if (DLB_SOCKET_OK != res)
    {
        printf("HTTP-SERVER: failed to send response header: %d\n", *err);
        return;
    }
    if (request->response_body_size)
    {
        TRACE(("%s", request->response_body));

        res = dlb_socket_stream_write(request->socket, request->response_body,
                                      request->response_body_size, err);
        if (DLB_SOCKET_OK != res)
        {
            printf("HTTP-SERVER: failed to send response data: %d\n", *err);
            return;
        }
    }
    TRACE(("\n\n"));
}


dlb_socket_res
dlb_http_server_run
    (dlb_http_server *server
    ,uint16_t port
    ,dlb_http_server_guts cb
    ,void *cbarg
    ,int *err
    )
{
    dlb_http_request request;
    dlb_socket_res res;
    char client[DLB_SOCKET_MAX_ADDRESS_SIZE];

    if (!cb)
    {
        return DLB_SOCKET_INVALID_ARG;
    }

    res = dlb_socket_create_stream_server(&server->listener_socket, NULL, port, err);
    if (DLB_SOCKET_OK != res)
    {
        return res;
    }
    
    server->running = 1;
    server->port = port;
    while (server->running)
    {
        TRACE(("HTTP-SERVER: waiting for connection....\n"));
        res = dlb_socket_accept(server->listener_socket, &request.socket,
                                DLB_SOCKET_BLOCKING, &client, err);
        if (DLB_SOCKET_OK == res)
        {
            TRACE(("HTTP-SERVER: received connection (%d)\n", server->running));
            if (server->running)
            {
                TRACE(("HTTP-SERVER: accepted connection from client %s\n", client));
                if (parse_http_request_header(&request))
                {
                    if (!cb(cbarg, &request))
                    {
                        request.return_code = "404 Not Found";
                        request.content_type[0] = '\0';
                    }
                    else
                    {
                        printf("received model via HTTP\n");
                    }
                }
                TRACE(("HTTP-SERVER: return \"%s\"\n", request.return_code));
                send_response(&request, err);
            }
            dlb_socket_close(request.socket);
        }
        else
        {
            printf("HTTP-SERVER: socket accept error %d\n", *err);
        }
    }
    TRACE(("HTTP-SERVER: ended\n"));
    dlb_socket_close(server->listener_socket);
    return DLB_SOCKET_OK;
}


void
dlb_http_server_stop
    (dlb_http_server *server
    )
{
    dlb_socket closer;
    dlb_socket_res res;
    int err;
    
    printf("signaling server to stop....\n");
    server->running = 0;

    /* now kick the server awake to make it take notice of the shutdown */
    res = dlb_socket_connect(&closer, "localhost", server->port, NULL, &err);
    if (DLB_SOCKET_OK != res)
    {
        printf("ERROR: failed to send shutdown signal to server: %d\n", err);
    }
    else
    {
        printf("signaled\n");
    }
    dlb_socket_close(closer);
}
