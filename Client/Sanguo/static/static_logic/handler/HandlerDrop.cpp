/*!
* \file HandlerDrop.h
*
* \author huangjiangping
* \date 3/17/2017
*
*
*/
#include "stdafx.h"
#include "HandlerDrop.h"
#include "../common/MsgMgr.h"
#include <msg/MsgPacketDrop.pb.h>
#include <data/MapDataMgr.h>
#include "data/role/Player.h"
#include "config/PlantCfg.h"
#include "RandomUtil.h"
//#include "item/ItemDropMgr.h"
#include "HandlerLogin.h"
#include "LogUtil.h"
#include "msg/MsgPVP.pb.h"
#include "msg/MsgType.pb.h"
//#include "ui/common/UiMsgBoxToast.h"

//#include "item/ItemDropBehavior.h"
//#include "player/PlayerComponentMgr.h"
//#include "player/PlayerComponent.h"
#include "HandlerRole.h"
//#include "item/ItemDropCheck.h"
//#include <SceneModule.h>
//#include "map/MapComponent.h"
//#include "map/MapAgentComponent.h"
//#include "ai/RootAgent.h"
//#include "HandlerLoginUtils.h"
#include "data/ItemData.h"

using namespace NS;
using namespace MsgPB;

static HandlerDrop* g_HandlerDrop = nullptr;
HandlerDrop& HandlerDrop::getInstance()
{
    assert(g_HandlerDrop);
    return *g_HandlerDrop;
}

HandlerDrop::HandlerDrop()
{
    assert(!g_HandlerDrop);
    g_HandlerDrop = this;
	reset();
}

HandlerDrop::~HandlerDrop()
{
    g_HandlerDrop = nullptr;
}

bool HandlerDrop::initialize()
{
	REGISTER_S2C(onMsgPrivateDropListS2C);
	REGISTER_S2C(onMsgPublicDropListS2C);

	REGISTER_S2C(onMsgPrivateDropDelS2C);
	REGISTER_S2C(onMsgPublicDropDelS2C);

	REGISTER_S2C(onMsgPrivateDropSetS2C);
	REGISTER_S2C(onMsgPublicDropSetS2C);

	REGISTER_S2C(onMsgPvPRandItemChestAppearance_MS2C);
	//REGISTER_S2C(onMsgPvPRandItemGrassAppearance_MS2C);
	REGISTER_S2C(onMsgPvPRandChestDispear_MS2C);
	REGISTER_S2C(onMsgPublicDropAppearanceS2C);

	REGISTER_S2C(onMsgPVPDiscardWeaponS2C);
	REGISTER_S2C(onMsgPVPChangeWeaponS2C);
	REGISTER_S2C(onMsgPVPChoiceWeaponS2C);
	REGISTER_S2C(onMsgDropPickUpErrS2C);

	//REG_EVENT(&HandlerLogin::getInstance())[this](const Event_MapLoading& evt)
	//{
	//	clearAllDrop();
	//	ItemDropMgr::getInstance().clearAllItems();
	//	ItemDropCheck::getInstance().stopCheck();
	//};

	//REG_EVENT(&HandlerLogin::getInstance())[this](const Event_MapPvPStart& evt)
	//{
	//	ItemDropCheck::getInstance().startCheck();
	//};

	return true;
}

void HandlerDrop::terminate()
{
	reset();
}

bool HandlerDrop::reset()
{
	return true;
}

bool HandlerDrop::do_drop(int nId)
{
	return false;
}

void HandlerDrop::onMsgPublicDropAppearanceS2C(const MsgPB::MsgPublicDropAppearanceS2C& msg)
{
}

bool HandlerDrop::addDrop(int insId, int defId, const glm::vec2& pos)
{
	return true;
}

//私人地图掉落结果
void HandlerDrop::onMsgPrivateDropListS2C(const MsgPB::MsgPrivateDropListS2C& msg)
{
	if (msg.drop_info_size() > 0)
	{
		for (int i = 0; i < msg.drop_info_size(); ++i)
		{
			const MsgPB::PBDropInfo& info = msg.drop_info(i);
			auto ite = m_mpDropSet.find(info.obj_id());
			if (ite == m_mpDropSet.end())
			{
				DropInfoItemData infoData;
				infoData.obj_id = info.obj_id();
				infoData.data_id = info.data_id();
				infoData.m_iggId = msg.iggid();
				infoData.m_mapId = msg.mapid();
				infoData.m_state = EDROP_WAIT_PICKUP;
				infoData.count = info.count();
				infoData.m_endMap = glm::ivec2{ info.pos_x(), info.pos_y() };
				infoData.m_startMap = glm::ivec2{ info.pos_x(), info.pos_y() };
				infoData.m_isWeapon = ItemPacketMgr::getInstance().isWeaponByTempId(infoData.data_id);
				infoData.m_isArm = ItemPacketMgr::getInstance().isArmByTempId(infoData.data_id);
				playAnimation(infoData);
			}
			else
			{
// 				playAnimation(ite->second);
			}
		}
	}
}

