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
 * @file pmd_sadm_ingester.c
 * @brief convert serial ADM model to PMD model
 */

#include "sadm/dlb_sadm_model.h"
#include "sadm/memstuff.h"
#include "sadm/idrefs.h"
#include "dlb_pmd_api.h"
#include "pmd_sadm_ingester.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#ifdef _MSC_VER
#  define strcasecmp _stricmp
#  define strncasecmp _strnicmp
#  define strtoll _strtoi64
__pragma(warning(disable:4127))
#endif


/**
 * @brief type of the sADM to PMD ingester
 */
struct pmd_sadm_ingester
{
    dlb_sadm_model *sm;
    dlb_pmd_model *pmd;
};


/**
 * @def ARRAYSZ(a)
 * @brief compute number of elements in array
 */
#define ARRAYSZ(a) (sizeof(a)/sizeof((a)[0]))


typedef struct
{
    const char *sadm_label;
    dlb_pmd_speaker pmd_target;
} speaker_mapping;


/**
 * @brief handy failure catch point for debugging
 */
#ifndef NDEBUG
static
dlb_pmd_success
pmd_sadm_ingester_failure_catchpoint
    (void
    )
{
    return PMD_FAIL;
}
#endif


/**
 * @def FAILURE
 * @brief macro wrapping the failure catchpoint for debuggers
 */
#ifndef NDEBUG
#define FAILURE pmd_sadm_ingester_failure_catchpoint()
#else
#define FAILURE PMD_FAIL
#endif


static
dlb_pmd_success
channel_target
    (dlb_sadm_block_format *blkfmt
    ,dlb_pmd_speaker *target
    )
{
    static speaker_mapping mapping[] = {
        { "RC_L",   PMD_SPEAKER_L   },
        { "RC_R",   PMD_SPEAKER_R   },
        { "RC_C",   PMD_SPEAKER_C   },
        { "RC_LFE", PMD_SPEAKER_LFE },
        { "RC_Ls",  PMD_SPEAKER_LS  },
        { "RC_Lss", PMD_SPEAKER_LS  },  //@kludge to read in IBC demo content
        { "RC_Rs",  PMD_SPEAKER_RS  },
        { "RC_Rss", PMD_SPEAKER_RS  },  //@kludge to read in IBC demo content
        { "RC_Lrs", PMD_SPEAKER_LRS },
        { "RC_Rrs", PMD_SPEAKER_RRS },

        { "RC_Ltf", PMD_SPEAKER_LTF },
        { "RC_Rtf", PMD_SPEAKER_RTF },
        { "RC_Ltm", PMD_SPEAKER_LTM },
        { "RC_Rtm", PMD_SPEAKER_RTM },
        { "RC_Ltr", PMD_SPEAKER_LTR },
        { "RC_Rtr", PMD_SPEAKER_RTR },

        { "RC_Lfw", PMD_SPEAKER_LFW },
        { "RC_Rfw", PMD_SPEAKER_RFW },

        //spherical names
        { "M+030",  PMD_SPEAKER_L   },
        { "M-030",  PMD_SPEAKER_R   },
        { "M+000",  PMD_SPEAKER_C   },
        { "LFE",    PMD_SPEAKER_LFE },
        { "M+110",  PMD_SPEAKER_LS  },
        { "M+090",  PMD_SPEAKER_LS  },
        { "M-110",  PMD_SPEAKER_RS  },
        { "M-090",  PMD_SPEAKER_RS  },
        { "M+135",  PMD_SPEAKER_LRS },
        { "M-135",  PMD_SPEAKER_RRS },

        { "U+030",  PMD_SPEAKER_LTF },
        { "U+045",  PMD_SPEAKER_LTF },
        { "U-030",  PMD_SPEAKER_RTF },
        { "U-045",  PMD_SPEAKER_RTF },
        { "U+090",  PMD_SPEAKER_LTM },
        { "U-090",  PMD_SPEAKER_RTM },
        { "U+110",  PMD_SPEAKER_LTR },
        { "U+135",  PMD_SPEAKER_LTR },
        { "U-110",  PMD_SPEAKER_RTR },
        { "U-135",  PMD_SPEAKER_RTR },

        { "M+060",  PMD_SPEAKER_LFW },
        { "M-060",  PMD_SPEAKER_RFW },
    };
    
    #define NUM_SPEAKER_MAPPINGS ((sizeof(mapping) / sizeof(mapping[0])))
    unsigned int i;

    /* todo: verify blkfmt's x,y,z match actual speaker locations. */
    for (i = 0; i < NUM_SPEAKER_MAPPINGS; ++i)
    {
        if (!strcmp((char*)blkfmt->speaker_label, mapping[i].sadm_label))
        {
            *target = mapping[i].pmd_target;
            return PMD_SUCCESS;
        }
    }
    return FAILURE;
}


