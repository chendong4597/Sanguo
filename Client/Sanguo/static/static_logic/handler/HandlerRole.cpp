/*!
 * \file HandlerRole.cpp
 *
 * \author laizhisheng
 * \date 2016/11/08 15:32
 *
 */
#include "stdafx.h"
#include "HandlerRole.h"
#include <common/Heartbeat.h>
#include <data/role/Player.h>
#include "../common/MsgMgr.h"
#include <msg/MsgType.pb.h>
#include <msg/MsgPlayer.pb.h>
#include <msg/MsgMapServer.pb.h>
#include <msg/MsgMove.pb.h>
#include <data/MapData.h>
#include <msg/MsgMob.pb.h>
#include "HandlerMonster.h"
#include "HandlerLogin.h"
#include <config/BuffCfg.h>
#include "HandlerLogin.h"
#include "data/ItemData.h"
#include "data/NotesData.h"
#include "../GameAppExtern.h"
#include "LogUtil.h"
#include "common/RandomData.h"
#include "../DataHandler.h"
#include "../GameAppExtern.h"
#include "../localserver/LocalServer_Buff.h"



using namespace NS;
using namespace MsgPB;

static HandlerRole* g_HandlerRole = nullptr;

HandlerRole& HandlerRole::getInstance()
{
    assert(g_HandlerRole);
    return *g_HandlerRole;
}

bool HandlerRole::isInitialized()
{
    return (g_HandlerRole != nullptr);
}

HandlerRole::HandlerRole()
{
    assert(!g_HandlerRole);
    g_HandlerRole = this;
    reset();
}

HandlerRole::~HandlerRole()
{
    g_HandlerRole = nullptr;
}

bool HandlerRole::initialize()
{
	REGISTER_S2C(onMsgPlayerInfo);
	REGISTER_S2C(onMsgPlayerNormalAttackRes);
	REGISTER_S2C(onMsgSkillEffectDotDmgMS2C);
	REGISTER_S2C(onMsgMapObjectUnderAttackG2C);
	REGISTER_S2C(onMsgSkillEffectDmgMS2C);
	REGISTER_S2C(onMsgCharacterDeadS2C);
	REGISTER_S2C(onMsgCharacterReviveS2C);
	REGISTER_S2C(onMsgBuffInfo_S2C);
	REGISTER_S2C(onMsgPlayerSysInfoGS2C);
	REGISTER_S2C(onMsgPlayerRenameFlagS2C);
	REGISTER_S2C(onMsgPlayerDailyResetS2C);
	REGISTER_S2C(onMsgUserBuffInfo_S2C);
	REGISTER_S2C(onMsgPlayerKickOutS2C);
	REGISTER_S2C(onMsgSkillEffectRestoreMS2C);
	

	//改名消息
	REGISTER_S2C(onMsgPlayerRenameS2C);
	//同步货币消息;
	//REGISTER_S2C(onMsgItemCurrencySyncS2C);

	//other players
	REGISTER_S2C(onMsgPlayerAppearance);
	//REGISTER_S2C(onMsgPlayerOnHandItemS2C); // 同步武器和工具
	REGISTER_S2C(onMsgOtherPlayerChangeEquipment); // 同步第三端玩家切换装备

	//self player
	REGISTER_S2C(onMsgPlayerDataSyncDoneG2C);
	REGISTER_S2C(onMsgPlayerDataContainerSyncDoneG2C);
	//REGISTER_S2C(onMsgItemPlayerEquipNotifyS2C);

	//击退
	REGISTER_S2C(onMsgCharacterBeBeatBackS2C);

	//激励视频
	REGISTER_S2C(onMsgPlayerADNVideoAwardS2C);

	REGISTER_S2C(onMsgLotteryS2C);
	return true;
}

void HandlerRole::terminate()
{
	UNREGISTER_TIMER(m_TimeId);
	__timer_host__.clearTimer();

	reset();
}

void HandlerRole::onMsgPlayerDailyResetS2C(const MsgPB::MsgPlayerDailyResetS2C& msg)
{
	NS::I64 nServerTimeNextDaily = Heartbeat::getInstance().getServerTimeNextDaily();
	nServerTimeNextDaily += 24 * 60 * 60;
	Heartbeat::getInstance().setServerTimeNextDaily(nServerTimeNextDaily);
}

bool HandlerRole::reset()
{
	return true;
}


