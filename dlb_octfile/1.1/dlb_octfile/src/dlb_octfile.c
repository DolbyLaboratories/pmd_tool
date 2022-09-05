/************************************************************************
 * dlb_octfile
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

#include "dlb_octfile/include/dlb_octfile.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

#define CHAR_OCTET (CHAR_BIT / 8)          /* octets per char */

/* bitmask for octet at top of char */
#define TOP_MASK ((unsigned char)(0xff << (CHAR_BIT - 8))) 
#define ALL_MASK ((unsigned char)-1)       /* all bits set */

#define MEMFILE_ALLOC_SIZE  (1024)
extern const dlb_octfile_vtable dlb_octfile_disk_vtable;
extern const dlb_octfile_vtable dlb_octfile_memory_vtable;
int
dlb_octfile_close(dlb_octfile *stream)
{
    return stream->vtable->octfile_close(stream);
}

int
dlb_octfile_flush(dlb_octfile *stream)
{
    return stream->vtable->octfile_flush(stream);
}

static
dlb_octfile *
open_internal(dlb_octfile* ret, const char *mode)
{
    if (strchr(mode, 'b') == NULL)
    {
        /* Text mode doesn't make sense for this */
#ifndef NDEBUG
        fprintf(stderr, "Error: file was opened in text mode.\n");
#endif
        /* 
         * Setting errno here would be polite.
         */
        return NULL;
    }

    if (    strchr(mode, 'w') == NULL
       &&   strchr(mode, 'r') == NULL
       &&   strchr(mode, '+') == NULL
       )
    {
        /* File should be opened for either read or write */
#ifndef NDEBUG
        fprintf(stderr, "Error: file was not opened for read or write.\n");
#endif
        /* 
         * Setting errno here would be polite.
         */
        return NULL;
    }

    ret->file = NULL;
    ret->mem_base = NULL;
    ret->mem_offset = 0;
    ret->mem_size = 0;
    ret->mem_alloc_size = 0;
    ret->mem_fixed = 0;
    ret->mem_eof = 0;

    ret->read_buffer = 0;
    ret->read_buffer_mask = 0;
    ret->write_buffer = 0;
    ret->write_buffer_mask = 0;

    return ret;
}

dlb_octfile *
dlb_octfile_open(dlb_octfile* ret, const char *filename, const char *mode)
{
    if (NULL != open_internal(ret, mode))
    {
        ret->file = fopen(filename, mode);
        if (!ret->file)
        {
            return NULL;
        }
        else
        {
            ret->vtable = &dlb_octfile_disk_vtable;
            return ret;
        }
    }
    else
    {
        return NULL;
    }
}

dlb_octfile *
dlb_octfile_open_memory_fixed
    (dlb_octfile*ret
    ,size_t      size
    ,void       *memory
    ,const char *mode 
    )
{
    if (NULL != open_internal(ret, mode))
    {
        ret->mem_base = memory;
        ret->mem_alloc_size = size;
        ret->mem_fixed = 1;
        if (strchr(mode, 'w') == NULL)
        {
            ret->mem_size = size;
        }
        ret->vtable = &dlb_octfile_memory_vtable;
        return ret;
    }
    else
    {
        return NULL;
    }
}

