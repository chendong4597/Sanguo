/*!
 * \file Role.cpp
 *
 * \author zhijieli
 * \date 2016/12/05
 *
 * 
 */
#include "stdafx.h"
#include "Role.h"
//#include "../../config/MapCfg.h"
#include "../../config/ArmorCfg.h"
#include "../../config/GlobalCfg.h"
#include "../../config/BuffCfg.h"
#include "../../config/MonsterCfg.h"
#include "../ItemData.h"
#include "msg/MsgGameDef.pb.h"
#include "RandomUtil.h"
#include "../../config/PlayerAttrCfg.h"
#include <msg/MsgItem.pb.h>
#include "../../config/PlayerAttrNameCfg.h"
#include "Player.h"
#include "MacroUtility.h"
#include "Framework.h"

using namespace NS;
using namespace MsgPB;


// 属性叠加
static void sumTo(MsgPB::ROLE_EFFECT_TYPE eType, float nBase, float nAddValue, float& nSum)
{
	if (eType >= MsgPB::ROLE_EFFECT_HPM && eType < MsgPB::ROLE_EFFECT_ATTR_STATIC_MAX) {
		nSum += nAddValue;
	}
	else if (eType >= MsgPB::ROLE_EFFECT_HPM_RATE && eType < MsgPB::ROLE_EFFECT_ATTR_RATE_MAX) {
		nSum += (nBase * nAddValue / 1000);
	}
};

// 计算千分比.
static void sumRateTo(MsgPB::ROLE_EFFECT_TYPE eType, float nNow, float nRateValue, float& nSum)
{
	nSum += (nNow * nRateValue / 1000);
};

// 属性计算.
static float calculateAddValue(ATTR_REF eAttrRef, float nRefTarget, float nRefValue = 0)
{
	return (eAttrRef == ATTR_REF_STATIC) ? nRefValue : nRefTarget * nRefValue / 1000;
};

// 获取属性值.
static float getEffectTypeValue(MsgPB::ROLE_EFFECT_TYPE eType, const AttrMap& attrs)
{
	auto findId = attrs.find((eType > 100) ? static_cast<MsgPB::ROLE_EFFECT_TYPE>(eType % 100) : eType);
	return (findId == attrs.end()) ? 0.0f : findId->second;
};

// 添加属性值.
static float& addEffectTypeRefValue(MsgPB::ROLE_EFFECT_TYPE eType, AttrMap& attrs)
{
	auto relType = (eType > 100) ? static_cast<MsgPB::ROLE_EFFECT_TYPE>(eType % 100) : eType;
	return attrs[relType];
};

// 合计属性
void recalculateByEffectSum(AttrMap& attrsReal, const AttrMap& attrsBase, const CEffectSum& effectSum)
{
	// 基本属性。
	attrsReal = attrsBase;
	// 收集与当前属性计算Map.	
	AttrMap collRateAttrs;
	float addValue{ 0 };

	const CEffectSum::MAP_EFFECT& mapEffects = effectSum.GetEffects();
	for (auto&& effect : mapEffects) {
		const CEffectSum::MAP_ATTRREF& mapAttrRefs = effect.second;
		for (auto&& attrRef : mapAttrRefs) {
			MsgPB::ROLE_EFFECT_TYPE eType = effect.first;
			ATTR_REF eAttrRef = attrRef.first;
			// 1. 基于基础 属性计算
			if ((eAttrRef >= ATTR_REF_TARGET_HPM_BASE && eAttrRef < ATTR_REF_TARGET_BASE_MAX) || eAttrRef == ATTR_REF_STATIC) {
				addValue = calculateAddValue(eAttrRef, getEffectTypeValue(eType, const_cast<AttrMap&>(attrsBase)), static_cast<float>(attrRef.second));
				// 服务器下发的 附加属性多 *1000;
				if (eType == MsgPB::ROLE_EFFECT_ATK_RANGE ||
					eType == MsgPB::ROLE_EFFECT_GATHER_SP ||
					eType == MsgPB::ROLE_EFFECT_ATK_SP)
				{
					addValue = addValue / 1000;
				}
				// 服务器下发的 附加属性多 *100;
				if (eType == MsgPB::ROLE_EFFECT_MV_SP ||
					eType == MsgPB::ROLE_EFFECT_CHASE)
				{
					addValue = addValue / 100;
				}
				// Sum 属性.
				if (MsgPB::ROLE_EFFECT_MR_ALL == eType) {
					for (int i = MsgPB::ROLE_EFFECT_MR_FIRE; i <= MsgPB::ROLE_EFFECT_MR_DARK; ++i) {
						MsgPB::ROLE_EFFECT_TYPE t = (MsgPB::ROLE_EFFECT_TYPE)i;
						sumTo(t, getEffectTypeValue(t, attrsBase), addValue, addEffectTypeRefValue(t, attrsReal));
					}
				}
				else if (MsgPB::ROLE_EFFECT_MR_ALL_RATE == eType) {
					for (int i = MsgPB::ROLE_EFFECT_MR_FIRE_RATE; i <= MsgPB::ROLE_EFFECT_MR_DARK_RATE; ++i) {
						MsgPB::ROLE_EFFECT_TYPE t = (MsgPB::ROLE_EFFECT_TYPE)i;
						sumTo(t, getEffectTypeValue(t, attrsBase), addValue, addEffectTypeRefValue(t, attrsReal));
					}
				}
				else {
					sumTo(eType, getEffectTypeValue(eType, attrsBase), addValue, addEffectTypeRefValue(eType, attrsReal));
				}
			} // 收集 基于自身当前的属性计算
			else if (eAttrRef >= ATTR_REF_TARGET_HPM_NOW && eAttrRef < ATTR_REF_TARGET_NOW_MAX)
			{
				collRateAttrs[eType] += attrRef.second;
			}
			// 技能狀態
			//if (eType == ROLE_EFFECT_SKILL_STATUS && eAttrRef == ATTR_REF_STATIC)
			//	uSkillStatus |= attrRef.second;

		} // End for
	} // End for

	// 2. 基于自身当前的属性计算
	for (auto&& rateAttr : collRateAttrs)
	{
		auto&& eType = rateAttr.first;
		sumRateTo(eType, getEffectTypeValue(eType, attrsReal), rateAttr.second, addEffectTypeRefValue(eType, attrsReal));
	}
};


