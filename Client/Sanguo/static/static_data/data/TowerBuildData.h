/*!
* \file ItemData.h
*
* \author ChenBk
* \date 12/01/2016
*
*
*/
#pragma once
#include "msg/MsgBuildTower.pb.h"
#include "../config/ItemCfg.h"
#include "../config/BuildingCfg.h"
#include <array>
#include <map>

using namespace NS;

enum {
	TOWER_BUILD_ROW_NUM = 20,
};

class TowerBuildingData
{
public:
	TowerBuildingData();
	virtual ~TowerBuildingData();

	bool initialize();
	void reset();

	NS::I64 GetUuid() { return m_uuid; }
	void SetUuid(NS::I64 uuid) { m_uuid = uuid; }

	bool initWithTemplateId(int itemId, bool bReset = false, bool bSetAttr = true);

public:
	NS::I64 m_uuid{ 0 };
	NS::int32 m_itemId{ 0 };
	NS::int32 m_Hp{ 0 };
	NS::int32 m_MaxHp{ 0 };
	int n_PosX{ 0 };
	int n_PosY{ 0 };
};

//TowerBuildMgr
//------------------------------------------------------------------
class TowerBuildMgr
{
public:
	typedef std::unordered_map<NS::I64/*uuid*/, std::shared_ptr<TowerBuildingData>> TowerBuildDataMap;					//所有数据, Key: UUID

public:
	TowerBuildMgr();
	virtual ~TowerBuildMgr();

	static TowerBuildMgr& getInstance();
	void terminate();

	void Clear() { m_towerBuildMap.clear(); }

	//add\remove
	bool addItem(std::shared_ptr<TowerBuildingData> itemData);
	bool removeItem(NS::I64 delUuid);

	std::shared_ptr<TowerBuildingData> findTowerBuildByUuid(NS::I64 uuid);
	std::shared_ptr<TowerBuildingData> findTowerBuildByPos(NS::int32 nPosX, NS::int32 nPosY);

	const TowerBuildDataMap& GetBuildingDatas() { return m_towerBuildMap; }
	int GetTowerDefenceProsperous() const;

private:
	TowerBuildDataMap m_towerBuildMap;
};
