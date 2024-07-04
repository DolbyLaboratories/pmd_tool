/************************************************************************
 * dlb_pmd
 * Copyright (c) 2016-2020, Dolby Laboratories Inc.
 * Copyright (c) 2016-2020, Dolby International AB.
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
 * @file dlb_pmd_xml_writer.c
 * @brief xml writer
 */
#include "pmd_model.h"
#include "pmd_idmap.h"

#include "xml_hex.h"
#include "xml_uuid.h"
#include "xml_eidr.h"
#include "xml_ad_id.h"
#include "xml_cdata.h"

#include "dlb_pmd_xml.h"
#include "pmd_error_helper.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <locale.h>


#if defined(_MSC_VER)
#  if !defined(PRIu64)
#    define PRIu64 "I64u"
#endif
typedef intptr_t ssize_t;
#else
#  include <inttypes.h>
#endif


/**
 * @brief state of writer
 */
typedef struct
{
    const dlb_pmd_model *model;    /**< the model being written */
    dlb_xmlpmd_get_buffer getbuf;  /**< get-more-buffer callback */
    void *cbarg;                   /**< client-supplied parameter for #getbuf */
    unsigned int indent;           /**< indentation level */
    char *pos;                     /**< current write position */
    char *end;                     /**< 1st byte after buffer */

    char saved_locale[128];        /**< locale setting at start of write operation */
} writer;


/**
 * @brief initialize writer state
 */
static
void
writer_init
   (      writer *w                  /**< [in] writer state */
   ,const dlb_pmd_model *model       /**< [in] the model being written */
   ,      dlb_xmlpmd_get_buffer gb   /**< [in] get buffer callback */
   ,      void *cbarg                /**< [in] client-supplied callback parameter */
   ,      unsigned int indent        /**< [in] initial indentation of 1st tag */
   )
{
    char *l = setlocale(LC_ALL, NULL);

    memset(w, 0, sizeof(*w));
    strncpy(w->saved_locale, l, sizeof(w->saved_locale));
    if (strcmp(l, "C") != 0)
    {
        setlocale(LC_ALL, "C");
    }

    w->model = model;
    w->getbuf = gb;
    w->cbarg = cbarg;
    w->indent = indent;
    w->pos = NULL;
    w->end = NULL;
}


/**
 * @brief finish writer, send final data to be written
 */
static
void
writer_finish
   (writer *w
   )
{
    w->getbuf(w->cbarg, w->pos, NULL, NULL);
    if (strcmp(w->saved_locale, "C") != 0)
    {
        setlocale(LC_ALL, w->saved_locale);
    }
}


/**
 * @brief increase indentation depth, if possible
 */
static
int                      /** @return 0 on success, 1 on failure */
write_indent
    (writer *w           /**< [in] writer state */
    )
{
    w->indent += 1;
    return 0;
}


/**
 * @brief decrease indentation depth, if possible
 */
static
int                      /** @return 0 on success, 1 on failure */
write_outdent
    (writer *w           /**< [in] writer state */
    )
{
    if (w->indent < 1) return 1;
    w->indent -= 1;
    return 0;
}


/**
 * @brief write a line to output
 *
 * This is the workhorse of the writer algorithm: it does all
 * the buffer management.
 */
static
int                      /** @return 0 on success, 1 on failure */
write_line
    (writer *w           /**< [in] writer state */
    ,const char *str     /**< [in] format string to write */
    ,...
    )
{
    char line[1024];

#define TAB "    "
#define TABSIZE (sizeof(TAB)-1)

    unsigned int i;
    size_t len;
    size_t linesize;
    va_list ap;

    va_start(ap, str);
    len = vsnprintf(line, sizeof(line), str, ap);
    va_end(ap);
    linesize = len + 2 + (TABSIZE * w->indent);

    if (w->pos + linesize >= w->end)
    {
        char *buf;
        size_t capacity;

        if (!w->getbuf(w->cbarg, w->pos, &buf, &capacity))
        {
            printf("Could not get buffer\n");
            return 1;
        }
        w->pos = buf;
        w->end = buf + capacity;
        if (w->pos + linesize >= w->end)
        {
            printf("Failed to get buffer big enough for %d bytes\n", (int)len);
            return 1;
        }
    }

    for (i = 0; i != w->indent; ++i)
    {
        snprintf(w->pos, w->end - w->pos, TAB);
        w->pos += TABSIZE;
        assert(w->pos < w->end);
    }

    w->pos += snprintf(w->pos, w->end - w->pos, "%s\n", line);
    assert(w->pos < w->end);
    return 0;
}


/**
 * @brief helper to write an arbitrary string
 */
static
int                       /** @return 0 on success, 1 on failure */
write_string
    (writer *w            /**< [in] writer state */
    ,const char    *tag   /**< [in] name string to write */
    ,const char    *lang  /**< [in] language attribute to write or NULL */
    ,const uint8_t *str   /**< [in] the tag's string */
    )
{
    uint8_t encoded_string[1024];
    uint8_t attribute[64];

    const uint8_t *send;
    uint8_t *wp = encoded_string;
    uint8_t *wend = wp + sizeof(encoded_string);

    if (!str || str[0] == 0xff)
    {
        return 0;
    }

    send = str + strlen((char*)str);
    memset(encoded_string, '\0', sizeof(encoded_string));
    memset(attribute, '\0', sizeof(attribute));
    if (lang)
    {
        snprintf((char*)attribute, sizeof(attribute), " language=\"%s\"", lang);
    }

    while (wp < wend && str < send)
    {
        uint8_t c = *str;

        if ('&' == c)
        {
            if (wp + 5 >= wend) return 1;
            sprintf((char*)wp, "&amp;");
            wp += 5;
        }
        else if ('<' == c)
        {
            if (wp + 4 >= wend) return 1;
            sprintf((char*)wp, "&lt;");
            wp += 4;
        }
        else
        {
            *wp++ = *str;
        }
        ++str;
    }
    wend[-1] = '\0';

    return write_line(w, "<%s%s>%s</%s>", tag, attribute, encoded_string, tag);
}


/**
 * @brief helper to write out a name
 *
 * unset names are usually written with 0xff, make sure we don't print these.
 */
static
int                      /** @return 0 on success, 1 on failure */
write_name
    (writer *w           /**< [in] writer state */
    ,const uint8_t *str  /**< [in] name string to write */
    )
{
    if (!str || str[0] == 0xff)
    {
        return 0;
    }
    return write_string(w, "Name", NULL, str);
}


/**
 * @brief type of function that can be iteratively applied
 *
 * Functions of this type can be passed to the #write_foreach function
 * to be applied to each entry in an array.  However, because C does
 * not have generic polymorhism (i.e., C++ templates), and has to use
 * void* to represent polymorphic types, the function must return the
 * size of the object, to tell #write_foreach how many bytes to advance
 * to reach the next entry in the list.
 */
typedef
int   /** @return 0 on failure, size of object in bytes in success */
(*write_iterator)
    (writer *w  /**< [in] writer state */
    ,void *obj  /**< [in] object to write */
    );


/**
 * @brief higher-order function to apply a #write_iterator to each
 * element of an array
 */
static
int                          /** @return 0 on success, 1 on failure */
write_foreach
    (writer *w               /**< [in] writer state */
    ,const void *array       /**< [in] array */
    ,unsigned int num        /**< [in] number of entries in array */
    ,write_iterator fn       /**< [in] fn to apply to each entry in array */
    )
{
    unsigned int i;
    uintptr_t addr = (uintptr_t)array;

    for (i = 0; i != num; ++i)
    {
        int res = fn(w, (void*)addr);
        if (!res) return 1;
        addr += res;
    }
    return 0;
}


/**
 * @brief write a tag with a boolean value
 */
static
int                    /** @return 0 on success, 1 on failure */
write_boolean
    (writer *w         /**< [in] writer state */
    ,const char *tag   /**< [in] tag name */
    ,pmd_bool val      /**< [in] value to write */
    )
{
    return write_line(w, "<%s>%s</%s>", tag, val ? "True" : "False", tag);
}


/**
 * @brief write SMPTE 2109 container config dynamic tags
 */
static
int                    /** @return 0 on success, 1 on failure */
write_dynamic_tags
    (writer *w         /**< [in] writer state */
    )
{
    const pmd_smpte2109 *smpte2109 = &w->model->smpte2109;
    const pmd_dynamic_tag *dtag = smpte2109->dynamic_tags;
    unsigned int i;

    if (write_line(w, "<DynamicTags>")) return 1;
    if (write_indent(w)) return 1;
    for (i = 0; i != smpte2109->num_dynamic_tags; ++i)
    {
        unsigned int lt;
        char label[64];
        char *l = label;

        memset(label, '\0', sizeof(label));

        for (lt = 0; lt != 15; ++lt)
        {
            sprintf(l, "%02x.", dtag->universal_label[lt]);
            l += 3;
        }
        sprintf(l, "%02x", dtag->universal_label[lt]);

        lt = dtag->local_tag;
        if (write_line(w, "<Tag id=\"%x\">%s</Tag>", lt, label))
        {
            return 1;
        }
        ++dtag;
    }
    if (write_outdent(w)) return 1;
    if (write_line(w, "</DynamicTags>")) return 1;
    return 0;
}


