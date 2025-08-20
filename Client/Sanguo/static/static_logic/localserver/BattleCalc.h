/*!
* \file BattleCalc.h
*
* \author zhichenxia
* \date 2018/6/23
*
*
*/
#include "data/role/Monsters.h"

#include "common/ClientSpec.h"
#include "common/CharacterAttr.h"
#include "common/SkillEffect.h"
#include "common/SkillRoleValue.h"
//#include "common/EffectEnum.h"
//#include "../handler/HandlerRole.h"


class AttrVector : public CharacterAttrVectorBase
{
public:
	int32  operator[] (int index) const { return m_arr[index]; }
	int32& operator[] (int index)       { return m_arr[index]; }

private:
	int32 m_arr[ATTR_NUM];
};

class CharacterAttributeSnapshot;

class MapCharacter
{
public:
	MapCharacter(const CharacterAttributeSnapshot& attrSnapshot, int nMaxHp, int nMaxMp, MapObj::Category eCategory, int nRare)
		: m_eCategory(eCategory)
		, m_nRare(nRare)
	{
		//m_base[AttrVector::ATTR_HPM          ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_HPM_BASE          ) * 1000.0f);
		//m_base[AttrVector::ATTR_MPM          ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_MPM_BASE          ) * 1000.0f);
		m_base[AttrVector::ATTR_HPM          ] = static_cast<int32>(nMaxHp);
		m_base[AttrVector::ATTR_MPM          ] = static_cast<int32>(nMaxMp);
		m_base[AttrVector::ATTR_HPR          ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_HPR_BASE          ) * 1000.0f);
		m_base[AttrVector::ATTR_HPRS         ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_HPRS_BASE         ) * 1000.0f);
		m_base[AttrVector::ATTR_MPR          ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_MPR_BASE          ) * 1000.0f);
		m_base[AttrVector::ATTR_MPRS         ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_MPRS_BASE         ) * 1000.0f);
		m_base[AttrVector::ATTR_AD           ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_AD_BASE           ) * 1000.0f);
		m_base[AttrVector::ATTR_AP           ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_AP_BASE           ) * 1000.0f);
		m_base[AttrVector::ATTR_ARM          ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_ARM_BASE          ) * 1000.0f);
		m_base[AttrVector::ATTR_MARM         ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_MARM_BASE         ) * 1000.0f);
		m_base[AttrVector::ATTR_AR           ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_AR_BASE           ) * 1000.0f);
		m_base[AttrVector::ATTR_MR_FIRE      ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_MR_FIRE_BASE      ) * 1000.0f);
		m_base[AttrVector::ATTR_MR_FORST     ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_MR_FORST_BASE     ) * 1000.0f);
		m_base[AttrVector::ATTR_MR_THUNDER   ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_MR_THUNDER_BASE   ) * 1000.0f);
		m_base[AttrVector::ATTR_MR_EARTH     ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_MR_EARTH_BASE     ) * 1000.0f);
		m_base[AttrVector::ATTR_MR_POSION    ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_MR_POSION_BASE    ) * 1000.0f);
		m_base[AttrVector::ATTR_MR_LIGHT     ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_MR_LIGHT_BASE     ) * 1000.0f);
		m_base[AttrVector::ATTR_MR_DARK      ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_MR_DARK_BASE      ) * 1000.0f);
		m_base[AttrVector::ATTR_ARP          ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_ARP_BASE          ) * 1000.0f);
		m_base[AttrVector::ATTR_MRP          ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_MRP_BASE          ) * 1000.0f);
		m_base[AttrVector::ATTR_CRI          ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_CRI_BASE          ) * 1000.0f);
		m_base[AttrVector::ATTR_MCRI         ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_MCRI_BASE         ) * 1000.0f);
		m_base[AttrVector::ATTR_CRID         ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_CRID_BASE         ) * 1000.0f);
		m_base[AttrVector::ATTR_MCRID        ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_MCRID_BASE        ) * 1000.0f);
		m_base[AttrVector::ATTR_RES          ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_RES_BASE          ) * 1000.0f);
		m_base[AttrVector::ATTR_MRES         ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_MRES_BASE         ) * 1000.0f);
		m_base[AttrVector::ATTR_CC_HIT       ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_CC_HIT_BASE       ) * 1000.0f);
		m_base[AttrVector::ATTR_CC_RES       ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_CC_RES_BASE       ) * 1000.0f);
		m_base[AttrVector::ATTR_ATK_SP       ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_ATK_SP_BASE       ) * 1000.0f);
		m_base[AttrVector::ATTR_MV_SP        ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_MV_SP_BASE        ) * 1000.0f);
		m_base[AttrVector::ATTR_HB           ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_HB_BASE           ) * 1000.0f);
		m_base[AttrVector::ATTR_GATHER_ATK   ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_GATHER_ATK_BASE   ) * 1000.0f);
		m_base[AttrVector::ATTR_GATHER_SP    ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_GATHER_SP_BASE    ) * 1000.0f);
		m_base[AttrVector::ATTR_JUMP         ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_JUMP_BASE         ) * 1000.0f);
		m_base[AttrVector::ATTR_SWIM         ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_SWIM_BASE         ) * 1000.0f);
		m_base[AttrVector::ATTR_BOSS_HURT    ] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_BOSS_HURT_BASE    ) * 1000.0f);
		m_base[AttrVector::ATTR_BOSS_DMGREDUC] = static_cast<int32>(attrSnapshot.getBaseAttrByRef   (ATTR_REF_SELF_BOSS_DMGREDUC_BASE) * 1000.0f);

		//m_now [AttrVector::ATTR_HPM          ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_HPM_NOW           ) * 1000.0f);
		//m_now [AttrVector::ATTR_MPM          ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_MPM_NOW           ) * 1000.0f);
		m_now [AttrVector::ATTR_HPM          ] = static_cast<int32>(nMaxHp);
		m_now [AttrVector::ATTR_MPM          ] = static_cast<int32>(nMaxMp);
		m_now [AttrVector::ATTR_HPR          ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_HPR_NOW           ) * 1000.0f);
		m_now [AttrVector::ATTR_HPRS         ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_HPRS_NOW          ) * 1000.0f);
		m_now [AttrVector::ATTR_MPR          ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_MPR_NOW           ) * 1000.0f);
		m_now [AttrVector::ATTR_MPRS         ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_MPRS_NOW          ) * 1000.0f);
		m_now [AttrVector::ATTR_AD           ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_AD_NOW            ) * 1000.0f);
		m_now [AttrVector::ATTR_AP           ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_AP_NOW            ) * 1000.0f);
		m_now [AttrVector::ATTR_ARM          ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_ARM_NOW           ) * 1000.0f);
		m_now [AttrVector::ATTR_MARM         ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_MARM_NOW          ) * 1000.0f);
		m_now [AttrVector::ATTR_AR           ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_AR_NOW            ) * 1000.0f);
		m_now [AttrVector::ATTR_MR_FIRE      ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_MR_FIRE_NOW       ) * 1000.0f);
		m_now [AttrVector::ATTR_MR_FORST     ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_MR_FORST_NOW      ) * 1000.0f);
		m_now [AttrVector::ATTR_MR_THUNDER   ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_MR_THUNDER_NOW    ) * 1000.0f);
		m_now [AttrVector::ATTR_MR_EARTH     ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_MR_EARTH_NOW      ) * 1000.0f);
		m_now [AttrVector::ATTR_MR_POSION    ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_MR_POSION_NOW     ) * 1000.0f);
		m_now [AttrVector::ATTR_MR_LIGHT     ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_MR_LIGHT_NOW      ) * 1000.0f);
		m_now [AttrVector::ATTR_MR_DARK      ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_MR_DARK_NOW       ) * 1000.0f);
		m_now [AttrVector::ATTR_ARP          ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_ARP_NOW           ) * 1000.0f);
		m_now [AttrVector::ATTR_MRP          ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_MRP_NOW           ) * 1000.0f);
		m_now [AttrVector::ATTR_CRI          ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_CRI_NOW           ) * 1000.0f);
		m_now [AttrVector::ATTR_MCRI         ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_MCRI_NOW          ) * 1000.0f);
		m_now [AttrVector::ATTR_CRID         ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_CRID_NOW          ) * 1000.0f);
		m_now [AttrVector::ATTR_MCRID        ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_MCRID_NOW         ) * 1000.0f);
		m_now [AttrVector::ATTR_RES          ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_RES_NOW           ) * 1000.0f);
		m_now [AttrVector::ATTR_MRES         ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_MRES_NOW          ) * 1000.0f);
		m_now [AttrVector::ATTR_CC_HIT       ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_CC_HIT_NOW        ) * 1000.0f);
		m_now [AttrVector::ATTR_CC_RES       ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_CC_RES_NOW        ) * 1000.0f);
		m_now [AttrVector::ATTR_ATK_SP       ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_ATK_SP_NOW        ) * 1000.0f);
		m_now [AttrVector::ATTR_MV_SP        ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_MV_SP_NOW         ) * 1000.0f);
		m_now [AttrVector::ATTR_HB           ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_HB_NOW            ) * 1000.0f);
		m_now [AttrVector::ATTR_GATHER_ATK   ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_GATHER_ATK_NOW    ) * 1000.0f);
		m_now [AttrVector::ATTR_GATHER_SP    ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_GATHER_SP_NOW     ) * 1000.0f);
		m_now [AttrVector::ATTR_JUMP         ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_JUMP_NOW          ) * 1000.0f);
		m_now [AttrVector::ATTR_SWIM         ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_SWIM_NOW          ) * 1000.0f);
		m_now [AttrVector::ATTR_BOSS_HURT    ] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_BOSS_HURT_NOW     ) * 1000.0f);
		m_now [AttrVector::ATTR_BOSS_DMGREDUC] = static_cast<int32>(attrSnapshot.getCurrentAttrByRef(ATTR_REF_SELF_BOSS_DMGREDUC_NOW ) * 1000.0f);
	}

	MOB_RARE GetMobRare(void) const
	{
		if (m_eCategory == MapObj::Category::Monster)
		{
			switch (static_cast<MonsterCfg::MonsterTypeRare>(m_nRare))
			{
			case MonsterCfg::MonsterTypeRare::NORMAL:
				return MOB_RARE::MOB_RARE_ORDINARY;

			case MonsterCfg::MonsterTypeRare::ELITE:
				return MOB_RARE::MOB_RARE_ELITE;

			case MonsterCfg::MonsterTypeRare::BOSS:
				return MOB_RARE::MOB_RARE_BOSS;

			default:
				break;
			}
		}
		return MOB_RARE::MOB_RARE_NONE;
	}

	MAP_OBJECT_TYPE GetObjectType(void) const
	{
		switch (m_eCategory)
		{
		case MapObj::Category::Player:
			return MAP_OBJECT_TYPE::OBJECT_PLAYER;
			break;

		case MapObj::Category::Monster:
			return MAP_OBJECT_TYPE::OBJECT_MOB;
			break;

		case MapObj::Category::Npc:
			return MAP_OBJECT_TYPE::OBJECT_NPC;
			break;

		case MapObj::Category::TowerBuilding:
			return MAP_OBJECT_TYPE::OBJECT_BUILDTOWER;
			break;

		case MapObj::Category::InteractiveBuilding:
			return MAP_OBJECT_TYPE::OBJECT_BUILDACTIVE;
			break;

		case MapObj::Category::Vehicle:
			return MAP_OBJECT_TYPE::OBJECT_VEHICLE;
			break;

		case MapObj::Category::Pet:
			return MAP_OBJECT_TYPE::OBJECT_PET;
			break;

		default:
			return MAP_OBJECT_TYPE::OBJECT_NONE;
		}
	}

	const MapCharacter& GetCharacterAttr(void) const { return *this; }

	      AttrVector& GetBase(void)       { return m_base; }
	const AttrVector& GetBase(void) const { return m_base; }
	      AttrVector& GetAttr(void)       { return m_now ; }
	const AttrVector& GetAttr(void) const { return m_now ; }

	int32 GetHP(void) const
	{
		return GetAttr()[AttrVector::ATTR_HPM];
	}

	int32 GetMP(void) const
	{
		return GetAttr()[AttrVector::ATTR_MPM];
	}

	template <typename T>
	void GetSkillRoleValue(const T&)
	{}

