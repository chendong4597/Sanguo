/*!
 * \file ArmorCfg.h
 *
 * \author ChenBk
 * \date 12/30/2016
 *
 */

#pragma once
#include <map>
#include <memory>
#include <unordered_map>
#include "tinyxml2.h"
#include "../data/role/Role.h"

//////////////////////////////////////////////////////////////////////////
class ItemData;
class ArmorCfg
{
public:
	enum TargetLayer : bzbee::uint8
	{
		NONE = 0,
		Front = 1,
		Back = 2,	
	};

public:
	class ArmorUpGrade
	{
	public:
		int id{};
		int nextId{};
		struct needInfo
		{
			int id{};
			int num{};
		};
		std::vector<needInfo> vecItem{};
	};

	struct AttaEffectInfo
	{
		int  n_effect_attrref{ 0 };
		int	 n_effect_value{ 0 };
	};
	typedef std::vector<AttaEffectInfo> AttrInfo;

	struct AttaInfoType
	{
		int  n_effect_type{ 0 };
		AttrInfo vecArmorAtta;
	};

    typedef std::vector<int> SkillVec;

	typedef std::vector<AttaInfoType> ArmorAttrVec;
	class ArmorType
	{
	public:
		bzbee::uint m_typeId{};  //装备id
		bzbee::tstring m_sMotion; //动作形态
		bzbee::tstring m_s3DModel; //装备外观
		bzbee::tstring m_s3DModelWoman; // 女性装备外观
		bzbee::tstring m_sSub3DModel; //装备外观2
		bzbee::tstring m_sSub3DModelWoman; // 女性装备外观2
		bzbee::tstring m_sHigh3DModel; //装备高模
		bzbee::tstring m_sHigh3DModelWoman; // 女性装备高模
		bzbee::tstring m_sSubHigh3DModel; //装备高模外观2
		bzbee::tstring m_sSubHigh3DModelWoman; // 女性装备高模外观2
		int m_equipType{};   //装备类型
		int m_equipSubType{}; //装备子类型
		int m_equipPos{};    //装备位置
		int m_starRank{};    //星等级
		int n_Durable{};     //耐久度
		float f_Pause_time{};	// 击中目标后攻击动作暂停时间
		float m_atkRange{};  //攻击距离
		float m_atkSpeed{};  //攻击速度
		int n_Atk_Limit{};     //攻击目标上限
		float f_GatherSpeed{};  //挖掘速度
		float m_ResourceAtkDmg;  //对资源的攻击力
		int m_durable{};		//耐久度
		int m_durableRate{};   //耐久消耗基数
		int n_target_layer{};  //挖掘层类型
		int n_gatherLv{};     //挖掘等级
		bool m_isDoubleHandler; //是否双持
		bool m_isContinuousAttackWeapon; //是否为一直按住攻击按钮连续攻击的武器
		int m_attrExt{ 0 };//装备附属ID;
		SkillVec m_vSkill{};
		SkillVec m_vPassiveSkill{};
		SkillVec n_vArmorSpcialEffectIDs{}; //装备特殊效果

		bzbee::IntegerList m_vHideNode;  //隐藏裸模部位
		bzbee::IntegerList m_vFemaleHideNode; //隐藏女性裸模部位
		bzbee::IntegerList m_vHideEquip;  //隐藏装备
		bzbee::IntegerList m_vDisablePos;	// disable equip pos
		bzbee::tstring s_trailEffectModel;
		int n_replaceNakedModel{};
		bzbee::tstring s_attach_node;
		bzbee::IntegerList m_actionEffect;
		glm::vec3 m_postionRegion;
		ArmorAttrVec m_vecEffectAttr{};
		std::vector<std::tuple<bzbee::tstring/*animation*/, bzbee::tstring/*effect*/>> m_atkAniEffect;
		std::vector<std::tuple<bzbee::tstring/*animation*/, bzbee::tstring/*effect*/>> m_jumpAtkAniEffect;
		bzbee::StringList m_hurtEffect;
		bzbee::tstring s_AttackSound;
		bzbee::tstring s_UnderAttackSound;