/**
 * @brief write SMPTE 2109 container config payload
 */
static
int                    /** @return 0 on success, 1 on failure */
write_container_config
    (writer *w         /**< [in] writer state */
    )
{
    const pmd_smpte2109 *smpte2109 = &w->model->smpte2109;

    return write_line(w, "<ContainerConfig>")
        || write_indent(w)
        || write_line(w, "<SampleOffset>%u</SampleOffset>", smpte2109->sample_offset)
        || write_dynamic_tags(w)
        || write_outdent(w)
        || write_line(w, "</ContainerConfig>");
}


/**
 * @brief write ProfessionalMetadata tag
 */
static
int                    /** @return 0 on success, 1 on failure */
write_professional_metadata
    (writer *w         /**< [in] writer state */
    )
{
    const dlb_pmd_model *model = w->model;
    const pmd_profile *p = &model->profile;
    uint8_t vmaj = model->version_maj;
    uint8_t vmin = model->version_min;

    char profile_num_string[64];
    char profile_level_string[64];

    profile_num_string[0] = '\0';
    profile_level_string[0] = '\0';
    if (p->profile_number)
    {
        snprintf(profile_num_string, sizeof(profile_num_string), " profile_number=\"%u\"",
                 p->profile_number);

        snprintf(profile_level_string, sizeof(profile_level_string), "profile_level=\"%u\"",
                 p->profile_level);
    }

    return write_line(w, "<ProfessionalMetadata version=\"%u.%u\"%s%s>", vmaj, vmin,
                      profile_num_string, profile_level_string);
}


/**
 * @brief write a channel object's speaker configuration
 */
static
int                             /** @return 0 on success, 1 on failure */
write_speaker_config
    (writer *w                  /**< [in] writer state */
    ,dlb_pmd_speaker_config cfg /**< [in] speaker config to write */
    )
{
    switch (cfg)
    {
        case DLB_PMD_SPEAKER_CONFIG_2_0:
            return write_line(w, "<SpeakerConfig>2.0</SpeakerConfig>");
        case DLB_PMD_SPEAKER_CONFIG_3_0:
            return write_line(w, "<SpeakerConfig>3.0</SpeakerConfig>");
        case DLB_PMD_SPEAKER_CONFIG_5_1:
            return write_line(w, "<SpeakerConfig>5.1</SpeakerConfig>");
        case DLB_PMD_SPEAKER_CONFIG_5_1_2:
            return write_line(w, "<SpeakerConfig>5.1.2</SpeakerConfig>");
        case DLB_PMD_SPEAKER_CONFIG_5_1_4:
            return write_line(w, "<SpeakerConfig>5.1.4</SpeakerConfig>");
        case DLB_PMD_SPEAKER_CONFIG_7_1_4:
            return write_line(w, "<SpeakerConfig>7.1.4</SpeakerConfig>");
        case DLB_PMD_SPEAKER_CONFIG_9_1_6:
            return write_line(w, "<SpeakerConfig>9.1.6</SpeakerConfig>");
        case DLB_PMD_SPEAKER_CONFIG_PORTABLE:
            return write_line(w, "<SpeakerConfig>Portable Speaker</SpeakerConfig>");
        case DLB_PMD_SPEAKER_CONFIG_HEADPHONE:
            return write_line(w, "<SpeakerConfig>Portable Headphone</SpeakerConfig>");
        default:
            printf("invalid speaker config %u\n", cfg);
            return 1;
    }
}


/**
 * @brief write an audio bed's source bed id, if any
 */
static
int                             /** @return 0 on success, 1 on failure */
write_audio_bed_source
    (writer *w                  /**< [in] writer state */
    ,pmd_channel_metadata *cmd  /**< [in] channel metadata */
    )
{
    if (cmd->derived)
    {
        return write_line(w, "<SourceBedId>%u</SourceBedId>", cmd->origin);
    }
    return 0;
}


/**
 * @brief write a generic object's class
 */
static
int                             /** @return 0 on success, 1 on failure */
write_class
    (writer *w                  /**< [in] writer state */
    ,pmd_object_class class     /**< [in] object class */
    )
{
    switch (class)
    {
    case PMD_CLASS_DIALOG:          return write_line(w, "<Class>Dialog</Class>");
    case PMD_CLASS_VDS:             return write_line(w, "<Class>VDS</Class>");
    case PMD_CLASS_VOICEOVER:       return write_line(w, "<Class>Voice Over</Class>");
    case PMD_CLASS_GENERIC:         return write_line(w, "<Class>Generic</Class>");
    case PMD_CLASS_SUBTITLE:        return write_line(w, "<Class>Spoken Subtitle</Class>");
    case PMD_CLASS_EMERGENCY_ALERT: return write_line(w, "<Class>Emergency Alert</Class>");
    case PMD_CLASS_EMERGENCY_INFO:  return write_line(w, "<Class>Emergency Information</Class>");
    default: return 1;
    }
}


/**
 * @brief write an object's positional co-ordinate
 */
static
int                             /** @return 0 on success, 1 on failure */
write_coordinate
    (writer *w                  /**< [in] writer state */
    ,const char *tag            /**< [in] tag name */
    ,pmd_position pos           /**< [in] position to write */
    )
{
    float p = pmd_decode_position(pos);
    return write_line(w, "<%s>%.*f</%s>", tag, w->model->coordinate_print_precision, p, tag);
}


/**
 * @brief write an object's size
 */
static
int                             /** @return 0 on success, 1 on failure */
write_size
    (writer *w                  /**< [in] writer state */
    ,pmd_size size              /**< [in] object size */
    )
{
    double s = (double)size / 31.0;
    return write_line(w, "<Size>%.*f</Size>", w->model->coordinate_print_precision, s);
}


/**
 * @brief write an object's or channel gain
 */
static
int                             /** @return 0 on success, 1 on failure */
write_gain
    (writer *w                  /**< [in] writer state */
    ,const char *tag            /**< [in] tag name */
    ,pmd_gain gain              /**< [in] gain value */
    )
{
    float f = pmd_gain_to_db(gain);
    if (isinf(f) && f < 0.0f)
    {
        return write_line(w, "<%s>-infdB</%s>", tag, tag);
    }
    else
    {
        return write_line(w, "<%s>%.01fdB</%s>", tag, f, tag);
    }
}


/**
 * @brief determine if any signals exist for a given speaker
 */
