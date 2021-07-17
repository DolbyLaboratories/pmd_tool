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

/*** Logging */
#include "dlb_st2110_logging.h"

using namespace std;

/************************* Constants ***************************/

#define MAX_FILENAME_SIZE 256
#define MAX_LABEL_SIZE 256
#define VERSION 0.9


/********************** Type Defs *****************************/

typedef struct userInfo
{
	std::string interface;
	std::string dstIpStr;
	uint32_t numPackets;
	char waveFilename[MAX_FILENAME_SIZE];
	char name[MAX_LABEL_SIZE];
	char streamTypeString[MAX_LABEL_SIZE];
	char metadataFilename[MAX_FILENAME_SIZE];
	double volumeDB;
	unsigned int smpte2110standard;
	uint32_t dataItemType;
	float latency;
	unsigned int blockSize;
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
};


void print_usage(void)
{
	fprintf(stderr, "dlb_2110_main -i <INPUT FILE> -if <INTERFACE> -di <DEST IP ADDRESS> -n <NUM PACKETS> -N <NAME> -l <LATENCY> -b <BLOCK SIZE> -smpte2110-<STANDARD> -volume <VOLUME> -dit <DATA_ITEM_TYPE>v%f\n", VERSION);
	fprintf(stderr, "Copyright Dolby Laboratories Inc., 2021. All rights reserved.\n\n");
	fprintf(stderr, "<INPUT FILE>              Filename of input file (WAV file for -30/-31, binary for -41)\n");
	fprintf(stderr, "                          A generator with a test pattern is used if no filename is supplied\n");
	fprintf(stderr, "<INTERFACE>               Interface to send streams on\n");
	fprintf(stderr, "<DEST IP ADDRESS>         Ip Address of destination  (239.150.150.1 by default)\n");
	fprintf(stderr, "<NUM PACKETS>             Number of packets to be sent (0 - forever) by default)\n");
	fprintf(stderr, "<NAME>                    Name of the stream to be advertised via SAP\n");
	fprintf(stderr, "<LATENCY>                 Transmit latency in seconds to be used (0.5 by default).\n");
	fprintf(stderr, "<BLOCK SIZE>              Size of audio blocks in samples between application and driver (512 by default).\n");
	fprintf(stderr, "<STANDARD>                SMPTE 2110 standard to be followed when creating stream (30/31/41) (30) by default)\n");
	fprintf(stderr, "<VOLUME>                  Volume in dBs to be used for playback. Only for -30. (0 by default)\n");
	fprintf(stderr, "<DATA ITEM TYPE>          Data Item Type to be used in Hex. Only used for -41 (0x3FF000 by default)\n");
}


// For now, share data with callbacks via globals
// This needs to be fixed via a structure and opaque pointer
struct CallBackData
{
	SndfileHandle *waveFile = nullptr;
	bool needEndSwap = false;
	unsigned char *metadata = nullptr;
	unsigned int metadataSize = 0;
	bool streamActive = false;
	unsigned int bytesPerSample = 0;
	float volume = 1.0;
};

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

bool aes67Callback(void *data, void *audioPtr, unsigned int numBytes, bool& haveTimeStamp, uint32_t& timeStamp)
{
	unsigned char *packetData = (unsigned char *)audioPtr;
	unsigned int readCount, j;
	uint8_t tmpByte;
	CallBackData *callBackData = (CallBackData *)data;

	readCount = callBackData->waveFile->readRaw(audioPtr, numBytes);
	/* This function is responsible for getting the data into machine order */
	/* The stream libary will detect if it is running on a little endian */
	/* machine and convert to big endian for the network as necessary */
	if ((readCount > 0) && (callBackData->needEndSwap || (callBackData->volume != 1.0)))
	{
		if (callBackData->bytesPerSample == 2)
		{
			for (j = 0 ; j < readCount-1 ; j+= 2)
			{
				if (callBackData->needEndSwap)
				{
					tmpByte = packetData[j];
					packetData[j] = packetData[j+1];
					packetData[j+1] = tmpByte;
				}
				*((int16_t *)&packetData[j]) = *((int16_t *)&packetData[j]) * callBackData->volume;
			}
		}
		else if (callBackData->bytesPerSample == 3)
		{
			for (j = 0 ; j < readCount-1 ; j+= 3)
			{
				if (callBackData->needEndSwap)
				{
					tmpByte = packetData[j];
					packetData[j] = packetData[j+2];
					packetData[j+2] = tmpByte;
				}
				writeSample24(&packetData[j], readSample24(&packetData[j]) * callBackData->volume);				
			}
		}		
		else
		{
			throw runtime_error("Unsupported bytes per Sample in callback");
		}
	}

	// If we wern't able to read any more then clear the rest of the buffer with zeros
	// This avoids a snat at the end of the file due to stale data
	if (readCount < numBytes)
	{
		memset(&packetData[readCount], 0, numBytes - readCount);
	}

	// Return true to continue stream
	// False will terminate
	callBackData->streamActive = (readCount == numBytes);
	haveTimeStamp = false;
	return(callBackData->streamActive);
}

