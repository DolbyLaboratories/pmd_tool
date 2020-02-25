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

/**
 * @file dlb_pmd_types.h
 * @brief data structures
 *
 * This file contains the data structures used by the PMD API.
 */

#ifndef DLB_PMD_TYPES_H
#define DLB_PMD_TYPES_H

#include <math.h>
#if defined(_MSC_VER) && !defined(INFINITY)
#  include <float.h>
#  define INFINITY (-logf(0.0f))
#  define isinf(x) (!_finite(x))
#endif

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @def DLB_PMD_TYPES_VERSION
 * @brief version information for this header file
 */
#define DLB_PMD_TYPES_VERSION "2.2"


/**
 * @def DLB_PMD_MAX_SIGNALS
 * @brief maximum number of input audio signals (where each audio signal
 * is one PCM channel)
 */
#define DLB_PMD_MAX_SIGNALS (255)


/**
 * @def DLB_PMD_MAX_ED2_STREAMS
 * @brief maximum number of supported ED2 streams
 */
#define DLB_PMD_MAX_ED2_STREAMS (16)


/**
 * @def DLB_PMD_MAX_AUDIO_ELEMENTS
 * @brief upper limit on the number of audio elements we can hold
 */
#define DLB_PMD_MAX_AUDIO_ELEMENTS (4095)


/**
 * @def DLB_PMD_MAX_BED_SOURCES
 * @brief upper limit on the number of source channels that can be mixed
 * for a target speaker
 */
#define DLB_PMD_MAX_BED_SOURCES (128)


/**
 * @def DLB_PMD_MAX_UPDATES
 * @brief upper limit on the number of updates we can hold
 */
#define DLB_PMD_MAX_UPDATES (2048)


/**
 * @def DLB_PMD_MAX_PRESENTATIONS
 * @brief upper limit on the number of audio presentations we can hold
 */
#define DLB_PMD_MAX_PRESENTATIONS (511)


/**
 * @def DLB_PMD_MAX_PRESENTATION_NAMES
 * @brief upper limit on the number of different names a presentation may have
 */
#define DLB_PMD_MAX_PRESENTATION_NAMES (16)


/**
 * @def DLB_PMD_MAX_NAME_LENGTH
 * @brief max length of a name string in bytes, not including NUL termination
 * @note NOT the character count, multi-byte Unicode characters are allowed
 */
#define DLB_PMD_MAX_NAME_LENGTH (67)


/**
 * @def DLB_PMD_NAME_ARRAY_SIZE
 * @brief number of bytes needed to store a name string, including NUL termination
 */
#define DLB_PMD_NAME_ARRAY_SIZE (DLB_PMD_MAX_NAME_LENGTH + 1)


/**
 * @def DLB_PMD_MAX_PRESENTATION_ELEMENTS
 * @brief upper limit on number of audio elements in one presentation
 */
#define DLB_PMD_MAX_PRESENTATION_ELEMENTS (128)


/**
 * @def DLB_PMD_MAX_EAC3_ENCODING_PARAMETERS
 * @brief upper limit up EAC3 encoding parameters structs within single video frame
 */
#define DLB_PMD_MAX_EAC3_ENCODING_PARAMETERS (255)


/**
 * @def DLB_PMD_MAX_ED2_TURNAROUNDS
 * @brief upper limit up ED2 turnaround descriptions within single video frame
 */
#define DLB_PMD_MAX_ED2_TURNAROUNDS (255)


/**
 * @def DLB_PMD_MAX_HEADPHONE
 * @brief upper limit of Headphone element descriptions
 */
#define DLB_PMD_MAX_HEADPHONE (255)


/**
 * @def DLB_PMD_TITLE_SIZE
 * @brief max text size of metadata title
 */
#define DLB_PMD_TITLE_SIZE (64)


/**
 * @brief simple boolean type
 */
typedef uint8_t dlb_pmd_bool;
enum
{
    PMD_FALSE,
    PMD_TRUE
};


/**
 * @brief signal ID
 *
 * A 'signal' is just a single mono PCM track, delivered alongside the PMD
 * metadata. 1 is the first channel.
 */
typedef uint8_t dlb_pmd_signal;


/**
* @def DLB_PMD_MIN_SIGNAL_ID
* @brief smallest acceptable signal id
*/
#define DLB_PMD_MIN_SIGNAL_ID (1)


/**
 * @def DLB_PMD_MAX_SIGNAL_ID
 * @brief largest acceptable signal id
 */
#define DLB_PMD_MAX_SIGNAL_ID (255)


/**
 * @brief channel enum
 */
typedef enum
{
    PMD_SPEAKER_NULL,       /**< "End-of-channels" reserved value */

    PMD_SPEAKER_L,          /**< Left                  */
    PMD_SPEAKER_R,          /**< Right                 */
    PMD_SPEAKER_C,          /**< Center                */
    PMD_SPEAKER_LFE,        /**< Low Frequency Effects */
    PMD_SPEAKER_LS,         /**< Left Surround         */
    PMD_SPEAKER_RS,         /**< Right Surround        */
    PMD_SPEAKER_LRS,        /**< Left Rear Surround    */
    PMD_SPEAKER_RRS,        /**< Right Rear Surround   */

    PMD_SPEAKER_LTF,        /**< Left Top Front        */
    PMD_SPEAKER_RTF,        /**< Right Top Front       */
    PMD_SPEAKER_LTM,        /**< Left Top Middle       */
    PMD_SPEAKER_RTM,        /**< Right Top Middle      */
    PMD_SPEAKER_LTR,        /**< Left Top Rear         */
    PMD_SPEAKER_RTR,        /**< Right Top Rear        */

    PMD_SPEAKER_LFW,        /**< Left Front Wide       */
    PMD_SPEAKER_RFW,        /**< Right Front Wide      */

    PMD_SPEAKER_LAST_VALID = PMD_SPEAKER_RFW,

    PMD_NUM_SPEAKERS,       /**< Number of valid speaker position values, plus 1 for the zero reserved value */

    PMD_SPEAKER_RESERVED_FIRST = PMD_SPEAKER_LAST_VALID + 1,
    PMD_SPEAKER_RESERVED_LAST = 0x3f

} dlb_pmd_speaker;


/**
 * @brief list of known speaker configurations
 */
typedef enum
{
    DLB_PMD_SPEAKER_CONFIG_2_0,       /**< L, R                                               */
    DLB_PMD_SPEAKER_CONFIG_3_0,       /**< L, R, C                                            */
    DLB_PMD_SPEAKER_CONFIG_5_1,       /**< L, R, C, Lfe, Ls, Rs                               */
    DLB_PMD_SPEAKER_CONFIG_5_1_2,     /**< L, R, C, Lfe, Ls, Rs, Ltm, Rtm                     */
    DLB_PMD_SPEAKER_CONFIG_5_1_4,     /**< L, R, C, Lfe, Ls, Rs, Ltf, Rtf, Ltr, Rtr           */

    DLB_PMD_SPEAKER_CONFIG_7_1_4,     /**< L, R, C, Lfe, Ls, Rs, Lrs, Rrs, Ltf, Rtf, Ltr, Rtr */
    DLB_PMD_SPEAKER_CONFIG_9_1_6,     /**< L, R, C, Lfe, Ls, Rs, Lrs, Rrs, Lfw, Rfw,
                                       *                         Ltf, Rtf, Ltm, Rtm, Ltr, Rtr */

    DLB_PMD_SPEAKER_CONFIG_PORTABLE,  /**< L, R, portable speakers   */
    DLB_PMD_SPEAKER_CONFIG_HEADPHONE, /**< L, R, portable headphones */

    DLB_PMD_SPEAKER_CONFIG_LAST = DLB_PMD_SPEAKER_CONFIG_HEADPHONE,
    NUM_PMD_SPEAKER_CONFIGS

} dlb_pmd_speaker_config;


