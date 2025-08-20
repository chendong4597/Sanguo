/*!
* \file NpcProtectorData.cpp
*
* \author LiQiangqiang;
* \date 2017/09/21
*
*
*/
#include "stdafx.h"
#include "NpcProtectorData.h"
#include "../config/BuildRoomCfg.h"
#include "../config/ItemCfg.h"
#include "../common/Heartbeat.h"
#include <stdint.h>
#include "ItemData.h"
#include "../common/calculate.h"
#include "Cache.h"
#include "../config/NpcActiveCfg.h"
#include "../config/NpcTypeCfg.h"
#include "role/Npc.h"
#include "../config/NpcBadgeCfg.h"
#include "../config/TowerBuildingCfg.h"
#include "MapDataMgr.h"
#include <StringUtil.h>
#include "../config/PlayerAttrNameCfg.h"
#include "../config/BuffCfg.h"
#include "config/HouseCfg.h"
#include "data/HouseData.h"

using namespace NS;

// NPC是否可現身
bool NpcProtectorData::CanAppear()
{
	switch (status)
	{
	default:
		return false;
	case MsgPB::PROTECTOR_STATUS::BADGE_STATUS_SP:
	case MsgPB::PROTECTOR_STATUS::BADGE_STATUS_ENABLE_FUNCTION: // 已入住，啟用房間功能
	case MsgPB::PROTECTOR_STATUS::BADGE_STATUS_PATROL:			// 巡邏中，不在房間
		return true;
	}
}


bool NpcProtectorData::CanBattle()
{
	return equipId != 0 && weaponId1 != 0 && weaponId2 != 0;
}

//辉章是否在房间;
bool NpcProtectorData::isAtRoom()
{
	return houseId > 0;
}

// 是否可操控的狀態;
bool NpcProtectorData::ControllableStatus()
{
	switch (status)
	{
	default:
		return false;
	case MsgPB::PROTECTOR_STATUS::BADGE_STATUS_ENABLE_FUNCTION: // 已入住，啟用房間功能
	//case MsgPB::PROTECTOR_STATUS::BADGE_STATUS_REFRESH_RENT: // 租金刷新
	//case MsgPB::PROTECTOR_STATUS::BADGE_STATUS_REFRESH_GIFT: // 租金刷新（有送禮）
	case MsgPB::PROTECTOR_STATUS::BADGE_STATUS_PATROL:	// 巡邏中，不在房間
		return true;
	}
}

NpcProtectorDataMgr* NpcProtectorDataMgr::getInstance()
{
	static NpcProtectorDataMgr s_npcProtectorDataMgr;
	return &s_npcProtectorDataMgr;
}

bool NpcProtectorDataMgr::isBadgeAlive(int badgeDefid)
{
	auto data = getNpcProtectorDataByDefId(badgeDefid);
	if (!data)
		return false;
	return (data->status == BADGE_STATUS_DEATH || data->status == BADGE_STATUS_NONE) ? false : true;
}

bool NpcProtectorDataMgr::isBadgeUnlock(int badgeDefid)
{
	auto data = getNpcProtectorDataByDefId(badgeDefid);
	if (!data)
		return false;
	return (data->status == BADGE_STATUS_LOCK || data->status == BADGE_STATUS_NONE) ? false : true;
}

bool NpcProtectorDataMgr::isHasBadgeUnlock()
{
	auto npcList = NpcProtectorDataMgr::getInstance()->getProtectorDataMap();
	for (auto data : npcList)
	{
		auto badgeData = data.second;
		if ((badgeData->CanAppear() || badgeData->status == BADGE_STATUS_DEATH) && badgeData->status != BADGE_STATUS_SP)
		{
			return true;
		}
	}
	return false;
}

