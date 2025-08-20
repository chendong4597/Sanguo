/*!
 * \file PassiveTrigger.cpp
 *
 * \author Dexuan Yang
 * \date 2019/10/04
 *
 *
 */

#include "stdafx.h"

#include "PassiveEvents.h"
#include "PassiveTrigger.h"
#include "PassiveSkillMgr.h"
#include "config/SkillCfg.h"
//#include "skill/SkillComponent.h"
//#include "skill/SpellComponent_Lua.h"
//#include "localserver/LocalServer_Battle.h"
//#include "events/SandboxEvents.h"
//#include "handler/HandlerSkill.h"
#include <data/ItemData.h>
//#include <data/DayNightData.h>
#include <data/MapObj.h>

#include <RandomUtil.h>
//#include "player/PlayerComponent.h"
//#include "npc/NpcComponent.h"
//#include "handler/HandlerRole.h"
//#include "pet/PetComponent.h"
//#include "ui/common/UiCommon.h"
//#include "handler/HandlerNpc.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

PassiveTrigger::PassiveTrigger(const SkillType* pSkillType, PassiveSkillMgr* pPassiveSkillMgr, NS::I64 nEquipmentUuid)
	: m_pSkillType(pSkillType)
	, m_pPassiveSkillMgr(pPassiveSkillMgr)
	, m_nEquipmentUuid(nEquipmentUuid)
	, m_eState(PassiveState::NONE)
{
}

PassiveTrigger::~PassiveTrigger()
{
	m_pSkillType = nullptr;
}

void PassiveTrigger::initialize(bool bForce/* = false*/)
{
	m_eState = PassiveState::INITIALIZED;
}

void PassiveTrigger::unInitialize()
{
	m_eState = PassiveState::NONE;
}

void PassiveTrigger::onDead()
{
	m_eState = PassiveState::UNTRIGGER;
}

void PassiveTrigger::onResurrection()
{
	initialize();
}

bool PassiveTrigger::trigger(const OtherParam& otherParam)
{
	// 被动触发条件
	PassiveTrigger::ConditionState eTriggerState = checkCondition(otherParam);

	if (PassiveTrigger::ConditionState::SUCCESS == eTriggerState)
	{
		// 触发前置条件
		if (checkPreCondition())
		{
			doTriggerAction(m_pPassiveSkillMgr->getCasterId(), otherParam);
		}
	}
	else if(PassiveTrigger::ConditionState::FAILED == eTriggerState)
	{
		if (m_pPassiveSkillMgr)
		{
			doUnTriggerAction(m_pPassiveSkillMgr->getCasterId(), otherParam);
		}
	}

	return true;
}

void PassiveTrigger::setTriggerEnable(bool bEnable)
{
	if (m_bTriggerEnable != bEnable)
	{
		m_bTriggerEnable = bEnable;
		bEnable ? initialize() : unInitialize();
	}
}

bool PassiveTrigger::checkPreCondition()
{
	if (!m_pSkillType || !m_pPassiveSkillMgr) { return false; }
	// 被动技能未被启用(装备相关，受装备位置和装备破损状态影响，非装备带来的被动技能总为true)
	if (!m_bTriggerEnable) { return false; }
	// 触发概率（被动技能才计算，主动技能视为百分百触发）
	if (m_pSkillType->IsPassivesSkill())
	{
		int nProbability = m_pSkillType->m_triggerParam1;
		if (NS::RandomUtil::getRandomInt(1, 100) > nProbability) { return false; }
	}
	// 技能施法条件(CD，MP，是否被沉默等)
	return m_pPassiveSkillMgr->canCastSkill(m_pSkillType);
}

int PassiveTrigger::getEquipmentPosIndex() const
{
	if (m_nEquipmentUuid == 0)
	{
		return 0;
	}
	std::shared_ptr<ItemData> pEquipmentData = ItemPacketMgr::getInstance().findItemDataByUuid(m_nEquipmentUuid);
	if (pEquipmentData)
	{
		// 武器位置为背包中的位置，不是装备位置，因此需要转换下
		//return pEquipmentData->getEquipPos() == MsgPB::EQUIP_PACKET_POS::EQUIP_PACKET_WEAPON ? pEquipmentData->getEquipPos() : pEquipmentData->getPos();
	}
	return 0;
}

int PassiveTrigger::getEquipmentCfgId() const
{
	if (m_nEquipmentUuid == 0)
	{
		return 0;
	}
	std::shared_ptr<ItemData> pEquipmentData = ItemPacketMgr::getInstance().findItemDataByUuid(m_nEquipmentUuid);
	if (pEquipmentData)
	{
		return pEquipmentData->getItemId();
	}
	return 0;
}

Role* PassiveTrigger::getCasterRoleData() const
{
	//const RoleComponent* pRoleComponent = m_pPassiveSkillMgr->getCasterRoleComponent();
	//if (!pRoleComponent) { return nullptr; }

	return m_pPassiveSkillMgr->getRole().get();
}

bool PassiveTrigger::checkDelayInitialize(const Role* pRoleData)
{
	// 外部需要检查pRoleData合法性
	return !pRoleData->isInitialized();
}

///////////////////////////////////////////////////////////////////////////////////////////
// n_trigger_type = 1, hp less
void PassiveTrigger_HpLess::initialize(bool bForce)
{
	const Role* pRoleData = getCasterRoleData();
	if (!m_pSkillType || !pRoleData) return;
	if (checkDelayInitialize(pRoleData) && !bForce)
	{
		// map load no finish yet or role data not init finish yet.
		m_pPassiveSkillMgr->pushDelayInitialize(this);
		return;
	}
	//auto p_casterRoleComponent = m_pPassiveSkillMgr->getCasterRoleComponent();
	//if (p_casterRoleComponent == nullptr) return;
	//CharacterAttributeComponent* p_casterCharacterAttributeComponent = p_casterRoleComponent->findComponent<CharacterAttributeComponent>();
	//if (p_casterCharacterAttributeComponent == nullptr) return;

	//NS::int32 nCasterMapId = m_pPassiveSkillMgr->getCasterId();
	//OtherParam initParam{};
	//HpEvent evt;
	//evt.hp = p_casterCharacterAttributeComponent->getHp();
	//evt.maxHp = p_casterCharacterAttributeComponent->getMaxHp();
	//initParam.m_pHpEvent = &evt;
	//trigger(initParam);
}

void PassiveTrigger_HpLess::unInitialize()
{
	NS::int32 nCasterMapId = m_pPassiveSkillMgr->getCasterId();
	OtherParam noneParam{};
	doUnTriggerAction(nCasterMapId, noneParam);
}

PassiveTrigger::ConditionState PassiveTrigger_HpLess::checkCondition(const OtherParam& otherParam)
{
	//if (!m_pSkillType) return PassiveTrigger::ConditionState::NONE;
	//if(otherParam.m_pHpEvent->maxHp == 0) return PassiveTrigger::ConditionState::NONE;
	//if (otherParam.m_pHpEvent->hp * 100 / otherParam.m_pHpEvent->maxHp < m_pSkillType->m_triggerParam2)
	//{
	//	return PassiveTrigger::ConditionState::SUCCESS;
	//}
	return PassiveTrigger::ConditionState::FAILED;
}

void PassiveTrigger_HpLess::doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{
	if (m_eState == PassiveState::TRIGGERING)
	{
		return;
	}
	std::shared_ptr<MapObj> pCasterMapObj = MapObjMgr::getInstance().findObj(nCasterMapObjId);
	if (!pCasterMapObj) { return; }
	// 标记被动技能触发中
	m_eState = PassiveState::TRIGGERING;

	// push 触发被动技能指令
	PassiveSkillMgr::PassiveCommand command{};
	command.m_pSkillType = m_pSkillType;
	if (m_pSkillType->m_skillType == SkillType::Type::Group)
	{
		command.m_targetPosition = pCasterMapObj->getPos();
	}
	else
	{
		command.m_vecTargetMapObjIds.push_back(nCasterMapObjId);
	}
	command.m_bTrigger = true;
	m_pPassiveSkillMgr->pushActionCommand(command, false);
}

