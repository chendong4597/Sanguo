/*!
 * \file PassiveSkillMgr.h
 *
 * \author Dexuan Yang
 * \date 2019/10/04
 *
 */

#include "stdafx.h"

#include "PassiveSkillMgr.h"
#include "PassiveTrigger.h"
#include "PassiveEvents.h"
#include "../../ItemData.h"
//#include "skill/SkillComponent.h"
//#include "skill/SpellComponent_Lua.h"
//#include "common/RoleComponent.h"
//#include "handler/HandlerNpc.h"
//#include "handler/HandlerPet.h"
#include "../../../../static_logic/localserver/LocalServer_Battle.h"
#include <config/SkillCfg.h>
#include <data/ItemData.h>
//#include "player/PlayerComponent.h"
//#include "common/TransformComponent.h"
//#include "common/HpMgr.h"
//#include <common/MsgMgr.h>
#include <config/BuffCfg.h>
#include "../Role.h"


const std::unordered_map<int, PassiveSkillMgr::PassiveEventType> PassiveSkillMgr::m_PassiveEvents = {
	{(int)SkillType::TriggerType::TriggerTypeNone,					PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_NONE},
	{(int)SkillType::TriggerType::TriggerTypeHpLess,				PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_HP_CHANGE},		// 血量低于x%触发
	{(int)SkillType::TriggerType::TriggerTypeBeforeHurt,			PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_NONE},		// 减伤，伤害计算前触发
	{(int)SkillType::TriggerType::TriggerTypeUnderAttackRemoveBuff, PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_BEATK},	// 受击(普攻)，移除Buff
	{(int)SkillType::TriggerType::TriggerTypeUnderSkillRemoveBuff,	PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_BEATK},	// 受击(技能)，移除Buff
	{(int)(SkillType::TriggerType::TriggerTypeUnderAttackRemoveBuff | SkillType::TriggerType::TriggerTypeUnderSkillRemoveBuff), PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_BEATK},
	{(int)SkillType::TriggerType::TriggerTypeUnderAttackCastSkill,	PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_BEATK},	// 受击(普攻)，释放技能
	{(int)SkillType::TriggerType::TriggerTypeUnderSkillCastSkill,	PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_BEATK},	// 受击(技能)，释放技能
	{(int)(SkillType::TriggerType::TriggerTypeUnderAttackCastSkill | SkillType::TriggerType::TriggerTypeUnderSkillCastSkill), PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_BEATK},
	{(int)SkillType::TriggerType::TriggerTypeUnderAttackFightBack,	PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_BEATK},	// 受击(普攻)，反击
	{(int)SkillType::TriggerType::TriggerTypeUnderSkillFightBack,	PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_BEATK},	// 受击(技能)，反击
	{(int)(SkillType::TriggerType::TriggerTypeUnderAttackFightBack | SkillType::TriggerType::TriggerTypeUnderSkillFightBack), PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_BEATK},
	{(int)SkillType::TriggerType::TriggerTypeAttackBloodSucking,	PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_ATK_HIT},	// 攻击命中(普攻)，吸血
	{(int)SkillType::TriggerType::TriggerTypeCastingBloodSucking,	PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_ATK_HIT},	// 攻击命中(技能)，吸血
	{(int)(SkillType::TriggerType::TriggerTypeAttackBloodSucking | SkillType::TriggerType::TriggerTypeCastingBloodSucking), PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_ATK_HIT},
	{(int)SkillType::TriggerType::TriggerTypeAttackCastSkill,		PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_ATK},		// 攻击(普攻)，释放技能
	{(int)SkillType::TriggerType::TriggerTypeCastingCastSkill,		PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_ATK},		// 攻击(技能)，释放技能
	{(int)(SkillType::TriggerType::TriggerTypeAttackCastSkill | SkillType::TriggerType::TriggerTypeCastingCastSkill), PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_ATK},
	//{, },	// 1 << 12,																									// 受击(普攻)，伤害宿主
	//{, },	// 1 << 13,																									// 受击(技能)，伤害宿主
	//{SkillType::TriggerType::TriggerTypeAllyDie, },
	{(int)SkillType::TriggerType::TriggerTypePermanent,				PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_NONE},		// 常驻型被动，装载直接触发上buff
	////{SkillType::TriggerType::TriggerTypeMobDie, },																	// 指定怪物死亡触发
	////{SkillType::TriggerType::TriggerTypeTargetByMobIDType, },														// 指定怪物在身边时候
	{(int)SkillType::TriggerType::TriggerTypeByTime,				PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_CLOCK},	// 指定时间段触发
	{(int)SkillType::TriggerType::TriggerTypeByFallingDist,			PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_MOVING},   // 坠落触发
	{(int)SkillType::TriggerType::TriggerTypeByWater,				PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_WATER},	// 液体中触发
	{(int)SkillType::TriggerType::TriggerTypeByMoving,				PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_MOVING},   // 移动时触发
	{(int)SkillType::TriggerType::TriggerTypeBySprint,				PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_MOVING},   // 冲刺时触发
	{(int)SkillType::TriggerType::TriggerTypeByPetOrNpc,			PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_FOLLOW},   // 宠物&NPC出战触发
	{(int)SkillType::TriggerType::TriggerTypeByYAxisDepth,          PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_POS_CHANGE},	// 达到Y轴一定深度触发
	//{SkillType::TriggerType::TriggerTypeHpLessLoop, },																// 每损失x%生命触发技能(不走CD)
	{(int)SkillType::TriggerType::TriggerTypeCasterKillTarget,      PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_KILL_SOMEONE},// 施法者杀死目标时施法者触发
	{(int)SkillType::TriggerType::TriggerTypeCasterKilled,          PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_KILL_SOMEONE},// 自己被击杀后施法
	{(int)SkillType::TriggerType::TriggerTypeAura,					PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_MAPOBJ_INOUT_SIGHT},// 自己被击杀后施法
};