static inline
dlb_pmd_success
sadm_to_pmd_lookup
    (dlb_sadm_idref idref
    ,unsigned int *id
    )
{
    const char *idname;
    unsigned int tmp;
    if (idref_name(idref, &idname)) return PMD_FAIL;

    idname = strchr(idname, '_');
    if (NULL == idname) return PMD_FAIL;
    
    if (1 != sscanf(idname, "_%x", &tmp)
        || tmp < 0x1001
        || tmp > 0x1fff)
    {
        return PMD_FAIL;
    }
    
    *id = tmp - 0x1000;
    return PMD_SUCCESS;
}


static
dlb_pmd_success
generate_pmd_bed
    (pmd_sadm_ingester    *c
    ,dlb_sadm_content     *content
    ,dlb_sadm_object      *object
    ,dlb_sadm_pack_format *packfmt
    )
{
    /* The number of audio_channel_idref entries in audio_pack can
     * inform us of the bed configuration without looking that the
     * speaker labels in the corresponding audio_blocks. This works
     * ok for fixed formats such as those used in the PMD editor 2,
     * 3, 6, 8, 10, 12
     */
    dlb_sadm_channel_format chanfmt;
    dlb_sadm_block_format blkfmt;
    dlb_sadm_track_uid track;
    dlb_sadm_idref *trackref;
    dlb_sadm_idref *chanref;
    dlb_sadm_idref idref;
    dlb_sadm_idref_array blkfmts;

    dlb_pmd_source sources[16];
    dlb_pmd_bed bed;
    unsigned int tmp;
    unsigned int i;
#ifdef ADD_LFE_FOR_DOT_0
    unsigned int add_lfe = 0;
#endif
    
    (void)content;
    memset(&bed, 0, sizeof(bed));

    if (1 != sscanf((char*)object->id.data, "AO_%x", &tmp)
        || tmp < 0x1001
        || tmp > 0x1fff)
    {
        printf("Illegal object id \"%s\"\n", (char*)object->id.data);
        return FAILURE;
    }
    bed.id = (dlb_pmd_element_id)(tmp - 0x1000u);

    (void)dlb_sadm_lookup_reference(c->sm, object->id.data, DLB_SADM_OBJECT, 0, &idref);
    bed.bed_type    = PMD_BED_ORIGINAL;
    bed.num_sources = (uint8_t)object->track_uids.num;
    bed.sources     = sources;
    memmove(bed.name, object->name.data, sizeof(bed.name));

    switch (packfmt->chanfmts.num)
    {
        case 2:  bed.config = DLB_PMD_SPEAKER_CONFIG_2_0;   break;
        case 3:  bed.config = DLB_PMD_SPEAKER_CONFIG_3_0;   break;
        case 6:  bed.config = DLB_PMD_SPEAKER_CONFIG_5_1;   break;
        case 8:  bed.config = DLB_PMD_SPEAKER_CONFIG_5_1_2; break;
        case 10: bed.config = DLB_PMD_SPEAKER_CONFIG_5_1_4; break;
        case 12: bed.config = DLB_PMD_SPEAKER_CONFIG_7_1_4; break;
        case 16: bed.config = DLB_PMD_SPEAKER_CONFIG_9_1_6; break;            
        /* Special case */
        case 11: /* check if this is 7.0.4 */
            if (packfmt->chanfmts.num != bed.num_sources)
            {
                printf("Error: Number of channels defined in bed[%s] (%d) is different than in pack format[%s] (%d) \n"
                        ,(char*)object->id.data
                        ,bed.num_sources
                        ,packfmt->id.data
                        ,packfmt->chanfmts.num);
                return FAILURE;
            }
            chanref = packfmt->chanfmts.array;
            for (i = 0; i != packfmt->chanfmts.num; ++i, ++chanref)
            {
                dlb_sadm_idref idrefs[128];
                blkfmts.num = 0;
                blkfmts.max = sizeof(idrefs)/sizeof(idrefs[0]);
                blkfmts.array = idrefs;
                dlb_sadm_channel_format_lookup(c->sm, *chanref, &chanfmt, &blkfmts);
                if (!strcasecmp((const char *)chanfmt.id.data, "AC_00010004"))  /* This is the ADM common definition ID for the LFE channel */
                {
                    /* If there is an LFE this is an error case */
                    printf("Error: failed to generate PMD bed for audioObject \"%s\"\n", object->name.data);
                    return FAILURE;
                }
            }
#ifdef ADD_LFE_FOR_DOT_0
            add_lfe = 1;
#endif
            bed.config = DLB_PMD_SPEAKER_CONFIG_7_1_4;
            break;
        default: 
            {
                printf("Error: failed to generate PMD bed for audioObject \"%s\"\n", object->name.data);
                return FAILURE;
            }
    }

    if (packfmt->chanfmts.num != object->track_uids.num)
    {
        printf("pack format number of channels != number of object's track uids\n");
        return FAILURE;
    }

    chanref = packfmt->chanfmts.array;
    trackref = object->track_uids.array;
    for (i = 0; i != bed.num_sources; ++i, ++chanref, ++trackref)
    {
        dlb_sadm_idref idrefs[128];

        blkfmts.num = 0;
        blkfmts.max = sizeof(idrefs)/sizeof(idrefs[0]);
        blkfmts.array = idrefs;
        
        if (   dlb_sadm_track_uid_lookup(c->sm, *trackref, &track)
            || dlb_sadm_channel_format_lookup(c->sm, *chanref, &chanfmt, &blkfmts)
            || dlb_sadm_block_format_lookup(c->sm, chanfmt.blkfmts.array[0], &blkfmt)
            || channel_target(&blkfmt, &sources[i].target)
           )
        {
            return FAILURE;
        }
        sources[i].source = (dlb_pmd_signal)track.channel_idx;
        sources[i].gain   = (dlb_pmd_gain)blkfmt.gain;
    }

#ifdef ADD_LFE_FOR_DOT_0
    if (add_lfe)
    {
        const unsigned int LFE_INDEX = 3;
        bed.num_sources += 1;
        for (i = bed.num_sources - 1; i != LFE_INDEX; --i)
        {
            sources[i].target = sources[i - 1].target;
            sources[i].source = sources[i - 1].source;
            sources[i].gain   = sources[i - 1].gain;
        }
        sources[LFE_INDEX].target = PMD_SPEAKER_LFE;
        /* TODO: is it really a good idea to assume channel 16 is correct here? */
        sources[LFE_INDEX].source = 16;  /* last channel of SDI (after reading S-ADM contains only zeros) */
        sources[LFE_INDEX].gain = 0;
    }
#endif
    
    if (dlb_pmd_set_bed(c->pmd, &bed))
    {
        printf("Error: failed to generate PMD bed for audioObject \"%s\"\n", object->name.data);
        return FAILURE;
    }
    return PMD_SUCCESS;
}


