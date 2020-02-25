/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020, Dolby Laboratories Inc.
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

#ifndef PMD_ABD_AOD_H
#define PMD_ABD_AOD_H

#include "pmd_bitstream_version.h"
#include "pmd_types.h"
#include "pmd_channels.h"
#include "pmd_language.h"
#include "pmd_signals.h"

#include <assert.h>
#include <stdlib.h>

 /* Object position */
#define PMD_OBJECT_POSITION_MIN (-1.0f)
#define PMD_OBJECT_POSITION_MAX (1.0f)

 /*  Object position encoding*/
#define PMD_OBJECT_POSITION_ENC_RESERVED (0)
#define PMD_OBJECT_POSITION_ENC_MAX (0x3ff)

 /* Object size */
#define PMD_OBJECT_SIZE_MIN (0.0f)
#define PMD_OBJECT_SIZE_MAX (1.0f)

/**
 * @file pmd_abd_aod.h
 * @brief models of audio element beds and objects (ABD and AOD) data stucture 
 *
 * This is a work-in-progress!
 *
 * An Audio Element in PMD is one-or-more audio signals that together
 * represent a consistent unit.  For example, it might be a single
 * track of audio (e.g., dialog object) or a set of tracks
 * representing an entire 5.1 bed.
 *
 * Audio Elements can be either static or dynamic, i.e., they can
 * either move in the spatial field or not.  Channel-based audio
 * objects (sometimes known as "beds") typically correspond to fixed
 * speaker positions, and tend not to move.
 *
 * (In principle you could try and lock several channels together in a
 * unit and 'move' them in sync.  The binary payload format will
 * support that use case, though the structures defined here will
 * not.)
 *
 * This file defines the data structure used to represent audio
 * objects inside the PMD model.
 */


/**
 * @brief channel position in group of PCM
 *
 * A track index is always relative to an input PCM stream:
 *
 * Track index 0 is the 1st PCM channel, usually Left.
 * Track index 1 is the 2nd PCM channel, often Right, sometimes Center.
 *
 * A track index is independent of any a-priori understanding of the
 * content of the channel.  For example, the incoming PCM may be
 * legacy channel-based 5.1, in which case track 0 will mean "Left".
 * Alternatively, the incoming PCM could be entirely object based, in
 * which case track 0 will simply mean "Object 0".
 */
typedef uint8_t pmd_track_index;


/**
 * @brief helper function to validate audio element ID
 */
static inline
dlb_pmd_payload_status
pmd_validate_audio_element_id
    (dlb_pmd_element_id id
    )
{
    dlb_pmd_payload_status status = DLB_PMD_PAYLOAD_STATUS_OK;

    if (id == DLB_PMD_RESERVED_ELEMENT_ID)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_RESERVED;
    }
    else if (id > DLB_PMD_MAX_ELEMENT_ID)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE;
    }

    return status;
}


/**
 * @brief helper function to validate presentation ID
 */
static inline
dlb_pmd_payload_status
pmd_validate_presentation_id
    (dlb_pmd_presentation_id id
    )
{
    dlb_pmd_payload_status status = DLB_PMD_PAYLOAD_STATUS_OK;

    if (id == DLB_PMD_RESERVED_PRESENTATION_ID)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_RESERVED;
    }
    else if (id > DLB_PMD_MAX_PRESENTATION_ID)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE;
    }

    return status;
}


/**
 * @brief represent object position
 *
 * An object position is specified in the range -1.0 - 1.0.
 * For x-coordinate, -1.0 means 'left' and 1.0 means 'right'.
 * For y-coordinate, -1.0 means 'back' and 1.0 means 'front'.
 * For z-coordinate, -1.0 means 'floor', 0.0 means 'horizon' and 1.0 means
 * 'ceiling'
 *
 * These are represented as an unsigned short in the range 1 - 0x3ff,
 * in units of 1.0/1024.  (i.e., 10 bits.)
 */
typedef unsigned short pmd_position;


/**
 * @brief helper function to validate encoded position
 */
static inline
dlb_pmd_payload_status
pmd_validate_encoded_position
    (pmd_position pos
    )
{
    dlb_pmd_payload_status status = DLB_PMD_PAYLOAD_STATUS_OK;

    if (pos == PMD_OBJECT_POSITION_ENC_RESERVED)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_RESERVED;
    }
    else if (pos > PMD_OBJECT_POSITION_ENC_MAX)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE;
    }

    return status;
}


