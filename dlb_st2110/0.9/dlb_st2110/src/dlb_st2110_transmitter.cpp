/************************************************************************
 * dlb_st2110
 * Copyright (c) 2021, Dolby Laboratories Inc.
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

#include "dlb_st2110_transmitter.h"
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

const char *channelLabels[MAX_CHANNELS] = {"L","R", "C", "LFE", "Ls", "Rs", "Lb", "Rb", "Tfl", "Tfr", "Tsl", "Tsr", "Tbl", "Tbr", "Tfc", "Tbc"};

const float minLatency = 0.001; //1ms
const float maxLatency = 1.0;   //1 sec

/************************* Globals ***************************/

extern AudioBuffers *audioBuffer;

/************************** Helper Functions *******************/


/************************* Methods ***************************/

void ST2110Transmitter::AudioStreamThread(void)
{
	unsigned char *dataPtr;
	unsigned char *headerPtr;
	unsigned int i, packetCount = 0;
	uint64_t rmaxTime = 0;
	rmax_status_t rmaxStatus;
	rmax_commit_flags_t rmaxCommitFlags{};
	MClock::Duration packetTime;
	MClock::TimePoint timeNow;
    MClock::Duration packetSchedDelay;
	MClock::Duration latencyDuration;
	packetTime.setMicroseconds(packetTimeMs * 1000);
    latencyDuration.setSeconds(streamInfo.latency);
    bool firstPacket = true;

	MClock::TimePoint getNextChunkEntry;
	MClock::TimePoint getNextChunkExit;
	unsigned int getNextChunkLoop;
	MClock::TimePoint callbackEntry;
	MClock::TimePoint callbackExit;
	unsigned int callBackBytesPerSample = (unsigned int) callBackInfo.audioFormat; // Note this break when support for floating point added

    unsigned int callBackBlockSizeBytes = callBackInfo.blockSize * streamInfo.audio.numChannels * callBackBytesPerSample;
	unsigned int numCallBackBlocks = ceil(callBackPacketSizeBytes / callBackBlockSizeBytes);
	uint32_t timeStamp;
	bool haveTimeStamp;

	if (numCallBackBlocks < 2)
	{
		numCallBackBlocks = 2;
	}
    unsigned char callBackBuffer[callBackBlockSizeBytes * numCallBackBlocks];
    unsigned int callBackBufferReadIndex = 0;
    unsigned int callBackBufferWriteIndex = 0;
    unsigned int callBackBufferSizeBytes = 0;

    CLOG(INFO, TRANS_LOG) << "Starting main loop";
    CLOG(INFO, TRANS_LOG) << "No. callback blocks: " << numCallBackBlocks;
    CLOG(INFO, TRANS_LOG) << "Callback block size: " << callBackBlockSizeBytes;
    CLOG(INFO, TRANS_LOG) << "Strides Per Chunk: " << stridesPerChunk;

	while(streamActive)
	{
		timeNow.SetNow();

		getNextChunkLoop = 0;
		do
		{
			rmaxStatus = rmax_out_get_next_chunk(rmxStreamId, (void **)&dataPtr, (void **)&headerPtr);
			getNextChunkLoop++;
			if ((getNextChunkLoop % 1000) == 0)
			{
				CLOG(WARNING, TRANS_LOG) << "Tried 1000 times to get next chunk";
			}
		}
		while(rmaxStatus == RMAX_ERR_NO_FREE_CHUNK);


		ST2110Hardware::RmaxError("rmax_out_get_next_chunk", rmaxStatus);

		for (i = 0 ; i < stridesPerChunk ; i++)
		{

			// Call callback to get payload
			// Check to see if we are in generator mode or not
			if (callBackInfo.callBack != nullptr)
			{
				// Have we got enough data if not then get more

				while (callBackBufferSizeBytes < callBackPacketSizeBytes)
				{
					// If the call back returns FALSE, the stream ends
					streamActive = (*callBackInfo.callBack)(callBackInfo.data, &callBackBuffer[callBackBufferWriteIndex], callBackBlockSizeBytes, haveTimeStamp, timeStamp);
					callBackBufferSizeBytes += callBackBlockSizeBytes;
					callBackBufferWriteIndex += callBackBlockSizeBytes;
					if (callBackBufferWriteIndex == (callBackBlockSizeBytes * numCallBackBlocks))
					{
						callBackBufferWriteIndex = 0;
					}
				}

				// This converts from the callback format defined in callBackInfo.audioFormat to that specified in streamInfo
				callBackBufferSizeBytes -= FormatPacketData(callBackBufferReadIndex, callBackBuffer, callBackBlockSizeBytes * numCallBackBlocks, dataPtr);

			}
			else
			{
				GeneratePayload(dataPtr, streamPacketSizeBytes);
			}

			timeNow.SetNow();
			if (firstPacket)
			{
				// First packet
				// Get time now
				lastPacketSchedTime = timeNow;
				if (haveTimeStamp)
				{
					lastPacketTxTime.SetRTPTimeStamp48kHz(timeStamp);
					packetSchedDelay = lastPacketTxTime - timeNow;
					CLOG(INFO, TRANS_LOG) << "tx timestamp: " << timeStamp;
					CLOG(INFO, TRANS_LOG) << "Timestamp now: " << timeNow.GetRTPTimeStamp48kHz();
					CLOG(INFO, TRANS_LOG) << "tx timestamp cross-check: " << lastPacketTxTime.GetRTPTimeStamp48kHz();
					CLOG(INFO, TRANS_LOG) << "Scheduled a packet for tx at " << lastPacketTxTime << ", time now: " << timeNow;
					CLOG(INFO, TRANS_LOG) << "Packet scheduling delay: " << packetSchedDelay;
					if (packetSchedDelay > latencyDuration)
					{
						// The timestamp we have requires too large a latency for the buffering that we have setup
						// Must be limited to avoid running out of chunks
						CLOG(WARNING, TRANS_LOG) << "Warning: Timestamp requires a latency of " << packetSchedDelay;
						CLOG(WARNING, TRANS_LOG) << "This is larger that the maximum of " << latencyDuration;
						CLOG(WARNING, TRANS_LOG) << "Warning: Ignoring Timestamp";
						packetSchedDelay = latencyDuration;
						lastPacketTxTime = lastPacketSchedTime + packetSchedDelay;				
					}
				}
				else
				{
					packetSchedDelay = latencyDuration;
					lastPacketTxTime = lastPacketSchedTime + packetSchedDelay;					
				}
				// Calculate transmit time but use TAI timebase
				// advance transmit times by delay
				rmaxTime = lastPacketTxTime.GetNanoSeconds();
				firstPacket = false;
			}

			// Check to see if deadline in the past
			if (timeNow > lastPacketTxTime)
			{
				// At least point we are too late to transmit. Something has delayed the processor so we have to
				// reinitialize the running timers
				lastPacketSchedTime = timeNow;
				lastPacketTxTime = lastPacketSchedTime + packetSchedDelay;
				rmaxTime = lastPacketTxTime.GetNanoSeconds();
				CLOG(WARNING, TRANS_LOG) << "Warning: Audio Stream Resync";
			}

			GetRTPHeader(headerPtr);
		
			dataPtr += streamPacketSizeBytes;
			lastPacketSchedTime = lastPacketSchedTime + packetTime;
			lastPacketTxTime = lastPacketTxTime + packetTime;
			packetCount++;
		}

		rmaxStatus = rmax_out_commit(rmxStreamId, rmaxTime, rmaxCommitFlags);
		ST2110Hardware::RmaxError("rmax_out_commit", rmaxStatus);
		// Time based is maintained by rivermax unless there is a resync
		rmaxTime = 0;
		// Don't bother scheduling during shutdown, just exit
		if (streamActive)
		{
			lastPacketSchedTime.SleepUntil();
		}

	}
	RiverMaxDestroy();
}


