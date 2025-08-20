/*!
 * \file HandlerDrop.h
 *
 * \author huangjiangping
 * \date 3/17/2017
 *
 * 
 */
#pragma once
#include <MsgHandler.h>
#include "BaseTypeDefine.h"
#include "msg/MsgPacketDrop.pb.h"
#include "msg/MsgPvPRandItem.pb.h"
#include "msg/MsgDungeon.pb.h"
#include <data/DropData.h>
#include "msg/MsgPVP.pb.h"


namespace MsgPB
{
	class MsgMobSpawnerG2C;
}


typedef std::vector<DropInfoItemData> TV_DROP;
class HandlerDrop : public NS::MsgHandler
{
	RTTI_DEF(HandlerDrop);
	DECLARE_EVENT_HOST();

public:
	static HandlerDrop& getInstance();

	HandlerDrop();
	virtual ~HandlerDrop();

	virtual bool initialize() override;
	virtual void terminate() override;
	virtual bool reset() override;
public:
	MP_DROP_INFO& getDropInfo(){ return m_mpDropSet; }
	const DropInfoItemData* findDropInfoById(NS::I64 nObjId);// { return m_mpDropSet.find(nObjId); }
	void clearAllDrop();
public:
	bool do_drop(int nId);
	bool discardWeaponReq(NS::int32 index);
	bool changeWeaponReq(int32 index1, NS::int32 index2);
	bool useWeaponReq(NS::int32 index);
	bool requestGetDropItem(NS::I64 nObjId, int index = -1);
	bool requsetGetPublicMapDropItem(NS::I64 nObjId,int index = -1);
	bool requestOpenStoneBox(NS::I64 nObjId);
	bool addDrop(int defId, int insId, const glm::vec2& pos);

	void onMsgPrivateDropListS2C(const MsgPB::MsgPrivateDropListS2C& msg);

	void playAnimation(DropInfoItemData &infoData);
	void playAnimationAtPublicMap(DropInfoItemData &infoData, bool isChestItem = false);

	void notifyDropDisappear(NS::I64 dropUuid);

	int getPosYByHigthAndDis(int nHigth, int nDistance);
	void onMsgPrivateDropDelS2C(const MsgPB::MsgPrivateDropDelS2C& msg);
	bool removeItem(NS::I64 delUuid);
	void onMsgPrivateDropSetS2C(const MsgPB::MsgPrivateDropSetS2C& msg);
	//void playSetDataAnimation();
	void onMsgPublicDropAppearanceS2C(const MsgPB::MsgPublicDropAppearanceS2C& msg);
	void onMsgPublicDropSetS2C(const MsgPB::MsgPublicDropSetS2C& msg);
	void onMsgPublicDropDelS2C(const MsgPB::MsgPublicDropDelS2C& msg);
	void onMsgPublicDropListS2C(const MsgPB::MsgPublicDropListS2C& msg);
	void onMsgPvPRandItemChestAppearance_MS2C(const MsgPB::MsgPvPRandItemChestAppearance_MS2C& msg);
	void onMsgPvPRandItemGrassAppearance_MS2C(const MsgPB::MsgPvPRandItemGrassAppearance_MS2C& msg);
	void onMsgPvPRandChestDispear_MS2C(const MsgPB::MsgPvPRandChestDispear_MS2C& msg);
	void updateAnimation(DropInfoItemData &infoData);
	void onMsgPVPDiscardWeaponS2C(const MsgPB::MsgPVPDiscardWeaponS2C& msg);
	void onMsgPVPChangeWeaponS2C(const MsgPB::MsgPVPChangeWeaponS2C& msg);
	void onMsgPVPChoiceWeaponS2C(const MsgPB::MsgPVPChoiceWeaponS2C& msg);
	void refreshPvpWeaponView();

	void onMsgDungeonBossDrop_MS2C(const MsgPB::MsgDungeonBossDrop_MS2C& msg);
	void onMsgDungeonBossChestDrop_MS2C(const MsgPB::MsgDungeonBossOpenChestRs_MS2C& msg);
	void handleDropInfo(const ::google::protobuf::RepeatedPtrField< ::MsgPB::PBDropInfo >& DropInfo, NS::I64 iggId, NS::int32 mapId);
private:
	void onMsgDropPickUpErrS2C(const MsgPB::MsgDropPickUpErrS2C& msg);

private:
	//void onMsgPrivateDropListS2C(const MsgPB::MsgPrivateDropListS2C& msg);

	MP_DROP_INFO m_mpDropSet;

	//MP_DROP_INFO m_mpCacheDropSet;

	//TV_DROP m_vDropData;
};

class HandlerDropEvent : public NS::Event
{
public:
	enum class EventType
	{
		Unknown = 0,
		Drop_Block,
		Drop_Get_Item,
		Drop_PvP_Item,
	};

public:
	HandlerDropEvent() {};
	virtual ~HandlerDropEvent(){};

public:
	EventType m_type{ EventType::Unknown };
	NS::I64 m_nUserUuid{ 0 };
	NS::int32 m_nDefId{0};
	int m_nCount{ 0 };
};
