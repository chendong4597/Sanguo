/*!
 * \file AchieveCfg.h
 *
 * \author ChenBK;
 * \date 2017/10/10
 *
 * 
 */
#pragma once

#include <functional>
#include <memory>
#include <array>
#include <map>
#include <unordered_map>
#include "tinyxml2.h"
#include "BaseTypeDefine.h"

using namespace NS;

enum {
	HOUSE_SEL_TYP_H = 0, //民居
	HOUSE_SEL_TYP_W, //城墙
	HOUSE_SEL_TYP_C, //纪念碑
	HOUSE_SEL_TYP_R, //道路
};

enum {
	BUILD_CITY_ID_NONE = 0,
	BUILD_CITY_ID_WALL = 1000, //城墙
	BUILD_CITY_ID_CHAMER = 1001, //纪念碑
	BUILD_CITY_ID_ROAD = 1002, //道路
};

class HouseCfg
{
public:
	enum
	{
		HOUSE_TYPE_Wall = 1, //城墙
		HOUSE_TYPE_Charm, //纪念碑
		HOUSE_TYPE_Road,		//道路
		HOUSE_TYPE_Normal = 9,	//普通民居
	};
public:
	struct HouseItemInfo
	{
		//-------------------------------------------
		// data
		NS::int32 n_ID;
		NS::int32 n_X;
		NS::int32 n_Y;
		NS::int32 n_HouseTpy;
	};

	struct HouseTypeInfo
	{
		int32 n_ID;
		int32 n_Type;
		int32 n_Quality; //星级
		std::string s_icon;
		std::string s_Name;
		std::string s_Path;
		int32 n_Honor; //增加荣誉值
		std::string s_items; //升级需要材料
		int32 n_Lev; //等级
		int32 n_Innum; //最多入住人数
		int32 n_NextId; //下一个建筑ID
		int32 n_upgradeTm; //升级需要时间
		int n_Prosperous{}; //增加繁荣度
		int n_RentRate{};// 提供租金系数
		int n_PlantRate{};// 种植系数
		int n_FarmRate{};// 养殖系数
		int n_MineRate{};// 挖矿系数
		int n_ShopRate{};// 商品优惠
		int n_CraftRate{};// 锻造效率
		int n_buildRate{};// 建筑房屋系数
		std::map<int, int> m_NeedItems;
	};


    typedef std::unordered_map<NS::uint, HouseItemInfo>			HouseItemMap;
	typedef std::unordered_map<NS::uint, HouseTypeInfo>			HouseTpyMap;

public:
	static HouseCfg& getInstance();

	bool initialize();
	void terminate();
    void reloadMultiLanguage();

	const HouseItemInfo* getHouseItemInfo(int id);
	const HouseTypeInfo* getHouseTypeInfo(int id);

	const HouseItemMap& getHouseItems() { return m_houseItemMap; }
	const HouseTpyMap& getHouseTypes() { return m_houseTypeMap; }
	void GetVecHouseTpy(int nHouseTpy, std::vector<int>& vecHouse);

private:
	bool parseHoueItemConfig(tinyxml2::XMLElement* pXmlRoot);
	bool parseHoueTypeConfig(tinyxml2::XMLElement* pXmlRoot);

private:
	HouseItemMap	m_houseItemMap;
	HouseTpyMap		m_houseTypeMap;
};
