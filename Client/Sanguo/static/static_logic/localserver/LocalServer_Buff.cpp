/*!
* \file LocalServer_Buff.cpp
*
* \author zhichenxia
* \date 2018/6/13
*
*
*/
#include "stdafx.h"
#include "LocalServer_Buff.h"
#include <data/role/Player.h>
#include <config/BuffCfg.h>
#include "config/BuffGroupRuleCfg.h"
#include "../handler/HandlerRole.h"
#include "data/role/Monsters.h"
#include <common/Heartbeat.h>
//#include <GameObject.h>
#include "SkillBuffStrategy.h"
#include "../common/SyncCache.h"
#include "common/ClientSpec.h"
#include "common/ComSkillBuff.h"
#include "common/SkillEffect.h"

using namespace MsgPB;

NS_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SkillBuff

struct SkillEventHandler {};


class SkillBuff : public ComSkillBuff
{
public:
	static SkillBuff& getInstance(void);

public:
	SkillBuff(void) = default;

public:
	// Buff离线同步机制
	void clearBuffContainerMap();
	void syncBuffUpdate(unsigned int time_escaple);
	void restoreOfflineBuff(const MsgPB::MsgUserBuffData::Buff& buff, int targetId);

	// 获取效果命中
	int32 CB_GetObjBuffControlRate(const int32 nMapObj);
	// 获取效果抵抗
	int32 CB_GetObjBuffControlAntiRate(const int32 nMapObj);

private:
	// 是否debuff
	virtual bool IsDebuff(int nBuffId) override;

	// Buff免疫规则
	virtual bool CB_AddBuffPreCheck(SkillBuffContainer& buffContainer, const ParamInAddBuff& ParamIn) override;
	// Buff替换/互斥规则
	virtual	SkillBuffInfo* CheckReplaceBuffExist(int nGroupID, SkillBuffContainer& buffContainer) override;
	virtual SkillBuffInfo* CheckExclusionBuffExist(int nGroupID, SkillBuffContainer& buffContainer) override;
	// Buff锁定机制
	virtual void BuffEventLock(SkillBuffInfo& buff, int effectIndex) override;
	virtual void BuffEventUnlock(SkillBuffInfo& buff, int effectIndex) override;
	// Buff等级规则
	virtual bool IsSameGroupHighRankBuffActive(int32& BuffIndex_LockBy, const SkillBuffContainer& buffContainer, const SkillBuffInfo& buff) override;
	// Buff策略更新
	virtual void BuffEventBegin(SkillBuffInfo& pBuff, int EffectIndex) override;
	virtual void BuffEventUpdate(SkillBuffInfo& pBuff, int EffectIndex) override;
	virtual void BuffEventEnd(SkillBuffInfo& pBuff, int EffectIndex) override;

	// 更新Role属性
	virtual void UpdateAttr(int objOwner, bool bUpdate) override;
	// Buff护盾
	virtual void InitBuffDurability(SkillBuffInfo& pBuff) override;

	// Buff同步消息
	virtual void UpdateBuffToClient(int objOwner, SkillBuffInfo* pBuff, long long SerialID, BUFF_EVENT nEvent) override;

private:
	skill::TSkillBuffStrategy<SkillEventHandler>* getStrategy(ROLE_EFFECT_TYPE type);

private:
	// 离线同步时间
	unsigned int m_syncUpdateTime{ 0 }; 
};

SkillBuff& SkillBuff::getInstance(void)
{
	static SkillBuff s_SkillBuff;
	return s_SkillBuff;
}

void SkillBuff::clearBuffContainerMap()
{
	mapBuffContainer.clear();
}