MapObj* HandlerRole::findRole(bzbee::uint mapObjId)
{
	auto&& obj = MapObjMgr::getInstance().findObj(mapObjId);
	if (!obj) { return nullptr; }

	return obj.get();

	//MapObj* gameObj{};
	//switch (obj->getCategory())
	//{
	//case MapObj::Category::Player:
	//{
	//	//gameObj = PlayerComponentMgr::getInstance().findPlayer(mapObjId);
	//}
	//break;

	//case MapObj::Category::Monster:
	//{
	//	//gameObj = MonsterComponentMgr::getInstance().findMonster(mapObjId);
	//}
	//break;

	//case MapObj::Category::Pet:
	//{
	//	//gameObj = PetComponentMgr::getInstance().findPet(mapObjId);
	//}
	//break;
	//case MapObj::Category::Npc:
	//{
	//	//gameObj = NpcComponentMgr::getInstance().findNpc(mapObjId);
	//}
	//break;

	//case MapObj::Category::TowerBuilding:
	//case MapObj::Category::InteractiveBuilding:
	//{
	//	//auto&& pBuildingObj = std::dynamic_pointer_cast<Building>(obj);
	//	//if (pBuildingObj)
	//	//{
	//	//	gameObj = BuildingMgr::getInstance().getBuilding(pBuildingObj->getCoordId());
	//	//}
	//}
	//break;

	//case MapObj::Category::Vehicle:
	//{
	//	//gameObj = VehicleComponentMgr::getInstance().findVehicle(mapObjId);
	//}
	//break;
	//default:
	//	break;
	//}

	//return gameObj;
}

void HandlerRole::onMsgPlayerRenameFlagS2C(const MsgPB::MsgPlayerRenameFlagS2C& msg)
{
	m_haveChangeName = msg.first_free_rename();
}


//改名请求
bool HandlerRole::changeNameReq(const bzbee::tstring name, bool isFirst)
{
	MsgPlayerRenameC2S msg;
	msg.set_msgtype(_MSG_PLAYER_RENAME_C2S);
	msg.set_iggid(PlayerMgr::getInstance().getSelf()->getIggId());
	msg.set_name(name);
	msg.set_firstfree(isFirst);
	return MsgMgr::getInstance().sendMessageDirect(_MSG_PLAYER_RENAME_C2S, &msg);
}

//改名结果;
void HandlerRole::onMsgPlayerRenameS2C(const MsgPB::MsgPlayerRenameS2C& msg)
{
	switch (msg.res())
	{
	case MSG_PLAYER_RESULT::MSG_PLAYER_RESULT_OK:
	{
		break;
	}
	case MSG_PLAYER_RESULT::MSG_PLAYER_RESULT_ERR:
	{
		break;
	}
	case MSG_PLAYER_RESULT::MSG_PLAYER_RESULT_REPEATED:
	{
		break;
	}
	case MSG_PLAYER_RESULT::MSG_PLAYER_RESULT_ILLEGAL:
	{
		break;
	}
	case MSG_PLAYER_RESULT::MSG_PLAYER_RESULT_NOT_ENOUGH:
	{
		break;
	}
	default:
		break;
	}
}

//同步货币;
//void HandlerRole::onMsgItemCurrencySyncS2C(const MsgPB::MsgItemCurrencySyncS2C& msg)
//{
//	auto player = PlayerMgr::getInstance().getSelf();
//	if (player == nullptr)
//	{
//		player = std::make_shared<Player>(0);
//	}
//	for (int i = 0; i < msg.data_size(); ++i)
//	{
//		auto info = msg.data(i);
//		auto cfg = ItemCfg::getInstance().getItemById((int)info.uuid());
//		int nCount = 0;
//		if (cfg && cfg->m_subType == MsgPB::ITEM_SUBTYPE_CURRENCY)
//		{
//			nCount = player->getMoney(cfg->m_itemId);
//			player->setMoney(cfg->m_itemId, info.value());
//		}
//	}
//}

