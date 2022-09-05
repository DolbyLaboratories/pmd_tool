/************************************************************************
 * dlb_st2110
 * Copyright (c) 2020 - 2022, Dolby Laboratories Inc.
 * Copyright (c) 2022, Dolby International AB.
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


#include <iostream>
#include <string>
//#include <cstdio>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <array>
#include <mutex>
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
using namespace std::literals::chrono_literals;

/************************* Constants ***************************/

#define MAX_LABEL_SIZE 256
#define VERSION 0.9

#define MAX_INPUT_CHANNELS MAX_CHANNELS
#define MAX_OUTPUT_CHANNELS MAX_CHANNELS
#define MAX_COMP_MIX_MATRIX_SIZE (MAX_INPUT_CHANNELS * MAX_OUTPUT_CHANNELS)

/********************** Type Defs *****************************/

typedef float pmd_studio_mix_matrix_array[MAX_INPUT_CHANNELS][MAX_OUTPUT_CHANNELS];
typedef float pmd_studio_mix_matrix[][MAX_OUTPUT_CHANNELS];


/********************** Structures *****************************/

struct pmd_studio_comp_mix_matrix
{
    unsigned int size;
    unsigned int input[MAX_COMP_MIX_MATRIX_SIZE];
    unsigned int output[MAX_COMP_MIX_MATRIX_SIZE];
    float coef[MAX_COMP_MIX_MATRIX_SIZE];
};

struct UserInfo
{
	string interface = "enp3s0f0";
	string dstIpStr = "239.150.150.1";
	string inputName;
	string outputName;
	string outputStreamTypeString;
	unsigned int outputSmpte2110standard = 0; // Auto selection
	unsigned int numChannels = 0; // Auto selection
	unsigned int bitDepth = 0; // Auto selection
	float latency = 1.0;
	unsigned int blockSize = 0;
	const AoipService *inputService  = nullptr;
	unsigned int callBackBitDepth = 32;
};


struct CallBackData
{
	StreamInfo streamInfo;
	struct pmd_studio_comp_mix_matrix comp_mix_matrix;
	ST2110TransceiverCallBackInfo callBackInfo;
};

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


static
void
pmd_studio_mix_matrix_reset
	(pmd_studio_mix_matrix mix_matrix)
{
	unsigned int i,j;
	for (i = 0 ; i < MAX_INPUT_CHANNELS ; i++)
	{
		for (j = 0 ; j < MAX_OUTPUT_CHANNELS ; j++)
		{
			mix_matrix[i][j] = 0.0;
		}
	}
}

static
void
pmd_studio_mix_matrix_unity
    (pmd_studio_mix_matrix mix_matrix)
{
    unsigned int i,j;
    for (i = 0 ; i < MAX_INPUT_CHANNELS ; i++)
    {
        for (j = 0 ; j < MAX_OUTPUT_CHANNELS ; j++)
        {
            if (i == j)
            {
                mix_matrix[i][j] = 1.0;
            }
            else
            {
                mix_matrix[i][j] = 0.0;
            }
        }
    }
}

static
void
compress_mix_matrix(pmd_studio_mix_matrix_array mix_matrix, struct pmd_studio_comp_mix_matrix *comp_mix_matrix, unsigned int channels)
{
    unsigned int i,j;

    comp_mix_matrix->size = 0;

    // if(channels > MAX_CHANNELS || channels < 0){
    //     throw std::length_error("[ER] pmd_studio_device compress_mix_matrix - Invalid channel value.");
    // }

    for (i = 0 ; i < channels ; i++)
    {
        for (j = 0 ; j < channels ; j++)
        {
            if (mix_matrix[i][j] != 0.0)
            {
                comp_mix_matrix->input[comp_mix_matrix->size] = i;
                comp_mix_matrix->output[comp_mix_matrix->size] = j;
                comp_mix_matrix->coef[comp_mix_matrix->size++] = mix_matrix[i][j];
                if (comp_mix_matrix->size == MAX_COMP_MIX_MATRIX_SIZE)
                {
                    break;
                }
            }
            if (comp_mix_matrix->size == MAX_COMP_MIX_MATRIX_SIZE)
            {
                break;
            }
        }
        if (comp_mix_matrix->size == MAX_COMP_MIX_MATRIX_SIZE)
        {
            break;
        }
    }
}

