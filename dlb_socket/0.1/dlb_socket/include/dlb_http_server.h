/************************************************************************
 * dlb_socket
 * Copyright (c) 2012-2019, Dolby International AB.
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
 * @file dlb_http_server.h
 * @brief API for a very basic simple cross-platform HTTP server
 */

#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "dlb_socket.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) && !defined(inline)
#  define inline __inline
#endif

#define LINESIZE (8192)
#define METHOD_SIZE (16)
#define URI_SIZE (256)
#define CT_SIZE (128)

/**
 * @brief encapsulation of HTTP request processing
 */
typedef struct
{
    dlb_socket socket;           /**< current request socket */

    char buf[LINESIZE];          /**< input buffer */
    char method[METHOD_SIZE];    /**< method, e.g., GET or POST */
    char uri[URI_SIZE];          /**< requested URI */
    char content_type[CT_SIZE];  /**< incoming/outgoing Content-Type field
                                  * processor callback should modify this */
    int chunked_transfer;        /**< 1 if chunked transfer, 0 if not */
    unsigned int chunk_left;     /**< bytes in chunk left to read */
    
    ssize_t buflen;              /**< length of actual data in buffer */
    char *body;                  /**< location of beginning of body in buf */

    char *return_code;           /**< HTTP return code, e.g., "200 OK" */
    char *response_body;         /**< response body text */
    size_t response_body_size;   /**< size of response body */
} dlb_http_request;


/**
 * @brief type of client code invoked whenever a valid HTTP header has been parsed
 *
 * The callback is expected to read in rest of the message and process
 * it accordingly, setting the #code, #content_type, #data and
 * #datasize fields of the #reply argument correctly.
 *
 * @note if the client callback returns 0, the server code will automatically
 * generate a "404 Not Found" message
 *
 * @note that the callback is expected to use the #dlb_http_request_read function
 * to read more data off the incoming socket
 */
typedef
int                             /** @return 1 on success, 0 on failure */
(*dlb_http_server_guts)
    (void *cbarg                /**< [in] client-supplied callback */
    ,dlb_http_request *request  /**< [in/out] request to parse and process */
    );
    

/**
 * @brief type of basic http server 'object'
 */
typedef struct
{
    dlb_socket listener_socket;   /**< server's main listener socket */
    uint16_t port;                /**< listener's port */
    volatile int running;         /**< flag to set to 0 to stop */
} dlb_http_server;
    

/**
 * @brief read more data off the HTTP connection
 */
dlb_socket_res                    /** @return DLB_SOCKET_OK on success, otherwise failure */
dlb_http_request_read
    (dlb_http_request *request    /**< [in] http request to read */
    ,char *buffer                 /**< [in] buffer to fill */
    ,size_t capacity              /**< [in] capacity of buffer */
    ,ssize_t *read                /**< [out] bytes read */
    ,int *err                     /**< [out] os-specific error code */
    );
    

/**
 * @brief run the simple http server
 *
 * @note this runs until #dlb_http_server_stop is invoked from
 * another thread.  I.e., it is expected that this function form
 * the executive part of a thread or process.
 */
dlb_socket_res                   /** @return return status */
dlb_http_server_run
    (dlb_http_server *server     /**< [in] server struct managed by this function */
    ,uint16_t port               /**< [in] local host port number */
    ,dlb_http_server_guts cb     /**< [in] client code to process given request */
    ,void *cbarg                 /**< [in] client argument to callback */
    ,int *err                    /**< [out] OS-specific error code in case the
                                   * server doesn't run */
    );


/**
 * @brief asynchronous stop function
 */
void
dlb_http_server_stop
    (dlb_http_server *server    /**< [in] server to stop */
    );
    


#ifdef __cplusplus
}
#endif


#endif /* HTTP_SERVER_H */
