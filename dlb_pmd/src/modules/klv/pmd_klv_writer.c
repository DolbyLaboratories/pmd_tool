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
 * @file klv_writer.c
 * @brief PMD model SMPTE 336 KLV writer
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <stdint.h>

//#define KLV_WRITE_TRACE
#ifdef KLV_WRITE_TRACE
#  define TRACE(x) printf x
#else
#  define TRACE(x)
#endif

#include "dlb_pmd_klv.h"
#include "pmd_model.h"
#include "klv_writer.h"
#include "klv_abd.h"
#include "klv_aod.h"
#include "klv_xyz.h"
#include "klv_hed.h"
#include "klv_eep.h"
#include "klv_esd.h"
#include "klv_esn.h"
#include "klv_etd.h"
#include "klv_aen.h"
#include "klv_iat.h"
#include "klv_apd.h"
#include "klv_pld.h"
#include "klv_apn.h"

/**
 * @brief write all dynamic updates in the current block to KLV format
 */
static inline
int                           /** @return 0 on success 1 on failure */
klv_write_xyz_key
   (klv_writer *w             /**< [in] writer struct */
   ,dlb_pmd_model *model      /**< [in] source model */
   ,unsigned int starttime    /**< [in] starting sample number */
   ,unsigned int endtime      /**< [in] ending sample nbumber */
   )
{
    if (model->num_xyz)
    {
        unsigned int time;
        if (endtime > 64) endtime = 64;
        for (time = starttime; time != endtime; ++time)
        {
            if (   klv_write_local_open(w, KLV_PMD_LOCAL_TAG_DYNAMIC_UPDATES)
                || klv_xyz_write(w, time)
                || klv_write_local_close(w))
            {
                return 1;
            }
        }
    }
    return 0;
}


/**
 * @brief write Headphone Element Description to KLV format
 */
