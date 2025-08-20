/*!
 * \file TimeUtil.cpp
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 03/31/2016
 *
 *
 */
#include "TimeUtil.h"
#include "StringUtil.h"
#include <regex>
#include <ctime>
#include "format.h"
#if defined(_MSC_VER)
#include <time.h>
#include <Windows.h>
#elif defined(__GNUC__)
#include <sys/time.h> // for gettimeofday
#endif


NS_BEGIN


static float s_timeStamp{};


std::chrono::time_point<std::chrono::high_resolution_clock> TimeUtil::startTime = std::chrono::high_resolution_clock::now();
std::chrono::time_point<std::chrono::high_resolution_clock> TimeUtil::endTime = std::chrono::high_resolution_clock::now();

float TimeUtil::cacheTimeStamp()
{
	s_timeStamp = now();
	return s_timeStamp;
}


float TimeUtil::getTimeStamp()
{
	return s_timeStamp;
}


// %Y-%m-%d %H:%M:%S = "2017-03-01 11:10:44"
I64 TimeUtil::strToTimestamp(const char* strTime, bool outUTC /*= true*/, const tstring& fmt /*= "%Y-%m-%d %H:%M:%S"*/, int zone)
{
	if (!strTime) { return 0; }

	std::tm tempTm{};
	std::tm localTm{};
	std::tm utcTm{};
	time_t localStamp, utcStamp, retStamp, tempStamp;

	//std::istringstream ss(strTime);
	//ss >> std::get_time(&tempTm, fmt.c_str());  // APK Build Error, NDK gcc4.9 Unfulfilled get_time

	sscanf(strTime, "%d-%d-%d %d:%d:%d", &tempTm.tm_year, &tempTm.tm_mon, &tempTm.tm_mday, &tempTm.tm_hour, &tempTm.tm_min, &tempTm.tm_sec);
	tempTm.tm_year -= 1900;
	tempTm.tm_mon -= 1;

	tempStamp = time(nullptr);

	localTm = *std::localtime(&tempStamp);
	localStamp = std::mktime(&localTm);

	utcTm = *std::gmtime(&tempStamp);
	utcStamp = std::mktime(&utcTm);

	retStamp = std::mktime(&tempTm);

	if (outUTC) return retStamp;
	else return retStamp + (localStamp - utcStamp) - zone*3600;
}

I64 TimeUtil::strToLocStamp(const char* strTime, int zone)
{
	if (!strTime) { return 0; }

	std::tm tempTm{};
	std::tm localTm{};
	std::tm utcTm{};
	time_t localStamp, utcStamp, retStamp, tempStamp;

	//std::istringstream ss(strTime);
	//ss >> std::get_time(&tempTm, fmt.c_str());  // APK Build Error, NDK gcc4.9 Unfulfilled get_time

	sscanf(strTime, "%d-%d-%d %d:%d:%d", &tempTm.tm_year, &tempTm.tm_mon, &tempTm.tm_mday, &tempTm.tm_hour, &tempTm.tm_min, &tempTm.tm_sec);
	tempTm.tm_year -= 1900;
	tempTm.tm_mon -= 1;

	tempStamp = time(nullptr);

	localTm = *std::localtime(&tempStamp);
	localStamp = std::mktime(&localTm);

	utcTm = *std::gmtime(&tempStamp);
	utcStamp = std::mktime(&utcTm);

	retStamp = std::mktime(&tempTm);

	//auto tm = retStamp + (localStamp - utcStamp);
	return retStamp + (localStamp - utcStamp) - zone *3600;
}


tstring TimeUtil::timestampToStr(time_t timestamp, bool outUTC /*= true*/, const tstring& fmt /*= "%Y-%m-%d %H:%M:%S"*/, int zone)
{
	char strReturn[100];
	std::tm tempTm;

	timestamp = timestamp + zone * 3600;

	tempTm = (outUTC) ? *std::gmtime(&timestamp) : *std::localtime(&timestamp);

	if (std::strftime(strReturn, sizeof(strReturn), fmt.c_str(), &tempTm))
	{
		return strReturn;
	}

	return EMPTY_STRING;
}

