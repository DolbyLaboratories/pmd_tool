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

#ifndef _DLB_ST2110_TRANSMITTER_H_
#define _DLB_ST2110_TRANSMITTER_H_

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
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
#include "dlb_st2110_logging.h"

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
#define RTP_PAYLOAD_SIZE_WORD_ALIGNED (MTU_SIZE - RTP_HEADER_SIZE - UDP_HEADER_SIZE - IP_HEADER_SIZE - ETHERNET_HEADER_SIZE - 2)
#define SADM_PAYLOAD_SIZE (RTP_PAYLOAD_SIZE_WORD_ALIGNED - SMPTE2110_41_HEADER_SIZE - SMPTE2116_HEADER_SIZE)
#define SMPTE2110_41_PAYLOAD_SIZE (RTP_PAYLOAD_SIZE_WORD_ALIGNED - SMPTE2110_41_HEADER_SIZE)


//const char *channelLabels[MAX_CHANNELS] = {"L","R", "C", "LFE", "Ls", "Rs", "Lb", "Rb", "Tfl", "Tfr", "Tsl", "Tsr", "Tbl", "Tbr", "Tfc", "Tbc"};

/************************* TypeDefs ***************************/

struct SAPAnnoucement
{
  uint8_t vartec;
  uint8_t auth_len;
  uint16_t msg_id_hash;
  uint32_t source_address;
  char payload_type[16];
};

/************************** Helper Functions *******************/

template<typename Clock, typename Duration>
std::ostream &operator<<(std::ostream &stream,
  const std::chrono::time_point<Clock, Duration> &time_point)
{
  const time_t time = Clock::to_time_t(time_point);
  struct tm tm;
  localtime_r(&time, &tm);
  return stream << std::put_time(&tm, "%c"); // Print standard date&time
}

/************************* Classes ***************************/

class ST2110Transmitter
{
	StreamInfo streamInfo;
	std::string sdpText;
	unsigned int streamPacketSizeBytes;
	unsigned int callBackPacketSizeBytes;
	unsigned int streamBytesPerSample;
	float packetTimeMs;
	uint16_t srcPort;
	uint16_t dstPort;
	AoipSystem system;
	MClock::TimePoint nextSAPPacketTxTime;
	MClock::Duration BaseSAPInterval;
	MClock::TimePoint lastPacketTxTime;
	MClock::TimePoint lastPacketSchedTime;
	bool firstPacket;
	AM824Framer aM824Framer;
	float klvFragTimeMs;
	std::shared_ptr<std::thread> streamThread;
	rmax_stream_id rmxStreamId;
	uint16_t rtpSequenceNo;
	bool streamActive;
	unsigned char sapPacketData[MTU_SIZE];
	uint32_t smpte2110_41SegmentCount;
	unsigned int stridesPerChunk;
	ST2110TransmitterCallBackInfo callBackInfo;
	bool generatorMode;

	// Main private functions


public:

	void Init(AoipSystem &newSystem, StreamInfo &newStreamInfo, char *newSdpText, ST2110TransmitterCallBackInfo *newCallBackInfo);

	void Init(AoipSystem &newSystem, StreamInfo &newStreamInfo, char *newSdpText)
	{
		Init(newSystem, newStreamInfo, newSdpText, nullptr);
	}

	ST2110Transmitter()
	{ 
		CLOG(INFO, TRANS_LOG) << "Transmitter Created";
		streamThread = nullptr;
	}

	ST2110Transmitter(ST2110Transmitter& copy) = delete;

	ST2110Transmitter(AoipSystem &newSystem, StreamInfo &streamInfo, char *newSdpText, ST2110TransmitterCallBackInfo &newCallBackInfo)
	{
		CLOG(INFO, TRANS_LOG) << "Transmitter Created";
		Init(newSystem, streamInfo, newSdpText, &newCallBackInfo);
	}

	// No callback so payload is internally generated
	// To start with this is a pattern for debugging but could evolve into signal generator
	ST2110Transmitter(AoipSystem &newSystem, StreamInfo &streamInfo, char *newSdpText)
	{
		CLOG(INFO, TRANS_LOG) << "Transmitter Created - Internal Payload Generation";
		Init(newSystem, streamInfo, newSdpText, nullptr);
	}

	~ST2110Transmitter(void)
	{		
		// Shut down thread
		CLOG(INFO, TRANS_LOG) << "Stream " << streamInfo.streamName << "(" << rmxStreamId << ") received shutdown signal...";
		streamActive = false;
		if (streamThread)
		{
			if (streamThread->joinable())
			{
				CLOG(INFO, TRANS_LOG) << "Joining thread...";
				streamThread->join();
			}
			else
			{
				CLOG(INFO, TRANS_LOG) << "Can't join thread";				
			}
		}
		else
		{
			CLOG(WARNING, TRANS_LOG) << "No thread to join";
		}
		CLOG(INFO, TRANS_LOG) << "Shutdown Complete...";
	}

