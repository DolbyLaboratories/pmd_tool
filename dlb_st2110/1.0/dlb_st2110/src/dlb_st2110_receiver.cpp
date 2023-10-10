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

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <string>
#include <cstdio>
#include <memory>
#include <thread>
#include <stdexcept>
#include <array>
#include <fstream>
#include <streambuf>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <linux/net_tstamp.h>
#include <poll.h>
#include <linux/errqueue.h>

#include "dlb_st2110_receiver.h"
#include "am824_framer.h"
#include "dlb_st2110_hardware.h"
#include "audio_buffer.h"

using namespace std;

/************************* Constants ***************************/


#define STRIDES_PER_CHUNK 10
#define CHUNKS_PER_BLOCK 10

// As per AES67
#define MAX_RTP_PAYLOAD 1440

// Time Delay ahead to schedule packets
#define PACKET_SCHED_DELAY_MS 1000

const float minLatency = 0.001; //1ms
const float maxLatency = 1.0;   //1 sec

const unsigned short threshold = 304;

/*************************Globals ***************************/

extern AudioBuffers *audioBuffer;

/************************** Helper Functions *******************/


/************************* Methods ***************************/

void ST2110Receiver::AudioStreamThread(void)
{
	unsigned char *headerPtr;
	rmax_status_t rmaxStatus;
	MClock::Duration packetTime;
	MClock::TimePoint timeNow;
    MClock::Duration packetSchedDelay;
    unsigned int const reblockingBufSize = blockSizeBytes * 2; // * overall size, 2 because pin/pong buffer
    unsigned char reblockingBuf[reblockingBufSize];
    unsigned int reblockingBufBytes; // Number of bytes in buffer (full/empty)
    unsigned int reblockingBufIndex;
    unsigned char *dataBytePtr;
    unsigned char *activeCallBackBlockPtr;
    MClock::Duration chunkTime;
	MClock::TimePoint wakeTime;
	bool firstPacket = true;
	unsigned int numPacketsChunk = numPacketsLatency;
	uint32_t timeStamp;
	int32_t timeStampCorrection;

	CLOG(INFO, RECEIVE_LOG) << "Number of Packets in Chunk: " << numPacketsChunk;
    packetTime.setMicroseconds(packetTimeMs * 1000);

	MClock::TimePoint getNextChunkEntry;
	MClock::TimePoint getNextChunkExit;
    chunkTime.setMicroseconds(packetTimeMs * 1000 * numPacketsChunk);
	CLOG(INFO, RECEIVE_LOG) << "Chunk Time: " << chunkTime;

	// Set the timeout to be our receive latency plus an extra packet time
	// This means if we run out of packets, the packets that did arrive
	// will be sent up but with a 1 packet time delay
	int rmaxTimeoutUs = numPacketsChunk * packetTimeMs * 1000.0;
	struct rmax_in_completion rmaxRxComp;
	// Not sure if the below should be set by rmax or application
	// This is a test to see if rmax rejects this or overwrites the pointer with its own
	rmaxRxComp.packet_info_arr = nullptr;

	reblockingBufIndex = 0;
	reblockingBufBytes = 0;
	activeCallBackBlockPtr = &reblockingBuf[0];

	rmaxStatus = rmax_in_attach_flow(rmaxStreamId, &rmaxInFlowAttr);
	ST2110Hardware::GetErrorMsg("rmax_in_attach_flow", rmaxStatus);

	while(streamActive)
	{
		getNextChunkEntry.SetNow();

		rmaxStatus = rmax_in_get_next_chunk(rmaxStreamId, numPacketsChunk, numPacketsChunk, rmaxTimeoutUs, 0, &rmaxRxComp);
		getNextChunkExit.SetNow();
		ST2110Hardware::GetErrorMsg("rmax_in_get_next_chunk", rmaxStatus);

		dataBytePtr = (unsigned char *)rmaxRxComp.data_ptr;
		headerPtr = (unsigned char *)rmaxRxComp.hdr_ptr;

		for (unsigned int i = 0 ; i < rmaxRxComp.chunk_size ; i++)
		{
			reblockingBufBytes += DeformatPacketData(&dataBytePtr[i * RTP_PAYLOAD_SIZE], reblockingBufIndex, reblockingBuf, reblockingBufSize, rmaxRxComp.packet_info_arr[i].data_size);
			// Ping Pong output buffers that are alternately filled up by DeformatPacketData
			if (reblockingBufBytes >= blockSizeBytes)
			{
				// get timestamp from RTP header
				timeStamp = be32toh(*(uint32_t *)(&headerPtr[(i * RTP_HEADER_SIZE) + 4]));
				//CLOG(INFO, RECEIVE_LOG) << "Raw TS: " << timeStamp;
				//correct timestamp according to offset position so that it refers the the first sample
				//of the next block when it is returned
				timeStampCorrection = round(((int32_t)rmaxRxComp.packet_info_arr[i].data_size - (int32_t)reblockingBufBytes) / (float)(streamInfo.audio.numChannels * streamInfo.audio.payloadBytesPerSample));
				timeStamp += timeStampCorrection; 
				//CLOG(INFO, RECEIVE_LOG) << "Corrected TS: " << rmaxRxComp.packet_info_arr[i].data_size  << " " << reblockingBufBytes << " " << streamInfo.audio.numChannels << " " << streamInfo.audio.payloadBytesPerSample << " " << timeStampCorrection << " " << timeStamp;
				streamActive = (callBackInfo.callBack)(callBackInfo.data, activeCallBackBlockPtr, blockSizeBytes, timeStamp);
				reblockingBufBytes -= blockSizeBytes;
				if (activeCallBackBlockPtr == reblockingBuf)
				{
					activeCallBackBlockPtr = &reblockingBuf[blockSizeBytes];
				}
				else
				{
					activeCallBackBlockPtr = &reblockingBuf[0];
				}
			}
		}

		if (streamActive)
		{
			if (firstPacket || (rmaxRxComp.chunk_size < numPacketsChunk))
			{
				if (!firstPacket)
				{
					CLOG(WARNING, RECEIVE_LOG) << "Got " << rmaxRxComp.chunk_size << " packets, expected " << numPacketsChunk << ". Resync...";
				}
				wakeTime = getNextChunkExit + chunkTime;
				firstPacket = false;
			}
			else
			{
				wakeTime = wakeTime + chunkTime;
			}

			wakeTime.SleepUntil();
		}
	}
	CLOG(INFO, RECEIVE_LOG) << "Shutting Down...";

	rmaxStatus = rmax_in_get_next_chunk(rmaxStreamId, 0, 0 , 0, 0, &rmaxRxComp);
	ST2110Hardware::GetErrorMsg("rmax_in_get_next_chunk", rmaxStatus);

	rmaxStatus = rmax_in_detach_flow(rmaxStreamId, &rmaxInFlowAttr);
	ST2110Hardware::GetErrorMsg("rmax_in_detach_flow", rmaxStatus);
	CLOG(INFO, RECEIVE_LOG) << "Detached Flow: " << rmaxStreamId;

	rmax_in_destroy_stream(rmaxStreamId);
	CLOG(INFO, RECEIVE_LOG) << "Destroyed Stream: " << rmaxStreamId;
}


