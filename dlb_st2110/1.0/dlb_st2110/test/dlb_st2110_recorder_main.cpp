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

#define LOGGING

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <string>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <fstream>
#include <streambuf>
#include <algorithm>
#include <sndfile.hh>
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>
#include <csignal>
#include <rivermax_api.h>

#include "dlb_st2110_api.h"

using namespace std;

/************************* Constants ***************************/

#define MAX_LABEL_SIZE 256
#define VERSION 0.9


/********************** Type Defs *****************************/

typedef struct userInfo
{
	string interface;
	float recordTime;
	string filename;
	string streamName;
	float latency;
	unsigned int ptpDomain;
	unsigned int blockSize;
	unsigned int bitDepth;
	const AoipService *service;
} UserInfo;

/************************** Helper Functions *******************/

class ConsoleSpinner
{
private:
    int counter;

public:    
    ConsoleSpinner()
    {
        counter = 0;
    }

    ~ConsoleSpinner()
    {
    	clear();
    }

    void turn()
    {
        counter++;        
        switch (counter % 4)
        {
            case 0: std::cout << "\b\\" << std::flush; break;
            case 1: std::cout << "\b|" << std::flush; break;
            case 2: std::cout << "\b/" << std::flush; break;
            case 3: std::cout << "\b-" << std::flush; break;
        }
    }
    void clear()
    {
    	std::cout << "\b" << std::flush;    	
    }
};
 
void print_usage(void)
{
	cerr << "dlb_2110_recorder_main -o <OUTPUT FILE> -if <INTERFACE> -b <BIT DEPTH> -bl <BLOCK SIZE> -t <RECORD TIME> -N <NAME> -l <LATENCY> -D <DOMAIN>" << VERSION << endl;
	cerr << "Copyright Dolby Laboratories Inc., 2021. All rights reserved." << endl << endl;
	cerr << "<OUTPUT FILE>             Filename of output file (WAV file for -30/-31, binary for -41)" << endl;
	cerr << "<INTERFACE>               Interface to receive streams on" << endl;
	cerr << "<BIT DEPTH>               Bit Depth of output file, default is to use bit depth of stream" << endl;
	cerr << "<BLOCK SIZE>              Selected Blocksize for callback, 512 by default" << endl;
	cerr << "<RECORD TIME>             Time to record stream in seconds (default = 10s)" << endl;
	cerr << "<NAME>                    Name of the stream to be recorded from (recorder-default)" << endl;
	cerr << "<LATENCY>                 Receive latency in seconds to be used (0.5 by default)." << endl;
	cerr << "<DOMAIN>                  PTP domain to be used (0 by default)" << endl;
}


// For now, share data with callbacks via globals
// This needs to be fixed via a structure and opaque pointer
typedef struct
{
	SndfileHandle *waveFile = nullptr;
	FILE *metadataFile = nullptr;
	unsigned char *metadata = nullptr;
	unsigned int metadataSize = 0;
	bool streamActive = false;
	unsigned int bytesRemaining = 0;
	float timeRemaining = 0.0;
	float packetTime = 0.0;
} CallBackData;

int32_t readSample24(void *p)
{
	uint8_t *bytep = (uint8_t *)p;
	int32_t result32;

	if (ST2110Hardware::IsLittleEndian())
	{
		result32 = ((int8_t)bytep[2]) << 16;
		result32 += bytep[1] << 8;
		result32 += bytep[0];
		return(result32);
	}
	else
	{
		result32 = ((int8_t)bytep[0]) << 16;
		result32 += bytep[1] << 8;
		result32 += bytep[2];
		return(result32);
	}
}

void writeSample24(void *p, int32_t sample24)
{
	uint8_t *bytep = (uint8_t *)p;

	if (ST2110Hardware::IsLittleEndian())
	{
		bytep[0] = sample24 & 0xff;
		bytep[1] = (sample24 & 0xff00) >> 8;
		bytep[2] = (sample24 & 0xff0000) >> 16;
	}
	else
	{
		bytep[2] = sample24 & 0xff;
		bytep[1] = (sample24 & 0xff00) >> 8;
		bytep[0] = (sample24 & 0xff0000) >> 16;
	}	
}

bool aes67Callback(void *data, void *audioPtr, unsigned int numBytes, uint32_t timeStamp)
{
	unsigned int writeCount;
	CallBackData *callBackData = (CallBackData *)data;

	// Sanity check
	if ((numBytes == 0) || !callBackData->streamActive)
	{
		return(callBackData->streamActive);
	}

	// Now conversion done here
	// Conversion to machine order is performed before callBack
	// Always write wave file in machine order for simplicity
	// Format is set accordingly

	if (numBytes > callBackData->bytesRemaining)
	{
		numBytes = callBackData->bytesRemaining;
	}

	writeCount = callBackData->waveFile->writeRaw(audioPtr, numBytes);
	if (writeCount != numBytes)
	{
		throw runtime_error("Error writing to wavefile");
	}
	callBackData->bytesRemaining -= numBytes;
	callBackData->streamActive = (callBackData->bytesRemaining > 0);
	return(callBackData->streamActive);
}

