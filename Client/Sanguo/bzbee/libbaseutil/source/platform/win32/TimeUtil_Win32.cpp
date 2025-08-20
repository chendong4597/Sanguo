/*!
 * \file TimeUtil_Win32.cpp
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 03/31/2016
 *
 * 
 */
#include "TimeUtil_Win32.h"
#include <windows.h>

NS_BEGIN

static double g_timerInvFreq;
static bool g_timerUsingQPF;

float TimeUtil::now()
{
	static UI64 startTicks = -1;
	if (startTicks == -1)
	{
		initialize();
		startTicks = timerReadTicks();
	}
	UI64 nowTicks = timerReadTicks();

	return static_cast<float>((float)(nowTicks - startTicks) * g_timerInvFreq);
}

bool TimeUtil::initialize()
{
	LARGE_INTEGER ticksPerSec;
	g_timerUsingQPF = QueryPerformanceFrequency(&ticksPerSec) ? true : false;
	if (g_timerUsingQPF)
		g_timerInvFreq = 1.0 / (double)ticksPerSec.QuadPart;
	else
		g_timerInvFreq = 1.0 / 1000.0;

	return true;
}

UI64 TimeUtil::timerReadTicks()
{
	if (g_timerUsingQPF)
	{
		LARGE_INTEGER qpft;
		QueryPerformanceCounter(&qpft);
		return qpft.QuadPart;
	}
	else
	{
		return timeGetTime();
	}
}

NS_END