/************************************************************************
 * dlb_wave
 * Copyright (c) 2023, Dolby Laboratories Inc.
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
#ifndef dlb_riff_H
#define dlb_riff_H

/* The dlb_octfile module is used to abstract away differences between
 * fread implementations on platforms where a byte (char) is not an octet
 * (8 bits), such as the TI C55x and the Versilicon ZSP.
 */
#include "dlb_octfile/include/dlb_octfile.h"
#include <limits.h>

/***************************************************************************//**
Status codes. Zero for success. Warnings are positive. Errors are negative.
*******************************************************************************/
#define DLB_RIFF_OK         0 /**< unequivocal success */
#define DLB_RIFF_W_EOF      1 /**< no more chunks available */
#define DLB_RIFF_W_TOOBIG   2 /**< larger than maximum chunk size */
#define DLB_RIFF_E_FILE    -1 /**< cannot open file */
#define DLB_RIFF_E_CHUNK   -2 /**< invalid chunk found */
#define DLB_RIFF_E_SEEK    -3 /**< failed to seek within file */
#define DLB_RIFF_E_TELL    -4 /**< failed to get file location */
#define DLB_RIFF_E_WRITE   -5 /**< failed to write to file */
#define DLB_RIFF_E_SPACE   -6 /**< insufficient space reserved in chunk */

#define DLB_RIFF_NID        4 /**< number of characters in a chunk identifier */

#define DLB_RIFF_MAX_SIZE   0xffffffffUL  /**< maximum size of a chunk */

/***************************************************************************//**
Data type for representing the sizes and locations in riff chunks
******************************************************************************/
#ifdef ULLONG_MAX

/* unsigned long long is a C99 standard type guaranteed to be at least 64 bit.
 * If the compiler supports it, ULLONG_MAX will be defined.
 */

/* 1 indicates that 64-bit (long long) values are used for the following */
#define DLB_RIFF_64             1
/* dlb_riff_size represents chunk sizes */
typedef unsigned long long      dlb_riff_size;
#define DLB_RIFF_SIZE_MAX       ULLONG_MAX
#define DLB_RIFF_SIZE_MIN       0
#define DLB_RIFF_SIZE_FMT       "%llu"  /**< printf format spec for dlb_riff_size */
/* dlb_riff_offset is a signed offset */
typedef signed long long        dlb_riff_offset;
#define DLB_RIFF_OFFSET_MAX     LLONG_MAX
#define DLB_RIFF_OFFSET_MIN     LLONG_MIN
#define DLB_RIFF_OFFSET_FMT     "%ll"   /**< printf format spec for dlb_riff_offset */
/* dlb_riff_location is an unsigned location */
typedef unsigned long long      dlb_riff_location;
#define DLB_RIFF_LOCATION_MAX   ULLONG_MAX
#define DLB_RIFF_LOCATION_MIN   0
#define DLB_RIFF_LOCATION_FMT   "%llu"  /**< printf format spec for dlb_riff_location */
#else

/* If ULLONG_MAX is not defined, fall back to the unsigned long data type.
 * In this case, some RF64 functionality won't be available.
 */

/* 0 indicates that 32-bit (long) values are used for the following */
#define DLB_RIFF_64             0
/* dlb_riff_size represents chunk sizes */
typedef unsigned long           dlb_riff_size;
#define DLB_RIFF_SIZE_MAX       ULONG_MAX
#define DLB_RIFF_SIZE_MIN       0
#define DLB_RIFF_SIZE_FMT       "%lu"   /**< printf format spec for dlb_riff_size */
/* dlb_riff_offset is a signed offset */
typedef signed long             dlb_riff_offset;
#define DLB_RIFF_OFFSET_MAX     LONG_MAX
#define DLB_RIFF_OFFSET_MIN     LONG_MIN
#define DLB_RIFF_OFFSET_FMT     "%l"    /**< printf format spec for dlb_riff_offset */
typedef unsigned long           dlb_riff_location;
/* dlb_riff_location is an unsigned location */
#define DLB_RIFF_LOCATION_MAX   ULONG_MAX
#define DLB_RIFF_LOCATION_MIN   0
#define DLB_RIFF_LOCATION_FMT   "%lu"   /**< printf format spec for dlb_riff_location */
#endif

/***************************************************************************//**
Represents an open RIFF file.
******************************************************************************/
typedef struct dlb_riff_file_s
{
    dlb_octfile          file;       /**< file stream object */
    dlb_octfile         *pfile;     /**< pointer to the file stream object */
} dlb_riff_file;

/***************************************************************************//**
Represents a chunk in a RIFF file.
This structure is exposed in this header file so you can conveniently
allocate local variables of this type without resorting to dynamic
memory allocation. Unfortunately, for C to know how big it is, we have
to expose the contents of the struct. This is not an invitation to
directly access the struct members. You should treat them as private and
use the functions below to work with dlb_riff_chunk objects.
******************************************************************************/
typedef struct dlb_riff_chunk_s dlb_riff_chunk;

struct dlb_riff_chunk_s
{
    int                  status;                /**< most recent status */
    dlb_riff_file*       priff;                 /**< back pointer to file */
    dlb_riff_chunk*      pparent;               /**< parent chunk when writing */
    dlb_riff_size        size;                  /**< chunk size */
    dlb_riff_size        location;              /**< where we are in the data */
    dlb_octpos           position;              /**< position in file */
    char                 id[DLB_RIFF_NID+1];    /**< chunk id + null term */
};