//公有地图掉落结果
void HandlerDrop::onMsgPublicDropListS2C(const MsgPB::MsgPublicDropListS2C& msg)
{
	if (msg.drop_info_size() > 0)
	{
		for (int i = 0; i < msg.drop_info_size(); ++i)
		{
			const MsgPB::PBDropInfo& info = msg.drop_info(i);
			auto ite = m_mpDropSet.find(info.obj_id());
			if (ite == m_mpDropSet.end())
			{
				DropInfoItemData infoData;
				infoData.obj_id = info.obj_id();
				infoData.data_id = info.data_id();
				infoData.m_iggId = msg.iggid();
				infoData.m_isWeapon = ItemPacketMgr::getInstance().isWeaponByTempId(infoData.data_id);
				infoData.m_isArm = ItemPacketMgr::getInstance().isArmByTempId(infoData.data_id);

				infoData.m_state = EDROP_WAIT_PICKUP;
				infoData.count = info.count();
				infoData.m_endMap = glm::ivec2{ info.pos_x(), info.pos_y() };
				infoData.m_startMap = glm::ivec2{ info.pos_x(), info.pos_y() };
				playAnimationAtPublicMap(infoData);
			}
		}
	}
}

//私人地图掉落结果
void HandlerDrop::onMsgPrivateDropSetS2C(const MsgPB::MsgPrivateDropSetS2C& msg)
{
	int nSize = msg.drop_info_size();
	if (nSize > 0)
	{
		for (int i = 0; i < nSize; ++i)
		{
			const MsgPB::PBDropInfo& info = msg.drop_info(i);
			auto ite = m_mpDropSet.find(info.obj_id());
			DropInfoItemData infoData;
			if (ite == m_mpDropSet.end())
			{
				infoData.obj_id = info.obj_id();
				infoData.data_id = info.data_id();
				infoData.m_iggId = msg.iggid();
				infoData.m_state = EDROP_DROPING;
				infoData.count = info.count();
				infoData.m_endMap = glm::ivec2{ info.pos_x(), info.pos_y() };
				infoData.m_startMap = glm::ivec2{ info.pos_x(), info.pos_y() };
			}
			else
			{
				infoData.obj_id = ite->second.obj_id;
				infoData.data_id = ite->second.data_id;
				infoData.m_iggId = ite->second.m_iggId;
				infoData.m_state = EDROP_DROPING;
				infoData.count = ite->second.count;
				infoData.m_startMap = ite->second.m_endMap;
				infoData.m_endMap = glm::ivec2{ info.pos_x(), info.pos_y() };
			}

			//m_vDropData.push_back(infoData);
			playAnimation(infoData);
		}
	}
}

//公有地图掉落结果
void HandlerDrop::onMsgPublicDropSetS2C(const MsgPB::MsgPublicDropSetS2C& msg)
{
	int nSize = msg.drop_info_size();
	if (nSize > 0)
	{
		for (int i = 0; i < nSize; ++i)
		{
			const MsgPB::PBDropInfo& info = msg.drop_info(i);
			auto ite = m_mpDropSet.find(info.obj_id());
			DropInfoItemData infoData;
			if (ite == m_mpDropSet.end())
			{
				infoData.obj_id = info.obj_id();
				infoData.data_id = info.data_id();
				infoData.m_iggId = msg.iggid();
				infoData.m_state = EDROP_DROPING;
				infoData.count = info.count();
				infoData.m_endMap = glm::ivec2{ info.pos_x(), info.pos_y() };
				infoData.m_startMap = glm::ivec2{ info.pos_x(), info.pos_y() };
				infoData.m_isWeapon = ItemPacketMgr::getInstance().isWeaponByTempId(infoData.data_id);
				infoData.m_isArm = ItemPacketMgr::getInstance().isArmByTempId(infoData.data_id);
			}
			else
			{
				infoData.obj_id = ite->second.obj_id;
				infoData.data_id = ite->second.data_id;
				infoData.m_iggId = ite->second.m_iggId;
				infoData.m_state = EDROP_DROPING;
				infoData.count = ite->second.count;
				infoData.m_isWeapon = ite->second.m_isWeapon;
				infoData.m_isArm = ite->second.m_isArm;
				infoData.m_startMap = ite->second.m_endMap;
				infoData.m_endMap = glm::ivec2{ info.pos_x(), info.pos_y() };
			}
			playAnimationAtPublicMap(infoData);
		}
	}
}

