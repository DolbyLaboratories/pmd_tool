/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2016-2019 by Dolby Laboratories,
 *                Copyright (C) 2016-2019 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

/**
 * @file pcm_vsync_timer.h
 * @brief datatype to count samples to determine where video sync occurs in PCM
 */

#ifndef PCM_VSYNC_TIMER_INC_
#define PCM_VSYNC_TIMER_INC_

#if defined(_MSC_VER)
#  if _MSC_VER < 1900 && !defined(inline)
#    define inline __inline
#  endif
#endif


/**
 * @def VF_CYCLE
 * @brief size of frame-size cycle
 *
 * Most video frame rates deliver constant frame sizes, i.e., every
 * frame contains the same number of samples.  Others, however, (e.g.,
 * the so-called 'drop frame' rates) deliver a fractional number of
 * samples per frame.  Since we cannot encode a fraction of a sample,
 * we instead allow a cycle of frame sizes that on aggregate deliver
 * the fractional sample.
 *
 * This macro defines how large this cycle of frame sizes must be.
 *
 * Note that the "drop-one" framerates 29.97, 59.94 and 119.88 don't
 * have a constant frame size - they vary over a cycle of 5 frames.
 */
#define VF_CYCLE (5)


static const size_t VF_SPACING[NUM_PMD_FRAMERATES][VF_CYCLE] =
{
    /*  23.97 fps */ { 2002, 2002, 2002, 2002, 2002 },
    /*  24    fps */ { 2000, 2000, 2000, 2000, 2000 },
    /*  25    fps */ { 1920, 1920, 1920, 1920, 1920 },
    /*  29.97 fps */ { 1601, 1602, 1601, 1602, 1602 },
    /*  30    fps */ { 1600, 1600, 1600, 1600, 1600 },
    /*  50    fps */ {  960,  960,  960,  960,  960 },
    /*  59.94 fps */ {  800,  801,  801,  801,  801 },
    /*  60    fps */ {  800,  800,  800,  800,  800 },
    /* 100    fps */ {  480,  480,  480,  480,  480 },
    /* 119.88 fps */ {  400,  400,  401,  400,  401 },
    /* 120    fps */ {  400,  400,  400,  400,  400 },
};


/**
 * @brief abstract type of video sync PCM sample counter
 */
typedef struct
{
    size_t        next_vsync;   /**< samples left until next vsync position */
    size_t        cycle_index;  /**< current framerate frame-size cycle position */
    const size_t *cycle;        /**< frame rate cycle data */
} vsync_timer;


/**
 * @brief setup vsync timer
 */
static inline
void
vsync_timer_init
   (vsync_timer        *vt           /**< [in] timer to initialize */
   ,dlb_pmd_frame_rate  rate         /**< [in] frame rate */
   ,size_t              first_vsync  /**< [in] samples until first vsync */
   )
{
    vt->next_vsync = first_vsync;
    vt->cycle_index = 0;
    vt->cycle = VF_SPACING[rate];
}


/**
 * @brief count a number of samples
 *
 * Adjust countdown to next vsync by subtracting this many samples
 */
static inline
size_t                        /** @return vsync offset in block, #VSYNC_NONE if none */
vsync_timer_add_samples
   (vsync_timer *vt           /**< [in] vsync timer */
   ,size_t       num_samples  /**< [in] number of samples read */
   )
{
    size_t res = DLB_PMD_VSYNC_NONE;
    if (vt->next_vsync < num_samples)
    {
        res = vt->next_vsync;
        while (vt->next_vsync <= num_samples)
        {
            num_samples    -= vt->next_vsync;
            vt->next_vsync  = vt->cycle[vt->cycle_index];
            vt->cycle_index = (vt->cycle_index + 1) % VF_CYCLE;
        }
    }
    vt->next_vsync -= num_samples;
    return res;
}


#endif /* PCM_VSYNC_TIMER_INC_ */
