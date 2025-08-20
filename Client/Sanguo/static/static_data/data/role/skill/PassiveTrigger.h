/*!
 * \file PassiveTrigger.h
 *
 * \author Dexuan Yang
 * \date 2019/10/04
 *
 *
 */

#pragma once

class SkillType;
class Role;
//class SkillComponent;
class PassiveSkillMgr;
//class RoleComponent;

union OtherParam;

class PassiveTrigger
{
public:
	enum class PassiveState : int
	{
		NONE = 0,
		INITIALIZED = 1,
		UNTRIGGER = 2,
		TRIGGERING = 3,
	};
	enum class ConditionState : int
	{
		NONE = 0,
		SUCCESS = 1,
		FAILED = 2,
	};

public:
	PassiveTrigger(const SkillType* pSkillType, PassiveSkillMgr* pPassiveSkillMgr, NS::I64 nEquipmentUuid);
	virtual ~PassiveTrigger();

public:
	virtual void initialize(bool bForce = false);
	virtual void unInitialize();
	virtual void onDead();
	virtual void onResurrection();

	void setTriggerEnable(bool bEnable);
	bool getTriggerEnable() const { return m_bTriggerEnable; }
	bool trigger(const OtherParam& otherParam);

	int getEquipmentPosIndex() const;
	int getEquipmentCfgId() const;
	NS::I64 getEquipmentUuid() const { return m_nEquipmentUuid; }

	const SkillType* getSkillType() const { return m_pSkillType; }
	inline bool isTriggering() const { return m_eState == PassiveState::TRIGGERING; }

protected:
	virtual ConditionState checkCondition(const OtherParam& otherParam) = 0; // 被动技能触发条件
	virtual bool checkPreCondition(); // 被动技能触发前置条件
	virtual void doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) = 0;
	virtual void doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) = 0;

	Role* getCasterRoleData() const;

protected:
	static bool checkDelayInitialize(const Role* pRoleData);

protected:
	const SkillType* m_pSkillType{ nullptr };
	PassiveSkillMgr* m_pPassiveSkillMgr{ nullptr };
	bool m_bTriggerEnable{ false };
	PassiveState m_eState{ PassiveState::NONE };

	NS::I64 m_nEquipmentUuid{ 0 };	// 关联的装备ID，ID == 0 意味着此被动技能非装备带来
};
///////////////////////////////////////////////////////////////////////////////////////////
/**
 * n_trigger_type = 16384 | 65536 | 131072 | 33554432, null trigger, 由AI控制的
 */
class PassiveTrigger_Ai final : public PassiveTrigger
{
public:
	PassiveTrigger_Ai(const SkillType* pSkillType, PassiveSkillMgr* pPassiveSkillMgr, NS::I64 nEquipmentUuid)
		: PassiveTrigger(pSkillType, pPassiveSkillMgr, nEquipmentUuid)
	{}

protected:
	ConditionState checkCondition(const OtherParam& otherParam)override { return ConditionState::NONE; }
	void doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override {}
	void doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override {}
};

///////////////////////////////////////////////////////////////////////////////////////////
/**
 * n_trigger_type = 1, hp less
 */
class PassiveTrigger_HpLess final : public PassiveTrigger
{
public:
	PassiveTrigger_HpLess(const SkillType* pSkillType, PassiveSkillMgr* pPassiveSkillMgr, NS::I64 nEquipmentUuid)
		: PassiveTrigger(pSkillType, pPassiveSkillMgr, nEquipmentUuid)
	{}

	void initialize(bool bForce = false) override;
	void unInitialize() override;

protected:
	ConditionState checkCondition(const OtherParam& otherParam) override;
	void doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;
	void doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;
};

///////////////////////////////////////////////////////////////////////////////////////////
/**
 * n_trigger_type = 2, Damage reduction, before damage calculation
 */
class PassiveTrigger_DamageReduction final : public PassiveTrigger
{
public:
	PassiveTrigger_DamageReduction(const SkillType* pSkillType, PassiveSkillMgr* pPassiveSkillMgr, NS::I64 nEquipmentUuid)
		: PassiveTrigger(pSkillType, pPassiveSkillMgr, nEquipmentUuid)
	{}

protected:
	ConditionState checkCondition(const OtherParam& otherParam) override;
	void doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;
	void doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override {} 
};

///////////////////////////////////////////////////////////////////////////////////////////
/**
 * n_trigger_type = 4 & 32, under normal / skill attack, remove buff
 */
class PassiveTrigger_BeAtkRemoveBuff final : public PassiveTrigger
{
public:
	PassiveTrigger_BeAtkRemoveBuff(const SkillType* pSkillType, PassiveSkillMgr* pPassiveSkillMgr, NS::I64 nEquipmentUuid)
		: PassiveTrigger(pSkillType, pPassiveSkillMgr, nEquipmentUuid)
	{}

protected:
	ConditionState checkCondition(const OtherParam& otherParam) override;
	void doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;
	void doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override {} // do nothing, beAtk is transient event
};

