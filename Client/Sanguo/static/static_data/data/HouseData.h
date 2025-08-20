/*!
* \file ItemData.h
*
* \author ChenBk
* \date 12/01/2016
*
*
*/
#pragma once
#include "msg/MsgHouse.pb.h"
#include "../config/HouseCfg.h"
#include <array>
#include <map>

using namespace NS;



class HouseData
{
public:
	HouseData();
	virtual ~HouseData();

	bool initialize();
	void reset();

public:
	NS::int32 m_houseId{ 0 };
	NS::int32 m_houseType{ 0 };
	NS::int32 m_nextHouseTpy{};
	NS::I64   m_completeTm{};
};

//TowerBuildMgr
//------------------------------------------------------------------
class HouseDataMgr
{
public:
	typedef std::unordered_map<NS::int32/*uuid*/, std::shared_ptr<HouseData>> HouseDataMap;					//所有数据, Key: UUID

public:
	HouseDataMgr();
	virtual ~HouseDataMgr();

	static HouseDataMgr& getInstance();
	void terminate();

	void Clear();


	int GetCityWallId();
	int GetCityWallLev();

	//add\remove
	bool addHouse(std::shared_ptr<HouseData> itemData);
	bool removeHouse(NS::int32 delHouseId);

	std::shared_ptr<HouseData> findByHouseId(NS::int32 uuid);

	const HouseDataMap& GetHouseDatas() { return m_houseMap; }

	std::vector<NS::I64> GetNpcsCheckInHouse(int houseId);

	int GetHouseProsperous() const;
	int GetHouseRent() const;
	int GetCityPlantRate() const;
	int GetCityFarmRate() const;
	int GetCityMineRate() const;
	int GetCityShopRate() const;
	int GetCityCraftRate() const;
	int GetCityBuildRate() const;

private:
	HouseDataMap m_houseMap;
};
