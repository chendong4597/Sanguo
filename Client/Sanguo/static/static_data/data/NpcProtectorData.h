/*!
* \file NpcProtectorData.h
*
* \author LiQiangqiang;
* \date 2017/09/2
*
*
*/
#pragma once
#include "msg/MsgProtector.pb.h" 
#include "msg/MsgGameDef.pb.h"
#include "../common/GameDefine.h"
#include "../common/CommonDataTypeDef.h"
//#include "msg/MsgTowerPVP.pb.h"
#include "role/Role.h"
#include "ItemData.h"


enum E_PROTECTOR_ACHIEVE_EFFECT
{
	PAE_PLANT_GROW_SPEEDUP,			// 浇水植物加速百分比 
	PAE_MAGIC_INJECTION_NEED_ENERGY,// 附魔减少能量消耗百分比
	PAE_TABLE_CRAFT_TIME,			// 工作台合成时间减少百分比
	PAE_ANVIL_CRAFT_TIME,			// 锻造台合成时间减少百分比
	PAE_SELL_ITEM_TO_STORE,			// 物品卖给商店时增加金额百分比
	PAE_BUY_ITEM_FROM_STORE,		// 商店购买时减少售价百分比
	PAE_NPATASK_FROM_STOER,			//商会奖励加成;
	PAE_ANIMALSPEED_FORM_FEED,		//小动物产出时长缩短;
	PAE_ANIMALPRO_DOUBLE,			//小动物产出翻倍概率;
	PAE_ANIMAL_HUG_ADD,				//小动物耐饿增长;
};

class NpcProtectorData
{
public:
	NS::I64 uuid{};	// uuid
	NS::int32 m_ownerMapObjId{}; //拥有者
	NS::int32 mapObjectID{}; 	// 地圖物件ID;
	NS::int32 templateid{};	// Mob樣版ID;
	NS::int32 itemid{};	// 徽章道具樣版ID;	
	NS::int32 level{};		//徽章等級;	
	NS::int32 exp{};		//徽章等級;	
	NS::int32 houseId{};	//住房ID
	NS::int32 hp{ 0 };
	NS::int32 mp{ 0 };
	MsgPB::PROTECTOR_STATUS status;	// 徽章狀態;	
	NS::int32 rect_day{ 0 };
	NS::I64 rent_time{ 0 };
	MsgPB::PROTECTOR_LESS_THAN_REQUIRED roomConditionUnmatchedFlag;	// 房间条件不满足flag;	
	NS::int32 comfort{}; // 舒適度等級;
	NS::I64 equipId{}; // 盔甲
	NS::I64 weaponId1{}; // 武器
	NS::I64 weaponId2{}; // 辅助武器
	NS::I64 follow_uuid{}; // 跟随将领ID;
	
	bool m_haveCreateNpc{ false };
	bool m_isCanCreateAtSpState{ false };
	bool m_isFromBirthMob{ false };

	//是不是守方玩家的徽章
	bool m_bIsDef{false};

public:
	void setComfor(int value);
	void setRoomConditionFlag(MsgPB::PROTECTOR_LESS_THAN_REQUIRED flg);
	void setOwnerMapObjId(NS::I64 id);
	void setMapObjId(int id);
	void setHaveCreateNpc(bool haveCreate);
	void setLevel(int value);
	void setHouseId(int value);
	void setHp(int value);
	int getHp() const { return hp; };
	void setIsCanCreateSpNpc(bool value);

	void RemoveProtectorData();
	bool CanAppear();
	bool isAtRoom();
	bool ControllableStatus();
	void setState(MsgPB::PROTECTOR_STATUS state);
	NS::I64 getEquipId();
	void setEquipId(NS::I64 nEquipid);
	void setWeapon1(NS::I64 nWeapon1);
	void setWeapon2(NS::I64 nWeapon2);
	void setFollowUUID(NS::I64 n64FollowUUID);
	bool CanBattle();
};

class NpcProtectorDataMgr
{

public:
	typedef std::map<NS::I64/*uuid*/, std::shared_ptr<NpcProtectorData>> Uuid2NpcProtectorDataMap;
	typedef std::map<glm::ivec2/*coord*/, std::shared_ptr<NpcProtectorData>> Coord2NpcProtectorDataMap;
	typedef std::map<NS::int32/*npcMapObjId*/, std::shared_ptr<NpcProtectorData>> NpcMapObjId2NpcProtectorDataMap;

public:
	NpcProtectorDataMgr(){};
	~NpcProtectorDataMgr() {};

	static NpcProtectorDataMgr* getInstance();