/**
 * @brief helper function to validate position
 */
static inline
dlb_pmd_payload_status
pmd_validate_position
    (float pos
    )
{
    dlb_pmd_payload_status status = DLB_PMD_PAYLOAD_STATUS_OK;

    if (pos < PMD_OBJECT_POSITION_MIN || pos > PMD_OBJECT_POSITION_MAX)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE;
    }

    return status;
}


/**
 * @brief helper function to convert between floating pt and encoded position
 */
static inline
float
pmd_decode_position
    (pmd_position pos
    )
{
    assert(pos != 0);
    return (((float)(pos-1)) / 0x3fe) * 2.0f - 1.0f;
}


/**
 * @brief helper function to convert between floating pt and encoded position
 */
static inline
pmd_position
pmd_encode_position
    (float pos
    )
{
    return 1 + (pmd_position)(((pos + 1.0f)/2.0f) * 0x3fe); 
}


/**
 * @brief represent object size
 *
 * The size indicates how 'wide' the field of an object might be
 * (i.e., it determines whether it needs to be rendered across
 * multiple speaker positions, and if so how many).
 *
 * Range: 0 - 31,
 *    where 0 means 'point source'
 *    and 31 means entire field.
 */
typedef unsigned short pmd_size;


/**
* @brief helper function to validate size
*/
static inline
dlb_pmd_payload_status
pmd_validate_size
    (float size
    )
{
    dlb_pmd_payload_status status = DLB_PMD_PAYLOAD_STATUS_OK;

    if (size < PMD_OBJECT_SIZE_MIN || size > PMD_OBJECT_SIZE_MAX)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE;
    }

    return status;
}


/**
 * @brief helper function to convert between floating pt and encoded position
 */
static inline
float
pmd_decode_size
    (pmd_size size
    )
{
    return (float)((double)size / 31.0);
}


/**
 * @brief helper function to convert between floating pt and encoded position
 */
static inline
pmd_size
pmd_encode_size
    (float size
    )
{
    return (pmd_size)((size * 31)+0.5);
}


/**
 * @brief type of element identifiers
 *
 * In PMD, elements are identified by an integer in the range 1 - 4096
 *
 * This type documents where the code base requires object
 * identifiers.
 */
typedef uint16_t pmd_element_id;


/**
 * @brief kinds of audio object
 *
 * backwards compatibility retrofitting new api 
 */
typedef dlb_pmd_object_class pmd_object_class;
    

/**
 * @brief gain of object
 *
 * The gain of an object controls whether its signal is amplified
 * or attenuated when it is rendered.
 *
 * Allowed range: 0 - 0x3f, in steps of 0.5 dB, where
 *   0x00 =  -inf dB
 *   0x01 = -25.0 dB
 *   0x33 =   0.0 dB
 *   0x3F =   6.0 dB
 */
typedef unsigned short pmd_gain;


/**
 * @brief convert dB value to PMD gain value
 */
static inline
pmd_gain             /** @return PMD gain representation */
pmd_db_to_gain
    (float db        /**< floating point gain in dB */
    )
{
    if (isinf(db) && db < 0)
    {
        return 0;
    }
    return (pmd_gain)(((db + 25.5f) / 0.5f));
}


/**
 * @brief convert PMD gain to dB value
 */
static inline
float
pmd_gain_to_db
    (pmd_gain gain
    )
{
    if (0 == gain) return -INFINITY;
    return (gain - 0x33) * 0.5f;
}


/**
 * @brief helper function to validate "source" (track)
 */
static inline
dlb_pmd_payload_status
pmd_validate_source
    (uint32_t src
    )
{
    dlb_pmd_payload_status status = DLB_PMD_PAYLOAD_STATUS_OK;

    if (src < DLB_PMD_MIN_SIGNAL_ID)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_RESERVED;
    }
    else if (src > DLB_PMD_MAX_SIGNAL_ID)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE;
    }

    return status;
}


/**
 * @brief audio object 'object-mode' metadata
 *
 * Note - divergence only makes sense for tracks that contain dialog.
 * Divergence divides the power of the track by two and duplicates it
 * in the horizontal mirror position.
 *
 * Similarly, size and vertical size make little sense for dialog.
 */
