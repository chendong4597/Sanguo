/*!
* \file LocalServer_Battle.cpp
*
* \author zhichenxia
* \date 2018/5/16
*
*
*/
#include "stdafx.h"
#include "Rect.h"
#include "LocalServer_Battle.h"
#include <msg/MsgMapServer.pb.h>
#include <msg/MsgMob.pb.h>
#include "../handler/HandlerRole.h"
#include "../handler/HandlerLogin.h"
#include <data/role/Player.h>
#include <data/role/Player.h>
//#include <data/role/Npc.h>
//#include <data/SkillData.h>
#include <config/MonsterCfg.h>
//#include <config/TowerBuildingCfg.h>
#include <config/PlayerAttrCfg.h>
//#include <config/NpcActiveCfg.h>
//#include <common/SyncCache.h>
#include "LocalServer_Buff.h"
//#include "LocalServer_Achieve.h"
//#include "LocalServer_ItemPacket.h"
#include "BattleCalc.h"
//#include "LocalServer_Npc.h"
//#include "LocalServer_BirthMob.h"
//#include "LocalServer_Survival.h"
//#include <cli_build/BuildLogicCli.h>
//#include "LocalServer_Pet.h"
#include <data/MapData.h>
#include <data/MapDataMgr.h>
#include <RandomUtil.h>
//#include <skill/SkillComponent.h>
#include "data/role/skill/PassiveEvents.h"
//#include "skill/passive/PassiveSkillMgr.h"
//#include "map/building/BuildingMgr.h"
#include "config/BuildingCfg.h"
#include <config/BuffCfg.h>
#include "common/RandomData.h"
//#include "player/PlayerComponentMgr.h"
//#include "ui/common/UiCommon.h"
//#include "pet/PetComponent.h"
//#include "GameLog.h"
//#include "handler/HandlerPlayerCacheSync.h"
//#include "LocalServer_Survival_Endless.h"
#include <TimeUtil.h>
//#include "monster/MonsterComponentMgr.h"
//#include "utils/MapDataUtil.h"
//#include "LocalServer_Dummy.h"
#include "common/SkillBuffContainer.h"
//#include "ui/common/Trackball.h"
#include "../handler/HandlerItem.h"
#include "data/SkillData.h"
//#include "monster/MadAlchemistBossComponent.h"
//#include "data/AdventureTaskData.h"


using namespace MsgPB;

NS_BEGIN


LocalServer_Battle& LocalServer_Battle::getInstance(void)
{
	static LocalServer_Battle instance;
	return instance;
}

const NS::I64 INIT_SERIAL_ID = 10020030000000;
LocalServer_Battle::LocalServer_Battle(void)
{
	s_nSerialId = INIT_SERIAL_ID;
	// 玩家攻击。
	HOOK_MSG(onMsgPlayerNormalAttackC2S);
	// 玩家施放技能。
	HOOK_MSG(onMsgSkillCastSkillC2GS);

	// 拦截前置施法消息。
	//HOOK[](const MsgSkillCastSkillFrontC2S&) {};
	// 拦截被动技能触发消息
	//HOOK[](const MsgSkillActivePassiveC2S&) {};
	// 拦截被动技能移除消息
	//HOOK[](const MsgSkillInactivePassiveC2S&) {};


	//UNHOOK_MSG(onMsgPlayerNormalAttackC2S);
	//UNHOOK(MsgPlayerNormalAttackC2S);

	m_nLastBattleTime = std::chrono::steady_clock().now();
}

void LocalServer_Battle::reduceEndurance(int32 target_id)
{
	//### ChenBK 被攻击 扣防具耐久.
	auto&& pSelf = PlayerMgr::getInstance().getSelf();
	if (pSelf)
	{
		if (pSelf->getId() == target_id)
		{
			m_eBattleState = EN_USER_BATTLESTATE::USER_BATTLESTATE_IN;
			m_nLastBattleTime = std::chrono::steady_clock().now();
			//gLocalServer_Survival.OnPlayerFight();
		}
	}
}

// 所有角色受到物理攻击。
void LocalServer_Battle::fight(int32 caster_id, int32 target_id)
{
	LocalServer_Battle_BeAttacked evt;
	evt.m_CasterMapObjID = caster_id;
	evt.m_TargetMapObjID = target_id;
	postEvent(evt);

	std::shared_ptr<MapObj> pCasterObject = MapObjMgr::getInstance().findObj(caster_id);//HandlerRole::findRole(target_id);
	if (!pCasterObject) return;

	auto pCasterData = std::dynamic_pointer_cast<Role>(pCasterObject);//std::make_shared<Role>(pGameObject);
	if (!pCasterData) {
		return;
	}

	std::shared_ptr<MapObj> pTargetObject = MapObjMgr::getInstance().findObj(target_id);//HandlerRole::findRole(target_id);
	if (!pTargetObject) return;

	auto pTargetData = std::dynamic_pointer_cast<Role>(pTargetObject);//std::make_shared<Role>(pGameObject);
	if (!pTargetData) {
		return;
	}

	int dmgValue{};
	int durability{};
	bool bCritical{};
	// 应该由强联网时服务端主动发起的攻击需要自动生成一个idHP，并把此值回传给客户端
	NS::I64 idHP = RandomDataMgr::getInstance().CreateGuidInt64();
	if (!ProcessFight(pCasterData, pTargetData, dmgValue, bCritical, idHP, durability))
		return;
	// 被攻击 扣防具耐久.
	//reduceEndurance(target_id);

	bool isTargetDead = pTargetData->isDead();
	// 发送攻击消息。
	MsgMapObjectUnderAttackG2C msg;
	msg.set_sourceobjid(caster_id);
	msg.set_servertimems(TimeUtil::getCurTime());
	auto pTargetInfo = msg.add_targetinfo();
	pTargetInfo->set_mapobjectid(target_id);
	pTargetInfo->set_damage(dmgValue);
	pTargetInfo->set_durability(durability);
	pTargetInfo->set_criticl(bCritical ? 1 : 0);
	int nCurHP = std::max(pTargetData->getHp() - dmgValue, 0);
	pTargetData->setHp(nCurHP);
	int nMaxHP = pTargetData->getMaxHp();
	if (nCurHP > nMaxHP)
	{
		nCurHP = nMaxHP;
	}
	if (nCurHP < 0)
	{
		nCurHP = 0;
	}
	pTargetInfo->set_currhp(nCurHP);
	msg.set_idhp(idHP);
	send(msg);

	//受击后触发效果， 普攻->物理伤害
	onAfterDamage(0, caster_id, target_id, dmgValue, (int)SKILL_EFFECT_DMG_WEAPON, idHP, HurtSource::Normal_Attack);

	if (pTargetInfo->currhp() == 0) kill(target_id, caster_id, isTargetDead);

	//击退
	if (pTargetInfo->currhp() > 0)
	{
		//doBeHitFly(caster_id, target_id, idHP);
	}
}

void LocalServer_Battle::multiFight(int32 caster_id, const std::vector<int32>& target_ids)
{
	if (target_ids.empty()) { return; }

	// 攻击者。
	std::shared_ptr<MapObj> pCasterObject = MapObjMgr::getInstance().findObj(caster_id);//HandlerRole::findRole(target_id);
	if (!pCasterObject) return;

	auto pCasterData = std::dynamic_pointer_cast<Role>(pCasterObject);//std::make_shared<Role>(pGameObject);
	if (!pCasterData) {
		return;
	}

	// 发送攻击消息。
	MsgMapObjectUnderAttackG2C msg;
	msg.set_sourceobjid(caster_id);
	msg.set_servertimems(TimeUtil::getCurTime());
	// 应该由强联网时服务端主动发起的攻击需要自动生成一个idHP，并把此值回传给客户端
	NS::I64 idHP = RandomDataMgr::getInstance().CreateGuidInt64();
	msg.set_idhp(idHP);

	std::vector<std::tuple<int32, int32, bool>> targetInfos{};
	for (auto&& target_id : target_ids)
	{
		// 目标。
		std::shared_ptr<MapObj> pTargetObject = MapObjMgr::getInstance().findObj(target_id);//HandlerRole::findRole(target_id);
		if (!pTargetObject) continue;;

		auto pTargetData = std::dynamic_pointer_cast<Role>(pTargetObject);//std::make_shared<Role>(pGameObject);
		if (!pTargetData) {
			continue;;
		}

		int dmgValue{};
		int durability{};
		bool bCritical{};
		// fight calculation
		if (!ProcessFight(pCasterData, pTargetData, dmgValue, bCritical, idHP, durability)) { continue; }
		// 被攻击 扣防具耐久.
		reduceEndurance(target_id);
		// 受击者是否已死
		bool isTargetDead = pTargetData->isDead();
		// push message
		auto pTargetInfo = msg.add_targetinfo();
		pTargetInfo->set_mapobjectid(target_id);
		pTargetInfo->set_damage(dmgValue);
		pTargetInfo->set_durability(durability);
		pTargetInfo->set_criticl(bCritical ? 1 : 0);
		int nCurHP = std::max(pTargetData->getHp() - dmgValue, 0);
		int nMaxHP = pTargetData->getMaxHp();
		pTargetData->setHp(nCurHP);
		if (nCurHP > nMaxHP)
		{
			nCurHP = nMaxHP;
		}
		if (nCurHP < 0)
		{
			nCurHP = 0;
		}
		pTargetInfo->set_currhp(nCurHP);

		if (!isTargetDead)
		{
			targetInfos.push_back(std::make_tuple(target_id, dmgValue, nCurHP == 0));
		}
	}
	send(msg);

	// after dmg trigger
	for (auto&& targetInfo : targetInfos)
	{
		int target_id = std::get<0>(targetInfo);
		int dmgValue = std::get<1>(targetInfo);
		int bIsDead = std::get<2>(targetInfo);

		// be atk trigger
		onAfterDamage(0, caster_id, target_id, dmgValue, (int)SKILL_EFFECT_DMG_WEAPON, idHP, HurtSource::Normal_Attack);
		// death trigger
		if (bIsDead) kill(target_id, caster_id, true);
	}
}

void LocalServer_Battle::dmgReduceEffect(int32 caster_id, int32 target_id, int& dmgValue, float hurtReduce, int effect_type, NS::I64 serialId,int32 skill_id, int& durability)
{
	if (effect_type == SKILL_EFFECT_DMG_REAL) return; //真实伤害不触发效果
	if (dmgValue <= 0) return;

	//触发的格挡被动技能效果值
	dmgValue = static_cast<int>(dmgValue * (100.0f - hurtReduce) / 100.0f);

	// 目标。
	//NS::GameObject* pGameObject = HandlerRole::findRole(target_id);
	std::shared_ptr<MapObj> pGameObject = MapObjMgr::getInstance().findObj(target_id);//HandlerRole::findRole(target_id);
	if (!pGameObject) return;

	auto pTargetData = std::dynamic_pointer_cast<Role>(pGameObject);//std::make_shared<Role>(pGameObject);
	if (!pTargetData) {
		return;
	}

	// 施法者角色增伤属性
	std::shared_ptr<MapObj> pCasterMapObj = MapObjMgr::getInstance().findObj(caster_id);
	std::shared_ptr<Role> pCasterData{ nullptr };
	if (pCasterMapObj)
	{
		pCasterData = std::dynamic_pointer_cast<Role>(pCasterMapObj);
	}
	if (pCasterData)
	{
		float valAdd = pCasterData->getAttr(MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_TOTAL_DAMAGE_INC);
		dmgValue = static_cast<int>(dmgValue * (1000.0f + valAdd) / 1000.0f);
		// debuff数量增伤属性
		float fDebuffAdd = pCasterData->getAttr(MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_DEBUFF_DAMAGE_INC);
		if (fDebuffAdd > 0.f)
		{
			int nDebuffCount = LocalServer_Buff::getInstance().countDebuff(pTargetData->getId());
			dmgValue = static_cast<int>(dmgValue * (1000.0f + fDebuffAdd * nDebuffCount) / 1000.0f);
		}
	}

	// 遍历Buff，检查减伤触发效果
	for (auto&& buff : pTargetData->getBuffs())
	{
		auto buffType = buff.getType();
		if (!buffType) continue;

		for (auto&& effect : buffType->effects)
		{
			if (effect.ref_values.size() <= 0) { continue; }
			switch ((BUFF_EFFECT_TYPE)effect.type)
			{
			case BUFF_EFFECT_FACE_REDUCE_HURT:
			{
				int percent = effect.ref_values[0].value;
				dmgValue = dmgValue * (1000 - percent) / 1000;
			}
			break;

			case BUFF_EFFECT_REDUCE_HURT_BY_SKILL: 
			{
				//指定技能减伤效果
				if (effect.ref_values[0].attrref == skill_id)
				{
					dmgValue = dmgValue * (1000 - effect.ref_values[0].value) / 1000;
				}
			}
			break;

			default:
				break;
			}
		}
	}
	// 护盾吸收
	absorbDamage(target_id, dmgValue, effect_type, durability);
}

void LocalServer_Battle::absorbDamage(int target_id, int& dmgValue, int effect_type, int& durability)
{
	auto buffContainer = LocalServer_Buff::getInstance().FindBuffContainer(target_id);
	if (buffContainer)
	{
		buffContainer->ForEachBuff([&dmgValue, effect_type, &durability](SkillBuffInfo& buff)
			{
				if (dmgValue <= 0)
				{
					return;
				}

				if (buff.IsEmpty())
					return;

				for (const auto& effect : buff.BuffEffects)
				{
					auto offsetType = SkillEffect_BuffDurability::GetTypeBySkillEffectType(effect_type);
					if (offsetType == BUFF_EFFECT_NULL)
					{
						continue;
					}

					if (effect.type == BUFF_EFFECT_SHIELD || effect.type == offsetType)
					{
						// 普通护盾
						if (dmgValue > buff.nDurability)
						{
							dmgValue -= buff.nDurability;
							durability += buff.nDurability;
							buff.nDurability = 0;
						}
						else
						{
							buff.nDurability -= dmgValue;
							durability += dmgValue;
							dmgValue = 0;
						}
					}
				}
			});
	}
}

// 弱联网下，受击前触发的被动技能
// @ TriggerTypeBeforeHurt == 2 , 触发在伤害计算前，解释说是减伤效果，影响本次伤害计算
void LocalServer_Battle::onBeforeDamage(int32 nCasterGroupId, int32 target_id, float& hurtReduce, NS::I64 serialId)
{
	// 目标。
	std::shared_ptr<MapObj> pTargetObject = MapObjMgr::getInstance().findObj(target_id);//HandlerRole::findRole(target_id);
	if (!pTargetObject) return;

	auto pTargetData = std::dynamic_pointer_cast<Role>(pTargetObject);//std::make_shared<Role>(pGameObject);
	if (!pTargetData) {
		return;
	}
	if (nCasterGroupId == pTargetData->getGroupId()) { return; }

	const PassiveSkillMgr* pPassiveSkillMgr = pTargetData->getPassiveSkillMgr();
	if (!pPassiveSkillMgr) { return; }
	// 手动触发减伤类被动技能
	hurtReduce = pPassiveSkillMgr->damageReduction();
}

