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

#include "dlb_http_client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

#ifdef _MSC_VER
#  define snprintf _snprintf
#endif

/* uncomment the following for debug tracing */
//#define DBG_TRACE
#ifdef DBG_TRACE
#  define TRACE(x) printf x
#  define TRACE_DATA(x,l)
#else
#  define TRACE(x)
#  define TRACE_DATA(x,l)
#endif


dlb_socket_res
dlb_http_client_open
    (dlb_http_client *client
    ,const char *uri
    ,int *err
    )
{
    dlb_socket_res res;
    char *c;
    char *port = "80";  /* default HTTP port */
    int p;
    
    memset(client->hostname, '\0', sizeof(client->hostname));
    if (0 != strncmp("http://", uri, 7))
    {
        return 0;
    }
    strncpy(client->hostname, uri+7, sizeof(client->hostname));
    client->filename = strchr(client->hostname, '/');
    if (client->filename)
    {
        *client->filename = '\0';
        client->filename++;
    }

    /* extract port */
    c = strchr(client->hostname, ':');
    if (NULL != c)
    {
        *c = '\0';
        port = c+1;
    }

    p = atoi(port);
    res = dlb_socket_connect(&client->socket, client->hostname, p, NULL, err);
    TRACE(("HTTP CLIENT: connect status res:%d err:%d\n", res, *err));
    return res;
}


void
dlb_http_client_close
    (dlb_http_client *client
    )
{
    if (client != NULL)
    {
        dlb_socket_close(client->socket);
    }
}


dlb_socket_res
dlb_http_client_get
    (dlb_http_client *client
    ,const char *content_type
    ,const char *range
    ,int *err
    )
{
    char sbuffer[1024];
    size_t len;

    /* generate request */
    len = snprintf(sbuffer, sizeof(sbuffer),
                   "GET /%s HTTP/1.1\r\n"
                   "Host: %s\r\n"
                   "User-Agent: dlb_http_client\r\n"
                   "%s%s%s"
                   "%s%s%s"
                   "Client: keep-alive\r\n\r\n",
                   client->filename, 
                   client->hostname,
                   NULL == range ? "" : "Range: ",
                   NULL == range ? "" : range,
                   NULL == range ? "" : "\r\n",
                   NULL == content_type ? "" : "Content-Type: ",
                   NULL == content_type ? "" : content_type,
                   NULL == content_type ? "" : "\r\n"
                   );
    
    TRACE(("HTTP-CLIENT: sending request:\n%s\n", sbuffer));

    /* send request */
    return dlb_socket_stream_write(client->socket, sbuffer, len, err);
}


dlb_socket_res
dlb_http_client_post
    (dlb_http_client *client
    ,const char *content_type
    ,size_t content_length
    ,int *err
    )
{
    char sbuffer[1024];
    char clen[128];
    size_t len;

    if (content_length)
    {
        snprintf(clen, sizeof(clen), "Content-Length: %u", (unsigned int)content_length);
        client->chunked = 0;
    }
    else
    {
        snprintf(clen, sizeof(clen), "Transfer-Encoding: chunked");
        client->chunked = 1;
    }
    
    
    /* generate request */
    len = snprintf(sbuffer, sizeof(sbuffer),
                   "POST /%s HTTP/1.1\r\n"
                   "Host: %s\r\n"
                   "User-Agent: dlb_http_client\r\n"
                   "%s%s%s"
                   "%s\r\n"
                   "\r\n",
                   client->filename,
                   client->hostname,
                   NULL == content_type ? "" : "Content-Type: ",
                   NULL == content_type ? "" : content_type,
                   NULL == content_type ? "" : "\r\n",
                   clen
                   );
    
    TRACE(("HTTP-CLIENT: sending request:\n%s\n", sbuffer));

    /* send request */
    return dlb_socket_stream_write(client->socket, sbuffer, len, err);
}


dlb_socket_res
dlb_http_client_send
    (dlb_http_client *http
    ,char *data
    ,size_t len
    ,int *err
    )
{
    if (http->chunked)
    {
        char hdr[128];
        ssize_t hdrlen = snprintf(hdr, sizeof(hdr), "%x\r\n", (unsigned int)len);
        TRACE(("HTTP CLIENT: posting \n%s\n", hdr));
        TRACE_DATA(data, len);
        TRACE(("\r\n  --------------------------------\n"));

        return dlb_socket_stream_write(http->socket, hdr, hdrlen, err)
            || dlb_socket_stream_write(http->socket, data, len, err)
            || dlb_socket_stream_write(http->socket, "\r\n", 2, err);
    }
    else
    {
        TRACE(("HTTP CLIENT: posting \n%s\n --------------------------------\n", data));
        return dlb_socket_stream_write(http->socket, data, len, err);        
    }
}
    


dlb_socket_res
dlb_http_client_read
    (dlb_http_client *client
    ,dlb_http_got_data cb
    ,void *cbarg
    )
{
    dlb_socket_res res;
    char buffer[8196];
    char *datastart;
    char *ok;
    int datalength;
    int datagot;
    int err;
    ssize_t sz;

    if (client->chunked)
    {
        /* send terminating, 0-length chunk */
        res = dlb_socket_stream_write(client->socket, "0\r\n\r\n", 5, &err);
        if (DLB_SOCKET_OK != res)
        {
            printf("could not send terminating chunk: %d\n", err);
            return res;
        }
    }

    /* now receive response */
    memset(buffer, '\0', sizeof(buffer));
    res = dlb_socket_read(client->socket, buffer, sizeof(buffer), &sz,
                          DLB_SOCKET_BLOCKING, &err);
    if (DLB_SOCKET_OK != res)
    {
        printf("dlb_http_client_read error: %d\n", err);
        return res;
    }
    
    TRACE(("HTTP CLIENT: received: \n%s\n --------------------------------\n", buffer));
    /* check to make sure we've received the full header */
    datastart = strstr(buffer, "\r\n\r\n");
    if (datastart == NULL)
    {
        printf("ERROR: did not receive HTTP header\n");
        return DLB_SOCKET_LIBRARY_ERR;
    }

    ok = strstr(buffer, "HTTP/1.1 200 OK");
    if (ok == NULL || ok > datastart)
    {
        printf("HTTP-CLIENT: ERROR: did not receive HTTP status 200 OK:\n\n");
        printf("%s", buffer);
        printf("\n\n");
        return DLB_SOCKET_LIBRARY_ERR;
    }
    
    ok = strstr(buffer, "Content-Length:");
    if (ok == NULL || ok > datastart)
    {
        printf("HTTP-CLIENT: ERROR: did not receive Content-Length field\n");
        return DLB_SOCKET_LIBRARY_ERR;
    }
    datalength = atoi(ok + 15);

    /* skip \r\n\r\n terminator */
    datastart += 4;
    datagot = (buffer + sz) - datastart;

    if (cb)
    {
        cb(cbarg, datastart, datagot);
    
        /* download remaining, if any */
        while (datagot < datalength)
        {
            res = dlb_socket_read(client->socket, buffer, sizeof(buffer), &sz,
                                  DLB_SOCKET_BLOCKING, &err);
            if (DLB_SOCKET_OK != res)
            {
                return res;
            }
            cb(cbarg, buffer, sz);
            datagot += sz;
        }
    }

    return DLB_SOCKET_OK;
}