void ST2110Receiver::MetadataStreamThread(void)
{
	throw runtime_error("Reception of metadata streams not yet supported");
}

void ST2110Receiver::Init(AoipSystem &newSystem, StreamInfo &newStreamInfo, ST2110ReceiverCallBackInfo &newCallBackInfo)
{
	rmax_status_t rmaxStatus;
	struct sockaddr_in localNicAddr;
    	struct rmax_in_buffer_attr rmaxInBufferAttr;
    	rmax_in_memblock dataMemBlock;
    	rmax_in_memblock hdrMemBlock;

	system = newSystem;
	streamInfo = newStreamInfo;
	callBackInfo = newCallBackInfo;
	streamThread = nullptr;
	streamActive = false;

	if (((callBackInfo.blockSize > 0) && (callBackInfo.blockSize < 64)) || (callBackInfo.blockSize > 65536))
	{
		throw runtime_error("Blocksize out of range (64-65536 samples)");
	}

	if ((callBackInfo.audioFormat == DLB_AOIP_AUDIO_FORMAT_32BIT_FLOAT) ||
		(callBackInfo.audioFormat == DLB_AOIP_AUDIO_FORMAT_8BIT_LPCM))
	{
		throw runtime_error("Audio Format not yet suppoted");
	}
	else
	{
		callBackBytesPerSample = GetAoipBytesPerSample(callBackInfo.audioFormat);
	}

	// If blocksize is not selected by user then use 1 packet per block for efficiency
	if (callBackInfo.blockSize == 0)
	{
		callBackInfo.blockSize = streamInfo.audio.samplesPerPacket;
	}

	// only supporting audio at the moment
	blockSizeBytes = callBackInfo.blockSize * streamInfo.audio.numChannels * callBackBytesPerSample;

	// Check latency is within limits
	if ((streamInfo.latency < minLatency) ||
		(streamInfo.latency > maxLatency))
	{
		throw runtime_error(string("Receive latency is out of range (") + to_string(minLatency) + string("-") + to_string(maxLatency) + string(")"));
	}

	// Dimension Chunks and Blocks according to stream Type
    	localNicAddr.sin_family = AF_INET;
    	localNicAddr.sin_addr.s_addr = GetNetIpInt(system.mediaInterface.ipStr);

   	if ((streamInfo.streamType == AES67) || (streamInfo.streamType == AM824))
   	{
   		// Ceil ensures it will be zero and tends to a larger latency than requested for safety
   		// This could be changed to round with a zero check
   		numPacketsLatency = ceil((streamInfo.latency * streamInfo.samplingFrequency) / (float)streamInfo.audio.samplesPerPacket );
   		packetTimeMs = (streamInfo.audio.samplesPerPacket * 1000) / (float)streamInfo.samplingFrequency;
   	}
   	else
   	{
   		numPacketsLatency = ceil((streamInfo.latency * 1000.0) / (float)streamInfo.metadata.packetTimeMs);
   		packetTimeMs = streamInfo.metadata.packetTimeMs;
   	}

   	//round numPacketsLatency to nearest power of 2
	CLOG(INFO, RECEIVE_LOG) << "Latency in Packets: " << numPacketsLatency;
   	numPacketsLatency = pow(2, round(log2(numPacketsLatency)));

	// Need to have more elements than packets required in the system
	// Otherwise we will have wraparound
	// We actually need a little over half this depending on how accurate the wakeup time is
    rmaxInBufferAttr.num_of_elements = numPacketsLatency * 2;
	CLOG(INFO, RECEIVE_LOG) << "Number of Elements: " << rmaxInBufferAttr.num_of_elements;

    dataMemBlock.ptr = nullptr; // let Rivermax allocate buffers
    dataMemBlock.min_size = 1;
    dataMemBlock.max_size = RTP_PAYLOAD_SIZE;
    dataMemBlock.stride_size = RTP_PAYLOAD_SIZE;

    hdrMemBlock.ptr = nullptr; // let Rivermax allocate buffers
    hdrMemBlock.min_size = RTP_HEADER_SIZE;
    hdrMemBlock.max_size = RTP_HEADER_SIZE;
    hdrMemBlock.stride_size = RTP_HEADER_SIZE;

    rmaxInBufferAttr.data = &dataMemBlock;
    rmaxInBufferAttr.hdr = &hdrMemBlock;
    rmaxInBufferAttr.attr_flags = RMAX_IN_BUFFER_ATTER_FLAG_NONE; //RMAX_IN_BUFFER_ATTER_STREAM_RTP_SEQN_PLACEMENT_ORDER;

    // Print out SDP as debug
	rmaxStatus = rmax_in_create_stream(RMAX_APP_PROTOCOL_PACKET, &localNicAddr,
                                       &rmaxInBufferAttr,
                                       RMAX_PACKET_TIMESTAMP_RAW_NANO,
    								   RMAX_IN_CREATE_STREAM_INFO_PER_PACKET,
                                       &rmaxStreamId);

	ST2110Hardware::GetErrorMsg("rmax_in_create_stream", rmaxStatus);

	// This memory clearing is essential and removing it can result in errors
	memset(&rmaxInFlowAttr, 0, sizeof(rmaxInFlowAttr));
	rmaxInFlowAttr.local_addr.sin_family = AF_INET;
	rmaxInFlowAttr.local_addr.sin_addr.s_addr = GetNetIpInt(streamInfo.dstIpStr);
	rmaxInFlowAttr.local_addr.sin_port = GetNetPort(streamInfo.port);
	rmaxInFlowAttr.remote_addr.sin_family = AF_INET;
	rmaxInFlowAttr.remote_addr.sin_addr.s_addr = GetNetIpInt(streamInfo.srcIpStr);

}

