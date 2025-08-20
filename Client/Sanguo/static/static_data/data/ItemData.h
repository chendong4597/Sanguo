/*!
* \file ItemData.h
*
* \author ChenBk
* \date 12/01/2016
*
*
*/
#pragma once
#include "msg/MsgItem.pb.h"
#include "../config/ItemCfg.h"
#include "../config/ArmorCfg.h"
#include "../config/BuildingCfg.h"
#include "../config/PlantCfg.h"
#include "../config/WeaponInfoTypeCfg.h"
#include <array>
#include <map>

class Player;

enum class AttrType :int
{
	ATTR_All = -1,
	ATTR_BASE,
	ATTR_EFFECT,
	ATTR_STATICVAL,
	ATTR_BASE_TEMP,
	ATTR_EFFECT_TEMP,
};

enum PACK_TYPE {
	PACK_TYPE_NONE = -1,  //无需操作
	PACK_TYPE_BAG = 0,  //背包
	PACK_TYPE_DEPOT = 1,  //仓库
	PACK_TYPE_BODY = 2,  //身上
	PACK_TYPE_SHOP = 4,  //商店
};

enum PACK_MAX_NUM {
	PACK_MAX_NUM_BODY = 100,  //最多100个空间
};

enum PackSortType
{
	PackSort_None = 0,
	PackEquipBag = 1u << 0,
	PackSortBag = 1u << 0,
};


typedef std::vector<Effect> AttrVec;
typedef std::map<AttrType, AttrVec> AttrVecMap;


class ItemData
{
public:
	ItemData();
	virtual ~ItemData();

	bool initialize();
	void reset();

	static NS::I64 getDiffTimeWithNowTime(NS::I64 diffTime);

	bool initWithItemId(int itemId, bool bReset = false, bool bSetAttr = true);
	const ItemCfg::Item* getItemCfg() const { return m_pItem; }

	void setUuid(NS::I64 uuid){ m_uuid = uuid; }
	NS::I64 getUuid() const { return m_uuid; }

	void setSubType(MsgPB::ITEM_SUBTYPE nType);

	void setItemId(int id) { m_itemId = id; };
	int getItemId() const { return m_itemId; }

	void setDurable(int durable) { m_durable = durable; };
	int getDurable() const { return m_durable; }

	void setDurableMax(int durable) { m_durableMax = durable; };
	int getDurableMax() const { return m_durableMax; }

	void setCostDurable(int durable) { m_costDurable = durable; };
	int getCostDurable() const { return m_costDurable; }
	int getDurableRate() const;

	void setCount(int count){ m_count = count; }
	int getCount()const { return m_count; } 

	void setPos(int pos) { m_pos = pos; }
	int getPos()const { return m_pos; }

	bool isBadge() { return m_isBadge; }
	void setIsBadge(bool isBadge) { m_isBadge = isBadge; }

	int getMaxHeap() const { return m_pItem->m_maxHeap; };
	bool getCanHeap() const;

	MsgPB::PACKET_TYPE getPacketType()const { return m_packetType; }
	void setPacketType(MsgPB::PACKET_TYPE nType) { m_packetType = nType; }

	std::string GetAssertIcon();

	int getQualityGrade(void) const { return m_pItem->m_qualityGrade; }
	int getLevel() const { return m_pItem->n_use_lv;}

	const ArmorCfg::ArmorType * getArmorCfg() const { return m_pArmorType; };

	void initAttrEffectVec();
	const AttrVec& getAttrEffectVec(AttrType attrtype);
	void addAttrEffect(AttrType attrtype, const MsgPB::AttrEffect& attrEffect);
	void cleanAttrEffect(AttrType attrtype);
	void sumEffect(CEffectSum& effectSum, bool bATK_SP = true);		// 装备属性.  bATK_SP 表示是否计算攻击速度
	const AttrVecMap& getAttrMap() { return m_mapAttrVec; }
	float getAttrValue(Effect& tDataInfo);


public:
	bool IsInCooldown();
	bool IsHeroUnFullAttr();
public:
	NS::I64 m_uuid{ 0 };
	NS::I64 m_auctionCoolDown{0};
	int m_itemId{ 0 };
	MsgPB::ITEM_SUBTYPE m_subType{};
	int m_pos{ 0 };
	int m_count{ 0 };
	bool m_bCanShing{ false };
	bool m_inCooldown{ false };  // (ms)
	int m_cooldown{ 0 };  // (ms)
	NS::I64 m_countdownTime{ 0 };  // (minute)
	std::string m_name;
	NS::I64 m_expiredTime{ 0 };
	int m_durable{ 0 };
	int m_durableMax{ 0 }; // 服务器, 当前最大 Durable
	int m_costDurable{ 0 }; // 缓存要扣除的耐久.
	glm::ivec2 m_posMap{};
	float m_fWidth{ 0.f };
	float m_fHeight{ 0.f };
	int m_strengthen{ 0 };
	bool m_hide{ false };
	bool m_isBadge{ false };

	AttrVecMap m_mapAttrVec;
	ArmorCfg::SkillVec m_vAllSkill;