void HandlerRole::onMsgPlayerInfo(const MsgPB::MsgPlayerInfoGS2C& msg)
{
	bool attrChged{ true };
	bool isOtherPlayer = false;
	auto player = PlayerMgr::getInstance().getSelf();
	if (player != nullptr) {
		auto  myIggId = player->getIggId();
		if (myIggId != msg.iggid() && msg.iggid() > 0) {
			isOtherPlayer = true;
		}
	}

	if (!isOtherPlayer) {
		if (player == nullptr) {
			player = std::make_shared<Player>(0);
			HandlerRoleEvent event;
			event.m_type = HandlerRoleEvent::EventType::createRole;
			POST_DATA_EVENT(event);
			PlayerMgr::getInstance().setSelf(player);
		}
	} else {
		player = PlayerMgr::getInstance().getPlayerByIggId(msg.iggid());
		if (player == nullptr)
			return;
	}

	std::string strTips;

	if (msg.has_iggid())
	{
		player->setIggId(msg.iggid());
	}
	if (msg.has_pvpsegment())
	{
		player->setPvpSegment(msg.pvpsegment());
	}
	if (msg.has_pvpscore())
	{
		player->setPvpScore(msg.pvpscore());
	}
	if (msg.has_pvpkillscore())
	{
		player->setPvpKillScore(msg.pvpkillscore());
	}
	if (msg.has_pvprankscore())
	{
		player->setPvpRankScore(msg.pvprankscore());
	}
	if (msg.has_pvptime())
	{
		player->setPvpTime(msg.pvptime());
	}
	if (msg.has_pvpcoin())
	{
		player->setPvpCoin(msg.pvpcoin());
	}

	
	if (msg.has_exp())
	{
		player->setExp(msg.exp());
	}
	if (msg.has_lv() && msg.lv() != player->getLevel())
	{
		int nPreLv = player->getLevel();
		player->setLevel(msg.lv());
		if (nPreLv != 0)
		{
			HandlerRoleEvent evtRole;
			evtRole.m_type = HandlerRoleEvent::EventType::PlayerLvUp;
			POST_DATA_EVENT(evtRole);
		}
	}

	

	if (msg.has_food())
	{
		player->setFood(msg.food());
	}
	if (msg.has_breath())
	{
		player->setBreath(msg.breath());
	}

	if (msg.has_hp() && msg.has_maxhp())
	{
		player->SetCacheHp(msg.hp());
		player->SetCacheMaxHp(msg.maxhp());
		player->setHp(msg.hp());
		player->setMaxHp(msg.maxhp());
	}
	else if (msg.has_hp())
	{
		int nPreHp = player->getHp();
		player->SetCacheHp(msg.hp());
		player->setHp(msg.hp());
		if (!isOtherPlayer && player->isInitialized())
		{
			if (nPreHp < msg.hp()) {
				//HandlerHeroAttrChangeEvent evt;
				//evt.m_type = HandlerHeroAttrChangeEvent::EventType::Event_HP_ADD;
				//evt.nValue = msg.hp() - nPreHp;
				//POST_DATA_EVENT(evt);
			}
			HandlerHeroAttrChangeEvent evt;
			evt.m_type = HandlerHeroAttrChangeEvent::EventType::Event_HP_VALUE;
			POST_DATA_EVENT(evt);
		}
	}
	else if (msg.has_maxhp())
	{
		player->SetCacheMaxHp(msg.maxhp());
		player->setMaxHp(msg.maxhp());
	}

	if (msg.has_mp())
	{
		player->setMp(msg.mp());
	}

	if (msg.has_nickname())
	{
		player->setNickname(msg.nickname());
	}
	if (msg.has_group())
	{
		player->setGroupId(msg.group());
	}

	auto& tData = player->getPlayerModelData();
	if (msg.has_face())
	{
		tData.m_face = msg.face();
	}
	if (msg.has_gender())
	{
		int nGender = tData.m_gender;
		tData.m_gender = msg.gender();
		if (nGender == -1 && msg.gender() != -1)
		{
			HandlerLogin::getInstance().enterMap();
		}
	}
	if (msg.has_skincolor())
	{
		tData.m_skincolor = msg.skincolor();
	}
	if (msg.has_hair())
	{
		tData.m_hair = msg.hair();
	}
	if (msg.has_haircolor())
	{
		tData.m_haircolor = msg.haircolor();
	}
	if (msg.has_facecolor())
	{
		tData.m_facecolor = msg.facecolor();
	}
	if (msg.has_underwear())
	{
		tData.m_underwear = msg.underwear();
	}
	if (msg.has_underwearcolor())
	{
		tData.m_underwearcolor = msg.underwearcolor();
	}
	if (msg.has_ownmapnum())
	{
		tData.m_ownmapnum = msg.ownmapnum();
	}
	if (msg.has_coin())
	{
		int nPreNum = player->getMoney(COIN_ITEM_ID);
		player->setMoney(COIN_ITEM_ID, msg.coin());
		if (nPreNum < msg.coin() && !isOtherPlayer) {
			std::string str = fmt::format("{0} X {1}", _LC("UI_COMMON_PLAYER_GET_COIN") , (int)(msg.coin() - nPreNum));
			GameAppExtern::ShowToastTips(str, static_cast<int>(HandlerTipsEvent::EventType::Evt_Coin));
		}
	}
	if (msg.has_tickets())
	{
		player->setMoney(TICKET_ITEM_ID, msg.tickets());
	}
	if (msg.has_cash())
	{
		int nPreNum = player->getMoney(CASH_ITEM_ID);
		player->setMoney(CASH_ITEM_ID, msg.cash());
		if (nPreNum < msg.cash() && !isOtherPlayer) {
			std::string str = fmt::format("{0} X {1}", _LC("UI_COMMON_PLAYER_GET_CASH") , (int)(msg.cash() - nPreNum));
			GameAppExtern::ShowToastTips(str, static_cast<int>(HandlerTipsEvent::EventType::Evt_Cash));
		}
	}

	if (msg.has_curdungeonid())
	{
		player->setCurInsMapId(msg.curdungeonid());
	}

	if (msg.has_lottytime())
	{
		player->setLottyTime(msg.lottytime());
	}

	POST_DATA_EVENT(msg);
	player->onInit();
}

