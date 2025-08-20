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
#include "../config/StoreCfg.h"
#include <array>
#include <map>

using namespace NS;


//------------------------------------------------------------------
class StoreDataMgr
{
public:
	typedef std::map<NS::I64, std::vector<NS::int32>> StoreDataMap;					//所有数据, Key: UUID

public:
	StoreDataMgr();
	virtual ~StoreDataMgr();

	static StoreDataMgr& getInstance();
	void terminate();

	void Clear() { m_storeMap.clear(); }

	void AddStoreData(NS::I64 uuid, std::vector<NS::int32> vecInvenroty);

	NS::int32 GetInventoryByUuid(NS::I64 uuid, NS::int32 nIdx);
	const StoreDataMap& GetStoreDatas() { return m_storeMap; }

private:
	StoreDataMap m_storeMap;
};
