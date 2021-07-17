/**
 * @note   This program is protected under international and United States
 *         copyright laws as an unpublished work.  This  program is confidential
 *         and proprietary to the copyright owners.  Reproduction or disclosure,
 *         in whole or in part, or the production of derivative works therefrom
 *         without the express permission of the copyright owners is prohibited.
 *         Copyright 2012 by Dolby Laboratories Inc. All rights reserved.
 */

#include "dlb_octfile/include/dlb_octfile.h"
#include <stdlib.h>
#include <string.h>

static
int
octfile_memory_close(dlb_octfile *stream)
{
    if (0 == stream->mem_fixed && NULL != stream->mem_base)
    {
        free(stream->mem_base);
    }
    stream->mem_base = NULL;
    stream->mem_offset = 0;
    stream->mem_size = 0;
    stream->mem_alloc_size = 0;
    return 0;
}

static
int
octfile_memory_flush(dlb_octfile *stream)
{
	(void)stream;
    return 0;
}

static
size_t
octfile_memory_write(const void *ptr, size_t size, size_t count, dlb_octfile *stream)
{
    size_t bytes_free = stream->mem_alloc_size - stream->mem_offset;
    size_t count_free = bytes_free / size;
    size_t bytes_to_write;
    unsigned char *mem;

    if (count > count_free && !stream->mem_fixed)
    {
        size_t required_size = (count - count_free) * size + stream->mem_alloc_size;
        size_t new_size = stream->mem_alloc_size;
        void *new_mem;
        while (new_size < required_size)
        {
            new_size *= 2;
        }
        new_mem = realloc(stream->mem_base, new_size);
        if (NULL != new_mem)
        {
            stream->mem_base = new_mem;
            stream->mem_alloc_size = new_size;
            bytes_free = stream->mem_alloc_size - stream->mem_offset;
            count_free = bytes_free / size;
        }
    }
    count = count > count_free ? count_free : count;
    bytes_to_write = size * count;

    mem = (unsigned char *)stream->mem_base + stream->mem_offset;
    memcpy(mem, ptr, bytes_to_write);
    stream->mem_offset += bytes_to_write;
    if (stream->mem_offset > stream->mem_size)
    {
        stream->mem_size += bytes_to_write;
    }

    return count;
}

static
size_t
octfile_memory_read(void *ptr, size_t size, size_t count, dlb_octfile *stream)
{
    size_t bytes_avail = stream->mem_size - stream->mem_offset;
    size_t count_avail = bytes_avail / size;
    size_t bytes_to_read;
    unsigned char *mem = (unsigned char *)stream->mem_base + stream->mem_offset;

    count = count > count_avail ? count_avail : count;
    bytes_to_read = size * count;

    memcpy(ptr, mem, bytes_to_read);
    stream->mem_offset += bytes_to_read;
    
    if (stream->mem_offset >= stream->mem_size)
    {
        stream->mem_eof = 1;
    }

    return count;
}

static
int
octfile_memory_seek(dlb_octfile *stream, long offset, int whence)
{
    if (SEEK_CUR == whence)
    {
        offset = stream->mem_offset + offset;
    }
    else if (SEEK_END == whence)
    {
        offset = stream->mem_size + offset;
    }

    if (offset > (long)stream->mem_size)
    {
        return -1;
    }
    else if (offset < 0)
    {
        return -1;
    }
    else
    {
        stream->mem_offset = offset;
        stream->mem_eof = 0;
        return 0;
    }
}

static
int
octfile_memory_setpos(dlb_octfile *stream, const dlb_octpos *pos)
{
    if (pos->mpos >= stream->mem_size)
    {
        return -1;
    }
    else
    {
        stream->mem_offset = pos->mpos;
        stream->mem_eof = 0;
        return 0;
    }
}

static
long
octfile_memory_tell(dlb_octfile *stream)
{
    return (long)stream->mem_offset;
}

static
int
octfile_memory_getpos(dlb_octfile *stream, dlb_octpos *pos)
{
    pos->mpos = stream->mem_offset;
    return 0;
}

static
void
octfile_memory_rewind(dlb_octfile *stream)
{
    octfile_memory_seek(stream, 0L, SEEK_SET);
}

static
int
octfile_memory_eof(dlb_octfile *stream)
{
    return stream->mem_eof;
}

const dlb_octfile_vtable dlb_octfile_memory_vtable = 
    {octfile_memory_close
    ,octfile_memory_flush
    ,octfile_memory_write
    ,octfile_memory_read
    ,octfile_memory_seek
    ,octfile_memory_setpos
    ,octfile_memory_tell
    ,octfile_memory_getpos
    ,octfile_memory_rewind
    ,octfile_memory_eof
    };
