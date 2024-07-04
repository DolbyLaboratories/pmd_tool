/************************************************************************
 * dlb_wave
 * Copyright (c) 2011-2015, Dolby Laboratories Inc.
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

#include "dlb_wave/include/dlb_riff.h"
#include <assert.h>

/* The format (in byte offsets) of any chunk in a RIFF file. Avoid the use
 * of structs so we don't have to worry about struct member alignment/packing
 * issues. That means increased portability.
 */
#define DLB_RIFF_CHUNK_TYPE0 0 /* first character of chunk type */
#define DLB_RIFF_CHUNK_TYPE1 1 /* second character of chunk type */
#define DLB_RIFF_CHUNK_TYPE2 2 /* third character of chunk type */
#define DLB_RIFF_CHUNK_TYPE3 3 /* fourth character of chunk type */
#define DLB_RIFF_CHUNK_SIZE0 4 /* data size byte 0 (LSB) */
#define DLB_RIFF_CHUNK_SIZE1 5 /* data size byte 1 */
#define DLB_RIFF_CHUNK_SIZE2 6 /* data size byte 2 */
#define DLB_RIFF_CHUNK_SIZE3 7 /* data size byte 3 (MSB) */
#define DLB_RIFF_CHUNK_HDR   8 /* size (chars) of the chunk header */

/* This macro extracts the chunk size into a long given a pointer to an array
 * of chars containing the chunk header.
 */
#define DLB_RIFF_CHUNK_SIZE(hdr)\
	( (((dlb_riff_size) ((unsigned char)hdr[DLB_RIFF_CHUNK_SIZE0])) <<  0)\
	| (((dlb_riff_size) ((unsigned char)hdr[DLB_RIFF_CHUNK_SIZE1])) <<  8)\
	| (((dlb_riff_size) ((unsigned char)hdr[DLB_RIFF_CHUNK_SIZE2])) << 16)\
	| (((dlb_riff_size) ((unsigned char)hdr[DLB_RIFF_CHUNK_SIZE3])) << 24)\
	)

/* Make a little-endian four byte array from a dlb_riff_size. */
#define DLB_RIFF_M4(a,i)\
	(a)[0] = (unsigned char) (((dlb_riff_size)i >> 0) & 0xff);\
	(a)[1] = (unsigned char) (((dlb_riff_size)i >> 8) & 0xff);\
	(a)[2] = (unsigned char) (((dlb_riff_size)i >> 16) & 0xff);\
	(a)[3] = (unsigned char) (((dlb_riff_size)i >> 24) & 0xff);

/******************************************************************************
functions for manipulating handlers
******************************************************************************/

dlb_riff_chunk_handler
dlb_riff_chunk_handler_new
    (int                          (*cb)(dlb_riff_chunk*, void*)
    ,void                          *context
    ,const dlb_riff_chunk_handler  *pnext_handler
    )
{
    dlb_riff_chunk_handler handler;
    handler.cb = cb;
    handler.context = context;
    handler.pnext_handler = pnext_handler;
    return handler;
}

int
dlb_riff_chunk_handler_call_chain
    (const dlb_riff_chunk_handler *phandler
    ,dlb_riff_chunk               *pchunk
    )
{
    int status = DLB_RIFF_OK;
    if (phandler)
    {
        int child_status;

        /* Call the handler callback with its context */
        child_status = phandler->cb(pchunk, phandler->context);
        if (child_status)
        {
            status = child_status;
        }

        /* Call next handler in chain */
        child_status = dlb_riff_chunk_handler_call_chain
            (phandler->pnext_handler
            ,pchunk
            );
        if ((child_status < 0) || ((child_status) && (!status)))
        {
            status = child_status;
        }
    }
    return status;
}

/******************************************************************************
general functions
******************************************************************************/
void
dlb_riff_close
    (dlb_riff_file  *priff)
{
    if (&priff->file == priff->pfile)
    {
        dlb_octfile_close(priff->pfile);
    }
}

