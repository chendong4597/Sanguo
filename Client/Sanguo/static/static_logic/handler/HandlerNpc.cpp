/*!
 * \file HandlerNpc.cpp
 * \date 02-23-2017 16:58:48
 *
 * \author yuechuanxu
 */
#include "stdafx.h"
#include "HandlerNpc.h"
#include "HandlerMonster.h"
#include "HandlerRole.h"
#include "../common/MsgMgr.h"
#include <msg/MsgMove.pb.h>
#include <msg/MsgNpc.pb.h>
#include <msg/MsgMob.pb.h>
#include <msg/MsgProtector.pb.h>
#include <data/MapData.h>
#include <data/role/Npc.h>
#include "data/role/Player.h"
#include "data/NotesData.h"
#include <data/NpcProtectorData.h>
#include <config/NpcActiveCfg.h>
#include <config/BuffCfg.h>
#include "config/NpcTypeCfg.h"
#include "HandlerItem.h"
#include "config/BuildRoomCfg.h"
#include "HandlerLogin.h"
#include "Rtti.h"
#include <common/PlayerPrefs.h>
#include "HandlerBuilding.h"
#include "Cache.h"
#include "TimeUtil.h"

using namespace NS;
using namespace MsgPB;

static HandlerNpc* g_HandlerNpc = nullptr;
HandlerNpc& HandlerNpc::getInstance()
{
	assert(g_HandlerNpc);
	return *g_HandlerNpc;
}

bool HandlerNpc::isInitialized()
{
    return (g_HandlerNpc != nullptr);
}

HandlerNpc::HandlerNpc()
{
	assert(!g_HandlerNpc);
	g_HandlerNpc = this;
	reset();
}

HandlerNpc::~HandlerNpc()
{
	g_HandlerNpc = nullptr;
}

bool HandlerNpc::initialize()
{
	//S2C
	REGISTER_S2C(onMsgNpcDestroy);
	REGISTER_S2C(onMsgNpcAppearance);
	REGISTER_S2C(onMsgNpcProtectorClaimRentAndGiftRes);
	REGISTER_S2C(onMsgSyncNpcProtectorData);
	REGISTER_S2C(onMsgUpdateNpcProtectorData);
	REGISTER_S2C(onMsgRemoveNpcProtectorData);
	REGISTER_S2C(onMsgGetNpcProtectorStatusResult);
	REGISTER_S2C(onMsgProtectorPatrolResult);
	REGISTER_S2C(onMsgMobInteractiveS2C);
	REGISTER_S2C(onMsgProtectorReviveResultS2C);
	REGISTER_S2C(onMsgBuildingRoomConditionS2C);
	REGISTER_S2C(onMsgProtectorEquipResS2C);
	REGISTER_S2C(onMsgNpcRentsAndGiftsRes);
	REGISTER_S2C(onMsgProtectorUseEssenceResS2C);
	REGISTER_EVENT(&HandlerBuilding::getInstance(), HandlerBuildingEvent, &HandlerNpc::onBuildingChangeEvt);
	REGISTER_S2C(onMsgProtectorRebornAtOnceS2C);

	return true;
}

void HandlerNpc::terminate()
{
	UNREG_S2C(MsgNpcDestroyG2C);
	UNREG_S2C(MsgNpcAppearance);
	UNREG_S2C(MsgProtectorRentResultS2C);
	UNREG_S2C(MsgProtectorSyncS2C);
	UNREG_S2C(MsgProtectorUpdateS2C);
	UNREG_S2C(MsgProtectorRemoveS2C);
	UNREG_S2C(MsgProtectorGetStatusResultS2C);
	UNREG_S2C(MsgProtectorPatrolResultS2C);
	UNREG_S2C(MsgMobInteractiveS2C);
	UNREG_S2C(MsgProtectorReviveResultS2C);
	UNREG_S2C(MsgProtectorUseEssenceResS2C);
	UNREG_S2C(MsgProtectorRebornAtOnceS2C);

	reset();
}

//复活npc请求
bool HandlerNpc::requestNpcRevive(NS::I64 nUuid)
{
	//LOGDE("-------HandlerNpc::requestNpcRevive()-------");
	MsgProtectorReviveC2S msg;
	msg.set_uuid(nUuid);
	return MsgMgr::getInstance().sendMessage(msg);
}

//复活npc结果;
void HandlerNpc::onMsgProtectorReviveResultS2C(const MsgPB::MsgProtectorReviveResultS2C& msg)
{
	//LOGDE("-------HandlerNpc::onMsgProtectorReviveResultS2C()-------");
	auto result = msg.result();
	tstring message = "";
	switch (result)
	{
	case MsgPB::MsgProtectorReviveResultS2C_RESULT_FAILURE:
		message = _LC("CLI_NpcBadgeSys-NPC_REVIVE_FAILURE");
		break;
	case MsgPB::MsgProtectorReviveResultS2C_RESULT_SUCCESS:
		message = _LC("CLI_NpcBadgeSys-NPC_REVIVE_SUCCESS");
		break;
	case MsgPB::MsgProtectorReviveResultS2C_RESULT_ERROR_COIN:
		message = _LC("CLI_NpcBadgeSys-NPC_REVIVE_FAILURE_BECAUSE_OF_NO_ENOUGH_COIN");
		break;
	default:
		break;
	}
	//UiMsgBoxToast::getInstance().showText(message, "");
}

//复活npc
void HandlerNpc::reviveNpc(int npcDefId)
{
	//LOGDE("-------HandlerNpc::reviveNpc()-------");
	auto data = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByDefId(npcDefId);
	if (!data || data->status != BADGE_STATUS_DEATH)
		return;
	data->setState(BADGE_STATUS_ENABLE_FUNCTION);
	//LocalServer_Npc::getInstance().reveieNpc(npcDefId);
}

//复活NPC，晶钻;
bool HandlerNpc::reviveNpcReq(NS::I64 badgeUuid)
{
	MsgProtectorRebornAtOnceC2S msg;
	msg.set_uuid(badgeUuid);
	return MsgMgr::getInstance().sendMessage(_MSG_PROTECTOR_REBORN_AT_ONCE_C2S, &msg);
}

//复活NPC结果;
void HandlerNpc::onMsgProtectorRebornAtOnceS2C(const MsgPB::MsgProtectorRebornAtOnceS2C& msg)
{
	tstring message = "";
	switch (msg.result())
	{
	case EC_Success:
		message = _LC("CLI_NpcBadgeSys-NPC_REVIVE_SUCCESS");
		break;
	case EC_ProtectorResult_NpcNotExist:
		message = _LC("SRV_MsgProtectorRebornAtOnceS2C-EC_ProtectorResult_NpcNotExist");
		break;
	case EC_ProtectorResult_RebornLackCost:
		message = _LC("SRV_MsgProtectorRebornAtOnceS2C-EC_ProtectorResult_RebornLackCost");
		break;
	default:
		message = _LC("CLI_NpcBadgeSys-NPC_REVIVE_FAILURE");
		break;
	}
	//UiMsgBoxToast::getInstance().showText(message, "");
}

// Npc徽章接收血量、状态改变事件
//void HandlerNpc::onNpcBadgeReciveEvent(const NpcBadgeReciveEvent& evt)
//{
//	//LOGDE("-------HandlerNpc::onNpcBadgeReciveEvent()-------");
//	//todo 同步数据给服务器;
//	//MsgPB::MsgProtectorData msg2;
//	//auto data = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByUuid(evt.uuid);
//	//if (data)
//	//{
//	//	LocalServer_Npc::getInstance().SetMsgProtectorDataToServer(*data, &msg2);
//	//	SyncCache::getInstance().setProtectorData(msg2);
//	//	MsgProtectorClientSyncRectTimeC2S msg;
//	//	msg.set_uuid(evt.uuid);
//	//	NS::MsgMgr::getInstance().sendMessage(_MSG_PROTECTOR_CLIENT_SYNC_RECT_TIME_C2S, &msg);
//	//}
//}

// 查找装备饰品位
int HandlerNpc::findAmuletPos()
{
	//LOGDE("-------HandlerNpc::findAmuletPos()-------");
	int retNewPos{ 0 };

	// 排序 数据;
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
		auto&& itemData = ItemPacketMgr::getInstance().findItemDataByPos(MsgPB::PACKET_TYPE::PROTECTOR_EQUIP, pos);
		if (itemData)
		{
			items.push_back(sortItem(itemData->getLevel(), itemData->getQualityGrade(), pos));
		}
		else // 有空位时, 直接装备在空位上;
		{
			retNewPos = pos;
			break;
		}
	}

	if (items.size() == 3)	// 没有空位时, 选择 Level 低, QG 低 的替换;
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

// 装备请求
bool HandlerNpc::npcEquipReq(NS::I64 badgeUuuid, NS::I64 itemUuuid, int equipPos)
{
	m_badgeUuid = static_cast<int>(badgeUuuid);
	MsgProtectorEquipReqC2S msg;
	msg.set_itemuuid(itemUuuid);
	msg.set_uuid(badgeUuuid);
	// 穿装饰品装备时, 有空位优先装备到空位, 没有空位时,替换品级和等级最低的饰品, 如果全部品级和等级一样, 则替换第一个;
	/*if (equipPos == MsgPB::EQUIP_PACKET_POS::EQUIP_PACKET_AMULET1 && itemUuuid != 0)
	{
		equipPos = findAmuletPos();
	}*/
	msg.set_pos((MsgPB::NPC_EQUIP_POS)equipPos);
	return MsgMgr::getInstance().sendMessage(_MSG_PROTECTOR_EQUIP_C2S, &msg);
}

