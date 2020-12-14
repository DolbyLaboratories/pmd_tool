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
 * @file pmd_sadm_generator.c
 * @brief implementation of sADM generation interface
 *
 * 'Dolby' serial ADM is a serial ADM profile, that restricts the type of
 * entities used to create metadata.
 */

#include "dlb_pmd_api.h"
#include "pmd_error_helper.h"
#include "pmd_sadm_generator.h"
#include "xml_uuid.h"
#include <stdio.h>


/**
 * @def ARRAYSZ(a)
 * @brief compute size of an array
 */
#define ARRAYSZ(a) (sizeof(a)/sizeof(a[0]))


/**
 * @def MIN(x,y)
 * @brief return min of x and y.
 *
 * this is not safe to use with side-effecting expressions as arguments
 */
#define MIN(x,y) ((x) < (y) ? (x) : (y))


/**
 * @def ADM_DIALOGUE_VALUE_FOR_BED
 * @brief magic number for ADM's content dialogue attribute value denoting audio beds
 */
#define ADM_DIALOGUE_VALUE_FOR_BED (2)


/**
 * @def ADM_DIALOGUE_VALUE_FOR_OBJECT
 * @brief magic number for ADM's content dialogue attribute value denoting non-generic objects
 */
#define ADM_DIALOGUE_VALUE_FOR_OBJECT (1)


/**
 * @def ADM_DIALOGUE_VALUE_FOR_GENERIC_OBJECT
 * @brief magic number for ADM's content dialogue attribute value denoting generic objects
 */
#define ADM_DIALOGUE_VALUE_FOR_GENERIC_OBJECT (2)


/**
 * @brief table of speaker counts for PMD speaker configs
 */
static unsigned int SPEAKER_CONFIG_COUNT[NUM_PMD_SPEAKER_CONFIGS] =
{
    2, 3, 6, 8, 10, 12, 16, 2, 2
};


/**
 * @brief table of audioPackFormat names for PMD speaker configs
 */
static const char *speaker_config_names[NUM_PMD_SPEAKER_CONFIGS] =
{
    "RoomCentric_2.0",
    "RoomCentric_3.0",
    "RoomCentric_5.1",
    "RoomCentric_5.1.2",
    "RoomCentric_5.1.4",
    "RoomCentric_7.1.4",
    "RoomCentric_9.1.6",
    "PortableSpeakers",
    "Headphones"
};


/**
 * @brief audioPackFormat name for non-standard 7.0.4 config
 */
static const char *speaker_config_7_0_4_name = "RoomCentric_7.0.4";


/**
 * @brief type of block format information used for channels in a bed
 */
typedef struct
{
    char name[64];   /**< audio channel format name */
    char label[32];  /**< dlb sADM channel id */
    float x;         /**< speaker's cartesian x coordinate */
    float y;         /**< speaker's cartesian y coordinate */
    float z;         /**< speaker's cartesian z coordinate */
} speaker_blkfmt;

    
/**
 * @brief sADM speaker block format information for formats using Lss/Rss
 */
static speaker_blkfmt SPEAKER_BLKFMT[PMD_NUM_SPEAKERS] =
{
    /* L */    { "RoomCentricLeft",              "RC_L",   -1.0f,  1.0f,    0.0f },
    /* R */    { "RoomCentricRight",             "RC_R",    1.0f,  1.0f,    0.0f },
    /* C */    { "RoomCentricCenter",            "RC_C",    0.0f,  1.0f,    0.0f },
    /* Lfe */  { "RoomCentricLFE",               "RC_LFE", -1.0f,  1.0f,   -1.0f },
    /* Lss */  { "RoomCentricLeftSideSurround",  "RC_Lss", -1.0f,  0.0f,    0.0f },
    /* Rss */  { "RoomCentricRightSideSurround", "RC_Rss",  1.0f,  0.0f,    0.0f },
    /* Lrs */  { "RoomCentricLeftRearSurround",  "RC_Lrs", -1.0f, -1.0f,    0.0f },
    /* Rrs */  { "RoomCentricRightRearSurround", "RC_Rrs",  1.0f, -1.0f,    0.0f },
    /* Ltf */  { "RoomCentricLeftTopFront",      "RC_Ltf", -1.0f,  1.0f,    1.0f },
    /* Rtf */  { "RoomCentricRightTopFront",     "RC_Rtf",  1.0f,  1.0f,    1.0f },
    /* Ltm */  { "RoomCentricLeftTopMiddle",     "RC_Ltm", -1.0f,  0.0f,    1.0f },
    /* Rtm */  { "RoomCentricRightTopMiddle",    "RC_Rtm",  1.0f,  0.0f,    1.0f },
    /* Ltr */  { "RoomCentricLeftTopRear",       "RC_Ltr", -1.0f, -1.0f,    1.0f },
    /* Rtr */  { "RoomCentricRightTopRear",      "RC_Rtr",  1.0f, -1.0f,    1.0f },
    /* Lw */   { "RoomCentricLeftFrontWide",     "RC_Lfw", -1.0f,  0.677f,  0.0f },
    /* Rw */   { "RoomCentricRightFrontWide",    "RC_Rfw",  1.0f,  0.677f,  0.0f },
};

    
/**
 * @brief sADM speaker block format information for Ls/Rs
 */
static speaker_blkfmt SPEAKER_BLKFMT_LS_RS[] =
{
    /* Ls */   { "RoomCentricLeftSurround",  "RC_Ls", -1.0f,  -1.0f,  0.0f },
    /* Rs */   { "RoomCentricRightSurround", "RC_Rs",  1.0f,  -1.0f,  0.0f },
};


/**
 * @brief map PMD speaker targets to sADM speaker numbers
 */