/**
 * @brief enumerate supported video frame rates
 */
typedef enum
{
    DLB_PMD_FRAMERATE_2398,  /**<  23.98 fps */
    DLB_PMD_FRAMERATE_2400,  /**<  24    fps */
    DLB_PMD_FRAMERATE_2500,  /**<  25    fps */
    DLB_PMD_FRAMERATE_2997,  /**<  29.97 fps */
    DLB_PMD_FRAMERATE_3000,  /**<  30    fps */
    DLB_PMD_FRAMERATE_LAST_ED2 = DLB_PMD_FRAMERATE_3000,
    DLB_PMD_FRAMERATE_5000,  /**<  50    fps */
    DLB_PMD_FRAMERATE_5994,  /**<  59.94 fps */
    DLB_PMD_FRAMERATE_6000,  /**<  60    fps */
    DLB_PMD_FRAMERATE_10000, /**< 100    fps */
    DLB_PMD_FRAMERATE_11988, /**< 119.88 fps */
    DLB_PMD_FRAMERATE_12000, /**< 120    fps */
    DLB_PMD_FRAMERATE_LAST = DLB_PMD_FRAMERATE_12000,
    NUM_PMD_FRAMERATES
} dlb_pmd_frame_rate;


/**
 * @brief gain of object
 *
 * The gain of an object controls whether its signal is amplified
 * or attenuated when it is rendered.
 *
 * Allowed range: -inf, -25.0 to 6.0 in dB
 */
typedef float dlb_pmd_gain;

#define PMD_MIN_FINITE_GAIN_DB (-25.0f)

#define PMD_MAX_GAIN_DB (6.0f)


/**
 * @brief object spatial coordinate
 *
 * An object position is specified in the range -1.0 to 1.0.
 * For x-coordinate, -1.0 means 'left' and 1.0 means 'right'.
 * For y-coordinate, -1.0 means 'back' and 1.0 means 'front'.
 * For z-coordinate, -1.0 means 'floor',
 *                    0.0 means 'horizon' and
 *                    1.0 means 'ceiling'
 */
typedef float dlb_pmd_coordinate;


/**
 * @brief object size
 *
 * The size indicates how 'wide' the field of an object might be
 * (i.e., it determines whether it needs to be rendered across
 * multiple speaker positions, and if so how many).
 *
 * 0.0 (point) - 1.0 (entire field)
 */
typedef float dlb_pmd_size;


/**
 * @brief enumerate the entire range of possible DE program configs
 */
typedef enum
{
    PMD_DE_PGMCFG_51_2            = 0,  /**< 5.1+2           */
    PMD_DE_PGMCFG_51_1_1          = 1,  /**< 5.1+1+1         */
    PMD_DE_PGMCFG_4_4             = 2,  /**< 4+4             */
    PMD_DE_PGMCFG_4_2_2           = 3,  /**< 4+2+2           */
    PMD_DE_PGMCFG_4_2_1_1         = 4,  /**< 4+2+1+1         */
    PMD_DE_PGMCFG_4_1_1_1_1       = 5,  /**< 4+1+1+1+1       */
    PMD_DE_PGMCFG_2_2_2_2         = 6,  /**< 2+2+2+2         */
    PMD_DE_PGMCFG_2_2_2_1_1       = 7,  /**< 2+2+2+1+1       */
    PMD_DE_PGMCFG_2_2_1_1_1_1     = 8,  /**< 2+2+1+1+1+1     */
    PMD_DE_PGMCFG_2_1_1_1_1_1_1   = 9,  /**< 2+1+1+1+1+1+1   */
    PMD_DE_PGMCFG_1_1_1_1_1_1_1_1 = 10, /**< 1+1+1+1+1+1+1+1 */
    PMD_DE_PGMCFG_51              = 11, /**< 5.1             */
    PMD_DE_PGMCFG_4_2             = 12, /**< 4+2             */
    PMD_DE_PGMCFG_4_1_1           = 13, /**< 4+1+1           */
    PMD_DE_PGMCFG_2_2_2           = 14, /**< 2+2+2           */
    PMD_DE_PGMCFG_2_2_1_1         = 15, /**< 2+2+1+1         */
    PMD_DE_PGMCFG_2_1_1_1_1       = 16, /**< 2+1+1+1+1       */
    PMD_DE_PGMCFG_1_1_1_1_1_1     = 17, /**< 1+1+1+1+1+1     */
    PMD_DE_PGMCFG_4               = 18, /**< 4               */
    PMD_DE_PGMCFG_2_2             = 19, /**< 2+2             */
    PMD_DE_PGMCFG_2_1_1           = 20, /**< 2+1+1           */
    PMD_DE_PGMCFG_1_1_1_1         = 21, /**< 1+1+1+1         */
    PMD_DE_PGMCFG_71              = 22, /**< 7.1             */
    PMD_DE_PGMCFG_71S             = 23, /**< 7.1 screen      */
    PMD_DE_PGMCFG_LAST            = PMD_DE_PGMCFG_71S
} dlb_pmd_de_program_config;


/**
 * @brief DE bit allocation reduction percentage
 *
 * To free up space for metadata, the DE portion of the ED2 stream is
 * compressed more than for legacy DE.  We measure the increase in
 * compression as the percentage decrease in output bitcount.
 */
typedef enum
{
    PMD_DE_COMPRESSION_97_5 = 1, /**< ED2 audio is 97.5% the size of normal DE */
    PMD_DE_COMPRESSION_95_0 = 2, /**< ED2 audio is 95.0% the size of normal DE */
    PMD_DE_COMPRESSION_92_5 = 3, /**< ED2 audio is 92.5% the size of normal DE */
    PMD_DE_COMPRESSION_90_0 = 4, /**< ED2 audio is 90.0% the size of normal DE */
    PMD_DE_COMPRESSION_87_5 = 5, /**< ED2 audio is 87.5% the size of normal DE */
    PMD_DE_COMPRESSION_85_0 = 6, /**< ED2 audio is 85.0% the size of normal DE */
    PMD_DE_COMPRESSION_82_5 = 7  /**< ED2 audio is 82.5% the size of normal DE */
} dlb_pmd_de_compression;


/**
 * @brief type of element identifiers
 *
 * In PMD, elements are identified by an integer in the range 1 to 4095
 *
 * This type documents where the code base requires object
 * identifiers.
 */
typedef uint16_t dlb_pmd_element_id;

/**
* @def DLB_PMD_RESERVED_ELEMENT_ID
* @brief reserved element id
*/
#define DLB_PMD_RESERVED_ELEMENT_ID (0)

/**
* @def DLB_PMD_MIN_ELEMENT_ID
* @brief smallest acceptable element id
*/
#define DLB_PMD_MIN_ELEMENT_ID (1)

/**
 * @def DLB_PMD_MAX_ELEMENT_ID
 * @brief largest acceptable element id
 */
#define DLB_PMD_MAX_ELEMENT_ID (4095)


/** -----------------  Audio Bed Descriptions (ABD) -------------------- */