bool NpcProtectorDataMgr::isHasSameNpc(int itemId)
{
	auto&& pNpcType = NpcTypeCfg::getInstance().GetNpcType(itemId);
	if (!pNpcType)
	{
		return false;
	}

	auto npcList = NpcProtectorDataMgr::getInstance()->getProtectorDataMap();
	for (auto data : npcList)
	{
		auto badgeData = data.second;
		auto&& pNpcType1 = NpcTypeCfg::getInstance().GetNpcType(badgeData->itemid);
		if (!pNpcType1)
		{
			return false;
		}

		if (badgeData->itemid == itemId) {
			return true;
		}

		if (pNpcType1->n_Type == NpcTypeCfg::NpcType_Genels && pNpcType->n_Type == NpcTypeCfg::NpcType_Genels
			&& itemId / 1000 == badgeData->itemid / 1000 && itemId < badgeData->itemid) {
			return true;
		}
	}
	return false;
}

bool NpcProtectorDataMgr::isHasBadgeByType(int nType)
{
	auto npcList = NpcProtectorDataMgr::getInstance()->getProtectorDataMap();
	for (auto data : npcList)
	{
		auto badgeData = data.second;
		if (nType >= 14700) {
			if (badgeData->itemid / 1000 == nType/10)
			{
				return true;
			}
		}
		else if (badgeData->itemid/100 == nType)
		{
			return true;
		}
	}
	return false;
}


void NpcProtectorDataMgr::postNpcBadgeStateChangedS2CEvent(std::shared_ptr<NpcProtectorData> pData, bool bClear)
{
	//NpcBadgeStateChangedS2CEvent evt;
	//evt.m_updatedData = pData;
	//evt.m_bClear = bClear;
	//postEvent(evt);
}

void NpcProtectorDataMgr::createDefPlayerProtectorDataToMap(MsgPB::MsgProtectorDataToClient &msgData)
{
	auto dataToAdd = createDeferNpcProtectorDataFromMsg(msgData, m_defPlayerProtectorDataMap);

	dataToAdd->m_bIsDef = true;
	m_defPlayerProtectorDataMap[dataToAdd->uuid] = dataToAdd;
	postNpcBadgeStateChangedS2CEvent(dataToAdd);
}

void NpcProtectorDataMgr::createDefPlayerProtectorData(const MsgPB::MsgProtectorSyncS2C &msg)
{
	clearDefPlayerProtectorData();
	int nDataSize = msg.data_size();
	for (int i{ 0 }; i < nDataSize; ++i)
	{
		auto msgData = msg.data(i);
		createDefPlayerProtectorDataToMap(msgData);
	}
}

//清理段位塞敌方NPC辉章数据;
void NpcProtectorDataMgr::clearSegDefPlayerProtectorData()
{
	m_pvpSegDeferProtectorDataMap.clear();
}

//段位赛敌方NPC辉章数据创建;
void NpcProtectorDataMgr::createSegDeferProtectorData(MsgPB::MsgProtectorDataToClient &msg)
{
	auto badgeId = BuildRoomCfg::getInstance().getBadgeId(msg.templateid());
	msg.set_uuid(badgeId);
	auto dataToAdd = createDeferNpcProtectorDataFromMsg(msg, m_pvpSegDeferProtectorDataMap,true);
	dataToAdd->m_bIsDef = true;
	m_pvpSegDeferProtectorDataMap[dataToAdd->uuid] = dataToAdd;
}

void NpcProtectorDataMgr::postBadgeDataToBadgeComp(bool bClear)
{
	auto dataMap = NpcProtectorDataMgr::getInstance()->getProtectorDataMap();
	for (auto it : dataMap)
	{
		if (it.second->status != BADGE_STATUS_LOCK)
		{
			postNpcBadgeStateChangedS2CEvent(it.second, bClear);
		}
	}
}

bool NpcProtectorDataMgr::updateNpcProtectorData(const MsgPB::MsgProtectorUpdateS2C& msg)
{
	auto msgData = msg.data();
	auto data = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByUuid(msgData.uuid());
	if (!data) {
		data = std::make_shared<NpcProtectorData>();
		m_uuid2NpcProtectorDataMap[msgData.uuid()] = data;
	}
	data->uuid = msgData.uuid();
	if (msgData.mapobjectid() != 0)
		data->setMapObjId(msgData.mapobjectid());
	data->templateid = msgData.templateid();
	data->itemid = msgData.itemid();
	data->setLevel(msgData.level());
	data->setComfor(msgData.comfort());
	data->setState(msgData.status());
	data->setEquipId(msgData.equipid());
	data->setWeapon1(msgData.weaponid1());
	data->setWeapon2(msgData.weaponid2());
	data->setHouseId(msgData.houseid());
	data->setRoomConditionFlag(msgData.required());
	data->setFollowUUID(msgData.follow_uuid());
	return true;
}