///////////////////////////////////////////////////////////////////////////////////////////
/**
 * n_trigger_type = 8 & 64, under normal / skill attack, cast skill
 */
class PassiveTrigger_BeAtkCastSkill final : public PassiveTrigger
{
public:
	PassiveTrigger_BeAtkCastSkill(const SkillType* pSkillType, PassiveSkillMgr* pPassiveSkillMgr, NS::I64 nEquipmentUuid)
		: PassiveTrigger(pSkillType, pPassiveSkillMgr, nEquipmentUuid)
	{}

protected:
	ConditionState checkCondition(const OtherParam& otherParam) override;
	void doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;
	void doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override {} // do nothing, beAtk is transient event
};

///////////////////////////////////////////////////////////////////////////////////////////
/**
 * n_trigger_type = 16 & 128, under normal / skill attack, CounterAttack
 */
class PassiveTrigger_CounterAttack final : public PassiveTrigger
{
public:
	PassiveTrigger_CounterAttack(const SkillType* pSkillType, PassiveSkillMgr* pPassiveSkillMgr, NS::I64 nEquipmentUuid)
		: PassiveTrigger(pSkillType, pPassiveSkillMgr, nEquipmentUuid)
	{}

protected:
	ConditionState checkCondition(const OtherParam& otherParam) override;
	void doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;
	void doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override {} // do nothing, beAtk is transient event
};

///////////////////////////////////////////////////////////////////////////////////////////
/**
 * n_trigger_type = 256 & 1024, normal / skill attack, suck blood
 */
class PassiveTrigger_BloodSucking final : public PassiveTrigger
{
public:
	PassiveTrigger_BloodSucking(const SkillType* pSkillType, PassiveSkillMgr* pPassiveSkillMgr, NS::I64 nEquipmentUuid)
		: PassiveTrigger(pSkillType, pPassiveSkillMgr, nEquipmentUuid)
	{}

protected:
	ConditionState checkCondition(const OtherParam& otherParam) override;
	void doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;
	void doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override {}; // do nothing
};

///////////////////////////////////////////////////////////////////////////////////////////
/**
 * n_trigger_type = 512 & 2048, normal / skill attack, cast skill
 */
class PassiveTrigger_AtkCastSkill final : public PassiveTrigger
{
public:
	PassiveTrigger_AtkCastSkill(const SkillType* pSkillType, PassiveSkillMgr* pPassiveSkillMgr, NS::I64 nEquipmentUuid)
		: PassiveTrigger(pSkillType, pPassiveSkillMgr, nEquipmentUuid)
	{}

protected:
	ConditionState checkCondition(const OtherParam& otherParam) override;
	void doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;
	void doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override {}; // do nothing
};

///////////////////////////////////////////////////////////////////////////////////////////
/**
 * n_trigger_type = 32768, no condition
 */