static
int                               /** @return 1 if speaker has signal, 0 otherwise */
speaker_exists
    (pmd_speaker speaker          /**< [in] target speaker */
    ,pmd_track_metadata *trackmd  /**< [in] array of track metadata */
    ,unsigned int num_tracks      /**< [in] number of tracks */
    )
{
    unsigned int i;

    for (i = 0; i != num_tracks; ++i)
    {
        if (trackmd[i].target == speaker)
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief write list of audio signal ids that contribute to a given speaker
 *
 * This one requires walking through all channel track metadata to find
 * channels that contribute to a speaker.
 */
static
int                               /** @return 0 on success, 1 on failure */
write_speaker_signals
    (writer *w                    /**< [in] writer state */
    ,pmd_speaker speaker          /**< [in] target speaker */
    ,pmd_track_metadata *trackmd  /**< [in] array of track metadata */
    ,unsigned int num_tracks      /**< [in] number of tracks */
    )
{
    unsigned int i;

    for (i = 0; i != num_tracks; ++i)
    {
        if (trackmd[i].target == speaker)
        {
            uint8_t signal_id = trackmd[i].source;
            float f = pmd_gain_to_db(trackmd[i].gain);
            char tmp[64];

            tmp[0] = '\0';
            if (f != 0.0f)
            {
                if (isinf(f) && f < 0.0f)
                {
                    snprintf(tmp, sizeof(tmp), " source_gain_db=\"-infdB\"");
                }
                else
                {
                    snprintf(tmp, sizeof(tmp), " source_gain_db=\"%.01fdB\"", f);
                }
            }
            if (write_line(w, "<ID%s>%u</ID>", tmp, signal_id+1))
            {
                return 1;
            }
        }
    }
    return 0;
}


static const char *speakernames[PMD_NUM_SPEAKERS] =
{
    "error!",
    "Left", "Right", "Center", "LFE",
    "Left Surround", "Right Surround",
    "Left Rear Surround", "Right Rear Surround",
    "Left Top Front", "Right Top Front",
    "Left Top Middle", "Right Top Middle",
    "Left Top Rear", "Right Top Rear",
    "Left Front Wide", "Right Front Wide"
};


/**
 * @brief write list of output targets for a channel-based object
 */
static
int                               /** @return 0 on success, 1 on failure */
write_output_targets
    (writer *w                    /**< [in] writer state */
    ,pmd_track_metadata *trackmd  /**< [in] array of track metadata */
    ,unsigned int num_tracks      /**< [in] number of tracks */
    )
{
    pmd_speaker i;

    if (write_line(w, "<OutputTargets>")) return 1;
    if (write_indent(w)) return 1;

    for (i = 0; i != PMD_NUM_SPEAKERS; ++i)
    {
        if (speaker_exists(i, trackmd, num_tracks))
        {
            if (   write_line(w, "<OutputTarget id=\"%s\">", speakernames[i])
                || write_indent(w)
                || write_line(w, "<AudioSignals>")
                || write_indent(w)
                || write_speaker_signals(w, i, trackmd, num_tracks)
                || write_outdent(w)
                || write_line(w, "</AudioSignals>")
                || write_outdent(w)
                || write_line(w, "</OutputTarget>")
                )
            {
                return 1;
            }
        }
    }

    if (write_outdent(w)) return 1;
    if (write_line(w, "</OutputTargets>")) return 1;
    return 0;
}


/**
 * @brief #write_iterator function to write channel-mode pmd_audio_object structs
 */
static
int                             /** @return 0 on failure, sizeof(pmd_audio_object) on success*/
write_audio_bed_iterator
    (writer *w                  /**< [in] writer state */
    ,pmd_element *e             /**< [in] pointer to pmd_audio_object element */
    )
{
    const uint8_t *name = pmd_model_lookup_element_name(w->model, e->id);

    if (   write_line(w, "<AudioBed id=\"%u\">", e->id)
        || write_indent(w)
        || write_name(w, name)
        || write_speaker_config(w, e->md.channel.config)
        || write_audio_bed_source(w, &e->md.channel)
        || write_output_targets(w, e->md.channel.metadata, e->md.channel.num_tracks)
        || write_outdent(w)
        || write_line(w, "</AudioBed>")
      )
    {
        error(w->model, "failed to write audio bed %u", e->id);
    }
    return sizeof(*e);
}


/**
 * @brief #write_iterator function to write generic-object-mode pmd_audio_object structs
 */
static
int                             /** @return 0 on failure, sizeof(pmd_audio_object) on success*/
write_generic_object_iterator
    (writer *w                  /**< [in] writer state */
    ,pmd_element *e             /**< [in] pointer to pmd_element */
    )
{
    const uint8_t *name = pmd_model_lookup_element_name(w->model, e->id);

    if (   write_line(w, "<AudioObject id=\"%u\">", e->id)
        || write_indent(w)
        || write_name(w, name)
        || write_class(w, e->md.object.oclass)
        || write_boolean(w, "DynamicUpdates", e->md.object.dynamic_updates)
        || write_coordinate(w, "X_Pos", e->md.object.x)
        || write_coordinate(w, "Y_Pos", e->md.object.y)
        || write_coordinate(w, "Z_Pos", e->md.object.z)
        || write_size(w, e->md.object.size)
        || write_boolean(w, "Size_3D", e->md.object.size_vertical)
        || write_boolean(w, "Diverge", e->md.object.diverge)
        || write_line(w, "<AudioSignal>%u</AudioSignal>", e->md.object.source+1)
        || write_gain(w, "SourceGainDB", e->md.object.gain)
        || write_outdent(w)
        || write_line(w, "</AudioObject>")
      )
    {
        error(w->model, "failed to write audio object %u", e->id);
    }
    return sizeof(*e);
}


/**
 * @brief #write_iterator function to write beds
 */
static
int                             /** @return 0 on failure, sizeof(pmd_audio_object) on success*/
write_bed_element_iterator
    (writer *w                  /**< [in] writer state */
    ,void *arg                  /**< [in] pointer to pmd_audio_bed element */
    )
{
    pmd_element *e = (pmd_element *)arg;

    if (e->mode == PMD_MODE_CHANNEL)
    {
        return write_audio_bed_iterator(w, e);
    }
    return sizeof(pmd_element);
}


/**
 * @brief #write_iterator function to write pmd_audio_object structs
 */
static
int                             /** @return 0 on failure, sizeof(pmd_audio_element) on success*/
write_obj_element_iterator
    (writer *w                  /**< [in] writer state */
    ,void *arg                  /**< [in] pointer to pmd_audio_object element */
    )
{
    pmd_element *e = (pmd_element *)arg;

    if (e->mode == PMD_MODE_OBJECT)
    {
        return write_generic_object_iterator(w, e);
    }
    return sizeof(pmd_element);
}


/**
 * @brief write a language string
 */
static
int                          /** @return 0 on success, 1 on failure */
write_language
    (writer *w               /**< [in] writer state */
    ,const char *tagname     /**< [in] tag name */
    ,pmd_langcode lang       /**< [in] PMD language code to write */
    )
{
    if (~0u != lang)
    {
        char tmp[4];
        pmd_langcode_string(lang, &tmp);
        return write_line(w, "<%s>%s</%s>", tagname, tmp, tagname);
    }
    return 0;
}


/**
 * @brief helper function to write the appropriate object count string
 */
static inline
void
snprintf_object
    (char **output
    ,size_t *capacity
    ,const char *objname
    ,unsigned int count
    )
{
    size_t len = 0;

    if (count > 1)
    {
        len = snprintf(*output, *capacity, " + %u%s", count, objname);
    }
    else if (count == 1)
    {
        len = snprintf(*output, *capacity, " + %s", objname);
    }
    if (len > *capacity)
    {
        len = *capacity;
    }
    *output = *output + len;
    *capacity = *capacity - len;
}


/**
 * @brief generate a human-readable presentation config string
 */
static inline
void
generate_presentation_config_string
    (      pmd_apd *pres
    ,const dlb_pmd_model *model
    ,      char *output
    ,      size_t capacity
    )
{
    static const char *speaker_config_names[NUM_PMD_SPEAKER_CONFIGS] =
    {
        "2.0", "3.0", "5.1", "5.1.2", "5.1.4", "7.1.4", "9.1.6",
        "Portable Speaker", "Portable Headphone"
    };

    ssize_t len = 0;
    unsigned int object_counts[PMD_CLASS_RESERVED];
    unsigned int num_obj = 0;
    unsigned int idx;

    pmd_apd_iterator pi;
    pmd_apd_iterator_init(&pi, pres);
    memset(object_counts, '\0', sizeof(object_counts));

    while (pmd_apd_iterator_next(&pi, &idx))
    {
        if (model->element_list[idx].mode == PMD_MODE_OBJECT)
        {
            const pmd_object_metadata *objmd = &model->element_list[idx].md.object;
            object_counts[objmd->oclass] += 1;
            ++num_obj;
            ++num_obj;
        }
    }

    len = snprintf(output, capacity, "%s", speaker_config_names[pres->config]);
    if (pres->config < DLB_PMD_SPEAKER_CONFIG_PORTABLE)
    {
        len += snprintf(output+len, capacity-len, " %s",
                        object_counts[PMD_CLASS_DIALOG] ? "ME" : "CM");
        output += len;
        capacity -= len;
        snprintf_object(&output, &capacity, "D",   object_counts[PMD_CLASS_DIALOG]);
        snprintf_object(&output, &capacity, "VDS", object_counts[PMD_CLASS_VDS]);
        snprintf_object(&output, &capacity, "VO",  object_counts[PMD_CLASS_VOICEOVER]);
        snprintf_object(&output, &capacity, "O",   object_counts[PMD_CLASS_GENERIC]);
        snprintf_object(&output, &capacity, "SS",  object_counts[PMD_CLASS_SUBTITLE]);
        snprintf_object(&output, &capacity, "EA",  object_counts[PMD_CLASS_EMERGENCY_ALERT]);
        snprintf_object(&output, &capacity, "EI",  object_counts[PMD_CLASS_EMERGENCY_INFO]);
    }
}


/**
 * @brief write array of audio signals
 */
static
int                                /** @return 0 on success, 1 on failure */
write_audio_signals
    (      writer *w               /**< [in] writer state */
    ,const dlb_pmd_model *model    /**< [in] model */
    ,      unsigned int num        /**< [in] number of audio signals */
    )
{
    unsigned int i;

    if (write_line(w, "<AudioSignals>")) return 1;
    if (write_indent(w)) return 1;

    i = 0;
    while (num)
    {
        /* signals are stored in the bitmap 0-based, but written to XML 1-based */
        if (pmd_signals_test(&model->signals, i))
        {
            if (   write_line(w, "<AudioSignal id=\"%u\">", i+1)
                || write_indent(w)
                || write_line(w, "<Name>Signal %u</Name>", i+1)
                || write_outdent(w)
                || write_line(w, "</AudioSignal>")
               )
            {
                return 1;
            }
            --num;
        }
        ++i;
    }

    if (write_outdent(w)) return 1;
    if (write_line(w, "</AudioSignals>")) return 1;
    return 0;
}


/**
 * @brief write array of audio elements
 */
static
int                                /** @return 0 on success, 1 on failure */
write_audio_elements
    (      writer *w               /**< [in] writer state */
    ,const pmd_element *e          /**< [in] array of audio elements */
    ,      unsigned int num        /**< [in] number of audio elements */
    )
{
    return write_line(w, "<AudioElements>")
        || write_indent(w)
        || write_foreach(w, e, num, write_bed_element_iterator)
        || write_foreach(w, e, num, write_obj_element_iterator)
        || write_outdent(w)
        || write_line(w, "</AudioElements>");
}


/**
 * @brief #write_iterator function to write pmd_audio_presentation structs
 */
static
int                             /** @return 0 on failure, sizeof(pmd_audio_presentation) on success*/
write_presentation_iterator
    (writer *w                  /**< [in] writer state */
    ,void *arg                  /**< [in] ptr to pmd_audio_presentation */
    )
{
    pmd_apd *p = (pmd_apd*)arg;
    pmd_apd_iterator pi;
    const pmd_apn *name;
    const pmd_element *elements = w->model->element_list;
    char cfg[128];
    unsigned int idx;
    unsigned int i;

    generate_presentation_config_string(p, w->model, cfg, sizeof(cfg));

    if (   write_line(w, "<Presentation id=\"%u\">", p->id)
        || write_indent(w)
       )
    {
        error(w->model, "failed to write presentation %u", p->id);
        return 0;
    }

    for (i = 0; i != p->num_names; ++i)
    {
        char tmp[4];
        name = pmd_apn_list_lookup(&w->model->apn_list, p->names[i]);
        if ((const uint8_t)name->text[0] == 0xff)
        {
            error(w->model, "illegal name for presentation %u\n", p->id);
        }
        else
        {
            pmd_langcode_string(name->lang, &tmp);
            if (write_string(w, "Name", tmp, name->text))
            {
                error(w->model, "failed to write name \"%s\" for presentation %u\n", tmp, p->id);
                return 0;
            }
        }
    }

    if (   write_line(w, "<Config>%s</Config>", cfg)
        || write_language(w, "Language", p->pres_lang))
    {
        error(w->model, "failed to write presentation %u\n", p->id);
        return 0;
    }

    pmd_apd_iterator_init(&pi, p);
    while (pmd_apd_iterator_next(&pi, &idx))
    {
        if (write_line(w, "<Element>%u</Element>", elements[idx].id))
        {
            error(w->model, "failed to write element %u in presentation %u\n", elements[idx].id, p->id);
            return 0;
        }
    }
    pmd_apd_iterator_finish(&pi);

    if (   write_outdent(w)
        || write_line(w, "</Presentation>")
       )
    {
        error(w->model, "failed to write presentation %u\n", p->id);
        return 0;
    }
    return sizeof(*p);
}


/**
 * @brief write array of audio presentations
 */
static
int                                       /** @return 0 on success, 1 on failure */
write_audio_presentations
    (      writer *w                      /**< [in] writer state */
    ,const pmd_apd *pres                  /**< [in] array of audio objects */
    ,      unsigned int num               /**< [in] number of audio objects */
    )
{
    return write_line(w, "<Presentations>")
        || write_indent(w)
        || write_foreach(w, pres, num, write_presentation_iterator)
        || write_outdent(w)
        || write_line(w, "</Presentations>");
}


/**
 * @brief write loudness practice type information
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_loudness_practice_type
    (writer *w                      /**< [in] writer state */
    ,pmd_pld *pld                   /**< [in] loudness information */
    )
{
    static const char *lptname[] =
    {
        "NI", "ATSC", "EBU", "ARIB", "FreeTV", "5", "6", "7",
        "8", "9", "10", "11", "12", "13", "Manual", "Consumer"
    };
    static const char *dialgate[] =
    {
        "NI", "Center", "Front", "Manual"
    };
    static const char *corrty[] =
    {
        "file", "realtime"
    };

    if (pld->lpt != 0)
    {
        char attributes1[64];
        char attributes2[64];
        attributes1[0] = '\0';
        attributes2[0] = '\0';

        if (pld->options & PMD_PLD_OPT_LOUDCORR_DIALGATE)
        {
            snprintf(attributes1, sizeof(attributes1), " dialgate=\"%s\"", dialgate[pld->dpt]);
        }

        snprintf(attributes2, sizeof(attributes2), " correction_type=\"%s\"", corrty[pld->corrty]);
        return write_line(w, "<PracticeType%s%s>%s</PracticeType>",
                          attributes1, attributes2, lptname[pld->lpt]);
    }
    return 0;
}


/**
 * @brief write loudness relative gated value
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_loudness_relgat
    (writer *w                      /**< [in] writer state */
    ,pmd_pld *pld /**< [in] loudness information */
    )
{
    if (pld->options & PMD_PLD_OPT_LOUDRELGAT)
    {
        return write_line(w, "<LoudnessRelativeGated>%.01f</LoudnessRelativeGated>",
                          pmd_decode_lufs(pld->lrg));
    }
    return 0;
}