static const dlb_sadm_speaker PMD_TO_SADM_SPEAKER[PMD_NUM_SPEAKERS] =
{
    /* PMD_SPEAKER_L   */ DLB_SADM_SPEAKER_L,
    /* PMD_SPEAKER_R   */ DLB_SADM_SPEAKER_R,
    /* PMD_SPEAKER_C   */ DLB_SADM_SPEAKER_C,
    /* PMD_SPEAKER_LFE */ DLB_SADM_SPEAKER_LFE,
    /* PMD_SPEAKER_LS  */ DLB_SADM_SPEAKER_LS,
    /* PMD_SPEAKER_RS  */ DLB_SADM_SPEAKER_RS,
    /* PMD_SPEAKER_LRS */ DLB_SADM_SPEAKER_LRS,
    /* PMD_SPEAKER_RRS */ DLB_SADM_SPEAKER_RRS,

    /* PMD_SPEAKER_LTF */ DLB_SADM_SPEAKER_LTF,
    /* PMD_SPEAKER_RTF */ DLB_SADM_SPEAKER_RTF,
    /* PMD_SPEAKER_LTM */ DLB_SADM_SPEAKER_LTM,
    /* PMD_SPEAKER_RTM */ DLB_SADM_SPEAKER_RTM,
    /* PMD_SPEAKER_LTR */ DLB_SADM_SPEAKER_LTR,
    /* PMD_SPEAKER_RTR */ DLB_SADM_SPEAKER_RTR,

    /* PMD_SPEAKER_LFW */ DLB_SADM_SPEAKER_LFW,
    /* PMD_SPEAKER_RFW */ DLB_SADM_SPEAKER_RFW
};


/**
 * @brief internal state of the OAMDI generator
 */
struct pmd_sadm_generator
{
    const dlb_pmd_model *pmd;
    dlb_sadm_model *sadm;
    unsigned int next_track_uid;
    dlb_sadm_idref bed_packformats[NUM_PMD_SPEAKER_CONFIGS];
    dlb_sadm_idref bed_7_0_4_packformat;
};


/**
 * @brief handy failure catch point for debugging
 */
static inline
dlb_pmd_success
pmd_sadm_generator_failure_catchpoint
    (void
    )
{
    return PMD_FAIL;
}


/**
 * @def FAILURE
 * @brief macro wrapping the failure catchpoint for debuggers
 */
#define FAILURE pmd_sadm_generator_failure_catchpoint()


/**
 * @brief generate the required form of an audio programme id
 *
 * This is derived from the id of the corresponding PMD presentation
 */ 
static inline
void
generate_programme_id
    (pmd_sadm_generator *g       /**< [in] PMD -> sADM model converter */
    ,dlb_pmd_presentation_id pid /**< [in] presentation identifier */
    ,dlb_sadm_id *id             /**< [out] sADM id to generate */
    )
{
    (void)g;
    snprintf((char*)id->data, sizeof(id->data), "APR_%04x", 0x1000 + pid);
}


/**
 * @brief generate the required form of an audio content id
 *
 * This is derived from the id of the corresponding PMD element
 */ 
static inline
void
generate_content_id
    (pmd_sadm_generator *g       /**< [in] PMD -> sADM model converter */
    ,dlb_pmd_element_id eid      /**< [in] element identifier */
    ,dlb_sadm_id *id             /**< [out] sADM id to generate */
    )
{
    (void)g;
    snprintf((char*)id->data, sizeof(id->data), "ACO_%04x", 0x1000 + eid);
}


/**
 * @brief generate the required form of an audio object id
 *
 * This is derived from the id of the corresponding PMD element
 */ 
static inline
void
generate_object_id
    (pmd_sadm_generator *g       /**< [in] PMD -> sADM model converter */
    ,dlb_pmd_element_id eid      /**< [in] element identifier */
    ,dlb_sadm_id *id             /**< [out] sADM id to generate */
    )
{
    (void)g;
    snprintf((char*)id->data, sizeof(id->data), "AO_%04x", 0x1000 + eid);
}


/**
 * @brief generate audio pack format id
 *
 * In DLB sADM, we use the following naming convention:
 *
 *   AC_yyyyxxxx
 *
 *  where <yyyy> is a 4-digit hex code with following values:
 *     0x0001 - DirectSpeakers (ABD direct)
 *     0x0002 - Matrix         (ABD derived)
 *     0x0003 - Objects        AOD
 *
 *  and where <xxxx> depends on <yyyy>
 *
 *  yyyy     xxxx
 *  0x0001   0x1001-0x1010, corresponds to particular speaker positions
 *  0x0002   0x1001-0x1010, corresponds to particular speaker positions
 *  0x0003   0x1001-0x1fff, corresponds to 0x1000 + object id
 *
 * Note that we generate a single packformat for each speaker config.
 * I.e., if we have two 5.1.2 beds, then they will both share the same
 * pack format (and associated channel formats).  Objects, on the other
 * hand, are uniquely associated with their own pack formats.
 */
static inline
void
generate_pack_format_id
    (pmd_sadm_generator *g       /**< [in] PMD -> sADM model converter */
    ,dlb_sadm_packfmt_type type  /**< [in] packformat type */
    ,dlb_pmd_element_id eid      /**< [in] element identifier (for objects) */
    ,dlb_pmd_speaker_config cfg  /**< [in] speaker config */
    ,dlb_sadm_id *id             /**< [out] sADM id to generate */
    )
{
    unsigned int yyyy = (unsigned int)type;
    unsigned int xxxx = type < DLB_SADM_PACKFMT_TYPE_OBJECT
        ? (unsigned int)cfg + 0x1001    /* PMDLIB-180: cfg is 0-based, pack ID must be 1001-based */
        : (unsigned int)eid + 0x1000;   /* element ID starts at 1, so it is OK like this */
    (void)g;

    snprintf((char*)id->data, sizeof(id->data), "AP_%04x%04x", yyyy, xxxx);
}