// NPC批量装备
bool HandlerNpc::npcBathEquipRequest(NS::I64 badgeUuid, std::vector<std::tuple<NS::I64, NS::int32>> equipments)
{
	if (equipments.empty())
		return false;

	m_badgeUuid = static_cast<NS::int32>(badgeUuid);

	for (auto&& equipmentInfo : equipments)
	{
		NS::I64 nItemUuid = std::get<0>(equipmentInfo);
		NS::int32 nPosIndex = std::get<1>(equipmentInfo);

		MsgProtectorEquipReqC2S msg;
		msg.set_itemuuid(nItemUuid);
		msg.set_uuid(badgeUuid);
		msg.set_pos((MsgPB::NPC_EQUIP_POS)nPosIndex);

		MsgMgr::getInstance().sendMessage(_MSG_PROTECTOR_EQUIP_C2S, &msg);
	}

	return true;
}

// 装备结果
void HandlerNpc::onMsgProtectorEquipResS2C(const MsgPB::MsgProtectorEquipResS2C& msg)
{
	//LOGDE("-------HandlerNpc::onMsgProtectorEquipResS2C()-------");
	switch (msg.result())
	{
	case MsgProtectorEquipResS2C_RESULT_SUCCESS:
	{
		HandlerNpcEvent evt;
		evt.m_type = HandlerNpcEvent::EventNpc::EventNpc_Equip;
		evt.uuid = msg.uuid();
		POST_DATA_EVENT(evt);
		HandlerItemEvent evt1;
		evt1.m_type = HandlerItemEvent::EventItem::EventItem_update;
		POST_DATA_EVENT(evt1);
		GameAppExtern::ShowToastTips(_LC("UI_NPC_QUIP_SUCCESS"));
		HandlerSoundEvent evt2;
		evt2.m_type = HandlerSoundEvent::EventType::Evt_Equip;
		POST_DATA_EVENT(evt2);
		break;
	}
	case MsgProtectorEquipResS2C_RESULT_ERR:
		//UiMsgBoxToast::getInstance().showText(_LC("SRV_MsgProtectorEquipResS2C-MsgProtectorEquipResS2C_RESULT_ERR"));
		break;
	case MsgProtectorEquipResS2C_RESULT_FAILURE_ITEM_NOT_EXIST:
		//UiMsgBoxToast::getInstance().showText(_LC("SRV_MsgProtectorEquipResS2C-MsgProtectorEquipResS2C_RESULT_FAILURE_ITEM_NOT_EXIST"));
		break;
	case MsgProtectorEquipResS2C_RESULT_FAILURE_POS_ERROR:
		//UiMsgBoxToast::getInstance().showText(_LC("SRV_MsgProtectorEquipResS2C-MsgProtectorEquipResS2C_RESULT_FAILURE_POS_ERROR"));
		break;
	case MsgProtectorEquipResS2C_RESULT_FAILURE_PACKET_FULL:
		//UiMsgBoxToast::getInstance().showText(_LC("SRV_MsgProtectorEquipResS2C-MsgProtectorEquipResS2C_RESULT_FAILURE_PACKET_FULL"));
		break;
	default:
		break;
	}
}

// Npc使用属性精华请求
bool HandlerNpc::npcUseAttrReq(NS::I64 badgeUuuid, NS::I64 itemUuuid, int count, int curNum, int maxNum)
{
	//LOGDE("-------HandlerNpc::npcUseAttrReq()-------");
	m_npcId = 0;
	m_itemId = 0;
	auto pBadgeData = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByUuid(badgeUuuid);
	if (pBadgeData)
	{
		m_npcId = pBadgeData->templateid;
	}
	auto pItemData = ItemPacketMgr::getInstance().findItemDataByUuid(itemUuuid);
	if (pItemData)
	{
		m_itemId = pItemData->getItemId();
	}

	m_curUseNum = curNum;
	m_maxUseNum = maxNum;
	MsgProtectorUseEssenceReqC2S msg;
	msg.set_itemuuid(itemUuuid);
	msg.set_uuid(badgeUuuid);
	msg.set_use_count(count);
	return MsgMgr::getInstance().sendMessage(_MSG_PROTECTOR_USE_ESSENCE_C2S, &msg);
}

// Npc使用属性精华结果
void HandlerNpc::onMsgProtectorUseEssenceResS2C(const MsgPB::MsgProtectorUseEssenceResS2C& msg)
{
	//LOGDE("-------HandlerNpc::onMsgProtectorUseEssenceResS2C()-------");
	switch (msg.result())
	{
	case MsgProtectorUseEssenceResS2C_RESULT_SUCCESS:
	{
		HandlerNpcAtterEvent evt;
		postEvent(evt);
		tstring str = fmt::format(_LC("SRV_MsgProtectorEquipResS2C-MsgProtectorUseEssenceResS2C_RESULT_SUCCESS , then use les = {0}/{1}"), m_curUseNum + 1, m_maxUseNum);
		/*if (!m_bPressFeed)
		{
			str += _LC("CLI-press can continue");
		}*/
		//UiMsgBoxToast::getInstance().showText(str);

		// 冒险日志:NPC使用属性精华
		if (m_npcId > 0 && m_itemId > 0)
		{
			//CommonTaskEvent ctevt(MsgPB::EN_PLAYER_EVENT_TYPE_npc_use_essence, 1);
			//ctevt.m_updateMode = UpdateMode::Add;
			//ctevt.m_taskEventParams.n_task_event_param_1 = m_npcId;
			//ctevt.m_taskEventParams.n_task_event_param_2 = m_itemId;
			//TaskEventComponent::getInstance().postEvent(ctevt);
			break;
		}
	}
	case MsgProtectorUseEssenceResS2C_RESULT_ERR:
		//UiMsgBoxToast::getInstance().showText(_LC("SRV_MsgProtectorEquipResS2C-MsgProtectorUseEssenceResS2C_RESULT_ERR"));
		break;
	case MsgProtectorUseEssenceResS2C_RESULT_FAILURE_ITEM_NOT_EXIST:
		//UiMsgBoxToast::getInstance().showText(_LC("SRV_MsgProtectorEquipResS2C-MsgProtectorUseEssenceResS2C_RESULT_FAILURE_ITEM_NOT_EXIST"));
		break;
	case MsgProtectorUseEssenceResS2C_RESULT_FAILURE_OVER_LIMIT:
		//UiMsgBoxToast::getInstance().showText(_LC("SRV_MsgProtectorEquipResS2C-MsgProtectorUseEssenceResS2C_RESULT_FAILURE_OVER_LIMIT"));
		break;
	default:
		break;
	}
}

bool HandlerNpc::reset()
{
	//LOGDE("-------HandlerNpc::reset()-------");
	return true;
}

// 徽章数据同步事件
void HandlerNpc::onNpcBadgeDataSyncS2cEvent()
{
	//LOGDE("-------HandlerNpc::onNpcBadgeDataSyncS2cEvent()-------");

	//REG_EVENT(&HandlerLogin::getInstance())[this](const Event_MapLoaded& evt)
	//{
	//	int nNumCheckIn = 0;
	//	int nNumRentAvailible = 0;
	//	int nNumNpcWaitRevive = 0;
	//	int nNumNpcLeave = 0;
	//	int nNumNpcGift = 0;

	//	NpcProtectorDataMgr::getInstance()->getNpcBadgeStatusStatistics(nNumCheckIn, nNumRentAvailible, nNumNpcWaitRevive, nNumNpcLeave, nNumNpcGift);

	//	if (!nNumCheckIn && !nNumRentAvailible && !nNumNpcWaitRevive && !nNumNpcLeave && !nNumNpcGift) return;

	//	if (evt.mapType != MapDataMgr::MapType::Private) return;
	//	//UiMsgBoxNpcAbout* pDialog = UiMgr::getInstance().findDialog<UiMsgBoxNpcAbout>();
	//	//if (!pDialog)
	//	//{
	//	//	pDialog = UiMsgBoxNpcAbout::create();
	//	//}

	//	if (nNumCheckIn > 0) 
	//		//pDialog->pushBackNpcAboutCellInfo(UiMsgBoxNpcAbout::NpcAboutCellInfo{ UiMsgBoxNpcAbout::NpcCheckINCount, nNumCheckIn });
	//	if (nNumRentAvailible > 0)
	//		//pDialog->pushBackNpcAboutCellInfo(UiMsgBoxNpcAbout::NpcAboutCellInfo{ UiMsgBoxNpcAbout::NpcRentAvailibleCount, nNumRentAvailible });
	//	if (nNumNpcGift > 0)
	//		//pDialog->pushBackNpcAboutCellInfo(UiMsgBoxNpcAbout::NpcAboutCellInfo{ UiMsgBoxNpcAbout::NpcGiftCount, nNumNpcGift });
	//	if (nNumNpcWaitRevive > 0) 
	//		//pDialog->pushBackNpcAboutCellInfo(UiMsgBoxNpcAbout::NpcAboutCellInfo{ UiMsgBoxNpcAbout::NpcWaitingForReviveCount, nNumNpcWaitRevive });
	//	if (nNumNpcLeave > 0)
	//		//pDialog->pushBackNpcAboutCellInfo(UiMsgBoxNpcAbout::NpcAboutCellInfo{ UiMsgBoxNpcAbout::NpcHasLeftCount, nNumNpcLeave });

	//	//pDialog->refreshView(UiMsgBoxNpcAbout::FunctionDisplayAllNpcStateWhenEnterMap);

	//	//UiMgr::getInstance().showModalDialog(pDialog);
	//	//UNREG_EVENT(&HandlerLogin::getInstance(), Event_MapLoaded);
	//};
}