typedef struct
{
    pmd_position x;            /**< left-right position of object */
    pmd_position y;            /**< front-back position of object */
    pmd_position z;            /**< top-bottom position of object */

    pmd_object_class oclass;   /**< class of object */
    pmd_size size;             /**< size of object field */
    pmd_bool size_vertical;    /**< is size 2d (horizontal circle) or 3d (sphere)?*/
    pmd_bool diverge;          /**< spread object field accross to horiz mirror position? */
    pmd_bool dynamic_updates;  /**< does this object update dynamically? */
    pmd_gain gain;             /**< attenuation or amplification of object */
    pmd_track_index source;    /**< source audio signal, for now limited to 1, but could expand:
                                    it may be required to cluster multiple objects together
                               */
} pmd_object_metadata;
    

/**
 * @brief metadata for single track of audio channel object
 *
 * An Audio 'Channel Object' can consist of multiple tracks, each
 * of which can have distinct gain.  This structure encapsulates
 * indvidiual metadata
 */
typedef struct
{
    uint8_t         source;   /**< AudioSignal identifier */
    pmd_speaker     target;   /**< target output speaker for source track */
    pmd_gain        gain;     /**< attenuation or amplification of channel */
} pmd_track_metadata;


/**
 * @brief audio channel metadata
 */
typedef struct
{
#define MAX_PMD_CHANNEL_METADATA (DLB_PMD_MAX_BED_SOURCES)

    dlb_pmd_speaker_config config;    /**< container speaker config */
    pmd_bool derived;                 /**< 0 means original, 1 means derived
                                       *   derived channels are the output of a rendering
                                       *   step, not original input channels */
    pmd_element_id origin;            /**< if derived, id of original bed element id */
    uint8_t num_tracks;               /**< number of audio tracks */
    pmd_track_metadata metadata[MAX_PMD_CHANNEL_METADATA]; /**< metadata for each component track */
    
} pmd_channel_metadata;


/**
 * @brief intention of individual audio element
 */
typedef enum
{
    PMD_MODE_CHANNEL,      /**< audio element is a channel */
    PMD_MODE_OBJECT,       /**< audio element is an object */
    PMD_MODE_HOA,          /**< audio element is Higher-Order Ambisonics */

    PMD_MODE_UNDEFINED
} pmd_element_mode;


#define PMD_MIN_OBJ_ID (1)
#define PMD_MAX_OBJ_ID (4095)


/**
 * @brief metadata associated with an audio element
 *
 * Some examples of Audio Element Metadata include positional metadata
 * (spatial information describing the position of objects in the
 * reproduction space, which may dynamically change over time, or
 * channel assignments), or personalization metadata (set by content
 * creator to enable certain personalization options such as turning
 * an element “on” or “off,” adjusting its position or gain, and
 * setting limits within which such adjustments may be made by the
 * user).
 */
typedef struct
{
    pmd_element_id id;                  /**< AudioObject identifier */
    pmd_element_mode mode;              /**< audio element mode */
    uint16_t hed_idx;                   /**< headphone element index, if it exists (or 0xffff)*/
    union
    {
        pmd_object_metadata object;     /**< object-based audio element metadata */
        pmd_channel_metadata channel;   /**< channel-based audio element metadata */
    } md;

} pmd_element;



/**
 * @brief compare two track metadata, used to sort bed track metadata
 */
static 
int                    /** @return < 0 if a should be ordered before b
                        *          0 if a and b are equal
                        *          > 0 if b should be ordered before a
                        */
compare_tracks
    (const void* a
    ,const void* b
    )
{
    const pmd_track_metadata *ta = (const pmd_track_metadata *)a;
    const pmd_track_metadata *tb = (const pmd_track_metadata *)b;

    if (ta->target < tb->target) return -1;
    if (ta->target > tb->target) return 1;
    if (ta->source < tb->source) return -1;
    if (ta->source > tb->source) return 1;
    if (ta->gain < tb->gain) return -1;
    if (ta->gain > tb->gain) return 1;
    return 0;
}


/**
 * @brief helper function to arrange a bed's channel metadata in normal form
 *
 * Normalizing the order of bed metadata means that operations like equality
 * checking are straightforward.
 */
static inline
void
pmd_bed_set_normal_form
    (pmd_channel_metadata *md
    )
{
    qsort(md->metadata, md->num_tracks, sizeof(pmd_track_metadata), compare_tracks);
}


#endif /* PMD_ABD_AOD_H */
