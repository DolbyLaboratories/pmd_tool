/************************************************************************
 * dlb_wave
 * Copyright (c) 2011-2016, Dolby Laboratories Inc.
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
#ifndef dlb_wave_H
#define dlb_wave_H

#include "dlb_wave/include/dlb_riff.h"
#include "dlb_octfile/include/dlb_octfile.h"

/***************************************************************************//**
DLB_WAVE VERSION INFORMATION.
for more details see code sharing policy.
*******************************************************************************/

#define DLB_WAVE_V_API  1
#define DLB_WAVE_V_FCT  6
#define DLB_WAVE_V_MTNC 2

typedef struct STRUCT_DBL_WAVE_VERSION_INFO
{
    int         v_api;
    int         v_fct;
    int         v_mtnc;
    const char *text;
} DLB_WAVE_VERSION_INFO;

const DLB_WAVE_VERSION_INFO *
dlb_wave_get_version(void);

/***************************************************************************//**
Status codes. Zero for success. Warnings are positive. Errors are negative.
See also the DLB_RIFF status codes.
*******************************************************************************/
#define DLB_WAVE_W_NOPCM       100  /**< File doesn't contain linear PCM data */
#define DLB_WAVE_E_NORIFF     -100  /**< RIFF chunk invalid or missing. */
#define DLB_WAVE_E_NOWAVE     -101  /**< RIFF form is not 'WAVE'. */
#define DLB_WAVE_E_NOFMT      -102  /**< 'fmt ' chunk nonexistant, duplicate or invalid */
#define DLB_WAVE_E_NODS64     -103  /**< 'ds64' chunk nonexistant, duplicate or invalid */
#define DLB_WAVE_E_NODATA     -104  /**< 'data' chunk nonexistant */
#define DLB_WAVE_E_TOOBIG     -105  /**< file is >4GB and RF64 is not enabled */
#define DLB_WAVE_E_BUFFER     -106  /**< dlb_buffer format error */
#define DLB_WAVE_E_NCHANNEL   -107  /**< dlb_buffer channel count error */
#define DLB_WAVE_E_NSEEK      -108  /**< Seek failed or not supported by file */

/******************************************************************************
format flags
******************************************************************************/
/** WAVE_FORMAT_EXTENSIBLE */
#define DLB_WAVE_EXTENSIBLE   0x01   

/** IEEE 754 floating point data */
#define DLB_WAVE_FLOAT        0x02   

/** EBU-TECH 3285 Broadcast Wave Format */
#define DLB_WAVE_BWF          0x04   

/** EBU-TECH 3306 RF64 >4Gbyte support.
 * Warning: Enabling this option when writing a wave file will cause a JUNK
 * chunk to be reserved in the file. This JUNK chunk will be migrated to a ds64
 * chunk if the file size spills over 4GB as documented in EBU-TECH 3306.
 * This JUNK chunk may cause some non-compliant wave readers to barf.
 */
#define DLB_WAVE_RF64         0x08

/** Force creation (for testing) of RF64 file even if size is < 4GB */
#define DLB_WAVE_FORCE_RF64   0x10

/** This is an RF64 file, but we don't have a 64 bit size type.
 * In this case we'll only be able to access the first 4GB of the
 * file.
 */
#define DLB_WAVE_RF64_4GB     0x20

/** Has at least one unrecognised (ignored) chunk in it. */
#define DLB_WAVE_JUNK         0x40

/******************************************************************************
Channel identifier bits as defined in EBU TECH 3306-2009
******************************************************************************/
/* Standard Microsoft speaker locations */
#define DLB_SPK_FRONT_LEFT              0x00000001
#define DLB_SPK_FRONT_RIGHT             0x00000002
#define DLB_SPK_FRONT_CENTER            0x00000004
#define DLB_SPK_LOW_FREQUENCY           0x00000008
#define DLB_SPK_BACK_LEFT               0x00000010
#define DLB_SPK_BACK_RIGHT              0x00000020
#define DLB_SPK_FRONT_LEFT_OF_CENTER    0x00000040
#define DLB_SPK_FRONT_RIGHT_OF_CENTER   0x00000080
#define DLB_SPK_BACK_CENTER             0x00000100
#define DLB_SPK_SIDE_LEFT               0x00000200
#define DLB_SPK_SIDE_RIGHT              0x00000400
#define DLB_SPK_TOP_CENTER              0x00000800
#define DLB_SPK_TOP_FRONT_LEFT          0x00001000
#define DLB_SPK_TOP_FRONT_CENTER        0x00002000
#define DLB_SPK_TOP_FRONT_RIGHT         0x00004000
#define DLB_SPK_TOP_BACK_LEFT           0x00008000
#define DLB_SPK_TOP_BACK_CENTER         0x00010000
#define DLB_SPK_TOP_BACK_RIGHT          0x00020000
#define DLB_SPK_ALL                     0x80000000

