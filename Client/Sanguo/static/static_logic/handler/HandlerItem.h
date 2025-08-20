/*!
* \file HandlerItem.h
*
* \author ChenBK
* \date 12/08/2016
*
*/
#pragma once

#include "BaseTypeDefine.h"
#include <MsgHandler.h>
#include <msg/MsgType.pb.h>
#include <msg/MsgItem.pb.h>
#include "data/ItemData.h"
#include "common/Timer.h"


namespace MsgPB
{
	class MsgItemUseResultS2C;
	class ItemPacketPosSyncData;
	class MsgFoodUseCountS2C;
}

class ItemData;

class HandlerItem : public bzbee::MsgHandler
{
	DECLARE_EVENT_HOST();
	DECLARE_TIMER_HOST();
public:
	RTTI_DEF(HandlerItem);

public:
	static HandlerItem& getInstance();

	HandlerItem();
	virtual ~HandlerItem();

	bool initialize() override;
	void terminate() override;
	bool reset() override;

	int findAmuletPos();
	void updateItemData(const MsgPB::MsgItemDataGS2C_ItemData &msg, bool bAdd = true);
public:
	bool requestItemPM(const NS::tstring& strPM);
	bool requestItemEquip(NS::I64 srcUuid, int equipPacketPos = 0, int bodyPos = 0, NS::I64 clientUseUuid = 0, bool bAutoFindAmuletPos = true);
	bool requestSyncItemPacketPos(MsgPB::PACKET_TYPE packet = MsgPB::PACKET_TYPE::BODY, NS::uint sortType = PackSortType::PackSortBag);

protected:
	void onMsgItemRemoveGS2C(const MsgPB::MsgItemRemoveGS2C& msg);
	void onMsgItemData(const MsgPB::MsgItemDataGS2C& msg);
	void onMsgItemEquipResultC2GS(const MsgPB::MsgItemEquipResultC2GS& msg);
	void onMsgSyncItemPacketPosGS2C(const MsgPB::MsgSyncItemPacketPosGS2C& msg);
	void onMsgItemMaterialS2C(const MsgPB::MsgItemMaterialS2C& msg);
	void onMsgItemUseResultS2C(const MsgPB::MsgItemUseResultS2C& msg);

private:

	bool m_handlerSortBag{ false };
	std::vector<MsgPB::ItemPacketPosSyncData> m_itemPacketPosSyncDataList{};
};

