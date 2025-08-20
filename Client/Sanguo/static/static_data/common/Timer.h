/*!
 * \file Timer.h
 *
 * \author laizhisheng
 * \date 2016/09/07 9:29
 *
 */

#pragma once
#include "BaseTypeDefine.h"
#include <Cpp11EventSystem.h>
#include <map>
#include <unordered_set>

class TimerEvent : public bzbee::Event
{
public:
	enum class EventType
	{
		Unknown = 0,
		Begin,
		Tick,
		End,
	};

	TimerEvent(EventType type, bzbee::uint timerId, float RemainTime, float dt) : m_type(type), m_timerId(timerId), m_fRemainTime(RemainTime), m_dt(dt){};

public:
	EventType m_type{ EventType::Unknown };
	bzbee::uint m_timerId;
	float m_fRemainTime{};
	float m_dt{};
};

class Timer : public bzbee::EventHandler
{
public:
	class TimerIdHost
	{
	public:
		virtual ~TimerIdHost()
		{
			clearTimer();
		}

		void addTimerId(bzbee::uint timerId)
		{
			m_timerIdSet.insert(timerId);
		}

		void removeTimerId(bzbee::uint timerId)
		{
			m_timerIdSet.erase(timerId);
		}

		void clearTimer()
		{
			for (auto timerId : m_timerIdSet)
			{
				Timer::getInstance().removeTimer(timerId);
			}
			m_timerIdSet.clear();
		}

	private:
		std::unordered_set<bzbee::uint> m_timerIdSet;
	};

public:
	typedef std::function<void(TimerEvent&)> CallbackTimer;

	class TimerInfo
	{
	public:
		float m_fRemainTime{};
		float m_fDurationTime{};
		float m_fDetailTime{};
		CallbackTimer m_callbackTimer;
		TimerEvent::EventType m_state{ TimerEvent::EventType::Unknown };
	};

	typedef std::map<bzbee::uint, TimerInfo> TimerInfoMap;
	typedef std::vector<bzbee::uint> RemoveTimerList;
public:
	Timer();
	virtual ~Timer();

	static Timer& getInstance();
public:
	void update(float dt);

	bool hasTimer(bzbee::uint timerId);

	bzbee::uint addTimer(TimerIdHost* host, float totalTime, const CallbackTimer& callbackTimer, float durationTime = 0.0f);
	bzbee::uint updateTimer(TimerIdHost* host, bzbee::uint timerId, float totalTime, const CallbackTimer& callbackTimer, float durationTime = 0.0f);

	void removeTimer(bzbee::uint timerId);

	void setSwitchScene(bool isSwitch);
	bool isSwitchScene();

private:
	bzbee::uint updateTimerInfo(const TimerInfo& info, bzbee::uint timerId = UINT_MAX);

	TimerInfoMap m_timerInfoMap;
	RemoveTimerList m_removeTimerId;
	bool m_isSwitchScene{ false };
};

#define DECLARE_TIMER_HOST() protected: Timer::TimerIdHost __timer_host__

#define REGISTER_TIMER(timerId, totalTime, durationTime, callback, ...)\
{ if (timerId == 0) timerId = Timer::getInstance().addTimer(&__timer_host__, totalTime, std::bind(callback, this, std::placeholders::_1, ##__VA_ARGS__), durationTime);\
else Timer::getInstance().updateTimer(&__timer_host__, timerId, totalTime, std::bind(callback, this, std::placeholders::_1, ##__VA_ARGS__), durationTime); }

#define UNREGISTER_TIMER(timerId)\
{ __timer_host__.removeTimerId(timerId);\
Timer::getInstance().removeTimer(timerId);\
timerId = 0; }

#define UNREGISTER_ALL_TIMER()\
{ __timer_host__.clearTimer(); }

//////////////////////////////////////////////////////////////////////////


class RegisterTimer
{
public:
	RegisterTimer(Timer::TimerIdHost* pTimerHost, float totalTime, float tick);

	typedef std::function<void(TimerEvent&)> OnTimer;
	unsigned int operator<< (const OnTimer& func);

	typedef std::function<void(void)> OnDelay;
	unsigned int operator>> (const OnDelay& func);

private:
	Timer::TimerIdHost* m_pTimerHost;
	float m_totalTime;
	float m_tick;
};


#define REG_TIMER_TICK(totalTime, tick) RegisterTimer(&__timer_host__, totalTime, tick) <<
#define REG_TIMER()                     RegisterTimer(&__timer_host__, FLT_MAX  , 0.0f) <<
#define DELAY(totalTime)                RegisterTimer(&__timer_host__, totalTime, 0.0f) >>