/**
 * @brief write loudness speech-gated value
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_loudness_spchgat
    (writer *w                      /**< [in] writer state */
    ,pmd_pld *pld /**< [in] loudness information */
    )
{
    if (pld->options & PMD_PLD_OPT_LOUDSPCHGAT)
    {
        static const char *dialgate[] =
        {
            "NI", "Center", "Front", "Manual", "4", "5", "6", "7"
        };

        return write_line(w, "<LoudnessSpeechGated dialgate=\"%s\">%.01f</LoudnessSpeechGated>",
                          dialgate[pld->sdpt],
                          pmd_decode_lufs(pld->lsg));
    }
    return 0;
}


/**
 * @brief write loudness short-term 3 seconds value
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_loudness_3sec
    (writer *w                      /**< [in] writer state */
    ,pmd_pld *pld /**< [in] loudness information */
    )
{
    if (pld->options & PMD_PLD_OPT_LOUDSTRM3S)
    {
        if (write_line(w, "<Loudness3Seconds>%.01f</Loudness3Seconds>",
                       pmd_decode_lufs(pld->l3g)))
        {
            return 1;
        }
    }
    if (pld->options & PMD_PLD_OPT_MAX_LOUDSTRM3S)
    {
        return write_line(w, "<MaxLoudness3Seconds>%.01f</MaxLoudness3Seconds>",
                          pmd_decode_lufs(pld->l3g_max));
    }
    return 0;
}


/**
 * @brief write loudness true-peak (and max true-peak) values if present
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_loudness_truepeak
    (writer *w                      /**< [in] writer state */
    ,pmd_pld *pld /**< [in] loudness information */
    )
{
    if (pld->options & PMD_PLD_OPT_TRUEPK)
    {
        if (write_line(w, "<TruePeak>%.01f</TruePeak>", pmd_decode_truepk(pld->tpk)))
        {
            return 1;
        }
    }
    if (pld->options & PMD_PLD_OPT_MAX_TRUEPK)
    {
        return write_line(w, "<MaxTruePeak>%.01f</MaxTruePeak>", pmd_decode_truepk(pld->tpk_max));
    }
    return 0;
}


/**
 * @brief write loudness program boundary value
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_loudness_pgmbndy
    (writer *w                      /**< [in] writer state */
    ,pmd_pld *pld /**< [in] loudness information */
    )
{
    if (pld->options & PMD_PLD_OPT_PRGMBNDY)
    {
        char attributes[128];

        attributes[0] = '\0';
        if (pld->options & PMD_PLD_OPT_PRGMBNDY_OFFSET)
        {
            snprintf(attributes, sizeof(attributes), " offset=\"%u\"", pld->prgmbndy_offset);
        }
        return write_line(w, "<ProgramBoundary%s>%s%d</ProgramBoundary>",
                          attributes,
                          pld->prgmbndy < 0 ? "-" : "",
                          1ul << labs(pld->prgmbndy));
    }
    return 0;
}


/**
 * @brief write loudness range
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_loudness_range
    (writer *w                      /**< [in] writer state */
    ,pmd_pld *pld                   /**< [in] loudness information */
    )
{
    if (pld->options & PMD_PLD_OPT_LRA)
    {
        return write_line(w, "<LoudnessRange practice=\"%d\">%.01f</LoudnessRange>",
                          (int)pld->lrap,
                          pmd_decode_lra(pld->lra));
    }
    return 0;
}


/**
 * @brief write momentary loudness
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_loudness_momentary
    (writer *w                      /**< [in] writer state */
    ,pmd_pld *pld /**< [in] loudness information */
    )
{
    if (pld->options & PMD_PLD_OPT_LOUDMNTRY)
    {
        if (write_line(w, "<MomentaryLoudness>%.01f</MomentaryLoudness>",
                       pmd_decode_lufs(pld->ml)))
        {
            return 1;
        }
    }
    if (pld->options & PMD_PLD_OPT_MAX_LOUDMNTRY)
    {
        return write_line(w, "<MaxMomentaryLoudness>%.01f</MaxMomentaryLoudness>",
                          pmd_decode_lufs(pld->ml_max));
    }
    return 0;
}


