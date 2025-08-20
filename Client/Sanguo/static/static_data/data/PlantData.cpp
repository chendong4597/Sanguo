
#include "stdafx.h"
#include "PlantData.h"

using namespace NS;
using namespace MsgPB;

PlantData::PlantData()
{

}

PlantData::~PlantData()
{

}

bool PlantData::initialize()
{
	return true;
}


void PlantData::reset()
{
}

bool PlantData::initWithPlantTypeId(int plantTpyId, bool bReset)
{
	if (bReset)
	{
		this->reset();
	}
	m_pPlantTpy = PlantCfg::getInstance().findPlantType(plantTpyId);
	if (!m_pPlantTpy)
	{
		return false;
	}
	m_plantTpyId = plantTpyId;
	return true;
}


//------------------------------------------------------------------------------------------------
// TowerBuildMgr
//------------------------------------------------------------------------------------------------

PlantDataMgr::PlantDataMgr()
{

}

PlantDataMgr::~PlantDataMgr()
{

}


PlantDataMgr& PlantDataMgr::getInstance()
{
	static PlantDataMgr s_plantDataMgr;
	return s_plantDataMgr;
}

bool PlantDataMgr::addItem(std::shared_ptr<PlantData> plantData)
{
	if (plantData == nullptr) return false;

	// add ItemData
	m_plantMap[plantData->m_uuid] = plantData;

	return true;
}

bool PlantDataMgr::removeItem(NS::I64 delUuid)
{
	PlantDataMap::iterator delIt = m_plantMap.find(delUuid);
	if (delIt != m_plantMap.end())
	{
		// Del ItemData
		m_plantMap.erase(delIt);

		return true;
	}

	return false;
}

NS::I64 PlantDataMgr::RemovePlantXY(int posX, int posY)
{
	NS::I64 nUuId = 0;
	PlantDataMap::iterator it = m_plantMap.begin();
	for (; it != m_plantMap.end() ; it++)
	{
		if (it->second->n_PosX == posX && it->second->n_PosY == posY) {
			nUuId = it->second->m_uuid;
			m_plantMap.erase(it);
			break;
		}
	}
	return nUuId;
}

std::shared_ptr<PlantData> PlantDataMgr::findPlantByUuid(NS::I64 uuid)
{
	PlantDataMap::iterator it = m_plantMap.find(uuid);
	if (it != m_plantMap.end())
	{
		return it->second;
	}

	return nullptr;
}

int PlantDataMgr::getCurPlantNumByProtectorId(NS::I64 protectorId)
{
	int nNum = 0;
	for (auto&& data : m_plantMap)
	{
		if (data.second->m_protectorId == protectorId) {
			nNum++;
		}
	}
	return nNum;
}

std::shared_ptr<PlantData> PlantDataMgr::findPlantByBlockId(int nBlockId)
{
	for (auto&& data : m_plantMap)
	{
		if (data.second->n_PosX + data.second->n_PosY*PLANT_ROW_NUM == nBlockId) {
			return data.second;
		}
	}
	return nullptr;
}

int PlantDataMgr::GetPlantProsperous() const
{
	int nProsperous = 0;
	for (auto&& data : m_plantMap)
	{
		auto&& pCfg = PlantCfg::getInstance().findPlantType(data.second->m_plantTpyId);
		if (!pCfg) {
			continue;
		}
		nProsperous += pCfg->n_Prosperous;
	}
	return nProsperous;
}


