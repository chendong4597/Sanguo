/*!
 * \file HandlerItem.cpp
 *
 * \author ChenBK
 * \date 12/08/2016
 *
 */
#include "stdafx.h"
#include "HandlerItem.h"
#include "data/ItemData.h"
#include "../common/MsgMgr.h"
#include "msg/MsgGM.pb.h"
#include "data/role/Player.h"
#include "config/PlayerAttrCfg.h"
#include "../localserver/LocalServer_Buff.h"
#include "HandlerLogin.h"

using namespace NS;
using namespace MsgPB;



static HandlerItem* g_HandlerItem = nullptr;
HandlerItem& HandlerItem::getInstance()
{
    assert(g_HandlerItem);
    return *g_HandlerItem;
}

HandlerItem::HandlerItem()
{
    assert(!g_HandlerItem);
    g_HandlerItem = this;
	reset();
}

HandlerItem::~HandlerItem()
{
    g_HandlerItem = nullptr;
}

bool HandlerItem::initialize()
{
	REGISTER_S2C(onMsgItemData);
	REGISTER_S2C(onMsgItemEquipResultC2GS);
	REGISTER_S2C(onMsgItemRemoveGS2C);
	REGISTER_S2C(onMsgSyncItemPacketPosGS2C);
	REGISTER_S2C(onMsgItemMaterialS2C);
	REGISTER_S2C(onMsgItemUseResultS2C);
	return true;
}

void HandlerItem::terminate()
{
}

bool HandlerItem::reset()
{

	return true;
}

void HandlerItem::updateItemData(const MsgPB::MsgItemDataGS2C_ItemData &msg, bool bAdd)
{
	auto&& pItem = ItemPacketMgr::getInstance().findItemDataByUuid(msg.uuid());
	if (pItem == nullptr)
	{
		pItem = std::make_shared<ItemData>();
		if (!pItem->initWithItemId(msg.dataid()))
		{
			// 创建 Item 失败。
			return;
		}
		pItem->setUuid(msg.uuid());
		ItemPacketMgr::getInstance().addItem(pItem);
		if (ItemPacketMgr::getInstance().AddNewItemTips(msg.uuid())) {
			HandlerItemGetEvent evt;
			evt.m_itemUuid = msg.uuid();
			evt.m_itemNum = msg.count();
			POST_DATA_EVENT(evt);

			std::string str = fmt::format("{0} X {1}", _LC(pItem->m_pItem->s_name.c_str()), (int)(msg.count()));
			GameAppExtern::ShowToastTips(str, static_cast<int>(HandlerTipsEvent::EventType::Evt_ItemTips));
		}
	} else {
		if (pItem->getCount() < msg.count() && pItem->m_pItem) {
			HandlerItemGetEvent evt;
			evt.m_itemUuid = msg.uuid();
			evt.m_itemNum = msg.count() - pItem->getCount();
			POST_DATA_EVENT(evt);

			std::string str = fmt::format("{0} X {1}", _LC(pItem->m_pItem->s_name.c_str()), (int)(msg.count() - pItem->getCount()));
			GameAppExtern::ShowToastTips(str, static_cast<int>(HandlerTipsEvent::EventType::Evt_ItemTips));
		}
	}
	pItem->setDurableMax(msg.durablemax());
	pItem->setPos(msg.pos());
	pItem->setCount(msg.count());
	pItem->setPacketType(msg.packettype());
	pItem->cleanAttrEffect(AttrType::ATTR_All);
	//装备附加属性
	for (int i = 0; i < msg.effects_size(); i++)
	{
		pItem->addAttrEffect(AttrType::ATTR_EFFECT, msg.effects(i));
	}

	for (int i = 0; i < msg.baseval_size(); i++)
	{
		pItem->addAttrEffect(AttrType::ATTR_BASE, msg.baseval(i));
	}

	for (int i = 0; i < msg.staticval_size(); i++)
	{
		pItem->addAttrEffect(AttrType::ATTR_STATICVAL, msg.staticval(i));
	}
}


int HandlerItem::findAmuletPos()
{
	int retNewPos{ 0 };

	// 排序 数据.
	struct sortItem
	{
		sortItem(int mK, int sK, int dV)
			: m_mK(mK), m_sK(sK), m_dV(dV) {}

		int m_mK{ 0 };
		int m_sK{ 0 };
		int m_dV{ 0 };

	};
	std::vector<sortItem> items;

	for (int pos = MsgPB::EQUIP_PACKET_POS::EQUIP_PACKET_AMULET1; pos <= MsgPB::EQUIP_PACKET_POS::EQUIP_PACKET_AMULET3; ++pos)
	{
		auto&& itemData = ItemPacketMgr::getInstance().findItemDataByPos(MsgPB::PACKET_TYPE::EQUIPMENT, pos);
		if (itemData)
		{
			items.push_back(sortItem(itemData->getLevel(), itemData->getQualityGrade(), pos));
		}
		else // 有空位时, 直接装备在空位上.
		{
			retNewPos = pos;
			break;
		}
	}

	if (items.size() == 3)	// 没有空位时, 选择 Level 低, QG 低 的替换.
	{
		std::sort(items.begin(), items.end(), [](sortItem& A, sortItem& B)
		{
			if (A.m_mK == B.m_mK) return A.m_sK < B.m_sK;
			return A.m_mK < B.m_mK;
		});

		retNewPos = items[0].m_dV;
	}

	return retNewPos;
}

