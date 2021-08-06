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
 * @file dlb_pmd_capture.c
 * @brief Capture metadata from a frame of audio input -- implementation
 */

#include "dlb_pmd/include/dlb_pmd_capture.h"
#include "dlb_pmd/include/dlb_pmd_pcm.h"
#include "pmd_profile.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifdef NDEBUG
#define CHECK_STATUS(s) if (s) return (s)
#else
static int retstat(int s)
{
    return s;   // Put a breakpoint here
}
#define CHECK_STATUS(s) if (s) return retstat((int)s)
#endif

#define MAX_FRAME_BUFFER_SAMPLES    (5000)
#define MAX_AES_3_CHANNELS          (2)
#define NO_METADATA                 (0xFFFF)
#define PMD_PA_BLOCK_SPACING        (160)
#define PMD_GUARD_BAND_SIZE         (32)
#define PMD_2ND_BLOCK_SPACING       (PMD_PA_BLOCK_SPACING - PMD_GUARD_BAND_SIZE)


#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

/**
 * @brief SMPTE 337m sync words
 */
typedef enum
{
    PA_16 = 0xf8720000,   /* IEC 958 preamble a (sync word 1)/bit depth 16 */
    PA_20 = 0x6f872000,   /* IEC 958 preamble a (sync word 1)/bit depth 20 */
    PA_24 = 0x96f87200,   /* IEC 958 preamble a (sync word 1)/bit depth 24 */
    PB_16 = 0x4e1f0000,   /* IEC 958 preamble b (sync word 2)/bit depth 16 */
    PB_20 = 0x54e1f000,   /* IEC 958 preamble b (sync word 2)/bit depth 20 */
    PB_24 = 0xa54e1f00,   /* IEC 958 preamble b (sync word 2)/bit depth 24 */
} PREAMBLE_WORDS;

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif


typedef uint32_t     buffer_data_type;
typedef uint16_t     feature_location_type;

typedef enum
{
    CAPTOR_STATE_UNKNOWN,
    CAPTOR_STATE_CLEAR,
    CAPTOR_STATE_CAPTURING,
    CAPTOR_STATE_GOOD,
    CAPTOR_STATE_DESTROYED
} CAPTOR_STATE;

struct dlb_pmd_frame_captor
{
    dlb_pmd_model_combo             *model;
    dlb_pcmpmd_extractor            *extractor;
    dlb_pmd_metadata_set            *metadata_set;

    void                            *model_memory;
    void                            *extractor_memory;
    void                            *metadata_set_memory;
    size_t                           model_memory_size;
    size_t                           extractor_memory_size;
    size_t                           metadata_set_memory_size;

    const dlb_pmd_blob_descriptor   *blob_descriptor;
    const void                      *blob;

    buffer_data_type                 data_buffer[MAX_FRAME_BUFFER_SAMPLES * MAX_AES_3_CHANNELS];
    size_t                           channel_count;
    size_t                           sample_count;

    uint16_t                         metadata_channel_index;
    dlb_pmd_bool                     is_pair;
    feature_location_type            pa_locations[MAX_FRAME_BUFFER_SAMPLES / PMD_PA_BLOCK_SPACING + 1];
    size_t                           pa_count;

    feature_location_type            frame_start;
    feature_location_type            frame_end;
    dlb_pmd_frame_rate               frame_rate;

    dlb_pmd_bool                     mallocated;
    CAPTOR_STATE                     captor_state;
};

static
size_t
get_model_memory_size
    (void
    )
{
    return dlb_pmd_model_combo_query_mem(NULL, NULL);
}

static
size_t
get_extractor_memory_size
    (void
    )
{
    return dlb_pcmpmd_extractor_query_mem(PMD_TRUE);
}

static
size_t
get_metadata_set_memory_size
    (void
    )
{
    return dlb_pmd_metadata_set_max_memory();
}

int
dlb_pmd_frame_captor_query_memory_size
    (size_t *sz
    )
{
    if (sz == NULL)
    {
        return DLB_PMD_FRAME_CAPTOR_STATUS_NULL_POINTER;
    }

    *sz = sizeof(dlb_pmd_frame_captor) + get_model_memory_size() + get_extractor_memory_size() + get_metadata_set_memory_size();

    return DLB_PMD_FRAME_CAPTOR_STATUS_OK;
}