dlb_octfile*
dlb_octfile_open_memory
    (dlb_octfile*ret
    ,const char *mode 
    )
{
    if (NULL != open_internal(ret, mode))
    {
        ret->mem_base = malloc(MEMFILE_ALLOC_SIZE);
        if (NULL != ret->mem_base)
        {
            ret->mem_alloc_size = MEMFILE_ALLOC_SIZE;
            ret->vtable = &dlb_octfile_memory_vtable;
            return ret;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
}

const
void *
dlb_octfile_get_memory
    (dlb_octfile *stream
    )
{
    return stream->mem_base;
}

#if CHAR_BIT != 8
static size_t
flush_write_buffer(dlb_octfile *stream)
{
    /*
     * This will empty the write buffer. We require the write buffer to
     * be full. Return value is the number of octets flushed (either 0
     * or CHAR_OCTET).
     */
    size_t chars_written;
    assert(stream->write_buffer_mask == ALL_MASK);
    
    chars_written = stream->vtable->octfile_write(&stream->write_buffer, 1, 1, stream);
    
    if (chars_written)
    {
        assert(chars_written == 1);
        
        stream->write_buffer_mask = 0;
        stream->write_buffer = 0;
        
        return CHAR_OCTET;
    }
    return 0;
}

static size_t
ugly_flush_write_buffer(dlb_octfile *stream)
{
    size_t padding_octets = 0;
    size_t written_octets;
    assert(stream->write_buffer);
    /* 
     * This fills up the write_buffer with zeros, and then flushes it. It
     * is probably a bug every time it is used, but it seems slightly better
     * than discarding the contents of the buffer.
     * Return value is the number of octets written (not including the extra
     * padding 
     */
    while (stream->write_buffer_mask != ALL_MASK)
    {
        stream->write_buffer = (stream->write_buffer >> 8);
        stream->write_buffer_mask = (stream->write_buffer_mask >> 8) | TOP_MASK;
        padding_octets++;
    }
#ifndef NDEBUG
    if (padding_octets)
    {
        fprintf(stderr, "flushing incomplete write buffer\n");
    }
#endif
    
    written_octets = flush_write_buffer(stream);
    
    if (written_octets)
    {
        assert(written_octets == CHAR_OCTET);
        return (CHAR_OCTET - padding_octets);
    }
    else
    {
        return 0;
    }
}
#endif /* CHAR_BIT != 8 */

#if CHAR_BIT != 8
static void
begin_read(void **pptr, size_t *pnmemb, dlb_octfile *stream)
{
    /* '*pnmemb' is the total number of octets to read. */

    /* This will clear as much of our buffer as possible */
    while (stream->read_buffer_mask && *pnmemb)
    {
        (*pnmemb)--;
        
        *((unsigned char*)(*pptr)) = (stream->read_buffer & 0xff);
        stream->read_buffer = stream->read_buffer >> 8;
        stream->read_buffer_mask = stream->read_buffer_mask >> 8;
        
        ((*(unsigned char**)pptr))++;
    }
}

static void
main_read(void **pptr, size_t *pnmemb, dlb_octfile *stream)
{
	/* '*pnmemb' is the total number of octets to read. */

    /* 
     * This does the middle/easy part of a file read. We require
     * read_buffer to be empty (unless we aren't reading anything),
     * and we don't read any fractions of chars (so read_buffer
     * will be empty when we finish).
     */
    size_t chars_asked;
    size_t chars_read;
    unsigned int i;
    unsigned int write_to;
     
    if (!(*pnmemb))
    {
        return;
    }
    assert(stream->read_buffer_mask == 0);
    
    /* Read data */
	/* Calculate the number of whole 'chars' still to be read.  The
     * remaining octets will be taken care of by 'end_read()'. */
    chars_asked = (*pnmemb) / CHAR_OCTET;
    chars_read = stream->vtable->octfile_read(*pptr, 1, chars_asked, stream);
    
#ifndef NDEBUG
    if ((chars_read != 0) && (chars_asked != chars_read))
    {
        fprintf(stderr,
                "Warning: short read: asked for %d chars, got %d.\n",
                chars_asked, chars_read);
    }
#endif
    
    /* Zero pad and move data */
    if (chars_read > 0)
    {
        for (i = chars_read; i > 0; i--)
        {
            unsigned char this_char = ((char*)(*pptr))[i - 1];
            unsigned int j;
            
            /* Extract each byte from 'this_char' into a separate
             * 'char'.  This is done in place, starting with the char
             * that was read last.  */
            write_to = (i - 1) * CHAR_OCTET;
            for (j = 0; j < CHAR_OCTET; j++)
            {
                ((unsigned char*)(*pptr))[write_to + j] = this_char & 0xff;
                this_char = this_char >> 8;
            }
        }
    }
    
    /* Update work out how many octets we read */
    assert(chars_read * CHAR_OCTET <= *pnmemb);
    *pnmemb -= chars_read * CHAR_OCTET;
    (*((unsigned char **)pptr)) += chars_read * CHAR_OCTET;
}

static void
end_read(void **pptr, size_t *pnmemb, dlb_octfile *stream)
{
    /* '*pnmemb' is the total number of _bytes_ to read. */
    
	/*
     * This does the final bit of a read, where the amount needed
     * is less than a full char.
     */
    size_t chars_read;
    
    if (!(*pnmemb))
    {
        return;
    }
    assert(stream->read_buffer_mask == 0);
    if (*pnmemb >= CHAR_OCTET)
    {
        return; /* probably a short read before */
    }
    
     /* Read one more 'char', that we only need a portion of. */
    chars_read = stream->vtable->octfile_read(&stream->read_buffer, 1, 1, stream);
    if (!chars_read)
    {
        return;
    }
    
    stream->read_buffer_mask = ALL_MASK;
    
    /* 
     * The logic for beginning a read operation works fine here
     * now that read_buffer is filled
     */
    begin_read(pptr, pnmemb, stream);
}
#endif /* CHAR_BIT != 8 */

size_t
dlb_octfile_read(void *ptr, size_t size, size_t nmemb, dlb_octfile *stream)
{
	/* 'size': number of bytes per element */
    /* 'nmemb': number of elements */
#if CHAR_BIT == 8
    return stream->vtable->octfile_read(ptr, size, nmemb, stream);
#elif (CHAR_BIT % 8) == 0
    size_t orig_nmemb = nmemb;
    
    nmemb *= size;    /* Now 'nmemb' is the total number of _bytes_ to
                       * read. */
    
    begin_read(&ptr, &nmemb, stream);
    main_read(&ptr, &nmemb, stream);
    end_read(&ptr, &nmemb, stream);

    /* 'nmemb' is now the number of bytes that for some reason haven't
     * been read.  This corresponds to (nmemb/size) whole data
     * elements of 'size' bytes each. */
 	return (orig_nmemb - (nmemb / size));
#else
#error "CHAR_BIT must be a multiple of 8"
#endif
}

#if CHAR_BIT != 8
static const void *
begin_write(const void *ptr, size_t *pnmemb, dlb_octfile *stream)
{
    /* Write the shortest amount of data needed to clear write_buffer */
    if (stream->write_buffer_mask == 0)
    {
        return ptr;
    }
    
    while ((stream->write_buffer_mask != ALL_MASK) && (*pnmemb))
    {
        unsigned char octet = *(unsigned const char*)(ptr);
        assert((octet & 0xff) == octet);
        stream->write_buffer = (stream->write_buffer >> 8) 
                             | (octet << (CHAR_BIT - 8));
        stream->write_buffer_mask = (stream->write_buffer_mask >> 8) | TOP_MASK;
        
        (*pnmemb)--;
        ptr = ((const unsigned char*)ptr) + 1;
    }
    
    /* 
     * if flush_write buffer fails, then it won't clear write_buffer_mask,
     * so we aren't losing information by ignoring its return value.
     */
    flush_write_buffer(stream);
    
    return ptr;
}

static const void *
main_write(const void *ptr, size_t *pnmemb, dlb_octfile *stream)
{
    /* 
     * Write as many whole chars-worth of octets as we can.
     */
    unsigned int i;
    
    /* Check we aren't already finished */
    if (*pnmemb == 0)
    {
        return ptr;
    }
    /* 
     * If the write_buffer_mask is full, then begin_write must have failed
     * so we shouldn't try either. Otherwise, we must not have a partially
     * full write_buffer.
     */
    if (stream->write_buffer_mask == ALL_MASK)
    {
        return ptr;
    }
    assert(stream->write_buffer_mask == 0);
    
    /* 
     * It is important that we actually have some data to write or else
     * the main loop body will not execute, and *pnmemb will be corrupted
     * when i underflows.
     */
    if (!(*pnmemb / CHAR_OCTET))
    {
        return ptr;
    }
    
    /* Write all of the whole chars */
    for (i = 0; i + CHAR_OCTET <= *pnmemb; i += CHAR_OCTET)
    {
        unsigned char this_char = 0;
        unsigned int j;
        size_t chars_written;
        
        /* Pack together these octets */
        for (j = 0; j < CHAR_OCTET; j++)
        {
            unsigned char this_octet = ((unsigned const char*)(ptr))[CHAR_OCTET - j - 1];
            
            assert((this_octet & 0xff) == this_octet);
            this_char = (this_char << 8) | this_octet;
        }
        
        chars_written = stream->vtable->octfile_write(&this_char, 1, 1, stream);
        
        if (chars_written == 0)
        {
            i++;
            break;
        }
        assert(chars_written == 1);
        
        /* Update read pointer */
        ptr = ((const unsigned char*)ptr) + CHAR_OCTET;
    }
    
    /* Update octet remaining count */
    (*pnmemb) -= i;
    
    return ptr;
}

static const void *
end_write(const void *ptr, size_t *pnmemb, dlb_octfile *stream)
{
    /*
     * This does the last bit of an fwrite, which isn't quite enough
     * to use up a whole char. So we just put stuff into write_buffer.
     */
    if (*pnmemb == 0)
    {
        return ptr; /* we are already finished */
    }
    if (*pnmemb >= CHAR_OCTET)
    {
        return ptr; /* probably a short write in main_write */
    }
    if (stream->write_buffer_mask)
    {
        return ptr; /* probably a short write in begin_write */
    }
    
    while (*pnmemb)
    {
        unsigned char this_octet = *(unsigned const char*)(ptr);
        
        stream->write_buffer = (stream->write_buffer >> 8) 
                             | (this_octet << (CHAR_BIT - 8));
        stream->write_buffer_mask = (stream->write_buffer_mask >> 8) | TOP_MASK;
        
        (*pnmemb)--;
        ptr = ((const unsigned char*)ptr) + 1;
    }
    return ptr;
}
#endif /* CHAR_BIT != 8 */

size_t
dlb_octfile_write(const void *ptr, size_t size, size_t nmemb, dlb_octfile *stream)
{
	 /* 'size': number of bytes per element */
     /* 'nmemb': number of elements */

#if CHAR_BIT == 8
    return stream->vtable->octfile_write(ptr, size, nmemb, stream);
#elif (CHAR_BIT % 8) == 0
    size_t orig_nmemb = nmemb;
    
    nmemb *= size;    /* Now 'nmemb' is the total number of _bytes_ to
                       * write. */
    
    ptr = begin_write(ptr, &nmemb, stream);
    ptr = main_write(ptr, &nmemb, stream);
    ptr = end_write(ptr, &nmemb, stream);
    
    /* 'nmemb' is now the number of bytes that for some reason haven't
     * been written.  This corresponds to (nmemb/size) whole data
     * elements of 'size' bytes each. */

    return (orig_nmemb - (nmemb / size));
#else
#error "CHAR_BIT must be a multiple of 8"
#endif
}

int
dlb_octfile_seek(dlb_octfile *stream, long offset, int whence)
{
    /* Confirm whence values are consistent */
#if DLB_OCTFILE_SEEK_SET != SEEK_SET
#error "DLB_OCTFILE_SEEK_SET inconsistent with SEEK_SET"
#endif /* DLB_OCTFILE_SEEK_SET != SEEK_SET */
#if DLB_OCTFILE_SEEK_CUR != SEEK_CUR
#error "DLB_OCTFILE_SEEK_CUR inconsistent with SEEK_CUR"
#endif /* DLB_OCTFILE_SEEK_CUR != SEEK_CUR */
#if DLB_OCTFILE_SEEK_END != SEEK_END
#error "DLB_OCTFILE_SEEK_END inconsistent with SEEK_END"
#endif /* DLB_OCTFILE_SEEK_END != SEEK_END */

    /* Don't seek to awkward positions please */
    assert(offset % CHAR_OCTET == 0);

#if CHAR_BIT != 8
    if (stream->write_buffer_mask)
    {
        ugly_flush_write_buffer(stream);
    }
#endif
    
    stream->read_buffer_mask = 0;
    stream->read_buffer = 0;
    assert(stream->write_buffer_mask == 0);
    return stream->vtable->octfile_seek(stream, offset / CHAR_OCTET, whence);
}

int
dlb_octfile_setpos(dlb_octfile *stream, const dlb_octpos *pos)
{
#if CHAR_BIT != 8
    if (stream->write_buffer_mask)
    {
        ugly_flush_write_buffer(stream);
    }
#endif

    stream->read_buffer_mask = 0;
    stream->read_buffer = 0;
    assert(stream->write_buffer_mask == 0);
    return stream->vtable->octfile_setpos(stream, pos);
}

long
dlb_octfile_tell(dlb_octfile *stream)
{
#if CHAR_BIT != 8
    /* In this case 'dlb_octfile_write()' is using 'write_buffer'.  Any
     * residual byte still in 'write_buffer' wouldn't be included in the byte
     * count returned by 'dlb_octfile_tell()'. */
    assert(stream->write_buffer_mask == 0);
#endif
    return stream->vtable->octfile_tell(stream) * CHAR_OCTET;
}

int
dlb_octfile_getpos(dlb_octfile *stream, dlb_octpos *pos)
{
#if CHAR_BIT != 8
    /* In this case 'dlb_octfile_write()' is using 'write_buffer'.  Any
     * residual byte still in 'write_buffer' wouldn't be included in the byte
     * count returned by 'dlb_octfile_tell()'. */
    assert(stream->write_buffer_mask == 0);
#endif
    /* No monkey business manipulating fpos_t because it is an opaque
     * data type.
     */
    return stream->vtable->octfile_getpos(stream, pos);
}

void
dlb_octfile_rewind(dlb_octfile *stream)
{
#if CHAR_BIT != 8
    if (stream->write_buffer_mask)
    {
        ugly_flush_write_buffer(stream);
    }
#endif
    
    stream->vtable->octfile_rewind(stream);
    stream->read_buffer_mask = 0;
    stream->read_buffer = 0;
    assert(stream->write_buffer_mask == 0);
}

int
dlb_octfile_eof(dlb_octfile *stream)
{
    return stream->vtable->octfile_eof(stream);
}


