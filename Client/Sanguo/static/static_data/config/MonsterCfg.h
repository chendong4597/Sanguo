/*!
* \file MonsterCfg.h
*
* \author cd
* \date 12/01/2016
*
*
*/
#pragma once

//#include <baseutil.h>
#include "BaseTypeDefine.h"
#include <map>
#include <memory>
#include <unordered_map>
#include "tinyxml2.h"
#include "../common/EffectEnum.h"

using namespace NS;

enum RoleCamp
{
	mid = 0,
	friendler,
	enemy,
	animals,
	//allied,
};

enum MonsterFuncType
{
	commonMob = 0,	//普通怪;
	animal,			//小动物;
	battleNpc,		//战斗NPC;
	flowerNpc,		//园丁NPC;
	artisan,		//合成台NPC;
	forging,		//锻造台NPC;
	newGuide,		//新手NPC;
	commonNpc,		//普通NPC;
	dakNpc,			//驿站NPC
	caravanNpc,		//商队NPC
	TeleportNpc,	//传送NPC
	cavNpc,			//矿洞NPC;

	chainMonster = 12, // 链式组合怪物
};

enum pvpTowerGroupType
{
	atter = 0, //进攻方;
	defer = 1, //防守方;
	midGroup = 2,  //中立;
};

//怪物出生朝向
enum MonsterBoneDir
{
	LEFT = 1,		//左
	RIGHT = 2,		//右
};

class MonsterCfg
{
public:
	static const int MIN_CHAIN_COUNT = 3;	// 链式组合怪物最小节数

	enum class MonsterTypeRare : int
	{
		NORMAL = 1,
		UNKONWN1 = 2,
		ELITE = 3,
		UNKONWN2 = 4,
		BOSS = 5
	};

	enum class MonsterTypeKind : int
	{
		MONSTER = 0,
		ANIMAL = 1,
		NPC = 2
	};

	enum class MonsterCfgType
	{
		MonsterCfgMob,//普通怪物数据
		MonsterCfgBadgeNpc,//背包NPC
		MonsterCfgSoul,//魂匣
	};

	enum class SkillConType
	{
		SkillConType_HP = 1,//血量低时触发
	};

	enum Monster_AI_Type
	{
		PatrolBeAttackEscape = 1,  // 来回巡逻被攻击逃跑				暂未实现  目前是使用 主动攻击怪物AI
		PatrolAttack = 2,		   // 来回巡逻主动攻击					暂未实现  目前是使用 主动攻击怪物AI
		PatrolBeAttack = 3,		   // 来回巡逻被动攻击					LoopBeAttack.lua
		AttackTowerBuild = 4,	   // 全地图搜塔防装置的AI怪物   		暂未实现  目前是使用 主动攻击怪物AI
		Static = 5,				   // 静止不动							Static.lua
		InvadeDistancePriority = 6,// 往水晶塔移动，距离近的优先攻击	Invade.lua
		InvadeMobTypePriority = 7, // 往水晶塔移动，按配置优先级攻击	Invade.lua
		FlyPatrolAttack = 8,	   // 飞行巡逻主动攻击					暂未实现  目前是使用 主动攻击怪物AI
		FlyPatrolBeAttack = 9,	   // 飞行巡逻被动攻击				    暂未实现  目前是使用 主动攻击怪物AI
		BrokenWall = 10,		   // 往水晶塔移动破墙主动攻击          BrokenWall.lua
		Mole = 11,				   // 鼹鼠AI					        Mole.lua
		MoleBoss = 12,			   // 鼹鼠Boss AI					    MoleBoss.lua
		TreeMan = 13,			   // 树人 AI						    TreeMan.lua
	};

	class MonsterType
	{
	public:
		enum { MoveTypeNone = 0, MoveTypeNormal = 1, MoveTypeFly = 2 };

		enum { MobTowerTypeNone = 0, MobTowerTypeCrystal = 1, MobTowerTypeDefTower = 2 };

