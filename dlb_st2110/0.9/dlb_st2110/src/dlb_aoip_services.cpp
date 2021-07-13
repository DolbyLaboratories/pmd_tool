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

#include <sys/socket.h>

// Avahi required for Ravenna discovery and advertisement
#include <avahi-client/client.h>

#include "dlb_st2110_api.h"
#include "dlb_st2110_hardware.h"

#include "dlb_st2110_logging.h"


using namespace std;

/************************** AoipRxService *******************/

// returns true on a match
bool AoipService::Compare(const AoipService &service2)
{
	if (streamInfo.streamName.compare(service2.streamInfo.streamName))
	{
		return(false);
	}
	if (streamInfo.streamType != service2.streamInfo.streamType)
	{
		return(false);
	}
	if (streamInfo.payloadType != service2.streamInfo.payloadType)
	{
		return(false);
	}
	if (streamInfo.dstIpStr.compare(service2.streamInfo.dstIpStr))
	{
		return(false);
	}
	if (streamInfo.srcIpStr.compare(service2.streamInfo.srcIpStr))
	{
		return(false);
	}
	if (streamInfo.port != service2.streamInfo.port)
	{
		return(false);
	}
	if (streamInfo.sessionId != service2.streamInfo.sessionId)
	{
		return(false);
	}
	if (streamInfo.samplingFrequency != service2.streamInfo.samplingFrequency)
	{
		return(false);
	}
	if (streamInfo.streamType == SMPTE2110_41)
	{
		if (streamInfo.metadata.packetTimeMs != service2.streamInfo.metadata.packetTimeMs)
		{
			return(false);
		}
		if (streamInfo.metadata.dataItemTypes != service2.streamInfo.metadata.dataItemTypes)
		{
			return(false);
		}
	}
	else
	{
		if (streamInfo.audio.numChannels != service2.streamInfo.audio.numChannels)
		{
			return(false);
		}
		if (streamInfo.audio.payloadBytesPerSample != service2.streamInfo.audio.payloadBytesPerSample)
		{
			return(false);
		}
		if (streamInfo.audio.samplesPerPacket != service2.streamInfo.audio.samplesPerPacket)
		{
			return(false);
		}
	}

	if (sdpSystemInfo.gmIdentity != service2.sdpSystemInfo.gmIdentity)
	{
		return(false);
	}
	if (sdpSystemInfo.domain != service2.sdpSystemInfo.domain)
	{
		return(false);
	}

	return(true);
}


/************************* Private Functions ***************************/



void AoipServices::AoipServicesThread(void)
{
	while(state == DLB_AOIP_ACTIVE)
	{
		sap->Poll();
		rav->Poll();
	}
}


/************************* Public Functions ***************************/

// Two seperate callbacks depending whether the application needs notification by stream or by service
// Either or both callbacks can be null if not required
// If callback are not used then Rx service/stream lists can be polled
AoipServices::AoipServices(
	AoipSystem &newSystem,
	void (*newRxServiceCallBack) (AoipService &))
{
	CLOG(INFO, SERVICES_LOG) << "AOIP Services Starting Up...";
	system = newSystem;
	hardware = new ST2110Hardware(system.interface, system.samplingFrequency);
	system.srcIpStr = hardware->GetSrcIpStr();
	system.gmIdentity = hardware->GetGmIdentity();
	system.domain = hardware->GetDomain();
	if (!hardware->GetSynched())
	{
		throw runtime_error("Not synchronized to PTP");
	}

	if (system.name.size() == 0)
	{
		throw runtime_error("Node name not set");
	}

	rxServiceCallBack = newRxServiceCallBack;

	sap = new SapDiscovery(*this, system);
	rav = new RavDiscovery(*this, system);

	/* Start thread */
	state = DLB_AOIP_ACTIVE;
	servicesThread = thread(&AoipServices::AoipServicesThread, this); 
}