void ST2110Transmitter::MetadataStreamThread(void)
{
	unsigned char *dataPtr;
	unsigned char *headerPtr;
	unsigned char *chunkDataPtr;
	unsigned char *chunkHeaderPtr;
	unsigned int i;
	uint64_t rmaxTime;
	rmax_status_t rmaxStatus;
	rmax_commit_flags_t rmaxCommitFlags{};
	MClock::Duration packetTime;
	MClock::TimePoint timeNow;
    MClock::Duration packetSchedDelay;
    // Define buf with default fragmentation size and alignment
    ProtoBuf metadataProtoBuf(SMPTE2110_41_PAYLOAD_SIZE, sizeof(uint32_t));
    unsigned char headerBuf[MAX_HEADER_SIZE];
    unsigned int headerSize;
    unsigned char *metadataFrame = new unsigned char [4 * (unsigned int)ceil((float)streamInfo.metadata.maxPayloadSizeBytes / 4.0)];
    unsigned int stridesToBeTXed = 0;
    unsigned int payloadSizeBytes = 0;
    unsigned int fragmentSize;
    unsigned int sumFragmentedSize;
    uint16_t *rmaxDataSizes;
    unsigned int thisPayloadBytesLeft = 0;
    MClock::TimePoint getNextChunkEntry;
    MClock::TimePoint getNextChunkExit;
    MClock::TimePoint callBackEntry;
    uint32_t timestamp;
    bool haveTimeStamp;

    packetTime.setMicroseconds(packetTimeMs * 1000);
    packetSchedDelay.setSeconds(streamInfo.latency);

	timeNow.SetNow();
	lastPacketSchedTime = timeNow;
	lastPacketTxTime = lastPacketSchedTime + packetSchedDelay;

	while(streamActive)
	{
		payloadSizeBytes = streamInfo.metadata.maxPayloadSizeBytes;
		// Get a single metadata message
		callBackEntry.SetNow();
		if (callBackInfo.callBack != nullptr)
		{
			streamActive = (*callBackInfo.callBack)(callBackInfo.data, metadataFrame, payloadSizeBytes, haveTimeStamp, timestamp);
		}
		else
		{
			GeneratePayload(metadataFrame, payloadSizeBytes);
		}
		// Round up to 32 bits
		metadataProtoBuf.Init();
		metadataProtoBuf.AddPayload(metadataFrame, payloadSizeBytes);
		// calculate the number of strides we need to send this metadata frame
		stridesToBeTXed = metadataProtoBuf.GetNumFragments();

		if (stridesToBeTXed > 0)
		{
			//getNextChunkEntry.SetNow();
			do
			{
				rmaxStatus = rmax_out_get_next_chunk_dynamic(rmxStreamId, (void **)&chunkDataPtr, (void **)&chunkHeaderPtr, stridesToBeTXed, &rmaxDataSizes, nullptr);
			}
			while(rmaxStatus == RMAX_ERR_NO_FREE_CHUNK);
			dataPtr = chunkDataPtr;
			headerPtr = chunkHeaderPtr;
			//getNextChunkExit.SetNow();
			ST2110Hardware::RmaxError("rmax_out_get_next_chunk", rmaxStatus);
			//freeStrides = STRIDES_PER_CHUNK;
			for (i = 0 ; i < stridesToBeTXed ; i++)
			{
				//Build packet

				if (i == 0)
				{
					metadataProtoBuf.Fragment(SADM_PAYLOAD_SIZE);
					headerSize = GetSMPTE2116Header(headerBuf);
					metadataProtoBuf.AddHeader(headerBuf, headerSize);
				}
				else
				{
					metadataProtoBuf.Fragment();
				}
				fragmentSize = metadataProtoBuf.GetFragmentSize();
				// The ProtoBuf class count total fragment size but for the offset we don't want to include
				// the -41 header, hence it has to be removed for every packet here
				sumFragmentedSize = metadataProtoBuf.GetFragmentedSum() - (i * SMPTE2110_41_HEADER_SIZE);
				//metadataProtoBuf.HexDumpFragment(32);

				headerSize = GetSMPTE2110_41Header(headerBuf, fragmentSize, sumFragmentedSize, (i == (stridesToBeTXed - 1)));
				metadataProtoBuf.AddHeader(headerBuf, headerSize);

				GetRTPHeader(headerPtr);
				// Determine current payload size
				//metadataProtoBuf.HexDumpFragment(32);
				fragmentSize = metadataProtoBuf.GetFragment(dataPtr);
				// The following line inserts a 16 'gap' between output buffers. This was added after outgoing packets with dynamic chunks
				// were found to have the first two bytes missing. This is a suspected bug in Rivermax and this solution has been found
				// through experimentation
				dataPtr += fragmentSize + 2;					
				rmaxDataSizes[i] = fragmentSize;
			}
			if (metadataProtoBuf.GetPayloadRemainingSize())
			{
				throw runtime_error(string("Metadata payload left over after transmission: ") + to_string(thisPayloadBytesLeft) + string(" Bytes"));
			}
		}
		timeNow.SetNow();
		// Check to see if deadline in the past
		if (timeNow > lastPacketTxTime)
		{
			// At least point we are too late to transmit. Something has delayed the processor so we have to
			// reinitialize the running timers
			lastPacketSchedTime = timeNow;
			lastPacketTxTime = lastPacketSchedTime + packetSchedDelay;
			CLOG(WARNING, TRANS_LOG) << "Warning: Metadata Stream Resync";
		}

		rmaxTime = lastPacketTxTime.GetNanoSeconds();
		lastPacketTxTime = lastPacketTxTime + packetTime;
		lastPacketSchedTime = lastPacketSchedTime + packetTime;
		rmaxStatus = rmax_out_commit(rmxStreamId, rmaxTime, rmaxCommitFlags);
		ST2110Hardware::RmaxError("rmax_out_commit", rmaxStatus);

		// If entering shutdown then just exit
		if (streamActive)
		{
			lastPacketSchedTime.SleepUntil();
		}
	}
	RiverMaxDestroy();
}

