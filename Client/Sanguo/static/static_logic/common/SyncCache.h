/*!
* \file SyncCache.h
*
* \author zhichenxia
* \date 2018/6/5
*
*
*/
#pragma once
#include "BaseTypeDefine.h"
#include <msg/MsgType.pb.h>
#include <unordered_map>




NS_BEGIN


class SyncItem
{
public:
	virtual ~SyncItem(void) = default;

public:
	//virtual void write_to_message(MsgPB::MsgSyncClientLocalDataC2S& msg) = 0;
};


//namespace
//{
//	using ::google::protobuf::FieldDescriptor;
//	using ::google::protobuf::Message;
//
//	void write_to_field(int32 value, const FieldDescriptor* pField, Message* pMessage)
//	{
//		pMessage->GetReflection()->SetInt32(pMessage, pField, value);
//	}
//
//	void write_to_field(NS::I64 value, const FieldDescriptor* pField, Message* pMessage)
//	{
//		pMessage->GetReflection()->SetInt64(pMessage, pField, value);
//	}
//
//	void write_to_field(std::string value, const FieldDescriptor* pField, Message* pMessage)
//	{
//		pMessage->GetReflection()->SetString(pMessage, pField, value);
//	}
//}


template <typename T> class SyncItemT final : public SyncItem
{
public:
	SyncItemT(const tstring& key, T value)
		: m_key(key)
		, m_value(value)
	{}

private:
	//virtual void write_to_message(MsgPB::MsgSyncClientLocalDataC2S& msg) override final
	//{
	//	write_to_field(m_value, msg.GetDescriptor()->FindFieldByName(m_key), &msg);
	//}

private:
	tstring m_key;
	T m_value;
};


class SyncCache final
{
	SyncCache(void) = default;

public:
	static SyncCache& getInstance(void);
	~SyncCache(void);

	inline void setMapId(int32 mapId) { m_mapId = mapId; }
	int32 getMapId(void) const { return m_mapId; }

	void clear(void);	// 仅清除内存中的数据。

	bool empty(void) const;
	inline bool isChanged(void) const { return m_changed; }

	void setLoginTime(NS::I64 loginTime);
	inline NS::I64 getOriginLoginTime(void) const noexcept { return m_originLoginTime; }

public:
	template <typename T> void set(const tstring& key, T value) { setItem(key, new SyncItemT<T>(key, value)); }

	//void setMapData(int32 x, int32 y, uint32 data, int debugInfo, bool bForceSync = false);
	//void setMapDataWaterFog(int32 x, int32 y, uint32 data);
	//void setProtectorData(const MsgPB::MsgProtectorData& data);
	//void setAchieveData(const MsgPB::MsgAchieveData& data);
	//void setPlayerEventTask(const MsgPB::MsgTaskData& data);
	//void setTowerBuildData(const MsgPB::MsgSyncTowerBuildData& data);
	//void setPlantData(const MsgPB::MsgSyncPlantUserData& data);
	//void setTriggerProbabilityData(const MsgPB::MsgTriggerProbabilityData& data);
	//void setSurvivalEventFlagData(const MsgPB::MsgLSSurvivalEventFlagData& data);
	//void setPriBuildData(const MsgPB::MsgSyncBuildData& data);

	//void setItemData(const MsgPB::MsgItemCommonData& data);
	//void addMobKill(int32 typeId);

	//void setPetData(const MsgPB::MsgPetData& data);
	//void setSoulBoxData(const MsgPB::MsgSoulData& data);
	//void setFeedSpeedData(const MsgPB::MsgSyncFeedingProductSpeed& data);
	//void setNpcDakData(const MsgPB::MsgNpcDakData& data);

	//void setUserBuffData(const MsgPB::MsgUserBuffData& data);

private:
	void setItem(const tstring& key, SyncItem* pItem);

	//bool isNotNeedCache() const;

private:
	bool m_changed{ false };

	std::unordered_map<tstring, SyncItem*> m_map;
	NS::I64 m_loginTime{}, m_originLoginTime{};
	int32 m_mapId{};
};


NS_END
