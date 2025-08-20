#pragma once
#include <memory>

#include <common/SkillBuffInfo.h>
#include <common/SkillEffect.h>
#include "BattleCalc.h"
#include "LocalServer_Battle.h"
#include "LocalServer_Buff.h"
#include <RandomUtil.h>
//#include <player/PlayerComponent.h>
#include <config/SkillCfg.h>
#include "config/BuffCfg.h"

#include <msg/MsgGameDef.pb.h>
//#include "common/RoleComponent.h"
//#include "common/CharacterAttributeComponent.h"
//#include "skill/SkillComponent.h"
#include "common/SkillBuffContainer.h"

namespace skill
{
	// 其它buff被添加时回调
	struct ParamBuffEvtTrg_OtherBuffPreAddCheck
	{
		ParamBuffEvtTrg_OtherBuffPreAddCheck(const SkillBuffInfo& refBuffInfo, const S_BuffTemplate_effect& refEffect)
			: in_RefBuffInfo(refBuffInfo)
			, in_RefEffect(refEffect)
		{
		}

		const SkillBuffInfo& in_RefBuffInfo;
		const S_BuffTemplate_effect& in_RefEffect;
		int32 in_OtherBuffID;

		bool out_bCanAdd = true;
	};



	// Buff功能執行時的策略 [Design Patterns: Strategy]
	template<typename TSkillEventHandler>
	class TSkillBuffStrategy
	{
	public:
		virtual void BuffInit  (SkillBuffInfo& pBuffInfo, NS::uint EffectIndex, TSkillEventHandler* pEventHandler) {}
		virtual void BuffUpdate(SkillBuffInfo& pBuffInfo, NS::uint EffectIndex, TSkillEventHandler* pEventHandler) {}
		virtual void BuffEnd   (SkillBuffInfo& pBuffInfo, NS::uint EffectIndex, TSkillEventHandler* pEventHandler) {}

		virtual void BuffEventTrigger_OtherBuffPreAddCheck(ParamBuffEvtTrg_OtherBuffPreAddCheck& param) {}

		virtual int32 GetAutoHeap(int32 buffDataID)
		{
			auto buffCfg = BuffCfg::getInstance().findBuffType(buffDataID);
			if (buffCfg)
			{
				return buffCfg->n_auto_heap;
			}
			return 0;
		}

		void AutoHeap(SkillBuffInfo& pBuffInfo, TSkillEventHandler* pEventHandler)
		{
			auto autoHeap = this->GetAutoHeap(pBuffInfo.BuffDataID);
			if (autoHeap <= 0)
				return;

			if (pBuffInfo.HeapCounter >= autoHeap)
				return;

			pBuffInfo.HeapCounter++;

			// 通知更新

			MsgBuffInfo_S2C msg;
			msg.set_buff_event(1);

			auto p = msg.add_buff_infos();
			p->set_index(pBuffInfo.Index);
			p->set_buff_dataid(pBuffInfo.BuffDataID);
			p->set_alive_timer(pBuffInfo.AliveTimer);
			p->set_heap_counter(pBuffInfo.HeapCounter);
			p->set_owner_mapobjectid(pBuffInfo.OwnerMapObjectID);
			p->set_caster_mapobjectid(pBuffInfo.CasterMapObjectID);
			p->set_effect_rate(pBuffInfo.nEffectRate);

			NS::LocalServer_Buff::getInstance().send(msg);

			//下线同步玩家buff状态
			NS::LocalServer_Buff::getInstance().syncBuffCache(&pBuffInfo, 1);

			//if (pEventHandler) pEventHandler->UpdateBuff(pBuffInfo.OwnerMapObjectID, &pBuffInfo, 1, 0);
		}

	};

	// 角色屬性策略
	template<typename TSkillEventHandler>
	class TSkillBuffStrategy_RoleAttr final : public TSkillBuffStrategy<TSkillEventHandler>
	{};

	// 角色狀態策略
	template<typename TSkillEventHandler>
	class TSkillBuffStrategy_RoleState final : public TSkillBuffStrategy<TSkillEventHandler>
	{
	//public:
	//	virtual void BuffUpdate(SkillBuffInfo& pBuffInfo, NS::uint EffectIndex, TSkillEventHandler* pEventHandler) override
	//	{
	//		if (pEventHandler == nullptr)
	//			return;

