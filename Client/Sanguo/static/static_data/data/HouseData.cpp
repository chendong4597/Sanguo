
#include "stdafx.h"
#include "HouseData.h"
#include "config/HouseCfg.h"
#include "data/NpcProtectorData.h"
#include "config/NpcBadgeCfg.h"

using namespace NS;
using namespace MsgPB;

HouseData::HouseData()
{

}

HouseData::~HouseData()
{

}

bool HouseData::initialize()
{
	return true;
}


void HouseData::reset()
{
}


//------------------------------------------------------------------------------------------------
// TowerBuildMgr
//------------------------------------------------------------------------------------------------

HouseDataMgr::HouseDataMgr()
{

}

HouseDataMgr::~HouseDataMgr()
{

}

void HouseDataMgr::terminate()
{
	Clear();
}

void HouseDataMgr::Clear()
{
	m_houseMap.clear();
}


HouseDataMgr& HouseDataMgr::getInstance()
{
	static HouseDataMgr s_houseDataMgr;
	return s_houseDataMgr;
}

bool HouseDataMgr::addHouse(std::shared_ptr<HouseData> houseData)
{
	if (houseData == nullptr) return false;

	// add ItemData
	m_houseMap[houseData->m_houseId] = houseData;

	return true;
}

bool HouseDataMgr::removeHouse(NS::int32 delId)
{
	HouseDataMap::iterator delIt = m_houseMap.find(delId);
	if (delIt != m_houseMap.end())
	{
		// Del ItemData
		m_houseMap.erase(delIt);

		return true;
	}

	return false;
}

std::shared_ptr<HouseData> HouseDataMgr::findByHouseId(NS::int32 houseId)
{
	HouseDataMap::iterator it = m_houseMap.find(houseId);
	if (it != m_houseMap.end())
	{
		return it->second;
	}

	return nullptr;
}


int HouseDataMgr::GetCityWallId()
{
	auto&& house = findByHouseId(1000);
	if (!house) {
		return 2001;
	}
	return house->m_houseType;
}

int HouseDataMgr::GetCityWallLev()
{
	int nHouseId = GetCityWallId();
	auto&& houseCfg = HouseCfg::getInstance().getHouseTypeInfo(nHouseId);
	if (houseCfg)
	{
		return houseCfg->n_Lev;
	}
	return 1;
}

std::vector<NS::I64> HouseDataMgr::GetNpcsCheckInHouse(int houseId)
{
	std::vector<NS::I64> vecUuids;
	HouseDataMap::iterator it = m_houseMap.find(houseId);
	if (it == m_houseMap.end())
	{
		return vecUuids;
	}

	auto&& mapData = NpcProtectorDataMgr::getInstance()->getProtectorDataMap();

	auto&& iter = mapData.begin();
	for (; iter != mapData.end(); iter++)
	{
		if (!iter->second || iter->second->houseId <= 0 || iter->second->uuid == 0)
		{
			continue;
		}
		if (iter->second->houseId == houseId)
		{
			vecUuids.push_back(iter->second->uuid);
		}
	}
	return vecUuids;
}

int HouseDataMgr::GetHouseProsperous() const
{
	int nProsperous = 0;
	HouseDataMap::const_iterator iter = m_houseMap.begin();
	for (; iter != m_houseMap.end(); iter++)
	{
		auto&& hCfg = HouseCfg::getInstance().getHouseTypeInfo(iter->second->m_houseType);
		if (!hCfg) {
			continue;
		}
		nProsperous += hCfg->n_Prosperous;
	}
	return nProsperous;
}

int HouseDataMgr::GetHouseRent() const
{
	int nRent = 0;
	auto&& mapData = NpcProtectorDataMgr::getInstance()->getProtectorDataMap();

	auto&& iter = mapData.begin();
	for (; iter != mapData.end(); iter++)
	{
		if (!iter->second || iter->second->houseId <= 0 || iter->second->uuid == 0)
		{
			continue;
		}
		auto&& npcBadge = NpcBadgeCfg::getInstance().findNpcBadgeCfg(iter->second->itemid, iter->second->level);
		if (!npcBadge) {
			continue;
		}

		auto&& iterH = m_houseMap.find(iter->second->houseId);
		if (iterH == m_houseMap.end())
		{
			continue;
		}

		auto&& hCfg = HouseCfg::getInstance().getHouseTypeInfo(iterH->second->m_houseType);
		if (!hCfg) {
			continue;
		}
		
		nRent += static_cast<int>(npcBadge->n_Rent*(100.f + hCfg->n_RentRate) / 100.f);
	}

	return nRent;
}