static
void
free_memory
    (dlb_pmd_frame_captor *captor
    )
{
    if (captor != NULL)
    {
        if (captor->extractor)
        {
            dlb_pcmpmd_extractor_finish(captor->extractor);
        }
        if (captor->model != NULL)
        {
            (void)dlb_pmd_model_combo_destroy(&captor->model);
        }
        if (captor->mallocated)
        {
            if (captor->metadata_set_memory != NULL)
            {
                free(captor->metadata_set_memory);
            }
            if (captor->extractor_memory != NULL)
            {
                free(captor->extractor_memory);
            }
            if (captor->model_memory != NULL)
            {
                free(captor->model_memory);
            }
            memset(captor, 0, sizeof(*captor));
            captor->captor_state = CAPTOR_STATE_DESTROYED;
            free(captor);
        }
        else
        {
            memset(captor, 0, sizeof(*captor));
            captor->captor_state = CAPTOR_STATE_DESTROYED;
        }
    }
}

static
void
frame_captor_clear
    (dlb_pmd_frame_captor *captor
    )
{
    if (captor != NULL)
    {
        if (captor->model)
        {
            dlb_pmd_model_combo_clear(captor->model);
        }
        else
        {
            memset(captor->model_memory, 0, captor->model_memory_size);
        }

        if (captor->extractor)
        {
            dlb_pcmpmd_extractor_finish(captor->extractor);
        }
        memset(captor->extractor_memory, 0, captor->extractor_memory_size);
        captor->extractor = NULL;

        memset(captor->metadata_set_memory, 0, captor->metadata_set_memory_size);
        captor->metadata_set = NULL;

        captor->blob_descriptor = NULL;
        captor->blob = NULL;

        memset(captor->data_buffer, 0, sizeof(captor->data_buffer));
        captor->channel_count = 0;
        captor->sample_count = 0;

        captor->metadata_channel_index = NO_METADATA;
        captor->is_pair = PMD_FALSE;
        memset(captor->pa_locations, 0, sizeof(captor->pa_locations));
        captor->pa_count = 0;

        captor->frame_start = NO_METADATA;
        captor->frame_end = NO_METADATA;
        captor->frame_rate = NO_METADATA;

        captor->captor_state = CAPTOR_STATE_CLEAR;
    }
}

int
dlb_pmd_frame_captor_open
    (dlb_pmd_frame_captor   **captor
    ,void                    *memory
    )
{
    int status = DLB_PMD_FRAME_CAPTOR_STATUS_OK;
    dlb_pmd_bool mallocate = (memory == NULL);
    uint8_t *mem = (uint8_t *)memory;
    dlb_pmd_frame_captor *fc;
    size_t sz;

    if (captor == NULL)
    {
        return DLB_PMD_FRAME_CAPTOR_STATUS_NULL_POINTER;
    }

    /* captor structure */
    if (mallocate)
    {
        fc = malloc(sizeof(*fc));
        if (fc == NULL)
        {
            return DLB_PMD_FRAME_CAPTOR_STATUS_OUT_OF_MEMORY;
        }
    } 
    else
    {
        fc = (dlb_pmd_frame_captor *)mem;
        mem += sizeof(*fc);
    }
    memset(fc, 0, sizeof(*fc));

    /* model memory and model */
    sz = get_model_memory_size();
    if (sz == 0)
    {
        status = DLB_PMD_FRAME_CAPTOR_STATUS_ERROR;
        goto err;
    }
    fc->model_memory_size = sz;
    if (mallocate)
    {
        fc->model_memory = malloc(sz);
        if (fc->model_memory == NULL)
        {
            status = DLB_PMD_FRAME_CAPTOR_STATUS_OUT_OF_MEMORY;
            goto err;
        }
    } 
    else
    {
        fc->model_memory = mem;
        mem += sz;
    }
    if (dlb_pmd_model_combo_init(&fc->model, NULL, NULL, PMD_FALSE, fc->model_memory))
    {
        status = DLB_PMD_FRAME_CAPTOR_STATUS_ERROR;
        goto err;
    }

    /* extractor memory */
    sz = get_extractor_memory_size();
    if (sz == 0)
    {
        status = DLB_PMD_FRAME_CAPTOR_STATUS_ERROR;
        goto err;
    }
    fc->extractor_memory_size = sz;
    if (mallocate)
    {
        fc->extractor_memory = malloc(sz);
        if (fc->extractor_memory == NULL)
        {
            status = DLB_PMD_FRAME_CAPTOR_STATUS_OUT_OF_MEMORY;
            goto err;
        }
    }
    else
    {
        fc->extractor_memory = mem;
        mem += sz;
    }

    /* metadata set memory */
    sz = get_metadata_set_memory_size();
    if (sz == 0)
    {
        status = DLB_PMD_FRAME_CAPTOR_STATUS_ERROR;
        goto err;
    }
    fc->metadata_set_memory_size = sz;
    if (mallocate)
    {
        fc->metadata_set_memory = malloc(sz);
        if (fc->metadata_set_memory == NULL)
        {
            status = DLB_PMD_FRAME_CAPTOR_STATUS_OUT_OF_MEMORY;
            goto err;
        }
    } 
    else
    {
        fc->metadata_set_memory = mem;
    }

    fc->mallocated = mallocate;
    frame_captor_clear(fc);
    *captor = fc;

    return status;

err:
    free_memory(fc);

    return status;
}