/**
 * @brief write loudness extension
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_loudness_extension
    (writer *w                      /**< [in] writer state */
    ,pmd_pld *pld                   /**< [in] loudness information */
    )
{
    if (pld->options & PMD_PLD_OPT_EXTENSION)
    {
        char tmp[sizeof(pld->extension) * 2+1];
        size_t tmpsize = sizeof(tmp);
        char attribute[64];
        pmd_bool hex = 0;

        attribute[0] = '\0';
        if (pld->extension_bits & 0x7)
        {
            snprintf(attribute, sizeof(attribute), " bits=\"%u\"",
                     pld->extension_bits);
        }

        memset(tmp, '\0', sizeof(tmp));
        if (encode_cdata(pld->extension, (pld->extension_bits + 7)/8, tmp, &tmpsize, &hex))
        {
            return 1;
        }
        return write_line(w, "<Extension%s><%s>%s</%s></Extension>",
                          attribute,
                          hex ? "base16" : "ascii",
                          tmp,
                          hex ? "base16" : "ascii"
                          );
    }
    return 0;
}


/**
 * @brief #write_iterator function to write pmd_pld structs
 */
static
int                             /** @return 0 on failure, sizeof(pmd_pld) on success*/
write_loudness_iterator
    (writer *w                  /**< [in] writer state */
    ,void *arg                  /**< [in] ptr to pmd_pld */
    )
{
    const dlb_pmd_model *model = w->model;
    pmd_pld *pld = (pmd_pld*)arg;

    if (   write_line(w, "<Presentation>")
        || write_indent(w)
        || write_line(w, "<PresentationId>%u</PresentationId>",
                      model->apd_list[pld->presid].id)
        || write_loudness_practice_type(w, pld)
        || write_loudness_relgat(w, pld)
        || write_loudness_spchgat(w, pld)
        || write_loudness_3sec(w, pld)
        || write_loudness_truepeak(w, pld)
        || write_loudness_pgmbndy(w, pld)
        || write_loudness_range(w, pld)
        || write_loudness_momentary(w, pld)
        || write_loudness_extension(w, pld)
        || write_outdent(w)
        || write_line(w, "</Presentation>")
       )
    {
        error(w->model, "failed to write presentation loudness for presentation %u\n",
              model->apd_list[pld->presid].id);
        return 0;
    }
    return sizeof(*pld);
}


/**
 * @brief write array of presentation loudness
 */
static
int                                 /** @return 0 on success, 1 on failure */
write_presentation_loudness
    (writer *w                      /**< [in] writer state */
    ,const pmd_pld *pld             /**< [in] array of presentation loudness descriptions */
    ,unsigned int num               /**< [in] number of presentation loudness descriptions */
    )
{
    if (w->model->num_pld)
    {
        return write_line(w, "<PresentationLoudness>")
            || write_indent(w)
            || write_foreach(w, pld, num, write_loudness_iterator)
            || write_outdent(w)
            || write_line(w, "</PresentationLoudness>");
    }
    return 0;
}


/**
 * @brief write HED channel exclusion bits
 */
static
int                                 /** @return 0 on success, 1 on failure */
write_excluded_speaker_bits
    (writer *w                      /**< [in] writer state */
    ,dlb_pmd_channel_mask mask      /**< [in] channel mask bits */
    )
{
    unsigned int m = 1u;
    unsigned int i;

    for (i = 0; i != 16; ++i)
    {
        if (!(mask & m))
        {
            if (write_line(w, "<ID>%s</ID>", speakernames[i+1])) return 1;
        }
        m = m << 1;
    }
    return 0;
}


/**
 * @brief write HED channel exclusions
 */
static
int                                 /** @return 0 on success, 1 on failure */
write_hed_channel_exclusions
    (writer *w                      /**< [in] writer state */
    ,dlb_pmd_headphone *hed         /**< [in] headphone description */
    )
{
    const dlb_pmd_model *model = w->model;
    const pmd_element *e = &model->element_list[hed->audio_element_id];

#define HED_MASK ((1u << 16)-1)

    if (e->mode != PMD_MODE_CHANNEL
        || (hed->channel_mask & HED_MASK) == HED_MASK)
    {
        return 0;
    }

    if (   write_line(w, "<ChannelExclusions>")
        || write_indent(w)
        || write_excluded_speaker_bits(w, hed->channel_mask)
        || write_outdent(w)
        || write_line(w, "</ChannelExclusions>")
       )
    {
        return 1;
    }
    return 0;
}


/**
 * @brief #write_iterator function to write dlb_pmd_headphone structs
 */
static
int                             /** @return 0 on failure, sizeof(dlb_pmd_headphone) on success*/
write_headphone_iterator
    (writer *w                  /**< [in] writer state */
    ,void *arg                  /**< [in] ptr to dlb_pmd_headphone */
    )
{
    dlb_pmd_headphone *hed = (dlb_pmd_headphone*)arg;
    const dlb_pmd_model *model = w->model;
    const pmd_element *e = &model->element_list[hed->audio_element_id];
    size_t id = 1 + (hed - model->hed_list);

    if (   write_line(w, "<HeadphoneElement id=\"%u\">", id)
        || write_indent(w)
        || write_line(w, "<Element>%u</Element>", e->id)
        || write_boolean(w, "HeadTrackingEnabled", hed->head_tracking_enabled)
        || write_line(w, "<RenderMode>%u</RenderMode>", hed->render_mode)
        || write_hed_channel_exclusions(w, hed)
        || write_outdent(w)
        || write_line(w, "</HeadphoneElement>")
       )
    {
        error(w->model, "Failed to write Headphone Elements\n");
        return 0;
    }
    return sizeof(*hed);
}


/**
 * @brief write array of headphone descriptions
 */
static
int                                 /** @return 0 on success, 1 on failure */
write_headphone_descriptions
    (writer *w                      /**< [in] writer state */
    ,const pmd_hed *hed             /**< [in] array of headphone descriptions */
    ,unsigned int num               /**< [in] number of headphone descriptions */
    )
{
    if (w->model->num_hed)
    {
        return write_line(w, "<HeadphoneElements>")
            || write_indent(w)
            || write_foreach(w, hed, num, write_headphone_iterator)
            || write_outdent(w)
            || write_line(w, "</HeadphoneElements>");
    }
    return 0;
}


/**
 * @brief write Bsmod
 */
static
int                              /** @return 0 on success, 1 on failure */
write_bsmod
    (writer *w                   /**< [in] writer state */
    ,pmd_bitstream_mode bsmod    /**< [in] bitstream mode value */
    )
{
    switch (bsmod)
    {
    case PMD_BSMOD_CM:  return write_line(w, "<BsMod>Complete Main</BsMod>");
    case PMD_BSMOD_ME:  return write_line(w, "<BsMod>Music and Effects</BsMod>");
    case PMD_BSMOD_VI:  return write_line(w, "<BsMod>Visually Impaired</BsMod>");
    case PMD_BSMOD_HI:  return write_line(w, "<BsMod>Hearing Impaired</BsMod>");
    case PMD_BSMOD_D:   return write_line(w, "<BsMod>Dialogue</BsMod>");
    case PMD_BSMOD_C:   return write_line(w, "<BsMod>Commentary</BsMod>");
    case PMD_BSMOD_E:   return write_line(w, "<BsMod>Emergency</BsMod>");
    case PMD_BSMOD_VO:  return write_line(w, "<BsMod>Voice Over</BsMod>");
    default:            return 1;
    }
}


/**
 * @brief write dsurmod value
 */
static
int                                       /** @return 0 on success, 1 on failure */
write_dsurmod
    (writer *w                            /**< [in] writer state */
    ,pmd_surround_mode dsurmod            /**< [in] Dolby surround mode */
    )
{
    switch (dsurmod)
    {
    case PMD_DSURMOD_NI:  return write_line(w, "<SurMod>Not Indicated</SurMod>");
    case PMD_DSURMOD_NO:  return write_line(w, "<SurMod>Not Dolby Surround Encoded</SurMod>");
    case PMD_DSURMOD_YES: return write_line(w, "<SurMod>Dolby Surround Encoded</SurMod>");
    default:              return 1;
    }
}


/**
 * @brief write DRC Compression standard
 */
static
int                                       /** @return 0 on success, 1 on failure */
write_compr
    (writer *w                            /**< [in] writer state */
    ,const char *tag                      /**< [in] tagname */
    ,pmd_compression_mode mode            /**< [in] compression mode */
    )
{
    switch (mode)
    {
    case PMD_COMPR_NONE:            return write_line(w, "<%s>None</%s>", tag, tag);
    case PMD_COMPR_FILM_STANDARD:   return write_line(w, "<%s>Film Standard</%s>", tag, tag);
    case PMD_COMPR_FILM_LIGHT:      return write_line(w, "<%s>Film Light</%s>", tag, tag);
    case PMD_COMPR_MUSIC_STANDARD:  return write_line(w, "<%s>Music Standard</%s>", tag, tag);
    case PMD_COMPR_MUSIC_LIGHT:     return write_line(w, "<%s>Music Light</%s>", tag, tag);
    case PMD_COMPR_SPEECH:          return write_line(w, "<%s>Speech</%s>", tag, tag);
    default:                        return 1;
    }
}