std::tm TimeUtil::timestampToStrTM(time_t timestamp, bool outUTC, int zone)
{
	std::tm tempTm;
	timestamp = timestamp + zone * 3600;
	tempTm = (outUTC) ? *std::gmtime(&timestamp) : *std::localtime(&timestamp);

	return tempTm;
}

tstring TimeUtil::getTimeDurationStringHMS(I64 totalSeconds)
{
	int hour = (int)(totalSeconds / SECONDS_OF_HOUR);
	totalSeconds %= SECONDS_OF_HOUR;

	int minite = (int)(totalSeconds / SECONDS_OF_MINITE);
	totalSeconds %= SECONDS_OF_MINITE;

	return fmt::format("{0:02d}:{1:02d}:{2:02d}", hour, minite, totalSeconds);
}

tstring TimeUtil::getTimeDurationStringM(I64 totalSeconds)
{
	int minite = (int)(totalSeconds / SECONDS_OF_MINITE);

	return fmt::format("{0}", minite);
}

tstring TimeUtil::getTimeDurationStringMS(I64 totalSeconds)
{
	int hour = (int)(totalSeconds / SECONDS_OF_HOUR);
	totalSeconds %= SECONDS_OF_HOUR;

	int minite = (int)(totalSeconds / SECONDS_OF_MINITE);
	totalSeconds %= SECONDS_OF_MINITE;

	if (hour > 0)
		return fmt::format("{0:02d}:{1:02d}:{2:02d}", hour, minite, totalSeconds);
	return fmt::format("{0:02d}:{1:02d}", minite, totalSeconds);
}

tstring TimeUtil::getTimeDurationStringDH(I64 totalSeconds)
{
	int day = (int)(totalSeconds / SECONDS_OF_DAY);
	totalSeconds %= SECONDS_OF_DAY;

	int hour = (int)(totalSeconds / SECONDS_OF_HOUR);
	totalSeconds %= SECONDS_OF_HOUR;

	return fmt::format("{0:02d}d {1:02d}h", day, hour);
}

tstring TimeUtil::getTimeDurationStringDHM(I64 totalSeconds)
{
	int day = (int)(totalSeconds / SECONDS_OF_DAY);
	totalSeconds %= SECONDS_OF_DAY;

	int hour = (int)(totalSeconds / SECONDS_OF_HOUR);
	totalSeconds %= SECONDS_OF_HOUR;

	int minite = (int)(totalSeconds / SECONDS_OF_MINITE);
	totalSeconds %= SECONDS_OF_MINITE;

	tstring ret;
	if (day > 0)
	{
		ret += fmt::format("{0:02d}d", day);
	}
	if (day > 0 || hour > 0)
	{
		ret += fmt::format("{0:02d}h", hour);
	}
	if (day > 0 || hour > 0 || minite >= 0)
	{
		ret += fmt::format("{0:02d}m", minite);
	}

	return ret;
}

tstring TimeUtil::stamp_to_standard(I64 stampTime /*= "%Y-%m-%d %H:%M"*/)
{
	time_t tick = (time_t)stampTime;
	struct tm tm;
	char s[100];

	tm = *localtime(&tick);
	strftime(s, sizeof(s), "%Y-%m-%d %H:%M", &tm);

	return s;
}

I64 TimeUtil::getCurTime()
{
	return time(nullptr);
}

I64 TimeUtil::getTime()
{
	endTime = std::chrono::high_resolution_clock::now();

	return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
}

UI64 TimeUtil::getTimeMicroseconds()
{
	endTime = std::chrono::high_resolution_clock::now();

	return std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
}

I64 TimeUtil::getPassTime(I64 beginTime)
{
	return getCurTime() - beginTime;
}