////////////////////////////////////////////////////////////////
Buff::Buff(const BuffType* buffType) :
	m_buffType{ buffType }
{
}

unsigned Buff::getRemainMs() const
{
	using namespace std::chrono;
	steady_clock::time_point now{ steady_clock::now() };
	if (m_pauseTime == steady_clock::time_point{})
	{
		if (now >= m_endTime) { return 0; }
		else
		{
			return static_cast<unsigned>(duration_cast<milliseconds>(m_endTime - now).count());
		}
	}
	else
	{
		//buff暂停
		return static_cast<unsigned>(duration_cast<milliseconds>(m_endTime - m_pauseTime).count());
	}
}

void Buff::setAliveTime(int ms)
{
	m_endTime = std::chrono::steady_clock::now() + std::chrono::milliseconds{ ms };
}

void Buff::pause()
{
	if (m_pauseTime == std::chrono::steady_clock::time_point{})
	{
		m_pauseTime = std::chrono::steady_clock::now();
	}
}

bool Buff::isPause() const
{
	return m_pauseTime != std::chrono::steady_clock::time_point{};
}

bool Buff::isDisplay() const
{
	if (!m_buffType)
	{
		return false;
	}

	return isPause() ? m_buffType->n_lock_buff_display : m_buffType->n_buff_display;
}

void Buff::restore()
{
	using namespace std::chrono;
	steady_clock::time_point now{ std::chrono::steady_clock::now() };
	//结束时间后推
	m_endTime += now - m_pauseTime;
	//重置暂停时间
	m_pauseTime = steady_clock::time_point{};
}

////////////////////////////////////////////////////////////////

CharacterAttributeSnapshot::CharacterAttributeSnapshot(const std::shared_ptr<Role>& pRole)
{
	m_mapAttr.clear();
	m_mapBase.clear();

	if (!pRole) { return; }

	// 当前属性快照
	AttrMap mapAttr = pRole->getAttrMap();
	m_mapAttr.insert(mapAttr.begin(), mapAttr.end());
	// 基础属性
	for (auto&& attrVal : m_mapAttr)
	{
		m_mapBase[attrVal.first] = pRole->getBaseAttr(attrVal.first);
	}
}

CharacterAttributeSnapshot::~CharacterAttributeSnapshot()
{
	m_mapAttr.clear();
	m_mapBase.clear();
}

float CharacterAttributeSnapshot::getBaseAttrByRef(ATTR_REF ref) const
{
	auto it = m_mapBase.find(static_cast<MsgPB::ROLE_EFFECT_TYPE>(ref - 100));
	return it == m_mapBase.end() ? 0.f : it->second / 1000.f;
}

float CharacterAttributeSnapshot::getCurrentAttrByRef(ATTR_REF ref) const
{
	auto it = m_mapAttr.find(static_cast<MsgPB::ROLE_EFFECT_TYPE>(ref - 200));
	return it == m_mapAttr.end() ? 0.f : it->second / 1000.f;
}

////////////////////////////////////////////////////////////////
int Role::getTotalPower()
{
	return m_totalPower;
}

float Role::getAttr(MsgPB::ROLE_EFFECT_TYPE type) const
{
	auto it = m_attrs.find(type);

	return it == m_attrs.end() ? 0.f : it->second;
}

float Role::getBaseAttrByRef(ATTR_REF ref) const
{
	return getBaseAttr(static_cast<MsgPB::ROLE_EFFECT_TYPE>(ref - 100)) / 1000.f;
}

float Role::getCurrentAttrByRef(ATTR_REF ref) const
{
	auto it = m_attrs.find(static_cast<MsgPB::ROLE_EFFECT_TYPE>(ref - 200));
	return it == m_attrs.end() ? 0.f : it->second / 1000.f;
}

float Role::getMoveAniTimeScale(bool isRunning) const
{
	float timeScale{ 1.f };
	auto&& baseAttr = getBaseAttr(isRunning ? MsgPB::ROLE_EFFECT_CHASE : MsgPB::ROLE_EFFECT_MV_SP, false);
	auto&& currentAttr = getAttr(isRunning ? MsgPB::ROLE_EFFECT_CHASE : MsgPB::ROLE_EFFECT_MV_SP);
	if (baseAttr > 0.f)
	{
		timeScale = currentAttr / baseAttr;
	}

	return timeScale;
}

void Role::onInit()
{
	m_PassiveSkillMgr = new PassiveSkillMgr(std::shared_ptr<Role>(this));
}

Role::~Role()
{
	
}

void Role::update(float dt)
{
	if (m_PassiveSkillMgr) {
		m_PassiveSkillMgr->update(dt);
	}
}

void Role::clearInit()
{
	m_initialized = false;  //这里delete会闪退，但是不delete，会内存泄露，后期查下原因
	//SAFE_DELETE(m_PassiveSkillMgr);
}

bool Role::hasAttr(MsgPB::ROLE_EFFECT_TYPE type) const
{
	return m_attrs.end() != m_attrs.find(type);
}

void Role::setPos(const glm::vec3& pos)
{
	MapObj::setPos(pos);

}

void Role::setOrientation(const glm::vec3& o)
{
	m_orientation = o;
}

//const std::tuple<glm::vec2, glm::vec2> Role::getAABB() const
//{
//	const glm::vec3& pos{ getPos() };
//	float halfWidth{ getWidth() / 2 };
//	float height{ getHeight() };
//	return std::make_tuple(glm::vec2{ pos.x - halfWidth, pos.y + height - MapCfg::HALF_TILE_SIZE }, glm::vec2{ pos.x + halfWidth, pos.y - MapCfg::HALF_TILE_SIZE });
//}

