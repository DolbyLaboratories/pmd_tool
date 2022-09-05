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
 * @file dlb_pmd_pcm.h
 * @brief PCM reader/writer
 *
 * This file contains the interface to routines to either:
 *
 * - adorn pure PCM with SMPTE 337m-wrapped KLV metadata (overwriting
 *    the final two channels)
 *
 * - read PCM with SMPTE 337m-wrapped KLV metadata and generate a model
 */

#ifndef DLB_PMD_PCM_H
#define DLB_PMD_PCM_H

#ifdef _MSC_VER
#  define __uint32_t uint32_t
#endif

#include "dlb_pmd_api.h"
#include "dlb_pmd_model_combo.h"
#include "dlb_pmd_klv.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @def DLB_PCMPMD_BLOCK_SIZE
 * @brief number of PCM sample sets in one block
 */
#define DLB_PCMPMD_BLOCK_SIZE (160)

/**
 * @def DLB_PMD_VSYNC_NONE
 * @brief symbolic constant indicating 'no sync point in next block'
 *
 * both #dlb_pcmpmd_extract and #dlb_pcmpmd_augment need to know
 * whether there is a video sync point within the block they are
 * operating upon, and if so, where in the block it occurs. If no
 * such point occurs, use this value.
 */
#define DLB_PMD_VSYNC_NONE (~0u)


/**
 * @brief status of writing a model within the boundary of a single frame
 */
typedef enum
{
    DLB_PCMPMD_WRITE_STATUS_ERROR = -1,     /**< Error during write */
    DLB_PCMPMD_WRITE_STATUS_GREEN,          /**< All model information written */
    DLB_PCMPMD_WRITE_STATUS_YELLOW,         /**< Some or all name payloads omitted */
    DLB_PCMPMD_WRITE_STATUS_RED             /**< Insufficient space to write the model */
}
dlb_pcmpmd_write_status;


/**
 * @brief status of parsing serial ADM
 */
typedef enum
{
    DLB_PCMSADM_OK = 0,                     /**< Success */
    DLB_PCMSADM_PARSE_ERROR = 1,            /**< Error while parsing XML */
    DLB_PCMSADM_DEC_ERROR = 2,              /**< Error while decompressing bitstream */
}
dlb_pcmsadm_status;


/**
 * @brief type of a callback used when #dlb_pcmpmd_extract2 discovers
 * a new frame or when #dlb_pcmpmd_augment2 is about to write a new
 * frame.
 */
typedef void (*dlb_pcmpmd_new_frame) (void *arg);


/**
 * @brief type of a callback used when #dlb_pcmpmd_extract3 decompresses
 * a new frame of sADM
 */
typedef void (*dlb_pcmpmd_new_sadm) (void *arg, void *buff, size_t size, dlb_pcmsadm_status result);


/**
 * @brief abstract type of structure used to augment PCM with
 * KLV metadata in last two channels.
 */
typedef struct dlb_pcmpmd_augmentor dlb_pcmpmd_augmentor;


/**
 * @brief return the smallest size in samples of a frame at the given rate,
 *        0 if frame_rate is out of range
 */
DLB_PMD_DLL_ENTRY
unsigned int
dlb_pcmpmd_min_frame_size
   (dlb_pmd_frame_rate frame_rate       /**< [in] frame rate */
   );


/**
 * @brief determine how much memory to provide to the initialization functions.
 *
 * To enable augmentation with serial ADM, set #sadm to PMD_TRUE.
 */
DLB_PMD_DLL_ENTRY
size_t                                  /**< [out] size of memory in bytes, or 0 if error */
dlb_pcmpmd_augmentor_query_mem
    (dlb_pmd_bool               sadm    /**< [in]  write serial ADM instead of PMD? */
    );