/**
 * @brief element names
 *
 * Unlike presentations, which can have multiple different names for
 * different languages, audio elements may have only one name. It is
 * expected that presentation names will be used by broadcasters when
 * emitting content. It is not expected that element names will be
 * emitted.
 */
typedef char dlb_pmd_element_name[DLB_PMD_NAME_ARRAY_SIZE];


/**
 * @brief beds can be original or derived
 *
 * In PMD, a bed is a collection of speaker feeds for a particular
 * output speaker configuration, e.g., stereo, 5.1, 7.1.4, etc. A simple
 * bed description may simply list those channels in the incoming PCM
 * that map to the speakers directly. These are called _original_
 * beds.  PMD also allows beds to be built out of other, previously
 * described, beds.  These are called _derived_ beds.
 */
typedef enum
{
    PMD_BED_ORIGINAL, /**< bed constructed directly from incoming PCM */
    PMD_BED_DERIVED   /**< bed is constructed from result of a previous rendering */
} dlb_pmd_bed_type;


/**
 * @brief metadata for single track of audio channel object
 *
 * An Audio 'Channel Object' (or 'bed') can be constructed out of
 * multiple incoming PCM source channels, or tracks, each of which can
 * have a distinct gain, Multiple incoming channels can contribute to
 * different output speakers.  This structure lists how one incoming
 * source channel is used to help construct the bed channel for the
 * given speaker.
 */
typedef struct dlb_pmd_source
{
    dlb_pmd_speaker target; /**< target output speaker for this bed channel */
    dlb_pmd_signal  source; /**< location of PCM channel in incoming stream */
    dlb_pmd_gain    gain;   /**< attenuation or amplification of source     */
} dlb_pmd_source;


/**
 * @brief desciption of a single Audio Bed
 *
 * A bed is a collection of channels that are assigned to well-known
 * speaker positions. A bed may be:
 *
 *  - Direct-Mapped, where the bed is simply a 1:1 mapping between
 *    audio signals and speakers.
 *
 *  - Mix-Mapped, where each target speaker may be the result of
 *    mixing a selection of source audio signals, where each signal
 *    has a specific gain to control how much it contributes to the
 *    final mix.
 *
 *    For instance, a custom downmix can be implemented via a mix-map.
 *
 *  - Derived, where the bed is constructed from the speaker outputs
 *    of another bed, rather than the original audio signals.
 */
typedef struct dlb_pmd_bed
{
    dlb_pmd_element_id      id;          /**< PMD element being described                */
    dlb_pmd_speaker_config  config;      /**< target speaker configuration               */
    dlb_pmd_bed_type        bed_type;    /**< original bed, or derived from another?     */
    dlb_pmd_element_id      source_id;   /**< if derived, the source bed id derived from */
    uint8_t                 num_sources; /**< number of input source mappings used       */
    dlb_pmd_source         *sources;     /**< list of source channel mappings            */
    dlb_pmd_element_name    name;        /**< element name                               */
} dlb_pmd_bed;


/** -----------------  Audio Object Descriptions (AOD) -------------------- */


/**
 * @brief enumeration of the classes of audio object
 */
typedef enum
{
    PMD_CLASS_DIALOG,          /**< dialog audio object             */
    PMD_CLASS_VDS,             /**< VDS (Video Description Service) */
    PMD_CLASS_VOICEOVER,       /**< Voiceover track                 */
    PMD_CLASS_GENERIC,         /**< ordinary audio object           */
    PMD_CLASS_SUBTITLE,        /**< spoken subtitles                */
    PMD_CLASS_EMERGENCY_ALERT, /**< emergency alert                 */
    PMD_CLASS_EMERGENCY_INFO,  /**< emergency information           */
    PMD_CLASS_RESERVED         /**< reserved for future use         */
} dlb_pmd_object_class;


/**
 * @brief describe an individual audio object
 *
 * An audio object is a single mono track of audio that represents
 * the sound of a noise-emitting entity
 */
typedef struct dlb_pmd_object
{
    dlb_pmd_element_id   id;              /**< unique identifier                            */
    dlb_pmd_object_class object_class;    /**< object class                                 */
    dlb_pmd_bool         dynamic_updates; /**< does it change?                              */
    dlb_pmd_coordinate   x;               /**< x coordinate of location in perceptual space */
    dlb_pmd_coordinate   y;               /**< y coordinate of location in perceptual space */
    dlb_pmd_coordinate   z;               /**< z coordinate of location in perceptual space */
    dlb_pmd_size         size;            /**< perceived size of object in perceptual field */
    dlb_pmd_bool         size_3d;         /**< is object flat or spherical?                 */
    dlb_pmd_bool         diverge;         /**< spread object energy across fronts?          */
    dlb_pmd_signal       source;          /**< source PCM track                             */
    dlb_pmd_gain         source_gain;     /**< gain to apply to source track                */
    dlb_pmd_element_name name;            /**< element name                                 */
} dlb_pmd_object;


/** -----------------  Audio Presentation Descriptions (APD) -------------------- */

/**
 * @brief type of PMD presentation identifier
 *
 * Like PMD audio elements, PMD presentations are identified using an
 * integer, in the range 1 to 511.
 */
typedef uint16_t dlb_pmd_presentation_id;


/**
* @def DLB_PMD_RESERVED_PRESENTATION_ID
* @brief reserved value for presentation id
*/
#define DLB_PMD_RESERVED_PRESENTATION_ID (0)


/**
* @def DLB_PMD_MIN_PRESENTATION_ID
* @brief smallest acceptable presentation id
*/
#define DLB_PMD_MIN_PRESENTATION_ID (1)


/**
 * @def DLB_PMD_MAX_PRESENTATION_ID
 * @brief largest acceptable presentation id
 */
#define DLB_PMD_MAX_PRESENTATION_ID (511)


/** -----------------  names  -------------------- */


/**
 * @brief encapsulate human-readable naming metadata
 *
 * PMD may transmit human-readable names for ED2 streams, for elements
 * and for presentations, and these may come in a variety of languages.
 */
typedef struct dlb_pmd_presentation_name
{
    char language[4];                   /**< ISO-639-1/2 language of name */
    char text[DLB_PMD_NAME_ARRAY_SIZE]; /**< name text (Unicode)          */
} dlb_pmd_presentation_name;


/**
 * @brief array of presentation names
 */
typedef dlb_pmd_presentation_name dlb_pmd_presentation_names[DLB_PMD_MAX_PRESENTATION_NAMES];


/**
 * @brief describe a single audio presentation
 *
 * A presentation encapsulates a particular user experience
 */
typedef struct dlb_pmd_presentation
{
    dlb_pmd_presentation_id     id;                /**< presentation identifier                    */
    dlb_pmd_speaker_config      config;            /**< presentation output speaker configuration  */
    char                        audio_language[4]; /**< ISO 639-1/2 language of audio              */
    unsigned int                num_elements;      /**< number of elements used in presentation    */
    dlb_pmd_element_id         *elements;          /**< array of element identifiers used          */
    unsigned int                num_names;         /**< number of names associated to presentation */
    dlb_pmd_presentation_names  names;             /**< list of names                              */
} dlb_pmd_presentation;


/** -----------------  Presentation Loudness Descriptions (PLD) -------------------- */