const std::tuple<bool, bzbee::tstring> Role::canRevive(bool freeRecurrection) const
{
	using namespace std::chrono;
	if (freeRecurrection && duration_cast<seconds>(steady_clock::now() - m_deadTime).count() < 10000)//GlobalCfg::getInstance().getPlayerReviveCfg().delaySecs)
	{
		return std::make_tuple(false, _LC("REVIVE_IN_CD"));
	}

	return std::make_tuple(true, "");
}

bool Role::isGroupMatch(NS::int32 groupId)
{
	auto&& selfGroupId = getGroupId();
	return groupId == selfGroupId || groupId == static_cast<NS::int32>(Role::GroupType::Npc) || selfGroupId == static_cast<NS::int32>(Role::GroupType::Npc);
}

void Role::addBuff(const Buff& buff)
{
	m_lastVecBuff = m_vecBuff;
	m_vecBuff.emplace_back(buff);

	addReduceNegativeEffectOnBuffBegin(buff); // 记录 降低负面效应影响
}

Buff* Role::findBuffByIdx(int idx)
{
	for (auto&& buff : m_vecBuff)
	{
		if (buff.getIdx() == idx)
		{
			return &buff;
		}
	}

	return {};
}

bool Role::IsHasBuffEffectId(int nBuffEffId)
{
	for (auto&& buff : m_vecBuff)
	{
		if (buff.getType() && buff.getType()->n_BuffEffID == nBuffEffId) {
			return true;
		}
	}
	return false;
}

void Role::removeBuff(int idx)
{
	m_lastVecBuff = m_vecBuff;
	for (auto it = m_vecBuff.begin(); it != m_vecBuff.end(); ++it)
	{
		if (it->getIdx() == idx)
		{
			removeReduceNegativeEffectOnBuffEnd(*it);	// 移除 降低负面效应影响
			m_vecBuff.erase(it);
			return;
		}
	}
}

void Role::clearBuff()
{
	for (auto it = m_vecBuff.begin(); it != m_vecBuff.end(); ++it)
	{
		removeReduceNegativeEffectOnBuffEnd(*it);	// 移除 降低负面效应影响
	}
	m_vecBuff.clear();
	m_lastVecBuff.clear();
}


void Role::addReduceNegativeEffectOnBuffBegin(const Buff& buff)
{
	if (auto&& buffType = buff.getType())
	{
		for (const auto& effect : buffType->effects)
		{
			if (static_cast<int>(effect.type) != static_cast<int>(BUFF_EFFECT_REDUCE_NEGATIVE_EFFECT_BY_RATE) || effect.ref_values.empty())
				continue;

			for (const auto& v : effect.ref_values)
			{
				if (v.attrref == ATTR_REF_NULL)
					continue;

				addReduceNegativeEffectByRate(buffType->n_ID, v.attrref, v.value);
			}
		}
	}
}

void Role::removeReduceNegativeEffectOnBuffEnd(const Buff& buff)
{
	if (auto&& buffType = buff.getType())
	{
		removeReduceNegativeEffectByRate(buffType->n_ID);
	}
}

void Role::addReduceNegativeEffectByRate(int32 buffDataID, int32 effect, int32 rate)
{
	auto& rates = m_reduceNegativeEffectByRateBuffs[buffDataID];
	rates.emplace(effect, rate);
}

void Role::removeReduceNegativeEffectByRate(int32 buffDataID)
{
	m_reduceNegativeEffectByRateBuffs.erase(buffDataID);
}

int Role::getReduceNegativeEffectRate(int32 effectType)
{
	int rate = 0;
	for (const auto& rs : m_reduceNegativeEffectByRateBuffs)
	{
		for (const auto& r : rs.second)
		{
			if (r.first == effectType)
			{
				rate += r.second;
			}
		}
	}
	return (std::min)(rate, 1000);
}

Buff* Role::findBuffById(int buffId)
{
	for (auto&& buff : m_vecBuff)
	{
		if (buff.getType() && buff.getType()->n_ID == buffId)
		{
			return &buff;
		}
	}

	return nullptr;
}

bool Role::isInvincible(Category nCategoryAttacker)
{
	if ((getCtrlState() & MsgPB::SKILL_STATUS_CANT_BE_ATTACK))
	{
		return true;
	}	
	if ( (nCategoryAttacker != Category::Player) && (getCtrlState() & MsgPB::SKILL_STATUS_CANT_BE_ATTACK_AI) )
	{
		return true;
	}
	return false;
}

unsigned Role::getCtrlState() const
{
	unsigned state{};
	auto&& buffs = getBuffs();
	for (auto&& buff : buffs)
	{
		const std::vector<int>& vecUnActiveEffectIndex = buff.getUnActiveEffectIndex();
		for (size_t i = 0; i < buff.getType()->effects.size(); ++i)
		{
			// 抵抗的控制效果，不加入位
			const auto& foundIter = std::find(vecUnActiveEffectIndex.begin(), vecUnActiveEffectIndex.end(), i);
			if (foundIter != vecUnActiveEffectIndex.end()) { continue; }

			const Effect& effect = buff.getType()->effects[i];
			if (effect.type == MsgPB::ROLE_EFFECT_SKILL_STATUS)
			{
				state |= effect.ref_values[0].value;
			}
		}
	}

	return state;
}

bool Role::setCurrSelectPos(int pos)
{
	switch ((MsgPB::EQUIP_PACKET_POS)pos)
	{
	case MsgPB::EQUIP_PACKET_COS_WEAPON:
	case MsgPB::EQUIP_PACKET_WEAPON:
	case MsgPB::EQUIP_PACKET_HORSE:
	{
		//if (m_mapType == MapDataMgr::MapType::PVP)
		//{
		//	m_currSelectPosAtPvP = pos;
		//}
		//else
		//{
		//	m_currSelectPos = pos;
		//}
		m_currSelectPos = pos;
		return true;
	}
	default:
		return false;
	}
	return false;
}

void Role::setBuffs(const BuffVec& buff)
{
	m_lastVecBuff = m_vecBuff;
	m_vecBuff = buff;
	calcAttr();
}