// 弱联网下，受击后触发的被动技能
void LocalServer_Battle::onAfterDamage(int32 nSkillId, int32 caster_id, int32 target_id, int dmgValue, int effect_type, NS::I64 serialId, HurtSource eHurtSource)
{
	if (effect_type == SKILL_EFFECT_DMG_REAL) return; //与确认，真实伤害不触发效果
	if (dmgValue <= 0) return;
	//// 攻击者。
	std::shared_ptr<MapObj> pGameObject = MapObjMgr::getInstance().findObj(caster_id);//HandlerRole::findRole(target_id);
	if (!pGameObject) return;

	auto pCasterData = std::dynamic_pointer_cast<Role>(pGameObject);//std::make_shared<Role>(pGameObject);
	if (!pCasterData) {
		return;
	}
	//// 目标。
	pGameObject = MapObjMgr::getInstance().findObj(target_id);//HandlerRole::findRole(target_id);
	if (!pGameObject) return;

	auto pTargetData = std::dynamic_pointer_cast<Role>(pGameObject);//std::make_shared<Role>(pGameObject);
	if (!pTargetData) {
		return;
	}

	////Buff效果
	buffEffectAfterDmg(pCasterData, pTargetData, dmgValue, effect_type, serialId);

	if (eHurtSource == HurtSource::Skill)
	{
		// 受到技能伤害，发出事件通知
		if (pTargetData->getPassiveSkillMgr())
		{
			PassiveEvent_BeAtk evtBeAtk{};
			evtBeAtk.m_nHitMapObjId = target_id;
			evtBeAtk.m_nAtkMapObjId = caster_id;
			evtBeAtk.m_nDamageValue = dmgValue;
			evtBeAtk.m_bIsCritical = false;	// TODO	技能计算流程，目前无法获取本次是否暴击，等结算流程重构后再传正确的值
			evtBeAtk.m_nHurtType = effect_type;
			evtBeAtk.m_nSkillId = nSkillId;
			pTargetData->getPassiveSkillMgr()->onBeAtkEvent(evtBeAtk);
		}

		// 攻击者命中，发出事件通知
		if (pCasterData->getPassiveSkillMgr())
		{
			PassiveEvent_Hit evtHit{};
			evtHit.m_nAtkMapObjId = caster_id;
			evtHit.m_nHitMapObjId = target_id;
			evtHit.m_nDamageValue = dmgValue;
			evtHit.m_bIsCritical = false;	// TODO	技能计算流程，目前无法获取本次是否暴击，等结算流程重构后再传正确的值
			evtHit.m_nHurtType = effect_type;
			evtHit.m_nSkillId = nSkillId;
			pCasterData->getPassiveSkillMgr()->onAtkHitEvent(evtHit);
		}
	}
}

//受击触发Buff效果
void LocalServer_Battle::buffEffectAfterDmg(const std::shared_ptr<Role> pCasterData, const std::shared_ptr<Role> pTargetData, int& dmgValue, int effect_type, NS::I64 serialId)
{
	if (effect_type == SKILL_EFFECT_DMG_REAL) return; //与确认，真实伤害不触发效果
	if (dmgValue <= 0) return;
	if (!pCasterData || !pTargetData) return;

	// 遍历Buff，检查受击后触发效果
	for (auto&& buff : pTargetData->getBuffs())
	{
		auto buffType = buff.getType();
		if (!buffType) continue;

		for (auto&& effect : buffType->effects)
		{
			if (effect.ref_values.size() <= 0) continue;
			switch ((BUFF_EFFECT_TYPE)effect.type)
			{
			case BUFF_EFFECT_PHY_HIT_BACK:  //物理伤害反伤
			case BUFF_EFFECT_ELEM_HIT_BACK: //魔法伤害反伤
			case BUFF_EFFECT_TOTAL_HIT_BACK: //全面反伤
			{
				bool isPhysicHurt = (BUFF_EFFECT_TYPE)effect.type == BUFF_EFFECT_PHY_HIT_BACK && effect_type == SKILL_EFFECT_DMG_WEAPON;
				bool isMagicHurt = (BUFF_EFFECT_TYPE)effect.type == BUFF_EFFECT_ELEM_HIT_BACK && (effect_type >= SKILL_EFFECT_DMG_FIRE && effect_type <= SKILL_EFFECT_DMG_DARK);
				bool isAllHurt = (BUFF_EFFECT_TYPE)effect.type == BUFF_EFFECT_TOTAL_HIT_BACK;
				if (!isMagicHurt && !isPhysicHurt && !isAllHurt) break;
				// 伤害消息。
				int percent = effect.ref_values[0].value;
				int hurtBackVal = dmgValue * percent / 1000; // 反伤伤害
				if (hurtBackVal <= 0) { hurtBackVal = 1; } // 反伤伤害保底1点
				MsgPassiveSkillEffectDmgMS2C msg;
				msg.set_serialid(serialId);
				msg.set_target_skill(0);
				msg.set_caster_mapobjectid(pTargetData->getId());
				msg.set_target_mapobjectid(pCasterData->getId());
				int durability{ 0 };
				absorbDamage(pCasterData->getId(), hurtBackVal, (int)SKILL_EFFECT_DMG_NULL, durability);
				msg.set_dmg_val(hurtBackVal);
				msg.set_durability(durability);
				msg.set_criticl(false);
				int nCurHP = std::max(pCasterData->getHp() - hurtBackVal, 0);
				int nMaxHP = pCasterData->getMaxHp();
				if (nCurHP > nMaxHP)
				{
					nCurHP = nMaxHP;
				}
				if (nCurHP < 0)
				{
					nCurHP = 0;
				}
				msg.set_currhp(nCurHP);
				send(msg);
			}
			break;

			case BUFF_EFFECT_HEAL_MEMBER_ON_UNDER_ATTACK: //被攻击时治疗范围友军
			{
				assert(effect.ref_values.size() == 2);
				if (effect.ref_values.size() != 2) break;
				int percent = effect.ref_values[0].value;	// n_effect_value1_1填治疗值的千分比
				int range = effect.ref_values[1].value;		// n_effect_value1_2填治疗范围
				//范围内友军
				//auto&& targets = MapObjMgr::getInstance().findNearestObjectsByMapObj(pTargetData.get(), (float)range, INT_MAX, [pTargetData](MapObj* pMapObj) -> bool
				//{
				//	//目标是自身
				//	if (pMapObj->getId() == pTargetData->getId()) { return false; }
				//	//筛选目标死亡
				//	auto&& pRole = dynamic_cast<Role*>(pMapObj);
				//	if (!pRole) return false;
				//	if (pRole->isDead()) { return false; }
				//	//友军
				//	auto group = static_cast<Role::GroupType>(pTargetData->getGroupId());
				//	auto enimyGroup = static_cast<Role::GroupType>(pRole->getGroupId());
				//	if (group == enimyGroup)
				//	{
				//		return true;
				//	}
				//	return false;
				//});
				////回血消息
				//int healthValue = dmgValue * percent / 1000;
				//if (healthValue)
				//{
				//	for (auto&& targetId : targets)
				//	{
				//		MsgSkillEffectRestoreMS2C msg;
				//		msg.set_serialid(serialId);
				//		msg.set_skill_id(0);
				//		msg.set_caster_mapobjectid(pTargetData->getId());
				//		msg.set_target_mapobjectid(targetId);
				//		msg.set_hp_val(healthValue);
				//		int nCurHP = pCasterCharacterAttributeComp->getRealHp() + healthValue;
				//		int nMaxHP = pCasterCharacterAttributeComp->getMaxHp();
				//		if (nCurHP > nMaxHP)
				//		{
				//			nCurHP = nMaxHP;
				//		}
				//		if (nCurHP < 0)
				//		{
				//			nCurHP = 0;
				//		}
				//		msg.set_currhp(nCurHP);
				//		send(msg);
				//	}
				//}
			}
				break;
			default:
				break;
			}
		}
	}
}

NS::I64 LocalServer_Battle::getCurSerialId()
{
	if (s_nSerialId == INIT_SERIAL_ID)
	{
		return s_nSerialId;
	}
	return (s_nSerialId - 1);
}

// 其它角色施法。
NS::I64 LocalServer_Battle::cast_skill(int32 skill_id, int32 caster_id, int32 target_id)
{
	////static NS::I64 serialId = 10030010000000;
	// 施法者。
	std::shared_ptr<MapObj> pCasterObject = MapObjMgr::getInstance().findObj(caster_id);//HandlerRole::findRole(target_id);
	if (!pCasterObject) return 0;

	auto pCasterData = std::dynamic_pointer_cast<Role>(pCasterObject);//std::make_shared<Role>(pGameObject);
	if (!pCasterData) {
		return 0;
	}

	std::shared_ptr<MapObj> pTargetObject = MapObjMgr::getInstance().findObj(target_id);//HandlerRole::findRole(target_id);
	if (!pTargetObject) return 0;

	auto pTargetData = std::dynamic_pointer_cast<Role>(pTargetObject);//std::make_shared<Role>(pGameObject);
	if (!pTargetData) {
		return 0;
	}

	// 施法消息。
	MsgSkillMapObjCastSkillMS2C msg;
	msg.set_serialid(s_nSerialId++);
	msg.set_skill_id(skill_id);
	msg.set_caster_mapobjectid(caster_id);
	msg.add_target_mapobjectids(target_id);
	auto serverPos = pCasterData->getPos();
	msg.set_caster_pos_x(static_cast<int32>(serverPos.x));
	msg.set_caster_pos_y(static_cast<int32>(serverPos.y));
	serverPos = pTargetData->getPos();
	msg.set_target_pos_x(static_cast<int32>(serverPos.x));
	msg.set_target_pos_y(static_cast<int32>(serverPos.y));

	// 发送施法消息。
	send(msg);

	//弱联下直击型技能buff改为关键帧时机调用触发
	//auto infoSkill = SkillCfg::getInstance().findSkillType(skill_id);
	//if (infoSkill &&  infoSkill->m_skillType == SkillType::Type::InstantSkill)
	//{
	//	skillBuff(skill_id, caster_id, target_id);
	//}

	return s_nSerialId;
}

NS::I64 LocalServer_Battle::easy_cast_skill(int32 skill_id, int32 caster_id, int32 target_id, const glm::vec2& target_pos)
{
	//auto&& pSkillType = SkillCfg::getInstance().findSkillType(skill_id);
	//if (!pSkillType) return -1;
	//auto&& pGameObject = HandlerRole::findRole(caster_id);
	//if (!pGameObject) return -1;
	//auto pCaster = pGameObject->findComponent<RoleComponent>();
	//if (!pCaster) return -1;
	//auto pCasterData = pCaster->getData();
	//if (!pCasterData) return -1;

	////和AI一样的逻辑，直击AOE->多目标接口，组合、瞬移类型技能->目标位置接口，其他->单目标接口
	//if (pSkillType->m_skillType == SkillType::Type::InstantSkill && pSkillType->m_targetCount > 1)
	//{
	//	auto&& targets = MapObjMgr::getInstance().findNearestObjectsByMapObj(pCasterData.get(), pSkillType->m_attkRange, pSkillType->m_targetCount, [this, pCasterData, pSkillType](MapObj* pMapObj)
	//	{
	//		return filterSkillCastFunc(pCasterData, pSkillType, pMapObj);
	//	});
	//	return cast_skill(skill_id, caster_id, targets);
	//}
	//else if (pSkillType->m_skillType == SkillType::Type::Group || pSkillType->m_skillType == SkillType::Type::TeleportSkill)
	//{
	//	//指定位置
	//	return cast_skill(skill_id, caster_id, target_pos);
	//}
	//else
	//{
	//	return cast_skill(skill_id, caster_id, target_id);
	//}
	return -1;
}


void LocalServer_Battle::skillEffect(const CastingSkillInfo& info)
{
	skillSummonEffect(info);
}

void LocalServer_Battle::castSummonEffect(int32 caster_id, int32 skill_id, NS::I64 serial_id, int effectIndex, std::vector<glm::vec3>* posList, std::vector<int32>* birthMapobjIdList) {

	skillSummonEffect(CastingSkillInfo{ skill_id, caster_id, serial_id, 0, posList->at(0)}, effectIndex, posList, birthMapobjIdList);
}

void LocalServer_Battle::skillSummonEffect(const CastingSkillInfo& info, int effectIndex, std::vector<glm::vec3>* posList, std::vector<int32>* birthMapobjIdList) {
	auto cfg = SkillCfg::getInstance().findSkillType(info.id);
	auto& casterMobs = m_summonMobs[info.caster];
	for (int i = 0; i < SkillType::MAX_EFFECTS; ++i)
	{
		if ((effectIndex != -1) && (effectIndex != i))
		{
			continue;
		}
		auto effect = cfg->findEffect(i);

		auto effectType = static_cast<SKILL_EFFECT_TYPE>(effect->m_type);

		if (effectType == SKILL_EFFECT_SUMMON || effectType == SKILL_EFFECT_COPY || effectType == SKILL_EFFECT_SUMMON_ON_FIXED_POS)
		{
			if (effectType == SKILL_EFFECT_SUMMON_ON_FIXED_POS)
			{
				if (posList == nullptr)
				{
					return;
				}
				if (birthMapobjIdList == nullptr)
				{
					return;
				}
			}
			if (RandomUtil::getRandomInt(1, 1000) > effect->m_hitpercent) continue;	//效果触发概率

			std::shared_ptr<MapObj> pGameObject = MapObjMgr::getInstance().findObj(info.caster);//HandlerRole::findRole(target_id);
			if (!pGameObject) continue;;

			auto pCastertData = std::dynamic_pointer_cast<Role>(pGameObject);//std::make_shared<Role>(pGameObject);
			if (!pCastertData) {
				return;
			}

			int hp = -1;
			if (effectType == SKILL_EFFECT_COPY)
			{
				hp = pCastertData->getHp();
			}

			MsgSkillEffectSummonMS2C msg;
			msg.set_serialid(info.serialId);
			glm::vec2 pos;
			switch (info.type)
			{
			case CastSkillType::Pos:
			{
				pos = info.pos;
				break;
			}
			case CastSkillType::Target:
			{
				std::shared_ptr<MapObj> pGameObject = MapObjMgr::getInstance().findObj(info.target);//HandlerRole::findRole(target_id);
				if (!pGameObject) continue;;

				auto pTargetData = std::dynamic_pointer_cast<Role>(pGameObject);//std::make_shared<Role>(pGameObject);
				if (!pTargetData) {
					return;
				}
				pos = pTargetData->getPos();
				break;
			}
			case CastSkillType::Targets:
			{
				std::shared_ptr<MapObj> pGameObject = MapObjMgr::getInstance().findObj(info.targets[0]);//HandlerRole::findRole(target_id);
				if (!pGameObject) continue;;

				auto pTargetData = std::dynamic_pointer_cast<Role>(pGameObject);//std::make_shared<Role>(pGameObject);
				if (!pTargetData) {
					return;
				}
				pos = pTargetData->getPos();
				break;
			}
			default:
				break;
			}
			std::shared_ptr<MapData> mapData{ MapDataMgr::getInstance().getCurrMapData() };
			if (!mapData) { return; }

			auto count = std::min(static_cast<int32>(cfg->m_targetCount - casterMobs.count(info.id)), effect->m_targetCount);
			for (int j = 0; j < count; ++j)
			{
				//glm::ivec2 outCoord;
				glm::vec3 posVec3(pos.x, pos.y, 0);
				//if (!mapData->worldPosToCoordinate(outCoord, posVec3)) { return; }
				Rect birthRect;
				int range = effect->m_range;
				if (range == 1)
				{//召唤技能随机范围，之前没有配置，配置数据1，写死5，为不影响为配置召唤随机范围，这里先兼容下。
					range = 5;
				}
				birthRect.m_left = posVec3.x - range;
				birthRect.m_right = posVec3.x + range;
				birthRect.m_top = posVec3.y - 3;
				birthRect.m_bottom = posVec3.y + 3;
				std::vector<glm::ivec2> forbiddenVec{};
				//CLV-2063 召唤图腾位置不可重复
				//if (info.id == 30198 || info.id == 30199)
				//{
				//	for (auto&& obj : casterMobs)
				//	{
				//		if (obj.first == 30198 || obj.first == 30199)
				//		{
				//			auto mobObj = HandlerRole::findRole(obj.second);
				//			if (!mobObj) continue;
				//			auto pMob = mobObj->findComponent<RoleComponent>();
				//			if (!pMob) continue;
				//			auto&& mobPos = pMob->getWorldPos();
				//			glm::ivec2 coord;
				//			if (!mapData->worldPosToCoordinate(coord, mobPos)) { continue; }
				//			forbiddenVec.push_back(coord);
				//		}
				//	}
				//}

//				glm::vec3 worldPos;
//
//				if (effectType == SKILL_EFFECT_SUMMON_ON_FIXED_POS)
//				{
//					worldPos = posList->at(j % posList->size());
//				} else if (LocalServer_BirthMob::getInstance().randomBirthPos(birthRect, effect->m_effectAttrRefs[0].m_effectValue, posVec3, true, LocalServer_BirthMob::E_BirthType::E_BirthBox, forbiddenVec))
//				{
//					if (!mapData->coordinateToWorldPos(worldPos, outCoord)) { return; }
//				}
//				else
//				{
//					continue;
//				}
//				int nMobDefID = effect->m_effectAttrRefs[0].m_effectValue;
//				const glm::vec3& birthWorldPos = worldPos;
////				bool bSave = false;
//				Role::GroupType owerGroup = static_cast<Role::GroupType>(pCaster->getData()->getGroupId());
////				int nHp = hp;
//				int nMp = -1;
////				NS::I64 serverMapObjId = -1;
//				NS::I64 ownerMapObjId = pCaster->getData()->getId();
//
//				std::shared_ptr<LocalServer_BirthMob::StBirthParm> parm = std::make_shared<LocalServer_BirthMob::StBirthParm>();
//				parm->nMobDefID = nMobDefID;
//				parm->birthWorldPos = birthWorldPos;
//				parm->eGroup = owerGroup;
//				parm->nHp = hp;
//				parm->nMp = nMp;
//				parm->ownerMapObjId = (bzbee::int32)ownerMapObjId;
//				parm->bPlayAppearEffect = false;
//
//				if (auto p_mobCfg = MonsterCfg::getInstance().findMonsterType(nMobDefID))
//				{
//					if (p_mobCfg->n_summon_effect) {
//						parm->bFadeIn = true;
//					}
//				}
//
//				int nBirthMobMapObjId = LocalServer_BirthMob::getInstance().birthMob(parm);
//				if (effectType == SKILL_EFFECT_SUMMON_ON_FIXED_POS)
//				{
//					birthMapobjIdList->push_back(nBirthMobMapObjId);
//				}
//				int32 summonId = static_cast<int32>(nBirthMobMapObjId);
//
//				auto mobObj = HandlerRole::findRole(summonId);
//				if (!mobObj) continue;
//				auto pMob = mobObj->findComponent<RoleComponent>();
//				if (!pMob) continue;
//				pMob->setSummonerId(info.caster);
//				msg.add_summon_id(summonId);
//				casterMobs.emplace(info.id, summonId);
//
//				if (effect->m_effectAttrRefs[1].m_effectValue & 2)
//				{
//					auto& saveCasterMobs = m_saveSummonMobs[info.caster];
//					saveCasterMobs.emplace(info.id, summonId);
//				}
//
//				auto&& mapType = HandlerLogin::getInstance().getMapType();
//				if (mapType == MapDataMgr::MapType::TowerChallenge)
//				{
//					LocalServer_Survival_Endless::getInstance().summonMob(info.caster, summonId);
//				}
			}

			//如果有保存召唤物
			//if (effect->m_effectAttrRefs[1].m_effectValue & 2)
			//{
			//	HandlerPlayerCacheSync::getInstance().sync(true);
			//}
			send(msg);
		}
	}
}