void HandlerRole::onMsgSkillEffectRestoreMS2C(const MsgSkillEffectRestoreMS2C& msg)
{
	const std::shared_ptr<Player> pPlayer = PlayerMgr::getInstance().getSelf();
	if (!pPlayer || msg.target_mapobjectid() != pPlayer->getId() ||
		msg.currhp() == 0) return;
	GameAppExtern::requestSynBase(MsgPlayerSynBaseC2S_MSG_SYN_BASE_TPY_MSG_SYN_BASE_TPY_HP, true, msg.currhp(),
		MsgPlayerSynBaseC2S_MSG_SYN_BASE_TPY_REASON_MSG_SYN_BASE_TPY_REASON_SKILL);
	POST_DATA_EVENT(msg);
}

void HandlerRole::onMsgBuffInfo_S2C(const MsgPB::MsgBuffInfo_S2C& msg)
{

	for (int i{}; i < msg.buff_infos_size(); ++i)
	{
		auto&& buffInfo = msg.buff_infos(i);

		auto&& objId = buffInfo.owner_mapobjectid();
		auto&& obj = findRole(objId);
		if (!obj)
		{
			LOGE("Cannot find role: {0}.", objId);
			continue;
		}

		Role* role = dynamic_cast<Role*>(obj);
		if (!role) {
			return;
		}

		int nPreStatus = role->getCtrlState();

		switch (static_cast<RoleBuffEvent::EventType>(msg.buff_event()))
		{
		case RoleBuffEvent::EventType::AddBuff:	// add
		{
			auto&& buffType = BuffCfg::getInstance().findBuffType(buffInfo.buff_dataid());
			if (!buffType)
			{
				LOGE("Invalid buff type: {0}.", buffInfo.buff_dataid());
				break;
			}

			Buff buff{ buffType };
			buff.setIdx(buffInfo.index());
			buff.setAliveTime(buffInfo.alive_timer());
			buff.setHeapCounter(buffInfo.heap_counter());
			if (buffInfo.has_caster_mapobjectid())
			{
				buff.setCasterId(buffInfo.caster_mapobjectid());
			}
			if (buffInfo.effect_idx_unactive_size() > 0)
			{
				std::vector<int> vecUnActivePosIndex{};
				for (int nUnActivePosIndex : buffInfo.effect_idx_unactive())
				{
					vecUnActivePosIndex.push_back(nUnActivePosIndex);
				}
				buff.setUnActiveEffectIndex(vecUnActivePosIndex);
			}

			role->addBuff(buff);
			if (buffType->n_BuffEffID > 0) {
				HandlerRoleBuffEvent evt;
				evt.m_type = HandlerRoleBuffEvent::EventType::EventType_Buff_Add;
				evt.buffId = buffType->n_BuffEffID;
				evt.objId = role->getId();
				POST_DATA_EVENT(evt);
			}
		}
		break;

		case RoleBuffEvent::EventType::UpdateBuff:	// update
		{
			auto&& buff = role->findBuffByIdx(buffInfo.index());

			bool removed{};
			if (buff && buff->getType() && buff->getType()->n_ID != buffInfo.buff_dataid())
			{
				role->removeBuff(buffInfo.index());
				int nEffId = buff->getType()->n_BuffEffID;
				if (!role->IsHasBuffEffectId(nEffId)) {
					HandlerRoleBuffEvent evt;
					evt.m_type = HandlerRoleBuffEvent::EventType::EventType_Buff_Del;
					evt.buffId = nEffId;
					evt.objId = role->getId();
					POST_DATA_EVENT(evt);
				}
				removed = true;
			}

			if (!buff || removed)
			{
				auto&& buffType = BuffCfg::getInstance().findBuffType(buffInfo.buff_dataid());
				if (!buffType)
				{
					LOGE("Invalid buff type: {0}.", buffInfo.buff_dataid());
					break;
				}
				Buff buff{ buffType };
				buff.setIdx(buffInfo.index());
				buff.setAliveTime(buffInfo.alive_timer());
				buff.setHeapCounter(buffInfo.heap_counter());
				if (buffInfo.has_caster_mapobjectid())
				{
					buff.setCasterId(buffInfo.caster_mapobjectid());
				}
				role->addBuff(buff);

				if (buffType->n_BuffEffID > 0) {
					HandlerRoleBuffEvent evt;
					evt.m_type = HandlerRoleBuffEvent::EventType::EventType_Buff_Add;
					evt.buffId = buffType->n_BuffEffID;
					evt.objId = role->getId();
					POST_DATA_EVENT(evt);
				}
			}
		}
		break;

		case RoleBuffEvent::EventType::RemoveBuff:	// remove
		{
			auto&& buff = role->findBuffByIdx(buffInfo.index());
			int nEffId = 0;
			if (buff && buff->getType()) {
				nEffId = buff->getType()->n_BuffEffID;
			}
			role->removeBuff(buffInfo.index());
			
			if (!role->IsHasBuffEffectId(nEffId)) {
				HandlerRoleBuffEvent evt;
				evt.m_type = HandlerRoleBuffEvent::EventType::EventType_Buff_Del;
				evt.buffId = nEffId;
				evt.objId = role->getId();
				POST_DATA_EVENT(evt);
			}
		}
		break;

		case RoleBuffEvent::EventType::LockBuff: //lock
		{
			//role->lockBuff(buffInfo.index());
		}
		break;

		case RoleBuffEvent::EventType::UnLockBuff: //unlock
		{
			
		}
		break;
		default:
			break;
		}
		role->calcAttr();
		int nCurStatus = role->getCtrlState();
		if (nCurStatus != nPreStatus) {
			HandlerRoleStatusEvent evt;
			evt.objId = role->getId();
			POST_DATA_EVENT(evt);
		}
	}
}

