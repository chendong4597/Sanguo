/*!
 * \file HandlerBuilding.cpp
 *
 * \author xiaobowang;
 * \date 2017/3/17
 *
 */
#include "stdafx.h"
#include "../common/MsgMgr.h"
#include "../GameAppExtern.h"
#include "HandlerSurvival.h"

using namespace NS;

static HandlerSurvival* g_HandlerSurvival = nullptr;
HandlerSurvival& HandlerSurvival::getInstance()
{
    assert(g_HandlerSurvival);
    return *g_HandlerSurvival;
}

HandlerSurvival::HandlerSurvival()
{
    assert(!g_HandlerSurvival);
	g_HandlerSurvival = this;
    reset();
}

HandlerSurvival::~HandlerSurvival()
{
	g_HandlerSurvival = nullptr;
}

bool HandlerSurvival::initialize()
{
	REGISTER_S2C(onMsgSurvivalNotifyEventS2C);
	REGISTER_S2C(onMsgSurvivalNotifyWaveS2C);
	REGISTER_S2C(onMsgSurvivalNotifyTargetInfoS2C);
	REGISTER_S2C(onMsgSurvivalNotifyMobBirthPointS2C);
	REGISTER_S2C(onMsgSurvivalNotifyClearMobS2C);
	REGISTER_S2C(onMsgSurvivalNotifyGameRewardS2C);
    return true;
}

void HandlerSurvival::terminate()
{

    reset();
}

bool HandlerSurvival::reset()
{
    return true;
}


void HandlerSurvival::onMsgSurvivalNotifyEventS2C(const MsgPB::MsgSurvivalNotifyEventS2C& msg)
{
	int a = 0;
	a++;
}

void HandlerSurvival::onMsgSurvivalNotifyWaveS2C(const MsgPB::MsgSurvivalNotifyWaveS2C& msg)
{
	int a = 0;
	a++;
}

void HandlerSurvival::onMsgSurvivalNotifyTargetInfoS2C(const MsgPB::MsgSurvivalNotifyTargetInfoS2C& msg)
{
	int a = 0;
	a++;
}

void HandlerSurvival::onMsgSurvivalNotifyMobBirthPointS2C(const MsgPB::MsgSurvivalNotifyMobBirthPointS2C& msg)
{
	int a = 0;
	a++;
}

void HandlerSurvival::onMsgSurvivalNotifyClearMobS2C(const MsgPB::MsgSurvivalNotifyClearMobS2C& msg)
{
	int a = 0;
	a++;
}

void HandlerSurvival::onMsgSurvivalNotifyGameRewardS2C(const MsgPB::MsgSurvivalNotifyGameRewardS2C& msg)
{
	int a = 0;
	a++;
}






