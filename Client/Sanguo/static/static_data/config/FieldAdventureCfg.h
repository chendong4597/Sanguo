#pragma once

#include <baseutil.h>
#include <unordered_map>
#include <memory>
#include <vector>
#include "Rtti.h"
#include <lua/LuaCallback.h>

// Map数据
class FieldAdventureMapInfo
{
public:
	int n_ID{ 0 };						// ID
	int n_mapID{ 0 };					// 地图ID
	float f_FT_coordinateX1{ 0.0f };		// 我方出生点坐标X1
	float f_FT_coordinateY1{ 0.0f };		// 我方出生点坐标Y1
    float f_FT_coordinateX2{ 0.0f };		// 我方出生点坐标X2
    float f_FT_coordinateY2{ 0.0f };		// 我方出生点坐标Y2
	float f_FT_radiusX{ 0.0f };			// 我方出生半径X
	float f_FT_radiusZ{ 0.0f };			// 我方出生半径Z
	float f_AT_coordinateX1{ 0.0f };		// 怪物出生点坐标X1
	float f_AT_coordinateY1{ 0.0f };		// 怪物出生点坐标Y1
    float f_AT_coordinateX2{ 0.0f };		// 怪物出生点坐标X2
    float f_AT_coordinateY2{ 0.0f };		// 怪物出生点坐标Y2
	float f_AT_radiusX{ 0.0f };			// 怪物出生半径X
	float f_AT_radiusZ{ 0.0f };			// 怪物出生半径Z
	NS::tstring s_MapNameKey{};         //地图名称key
	glm::vec3 vec3CameraPos;			// 摄像机位置
	glm::vec3 vec3CameraLookAtPos;		// 摄像机目标位置
// 	glm::vec3 vec3CameraRotate;			// 摄像机角度
// 	float f_CameraZoom{ 0.0f };			// 摄像机Zoom
	glm::ivec2 m_vec2MapBoundaryLeftUpper; // 额外设置的地图边界的左上边界
	glm::ivec2 m_vec2MapBoundaryLowerRight; // 额外设置的地图边界的右下边界
};

// 怪物数据
class FieldAdventureMobInfo
{
public:
	int n_ID{ 0 };						// 怪物ID
	NS::tstring s_MonsterIcon;			// 怪物头像
	NS::tstring s_MonsterTips;			// 怪物描述KEY
	int n_PowerBuff{ 0 };				// 属性BUFF
	int n_AblityBuff{ 0 };				// 特殊能力BUFF
	int n_AIType{ 0 };					// AI类型
	float f_BornPlaceX{ 0.0f };			// 出生坐标校正X
	float f_BornPlaceY{ 0.0f };			// 出生坐标校正Y
	float f_BornPlaceZ{ 0.0f };			// 出生坐标校正Z
};

// 任务数据
class FieldAdventureInfo
{
public:
	int n_ID{ 0 };						// ID
	NS::tstring s_namekey;				// 任务名称
	int n_mapID{ 0 };					// 地图ID
	int n_type{ 0 };					// 类型
	int n_rewardID{ 0 };				// 任务奖励ID
	NS::tstring s_Icon{ 0 };			// 任务图标
	NS::tstring s_missiontipskey{ 0 };	// 任务简介KEY

	int n_biome{ 0 };
	int n_num{ 0 };
	int n_unlockbiome{ 0 };

	int n_AdventureRe{ 0 };//野外挂机奖励
	int n_MineRe{ 0 };//挖矿奖励
	int n_addExp{ 0 };//经验奖励
	// 怪物数据
	class FieldAdventureMobInfo
	{
	public:
		int nMobID{ 0 };				// 怪物ID 对应 NPCmissionMob.xlsx 和 Mob.xlsx  配置文件的ID
		int nMobNum{ 0 };				// 怪物数量
	}; 
	std::vector<FieldAdventureMobInfo> vecMobData{}; // 怪物数据
};

class FieldAdventureCfg
{
public:
	RTTI_ROOT(FieldAdventureCfg);
public:
	static FieldAdventureCfg& getInstance();

	bool initialize();
	void terminate();

	// 任务数据
	const FieldAdventureInfo* FindFieldAdventureInfo(int idTask);
	// 怪物数据
	const FieldAdventureMobInfo* FindFieldAdventureMobInfo(int idMob);
	// 地图数据
	const FieldAdventureMapInfo* FindFieldAdventureMapInfo(int idMap);
	//LuaTable METHOD(getFieldAdventureItem_lua)(int idTask);

	int getBiomeMiningRewardId(NS::I64 biomeId);

	//是否是NpcPetFight场景
	bool IsFieldAdventureFightMap(int idMap);

	bool isCanUnLockNextBiome(int nBiome, int nStage);

	const FieldAdventureInfo* FindFieldAdventureInfo(int nBiome, int nStage);

	const std::map<int, const FieldAdventureInfo*>* FindFieldAdventureInfoByBiome(int Biome);

	void getRandomAtkDropInfo(std::unordered_map<int, int>& out);
	const glm::vec3 getDropOffset();
	const int getDropTime();
private:
	// 解析 NpcAutomaticCfg.xml 文件
	bool parseNpcAutomaticConfig(tinyxml2::XMLElement* pXmlRoot);
	// 解析 NpcAutomaticMaps.xml 文件
	//bool parseNpcAutomaticMapsBinary();
	// 解析 NpcAutomaticMob.xml 文件
	//bool parseNpcAutomaticMobBinary();
private:
	// 任务数据
	std::unordered_map<int, FieldAdventureInfo> m_mapFieldAdventureInfo;
	std::unordered_map<int, std::map<int, const FieldAdventureInfo*>> m_mapFieldAdventureInfoSortBiome;
	// 怪物数据
	std::unordered_map<int, FieldAdventureMobInfo> m_mapFieldAdventureMobInfo;
	// Map数据
	std::unordered_map<int, FieldAdventureMapInfo> m_mapFieldAdventureMapInfo;
	//挖矿奖励id
	std::unordered_map<int, int> m_biome2MiningRewardId{};
	//战斗挂机的物品掉落表现数据
	std::vector<std::tuple<int/*物品id*/,int/*最小掉落*/,int/*最大掉落*/>> m_atkDropInfo{};
	//战斗挂机的物品掉落位置的偏移
	glm::vec3 m_dropOffset{};
	//战斗挂机的物品存续时长
	int m_dropTime{};
};

#define gFieldAdventureCfg (FieldAdventureCfg::getInstance())
