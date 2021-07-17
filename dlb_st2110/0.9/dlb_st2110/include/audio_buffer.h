/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019-2020 by Dolby Laboratories,
 *                Copyright (C) 2019-2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef _AUDIO_BUFFER_H_
#define _AUDIO_BUFFER_H_


#include <memory>
#include <mutex>
#include <cmath>

#include "dlb_st2110.h"

inline
void ReadSamples24LittleEndian(int32_t *outputPtr, uint8_t *inputPtr, unsigned int numSamples)
{
	int32_t result32;
	for (unsigned int i = 0 ; i < numSamples ; i++)
	{
		result32 = *inputPtr++;
		result32 += *inputPtr++ << 8;
		result32 += *inputPtr++ << 16;
		*outputPtr++ = result32;
	}
}

inline
void ReadSamples24BigEndian(int32_t *outputPtr, uint8_t *inputPtr, unsigned int numSamples)
{
	int32_t result32;

	for (unsigned int i = 0 ; i < numSamples ; i++)
	{
		result32 = *inputPtr++ << 16;
		result32 += *inputPtr++ << 8;
		result32 += *inputPtr++;
		*outputPtr++ = result32;
	}
}

inline
void WriteSamples24LittleEndian(uint8_t *outputPtr, int32_t *inputPtr, unsigned int numSamples)
{
	for (unsigned int i = 0 ; i < numSamples ; i++)
	{
		*outputPtr++ = *inputPtr & 0xff;
		*outputPtr++ = (*inputPtr & 0xff00) >> 8;
		*outputPtr++ = (*inputPtr++ & 0xff0000) >> 16;
	}
}

inline
void WriteSamples24BigEndian(uint8_t *outputPtr, int32_t *inputPtr, unsigned int numSamples)
{
	for (unsigned int i = 0 ; i < numSamples ; i++)
	{
		*outputPtr++ = (*inputPtr & 0xff0000) >> 16;
		*outputPtr++ = (*inputPtr & 0xff00) >> 8;
		*outputPtr++ = *inputPtr++ & 0xff;
	}
}		



class AudioBuffers
{
public:
	struct BufferReader
	{
		BufferReader(unsigned int s, unsigned int n) : startChannel(s), numChannels(n) {} 
		unsigned int startChannel;
		unsigned int numChannels;
	};

private:
	std::unique_ptr<uint8_t[]> buf;
	std::unique_ptr<uint32_t[]> timeStampBuf;
	std::unique_ptr<unsigned int[]> readIndicies;
	std::unique_ptr<unsigned int[]> bufNumSamples;
	unsigned int inputBufferWriteIndex;
	unsigned int bufSize;
	std::mutex bufMutex;
	unsigned int bufNumChannels;
	unsigned int bufNumInputBuffers;
	unsigned int bufNumInputBufferSizeSamples;
	bool littleEndian;
	unsigned int bytesPerSample;
	unsigned int numOutputStreams;
	std::vector<BufferReader> bufferReaders;
	uint8_t *lastbufBytePtr;

public:
	AudioBuffers(unsigned int numInputBuffers, unsigned int inputBufferSizeSamples, std::vector<BufferReader> &newBufferReaders, unsigned int numChannels, AoipAudioFormat audioFormat)
	{
		bytesPerSample = GetAoipBytesPerSample(audioFormat);
		// Buffer of 32 bit samples
		buf = std::make_unique<uint8_t[]>(inputBufferSizeSamples * numInputBuffers * numChannels * bytesPerSample);
		lastbufBytePtr = &buf.get()[(inputBufferSizeSamples * numInputBuffers * numChannels * bytesPerSample) - 1];
		// timestamp represents RTP timestamp of 0th location in the array
		// This is updated when the write pointer wraps
		timeStampBuf = std::make_unique<uint32_t[]>(inputBufferSizeSamples * numInputBuffers);
		bufferReaders = newBufferReaders;
		numOutputStreams = bufferReaders.size();
		readIndicies = std::make_unique<unsigned int[]>(numOutputStreams);
		bufNumSamples = std::make_unique<unsigned int[]>(numOutputStreams);
		bufSize = inputBufferSizeSamples * numInputBuffers * numChannels;
		bufNumInputBuffers = numInputBuffers;
		bufNumInputBufferSizeSamples = inputBufferSizeSamples;
		inputBufferWriteIndex = 0;
		for (unsigned int i = 0 ; i < numOutputStreams ; i++)
		{
			readIndicies.get()[i] = 0;
			bufNumSamples.get()[i] = 0;
		}		
		bufNumChannels = numChannels;
		littleEndian = ST2110Hardware::IsLittleEndian();
	}

	// Explicitly stopping copying as contains mutex which cannot copy
	AudioBuffers(AudioBuffers& copy) = delete;

	void CheckSane(void)
	{
		unsigned int txStream = 0;
		unsigned int timeStampIndex = readIndicies.get()[txStream] / bufNumChannels;


		//if we don't have at least 2 samples we can't check
		if (bufNumSamples.get()[txStream] < 2)
		{
			return;
		}

		bufMutex.lock();
		// Check timestamp sanity
		uint32_t ts1 = timeStampBuf.get()[timeStampIndex];
		unsigned int tsi2 = timeStampIndex + 1;
		if (tsi2 == bufSize)
		{
			tsi2 = 0;
		}
		uint32_t ts2 = timeStampBuf.get()[tsi2];
		if (ts2 != (ts1 + 1))
		{
			std::cout << "ts1 :" << ts1 << std::endl;
			std::cout << "ts2 :" << ts2 << std::endl;
			std::cout.flush();
			throw std::runtime_error("Timestamps not sane");
		}
		else
		{
			std::cout << "Sane ts1 :" << ts1 << "   ts2 :" << ts2 << std::endl;			
			std::cout.flush();
		}

		bufMutex.unlock();

	}

	unsigned int GetSize(void)
	{
		unsigned int samples = 0;
		bufMutex.lock();
		for (unsigned int i = 0 ; i < numOutputStreams ; i++)
		{
			if (bufNumSamples.get()[i] > samples)
			{
				samples = bufNumSamples.get()[i];
			}
		}
		bufMutex.unlock();
		return(samples);
	}

	float GetPercentFull(void)
	{
		return(((float)GetSize() / (float) bufSize) * 100.0);
	}

	void *GetNextInputBuffer();
	void CommitInputBuffer(uint32_t timestamp);
	unsigned int GetBuffer(unsigned int txStream, void *samples, unsigned int numBytes, uint32_t &timestamp);

};

#endif // _AUDIO_BUFFER_H_