/***************************************************************************//**
Represents a handler (callback) in a chain of RIFF chunk handlers.
******************************************************************************/
typedef struct dlb_riff_chunk_handler_s dlb_riff_chunk_handler;

struct dlb_riff_chunk_handler_s
{
    /** callback */
    int                     (*cb)(dlb_riff_chunk*, void*); 
    /** context callback */
    void                    *context;
    /** next handler in chain */
    const dlb_riff_chunk_handler  *pnext_handler;
};

/******************************************************************************
functions for manipulating handlers
******************************************************************************/

/** Helper function for creating a new handler */
dlb_riff_chunk_handler
dlb_riff_chunk_handler_new
    (int                          (*cb)(dlb_riff_chunk*, void*)
    ,void                          *context
    ,const dlb_riff_chunk_handler  *pnext_handler
    );

/** Call a chain of handlers */
int
dlb_riff_chunk_handler_call_chain
    (const dlb_riff_chunk_handler *phandler
    ,dlb_riff_chunk               *pchunk
    );
    
/******************************************************************************
general functions
******************************************************************************/

/** Close an open RIFF file */
void
dlb_riff_close
    (dlb_riff_file  *priff);

/** Get the ID of a chunk */
const char*
dlb_riff_chunk_id
    (const dlb_riff_chunk *pck);

/** Determine whether a chunk's ID matches a constant ID */
int
dlb_riff_chunk_id_matches
    (const dlb_riff_chunk *pck
    ,const char            id[DLB_RIFF_NID]
    );

/** Get the most recent status of a chunk */
int
dlb_riff_chunk_status
    (const dlb_riff_chunk *pck);

/** Get the size of a chunk */
dlb_riff_size
dlb_riff_chunk_size
    (const dlb_riff_chunk *pck);

/** Get the current location within chunk */
dlb_riff_location
dlb_riff_chunk_location
    (const dlb_riff_chunk *pck);

/******************************************************************************
functions for reading riff files
******************************************************************************/

/** Open a RIFF file for reading. */
int
dlb_riff_open_read
    (dlb_riff_file  *priff
    ,const char     *filename
    );

/** Open a RIFF file for reading based on the provided dlb_octfile. */
int
dlb_riff_octfile_read
    (dlb_riff_file  *priff
    ,dlb_octfile    *pfile
    );

/** Open the next RIFF chunk for reading. The caller is responsible for
 * ensuring that the underlying stream is at the position where the next RIFF
 * chunk header can be read. This can be ensured by calling the
 * dlb_riff_skip_chunk_data() function on the previously opened chunk. */
int
dlb_riff_read_next_chunk
    (dlb_riff_file  *priff
    ,dlb_riff_chunk *pck
    );

/** Read data from the current RIFF chunk. */
size_t
dlb_riff_read_chunk_data
    (dlb_riff_chunk *pck
    ,unsigned char  *pdata
    ,size_t          ndata
    );

/** Skip to the next RIFF chunk. */
int
dlb_riff_skip_chunk_data
    (dlb_riff_chunk *pck);

/** Seeks to a position inside a chunk. The position is specified using
 * the same units as dlb_riff_chunk_location() and must be less than
 * or equal to the number returned by dlb_riff_chunk_size() for this chunk.
 * @param stream    The riff file where the seek is to be performed.
 * @param location  The position in this chunk to jump
 * @return
 *  non-zero on unsuccessful seek. Position in chunk is unspecified.
 *  0 on successful seek.
 */
int
dlb_riff_seek_chunk
    (dlb_riff_chunk    *ck
    ,dlb_riff_location  location
    );

/** Override the size of a RIFF chunk read from the chunk header.
 *  This is useful in RF64 files, where the actual 64 bit size of
 *  the RIFF and data chunks comes from the ds64 chunk instead of
 *  the RIFF and data chunk headers.
 */
void
dlb_riff_override_chunk_size
    (dlb_riff_chunk *priff
    ,dlb_riff_size   size
    );

/******************************************************************************
functions for writing riff files
******************************************************************************/

/** Open a new RIFF file for writing. */
int
dlb_riff_open_write
    (dlb_riff_file  *priff
    ,const char     *filename
    );

/** Open a RIFF file for writing based on the provided dlb_octfile. */
int
dlb_riff_octfile_write
    (dlb_riff_file  *priff
    ,dlb_octfile    *pfile
    );

/** Append a new chunk and open it for writing. */
int
dlb_riff_open_chunk
    (dlb_riff_file  *pfile
    ,dlb_riff_chunk *priff
    ,dlb_riff_chunk *pparent
    ,const char      id[DLB_RIFF_NID]
    );

/** Reopen a previously closed chunk for writing. */
int
dlb_riff_reopen_chunk
    (dlb_riff_chunk *priff);

/** Rename an open chunk. */
int
dlb_riff_rename_chunk
    (dlb_riff_chunk *pck
    ,const char      id[DLB_RIFF_NID]
    );

/** Write data into current open chunk. */
int
dlb_riff_write_chunk_data
    (dlb_riff_chunk *pck
    ,const char     *pdata
    ,size_t          ndata
    );

/** Close an open chunk. */
int
dlb_riff_close_chunk
    (dlb_riff_chunk *priff
    );

#endif
