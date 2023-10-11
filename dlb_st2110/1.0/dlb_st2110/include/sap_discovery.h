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

#ifndef _SAP_DISCOVERY_H_
#define _SAP_DISCOVERY_H_

#include "dlb_aoip_discovery.h"
#include "dlb_st2110_api.h"
#include "dlb_st2110.h"

/* Classes */

class SapDiscovery : public AoipDiscovery
{
private:
	class SapTxService
	{
		MClock::TimePoint nextSapPacketTxTime;
		MClock::Duration baseSapInterval;
		unsigned char sapPacket[MAX_MTU];
		unsigned int sapPacketSize;
		std::string name;
		std::string srcIpStr;

	public:

		SapTxService(AoipSystem& system, StreamInfo& streamInfo);
		void SendSAPPacket(int sapSocket);

		void SetNextSAPTxTime();
		/* returns number of bytes in packet */
		void BuildSAPPacket(char *sdpText, std::string localIpAddress);
		std::string GetName();
	};

	bool TxServiceExists(std::string serviceName);

	std::vector<AoipService> sapRxServices;
	std::vector<SapTxService> sapTxServices;
	int sapSocket;
	unsigned char rxPacket[MAX_MTU]; // This could be defined locally in Poll but is defined here for efficiency
	char sdpText[MAX_MTU];           // As above


public:
	SapDiscovery(const CallBacks &newCallBacks, AoipSystem &system);

	~SapDiscovery(void)
	{
		close(sapSocket);
	}

	void Poll(void);
	void AddTxService(StreamInfo& streamInfo);
	void UpdateTxService(StreamInfo& streamInfo);
	void RemoveTxService(std::string serviceName);
	void SetInputService(std::string serviceName)
	{
	}

private:
	void ParseSapPacket(unsigned char sapPacket[], unsigned int numBytes);

};

#endif // _SAP_DISCOVERY_H_