static
dlb_pmd_success
check_descriptor
    (const dlb_pmd_blob_descriptor *descriptor
    )
{
    unsigned int min_frame_size = dlb_pcmpmd_min_frame_size(DLB_PMD_FRAMERATE_LAST);
    dlb_pmd_success success = PMD_FAIL;

    if (descriptor == NULL ||
        descriptor->number_of_samples < min_frame_size ||
        descriptor->number_of_channels == 0)
    {
        goto done;
    }

    switch (descriptor->bit_depth)
    {
    case 24:
    case 32:
        break;

    default:
        goto done;
    }

    success = PMD_SUCCESS;
done:
    return success;
}

static
int
convert_sample
    (dlb_pmd_frame_captor   *captor
    ,buffer_data_type       *dest
    ,void                   *src
    )
{
    /* NOTE: this function assumes it is running on a little-endian architecture. */

    buffer_data_type cvt = 0;
    uint8_t shift;
    uint8_t *p;
    size_t i;

    if (captor == NULL || dest == NULL || src == NULL || captor->blob_descriptor == NULL)
    {
        return DLB_PMD_FRAME_CAPTOR_STATUS_NULL_POINTER;
    }

    p = (uint8_t *)src;
    if (captor->blob_descriptor->big_endian)        /* Network byte order */
    {
        switch (captor->blob_descriptor->bit_depth)
        {
        case 32:
            /* Assume 32-bit big-endian has 24 bits of audio (e.g., AM824 subframe) */
            ++p;
            /*** FALL THROUGH!!! ***/
        case 24:
            shift = CHAR_BIT * 3;
            for (i = 0; i < 3; i++)
            {
                cvt += (*p++ << shift);
                shift -= CHAR_BIT;
            }
            break;

        default:
            return DLB_PMD_FRAME_CAPTOR_STATUS_ERROR;
        }
    } 
    else                                            /* Wave file byte order */
    {
        switch (captor->blob_descriptor->bit_depth)
        {
        case 32:
            cvt = *((buffer_data_type *)p);
            break;

        case 24:
            shift = CHAR_BIT;
            for (i = 0; i < 3; i++)
            {
                cvt += (*p++ << shift);
                shift += CHAR_BIT;
            }
            break;

        default:
            return DLB_PMD_FRAME_CAPTOR_STATUS_ERROR;
        }
    }

    *dest = cvt;

    return DLB_PMD_FRAME_CAPTOR_STATUS_OK;
}