bool NpcProtectorDataMgr::syncNpcProtectorData(const MsgPB::MsgProtectorSyncS2C& msg)
{
	int nDataSize = msg.data_size();
	for (int i{ 0 }; i < nDataSize; ++i)
	{
		auto msgData = msg.data(i);
		auto dataToAdd = createNpcProtectorDataFromMsg(msgData, false);
		m_uuid2NpcProtectorDataMap[dataToAdd->uuid] = dataToAdd;
	}
	return true;
}


bool NpcProtectorDataMgr::addNpcProtectorData(const MsgPB::MsgProtectorUpdateS2C& msg)
{
	return true;
}

std::shared_ptr<NpcProtectorData> NpcProtectorDataMgr::createNpcProtectorDataFromMsg(MsgPB::MsgProtectorDataToClient &msgData, bool isCanSetLastState)
{
	auto&& data = createDeferNpcProtectorDataFromMsg(msgData, m_uuid2NpcProtectorDataMap, false);
	auto npc = NpcMgr::getInstance().getNpc(data->mapObjectID);
	if (npc)
		npc->calcAttr();

	return data;
}

bool NpcProtectorDataMgr::removeNpcProtectorData(const MsgPB::MsgProtectorRemoveS2C& msg)
{
	for (auto it : m_uuid2NpcProtectorDataMap)
	{
		if (it.second && it.second.get() && it.second.get()->uuid == msg.uuid())
		{
			it.second = nullptr;
			m_uuid2NpcProtectorDataMap.erase(it.first);
			break;
		}
	}
	return true;
}

void NpcProtectorDataMgr::removeAllNpcProtectorData()
{
	m_uuid2NpcProtectorDataMap.clear();
}

void NpcProtectorDataMgr::Clear()
{
	m_isCanBirthDefNpc = false;
	m_vecNeedShowNpcAtterType.clear();
	removeAllNpcProtectorData();
}

int NpcProtectorDataMgr::GetProtectorProsperous() const
{
	int nProsperous = 0;
	Uuid2NpcProtectorDataMap::const_iterator iter = m_uuid2NpcProtectorDataMap.begin();
	for (; iter != m_uuid2NpcProtectorDataMap.end(); iter++) {
		if (iter->second->houseId <= 0)
		{
			continue;
		}
		auto&& npcBadge = NpcBadgeCfg::getInstance().findNpcBadgeCfg(iter->second->itemid, iter->second->level);
		if (!npcBadge) {
			continue;
		}
		nProsperous += npcBadge->n_Prosperous;
	}
	return nProsperous;
}

std::shared_ptr<NpcProtectorData> NpcProtectorDataMgr::getNpcProtectorDataByNpcMapObjId(const NS::int32 nMapObjId)
{
	for (auto it : m_uuid2NpcProtectorDataMap)
	{
		if (it.second && it.second.get() && it.second.get()->mapObjectID == nMapObjId)
		{
			return  it.second;
		}
	}
	return nullptr;

}

std::shared_ptr<NpcProtectorData> NpcProtectorDataMgr::getNpcProtectorDataByUuid(const NS::I64 nUuid)
{
	auto&& iter = m_uuid2NpcProtectorDataMap.find(nUuid);
	if (iter == m_uuid2NpcProtectorDataMap.end()) { return nullptr; }

	return iter->second;
}

std::shared_ptr<NpcProtectorData> NpcProtectorDataMgr::getNpcProtectorDataByDefId(const NS::int32 nDefId)
{
	for (auto it : m_uuid2NpcProtectorDataMap)
	{
		if (it.second && it.second.get() && it.second.get()->templateid == nDefId)
		{
			return  it.second;
		}
	}
	return nullptr;
}


