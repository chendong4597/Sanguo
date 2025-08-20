/*!
* \file LocalServer_Battle.h
*
* \author zhichenxia
* \date 2018/5/16
*
*
*/
#pragma once
#include <ctime>
#include <chrono>
#include "LocalServer.h"
#include <common/Timer.h>
#include <msg/MsgSkill.pb.h>
#include <unordered_map>
#include <config/SkillCfg.h>
#include "data/role/Role.h"
//#include <build/DropDataDef.h>


namespace MsgPB
{
	class MsgPlayerNormalAttackC2S;
	class MsgSkillCastSkillC2GS;
	class MsgPlayerInfoGS2C;
	//class MsgPetNormalAttackC2S;
}

class Player;
class Role;
class MapObj;
class SkillData;

class LocalServer_Battle_NormalAttack : public NS::Event
{
public:

	std::list<bzbee::uint32> m_lstTargetMapObjID;
};

class LocalServer_Battle_BeAttacked : public NS::Event
{
public:
	bzbee::int32 m_CasterMapObjID;
	bzbee::int32 m_TargetMapObjID;
};

class LocalServer_Battle_Clock : public NS::Event
{
public:
	NS::int32 m_clock;
};

NS_BEGIN

class LocalServer_Battle : public LocalServer
{
	RTTI_DEF(LocalServer_Battle);
	DECLARE_TIMER_HOST();

	LocalServer_Battle(void);

public:
	static LocalServer_Battle& getInstance(void);

	enum EN_USER_BATTLESTATE
	{
		USER_BATTLESTATE_NONE = 0,
		USER_BATTLESTATE_IN = 1,		// 进入战斗
		USER_BATTLESTATE_OUT = 2,		// 脱离战斗
	};

private:
	// 对应三种cast方式
	enum class CastSkillType : char {
		Target,
		Targets,
		Pos,
	};

	enum class HurtSource : char
	{
		Unknow = 0,
		Normal_Attack,
		Skill,
	};

	// 技能施放前摇完的后处理信息
	typedef struct CastingSkillInfo {
		int32 id;
		int32 caster;
		NS::I64 serialId;
		float delay;
		CastSkillType type;
		union {
			int32 target;
			glm::vec2 pos;
			std::vector<int32> targets;
		};


		CastingSkillInfo(int32 _id, int32 _caster, NS::I64 _serialId, float _delay, int _target)
			:CastingSkillInfo(_id, _caster, _serialId, _delay, CastSkillType::Target)
		{
			target = _target;
		}
		CastingSkillInfo(int32 _id, int32 _caster, NS::I64 _serialId, float _delay, const std::vector<int32> _targets)
			:CastingSkillInfo(_id, _caster, _serialId, _delay, CastSkillType::Targets)
		{
			new(&targets) std::vector<int32>(_targets);
		}
		CastingSkillInfo(int32 _id, int32 _caster, NS::I64 _serialId, float _delay, glm::vec2 _pos)
			:CastingSkillInfo(_id, _caster, _serialId, _delay, CastSkillType::Pos)
		{
			pos = _pos;
		}

		CastingSkillInfo(const CastingSkillInfo& other)
			:CastingSkillInfo(other.id, other.caster, other.serialId, other.delay, other.type)
		{
			switch (type)
			{
				case CastSkillType::Target:
				{
					target = other.target;
					break;
				}
				case CastSkillType::Targets:
				{
					new(&targets) std::vector<int32>(other.targets);
					break;
				}
				case CastSkillType::Pos:
				{
					pos = other.pos;
					break;
				}
			}
		}

		~CastingSkillInfo()
		{
			if (type == CastSkillType::Targets)
			{
				targets.~vector<int32>();
			}
		}

	private:
		CastingSkillInfo(int32 _id, int32 _caster, NS::I64 _serialId, float _delay, CastSkillType _type)
			: id(_id)
			, caster(_caster)
			, serialId(_serialId)
			, delay(_delay)
			, type(_type) {}
	}CastingSkillInfo;

public:
	virtual void update(float dt) override;

