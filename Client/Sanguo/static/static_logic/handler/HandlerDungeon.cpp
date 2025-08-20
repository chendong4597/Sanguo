/*!
 * \file HandlerBuilding.cpp
 *
 * \author xiaobowang;
 * \date 2017/3/17
 *
 */
#include "stdafx.h"
#include "../common/MsgMgr.h"
#include "data/MapDataMgr.h"
#include "../GameAppExtern.h"
#include "HandlerDungeon.h"

using namespace NS;

static HandlerDungeon* g_HandlerDungeon = nullptr;
HandlerDungeon& HandlerDungeon::getInstance()
{
    assert(g_HandlerDungeon);
    return *g_HandlerDungeon;
}

HandlerDungeon::HandlerDungeon()
{
    assert(!g_HandlerDungeon);
	g_HandlerDungeon = this;
    reset();
}

HandlerDungeon::~HandlerDungeon()
{
	g_HandlerDungeon = nullptr;
}

bool HandlerDungeon::initialize()
{
	REGISTER_S2C(onMsgMsgDungeonInfo_MS2C);
	REGISTER_S2C(onMsgDungeonBossChest_MS2C);
	REGISTER_S2C(onMsgDungeonBossOpenChestRs_MS2C);
	REGISTER_S2C(onMsgDungeonBossDrop_MS2C);
	REGISTER_S2C(onMsgDungeonBossComing_MS2C);
	REGISTER_S2C(onMsgDungeonBossKill_MS2C);
	REGISTER_S2C(onMsgDungeonNextMob_S2C);
	REGISTER_S2C(onMsgDungeonSucExp_S2C);
    return true;
}

void HandlerDungeon::terminate()
{

    reset();
}

bool HandlerDungeon::reset()
{
    return true;
}


void HandlerDungeon::onMsgMsgDungeonInfo_MS2C(const MsgDungeonInfo_MS2C & msg)
{
	// 	LOGD("MsgDungeonInfo_MS2C");

	//m_dungeonStartTime = msg.start_time();
	//m_dungeonEndTime = msg.end_time();

	//	auto time = Heartbeat::getInstance().getServerTime();

	//postOldHandlerTeamEventByType(HandlerDungeonEvent::EventType::DungeonTimerGotton);
}


void HandlerDungeon::onMsgDungeonBossKill_MS2C(const MsgDungeonBossKill_MS2C& msg)
{
	//if (HandlerLogin::getInstance().getMapType() == MapDataMgr::MapType::Dungeon ||
	//	HandlerLogin::getInstance().getMapType() == MapDataMgr::MapType::Endless)
	//{
	//	m_bossState = 2;
	//	UiDungeonBossNews::createBossDie(msg.mob_def_id());
	//}
	//int32 id = msg.map_object_id();
	//removeBossFromBossInfo(id);
}

void HandlerDungeon::onMsgDungeonBossChest_MS2C(const MsgDungeonBossChest_MS2C& msg)
{
	POST_DATA_EVENT(msg);
}



void HandlerDungeon::onMsgDungeonBossOpenChestRs_MS2C(const MsgDungeonBossOpenChestRs_MS2C& msg)
{
	POST_DATA_EVENT(msg);
}

void HandlerDungeon::onMsgDungeonBossDrop_MS2C(const MsgDungeonBossDrop_MS2C& msg)
{
}

void HandlerDungeon::onMsgDungeonBossComing_MS2C(const MsgDungeonBossComing_MS2C& msg)
{

}

void HandlerDungeon::onMsgDungeonNextMob_S2C(const MsgPB::MsgDungeonNextMob_S2C& msg)
{
	POST_DATA_EVENT(msg);
}

void HandlerDungeon::onMsgDungeonSucExp_S2C(const MsgPB::MsgDungeonSucExp_S2C& msg)
{
	MapDataMgr::getInstance().SetDungeonExpData(msg);
}