//徽章状态改变
//void HandlerNpc::onNpcBadgeStateChagedEvent(const NpcBadgeStateChangedS2CEvent& evt)
//{
//	//LOGDE("-------HandlerNpc::onNpcBadgeStateChagedEvent()-------");
//	auto pUpdateData = evt.m_updatedData.lock();
//	if (pUpdateData && pUpdateData->status == MsgPB::BADGE_STATUS_ENABLE_FUNCTION && pUpdateData->last_status == MsgPB::BADGE_STATUS_WAIT_JOIN)
//	{
//		UiMsgBoxNpcAbout* pDialog = UiMgr::getInstance().findDialog<UiMsgBoxNpcAbout>();
//		if (!pDialog)
//		{
//			pDialog = UiMsgBoxNpcAbout::create();
//		}
//		pDialog->pushBackNpcAboutCellInfo(UiMsgBoxNpcAbout::NpcAboutCellInfo{ UiMsgBoxNpcAbout::NpcCheckInNotification, pUpdateData->templateid });
//		pDialog->refreshView(UiMsgBoxNpcAbout::FunctionNotificationCheckIn);
//		UiMgr::getInstance().showModalDialog(pDialog);
//	}
//
//	auto data = evt.m_updatedData.lock();
//	if (!data)
//		return;
//
//	UiMsgBoxNpcAbout* pDialog = UiMgr::getInstance().findDialog<UiMsgBoxNpcAbout>();
//	if (pDialog && MapDataMgr::getInstance().getCurMapType() == MapDataMgr::MapType::Private)
//	{
//		pDialog->updateView();
//	}
//
//	if (data->last_status == MsgPB::BADGE_STATUS_LOCK && data->status != MsgPB::BADGE_STATUS_LOCK)
//	{
//		auto badgeIdstr = fmt::format("get{0}", data->uuid);
//		GameLog::getInstance().onSendGetBadgetEvent(badgeIdstr);
//		NpcProtectorDataMgr::getInstance()->addDataToBadgeBag(data->uuid);
//		LocalServer_Npc::getInstance().battleNpcUnLockEvent(data->uuid, true, true);
//		// 强制引导期间 徽章不添加到快捷栏.;
//		if (HandlerLogin::getInstance().getNewbieFlags(HandlerLogin::NewbieFlag::ForcedGuidEnd)
//			&& !evt.m_bClear && data->status == BADGE_STATUS_NONE)
//		{
//			HandlerItem::getInstance().requestItemShortcutSet(pUpdateData->uuid, ItemPacketMgr::getInstance().getItemShortcutFreePos(pUpdateData->uuid));
//		}
//		//龙斗士特殊处理,辉章解锁时，直接创建NPC;
//		if (NpcProtectorDataMgr::getInstance()->isDragonBallNpc(data->templateid))
//		{
//			auto isHaveCreate = HandlerLogin::getInstance().getNewbieFlags(HandlerLogin::DragonNpcFirstApper);
//			if (data->status == BADGE_STATUS_NONE && data->pos_x == 0 && data->pos_y == 0 && !isHaveCreate)
//			{
//				NpcProtectorDataMgr::getInstance()->createSpNpcData(data->templateid, 0, 0);
//				/*auto pSelfPlayer = PlayerMgr::getInstance().getSelf();
//				if (pSelfPlayer && MapDataMgr::getInstance().getCurrMapData())
//				{
//					glm::vec3 worldPos{};
//					MapDataMgr::getInstance().getCurrMapData()->coordinateToWorldPos(worldPos, pSelfPlayer->getCoord());
//					std::shared_ptr<LocalServer_BirthMob::StBirthParm> stParm = std::make_shared<LocalServer_BirthMob::StBirthParm>();
//					stParm->nMobDefID = data->templateid;
//					stParm->ownerMapObjId = pSelfPlayer->getId();
//					stParm->birthWorldPos = worldPos;
//					stParm->bSave = false;
//					stParm->bCollectEnable = false;
//					LocalServer_BirthMob::getInstance().birthMob(stParm);
//				}
//				else
//				{
//					assert(0);
//				}*/
//				HandlerLogin::getInstance().setNewbieFlag(HandlerLogin::DragonNpcFirstApper, true);
//			}
//		}
//
//		// 任意 “徽章”道具首次进入背包时 弹出难点引导 Tip;
//		if (HandlerLogin::getInstance().isInitNewbieFlags()
//			&& !HandlerLogin::getInstance().getNewbieFlags(HandlerLogin::NewbieFlag::NodusContent_BagGetFirstBadge))
//		{
//			auto&& pLobby = dynamic_cast<UiLobby*>(UiMgr::getInstance().findScreen(ScreenIds::UiLobby));
//			if (pLobby)
//			{
//				if (auto && pUi = pLobby->getUiGuideWatchAnimation())
//				{
//					pUi->pushGuideNodus(HandlerLogin::NewbieFlag::NodusContent_BagGetFirstBadge);
//				}
//			}
//			HandlerLogin::getInstance().setNewbieFlag(HandlerLogin::NewbieFlag::NodusContent_BagGetFirstBadge, true);
//		}
//
//		// 雅各布森徽章获取后，隐藏ui图标
//		if (unLockFunEvent::getLeaderBadgeItemId() == data->itemid)
//		{
//			auto&& pLobby = UiMgr::getInstance().findDialog<UiLobby>();
//			if (pLobby)
//			{
//				pLobby->showGetLeaderBadge(false);
//				unLockFunEvent evt;
//				evt.m_type = unLockFunEvent::EventType::getLeaderBadge;
//				HandlerRole::getInstance().postEvent(evt);
//			}
//		}
//		if (data->status == BADGE_STATUS_NONE)
//		{
//			auto cfg = ItemCfg::getInstance().getItemById((int)data->uuid);
//			if (!cfg)
//			{
//				assert(0);
//				return;
//			}
//			CommonTaskEvent evt2(MsgPB::EN_PLAYER_EVENT_TYPE_get_protector);
//			evt2.m_taskEventParams.n_task_event_param_1 = data->level;
//			evt2.m_taskEventParams.n_task_event_param_2 = cfg->m_qualityGrade;
//			TaskEventComponent::getInstance().postEvent(evt2);
//		}
//	}
//
//	if (data->status >= MsgPB::BADGE_STATUS_NONE ||
//		data->status <= MsgPB::BADGE_STATUS_WAIT_JOIN)
//	{
//		// 徽章放置成功后, 角色状态切换为战斗.;
//		auto&& self = PlayerComponentMgr::getInstance().getSelf();
//		if (self)
//		{
//			auto&& pPlayerCom = self->findComponent<PlayerComponent>();
//			if (pPlayerCom)
//			{
//				auto handItemdata = pPlayerCom->getHandItemData();
//				if (handItemdata && handItemdata->getUuid() == evt.m_updatedData.lock()->uuid)
//					pPlayerCom->changeState(PlayerComponent::State::Fight);
//			}
//		}
//		/*auto&& pUiLobby = UiMgr::getInstance().findDialog<UiLobby>();
//		if (pUiLobby)
//		{
//			auto pUiBag = pUiLobby->getUiBag();
//			if (pUiBag)
//				pUiBag->refreshBag(true);
//		}*/
//	}
//}

//七龙珠许愿请求
bool HandlerNpc::dragonBallListReq(int interactedMapObjId)
{
	//LOGDE("-------HandlerNpc::dragonBallListReq()-------");
	//n_ndragonBallNpcMapObjId = interactedMapObjId;
	//MsgDragonBallListReqC2S msg;
	//return MsgMgr::getInstance().sendMessage(_MSG_DRAGON_BALL_LIST_REQ, &msg);
	return true;
}

//七龙珠许愿结果
//void HandlerNpc::onMsgDragonBallLisResS2C(const MsgPB::MsgDragonBallLisResS2C& msg)
//{
//	//LOGDE("-------HandlerNpc::onMsgDragonBallLisResS2C()-------");
//	switch (msg.result())
//	{
//	case MsgPB::MsgDragonBallLisResS2C::FAILURE_COST_NOT_ENOUGH:
//		UiMsgBoxToast::getInstance().showText(_LC("SRV_onMsgDragonBallLisResS2C-FAILURE_COST_NOT_ENOUGH"), "");
//		break;
//	case MsgPB::MsgDragonBallLisResS2C::ERR:
//		UiMsgBoxToast::getInstance().showText(_LC("SRV_onMsgDragonBallLisResS2C-ERROR"), "");
//		break;
//	case MsgPB::MsgDragonBallLisResS2C::SUCCESS:
//	{
//		auto ui = UiMgr::getInstance().showDialog<UiNpc_wish>();
//		ui->setNpcObjId(n_ndragonBallNpcMapObjId);
//		if (ui)
//		{
//			std::vector<int> vecId{};
//			for (int i = 0; i < msg.id_size(); ++i)
//			{
//				const int id = msg.id(i);
//				vecId.push_back(id);
//			}
//			ui->refreshView(vecId);
//		}
//		break;
//	}
//	default:
//		break;
//	}
//}

//触发一个龙珠愿望请求
bool HandlerNpc::dragonBallTrigReq(NS::int32 id)
{
	//LOGDE("-------HandlerNpc::dragonBallTrigReq()-------");
	//MsgDragonBallTrigReqC2S msg;
	//msg.set_id(id);
	//return MsgMgr::getInstance().sendMessage(_MSG_DRAGON_BALL_TRIG_REQ, &msg);
	return true;
}