/**
 * @brief generate audio channel format id
 *
 * In DLB sADM, we use the following naming convention:
 *
 *   AC_yyyyxxxx
 *
 *  where <yyyy> is a 4-digit hex code with following values:
 *     0x0001 - DirectSpeakers (ABD direct)
 *     0x0002 - Matrix         (ABD derived)
 *     0x0003 - Objects        AOD
 *
 *  and where <xxxx> depends on <yyyy>
 *
 *  yyyy     xxxx
 *  0x0001   0x1001-0x1010, corresponds to particular speaker positions
 *  0x0002   0x1001-0x1010, corresponds to particular speaker positions
 *  0x0003   0x1001-0x1fff, corresponds to 0x1000 + object id
 */
static inline
void
generate_channel_format_id
    (pmd_sadm_generator *g       /**< [in] PMD -> sADM model converter */
    ,dlb_sadm_packfmt_type type  /**< [in] packformat type */
    ,dlb_pmd_element_id eid      /**< [in] element identifier (for objects) */
    ,dlb_pmd_speaker speaker     /**< [in] speaker position (for beds) */
    ,dlb_sadm_id *id             /**< [out] sADM id to generate */
    )
{
    unsigned int yyyy = (unsigned int)type;
    unsigned int xxxx = type < DLB_SADM_PACKFMT_TYPE_OBJECT
        ? (unsigned int)PMD_TO_SADM_SPEAKER[speaker-1] + 0x1000
        : (unsigned int)eid + 0x1000;
    (void)g;

    snprintf((char*)id->data, sizeof(id->data), "AC_%04x%04x", yyyy, xxxx);
}


/**
 * @brief generate audio block format id
 *
 * In DLB sADM, we use the following naming convention:
 *
 *   AC_yyyyxxxx_zzzzzzzz
 *
 *  where <yyyy> is a 4-digit hex code with following values:
 *     0x0001 - DirectSpeakers (ABD direct)
 *     0x0002 - Matrix         (ABD derived)
 *     0x0003 - Objects        AOD
 *
 *  and where <xxxx> depends on <yyyy>
 *
 *  yyyy     xxxx
 *  0x0001   0x1001-0x1010, corresponds to particular speaker positions
 *  0x0002   0x1001-0x1010, corresponds to particular speaker positions
 *  0x0003   0x1001-0x1fff, corresponds to 0x1000 + object id
 *
 * and where <zzzzzzzz> is just the number of the block.  In dolby serial ADM,
 * this is always 1.  (If we were to support in-frame dynamic updates, then
 * we might have more than 1).
 */
static inline
void
generate_block_format_id
    (pmd_sadm_generator *g  /**< [in] PMD -> sADM model converter */
    ,dlb_sadm_packfmt_type type  /**< [in] packformat type */
    ,dlb_pmd_element_id eid      /**< [in] element identifier (for objects) */
    ,dlb_pmd_speaker speaker     /**< [in] speaker position (for beds) */
    ,unsigned int blocknum  /**< [in] block number */
    ,dlb_sadm_id *id        /**< [out] sADM id to generate */
    )
{
    unsigned int yyyy = (unsigned int)type;
    unsigned int xxxx = type < DLB_SADM_PACKFMT_TYPE_OBJECT
        ? (unsigned int)speaker + 0x1000
        : (unsigned int)eid + 0x1000;
    (void)g;

    snprintf((char*)id->data, sizeof(id->data), "AB_%04x%04x_%08x", yyyy, xxxx, blocknum);
}


/**
 * @brief generate a track UID for a particular use of a PMD signal
 */
static inline
dlb_pmd_success              /** @return PMD_SUCCESS(=1) on success and PMD_FAIL(=0) otherwise */
generate_track_uid
    (pmd_sadm_generator *g   /**< [in] PMD -> sADM model converter */
    ,dlb_sadm_idref packfmt  /**< [in] track's pack format */
    ,dlb_sadm_idref chanfmt  /**< [in] track's channel format */
    ,dlb_pmd_signal sig      /**< [in] PMD signal */
    ,dlb_sadm_idref *idref   /**< [out] sADM id to generate */
    )
{
    dlb_sadm_track_uid track_uid;
    (void)g;
    snprintf((char*)track_uid.id.data, sizeof(track_uid.id.data), "ATU_%08x", g->next_track_uid++);

    track_uid.chanfmt = chanfmt;
    track_uid.packfmt = packfmt;
    track_uid.channel_idx = (unsigned int)sig;

    if (dlb_sadm_set_track_uid(g->sadm, &track_uid, idref))
    {
        error(g->pmd, "sADM generator failure: %s", dlb_sadm_error(g->sadm));
        return FAILURE;
    }
    return PMD_SUCCESS;
}


/**
 * @brief check whether the presentation is able to be converted to an sADM programme
 *
 * In Dolby's profile of serial ADM, programme speaker configs are
 * determined by the multi-channel pack formats.  This means that the
 * presentation's speaker config *must* match the speaker config of
 * each bed.
 */
static inline
dlb_pmd_success                /** @return PMD_SUCCESS(=1) on success and PMD_FAIL(=0) otherwise */
check_presentation_config
    (pmd_sadm_generator *g       /**< [in] PMD -> sADM model converter */
    ,dlb_pmd_presentation *pres  /**< [in] PMD presentation to check */
    )
{
    dlb_pmd_source sources[16];
    dlb_pmd_bed bed;
    unsigned int i;
    
    for (i = 0; i != pres->num_elements; ++i)
    {
        if (!dlb_pmd_bed_lookup(g->pmd, pres->elements[i], &bed, 16, sources))
        {
            if (pres->config != bed.config)
            {
                error(g->pmd, "sADM generation failure: presentation %u has config %u, "
                      "but its bed (id %u) has a different config %u: not supported\n",
                      pres->id, pres->config, bed.id, bed.config);
                return FAILURE;
            }
        }
    }
    return PMD_SUCCESS;
}