int32 LocalServer_Battle::findNearestSummon(int32 casterId, const glm::vec3& pos, int32 mobCfgId)
{
	//auto casterMobsIt = m_summonMobs.find(casterId);
	//if (casterMobsIt == m_summonMobs.end()) { return -1; }

	//auto summonMap = casterMobsIt->second;
	//float maxDistance{ FLT_MAX };
	//int32 targetId{ -1 };

	//for (auto&& sumIter : summonMap)
	//{
	//	auto pMapObj = MapObjMgr::getInstance().findObj(sumIter.second);
	//	if (!pMapObj) continue;
	//	//检查指定Id(怪物)
	//	if (mobCfgId > 0)
	//	{
	//		auto pMonster = dynamic_cast<Monster*>(pMapObj.get());
	//		if (pMonster)
	//		{
	//			if (pMonster->getMonsterType()->n_ID != mobCfgId) continue;
	//		}
	//		else
	//		{
	//			continue;
	//		}
	//	}

	//	float len = glm::distance(pos, glm::vec3{ pMapObj->getPos() });
	//	if (len < maxDistance)
	//	{
	//		maxDistance = len;
	//		targetId = sumIter.second;
	//	}
	//}
	//return targetId;
	return -1;
}

void LocalServer_Battle::summonRemove(int32 casterId, int32 summonId)
{
	//casterRemove(summonId);
	//if (casterId == 0) { return; }
	//auto casterMobsIt = m_summonMobs.find(casterId);
	//if (casterMobsIt == m_summonMobs.end()) { return; }
	//auto& map = casterMobsIt->second;
	//for (auto it = map.begin(); it != map.end(); ++it)
	//{
	//	if (it->second == summonId)
	//	{
	//		casterMobsIt->second.erase(it);
	//		break;
	//	}
	//}

	//auto saveCasterMobsIt = m_saveSummonMobs.find(casterId);
	//if (saveCasterMobsIt == m_saveSummonMobs.end()) { return; }
	//auto& mapSaveCasterMob = saveCasterMobsIt->second;
	//for (auto itSave = mapSaveCasterMob.begin(); itSave != mapSaveCasterMob.end(); ++itSave)
	//{
	//	if (itSave->second == summonId)
	//	{
	//		saveCasterMobsIt->second.erase(itSave);
	//		break;
	//	}
	//}
}

void LocalServer_Battle::DieWithCaster(int32 casterId)
{
	//auto casterMobsIt = m_summonMobs.find(casterId);
	//if (casterMobsIt == m_summonMobs.end())
	//{
	//	return;
	//}
	//auto summonMap = casterMobsIt->second;
	//for (auto&& sumIter : summonMap)
	//{
	//	GameObject* pGameObjectMonster = MonsterComponentMgr::getInstance().findMonster(sumIter.second);
	//	assert(pGameObjectMonster);
	//	if (!pGameObjectMonster)
	//	{
	//		continue;
	//	}
	//	MonsterComponent* pMonsterComp = pGameObjectMonster->findComponent<MonsterComponent>();
	//	assert(pMonsterComp);
	//	if (!pMonsterComp || !pMonsterComp->getMonsterData() || !pMonsterComp->getMonsterData()->isAlive() || !pMonsterComp->getMonsterData()->getMonsterType() || !pMonsterComp->getMonsterData()->getMonsterType()->m_bDieWithOwner)
	//	{
	//		continue;
	//	}
	//	pMonsterComp->onDayNightLeave();
	//}
}

void LocalServer_Battle::casterRemove(int32 casterId)
{
	DieWithCaster(casterId);
	m_summonMobs.erase(casterId);
	m_saveSummonMobs.erase(casterId);
}

int LocalServer_Battle::summonCount(int32 caster_id, int32 skill_id)
{
	auto&& iter = m_summonMobs.find(caster_id);
	if (iter == m_summonMobs.end()) return 0;

	return static_cast<int>(iter->second.count(skill_id));
}

void LocalServer_Battle::resumePlayerSummons()
{
	//auto&& player = PlayerMgr::getInstance().getSelf();
	//if (!player) { return; }
	//auto& casterMobs = m_summonMobs[player->getId()];
	//for (auto it = m_playerStoreSummons.begin(); it != m_playerStoreSummons.end(); ++it)
	//{
	//	std::shared_ptr<LocalServer_BirthMob::StBirthParm> parm = std::make_shared<LocalServer_BirthMob::StBirthParm>();
	//	parm->nMobDefID = it->defId;
	//	parm->birthWorldPos = player->getPos();
	//	parm->eGroup = (Role::GroupType)player->getGroupId();
	//	parm->ownerMapObjId = player->getId();

	//	int32 summonId = static_cast<int32>(LocalServer_BirthMob::getInstance().birthMob(parm));

	//	auto mobObj = HandlerRole::findRole(summonId);
	//	if (!mobObj) continue;
	//	auto pMob = mobObj->findComponent<RoleComponent>();
	//	if (!pMob) continue;
	//	auto&& data = pMob->getData();
	//	if (!data) { return; }
	//	assert(pMob);
	//	if (pMob)
	//	{
	//		CharacterAttributeComponent* pCharacterAttributeComp = pMob->findComponent<CharacterAttributeComponent>();
	//		assert(pCharacterAttributeComp);
	//		if (pCharacterAttributeComp)
	//		{
	//			std::string tag = fmt::format("LocalServer_Battle::resumePlayerSummons: mapobjectid = {0}", summonId);
	//			pCharacterAttributeComp->SetHp(it->hp, tag);
	//		}
	//	}
	//	//data->setHp(it->hp);
	//	data->setMp(it->mp);
	//	data->setBuffs(it->buffs);
	//	casterMobs.emplace(it->skillId, summonId);
	//}
	//m_playerStoreSummons.clear();
}

void LocalServer_Battle::restorePlayerSummons()
{
	//auto&& player = PlayerMgr::getInstance().getSelf();
	//if (!player) { return; }

	//auto casterMobsIt = m_summonMobs.find(player->getId());
	//if (casterMobsIt == m_summonMobs.end()) { return; }

	//auto& map = casterMobsIt->second;
	//for (auto it = map.begin(); it != map.end(); ++it)
	//{
	//	auto&& mapObj = MapObjMgr::getInstance().findObj(it->second);
	//	if (!mapObj) { continue; }
	//	auto&& role = HandlerRole::findRole(mapObj->getId());
	//	if(!role) { continue; }
	//	auto&& roleComp = role->findComponent<MonsterComponent>();
	//	if(!roleComp) { continue; }
	//	auto&& roleData = roleComp->getMonsterData();
	//	if(!roleData) { continue;}
	//	CharacterAttributeComponent* pCharacterAttributeCompMonster = roleComp->findComponent<CharacterAttributeComponent>();
	//	assert(pCharacterAttributeCompMonster);
	//	if (!pCharacterAttributeCompMonster) { continue; }
	//	m_playerStoreSummons.emplace_back(PlayerStoreSummonInfo{ it->first, roleData->getDefId() , pCharacterAttributeCompMonster->getRealHp(), roleData->getMp(), 0, roleData->getBuffs() });
	//}
}
void LocalServer_Battle::mapRestore()
{
	LocalServer_Battle::restorePlayerSummons();
	m_summonMobs.clear();
	m_saveSummonMobs.clear();
}

void LocalServer_Battle::mapResume()
{
	LocalServer_Battle::resumePlayerSummons();
}


// 其它角色施法。
NS::I64 LocalServer_Battle::cast_skill(int32 skill_id, int32 caster_id, const std::vector<int32>& target_ids)
{
	std::shared_ptr<MapObj> pCasterObject = MapObjMgr::getInstance().findObj(caster_id);//HandlerRole::findRole(target_id);
	if (!pCasterObject) return 0;

	auto pCasterData = std::dynamic_pointer_cast<Role>(pCasterObject);//std::make_shared<Role>(pGameObject);
	if (!pCasterData) {
		return 0;
	}

	// 施法消息。
	MsgSkillMapObjCastSkillMS2C msg;
	msg.set_serialid(s_nSerialId);
	msg.set_skill_id(skill_id);
	msg.set_caster_mapobjectid(caster_id);


	auto serverPos = pCasterData->getPos();
	msg.set_caster_pos_x(static_cast<int32>(serverPos.x));
	msg.set_caster_pos_y(static_cast<int32>(serverPos.y));

	for (const auto v : target_ids)
		msg.add_target_mapobjectids(v);

	// 发送施法消息。
	send(msg);

	//bool bProcSurvival = true;
	//for (const auto v : target_ids)
	//{
	//	if (bProcSurvival)
	//		bProcSurvival = gLocalServer_Survival.OnCastSkill(caster_id, v);
	//}

	++s_nSerialId;

	return s_nSerialId;
}


// 其它角色施法。
NS::I64 LocalServer_Battle::cast_skill(int32 skill_id, int32 caster_id, glm::vec2 target_position)
{
	// 施法者。
	std::shared_ptr<MapObj> pCasterObject = MapObjMgr::getInstance().findObj(caster_id);//HandlerRole::findRole(target_id);
	if (!pCasterObject) return 0;

	auto pCaster = std::dynamic_pointer_cast<Role>(pCasterObject);//std::make_shared<Role>(pGameObject);
	if (!pCaster) {
		return 0;
	}

	//被动技能需要计算触发概率
	auto infoSkill = SkillCfg::getInstance().findSkillType(skill_id);
	if (!infoSkill) { return -1; }
	if (infoSkill->m_triggerType == 2)
	{
		if (RandomUtil::getRandomInt(1, 100) > infoSkill->m_triggerParam1) return -1;	// 触发概率。
	}

	// 施法消息。
	MsgSkillMapObjCastSkillMS2C msg;
	msg.set_serialid(s_nSerialId);
	msg.set_skill_id(skill_id);
	msg.set_caster_mapobjectid(caster_id);
	msg.add_target_mapobjectids(0);

	auto serverPos = pCaster->getPos();
	msg.set_caster_pos_x(static_cast<int32>(serverPos.x));
	msg.set_caster_pos_y(static_cast<int32>(serverPos.y));

	serverPos = glm::vec3(target_position.x, target_position.y, 0.0f);
	msg.set_target_pos_x(static_cast<int32>(serverPos.x));
	msg.set_target_pos_y(static_cast<int32>(serverPos.y));

	// 发送施法消息。
	send(msg);
	skillBuff(skill_id, caster_id);

	auto cfg = SkillCfg::getInstance().findSkillType(skill_id);
	if (cfg->m_singingTime <= 0)
	{
		skillEffect(CastingSkillInfo{ skill_id, caster_id, s_nSerialId,
			static_cast<float>(cfg->m_singingTime / 1000.0f), target_position });
	}
	else
	{
		m_castingSkills.emplace_back(CastingSkillInfo{ skill_id, caster_id, s_nSerialId,
			static_cast<float>(cfg->m_singingTime/1000.0f), target_position });
	}
	s_nSerialId++;

	return s_nSerialId;
}


//void LocalServer_Battle::cast_skill(const LuaTable& table)
//{
//	static NS::I64 serialId = 30010020000000;
//
//	MsgPB::MsgSkillPreviewS2C msg;
//	message_from_table(&msg, table);
//	msg.set_serialid(serialId++);
//	send(msg);
//}


void LocalServer_Battle::skillDamageEarth(NS::I64 serialId, int32 skill_id, int32 caster_id, const glm::vec3& target_pos)
{
	// 施法者。
}

void LocalServer_Battle::skillDamagePlant(NS::I64 serialId, int32 skill_id, int32 caster_id, const glm::vec3& target_pos)
{
	// 施法者。
}

void LocalServer_Battle::skillPutBlock(const glm::ivec2& coordPos, int32 layer, int32 templateid) {
	//LocalServer_BuildLogicCli::getInstance().GetBuildLogic().PutBlock(coordPos.x, coordPos.y, MsgPB::BUILD_LAYER(layer), templateid, -1);
}

bool LocalServer_Battle::skillPutBuild(const glm::ivec2& coordPos, int32 templateid) {
	//return LocalServer_BuildLogicCli::getInstance().GetBuildLogic().PutBuild(coordPos.x, coordPos.y, templateid,-1,-1);
	return false;
}

bool LocalServer_Battle::IsSkillTargetLogic(std::shared_ptr<Role> casterData, const SkillType* skillType, Role* pRole)
{
	//if (pRole == nullptr)
	//{
	//	// 不用判断
	//	return true;
	//}
	//if (skillType->m_targetArrtType == (int)SkillType::TargetArrType::NoAction)
	//{
	//	// 不用判断
	//	return true;
	//}
	//if (skillType->m_logicOperation == (int)SkillType::LogicOperationType::NoneOperation)
	//{
	//	// 没配置比较条件
	//	return true;
	//}
	//int nMonsterCurValue = 0;
	//int nMonsterMaxValue = 1;
	//if (skillType->m_targetArrtType == (int)SkillType::TargetArrType::HP)
	//{
	//	GameObject* pGameObject = HandlerRole::getInstance().findRole(pRole->getId());
	//	assert(pGameObject);
	//	if (!pGameObject)
	//		return false;
	//	RoleComponent* pRoleComp = pGameObject->findComponent<RoleComponent>();
	//	assert(pRoleComp);
	//	if (!pRoleComp)
	//		return false;
	//	CharacterAttributeComponent* pCharacterAttributeComp = pRoleComp->findComponent<CharacterAttributeComponent>();
	//	assert(pCharacterAttributeComp);
	//	if (!pCharacterAttributeComp)
	//		return false;

	//	nMonsterCurValue = pCharacterAttributeComp->getRealHp();
	//	nMonsterMaxValue = pCharacterAttributeComp->getMaxHp();
	//}
	//else if (skillType->m_targetArrtType == (int)SkillType::TargetArrType::MP)
	//{
	//	nMonsterCurValue = pRole->getMp();
	//	nMonsterMaxValue = pRole->getMaxMp();
	//}
	//if (nMonsterMaxValue == 0)
	//{
	//	// 数据出问题了
	//	return true;
	//}
	//int nMonsterLogicValue = static_cast<int>(static_cast<float>(nMonsterCurValue) * 1000.0f / static_cast<float>(nMonsterMaxValue));
	//if (skillType->m_logicOperation == (int)SkillType::LogicOperationType::GreaterThan)
	//{
	//	// 大于
	//	return (nMonsterLogicValue > skillType->m_logicValue) ? true : false;
	//}
	//if (skillType->m_logicOperation == (int)SkillType::LogicOperationType::LessThan)
	//{
	//	// 小于
	//	return (nMonsterLogicValue < skillType->m_logicValue) ? true : false;
	//}
	//if (skillType->m_logicOperation == (int)SkillType::LogicOperationType::Equality)
	//{
	//	// 等于
	//	return (nMonsterLogicValue == skillType->m_logicValue) ? true : false;
	//}

	return true;
}

