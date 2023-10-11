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

/** @ingroup st2110 */

/** @file aoip_services.h
 *  @brief Provides ST2110-30/21 receive and transmit services - API
 */


#ifndef _DLB_AOIP_SERVICES_H_
#define _DLB_AOIP_SERVICES_H_

#include <thread>
#include <memory>
#include <string>
#include <arpa/inet.h>

struct ST2110TransceiverCallBackInfo;

#include "dlb_st2110.h"
#include "dlb_st2110_sdp.h"
#include "dlb_st2110_hardware.h"
#include "mclock.h"
#include "dlb_st2110_transmitter.h"
#include "dlb_st2110_receiver.h"
#include "audio_buffer.h"

//#include "sap_discovery.h"
//#include "rav_discovery.h"
//#include "nmos_discovery.h"

/************************* Constants ***************************/

/**
 * @def MAX_MTU
 * @brief maximum size of ethernet transmission unit
 */
#define MAX_MTU 1500

/**
 * @def MAX_SDP_SIZE
 * @brief maximum supported SDP size
 */
#define MAX_SDP_SIZE 8192

/************************* TypeDefs ***************************/

class AoipRxTxStream
{
public:

/**
 * @brief Provides information related to application callback by transceiver
 * 
 */

	struct TxCallBackData
	{
		AoipRxTxStream *stream; /**< Handle to stream */
		unsigned int txStream;  /**< Index to stream starting at 0 */
	};

/**
 * @brief Provides additional information to a transmitter during callback 
 * 
 */

private:
	
	StreamInfo rxStreamInfo;
	unsigned int numTxStreams;
	std::vector<StreamInfo> txStreamInfos;
	unsigned int numTxChannels;
	float latency;
	ST2110TransceiverCallBackInfo callBackInfo;
	std::unique_ptr<AudioBuffers> audioBuffer;
	bool streamActive;
	unsigned int latencyOffset;
	unsigned int callBackBytesPerSample;
	unsigned int rxCallBackOutputSizeBytes;

public:
	AoipRxTxStream(StreamInfo &newRxStreamInfo, std::vector<StreamInfo> &newTxStreamInfos, float newLatency, ST2110TransceiverCallBackInfo &newCallBackInfo):
	 	rxStreamInfo(newRxStreamInfo),
	 	txStreamInfos(newTxStreamInfos),
	 	latency(newLatency),
	 	callBackInfo(newCallBackInfo)
	 {
	 	std::vector<AudioBuffers::BufferReader> bufferReaders;

	 	if (latency == 0)
	 	{
	 		throw std::runtime_error("Invalid Latency");
	 	}
	 	if (callBackInfo.blockSize == 0)
	 	{
	 		throw std::runtime_error("Invalid blocksize");
	 	}

	 	numTxStreams = txStreamInfos.size();

	 	if (numTxStreams == 0)
	 	{
	 		throw std::runtime_error("No Tx Stream descriptors");
	 	}


	 	// Determine number of output channels required
	 	numTxChannels = 0;
	 	for (unsigned int i = 0 ; i < numTxStreams; i++)
	 	{
	 		unsigned int lastStreamChannelPlus1 = (txStreamInfos[i]).sourceIndex + (txStreamInfos[i]).audio.numChannels;
	 		if ( lastStreamChannelPlus1 > numTxChannels)
	 		{
	 			numTxChannels = lastStreamChannelPlus1;
	 		}
	 		bufferReaders.push_back(AudioBuffers::BufferReader((txStreamInfos[i]).sourceIndex, (txStreamInfos[i]).audio.numChannels));
	 	}
	 	if (numTxChannels == 0)
	 	{
	 		throw std::runtime_error("Invalid total number of Tx Channels");
	 	}


	 	latencyOffset = latency * txStreamInfos[0].samplingFrequency;

	 	if ((callBackInfo.audioFormat != DLB_AOIP_AUDIO_FORMAT_16BIT_LPCM) &&
	 		(callBackInfo.audioFormat != DLB_AOIP_AUDIO_FORMAT_24BIT_LPCM) &&
			(callBackInfo.audioFormat != DLB_AOIP_AUDIO_FORMAT_32BIT_LPCM))
	 	{
	 		throw std::runtime_error("Unsupported audio format");
	 	}
	 	callBackBytesPerSample = GetAoipBytesPerSample(callBackInfo.audioFormat);

	 	unsigned int numInputBuffers = (newLatency * newTxStreamInfos[0].samplingFrequency) / callBackInfo.blockSize;
	 	if (numInputBuffers < 2)
	 	{
	 		numInputBuffers = 2;

	 	}
	 	CLOG(INFO, SERVICES_LOG) << "Requesting " << numInputBuffers << " input Buffers of " << callBackInfo.blockSize << " frames";

	 	audioBuffer = std::make_unique<AudioBuffers>(numInputBuffers, callBackInfo.blockSize, bufferReaders, newRxStreamInfo.audio.numChannels, newCallBackInfo.audioFormat);
	 	rxCallBackOutputSizeBytes = callBackInfo.blockSize * newRxStreamInfo.audio.numChannels * callBackBytesPerSample;
	 	CLOG(INFO, SERVICES_LOG) << "Receive CallBack output block size: " << rxCallBackOutputSizeBytes << " bytes";
	 	CLOG(INFO, SERVICES_LOG) << "Number of output streams: " << numTxStreams;
	 	CLOG(INFO, SERVICES_LOG) << "Number of output channels allocated: " << numTxChannels;
	 	CLOG(INFO, SERVICES_LOG) << "Latency Offset: " << latency << "s";
	 }

