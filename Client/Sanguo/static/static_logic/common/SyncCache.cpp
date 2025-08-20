/*!
* \file SyncCache.cpp
*
* \author zhichenxia
* \date 2018/6/5
*
*
*/
#include "SyncCache.h"
#include "MsgMgr.h"
#include "../common/MessageQueue.h"
#include "stdafx.h"

using namespace MsgPB;


NS_BEGIN


SyncCache& SyncCache::getInstance(void)
{
	static SyncCache s;
	return s;
}


SyncCache::~SyncCache(void)
{
	for (const auto& v : m_map)
		delete v.second;
}


//bool SyncCache::isNotNeedCache() const
//{
//	return false;
//}




void SyncCache::clear(void)
{
	for (const auto& v : m_map)
		delete v.second;
	m_map.clear();

	m_changed = false;
}


bool SyncCache::empty(void) const
{
	//if (isNotNeedCache()) return true;
	return m_map.empty();
}


void SyncCache::setLoginTime(NS::I64 loginTime)
{
	m_originLoginTime = loginTime;
	m_loginTime = loginTime - std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}


void SyncCache::setItem(const tstring& key, SyncItem* pItem)
{
	//if (isNotNeedCache())
	//{
	//	delete pItem;
	//	return;
	//}

	auto result = m_map.insert(std::make_pair(key, pItem));
	if (!result.second)
	{
		delete result.first->second;
		result.first->second = pItem;
	}

	m_changed = true;
}


class SyncItem_MapData final : public SyncItem
{
public:
	SyncItem_MapData(int32 x, int32 y, uint32 data, bool bIsWaterFog, int debugInfo)
		: m_x(x)
		, m_y(y)
		, m_data(data)
		, m_bIsWaterFog(bIsWaterFog)
		, m_debugInfo(debugInfo)
	{}

private:
	//virtual void write_to_message(MsgSyncClientLocalDataC2S& msg) override final
	//{
	//	auto pSub = msg.add_blockgrids();
	//	pSub->set_x(m_x);
	//	pSub->set_y(m_y);
	//	pSub->set_data(m_data);
	//	pSub->set_iswaterfog(m_bIsWaterFog);
	//	pSub->set_debug_info(m_debugInfo);
	//}

private:
	int32 m_x;
	int32 m_y;
	uint32 m_data;
	bool m_bIsWaterFog{};
	int m_debugInfo{};
};

NS_END
//void SyncCache::setMapData(int32 x, int32 y, uint32 data, int debugInfo, bool bForceSync/* = false*/)
//{
//	if (isNotNeedCache())
//	{
//		return;
//	}
//
//	//if (MapDataMgr::getInstance().isForbidMapSyncCache())
//	//{
//	//	return;
//	//}
//
//	setItem(fmt::format("Map[{0}, {1}]", x, y), new SyncItem_MapData(x, y, data, false, debugInfo));
//
//	if (bForceSync)
//	{
//		//TODO 强制同步缓存
//		MsgProtectorClientSyncRectTimeC2S msg;
//		msg.set_uuid(-1);
//		NS::MsgMgr::getInstance().sendMessage(_MSG_PROTECTOR_CLIENT_SYNC_RECT_TIME_C2S, &msg);
//	}
//
//}

//void SyncCache::setMapDataWaterFog(int32 x, int32 y, uint32 data)
//{
//	if (isNotNeedCache())
//	{
//		return;
//	}
//
//	//if (MapDataMgr::getInstance().isForbidMapSyncCache())
//	//{
//	//	return;
//	//}
//
//	setItem(fmt::format("MapWaterFog[{0}, {1}]", x, y), new SyncItem_MapData(x, y, data, true, 0));
//}


//class SyncItem_ItemData final : public SyncItem
//{
//public:
//	void push(const MsgItemCommonData& data)
//	{
//		m_list.push_back(data);
//	}
//
//private:
//	//virtual void write_to_message(MsgSyncClientLocalDataC2S& msg) override final
//	//{
//	//	for (auto it = m_list.begin(); it!= m_list.end(); ++it)
//	//		(*msg.add_items()) = *it;
//	//}
//
//private:
//	std::list<MsgItemCommonData> m_list;
//};