const char*
dlb_riff_chunk_id
    (const dlb_riff_chunk *pck)
{
    return pck->id;
}

int
dlb_riff_chunk_id_matches
    (const dlb_riff_chunk *pck
    ,const char            id[DLB_RIFF_NID]
    )
{
    unsigned i;
    for (i = 0; i < DLB_RIFF_NID; i++)
    {
        if (id[i] != pck->id[i])
        {
            return 0;
        }
    }
    return 1;
}

int
dlb_riff_chunk_status
    (const dlb_riff_chunk *pck)
{
    return pck->status;
}

dlb_riff_size
dlb_riff_chunk_size
    (const dlb_riff_chunk *pck)
{
    return pck->size;
}

dlb_riff_location
dlb_riff_chunk_location
    (const dlb_riff_chunk *pck)
{
    return pck->location;
}

/******************************************************************************
functions for reading riff files
******************************************************************************/
int
dlb_riff_open_read
    (dlb_riff_file  *priff
    ,const char     *filename
    )
{
    priff->pfile = &priff->file;
    if (NULL == dlb_octfile_open(priff->pfile, filename, "rb"))
    {
        priff->pfile = NULL;
        return DLB_RIFF_E_FILE;
    }
    else
    {
        return DLB_RIFF_OK;
    }
}

int
dlb_riff_octfile_read
    (dlb_riff_file  *priff
    ,dlb_octfile    *pfile
    )
{
    priff->pfile = pfile;
    if (NULL == priff->pfile)
    {
        return DLB_RIFF_E_FILE;
    }
    else
    {
        return DLB_RIFF_OK;
    }
}

int
dlb_riff_read_next_chunk
    (dlb_riff_file  *priff
    ,dlb_riff_chunk *pck
    )
{
    unsigned i;
    char hdr[DLB_RIFF_CHUNK_HDR];
    size_t read;

    /* initialise chunk */
    pck->id[0]      = 0;
    pck->priff      = priff;
    pck->pparent    = 0;

    /* remember file location */
    if (dlb_octfile_getpos(priff->pfile, &pck->position))
    {
        return pck->status = DLB_RIFF_E_TELL;
    }

    /* read RIFF header */
    read = dlb_octfile_read(hdr, sizeof(hdr[0]), DLB_RIFF_CHUNK_HDR, priff->pfile);
    if (!read)
    {
        return pck->status = DLB_RIFF_W_EOF;
    }
    else if (read != DLB_RIFF_CHUNK_HDR)
    {
        return pck->status = DLB_RIFF_E_CHUNK;
    }

    /* populate fourcc code */
    for (i = 0; i < DLB_RIFF_NID; i++)
    {
        pck->id[i] = hdr[DLB_RIFF_CHUNK_TYPE0+i];
    }
    pck->id[i] = 0; /* null terminate */

    /* populate size */
    pck->size = DLB_RIFF_CHUNK_SIZE(hdr);
    pck->location = 0;

    return pck->status = DLB_RIFF_OK;
}

size_t
dlb_riff_read_chunk_data
    (dlb_riff_chunk *pck
    ,unsigned char  *pdata
    ,size_t          ndata
    )
{
    assert(pck->status >= 0);

    if ((pck->location + ndata) > pck->size)
    {
        ndata = (size_t) (pck->size - pck->location);
    }
    ndata = dlb_octfile_read(pdata, 1, ndata, pck->priff->pfile);
    pck->location += ndata;
    return ndata;
}