bool LocalServer_Battle::filterSkillCastFunc(std::shared_ptr<Role> casterData, const SkillType* skillType, MapObj* pMapObj)
{
	//筛选目标死亡
	//auto&& pRole = dynamic_cast<Role*>(pMapObj);
	//if (pRole && pRole->isDead())
	//{
	//	return false;
	//}
	//if (pRole && !IsSkillTargetLogic(casterData, skillType, pRole))
	//{
	//	return false;
	//}
	////检查目标塔防是否可以被作用
	//auto pBuilding = dynamic_cast<::TowerBuilding*>(pMapObj);
	//if (pBuilding)
	//{
	//	if (pBuilding->getTowerBuildingDefence())
	//	{
	//		if (pBuilding->getTowerBuildingDefence()->m_canAttack == 0) return false;
	//	}
	//}
	////检查目标NPC/怪物/小动物是否可以被作用
	//auto pMonster = dynamic_cast<Monster*>(pMapObj);
	//if (pMonster)
	//{
	//	if (pMonster->getMonsterType())
	//	{
	//		if (pMonster->getMonsterType()->n_Camp == RoleCamp::mid || pMonster->isBeCatch()) return false;
	//	}
	//}
	////飞船、互动家具无法被作用
	//if (pMapObj->getCategory() == MapObj::Category::Vehicle || pMapObj->getCategory() == MapObj::Category::InteractiveBuilding)
	//{
	//	return false;
	//}
	////全阵营作用类型
	//if ((skillType->m_targetType & SkillType::TargetType::AnyGroup) == SkillType::TargetType::AnyGroup)
	//{
	//	return filterTargetCategory(skillType->m_targetCategory, pMapObj);
	//}
	////作用自己
	//if (skillType->m_targetType & SkillType::TargetType::Self)
	//{
	//	if (pMapObj->getId() == casterData->getId())
	//	{
	//		return true;
	//	}
	//}
	////作用敌方
	//if (skillType->m_targetType & SkillType::TargetType::EnemyCamp)
	//{
	//	if (!pRole) { return false; }
	//	auto group = static_cast<Role::GroupType>(casterData->getGroupId());
	//	auto enimyGroup = static_cast<Role::GroupType>(pRole->getGroupId());
	//	if (group != enimyGroup && enimyGroup != Role::GroupType::Npc)
	//	{
	//		return filterTargetCategory(skillType->m_targetCategory, pMapObj);
	//	}
	//}
	////作用友方
	//if (skillType->m_targetType & SkillType::TargetType::FriendlyCamp)
	//{
	//	//排除施法者自己
	//	if (pMapObj->getId() == casterData->getId())
	//	{
	//		return false;
	//	}
	//	if (!pRole) { return false; }
	//	auto group = static_cast<Role::GroupType>(casterData->getGroupId());
	//	auto enimyGroup = static_cast<Role::GroupType>(pRole->getGroupId());
	//	if (group == enimyGroup)
	//	{
	//		return filterTargetCategory(skillType->m_targetCategory, pMapObj);
	//	}
	//}
	return false;
}


void bzbee::LocalServer_Battle::restoreSaveSummonMob(int nCasterMapObjId, int nSkillId, int nSummonMapObjId)
{
	auto& casterMobs = m_summonMobs[nCasterMapObjId];
	casterMobs.emplace(nSkillId, nSummonMapObjId);

	auto& saveCasterMobs = m_saveSummonMobs[nCasterMapObjId];
	saveCasterMobs.emplace(nSkillId, nSummonMapObjId);
}

void bzbee::LocalServer_Battle::getSaveSummonMobs(int nCasterMapObjId, std::unordered_multimap<int, int>& result)
{
	auto it = m_saveSummonMobs.find(nCasterMapObjId);
	if (it != m_saveSummonMobs.end())
	{

		result = it->second;
	}
}

bool LocalServer_Battle::filterSkillTargetFunc(NS::int32 nCasterId, NS::int32 nCasterGroupId, std::shared_ptr<Role> targetData, const SkillType* skillType, const SkillType::Effect* effect, MapObj* obj)
{
	//if (targetData->isDead()) { return false; }

	//auto pMapRole = dynamic_cast<Role*>(obj);
	//if (pMapRole && pMapRole->isDead()) { return false; }

	////检查目标塔防是否可以被攻击
	//auto pBuilding = dynamic_cast<::TowerBuilding*>(obj);
	//if (pBuilding)
	//{
	//	if (pBuilding->getTowerBuildingDefence())
	//	{
	//		if (pBuilding->getTowerBuildingDefence()->m_canAttack == 0) return false;
	//	}
	//}
	////检查目标NPC是否可以被攻击
	//auto pMonster = dynamic_cast<Monster*>(obj);
	//if (pMonster)
	//{
	//	if (pMonster->getMonsterType())
	//	{
	//		if (pMonster->getMonsterType()->n_Camp == RoleCamp::mid || pMonster->isBeCatch()) return false;
	//	}
	//}

	//if (obj->getCategory() == MapObj::Category::Vehicle || obj->getCategory() == MapObj::Category::InteractiveBuilding) { return false; }

	//if ((effect->m_targetType & SkillType::TargetType::AnyGroup) == SkillType::TargetType::AnyGroup)
	//{
	//	return filterTargetCategory(effect->m_targetCategory, obj);
	//}

	//if (effect->m_targetType & SkillType::TargetType::Self)
	//{
	//	if (obj->getId() == nCasterId)
	//	{
	//		return true;
	//	}
	//}

	//if (effect->m_targetType & SkillType::TargetType::EnemyCamp)
	//{
	//	auto pEnimy = dynamic_cast<Role*>(obj);
	//	if (!pEnimy) { return false; }

	//	auto group = static_cast<Role::GroupType>(nCasterGroupId);
	//	auto enimyGroup = static_cast<Role::GroupType>(pEnimy->getGroupId());
	//	if (group != enimyGroup && enimyGroup != Role::GroupType::Npc)
	//	{
	//		return filterTargetCategory(effect->m_targetCategory, obj);
	//	}
	//}

	//if (effect->m_targetType & SkillType::TargetType::FriendlyCamp)
	//{
	//	if (obj->getId() == nCasterId)
	//	{
	//		return false;
	//	}

	//	auto pEnimy = dynamic_cast<Role*>(obj);
	//	if (!pEnimy) { return false; }

	//	auto group = static_cast<Role::GroupType>(nCasterGroupId);
	//	auto enimyGroup = static_cast<Role::GroupType>(pEnimy->getGroupId());
	//	if (group == enimyGroup && filterTargetCategory(effect->m_targetCategory, obj))
	//	{
	//		//检查特定的怪物ID
	//		auto pMonster = dynamic_cast<Monster*>(pEnimy);
	//		if (pMonster)
	//		{
	//			if (effect->m_mobid.size() == 1 && effect->m_mobid[0] == 0)
	//			{
	//				return true;
	//			}
	//			auto iter = std::find(effect->m_mobid.begin(), effect->m_mobid.end(), pMonster->getMonsterType()->n_ID);
	//			if (iter != effect->m_mobid.end())
	//			{
	//				return true;
	//			}
	//			return false;
	//		}
	//		return true;
	//	}
	//}

	return false;
}

// 过滤目标对象类型
bool LocalServer_Battle::filterTargetCategory(const SkillType::TargetCategory category, const MapObj* pTargetObj)
{
	if (!pTargetObj) { return false; }

	bool isTarget{ false };
	switch (pTargetObj->getCategory())
	{
	case MapObj::Category::Player:
		isTarget = (category & SkillType::TargetCategory::CategoryPlayer) == SkillType::TargetCategory::CategoryPlayer;
		break;
	case MapObj::Category::Monster:
		isTarget = (category & SkillType::TargetCategory::CategoryMonster) == SkillType::TargetCategory::CategoryMonster;
		break;
	case MapObj::Category::Npc:
		isTarget = (category & SkillType::TargetCategory::CategoryNpc) == SkillType::TargetCategory::CategoryNpc;
		break;
	case MapObj::Category::Pet:
		isTarget = (category & SkillType::TargetCategory::CategoryPet) == SkillType::TargetCategory::CategoryPet;
		break;
	case MapObj::Category::TowerBuilding:
		isTarget = (category & SkillType::TargetCategory::CategoryTowerBuilding) == SkillType::TargetCategory::CategoryTowerBuilding;
		break;
	case MapObj::Category::InteractiveBuilding:
		isTarget = (category & SkillType::TargetCategory::InteractiveBuilding) == SkillType::TargetCategory::InteractiveBuilding;
		break;
	default:
		break;
	}

	return isTarget;
}

// 计算技能伤害。
void LocalServer_Battle::skillDamage(NS::I64 serialId, int32 skill_id, int32 caster_id, int32 target_id)
{
	// 技能数据
	SkillDataPtr pSkillData = SkillDataMgr::getInstance().find(serialId);
	if (!pSkillData) { return; }

	// 目标，必需存在
	std::shared_ptr<MapObj> pTargetMapObject = MapObjMgr::getInstance().findObj(target_id);
	if (!pTargetMapObject) { return; }
	std::shared_ptr<Role> pTargetRoleData = std::dynamic_pointer_cast<Role>(pTargetMapObject);
	if (!pTargetRoleData) { return; }

	const SkillType* pSkillType = SkillCfg::getInstance().findSkillType(skill_id);
	if (!pSkillType) { return; }

	//if (pSkillType->m_skillType == SkillType::Type::InstantSkill && pSkillData->getCasterCategory() != MapObj::Category::InteractiveBuilding) // 互动家具无任何属性，血值始终为0
	//{
	//	// 直击技能命中时，要求施法者必需存活
	//	std::shared_ptr<MapObj> pCasterObject = MapObjMgr::getInstance().findObj(caster_id);//HandlerRole::findRole(target_id);
	//	if (!pCasterObject) return;

	//	auto pCasterData = std::dynamic_pointer_cast<Role>(pCasterObject);//std::make_shared<Role>(pGameObject);
	//	if (!pCasterData) {
	//		return;
	//	}
	//	if (pCasterData->getHp()) return;
	//}

	for (int32 index = 0; index < pSkillType->MAX_EFFECTS; index++)
	{
		//分离效果
		skillEffectDamage(pSkillData, pSkillType, caster_id, pTargetRoleData, index);
	}
}

void LocalServer_Battle::skillDamage(NS::I64 serialId, int32 skill_id, int32 caster_id, int32 target_id, int32 index)
{
	// 技能数据
	SkillDataPtr pSkillData = SkillDataMgr::getInstance().find(serialId);
	if (!pSkillData) { return; }

	// 施法者。
	std::shared_ptr<MapObj> pCasterObject = MapObjMgr::getInstance().findObj(caster_id);
	if (!pCasterObject) { return; }
	std::shared_ptr<Role> pCasterData = std::dynamic_pointer_cast<Role>(pCasterObject);
	if (!pCasterData) { return; }

	// 目标。
	std::shared_ptr<MapObj> pTargetObject = MapObjMgr::getInstance().findObj(target_id);
	if (!pTargetObject) { return; }
	std::shared_ptr<Role> pTargetData = std::dynamic_pointer_cast<Role>(pTargetObject);
	if (!pTargetData) { return; }

	if (pCasterData->getCategory() != MapObj::Category::Player && (pTargetData->getCtrlState() & MsgPB::SKILL_STATUS::SKILL_STATUS_CANT_BE_ATTACK_AI) == MsgPB::SKILL_STATUS::SKILL_STATUS_CANT_BE_ATTACK_AI)
	{
		//施法者不是玩家且目标身上有AI无敌BUFF，无法造成伤害
		return;
	}

	const SkillType* pSkillType = SkillCfg::getInstance().findSkillType(skill_id);
	if (!pSkillType) { return; }

	skillEffectDamage(pSkillData, pSkillType, caster_id, pTargetData, index);
}