void Role::sumEffectBuf(CEffectSum & effectSum, bool bCompare)
{
	float atkSpeed{};
	m_buffAtkSpeed = 0.f;
	auto&& vecBuff = bCompare ? m_lastVecBuff : m_vecBuff;
	for (const auto& buff : vecBuff)
	{
		auto&& buffType = buff.getType();
		if (buff.isPause())
		{
			continue;
		}
		for (const auto& effect : buffType->effects)
		{
			if (effect.type == ROLE_EFFECT_NULL || effect.type >= ROLE_EFFECT_ATTR_MAX)
				continue;

			// BUFF叠加
			std::vector<AttrEffectValue> mutRef_values(effect.ref_values);
			for (auto& attrVal : mutRef_values)
			{
				if (attrVal.attrref == ATTR_REF_NULL)
					continue;

				attrVal.value *= buff.getHeapCounter();

				if (effect.type == MsgPB::ROLE_EFFECT_ATK_SP)
				{
					if (attrVal.attrref == ATTR_REF_STATIC)
						atkSpeed = static_cast<float>(attrVal.value);
					else if (attrVal.attrref >= ATTR_REF_TARGET_HPM_BASE && attrVal.attrref < ATTR_REF_TARGET_BASE_MAX)
						atkSpeed = getBaseAttrByRef(attrVal.attrref) * attrVal.value;
					else if (attrVal.attrref >= ATTR_REF_TARGET_HPM_NOW && attrVal.attrref < ATTR_REF_TARGET_NOW_MAX)
						atkSpeed = getCurrentAttrByRef(attrVal.attrref) * attrVal.value;

					if (!bCompare)
					{
						m_buffAtkSpeed += atkSpeed / 1000.f;
					}
				}
			}

			int32 reduceNegativeEffectRate = getReduceNegativeEffectRate(effect.type);
			effectSum.AddEffect(effect.type, mutRef_values, 0, reduceNegativeEffectRate);
			effectSum.AddEffectByCaster(effect.type, mutRef_values, 0, buff.getCasterAttr(),reduceNegativeEffectRate);
		}
	}
}

const std::shared_ptr<ItemData> Role::getEquipByPos(MsgPB::EQUIP_PACKET_POS ePos) const
{
	//if (m_mapType == MapDataMgr::MapType::PVP)
	//{
	//	EquipMap::const_iterator it = m_pvpEquipMap.find(ePos);
	//	return it == m_pvpEquipMap.end() ? nullptr : it->second;
	//}
	//else
	//{
	//	EquipMap::const_iterator it = m_equipMap.find(ePos);
	//	if (it == m_equipMap.end())
	//		return nullptr;
	//	else
	//		return  it->second;
	//}
	EquipMap::const_iterator it = m_equipMap.find(ePos);
	if (it == m_equipMap.end())
		return nullptr;
	else
		return  it->second;
	return nullptr;
}

const std::shared_ptr<ItemData> Role::getEquipIncludingHideByPos(MsgPB::EQUIP_PACKET_POS ePos) const
{
	EquipMap::const_iterator it = m_equipMap.find(ePos);
	if (it == m_equipMap.end())
		return nullptr;
	else
		return  it->second;
	//if (m_mapType == MapDataMgr::MapType::PVP)
	//{
	//	EquipMap::const_iterator it = m_pvpEquipMap.find(ePos);
	//	return it == m_pvpEquipMap.end() ? nullptr : it->second;
	//}
	//else
	//{
	//	EquipMap::const_iterator it = m_equipMap.find(ePos);
	//	if (it == m_equipMap.end())
	//		return nullptr;
	//	else
	//		return  it->second;
	//}
	//return nullptr;
}

const std::shared_ptr<ItemData> Role::getEquipmentByItemUuid(NS::I64 nItemUuid)
{
	Role::EquipMap& mapEquipments = m_equipMap;
	//if (m_mapType == MapDataMgr::MapType::PVP)
	//{
	//	mapEquipments = m_pvpEquipMap;
	//}

	for (auto iter = mapEquipments.begin(); iter != mapEquipments.end(); ++iter)
	{
		if (iter->second && iter->second->getUuid() == nItemUuid)
		{
			return iter->second;
		}
	}
	return nullptr;
}

const std::shared_ptr<ItemData> Role::getCurrFarmTool() const
{
	return getEquipByPos(MsgPB::EQUIP_PACKET_HORSE);
}

const std::shared_ptr<ItemData> Role::getCurrCosWeapon() const
{
	return getEquipByPos(MsgPB::EQUIP_PACKET_COS_WEAPON);
}

const std::shared_ptr<ItemData> Role::getCurrNorWeapon() const
{
	return getEquipByPos(MsgPB::EQUIP_PACKET_WEAPON);
}

int Role::getCurrWeaponPos()
{
	auto cosWeapon = getCurrCosWeapon();
	if (cosWeapon != nullptr)
	{
		return MsgPB::EQUIP_PACKET_COS_WEAPON;
	}
	auto weapon = getCurrNorWeapon();
	if (weapon != nullptr)
	{
		return MsgPB::EQUIP_PACKET_WEAPON;
	}
	return -1;
}

const std::shared_ptr<ItemData> Role::getCurrEftWeapon() const
{
	//auto cosWeapon = getCurrCosWeapon();
	//auto norWeapon = getCurrNorWeapon();
	//if (norWeapon == nullptr)
	//	return nullptr;
	//if ((cosWeapon && !cosWeapon->getArmorCfg()) || (norWeapon && !norWeapon->getArmorCfg()))
	//	return nullptr;
	//if (cosWeapon && norWeapon && ArmorCfg::getInstance().isSameTypeGroup(cosWeapon->getArmorCfg()->m_equipSubType, norWeapon->getArmorCfg()->m_equipSubType))
	//	return cosWeapon;
	//if (norWeapon) return norWeapon;
	//if (cosWeapon) return cosWeapon;
	return nullptr;
}