static
void
pmd_studio_mix_matrix_print
	(pmd_studio_mix_matrix mix_matrix)
{
	unsigned int i,j;

    printf("Outputs\tMix Matrix\n\t==== ======\n");
    printf("|\tInputs---->\n|\n|\nV\n   ");
    for (j = 0 ; j < MAX_INPUT_CHANNELS ; j++)
    {
        printf("%5u",j + 1);
    }
    printf("\n");
    for (i = 0 ; i < (MAX_INPUT_CHANNELS * 5) + 3 ; i++)
    {
        printf("-");
    }
    printf("\n");
    for (i = 0 ; i < MAX_OUTPUT_CHANNELS ; i++)
    {
        printf("%2u| ",i + 1);
        for (j = 0 ; j < MAX_INPUT_CHANNELS ; j++)
        {
            printf("%2.2f ", mix_matrix[j][i]);
        }
        printf("\n");
    }
}

void print_usage(void)
{
	cerr << "dlb_2110_mixer -i <INPUT NAME> -o <OUTPUT NAME> -if <INTERFACE> -di <DEST IP ADDRESS> -c <CHANNELS> -b <BIT DEPTH> -p <CALLBACK BIT DEPTH> -l <LATENCY> -bl <BLOCK SIZE> -smpte2110-<STANDARD> v" << VERSION << endl;
	cerr <<"Copyright Dolby Laboratories Inc., 2021. All rights reserved." << endl;
	cerr << "<INPUT NAME>              Name of input SMPTE ST2110-30/31 stream (default = dlb-2110-mixer-in)" << endl;
	cerr << "<OUTPUT NAME>             Name of output SMPTE ST2110-30/31 stream (default = dlb-2110-mixer-out)" << endl;
	cerr << "<INTERFACE>               Interface to use for stream reception and transmission" << endl;
	cerr << "<DEST IP ADDRESS>         Ip Address of destination  (239.150.150.1 by default)" << endl;
	cerr << "<CHANNELS>                Number of output channels (silence if more than input)" << endl;
	cerr << "<BIT DEPTH>               Bit depth of output stream 16/24 (16 by default, SMPTE ST2110-30 only)" << endl;
	cerr << "<CALLBACK BIT DEPTH>      Bit depth used by callbacks (32 bu default)" << endl;
	cerr << "<LATENCY>                 Latency in seconds between input and output streams (0.5 by default)" << endl;
	cerr << "<BLOCK SIZE>              Size of audio blocks in samples between application and driver (512 by default)" << endl;
	cerr << "<STANDARD>                SMPTE 2110 standard to be followed when creating stream, 30/31 (30 by default)" << endl;
}



// This global is required because we don't have opaque pointers implemented for callbacks.
AoipServices *aoipServices = nullptr;

unsigned int outputCallBackCounter = 0;


bool RxTxCallback(void *data, void *inputAudio, void *outputAudio, unsigned int inputBytes, unsigned int outputBytes)
{
	CallBackData *callBackData = (CallBackData *)data;
	unsigned int numSamples;
	int32_t *readPtr = (int32_t *)inputAudio;
	int32_t *writePtr = (int32_t *)outputAudio;

	// streamInfo is input StreamInfo

	numSamples = inputBytes / ((GetAoipBytesPerSample(callBackData->callBackInfo.audioFormat)) * callBackData->streamInfo.audio.numChannels);

//	memcpy(outputAudio, inputAudio, numBytes);
//	return(true);

    // First run mixing matrix in compressed form
    for( unsigned int i = 0; i < numSamples ; i++ )
    {
        for (unsigned int j = 0 ; j < callBackData->streamInfo.audio.numChannels ; j++)
        {
            writePtr[j] = 0;
        }
        for (unsigned int j = 0 ; j < callBackData->comp_mix_matrix.size ; j++)
        {
            writePtr[callBackData->comp_mix_matrix.output[j]] += readPtr[callBackData->comp_mix_matrix.input[j]] * callBackData->comp_mix_matrix.coef[j]; /* TODO: clarify mixed-types arithmetic -- where do we truncate? */
        }
        writePtr += callBackData->streamInfo.audio.numChannels;
        readPtr += callBackData->streamInfo.audio.numChannels;
    }

	return(true);
}