static inline
dlb_pmd_success
generate_content_classification
    (dlb_sadm_content_type ct
    ,dlb_pmd_object_class *c
    )
{
    if (  (ct < DLB_SADM_CONTENT_DK || ct > DLB_SADM_CONTENT_MK)
       && (ct != DLB_SADM_CONTENT_UNSET)
       )
    {
        printf("Error: no object can have content type %u\n", ct);
        return FAILURE;
    }

    switch (ct)
    {
        case DLB_SADM_CONTENT_DK_DIALOGUE:    *c = PMD_CLASS_DIALOG;          break;
        case DLB_SADM_CONTENT_DK_VOICEOVER:   *c = PMD_CLASS_VOICEOVER;       break;
        case DLB_SADM_CONTENT_DK_SPOKEN_SUB:  *c = PMD_CLASS_SUBTITLE;        break;
        case DLB_SADM_CONTENT_DK_AUD_DESC:    *c = PMD_CLASS_VDS;             break;
        case DLB_SADM_CONTENT_DK_COMMENTARY:  *c = PMD_CLASS_DIALOG;          break;
        case DLB_SADM_CONTENT_DK_EMERGENCY:   *c = PMD_CLASS_EMERGENCY_ALERT; break;
        default:                              *c = PMD_CLASS_GENERIC;         break;
    }
    return PMD_SUCCESS;
}


