/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019-2021 by Dolby Laboratories,
 *                Copyright (C) 2019-2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

/**
 * @file md_writer.h
 * @brief abstraction of code to add metadata into the audio stream before
 * outputting it
 */


#include "dlb_pmd_pcm.h"
#include "args.h"
#include "md_http_listener.h"
#include <stdlib.h>


/**
 * @def MODEL_QUEUE_SIZE (2)
 * @brief size of pending model queue
 *
 */
#define MODEL_QUEUE_SIZE (2)


/**
 * @brief type of the metadata writer
 */
typedef struct
{
    dlb_pmd_frame_rate rate;       /**< PCM frame rate */
    dlb_klvpmd_universal_label ul; /**< which PMD KLV 16-byte key to use */
    dlb_pmd_bool mark_pcm_blocks;  /**< mark empty PCM blocks with NULL SMPTE 337m frames? */
    dlb_pmd_bool subframe_mode;    /**< single-channel, or pair mode SMPTE 337m wrapping for PMD? */
    unsigned int pmd_chan;         /**< start channel of PMD */

    md_http_listener *mhl;        /**< optional HTTP listener */
    dlb_pcmpmd_augmentor *aug;    /**< PCM augmentor */
    void *mem;                    /**< memory for the augmentor */
    model current;                /**< currently poplulated model for writing */
    model_queue *pending;         /**< queue of pending models */
} md_writer;


/**
 * @brief helper function to set up the PCM+PMD augmentor of a metadata writer
 */
static inline
dlb_pmd_success                   /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
augmentor_init
    (md_writer *mdw               /**< [in] metadata writer to populate */
    ,Args *args                   /**< [in] command-line arguments */
    ,unsigned int nc              /**< [in] number of channels */
    )
{
    if (!mdw->mem)
    {
        size_t sz;

        sz = dlb_pcmpmd_augmentor_query_mem(args->sadm);
        mdw->mem = malloc(sz);
    }
    if (!mdw->mem)
    {
        return PMD_FAIL;
    }
    
    dlb_pcmpmd_augmentor_init2(&mdw->aug,
                               mdw->current.model,
                               mdw->mem,
                               args->rate,
                               args->ul,
                               args->mark_pcm_blocks,
                               nc,
                               nc,
                               !args->subframe_mode,
                               args->pmd_chan,
                               args->sadm);
    return PMD_SUCCESS;
}


/**
 * @brief helper function to finish off an augmentor
 */
static inline
void
augmentor_finish
    (md_writer *mdw             /**< [in] metadata writer containing augmentor to finish */
    )
{
    if (mdw->mem)
    {
        dlb_pcmpmd_augmentor_finish(mdw->aug);
        free(mdw->mem);
        mdw->aug = NULL;
        mdw->mem = NULL;
    }
}


/**
 * @brief initialize a metadata writer
 */
static inline
dlb_pmd_success                /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
md_writer_init
    (md_writer *mdw            /**< [in] metadata writer to initialize */
    ,Args *args                /**< [in] command-line arguments */
    ,unsigned int nc           /**< [in] number of channels */
    )
{
    if (model_init(&mdw->current)) goto error1;
    if (model_queue_init(&mdw->pending, MODEL_QUEUE_SIZE)) goto error2;
    if (model_populate(&mdw->current, args->md_file_in)) goto error3;
    if (augmentor_init(mdw, args, nc)) goto error3;
    if (md_http_listener_init(args, mdw->pending, &mdw->mhl)) goto error4;
    return PMD_SUCCESS;

  error4: augmentor_finish(mdw);
  error3: model_queue_finish(mdw->pending);
  error2: model_finish(&mdw->current);
  error1: return PMD_FAIL;
}


/**
 * @brief finish a metadata writer
 */
static inline
void
md_writer_finish
    (md_writer *mdw            /**< [in] metadata writer to finish */
    )
{
    md_http_listener_finish(mdw->mhl);
    augmentor_finish(mdw);
    model_queue_finish(mdw->pending);
    model_finish(&mdw->current);
}


/**
 * @brief client callback invoked whenever PCM augmentor is about to begin
 * a new frame
 *
 * We use this time to update models, if we have a new one pending.
 */
static
void
new_frame_callback
    (void *arg
    )
{
    md_writer *mdw = (md_writer*)arg;
    dlb_pmd_model *model = model_queue_front(mdw->pending);
    if (model)
    {
        dlb_pmd_copy(mdw->current.model, model);
        model_queue_pop(mdw->pending);
    }
}


/**
 * @brief send a block of samples to the writer to be overwritten by metadata
 */
static inline
void
md_writer_write
    (md_writer *mdw            /**< [in] metadata writer */
    ,uint32_t *data            /**< [in] start of PCM data to overwrite */
    ,size_t num_samples        /**< [in] number of samples of audio in block */
    ,size_t video_sync         /**< [in] position of video frame boundary in given
                                * PCM block, or -1 if none */
    )
{
    dlb_pcmpmd_augment2(mdw->aug, data, num_samples, video_sync, new_frame_callback, mdw);   
}
