/************************************************************************
 * dlb_pmd
 * Copyright (c) 2019-2020, Dolby Laboratories Inc.
 * Copyright (c) 2019-2020, Dolby International AB.
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

