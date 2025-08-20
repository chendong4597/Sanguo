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
#include "data/HouseData.h"
#include "data/NotesData.h"
#include "HandlerHouse.h"

using namespace NS;

static HandlerHouse* g_HandlerHouse = nullptr;
HandlerHouse& HandlerHouse::getInstance()
{
    assert(g_HandlerHouse);
    return *g_HandlerHouse;
}

HandlerHouse::HandlerHouse()
{
    assert(!g_HandlerHouse);
	g_HandlerHouse = this;
    reset();
}

HandlerHouse::~HandlerHouse()
{
	g_HandlerHouse = nullptr;
}

bool HandlerHouse::initialize()
{
	REGISTER_S2C(onMsgHouseSyncS2C);
	REGISTER_S2C(onMsgHouseUpdateS2C);
	REGISTER_S2C(onMsgHouseUpgradeS2C);
    return true;
}

void HandlerHouse::terminate()
{
    reset();
}

bool HandlerHouse::reset()
{
    return true;
}



bool HandlerHouse::requestUpgrade(int nHouseId, int nHouseTpy)
{
	MsgPB::MsgHouseUpgradeC2S msg;
	msg.set_houseid(nHouseId);
	msg.set_next_house_tpy(nHouseTpy);
	return MsgMgr::getInstance().sendMessageDirect(msg.msgtype(), &msg);
}

bool HandlerHouse::requestHome(NS::int32 nHouseId, NS::I64 uuid, bool bCheckin)
{
	MsgPB::MsgHouseHomeC2S msg;
	msg.set_houseid(nHouseId);
	msg.set_protectorid(uuid);
	msg.set_bcheckin(bCheckin);
	return MsgMgr::getInstance().sendMessageDirect(msg.msgtype(), &msg);
}

void HandlerHouse::onMsgHouseSyncS2C(const MsgPB::MsgHouseSyncS2C& msg)
{
	HouseDataMgr::getInstance().Clear();
	for (auto&& data : msg.data())
	{
		auto pItem = std::make_shared<HouseData>();
		pItem->m_houseId = data.houseid();
		pItem->m_houseType = data.housetype();
		pItem->m_nextHouseTpy = data.nexthouse();
		pItem->m_completeTm = data.completetm();
		HouseDataMgr::getInstance().addHouse(pItem);
	}
	HandlerHouseEvent evt;
	evt.m_nHouseId = msg.data_size() > 0 ? msg.data(0).houseid() : 0;
	evt.m_type = HandlerHouseEvent::EventType::SynHouse;
	POST_DATA_EVENT(evt);
}

void HandlerHouse::onMsgHouseUpdateS2C(const MsgPB::MsgHouseUpdateS2C& msg)
{
	auto&& pItem = HouseDataMgr::getInstance().findByHouseId(msg.data().houseid());
	if (pItem == nullptr)
	{
		pItem = std::make_shared<HouseData>();
		pItem->m_houseId = msg.data().houseid();
		pItem->m_houseType = msg.data().housetype();
		pItem->m_nextHouseTpy = msg.data().nexthouse();
		pItem->m_completeTm = msg.data().completetm();
		HouseDataMgr::getInstance().addHouse(pItem);
	}
	else {
		int nPreHouseTpy = pItem->m_houseType;
		int nPreNxtHouse = pItem->m_nextHouseTpy;
		pItem->m_houseType = msg.data().housetype();
		pItem->m_nextHouseTpy = msg.data().nexthouse();
		pItem->m_completeTm = msg.data().completetm();
		if (nPreHouseTpy != msg.data().housetype()) {
			NotesDataMgr::getInstance().addItem(NOTES_DATA_TPY_HOUSE_LV_END, 0, 0, pItem->m_houseId, msg.data().housetype());
		}

		if (nPreNxtHouse == 0 && pItem->m_nextHouseTpy != 0)
		{
			NotesDataMgr::getInstance().addItem(NOTES_DATA_TPY_HOUSE_LV, 0, 0, pItem->m_houseId, msg.data().nexthouse());
		}
	}
	HandlerHouseEvent evt;
	evt.m_nHouseId = msg.data().houseid();
	evt.m_type = HandlerHouseEvent::EventType::UpdateHouse;
	POST_DATA_EVENT(evt);
}

void HandlerHouse::onMsgHouseUpgradeS2C(const MsgPB::MsgHouseUpgradeS2C& msg)
{
	//if (msg.res() == MsgPB::PLANT_RESULT::PLANT_RESULT_OK) {
	//	NS::I64 uuid = PlantDataMgr::getInstance().RemovePlantXY(msg.pos_x(), msg.pos_y());
	//	if (uuid != 0)
	//	{
	//		HandlerPlantEvent evt;
	//		evt.m_uuid = uuid;
	//		evt.m_type = HandlerPlantEvent::EventPlant::EventPlant_Del;
	//		POST_DATA_EVENT(evt);
	//	}
	//}
}