class PassiveTrigger_ByNothing final : public PassiveTrigger
{
public:
	PassiveTrigger_ByNothing(const SkillType* pSkillType, PassiveSkillMgr* pPassiveSkillMgr, NS::I64 nEquipmentUuid)
		: PassiveTrigger(pSkillType, pPassiveSkillMgr, nEquipmentUuid)
	{}

public:
	void initialize(bool bForce = false) override;
	void unInitialize() override;

protected:
	ConditionState checkCondition(const OtherParam& otherParam) override;
	bool checkPreCondition() override; // 被动技能触发前置条件
	void doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;
	void doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * n_trigger_type = 262144, game clock change
 */
class PassiveTrigger_ByClock final : public PassiveTrigger
{
public:
	PassiveTrigger_ByClock(const SkillType* pSkillType, PassiveSkillMgr* pPassiveSkillMgr, NS::I64 nEquipmentUuid)
		: PassiveTrigger(pSkillType, pPassiveSkillMgr, nEquipmentUuid)
	{}

public:
	void initialize(bool bForce = false) override;
	void unInitialize() override;

protected:
	ConditionState checkCondition(const OtherParam& otherParam) override;
	bool checkPreCondition() override; // 被动技能触发前置条件
	void doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;
	void doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * n_trigger_type = 524288, role is falling
 */
class PassiveTrigger_ByFalling final : public PassiveTrigger
{
public:
	PassiveTrigger_ByFalling(const SkillType* pSkillType, PassiveSkillMgr* pPassiveSkillMgr, NS::I64 nEquipmentUuid)
		: PassiveTrigger(pSkillType, pPassiveSkillMgr, nEquipmentUuid)
	{}

public:
	void unInitialize() override;
	void onDead() override;

protected:
	ConditionState checkCondition(const OtherParam& otherParam) override;
	bool checkPreCondition() override; // 被动技能触发前置条件
	void doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;
	void doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;

private:
	bool m_bBeginFalling{ false };
	int m_curDist{ 0 };
};

//////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * n_trigger_type = 1048576, role is in water
 */
class PassiveTrigger_InWater final : public PassiveTrigger
{
public:
	PassiveTrigger_InWater(const SkillType* pSkillType, PassiveSkillMgr* pPassiveSkillMgr, NS::I64 nEquipmentUuid)
		: PassiveTrigger(pSkillType, pPassiveSkillMgr, nEquipmentUuid)
	{}

public:
	void initialize(bool bForce = false) override;
	void unInitialize() override;

protected:
	ConditionState checkCondition(const OtherParam& otherParam) override;
	bool checkPreCondition() override; // 被动技能触发前置条件
	void doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;
	void doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * n_trigger_type = 2097152, role is moving
 */
class PassiveTrigger_ByMoving final : public PassiveTrigger
{
public:
	PassiveTrigger_ByMoving(const SkillType* pSkillType, PassiveSkillMgr* pPassiveSkillMgr, NS::I64 nEquipmentUuid)
		: PassiveTrigger(pSkillType, pPassiveSkillMgr, nEquipmentUuid)
	{}
	void unInitialize() override;
	void onDead() override;

protected:
	ConditionState checkCondition(const OtherParam& otherParam) override;
	void doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;
	void doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * n_trigger_type = 4194304, role is sprinting
 */
class PassiveTrigger_BySprinting final : public PassiveTrigger
{
public:
	PassiveTrigger_BySprinting(const SkillType* pSkillType, PassiveSkillMgr* pPassiveSkillMgr, NS::I64 nEquipmentUuid)
		: PassiveTrigger(pSkillType, pPassiveSkillMgr, nEquipmentUuid)
	{}
	void unInitialize() override;
	void onDead() override;

protected:
	ConditionState checkCondition(const OtherParam& otherParam) override;
	void doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;
	void doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;

private:
	time_t m_startRun{ 0 };//施法者开始奔跑的时间
	glm::vec3 m_lastFpsWorldPos{};
	NS::int32 m_lastOrientationX{};
};

//////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * n_trigger_type = 8388608, npc or pet exit
 */
class PassiveTrigger_ByNpcOrPet final : public PassiveTrigger
{
public:
	PassiveTrigger_ByNpcOrPet(const SkillType* pSkillType, PassiveSkillMgr* pPassiveSkillMgr, NS::I64 nEquipmentUuid)
		: PassiveTrigger(pSkillType, pPassiveSkillMgr, nEquipmentUuid)
	{}
	void initialize(bool bForce = false) override;
	void unInitialize() override;

protected:
	ConditionState checkCondition(const OtherParam& otherParam) override;
	bool checkPreCondition() override;
	void doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;
	void doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;
private:
	NS::int32 m_lastNpcId{ -1 };
	NS::int32 m_lastPetId{ -1 };
};

//////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * n_trigger_type = 16777216, role position y
 */
class PassiveTrigger_ByPositionY final : public PassiveTrigger
{
public:
	PassiveTrigger_ByPositionY(const SkillType* pSkillType, PassiveSkillMgr* pPassiveSkillMgr, NS::I64 nEquipmentUuid)
		: PassiveTrigger(pSkillType, pPassiveSkillMgr, nEquipmentUuid)
	{}
	void initialize(bool bForce = false) override;
	void unInitialize() override;
protected:
	ConditionState checkCondition(const OtherParam& otherParam) override;
	void doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;
	void doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * n_trigger_type = 67108864, kill someone
 */
class PassiveTrigger_ByKillSomeone final : public PassiveTrigger
{
public:
	PassiveTrigger_ByKillSomeone(const SkillType* pSkillType, PassiveSkillMgr* pPassiveSkillMgr, NS::I64 nEquipmentUuid)
		: PassiveTrigger(pSkillType, pPassiveSkillMgr, nEquipmentUuid)
	{}

protected:
	ConditionState checkCondition(const OtherParam& otherParam) override;
	void doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;
	void doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override {}
};

//////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * n_trigger_type = 67108864, kill someone
 */
class PassiveTrigger_Bykilled final : public PassiveTrigger
{
public:
	PassiveTrigger_Bykilled(const SkillType* pSkillType, PassiveSkillMgr* pPassiveSkillMgr, NS::I64 nEquipmentUuid)
		: PassiveTrigger(pSkillType, pPassiveSkillMgr, nEquipmentUuid)
	{}

protected:
	ConditionState checkCondition(const OtherParam& otherParam) override;
	void doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;
	void doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override {};
};

//////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * n_trigger_type = 32769, aura
 */
class PassiveTrigger_Aura final : public PassiveTrigger
{
public:
	PassiveTrigger_Aura(const SkillType* pSkillType, PassiveSkillMgr* pPassiveSkillMgr, NS::I64 nEquipmentUuid)
		: PassiveTrigger(pSkillType, pPassiveSkillMgr, nEquipmentUuid)
	{}

public:
	void initialize(bool bForce = false) override;
	void unInitialize() override;
	void onDead() override;

protected:
	ConditionState checkCondition(const OtherParam& otherParam) override;
	void doTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;
	void doUnTriggerAction(NS::int32 nCasterMapObjId, const OtherParam& otherParam) override;

private:
	bool filterTarget(NS::int32 nMapObjId);
};