////触发一个龙珠愿望结果
//void HandlerNpc::onMsgDragonBallTrigResS2C(const MsgPB::MsgDragonBallTrigResS2C& msg)
//{
//	//LOGDE("-------HandlerNpc::onMsgDragonBallTrigResS2C()-------");
//	switch (msg.result())
//	{
//	case MsgPB::MsgDragonBallTrigResS2C::ERR:
//		UiMsgBoxToast::getInstance().showText(_LC("SRV_onMsgDragonBallTrigResS2C-ERROR"), "");
//		break;
//	case MsgPB::MsgDragonBallTrigResS2C::SUCCESS:
//		break;
//	default:
//		break;
//	}
//	auto ui = UiMgr::getInstance().findDialog<UiNpc_wish>();
//	if (ui)
//	{
//		ui->onTriRes(msg.result(), msg.interactive());
//	}
//}
//
////刷新龙珠许愿UI
//void HandlerNpc::onMsgDragonBallTrigUIResS2C(const MsgPB::MsgDragonBallTrigUIResS2C& msg)
//{
//	//LOGDE("-------HandlerNpc::onMsgDragonBallTrigUIResS2C()-------");
//	switch ((InteractiveEffectCfg::EffectType)msg.interactive_type())
//	{
//	case InteractiveEffectCfg::EffectType::GET_ITEM:
//	case InteractiveEffectCfg::EffectType::EXCHANGE_PACKET:
//	{
//		auto pDialog = UiMgr::getInstance().showDialog<UiNpc_wish_reward>();
//		if (pDialog)
//		{
//			pDialog->refreshView(msg);
//		}
//	}
//	break;
//	default:
//	{
//	}
//	}
//}

// 添加一个npc
bool HandlerNpc::addNpc(int mapObjId, NS::I64 insId, int defId, const glm::vec3& pos, bool createWithLocal, bool isBadgeNpc)
{
	//LOGDE("-------HandlerNpc::addNpc()-------");
	std::shared_ptr<Npc> npc = std::make_shared<Npc>(mapObjId, 0, defId);
	npc->setPos(pos);
	npc->setIsBadgeNpc(isBadgeNpc);
	npc->SetProtectorUuid(insId);
	auto pNpcData = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByUuid(insId);
	if (pNpcData)
	{
		npc->setLevel(pNpcData->level);
	}
	npc->calcAttr();
	return NpcMgr::getInstance().addNpc(npc);
}

//npc创建事件
void HandlerNpc::onMsgNpcAppearance(const MsgNpcAppearance& msg)
{
	int mapObjId{ msg.mapobjectid() }, defId{ msg.npcdefid() };
	NS::I64 insId{ msg.npcinsid() };
	//LOGE("add monster, mapObjId: {0}, insId: {1}, defId: {2}.", mapObjId, insId, defId);
	auto npc = addNpc(mapObjId, insId, defId, glm::vec3(msg.movepath().pos().x(), msg.movepath().pos().y(), 0), true, true);
	if (!npc)
	{
		//LOGE("add monster failed, mapObjId: {0}, insId: {1}, defId: {2}.", mapObjId, insId, defId);
		return;
	}

	HandlerNpcEvent evt;
	evt.m_type = HandlerNpcEvent::EventNpc::EventNpc_Appearance;
	evt.mapObjId = mapObjId;
	evt.insId = insId;
	evt.defId = defId;
	POST_DATA_EVENT(evt);
}

//npc销毁
void HandlerNpc::onMsgNpcDestroy(const MsgNpcDestroyG2C& msg)
{
	HandlerNpcEvent evt;
	evt.m_type = HandlerNpcEvent::EventNpc::EventNpc_DisAppearance;
	evt.mapObjId = msg.mapobjectid();
	evt.insId = msg.npcinsid();
	POST_DATA_EVENT(evt);
}




bool HandlerNpc::requestNpcFeatures(int npcId, NS::I64 roomPos)
{
	//LOGDE("-------HandlerNpc::requestNpcFeatures()-------");
	//MsgNpcFeaturesC2G msg;
	//msg.set_npcid(npcId);
	//msg.set_roompos(roomPos);
	//return MsgMgr::getInstance().sendMessage(_MSG_NPC_FEATURES_C2G, &msg);
	return false;
}

void HandlerNpc::onMsgNpcFeaturesRes(const MsgNpcFeaturesResultS2C& msg)
{
	//LOGDE("-------HandlerNpc::onMsgNpcFeaturesRes()-------");
	//switch (msg.result())
	//{
	//case MsgNpcFeaturesG2C_RESULT_SUCCESS:
	//	break;
	//case MsgNpcFeaturesG2C_RESULT_FAILURE:
	//	break;
	//default:
	//	break;
	//}
}

bool HandlerNpc::requestNpcBadgeUpgrade(int npcId, NS::I64 roomPos)
{
	//LOGDE("-------HandlerNpc::requestNpcBadgeUpgrade()-------");
	return true;
}

bool HandlerNpc::requestNpcRentsAndGifts()
{
	MsgProtectorMultiRentC2S msg;
	if (!NpcProtectorDataMgr::getInstance()) return false;

	auto&& npcBadges = NpcProtectorDataMgr::getInstance()->getProtectorDataMap();
	if (npcBadges.size() > 0)
	{
		for (auto&& badge : npcBadges)
		{
			if (!badge.second) continue;

			//if (badge.second->status == BADGE_STATUS_REFRESH_RENT || badge.second->status == BADGE_STATUS_REFRESH_GIFT)
			//{
				msg.add_uuid(badge.first);
			//}
		}
	}

	return MsgMgr::getInstance().sendMessage(_MSG_PROTECTOR_MULTI_RENT_C2S, &msg);
}

void HandlerNpc::onMsgNpcRentsAndGiftsRes(const MsgPB::MsgProtectorMultiRentS2C & msg)
{
	//UiMsgBoxCommonMessage::StyleParam param;
	//param.m_giftVec.clear();

	//MsgPB::E_ErrorCode result{ msg.result() };
	//if (result == MsgPB::E_ErrorCode::EC_Success)
	//{
	//	int nRentCoins{ msg.coin() };
	//	param.addSimpleItemInfo(1, nRentCoins);

	//	int nSize = msg.item_size();
	//	std::unordered_map<NS::int32, int> gifts;
	//	for (int i{ 0 }; i < nSize; ++i)
	//	{
	//		const ItemReward& it = msg.item(i);

	//		if (gifts.find(it.itemid()) == gifts.end())
	//			gifts[it.itemid()] = it.itemcount();
	//		else
	//			gifts[it.itemid()] += it.itemcount();
	//	}

	//	for (auto&& gift : gifts)
	//	{
	//		param.addSimpleItemInfo(gift.first, gift.second);
	//	}

	//	AudioModule* pAudioModule = Framework::getInstance().findComponent<AudioModule>();
	//	if (pAudioModule)
	//	{
	//		pAudioModule->PlayAudioByKey("trade");
	//	}

	//	param.m_style = UiMsgBoxCommonMessage::SHOW_RENT_GIFT_COMFIRM;
	//	param.m_textForGiftStyle = _LC("CLI_NpcBadgeSys-You have Claimed rewards from NPC");
	//	showRentAndGiftDialog(param);
	//}
}