int PassiveSkillMgr::getMapKeyBySkillType(const SkillType* pSkillType)
{
	if (!pSkillType)
	{
		return -1;
	}
	// 避免策划非武器类的装备主动技能，触发类型瞎配
	if (pSkillType->IsActivesSkill())
	{
		return 0;
	}
	// 受击事件触发类型，统一为受击事件的Key
	NS::int32 nTriggerType = pSkillType->m_triggerType;
	if (((SkillType::TriggerType::TriggerTypeUnderAttackRemoveBuff
		| SkillType::TriggerType::TriggerTypeUnderAttackCastSkill
		| SkillType::TriggerType::TriggerTypeUnderAttackFightBack
		| SkillType::TriggerType::TriggerTypeUnderSkillRemoveBuff
		| SkillType::TriggerType::TriggerTypeUnderSkillCastSkill
		| SkillType::TriggerType::TriggerTypeUnderSkillFightBack) & nTriggerType) != 0)
	{
		return static_cast<int>(PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_BEATK);
	}
	// 攻击事件触发类型，统一为攻击事件的Key
	if (((SkillType::TriggerType::TriggerTypeAttackCastSkill
		| SkillType::TriggerType::TriggerTypeCastingCastSkill) & nTriggerType) != 0)
	{
		return static_cast<int>(PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_ATK);
	}

	if (((SkillType::TriggerType::TriggerTypeAttackBloodSucking
		| SkillType::TriggerType::TriggerTypeCastingBloodSucking) & nTriggerType) != 0)
	{
		return static_cast<int>(PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_ATK_HIT);
	}

	return nTriggerType;
}

PassiveSkillMgr::PassiveSkillMgr(std::shared_ptr<Role> pRole)
	: m_pRole(pRole)
	, m_arrayCounters({})
{
}

PassiveSkillMgr::~PassiveSkillMgr()
{
	// clear passive trigger
	for (auto iter : m_mapPassives)
	{
		SAFE_DELETE(iter.second);
	}
	m_mapPassives.clear();
	while (m_vecPassiveCommands.size() > 0)
	{
		m_vecPassiveCommands.pop();
	}

	//m_pSkillComponent = nullptr;
}

void PassiveSkillMgr::update(float dt)
{
	//if (!NS::MsgMgr::getInstance().getClientMode()) { return; }
	doDelayInitialze();
	doTriggerAction();
}

void PassiveSkillMgr::onDead()
{
	for (auto&& iter : m_mapPassives)
	{
		PassiveTrigger* pPassiveTrigger = iter.second;
		if (!pPassiveTrigger || !pPassiveTrigger->getTriggerEnable()) { continue; }

		pPassiveTrigger->onDead();
	}
}

void PassiveSkillMgr::onResurrection()
{
	for (auto&& iter : m_mapPassives)
	{
		PassiveTrigger* pPassiveTrigger = iter.second;
		if (!pPassiveTrigger || !pPassiveTrigger->getTriggerEnable()) { continue; }

		pPassiveTrigger->onResurrection();
	}
}

void PassiveSkillMgr::pushPassiveSkill(const SkillType* pSkillType, NS::I64 nItemUuid /* = 0 */)
{
	//if (!NS::MsgMgr::getInstance().getClientMode()) { return; }
	if (!pSkillType) { return; }

	std::shared_ptr<ItemData> pEquipmentData{ nullptr };
	if (nItemUuid != 0)
	{
		pEquipmentData = ItemPacketMgr::getInstance().findItemDataByUuid(nItemUuid);
	}
	PassiveTrigger* pPassiveTrigger = getPassiveTrigger(pSkillType, pEquipmentData);
	if (pPassiveTrigger)
	{
		// 准备激活被动技能状态
		int nKey = getMapKeyBySkillType(pSkillType);
		activateSkill(pPassiveTrigger, pEquipmentData, true);
		m_mapPassives.emplace(nKey, pPassiveTrigger);
		int nTriggerType = pSkillType->IsActivesSkill() ? 0 : pSkillType->m_triggerType;
		registerPassiveListener(nTriggerType);
	}
}

void PassiveSkillMgr::removePassiveSkill(const SkillType* pSkillType, NS::I64 nItemUuid)
{
	//if (!NS::MsgMgr::getInstance().getClientMode()) { return; }
	if (!pSkillType) { return; }

	int nKey = getMapKeyBySkillType(pSkillType);
	// find remove iter
	std::unordered_multimap<int, PassiveTrigger*>::iterator removeIter{ m_mapPassives.end() };
	auto range = m_mapPassives.equal_range(nKey);
	for (auto&& iter = range.first; iter != range.second; ++iter)
	{
		if (iter->second->getSkillType()->m_id == pSkillType->m_id && iter->second->getEquipmentUuid() == nItemUuid)
		{
			removeIter = iter;
			break;
		}
	}
	// remove
	if (removeIter == m_mapPassives.end()) { return; }

	std::shared_ptr<ItemData> pEquipmentData{ nullptr };
	if (nItemUuid != 0)
	{
		pEquipmentData = ItemPacketMgr::getInstance().findItemDataByUuid(nItemUuid);
	}
	PassiveTrigger* pPassiveTrigger{ removeIter->second };
	m_mapPassives.erase(removeIter);
	int nTriggerType = pSkillType->IsActivesSkill() ? 0 : pSkillType->m_triggerType;
	unRegisterPassiveListener(nTriggerType);
	if (!pPassiveTrigger)
	{
		return;
	}
	// 检查是否是否切换装备激活
	activateSkill(pPassiveTrigger, pEquipmentData, false);
	SAFE_DELETE(pPassiveTrigger);
}

void PassiveSkillMgr::refreshCommonSkills(std::vector<const SkillType*>& vecSkills)
{
	//if (!NS::MsgMgr::getInstance().getClientMode()) { return; }
	std::vector<PassiveTrigger*> m_waitDelSkills;
	for (auto&& iter : m_mapPassives)
	{
		if (iter.second->getEquipmentUuid() != 0) { continue; }	// 这里只更新角色自带技能，装备技能不处理
		const SkillType* pSkillType = iter.second->getSkillType();
		auto&& findIter = std::find(vecSkills.begin(), vecSkills.end(), pSkillType);

		if (findIter == vecSkills.end())
		{
			m_waitDelSkills.push_back(iter.second); 
		}
		else
		{
			vecSkills.erase(findIter); // 已存在的被动技能，不处理
		}
	}
	// 移除消失的被动技能和效果
	for (size_t i = 0; i < m_waitDelSkills.size(); ++i)
	{
		PassiveTrigger* pPassiveTrigger = m_waitDelSkills[i];
		removePassiveSkill(pPassiveTrigger->getSkillType(), pPassiveTrigger->getEquipmentUuid());
	}

	// 添加新的被动技能和效果
	for (size_t i = 0; i < vecSkills.size(); ++i)
	{
		const SkillType* pSkillType = vecSkills[i];
		if (!pSkillType) { continue; }
		pushPassiveSkill(pSkillType);
	}
}