	//		// 取得效果的內容
	//		if (EffectIndex < 0 || EffectIndex >= pBuffInfo.BuffEffects.size())
	//			return;

	//		if (pBuffInfo.BuffEffects[EffectIndex].values.size() == 0)
	//			return;
	//		if (pBuffInfo.BuffEffects[EffectIndex].values[0].attrref != ATTR_REF_STATIC)
	//			return;

	//		int nState = pBuffInfo.BuffEffects[EffectIndex].values[0].value;
	//		if ((nState & SKILL_STATUS_FOLLOWMOVE) != 0)
	//		{
	//			// 跟随改为客户端处理
	//			// 		if (pBuffInfo.pBuffCaster == nullptr) {
	//			// 			pBuffInfo.AliveTimer = 0;
	//			// 			return;
	//			// 		}
	//			// 		pBuffInfo.pBuffOwner->SetPos(pBuffInfo.pBuffCaster->GetPos());
	//		}
	//	};
	};

	// 持续伤害。
	template< typename TSkillEventHandler, typename TSkillEffectParam, typename TMapObject>
	class TSkillBuffStrategy_Dot final : public TSkillBuffStrategy<TSkillEventHandler>
	{
	public:
		virtual void BuffInit(SkillBuffInfo& pBuffInfo, NS::uint EffectIndex, TSkillEventHandler* /*pEventHandler*/) override
		{
			//if (!pEventHandler) return;

			// 取得效果的內容
			if (/*EffectIndex < 0 || */EffectIndex >= pBuffInfo.BuffEffects.size())
				return;

			//shared_ptr<TMap> pMap = pEventHandler->GetMap();
			//if (pMap == nullptr)
			//	return;

			// 取得效果及驗証
			S_BuffTemplate_effect BuffEffect = pBuffInfo.BuffEffects[EffectIndex];
			BUFF_EFFECT_TYPE emDotType = (BUFF_EFFECT_TYPE)BuffEffect.type;
			//if (emDotType < BUFF_EFFECT_REAL_DOT || emDotType > BUFF_EFFECT_DARK_DOT)
			//	return;

			//// 取得對應的IEffect效果(直接取用法術作用效果)
			//shared_ptr<TSkillEffect> pDmgEffect = this->GetSkillEffect(emDotType, pEventHandler);
			//if (pDmgEffect == nullptr)
			//	return;

			// 施法者。
			std::shared_ptr<MapObj> pCasterObj = MapObjMgr::getInstance().findObj(pBuffInfo.CasterMapObjectID);
			if (!pCasterObj) { return; }
			std::shared_ptr<Role> pCasterData = std::dynamic_pointer_cast<Role>(pCasterObj);
			if (!pCasterData)
			{
				return;
			}

			//auto pGameObject = HandlerRole::findRole(pBuffInfo.CasterMapObjectID);
			//if (!pGameObject) return;
			//auto pCasterData = pGameObject->getData();
			// 目标。

			std::shared_ptr<MapObj> pMapTargetObj = MapObjMgr::getInstance().findObj(pBuffInfo.OwnerMapObjectID);
			if (!pMapTargetObj) { return; }
			std::shared_ptr<Role> pTargetData = std::dynamic_pointer_cast<Role>(pMapTargetObj);
			if (!pTargetData)
			{
				return;
			}

			//pGameObject = HandlerRole::findRole(pBuffInfo.OwnerMapObjectID);
			//if (!pGameObject) return;
			//auto pTargetData = pGameObject->getData();

			// 執行
			TSkillEffectParam param;
			param.pCaster = std::make_shared<MapCharacter>(CharacterAttributeSnapshot(pCasterData), pCasterData->getMaxHp(), pCasterData->getMaxMp(), pCasterData->getCategory(), pCasterData->getMobRare());
			param.pTarget = std::make_shared<MapCharacter>(CharacterAttributeSnapshot(pTargetData), pTargetData->getMaxHp(), pTargetData->getMaxMp(), pTargetData->getCategory(), pTargetData->getMobRare());
			param.CasterRoleValue.GetSkillRoleValue_ByCharacter(param.pCaster);
			param.TargetRoleValue.GetSkillRoleValue_ByCharacter(param.pTarget);
			param.theEffect.values = BuffEffect.values;
			param.EffectRate = 1000 * std::max(pBuffInfo.HeapCounter, 1);
			param.SerialID = 1;
			param.nEffectCount = pBuffInfo.nEffectCount;
			param.nDmg = pBuffInfo.nDmg;

			int dmgValue;
			bool bCritical = false;
			switch (emDotType)
			{
			case BUFF_EFFECT_REAL_DOT:
			{
				skill::ISkillEffect_RealDmg<SkillRoleValue, SkillEffectParam, MapCharacter, MapCharacter, MapCharacter, AttrVector> realDmg;
				dmgValue = realDmg.CalEffect(param);
			}
			break;

			case BUFF_EFFECT_WEAPON_DOT:
			{
				SkillFormulaParam formulaParam;
				SkillEffect_WeaponDmg().GetFormulaParam(formulaParam, param);
				CEffectCalculation().CalEffect(formulaParam, dmgValue, bCritical);
			}
			break;

			default:
				SkillFormulaParam formulaParam;
				SkillEffect_BuffDmg(emDotType).GetFormulaParam(formulaParam, param);
				CEffectCalculation().CalEffect(formulaParam, dmgValue, bCritical);
				break;
			}

			pBuffInfo.Param = dmgValue / pBuffInfo.nEffectCount;
		}