/**
 * @brief initialize PCM augmentor
 *
 * Note that we assume:
 * 1. Audio sample rate is always 48 kHz.
 *
 * The augmentor can generate either 1- or 2-channel SMPTE 337m
 * wrapped PMD (SMPTE 2109, similar to KLV format). These follow
 * the subframe and frame-based encoding of SMPTE 337m.
 *
 * The augmentor will copy the source PCM to output PCM, overwriting
 * the selected channel or pair of channels with PMD.
 *
 * If #sadm is true, the size for #mem must have been calculated by
 * calling #dlb_pcmpmd_augmentor_query_mem with #sadm true.
 */
DLB_PMD_DLL_ENTRY
void
dlb_pcmpmd_augmentor_init3
    (dlb_pcmpmd_augmentor          **augptr             /**< [in] PCM augmentor to initialize */
    ,dlb_pmd_model_combo            *model              /**< [in] PMD model */
    ,void                           *mem                /**< [in] memory for PCM augmentor */
    ,unsigned int                    wrap_depth         /**< [in] s337m wrapping bit depth (16, 20 or 24) */
    ,dlb_pmd_frame_rate              rate               /**< [in] video frame rate */
    ,dlb_klvpmd_universal_label      ul                 /**< [in] universal label */
    ,dlb_pmd_bool                    mark_empty_blocks  /**< [in] mark empty PMD blocks with SMPTE 337m NULL data bursts */
    ,unsigned int                    numchannels        /**< [in] number of channels of PCM */
    ,unsigned int                    stride             /**< [in] channel stride */
    ,dlb_pmd_bool                    is_pair            /**< [in] 1 = pair of channels, 0 = single channel */
    ,unsigned int                    start              /**< [in] 1st pair or channel to write, (see #pmd_pair) */
    ,dlb_pmd_bool                    sadm               /**< [in] generate sADM instead of PMD */
    );


/**
 * @brief initialize PCM augmentor
 *
 * Note that we assume:
 * 1. Audio sample rate is always 48 kHz.
 *
 * The augmentor can generate either 1- or 2-channel SMPTE 337m
 * wrapped PMD (SMPTE 2109, similar to KLV format). These follow
 * the subframe and frame-based encoding of SMPTE 337m.
 *
 * The augmentor will copy the source PCM to output PCM, overwriting
 * the selected channel or pair of channels with PMD.
 *
 * If #sadm is true, the size for #mem must have been calculated by
 * calling #dlb_pcmpmd_augmentor_query_mem with #sadm true.
 *
 * s337m wrapping bit depth defaults to 20, unless #sadm is true,
 * in which case it is 24.
 */
DLB_PMD_DLL_ENTRY
void
dlb_pcmpmd_augmentor_init2
    (dlb_pcmpmd_augmentor          **augptr             /**< [in] PCM augmentor to initialize */
    ,dlb_pmd_model_combo            *model              /**< [in] PMD model */
    ,void                           *mem                /**< [in] memory for PCM augmentor */
    ,dlb_pmd_frame_rate              rate               /**< [in] video frame rate */
    ,dlb_klvpmd_universal_label      ul                 /**< [in] universal label */
    ,dlb_pmd_bool                    mark_empty_blocks  /**< [in] mark empty PMD blocks with SMPTE 337m NULL data bursts */
    ,unsigned int                    numchannels        /**< [in] number of channels of PCM */
    ,unsigned int                    stride             /**< [in] channel stride */
    ,dlb_pmd_bool                    is_pair            /**< [in] 1 = pair of channels, 0 = single channel */
    ,unsigned int                    start              /**< [in] 1st pair or channel to write, (see #pmd_pair) */
    ,dlb_pmd_bool                    sadm               /**< [in] generate sADM instead of PMD */
    );