//npc创建事件
void HandlerNpc::onMsgNpcProtectorAppearance(const MsgMobAppearance& msg)
{
	//LOGDE("-------HandlerNpc::onMsgNpcProtectorAppearance()-------");
	//if (msg.has_hp() && msg.hp() <= 0)
	//{
	//	LOG_EVENT_D("is dead npc appearance enable ? {0}.", msg.mapobjectid());
	//	return;
	//}

	//// 删除私人地图已存在的同id npc
	//int mapObjId{ msg.mapobjectid() }, insId{ msg.mobinsid() }, defId{ msg.mobdefid() };
	//do
	//{
	//	//auto curMapId = MapSettingCfg::getInstance().getCurSceneId();
	//	//if (PriveMapId != curMapId)
	//	//	break;
	//	//auto preNpcObj = NpcComponentMgr::getInstance().findNpcByDefId(defId);
	//	//if (!preNpcObj)
	//	//	break;
	//	//NpcComponentMgr::getInstance().removeNpc(preNpcObj);
	//} while (0);

	//glm::vec3 pos{ msg.movepath().pos().x(), msg.movepath().pos().y(), 0.0f };
	//auto mob = LocalServer_BirthMob::getInstance().getMobInfo(msg.mapobjectid());
	//if (mob)
	//{
	//	pos.z = mob->m_fOffsetBirthWorldPosZ;
	//}
	//bool isBadgeNpc = UiCommon::getInstance().isBadgeByNpcId(defId, mapObjId);
	//std::shared_ptr<Npc> npc = std::make_shared<Npc>(mapObjId, insId, defId);
	//npc->setPos(MapData::serverWorldPos2ClientWorldPos(pos.x, pos.y));
	//npc->setIsBadgeNpc(isBadgeNpc);
	//npc->setOwnerMapObjId(msg.ownermapobjid());
	//if (!npc)
	//{
	//	LOG_EVENT_D("add npc failed, mapObjId: {0}, insId: {1}, defId: {2}. position({3},{4})", mapObjId, insId, defId, pos.x, pos.y);
	//	return;
	//}
	////npc->setHp(msg.hp());
	//npc->setMp(msg.mp());
	//npc->setGroupId(msg.groupid());
	//if (npc->getGroupId() == (int)Role::GroupType::Mob)
	//	npc->setIsMyNpc(false);
	//if (HandlerLogin::getInstance().getMapType() == MapDataMgr::MapType::TowerPVP || HandlerLogin::getInstance().getMapType() == MapDataMgr::MapType::SquadPvp)
	//{
	//	if (HandlerLogin::getInstance().getMapType() == MapDataMgr::MapType::TowerPVP)
	//	{
	//		npc->setIsAtPvpTower(true);
	//	}
	//	if (npc->getGroupId() == (int)Role::GroupType::Mob)
	//	{
	//		auto otherPlayers = PlayerMgr::getInstance().getOtherPlayer();
	//		auto iter = otherPlayers.begin();
	//		if (iter != otherPlayers.end())
	//		{
	//			npc->setEnemyPlayerLevel(iter->second->getLevel());
	//		}
	//	}
	//}
	//	
	//NpcComponentMgr::getInstance().addNpc(npc, msg.has_islocalserver() && msg.islocalserver());

	//HandlerRegDefEvent createEvt;
	//createEvt.m_type = HandlerRegDefEvent::type::createNpc;
	//createEvt.mapObjId = msg.mapobjectid();
	//HandlerRole::getInstance().postEvent(createEvt);

	//// add buff
	//auto&& gameObjNpc = NpcComponentMgr::getInstance().findNpc(msg.mapobjectid());
	//if (!gameObjNpc) { return; }
	//auto pNpcComponent = gameObjNpc->findComponent<NpcComponent>();
	//assert(pNpcComponent);
	//if (pNpcComponent)
	//{
	//	auto&& pTransformComp = pNpcComponent->findComponent<TransformComponent>();
	//	if (pTransformComp)
	//	{
	//		auto posCur = pTransformComp->getPosition();
	//		posCur.z += pos.z;
	//		pTransformComp->setPosition(posCur);
	//	}

	//	CharacterAttributeComponent* pCharacterAttributeComp = pNpcComponent->findComponent<CharacterAttributeComponent>();
	//	assert(pCharacterAttributeComp);
	//	if (pCharacterAttributeComp)
	//	{
	//		// 			pCharacterAttributeComp->ResetMaxHp();
	//		// 			pCharacterAttributeComp->SetHp(msg.hp());
	//		int nMaxHp = pCharacterAttributeComp->CalculateMaxHp();
	//		std::string tag = fmt::format("HandlerNpc::onMsgNpcProtectorAppearance: nMapObjID = {0}", msg.mapobjectid());
	//		if (msg.has_hp())
	//		{
	//			pCharacterAttributeComp->InitHp(msg.hp(), nMaxHp, tag);
	//		}
	//		else
	//		{
	//			// 默认NPC出生满血
	//			pCharacterAttributeComp->InitHp(nMaxHp, nMaxHp, tag);
	//		}
	//	}
	//}
	////	int size = msg.buff_infos_size();
	//for (int i{}; i < msg.buff_infos_size(); ++i)
	//{
	//	auto&& buffInfo = msg.buff_infos(i);
	//	auto&& buffType = BuffCfg::getInstance().findBuffType(buffInfo.buff_dataid());
	//	if (!buffType)
	//	{
	//		LOG_EVENT_D("Invalid buff type: {0}.", buffInfo.buff_dataid());
	//		continue;
	//	}

	//	Buff buff{ buffType };
	//	buff.setIdx(buffInfo.index());
	//	buff.setAliveTime(buffInfo.alive_timer());
	//	buff.setHeapCounter(buffInfo.heap_counter());
	//	gameObjNpc->findComponent<NpcComponent>()->addBuff(buff);
	//}

	//npc->onInit();

	//bzbee::int32 nOwnerMapObjID = msg.ownermapobjid();
	//if (nOwnerMapObjID > 0)
	//{
	//	GameObject* pGameObject = HandlerRole::getInstance().findRole(bzbee::uint(nOwnerMapObjID));
	//	if (pGameObject)
	//	{
	//		RoleComponent* pRoleComp = pGameObject->findComponent<RoleComponent>();
	//		assert(pRoleComp);
	//		if (pRoleComp)
	//		{
	//			RootAgentComponent* pRootAgentComp = pRoleComp->findComponent<RootAgentComponent>();
	//			//assert(pRootAgentComp);
	//			if (pRootAgentComp)
	//			{
	//				pNpcComponent->setOwnerAngent(pRootAgentComp);
	//			}
	//		}
	//	}
	//}


	//if (msg.has_strongholdid())
	//{
	//	StrongholdMgr::getIntance().addObj(msg.strongholdid(), gameObjNpc);
	//}

	//HandlerRole::getInstance().movePath(mapObjId, msg.movepath());

	////徽章NPC出现，巡逻NPC移除
	//LuaCallback("NpcAppear")(mapObjId, defId);
	//if (isBadgeNpc)
	//{
	//	auto readyDelNpcId = BuildRoomCfg::getInstance().getReplaceNpcId(defId);
	//	if (readyDelNpcId != -1)
	//	{
	//		LuaCallback("NpcAppear")(mapObjId, readyDelNpcId);
	//	}
	//}

	//auto newNpc = NpcMgr::getInstance().getNpc(GlobalCfg::getInstance().getNewbieNpcObjId());
	//if (newNpc && newNpc->getDefId() == defId + 1) // 新手NPC 20001 == 徽章凡娜 2000 只能出现一个.
	//{
	//	auto data = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByDefId(defId);
	//	if (data && (data->pos_x != 0 || data->pos_y != 0))
	//	{
	//		NpcComponentMgr::getInstance().removeNpc(GlobalCfg::getInstance().getNewbieNpcObjId());
	//	}
	//}

	//if (npc->getIsMyNpc() && npc->getDefId() == ACHIEVE_NPC_ID)
	//{
	//	npc->setCalcAttrTriggerAchieveCallback(std::bind(&HandlerNpc::onTriggerAchieveEvent, this, std::placeholders::_1));
	//}
	//auto pNpcData = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByDefId(npc->getDefId());
	//auto me = PlayerMgr::getInstance().getSelf();
	//if (isBadgeNpc && npc->getIsMyNpc() && me && pNpcData)
	//{
	//	int power = UiBaseItem::getNpcPower(pNpcData->templateid, pNpcData, me->getLevel());
	//	CommonTaskEvent ctevt(MsgPB::EN_PLAYER_EVENT_TYPE_npc_power, power);
	//	ctevt.m_updateMode = UpdateMode::Set;
	//	TaskEventComponent::getInstance().postEvent(ctevt);
	//}
}

//触发一个成就事件
void HandlerNpc::onTriggerAchieveEvent(const Npc* npc)
{
	//LOGDE("-------HandlerNpc::onTriggerAchieveEvent()-------");
	//if (!npc)
	//{
	//	return;
	//}

	//if (npc->getDefId() == ACHIEVE_NPC_ID)
	//{
	//	LocalServer_Achieve::getInstance().TriggerAchieveEvent(ACHIEVE_EVENT_TEMPLATE_ID_79035, static_cast<int>(npc->getAttr(MsgPB::ROLE_EFFECT_AD)), AchieveEventArg::EM_MODE::EM_MODE_SET, 0);
	//}
}

//请求收取NPC租金和礼物
bool HandlerNpc::requestNpcProtectorClaimRentAndGift(NS::I64 nUuid)
{
	//LOGDE("-------HandlerNpc::requestNpcProtectorClaimRentAndGift()-------");
	MsgProtectorRentC2S msg;
	msg.set_uuid(nUuid);
	return MsgMgr::getInstance().sendMessage(_MSG_PROTECTOR_RENT_C2S, &msg);
}

//收取NPC租金和礼物结果
void HandlerNpc::onMsgNpcProtectorClaimRentAndGiftRes(const MsgProtectorRentResultS2C& msg)
{
	if (msg.coin() > 0)
	{
		NotesDataMgr::getInstance().addItem(NOTES_DATA_TPY_RENT, 0, 0, msg.coin(), 0);
	}
}

//Npc巡逻请求;
bool HandlerNpc::requestNpcProtectorPatrolResult(NS::I64 nUuid)
{
	//LOGDE("-------HandlerNpc::requestNpcProtectorPatrolResult()-------");
	MsgProtectorPatrolC2S msg;
	msg.set_mode(MsgProtectorPatrolC2S_MODE_GOHOME);
	msg.set_uuid(nUuid);
	return MsgMgr::getInstance().sendMessage(_MSG_PROTECTOR_PATROL_C2S, &msg);
}

//Npc巡逻结果;
void HandlerNpc::onMsgProtectorPatrolResult(const MsgProtectorPatrolResultS2C& msg)
{
	//LOGDE("-------HandlerNpc::onMsgProtectorPatrolResult()-------");
	auto result = msg.result();
	if (result == MsgProtectorPatrolResultS2C::RESULT::MsgProtectorPatrolResultS2C_RESULT_FAILURE)
	{
		return;
	}
	else
	{
		//UiMsgBoxToast::getInstance().showText(_LC("CLI_NpcBadgeSys-npc called back"), "TEST");
	}
}

//同步npc徽章数据
void HandlerNpc::onMsgSyncNpcProtectorData(const MsgProtectorSyncS2C& msg)
{
	NpcProtectorDataMgr::getInstance()->syncNpcProtectorData(msg);
	HandlerProtectorEvent evt;
	evt.m_type = HandlerProtectorEvent::EventProtector::EventProtector_Update;
	POST_DATA_EVENT(evt);
}