/**
 * @brief generate sADM audioProgramme entities for each PMD presentation
 */
static
dlb_pmd_success              /** @return PMD_SUCCESS(=1) on success and PMD_FAIL(=0) otherwise */
generate_programmes
    (pmd_sadm_generator *g   /**< [in] PMD -> sADM model converter */
    )
{
    dlb_pmd_presentation_iterator pi;
    dlb_pmd_presentation pres;
    dlb_pmd_element_id elements[4095];
    dlb_sadm_programme_label labels[128];
    dlb_sadm_idref carray[128];
    dlb_sadm_programme prog;
    unsigned int i;
    
    if (dlb_pmd_presentation_iterator_init(&pi, g->pmd))
    {
        return FAILURE;
    }

    while (!dlb_pmd_presentation_iterator_next(&pi, &pres, 4095, elements))
    {
        dlb_pmd_bool got_name = PMD_FALSE;

        generate_programme_id(g, pres.id, &prog.id);
        memmove(prog.language, pres.audio_language, sizeof(pres.audio_language));

        if (pres.num_names == 1)
        {
            memmove(prog.name.data, pres.names[0].text, sizeof(prog.name.data));
            got_name = PMD_TRUE;
        }
        else if (pres.num_names > 1)
        {
            for (i = 0; i < pres.num_names && !got_name; i++)
            {
                if (!memcmp(pres.audio_language, pres.names[i].language, sizeof(pres.audio_language)))
                {
                    got_name = PMD_TRUE;
                    break;
                }
            }
            if (got_name)
            {
                memmove(prog.name.data, pres.names[i].text, sizeof(prog.name.data));
            }
        }
        if (!got_name)
        {
            snprintf((char*)prog.name.data, sizeof(prog.name.data), "Pres-%u", pres.id);
        }

        if (check_presentation_config(g, &pres))
        {
            return FAILURE;
        }

        /* populate content array */
        prog.contents.num   = pres.num_elements;
        prog.contents.max   = ARRAYSZ(carray);
        prog.contents.array = carray;
        if (pres.num_elements > prog.contents.max)
        {
            error(g->pmd, "too many elements in presentation, converter can only manage %u",
                  prog.contents.max);
            return FAILURE;
        }

        for (i = 0; i != pres.num_elements; ++i)
        {
            dlb_sadm_id id;
            generate_content_id(g, pres.elements[i], &id);
            if (dlb_sadm_lookup_reference(g->sadm, id.data, DLB_SADM_CONTENT, 0, &carray[i]))
            {
                error(g->pmd, "could not generate sADM audioContent forward reference (too many)");
                return FAILURE;
            }
        }

        /* populate label array */
        if (pres.num_names > ARRAYSZ(labels))
        {
            error(g->pmd, "presentation has too many names for sADM generator, it has max %u",
                  ARRAYSZ(labels));
            return FAILURE;
        }

        prog.num_labels = pres.num_names;
        prog.labels = labels;
        for (i = 0; i != pres.num_names; ++i)
        {
            memmove(prog.labels[i].language, pres.names[i].language, 4);
            memmove(prog.labels[i].name.data, pres.names[i].text, sizeof(prog.labels[i].name.data));
        }

        if (dlb_sadm_set_programme(g->sadm, &prog, NULL))
        {
            error(g->pmd, "sADM generator failure: %s", dlb_sadm_error(g->sadm));
            return FAILURE;
        }
    }
    return PMD_SUCCESS;
}


/**
 * @brief generate an sADM audioChannelFormat for a PMD bed element
 */
static inline
dlb_pmd_success              /** @return PMD_SUCCESS(=1) on success and PMD_FAIL(=0) otherwise */
generate_bed_channel_format
    (pmd_sadm_generator *g   /**< [in] PMD -> sADM model converter */
    ,dlb_sadm_packfmt_type type
    ,dlb_pmd_bed *bed
    ,unsigned int idx
    ,dlb_sadm_idref *idref
    ,dlb_pmd_bool alt_spkrs
    )
{
    dlb_sadm_channel_format chanfmt;
    dlb_sadm_block_format blkfmt;
    dlb_sadm_idref blkfmt_idref;
    speaker_blkfmt *sb = &SPEAKER_BLKFMT[bed->sources[idx].target-1];

    if (!alt_spkrs)
    {
        /* !alt_spkrs means 2.0 - 5.1.4 (i.e., no rear surrounds)
         * when no rear surrounds, Ls and Rs take the Lrs and Rrs
         * speaker positions, plus shorter names
         */
        switch (bed->sources[idx].target)
        {
        case PMD_SPEAKER_LS:
            sb = &SPEAKER_BLKFMT_LS_RS[0];
            break;
        case PMD_SPEAKER_RS:
            sb = &SPEAKER_BLKFMT_LS_RS[1];
            break;
        default:
            break;
        }
    }
    
    generate_channel_format_id(g, type, 0, bed->sources[idx].target, &chanfmt.id);
    memmove(chanfmt.name.data, sb->name, sizeof(chanfmt.name.data));

    chanfmt.blkfmts.num   = 1;
    chanfmt.blkfmts.max   = 1;    
    chanfmt.blkfmts.array = &blkfmt_idref;

    generate_block_format_id(g, type, 0, bed->sources[idx].target, 1, &blkfmt.id);
    memmove(blkfmt.speaker_label, sb->label, MIN(sizeof(blkfmt.speaker_label), sizeof(sb->label)));

    blkfmt.gain = bed->sources[idx].gain;

    blkfmt.cartesian_coordinates = 1;
    blkfmt.azimuth_or_x          = sb->x;
    blkfmt.elevation_or_y        = sb->y;
    blkfmt.distance_or_z         = sb->z;

    if (dlb_sadm_set_block_format(g->sadm, &blkfmt, &blkfmt_idref))
    {
        error(g->pmd, "sADM generator failure: %s", dlb_sadm_error(g->sadm));
        return FAILURE;
    }
    if (dlb_sadm_set_channel_format(g->sadm, &chanfmt, idref))
    {
        error(g->pmd, "sADM generator failure: %s", dlb_sadm_error(g->sadm));
        return FAILURE;
    }
    return PMD_SUCCESS;
}