/**
 * @brief loudness value
 *
 * This type is used to record 5 different loudness measurements
 * (in LUFS): relative-gated, speech-gated, short-term-3s, true-peak
 * and momentary loudness.
 *
 * - Relative-Gated measurements determine the integrated loudness of
 *   an audio programme, measured according to Recommendation ITU-R
 *   BS.1770, without any gain adjustments due to dialnorm or DRC.
 *
 * - Speech-Gated measurements determine the integrated dialog-based
 *   loudness of the entire audio programme, measured according to
 *   formula (2) of Recommendation ITU-R BS.1770 with dialog-gating.
 *   This represents the dialog-based loudness without gain
 *   adjustments due to dialnorm or DRC.
 *
 * - Short-Term-3s measurements determine the loudness of the
 *   preceding 3 seconds of the audio programme, measured according to
 *   ITU-R BS.1771, without gain adjustments due to dialnorm or DRC.
 *
 * - True peak is measured according to Annex 2 of Recommendation
 *   ITU-R BS.1770.
 *
 * - momentary loudness measurements, measured according to
 *   Recommendation ITU-R BS.1771, without any gain adjustments due to
 *   dialnorm or DRC.
 *
 * The unit of loudness measurement, LUFS, stands for "Loudness Units
 * Full Scale".  This is a synonym for the other unit LKFS "Loudness,
 * K-weighted, relative to full scale.  LKFS is standardized in ITU-R
 * BS.1770, and LUFS in EBU R128.
 *
 * Both LUFS and LKFS are both measured in absolute scale and both are
 * equal to one decibel. In PMD, types of this value are expected to
 * be in the range -102.4 to 102.3 inclusive.
 */
typedef float dlb_pmd_lufs;

#define DLB_PMD_LUFS_MIN (-102.4f)
#define DLB_PMD_LUFS_MAX (102.3f)


/**
 * @brief loudness units
 *
 * indicates the loudness range of the programme, as specified in
 * EBU Tech Document 3342.
 *
 * LU, loudness units, are equal to one decibel. Unlike LUFS, these
 * are used for relative measures, i.e., the difference between two
 * loudness values. In PMD, these are used to specify the loudness
 * range of a programme.
 *
 * This takes values 0.0f - 102.3f
 */
typedef float dlb_pmd_lu;

#define DLB_PMD_LU_MIN (0.0f)
#define DLB_PMD_LU_MAX (102.3f)

/**
 * @brief enumerate loudness practice types
 *
 * Loudness Practice types describe how loudness measurements were
 * made.
 */
typedef enum
{
    PMD_PLD_LOUDNESS_PRACTICE_NOT_INDICATED,    /**< Loudness regulation compliance not indicated */
    PMD_PLD_LOUDNESS_PRACTICE_ATSC_A_85,        /**< Loudness according to ATSC A/85              */
    PMD_PLD_LOUDNESS_PRACTICE_EBU_R128,         /**< Loudness according to EBU R126               */
    PMD_PLD_LOUDNESS_PRACTICE_ARIB_TR_B32,      /**< Loudness according to ARIB TR-B32            */
    PMD_PLD_LOUDNESS_PRACTICE_FREETV_OP_59,     /**< Loudness according to Free TV OP-59          */
    PMD_PLD_LOUDNESS_PRACTICE_RESERVED_05,      /**< reserved for future use                      */
    PMD_PLD_LOUDNESS_PRACTICE_RESERVED_06,      /**< reserved for future use                      */
    PMD_PLD_LOUDNESS_PRACTICE_RESERVED_07,      /**< reserved for future use                      */
    PMD_PLD_LOUDNESS_PRACTICE_RESERVED_08,      /**< reserved for future use                      */
    PMD_PLD_LOUDNESS_PRACTICE_RESERVED_09,      /**< reserved for future use                      */
    PMD_PLD_LOUDNESS_PRACTICE_RESERVED_10,      /**< reserved for future use                      */
    PMD_PLD_LOUDNESS_PRACTICE_RESERVED_11,      /**< reserved for future use                      */
    PMD_PLD_LOUDNESS_PRACTICE_RESERVED_12,      /**< reserved for future use                      */
    PMD_PLD_LOUDNESS_PRACTICE_RESERVED_13,      /**< reserved for future use                      */
    PMD_PLD_LOUDNESS_PRACTICE_MANUAL,           /**< Manual measurement                           */
    PMD_PLD_LOUDNESS_PRACTICE_CONSUMER_LEVELLER /**< consumer leveller                            */
} dlb_pmd_loudness_practice;


/**
 * @brief enumerate known dialog gating practice methods
 */
typedef enum
{
    PMD_PLD_GATING_PRACTICE_NOT_INDICATED,          /**< dialog gating method not indicated                       */
    PMD_PLD_GATING_PRACTICE_AUTOMATED_C_OR_L_R,     /**< dialog gating applied to center channel, or LR in stereo */
    PMD_PLD_GATING_PRACTICE_AUTOMATED_L_C_AND_OR_R, /**< dialog gating applied to all front (main) channels       */
    PMD_PLD_GATING_PRACTICE_MANUAL,                 /**< dialog gating method - manual                            */
    PMD_PLD_GATING_PRACTICE_RESERVED_04,            /**< reserved                                                 */
    PMD_PLD_GATING_PRACTICE_RESERVED_05,            /**< reserved                                                 */
    PMD_PLD_GATING_PRACTICE_RESERVED_06,            /**< reserved                                                 */
    PMD_PLD_GATING_PRACTICE_RESERVED_07             /**< reserved                                                 */
} dlb_pmd_dialgate_practice;


/**
 * @brief loudness correction type
 *
 * This is used to indicate how loudness has been corrected upstream,
 * if at all. (If it has already been corrected, re-correcting it
 * would lead to muddy audio, and should be avoided.)
 */
typedef enum
{
    PMD_PLD_CORRECTION_FILE_BASED, /**< corrected with infinite lookahead (file-based) */
    PMD_PLD_CORRECTION_REALTIME    /**< corrected with finite lookahead (realtime)     */
} dlb_pmd_correction_type;


/**
 * @brief indicates method used to compute loudness range
 */
typedef enum
{
    PMD_PLD_RANGE_PRACTICE_EBU_3342_V1, /**< Loudness Range as per EBU Tech 3342 v1 */
    PMD_PLD_RANGE_PRACTICE_EBU_3342_V2, /**< Loudness Range as per EBU Tech 3342 v2 */
    PMD_PLD_RANGE_PRACTICE_RESERVED_02, /**< reserved                               */
    PMD_PLD_RANGE_PRACTICE_RESERVED_03, /**< reserved                               */
    PMD_PLD_RANGE_PRACTICE_RESERVED_04, /**< reserved                               */
    PMD_PLD_RANGE_PRACTICE_RESERVED_05, /**< reserved                               */
    PMD_PLD_RANGE_PRACTICE_RESERVED_06, /**< reserved                               */
    PMD_PLD_RANGE_PRACTICE_RESERVED_07  /**< reserved                               */
} dlb_pmd_loudness_range_practice;


/**
 * @brief program boundary
 *
 * Indicates the base-2 log of the number of frames between the
 * current frame and the next frame that contains the boundary between
 * two different audio programmes.  This data may be used to determine
 * when to begin and end the measurement of the loudness parameters
 * specified in the payload.  This value is restricted to be the range
 * +/-[1,9] (i.e., +/-[2,4,8,16,32,64,128,256,512]).
 *
 * If negative, this indicates number of frames since previous program boundary.
 * If positive, this indicates number of frames until next program boundary.
 */