//class SyncSigleItem_ItemData final : public SyncItem
//{
//public:
//	SyncSigleItem_ItemData(const MsgItemCommonData& data)
//		: m_data(data)
//	{}
//
//private:
//	virtual void write_to_message(MsgSyncClientLocalDataC2S& msg) override final
//	{
//		(*msg.add_items()) = m_data;
//	}
//
//private:
//	MsgItemCommonData m_data;
//};

//void SyncCache::setItemData(const MsgItemCommonData& data)
//{
//	//LOGI("========= DEBUG: SyncCache::setItemData({0} -> {1})", data.pos(), data.uuid());
//
//	if (isNotNeedCache()) return;
//
//	NS::tstring itemKey = fmt::format("Item[{0}]", data.uuid());
//	
//	auto it = m_map.find(itemKey);
//	if (it == m_map.end())
//	{
//		setItem(itemKey, new SyncSigleItem_ItemData(data));
//	}
//	else 
//	{
//		delete it->second;
//		it->second = new SyncSigleItem_ItemData(data);
//	}
//}
//
//class SyncItem_ProtectorData final : public SyncItem
//{
//public:
//	SyncItem_ProtectorData(const MsgProtectorData& data)
//		: m_data(data)
//	{}
//
//private:
//	virtual void write_to_message(MsgSyncClientLocalDataC2S& msg) override final
//	{
//		(*msg.add_protectors()) = m_data;
//	}
//
//private:
//	MsgProtectorData m_data;
//};
//void SyncCache::setProtectorData(const MsgProtectorData& data)
//{
//	if (isNotNeedCache())
//	{
//		return;
//	}
//	setItem(fmt::format("Protector[{0}]", data.uuid()), new SyncItem_ProtectorData(data));
//}


//class SyncItem_AchieveData final : public SyncItem
//{
//public:
//	SyncItem_AchieveData(const MsgAchieveData& data)
//		: m_data(data)
//	{}
//
//private:
//	virtual void write_to_message(MsgSyncClientLocalDataC2S& msg) override final
//	{
//		(*msg.add_achievedata()) = m_data;
//	}
//
//private:
//	MsgAchieveData m_data;
//};
//void SyncCache::setAchieveData(const MsgPB::MsgAchieveData& data)
//{
//	setItem(fmt::format("Achieve[{0}]", data.uuid()), new SyncItem_AchieveData(data));
//}

// SyncItem_TaskData
//class SyncItem_TaskData final : public SyncItem
//{
//public:
//	SyncItem_TaskData(const MsgPB::MsgTaskData& data)
//		: m_data(data)
//	{}
//
//private:
//	virtual void write_to_message(MsgSyncClientLocalDataC2S& msg) override final
//	{
//		(*msg.add_task_data()) = m_data;
//	}
//
//private:
//	MsgPB::MsgTaskData m_data;
//};
////void SyncCache::setPlayerEventTask(const MsgPB::MsgTaskData& data)
//{
//	setItem(fmt::format("TaskData[{0}]", data.task_id()), new SyncItem_TaskData(data));
//}

//class SyncItem_TowerBuildData final : public SyncItem
//{
//public:
//	SyncItem_TowerBuildData(const MsgSyncTowerBuildData& data)
//		: m_data(data)
//	{}
//
//private:
//	virtual void write_to_message(MsgSyncClientLocalDataC2S& msg) override final
//	{
//		(*msg.add_towerbuild_data()) = m_data;
//	}
//
//private:
//	MsgSyncTowerBuildData m_data;
//};
//void SyncCache::setTowerBuildData(const MsgPB::MsgSyncTowerBuildData& data)
//{
//	if (isNotNeedCache())
//	{
//		return;
//	}
//
//	//if (MapDataMgr::getInstance().isForbidMapSyncCache())
//	//{
//	//	return;
//	//}
//
//	setItem(fmt::format("TowerBuild[{0}]", data.uuid()), new SyncItem_TowerBuildData(data));
//}


