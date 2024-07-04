/************************************************************************
 * dlb_st2110
 * Copyright (c) 2021, Dolby Laboratories Inc.
 * Copyright (c) 2021, Dolby International AB.
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

#ifndef _RAV_DISCOVERY_H_
#define _RAV_DISCOVERY_H_

#include <memory>

#include <avahi-client/client.h>
#include <avahi-client/publish.h>
#include <avahi-common/simple-watch.h>
#include <avahi-client/lookup.h>
//#include <avahi-common/malloc.h>
#include <avahi-common/error.h>
//#include <avahi-common/alternative.h>

#include "dlb_aoip_discovery.h"
#include "dlb_st2110_api.h"
#include "dlb_st2110.h"

/* Forward Definitions */
class AoipServices;
class AoipService;

/* Constants */

const uint16_t ravennaRtspPort = 8082;

/* Classes */

class RavDiscovery : public AoipDiscovery
{
private:
    class RavTxService
	{
		AvahiEntryGroup *avahiGroup;
		std::string name;
		uint64_t sessionId;
		std::string sdpText;

	public:
		RavTxService(AoipSystem& system, StreamInfo& streamInfo, AvahiClient *avahiClient);
		~RavTxService();
		RavTxService(const RavTxService&) = delete;
  		RavTxService& operator=(const RavTxService&) = delete;

		std::string GetName();
		void SendRtspDescribeResponse(int connfd, unsigned int cseq);
		void SendRtspSetupResponse(int connfd, unsigned int cseq);
	};


	class DescribeResponseParser
	{
	private:
	    unsigned int contentLength;
		unsigned int packetCounter;
		std::string sdpText;
		bool finished;
		Sdp2110 sdp;
		const unsigned int cseq;

	public:
		DescribeResponseParser(unsigned int newCseq) : packetCounter(0), finished(false), sdp(""), cseq(newCseq) {}
		void ProcessPacket(const char *pkt, unsigned int len);
		bool GotSdp() const { return sdp.Valid(); }
		Sdp2110& GetSdp() { return sdp; }
		bool Finished() const { return finished; }
	};

	AvahiSimplePoll *avahiPollObject;
    AvahiClient *avahiClient;
	AvahiEntryGroup *mainAvahiGroup;
    AvahiServiceBrowser *avahiServiceBrowser;
    int rtspRxSock;
    int rtspTxSock;
	unsigned int rtspSeq;
    std::vector<std::shared_ptr<RavTxService>> ravTxServices;
    std::vector<AoipService> ravRxServices;
    char rxRtspPacket[MAX_MTU];
    char txRtspPacket[MAX_MTU];

	void ReceiveRtspPacket(void);
	void ProcessRxRtspPacket(int connfd);
	int ProcessRxRtspResponse(void);
	void SendRtspOptionsResponse(int connfd, unsigned int cseq);


public:
	RavDiscovery(const CallBacks &newCallBacks, AoipSystem &newSystem);
	void Poll(void);
	void AddTxService(StreamInfo& streamInfo);
	void UpdateTxService(StreamInfo& streamInfo);
	void RemoveTxService(std::string serviceName);
	void SetInputService(std::string serviceName)
	{
	}

	~RavDiscovery()
	{
		while(!ravTxServices.empty())
		{
			ravTxServices.pop_back();
		}
		while(!ravRxServices.empty())
		{
			ravRxServices.pop_back();
		}    

		if (mainAvahiGroup)
		{
			avahi_entry_group_free(mainAvahiGroup);
		}
		if (avahiClient)
		{
			avahi_client_free(avahiClient);
		}
		if (avahiPollObject)
		{
			avahi_simple_poll_free(avahiPollObject);
		}
		close(rtspRxSock);
		close(rtspTxSock);
	}

	/* Not part of the API but used by Avahi callback */
	void CreateRavennaServices(AvahiClient *newAvahiClient);
	void CreateResolver(AvahiIfIndex interface, AvahiProtocol protocol, const char *name, const char *type, const char *domain);
	void SendDescribe(std::string address, uint16_t port, std::string name);
	int ReceiveRtspResponse(void);
	bool RxServiceExists(std::string name);
	bool TxServiceExists(std::string name);

private:
};

#endif // _RAV_DISCOVERY_H_