private:
	MapObj::Category m_eCategory;
	int m_nRare;

	AttrVector m_base;
	AttrVector m_now;
};


typedef TSkillRoleValue<MapCharacter, MapCharacter, AttrVector> SkillRoleValue;

typedef skill::TSkillEffectParam<MapCharacter, MapCharacter, AttrVector> SkillEffectParam;
//typedef skill::TSkillEffect<SkillRoleValue, MapCharacter, AttrVector> SkillEffect;


// 物理伤害
class SkillEffect_WeaponDmg : public skill::TSkillEffect_WeaponDmg<SkillRoleValue, SkillEffectParam, MapCharacter>
{
	// 取得目標的免復值
	virtual int GetTargetElemetRemitValue(const SkillRoleValue& TargetRoleValue) override
	{
		return TargetRoleValue.NowAR;
	}
};


// 法术伤害
class SkillEffect_ElemtDmg : public skill::TSkillEffect_ElemtDmg<SkillRoleValue, SkillEffectParam, MapCharacter>
{
public:
	SkillEffect_ElemtDmg(int32 skill_id) : m_skill_id(skill_id) {}

private:
	// 取得目標的免復值
	virtual int GetTargetElemetRemitValue(const SkillRoleValue& TargetRoleValue) override;

private:
	int32 m_skill_id;
};


