#include "stdafx.h"
#include "../config/TowerBuildingCfg.h"
#include "TowerBuildData.h"

using namespace NS;
using namespace MsgPB;

TowerBuildingData::TowerBuildingData()
{

}

TowerBuildingData::~TowerBuildingData()
{

}

bool TowerBuildingData::initialize()
{
	return true;
}


void TowerBuildingData::reset()
{
}

bool TowerBuildingData::initWithTemplateId(int itemId, bool bReset, bool bSetAttr)
{
	m_itemId = itemId;
	return true;
}


//------------------------------------------------------------------------------------------------
// TowerBuildMgr
//------------------------------------------------------------------------------------------------

TowerBuildMgr::TowerBuildMgr()
{

}

TowerBuildMgr::~TowerBuildMgr()
{

}


TowerBuildMgr& TowerBuildMgr::getInstance()
{
	static TowerBuildMgr s_buildDataMgr;
	return s_buildDataMgr;
}

bool TowerBuildMgr::addItem(std::shared_ptr<TowerBuildingData> itemData)
{
	if (itemData == nullptr) return false;

	// add ItemData
	m_towerBuildMap[itemData->m_uuid] = itemData;

	return true;
}

bool TowerBuildMgr::removeItem(NS::I64 delUuid)
{
	//ItemDataMap::iterator delIt = m_itemDataMap.find(delUuid);
	//if (delIt != m_itemDataMap.end())
	//{
	//	// Del ItemData
	//	m_itemDataMap.erase(delIt);

	//	return true;
	//}

	return false;
}

std::shared_ptr<TowerBuildingData> TowerBuildMgr::findTowerBuildByUuid(NS::I64 uuid)
{
	TowerBuildDataMap::iterator it = m_towerBuildMap.find(uuid);
	if (it != m_towerBuildMap.end())
	{
		return it->second;
	}

	return nullptr;
}

std::shared_ptr<TowerBuildingData> TowerBuildMgr::findTowerBuildByPos(NS::int32 nPosX, NS::int32 nPosY)
{
	for (auto&& iter = m_towerBuildMap.begin(); iter != m_towerBuildMap.end(); iter++)
	{
		if (iter->second->n_PosX == nPosX &&
			iter->second->n_PosY == nPosY) {
			return iter->second;
		}
	}
	return nullptr;
}

int TowerBuildMgr::GetTowerDefenceProsperous() const
{
	int nProsperous = 0;
	for (auto&& iter = m_towerBuildMap.begin(); iter != m_towerBuildMap.end(); iter++)
	{
		auto&& pBCfg = TowerBuildingCfg::getInstance().getTowerBuildingDefence(iter->second->m_itemId);
		if (!pBCfg) {
			continue;
		}
		nProsperous += pBCfg->n_Prosperous;
	}

	return nProsperous;
}