/**
 * @brief initialize PCM augmentor (for backwards compatibility with
 * previous versions)
 *
 * Note that we assume:
 * 1. Audio sample rate is always 48 kHz.
 *
 * The augmentor can generate either 1- or 2- channel SMPTE 337m
 * wrapped PMD (SMPTE 2109, similar to KLV format). These follow
 * the subframe and frame-based encoding of SMPTE 337m.
 *
 * The augmentor will copy the source PCM to output PCM, overwriting the
 * selected channel or pair of channels with PMD.
 *
 * For sADM, use #dlb_pcmpmd_augmentor_init2.
 *
 * This function calls #dlb_pcmpmd_augmentor_init2 with sadm == PMD_FALSE.
 */
DLB_PMD_DLL_ENTRY
void
dlb_pcmpmd_augmentor_init
    (dlb_pcmpmd_augmentor          **augptr             /**< [in] PCM augmentor to initialize */
    ,dlb_pmd_model_combo            *model              /**< [in] PMD model */
    ,void                           *mem                /**< [in] memory for PCM augmentor */
    ,dlb_pmd_frame_rate              rate               /**< [in] video frame rate */
    ,dlb_klvpmd_universal_label      ul                 /**< [in] universal label */
    ,dlb_pmd_bool                    mark_empty_blocks  /**< [in] mark empty PMD blocks with SMPTE 337m NULL data bursts */
    ,unsigned int                    numchannels        /**< [in] number of channels of PCM */
    ,unsigned int                    stride             /**< [in] channel stride */
    ,dlb_pmd_bool                    is_pair            /**< [in] 1 = pair of channels, 0 = single channel */
    ,unsigned int                    start              /**< [in] 1st pair or channel to write, (see #is_pair) */
    );


/**
 * @brief finalize PCM augmentor
 */
DLB_PMD_DLL_ENTRY
void
dlb_pcmpmd_augmentor_finish
    (dlb_pcmpmd_augmentor *aug  /**< [in] PCM augmentor to clean up */
    );


/**
 * @brief calculate how many bytes of external memory are
 * needed to "try" a frame
 */
DLB_PMD_DLL_ENTRY
size_t                                  /**< [out] size of memory in bytes, or 0 if error */
dlb_pcmpmd_augmentor_try_frame_query_mem
    (dlb_pcmpmd_augmentor *aug          /**< [in] PCM augmentor */
    );


/**
 * @brief "try" to write the model into a single frame
 *
 * The purpose of this function is to determine whether,
 * and how well, the serialization of the model will fit
 * into a single video frame.
 *
 * @param aug An initialized augmentor instance.
 *
 * @param mem A pointer to a block of memory of sufficient
 * size to complete the processing; you may call the
 * #dlb_pcmpmd_augmentor_try_frame_query_mem() function to
 * determine this size.
 *
 * @param buf A buffer at least numchannels * num_samples
 * in size.  The memory will be modified, but not in any
 * useful way.
 *
 * @param numchannels The number of channels in the buffer,
 * must be equal to or greater than the number of PMD channels
 * (which is 1 or 2).
 *
 * @param num_samples The number of samples in the buffer,
 * must be greater than or equal to the number of samples
 * in the minimum-size frame for the frame rate.
 *
 * @note This is an expensive operation, it is recommended
 * to call it only when the model structure changes.
 */
DLB_PMD_DLL_ENTRY
dlb_pcmpmd_write_status
dlb_pcmpmd_augmentor_try_frame
    (dlb_pcmpmd_augmentor *aug          /**< [in]     PCM augmentor */
    ,void                 *mem          /**< [in]     scratch memory needed to "try" the frame */
    ,uint32_t             *buf          /**< [in/out] modifiable buffer */
    ,unsigned int          num_channels /**< [in]     number of channels in the buffer */
    ,unsigned int          num_samples  /**< [in]     number of samples in the buffer */
    );


/**
 * @brief calculate how many bytes of external memory are
 * needed to "try" a frame, starting from a model
 */
DLB_PMD_DLL_ENTRY
size_t                                  /**< [out] size of memory in bytes, or 0 if error */
dlb_pcmpmd_augmentor_model_try_frame_query_mem
    (dlb_pmd_model_combo  *model        /**< [in] PMD model */
    ,dlb_pmd_bool          sadm         /**< [in] serial ADM encoding? */
    );


