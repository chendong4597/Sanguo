/*!
 * \file PassiveSkillMgr.h
 *
 * \author Dexuan Yang
 * \date 2019/10/04
 *
 */

#pragma once

#include <unordered_map>
#include <queue>
#include <array>
#include "BaseTypeDefine.h"


using namespace std;

class SkillType;
class ItemData;
class PassiveTrigger;
class Role;
//class LocalServer_Battle_Clock;
//class MovementComponentEvent;
class LuaSkillEvent;
//class RoleComponent;
struct PassiveEvent_Follow_NpcOrPetChange;
struct PassiveEvent_BeAtk;
struct PassiveEvent_Attack;
struct PassiveEvent_Hit;
struct PassiveEvent_WaterInOut;
class TransformSetPositionEvent;
//class HpEvent;
struct PassiveEvent_KillSomeone;
struct PassiveEvent_RoleInOutSight;

class PassiveSkillMgr
{
public:
	enum class PassiveEventType : int
	{
		PASSIVE_EVENT_NONE = 0,
		PASSIVE_EVENT_CLOCK = 1,
		PASSIVE_EVENT_MOVING = 2,
		PASSIVE_EVENT_FOLLOW = 3,
		PASSIVE_EVENT_BEATK = 4,	// 这里还作为所有受击触发类型在m_mapPassives中的key，避免与 血值低于x% = 1 和 伤害结算前(减伤) = 2 冲突，值不可小于 2
		PASSIVE_EVENT_ATK = 5,		// 这里还作为所有攻击触发类型在m_mapPassives中的key，避免与 血值低于x% = 1 和 伤害结算前(减伤) = 2 冲突，值不可小于 2
		PASSIVE_EVENT_ATK_HIT = 6,	// 这里还作为所有攻击命中触发类型在m_mapPassives中的key，避免与 血值低于x% = 1 和 伤害结算前(减伤) = 2 冲突，值不可小于 2
		PASSIVE_EVENT_WATER = 7,
		PASSIVE_EVENT_POS_CHANGE = 8,//不同于移动事件，这里的是位置实时变化监听
		PASSIVE_EVENT_HP_CHANGE = 9, //血量变化，包括扣血和回血
		PASSIVE_EVENT_KILL_SOMEONE = 10,//击杀目标时触发
		PASSIVE_EVENT_MAPOBJ_INOUT_SIGHT = 11,	//MapObj进出视野

		PASSIVE_EVENT_MAX,
	};

	struct PassiveCommand
	{
		enum class Type : int
		{
			CAST_SKILL = 1,		// 释放技能
			COUNTERATK = 2,		// 反击
			SUCKING	= 3,		// 吸血
		};
		PassiveCommand::Type m_eType{ PassiveCommand::Type::CAST_SKILL };
		const SkillType* m_pSkillType;
		std::vector<NS::int32> m_vecTargetMapObjIds;
		glm::vec3 m_targetPosition;
		bool m_bTrigger;
		int m_nHurtValue;
	};

public:
	PassiveSkillMgr(std::shared_ptr<Role> pRole);
	~PassiveSkillMgr();

	void pushPassiveSkill(const SkillType* pSkillType, NS::I64 nItemUuid = 0);
	void removePassiveSkill(const SkillType* pSkillType, NS::I64 nItemUuid = 0);
	void refreshCommonSkills(std::vector<const SkillType*>& vecSkills);
	void refreshEquipmentSkills(std::vector<std::pair<const SkillType*, NS::I64>>& vecEquipmentSkills);

	void notifyEquipmentBroken(const SkillType* pSkillType, const std::shared_ptr<ItemData>& pEquipmentData, bool bIsBroken);
	//void notifySkillEndEvent(LuaSkillEvent& evt);

	void update(float dt);
	void pushActionCommand(const PassiveCommand& command, bool bDoRighNow = false);
	void pushDelayInitialize(PassiveTrigger* pPassiveTrigger);
	// 减伤技能的触发，返回减伤值
	float damageReduction() const;
	
