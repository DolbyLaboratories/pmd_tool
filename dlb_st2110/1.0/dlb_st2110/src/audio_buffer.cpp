/************************************************************************
 * dlb_st2110
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


#include "dlb_st2110_hardware.h"
#include "audio_buffer.h"

using namespace std;



void *AudioBuffers::GetNextInputBuffer()
{
	void *ptr;

	bufMutex.lock();
	unsigned int maxBufNumSamples = 0;

	for (unsigned int i = 0 ; i < numOutputStreams ; i++)
	{
		if (bufNumSamples.get()[i] > maxBufNumSamples)
		{
			maxBufNumSamples = bufNumSamples.get()[i];
		}
	}

	if (maxBufNumSamples == bufSize)
	{
		ptr = nullptr;
	}
	else
	{
		ptr = (void *)&buf.get()[inputBufferWriteIndex * bufNumInputBufferSizeSamples * bufNumChannels * bytesPerSample];
	}
	inputBufferWriteIndex++;
	if (inputBufferWriteIndex == bufNumInputBuffers)
	{
		inputBufferWriteIndex = 0;
	}
	bufMutex.unlock();
	return(ptr);
}

void AudioBuffers::CommitInputBuffer(uint32_t timestamp)
{
	unsigned int previousInputBufferWriteIndex;

	bufMutex.lock();
	unsigned int maxBufNumSamples = 0;

	for (unsigned int i = 0 ; i < numOutputStreams ; i++)
	{
		if (bufNumSamples.get()[i] > maxBufNumSamples)
		{
			maxBufNumSamples = bufNumSamples.get()[i];
		}
	}
	if (maxBufNumSamples == bufSize)
	{
		bufMutex.unlock();
		return;
	}

	for (unsigned int i = 0 ; i < numOutputStreams ; i++)
	{
		bufNumSamples.get()[i] += bufNumInputBufferSizeSamples * bufNumChannels;
	}

	if (inputBufferWriteIndex == 0)
	{
		previousInputBufferWriteIndex = bufNumInputBuffers - 1;
	}
	else
	{
		previousInputBufferWriteIndex = inputBufferWriteIndex - 1;
	}
	unsigned int timeStampIndex = previousInputBufferWriteIndex * bufNumInputBufferSizeSamples;
	uint32_t *pTimeStamp = &timeStampBuf.get()[timeStampIndex];
	for (unsigned int i = 0 ; i < bufNumInputBufferSizeSamples ; i++)
	{
		*pTimeStamp++ = timestamp++;
	}

	bufMutex.unlock();
}

// Returns the number of bytes read
unsigned int AudioBuffers::GetBuffer(unsigned int txStream, void *samples, unsigned int numBytes, uint32_t &timestamp)
{
	if (txStream >= numOutputStreams)
	{
		throw runtime_error("Invalid Tx Stream index");
	}

	if (bytesPerSample != 4)
	{
		// Want to use nativ data types for speed rather than byte transfers
		// This requires a generic byte transfer for fallback and specific ones for 16/32 bit for speed
		// Implemented only 32 bit version for now with this assert to catch any change
		throw runtime_error("Only 32bit support implemented for speed");
	}

	if (bufNumSamples.get()[txStream] == 0)
	{
		return(0);
	}

	bufMutex.lock();
	unsigned int numSamplesToWrite;
	unsigned int numFrames;
	unsigned int numSamplesToRead;
	unsigned int timeStampIndex = readIndicies.get()[txStream] / bufNumChannels;
	unsigned int numFrames1, numFrames2;
	unsigned int numChannels = bufferReaders[txStream].numChannels;
	unsigned int startChannel = bufferReaders[txStream].startChannel;
	unsigned int channelSkip = bufNumChannels - numChannels;
	unsigned int readIndex = readIndicies.get()[txStream];
	uint32_t *readPtr;
	uint32_t *writePtr;

	// Do timestamp first
	timestamp = timeStampBuf.get()[timeStampIndex];

	numSamplesToRead = (numBytes / (numChannels * bytesPerSample)) * bufNumChannels; 
	// Check to see if this is going to empty buffer
	if (numSamplesToRead > bufNumSamples.get()[txStream])
	{
		numSamplesToRead = bufNumSamples.get()[txStream];
	}

	numFrames = numSamplesToRead / bufNumChannels;
	numSamplesToWrite = numFrames * numChannels; // Note numSamples is total samples across all channels

	// Split read into buffer into 2 across boundary for 
	// simplicity and efficiency
	if ((readIndex + numSamplesToRead) >= bufSize)
	{
		numFrames1 = (bufSize - readIndex) / bufNumChannels;
		numFrames2 = numFrames - numFrames1;
	}
	else
	{
		numFrames1 = numSamplesToWrite / numChannels;
		numFrames2 = 0;
	}

	// Get start of frame for read
	readPtr = (uint32_t *)&buf.get()[readIndex * bytesPerSample];
	// Move to correct channel
	readPtr += startChannel;
	writePtr = (uint32_t *)samples;

	unsigned int j;
	for (unsigned int i = 0 ; i < numFrames1 ; i++)
	{
		for (j = 0 ; j < numChannels ; j++)
		{
			*writePtr++ = *readPtr++;
		}
		readPtr += channelSkip;
	}
	//memcpy(writePtr, readPtr, numSamples1 * bytesPerSample);
//	writePtr += numSamples1 * bytesPerSample;
	readIndicies.get()[txStream] += numFrames1 * bufNumChannels;
	readIndex = readIndicies.get()[txStream];

	if (readIndex >= bufSize)
	{
		readIndex = readIndicies.get()[txStream] = 0;
	}

	// For speed only
	if (numFrames2 > 0)
	{
		readPtr = (uint32_t *)&buf.get()[0];
		readPtr += startChannel;
		for (unsigned int i = 0 ; i < numFrames2 ; i++)
		{
			for (j = 0 ; j < numChannels ; j++)
			{
				*writePtr++ = *readPtr++;
			}
			readPtr += channelSkip;
		}
		//memcpy(writePtr, readPtr, numSamples2 * bytesPerSample);
		readIndicies.get()[txStream] += numFrames2 * bufNumChannels;
		readIndex = readIndicies.get()[txStream]; // debugging only
	}

	bufNumSamples.get()[txStream] -= numSamplesToRead;

	bufMutex.unlock();

	return(numSamplesToWrite * bytesPerSample);
}
