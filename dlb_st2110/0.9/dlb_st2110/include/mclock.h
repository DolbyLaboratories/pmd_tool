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

#ifndef _MCLOCK_H_
#define _MCLOCK_H_


#include <time.h>
#include <stdint.h>
#include <sys/timex.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <array>
#include <memory>
#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "dlb_st2110_logging.h"

void printSleepTimes(void);

class MClock
{

public:

	class TimePoint;

	class Duration
	{
	private:
		int64_t nsec;
		int64_t sec;

	public:
		Duration(void)
		{
			nsec = 0;
			sec = 0;
		}

		Duration(uint64_t newNsec)
		{
			setNanoseconds(newNsec);
		}

		Duration(int64_t newSec, int64_t newNsec)
		{
			sec = newSec;
			nsec = newNsec;
		}

		void setNanoseconds(uint64_t newNsec)
		{
			sec = newNsec / 1000000000;
			nsec = newNsec % 1000000000;
		}

		void setMicroseconds(uint64_t newUsec)
		{
			sec = newUsec / 1000000;
			nsec = (newUsec % 1000000) * 1000;
		}

		void setMilliseconds(uint64_t newMsec)
		{
			sec = newMsec / 1000;
			nsec = (newMsec % 1000) * 1000000;
		}

		void setSeconds(uint64_t newSec)
		{
			sec = newSec;
			nsec = 0;
		}

		void setSeconds(int newSec)
		{
			// take absolute value rather than risk rollover
			sec = abs(newSec);
			nsec = 0;
		}

		void setSeconds(float newSec)
		{
			sec = floor(newSec);
			nsec = round(1000000000 * (newSec - (float)sec));
		}

		void setSeconds(double newSec)
		{
			sec = floor(newSec);
			nsec = round(1000000000 * (newSec - (double)sec));
		}

		Duration operator+(const Duration& offset)
		{
			Duration sum;

			sum.nsec = (nsec + offset.nsec) % 1000000000;
			sum.sec  = (nsec + offset.nsec) / 1000000000;
			sum.sec += (sec + offset.sec);
			return(sum);
		}

		Duration operator*(const double& scaler)
		{
			Duration product1, product2, sum;

			product1.nsec = ((uint64_t)(nsec * scaler)) % 1000000000;
			product1.sec  = (nsec * scaler) / 1000000000;
			product2.nsec = ((uint64_t)(sec * scaler * 1000000000)) % 1000000000;
			product2.sec  = (sec * scaler * 1000000000) / 1000000000;
			sum = product1 + product2;
			return(sum);
		}

		bool operator >(const Duration& duration)
		{
			if (sec > duration.sec)
			{
				return true;
			}
			else if (sec < duration.sec)
			{
				return false;
			}
			else
			{
				return(nsec > duration.nsec);
			}
		}


		std::string GetString(void) const // The last const is so guarantee to the << operator that the members won't change
		{
			return ("sec: " + std::to_string(sec) + " nsec:" + std::to_string(nsec));
		}

		friend class TimePoint;

	};

	class TimePoint
	{
	private:
		// TIME in UTC
		// All conversions to TAI handled manually
		uint64_t nsec;
		uint64_t sec;
		unsigned int UTCOffset;

		void get_tai_offset(void)
		{
			struct timex t = { 0 };
			if (adjtimex(&t) == -1)
			{
				throw std::runtime_error("adjtimex() failed");
			}
			UTCOffset = t.tai;
			if (UTCOffset < 37)
			{
				throw std::runtime_error("UTC Offset incorrectly set in kernel. Use adjtimex to correct");
			}
		}	


	public:

		TimePoint(void)
		{
			sec = 0;
			nsec = 0;
			get_tai_offset();			
		}

		TimePoint(uint64_t newNsec)
		{
			sec = newNsec  / 1000000000L;
			nsec = newNsec % 1000000000L;
			get_tai_offset();
		}

		TimePoint(uint64_t newSec, uint64_t newNsec)
		{
			sec = newSec;
			nsec = newNsec;
			get_tai_offset();
		}

		TimePoint(uint64_t newSec, uint64_t newNsec, unsigned int newUTCOffset)
		{
			sec = newSec;
			nsec = newNsec;
			UTCOffset = newUTCOffset;
		}

		void SetNow(void)
		{
			struct timespec timeNow;
			// Get time in UTC
			clock_gettime(CLOCK_REALTIME, &timeNow);
			sec = timeNow.tv_sec;
			nsec = timeNow.tv_nsec;
		}

		uint32_t GetRTPTimeStamp48kHz(void) const
		{
			uint32_t timestamp;
			// Add UTCOffset to convert to TAI
			uint64_t microseconds = (sec + UTCOffset) * 1000000 + (nsec / 1000);
			uint64_t samples = (microseconds * 48) / 1000;
			timestamp = (uint32_t)samples & 0xffffffff;
			return(timestamp);
		}

		TimePoint operator+(const Duration& offset)
		{
			TimePoint sum(0,0,0);
			sum.UTCOffset = UTCOffset;
			sum.nsec = (nsec + offset.nsec) % 1000000000;
			sum.sec  = (nsec + offset.nsec) / 1000000000;
			sum.sec += (sec + offset.sec);
			return(sum);			
		}

		TimePoint operator-(const Duration& offset)
		{
			TimePoint diff(0,0,0);
			diff.UTCOffset = UTCOffset;
			int64_t nsecDiff = nsec - offset.nsec;
			diff.nsec = (nsecDiff + 1000000000) % 1000000000; //(nsec - offset.nsec) % 1000000000;
			if (nsecDiff < 0)
			{
				diff.sec = sec - offset.sec  - 1;
			}
			else
			{
				diff.sec = sec - offset.sec;
			}
			return(diff);			
		}