		virtual void BuffUpdate(SkillBuffInfo& pBuffInfo, NS::uint EffectIndex, TSkillEventHandler* /*pEventHandler*/) override
		{
			static NS::UI64 serialId{};

			MsgSkillEffectDotDmgMS2C msg;
			msg.set_serialid(serialId);
			msg.set_caster_mapobjectid(pBuffInfo.CasterMapObjectID);
			msg.set_target_mapobjectid(pBuffInfo.OwnerMapObjectID);
			msg.set_skill_id(pBuffInfo.BuffDataID);
			int dmgValue = pBuffInfo.Param;
			int durability{ 0 };
			if (dmgValue > 0)
			{
				auto pbuff = NS::LocalServer_Buff::getInstance().FindBuffContainer(pBuffInfo.OwnerMapObjectID);
				if (pbuff)
				{
					int effect_type = pBuffInfo.BuffEffects[0].type;
					pbuff->ForEachBuff([&dmgValue, effect_type, &durability](SkillBuffInfo& buff)
						{
							if (dmgValue > 0 && (!buff.IsEmpty()))
							{

								for (const auto& effect : buff.BuffEffects)
								{
									auto offsetType = SkillEffect_BuffDurability::GetTypeByBuffEffectType(effect_type);
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
							}
						});
				}
			}
			msg.set_dmg_val(dmgValue);
			msg.set_durability(durability);
			msg.set_criticl(false);

			int nCurHP = -1;
			bool isTargetDead = false;
			if (auto pRoleObject = HandlerRole::findRole(pBuffInfo.OwnerMapObjectID))
			{
				Role* role = dynamic_cast<Role*>(pRoleObject);
				if (role)
				{
					nCurHP = role->getHp() - dmgValue;
					int nMaxHp = role->getMaxHp();
					nCurHP = (nCurHP > nMaxHp) ? nMaxHp : nCurHP;
					nCurHP = (nCurHP < 0) ? 0 : nCurHP;
					role->setHp(nCurHP);
				}
			}
			msg.set_currhp(nCurHP);
			NS::LocalServer_Buff::getInstance().send(msg);

			if (nCurHP == 0)
			{
				NS::LocalServer_Battle::getInstance().kill(pBuffInfo.OwnerMapObjectID, pBuffInfo.CasterMapObjectID, isTargetDead);
			}

			++serialId;
		}

		virtual void BuffEnd(SkillBuffInfo& pBuffInfo, NS::uint EffectIndex, TSkillEventHandler* pEventHandler) override
		{
			pBuffInfo.AliveTimer = 0;
		}

	//protected:
	//	shared_ptr<TSkillEffect> GetSkillEffect(BUFF_EFFECT_TYPE emDotType, TSkillEventHandler* pEventHandler)
	//	{
	//		if (!pEventHandler) return nullptr;

	//		switch (emDotType)
	//		{
	//		case BUFF_EFFECT_REAL_DOT:		return pEventHandler->GetSkillEffect(SKILL_EFFECT_DMG_REAL);
	//		case BUFF_EFFECT_FIRE_DOT:		return pEventHandler->GetSkillEffect(SKILL_EFFECT_DMG_FIRE);
	//		case BUFF_EFFECT_FORST_DOT:		return pEventHandler->GetSkillEffect(SKILL_EFFECT_DMG_FORST);
	//		case BUFF_EFFECT_THUNDER_DOT:	return pEventHandler->GetSkillEffect(SKILL_EFFECT_DMG_THUNDER);
	//		case BUFF_EFFECT_POSION_DOT:	return pEventHandler->GetSkillEffect(SKILL_EFFECT_DMG_POSION);
	//		case BUFF_EFFECT_LIGHT_DOT:		return pEventHandler->GetSkillEffect(SKILL_EFFECT_DMG_LIGHT);
	//		case BUFF_EFFECT_DARK_DOT:		return pEventHandler->GetSkillEffect(SKILL_EFFECT_DMG_DARK);
	//		case BUFF_EFFECT_OXY_DOT:		return pEventHandler->GetSkillEffect(SKILL_EFFECT_DMG_OXY);
	//		case BUFF_EFFECT_WEAPON_DOT:	return pEventHandler->GetSkillEffect(SKILL_EFFECT_DMG_WEAPON);
	//		}
	//		return nullptr;
	//	};
	};