AoipServices::~AoipServices(void)
{
	// signal to thread to exit
	state = DLB_AOIP_SHUTDOWN;
	
	CLOG(INFO, SERVICES_LOG) << "AOIP Services Shutting Down...";

	// While services are shutting down, destroy the streams
	// Receiver shuts down quickly whereas transmitters take a long time to tear down
	// To avoid lots of buffer overruns, tear down receivers first
	while (!receivers.empty())
	{
		receivers.pop_back();
	}

	while (!txStreams.empty())
	{
		txStreams.pop_back();
	}

	while(!rxTxStreams.empty())
	{
		rxTxStreams.pop_back();
	}

	if (servicesThread.joinable())
	{
		servicesThread.join();
	}

	delete sap;
	delete rav;
	// Hardware should always be last to go as it is dependent upon nothing
	// but everything is dependent upon it so caused problems if it disappears midstream
	delete hardware;
}

void AoipServices::ValidateTxStreamInfo(StreamInfo &newStreamInfo)
{
	if (newStreamInfo.streamName.size() == 0)
	{
		throw runtime_error("Stream Name is invalid");
	}

	if ((newStreamInfo.streamType != AES67) &&
		(newStreamInfo.streamType != AM824) &&
		(newStreamInfo.streamType != SMPTE2110_41))
	{
		throw runtime_error("Stream Type is invalid");
	}

	if ((newStreamInfo.payloadType < 96) ||
		(newStreamInfo.payloadType > 127))
	{
		throw runtime_error("Payload type out of range");
	}

	if (!ST2110Hardware::ValidMcastIPAddress(newStreamInfo.dstIpStr))
	{
		throw runtime_error("Invalid destination multicast address");
	}

	// Check to see if we have a source IP address, if not use system address
	if (newStreamInfo.srcIpStr.size() == 0)
	{
		newStreamInfo.srcIpStr = system.srcIpStr;
	}
	else
	{
		if (!ST2110Hardware::ValidUnicastIPAddress(newStreamInfo.srcIpStr))
		{
			throw runtime_error("Invalid source IP address");
		}
	}

	// Selection of 0 for port signals automatic port selection
	if (newStreamInfo.port == 0)
	{
		newStreamInfo.port = 5004;
	}
	else
	{
		if (newStreamInfo.port < 1024)
		{
			throw runtime_error("Invalid port");
		}
	}

	if ((newStreamInfo.streamType == AES67) ||
		(newStreamInfo.streamType == AM824))
	{
		if ((newStreamInfo.audio.numChannels < 1) || (newStreamInfo.audio.numChannels > maxChannels))
		{
			throw runtime_error("Invalid number of channels");
		}

		if ((newStreamInfo.audio.payloadBytesPerSample < 2) || (newStreamInfo.audio.payloadBytesPerSample > 3))
		{
			throw runtime_error("Invalid sample size");
		}

		// Only supporting 2110-30 packet times not full set in 2110-31 or AES67
		if ((newStreamInfo.audio.samplesPerPacket != 6) &&
			(newStreamInfo.audio.samplesPerPacket != 48))
		{
			throw runtime_error("Invalid or unsupported number of samples per packet");
		}

		if (newStreamInfo.samplingFrequency != 48000)
		{
			throw runtime_error("Invalid sampling Rate, only 48kHz supported");
		}
	}
	else
	{
		if ((newStreamInfo.metadata.packetTimeMs < 0.125) || (newStreamInfo.metadata.packetTimeMs > 1000.0))
		{
			throw runtime_error("Invalid or unsupported packet time. 125us to 1sec supported");
		}

		// Support message size of up to 1Mbyte
		if ((newStreamInfo.metadata.maxPayloadSizeBytes < 1) || (newStreamInfo.metadata.maxPayloadSizeBytes > 1000000))
		{
			throw runtime_error("Invalid message size (max 1Mbyte)");
		}

		if (newStreamInfo.metadata.dataItemTypes.size() != 1)
		{
			throw runtime_error("Invalid number of Data Item Types (DITs), only 1 supported");
		}
	}
}