static
dlb_pmd_bool
is_bed_7_0_4
    (dlb_pmd_bed *bed
    )
{
    dlb_pmd_bool bed_is_7_0_4 = (bed->num_sources == 11);   /* Preliminary guess */
    dlb_pmd_source *source;
    size_t i;

    if (bed_is_7_0_4)                       /* Now check all the sources */
    {
        dlb_pmd_bool got_L = PMD_FALSE;
        dlb_pmd_bool got_R = PMD_FALSE;
        dlb_pmd_bool got_C = PMD_FALSE;
        dlb_pmd_bool got_Lss = PMD_FALSE;
        dlb_pmd_bool got_Rss = PMD_FALSE;
        dlb_pmd_bool got_Lrs = PMD_FALSE;
        dlb_pmd_bool got_Rrs = PMD_FALSE;
        dlb_pmd_bool got_LTF = PMD_FALSE;
        dlb_pmd_bool got_RTF = PMD_FALSE;
        dlb_pmd_bool got_LTR = PMD_FALSE;
        dlb_pmd_bool got_RTR = PMD_FALSE;

        source = bed->sources;
        for (i = 0; i < bed->num_sources && bed_is_7_0_4; i++, source++)
        {
            switch (source->target)
            {
            case PMD_SPEAKER_L:
                if (got_L)
                {
                    bed_is_7_0_4 = PMD_FALSE;
                }
                else
                {
                    got_L = PMD_TRUE;
                }
                break;

            case PMD_SPEAKER_R:
                if (got_R)
                {
                    bed_is_7_0_4 = PMD_FALSE;
                }
                else
                {
                    got_R = PMD_TRUE;
                }
                break;

            case PMD_SPEAKER_C:
                if (got_C)
                {
                    bed_is_7_0_4 = PMD_FALSE;
                }
                else
                {
                    got_C = PMD_TRUE;
                }
                break;

            case PMD_SPEAKER_LS:
                if (got_Lss)
                {
                    bed_is_7_0_4 = PMD_FALSE;
                }
                else
                {
                    got_Lss = PMD_TRUE;
                }
                break;

            case PMD_SPEAKER_RS:
                if (got_Rss)
                {
                    bed_is_7_0_4 = PMD_FALSE;
                }
                else
                {
                    got_Rss = PMD_TRUE;
                }
                break;

            case PMD_SPEAKER_LRS:
                if (got_Lrs)
                {
                    bed_is_7_0_4 = PMD_FALSE;
                }
                else
                {
                    got_Lrs = PMD_TRUE;
                }
                break;

            case PMD_SPEAKER_RRS:
                if (got_Rrs)
                {
                    bed_is_7_0_4 = PMD_FALSE;
                }
                else
                {
                    got_Rrs = PMD_TRUE;
                }
                break;

            case PMD_SPEAKER_LTF:
                if (got_LTF)
                {
                    bed_is_7_0_4 = PMD_FALSE;
                }
                else
                {
                    got_LTF = PMD_TRUE;
                }
                break;

            case PMD_SPEAKER_RTF:
                if (got_RTF)
                {
                    bed_is_7_0_4 = PMD_FALSE;
                }
                else
                {
                    got_RTF = PMD_TRUE;
                }
                break;

            case PMD_SPEAKER_LTR:
                if (got_LTR)
                {
                    bed_is_7_0_4 = PMD_FALSE;
                }
                else
                {
                    got_LTR = PMD_TRUE;
                }
                break;

            case PMD_SPEAKER_RTR:
                if (got_RTR)
                {
                    bed_is_7_0_4 = PMD_FALSE;
                }
                else
                {
                    got_RTR = PMD_TRUE;
                }
                break;

            default:
                bed_is_7_0_4 = PMD_FALSE;
                break;
            }
        }
    }

    return bed_is_7_0_4;
}

/**
 * @brief generate an sADM audioObject for a PMD bed element
 */
