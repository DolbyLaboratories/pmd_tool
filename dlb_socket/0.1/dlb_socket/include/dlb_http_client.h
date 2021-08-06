/************************************************************************
 * dlb_socket
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
 * @file dlb_http_client.h
 * @brief API for simple cross-platform HTTP client
 */

#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include "dlb_socket.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) && !defined(inline)
#  define inline __inline
#endif


/**
 * @brief encapsulation of stuff required by HTTP connection
 */
typedef struct
{
    dlb_socket socket;
    char hostname[256];
    char *filename;
    int port;
    int chunked;
}  dlb_http_client;
    
    
/**
 * @brief type of 'got data' callback
 */
typedef
void
(*dlb_http_got_data)
    (void *arg                /**< [in] client-supplied arg */
    ,char *buffer             /**< [in] buffer of received data */
    ,size_t length            /**< [in] size of received data */
    );


/**
 * @brief open an HTTP socket connection
 */
dlb_socket_res                  /** @return success or failure */
dlb_http_client_open
    (dlb_http_client *http      /**< [in] http client */
    ,const char *uri            /**< [in] URI */
    ,int *err                   /**< [out] OS-specific error code */
    );
    

/**
 * @brief shut down an HTTP socket client
 */
void
dlb_http_client_close
    (dlb_http_client *http  /**< [in] http client */
    );
    

/**
 * @brief send an HTTP GET message
 *
 * The response must be read separately via the #dlb_http_client_read
 * function
 */
dlb_socket_res                  /** @return success or failure */
dlb_http_client_get
    (dlb_http_client *http      /**< [in] http client */
    ,const char *content_type   /**< [in] content type string, or NULL for none */
    ,const char *range          /**< [in] byte range in the form "<begin>-<end>",
                                  * or NULL to download entire file pointed to by URL */
    ,int *err                   /**< [out] OS-specific error code */
    );
    

/**
 * @brief open an HTTP POST message
 *
 * open an HTTP post message, and send header. Data to be posted must be
 * sent by the #dlb_http_client_send function.  Once all data is posted,
 * the response must be read by the #dlb_http_client_read.
 */
dlb_socket_res                  /** @return success or failure */
dlb_http_client_post
    (dlb_http_client *http      /**< [in] http client */
    ,const char *content_type   /**< [in] content type string, or NULL for none */
    ,size_t content_length      /**< [in] length of data */
    ,int *err                   /**< [out] OS-specific error code */
    );


/**
 * @brief send data to be posted after an HTTP POST header
 *
 * This function must be invoked after #dlb_http_client_post to send the
 * actual data to be posted.
 */
dlb_socket_res                  /** @return success or failure */
dlb_http_client_send
    (dlb_http_client *http      /**< [in] http client */
    ,char *data                 /**< [in] data to send */
    ,size_t len                 /**< [in] length of data to send */
    ,int *err                   /**< [out] OS-specific error code */
    );
    

/**
 * @brief read response from an HTTP GET or HTTP POST message
 */
dlb_socket_res
dlb_http_client_read
    (dlb_http_client *http      /**< [in] http client */
    ,dlb_http_got_data cb       /**< [in] callback to invoke on each chunk of data read */
    ,void *cbarg                /**< [in] client-supplied callback */
    );
    


#ifdef __cplusplus
}
#endif

#endif /* HTTP_CLIENT_H */
/** }@ */