bool HandlerItem::requestItemPM(const NS::tstring& strPM)
{
	//if (!HandlerLogin::getInstance().isConnect())
	//{
	//	UiMsgBoxToast::getInstance().showText(_LC("CLI_Common-network is diconnect."));
	//	return false;
	//}

	MsgGM  msg;
	msg.set_iggid(HandlerLogin::getInstance().GetHeroIggId());
	msg.set_content(strPM.c_str());
	return MsgMgr::getInstance().sendMessageDirect(msg);
}


//穿的时候发UUID，脱得时候为0.
bool HandlerItem::requestItemEquip(NS::I64 srcUuid, int equipPacketPos, int bodyPos, NS::I64 clientUseUuid, bool bAutoFindAmuletPos)
{
	// 穿装饰品装备时, bAutoFindAmuletPos 有空位优先装备到空位, 没有空位时,替换品级和等级最低的饰品, 如果全部品级和等级一样, 则替换第一个.
	//if (srcUuid != 0 && bAutoFindAmuletPos && equipPacketPos == MsgPB::EQUIP_PACKET_POS::EQUIP_PACKET_AMULET1)
	//{
	//	equipPacketPos = findAmuletPos();
	//}

	MsgPB::MsgItemEquipC2GS msg;
	msg.set_serialid(clientUseUuid);
	msg.set_uuid(srcUuid);
	msg.set_equippacketpos(equipPacketPos);
	msg.set_bodypacketpos(bodyPos);
	return MsgMgr::getInstance().sendMessageDirect(msg);
	
}

void HandlerItem::onMsgItemRemoveGS2C(const MsgPB::MsgItemRemoveGS2C& msg)
{
	bool bDel = false;
	auto&& pItemData = ItemPacketMgr::getInstance().findItemDataByUuid(msg.uuid());
	if (pItemData)
	{
		bDel = ItemPacketMgr::getInstance().removeItem(msg.uuid());
	}
	if (bDel) {
		HandlerItemEvent evt;
		evt.m_type = HandlerItemEvent::EventItem::EventItem_del;
		POST_DATA_EVENT(evt);
	}
}

//********************************************************************************************************************************************************************************
//
//********************************************************************************************************************************************************************************
void HandlerItem::onMsgItemData(const MsgPB::MsgItemDataGS2C& msg)
{
	bool bLocalServer = false;
	if (msg.has_islocalserver() && msg.islocalserver())
	{
		bLocalServer = true;
	}

	for (auto&& data : msg.items())
	{
		auto&& pItemData = ItemPacketMgr::getInstance().findItemDataByUuid(data.uuid());
		if (pItemData)
		{
			if (data.count() == 0 || data.dataid() == 0)
			{
				ItemPacketMgr::getInstance().removeItem(data.uuid());
			}
			else
			{
				updateItemData(data);
			}
		}
		else
		{
			updateItemData(data);
		}
	}
	
	Role::EquipMap equipMap;
	const auto self = PlayerMgr::getInstance().getSelf();
	if (self) self->calcAttr();
	const PlayerAttrCfg::AttachInfoMap& pList = PlayerAttrCfg::getInstance().getAttachInfoList();
	bool bShowCos = true;// Equip时也可显示时装,后续bShowCos可受复选框状态控制
	for (const auto& it : pList)
	{
		std::shared_ptr<ItemData>&& pItemData = ItemPacketMgr::getInstance().findItemDataByPos(MsgPB::PACKET_TYPE::EQUIPMENT, it.m_attachPos);
		if (!pItemData || !pItemData->getArmorCfg() || (it.m_attachPos > 8 && !bShowCos))
		{
			continue;
		}
		equipMap.insert(std::make_pair(it.m_attachPos, pItemData));
	}
	self->setEquipMap(equipMap);
	self->calcAttr();

	HandlerItemEvent evt;
	evt.m_type = HandlerItemEvent::EventItem::EventItem_update;
	POST_DATA_EVENT(evt);
}