void PassiveSkillMgr::refreshEquipmentSkills(std::vector<std::pair<const SkillType*, NS::I64>>& vecEquipmentSkills)
{
	//if (!NS::MsgMgr::getInstance().getClientMode()) { return; }
	std::vector<PassiveTrigger*> m_waitDelSkills;
	for (auto&& iter : m_mapPassives)
	{
		if (iter.second->getEquipmentUuid() == 0) { continue; }	// 这里只更新角色装备技能，非装备技能不处理
		const SkillType* pTargetSkillType = iter.second->getSkillType();
		NS::int32 nTargetSkillId = pTargetSkillType->m_id;
		NS::I64 nTargetEquipmentUuId = iter.second->getEquipmentUuid();
		auto&& findIter = std::find_if(vecEquipmentSkills.begin(), vecEquipmentSkills.end(), [nTargetSkillId, nTargetEquipmentUuId](const std::pair<const SkillType*, NS::I64>& itemSkillData) -> bool
			{
				const SkillType* pSkillTypeCurr = std::get<0>(itemSkillData);
				NS::I64 nEquipmentUuId = std::get<1>(itemSkillData);
				return pSkillTypeCurr->m_id == nTargetSkillId && nEquipmentUuId == nTargetEquipmentUuId;
			});

		if (findIter == vecEquipmentSkills.end())
		{
			m_waitDelSkills.push_back(iter.second);
		}
		else
		{
			vecEquipmentSkills.erase(findIter); // 已存在的被动技能，不处理
		}
	}
	// 移除的被动技能和效果
	for (size_t i = 0; i < m_waitDelSkills.size(); ++i)
	{
		PassiveTrigger* pPassiveTrigger = m_waitDelSkills[i];
		removePassiveSkill(pPassiveTrigger->getSkillType(), pPassiveTrigger->getEquipmentUuid());
	}

	// 新的被动技能和效果
	for (size_t i = 0; i < vecEquipmentSkills.size(); ++i)
	{
		const SkillType* pSkillType = std::get<0>(vecEquipmentSkills[i]);
		NS::I64 nEquipmentUuid = std::get<1>(vecEquipmentSkills[i]);
		if (!pSkillType) { continue; }
		pushPassiveSkill(pSkillType, nEquipmentUuid);
	}
}

void PassiveSkillMgr::notifyEquipmentBroken(const SkillType* pSkillType, const std::shared_ptr<ItemData>& pEquipmentData, bool bIsBroken)
{
	//if (!NS::MsgMgr::getInstance().getClientMode()) { return; }
	if (!pSkillType || !pEquipmentData) { return; }

	int nKey = getMapKeyBySkillType(pSkillType);
	auto range = m_mapPassives.equal_range(nKey);
	std::unordered_multimap<int, PassiveTrigger*>::iterator changeIter{ m_mapPassives.end() };
	for (auto&& iter = range.first; iter != range.second; ++iter)
	{
		if (iter->second->getSkillType()->m_id == pSkillType->m_id && iter->second->getEquipmentUuid() == pEquipmentData->getUuid())
		{
			changeIter = iter;
			break;
		}
	}

	if (changeIter == m_mapPassives.end()) { return; }

	PassiveTrigger* pPassiveTrigger{ changeIter->second };
	if (!pPassiveTrigger) { return; }

	activateSkill(pPassiveTrigger, pEquipmentData, !bIsBroken);
}

//void PassiveSkillMgr::notifySkillEndEvent(LuaSkillEvent& evt)
//{
//	if (m_pSkillComponent && m_pSkillComponent->getEntity())
//	{
//		m_pSkillComponent->getEntity()->postEvent(evt);
//	}
//}

bool PassiveSkillMgr::activateSkill(PassiveTrigger* pPassiveTrigger, const std::shared_ptr<ItemData>& pEquipmentData, bool bEnable)
{
	// 无关装备的被动技能，无规则过滤，可以直接激活
	if (!pEquipmentData)
	{
		pPassiveTrigger->setTriggerEnable(bEnable);
		return true;
	}
	// 破损的装备，直接设置触发器不启用
	if (pEquipmentData->getDurable() <= 0 && bEnable) 
	{
		pPassiveTrigger->setTriggerEnable(false);
		return true;
	}
	// 触发类型
	int nKey = getMapKeyBySkillType(pPassiveTrigger->getSkillType());
	if (nKey < 0)
	{
		// Something error happened.
		//LOGE("PassiveSkillMgr::activateSkill, not Found SkillType.");
		return false;
	}
	// 同个装备被动技能，只要激活一个就好了，特例，缓降触发的被动只要触发一个就行了，激活规则：装备位在前的优先触发
//	int nChangePassiveSkillId = pPassiveTrigger->getSkillType()->m_id;
	auto range = m_mapPassives.equal_range(nKey);
	std::vector<PassiveTrigger*> vecTriggers{};
	for (auto&& iter = range.first; iter != range.second; ++iter)
	{
		NS::I64 nNowEquipmentUuid = iter->second->getEquipmentUuid();
		// 过滤非装备被动技能和破损的装备
		std::shared_ptr<ItemData> pNowEquipmentData = ItemPacketMgr::getInstance().findItemDataByUuid(nNowEquipmentUuid);
		if (!pNowEquipmentData || pNowEquipmentData->getDurable() <= 0) { continue; }
		// 同装备不同uid or 同降落触发被动技能，push
		if (nNowEquipmentUuid != pEquipmentData->getUuid() && pNowEquipmentData->getItemId() == pEquipmentData->getItemId())
		{
			vecTriggers.push_back(iter->second);
		}
		else if(nKey == SkillType::TriggerType::TriggerTypeByFallingDist && nNowEquipmentUuid != pEquipmentData->getUuid())
		{
			vecTriggers.push_back(iter->second);
		}
	}
	if (vecTriggers.empty())
	{
		pPassiveTrigger->setTriggerEnable(bEnable);
		return true;
	}
	// 按装备位置排序
	std::sort(vecTriggers.begin(), vecTriggers.end(), [](const PassiveTrigger* lData, const PassiveTrigger* rData) -> bool
		{
			return lData->getEquipmentPosIndex() < rData->getEquipmentPosIndex();
		});

	PassiveTrigger* pCurPassiveTrigger = *(vecTriggers.begin());
	if (!bEnable)
	{
		// 卸载技能，最小位直接激活
		pCurPassiveTrigger->setTriggerEnable(!bEnable);
		pPassiveTrigger->setTriggerEnable(bEnable);
	}
	else
	{
		// 装载技能，最小位与装载装备技能比较，做对应变更
		if (pCurPassiveTrigger->getEquipmentPosIndex() > pPassiveTrigger->getEquipmentPosIndex())
		{
			pCurPassiveTrigger->setTriggerEnable(!bEnable);
			pPassiveTrigger->setTriggerEnable(bEnable);
		}
	}

	return true;
}