void HandlerRole::onMsgUserBuffInfo_S2C(const MsgPB::MsgUserBuffDataS2C& msg)
{
	const std::shared_ptr<Player> pPlayer = PlayerMgr::getInstance().getSelf();
	if (!pPlayer) return;

	for (int i = 0; i < msg.buffs_size(); ++i)
	{
		MsgPB::MsgUserBuffData::Buff buff = msg.buffs(i);
		//LocalServer_Buff::getInstance().saveRestoreBuff(buff);
	}
}


/**********************************************************************************************************************************/

// others attack others
void HandlerRole::onMsgMapObjectUnderAttackG2C(const MsgPB::MsgMapObjectUnderAttackG2C& msg)
{
	//auto&& srcMapObjId = msg.sourceobjid();
	//auto&& srcRole = findRole(srcMapObjId);
	//if (!srcRole)
	//{
	//	LOGE("src obj not found, mapObjId: {0}.", srcMapObjId);
	//	return;
	//}
	POST_DATA_EVENT(msg);
}

void HandlerRole::onMsgSkillEffectDotDmgMS2C(const MsgPB::MsgSkillEffectDotDmgMS2C& msg)
{
	POST_DATA_EVENT(msg);
}

// self attack others
void HandlerRole::onMsgPlayerNormalAttackRes(const MsgPB::MsgPlayerNormalAttackResS2C& msg)
{
	if (msg.res() != MSG_PLAYER_RESULT_OK)
	{
		LOGE("normal attack result: {0}", msg.res());
		return;
	}
	POST_DATA_EVENT(msg);
}

void HandlerRole::onMsgSkillEffectDmgMS2C(const MsgPB::MsgSkillEffectDmgMS2C& msg)
{
	POST_DATA_EVENT(msg);
}

// 击退消息
void HandlerRole::onMsgCharacterBeBeatBackS2C(const MsgPB::MsgCharacterBeBeatBackS2C& msg)
{
	int32 caster_id = msg.cast_id();
}


bool HandlerRole::requestPlayerOnHandItemC2S(MsgPB::EQUIP_PACKET_POS equipPos)
{
	//MsgPB::MsgPlayerOnHandItemC2S msg;
	//msg.set_onhand_item(equipPos);

	//return MsgMgr::getInstance().sendMessage(_MSG_PLAYER_ONHAND_ITEM_C2S, &msg);
	return true;
}

void HandlerRole::onMsgCharacterDeadS2C(const MsgPB::MsgCharacterDeadS2C& msg)
{
	LocalServer_Buff::getInstance().delAllBuff(msg.mapobjectid(), true);

	POST_DATA_EVENT(msg);
}

void HandlerRole::onMsgCharacterReviveS2C(const MsgPB::MsgCharacterReviveS2C& msg)
{
}

