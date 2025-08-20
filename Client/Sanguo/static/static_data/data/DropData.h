/*!
* \file DropData.h
*
* \author ChenBk
* \date 12/01/2016
*
*
*/
#pragma once
#include <baseutil.h>
#include <memory.h>
#include "role/Player.h"
#include "msg/MsgPacketDrop.pb.h"
#include <Cpp11EventSystem.h>


class PlayerMgr;

enum EDROP_FROM_TYPE
{
	EDROP_FROM_NO = -1, //
	EDROP_FROM_NONE = 0,	//零值
	EDROP_FROM_BLOCK = 1,	//基础块
	EDROP_FROM_BUILD = 2,	//建筑
	EDROP_FROM_PLANT = 3,	//植物
	EDROP_FROM_MONSTER = 4,	//怪物
	EDROP_FROM_OTHER = 5,	//其他
	EDROP_FROM_INTERACTIVE = 6, //弱联网互动效果掉落
	EDROP_FROM_REMOTE_INTERACTIVE = 7, //强联网互动效果掉落
	EDROP_FROM_REMOTE_MONSTER = 8, //强联网怪物掉落
	EDROP_FROM_LUA_FUNCTION = 9, //LUA脚本特殊掉落
	EDROP_FROM_TOWERTOMBSTONE = 10, //坟墓奖励
};

enum EDrop_State
{
	EDROP_NONE = 0,
	EDROP_DROPING = 1,		//掉落中
	EDROP_PICKUPING = 2,	//拾取中
	EDROP_WAIT_PICKUP = 3,	//等待拾取
	EDROP_DELAY_PICK = 4,  //宝箱开出的物品，不能马上拾取需要延迟一段时间再拾取
};

class DropInfoItemData
{
public:
	NS::I64 obj_id{ 0 }; //拾取时,用这个id标识掉落的对象
	NS::I64 m_iggId;
	NS::int32 m_mapId;
	NS::int32 data_id{0}; //物品类型
	NS::int32 count{0}; //数量 
	bool m_isWeapon{ false };
	bool m_isArm{false};
	bool m_isPriveItem{ true };
	EDrop_State m_state{ EDROP_NONE };
	EDROP_FROM_TYPE m_FromType{ EDROP_FROM_NONE };
	NS::int32 m_nFromDataId{ 0 };
	NS::int32 m_nFromDataId_extra{ 0 };
	glm::ivec2 m_endMap{ 0, 0 };
	glm::ivec2 m_startMap{ 0, 0 };
	bool m_isMagicBoxDrop{false};		//for magic box drop
	void setCoord(glm::ivec2 pos)
	{
		m_endMap = pos;
	};

	const glm::ivec2& getCoord() const
	{
		return m_endMap;
	}

	const glm::ivec2& getStartCoord() const
	{
		return m_startMap;
	}

	const NS::tstring& getPiceId();

	void setState(EDrop_State state)
	{
		m_state = state;
	};

	EDrop_State getState() const
	{
		return m_state;
	}
	bool getIsCanGet()
	{
		return m_iggId == 0 || PlayerMgr::getInstance().isMyself(m_iggId);
	}
};

struct dropWeaponInfo
{
	int m_skillId;
	NS::I64 m_itemId;
};

typedef std::map<NS::I64, DropInfoItemData> MP_DROP_INFO;

typedef std::map<NS::I64, MsgPB::PBDropInfo> MP_DROP;
typedef std::map<NS::int32, dropWeaponInfo> MAP_PVP_EQUIP_INFO;

class HandlerPvpWeaponEvent : public NS::Event
{
public:
	HandlerPvpWeaponEvent() {};
	virtual ~HandlerPvpWeaponEvent(){};

public:
	NS::int32 m_selIndex{ -1 };
	NS::I64 m_itemId{0};
};

class DropDataMgr
{
public:

public:
	DropDataMgr();
	virtual ~DropDataMgr();

	static DropDataMgr& getInstance();
	void terminate();

	//add\remove
	bool addItem(MsgPB::PBDropInfo dropData);
	bool removeItem(NS::I64 delUuid);
	const MP_DROP& getDropMapData()const { return m_mapDrop; };
	MsgPB::PBDropInfo* getDropItem(NS::I64 delUuid);
private:
	MP_DROP m_mapDrop{};
};