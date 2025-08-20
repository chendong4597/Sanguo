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
#include "data/role/Player.h"
#include "HandlerInteraction.h"

using namespace NS;

static HandlerInteraction* g_HandlerInteraction = nullptr;
HandlerInteraction& HandlerInteraction::getInstance()
{
    assert(g_HandlerInteraction);
    return *g_HandlerInteraction;
}

HandlerInteraction::HandlerInteraction()
{
    assert(!g_HandlerInteraction);
	g_HandlerInteraction = this;
    reset();
}

HandlerInteraction::~HandlerInteraction()
{
	g_HandlerInteraction = nullptr;
}

bool HandlerInteraction::initialize()
{
	REGISTER_S2C(onMsgInteractionInWorldS2C);
	REGISTER_S2C(onMsgInteractionWalkS2C);
	REGISTER_S2C(onMsgInteractionS2C);
	REGISTER_S2C(onMsgMagicEffectAppenceS2C);
	REGISTER_S2C(onMsgInteractionTurnS2C);
    return true;
}

void HandlerInteraction::terminate()
{
    reset();
}

bool HandlerInteraction::reset()
{
    return true;
}

void HandlerInteraction::onMsgInteractionInWorldS2C(const MsgPB::MsgInteractionInWorldS2C& msg)
{
	auto player = PlayerMgr::getInstance().getPlayer(msg.objid());
	if (!player) {
		player = std::make_shared<Player>(msg.objid());
		player->setIggId(msg.target_iggid());
		PlayerMgr::getInstance().addPlayer(player);
	}
	POST_DATA_EVENT(msg);
	return;
}

void HandlerInteraction::onMsgInteractionWalkS2C(const MsgPB::MsgInteractionWalkS2C& msg)
{
	POST_DATA_EVENT(msg);
}

void HandlerInteraction::onMsgInteractionS2C(const MsgPB::MsgInteractionS2C& msg)
{
	POST_DATA_EVENT(msg);
}

void HandlerInteraction::onMsgMagicEffectAppenceS2C(const MsgPB::MsgMagicEffectAppenceS2C& msg)
{
	POST_DATA_EVENT(msg);
}

void HandlerInteraction::onMsgInteractionTurnS2C(const MsgPB::MsgInteractionTurnS2C& msg)
{
	POST_DATA_EVENT(msg);
}