static
int
dlb_riff_seek_chunk_internal
    (dlb_riff_chunk    *ck
    ,dlb_riff_location  location
    )
{
    int status;
    /* location is the distance from the start of the chunk. This distance
     * is originally the location requested, but because we may only be able
     * to jump in steps of LONG_MAX, we may take several calls to
     * dlb_octfile_seek() to get there.
     */
    
    /* Move to start of the chunk */
    status = dlb_octfile_setpos(ck->priff->pfile, &ck->position);
    if (status)
    {
        return DLB_RIFF_E_SEEK;
    }
    
    /* The start of the chunk has a 4 character code, then 4 octets with the
     * chunk size. We need to jump past them, and not include them in the
     * location */
    location += DLB_RIFF_CHUNK_HDR;
    ck->location = 0;
    
    while (location)
    {
        long thisseek = (location > LONG_MAX) ? LONG_MAX : (long)location;

        status = dlb_octfile_seek(ck->priff->pfile, thisseek, DLB_OCTFILE_SEEK_CUR);
        if (status)
        {
            ck->location = (ck->location > DLB_RIFF_CHUNK_HDR) ? (ck->location - DLB_RIFF_CHUNK_HDR) : 0;
            return DLB_RIFF_E_SEEK;
        }

        location     -= thisseek;
        ck->location += thisseek;
    }

    ck->location = (ck->location > DLB_RIFF_CHUNK_HDR) ? (ck->location - DLB_RIFF_CHUNK_HDR) : 0;
    return DLB_RIFF_OK;
}

int
dlb_riff_skip_chunk_data
    (dlb_riff_chunk *pck)
{
    assert(pck->status >= 0);
    
    return dlb_riff_seek_chunk_internal(pck, pck->size + (pck->size & 1));
}

int
dlb_riff_seek_chunk
    (dlb_riff_chunk    *ck
    ,dlb_riff_location  location
    )
{
    assert(ck->status >= 0);

    /* Don't allow jumps outside this chunk */
    if (location > dlb_riff_chunk_size(ck))
    {
        return DLB_RIFF_E_SEEK;
    }

    return dlb_riff_seek_chunk_internal(ck, location);
}

void
dlb_riff_override_chunk_size
    (dlb_riff_chunk *priff
    ,dlb_riff_size   size
    )
{
    priff->size = size;
}

/******************************************************************************
functions for writing riff files
******************************************************************************/
int
dlb_riff_open_write
    (dlb_riff_file  *priff
    ,const char     *filename
    )
{
    priff->pfile = &priff->file;
    if (NULL == dlb_octfile_open(priff->pfile, filename, "wb"))
    {
        priff->pfile = NULL;
        return DLB_RIFF_E_FILE;
    }
    else
    {
        return DLB_RIFF_OK;
    }
}

/** Open a RIFF file for writing based on the provided dlb_octfile. */
int
dlb_riff_octfile_write
    (dlb_riff_file  *priff
    ,dlb_octfile    *pfile
    )
{
    priff->pfile = pfile;
    if (NULL == priff->pfile)
    {
        return DLB_RIFF_E_FILE;
    }
    else
    {
        return DLB_RIFF_OK;
    }
}

int
dlb_riff_open_chunk
    (dlb_riff_file  *priff
    ,dlb_riff_chunk *pck
    ,dlb_riff_chunk *pparent
    ,const char      id[DLB_RIFF_NID]
    )
{
    unsigned i;
    char hdr[DLB_RIFF_CHUNK_HDR];

    /* initialise chunk */
    pck->priff = priff;
    pck->pparent = pparent;
    pck->id[0] = 0;

    /* always add chunks at the end of the file */
    if (dlb_octfile_seek(priff->pfile, 0, DLB_OCTFILE_SEEK_END))
    {
        return pck->status = DLB_RIFF_E_SEEK;
    }

    for (i = 0; i < DLB_RIFF_NID; i++)
    {
        pck->id[i] = id[i];
    }
    pck->id[i] = 0;
    if (dlb_octfile_getpos(priff->pfile, &pck->position))
    {
        return pck->status = DLB_RIFF_E_TELL;
    }
    pck->location = 0;
    pck->size = 0;

    /* write header to file */
    for (i = 0; i < DLB_RIFF_NID; i++)
    {
        hdr[DLB_RIFF_CHUNK_TYPE0+i] = id[i];
    }
    /* initialise size to 0 */
    DLB_RIFF_M4(hdr + DLB_RIFF_CHUNK_SIZE0, pck->size)
    if (DLB_RIFF_CHUNK_HDR != dlb_octfile_write(hdr, 1, DLB_RIFF_CHUNK_HDR, priff->pfile))
    {
        return pck->status = DLB_RIFF_E_WRITE;
    }

    /* adjust parent locations */
    while (pparent)
    {
        pparent->location += DLB_RIFF_CHUNK_HDR;
        pparent = pparent->pparent;
    }

    return pck->status = DLB_RIFF_OK;
}