	void getPassiveSkills(std::vector<NS::int32>& vecPassiveSkills);
	const SkillType* findSkillTypeById(NS::int32 nSkillId);
	bool canCastSkill(const SkillType* pSkillType) const;
	NS::int32 getCasterId() const;
	//const RoleComponent* getCasterRoleComponent() const;
	const bool isTriggerTargetSkill(int nSkillId) const;
	const void findOverrideFirstMobId(int& outId) const;

	void onDead();
	void onResurrection();

	//直接设置Role
	std::shared_ptr<Role> getRole() { return m_pRole; }
	void setRole(std::shared_ptr<Role> role) { m_pRole = role; }

private:
	static int getMapKeyBySkillType(const SkillType* pSkillType);

public:
	// 获取被动技能触发器
	PassiveTrigger* getPassiveTrigger(const SkillType* pSkillType, const std::shared_ptr<ItemData>& pEquipmentData);
	// 激活被动技能
	bool activateSkill(PassiveTrigger* pPassiveTrigger, const std::shared_ptr<ItemData>& pEquipmentData, bool bEnable);
	// 注册被动事件
	void registerPassiveListener(int nTriggerType);	
	void unRegisterPassiveListener(int nTriggerType);
	int getEventTypeByTriggerType(int nTriggerType);
	void doRegisterPassiveListener(PassiveEventType eEventType, bool bReg);
	// 受击事件
	void registerBeAtkEvent(bool bReg);
	void onBeAtkEvent(const PassiveEvent_BeAtk& evt);
	// 攻击事件
	void registerAtkEvent(bool bReg);
	void onAtkEvent(const PassiveEvent_Attack& evt);
	// 攻击命中事件
	void registerAtkHitEvent(bool bReg);
	void onAtkHitEvent(const PassiveEvent_Hit& evt);
	// 游戏时钟变更
	//void regsiterGameClockEvent(bool bReg);
	//void onGameClockChange(const LocalServer_Battle_Clock& evt);
	// 角色移动
	void registerMovementEvent(bool bReg);
	//void onMovementEvent(const MovementComponentEvent& evt);
	// NPC/PET跟随变更
	void registerFollowEvent(bool bReg);
	void onFollowNpcOrPetChange(const PassiveEvent_Follow_NpcOrPetChange& evt);
	//位置的Y轴变化
	void registerPositionChange(bool bReg);
	//void onPositionChange(TransformSetPositionEvent evt);
	// 出入水变更
	void registerWaterInOutEvent(bool bReg);
	void onWaterInOutChange(const PassiveEvent_WaterInOut& evt);
	//血量变化
	void registerHpChangeEvent(bool bReg);
	//void onHpChangeEvent(const HpEvent& evt);
	//击杀事件
	void registerKillSomeoneEvent(bool bReg);
	void onKillSomeone(const PassiveEvent_KillSomeone& evt);
	//MapObj进出视野事件
	void registerMapObjInOutSight(bool bReg);
	void onMapObjInOutSight(const PassiveEvent_RoleInOutSight& evt);
	// 触发被动技能效果
	void doCastPassiveSkill(const PassiveCommand& command);
	void doCancelPassiveEffect(const SkillType* pSkillType, const std::vector<NS::int32>& vecTargetMapObjIds);

	void doTriggerAction();
	void doDelayInitialze();

	

private:
	//SkillComponent* m_pSkillComponent{ nullptr };
	// passive skill map
	std::unordered_map<int, PassiveTrigger*> m_mapPassives;
	// event ref counter
	std::array<int, (size_t)PassiveEventType::PASSIVE_EVENT_MAX> m_arrayCounters;
	// passive update vector
	std::queue<PassiveCommand> m_vecPassiveCommands;
	// 延时初始化的触发器，用于Loading阶段，数据不完全，延时到地图加载完成在执行初始化操作
	std::vector<PassiveTrigger*> m_vecDelayInitializes;

private:
	// 触发类型和被动事件映射
	const static std::unordered_map<int, PassiveEventType> m_PassiveEvents;

	std::shared_ptr<Role> m_pRole{};
};