void PassiveSkillMgr::getPassiveSkills(std::vector<NS::int32>& vecPassiveSkills)
{
	for (auto&& iter : m_mapPassives)
	{
		const SkillType* pSkillType = iter.second->getSkillType();
		if (!pSkillType) { continue; }
		vecPassiveSkills.push_back(pSkillType->m_id);
	}
}

const SkillType* PassiveSkillMgr::findSkillTypeById(NS::int32 nSkillId)
{
	const SkillType* pSkillType{ nullptr };
	for (auto&& iter : m_mapPassives)
	{
		if (iter.second && iter.second->getSkillType()->m_id == nSkillId)
		{
			pSkillType = iter.second->getSkillType();
			break;
		}
	}
	return pSkillType;
}

bool PassiveSkillMgr::canCastSkill(const SkillType* pSkillType) const
{
	//if (!m_pSkillComponent || !pSkillType) { return false; }
	//return m_pSkillComponent->canCast(pSkillType, false);
	return true;
}

NS::int32 PassiveSkillMgr::getCasterId() const
{
	if (!m_pRole) { return -1; }

	return m_pRole->getId();
}

//const RoleComponent* PassiveSkillMgr::getCasterRoleComponent() const
//{
//	if (!m_pSkillComponent)
//	{
//		return nullptr;
//	}
//	return dynamic_cast<RoleComponent*>(m_pSkillComponent->getEntity());
//}

const bool PassiveSkillMgr::isTriggerTargetSkill(int nSkillId) const
{
	for (auto&& iter : m_mapPassives)
	{
		if (iter.second->isTriggering())
		{
			auto* p_skillType = iter.second->getSkillType();
			if (p_skillType->m_id == nSkillId)
			{
				return true;
			}
		}
	}
	return false;
}

const void PassiveSkillMgr::findOverrideFirstMobId(int& outId) const
{
	if (!m_pRole) return;
	if (m_pRole->getHp() <= 0) return;
	int p_lastFindPos{ 999 };
	for (auto&& iter : m_mapPassives)
	{
		if (!iter.second->isTriggering()) continue;
		auto* p_skillType = iter.second->getSkillType();
		if (!p_skillType) continue;
		for (int i = 0; i < SkillType::MAX_EFFECTS; ++i)
		{
			const SkillType::Effect* pEffect = p_skillType->findEffect(i);
			if (!pEffect) continue;
			if (pEffect->m_type != SkillType::Effect::Type::AddBuff) continue;

			int32 p_BuffId = pEffect->m_effectAttrRefs[0].m_effectValue;
			if (p_BuffId <= 0) continue;

			const BuffType* pBuffType = BuffCfg::getInstance().findBuffType(p_BuffId);
			int nPosIndex{ -1 };
			int nOverrideModelMonsterId{ -1 };
			//if (p_roleComp->getRoleOverrideModelEffect(pBuffType, nPosIndex, nOverrideModelMonsterId))
			//{
			//	if (iter.second->getEquipmentPosIndex() < p_lastFindPos)
			//	{
			//		p_lastFindPos = iter.second->getEquipmentPosIndex();
			//		outId = nOverrideModelMonsterId;
			//	}
			//}
		}

	}
}

void PassiveSkillMgr::pushActionCommand(const PassiveCommand& command, bool bDoRighNow/* = false*/)
{
	if (bDoRighNow)
	{
		if (command.m_bTrigger)
		{
			doCastPassiveSkill(command);
		}
		else
		{
			doCancelPassiveEffect(command.m_pSkillType, command.m_vecTargetMapObjIds);
		}
	}
	else
	{
		m_vecPassiveCommands.push(command);
	}
}

void PassiveSkillMgr::pushDelayInitialize(PassiveTrigger* pPassiveTrigger)
{
	m_vecDelayInitializes.push_back(pPassiveTrigger);
}

float PassiveSkillMgr::damageReduction() const
{
	float fReduceDamagePercent{ 0.0f };
	auto&& range = m_mapPassives.equal_range(SkillType::TriggerType::TriggerTypeBeforeHurt);
	for (auto&& iter = range.first; iter != range.second; ++iter)
	{
		PassiveTrigger* pPassiveTrigger = iter->second;
		if (!pPassiveTrigger || !pPassiveTrigger->getTriggerEnable()) { continue; }
		// 触发被动技能
		OtherParam noneParams{};
		pPassiveTrigger->trigger(noneParams);
		// 根据触发状态计算减伤值
		if (pPassiveTrigger->isTriggering())
		{
			const SkillType* pSkillType = iter->second->getSkillType();
			fReduceDamagePercent = fReduceDamagePercent <= 0 ? static_cast<float>(pSkillType->m_triggerParam2) : fReduceDamagePercent * (100.0f + pSkillType->m_triggerParam2) / 100.0f; // 和志凌确认，多格挡技能 乘算
		}
		// 重置触发状态，
		pPassiveTrigger->unInitialize();
	}

	return fReduceDamagePercent;
}

void PassiveSkillMgr::doCastPassiveSkill(const PassiveCommand& command)
{
	const SkillType* pSkillType{ command.m_pSkillType };
	if (!pSkillType) { return; }

	NS::int32 nCasterMapObjId{ getCasterId() };
	if (nCasterMapObjId < 0) { return; }

	const std::vector<NS::int32> vecTargetMapObjIds = command.m_vecTargetMapObjIds;

	if (vecTargetMapObjIds.empty())
	{
		NS::LocalServer_Battle::getInstance().cast_skill(pSkillType->m_id, nCasterMapObjId, command.m_targetPosition);
	}
	else if (vecTargetMapObjIds.size() == 1)
	{
		NS::int32 nTargetMapObjId = vecTargetMapObjIds[0];
		NS::LocalServer_Battle::getInstance().cast_skill(pSkillType->m_id, nCasterMapObjId, nTargetMapObjId);
	}
	else
	{
		NS::LocalServer_Battle::getInstance().cast_skill(pSkillType->m_id, nCasterMapObjId, vecTargetMapObjIds);
	}

	if (command.m_eType == PassiveCommand::Type::COUNTERATK && command.m_nHurtValue > 0)
	{
		// 反击，额外计算反击伤害
		for (size_t i = 0; i < vecTargetMapObjIds.size(); ++i)
		{
			NS::LocalServer_Battle::getInstance().countAttack(pSkillType->m_id, nCasterMapObjId, vecTargetMapObjIds[i], command.m_nHurtValue);
		}
	}
	if (command.m_eType == PassiveCommand::Type::SUCKING && command.m_nHurtValue > 0)
	{
		// 吸血，额外计算回复值
		for (size_t i = 0; i < vecTargetMapObjIds.size(); ++i)
		{
			NS::LocalServer_Battle::getInstance().suckingBlood(pSkillType->m_id, nCasterMapObjId, vecTargetMapObjIds[i], command.m_nHurtValue);
		}
	}
}

