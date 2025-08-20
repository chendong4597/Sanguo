/*!
 * \file HandlerCraft.cpp
 *
 * \author xiaobowang;
 * \date 3/1/2017
 *
 */
#include "stdafx.h"
#include "HandlerCraft.h"
#include "data/CraftData.h"
#include "data/ItemData.h"
#include "data/NotesData.h"
#include "common/Heartbeat.h"
#include "common/Heartbeat.h"
//#include "ui/UiMgr.h"
//#include "ui/craft/UiCraftReceiveItems.h"
//#include "ui/common/UiMsgBoxToast.h"
#include <data/ItemData.h>
#include <msg/MsgType.pb.h>
#include "msg/MsgCraft.pb.h"
#include <set>
#include <bitset>
//#include "map/building/BuildingMgr.h"
//#include "data/StaticObj.h"
//#include "GameObject.h"
//#include "map/building/BuildingUpgradeEffectBehavior.h"
#include "data/NpcProtectorData.h"
#include "HandlerLogin.h"
//#include "HandlerAppsFlyer.h"
//#include "common/TaskEventComponent.h"
//#include "ui/lobby/UiLobby.h"

using namespace NS;

static HandlerCraft* g_HandlerCraft = nullptr;
HandlerCraft& HandlerCraft::getInstance()
{
    assert(g_HandlerCraft);
    return *g_HandlerCraft;
}

HandlerCraft::HandlerCraft()
{
    assert(!g_HandlerCraft);
    g_HandlerCraft = this;
}

HandlerCraft::~HandlerCraft()
{
    g_HandlerCraft = nullptr;
}

bool HandlerCraft::initialize()
{
	REGISTER_S2C(onMsgCraft);
	REGISTER_S2C(onMsgCraftToolNotify);
	REGISTER_S2C(onMsgCraftGet);
	REGISTER_S2C(onMsgCraftCancel);
	REGISTER_S2C(onMsgCraftToolUpgrade);
	REGISTER_S2C(onMsgCraftComplete);
	REGISTER_S2C(onMsgCraftToolUpdateG2C);
	REGISTER_S2C(onMsgCraftAllUnlockNotify);
	REGISTER_S2C(onMsgCraftUnlockNotify);
	REGISTER_S2C(onMsgCraftToolUnitUnlockNotify);
	
	return true;
}

void HandlerCraft::terminate()
{
	UNREG_S2C(MsgCraftG2C);
	UNREG_S2C(MsgCraftToolNotifyG2C);
	UNREG_S2C(MsgCraftGetG2C);
	UNREG_S2C(MsgCraftCancelG2C);
	UNREG_S2C(MsgCraftToolUpgradeG2C);
	UNREG_S2C(MsgCraftCompleteG2C);
	UNREG_S2C(MsgCraftAllUnlockNotifyG2C);
	UNREG_S2C(MsgCraftUnlockNotifyG2C);

	this->reset();
}

bool HandlerCraft::reset()
{
	return true;
}

///////////response////////////

void HandlerCraft::onMsgCraftToolNotify(const MsgPB::MsgCraftToolNotifyG2C& msg)
{
	CraftDataMgr::getInstance().clearCraftTaskData();
	for (int i = 0 ; i < msg.crafttools_size(); i++)
	{
		std::shared_ptr<CraftTaskData> data = std::make_shared<CraftTaskData>();
		data->m_craftId = msg.crafttools(i).patternid();
		data->m_protectorUUID = msg.crafttools(i).protectoruuid();
		data->m_craftNum = msg.crafttools(i).completenum();
		data->m_finishTime = msg.crafttools(i).completetime();
		data->m_canelFinishNum = msg.crafttools(i).cancelnum();
		CraftDataMgr::getInstance().addCraftTaskData(data);
	}
	HandlerCraftEvent evt;
	evt.m_type = HandlerCraftEvent::EventType::Update;
	POST_DATA_EVENT(evt);
}

void HandlerCraft::onMsgCraft(const MsgPB::MsgCraftG2C& msg)
{
	UNREGISTER_TIMER(m_reqCraftLockTimer);

	if (msg.result() == MsgCraftG2C_RESULT_SUCCESS) {
		HandlerCraftEvent evt;
		evt.m_type = HandlerCraftEvent::EventType::Craft_Suc;
		POST_DATA_EVENT(evt);
	}
}

void HandlerCraft::onMsgCraftGet(const MsgPB::MsgCraftGetG2C& msg)
{
	UNREGISTER_TIMER(m_reqCraftGetLockTimer);

	if (msg.result() == MsgCraftG2C_RESULT_SUCCESS) {
		auto&& data = CraftDataMgr::getInstance().findCraftTaskData(msg.protectoruuid());
		if (data) {
			NotesDataMgr::getInstance().addItem(NOTES_DATA_TPY_CRAFT_GET, msg.protectoruuid(), 0, data->m_craftId, 0);
		}
		
		CraftDataMgr::getInstance().removeCraftTaskData(msg.protectoruuid());
		HandlerCraftEvent evt;
		evt.m_type = HandlerCraftEvent::EventType::Get_Suc;
		evt.m_nProtectorUUID = msg.protectoruuid();
		POST_DATA_EVENT(evt);
	}
}