/* MBWF extended channel labels */
#define DLB_SPK_STEREO_LEFT             0x20000000
#define DLB_SPK_STEREO_RIGHT            0x40000000
#define DLB_SPK_CONTROLSAMPLE_1         0x08000000
#define DLB_SPK_CONTROLSAMPLE_2         0x10000000
#define DLB_SPK_BITSTREAM_1_LEFT        0x00800000
#define DLB_SPK_BITSTREAM_1_RIGHT       0x01000000
#define DLB_SPK_BITSTREAM_2_LEFT        0x02000000
#define DLB_SPK_BITSTREAM_2_RIGHT       0x04000000

/*******************************************************************************
RIFF abstraction layer.
Only dlb_wave types should be exposed.
******************************************************************************/

/* Indicates if 64-bit (long long) values are used for the following */
#define DLB_WAVE_64             DLB_RIFF_64
/* dlb_wave_size represents chunk sizes */
typedef dlb_riff_size           dlb_wave_size;
#define DLB_WAVE_SIZE_MAX       DLB_RIFF_SIZE_MAX
#define DLB_WAVE_SIZE_MIN       DLB_RIFF_SIZE_MIN
#define DLB_WAVE_SIZE_FMT       DLB_RIFF_SIZE_FMT       /**< printf format spec for dlb_wav_size */
/* dlb_wave_offset is a signed offset */
typedef dlb_riff_offset         dlb_wave_offset;
#define DLB_WAVE_OFFSET_MAX     DLB_RIFF_OFFSET_MAX
#define DLB_WAVE_OFFSET_MIN     DLB_RIFF_OFFSET_MIN
#define DLB_WAVE_OFFSET_FMT     DLB_RIFF_OFFSET_FMT     /**< printf format spec for dlb_wave_offset */
/* dlb_wave_location is an unsigned location */
typedef dlb_riff_location       dlb_wave_location;
#define DLB_WAVE_LOCATION_MAX   DLB_RIFF_LOCATION_MAX
#define DLB_WAVE_LOCATION_MIN   DLB_RIFF_LOCATION_MIN
#define DLB_WAVE_LOCATION_FMT   DLB_RIFF_LOCATION_FMT   /**< printf format spec for dlb_wave_location */

/*******************************************************************************
data types
******************************************************************************/
#define DLB_WAVE_NCHUNK 3   /**< number of special chunks we track */

typedef struct dlb_wave_format_s
{
    unsigned short  format_type;
    unsigned short  channel_count;
    unsigned long   sample_rate;
    unsigned long   bytes_per_second;
    unsigned short  block_alignment;
    unsigned short  bits_per_sample;
    unsigned short  octets_per_sample;
    unsigned short  valid_bits_per_sample;
    unsigned long   channel_mask;
} dlb_wave_format;

/** Represents a WAVE file.
 * This structure is exposed in this header file so you can conveniently
 * allocate local variables of this type without resorting to dynamic
 * memory allocation. Unfortunately, for C to know how big it is, we have
 * to expose the contents of the struct. This is not an invitation to
 * directly access the struct members. You should treat them as private and
 * use the functions below to work with dlb_wave_file objects.
 */
typedef struct dlb_wave_file_s
{
    dlb_riff_file   riff;
    dlb_wave_format format;
    unsigned        format_flags;
    unsigned        private_flags;
    dlb_riff_size   riff_size;      /**< RIFF chunk size */
    dlb_riff_size   data_size;      /**< data chunk size */
    unsigned        nchunk;         /**< number of special chunks tracked */
    dlb_riff_chunk  chunks[DLB_WAVE_NCHUNK];
    dlb_riff_chunk *priff;          /**< riff chunk (during writing) */
    dlb_riff_chunk *pds64;          /**< ds64 chunk (during writing) */
    dlb_riff_chunk *pdata;          /**< data chunk */

    /** head of optional chain of custom chunk handlers */
    const dlb_riff_chunk_handler *phandler;
} dlb_wave_file;