static inline
dlb_pmd_success              /** @return PMD_SUCCESS(=1) on success and PMD_FAIL(=0) otherwise */
generate_bed_object
    (pmd_sadm_generator *g   /**< [in] PMD -> sADM model converter */
    ,dlb_pmd_bed *bed
    ,dlb_sadm_idref *object_idref
    )
{
    dlb_sadm_object object;
    dlb_sadm_idref tracks[DLB_PMD_MAX_BED_SOURCES];
    dlb_sadm_pack_format packfmt;
    dlb_sadm_idref object_ref;
    dlb_sadm_idref chanfmt_idrefs[DLB_PMD_MAX_BED_SOURCES];
    dlb_sadm_idref *chanfmt;
    dlb_pmd_source *source;
    dlb_pmd_bool alt_spkrs;
    dlb_pmd_bool bed_is_7_0_4 = PMD_FALSE;
    unsigned int i;

    packfmt.type           = DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS; /* can't handle mixed */
    packfmt.chanfmts.num   = bed->num_sources;
    packfmt.chanfmts.max   = ARRAYSZ(chanfmt_idrefs);
    packfmt.chanfmts.array = chanfmt_idrefs;
    generate_pack_format_id(g, packfmt.type, 0, bed->config, &packfmt.id);

    if (bed->num_sources != SPEAKER_CONFIG_COUNT[bed->config])
    {
        bed_is_7_0_4 = is_bed_7_0_4(bed);      /* Special case for 7.0.4 */

        if (!bed_is_7_0_4)
        {
            error(g->pmd, "serial ADM beds must have exactly one source per target speaker\n");
            return FAILURE;
        }
    }
    if (bed->bed_type != PMD_BED_ORIGINAL)
    {
        error(g->pmd, "serial ADM beds cannot be derived\n");
        return FAILURE;
    }
    if (bed->config > DLB_PMD_SPEAKER_CONFIG_9_1_6)
    {
        error(g->pmd, "serial ADM beds cannot have portable or headphone config\n");
        return FAILURE;
    }

    for (i = bed->num_sources - 1; i > 0; i--)
    {
        if (bed->sources[i].gain != bed->sources[0].gain)
        {
            error(g->pmd, "sources of serial ADM beds must all have the same gain\n");
            return FAILURE;
        }
    }
    object.gain = bed->sources[0].gain;
    //printf("bed object gain %f\n", object.gain);

    if (bed_is_7_0_4)
    {
        object.pack_format = g->bed_7_0_4_packformat;
    } 
    else
    {
        object.pack_format = g->bed_packformats[bed->config];
    }
    if (object.pack_format != NULL)
    {
        /* look up pack format if previously generated */
        if (dlb_sadm_pack_format_lookup(g->sadm, object.pack_format, &packfmt, &packfmt.chanfmts))
        {
            error(g->pmd, "sADM generator failure: %s", dlb_sadm_error(g->sadm));
            return FAILURE;
        }
    }
    else
    {
        alt_spkrs = (bed->config > DLB_PMD_SPEAKER_CONFIG_5_1_4);
        const char *pack_fmt_name;

        if (bed_is_7_0_4)
        {
            pack_fmt_name = speaker_config_7_0_4_name;
        } 
        else
        {
            pack_fmt_name = speaker_config_names[bed->config];
        }
        sprintf((char *)packfmt.name.data, "%s", pack_fmt_name);

        for (i = 0; i != bed->num_sources; ++i)
        {
            if (generate_bed_channel_format(g, packfmt.type, bed, i, &chanfmt_idrefs[i], alt_spkrs))
            {
                return FAILURE;
            }
        }

        if (dlb_sadm_set_pack_format(g->sadm, &packfmt, &object.pack_format))
        {
            error(g->pmd, "sADM generator failure: %s", dlb_sadm_error(g->sadm));
            return FAILURE;
        }
        if (bed_is_7_0_4)
        {
            g->bed_7_0_4_packformat = object.pack_format;
        } 
        else
        {
            g->bed_packformats[bed->config] = object.pack_format;
        }
    }

    generate_object_id(g, bed->id, &object.id);
    memmove(object.name.data, bed->name, sizeof(object.name.data));
    object.object_refs.num   = 0;
    object.object_refs.max   = 1;
    object.object_refs.array = &object_ref;
    object.track_uids.num    = bed->num_sources;
    object.track_uids.max    = ARRAYSZ(tracks);
    object.track_uids.array  = tracks;
    
    if (bed->num_sources != object.track_uids.num)
    {
        error(g->pmd, "cannot support anything other than simple beds (one source per speaker)\n");
        return FAILURE;
    }

    source = bed->sources;
    chanfmt = packfmt.chanfmts.array;
    for (i = 0; i != bed->num_sources; ++i, ++source, ++chanfmt)
    {
        if (generate_track_uid(g, object.pack_format, *chanfmt, source->source, &tracks[i]))
        {
            return FAILURE;
        }
    }
    return dlb_sadm_set_object(g->sadm, &object, object_idref);
}


/**
 * @brief generate an sADM audioChannelFormat for a PMD object element
 */
static inline
dlb_pmd_success              /** @return PMD_SUCCESS(=1) on success and PMD_FAIL(=0) otherwise */
generate_nonbed_channel_format
    (pmd_sadm_generator *g   /**< [in] PMD -> sADM model converter */
    ,dlb_pmd_object *object
    ,dlb_sadm_idref *idref
   )
{
    dlb_sadm_channel_format chanfmt;
    dlb_sadm_idref blkfmt_idref;
    dlb_sadm_block_format blkfmt;

    generate_channel_format_id(g, DLB_SADM_PACKFMT_TYPE_OBJECT, object->id, 0, &chanfmt.id);
    memmove(chanfmt.name.data, object->name, sizeof(chanfmt.name.data));

    chanfmt.blkfmts.num   = 1;
    chanfmt.blkfmts.max   = 1;    
    chanfmt.blkfmts.array = &blkfmt_idref;

    memset(&blkfmt, '\0', sizeof(blkfmt));
    generate_block_format_id(g, DLB_SADM_PACKFMT_TYPE_OBJECT, object->id, 0, 1, &blkfmt.id);
    blkfmt.cartesian_coordinates = 1;
    blkfmt.azimuth_or_x          = object->x;
    blkfmt.elevation_or_y        = object->y;
    blkfmt.distance_or_z         = object->z;

    if (dlb_sadm_set_block_format(g->sadm, &blkfmt, &blkfmt_idref))
    {
        error(g->pmd, "sADM generator failure: %s", dlb_sadm_error(g->sadm));
        return FAILURE;
    }
    return dlb_sadm_set_channel_format(g->sadm, &chanfmt, idref);
}