//玩家特定部位不能裸装，所以在裸装时，上衣和裤子要穿上特定的装备;
void Role::getEquipMap(const EquipMap &equipMapIn, EquipMap &equipMapOut)
{
	//equipMapOut = equipMapIn;
	//if (!isEquip(equipMapOut, MsgPB::EQUIP_PACKET_CLOTHES, MsgPB::EQUIP_PACKET_COS_CLOTHES))
	//{
	//	if (!m_clothesEquipItem || !m_clothesEquipItem.get())
	//	{
	//		m_clothesEquipItem = std::make_shared<ItemData>();
	//		m_clothesEquipItem->initWithItemId(GlobalCfg::getInstance().getNewEquipClosthId());
	//	}
	//	equipMapOut[MsgPB::EQUIP_PACKET_CLOTHES] = m_clothesEquipItem;
	//}
	//if (!isEquip(equipMapOut, MsgPB::EQUIP_PACKET_PANTS, MsgPB::EQUIP_PACKET_COS_PANTS))
	//{
	//	if (!m_PantsEquipItem || !m_PantsEquipItem.get())
	//	{
	//		m_PantsEquipItem = std::make_shared<ItemData>();
	//		m_PantsEquipItem->initWithItemId(GlobalCfg::getInstance().getNewEquipPantsId());
	//	}
	//	equipMapOut[MsgPB::EQUIP_PACKET_PANTS] = m_PantsEquipItem;
	//}

	//ArmorCfg::getInstance().findHideEquipAndNode(equipMapOut, m_vHideEquip, m_vHideNode, m_vFemaleHideNode, equipMapOut);
	//for (auto it : m_vHideEquip)
	//{
	//	equipMapOut[it] = nullptr;
	//}
}

const Role::EquipMap& Role::getEquipMap() const
{ 
	return m_equipMap;
	//if (m_mapType == MapDataMgr::MapType::PVP)
	//	return m_pvpEquipMap;
	//else
	//	return m_equipMap;
}

void Role::setEquipMap(const EquipMap& equipMap)
{
	m_lastEquipMap = equipMap;
	m_equipMap = equipMap;
	refreshEquip(m_equipMap);
	//if (m_mapType == MapDataMgr::MapType::PVP)
	//{
	//	m_pvpEquipMap = equipMap;
	//	int curSelWeaponPos = getCurrWeaponPos();
	//	if (!setCurrSelectPos(curSelWeaponPos))
	//		m_currSelectPosAtPvP = -1;
	//	refreshEquip(m_pvpEquipMap);
	//	return;
	//}
	//else
	//{
	//	//calChangeEquipAttr(equipMap);
	//	m_lastEquipMap = equipMap;
	//	m_equipMap = equipMap;
	//	refreshEquip(m_equipMap);
	//}
}

void Role::setEquipMap(int pos, std::shared_ptr<ItemData> itemData)
{
	//if (m_mapType == MapDataMgr::MapType::PVP)
	//{
	//	auto iterMap = m_pvpEquipMap.find(pos);
	//	if (iterMap != m_pvpEquipMap.end()) 
	//		iterMap->second = itemData;
	//	else
	//		m_pvpEquipMap[pos] = itemData;
	//	int curSelWeaponPos = getCurrWeaponPos();
	//	if (!setCurrSelectPos(curSelWeaponPos))
	//		m_currSelectPosAtPvP = -1;
	//	refreshEquip(m_pvpEquipMap);
	//	return;
	//}

	m_lastEquipMap[pos] = m_equipMap[pos];
	m_equipMap[pos] = itemData;

	refreshEquip(m_equipMap);
}

bool Role::equipPvPWeapon(int itemId)
{
	//if (itemId == -1 || itemId == 0)
	//{
	//	m_currSelectPosAtPvP = MsgPB::EQUIP_PACKET_POS::EQUIP_PACKET_WEAPON;
	//	auto iterMap = m_pvpEquipMap.find(MsgPB::EQUIP_PACKET_POS::EQUIP_PACKET_WEAPON);
	//	if (iterMap != m_pvpEquipMap.end())
	//		m_pvpEquipMap.erase(iterMap);
	//	refreshEquip(m_pvpEquipMap);
	//	return true;
	//}
	//
	//int pos = ItemPacketMgr::getInstance().getEquipPosById(itemId);
	//bool isWeapon = ItemPacketMgr::getInstance().isWeaponByTempId(itemId);
	//if (isWeapon)
	//{
	//	setCurrSelectPos(pos);
	//}

	//if (!isWeapon && !isCanEquipArm(itemId)) return false;

	//std::shared_ptr<ItemData> pItemData = std::make_shared<ItemData>();
	//pItemData->setUuid(0);
	//pItemData->initWithItemId(itemId);
	//pItemData->setPos(pos);
	//pItemData->setDurable(100);
	//pItemData->setDurableMax(100);
	//setEquipMap(pos, pItemData);

	return true;
}

bool Role::isCanEquipArm(int itemId)
{
	return true;
}

int Role::getCurrSelectPos() const
{
	//if (m_mapType == MapDataMgr::MapType::PVP)
	//{
	//	return m_currSelectPosAtPvP;
	//}
	return m_currSelectPos;
}

//bool Role::isEquip(NS::I64 id, const EquipMap &equipMap)
//{
//	for (auto iteMap = equipMap.begin(); iteMap != equipMap.end(); iteMap++)
//	{
//		if (iteMap->second->getUuid() == id)
//		{
//			return true;
//		}
//	}
//	return false;
//}


bool Role::switchEquipment(NS::int32 pos, std::shared_ptr<ItemData> pNewEquipment, std::shared_ptr<ItemData>& pOldEquipment)
{
	//if (m_mapType == MapDataMgr::MapType::PVP)
	//{
	//	auto iterMap = m_pvpEquipMap.find(pos);
	//	if (iterMap != m_pvpEquipMap.end())
	//	{
	//		iterMap->second = pNewEquipment;
	//	}
	//	else
	//	{
	//		pOldEquipment = m_pvpEquipMap[pos];
	//		m_pvpEquipMap[pos] = pNewEquipment;
	//	}
	//	int curSelWeaponPos = getCurrWeaponPos();
	//	if (!setCurrSelectPos(curSelWeaponPos))
	//	{
	//		m_currSelectPosAtPvP = -1;
	//	}
	//	refreshEquip(m_pvpEquipMap);
	//	return true;
	//}

	pOldEquipment = m_equipMap[pos];
	m_lastEquipMap[pos] = pOldEquipment;
	m_equipMap[pos] = pNewEquipment;

	refreshEquip(m_equipMap);

	return true;
}

