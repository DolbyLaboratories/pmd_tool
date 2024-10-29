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

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <cmath>

// For stream and system info structures
#include "dlb_st2110.h"
#include "dlb_st2110_sdp.h"

using namespace std;

/************************* Constants ***************************/
// SDP file seperators
const char sep[] = " :=/;,\r";

/************************* Static helper functions ***************************/

void Sdp2110::reset(void)
{
	valid = false;
	streamInfo.reset();
	systemInfo.reset();
}


vector<string> Sdp2110::FilterByType(char type, vector<string> lines)
{
	vector<string> newLines;
	for (vector<string>::iterator line = lines.begin() ; line != lines.end(); line++)
	{
		size_t start = line->find_first_not_of(" ", 0);
		size_t end = line->find_first_of(" =", start);
		if ((end == (start + 1)) && (line->at(start) == type))
		{
			newLines.push_back(*line);
		}
	}
	return(newLines);
}

vector<string> Sdp2110::FilterByString(string s, vector<string> lines)
{
	vector<string> newLines;
	for (vector<string>::iterator line = lines.begin() ; line != lines.end(); line++)
	{
		size_t start = line->find_first_not_of(" ", 0);
		size_t end = line->find_first_of(" ", start);
		if ((end > start) && (s == line->substr(start, end)))
		{
			newLines.push_back(*line);
		}
	}
	return(newLines);
}

vector<string> Sdp2110::FilterLineByField(string match, unsigned int fieldPos, vector<string> lines)
{
	vector<string> newLines;
	for (vector<string>::iterator line = lines.begin() ; line != lines.end(); line++)
	{
		unsigned int i = fieldPos;
		size_t startPos, endPos = 0;
		do
		{
			startPos = line->find_first_not_of(sep, endPos);
			endPos = line->find_first_of(sep, startPos);
		}
		while(i-- > 0);
		string stringToMatch = line->substr(startPos, endPos - startPos);
		if (!stringToMatch.compare(match))
		{
			newLines.push_back(*line);
		}
	}
	return(newLines);
}

bool Sdp2110::FilterLinesBetweenStrings(vector<string> &lines, string startStr, string endStr)
{
	vector<string>::iterator startLine = lines.end();
	vector<string>::iterator endLine = lines.end();
	for(vector<string>::iterator line = lines.begin() ; (line != lines.end()) && ((startLine == lines.end()) || (endLine == lines.end())) ; line++)
	{
		if (line->find(startStr) != string::npos)
		{
			startLine = line;
		}
		if (line->find(endStr) != string::npos)
		{
			endLine = line;
		}
	}
	unsigned int linesBeyondEnd = distance(endLine, lines.end());
	if ((startLine != lines.end()) &&
		(endLine != lines.end()) &&
		(distance(startLine, endLine) > 0))
	{
		lines.erase(lines.begin(), startLine);
		// endLine invalidated so we need to remove the tail another way
		for (unsigned int i = 0 ; i < linesBeyondEnd ; i++)
		{
			lines.pop_back();
		}
		return(false);
	}
	else
	{
		return(true);
	}
}

vector<string> Sdp2110::LineToFields(string line)
{
	vector<string> fields;

	size_t beforeFirst = 0, first, firstSep;
	while(beforeFirst < line.length())
	{
		first = line.find_first_not_of(sep, beforeFirst);
		firstSep = line.find_first_of(sep, first);
		if (firstSep > first)
		{
			fields.push_back(line.substr(first, firstSep - first));
		}
		beforeFirst = firstSep;
	}
	return(fields);
}

bool Sdp2110::GetField(const vector<string> &fields, unsigned int selection, string &out)
{
	if (selection >= fields.size())
	{
		return(true);
	}
	else
	{
		out = fields[selection];
		return(false);
	}
}

bool Sdp2110::GetField(const vector<string> &fields, unsigned int selection, uint64_t &out)
{
	if (selection >= fields.size())
	{
		return(true);
	}
	else
	{
		try
		{
	  		out = stoul(fields[selection]);
		}
		catch(...)
		{
			return(true);
		}
		return(false);
	}
}


