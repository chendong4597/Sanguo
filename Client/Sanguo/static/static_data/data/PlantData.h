/*!
* \file ItemData.h
*
* \author ChenBk
* \date 12/01/2016
*
*
*/
#pragma once
#include "msg/MsgPlant.pb.h"
#include "../config/ItemCfg.h"
#include "../config/PlantCfg.h"
#include <array>
#include <map>

using namespace NS;

enum {
	PLANT_ROW_NUM=6,
	PLANT_COLUM_NUM = 10,
};

class PlantData
{
public:
	PlantData();
	virtual ~PlantData();

	bool initialize();
	void reset();

	NS::I64 GetUuid() { return m_uuid; }
	void SetUuid(NS::I64 uuid) { m_uuid = uuid; }

	bool initWithPlantTypeId(int plantTpyId, bool bReset = false);

public:
	NS::I64 m_uuid{ 0 };
	NS::int32 m_plantTpyId{ 0 };
	int n_PosX{ 0 };
	int n_PosY{ 0 };
	NS::I64 m_beginTm{ 0 };
	NS::I64 m_growTm{ 0 };
	NS::I64 m_protectorId{ 0 };
	int m_growStage{ 0 };
	const PlantCfg::PlantType * m_pPlantTpy{ nullptr };
};

//TowerBuildMgr
//------------------------------------------------------------------
class PlantDataMgr
{
public:
	typedef std::unordered_map<NS::I64/*uuid*/, std::shared_ptr<PlantData>> PlantDataMap;					//所有数据, Key: UUID

public:
	PlantDataMgr();
	virtual ~PlantDataMgr();

	static PlantDataMgr& getInstance();
	void terminate();

	void Clear() { m_plantMap.clear(); }

	//add\remove
	bool addItem(std::shared_ptr<PlantData> itemData);
	bool removeItem(NS::I64 delUuid);
	NS::I64 RemovePlantXY(int posX , int posY);

	std::shared_ptr<PlantData> findPlantByUuid(NS::I64 uuid);
	std::shared_ptr<PlantData> findPlantByBlockId(int nBlockId);
	int getCurPlantNumByProtectorId(NS::I64 protectorId);

	const PlantDataMap& GetPlantDatas() { return m_plantMap; }
	int GetPlantProsperous() const;

private:
	PlantDataMap m_plantMap;
};