	// 持续扣氧。
	template< typename TSkillEventHandler, typename TSkillEffectParam, typename TMapObject>
	class TSkillBuffStrategy_OxyDot final : public TSkillBuffStrategy<TSkillEventHandler>
	{
	public:
		virtual void BuffUpdate(SkillBuffInfo& pBuffInfo, NS::uint EffectIndex, TSkillEventHandler* /*pEventHandler*/) override
		{
			//static NS::UI64 serialId{ 10102030000u };

			//if (!pEventHandler) return;

			// 取得效果的內容
			if (/*EffectIndex < 0 ||*/ EffectIndex >= pBuffInfo.BuffEffects.size())
				return;

			//shared_ptr<TMap> pMap = pEventHandler->GetMap();
			//if (pMap == nullptr)
			//	return;

			// 取得效果及驗証
			S_BuffTemplate_effect BuffEffect = pBuffInfo.BuffEffects[EffectIndex];
//			BUFF_EFFECT_TYPE emDotType = (BUFF_EFFECT_TYPE)BuffEffect.type;
			//if (emDotType < BUFF_EFFECT_REAL_DOT || emDotType > BUFF_EFFECT_DARK_DOT)
			//	return;

			//// 取得對應的IEffect效果(直接取用法術作用效果)
			//shared_ptr<TSkillEffect> pDmgEffect = this->GetSkillEffect(emDotType, pEventHandler);
			//if (pDmgEffect == nullptr)
			//	return;

			// 目标。
			//auto pGameObject = HandlerRole::findRole(pBuffInfo.OwnerMapObjectID);
			//if (!pGameObject) return;

			//PlayerComponent* pPlayerComponent = pGameObject->findComponent<PlayerComponent>();
			//if (!pPlayerComponent) { return; }
			//auto pTargetData = pPlayerComponent->getPlayerData();

			////// 執行
			//int dmgVal = !BuffEffect.values.empty() ? BuffEffect.values[0].value : 3;

			// 发送扣氧气通知。
			//MsgPlayerInfoGS2C msg;
			//msg.set_iggid(pTargetData->getIggId());
			//msg.set_breath(pTargetData->getBreath() - dmgVal);
			//NS::LocalServer_Buff::getInstance().send(msg);
		};

		virtual void BuffEnd(SkillBuffInfo& pBuffInfo, NS::uint EffectIndex, TSkillEventHandler* pEventHandler) override
		{
			pBuffInfo.AliveTimer = 0;
		};
	};