//删除私人地图上的掉落物
void HandlerDrop::onMsgPrivateDropDelS2C(const MsgPB::MsgPrivateDropDelS2C& msg)
{
//	NS::I64 nObj_id = msg.obj_id();
//	MP_DROP_INFO::iterator ite = m_mpDropSet.find(nObj_id);
//	if (ite != m_mpDropSet.end())
//	{
//		auto resion = msg.del_reason();
//		if (resion == MsgPB::ENUM_DROP_DEL_REASON_TIMEOUT)
//		{
//			ItemDropMgr::getInstance().removeDropObj(nObj_id, msg.iggid(), true);
//			removeItem(nObj_id);
//			notifyDropDisappear(nObj_id);
//			return;
//		}
//
//		if (resion == MsgPB::ENUM_DROP_DEL_REASON_MOBPICK)
//		{
//			//蚂蚁据点怪物拾取
//			ItemDropMgr::getInstance().removeDropObj(nObj_id, msg.iggid(), true);
//			removeItem(nObj_id);
//			notifyDropDisappear(nObj_id);
//			return;
//		}
//
////		auto itemObj = ItemDropMgr::getInstance().findItemDropObj(nObj_id);
//		if (PlayerMgr::getInstance().isMyself(msg.iggid()))
//		{
//			HandlerDropEvent event;
//			event.m_type = HandlerDropEvent::EventType::Drop_Get_Item;
//			event.m_nDefId = ite->second.data_id;
//			event.m_nCount = ite->second.count;
//			event.m_nUserUuid = ite->second.m_iggId;
//			postEvent(event);
//
//			ite->second.m_isWeapon = ItemPacketMgr::getInstance().isWeaponByTempId(ite->second.data_id);
//			ite->second.m_isArm = ItemPacketMgr::getInstance().isArmByTempId(ite->second.data_id);
//			if (ite->second.m_isWeapon && HandlerLogin::getInstance().getMapType() == MapDataMgr::MapType::PVP)
//			{
//				ItemDropMgr::getInstance().addWeapon(ite->second.data_id);
//				refreshPvpWeaponView();
//			}
//		}
//		ItemDropMgr::getInstance().removeDropObj(nObj_id, msg.iggid());
//		removeItem(nObj_id);
//		notifyDropDisappear(nObj_id);
//	}
}

//删除公有地图上的掉落物
void HandlerDrop::onMsgPublicDropDelS2C(const MsgPB::MsgPublicDropDelS2C& msg)
{
	//NS::I64 nObj_id = msg.obj_id();
	//MP_DROP_INFO::iterator ite = m_mpDropSet.find(nObj_id);
	//if (ite != m_mpDropSet.end())
	//{
	//	auto resion = msg.del_reason();
	//	if (resion == MsgPB::ENUM_DROP_DEL_REASON_TIMEOUT)
	//	{
	//		ItemDropMgr::getInstance().removeDropObj(nObj_id, msg.iggid(), true);
	//		ItemDropMgr::getInstance().removeDropEffectObj(nObj_id);
	//		removeItem(nObj_id);
	//		return;
	//	}

	//	auto itemConfig = ItemCfg::getInstance().getItemById(ite->second.data_id);
	//	if (itemConfig && itemConfig->m_subType != MsgPB::ITEM_SUBTYPE::ITEM_SUBTYPE_CHEST)
	//	{
	//		if (PlayerMgr::getInstance().isMyself(msg.iggid()))
	//		{
	//			HandlerDropEvent event;
	//			event.m_type = HandlerDropEvent::EventType::Drop_PvP_Item;
	//			event.m_nDefId = ite->second.data_id;
	//			event.m_nCount = ite->second.count;
	//			event.m_nUserUuid = msg.iggid();
	//			postEvent(event);
	//			ite->second.m_isWeapon = ItemPacketMgr::getInstance().isWeaponByTempId(ite->second.data_id);
	//			ite->second.m_isArm = ItemPacketMgr::getInstance().isArmByTempId(ite->second.data_id);
	//			if (ite->second.m_isWeapon && HandlerLogin::getInstance().getMapType() == MapDataMgr::MapType::PVP)
	//			{
	//				ItemDropMgr::getInstance().addWeapon(ite->second.data_id);
	//				refreshPvpWeaponView();
	//			}
	//		}
	//	}
	//	ItemDropMgr::getInstance().removeDropObj(nObj_id, msg.iggid());
	//	ItemDropMgr::getInstance().removeDropEffectObj(nObj_id);
	//	removeItem(nObj_id);
	//}
}