static
dlb_pmd_success
generate_pmd_object
    (pmd_sadm_ingester    *c
    ,dlb_sadm_content     *content
    ,dlb_sadm_object      *object
    ,dlb_sadm_pack_format *packfmt
    )
{
    dlb_sadm_track_uid track;
    dlb_sadm_idref trackref;
    dlb_sadm_idref chanref;
    dlb_sadm_channel_format chanfmt;
    dlb_sadm_block_format blkfmt;
    dlb_sadm_idref_array blkfmts;
    dlb_sadm_idref blkfmts_array[32];
    dlb_sadm_idref idref;

    dlb_pmd_object obj;
    unsigned int tmp;

    float x;
    float y;
    float z;

    static const double PI_OVER_180 = 3.1415926535897932384626433832795 / 180.0;

    blkfmts.num = 0;
    blkfmts.max = sizeof(blkfmts_array)/sizeof(blkfmts_array[0]);
    blkfmts.array = blkfmts_array;

    assert(object->track_uids.num == 1);
    assert(packfmt->chanfmts.num == 1);

    trackref = object->track_uids.array[0];
    if (dlb_sadm_track_uid_lookup(c->sm, trackref, &track))
    {
        return FAILURE;
    }

    chanref = packfmt->chanfmts.array[0];
    if (dlb_sadm_channel_format_lookup(c->sm, chanref, &chanfmt, &blkfmts))
    {
        return FAILURE;
    }
    
    /* Non-Dolby sources are sometimes generating 2, but we always use the first one */
    /*assert(chanfmt.blkfmts.num == 1);*/
    if (dlb_sadm_block_format_lookup(c->sm, chanfmt.blkfmts.array[0], &blkfmt))
    {
        return FAILURE;
    }

    /* TODO: do we apply the blkfmt gain somehow? */
    
    if (blkfmt.cartesian_coordinates)
    {
        x = blkfmt.azimuth_or_x;
        y = blkfmt.elevation_or_y;
        z = blkfmt.distance_or_z;
    }
    else
    {
        double xy_plane;

        if (blkfmt.distance_or_z == 0)
        {
            /* If the distance is zero it means it was not set 
             * so we should set it to default which is 1
             */
            blkfmt.distance_or_z = 1; 
        }
        xy_plane = fabs(blkfmt.distance_or_z * cos(blkfmt.elevation_or_y * PI_OVER_180));

        x = (float)(xy_plane * (sin(blkfmt.azimuth_or_x * PI_OVER_180) * -1.0));
        y = (float)(xy_plane * cos(blkfmt.azimuth_or_x * PI_OVER_180));
        z = (float)(blkfmt.distance_or_z * sin(blkfmt.elevation_or_y * PI_OVER_180));
    }
    
    if (generate_content_classification(content->type, &obj.object_class))
    {
        return FAILURE;
    }

    if (1 != sscanf((char*)object->id.data, "AO_%x", &tmp)
        || tmp < 0x1001
        || tmp > 0x1fff)
    {
        printf("Illegal object id \"%s\"\n", (char*)object->id.data);
        return FAILURE;
    }
    obj.id = (dlb_pmd_element_id)(tmp - 0x1000u);

    obj.dynamic_updates = 0;
    obj.x               = x;
    obj.y               = y;
    obj.z               = z;
    obj.size            = 0;
    obj.size_3d         = 0;
    obj.diverge         = 0;
    obj.source          = (dlb_pmd_signal)track.channel_idx;
    obj.source_gain     = object->gain;
    memmove(obj.name, object->name.data, sizeof(obj.name));

    (void)dlb_sadm_lookup_reference(c->sm, object->id.data, DLB_SADM_OBJECT, 0, &idref);
    if (dlb_pmd_set_object(c->pmd, &obj))
    {
        printf("Error: failed to generate PMD object for audioObject \"%s\"\n",
               (char*)object->name.data);
        return FAILURE;
    }
    return PMD_SUCCESS;
}


