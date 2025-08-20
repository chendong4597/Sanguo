/*!
 * \file HandlerBuilding.cpp
 *
 * \author xiaobowang;
 * \date 2017/3/17
 *
 */
#include "stdafx.h"
#include "../common/MsgMgr.h"
#include "msg/MsgBuildTower.pb.h"
#include "data/TowerBuildData.h"
#include "../GameAppExtern.h"
#include "HandlerTowerBuild.h"

using namespace NS;

static HandlerTowerBuild* g_HandlerTowerBuild = nullptr;
HandlerTowerBuild& HandlerTowerBuild::getInstance()
{
    assert(g_HandlerTowerBuild);
    return *g_HandlerTowerBuild;
}

HandlerTowerBuild::HandlerTowerBuild()
{
    assert(!g_HandlerTowerBuild);
	g_HandlerTowerBuild = this;
    reset();
}

HandlerTowerBuild::~HandlerTowerBuild()
{
	g_HandlerTowerBuild = nullptr;
}

bool HandlerTowerBuild::initialize()
{
	REGISTER_S2C(onMsgBuildTowerInfoListS2C);
	REGISTER_S2C(onMsgBuildTowerAddTowerData_S2C);
	REGISTER_S2C(onMsgBuildTowerDelTowerData_S2C);
	REGISTER_S2C(onMsgBuildTowerUpdateRes_MS2C);

    return true;
}

void HandlerTowerBuild::terminate()
{
	UNREG_S2C(MsgPB::MsgBuildTowerInfoListS2C);

    reset();
}

bool HandlerTowerBuild::reset()
{
    return true;
}

void HandlerTowerBuild::onMsgBuildTowerInfoListS2C(const MsgPB::MsgBuildTowerInfoListS2C& msg)
{
	for (auto&& data : msg.tower_data_lst())
	{
		auto&& pItem = TowerBuildMgr::getInstance().findTowerBuildByUuid(data.tower_data().uuid());
		if (pItem == nullptr)
		{
			pItem = std::make_shared<TowerBuildingData>();
			if (!pItem->initWithTemplateId(data.tower_data().templateid()))
			{
				// 创建 Item 失败。
				continue;
			}
			pItem->SetUuid(data.tower_data().uuid());
			pItem->n_PosX = data.tower_data().pos_x();
			pItem->n_PosY = data.tower_data().pos_y();
			pItem->m_Hp = data.tower_data().hp();
			TowerBuildMgr::getInstance().addItem(pItem);
		}
		else {

		}
	}
	HandlerTowerBuildEvent evt;
	evt.m_type = HandlerTowerBuildEvent::EventTowerBuild::EventTowerBuild_List;
	POST_DATA_EVENT(evt);
}

void HandlerTowerBuild::onMsgBuildTowerAddTowerData_S2C(const MsgPB::MsgBuildTowerAddTowerData_S2C& msg)
{
	auto&& pItem = TowerBuildMgr::getInstance().findTowerBuildByUuid(msg.tower_uuid());
	if (pItem == nullptr)
	{
		pItem = std::make_shared<TowerBuildingData>();
		if (!pItem->initWithTemplateId(msg.tower_data().templateid()))
		{
			// 创建 Item 失败。
			return;
		}
		pItem->SetUuid(msg.tower_data().uuid());
		pItem->n_PosX = msg.tower_data().pos_x();
		pItem->n_PosY = msg.tower_data().pos_y();
		pItem->m_Hp = msg.tower_data().hp();

		TowerBuildMgr::getInstance().addItem(pItem);
		HandlerTowerBuildEvent evt;
		evt.m_type = HandlerTowerBuildEvent::EventTowerBuild::EventTowerBuild_Add;
		evt.m_uuid = msg.tower_uuid();
		evt.m_nPosX = msg.tower_data().pos_x();
		evt.m_nPosY = msg.tower_data().pos_y();
		POST_DATA_EVENT(evt);
	}
}

void HandlerTowerBuild::onMsgBuildTowerDelTowerData_S2C(const MsgPB::MsgBuildTowerDelTowerData_S2C& msg)
{
	auto&& pItem = TowerBuildMgr::getInstance().findTowerBuildByUuid(msg.uuid());
	if (pItem != nullptr)
	{
		TowerBuildMgr::getInstance().removeItem(msg.uuid());
		HandlerTowerBuildEvent evt;
		evt.m_type = HandlerTowerBuildEvent::EventTowerBuild::EventTowerBuild_Del;
		evt.m_uuid = msg.uuid();
		POST_DATA_EVENT(evt);
	}
}

void HandlerTowerBuild::onMsgBuildTowerUpdateRes_MS2C(const MsgPB::MsgBuildTowerUpdateRes_MS2C& msg)
{
	auto&& pItem = TowerBuildMgr::getInstance().findTowerBuildByUuid(msg.tower_uuid());
	if (pItem != nullptr)
	{
		pItem->m_itemId = msg.templateid();

		HandlerTowerBuildEvent evt;
		evt.m_type = HandlerTowerBuildEvent::EventTowerBuild::EventTowerBuild_Update;
		evt.m_uuid = msg.tower_uuid();
		evt.m_nPosX = msg.pos_x();
		evt.m_nPosY = msg.pos_y();
		POST_DATA_EVENT(evt);
	}
}


bool HandlerTowerBuild::requestRemove(NS::I64 uuid)
{
	MsgPB::MsgBuildTowerDelTowerData_C2S msg;
	msg.set_uuid(uuid);
	msg.set_digid(0);
	return MsgMgr::getInstance().sendMessageDirect(MsgPB::_MSG_BUILDTOWER_DEL_TOWERDATA_C2S, &msg);
}

bool HandlerTowerBuild::requestPlace(const glm::ivec2& globalCoord, int templateId, NS::I64 uuid)
{
	MsgPB::MsgBuildTowerAddTowerData_C2S msg;
	msg.set_pos_x(globalCoord.x);
	msg.set_pos_y(globalCoord.y);
	msg.set_tower_templateid(templateId);
	msg.set_item_uuid(uuid);
	return MsgMgr::getInstance().sendMessageDirect(MsgPB::_MSG_BUILDTOWER_ADD_TOWERDATA_C2S, &msg);
}