/**
 * @brief write preferred downmix mode
 */
static
int                                       /** @return 0 on success, 1 on failure */
write_dmixmod
    (writer *w                            /**< [in] writer state */
    ,pmd_preferred_downmix dmixmod        /**< [in] preferred downmix */
    )
{
    switch (dmixmod)
    {
    case PMD_PREFDMIX_NI:   return write_line(w, "<PrefDMixMod>Not Indicated</PrefDMixMod>");
    case PMD_PREFDMIX_LTRT: return write_line(w, "<PrefDMixMod>LtRt</PrefDMixMod>");
    case PMD_PREFDMIX_LORO: return write_line(w, "<PrefDMixMod>LoRo</PrefDMixMod>");
    case PMD_PREFDMIX_PLII: return write_line(w, "<PrefDMixMod>PLII</PrefDMixMod>");
    default:                return 1;
    }
}


/**
 * @brief write center downmix scaling level
 */
static
int                                       /** @return 0 on success, 1 on failure */
write_cmixlev
    (writer *w                            /**< [in] writer state */
    ,const char *tag                      /**< [in] tag name */
    ,pmd_cmix_level mixlev                /**< [in] downmix level */
    )
{
    switch (mixlev)
    {
    case PMD_CMIX_LEVEL_30:   return write_line(w, "<%s>+3.0dB</%s>", tag, tag);
    case PMD_CMIX_LEVEL_15:   return write_line(w, "<%s>+1.5dB</%s>", tag, tag);
    case PMD_CMIX_LEVEL_00:   return write_line(w, "<%s>0.0dB</%s>", tag, tag);
    case PMD_CMIX_LEVEL_M15:  return write_line(w, "<%s>-1.5dB</%s>", tag, tag);
    case PMD_CMIX_LEVEL_M30:  return write_line(w, "<%s>-3.0dB</%s>", tag, tag);
    case PMD_CMIX_LEVEL_M45:  return write_line(w, "<%s>-4.5dB</%s>", tag, tag);
    case PMD_CMIX_LEVEL_M60:  return write_line(w, "<%s>-6.0dB</%s>", tag, tag);
    case PMD_CMIX_LEVEL_MINF: return write_line(w, "<%s>-infdB</%s>", tag, tag);
    default:                  return 1;
    }
}


/**
 * @brief write surround downmix scaling level
 */
static
int                                       /** @return 0 on success, 1 on failure */
write_surmixlev
    (writer *w                            /**< [in] writer state */
    ,const char *tag                      /**< [in] tag name */
    ,pmd_surmix_level mixlev              /**< [in] downmix level */
    )
{
    switch (mixlev)
    {
    case PMD_SURMIX_LEVEL_M15:  return write_line(w, "<%s>-1.5dB</%s>", tag, tag);
    case PMD_SURMIX_LEVEL_M30:  return write_line(w, "<%s>-3.0dB</%s>", tag, tag);
    case PMD_SURMIX_LEVEL_M45:  return write_line(w, "<%s>-4.5dB</%s>", tag, tag);
    case PMD_SURMIX_LEVEL_M60:  return write_line(w, "<%s>-6.0dB</%s>", tag, tag);
    case PMD_SURMIX_LEVEL_MINF: return write_line(w, "<%s>-infdB</%s>", tag, tag);
    default:                    return 1;
    }
}


/**
 * @brief write height mixing level
 */
static
int                                       /** @return 0 on success, 1 on failure */
write_hmixlev
    (writer *w                            /**< [in] writer state */
    ,unsigned char hmixlev                /**< [in] heights downmix level */
    )
{
    return (hmixlev == 31)
        ? write_line(w, "<HMixLev>-infdB</HMixLev>")
        : (hmixlev == 0)
        ? write_line(w, "<HMixLev>0.0dB</HMixLev>")
        : write_line(w, "<HMixLev>-%u.0dB</HMixLev>", hmixlev);
}



/**
 * @brief write optional bitstream parameters in EAC3 encoding params (if present)
 */
static
int                                     /** @return 0 on success, 1 on failure */
write_optional_eep_bitstream_params
    (writer *w                          /**< [in] writer state */
    ,pmd_eep *eep  /**< [in] EAC3 encoder parameters */
    )
{
    if (eep->options & PMD_EEP_BITSTREAM_PRESENT)
    {
        return write_line(w, "<Bitstream>")
            || write_indent(w)
            || write_bsmod(w, eep->bsmod)
            || write_dsurmod(w, eep->dsurmod)
            || write_line(w, "<Dialnorm>%u</Dialnorm>", eep->dialnorm)
            || write_dmixmod(w, eep->dmixmod)
            || write_cmixlev(w, "LtRtCMixLev", eep->ltrtcmixlev)
            || write_surmixlev(w, "LtRtSurMixLev", eep->ltrtsurmixlev)
            || write_cmixlev(w, "LoRoCMixLev", eep->lorocmixlev)
            || write_surmixlev(w, "LoRoSurMixLev", eep->lorosurmixlev)
            || write_outdent(w)
            || write_line(w, "</Bitstream>");
    }
    return 0;
}


/**
 * @brief write optional encoding params in EAC3 encoding params (if present)
 */
static
int                                    /** @return 0 on success, 1 on failure */
write_optional_eep_encoding_params
    (writer *w                         /**< [in] writer state */
    ,pmd_eep *eep /**< [in] EAC3 encoding parameters */
    )
{
    if (eep->options & PMD_EEP_ENCODER_PRESENT)
    {
        return write_line(w, "<Encoder>")
            || write_indent(w)
            || write_compr(w, "DynrngProf", eep->dynrng_prof)
            || write_compr(w, "ComprProf", eep->compr_prof)
            || write_line(w, "<Surround90>%s</Surround90>", eep->surround90 ? "On" : "Off")
            || write_hmixlev(w, eep->hmixlev)
            || write_outdent(w)
            || write_line(w, "</Encoder>");
    }
    return 0;
}


/**
 * @brief write optional DRC struct in EAC3 encoding parameters struct if present
 */
static
int                                    /** @return 0 on success, 1 on failure */
write_optional_eep_drc
    (writer *w                         /**< [in] writer state */
    ,pmd_eep *eep /**< [in] EAC3 encoding parameters */
    )
{
    if (eep->options & PMD_EEP_DRC_PRESENT)
    {
        return write_line(w, "<DRC>")
            || write_indent(w)
            || write_compr(w, "Portable_Speakers_DRC_Profile",   eep->drc_port_spkr)
            || write_compr(w, "Portable_Headphones_DRC_Profile", eep->drc_port_hphone)
            || write_compr(w, "Flat_Panel_DRC_Profile",          eep->drc_flat_panl)
            || write_compr(w, "Home_Theater_DRC_Profile",        eep->drc_home_thtr)
            || write_compr(w, "DDPlus_DRC_Profile",              eep->drc_ddplus)
            || write_outdent(w)
            || write_line(w, "</DRC>");
    }
    return 0;
}


/**
 * @brief #write_iterator function to write presentation ids
 */
static
int                             /** @return 0 on failure, sizeof(pmd_apd_id) on success*/
write_presentation_id_iterator
    (writer *w                  /**< [in] writer state */
    ,void *arg                  /**< [in] ptr to presentation id */
    )
{
    pmd_presentation_id pres_idx = *(pmd_presentation_id*)arg;
    const dlb_pmd_model *model = w->model;

    if (pres_idx >= model->num_apd)
    {
        error(w->model, "illegal model: no such presentation: %u", pres_idx);
        return sizeof(pmd_presentation_id);
    }
    if (write_line(w, "<ID>%u</ID>", model->apd_list[pres_idx].id))
    {
        error(w->model, "failed to write presentation id %u\n", model->apd_list[pres_idx].id);
        return 0;
    }
    return sizeof(pmd_presentation_id);
}


/**
 * @brief write optional eac3 encoding parmeters presentation list
 */
static
int                                     /** @return 0 on success, 1 on failure */
write_optional_eep_presentation_list
    (writer *w                          /**< [in] writer state */
    ,pmd_eep *eep  /**< [in] eac3 encoding parameters containing list */
    )
{
    if (eep->num_presentations > 0)
    {
        return write_line(w, "<Presentations>")
            || write_indent(w)
            || write_foreach(w, eep->presentations, eep->num_presentations,
                             write_presentation_id_iterator)
            || write_outdent(w)
            || write_line(w, "</Presentations>");
    }
    return 0;
}


/**
 * @brief #write_iterator function to write pmd_eep structs
 */
