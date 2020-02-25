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
 * @file dlb_sadm_writer.c
 * @brief generate Serial ADM XML format from sADM model
 */

/* taken from the PMD's own XML reader/writer module */
#include "xml_hex.h"
#include "xml_uuid.h"
#include "xml_eidr.h"
#include "xml_ad_id.h"
#include "xml_cdata.h"

#include "dlb_pmd_xml.h"
#include "pmd_error_helper.h"

#include "sadm/dlb_sadm_writer.h"
#include "memstuff.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>


#if defined(_MSC_VER) 
#  if !defined(PRIu64)
#    define PRIu64 "I64u"
#endif
typedef intptr_t ssize_t;
#else
#  include <inttypes.h>
#endif


/**
 * @def ARRAYSZ(a)
 * @brief compute size of an array
 */
#define ARRAYSZ(a) (sizeof(a)/sizeof(a[0]))


/**
 * @brief state of writer
 */
typedef struct
{
    const dlb_sadm_model *model;   /**< the model being written */
    dlb_xmlpmd_get_buffer getbuf;  /**< get-more-buffer callback */
    void *cbarg;                   /**< client-supplied parameter for #getbuf */
    unsigned int indent;           /**< indentation level */
    char *pos;                     /**< current write position */
    char *end;                     /**< 1st byte after buffer */
} writer;


/**
 * @brief initialize writer state
 */