/**
 * @brief "try" to write the model into a single frame
 *
 * Similar to dlb_pcmpmd_augmentor_try_frame() but starting from a model instead
 * of an augmentor.  Use #dlb_pcmpmd_augmentor_model_try_frame_query_mem() to
 * determine the size needed for #mem.
 */
DLB_PMD_DLL_ENTRY
dlb_pcmpmd_write_status
dlb_pcmpmd_augmentor_model_try_frame
    (dlb_pmd_model_combo    *model          /**< [in]     PMD model */
    ,void                   *mem            /**< [in]     scratch memory needed to "try" the frame */
    ,uint32_t               *buf            /**< [in/out] modifiable buffer */
    ,unsigned int            num_channels   /**< [in]     number of channels in the buffer */
    ,unsigned int            num_samples    /**< [in]     number of samples in the buffer */
    ,dlb_pmd_frame_rate      rate           /**< [in]     video frame rate */
    ,dlb_pmd_bool            pair           /**< [in]     single channel or pair for metadata encoding? */
    ,dlb_pmd_bool            sadm           /**< [in]     serial ADM encoding? */
);


/**
 * @brief take a block of PCM and augment it with PMD metadata
 *
 * IMPORTANT: Note that no blocks will be written until the first video
 * sync is given, and the target channel or pair will be zeroed out.  The
 * video sync positions must be generated by or passed through from the
 * caller of this function, the augmentor does not otherwise keep track of
 * where frames start.
 *
 * Note that the PCM is assumed to be 24-bit samples, carried in a
 * 32-bit integer (because that is a handy machine datatype), such
 * that the 24 bits are shifted to the most significant bits.
 */
DLB_PMD_DLL_ENTRY
void
dlb_pcmpmd_augment
    (dlb_pcmpmd_augmentor *aug          /**< [in]     PCM augmentor */
    ,uint32_t             *pcm          /**< [in/out] PCM channel buffer to modify */
    ,size_t                num_samples  /**< [in]     number of samples in current block */
    ,size_t                video_sync   /**< [in]     video frame sync occurs at the given position
                                          *           in the input block, or use #DLB_PMD_VSYNC_NONE
                                          *           if there is no sync point in the current block. */
    );


/**
 * @brief take a block of PCM and augment it with PMD metadata
 *
 * This is just #dlb_pcmpmd_augment, but in addition it will invoke a
 * callback whenever a new frame is about to be written. This allows
 * clients to update models so that they begin to be written at the
 * start of the frame, without fear of inconsistencies with previous
 * metadata blocks.
 */
DLB_PMD_DLL_ENTRY
void
dlb_pcmpmd_augment2
    (dlb_pcmpmd_augmentor *aug          /**< [in]     PCM augmentor */
    ,uint32_t             *pcm          /**< [in/out] PCM channel buffer to modify */
    ,size_t                num_samples  /**< [in]     number of samples in current block */
    ,size_t                video_sync   /**< [in]     video frame sync occurs at the given position
                                          *           in the input block, or use #DLB_PMD_VSYNC_NONE
                                          *           if there is no sync point in the current block. */
    ,dlb_pcmpmd_new_frame  callback     /**< [in]     callback to invoke when a new frame is started */
    ,void                 *cbarg        /**< [in]     user argument to callback */
    );


/**
 * @brief abstract type of structure used to remove KLV metadata
 * from final two channels in PCM.
 */
typedef struct dlb_pcmpmd_extractor dlb_pcmpmd_extractor;


/**
 * @brief determine number of bytes required to initialize an extractor
 *
 * To enable extraction with serial ADM, set #sadm to PMD_TRUE.
 */
DLB_PMD_DLL_ENTRY
size_t                                  /** @return number of bytes */
dlb_pcmpmd_extractor_query_mem
    (dlb_pmd_bool               sadm    /**< [in] sADM supported? */
    );