static
int                             /** @return 0 on failure, sizeof(pmd_eep) on success*/
write_eep_iterator
    (writer *w                  /**< [in] writer state */
    ,void *arg                  /**< [in] ptr to pmd_encoder_config */
    )
{
    pmd_eep *eep = (pmd_eep*)arg;

    if (   write_line(w, "<Eac3EncodingParameters id=\"%u\">", eep->id)
        || write_indent(w)
        || write_line(w, "<Name>Eac3EncodingParameters %u</Name>", eep->id)
        || write_optional_eep_encoding_params(w, eep)
        || write_optional_eep_bitstream_params(w, eep)
        || write_optional_eep_drc(w, eep)
        || write_optional_eep_presentation_list(w, eep)
        || write_outdent(w)
        || write_line(w, "</Eac3EncodingParameters>")
      )
    {
        error(w->model, "Failed to write Eac3 Encoding Parameters %u\n", eep->id);
        return 0;
    }
    return sizeof(*eep);
}


/**
 * @brief write Dolby E framerate for ED2 turnaround
 *
 * Note that although PMD supports framerates up to 120 fps for PCM+PMD,
 * for DE/ED2 turnarounds, only 23.98 - 30 fps are supported.
 */
static
int                                /** @return 0 on success, 1 on failure */
write_de_framerate
    (writer *w                     /**< [in] writer state */
    ,dlb_pmd_frame_rate framerate  /**< [in] DE frame rate */
    )
{
    switch (framerate)
    {
        case DLB_PMD_FRAMERATE_2398:  return write_line(w, "<FrameRate>23.98</FrameRate>");
        case DLB_PMD_FRAMERATE_2400:  return write_line(w, "<FrameRate>24</FrameRate>");
        case DLB_PMD_FRAMERATE_2500:  return write_line(w, "<FrameRate>25</FrameRate>");
        case DLB_PMD_FRAMERATE_2997:  return write_line(w, "<FrameRate>29.97</FrameRate>");
        case DLB_PMD_FRAMERATE_3000:  return write_line(w, "<FrameRate>30</FrameRate>");
        default: return 1;
    }
}


/**
 * @brief write Dolby E program config
 */
static
int                              /** @return 0 on success, 1 on failure */
write_de_pgm_config
    (writer *w                   /**< [in] writer state */
    ,unsigned char pgm_config    /**< [in] DE program config */
    )
{
    static const char *pgmcfgs[] =
        { "5.1+2", "5.1+1+1", "4+4", "4+2+2", "4+2+1+1", "4+1+1+1+1",
          "2+2+2+2", "2+2+2+1+1", "2+2+1+1+1+1", "2+1+1+1+1+1+1",
          "1+1+1+1+1+1+1+1", "5.1", "4+2", "4+1+1", "2+2+2", "2+2+1+1",
          "2+1+1+1+1", "1+1+1+1+1+1", "4", "2+2", "2+1+1", "1+1+1+1",
          "7.1", "7.1s"
        };

#define NUM_PGMCFGS (sizeof(pgmcfgs)/sizeof(pgmcfgs[0]))

    if (pgm_config >= NUM_PGMCFGS)
    {
        return 0;
    }

    return write_line(w, "<ProgramConfiguration>%s</ProgramConfiguration>",
                      pgmcfgs[pgm_config]);
}


/**
 * @brief #write_iterator function to write turnaround structs
 */
static
int                             /** @return 0 on failure, sizeof(turnaround) on success*/
write_turnaround_iterator
    (writer *w                  /**< [in] writer state */
    ,void *arg                  /**< [in] ptr to turnaround */
    )
{
    const pmd_apd *apd_list = w->model->apd_list;
    const pmd_eep *eep_list = w->model->eep_list;
    turnaround *t = (turnaround*)arg;

    if (   write_line(w, "<Presentation>")
        || write_indent(w)
        || write_line(w, "<ID>%u</ID>", apd_list[t->presid].id)
        || write_line(w, "<Eac3EncodingParameters>%u</Eac3EncodingParameters>", eep_list[t->eepid].id)
        || write_outdent(w)
        || write_line(w, "</Presentation>")
      )
    {
        error(w->model, "Failed to write ED2 turnaround for APD %u and EEP %u\n", apd_list[t->presid].id,
              eep_list[t->eepid].id);
        return 0;
    }
    return sizeof(*t);
}


/**
 * @brief write optional ED2 turnaround info (if present)
 */
static
int                           /** @return 0 on success, 1 on failure */
write_optional_ed2
    (writer *w                /**< [in] writer state */
    ,pmd_etd *etd  /**< [in] Turnaround */
    )
{
    if (etd->ed2_presentations > 0)
    {
        return write_line(w, "<ED2>")
            || write_indent(w)
            || write_de_framerate(w, etd->ed2_framerate)
            || write_line(w, "<Presentations>")
            || write_indent(w)
            || write_foreach(w, etd->ed2_turnaround, etd->ed2_presentations,
                             write_turnaround_iterator)
            || write_outdent(w)
            || write_line(w, "</Presentations>")
            || write_outdent(w)
            || write_line(w, "</ED2>");
    }
    return 0;
}


/**
 * @brief write optional Dolby E turnaround info (if present)
 */
static
int                           /** @return 0 on success, 1 on failure */
write_optional_de
    (writer *w                /**< [in] writer state */
    ,pmd_etd *etd  /**< [in] Turnaround */
    )
{
    if (etd->de_presentations > 0)
    {
        return write_line(w, "<DolbyE>")
            || write_indent(w)
            || write_de_framerate(w, etd->de_framerate)
            || write_de_pgm_config(w, etd->pgm_config)
            || write_line(w, "<Presentations>")
            || write_indent(w)
            || write_foreach(w, etd->de_turnaround, etd->de_presentations,
                             write_turnaround_iterator)
            || write_outdent(w)
            || write_line(w, "</Presentations>")
            || write_outdent(w)
            || write_line(w, "</DolbyE>");
    }
    return 0;
}


/**
 * @brief #write_iterator function to write pmd_etd structs
 */
static
int                             /** @return 0 on failure, sizeof(pmd_etd) on success*/
write_etd_iterator
    (writer *w                  /**< [in] writer state */
    ,void *arg                  /**< [in] ptr to pmd_etd */
    )
{
    pmd_etd *etd = (pmd_etd*)arg;

    if (   write_line(w, "<ED2Turnaround id=\"%u\">", etd->id)
        || write_indent(w)
        || write_line(w, "<Name>ED2 Turnaround %u</Name>", etd->id)
        || write_optional_ed2(w, etd)
        || write_optional_de(w, etd)
        || write_outdent(w)
        || write_line(w, "</ED2Turnaround>")
      )
    {
        error(w->model, "failed to write ED2 turnaround %u\n", etd->id);
        return 0;
    }
    return sizeof(*etd);
}


/**
 * @brief write array of encoder configs
 */
static
int                                 /** @return 0 on success, 1 on failure */
write_encoder_configs
    (writer *w                      /**< [in] writer state */
    ,const dlb_pmd_model *model     /**< [in] model */
    )
{
    return write_line(w, "<EncoderConfigurations>")
        || write_indent(w)
        || write_foreach(w, model->eep_list, model->num_eep, write_eep_iterator)
        || write_foreach(w, model->etd_list, model->num_etd, write_etd_iterator)
        || write_outdent(w)
        || write_line(w, "</EncoderConfigurations>");
}


/**
 * @brief #write_iterator function to write pmd_audio_update structs
 */
static
int                             /** @return 0 on failure, sizeof(pmd_audio_update) on success*/
write_update_iterator
    (writer *w                  /**< [in] writer state */
    ,void *arg                  /**< [in] ptr to pmd_audio_update */
    )
{
    const pmd_element *element_list = w->model->element_list;
    pmd_xyz *u = (pmd_xyz*)arg;

    if (   write_line(w, "<DynamicUpdate sample_time=\"%u\">", u->time*32)
        || write_indent(w)
        || write_line(w, "<ID>%u</ID>", element_list[u->obj_idx].id)
        || write_coordinate(w, "X_Pos", u->x)
        || write_coordinate(w, "Y_Pos", u->y)
        || write_coordinate(w, "Z_Pos", u->z)
        || write_outdent(w)
        || write_line(w, "</DynamicUpdate>")
      )
    {
        error(w->model, "failed to write Dynamic Update (XYZ) at time %u for element %u\n",
              u->time*32, element_list[u->obj_idx].id);
        return 0;
    }
    return sizeof(*u);
}


/**
 * @brief write array of dynamic audio updates
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_audio_updates
    (writer *w                      /**< [in] writer state */
    ,const pmd_xyz *update          /**< [in] array of audio updates */
    ,unsigned int num               /**< [in] number of audio updates */

    )
{
    return write_foreach(w, update, num, write_update_iterator);
}