/**
 * @brief generate an sADM audioObject for a PMD object element
 */
static inline
dlb_pmd_success              /** @return PMD_SUCCESS(=1) on success and PMD_FAIL(=0) otherwise */
generate_nonbed_object
    (pmd_sadm_generator *g   /**< [in] PMD -> sADM model converter */
    ,dlb_pmd_object *obj
    ,dlb_sadm_idref *object_idref
    )
{
    dlb_sadm_object object;
    dlb_sadm_pack_format packfmt;
    dlb_sadm_idref object_ref;
    dlb_sadm_idref chanfmt_idref;
    dlb_sadm_idref packfmt_idref;
    dlb_sadm_idref track;
    
    memmove(object.name.data, obj->name, sizeof(object.name.data));
    generate_object_id(g, obj->id, &object.id);

    if (obj->dynamic_updates)
    {
        error(g->pmd, "sADM generator failure: object %u has dynamic updates, "
              "which are not supported\n", obj->id);
        return FAILURE;
    }
    if (obj->size != 0.0f)
    {
        error(g->pmd, "sADM generator failure: object %u has non-point size (%.*f), "
              "which is not supported\n", obj->id, g->pmd->coordinate_print_precision, obj->size);
        return FAILURE;
    }
    if (obj->size_3d != PMD_FALSE)
    {
        error(g->pmd, "sADM generator failure: object %u specifies 3D objects "
              "which are not supported\n", obj->id);
        return FAILURE;
    }
    if (obj->diverge)
    {
        error(g->pmd, "sADM generator failure: object %u specifies divergence "
              "which is not supported\n", obj->id);
        return FAILURE;
    }

    object.gain  = obj->source_gain;

    packfmt.type           = DLB_SADM_PACKFMT_TYPE_OBJECT;
    packfmt.chanfmts.num   = 1;
    packfmt.chanfmts.max   = 1;
    packfmt.chanfmts.array = &chanfmt_idref;
    generate_pack_format_id(g, packfmt.type, obj->id, 0, &packfmt.id);
    memmove(packfmt.name.data, obj->name, sizeof(packfmt.name.data));

    if (generate_nonbed_channel_format(g, obj, &chanfmt_idref))
    {
        return FAILURE;
    }
    if (dlb_sadm_set_pack_format(g->sadm, &packfmt, &packfmt_idref))
    {
        error(g->pmd, "sADM generator failure: %s", dlb_sadm_error(g->sadm));
        return FAILURE;
    }

    object.pack_format       = packfmt_idref;
    object.object_refs.num   = 0;
    object.object_refs.max   = 1;
    object.object_refs.array = &object_ref;
    object.track_uids.num    = 1;
    object.track_uids.max    = 1;
    object.track_uids.array  = &track;
    if (generate_track_uid(g, packfmt_idref, chanfmt_idref, obj->source, &track))
    {
        return FAILURE;
    }

    return dlb_sadm_set_object(g->sadm, &object, object_idref);
}


/**
 * @brief generate audioContent definitions for PMD bed elements
 */
static inline
dlb_pmd_success              /** @return PMD_SUCCESS(=1) on success and PMD_FAIL(=0) otherwise */
generate_contents_beds
    (pmd_sadm_generator *g   /**< [in] PMD -> sADM model converter */
    )
{
    dlb_pmd_bed_iterator bi;
    dlb_pmd_source sources[128];
    dlb_pmd_bed bed;

    if (dlb_pmd_bed_iterator_init(&bi, g->pmd))
    {
        return FAILURE;
    }

    while (!dlb_pmd_bed_iterator_next(&bi, &bed, ARRAYSZ(sources), sources))
    {
        dlb_sadm_idref objref;
        dlb_sadm_content content;

        if (generate_bed_object(g, &bed, &objref))
        {
            return FAILURE;
        }
        content.objects.array = &objref;
        content.objects.max = 1;
        content.objects.num = 1;
        
        generate_content_id(g, bed.id, &content.id);
        memmove(&content.name, &bed.name, sizeof(content.name));
        content.dialogue_value = ADM_DIALOGUE_VALUE_FOR_BED;
        content.type = DLB_SADM_CONTENT_MK_MIXED;
        if (dlb_sadm_set_content(g->sadm, &content, NULL))
        {
            error(g->pmd, "sADM generator failure: %s", dlb_sadm_error(g->sadm));
            return FAILURE;
        }
    }
    return PMD_SUCCESS;
}


/**
 * @brief generate audioContent definitions for PMD object elements
 */