void AoipServices::AddRxService(AoipServiceType serviceType, AoipService &newService)
{
	vector<AoipService>::iterator service;
	vector<AoipService>::iterator serviceListEnd;
	vector<AoipService> *serviceList;
	string serviceName = newService.GetName();

	if (serviceType == AOIP_SERVICE_SAP)
	{
		serviceList = &sapRxServices;
		service = sapRxServices.begin();
		serviceListEnd = sapRxServices.end();
	}
	else if (serviceType == AOIP_SERVICE_RAVENNA)
	{
		serviceList = &ravRxServices;			
		service = ravRxServices.begin();
		serviceListEnd = ravRxServices.end();			
	}
	else
	{
		throw runtime_error("Unknown Service Type");
	}
	for (;service != serviceListEnd ; service++)
	{
		if (!serviceName.compare(service->GetName()))
		{
			// Add log message here as we are trying to add something that has already been added
			CLOG(INFO, SERVICES_LOG) << "Duplicate Service detected: " << serviceName << endl;
			return;
		}
	}	

	serviceList->push_back(newService);

	if (rxServiceCallBack)
    {
    	(*rxServiceCallBack)(newService);
    }
}

void AoipServices::UpdateRxService(AoipServiceType serviceType, AoipService &updatedService)
{
	vector<AoipService>::iterator service;
	vector<AoipService>::iterator serviceListEnd;

	if (serviceType == AOIP_SERVICE_SAP)
	{
		service = sapRxServices.begin();
		serviceListEnd = sapRxServices.end();
	}
	else if (serviceType == AOIP_SERVICE_RAVENNA)
	{
		service = ravRxServices.begin();
		serviceListEnd = ravRxServices.end();			
	}
	else
	{
		throw runtime_error("Unknown Service Type");
	}
	for (;service != serviceListEnd ; service++)
	{
		if (service->Compare(updatedService))
		{
			*service = updatedService;
			break;
		}
	}
}

void AoipServices::RemoveRxService(AoipServiceType serviceType, string serviceName)
{
	vector<AoipService>::iterator service;
	vector<AoipService>::iterator serviceListEnd;
	vector<AoipService> *serviceList;

	if (serviceType == AOIP_SERVICE_SAP)
	{
		serviceList = &sapRxServices;
		service = sapRxServices.begin();
		serviceListEnd = sapRxServices.end();
	}
	else if (serviceType == AOIP_SERVICE_RAVENNA)
	{
		serviceList = &ravRxServices;			
		service = ravRxServices.begin();
		serviceListEnd = ravRxServices.end();			
	}
	else
	{
		throw runtime_error("Unknown Service Type");
	}
	for (;service != serviceListEnd ; service++)
	{
		if (!serviceName.compare(service->GetName()))
		{
			serviceList->erase(service);
			break;
		}
	}	
}


void AoipServices::AddTxStream(StreamInfo &newStreamInfo, ST2110TransmitterCallBackInfo *callBackInfo)
{
	ValidateTxStreamInfo(newStreamInfo);
	shared_ptr<AoipTxStream> tmpPtr = make_shared<AoipTxStream>(newStreamInfo, system, callBackInfo);
	txStreams.push_back(tmpPtr);
	sap->AddTxService(newStreamInfo);
	rav->AddTxService(newStreamInfo);
}

void AoipServices::AddTxStream(StreamInfo &newStreamInfo)
{
	ValidateTxStreamInfo(newStreamInfo);
	shared_ptr<AoipTxStream> tmpPtr = make_shared<AoipTxStream>(newStreamInfo, system, nullptr);
	txStreams.push_back(tmpPtr);
}

void AoipServices::StartTxStream(string streamName)
{
	bool foundStream = false;

	for (vector<shared_ptr<AoipTxStream>>::iterator txStream = txStreams.begin() ; (txStream != txStreams.end()) && !foundStream ; txStream++)
	{
		if (!streamName.compare((*txStream)->GetName()))
		{
			(*txStream)->Start();
			foundStream = true;
		}
	}
	if (!foundStream)
	{
		throw runtime_error("Error: Tried to start non-existant stream");		
	}
}