int TimeUtil::getRemainingSeconds()
{
	auto now = time(nullptr);

	struct tm utc_tmNow;

	utc_tmNow = *gmtime(&now);

	utc_tmNow.tm_year = 0;
	utc_tmNow.tm_mon = 0;
	utc_tmNow.tm_mday = 0;
	utc_tmNow.tm_wday = 0;
	utc_tmNow.tm_yday = 0;

	auto elapsed = utc_tmNow.tm_hour * 3600 + utc_tmNow.tm_min * 60 + utc_tmNow.tm_sec;

	const time_t oneDay = 86400;

	auto remaining = oneDay - elapsed;

	return (int)remaining;
}

tstring TimeUtil::getTimeBySec(I64 totalSeconds)
{
	int days = (int)(totalSeconds / SECONDS_OF_DAY);
	totalSeconds %= SECONDS_OF_DAY;

	int hour = (int)(totalSeconds / SECONDS_OF_HOUR);
	totalSeconds %= SECONDS_OF_HOUR;

	int minite = (int)(totalSeconds / SECONDS_OF_MINITE);
	totalSeconds %= SECONDS_OF_MINITE;

	tstring out{};
	if (days > 0)
	{
		auto key = _LC("CLI_LeftTime_day{0}");
		out = fmt::format(key, days);
		return out;
	}
	if (hour > 0)
	{
		auto key = _LC("CLI_LeftTime_hour{0}");
		out = fmt::format(key, hour);
		return out;
	}
	if (minite > 0)
	{
		auto key = _LC("CLI_LeftTime_min{0}");
		out = fmt::format(key, minite);
		return out;
	}
	auto key = _LC("CLI_LeftTime_sec{0}");
	out = fmt::format(key, totalSeconds);
	return out;
}


void TimeUtil::convertTimestampToStr(tstring& textStr, tstring regexTsStr)
{
	tstring tempStr{ textStr };
	std::vector<std::tuple<tstring, tstring>> tsTupleVec;

	std::smatch smTs;
	std::regex regexTs(regexTsStr);
	const int TS_TOTAL_NUM{14};
	const int TS_BEGIN{3};
	const int TS_END{12};
	while (regex_search(tempStr, smTs, regexTs))
	{
		tstring uclearTs = smTs.str();

		// 得到 时间戳
		if (uclearTs.length() != TS_TOTAL_NUM) // format: <%t1560546000>
		{
			tsTupleVec.push_back(std::tuple<tstring, tstring>(uclearTs, "_DATE_"));
			continue;
		}

		I64 nTs{};
		StringUtil::str2int64(nTs, uclearTs.substr(TS_BEGIN, TS_END).c_str());

		tsTupleVec.push_back(std::tuple<tstring, tstring>(uclearTs, timestampToStr(nTs, false)));

		// 处理下一个
		tempStr = smTs.suffix();
	}

	// replace Ts
	for (auto&& tsTuple : tsTupleVec)
	{
		StringUtil::replaceString(textStr, std::get<0>(tsTuple), std::get<1>(tsTuple));
	}
}

tstring TimeUtil::getTimeDurationStringDHMS(I64 totalSeconds)
{
	int day = (int)(totalSeconds / SECONDS_OF_DAY);
	totalSeconds %= SECONDS_OF_DAY;

	int hour = (int)(totalSeconds / SECONDS_OF_HOUR);
	totalSeconds %= SECONDS_OF_HOUR;

	int minite = (int)(totalSeconds / SECONDS_OF_MINITE);
	totalSeconds %= SECONDS_OF_MINITE;

	tstring ret;
	if (day > 0)
	{
		ret += fmt::format("{0:02d}d:", day);
	}
	if (hour >= 0)
	{
		ret += fmt::format("{0:02d}:", hour);
	}
	if (minite >= 0)
	{
		ret += fmt::format("{0:02d}:", minite);
	}
	if (totalSeconds >= 0)
	{
		ret += fmt::format("{0:02d}", totalSeconds);
	}
	return ret;
}