	void Start(void);

private:

	unsigned int GetRTPHeader(uint8_t *&buff);
	unsigned int GetSMPTE2110_41Header(uint8_t *const buff, unsigned int byteCount, unsigned int byteOffset, bool lastPacket);
	unsigned int GetSMPTE2116Header(uint8_t *const buff);


	bool GeneratePayload(void *dataPtr, unsigned int numBytes);

	void AudioStreamThread(void);
	void MetadataStreamThread(void);
	void RiverMaxDestroy(void);

	unsigned int FormatPacketData(unsigned int& readIndex, void *readBufBegin, unsigned int readBufSize, void *streamPacketBuf);

	unsigned int BuildSAPPacket(void);

	// Helper functions

};

class ProtoBuf
{
private:
	unsigned int maxFragmentSize;
	unsigned int align;
	unsigned int fragmentedSizeSum;
	std::deque<unsigned char> data;
	std::deque<unsigned char> fragment;

public:

	ProtoBuf(unsigned int newMaxFragmentSize, unsigned int newAlign) : maxFragmentSize(newMaxFragmentSize), align(newAlign)  {}

	void AddHeader(unsigned char *header, unsigned int headerSize)
	{
		for (int i = headerSize-1 ; i >= 0 ; i--)
		{
			fragment.push_front(header[i]);
		}
	}

	void Init(void)
	{
		data.clear();
		fragment.clear();
		fragmentedSizeSum = 0;
	}


	void AddPayload(unsigned char *payload, unsigned int payloadSize)
	{
		unsigned int alignPayloadSize = align * ceil((float)payloadSize / 4.0);
		data.insert(data.end(), payload, payload + payloadSize);
		if (alignPayloadSize > payloadSize)
		{
			data.insert(data.end(), alignPayloadSize - payloadSize, 0);
		}
	}

	unsigned int GetPayloadRemainingSize() const
	{
		return(data.size());
	}

	void Align()
	{
		unsigned int newAlignSize = align * std::ceil((float)data.size() / 4.0);
		if (newAlignSize > data.size())
		{
			data.resize(newAlignSize);
		}
	}

	unsigned int GetNumFragments() const
	{
		return(std::ceil(data.size() / (float)maxFragmentSize));
	}

	void Fragment(unsigned int thisMaxFragmentSize, unsigned int align)
	{
		thisMaxFragmentSize = align * std::ceil((float)thisMaxFragmentSize / 4.0);

		if (thisMaxFragmentSize > data.size())
		{
			thisMaxFragmentSize = data.size();
		}
		// updated fragmented so far count
		fragment.resize(thisMaxFragmentSize);
		std::copy(data.begin(), data.begin() + thisMaxFragmentSize, fragment.begin());
		data.erase(data.begin(), data.begin() + thisMaxFragmentSize);
	}

	void Fragment(unsigned int thisMaxFragmentSize)
	{
		Fragment(thisMaxFragmentSize, align);
	}

	void Fragment()
	{
		Fragment(maxFragmentSize, align);
	}

	unsigned int GetFragmentedSum() const
	{
		return(fragmentedSizeSum);
	}

	unsigned int GetFragment(unsigned char *dst)
	{
		unsigned int size = fragment.size();
		for (std::deque<unsigned char>::iterator i = fragment.begin() ; i != fragment.end();)
		{
			*dst++ = *i++;
		}
		fragmentedSizeSum += fragment.size();
		fragment.clear();
		return(size);
	}

	unsigned int GetFragmentSize() const
	{
		return(fragment.size());
	}

	void HexDumpFragment(unsigned int max) const
	{
		unsigned int count = 0;
		for (std::deque<unsigned char>::const_iterator i = fragment.begin() ; (i != fragment.end()) && (count < max) ; i++)
		{
			if ((count % 4) == 0)
			{
				std::cout << std::endl;
				std::cout << std::setfill('0') << std::setw(4) << std::right << std::hex << count ;
				std::cout << ": ";
			}
			std::cout << std::setfill('0') << std::setw(2) << std::right << std::hex << (int)*i << " ";
			count++;
		}
		std::cout << std::endl;
	}

	void HexDumpFragment() const
	{
		HexDumpFragment(fragment.size());
	}

};





#endif // DLB_ST2110_TRANSMITTER_H