	bool Start();

	std::vector<std::string> GetTxNames() const
	{
		std::vector<std::string> txNames;
		for (std::vector<StreamInfo>::const_iterator streamInfo = txStreamInfos.begin() ; streamInfo != txStreamInfos.end() ; streamInfo++)
		{
			txNames.push_back(streamInfo->streamName);
		}
		return(txNames);
	}

	std::string GetRxName() const
	{
		return(rxStreamInfo.streamName);
	}


	bool RxStreamCallBack(void *inputAudio, unsigned int numBytes, uint32_t timeStamp);
	bool TxStreamCallBack(unsigned int txStream, void *outputAudio, unsigned int &numBytes, bool &haveTimeStamp, uint32_t &timeStamp);

};

inline
std::ostream& operator<<(std::ostream& os, const AoipService& service)
{
	service.Print(os);
	return(os);
}


/************************** Helper Functions *******************/


inline
uint64_t GetRandomInt(unsigned int numBits)
{
	// Simple random number generator
	// Needs to be improved as per Appendix A.6 of RFC 3550. This has code to use
	uint64_t rand64 = ((uint64_t)(rand() & 0x7fff) << 49) | ((uint64_t)(rand() & 0x7fff) << 34) | ((rand() & 0x7fff) << 19) | ((rand() & 0x7fff) << 4) | (rand() & 0xf);

	// reduce to number of bits required
	return(rand64 & (((unsigned int)pow(2, numBits)) - 1));
}

/************************* Classes ***************************/

// forward definitions
class AoipTxStream;
//class SapDiscovery;
//class RavDiscovery;
//class NmosDiscovery;


class AoipTxStream
{
	StreamInfo streamInfo;
	char sdpText[MAX_MTU];
		
	MClock::TimePoint nextSapPacketTxTime;
	MClock::Duration baseSapInterval;
	ST2110Transmitter transmitter;
	SdpSystemInfo sdpSystemInfo;

public:
	AoipTxStream(StreamInfo &newStreamInfo, AoipSystem &newSystem, ST2110TransmitterCallBackInfo *callBackInfo)
	{
		streamInfo = newStreamInfo;

		sdpSystemInfo.gmIdentity = newSystem.gmIdentity;
		sdpSystemInfo.domain = newSystem.domain;
		streamInfo.sessionId = GetRandomInt(48);
		streamInfo.ssrc = GetRandomInt(32);

		Sdp2110 sdp(streamInfo, sdpSystemInfo);
		// Build normal SDP for SAP packet
		sdp.GetText(sdpText, sizeof(sdpText));

		// This SDP is used to configure Rmax so get that version
		// This gets round lack of support for SMPTE 2110-41 but uses -40 in the SDP
		sdp.GetTextRmax(sdpText, sizeof(sdpText));

		transmitter.Init(newSystem, newStreamInfo, sdpText, callBackInfo);
	}

	std::string GetName()
	{
		return(streamInfo.streamName);
	}

	uint32_t GetHostSrcIpInt(void)
	{
		return(ntohl(inet_addr(streamInfo.srcIpStr.c_str())));
	}

	void Start()
	{
		transmitter.Start();
	}

	void Stop()
	{
		transmitter.Stop();
	}

	void Update(StreamInfo &newStreamInfo)
	{
		Sdp2110 sdp(streamInfo, sdpSystemInfo);
		// Build normal SDP for SAP packet
		sdp.GetText(sdpText, sizeof(sdpText));

		// This SDP is used to configure Rmax so get that version
		// This gets round lack of support for SMPTE 2110-41 but uses -40 in the SDP
		sdp.GetTextRmax(sdpText, sizeof(sdpText));
		transmitter.Update(newStreamInfo, sdpText);
	}
};


#endif // _DLB_AOIP_SERVICES_H_
