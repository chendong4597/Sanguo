
#include "stdafx.h"
#include "StoreData.h"

using namespace NS;
using namespace MsgPB;


//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------

StoreDataMgr::StoreDataMgr()
{

}

StoreDataMgr::~StoreDataMgr()
{

}


StoreDataMgr& StoreDataMgr::getInstance()
{
	static StoreDataMgr s_storeDataMgr;
	return s_storeDataMgr;
}

void StoreDataMgr::terminate()
{
	Clear();
}

void StoreDataMgr::AddStoreData(NS::I64 uuid, std::vector<NS::int32> vecInvenroty)
{
	m_storeMap[uuid] = vecInvenroty;
}

NS::int32 StoreDataMgr::GetInventoryByUuid(NS::I64 uuid, NS::int32 nIdx)
{
	auto&& iter = m_storeMap.find(uuid);
	if (iter == m_storeMap.end()) {
		return -1;
	}
	if (nIdx < iter->second.size()) {
		return iter->second[nIdx];
	}

	return -1;
}