static
void
writer_init
   (      writer *w                  /**< [in] writer state */
   ,const dlb_sadm_model *model      /**< [in] the model being written */
   ,      dlb_xmlpmd_get_buffer gb   /**< [in] get buffer callback */
   ,      void *cbarg                /**< [in] client-supplied callback parameter */
   ,      unsigned int indent        /**< [in] initial indentation of 1st tag */
   )
{
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

#define TAB "  "
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
        char *buf = NULL;
        size_t capacity;

        static int count = 0;
        ++count;
        
        if (!w->getbuf(w->cbarg, w->pos, &buf, &capacity))
        {
            printf("Could not get buffer %u\n", count);
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
dlb_pmd_success
encode_string
    (writer *w            /**< [in] writer state */
    ,const uint8_t *str   /**< [in] the string to encode */
    ,uint8_t *dest        /**< [in] the destination array to write */
    ,size_t capacity      /**< [in] the capacity of the destination array */
    )
{
    const uint8_t *send;
    uint8_t *wp = dest;
    uint8_t *wend = wp + capacity;

    if (!str || str[0] == 0xff)
    {
        dest[0] = '\0';
    }
    else
    {
        send = str + strlen((char*)str);
        memset(dest, '\0', capacity);
        
        while (wp < wend && str < send)
        {
            uint8_t c = *str;
            
            if ('&' == c)
            {
                if (wp + 5 >= wend) goto fail;
                sprintf((char*)wp, "&amp;");
                wp += 5;
            }
            else if ('<' == c)
            {
                if (wp + 4 >= wend) goto fail;
                sprintf((char*)wp, "&lt;");
                wp += 4;
            }
            else if ('>' == c)
            {
                if (wp + 4 >= wend) goto fail;
                sprintf((char*)wp, "&gt;");
                wp += 4;
            }
            else if ('\"' == c)
            {
                if (wp + 6 >= wend) goto fail;
                sprintf((char*)wp, "&quot;");
                wp += 6;
            }
            else if ('\'' == c)
            {
                if (wp + 6 >= wend) goto fail;
                sprintf((char*)wp, "&apos;");
                wp += 6;
            }
            else
            {
                *wp++ = *str;
            }
            ++str;
        }
        wend[-1] = '\0';
    }
    return PMD_SUCCESS;

  fail:
    dlb_sadm_set_error(w->model, "oops! encoded string capacity too small");
    return PMD_FAIL;
}


/**
 * @brief write an audio channel format reference
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_channel_format_idref
    (writer *w
    ,dlb_sadm_idref idref
    )
{
    dlb_sadm_idref_array blkfmts;
    dlb_sadm_idref blkfmts_array[128];
    dlb_sadm_channel_format chanfmt;

    blkfmts.num = 0;
    blkfmts.max = ARRAYSZ(blkfmts_array);
    blkfmts.array = blkfmts_array;

    return dlb_sadm_channel_format_lookup(w->model, idref, &chanfmt, &blkfmts)
        || write_line(w, "<audioChannelFormatIDRef>%s</audioChannelFormatIDRef>", chanfmt.id.data);
}


/**
 * @brief write an audio content reference
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_content_idref
    (writer *w
    ,dlb_sadm_idref idref
    )
{
    dlb_sadm_content content;
    return dlb_sadm_content_lookup(w->model, idref, &content)
        || write_line(w, "<audioContentIDRef>%s</audioContentIDRef>", content.id.data);
}


/**
 * @brief write an audio object reference
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_object_idref
    (writer *w
    ,dlb_sadm_idref idref
    )
{
    dlb_sadm_object object;
    dlb_sadm_idref_array track_uids;
    dlb_sadm_idref track_uids_array[128];

    track_uids.num = 0;
    track_uids.max = ARRAYSZ(track_uids_array);
    track_uids.array = track_uids_array;

    return dlb_sadm_object_lookup(w->model, idref, &object, &track_uids)
        || write_line(w, "<audioObjectIDRef>%s</audioObjectIDRef>", object.id.data);
}


/**
 * @brief write an audio pack format uid reference
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_pack_format_idref
    (writer *w
    ,dlb_sadm_idref idref
    )
{
    dlb_sadm_pack_format packfmt;
    dlb_sadm_idref_array chanfmts;
    dlb_sadm_idref chanfmts_array[128];
        
    chanfmts.num = 0;
    chanfmts.max = ARRAYSZ(chanfmts_array);
    chanfmts.array = chanfmts_array;

    return dlb_sadm_pack_format_lookup(w->model, idref, &packfmt, &chanfmts)
        || write_line(w, "<audioPackFormatIDRef>%s</audioPackFormatIDRef>", packfmt.id.data);
}


/**
 * @brief write a track uid reference
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_track_uid_idref
    (writer *w
    ,dlb_sadm_idref idref
    )
{
    dlb_sadm_track_uid track_uid;

    return dlb_sadm_track_uid_lookup(w->model, idref, &track_uid)
        || write_line(w, "<audioTrackUIDRef>%s</audioTrackUIDRef>", track_uid.id.data);
}


/**
 * @brief write labels for an sADM audio programme
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_programme_labels
    (writer *w                      /**< [in] writer state */
    ,dlb_sadm_programme *prog       /**< [in] container programme */
    )
{
    dlb_sadm_programme_label *label = prog->labels;
    uint8_t encoded_string[1024];
    unsigned int i;
    
    for (i = 0; i != prog->num_labels; ++i, ++label)
    {
        if (encode_string(w, label->name.data, encoded_string, sizeof(encoded_string)))
        {
            return 1;
        }
        if (write_line(w, "<audioProgrammeLabel language=\"%s\">%s</audioProgrammeLabel>",
                       label->language, encoded_string))
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief write sADM audio contents
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_programme_contents
    (writer *w                      /**< [in] writer state */
    ,dlb_sadm_programme *prog       /**< [in] container programme */
    )
{
    unsigned int i;

    for (i = 0; i != prog->contents.num; ++i)
    {
        if (write_content_idref(w, prog->contents.array[i]))
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief write sADM audio programmes
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_programmes
    (writer *w                      /**< [in] writer state */
    )
{
    dlb_sadm_programme_iterator pi;
    dlb_sadm_programme_label labels[32];
    dlb_sadm_programme prog;
    dlb_sadm_idref_array contents;
    dlb_sadm_idref contents_array[128];
    uint8_t encoded_string[256];    
    char attributes[256];

    if (dlb_sadm_programme_iterator_init(&pi, w->model))
    {
        return 1;
    }

    contents.num = 0;
    contents.max = ARRAYSZ(contents_array);
    contents.array = contents_array;

    while (!dlb_sadm_programme_iterator_next(&pi, &prog, &contents, labels, ARRAYSZ(labels)))
    {
        if (encode_string(w, prog.name.data, encoded_string, sizeof(encoded_string)))
        {
            return 1;
        }
        
        snprintf(attributes, sizeof(attributes),
                 "audioProgrammeID=\"%s\" "
                 "audioProgrammeName=\"%s\" "
                 "audioProgrammeLanguage=\"%s\"",
                 prog.id.data,
                 encoded_string,
                 prog.language);

        if (   write_line(w, "<audioProgramme %s>", attributes)
            || write_indent(w)
            || write_programme_labels(w, &prog)
            || write_programme_contents(w, &prog)
            || write_outdent(w)
            || write_line(w, "</audioProgramme>"))
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief write sADM audio content dialogue line
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_dialogue
    (writer *w                      /**< [in] writer state */
    ,dlb_sadm_content *c            /**< [in] content */
    )
{
    char attributes[256];
    
    if (c->type < DLB_SADM_CONTENT_DK)
    {
        snprintf(attributes, sizeof(attributes), "nonDialogueContentKind=\"%d\"",
                 (int)(c->type - DLB_SADM_CONTENT_NK));
    }
    else if (c->type < DLB_SADM_CONTENT_MK)
    {
        snprintf(attributes, sizeof(attributes), "dialogueContentKind=\"%d\"",
                 (int)(c->type - DLB_SADM_CONTENT_DK));
    }
    else
    {
        snprintf(attributes, sizeof(attributes), "mixedContentKind=\"%d\"",
                 (int)(c->type - DLB_SADM_CONTENT_MK));
    }
    
    return write_line(w, "<dialogue %s>%d</dialogue>", attributes, c->dialogue_value);
}


/**
 * @brief write sADM audio contents
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_contents
    (writer *w                      /**< [in] writer state */
    )
{
    dlb_sadm_content_iterator ci;
    dlb_sadm_content content;
    uint8_t encoded_string[256];
    char attributes[256];

    if (dlb_sadm_content_iterator_init(&ci, w->model))
    {
        return 1;
    }
    
    while (!dlb_sadm_content_iterator_next(&ci, &content))
    {
        if (encode_string(w, content.name.data, encoded_string, sizeof(encoded_string)))
        {
            return 1;
        }
        snprintf(attributes, sizeof(attributes),
                 "audioContentID=\"%s\" "
                 "audioContentName=\"%s\"",
                 content.id.data, encoded_string);

        if (   write_line(w, "<audioContent %s>", attributes)
            || write_indent(w)
            || write_object_idref(w, content.object)
            || write_dialogue(w, &content)
            || write_outdent(w)
            || write_line(w, "</audioContent>"))
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief write the track UIDs associated to an object
 */
static inline
int                                   /** @return 0 on success, 1 on failure */
write_object_track_uids
    (writer *w                        /**< [in] writer state */
    ,dlb_sadm_idref_array *track_uids /**< [in] array of track_uid idrefs to write */
    )
{
    unsigned int i;
    
    for (i = 0; i != track_uids->num; ++i)
    {
        if (write_track_uid_idref(w, track_uids->array[i]))
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief write sADM audio object gain value
 */
static inline
int                                   /** @return 0 on success, 1 on failure */
write_gain
    (writer *w                        /**< [in] writer state */
    ,float gain
    )
{
    if (isinf(gain) && gain  < 0.0f)
    {
        return write_line(w, "<gain gainUnit=\"dB\">-999</gain>");
    }
    else
    {
        return write_line(w, "<gain gainUnit=\"dB\">%f</gain>", gain);
    }
}


/**
 * @brief write sADM audio objects
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_objects
    (writer *w                      /**< [in] writer state */
    )
{
    dlb_sadm_object_iterator oi;
    dlb_sadm_idref_array track_uids;
    dlb_sadm_idref track_uids_array[128];
    dlb_sadm_object object;
    uint8_t encoded_string[256];    

    track_uids.num = 0;
    track_uids.max = ARRAYSZ(track_uids_array);
    track_uids.array = track_uids_array;

    if (dlb_sadm_object_iterator_init(&oi, w->model))
    {
        return 1;
    }

    while (!dlb_sadm_object_iterator_next(&oi, &object, &track_uids))
    {
#ifndef NDEBUG
        {
            dlb_sadm_pack_format packfmt;
            dlb_sadm_idref_array chanfmts;
            dlb_sadm_idref chanfmts_array[128];
            
            chanfmts.num = 0;
            chanfmts.max = ARRAYSZ(chanfmts_array);
            chanfmts.array = chanfmts_array;
            
            assert(!dlb_sadm_pack_format_lookup(w->model, object.pack_format, &packfmt, &chanfmts));
            assert(object.track_uids.num == packfmt.chanfmts.num);
        }
#endif
        if (encode_string(w, object.name.data, encoded_string, sizeof(encoded_string)))
        {
            return 1;
        }
        
        if (   write_line(w, "<audioObject audioObjectID=\"%s\" audioObjectName=\"%s\">",
                          object.id.data, encoded_string)
            || write_indent(w)
            || write_gain(w, object.gain)
            || write_pack_format_idref(w, object.pack_format)
            || write_object_track_uids(w, &object.track_uids)
            || write_outdent(w)
            || write_line(w, "</audioObject>"))
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief write the audio channel format references for an audio pack format
 */
static inline
int                                   /** @return 0 on success, 1 on failure */
write_packfmt_chanfmts
    (writer *w                        /**< [in] writer state */
    ,dlb_sadm_idref_array *chanfmts   /**< [in] array of channel format idrefs to write */
    )
{
    unsigned int i;
    
    for (i = 0; i != chanfmts->num; ++i)
    {
        if (write_channel_format_idref(w, chanfmts->array[i]))
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief write sADM audio pack formats
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_pack_formats
    (writer *w                      /**< [in] writer state */
    )
{
    dlb_sadm_pack_format_iterator pfi;
    dlb_sadm_pack_format packfmt;
    dlb_sadm_idref_array chanfmts;
    dlb_sadm_idref chanfmts_array[128];
    uint8_t encoded_string[1024];
    char attributes[256];
    
    chanfmts.num = 0;
    chanfmts.max = ARRAYSZ(chanfmts_array);
    chanfmts.array = chanfmts_array;

    if (dlb_sadm_pack_format_iterator_init(&pfi, w->model))
    {
        return 1;
    }

    while (!dlb_sadm_pack_format_iterator_next(&pfi, &packfmt, &chanfmts))
    {
        char type_definition[128];

        switch (packfmt.type)
        {
            case DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS:
                snprintf(type_definition, sizeof(type_definition), "DirectSpeakers");
                break;
            case DLB_SADM_PACKFMT_TYPE_OBJECT:
                snprintf(type_definition, sizeof(type_definition), "Objects");
                break;                
            default:
                return 1;
        }

        if (encode_string(w, packfmt.name.data, encoded_string, sizeof(encoded_string)))
        {
            return 1;
        }
        snprintf(attributes, sizeof(attributes),
                 "audioPackFormatID=\"%s\" "
                 "audioPackFormatName=\"%s\" "
                 "typeLabel=\"%04x\" "
                 "typeDefinition=\"%s\"",
                 packfmt.id.data, encoded_string,
                 packfmt.type,
                 type_definition);

        if (   write_line(w, "<audioPackFormat %s>", attributes)
            || write_indent(w)
            || write_packfmt_chanfmts(w, &packfmt.chanfmts)
            || write_outdent(w)
            || write_line(w, "</audioPackFormat>")
           )
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief write sADM audio block format speaker label (or cartesian tag)
 */
static inline
int                                   /** @return 0 on success, 1 on failure */
write_speaker_label
    (writer *w                        /**< [in] writer state */
    ,dlb_sadm_block_format *blkfmt
    )
{
    if (blkfmt->speaker_label[0])
    {
        return write_line(w, "<speakerLabel>%s</speakerLabel>", blkfmt->speaker_label);
    }
    else
    {
        return write_line(w, "<cartesian>1</cartesian>");
    }
}


/**
 * @brief write sADM audio block format coordinate
 */
static inline
int                                   /** @return 0 on success, 1 on failure */
write_position
    (writer *w                        /**< [in] writer state */
    ,const char *label
    ,float val
    )
{
    return write_line(w, "<position coordinate=\"%s\">%f</position>", label, val);
}


/**
 * @brief write sADM audio block formats
 */
static inline
int                                   /** @return 0 on success, 1 on failure */
write_block_formats
    (writer *w                        /**< [in] writer state */
    ,dlb_sadm_idref_array *blkfmts    /**< [in] container channel format */
    )
{
    dlb_sadm_block_format blkfmt;
    unsigned int i;

    for (i = 0; i != blkfmts->num; ++i)
    {
        if (   dlb_sadm_block_format_lookup(w->model, blkfmts->array[i], &blkfmt)
            || write_line(w, "<audioBlockFormat audioBlockFormatID=\"%s\">", blkfmt.id.data)
            || write_indent(w)
            || write_speaker_label(w, &blkfmt)
            || write_position(w, "X", blkfmt.azimuth_or_x)
            || write_position(w, "Y", blkfmt.elevation_or_y)
            || write_position(w, "Z", blkfmt.distance_or_z)
            || write_outdent(w)
            || write_line(w, "</audioBlockFormat>")
            )
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief infer the audio type (pack format) from the id
 */
static
int                                     /** @return 0 on success, 1 on failure */
infer_audio_type_from_id
    (dlb_sadm_id            id          /**< [in]  id from which to infer the audio type */
    ,dlb_sadm_packfmt_type *audio_type  /**< [out] audio type return value */
    )
{
    int result = 0;
    char type_string[5];    /* TODO: symbolic constants */
    size_t first = 3;
    const size_t len = 4;
    int n;

    type_string[4] = '\0';
    if (id.data[first] == '_')
    {
        first++;
    }
    strncpy(type_string, (const char *)id.data + first, len);
    n = atoi(type_string);
    switch (n)
    {
    case 1:
    case 2:
    case 3:
        if (audio_type != NULL)
        {
            *audio_type = (dlb_sadm_packfmt_type)n;
            break;
        }
        /*** FALL THROUGH for "else" case ***/

    default:
        result = 1;
        break;
    }

    return result;
}
                

/**
 * @brief write sADM audio channel formats
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_channel_formats
    (writer *w                      /**< [in] writer state */
    )
{
    dlb_sadm_channel_format_iterator cfi;
    dlb_sadm_channel_format chanfmt;
    dlb_sadm_idref_array blkfmts;
    dlb_sadm_idref blkfmts_array[128];
    uint8_t encoded_string[256];    
    char attributes[256];

    blkfmts.num = 0;
    blkfmts.max = ARRAYSZ(blkfmts_array);
    blkfmts.array = blkfmts_array;

    if (dlb_sadm_channel_format_iterator_init(&cfi, w->model))
    {
        return 1;
    }

    while (!dlb_sadm_channel_format_iterator_next(&cfi, &chanfmt, &blkfmts))
    {
        dlb_sadm_packfmt_type audio_type;
        char type_definition[128];

        if (   encode_string(w, chanfmt.name.data, encoded_string, sizeof(encoded_string))
            || infer_audio_type_from_id(chanfmt.id, &audio_type))
        {
            return 1;
        }

        switch (audio_type)
        {
        case DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS:
            snprintf(type_definition, sizeof(type_definition), "DirectSpeakers");
            break;
        case DLB_SADM_PACKFMT_TYPE_OBJECT:
            snprintf(type_definition, sizeof(type_definition), "Objects");
            break;
        default:
            return 1;
        }
        
        snprintf(attributes, sizeof(attributes),
                 "audioChannelFormatID=\"%s\" "
                 "audioChannelFormatName=\"%s\" "
                 "typeLabel=\"%04x\" "
                 "typeDefinition=\"%s\"",
                 chanfmt.id.data, encoded_string, audio_type, type_definition);
        
        if (   write_line(w, "<audioChannelFormat %s>", attributes)
            || write_indent(w)
            || write_block_formats(w, &chanfmt.blkfmts)
            || write_outdent(w)
            || write_line(w, "</audioChannelFormat>")
           )
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief write sADM track UIDs
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_track_uids
    (writer *w                      /**< [in] writer state */
    )
{
    dlb_sadm_track_uid_iterator ti;
    dlb_sadm_track_uid track_uid;

    if (dlb_sadm_track_uid_iterator_init(&ti, w->model))
    {
        return 1;
    }

    while (!dlb_sadm_track_uid_iterator_next(&ti, &track_uid))
    {
        if (   write_line(w, "<audioTrackUID UID=\"%s\">", track_uid.id.data)
            || write_indent(w)
            || write_channel_format_idref(w, track_uid.chanfmt)
            || write_pack_format_idref(w, track_uid.packfmt)            
            || write_outdent(w)
            || write_line(w, "</audioTrackUID>")
           )
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief write source notice
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_source_notice
    (writer *w                      /**< [in] writer state */
    )
{
    char version_string[128];
    unsigned int epoch;
    unsigned int maj;
    unsigned int min;
    unsigned int build;
    unsigned int y, z;

    dlb_pmd_library_version(&epoch, &maj, &min, &build, &y, &z);
    snprintf(version_string, 128, "<!--      This file is generated by the PMD Library v%u.%u.%u.%u      -->", epoch, maj, min, build);

    return write_line(w, version_string)
        || write_line(w, "<!--**************************************************************-->")
        || write_line(w, "<!--**** WARNING This file contains draft BS.2076-2 elements *****-->")
        || write_line(w, "<!--****        DO NOT USE OUTSIDE OF A TEST ENVIROMENT      *****-->")
        || write_line(w, "<!--**************************************************************-->");
}


/**
 * @brief write sADM core metadata
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_core_metadata
    (writer *w                      /**< [in] writer state */
    )
{
    return write_line(w, "<audioFormatExtended version=\"ITU-R_BS.2076-2\">")
        || write_indent(w)
        || write_programmes(w)
        || write_contents(w)
        || write_objects(w)
        || write_pack_formats(w)
        || write_channel_formats(w)
        || write_track_uids(w)
        || write_outdent(w)
        || write_line(w, "</audioFormatExtended>");
}


/**
 * @brief write sADM frame header's programme idrefs
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_changed_ids_programmes
    (writer *w                      /**< [in] writer state */
    )
{
    dlb_sadm_programme_label labels[32];
    dlb_sadm_idref_array contents;
    dlb_sadm_idref contents_array[128];
    dlb_sadm_programme_iterator pi;
    dlb_sadm_programme prog;

    if (dlb_sadm_programme_iterator_init(&pi, w->model))
    {
        return 1;
    }

    contents.num = 0;
    contents.max = ARRAYSZ(contents_array);
    contents.array = contents_array;

    while (!dlb_sadm_programme_iterator_next(&pi, &prog, &contents, labels, ARRAYSZ(labels)))
    {
        if (write_line(w, "<audioProgrammeIDRef status=\"new\">%s</audioProgrammeIDRef>",
                       prog.id.data))
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief write sADM frame header's content idrefs
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_changed_ids_contents
    (writer *w                      /**< [in] writer state */
    )
{
    dlb_sadm_content_iterator ci;
    dlb_sadm_content content;

    if (dlb_sadm_content_iterator_init(&ci, w->model))
    {
        return 1;
    }

    while (!dlb_sadm_content_iterator_next(&ci, &content))
    {
        if (write_line(w, "<audioContentIDRef status=\"new\">%s</audioContentIDRef>",
                       content.id.data))
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief write sADM frame header's object idrefs
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_changed_ids_objects
    (writer *w                      /**< [in] writer state */
    )
{
    dlb_sadm_object_iterator oi;
    dlb_sadm_object object;
    dlb_sadm_idref_array track_uids;
    dlb_sadm_idref track_uids_array[128];

    if (dlb_sadm_object_iterator_init(&oi, w->model))
    {
        return 1;
    }

    track_uids.num = 0;
    track_uids.max = sizeof(track_uids_array)/sizeof(track_uids_array[0]);
    track_uids.array = track_uids_array;

    while (!dlb_sadm_object_iterator_next(&oi, &object, &track_uids))
    {
        if (write_line(w, "<audioObjectIDRef status=\"new\">%s</audioObjectIDRef>",
                       object.id.data))
        {
            return 1;
        }
    }
    return 0;
}



/**
 * @brief write sADM frame header's pack format idrefs
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_changed_ids_pack_formats
    (writer *w                      /**< [in] writer state */
    )
{
    dlb_sadm_pack_format_iterator pfi;
    dlb_sadm_pack_format packfmt;
    dlb_sadm_idref_array chanfmts;
    dlb_sadm_idref chanfmts_array[128];

    chanfmts.num = 0;
    chanfmts.max = sizeof(chanfmts_array)/sizeof(chanfmts_array[0]);
    chanfmts.array = chanfmts_array;

    if (dlb_sadm_pack_format_iterator_init(&pfi, w->model))
    {
        return 1;
    }

    while (!dlb_sadm_pack_format_iterator_next(&pfi, &packfmt, &chanfmts))
    {
        if (write_line(w, "<audioPackFormatIDRef status=\"new\">%s</audioPackFormatIDRef>",
                       packfmt.id.data))
        {
            return 1;
        }
    }

    return 0;
}


/**
 * @brief write sADM frame header's channel format idrefs
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_changed_ids_channel_formats
    (writer *w                      /**< [in] writer state */
    )
{
    dlb_sadm_channel_format_iterator cfi;
    dlb_sadm_channel_format chanfmt;
    dlb_sadm_idref_array blkfmts;
    dlb_sadm_idref blkfmts_array[128];

    blkfmts.num = 0;
    blkfmts.max = sizeof(blkfmts_array)/sizeof(blkfmts_array[0]);
    blkfmts.array = blkfmts_array;

    if (dlb_sadm_channel_format_iterator_init(&cfi, w->model))
    {
        return 1;
    }

    while (!dlb_sadm_channel_format_iterator_next(&cfi, &chanfmt, &blkfmts))
    {
        if (write_line(w, "<audioChannelFormatIDRef status=\"new\">%s</audioChannelFormatIDRef>",
                       chanfmt.id.data))
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief write sADM frame header's track UID idrefs
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_changed_ids_track_uids
    (writer *w                      /**< [in] writer state */
    )
{
    dlb_sadm_track_uid_iterator ti;
    dlb_sadm_track_uid track_uid;

    if (dlb_sadm_track_uid_iterator_init(&ti, w->model))
    {
        return 1;
    }

    while (!dlb_sadm_track_uid_iterator_next(&ti, &track_uid))
    {
        if (write_line(w, "<audioTrackUIDRef status=\"new\">%s</audioTrackUIDRef>",
                       track_uid.id.data))
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief write sADM frame header's changed ids block
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_changed_ids
    (writer *w                      /**< [in] writer state */
    )
{
    return write_line(w, "<changedIDs>")
        || write_indent(w)
        || write_changed_ids_programmes(w)
        || write_changed_ids_contents(w)
        || write_changed_ids_objects(w)           
        || write_changed_ids_pack_formats(w)
        || write_changed_ids_channel_formats(w)
        || write_changed_ids_track_uids(w)
        || write_outdent(w)
        || write_line(w, "</changedIDs>");
}


/**
 * @brief write sADM frame header's transport track format line
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_transport_track_format
    (writer *w                      /**< [in] writer state */
    )
{
    dlb_sadm_track_uid_iterator ti;
    dlb_sadm_track_uid track_uid;
    dlb_sadm_counts sc;
    char attributes[256];

    if (dlb_sadm_model_counts(w->model, &sc))
    {
        return 1;
    }
    
    snprintf(attributes, sizeof(attributes),
             "transportID=\"TP_0001\" "
             "transportName=\"X\" "
             "numIDs=\"%d\" "
             "numTracks=\"%d\"",
             (int)sc.num_track_uids,
             (int)sc.num_track_uids);

    if (   write_line(w, "<transportTrackFormat %s>", attributes)
        || write_indent(w)
        || dlb_sadm_track_uid_iterator_init(&ti, w->model)
        )
    {
        return 1;
    }
    
    while (!dlb_sadm_track_uid_iterator_next(&ti, &track_uid))
    {
        if (   write_line(w, "<audioTrack trackID=\"%d\">", track_uid.channel_idx)
            || write_indent(w)
            || write_line(w, "<audioTrackUIDRef>%s</audioTrackUIDRef>", track_uid.id.data)
            || write_outdent(w)
            || write_line(w, "</audioTrack>"))
        {
            return 1;
        }
    }

    return write_outdent(w)
        || write_line(w, "</transportTrackFormat>");
}


/**
 * @brief write sADM frame header's frame format line
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_frame_format
    (writer *w                      /**< [in] writer state */
    )
{
    char attributes[256];

    snprintf(attributes, sizeof(attributes),
             "frameFormatID=\"FF_00000001\" "
             "type=\"header\" "
             "start=\"00:00:00.00000\" "
             "duration=\"00:00:00.02000\"");  /* todo: check duration frame rate */

    return write_line(w, "<frameFormat %s>", attributes)
        || write_indent(w)
        || write_changed_ids(w)
        || write_transport_track_format(w)
        || write_outdent(w)
        || write_line(w, "</frameFormat>");
}



/**
 * @brief write sADM frame header
 */
static inline
int                                 /** @return 0 on success, 1 on failure */
write_frame_header
    (writer *w                      /**< [in] writer state */
    )
{
    return write_line(w, "<frameHeader>")
        || write_indent(w)
        || write_frame_format(w)
        || write_outdent(w)
        || write_line(w, "</frameHeader>");
}


/* -------------------------- public API ----------------------------- */


dlb_pmd_success                        
dlb_sadm_write
   (      dlb_xmlpmd_get_buffer gb
   ,      unsigned int indent
   ,      void *cbarg
   ,const dlb_sadm_model *model
   )
{
    dlb_pmd_success res = PMD_SUCCESS;
    writer w;
    
    dlb_sadm_error_reset(model);

    writer_init(&w, model, gb, cbarg, indent);
    if (indent == 0)
    {
        write_line(&w, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    }

    res =  write_line(&w, "<frame>")
        || write_indent(&w)
        || write_frame_header(&w)
        || write_source_notice(&w)
        || write_core_metadata(&w)
        || write_outdent(&w)
        || write_line(&w, "</frame>") 
        ;

    writer_finish(&w);
    return res;
}
