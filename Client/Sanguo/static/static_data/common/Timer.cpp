/*!
 * \file Timer.cpp
 *
 * \author laizhisheng
 * \date 2016/09/07 9:29
 *
 */
#include "stdafx.h"
#include "Timer.h"

using namespace NS;



Timer::Timer()
{
}

Timer::~Timer()
{
}

Timer& Timer::getInstance()
{
	static Timer timer;
	return timer;
}

void Timer::update(float dt)
{
	if (isSwitchScene())
	{
		// 目前切地图时，定时器（Timer）触发事件之后，会引起一些崩溃，所以在切地图时定时器（Timer）将不再update
		return;
	}

	for (auto timerId : m_removeTimerId)
	{
		if (m_timerInfoMap.find(timerId) != m_timerInfoMap.end())
		{
			m_timerInfoMap.erase(timerId);
		}
	}
	m_removeTimerId.clear();

	for (auto itor = m_timerInfoMap.begin(); itor != m_timerInfoMap.end();)
	{
		itor->second.m_fDetailTime += dt;
		if (itor->second.m_state == TimerEvent::EventType::Unknown)
		{
			itor->second.m_state = TimerEvent::EventType::Begin;
			TimerEvent evt(TimerEvent::EventType::Begin, itor->first, itor->second.m_fRemainTime, dt);
			itor->second.m_callbackTimer(evt);
		}
		else
		{
			if (itor->second.m_fDetailTime >= itor->second.m_fDurationTime)
			{
				itor->second.m_fRemainTime -= itor->second.m_fDetailTime;

				if (itor->second.m_fRemainTime <= 0)
				{
					itor->second.m_state = TimerEvent::EventType::End;
					TimerEvent evt(TimerEvent::EventType::End, itor->first, itor->second.m_fRemainTime, itor->second.m_fDetailTime);
					itor->second.m_callbackTimer(evt);

					itor = m_timerInfoMap.erase(itor);
					continue;
				}
				else
				{
					itor->second.m_state = TimerEvent::EventType::Tick;
					TimerEvent evt(TimerEvent::EventType::Tick, itor->first, itor->second.m_fRemainTime, itor->second.m_fDetailTime);
					itor->second.m_callbackTimer(evt);
				}

				itor->second.m_fDetailTime = 0.0f;
			}
		}
		++itor;
	}
}

bzbee::uint Timer::updateTimerInfo(const TimerInfo& info, bzbee::uint timerId /*= UINT_MAX*/)
{
	static bzbee::uint s_timerId = 1000;
	if (timerId == UINT_MAX)
	{
		timerId = ++s_timerId;
	}

	m_timerInfoMap[timerId] = info;

	return timerId;
}

bool Timer::hasTimer(bzbee::uint timerId)
{
	if (std::find(m_removeTimerId.begin() , m_removeTimerId.end() , timerId) != m_removeTimerId.end()) { return false; }

	return m_timerInfoMap.find(timerId) != m_timerInfoMap.end();
}

bzbee::uint Timer::addTimer(TimerIdHost* host, float totalTime, const CallbackTimer& callbackTimer, float durationTime /*= 0.0f*/)
{
	TimerInfo info;
	info.m_fRemainTime = totalTime;
	info.m_fDurationTime = durationTime;
	info.m_callbackTimer = callbackTimer;
	
	bzbee::uint timerId = updateTimerInfo(info, UINT_MAX);
	host->addTimerId(timerId);

	return timerId;
}

bzbee::uint Timer::updateTimer(TimerIdHost* host, bzbee::uint timerId, float totalTime, const CallbackTimer& callbackTimer, float durationTime /*= 0.0f*/)
{
	TimerInfo info;
	info.m_fRemainTime = totalTime;
	info.m_fDurationTime = durationTime;
	info.m_callbackTimer = callbackTimer;
	
	timerId = updateTimerInfo(info, timerId);
	host->addTimerId(timerId);

	return timerId;
}

void Timer::removeTimer(bzbee::uint timerId)
{
	if (std::find(m_removeTimerId.begin() , m_removeTimerId.end() , timerId) == m_removeTimerId.end())
	{
		m_removeTimerId.push_back(timerId);
	}
}

void Timer::setSwitchScene(bool isSwitch)
{ 
	m_isSwitchScene = isSwitch; 
};

bool Timer::isSwitchScene() 
{ 
	return m_isSwitchScene;
};


//////////////////////////////////////////////////////////////////////////


RegisterTimer::RegisterTimer(Timer::TimerIdHost* pTimerHost, float totalTime, float tick)
	: m_pTimerHost(pTimerHost)
	, m_totalTime(totalTime)
	, m_tick(tick)
{}


uint RegisterTimer::operator<< (const OnTimer& func)
{
	auto pTimerHost = m_pTimerHost;
	auto wrapper = [func, pTimerHost](void*, TimerEvent& evt)
	{
		// Note: Unregister is not necessary.
		if (evt.m_type == TimerEvent::EventType::End)
		{
			Timer::TimerIdHost& __timer_host__ = *pTimerHost;
			auto timerId = evt.m_timerId;
			UNREGISTER_TIMER(timerId);
		}

		func(evt);
	};

	uint timerId{};

	Timer::TimerIdHost& __timer_host__ = *pTimerHost;
	REGISTER_TIMER(timerId, m_totalTime, m_tick, wrapper);

	return timerId;
}


uint RegisterTimer::operator>> (const OnDelay& func)
{
	return (*this) << [func](TimerEvent& evt)
	{
		if (evt.m_type == TimerEvent::EventType::End) func();
	};
}