static
dlb_pmd_success
generate_pmd_element
    (pmd_sadm_ingester  *ingester
    ,dlb_sadm_content   *content
    ,dlb_sadm_idref      object_id
    )
{
    dlb_pmd_success success;
    dlb_sadm_pack_format packfmt;
    dlb_sadm_object object;

    dlb_sadm_idref_array chanfmts;
    dlb_sadm_idref_array object_refs;
    dlb_sadm_idref_array track_uids;
    dlb_sadm_idref chanfmts_array[DLB_PMD_MAX_BED_SOURCES];
    dlb_sadm_idref object_refs_array[MAX_AO_AO];
    dlb_sadm_idref track_uids_array[DLB_PMD_MAX_BED_SOURCES];

    track_uids.num = 0;
    track_uids.max = sizeof(track_uids_array)/sizeof(track_uids_array[0]);
    track_uids.array = track_uids_array;

    object_refs.num = 0;
    object_refs.max = sizeof(object_refs_array) / sizeof(object_refs_array[0]);
    object_refs.array = object_refs_array;

    chanfmts.num = 0;
    chanfmts.max = sizeof(chanfmts_array)/sizeof(chanfmts_array[0]);
    chanfmts.array = chanfmts_array;

    if (dlb_sadm_object_lookup(ingester->sm, object_id, &object, &object_refs, &track_uids))
    {
        return FAILURE;
    }

    if (object.pack_format != NULL)
    {
        if (dlb_sadm_pack_format_lookup(ingester->sm, object.pack_format, &packfmt, &chanfmts))
        {
            return FAILURE;
        }
    }
    else if (object.object_refs.num > 0)
    {
        size_t i;

        for (i = 0; i < object.object_refs.num; i++)
        {
            if (generate_pmd_element(ingester, content, object.object_refs.array[i]))
            {
                return FAILURE;
            }
        }
        return PMD_SUCCESS;
    }
    else
    {
        return FAILURE;
    }

    switch (packfmt.type)
    {
    case DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS:
        switch (packfmt.chanfmts.num)
        {
        case 0:
            success = PMD_FAIL;
            break;
        case 1:
            success = generate_pmd_object(ingester, content, &object, &packfmt);
            break;
        default:
            success = generate_pmd_bed(ingester, content, &object, &packfmt);
            break;
        }
        break;

    case DLB_SADM_PACKFMT_TYPE_OBJECT:
        success = generate_pmd_object(ingester, content, &object, &packfmt);
        break;

    default:
        success = PMD_FAIL;
        break;
    }

    return success;
}


static inline
dlb_pmd_bool
config_is_bed
    (dlb_pmd_speaker_config config
    )
{
    return config <= DLB_PMD_SPEAKER_CONFIG_LAST_BED;
}