//公有地图宝箱出现
void HandlerDrop::onMsgPvPRandItemChestAppearance_MS2C(const MsgPB::MsgPvPRandItemChestAppearance_MS2C& msg)
{
	//DropInfoItemData infoData;
	//auto ite = m_mpDropSet.find(msg.mapobjectid());
	//if (ite == m_mpDropSet.end())
	//{
	//	infoData.obj_id = msg.mapobjectid();
	//	infoData.data_id = msg.itemid();
	//	infoData.m_state = EDROP_WAIT_PICKUP;
	//	infoData.count = 1;
	//	infoData.m_endMap = glm::ivec2{ msg.posx(), msg.posy() };
	//	infoData.m_startMap = glm::ivec2{ msg.posx(), msg.posy() };
	//}
	//else
	//{
	//	infoData.obj_id = ite->second.obj_id;
	//	infoData.data_id = ite->second.data_id;
	//	infoData.m_state = EDROP_WAIT_PICKUP;
	//	infoData.count = ite->second.count;
	//	infoData.m_startMap = ite->second.m_endMap;
	//	infoData.m_endMap = glm::ivec2{ msg.posx(), msg.posy() };
	//}
	//playAnimationAtPublicMap(infoData,true);
}

//公有地图宝箱消失
void HandlerDrop::onMsgPvPRandChestDispear_MS2C(const MsgPB::MsgPvPRandChestDispear_MS2C& msg)
{
//	NS::I64 nObj_id = msg.mapobjectid();
//	MP_DROP_INFO::iterator ite = m_mpDropSet.find(nObj_id);
//	if (ite != m_mpDropSet.end())
//	{
////		auto OldInfo = ite->second;
////		auto itemObj = ItemDropMgr::getInstance().findItemDropObj(nObj_id);
//		ItemDropMgr::getInstance().removeDropCheatObj(nObj_id);
//		removeItem(nObj_id);
//
//		ChestDropInfo info = msg.drop_info(0);
//		DropInfoItemData infoData;
//		infoData.obj_id = info.obj_id();
//		infoData.data_id = info.data_id();
//		infoData.m_isWeapon = ItemPacketMgr::getInstance().isWeaponByTempId(infoData.data_id);
//		infoData.m_isArm = ItemPacketMgr::getInstance().isArmByTempId(infoData.data_id);
//
//		infoData.m_state = EDROP_DELAY_PICK;
//		infoData.count = info.count();
//		infoData.m_endMap = glm::ivec2{ info.pos_x(), info.pos_y() - 1 };
//		infoData.m_startMap = glm::ivec2{ info.pos_x(), info.pos_y() - 1 };
//		playAnimationAtPublicMap(infoData);
//	}
}

void HandlerDrop::onMsgPvPRandItemGrassAppearance_MS2C(const MsgPB::MsgPvPRandItemGrassAppearance_MS2C& msg)
{
	//DropInfoItemData infoData;
	//auto ite = m_mpDropSet.find(msg.mapobjectid());
	//if (ite == m_mpDropSet.end())
	//{
	//	infoData.obj_id = msg.mapobjectid();
	//	infoData.data_id = msg.itemid();
	//	infoData.m_state = EDROP_WAIT_PICKUP;
	//	infoData.count = 1;
	//	infoData.m_endMap = glm::ivec2{ msg.posx(), msg.posy() };
	//	infoData.m_startMap = glm::ivec2{ msg.posx(), msg.posy() };
	//}
	//else
	//{
	//	infoData.obj_id = ite->second.obj_id;
	//	infoData.data_id = ite->second.data_id;
	//	infoData.m_state = EDROP_WAIT_PICKUP;
	//	infoData.count = ite->second.count;
	//	infoData.m_startMap = ite->second.m_endMap;
	//	infoData.m_endMap = glm::ivec2{ msg.posx(), msg.posy() };
	//}
	//playAnimationAtPublicMap(infoData);
}

bool HandlerDrop::removeItem(NS::I64 delUuid)
{
	auto ite = m_mpDropSet.find(delUuid);
	if (ite != m_mpDropSet.end())
	{
		m_mpDropSet.erase(ite);
	}
	return true;
}