static inline
int                           /** @return 0 on success 1 on failure */
klv_write_hed_key
   (klv_writer *w             /**< [in] writer struct */
   ,dlb_pmd_model *model      /**< [in] source model */
   )
{
    if (model->write_state.hed_written != model->num_hed)
    {
        if (   klv_write_local_open(w, KLV_PMD_LOCAL_TAG_HEADPHONE_ELEMENT_DESC)
            || klv_hed_write(w, model)
            || klv_write_local_close(w))
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief write IAT to KLV format
 */
static inline
int                           /** @return 0 on success 1 on failure */
klv_write_iat_key
   (klv_writer *w             /**< [in] writer struct */
   ,dlb_pmd_model *model      /**< [in] source model */
   )
{
    pmd_bool written;

    if (model->iat && !model->write_state.iat_written)
    {
        if (model->iat->options & PMD_IAT_PRESENT)
        {
            if (   klv_write_local_open(w, KLV_PMD_LOCAL_TAG_IAT)
                || klv_iat_write(w, model, &written)
                || klv_write_local_close(w))
            {
                return 1;
            }
            model->write_state.iat_written = written;
        }
    }
    return 0;
}


/**
 * @brief write ED2 stream description (if it is present) to KLV format
 */
static inline
int                           /** @return 0 on success 1 on failure */
klv_write_esd_key
   (klv_writer *w             /**< [in] writer struct */
   ,dlb_pmd_model *model      /**< [in] source model */
   )
{
    pmd_bool written = PMD_FALSE;
    
    if (model->esd && model->esd_present && model->write_state.esd_written == w->sindex)
    {
        if (   klv_write_local_open(w, KLV_PMD_LOCAL_TAG_ED2_SUBSTREAM_DESC)
            || klv_esd_write(w, model->esd, &written)
            || klv_write_local_close(w))
        {
            return 1;
        }
        model->write_state.esd_written = (w->sindex+1) % model->esd->count;
    }
    return 0;
}


/**
 * @brief write ED2 stream names (if it is present) to KLV format
 */
static inline
int                           /** @return 0 on success 1 on failure */
klv_write_esn_key
   (klv_writer *w             /**< [in] writer struct */
   ,dlb_pmd_model *model      /**< [in] source model */
   )
{
    unsigned int sindex = w->sindex;

    if (sindex == DLB_PMD_NO_ED2_STREAM_INDEX)
    {
        sindex = 0;
    }
    
    if (!(model->write_state.esn_bitmap & (1ul << sindex)))
    {
        if (   klv_write_local_open(w, KLV_PMD_LOCAL_TAG_ED2_SUBSTREAM_NAMES)
            || klv_esn_write(w, model, sindex)
            || klv_write_local_close(w))
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief write presentation names to KLV format
 */
static inline
int                           /** @return 0 on success 1 on failure */
klv_write_apn_key
   (klv_writer *w             /**< [in] writer struct */
   ,dlb_pmd_model *model      /**< [in] source model */
   )
{
    if (!pmd_apn_list_iterator_done(&model->write_state.apni))
    {
        if (   klv_write_local_open(w, KLV_PMD_LOCAL_TAG_AUDIO_PRESENTATION_NAMES)
            || klv_apn_write(w, model)
            || klv_write_local_close(w))
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief write all EAC3 encoding parameters to KLV format
 *
 * @todo: distribute these across blocks depending how much
 * space we have left after IAT and dynamic updates
 */
static inline
int                           /** @return 0 on success 1 on failure */
klv_write_eep_key
   (klv_writer *w             /**< [in] writer struct */
   ,dlb_pmd_model *model      /**< [in] source model */
   )
{
    if (model->write_state.eep_written < model->num_eep)
    {
        if (   klv_write_local_open(w, KLV_PMD_LOCAL_TAG_EAC3_ENCODING_PARAMETERS)
            || klv_eep_write(w, model)
            || klv_write_local_close(w))
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief write all ED2 Turnaround descriptions to KLV format
 *
 * @todo: distribute these accross blocks depending how much
 * space we have left after IAT and dynamic updates
 */
static inline
int                           /** @return 0 on success 1 on failure */
klv_write_etd_key
   (klv_writer *w             /**< [in] writer struct */
   ,dlb_pmd_model *model      /**< [in] source model */
   )
{
    if (model->write_state.etd_written < model->num_etd)
    {
        if (   klv_write_local_open(w, KLV_PMD_LOCAL_TAG_ED2_TURNAROUND_DESC)
            || klv_etd_write(w, model)
            || klv_write_local_close(w))
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief write all presentation loudness descriptions to KLV format
 *
 * @todo: distribute these accross blocks depending how much
 * space we have left after previous payloads
 */
static inline
int                           /** @return 0 on success 1 on failure */
klv_write_pld_key
   (klv_writer *w             /**< [in] writer struct */
   ,dlb_pmd_model *model      /**< [in] source model */
   )
{
    if (model->write_state.pld_written < model->num_pld)
    {
        if (   klv_write_local_open(w, KLV_PMD_LOCAL_TAG_PRES_LOUDNESS_DESC)
            || klv_pld_write(w, model)
            || klv_write_local_close(w))
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief write all element names to KLV format
 *
 * @todo: distribute these across blocks depending how much
 * space we have left after IAT and dynamic updates
 */
static inline
int                           /** @return 0 on success 1 on failure */
klv_write_aen_key
   (klv_writer *w             /**< [in] writer struct */
   ,dlb_pmd_model *model      /**< [in] source model */
   )
{
    if (model->write_state.aen_written < model->num_elements)
    {
        if (   klv_write_local_open(w, KLV_PMD_LOCAL_TAG_AUDIO_ELEMENT_NAMES)
            || klv_aen_write(w, model)
            || klv_write_local_close(w))
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief write 1st APA block (Mtx(0)) after video sync
 *
 * The first block after video sync is always the list of audio objects
 * and presentations
 */
static
int                           /** @return 0 on success 1 on failure */
klv_write_mtx0
   (klv_writer *w             /**< [in] writer struct */
   ,dlb_pmd_model *model      /**< [in] source model */
   ,unsigned int block_number /**< [in] block number */
   )
{
    /* make sure we have cycled through all different types of
     * optional payloads before starting again
     */
    if (0 == block_number || DLB_PMD_NO_ED2_STREAM_INDEX == block_number)
    {
        /* beds, objects and presentations are all that is needed to render,
         * so they can be reset even when the other payloads haven't completely
         * been sent */
        model->write_state.abd_written = 0;
        model->write_state.aod_written = 0;
        model->write_state.apd_written = 0;
        model->write_state.hed_written = 0;
        model->write_state.bed_write_index = 0;
        model->write_state.obj_write_index = 0;
        model->write_state.iat_written = 0;
        model->write_state.esd_written = 0;

        if (   model->num_eep == model->write_state.eep_written
            && model->num_etd == model->write_state.etd_written
            && (!model->esd || (model->esd->count == model->write_state.esn_written))
            && model->num_pld == model->write_state.pld_written
            && pmd_apn_list_iterator_done(&model->write_state.apni)
            && model->num_elements == model->write_state.aen_written
          )
        {
            model->write_state.pld_written = 0;
            model->write_state.eep_written = 0;
            model->write_state.etd_written = 0;
            model->write_state.apn_written = 0;
            model->write_state.aen_written = 0;
            model->write_state.esn_written = 0;
            model->write_state.esn_bitmap  = 0;
        }

        if (   0 == model->write_state.eep_written
            && 0 == model->write_state.etd_written
            && 0 == model->write_state.pld_written
            && 0 == model->write_state.apn_written
            && 0 == model->write_state.aen_written
           )
        {
            pmd_apn_list_iterator_init(&model->write_state.apni, &model->apn_list);
        }
    }

    /* write ESD if it exists and is possible */
    if (klv_write_esd_key(w, model))
    {
        return 1;
    }

    if (   klv_write_local_open(w, KLV_PMD_LOCAL_TAG_AUDIO_BED_DESC)
        || klv_abd_write(w, model)
        || klv_write_local_close(w))
    {
        return 1;
    }
    
    if (   klv_write_local_open(w, KLV_PMD_LOCAL_TAG_AUDIO_OBJECT_DESC)
        || klv_aod_write(w, model)
        || klv_write_local_close(w))
    {
        return 1;
    }

    if (   klv_write_local_open(w, KLV_PMD_LOCAL_TAG_AUDIO_PRESENTATION_DESC)
        || klv_apd_write(w, model)
        || klv_write_local_close(w))
    {
        return 1;
    }

    return 0;
}


/**
 * @brief write 'inner' block of video frame
 *
 * Inner blocks are used to store additional payloads, such as IAT
 * ECD, presentation names etc.
 */
static
int                           /** @return 0 on success 1 on failure */
klv_write_block
   (klv_writer *w             /**< [in] writer struct */
   ,dlb_pmd_model *model      /**< [in] source model */
   ,unsigned int blockno      /**< [in] PCM block number 'within video frame' */
   )
{
    if (0 == blockno)
    {
        return klv_write_mtx0(w, model, blockno)
            || klv_write_hed_key(w, model);
    }
    else
    {
        return klv_write_mtx0(w, model, blockno)
            || klv_write_hed_key(w, model)
            || klv_write_iat_key(w, model)
            || klv_write_xyz_key(w, model, blockno*5, (blockno+1)*5)
            || klv_write_eep_key(w, model)
            || klv_write_etd_key(w, model)
            || klv_write_pld_key(w, model)
            || klv_write_apn_key(w, model)
            || klv_write_esn_key(w, model)
            || klv_write_aen_key(w, model);
    } 
}


unsigned int
dlb_klvpmd_min_block_size
    (void
    )
{
    unsigned int size = UNIVERSAL_KEY_SIZE
        + 4 /* length field */
        + 4 /* min container config payload */
        + 4 /* version field */
        + 6 /* CRC32 field */
        ;
    /* we always round up to next highest even number of 20-bit words,
     * reflecting an integral number of SMPTE 337m PCM frames.
     */
    return ((size + 9) / 10) * 10;
}


int
dlb_klvpmd_write_block
   (dlb_pmd_model             *model
   ,uint8_t                    sindex
   ,unsigned int               block_number
   ,uint8_t                   *buffer
   ,size_t                     capacity
   ,dlb_klvpmd_universal_label ul
   )
{
    klv_writer w;
    int res = 0;
    int bytes_written = 0;

    TRACE(("write block %u...\n", block_number));

    pmd_mutex_lock(&model->lock);

    klv_writer_init(&w, model, sindex, buffer, capacity, ul);
    res = klv_write_block(&w, model, block_number);
    if (!res)
    {
        bytes_written = klv_writer_finish(&w);
    }

    pmd_mutex_unlock(&model->lock);
    return bytes_written;
}


int
dlb_klvpmd_write_all
   (dlb_pmd_model *model
   ,uint8_t sindex
   ,unsigned char *buffer
   ,size_t capacity
   ,dlb_klvpmd_universal_label ul
   )
{
    klv_writer w;
    int res;
    
    TRACE(("write all, substream %u...\n", sindex));

    pmd_mutex_lock(&model->lock);    

    if (0 == sindex || DLB_PMD_NO_ED2_STREAM_INDEX == sindex)
    {
        model->write_state.bed_write_index = 0;
        model->write_state.obj_write_index = 0;
        model->write_state.abd_written = 0;
        model->write_state.aod_written = 0;
        model->write_state.apd_written = 0;
        model->write_state.hed_written = 0;
        model->write_state.iat_written = 0;
        model->write_state.esd_written = 0;

        if (   DLB_PMD_NO_ED2_STREAM_INDEX == sindex
            || (model->num_eep == model->write_state.eep_written
                && model->num_etd == model->write_state.etd_written
                && (!model->esd || (model->esd->count == model->write_state.esn_written))
                && model->num_pld == model->write_state.pld_written
                && pmd_apn_list_iterator_done(&model->write_state.apni)
                && model->num_elements == model->write_state.aen_written
               )
           )
        {
            model->write_state.pld_written = 0;
            model->write_state.eep_written = 0;
            model->write_state.etd_written = 0;
            model->write_state.apn_written = 0;
            model->write_state.aen_written = 0;
            model->write_state.esn_written = 0;
            model->write_state.esn_bitmap  = 0;
        }

        if (0 == model->write_state.eep_written
            && 0 == model->write_state.etd_written
            && 0 == model->write_state.pld_written
            && 0 == model->write_state.apn_written
            && 0 == model->write_state.aen_written
           )
        {
            pmd_apn_list_iterator_init(&model->write_state.apni, &model->apn_list);
        }
        pmd_xyz_set_init(&model->write_state.xyz_written);
    }
    
    if (model->esd && model->esd->count > 1)
    {
        TRACE(("ED2: SUBSTREAM %u/%u\n", sindex, model->esd ? model->esd->count : 0));
        TRACE(("    esd_written: %u\n", model->write_state.esd_written));
        TRACE(("    iat_written: %u\n", model->write_state.iat_written));
        TRACE(("    abd_written: %u\n", model->write_state.abd_written));
        TRACE(("    aod_written: %u\n", model->write_state.aod_written));
        TRACE(("    apd_written: %u\n", model->write_state.apd_written));
        TRACE(("    hed_written: %u\n", model->write_state.hed_written));
        TRACE(("    pld_written: %u\n", model->write_state.pld_written));
        TRACE(("    aen_written: %u\n", model->write_state.apn_written));
        TRACE(("    aen_written: %u\n", model->write_state.aen_written));
        TRACE(("    eep_written: %u\n", model->write_state.eep_written));
        TRACE(("    etd_written: %u\n", model->write_state.etd_written));
        TRACE(("    esn_written: %u\n", model->write_state.esn_written));
        TRACE(("    xyz_written: %u\n", model->write_state.xyz_written.bitmap[0]));
    }   

    klv_writer_init(&w, model, sindex, buffer, capacity, ul);
    /* we must *always* be able to write MTx(0) */
    if (   klv_write_mtx0(&w, model, sindex)
        || klv_write_hed_key(&w, model)
        || klv_write_iat_key(&w, model)
        || klv_write_xyz_key(&w, model, 0, 64) /* 63 is max possible update time value */
        || klv_write_eep_key(&w, model)
        || klv_write_etd_key(&w, model)
        || klv_write_pld_key(&w, model)
        || klv_write_apn_key(&w, model)
        || klv_write_aen_key(&w, model)
        || klv_write_esn_key(&w, model)
       )
    {
        res = 0;
    }
    else
    {
        res = klv_writer_finish(&w);
    }

    pmd_mutex_unlock(&model->lock);
    return res;
}