std::shared_ptr<NpcProtectorData> NpcProtectorDataMgr::getNpcProtectorDataByBadgeBuildType(const NS::int32 BadgeItemid)
{
	for (auto it : m_uuid2NpcProtectorDataMap)
	{
		if (it.second && it.second.get() && it.second.get()->itemid == BadgeItemid)
		{
			return  it.second;
		}
	}
	return nullptr;
}

NS::tstring NpcProtectorDataMgr::parseConditionUnMatch(const NpcProtectorData& data)
{
	auto pBuildRoomDef = BuildRoomCfg::getInstance().findBuilRoomDef((int)data.uuid);
	if (!pBuildRoomDef)
	{
		return _LC("CLI_NpcBadgeSys-Undefined BuildRoom config!");
	}

	switch (data.roomConditionUnmatchedFlag)
	{
	case MsgPB::PROTECTOR_LESS_THAN_REQUIRED::PROTECTOR_LESS_THAN_REQUIRED_NONE:
		break;

	case MsgPB::PROTECTOR_LESS_THAN_REQUIRED::PROTECTOR_LESS_THAN_REQUIRED_SPACE:
		return _LC("CLI_NpcBadgeSys-room space param error");
		break;

	case MsgPB::PROTECTOR_LESS_THAN_REQUIRED::PROTECTOR_LESS_THAN_REQUIRED_WALL:
		break;

	case MsgPB::PROTECTOR_LESS_THAN_REQUIRED::PROTECTOR_LESS_THAN_REQUIRED_FURNITURE:
	{
	} break;

	case MsgPB::PROTECTOR_LESS_THAN_REQUIRED::PROTECTOR_LESS_THAN_REQUIRED_COMFORE:
		return fmt::format(_LC("CLI_NpcBadgeSys-room comfort is not enough"));
		break;

	case MsgPB::PROTECTOR_LESS_THAN_REQUIRED::PROTECTOR_LESS_THAN_REQUIRED_HEIGHT:
		return fmt::format(_LC("CLI_NpcBadgeSys-room height not enough"));

	default:
		break;
	}
	return _LC("CLI_NpcBadgeSys-Undefined Room Condition Type");
}

void NpcProtectorData::setComfor(int value)
{
	comfort = value;
}

void NpcProtectorData::setHouseId(int value)
{
	houseId = value;
}

void NpcProtectorData::setRoomConditionFlag(MsgPB::PROTECTOR_LESS_THAN_REQUIRED flg)
{
	roomConditionUnmatchedFlag = flg;
}

void NpcProtectorData::setOwnerMapObjId(NS::I64 id)
{
	m_ownerMapObjId = id;
}

void NpcProtectorData::setMapObjId(int id)
{
	mapObjectID = id;
}

void NpcProtectorData::setHaveCreateNpc(bool haveCreate)
{
	m_haveCreateNpc = haveCreate;
}

void NpcProtectorData::setLevel(int value)
{
	level = value;
}

void NpcProtectorData::setHp(int value)
{
	hp = value;
}

void NpcProtectorData::setEquipId(NS::I64 nEquipid)
{ 
	equipId = nEquipid; 
}

void NpcProtectorData::setWeapon1(NS::I64 nWeapon1)
{
	weaponId1 = nWeapon1;
}

void NpcProtectorData::setWeapon2(NS::I64 nWeapon2)
{
	weaponId2 = nWeapon2;
}

void NpcProtectorData::setFollowUUID(NS::I64 n64FollowUUID)
{
	follow_uuid = n64FollowUUID;
}

void NpcProtectorData::setIsCanCreateSpNpc(bool value)
{
	m_isCanCreateAtSpState = value;
}

void NpcProtectorData::setState(MsgPB::PROTECTOR_STATUS state)
{
	status = state;
}

//////////////////////////////////////////////////////////////////////////
bool NpcProtectorDataMgr::isHaveBadgeData(NS::int32 nDefId)
{
	auto data = getNpcProtectorDataByDefId(nDefId);
	if (!data) return false;
	return data->CanAppear();
}

NS::I64 NpcProtectorData::getEquipId()
{
	return equipId;
}