void HandlerDrop::notifyDropDisappear(NS::I64 dropUuid)
{
	//auto mapComp = SceneModule::getCurrentWorld()->getCurrentLevel()->findComponent<MapComponent>();
	//if (!mapComp) return;

	////if (mapComp->getMapData() && mapComp->getMapData()->getId() == )
	//{
	//	//沙漠地图
	//	auto mapAgentComp = mapComp->findComponent<MapAgentComponent>();
	//	if (!mapAgentComp) return;
	//	mapAgentComp->onDropDisappear(dropUuid);
	//}

}

//void HandlerDrop::playSetDataAnimation()
//{
//	if (m_vDropData.size()>0)
//	{
//		for (auto infoData : m_vDropData)
//		{
//			updateAnimation(infoData);
//		}
//	}
//	m_vDropData.clear();
//}

void HandlerDrop::updateAnimation( DropInfoItemData &infoData)
{
	auto ite = m_mpDropSet.find(infoData.obj_id);
	if (ite != m_mpDropSet.end())
	{
		ite->second.m_startMap = infoData.m_startMap;
		ite->second.m_endMap = infoData.m_endMap;
		ite->second.m_state = infoData.m_state;
	}
}

void HandlerDrop::playAnimation(DropInfoItemData &infoData)
{
	//auto ite = m_mpDropSet.find(infoData.obj_id);
	//if (ite == m_mpDropSet.end())
	//{
	//	infoData.m_isWeapon = ItemPacketMgr::getInstance().isWeaponByTempId(infoData.data_id);
	//	infoData.m_isArm = ItemPacketMgr::getInstance().isArmByTempId(infoData.data_id);
	//	infoData.m_isPriveItem = true;
	//	m_mpDropSet[infoData.obj_id] = infoData;
	//	ItemDropMgr::getInstance().updateDropItemData(infoData);
	//}
	//else
	//{
	//	ite->second.m_startMap = infoData.m_startMap;
	//	ite->second.m_endMap = infoData.m_endMap;
	//	ite->second.m_state = infoData.m_state;
	//	ite->second.m_isWeapon = ItemPacketMgr::getInstance().isWeaponByTempId(ite->second.data_id);
	//	ite->second.m_isArm = ItemPacketMgr::getInstance().isArmByTempId(ite->second.data_id);
	//	ite->second.m_isPriveItem = true;
	//	ItemDropMgr::getInstance().updateDropItemData(ite->second,false);
	//}
}

void HandlerDrop::playAnimationAtPublicMap(DropInfoItemData &infoData, bool isChestItem /*false*/)
{
	//auto ite = m_mpDropSet.find(infoData.obj_id);
	//if (ite == m_mpDropSet.end())
	//{
	//	infoData.m_isWeapon = ItemPacketMgr::getInstance().isWeaponByTempId(infoData.data_id);
	//	infoData.m_isArm = ItemPacketMgr::getInstance().isArmByTempId(infoData.data_id);
	//	infoData.m_isPriveItem = false;
	//	m_mpDropSet[infoData.obj_id] = infoData;
	//	if (isChestItem)
	//		ItemDropMgr::getInstance().updatePublicMapDropChestItem(infoData);
	//	else
	//		ItemDropMgr::getInstance().updatePublicMapDropItem(infoData);
	//}
	//else
	//{
	//	ite->second.m_startMap = infoData.m_startMap;
	//	ite->second.m_endMap = infoData.m_endMap;
	//	ite->second.m_state = infoData.m_state;
	//	infoData.m_isPriveItem = false;
	//	ite->second.m_isWeapon = ItemPacketMgr::getInstance().isWeaponByTempId(ite->second.data_id);
	//	ite->second.m_isArm = ItemPacketMgr::getInstance().isArmByTempId(ite->second.data_id);

	//	if (isChestItem)
	//		ItemDropMgr::getInstance().updatePublicMapDropChestItem(ite->second, false);
	//	else
	//		ItemDropMgr::getInstance().updatePublicMapDropItem(ite->second, false);
	//}
}

int HandlerDrop::getPosYByHigthAndDis(int nHigth, int nDistance)
{
	/*nDistance = std::abs(nDistance);
	if (nDistance > nHigth)
	{
		return nHigth;
	}
	else
	{
		return RandomUtil::getRandomInt(nDistance, nHigth);
	}*/
	return 0;
}