void ST2110Transmitter::RiverMaxDestroy(void)
{
    unsigned int timeout = 40; // 10 times latency, sleep time is quarter latency
	rmax_status_t rmaxStatus;

	rmax_out_cancel_unsent_chunks(rmxStreamId);
	do
	{
		//using namespace std::literals::chrono_literals;
		CLOG(INFO, TRANS_LOG) << "Trying to destroy stream: " << rmxStreamId;
		rmaxStatus = rmax_out_destroy_stream(rmxStreamId);
		if (rmaxStatus == RMAX_ERR_BUSY)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds((unsigned int)round(streamInfo.latency * 250)));
			timeout--;
		}
	}
	while((rmaxStatus == RMAX_ERR_BUSY) && (timeout > 0));
	if (timeout == 0)
	{
		CLOG(INFO, TRANS_LOG) << "Destruction of stream " << rmxStreamId << " timed out";		
		ST2110Hardware::RmaxError("rmax_out_destroy_stream", rmaxStatus);
	}
	else
	{
		CLOG(INFO, TRANS_LOG) << "Destruction of stream " << rmxStreamId << " succeeded";
	}
}

void ST2110Transmitter::Init(AoipSystem &newSystem, StreamInfo &newStreamInfo, char *newSdpText, ST2110TransmitterCallBackInfo *newCallBackInfo)
{
	rmax_status_t rmaxStatus;
	system = newSystem;
	unsigned int i;
	enum AM824ErrorCode am824Err;
	unsigned int chunksPerBlock;
	struct rmax_buffer_attr rmaxBufferAttr;
    struct rmax_mem_block rmaxMemBlock;
	struct rmax_qos_attr rmxQos;

	// Dimension Chunks and Blocks according to stream Type

	if ((newCallBackInfo != nullptr) && (((newCallBackInfo->blockSize > 0) && (newCallBackInfo->blockSize < 64)) || (newCallBackInfo->blockSize > 65536)))
	{
		throw runtime_error("CallBack blocksize out of range (64-65536)");
	}

	CLOG(INFO, TRANS_LOG) << "Tx Stream Initialization";

	// Copy stream Info structure
	streamInfo = newStreamInfo;
	sdpText = newSdpText;

	if (newCallBackInfo != nullptr)
	{
		callBackInfo = *newCallBackInfo; // Take copy of callBack Info
		generatorMode = false;
	}
	else
	{
		generatorMode = true;
	}

	// Do common audio specific checking
	// Validate streamInfo structure
	if (((streamInfo.streamType == AES67) || (streamInfo.streamType == AM824)) &&
		((streamInfo.audio.numChannels == 0) || (streamInfo.audio.numChannels > MAX_CHANNELS)))
	{
		throw runtime_error("Error: Number of channels out of range");
	}

	// Check latency is in range
	if ((streamInfo.latency < minLatency) || (streamInfo.latency > maxLatency))
	{
		throw runtime_error("Transmit Latency is out of range");
	}

	// Initialize AM824 framer if required
	switch (streamInfo.streamType)
	{
		case AES67:
			streamBytesPerSample = streamInfo.audio.payloadBytesPerSample;
			break;

		case AM824:
			streamBytesPerSample = 4;
			switch (callBackInfo.audioFormat)
			{
			case DLB_AOIP_AUDIO_FORMAT_16BIT_LPCM:
				aM824Framer.Init(streamInfo.audio.numChannels, 16, AM824_BIG_ENDIAN, am824Err);
				break;
			case DLB_AOIP_AUDIO_FORMAT_24BIT_LPCM:
				aM824Framer.Init(streamInfo.audio.numChannels, 24, AM824_BIG_ENDIAN, am824Err);
				break;
			case DLB_AOIP_AUDIO_FORMAT_32BIT_LPCM:
				aM824Framer.Init(streamInfo.audio.numChannels, 32, AM824_BIG_ENDIAN, am824Err);
				break;
			default:
				throw runtime_error("Error: AudioFormat for AM824 stream not supported, must be 16,24 or 32bit PCM");
			}
			if (am824Err != AM824_ERR_OK)
			{
				throw runtime_error("AM824 Framer error");
			}
			aM824Framer.setProfessionalMode();
			aM824Framer.setDataMode();
			switch(streamInfo.samplingFrequency)
			{
			case 32000:
				aM824Framer.setSamplingFrequency(FS_32000_HZ);
				break;
			case 44100:
				aM824Framer.setSamplingFrequency(FS_44100_HZ);
				break;
			case 48000:
				aM824Framer.setSamplingFrequency(FS_48000_HZ);
				break;
			default:
				aM824Framer.setSamplingFrequency(FS_NOT_INDICATED);
			}
			break;

		case SMPTE2110_41:
			klvFragTimeMs = 1.0;
			break;

		default:
		throw runtime_error("Error: Unknown stream type");
	}

	// Detect automatic port selection
	if (streamInfo.port == 0)
	{
		srcPort = dstPort = 5004;
	}
	else
	{
		srcPort = dstPort = streamInfo.port;
	}

	// Now that we have the stream configuration, set the remining stream characteristics
	// Set packet dimension based on stream configuration
	if (streamInfo.streamType == SMPTE2110_41)
	{
		// Dimension the memory to allow for maximum size payload
		streamPacketSizeBytes = RTP_PAYLOAD_SIZE;
		//payloadPacketSizeBytes = streamPacketSizeBytes;
		callBackPacketSizeBytes= streamPacketSizeBytes;
		packetTimeMs = streamInfo.metadata.packetTimeMs; // Send 1 KLV unit per video frame, Only supporting 25fps for now
	}
	else
	{
		streamPacketSizeBytes = streamInfo.audio.samplesPerPacket * streamBytesPerSample * streamInfo.audio.numChannels;
		callBackPacketSizeBytes = streamInfo.audio.samplesPerPacket * GetAoipBytesPerSample(callBackInfo.audioFormat) * streamInfo.audio.numChannels;
		//payloadPacketSizeBytes = streamInfo.audio.samplesPerPacket * streamInfo.audio.payloadBytesPerSample * streamInfo.audio.numChannels;
		packetTimeMs = (streamInfo.audio.samplesPerPacket * 1000.0) / system.samplingFrequency;
	}

	// If no block size selected then select payload size for efficiency
	if (callBackInfo.blockSize == 0)
	{
		callBackInfo.blockSize = streamInfo.audio.samplesPerPacket;
		CLOG(INFO, TRANS_LOG) << "No block size selected so using payload size: " << callBackInfo.blockSize;
	}

	if ((streamInfo.payloadType < 96) || (streamInfo.payloadType > 127))
	{
		throw runtime_error("Error: payload type out of range");		
	}
	rtpSequenceNo = 0; //GetRandomInt(16);

	rmxQos.dscp = 46; // Diff Serv Code Point EF46
	rmxQos.pcp = 0; // Copies from example application, not clear if this doesn anything
	rmaxMemBlock.data_ptr = nullptr;
   	rmaxMemBlock.app_hdr_ptr = nullptr;

   	// Create data structure dimensions
	if (newStreamInfo.streamType == SMPTE2110_41)
	{
		// For metadata must have enough blocks to copy with latency
		// and enough strides to cope with largest data size
		chunksPerBlock = ceil((streamInfo.latency * 1000) / streamInfo.metadata.packetTimeMs) + 2; // extra 2 for luck
		stridesPerChunk = ceil(streamInfo.metadata.maxPayloadSizeBytes / SADM_PAYLOAD_SIZE) + 2; // again extra 2 for luck
	}
	else
	{
		// Enough chunks to cover latency
		// If we run out of chunks then the transmitter thread will starve and halt
		/*
		stridesPerChunk = floor((callBackBlockSize * streamInfo.audio.numChannels * streamInfo.audio.payloadBytesPerSample) / (float)payloadPacketSizeBytes);
		if (stridesPerChunk == 0)
		{
			stridesPerChunk = 1;
		}
		*/
		// The constant used on the next line is a "magic" number and is the ratio of chunk size to the latency.
		// Making this a high number reduces the chunk size and overhead dealing with lots of small chunks
		// As this approaches 1 then the buffering will be much lumpier and the chance of underrun/overrun increase
		// Patricularly at low latencies
		stridesPerChunk = ceil((streamInfo.latency * system.samplingFrequency) / (streamInfo.audio.samplesPerPacket * 10));
		// work out how many chunks we need to cover latency as we only get one block of buffers
		chunksPerBlock = ceil((streamInfo.latency * system.samplingFrequency) / (stridesPerChunk * streamInfo.audio.samplesPerPacket)) + 2;
	}

	if (stridesPerChunk == 0)
	{
		throw runtime_error("stridesPerChunk = 0");
	}

	if (chunksPerBlock == 0)
	{
		throw runtime_error("chunksPerBlock = 0");
	}

	uint16_t rmaxDataSizes[stridesPerChunk * chunksPerBlock];
	uint16_t rmaxHeaderSizes[stridesPerChunk * chunksPerBlock];


    for (i = 0 ; i < stridesPerChunk * chunksPerBlock ; i++)
    {
    	if (streamInfo.streamType != SMPTE2110_41)
    	{
    		rmaxDataSizes[i] = streamPacketSizeBytes;
    	}
    	rmaxHeaderSizes[i] = RTP_HEADER_SIZE;
    }
    // Use dynamic sizes for -41 streams
    if (streamInfo.streamType == SMPTE2110_41)
    {
    	rmaxMemBlock.data_size_arr = nullptr;
    }
    else
    {
    	rmaxMemBlock.data_size_arr = rmaxDataSizes;
    }
    rmaxMemBlock.app_hdr_size_arr = rmaxHeaderSizes;
    rmaxMemBlock.chunks_num = chunksPerBlock;

    rmaxBufferAttr.chunk_size_in_strides = stridesPerChunk;
    rmaxBufferAttr.mem_block_array = &rmaxMemBlock;
    rmaxBufferAttr.mem_block_array_len = 1;
    rmaxBufferAttr.data_stride_size = streamPacketSizeBytes;
    rmaxBufferAttr.app_hdr_stride_size = RTP_HEADER_SIZE;

	rmaxStatus = rmax_out_create_stream(const_cast<char*>(sdpText.c_str()), &rmaxBufferAttr, &rmxQos, 1, 0, &rmxStreamId);
	ST2110Hardware::RmaxError("rmax_out_create_stream", rmaxStatus);

	CLOG(INFO, TRANS_LOG) << "***Created Stream " << streamInfo.streamName << "***";
	if (streamInfo.streamType == SMPTE2110_41)
	{
		CLOG(INFO, TRANS_LOG) << "Stream Type: SMPTE 2110-41";
		CLOG(INFO, TRANS_LOG) << "Maximum Payoad size (Bytes): " << streamInfo.metadata.maxPayloadSizeBytes;
		// only capable of transmitting one DIT so we can cheat by using front here. Will need a loop
		CLOG(INFO, TRANS_LOG) << "Data Item Type: " << streamInfo.metadata.dataItemTypes.front();
	}
	else
	{
		if (streamInfo.streamType == AES67)
		{
			CLOG(INFO, TRANS_LOG) << "Stream Type: SMPTE 2110-30 / AES67";
		}
		else if (streamInfo.streamType == AM824)
		{
			CLOG(INFO, TRANS_LOG) << "Stream Type: SMPTE 2110-31 / AM824";
		}
		CLOG(INFO, TRANS_LOG) << "Number of Channels: " << streamInfo.audio.numChannels;
		CLOG(INFO, TRANS_LOG) << "Input audio bit-depth: " << GetAoipBytesPerSample(callBackInfo.audioFormat) * 8 << " bits";		
		CLOG(INFO, TRANS_LOG) << "Output stream bits per Sample: " << streamBytesPerSample * 8 << " bits";
	}
	CLOG(INFO, TRANS_LOG) << "Packet Time (ms): " << packetTimeMs;
	CLOG(INFO, TRANS_LOG) << "Latency (ms): " << streamInfo.latency * 1000; // latency is in seconds
	CLOG(INFO, TRANS_LOG) << "Number of Chunks: " << chunksPerBlock;
	CLOG(INFO, TRANS_LOG) << "Strides Per Chunk: " << stridesPerChunk;
	CLOG(INFO, TRANS_LOG) << "Toal number of Strides: " << chunksPerBlock * stridesPerChunk;

	CLOG(INFO, TRANS_LOG) << "Dest IP Address: " << streamInfo.dstIpStr;
}