void LocalServer_Battle::skillEffectDamage(std::shared_ptr<SkillData> pSkillData, const SkillType* pSkillType, int32 nCasterId, const std::shared_ptr<Role>& pTargetData, int32 index)
{
	if (!pSkillType) { return; }

	auto pEffect = pSkillType->findEffect(index);
	if (!pEffect) return;

	if (RandomUtil::getRandomInt(1, 1000) > pEffect->m_hitpercent) return; //效果触发概率

	auto needAlterTarget = !(pSkillType->m_skillType == SkillType::Type::BulletSkill
		|| pSkillType->m_skillType == SkillType::Type::CurveBulletSkill
		|| pSkillType->m_skillType == SkillType::Type::ParabolaSkill
		|| pSkillType->m_skillType == SkillType::Type::ThunderBulletSkill
		|| pSkillType->m_skillType == SkillType::Type::TeleportBulletSkill
		|| pSkillType->m_skillType == SkillType::Type::PreviewSkill
		|| pSkillType->m_skillType == SkillType::Type::RandomHeightBulletSkill
		|| pSkillType->m_skillType == SkillType::Type::VerticalBulletSkill);

	std::vector<int32> targets;
	//if (needAlterTarget && pEffect->m_range != 0 && pEffect->m_targetCount != 1)
	//{
	//	targets = MapObjMgr::getInstance().findNearestObjectsByMapObj(pTargetData.get(), static_cast<float>(pEffect->m_range), pEffect->m_targetCount, [this, nCasterId, pSkillData, pTargetData, pSkillType, pEffect](MapObj* obj)->bool {
	//		return filterSkillTargetFunc(nCasterId, pSkillData->getCasterGroupId(), pTargetData, pSkillType, pEffect, obj);
	//	});
	//}
	//else 
	if (pEffect->m_targetType == SkillType::TargetType::Self)
	{
		//技能效果目标类型是自己
		targets.push_back(nCasterId);
	}
	else
	{
		targets.push_back(pTargetData->getId());
	}

	if (targets.size() == 0) { return; }

	std::vector<float> hurtReduceVec(targets.size());
	// 被动技能不触发
	if (!pSkillType->IsPassivesSkill())
	{
		//命中前触发被动技能
		int index{ 0 };
		for (auto&& targetId : targets)
		{
			onBeforeDamage(pSkillData->getCasterGroupId(), targetId, hurtReduceVec[index++], pSkillData->getUniqueId());//触发影响本次伤害计算。
		}
	}

	std::shared_ptr<Role> pCasterRoleData{ nullptr };
	std::shared_ptr<MapObj> pCasterMapObj = MapObjMgr::getInstance().findObj(nCasterId);
	if (pCasterMapObj)
	{
		pCasterRoleData = std::dynamic_pointer_cast<Role>(pCasterMapObj);
	}

	SkillEffectParam skillEffectParam;
	if (pCasterRoleData)
	{
		skillEffectParam.pCaster = std::make_shared<MapCharacter>(CharacterAttributeSnapshot(pCasterRoleData), pCasterRoleData->getMaxHp(), pCasterRoleData->getMaxMp(), pSkillData->getCasterCategory(), pSkillData->getMonsterRare());
	}
	else
	{
		skillEffectParam.pCaster = std::make_shared<MapCharacter>(pSkillData->getCasterAttrSnapshot(), 0, 0, pSkillData->getCasterCategory(), pSkillData->getMonsterRare());
	}

	if (pEffect->m_targetType == SkillType::TargetType::Self)
	{
		skillEffectParam.pTarget = skillEffectParam.pCaster; //::make_shared<MapCharacter>(pCasterData);
	}
	else
	{
		skillEffectParam.pTarget = std::make_shared<MapCharacter>(CharacterAttributeSnapshot(pTargetData), pTargetData->getMaxHp(), pTargetData->getMaxMp(), pSkillData->getCasterCategory(), pSkillData->getMonsterRare());
	}
	skillEffectParam.CasterRoleValue.GetSkillRoleValue_ByCharacter(skillEffectParam.pCaster);
	skillEffectParam.TargetRoleValue.GetSkillRoleValue_ByCharacter(skillEffectParam.pTarget);

	auto effectType = static_cast<SKILL_EFFECT_TYPE>(pEffect->m_type);
	skillEffectParam.theEffect.SkillID = pSkillType->m_id;
	skillEffectParam.theEffect.type = effectType;
	skillEffectParam.theEffect.setmobid.insert(pEffect->m_mobid.begin(), pEffect->m_mobid.end());
	skillEffectParam.theEffect.range = pEffect->m_range;
	skillEffectParam.theEffect.targetcount = pEffect->m_targetCount;
	skillEffectParam.theEffect.hitpercent = pEffect->m_hitpercent;

    for (const auto& v : pEffect->m_effectAttrRefs)
    {
        skillEffectParam.theEffect.values.push_back(AttrEffectValue{ static_cast<ATTR_REF>(v.m_effectAttrref), v.m_effectValue });
    }

	SkillFormulaParam FormulaParam;
	std::vector<int> dmgValue(targets.size());
	std::vector<int> durability(targets.size());
	bool bCritical{ false };

	switch (effectType)
	{
	case SKILL_EFFECT_DMG_REAL:		// 真實傷害
	{
		skill::ISkillEffect_RealDmg<SkillRoleValue, SkillEffectParam, MapCharacter, MapCharacter, MapCharacter, AttrVector> realDmg;

		for (int i = 0; i < static_cast<int>(targets.size()); ++i)
		{
			skillEffectParam.theEffect.target = targets[i];
			dmgValue[i] = realDmg.CalEffect(skillEffectParam);
		}
	}
	break;

	case SKILL_EFFECT_DMG_WEAPON:	// 直接傷害(物理)
	{
		for (int i = 0; i < static_cast<int>(targets.size()); ++i)
		{
			skillEffectParam.theEffect.target = targets[i];

			SkillEffect_WeaponDmg().GetFormulaParam(FormulaParam, skillEffectParam);
			CEffectCalculation().CalEffect(FormulaParam, dmgValue[i], bCritical);
			//减伤计算
			dmgReduceEffect(nCasterId, targets[i], dmgValue[i], hurtReduceVec[i], (int)effectType, pSkillData->getUniqueId(), pSkillType->m_id, durability[i]);
		}
		break;
	}

	case SKILL_EFFECT_DMG_FIRE:		// 直接傷害(火)
	case SKILL_EFFECT_DMG_FORST:	// 直接傷害(冰)
	case SKILL_EFFECT_DMG_THUNDER:	// 直接傷害(雷)
	case SKILL_EFFECT_DMG_EARTH:	// 直接傷害(土)
	case SKILL_EFFECT_DMG_POSION:	// 直接傷害(毒)
	case SKILL_EFFECT_DMG_LIGHT:	// 直接傷害(光)
	case SKILL_EFFECT_DMG_DARK:		// 直接傷害(暗)
	{

		for (int i = 0; i < static_cast<int>(targets.size()); ++i)
		{
			skillEffectParam.theEffect.target = targets[i];
			SkillEffect_ElemtDmg(pSkillType->m_id).GetFormulaParam(FormulaParam, skillEffectParam);
			CEffectCalculation().CalEffect(FormulaParam, dmgValue[i], bCritical);
			//减伤计算
			dmgReduceEffect(nCasterId, targets[i], dmgValue[i], hurtReduceVec[i], (int)effectType, pSkillData->getUniqueId(), pSkillType->m_id, durability[i]);
		}
		break;
	}
	case SKILL_EFFECT_DIRECT_VAMPIRIC:
		//直接吸血，把效果作用对象强行改回施法者，后续逻辑与hp恢复走相同逻辑
		for (int i = 0; i < static_cast<int>(targets.size()); ++i)
		{
			targets[i] = nCasterId;
		}
	case SKILL_EFFECT_HP_RESTORE:	// HP恢復
	{
		for (int i = 0; i < static_cast<int>(targets.size()); ++i)
		{
			skillEffectParam.theEffect.target = targets[i];
			if (SkillEffect_Restore().CalEffect(skillEffectParam, dmgValue[i]) && dmgValue[i])
			{
				std::shared_ptr<MapObj> pTargetObject = MapObjMgr::getInstance().findObj(targets[i]);
				if (!pTargetObject) { continue; }
				std::shared_ptr<Role> pTargetData = std::dynamic_pointer_cast<Role>(pTargetObject);
				if (!pTargetData || pTargetData->isDead()) { continue; }

				MsgSkillEffectRestoreMS2C msg;
				msg.set_serialid(pSkillData->getUniqueId());
				msg.set_caster_mapobjectid(nCasterId);
				msg.set_target_mapobjectid(targets[i]);
				msg.set_skill_id(pSkillType->m_id);
				msg.set_hp_val(dmgValue[i]);
				int nCurHP = pTargetData->getHp() + dmgValue[i];
				int nMaxHP = pTargetData->getMaxHp();
				if (nCurHP > nMaxHP)
				{
					nCurHP = nMaxHP;
				}
				if (nCurHP < 0)
				{
					nCurHP = 0;
				}
				pTargetData->setHp(nCurHP);
				msg.set_currhp(dmgValue[i]);
				send(msg);
			}
		}
		return;
	}

	case SKILL_EFFECT_SUCKBLOOD:	//技能主动吸血，吸血量 == 伤害量

		for (int i = 0; i < static_cast<int>(targets.size()); ++i)
		{
			skillEffectParam.theEffect.target = targets[i];
			SkillEffect_ElemtDmg(pSkillType->m_id).GetFormulaParam(FormulaParam, skillEffectParam);
			CEffectCalculation().CalEffect(FormulaParam, dmgValue[i], bCritical);
			//减伤计算
			dmgReduceEffect(nCasterId, targets[i], dmgValue[i], hurtReduceVec[i], (int)effectType, pSkillData->getUniqueId(), pSkillType->m_id, durability[i]);

			//伤害量转化为施法者血量
			if (dmgValue[i])
			{
				int restoreValue = dmgValue[i];
				// 参数2为吸血的百分比
                int percent = pEffect->m_effectAttrRefs[1].m_effectValue;
                if (percent == 0) percent = 100;
                restoreValue = std::max(restoreValue * percent / 100, 1);

				std::shared_ptr<MapObj> pCasterObject = MapObjMgr::getInstance().findObj(nCasterId);
				if (!pCasterObject) { continue; }
				std::shared_ptr<Role> pCasterData = std::dynamic_pointer_cast<Role>(pCasterObject);
				if (!pCasterData) { continue; }

				MsgSkillEffectRestoreMS2C msg;
				msg.set_serialid(pSkillData->getUniqueId());
				msg.set_caster_mapobjectid(nCasterId);
				msg.set_target_mapobjectid(nCasterId);
				msg.set_skill_id(pSkillType->m_id);
				msg.set_hp_val(restoreValue);
				int nCurHP = pCasterData->getHp() + restoreValue;
				int nMaxHP = pCasterData->getMaxHp();
				if (nCurHP > nMaxHP)
				{
					nCurHP = nMaxHP;
				}
				if (nCurHP < 0)
				{
					nCurHP = 0;
				}
				pCasterData->setHp(nCurHP);
				msg.set_currhp(restoreValue);
				send(msg);
			}
		}
		break;
	case SKILL_EFFECT_BUFF:	// 增加Buff 陈东
	{
		if (pEffect->m_type != SkillType::Effect::Type::AddBuff)
		{
			break;
		}
		for (int i = 0; i < static_cast<int>(targets.size()); ++i)
		{
			skillEffectParam.theEffect.target = targets[i];
			for (int j = 0; j < skillEffectParam.theEffect.values.size(); j++)
			{
				if (skillEffectParam.theEffect.values[j].value == 0)
				{
					continue;
				}
				LocalServer_Buff::getInstance().addBuff(skillEffectParam.theEffect.values[j].value, 
					skillEffectParam.theEffect.target, nCasterId);
			}
		}
	}
	break;
	default:
		break;
	}
	for (int i = 0; i < static_cast<int>(targets.size()); ++i)
	{
		if (!dmgValue[i] && !durability[i]) continue;

		std::shared_ptr<MapObj> pTargetObject = MapObjMgr::getInstance().findObj(targets[i]);
		if (!pTargetObject) { continue; }
		std::shared_ptr<Role> pTargetData = std::dynamic_pointer_cast<Role>(pTargetObject);
		if (!pTargetData || pTargetData->isDead()) { continue; }

		int32 damage = dmgValue[i];

		//水晶被技能伤害，数值为1
		std::shared_ptr<Monster>  pMonster = std::dynamic_pointer_cast<Monster>(pTargetObject);
		if (pMonster && pMonster->getMonsterType() && 
			pMonster->getMonsterType()->n_nTowerTpy == MonsterCfg::MonsterType::MobTowerTypeCrystal) {
			damage = 1;
		}


		bool isTargetDead = pTargetData->isDead();

		// 伤害消息。
		MsgSkillEffectDmgMS2C msg;
		msg.set_serialid(pSkillData->getUniqueId());
		msg.set_skill_id(pSkillType->m_id);
		msg.set_caster_mapobjectid(nCasterId);
		msg.set_target_mapobjectid(targets[i]);
		//msg.set_target_skill(0);
		msg.set_dmg_val(damage);
		msg.set_durability(durability[i]);
		msg.set_criticl(bCritical ? 1 : 0);
		int nCurHP = std::max(pTargetData->getHp() - damage, 0);
		int nMaxHP = pTargetData->getMaxHp();
		if (nCurHP > nMaxHP)
		{
			nCurHP = nMaxHP;
		}
		if (nCurHP < 0)
		{
			nCurHP = 0;
		}
		msg.set_currhp(nCurHP);
		pTargetData->setHp(nCurHP);
		send(msg);

		if (msg.currhp() == 0) kill(targets[i], nCasterId, isTargetDead);

		//命中伤害后触发效果
		if (effectType == SKILL_EFFECT_DMG_WEAPON	//物理伤害
			|| effectType == SKILL_EFFECT_DMG_FIRE		// 直接傷害(火)
			|| effectType == SKILL_EFFECT_DMG_FORST		// 直接傷害(冰)
			|| effectType == SKILL_EFFECT_DMG_THUNDER		// 直接傷害(雷)
			|| effectType == SKILL_EFFECT_DMG_EARTH		// 直接傷害(土)
			|| effectType == SKILL_EFFECT_DMG_POSION		// 直接傷害(毒)
			|| effectType == SKILL_EFFECT_DMG_LIGHT		// 直接傷害(光)
			|| effectType == SKILL_EFFECT_DMG_DARK		// 直接傷害(暗)
			|| effectType == SKILL_EFFECT_SUCKBLOOD		// 吸血
			)
		{
			onAfterDamage(pSkillType->m_id, nCasterId, targets[i], damage, effectType, pSkillData->getUniqueId(), HurtSource::Skill);
		}

		// 击退
		if (msg.currhp() > 0)
		{
			//doBeHitFly(pCasterData->getId(), targets[i], serialId);
		}
	}
}


// 玩家普通攻击。
void LocalServer_Battle::onMsgPlayerNormalAttackC2S(const MsgPlayerNormalAttackC2S& msg)//lsc普攻
{
	LocalServer_Battle_NormalAttack evt;
	struct KillInfo
	{
		uint32 nMapObjID;
		bool isDead;
	};
	std::list<KillInfo> kills;


	MsgPlayerNormalAttackResS2C rs;
	rs.set_serialid(msg.serialid());

	auto pSelf = PlayerMgr::getInstance().getSelf();
	if (!pSelf)
		rs.set_res(MSG_PLAYER_RESULT::MSG_PLAYER_RESULT_ERR);
	else if (pSelf->isDead())
		rs.set_res(MSG_PLAYER_RESULT::MSG_PLAYER_RESULT_DEAD);
	else
	{
		//### ChenBK 进入战斗
		m_eBattleState = EN_USER_BATTLESTATE::USER_BATTLESTATE_IN;
		m_nLastBattleTime = std::chrono::steady_clock().now();

		// 攻击时移除复活保护 BUFF。
		/*LocalServer_Buff::getInstance().removeBuff(9999, pSelf->getId());*/
		// 攻击消失buff
		for (auto&& buff : pSelf->getBuffs())
		{
			if (!buff.getType()) { continue; }
			//获取buff的消失机制
			if ((buff.getType()->n_buff_del & BuffType::BUFF_DEL_TYPE::BY_ATTACK) == BuffType::BUFF_DEL_TYPE::BY_ATTACK)
			{
				LocalServer_Buff::getInstance().removeBuff(buff.getType()->n_ID, pSelf->getId());
			}
		}

		for (auto v : msg.target_obj())
		{
			evt.m_lstTargetMapObjID.push_back(v);

			std::shared_ptr<MapObj> pGameObject = MapObjMgr::getInstance().findObj(v);//HandlerRole::findRole(target_id);
			if (!pGameObject) continue;;

			auto pTargetData = std::dynamic_pointer_cast<Role>(pGameObject);//std::make_shared<Role>(pGameObject);
			if (!pTargetData) {
				return;
			}

			int dmgValue{};
			int durability{};
			bool bCritical{};
			if (!ProcessFight(pSelf, pTargetData, dmgValue, bCritical, msg.idhp(), durability))
				continue;

			auto pInfo = rs.add_targetinfo();
			pInfo->set_mapobjectid(v);
			pInfo->set_damage(dmgValue);
			pInfo->set_durability(durability);
			pInfo->set_criticl(bCritical ? 1 : 0);

			auto currHp = pTargetData->getHp() - dmgValue;
			if (currHp > 0) {
				pInfo->set_currhp(currHp);
				pTargetData->setHp(currHp);
			}
			else
			{
				pInfo->set_currhp(0);
				pTargetData->setHp(0);
				pTargetData->setDead(true);
				KillInfo infoKill;
				infoKill.nMapObjID = pTargetData->getId();
				infoKill.isDead = pTargetData->isDead();
				kills.push_back(infoKill);
			}

			//受击后触发效果， 普攻->物理伤害
			onAfterDamage(0, pSelf->getId(), v, dmgValue, (int)SKILL_EFFECT_DMG_WEAPON, msg.idhp(), HurtSource::Normal_Attack);

			// 击退
			if (pInfo->currhp() > 0)
			{
				doBeHitFly(pSelf->getId(), v, msg.idhp(), msg.isfinalblow());
			}
		}
		rs.set_res(MSG_PLAYER_RESULT::MSG_PLAYER_RESULT_OK);
	}
	postEvent(evt);

	rs.set_idhp(msg.idhp());
	send(rs);

	for (auto infoKill : kills)
		kill(infoKill.nMapObjID, pSelf->getId(), infoKill.isDead);
}