//更新npc徽章数据
void HandlerNpc::onMsgUpdateNpcProtectorData(const MsgProtectorUpdateS2C& msg)
{
	//LOGDE("-------HandlerNpc::onMsgUpdateNpcProtectorData()-------");


	bool bPreDeath = false;
	int preLv = 0;
	int nPreHouse = -1;
	bool bPreHave = false;
	auto data = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByUuid(msg.data().uuid());
	if (data) {
		bPreHave = true;
		nPreHouse = data->houseId;
		preLv = data->level;
		bPreDeath = data->status == BADGE_STATUS_DEATH;
	}
	NpcProtectorDataMgr::getInstance()->updateNpcProtectorData(msg);

	if (!bPreHave) {
		HandlerProtectorEvent evt;
		evt.m_type = HandlerProtectorEvent::EventProtector::EventProtector_Add;
		evt.uuid = msg.data().uuid();
		POST_DATA_EVENT(evt);
	} else {
		HandlerProtectorEvent evt;
		evt.m_type = HandlerProtectorEvent::EventProtector::EventProtector_Update;
		evt.uuid = msg.data().uuid();
		POST_DATA_EVENT(evt);
	}

	if (nPreHouse == 0 && msg.data().houseid() > 0) {
		auto&& npcType = NpcTypeCfg::getInstance().GetNpcType(data->itemid);
		if (npcType) {
			std::string str = _LC(npcType->s_name.c_str());
			str += _LC("UI_HOUSE_IN_TIPS");
			GameAppExtern::ShowToastTips(str);
		}
		NotesDataMgr::getInstance().addItem(NOTES_DATA_TPY_NPC_IN, data->uuid, 0, msg.data().houseid());
	} else if (nPreHouse > 0 && msg.data().houseid() == 0) {
		auto&& npcType = NpcTypeCfg::getInstance().GetNpcType(data->itemid);
		if (npcType) {
			std::string str = _LC(npcType->s_name.c_str());
			str += _LC("UI_HOUSE_OUT_TIPS");
			GameAppExtern::ShowToastTips(str);
		}
		NotesDataMgr::getInstance().addItem(NOTES_DATA_TPY_NPC_OUT, data->uuid, 0, nPreHouse);
	}

	if (msg.data().level() > preLv && preLv != 0)
	{
		NotesDataMgr::getInstance().addItem(NOTES_DATA_TPY_NPC_LV, data->uuid, 0, msg.data().level());
	}

	if (bPreDeath && msg.data().status() != BADGE_STATUS_DEATH) {  //武将复活
		std::shared_ptr<Npc> npc = NpcMgr::getInstance().getNpcByProtectorUuid(data->uuid);
		if (npc) {
			npc->setHp(npc->getMaxHp());
			HandlerNpcAliveEvent evt;
			evt.m_uuid = data->uuid;
			POST_DATA_EVENT(evt);
		}
	}

	//if (bPreDeath && msg.data().status() != BADGE_STATUS_DEATH)
	//{
	//	HandlerNpcReliveEvent evt;
	//	evt.m_npcId = msg.data().templateid();
	//	postEvent(evt);
	//	auto&& mobCfg = MonsterCfg::getInstance().findMonsterType(msg.data().templateid());
	//	if (mobCfg) {
	//		//UiMsgBoxToast::getInstance().showText(fmt::format(_LC("UI_UiNpc_show-npc{0} has relive"), mobCfg->s_name));
	//	}
	//	// 重生后Last 置None
	//}
	//else if (!bPreDeath && msg.data().status() == BADGE_STATUS_DEATH)
	//{
	//	HandlerNpcDeathEvent evt;
	//	evt.m_npcId = msg.data().templateid();
	//	postEvent(evt);
	//}
	//if (preLv > 0 && msg.data().level() != preLv)
	//{
	//	HandlerNpcAtterEvent evt;
	//	postEvent(evt);
	//}
}

//void HandlerNpc::onNpcBadgeEnableEvent(const NpcBadgeEnableFuncEvent& evt)
//{
//	if (!UiCommon::getInstance().getIsFollowPlayerFromFile(evt.tempId))
//	{
//		return;
//	}
//	auto&& data = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByDefId(evt.tempId);
//	if (!data)
//	{
//		assert(0);
//		return;
//	}
//	auto npcGameObj = NpcComponentMgr::getInstance().findNpc(data->mapObjectID);
//	if (!npcGameObj) {
//		if (data->CanAppear() && data->status != BADGE_STATUS_SP)
//		{
//			HandlerNpc::getInstance().setFollowNpc(data->templateid);
//		}
//	}
//	else
//	{
//		NS::ComponentPtr<NpcComponent> npcComp = npcGameObj->findComponent<NpcComponent>();
//		if (npcComp) {
//			if (!npcComp)return;
//			bool bFollow = npcComp->isFollowPlayer();
//			if (!bFollow)
//			{
//				npcComp->setIsFollowPlayer(true);
//			}
//		}
//	}
//}

//移除npc徽章数据
void HandlerNpc::onMsgRemoveNpcProtectorData(const MsgProtectorRemoveS2C& msg)
{
	//LOGDE("-------HandlerNpc::onMsgRemoveNpcProtectorData()-------");
	NpcProtectorDataMgr::getInstance()->removeNpcProtectorData(msg);
	HandlerProtectorEvent evt;
	evt.m_type = HandlerProtectorEvent::EventProtector::EventProtector_Delete;
	evt.uuid = msg.uuid();
	POST_DATA_EVENT(evt);
}

//npc互动请求
bool HandlerNpc::requestMobInteractiveC2S(NS::int32 npcMapObjId, NS::int32 npcActiveEffectId)
{
	//LOGDE("-------HandlerNpc::requestMobInteractiveC2S()-------");
	//if (!HandlerLogin::getInstance().isConnect())
	//{
	//	//UiMsgBoxToast::getInstance().showText(_LC("CLI_Common-network is diconnect."));
	//	return false;
	//}
	MsgMobInteractiveC2S msg;
	msg.set_mapobjectid(npcMapObjId);
	msg.set_effectid(npcActiveEffectId);
	return MsgMgr::getInstance().sendMessage(msg);
}

//npc互动结果
void HandlerNpc::onMsgMobInteractiveS2C(const MsgMobInteractiveS2C& msg)
{
	//LOGDE("-------HandlerNpc::onMsgMobInteractiveS2C()-------");
	auto result = msg.result();
	switch (result)
	{
	case MsgMobInteractiveS2C_RESULT_FAILURE:
	{
	}
	break;

	case MsgMobInteractiveS2C_RESULT_SUCCESS:
	{
		//LocalServer_NpcPetFight::getInstance().onMsgMobInteractiveS2C();
	}
	// 	{
	// 		auto uiid = msg.uiid();
	//
	// 		auto pUI = InteractiveBuildingCfg::getInstance().findUI(uiid);
	// 		const NS::tstring& uiTag = pUI->m_ui;
	//
	// 		if (uiTag == "UiMsgBoxNpcDungeon")
	// 		{
	// 			UiMsgBoxNpcDungeon* pDialog = UiMgr::getInstance().findDialog<UiMsgBoxNpcDungeon>();
	// 			if (!pDialog)
	// 			{
	// 				pDialog = UiMsgBoxNpcDungeon::create();
	// 				UiMgr::getInstance().showModalDialog(pDialog);
	// 			}
	// 			else
	// 			{
	// 				pDialog->refreshView();
	// 			}
	// 		}
	// 		else if (uiTag == "CraftUpgrade")
	// 		{
	// 		}
	// 		{
	// 			//TODO:
	// 		}
	// 	}
	break;

	case MsgMobInteractiveS2C_RESULT_PACKET_FULL:
	{
		//UiMsgBoxToast::getInstance().showText(_LC("CLI_NPC_INTERACTIVE_RESULT_PACKET_FULL"));
	}
	break;
	default:

		break;
	}
}

//显示租金和礼物界面
//void HandlerNpc::showRentAndGiftDialog(UiMsgBoxCommonMessage::StyleParam param)
//{
//	//LOGDE("-------HandlerNpc::showRentAndGiftDialog()-------");
//	auto pDialog = UiMgr::getInstance().findDialog<UiMsgBoxCommonMessage>();
//	if (!pDialog)
//	{
//		pDialog = UiMsgBoxCommonMessage::create();
//	}
//
//	pDialog->refreshView(param);
//	UiMgr::getInstance().showModalDialog(pDialog);
//}


//npc不满足入住条件提示
void HandlerNpc::mapRoomServerMsgInfo2UiviewTipInfo(const MsgBuildingRoomCondition_S2C& msg, CheckAllCondInfo &viewInfo)
{
	//LOGDE("-------HandlerNpc::mapRoomServerMsgInfo2UiviewTipInfo()-------");
	//auto pCondition =  msg.condition_type();
	//for (int i = 0; i < msg.condition_type_size(); i++)
	//{
	//	const MsgPB::EN_ROOM_CONDITION_TYPE& pCondition = msg.condition_type(i);
	//	viewInfo.m_setReq.insert((CheckAllCondInfo::PUT_NPC_BADGE_FAIL_DATA)(pCondition));
	//}
	//viewInfo.m_bHasDoor = msg.has_door();
	////auto pMapBuild = msg.build_type();

	//int pbadgeId = StaticObjMgr::getInstance().findBuildingByGlobalCoord(glm::ivec2(msg.pos_x(), msg.pos_y()))->getTypeId();
	//auto cfg = BuildRoomCfg::getInstance().findBuilRoomDef(pbadgeId);


	//for (int i = 0; i < std::end(cfg->n_furnitures) - std::begin(cfg->n_furnitures); i++)
	//{
	//	if (cfg->n_furnitures[i] != 0)
	//	{
	//		viewInfo.m_mapBuild[cfg->n_furnitures[i]] = true;
	//	}
	//}

	//for (int i = 0; i < msg.build_type_size(); i++)
	//{
	//	const int32 & pMapBuild = msg.build_type(i);
	//	viewInfo.m_mapBuild[pMapBuild] = false;
	//}
}

//npc不满足入住条件提示
void HandlerNpc::mapRoomServerTipInfo2UiViewTipInfo(CheckAllConditionRet & serverInfo, CheckAllCondInfo & viewInfo)
{
	//LOGDE("-------HandlerNpc::mapRoomServerMsgInfo2UiviewTipInfo()-------");
	/*std::set<CheckAllConditionRet::PUT_NPC_BADGE_FAIL_DATA>::iterator it;
	for (it = serverInfo.m_setReq.begin(); it != serverInfo.m_setReq.end(); it++)
	{
		viewInfo.m_setReq.insert((CheckAllCondInfo::PUT_NPC_BADGE_FAIL_DATA)((int)(*it)));
	}
	std::map<NS::I64, bool>::iterator kt;
	for (kt = serverInfo.m_mapBuild.begin(); kt != serverInfo.m_mapBuild.end(); kt++)
	{
		viewInfo.m_mapBuild[kt->first] = kt->second;
	}*/
}

void HandlerNpc::onMsgGetNpcProtectorStatusResult(const MsgProtectorGetStatusResultS2C& msg)
{
	//LOGDE("-------HandlerNpc::onMsgGetNpcProtectorStatusResult()-------");
}