void ST2110Transmitter::Start(void)
{
	streamActive = true;
	switch(streamInfo.streamType)
	{
	case AES67:
	case AM824:
		streamThread = make_shared<thread>(thread(&ST2110Transmitter::AudioStreamThread, this));
		break;
	case SMPTE2110_41:
		streamThread = make_shared<thread>(thread(&ST2110Transmitter::MetadataStreamThread, this));
		break;
	default:
		throw runtime_error("Unknown Stream Type");
	}
}


unsigned int ST2110Transmitter::GetRTPHeader(uint8_t *&buff)
{
	uint32_t timestamp = lastPacketTxTime.GetRTPTimeStamp48kHz();

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


    buff[0] = 0x80;  // 10000000 - version2, no padding, no extension, no CSRC
    buff[1] = streamInfo.payloadType & 0x7f; // Marker bit is 0
    buff[2] = (rtpSequenceNo >> 8) & 0xff;  // sequence number
    buff[3] = (rtpSequenceNo) & 0xff;  // sequence number

    *(uint32_t *)&buff[4] = htobe32((uint32_t)timestamp);
    *(uint32_t *)&buff[8] = htobe32((uint32_t)streamInfo.ssrc);
    

    /* Update sequence number */
    if (rtpSequenceNo == 65535)
    {
    	rtpSequenceNo = 0;
    }
    else
    {
    	rtpSequenceNo++;
    }
 
    buff += RTP_HEADER_SIZE;
    return(RTP_HEADER_SIZE); // bytes written
}

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

