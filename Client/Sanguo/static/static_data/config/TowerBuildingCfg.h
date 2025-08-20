/*!
 * \file TowerBuildingCfg.h
 *
 * \author xiaobowang;
 * \date 2017/07/07
 *
 * 
 */
#pragma once
#include <map>
#include <memory>
#include <unordered_map>
#include "../data/role/Role.h"

// 装置种类
enum class TowerBuildingCategory : int
{
	EXPORT = 0,			// 输出
	TIE = 1,			// 牵制
	ASSIST = 2,			// 辅助
	URGENT = 3,			// 紧急
	CALL = 4,			// 召唤 
	MOVE = 5,			// 移动
	CRYSTAL = 6,		// 水晶
	CRYSTAL_BASE = 7,	// 水晶底座
	SOUL_TOWER = 8,		// 魂塔(召怪)	
	CRYSTAL_SMALL = 9,	// 水晶接收站 
	SOLDIER_OUT = 10,	// 出兵点
	SHIP_MAINGUN = 11,	// 飞船主炮
	SHIP_ENGINE = 12,	// 引擎家具
};

struct TowerBuildSkillInfo
{
	NS::int32 n_skill;
	NS::int32 n_skill_target_choose_first = 0;
	NS::int32 n_skill_target_movetype = 0;
	NS::int32 n_skill_cast_event = 0;

	NS::UI64 n_target_type = 256;
	NS::UI64 n_target_camp = 2;
};
struct TowerFunctionInfo
{
	NS::int32 n_function;
	NS::int32 n_function_param1;
	NS::int32 n_function_param2;
	NS::int32 n_function_param3;
	NS::int32 n_function_param4;
	std::string s_function_param;
};

// 水晶结界数据
struct CrystalWardsData
{
	NS::int32 n_WardsMobLev;  // 结界影响怪物等级(包含及以下)
	std::vector<int> vecWardsBuff; // 进入结界时给目标添加的Buff
	std::vector<int> vecWardsImmunityMob; // 免疫结界效果怪物ID
};

struct TowerBuildingDefence
{
	NS::uint m_id{};
	NS::uint m_buildingTypeId{};
	int m_level{};
	NS::tstring m_name{};
	NS::tstring m_name_untrans{};
	NS::tstring m_note{};
	NS::tstring m_note_untrans{};
	TowerBuildingCategory m_category{};
	NS::tstring m_typeDes{};
	NS::tstring m_typeDes_untrans{};
	int m_energy{};
	int m_power{};
	int m_hp{};
	int n_Prosperous{}; //增加繁荣度
	bool m_canAttack{};
	float m_hpUiCheck{};
	NS::uint m_fov{};
	NS::uint m_forbidden{};
	NS::uint m_ad{};
	NS::uint m_ap{};
	NS::tstring m_skillSchematic{};
	NS::uint m_roleLevelLimit{};


	NS::int32 n_arm;
	NS::int32 n_marm;
	NS::int32 n_public_cd;
	NS::int32 n_disapear_die = 0;
	std::vector<TowerBuildSkillInfo> m_vSkill;
	std::vector<TowerFunctionInfo> m_vTowerFunction;

	CrystalWardsData m_CrystalWardsData;

	NS::int32 n_BeAttackNumberLimit{-1};

	NS::int32 n_mob_id{};

	AttrMap getAttr() const;
};

struct TowerBuildingRepair
{
	struct RepairMaterial
	{
		int m_itemId{};
		NS::uint m_num{};
	};

	NS::uint m_id{};
	NS::uint m_buildingTypeId{};
	int m_level{};
	NS::uint m_cashSkip{};
	NS::uint m_skipSecUnit{};
	NS::uint m_repairGold{};
	NS::uint m_repairTime{};

	std::vector<RepairMaterial> m_repairMaterialList{};
};

struct TowerBuildingUpdate
{
	struct UpgradeStuff {
		NS::uint m_itemTypeId{};
		NS::uint m_itemNum{};
	};


	NS::uint m_id{};
	NS::uint m_buildingTypeId{};
	int m_level{};
	NS::uint m_needCoin{};

	UpgradeStuff m_upItem1;
	UpgradeStuff m_upItem2;
	std::map<NS::uint, UpgradeStuff> m_upgradeStuffMap{};
};

struct TowerBuildingReform
{
	struct EvolveStuff {
		NS::uint m_itemTypeId{};
		NS::uint m_itemNum{};
	};

	NS::uint m_id{};
	NS::uint m_buildingTypeId{};
	NS::uint m_reformBuildTypeId{};
	NS::uint m_needCoin{};

	std::map<NS::uint, EvolveStuff> m_evolveStuffMap{};
};

struct TowerBuildShootFov
{
	double m_fShootingDirX;
	double m_fShootingDirY;

	double m_fShootingCos;
};
struct TowerBuildTemplateAttack
{
	NS::int32 n_ID;