bool smpte2110_41Callback(void *data, void *metadataPtr, unsigned int numBytes, uint32_t timeStamp)
{
	unsigned int bytesWritten;
	CallBackData *callBackData = (CallBackData *)data;

	bytesWritten = fwrite(metadataPtr, 1, numBytes, callBackData->metadataFile);
	if (bytesWritten != numBytes)
	{
		throw runtime_error("Error writing to metadata file");		
	}
	callBackData->timeRemaining -= callBackData->packetTime;
	callBackData->streamActive = (callBackData->timeRemaining > 0);

	return(callBackData->streamActive);
}

bool inSignalHandler = false;

void ShutDown(int status)
{
	LOG(INFO) << "Shutdown Complete";
	exit(status);
}

AoipServices *aoipServices = nullptr;



void SignalHandler( int signum )
{
	inSignalHandler = true;
	LOG(INFO) << "Interrupt signal (" << signum << ") received.";
	//printSleepTimes();

	if (aoipServices)
	{
		LOG(INFO) << "Tearing down Services";		
		delete aoipServices;
	}

	ShutDown(signum);
}


int main(int argc, char *argv[])
{
#ifdef RTP_SOCKETTYPE_WINSOCK
	WSADATA dat;
	WSAStartup(MAKEWORD(2,2),&dat);
#endif // RTP_SOCKETTYPE_WINSOCK
	
	int status,i;
	std::string tmpStr;
	UserInfo userInfo;
	unsigned int sleepComplete = 0;
	CallBackData callBackData;
	ST2110ReceiverCallBackInfo callBackInfo;

	signal(SIGINT, SignalHandler);  

	InitLogging(argc, argv, "");

	// Single system and single stream
	AoipSystem system;
	StreamInfo streamInfo;
	// User Defaults, employed if options are missing
	userInfo.interface.assign("enp3s0f0"); // First Mellanox device name by default on an Ubuntu system
	userInfo.recordTime = 10.0; // seconds
	userInfo.latency = 0.5; // 500ms
	userInfo.bitDepth = 0; // follow input
	userInfo.blockSize = 512;
	userInfo.ptpDomain = 0;

	try{

		for (i = 1; i < argc; i++)
		{
			if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "-help") || !strcmp(argv[i], "--help"))
			{
				print_usage();
				ShutDown(0);
			}

			if (!strcmp(argv[i], "-o"))
			{
				if (i == (argc - 1))
				{
					cerr << "Error: Can't find input filename" << endl;
					print_usage();
					ShutDown(-1);
				}
				userInfo.filename = argv[++i];
			}

			if (!strcmp(argv[i], "-if"))
			{
				if (i == (argc - 1))
				{
					cerr << "Error: Can't find interface name" << endl;
					print_usage();
					ShutDown(-1);
				}
				userInfo.interface.assign(argv[++i]);
			}

			if (!strcmp(argv[i], "-l"))
			{
				if (i == (argc - 1))
				{
					cerr << "Error: Con't find Latency" << endl;
					print_usage();
					ShutDown(-1);
				}
				userInfo.latency = atof(argv[++i]);
			}

			if (!strcmp(argv[i], "-b"))
			{
				if (i == (argc - 1))
				{
					cerr << "Error: Con't find Bit Depth" << endl;
					print_usage();
					ShutDown(-1);
				}
				userInfo.bitDepth = atoi(argv[++i]);
			}

			if (!strcmp(argv[i], "-bl"))
			{
				if (i == (argc - 1))
				{
					cerr << "Error: Con't find Block Size" << endl;
					print_usage();
					ShutDown(-1);
				}
				userInfo.blockSize = atoi(argv[++i]);
			}


			if (!strcmp(argv[i], "-N"))
			{
				if (i == (argc - 1))
				{
					cerr << "Error: Can't find stream name" << endl;
					print_usage();
					ShutDown(-1);
				}
				userInfo.streamName = argv[++i];
			}

			if (!strcmp(argv[i], "-t"))
			{
				if (i == (argc - 1))
				{
					cerr << "Error: Can't find record time" << endl;
					print_usage();
					ShutDown(-1);
				}			
				userInfo.recordTime = strtod(argv[++i], NULL);
			}

			if (!strcmp(argv[i], "-D"))
			{
				if (i == (argc - 1))
				{
					cerr << "Error: Can't find domain" << endl;
					print_usage();
					ShutDown(-1);
				}			
				userInfo.ptpDomain = strtod(argv[++i], NULL);
			}


		}
		
		if ((userInfo.bitDepth != 0) && 
			(userInfo.bitDepth != 16) &&
			(userInfo.bitDepth != 24) &&
			(userInfo.bitDepth != 32))
		{
			throw runtime_error("Invalid Bit Depth, Must be 16,24 or 32 bits");
		}

		struct sched_param sp;

		sp.sched_priority = sched_get_priority_max(SCHED_FIFO);
		status = sched_setscheduler(0, SCHED_FIFO, &sp);
		if (status == -1)
		{
	  		LOG(WARNING) << "Failed to set priority to MAX/SCHED_FIFO - Need to be root";
		}

		// First task is to obtain details about the stream to be received via discovery
		// This could take up to 30s if SAP only so timeout is required.

		// If stream name not specified then use a simple default
		if (userInfo.streamName.empty())
		{
			userInfo.streamName = "recorder-default";
		}

		LOG(INFO) << "Obtaining Session Description for Stream " << userInfo.streamName;
		LOG(INFO) << "Please Wait. This can take up to 30s";
		system.samplingFrequency = 48000;
		system.name = "dlb_recorder";

		AoipPort interface;
		interface.interfaceName = userInfo.interface;
		system.mediaInterface = interface;
		system.manageInterface = interface;
		if (userInfo.ptpDomain > 127)
		{
			LOG(FATAL) << "Invalid PTP domain...";	
		}
		system.domain = userInfo.ptpDomain;
		unsigned int services = AOIP_SERVICE_RAVENNA | AOIP_SERVICE_SAP | AOIP_SERVICE_NMOS;

		// Don't use callback for simplicity
		AoipServices::CallBacks callBacks;

		callBacks.newRxServiceCallBack = nullptr;
		callBacks.connectionReqCallBack = nullptr;

		aoipServices = new AoipServices(system, services, callBacks);
		float timeLeft = 30.0; // allow 30s for discovery before timeout
		userInfo.service = nullptr;
		ConsoleSpinner spinner;
		while((sleepComplete == 0) && !userInfo.service && (timeLeft > 0.0))
		{
			sleepComplete = sleep(1);
			vector<AoipService>& rxStreams = aoipServices->GetAvailableServicesForRx();
			for (vector<AoipService>::iterator stream = rxStreams.begin() ; stream != rxStreams.end() ; stream++)
			{
				if (stream->GetName() == userInfo.streamName)
				{
					userInfo.service = &(*stream);
					break;
				}
			}
			spinner.turn();
			timeLeft--;;
		}
		spinner.clear();
		LOG(INFO);
		if (sleepComplete != 0)
		{
			LOG(FATAL) << "Break detected...";
			ShutDown(-1);
		}
		if (!userInfo.service)
		{
			LOG(FATAL) << "Stream not found...Terminating";
			ShutDown(-2);
		}
		// Just use first service, ignore rest for now
		// Service uses copy so application owns service and streamInfo
		// Make another copy so we have record of what was provided as well as changes made
		// for reception
		streamInfo = userInfo.service->GetStreamInfo();

		LOG(INFO) << "***Created Input Stream " << streamInfo.streamName << "***";
		if (streamInfo.streamType == SMPTE2110_41)
		{
			LOG(INFO) << "Stream Type: SMPTE 2110-41";
			LOG(INFO) << "Packet Time (ms) " << streamInfo.metadata.packetTimeMs;
			LOG(INFO) << "Maximum Payoad size (Bytes): " << streamInfo.metadata.maxPayloadSizeBytes;
			// only capable of transmitting one DIT so we can cheat by using front here. Will need a loop
		}
		else
		{
			if (streamInfo.streamType == AES67)
			{
				LOG(INFO) << "Stream Type: AES67";
			}
			else if (streamInfo.streamType == AM824)
			{
				LOG(INFO) << "Stream Type: AM824";
			}
			LOG(INFO) << "Number of Channels: " << streamInfo.audio.numChannels;
			LOG(INFO) << "Samples per packet: " << streamInfo.audio.samplesPerPacket;
			LOG(INFO) << "Input stream bit-depth: " << streamInfo.audio.payloadBytesPerSample * 8 << " bits";
		}
		LOG(INFO) << "Latency (ms) " << userInfo.latency * 1000; // latency is in seconds
		LOG(INFO) << "Blocksize (samples) " << userInfo.blockSize;
		LOG(INFO) << "MCast IP Address: " << streamInfo.dstIpStr;

		if (streamInfo.samplingFrequency != 48000)
		{
			LOG(FATAL) << "Unsupported Sampling Frequency: " << streamInfo.samplingFrequency;
			LOG(FATAL) << "Only 48kHz supported";
			ShutDown(-3);
		}

		// Set missing streamInfo from user selection 
		streamInfo.latency = userInfo.latency;
		// No output filename provide so construct default
		if (userInfo.filename.empty())
		{
			userInfo.filename = "record-" + userInfo.filename + streamInfo.streamName;
			if ((streamInfo.streamType == AM824) || (streamInfo.streamType == AES67))
			{
				userInfo.filename += ".wav";				
			}
			else
			{
				userInfo.filename += ".dat";				
			}
		}

		int format = SF_FORMAT_WAV;

		// Declare stream object
		switch (streamInfo.streamType)
		{
		case AM824:
		case AES67:
			if (userInfo.bitDepth == 0)
			{
				callBackInfo.audioFormat = (AoipAudioFormat) streamInfo.audio.payloadBytesPerSample;
			}
			else
			{
				callBackInfo.audioFormat = (AoipAudioFormat) (unsigned int)(userInfo.bitDepth / 8);
			}

			switch (callBackInfo.audioFormat)
			{
			case DLB_AOIP_AUDIO_FORMAT_16BIT_LPCM:
				format |= SF_FORMAT_PCM_16;
				break;
			case DLB_AOIP_AUDIO_FORMAT_24BIT_LPCM:
				format |= SF_FORMAT_PCM_24;
				break;
			case DLB_AOIP_AUDIO_FORMAT_32BIT_LPCM:
				format |= SF_FORMAT_PCM_32;
				break;
			default:
				LOG(FATAL) << "Error: Stream has invalid bitdepth";
				ShutDown(-4);				
			}
			// Wavefile always follows machine order
			// Find out what machine order is and set wavfile accordingly
			// This allows raw writes with no conversion
			if (ST2110Hardware::IsLittleEndian())
			{
				format |= SF_ENDIAN_LITTLE;
			}
			else
			{
				format |= SF_ENDIAN_BIG;
			}

			callBackData.waveFile = new SndfileHandle(userInfo.filename, SFM_WRITE, format, streamInfo.audio.numChannels, 48000) ;
			if (callBackData.waveFile->error())
			{
				LOG(FATAL) << "Error opening output wave file: " << userInfo.filename;
				ShutDown(-5);
			}
			callBackData.bytesRemaining = round(userInfo.recordTime * streamInfo.samplingFrequency * streamInfo.audio.numChannels * streamInfo.audio.payloadBytesPerSample);
			callBackInfo.callBack = aes67Callback;
			callBackInfo.blockSize = userInfo.blockSize;
			callBackInfo.data = &callBackData;
			aoipServices->AddRxStream(streamInfo, callBackInfo);
			break;

		case SMPTE2110_41:
			callBackData.metadataFile = fopen(userInfo.filename.c_str(),"w");
			if (!callBackData.metadataFile)
			{				
				LOG(FATAL) << "Error: Can't open Stream data output file";
				ShutDown(-6);
			}
			callBackData.timeRemaining = userInfo.recordTime;
			callBackData.packetTime = streamInfo.metadata.packetTimeMs / 1000;
			callBackInfo.callBack = smpte2110_41Callback;
			callBackInfo.blockSize = userInfo.blockSize;
			callBackInfo.data = &callBackData;
			aoipServices->AddRxStream(streamInfo, callBackInfo);
			break;
		default:

			LOG(FATAL) << "Error: Unsupported stream type";
			ShutDown(-7);
		}

		callBackData.streamActive = true;

		// Bring up aoip services
		// Locking the entire system at 48kHz for now
		// Last too pointers are callbacks for received streams. As this is a player
		// application there is no need to know about streams available for reception

		aoipServices->StartRxStream(streamInfo.streamName);

		while(callBackData.streamActive)
		{
			using namespace std::literals::chrono_literals;
			// Check for stream termination once a second
			sleep(1);
			spinner.turn();
		}
		spinner.clear();
		LOG(INFO) << "Shutting Down...";
		if (callBackData.metadata)
		{
			delete[] callBackData.metadata;
		}
		if (callBackData.metadataFile)
		{
			fclose(callBackData.metadataFile);
		}
    }

	catch(runtime_error& e)
	{
	  	LOG(INFO) << e.what();
		LOG(FATAL) << "*** Runtime Error Exception ***";
	  	if (!inSignalHandler)
	  	{
	  		ShutDown(-1);
	  	}
	  	else
	  	{
	  		while(1);
	  	}
	}

	return 0;
}