static
int
convert_channels
    (dlb_pmd_frame_captor   *captor
    ,size_t                  first_channel_index
    )
{
    uint8_t *src_start;
    buffer_data_type *dest_start;
    size_t channel_count;
    size_t last_channel_index;
    size_t blob_count;
    size_t data_buffer_count;
    size_t sample_count;
    size_t src_sample_sz;
    size_t src_stride;
    size_t channel;
    size_t sample;

    if (captor == NULL || captor->blob == NULL || captor->blob_descriptor == NULL)
    {
        return DLB_PMD_FRAME_CAPTOR_STATUS_NULL_POINTER;
    }
    channel_count = captor->channel_count;
    last_channel_index = first_channel_index + channel_count - 1;

    if (channel_count < 1 ||
        first_channel_index >= captor->blob_descriptor->number_of_channels ||
        last_channel_index  >= captor->blob_descriptor->number_of_channels)
    {
        return DLB_PMD_FRAME_CAPTOR_STATUS_ERROR;
    }

    blob_count = captor->blob_descriptor->number_of_samples;
    data_buffer_count = (sizeof(captor->data_buffer) / sizeof(captor->data_buffer[0])) / channel_count;
    sample_count = ((blob_count < data_buffer_count) ? blob_count : data_buffer_count);
    src_sample_sz = captor->blob_descriptor->bit_depth / CHAR_BIT;
    src_stride = captor->blob_descriptor->number_of_channels * src_sample_sz;
    dest_start = captor->data_buffer;
    src_start = (uint8_t *)captor->blob + first_channel_index * src_sample_sz;
    for (channel = 0; channel < channel_count; channel++)
    {
        buffer_data_type *dest = dest_start + channel;
        uint8_t *src = src_start + channel * src_sample_sz;

        for (sample = 0; sample < sample_count; sample++)
        {
            int status = convert_sample(captor, dest, src);
            CHECK_STATUS(status);
            dest += channel_count;  /* i.e., stride */
            src += src_stride;
        }
    }
    captor->sample_count = sample_count;

    return DLB_PMD_FRAME_CAPTOR_STATUS_OK;
}

static
int
scan_for_pa
    (dlb_pmd_frame_captor *captor
    )
{
    size_t stride;
    size_t current_sample;
    size_t last_sample;
    buffer_data_type *p;

    if (captor == NULL)
    {
        return DLB_PMD_FRAME_CAPTOR_STATUS_NULL_POINTER;
    }

    if (captor->channel_count < 1 || captor->channel_count > 2 || captor->sample_count < 2)
    {
        return DLB_PMD_FRAME_CAPTOR_STATUS_ERROR;
    }

    p = captor->data_buffer;
    stride = captor->channel_count;
    last_sample = captor->sample_count - 2;
    for (current_sample = 0; current_sample < last_sample; current_sample++)
    {
        buffer_data_type v0 = p[0];
        buffer_data_type v1 = p[1];
        buffer_data_type v2 = p[stride];

        if ((v0 == PA_16 && v1 == PB_16) ||
            (v0 == PA_20 && v1 == PB_20) ||
            (v0 == PA_24 && v1 == PB_24))
        {
            captor->pa_locations[captor->pa_count++] = (feature_location_type)current_sample;
            captor->is_pair = (stride > 1);
        }
        else if ((v0 == PA_16 && v2 == PB_16) ||
                 (v0 == PA_20 && v2 == PB_20) ||
                 (v0 == PA_24 && v2 == PB_24))
        {
            captor->pa_locations[captor->pa_count++] = (feature_location_type)current_sample;
            captor->is_pair = PMD_FALSE;
        }
        p += stride;
    }

    return DLB_PMD_FRAME_CAPTOR_STATUS_OK;
}

static
int
find_metadata_channel
    (dlb_pmd_frame_captor *captor
    )
{
    size_t last_channel;
    size_t channel;
    int status;

    if (captor == NULL || captor->blob_descriptor == NULL)
    {
        return DLB_PMD_FRAME_CAPTOR_STATUS_NULL_POINTER;
    }

    captor->channel_count = 2;
    last_channel = captor->blob_descriptor->number_of_channels - 1;
    for (channel = 0; channel < last_channel; channel += 2)
    {
        status = convert_channels(captor, channel);
        CHECK_STATUS(status);
        status = scan_for_pa(captor);
        CHECK_STATUS(status);
        if (captor->pa_count > 0)
        {
            goto found;
        }
    }

    captor->channel_count = 1;
    channel = last_channel;
    status = convert_channels(captor, channel);
    CHECK_STATUS(status);
    status = scan_for_pa(captor);
    CHECK_STATUS(status);
    if (captor->pa_count > 0)
    {
        goto found;
    }

    return DLB_PMD_FRAME_CAPTOR_STATUS_NOT_FOUND;

found:
    captor->metadata_channel_index = (uint16_t)channel;

    return status;
}