bool inSignalHandler = false;

/* Anything that needs to be freed in the signal handler to ensure smooth ShutDown */
/* because of a Sig-Int needs be defined here so the handler can see it */
/* The crucial thing is not to take down the aoipSystem (incl. rmax) */
/* before stopping all the threads */
/* This should properly close all the sockets too */

void ShutDown(int status)
{
	if (aoipServices)
	{
		LOG(INFO) << "Tearing down Services";		
		delete aoipServices;
	}

	LOG(INFO) << "Shutdown Complete";
	exit(status);
}




void SignalHandler( int signum )
{
	inSignalHandler = true;
	LOG(INFO) << "Interrupt signal (" << signum << ") received.";
	//printSleepTimes();

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
	bool success;
	pmd_studio_mix_matrix_array mix_matrix;
	CallBackData callBackData;
	ST2110TransceiverCallBackInfo callBackInfo;

	signal(SIGINT, SignalHandler);  

	InitLogging(argc, argv, "");

	// Single aoipSystem and single stream
	AoipSystem aoipSystem;
	StreamInfo inputStreamInfo;
	StreamInfo outputStreamInfo;
	// User Defaults, employed if options are missing

	pmd_studio_mix_matrix_unity(mix_matrix);
    //pmd_studio_mix_matrix_print(mix_matrix);
    compress_mix_matrix(mix_matrix, &callBackData.comp_mix_matrix, MAX_CHANNELS);

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
					cerr << "Error: Can't find input Stream Name" << endl;
					print_usage();
					ShutDown(-1);
				}
				userInfo.inputName = string(argv[++i]);				
			}

			if (!strcmp(argv[i], "-o"))
			{
				if (i == (argc - 1))
				{
					cerr << "Error: Can't find output Stream Name" << endl;
					print_usage();
					ShutDown(-1);
				}
				userInfo.outputName = string(argv[++i]);				
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

			if (!strcmp(argv[i], "-di"))
			{
				if (i == (argc - 1))
				{
					cerr << "Error: Can't find destination ip address" << endl;
					print_usage();
					ShutDown(-1);
				}
				userInfo.dstIpStr.assign(argv[++i]);
			}

			if (!strcmp(argv[i], "-c"))
			{
				if (i == (argc - 1))
				{
					cerr << "Error: Can't find number of channels" << endl;
					print_usage();
					ShutDown(-1);
				}
				userInfo.numChannels = atoi(argv[++i]);
			}

			if (!strcmp(argv[i], "-b"))
			{
				if (i == (argc - 1))
				{
					cerr << "Error: Can't find bit depth" << endl;
					print_usage();
					ShutDown(-1);
				}
				userInfo.bitDepth = atoi(argv[++i]);
			}

			if (!strcmp(argv[i], "-p"))
			{
				if (i == (argc - 1))
				{
					cerr << "Error: Can't find callback bit depth" << endl;
					print_usage();
					ShutDown(-1);
				}
				userInfo.callBackBitDepth = atoi(argv[++i]);
			}

			if (!strcmp(argv[i], "-l"))
			{
				if (i == (argc - 1))
				{
					cerr << "Error: Latency" << endl;
					print_usage();
					ShutDown(-1);
				}
				userInfo.latency = atof(argv[++i]);
			}

			if (!strcmp(argv[i], "-bl"))
			{
				if (i == (argc - 1))
				{
					cerr << "Error: Blocksize" << endl;
					print_usage();
					ShutDown(-1);
				}
				userInfo.blockSize = atoi(argv[++i]);
			}

			if (!strcmp(argv[i], "-smpte2110-30"))
			{
				userInfo.outputSmpte2110standard = 30;
			}

			if (!strcmp(argv[i], "-smpte2110-31"))
			{
				userInfo.outputSmpte2110standard = 31;
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

		if (userInfo.inputName.empty())
		{
			// set the name based on the tool version and stream type
			userInfo.inputName = "dlb-2110-mixer-in"; 
		}

		if (userInfo.outputName.empty())
		{
			// set the name based on the tool version and stream type
			userInfo.outputName = "dlb-2110-mixer-out"; 
		}


		// Set stream type based on user selection
		if (userInfo.outputSmpte2110standard == 30)
		{
			outputStreamInfo.streamType = AES67;
		}
		else if (userInfo.outputSmpte2110standard == 31)
		{
			// Could add an if here to support AM824 generation
			outputStreamInfo.streamType = AM824;
		}
		else if (userInfo.outputSmpte2110standard == 0)
		{
			outputStreamInfo.streamType = STREAM_TYPE_NONE; // Automatic selection - follows input
		}
		else
		{
			print_usage();
			throw runtime_error("Error: Unknown stream type requested");
		}


		aoipSystem.interface = userInfo.interface;
		aoipSystem.samplingFrequency = 48000;
		aoipSystem.name = "dlb_mixer";
		aoipServices = new AoipServices(aoipSystem, nullptr);
		// allow time for services to come up before starting stream 
		sleep(1);

		inputStreamInfo.streamName = userInfo.inputName;
		outputStreamInfo.streamName = userInfo.outputName;

		if (userInfo.numChannels > MAX_CHANNELS)
		{
			throw runtime_error("Number of channels selected is out of range");
		}

		outputStreamInfo.audio.numChannels = userInfo.numChannels;
		// If using auto channel number selection then
		// use automatic packet size selection i.e. follow input
		if (outputStreamInfo.audio.numChannels == 0)
		{
			outputStreamInfo.audio.samplesPerPacket = 0;
		}
		else if (outputStreamInfo.audio.numChannels < 9)
		{
			outputStreamInfo.audio.samplesPerPacket = 48; // 2110-30/31 class A		
		}
		else
		{
			outputStreamInfo.audio.samplesPerPacket = 6; // 2110-30/31 class C	
		}

		switch(userInfo.callBackBitDepth)
		{
		case 16:
			callBackInfo.audioFormat = DLB_AOIP_AUDIO_FORMAT_16BIT_LPCM;
			break;
		case 24:
			callBackInfo.audioFormat = DLB_AOIP_AUDIO_FORMAT_24BIT_LPCM;
			break;
		case 32:
			callBackInfo.audioFormat = DLB_AOIP_AUDIO_FORMAT_32BIT_LPCM;
			break;
		default:
			throw runtime_error("Unsupported callback bit depth");
		}

		outputStreamInfo.payloadType = 98; // Dynamic payloads only 96 - 127
		outputStreamInfo.dstIpStr = userInfo.dstIpStr;
		outputStreamInfo.port = 0; // Select for aoipSystem to select Port

		LOG(INFO) << "Searching for Input Stream:" << inputStreamInfo.streamName;
		float timeLeft = 30.0; // allow 30s for discovery before timeout
		ConsoleSpinner spinner;
		unsigned int sleepComplete = 0;

		while((sleepComplete == 0) && !userInfo.inputService && (timeLeft > 0.0))
		{
			sleepComplete = sleep(1);
			vector<AoipService>& rxStreams = aoipServices->GetAvailableServicesForRx();
			for (vector<AoipService>::iterator stream = rxStreams.begin() ; stream != rxStreams.end() ; stream++)
			{
				if (stream->GetName() == inputStreamInfo.streamName)
				{
					userInfo.inputService = &(*stream);
					break;
				}
			}
			spinner.turn();
			timeLeft--;
		}
		spinner.clear();
		if (sleepComplete != 0)
		{
			LOG(FATAL) << "Break detected...";
			ShutDown(-1);
		}
		if (!userInfo.inputService)
		{
			LOG(FATAL) << "Input stream not found...Terminating";
			//sleep(120); // Allow time for debug before shutting down
			ShutDown(-2);
		}
		// Just use first service, ignore rest for now
		// Service uses copy so application owns service and streamInfo
		// Make another copy so we have record of what was provided as well as changes made
		// for reception
		inputStreamInfo = userInfo.inputService->GetStreamInfo();

		LOG(INFO) << "***Found Input Stream " << inputStreamInfo.streamName << "***";
		if (inputStreamInfo.streamType == AES67)
		{
			LOG(INFO) << "Stream Type: AES67";
		}
		else if (inputStreamInfo.streamType == AM824)
		{
			LOG(INFO) << "Stream Type: AM824";
		}
		LOG(INFO) << "Number of Channels: " << inputStreamInfo.audio.numChannels;
		LOG(INFO) << "Samples per packet: " << inputStreamInfo.audio.samplesPerPacket;
		LOG(INFO) << "Input stream bit-depth: " << inputStreamInfo.audio.payloadBytesPerSample * 8 << " bits";
		LOG(INFO) << "MCast IP Address: " << inputStreamInfo.dstIpStr;

		// Fill in output parameters from input parameters
		outputStreamInfo.audio.numChannels = userInfo.numChannels;
		if(userInfo.bitDepth == 0)
		{
			outputStreamInfo.audio.payloadBytesPerSample = 0;
		}
		else
		{
			if (userInfo.bitDepth == 16)
			{
				outputStreamInfo.audio.payloadBytesPerSample = 2;
			}
			else if (userInfo.bitDepth == 24)
			{
				outputStreamInfo.audio.payloadBytesPerSample = 3;
			}
			else
			{
				throw runtime_error("Unsupported bit depth");
			}
		}
		outputStreamInfo.samplingFrequency = inputStreamInfo.samplingFrequency;
		if (outputStreamInfo.samplingFrequency != 48000)
		{
			throw runtime_error("Unsupported Sampling Frequency (48kHz only)");
		}

		// Create output stream but don't start it
		callBackData.streamInfo = inputStreamInfo;
		callBackInfo.callBack = RxTxCallback;
		callBackInfo.data = (void *)&callBackData;
		callBackInfo.blockSize = userInfo.blockSize;
		callBackData.callBackInfo = callBackInfo;

		std::vector<StreamInfo> outputStreamInfos;
		outputStreamInfos.push_back(outputStreamInfo);

		void *rxTxStream = aoipServices->AddRxTxStream(inputStreamInfo, outputStreamInfos, userInfo.latency, callBackInfo);
		success = aoipServices->StartRxTxStream(rxTxStream);
		LOG(INFO) << "Receiving " << inputStreamInfo.streamName << "...";
		if (!success)
		{
			throw runtime_error(string("Timed out waiting for data from " + inputStreamInfo.streamName));
		}

		// Continue until something causes shutdown or a break is received
		while(sleepComplete == 0)
		{
			// Check for stream termination once a second
			sleepComplete = sleep(2); // probably better to use nanosleep here
			spinner.turn();
		}
		LOG(INFO) << "Preparing to Shut Down...";
		ShutDown(0);
    }

	catch(runtime_error& e)
	{

	  	LOG(WARNING) << e.what();
		LOG(FATAL) << "*** Runtime Error Exception ***";
	  	// Check to see if this is already being handled
	  	if (!inSignalHandler)
	  	{
	  		ShutDown(-1);
	  	}
	  	else
	  	{
	  		// If it is then spin and wait for death
	  		while(1);
	  	}
	}

	return 0;
}