void ST2110Receiver::Start(void)
{
	streamActive = true;
	switch(streamInfo.streamType)
	{
	case AES67:
	case AM824:
		streamThread = make_shared<thread>(thread(&ST2110Receiver::AudioStreamThread, this));
		break;
	case SMPTE2110_41:
		streamThread = make_shared<thread>(thread(&ST2110Receiver::MetadataStreamThread, this));
		break;
	default:
		throw runtime_error("Unknown Stream Type");
	}
}


/*
        0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P|X|  CC   |M|     PT      |       sequence number         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                           timestamp                           |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |           synchronization source (SSRC) identifier            |
   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
   |            contributing source (CSRC) identifiers             |
   |                             ....                              |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/


/*
0                   1                   2                   3 
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                  Data Item Type           |M| Data Item Length|
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                     Segment Data Offset                       |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                           Segment Contents                    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                            *      *      *
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                           Segment Contents                    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                           Segment Contents                    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
/*
unsigned int ST2110Receiver::DeFragSMPTE2110_41(uint8_t *const buff, unsigned int byteCount, unsigned int byteOffset, bool lastPacket)
{
	unsigned int wordCount = ceil((float)byteCount / 4.0f);
	unsigned int wordOffset = ceil((float)byteOffset / 4.0f);

	return(SMPTE2110_41_HEADER_SIZE);
}
*/