// 玩家施放技能。
void LocalServer_Battle::onMsgSkillCastSkillC2GS(const MsgSkillCastSkillC2GS& msg)
{
	MsgSkillCastSkillResultGS2C rs;
	rs.set_serialid(msg.serialid());

	auto pSelf = PlayerMgr::getInstance().getSelf();
	if (!pSelf)
		rs.set_result(CASTSKILL_CASTER_FAILE);
	else
	{
		//### ChenBK 进入战斗
		m_eBattleState = EN_USER_BATTLESTATE::USER_BATTLESTATE_IN;
		m_nLastBattleTime = std::chrono::steady_clock().now();

		auto pCfg = SkillCfg::getInstance().findSkillType(msg.debug_skill_id());

		if (pSelf->getMp() < pCfg->m_costMp)
			rs.set_result(CASTSKILL_MP_LACKING);
		else
		{
			// 攻击时移除复活保护 BUFF。
			//LocalServer_Buff::getInstance().removeBuff(9999, pSelf->getId());
			// 攻击消失buff
			for (auto&& buff : pSelf->getBuffs())
			{
				if (!buff.getType()) { continue; }
				//获取buff的消失机制
				if ((buff.getType()->n_buff_del & BuffType::BUFF_DEL_TYPE::BY_ATTACK) == BuffType::BUFF_DEL_TYPE::BY_ATTACK)
				{
					LocalServer_Buff::getInstance().removeBuff(buff.getType()->n_ID, pSelf->getId());
				}
			}

			skillBuffOnlySelf(msg.debug_skill_id(), pSelf->getId());

			for (auto v : msg.target_mapobjectids())
			{
				std::shared_ptr<MapObj> pGameObject = MapObjMgr::getInstance().findObj(v);//HandlerRole::findRole(target_id);
				if (!pGameObject) continue;;

				auto pTargetData = std::dynamic_pointer_cast<Role>(pGameObject);//std::make_shared<Role>(pGameObject);
				if (!pTargetData) {
					return;
				}

				SkillEffectParam skillEffectParam;
				skillEffectParam.pCaster = std::make_shared<MapCharacter>(CharacterAttributeSnapshot(pSelf), pSelf->getMaxHp(), pSelf->getMaxMp(), pSelf->getCategory(), pSelf->getMobRare());
				skillEffectParam.pTarget = std::make_shared<MapCharacter>(CharacterAttributeSnapshot(pTargetData), pTargetData->getMaxHp(), pTargetData->getMaxMp(), pTargetData->getCategory(), pTargetData->getMobRare());
				skillEffectParam.CasterRoleValue.GetSkillRoleValue_ByCharacter(skillEffectParam.pCaster);
				skillEffectParam.TargetRoleValue.GetSkillRoleValue_ByCharacter(skillEffectParam.pTarget);

				SkillFormulaParam FormulaParam;
				SkillEffect_ElemtDmg(msg.debug_skill_id()).GetFormulaParam(FormulaParam, skillEffectParam);

				int dmgValue;
				bool bCritical;
				CEffectCalculation().CalEffect(FormulaParam, dmgValue, bCritical);
			}
			rs.set_result(CASTSKILL_SUCCESS);
		}
	}
	send(rs);

	if (rs.result() == CASTSKILL_SUCCESS)
	{
		auto pCfg = SkillCfg::getInstance().findSkillType(msg.debug_skill_id());

		// 扣蓝。
		MsgPlayerInfoGS2C msgPlayerInfo;
		msgPlayerInfo.set_iggid(pSelf->getIggId());
		msgPlayerInfo.set_mp(pSelf->getMp() - pCfg->m_costMp);
		send(msgPlayerInfo);

		// 扣除物品。
		//if (pCfg->m_costItem) LocalServer_ItemPacket::getInstance().ItemCurrencyCost(pCfg->m_costItem, 1, 0, 0, 0, HandlerItem::getInstance().getCurrPacketType());

		std::vector<int> targets;
		for (auto v : msg.target_mapobjectids())
		{
			targets.emplace_back(v);
		}

		if (pCfg->m_singingTime <= 0)
		{
			skillEffect(CastingSkillInfo{ msg.debug_skill_id(), pSelf->getId(), msg.serialid(),
				static_cast<float>(pCfg->m_singingTime / 1000.0f), targets });
		}
		else
		{
			m_castingSkills.emplace_back(CastingSkillInfo{ msg.debug_skill_id(), pSelf->getId(), msg.serialid(),
				static_cast<float>(pCfg->m_singingTime / 1000.0f), targets });
		}
	}
}

void LocalServer_Battle::skillBuffOnlySelf(int32 skill_id, int32 caster_id)
{
	auto pType = SkillCfg::getInstance().findSkillType(skill_id);
	if (!pType) return;

	for (int i = 0; i < SkillType::MAX_EFFECTS; ++i)
	{
		auto pEffect = pType->findEffect(i);
		if (!pEffect || pEffect->m_type != SkillType::Effect::Type::AddBuff) continue;

		auto buffId = pEffect->m_effectAttrRefs[0].m_effectValue;

		if (pEffect->m_targetType == SkillType::TargetType::Self)
			LocalServer_Buff::getInstance().addBuff(buffId, caster_id, caster_id);
	}
}

void LocalServer_Battle::skillBuffRemove(int32 skill_id, int32 caster_id, int32 target_id /* = 0 */)
{
	//const SkillType* pSkillType = SkillCfg::getInstance().findSkillType(skill_id);
	//if (!pSkillType) return;
	////直接移除buff
	//for (int i = 0; i < SkillType::MAX_EFFECTS; ++i)
	//{
	//	const SkillType::Effect* pEffect = pSkillType->findEffect(i);
	//	if (pEffect && pEffect->m_type == SkillType::Effect::Type::AddBuff)
	//	{
	//		int32 removeBuffId = pEffect->m_effectAttrRefs[0].m_effectValue;
	//		if (removeBuffId > 0)
	//		{
	//			if (target_id <= 0) //根据施法类型检索目标
	//			{
	//				// 移除自身buff
	//				if ((pEffect->m_targetType & SkillType::TargetType::Self) == SkillType::TargetType::Self)
	//				{
	//					LocalServer_Buff::getInstance().removeBuff(removeBuffId, caster_id);
	//				}
	//				// 移除友方Npc Buff
	//				if ((pEffect->m_targetType & SkillType::TargetType::FriendlyCamp) == SkillType::TargetType::FriendlyCamp && (pEffect->m_targetCategory & SkillType::TargetCategory::CategoryNpc) == SkillType::TargetCategory::CategoryNpc)
	//				{
	//					int32 followNpcMapOjbId = UiCommon::getInstance().getFollowNpcObjId();
	//					if (followNpcMapOjbId > 0)
	//					{
	//						LocalServer_Buff::getInstance().removeBuff(removeBuffId, followNpcMapOjbId);
	//					}
	//				}
	//				// 移除友方Pet Buff
	//				if ((pEffect->m_targetType & SkillType::TargetType::FriendlyCamp) == SkillType::TargetType::FriendlyCamp && (pEffect->m_targetCategory & SkillType::TargetCategory::CategoryPet) == SkillType::TargetCategory::CategoryPet)
	//				{
	//					GameObject* pGameObject = HandlerRole::getInstance().findRole(caster_id);
	//					if (!pGameObject) continue;
	//					PlayerComponent* pCaster = pGameObject->findComponent<PlayerComponent>();
	//					if (!pCaster) continue;
	//					PetComponent* pPetComponent = pCaster->getActivePet();
	//					if (!pPetComponent || !pPetComponent->getData()) continue;
	//					if (pPetComponent->getData()->getId() > 0)
	//					{
	//						LocalServer_Buff::getInstance().removeBuff(removeBuffId, pPetComponent->getData()->getId());
	//					}
	//				}
	//			}
	//			else
	//			{
	//				//移除目标的Buff
	//				LocalServer_Buff::getInstance().removeBuff(removeBuffId, target_id);
	//			}
	//		}
	//	}
	//}
}

// 技能附带的 BUFF。
void LocalServer_Battle::skillBuff(int32 skill_id, int32 caster_id, int32 target_id/* = 0*/)
{
	//auto pType = SkillCfg::getInstance().findSkillType(skill_id);
	//if (!pType) return;

	//// 施法者。
	//auto pGameObject = HandlerRole::findRole(caster_id);
	//if (!pGameObject) return;
	//auto pCaster = pGameObject->findComponent<RoleComponent>();
	//auto pCasterData = pCaster->getData();
	//// 目标。
	//std::shared_ptr<Role> pTargetData;
	//if (target_id == 0 || target_id == caster_id)
	//{
	//	target_id = caster_id;
	//	pTargetData = pCasterData;
	//}
	//else
	//{
	//	pGameObject = HandlerRole::findRole(target_id);
	//	if (!pGameObject) return;
	//	pTargetData = pGameObject->findComponent<RoleComponent>()->getData();
	//}

	//for (int i = 0; i < SkillType::MAX_EFFECTS; ++i)
	//{
	//	auto pEffect = pType->findEffect(i);
	//	if (!pEffect || pEffect->m_type != SkillType::Effect::Type::AddBuff) continue;

	//	if (RandomUtil::getRandomInt(1, 1000) > pEffect->m_hitpercent) continue; //触发概率

 //       auto buffId = pEffect->m_effectAttrRefs[0].m_effectValue;

 //       std::vector<int32> targets;
	//	bool needAlterTarget = true;
	//	if (pEffect->m_targetCount == 1)
	//	{
	//		if ((pEffect->m_targetType == SkillType::TargetType::Self && caster_id == target_id) //施法目标自己，且目标 == 施法者
	//			|| (pEffect->m_targetType == SkillType::TargetType::EnemyCamp && pTargetData->getGroupId() != pCasterData->getGroupId() && static_cast<Role::GroupType>(pTargetData->getGroupId()) != Role::GroupType::Npc)
	//			|| (((pEffect->m_targetCategory & SkillType::TargetCategory::CategoryNpc) == SkillType::TargetCategory::CategoryNpc) && pTargetData->getCategory() == MapObj::Category::Npc) //施法目标包含NPC，目标 == 友方NPC
	//			|| (((pEffect->m_targetCategory & SkillType::TargetCategory::CategoryPet) == SkillType::TargetCategory::CategoryPet) && pTargetData->getCategory() == MapObj::Category::Pet) //施法目标包含宠物，目标 == 宠物
	//			)
	//		{
	//			needAlterTarget = false;
	//		}
	//	}

	//	if (pEffect->m_targetType == SkillType::TargetType::Self)
	//	{
	//		if (filterTargetCategory(pEffect->m_targetCategory, pCasterData.get()))
	//		{
	//			targets.push_back(caster_id);
	//		}
	//	}
	//	else if (needAlterTarget && pEffect->m_range != 0 && !(pEffect->m_targetCount == 1 && pEffect->m_targetType == SkillType::TargetType::Self))
	//	{
	//		// 技能效果配置中的效果范围和效果对象数量
	//		targets = MapObjMgr::getInstance().findNearestObjectsByMapObj(pTargetData.get(), static_cast<float>(pEffect->m_range), pEffect->m_targetCount, [this, pCasterData, pTargetData, pType, pEffect](MapObj* obj)->bool {
	//			return filterSkillTargetFunc(pCasterData->getId(), pCasterData->getGroupId(), pTargetData, pType, pEffect, obj);
	//		});
	//	}
	//	else
	//	{
	//		targets.push_back(target_id);
	//	}

	//	if (targets.size() == 0) { continue; }

	//	// 玩家免疫效果
	//	bool isPlayerImmunity{ false };
	//	const BuffType* pBuffType = BuffCfg::getInstance().findBuffType(buffId);
	//	if (!pBuffType) { continue; }
	//	for (auto&& effect : pBuffType->effects)
	//	{
	//		if (effect.type == MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_SKILL_STATUS)
	//		{
	//			isPlayerImmunity = effect.ref_values[0].value & MsgPB::SKILL_STATUS::SKILL_STATUS_TAUNT		// 嘲讽效果
	//				|| effect.ref_values[0].value & MsgPB::SKILL_STATUS::SKILL_STATUS_CONFUSION				// 混乱效果
	//				|| effect.ref_values[0].value & MsgPB::SKILL_STATUS::SKILL_STATUS_CHANGE_GROUP;			// 魅惑效果

	//			break;
	//		}
	//	}

	//	for (auto target : targets)
	//	{
	//		//过滤指定ID对象
	//		auto&& targetRole = findRole(target);
	//		if (!targetRole || targetRole->getData()->isDead()) continue;

	//		if (targetRole->getData() && targetRole->getData()->getCategory() == MapObj::Category::Player && isPlayerImmunity)
	//		{
	//			//玩家免疫部分buff效果，嘲讽 / 混乱 / 魅惑
	//			continue;
	//		}

	//		if (pEffect->m_mobid.size() == 1 && pEffect->m_mobid[0] == 0)
	//		{
	//			LocalServer_Buff::getInstance().addBuff(buffId, target, caster_id);
	//		}
	//		else
	//		{
	//			auto&& pMonster = dynamic_cast<Monster*>(targetRole->getData().get());
	//			if (!pMonster) continue;

	//			auto&& iter = std::find(pEffect->m_mobid.begin(), pEffect->m_mobid.end(), pMonster->getMonsterType()->n_ID);
	//			if (iter != pEffect->m_mobid.end())
	//			{
	//				LocalServer_Buff::getInstance().addBuff(buffId, target, caster_id);
	//			}
	//		}
	//	}
	//}
}