skill::TSkillBuffStrategy<SkillEventHandler>* SkillBuff::getStrategy(ROLE_EFFECT_TYPE type)
{
	static skill::TSkillBuffStrategy_Dot      <SkillEventHandler, SkillEffectParam, MapCharacter> dot;
	static skill::TSkillBuffStrategy_OxyDot   <SkillEventHandler, SkillEffectParam, MapCharacter> oxyDot;
	static skill::TSkillBuffStrategy_Restore  <SkillEventHandler, SkillEffectParam, MapCharacter> restore;
	static skill::TSkillBuffStrategy_CastSkill<SkillEventHandler                                > castSkill;
	static skill::TSkillBuffStrategy_RoleAttr <SkillEventHandler								> attr;

	static skill::TSkillBuffStrategy_Marking<SkillEventHandler> marking;
	static skill::TSkillBuffStrategy_AddPassiveSkill<SkillEventHandler> addPassiveSkill;
	static skill::TSkillBuffStrategy_BuffImmune<SkillEventHandler> buffImmune;

	// 角色
	if (type > ROLE_EFFECT_NULL&& type < ROLE_EFFECT_MAX)
	{
		// 角色屬性改變
		if (type < ROLE_EFFECT_ATTR_MAX)
			return &attr; // m_BuffStrategys[BUFF_STRATEGY_ROLE_ATTR];

		//// 角色狀態改變
		//if (type == ROLE_EFFECT_SKILL_STATUS)
		//	return m_BuffStrategys[BUFF_STRATEGY_ROLE_STATE];

		// 施展技能
		if (type == ROLE_EFFECT_CAST_SKILL)
			return &castSkill;

		//// 輸用帶
		//if (type == ROLE_EFFECT_TRANSPORT)
		//	return m_BuffStrategys[BUFF_STRATEGY_TRANSPORT];

		//// 探勘狀態
		//if (type == ROLE_EFFECT_PROSPECT)
		//	return m_BuffStrategys[BUFF_STRATEGY_PROSPECT];

		//// 探測狀態_礦石
		//if (type == ROLE_EFFECT_DETECTING_ORE)
		//	return m_BuffStrategys[BUFF_STRATEGY_DETECTING_ORE];

		//// 探測狀態_地宮
		//if (type == ROLE_EFFECT_DETECTING_DUNGEON)
		//	return m_BuffStrategys[BUFF_STRATEGY_DETECTING_DUNGEON];
	}

	// Buff持續效果
	BUFF_EFFECT_TYPE BuffEffectType = (BUFF_EFFECT_TYPE)type;
	if (BuffEffectType > BUFF_EFFECT_NULL&& BuffEffectType < BUFF_EFFECT_MAX)
	{
		switch (BuffEffectType)
		{
		case BUFF_EFFECT_REAL_DOT:
		case BUFF_EFFECT_FIRE_DOT:
		case BUFF_EFFECT_FORST_DOT:
		case BUFF_EFFECT_THUNDER_DOT:
		case BUFF_EFFECT_POSION_DOT:
		case BUFF_EFFECT_LIGHT_DOT:
		case BUFF_EFFECT_DARK_DOT:
		case BUFF_EFFECT_WEAPON_DOT:
			return &dot;

		case BUFF_EFFECT_OXY_DOT:
			return &oxyDot;

		case BUFF_EFFECT_HP_RESTORE:
		case BUFF_EFFECT_MP_RESTORE:
			return &restore;

		case BUFF_EFFECT_MARKING:
			return &marking;


		case BUFF_EFFECT_CAST_SKILL_WHEN_OBJ_IN_VIEW:
		case BUFF_EFFECT_ADD_PASSIVE_SKILL:
			return &addPassiveSkill;

		case BUFF_EFFECT_BUFF_IMMUNE:
			return &buffImmune;

		default:
			break;
		}
	}
	return nullptr;
}