const std::shared_ptr<Role> HandlerRole::addOtherPlayer(int mapObjId, const glm::vec3 & pos)
{
	//auto&& otherPlayer = std::make_shared<Player>(mapObjId);
	//otherPlayer->setPos(pos);
	//otherPlayer->setLevel(1);
	//return PlayerComponentMgr::getInstance().addPlayer(otherPlayer) ? otherPlayer : nullptr;
	return nullptr;
}

void HandlerRole::onMsgPlayerAppearance(const MsgPB::MsgPlayerAppearance& msg)
{
	if (PlayerMgr::getInstance().isMyself(msg.iggid())) {
		std::shared_ptr<Player> player = PlayerMgr::getInstance().getSelf();
		if (player)
		{
			player->setId(msg.mapobjectid());
			player->setHp(msg.hp());
			PlayerMgr::getInstance().setSelf(player);
		}
		HandlerHeroAppearanceEvent evt;
		POST_DATA_EVENT(evt);
		return;
	}
		

	bool bFullHpMp = false;
	if (msg.has_full_hp_mp() && msg.full_hp_mp())
	{
		bFullHpMp = true;
	}
	auto otherPlayer = std::make_shared<Player>(1);
	otherPlayer->setIggId(msg.iggid());
	otherPlayer->setId(msg.mapobjectid());
	otherPlayer->setLevel(msg.lv());
	otherPlayer->setPrivateLev(msg.lv());
	//otherPlayer->setPos(MapData::serverWorldPos2ClientWorldPos(msg.posx(), msg.posy()));
	otherPlayer->setHp(msg.hp());
	otherPlayer->setMp(msg.mp());
	otherPlayer->setGroupId(msg.group());
	otherPlayer->setNickname(msg.nickname());
	//otherPlayer->setMaxHp(msg.maxhp());
	otherPlayer->getPlayerModelData().m_gender = msg.gender();
	auto& tData = otherPlayer->getPlayerModelData();
	if (msg.has_skincolor()) tData.m_skincolor = msg.skincolor();
	if (msg.has_hair     ()) tData.m_hair      = msg.hair     ();
	if (msg.has_haircolor()) tData.m_haircolor = msg.haircolor();

	

	for (int i{}; i < msg.buff_infos_size(); ++i)
	{
		auto&& buffInfo = msg.buff_infos(i);
		auto&& buffType = BuffCfg::getInstance().findBuffType(buffInfo.buff_dataid());
		if (!buffType)
		{
			LOGE("Invalid buff type: {0}.", buffInfo.buff_dataid());
			continue;
		}

		Buff buff{ buffType };
		buff.setIdx(buffInfo.index());
		buff.setAliveTime(buffInfo.alive_timer());
		buff.setHeapCounter(buffInfo.heap_counter());
		//playerComp->addBuff(buff);
	}

	// 其他角色装备信息
	Role::EquipMap equipMap{};
	for (int i{}; i < msg.equip_infos_size(); ++i)
	{
		auto&& equipInfo = msg.equip_infos(i);
		std::shared_ptr<ItemData> pItemData = std::make_shared<ItemData>();
		pItemData->setUuid(equipInfo.uuid());
		pItemData->initWithItemId(equipInfo.dataid());
		pItemData->setPos(equipInfo.pos());
		if (equipInfo.has_hide())
		{
			//pItemData->setHide(equipInfo.hide());
		}
		if (equipInfo.has_durable())
		{
			pItemData->setDurable(equipInfo.durable());
		}

		//装备附加属性
		//for (int i = 0; i < equipInfo.effects_size(); i++)
		//{
		//	pItemData->addAttrEffect(AttrType::ATTR_EFFECT, equipInfo.effects(i));

		//	if (i < equipInfo.effects_color_size())
		//	{
		//		pItemData->addtionalAttrColor(equipInfo.effects(i).type(), equipInfo.effects_color(i));
		//	}
		//	for (int j = 0; j < equipInfo.effects(i).values_size(); ++j)
		//	{
		//		pItemData->addtionalAttrColorMap(equipInfo.effects(i).type(), equipInfo.effects_color(i), equipInfo.effects(i).values(j).val() * 1.f);
		//	}
		//}
		//for (int i = 0; i < equipInfo.baseval_size(); i++)
		//{
		//	pItemData->addAttrEffect(AttrType::ATTR_BASE, equipInfo.baseval(i));
		//}
		//if (equipInfo.effects_size() > 0)
		//{
		//	pItemData->setIsCanShing(true);
		//}
		//else
		//{
		//	pItemData->setIsCanShing(false);
		//}
		//for (int i = 0; i < equipInfo.staticval_size(); i++)
		//{
		//	pItemData->addAttrEffect(AttrType::ATTR_STATICVAL, equipInfo.staticval(i));
		//}
		equipMap.insert(make_pair(equipInfo.pos(), pItemData));
	}
	otherPlayer->onInit();
}

