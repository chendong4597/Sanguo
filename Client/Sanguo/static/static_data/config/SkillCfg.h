/*!
 * \file SkillCfg.h
 *
 * \author zhijieli
 * \date 2016/08/30
 *
 *
 */
#pragma once
#include "BaseTypeDefine.h"
#include <map>
#include <memory>
#include <unordered_map>
#include "tinyxml2.h"



class SkillCfg;
class SkillType
{
	friend class SkillCfg;

public:
	enum class Type : NS::uint8
	{
		InstantSkill				= 1,	// 1, 直接攻擊			
		BulletSkill					= 2,	// 2, 法球
		CurveBulletSkill			= 3,	// 3, 曲線法球
		AreaSkill					= 4,	// 4, 區域陷阱
		RaySkill					= 5,	// 5, 射線
		Group						= 6,	// 6, 组合技能(非指向性)召唤技能需要用这个
		ThunderInstantSkill			= 7,	// 7, 滚地雷爬坡直击	
		HorizontalSkill				= 8,    // 8, 横向技能（水平移动）
		ParabolaSkill				= 9,	// 9, 抛物线法球直击
		ThunderBulletSkill			= 10,	// 10.滚地雷法球
		TeleportSkill				= 11,	// 11.瞬移
		TeleportBulletSkill			= 12,	// 12.位移法球
		PreviewSkill				= 13,	// 13.技能预演
		RandomHeightBulletSkill		= 14,	// 14.随机高度直线坠落法球
		VerticalBulletSkill			= 15,	// 15.竖直向上 法球
											// 16.强制瞬移
											// 17.水平法球
											// 18.固定距离释放法球
											// 19.连锁组合技能
											// 20.直线飞行并返回法球
											// 21.客户端控制法球 	该类型法球只能由玩家发出   不支持怪物
											// 22.可空放组合技
											// 23.单体组合技
											// 24.跟随施法者移动的法球
											// 25.直接伤害+吸血
											// 26.MP减少
											// 27.复制
											// 28.位移
											// 29.固定坐标召唤
											// 30.对地块伤害(不掉落)
											// 31.对植物伤害
											// 32.对植物伤害（不掉落）
											// 33.抛物线法球
											// 34.宽度可变区域陷阱
											// 35.贴地飞行法球(遇到障碍或悬空消失)
											// 36.辐射状发动技能
		AttackFarthestTargetDirectly = 37,	// 37.直击-攻击最远目标
	};


	enum /*class*/ TargetType : NS::uint32
	{
		None			= 0,
		Self			= 1,			// 自己
		EnemyCamp		= 1 << 1,		// 敌方
		FriendlyCamp	= 1 << 2,		// 友方

		AnyGroup		= Self | EnemyCamp | FriendlyCamp, // 所有阵营

		//Guild = 1 << 3,
		//Team = 1 << 4,
		//FriendList = 1 << 5,
		//Resource = 1 << 6,
		//TowerBuild = 1 << 7,
		//Door = 1 << 9,
		//Pet = 1 << 10,
		//NPC = 1 << 11,
	};

	enum TargetCategory : NS::uint32
	{
		CategoryNone			= 0,			// 无
		CategoryPlayer			= 1,			// 玩家
		CategoryMonster			= 1 << 1,		// 怪物
		CategoryNpc				= 1 << 2,		// NPC
		CategoryPet				= 1 << 3,		// 宠物
		CategoryRoleAll			= CategoryPlayer | CategoryMonster | CategoryNpc | CategoryPet,	// 所有角色

		CategoryTowerBuilding	= 1 << 4,		//塔防
		InteractiveBuilding		= 1 << 5,		//互动家具
		BuildingAll				= CategoryTowerBuilding | InteractiveBuilding,	// 所有建筑

		CategoryBlock			= 1 << 6,		// 资源块

		NormalBuilding			= 1 << 7,       // 普通家具
		Plant					= 1 << 8,		// 植物
		BackBlock				= 1 << 9,		// 背景块
		StaticAll				= NormalBuilding | Plant | BackBlock | CategoryBlock,	// 静态物件所有

		CategoryDoor			= 1 << 10,		// 门
	};

	enum TargetArrType : NS::int32
	{
		NoAction,
		HP,
		MP,
	};

	enum LogicOperationType : NS::int32
	{
		NoneOperation,	// 无
		GreaterThan,	// 大于
		LessThan,		// 小于
		Equality,		// 等于
	};