		int n_ID{};
		std::string s_name;
		std::string s_name_untrans;
		std::string s_title;
		std::string s_title_untrans;
		std::string s_features_des;
		std::string s_features_des_untrans;
		int n_PatrolRange{};
		int n_FollowRange{};
		int n_HPM{};
		int n_MPM{};
		int n_AD{};				     // 物理攻擊
		int n_AP{};				     // 魔法攻擊
		int n_ARM{};			     // 物理防禦
		int n_MARM{};			     // 魔法防禦
		int n_AR{};                  // 物理減免	
		int n_MR_Fire{};             // 魔法減免 - 火
		int n_MR_Forst{};            // 魔法減免 - 冰
		int n_MR_Thunder{};          // 魔法減免 - 雷
		int n_MR_Earth{};            // 魔法減免 - 土
		int n_MR_Poison{};           // 魔法減免 - 毒
		int n_MR_Light{};            // 魔法減免 - 光
		int n_MR_Dark{};             // 魔法減免 - 暗
		float f_RES{};               // 物理韌性	
		float f_MRES{};              // 魔法韌性
		float f_atkRange{};	         // 攻击距离
		float f_CRI{};		         // 物理爆擊率
		float f_MCRI{};		         // 魔法爆擊率
		float f_ARP{};		         // 物理穿透
		float f_MRP{};		         // 魔法穿透
		float f_CRID{};		         // 物理爆擊傷害增加
		float f_MCRID{};	         // 魔法爆擊傷害增加
		int n_CC_Hit{};              // 控制命中
		int n_CC_Res{};              // 控制抵抗
		int n_HPR{};				 // 生命回復
		int n_HPRS{};				 // 生命回復CD
		int n_MPR{};				 // 魔力回復
		int n_MPRS{};				 // 魔力回復CD
		float f_atk_speed_modifiy{}; // 攻速修正
		float f_Move{};	             // 移動
		float f_Run{};				 // 追擊
		float f_scale{ 1.0f };       // 模型缩放
		float f_uiModelScale{ 1.0f }; // 界面模型缩放
		float f_Width{};			 // 宽度
		float f_Height{};			 // 高度
		float f_depth{};			 // z value
		int n_Jump{};				 // 跳跃
		int n_gethit_cd{};
		int n_noapplylight{ 0 };	 // 不受日夜影響;
		int n_hurtEffectId{};  //受击效果ID;
		int n_FOV{};			// 视野
		int n_is_capture{};			// 是否可捕捉
		int n_egg_id{ 0 };			// 宠物蛋
		int n_MobType{};			// 兵种类型
		float f_SpeakUiCheck{};		//怪物喊话高度
		int n_MobAI{};				// 怪物AI类型
		int n_AITypeMob{};			// 怪物ai脚本选择
		float f_CorpseTime{};         //尸体时间
		std::string s_hurtEffect{}; //受击粒子特效;
		std::vector<std::tuple<std::string/*animation*/, std::string/*effect*/>> m_atkAniEffect;
		std::unordered_map<std::string/*animation*/, std::string/*effect*/> m_animToUnderAttackEffectId;
		std::vector<std::tuple<std::string/*animation*/, std::string/*effect*/>> m_atkAniSound;
		std::string s_UnderAttackEffectID; // 攻击其他角色时的受击特效
		std::string s_model;      // 模型ID
		//std::string s_mob_attack_sound;
		std::string s_mob_dead_sound;
		using MaterialTextureVec = std::vector<std::tuple<bzbee::uint8/*material id*/, std::string/*texture*/>>;
		using EventTypeToMaterial = std::unordered_map<bzbee::uint8/*event id*/, MaterialTextureVec>;
		EventTypeToMaterial m_eventTypeToMaterial;
		MonsterTypeRare n_rare;			 // 品质
		MonsterTypeKind n_kind;			//种类
		float f_HpUICheck{};		//距离血条高度
		int n_hide_hp{};
		int n_MobLv{};				//等级
		NS::uint8 n_MoveType{ MoveTypeNormal };
		NS::uint8 n_CanAttack{ };  //是否能主动攻击
		NS::uint8 n_ForbidAtk{ };  //是否禁止被攻击
		NS::uint8 n_IsOnlyNormalAtk{ };  //是否只能被普攻伤害
		NS::uint8 n_nTowerTpy{ MobTowerTypeNone };  //0：正常怪 1：水晶  2：炮台
		bool      m_bSolider{ false };  //是否是士兵
		NS::uint8 n_through_wall{};
		std::string m_briefRes{};             //怪物缩略图
		NS::int32 n_shake_map_id{ 0 };			 // 品质
		std::vector<int> s_AttackObjectType; // 攻击对象类型优先级
		int n_Ignore_AttackObject{};			// 是否忽略优先级
		int n_catchBuffId{};
		int n_DisappearTime{};
		int n_type{};
		int n_mob_place{};  //来源
		int n_summon_effect{ 0 };//被召唤时是否淡入
		int n_isCanBeFly{ 0 };
		//int n_SoundBankType{ 0 };
		std::vector<int> s_SoundBankVec;
		bzbee::IntegerList speakList;
		int n_SpeakInterval{ 0 };
		int n_Camp{ 0 };
		int n_Camp_Change{ 0 };