static
dlb_pmd_bool
find_frame_rate
    (dlb_pmd_frame_rate     *frame_rate
    ,feature_location_type   frame_start
    ,feature_location_type   frame_end
    )
{
    dlb_pmd_bool found = PMD_FALSE;

    if (frame_rate != NULL)
    {
        feature_location_type frame_length = frame_end - frame_start;

        *frame_rate = NO_METADATA;
        switch (frame_length)
        {
        case 2002:
        case 2001:  /* TODO: is this correct? */
            *frame_rate = DLB_PMD_FRAMERATE_2398;
            found = PMD_TRUE;
            break;

        case 2000:
            *frame_rate = DLB_PMD_FRAMERATE_2400;
            found = PMD_TRUE;
            break;

        case 1920:
            *frame_rate = DLB_PMD_FRAMERATE_2500;
            found = PMD_TRUE;
            break;

        case 1602:
        case 1601:
            *frame_rate = DLB_PMD_FRAMERATE_2997;
            found = PMD_TRUE;
            break;

        case 1600:
            *frame_rate = DLB_PMD_FRAMERATE_3000;
            found = PMD_TRUE;
            break;

        case 960:
            *frame_rate = DLB_PMD_FRAMERATE_5000;
            found = PMD_TRUE;
            break;

        case 800:   /* This is ambiguous, could be DLB_PMD_FRAMERATE_5994 */
            *frame_rate = DLB_PMD_FRAMERATE_6000;
            found = PMD_TRUE;
            break;

        case 480:
            *frame_rate = DLB_PMD_FRAMERATE_10000;
            found = PMD_TRUE;
            break;

        case 400:   /* This is ambiguous, could be DLB_PMD_FRAMERATE_11988 */
            *frame_rate = DLB_PMD_FRAMERATE_12000;
            found = PMD_TRUE;
            break;

        default:
            break;
        }
    }

    return found;
}

static
int
find_frame
    (dlb_pmd_frame_captor *captor
    )
{
    feature_location_type frame_start = NO_METADATA;
    feature_location_type frame_end = NO_METADATA;
    feature_location_type *pa = captor->pa_locations;
    size_t pa_count = captor->pa_count;
    int status = DLB_PMD_FRAME_CAPTOR_STATUS_NOT_FOUND;

    if (pa_count == 0)
    {
        return status;
    }

    if (pa[0] < PMD_GUARD_BAND_SIZE)    /* We need a full guardband before the first Pa */
    {
        --pa_count;
        ++pa;
    }

    /* For current PMD, a video frame will have a minimum of two payload blocks spaced a known
     * and unique distance apart.  Look for that pattern to identify the beginning of a frame.
     * Older PMD and sADM will have Pa spacing at the video frame rate.
     */

    if (pa_count >= 3)                  /* Look for current PMD Pa spacing */
    {
        dlb_pmd_bool found = PMD_FALSE;
        size_t last_pa_index = pa_count - 1;
        size_t i = 0;
        size_t j = 1;

        for (i = 0, j = 1; j < last_pa_index; i++, j++)         /* Find the frame start */
        {
            if (pa[j] - pa[i] == PMD_2ND_BLOCK_SPACING)
            {
                frame_start = pa[i] - PMD_GUARD_BAND_SIZE;
                found = PMD_TRUE;
                break;
            }
        }

        if (found)
        {
            size_t k;

            found = PMD_FALSE;
            for (k = j + 1; k <= last_pa_index; j++, k++)       /* Find the frame end */
            {
                if (pa[k] - pa[j] > PMD_PA_BLOCK_SPACING)
                {
                    frame_end = pa[k] - PMD_GUARD_BAND_SIZE;
                    found = PMD_TRUE;
                    break;
                }
            }

            if (found && find_frame_rate(&captor->frame_rate, frame_start, frame_end))
            {
                captor->frame_start = frame_start;
                captor->frame_end = frame_end;
                status = DLB_PMD_FRAME_CAPTOR_STATUS_OK;
            }
        }
    }

    if (status == DLB_PMD_FRAME_CAPTOR_STATUS_NOT_FOUND && pa_count >= 2)   /* We need at least two points to determine frame size */
    {
        frame_start = pa[0] - PMD_GUARD_BAND_SIZE;
        frame_end = pa[1] - PMD_GUARD_BAND_SIZE;
        if (find_frame_rate(&captor->frame_rate, frame_start, frame_end))
        {
            captor->frame_start = frame_start;
            captor->frame_end = frame_end;
            status = DLB_PMD_FRAME_CAPTOR_STATUS_OK;
        }
    }

    return status;
}