	void fight(int32 caster_id, int32 target_id);
	void multiFight(int32 caster_id, const std::vector<int32>& target_ids);

	NS::I64 easy_cast_skill(int32 skill_id, int32 caster_id, int32 target_id, const glm::vec2& target_pos);

	NS::I64 cast_skill(int32 skill_id, int32 caster_id, int32 target_id);
	NS::I64 cast_skill(int32 skill_id, int32 caster_id, const std::vector<int32>& target_ids);
	NS::I64 cast_skill(int32 skill_id, int32 caster_id, glm::vec2 target_position);
	//void cast_skill(const LuaTable& table);

	bool CheckAttackTarget(int32 caster_id, int32 target_id, int SkillID);
	void PetFight(NS::I64 idHP, int32 caster_id, int32 target_id, int32 serialid);

	void kill(int32 target_id, int32 caster_id, bool isTargetDead);

	//void onPhysicDamage(int32 caster_id, int32 target_id, int32 dmgValue);	

	NS::I64 getCurSerialId();

	void countAttack(int32 nSkillId, int32 nCaster_id, int32 nTarget_id, int32 nDamageValue);
	void suckingBlood(int32 nSkillId, int32 nCaster_id, int32 nTarget_id, int32 nDamageValue);

public:
	void skillDamage(NS::I64 serialId, int32 skill_id, int32 caster_id, int32 target_id);
	void skillDamage(NS::I64 serialId, int32 skill_id, int32 caster_id, int32 target_id, int32 index);
	
	void skillDamageEarth(NS::I64 serialId, int32 skill_id, int32 caster_id, const glm::vec3& target_pos);
	void skillDamagePlant(NS::I64 serialId, int32 skill_id, int32 caster_id, const glm::vec3& target_pos);

	void skillPutBlock(const glm::ivec2& coordPos, int32 layer, int32 templateid);
	bool skillPutBuild(const glm::ivec2& coordPos, int32 templateid);

	void skillBuff(int32 skill_id, int32 caster_id, int32 target_id = 0);
	void skillBuffOnlySelf(int32 skill_id, int32 caster_id);
	void skillBuffRemove(int32 skill_id, int32 caster_id, int32 target_id = 0);

	void addExp(const int32 nAddExp);

	void summonRemove(int32 casterId, int32 summonId);
	void casterRemove(int32 casterId);
	void DieWithCaster(int32 casterId);
	int summonCount(int32 caster_id, int32 skill_id);
	void KillSelf(int32 nMapObjID);

	void mapRestore();
	void mapResume();

	int32 findNearestSummon(int32 casterId, const glm::vec3& pos, int32 mobCfgId = -1); //寻找目标点最近的召唤物（指定）
	// 判决是否满足技能的判断逻辑
	bool IsSkillTargetLogic(std::shared_ptr<Role> casterData, const SkillType* skillType, Role* pRole);
	//筛选施法作用对象目标
	bool filterSkillCastFunc(std::shared_ptr<Role> casterData, const SkillType* skillType, MapObj* obj);
	void getSaveSummonMobs(int nCasterMapObjId, std::unordered_multimap<int, int>& result);
	void restoreSaveSummonMob(int nCasterMapObjId, int nSkillId, int nSummonMapObjId);
	void castSummonEffect(int32 caster_id, int32 skill_id, NS::I64 serial_id, int effectIndex, std::vector<glm::vec3>* posList, std::vector<int32>* birthMapobjIdList);
	// 是否战斗状态
	bool inBattle() const { return m_eBattleState == EN_USER_BATTLESTATE::USER_BATTLESTATE_IN; }

private:
	void skillEffectDamage(std::shared_ptr<SkillData> pSkillData, const SkillType* pSkillType, int32 nCasterId, const std::shared_ptr<Role>& pTargetData, int32 index);

