/*!
 * \file Monster.cpp
 *
 * \author zhijieli
 * \date 2016/12/22
 *
 * 
 */
#include "stdafx.h"
#include "Monsters.h"
#include "data/NpcProtectorData.h"
#include "LogUtil.h"
#include <RandomUtil.h>
#include "msg/MsgGameDef.pb.h"
#include "../../config/GlobalCfg.h"

Monster::Monster(bzbee::int32 id, bzbee::int32 insId, bzbee::int32 defId)
	: Role(id)
	, m_insId(insId)
	, m_defId(defId)
{
	m_category = MapObj::Category::Monster;

	m_monsterType = MonsterCfg::getInstance().findMonsterType(m_defId);
	if (!m_monsterType)
	{
		LOGE("monster defId not found: {0}.", m_defId);
		m_defId = 1;
		m_monsterType = MonsterCfg::getInstance().findMonsterType(m_defId);
		if (!m_monsterType) { return; }
	}

	//setHp(m_monsterType->n_HPM);
	setMp(m_monsterType->n_MPM);
	setLevel(m_monsterType->n_MobLv);
}


void Monster::onInit()
{
	Role::onInit();
}

void Monster::calcAttr(bool bInherBuff)
{
	m_attrs.clear();
	m_effectSum.CleanUp();

	if (m_npcUuid != 0)   //士兵作为怪物类，基础父将的武器
	{
		auto data = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByUuid(m_npcUuid);
		//if (!m_isMyNpc)
			//data = NpcProtectorDataMgr::getInstance()->getDefNpcProtectorDataByDefId(m_npcUuid);
		if (data)
		{
			if (data->equipId != 0) {
				auto itemData = ItemPacketMgr::getInstance().findItemDataByUuid(data->equipId);
				if (itemData) {
					// 武器攻击速度不带入npc
					itemData->sumEffect(m_effectSum, false);
				}
			}
			if (data->weaponId1 != 0) {
				auto itemData = ItemPacketMgr::getInstance().findItemDataByUuid(data->weaponId1);
				if (itemData) {
					// 武器攻击速度不带入npc
					itemData->sumEffect(m_effectSum, false);
				}
			}
			if (data->weaponId2 != 0) {
				auto itemData = ItemPacketMgr::getInstance().findItemDataByUuid(data->weaponId2);
				if (itemData) {
					// 武器攻击速度不带入npc
					itemData->sumEffect(m_effectSum, false);
				}
			}
		}
	}
	

	// Buff 增加属性
	if (bInherBuff)
		sumEffectBuf(m_effectSum);
	// 计算 当前属性
	AttrMap baseAttr;
	m_monsterType->getAttr(MonsterCfg::MonsterCfgType::MonsterCfgMob, baseAttr, -1);
	recalculateByEffectSum(m_attrs, baseAttr, m_effectSum);
	setMaxHp((int)getAttr(MsgPB::ROLE_EFFECT_HPM));
	if (!isInitialized()) {
		if (getHp() <= 0) {
			setHp(getMaxHp());
		}
	}
	setInitialzed();
}

float Monster::getBaseAttr(MsgPB::ROLE_EFFECT_TYPE type, bool isInhert) const
{
	assert(m_monsterType);
	if (!m_monsterType) { return{}; }
	AttrMap attrs{};
	m_monsterType->getAttr(MonsterCfg::MonsterCfgType::MonsterCfgMob, attrs, -1);
	auto it = attrs.find(type);
	return (it == attrs.end()) ? 0.f : it->second;
}

std::string Monster::GetBattleSound(int nSountTpy)
{
	assert(m_monsterType);
	if (!m_monsterType) { return{}; }

	if (nSountTpy == SOUND_TPY_ATK) {
		if (m_monsterType->s_atk_sound.empty()) {
			return "";
		}
		int nIdx = std::rand() % 100;
		if (nIdx <= m_monsterType->n_atk_tri) return m_monsterType->s_atk_sound;
	}
	else if (nSountTpy == SOUND_TPY_HIT) {
		if (m_monsterType->s_hit_sound.empty()) {
			return "";
		}
		int nIdx = std::rand() % 100;
		if (nIdx <= m_monsterType->n_hit_tri) return m_monsterType->s_hit_sound;
	}
	else if (nSountTpy == SOUND_TPY_DEAD) {
		if (m_monsterType->s_dead_sound.empty()) {
			return "";
		}
		int nIdx = std::rand() % 100;
		if (nIdx <= m_monsterType->n_dead_tri) return m_monsterType->s_dead_sound;
	}
	return "";
}




//////////////////////////////////////////////////////////////////////////
MonsterDataMgr& MonsterDataMgr::getInstance()
{
	static MonsterDataMgr mgr;
	return mgr;
}

std::shared_ptr<Monster> MonsterDataMgr::getMonster(bzbee::int32 mapObjId)
{
	auto iter = m_monsters.find(mapObjId);
	return (iter == m_monsters.end()) ? nullptr : iter->second;
}

std::shared_ptr<Monster> MonsterDataMgr::getMonsterByTypeId(int mobTypeId)
{
	for (auto&& pir : m_monsters)
	{
		auto&& pMobType = pir.second->getMonsterType();

		if (pMobType && pMobType->n_ID == mobTypeId)
		{
			return pir.second;
		}
	}

	return nullptr;
}

bool MonsterDataMgr::addMonster(const std::shared_ptr<Monster>& monster)
{
	if (!monster) { return false; }
	if (getMonster(monster->getId()))
	{
		LOGE("duplicate map obj id: {0}, override.", monster->getId());
	}
	m_monsters[monster->getId()] = monster;
	return MapObjMgr::getInstance().addObj(monster);;
}

bool MonsterDataMgr::removeMonster(bzbee::int32 mapObjId)
{
	return m_monsters.erase(mapObjId) == 1 && MapObjMgr::getInstance().removeObj(mapObjId);
}

bool MonsterDataMgr::isHaveCreate(NS::I64 serverMapObjId)
{
	return false;
}