bool smpte2110_41Callback(void *data, void *metadataPtr, unsigned int numBytes, bool& haveTimeStamp, uint32_t& timeStamp)
{
	unsigned int bytesToRead;
	CallBackData *callBackData = (CallBackData *)data;

	if (callBackData->metadataSize > 0)
	{
		if (callBackData->metadataSize > numBytes)
		{
			bytesToRead = numBytes;
		}
		else
		{
			bytesToRead = callBackData->metadataSize;
		}

	}
	memcpy(metadataPtr, callBackData->metadata, bytesToRead);
	haveTimeStamp = false;
	return(true); // run until ^C
}

bool inSignalHandler = false;

/* Anything that needs to be freed in the signal handler to ensure smooth ShutDown */
/* because of a Sig-Int needs be defined here so the handler can see it */
/* The crucial thing is not to take down the aoipSystem (incl. rmax) */
/* before stopping all the threads */
/* This should properly close all the sockets too */

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
	FILE *metadataFile = nullptr;
	uint32_t waveFormat;
	UserInfo userInfo;

	signal(SIGINT, SignalHandler);  

	InitLogging(argc, argv, "");

	// Single aoipSystem and single stream
	AoipSystem aoipSystem;
	StreamInfo streamInfo;
	// User Defaults, employed if options are missing
	userInfo.dstIpStr.assign("239.150.150.1");
	userInfo.interface.assign("enp3s0f0"); // First Mellanox device name by default on an Ubuntu aoipSystem
	userInfo.numPackets = 0; //Forever
	userInfo.volumeDB = 0.0;
	userInfo.latency = 0.5;
	userInfo.blockSize = 512;
	strcpy(userInfo.waveFilename,"");
	strcpy(userInfo.metadataFilename,"");
	strcpy(userInfo.name,"");
	userInfo.smpte2110standard = 30; //AES67
	userInfo.dataItemType = 0x3FF000; // First experimental data item type as per March 2020 Draft of SMPTE 2110-41
	ST2110TransmitterCallBackInfo callBackInfo;
	CallBackData callBackData;

	MClock::CheckTaiOffset();

	try{

		for (i = 1; i < argc; i++)
		{
			if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "-help") || !strcmp(argv[i], "--help"))
			{
				print_usage();
				ShutDown(0);
			}

			if (!strcmp(argv[i], "-i"))
			{
				if (i == (argc - 1))
				{
					fprintf(stderr, "Error: Can't find input filename\n");
					print_usage();
					ShutDown(-1);
				}
				if (strstr(argv[i+1],".wav"))
				{
					strncpy(userInfo.waveFilename, argv[++i], MAX_FILENAME_SIZE);
				}
				else
				{
					strncpy(userInfo.metadataFilename, argv[++i], MAX_FILENAME_SIZE);				
				}
			}

			if (!strcmp(argv[i], "-if"))
			{
				if (i == (argc - 1))
				{
					fprintf(stderr, "Error: Can't find interface name\n");
					print_usage();
					ShutDown(-1);
				}
				userInfo.interface.assign(argv[++i]);
			}

			if (!strcmp(argv[i], "-di"))
			{
				if (i == (argc - 1))
				{
					fprintf(stderr, "Error: Can't find destination ip address\n");
					print_usage();
					ShutDown(-1);
				}
				userInfo.dstIpStr.assign(argv[++i]);
			}

			if (!strcmp(argv[i], "-l"))
			{
				if (i == (argc - 1))
				{
					fprintf(stderr, "Error: Latency\n");
					print_usage();
					ShutDown(-1);
				}
				userInfo.latency = atof(argv[++i]);
			}

			if (!strcmp(argv[i], "-b"))
			{
				if (i == (argc - 1))
				{
					fprintf(stderr, "Error: Blocksize\n");
					print_usage();
					ShutDown(-1);
				}
				userInfo.blockSize = atoi(argv[++i]);
			}

			if (!strcmp(argv[i], "-n"))
			{
				if (i == (argc - 1))
				{
					fprintf(stderr, "Error: Number of packets to transmit\n");
					print_usage();
					ShutDown(-1);
				}
				userInfo.numPackets = atoi(argv[++i]);
			}

			if (!strcmp(argv[i], "-N"))
			{
				if (i == (argc - 1))
				{
					fprintf(stderr, "Error: Can't find stream name\n");
					print_usage();
					ShutDown(-1);
				}
				strncpy(userInfo.name, argv[++i], MAX_LABEL_SIZE);
			}

			if (!strcmp(argv[i], "-volume"))
			{
				if (i == (argc - 1))
				{
					fprintf(stderr, "Error: Can't find stream type\n");
					print_usage();
					ShutDown(-1);
				}			
				userInfo.volumeDB = strtod(argv[++i], NULL);
			}

			if (!strcmp(argv[i], "-smpte2110-30"))
			{
				userInfo.smpte2110standard = 30;
			}

			if (!strcmp(argv[i], "-smpte2110-31"))
			{
				userInfo.smpte2110standard = 31;
			}

			if (!strcmp(argv[i], "-smpte2110-41"))
			{
				userInfo.smpte2110standard = 41;
			}

			if (!strcmp(argv[i], "-dit"))
			{
				if (i == (argc - 1))
				{
					fprintf(stderr, "Error: Can't find stream type\n");
					print_usage();
					ShutDown(-1);
				}			
				userInfo.dataItemType = std::stoi(argv[++i], nullptr, 16); // Input is hex so use stoi from STL
			}

		}

		// std::cout << rmax_get_version_string();
		
		struct sched_param sp;

		sp.sched_priority = sched_get_priority_max(SCHED_FIFO);
		status = sched_setscheduler(0, SCHED_FIFO, &sp);
		if (status == -1)
		{
	  		printf("Failed to set priority to MAX/SCHED_FIFO - Need to be root\n");
		}

		// Read wavefile into memory
		if (strlen(userInfo.waveFilename) > 0)
		{
			callBackData.waveFile = new SndfileHandle(userInfo.waveFilename);
			if (callBackData.waveFile->error())
			{
				fprintf(stderr, "Error opening input wave file: %s\n", userInfo.waveFilename);
				ShutDown(-3);	
			}

			if (callBackData.waveFile->samplerate() != 48000)
			{
				fprintf(stderr, "Input wavefile sampling rate is %u, Only 48kHz supported\n", callBackData.waveFile->samplerate());
				ShutDown(-3);
			}

			// Determine if endswap is needed
			// Must be in big endian format in the RTP packet
			// note using != as logically equivilient to XOR
			// IF big endian and on big endian machine then no endswap etc.
			callBackData.needEndSwap = callBackData.waveFile->command (SFC_RAW_DATA_NEEDS_ENDSWAP , NULL, 0);

			waveFormat = callBackData.waveFile->format();
			if (((waveFormat & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) &&
				((waveFormat & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAVEX) &&
				((waveFormat & SF_FORMAT_TYPEMASK) != SF_FORMAT_RF64))
			{
				fprintf(stderr, "Input file is not a wavefile %x\n", waveFormat);
				ShutDown(-4);

			}

			if (((waveFormat & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) &&
				((waveFormat & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_24) &&
				((waveFormat & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_32))
			{
				fprintf(stderr,"Only 16, 24 and 32 bit LPCM wav format supported\n");
				ShutDown(-5);		
			}
		}
		if (strlen(userInfo.name) == 0)
		{
			// set the name based on the tool version and stream type
			sprintf(userInfo.name,"dlb-2110-%u-%1.1f", userInfo.smpte2110standard, VERSION);
		}

		// Set stream type based on user selection
		if (userInfo.smpte2110standard == 30)
		{
			streamInfo.streamType = AES67;
		}
		else if (userInfo.smpte2110standard == 31)
		{
			// Could add an if here to support AM824 generation
			streamInfo.streamType = AM824;
		}
		else if (userInfo.smpte2110standard == 41)
		{
			streamInfo.streamType = SMPTE2110_41;
		}
		else
		{
			print_usage();
			throw runtime_error("Error: Unknown stream type requested");
		}

		// Bring up aoip services
		// Locking the entire aoipSystem at 48kHz for now
		// Last too pointers are callbacks for received streams. As this is a player
		// application there is no need to know about streams available for reception

		aoipSystem.interface = userInfo.interface;
		aoipSystem.samplingFrequency = 48000;
		aoipSystem.name = "dlb_player";
		aoipServices = new AoipServices(aoipSystem, nullptr);
		// allow time for services to come up before starting stream 
		sleep(1);

		streamInfo.streamName = std::string(userInfo.name);
		// Audio only settings 
		if (streamInfo.streamType != SMPTE2110_41)
		{
			if (callBackData.waveFile)
			{
				streamInfo.audio.numChannels = callBackData.waveFile->channels();

				if ((callBackData.waveFile->format() & SF_FORMAT_SUBMASK) == SF_FORMAT_PCM_16)
				{
					streamInfo.audio.payloadBytesPerSample = 2;
					callBackInfo.audioFormat = DLB_AOIP_AUDIO_FORMAT_16BIT_LPCM;
					callBackData.bytesPerSample = 2;
				}
				else if ((callBackData.waveFile->format() & SF_FORMAT_SUBMASK) == SF_FORMAT_PCM_24)
				{
					streamInfo.audio.payloadBytesPerSample = 3;
					callBackInfo.audioFormat = DLB_AOIP_AUDIO_FORMAT_24BIT_LPCM;
					callBackData.bytesPerSample = 3;
				}
				else if ((callBackData.waveFile->format() & SF_FORMAT_SUBMASK) == SF_FORMAT_PCM_32)
				{
					streamInfo.audio.payloadBytesPerSample = 3; // Not L32 not supported by AES67 / -30
					callBackInfo.audioFormat = DLB_AOIP_AUDIO_FORMAT_32BIT_LPCM;
					callBackData.bytesPerSample = 4;
				}
				else
				{
					throw runtime_error("Error: Unsupported format");
				}

			}
			else
			{
				// Default to 2, 16 bits for generator for now
				// Add an optional switch later
				streamInfo.audio.numChannels = 2;
				callBackData.bytesPerSample = 2;
				streamInfo.audio.payloadBytesPerSample = 2;
			}
			if (streamInfo.audio.numChannels < 9)
			{
				streamInfo.audio.samplesPerPacket = 48; // 2110-30/31 class A		
			}
			else
			{
				streamInfo.audio.samplesPerPacket = 6; // 2110-30/31 class C	
			}
		}
		streamInfo.samplingFrequency = aoipSystem.samplingFrequency;
		streamInfo.payloadType = 98; // Dynamic payloads only 96 - 127
		streamInfo.dstIpStr = userInfo.dstIpStr;
		streamInfo.port = 0; // Select for aoipSystem to select Port
		streamInfo.latency = userInfo.latency;

		callBackInfo.blockSize = userInfo.blockSize;
		callBackInfo.data = &callBackData;

		// Declare stream object
		switch (streamInfo.streamType)
		{

		case AM824:
			if (userInfo.volumeDB != 0.0)
			{
				throw runtime_error("Volume not supported with AM824 streams");
			}
		case AES67:
			if (userInfo.volumeDB == 0.0)
			{
				callBackData.volume = 1.0;
			}
			else
			{
				callBackData.volume = pow(10.0,userInfo.volumeDB / 20.0);
			}
			if (callBackData.waveFile)
			{
				/* set up callback */
				callBackInfo.callBack = aes67Callback;
				aoipServices->AddTxStream(streamInfo, &callBackInfo);
			}
			else
			{
				aoipServices->AddTxStream(streamInfo);
			}
			break;

		case SMPTE2110_41:
			metadataFile = fopen(userInfo.metadataFilename,"r");
			if (!metadataFile)
			{
				throw runtime_error("Error: Metadata file not found");
			}
			fseek(metadataFile, 0L, SEEK_END);
			callBackData.metadataSize = ftell(metadataFile);
			rewind(metadataFile);
			if (callBackData.metadataSize == 0)
			{
				throw runtime_error("Error: Reading Metadata file");
			}
			callBackData.metadata = new unsigned char [callBackData.metadataSize];
			fread(callBackData.metadata, 1,  callBackData.metadataSize, metadataFile);
			fclose(metadataFile);
			// Signal to stream object how much memory we need for 
			streamInfo.metadata.maxPayloadSizeBytes = callBackData.metadataSize;
			streamInfo.metadata.packetTimeMs = 40.0; // 25fps
			streamInfo.metadata.dataItemTypes.push_back(userInfo.dataItemType);
			callBackInfo.callBack = smpte2110_41Callback;
			aoipServices->AddTxStream(streamInfo, &callBackInfo); // Blocksize not used
			break;
		default:
			throw runtime_error("Error: Unsupported stream type");
		}

		callBackData.streamActive = true;
		aoipServices->StartTxStream(streamInfo.streamName);
		unsigned int sleepComplete = 0;

		ConsoleSpinner spinner;
		LOG(INFO) << "Playing " << streamInfo.streamName << "...";

		while(callBackData.streamActive && (sleepComplete == 0))
		{
			// Check for stream termination once a second
			sleepComplete = sleep(1); // probably better to use nanosleep here
			//spinner.turn();
		}
		LOG(INFO) << "Preparing to Shut Down...";
		if (callBackData.metadata)
		{
			delete[] callBackData.metadata;
		}
		if (callBackData.waveFile)
		{
			delete callBackData.waveFile;
		}
		if (aoipServices)
		{
			LOG(INFO) << "Tearing down Services";		
			delete aoipServices;
		}
		ShutDown(0);
    }

	catch(runtime_error& e)
	{
		time_t timenow = chrono::system_clock::to_time_t(chrono::system_clock::now());  
	    cout << ctime(&timenow) << endl;

		cout << "*** Runtime Error Exception ***" << endl;
	  	cout << e.what() << "\n";
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

