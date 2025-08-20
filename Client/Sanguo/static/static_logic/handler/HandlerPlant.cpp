/*!
 * \file HandlerBuilding.cpp
 *
 * \author xiaobowang;
 * \date 2017/3/17
 *
 */
#include "stdafx.h"
#include "../common/MsgMgr.h"
#include "msg/MsgPlant.pb.h"
#include "../GameAppExtern.h"
#include "data/PlantData.h"
#include "data/NotesData.h"
#include "HandlerPlant.h"

using namespace NS;

static HandlerPlant* g_HandlerPlant = nullptr;
HandlerPlant& HandlerPlant::getInstance()
{
    assert(g_HandlerPlant);
    return *g_HandlerPlant;
}

HandlerPlant::HandlerPlant()
{
    assert(!g_HandlerPlant);
	g_HandlerPlant = this;
    reset();
}

HandlerPlant::~HandlerPlant()
{
	g_HandlerPlant = nullptr;
}

bool HandlerPlant::initialize()
{
	REGISTER_S2C(onMsgPlantInfo);
	REGISTER_S2C(onMsgPlantSet);
	REGISTER_S2C(onMsgPlantCut);
    return true;
}

void HandlerPlant::terminate()
{

    reset();
}

bool HandlerPlant::reset()
{
    return true;
}



bool HandlerPlant::requestCut(NS::I64 uuid)
{
	auto&& pItem = PlantDataMgr::getInstance().findPlantByUuid(uuid);
	if (pItem == nullptr)
	{
		return false;
	}
	MsgPB::MsgPlantCutReq msg;
	msg.set_pos_x(pItem->n_PosX);
	msg.set_pos_y(pItem->n_PosY);
	return MsgMgr::getInstance().sendMessageDirect(MsgPB::_MSG_PLANT_CUT_REQ, &msg);
}

bool HandlerPlant::requestPlant(const glm::ivec2& globalCoord, int templateId, NS::I64 uuid)
{
	MsgPB::MsgPlantSetReq msg;
	msg.set_pos_x(globalCoord.x);
	msg.set_pos_y(globalCoord.y);
	msg.set_plant_seed_type(templateId);
	return MsgMgr::getInstance().sendMessageDirect(msg.msgtype(), &msg);
}

bool HandlerPlant::requestMangerPlant(int X, int Y, NS::I64 protector_id)
{
	MsgPB::MsgPlantNpcManger_C2S msg;
	msg.set_pos_x(X);
	msg.set_pos_y(Y);
	msg.set_protector_id(protector_id);
	return MsgMgr::getInstance().sendMessageDirect(msg.msgtype(), &msg);
}

void HandlerPlant::onMsgPlantInfo(const MsgPB::MsgPlantInfo& msg)
{
	static bool s_onmsg_old = false;
	std::vector<NS::I64> vecInfo;
	for (auto&& data : msg.plant_info())
	{	
		if (data.plant_id() == 0)
		{
			continue;
		}
		bool bUpdate = true;
		int nPreStage = 0;
		NS::I64 nPreProtectUuid = 0;
		auto&& pItem = PlantDataMgr::getInstance().findPlantByUuid(data.plant_id());
		if (pItem == nullptr)
		{
			pItem = std::make_shared<PlantData>();
			if (!pItem->initWithPlantTypeId(data.plant_type()))
			{
				// 创建 Item 失败。
				continue;
			}
			pItem->SetUuid(data.plant_id());
			PlantDataMgr::getInstance().addItem(pItem);
			vecInfo.push_back(data.plant_id());
			bUpdate = false;
		}
		else {
			nPreStage = pItem->m_growStage;
			nPreProtectUuid = pItem->m_protectorId;
		}
		pItem->n_PosX = data.pos_x();
		pItem->n_PosY = data.pos_y();
		pItem->m_beginTm = data.grow_begin();
		pItem->m_growTm = data.grow_time();
		pItem->m_growStage = data.grow_stage();
		pItem->m_protectorId = data.protectorid();

		if (bUpdate) {
			HandlerPlantEvent evt;
			evt.m_type = HandlerPlantEvent::EventPlant::EventPlant_Update;
			evt.m_uuid = data.plant_id();
			evt.m_blockId = data.pos_x() + data.pos_y()*PLANT_ROW_NUM;
			POST_DATA_EVENT(evt);

			if (nPreStage > data.grow_stage())
			{
				NotesDataMgr::getInstance().addItem(NOTES_DATA_TPY_PLANT_GET, data.plant_id(), data.protectorid());
			}

			if (nPreProtectUuid == 0 && data.protectorid() != 0)
			{
				NotesDataMgr::getInstance().addItem(NOTES_DATA_TPY_PLANT_MANA, data.plant_id(), data.protectorid());
			}
		}
	}
	
	if (!s_onmsg_old) {
		HandlerPlantEvent evt;
		evt.m_type = HandlerPlantEvent::EventPlant::EventPlant_List;
		POST_DATA_EVENT(evt);
		s_onmsg_old = true;
	} else {
		HandlerPlantEvent evt;
		evt.m_type = HandlerPlantEvent::EventPlant::EventPlant_Add;
		evt.m_vecUuid = vecInfo;
		POST_DATA_EVENT(evt);
	}
}

void HandlerPlant::onMsgPlantSet(const MsgPB::MsgPlantSet& msg)
{
	auto&& pItem = PlantDataMgr::getInstance().findPlantByUuid(msg.plant_id());
	if (pItem == nullptr)
	{
		pItem = std::make_shared<PlantData>();
		if (!pItem->initWithPlantTypeId(msg.templateid()))
		{
			// 创建 Item 失败。
			return;
		}
		pItem->SetUuid(msg.plant_id());
		PlantDataMgr::getInstance().addItem(pItem);
	}
	pItem->n_PosX = msg.pos_x();
	pItem->n_PosY = msg.pos_y();
	pItem->m_beginTm = msg.grow_begin();
	pItem->m_growTm = msg.grow_time();
	pItem->m_growStage = msg.grow_stage();
	pItem->m_protectorId = msg.protectorid();
	HandlerPlantEvent evt;
	evt.m_vecUuid.push_back(msg.plant_id());
	evt.m_type = HandlerPlantEvent::EventPlant::EventPlant_Add;
	POST_DATA_EVENT(evt);

	NotesDataMgr::getInstance().addItem(NOTES_DATA_TPY_PLANT_IN, msg.plant_id());
}

void HandlerPlant::onMsgPlantCut(const MsgPB::MsgPlantCut& msg)
{
	if (msg.res() == MsgPB::PLANT_RESULT::PLANT_RESULT_OK) {
		int nBlockId = msg.pos_x() + msg.pos_y() * PLANT_ROW_NUM;
		auto&& data = PlantDataMgr::getInstance().findPlantByBlockId(nBlockId);
		if (data)
		{
			NotesDataMgr::getInstance().addItem(NOTES_DATA_TPY_PLANT_CUT, data->m_uuid);
		}
		
		NS::I64 uuid = PlantDataMgr::getInstance().RemovePlantXY(msg.pos_x(), msg.pos_y());
		if (uuid != 0)
		{
			HandlerPlantEvent evt;
			evt.m_uuid = uuid;
			evt.m_blockId = msg.pos_x() + msg.pos_y()*PLANT_ROW_NUM;
			evt.m_type = HandlerPlantEvent::EventPlant::EventPlant_Del;
			POST_DATA_EVENT(evt);
		}
	}
}