void HandlerRole::onMsgItemPlayerEquipNotifyS2C(const MsgPB::MsgItemPlayerEquipNotifyS2C& msg)
{
	// 废弃
	// 服务器反馈与MsgPB::MsgItemOtherPlayerChangeEquip_S2C是相同的作用，MsgPB::MsgItemOtherPlayerChangeEquip_S2C服务器上用的更多。
	// 客户端改为只处理MsgPB::MsgItemOtherPlayerChangeEquip_S2C消息。

	//if (PlayerMgr::getInstance().isMyself(msg.iggid()))
	//	return;
	//if (PublicMapPlayerMgr::getInstance().isShadowPlayer(msg.mapobjectid()))
	//	return;
	//
	//auto&& gameObjPlayer = PlayerComponentMgr::getInstance().findPlayer(msg.mapobjectid());
	//if (!gameObjPlayer) { return; }
	//auto&& playerComp = gameObjPlayer->findComponent<PlayerComponent>();
	//std::shared_ptr<ItemData> pItemData = std::make_shared<ItemData>();
	//auto&& equipInfo = msg.equip_info();
	//pItemData->setUuid(equipInfo.uuid());
	//pItemData->initWithItemId(equipInfo.dataid());
	//pItemData->setPos(equipInfo.pos());
	//if (equipInfo.has_hide())
	//{
	//	pItemData->setHide(equipInfo.hide());
	//}
	//if (equipInfo.has_durable())
	//{
	//	pItemData->setDurable(equipInfo.durable());
	//}

	////装备附加属性
	//for (int i = 0; i < equipInfo.effects_size(); i++)
	//{
	//	pItemData->addAttrEffect(AttrType::ATTR_EFFECT, equipInfo.effects(i));

	//	if (i < equipInfo.effects_color_size())
	//	{
	//		pItemData->addtionalAttrColor(equipInfo.effects(i).type(), equipInfo.effects_color(i));
	//	}
	//	for (int j = 0; j < equipInfo.effects(i).values_size(); ++j)
	//	{
	//		pItemData->addtionalAttrColorMap(equipInfo.effects(i).type(), equipInfo.effects_color(i), equipInfo.effects(i).values(j).val() * 1.f);
	//	}
	//}
	//for (int i = 0; i < equipInfo.baseval_size(); i++)
	//{
	//	pItemData->addAttrEffect(AttrType::ATTR_BASE, equipInfo.baseval(i));
	//}
	//if (equipInfo.effects_size() > 0)
	//{
	//	pItemData->setIsCanShing(true);
	//}
	//else
	//{
	//	pItemData->setIsCanShing(false);
	//}
	//for (int i = 0; i < equipInfo.staticval_size(); i++)
	//{
	//	pItemData->addAttrEffect(AttrType::ATTR_STATICVAL, equipInfo.staticval(i));
	//}

	//if (equipInfo.pos() == MsgPB::EQUIP_PACKET_POS::EQUIP_PACKET_WEAPON || equipInfo.pos() == MsgPB::EQUIP_PACKET_POS::EQUIP_PACKET_FARMTOOL)
	//{
	//	playerComp->setCurrShortCutSelected(equipInfo.pos());
	//}
	//playerComp->switchEquip(equipInfo.pos(), pItemData);
}

//void HandlerRole::onMsgPlayerOnHandItemS2C(const MsgPB::MsgPlayerOnHandItemS2C& msg)
//{
//}