	enum TriggerType : NS::uint32
	{
		TriggerTypeNone						= 0,
		TriggerTypeHpLess					= 1,
		TriggerTypeBeforeHurt				= 1 << 1,
		TriggerTypeUnderAttackRemoveBuff	= 1 << 2,
		TriggerTypeUnderAttackCastSkill		= 1 << 3,
		TriggerTypeUnderAttackFightBack		= 1 << 4,
		TriggerTypeUnderSkillRemoveBuff		= 1 << 5,
		TriggerTypeUnderSkillCastSkill		= 1 << 6,
		TriggerTypeUnderSkillFightBack		= 1 << 7,
		TriggerTypeAttackBloodSucking		= 1 << 8,
		TriggerTypeAttackCastSkill			= 1 << 9,
		TriggerTypeCastingBloodSucking		= 1 << 10,
		TriggerTypeCastingCastSkill			= 1 << 11,
		// 1 << 12, //普攻，伤害宿主
		// 1 << 13, //技能，伤害宿主
		TriggerTypeAllyDie					= 1 << 14,
		TriggerTypePermanent				= 1 << 15,	//常驻型被动
		TriggerTypeMobDie					= 1 << 16,	//指定怪物死亡触发
		TriggerTypeTargetByMobIDType		= 1 << 17,	//指定怪物在身边时候
		TriggerTypeByTime					= 1 << 18,  //指定时间段触发
		TriggerTypeByFallingDist			= 1 << 19,  //坠落距离触发
		TriggerTypeByWater					= 1 << 20,  //液体中触发
		TriggerTypeByMoving					= 1 << 21,  //移动时触发
		TriggerTypeBySprint					= 1 << 22,  //冲刺时触发
		TriggerTypeByPetOrNpc				= 1 << 23,  //宠物&NPC出战触发
		TriggerTypeByYAxisDepth             = 1 << 24,  //达到Y轴一定深度触发
		TriggerTypeHpLessLoop				= 1 << 25,  //每损失x%生命触发技能(不走CD)
		TriggerTypeCasterKillTarget         = 1 << 26,  //施法者杀死目标时施法者触发
		TriggerTypeCasterKilled             = 1 << 27,//被击杀时触发
		TriggerTypeAura						= 1 << 28,	// 光环触发技能
	};

	enum class ThroughMode : NS::uint8
	{
		None = 0,
		Block = 1,
		Role = 2,
		All = 3,
	};

	enum : NS::int32
	{
		MAX_EFFECTS = 3,
        MAX_ATTR_REF = 2,
	};

	enum : NS::int32
	{
		SKILL_INF = 3000000,
		SKILL_CAV = 3000001,
		SKILL_ARC = 3000002,
		SKILL_BLA = 3000003,
		SKILL_MAG = 3000004,
	};

	enum : NS::int8 {
		ENV_ONLY_SERVER = 1,
		ENV_ONLY_LOCAL = 2,
		ENV_LOCAL_AND_SERVER = 3,
	};

	class EffectAttrRef
	{
	public:
		NS::int32 m_effectAttrref{};
		NS::int32 m_effectValue{};
	};

	class Effect
	{
	public:
		enum class Type : NS::uint8
		{
			None = 0,
			TrueDamage = 1,			// 1, 真實傷害
			PhysicalDamage = 2,		// 2, 直接傷害(物理)
			FireDamage = 3,			// 3, 直接傷害(火)
			IceDamage = 4,			// 4, 直接傷害(冰)
			ThunderDamage = 5,		// 5, 直接傷害(雷)
			EarthDamage = 6,		// 6, 直接傷害(土)
			PoisonDamage = 7,		// 7, 直接傷害(毒)
			LightDamage = 8,		// 8, 直接傷害(光)
			DarkDamage = 9,			// 9, 直接傷害(暗)

			AddBuff = 10,			// 10, 增加Buff
			Summon = 11,			// 11, 召喚
			HealthRecovery = 12,	// 12, HP恢復
			// 13, MP恢復
			// 14, 復活
			// 15. 击退
			CastSkill = 16,			// 16. 调用技能
			HurtOXY = 17,			// 17. 扣氧气
			HurtSoil = 18,			// 18. 对地块伤害
			// 19. 连续发射器（跟16类似，区别在于16会随机位置而且会找落地的点，19是指向一个位置连续发射）
			// 20. 调用技能（跟16的区别在于 根据法球半径产生间隔 坐标不重复）
			// 21. 吸血(物理)并反馈给召唤者
			// 22. 吸血(技能)并反馈给召唤者
			SummonTarget = 23,		// 23.对目标召唤受击镜像
			SummonTargetPos = 24,	// 24.在目标位置召唤
			HurtAndSucking = 25,	// 25.直接伤害 + 吸血
			// 26.MP减少
			// 27.复制
			// 28.位移
			// 29.固定位置召唤
			HurtSoilNoDrop = 30,// 30.破坏地块（不掉落）
			HurtPlant = 31, // 31.对植物伤害
			HurtPlantNoDrop = 32,// 32.对植物伤害 (不掉落)
			// 33, // 朝目标左右方向调用技能
			PutBlock = 34, //放置土块
			PutBuild = 35, //放置家具
			// 36, //技能结束后,朝目标左右方向调用技能
			// 37, //技能结束后,再施放一个法球类技能
			HurtSoilNoIgnoreStick = 38,			// 38. 对地块伤害，地块有掉落，无视附作物，附作物无掉落	弱联网专用效果
			HurtSoilNoDropIgnoreStick = 39,		// 39. 对地块伤害，地块无掉落，无视附作物，附作物无掉落	弱联网专用效果
		};