	NS::int32 m_iShootingPointX;
	NS::int32 m_iShootingPointY;
	std::vector<TowerBuildShootFov> m_vShootFovCfg;
};

struct TowerBuildCrystalBuffCostItem
{
	int32 m_ItemID = -1;
	int32 m_ItemCount = 0;
};
struct TowerBuildCrystalBuff
{
	int32 n_ID;         // #主键id, 无业务含义
	int32 n_need_level; // #等级
	int32 n_buff_id;    // #buff id
	std::string s_name; // #名称
	NS::tstring s_name_untrans{};
	std::vector<TowerBuildCrystalBuffCostItem> m_vCrystalBuffItemCost; // s_unlock_item #解锁加护物品 格式: [物品id, 数量], ..., [物品id, 数量]
	int32 n_buff_duration; //#持续时间
};

class TowerBuildingCfg
{
public:
    using TowerBuildingDefenceMap = std::unordered_map<NS::uint, TowerBuildingDefence>;
    using TowerBuildingRepairMap = std::unordered_map<NS::uint, TowerBuildingRepair>;
    using TowerBuildingUpdateMap = std::unordered_map<NS::uint, TowerBuildingUpdate>;
    using TowerBuildingReformMap = std::unordered_map<NS::uint, TowerBuildingReform>;
    using TowerBuildTemplateAttackMap = std::unordered_map<NS::uint, TowerBuildTemplateAttack>;
    using TowerBuildCrystalBuffMap = std::unordered_map<NS::uint, TowerBuildCrystalBuff>;

public:
	static TowerBuildingCfg& getInstance();

	bool initialize();
	void terminate();
	
	const TowerBuildingDefence* getTowerBuildingDefence(NS::uint nId);
	const TowerBuildingRepair* getTowerBuildingRepair(NS::uint buildingTypeId, int level);
	const TowerBuildingUpdate* getTowerBuildingUpdate(NS::uint buildingTypeId, int level);
	int getTowerBuildingEvoMaxLev(NS::uint buildingTypeId);
	std::vector<const TowerBuildingReform*> getTowerBuildingReformList(NS::uint buildingTypeId);
	const TowerBuildingReform* getTowerBuildingReform(NS::uint buildingTypeId, NS::uint toBuildingTypeId);
	const TowerBuildingReform* getTowerBuildingReform(NS::uint toBuildingTypeId);
	NS::uint getPrevEvolveBuildingTypeId(NS::uint buildingTypeId);

	bool isCanEvolve(NS::uint buildingTypeId, NS::uint toBuildingTypeId, bool recursive);

	NS::int32  getTowerBuildingSoulMobNum(NS::uint towerBuildingDefenceId, int monsterTypeId);

	const TowerBuildingDefenceMap& getTowerBuildingDefenceMap(){ return m_towerBuildingDefenceMap; }
	const TowerBuildingRepairMap& getTowerBuildingRepairMap(){ return m_towerBuildingRepairMap; }
	const TowerBuildingUpdateMap& getTowerBuildingUpdateMap() { return m_towerBuildingUpdateMap; }
	const TowerBuildingReformMap& getTowerBuildingReformMap() { return m_towerBuildingReformMap; }
	const TowerBuildTemplateAttackMap& getTowerBuildTemplateAttackMap() { return m_towerBuildTemplateAttackMap; }
	const TowerBuildCrystalBuffMap& getTowerBuildCrystalBuffMap() { return m_towerBuildCrystalBuffMap; }

	int getTowerBuildingShipGunMaxLev() { return m_towerBuildingShipGunMaxLev; }

	bool reloadMultiLanguage();

private:
    TowerBuildingCfg() = default;
    ~TowerBuildingCfg();

	static void ParseShootFov(const std::string& str, const bzbee::int32 ShootFov, TowerBuildShootFov& Cfg);

	bool parseBuildTowerDefence(tinyxml2::XMLElement* pXmlRoot);
    bool parseBuildTowerRepair(tinyxml2::XMLElement* pXmlRoot);
	bool parseBuildTowerUpdate(tinyxml2::XMLElement* pXmlRoot);
	bool parseBuildTowerReform(tinyxml2::XMLElement* pXmlRoot);
	bool parseBuildTowerTemplateAttack(tinyxml2::XMLElement* pXmlRoot);
	bool parseBuildTowerCrystalBuff(tinyxml2::XMLElement* pXmlRoot);

private:
	TowerBuildingDefenceMap m_towerBuildingDefenceMap{};
	TowerBuildingRepairMap m_towerBuildingRepairMap{};
	TowerBuildingUpdateMap m_towerBuildingUpdateMap{};
	TowerBuildingReformMap m_towerBuildingReformMap{};
	TowerBuildTemplateAttackMap m_towerBuildTemplateAttackMap{};
	TowerBuildCrystalBuffMap m_towerBuildCrystalBuffMap{};

	int m_towerBuildingShipGunMaxLev{};
};