// 同步第三端玩家切换装备
void HandlerRole::onMsgOtherPlayerChangeEquipment(const MsgPB::MsgItemOtherPlayerChangeEquip_S2C& msg)
{
	if (!msg.has_itemdata())
	{
		return;
	}
	// 玩家自己，忽略
	if (PlayerMgr::getInstance().isMyself(msg.iggid()))
	{
		return;
	}

	// 装备基础信息
	const MsgPB::MsgItemCommonData& equipmentData = msg.itemdata();

	if (msg.take_off())
	{
		// 装备卸载
		NS::I64 itemUuid = equipmentData.uuid();
		//pPlayerComponent->removeEquip(itemUuid);
	}
	else if (equipmentData.packettype() == MsgPB::PACKET_TYPE::EQUIPMENT)
	{
		// 装备穿戴
		std::shared_ptr<ItemData> pEquipmentData = std::make_shared<ItemData>();
		pEquipmentData->setUuid(equipmentData.uuid());
		pEquipmentData->initWithItemId(equipmentData.dataid());
		if (equipmentData.has_pos())
		{
			pEquipmentData->setPos(equipmentData.pos());
		}
		// 装备是否隐藏(时装)
		if (equipmentData.has_hide())
		{
			//pEquipmentData->setHide(equipmentData.hide());
		}
		// 装备当前耐久
		if (equipmentData.has_durable())
		{
			pEquipmentData->setDurable(equipmentData.durable());
		}
		// 装备最大耐久，破损修理会使耐久降低
		if (equipmentData.has_durablemax())
		{
			pEquipmentData->setDurable(equipmentData.has_durablemax());
		}
		//// 装备随机技能
		//if (equipmentData.has_extskill())
		//{
		//	pEquipmentData->m_vAllSkill.push_back(equipmentData.extskill());
		//}
		//// 装备基础属性
		//for (int i = 0; i < equipmentData.baseval_size(); i++)
		//{
		//	pEquipmentData->addAttrEffect(AttrType::ATTR_BASE, equipmentData.baseval(i));
		//}
		//// 装备固定属性
		//for (int i = 0; i < equipmentData.staticval_size(); i++)
		//{
		//	pEquipmentData->addAttrEffect(AttrType::ATTR_STATICVAL, equipmentData.staticval(i));
		//}
		//// 装备附加属性
		//for (int i = 0; i < equipmentData.effects_size(); i++)
		//{
		//	pEquipmentData->addAttrEffect(AttrType::ATTR_EFFECT, equipmentData.effects(i));

		//	if (i < equipmentData.effects_color_size())
		//	{
		//		pEquipmentData->addtionalAttrColor(equipmentData.effects(i).type(), equipmentData.effects_color(i));
		//	}
		//	for (int j = 0; j < equipmentData.effects(i).values_size(); ++j)
		//	{
		//		pEquipmentData->addtionalAttrColorMap(equipmentData.effects(i).type(), equipmentData.effects_color(i), equipmentData.effects(i).values(j).val() * 1.f);
		//	}
		//}
		//// 装备是否可洗练
		//equipmentData.effects_size() > 0 ? pEquipmentData->setIsCanShing(true) : pEquipmentData->setIsCanShing(false);
		//// 记录当前玩家手持装备类型
		//if (equipmentData.pos() == MsgPB::EQUIP_PACKET_POS::EQUIP_PACKET_WEAPON || equipmentData.pos() == MsgPB::EQUIP_PACKET_POS::EQUIP_PACKET_FARMTOOL)
		//{
		//	pPlayerComponent->setCurrShortCutSelected(equipmentData.pos());
		//}

		//// 更新客户端玩家装备
		//pPlayerComponent->switchEquip(pEquipmentData->getEquipPos(), pEquipmentData);
	}

}

void HandlerRole::onMsgPlayerDataSyncDoneG2C(const MsgPB::MsgPlayerDataSyncDoneG2C& msg)
{
	// 20180614 更改到 MsgPlayerDataContainerSyncDoneG2C
	//auto&& player = PlayerMgr::getInstance().getSelf();
	//if (player)
	//{
	//	player->onInit();
	//}
}

void HandlerRole::onMsgPlayerDataContainerSyncDoneG2C(const MsgPB::MsgPlayerDataContainerSyncDoneG2C& msg)
{
	if (msg.has_next_zero_time())
	{
		Heartbeat::getInstance().setServerTimeNextDaily(msg.next_zero_time());
	}
	auto&& player = PlayerMgr::getInstance().getSelf();
	if (player)
	{
		player->onInit();
	}
}

void HandlerRole::onMsgPlayerSysInfoGS2C(const MsgPB::MsgPlayerSysInfoGS2C& msg)
{
	const MsgPB::MsgPlayerSysBaseData& msgData = msg.playersys();
}

void HandlerRole::onMsgPlayerKickOutS2C(const MsgPB::MsgPlayerKickOutS2C& msg)
{
}

void HandlerRole::onMsgPlayerADNVideoAwardS2C(const MsgPB::MsgPlayerADNVideoAwardS2C& msg)
{
	if (msg.nexttime() > 0)
	{
		auto&& player = PlayerMgr::getInstance().getSelf();
		if (player)
		{
			player->SetADNVideoNextTime(msg.nexttime());
		}
	}
	POST_DATA_EVENT(msg);
}

void HandlerRole::onTimeEvent(TimerEvent& evt)
{
	UNREGISTER_TIMER(m_timeId);
	if (m_haveSend)
		return;
	m_haveSend = true;
}

void HandlerRole::onMsgLotteryS2C(const MsgPB::MsgLotteryS2C& msg)
{
	if (msg.result() != MsgLotteryS2C_RESULT_SUCCESS)
	{
		return;
	}
	NotesDataMgr::getInstance().addItem(NOTES_DATA_TPY_LOTTERY, 0, 0, msg.itemid(), msg.itemcount());
}