	// HP/MP回復策略
	template<typename TSkillEventHandler, /*typename TSkillEffect,*/ typename TSkillEffectParam, /*typename TSkillEffect_Restore,*/ typename TMapObject>
	class TSkillBuffStrategy_Restore final : public TSkillBuffStrategy<TSkillEventHandler>
	{
		virtual void BuffUpdate(SkillBuffInfo& pBuffInfo, NS::uint EffectIndex, TSkillEventHandler* /*pEventHandler*/) override
		{
			//if (!pEventHandler) return;

			// 取得效果的內容
			if (/*EffectIndex < 0||*/  EffectIndex >= pBuffInfo.BuffEffects.size())
				return;

			//auto pMap = pEventHandler->GetMap();
			//if (pMap == nullptr)
			//	return;

			// 取得效果及驗証
			S_BuffTemplate_effect BuffEffect = pBuffInfo.BuffEffects[EffectIndex];
			BUFF_EFFECT_TYPE emDotType = (BUFF_EFFECT_TYPE)BuffEffect.type;
			if (emDotType != BUFF_EFFECT_HP_RESTORE && emDotType != BUFF_EFFECT_MP_RESTORE)
				return;

			// 取得對應的IEffect效果(直接取用法術作用效果)
			//shared_ptr<TSkillEffect> pEffect;
			switch (emDotType)
			{
			//case BUFF_EFFECT_HP_RESTORE: pEffect = pEventHandler->GetSkillEffect(SKILL_EFFECT_HP_RESTORE); break;
			//case BUFF_EFFECT_MP_RESTORE: pEffect = pEventHandler->GetSkillEffect(SKILL_EFFECT_MP_RESTORE); break;

			case BUFF_EFFECT_HP_RESTORE:
			case BUFF_EFFECT_MP_RESTORE:
				break;

			default:
				return;
			}

			//shared_ptr<TSkillEffect_Restore> pRestoreEffect = dynamic_pointer_cast<TSkillEffect_Restore>(pEffect);
			//if (pRestoreEffect == nullptr)
			//	return;

			//shared_ptr<TMapObject> pOwner;
			//pMap->GetMapObject(pBuffInfo.OwnerMapObjectID, pOwner);
			//if (pOwner == nullptr)
			//	return;

			// 施法者。
			auto pCasterObj = HandlerRole::findRole(pBuffInfo.CasterMapObjectID);
			//if (!pCasterObj) return;
			//auto pCasterData = pCasterObj->findComponent<RoleComponent>()->getData();
			//auto pGameObject = HandlerRole::findRole(pBuffInfo.OwnerMapObjectID);
			//if (!pGameObject) return;
			//RoleComponent* pRoleCompTarget = pGameObject->findComponent<RoleComponent>();
			//assert(pRoleCompTarget);
			//if (!pRoleCompTarget) return;
			//auto pTargetData = pGameObject->findComponent<RoleComponent>()->getData();
			//CharacterAttributeComponent* pCharacterAttributeCompTarget = pRoleCompTarget->findComponent<CharacterAttributeComponent>();
			//assert(pCharacterAttributeCompTarget);
			//if (!pCharacterAttributeCompTarget) return;

			//if (pCharacterAttributeCompTarget->isDead() || pCharacterAttributeCompTarget->isDying())
			//{
			//	// 目标都已经死了，就不需要再回血了
			//	return;
			//}

			// 執行
			TSkillEffectParam param;
			//param.pCaster = std::make_shared<MapCharacter>(CharacterAttributeSnapshot(pCasterData), pCasterData->getMaxHp(), pCasterData->getMaxMp(), pCasterData->getCategory(), pCasterData->getMobRare());
			//param.pTarget = std::make_shared<MapCharacter>(CharacterAttributeSnapshot(pTargetData), pTargetData->getMaxHp(), pTargetData->getMaxMp(), pTargetData->getCategory(), pTargetData->getMobRare());
			param.CasterRoleValue.GetSkillRoleValue_ByCharacter(param.pCaster);
			param.TargetRoleValue.GetSkillRoleValue_ByCharacter(param.pTarget);
			param.theEffect.values = BuffEffect.values;

			int restoreValue;
			SkillEffect_Restore().CalEffect(param, restoreValue);

			//// 最终回复值会在90%到100%之间随机
			//restoreValue = static_cast<int>(static_cast<float>(restoreValue) * NS::RandomUtil::getRandomFloat(0.9f, 1.0f));
			//// 執行
			//pRestoreEffect->DoEffect_ForDot(Param, pEventHandler);

			MsgSkillEffectDotRestoreMS2C msg;
			msg.set_caster_mapobjectid(pBuffInfo.CasterMapObjectID);
			msg.set_target_mapobjectid(pBuffInfo.OwnerMapObjectID);
			//msg.set_skill_id(0);
			if (emDotType == BUFF_EFFECT_HP_RESTORE)
			{
				//int nCurHP = pCharacterAttributeCompTarget->getRealHp();
				//int nMaxHp = pCharacterAttributeCompTarget->getMaxHp();
				//// CLV - 8876【药剂】满蓝，使用回蓝药剂，不应该显示加多少蓝
				//// 如果已经满血/满蓝了，在Buff中添加处理，不需要再下发加血/加蓝消息，客户端修改完毕之后，请再转服务端处理
				//if (nCurHP >= nMaxHp)
				//{
				//	return;
				//}

				//nCurHP += restoreValue;
				//if (nCurHP > nMaxHp)
				//{
				//	// Hp将满修正变更值
				//	restoreValue -= (nCurHP - nMaxHp);
				//	nCurHP = nMaxHp;
				//}
				//else if(nCurHP < 0)
				//{
				//	nCurHP = 0;
				//}
				//msg.set_hp_val(restoreValue);
				//msg.set_currhp(nCurHP);
				//NS::LocalServer_Buff::getInstance().send(msg);
			}
			else if (emDotType == BUFF_EFFECT_MP_RESTORE)
			{
				// CLV - 8876【药剂】满蓝，使用回蓝药剂，不应该显示加多少蓝
				//int nCurMp = pCharacterAttributeCompTarget->getMp();
				//int nMaxMp = pCharacterAttributeCompTarget->getMaxMp();
				//// 如果已经满血/满蓝了，在Buff中添加处理，不需要再下发加血/加蓝消息，客户端修改完毕之后，请再转服务端处理
				//if (nCurMp >= nMaxMp)
				//{
				//	return;
				//}

				//// Mp将满修正变更值
				//restoreValue = std::min(restoreValue, nMaxMp - nCurMp);
				//msg.set_mp_val(restoreValue);
				//// 血值同步强制要求要currhp需要设值
				//int nCurHP = pCharacterAttributeCompTarget->getRealHp();
				//msg.set_currhp(nCurHP);
				//NS::LocalServer_Buff::getInstance().send(msg);
			}
		};
	};