void PassiveSkillMgr::doCancelPassiveEffect(const SkillType* pSkillType, const std::vector<NS::int32>& vecTargetMapObjIds)
{
	if (!pSkillType) { return; }
	if (vecTargetMapObjIds.empty()) { return; }

	NS::int32 nCasterMapObjId{ getCasterId() };
	if (nCasterMapObjId < 0) { return; }

	// 持续性的被动技能效果，移除Buff
	for (size_t i = 0; i < vecTargetMapObjIds.size(); ++i)
	{
		NS::int32 nTargetMapObjId = vecTargetMapObjIds[i];
		NS::LocalServer_Battle::getInstance().skillBuffRemove(pSkillType->m_id, nCasterMapObjId, nTargetMapObjId);
	}
}

void PassiveSkillMgr::doTriggerAction()
{
	if (m_vecPassiveCommands.empty()) { return; }

	while (m_vecPassiveCommands.size() > 0)
	{
		auto&& command = m_vecPassiveCommands.front();
		if (command.m_bTrigger)
		{
			doCastPassiveSkill(command);
		}
		else
		{
			doCancelPassiveEffect(command.m_pSkillType, command.m_vecTargetMapObjIds);
		}
		m_vecPassiveCommands.pop();
	}
}

void PassiveSkillMgr::doDelayInitialze()
{
	if (m_vecDelayInitializes.empty()) { return; }

	for (PassiveTrigger* pPassiveTrigger : m_vecDelayInitializes)
	{
		pPassiveTrigger->initialize(true);
	}
	m_vecDelayInitializes.clear();
}

PassiveTrigger* PassiveSkillMgr::getPassiveTrigger(const SkillType* pSkillType, const std::shared_ptr<ItemData>& pEquipmentData)
{
	bool bIsLegalEquipmentPos{ false };
	NS::I64 nItemUuid{ 0 };
	//if (pEquipmentData)
	//{
	//	bIsLegalEquipmentPos = pEquipmentData->getEquipPos() != MsgPB::EQUIP_PACKET_POS::EQUIP_PACKET_WEAPON
	//		&& pEquipmentData->getEquipPos() != MsgPB::EQUIP_PACKET_POS::EQUIP_PACKET_FARMTOOL
	//		&& pEquipmentData->getEquipPos() != MsgPB::EQUIP_PACKET_POS::EQUIP_PACKET_COS_WEAPON;

	//	nItemUuid = pEquipmentData->getUuid();
	//}
	// 非 武器/工具/时装武器 上的主动技能，视为无条件立即触发的被动技能
	if (pSkillType->IsActivesSkill() && bIsLegalEquipmentPos)
	{
		return new PassiveTrigger_ByNothing(pSkillType, this, nItemUuid);
	}

	switch (pSkillType->m_triggerType)
	{
	//case SkillType::TriggerType::TriggerTypeNone:
	//	break;
	case SkillType::TriggerType::TriggerTypeHpLess:
		return new PassiveTrigger_HpLess(pSkillType, this, nItemUuid);
		break;

	case SkillType::TriggerType::TriggerTypeBeforeHurt:
		return new PassiveTrigger_DamageReduction(pSkillType, this, nItemUuid);
		break;

	case SkillType::TriggerType::TriggerTypeUnderAttackRemoveBuff:
	case SkillType::TriggerType::TriggerTypeUnderSkillRemoveBuff:
	case (SkillType::TriggerType::TriggerTypeUnderAttackRemoveBuff | SkillType::TriggerType::TriggerTypeUnderSkillRemoveBuff):
		return new PassiveTrigger_BeAtkRemoveBuff(pSkillType, this, nItemUuid);

	case SkillType::TriggerType::TriggerTypeUnderAttackCastSkill:
	case SkillType::TriggerType::TriggerTypeUnderSkillCastSkill:
	case (SkillType::TriggerType::TriggerTypeUnderAttackCastSkill | SkillType::TriggerType::TriggerTypeUnderSkillCastSkill):
		return new PassiveTrigger_BeAtkCastSkill(pSkillType, this, nItemUuid);

	case SkillType::TriggerType::TriggerTypeUnderAttackFightBack:
	case SkillType::TriggerType::TriggerTypeUnderSkillFightBack:
	case (SkillType::TriggerType::TriggerTypeUnderAttackFightBack | SkillType::TriggerType::TriggerTypeUnderSkillFightBack):
		return new PassiveTrigger_CounterAttack(pSkillType, this, nItemUuid);

	case SkillType::TriggerType::TriggerTypeAttackBloodSucking:
	case SkillType::TriggerType::TriggerTypeCastingBloodSucking:
	case (SkillType::TriggerType::TriggerTypeAttackBloodSucking | SkillType::TriggerType::TriggerTypeCastingBloodSucking):
		return new PassiveTrigger_BloodSucking(pSkillType, this, nItemUuid);

	case SkillType::TriggerType::TriggerTypeAttackCastSkill:
	case SkillType::TriggerType::TriggerTypeCastingCastSkill:
	case (SkillType::TriggerType::TriggerTypeAttackCastSkill | SkillType::TriggerType::TriggerTypeCastingCastSkill):
		return new PassiveTrigger_AtkCastSkill(pSkillType, this, nItemUuid);

	case SkillType::TriggerType::TriggerTypeAllyDie:
		return new PassiveTrigger_Ai(pSkillType, this, nItemUuid);
		break;

	case SkillType::TriggerType::TriggerTypePermanent:
		return new PassiveTrigger_ByNothing(pSkillType, this, nItemUuid);

	case SkillType::TriggerType::TriggerTypeMobDie:
		return new PassiveTrigger_Ai(pSkillType, this, nItemUuid);
		break;
	case SkillType::TriggerType::TriggerTypeTargetByMobIDType:
		return new PassiveTrigger_Ai(pSkillType, this, nItemUuid);
		break;

	case SkillType::TriggerType::TriggerTypeByTime:
		return new PassiveTrigger_ByClock(pSkillType, this, nItemUuid);

	case SkillType::TriggerType::TriggerTypeByFallingDist:
		return new PassiveTrigger_ByFalling(pSkillType, this, nItemUuid);

	case SkillType::TriggerType::TriggerTypeByWater:
		return new PassiveTrigger_InWater(pSkillType, this, nItemUuid);

	case SkillType::TriggerType::TriggerTypeByMoving:
		return new PassiveTrigger_ByMoving(pSkillType, this, nItemUuid);

	case SkillType::TriggerType::TriggerTypeBySprint:
		return new PassiveTrigger_BySprinting(pSkillType, this, nItemUuid);

	case SkillType::TriggerType::TriggerTypeByPetOrNpc:
		return new PassiveTrigger_ByNpcOrPet(pSkillType, this, nItemUuid);

	case SkillType::TriggerType::TriggerTypeByYAxisDepth:
		return new PassiveTrigger_ByPositionY(pSkillType, this, nItemUuid);

	case SkillType::TriggerType::TriggerTypeHpLessLoop:
		return new PassiveTrigger_Ai(pSkillType, this, nItemUuid);

	case SkillType::TriggerType::TriggerTypeCasterKillTarget:
		return new PassiveTrigger_ByKillSomeone(pSkillType, this, nItemUuid);

	case SkillType::TriggerType::TriggerTypeCasterKilled:
		return new PassiveTrigger_Bykilled(pSkillType, this, nItemUuid);

	case SkillType::TriggerType::TriggerTypeAura:
		return new PassiveTrigger_Aura(pSkillType, this, nItemUuid);

	default:
		break;
	}

	return nullptr;
}

