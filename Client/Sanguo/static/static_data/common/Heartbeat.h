/*!
 * \file Heartbeat.h
 *
 * \author laizhisheng
 * \date 2016/12/30 10:43
 *
 */

#pragma once

#include "BaseTypeDefine.h"
#include <chrono>

#define heartbeat_ser_time_zone -5

class Heartbeat
{
public:
	static Heartbeat& getInstance();

	void setServerTimeInMs(NS::I64 serverTimeMs);
	//please use it to get server time milliseconds
	NS::I64 getServerTimeInMs() const;

	void setServerTime(NS::I64 seconds, NS::I64 milliseconds);
	NS::I64 getServerTime() const;
	NS::I64 getServerTimeMs() const;

	int getServerTimeZone() {return m_serverTimeZone;}
	void setServerTimeZone(int timeZone) {m_serverTimeZone = timeZone;}
	void setServerTimeNextDaily(NS::I64 nServerTimeNextDaily) { m_ServerTimeNextDaily = nServerTimeNextDaily; }
	NS::I64 getServerTimeNextDaily() { return m_ServerTimeNextDaily; }
	//[[deprecated("deprecated")]]
	void setServerTimestamp(NS::I64 timestamp);

	//[[deprecated("deprecated")]]
	NS::I64 getServerTimestamp() const;

	void setClientSendTimeMs(NS::I64 timestamp){ m_clientSendTimeMs = timestamp; };
	NS::I64 getClientSendTimeMs() const{return m_clientSendTimeMs; };

	void setClientReceiveTimeMs(NS::I64 timestamp){ m_clientReceiveTimeMs = timestamp; };
	NS::I64 getClientReceiveTimeMs() const{return m_clientReceiveTimeMs; };

	NS::I64 getGamingTime();
private:
	NS::I64 m_ServerTimeNextDaily{};
	NS::I64 m_serverTimeSecs{};
	NS::I64 m_clientTimeMs{};
	NS::I64 m_serverTimestamp{};

	NS::I64 m_clientSendTimeMs{};
	NS::I64 m_clientReceiveTimeMs{};
	std::chrono::steady_clock::time_point m_tmBeg;
	std::chrono::steady_clock::time_point m_tmReceiveServerTimestamp; // time point when receive server's timestamp

	NS::I64 m_serverTimeMs{};

	int m_serverTimeZone{ heartbeat_ser_time_zone };

	NS::I64 m_firstRecPingpongTime{ -1 };
};