	// 施放技能
	template<typename TSkillEventHandler>
	class TSkillBuffStrategy_CastSkill final : public TSkillBuffStrategy<TSkillEventHandler>
	{
		virtual void BuffUpdate(SkillBuffInfo& pBuffInfo, NS::uint EffectIndex, TSkillEventHandler* /*pEventHandler*/) override
		{
			//if (!pEventHandler) return;

			// 取得效果的內容
			if (/*EffectIndex < 0 ||*/ EffectIndex >= pBuffInfo.BuffEffects.size())
				return;

			// 技能
			if (pBuffInfo.BuffEffects.size() == 0)
				return;
			if (pBuffInfo.BuffEffects[EffectIndex].type != ROLE_EFFECT_CAST_SKILL)
				return;
			if (pBuffInfo.BuffEffects[EffectIndex].values.size() == 0)
				return;
			if (pBuffInfo.BuffEffects[EffectIndex].values[0].attrref != ATTR_REF_STATIC)
				return;
			int SkillID = pBuffInfo.BuffEffects[EffectIndex].values[0].value;

			if (auto pSkillType = SkillCfg::getInstance().findSkillType(SkillID))
			{
				if (pSkillType->m_targetType == SkillType::TargetType::Self)	// 对自己施法。
				{
					NS::LocalServer_Battle::getInstance().cast_skill(SkillID, pBuffInfo.CasterMapObjectID, pBuffInfo.CasterMapObjectID);
				}
			}

			//// 取得技能系統
			//auto pMap = pEventHandler->GetMap();
			//if (pMap == nullptr)
			//	return;

			//auto pSkillSys = pEventHandler->GetSkillSys();
			//if (pSkillSys == nullptr)
			//	return;

			//shared_ptr<TMapObject> pObject;
			//pMap->GetMapObject(pBuffInfo.CasterMapObjectID, pObject);
			//if (pObject == nullptr)
			//	return;

			///*
			//對自已施展法術
			//EasyCastSkill参数：
			//1. shared_ptr<TMapObject> pCaster  施法者
			//2. const shared_ptr<TMapObject>& pTarget 目标
			//3. int SkillID 技能id
			//4. const shared_ptr<TMapObject>& pDir); 朝向
			//*/
			//pSkillSys->EasyCastSkill(pObject, pObject, SkillID, pObject);
		};
	};