unsigned int ST2110Transmitter::GetSMPTE2110_41Header(uint8_t *const buff, unsigned int byteCount, unsigned int byteOffset, bool lastPacket)
{
	unsigned int wordCount = ceil((float)byteCount / 4.0f);
	unsigned int wordOffset = ceil((float)byteOffset / 4.0f);

	// Assume only one DIT for now
	buff[0] = streamInfo.metadata.dataItemTypes.front() >> 14;
	buff[1] = (streamInfo.metadata.dataItemTypes.front() >> 6) & 0xff;
	buff[2] = ((streamInfo.metadata.dataItemTypes.front() << 2) & 0xfc) | ((wordCount >> 8) & 0x1);
	// Add M bit
	if (lastPacket)
	{
		buff[2] |= 0x2;
	}
	buff[3] = wordCount & 0xff;
	// Now Segment Data Offset
	buff[4] = wordOffset >> 24;
	buff[5] = (wordOffset >> 16) & 0xff;
	buff[6] = (wordOffset >> 8) & 0xff;
	buff[7] = wordOffset & 0xff;
	return(SMPTE2110_41_HEADER_SIZE);
}

unsigned int ST2110Transmitter::GetSMPTE2116Header(uint8_t *const buff)
{
	// assemble info word (16bit mode from SMPTE 2116)
	buff[0] = buff[1] = 0;
	// format info word (16bit mode from SMPTE 2116)
	buff[2] = 0;
	buff[3] = 1; // Gzip format
	return(SMPTE2116_HEADER_SIZE);
}

