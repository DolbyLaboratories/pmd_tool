/************************************************************************
 * dlb_octfile
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

#include "dlb_octfile/include/dlb_octfile.h"

static
int
octfile_disk_close(dlb_octfile *stream)
{
    return fclose(stream->file);
}

static
int
octfile_disk_flush(dlb_octfile *stream)
{
    if (stream == NULL)
    {
        /* Flush all open output streams. */
        return fflush(NULL);
    }
    else
    {
        return fflush(stream->file);
    }
}

static
size_t
octfile_disk_write(const void *ptr, size_t size, size_t count, dlb_octfile *stream)
{
    return fwrite(ptr, size, count, stream->file);
}

static
size_t
octfile_disk_read(void *ptr, size_t size, size_t count, dlb_octfile *stream)
{
    return fread(ptr, size, count, stream->file);
}

static
int
octfile_disk_seek(dlb_octfile *stream, long offset, int whence)
{
    return fseek(stream->file, offset, whence);
}

static
int
octfile_disk_setpos(dlb_octfile *stream, const dlb_octpos *pos)
{
    return fsetpos(stream->file, &pos->fpos);
}

static
long
octfile_disk_tell(dlb_octfile *stream)
{
    return ftell(stream->file);
}

static
int
octfile_disk_getpos(dlb_octfile *stream, dlb_octpos *pos)
{
    return fgetpos(stream->file, &pos->fpos);
}

static
void
octfile_disk_rewind(dlb_octfile *stream)
{
    rewind(stream->file);
}

static
int
octfile_disk_eof(dlb_octfile *stream)
{
    return feof(stream->file);
}

const dlb_octfile_vtable dlb_octfile_disk_vtable = 
    {octfile_disk_close
    ,octfile_disk_flush
    ,octfile_disk_write
    ,octfile_disk_read
    ,octfile_disk_seek
    ,octfile_disk_setpos
    ,octfile_disk_tell
    ,octfile_disk_getpos
    ,octfile_disk_rewind
    ,octfile_disk_eof
    };