	// 輸送帶
	template<typename TSkillEventHandler>
	class TSkillBuffStrategy_Transport final : public TSkillBuffStrategy<TSkillEventHandler>
	{};

	// 探勘狀態
	template<typename TSkillEventHandler>
	class TSkillBuffStrategy_Prospect final : public TSkillBuffStrategy<TSkillEventHandler>
	{};

	// 探測狀態_礦石
	template<typename TSkillEventHandler>
	class TSkillBuffStrategy_DetectingOre final : public TSkillBuffStrategy<TSkillEventHandler>
	{};

	// 探測狀態_地宮
	template<typename TSkillEventHandler>
	class TSkillBuffStrategy_DetectingDungeon final : public TSkillBuffStrategy<TSkillEventHandler>
	{};

	// 印记，X层后触发技能，产生指定效果
	template<typename TSkillEventHandler>
	class TSkillBuffStrategy_Marking final : public TSkillBuffStrategy<TSkillEventHandler>
	{
		virtual void BuffInit(SkillBuffInfo& pBuffInfo, NS::uint EffectIndex, TSkillEventHandler* pEventHandler) override
		{
			if (EffectIndex >= pBuffInfo.BuffEffects.size())
			{
				return;
			}

			const BuffType* pBuffType = BuffCfg::getInstance().findBuffType(pBuffInfo.BuffDataID);
			if (!pBuffType)
			{
				return;
			}

			if (pBuffInfo.HeapCounter < static_cast<int>(pBuffType->n_max_heap))
			{
				return;
			}

			// 技能
			const S_BuffTemplate_effect_RT& buffEffect = pBuffInfo.BuffEffects[EffectIndex];
			if (buffEffect.type != BUFF_EFFECT_MARKING)
				return;
			if (buffEffect.values.size() == 0)
				return;
			if (buffEffect.values[0].attrref != ATTR_REF_STATIC)
				return;
			int SkillID = buffEffect.values[0].value;

			if (const SkillType* pSkillType = SkillCfg::getInstance().findSkillType(SkillID))
			{
				if (pSkillType->m_targetType == SkillType::TargetType::Self)	// 对自己施法。
				{
					NS::LocalServer_Battle::getInstance().cast_skill(SkillID, pBuffInfo.OwnerMapObjectID, pBuffInfo.OwnerMapObjectID);
				}
			}

			pBuffInfo.AliveTimer = 0; // Buff直接结束
		}
	};

	//BUFF提供被动技能
	template<typename TSkillEventHandler>
	class TSkillBuffStrategy_AddPassiveSkill final : public TSkillBuffStrategy<TSkillEventHandler>
	{
		virtual void BuffInit(SkillBuffInfo& pBuffInfo, NS::uint EffectIndex, TSkillEventHandler* pEventHandler) override
		{
			if (EffectIndex >= pBuffInfo.BuffEffects.size()) return;

			// 技能
			if (pBuffInfo.BuffEffects.size() == 0)
				return;
			if (pBuffInfo.BuffEffects[EffectIndex].type != BUFF_EFFECT_ADD_PASSIVE_SKILL
				&& pBuffInfo.BuffEffects[EffectIndex].type != BUFF_EFFECT_CAST_SKILL_WHEN_OBJ_IN_VIEW)
				return;
			if (pBuffInfo.BuffEffects[EffectIndex].values.size() == 0)
				return;
			if (pBuffInfo.BuffEffects[EffectIndex].values[0].attrref != ATTR_REF_STATIC)
				return;
			int p_skillId = pBuffInfo.BuffEffects[EffectIndex].values[0].value;

			auto pGameObject = HandlerRole::findRole(pBuffInfo.OwnerMapObjectID);
			if (!pGameObject) return;
			//RoleComponent* pRoleCompTarget = pGameObject->findComponent<RoleComponent>();
			//assert(pRoleCompTarget);
			//if (!pRoleCompTarget) return;
			//auto pTargetData = pGameObject->findComponent<RoleComponent>()->getData();
			//SkillComponent* pSkillComponent = pRoleCompTarget->findComponent<SkillComponent>();
			//assert(pSkillComponent);
			//if (!pSkillComponent) return;
			//pSkillComponent->learnSkill(p_skillId);
		}