void NpcProtectorData::RemoveProtectorData()
{
	setOwnerMapObjId(0);
	if (status != MsgPB::PROTECTOR_STATUS::BADGE_STATUS_DEATH)
	{
		setState(MsgPB::PROTECTOR_STATUS::BADGE_STATUS_NONE);
	}
	
	setComfor(0);
}


bool NpcProtectorDataMgr::isHaveNpc(int npcDefId)
{
	auto data = getNpcProtectorDataByDefId(npcDefId);
	if (!data) return false;
	return data->CanAppear();
}

//**********************************************************************def****************************************************************************************************/
void NpcProtectorDataMgr::clearDefPlayerProtectorData()
{
	m_defPlayerProtectorDataMap.clear();
}

std::shared_ptr<NpcProtectorData> NpcProtectorDataMgr::getDefNpcProtectorDataByUuid(const NS::I64 nUuid)
{
	auto&& iter = m_defPlayerProtectorDataMap.find(nUuid);
	if (iter == m_defPlayerProtectorDataMap.end()) { return nullptr; }

	return iter->second;
}

std::shared_ptr<NpcProtectorData> NpcProtectorDataMgr::createDeferNpcProtectorDataFromMsg(MsgPB::MsgProtectorDataToClient &msgData, Uuid2NpcProtectorDataMap &mapData, bool isPvPSeg)
{
	auto dataToAdd = NpcProtectorDataMgr::getInstance()->getDefNpcProtectorDataByUuid(msgData.uuid());
	if (isPvPSeg)
	{
		for (auto it : m_pvpSegDeferProtectorDataMap)
		{
			if (it.second && it.second.get() && it.second.get()->uuid == msgData.uuid())
			{
				dataToAdd = it.second;
			}
		}
	}
	if (!dataToAdd)
		dataToAdd = std::make_shared<NpcProtectorData>();
	dataToAdd->uuid = msgData.uuid();
	if (msgData.mapobjectid() != 0)
		dataToAdd->setMapObjId(msgData.mapobjectid());
	dataToAdd->templateid = msgData.templateid();
	dataToAdd->itemid = msgData.itemid();
	dataToAdd->setLevel(msgData.level());
	dataToAdd->setComfor(msgData.comfort());
	dataToAdd->setState(msgData.status());
	dataToAdd->setEquipId(msgData.equipid());
	dataToAdd->setWeapon1(msgData.weaponid1());
	dataToAdd->setWeapon2(msgData.weaponid2());
	dataToAdd->setHouseId(msgData.houseid());
	dataToAdd->setRoomConditionFlag(msgData.required());
	dataToAdd->setFollowUUID(msgData.follow_uuid());
	
	mapData[dataToAdd->uuid] = dataToAdd;
	return dataToAdd;
}

std::shared_ptr<NpcProtectorData> NpcProtectorDataMgr::getDefNpcProtectorDataByObjId(const NS::I64 objId)
{
	for (auto it : m_defPlayerProtectorDataMap)
	{
		if (it.second && it.second.get() && it.second.get()->mapObjectID == objId)
		{
			return  it.second;
		}
	}
	return nullptr;
}

std::shared_ptr<NpcProtectorData> NpcProtectorDataMgr::getDefNpcProtectorDataByDefId(const NS::int32 nDefId)
{
	for (auto it : m_defPlayerProtectorDataMap)
	{
		if (it.second && it.second.get() && it.second.get()->templateid == nDefId)
		{
			return  it.second;
		}
	}
	return nullptr;
}

std::shared_ptr<NpcProtectorData> NpcProtectorDataMgr::getSegDefNpcProtectorDataByUuid(const NS::I64 nUuid)
{
	auto&& iter = m_pvpSegDeferProtectorDataMap.find(nUuid);
	if (iter == m_pvpSegDeferProtectorDataMap.end()) { return nullptr; }

	return iter->second;
}

//获取段位赛防守方玩家辉章数据;
std::shared_ptr<NpcProtectorData> NpcProtectorDataMgr::getSegDefNpcProtectorDataByDefId(const NS::int32 nDefId)
{
	for (auto it : m_pvpSegDeferProtectorDataMap)
	{
		if (it.second && it.second.get() && it.second.get()->templateid == nDefId)
		{
			return  it.second;
		}
	}
	return nullptr;
}

