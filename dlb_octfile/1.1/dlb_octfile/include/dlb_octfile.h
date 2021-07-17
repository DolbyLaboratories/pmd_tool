/**
 * @brief  Abstraction for working with octet-streams (when target might use
 *         non 8-bit chars)
 * @note   Confidential Information - Limited distribution to authorized persons
 *         only. This material is protected under international copyright laws
 *         as an unpublished work. Do not copy.
 *         Copyright (C) 2010 Dolby Laboratories Inc. All rights reserved.
 *         
 */

#ifndef dlb_octfile_H
#define dlb_octfile_H

/*
 * This defines a wrapper around the stdio FILE type which allows file 
 * operations to work on octets, rather than chars. On platforms where
 * CHAR_BIT is >8, the top bits in each char will be zero padded.
 */

#include <stddef.h>
#include <stdio.h>

/* Whence values to use in dlb_octfile.
 * At compile time, these are confirmed to match lower layers.
 */
#define DLB_OCTFILE_SEEK_SET    0
#define DLB_OCTFILE_SEEK_CUR    1
#define DLB_OCTFILE_SEEK_END    2
typedef struct dlb_octfile_vtable_s dlb_octfile_vtable;

typedef struct dlb_octfile_s
{
    FILE *file;
    
    void            *mem_base;      /**< Based address of the memory file. */
    size_t           mem_offset;    /**< Current offset into the memory file. */
    size_t           mem_size;      /**< Current size of the memory file. */
    size_t           mem_alloc_size;/**< Allocated size of the memory file. */
    unsigned char    mem_fixed;     /**< Boolean flag indicating the memory file is fixed size. */
    unsigned char    mem_eof;       /**< Boolean flag indicating we have hit the end-of-file. */
    /* Pointer to file operation function table */
    const dlb_octfile_vtable *vtable;
    /*
     * When we do a read which doesn't use up all of a char, we store
     * the last char in read_buffer, and set read_buffer_mask to be the 
     * bitmask over all the values which haven't been read yet.
     * If read_buffer_mask is 0, then read_buffer should not be used.
     * The relevant bits should always be right shifted, so that all
     * '1' bits in read_buffer_mask occur after all '0' bits.     */
    unsigned char read_buffer;
    unsigned char read_buffer_mask;
    
    /*
     * Similarly, when we do a write, if we can't put together a full
     * char, then we put the leftover bits into write_buffer and set
     * the relevant bits in write_buffer_mask to show which bits there
     * are valid. 
     * The values are always left shifted so all the '1' bits in
     * write_buffer_mask occur before all '0' bits.
     */
    unsigned char write_buffer;
    unsigned char write_buffer_mask;
} dlb_octfile;

/* Opaque struct to store a file position. */
typedef union
{
    fpos_t  fpos;
    size_t  mpos;
} dlb_octpos;

/* Set of function pointers to abstract operations for different stream types. */
typedef int
(*fn_octfile_close)
    (dlb_octfile *stream);

typedef int
(*fn_octfile_flush)
    (dlb_octfile *stream);

typedef size_t
(*fn_octfile_write)
    (const void *ptr, size_t size, size_t count, dlb_octfile *stream);

typedef size_t
(*fn_octfile_read)
    (void *ptr, size_t size, size_t count, dlb_octfile *stream);

typedef int
(*fn_octfile_seek)
    (dlb_octfile *stream, long offset, int whence);

typedef int
(*fn_octfile_setpos)
    (dlb_octfile *stream, const dlb_octpos *pos);

typedef long
(*fn_octfile_tell)
    (dlb_octfile *stream);

typedef int
(*fn_octfile_getpos)
    (dlb_octfile *stream, dlb_octpos *pos);

typedef void
(*fn_octfile_rewind)
    (dlb_octfile *stream);

typedef int
(*fn_octfile_eof)
    (dlb_octfile *stream);

struct dlb_octfile_vtable_s
{
    fn_octfile_close    octfile_close;
    fn_octfile_flush    octfile_flush;
    fn_octfile_write    octfile_write;
    fn_octfile_read     octfile_read;
    fn_octfile_seek     octfile_seek;
    fn_octfile_setpos   octfile_setpos;
    fn_octfile_tell     octfile_tell;
    fn_octfile_getpos   octfile_getpos;
    fn_octfile_rewind   octfile_rewind;
    fn_octfile_eof      octfile_eof;
};

/* These are all exactly analogous to the standard stdio FILE function */
int
dlb_octfile_close(dlb_octfile *stream);
/* This is roughly equivalent to fopen, except that the caller must supply
 * the storage for the dlb_octfile struct (whereas fopen supplies storage
 * for the FILE struct). This means we don't need to do any dynamic memory
 * allocation.
 */
dlb_octfile*
dlb_octfile_open
    (dlb_octfile*
    ,const char *filename
    ,const char *mode
    );

/* Open a memory-based file at the given address with a fixed size.
 * Similar to the disk-file open the caller must provide storage for
 * the dlb_octfile struct.
 */
dlb_octfile*
dlb_octfile_open_memory_fixed
    (dlb_octfile*
    ,size_t      size
    ,void       *memory
    ,const char *mode 
    );

/* Open a memory-based file at the given address with a variable size.
 * Similar to the disk-file open the caller must provide storage for
 * the dlb_octfile struct.
 */
dlb_octfile*
dlb_octfile_open_memory
    (dlb_octfile*
    ,const char *mode 
    );

const
void *
dlb_octfile_get_memory
    (dlb_octfile*
    );

int
dlb_octfile_flush(dlb_octfile *stream);

size_t
dlb_octfile_read
    (void           *ptr
    ,size_t          size
    ,size_t          nmemb
    ,dlb_octfile    *stream
    );

size_t
dlb_octfile_write
    (const void     *ptr
    ,size_t          size
    ,size_t          nmemb
    ,dlb_octfile    *stream
    );

int
dlb_octfile_seek
    (dlb_octfile *stream
    ,long         offset
    ,int          whence
    );

long
dlb_octfile_tell
    (dlb_octfile  *stream);

int
dlb_octfile_setpos
    (dlb_octfile  *stream
    ,const dlb_octpos   *pos
    );

int
dlb_octfile_getpos
    (dlb_octfile  *stream
    ,dlb_octpos     *pos
    );

void
dlb_octfile_rewind
    (dlb_octfile *stream);

int
dlb_octfile_eof
    (dlb_octfile *stream);

#endif