void SkillBuff::syncBuffUpdate(unsigned int time_escaple)
{
	//玩家角色
	const std::shared_ptr<Player> pPlayer = PlayerMgr::getInstance().getSelf();
	if (!pPlayer) return;

	// 1000ms更新一次
	static unsigned int FixSyncUpdateTimeInterval = 1000;
	m_syncUpdateTime += time_escaple;
	if (m_syncUpdateTime < FixSyncUpdateTimeInterval)
		return;
	unsigned int UpdateTimer = m_syncUpdateTime;
	m_syncUpdateTime = 0;

	// 玩家身上的buff。
	SkillBuffContainer& buffContainer = mapBuffContainer[pPlayer->getId()];
	// 遍历同步
	auto doSyncFunc = [this, UpdateTimer, &buffContainer](SkillBuffInfo& buff) {
		// 闲置位
		if (buff.IsEmpty())
		{
			return;
		}
		// buff被锁定
		if (buff.locked)
		{
			return;
		}
		// buff过期或者耗光耐久
		if (buff.IsExpired() || buff.IsNotDurable())
		{
			return;
		}
		LocalServer_Buff::getInstance().syncBuffCache(&buff, BUFF_EVENT::BUFF_EVENT_UPDATE);
	};
	//执行操作
	buffContainer.ForEachBuff(doSyncFunc);
}

void SkillBuff::restoreOfflineBuff(const MsgPB::MsgUserBuffData::Buff& buff, int targetId)
{
	const BuffType* pBuffType = BuffCfg::getInstance().findBuffType(buff.buffdataid());
	if (!pBuffType) { return; }

	std::vector<S_BuffTemplate_effect> effects;
	for (const auto& v : pBuffType->effects)
	{
		if (v.type == 0u) continue;

		S_BuffTemplate_effect e;
		e.type = static_cast<ROLE_EFFECT_TYPE>(v.type);

		for (const auto& w : v.ref_values)
		{
			if (w.attrref == ATTR_REF_NULL) continue;
			e.values.push_back(w);
		}

		effects.push_back(e);
	}

	SkillBuffContainer& buffContainer = mapBuffContainer[targetId];
	int buffIndex = buff.index();
	// 取得Buff
	SkillBuffInfo* pBuff = buffContainer.GetBuff(buffIndex);
	if (pBuff == nullptr) { return; }

	// 設定Buff資料
	pBuff->BuffDataID = buff.buffdataid();
	pBuff->AliveTimer = buff.alivetimer();
	pBuff->HeapCounter = buff.heapcounter();
	pBuff->Param = buff.param();
	pBuff->nBuffDel = pBuffType->n_buff_del;
	pBuff->nEffectCount = buff.neffectcount();
	pBuff->nDmg = buff.ndmg();
	pBuff->nGroupId = buff.ngroupid();
	pBuff->nDurability = buff.ndurability();
	pBuff->locked = buff.locked();
	pBuff->CasterMapObjectID = targetId;
	pBuff->OwnerMapObjectID = targetId;
	pBuff->nEffectRate = buff.neffectcount();
	pBuff->IntervalTimer = pBuffType->n_interval_time;
	for (auto&& effect : effects)
	{
		pBuff->BuffEffects.push_back(effect);
	}
	pBuff->SerialID = -1;

	// 初始化buff耐久（护盾buff=护盾值，其他buff=0）
	InitBuffDurability(*pBuff);
	// 確認Buff狀態
	bool SetUpdateEvent = false;	// 是否更新角色數值
	// 更新給Client
	this->UpdateBuffToClient(targetId, pBuff, pBuff->SerialID, BUFF_EVENT::BUFF_EVENT_ADD);
	// 更新属性
	UpdateAttr(targetId, true);
	// 需要初始
	if (SetUpdateEvent)
		DoBuffEvent(BUFF_TRIGGER_INIT, *pBuff);
}

bool SkillBuff::IsDebuff(int buffId)
{
	auto pCfg = BuffCfg::getInstance().findBuffType(buffId);
	return pCfg && pCfg->is_debuff;
}

SkillBuffInfo* SkillBuff::CheckReplaceBuffExist(int nGroupID, SkillBuffContainer& buffContainer)
{
	// 讀入Buff Group配置表 來判別哪些Buff是可以取代
	for (SkillBuffInfo& v : buffContainer.GetBuffInfos())
	{
		if (BuffGroupRuleCfg::getInstance().checkReplace(nGroupID, v.nGroupId))
			return &v;
	}
	return nullptr;
}

