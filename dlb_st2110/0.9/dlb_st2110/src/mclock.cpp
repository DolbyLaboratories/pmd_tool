/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2020 by Dolby Laboratories,
 *                Copyright (C) 2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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