		int n_LifeCycle;
		int n_life_disappear{ 1 };

		int n_Exp;
		int n_DropId;
		int n_DeadSkill;

		int n_mob_direction;
		bool n_ischange_direction;
		bool n_isRemotely{ false };
		int n_npc_voice_type{ 0 };
		int n_PlotDistance{ 0 };
		int n_PlotId{ 0 };
		int n_BornPlotID{};
		bool m_bIgnoreSpeakDistance{ false }; // 忽略喊话距离限制
		bool m_bIgnoreFlyElevate{ true }; // 忽略飞行自动抬高
		int n_repel_distance{ 0 }; // 怪物被击退距离
		bool m_bDieWithOwner{ false }; // 与主人共死

		//新加
		int n_FocusSpeed{};
		int n_PartalSpeed{};
		std::string s_SubFoder;
		std::string s_BPName;
		int n_AttackNum{};  //普通攻击动作个数
		int n_AttackSkill{};//普通攻击技能
		int n_AttackDis{};  //普通攻击需要距离
		int n_AtkSight{};
		int n_LoseSight{};
		int n_AtkAngle{};

		std::string s_atk_sound;
		int n_atk_tri{0};
		std::string s_hit_sound;
		int n_hit_tri{0};
		std::string s_dead_sound;
		int n_dead_tri{0};


		typedef struct SkillConInfo {
			int nSkillId{};		//触发技能ID
			int nAniIdx{};		//技能动作索引
			int nConTpy{};      //触发条件类型
			int nConValue{};	//触发值
			int nConRate{};		//触发概率
		}SKILL_CON;
		int n_AI{};
		std::string s_desc{};

		std::vector<SKILL_CON> vecSkillCon;
		std::string strTxtFonder;
		std::vector<std::string> vecTxtures;

	public:
		void getAttr(MonsterCfgType cfgType, AttrMap& mapData, int level) const;
	};

	typedef std::unordered_map<int, MonsterType> MonsterTypeMap;



public:
	static MonsterCfg& getInstance();

	bool initializeCfg();
	void terminate();

	const MonsterType* findMonsterType(int id) const;
	const MonsterTypeMap& getMonsterTypeMap() const { return m_monsterTypeMap; };

public:
	std::string GetBPMonsterPath(int id);
	std::string GetAttackPath(int id, int nIdx);
	std::string GetMagicAttackPath(int id, int nIdx);
	std::string GetDeathPath(int id);
	std::string GetHitPath(int id);
	std::string GetMonsterMatTxt(int id, std::string strTxt);

private:
	bool parseMonsterConfig(tinyxml2::XMLElement* pXmlRoot);

private:
	MonsterTypeMap m_monsterTypeMap;

	float m_spawnerSpaceScale;
	int m_recHpRare;
	int m_recHpTime;
	int m_srvDropRare{ 3 };

	int m_repelDistanceTime{ 500 }; // 击退表现时长
	int m_finalBlowCoefficient{ 1000 }; // 最后一击击退系数(千分比)

	//add cd
	std::string m_strPath;
	std::string m_strDeathName;
	std::string m_strHitName;
	std::string m_strAniName;
};