//class SyncItem_PlantData final : public SyncItem
//{
//public:
//	SyncItem_PlantData(const MsgSyncPlantUserData& data)
//		: m_data(data)
//	{}
//
//private:
//	virtual void write_to_message(MsgSyncClientLocalDataC2S& msg) override final
//	{
//		(*msg.add_plant_data()) = m_data;
//	}
//
//private:
//	MsgSyncPlantUserData m_data;
//}; 
//void SyncCache::setPlantData(const MsgPB::MsgSyncPlantUserData& data)
//{
//	if (isNotNeedCache())
//	{
//		return;
//	}
//
//	//if (MapDataMgr::getInstance().isForbidMapSyncCache())
//	//{
//	//	return;
//	//}
//
//	setItem(fmt::format("Plant[{0}]", data.plant_id()), new SyncItem_PlantData(data));
//}

//
//class SyncItem_TriggerProbabilityData final : public SyncItem
//{
//public:
//	SyncItem_TriggerProbabilityData(const MsgTriggerProbabilityData& data)
//		: m_data(data)
//	{}
//
//private:
//	virtual void write_to_message(MsgSyncClientLocalDataC2S& msg) override final
//	{
//		(*msg.add_trigger_probability_data()) = m_data;
//	}
//
//private:
//	MsgTriggerProbabilityData m_data;
//};
//void SyncCache::setTriggerProbabilityData(const MsgPB::MsgTriggerProbabilityData& data)
//{
//	setItem(fmt::format("TriggerProbability[{0}]", data.uuid()), new SyncItem_TriggerProbabilityData(data));
//}

//
//class SyncItem_SurvivalEventFlag final : public SyncItem
//{
//public:
//	SyncItem_SurvivalEventFlag(const MsgLSSurvivalEventFlagData& data)
//		: m_data(data)
//	{}
//
//private:
//	virtual void write_to_message(MsgSyncClientLocalDataC2S& msg) override final
//	{
//		(*msg.add_survival_event_flag_data()) = m_data;
//	}
//
//private:
//	MsgLSSurvivalEventFlagData m_data;
//};
//void SyncCache::setSurvivalEventFlagData(const MsgPB::MsgLSSurvivalEventFlagData& data)
//{
//	setItem(fmt::format("SurvivalEventFlag[{0}]", data.idx()), new SyncItem_SurvivalEventFlag(data));
//}

//
//class SyncItem_PriBuildData final : public SyncItem
//{
//public:
//	SyncItem_PriBuildData(const MsgPB::MsgSyncBuildData& data)
//		: m_data(data)
//	{}
//
//private:
//	virtual void write_to_message(MsgSyncClientLocalDataC2S& msg) override final
//	{
//		(*msg.add_build_data()) = m_data;
//	}
//
//private:
//	MsgSyncBuildData m_data;
//};
//void SyncCache::setPriBuildData(const MsgPB::MsgSyncBuildData& data)
//{
//	if (isNotNeedCache())
//	{
//		return;
//	}
//
//	if (MapDataMgr::getInstance().isForbidMapSyncCache())
//	{
//		return;
//	}
//
//	setItem(fmt::format("PriBuildData[{0}]", data.uuid()), new SyncItem_PriBuildData(data));
//}


//class SyncItem_MobKillCount final : public SyncItem
//{
//public:
//	SyncItem_MobKillCount(int32 typeId)
//		: m_typeId(typeId)
//	{}
//
//	void add(void)
//	{
//		++m_count;
//	}
//
//private:
//	virtual void write_to_message(MsgSyncClientLocalDataC2S& msg) override final
//	{
//		auto p = msg.add_mobkillcounts();
//		p->set_dataid(m_typeId);
//		p->set_count(m_count);
//	}
//
//private:
//	int32 m_typeId{};
//	int32 m_count{};
//};
//void SyncCache::addMobKill(int32 typeId)
//{
//	if (isNotNeedCache())
//	{
//		return;
//	}
//
//	if (MapDataMgr::getInstance().getCurMapType() == MapDataMgr::MapType::TowerChallenge || MapDataMgr::getInstance().getCurMapType() == MapDataMgr::MapType::SquadPvp)
//	{
//		return;
//	}
//
//	auto p = new SyncItem_MobKillCount(typeId);
//	auto result = m_map.insert(std::make_pair(fmt::format("MobKill[{0}]", typeId), p));
//	if (!result.second) delete p;
//	dynamic_cast<SyncItem_MobKillCount*>(result.first->second)->add();
//
//	m_changed = true;
//}