void HandlerNpc::onMsgBuildingRoomConditionS2C(const MsgBuildingRoomCondition_S2C & msg)
{
	//LOGDE("-------HandlerNpc::onMsgBuildingRoomConditionS2C()-------");
	/*auto pBuildObj = StaticObjMgr::getInstance().findBuildingByGlobalCoord(glm::ivec2(msg.pos_x(), msg.pos_y()));
	CheckAllCondInfo viewInfo;
	mapRoomServerMsgInfo2UiviewTipInfo(msg, viewInfo);

	UiNpcBadgePlaceTip * pDialog = UiMgr::getInstance().showDialog<UiNpcBadgePlaceTip>(true);
	pDialog->showEnlistTip(viewInfo, pBuildObj->getTypeId());*/
}

//辉章放不上去的错误提示;
//void HandlerNpc::showPlaceBadgeErrorTip(CheckConditionRet &rc)
//{
//	//LOGDE("-------HandlerNpc::showPlaceBadgeErrorTip()-------");
//	// 如果前提条件不是个房间，就无所谓判断后面的条件，所以不插入;
//	std::map<int, bool> condMap{};
//	if (!rc.m_isRoom)
//	{
//		condMap.insert(std::pair<int, bool>(MsgPB::EN_ROOM_CONDITION_TYPE_NO_ROOM, false));
//	}
//	else
//	{
//		condMap[MsgPB::EN_ROOM_CONDITION_TYPE_NO_ROOM] = rc.m_isRoom;
//		condMap[MsgPB::EN_ROOM_CONDITION_TYPE_NO_LIGHT] = rc.m_hasLight;
//		condMap[MsgPB::EN_ROOM_CONDITION_TYPE_NO_DOOR] = rc.m_hasDoor;
//		condMap[MsgPB::EN_ROOM_CONDITION_TYPE_TOO_MANY_FLAG] = rc.m_onlyHaveOneBadge;
//	}
//
//	auto pDialog = UiMgr::getInstance().showDialog<UiNpcBadgePlaceTip>(true);
//	pDialog->showPutTip(condMap);
//}

//请求npc状态结果
bool HandlerNpc::requestNpcProtectorStatusResult(NS::I64 nUuid)
{
	//LOGDE("-------HandlerNpc::requestNpcProtectorStatusResult()-------");
	/*MsgBuildingRoomConditionGet_C2S msg;
	msg.set_iggid(PlayerMgr::getInstance().getSelf()->getIggId());
	msg.set_pos_x(NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByUuid(nUuid)->pos_x);
	msg.set_pos_y(NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByUuid(nUuid)->pos_y);
	bool pIsSuccess = MsgMgr::getInstance().sendMessage(_MSG_BUILD_ROOM_CONDITION_GET_C2S, &msg);
	return pIsSuccess;*/

	auto data = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByUuid(nUuid);
	if (!data)
		return false;
	//CheckConditionRet cr;
	//NpcBadgeRoomCheck::getBadgeRoomComponent()->checkRoomConditionAll(data->pos_x, data->pos_y, cr);
	//UiNpcBadgePlaceTip * pDialog = UiMgr::getInstance().showDialog<UiNpcBadgePlaceTip>(true);
	//pDialog->showEnlistTip(cr, nUuid);

	return true;
}

//npc互动效果
bool HandlerNpc::onNpcInteracted(NS::int32 npcMapObjId, bool isLocalCreate)
{
	//LOGDE("-------HandlerNpc::onNpcInteracted()-------");
	//if (!HandlerLogin::getInstance().isConnect())
	//{
	//	UiMsgBoxToast::getInstance().showText(_LC("CLI_Common-network is diconnect."));
	//	return false;
	//}

	//auto npcData = NpcMgr::getInstance().getNpc(npcMapObjId);
	//if (!npcData)
	//{
	//	assert(false);
	//	return false;
	//}

	//auto npcGameObj = NpcComponentMgr::getInstance().findNpc(npcMapObjId);
	//if (!npcGameObj) return false;
	//auto npcComp = npcGameObj->findComponent<NpcComponent>();
	//if (!npcComp) return false;
	//int badgeLevel = 1;
	//auto badgeData = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByDefId(npcData->getDefId());
	//if (badgeData)
	//	badgeLevel = badgeData->level;
	//auto npcActiveDef = NpcActiveCfg::getInstance().findNpcActive(npcData->getDefId(), badgeLevel);
	//if (!npcActiveDef)
	//	return false;
	//bool hideAllEffect = npcActiveDef->m_npcActiveEffects.size() == 0 ? true : false;
	///*if (npcActiveDef->m_npcActiveEffects.size() == 0)
	//	return false;*/

	//	//记录交互npc DefId;
	//PlayerPrefs::SetInt("InteractedNpcDefId", npcData->getDefId());
	//auto&& pMonsterType = MonsterCfg::getInstance().findMonsterType(npcData->getDefId());
	//if (pMonsterType)
	//{
	//	PlayerPrefs::SetInt("InteractedNpcVoiceType", pMonsterType->n_npc_voice_type);
	//}

	//auto cfg = npcData->getMonsterType();
	//if (cfg && cfg->n_type == MonsterFuncType::TeleportNpc)
	//{
	//	if (!HandlerRole::getInstance().isHaveChangeName())
	//	{
	//		auto pDialog = UiMgr::getInstance().showDialog<UiNpcChangeName>(true);
	//		pDialog->refreshView(npcMapObjId, isLocalCreate);
	//		for (size_t i = 0; i < npcActiveDef->m_npcActiveEffects.size(); i++)
	//		{
	//			auto pEffectInfo = npcActiveDef->m_npcActiveEffects[i];
	//			auto cfg = InteractiveEffectCfg::getInstance().findInteractiveEffect(pEffectInfo.n_effect_id);
	//			if (cfg && cfg->m_type == InteractiveEffectCfg::EffectType::GOMAP)
	//			{
	//				Json::Value id = pEffectInfo.n_effect_id;
	//				HandlerPlayerCacheSync::getInstance().setGlobalCache("telportEffectId", id);
	//				break;
	//			}
	//		}
	//		return true;
	//	}
	//}

	//if (npcActiveDef->isPopCommonWin == 1)
	//{
	//	auto pDialog = UiMgr::getInstance().showDialog<UiNpcCommonWindow>(true);
	//	pDialog->refreshView(npcMapObjId, isLocalCreate, hideAllEffect);
	//}
	//else if (npcActiveDef->isPopCommonWin == 4)
	//{
	//	auto pDialog = UiMgr::getInstance().showDialog<UiNpcCommonWindow>(true);
	//	pDialog->refreshView(npcMapObjId, isLocalCreate, true);
	//}
	//else if (npcActiveDef->isPopCommonWin == 2)//矿洞老头隐藏任务特殊处理;
	//{
	//	auto pDialog = UiMgr::getInstance().showDialog<UiOreHoleTask>(true);
	//	pDialog->setTaskType(UiOreHoleTask::E_Task_Type::E_Task_Ore_Old_Man);
	//	pDialog->refreshView(npcMapObjId, isLocalCreate);
	//}
	//else if (npcActiveDef->isPopCommonWin == 3)//NPC冒险任务处理;
	//{
	//	if (HandlerNpcRemuneration::getInstance().isRecMsg())
	//	{
	//		HandlerNpcRemuneration::getInstance().openNpcTaskDlg(npcMapObjId);
	//	}
	//	else
	//	{
	//		HandlerNpcRemuneration::getInstance().reqState(npcMapObjId);
	//	}
	//}
	//else if (npcActiveDef->isPopCommonWin == 5)//驿站NPC;
	//{
	//	// 如果驿站商队NPC已经不存在了，就不需要再打开界面了;
	//	if (NpcLogic::getInstance().GetCaravanNpcExist(npcMapObjId))
	//	{
	//		auto pDialog = UiMgr::getInstance().showDialog<UiNpcDsk>(true);
	//		pDialog->refreshView(npcMapObjId, isLocalCreate);
	//	}
	//}
	//else if (npcActiveDef->isPopCommonWin == 6) //飞船回购;
	//{
	//	HandlerNpcDak::getInstance().requestNpcDakRepo();
	//}
	//else if (npcActiveDef->isPopCommonWin == 7) // 成就npc Lynxia,给面包任务
	//{
	//	auto pDialog = UiMgr::getInstance().showDialog<UiAchieveGive>(true);
	//	pDialog->refreshView(npcMapObjId, isLocalCreate, hideAllEffect);
	//}else if (npcActiveDef->isPopCommonWin == 8) //矿洞解救艾伦
	//{
	//	auto pDialog = UiMgr::getInstance().showDialog<UiOreHoleTask>(true);
	//	pDialog->setTaskType(UiOreHoleTask::E_Task_Type::E_Task_Save_Allen);
	//	pDialog->refreshView(npcMapObjId, isLocalCreate);
	//}
	//else
	//{
	//	HandlerNpc::getInstance().requestMobInteractiveC2S(npcMapObjId, npcActiveDef->m_npcActiveEffects[0].n_effect_id);
	//}

	return true;
}