	void Clear();

public:
	bool syncNpcProtectorData(const MsgPB::MsgProtectorSyncS2C& msg);
	bool addNpcProtectorData(const MsgPB::MsgProtectorUpdateS2C& msg);
	bool updateNpcProtectorData(const MsgPB::MsgProtectorUpdateS2C& msg);
	void postBadgeDataToBadgeComp(bool bClear = false);

	void postNpcBadgeStateChangedS2CEvent(std::shared_ptr<NpcProtectorData> pData, bool bClear = false);

	std::shared_ptr<NpcProtectorData> createNpcProtectorDataFromMsg(MsgPB::MsgProtectorDataToClient &msgData,bool isCanSetLastState = true);

	bool removeNpcProtectorData(const MsgPB::MsgProtectorRemoveS2C& msg);
	void removeAllNpcProtectorData();
	NS::tstring parseConditionUnMatch(const NpcProtectorData& data);

	std::shared_ptr<NpcProtectorData> getNpcProtectorDataByNpcMapObjId(const NS::int32 nMapObjId);
	std::shared_ptr<NpcProtectorData> getNpcProtectorDataByUuid(const NS::I64 nUuid);
	std::shared_ptr<NpcProtectorData> getNpcProtectorDataByDefId(const NS::int32 nDefId);
	std::shared_ptr<NpcProtectorData> getNpcProtectorDataByBadgeBuildType(const NS::int32 BadgeItemid);
	const Uuid2NpcProtectorDataMap& getProtectorDataMap() { return m_uuid2NpcProtectorDataMap; };
	bool isHaveBadgeData(NS::int32 nDefId);
	int GetProtectorProsperous() const;

	bool isHaveNpc(int npcDefId);

	void createDefPlayerProtectorDataToMap(MsgPB::MsgProtectorDataToClient &msgData);
	void createDefPlayerProtectorData(const MsgPB::MsgProtectorSyncS2C &msg);
	void createSegDeferProtectorData(MsgPB::MsgProtectorDataToClient &msg);
	void clearDefPlayerProtectorData();
	void clearSegDefPlayerProtectorData();
	std::shared_ptr<NpcProtectorData> createDeferNpcProtectorDataFromMsg(MsgPB::MsgProtectorDataToClient &msgData, Uuid2NpcProtectorDataMap &mapData,bool isPvPSeg = false);
	std::shared_ptr<NpcProtectorData> getDefNpcProtectorDataByUuid(const NS::I64 nUuid);
	void setIsCanBirthDefNpc(bool isCanBegin) { m_isCanBirthDefNpc = isCanBegin; };
	const Uuid2NpcProtectorDataMap getDefProtectorDataMap() { return m_defPlayerProtectorDataMap; };
	std::shared_ptr<NpcProtectorData> getDefNpcProtectorDataByDefId(const NS::int32 nDefId);
	std::shared_ptr<NpcProtectorData> getDefNpcProtectorDataByObjId(const NS::I64 objId);
	std::shared_ptr<NpcProtectorData> getSegDefNpcProtectorDataByUuid(const NS::I64 nUuid);
	std::shared_ptr<NpcProtectorData> getSegDefNpcProtectorDataByDefId(const NS::int32 nDefId);
	std::shared_ptr<NpcProtectorData> getSegDefNpcProtectorDataByObjId(NS::int32 nMapObjId);

	float getNpcAttrValue(const AttrMap &mapAtter,MsgPB::ROLE_EFFECT_TYPE type);
	void getNpcAtters(AttrMap &mapAtter, int npcDefId, std::shared_ptr<NpcProtectorData> data = nullptr,\
		int playerLevel = -1, int badgeLv = 0, bool bCalEssence = false, bool bCalLv = false , \
		bool bCalEquip = true , bool bCalFightBuff = false);


	bool isBadgeAlive(int badgeDefid);//参数传入徽章的ID
	bool isBadgeUnlock(int badgeDefid);// 参数传入徽章的ID
	bool isHasBadgeUnlock();

	bool isHasBadgeByType(int nType);

	bool isHasSameNpc(int itemId);

	//售卖打折
	int  GetSoldRate(NS::I64 uuid);

	// 查找所有的Npc数据
	std::shared_ptr<NpcProtectorData> findNpcProtectorDataByMapObjId(int nMapObjId);

private:
	Uuid2NpcProtectorDataMap m_uuid2NpcProtectorDataMap{};
	Uuid2NpcProtectorDataMap m_defPlayerProtectorDataMap{};
	Uuid2NpcProtectorDataMap m_pvpSegDeferProtectorDataMap{};
	bool m_isCanBirthDefNpc{ false };
	std::vector<int> m_vecNeedShowNpcAtterType{};
};