/******************************************************************************
general functions
******************************************************************************/
unsigned long
dlb_wave_get_sample_rate
    (const dlb_wave_file *pwf);

unsigned
dlb_wave_get_format_flags
    (const dlb_wave_file *pwf);

unsigned
dlb_wave_get_channel_count
    (const dlb_wave_file *pwf);

unsigned
dlb_wave_get_bit_depth
    (const dlb_wave_file *pwf);

unsigned long
dlb_wave_get_channel_mask
    (const dlb_wave_file *pwf);

/* Returns the number of frames in the file.
 * A frame is a sample for each channel.
 * NOTE: This function's behaviour is undefined while writing a file.
 */
dlb_wave_location
dlb_wave_get_num_frames
    (const dlb_wave_file *pwf);

/* Returns the current frame number we are up to.
 * A frame is a sample for each channel. */
dlb_wave_location
dlb_wave_get_current_frame
    (const dlb_wave_file *pwf);

const dlb_wave_format*
dlb_wave_get_format
    (const dlb_wave_file *pwf);

/** Seeks to the desired frame location within the stream.
 * A frame is a sample for each channel.
 * The valid frame range is: 0 <= target_frame < dlb_wave_get_num_frames(pwf).
 * NOTE: 
 *  - This function's behaviour is undefined while writing a file.
 *  - If an invalid frame is requested, the operation is not performed and a return code indicating error is returned.
 *  - The current stream position is unspecified if the seek failed with a non-zero returned code.
 *
 * @param stream        The dlb_wave_file stream where the seek is to be performed.
 * @param target_frame  This (unsigned) frame number to seek.
 * @return
 *  DLB_RIFF_OK         on successful seek, otherwise:
 *  DLB_WAVE_E_NODATA   if the wave file does not exist, does not contain a valid data section,
 *                          or the target_frame is outside sample data range.
 *  DLB_WAVE_E_NSEEK    if the frame data is not an octet multiple, or the seek failed.
 */
int
dlb_wave_seek_to_frame
    (dlb_wave_file      *pwf
    ,dlb_wave_location  target_frame);

void
dlb_wave_close
    (dlb_wave_file *pwf);

/******************************************************************************
functions for reading wave files
******************************************************************************/

/** Open a wave file for reading. */
int
dlb_wave_open_read
    (dlb_wave_file                *pwf
    ,const char                   *filename     
    ,const dlb_riff_chunk_handler *phandler /**< optional chain of handlers */
    );

/** Open a wave file for reading based on the provided dlb_octfile. */
int
dlb_wave_octfile_read
    (dlb_wave_file                *pwf
    ,dlb_octfile                  *pfile
    ,const dlb_riff_chunk_handler *phandler /**< optional chain of handlers */
    );

/** Read some audio data from the file. */
int
dlb_wave_read_data
    (dlb_wave_file *pwf
    ,void          *pdata
    ,size_t         ndata
    ,size_t        *pnread
    );

/******************************************************************************
functions for writing wave files
******************************************************************************/
int
dlb_wave_open_write
    (dlb_wave_file  *pwf
    ,const char     *filename
    ,unsigned        format_flags
    ,unsigned long   sample_rate
    ,unsigned        channel_count
    ,unsigned long   channel_mask
    ,unsigned        bit_depth
    );

/** Open a wave file for writing based on the provided dlb_octfile. */
int
dlb_wave_octfile_write
    (dlb_wave_file  *pwf
    ,dlb_octfile    *pfile
    ,unsigned        format_flags
    ,unsigned long   sample_rate
    ,unsigned        channel_count
    ,unsigned long   channel_mask
    ,unsigned        bit_depth
    );

int
dlb_wave_insert_chunk
    (dlb_wave_file  *pwf
    ,dlb_riff_chunk *priff
    ,const char      id[DLB_RIFF_NID]
    );

int
dlb_wave_begin_data
    (dlb_wave_file  *pwf);

int
dlb_wave_write_data
    (dlb_wave_file  *pwf
    ,const void     *pdata
    ,size_t          ndata
    );

int
dlb_wave_end_data
    (dlb_wave_file  *pwf);

#endif