SkillBuffInfo* SkillBuff::CheckExclusionBuffExist(int nGroupID, SkillBuffContainer& buffContainer)
{
	// 讀入Buff Group配置表 來判別哪些Buff是互斥
	for (SkillBuffInfo& v : buffContainer.GetBuffInfos())
	{
		if (BuffGroupRuleCfg::getInstance().checkExclusion(nGroupID, v.nGroupId))
			return &v;
	}
	return nullptr;
}

void SkillBuff::BuffEventLock(SkillBuffInfo& buff, int effectIndex)
{
	UpdateBuffToClient(buff.OwnerMapObjectID, &buff, buff.SerialID, BUFF_EVENT_LOCK);
}

void SkillBuff::BuffEventUnlock(SkillBuffInfo& buff, int effectIndex)
{
	UpdateBuffToClient(buff.OwnerMapObjectID, &buff, buff.SerialID, BUFF_EVENT_UNLOCK);
}

// 是否有同组优先级更高的buff运行中
bool SkillBuff::IsSameGroupHighRankBuffActive(int32& BuffIndex_LockBy, const SkillBuffContainer& buffContainer, const SkillBuffInfo& buff)
{
	const auto& pBuffTemplate = BuffCfg::getInstance().findBuffType(buff.BuffDataID);
	if (!pBuffTemplate) return false;
	if (pBuffTemplate->n_groupid <= 0)
	{
		return false;
	}

	bool active = false;
	buffContainer.ConstForEachBuff([&BuffIndex_LockBy, &active, &buff, &pBuffTemplate](const SkillBuffInfo& otherBuff)->bool {
		if (otherBuff.IsEmpty())
		{
			return true;
		}
		if (otherBuff.Index == buff.Index)
		{
			return true;
		}
		if (otherBuff.IsExpired() || otherBuff.IsNotDurable())
		{
			return true;
		}
		const auto& pOtherBuffTemplate = BuffCfg::getInstance().findBuffType(otherBuff.BuffDataID);
		if (!pOtherBuffTemplate) return true;
		if (pOtherBuffTemplate->n_groupid == pBuffTemplate->n_groupid
			&& pOtherBuffTemplate->n_group_rank > pBuffTemplate->n_group_rank)
		{
			active = true;
			BuffIndex_LockBy = otherBuff.Index;

			return false;
		}
		return true;
		});
	return active;
}

void SkillBuff::BuffEventBegin(SkillBuffInfo& pBuff, int EffectIndex)
{
	auto pStrategy = getStrategy(pBuff.BuffEffects[EffectIndex].type);
	if (!pStrategy) return;

	pStrategy->BuffInit(pBuff, EffectIndex, nullptr);
}

void SkillBuff::BuffEventUpdate(SkillBuffInfo& pBuff, int EffectIndex)
{
	auto pStrategy = getStrategy(pBuff.BuffEffects[EffectIndex].type);
	if (!pStrategy) return;

	auto pGameObject = HandlerRole::findRole(pBuff.OwnerMapObjectID);
	if (!pGameObject) return;
	//auto pTargetData = pGameObject->findComponent<RoleComponent>()->getData();

	//if (pTargetData->isAlive())
	//{
		pStrategy->AutoHeap(pBuff, nullptr);
		pStrategy->BuffUpdate(pBuff, EffectIndex, nullptr);
	//}
}

void SkillBuff::BuffEventEnd(SkillBuffInfo& pBuff, int EffectIndex)
{
	auto pStrategy = getStrategy(pBuff.BuffEffects[EffectIndex].type);
	if (!pStrategy) return;

	pStrategy->BuffEnd(pBuff, EffectIndex, nullptr);
}

