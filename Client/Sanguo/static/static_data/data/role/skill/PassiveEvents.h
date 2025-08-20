/*!
 * \file PassiveEvent.h
 *
 * \author Dexuan Yang
 * \date 2019/10/04
 *
 */

#pragma once

#include <Cpp11EventSystem.h>

class PetComponent;
//class LocalServer_Battle_Clock;
class MovementComponentEvent;
struct PassiveEvent_PetChange;
class TransformSetPositionEvent;
class HpEvent;

// BeAtk Event
struct PassiveEvent_BeAtk : public NS::Event
{
	NS::int32 m_nHitMapObjId;	// 受击者ID
	NS::int32 m_nAtkMapObjId;	// 攻击者ID
	NS::int32 m_nDamageValue;	// 伤害值
	bool m_bIsCritical;			// 是否暴击
	NS::int32 m_nHurtType;		// 伤害类型，武器伤害 / 火元素伤害 / 冰元素伤害 ...
	NS::int32 m_nSkillId;		// 伤害来源技能ID，普攻填0
};

// Atk Event
struct PassiveEvent_Attack : public NS::Event
{
	NS::int32 m_nAtkMapObjId;					// 攻击者ID
	std::vector<NS::int32> m_vecHitMapObjIds;	// 受击者ID
	NS::int32 m_nSkillId;						// 来源技能ID，普攻填0
};

// Atk hit Event
struct PassiveEvent_Hit : public NS::Event
{
	NS::int32 m_nAtkMapObjId;	// 攻击者ID
	NS::int32 m_nHitMapObjId;	// 受击者ID
	NS::int32 m_nDamageValue;	// 伤害值
	bool m_bIsCritical;			// 是否暴击
	NS::int32 m_nHurtType;		// 伤害类型，武器伤害 / 火元素伤害 / 冰元素伤害 ...
	NS::int32 m_nSkillId;		// 伤害来源技能ID，普攻填0
};

struct PassiveEvent_KillSomeone : public NS::Event
{
public:
	bzbee::int32 m_nAtkMapObjId;
	bzbee::int32 m_nHitMapObjId;
};

// Pet follow change event
struct PassiveEvent_Follow_NpcOrPetChange : public NS::Event
{
	NS::int32 m_npcMapObjId{ -1 };
	bool m_bNpcChange{ false };
	NS::int32 m_petMapObjId{ -1 };
	bool m_bPetChange{ false };
};

// Role get in water or get out water.
struct PassiveEvent_WaterInOut : public NS::Event
{
	enum class State : int
	{
		WATER_IN = 1,
		WATER_OUT = 2,
		BREATH_IN = 3,
		BREATH_OUT = 4,
	};

	PassiveEvent_WaterInOut::State m_eState;
};

// Role get in or get out sight
struct PassiveEvent_RoleInOutSight : public NS::Event
{
	enum class State : int
	{
		GET_INTO = 1,
		GET_OUT = 2,
	};

	PassiveEvent_RoleInOutSight::State m_eState;
	std::vector<NS::int32> m_vecMapObjIds;
};

union OtherParam
{
	//const LocalServer_Battle_Clock* m_pClockData;
	//const MovementComponentEvent* m_pMovementData;
	const PassiveEvent_Follow_NpcOrPetChange* m_pFollowNpcOrPetData;
	const PassiveEvent_BeAtk* m_pBeAtkEvent;
	const PassiveEvent_Attack* m_pAtkEvent;
	const PassiveEvent_Hit* m_pHitEvent;
	const PassiveEvent_WaterInOut* m_pWaterInOutState;
	//const TransformSetPositionEvent* m_pPosChangeEvent;
	//const HpEvent* m_pHpEvent;
	const PassiveEvent_KillSomeone* m_pKillEvent;
	const PassiveEvent_RoleInOutSight* m_pInOutSight;
};