static
dlb_pmd_success
add_object_elements
    (pmd_sadm_ingester *ingester
    ,dlb_sadm_idref object_id
    ,dlb_pmd_presentation *presentation
    )
{
    dlb_sadm_model *model = ingester->sm;
    dlb_sadm_object object;
    dlb_sadm_idref_array objectrefs;
    dlb_sadm_idref_array track_uids;
    dlb_sadm_idref object_refs_array[MAX_AO_AO];
    dlb_sadm_idref track_uids_array[DLB_PMD_MAX_BED_SOURCES];

    objectrefs.num = 0;
    objectrefs.max = sizeof(object_refs_array) / sizeof(object_refs_array[0]);
    objectrefs.array = object_refs_array;

    track_uids.num = 0;
    track_uids.max = sizeof(track_uids_array) / sizeof(track_uids_array[0]);
    track_uids.array = track_uids_array;

    if (dlb_sadm_object_lookup(model, object_id, &object, &objectrefs, &track_uids))
    {
        printf("Error while looking up audio object reference (add_object_elements)\n");
        return FAILURE;
    }

    if (object.object_refs.num == 0)    /* this is a leaf element */
    {
        dlb_pmd_bed bed;
        dlb_pmd_source sources[12];
        const unsigned int max_sources = sizeof(sources) / sizeof(sources[0]);
        unsigned int elidx;
        unsigned int elid;

        if (presentation->num_elements >= DLB_PMD_MAX_AUDIO_ELEMENTS)
        {
            printf("Error: too many elements in the presentation (add_object_elements)\n");
            return FAILURE;
        }

        if (sadm_to_pmd_lookup(object_id, &elid))
        {
            printf("Error: element id for object reference not found (add_object_elements)\n");
            return FAILURE;
        }

        elidx = presentation->num_elements++;
        presentation->elements[elidx] = (dlb_pmd_element_id)elid;
        if (!dlb_pmd_bed_lookup(ingester->pmd, presentation->elements[elidx], &bed, max_sources, sources))
        {
            if (presentation->config != bed.config)
            {
                dlb_pmd_bool use_larger = PMD_FALSE;

                /* presentation config is uninitialized, use the bed config */
                if (presentation->config == NUM_PMD_SPEAKER_CONFIGS)
                {
                    presentation->config = bed.config;
                }
                /* if both are beds, or both are not beds, use the larger value */
                else if (( config_is_bed(presentation->config) &&  config_is_bed(bed.config)) ||
                         (!config_is_bed(presentation->config) && !config_is_bed(bed.config)))
                {
                    use_larger = PMD_TRUE;
                }
                /* if the current object's format is a bed, and the presentation's is not, use the bed */
                else if (config_is_bed(bed.config))
                {
                    presentation->config = bed.config;
                }

                if (use_larger && bed.config > presentation->config)
                {
                    /* TODO: this is a little bothersome for portable v. headphones... */
                    presentation->config = bed.config;
                }
            }
        }
    }
    else        /* this is a compound object, recurse over the component objects */
    {
        dlb_pmd_success success;
        unsigned int i;

        for (i = 0; i < object.object_refs.num; i++)
        {
            success = add_object_elements(ingester, object.object_refs.array[i], presentation);
            if (success != PMD_SUCCESS)
            {
                return PMD_FAIL;
            }
        }
    }

    return PMD_SUCCESS;
}