typedef signed short dlb_pmd_progbound;


/**
 * @brief offset in samples to the next boundary
 */
typedef unsigned int dlb_pmd_progbound_offset;


/**
 * @brief encapsulate the IAT user data field
 *
 * @note that this is an optional field. If not present then the
 * #size field will be 0.
 */
#define PMD_USER_DATA_MAX_BYTES (256)
typedef struct dlb_pmd_user_data
{
    size_t  size;                          /**< size of user data, 0 if not present */
    uint8_t data[PMD_USER_DATA_MAX_BYTES]; /**< user data                           */
} dlb_pmd_user_data;


/**
 * @brief encapsulate the Loudness/IAT extension field
 *
 * @note that this is an optional field. If not present then the
 * #size field will be 0.
 */
#define PMD_EXTENSION_MAX_BYTES (256)
typedef struct dlb_pmd_extension
{
    size_t  size;                          /**< size of extension in bits, 0 if not present */
    uint8_t data[PMD_EXTENSION_MAX_BYTES]; /**< extension data                              */
} dlb_pmd_extension;


/**
 * @brief implementation of PMD Presentation loudness data structure
 *
 * This structure is defined in ETSI TS 103 190-1 v1.2.1, section
 * 4.2.14.3
 */
typedef struct dlb_pmd_loudness
{
    dlb_pmd_presentation_id         presid;            /**< presentation identifier             */

    dlb_pmd_loudness_practice       loud_prac_type;    /**< how loudness measured               */
    dlb_pmd_bool                    b_loudcorr_gating; /**< dialog gated present?               */
    dlb_pmd_dialgate_practice       loudcorr_gating;   /**< where dialog gated                  */
    dlb_pmd_correction_type         loudcorr_type;     /**< if loudness corrected, how          */

    dlb_pmd_bool                    b_loudrelgat;      /**< relative-gated measurement present? */
    dlb_pmd_lufs                    loudrelgat;        /**< relative-gated loudness measure     */

    dlb_pmd_bool                    b_loudspchgat;     /**< speech-gated measurement present?   */
    dlb_pmd_lufs                    loudspchgat;       /**< speech-gated loudness measurement   */
    dlb_pmd_dialgate_practice       loudspch_gating;   /**< speech-gating loudness practice     */

    dlb_pmd_bool                    b_loudstrm3s;      /**< 3-second measurement present?       */
    dlb_pmd_lufs                    loudstrm3s;        /**< 3-second loudness measurement       */
    dlb_pmd_bool                    b_max_loudstrm3s;  /**< max 3-second measure present?       */
    dlb_pmd_lufs                    max_loudstrm3s;    /**< max 3-second loudness measurement   */

    dlb_pmd_bool                    b_truepk;          /**< true peak loudness measure present? */
    dlb_pmd_lufs                    truepk;            /**< true peak loudness measurement      */
    dlb_pmd_bool                    b_max_truepk;      /**< max true peak measurment present?   */
    dlb_pmd_lufs                    max_truepk;        /**< max true peak loudness measurement  */

    dlb_pmd_bool                    b_prgmbndy;        /**< program boundary present?           */
    dlb_pmd_progbound               prgmbndy;          /**< program boundary                    */
    dlb_pmd_bool                    b_prgmbndy_offset; /**< prog bndry sample offset present    */
    dlb_pmd_progbound_offset        prgmbndy_offset;   /**< program boundary sample offset      */

    dlb_pmd_bool                    b_lra;             /**< loudness range present?             */
    dlb_pmd_lu                      lra;               /**< loudness range measurement          */
    dlb_pmd_loudness_range_practice lra_prac_type;     /**< loudness range measurement type     */

    dlb_pmd_bool                    b_loudmntry;       /**< momentary loudness present?         */
    dlb_pmd_lufs                    loudmntry;         /**< momentary loudness measurment       */
    dlb_pmd_bool                    b_max_loudmntry;   /**< max momentary loudness present?     */
    dlb_pmd_lufs                    max_loudmntry;     /**< max momentary loudness measurement  */

    dlb_pmd_extension               extension;         /**< extended data                       */

} dlb_pmd_loudness;


/** -----------------  Object Update (XYZ)  -------------------- */

/**
 * @brief type of PMD audio object update
 *
 * In PMD, the audio objects and presentations are always delivered in
 * entirety at the start of a video frame boundary.  However, the
 * PMD resolution is much higher than frames, allowing object positional
 * updates within the duration of a video frame.
 *
 * This structure details one such positional update for one object.
 */
typedef struct dlb_pmd_update
{
    unsigned int sample_offset; /**< num samples after previous object description at which this
                                  *  update applies (note that PMD has a resolution of 32
                                  *  samples, so this value will be rounded to lowest multiple
                                  *  of 32 samples)          */
    dlb_pmd_element_id id;      /**< audio object identifier */
    dlb_pmd_coordinate x;       /**< new X position          */
    dlb_pmd_coordinate y;       /**< new Y position          */
    dlb_pmd_coordinate z;       /**< new Z position          */
} dlb_pmd_update;


/**
 * @def DLB_PMD_MAX_UPDATE_TIME
 * @brief maximum allowed value for sample_offset field in dlb_pmd_update
 *
 * See comment on sample_offset field of #dlb_pmd_update structure to
 * understand how PMD quantizes update time.
 */
#define DLB_PMD_MAX_UPDATE_TIME (2047)


/** -----------------  Identity and Timing (IAT)  -------------------- */


/**
 * @brief enumerate well-known IAT Content ID types
 *
 * Note that there may be other, not-currently-well-known values
 */
typedef enum
{
    PMD_IAT_CONTENT_ID_UUID  = 0x00, /**< 128-bit UUID as defined by IETF RFC 4122        */
    PMD_IAT_CONTENT_ID_EIDR  = 0x01, /**< 96-bit EIDR identifier in Compact Binary format */
    PMD_IAT_CONTENT_ID_AD_ID = 0x02  /**< Ad_ID string, defined by http://www.ad-id.org   */
} dlb_pmd_content_id_type;


/**
 * @brief encapsulate the IAT Content ID
 */
#define PMD_CONTENT_ID_MAX_BYTES (32)
typedef struct dlb_pmd_content_id
{
    dlb_pmd_content_id_type type;
    size_t                  size;
    uint8_t                 data[PMD_CONTENT_ID_MAX_BYTES];
} dlb_pmd_content_id;


/**
 * @brief enumerate well-known IAT Distribution ID types
 *
 * Currently only ATSC-3.0 is known, and in this case, the data must have
 * the following structure:
 *    BSID             - 16 bits
 *    reserved         -  4 bits
 *    major_channel_no - 10 bits
 *    minor_channel_no - 10 bits
 *
 * i.e, 40 bits.
 */
typedef enum
{
    PMD_IAT_DISTRIBUTION_ID_ATSC3 = 0x00  /**< ATSC 3.0 VP1 Channel ID (ATSC A/336) */
} dlb_pmd_distribution_id_type;


/**
 * @brief encapsulate the IAT distribution id
 */
#define PMD_DISTRIBUTION_ID_MAX_BYTES (16)
typedef struct dlb_pmd_distribution_id
{
    size_t                       size;                                /**< size of distribution id, 0 if not present */
    dlb_pmd_distribution_id_type type;                                /**< distribution id type                      */
    uint8_t                      data[PMD_DISTRIBUTION_ID_MAX_BYTES]; /**< distribution id bytes                     */
} dlb_pmd_distribution_id;


