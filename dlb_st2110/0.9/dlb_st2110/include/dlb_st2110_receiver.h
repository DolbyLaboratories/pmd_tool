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
#define MAX_HEADER_SIZE 20
#define SMPTE2110_41_HEADER_SIZE 8
#define SMPTE2116_HEADER_SIZE 4
#define RTP_HEADER_SIZE 12
#define UDP_HEADER_SIZE 8
#define IP_HEADER_SIZE 20
#define ETHERNET_HEADER_SIZE 14
#define RTP_PAYLOAD_SIZE (MTU_SIZE - RTP_HEADER_SIZE - UDP_HEADER_SIZE - IP_HEADER_SIZE - ETHERNET_HEADER_SIZE)
#define L4_PAYLOAD_SIZE (MTU_SIZE - UDP_HEADER_SIZE - IP_HEADER_SIZE - ETHERNET_HEADER_SIZE)
#define RTP_PAYLOAD_SIZE_WORD_ALIGNED (MTU_SIZE - RTP_HEADER_SIZE - UDP_HEADER_SIZE - IP_HEADER_SIZE - ETHERNET_HEADER_SIZE - 2)
#define SADM_PAYLOAD_SIZE (RTP_PAYLOAD_SIZE_WORD_ALIGNED - SMPTE2110_41_HEADER_SIZE - SMPTE2116_HEADER_SIZE)
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