/**
 * @brief initialize PCM extractor
 *
 * Same as #dlb_pcmpmd_extractor_init2, with an additional argument to specify s337m wrapping bit depth.
 */
DLB_PMD_DLL_ENTRY
void
dlb_pcmpmd_extractor_init3
    (dlb_pcmpmd_extractor          **extptr     /**< [out] PCM extractor to initialize */
    ,void                           *mem        /**< [in]  memory to use to initialize extractor */
    ,unsigned int                    wrap_depth /**< [in]  s337m wrapping bit depth (16, 20 or 24) */
    ,dlb_pmd_frame_rate              rate       /**< [in]  video frame rate */
    ,unsigned int                    chan       /**< [in]  channel (or 1st channel of pair) to decode */
    ,unsigned int                    stride     /**< [in]  PCM stride */
    ,dlb_pmd_bool                    ispair     /**< [in]  1: extract a pair, 0: extract single channel */
    ,dlb_pmd_model_combo            *model      /**< [in]  model to populate */
    ,dlb_pmd_payload_set_status     *status     /**< [out] payload set status, may be NULL; if given,
                                                           must be initialized properly */
    ,dlb_pmd_bool                    sadm       /**< [in]  support DLB-serial ADM? */
    );


/**
 * @brief initialize PCM extractor
 *
 * If #sadm is true, the size for #mem must have been calculated by
 * calling #dlb_pcmpmd_extractor_query_mem with #sadm true.
 *
 * If #sadm is not true, and the extractor encounters serial ADM
 * content, that content will be ignored.
 *
 * s337m wrapping bit depth defaults to 20, unless #sadm is true,
 * in which case it is 24.
 */
DLB_PMD_DLL_ENTRY
void
dlb_pcmpmd_extractor_init2
    (dlb_pcmpmd_extractor          **extptr     /**< [out] PCM extractor to initialize */
    ,void                           *mem        /**< [in]  memory to use to initialize extractor */
    ,dlb_pmd_frame_rate              rate       /**< [in]  video frame rate */
    ,unsigned int                    chan       /**< [in]  channel (or 1st channel of pair) to decode */
    ,unsigned int                    stride     /**< [in]  PCM stride */
    ,dlb_pmd_bool                    ispair     /**< [in]  1: extract a pair, 0: extract single channel */
    ,dlb_pmd_model_combo            *model      /**< [in]  model to populate */
    ,dlb_pmd_payload_set_status     *status     /**< [out] payload set status, may be NULL; if given,
                                                           must be initialized properly */
    ,dlb_pmd_bool                    sadm       /**< [in]  support DLB-serial ADM? */
    );


/**
 * @brief initialize PCM extractor
 *
 * For sADM, use #dlb_pcmpmd_extractor_init2.  This forwards its arguments to #dlb_pcmpmd_extractor_init2,
 * plus sadm == PMD_FALSE.
 */
DLB_PMD_DLL_ENTRY
void
dlb_pcmpmd_extractor_init
    (dlb_pcmpmd_extractor          **extptr     /**< [out] PCM extractor to initialize */
    ,void                           *mem        /**< [in]  memory to use to initialize extractor */
    ,dlb_pmd_frame_rate              rate       /**< [in]  video frame rate */
    ,unsigned int                    chan       /**< [in]  channel (or 1st channel of pair) to decode */
    ,unsigned int                    nstride    /**< [in]  PCM stride */
    ,dlb_pmd_bool                    ispair     /**< [in]  1: extract a pair, 0: extract single channel */
    ,dlb_pmd_model_combo            *model      /**< [in]  model to populate */
    ,dlb_pmd_payload_set_status     *status     /**< [out] payload set status, may be NULL; if given,
                                                           must be initialized properly */
    );


/**
 * @brief clean up resources
 */