int HouseDataMgr::GetCityPlantRate() const
{
	int nPlantRate = 0;
	HouseDataMap::const_iterator iter = m_houseMap.begin();
	for (; iter != m_houseMap.end(); iter++)
	{
		if (iter->second->m_houseId < BUILD_CITY_ID_WALL) {
			continue;
		}

		auto&& hCfg = HouseCfg::getInstance().getHouseTypeInfo(iter->second->m_houseType);
		if (!hCfg) {
			continue;
		}
		nPlantRate += hCfg->n_PlantRate;
	}
	return nPlantRate;
}

int HouseDataMgr::GetCityFarmRate() const
{
	int nFarmRate = 0;
	HouseDataMap::const_iterator iter = m_houseMap.begin();
	for (; iter != m_houseMap.end(); iter++)
	{
		if (iter->second->m_houseId < BUILD_CITY_ID_WALL) {
			continue;
		}

		auto&& hCfg = HouseCfg::getInstance().getHouseTypeInfo(iter->second->m_houseType);
		if (!hCfg) {
			continue;
		}
		nFarmRate += hCfg->n_FarmRate;
	}

	return nFarmRate;
}

int HouseDataMgr::GetCityMineRate() const
{
	int nMineRate = 0;
	{
		HouseDataMap::const_iterator iter = m_houseMap.begin();
		for (; iter != m_houseMap.end(); iter++)
		{
			if (iter->second->m_houseId < BUILD_CITY_ID_WALL) {
				continue;
			}

			auto&& hCfg = HouseCfg::getInstance().getHouseTypeInfo(iter->second->m_houseType);
			if (!hCfg) {
				continue;
			}
			nMineRate += hCfg->n_MineRate;
		}
	}
	{
		auto&& mapData = NpcProtectorDataMgr::getInstance()->getProtectorDataMap();

		auto&& iter = mapData.begin();
		for (; iter != mapData.end(); iter++)
		{
			if (!iter->second || iter->second->houseId <= 0 || iter->second->uuid == 0)
			{
				continue;
			}
			auto&& npcBadge = NpcBadgeCfg::getInstance().findNpcBadgeCfg(iter->second->itemid, iter->second->level);
			if (!npcBadge) {
				continue;
			}

			nMineRate += npcBadge->n_RateMine;
		}
	}
	return nMineRate;
}

int HouseDataMgr::GetCityShopRate() const
{
	int nShopRate = 0;
	HouseDataMap::const_iterator iter = m_houseMap.begin();
	for (; iter != m_houseMap.end(); iter++)
	{
		if (iter->second->m_houseId < BUILD_CITY_ID_WALL) {
			continue;
		}

		auto&& hCfg = HouseCfg::getInstance().getHouseTypeInfo(iter->second->m_houseType);
		if (!hCfg) {
			continue;
		}
		nShopRate += hCfg->n_ShopRate;
	}

	return nShopRate;
}

int HouseDataMgr::GetCityCraftRate() const
{
	int nCraftRate = 0;
	HouseDataMap::const_iterator iter = m_houseMap.begin();
	for (; iter != m_houseMap.end(); iter++)
	{
		if (iter->second->m_houseId < BUILD_CITY_ID_WALL) {
			continue;
		}

		auto&& hCfg = HouseCfg::getInstance().getHouseTypeInfo(iter->second->m_houseType);
		if (!hCfg) {
			continue;
		}
		nCraftRate += hCfg->n_CraftRate;
	}

	return nCraftRate;
}

int HouseDataMgr::GetCityBuildRate() const
{
	int nBuildRate = 0;
	HouseDataMap::const_iterator iter = m_houseMap.begin();
	for (; iter != m_houseMap.end(); iter++)
	{
		if (iter->second->m_houseId < BUILD_CITY_ID_WALL) {
			continue;
		}

		auto&& hCfg = HouseCfg::getInstance().getHouseTypeInfo(iter->second->m_houseType);
		if (!hCfg) {
			continue;
		}
		nBuildRate += hCfg->n_buildRate;
	}

	return nBuildRate;
}