void HandlerCraft::onMsgCraftCancel(const MsgPB::MsgCraftCancelG2C& msg)
{
	UNREGISTER_TIMER(m_reqCraftCancelLockTimer);
}

void HandlerCraft::onMsgCraftToolUpdateG2C(const MsgPB::MsgCraftToolUpdateG2C& msg)
{
	if (msg.crafttool().protectoruuid() == 0) {
		return;
	}
	std::shared_ptr<CraftTaskData> data = std::make_shared<CraftTaskData>();
	data->m_craftId = msg.crafttool().patternid();
	data->m_protectorUUID = msg.crafttool().protectoruuid();
	data->m_craftNum = msg.crafttool().completenum();
	data->m_finishTime = msg.crafttool().completetime();
	data->m_canelFinishNum = msg.crafttool().cancelnum();
	CraftDataMgr::getInstance().updateCraftTaskData(data);
	HandlerCraftEvent evt;
	evt.m_type = HandlerCraftEvent::EventType::Update;
	evt.m_nProtectorUUID = data->m_protectorUUID;
	POST_DATA_EVENT(evt);
	if (data->m_finishTime > Heartbeat::getInstance().getServerTime() + 2)
	{
		NotesDataMgr::getInstance().addItem(NOTES_DATA_TPY_CRAFT_BEGIN, data->m_protectorUUID, 0, data->m_craftId, 0);
	}
	else {
		NotesDataMgr::getInstance().addItem(NOTES_DATA_TPY_CRAFT_END, data->m_protectorUUID, 0, data->m_craftId, 0);
	}
}


void HandlerCraft::onMsgCraftToolUpgrade(const MsgPB::MsgCraftToolUpgradeG2C& msg)
{
}

void HandlerCraft::onMsgCraftToolRemove(const MsgPB::MsgCraftToolRemoveG2C& msg)
{
	
}

//Immediate Complete
void HandlerCraft::onMsgCraftComplete(const MsgPB::MsgCraftCompleteG2C& msg)
{
	UNREGISTER_TIMER(m_reqCraftCompleteLockTimer);
}

void HandlerCraft::onMsgCraftAllUnlockNotify(const MsgPB::MsgCraftAllUnlockNotifyG2C& msg)
{
}

void HandlerCraft::onMsgCraftUnlockNotify(const MsgPB::MsgCraftUnlockNotifyG2C& msg)
{
}

void HandlerCraft::onMsgCraftToolUnitUnlockNotify(const MsgPB::MsgCraftToolUnitUnlockNotifyS2C& msg)
{
}

///////////send////////////

bool HandlerCraft::requestCraft(const std::shared_ptr<CraftData>& pCraftData, NS::I64 protectorUUID, int craftNum)
{
	return false;
}

bool HandlerCraft::requestCraftGet(NS::I64 protectorUUID)
{
	if (!HandlerLogin::getInstance().isConnect())
	{
		//UiMsgBoxToast::getInstance().showText(_LC("CLI_Common-network is diconnect."));
		return false;
	}

	if (m_reqCraftGetLockTimer != 0)
	{
		return true;
	}

	MsgPB::MsgCraftGetC2G msg;
	msg.set_protectoruuid(protectorUUID);

	m_reqCraftGetLockTimer = DELAY(10.0f)[this]() 
	{
		UNREGISTER_TIMER(m_reqCraftGetLockTimer);
	};
	return MsgMgr::getInstance().sendMessage(MsgPB::_MSG_CRAFT_GET_C2G, &msg);
}

bool HandlerCraft::requestCraftCancel(NS::I64 protectorUUID)
{
	if (!HandlerLogin::getInstance().isConnect())
	{
		//UiMsgBoxToast::getInstance().showText(_LC("CLI_Common-network is diconnect."));
		return false;
	}

	if (m_reqCraftCancelLockTimer != 0)
	{
		return true;
	}

	MsgPB::MsgCraftGetC2G msg;
	msg.set_protectoruuid(protectorUUID);

	m_reqCraftCancelLockTimer = DELAY(10.0f)[this]() 
	{
		UNREGISTER_TIMER(m_reqCraftCancelLockTimer);
	};
	return MsgMgr::getInstance().sendMessage(MsgPB::_MSG_CRAFT_CANCEL_C2G, &msg);
}

bool HandlerCraft::requestCraftComplete(NS::I64 protectorUUID)
{
	if (!HandlerLogin::getInstance().isConnect())
	{
		//UiMsgBoxToast::getInstance().showText(_LC("CLI_Common-network is diconnect."));
		return false;
	}

	if (m_reqCraftCompleteLockTimer != 0)
	{
		return true;
	}

	MsgPB::MsgCraftCompleteC2G msg;
	msg.set_protectoruuid(protectorUUID);

	m_reqCraftCompleteLockTimer = DELAY(10.0f)[this]() 
	{
		UNREGISTER_TIMER(m_reqCraftCompleteLockTimer);
	};
	return MsgMgr::getInstance().sendMessage(MsgPB::_MSG_CRAFT_COMPLETE_C2G, &msg);
}