static
dlb_pmd_success
add_content_elements
    (pmd_sadm_ingester *ingester
    ,dlb_sadm_idref content_id
    ,dlb_pmd_presentation *presentation
    )
{
    dlb_sadm_model *model = ingester->sm;
    dlb_sadm_content content;
    dlb_sadm_idref_array objectrefs;
    dlb_sadm_idref objectref_array[32];     /* TODO: symbolic constant */
    dlb_pmd_success success;
    size_t i;

    objectrefs.num = 0;
    objectrefs.max = sizeof(objectref_array) / sizeof(objectref_array[0]);
    objectrefs.array = objectref_array;

    if (dlb_sadm_content_lookup(model, content_id, &content, &objectrefs))
    {
        printf("Error while looking up audio content reference (add_content_elements)\n");
        return FAILURE;
    }

    for (i = 0; i < content.objects.num; i++)
    {
        success = add_object_elements(ingester, content.objects.array[i], presentation);
        if (success != PMD_SUCCESS)
        {
            return PMD_FAIL;
        }
    }

    return PMD_SUCCESS;
}


static
dlb_pmd_success
add_presentation_elements
    (pmd_sadm_ingester *ingester
    ,dlb_sadm_programme *programme
    ,dlb_pmd_presentation *presentation
    )
{
    dlb_pmd_success success;
    size_t i;

    presentation->num_elements = 0;
    for (i = 0; i < programme->contents.num; i++)
    {
        success = add_content_elements(ingester, programme->contents.array[i], presentation);
        if (success != PMD_SUCCESS)
        {
            presentation->num_elements = 0;
            return PMD_FAIL;
        }
    }

    return PMD_SUCCESS;
}


static
dlb_pmd_success
generate_pmd_presentation
    (pmd_sadm_ingester  *c
    ,dlb_sadm_programme *programme
    )
{
    dlb_pmd_element_id elements[DLB_PMD_MAX_AUDIO_ELEMENTS];
    dlb_pmd_presentation_name *name;
    dlb_pmd_presentation pres;
    unsigned int presid;
    
    dlb_sadm_idref idref;
    dlb_sadm_programme_label *label;
    unsigned int tmp;
    unsigned int i;

    if (1 != sscanf((char*)programme->id.data, "APR_%x", &tmp)
        || tmp < 0x1001
        || tmp > 0x11ff)
    {
        printf("Illegal programme id \"%s\"\n", (char*)programme->id.data);
        return FAILURE;
    }
    presid = tmp - 0x1000;

    (void)dlb_sadm_lookup_reference(c->sm, programme->id.data, DLB_SADM_PROGRAMME, 0, &idref);
    pres.id             = (dlb_pmd_presentation_id)presid;
    pres.config         = NUM_PMD_SPEAKER_CONFIGS;  /* TODO: after adding elements, make certain we've set this to a specific config */
    pres.num_elements   = 0;
    pres.elements       = elements;
    pres.num_names      = programme->num_labels;

    memmove(pres.audio_language, programme->language, sizeof(pres.audio_language));

    if (add_presentation_elements(c, programme, &pres))
    {
        return PMD_FAIL;
    }

    if (pres.num_names > DLB_PMD_MAX_PRESENTATION_NAMES)
    {
        pres.num_names = DLB_PMD_MAX_PRESENTATION_NAMES;
    }
    name = pres.names;
    label = programme->labels;
    for (i = 0; i != pres.num_names; ++i, ++name, ++label)
    {
        memmove(name->language, label->language, sizeof(name->language));
        memmove(name->text, label->name.data, sizeof(name->text));
    }

    if (dlb_pmd_set_presentation(c->pmd, &pres))
    {
        printf("Error: failed to add presentation\n");
        return FAILURE;
    }
    return PMD_SUCCESS;
}


static
dlb_pmd_success
ingest_track_uids
    (pmd_sadm_ingester *c
    )
{
    dlb_sadm_track_uid_iterator tui;
    dlb_sadm_track_uid track_uid;
    dlb_sadm_idref idref;
    
    if (dlb_sadm_track_uid_iterator_init(&tui, c->sm))
    {
        return FAILURE;
    }

    while (!dlb_sadm_track_uid_iterator_next(&tui, &track_uid))
    {
        dlb_pmd_bool is_common;

        if (dlb_sadm_track_uid_is_common_def(c->sm, &track_uid, &is_common))
        {
            return PMD_FAIL;
        }
        if (is_common && track_uid.channel_idx == 0)
        {
            /* If the track uid is a common def and has no definite channel assignment, skip it */
            continue;
        }

        if (dlb_sadm_lookup_reference(c->sm, track_uid.id.data, DLB_SADM_TRACKUID, 0, &idref) ||
            dlb_pmd_add_signal(c->pmd, (dlb_pmd_signal)track_uid.channel_idx))
        {
            return FAILURE;
        }
    }

    return PMD_SUCCESS;
}