void SkillBuff::UpdateAttr(int objOwner, bool bUpdate)
{
	// 通知角色數值要更新
	if (auto pMapObj = MapObjMgr::getInstance().findObj(objOwner))
	{
		if (auto pRole = std::dynamic_pointer_cast<Role>(pMapObj))
		{
			pRole->calcAttr();
		}
	}
}

void SkillBuff::InitBuffDurability(SkillBuffInfo& pBuff)
{
	//if (pBuff.BuffEffects.empty()) { return; }

	//// 施法者。
	//auto pGameObject = HandlerRole::findRole(pBuff.CasterMapObjectID);
	//if (!pGameObject) return;
	//auto pCasterData = pGameObject->findComponent<RoleComponent>()->getData();
	//auto pCaster = std::make_shared<MapCharacter>(CharacterAttributeSnapshot(pCasterData), pCasterData->getMaxHp(), pCasterData->getMaxMp(), pCasterData->getCategory(), pCasterData->getMobRare());

	//// 目标。
	//pGameObject = HandlerRole::findRole(pBuff.OwnerMapObjectID);
	//if (!pGameObject) return;
	//auto pTargetData = pGameObject->findComponent<RoleComponent>()->getData();
	//auto pTarget = std::make_shared<MapCharacter>(CharacterAttributeSnapshot(pTargetData), pTargetData->getMaxHp(), pTargetData->getMaxMp(), pTargetData->getCategory(), pTargetData->getMobRare());

	//SkillRoleValue casterRoleValue;
	//casterRoleValue.GetSkillRoleValue_ByCharacter(pCaster);

	//SkillRoleValue targetRoleValue;
	//targetRoleValue.GetSkillRoleValue_ByCharacter(pTarget);

	//for (const auto& buffEffect : pBuff.BuffEffects)
	//{
	//	switch (buffEffect.type)
	//	{
	//	case BUFF_EFFECT_SHIELD:
	//	case BUFF_EFFECT_REAL_SHIELD:
	//	case BUFF_EFFECT_FIRE_SHIELD:
	//	case BUFF_EFFECT_FORST_SHIELD:
	//	case BUFF_EFFECT_THUNDER_SHIELD:
	//	case BUFF_EFFECT_EARTH_SHIELD:
	//	case BUFF_EFFECT_POSION_SHIELD:
	//	case BUFF_EFFECT_LIGHT_SHIELD:
	//	case BUFF_EFFECT_WEAPON_SHIELD:
	//	case BUFF_EFFECT_DARK_SHIELD:
	//	{
	//		std::vector<int32> EffectResults{};
	//		pBuff.nDurability += skill::TSkillEffect<SkillRoleValue, SkillEffectParam, MapCharacter>::GetSkillEffectValue
	//		(
	//			casterRoleValue,
	//			targetRoleValue,
	//			buffEffect.values,
	//			1000 * std::max(pBuff.HeapCounter, 1),
	//			EffectResults
	//		);
	//		break;
	//	}
	//	default:
	//		break;
	//	}
	//}
}

bool SkillBuff::CB_AddBuffPreCheck(SkillBuffContainer& buffContainer, const SkillBuff::ParamInAddBuff& ParamIn)
{
	//是否现有的buff免疫
	std::list<int32> lstDelIdx;
	bool bRet = true;
	auto pLambda = [this, &bRet, &ParamIn](const SkillBuffInfo& buff) {

		for (auto&& iter : buff.BuffEffects)
		{
			auto pBuffStrategy = getStrategy(iter.type);
			if (!pBuffStrategy)
				continue;

			skill::ParamBuffEvtTrg_OtherBuffPreAddCheck ParamCheck(buff, iter);
			ParamCheck.in_OtherBuffID = ParamIn.BuffDataID;
			pBuffStrategy->BuffEventTrigger_OtherBuffPreAddCheck(ParamCheck);

			bRet = bRet && ParamCheck.out_bCanAdd;
		}
	};
	buffContainer.ForEachNoEmptyBuff(pLambda);

	return bRet;
}