int
dlb_riff_close_chunk
    (dlb_riff_chunk *pck
    )
{
    unsigned i;
    char hdr[DLB_RIFF_CHUNK_HDR];
    dlb_riff_size size;
    int okstatus = DLB_RIFF_OK;

    assert(pck->status >= 0);

    /* We must add a pad octet to odd-length chunks. This is defined in the
     * RIFF specification. */
    if (pck->location & 1)
    {
        unsigned char data[1];
        data[0] = 0;
        if (1 != dlb_octfile_write(data, 1, 1, pck->priff->pfile))
        {
            return pck->status = DLB_RIFF_E_WRITE;
        }
    }

    if (pck->location > pck->size)
    {
        if (pck->pparent)
        {
            /* update parent location */
            pck->pparent->location += pck->location + (pck->location & 1) - pck->size;
        }
        pck->size = pck->location;
    }

    /* Seek back to header */
    if (dlb_octfile_setpos(pck->priff->pfile, &pck->position))
    {
        return pck->status = DLB_RIFF_E_SEEK;
    }

    /* Rebuild header */
    for (i = 0; i < DLB_RIFF_NID; i++)
    {
        hdr[DLB_RIFF_CHUNK_TYPE0+i] = pck->id[i];
    }
    size = pck->size;
    if (size > DLB_RIFF_MAX_SIZE)
    {
        size = DLB_RIFF_MAX_SIZE;
        okstatus = DLB_RIFF_W_TOOBIG;
    }
    DLB_RIFF_M4(hdr+DLB_RIFF_CHUNK_SIZE0, size);

    /* Write header back to file */
    if (DLB_RIFF_CHUNK_HDR != dlb_octfile_write(hdr, 1, DLB_RIFF_CHUNK_HDR, pck->priff->pfile))
    {
        return pck->status = DLB_RIFF_E_WRITE;
    }

    return pck->status = okstatus;
}

int
dlb_riff_rename_chunk
    (dlb_riff_chunk *pck
    ,const char      id[DLB_RIFF_NID]
    )
{
    unsigned i;

    assert(pck->status >= 0);

    /* This works on an open chunk. The chunk header gets rewritten
     * when the chunk is closed.
     */
    for (i = 0; i < DLB_RIFF_NID; i++)
    {
        pck->id[i] = id[i];
    }
    pck->id[i] = 0;
    return pck->status = DLB_RIFF_OK;
}

int
dlb_riff_reopen_chunk
    (dlb_riff_chunk *pck)
{
    assert(pck->status >= 0);
    return dlb_riff_seek_chunk(pck, 0);

}

int
dlb_riff_write_chunk_data
    (dlb_riff_chunk *pck
    ,const char     *pdata
    ,size_t          ndata
    )
{
    assert(pck->status >= 0);

    /* If writing to an existing chunk (i.e. size has already been set), we
     * will not write past the end of the chunk. In-fact, we will not write at
     * all... */
    if ((pck->size) && ((pck->location + ndata) > pck->size))
    {
        return pck->status = DLB_RIFF_E_SPACE;
    }

    if (ndata != dlb_octfile_write(pdata, 1, ndata, pck->priff->pfile))
    {
        return pck->status = DLB_RIFF_E_WRITE;
    }

    pck->location += ndata;
    return pck->status = DLB_RIFF_OK;
}
