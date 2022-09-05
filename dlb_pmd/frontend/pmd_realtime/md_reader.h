/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020 - 2022, Dolby Laboratories Inc.
 * Copyright (c) 2022, Dolby International AB.
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
 * @file md_reader.h
 * @brief abstraction of code to extract metadata out of audio stream and save it
 */


#include "dlb_pmd_pcm.h"
#include "model.h"
#include "md_http_sender.h"
#include "dlb_pmd_sadm_file.h"

#ifdef _MSC_VER
#  define snprintf _snprintf
#endif

/**
 * @brief type of metadata reader
 */
typedef struct
{
    char *md_file_name;           /**< [in] stem of desired output .xml file */
    dlb_pmd_bool filter_md;       /**< [in] dump every frame's metadata, or only if changed? */
    dlb_pmd_bool send_socket;     /**< [in] request socket output */
    dlb_pmd_bool sadm;            /**< [in] prefer sADM format over PMD? */
    md_http_sender http;          /**< [in] http send stuff */
    dlb_pcmpmd_extractor *ext;    /**< [in] PMD extractor */
    void *mem;                    /**< [in] memory for PMD extractor */
    model current;                /**< [in] current PMD model */
    model next;                   /**< [in] newly extracted PMD model */
    unsigned int frame_count;     /**< [in] number of frames read */
    unsigned int error_count;     /**< [in] number of errors discovered */
} md_reader;
    

/**
 * @brief helper function to initialize PMD exractor
 */
static inline
dlb_pmd_success                   /** @return PMD_SUCCESS on success */
extractor_init
    (md_reader *mdr               /**< [in] metadata reader to initialize */
    ,Args *args                   /**< [in] command-line arguments */
    ,unsigned int nc              /**< [in] number of input channels */
    )
{
    if (args->md_file_out)
    {
        size_t sz = dlb_pcmpmd_extractor_query_mem(PMD_TRUE);

        mdr->mem = malloc(sz);
        if (!mdr->mem) return PMD_FAIL;
        
        dlb_pcmpmd_extractor_init2(&mdr->ext,
                                   mdr->mem,
                                   args->rate,
                                   args->pmd_chan,
                                   nc,
                                   !args->subframe_mode,
                                   mdr->next.model,
                                   NULL,
                                   1);
    }
    return PMD_SUCCESS;
}


/**
 * @brief helper function to finish up PMD extractor
 */
static inline
void
extractor_finish
    (md_reader *mdr              /**< [in] metadata reader containing extractor to finish */
    )
{
    if (mdr->mem)
    {
        dlb_pcmpmd_extractor_finish(mdr->ext);
        free(mdr->mem);
        mdr->ext = NULL;
        mdr->mem = NULL;
    }
}


/**
 * @brief set up file-based metadata output
 */
static inline
void
file_writer_init
    (md_reader *mdr            /**< [in] metadata reader to initialize */
    ,Args *args                /**< [in] command-line arguments */
    )
{
    /* copy the metadata output file, and then drop the .wav suffix */
    size_t len = strlen(args->md_file_out);
    char *c;

    c = &mdr->md_file_name[len-1];
    while (c != mdr->md_file_name && *c != '.') --c;
    if (*c == '.') *c = '\0';
}


/**
 * @brief initialize a metadata reader 
 */
static inline
dlb_pmd_success
md_reader_init
    (md_reader *mdr            /**< [in] metadata reader to initialize */
    ,Args *args                /**< [in] command-line arguments */
    ,unsigned int ic           /**< [in] number of input channels */
    )
{
    memset(mdr, '\0', sizeof(*mdr));
    mdr->filter_md = args->filter_md;
    if (args->md_file_out)
    {
        size_t len = strlen(args->md_file_out);
        mdr->md_file_name = malloc(len+1);
        memset(mdr->md_file_name, '\0', len);
        strcpy(mdr->md_file_name, args->md_file_out);
        if (0 == strncmp("http://", args->md_file_out, 7))
        {
            if (md_http_sender_init(&mdr->http, args)) goto error0;
            mdr->send_socket = 1;
        }
        else
        {
            file_writer_init(mdr, args);
            mdr->send_socket = 0;
        }
        mdr->sadm = args->sadm;
    }

    if (model_init(&mdr->current)) goto error1;
    if (model_init(&mdr->next)) goto error2;
    if (extractor_init(mdr, args, ic)) goto error3;
    return PMD_SUCCESS;

  error3: model_finish(&mdr->next);
  error2: model_finish(&mdr->current);
  error1: free(mdr->md_file_name);
          md_http_sender_finish(&mdr->http);
  error0: return PMD_FAIL;
}


/**
 * @brief tidy up metadata reader
 */
static inline
void
md_reader_finish
    (md_reader *mdr                       /**< [in] metadata reader to finish */
    )
{
    extractor_finish(mdr);
    model_finish(&mdr->next);
    model_finish(&mdr->current);
    md_http_sender_finish(&mdr->http);
    free(mdr->md_file_name);
    printf("frame count:%u\n", mdr->frame_count);
}


/**
 * @brief metadata reader 'new frame' callback
 *
 * Every time the metadata extractor detects a new frame, this callback
 * will be invoked
 */
static
void
md_reader_callback
    (void *arg                            /**< [in] callback arg, the metadata reader */
    )
{
    md_reader *mdr = (md_reader*)arg;
    char filename[256];
    
    if (!mdr->filter_md || dlb_pmd_equal(mdr->current.model, mdr->next.model, 0, 0))
    {
        snprintf(filename, sizeof(filename), "%s_%u.xml", mdr->md_file_name, mdr->frame_count);

        if (mdr->send_socket)
        {
            if (md_http_sender_post(&mdr->http, mdr->next.model))
            {
                printf("Error: could not send to socket %s\n", mdr->md_file_name);
            }
        }
        else if (mdr->sadm)
        {
            if (dlb_pmd_sadm_file_write(filename, mdr->next.model))
            {
                printf("Error: could not write %s\n", filename);
            }
        }
        else if (dlb_xmlpmd_file_write(filename, mdr->next.model))
        {
            printf("Error: could not write %s\n", filename);
        }
        dlb_pmd_copy(mdr->current.model, mdr->next.model);
    }
    mdr->frame_count += 1;
}


/**
 * @brief feed the metadata reader with PCM samples
 */
static inline
void
md_reader_feed
    (md_reader *mdr                     /**< [in] metadata reader to feed */
    ,uint32_t *data                     /**< [in] pointer to start of PCM samples */
    ,size_t num_samples                 /**< [in] number of samples to feed */
    ,size_t *video_sync                 /**< [out] offset of video frame start from beginning
                                         *  of current block, or -1 */
    )
{
    if (dlb_pcmpmd_extract2(mdr->ext, data, num_samples, md_reader_callback, mdr, video_sync))
    {
        printf("%s", dlb_pmd_error(mdr->next.model));
        printf("    at frame %u\n", mdr->frame_count);
        mdr->error_count += 1;
    }
}
