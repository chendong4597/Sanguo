/*!
 * \file Framework.cpp
 * \date 10-10-2014 11:44:39
 * 
 * 
 * \author zjhlogo (zjhlogo@gmail.com)
 */
#include "Framework.h"
#include <chrono>
#include "debugs/Debug.h"
#include <platform/Platform.h>
#include "id_jobsystem/ParallelJobList.h"
#include <TimeUtil.h>
#include <thread>
#include <Name.h>
#include <GarbageCollection.h>

NS_BEGIN

static Framework* g_pFramework = nullptr;

Framework::Framework():m_bLimitFPS(false), m_nLimitFPSNum(60), m_fLimitFPSTick(16)
{
	m_currTime = TimeUtil::cacheTimeStamp();
	g_pFramework = this;
}

Framework::~Framework()
{
	GarbageCollection::getInstance().terminate();
}

Framework& Framework::getInstance()
{
	return *g_pFramework;
}

extern uint32 PRIVATE_gameThreadId;

bool Framework::initialize(Component* pEntity)
{
	Name::staticInit();
    // init the parallel job system
	parallelJobManager->Init();
	

// 	LOGI("Finish initialized framework.");
	return true;
}

void Framework::terminate()
{
	Name::staticExit();
}

void Framework::step()
{
	static int s_index = 0;
	static float s_deltaTimes[10] = { 0.0166f, 0.0166f, 0.0166f, 0.0166f, 0.0166f, 0.0166f, 0.0166f, 0.0166f, 0.0166f, 0.0166f };

	I64 frameStopCycles;
	static I64 syncEndCycles = 0;

	{
		GarbageCollection::getInstance().run();

		BeginFrameEvent evt;
		postEvent(evt);
	}

    {
		m_lastTime = m_currTime;
		m_currTime = TimeUtil::cacheTimeStamp();

		const float frameTime = m_currTime - m_lastTime;
		// Tick frame
		{
			auto dt = std::min(frameTime, 0.25f);

			s_deltaTimes[s_index] = dt;
			s_index = (s_index + 1) % 10;

			m_deltaTime =
				( s_deltaTimes[0]
				+ s_deltaTimes[1]
				+ s_deltaTimes[2]
				+ s_deltaTimes[3]
				+ s_deltaTimes[4]
				+ s_deltaTimes[5]
				+ s_deltaTimes[6]
				+ s_deltaTimes[7]
				+ s_deltaTimes[8]
				+ s_deltaTimes[9]) * 0.1f * Debug::getTimeScale();


			{
				if (Debug::playControl())
				{
					update(m_deltaTime);
				}
			}

			FrameStepEvent evt(m_deltaTime);
			evt.m_frameTime = dt;
			postEvent(evt);
		}

        //// End frame
		EndFrameEvent evt;
		postEvent(evt);

		frameStopCycles = std::chrono::duration_cast<std::chrono::duration< I64, std::micro>>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	} // update end

    ++m_frameNumber;

#if defined(__ANDROID__) || defined (WIN32)
	// start sync to limited FPS
	if (m_bLimitFPS)
	{
		I64 nextFrameCycles = syncEndCycles + (I64)(this->GetLimitFPSTick() * 1000);
		if (frameStopCycles < nextFrameCycles)
		{
			I64 deltaCycles = nextFrameCycles - frameStopCycles;
			std::this_thread::sleep_for(std::chrono::microseconds(deltaCycles));
		}
	}
#endif

	syncEndCycles = std::chrono::duration_cast<std::chrono::duration< I64, std::micro>>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

void Framework::resize(const glm::ivec2& deviceSize)
{
	ResizeEvent evt(deviceSize);
	postEvent(evt);
}

void Framework::setSafeArea(const int top, const int left, const int bottom, const int right)
{
    SetSafeAreaEvent evt(top, left, bottom, right);
    postEvent(evt);
}

void Framework::reload(bool freeOld)
{
	ReloadEvent evt(freeOld);
	postEvent(evt);
}

void TouchEvent::addPoint(int pointId, ActionTypeId actionType, const glm::vec2& pos)
{
	TouchPointer point;
	point.m_pointId = pointId;
	point.m_actionType = actionType;
	point.m_touchPos = pos;
	m_touchPointerList.push_back(point);
}

void TouchEvent::removePoint(int pointId)
{
	for (auto it = m_touchPointerList.begin(); it != m_touchPointerList.end(); ++it)
	{
		if (it->m_pointId == pointId)
		{
			m_touchPointerList.erase(it);
			break;
		}
	}
}

NS_END

NS::Framework* LUA(get_framework)(void)
{
	return &NS::Framework::getInstance();
}
