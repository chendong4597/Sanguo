/*!
* \file CraftData.h
*
* \author xiaobowang;
* \date 2017/02/22
*
*
*/
#pragma once
#include <vector>
#include <set>
#include <memory>
#include "../config/CraftCfg.h"
#include <tuple>

class CraftTaskData
{

public:
	
	enum class State : int
	{
		NONE = -1,
		FREE = 0,	//
		BUSY,		//
		FINISH,		//
		CANCEL,
		TO_CRAFT,
	};

	NS::I64 m_protectorUUID{ 0 };

	NS::I64 m_finishTime{ 0 };

	NS::uint m_craftId{0};

	NS::uint m_craftNum{0};

	//finish num when cancel
	int m_canelFinishNum{0};

	int getImmediateNeedCash();

	State getStates();

	const CraftCfg::Craft* m_pCfgCraft{};
};

class CraftDataMgr
{
public:

	//{taskIndex : craftTaskData}
	using CraftTaskDataMap = std::map<NS::I64, std::shared_ptr<CraftTaskData>>;

	static CraftDataMgr& getInstance();

	void addCraftTaskData(std::shared_ptr<CraftTaskData> pCraftTaskData);
	void updateCraftTaskData(std::shared_ptr<CraftTaskData> pCraftTaskData);
	std::shared_ptr<CraftTaskData> findCraftTaskData(NS::I64 protectorUUID);
	void removeCraftTaskData(NS::I64 protectorUUID);
	void clearCraftTaskData();

	const CraftTaskDataMap& getCraftTaskDataMap() { return m_craftTaskDataMap; }

private:

	CraftDataMgr();
	CraftTaskDataMap m_craftTaskDataMap{};
};