		std::map<int, bzbee::tstring> m_AnimationMapMale;
		std::map<int, bzbee::tstring> m_HighAnimationMapMale;
		std::map<int, bzbee::tstring> m_AnimationMapFemale;
		std::map<int, bzbee::tstring> m_HighAnimationMapFemale;
		int m_enchantmentType; // 附魔类型ID
		int m_hurtEffectId{};  //受击效果ID
		int n_pet_moveType{};
        int n_SoundBankType{ 0 };
		int n_canUpLevel{ 0 };
        std::vector<int> s_SoundBankVec;

	public:
		const auto& getAnimationMap(int gender) const { return gender == Role::Gender::Female ? m_AnimationMapFemale : m_AnimationMapMale; }
		const auto& getHighAnimationMap(int gender) const { return gender == Role::Gender::Female ? m_HighAnimationMapFemale : m_HighAnimationMapMale; }
		const auto& getModel(int gender) const { return gender == Role::Gender::Female ? m_s3DModelWoman : m_s3DModel; }
		const auto& getSubModel(int gender) const { return gender == Role::Gender::Female ? m_sSub3DModelWoman : m_sSub3DModel; }
		const auto& getHighModel(int gender) const { return gender == Role::Gender::Female ? m_sHigh3DModelWoman : m_sHigh3DModel; }
		const auto& getHighSubModel(int gender) const { return gender == Role::Gender::Female ? m_sSubHigh3DModelWoman : m_sSubHigh3DModel; }
        bool isAttrIsStar(int effect_type);

	};
	typedef std::unordered_map<bzbee::uint, ArmorType> ArmorTypeMap;

	class ArmorTemplate
	{
	public:
		bzbee::uint m_templateId{};
		bzbee::uint m_typeId{};
		bzbee::uint m_target_layer;
	};

	typedef std::unordered_map<bzbee::uint, ArmorTemplate> ArmorTemplateMap;
	typedef std::unordered_multimap<bzbee::uint, const ArmorTemplate*> ArmorTemplateByTypeMap;


	class ArmorEnchantAttr
	{
	public:
		bzbee::uint m_id{};  //id
		bzbee::uint m_enchantment_type{};  //附魔类型
		int m_energy{};  //能量需求
		bzbee::uint m_weight{};  //基础权重
		bzbee::tstring s_name;  //名称
		bzbee::uint m_rate{}; //成功率
		std::map<int, int> m_attr;  //属性值
	};
	typedef ::std::unordered_map<bzbee::uint, ArmorEnchantAttr> ArmorEnchantAttrMap;

	class ArmorAttrExt
	{
	public:
		bzbee::uint m_id{};  //id
		int min{};
		int max{};
		int rateMin{};
		int rateMax{};
		std::map<int, int> m_attr; //附加属性
	};
	typedef ::std::unordered_map<bzbee::uint, ArmorAttrExt> ArmorAttrExtMap;

	struct ArmorSuitInfo
	{
		NS::int32 n_id = 0;
		std::set<NS::int32> setItemID;
	};
	typedef std::unordered_map<NS::int32, ArmorSuitInfo> ArmorSuitMap;

	struct ArmorFogUnlockInfo
	{
		NS::int32 n_id = 0;
		NS::int32 n_unlock_radius;
		NS::int32 n_unlock_angle;
	};
	typedef std::unordered_map<NS::int32, ArmorFogUnlockInfo> ArmorFogUnlockMap;

	// 装备特效效果配置
	class ArmorSpecialEffect
	{
	public:
		// 装备特效特效类型
		enum EFFECT_ARMOR_TYPE : int
		{
			ARMOR_EFFECT_ADD_SHOOT_RANGE		= 1, // 增加射程
			ARMOR_EFFECT_FORCE_BACK		= 2, // 击退系数
			ARMOR_EFFECT_ADD_BUFF		= 3, // 增加buff
			ARMOR_EFFECT_ADD_NORATK_RANGE    = 4, //增加普攻距离
		};

		// 装备特效堆叠规则
		enum EFFECT_ARMOR_RULE : int
		{
			RULE_ARMOR_REPLACE = 1,
			RULE_ARMOR_ADD = 2,
		};