//查找掉落物品信息
const DropInfoItemData* HandlerDrop::findDropInfoById(NS::I64 nObjId)
{
	auto&& iterDrop = m_mpDropSet.find(nObjId);
	if (iterDrop != m_mpDropSet.end())
	  {
		  return &iterDrop->second;
	  }
	return nullptr;
}

//清理所有的掉落物品缓存
void HandlerDrop::clearAllDrop()
{
	m_mpDropSet.clear();
	//m_vDropData.clear();
}

//拾取私人地图上的掉落物品请求
bool HandlerDrop::requestGetDropItem(NS::I64 nObjId, int index)
{
	//MsgPB::MsgPrivateDropPickUpC2S msg;
	//msg.set_iggid(PlayerMgr::getInstance().getSelf()->getIggId());
	//msg.set_obj_id(nObjId);
	//if (index != -1)
	//{
	//	PvpPick *pMsg = msg.mutable_pvp_pick();
	//	if (!pMsg)
	//		return false;
	//	pMsg->set_nindex(index);
	//}


	//auto&& role = HandlerRole::findRole(PlayerMgr::getInstance().getSelf()->getId());
	//if (role)
	//{
	//	auto&& roleComp = role->findComponent<RoleComponent>();
	//	if (roleComp)
	//	{
	//		auto&& roleAgentComp = roleComp->findComponent<RootAgentComponent>();
	//		if (roleAgentComp)
	//		{
	//			auto agent = roleAgentComp->getAgent();
	//			if (agent)
	//			{
	//				agent->SendEvent("PickUp");
	//			}
	//		}
	//	}
	//}

	//return MsgMgr::getInstance().sendMessage(_MSG_PRIVATE_DROP_PICKUP_C2S, &msg);
	return true;
}

//拾取公有地图上的掉落物品请求
bool HandlerDrop::requsetGetPublicMapDropItem(NS::I64 nObjId, int index)
{
	MsgPB::MsgPublicDropPickUpC2S msg;
	msg.set_iggid(PlayerMgr::getInstance().getSelf()->getIggId());
	msg.set_obj_id(nObjId);
	if (index != -1)
	{
		PvpPick *pMsg = msg.mutable_pvp_pick();
		if (!pMsg)
			return false;
		pMsg->set_nindex(index);
	}
	return MsgMgr::getInstance().sendMessage(_MSG_PUBLIC_DROP_PICKUP_C2S, &msg);
}

//打开宝箱请求
bool HandlerDrop::requestOpenStoneBox(NS::I64 nObjId)
{
	MsgPB::MsgPvPRandChestOpen_C2MS msg;
	msg.set_iggid(PlayerMgr::getInstance().getSelf()->getIggId());
	msg.set_mapobjectid((int32)nObjId);
	return MsgMgr::getInstance().sendMessage(_MSG_PVP_RAND_CHEST_OPEN_C2MS, &msg);
}

//丢弃快捷栏上的PVP武器请求
bool HandlerDrop::discardWeaponReq(int32 index)
{
	MsgPB::MsgPVPDiscardWeaponC2S msg;
	msg.set_iggid(PlayerMgr::getInstance().getSelf()->getIggId());
	msg.set_index(index);
	return MsgMgr::getInstance().sendMessage(_MSG_PVP_DISCARD_WEAPON_C2S, &msg);
}

//丢弃快捷栏上的PVP武器结果
void HandlerDrop::onMsgPVPDiscardWeaponS2C(const MsgPB::MsgPVPDiscardWeaponS2C& msg)
{
	//tstring strMsg = EMPTY_STRING;
	//switch (msg.res())
	//{
	//case PVP_WEAPON_RESULT::PVP_WEAPON_RESULT_ERR:
	//	strMsg = "PVP_WEAPON_RESULT_ERR";
	//	break;
	//case PVP_WEAPON_RESULT::PVP_WEAPON_RESULT_OK:
	//	ItemDropMgr::getInstance().removeWeapon(msg.index());
	//	refreshPvpWeaponView();
	//	break;
	//case PVP_WEAPON_RESULT::PVP_WEAPON_RESULT_EMPTY:
	//	strMsg = "PVP_WEAPON_RESULT_EMPTY";
	//	break;
	//default:
	//	break;
	//}
	//if (!strMsg.empty())
	//	UiMsgBoxToast::getInstance().showText(strMsg, _LC("onMsgPVPDiscardWeaponS2C"));
}