static
int
extract_model
    (dlb_pmd_frame_captor *captor
    )
{
    int status = DLB_PMD_FRAME_CAPTOR_STATUS_ERROR;

    if (captor == NULL)
    {
        return DLB_PMD_FRAME_CAPTOR_STATUS_NULL_POINTER;
    }

    dlb_pcmpmd_extractor_init2
    (
        &captor->extractor,
        captor->extractor_memory,
        captor->frame_rate,
        0,
        (unsigned int)captor->channel_count,
        captor->is_pair,
        captor->model,
        NULL,
        PMD_TRUE
    );

    status = dlb_pcmpmd_extract
    (
        captor->extractor,
        captor->data_buffer + captor->frame_start * captor->channel_count,
        captor->frame_end - captor->frame_start,
        0
    );

    return status;
}

static
int
create_metadata_set
    (dlb_pmd_frame_captor *captor
    )
{
    const dlb_pmd_model *pmd_model;
    int status;

    if (captor == NULL)
    {
        return DLB_PMD_FRAME_CAPTOR_STATUS_NULL_POINTER;
    }

    if (dlb_pmd_model_combo_ensure_readable_pmd_model(captor->model, &pmd_model, PMD_FALSE))
    {
        return DLB_PMD_FRAME_CAPTOR_STATUS_ERROR;
    }

    status = dlb_pmd_create_metadata_set(pmd_model, captor->metadata_set_memory, &captor->metadata_set);

    return status;
}

int
dlb_pmd_frame_captor_capture
    (dlb_pmd_metadata_set           **metadata_set
    ,dlb_pmd_frame_captor            *captor
    ,const dlb_pmd_blob_descriptor   *descriptor
    ,const void                      *data_blob
    )
{
    int status = DLB_PMD_FRAME_CAPTOR_STATUS_ERROR;

    if (metadata_set == NULL ||
        captor       == NULL ||
        data_blob    == NULL)
    {
        return DLB_PMD_FRAME_CAPTOR_STATUS_NULL_POINTER;
    }

    if (captor->captor_state         <  CAPTOR_STATE_CLEAR  ||
        captor->captor_state         >  CAPTOR_STATE_GOOD   ||
        check_descriptor(descriptor) != PMD_SUCCESS)
    {
        return DLB_PMD_FRAME_CAPTOR_STATUS_ERROR;
    }

    frame_captor_clear(captor);
    captor->captor_state = CAPTOR_STATE_CAPTURING;
    captor->blob_descriptor = descriptor;
    captor->blob = data_blob;
    status = find_metadata_channel(captor);
    CHECK_STATUS(status);
    status = find_frame(captor);
    CHECK_STATUS(status);
    status = extract_model(captor);
    CHECK_STATUS(status);
    status = create_metadata_set(captor);
    CHECK_STATUS(status);
    captor->captor_state = CAPTOR_STATE_GOOD;

    *metadata_set = captor->metadata_set;

    return status;
}

int
dlb_pmd_frame_captor_close
    (dlb_pmd_frame_captor   **captor
    )
{
    dlb_pmd_frame_captor *fc;
    CAPTOR_STATE state;

    if (captor == NULL || *captor == NULL)
    {
        return DLB_PMD_FRAME_CAPTOR_STATUS_NULL_POINTER;
    }
    fc = *captor;

    state = fc->captor_state;
    if (state < CAPTOR_STATE_CLEAR || state > CAPTOR_STATE_GOOD)
    {
        return DLB_PMD_FRAME_CAPTOR_STATUS_ERROR;
    }

    if (fc->mallocated)
    {
        free_memory(fc);
    }
    else
    {
        frame_captor_clear(fc);
        fc->captor_state = CAPTOR_STATE_DESTROYED;
    }
    *captor = NULL;

    return DLB_PMD_FRAME_CAPTOR_STATUS_OK;
}