//=============================================================================
///
/// CTimerDeviceImp
///
//=============================================================================
class CTimerDeviceImp : public CTimerDevice
{
public:
	CTimerDeviceImp();
	virtual ~CTimerDeviceImp();
public:
	virtual void StartUp();
	virtual void ShutDown();
	virtual double GetElapsed(PRECISION_TOKEN_DEF iPrecision = PRECISION_SECOND);
private:
	//-------------------------------------------------------------------------
	/// Get time of either starting or difference.
	///
	/// @param dPrevious [in] previous time
	///
	/// @return time
	//-------------------------------------------------------------------------
	double GetTime(double dPrevious = 0.0);

private:
	bool m_bContinue;
	double m_dStarting;
	double m_dDifference;

#if defined(_MSC_VER)
	LARGE_INTEGER m_Frequency;
#endif
}; // End of CTimer



//=============================================================================
//
// Implementation of CTimerDevice
//
//=============================================================================
CTimerDeviceImp::CTimerDeviceImp()
	: m_bContinue(false)
	, m_dStarting(0.0)
	, m_dDifference(0.0)
{
#if defined(_MSC_VER)
	QueryPerformanceFrequency(&m_Frequency);
#endif
} // End of Constructor for CTimerDeviceImp

CTimerDeviceImp::~CTimerDeviceImp()
{

}

//-----------------------------------------------------------------------------
void
CTimerDeviceImp::StartUp()
{
	m_bContinue = true;

	m_dStarting = GetTime();
} // End of StartUp for CTimerDeviceImp

//-----------------------------------------------------------------------------
void
CTimerDeviceImp::ShutDown()
{
	if (false == m_bContinue)
		return;

	m_dDifference = GetTime(m_dStarting);

	m_bContinue = false;
} // End of ShutDown for CTimerDeviceImp

//-----------------------------------------------------------------------------
double
CTimerDeviceImp::GetElapsed(PRECISION_TOKEN_DEF iPrecision)
{
	if (m_bContinue)
		m_dDifference = GetTime(m_dStarting);

	return m_dDifference * iPrecision;
} // End of GetElapsed for CTimerDeviceImp

//-----------------------------------------------------------------------------
double
CTimerDeviceImp::GetTime(double dPrevious)
{
#if defined(_MSC_VER)
	static LARGE_INTEGER Time;
	QueryPerformanceCounter(&Time);
	return ((double)Time.QuadPart / (double)m_Frequency.QuadPart) - dPrevious;
#endif

#if defined(__GNUC__)
	static timeval Time;
	gettimeofday(&Time, NULL);
	return ((double)Time.tv_sec + (double)Time.tv_usec / 1000000.0) - dPrevious;
#endif
} // End of GetTime for CTimerDeviceImp



//=============================================================================
//
// CTimer
//
//=============================================================================
CTimer::~CTimer()
{
	for (DeviceSET_CITER i = m_Devices.begin(); i != m_Devices.end(); ++i)
	{
		CTimerDevice* const& pDevice = *i;
		assert(pDevice);

		delete pDevice;
	} // End for

	m_Devices.clear();
} // End of

//-----------------------------------------------------------------------------
CTimerDevice* CTimer::CreateDevice(bool bAutoStart)
{
	CTimerDevice* pDevice = new CTimerDeviceImp();
	assert(pDevice);
	assert(m_Devices.find(pDevice) == m_Devices.end());

	m_Devices.insert(pDevice);

	if (bAutoStart)
		pDevice->StartUp();

	return pDevice;
} // End of CreateDevice for CTimer

//-----------------------------------------------------------------------------
void CTimer::DestroyDevice(CTimerDevice* pUnwanted)
{
	if (NULL == pUnwanted)
		return;

	DeviceSET_ITER found = m_Devices.find(pUnwanted);
	if (m_Devices.end() == found)
		return;

	m_Devices.erase(found);

	delete pUnwanted;
} // End of DestroyDevice for CTimer


static CTimer s_Timer;
static CTimerDevice* s_pTimerDevice = s_Timer.CreateDevice(true);

void
StartMicroSecondTimer()
{
	s_pTimerDevice->StartUp();
} // End of StartMicroSecondTimer

int
StopMicroSecondTimer()
{
	return (int)s_pTimerDevice->GetElapsed(CTimerDevice::PRECISION_MICROSECOND);
} // End of StopMicroSecondTimer



NS_END

