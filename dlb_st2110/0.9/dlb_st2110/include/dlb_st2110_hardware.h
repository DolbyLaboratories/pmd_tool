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

#ifndef _DLB_ST2110_HARDWARE_H_
#define _DLB_ST2110_HARDWARE_H_

#include <string>
#include <array>
#include <memory>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <rivermax_api.h>
#include <time.h>
#include <unistd.h>

#define NUM_RMAX_ERR_CODES 24
#define RMAX_ERR_MSG_LEN 100

// Service for getting information about PTP service
std::string PmcGet(const char *variable, const char *key);

class ST2110Hardware
{
	bool synched;
	std::string gmIdentity;
	unsigned int domain;
	unsigned int samplingFrequency;
	std::string interface;
	std::string srcIpStr;

private:
	void RiverMaxInit(void);

public:

	static bool IsLittleEndian(void)
	{
		int n = 1;
		return(*(char *)&n == 1);
	}

	static bool IsBigEndian(void)
	{
		return(!IsLittleEndian());
	}

	static bool ValidMcastIPAddress(std::string ipStr)
	{
		in_addr_t ipAddr = ntohl(inet_addr(ipStr.c_str()));
		return((ipAddr > ntohl(inet_addr("224.0.0.0"))) &&
			   (ipAddr < ntohl(inet_addr("239.255.255.255"))));
	}

	static bool ValidUnicastIPAddress(std::string ipStr)
	{
		in_addr_t ipAddr = ntohl(inet_addr(ipStr.c_str()));
		// Assume private network only
		return(((ipAddr > ntohl(inet_addr("10.0.0.0"))) &&
			   (ipAddr < ntohl(inet_addr("10.255.255.255")))) ||
			   ((ipAddr > ntohl(inet_addr("172.16.0.0"))) &&
			   (ipAddr < ntohl(inet_addr("172.16.31.255")))) ||
			   ((ipAddr > ntohl(inet_addr("192.18.0.0"))) &&
			   (ipAddr < ntohl(inet_addr("192.19.255.255")))) ||
			   ((ipAddr > ntohl(inet_addr("192.168.0.0"))) &&
			   (ipAddr < ntohl(inet_addr("192.168.255.255")))));
	}

	static void RmaxError(const char *msg, rmax_status_t error);

	ST2110Hardware(void)
	{
		synched = false;
		gmIdentity = "";
		domain = 0;
		samplingFrequency = 0;
		interface = "";
	}

	ST2110Hardware(std::string localInterface, unsigned int fs);

	~ST2110Hardware(void);

	unsigned int GetDomain(void)
	{

		return(domain);
	}

	unsigned int GetSamplingFrequency(void)
	{
		return(samplingFrequency);
	}

	std::string GetGmIdentity(void)
	{
		return(gmIdentity);
	}

	bool GetSynched(void)
	{
		return(synched);
	}

	std::string GetInterface(void)
	{
		return(interface);
	}

	uint32_t GetHostSrcIp(void)
	{
		return(ntohl(inet_addr(srcIpStr.c_str())));
	}

	uint32_t GetNetSrcIp(void)
	{
		return(inet_addr(srcIpStr.c_str()));
	}

	const char *GetSrcIpStr(void)
	{
		return(srcIpStr.c_str());
	}

};

#endif // DLB_ST2110_HARDWARE