/* Receive Services */

void AoipServices::AddRxStream(StreamInfo &newStreamInfo, ST2110ReceiverCallBackInfo &callBackInfo)
{
	receivers.push_back(ST2110Receiver(system, newStreamInfo, callBackInfo));
}

void AoipServices::StartRxStream(std::string streamName)
{
	bool foundReceiver = false;

	for (vector<ST2110Receiver>::iterator receiver = receivers.begin() ; (receiver != receivers.end()) && !foundReceiver ; receiver++)
	{
		if (!streamName.compare(receiver->GetName()))
		{
			foundReceiver = true;
			receiver->Start();
		}
	}
	if (!foundReceiver)
	{
		throw runtime_error("Error: Tried to receive from non-existant stream");		
	}
}

std::vector<AoipService>& AoipServices::GetAvailableServicesForRx(void)
{
	allRxServices.clear();
	allRxServices = sapRxServices; // copy SAP
	allRxServices.insert(allRxServices.end(), ravRxServices.begin(), ravRxServices.end());
	return(allRxServices);
}


static bool RxStreamCallBack(void *data, void *inputAudio, unsigned int numBytes, uint32_t timeStamp)
{
	AoipRxTxStream *stream = (AoipRxTxStream *)data;
	return(stream->RxStreamCallBack(inputAudio, numBytes, timeStamp));
}

static bool TxStreamCallBack(void *data, void *outputAudio, unsigned int numBytes, bool &haveTimeStamp, uint32_t &timeStamp)
{
	AoipRxTxStream::TxCallBackData *txCallBackData = (AoipRxTxStream::TxCallBackData *)data;
	return(txCallBackData->stream->TxStreamCallBack(txCallBackData->txStream, outputAudio, numBytes, haveTimeStamp, timeStamp));
}

bool AoipRxTxStream::RxStreamCallBack(void *inputAudio, unsigned int numBytes, uint32_t timeStamp)
{
	void *inputBuffer; // input buffer pointer for thread safe buffer structure

	// Sanity check
	if (numBytes == 0)
	{
		return(streamActive);
	}
	// Signal that stream has started

	unsigned int expectingBytes = callBackInfo.blockSize * rxStreamInfo.audio.numChannels * callBackBytesPerSample;

	if(numBytes != expectingBytes)
	{
		throw runtime_error("Expected " + to_string(expectingBytes) + " but got " + to_string(numBytes));
	}

	inputBuffer = audioBuffer->GetNextInputBuffer();

	//audioBuffer->CheckSane();
	if (inputBuffer != nullptr)
	{
		// Call Application callBack for processing before adding output to buffer
		streamActive = (*callBackInfo.callBack)(callBackInfo.data, inputAudio, inputBuffer, numBytes, rxCallBackOutputSizeBytes);
		audioBuffer->CommitInputBuffer(timeStamp);
	}		
	else
	{
		CLOG(WARNING, SERVICES_LOG) << "Unable to receive samples - Buffer Overflow";
	}
	//audioBuffer->CheckSane();
	return(streamActive);
}

bool AoipRxTxStream::TxStreamCallBack(unsigned int txStream, void *outputAudio, unsigned int numBytes, bool &haveTimeStamp, uint32_t &timeStamp)
{
	unsigned int bytesRead;

	haveTimeStamp = true;
	bytesRead = audioBuffer->GetBuffer(txStream, outputAudio, numBytes, timeStamp);
	// Note that because timeStamp is unsigned then the overflow behaviour is defined by the standard
	timeStamp += latencyOffset;

	if (bytesRead < numBytes)
	{
		CLOG(WARNING, SERVICES_LOG) << "Unable to transmit samples - Buffer underflow";
		// zero out samples that were not read to avoid buffer underflow
		memset(outputAudio, 0, numBytes - bytesRead);
		// If we didn't get any samples then we didn't get a timestamp either
		if (bytesRead == 0)
		{
			haveTimeStamp= false;
		}
		//throw runtime_error("Underflow");
	}

	return(streamActive);

}

