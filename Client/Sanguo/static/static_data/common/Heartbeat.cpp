/*!
 * \file Heartbeat.cpp
 *
 * \author laizhisheng
 * \date 2016/12/30 10:44
 *
 */
#include "stdafx.h"
#include "Heartbeat.h"
#include <ctime>
#include "BaseType.h"

using namespace NS;
using namespace std::chrono;

Heartbeat& Heartbeat::getInstance()
{
	static Heartbeat heartbeat;
	return heartbeat;
}

void Heartbeat::setServerTimeInMs(NS::I64 serverTimeMs)
{
	m_tmBeg = steady_clock::now();
	m_serverTimeMs = serverTimeMs;
}

NS::I64 Heartbeat::getServerTimeInMs() const
{
	return m_serverTimeMs + duration_cast<milliseconds>(steady_clock::now() - m_tmBeg).count();
}

void Heartbeat::setServerTime(NS::I64 seconds, NS::I64 milliseconds)
{
	m_serverTimeSecs = seconds;
	m_clientTimeMs = milliseconds;
	if (m_firstRecPingpongTime <= 0)
	{
		m_firstRecPingpongTime = m_serverTimeSecs;
	}
	m_tmBeg = steady_clock::now();
}

NS::I64 Heartbeat::getServerTime() const
{
	return m_serverTimeSecs + duration_cast<seconds>(steady_clock::now() - m_tmBeg).count();
}

NS::I64 Heartbeat::getServerTimeMs() const
{
	return m_clientTimeMs + duration_cast<milliseconds>(steady_clock::now() - m_tmBeg).count();
}

void Heartbeat::setServerTimestamp(NS::I64 timestamp)
{
	m_serverTimestamp = timestamp;
	m_tmReceiveServerTimestamp = steady_clock::now();
}

NS::I64 Heartbeat::getServerTimestamp() const
{
	return m_serverTimestamp + duration_cast<milliseconds>(steady_clock::now() - m_tmReceiveServerTimestamp).count();
}

NS::I64 Heartbeat::getGamingTime()
{
	if (m_firstRecPingpongTime <= 0)
	{
		return 0;
	}

	return m_serverTimeSecs - m_firstRecPingpongTime;
}

/////////////////////////////////////////////////////////////////////////
//Lua开放接口

//获取服务器时间，秒
NS::I64 LUA(Heartbeat_GetServerTime)()
{
	return Heartbeat::getInstance().getServerTime();
}

int LUA(Heartbeat_GetServerYear)()
{
	NS::I64 serTm = Heartbeat::getInstance().getServerTime() + Heartbeat::getInstance().getServerTimeZone() * 3600;
	std::tm tempTm = *std::gmtime((const time_t*)(&serTm));
	return 1900 + tempTm.tm_year;
}


int LUA(Heartbeat_GetServerMonth)()
{
	NS::I64 serTm = Heartbeat::getInstance().getServerTime() + Heartbeat::getInstance().getServerTimeZone() * 3600;
	std::tm tempTm = *std::gmtime((const time_t*)(&serTm));
	return tempTm.tm_mon + 1;
}