	public:
		SkillType::Effect::Type m_type;
		TargetType m_targetType;
		TargetCategory m_targetCategory;
		NS::int32 m_range{};
		NS::int32 m_targetCount{};
		std::vector<NS::int32> m_mobid{};
		NS::int32 m_hitpercent{};
		NS::tstring m_effectsound{};
        EffectAttrRef m_effectAttrRefs[MAX_ATTR_REF];

		NS::int32 m_HitEffectId;
	};

public:
	virtual ~SkillType();

	const Effect* findEffect(int index) const;

	bool IsHealthRecovery() const;
	bool IsRestoreHP() const;
	bool IsPassivesSkill() const;
	bool IsActivesSkill() const;
	// AI使用的特殊技能
	bool IsAiSpecialSkill() const;
	bool isDamageSkill() const;

    bool isTriggerBeforeDmg() const;
    bool isTriggerAfterDmg() const;

public:
	NS::int32 m_id{};
	NS::int32 m_tempSkillId{};
	NS::int32 m_lv{};
	NS::tstring m_name{};
    NS::tstring m_name_untrans;
	NS::tstring m_desc{};
	NS::tstring m_desc_untrans{};
	NS::tstring m_skillIcon{};
	NS::tstring m_skillAction{};
	NS::tstring m_skillJumpAction{};
	NS::tstring m_skillDirection{};
	Type m_skillType{};
	NS::int32 m_castType{};
	NS::int32 m_costItem{};
	NS::int32 m_costMp{};
	NS::int32 m_showcostmp{};
	NS::int32 m_cooldown{};

	TargetType m_targetType{ TargetType::None };
	TargetCategory m_targetCategory{ TargetCategory::CategoryNone };
	NS::int32 m_targetCount{};

	NS::int32 m_targetArrtType{ TargetArrType::NoAction };
	NS::int32 m_logicOperation{ LogicOperationType::NoneOperation };
	NS::int32 m_logicValue{};

	float m_attkRange{};
	float m_attkMinRange{};
	ThroughMode m_through{};
	NS::int32 m_effectUseBallSetting{};

	float m_ballSize{};
	float m_ballSpeed{};
	float m_ballTime{};
	float m_ballAction{};
	NS::tstring m_ballSound{};
	NS::int32 m_ball_num_max{};

	float m_areaWidth{};
	float m_areaHeight{};
	NS::int32 m_areaAliveTime{};
	NS::int32 m_areaIntervalTime{};

	float m_lineWidth{};
	NS::int32 m_lineAliveTime{};
	NS::int32 m_lineIntervalTime{};

	NS::int32 m_buffEffectDelay{};

	NS::int32 m_CastEffectId;
	NS::tstring m_CastEffectSound;
	NS::int32 m_FlyEffectId;

	NS::tstring m_scriptPath;
	NS::tstring m_scriptParam;

	NS::int32 m_singingTime{};
	NS::int32 m_skillInCD{};
	NS::int32 m_triggerType{};
	NS::int32 m_triggerParam1{};
	NS::int32 m_triggerParam2{};
	NS::int32 m_triggerParam3{};

	NS::int32 m_effect_env{};

	float m_hitEffectOffsetX{};
	float m_hitEffectOffsetY{};

	float m_ballOffsetX{};
	float m_ballOffsetY{};

	//技能对目标施加buff时候，在目标身上添加施法特效
	std::map<NS::int32, NS::int32> m_effectWhenApplyBuffToTarget;

	int m_display_atTip;
	int m_skillCastType{};

	Effect m_effects[MAX_EFFECTS];
    bool m_bSummon{}; // 是否为召唤技能

    float m_value[5];

	int m_unlock{};
};

class SkillCfg
{
public:
	typedef std::unordered_map<NS::int32, SkillType> SkillTypeMap;
	typedef std::unordered_map<NS::int32, NS::int32> SkillTempTypeMap;  //第三帝国技能配置与旧配置对应
	typedef std::map<int, std::map<int, int>> SkillReplaceMap;

public:
	static const float AURA_FOV_RANGE;

public:
	static SkillCfg& getInstance();

	bool initialize();
	void terminate();
    void reloadMultiLanguage();

	const SkillType* findSkillType(NS::int32 id) const;
	const SkillType* findSkillTypeByTempId(NS::int32 id) const;
	bool findReplaceSkill(int cosSkillId, int oldSkillId, int& newSkillId);// 造型武器->技能替换

private:
	bool parseSkillConfigConfig(tinyxml2::XMLElement* pXmlRoot);
	bool parseSkillReplaceConfig(tinyxml2::XMLElement* pXmlRoot);

private:
	SkillTypeMap m_skillTypeMap;
	SkillTempTypeMap m_skillTempTypeMap;
	SkillReplaceMap m_skillReplaceMap;

};

//ENUM_CLASS_FLAGS(SkillType::Type);
//ENUM_CLASS_FLAGS(SkillType::TargetType);
//ENUM_CLASS_FLAGS(SkillType::ThroughMode);
//ENUM_CLASS_FLAGS(SkillType::Effect::Type);