bool Sdp2110::GetField(const vector<string> &fields, unsigned int selection, int &out)
{
	if (selection >= fields.size())
	{
		return(true);
	}
	else
	{
		try
		{
	  		out = stoi(fields[selection]);
		}
		catch(...)
		{
			return(true);
		}
		return(false);
	}
}

bool Sdp2110::GetField(const vector<string> &fields, unsigned int selection, uint32_t &out)
{
	uint64_t longOut = 0;
	bool b = GetField(fields, selection, longOut);
	if (b || (longOut > 0xFFFFFFFFL))
	{
		return(true);
	}
	else
	{
		out = longOut;
		return(false);
	}
}

bool Sdp2110::GetField(const vector<string> &fields, unsigned int selection, uint16_t &out)
{
	uint64_t longOut = 0;
	bool b = GetField(fields, selection, longOut);
	if (b || longOut > 65535)
	{
		return(true);
	}
	else
	{
		out = longOut;
		return(false);
	}

}

bool Sdp2110::GetField(const vector<string> &fields, unsigned int selection, float &out)
{
	if (selection >= fields.size())
	{
		return(true);
	}
	else
	{
		try
		{
	  		out = stof(fields[selection]);
		}
		catch(...)
		{
			return(true);
		}
		return(false);
	}
}