bool Role::removeEquipmentByItemUuid(NS::I64 itemUuid, std::shared_ptr<ItemData>& pRemoveEquipment)
{
	//if (m_mapType == MapDataMgr::MapType::PVP)
	//{
	//	// PVP 模式目前未开放，先不处理
	//	// m_pvpEquipMap
	//	return false;
	//}
	auto&& removeIter = m_equipMap.end();
	for (auto&& iter = m_equipMap.begin(); iter != m_equipMap.end(); ++iter)
	{
		if (iter->second->getUuid() == itemUuid)
		{
			removeIter = iter;
			break;
		}
	}
	if (removeIter != m_equipMap.end())
	{
		pRemoveEquipment = removeIter->second;
		m_lastEquipMap[removeIter->first] = pRemoveEquipment;
		m_equipMap.erase(removeIter);
		refreshEquip(m_equipMap);
		return true;
	}

	return false;
}

bool Role::removeEquipmentByPosIndex(NS::int32 pos, std::shared_ptr<ItemData>& pRemoveEquipment)
{
	//if (m_mapType == MapDataMgr::MapType::PVP)
	//{
	//	// PVP 模式目前未开放，先不处理
	//	// m_pvpEquipMap
	//	return false;
	//}

	auto&& removeIter = m_equipMap.find(pos);
	if (removeIter != m_equipMap.end())
	{
		pRemoveEquipment = removeIter->second;
		m_lastEquipMap[removeIter->first] = pRemoveEquipment;
		m_equipMap.erase(removeIter);
		refreshEquip(m_equipMap);
		return true;
	}

	return false;
}

bool Role::clearAllEquipment()
{
	//if (m_mapType == MapDataMgr::MapType::PVP)
	//{
	//	// PVP 模式目前未开放，先不处理
	//	//m_pvpEquipMap.clear();
	//	return false;
	//}

	m_equipMap.clear();
	refreshEquip(m_equipMap);

	return true;
}

void Role::getLastBuffCompare(ChangeAttrMap& chgMap)
{
	// 旧Buff
	CEffectSum oldEff;
	sumEffectBuf(oldEff, true);

	// 新Buff
	CEffectSum newEff;
	sumEffectBuf(newEff, false);

	// 计算 Diff
	auto&& oldEffMap = oldEff.GetEffects();
	auto&& newEffMap = newEff.GetEffects();
	for (int i = MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_HPM; i <= MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MR_ALL; ++i)
	{
		float oldValue{ 0 };
		float newValue{ 0 };

		auto oldIterAttr = oldEffMap.find((MsgPB::ROLE_EFFECT_TYPE)i);
		auto newIterAttr = newEffMap.find((MsgPB::ROLE_EFFECT_TYPE)i);

		if (oldIterAttr != oldEffMap.end())
		{
			for (auto&& attr : oldIterAttr->second)
			{
				if (attr.first == ATTR_REF::ATTR_REF_STATIC)
				{
					oldValue += attr.second;
				}
			}
		}
		
		if (newIterAttr != newEffMap.end())
		{
			for (auto&& attr : newIterAttr->second)
			{
				if (attr.first == ATTR_REF::ATTR_REF_STATIC)
				{
					newValue += attr.second;
				}
			}
		}

		float diffValue = newValue - oldValue;
		auto&& pAttrName = PlayerAttrNameCfg::getInstance().findPlayerAttrNameType((MsgPB::ROLE_EFFECT_TYPE)i);
		if (pAttrName && diffValue != 0)
		{
			if (pAttrName->n_Class != 1)
				diffValue = diffValue / 1000 * 100;

			chgMap[pAttrName->m_Name] += diffValue;
		}
	}
}

void Role::getLastEquipCompare(ChangeAttrMap& chgMap)
{
	//// 旧装备表
	//AttrMap oldAttrs;
	//for (auto iteMap = m_lastEquipMap.begin(); iteMap != m_lastEquipMap.end(); iteMap++)
	//{
	//	if (iteMap->second)
	//	{
	//		iteMap->second->getArmorAttrValue(oldAttrs);
	//	}
	//}
	//	

	//// 新装备表
	//AttrMap newAttrs;
	//for (auto iteMap = m_equipMap.begin(); iteMap != m_equipMap.end(); iteMap++)
	//{
	//	if (iteMap->second)
	//	{
	//		iteMap->second->getArmorAttrValue(newAttrs);
	//	}
	//	// 临时做法，属性变更后计算，将m_lastEquipMap和m_equipMap数据同步下。
	//	m_lastEquipMap[iteMap->first] = m_equipMap[iteMap->first];
	//}

	//// 计算 Diff
	//for (int i = MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_HPM; i <= MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MR_ALL; ++i)
	//{
	//	float oldValue{ 0 };
	//	float newValue{ 0 };

	//	auto oldIterAttr = oldAttrs.find((MsgPB::ROLE_EFFECT_TYPE)i);
	//	auto newIterAttr = newAttrs.find((MsgPB::ROLE_EFFECT_TYPE)i);

	//	if (oldIterAttr != oldAttrs.end())
	//		oldValue = oldIterAttr->second;
	//	if (newIterAttr != newAttrs.end())
	//		newValue = newIterAttr->second;

	//	float diffValue = newValue - oldValue;
	//	auto&& pAttrName = PlayerAttrNameCfg::getInstance().findPlayerAttrNameType((MsgPB::ROLE_EFFECT_TYPE)i);
	//	if (pAttrName && diffValue != 0)
	//	{
	//		if (pAttrName->n_Class != 1)
	//			diffValue = diffValue / 1000 * 100;

	//		chgMap[pAttrName->m_Name] += diffValue;
	//	}
	//}
}