	void resumePlayerSummons();
	void restorePlayerSummons();

	void onMsgPlayerNormalAttackC2S(const MsgPB::MsgPlayerNormalAttackC2S& msg);
	void onMsgSkillCastSkillC2GS   (const MsgPB::MsgSkillCastSkillC2GS   & msg);
	//void onMsgPetNormalAtk(const MsgPB::MsgPetNormalAttackC2S& msg);

	void skillEffect(const CastingSkillInfo& info);
	void skillSummonEffect(const CastingSkillInfo& info, int effectIndex = -1, std::vector<glm::vec3>* posList = nullptr, std::vector<int32>* birthMapobjIdList = nullptr);

	bool ProcessFight(const std::shared_ptr<Role> pCaster, const std::shared_ptr<Role> pTarget, int& dmgValue, bool& bCritical, NS::I64 serialId, int& durability);

	bool filterTargetCategory(const SkillType::TargetCategory category, const MapObj* pTargetObj);

private:
	// add exp
	void killMobAddExp(int32 target_id, int32 caster_id = -1);
	// level up
	bool levelUp(NS::I64 exp, MsgPB::MsgPlayerInfoGS2C& msg, const std::shared_ptr<Player> data);
	// do drop
	//std::vector<DropObjInfo> DoDrop(int32 target_id, int32 drop_id = -1, int32 caster_id = -1);
	//筛选技能效果目标
	bool filterSkillTargetFunc(NS::int32 nCasterId, NS::int32 nCasterGroupId, std::shared_ptr<Role> targetData, const SkillType* skillType, const SkillType::Effect* effect, MapObj* obj);
	// 受击减少装备耐久
	void reduceEndurance(int32 target_id);

	// 减伤效果
	void dmgReduceEffect(int32 caster_id, int32 target_id, int& dmgValue, float hurtReduce, int effect_type, NS::I64 serialId, int32 skill_id, int & durability);
	// 护盾吸收伤害
	void absorbDamage(int target_id, int& dmgValue, int effect_type, int& durability);
	// 受击后触发
	void onAfterDamage(int32 nSkillId, int32 caster_id, int32 target_id, int dmgValue, int effect_type, NS::I64 serialId, HurtSource eHurtSource = HurtSource::Unknow);
	// 受击buff效果触发
	void buffEffectAfterDmg(const std::shared_ptr<Role> casterData, const std::shared_ptr<Role> targetData, int& dmgValue, int effect_type, NS::I64 serialId);
	// 受击前触发被动技能
	void onBeforeDamage(int32 nCasterGroupId, int32 target_id, float& hurtReduce, NS::I64 serialId);
	// 击退
	void doBeHitFly(int32 caster_id, int32 target_id, NS::I64 serialId, bool isFinalBlow = false);
	// 检查与目标间是否有阻挡
	bool checkCanHit(const MsgPB::MsgSkillCastSkillC2GS& msg, const SkillType* pSkillType);
	// 被动技能伤害效果消息
	void sendPassiveEffectDmg(int32 nSkillId, int32 nCasterId, int32 nTargetId, int nCurHp, int nMaxHp, int nDamageValue);

private:
	std::chrono::steady_clock::time_point m_nLastBattleTime{};
	EN_USER_BATTLESTATE m_eBattleState{ EN_USER_BATTLESTATE::USER_BATTLESTATE_NONE };

	int32 m_lastClock{ -1 };

	std::list<CastingSkillInfo> m_castingSkills;

	std::unordered_map<int, std::unordered_multimap<int, int>> m_summonMobs;
	std::unordered_map<int, std::unordered_multimap<int, int>> m_saveSummonMobs;

	struct PlayerStoreSummonInfo {
		int skillId;
		int defId;
		int hp;
		int mp;
		int lifeTime;
		Role::BuffVec buffs;
	};
	std::vector<PlayerStoreSummonInfo> m_playerStoreSummons;	

	NS::I64 s_nSerialId;
};

NS_END