	MsgPB::PACKET_TYPE m_packetType{};
	const ItemCfg::Item * m_pItem{ nullptr };
	const ArmorCfg::ArmorType * m_pArmorType{ nullptr };
	//const BuildingCfg::BuildingType * m_pBuildingType{ nullptr };
	const PlantCfg::PlantType * m_pPlantSeed{ nullptr };

	const WeaponInfoTypeCfg::WeaponInfoTypeCfgInfo * m_pWeaponCfg{ nullptr };

	//-----------------------
	AttrVec M_EMPTY_ATTR{};
};

//ItemPacketMgr
//------------------------------------------------------------------
class ItemPacketMgr
{
public:
	typedef struct PacketInfoTag
	{
	public:
		MsgPB::PACKET_TYPE m_packetType{};
		int m_packetInit{ 0 };
		int m_packetMax{ 0 };
		NS::I64 m_nDeadlineStamp;
	} PacketInfo;

public:
	typedef std::unordered_map<NS::I64/*uuid*/, std::shared_ptr<ItemData>> ItemDataMap;					//所有数据, Key: UUID

	typedef std::unordered_map<NS::I64/*uuid  */, std::weak_ptr<ItemData>> WeakItemDataKeyUuidMap;			//类型背包容器, Key: uuid
	typedef std::unordered_map<NS::uint/*pos  */, std::weak_ptr<ItemData>> WeakItemDataKeyPosMap;			//类型背包容器, Key: Pos
	typedef std::multimap<NS::uint/*ItemId*/, std::weak_ptr<ItemData>> WeakItemDataKeyItemIdMutmap;			//类型背包容器, Key: ItemId
	typedef std::multimap<NS::uint/*TypeId*/, std::weak_ptr<ItemData>> WeakItemDataKeyTypeIdMutmap;			//类型背包容器, Key: TypeId
	typedef std::multimap<NS::uint/*SubType*/, std::weak_ptr<ItemData>> WeakItemDataKeySubTypeMutmap;		//类型背包容器, Key: SubType

	typedef std::unordered_map<NS::uint/*packet*/, PacketInfo> PacketInfoMap;								//背包的头信息数据 
	typedef std::vector<std::shared_ptr<ItemData>> ItemDataVec;

	typedef std::unordered_map<NS::uint, NS::I64> ItemMaterialMap;					//基础材料数据

public:
	ItemPacketMgr();
	virtual ~ItemPacketMgr();

	static ItemPacketMgr& getInstance();
	void terminate();

	void clearData();

	//add\remove
	bool addItem(std::shared_ptr<ItemData> itemData);
	bool removeItem(NS::I64 delUuid);

	WeakItemDataKeyPosMap findPacketMap(MsgPB::PACKET_TYPE packetType);

	std::shared_ptr<ItemData> findItemDataByUuid(NS::I64 uuid);
	std::shared_ptr<ItemData> findItemDataByPos(MsgPB::PACKET_TYPE packetType, int pos);
	std::shared_ptr<ItemData> findFirstItemDataItemId(MsgPB::PACKET_TYPE packetType, int itemId);
	int getPacketItemCount(MsgPB::PACKET_TYPE packetType, int itemId);
	WeakItemDataKeyUuidMap findPacketMapByTag(MsgPB::PACKET_TYPE packetType, const NS::tstring& strTag, int nSubType = 0);
	WeakItemDataKeyItemIdMutmap findPacketMapByItemId(MsgPB::PACKET_TYPE packetType, int itemId);
	WeakItemDataKeyPosMap findPacketMapByPos(MsgPB::PACKET_TYPE packetType, int bPos, int ePos);

	std::shared_ptr<ItemData> findFirstBagItemDataByEquipPos(ARMOR_EQUIP_POS pos);

	// Packet Info
	bool addPacketInfo(const PacketInfo* info);
	const PacketInfo* findPacketInfo(MsgPB::PACKET_TYPE packetType);
	bool checkPacketInfoMax(MsgPB::PACKET_TYPE packetType);	// 检查是否达到最大限值.
	int getLessPackNum();

	
	int getEquipPosByArmCfgPos(int armCfgPos);

	bool isWeaponByTempId(int id);
	int getEquipPosById(int id);
	bool isArmByTempId(int id);

	// Sort Function
	ItemDataVec& getSortBag(MsgPB::PACKET_TYPE Bag, PackSortType sort);
	bool sortBag(const std::shared_ptr<ItemData> lhs, const std::shared_ptr<ItemData> rhs);		// 背包排序，ID < ID

public:
	void SetMaterial(NS::uint unItemId, NS::I64 n64Num);
	NS::I64 GetMaterial(NS::uint unItemId);

	bool AddNewItemTips(NS::I64 uuid);

public:
	void ResetCooldown(int nItemId);
	int GetCooldown(int nItemId);
private:
	ItemDataMap m_itemDataMap;
	PacketInfoMap m_packetInfoMap;

	ItemDataVec m_retItemDataVec;	// 返回值 Vec		
	ItemDataVec m_retSortBagVec;	// 背包排序 Vec		

	ItemMaterialMap m_mapMaterials;

	std::map<NS::I64, bool> m_mapItemGets;
	std::map<int, NS::I64> m_mapCooldown;
};