void PassiveSkillMgr::registerPassiveListener(int nTriggerType)
{
	int posIndex = getEventTypeByTriggerType(nTriggerType);
	if (posIndex < 0 || posIndex >= static_cast<int>(m_arrayCounters.size()))
	{
		return;
	}

	int& nCounter = m_arrayCounters[posIndex];
	nCounter++;
	if (nCounter == 1)
	{
		doRegisterPassiveListener(static_cast<PassiveEventType>(posIndex), true);
	}
}

void PassiveSkillMgr::unRegisterPassiveListener(int nTriggerType)
{
	int posIndex = getEventTypeByTriggerType(nTriggerType);
	if (posIndex < 0 || posIndex >= static_cast<int>(m_arrayCounters.size()))
	{
		return;
	}

	int& nCounter = m_arrayCounters[posIndex];
	nCounter = nCounter <= 0 ? 0 : nCounter - 1;
	if (nCounter == 0)
	{
		doRegisterPassiveListener(static_cast<PassiveEventType>(posIndex), false);
	}
}

int PassiveSkillMgr::getEventTypeByTriggerType(int nTriggerType)
{
	auto&& iter = m_PassiveEvents.find(nTriggerType);
	if (iter == m_PassiveEvents.end())
	{
		return -1;
	}
	return static_cast<int>(iter->second);
}

void PassiveSkillMgr::doRegisterPassiveListener(PassiveEventType eEventType, bool bReg)
{
	switch (eEventType)
	{
	case PassiveEventType::PASSIVE_EVENT_BEATK:
		// 受击事件监听
		registerBeAtkEvent(bReg);
		break;

	case PassiveEventType::PASSIVE_EVENT_ATK:
		// 攻击事件监听
		registerAtkEvent(bReg);
		break;

	case PassiveEventType::PASSIVE_EVENT_ATK_HIT:
		// 攻击命中事件
		registerAtkHitEvent(bReg);
		break;

	case PassiveEventType::PASSIVE_EVENT_CLOCK:
		// 时间变化事件监听
		//regsiterGameClockEvent(bReg);
		break;

	case PassiveEventType::PASSIVE_EVENT_MOVING:
		// 移动事件监听
		registerMovementEvent(bReg);
		break;

	case PassiveEventType::PASSIVE_EVENT_FOLLOW:
		// 跟随状态变更事件监听
		registerFollowEvent(bReg);
		break;

	case PassiveEventType::PASSIVE_EVENT_WATER:
		// 出入水变更监听
		registerWaterInOutEvent(bReg);
		break;
	case PassiveEventType::PASSIVE_EVENT_POS_CHANGE:
		//位置变化监听
		registerPositionChange(bReg);
		break;
	case PassiveEventType::PASSIVE_EVENT_HP_CHANGE:
		//血量变化监听
		registerHpChangeEvent(bReg);
		break;
	case PassiveEventType::PASSIVE_EVENT_KILL_SOMEONE:
		registerKillSomeoneEvent(bReg);
		break;
	case PassiveEventType::PASSIVE_EVENT_MAPOBJ_INOUT_SIGHT:
		registerMapObjInOutSight(bReg);
		break;
	default:
		break;
	}
}

void PassiveSkillMgr::registerBeAtkEvent(bool bReg)
{
	//if (!m_pSkillComponent) { return; }
	//RoleComponent* pOwner = dynamic_cast<RoleComponent*>(m_pSkillComponent->getEntity());
	//if (!pOwner) { return; }
	//if (bReg)
	//{
	//	REGISTER_EVENT(pOwner, PassiveEvent_BeAtk, &PassiveSkillMgr::onBeAtkEvent);
	//}
	//else
	//{
	//	UNREGISTER_EVENT(pOwner, PassiveEvent_BeAtk);
	//}
}

void PassiveSkillMgr::onBeAtkEvent(const PassiveEvent_BeAtk& evt)
{
	auto&& range = m_mapPassives.equal_range(static_cast<int>(PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_BEATK));
	for (auto&& iter = range.first; iter != range.second; ++iter)
	{
		if (!iter->second->getTriggerEnable()) { continue; }
		OtherParam params{};
		params.m_pBeAtkEvent = &evt;
		iter->second->trigger(params);
	}
}