DLB_PMD_DLL_ENTRY
void
dlb_pcmpmd_extractor_finish
    (dlb_pcmpmd_extractor *ext      /**< [in] PCM extractor to tidy up */
    );


/**
 * @brief unwrap one block of PCM
 *
 * Note that the PCM is expected to be 24-bit samples carried in
 * 32-bit machine words.
 *
 * Note that a non-zero return code indicates that the encoded
 * metadata failed to decode; the PCM can continue to be extracted.
 *
 * Note that this version depends upon an external video frame sync
 * for determining the start of video frames.  A common misconception
 * is that giving #DLB_PMD_VSYNC_NONE from the start of decoding will
 * cause automatic detection of frame start, however, what really
 * happens is no metadata will be decoded until a sync point is given.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                        /** @return 0 on success, non-zero otherwise */
dlb_pcmpmd_extract
    (dlb_pcmpmd_extractor *ext         /**< [in] PCM extractor struct */
    ,uint32_t             *pcm         /**< [in] PCM to unwrap */
    ,size_t                num_samples /**< [in] number of PCM sample sets */
    ,size_t                video_sync  /**< [in] video frame sync occurs at given line,
                                         *       or #DLB_PMD_VSYNC_NONE if no frame sync. */
    );


/**
 * @brief unwrap one block of PCM
 *
 * Note that the PCM is expected to be 24-bit samples carried in
 * 32-bit machine words.
 *
 * Note that a non-zero return code indicates that the encoded
 * metadata failed to decode; the PCM can continue to be extracted.
 *
 * Note that this is a variant of #dlb_pcmpmd_extract, which attempts
 * to automatically discover start of frames.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                        /** @return 0 on success, non-zero otherwise */
dlb_pcmpmd_extract2
    (dlb_pcmpmd_extractor *ext         /**< [in]  PCM extractor struct */
    ,uint32_t             *pcm         /**< [in]  PCM to unwrap */
    ,size_t                num_samples /**< [in]  number of PCM sample sets */
    ,dlb_pcmpmd_new_frame  callback    /**< [in]  callback to invoke when new frame is found */
    ,void                 *cbarg       /**< [in]  user argument to callback */
    ,size_t               *video_sync  /**< [out] video frame sync occurs at given line,
                                         *        or #DLB_PMD_VSYNC_NONE if no frame sync. */
    );


/**
 * @brief unwrap one block of PCM
 *
 * This is a variant of #dlb_pcmpmd_extract2, adding a callback function
 * and buffer for serial ADM
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                             /** @return 0 on success, non-zero otherwise */
dlb_pcmpmd_extract3
    (dlb_pcmpmd_extractor *ext              /**< [in]  PCM extractor struct */
    ,uint32_t             *pcm              /**< [in]  PCM to unwrap */
    ,size_t                num_samples      /**< [in]  number of PCM sample sets */
    ,dlb_pcmpmd_new_frame  callback         /**< [in]  callback to invoke when new frame is found */
    ,dlb_pcmpmd_new_sadm   sadm_callback    /**< [in]  callback to invoke when new sadm packet is processed */
    ,char                 *sadm_xml_buf     /**< [in]  buffer to store sadm xml */
    ,void                 *cbarg            /**< [in]  user argument to callback */
    ,size_t               *video_sync       /**< [out] video frame sync occurs at given line,
                                             **<       or #DLB_PMD_VSYNC_NONE if no frame sync. */
    );


DLB_PMD_DLL_ENTRY
dlb_pmd_bool
dlb_pcmpmd_extractor_error_flag
    (dlb_pcmpmd_extractor   *ext            /**< [in]  PCM extractor struct */
    );


DLB_PMD_DLL_ENTRY
const char *
dlb_pcmpmd_extractor_error_msg
    (dlb_pcmpmd_extractor   *ext            /**< [in]  PCM extractor struct */
    );


#ifdef __cplusplus
}
#endif

#endif /* DLB_PMD_PCM_H */