/**
 * @brief write IAT Content ID, if present
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_iat_content_id
    (writer *w                      /**< [in] writer state */
    ,const pmd_iat *iat             /**< [in] IAT structure to write */
    )
{
    if (iat->content_id_size)
    {
        char tmp[1024];
        size_t tmpsize = sizeof(tmp);
        pmd_bool hex = 0;

        /* ensure all encoded strings are null-terminated */
        memset(tmp, '\0', sizeof(tmp));

        switch (iat->content_id_type)
        {
        case PMD_IAT_CONTENT_ID_UUID:
            assert(iat->content_id_size == 16);
            write_uuid(iat->content_id, tmp);
            return write_line(w, "<Content_ID><UUID>%s</UUID></Content_ID>", tmp);

        case PMD_IAT_CONTENT_ID_EIDR:
            assert(iat->content_id_size == 12);
            write_eidr(iat->content_id, tmp);
            return write_line(w, "<Content_ID><EIDR>%s</EIDR></Content_ID>", tmp);

        case PMD_IAT_CONTENT_ID_AD_ID:
            write_ad_id(iat->content_id, tmp);
            return write_line(w, "<Content_ID><Ad-ID>%s</Ad-ID></Content_ID>", tmp);

        default:
            if (encode_cdata(iat->content_id, iat->content_id_size, tmp, &tmpsize, &hex))
            {
                return 1;
            }
            return write_line(w,
                              "<Content_ID><Raw type=\"%d\"><%s>%s</%s></Raw></Content_ID>",
                              iat->content_id_type,
                              hex ? "base16" : "ascii",
                              tmp,
                              hex ? "base16" : "ascii");
        }
    }
    return 0;
}


/**
 * @brief write IAT Distribution ID, ATSC 3.0 VP1 Channel ID format
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_iat_distribution_id_atsc3
    (writer *w                      /**< [in] writer state */
    ,const pmd_iat *iat             /**< [in] IAT structure to write */
    )
{
    const uint8_t *id = iat->distribution_id;
    uint16_t bsid = ((uint16_t)id[0])<<8 | id[1];
    uint16_t maj  = (((uint16_t)id[2] & 0x0f)<<6) | ((id[3] >> 2) & 0x3f);
    uint16_t min  = (((uint16_t)id[3] & 0x03)<<8) | id[4];

    return write_line(w, "<Distribution_ID>")
        || write_indent(w)
        || write_line(w, "<ATSC3>")
        || write_indent(w)
        || write_line(w, "<BroadcastStreamID>%u</BroadcastStreamID>", bsid)
        || write_line(w, "<Major_Channel_Number>%u</Major_Channel_Number>", maj)
        || write_line(w, "<Minor_Channel_Number>%u</Minor_Channel_Number>", min)
        || write_outdent(w)
        || write_line(w, "</ATSC3>")
        || write_outdent(w)
        || write_line(w, "</Distribution_ID>");
}


/**
 * @brief write IAT Distribution ID, if present
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_iat_distribution_id
    (writer *w                      /**< [in] writer state */
    ,const pmd_iat *iat             /**< [in] IAT structure to write */
    )
{
    if (iat->distribution_id_size)
    {
        char tmp[1024];
        size_t tmpsize = sizeof(tmp);
        pmd_bool hex = 0;

        memset(tmp, '\0', sizeof(tmp));

        switch (iat->distribution_id_type)
        {
        case PMD_IAT_DISTRIBUTION_ID_ATSC3:
            /* ATSC3 channel id is fixed at 40 bits */
            assert(iat->distribution_id_size == 5);
            return write_iat_distribution_id_atsc3(w, iat);

        default:
            if (encode_cdata(iat->distribution_id,
                             iat->distribution_id_size,
                             tmp,
                             &tmpsize,
                             &hex))
            {
                return 1;
            }
            return write_line(w,
                              "<Distribution_ID><Raw type=\"%d\"><%s>%s</%s></Raw></Distribution_ID>",
                              iat->distribution_id_type,
                              hex ? "base16" : "ascii",
                              tmp,
                              hex ? "base16" : "ascii");
        }
    }
    return 0;
}


/**
 * @brief write IAT Timestamp
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_iat_timestamp
    (writer *w                      /**< [in] writer state */
    ,const pmd_iat *iat             /**< [in] IAT structure to write */
    )
{
    return write_line(w, "<Timestamp>%"PRIu64"</Timestamp>", iat->timestamp);
}


/**
 * @brief write IAT Offset, if present
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_iat_offset
    (writer *w                      /**< [in] writer state */
    ,const pmd_iat *iat             /**< [in] IAT structure to write */
    )
{
    if (iat->options & PMD_IAT_OFFSET_PRESENT)
    {
        return write_line(w, "<Offset>%u</Offset>", iat->offset);
    }
    return 0;
}


/**
 * @brief write IAT Validity Duration, if present
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_iat_validity_duration
    (writer *w                      /**< [in] writer state */
    ,const pmd_iat *iat             /**< [in] IAT structure to write */
    )
{
    if (iat->options & PMD_IAT_VALIDITY_DUR_PRESENT)
    {
        return write_line(w, "<Validity_Duration>%u</Validity_Duration>", iat->validity_duration);
    }
    return 0;
}


/**
 * @brief write IAT User Data, if present
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_iat_user_data
    (writer *w                      /**< [in] writer state */
    ,const pmd_iat *iat             /**< [in] IAT structure to write */
    )
{
    if (iat->user_data_size)
    {
        char tmp[1024];
        size_t tmpsize = sizeof(tmp);
        pmd_bool hex = 0;

        /* ensure all encoded strings are null-terminated */
        memset(tmp, '\0', sizeof(tmp));

        if (encode_cdata(iat->user_data, iat->user_data_size, tmp, &tmpsize, &hex))
        {
            return 1;
        }

        return write_line(w, "<User_Data><%s>%s</%s></User_Data>",
                          hex ? "base16" : "ascii",
                          tmp,
                          hex ? "base16" : "ascii");

    }
    return 0;
}


/**
 * @brief write IAT Extension, if present
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_iat_extension
    (writer *w                      /**< [in] writer state */
    ,const pmd_iat *iat             /**< [in] IAT structure to write */
    )
{
    if (iat->extension_size)
    {
        char tmp[1024];
        size_t tmpsize = sizeof(tmp);
        pmd_bool hex = 0;

        /* ensure all encoded strings are null-terminated */
        memset(tmp, '\0', sizeof(tmp));

        if (encode_cdata(iat->extension_data, iat->extension_size, tmp, &tmpsize, &hex))
        {
            return 1;
        }
        return write_line(w, "<Extension><%s>%s</%s></Extension>",
                          hex ? "base16" : "ascii",
                          tmp,
                          hex ? "base16" : "ascii");
    }
    return 0;
}


/**
 * @brief write IAT, if present
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_iat
    (writer *w                      /**< [in] writer state */
    ,const pmd_iat *iat             /**< [in] IAT structure to write */
    )
{
    if (iat && (iat->options & PMD_IAT_PRESENT))
    {
        return write_line(w, "<IAT>")
            || write_indent(w)
            || write_iat_content_id(w, iat)
            || write_iat_distribution_id(w, iat)
            || write_iat_timestamp(w, iat)
            || write_iat_offset(w, iat)
            || write_iat_validity_duration(w, iat)
            || write_iat_user_data(w, iat)
            || write_iat_extension(w, iat)
            || write_outdent(w)
            || write_line(w, "</IAT>");
    }
    return 0;
}


dlb_pmd_success
dlb_xmlpmd_write
   (dlb_xmlpmd_get_buffer gb
   ,unsigned int indent
   ,void *cbarg
   ,const dlb_pmd_model *model
   )
{
    dlb_pmd_success res = PMD_SUCCESS;
    writer w;

    pmd_mutex_lock((pmd_mutex*)&model->lock);
    error_reset(model);

    writer_init(&w, model, gb, cbarg, indent);
    if (indent == 0)
    {
        write_line(&w, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    }

    if (   write_line(&w, "<Smpte2109>")
        || write_indent(&w)
        || write_container_config(&w)
        || write_professional_metadata(&w)
        || write_indent(&w)
        || write_string(&w, "Title", NULL, model->title)
        || write_audio_signals(&w, model, model->num_signals)
        || write_audio_elements(&w, model->element_list, model->num_elements)
        || write_audio_presentations(&w, model->apd_list, model->num_apd)
        || write_presentation_loudness(&w, model->pld_list, model->num_pld)
        || write_encoder_configs(&w, model)
        || write_audio_updates(&w, model->xyz_list, model->num_xyz)
        || write_iat(&w, model->iat)
        || write_headphone_descriptions(&w, model->hed_list, model->num_hed)
        || write_outdent(&w)
        || write_line(&w, "</ProfessionalMetadata>")
        || write_outdent(&w)
        || write_line(&w, "</Smpte2109>")
      )
    {
        res = PMD_FAIL;
    }

    writer_finish(&w);

    pmd_mutex_unlock((pmd_mutex*)&model->lock);

    return res;
}
