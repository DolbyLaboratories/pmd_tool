/************************************************************************
 * dlb_st2110
 * Copyright (c) 2019-2020, Dolby Laboratories Inc.
 * Copyright (c) 2019-2020, Dolby International AB.
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

#ifndef _DLB_ST2110_RECEIVER_H_
#define _DLB_ST2110_RECEIVER_H_

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <fstream>
#include <streambuf>
#include <algorithm>
#include <linux/net_tstamp.h>
#include <poll.h>
#include <chrono>
#include <linux/errqueue.h>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <deque>
#include <iomanip>
#include <rivermax_api.h>


#include "dlb_st2110.h"
#include "am824_framer.h"
#include "mclock.h"

/************************* Constants ***************************/


// Number of channels supported
#define MAX_CHANNELS 16
#define MTU_SIZE 1500
#define SMPTE2110_41_HEADER_SIZE 8
#define RTP_HEADER_SIZE 12
#define UDP_HEADER_SIZE 8
#define IP_HEADER_SIZE 20
#define ETHERNET_HEADER_SIZE 14
#define RTP_PAYLOAD_SIZE (MTU_SIZE - RTP_HEADER_SIZE - UDP_HEADER_SIZE - IP_HEADER_SIZE - ETHERNET_HEADER_SIZE)
#define L4_PAYLOAD_SIZE (MTU_SIZE - UDP_HEADER_SIZE - IP_HEADER_SIZE - ETHERNET_HEADER_SIZE)
#define RTP_PAYLOAD_SIZE_WORD_ALIGNED (MTU_SIZE - RTP_HEADER_SIZE - UDP_HEADER_SIZE - IP_HEADER_SIZE - ETHERNET_HEADER_SIZE - 2)
#define SMPTE2110_41_PAYLOAD_SIZE (RTP_PAYLOAD_SIZE_WORD_ALIGNED - SMPTE2110_41_HEADER_SIZE)

/************************* Classes ***************************/

class ST2110Receiver
{
	StreamInfo streamInfo;
	unsigned int blockSizeBytes;
	float packetTimeMs;
	AoipSystem system;
	MClock::TimePoint nextSAPPacketTxTime;
	MClock::Duration BaseSAPInterval;
	MClock::TimePoint lastPacketTxTime;
	MClock::TimePoint lastPacketSchedTime;
	AM824Framer aM824Framer;
	std::shared_ptr<std::thread> streamThread;
	rmax_stream_id rmaxStreamId;
	rmax_in_flow_attr rmaxInFlowAttr;
	uint16_t rtpSequenceNo;
	bool streamActive;
	unsigned char sapPacketData[MTU_SIZE];
	uint32_t smpte2110_41SegmentCount;
    unsigned int numPacketsLatency;
    ST2110ReceiverCallBackInfo callBackInfo;
    unsigned int callBackBytesPerSample;

	// Main private functions


public:

	// blockSize is size of block to be returned int callback in samples
	// For metadata this may be unused or the number of packets

	void Init(AoipSystem &newSystem, StreamInfo &newStreamInfo, ST2110ReceiverCallBackInfo &newCallBackInfo);

	ST2110Receiver() { streamThread = nullptr; }

	ST2110Receiver(AoipSystem &system, StreamInfo &streamInfo, ST2110ReceiverCallBackInfo &callBackInfo)
	{
		Init(system, streamInfo, callBackInfo);
	}

	~ST2110Receiver(void)
	{
		// Shut down thread
		streamActive = false;
		CLOG(INFO, RECEIVE_LOG) << "Received Shutdown Signal...";
		if (streamThread)
		{
			if (streamThread->joinable())
			{
				CLOG(INFO, RECEIVE_LOG) << "Joining thread...";
				streamThread->join();
			}
			else
			{
				CLOG(WARNING, RECEIVE_LOG) << "Can't join thread";				
			}
		}
		else
		{
			CLOG(WARNING, RECEIVE_LOG) << "No thread to join";
		}
		CLOG(INFO, RECEIVE_LOG) << "Shutdown Complete...";
	}

	void Start(void);

	std::string GetName()
	{
		return(streamInfo.streamName);
	}

private:

	void AudioStreamThread(void);
	void MetadataStreamThread(void);
	unsigned int DeformatPacketData(void *streamPacketBuf, unsigned int& index, void *outputBufBegin, unsigned int outputBufSize, int numBytes);


	// Helper functions

};






#endif // DLB_ST2110_RECEIVER_H