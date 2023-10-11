#ifndef PA_CALLBACK_H
#define PA_CALLBACK_H

/*
 * $Id$
 * PortAudio Portable Real-Time Audio Library
 * CALLBACK-specific extensions
 *
 * Copyright (c) 1999-2000 Ross Bencina and Phil Burk
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * The text above constitutes the entire PortAudio license; however, 
 * the PortAudio community also makes the following non-binding requests:
 *
 * Any person wishing to distribute modifications to the Software is
 * requested to send the modifications to the original developer so that
 * they can be incorporated into the canonical version. It is also 
 * requested that these non-binding requests be included along with the 
 * license above.
 */

/** @file
 *  @ingroup public_header
 *  @brief CALLBACK-specific PortAudio API extension header file.
 */

#include "portaudio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*PaCallbackCallback)(void* arg, void* buffer, size_t size);

typedef struct PaCallbackStreamInfo
{
    unsigned long size;
    PaHostApiTypeId hostApiType;
    unsigned long version;

    PaCallbackCallback callback;
    void *arg;

    const char *deviceString;
}
PaCallbackStreamInfo;

/** Initialize host API specific structure, call this before setting relevant attributes. */
void PaCallback_InitializeStreamInfo( PaCallbackStreamInfo *info );

/** Instruct whether to enable real-time priority when starting the audio thread.
 *
 * If this is turned on by the stream is started, the audio callback thread will be created
 * with the FIFO scheduling policy, which is suitable for realtime operation.
 **/
void PaCallback_EnableRealtimeScheduling( PaStream *s, int enable );

#if 0
void PaCallback_EnableWatchdog( PaStream *s, int enable );
#endif

/** Get the CALLBACK-lib card index of this stream's input device. */
PaError PaCallback_GetStreamInputCard( PaStream *s, int *card );

/** Get the CALLBACK-lib card index of this stream's output device. */
PaError PaCallback_GetStreamOutputCard( PaStream *s, int *card );

/** Set the number of periods (buffer fragments) to configure devices with.
 *
 * By default the number of periods is 4, this is the lowest number of periods that works well on
 * the author's soundcard.
 * @param numPeriods The number of periods.
 */
PaError PaCallback_SetNumPeriods( int numPeriods );

/** Set the maximum number of times to retry opening busy device (sleeping for a
 * short interval inbetween).
 */
PaError PaCallback_SetRetriesBusy( int retries );

/** Set the path and name of CALLBACK library file if PortAudio is configured to load it dynamically (see
 *  PA_CALLBACK_DYNAMIC). This setting will overwrite the default name set by PA_CALLBACK_PATHNAME define.
 * @param pathName Full path with filename. Only filename can be used, but dlopen() will lookup default
 *                 searchable directories (/usr/lib;/usr/local/lib) then.
 */
void PaCallback_SetLibraryPathName( const char *pathName );

#ifdef __cplusplus
}
#endif

#endif