//void Role::calChangeEquipAttr(const std::map<int, std::shared_ptr<ItemData>>& equipMap)
//{
//	if (!m_isCalChangeAttr) 
//		return;
//
//	m_changeEuipAttrs.clear();
//	if (PlayerMgr::getInstance().getSelf().get() != this)
//		return;
//
//	AttrMap oldAttrs;
//	oldAttrs.clear();
//	for (auto iteMap = m_equipMap.begin(); iteMap != m_equipMap.end(); iteMap++)
//	{
//		iteMap->second->getArmorAttrValue(oldAttrs);
//	}
//	
//	AttrMap newAttrs;
//	newAttrs.clear();
//	for (auto iteMap = equipMap.begin(); iteMap != equipMap.end(); iteMap++)
//	{
//		iteMap->second->getArmorAttrValue(newAttrs);
//	}
//
//	if ((int)equipMap.size() >= (int)m_equipMap.size())
//	{
//		for (auto iteMap = equipMap.begin(); iteMap != equipMap.end(); iteMap++)
//		{
//			if (!isEquip(iteMap->second->getUuid(),m_equipMap))
//			{
//				if (iteMap->second->getItemCfg())
//				{
//					m_newEuipName = iteMap->second->getItemCfg()->m_name;
//				}
//				break;
//			}
//		}
//		m_isEquipItem = true;
//	}
//	else
//	{
//		for (auto iteMap = m_equipMap.begin(); iteMap != m_equipMap.end(); iteMap++)
//		{
//			if (!isEquip(iteMap->second->getUuid(), equipMap))
//			{
//				if (iteMap->second->getItemCfg())
//				{
//					m_newEuipName = iteMap->second->getItemCfg()->m_name;
//				}
//				break;
//			}
//		}
//		m_isEquipItem = false;
//	}
//
//	for (int i = MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_HPM; i <= MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_MR_ALL; ++i)
//	{
//		float oldAttrValue = 0;
//		auto oldIterAttr = oldAttrs.find((MsgPB::ROLE_EFFECT_TYPE)i);
//		if (oldIterAttr != oldAttrs.end())
//		{
//			oldAttrValue = oldIterAttr->second;
//		}
//
//		float newaAttrValue = 0;
//		auto newIterAttr = newAttrs.find((MsgPB::ROLE_EFFECT_TYPE)i);
//		if (newIterAttr != newAttrs.end())
//		{
//			newaAttrValue = newIterAttr->second;
//		}
//
//		float diffValue = newaAttrValue - oldAttrValue;
//		const PlayerAttrNameCfg::PlayerAttrNameType* pAttrNameType = PlayerAttrNameCfg::getInstance().findPlayerAttrNameType((MsgPB::ROLE_EFFECT_TYPE)i);
//		if (!pAttrNameType)
//		{
//			return;
//		}
//
//		if (pAttrNameType->n_Class != 1)
//		{
//			diffValue = diffValue / 1000 * 100;
//		}
//
//		if (diffValue != 0)
//		{
//			m_changeEuipAttrs[pAttrNameType->m_Name] = diffValue;
//		}
//	}
//}

bool Role::isEquip(EquipMap &mapEquip, int equipPos, int equipFishionPos)
{
	bool isHaveEquip{ false };
	auto iterClothes = mapEquip.find(equipPos);
	if (iterClothes == mapEquip.end())
	{
		iterClothes = mapEquip.find(equipFishionPos);
		if (iterClothes != mapEquip.end())
			isHaveEquip = true;
	}
	else
		isHaveEquip = true;
	return isHaveEquip;
}

void  Role::refreshEquip(const EquipMap &mapEquip)
{
	//s_currNakedModel = m_naked3DModel;
	//s_currNakedHighModel = m_naked3DHighModel;

	//s_trailModel = EMPTY_STRING;
	//s_currNakedAttachId = "naked";
	//EquipMap equipMapOut{};
	//getEquipMap(mapEquip,equipMapOut);
	//ArmorCfg::getInstance().findHideEquipAndNode(equipMapOut, m_vHideEquip, m_vHideNode, m_vFemaleHideNode, equipMapOut);
	//const PlayerAttrCfg::AttachInfoMap& pList = PlayerAttrCfg::getInstance().getAttachInfoList();
	//for (auto&& it : pList)
	//{
	//	auto pItemData = equipMapOut.find(it.m_attachPos) == equipMapOut.end() ? nullptr : equipMapOut.find(it.m_attachPos)->second;
	//	if (!pItemData || !pItemData->getArmorCfg() || std::find(m_vHideEquip.begin(), m_vHideEquip.end(), it.m_attachPos) != m_vHideEquip.end()) continue;
	//	if (pItemData->isHide()) continue;
	//	auto&& pArmor = pItemData->getArmorCfg();
	//	s_trailModel = !pArmor->s_trailEffectModel.empty() ? pArmor->s_trailEffectModel : s_trailModel;
	//	if (pArmor->n_replaceNakedModel == 1)
	//	{
	//		s_currNakedModel = pArmor->getModel(getGender());
	//		s_currNakedHighModel = pArmor->getHighModel(getGender());

	//		s_currNakedAttachId = it.m_mainAttachId;
	//	}
	//}

	//m_isEquipTwoHandWeapon = false;
	//auto itemDataIter = equipMapOut.find(MsgPB::EQUIP_PACKET_WEAPON);
	//if (itemDataIter != equipMapOut.end() && itemDataIter->second != nullptr)
	//{
	//	if (ArmorCfg::getInstance().isDoubleHand(itemDataIter->second->getItemId()))
	//	{
	//		m_isEquipTwoHandWeapon = true;
	//	}
	//}
}

bzbee::tstring Role::getAnimationPath(int eftPos, ModelLevel modelLv)
{
	return getAnimationPath(getEquipMap(), eftPos, modelLv);
}

