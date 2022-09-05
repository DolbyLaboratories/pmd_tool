/************************************************************************
 * dlb_wave
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

#include "dlb_wave_tests.h"
#include "dlb_octfile/include/dlb_octfile.h"
#include "dlb_wave/include/dlb_wave.h"
#include "dlb_wave/include/dlb_wave_float.h"
#include "dlb_buffer/include/dlb_buffer.h"
#include "munit/include/eval.h"
#include "buftype_dlb_wave.h"
#include "buftype_dlb_buffer.h"
#include "buftype_util.h"
#include "memwave.h"
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#define MAX_CHANNELS_IN_TEST_DATA (6)

typedef struct {
    const unsigned char       *data;
    size_t                     data_size;
    unsigned                   format_flags;
} dlb_wave_read_test;

static const int   buffer_format_codes[] = {DLB_BUFFER_SHORT_16, DLB_BUFFER_INT_LEFT, DLB_BUFFER_LONG_32, DLB_BUFFER_FLOAT, DLB_BUFFER_DOUBLE};
static const char *buffer_format_names[] = {"DLB_BUFFER_SHORT_16", "DLB_BUFFER_INT_LEFT", "DLB_BUFFER_LONG_32", "DLB_BUFFER_FLOAT", "DLB_BUFFER_DOUBLE"};

static unsigned get_dlb_buffer_mant_bits(int buffer_code)
{
    buffer_type bt;
    get_dlb_buffer_type(&bt, buffer_code);
    return bt.nb_mant_bits;
}

static const char *get_dlb_wave_buffer_type_from_memory(buffer_type *bt, const void *p_data, size_t data_size)
{
    dlb_wave_format fmt;
    int b_is_float;
    const char *p_err;

    p_err =
        memwave_load_format
            (p_data
            ,data_size
            ,&fmt
            ,&b_is_float
            );
    if (p_err)
    {
        return p_err;
    }

    return
        get_dlb_wave_buffer_type
            (bt
            ,&fmt
            ,b_is_float ? DLB_WAVE_FLOAT : 0
            );
}

/* This function tests:
 *   dlb_wave_get_format()
 *   dlb_wave_get_sample_rate()
 *   dlb_wave_get_channel_count()
 *   dlb_wave_get_bit_depth()
 *   dlb_wave_get_channel_mask()
 *   dlb_wave_get_format_flags()
 * If any of these functions produce unexpected output, the function will
 * return non-zero. Otherwise, the function returns zero. */