/**
 * @brief IAT timestamp
 *
 * The IAT timestamp is 35 bits long. It specifies the content's
 * internal timestamp for the position within the essence that
 * contains the PMD block containing the IAT payload instance.
 *
 * The unit of timestamp is 1/240,000 seconds.
 */
typedef uint64_t dlb_pmd_timestamp;


/**
 * @brief IAT validity duration
 *
 * The validity duration specifies how many samples in the future
 * correspond to this IAT message (e.g., before the programme changes,
 * or something like an Advertising break occurs). The samples are
 * measured relative to the first sample in the essence associated
 * with the PMD block that contains this payload)
 *
 * This is an 11-bit number.
 */
typedef struct dlb_pmd_validity_duration
{
    dlb_pmd_bool present; /**< PMD_TRUE if valid (present in bitstream), PMD_FALSE otherwise */
    uint16_t     vdur;    /**< validity duration if valid */
} dlb_pmd_validity_duration;


/**
 * @brief IAT offset
 */
typedef struct dlb_pmd_offset
{
    dlb_pmd_bool present;   /**< PMD_TRUE if valid (present in bitstream), PMD_FALSE otherwise */
    uint16_t     offset;    /**< IAT offset if valid */
} dlb_pmd_offset;


/**
 * @brief encapsulate the PMD Identity and Timestamp (IAT) struct
 */
typedef struct dlb_pmd_identity_and_timing
{
    dlb_pmd_content_id        content_id;
    dlb_pmd_distribution_id   distribution_id;
    dlb_pmd_timestamp         timestamp;
    dlb_pmd_offset            offset;
    dlb_pmd_validity_duration validity_duration;
    dlb_pmd_user_data         user_data;
    dlb_pmd_extension         extension;
} dlb_pmd_identity_and_timing;


/** -----------------  EAC3 Encoding Parameters (EEP)  -------------------- */


/**
 * @def PMD_EAC3_PARAMS_ID_RESERVED (0)
 * @brief Reserved value for EAC3 parameters ID
 */
#define PMD_EAC3_PARAMS_ID_RESERVED (0)


/**
 * @def PMD_EAC3_PARAMS_ID_MAX (255)
 * @brief Maximum value for EAC3 parameters ID
 */
#define PMD_EAC3_PARAMS_ID_MAX (255)


/**
 * @brief Dolby Digital and Dolby Digital Plus bitstream mode
 */
typedef enum
{
    PMD_BSMOD_CM = 0, /**< Bitstream mode 0: Main audio service: complete main (CM)           */
    PMD_BSMOD_ME = 1, /**< Bitstream mode 1: Main audio service: music and effects (ME)       */
    PMD_BSMOD_VI = 2, /**< Bitstream mode 2: Associated audio service: visually impaired (VI) */
    PMD_BSMOD_HI = 3, /**< Bitstream mode 3: Associated audio service: hearing impaired (HI)  */
    PMD_BSMOD_D  = 4, /**< Bitstream mode 4: Associated audio service: dialogue (D)           */
    PMD_BSMOD_C  = 5, /**< Bitstream mode 5: Associated audio service: commentary (C)         */
    PMD_BSMOD_E  = 6, /**< Bitstream mode 6: Associated audio service: emergency (E)          */
    PMD_BSMOD_VO = 7, /**< Bitstream mode 7: Associated audio service: voice over (VO)        */

    PMD_NUM_BSMOD
} dlb_pmd_bsmod;


/**
 * @brief center downmix levels
 *
 * When generating stereo downmix content from 5.1, or 7.1 to 5.1 (etc.)
 * We scale the channels that are being removed before we mix them into
 * the remaining channels.  This enum indicates
 */
typedef enum
{
    PMD_CMIX_LEVEL_30,   /**< scale channel by +3.0 dB (scale by 1.413) */
    PMD_CMIX_LEVEL_15,   /**< scale channel by +1.5 dB (scale by 1.189) */
    PMD_CMIX_LEVEL_00,   /**< scale channel by  0.0 dB (scale by 1)     */
    PMD_CMIX_LEVEL_M15,  /**< scale channel by -1.5 dB (scale by 0.841) */
    PMD_CMIX_LEVEL_M30,  /**< scale channel by -3.0 dB (scale by 0.707) */
    PMD_CMIX_LEVEL_M45,  /**< scale channel by -4.5 dB (scale by 0.595) */
    PMD_CMIX_LEVEL_M60,  /**< scale channel by -6.0 dB (scale by 0.5)   */
    PMD_CMIX_LEVEL_MINF, /**< scale channel by -inf dB (scale by 0)     */
    PMD_CMIX_LEVEL_LAST = PMD_CMIX_LEVEL_MINF,

    PMD_NUM_CMIX_LEVEL
} dlb_pmd_cmixlev;


/**
 * @brief surround downmix levels
 *
 * When generating stereo downmix content from 5.1, or 7.1 to 5.1 (etc.)
 * We scale the channels that are being removed before we mix them into
 * the remaining channels.  This enum indicates
 */
typedef enum
{
    PMD_SURMIX_LEVEL_M15,  /**< scale channel by -1.5 dB (scale by 0.841) */
    PMD_SURMIX_LEVEL_M30,  /**< scale channel by -3.0 dB (scale by 0.707) */
    PMD_SURMIX_LEVEL_M45,  /**< scale channel by -4.5 dB (scale by 0.595) */
    PMD_SURMIX_LEVEL_M60,  /**< scale channel by -6.0 dB (scale by 0.5)   */
    PMD_SURMIX_LEVEL_MINF, /**< scale channel by -inf dB (scale by 0)     */

    PMD_NUM_SURMIX_LEVEL
} dlb_pmd_surmixlev;


/**
 * @brief Height mix level
 *
 * Allowed values 0 to 31, where
 *  * 0 to 30 encodes -0 dB to -30 dB attenuation
 *  *      31 encodes -inf dB (i.e., muting)
 */
typedef unsigned char dlb_pmd_hmixlev;


/**
 * @brief Dolby surround mode enum
 *
 * When operating in the two channel mode, this 2-bit code indicates
 * whether or not the program has been encoded in Dolby Surround. This
 * information is not used by the AC-3 decoder, but may be used by
 * other portions of the audio reproduction equipment.  If dsurmod is
 * set to the reserved code, the decoder should still reproduce
 * audio. The reserved code may be interpreted as “not indicated”.
 */
typedef enum
{
    PMD_DSURMOD_NI,       /**< Dolby surround mode not indicated */
    PMD_DSURMOD_NO,       /**< Not Dolby surround mode encoded   */
    PMD_DSURMOD_YES,      /**< Dolby surround mode encoded       */
    PMD_DSURMOD_RESERVED, /**< reserved value                    */

    PMD_NUM_DSURMOD = PMD_DSURMOD_RESERVED
} dlb_pmd_surmod;


/**
 * This 5-bit code indicates how far the average dialogue level is
 * below digital 100 percent. Valid values are 1 to 31. The value of 0 is
 * reserved. The values of 1 to 31 are interpreted as –1 dB to –31 dB
 * with respect to digital 100 percent. If the reserved value of 0 is
 * received, the decoder shall use –31 dB. The value of dialnorm shall
 * affect the sound reproduction level. If the value is not used by
 * the AC-3 decoder itself, the value shall be used by other parts of
 * typedef
 */
