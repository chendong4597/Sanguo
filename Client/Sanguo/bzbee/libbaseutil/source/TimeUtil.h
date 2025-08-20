/*!
 * \file TimeUtil.h
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 03/31/2016
 *
 * 
 */
#pragma once

#include "BaseType.h"
#include <chrono>

#ifndef CHECK_TIME_INTERVAL
#define CHECK_TIME_INTERVAL(interval)\
{\
    static auto time1 = ::time(nullptr);\
    const auto& time2 = ::time(nullptr);\
    if (time2 - time1 < interval)\
    {\
        return;\
    }\
    time1 = time2;\
}
#endif

NS_BEGIN

class TimeUtil
{
public:
	enum CONST_DEFINE
	{
		SECONDS_OF_MINITE = 60,
		SECONDS_OF_HOUR = SECONDS_OF_MINITE * 60,
		SECONDS_OF_DAY = SECONDS_OF_HOUR * 24,
		SECONDS_OF_WEEK = SECONDS_OF_DAY * 7,
		SECONDS_OF_MONTH = SECONDS_OF_DAY * 30,
		SECONDS_OF_YEAR = SECONDS_OF_MONTH * 12,
	};

public:
	static float now();
	static float cacheTimeStamp();
	static float getTimeStamp();

	static UI64 timerReadTicks();
	
	static I64 strToTimestamp(const char* strTime, bool outUTC = true, const tstring& fmt = "%Y-%m-%d %H:%M:%S" , int zone = 0);
	static I64 strToLocStamp(const char* strTime, int zone = 0);
	static tstring timestampToStr(time_t timestamp, bool outUTC = true, const tstring& fmt = "%Y-%m-%d %H:%M:%S" , int zone = 0);
	static std::tm timestampToStrTM(time_t timestamp, bool outUTC = true, int zone = 0);
	static void convertTimestampToStr(tstring& textStr, tstring regexTsStr = "<%t[[:digit:]]{10}>"); // format: <%t1560546000>

	static tstring getTimeDurationStringHMS(I64 totalSeconds);
	static tstring getTimeDurationStringDH(I64 totalSeconds);
	static tstring getTimeDurationStringM(I64 totalSeconds);
	static tstring getTimeDurationStringMS(I64 totalSeconds);
	static tstring getTimeDurationStringDHM(I64 totalSeconds);
	static tstring getTimeDurationStringDHMS(I64 totalSeconds);

	static tstring stamp_to_standard(I64 stampTime);
	static tstring getTimeBySec(I64 totalSeconds);
	static I64 getCurTime();
	static I64 getPassTime(I64 beginTime);
	static int getRemainingSeconds();

	static I64 getTime();
	static UI64 getTimeMicroseconds();

protected:
	static bool initialize();

private:
	static std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
	static std::chrono::time_point<std::chrono::high_resolution_clock> endTime;

};

//=============================================================================
///
/// CTimerDevice
///
//=============================================================================
class CTimerDevice
{
public:
	enum PRECISION_TOKEN_DEF
	{
		PRECISION_SECOND = 1,
		PRECISION_MILLISECOND = 1000,
		PRECISION_MICROSECOND = 1000000
	}; // End of PRECISION_TOKEN_DEF
public:
	CTimerDevice(){}
	virtual ~CTimerDevice(){}
public:
	virtual void StartUp() = 0;
	virtual void ShutDown() = 0;
	virtual double GetElapsed(PRECISION_TOKEN_DEF iPrecision = PRECISION_SECOND) = 0;
};


//=============================================================================
///
/// CTimer
///
//=============================================================================
class CTimer
{
	typedef std::set<CTimerDevice*> DeviceSET;
	typedef DeviceSET::iterator DeviceSET_ITER;
	typedef DeviceSET::const_iterator DeviceSET_CITER;
	typedef DeviceSET::reverse_iterator DeviceSET_RITER;
	typedef DeviceSET::const_reverse_iterator DeviceSET_CRITER;

public:
	CTimer() {}
	~CTimer();

public:
	//-------------------------------------------------------------------------
	/// Create device.
	///
	/// @param bAutoStart [in] start up automatically
	///
	/// @return new device @sa CTimerDevice
	//-------------------------------------------------------------------------
	CTimerDevice* CreateDevice(bool bAutoStart = false);

	//-------------------------------------------------------------------------
	/// Destroy device.
	///
	/// @param pUnwanted [in] unwanted document
	///
	/// @return None
	//-------------------------------------------------------------------------
	void DestroyDevice(CTimerDevice* pUnwanted);

private:
	DeviceSET m_Devices;
}; // End of CTimer


void StartMicroSecondTimer();
int StopMicroSecondTimer();


NS_END
