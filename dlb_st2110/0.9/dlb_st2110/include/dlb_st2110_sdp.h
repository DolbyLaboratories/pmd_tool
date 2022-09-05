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

#ifndef _DLB_SDP_ST2110_H_
#define _DLB_SDP_ST2110_H_

#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>

// For stream and system info structures
#include "dlb_st2110.h"

/************************* Constants ***************************/

#define MAX_SDP_SIZE 8192

/************************* Static helper functions ***************************/

class Sdp2110
{
	/* Data members */
	/* uses RxStreamInfo because this is the superset */
	StreamInfo streamInfo;
	SdpSystemInfo systemInfo;
	bool valid; // indicates that the current data in the object is valid

	/* private helper member functions */

	std::vector<std::string> FilterByType(char type, std::vector<std::string> lines);


	std::vector<std::string> FilterLineByField(std::string match, unsigned int fieldPos, std::vector<std::string> lines);

	std::vector<std::string> LineToFields(std::string line);

	bool GetField(const std::vector<std::string> &fields, unsigned int selection, std::string &out);
	bool GetField(const std::vector<std::string> &fields, unsigned int selection, int &out);
	bool GetField(const std::vector<std::string> &fields, unsigned int selection, float &out);
	bool GetField(const std::vector<std::string> &fields, unsigned int selection, uint32_t &out);
	bool GetField(const std::vector<std::string> &fields, unsigned int selection, uint16_t &out);
	bool GetField(const std::vector<std::string> &fields, unsigned int selection, uint64_t &out);

	void reset(void);

public:

	Sdp2110(const char *text)
	{
		SetText(text);
	}

  	Sdp2110(std::string text) : Sdp2110(text.c_str()) {}

	Sdp2110(StreamInfo newStreamInfo, SdpSystemInfo newSystemInfo)
	{
		streamInfo = newStreamInfo;
		systemInfo = newSystemInfo;
	}

	void SetText(const char *text);

	void GetText(char *sdpText, unsigned int size, bool rmax) const;

	std::string GetText(bool rmax) const;

	void GetText(char *sdpText, unsigned int size) const
	{
		GetText(sdpText, size, false);
	}

	void GetTextRmax(char *sdpText, unsigned int size) const
	{
		GetText(sdpText, size, true);		
	}

	StreamInfo GetStreamInfo(void) const
	{
		return streamInfo;
	}

	SdpSystemInfo GetSystemInfo(void) const
	{
		return systemInfo;
	}

	bool Valid() const
	{
		return valid;
	}

};

std::ostream& operator<<(std::ostream& os, const Sdp2110& sdp);

#endif //_DLB_SDP_ST2110_H_