void PassiveSkillMgr::registerAtkEvent(bool bReg)
{
	//if (!m_pSkillComponent) { return; }
	//RoleComponent* pOwner = dynamic_cast<RoleComponent*>(m_pSkillComponent->getEntity());
	//if (!pOwner) { return; }
	//if (bReg)
	//{
	//	REGISTER_EVENT(pOwner, PassiveEvent_Attack, &PassiveSkillMgr::onAtkEvent);
	//}
	//else
	//{
	//	UNREGISTER_EVENT(pOwner, PassiveEvent_Attack);
	//}
}

void PassiveSkillMgr::onAtkEvent(const PassiveEvent_Attack& evt)
{
	auto&& range = m_mapPassives.equal_range(static_cast<int>(PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_ATK));
	for (auto&& iter = range.first; iter != range.second; ++iter)
	{
		if (!iter->second->getTriggerEnable()) { continue; }
		OtherParam params{};
		params.m_pAtkEvent = &evt;
		iter->second->trigger(params);
	}
}

void PassiveSkillMgr::registerAtkHitEvent(bool bReg)
{
	//if (!m_pSkillComponent) { return; }
	//RoleComponent* pOwner = dynamic_cast<RoleComponent*>(m_pSkillComponent->getEntity());
	//if (!pOwner) { return; }
	//if (bReg)
	//{
	//	REGISTER_EVENT(pOwner, PassiveEvent_Hit, &PassiveSkillMgr::onAtkHitEvent);
	//}
	//else
	//{
	//	UNREGISTER_EVENT(pOwner, PassiveEvent_Hit);
	//}
}

void PassiveSkillMgr::onAtkHitEvent(const PassiveEvent_Hit& evt)
{
	auto&& range = m_mapPassives.equal_range(static_cast<int>(PassiveSkillMgr::PassiveEventType::PASSIVE_EVENT_ATK_HIT));
	for (auto&& iter = range.first; iter != range.second; ++iter)
	{
		if (!iter->second->getTriggerEnable()) { continue; }
		OtherParam params{};
		params.m_pHitEvent = &evt;
		iter->second->trigger(params);
	}
}

//void PassiveSkillMgr::regsiterGameClockEvent(bool bReg)
//{
//	if (bReg)
//	{
//		REGISTER_EVENT(&NS::LocalServer_Battle::getInstance(), LocalServer_Battle_Clock, &PassiveSkillMgr::onGameClockChange);
//	}
//	else
//	{
//		UNREGISTER_EVENT(&NS::LocalServer_Battle::getInstance(), LocalServer_Battle_Clock);
//	}
//}
//
//void PassiveSkillMgr::onGameClockChange(const LocalServer_Battle_Clock& evt)
//{
//	auto&& range = m_mapPassives.equal_range(SkillType::TriggerType::TriggerTypeByTime);
//	for (auto&& iter = range.first; iter != range.second; ++iter)
//	{
//		if (!iter->second->getTriggerEnable()) { continue; }
//		OtherParam params{};
//		params.m_pClockData = &evt;
//		iter->second->trigger(params);
//	}
//}

void PassiveSkillMgr::registerMovementEvent(bool bReg)
{
	//if (!m_pSkillComponent) { return; }
	//RoleComponent* pOwner = dynamic_cast<RoleComponent*>(m_pSkillComponent->getEntity());
	//if (!pOwner) { return; }
	//if (bReg)
	//{
	//	REGISTER_EVENT(pOwner, MovementComponentEvent, &PassiveSkillMgr::onMovementEvent);
	//}
	//else
	//{
	//	UNREGISTER_EVENT(pOwner, MovementComponentEvent);
	//}
}
//
//void PassiveSkillMgr::onMovementEvent(const MovementComponentEvent& evt)
//{
//	// 降落触发被动
//	auto&& range = m_mapPassives.equal_range(SkillType::TriggerType::TriggerTypeByFallingDist);
//	for (auto&& iter = range.first; iter != range.second; ++iter)
//	{
//		if (!iter->second->getTriggerEnable()) { continue; }
//		OtherParam params{};
//		//params.m_pMovementData = &evt;
//		iter->second->trigger(params);
//	}
//	// 移动触发被动
//	range = m_mapPassives.equal_range(SkillType::TriggerType::TriggerTypeByMoving);
//	for (auto&& iter = range.first; iter != range.second; ++iter)
//	{
//		if (!iter->second->getTriggerEnable()) { continue; }
//		OtherParam params{};
//		//params.m_pMovementData = &evt;
//		iter->second->trigger(params);
//	}
//	// 冲刺触发被动
//	range = m_mapPassives.equal_range(SkillType::TriggerType::TriggerTypeBySprint);
//	for (auto&& iter = range.first; iter != range.second; ++iter)
//	{
//		if (!iter->second->getTriggerEnable()) { continue; }
//		OtherParam params{};
//		//params.m_pMovementData = &evt;
//		iter->second->trigger(params);
//	}
//}

void PassiveSkillMgr::registerFollowEvent(bool bReg)
{
	//if (!m_pSkillComponent) { return; }
	//RoleComponent* pOwner = dynamic_cast<RoleComponent*>(m_pSkillComponent->getEntity());
	//if (!pOwner) { return; }
	//if (!dynamic_cast<PlayerComponent*>(m_pSkillComponent->getEntity())) return;
	//if (bReg)
	//{
	//	REGISTER_EVENT(pOwner, PassiveEvent_Follow_NpcOrPetChange, &PassiveSkillMgr::onFollowNpcOrPetChange);
	//}
	//else
	//{
	//	UNREGISTER_EVENT(pOwner, PassiveEvent_Follow_NpcOrPetChange);
	//}
}

void PassiveSkillMgr::onFollowNpcOrPetChange(const PassiveEvent_Follow_NpcOrPetChange& evt)
{
	// NPC宠物出战变更触发被动
	auto&& range = m_mapPassives.equal_range(SkillType::TriggerType::TriggerTypeByPetOrNpc);
	for (auto&& iter = range.first; iter != range.second; ++iter)
	{
		if (!iter->second->getTriggerEnable()) { continue; }
		OtherParam params{};
		params.m_pFollowNpcOrPetData = &evt;
		iter->second->trigger(params);
	}
}

void PassiveSkillMgr::registerPositionChange(bool bReg)
{
	//if (!m_pSkillComponent) { return; }
	//RoleComponent* pOwner = dynamic_cast<RoleComponent*>(m_pSkillComponent->getEntity());
	//if (!pOwner) { return; }
	//TransformComponent* pOwnerTransformComp = pOwner->findComponent<TransformComponent>();
	//if (!pOwnerTransformComp) return;
	//if (bReg)
	//{
	//	REGISTER_EVENT(pOwnerTransformComp, TransformSetPositionEvent, &PassiveSkillMgr::onPositionChange);
	//}
	//else
	//{
	//	UNREGISTER_EVENT(pOwnerTransformComp, TransformSetPositionEvent);
	//}
}