typedef uint8_t dlb_pmd_dialnorm;

#define DLB_PMD_RESERVED_DIALNORM (0)
#define DLB_PMD_MIN_DIALNORM (1)
#define DLB_PMD_MAX_DIALNORM (31)


/**
 * @brief indicate encoder DRC compression profile
 */
typedef enum
{
    PMD_COMPR_NONE,             /**< no compression profile             */
    PMD_COMPR_FILM_STANDARD,    /**< Film Standard compression profile  */
    PMD_COMPR_FILM_LIGHT,       /**< Film light compression profile     */
    PMD_COMPR_MUSIC_STANDARD,   /**< Music standard compression profile */
    PMD_COMPR_MUSIC_LIGHT,      /**< Music light compression profile    */
    PMD_COMPR_SPEECH,           /**< Speech compression profile         */
    PMD_COMPR_RESERVED_FIRST,   /**< Reserved value 0x06                */
    PMD_NUM_COMPR = PMD_COMPR_RESERVED_FIRST,
    PMD_COMPR_RESERVED_LAST     /**< Reserved value 0x07                */
} dlb_pmd_compr;


/**
 * @brief indicate downmix type preference, LoRo or LtRt
 */
typedef enum
{
    PMD_PREFDMIX_NI,   /**< preferred downmix not indicated */
    PMD_PREFDMIX_LTRT, /**< prefer LtRt downmix             */
    PMD_PREFDMIX_LORO, /**< prefer LoRo downmix             */
    PMD_PREFDMIX_PLII, /**< prefer PLII downmix             */

    PMD_NUM_PREFDMIX
} dlb_pmd_prefdmix;


/**
 * @def PMD_EEP_MAX_PRESENTATIONS (15)
 * @brief maximum number of supported AC4 presentations per AC3 program metadata
 */
#define PMD_EEP_MAX_PRESENTATIONS (15)


/**
 * @brief type of PMD EAC3 encoder parameters
 */
typedef struct dlb_pmd_eac3
{
    unsigned int      id;               /**< AC3 program metadata id               */

    /* ---------- optional encoder parameters------------------------------------- */
    dlb_pmd_bool      b_encoder_params; /**< encoder parameters valid?             */
    dlb_pmd_compr     dynrng_prof;      /**< compression profile required for
                                          * dynrng DRC gain words for output DD(+)
                                          * bitstream                              */
    dlb_pmd_compr     compr_prof;       /**< RF mode (heavy) compression profile   */
    dlb_pmd_bool      surround90;       /**< 90-degree phase shift in surrounds?   */
    dlb_pmd_hmixlev   hmixlev;          /**< Heights downmix level                 */

    /* ---------- optional bitstream parameters----------------------------------- */
    dlb_pmd_bool      b_bitstream_params;/**< bitstream parameters valid?          */
    dlb_pmd_bsmod     bsmod;            /**< bistream mode                         */
    dlb_pmd_surmod    dsurmod;          /**< Dolby surround mode status            */
    dlb_pmd_dialnorm  dialnorm;         /**< dialogue normalization                */
    dlb_pmd_prefdmix  dmixmod;          /**< preferred downmix mode                */
    dlb_pmd_cmixlev   ltrtcmixlev;      /**< Center downmix for LtRt               */
    dlb_pmd_surmixlev ltrtsurmixlev;    /**< Surround downmix level for LtRt       */
    dlb_pmd_cmixlev   lorocmixlev;      /**< Center downmix for LoRo               */
    dlb_pmd_surmixlev lorosurmixlev;    /**< Surround downmix level for LoRo       */

    /* ---------- optional extended DRC data ------------------------------------- */
    dlb_pmd_bool      b_drc_params;     /**< DRC parameters valid?                 */
    dlb_pmd_compr     drc_port_spkr;    /**< compression for portable speakers     */
    dlb_pmd_compr     drc_port_hphone;  /**< compression for portable headphones   */
    dlb_pmd_compr     drc_flat_panl;    /**< compression for flat-screen TV        */
    dlb_pmd_compr     drc_home_thtr;    /**< compression mode for Home Theatre     */
    dlb_pmd_compr     drc_ddplus;       /**< compression mode for DD+ encode       */

    /* ---------- optional AC4 data ---------------------------------------------- */
    unsigned int            num_presentations;                        /**< number of ac-4 presentations */
    dlb_pmd_presentation_id presentations[PMD_EEP_MAX_PRESENTATIONS]; /**< presentation identifiers     */
} dlb_pmd_eac3;


/** -----------------  ED2 Turnaround (ETD)  -------------------- */


/**
 * @brief helper struct associating an individual presentation to a particular
 * set of AC3 program metadata; 0 means 'default'
 */
typedef struct dlb_pmd_turnaround
{
    uint16_t presid; /**< presentation id */
    uint16_t eepid;  /**< EAC3 encoder parameters id */
} dlb_pmd_turnaround;


/**
 * @def PMD_ETD_MAX_PRESENTATIONS
 * @brief maximum presentations per ED2 turnaround
 */
#define PMD_ETD_MAX_PRESENTATIONS (15)


/**
 * @brief type of PMD ED2 Turnaround
 */
typedef struct dlb_pmd_ed2_turnaround
{
    unsigned int               id;                /**< ED2 turnaround id                 */

    /* ------------- optional ED2 parameters ------------------------------------------- */
    unsigned int               ed2_presentations; /**< number of ED2 presentations, or 0 */
    dlb_pmd_frame_rate         ed2_framerate;     /**< ED2 frame rate                    */
    dlb_pmd_turnaround         ed2_turnarounds[PMD_ETD_MAX_PRESENTATIONS];
                                                  /**< array of turnaround structs       */

    /* ------------- optional DE parameters -------------------------------------------- */
    unsigned int               de_presentations;  /**< number of DE presentations, or 0  */
    dlb_pmd_frame_rate         de_framerate;      /**< DE frame rate                     */
    dlb_pmd_de_program_config  pgm_config;        /**< DE program config, 0-23           */
    dlb_pmd_turnaround         de_turnarounds[PMD_ETD_MAX_PRESENTATIONS];
                                                  /**< array of turnaround structs,
                                                   * one per pgm                         */
} dlb_pmd_ed2_turnaround;


/** -----------------  ED2 Stream Description (ESD)  -------------------- */


/**
 * @brief information about a single ED2 stream
 *
 * This information is delivered via the ED2 Stream Description
 * payload.
 */
typedef struct dlb_pmd_ed2_stream
{
    dlb_pmd_de_program_config   config;      /**< DE stream config                   */
    uint8_t                     compression; /**< additional compression of DE audio */
} dlb_pmd_ed2_stream;


/**
 * @brief information about an entire system of ED2 streams
 *
 * ED2 is typically carried via a collection of ED2 'sub'streams.
 * This structure combines the individual ED2 stream descriptions
 * delivered in each individual substream to give a complete
 * view.
 */
typedef struct dlb_pmd_ed2_system
{
    uint8_t            count;     /**< total number of streams (1-16) representing 0-15)    */
    dlb_pmd_frame_rate rate;      /**< frame rate                                           */
    dlb_pmd_ed2_stream streams[DLB_PMD_MAX_ED2_STREAMS]; /**< individual stream information */
} dlb_pmd_ed2_system;


/** -----------------  Headphone Element Description (HED)  -------------- */