// 回复（包括 HP 和 MP）。
class SkillEffect_Restore : public skill::TSkillEffect_Restore<SkillRoleValue, SkillEffectParam, MapCharacter> {};


// 属性伤害（用于BUFF）。
class SkillEffect_BuffDmg : public skill::TSkillEffect_ElemtDmg<SkillRoleValue, SkillEffectParam, MapCharacter>
{
public:
	SkillEffect_BuffDmg(BUFF_EFFECT_TYPE type) : m_type(type) {}

private:
	// 取得目標的免復值
	virtual int GetTargetElemetRemitValue(const SkillRoleValue& TargetRoleValue) override
	{
		switch (m_type)
		{
		case BUFF_EFFECT_REAL_DOT:	  return TargetRoleValue.NowAR;
		case BUFF_EFFECT_FIRE_DOT:	  return TargetRoleValue.NowMR_FIRE;
		case BUFF_EFFECT_FORST_DOT:	  return TargetRoleValue.NowMR_FORST;
		case BUFF_EFFECT_THUNDER_DOT: return TargetRoleValue.NowMR_THUNDER;
		case BUFF_EFFECT_POSION_DOT:  return TargetRoleValue.NowMR_POSION;
		case BUFF_EFFECT_LIGHT_DOT:	  return TargetRoleValue.NowMR_LIGHT;
		case BUFF_EFFECT_DARK_DOT:	  return TargetRoleValue.NowMR_DARK;
		case BUFF_EFFECT_OXY_DOT:	  return 0;
		case BUFF_EFFECT_WEAPON_DOT:  return TargetRoleValue.NowAR;

		case BUFF_EFFECT_HP_RESTORE:
		case BUFF_EFFECT_MP_RESTORE:
		case BUFF_EFFECT_ROLE_STATE:
		default:                      return 0;
		}
	}

private:
	BUFF_EFFECT_TYPE m_type;
};

