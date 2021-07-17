/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2020 by Dolby Laboratories,
 *                Copyright (C) 2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/


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