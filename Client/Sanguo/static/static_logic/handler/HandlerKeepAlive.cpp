/*!
 * \file HandlerKeepAlive.cpp
 *
 * \author zhijieli
 * \date 2016/12/20
 *
 *
 */
#include "HandlerKeepAlive.h"
#include "stdafx.h"
#include <msg/MsgType.pb.h>
#include <msg/MsgPing.pb.h>
#include "../common/MsgMgr.h"
#include <MsgHandler.h>
#include <common/Heartbeat.h>
#include "TimeUtil.h"
#include <chrono>
#include "HandlerLogin.h"


using namespace NS;
using namespace MsgPB;

static HandlerKeepAlive* g_HandlerKeepAlive = nullptr;



HandlerKeepAlive& HandlerKeepAlive::getInstance()
{
    assert(g_HandlerKeepAlive);
    return *g_HandlerKeepAlive;
}

HandlerKeepAlive::HandlerKeepAlive()
{
    assert(!g_HandlerKeepAlive);
    g_HandlerKeepAlive = this;
}

HandlerKeepAlive::~HandlerKeepAlive()
{
    g_HandlerKeepAlive = nullptr;
}

bool HandlerKeepAlive::initialize()
{
	REGISTER_S2C(onMsgPong);

	return true;
}

void HandlerKeepAlive::onMsgPong(const MsgPB::MsgPong& msg)
{
	//LOGI("HandlerKeepAlive::onMsgPong time = {0}", msg.servertime());
	m_serverTime = msg.servertime();

	HandlerKeepAliveEvent evt;
	evt.m_type = HandlerKeepAliveEvent::EventType::Time_Beat;
	evt.m_timeDelay = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()
	-msg.clienttimeinms();
	this->postEvent(evt);
	Heartbeat::getInstance().setServerTime(msg.servertime(), msg.clienttimeinms());
	Heartbeat::getInstance().setServerTimeInMs(msg.servertimems());
}


