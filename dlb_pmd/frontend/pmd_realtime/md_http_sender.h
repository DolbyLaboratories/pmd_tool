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
 * @file md_http_send.h
 * @brief abstraction of code to send metadata to a destination HTTP address via HTTP POST
 */

#include "dlb_http_client.h"

/* uncomment the following to dump the XML before prior to sending */
//#define MHS_DUMP_FILES


/**
 * @brief encapsulate everything to do with sending model XML via HTTP 
 */
typedef struct
{
    dlb_http_client http_client;
    const char *url;
    dlb_pmd_model *model;
    char line[4096];
    int length;
    int indent;
} md_http_sender;

    
/**
 * @brief for debugging: dump model XML if #MHS_DUMP_FILES is defined
 */
static inline
void
MHS_DUMP
    (dlb_pmd_model *model
    )
{
#ifdef MHS_DUMP_FILES    
    static unsigned int count = 0;
    char name[64];
    snprintf(name, sizeof(name), "send_%u.xml", count++);
    dlb_xmlpmd_file_write(name, model);
#else
    (void)model;
#endif
}


/**
 * @brief set up socket-based metadata output
 *
 * @note this only happens unsecure http:// based output, and IPv4
 * addresses, no DNS names
 */
static inline
dlb_pmd_success
md_http_sender_init
    (md_http_sender *mhs       /**< [in] metadata sender to initialize */
    ,Args *args                /**< [in] command-line arguments */
    )
{
    mhs->url = args->md_file_out;
    return PMD_SUCCESS;
}


/**
 * @brief shut down HTTP client
 */
static inline
void
md_http_sender_finish
    (md_http_sender *mhs       /**< [in] metadata sender to finish */
    )
{
    printf("closing HTTP socket\n");
    dlb_http_client_close(&mhs->http_client);
}


/**
 * @brief callback for the xml writer
 */
static
int
get_buffer
    (void *arg
    ,char *pos
    ,char **buf
    ,size_t *capacity
    )
{
    md_http_sender *mhs = (md_http_sender *)arg;

    if (NULL != pos)
    {
        ptrdiff_t len = pos - mhs->line;
        int err;

        if (len < 0 || len > (ptrdiff_t)sizeof(mhs->line))
        {
            printf("Bad write-pointer returned by xml writer to get_buffer\n");
            return 0;
        }

        if (DLB_SOCKET_OK != dlb_http_client_send(&mhs->http_client, mhs->line, len, &err))
        {
            printf("Failed to send data down socket: %d\n", err);
            return 0;
        }
    }
    
    if (NULL != buf)
    {
        *buf = mhs->line;
        *capacity = sizeof(mhs->line);
    }
    return 1;
}


/**
 * @brief send model XML format down socket
 */
static inline
dlb_pmd_success
md_http_sender_post
    (md_http_sender *mhs       /**< [in] metadata sender */
    ,dlb_pmd_model *model      /**< [in] model to send */
    )
{
    int err;

    mhs->model = model;

    MHS_DUMP(model);
    if (!dlb_http_client_open(&mhs->http_client, mhs->url, &err) &&
        !dlb_http_client_post(&mhs->http_client, "application/xml; charset=utf-8", 0, &err) &&
        !dlb_xmlpmd_write(get_buffer, 0, mhs, model) &&
        !dlb_http_client_read(&mhs->http_client, NULL, NULL)
       )
    {
        dlb_http_client_close(&mhs->http_client);
        return PMD_SUCCESS;
    }
    dlb_http_client_close(&mhs->http_client);
    return PMD_FAIL;
}