//可抵消该效果的护盾类型
class SkillEffect_BuffDurability
{
public:
	static BUFF_EFFECT_TYPE GetTypeBySkillEffectType(int skillEffectType)
	{
		switch (skillEffectType)
		{
		case SKILL_EFFECT_DMG_REAL:
			return BUFF_EFFECT_REAL_SHIELD;
			break;
		case SKILL_EFFECT_DMG_WEAPON:
			return BUFF_EFFECT_WEAPON_SHIELD;
			break;
		case SKILL_EFFECT_DMG_FIRE:
			return BUFF_EFFECT_FIRE_SHIELD;
			break;
		case SKILL_EFFECT_DMG_FORST:
			return BUFF_EFFECT_FORST_SHIELD;
			break;
		case SKILL_EFFECT_DMG_THUNDER:
			return BUFF_EFFECT_THUNDER_SHIELD;
			break;
		case SKILL_EFFECT_DMG_EARTH:
			return BUFF_EFFECT_EARTH_SHIELD;
			break;
		case SKILL_EFFECT_DMG_POSION:
			return BUFF_EFFECT_POSION_SHIELD;
			break;
		case SKILL_EFFECT_DMG_LIGHT:
			return BUFF_EFFECT_LIGHT_SHIELD;
			break;
		case SKILL_EFFECT_DMG_DARK:
			return BUFF_EFFECT_DARK_SHIELD;
			break;
		default:
			return BUFF_EFFECT_SHIELD;
			break;
		}
	}
	static BUFF_EFFECT_TYPE GetTypeByBuffEffectType(int buffEffectType)
	{
		switch (buffEffectType)
		{
		case BUFF_EFFECT_REAL_DOT:
			return BUFF_EFFECT_REAL_SHIELD;
			break;
		case BUFF_EFFECT_FIRE_DOT:
			return BUFF_EFFECT_FIRE_SHIELD;
			break;
		case BUFF_EFFECT_FORST_DOT:
			return BUFF_EFFECT_FORST_SHIELD;
			break;
		case BUFF_EFFECT_THUNDER_DOT:
			return BUFF_EFFECT_THUNDER_SHIELD;
			break;
		case BUFF_EFFECT_EARTH_DOT:
			return BUFF_EFFECT_EARTH_SHIELD;
			break;
		case BUFF_EFFECT_POSION_DOT:
			return BUFF_EFFECT_POSION_SHIELD;
			break;
		case BUFF_EFFECT_LIGHT_DOT:
			return BUFF_EFFECT_LIGHT_SHIELD;
			break;
		case BUFF_EFFECT_WEAPON_DOT:
			return BUFF_EFFECT_WEAPON_SHIELD;
			break;
		case BUFF_EFFECT_DARK_DOT:
			return BUFF_EFFECT_DARK_SHIELD;
			break;
		default:
			return BUFF_EFFECT_SHIELD;
			break;
		}
	}
};

