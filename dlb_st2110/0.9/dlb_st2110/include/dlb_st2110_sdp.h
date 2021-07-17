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