static
dlb_pmd_success
ingest_content
    (pmd_sadm_ingester *c
    )
{
    dlb_sadm_content_iterator ci;
    dlb_sadm_content content;

    dlb_sadm_content_iterator_init(&ci, c->sm);
    while (!dlb_sadm_content_iterator_next(&ci, &content))
    {
        size_t i;

        for (i = 0; i < content.objects.num; i++)
        {
            if (generate_pmd_element(c, &content, content.objects.array[i]))
            {
                return FAILURE;
            }
        }
    }
    return PMD_SUCCESS;
}


static
dlb_pmd_success
ingest_programmes
    (pmd_sadm_ingester *c
    )
{
#define MAX_LABELS (128)
    dlb_sadm_programme_label labels[MAX_LABELS];
    dlb_sadm_programme_iterator pi;
    dlb_sadm_programme programme;
    dlb_sadm_idref contents_array[128];
    dlb_sadm_idref_array contents;

    contents.num   = 0;
    contents.max   = ARRAYSZ(contents_array);
    contents.array = contents_array;

    dlb_sadm_programme_iterator_init(&pi, c->sm);
    while (!dlb_sadm_programme_iterator_next(&pi, &programme, &contents, labels, MAX_LABELS))
    {
        if (generate_pmd_presentation(c, &programme))
        {
            return FAILURE;
        }
    }
    return PMD_SUCCESS;
#undef MAX_LABELS
}


static
dlb_pmd_success
ingest_frame_format
    (pmd_sadm_ingester *c
    )
{
    dlb_pmd_success success;
    char uuid[37];

    success = dlb_sadm_get_flow_id(c->sm, uuid, sizeof(uuid));
    if (success == PMD_SUCCESS && uuid[0] != '\0')
    {
        success = 
            dlb_pmd_iat_add(c->pmd, 0) ||
            dlb_pmd_iat_content_id_uuid(c->pmd, uuid);
    }

    return success;
}


/* ----------------------------- public API ------------------------- */

size_t
pmd_sadm_ingester_query_mem
    (dlb_sadm_counts *sc
    )
{
    (void)sc;
    return ALIGN_TO_MPTR(sizeof(pmd_sadm_ingester));
}


dlb_pmd_success
pmd_sadm_ingester_init
    (pmd_sadm_ingester **cptr
    ,void *mem
    ,dlb_sadm_model *sadm
    )
{
    pmd_sadm_ingester *c = (pmd_sadm_ingester *)mem;
    uintptr_t mc = (uintptr_t)mem;
    dlb_sadm_counts sc;
    
    if (dlb_sadm_model_limits(sadm, &sc))
    {
        return FAILURE;
    }

    memset(c, '\0', sizeof(*c));

    mc += ALIGN_TO_MPTR(sizeof(pmd_sadm_ingester));

    c->sm = sadm;
    *cptr = c;
    return PMD_SUCCESS;
}


void
pmd_sadm_ingester_finish
   (pmd_sadm_ingester *c
   )
{
    (void)c;
}


dlb_pmd_success
pmd_sadm_ingester_ingest
    (pmd_sadm_ingester  *c
    ,const char *title
    ,dlb_pmd_model *pmd
    )
{
    c->pmd = pmd;
    dlb_pmd_reset(pmd);
    return dlb_pmd_set_title(pmd, title)
        || ingest_track_uids(c)
        || ingest_content(c)
        || ingest_programmes(c)
        || ingest_frame_format(c);
}