//void PassiveSkillMgr::onPositionChange(TransformSetPositionEvent evt)
//{
//	// NPC宠物出战变更触发被动
//	auto&& range = m_mapPassives.equal_range(SkillType::TriggerType::TriggerTypeByYAxisDepth);
//	for (auto&& iter = range.first; iter != range.second; ++iter)
//	{
//		if (!iter->second->getTriggerEnable()) { continue; }
//		OtherParam params{};
//		params.m_pPosChangeEvent = &evt;
//		iter->second->trigger(params);
//	}
//}

void PassiveSkillMgr::registerWaterInOutEvent(bool bReg)
{
	//if (!m_pSkillComponent) { return; }
	//RoleComponent* pOwner = dynamic_cast<RoleComponent*>(m_pSkillComponent->getEntity());
	//if (!pOwner) { return; }
	//if (bReg)
	//{
	//	REGISTER_EVENT(pOwner, PassiveEvent_WaterInOut, &PassiveSkillMgr::onWaterInOutChange);
	//}
	//else
	//{
	//	UNREGISTER_EVENT(pOwner, PassiveEvent_WaterInOut);
	//}
}

void PassiveSkillMgr::onWaterInOutChange(const PassiveEvent_WaterInOut& evt)
{
	auto&& range = m_mapPassives.equal_range(SkillType::TriggerType::TriggerTypeByWater);
	for (auto&& iter = range.first; iter != range.second; ++iter)
	{
		OtherParam param{};
		param.m_pWaterInOutState = &evt;
		iter->second->trigger(param);
	}
}

void PassiveSkillMgr::registerHpChangeEvent(bool bReg)
{
	//if (!m_pSkillComponent) { return; }
	//RoleComponent* pOwner = dynamic_cast<RoleComponent*>(m_pSkillComponent->getEntity());
	//if (!pOwner) { return; }
	//CharacterAttributeComponent* pOwnerCharacterAttributeComponent = pOwner->findComponent<CharacterAttributeComponent>();
	//if (!pOwnerCharacterAttributeComponent) return;
	//if (!pOwnerCharacterAttributeComponent->GetHpManager()) return;
	//if (bReg)
	//{
	//	REGISTER_EVENT(pOwnerCharacterAttributeComponent->GetHpManager(), HpEvent, &PassiveSkillMgr::onHpChangeEvent);
	//}
	//else
	//{
	//	UNREGISTER_EVENT(pOwnerCharacterAttributeComponent->GetHpManager(), HpEvent);
	//}
}

//void PassiveSkillMgr::onHpChangeEvent(const HpEvent& evt)
//{
//	// NPC宠物出战变更触发被动
//	auto&& range = m_mapPassives.equal_range(SkillType::TriggerType::TriggerTypeHpLess);
//	for (auto&& iter = range.first; iter != range.second; ++iter)
//	{
//		if (!iter->second->getTriggerEnable()) { continue; }
//		OtherParam params{};
//		params.m_pHpEvent = &evt;
//		iter->second->trigger(params);
//	}
//}

void PassiveSkillMgr::registerKillSomeoneEvent(bool bReg)
{
	//if (!m_pSkillComponent) { return; }
	//RoleComponent* pOwner = dynamic_cast<RoleComponent*>(m_pSkillComponent->getEntity());
	//if (!pOwner) { return; }
	//if (bReg)
	//{
	//	REGISTER_EVENT(pOwner, PassiveEvent_KillSomeone, &PassiveSkillMgr::onKillSomeone);
	//}
	//else
	//{
	//	UNREGISTER_EVENT(pOwner, PassiveEvent_KillSomeone);
	//}
}

void PassiveSkillMgr::onKillSomeone(const PassiveEvent_KillSomeone& evt)
{
	auto&& range = m_mapPassives.equal_range(static_cast<int>(SkillType::TriggerType::TriggerTypeCasterKillTarget));
	for (auto&& iter = range.first; iter != range.second; ++iter)
	{
		if (!iter->second->getTriggerEnable()) { continue; }
		OtherParam params{};
		params.m_pKillEvent = &evt;
		iter->second->trigger(params);
	}
	range = m_mapPassives.equal_range(static_cast<int>(SkillType::TriggerType::TriggerTypeCasterKilled));
	for (auto&& iter = range.first; iter != range.second; ++iter)
	{
		if (!iter->second->getTriggerEnable()) { continue; }
		OtherParam params{};
		params.m_pKillEvent = &evt;
		iter->second->trigger(params);
	}
}

void PassiveSkillMgr::registerMapObjInOutSight(bool bReg)
{
	//if (!m_pSkillComponent) { return; }
	//RoleComponent* pOwner = dynamic_cast<RoleComponent*>(m_pSkillComponent->getEntity());
	//if (!pOwner || !pOwner->getData()) { return; }
	//if (bReg)
	//{
	//	REGISTER_EVENT(pOwner, PassiveEvent_RoleInOutSight, &PassiveSkillMgr::onMapObjInOutSight);
	//	// 添加被动技能视野，P.S. 服务器限制，配置确定所有光环范围固定20，故所有光环技能统一添加一个 20x20的视野范围
	//	pOwner->getData()->setFOV(MapObj::FOV{ SkillCfg::AURA_FOV_RANGE, SkillCfg::AURA_FOV_RANGE }, MapObj::FOV_INDEX::INDEX_BUFF_RANGE);
	//}
	//else
	//{
	//	UNREGISTER_EVENT(pOwner, PassiveEvent_RoleInOutSight);
	//	// 移除被动技能视野
	//	pOwner->getData()->removeFOV(MapObj::FOV_INDEX::INDEX_BUFF_RANGE);
	//}
}

void PassiveSkillMgr::onMapObjInOutSight(const PassiveEvent_RoleInOutSight& evt)
{
	auto&& range = m_mapPassives.equal_range(static_cast<int>(SkillType::TriggerType::TriggerTypeAura));
	for (auto&& iter = range.first; iter != range.second; ++iter)
	{
		if (!iter->second->getTriggerEnable()) { continue; }
		OtherParam params{};
		params.m_pInOutSight = &evt;
		iter->second->trigger(params);
	}
}