unsigned int ST2110Receiver::DeformatPacketData(void *streamPacketBuf, unsigned int& index, void *outputBufBegin, unsigned int outputBufSize, int numBytes)
{
	uint8_t *pNew;
	uint8_t *pStream;
	uint32_t i,j;
	unsigned int numSamples;
	unsigned int minBytesPerSample;

	if (streamInfo.audio.payloadBytesPerSample < callBackBytesPerSample)
	{
		minBytesPerSample = streamInfo.audio.payloadBytesPerSample;
	}
	else
	{
		minBytesPerSample = callBackBytesPerSample;
	}

	switch(streamInfo.streamType)
	{
		case AES67:
			pNew = &((uint8_t *)outputBufBegin)[index];
			pStream = (uint8_t *)streamPacketBuf;
			numSamples = floor(numBytes / streamInfo.audio.payloadBytesPerSample);
			// Convert from little endian to big endian (network order) if required
			if (ST2110Hardware::IsLittleEndian())
			{
				for (i = 0 ; i < numSamples ; i++)
				{
					// Copy MSB bytes first
					for (j = 0 ; j < minBytesPerSample ; j++)
					{
						pNew[callBackBytesPerSample - j - 1] = pStream[j];
					}
					// complete with zeros if necessary
					for (; j < callBackBytesPerSample ; j++)
					{
						pNew[callBackBytesPerSample - j - 1] = 0;
					}
					pNew += callBackBytesPerSample;
					index += callBackBytesPerSample;
					if (index >= outputBufSize)
					{
						pNew = (uint8_t *)outputBufBegin;
						index = 0;
					}
					pStream += streamInfo.audio.payloadBytesPerSample;
				}
			return(callBackBytesPerSample * numSamples);
			}
			else
			{
				if (streamInfo.audio.payloadBytesPerSample == callBackBytesPerSample)
				{
					// Machine is big endian so samples are already in the right order
					if ((index + numBytes) > outputBufSize)
					{
						unsigned int bytesLeft = outputBufSize - index;
						memcpy(pNew, pStream, bytesLeft);
						memcpy(outputBufBegin, &pStream[bytesLeft], numBytes - bytesLeft);
					}
					else
					{
						memcpy(pNew, pStream, numBytes);
					}
				}
				else
				{
					throw runtime_error("Big Endian Support not implemented yet");
				}
			return(numBytes);
			}
			break;
		case AM824:
			// Simply strip off PCUV word and return 24bit word for now
			// No checking of CRC etc.
			// Step over the first PCUV byte
			pStream = (uint8_t *)streamPacketBuf + 1;
			pNew = &((uint8_t *)outputBufBegin)[index];
			numSamples = floor(numBytes / 4); // AM824 packet has 4 bytes per sample in stream
			if (ST2110Hardware::IsLittleEndian())
			{
				// start at MSB on output
				for (i = 0 ; i < numSamples ; i++)
				{
					for (j = 0 ; j < minBytesPerSample ; j++)
					{
						pNew[callBackBytesPerSample - j - 1] = pStream[j];
					}
					pNew += callBackBytesPerSample;
					index += callBackBytesPerSample;
					pStream += 4; // Step to next MSB in stream
					if (index >= outputBufSize)
					{
						pNew = ((uint8_t *)outputBufBegin);
						index = 0;
					}
				}
			}
			else
			{
				throw runtime_error("Big Endian not implemented yet");
			}
			return(numSamples * callBackBytesPerSample);
		break;
		default:
		throw runtime_error("Error: Unsupported Format");
	}
}