static inline
dlb_pmd_success              /** @return PMD_SUCCESS(=1) on success and PMD_FAIL(=0) otherwise */
generate_contents_objects
    (pmd_sadm_generator *g   /**< [in] PMD -> sADM model converter */
    )
{
    dlb_pmd_object_iterator oi;
    dlb_pmd_object object;

    if (dlb_pmd_object_iterator_init(&oi, g->pmd))
    {
        return FAILURE;
    }

    while (!dlb_pmd_object_iterator_next(&oi, &object))
    {
        dlb_sadm_idref objref;
        dlb_sadm_content content;

        if (generate_nonbed_object(g, &object, &objref))
        {
            return FAILURE;
        }
        content.objects.array = &objref;
        content.objects.max = 1;
        content.objects.num = 1;
        
        generate_content_id(g, object.id, &content.id);
        memmove(&content.name, &object.name, sizeof(content.name));

        switch (object.object_class)
        {
            case PMD_CLASS_DIALOG:
                content.dialogue_value = ADM_DIALOGUE_VALUE_FOR_OBJECT;
                content.type           = DLB_SADM_CONTENT_DK_DIALOGUE;
                break;
            case PMD_CLASS_VDS:
                content.dialogue_value = ADM_DIALOGUE_VALUE_FOR_OBJECT;
                content.type           = DLB_SADM_CONTENT_DK_AUD_DESC;
                break;
            case PMD_CLASS_VOICEOVER:
                content.dialogue_value = ADM_DIALOGUE_VALUE_FOR_OBJECT;
                content.type           = DLB_SADM_CONTENT_DK_VOICEOVER;
                break;
            case PMD_CLASS_GENERIC:
                content.dialogue_value = ADM_DIALOGUE_VALUE_FOR_GENERIC_OBJECT;
                content.type           = DLB_SADM_CONTENT_MK_UNDEFINED;
                break;
            case PMD_CLASS_SUBTITLE:
                content.dialogue_value = ADM_DIALOGUE_VALUE_FOR_OBJECT;
                content.type           = DLB_SADM_CONTENT_DK_SPOKEN_SUB;
                break;
            case PMD_CLASS_EMERGENCY_ALERT:
                content.dialogue_value = ADM_DIALOGUE_VALUE_FOR_OBJECT;
                content.type           = DLB_SADM_CONTENT_DK_EMERGENCY;
                break;
            case PMD_CLASS_EMERGENCY_INFO:
                content.dialogue_value = ADM_DIALOGUE_VALUE_FOR_OBJECT;
                content.type           = DLB_SADM_CONTENT_DK_EMERGENCY;
                break;
            default:
                return FAILURE;
        }
        if (dlb_sadm_set_content(g->sadm, &content, NULL))
        {
            error(g->pmd, "sADM generator failure: %s", dlb_sadm_error(g->sadm));
            return FAILURE;
        }
    }
    return PMD_SUCCESS;
}


/**
 * @brief generate all audioContent, audioObject, audioPackFormat, audioChannelFormat,
 * audioBlockFormat and audioTrackUID serial ADM elements
 */
static inline
dlb_pmd_success              /** @return PMD_SUCCESS(=1) on success and PMD_FAIL(=0) otherwise */
generate_contents
    (pmd_sadm_generator *g   /**< [in] PMD -> sADM model converter */
    )
{
    return generate_contents_beds(g)
        || generate_contents_objects(g);
}


/**
 * @brief generate frameFormat elements
 */
static inline
dlb_pmd_success              /** @return PMD_SUCCESS(=1) on success and PMD_FAIL(=0) otherwise */
generate_frame_format
    (pmd_sadm_generator *g   /**< [in] PMD -> sADM model converter */
    )
{
    dlb_pmd_identity_and_timing iat;
    dlb_pmd_success iat_success = dlb_pmd_iat_lookup(g->pmd, &iat);
    dlb_pmd_success success = PMD_SUCCESS;

    if (iat_success == PMD_SUCCESS && iat.content_id.size > 0)
    {
        char uuid[37];

        memset(uuid, 0, sizeof(uuid));
        write_uuid(iat.content_id.data, uuid);
        success = dlb_sadm_set_flow_id(g->sadm, uuid, sizeof(uuid));
    }

    return success;
}


/**
 * @brief make sure we have generated everything
 */
static inline
dlb_pmd_success              /** @return PMD_SUCCESS(=1) on success and PMD_FAIL(=0) otherwise */
verify
    (pmd_sadm_generator *g   /**< [in] PMD -> sADM model converter */
    )
{
#define NUM_UNDEFINED (128)
    dlb_sadm_undefined_ref undefined[NUM_UNDEFINED];
    size_t count = 0;

    count = dlb_sadm_get_undefined_references(g->sadm, undefined, NUM_UNDEFINED);
    if (count)
    {
        static const char *reftypes[] = { "audioProgramme", "audioContent", "audioChannelFormat",
                                          "audioObject", "audioPackFormat", "audioTrackUID",
                                          "audioBlockFormat"  };
        size_t i;
        for (i = 0; i < count && i < NUM_UNDEFINED; ++i)
        {
            error(g->pmd, "ERROR: undefined %s reference \"%s\" at line %u\n",
                  reftypes[undefined[i].type], undefined[i].id, undefined[i].lineno);
        }
        return FAILURE;
    }
    return PMD_SUCCESS;
}


/* ------------------------ public API -------------------------- */


size_t
pmd_sadm_generator_query_mem
    (void
    )
{
    return sizeof(pmd_sadm_generator);
}


dlb_pmd_success
pmd_sadm_generator_init
    (void *mem
    ,pmd_sadm_generator **cptr
    )
{
    pmd_sadm_generator *g = (pmd_sadm_generator*)mem;
    unsigned int i;
    
    g->next_track_uid = 1;
    for (i = 0; i != NUM_PMD_SPEAKER_CONFIGS; ++i)
    {
        g->bed_packformats[i] = NULL;
    }
    g->bed_7_0_4_packformat = NULL;
    *cptr = g;
    return PMD_SUCCESS;
}


void
pmd_sadm_generator_finish
    (pmd_sadm_generator  *g
    )
{
    (void)g;
}


dlb_pmd_success
pmd_sadm_generator_generate
    (pmd_sadm_generator  *g
    ,const dlb_pmd_model *pmd
    ,dlb_sadm_model *sadm
    )
{
    unsigned int i;

    g->pmd = pmd;
    g->sadm = sadm;
    g->next_track_uid = 1;
    for (i = 0; i != NUM_PMD_SPEAKER_CONFIGS; ++i)
    {
        g->bed_packformats[i] = NULL;
    }

    dlb_sadm_reinit(sadm);

    return generate_programmes(g)
        || generate_contents(g)
        || generate_frame_format(g)
        || verify(g)
        ;
}