std::shared_ptr<NpcProtectorData> NpcProtectorDataMgr::getSegDefNpcProtectorDataByObjId(const NS::int32 nMapObjId)
{
	for (auto it : m_pvpSegDeferProtectorDataMap)
	{
		if (it.second && it.second.get() && it.second.get()->mapObjectID == nMapObjId)
		{
			return  it.second;
		}
	}
	return nullptr;
}

void NpcProtectorDataMgr::getNpcAtters(AttrMap &mapAtter,int npcDefId,std::shared_ptr<NpcProtectorData> data,\
	int playerLevel , int badgeLv, bool bCalEssence, bool bCalLv, bool bCalEquip , bool bCalFightBuff)
{
	// 增加的属性(装备+Buf+成就);
	CEffectSum effectSum;			
	if (data)
	{
		if(bCalLv)
		{
			auto cfg = NpcActiveCfg::getInstance().findBuildRoomActive(data->templateid, badgeLv > 0 ? badgeLv : data->level);
			if (cfg)
			{
				for (auto&& buffId : cfg->vecBuff)
				{
					auto&& buffType = BuffCfg::getInstance().findBuffType(buffId);
					if (buffType)
					{
						for (auto&& effect : buffType->effects)
						{
							effectSum.AddEffect(effect.type, effect.ref_values, 0);
						}
					}
				}
			}
		}
	}

	auto monsterCfg = MonsterCfg::getInstance().findMonsterType(npcDefId);
	if (!monsterCfg)
	{
		assert(0);
		return;
	}

	AttrMap baseAttr;
	monsterCfg->getAttr(MonsterCfg::MonsterCfgType::MonsterCfgBadgeNpc, baseAttr, playerLevel);

	if (bCalFightBuff)
	{
		if (auto&& pActiveNpc = NpcMgr::getInstance().getNpcByDefId(npcDefId))
		{
			pActiveNpc->sumEffectBuf(effectSum);
		}
	}

	recalculateByEffectSum(mapAtter, baseAttr, effectSum);
}

float NpcProtectorDataMgr::getNpcAttrValue(const AttrMap &mapAtter, MsgPB::ROLE_EFFECT_TYPE type)
{
	auto it = mapAtter.find(type);
	if (it != mapAtter.end())
	{
		return it->second;
	}
	return 0.0f;
}

std::shared_ptr<NpcProtectorData> NpcProtectorDataMgr::findNpcProtectorDataByMapObjId(int nMapObjId)
{
	// find play self
	std::shared_ptr<NpcProtectorData> pNpcProtectorData = getNpcProtectorDataByNpcMapObjId(nMapObjId);
	if (pNpcProtectorData)
	{
		return pNpcProtectorData;
	}
	
	// find pvp tower fighter
	pNpcProtectorData = getDefNpcProtectorDataByObjId(nMapObjId);
	if (pNpcProtectorData)
	{
		return pNpcProtectorData;
	}

	// find team pvp fighter;
	pNpcProtectorData = getSegDefNpcProtectorDataByObjId(nMapObjId);
	if (pNpcProtectorData)
	{
		return pNpcProtectorData;
	}

	return nullptr;
}

int NpcProtectorDataMgr::GetSoldRate(NS::I64 uuid)
{
	int nRate = 0;

	auto&& proData = getNpcProtectorDataByUuid(uuid);
	if (!proData || proData->houseId <= 0)
	{
		return 0;
	}
	auto&& mapData = HouseDataMgr::getInstance().GetHouseDatas();
	for (auto&& data : mapData)
	{
		auto&& hCfg = HouseCfg::getInstance().getHouseTypeInfo(data.second->m_houseType);
		if (!hCfg)
		{
			continue;
		}
		if (data.second->m_houseId == BUILD_CITY_ID_WALL ||
			data.second->m_houseId == BUILD_CITY_ID_CHAMER ||
			data.second->m_houseId == BUILD_CITY_ID_ROAD) {
			nRate += hCfg->n_ShopRate;
		}
		else if (data.second->m_houseId == proData->houseId) {
			nRate += hCfg->n_ShopRate;
		}
	}
	return nRate;
}