void HandlerNpc::setUnFollowNpc()
{
	//auto datas = NpcComponentMgr::getInstance().getAllNpcs();
	//auto protectDatas = NpcProtectorDataMgr::getInstance()->getProtectorDataMap();
	//for (auto protectData : protectDatas)
	//{
	//	if (protectData.second->status == PROTECTOR_STATUS::BADGE_STATUS_SP && NpcProtectorDataMgr::getInstance()->isDragonBallNpc(protectData.second->templateid))
	//		continue;
	//	for (auto npcGameObj : datas)
	//	{
	//		auto npcComp = npcGameObj.second->findComponent<NpcComponent>();
	//		if (npcComp && npcComp->getNpcData()->getDefId() == protectData.second->templateid && npcComp->getNpcData()->isFollowPlayer())
	//		{
	//			npcComp->getNpcData()->setFollowPlayer(false);
	//			auto agentComp = npcGameObj.second->findComponent<NpcAgentComponent>();
	//			auto agent = agentComp != nullptr ? agentComp->getAgent() : nullptr;
	//			auto npcData = npcComp->getNpcData();
	//			if (agent && npcData)
	//			{
	//				agent->SendEvent(ai::BaseAgent::NPC_FOLLOW_PLAYER, npcData->isFollowPlayer());
	//			}
	//			npcComp->getNpcData()->setIsCanOpenDoor(false);
	//			npcComp->updateHudState();
	//			if (npcComp->getNpcData()->isAlive())
	//				npcComp->openNpcMoveTimer();
	//			auto renderComp = npcComp->findComponent<RoleRenderComponent>();
	//			if (renderComp && renderComp->getModel())
	//				renderComp->getModel()->setTimeScale(1.0f);
	//			auto mapUiComp = npcComp->findComponent<MapUiComponent>();
	//			if (mapUiComp)
	//			{
	//				auto ui = dynamic_cast<UiRolePendant*>(mapUiComp->getUi());
	//				if (ui)
	//				{
	//					auto btn = ui->getFollowBtn();
	//					if (btn)
	//						btn->setChecked(true);
	//				}
	//			}
	//		}
	//	}
	//}
}

//设置跟随NPC;  与NPC不在同一地图上
void HandlerNpc::setFollowNpc(int npcDefId)
{
	setUnFollowNpc();
	//UiCommon::getInstance().saveFollowPlayerDataToFile(npcDefId, true);
}

//扫描地图上所有的徽章
void HandlerNpc::scanAllBadgeOfAtMap()
{
	//LOGDE("-------HandlerNpc::scanAllBadgeOfAtMap()-------");
	//auto curMapId = MapSettingCfg::getInstance().getCurSceneId();
	//for (auto&& badgeData : NpcProtectorDataMgr::getInstance()->getProtectorDataMap())
	//{
	//	auto data = badgeData.second;
	//	//徽章没在地图上;
	//	if (data->pos_x == 0 && data->pos_y == 0)
	//		continue;
	//	//徽章不在当前地图的，不处理;
	//	if (curMapId != data->mapid)
	//		continue;
	//	NpcBadgeRoomCheck::getBadgeRoomComponent()->pushReadyCheckData(false, true, (int)data->uuid, data->pos_x, data->pos_y);
	//}
}

//徽章监听地形变化事件;
void HandlerNpc::onBuildingChangeEvt(const HandlerBuildingEvent &evt)
{
	//LOGDE("-------HandlerNpc::onBuildingChangeEvt()-------");
	//auto curMapId = MapSettingCfg::getInstance().getCurSceneId();
	//for (auto&& badgeData : NpcProtectorDataMgr::getInstance()->getProtectorDataMap())
	//{
	//	auto data = badgeData.second;
	//	if (!evt.isBadge) //放置的不是辉章;
	//	{
	//		//徽章没在墙上，不处理拆放家具事件;
	//		if (data->pos_x == 0 && data->pos_y == 0)
	//			continue;
	//		//徽章不在当前地图的，不处理;
	//		if (curMapId != data->mapid)
	//			continue;
	//		//徽章和拆放的家具距离超过200的不处理;
	//		if (abs(evt.coordX - data->pos_x) > 200 || abs(evt.coordY - data->pos_y) > 200)
	//			continue;
	//		NpcBadgeRoomCheck::getBadgeRoomComponent()->pushReadyCheckData(evt.isBadge, evt.isPlace, (int)data->uuid, evt.coordX, evt.coordY);
	//	}
	//	else
	//	{
	//		if ((int)data->uuid != evt.badgeId) //操作的辉章不是自己;
	//		{
	//			//徽章没在墙上，不处理拆放家具事件;
	//			if (data->pos_x == 0 && data->pos_y == 0)
	//				continue;
	//			//徽章不在当前地图的，不处理;
	//			if (curMapId != data->mapid)
	//				continue;
	//			//徽章和拆放的家具距离超过200的不处理;
	//			if (abs(evt.coordX - data->pos_x) > 200 || abs(evt.coordY - data->pos_y) > 200)
	//				continue;;
	//			NpcBadgeRoomCheck::getBadgeRoomComponent()->pushReadyCheckData(false, evt.isPlace, (int)data->uuid, evt.coordX, evt.coordY);
	//		}
	//		else
	//		{
	//			NpcBadgeRoomCheck::getBadgeRoomComponent()->pushReadyCheckData(evt.isBadge, evt.isPlace, (int)data->uuid, evt.coordX, evt.coordY);
	//		}
	//	}
	//}
}

void HandlerNpc::handlerReliveNpc(int badgeId)
{
	//auto&& data = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByDefId(badgeId);
	//if (!data)
	//{
	//	assert(0);
	//	return;
	//}
	//m_totalTm = data->getWatiTime() - Heartbeat::getInstance().getServerTime();
	//if (m_totalTm <= 0)
	//{
	//	return;
	//}
	//auto castMoney = BuildRoomCfg::getInstance().getRebornCastMoney(m_totalTm, data->level);
	//auto player = PlayerMgr::getInstance().getSelf();
	//if (!player)
	//{
	//	assert(0);
	//	return;
	//}
	//if (getRebirthTips())
	//{
	//	if (player->getMoney(MsgPB::ITEM_CURRENCY_TYPE_DIAMOND) < castMoney)
	//	{
	//		UiCommon::getInstance().showCashNoEnoughView();
	//		return;
	//	}
	//	reviveNpcReq(data->uuid);
	//	return;
	//}
	////auto&& itemCfg = ItemCfg::getInstance().getItemById(3);
	//UiMsgBox* pUiMsgBox = UiMsgBox::normalMsgBox(NS::EMPTY_STRING, _LC("CLI_DungeonSys-Revive"), UiMsgBox::BoxStyle::SHOW_TEXT_WARNTEXT_CHECKBOX, true, UiMsgBox::MsgBoxConfirmKey::NpcbirthTipsBox);
	////pUiMsgBox->setMoneyIcon(itemCfg->m_icon);
	//tstring str = fmt::format("<font color='ui.cs_red_011' size='35'>{0}</font>", NS::TimeUtil::getTimeDurationStringMS(m_totalTm));
	////pUiMsgBox->setCastName(fmt::format("{0}\n{1}" , _LC("setCastName"), str));
	//pUiMsgBox->setWarnText("");
	//pUiMsgBox->setText(fmt::format("<font size='35'>{0}\n</font>{1}", _LC("UI_npc_show-Countdown to death"), str));
	//pUiMsgBox->setCheckBoxText(_LC("CLI_HandlerPet-Do not remind again"));
	//pUiMsgBox->setConfirmText(fmt::format("<image id = 'formal02.ps_game_icon_diamond' scale = '0.6, 0.6'/>  {0}\n{1}", castMoney, _LC("UI_npc_show-Rebirth")));
	////pUiMsgBox->setMoneyCount(fmt::format("{0}", castMoney));
	//int lev = data->level;
	//NS::I64 uuid = data->uuid;
	//REG_EVENT(pUiMsgBox)[this, badgeId, lev, uuid](const UiMsgBoxEvent& evt)
	//{
	//	UNREGISTER_TIMER(m_rebirthTimerId);
	//	auto tpy = evt.m_type;
	//	switch (tpy)
	//	{
	//	case UiMsgBoxEvent::EventType::Unknown:
	//		break;
	//	case UiMsgBoxEvent::EventType::Confirm:
	//	{
	//		auto player = PlayerMgr::getInstance().getSelf();
	//		if (player->getMoney(MsgPB::ITEM_CURRENCY_TYPE_DIAMOND) < BuildRoomCfg::getInstance().getRebornCastMoney(m_totalTm, lev))
	//		{
	//			UiCommon::getInstance().showCashNoEnoughView();
	//			return;
	//		}
	//		if (evt.m_bCheckBox)
	//		{
	//			setRebirthTips(true);
	//		}
	//		reviveNpcReq(uuid);
	//	}
	//	break;
	//	case UiMsgBoxEvent::EventType::Cancel:
	//	{
	//	}
	//	break;
	//	default:
	//		break;
	//	}
	//};

	//UNREGISTER_TIMER(m_rebirthTimerId);
	//if (data->status == BADGE_STATUS_DEATH && m_totalTm > 0)
	//{
	//	REGISTER_TIMER(m_rebirthTimerId, FLT_MAX, 1.0f, &HandlerNpc::onRebirthTimer, data->level);
	//}
}

void HandlerNpc::onRebirthTimer(const TimerEvent& evt, int level)
{
	//m_totalTm--;
	//if (m_totalTm <= 0)
	//{
	//	UNREGISTER_TIMER(m_rebirthTimerId);
	//	UiMgr::getInstance().closeDialog<UiMsgBox>();
	//	return;
	//}
	//auto castMoney = BuildRoomCfg::getInstance().getRebornCastMoney(m_totalTm, level);
	//castMoney = castMoney < 0 ? 0 : castMoney;
	//auto&& dia = UiMgr::getInstance().findDialog<UiMsgBox>();
	//if (dia)
	//{
	//	tstring str = fmt::format("<font color='ui.cs_red_011' size='35'>{0}</font>", NS::TimeUtil::getTimeDurationStringMS(m_totalTm));
	//	dia->setText(fmt::format("<font size='35'>{0}\n</font>{1}", _LC("UI_npc_show-Countdown to death"), str));
	//	dia->setConfirmText(fmt::format("<image id = 'formal02.ps_game_icon_diamond' scale = '0.6, 0.6'/>  {0}\n{1}", castMoney, _LC("UI_npc_show-Rebirth")));
	//}
}

