/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2012-2019 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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
