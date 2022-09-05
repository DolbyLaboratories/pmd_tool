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

#include "sap_discovery.h"
#include "dlb_st2110_sdp.h"
#include "dlb_st2110_logging.h"

using namespace std;

const char sapMcastAddr[16] = "239.255.255.255";
const uint16_t sapMcastPort = 9875;


SapDiscovery::SapDiscovery(AoipServices &newServices, AoipSystem &newSystem) : AoipDiscovery(newServices, newSystem)
{
	MClock::TimePoint timeNow;
	struct sockaddr_in bindAddr;
	struct ip_mreq mreq;
	struct in_addr interface_addr;

	// Create SAP Socket used for Tx & Rx, bind then add membership 
	sapSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if(sapSocket < 0)
	{
		throw runtime_error("Opening datagram socket error");
	}

	/* Initialize the group sockaddr structure with a */
	/* SAP annoucement address of 239.255.255.255 and port 9875. */

	memset((char *) &bindAddr, 0, sizeof(bindAddr));

	/* Bind to ensure SAP goes out on the right interface */
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_addr.s_addr = htonl(INADDR_ANY); //GetNetSrcIpInt();
	//groupSock.sin_addr.s_addr = inet_addr(sapMcastAddr);
	bindAddr.sin_port = htons(sapMcastPort);
	if (bind(sapSocket, (struct sockaddr *)&bindAddr, sizeof bindAddr))
	{
		throw runtime_error("Binding to local socket for SAP failed");
	}

	/* This socket option is required to ensure that the SAP packets go out on the right interface */
	interface_addr.s_addr = GetNetIpInt(system.srcIpStr);
	if (setsockopt(sapSocket, IPPROTO_IP, IP_MULTICAST_IF, &interface_addr, sizeof(interface_addr)) < 0)
    {
        throw runtime_error("Setting SAP multicast interface failed");
    }

    /* IGMPv3 join */
	mreq.imr_multiaddr.s_addr = inet_addr(sapMcastAddr);
   	mreq.imr_interface.s_addr = GetNetIpInt(system.srcIpStr);

    if (setsockopt(sapSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
    {
        throw runtime_error("Joining SAP multicase group failed");
    }

}

void SapDiscovery::Poll(void)
{
	fd_set rfds;
	struct timeval rxPollTimeOut;
	int packetWaiting;
	unsigned int numBytes;

	FD_ZERO(&rfds);
	FD_SET(sapSocket, &rfds);
	rxPollTimeOut.tv_sec = 0;
	rxPollTimeOut.tv_usec = 100000; // 100ms polling interval, allows 100ms tx accuracy

	/* wait to receive SAP packets for a short period of time */
	packetWaiting = select(sapSocket + 1, &rfds, nullptr, nullptr, &rxPollTimeOut);
	if (packetWaiting == -1)
	{
		throw std::runtime_error("Select() failed");
	}
	if (packetWaiting)
	{
		numBytes = recvfrom(sapSocket, rxPacket, MAX_MTU, 0, nullptr, 0);
		ParseSapPacket(rxPacket, numBytes);
	}
	// Before waiting for receive again, check to see if we have a SAP packet to transmit
	// checking for time is done internally
	for (vector<SapTxService>::iterator sapService = sapTxServices.begin() ; sapService != sapTxServices.end() ; sapService++)
	{
		sapService->SendSAPPacket(sapSocket);
	}

}

void SapDiscovery::AddTxService(StreamInfo& streamInfo)
{
	sapTxServices.push_back(SapTxService(system, streamInfo));
}

void SapDiscovery::UpdateTxService(StreamInfo& streamInfo)
{
	bool found = false;
	for (vector<SapTxService>::iterator service = sapTxServices.begin() ; service != sapTxServices.end() ; service++)
	{
		if (!streamInfo.streamName.compare(service->GetName()))
		{
			sapTxServices.erase(service);
			sapTxServices.push_back(SapTxService(system, streamInfo));
			found = true;
			break;
		}
	}
	if (!found)
	{
		CLOG(WARNING, SAP_LOG) << "Tried to update Sap Tx Service that doesn't exist";
	}
}

void SapDiscovery::RemoveTxService(string serviceName)
{
	bool found = false;
	for (vector<SapTxService>::iterator service = sapTxServices.begin() ; service != sapTxServices.end() ; service++)
	{
		if (!serviceName.compare(service->GetName()))
		{
			sapTxServices.erase(service);
			found = true;
			break;
		}
	}
	if (!found)
	{
		CLOG(WARNING, SAP_LOG) << "Tried to remove an Sap Tx Service that doesn't exist";
	}
}



void SapDiscovery::ParseSapPacket(unsigned char sapPacket[], unsigned int numBytes)
{
	SAPAnnoucement *header = (SAPAnnoucement *)sapPacket;
	SdpSystemInfo sdpSystemInfo;
	unsigned int sdpSize;

	if (strcmp(header->payload_type, "application/sdp"))
	{
		CLOG(WARNING, SAP_LOG) << "Received and SAP packet not containing an SDP, payload type: " << header->payload_type;
		return;
	}
	char *newSdpText = (char *)(&sapPacket[sizeof(SAPAnnoucement)]);
	// Limit copying of SDP to one less than we have size for
	// so we can add a terminator
	if ((numBytes - sizeof(SAPAnnoucement)) < (MAX_SDP_SIZE - 1))
	{
		sdpSize = numBytes - sizeof(SAPAnnoucement);
	}
	else
	{
		sdpSize = MAX_SDP_SIZE - 1;
	}
	strncpy(sdpText, newSdpText, sdpSize); // termination not guaranteed
	sdpText[sdpSize] = 0; // terminated
	//cout << "New SDP Length: " << sdpSize << " " << strlen(sdpText) << endl << sdpText << endl;
	Sdp2110 newSdp(sdpText);
	StreamInfo newStreamInfo = newSdp.GetStreamInfo();
	SdpSystemInfo newSdpSystemInfo = newSdp.GetSystemInfo();
	AoipService newService(AOIP_SERVICE_SAP, newStreamInfo, newSdpSystemInfo);

	// Now check if a new service or stream needs to be added
	bool foundStream = false;
	for (vector<AoipService>::iterator service = sapRxServices.begin() ; service != sapRxServices.end() ; service++)
	{
		if (!newStreamInfo.streamName.compare(service->GetName()))
		{
			foundStream = true;
			// found service
			// Now do a full compare to see if update is required
			if (!service->Compare(newService))
			{		
				*service = newService;
				owner.UpdateRxService(AOIP_SERVICE_SAP, newService);
			}
		}
	}
	if (!foundStream)
	{
		// Create local copy
		sapRxServices.push_back(newService);
		owner.AddRxService(AOIP_SERVICE_SAP, newService);
	}
}

/************************* Sap Tx Service ***************************/
SapDiscovery::SapTxService::SapTxService(AoipSystem& system, StreamInfo& streamInfo)
{
	char sdpText[MAX_SDP_SIZE];
	SdpSystemInfo sdpSystemInfo;

	sdpSystemInfo.domain = system.domain;
	sdpSystemInfo.gmIdentity = system.gmIdentity;
	name = streamInfo.streamName;
	srcIpStr = system.srcIpStr;

	Sdp2110 sdp(streamInfo, sdpSystemInfo);
    // Build normal SDP for SAP packet
   	sdp.GetText(sdpText, sizeof(sdpText));

	BuildSAPPacket(sdpText, streamInfo.srcIpStr);
	baseSapInterval.setSeconds(sapPacketSize / 20.0);
	// This ensures the first Sap packet is sent immediately
	nextSapPacketTxTime.SetNow();
}

string SapDiscovery::SapTxService::GetName(void)
{
	return(name);
}


void SapDiscovery::SapTxService::SetNextSAPTxTime()
{
	double fraction = (rand() / (double)RAND_MAX) * 0.6666;
	MClock::Duration varying;
	MClock::Duration fixed;

	varying = baseSapInterval * fraction;
	fixed = baseSapInterval * 0.6666;
	varying = varying + fixed;
	nextSapPacketTxTime = nextSapPacketTxTime + varying;
}

void SapDiscovery::SapTxService::SendSAPPacket(int sapSocket)
{
	// Prepare the header
	MClock::TimePoint timeNow;
	struct sockaddr_in multicastAddr; /* Multicast address */

	// Check to see if it is time to send packet */
	/* This function assumes frequent polling */
	timeNow.SetNow();
	if (nextSapPacketTxTime > timeNow)
	{
		return;
	}

	/* Construct local address structure */
    memset(&multicastAddr, 0, sizeof(multicastAddr));   /* Zero out structure */
    multicastAddr.sin_family = AF_INET;                 /* Internet address family */
    multicastAddr.sin_addr.s_addr = inet_addr(sapMcastAddr);/* Multicast IP address */
    multicastAddr.sin_port = htons(sapMcastPort);         /* Multicast port */

	CLOG(INFO, SAP_LOG) << "Sending SAP Packet for stream" << name;

	/* groupSock sockaddr structure. */
	if(sendto(sapSocket, sapPacket, sapPacketSize, 0, (struct sockaddr*)&multicastAddr, sizeof(multicastAddr)) < 0)
	{
		throw runtime_error("Sending SDP datagram message error");
	}
	else
	{
		// Now recalculate and set next TxTime
		SetNextSAPTxTime();
	}
}


void SapDiscovery::SapTxService::BuildSAPPacket(char *sdpText, string localIpAddress)
{
	uint32_t dataLen = 0;
	SAPAnnoucement header;
	char *p;
	unsigned int sdpCopySize;
	unsigned int sdpSize = strlen(sdpText);

	header.vartec = 0x20;
	header.auth_len = 0;
	header.msg_id_hash = 0x0001;

	header.source_address = GetHostIpInt(srcIpStr);
	strcpy(header.payload_type, "application/sdp");

	memcpy(sapPacket, &header, sizeof(header));
	dataLen += sizeof(header);
	sapPacket[dataLen] = 0;
	p = (char *)&sapPacket[dataLen];

	if (sdpSize > (MAX_MTU - dataLen))
	{
		sdpCopySize = MAX_MTU - dataLen;
	}
	else
	{
		sdpCopySize = sdpSize;
	}
	strncpy(p, sdpText, sdpCopySize);
	dataLen += sdpCopySize;

	if (dataLen == 0 )
	{
		throw runtime_error("Error: failed to build SAP packet");
	}
	sapPacketSize = dataLen;
}