// 击杀。
void LocalServer_Battle::kill(int32 target_id, int32 caster_id, bool isTargetDead)
{
	//auto pGameObject = HandlerRole::findRole(target_id);
	//if (!pGameObject) return;
	//auto pTargetData = pGameObject->findComponent<RoleComponent>()->getData();
	//if (isTargetDead) return;

	////特殊怪物死亡流程
	//auto pMonsterCom = pGameObject->findComponent<MonsterComponent>();
	//if (pMonsterCom)
	//{
	//	auto pMonData = pMonsterCom->getMonsterData();
	//	if (pMonData)
	//	{
	//		if (GlobalCfg::getInstance().getMadAlchemistBossId() == pMonData->getDefId())
	//		{
	//			auto madAlcCmp = dynamic_cast<MadAlchemistBossComponent*>(pMonsterCom);
	//			if (madAlcCmp && madAlcCmp->getBossState() == MadAlchemistBossComponent::E_Boss_State::Fight)
	//			{
	//				return;
	//			}
	//		}
	//	}
	//}

	////怪物死亡触发技能
	//if (pTargetData->getCategory() == MapObj::Category::Monster)
	//{
	//	if (auto pMonsterData = dynamic_cast<Monster*>(pTargetData.get()))
	//	{
	//		if (auto pMonsterCfg = MonsterCfg::getInstance().findMonsterType(pMonsterData->getDefId()))
	//		{
	//			if (pMonsterCfg->n_DeadSkill) cast_skill(pMonsterCfg->n_DeadSkill, target_id, target_id);
	//		}
	//	}
	//}

	////buff效果，死亡触发技能
	//auto&& buffs = pTargetData->getBuffs();
	//if (buffs.size() > 0)
	//{
	//	for (auto&& buff : buffs)
	//	{
	//		auto&& buffType = buff.getType();
	//		if (!buffType) continue;
	//		//遍历效果类型，检查是否有死亡触发的技能
	//		for (int i = 0; i < BuffType::EFFECT_NUM; ++i)
	//		{
	//			if ((BUFF_EFFECT_TYPE)buffType->effects[i].type == BUFF_EFFECT_TYPE::BUFF_EFFECT_CAST_SKILL_ON_DEAD)
	//			{
	//				assert(buffType->effects[i].ref_values.size() > 0);
	//				int skill_id = buffType->effects[i].ref_values[0].value;//死亡触发技能固定配置在buff效果第一组
	//				auto pSkillType = SkillCfg::getInstance().findSkillType(skill_id);
	//				if (pSkillType)
	//				{
	//					bool pIsSummon = false;//是否是召唤技能
	//					bool pIsSummonOk = false;//是否成功触发召唤技能
	//					for (int32 index = 0; index < pSkillType->MAX_EFFECTS; index++) {
	//						const SkillType::Effect* pEffect = pSkillType->findEffect(index);
	//						if (pEffect->m_type == SkillType::Effect::Type::Summon) {
	//							pIsSummon = true;
	//						}
	//					}
	//					if (pIsSummon) {
	//						pGameObject = HandlerRole::findRole(target_id);
	//						if (pGameObject) {
	//							auto pTarget = pGameObject->findComponent<RoleComponent>();
	//							if (pTarget) {
	//								cast_skill(skill_id, target_id, pTarget->getWorldPos());
	//								pIsSummonOk = true;
	//							}
	//						}
	//					}
	//					if (! pIsSummonOk)
	//					{
	//						cast_skill(skill_id, target_id, target_id);
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	//PassiveEvent_KillSomeone evt;
	//evt.m_nAtkMapObjId = caster_id;
	//evt.m_nHitMapObjId = target_id;
	////击杀目标，施法者触发被动技能
	//NS::GameObject* p_casterGameObject = HandlerRole::findRole(caster_id);
	//if (p_casterGameObject)
	//{
	//	RoleComponent* pCasterRoleComponent = p_casterGameObject->findComponent<RoleComponent>();
	//	if (pCasterRoleComponent)
	//	{
	//		pCasterRoleComponent->postEvent(evt);
	//	}
	//}
	////被击杀者触发被动技能
	//RoleComponent* pTargetRoleComponent = pGameObject->findComponent<RoleComponent>();
	//if (pTargetRoleComponent)
	//{
	//	pTargetRoleComponent->postEvent(evt);
	//}

	MsgCharacterDeadS2C msg;
	msg.set_mapobjectid(target_id);
	send(msg);

	////客户端生怪移除
	//gLocalServer_Survival.CharacterDead(caster_id, target_id);	// 一定要在removeMobData之前触发

	//gLocalServerSurvivalEndless.mobKill(target_id);
	////LocalServer_BirthMob::getInstance().removeMobData(target_id);

	////zjy 战斗NPC杀死怪物触发成就
	//LocalServer_Npc::getInstance().onNpcKillMonsterAchieve(caster_id, target_id);

	//// 判断是否是玩家一方直接或间接击杀目标。
	bool playerGroup = false;
	I64 i64IggId = 0;
	if (auto pSelf = PlayerMgr::getInstance().getSelf())
	{
		i64IggId = pSelf->getIggId();
		playerGroup = true;
		//else if (auto pCasterGameObject = HandlerRole::findRole(caster_id))
		//{
		//	if (auto pCasterData = pCasterGameObject->findComponent<RoleComponent>()->getData())
		//	{
		//		if (pCasterData->getGroupId() == pSelf->getGroupId())
		//		{
		//			playerGroup = true;
		//		}
		//		else
		//		{
		//			for (auto& buff : pCasterData->getBuffs())
		//			{
		//				auto pBuffType = buff.getType();
		//				if (!pBuffType) continue;

		//				bool playerGroupBuff = false;
		//				if (buff.getCasterId() == pSelf->getId())
		//				{
		//					playerGroupBuff = true;
		//				}
		//				else if (auto pRoleObj = HandlerRole::findRole(buff.getCasterId()))
		//				{
		//					auto pRoleData = pRoleObj->findComponent<RoleComponent>()->getData();
		//					if (pRoleData->getGroupId() == pSelf->getGroupId())
		//					{
		//						playerGroupBuff = true;
		//					}
		//				}
		//				if (!playerGroupBuff) continue;

		//				for (int i = 0; i < BuffType::EFFECT_NUM; ++i)
		//				{
		//					const auto& effect = pBuffType->effects[i];
		//					if (effect.type == 301)
		//					{
		//						assert(effect.ref_values.size() > 0);
		//						if (effect.ref_values[0].value & 512)	// 混乱效果。
		//						{
		//							playerGroup = true;
		//							break;
		//						}
		//					}
		//				}
		//				if (playerGroup) break;
		//			}
		//		}
		//	}
		//}
	}

	bool bSendKillMob = true;
	int nMobTypeId = 0;
	//// 玩家一方直接或间接击杀怪物，获取经验。
	if (playerGroup)
	{
		killMobAddExp(target_id, caster_id);

		//if (auto pMonsterData = std::dynamic_pointer_cast<Monster>(pTargetData))
		//{
			//SyncCache::getInstance().addMobKill(pMonsterData->getDefId());
			//gLocalServer_Achieve.OnKillMobEvent(pMonsterData->getDefId());

			//auto monsterType = pMonsterData->getMonsterType();
			//if (monsterType)
			//{
			//	if ((int)monsterType->n_rare >= GlobalCfg::getInstance().getReportKillMobRare())
			//	{
			//		bSendKillMob = true;
			//		nMobTypeId = monsterType->n_ID;
			//	}
			//}
		//}
	}

	//// 只要被击杀的目标是怪物，都要掉落 （假人非跟随状态除外）。
	//std::vector<DropObjInfo> vecDrop;
	//if (!(LocalServer_Dummy::getInstance().IsDummyPlayer(caster_id) && !LocalServer_Dummy::getInstance().IsFollowDummyPlayer(caster_id)))
	//{
	//	int nMultiplyingPower = 1;
	//	if (playerGroup)
	//	{
	//		nMultiplyingPower = LocalServer_Buff::getInstance().DropBuffMultiplyingPower(caster_id);			
	//	}
	//	for (int i = 0; i < nMultiplyingPower; i++)
	//	{
	//		std::vector<DropObjInfo> vDropObj = DoDrop(target_id, -1, caster_id);
	//		vecDrop.insert(vecDrop.end(), vDropObj.begin(), vDropObj.end());
	//	}
	//}

	if (i64IggId > 0 )
	{
		//std::shared_ptr<MapData> mapData{ MapDataMgr::getInstance().getCurrMapData() };
		//if (mapData) {
			MsgReportKillMobRequest reportMsg;
			reportMsg.set_iggid(i64IggId);
			reportMsg.set_mob_type_id(target_id);
			reportMsg.set_map_type_id(caster_id);
			//if (vecDrop.size() > 0)
			//{
			//	for (auto drop : vecDrop)
			//	{
			//		NS::I64 dropid = (NS::I64)drop.m_dataid * 10000;
			//		dropid += drop.m_Count;
			//		reportMsg.add_drop_lst(dropid);
			//	}
			//}

			MsgMgr::getInstance().sendMessageDirect(_MSG_REPORT_KILL_MOB, &reportMsg);
		//}
	}
}


// 经验。
void LocalServer_Battle::killMobAddExp(int32 target_id, int32 caster_id)
{
	//if (MapDataMgr::getInstance().getCurMapType() != MapDataMgr::MapType::Private)
	//{
	//	return;
	//}
	//GameObject* pGameObject = HandlerRole::findRole(target_id);
	//if (!pGameObject)
	//	return;

	//auto pLocalMobData = LocalServer_BirthMob::getInstance().getMobInfo(target_id);
	//if (pLocalMobData && pLocalMobData->m_bDisableAddExp)
	//	return;

	//auto pMonsterCom = pGameObject->findComponent<MonsterComponent>();
	//if (!pMonsterCom)
	//	return;

	//auto pMonData = pMonsterCom->getMonsterData();
	//if (!pMonData)
	//	return;

	////int32 MonID = pMonData->getDefId();

	////const MonsterCfg::MonsterType* pCfg = MonsterCfg::getInstance().findMonsterType(MonID);
	//auto pCfg = pMonData->getMonsterType();
	//if (!pCfg)
	//	return;

	//addExp(pCfg->n_Exp);
	//GameLog::getInstance().addExp(pCfg->n_Exp, GameLog::E_EXP_ADD_BATTLE, pCfg->n_ID);
	//gLocalServer_Pet.OnOwnerAddExp(pCfg->n_Exp);
}


// 升级
bool LocalServer_Battle::levelUp(NS::I64 newExp, MsgPlayerInfoGS2C& msg, const std::shared_ptr<Player> pSelf)
{
	//bzbee::GameObject* pGameObjectPlayer = PlayerComponentMgr::getInstance().findPlayer(pSelf->getId());
	//assert(pGameObjectPlayer);
	//if (!pGameObjectPlayer)
	//	return false;
	//RoleComponent* pRoleCompPlayer = pGameObjectPlayer->findComponent<RoleComponent>();
	//assert(pRoleCompPlayer);
	//if (!pRoleCompPlayer)
	//	return false;
	//CharacterAttributeComponent* pCharacterAttributeCompPlayer = pRoleCompPlayer->findComponent<CharacterAttributeComponent>();
	//assert(pCharacterAttributeCompPlayer);
	//if (!pCharacterAttributeCompPlayer)
	//	return false;

	//// 判断是否需要升级
	//bool bLevelUp{ false };
	//int32 newLev{ pSelf->getLevel() };

	//auto&& currLevCfg = PlayerAttrCfg::getInstance().findPlayerAttr(newLev);
	//while(currLevCfg && newExp >= currLevCfg->n_exp && newLev < PlayerAttrCfg::getInstance().m_maxLevel)
	//{
	//	bLevelUp = true;
	//	currLevCfg = PlayerAttrCfg::getInstance().findPlayerAttr(++newLev);
	//}

	//if (!bLevelUp)
	//	return false;

	//// 防止超过 100级.
	//newLev = std::min(newLev, 100);

	//// 重新计算属性
	//Player newAttr = *pSelf.get();
	//newAttr.setLevel(newLev);
	//newAttr.setPrivateLev(newLev);
	//newAttr.calcAttr();

	//msg.set_lv(newLev);
	//bool isDeath = pSelf->isDead() || pCharacterAttributeCompPlayer->getRealHp() <= 0;
	//if (!isDeath)
	//{
	//	msg.set_hp(static_cast<int32>(newAttr.getAttr(MsgPB::ROLE_EFFECT_HPM)));
	//	msg.set_mp(static_cast<int32>(newAttr.getAttr(MsgPB::ROLE_EFFECT_MPM)));
	//}
	//msg.set_maxhp(static_cast<int32>(newAttr.getAttr(MsgPB::ROLE_EFFECT_HPM)));
	// TODO: new Attr Data.
	return true;
}


// 掉落。
//std::vector<DropObjInfo> LocalServer_Battle::DoDrop(int32 target_id, int32 drop_id, int32 caster_id)
//{
//	std::vector<DropObjInfo>vDropObj;
//	GameObject* pGameObject = HandlerRole::findRole(target_id);
//	if (!pGameObject)
//		return vDropObj;
//
//	auto pMonsterCom = pGameObject->findComponent<MonsterComponent>();
//	if (!pMonsterCom)
//		return vDropObj;
//
//	auto pMonData = pMonsterCom->getMonsterData();
//	if (!pMonData)
//		return vDropObj;
//
//	auto pLocalMobData = LocalServer_BirthMob::getInstance().getMobInfo(target_id);
//	if (pLocalMobData && pLocalMobData->m_bDisableDrop)
//		return vDropObj;
//
//	int32 RealDropID = drop_id;
//	int32 MonID = pMonData->getDefId();
//	const MonsterCfg::MonsterType* pCfg = MonsterCfg::getInstance().findMonsterType(MonID);
//	if (!pCfg)
//		return vDropObj;
//
//	if (RealDropID < 0)
//		RealDropID = pCfg->n_DropId;
//
//	CommonMonsterComponent* pCommonMonsterComponent = dynamic_cast<CommonMonsterComponent*>(pMonsterCom);
//	if (pCommonMonsterComponent)
//	{
//		int32 nRobNpcDakDropID = pCommonMonsterComponent->GetRobNpcDakDropID();
//		if (nRobNpcDakDropID > 0)
//			RealDropID = nRobNpcDakDropID;
//	}
//
//	auto pSelf = PlayerMgr::getInstance().getSelf();
//	glm::vec3 worldPos = pMonsterCom->getWorldPos();
//	auto pMap = MapDataMgr::getInstance().getCurrMapData();
//	if (!pMap)
//		return vDropObj;
//	glm::ivec2 coordOut;
//	pMap->worldPosToCoordinate(coordOut, worldPos);
//	if (pMap->isBasicTile(coordOut))
//	{
//		// 如果掉落的位置是基础块，则调整到一个玩家可以站立的地方
//		coordOut = pMap->GetNotInBlockingBasicTile(coordOut, 1, 2, true, 10, 30);
//	}	
//
//	if (MonsterCfg::getInstance().getSrvDropRare() <= (int)pCfg->n_rare)
//	{
//		int nDropCacheId = MapDataUtil::createLocalMapObjID();
//		if (pLocalMobData)
//		{
//			LocalServer_BirthMob::E_BirthType eBirthType = pLocalMobData->m_eBirthType;
//			int nParm_1 = -10000;
//			int nParm_2 = -10000;
//			if (eBirthType == LocalServer_BirthMob::E_BirthBox)
//			{
//				nParm_1 = pLocalMobData->m_nBirthCfgID;
//				nParm_2 = pLocalMobData->m_nExpID;
//			}
//
//			HandlerPlayerCacheSync::getInstance().addServerDropCache(nDropCacheId, MonID, RealDropID, coordOut, nParm_1, nParm_2);
//		}
//		bool isBoss = pCfg->n_rare == MonsterCfg::MonsterTypeRare::BOSS;
//		if (isBoss)
//		{
//			TrackballTouchDelegate::getInstance().setDisableTrackball(true);
//		}
//		MsgPB::MsgGetDropInfoC2S msg;
//		msg.set_pos_x(coordOut.x);
//		msg.set_pos_y(coordOut.y);
//		msg.set_drop_id(RealDropID);
//		msg.set_type(MsgPB::ENUM_GET_DROP_TYPE_KILL_MOB);
//		msg.set_check_data(MonID);
//		msg.set_serial(nDropCacheId);
//		MsgMgr::getInstance().sendMessage(_MSG_GET_DROP_INFO_C2S, &msg);
//		if (isBoss)
//		{
//			TrackballTouchDelegate::getInstance().setDisableTrackball(false);
//		}
//		return vDropObj;
//	}
//
//	LocalServer_BuildLogicCli::getInstance().DoPrivateDropID(vDropObj, pSelf->getIggId(),
//		coordOut.x, coordOut.y, RealDropID);
//	if (!vDropObj.empty()) {
//		MsgMobDropG2C msgDrop;
//		msgDrop.set_iggid(pSelf->getIggId());
//		msgDrop.set_mobinsid(pMonData->getInsId());
//		msgDrop.set_mapobjectid(pMonData->getId());
//		auto& pos = *msgDrop.mutable_pos();
//		pos.set_x(coordOut.x);
//		pos.set_y(coordOut.y);
//		for (const auto& dropInfo : vDropObj) {
//			if (dropInfo.m_Count > 0) {
//				DropMgrABS::DropInfo2PB(dropInfo, msgDrop.add_drop_info());
//			}
//		}
//		if (msgDrop.drop_info_size() > 0)
//			send(msgDrop);
//	}
//	return vDropObj;
//}

void LocalServer_Battle::update(float dt)
{
	Super::update(dt);

	//if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock().now() - m_nLastBattleTime).count() > 6)
	//{
	//	if (m_eBattleState == USER_BATTLESTATE_IN)
	//	{
	//		m_eBattleState = USER_BATTLESTATE_OUT;
	//		LocalServer_ItemPacket::getInstance().OnBattleOut();
	//	}
	//}

	//for (auto it = m_castingSkills.begin(); it != m_castingSkills.end();)
	//{
	//	it->delay -= dt;
	//	if (it->delay <= 0)
	//	{
	//		skillEffect(*it);
	//		it = m_castingSkills.erase(it);
	//	}
	//	else
	//	{
	//		++it;
	//	}
	//}

	//const DayNightTime::Time& nowTime = DayNightData::getInstance().getNowTime();
	//if (nowTime.m_clock != m_lastClock)
	//{
	//	m_lastClock = nowTime.m_clock;

	//	LocalServer_Battle_Clock evt;
	//	evt.m_clock = m_lastClock;
	//	postEvent(evt);
	//}
}

//////////////////////////////////////////////////////////////////////////
void LocalServer_Battle::addExp(const int32 nAddExp)
{
	if (nAddExp <= 0)
		return;

	auto pSelf = PlayerMgr::getInstance().getSelf();
	if (pSelf == nullptr)
		return;

	NS::I64 nNewExp = pSelf->getExp() + nAddExp;

	MsgPlayerInfoGS2C msg;
	msg.set_iggid(pSelf->getIggId());
	msg.set_exp(nNewExp);
	// 升级
	bool bUpLev = levelUp(nNewExp, msg, pSelf);
	send(msg);

	// 客户端完成升级流程后，触发弱联网相关业务
	if (bUpLev)
	{
		//gLocalServer_Achieve.OnPlayerUpLev();
	}
}