/**
 * @brief audio element room reverberation
 *
 * this type takes values in the range 0 to 127, where 0 represents
 * totally anechoic (no room reverberation) and 127 represents maximum
 * room reverberation.
 */
typedef uint8_t dlb_pmd_render_mode;


/**
 * @brief bitmap of selected channels
 *
 * Each bit in the map represents an individual speaker channel. For
 * each channel constant C defined in #dlb_pmd_speaker, the bit position is
 *
 * 1ul << (C-1)
 *
 * When the bit is 1, that channel is selected for headphone processing,
 * when 0, it is not.
 *
 * Note that if the audio element in question is *not* a bed, the value
 * of this field is ignored.
 */
typedef unsigned int dlb_pmd_channel_mask;


/**
 * @brief optional binaural processing information for elements
 *
 * This structure lists a supplemental set of parameters that are used
 * to further refine the behaviour of an audio element that is to be included
 * in a binaural processing delivery workflow for headphone reproduction.
 */
typedef struct dlb_pmd_headphone
{
    dlb_pmd_element_id   audio_element_id;
    dlb_pmd_bool         head_tracking_enabled;
    dlb_pmd_render_mode  render_mode;
    dlb_pmd_channel_mask channel_mask;
} dlb_pmd_headphone;


/** -----------------  metadata set  ------------------------------------- */


/**
 * @brief count number of different types of things currently in model
 */
typedef struct dlb_pmd_metadata_count
{
    unsigned int num_signals;
    unsigned int num_beds;
    unsigned int num_objects;
    unsigned int num_updates;
    unsigned int num_presentations;
    unsigned int num_loudness;
    unsigned int num_iat;             /**< 0 or 1 (non-zero interpreted as 1) */
    unsigned int num_eac3;
    unsigned int num_ed2_system;      /**< 0 or 1 (non-zero interpreted as 1) */
    unsigned int num_ed2_turnarounds;
    unsigned int num_headphone_desc;
} dlb_pmd_metadata_count;


/**
 * @brief expanded snapshot of state of professional metadata model
 */
typedef struct dlb_pmd_metadata_set
{
    dlb_pmd_metadata_count       count;

    char                         title[DLB_PMD_TITLE_SIZE];

    dlb_pmd_bed                 *beds;
    dlb_pmd_object              *objects;
    dlb_pmd_update              *updates;
    dlb_pmd_presentation        *presentations;
    dlb_pmd_loudness            *loudness;
    dlb_pmd_identity_and_timing *iat;
    dlb_pmd_eac3                *eac3;
    dlb_pmd_ed2_turnaround      *ed2_turnarounds;
    dlb_pmd_ed2_system          *ed2_system;
    dlb_pmd_headphone           *headphones;
} dlb_pmd_metadata_set;


/** -----------------  model constraints  ------------------------------------- */


/**
 * @brief model constraints
 *
 * Model constraints are used to limit the size of memory required by
 * a model, #dlb_pmd_query_mem_constrained and
 * #dlb_pmd_init_constrained.  Internally, they are also used to model
 * _profiles_, which constrain models to well-known use cases.
 */
typedef struct dlb_pmd_model_constraints
{
    dlb_pmd_metadata_count max;                    /**< max entity count                                           */
    unsigned int           max_elements;           /**< if not 0, upper bound for (max.num_beds + max_num_objects) */
    unsigned int           max_presentation_names; /**< max count of presentation names                            */
} dlb_pmd_model_constraints;


/**
 * @brief abstract type representing the internal PMD model
 */
typedef struct dlb_pmd_model dlb_pmd_model;


/** -----------------  payload set read/write status  ------------------------- */

#define DLB_PMD_AUDIO_ELEMENT_ID_RESERVED (0)

#define DLB_PMD_PAYLOAD_ERROR_DESCRIPTION_MAX (256)
#define DLB_PMD_PAYLOAD_ERROR_DESCRIPTION_LAST (DLB_PMD_PAYLOAD_ERROR_DESCRIPTION_MAX - 1)

typedef enum
{
    DLB_PMD_PAYLOAD_STATUS_OK,
    DLB_PMD_PAYLOAD_STATUS_ERROR,
    DLB_PMD_PAYLOAD_STATUS_OUT_OF_MEMORY,
    DLB_PMD_PAYLOAD_STATUS_VALUE_RESERVED,
    DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE,
    DLB_PMD_PAYLOAD_STATUS_MISSING_AUDIO_ELEMENT,
    DLB_PMD_PAYLOAD_STATUS_INCORRECT_STRUCTURE
} dlb_pmd_payload_status;


struct dlb_pmd_payload_set_status_s;

typedef int (*dlb_pmd_payload_set_status_callback)(struct dlb_pmd_payload_set_status_s *status);


typedef struct dlb_klvpmd_payload_status_s
{
    dlb_pmd_payload_status   payload_status;
    char                     error_description[DLB_PMD_PAYLOAD_ERROR_DESCRIPTION_MAX];
} dlb_pmd_payload_status_record;


typedef struct dlb_pmd_payload_set_status_s
{
    dlb_pmd_bool                     new_frame;                 /* Non-zero if this payload set started at a frame boundary */
    dlb_pmd_payload_status_record    payload_set_status;        /* Overall status of the payload set */

    /* Status for individual payloads in the set... */

    dlb_pmd_bool                     has_crc_payload;
    dlb_pmd_payload_status_record    crc_payload_status;

    dlb_pmd_bool                     has_ver_payload;
    dlb_pmd_payload_status_record    ver_payload_status;

    dlb_pmd_bool                     has_abd_payload;
    dlb_pmd_payload_status_record    abd_payload_status;

    dlb_pmd_bool                     has_aod_payload;
    dlb_pmd_payload_status_record    aod_payload_status;

    dlb_pmd_bool                     has_apd_payload;
    dlb_pmd_payload_status_record    apd_payload_status;

    dlb_pmd_bool                     has_apn_payload;
    dlb_pmd_payload_status_record    apn_payload_status;

    dlb_pmd_bool                     has_aen_payload;
    dlb_pmd_payload_status_record    aen_payload_status;

    dlb_pmd_bool                     has_esd_payload;
    dlb_pmd_payload_status_record    esd_payload_status;

    dlb_pmd_bool                     has_esn_payload;
    dlb_pmd_payload_status_record    esn_payload_status;

    dlb_pmd_bool                     has_eep_payload;
    dlb_pmd_payload_status_record    eep_payload_status;

    unsigned int                     xyz_payload_count;
    dlb_pmd_payload_status_record   *xyz_payload_status;
    unsigned int                     xyz_payload_count_max;

    dlb_pmd_bool                     has_iat_payload;
    dlb_pmd_payload_status_record    iat_payload_status;

    dlb_pmd_bool                     has_pld_payload;
    dlb_pmd_payload_status_record    pld_payload_status;

    dlb_pmd_bool                     has_etd_payload;
    dlb_pmd_payload_status_record    etd_payload_status;

    dlb_pmd_bool                     has_hed_payload;
    dlb_pmd_payload_status_record    hed_payload_status;

    /* ...status for individual payloads in the set */

    dlb_pmd_bool                         count_frames;
    uint64_t                             frame_count;
    uint64_t                             burst_count;

    void                                *callback_arg;
    dlb_pmd_payload_set_status_callback  callback;

} dlb_pmd_payload_set_status;


#ifdef __cplusplus
}
#endif

#endif /* DLB_PMD_TYPES_H */