//交换快捷栏上的PVP武器请求
bool HandlerDrop::changeWeaponReq(int32 index1, int32 index2)
{
	MsgPB::MsgPVPChangeWeaponC2S msg;
	msg.set_iggid(PlayerMgr::getInstance().getSelf()->getIggId());
	msg.set_index1(index1);
	msg.set_index2(index2);
	return MsgMgr::getInstance().sendMessage(_MSG_PVP_CHANGE_WEAPON_C2S, &msg);
}

//交换PVP武器结果
void HandlerDrop::onMsgPVPChangeWeaponS2C(const MsgPB::MsgPVPChangeWeaponS2C& msg)
{
	//tstring strMsg = EMPTY_STRING;
	//switch (msg.res())
	//{
	//case PVP_WEAPON_RESULT::PVP_WEAPON_RESULT_ERR:
	//	strMsg = "PVP_WEAPON_RESULT_ERR";
	//	break;
	//case PVP_WEAPON_RESULT::PVP_WEAPON_RESULT_OK:
	//	ItemDropMgr::getInstance().changeWeapon(msg.index1(), msg.index2());
	//	refreshPvpWeaponView();
	//	break;
	//case PVP_WEAPON_RESULT::PVP_WEAPON_RESULT_EMPTY:
	//	strMsg = "PVP_WEAPON_RESULT_EMPTY";
	//	break;
	//default:
	//	break;
	//}
	//if (!strMsg.empty())
	//	UiMsgBoxToast::getInstance().showText(strMsg, _LC("onMsgPVPChangeWeaponS2C"));
}

//使用PVP武器请求
bool HandlerDrop::useWeaponReq(int32 index)
{
	MsgPB::MsgPVPChoiceWeaponC2S msg;
	msg.set_iggid(PlayerMgr::getInstance().getSelf()->getIggId());
	msg.set_index(index);
	return MsgMgr::getInstance().sendMessage(_MSG_PVP_CHOICE_WEAPON_C2S, &msg);
}

//使用PVP武器结果
void HandlerDrop::onMsgPVPChoiceWeaponS2C(const MsgPB::MsgPVPChoiceWeaponS2C& msg)
{
	//tstring strMsg = EMPTY_STRING;
	//switch (msg.res())
	//{
	//case PVP_WEAPON_RESULT::PVP_WEAPON_RESULT_ERR:
	//	strMsg = "PVP_WEAPON_RESULT_ERR";
	//	break;
	//case PVP_WEAPON_RESULT::PVP_WEAPON_RESULT_OK:
	//	ItemDropMgr::getInstance().selWeapon(msg.index());
	//	refreshPvpWeaponView();
	//	break;
	//case PVP_WEAPON_RESULT::PVP_WEAPON_RESULT_EMPTY:
	//	strMsg = "PVP_WEAPON_RESULT_EMPTY";
	//	break;
	//default:
	//	break;
	//}
	//if (!strMsg.empty())
	//	UiMsgBoxToast::getInstance().showText(strMsg, _LC("onMsgPVPChoiceWeaponS2C"));
}

//刷新PVP武器界面
void HandlerDrop::refreshPvpWeaponView()
{
	//HandlerPvpWeaponEvent event;
	//event.m_selIndex = ItemDropMgr::getInstance().getSelWeapon();
	//event.m_itemId = ItemDropMgr::getInstance().getWeaponTemplateId(event.m_selIndex);
	//postEvent(event);
}