unsigned int ST2110Transmitter::FormatPacketData(unsigned int& readIndex, void *readBufBegin, unsigned int readBufSize, void *streamPacketBuf)
{
	uint16_t *p16;
	uint32_t *p32;
	uint8_t *pRead = &((uint8_t *)readBufBegin)[readIndex];
	uint8_t *pStream = (uint8_t *)streamPacketBuf;
	uint32_t i,j;
	uint32_t input24;
	unsigned int callBackBytesPerSample;
	unsigned int totalCallBackBytes;

	if ((callBackInfo.audioFormat == DLB_AOIP_AUDIO_FORMAT_32BIT_FLOAT) ||
		(callBackInfo.audioFormat == DLB_AOIP_AUDIO_FORMAT_8BIT_LPCM))
	{
		throw runtime_error("Audio Format not yet suppoted");
	}
	else
	{
		callBackBytesPerSample = GetAoipBytesPerSample(callBackInfo.audioFormat);
	}

	totalCallBackBytes = streamInfo.audio.samplesPerPacket * streamInfo.audio.numChannels * callBackBytesPerSample;

	switch(streamInfo.streamType)
	{
		case AES67:
		if (streamInfo.audio.payloadBytesPerSample == callBackBytesPerSample)
		{
			// Convert from little endian to big endian (network order) if required
			if (ST2110Hardware::IsLittleEndian())
			{
				for (i = 0 ; i < streamInfo.audio.samplesPerPacket * streamInfo.audio.numChannels; i++)
				{
					for (j = 0 ; j < streamInfo.audio.payloadBytesPerSample ; j++)
					{
						pStream[j] = pRead[streamInfo.audio.payloadBytesPerSample - j - 1];
					}
					pStream += streamInfo.audio.payloadBytesPerSample;
					pRead += streamInfo.audio.payloadBytesPerSample;
					readIndex += streamInfo.audio.payloadBytesPerSample;
					if (readIndex >= readBufSize)
					{
						pRead = (uint8_t *)readBufBegin;
						readIndex = 0;
					}
				}
			}
			else
			{
				unsigned int bytesToCopy = streamInfo.audio.samplesPerPacket * streamInfo.audio.payloadBytesPerSample * streamInfo.audio.numChannels;
				unsigned int readBytesLeft = readBufSize - readIndex;
				if (bytesToCopy <= readBytesLeft)
				{
					memcpy(pStream, pRead, bytesToCopy);
				}
				else
				{
					memcpy(pStream, pRead, readBytesLeft);
					memcpy(&pStream[readBytesLeft], readBufBegin, bytesToCopy - readBytesLeft);
				}
			}
		}
		else
		{
			// Convert from format specified in callBackInfo to stream format
			if (ST2110Hardware::IsLittleEndian())
			{
				unsigned int minBytesPerSample = callBackBytesPerSample;
				if (streamInfo.audio.payloadBytesPerSample < callBackBytesPerSample)
				{
					minBytesPerSample = streamInfo.audio.payloadBytesPerSample;
				}
				for (i = 0 ; i < streamInfo.audio.samplesPerPacket * streamInfo.audio.numChannels; i++)
				{
					// Note that input format is little endian, output is network or big endian
					// Start with the MSB first, assume left justified audio 
					for (j = 0 ; j < minBytesPerSample ; j++)
					{
						pStream[j] = pRead[callBackBytesPerSample - j - 1];
					}
					// If output bytes per sample is large then fill in with zeros
					for (; j < streamInfo.audio.payloadBytesPerSample ; j++)
					{
						pStream[j] = 0;
					}
					pStream += streamInfo.audio.payloadBytesPerSample;
					pRead += callBackBytesPerSample;
					readIndex += callBackBytesPerSample;
					if (readIndex >= readBufSize)
					{
						pRead = (uint8_t *)readBufBegin;
						readIndex = 0;
					}
				}
			}
			else
			{
				throw runtime_error("Big Endian Conversion Not implemented yet");
			}
		}
		break;
		case AM824:
		switch(callBackBytesPerSample)
		{
			case 2:
			p16 = (uint16_t *)pRead;
			for (i = 0 ; i < streamInfo.audio.samplesPerPacket * streamInfo.audio.numChannels; i++)
			{
				aM824Framer.getAM824Sample(*p16++, pStream);
				pStream += streamBytesPerSample;
				readIndex += 2;
				if (readIndex >= readBufSize)
				{
					p16 = (uint16_t *)readBufBegin;
					readIndex = 0;
				}
			}
			break;
			case 3:
			for (i = 0 ; i < streamInfo.audio.samplesPerPacket * streamInfo.audio.numChannels ; i++)
			{
				if (ST2110Hardware::IsLittleEndian())
				{
					// Conversion to big endian for 24 bit word
					input24 = *pRead++;
					input24 += *pRead++ << 8;
					input24 += *pRead++ << 16;
				}
				else
				{
					input24 = *pRead++ << 16;
					input24 += *pRead++ << 8;
					input24 += *pRead++;
				}
				aM824Framer.getAM824Sample(input24, pStream);
				pStream += streamBytesPerSample;
				readIndex += 3;
				if (readIndex >= readBufSize)
				{
					pRead = (uint8_t *)readBufBegin;
					readIndex = 0;
				}				
			}
			break;
			case 4:
			p32 = (uint32_t *)pRead;
			for (i = 0 ; i < streamInfo.audio.samplesPerPacket * streamInfo.audio.numChannels; i++)
			{
				aM824Framer.getAM824Sample(*p32++, pStream);
				pStream += streamBytesPerSample;
				readIndex += 4;
				if (readIndex >= readBufSize)
				{
					p32 = (uint32_t *)readBufBegin;
					readIndex = 0;
				}
			}
			break;
			default:
			throw runtime_error("Error: Unsupported bit depth");
		}
		break;
		default:
		throw runtime_error("Error: Unsupported Format");
	}
	return(totalCallBackBytes);
}

bool ST2110Transmitter::GeneratePayload(void *dataPtr, unsigned int numBytes)
{
	unsigned int i,j;
	uint8_t *p = (uint8_t *)dataPtr;
	unsigned int samples = numBytes / streamBytesPerSample;

	// Generate dummy payload for testing
	for (i = 0 ; i < samples ; i++)
	{
		for (j = 0 ; j < streamBytesPerSample ; j++)
		{
			p[(i * streamBytesPerSample) + j] = i % 256;
		}
	}
	// Never terminate, this has be done externally
	return(true);
}