void HandlerItem::onMsgItemEquipResultC2GS(const MsgPB::MsgItemEquipResultC2GS& msg)
{
	if (msg.result() == ITEM_EQUIP_SUCCESS) {
		HandlerItemEvent evt;
		evt.m_type = HandlerItemEvent::EventItem::EventItem_equip;
		POST_DATA_EVENT(evt);
		GameAppExtern::ShowToastTips(_LC("UI_NPC_QUIP_SUCCESS"));

		HandlerSoundEvent evt2;
		evt2.m_type = HandlerSoundEvent::EventType::Evt_Equip;
		POST_DATA_EVENT(evt2);
	}
}


bool HandlerItem::requestSyncItemPacketPos(MsgPB::PACKET_TYPE packet, NS::uint sortType)
{
	if (!HandlerLogin::getInstance().isConnect() || m_handlerSortBag)
	{
		return false;
	}


	m_handlerSortBag = true;
	// 清除上一次背包同步数据。
	m_itemPacketPosSyncDataList.clear();

	// 获取背包排序
	auto&& sortBag = ItemPacketMgr::getInstance().getSortBag(packet, static_cast<PackSortType>(sortType));
	for (int newPos = 0; newPos < static_cast<int>(sortBag.size()); ++newPos)
	{
		MsgPB::ItemPacketPosSyncData syncData;
		syncData.set_pos(newPos);
		syncData.set_item_uuid(sortBag[newPos]->getUuid());

		m_itemPacketPosSyncDataList.push_back(syncData);
	}

	// 发送请求排序。
	MsgSyncItemPacketPosC2GS msg;
	msg.set_packet(packet);
	for (auto&& syncData : m_itemPacketPosSyncDataList)
	{
		if (auto&& addData = msg.add_datas())
		{
			addData->CopyFrom(syncData);
		}
	}

	HandlerSoundEvent evt2;
	evt2.m_type = HandlerSoundEvent::EventType::Evt_ItemMan;
	POST_DATA_EVENT(evt2);

	return MsgMgr::getInstance().sendMessage(msg);
}


void HandlerItem::onMsgSyncItemPacketPosGS2C(const MsgPB::MsgSyncItemPacketPosGS2C& msg)
{
	tstring strMsg = EMPTY_STRING;
	switch (static_cast<MsgPB::E_SyncItemPacketPosResult>(msg.result()))
	{
	case MsgPB::E_SyncItemPacketPosResult_Success:	// 排序完成。
	{
		for (auto&& syncData : m_itemPacketPosSyncDataList)
		{
			auto&& findItemData = ItemPacketMgr::getInstance().findItemDataByUuid(syncData.item_uuid());
			if (findItemData)
			{
				//LOGD("Info: ========= DEBUG: SyncItemPacketPos: {0} -> {1}", syncData.pos(), syncData.item_uuid());
				findItemData->setPos(syncData.pos());
			}
		}
	}
	default: break;
	}
	// 恢复排序状态。
	m_handlerSortBag = false;
	m_itemPacketPosSyncDataList.clear();
	HandlerItemEvent evt;
	evt.m_type = HandlerItemEvent::EventItem::EventItem_update;
	POST_DATA_EVENT(evt);
}

void HandlerItem::onMsgItemMaterialS2C(const MsgPB::MsgItemMaterialS2C& msg)
{
	for (int i = 0; i < msg.data_size(); i++)
	{
		NS::I64 nPreNum = ItemPacketMgr::getInstance().GetMaterial(msg.data(i).itemid());
		ItemPacketMgr::getInstance().SetMaterial(msg.data(i).itemid(), msg.data(i).itemnum());

		if (nPreNum != 0 && msg.data(i).itemnum() > nPreNum) {
			auto&& pMaterial = ItemCfg::getInstance().getMaterial(msg.data(i).itemid());
			if (!pMaterial)
			{
				continue;
			}
			std::string str = fmt::format("{0} X {1}", _LC(pMaterial->s_name.c_str()), (int)(msg.data(i).itemnum() - nPreNum));
			GameAppExtern::ShowToastTips(str, static_cast<int>(HandlerTipsEvent::EventType::Evt_Material));
		}
	}
	POST_DATA_EVENT(msg);
}

void HandlerItem::onMsgItemUseResultS2C(const MsgPB::MsgItemUseResultS2C& msg)
{
	if (msg.result() == ITEM_USE_SUCCESS) {
		ItemPacketMgr::getInstance().ResetCooldown(msg.itemid());
		GameAppExtern::ShowToastTips(_LC("UI_USE_ITEM_SUCCESS"));
		if (msg.add_buff_id() > 0) {
			const auto self = PlayerMgr::getInstance().getSelf();
			if (self && !self->findBuffById(msg.add_buff_id())) {
				LocalServer_Buff::getInstance().addBuff(msg.add_buff_id(), self->getId(), self->getId());
			}
		}
	}
	POST_DATA_EVENT(msg);
}

