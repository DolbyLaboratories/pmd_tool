/************************************************************************
 * dlb_st2110
 * Copyright (c) 2020, Dolby Laboratories Inc.
 * Copyright (c) 2020, Dolby International AB.
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

#include "mclock.h"
#include <ostream>

using namespace std;

void MClock::TimePoint::SleepUntil(void)
{
	timespec ts;
	ts.tv_sec = sec;
	ts.tv_nsec = nsec;

	#ifdef CHECKING_LATE_TIMES
	MClock::TimePoint now(0,0,37); // The UTC offset isn't used but setting it here saves time
	#endif

	clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &ts, NULL);
	#ifdef CHECKING_LATE_TIMES
	now.SetNow();
	MClock::Duration delay = now - *this;
	MClock::Duration oneMs;
	oneMs.setMilliseconds(10);
	if (delay > oneMs)
	{
		CLOG(WARNING, HARDWARE) << "Wakeup late, delay = " << delay;
	}
	#endif

}