// This parses SDP text
// Does not support more that 1 session per call
void Sdp2110::SetText(const char *text)
{
	vector<string> lines;
	const unsigned int lineSize = 2048; // Maximum allowed line size, exceeding this will cause a failure
	char line[lineSize];
	string codec;
	unsigned int frameCount = 0;
	float packetTimeMs;

	valid = false;

	stringstream iss;
	//cout << text;
	//printf("%s", text);
	iss << text;
	while (iss.getline(line, lineSize))
	{
		lines.push_back(string(line));
		//cout << i++ << ": " << line << endl;
	}
	// Now have a vector of lines
	// First get Session name
	vector<string> nameLine = FilterByType('s', lines);
	// Check there is 1 and only one name entry
	if (nameLine.size() != 1)
	{
		return;
	}
	// name is last field on line
	streamInfo.streamName = LineToFields(nameLine.front()).back();

	// Session ID;
	vector<string> originLine = FilterByType('o', lines);
	// Check there is 1 and only one name entry
	if (originLine.size() != 1)
	{
		return;
	}
	vector<string> originFields = LineToFields(originLine.front());
	GetField(originFields, 2, streamInfo.sessionId); // ignore error

	// Next check for groupings and if they exist, take first steam i.e. assume primary
	// TODO would be to return a vector
	vector<string> groupLines = FilterByString("a=group:DUP", lines);

	// Only allow one of these
	if (groupLines.size() > 1)
	{
		return;
	}
	if (groupLines.size() == 1)
	{
		// remove everything beyond the a=mid: line
		vector <string> groupLineFields = LineToFields(groupLines.front());
		if (groupLineFields.size() > 1)
		{
			string firstGroupName = groupLineFields[3];
			string groupTermLine(string("a=mid:") + firstGroupName);
			if (FilterLinesBetweenStrings(lines, groupLines.front(), groupTermLine))
			{
				// Search for termination line failed
				return;
			}
		}
	}
	// If no groups then just process as normal
	
	// Now get destination ip from connection data
	vector<string> connectionLine = FilterByType('c', lines);
	// Allow more than 1 connection entry
	// This is tolerated as I've seen this on some implmentations
	// For example Merging Mac Virtual Sound Card
	// Just take first line and ignore the rest, i.e. assume all the same
	if (connectionLine.size() == 0)
	{
		return;
	}
	vector<string> connectionFields = LineToFields(connectionLine.front());
	if (GetField(connectionFields, 3, streamInfo.dstIpStr)) // don't ignore destination IP
	{
		return;
	}

	vector<string> mediaLine = FilterByType('m', lines);
	if (mediaLine.size() != 1)
	{
		return;
	}
	vector<string> mediaFields = LineToFields(mediaLine.front());
	streamInfo.payloadType = stoi(mediaFields.back());
	// Check validity of payload type
	if ((streamInfo.payloadType < 96) || (streamInfo.payloadType > 127))
	{
		return;
	}
	if (GetField(mediaFields, 2, streamInfo.port))
	{
		return;
	}
	string mediaType;
	GetField(mediaFields, 1, mediaType); // next line checks success
	if (!mediaFields[3].compare("RTP/AVP"))
	{
		return;
	}
	vector<string> attributeLines = FilterByType('a', lines);


	vector<string> sourceFilterLine = FilterLineByField("source-filter", 1, attributeLines);
	// if no source filter line then set source IP for filtering to 'ANY'
	if (sourceFilterLine.size() > 0)
	{
		vector<string> sourceFilterFields = LineToFields(sourceFilterLine.front());
		GetField(sourceFilterFields, 6, streamInfo.srcIpStr);
	}
	else
	{
		streamInfo.srcIpStr = "0.0.0.0";
	}

	vector<string> rtpmapLine = FilterLineByField("rtpmap", 1, attributeLines);
	// check payload type matches payload type from media line
	if (rtpmapLine.size() != 1)
	{
		return;
	}
	vector<string> rtpmapFields = LineToFields(rtpmapLine.front());
	unsigned int payloadType2 = 0;
	GetField(rtpmapFields, 2, payloadType2);
	if (payloadType2 != streamInfo.payloadType)
	{
		return;
	}
	// Try and get frameCount parameter
	vector<string> frameCountLine = FilterLineByField("framecount", 1, attributeLines);
	// If there is exactly one framecount line then use it
	if (frameCountLine.size() > 0)
	{
		GetField(LineToFields(frameCountLine.front()), 2, frameCount );
	}
	// Get packet time
	vector<string> packetTimeLine = FilterLineByField("ptime", 1, attributeLines);
	// Check there is only one packet time line
	if (packetTimeLine.size() == 1)
	{
		if (GetField(LineToFields(packetTimeLine.front()), 2, packetTimeMs))
		{
			return;
		}
	}
	else
	{
		return;
	}

	if (GetField(rtpmapFields, 3, codec) ||
		GetField(rtpmapFields, 4, streamInfo.samplingFrequency))
	{
		return;
	}

	// Check for audio streams
	if (!codec.compare("L16") || !codec.compare("L24") || !codec.compare("AM824"))
	{
		// Check media type was listed as audio
		if (mediaType.compare("audio"))
		{
			return;
		}
		if (GetField(rtpmapFields, 5, streamInfo.audio.numChannels))
		{
			return;
		}
		if (frameCount > 0)
		{
			streamInfo.audio.samplesPerPacket = frameCount;
		}
		else
		{
			streamInfo.audio.samplesPerPacket = round((packetTimeMs * streamInfo.samplingFrequency) / 1000.0);
		}
		if (!codec.compare("AM824"))
		{
			streamInfo.streamType = AM824;
			// Note that this is the number of audio bytes per sample that is passed across the API
			// Obviously the packet has 4 bytes per sample but this is hidden from the application
			// at least for now
			// A new item called streamBytesPerSample could be introduced which would be 4 but this
			// is not currently required by the application
			streamInfo.audio.payloadBytesPerSample = 3;
		}
		else
		{
			streamInfo.streamType = AES67;
			if (!codec.compare("L16"))
			{
				streamInfo.audio.payloadBytesPerSample = 2;
			}
			else
			{
				streamInfo.audio.payloadBytesPerSample = 3;
			}
		}
	}
	else
	{
		// Check media type was listed as application as it should metadata
		if (mediaType.compare("application"))
		{
			return;
		}
		// Metadata streams - only support -41 at the moment
		if (codec.compare("ST2110-41"))
		{
			return;
		}
		streamInfo.streamType = SMPTE2110_41;
		streamInfo.metadata.packetTimeMs = packetTimeMs;
		vector<string> ditLine = FilterLineByField("ftmp", 1, attributeLines);
		// Check there is one and only one of these lines
		if (ditLine.size() != 1)
		{
			return;
		}
		// Check payload Type
		vector<string> ditLineFields = LineToFields(ditLine.front());
		unsigned int ditPayloadType = 0;
		GetField(ditLineFields, 2, ditPayloadType);
		if ((ditPayloadType != streamInfo.payloadType) ||
			(ditLineFields.size() < 7))
		{
			return;
		}
		// Check -41 string and standard version
		if ((ditLineFields[3].compare("SSN")) ||
			(ditLineFields[4].compare("ST2110-41")) ||
			(ditLineFields[5].compare("2024")) ||
			(ditLineFields[6].compare("DIT")))
		{
			return;
		}
		// Extract Dit numbers
		for (unsigned int ditIndex = 7 ; ditIndex < ditLine.front().length() ; ditIndex++)
		{
			streamInfo.metadata.dataItemTypes.push_back(stoi(ditLineFields[ditIndex]));
		}
	}

	// Now get PTP information, allow multiple lines but use first
	// Don't return errors
	vector<string> clockDomainLine = FilterLineByField("clock-domain", 1, attributeLines);
	if (!clockDomainLine.empty())
	{
		GetField(LineToFields(clockDomainLine.front()), 3, systemInfo.domain);
		string ptpVersionString;
		GetField(LineToFields(clockDomainLine.front()), 2, ptpVersionString);
		if (ptpVersionString.compare("PTPv2"))
		{
			return;
		}
	}
	vector<string> refClockLine = FilterLineByField("ts-refclk", 1, attributeLines);
	if (!refClockLine.empty())
	{
		GetField(LineToFields(refClockLine.front()), 4, systemInfo.gmIdentity);
	}

	valid = true;
}

