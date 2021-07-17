/**
 * @note   This program is protected under international and United States
 *         copyright laws as an unpublished work.  This  program is confidential
 *         and proprietary to the copyright owners.  Reproduction or disclosure,
 *         in whole or in part, or the production of derivative works therefrom
 *         without the express permission of the copyright owners is prohibited.
 *         Copyright 2012 by Dolby Laboratories Inc. All rights reserved.
 */

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
