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
 * @file pa.h
 * @brief portaudio interface for PCM+PMD realtime app
 */


#ifndef PA_H_
#define PA_H_

#include "args.h"
#include "dlb_pmd_types.h"
#include "dlb_buffer/include/dlb_buffer.h"


/**
 * @brief abstract type of the portaudio manager state
 */
typedef struct pa_state pa_state;


/**
 * @brief initialize portaudio
 */
dlb_pmd_success                   /** @return PMD_SUCCESS on success, otherwise PMD_FAIL */
pa_init
    (void
    );


/**
 * @brief finish off portaudio
 */
dlb_pmd_success                   /** @return PMD_SUCCESS on success, otherwise PMD_FAIL */
pa_finish
    (void
    );


#if 0
/**
 * @brief count
 */
dlb_pmd_success                   /** @return PMD_SUCCESS on success, otherwise PMD_FAIL */
pa_compute_channel_count
    (Args *args
    ,unsigned int *input_chans
    );
#endif


/**
 * @brief return number of input and output channels of a device
 */
dlb_pmd_success                   /** @return PMD_SUCCESS on success, otherwise PMD_FAIL */
pa_channel_count
    (unsigned int device          /**< [in] device number to query */
    ,unsigned int *input_chans    /**< [out] number of input channels */
    ,unsigned int *output_chans   /**< [out] number of output channels */
    );


/**
 * @brief enumerate all devices known to portaudio
 */
dlb_pmd_success                   /** @return PMD_SUCCESS on success, otherwise PMD_FAIL */
pa_list
    (void
    );


/**
 * @brief Initialize PortAudio state
 */
dlb_pmd_success                   /** @return PMD_SUCCESS on success, otherwise PMD_FAIL */
pa_state_init
    (pa_state   **state           /**< [out] portaudio state */
    ,Args        *args            /**< [in] command-line arguments */
    ,unsigned int input_chans     /**< [in] number of input channels */
    ,unsigned int output_chans    /**< [in] number of output channels */
    );


/**
 * @brief tear down the portaudio manager
 */
void
pa_state_finish
    (pa_state *state              /**< [in] portaudio state to tear down */
    );


/**
 * @brief set portaudio running
 */
dlb_pmd_success                   /** @return PMD_SUCCESS on success, otherwise PMD_FAIL */
pa_start
    (pa_state *state              /**< [in] portaudio manager state */
    );


/**
 * @brief stop portaudio thread if it is running
 */
dlb_pmd_success                   /** @return PMD_SUCCESS on success, otherwise PMD_FAIL */
pa_stop
    (pa_state *state              /**< [in] portaudio manager state */
    );


/**
 * @brief send a bunch of samples to portaudio
 */
void
pa_state_feed
    (pa_state   *state       /**< [in] state whose input is being fed */
    ,dlb_buffer *input       /**< [in] dlb_buffer full of samples */
    ,size_t      num_samples /**< [in] number of samples */
    );


/**
 * @brief read a bunch of samples from portaudio
 */
size_t                       /** @return number of samples read */
pa_state_read
    (pa_state   *state       /**< [in] state whose input is being fed */
    ,dlb_buffer *output      /**< [in] dlb_buffer to fill with samples */
    ,size_t      num_samples /**< [in] capacity of buffer to fill */
    );


#endif /* PA_H_ */