void PassiveTrigger_HpLess::doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{
	if (m_eState != PassiveState::TRIGGERING)
	{
		return;
	}
	m_eState = PassiveState::UNTRIGGER;
	// push 解除被动技能指令
	PassiveSkillMgr::PassiveCommand command{};
	command.m_pSkillType = m_pSkillType;
	command.m_vecTargetMapObjIds.push_back(nCasterMapObjId);
	command.m_bTrigger = false;
	m_pPassiveSkillMgr->pushActionCommand(command, false);
}

///////////////////////////////////////////////////////////////////////////////////////////
// n_trigger_type = 2, Damage reduction, before damage calculation
PassiveTrigger::ConditionState PassiveTrigger_DamageReduction::checkCondition(const OtherParam& otherParam)
{
	return PassiveTrigger::ConditionState::SUCCESS;
}

void PassiveTrigger_DamageReduction::doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{
	if (m_eState == PassiveState::TRIGGERING)
	{
		return;
	}
	// 标记被动技能触发中
	m_eState = PassiveState::TRIGGERING;
	// push 触发被动技能指令
	PassiveSkillMgr::PassiveCommand command{};
	command.m_pSkillType = m_pSkillType;
	command.m_vecTargetMapObjIds.push_back(nCasterMapObjId);
	command.m_bTrigger = true;
	m_pPassiveSkillMgr->pushActionCommand(command, false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// n_trigger_type = 4 & 32, under normal / skill attack, remove buff
PassiveTrigger::ConditionState PassiveTrigger_BeAtkRemoveBuff::checkCondition(const OtherParam& otherParam)
{
	// TODO 弱联网暂不实现
	return PassiveTrigger::ConditionState::NONE;
}

void PassiveTrigger_BeAtkRemoveBuff::doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{
	// TODO 弱联网暂不实现
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// n_trigger_type = 8 & 64, under normal / skill attack, cast skill
PassiveTrigger::ConditionState PassiveTrigger_BeAtkCastSkill::checkCondition(const OtherParam& otherParam)
{
	if (!m_pSkillType)
	{
		return PassiveTrigger::ConditionState::NONE;
	}

	const PassiveEvent_BeAtk* pBeAtkEvent = otherParam.m_pBeAtkEvent;
	if (!pBeAtkEvent)
	{
		return PassiveTrigger::ConditionState::NONE;
	}
	if (pBeAtkEvent->m_nDamageValue <= 0)
	{
		// 回复血值，不触发被动技能
		return PassiveTrigger::ConditionState::NONE;
	}
	// 受击来源技能ID，ID == 0 为普攻
	NS::int32 nSkillId = pBeAtkEvent->m_nSkillId;
	if (nSkillId == 0)
	{
		if ((m_pSkillType->m_triggerType & SkillType::TriggerType::TriggerTypeUnderAttackCastSkill) == SkillType::TriggerType::TriggerTypeUnderAttackCastSkill)
		{
			return PassiveTrigger::ConditionState::SUCCESS;
		}
	}
	else
	{
		// 受击来源技能为被动技能，不触发被动
		const SkillType* pSkillType = SkillCfg::getInstance().findSkillType(nSkillId);
		if (!pSkillType || pSkillType->IsPassivesSkill())
		{
			return PassiveTrigger::ConditionState::NONE;
		}

		if ((m_pSkillType->m_triggerType & SkillType::TriggerType::TriggerTypeUnderSkillCastSkill) == SkillType::TriggerType::TriggerTypeUnderSkillCastSkill)
		{
			return PassiveTrigger::ConditionState::SUCCESS;
		}
	}

	return PassiveTrigger::ConditionState::NONE;
}

void PassiveTrigger_BeAtkCastSkill::doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{// 一次性被动效果，不需要记录state
	// 受击参数
	const PassiveEvent_BeAtk* pBeAtkEvent = otherParam.m_pBeAtkEvent;
	if (!pBeAtkEvent) { return; }
	// 受击者 and 攻击者
	std::shared_ptr<MapObj> pHitMapObj = MapObjMgr::getInstance().findObj(pBeAtkEvent->m_nHitMapObjId);
	std::shared_ptr<MapObj> pAtkMapObj = MapObjMgr::getInstance().findObj(pBeAtkEvent->m_nAtkMapObjId);
	if (!pHitMapObj || !pAtkMapObj) { return; }
	// push 触发被动技能指令
	PassiveSkillMgr::PassiveCommand command{};
	command.m_pSkillType = m_pSkillType;
	if (m_pSkillType->m_skillType == SkillType::Type::Group)
	{
		command.m_targetPosition = pHitMapObj->getPos();
	}
	else if (m_pSkillType->m_targetType == SkillType::TargetType::Self)
	{
		command.m_vecTargetMapObjIds.push_back(pBeAtkEvent->m_nHitMapObjId);
	}
	else
	{
		command.m_vecTargetMapObjIds.push_back(pBeAtkEvent->m_nAtkMapObjId);
	}
	command.m_bTrigger = true;
	m_pPassiveSkillMgr->pushActionCommand(command);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// n_trigger_type = 16 & 128, under normal / skill attack, CounterAttack
PassiveTrigger::ConditionState PassiveTrigger_CounterAttack::checkCondition(const OtherParam& otherParam)
{
	if (!m_pSkillType)
	{
		return PassiveTrigger::ConditionState::NONE;
	}
	const PassiveEvent_BeAtk* pBeAtkEvent = otherParam.m_pBeAtkEvent;
	if (!pBeAtkEvent)
	{
		return PassiveTrigger::ConditionState::NONE;
	}
	if (pBeAtkEvent->m_nDamageValue <= 0)
	{
		// 回复血值，不触发被动技能
		return PassiveTrigger::ConditionState::NONE;
	}

	if (pBeAtkEvent->m_nAtkMapObjId <= 0)
	{
		// 反击技要求必须反击有目标
		return PassiveTrigger::ConditionState::NONE;
	}

	// 受击来源技能ID，ID == 0 为普攻
	NS::int32 nSkillId = pBeAtkEvent->m_nSkillId;
	if (nSkillId == 0)
	{
		if ((m_pSkillType->m_triggerType & SkillType::TriggerType::TriggerTypeUnderAttackFightBack) == SkillType::TriggerType::TriggerTypeUnderAttackFightBack)
		{
			return PassiveTrigger::ConditionState::SUCCESS;
		}
	}
	else
	{
		// 受击来源技能为被动技能，不触发被动
		const SkillType* pSkillType = SkillCfg::getInstance().findSkillType(nSkillId);
		if (!pSkillType || pSkillType->IsPassivesSkill())
		{
			return PassiveTrigger::ConditionState::NONE;
		}

		if ((m_pSkillType->m_triggerType & SkillType::TriggerType::TriggerTypeUnderSkillFightBack) == SkillType::TriggerType::TriggerTypeUnderSkillFightBack)
		{
			return PassiveTrigger::ConditionState::SUCCESS;
		}
	}
	return PassiveTrigger::ConditionState::NONE;
}

void PassiveTrigger_CounterAttack::doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{// 一次性被动效果，不需要记录state
	// 受击参数
	const PassiveEvent_BeAtk* pBeAtkEvent = otherParam.m_pBeAtkEvent;
	if (!pBeAtkEvent)
	{
		return;
	}
	// 受击者 and 攻击者
	std::shared_ptr<MapObj> pHitMapObj = MapObjMgr::getInstance().findObj(pBeAtkEvent->m_nHitMapObjId);
	std::shared_ptr<MapObj> pAtkMapObj = MapObjMgr::getInstance().findObj(pBeAtkEvent->m_nAtkMapObjId);
	if (!pHitMapObj || !pAtkMapObj)
	{
		return;
	}
	// push 触发被动技能指令
	PassiveSkillMgr::PassiveCommand command{};
	command.m_eType = PassiveSkillMgr::PassiveCommand::Type::COUNTERATK;
	command.m_pSkillType = m_pSkillType;
	command.m_vecTargetMapObjIds.push_back(pBeAtkEvent->m_nAtkMapObjId); // 反击目标必定是攻击者
	command.m_bTrigger = true;
	command.m_nHurtValue = pBeAtkEvent->m_nDamageValue;
	m_pPassiveSkillMgr->pushActionCommand(command);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// n_trigger_type = 256 & 1024, normal / skill attack, bloodsucking
PassiveTrigger::ConditionState PassiveTrigger_BloodSucking::checkCondition(const OtherParam& otherParam)
{
	if (!m_pSkillType)
	{
		return PassiveTrigger::ConditionState::NONE;
	}
	const PassiveEvent_Hit* pHitEvent = otherParam.m_pHitEvent;
	if (!pHitEvent)
	{
		return PassiveTrigger::ConditionState::NONE;
	}
	if (pHitEvent->m_nDamageValue <= 0)
	{
		// 回复血值，不触发被动技能
		return PassiveTrigger::ConditionState::NONE;
	}

	if (pHitEvent->m_nHitMapObjId <= 0)
	{
		// 吸血技要求必须有吸血目标
		return PassiveTrigger::ConditionState::NONE;
	}

	// 来源技能ID，ID == 0 为普攻
	NS::int32 nSkillId = pHitEvent->m_nSkillId;
	if (nSkillId == 0)
	{
		if ((m_pSkillType->m_triggerType & SkillType::TriggerType::TriggerTypeAttackBloodSucking) == SkillType::TriggerType::TriggerTypeAttackBloodSucking)
		{
			return PassiveTrigger::ConditionState::SUCCESS;
		}
	}
	else
	{
		// 来源技能为被动技能，不触发被动
		const SkillType* pSkillType = SkillCfg::getInstance().findSkillType(nSkillId);
		if (!pSkillType || pSkillType->IsPassivesSkill())
		{
			return PassiveTrigger::ConditionState::NONE;
		}

		if ((m_pSkillType->m_triggerType & SkillType::TriggerType::TriggerTypeCastingBloodSucking) == SkillType::TriggerType::TriggerTypeCastingBloodSucking)
		{
			return PassiveTrigger::ConditionState::SUCCESS;
		}
	}
	return PassiveTrigger::ConditionState::NONE;
}

void PassiveTrigger_BloodSucking::doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{// 一次性被动效果，不需要记录state
	// 攻击参数
	const PassiveEvent_Hit* pHitEvent = otherParam.m_pHitEvent;
	if (!pHitEvent) { return; }
	// 攻击者 and 受击者
	std::shared_ptr<MapObj> pAtkMapObj = MapObjMgr::getInstance().findObj(pHitEvent->m_nAtkMapObjId);
	std::shared_ptr<MapObj> pHitMapObj = MapObjMgr::getInstance().findObj(pHitEvent->m_nHitMapObjId);
	if (!pHitMapObj || !pAtkMapObj) { return; }
	// push 触发被动技能指令
	PassiveSkillMgr::PassiveCommand command{};
	command.m_eType = PassiveSkillMgr::PassiveCommand::Type::SUCKING;
	command.m_pSkillType = m_pSkillType;
	command.m_vecTargetMapObjIds.push_back(pHitEvent->m_nAtkMapObjId); // 吸血技施法目标必定是受击者，效果目标是施法者自己
	command.m_bTrigger = true;
	command.m_nHurtValue = pHitEvent->m_nDamageValue;
	m_pPassiveSkillMgr->pushActionCommand(command);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// n_trigger_type = 512 & 2048, normal / skill attack, cast skill
PassiveTrigger::ConditionState PassiveTrigger_AtkCastSkill::checkCondition(const OtherParam& otherParam)
{
	if (!m_pSkillType)
	{
		return PassiveTrigger::ConditionState::NONE;
	}
	const PassiveEvent_Attack* pAtkEvent = otherParam.m_pAtkEvent;
	if (!pAtkEvent)
	{
		return PassiveTrigger::ConditionState::NONE;
	}
	// 来源技能ID，ID == 0 为普攻
	NS::int32 nSkillId = pAtkEvent->m_nSkillId;
	if (nSkillId == 0)
	{
		if ((m_pSkillType->m_triggerType & SkillType::TriggerType::TriggerTypeAttackCastSkill) == SkillType::TriggerType::TriggerTypeAttackCastSkill)
		{
			return PassiveTrigger::ConditionState::SUCCESS;
		}
	}
	else
	{
		// 来源技能为被动技能，不触发被动
		const SkillType* pSkillType = SkillCfg::getInstance().findSkillType(nSkillId);
		if (!pSkillType || pSkillType->IsPassivesSkill())
		{
			return PassiveTrigger::ConditionState::NONE;
		}

		if ((m_pSkillType->m_triggerType & SkillType::TriggerType::TriggerTypeCastingCastSkill) == SkillType::TriggerType::TriggerTypeCastingCastSkill)
		{
			return PassiveTrigger::ConditionState::SUCCESS;
		}
	}
	return PassiveTrigger::ConditionState::NONE;
}

void PassiveTrigger_AtkCastSkill::doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{// 一次性被动效果，不需要记录state
	// 攻击参数
	const PassiveEvent_Attack* pAtkEvent = otherParam.m_pAtkEvent;
	if (!pAtkEvent) { return; }
	std::shared_ptr<MapObj> pAtkMapObj = MapObjMgr::getInstance().findObj(pAtkEvent->m_nAtkMapObjId);
	if (!pAtkMapObj) { return; }
	// push 触发被动技能指令
	PassiveSkillMgr::PassiveCommand command{};
	command.m_pSkillType = m_pSkillType;
	if (m_pSkillType->m_skillType == SkillType::Type::Group)
	{
		command.m_targetPosition = pAtkMapObj->getPos();	// 攻击者位置，一般是召唤类型技能
	}
	else if (m_pSkillType->m_targetType == SkillType::TargetType::Self)
	{
		command.m_vecTargetMapObjIds.push_back(pAtkEvent->m_nAtkMapObjId);
	}
	else if (pAtkEvent->m_vecHitMapObjIds.empty())
	{
		// 施法目标对敌方但没有目标
		return;
	}
	else
	{
		for (size_t i = 0; i < pAtkEvent->m_vecHitMapObjIds.size(); i++)
		{
			int nTargetId = pAtkEvent->m_vecHitMapObjIds[i];
			command.m_vecTargetMapObjIds.push_back(nTargetId);
		}
	}
	command.m_bTrigger = true;
	m_pPassiveSkillMgr->pushActionCommand(command);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// n_trigger_type = 32768, no condition

void PassiveTrigger_ByNothing::initialize(bool bForce/* = false*/)
{
	const Role* pRoleData = getCasterRoleData();
	if (!m_pSkillType || !pRoleData) return;
	if (checkDelayInitialize(pRoleData) && !bForce)
	{
		// map load no finish yet or role data not init finish yet.
		m_pPassiveSkillMgr->pushDelayInitialize(this);
		return;
	}
	OtherParam noneParam{};
	trigger(noneParam);
}

void PassiveTrigger_ByNothing::unInitialize()
{
	NS::int32 nCasterMapId = m_pPassiveSkillMgr->getCasterId();
	OtherParam noneParam{};
	// 卸载被动技能效果
	doUnTriggerAction(nCasterMapId, noneParam);
}

PassiveTrigger::ConditionState PassiveTrigger_ByNothing::checkCondition(const OtherParam& otherParam)
{
	// 无触发条件
	return PassiveTrigger::ConditionState::SUCCESS;
}

bool PassiveTrigger_ByNothing::checkPreCondition()
{
	if (!PassiveTrigger::checkPreCondition()) { return false; }
	// 无触发类型的被动技能，施法作用对象只允许是自己
	return (m_pSkillType->m_targetType & SkillType::TargetType::Self) == SkillType::TargetType::Self;
}

void PassiveTrigger_ByNothing::doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{
	// 触发时间内持续效果，避免重复触发
	if (m_eState == PassiveState::TRIGGERING)
	{
		return;
	}
	// 标记被动技能触发中
	m_eState = PassiveState::TRIGGERING;
	// push 触发被动技能指令
	PassiveSkillMgr::PassiveCommand command{};
	command.m_pSkillType = m_pSkillType;
	command.m_vecTargetMapObjIds.push_back(nCasterMapObjId);	
	command.m_bTrigger = true;
	m_pPassiveSkillMgr->pushActionCommand(command);
}

void PassiveTrigger_ByNothing::doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{
	if (m_eState != PassiveState::TRIGGERING)
	{
		return;
	}
	m_eState = PassiveState::UNTRIGGER;
	// push 解除被动技能指令
	PassiveSkillMgr::PassiveCommand command{};
	command.m_pSkillType = m_pSkillType;
	command.m_vecTargetMapObjIds.push_back(nCasterMapObjId);	// 时间触发类型的被动技能，施法作用对象只允许是自己
	command.m_bTrigger = false;
	m_pPassiveSkillMgr->pushActionCommand(command);

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// n_trigger_type = 262144, game clock change

void PassiveTrigger_ByClock::initialize(bool bForce/* = false*/)
{
	const Role* pRoleData = getCasterRoleData();
	if (!m_pSkillType || !pRoleData) return;
	if (checkDelayInitialize(pRoleData) && !bForce)
	{
		// map load no finish yet or role data not init finish yet.
		m_pPassiveSkillMgr->pushDelayInitialize(this);
		return;
	}
	//LocalServer_Battle_Clock evt;
	//evt.m_clock = 5;//DayNightData::getInstance().getNowTime().m_clock;
	//OtherParam param{};
	//param.m_pClockData = &evt;
	//// 检查被动技能是否应该触发
	//trigger(param);
}

void PassiveTrigger_ByClock::unInitialize()
{
	NS::int32 nCasterMapId = m_pPassiveSkillMgr->getCasterId();
	//LocalServer_Battle_Clock evt;
	//evt.m_clock = 5;//DayNightData::getInstance().getNowTime().m_clock;
	//OtherParam param{};
	//param.m_pClockData = &evt;
	//// 卸载被动技能效果
	//doUnTriggerAction(nCasterMapId, param);
}

PassiveTrigger::ConditionState PassiveTrigger_ByClock::checkCondition(const OtherParam& otherParam)
{
	if (!m_pSkillType)
	{
		return PassiveTrigger::ConditionState::FAILED;
	}
	//const LocalServer_Battle_Clock* pClockEvent = otherParam.m_pClockData;
	//if (!pClockEvent)
	//{
	//	return PassiveTrigger::ConditionState::FAILED;
	//}
	//int nBeginHour = m_pSkillType->m_triggerParam2;
	//int nEndHour = m_pSkillType->m_triggerParam3;
	//int nNowHour = pClockEvent->m_clock;
	//if (nEndHour < nBeginHour) // 跨天 + 24；
	//{
	//	nEndHour = nEndHour + 24;
	//} 
	//if (nEndHour > 24 && nNowHour < m_pSkillType->m_triggerParam3)
	//{
	//	nNowHour = nNowHour + 24;
	//}
	//if (nNowHour >= nBeginHour && nNowHour < nEndHour)
	//{
	//	return PassiveTrigger::ConditionState::SUCCESS;
	//}
	return PassiveTrigger::ConditionState::FAILED;
}

bool PassiveTrigger_ByClock::checkPreCondition()
{
	if (!PassiveTrigger::checkPreCondition()) { return false; }
	// 时间触发类型的被动技能，施法作用对象只允许是自己
	return (m_pSkillType->m_targetType & SkillType::TargetType::Self) == SkillType::TargetType::Self;
}

void PassiveTrigger_ByClock::doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{
	// 触发时间内持续效果，避免重复触发
	if (m_eState == PassiveState::TRIGGERING)
	{
		return;
	}
	// 标记被动技能触发中
	m_eState = PassiveState::TRIGGERING;
	// push 触发被动技能指令
	PassiveSkillMgr::PassiveCommand command{};
	command.m_pSkillType = m_pSkillType;
	command.m_vecTargetMapObjIds.push_back(nCasterMapObjId);	// 时间触发类型的被动技能，施法作用对象只允许是自己
	command.m_bTrigger = true;
	m_pPassiveSkillMgr->pushActionCommand(command);
}

void PassiveTrigger_ByClock::doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{
	if (m_eState != PassiveState::TRIGGERING)
	{
		return;
	}
	m_eState = PassiveState::UNTRIGGER;
	// push 解除被动技能指令
	PassiveSkillMgr::PassiveCommand command{};
	command.m_pSkillType = m_pSkillType;
	command.m_vecTargetMapObjIds.push_back(nCasterMapObjId);	// 时间触发类型的被动技能，施法作用对象只允许是自己
	command.m_bTrigger = false;
	m_pPassiveSkillMgr->pushActionCommand(command);

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// n_trigger_type = 524288, role is falling

void PassiveTrigger_ByFalling::unInitialize()
{
	// 卸载被动技能效果
	NS::int32 nCasterMapId = m_pPassiveSkillMgr->getCasterId();
	OtherParam param{};
	doUnTriggerAction(nCasterMapId, param);
}

void PassiveTrigger_ByFalling::onDead()
{
	if (isTriggering())
	{
		// 额外通知技能流程结束，避免卡死
		//LuaSkillEvent evt{};
		//evt.setType(LuaSkillEvent::EventType::CasterLand);
		//m_pPassiveSkillMgr->notifySkillEndEvent(evt);
	}
	m_eState = PassiveState::UNTRIGGER;
}

PassiveTrigger::ConditionState PassiveTrigger_ByFalling::checkCondition(const OtherParam& otherParam)
{
	//if (!m_pSkillType) { return PassiveTrigger::ConditionState::FAILED; }
	//const MovementComponentEvent* pMovementData = otherParam.m_pMovementData;
	//if (!pMovementData) { return PassiveTrigger::ConditionState::FAILED; }

	//switch (pMovementData->m_movingType)
	//{
	//case MovementComponentEvent::MovingType::JumpDown:					// fall begin
	//{
	//	m_bBeginFalling = true;
	//	m_curDist = 0;
	//}
	//	break;

	//case MovementComponentEvent::MovingType::GridPosChanged:			// fall
	//{
	//	if (!m_bBeginFalling) { break; }
	//	if (pMovementData->m_movingDir.y >= 0.f) { break; }
	//	m_curDist += static_cast<int>(std::abs(pMovementData->m_movingDir.y));
	//}
	//	break;

	//case MovementComponentEvent::MovingType::Land:						// fall end or teminate
	//case MovementComponentEvent::MovingType::Teleport:
	//case MovementComponentEvent::MovingType::TerminateFallingSlow:
	//{
	//	m_bBeginFalling = false;
	//	m_curDist = 0;
	//}
	//	break;

	//default:
	//	break;
	//}

	//if (m_bBeginFalling && m_curDist >= m_pSkillType->m_triggerParam2)
	//{
	//	return PassiveTrigger::ConditionState::SUCCESS;
	//}
	return PassiveTrigger::ConditionState::FAILED;
}

bool PassiveTrigger_ByFalling::checkPreCondition()
{
	if (!PassiveTrigger::checkPreCondition()) { return false; }
	// 降落触发类型的被动技能，施法作用对象只允许是自己
	return (m_pSkillType->m_targetType & SkillType::TargetType::Self) == SkillType::TargetType::Self;
}

void PassiveTrigger_ByFalling::doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{
	// 触发时间内持续效果，避免重复触发
	if (m_eState == PassiveState::TRIGGERING)
	{
		return;
	}
	// 标记被动技能触发中
	m_eState = PassiveState::TRIGGERING;
	// push 触发被动技能指令
	PassiveSkillMgr::PassiveCommand command{};
	command.m_pSkillType = m_pSkillType;
	command.m_vecTargetMapObjIds.push_back(nCasterMapObjId);
	command.m_bTrigger = true;
	m_pPassiveSkillMgr->pushActionCommand(command, true);
}

void PassiveTrigger_ByFalling::doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{
	if (m_eState != PassiveState::TRIGGERING)
	{
		return;
	}
	m_eState = PassiveState::UNTRIGGER;
	// push 解除被动技能指令
	PassiveSkillMgr::PassiveCommand command{};
	command.m_pSkillType = m_pSkillType;
	command.m_vecTargetMapObjIds.push_back(nCasterMapObjId);
	command.m_bTrigger = false;
	m_pPassiveSkillMgr->pushActionCommand(command, true);

	// 额外操作
	//LuaSkillEvent evt{};
	//evt.setType(LuaSkillEvent::EventType::CasterLand);
	//m_pPassiveSkillMgr->notifySkillEndEvent(evt);

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// n_trigger_type = 1048576, role is in water

void PassiveTrigger_InWater::initialize(bool bForce/* = false*/)
{
	if (!m_pSkillType || m_pSkillType->m_triggerType != SkillType::TriggerType::TriggerTypeByWater) { return; }
	if (!m_pPassiveSkillMgr) { return; }

	//const RoleComponent* pRoleComponent = m_pPassiveSkillMgr->getCasterRoleComponent();
	//if (!pRoleComponent) { return; }

	//std::shared_ptr<Role> pRoleData = pRoleComponent->getData();
	//if (!pRoleData) { return; }

	//if (m_pSkillType->m_triggerParam2 == 1)
	//{
	//	// 水下呼吸效果，设置水下呼吸深度属性值
	//	int nBreathDeep = m_pSkillType->m_triggerParam3;
	//	pRoleData->setWaterBreathDeep(nBreathDeep);
	//}
}

void PassiveTrigger_InWater::unInitialize()
{
	if (!m_pSkillType || m_pSkillType->m_triggerType != SkillType::TriggerType::TriggerTypeByWater) { return; }
	if (!m_pPassiveSkillMgr) { return; }

	//const RoleComponent* pRoleComponent = m_pPassiveSkillMgr->getCasterRoleComponent();
	//if (!pRoleComponent) { return; }

	//std::shared_ptr<Role> pRoleData = pRoleComponent->getData();
	//if (!pRoleData) { return; }

	//if (m_pSkillType->m_triggerParam2 == 1)
	//{
	//	// 水下呼吸效果，设置水下呼吸深度属性值
	//	pRoleData->setWaterBreathDeep(0);
	//}
	//// 卸载被动技能效果
	//NS::int32 nCasterMapId = m_pPassiveSkillMgr->getCasterId();
	//OtherParam param{};
	//doUnTriggerAction(nCasterMapId, param);
}

PassiveTrigger::ConditionState PassiveTrigger_InWater::checkCondition(const OtherParam& otherParam)
{
	if (!m_pSkillType) { return PassiveTrigger::ConditionState::NONE; }

	const PassiveEvent_WaterInOut* pWaterInOutState = otherParam.m_pWaterInOutState;
	if (!pWaterInOutState)
	{
		return PassiveTrigger::ConditionState::NONE;
	}

	if (m_pSkillType->m_triggerParam2 == 1)
	{
		if (pWaterInOutState->m_eState == PassiveEvent_WaterInOut::State::BREATH_IN)
		{
			return PassiveTrigger::ConditionState::SUCCESS;
		}
		else if (pWaterInOutState->m_eState == PassiveEvent_WaterInOut::State::BREATH_OUT)
		{
			return PassiveTrigger::ConditionState::FAILED;
		}
	}
	else
	{
		if (pWaterInOutState->m_eState == PassiveEvent_WaterInOut::State::WATER_IN)
		{
			return PassiveTrigger::ConditionState::SUCCESS;
		}
		else if (pWaterInOutState->m_eState == PassiveEvent_WaterInOut::State::WATER_OUT)
		{
			return PassiveTrigger::ConditionState::FAILED;
		}
	}

	return PassiveTrigger::ConditionState::NONE;
}

bool PassiveTrigger_InWater::checkPreCondition()
{
	if (!PassiveTrigger::checkPreCondition()) { return false; }
	// 出入水触发类型的被动技能，施法作用对象只允许是自己
	return (m_pSkillType->m_targetType & SkillType::TargetType::Self) == SkillType::TargetType::Self;
}

void PassiveTrigger_InWater::doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{
	// 触发时间内持续效果，避免重复触发
	if (m_eState == PassiveState::TRIGGERING)
	{
		return;
	}
	// 标记被动技能触发中
	m_eState = PassiveState::TRIGGERING;
	// push 触发被动技能指令
	PassiveSkillMgr::PassiveCommand command{};
	command.m_pSkillType = m_pSkillType;
	command.m_vecTargetMapObjIds.push_back(nCasterMapObjId);	// 水下触发类型的被动技能，施法作用对象只允许是自己
	command.m_bTrigger = true;
	m_pPassiveSkillMgr->pushActionCommand(command);
}

void PassiveTrigger_InWater::doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{
	if (m_eState != PassiveState::TRIGGERING)
	{
		return;
	}
	m_eState = PassiveState::UNTRIGGER;
	// push 解除被动技能指令
	PassiveSkillMgr::PassiveCommand command{};
	command.m_pSkillType = m_pSkillType;
	command.m_vecTargetMapObjIds.push_back(nCasterMapObjId);	// 时间触发类型的被动技能，施法作用对象只允许是自己
	command.m_bTrigger = false;
	m_pPassiveSkillMgr->pushActionCommand(command);

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// n_trigger_type = 2097152, role is moving
void PassiveTrigger_ByMoving::unInitialize()
{
	NS::int32 nCasterMapId = m_pPassiveSkillMgr->getCasterId();
	OtherParam noneParam{};
	doUnTriggerAction(nCasterMapId, noneParam);
}

void PassiveTrigger_ByMoving::onDead()
{
	if (isTriggering())
	{
		// 额外操作
		//LuaSkillEvent evt;
		//evt.setType(LuaSkillEvent::GridPosChangedEnd);
		//m_pPassiveSkillMgr->notifySkillEndEvent(evt);
	}
	m_eState = PassiveState::UNTRIGGER;
}

PassiveTrigger::ConditionState PassiveTrigger_ByMoving::checkCondition(const OtherParam& otherParam)
{
	//if (!m_pSkillType) { return PassiveTrigger::ConditionState::FAILED; }
	//const MovementComponentEvent* pMovementData = otherParam.m_pMovementData;
	//if (!pMovementData) { return PassiveTrigger::ConditionState::FAILED; }

	//switch (pMovementData->m_movingType)
	//{
	//case MovementComponentEvent::MovingType::GridPosChanged:
	//	return PassiveTrigger::ConditionState::SUCCESS;
	//	break;
	//case MovementComponentEvent::MovingType::Unknown:
	//case MovementComponentEvent::MovingType::Standby:
	//case MovementComponentEvent::MovingType::Teleport:
	//	return PassiveTrigger::ConditionState::FAILED;
	//	break;
	//default:
	//	break;
	//}

	return PassiveTrigger::ConditionState::NONE;
}

void PassiveTrigger_ByMoving::doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{
	if (m_eState == PassiveState::TRIGGERING)
	{
		return;
	}
	// push 触发被动技能指令
	PassiveSkillMgr::PassiveCommand command{};
	command.m_pSkillType = m_pSkillType;
	command.m_vecTargetMapObjIds.push_back(nCasterMapObjId);
	command.m_bTrigger = true;
	m_pPassiveSkillMgr->pushActionCommand(command, false);
	// 标记被动技能触发中
	m_eState = PassiveState::TRIGGERING;
}

void PassiveTrigger_ByMoving::doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{
	if (m_eState != PassiveState::TRIGGERING)
	{
		return;
	}
	m_eState = PassiveState::UNTRIGGER;
	// push 解除被动技能指令
	PassiveSkillMgr::PassiveCommand command{};
	command.m_pSkillType = m_pSkillType;
	command.m_vecTargetMapObjIds.push_back(nCasterMapObjId);
	command.m_bTrigger = false;
	m_pPassiveSkillMgr->pushActionCommand(command, false);

	// 额外操作
	//LuaSkillEvent evt;
	//evt.setType(LuaSkillEvent::GridPosChangedEnd);
	//m_pPassiveSkillMgr->notifySkillEndEvent(evt);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// n_trigger_type = 2097152, role is sprinting
void PassiveTrigger_BySprinting::unInitialize()
{
	NS::int32 nCasterMapId = m_pPassiveSkillMgr->getCasterId();
	OtherParam noneParam{};
	doUnTriggerAction(nCasterMapId, noneParam);
}

void PassiveTrigger_BySprinting::onDead()
{
	if (isTriggering())
	{
		m_startRun = 0;
		//LuaSkillEvent evt;
		//evt.setType(LuaSkillEvent::StopSprint);
		//m_pPassiveSkillMgr->notifySkillEndEvent(evt);
	}
	m_eState = PassiveState::UNTRIGGER;
}

PassiveTrigger::ConditionState PassiveTrigger_BySprinting::checkCondition(const OtherParam& otherParam)
{
	//if (!m_pSkillType) { return PassiveTrigger::ConditionState::FAILED; }
	//const MovementComponentEvent* pMovementData = otherParam.m_pMovementData;
	//if (!pMovementData) { return PassiveTrigger::ConditionState::FAILED; }

	auto p_conditionState = PassiveTrigger::ConditionState::NONE;
	//switch (pMovementData->m_movingType)
	//{
	//case MovementComponentEvent::MovingType::Moving:
	//	if (pMovementData->m_isRunning)
	//	{
	//		if (m_startRun != 0)
	//		{
	//			auto p_now_time = ::time(nullptr);
	//			if (p_now_time - m_startRun >= m_pSkillType->m_triggerParam2)
	//			{
	//				p_conditionState = PassiveTrigger::ConditionState::SUCCESS;
	//			}
	//		}
	//		else
	//		{
	//			m_startRun = ::time(nullptr);
	//		}
	//	}
	//	else
	//	{
	//		p_conditionState = PassiveTrigger::ConditionState::FAILED;
	//	}
	//	break;
	//case MovementComponentEvent::MovingType::Standby:
	//	p_conditionState = PassiveTrigger::ConditionState::FAILED;
	//	break;
	//default:
	//	break;
	//}
	//auto p_casterRoleComponent = m_pPassiveSkillMgr->getCasterRoleComponent();
	//if (p_casterRoleComponent != nullptr)
	//{
	//	if (m_lastOrientationX != (int32)(p_casterRoleComponent->getOrientation())) {
	//		p_conditionState = PassiveTrigger::ConditionState::FAILED;
	//	}
	//	if (std::fabs(m_lastFpsWorldPos.y - p_casterRoleComponent->getWorldPos().y) > 0.001f)
	//	{
	//		m_startRun = 0;
	//	}
	//	m_lastOrientationX = (int32)(p_casterRoleComponent->getOrientation());
	//	m_lastFpsWorldPos = p_casterRoleComponent->getWorldPos();
	//}

	return p_conditionState;
}

void PassiveTrigger_BySprinting::doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{
	if (m_eState == PassiveState::TRIGGERING)
	{
		return;
	}
	// push 触发被动技能指令
	PassiveSkillMgr::PassiveCommand command{};
	command.m_pSkillType = m_pSkillType;
	command.m_vecTargetMapObjIds.push_back(nCasterMapObjId);
	command.m_bTrigger = true;
	m_pPassiveSkillMgr->pushActionCommand(command, false);
	// 标记被动技能触发中
	m_eState = PassiveState::TRIGGERING;
}

void PassiveTrigger_BySprinting::doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{
	if (m_eState != PassiveState::TRIGGERING)
	{
		return;
	}
	m_eState = PassiveState::UNTRIGGER;
	// push 解除被动技能指令
	PassiveSkillMgr::PassiveCommand command{};
	command.m_pSkillType = m_pSkillType;
	command.m_vecTargetMapObjIds.push_back(nCasterMapObjId);
	command.m_bTrigger = false;
	m_pPassiveSkillMgr->pushActionCommand(command, false);

	m_startRun = 0;
	//LuaSkillEvent evt;
	//evt.setType(LuaSkillEvent::StopSprint);
	//m_pPassiveSkillMgr->notifySkillEndEvent(evt);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// n_trigger_type = 8388608, npc or pet exit
void PassiveTrigger_ByNpcOrPet::initialize(bool bForce)
{
	const Role* pRoleData = getCasterRoleData();
	if (!m_pSkillType || !pRoleData) return;
	if (checkDelayInitialize(pRoleData) && !bForce)
	{
		// map load no finish yet or role data not init finish yet.
		m_pPassiveSkillMgr->pushDelayInitialize(this);
		return;
	}
	//auto p_casterRoleComponent = m_pPassiveSkillMgr->getCasterRoleComponent();
	//if (p_casterRoleComponent == nullptr) return;
	//const PlayerComponent* pPlayerComponent = dynamic_cast<const PlayerComponent*>(p_casterRoleComponent);
	//if (!pPlayerComponent)
	//{
	//	//如果是npc的情况下，只给当前npc自己加buff
	//	const NpcComponent* pNpcComponent = dynamic_cast<const NpcComponent*>(p_casterRoleComponent);
	//	if (pNpcComponent)
	//	{
	//		if (checkPreCondition())
	//		{
	//			auto&& npcData = pNpcComponent->getNpcData();
	//			if (npcData)
	//			{
	//				OtherParam p_otherParam{};
	//				PassiveEvent_Follow_NpcOrPetChange p_npcOrPetChangeData{};
	//				p_npcOrPetChangeData.m_npcMapObjId = npcData->getId();
	//				p_npcOrPetChangeData.m_bNpcChange = true;
	//				p_otherParam.m_pFollowNpcOrPetData = &p_npcOrPetChangeData;
	//				doTriggerAction(m_pPassiveSkillMgr->getCasterId(), p_otherParam);
	//			}
	//		}
	//	}
	//}
	//else
	//{
	//	OtherParam p_otherParam{};
	//	//当前出战的宠物
	//	PetComponent* pPetComponent = pPlayerComponent->getActivePet();
	//	PassiveEvent_Follow_NpcOrPetChange p_npcOrPetChangeData{};
	//	if (pPetComponent && pPetComponent->getData())
	//	{
	//		p_npcOrPetChangeData.m_npcMapObjId = pPetComponent->getData()->getId();
	//		p_npcOrPetChangeData.m_bNpcChange = true;
	//	}
	//	//当前出战的NPC
	//	int32 followNpcMapOjbId = UiCommon::getInstance().getFollowNpcObjId();
	//	if (followNpcMapOjbId > 0)
	//	{
	//		p_npcOrPetChangeData.m_petMapObjId = followNpcMapOjbId;
	//		p_npcOrPetChangeData.m_bPetChange = true;
	//	}
	//	p_otherParam.m_pFollowNpcOrPetData = &p_npcOrPetChangeData;
	//	// 检查被动技能是否应该触发
	//	trigger(p_otherParam);
	//}
}

void PassiveTrigger_ByNpcOrPet::unInitialize()
{
	NS::int32 nCasterMapId = m_pPassiveSkillMgr->getCasterId();
	OtherParam noneParam{};
	PassiveEvent_Follow_NpcOrPetChange pNoneNpcPetData{};
	pNoneNpcPetData.m_npcMapObjId = -1;
	pNoneNpcPetData.m_petMapObjId = -1;
	pNoneNpcPetData.m_bNpcChange = true;
	pNoneNpcPetData.m_bPetChange = true;
	noneParam.m_pFollowNpcOrPetData = &pNoneNpcPetData;
	doUnTriggerAction(nCasterMapId, noneParam);
}

PassiveTrigger::ConditionState PassiveTrigger_ByNpcOrPet::checkCondition(const OtherParam& otherParam)
{
	//auto p_casterRoleComponent = m_pPassiveSkillMgr->getCasterRoleComponent();
	//if (p_casterRoleComponent != nullptr)
	//{
	//	if (dynamic_cast<const PlayerComponent*>(p_casterRoleComponent))
	//	{
	//		return PassiveTrigger::ConditionState::SUCCESS;
	//	}
	//}
	return PassiveTrigger::ConditionState::NONE;
}

bool PassiveTrigger_ByNpcOrPet::checkPreCondition()
{
	return true;
}

void PassiveTrigger_ByNpcOrPet::doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{
	doUnTriggerAction(nCasterMapObjId, otherParam);
	if (m_eState == PassiveState::TRIGGERING)
	{
		return;
	}
	m_eState = PassiveState::TRIGGERING;
	PassiveSkillMgr::PassiveCommand command{};
	command.m_pSkillType = m_pSkillType;
	command.m_bTrigger = true;
	const PassiveEvent_Follow_NpcOrPetChange* p_npcOrPetData = otherParam.m_pFollowNpcOrPetData;
	if (p_npcOrPetData->m_bNpcChange)
	{
		if (p_npcOrPetData->m_npcMapObjId != m_lastNpcId)
		{
			command.m_vecTargetMapObjIds.push_back(p_npcOrPetData->m_npcMapObjId);
			m_lastNpcId = p_npcOrPetData->m_npcMapObjId;
		}
	}
	if (p_npcOrPetData->m_bPetChange)
	{
		if (p_npcOrPetData->m_petMapObjId != m_lastPetId)
		{
			command.m_vecTargetMapObjIds.push_back(p_npcOrPetData->m_petMapObjId);
			m_lastPetId = p_npcOrPetData->m_petMapObjId;
		}
	}
	if (command.m_vecTargetMapObjIds.size() > 0)
	{
		m_pPassiveSkillMgr->pushActionCommand(command, false);
	}
	
}

void PassiveTrigger_ByNpcOrPet::doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{
	if (m_eState != PassiveState::TRIGGERING)
	{
		return;
	}
	m_eState = PassiveState::UNTRIGGER;
	PassiveSkillMgr::PassiveCommand command{};
	command.m_pSkillType = m_pSkillType;
	command.m_bTrigger = false;
	const PassiveEvent_Follow_NpcOrPetChange* p_npcOrPetData = otherParam.m_pFollowNpcOrPetData;

	if (p_npcOrPetData->m_bNpcChange)
	{
		if (p_npcOrPetData->m_npcMapObjId != m_lastNpcId)
		{
			command.m_vecTargetMapObjIds.push_back(m_lastNpcId);
		}
	}

	if (p_npcOrPetData->m_bPetChange)
	{
		if (p_npcOrPetData->m_petMapObjId != m_lastPetId)
		{
			command.m_vecTargetMapObjIds.push_back(m_lastPetId);
		}
	}
	if (command.m_vecTargetMapObjIds.size() > 0)
	{
		m_pPassiveSkillMgr->pushActionCommand(command, false);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// n_trigger_type = 16777216, role position y
void PassiveTrigger_ByPositionY::initialize(bool bForce)
{
	const Role* pRoleData = getCasterRoleData();
	if (!m_pSkillType || !pRoleData) return;
	if (checkDelayInitialize(pRoleData) && !bForce)
	{
		// map load no finish yet or role data not init finish yet.
		m_pPassiveSkillMgr->pushDelayInitialize(this);
		return;
	}

	//auto p_casterRoleComponent = m_pPassiveSkillMgr->getCasterRoleComponent();
	//if (p_casterRoleComponent == nullptr) return;
	//TransformComponent * p_casterTransformComponent = p_casterRoleComponent->findComponent<TransformComponent>();
	//if (p_casterTransformComponent)
	//{
	//	OtherParam p_otherParam{};
	//	TransformSetPositionEvent p_posChangeEvent{};
	//	p_posChangeEvent.m_currPos = p_casterTransformComponent->getPosition();
	//	p_otherParam.m_pPosChangeEvent = &p_posChangeEvent;
	//	trigger(p_otherParam);
	//}
}

void PassiveTrigger_ByPositionY::unInitialize()
{
	NS::int32 nCasterMapId = m_pPassiveSkillMgr->getCasterId();
	OtherParam noneParam{};
	// 卸载被动技能效果
	doUnTriggerAction(nCasterMapId, noneParam);
}

PassiveTrigger::ConditionState PassiveTrigger_ByPositionY::checkCondition(const OtherParam& otherParam)
{
	//if (otherParam.m_pPosChangeEvent->m_currPos.y < m_pSkillType->m_triggerParam2)
	//{
	//	return PassiveTrigger::ConditionState::SUCCESS;
	//}
	return PassiveTrigger::ConditionState::FAILED;
}

void PassiveTrigger_ByPositionY::doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{
	if (m_eState == PassiveState::TRIGGERING)
	{
		return;
	}
	// 标记被动技能触发中
	m_eState = PassiveState::TRIGGERING;
	// push 触发被动技能指令
	PassiveSkillMgr::PassiveCommand command{};
	command.m_pSkillType = m_pSkillType;
	command.m_vecTargetMapObjIds.push_back(nCasterMapObjId);
	command.m_bTrigger = true;
	m_pPassiveSkillMgr->pushActionCommand(command, false); //当前帧触发，和变身有关，防止出错
}

void PassiveTrigger_ByPositionY::doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{
	if (m_eState != PassiveState::TRIGGERING)
	{
		return;
	}
	m_eState = PassiveState::UNTRIGGER;
	// push 解除被动技能指令
	PassiveSkillMgr::PassiveCommand command{};
	command.m_pSkillType = m_pSkillType;
	command.m_vecTargetMapObjIds.push_back(nCasterMapObjId);
	command.m_bTrigger = false;
	m_pPassiveSkillMgr->pushActionCommand(command, false); //当前帧触发,目标已死亡，当帧处理
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// n_trigger_type = 67108864, kill someone
PassiveTrigger::ConditionState PassiveTrigger_ByKillSomeone::checkCondition(const OtherParam& otherParam)
{
	auto killEvt = otherParam.m_pKillEvent;
	if (killEvt->m_nAtkMapObjId != m_pPassiveSkillMgr->getCasterId())
	{
		return PassiveTrigger::ConditionState::NONE;
	}
	return PassiveTrigger::ConditionState::SUCCESS;
}

void PassiveTrigger_ByKillSomeone::doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{
	// push 触发被动技能指令
	auto killEvt = otherParam.m_pKillEvent;
	std::shared_ptr<MapObj> pHitMapObj = MapObjMgr::getInstance().findObj(killEvt->m_nHitMapObjId);
	if (!pHitMapObj) { return; }
	PassiveSkillMgr::PassiveCommand command{};
	command.m_pSkillType = m_pSkillType;
	if (m_pSkillType->m_skillType == SkillType::Type::Group)
	{
		command.m_targetPosition = pHitMapObj->getPos();
	}
	else if (m_pSkillType->m_targetType == SkillType::TargetType::Self)
	{
		command.m_vecTargetMapObjIds.push_back(nCasterMapObjId);
	}
	else
	{
		command.m_vecTargetMapObjIds.push_back(killEvt->m_nHitMapObjId);
	}
	command.m_bTrigger = true;
	m_pPassiveSkillMgr->pushActionCommand(command, false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * n_trigger_type = 67108864 + 1, caster killed
 */

PassiveTrigger::ConditionState PassiveTrigger_Bykilled::checkCondition(const OtherParam& otherParam)
{
	auto killEvt = otherParam.m_pKillEvent;
	if (killEvt->m_nHitMapObjId != m_pPassiveSkillMgr->getCasterId())
	{
		return PassiveTrigger::ConditionState::NONE;
	}
	return PassiveTrigger::ConditionState::SUCCESS;
}

void PassiveTrigger_Bykilled::doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{
	// push 触发被动技能指令
	auto killEvt = otherParam.m_pKillEvent;

	PassiveSkillMgr::PassiveCommand command{};
	command.m_pSkillType = m_pSkillType;

	std::shared_ptr<MapObj> pHitMapObj = MapObjMgr::getInstance().findObj(killEvt->m_nHitMapObjId);
	if (m_pSkillType->m_skillType == SkillType::Type::Group)
	{
		command.m_targetPosition = pHitMapObj->getPos();
	}
	else if (m_pSkillType->m_targetType == SkillType::TargetType::Self)
	{
		command.m_vecTargetMapObjIds.push_back(nCasterMapObjId);
	}
	else
	{
		command.m_vecTargetMapObjIds.push_back(killEvt->m_nHitMapObjId);
	}
	command.m_bTrigger = true;
	m_pPassiveSkillMgr->pushActionCommand(command, false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// n_trigger_type = 268435456, aura

void PassiveTrigger_Aura::initialize(bool bForce/* = false*/)
{
	const Role* pRoleData = getCasterRoleData();
	if (!m_pSkillType || !pRoleData) return;
	if (checkDelayInitialize(pRoleData) && !bForce)
	{
		// map load no finish yet or role data not init finish yet.
		m_pPassiveSkillMgr->pushDelayInitialize(this);
		return;
	}

	Role* pCasterRole = getCasterRoleData();
	if (!pCasterRole) { return; }

	//std::set<MapObj*>* pSetMapObjInSight = MapObjMgr::getInstance().getAllObjsInSight(pCasterRole, MapObj::FOV_INDEX::INDEX_BUFF_RANGE);
	//if (!pSetMapObjInSight || pSetMapObjInSight->empty()) { return; }
	//PassiveEvent_RoleInOutSight evt{};
	//evt.m_eState = PassiveEvent_RoleInOutSight::State::GET_INTO;
	//for (MapObj* pMapObj : *pSetMapObjInSight)
	//{
	//	evt.m_vecMapObjIds.push_back(pMapObj->getId());
	//}

	//NS::int32 nCasterMapId = m_pPassiveSkillMgr->getCasterId();
	//OtherParam param{};
	//param.m_pInOutSight = &evt;
	//trigger(param);
}

void PassiveTrigger_Aura::unInitialize()
{
	Role* pCasterRole = getCasterRoleData();
	if (!pCasterRole) { return; }

	//std::set<MapObj*>* pSetMapObjInSight = MapObjMgr::getInstance().getAllObjsInSight(pCasterRole, MapObj::FOV_INDEX::INDEX_BUFF_RANGE);
	//if (!pSetMapObjInSight || pSetMapObjInSight->empty()) { return; }
	//PassiveEvent_RoleInOutSight evt{};
	//evt.m_eState = PassiveEvent_RoleInOutSight::State::GET_OUT;
	//for (MapObj* pMapObj : *pSetMapObjInSight)
	//{
	//	evt.m_vecMapObjIds.push_back(pMapObj->getId());
	//}

	//NS::int32 nCasterMapId = m_pPassiveSkillMgr->getCasterId();
	//OtherParam param{};
	//param.m_pInOutSight = &evt;
	//doUnTriggerAction(nCasterMapId, param);
}

void PassiveTrigger_Aura::onDead()
{
	unInitialize();
}

PassiveTrigger_Aura::ConditionState PassiveTrigger_Aura::checkCondition(const OtherParam& otherParam)
{
	const PassiveEvent_RoleInOutSight* pInOutSightEvent = otherParam.m_pInOutSight;
	if (!pInOutSightEvent) { return ConditionState::NONE; }

	if (pInOutSightEvent->m_eState == PassiveEvent_RoleInOutSight::State::GET_INTO)
	{
		return ConditionState::SUCCESS;
	}
	else if (pInOutSightEvent->m_eState == PassiveEvent_RoleInOutSight::State::GET_OUT)
	{
		return ConditionState::FAILED;
	}
	return ConditionState::NONE;
}

void PassiveTrigger_Aura::doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{
	const PassiveEvent_RoleInOutSight* pInOutSightEvent = otherParam.m_pInOutSight;
	if (!pInOutSightEvent) { return; }
	// push 解除被动技能指令
	PassiveSkillMgr::PassiveCommand command{};
	command.m_pSkillType = m_pSkillType;
	command.m_bTrigger = true;

	for (NS::int32 nMapObjId : pInOutSightEvent->m_vecMapObjIds)
	{
		if (!filterTarget(nMapObjId)) { continue; }
		command.m_vecTargetMapObjIds.push_back(nMapObjId);
	}

	if (!command.m_vecTargetMapObjIds.empty())
	{
		m_pPassiveSkillMgr->pushActionCommand(command, false);
	}
}

void PassiveTrigger_Aura::doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam)
{
	const PassiveEvent_RoleInOutSight* pInOutSightEvent = otherParam.m_pInOutSight;
	if (!pInOutSightEvent) { return; }
	// push 解除被动技能指令
	PassiveSkillMgr::PassiveCommand command{};
	command.m_pSkillType = m_pSkillType;
	command.m_bTrigger = false;

	for (NS::int32 nMapObjId : pInOutSightEvent->m_vecMapObjIds)
	{
		if (!filterTarget(nMapObjId)) { continue; }
		command.m_vecTargetMapObjIds.push_back(nMapObjId);
	}

	if (!command.m_vecTargetMapObjIds.empty())
	{
		m_pPassiveSkillMgr->pushActionCommand(command, false);
	}
}

bool PassiveTrigger_Aura::filterTarget(NS::int32 nMapObjId)
{
	if (!m_pSkillType) { return false; }
	const Role* pCasterRole = getCasterRoleData();
	if (!pCasterRole) { return false; }

	std::shared_ptr<MapObj> pMapObj = MapObjMgr::getInstance().findObj(nMapObjId);
	if (!pMapObj) { return false; }
	Role* pRoleData = dynamic_cast<Role*>(pMapObj.get());
	if (!pRoleData) { return false; }

	//bool bIsCastTarget = SkillComponent::filterTargetCategory(m_pSkillType->m_targetCategory, pMapObj.get());
	//if (!bIsCastTarget) { return false; }
	//// filter State
	//bIsCastTarget = SkillComponent::filterTargetState(pRoleData);
	//if (!bIsCastTarget) { return false; }
	//// filter GroupId
	//return SkillComponent::filterTargetGroup(m_pSkillType->m_targetType, pCasterRole, pRoleData);
	return false;
}