		virtual void BuffEnd(SkillBuffInfo& pBuffInfo, NS::uint EffectIndex, TSkillEventHandler* pEventHandler) override
		{
			if (EffectIndex >= pBuffInfo.BuffEffects.size()) return;

			// 技能
			if (pBuffInfo.BuffEffects.size() == 0)
				return;
			if (pBuffInfo.BuffEffects[EffectIndex].type != BUFF_EFFECT_ADD_PASSIVE_SKILL
				&& pBuffInfo.BuffEffects[EffectIndex].type != BUFF_EFFECT_CAST_SKILL_WHEN_OBJ_IN_VIEW)
				return;
			if (pBuffInfo.BuffEffects[EffectIndex].values.size() == 0)
				return;
			if (pBuffInfo.BuffEffects[EffectIndex].values[0].attrref != ATTR_REF_STATIC)
				return;
			int p_skillId = pBuffInfo.BuffEffects[EffectIndex].values[0].value;

			auto pGameObject = HandlerRole::findRole(pBuffInfo.OwnerMapObjectID);
			if (!pGameObject) return;
			//RoleComponent* pRoleCompTarget = pGameObject->findComponent<RoleComponent>();
			//assert(pRoleCompTarget);
			//if (!pRoleCompTarget) return;
			//auto pTargetData = pGameObject->findComponent<RoleComponent>()->getData();
			//SkillComponent* pSkillComponent = pRoleCompTarget->findComponent<SkillComponent>();
			//assert(pSkillComponent);
			//if (!pSkillComponent) return;
			//pSkillComponent->removePassiveSkill(p_skillId);
		}
	};

	//BUFF免疫策略
	template<typename TSkillEventHandler>
	class TSkillBuffStrategy_BuffImmune final : public TSkillBuffStrategy<TSkillEventHandler>
	{
		virtual void BuffInit(SkillBuffInfo& pBuffInfo, NS::uint EffectIndex, TSkillEventHandler* pEventHandler) override
		{
			const S_BuffTemplate_effect_RT& effect = pBuffInfo.BuffEffects[EffectIndex];
			if (effect.values.size() < 1) { return; }

			int nBuffId = effect.values[0].value;
			SkillBuffContainer* pBuffContainer = NS::LocalServer_Buff::getInstance().FindBuffContainer(pBuffInfo.OwnerMapObjectID);
			if (!pBuffContainer) { return; }

			std::list<int> lstDelIndex;
			auto pLambda = [&lstDelIndex, nBuffId](const SkillBuffInfo& buff)
			{
				if (nBuffId != buff.BuffDataID)
				{
					return;
				}
				lstDelIndex.push_back(buff.Index);
			};
			pBuffContainer->ForEachNoEmptyBuff(pLambda);

			for (int index : lstDelIndex)
			{
				SkillBuffInfo* pWaitDelBuffInfo = pBuffContainer->GetBuff(index);
				if (!pWaitDelBuffInfo) { continue; }
				// buff持续时间置为0
				pWaitDelBuffInfo->AliveTimer = 0;
			}
		}

		void BuffEventTrigger_OtherBuffPreAddCheck(ParamBuffEvtTrg_OtherBuffPreAddCheck& param)
		{
			//检测要添加的buff,是否免疫
			if (param.in_RefEffect.values.size() < 1)
				return;

			int buff_id = param.in_RefEffect.values[0].value;

			if (buff_id != param.in_OtherBuffID)
				param.out_bCanAdd = true;
			else
				param.out_bCanAdd = false;
		}
	};
}
