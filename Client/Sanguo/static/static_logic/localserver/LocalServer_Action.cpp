/*!
* \file LocalServer_Action.cpp
*
* \author zhichenxia
* \date 2018/6/13
*
*
*/
#include "stdafx.h"
#include "LocalServer_Action.h"
#include <data/role/Player.h>
#include <config/BuffCfg.h>
#include "config/BuffGroupRuleCfg.h"
#include "../handler/HandlerRole.h"
#include "data/role/Monsters.h"
#include "config/GlobalCfg.h"
#include <common/Heartbeat.h>
//#include <GameObject.h>
#include "SkillBuffStrategy.h"
#include "../common/SyncCache.h"
#include "common/ClientSpec.h"
#include "common/ComSkillBuff.h"
#include "common/SkillEffect.h"
#include "../handler/HandlerMsg.h"

using namespace MsgPB;

NS_BEGIN


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LocalServer_Action

LocalServer_Action& LocalServer_Action::getInstance(void)
{
	static LocalServer_Action instance;
	return instance;
}


LocalServer_Action::LocalServer_Action()
{
	HOOK_MSG(onMsgInteractionWalkC2S);
	HOOK_MSG(onMsgInteractionC2S);
	HOOK_MSG(onMsgInteractionTurnC2S);
}

void LocalServer_Action::onMsgInteractionWalkC2S(const MsgInteractionWalkC2S& msg)
{
	MsgInteractionWalkS2C retMsg;
	retMsg.set_objid(msg.objid());
	retMsg.set_player_iggid(msg.player_iggid());
	auto& pos = *retMsg.mutable_p();
	pos.set_x(msg.p().x());
	pos.set_y(msg.p().y());
	pos.set_z(msg.p().z());
	auto& Rot = *retMsg.mutable_r();
	Rot.set_r(msg.r().r());
	Rot.set_p(msg.r().p());
	Rot.set_y(msg.r().y());
	send(retMsg);
}

void LocalServer_Action::onMsgInteractionC2S(const MsgPB::MsgInteractionC2S& msg)
{
	MsgInteractionS2C retMsg;
	retMsg.set_objid(msg.objid());
	retMsg.set_player_iggid(msg.player_iggid());
	retMsg.set_is_from_server(false);
	retMsg.set_action_type(msg.action_type());
	retMsg.set_data1(msg.data1());
	retMsg.set_data2(msg.data2());
	retMsg.set_data3(msg.data3());
	send(retMsg);
}

void LocalServer_Action::onMsgInteractionTurnC2S(const MsgPB::MsgInteractionTurnC2S& msg)
{
	MsgInteractionTurnS2C retMsg;
	retMsg.set_objid(msg.objid());
	retMsg.set_player_iggid(msg.player_iggid());
	auto& Rot = *retMsg.mutable_r();
	Rot.set_r(msg.r().r());
	Rot.set_p(msg.r().p());
	Rot.set_y(msg.r().y());
	send(retMsg);
}

NS_END


//void LUA(add_buff)(int32 buffId, int32 target_id)
//{
//	if (target_id <= 0)
//	{
//		auto pSelf = PlayerMgr::getInstance().getSelf();
//		if (!pSelf) return;
//		target_id = pSelf->getId();
//	}
//
//	NS::LocalServer_Action::getInstance().addBuff(buffId, target_id, target_id);
//}
//
//
//void LUA(add_buff_ex)(int32 buffId, int32 target_id, int32 caster_id)
//{
//	if (target_id <= 0)
//	{
//		auto pSelf = PlayerMgr::getInstance().getSelf();
//		if (!pSelf) return;
//		target_id = pSelf->getId();
//	}
//
//	NS::LocalServer_Action::getInstance().addBuff(buffId, target_id, caster_id);
//}
//
//void LUA(remove_buff)(int32 buffId, int32 target_id)
//{
//	if (target_id <= 0)
//	{
//		auto pSelf = PlayerMgr::getInstance().getSelf();
//		if (!pSelf) return;
//		target_id = pSelf->getId();
//	}
//	NS::LocalServer_Action::getInstance().removeBuff(buffId, target_id);
//}