//拾取物品成功与否消息
void HandlerDrop::onMsgDropPickUpErrS2C(const MsgPB::MsgDropPickUpErrS2C& msg)
{
	//tstring strMsg = EMPTY_STRING;
	//if (PlayerMgr::getInstance().isMyself(msg.iggid()))
	//{
	//	switch (msg.res())
	//	{
	//	case ENUM_DROP_RESULT::ENUM_DROP_RESULT_FAIL:
	//		strMsg = _LC("SRV_onMsgDropPickUpErrS2C-ENUM_DROP_RESULT_FAIL");
	//		break;
	//	case ENUM_DROP_RESULT::ENUM_DROP_RESULT_FULL:
	//		strMsg = _LC("SRV_onMsgDropPickUpErrS2C-ENUM_DROP_RESULT_FULL");
	//		break;
	//	case ENUM_DROP_RESULT::ENUM_DROP_RESULT_PVP_ERR_PRIORITY:
	//		strMsg = _LC("SRV_onMsgDropPickUpErrS2C-ENUM_DROP_RESULT_PVP_ERR_PRIORITY");
	//		break;
	//	case ENUM_DROP_RESULT::ENUM_DROP_RESULT_PLAYER_DIE:
	//		strMsg = _LC("SRV_onMsgDropPickUpErrS2C-ENUM_DROP_RESULT_PLAYER_DIE");
	//		break;
	//	case ENUM_DROP_RESULT::ENUM_DROP_RESULT_ERR_DISTANCE:
	//		strMsg = _LC("SRV_onMsgDropPickUpErrS2C-ENUM_DROP_RESULT_ERR_DISTANCE");
	//		break;
	//	case ENUM_DROP_RESULT::ENUM_DROP_RESULT_OK:
	//		break;
	//	default:
	//		break;
	//	}
	//}
	//if (!strMsg.empty())
	//{
	//	UiMsgBoxToast::getInstance().showText(strMsg, _LC("onMsgDropPickUpErrS2C"));

	//	auto ite = m_mpDropSet.find(msg.obj_id());
	//	if (ite != m_mpDropSet.end())
	//	{
	//		GameObject* pDropObj = ItemDropMgr::getInstance().findItemDropObj(ite->second.obj_id);
	//		if (pDropObj)
	//		{
	//			auto pBehavior = pDropObj->findComponent<ItemDropBehavior>();
	//			if (pBehavior)
	//			{
	//				pBehavior->setHaveReq(false);
	//			}
	//		}
	//	}
	//}
}

void HandlerDrop::onMsgDungeonBossDrop_MS2C(const MsgPB::MsgDungeonBossDrop_MS2C& msg)
{
	auto dropInfos = msg.drop_info().size();
	if (dropInfos > 0)
	{
		for (int i = 0; i < dropInfos; ++i)
		{
			const MsgPB::PBDropInfo& info = msg.drop_info(i);
			auto ite = m_mpDropSet.find(info.obj_id());
			if (ite == m_mpDropSet.end())
			{
				DropInfoItemData infoData;
				infoData.obj_id = info.obj_id();
				infoData.data_id = info.data_id();
				infoData.m_iggId = msg.iggid();
				// 				infoData.m_mapId = msg.mapid();
				infoData.m_state = EDROP_WAIT_PICKUP;
				infoData.count = info.count();
				infoData.m_endMap = glm::ivec2{ info.pos_x(), info.pos_y() };
				infoData.m_startMap = glm::ivec2{ info.pos_x(), info.pos_y() };
				playAnimation(infoData);
			}
		}
	}
}

void HandlerDrop::onMsgDungeonBossChestDrop_MS2C(const MsgPB::MsgDungeonBossOpenChestRs_MS2C& msg)
{
//	auto DropCount = msg.drop_info().size();

	if (msg.drop_info().size() > 0)
	{
		for (int i = 0; i < msg.drop_info().size(); ++i)
		{
			const MsgPB::PBDropInfo& info = msg.drop_info(i);
			auto ite = m_mpDropSet.find(info.obj_id());
			if (ite == m_mpDropSet.end())
			{
				DropInfoItemData infoData;
				infoData.obj_id = info.obj_id();
				infoData.data_id = info.data_id();
				infoData.m_iggId = msg.iggid();
				// 				infoData.m_mapId = msg.mapid();
				infoData.m_state = EDROP_WAIT_PICKUP;
				infoData.count = info.count();
				infoData.m_endMap = glm::ivec2{ info.pos_x(), info.pos_y() };
				infoData.m_startMap = glm::ivec2{ info.pos_x(), info.pos_y() };
				playAnimation(infoData);
			}
		}
	}
}

//掉落物品信息
void HandlerDrop::handleDropInfo(const ::google::protobuf::RepeatedPtrField< ::MsgPB::PBDropInfo >& DropInfo, NS::I64 iggId, int32 mapId)
{
	auto DropCount = DropInfo.size();

	if (DropCount > 0)
	{
		for (int i = 0; i < DropCount; ++i)
		{
			MsgPB::PBDropInfo info = DropInfo.Get(i);
			auto ite = m_mpDropSet.find(info.obj_id());
			if (ite == m_mpDropSet.end())
			{
				DropInfoItemData infoData;
				infoData.obj_id = info.obj_id();
				infoData.data_id = info.data_id();
				infoData.m_iggId = iggId;
// 				infoData.m_mapId = mapId;
				infoData.m_state = EDROP_WAIT_PICKUP;
				infoData.count = info.count();
				infoData.m_endMap = glm::ivec2{ info.pos_x(), info.pos_y() };
				infoData.m_startMap = glm::ivec2{ info.pos_x(), info.pos_y() };
				playAnimation(infoData);
			}
		}
	}

}