int32 SkillBuff::CB_GetObjBuffControlRate(const int32 nMapObjId)
{
	std::shared_ptr<MapObj> pMapObj = MapObjMgr::getInstance().findObj(nMapObjId);
	if (!pMapObj) { return 0; }

	std::shared_ptr<Role> pRoleData = std::dynamic_pointer_cast<Role>(pMapObj);
	if (!pRoleData) { return 0; }

	return static_cast<int32>(pRoleData->getAttr(MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_CC_HIT));
}

int32 SkillBuff::CB_GetObjBuffControlAntiRate(const int32 nMapObjId)
{
	std::shared_ptr<MapObj> pMapObj = MapObjMgr::getInstance().findObj(nMapObjId);
	if (!pMapObj) { return 0; }

	std::shared_ptr<Role> pRoleData = std::dynamic_pointer_cast<Role>(pMapObj);
	if (!pRoleData) { return 0; }

	return static_cast<int32>(pRoleData->getAttr(MsgPB::ROLE_EFFECT_TYPE::ROLE_EFFECT_CC_RES));
}

// Buff 更新給 Client
void SkillBuff::UpdateBuffToClient(int objOwner, SkillBuffInfo* pBuff, long long SerialID, BUFF_EVENT nEvent)
{
	MsgBuffInfo_S2C msg;
	msg.set_buff_event(nEvent);

	auto p = msg.add_buff_infos();
	p->set_index(pBuff->Index);
	p->set_buff_dataid(pBuff->BuffDataID);
	p->set_alive_timer(pBuff->AliveTimer);
	p->set_heap_counter(pBuff->HeapCounter);
	p->set_owner_mapobjectid(pBuff->OwnerMapObjectID);
	p->set_caster_mapobjectid(pBuff->CasterMapObjectID);
	p->set_effect_rate(pBuff->nEffectRate);
	p->set_buff_durability(pBuff->nDurability);

	// 不生效的效果索引
	for (size_t i = 0; i < pBuff->BuffEffects.size(); i++)
	{
		const S_BuffTemplate_effect_RT& effect = pBuff->BuffEffects[i];
		if (!effect.m_bActive)
		{
			p->add_effect_idx_unactive((int)i);
		}
	}

	LocalServer_Buff::getInstance().send(msg);

	//下线同步玩家buff状态
	LocalServer_Buff::getInstance().syncBuffCache(pBuff, nEvent);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LocalServer_Buff

LocalServer_Buff& LocalServer_Buff::getInstance(void)
{
	static LocalServer_Buff instance;
	return instance;
}

bool LocalServer_Buff::addBuff(int32 buffId, int32 target_id, int32 caster_id /* = -1 */, NS::uint32 lastTime /* = 0 */, NS::int32 nAddHeap /* = 1 */)
{
	static long long serialId{};

	//std::cerr << "add_buff : target_id = " << target_id << "buffId = " << buffId << std::endl;
	auto pCfg = BuffCfg::getInstance().findBuffType(buffId);
	if (!pCfg) return false;

	std::vector<S_BuffTemplate_effect> effects;
	for (const auto& v : pCfg->effects)
	{
		if (v.type == 0u) continue;

		S_BuffTemplate_effect e;
		e.type = static_cast<ROLE_EFFECT_TYPE>(v.type);

		for (const auto& w : v.ref_values)
		{
			if (w.attrref == ATTR_REF_NULL) continue;
			e.values.push_back(w);
		}

		effects.push_back(e);
	}

	if (lastTime == 0)
		lastTime = pCfg->n_alive_time;

	int nAttrRuntimeEffectRate = 1000;

	SkillBuff::ParamInAddBuff param(effects);

	param.objCaster = caster_id > 0 ? caster_id : target_id;
	param.objTarget = target_id;
	param.Caster_CC_HIT = SkillBuff::getInstance().CB_GetObjBuffControlRate(param.objCaster);
	param.Target_CC_RES = SkillBuff::getInstance().CB_GetObjBuffControlAntiRate(param.objTarget);
	param.BuffDataID = buffId;
	param.AliveTimer = lastTime;
	param.IntervalTimer = pCfg->n_interval_time;
	param.nGroupId = pCfg->n_groupid;
	param.nMaxHeap = pCfg->n_max_heap;

	param.nBuffDel = pCfg->n_buff_del;
	param.SerialID = ++serialId;
	param.AttrRuntimeEffectRate = nAttrRuntimeEffectRate;
	param.nAddHeap = nAddHeap;

	return SkillBuff::getInstance().AddBuff(param);
}

bool bzbee::LocalServer_Buff::removeBuff(int32 buffId, int32 target_id)
{
	return SkillBuff::getInstance().DelBuff_ByBuffID(target_id, buffId);
}

bool LocalServer_Buff::delAllBuff(int target_id, bool bDebuff)
{
	return SkillBuff::getInstance().DelAllBuff(target_id, bDebuff);
}

bool LocalServer_Buff::delOneDebuff(int target_id)
{
	return SkillBuff::getInstance().DelOneDeBuff(target_id);
}

int LocalServer_Buff::SearchBuffIndex(int CasterMapObjectID, int BuffDataID)
{
	return SkillBuff::getInstance().SearchBuffIndex(CasterMapObjectID, BuffDataID);
}

void bzbee::LocalServer_Buff::clearBuffContainerMap()
{
	SkillBuff::getInstance().clearBuffContainerMap();
}

void bzbee::LocalServer_Buff::saveRestoreBuff(const MsgPB::MsgUserBuffData::Buff& msgBuff)
{
	m_vecPlayerRestoreBuffs.push_back(msgBuff);
}

void bzbee::LocalServer_Buff::restoreBuff()
{
	if (!getEnable())
	{
		m_vecPlayerRestoreBuffs.clear();
		return;
	}

	auto pSelf = PlayerMgr::getInstance().getSelf();
	if (!pSelf)
	{
		m_vecPlayerRestoreBuffs.clear();
		return;
	}
//	int playerId = pSelf->getId();
	for (auto&& restoreBuff : m_vecPlayerRestoreBuffs) {
		// 还原离线buff状态
		SkillBuff::getInstance().restoreOfflineBuff(restoreBuff, pSelf->getId());
	}

	m_vecPlayerRestoreBuffs.clear();
}

void LocalServer_Buff::update(float dt)
{
	unsigned int time_escaple = static_cast<unsigned int>(dt * 1000.0f);
	SkillBuff::getInstance().Update(static_cast<unsigned int>(dt * 1000.0f));
	// 同步buff
	SkillBuff::getInstance().syncBuffUpdate(time_escaple);
}

bool LocalServer_Buff::syncBuffCache(const SkillBuffInfo* pBuff, int buffEvent)
{
	if (!pBuff) { return false; }

	//玩家自身存在且buff持有者为玩家自身
	const std::shared_ptr<Player> pSelf = PlayerMgr::getInstance().getSelf();
	if (!pSelf || pSelf->getId() != pBuff->OwnerMapObjectID) { return false; }

	//buff配置是否需要保存
	const BuffType* pBuffType = BuffCfg::getInstance().findBuffType(pBuff->BuffDataID);
	if (!pBuffType || !pBuffType->isNeedSaveBuff()) { return false; }

	// 缓存同步消息
	MsgPB::MsgUserBuffData msg{};
	// push buff data
	MsgPB::MsgUserBuffData::Buff* buffData{ msg.mutable_data() };
	buffData->set_index(pBuff->Index);
	buffData->set_buffdataid(pBuff->BuffDataID);
	buffData->set_alivetimer(pBuff->AliveTimer);
	buffData->set_heapcounter(pBuff->HeapCounter);
	buffData->set_param(pBuff->Param);
	buffData->set_neffectcount(pBuff->nEffectCount);
	buffData->set_ndmg(pBuff->nDmg);
	buffData->set_ngroupid(pBuff->nGroupId);
	buffData->set_ndurability(pBuff->nDurability);
	buffData->set_locked(pBuff->locked);
	buffData->set_neffectrate(pBuff->nEffectRate);

	//idx, 可作为SyncCache唯一标识
	msg.set_idx(pBuff->Index + 1);
	msg.set_is_del(buffEvent == BUFF_EVENT::BUFF_EVENT_DEL ? 1 : 0);
	//if (pBuffType->n_logout_save == BuffType::BUFF_LOGOUT_SAVE::SAVE_AND_CONTINUE)
	//{
	//	msg.set_end_time(Heartbeat::getInstance().getServerTime() + pBuff->AliveTimer);
	//}
	//else
	//{
		msg.set_end_time(-1);
	//}

	//SyncCache::getInstance().setUserBuffData(msg);

	return true;
}

// 获取掉落Buff的掉落物品数量的倍率 (这个函数会直接根据配合的概率进行计算并返回结果，所以每次调用此函数得到的结果都不一定是一样的)，无掉落Buff时返回 1
int LocalServer_Buff::DropBuffMultiplyingPower(int nMapObjID)
{
	SkillBuffContainer* pSkillBuffContainer = FindBuffContainer(nMapObjID);
	if (pSkillBuffContainer)
	{
		std::vector<SkillBuffInfo> vecSkillBuffInfo = pSkillBuffContainer->GetBuffInfos();
		if (!vecSkillBuffInfo.empty())
		{
			std::vector<SkillBuffInfo>::iterator iter = vecSkillBuffInfo.begin();
			for (; iter != vecSkillBuffInfo.end(); ++iter)
			{
				SkillBuffInfo info = *iter;
				if (!info.locked)
				{
					int nBuffDataID = info.BuffDataID; 
					const BuffType* pBuffType = BuffCfg::getInstance().findBuffType(nBuffDataID);
					if (pBuffType)
					{
						for (auto effect : pBuffType->effects) 
						{
							if (effect.type == BUFF_EFFECT_TYPE::BUFF_EFFECT_BUFF_KILL_MOB_DROP)
							{
								if (effect.ref_values.size() >= 2)
								{
									int nProbability = effect.ref_values[0].value;
									int nMultiplyingPower = effect.ref_values[1].value;
									int nRandom = bzbee::RandomUtil::getRandomInt(1, 100);
									if (nRandom <= nProbability)
									{
										return nMultiplyingPower;
									}
								}								
							}
						}
					}
				}
			}
		}
	}
	return 1;
}

SkillBuffContainer* LocalServer_Buff::FindBuffContainer(int idObj)
{
	return SkillBuff::getInstance().FindBuffContainer(idObj);
}

int LocalServer_Buff::countDebuff(int nTargetId)
{
	return SkillBuff::getInstance().CountDebuff(nTargetId);
}

NS_END


//void LUA(add_buff)(int32 buffId, int32 target_id)
//{
//	if (target_id <= 0)
//	{
//		auto pSelf = PlayerMgr::getInstance().getSelf();
//		if (!pSelf) return;
//		target_id = pSelf->getId();
//	}
//
//	NS::LocalServer_Buff::getInstance().addBuff(buffId, target_id, target_id);
//}
//
//
//void LUA(add_buff_ex)(int32 buffId, int32 target_id, int32 caster_id)
//{
//	if (target_id <= 0)
//	{
//		auto pSelf = PlayerMgr::getInstance().getSelf();
//		if (!pSelf) return;
//		target_id = pSelf->getId();
//	}
//
//	NS::LocalServer_Buff::getInstance().addBuff(buffId, target_id, caster_id);
//}
//
//void LUA(remove_buff)(int32 buffId, int32 target_id)
//{
//	if (target_id <= 0)
//	{
//		auto pSelf = PlayerMgr::getInstance().getSelf();
//		if (!pSelf) return;
//		target_id = pSelf->getId();
//	}
//	NS::LocalServer_Buff::getInstance().removeBuff(buffId, target_id);
//}