		Duration operator-(const TimePoint& time)
		{
			int64_t diffNsec = nsec - time.nsec;
			int64_t diffSec = sec - time.sec;
			Duration difference;
			if (diffNsec < 0)
			{
				difference.nsec = 1000000000 + diffNsec;
				difference.sec  = sec - time.sec - 1;
			}
			else
			{
				if (diffSec < 0)
				{
					difference.nsec = 1000000000 - diffNsec;
			 		difference.sec  = sec - time.sec - 1;
				}
				else
				{
					difference.nsec = diffNsec;
			 		difference.sec  = sec - time.sec;
			 	}
			}
			return(difference);			
		}

		void SetRTPTimeStamp48kHz(uint32_t timestamp)
		{
			SetNow();
			uint32_t nowTs = GetRTPTimeStamp48kHz();
			// Take a signed difference here so taking closest time to now either forwards or backwards
			int64_t nowTsDiff = (int64_t)timestamp - (int64_t)nowTs;
			if (nowTsDiff < 0)
			{
				Duration nowTsDiffDur(20833L * -nowTsDiff);
				*this = *this - nowTsDiffDur;
			}
			else
			{
				Duration nowTsDiffDur(20833L * nowTsDiff);
				*this = *this + nowTsDiffDur;
			}
		}


		bool operator >(const TimePoint& time)
		{
			if (sec > time.sec)
			{
				return true;
			}
			else if (sec < time.sec)
			{
				return false;
			}
			else
			{
				return(nsec > time.nsec);
			}
		}

		bool operator <(const TimePoint& time)
		{
			if (sec < time.sec)
			{
				return true;
			}
			else if (sec > time.sec)
			{
				return false;
			}
			else
			{
				return(nsec < time.nsec);
			}
		}


		uint64_t GetNanoSeconds(void) const
		{
			return((sec * 1000000000) + nsec);
		}

		void SetNanoSeconds(uint64_t newNsec)
		{
			sec = newNsec  / 1000000000L;
			nsec = newNsec % 1000000000L;
		}

		std::string GetString(void) const // The last const is so guarantee to the << operator that the members won't change
		{
			return ("sec: " + std::to_string(sec) + " nsec:" + std::to_string(nsec));
		}

		void SleepUntil(void);

	};

private:

	static unsigned int get_tai_offset(void)
	{
		struct timex t = { 0 };

		if (adjtimex(&t) == -1)
		{
			throw std::runtime_error("adjtimex failed");
		}
		return(t.tai);
	}

	static void set_tai_offset(unsigned int offset)
	{
		struct timex t = { 0 };

		t.modes = ADJ_TAI;
		t.constant = offset;
		if (adjtimex(&t) == -1)
		{
			throw std::runtime_error("adjtimex failed");
		}
	}


	static std::string exec(std::string cmd)
		{
	    std::array<char, 128> buffer;
	    std::string result;
	    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
	    if (!pipe)
	    {
	        throw std::runtime_error("popen() failed!");
	    }
	    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
	        result += buffer.data();
	    }
	    return result;
	}



	static std::string PmcGet(const char *variable, const char *key)
	{
		char key_with_ws[256];

		strncpy(key_with_ws,"\t", 256);
		strncat(key_with_ws, key, 256);
		strncat(key_with_ws, " ", 256);
		std::string cfgText = exec(std::string("sudo pmc -u -b 0 'GET ") + variable + std::string("'"));
		//std::ifstream t("audio_md_system.cfg");
		//std::string cfgText((std::istreambuf_iterator<char>(t)),std::istreambuf_iterator<char>());
		if (cfgText.length() == 0)
		{
			throw std::runtime_error("PmcGet Failed");
		}
		std::string::size_type start = cfgText.rfind(key_with_ws) + strlen(key_with_ws);
		std::string::size_type end = cfgText.find("\n",start);
		std::string result = cfgText.substr(start,end-start);
		// remove all white space
		result.erase(remove_if(result.begin(),result.end(), isspace), result.end());
		return(result);
	}


public:

	static void CheckTaiOffset(void)
	{
		unsigned int offset, offsetFromPmc;
		std::string tmpStr;

		tmpStr = PmcGet("TIME_PROPERTIES_DATA_SET", "currentUtcOffset");
		offsetFromPmc = std::stoi(tmpStr, NULL, 10);

		offset = get_tai_offset();
		if (offset != offsetFromPmc)
		{
			CLOG(WARNING, HARDWARE_LOG) << "UTC Offset wrong in kernal, was " << offset << ", setting it to " << offsetFromPmc << " from PMC" << std::endl;
			set_tai_offset(offsetFromPmc);
			offset = get_tai_offset();
			CLOG(INFO, HARDWARE_LOG) << "New UTC-TAI offset in kernel: " << offset << std::endl;
		}
		else
		{
			CLOG(INFO, HARDWARE_LOG) << "UTC Offset correctly set in kernal as " << offset << " seconds" << std::endl;
		}

	}

};

inline std::ostream& operator<<(std::ostream& stream, const MClock::TimePoint& time)
{
	stream << time.GetString();
	return(stream);
}

inline std::ostream& operator<<(std::ostream& stream, const MClock::Duration& duration)
{
	stream << duration.GetString();
	return(stream);
}

#endif // _M_CLOCK_H_