bool LocalServer_Battle::CheckAttackTarget(int32 caster_id, int32 target_id, int SkillID) // SkillSys::CheckAttackTarget
{
	//auto pSkillCfg = SkillCfg::getInstance().findSkillType(SkillID);
	//if (!pSkillCfg)
	//	return false;

	//std::shared_ptr<MapObj> pTargetObj = MapObjMgr::getInstance().findObj(target_id);
	//if (!pTargetObj)
	//	return false;

	//std::shared_ptr<Role> pTargetRoleObj = std::dynamic_pointer_cast<Role>(pTargetObj);
	//if (!pTargetRoleObj)
	//	return false;
	//GameObject* pGameObject = HandlerRole::getInstance().findRole(target_id);
	//assert(pGameObject);
	//if (!pGameObject)
	//	return false;
	//RoleComponent* pRoleComp = pGameObject->findComponent<RoleComponent>();
	//assert(pRoleComp);
	//if (!pRoleComp)
	//	return false;
	//CharacterAttributeComponent* pCharacterAttributeComp = pRoleComp->findComponent<CharacterAttributeComponent>();
	//assert(pCharacterAttributeComp);
	//if (!pCharacterAttributeComp)
	//	return false;

	//if (pSkillCfg->IsRestoreHP()) {
	//	//血满,不施加血技能
	//	if (pCharacterAttributeComp->getRealHp() >= pCharacterAttributeComp->getMaxHp())
	//		return false;
	//}

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool LocalServer_Battle::ProcessFight(const std::shared_ptr<Role> pCaster, const std::shared_ptr<Role> pTarget, int& dmgValue, bool& bCritical, NS::I64 serialId, int& durability)
{
	if (pCaster == nullptr || pTarget == nullptr)
		return false;

	////命中前触发被动技能
	float hurtReduceValue{ 0.0f };
	onBeforeDamage(pCaster->getGroupId(), pTarget->getId(), hurtReduceValue, serialId);

	dmgValue = 0;
	bCritical = false;

	SkillEffectParam skillEffectParam;
	skillEffectParam.pCaster = std::make_shared<MapCharacter>(CharacterAttributeSnapshot(pCaster), pCaster->getMaxHp(), pCaster->getMaxMp(), pCaster->getCategory(), pCaster->getMobRare());
	skillEffectParam.pTarget = std::make_shared<MapCharacter>(CharacterAttributeSnapshot(pTarget), pTarget->getMaxHp(), pTarget->getMaxMp(), pTarget->getCategory(), pTarget->getMobRare());
	skillEffectParam.CasterRoleValue.GetSkillRoleValue_ByCharacter(skillEffectParam.pCaster);
	skillEffectParam.TargetRoleValue.GetSkillRoleValue_ByCharacter(skillEffectParam.pTarget);

	SkillFormulaParam FormulaParam;
	SkillEffect_WeaponDmg().GetFormulaParam(FormulaParam, skillEffectParam);

	CEffectCalculation().CalEffect(FormulaParam, dmgValue, bCritical);
	//减伤计算
	dmgReduceEffect(pCaster->getId(), pTarget->getId(), dmgValue, hurtReduceValue, (int)SKILL_EFFECT_DMG_WEAPON, serialId, -1, durability);
	return true;
}

//////////////////////////////////////////////////////////////////////////
void LocalServer_Battle::PetFight(NS::I64 idHP, int32 caster_id, int32 target_id, int32 serialid)
{
	//LocalServer_Battle_BeAttacked evt;
	//evt.m_CasterMapObjID = caster_id;
	//evt.m_TargetMapObjID = target_id;
	//postEvent(evt);

	//// 攻击者。
	//auto pGameObject = HandlerRole::findRole(caster_id);
	//if (!pGameObject)
	//	return;
	//auto pCasterData = pGameObject->findComponent<RoleComponent>()->getData();
	//// 目标。
	//pGameObject = HandlerRole::findRole(target_id);
	//if (!pGameObject)
	//	return;
	//RoleComponent* pTargetRoleComponent = pGameObject->findComponent<RoleComponent>();
	//CharacterAttributeComponent* pTargetCharacterAttributeComp = pTargetRoleComponent->findComponent<CharacterAttributeComponent>();
	//if (!pTargetCharacterAttributeComp) return;
	//auto pTargetData = pTargetRoleComponent->getData();

	//int dmgValue{};
	//int durability{};
	//bool bCritical{};
	//if (!ProcessFight(pCasterData, pTargetData, dmgValue, bCritical, idHP, durability))
	//	return;

	//bool isTargetDead = pTargetCharacterAttributeComp->isDead();
	//// 发送攻击消息。
	//MsgPB::MsgPetNormalAttackResS2C msg;
	//msg.set_serialid(serialid);
	//auto pTargetInfo = msg.add_targetinfo();
	//if (pTargetInfo == nullptr)
	//	return;
	//pTargetInfo->set_mapobjectid(target_id);
	//pTargetInfo->set_damage(dmgValue);
	//pTargetInfo->set_durability(durability);
	//pTargetInfo->set_criticl(bCritical ? 1 : 0);
	//int nCurHP = std::max(pTargetCharacterAttributeComp->getRealHp() - dmgValue, 0);
	//int nMaxHP = pTargetCharacterAttributeComp->getMaxHp();
	//if (nCurHP > nMaxHP)
	//{
	//	nCurHP = nMaxHP;
	//}
	//if (nCurHP < 0)
	//{
	//	nCurHP = 0;
	//}
	//pTargetInfo->set_currhp(nCurHP);

	//msg.set_idhp(idHP);
	//send(msg);

	////受击后触发效果， 普攻->物理伤害
	//onAfterDamage(0, caster_id, target_id, dmgValue, (int)SKILL_EFFECT_DMG_WEAPON, idHP, HurtSource::Normal_Attack);

	//if (pTargetInfo->currhp() == 0)
	//	kill(target_id, caster_id, isTargetDead);

	//// 击退
	//if (pTargetInfo->currhp() > 0)
	//{
	//	//doBeHitFly(caster_id, target_id, idHP);
	//}
}

////////////////////////////////////////////////////////////////////////////////
void LocalServer_Battle::doBeHitFly(int32 caster_id, int32 target_id, NS::I64 serialId, bool isFinalBlow /* = false */)
{
	// 攻击者。
	//GameObject* pGameObject = HandlerRole::findRole(caster_id);
	//if (!pGameObject) { return; }
	//RoleComponent* pCasterComponent = pGameObject->findComponent<RoleComponent>();
	//if (!pCasterComponent) { return; }
	//std::shared_ptr<Role> pCasterData = pCasterComponent->getData();
	//if (!pCasterData) { return; }

	//// 目标。
	//pGameObject = HandlerRole::findRole(target_id);
	//if (!pGameObject) { return; }
	//RoleComponent* pTargetComponent = pGameObject->findComponent<RoleComponent>();
	//if (!pTargetComponent) { return; }
	//std::shared_ptr<Role> pTargetData = pTargetComponent->getData();
	//if (!pTargetData) { return; }

	//// 1. 检查目标状态，是否可被击退
	//if (((pTargetData->getCtrlState() & MsgPB::SKILL_STATUS_CANT_MOVE) == MsgPB::SKILL_STATUS_CANT_MOVE)) { return; }

	//// 击退相关数值
	//float flyDisX{ 0.f };
	//float flyDisY{ 0.f };
	//int32 flyTime{ MonsterCfg::getInstance().getRepelTime() }; // 击退效果持续时间，毫秒
	//int perThound{ 0 };// 系数千分比，默认0

	//// 击退计算
	//do
	//{
	//	// 2. 目标怪物/宠物/NPC， 配置的击退距离，0不可击退
	//	if (pTargetData->getCategory() == MapObj::Category::Monster)
	//	{
	//		std::shared_ptr<Monster> pTargetMonster = dynamic_pointer_cast<Monster>(pTargetData);
	//		if (!pTargetMonster || !pTargetMonster->getMonsterType()) { break; }
	//		flyDisX = static_cast<float>(pTargetMonster->getMonsterType()->n_repel_distance) / 100.0f;
	//	}

	//	// 3. 攻击者玩家，获取武器击退系数，
	//	if (pCasterData->getCategory() == MapObj::Category::Player)
	//	{
	//		std::shared_ptr<Player> pCasterPlayer = dynamic_pointer_cast<Player>(pCasterData);
	//		if (!pCasterPlayer) { break; }

	//		// 只考虑当前的真实武器
	//		const std::shared_ptr<ItemData> pCurrWeapon = pCasterPlayer->getCurrNorWeapon();
	//		if (!pCurrWeapon || !pCurrWeapon->getArmorCfg()) { break; }

	//		// 查找武器特效配置，是否是击退效果
	//		for (auto&& effectId : pCurrWeapon->getArmorCfg()->n_vArmorSpcialEffectIDs)
	//		{
	//			if (effectId <= 0) { continue; }

	//			const ArmorCfg::ArmorSpecialEffect* pSpEffect = ArmorCfg::getInstance().findArmorSpecialEffect(effectId);
	//			if (!pSpEffect) { continue; }

	//			if (pSpEffect->n_type == ArmorCfg::ArmorSpecialEffect::ARMOR_EFFECT_FORCE_BACK)
	//			{
	//				perThound = pSpEffect->n_value;
	//				break;
	//			}
	//		}
	//	}
	//} while (0);

	//// 攻击者朝向
	//int32 orientation = pCasterComponent->getOrientation() == RoleComponent::Orientation::Left ? -1 : 1;

	//// 4. 受击者击退距离乘武器系数，计算最终击退距离
	//flyDisX = orientation * (flyDisX * static_cast<float>(perThound) / 1000.0f);

	//// 最后一击系数
	//int32 finalBlowValue{ MonsterCfg::getInstance().getFinalBlowCoeff() };	// 额外系数，用于最后一击的计算，默认为1000
	//if (isFinalBlow)
	//{
	//	flyDisX = flyDisX * static_cast<float>(finalBlowValue) / 1000.0f;
	//}

	//// 击退距离小于0 忽略
	//if (std::abs(flyDisX) <= 1.e-9f) { return; }

	//// 发送击退消息
	//MsgPB::MsgCharacterBeBeatBackS2C msg;
	//msg.set_serialid(serialId);
	//msg.set_cast_id(caster_id);
	//msg.set_obj_id(target_id);
	//glm::vec2 newPos = MapData::clientWorldPos2ServerWorldPos(pTargetData->getPos() + glm::vec3(flyDisX, flyDisY, 0.f));
	//msg.set_pos_x(static_cast<int32>(newPos.x)); // 服务器下发击退位置X
	//msg.set_pos_y(static_cast<int32>(newPos.y)); // 服务器下发击退位置Y
	//msg.set_time(flyTime);

	//send(msg);
}

void LocalServer_Battle::KillSelf(int32 nMapObjID)
{
	// 目标。
	//auto pTargetGameObject = HandlerRole::findRole(nMapObjID);
	//if (!pTargetGameObject)
	//{
	//	return;
	//}
	//auto pTargetRoleComp = pTargetGameObject->findComponent<RoleComponent>();
	//if (!pTargetRoleComp)
	//{
	//	return;
	//}
	//CharacterAttributeComponent* pTargetCharacterAttributeComp = pTargetRoleComp->findComponent<CharacterAttributeComponent>();
	//if (!pTargetCharacterAttributeComp)
	//{
	//	return;
	//}
	//std::string tag = fmt::format("LocalServer_Battle::KillSelf nMapObjID = {0}", nMapObjID);
	//pTargetCharacterAttributeComp->SetHpAndFlutterType(0, tag, FlutterType::Default);

	//MsgCharacterDeadS2C msg;
	//msg.set_mapobjectid(nMapObjID);
	//send(msg);
}

bool LocalServer_Battle::checkCanHit(const MsgPB::MsgSkillCastSkillC2GS& msg, const SkillType* pSkillType)
{
	//if (!pSkillType) { return false; }
	//// only self need to check
	//auto&& pMapData = MapDataMgr::getInstance().getCurrMapData();
	//if (!pMapData) { return false; }

	//std::shared_ptr<Player> pPlayerData = PlayerMgr::getInstance().getSelf();
	//if (!pPlayerData) { return false; }

	//// 非直击技能，不做检查
	//if (pSkillType->m_skillType != SkillType::Type::InstantSkill) { return true; }
	//// 直击技能目标集未空，空放技能，不提示
	//if (msg.target_mapobjectids_size() <= 0 || (msg.target_mapobjectids_size() == 1 && msg.target_mapobjectids(0) == 0)) { return true; }

	//// 默认取玩家位置 + 半格高度
	//glm::ivec2 playerCoord = pPlayerData->getCoord() + glm::ivec2{ 0, -static_cast<int>(pPlayerData->getHeight() / 2) };
	//if (msg.has_caster_pos_x() && msg.has_caster_pos_y())
	//{
	//	glm::vec3 caster_position = MapData::serverWorldPos2ClientWorldPos((int)msg.caster_pos_x(), (int)msg.caster_pos_y());
	//	playerCoord = pMapData->worldPosToCoord(caster_position);
	//}
	//for (auto&& targetMapObjId : msg.target_mapobjectids())
	//{
	//	GameObject* pGameObject = HandlerRole::getInstance().findRole(targetMapObjId);
	//	if (!pGameObject) { continue; }
	//	RoleComponent* pRoleComponent = pGameObject->findComponent<RoleComponent>();
	//	if (!pRoleComponent) { continue; }
	//	std::shared_ptr<Role> pRoleData = pRoleComponent->getData();
	//	if (!pRoleData) { continue; }

	//	// 增加地块阻挡判断
	//	auto&& targetWidth = static_cast<int>(round(pRoleData->getWidth()));
	//	auto&& targetHeight = static_cast<int>(round(pRoleData->getHeight()));
	//	for (int i{}; i < targetWidth; ++i)
	//	{
	//		for (int j{}; j < targetHeight; ++j)
	//		{
	//			if (pMapData->isConnected(pRoleData->getCoord() + glm::ivec2{ i, -j }, playerCoord))
	//			{
	//				return true;
	//			}
	//		}
	//	}
	//}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////
// 被动技能结算

// 被动 - 反伤 结算 伤害值百分比 or 固定值
void LocalServer_Battle::countAttack(int32 nSkillId, int32 nCaster_id, int32 nTarget_id, int32 nDamageValue)
{
	const SkillType* pSkillType = SkillCfg::getInstance().findSkillType(nSkillId);
	if (!pSkillType) { return; }

	std::shared_ptr<MapObj> pTargetObject = MapObjMgr::getInstance().findObj(nTarget_id);
	if (!pTargetObject) { return; }
	std::shared_ptr<Role> pTargetData = std::dynamic_pointer_cast<Role>(pTargetObject);
	if (!pTargetData) { return; }

	// 反击伤害计算
	int nCountAttackDamage{ 0 };
	if (pSkillType->m_triggerParam2 == 1)
	{
		nCountAttackDamage = pSkillType->m_triggerParam3;
	}
	else
	{
		nCountAttackDamage = static_cast<int>(static_cast<float>(nDamageValue * pSkillType->m_triggerParam3) / 100.0f);
	}
	if (nCountAttackDamage <= 0) { return; }
	// 被动技能伤害效果消息
	sendPassiveEffectDmg(pSkillType->m_id, nCaster_id, nTarget_id, pTargetData->getHp(), pTargetData->getMaxHp(), nCountAttackDamage);
}

// 被动 - 吸血 结算 伤害值百分比
void LocalServer_Battle::suckingBlood(int32 nSkillId, int32 nCaster_id, int32 nTarget_id, int32 nDamageValue)
{
	const SkillType* pSkillType = SkillCfg::getInstance().findSkillType(nSkillId);
	if (!pSkillType) { return; }

	std::shared_ptr<MapObj> pCasterObject = MapObjMgr::getInstance().findObj(nCaster_id);
	if (!pCasterObject) { return; }
	std::shared_ptr<Role> pCasterData = std::dynamic_pointer_cast<Role>(pCasterObject);
	if (!pCasterData) { return; }

	// 伤害百分比吸血计算
	int nHpVal = static_cast<int>(static_cast<float>(nDamageValue * pSkillType->m_triggerParam2) / 100.0f);
	if (nHpVal <= 0) { return; }
	// 被动技能伤害效果消息
	sendPassiveEffectDmg(pSkillType->m_id, nCaster_id, nCaster_id, pCasterData->getHp(), pCasterData->getMaxHp(), -nHpVal);
}

// 发送被动技能伤害效果消息
void LocalServer_Battle::sendPassiveEffectDmg(int32 nSkillId, int32 nCasterId, int32 nTargetId, int nCurHp, int nMaxHp, int nDamageValue)
{
	// 反击伤害效果
	MsgPassiveSkillEffectDmgMS2C msg;
	msg.set_target_skill(nSkillId);
	msg.set_caster_mapobjectid(nCasterId);
	msg.set_target_mapobjectid(nTargetId);

	if (nDamageValue > 0)
	{
		// 伤害，需要额外计算护盾效果的影响
		int durability{ 0 };
		absorbDamage(nTargetId, nDamageValue, (int)SKILL_EFFECT_DMG_NULL, durability);
		msg.set_durability(durability);
	}
	msg.set_dmg_val(nDamageValue);
	msg.set_criticl(false);
	nCurHp -= nDamageValue;
	nCurHp = std::min(std::max(nCurHp, 0), nMaxHp);
	msg.set_currhp(nCurHp);
	send(msg);
}

NS_END

