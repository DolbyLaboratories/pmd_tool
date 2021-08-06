/************************************************************************
 * dlb_pmd
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
 * @file model_queue.h
 * @brief helper datastruct to handle a queue of incoming models
 */

#ifndef __MODEL_QUEUE_H__
#define __MODEL_QUEUE_H__

#include "model.h"
#include "pmd_os.h"

/**
 * @brief encapsulates a queue of waiting models
 */
typedef struct
{
    pmd_mutex lock;     /**< mutex */
    unsigned int max;   /**< queue capacity */
    unsigned int num;   /**< num actually filled */
    unsigned int front; /**< index of front of queue */
    unsigned int back;  /**< index of back of queue */
    model *mq;          /**< queue of models */
} model_queue;
    

/**
 * @brief create a queue of models
 */
static inline
dlb_pmd_success           /** @return PMD_SUCCESS if ok, PMD_FAIL otherwise */
model_queue_init
    (model_queue **mqptr  /**< [out] model queue handle to set */
    ,unsigned int max     /**< [in] maximum number of models in queue */
    )
{
    size_t memsz = sizeof(model) * max + sizeof(model_queue);
    model_queue *mq = (model_queue*)malloc(memsz);
    unsigned int i;
    if (!mq)
    {
        return PMD_FAIL;
    }
    memset(mq, '\0', memsz);
    pmd_mutex_init(&mq->lock);

    mq->max = max;
    mq->num = 0;
    mq->front = 0;
    mq->back = 0;
    mq->mq = (model *)(mq+1);

    for (i = 0; i != max; ++i)
    {
        if (model_init(&mq->mq[i]))
        {
            return PMD_FAIL;
        }
    }
    *mqptr = mq;    
    return PMD_SUCCESS;
}


/**
 * @brief destroy model queue and free resources
 */
static inline
void
model_queue_finish
    (model_queue *mq     /**< [in] model queue to finish */
    )
{
    if (mq)
    {
        unsigned int i;
        for (i = 0; i != mq->max; ++i)
        {
            model_finish(&mq->mq[i]);
        }
        mq->max = 0;
        mq->num = 0;
        pmd_mutex_finish(&mq->lock);
        free(mq);
    }
}


/**
 * @brief return front of queue, if there is one
 */
static inline
dlb_pmd_model *          /** @return 1st model of queue, or NULL if none */
model_queue_front
    (model_queue *mq     /**< [in] model queue to query */
    )
{
    dlb_pmd_model *res = NULL;

    pmd_mutex_lock(&mq->lock);
    if (mq->num)
    {
        res = mq->mq[mq->front].model;
    }
    pmd_mutex_unlock(&mq->lock);
    return res;
}


/**
 * @brief remove front of queue, if there is one
 */
static inline
dlb_pmd_model *          /** @return 1st model of queue, or NULL if none */
model_queue_pop
    (model_queue *mq     /**< [in] model queue to query */
    )
{
    dlb_pmd_model *res = NULL;

    pmd_mutex_lock(&mq->lock);
    if (mq->num)
    {
        mq->front = (mq->front + 1) % mq->max;
        mq->num -= 1;
    }
    pmd_mutex_unlock(&mq->lock);
    return res;
}


/**
 * @brief find next free model to populate (before pushing)
 */
static inline
dlb_pmd_model *          /** @return 1st free model of queue, or NULL if none */
model_queue_back
    (model_queue *mq     /**< [in] model queue to query */
    )
{
    dlb_pmd_model *res = NULL;

    pmd_mutex_lock(&mq->lock);
    if (mq->num < mq->max)
    {
        res = mq->mq[mq->back].model;        
    }
    pmd_mutex_unlock(&mq->lock);
    return res;
}


/**
 * @brief push next free model to populate (before pushing)
 */
static inline
dlb_pmd_model *          /** @return 1st free model of queue, or NULL if none */
model_queue_push
    (model_queue *mq     /**< [in] model queue to query */
    )
{
    dlb_pmd_model *res = NULL;

    pmd_mutex_lock(&mq->lock);
    if (mq->num < mq->max)
    {
        mq->back = (mq->back + 1) % mq->max;
        mq->num += 1;
    }
    pmd_mutex_unlock(&mq->lock);
    return res;
}



#endif /* __MODEL_QUEUE_H__ */