bool AoipRxTxStream::Start()
{
	CLOG(INFO, SERVICES_LOG) << "Waiting to receive data from " << rxStreamInfo.streamName << "...";
	cout << endl;
	cout.flush();
	// Allocate 25% of Latency Budget to Buffer
	// This happens by starting TX when Buffer is about 25% full
	// Buffer size = latency
	// Sleep for 5% until 40%

	float bufferPercentFull = audioBuffer->GetPercentFull();
	MClock::TimePoint timeOutTime, timeNow;
	MClock::Duration timeOutDuration;
	timeOutDuration.setSeconds(10);
	timeNow.SetNow();
	timeOutTime = timeNow + timeOutDuration;

	while ((bufferPercentFull < 50.0) && (timeNow < timeOutTime))
	{
		// Sleep for 5% to avoid thrashing for too long
		// Spin for last 5% to get accurate start time
		this_thread::sleep_for(chrono::microseconds(100));	
		timeNow.SetNow();
		bufferPercentFull = audioBuffer->GetPercentFull();
	}
	CLOG(INFO, SERVICES_LOG) << "Buffer " << bufferPercentFull << "% full ";
	return(!(timeNow > timeOutTime));

}



/* Combo RXTX Services */

void *AoipServices::AddRxTxStream(StreamInfo &rxStreamInfo, vector <StreamInfo> &txStreamInfos, float latency, ST2110TransceiverCallBackInfo &callBackInfo)
{
	ST2110TransmitterCallBackInfo txCallBackInfo;
	ST2110ReceiverCallBackInfo rxCallBackInfo;


	if ((rxStreamInfo.streamType != AES67) && (rxStreamInfo.streamType != AM824))
	{
		throw runtime_error("Can only Receive/Transmit SMPTE ST 2110-30(AES67) or SMPTE ST 2110-31 (AM824) streams");
	}

	// Fill in missing transmit options based on receive options if not selected
	// Make sure transmit sampling frequency is the same as receive
	// Sample rate conversion not supported
	CLOG(INFO, SERVICES_LOG) << "Found " << txStreamInfos.size() << " Tx streams";
	for (vector<StreamInfo>::iterator txStreamInfo = txStreamInfos.begin() ; txStreamInfo != txStreamInfos.end() ; txStreamInfo++)
	{
		CLOG(INFO, SERVICES_LOG) << "Adding Next Tx stream:";
		txStreamInfo->samplingFrequency = rxStreamInfo.samplingFrequency;

		// Check for automatic selection of output stream type
		if (txStreamInfo->streamType == STREAM_TYPE_NONE)
		{
			txStreamInfo->streamType = rxStreamInfo.streamType;
			switch(txStreamInfo->streamType)
			{
			case AES67:
				CLOG(INFO, SERVICES_LOG) << "Automatic selection of stream type (same as rx): AES67";
				break;
			case AM824:
				CLOG(INFO, SERVICES_LOG) << "Automatic selection of stream type (same as rx): AM824";
				break;
			default:
				CLOG(INFO, SERVICES_LOG) << "Automatic selection of unknown stream type";
			}
		}

		// Check for automatic seletion of bit depth
		if (txStreamInfo->audio.payloadBytesPerSample == 0)
		{
			txStreamInfo->audio.payloadBytesPerSample = rxStreamInfo.audio.payloadBytesPerSample;
			CLOG(INFO, SERVICES_LOG) << "Automatic selection of transmit bit depth (same as rx): " << txStreamInfo->audio.payloadBytesPerSample * 8 << " bit";
		}

		// Check for automatic selection of number of channels
		if (txStreamInfo->audio.numChannels == 0)
		{
			txStreamInfo->audio.numChannels = rxStreamInfo.audio.numChannels;
			CLOG(INFO, SERVICES_LOG) << "Automatic selection of number of channels (same as rx): " << txStreamInfo->audio.numChannels;
		}

		// Check for automatic selection of number of samples per packet
		if (txStreamInfo->audio.samplesPerPacket == 0)
		{
			txStreamInfo->audio.samplesPerPacket = rxStreamInfo.audio.samplesPerPacket;
			CLOG(INFO, SERVICES_LOG) << "Automatic selection of samples per packet (same as rx): " << txStreamInfo->audio.samplesPerPacket;
		}

			// Even though we are using timestamps to set transmit time
		// transmitter still requires latency to dimension buffers
		txStreamInfo->latency = latency;
	}

	// Allocate quarter of total latency budget to input buffers
	rxStreamInfo.latency = latency / 4.0;

	if (callBackInfo.blockSize == 0)
	{
		// Automatic blocksize selection
		// Find number of input packets in latency
		CLOG(INFO, SERVICES_LOG) << "Automatic blocksize selection";
		float numPacketsInputLatency = ceil(latency / ((float)rxStreamInfo.audio.samplesPerPacket / (float)rxStreamInfo.samplingFrequency));
		CLOG(INFO, SERVICES_LOG) << "Number of input Packets covering latency: " << numPacketsInputLatency;
		unsigned int numPacketsChosenBlockSize = ceil(numPacketsInputLatency / 8.0);
		CLOG(INFO, SERVICES_LOG) << "Number of Packets chosen for Block size: " << numPacketsChosenBlockSize;
		callBackInfo.blockSize = numPacketsChosenBlockSize * rxStreamInfo.audio.samplesPerPacket;
		CLOG(INFO, SERVICES_LOG) << " Selected Blocksize: " << callBackInfo.blockSize;
	}

	shared_ptr<AoipRxTxStream> tmpPtr = make_shared<AoipRxTxStream>(rxStreamInfo, txStreamInfos, latency, callBackInfo);
	rxTxStreams.push_back(tmpPtr);
	AoipRxTxStream *stream = &(*rxTxStreams.back());

	// Select 32bit as common format for connecting pipe and buffers between rx and tx

	rxCallBackInfo.callBack = RxStreamCallBack;
	rxCallBackInfo.blockSize = callBackInfo.blockSize;
	rxCallBackInfo.audioFormat = callBackInfo.audioFormat;
	rxCallBackInfo.data = stream;
	AddRxStream(rxStreamInfo, rxCallBackInfo);

	unsigned int i = 0;
	for (vector<StreamInfo>::iterator txStreamInfo = txStreamInfos.begin() ; txStreamInfo != txStreamInfos.end() ; txStreamInfo++)
	{
		shared_ptr<AoipRxTxStream::TxCallBackData> newtxCallBackData = make_shared<AoipRxTxStream::TxCallBackData>();
		newtxCallBackData->stream = stream;
		newtxCallBackData->txStream = i;
		txCallBackData.push_back(newtxCallBackData);
		txCallBackInfo.callBack = TxStreamCallBack;
		txCallBackInfo.blockSize = callBackInfo.blockSize;
		txCallBackInfo.audioFormat = callBackInfo.audioFormat;
		txCallBackInfo.data = &(*txCallBackData.back());
		AddTxStream(*txStreamInfo, &txCallBackInfo);
		i++;
	}
	return((void *)stream);
}


bool AoipServices::StartRxTxStream(void *hdl)
{
	bool success;
	AoipRxTxStream *stream = (AoipRxTxStream *)hdl;

	// Start receive stream
	StartRxStream(stream->GetRxName());
	// Wait for receive stream data to arrive
	success = stream->Start();
	if (success)
	{
		vector<string> txStreamNames = stream->GetTxNames();
		// start transmission of all transmit streams
		for (vector<string>::iterator txStreamName = txStreamNames.begin() ; txStreamName != txStreamNames.end() ; txStreamName++)
		{
			StartTxStream(*txStreamName);
			CLOG(INFO, SERVICES_LOG) << "Starting " << *txStreamName << "...";
		}
	}
	return(success);
}

