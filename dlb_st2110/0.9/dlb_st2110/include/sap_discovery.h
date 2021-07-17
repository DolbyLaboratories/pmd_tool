/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2021 by Dolby Laboratories,
 *                Copyright (C) 2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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

	std::vector<AoipService> sapRxServices;
	std::vector<SapTxService> sapTxServices;
	int sapSocket;
	unsigned char rxPacket[MAX_MTU]; // This could be defined locally in Poll but is defined here for efficiency
	char sdpText[MAX_MTU];           // As above


public:
	SapDiscovery(AoipServices &newServices, AoipSystem &system);

	~SapDiscovery(void)
	{
		close(sapSocket);
	}

	void Poll(void);
	void AddTxService(StreamInfo& streamInfo);
	void UpdateTxService(StreamInfo& streamInfo);
	void RemoveTxService(std::string serviceName);

private:
	void ParseSapPacket(unsigned char sapPacket[], unsigned int numBytes);

};

#endif // _SAP_DISCOVERY_H_