static
int
dlb_wave_test_query_apis
    (const munit_test_case     *p_def
    ,const munit_collator      *p_collator
    ,const dlb_wave_format     *ref_fmt
    ,int                        non_extensible_additional_flags
    ,dlb_wave_file             *p_wave
    )
{
    int test_failed = 0;
    const dlb_wave_format *fmt       = dlb_wave_get_format(p_wave);
    int ref_format_flags =
        (   (non_extensible_additional_flags)
        |   ((ref_fmt->format_type == 0xFFFEu) ? DLB_WAVE_EXTENSIBLE : 0)
        );

#define STORE_AND_COMPARE_TYPE(dut, ref, name, typename, printfformat) \
    { \
        const typename refstore = ref; \
        const typename dutstore = dut; \
        if (refstore != dutstore) \
        { \
            test_failed = 1; \
            p_collator->fail \
                (p_def \
                ,p_collator->context \
                ,#name " (expected: " printfformat ", got: " printfformat ")" \
                ,refstore \
                ,dutstore \
                ); \
        } \
        else \
        { \
            p_collator->pass \
                (p_def \
                ,p_collator->context \
                ); \
        } \
    }

#define FORMAT_STRUCTURE_COMPARISON(member, typename, format_flags) \
    STORE_AND_COMPARE_TYPE(fmt->member, ref_fmt->member, "unexpected dlb_wave_get_format()->" #member, typename, format_flags)

    STORE_AND_COMPARE_TYPE(dlb_wave_get_sample_rate(p_wave),   ref_fmt->sample_rate,     "dlb_wave_get_sample_rate",   unsigned long, "%lu")
    STORE_AND_COMPARE_TYPE(dlb_wave_get_channel_count(p_wave), ref_fmt->channel_count,   "dlb_wave_get_channel_count", unsigned,      "%u")
    STORE_AND_COMPARE_TYPE(dlb_wave_get_bit_depth(p_wave),     ref_fmt->bits_per_sample, "dlb_wave_get_bit_depth",     unsigned,      "%u")
    STORE_AND_COMPARE_TYPE(dlb_wave_get_channel_mask(p_wave),  ref_fmt->channel_mask,    "dlb_wave_get_channel_mask",  unsigned long, "%lu")
    STORE_AND_COMPARE_TYPE(dlb_wave_get_format_flags(p_wave),  ref_format_flags,         "dlb_wave_get_format_flags",  unsigned,      "%u")
    FORMAT_STRUCTURE_COMPARISON(format_type,           unsigned short, "%hu");
    FORMAT_STRUCTURE_COMPARISON(channel_count,         unsigned short, "%hu");
    FORMAT_STRUCTURE_COMPARISON(sample_rate,           unsigned long,  "%lu");
    FORMAT_STRUCTURE_COMPARISON(bytes_per_second,      unsigned long,  "%lu");
    FORMAT_STRUCTURE_COMPARISON(block_alignment,       unsigned short, "%hu");
    FORMAT_STRUCTURE_COMPARISON(bits_per_sample,       unsigned short, "%hu");
    FORMAT_STRUCTURE_COMPARISON(octets_per_sample,     unsigned short, "%hu");
    FORMAT_STRUCTURE_COMPARISON(valid_bits_per_sample, unsigned short, "%hu");
    FORMAT_STRUCTURE_COMPARISON(channel_mask,          unsigned long,  "%lu");

    return test_failed;
}

static
int
dlb_wave_test_query_apis_from_test
    (const munit_test_case     *p_def
    ,const munit_collator      *p_collator
    ,const dlb_wave_read_test  *p_test_info
    ,dlb_wave_file             *p_wave
    )
{
    dlb_wave_format ref_fmt;
    int is_float;
    const char *p_err =
        memwave_load_format
            (p_test_info->data
            ,p_test_info->data_size
            ,&ref_fmt
            ,&is_float
            );

    (void)p_err; /* Silence VS warning in release builds. */
    assert(p_err == NULL); /* The format was loaded from a test vector. The
                            * test vector must be valid. */

    assert(!is_float == !(p_test_info->format_flags & DLB_WAVE_FLOAT));

    return
        dlb_wave_test_query_apis
            (p_def
            ,p_collator
            ,&ref_fmt
            ,p_test_info->format_flags
            ,p_wave
            );
}

static
int
dlb_wave_read_comparison_test
    (const munit_test_case    *p_def
    ,const munit_collator     *p_collator
    ,const dlb_wave_read_test *p_test_info
    ,dlb_wave_file            *p_wave
    )
{
    size_t data_chunk_size;
    size_t total_read = 0;
    dlb_wave_format expected_properties;
    const unsigned char *p_data;
    size_t frame_size;
    int b_is_float;
    const char *p_err;

    p_err = memwave_find_data(p_test_info->data, p_test_info->data_size, &p_data, &data_chunk_size);
    assert(p_err == NULL);

    p_err = memwave_load_format(p_test_info->data, p_test_info->data_size, &expected_properties, &b_is_float);
    assert(p_err == NULL);

    (void)p_err;

    frame_size = expected_properties.channel_count * ((expected_properties.bits_per_sample + 7) / 8);

    {   /* Test that the number of frames returned by the dlb_wave API
         * matches what we expect given our input data buffer. */
        const size_t nb_expected_frames = data_chunk_size / frame_size;
        const size_t nb_frames = (size_t)dlb_wave_get_num_frames(p_wave);

        eval_assert
            (p_def
            ,p_collator
            ,nb_expected_frames == nb_frames
            ,"dlb_wave_get_num_frames() gave an unexpected result (expected: %lu, got: %lu)"
            ,nb_expected_frames
            ,nb_frames
            );
    }

    /* We repeatedly read an obscure amount of samples from the wave
     * file to test the read logic is working. */
    while (data_chunk_size)
    {
        size_t        amt_read;
        unsigned char a_read_buffer[5];

        {   /* Run and test the actual read API. */
            int err =
                dlb_wave_read_data
                    (p_wave
                    ,a_read_buffer
                    ,sizeof(a_read_buffer)
                    ,&amt_read
                    );

            if (err == 0)
            {
                /* There was no error reading the data. Check to make sure
                 * that we received exactly as much data as we asked
                 * for. */
                if (amt_read != sizeof(a_read_buffer))
                {
                    p_collator->fail
                        (p_def
                        ,p_collator->context
                        ,"dlb_wave_read_data() returned no-error but did not return the amount of data that was asked for"
                        );
                    return 1;
                }
            }
            else if (err != DLB_RIFF_W_EOF)
            {
                /* An unexpected read error occured; abort the test. */
                p_collator->fail
                    (p_def
                    ,p_collator->context
                    ,"dlb_wave_read_data() failed unexpectedly (%d)"
                    ,err
                    );
                return 1;
            }

            if (amt_read > data_chunk_size)
            {
                p_collator->fail
                    (p_def
                    ,p_collator->context
                    ,"dlb_wave_read_data() said that it returned more data than what was given in the data chunk"
                    );
                return 1;
            }

            p_collator->pass(p_def, p_collator->context);
        }

        {   /* Test that the returned data is correct. */
            size_t        j;
            int           had_failures = 0;

            for (j = 0; j < amt_read; j++)
            {
                if (p_data[j] != a_read_buffer[j])
                {
                    p_collator->fail
                        (p_def
                        ,p_collator->context
                        ,"dlb_wave_read_data() produced unexpected output (frame %lu)"
                        ,total_read + j
                        );
                    had_failures = 1;
                }
            }

            if (had_failures)
            {
                return 1;
            }

            p_collator->pass(p_def, p_collator->context);
        }

        total_read      += amt_read;
        p_data          += amt_read;
        data_chunk_size -= amt_read;

        {   /* Test the dlb_wave_get_current_frame() API */
            size_t dut_frame = (size_t)dlb_wave_get_current_frame(p_wave);
            size_t ref_frame = total_read / frame_size;
            if (dut_frame != ref_frame)
            {
                p_collator->fail
                    (p_def
                    ,p_collator->context
                    ,"dlb_wave_get_current_frame() returned an unexpected result (expected: %lu, got: %lu)"
                    ,ref_frame
                    ,dut_frame
                    );
                return 1;
            }

            p_collator->pass(p_def, p_collator->context);
        }
    } /* while (data_chunk_size) */

    return 0;
}

static
int
dlb_wave_buffer_read_comparison_test
    (const munit_test_case    *p_def
    ,const munit_collator     *p_collator
    ,const dlb_wave_read_test *p_test_info
    ,dlb_wave_file            *p_wave
    ,int                       buffer_format
    ,const char               *p_buffer_name
    )
{
    size_t               data_chunk_size;
    size_t               total_frames_read = 0;
    dlb_wave_format      expected_properties;
    size_t               container_width;
    size_t               frame_size;
    const unsigned char *p_data_root;
    size_t               nb_expected_frames;
    size_t               nb_frames = (size_t)dlb_wave_get_num_frames(p_wave);

    buffer_type          wav_buf_type;
    buffer_type          dlb_buf_type;

    unsigned i;
    long    a_long_data[MAX_CHANNELS_IN_TEST_DATA];
    int     a_int_data[MAX_CHANNELS_IN_TEST_DATA];
    short   a_short_data[MAX_CHANNELS_IN_TEST_DATA];
    float   a_float_data[MAX_CHANNELS_IN_TEST_DATA];
    double  a_double_data[MAX_CHANNELS_IN_TEST_DATA];
    void   *ap_pointers[MAX_CHANNELS_IN_TEST_DATA];
    dlb_buffer buf;


    int b_is_float;
    const char *p_err;

    p_err = memwave_find_data(p_test_info->data, p_test_info->data_size, &p_data_root, &data_chunk_size);
    assert(p_err == NULL);

    p_err = memwave_load_format(p_test_info->data, p_test_info->data_size, &expected_properties, &b_is_float);
    assert(p_err == NULL);

    container_width = (expected_properties.bits_per_sample + 7) / 8;
    frame_size = expected_properties.channel_count * container_width;
    nb_expected_frames = data_chunk_size / frame_size;

    /* Early abort if the number of frames in the wave is not what we
     * expect. */
    if (nb_frames != nb_expected_frames)
    {
        p_collator->fail
            (p_def
            ,p_collator->context
            ,"dlb_wave_get_num_frames() returned an unexpected number of frames (expected %lu, got %lu)"
            ,nb_expected_frames
            ,nb_frames
            );
        return 1;
    }

    /* dlb_buffer setup boiler plate (we are only ever going to read one
     * sample at a time in this test). */
    for (i = 0; i < expected_properties.channel_count; i++)
    {
        switch (buffer_format)
        {
        case DLB_BUFFER_SHORT_16:   ap_pointers[i] = a_short_data + i; break;
        case DLB_BUFFER_INT_LEFT:   ap_pointers[i] = a_int_data + i; break;
        case DLB_BUFFER_LONG_32:    ap_pointers[i] = a_long_data + i; break;
        case DLB_BUFFER_FLOAT:      ap_pointers[i] = a_float_data + i; break;
        case DLB_BUFFER_DOUBLE:     ap_pointers[i] = a_double_data + i; break;
        }
    }
    buf.data_type   = buffer_format;
    buf.nchannel    = expected_properties.channel_count;
    buf.nstride     = expected_properties.channel_count;
    buf.ppdata      = ap_pointers;

    get_dlb_buffer_type(&dlb_buf_type, buf.data_type);

    p_err = get_dlb_wave_buffer_type(&wav_buf_type, dlb_wave_get_format(p_wave), dlb_wave_get_format_flags(p_wave));
    if (p_err != NULL)
    {
        p_collator->fail
            (p_def
            ,p_collator->context
            ,"Cannot interpret the data format of the wave buffer '%s'"
            ,p_err
            );
        return 1;
    }

    /* We repeatedly read an obscure amount of samples from the wave
     * file to test the read logic is working. */
    while (nb_frames)
    {
        size_t        frames_read;
        size_t        j;

        {   /* Run and test the actual read API. */
            int err =
                dlb_wave_float_read
                    (p_wave
                    ,&buf
                    ,1
                    ,&frames_read
                    );

            if (err == 0)
            {
                /* There was no error reading the data. Check to make sure
                 * that we received exactly as much data as we asked
                 * for. */
                if (frames_read != 1)
                {
                    p_collator->fail
                        (p_def
                        ,p_collator->context
                        ,"dlb_wave_float_read() returned no-error but did not return the amount of data that was asked for"
                        );
                    return 1;
                }
            }
            else if (err != DLB_RIFF_W_EOF)
            {
                /* An unexpected read error occured; abort the test. */
                p_collator->fail
                    (p_def
                    ,p_collator->context
                    ,"dlb_wave_float_read() failed unexpectedly (%d)"
                    ,err
                    );
                return 1;
            }

            if (frames_read > 1)
            {
                p_collator->fail
                    (p_def
                    ,p_collator->context
                    ,"dlb_wave_float_read() said that it returned more data than what was asked for"
                    );
                return 1;
            }

            p_collator->pass(p_def, p_collator->context);
        }

        for (j = 0; j < frames_read; j++)
        {
            unsigned k;

            for (k = 0; k < expected_properties.channel_count; k++)
            {
                int tolerance;

                compare_result e =
                    compare_sample
                        (&dlb_buf_type
                        ,ap_pointers[k]
                        ,&wav_buf_type
                        ,(void *)(p_data_root + ((total_frames_read + j) * expected_properties.channel_count + k) * wav_buf_type.nb_octets)
                        );
                
                tolerance = e.b_was_float ? 1 : 0;

                eval_assert
                    (p_def
                    ,p_collator
                    ,e.error <= tolerance && e.error >= -tolerance
                    ,"dlb_wave_float_read() returned unexpected values (mode: %s, channel: %u, sample: %lu, error: %ld, prec: %u, tol: %d)"
                    ,p_buffer_name
                    ,k
                    ,(unsigned long)total_frames_read + j
                    ,e.error
                    ,e.mant_bits
                    ,tolerance
                    );
            }

            p_collator->pass(p_def, p_collator->context);
        }

        total_frames_read += frames_read;
        nb_frames -= frames_read;

        {   /* Test the dlb_wave_get_current_frame() API */
            size_t dut_frame = (size_t)dlb_wave_get_current_frame(p_wave);
            size_t ref_frame = total_frames_read;
            if (dut_frame != ref_frame)
            {
                p_collator->fail
                    (p_def
                    ,p_collator->context
                    ,"dlb_wave_get_current_frame() returned an unexpected result (expected: %lu, got: %lu)"
                    ,ref_frame
                    ,dut_frame
                    );
                return 1;
            }

            p_collator->pass(p_def, p_collator->context);
        }
    } /* while (data_chunk_size) */

    return 0;
}

/* This function aims to test all of the dlb_wave read API. It takes a
 * dlb_wave and a descriptor of the format. It ensures that the wave matches
 * the description and then tests the actual reading API. It does this by
 * finding the data chunk in the actual test data and reading that along-side
 * calls to the dlb_wave_read_data() API; it does this step twice to ensure
 * that the wave seeking operation works (i.e. we rewind). Then there are
 * calls to the float-read API (which also tests the int-read API) followed
 * by a dlb_buffer_convert operation to validate the data for all the
 * different read types. */
static
void
dlb_wave_read_test_internal
    (const munit_test_case    *p_def
    ,const munit_collator     *p_collator
    ,const dlb_wave_read_test *p_test_info
    ,dlb_wave_file            *p_wave
    )
{
    unsigned i;

    {   /* Test that the wave reader understands how to read a wave format
         * structure. */
        int err =
            dlb_wave_test_query_apis_from_test
                (p_def
                ,p_collator
                ,p_test_info
                ,p_wave
                );

        /* If the wave reader could not detect the correct data format, it is
         * probably unwise to even try to test the data reading API. */
        if (err)
        {
            return;
        }
    }

    {   /* Perform a raw data-reading comparison. */
        int err =
            dlb_wave_read_comparison_test
                (p_def
                ,p_collator
                ,p_test_info
                ,p_wave
                );

        if (err)
        {
            return;
        }
    }

    {   /* Rewind for next test (this is how we test the seek logic) */
        int err = dlb_wave_seek_to_frame(p_wave, 0);

        if (err)
        {
            p_collator->fail
                (p_def
                ,p_collator->context
                ,"dlb_wave_seek_to_frame() failed unexpectedly (%d)"
                ,err
                );
            return;
        }
    }

    {   /* Perform a raw data-reading comparison again to make sure that the
         * rewind operation actually worked. */
        int err =
            dlb_wave_read_comparison_test
                (p_def
                ,p_collator
                ,p_test_info
                ,p_wave
                );

        if (err)
        {
            return;
        }
    }

    for (i = 0; i < sizeof(buffer_format_codes) / sizeof(buffer_format_codes[0]); i++)
    {
        {   /* Rewind again for dlb_buffer reading tests */
            int err = dlb_wave_seek_to_frame(p_wave, 0);

            if (err)
            {
                p_collator->fail
                    (p_def
                    ,p_collator->context
                    ,"dlb_wave_seek_to_frame() failed unexpectedly (%d)"
                    ,err
                    );
                return;
            }
        }

        {   /* Perform a raw data-reading comparison again to make sure that the
             * rewind operation actually worked. */
            int err =
                dlb_wave_buffer_read_comparison_test
                    (p_def
                    ,p_collator
                    ,p_test_info
                    ,p_wave
                    ,buffer_format_codes[i]
                    ,buffer_format_names[i]
                    );

            if (err)
            {
                return;
            }
        }
    }
}

static
void
dlb_wave_read_test_proc
    (const munit_test_case *p_def
    ,const munit_collator  *p_collator
    ,munit_test_memory     *p_mem
    )
{
    const dlb_wave_read_test *p_test_info  = munit_get_pointer_param(p_mem, "testdata");
    int err;
    dlb_octfile f;
    dlb_octfile *p_f;
    dlb_wave_file wave;

    p_f =
        dlb_octfile_open_memory_fixed
            (&f
            ,p_test_info->data_size
            ,(char *)p_test_info->data
            ,"rb"
            );

    /* If dlb_octfile fails, the best thing to do here is abort. The failure
     * was nothing to do with dlb_wave. */
    if (p_f == NULL)
    {
        abort();
    }

    err =
        dlb_wave_octfile_read
            (&wave
            ,p_f
            ,NULL /* Todo: this probably should get a test at some point. */
            );

    if (err == 0)
    {
        dlb_wave_read_test_internal
            (p_def
            ,p_collator
            ,p_test_info
            ,&wave
            );

        dlb_wave_close(&wave);
    }
    else
    {
        p_collator->fail
            (p_def
            ,p_collator->context
            ,"dlb_wave_octfile_read() failed unexpectedly %d\n"
            ,err
            );
    }

    dlb_octfile_close(p_f);
}

static
int
dlb_wave_write_data_step
    (const munit_test_case    *p_def
    ,const munit_collator     *p_collator
    ,const dlb_wave_read_test *p_test_info
    ,dlb_wave_file            *p_wave
    ,int                       buffer_format
    )
{
    size_t               container_width;
    size_t               frame_size;
    size_t               data_chunk_size;
    const unsigned char *p_data;
    size_t               nb_expected_frames;

    unsigned i;
    long    a_long_data[MAX_CHANNELS_IN_TEST_DATA];
    int     a_int_data[MAX_CHANNELS_IN_TEST_DATA];
    short   a_short_data[MAX_CHANNELS_IN_TEST_DATA];
    float   a_float_data[MAX_CHANNELS_IN_TEST_DATA];
    double  a_double_data[MAX_CHANNELS_IN_TEST_DATA];
    void   *ap_pointers[MAX_CHANNELS_IN_TEST_DATA];
    dlb_buffer buf;
    int err;
    buffer_type          wav_buf_type;
    buffer_type          dlb_buf_type;

    dlb_wave_format expected_properties;
    int b_is_float;
    const char *p_err;

    p_err = memwave_find_data(p_test_info->data, p_test_info->data_size, &p_data, &data_chunk_size);
    assert(p_err == NULL);

    p_err = memwave_load_format(p_test_info->data, p_test_info->data_size, &expected_properties, &b_is_float);
    assert(p_err == NULL);

    p_err = get_dlb_wave_buffer_type(&wav_buf_type, &expected_properties, b_is_float ? DLB_WAVE_FLOAT : 0);
    assert(p_err == NULL);

    (void)p_err;

    container_width = (expected_properties.bits_per_sample + 7) / 8;
    frame_size = expected_properties.channel_count * container_width;
    nb_expected_frames = data_chunk_size / frame_size;

    /* dlb_buffer setup boiler plate (we are only ever going to read one
     * sample at a time in this test). */
    for (i = 0; i < expected_properties.channel_count; i++)
    {
        switch (buffer_format)
        {
        case DLB_BUFFER_SHORT_16:   ap_pointers[i] = a_short_data + i; break;
        case DLB_BUFFER_INT_LEFT:   ap_pointers[i] = a_int_data + i; break;
        case DLB_BUFFER_LONG_32:    ap_pointers[i] = a_long_data + i; break;
        case DLB_BUFFER_FLOAT:      ap_pointers[i] = a_float_data + i; break;
        case DLB_BUFFER_DOUBLE:     ap_pointers[i] = a_double_data + i; break;
        }
    }

    buf.data_type   = buffer_format;
    buf.nchannel    = expected_properties.channel_count;
    buf.nstride     = expected_properties.channel_count;
    buf.ppdata      = ap_pointers;

    get_dlb_buffer_type(&dlb_buf_type, buf.data_type);

    err = dlb_wave_begin_data(p_wave);
    if (err)
    {
        p_collator->fail
            (p_def
            ,p_collator->context
            ,"dlb_wave_begin_data() failed unexpectedly %d\n"
            ,err
            );
        return -1;
    }

    while (nb_expected_frames--)
    {
        unsigned k;

        for (k = 0; k < expected_properties.channel_count; k++)
        {
            convert_sample
                (&dlb_buf_type
                ,ap_pointers[k]
                ,&wav_buf_type
                ,p_data
                );

            p_data += wav_buf_type.nb_octets;
        }

        err =
            dlb_wave_float_write
                (p_wave
                ,&buf
                ,1
                );

        if (err)
        {
            p_collator->fail
                (p_def
                ,p_collator->context
                ,"dlb_wave_float_write() failed unexpectedly %d\n"
                ,err
                );
            break;
        }
    }

    {
        int err2 = dlb_wave_end_data(p_wave);

        if (err2)
        {
            p_collator->fail
                (p_def
                ,p_collator->context
                ,"dlb_wave_end_data() failed unexpectedly %d\n"
                ,err2
                );
            if (!err)
            {
                err = err2;
            }
        }
    }

    return err;
}

/* Compare the sample data stored in two wave files stored in memory. */
static
int
dlb_wave_write_data_compare
    (const munit_test_case *p_def
    ,const munit_collator  *p_collator
    ,const void            *p_ref
    ,size_t                 ref_size
    ,const void            *p_dut
    ,size_t                 dut_size
    ,const char            *p_conversion_name
    ,unsigned               conversion_mant_bits
    ,int                    b_conversion_float
    )
{
    buffer_type ref_buf_type;
    buffer_type dut_buf_type;
    size_t refsize;
    size_t dutsize;
    const unsigned char *ref;
    const unsigned char *dut;
    unsigned rp, dp;
    unsigned sample;
    const char *p_err;
    int failed = 0;

    /* The reference data is provided by the test harness and must be valid.
     * It is undefined to test against an invalid source wave. */
    p_err = get_dlb_wave_buffer_type_from_memory(&ref_buf_type, p_ref, ref_size);
    assert(p_err == NULL);
    p_err = memwave_find_data(p_ref, ref_size, &ref, &refsize);
    assert(p_err == NULL);

    /* If the DUT data is invalid, the implementation must be completely
     * broken as we would have just finished writing the wave file. Don't even
     * try to test the data. */
    p_err = get_dlb_wave_buffer_type_from_memory(&dut_buf_type, p_dut, dut_size);
    if (p_err != NULL)
    {
        p_collator->fail(p_def, p_collator->context, "%s", p_err);
        return -1;
    }
    p_err = memwave_find_data(p_dut, dut_size, &dut, &dutsize);
    if (p_err != NULL)
    {
        p_collator->fail(p_def, p_collator->context, "%s", p_err);
        return -1;
    }

    /* Perform a sample by sample test of the data in the wave file. */
    for (rp = 0, dp = 0, sample = 0
        ;rp + ref_buf_type.nb_octets <= refsize && dp + dut_buf_type.nb_octets <= dutsize
        ;rp += ref_buf_type.nb_octets, dp += dut_buf_type.nb_octets, sample++
        )
    {
        int tolerance;

        compare_result e =
            compare_sample_limited
                (&dut_buf_type
                ,(const void *)(dut + dp)
                ,&ref_buf_type
                ,(const void *)(ref + rp)
                ,conversion_mant_bits
                );

        tolerance = (b_conversion_float || e.b_was_float) ? 1 : 0;

        if (e.error > tolerance || e.error < -tolerance)
        {
            p_collator->fail
                (p_def
                ,p_collator->context
                ,"dlb_wave_float_write() introduced unexpected values (mode: %s, absolute interleaved sample: %u, error: %ld, prec: %u, tol: %d)"
                ,p_conversion_name
                ,sample
                ,e.error
                ,e.mant_bits
                ,tolerance
                );
            failed = 1;
        }
        else
        {
            p_collator->pass(p_def, p_collator->context);
        }

    }

    if (failed)
    {
        return 1;
    }

    if (rp + ref_buf_type.nb_octets <= refsize && dp + dut_buf_type.nb_octets > dutsize)
    {
        p_collator->fail(p_def, p_collator->context, "Not all test data was written into the wave.");
        return -1;
    }

    return 0;
}


static
void
dlb_wave_write_test_proc
    (const munit_test_case *p_def
    ,const munit_collator  *p_collator
    ,munit_test_memory     *p_mem
    )
{
    const dlb_wave_read_test *p_test_info = munit_get_pointer_param(p_mem, "testdata");
    dlb_wave_format input_data_format;
    unsigned i, j;
    int b_is_float;
    const char *p_err;

    static const struct
    {
        unsigned short format_type;
        unsigned       on_flags;
        unsigned       off_flags;
        unsigned short valid_bits_per_sample;
        unsigned short bits_per_sample;
    } wave_format_coefs[] =
    {   {   0xFFFEu
        ,   DLB_WAVE_EXTENSIBLE
        ,   DLB_WAVE_FLOAT | DLB_WAVE_JUNK
        ,   32
        ,   32
        } /* 32 bit, wave format extensible, pcm */
    ,   {   0xFFFEu
        ,   DLB_WAVE_EXTENSIBLE | DLB_WAVE_FLOAT
        ,   DLB_WAVE_JUNK
        ,   32
        ,   32
        } /* 32 bit, wave format extensible, float */
    ,   {   0x0003u
        ,   DLB_WAVE_FLOAT
        ,   DLB_WAVE_EXTENSIBLE | DLB_WAVE_JUNK
        ,   32
        ,   32
        } /* 32 bit, float */
    ,   {   0x0001u
        ,   0
        ,   DLB_WAVE_EXTENSIBLE | DLB_WAVE_FLOAT | DLB_WAVE_JUNK
        ,   24
        ,   24
        } /* 24 bit, pcm */
    ,   {   0x0001u
        ,   0
        ,   DLB_WAVE_EXTENSIBLE | DLB_WAVE_FLOAT | DLB_WAVE_JUNK
        ,   16
        ,   16
        } /* 16 bit, pcm */
    ,   {   0x0001u
        ,   0
        ,   DLB_WAVE_EXTENSIBLE | DLB_WAVE_FLOAT | DLB_WAVE_JUNK
        ,   8
        ,   8
        } /* 8 bit, pcm */
    };

    p_err = memwave_load_format(p_test_info->data, p_test_info->data_size, &input_data_format, &b_is_float);
    assert(p_err == NULL);

    (void)p_err;

    for (j = 0; j < sizeof(wave_format_coefs) / sizeof(wave_format_coefs[0]); j++)
    {
        for (i = 0; i < sizeof(buffer_format_codes) / sizeof(buffer_format_codes[0]); i++)
        {
            int err;
            dlb_octfile f;
            dlb_octfile *p_f;
            dlb_wave_file wave;
            dlb_wave_format ref_fmt;
            int ref_flags;

            ref_fmt.format_type             = wave_format_coefs[j].format_type;
            ref_fmt.channel_count           = input_data_format.channel_count;
            ref_fmt.sample_rate             = input_data_format.sample_rate;
            ref_fmt.valid_bits_per_sample   = wave_format_coefs[j].valid_bits_per_sample;
            ref_fmt.channel_mask            = input_data_format.channel_mask;
            ref_fmt.bits_per_sample         = wave_format_coefs[j].bits_per_sample;
            ref_fmt.octets_per_sample       = (ref_fmt.bits_per_sample + 7) >> 3;
            ref_fmt.block_alignment         = ref_fmt.channel_count * ref_fmt.octets_per_sample;
            ref_fmt.bytes_per_second        = ref_fmt.block_alignment * ref_fmt.sample_rate;

            {
                unsigned tmp = (unsigned)p_test_info->format_flags;
                tmp |= wave_format_coefs[j].on_flags;
                tmp &= ~wave_format_coefs[j].off_flags;
                ref_flags = (int)tmp;
            }

            p_f =
                dlb_octfile_open_memory
                    (&f
                    ,"wb"
                    );

            /* If dlb_octfile fails, the best thing to do here is abort. The failure
             * was nothing to do with dlb_wave. */
            if (p_f == NULL)
            {
                abort();
            }

            /* Begin writing a new wave file into this memory file. */
            err =
                dlb_wave_octfile_write
                    (&wave
                    ,p_f
                    ,ref_flags
                    ,ref_fmt.sample_rate
                    ,ref_fmt.channel_count
                    ,ref_fmt.channel_mask
                    ,ref_fmt.bits_per_sample
                    );

            if (err == 0)
            {
                /* Write data into the wave file in the current selected buffer
                 * format. */
                err =
                    dlb_wave_write_data_step
                        (p_def
                        ,p_collator
                        ,p_test_info
                        ,&wave
                        ,buffer_format_codes[i]
                        );

                if (err == 0)
                {
                    err =
                        dlb_wave_test_query_apis
                            (p_def
                            ,p_collator
                            ,&ref_fmt
                            ,ref_flags
                            ,&wave
                            );
                }

                dlb_wave_close(&wave);
            }
            else
            {
                p_collator->fail
                    (p_def
                    ,p_collator->context
                    ,"dlb_wave_octfile_write() failed unexpectedly %d\n"
                    ,err
                    );
            }

            if (err == 0)
            {
                unsigned buffer_mant_bits = get_dlb_buffer_mant_bits(buffer_format_codes[i]);
                err =
                    dlb_wave_write_data_compare
                        (p_def
                        ,p_collator
                        ,p_test_info->data
                        ,p_test_info->data_size
                        ,p_f->mem_base
                        ,p_f->mem_size
                        ,buffer_format_names[i]
                        ,buffer_mant_bits
                        ,buffer_format_codes[i] == DLB_BUFFER_DOUBLE || buffer_format_codes[i] == DLB_BUFFER_FLOAT
                        );
            }

            /* Close the file */
            dlb_octfile_close(p_f);
        }
    }
}

/* Test data sets. */

static const unsigned char basic_8bit_1ch_32khz[] =
{'R', 'I', 'F', 'F', 40, 0, 0, 0
,'W', 'A', 'V', 'E'
,'f', 'm', 't', ' ', 16, 0, 0, 0
    ,0x01, 0x00             /* wFormatTag */
    ,0x01, 0x00             /* wChannels */
    ,0x00, 0x7D, 0x00, 0x00 /* dwSamplesPerSec */
    ,0x00, 0x7D, 0x00, 0x00 /* dwAvgBytesPerSec */
    ,0x01, 0x00             /* wBlockAlign */
    ,0x08, 0x00             /* wBitsPerSample */
,'d', 'a', 't', 'a', 4, 0, 0, 0
    ,0xFF
    ,0x80
    ,0x7F
    ,0x00
};

static const unsigned char basic_16bit_3ch_96khz[] =
{'R', 'I', 'F', 'F', 48, 0, 0, 0
,'W', 'A', 'V', 'E'
,'f', 'm', 't', ' ', 16, 0, 0, 0
    ,0x01, 0x00             /* wFormatTag */
    ,0x03, 0x00             /* wChannels */
    ,0x00, 0x77, 0x01, 0x00 /* dwSamplesPerSec */
    ,0x00, 0xCA, 0x08, 0x00 /* dwAvgBytesPerSec */
    ,0x06, 0x00             /* wBlockAlign */
    ,0x10, 0x00             /* wBitsPerSample */
,'d', 'a', 't', 'a', 12, 0, 0, 0
    ,0x00, 0x00
    ,0x80, 0x00
    ,0xFF, 0x7F

    ,0xFF, 0xFF
    ,0xFE, 0xFF
    ,0xFF, 0xFE
};

static const unsigned char basic_24bit_2ch_32khz[] =
{'R', 'I', 'F', 'F', 48, 0, 0, 0
,'W', 'A', 'V', 'E'
,'f', 'm', 't', ' ', 16, 0, 0, 0
    ,0x01, 0x00             /* wFormatTag */
    ,0x02, 0x00             /* wChannels */
    ,0x00, 0x7D, 0x00, 0x00 /* dwSamplesPerSec */
    ,0x00, 0xEE, 0x02, 0x00 /* dwAvgBytesPerSec */
    ,0x06, 0x00             /* wBlockAlign */
    ,0x18, 0x00             /* wBitsPerSample */
,'d', 'a', 't', 'a', 12, 0, 0, 0
    ,0xFF, 0xFF, 0xFF
    ,0x00, 0x00, 0x80

    ,0xFF, 0xFF, 0x7F
    ,0xA5, 0xA5, 0xA5
};

static const unsigned char basic_32bit_1ch_256hz[] =
{'R', 'I', 'F', 'F', 52, 0, 0, 0
,'W', 'A', 'V', 'E'
,'f', 'm', 't', ' ', 16, 0, 0, 0
    ,0x01, 0x00             /* wFormatTag */
    ,0x01, 0x00             /* wChannels */
    ,0x00, 0x01, 0x00, 0x00 /* dwSamplesPerSec */
    ,0x00, 0x04, 0x00, 0x00 /* dwAvgBytesPerSec */
    ,0x04, 0x00             /* wBlockAlign */
    ,0x20, 0x00             /* wBitsPerSample */
,'d', 'a', 't', 'a', 16, 0, 0, 0
    ,0xFF, 0xFF, 0xFF, 0xFF
    ,0x00, 0x00, 0x80, 0x80
    ,0xFF, 0xFF, 0xFF, 0x7F
    ,0xA5, 0xA5, 0xA5, 0xA5
};

static const unsigned char comprehensive_32bit_1ch_256hz[] =
{'R', 'I', 'F', 'F', 36 + 8 + 28*6, 0, 0, 0
,'W', 'A', 'V', 'E'
,'f', 'm', 't', ' ', 16, 0, 0, 0
    ,0x01, 0x00             /* wFormatTag */
    ,0x01, 0x00             /* wChannels */
    ,0x00, 0x01, 0x00, 0x00 /* dwSamplesPerSec */
    ,0x00, 0x04, 0x00, 0x00 /* dwAvgBytesPerSec */
    ,0x04, 0x00             /* wBlockAlign */
    ,0x20, 0x00             /* wBitsPerSample */
,'d', 'a', 't', 'a', 8 + 28*6, 0, 0, 0
    /* Zero test  (4) */
    ,0x00, 0x00, 0x00, 0x00
    /* Test that seems to trigger rounding issues. */
    ,0x01, 0x00, 0x00, 0xFF
    /* Rounding to 8 bit positive tests (28) */
    ,0x00, 0x00, 0x83, 0x00
    ,0x00, 0x00, 0x82, 0x00
    ,0x00, 0x00, 0x81, 0x00
    ,0x00, 0x00, 0x80, 0x00
    ,0x00, 0x00, 0x7F, 0x00
    ,0x00, 0x00, 0x7E, 0x00
    ,0x00, 0x00, 0x7D, 0x00
    /* Rounding to 16 bit positive tests (28) */
    ,0x00, 0x83, 0x00, 0x00
    ,0x00, 0x82, 0x00, 0x00
    ,0x00, 0x81, 0x00, 0x00
    ,0x00, 0x80, 0x00, 0x00
    ,0x00, 0x7F, 0x00, 0x00
    ,0x00, 0x7E, 0x00, 0x00
    ,0x00, 0x7D, 0x00, 0x00
    /* Rounding to 24 bit positive tests (28) */
    ,0x83, 0x00, 0x00, 0x00
    ,0x82, 0x00, 0x00, 0x00
    ,0x81, 0x00, 0x00, 0x00
    ,0x80, 0x00, 0x00, 0x00
    ,0x7F, 0x00, 0x00, 0x00
    ,0x7E, 0x00, 0x00, 0x00
    ,0x7D, 0x00, 0x00, 0x00
    /* Rounding to 8 bit negative tests (28) */
    ,0x00, 0x00, 0x83, 0xFF
    ,0x00, 0x00, 0x82, 0xFF
    ,0x00, 0x00, 0x81, 0xFF
    ,0x00, 0x00, 0x80, 0xFF
    ,0x00, 0x00, 0x7F, 0xFF
    ,0x00, 0x00, 0x7E, 0xFF
    ,0x00, 0x00, 0x7D, 0xFF
    /* Rounding to 16 bit negative tests (28) */
    ,0x00, 0x83, 0xFF, 0xFF
    ,0x00, 0x82, 0xFF, 0xFF
    ,0x00, 0x81, 0xFF, 0xFF
    ,0x00, 0x80, 0xFF, 0xFF
    ,0x00, 0x7F, 0xFF, 0xFF
    ,0x00, 0x7E, 0xFF, 0xFF
    ,0x00, 0x7D, 0xFF, 0xFF
    /* Rounding to 24 bit negative tests (28) */
    ,0x83, 0xFF, 0xFF, 0xFF
    ,0x82, 0xFF, 0xFF, 0xFF
    ,0x81, 0xFF, 0xFF, 0xFF
    ,0x80, 0xFF, 0xFF, 0xFF
    ,0x7F, 0xFF, 0xFF, 0xFF
    ,0x7E, 0xFF, 0xFF, 0xFF
    ,0x7D, 0xFF, 0xFF, 0xFF
};

static const unsigned char basic_float_1ch_24khz[] =
{'R', 'I', 'F', 'F', 36 + 25 * 4, 0, 0, 0
,'W', 'A', 'V', 'E'
,'f', 'm', 't', ' ', 16, 0, 0, 0
    ,0x03, 0x00             /* wFormatTag */
    ,0x01, 0x00             /* wChannels */
    ,0xC0, 0x5D, 0x00, 0x00 /* dwSamplesPerSec */
    ,0x00, 0x77, 0x01, 0x00 /* dwAvgBytesPerSec */
    ,0x04, 0x00             /* wBlockAlign */
    ,0x20, 0x00             /* wBitsPerSample */
,'d', 'a', 't', 'a', 25 * 4, 0, 0, 0
    ,0x00, 0x00, 0x80, 0x3F /* 0) 1.0 */
    ,0x00, 0x00, 0x80, 0xBF /* 1) -1.0 */
    ,0xCD, 0xCC, 0xCC, 0xBD /* 2) -0.1 */
    ,0x6F, 0x12, 0x83, 0x3A /* 3) 0.001 */
    ,0x92, 0x24, 0x49, 0xBC /* 4) -0.012276785 */
    ,0x00, 0x00, 0x00, 0x00 /* 5) 0.0 - test positive zero handling */
    ,0x00, 0x00, 0x00, 0x80 /* 6) -0.0 - test negative zero handling */
    ,0x00, 0x00, 0x80, 0x7F /* 7) Inf - test positive infinity handling */
    ,0x00, 0x00, 0x80, 0xFF /* 8) -Inf - test negative infinity handling */
    ,0x00, 0x00, 0x00, 0x40 /* 9) 2.0 - test positive overflow handling */
    ,0x00, 0x00, 0x00, 0xC0 /* 10) -2.0 - test negative overflow handling */
    ,0xFF, 0xFF, 0xFF, 0x7F /* 11) quiet nan 1 - test positive nan handling */
    ,0xFF, 0xFF, 0xFF, 0xFF /* 12) quiet nan 1 - test negative nan handling */
    ,0x00, 0x00, 0xC0, 0x7F /* 13) quiet nan 2 - test positive nan handling */
    ,0x00, 0x00, 0xC0, 0xFF /* 14) quiet nan 2 - test negative nan handling */
    ,0xFF, 0xFF, 0xBF, 0x7F /* 15) signaling nan 1 - test positive nan handling */
    ,0xFF, 0xFF, 0xBF, 0xFF /* 16) signaling nan 1 - test negative nan handling */
    ,0x01, 0x00, 0x80, 0x7F /* 17) signaling nan 2 - test positive nan handling */
    ,0x01, 0x00, 0x80, 0xFF /* 18) signaling nan 2 - test negative nan handling */
    ,0xFF, 0xFF, 0xFF, 0x3F /* 19) max positive mantissa */
    ,0xFF, 0xFF, 0xFF, 0xBF /* 20) max negative mantissa */
    ,0xFF, 0xFF, 0x7F, 0x80 /* 21) negative min denorm */
    ,0x01, 0x00, 0x00, 0x80 /* 22) negative max denorm */
    ,0x01, 0x00, 0x00, 0x00 /* 23) positive min denorm */
    ,0xFF, 0xFF, 0x7F, 0x00 /* 24) positive max denorm */
};

static const unsigned char extensible_16bit_2ch_48khz[] =
{'R', 'I', 'F', 'F', 76, 0, 0, 0
,'W', 'A', 'V', 'E'
,'f', 'm', 't', ' ', 40, 0, 0, 0
    ,0xFE, 0xFF             /* wFormatTag */
    ,0x02, 0x00             /* wChannels */
    ,0x80, 0xBB, 0x00, 0x00 /* dwSamplesPerSec */
    ,0x00, 0xEE, 0x02, 0x00 /* dwAvgBytesPerSec */
    ,0x04, 0x00             /* wBlockAlign */
    ,0x10, 0x00             /* wBitsPerSample */
    ,0x16, 0x00             /* cbSize */
    ,0x10, 0x00             /* wValidBitsPerSample OR wSamplesPerBlock */
    ,0x03, 0x00, 0x00, 0x00 /* dwChannelMask */
    ,0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71
,'d', 'a', 't', 'a', 16, 0, 0, 0
    ,0x00, 0x00
    ,0x00, 0x00

    ,0xAC, 0x01
    ,0xAC, 0x01

    ,0x59, 0x03
    ,0x59, 0x03

    ,0x06, 0x05
    ,0x06, 0x05
};

static const unsigned char extensible_24bit_6ch_48khz_cue[] =
{'R', 'I', 'F', 'F', 138, 0, 0, 0
,'W', 'A', 'V', 'E'
,'f', 'm', 't', ' ', 40, 0, 0, 0
    ,0xFE, 0xFF             /* wFormatTag */
    ,0x06, 0x00             /* wChannels */
    ,0x80, 0xBB, 0x00, 0x00 /* dwSamplesPerSec */
    ,0x00, 0x2F, 0x0D, 0x00 /* dwAvgBytesPerSec */
    ,0x12, 0x00             /* wBlockAlign */
    ,0x18, 0x00             /* wBitsPerSample */
    ,0x16, 0x00             /* cbSize */
    ,0x18, 0x00             /* wValidBitsPerSample OR wSamplesPerBlock */
    ,0x3F, 0x00, 0x00, 0x00 /* dwChannelMask */
    ,0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71
,'c', 'u', 'e', ' ', 52, 0, 0, 0
    ,0x00, 0x00, 0x00, 0x00     /* dwCuePoints */

        ,0x00, 0x00, 0x00, 0x00 /* dwName */
        ,0x00, 0x00, 0x00, 0x00 /* dwPosition */
        ,'d', 'a', 't', 'a'     /* fccChunk */
        ,0x00, 0x00, 0x00, 0x00 /* dwChunkStart */
        ,0x00, 0x00, 0x00, 0x00 /* dwBlockStart */
        ,0x00, 0x00, 0x00, 0x00 /* dwSampleOffset */

        ,0x00, 0x00, 0x00, 0x01 /* dwName */
        ,0x00, 0x00, 0x00, 0x00 /* dwPosition */
        ,'d', 'a', 't', 'a'     /* fccChunk */
        ,0x00, 0x00, 0x00, 0x00 /* dwChunkStart */
        ,0x00, 0x00, 0x00, 0x00 /* dwBlockStart */
        ,0x00, 0x00, 0x00, 0x00 /* dwSampleOffset */
,'d', 'a', 't', 'a', 18, 0, 0, 0
    ,0x00, 0x00, 0x00
    ,0x00, 0x00, 0x80
    ,0xFF, 0xFF, 0xFF
    ,0x01, 0x00, 0x00
    ,0xAB, 0xCD, 0xEF
    ,0x01, 0x23, 0x45
};

#define DLB_WAVE_READ_TEST_DEF(data_set, flags) \
static const dlb_wave_read_test read_ ## data_set ## _test_data = \
{   data_set \
,   sizeof(data_set) \
,   flags \
}; \
static \
const \
munit_test_memory_def \
read_ ## data_set ## _test_memory_def[] = \
    {MUNIT_POINTER_PARAM("testdata", &read_ ## data_set ## _test_data) \
    ,MUNIT_LAST_MEMORY_DEF \
    }; \
static \
const \
munit_test_case \
read_ ## data_set ## _test_case = \
    {/* name */    "read_" #data_set \
    ,/* memory */  read_ ## data_set ## _test_memory_def \
    ,/* test_fn */ &dlb_wave_read_test_proc \
    };

#define DLB_WAVE_WRITE_TEST_DEF(data_set, flags) \
static const dlb_wave_read_test write_ ## data_set ## _test_data = \
{   data_set \
,   sizeof(data_set) \
,   flags \
}; \
static \
const \
munit_test_memory_def \
write_ ## data_set ## _test_memory_def[] = \
    {MUNIT_POINTER_PARAM("testdata", &write_ ## data_set ## _test_data) \
    ,MUNIT_LAST_MEMORY_DEF \
    }; \
static \
const \
munit_test_case \
write_ ## data_set ## _test_case = \
    {/* name */    "write_" #data_set \
    ,/* memory */  write_ ## data_set ## _test_memory_def \
    ,/* test_fn */ &dlb_wave_write_test_proc \
    };

DLB_WAVE_READ_TEST_DEF(basic_8bit_1ch_32khz, 0)
DLB_WAVE_READ_TEST_DEF(basic_16bit_3ch_96khz, 0)
DLB_WAVE_READ_TEST_DEF(basic_24bit_2ch_32khz, 0)
DLB_WAVE_READ_TEST_DEF(basic_32bit_1ch_256hz, 0)
DLB_WAVE_READ_TEST_DEF(basic_float_1ch_24khz, DLB_WAVE_FLOAT)
DLB_WAVE_READ_TEST_DEF(comprehensive_32bit_1ch_256hz, 0)
DLB_WAVE_READ_TEST_DEF(extensible_16bit_2ch_48khz, DLB_WAVE_EXTENSIBLE)
DLB_WAVE_READ_TEST_DEF(extensible_24bit_6ch_48khz_cue, DLB_WAVE_EXTENSIBLE | DLB_WAVE_JUNK)
DLB_WAVE_WRITE_TEST_DEF(basic_8bit_1ch_32khz, 0)
DLB_WAVE_WRITE_TEST_DEF(basic_16bit_3ch_96khz, 0)
DLB_WAVE_WRITE_TEST_DEF(basic_24bit_2ch_32khz, 0)
DLB_WAVE_WRITE_TEST_DEF(basic_32bit_1ch_256hz, 0)
DLB_WAVE_WRITE_TEST_DEF(basic_float_1ch_24khz, DLB_WAVE_FLOAT)
DLB_WAVE_WRITE_TEST_DEF(comprehensive_32bit_1ch_256hz, 0)

static const munit_test_case *dlb_wave_tests[] =
    {&read_basic_8bit_1ch_32khz_test_case
    ,&read_basic_16bit_3ch_96khz_test_case
    ,&read_basic_24bit_2ch_32khz_test_case
    ,&read_basic_32bit_1ch_256hz_test_case
    ,&read_basic_float_1ch_24khz_test_case
    ,&read_comprehensive_32bit_1ch_256hz_test_case
    ,&read_extensible_16bit_2ch_48khz_test_case
    ,&read_extensible_24bit_6ch_48khz_cue_test_case
    ,&write_basic_8bit_1ch_32khz_test_case
    ,&write_basic_16bit_3ch_96khz_test_case
    ,&write_basic_24bit_2ch_32khz_test_case
    ,&write_basic_32bit_1ch_256hz_test_case
    ,&write_basic_float_1ch_24khz_test_case
    ,&write_comprehensive_32bit_1ch_256hz_test_case
    ,NULL
    };

const munit_test_dir dlb_wave_test_dir =
    {/* name */       "dlb_wave"
    ,/* directories */ NULL
    ,/* tests */       dlb_wave_tests
    };