string Sdp2110::GetText(bool rmax) const
{
	char sdpText[MAX_SDP_SIZE];
	GetText(sdpText, MAX_SDP_SIZE, rmax);
	return(string(sdpText));
}


// This is a simple c char version to start
// Move over to a C++ string based on later
void Sdp2110::GetText(char *sdpText, unsigned int size, bool rmax) const
{
	const unsigned int lineSize = 256;
	const unsigned int codecStrSize = 20;
	char codec[codecStrSize + 1];
	char line[lineSize + 1];
	float packetTimeMs;

	// Generate codec string from stream type
	switch(streamInfo.streamType)
	{
		case AES67:
		if (streamInfo.audio.payloadBytesPerSample == 2)
		{
			strncpy(codec, "L16", codecStrSize);
		}
		if (streamInfo.audio.payloadBytesPerSample == 3)
		{
			strncpy(codec, "L24", codecStrSize);
		}
		break;
		case AM824:
			strncpy(codec, "AM824", codecStrSize);
		break;
		case SMPTE2110_41:
			if (rmax)
			{
				strncpy(codec, "smpte291", codecStrSize);
			}
			else
			{
				strncpy(codec, "ST2110-41", codecStrSize);				
			}
		break;
		default:
		throw std::runtime_error("Error: Unrecognized format");
	}

	snprintf(sdpText, size, "v=0\n");
	snprintf(line, lineSize, "o=- %lu 0 IN IP4 %s\n", streamInfo.sessionId, streamInfo.srcIpStr.c_str());
	strncat(sdpText, line, size);
	snprintf(line, lineSize, "s=%s\n", streamInfo.streamName.c_str());
	strncat(sdpText, line, size);
	sprintf(line, "c=IN IP4 %s/31\n", streamInfo.dstIpStr.c_str());
	strncat(sdpText, line, size);
	strncat(sdpText, "t=0 0\n", size);
	if (streamInfo.streamType == SMPTE2110_41)
	{
		if (rmax)
		{
			snprintf(line, lineSize, "m=video %u RTP/AVP %u\n", streamInfo.port, streamInfo.payloadType);
		}
		else
		{
			snprintf(line, lineSize, "m=application %u RTP/AVP %u\n", streamInfo.port, streamInfo.payloadType);			
		}
	}
	else
	{
		snprintf(line, lineSize, "m=audio %u RTP/AVP %u\n", streamInfo.port, streamInfo.payloadType);
	}
	strncat(sdpText, line, size);
	if ((streamInfo.streamType == AES67) &&
		(streamInfo.channelLabels.size() == streamInfo.audio.numChannels))
	{
		// Check that the number of labels we have matches the number of channels
		snprintf(line, lineSize, "i=");
		unsigned int i = 0;
		for (vector<string>::const_iterator channelLabel = streamInfo.channelLabels.begin() ; channelLabel != streamInfo.channelLabels.end() ; channelLabel++)
		{
			strncat(line, channelLabel->c_str(), lineSize);
			if (i++ != (streamInfo.audio.numChannels - 1))
			{
				strncat(line, ",", lineSize);
			}
		}
		strncat(line, "\n", lineSize);
		strncat(sdpText, line, size);
	}
	snprintf(line, lineSize, "a=ts-refclk:ptp=IEEE1588-2008:%s:%d\n", systemInfo.gmIdentity.c_str(), systemInfo.domain);
	strncat(sdpText, line, size);
	strncat(sdpText, "a=mediaclk:direct=0\n", size);
	snprintf(line, lineSize, "a=clock-domain:PTPv2 %u\n", systemInfo.domain);
	strncat(sdpText, line, size);
	snprintf(line, lineSize, "a=source-filter: incl IN IP4 %s %s\n", streamInfo.dstIpStr.c_str(), streamInfo.srcIpStr.c_str());
	strncat(sdpText, line, size);
	if (streamInfo.streamType == SMPTE2110_41)
	{
		snprintf(line, lineSize, "a=rtpmap:%u %s/%u\n", streamInfo.payloadType, codec, streamInfo.samplingFrequency);
		packetTimeMs = streamInfo.metadata.packetTimeMs;			
	}
	else
	{
		snprintf(line, lineSize, "a=rtpmap:%u %s/%u/%u\n", streamInfo.payloadType, codec, streamInfo.samplingFrequency, streamInfo.audio.numChannels);
		strncat(sdpText, line, size);
		snprintf(line, lineSize, "a=framecount:%u\n", streamInfo.audio.samplesPerPacket); // samples per packet
		packetTimeMs = (streamInfo.audio.samplesPerPacket * 1000.0) / streamInfo.samplingFrequency;
	}
	strncat(sdpText, line, size);
	snprintf(line, lineSize, "a=ptime:%1.3f\n", packetTimeMs); // time represented by media in packet in ms so derived from above
	strncat(sdpText, line, size);
	strncat(sdpText, "a=recvonly\n", size);
	strncat(sdpText, "a=sync-time:0\n", size);
	if ((streamInfo.streamType == SMPTE2110_41) && !rmax)
	{
		snprintf(line, lineSize, "a=fmtp:%u SSN=ST2110-41:2024; DIT=", streamInfo.payloadType);
		strncat(sdpText, line, size);
		vector<unsigned int>::const_iterator penultimateDit = streamInfo.metadata.dataItemTypes.end() - 1;
		for (vector<unsigned int>::const_iterator dit = streamInfo.metadata.dataItemTypes.begin() ; dit != streamInfo.metadata.dataItemTypes.end() ; dit++)
		{
			snprintf(line, lineSize, "%X", *dit);
			if (dit != penultimateDit)
			{
				strncat(line, ",", lineSize);		
			}
		}
		strncat(line, "\n", lineSize);		
		strncat(sdpText, line, size);		
	}

}

std::ostream& operator<<(std::ostream& os, const Sdp2110& sdp)
{
 //   os << dt.mo << '/' << dt.da << '/' << dt.yr;
	const unsigned int maxSDPSize = 16384;
	char sdpText[maxSDPSize];

	sdp.GetText(sdpText, maxSDPSize);
	os << sdpText;
	//os << "Session Name: " << streamInfo.streamName;
    return os;
}