bzbee::tstring Role::getAnimationPath(const Role::EquipMap& equipMap, int eftPos, ModelLevel modelLv)
{
//	if (eftPos == -1) return EMPTY_STRING;
//
//	std::map<int, int> mArmorId;
//	std::vector<std::tuple<int, std::map<int, tstring>>> mAnimFile;
//	const PlayerAttrCfg::AttachInfoMap& pList = PlayerAttrCfg::getInstance().getAttachInfoList();
//	IntegerList vHideEquip;
//	IntegerList vHideNode;
//	IntegerList vFemaleHideNode;
//	auto&& fullEquipMap = getEquipMap();
//	ArmorCfg::getInstance().findHideEquipAndNode(equipMap, vHideEquip, vHideNode, vFemaleHideNode, fullEquipMap);
//	for (auto&& it : pList)
//	{
//		// NOTE:不显示的装备不参与animation获取处理
//		if (std::find(vHideEquip.begin(), vHideEquip.end(), it.m_attachPos) != vHideEquip.end())
//			continue;
//
//		auto pItemData = equipMap.find(it.m_attachPos) == equipMap.end() ? nullptr : equipMap.find(it.m_attachPos)->second;
//		if (!pItemData || !pItemData->getArmorCfg())
//			continue;
//		// NOTE:获取非手持工具时的animation时,不采用MsgPB::EQUIP_PACKET_FARMTOOL的animation配置
//		if (eftPos != MsgPB::EQUIP_PACKET_FARMTOOL && it.m_attachPos == MsgPB::EQUIP_PACKET_FARMTOOL)
//			continue;
//
//		auto&& pArmor = pItemData->getArmorCfg();
//		mArmorId.insert(std::make_pair(it.m_attachPos, pArmor->m_typeId));
//
//		auto&& animMap = (modelLv == ModelLevel::Low)
//			? pArmor->getAnimationMap(getGender())
//			: pArmor->getHighAnimationMap(getGender());
//		if (!animMap.empty())
//		{
//			mAnimFile.push_back(std::make_tuple(it.m_attachPos, animMap));
//		}
//	}
//
//	if (mAnimFile.empty())
//	{
//		// NOTE:没有武器,使用空手动作
//		return PlayerAttrCfg::getInstance().getAnimationByModel(getCurrNakedModel(modelLv), modelLv);
////		return EMPTY_STRING;
//	}
//
//	auto ait = std::get<1>(mAnimFile.back());
//	tstring animationFilePath = ait.find(0) == ait.end() ? EMPTY_STRING : ait.find(0)->second;
//	for (std::map<int, int>::reverse_iterator it = mArmorId.rbegin(); it != mArmorId.rend(); ++it)
//	{
//		if (ait.find(it->second) != ait.end())
//			return ait[it->second];
//	}
//	return animationFilePath;
	return "";
}

int Role::getAtkAniCount(bool isJumping)
{
	//auto&& weapon = getCurrEftWeapon();
	//if (!weapon || !weapon->getArmorCfg())
	//{
	//	return GlobalCfg::getInstance().GetModelEmptyWeaponAniCount();
	//}

	//// 变形状态下，动画计数取怪物的配置
	//if (m_isOverrideModel)
	//{
	//	const MonsterCfg::MonsterType* pMonsterType = MonsterCfg::getInstance().findMonsterType(m_overrideMonsterId);
	//	if (pMonsterType)
	//	{
	//		return pMonsterType->m_atkAniEffect.size();
	//	}
	//}

	//const auto& aniEffect = isJumping ? weapon->getArmorCfg()->m_jumpAtkAniEffect : weapon->getArmorCfg()->m_atkAniEffect;
	//if (aniEffect.empty()) return 0;
	//return (int)aniEffect.size();
	return 0;
}

void Role::setDead(bool isDead)
{
	if (m_isDead && isDead == true)
		return;
	m_isDead = isDead;
	if (m_isDead)
	{
		m_deadTime = std::chrono::steady_clock::now();
	}
}

void Role::setHp(int hp, bool setDisplay)
{
	m_hp = std::min(m_nMaxHp, std::max(hp, 0));
}


// void Role::setDisplayHp(int hp)
// {
// 	assert(m_refreshFlatter == false); 
// 	m_displayHp = hp; 
// 	if (m_displayHp < 0)
// 		m_displayHp = 0;
// }

void Role::SetMoveSpeedRatio(float fMoveSpeedRatio)
{
	m_fMoveSpeedRatio = fMoveSpeedRatio;
}

float Role::GetMoveSpeedRatio()
{
	return m_fMoveSpeedRatio;
}

void Role::startOverrideModel(float overrideWidth, float overrideHeight, int targetMonsterId)
{
	m_isOverrideModel = true;
	m_overrideMonsterId = targetMonsterId;
	m_overrideWidth = overrideWidth;
	m_overrideHeight = overrideHeight;
}

void Role::endOverrideModel()
{
	m_isOverrideModel = false;
	m_overrideMonsterId = -1;
	m_overrideWidth = 0.0f;
	m_overrideHeight = 0.0f;
}

bzbee::tstring Role::GetShowIDTag()
{
	std::string strTag = fmt::format("{0}\nGroupId={1}\nCategory={2}", getId(), getGroupId(), (int)getCategory());
	return strTag;
}


void Role::filterAttrShow(AttrMap& attr)
{
	AttrMap::iterator iter = attr.begin();
	for (; iter != attr.end(); iter)
	{
		if (iter->first == ROLE_EFFECT_GATHER_ATK || iter->first == ROLE_EFFECT_JUMP || iter->first == ROLE_EFFECT_SWIM ||
			iter->first == ROLE_EFFECT_GATHER_SP || iter->first == ROLE_EFFECT_MV_SP || iter->first == ROLE_EFFECT_JUMP_INIT_SPEED ||
			iter->first == ROLE_EFFECT_HB || iter->first == ROLE_EFFECT_ATK_RANGE) {
			iter = attr.erase(iter);
		}
		else {
			iter++;
		}
	}
}
