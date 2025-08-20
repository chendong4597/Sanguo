/*!
 * \file TimeUtil_Android.cpp
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 03/31/2016
 *
 * 
 */
#include "TimeUtil_Android.h"
#include <chrono>

NS_BEGIN

static auto g_initTime = std::chrono::steady_clock::now();

float TimeUtil::now()
{
	static bool s_bInitialized = initialize();

	auto currTime = std::chrono::steady_clock::now();
	std::chrono::duration<double> diff = currTime - g_initTime;

	return static_cast<float>(diff.count());
}

bool TimeUtil::initialize()
{
	return true;
}

NS_END