	public:
		// 是否生效
        bool isEffective(int armorType) const;

    public:
		NS::int32 n_ID;
		EFFECT_ARMOR_TYPE n_type;
		std::vector<int> v_armor_sub_types;
		NS::int32 n_value;
		NS::int32 n_buff_id;
		EFFECT_ARMOR_RULE n_replace;
	};
	typedef std::unordered_map<NS::int32, ArmorSpecialEffect> ArmorSpecialEffectMap;

public:
	static ArmorCfg& getInstance();

	bool initialize();
	void terminate();

	const ArmorType* findArmorType(bzbee::uint typeId);
	const ArmorType* findArmorTypeByTemplateId(bzbee::uint templateId);
	const ArmorTemplate* findArmorTemplate(bzbee::uint templateId);
	const ArmorTemplate* findBuildingTemplate(int typeId, ArmorCfg::TargetLayer targetLayer);
	const ArmorEnchantAttr* findArmorEnchantAttr(bzbee::uint enchantId);
	const ArmorCfg::ArmorAttrExt* findArmorAttrExt(bzbee::uint itemId);
	const ArmorSuitInfo* findArmorSuitCfg(const NS::int32 nItemID);
	const ArmorFogUnlockInfo* findArmorFogUnlockCfg(const NS::int32 nItemId);
	const ArmorSpecialEffect* findArmorSpecialEffect(const NS::int32 nItemId) const;
	int getEnchantmentEnergyMax(bzbee::uint enchantType);  
	int getEnchantmentEnergyMin(bzbee::uint enchantType);
	bool isSameTypeGroup(int nType0, int nType1);
	//void findHideEquipAndNode(const Role::EquipMap& equipMap, bzbee::IntegerList& m_vHideEquip, bzbee::IntegerList& m_vHideNode, bzbee::IntegerList& m_vFemaleHideNode, const Role::EquipMap& fullEquipMap);
	int getCosEquipType(int equipType);
	int toEquipPacketPos(int cfgEquipPos); // 装备Pos, 从配置Pos 转为 Pack Pos
	bool isDoubleHand(int id);
	void getRandomEquipments(std::unordered_map<int, std::shared_ptr<ItemData>>& equipments) const;

	const ArmorType* findFirstArmorByEquipSubType(bzbee::uint equipSubTpy);

	static const int getEquipPacketPos(const int emArmorPos);

	const ArmorType* findCanDigArmorType(NS::int32 digLevel, ArmorCfg::TargetLayer);
	const ArmorCfg::ArmorUpGrade* getUpLevelCfg(int id);


private:
	//bool parseArmorBinary(); 	
	//bool parseArmorCfgTemplateBinary();
	//bool parseArmorMagicAddBinary();
	//bool parseArmorAttrExtBinary();
	//bool parseArmorTypeGroupBinary();
	//bool parseArmorSuitCfgBinary();
   // bool parseArmorUpGradeCfgBinary();
 //   bool parseArmorFogUnlockBinary();
	//bool parseArmorSpecialEffectCfgBinary();

	bool parseArmorConfig(tinyxml2::XMLElement* pXmlRoot);
	bool parseArmorUpGradeConfig(tinyxml2::XMLElement* pXmlRoot);

private:
	ArmorTypeMap m_armorTypeMap;   
	ArmorTemplateMap m_armorTemplateMap;
	ArmorTemplateByTypeMap m_armorTemplateByTypeMap;
	ArmorEnchantAttrMap m_armorEnchantAttrMap;
	ArmorAttrExtMap m_armorAttrExtMap;
	std::vector<bzbee::IntegerList> m_armorTypeGroupList;
	ArmorSuitMap m_mapArmorSuit;
	ArmorFogUnlockMap m_armorFogUnlockMap;
	std::unordered_map<NS::int32, NS::int32> m_mapItemID2SuitID;
	std::unordered_map<int, ArmorUpGrade> m_mapUpLevel;

	ArmorSpecialEffectMap m_mapArmorSpecialEffects{};

	using Pos2ArmorTypes = std::map<int/*position*/, std::vector<const ArmorType*>>;
	mutable Pos2ArmorTypes m_pos2ArmorTypes;

};
