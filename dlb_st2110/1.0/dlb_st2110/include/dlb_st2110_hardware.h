/************************************************************************
 * dlb_st2110
 * Copyright (c) 2019-2020, Dolby Laboratories Inc.
 * Copyright (c) 2019-2020, Dolby International AB.
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


typedef std::array<unsigned char, 6> MacAddrByteArray;

bool GetMacAddrByteArray(std::string interfaceName, MacAddrByteArray &byteArray);

class ST2110Hardware
{
	bool synched;
	std::string gmIdentity;
	unsigned int domain;
	unsigned int samplingFrequency;

private:
	void RiverMaxInit(void);

	// Service for getting information about PTP service

	bool PmcGet(const std::string variable, const std::string key, std::string &result);

	void CheckTaiOffset(void);

	static unsigned int get_tai_offset(void);
	static void set_tai_offset(unsigned int offset);


public:

	ST2110Hardware(unsigned int newDomain);

	~ST2110Hardware(void);

	void GetPTPInfo(std::string &gm, unsigned int &dom, bool &sync);

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

	static std::string GetIpStrFromInterface(std::string interfaceName);

	static uint32_t GetHostIpIntFromInterface(std::string interfaceName);

	static uint32_t GetNetIpIntFromInterface(std::string interfaceName);

	static void GetErrorMsg(const char *msg, rmax_status_t error);

};

#endif // DLB_ST2110_HARDWARE