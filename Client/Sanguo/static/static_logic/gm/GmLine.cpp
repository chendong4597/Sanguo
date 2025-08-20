/*!
 * \file GmLine.cpp
 *
 * \author ChenBK;
 * \date 2017/06/16
 *
 *
 */
#include "stdafx.h"
#include <fstream>
#include <string>
#include <lua/ScriptVM.h>
#include "GmLine.h"
#include "Cache.h"
#include "StringUtil.h"
#include "../handler/HandlerItem.h"
//#include "msg/MsgMapServer.pb.h"
//#include "msg/MsgPlayer.pb.h"
//#include "msg/MsgPing.pb.h"
//#include "msg/MsgItemCurrency.pb.h"
//#include "player/PlayerComponentMgr.h"
//#include "msg/MsgPlayer.pb.h"
//#include "config/PlayerAttrCfg.h"
//#include "client/GameWorld.h"
//#include "map/MapComponent.h"
//#include "map/MapRenderComponent.h"
//#include "map/AirBrickRenderComponent.h"
//#include "SceneModule.h"
//#include "common/TransformComponent.h"
//#include "GameObjectModule.h"
//#include "Framework.h"
//#include "player/PlayerComponent.h"
#include "GmCfg.h"
//#include "ui/item/UiEnchantment.h"
//#include "ui/UiMgr.h"
//#include "ui/bag/UiStorageBox.h"
//#include "ui/lobby/UiLobby.h"
//#include "data/DayNightData.h"
//#include "map/daynight/DayNightComponent.h"
//#include "ui/tower/UiTowerDefenseNews.h"
//#include "ui/achievements/UiAchievements.h"
//#include "ui/tradingCenter/UiSysBuyShop.h"
//#include "ui/tradingCenter/UiSysSellShop.h"
//#include "ui/financial/UiFinancialSystem.h"
//#include "ui/guide/UiGuideHandbook.h"
//#include "ui/common/UiChestCollection.h"
//#include "data/ItemData.h"
//#include "data/TowerChallengeData.h"
//#include "ui/mail/UiMail.h"
//#include "sound/AudioModule.h"
//#include "ui/dungeon/UiDungeonBossNews.h"
//#include "ui/blacksmith/UiBlacksmithShop.h"
//#include "data/ChatData.h"
//#include "handler/HandlerChat.h"
//#include "ui/chat/UiChatSystemFull.h"
//#include "ui/login/UiImportant_reminder.h"
//#include "UiRenderCache.h"
//#include <NetworkModule.h>
//#include "ui/item/UiPoloShing.h"
//#include <handler/HandlerMonster.h>
//#include <handler/HandlerPet.h>
//#include <handler/HandlerRole.h>
//#include <camera/TargetCameraComponent.h>
//#include "ui/npc/UiMsgBoxNpcDungeon.h"
//#include "ui/dungeon/UiEndlessModeBet.h"
//#include "localserver/LocalServer_ItemPacket.h"
//#include "localserver/LocalServer_Battle.h"
//#include "localserver/LocalServer_BirthMob.h"
//#include "localserver/LocalServer_Achieve.h"
//#include "localserver/LocalServer_Buff.h"
//#include "localserver/LocalServer_Survival.h"
//#include "cli_build/BuildLogicCli.h"
//#include <data/MapData.h>
//#include "msg/MsgTowerPVP.pb.h"
//#include "cli_build/DropProcessCli.h"
//#include "handler/HandlerMatchEnemy.h"
//#include "map/airship/ShipComponent.h"
//#include "npc/NpcLogic.h"
//#include "../../../../../common/NpcDak/NpcDakDefine.h"
//#include "ai/RootAgent.h"
//#include "localserver/LocalServer_NpcDak.h"
//#include "ui/craft/UiCraftDialog.h"
//#include "ui/shoppingMall/GetMallItemEvent.h"
//#include "ui/shoppingMall/EventMediator.h"
//#include "ui/setting/UiSystem_setting.h"
//#include "ui/friend/UiFriends.h"
//#include "ui/tower/UiTowerCombat.h"
//#include "ui/tower/UiTowerChallenge.h"
//#include "ui/tower/UiTowerChallengeReport.h"
//#include "ui/adventure_log/UiAdventureLog.h"
//#include "localserver/LocalServer_FoodMedicament.h"
//#include <Config.h>
//#include "MacroUtility.h"
//#include "map/building/BuildingMgr.h"
//#include "animator/AnimController.h"
//#include "render/ModelRender.h"
//#ifdef USE_WWISE
//#include "wwise/WwiseSoundMgr.h"
//#endif
//#include "ui/npcDsk/UiStation_diplomatic.h"
//#include "handler/HandlerNpc.h"
//#include "npc/NpcPetFight.h"
//#include "ui/npcEquip/UiNpc_show.h"
//#include "plot/PlotComponent.h"
//#include "handler/HandlerLogin.h"
//#include "localserver/LocalServer_NpcPetFight.h"
//#include "pet/PetComponentMgr.h"
//#include "npc/NpcComponentMgr.h"
//#include "ui/cook/UiCooking_machine.h"
//#include "map/WarFogRenderComponent.h"
//#include "monster/MonsterComponentMgr.h"
//#include "map/airship/AirshipGunBehavior.h"
//#include "config/BuildRoomCfg.h"
//#include "utils/FpsLabel.h"
//
//#include "map/BuildingPlantCheckMgr.h"
//#include "ui/networkLog/NetWorkLog.h"
//#include "ModelModule.h"
//#include "handler/HandlerPlatform.h"
//#include "SandboxGamePlay.h"
//#include "../log/LogModule.h"
//#include "GameLog.h"
//#include "ui/common/UiUpdateNote.h"
//#include "ai/AIManager.h"
//#include "msg/MsgDebugTools.pb.h"
//#include "map/building/NpcBadgeRoomCheck.h"
//#include "config/SystemSettingCfg.h"
//#include "handler/HandlerAppsFlyer.h"
//#include "handler/HandlerAdventureTask.h"
//#include "handler/HandlerTowerChallenge.h"
//#include "ui/sqad_pvp/UiSquad_pvp.h"
//#include "ui/sqad_pvp/UiSquad_pvp_result.h"
//#include "ui/npc/UiStation_warehouse.h"
//#include "localserver/LocalServer_SquadPvp.h"
//#include "data/SquadPvpData.h"
//#include "data/NpcProtectorData.h"
//#include "data/role/Pet.h"
//#include "msg/MsgPet.pb.h"
//#include "ui/sqad_pvp/UiSquad_pvp_combat.h"
//#include "ui/sqad_pvp/UiSquad_pvp_settlement.h"
//#include "localserver/LocalServer_Dummy.h"
//#include "handler/HandlerSquadPvp.h"
//#include "ui/sqad_pvp/UiSquad_pvp_stageswitching.h"
//#include "map/building/NpcBadgeAutoPlaceComponent.h"
//#include "config/AddBagSizeCfg.h"
//#include "ui/common/UiMsgBox.h"
//#include "ui/hangup/UiHangup_savenpc.h"
//#include "handler/HandlerFieldAdventure.h"
//#include "ui/hangup/UiHangup_selnpc.h"
//#include "ui/hangup/UiHangup_farrm_settlement.h"
//#include "hangup/HangupCommonMapMgr.h"
//#include "hangup/HangupCommonMapCfg.h"
//#include "ui/common/UiMsgBoxToast.h"
//#include "ui/npc/UiNpc_beauty.h"
//#include "data/HangupFarmData.h"
//#include "localserver/LocalServer_FieldAdventureDisplay.h"
//#include "ui/refining/UiRefining.h"
//#include "ui/airship_wreckage/UiSpacecraftRepair.h"
//#include "ui/refining/UiRefining_log.h"
//#include "ui/refine_food/UiRefine_food.h"
//#include "ui/hangup/UiHangup_ore_refiner.h"
//#include "ui/refining/UiRefining_log.h"
//#include <handler/HandlerGoblinTribute.h>


using namespace NS;

//---------------------------------------------------------------------------------------------------
bool GmlineS::run(const tstring& trimGmline)
{
	return HandlerItem::getInstance().requestItemPM(fmt::format("<font>{0}</font>", trimGmline));
}

GmlineC::GmlineC()
{
	//m_gmMap.emplace("@showid",			std::bind(&GmlineC::showid, this));
	//m_gmMap.emplace("@hideid",			std::bind(&GmlineC::hideid, this));
	//m_gmMap.emplace("@showdialog",		std::bind(&GmlineC::showdialog, this));
	//m_gmMap.emplace("@net",				std::bind(&GmlineC::createNetWorkLog, this));
	//m_gmMap.emplace("@sendlog",			std::bind(&GmlineC::sendTestLogToSrv, this));
	//m_gmMap.emplace("@tran",			std::bind(&GmlineC::tranlan, this));
	//m_gmMap.emplace("@openscore",		std::bind(&GmlineC::openScoreReq, this));
	//m_gmMap.emplace("@openlocalpush",	std::bind(&GmlineC::openLocalPushReq, this));
	//m_gmMap.emplace("@watchtv",			std::bind(&GmlineC::watchAdmobReq, this));
	//m_gmMap.emplace("@friendlist",		std::bind(&GmlineC::getFriendList, this));
	//m_gmMap.emplace("@addressbook",		std::bind(&GmlineC::registerAddressBook, this));
	//m_gmMap.emplace("@player",			std::bind(&GmlineC::addPlayer, this));
	//m_gmMap.emplace("@switchskin",		std::bind(&GmlineC::switchskin, this));
	//m_gmMap.emplace("@shownode",		std::bind(&GmlineC::shownode, this));
	//m_gmMap.emplace("@showequip",		std::bind(&GmlineC::showequip, this));
	//m_gmMap.emplace("@gameobject",		std::bind(&GmlineC::gameobject, this));
	//m_gmMap.emplace("@daynightadd",		std::bind(&GmlineC::daynightadd, this));
	//m_gmMap.emplace("@daynightopen",	std::bind(&GmlineC::daynightopen, this));
	//m_gmMap.emplace("@undergroundlightopen",std::bind(&GmlineC::undergroundLightOpen, this));
	//m_gmMap.emplace("@daynight",		std::bind(&GmlineC::daynight, this));
	//m_gmMap.emplace("@testnews",		std::bind(&GmlineC::testnews, this));
	//m_gmMap.emplace("@testwave",		std::bind(&GmlineC::testwave, this));
	//m_gmMap.emplace("@clearbag",		std::bind(&GmlineC::clearBag, this));
	//m_gmMap.emplace("@turnonbgm",		std::bind(&GmlineC::turnOnBgm, this));
	//m_gmMap.emplace("@turnoffbgm",		std::bind(&GmlineC::turnOffBgm, this));
	//m_gmMap.emplace("@testbossnews",	std::bind(&GmlineC::testbossnews, this));
	//m_gmMap.emplace("@send",			std::bind(&GmlineC::send, this));
	//m_gmMap.emplace("@turnonuibatching",std::bind(&GmlineC::turnOnUiBatching, this));
	//m_gmMap.emplace("@turnoffuibatching",std::bind(&GmlineC::turnOffUiBatching, this));
	//m_gmMap.emplace("@turnofffpslimit",	std::bind(&GmlineC::turnOffFpsLimit, this));
	//m_gmMap.emplace("@turnonfpslimit",	std::bind(&GmlineC::turnOnFpsLimit, this));
	//m_gmMap.emplace("@parallelupdateanimation",std::bind(&GmlineC::parallelUpateAnimation, this));
	//m_gmMap.emplace("@stat",			std::bind(&GmlineC::stat, this));
	//m_gmMap.emplace("@playsound",		std::bind(&GmlineC::playSound, this));
	//m_gmMap.emplace("@soundsetstate",	std::bind(&GmlineC::soundSetState, this));
	//m_gmMap.emplace("@soundsetswitch",	std::bind(&GmlineC::soundSetSwitch, this));
	//m_gmMap.emplace("@adddummymob",		std::bind(&GmlineC::addDummyMob, this));
	//m_gmMap.emplace("@adddummyplayer",	std::bind(&GmlineC::addDummyPlayer, this));
	//m_gmMap.emplace("@showairbrick",	std::bind(&GmlineC::showAirBrick, this));
	//m_gmMap.emplace("@changeshipguntype",std::bind(&GmlineC::changeShipGunType, this));
	//m_gmMap.emplace("@setshipposition", std::bind(&GmlineC::setShipPosition, this));
	//m_gmMap.emplace("@setcameraparam",  std::bind(&GmlineC::setCameraParam, this));
	//m_gmMap.emplace("@setcamerapos", std::bind(&GmlineC::setCameraPos, this));
	//m_gmMap.emplace("@clientswitchmap", std::bind(&GmlineC::clientSwitchMap, this));
	//m_gmMap.emplace("@clientquitmap",   std::bind(&GmlineC::clientquitmap, this));
	//m_gmMap.emplace("@changeshipcrystalstatus",	std::bind(&GmlineC::changeShipCrystalStatus, this));
	//// 商队到来
	//m_gmMap.emplace("@lsnpccaravanarrival",	std::bind(&GmlineC::NpcCaravanArrival, this));
	//// 商队离开
	//m_gmMap.emplace("@lsnpccaravanleave",std::bind(&GmlineC::NpcCaravanLeave, this));
	//// 商队逗留
	//m_gmMap.emplace("@lsnpccaravanstay",std::bind(&GmlineC::NpcCaravanStay, this));
	//// 商队被打劫(杀人越货)
	//m_gmMap.emplace("@lsrobnpccaravan",std::bind(&GmlineC::RobNpcCaravan, this));
	//// 以物易物
	//m_gmMap.emplace("@lsnpcgiftexchange",std::bind(&GmlineC::GiftExchange, this));
	//// 驿站创建协助塔防
	//m_gmMap.emplace("@lsnpcdakcreatedefensemob",std::bind(&GmlineC::NpcDakCreateDefenseMob, this));
	//// NPC大悬赏创建怪物
	//m_gmMap.emplace("@lsnpcpetfight",	std::bind(&GmlineC::NpcPetFight, this));
	//m_gmMap.emplace("@lsnpcbadgeroomcheckupdatecounter",std::bind(&GmlineC::NpcBadgeRoomCheckUpdateCounter, this));
	//m_gmMap.emplace("@lssethidecropinevent",std::bind(&GmlineC::SetHideCropInEvent, this));
	//m_gmMap.emplace("@lssethidebuildinginevent",std::bind(&GmlineC::SetHideBuildingInEvent, this));
	//// 批量执行.
	//m_gmMap.emplace("@rungmscript",		std::bind(&GmlineC::rungmscript, this));
	//m_gmMap.emplace("@lsadditem",		std::bind(&GmlineC::addLocalItem, this));
	//m_gmMap.emplace("@lslevelup",		std::bind(&GmlineC::levelup, this));
	//m_gmMap.emplace("@lssethp",			std::bind(&GmlineC::sethp, this));
	//m_gmMap.emplace("@lssetmp",			std::bind(&GmlineC::setmp, this));
	//m_gmMap.emplace("@lsfullhpmp",		std::bind(&GmlineC::fullHpMp, this));
	//m_gmMap.emplace("@lssetfood",		std::bind(&GmlineC::setfood, this));
	//m_gmMap.emplace("@lsaddexp",		std::bind(&GmlineC::addexp, this));
	//m_gmMap.emplace("@lsaddcash",		std::bind(&GmlineC::addcash, this));
	//m_gmMap.emplace("@lsaddcoin",		std::bind(&GmlineC::addcoin, this));
	//m_gmMap.emplace("@lsaddticket",		std::bind(&GmlineC::addticket, this));
	//m_gmMap.emplace("@lsaddmob",		std::bind(&GmlineC::addmob, this));
	//m_gmMap.emplace("@lsclearmobandnpc", std::bind(&GmlineC::ClearMobAndNpc, this));
	//m_gmMap.emplace("@lsdisableautobirthmob",std::bind(&GmlineC::DisableAutobirthMob, this));
	//m_gmMap.emplace("@lsenableautobirthmob",std::bind(&GmlineC::EnableAutobirthMob, this));
	//m_gmMap.emplace("@lsclearallmob",	std::bind(&GmlineC::ClearAllMob, this));
	//m_gmMap.emplace("@lsclearallnpc",	std::bind(&GmlineC::ClearAllNpc, this));
	//m_gmMap.emplace("@lssetallaienable",std::bind(&GmlineC::SetAllAIEnable, this));
	//m_gmMap.emplace("@lssetachieveevent",std::bind(&GmlineC::setachieveevent, this));
	//m_gmMap.emplace("@lsteleport",		std::bind(&GmlineC::teleport, this));
	//m_gmMap.emplace("@lsgoprivatemap",	std::bind(&GmlineC::goprivatemap, this));
	//m_gmMap.emplace("@lsaddbuff",		std::bind(&GmlineC::addBuff, this));
	//m_gmMap.emplace("@lsaddtargetbuff",	std::bind(&GmlineC::addTargetBuff, this));
	//m_gmMap.emplace("@lssurvivaltrigger",std::bind(&GmlineC::survivaltrigger, this));
	//m_gmMap.emplace("@showmap",			std::bind(&GmlineC::showmap, this));
	//m_gmMap.emplace("@unlockallfog",	std::bind(&GmlineC::unlockAllFog, this));
	//m_gmMap.emplace("@towerpvp",		std::bind(&GmlineC::towerpvp, this));
	//m_gmMap.emplace("@towerpvprevenge",	std::bind(&GmlineC::TowerpvpRevenge, this));
	//m_gmMap.emplace("@towergettomb",	std::bind(&GmlineC::towergettomb, this));
	//m_gmMap.emplace("@towerstartpvp",	std::bind(&GmlineC::towerpvpstart, this));
	//m_gmMap.emplace("@towerendpvp",		std::bind(&GmlineC::towerpvpend, this));
	//m_gmMap.emplace("@droptest",		std::bind(&GmlineC::droptest, this));
	//m_gmMap.emplace("@checkroom",		std::bind(&GmlineC::checkroom, this));
	//m_gmMap.emplace("@lua_gm",			std::bind(&GmlineC::LUA_GM, this));
	//m_gmMap.emplace("@lsaddtowerpvpmob",std::bind(&GmlineC::AddTowerPVPMob, this));
	//m_gmMap.emplace("@setplayermodel",	std::bind(&GmlineC::setPlayerModel, this));
	//m_gmMap.emplace("@lssetplantgrowtime",std::bind(&GmlineC::setplantgrowtime, this));
	//m_gmMap.emplace("@lssettbdebug",	std::bind(&GmlineC::settbdebug, this));
	//m_gmMap.emplace("@lsnotifydakevent",std::bind(&GmlineC::NotifyNpcDakEvent, this));
	//m_gmMap.emplace("@crystalbuff",		std::bind(&GmlineC::CrystalBuff, this));
	//m_gmMap.emplace("@showroom",		std::bind(&GmlineC::showroom, this));
	//m_gmMap.emplace("@showallroom",		std::bind(&GmlineC::showallroom, this));
	//m_gmMap.emplace("@showfog",			std::bind(&GmlineC::showfog, this));
	//m_gmMap.emplace("@scalert",			std::bind(&GmlineC::scaleRT, this));
	//m_gmMap.emplace("@airshipupdate",	std::bind(&GmlineC::airshipupdate, this));
	//m_gmMap.emplace("@airshipgunupgrade",std::bind(&GmlineC::airshipGunUpgrade, this));
	//m_gmMap.emplace("@showplantpos",	std::bind(&GmlineC::showplantpos, this));
	//m_gmMap.emplace("@showdebuginfo",	std::bind(&GmlineC::showdebuginfo, this));
	//m_gmMap.emplace("@checkallbuildings",std::bind(&GmlineC::checkAllBuildings, this));
	//m_gmMap.emplace("@openlivechat",	std::bind(&GmlineC::openLC, this));
	//m_gmMap.emplace("@setforbidchangezoom",std::bind(&GmlineC::setForbidChangeZoom, this));
	//m_gmMap.emplace("@uploadlogs",		std::bind(&GmlineC::uploadLogs, this));
	//m_gmMap.emplace("@canceluploadlog",	std::bind(&GmlineC::cancelUploadLog, this));
	//m_gmMap.emplace("@reqagreementstatus",std::bind(&GmlineC::reqAgreementStatus, this));
	//m_gmMap.emplace("@reqagreementlist",std::bind(&GmlineC::reqAgreementList, this));
	//m_gmMap.emplace("@agreeagreement",	std::bind(&GmlineC::agreeAgreement, this));
	//m_gmMap.emplace("@afevent",			std::bind(&GmlineC::afevent, this));
	//m_gmMap.emplace("@fbevent",			std::bind(&GmlineC::fbevent, this));
	//m_gmMap.emplace("@rating",			std::bind(&GmlineC::ratingGame, this));
	//m_gmMap.emplace("@uploadlogfile",	std::bind(&GmlineC::uploadlogfile, this));
	//m_gmMap.emplace("@showupdate",		std::bind(&GmlineC::showUpgradeNote, this));
	//m_gmMap.emplace("@invalidaccount",	std::bind(&GmlineC::invalidAccount, this));
	//m_gmMap.emplace("@drawplantgrid",	std::bind(&GmlineC::drawplantgrid, this));
	//m_gmMap.emplace("@reloadsercfg",	std::bind(&GmlineC::reloadServerCfg, this));
	//m_gmMap.emplace("@tornadoplay",		std::bind(&GmlineC::tornadoplay, this));
	//m_gmMap.emplace("@toggle_plain_lightmap",	std::bind(&GmlineC::togglePlainLightmap, this));
	//m_gmMap.emplace("@lssquadpvpgofight", std::bind(&GmlineC::SquadPvpGoFight, this));
	//m_gmMap.emplace("@lssquadpvpquitmap", std::bind(&GmlineC::SquadPvpQuitMap, this));
	//m_gmMap.emplace("@lssquadpvpcheck", std::bind(&GmlineC::SquadPvpCheck, this));
	//m_gmMap.emplace("@lsdummyplayer", std::bind(&GmlineC::createDummyPlayer, this));
	//// 跳过新手地牢
	//m_gmMap.emplace("@lsskipnewbie", std::bind(&GmlineC::skipNewbie, this));
	//m_gmMap.emplace("@getadventurelog", std::bind(&GmlineC::getAdventureLog, this));
	//m_gmMap.emplace("@lsjumpachieve", std::bind(&GmlineC::jumpAchieve, this));
	//m_gmMap.emplace("@fastnpcbadge", std::bind(&GmlineC::fastNpcBadge, this));
	//m_gmMap.emplace("@fieldadventrue", std::bind(&GmlineC::fieldAdventrue, this));
	//m_gmMap.emplace("@lshangupcommonentermap", std::bind(&GmlineC::HangupCommonEnterMap, this));
	//m_gmMap.emplace("@lshangupcommonquitmap", std::bind(&GmlineC::HangupCommonQuitMap, this));
	//m_gmMap.emplace("@lshangupcommonswitchhanguptype", std::bind(&GmlineC::HangupCommonSwitchHangupType, this));
	//m_gmMap.emplace("@lsfieldadventruedisplaymonsterdefid", std::bind(&GmlineC::SetFieldAdventureDisplayTestMonsterDefId, this));
	//m_gmMap.emplace("@lsfieldadventruedisplaytaskid", std::bind(&GmlineC::SetFieldAdventureDisplayTestTaskId, this));
	//m_gmMap.emplace("@lsbeginpatrolattack", std::bind(&GmlineC::BeginPatrolAttack, this));
	//m_gmMap.emplace("@lsendpatrolattack", std::bind(&GmlineC::EndPatrolAttack, this));
	//m_gmMap.emplace("@movetributelastawardtime", std::bind(&GmlineC::moveTributeLastAwardTime, this));
}

GmlineC::~GmlineC()
{
	m_gmMap.clear();
}

//---------------------------------------------------------------------------------------------------
bool GmlineC::run(const tstring& trimGmline)
{
	//init gmline
	m_gmline = trimGmline;
	m_gmParaList.clear();
	StringUtil::splitString(m_gmParaList, m_gmline, " ");

	if (m_gmParaList.size() < 1)
		return false;

	//Exec gm
	auto&& findIt = m_gmMap.find(m_gmParaList[0]);
	return findIt == m_gmMap.end() ? false : findIt->second();

	return false;
}

bool GmlineC::runQuery(const NS::tstring& trimQuery)
{
	//init gmline
	m_gmline = trimQuery;
	m_gmParaList.clear();
	StringUtil::splitString(m_gmParaList, m_gmline, " ");

	//Exec Server Query
	//if (m_gmline.find(":igg_query_server_info") != tstring::npos) return iggQueryServerInfo();

	return false;
}
//
//bool GmlineC::showroom()
//{
//	auto pself = PlayerMgr::getInstance().getSelf();
//
//	auto grid = pself->getCoord();
//
//	LocalServer_BuildLogicCli::getInstance().Debug_showroom(grid.x, grid.y);
//
//	return true;
//}
//
//bool GmlineC::showallroom()
//{
//	LocalServer_BuildLogicCli::getInstance().Debug_showallroom();
//
//	return true;
//}
//
//bool GmlineC::airshipupdate()
//{
//	MsgPB::MsgBuildTowerUpdateTowerPVPAirShip_C2MS msg;
//
//	MsgMgr::getInstance().sendMessage(msg.msgtype(), &msg);
//
//	return true;
//}
//
//bool GmlineC::showplantpos()
//{
//	std::map<int/*plant type*/, std::list<PlantBasePtr> > mapTypePlant;
//	{
//		auto mapPlant = LocalServer_BuildLogicCli::getInstance().GetCurMapABS()->GetPlantMapNature();
//		for (auto& it : mapPlant) {
//			mapTypePlant[it.second->GetPlantType()].push_back(it.second);
//		}
//	}
//
//	int32 plantType = -1;
//	getGmPara(plantType, 1);
//
//	FILE * pfile = fopen("plant.txt", "wb");
//
//	if (plantType < 0) {
//		for (auto& itType : mapTypePlant) {
//			fprintf(pfile, "\r\n\r\n plant:%d", itType.first);
//			for (auto& itPlant : itType.second) {
//				fprintf(pfile, "[%d:%d]\r\n", itPlant->GetGridPosX(), itPlant->GetGridPosY());
//			}
//		}
//
//	}
//	else {
//		auto itType = mapTypePlant.find(plantType);
//		fprintf(pfile, "\r\n\r\n plant:%d", plantType);
//		if (mapTypePlant.end() != itType) {
//			for (auto& itPlant : itType->second) {
//				fprintf(pfile, "[%d:%d]\r\n", itPlant->GetGridPosX(), itPlant->GetGridPosY());
//			}
//		}
//	}
//
//	fclose(pfile);
//	return true;
//}
//
//bool GmlineC::drawplantgrid()
//{
//	auto pself = PlayerMgr::getInstance().getSelf();
//
//	int32 x = -1;// pself->getCoord().x;
//	int32 y = -1;// pself->getCoord().y;
//
//	if (!getGmPara(x, 1))
//		x = pself->getCoord().x;
//	if (!getGmPara(y, 2))
//		y = pself->getCoord().y;
//
//	auto pMap = LocalServer_BuildLogicCli::getInstance().GetCurMapABS();
//	if (!pMap)
//		return true;
//
//	auto itPlant = pMap->GetStaticObjMgr().GetMapUserPlant().find(CIntPoint(x, y));
//	if (pMap->GetStaticObjMgr().GetMapUserPlant().end() == itPlant)
//		return true;
//
//	if (!itPlant->second)
//		return true;
//
//	auto Grids = itPlant->second->GetGrid();
//
//	for (auto& itGrid : Grids) {
//		MsgPB::MsgDebugDrawOnClient_S2C MsgDebugLine;
//		MsgDebugLine.set_draw_type(DEBUG_DRAW_LINE);
//		MsgDebugLine.set_debug_msg("");
//		MsgDebugLine.set_serialid(0);
//		MsgDebugLine.set_show_time(3);
//
//		int32 wx = pMap->PosToWorldX(itGrid.x);
//		int32 wy = pMap->PosToWorldY(itGrid.y);
//
//		auto theMsgPoint = MsgDebugLine.add_line_points();
//		theMsgPoint->set_x(wx - 50);
//		theMsgPoint->set_y(wy + 50);
//
//		theMsgPoint = MsgDebugLine.add_line_points();
//		theMsgPoint->set_x(wx - 50);
//		theMsgPoint->set_y(wy - 50);
//
//		theMsgPoint = MsgDebugLine.add_line_points();
//		theMsgPoint->set_x(wx + 50);
//		theMsgPoint->set_y(wy - 50);
//
//		theMsgPoint = MsgDebugLine.add_line_points();
//		theMsgPoint->set_x(wx + 50);
//		theMsgPoint->set_y(wy + 50);
//
//
//		theMsgPoint = MsgDebugLine.add_line_points();
//		theMsgPoint->set_x(wx - 50);
//		theMsgPoint->set_y(wy + 50);
//
//		LocalServer_BuildLogicCli::getInstance().sendMsg(MsgDebugLine);
//	}
//
//	return true;
//}
//
//bool GmlineC::showdebuginfo()
//{
//	if (m_gmParaList.size() < 2) { return false; }
//
//	int iShow = 0;
//	if (!bzbee::StringUtil::stringToType(iShow, m_gmParaList[1])) { return false; }
//
//	FpsLabel::setRenderFps(iShow > 0);
//
//	return true;
//}
//
//bool GmlineC::airshipGunUpgrade()
//{
//	for (auto&& pir : BuildingMgr::getInstance().getCoordId2Buildings())
//	{
//		auto&& pGameObj = pir.second->getGameObject();
//		if (pGameObj)
//		{
//			auto&& pAirshipGunBehavior = pGameObj->findComponent<AirshipGunBehavior>();
//			if (pAirshipGunBehavior)
//			{
//				pAirshipGunBehavior->playUpgradeEffect();
//				return true;
//			}
//		}
//	}
//
//	return true;
//}
//
//bool GmlineC::checkAllBuildings()
//{
//	BuildingPlantCheckMgr::getInstance().checkAllBuildings();
//
//	return false;
//}
//
//bool GmlineC::towerPvPRevenge()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//	tstring dialog = EMPTY_STRING;
//	if (!getGmPara(dialog, 1)) return false;
//	auto iggid = atoi(dialog.c_str());
//	return HandlerMatchEnemy::getInstance().revengeReq(iggid);
//}
//
//bool GmlineC::showmap()
//{
//	LocalServer_BuildLogicCli::getInstance().showmap();
//	return true;
//}
//
//bool GmlineC::unlockAllFog()
//{
//	LocalServer_BuildLogicCli::getInstance().UnLockAllFog();
//	return true;
//}
//
//bool GmlineC::towerpvp()
//{
//	NS::I64 iggid;
//	if (!getGmPara(iggid, 1)) return false;
//
//	MsgPB::MsgTowerPVPPreAttack_C2S msg;
//	msg.set_iggid_defence(iggid);
//	MsgMgr::getInstance().sendMessage(msg.msgtype(), &msg);
//	return true;
//}
//
//bool GmlineC::TowerpvpRevenge()
//{
//	NS::I64 Revengeiggid;
//	if (!getGmPara(Revengeiggid, 1)) return false;
//
//	MsgPB::MsgTowerpvpRevengePairReqC2S msg;
//	msg.set_revengeiggid(Revengeiggid);
//	msg.set_msg_ver(1);
//	MsgMgr::getInstance().sendMessage(msg.msgtype(), &msg);
//	return true;
//}
//
//bool GmlineC::towerpvpstart()
//{
//	MsgPB::MsgTowerPVPAttackStart_C2S msg;
//	MsgMgr::getInstance().sendMessage(msg);
//	return true;
//}
//
//bool GmlineC::towerpvpend()
//{
//	MsgPB::MsgTowerPVPAttackEnd_C2S msg;
//	//msg.set_iggid(m_iggid);
//	MsgMgr::getInstance().sendMessage(msg);
//	return true;
//}
//
//bool GmlineC::towergettomb()
//{
//	MsgPB::MsgTowerTombstoneGetReward_C2S msg;
//
//	int32 x = 192;
//	int32 y = 127;
//	getGmPara(x, 1);
//	getGmPara(y, 2);
//
//	msg.set_tomb_x(x);
//	msg.set_tomb_y(y);
//
//	MsgMgr::getInstance().sendMessage(msg.msgtype(), &msg);
//	return true;
//}
//
//bool GmlineC::droptest()
//{
//	int32 drop_id = -1;
//	if (!getGmPara(drop_id, 1))
//		return false;
//	int32 count = 1;
//	getGmPara(count, 2);
//
//	std::vector<DropRet> vDropRet;
//	std::map<int32, DropRet> mapItemCount;
//	for (int32 i = 0; i < count; i++) {
//		vDropRet.resize(0);
//		DropProcessCli::GetInstance().DoDrop(drop_id, vDropRet);
//		for (size_t j = 0; j < vDropRet.size(); j++) {
//			const DropRet& dr = vDropRet[j];
//			mapItemCount[dr.m_dataid].m_iCount += dr.m_iCount;
//		}
//	}
//
//	{
//		FILE * pfile = fopen("droptest_unsort.txt", "wb");
//		if (!pfile) {
//			return true;
//		}
//
//		fprintf(pfile, "drop id:%d exe_count:%d\r\n\r\n", drop_id, count);
//
//		fprintf(pfile, "Item ID\t\t" "Item Name\t\t" "Quality\t\t" "Count\r\n");
//
//		for (auto& it : mapItemCount) {
//			int32 quality = -1;
//			std::string strName;
//			auto pCfg = ItemCfg::getInstance().getItemById(it.first);
//			if (pCfg) {
//				quality = pCfg->m_qualityGrade;
//				strName = pCfg->m_name_untrans;
//			}
//
//			fprintf(pfile, "item:%7d\t\t" "%s\t\t" "%d\t\t" "item_count:%d""\r\n",
//				it.first, strName.c_str(), quality, it.second.m_iCount);
//		}
//		fclose(pfile);
//	}
//
//	{
//		FILE * pfile = fopen("droptest.txt", "wb");
//		if (!pfile) {
//			return true;
//		}
//
//		fprintf(pfile, "drop id:%d exe_count:%d\r\n\r\n", drop_id, count);
//
//		fprintf(pfile, "Item ID\t\t" "Item Name\t\t" "Quality\t\t" "Count\r\n");
//
//		std::multimap<int32, int32> mapSort;
//		for (auto& it : mapItemCount) {
//			mapSort.emplace(it.second.m_iCount, it.first);
//		}
//
//		for (auto& it : mapSort) {
//			int32 quality = -1;
//			std::string strName;
//			auto pCfg = ItemCfg::getInstance().getItemById(it.second);
//			if (pCfg) {
//				quality = pCfg->m_qualityGrade;
//				strName = pCfg->m_name_untrans;
//			}
//
//			fprintf(pfile, "item:%7d\t\t" "%s\t\t" "%d\t\t" "item_count:%d""\r\n",
//				it.second, strName.c_str(), quality, it.first);
//		}
//		fclose(pfile);
//	}
//
//	return true;
//}
//
//bool GmlineC::checkroom()
//{
//	int x;
//	int y;
//	getGmPara(x, 1);
//	getGmPara(y, 2);
//
//	MsgPB::MsgBuildingRoomConditionGet_C2S msg;
//
//	msg.set_pos_x(x);
//	msg.set_pos_y(y);
//
//	MsgMgr::getInstance().sendMessage(msg.msgtype(), &msg);
//
//	return true;
//}
//
//uint GmlineC::requestGmParaCount(uint count)
//{
//	return static_cast<uint>(m_gmParaList.size()) >= count + 1;
//}
//
//bool GmlineC::showdialog()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//	tstring dialog = EMPTY_STRING;
//	if (!getGmPara(dialog, 1)) return false;
//	if (dialog.find("parsebaglock") != tstring::npos)
//	{
//		int totalPower{ 0 };
//		NS::tstring name = {};
//		auto map = AddBagSizeCfg::getInstance().getMap();
//		for (int i=11;i<=15;++i)
//		{
//			auto iter = map.find(i);
//			if (iter != map.end())
//			{
//				auto str = fmt::format("id={0},cast={1}", iter->second.m_id,iter->second.m_cost);
//				name += str;
//				totalPower += iter->second.m_cost;
//			}
//		}
//		UiMsgBox::normalMsgBox(name,fmt::format("total={0}", totalPower));
//	}
//	if (dialog.find("refing") != tstring::npos)
//	{
//		UiMgr::getInstance().showDialog<UiRefining>();
//	}
//	if (dialog.find("refood") != tstring::npos)
//	{
//		UiMgr::getInstance().showDialog<UiRefine_food>();
//	}
//	if (dialog.find("sethanguptime") != tstring::npos)
//	{
//		int fenceDefId{};
//		int time{};
//		if (!getGmPara(fenceDefId, 2)) return false;
//		if (!getGmPara(time, 3)) return false;
//		HangupFarmData::getInstance().modifHangupTimeOfUseTest(fenceDefId, time);
//	}
//	if (dialog.find("insertpvprank") != tstring::npos)
//	{
//		auto ui = UiMgr::getInstance().findDialog<UiSquad_pvp>();
//		if (ui)
//		{
//			NS::I64 iggid{};
//			int rank{};
//			if (!getGmPara(iggid, 2)) return false;
//			if (!getGmPara(rank, 3)) return false;
//			ui->saveSnapReq(iggid,rank);
//		}
//	}
//	if (dialog.find("seglevel") != tstring::npos)
//	{
//		int level{};
//		if (!getGmPara(level, 2)) return false;
//		SquadPvpDataMgr::getInstance().setViewData(100, level, 0, 0, 0);
//		auto me = PlayerMgr::getInstance().getSelf();
//		if (me)
//			me->calcAttr();
//	}
//	if (dialog.find("close") != tstring::npos)
//	{
//		MsgLocGameBackpressedReq msg;
//		NS::MsgMgr::getInstance().sendMessage(MSG_LOC_GAME_BACKPRESSED_REQ, &msg);
//	}
//	if (dialog.find("npcbeauty") != tstring::npos)
//	{
//		UiMgr::getInstance().showDialog<UiNpc_beauty>(true);
//	}
//	if (dialog.find("hangupsavenpc") != tstring::npos)
//	{
//		UiMgr::getInstance().showDialog<UiHangup_savenpc>();
//	}
//	if (dialog.find("hangupselnpc") != tstring::npos)
//	{
//		UiMgr::getInstance().showDialog<UiHangup_selnpc>(true);
//	}
//	else if (dialog.find("hangupfarmend") != tstring::npos)
//	{
//		UiMgr::getInstance().showDialog<UiHangup_farrm_settlement>(true);
//	}
//	else if (dialog.find("adm") != tstring::npos)
//	{
//		auto ui = UiMgr::getInstance().findDialog<UiSquad_pvp>();
//		if (!ui)
//		{
//			ui = UiMgr::getInstance().showDialog<UiSquad_pvp>();
//		}
//		int defId{};
//		float scale{};
//		float rotation{};
//		float offsetX{};
//		float offsetY{};
//		if (!getGmPara(defId, 2)) return false;
//		if (!getGmPara(scale, 3)) return false;
//		if (!getGmPara(rotation, 4)) return false;
//		if (!getGmPara(offsetX, 5)) return false;
//		if (!getGmPara(offsetY, 6)) return false;
//		ui->adjustModelSize(defId, scale, rotation, offsetX, offsetY);
//	}
//	if (dialog.find("cook") != tstring::npos)
//	{
//		//UiMgr::getInstance().showDialog<UiCooking_machine>(true);
//		HandlerSquadPvp::getInstance().updateAllRankView();
//		UiMgr::getInstance().showDialog<UiSquad_pvp_stageswitching>();
//	}
//	else if (dialog.find("stationhouse") != tstring::npos)
//	{
//		UiMgr::getInstance().showDialog<UiStation_warehouse>();
//	}
//	else if (dialog.find("s_pvp") != tstring::npos)
//	{
//		UiMgr::getInstance().showDialog<UiSquad_pvp>(true);
//	}
//	else if (dialog.find("pvp_result") != tstring::npos)
//	{
//		NS::uint preScore{};
//		NS::uint newScore{};
//		if (!getGmPara(preScore, 2)) return false;
//		if (!getGmPara(newScore, 3)) return false;
//		auto dia = UiMgr::getInstance().showDialog<UiSquad_pvp_settlement>(true);
//		dia->setScore(preScore , newScore);
//	}
//	else if (dialog.find("comoper") != tstring::npos)
//	{
//		UiMgr::getInstance().showDialog<UiSquad_pvp_combat>(true);
//	}
//	else if (dialog.find("npclist") != tstring::npos)
//	{
//		UiMgr::getInstance().showDialog<UiNpc_show>(true);
//	}
//	else if (dialog.find("npcdsksend") != tstring::npos)
//	{
//		UiMgr::getInstance().showDialog<UiStation_diplomatic>(true);
//	}
//	else if (dialog.find("UiCraftDialog") != tstring::npos)
//	{
//		UiCraftDialog* pCraftDialog = UiMgr::getInstance().showDialog<UiCraftDialog>(true);
//		if (pCraftDialog)
//		{
//			pCraftDialog->refreshView();
//		}
//	}
//	else if (dialog.find("shopping") != tstring::npos)
//	{
//		// 跳转到商城;
//		LaunchShoppingMall e;
//		e.m_menu = MENU::CHARGE_PAGE;
//		e.m_shopType = SHOP_TYPE::CRYSTEAR;
//
//		EventMediator::getInstance()->postEvent(e);
//	}
//	else if (dialog.find("sign") != tstring::npos)
//	{
//		LuaCallback("show_window")("daily_login/daily_welfare.lua");
//	}
//	else if (dialog.find("piggy") != tstring::npos)
//	{
//		LuaCallback("show_window")("piggy/piggy_main.lua", false);
//	}
//	else if (dialog.find("enchant") != tstring::npos)
//	{
//		UiEnchantment* pUiEnchantDialog = UiMgr::getInstance().findDialog<UiEnchantment>();
//		if (!pUiEnchantDialog)
//			pUiEnchantDialog = UiEnchantment::create();
//
//		UiMgr::getInstance().showDialog(pUiEnchantDialog);
//	}
//	else if (dialog.find("bag") != tstring::npos)
//	{
//		auto&& pLobby = dynamic_cast<UiLobby*>(UiMgr::getInstance().findScreen(ScreenIds::UiLobby));
//		if (pLobby)
//		{
//			pLobby->openBag(true);
//		}
//	}
//	else if (dialog.find("box") != tstring::npos)
//	{
//		int boxType{};
//		if (!getGmPara(boxType, 2)) return false;
//
//		if(auto&& pUI = dynamic_cast<UiStorageBox*>(UiMgr::getInstance().showDialog<UiStorageBox>()))
//		{
//			pUI->refreshBox(static_cast<MsgPB::PACKET_TYPE>(boxType));
//
//			// 显示背包
//			auto&& pLobby = dynamic_cast<UiLobby*>(UiMgr::getInstance().findScreen(ScreenIds::UiLobby));
//			if (pLobby)
//			{
//				pLobby->openBag(true);
//			}
//		}
//	}
//	else if (dialog.find("chengjiu") != tstring::npos)
//	{
//		UiAchievements* pUiAchievements = UiMgr::getInstance().findDialog<UiAchievements>();
//		if (!pUiAchievements)
//		{
//			UiMgr::getInstance().showDialog(UiAchievements::create());
//		}
//	}
//	else if (dialog.find("shop1") != tstring::npos)
//	{
//		UiSysBuyShop* pShop = UiMgr::getInstance().findDialog<UiSysBuyShop>();
//		if (!pShop)
//		{
//			UiMgr::getInstance().showDialog(UiSysBuyShop::create(1001,0,0));
//		}
//	}
//	else if (dialog.find("shop2") != tstring::npos)
//	{
//		UiSysBuyShop* pShop = UiMgr::getInstance().findDialog<UiSysBuyShop>();
//		if (!pShop)
//		{
//			UiMgr::getInstance().showDialog(UiSysBuyShop::create(1002,0,0));
//		}
//	}
//	else if (dialog.find("shop3") != tstring::npos)
//	{
//		UiSysBuyShop* pShop = UiMgr::getInstance().findDialog<UiSysBuyShop>();
//		if (!pShop)
//		{
//			UiMgr::getInstance().showDialog(UiSysBuyShop::create(1003,0,0));
//		}
//	}
//	else if (dialog.find("shop4") != tstring::npos)
//	{
//		UiSysSellShop* pShop = UiMgr::getInstance().findDialog<UiSysSellShop>();
//		if (!pShop)
//		{
//			UiMgr::getInstance().showDialog(UiSysSellShop::create());
//		}
//	}
//	else if (dialog.find("shop") != tstring::npos)
//	{
//		tstring param2 = EMPTY_STRING;
//		if (!getGmPara(param2, 2)) return false;
//		UiSysBuyShop* pShop = UiMgr::getInstance().findDialog<UiSysBuyShop>();
//		if (!pShop)
//		{
//			int out{};
//			StringUtil::stringToType(out, param2);
//			UiMgr::getInstance().showDialog(UiSysBuyShop::create(out,0,0));
//		}
//	}
//	else if (dialog.find("mail") != tstring::npos)
//	{
//		UiMail* pMail = UiMgr::getInstance().findDialog<UiMail>();
//		if (!pMail)
//		{
//			UiMgr::getInstance().showDialog(UiMail::create());
//		}
//	}
//	else if (dialog.find("main_rank") != tstring::npos)
//	{
//		LuaCallback("show_window")("Rank/main_rank.lua", true);
//	}
//	else if (dialog.find("UiSystem_setting") != tstring::npos)
//	{
//		auto pDialog = UiSystem_setting::create();
//		UiMgr::getInstance().showModalDialog(pDialog);
//	}
//	else if (dialog.find("uifriends") != tstring::npos)
//	{
//		UiMgr::getInstance().showDialog<UiFriends>();
//	}
//// 	else if (dialog.find("auction") != tstring::npos)
//// 	{
//// 		UiMgr::getInstance().showDialog<UiAcuctionNpc>(true);
//// 	}
//// 	else if (dialog.find("history") != tstring::npos)
//// 	{
//// 		UiMgr::getInstance().showDialog<UiAcuctionRecord>();
//// 	}
//	else if (dialog.find("repair") != tstring::npos)
//	{
//		UiLobby* pUiLobby = UiMgr::getInstance().findDialog<UiLobby>();
//		if (pUiLobby)
//		{
//			pUiLobby->openBag(true);
//			auto&& pUiBag = pUiLobby->getUiBag();
//			if (pUiBag)
//			{
//				pUiBag->changeState(BagState::Repair);
//				pUiBag->setSelectTab(UiBag::TabName::Weqpon);
//
//				auto&& pRepair = UiMgr::getInstance().showDialog<UiBlacksmithShop>();
//				if (pRepair)
//				{
//					auto&& bagPos = CoordinateUtil::getScreenPosition(pUiBag) - glm::vec3(15.0f, 0.0f, 0.0f);
//					auto&& newPos = bagPos - glm::vec3(pUiBag->getSize().x*0.5f, 0.0f, 0.0f) - glm::vec3(pRepair->getSize().x*0.5f, 0.0f, 0.0f);
//					pRepair->setPosition(newPos);
//				}
//			}
//		}
//	}
//	else if (dialog.find("chat") != tstring::npos)
//	{
//		UiMgr::getInstance().showDialog<UiChatSystemFull>();
//	}
//	else if (dialog.find("poloish") != tstring::npos)
//	{
//		UiMgr::getInstance().showDialog<UiPoloShing>();
//	}
//	else if (dialog.find("dungeon") != tstring::npos)
//	{
//		auto pDialog = UiMgr::getInstance().showDialog<UiMsgBoxNpcDungeon>();
//		pDialog->refreshView(UiMsgBoxNpcDungeon::Display_Team);
//	}
//	else if (dialog.find("chest_coll") != tstring::npos)
//	{
//		int count{ 0 };
//		getGmPara<int>(count, 2);
//		if (count == 0)
//		{
//			count = 12;
//		}
//		auto&& pUi = UiMgr::getInstance().showDialog<UiChestCollection>(true);
//
//		std::vector<UiChestCollection::ItemInfo> itemVec;
//		for (int i = 0; i < count; ++i)
//		{
//			itemVec.push_back(UiChestCollection::ItemInfo(1301, 10));
//		}
//
//		pUi->setupUi(itemVec, "models/buildings/public/mimic/gold_chest.model");
//	}
//// 	else if (dialog.find("chest") != tstring::npos)
//// 	{
//// 		UiMgr::getInstance().showDialog<UiEndlessOpenChest>(true);
//// 	}
//	else if (dialog.find("bet") != tstring::npos)
//	{
//		UiMgr::getInstance().showDialog<UiEndlessModeBet>(true);
//	}
//	else if(dialog.find("fund") != tstring::npos)
//	{
//		UiMgr::getInstance().showDialog<UiFinancialSystem>();
//	}
//	else if (dialog.find("challenge_report") != tstring::npos)
//	{
//		int reason{};
//		getGmPara(reason, 2);
//
//		if (reason)
//		{
//			HandlerTowerChallenge::getInstance().sendMsgTowerChallengeEnd_C2S(reason);
//		}
//		else
//		{
//			auto&& pUi = UiMgr::getInstance().showDialog<UiTowerChallengeReport>();
//			pUi->refreshUi(TowerChallengeDataMgr::getInstance().getTcEndInfo());
//		}
//	}
//	else if(dialog.find("challenge") != tstring::npos)
//	{
//		bool bOpenUi{};
//		getGmPara(bOpenUi, 2);
//		if (bOpenUi)
//		{
//			auto&& pUi = UiMgr::getInstance().showDialog<UiTowerChallenge>();
//			pUi->refreshUi(TowerChallengeDataMgr::getInstance().getTcSummary());
//		}
//		else
//		{
//			HandlerTowerChallenge::getInstance().sendMsgTowerChallengeSummaryGet_C2S();
//		}
//	}
//	else if(dialog.find("combat") != tstring::npos)
//	{
//		UiMgr::getInstance().showDialog<UiTowerCombat>();
//	}
//	else if(dialog.find("adventurelog") != tstring::npos)
//	{
//		UiMgr::getInstance().showDialog<UiAdventureLog>();
//	}
//	else if (dialog.find("book") != tstring::npos)
//	{
//		UiMgr::getInstance().showDialog<UiGuideHandbook>();
//	}
//	else if (dialog.find("pet") != tstring::npos)
//	{
//		LuaCallback("show_window")("pet/pet_main.lua");
//	}
//	else if (dialog.find("fp") != tstring::npos)	// 首冲界面.
//	{
//		LuaCallback("show_window")("charge_reward/first_prize.lua");
//	}
//	else if (dialog.find("turntable") != tstring::npos)
//	{
//		int turnType{ 0 };
//		getGmPara<int>(turnType, 2);
//
//		LuaCallback("show_window")("turntable/turntable_main.lua", false, turnType);
//	}
//	else if (dialog.find("jackpot") != tstring::npos)
//	{
//		int num{ 0 };
//		getGmPara<int>(num, 2);
//
//		LuaCallback("show_window")("turntable/turntable_jackpot.lua", false, num);
//	}
//	else if (dialog.find("journal") != tstring::npos)
//	{
//		LuaCallback("show_window")("charge_reward/journal.lua", true);
//	}
//	else if (dialog.find("slot") != tstring::npos)
//	{
//		LuaCallback("show_window")("welfare/slot_machine_welfare.lua", true);
//	}
//	else if(dialog.find("badge") != tstring::npos)
//	{
//		auto map = NpcProtectorDataMgr::getInstance()->getProtectorDataMap();
//		for (auto it:map)
//		{
//			it.second->setState(BADGE_STATUS_NONE);
//		}
//	}
//	else if(dialog.find("queue_pos") != tstring::npos)
//	{
//		int curPos{ 0 };
//		getGmPara<int>(curPos, 2);
//
//		auto&& pQueueDialog = UiMgr::getInstance().findDialog<UiImporttant_reminder>();
//
//		if (curPos > 0)
//		{
//			if (!pQueueDialog)
//			{
//				pQueueDialog = UiImporttant_reminder::create();
//				UiMgr::getInstance().showModalDialog(pQueueDialog);
//			}
//
//			pQueueDialog->refreshView(curPos);
//		}
//		else
//		{
//			if (pQueueDialog)
//			{
//				UiMgr::getInstance().closeDialog(pQueueDialog);
//			}
//		}
//	}
//	else if (dialog.find("wreckage") != tstring::npos)
//	{
//		UiMgr::getInstance().showDialog<UiSpacecraftRepair>();
//	}
//	else if (dialog.find("orerefiner") != tstring::npos)
//	{
//		UiMgr::getInstance().showDialog<UiHangup_ore_refiner>();
//	}
//	else if (dialog.find("refininglog") != tstring::npos)
//	{
//		UiMgr::getInstance().showDialog<UiRefining_log>();
//	}
//	
//
//	return true;
//}
//
//bool GmlineC::daynightadd()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(2)) return false;
//	NS::uint t{};
//	int p{};
//	if (!getGmPara(t, 1)) return false;
//
//	if (!getGmPara(p, 2)) return false;
//
//	if (t == 1)		//CLOCK
//	{
//		DayNightData::getInstance().setOffset(p * DayNightTime::MINUTE_NUM * DayNightTime::SECOND_NUM);
//	}
//	else if (t == 2)
//	{
//		DayNightData::getInstance().setOffset(p * DayNightTime::SECOND_NUM);
//	}
//	else if (t == 3)
//	{
//		DayNightData::getInstance().setOffset(p);
//	}
//
//	return true;
//}
//
//bool GmlineC::daynight()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(3)) return false;
//	NS::uint clock{};
//	NS::uint minute{};
//	NS::uint second{};
//	if (!getGmPara(clock, 1)) return false;
//	if (!getGmPara(minute, 2)) return false;
//	if (!getGmPara(second, 3)) return false;
//
//	/*DayNightTime::Time gameTime;
//	gameTime.m_clock = (DayNightTime::CLOCK)clock;
//	gameTime.m_minute = (DayNightTime::MINUTE)minute;
//	gameTime.m_second = (DayNightTime::SECOND)second;
//	DayNightData::getInstance().setTime(gameTime);*/
//
//	auto&& pWorld = NS::SceneModule::getCurrentWorld();
//	auto&& pMapComponent = pWorld->getCurrentLevel()->findComponent<MapComponent>();
//	if (pMapComponent)
//	{
//		auto pDayNightComponent = pMapComponent->findComponent<DayNightComponent>();
//		if (pDayNightComponent)
//		{
//			pDayNightComponent->setDayNightTime(clock, minute, second);
//		}
//	}
//
//	return true;
//}
//
//bool GmlineC::daynightopen()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//	bool isOpen{};
//	if (!getGmPara(isOpen, 1)) return false;
//
//	auto&& pWorld = NS::SceneModule::getCurrentWorld();
//	auto&& pMapComponent = pWorld->getCurrentLevel()->findComponent<MapComponent>();
//	if (pMapComponent)
//	{
//		auto pDayNightComponent = pMapComponent->findComponent<DayNightComponent>();
//		if (pDayNightComponent)
//		{
//			pDayNightComponent->dayNightOpen(isOpen);
//		}
//	}
//
//	return true;
//}
//
//bool GmlineC::undergroundLightOpen()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//	bool isOpen{};
//	if (!getGmPara(isOpen, 1)) return false;
//
//	auto&& pWorld = NS::SceneModule::getCurrentWorld();
//	auto&& pMapComponent = pWorld->getCurrentLevel()->findComponent<MapComponent>();
//	if (pMapComponent)
//	{
//		auto pDayNightComponent = pMapComponent->findComponent<DayNightComponent>();
//		if (pDayNightComponent)
//		{
//			pDayNightComponent->undergroundOpen(isOpen);
//		}
//	}
//
//	return true;
//}
//
//bool GmlineC::testnews()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//	int type{};
//	if (!getGmPara(type, 1)) return false;
//	int evtId{ 0 };
//	if (!getGmPara(evtId, 2)) return false;
//
//	UiTowerDefenseNews* m_pTowerDefenseNews = UiMgr::getInstance().findDialog<UiTowerDefenseNews>();
//	if (type == 1)
//	{
//		int wave{0};
//		if (!getGmPara(wave, 3)) return false;
//
//		if (m_pTowerDefenseNews)
//		{
//			m_pTowerDefenseNews->setMobWaitWave(evtId, wave);
//		}
//		else
//		{
//			m_pTowerDefenseNews = UiTowerDefenseNews::create();
//			m_pTowerDefenseNews->showNews(evtId, UiTowerDefenseNews::ActiveType::Wave);
//			m_pTowerDefenseNews->setMobWave(evtId, wave);
//		}
//	}
//	else if (type == 2)
//	{
//		if (m_pTowerDefenseNews)
//		{
//			m_pTowerDefenseNews->setMobWaitWave(evtId, 0, UiTowerDefenseNews::ActiveType::Win);
//		}
//		else {
//			m_pTowerDefenseNews = UiTowerDefenseNews::create();
//			m_pTowerDefenseNews->showNews(evtId, UiTowerDefenseNews::ActiveType::Win);
//		}
//	}
//	else if (type == 3)
//	{
//		if (m_pTowerDefenseNews)
//		{
//			m_pTowerDefenseNews->setMobWaitWave(evtId, 0, UiTowerDefenseNews::ActiveType::Win);
//		}
//		else {
//			m_pTowerDefenseNews = UiTowerDefenseNews::create();
//			m_pTowerDefenseNews->showNews(evtId, UiTowerDefenseNews::ActiveType::Win);
//		}
//	}
//	else if (type == 4)
//	{
//		m_pTowerDefenseNews = UiTowerDefenseNews::create();
//		m_pTowerDefenseNews->showNews(evtId);
//	}
//
//	return true;
//}
//
//bool GmlineC::send()
//{
//	if (m_gmline.empty()) return false;
//	int channel{};
//	if (!getGmPara(channel, 1)) return false;
//	tstring text{};
//	if (!getGmPara(text, 2)) return false;
//
//	if (channel == ChatData::Channel::World)
//	{
//		HandlerChat::getInstance().sendToWorld(text);
//	}
//	else if (channel == ChatData::Channel::System)
//	{
//		HandlerChat::getInstance().sendToSystem(text);
//	}
//	else if (channel == ChatData::Channel::Team)
//	{
//		HandlerChat::getInstance().sendToTeam(text);
//	}
//	else if (channel == ChatData::Channel::Union)
//	{
//		HandlerChat::getInstance().sendToUnion(text);
//	}
//	else if (channel == ChatData::Channel::Public)
//	{
//		HandlerChat::getInstance().sendToPublic(text);
//	}
//
//	return true;
//}
//
//bool GmlineC::testbossnews()
//{
//	int type = 1;
//	getGmPara(type, 1);
//
//	switch (type)
//	{
//	case 1:
//		UiDungeonBossNews::createBossComing(101);
//		break;
//
//	case 2:
//		UiDungeonBossNews::createBossDie(101);
//		break;
//	}
//
//	return true;
//}
//
//bool GmlineC::testwave()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(2)) return false;
//	int evtId{};
//	if (!getGmPara(evtId, 1)) return false;
//	int waveNum{};
//	if (!getGmPara(waveNum, 2)) return false;
//
//	UiTowerDefenseNews* pTowerDefenseNews = UiMgr::getInstance().findDialog<UiTowerDefenseNews>();
//	if (pTowerDefenseNews)
//	{
//		pTowerDefenseNews->setMobWave(evtId, waveNum);
//	}
//
//	return true;
//}
//
//bool GmlineC::clearBag()
//{
//	ItemPacketMgr::WeakItemDataKeyPosMap pBagList = ItemPacketMgr::getInstance().findPacketMap(MsgPB::PACKET_TYPE::BODY);
//	for (auto it : pBagList)
//	{
//		HandlerItem::getInstance().requestItemDel(it.second.lock()->getUuid());
//	}
//
//	return true;
//}
//
//
//bool GmlineC::turnOnBgm()
//{
//	AudioModule* pAudioModule = Framework::getInstance().findComponent<AudioModule>();
//	if (nullptr != pAudioModule)
//	{
//		pAudioModule->TurnOnBGM();
//	}
//	return true;
//}
//
//bool GmlineC::turnOffBgm()
//{
//	AudioModule* pAudioModule = Framework::getInstance().findComponent<AudioModule>();
//	if (nullptr != pAudioModule)
//	{
//		pAudioModule->TurnOffBGM();
//	}
//	return true;
//}
//
//bool GmlineC::turnOnUiBatching()
//{
//	UiRenderCache::getInstance().enableBatching(true);
//	return true;
//}
//
//bool GmlineC::turnOffUiBatching()
//{
//	UiRenderCache::getInstance().enableBatching(false);
//	return true;
//}
//
//bool GmlineC::turnOffFpsLimit()
//{
//	Framework::getInstance().SetLimitFPS(false);
//	return true;
//}
//
//bool GmlineC::parallelUpateAnimation()
//{
//	if (m_gmline.empty()) return false;
//	NS::uint enabled{};
//	if (!getGmPara(enabled, 1)) return false;
//
//	ModelModule* pModuleModule = Framework::getInstance().findComponent<ModelModule>();
//	if (nullptr != pModuleModule)
//	{
//		pModuleModule->setParallelUpdateAnimation(enabled);
//	}
//	return true;
//}
//
//bool GmlineC::turnOnFpsLimit()
//{
//	uint fps{};
//	if (!getGmPara(fps, 1)) return false;
//
//	Framework::getInstance().SetLimitFPS(true, fps);
//	return true;
//}
//
//bool GmlineC::showid()
//{
//	extern bool g_showId;
//	g_showId = true;
//
//	return true;
//}
//
//bool GmlineC::hideid()
//{
//	extern bool g_showId;
//	g_showId = false;
//
//	return true;
//}
//
//
//bool GmlineC::createNetWorkLog()
//{
//	tstring ip = EMPTY_STRING;
//	if (!getGmPara(ip, 1))
//		return false;
//	NetWorkLog::getInstance().addToFrameWork();
//	NetWorkLog::getInstance().connectLocalServer(ip, 8001);
//	return true;
//}
//
//bool GmlineC::sendTestLogToSrv()
//{
//	tstring text = EMPTY_STRING;
//	if (!getGmPara(text, 1))
//		return false;
//	LOG_EVENT_I(text);
//	return true;
//}
//
//bool GmlineC::openScoreReq()
//{
//	HandlerPlatform::getInstance().openScoreReq(HandlerPlatform::OpenScoreEnum::rechare);
//    return true;
//}
//
//bool GmlineC::openLocalPushReq()
//{
//	tstring text = EMPTY_STRING;
//	if (!getGmPara(text, 1))
//		return false;
//	int typeValue = 0;
//	if (!getGmPara(typeValue, 2))
//		return false;
//	LOCAL_PUAH_TYPE eType = LOCAL_PUAH_TYPE(typeValue);
//	HandlerPlatform::getInstance().openLocalPushReq(15, text, eType);
//	return true;
//}
//
//bool GmlineC::watchAdmobReq()
//{
//	HandlerPlatform::getInstance().showAdsMob(HandlerPlatform::Adsmob_Reward_Tpy::Adsmob_Reward_Tpy_Build);
//	return true;
//}
//
//bool GmlineC::getFriendList()
//{
//	HandlerPlatform::getInstance().getFriendList();
//	return true;
//}
//
//bool GmlineC::registerAddressBook()
//{
//	HandlerPlatform::getInstance().getRegisterAddressBookReq();
//	return true;
//}
//
//bool GmlineC::tranlan()
//{
//	tstring text = EMPTY_STRING;
//	if (!getGmPara(text, 1))
//		return false;
//	HandlerChat::getInstance().tranLanReq(text, 2);
//	return true;
//}
//
//bool GmlineC::addPlayer()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(2)) return false;
//
//	int hp = 0;
//	int mp = 0;
//	if (!this->getGmPara(hp, 1)) return false;
//	if (!this->getGmPara(mp, 2)) return false;
//
//	MsgPB::MsgPlayerAppearance* msg = new MsgPB::MsgPlayerAppearance;
//	msg->set_mapobjectid(rand());
//	msg->set_iggid(rand());
//	msg->set_hp(hp);
//	msg->set_mp(mp);
//	int x{}, y{};
//
//	if (!PlayerComponentMgr::getInstance().getSelf()) return false;
//	if (PlayerComponentMgr::getInstance().getSelf()->findComponent<PlayerComponent>())
//	{
//		auto pos = PlayerComponentMgr::getInstance().getSelf()->findComponent<PlayerComponent>()->getPlayerData()->getPos();
//		x = static_cast<int>(100 * pos.x);
//		y = static_cast<int>(100 * pos.y);
//	}
//
//	msg->set_posx(x);
//	msg->set_posy(y);
//
//	NetworkModule::getInstance().dispatchOneMessage(msg->msgtype(), msg);
//	return true;
//}
//
//
//bool GmlineC::switchskin()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//
//	int skinId = 1;
//	if(!this->getGmPara(skinId, 1)) return false;
//
//	MsgPB::MsgPlayerInfoGS2C* msg = new MsgPB::MsgPlayerInfoGS2C;
//	msg->set_skincolor(skinId);
//	NetworkModule::getInstance().dispatchOneMessage(msg->msgtype(), msg);
//
//	return true;
//}
//
//bool GmlineC::shownode()
//{
//	if (!PlayerComponentMgr::getInstance().getSelf()) return false;
//	if (m_gmline.empty() || !this->requestGmParaCount(2)) return false;
//
//	tstring nodeId;
//	bool visible = true;
//	if (!this->getGmPara(nodeId, 1)) return false;
//	if (!this->getGmPara(visible, 2)) return false;
//
//	int nNodePos = PlayerAttrCfg::getInstance().getNodePos(nodeId);
//	auto pPlayer = PlayerComponentMgr::getInstance().getSelf()->findComponent<PlayerComponent>();
//	if (pPlayer && nNodePos)
//	{
//		pPlayer->showNode(nNodePos, visible);
//	}
//
//	return true;
//}
//
//bool GmlineC::showequip()
//{
//	if (!PlayerComponentMgr::getInstance().getSelf()) return false;
//	if (m_gmline.empty() || !this->requestGmParaCount(2)) return false;
//
//	tstring attachId;
//	bool visible = true;
//	if (!this->getGmPara(attachId, 1)) return false;
//	if (!this->getGmPara(visible, 2)) return false;
//
//	int nPos = PlayerAttrCfg::getInstance().getAttachPos(attachId);
//	auto pPlayer = PlayerComponentMgr::getInstance().getSelf()->findComponent<PlayerComponent>();
//	if (pPlayer && nPos)
//	{
//		pPlayer->showEquip(nPos, visible);
//	}
//
//	return true;
//}
//
//
//bool GmlineC::gameobject()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//
//	tstring fileName;
//	if (!this->getGmPara(fileName, 1)) return false;
//	if (!PlayerComponentMgr::getInstance().getSelf()) return false;
//
//	auto pPlayer = PlayerComponentMgr::getInstance().getSelf()->findComponent<PlayerComponent>();
//	bzbee::GameWorld* pWorld = bzbee::SceneModule::getCurrentWorld();
//	MapComponent* pMapComponent = pWorld->getCurrentLevel()->findComponent<MapComponent>();
//	TransformComponent* pTransformComponent = pPlayer->findComponent<TransformComponent>();
//
//	bzbee::tstring filePath = "game_object/" + fileName + ".xml";
//
//	bzbee::GameObjectModule* pGameObjectFactory = Framework::getInstance().findComponent<bzbee::GameObjectModule>();
//	pGameObjectFactory->clear();
//	bzbee::GameObject* pGameObject = bzbee::GameObject::createGameObject(filePath);
//	if (pGameObject)
//	{
//        pMapComponent->addMapGameObject(
//                pGameObject->getPosition() + pTransformComponent->getPosition(), pGameObject);
//	}
//
//	return true;
//}
//
//bool GmlineC::stat()
//{
//    return true;
//}
//
//bool GmlineC::playSound()
//{
//	if (m_gmParaList.size() < 2) { return false; }
//
//	tstring strAudioKey;
//	if (!bzbee::StringUtil::stringToType(strAudioKey, m_gmParaList[1])) { return false; }
//
//#ifdef USE_WWISE
//	WwiseSoundMgr* pWwiseSoundMgr = Framework::getInstance().findComponent<WwiseSoundMgr>();
//	if (pWwiseSoundMgr)
//	{
//		return pWwiseSoundMgr->PostEvent(strAudioKey);
//	}
//#endif
//
//	return false;
//}
//
//bool GmlineC::soundSetState()
//{
//	if (m_gmParaList.size() < 3) { return false; }
//
//	tstring strStateGroup;
//	tstring strState;
//	if (!bzbee::StringUtil::stringToType(strStateGroup, m_gmParaList[1])) { return false; }
//	if (!bzbee::StringUtil::stringToType(strState, m_gmParaList[2])) { return false; }
//
//#ifdef USE_WWISE
//	WwiseSoundMgr* pWwiseSoundMgr = Framework::getInstance().findComponent<WwiseSoundMgr>();
//	if (pWwiseSoundMgr)
//	{
//		return pWwiseSoundMgr->SetState(strStateGroup.c_str(), strState.c_str());
//	}
//#endif
//
//	return false;
//}
//
//bool GmlineC::soundSetSwitch()
//{
//	if (m_gmParaList.size() < 3) { return false; }
//
//	tstring strSwitchGroup;
//	tstring strSwitchState;
//	if (!bzbee::StringUtil::stringToType(strSwitchGroup, m_gmParaList[1])) { return false; }
//	if (!bzbee::StringUtil::stringToType(strSwitchState, m_gmParaList[2])) { return false; }
//
//#ifdef USE_WWISE
//	WwiseSoundMgr* pWwiseSoundMgr = Framework::getInstance().findComponent<WwiseSoundMgr>();
//	if (pWwiseSoundMgr)
//	{
//		return pWwiseSoundMgr->SetSwitch(strSwitchGroup.c_str(), strSwitchState.c_str());
//	}
//#endif
//
//	return false;
//}
//
//bool GmlineC::clientSwitchMap()
//{
//    if (m_gmParaList.size() < 2) { return false; }
//
//    int mapId = 0;
//    if (!bzbee::StringUtil::stringToType(mapId, m_gmParaList[1])) { return false; }
//
//    std::string path = "map/map_all/NpcPveMap/";
//    if (m_gmParaList.size() > 2)
//    {
//        if (!bzbee::StringUtil::stringToType(path, m_gmParaList[2])) { return false; }
//    }
//
//    HandlerLogin::getInstance().clientSwitchMap(mapId, path);
//
//    return true;
//}
//
//bool GmlineC::addDummyMob()
//{
//	if (m_gmParaList.size() < 2) { return false; }
//	int defId;
//	if (!bzbee::StringUtil::stringToType(defId, m_gmParaList[1])) { return false; }
//
//	static int mapObjId{ 9999 };
//	if (HandlerMonster::getInstance().addMonster(mapObjId++, 0, defId, PlayerMgr::getInstance().getSelf()->getPos())) { return true; }
//
//	return false;
//}
//
//bool GmlineC::LUA_GM()
//{
//	std::string strParams{};
//	if (!getGmPara(strParams, 1)) return false;
//	LuaCallback("LUA_GM")(m_gmParaList);
//
//	return false;
//}
//
//bool GmlineC::addDummyPlayer()
//{
//	static int mapObjId{ 99999 };
//	auto pos = PlayerMgr::getInstance().getSelf()->getPos();
//	if (m_gmParaList.size() > 2)
//	{
//		float x;
//		if (bzbee::StringUtil::stringToType(x, m_gmParaList[1]))
//		{
//			pos.x += x;
//		}
//		float y;
//		if (bzbee::StringUtil::stringToType(y, m_gmParaList[2]))
//		{
//			pos.y += y;
//		}
//		return addOneDummyPlayer(++mapObjId, pos);
//	}
//	if (m_gmParaList.size() > 1)
//	{
//		unsigned count;
//		if (!bzbee::StringUtil::stringToType(count, m_gmParaList[1]) || count <= 0) { return false; }
//
//		bzbee::GameWorld* pWorld = bzbee::SceneModule::getCurrentWorld();
//		if (!pWorld) { return false; }
//		TargetCameraComponent* cameraComp{ pWorld->findComponent<TargetCameraComponent>() };
//		if (!cameraComp) { return false; }
//		auto&& screenSize = cameraComp->getScreenSize();
//		glm::vec3 worldPosTopLeft{ cameraComp->screenPosToWorldPos({ 0.f, 0.f + screenSize.y }, bzbee::VEC3_Z, MapCfg::FRONT_TILE_Z_NEAR()) };
//		glm::vec3 worldPosTopRight{ cameraComp->screenPosToWorldPos({ 0.f + screenSize.x, 0.f + screenSize.y }, bzbee::VEC3_Z, MapCfg::FRONT_TILE_Z_NEAR()) };
//		pos = worldPosTopLeft + glm::vec3{ .5f, -1.5f, 0.f };
//		for (unsigned i = 0; i < count; ++i)
//		{
//			addOneDummyPlayer(++mapObjId, pos);
//			pos.x += 1.f;
//			if (pos.x > worldPosTopRight.x)
//			{
//				pos.y -= 2.f;
//				pos.x = worldPosTopLeft.x + .5f;
//			}
//		}
//
//		return true;
//	}
//
//	return false;
//}
//
//bool GmlineC::addOneDummyPlayer(int mapObjId, const glm::vec3 & pos)
//{
//	if (!HandlerRole::getInstance().addOtherPlayer(mapObjId, pos)) { return false; }
//	auto&& playerGameObj = PlayerComponentMgr::getInstance().findPlayer(mapObjId);
//	if (!playerGameObj) { return false; }
//	auto&& playerComp = playerGameObj->findComponent<PlayerComponent>();
//	if (!playerComp) { return false; }
//
//    std::unordered_map<int, std::shared_ptr<ItemData>> equipments;
//    ArmorCfg::getInstance().getRandomEquipments(equipments);
//	playerComp->initAllEquipment(equipments);
//
//	return true;
//}
//
//bool GmlineC::addLocalItem()
//{
//	if (m_gmParaList.size() > 2)
//	{
//		int id{};
//		bzbee::StringUtil::stringToType(id, m_gmParaList[1]);
//		int num{};
//		bzbee::StringUtil::stringToType(num, m_gmParaList[2]);
//		LocalServer_ItemPacket::getInstance().AddItemByDataID(id, num, MsgPB::PACKET_TYPE::BODY);
//	}
//	if (m_gmParaList.size() > 1)
//	{
//		int id{};
//		bzbee::StringUtil::stringToType(id, m_gmParaList[1]);
//		LocalServer_ItemPacket::getInstance().AddItemByDataID(id, 1, MsgPB::PACKET_TYPE::BODY);
//		return true;
//	}
//	return true;
//}
//
//// 商队到来
//bool GmlineC::NpcCaravanArrival()
//{
//	int idData{};
//	bzbee::StringUtil::stringToType(idData, m_gmParaList[1]);
//	NpcLogic::getInstance().StartNpcDakEvent(NPC_DAK_EVENT_TYPE::NPC_DAK_EVENT_TYPE_SHOP, idData);
//	return true;
//}
//
//// 商队离开
//bool GmlineC::NpcCaravanLeave()
//{
//	int idData{};
//	bzbee::StringUtil::stringToType(idData, m_gmParaList[1]);
//	NpcLogic::getInstance().EndNpcDakEvent(NPC_DAK_EVENT_TYPE::NPC_DAK_EVENT_TYPE_SHOP, idData);
//	return true;
//}
//
//// 商队逗留
//bool GmlineC::NpcCaravanStay()
//{
//	int idData{};
//	bzbee::StringUtil::stringToType(idData, m_gmParaList[1]);
//	// 商队逗留
//	NpcLogic::getInstance().OnCaravanStayEvent(idData);
//	return true;
//}
//
//// 商队被打劫(杀人越货)
//bool GmlineC::RobNpcCaravan()
//{
//	int idData{};
//	bzbee::StringUtil::stringToType(idData, m_gmParaList[1]);
//	// 向商队发送 逗留 事件
//	NpcLogic::getInstance().RobCaravan(idData);
//	return true;
//}
//
//// 以物易物
//bool GmlineC::GiftExchange()
//{
//	int idData{};
//	bzbee::StringUtil::stringToType(idData, m_gmParaList[1]);
//	//  打开以物易物
//	NpcLogic::getInstance().ShowGiftExchange(idData);
//	return true;
//}
//
//// 驿站创建协助塔防Mob
//bool GmlineC::NpcDakCreateDefenseMob()
//{
//	int idData{};
//	bzbee::StringUtil::stringToType(idData, m_gmParaList[1]);
//	//  创建协助塔防Mob
//	NpcLogic::getInstance().CreateDefenseMob(idData);
//	return true;
//}
//
//// NPC大悬赏
//bool GmlineC::NpcPetFight()
//{
//	int idTask{};
//	bzbee::StringUtil::stringToType(idTask, m_gmParaList[1]);
//	//ObjIdVec npcIdList{ 20013, 20012, 20014 };
//	ObjIdVec npcIdList;
//	NpcComponentMgr::getInstance().GetAllBattleNpcDefID(npcIdList);
//	ObjIdVec petIdList;
//	const auto& mapData = PetData::getInstance().getPetDataMap();
//	for (auto pair : mapData)
//	{
//		petIdList.emplace_back(pair.first);
//	}
//	//PetComponentMgr::getInstance().GetAllPet(petIdList);
//	gNpcPetFight.GoNpcPetFight(idTask, npcIdList, petIdList);
//	return true;
//}
//
////水晶加护
//bool GmlineC::CrystalBuff()
//{
//	MsgBuildTowerCrystalBuffUnlock_C2S msg;
//
//	int32 pos_x;
//	bzbee::StringUtil::stringToType(pos_x, m_gmParaList[1]);
//	int32 pos_y;
//	bzbee::StringUtil::stringToType(pos_y, m_gmParaList[2]);
//	int32 cfg_id;
//	bzbee::StringUtil::stringToType(cfg_id, m_gmParaList[3]);
//
//	msg.set_pos_x(pos_x);
//	msg.set_pos_y(pos_y);
//	msg.set_crystal_buff_cfgid(cfg_id);
//
//	MsgMgr::getInstance().sendMessage(msg.msgtype(), &msg);
//
//	return true;
//}
//
//bool GmlineC::NpcBadgeRoomCheckUpdateCounter()
//{
//	int nUpdateCounter{1};
//	bzbee::StringUtil::stringToType(nUpdateCounter, m_gmParaList[1]);
//
//	if (NpcBadgeRoomCheck::getBadgeRoomComponent())
//	{
//		NpcBadgeRoomCheck::getBadgeRoomComponent()->SetUpdateCounter(nUpdateCounter);
//	}
//
//	return true;
//}
//
//bool GmlineC::SetHideCropInEvent()
//{
//	int nHideCrop{ 1 };
//	bzbee::StringUtil::stringToType(nHideCrop, m_gmParaList[1]);
//	if (nHideCrop != 0)
//	{
//		SystemSettingCfg::getInstance().SetHideCropInEvent(true);
//	}
//	else
//	{
//		SystemSettingCfg::getInstance().SetHideCropInEvent(false);
//	}
//
//	return true;
//}
//
//bool GmlineC::SetHideBuildingInEvent()
//{
//	int nHideCrop{ 1 };
//	bzbee::StringUtil::stringToType(nHideCrop, m_gmParaList[1]);
//	if (nHideCrop != 0)
//	{
//		SystemSettingCfg::getInstance().SetHideBuildingInEvent(true);
//	}
//	else
//	{
//		SystemSettingCfg::getInstance().SetHideBuildingInEvent(false);
//	}
//
//	return true;
//}
//
//
//bool GmlineC::BeginPatrolAttack()
//{
//	int nMapObjID{0};
//	bzbee::StringUtil::stringToType(nMapObjID, m_gmParaList[1]);
//	float fPatrolAttackValidDistance{ 100.0f };
//	bzbee::StringUtil::stringToType(fPatrolAttackValidDistance, m_gmParaList[2]);
//
//	GameObject* pGameObject = HandlerRole::getInstance().findRole(nMapObjID);
//	if (pGameObject)
//	{
//		RoleComponent* pRoleComponent = pGameObject->findComponent<RoleComponent>();
//		if (pRoleComponent)
//		{
//			pRoleComponent->BeginPatrolAttack(fPatrolAttackValidDistance);
//		}
//	}
//
//	return true;
//}
//
//bool GmlineC::EndPatrolAttack()
//{
//	int nMapObjID{ 0 };
//	bzbee::StringUtil::stringToType(nMapObjID, m_gmParaList[1]);
//
//	GameObject* pGameObject = HandlerRole::getInstance().findRole(nMapObjID);
//	if (pGameObject)
//	{
//		RoleComponent* pRoleComponent = pGameObject->findComponent<RoleComponent>();
//		if (pRoleComponent)
//		{
//			pRoleComponent->EndPatrolAttack();
//		}
//	}
//
//	return true;
//}
//
////Exec Gm Script --------------------------------
//bool GmlineC::rungmscript()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//	NS::tstring scriptDir{"config/GMScript/"};
//	NS::tstring scriptFile{};
//	if (!getGmPara(scriptFile, 1)) return false;
//
//	// 如果没有后缀, 自动添加一个.
//	if (scriptFile.find(".txt") == std::string::npos)
//		scriptFile += ".txt";
//
//	// 打开文件以读取
//	std::ifstream scriptFin(scriptDir+scriptFile, std::ios::binary);
//	if (!scriptFin.is_open()) {
//		LOGE("failed to open {0}", scriptDir+scriptFile);
//		return false;
//	}
//	else {
//		NS::tstring gmline;
//		while (getline(scriptFin, gmline))
//		{
//			if (gmline.find("rungmscript") == std::string::npos) // 防死循环.
//			{
//				NS::tstring cleanGM;
//				StringUtil::trim(cleanGM, gmline);
//
//				// 跳过注释
//				if (cleanGM[0] == '#')
//				{
//					continue;
//				}
//				// 执行GM
//				if(cleanGM[0] == '@')
//				{
//					Gmline::getInstance().run(cleanGM)
//						? LOGI("run gmline: {0}", cleanGM)
//						: LOGE("run gmline: {0}", cleanGM);
//				}
//			}
//		}
//	}
//
//	return true;
//}
//
//// LocalServer GM --------------------------------
//bool GmlineC::levelup()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//
//	int newLev{};
//	if (!getGmPara(newLev, 1)) return false;
//
//	auto&& newAttrCfg = PlayerAttrCfg::getInstance().findPlayerAttr(newLev);
//	auto pSelf = PlayerMgr::getInstance().getSelf();
//	if (!pSelf || !newAttrCfg)
//		return false;
//
//	// 重新计算属性
//	Player newAttr = *pSelf.get();
//	newAttr.setLevel(newLev);
//	newAttr.setPrivateLev(newLev);
//	newAttr.calcAttr();
//
//	// Send Msg
//	MsgPB::MsgPlayerInfoGS2C msg;
//	msg.set_iggid(pSelf->getIggId());
//	msg.set_lv(std::min(newLev, 100));
//	msg.set_exp(newAttr.getMaxExp());
//	msg.set_hp(static_cast<int>(newAttr.getAttr(MsgPB::ROLE_EFFECT_HPM)));
//	msg.set_mp(static_cast<int>(newAttr.getAttr(MsgPB::ROLE_EFFECT_MPM)));
//	msg.set_maxhp(static_cast<int>(newAttr.getAttr(MsgPB::ROLE_EFFECT_HPM)));
//
//	MsgMgr::getInstance().send_s2c(MsgPB::_MSG_PLAYER_INFO_GS2C, &msg);
//	gLocalServer_Achieve.OnPlayerUpLev();
//	return true;
//}
//
//bool GmlineC::sethp()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//
//	int newHp{};
//	if (!getGmPara(newHp, 1)) return false;
//
//	int nTargetMapObjectId{ -1 };
//	getGmPara(nTargetMapObjectId, 2);
//
//	bzbee::GameObject* pSelfGameObject{ nullptr };
//	NS::I64 nIggid{ 0 };
//	if (nTargetMapObjectId < 0)
//	{
//		auto pSelf = PlayerMgr::getInstance().getSelf();
//		if (!pSelf)
//			return false;
//		pSelfGameObject = PlayerComponentMgr::getInstance().getSelf();
//		nIggid = pSelf->getIggId();
//
//		if (!pSelfGameObject) return false;
//		auto pSelfRoleComp = pSelfGameObject->findComponent<RoleComponent>();
//		if (!pSelfRoleComp) return false;
//		CharacterAttributeComponent* pSelfCharacterAttributeComp = pSelfRoleComp->findComponent<CharacterAttributeComponent>();
//		if (!pSelfCharacterAttributeComp) return false;
//
//		// Send Msg
//		MsgPB::MsgPlayerInfoGS2C msg;
//		msg.set_iggid(nIggid);
//		msg.set_hp(newHp < pSelfCharacterAttributeComp->getMaxHp() ? newHp : pSelfCharacterAttributeComp->getMaxHp());
//
//		MsgMgr::getInstance().send_s2c(MsgPB::_MSG_PLAYER_INFO_GS2C, &msg);
//	}
//	else
//	{
//		pSelfGameObject = HandlerRole::getInstance().findRole(nTargetMapObjectId);
//
//		if (!pSelfGameObject) return false;
//		auto pSelfRoleComp = pSelfGameObject->findComponent<RoleComponent>();
//		if (!pSelfRoleComp) return false;
//		CharacterAttributeComponent* pSelfCharacterAttributeComp = pSelfRoleComp->findComponent<CharacterAttributeComponent>();
//		if (!pSelfCharacterAttributeComp) return false;
//		pSelfCharacterAttributeComp->PostHpMgrSetByServer(newHp, fmt::format("GmLine for test, set mapObjId = {0} hp = {1}", nTargetMapObjectId, newHp), false);
//	}
//
//
//	return true;
//}
//
//bool GmlineC::addexp()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//
//	int newExp{};
//	if (!getGmPara(newExp, 1)) return false;
//
//	auto pSelf = PlayerMgr::getInstance().getSelf();
//	if (!pSelf)
//		return false;
//
//	LocalServer_Battle::getInstance().addExp(newExp);
//	GameLog::getInstance().addExp(newExp, GameLog::E_EXP_ADD_GM);
//	return true;
//}
//
//bool GmlineC::addcash()
//{
//	return addMoney(GlobalCfg::CASH_ITEM_ID);
//}
//
//bool GmlineC::addcoin()
//{
//	return addMoney(GlobalCfg::COIN_ITEM_ID);
//}
//
//bool GmlineC::addticket()
//{
//	return addMoney(GlobalCfg::TICKET_ITEM_ID);
//}
//
//bool GmlineC::addMoney(int nId)
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//
//	int newVal{};
//	if (!getGmPara(newVal, 1)) return false;
//
//	auto pSelf = PlayerMgr::getInstance().getSelf();
//	if (!pSelf)
//		return false;
//
//	if(nId == GlobalCfg::COIN_ITEM_ID)
//		newVal += pSelf->getMoney(GlobalCfg::COIN_ITEM_ID);
//	else if(nId == GlobalCfg::TICKET_ITEM_ID)
//		newVal += pSelf->getMoney(GlobalCfg::TICKET_ITEM_ID);
//	else if(nId == GlobalCfg::CASH_ITEM_ID)
//		newVal += pSelf->getMoney(GlobalCfg::CASH_ITEM_ID);
//
//	// Send Msg
//	MsgPB::MsgItemCurrencySyncS2C msg;
//	auto&& add = msg.add_data();
//	add->set_uuid(nId);
//	add->set_value(newVal);
//
//	MsgMgr::getInstance().send_s2c(MsgPB::_MSG_ITEM_CURRENCY_SYNC_S2C, &msg);
//	return true;
//}
//
//bool GmlineC::addmob()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//
//	int mobId{};
//	if (!getGmPara(mobId, 1)) return false;
//
//	auto pSelf = PlayerMgr::getInstance().getSelf();
//	if (!pSelf)
//		return false;
//
//	std::shared_ptr<LocalServer_BirthMob::StBirthParm> parm = std::make_shared<LocalServer_BirthMob::StBirthParm>();
//	parm->nMobDefID = mobId;
//	parm->birthWorldPos = pSelf->getPos();
//	LocalServer_BirthMob::getInstance().birthMob(parm);
//
//	return true;
//}
//
//bool GmlineC::fastNpcBadge()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//
//	int badgeDefId{};
//	if (!getGmPara(badgeDefId, 1)) return false;
//
//	NpcBadgeAutoPlaceComponent::getNpcBadgeAutoPlaceComponent()->setFastNpcBadge(badgeDefId);
//
//	return true;
//}
//
//bool GmlineC::AddTowerPVPMob()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//
//	int mobId{};
//	if (!getGmPara(mobId, 1)) return false;
//
//	auto pSelf = PlayerMgr::getInstance().getSelf();
//	if (!pSelf)
//		return false;
//
//	LocalServer_BirthMob::getInstance().birthSoulMob(mobId, 1, pSelf->getPos(), Role::GroupType::Player, -1);
//
//	return true;
//}
//
//// 停止自动生怪
//bool GmlineC::DisableAutobirthMob()
//{
//	LocalServer_BirthMob::getInstance().SetDisableAutoBirthMob(true);
//	return true;
//}
//
//// 开启自动生怪
//bool GmlineC::EnableAutobirthMob()
//{
//	LocalServer_BirthMob::getInstance().SetDisableAutoBirthMob(false);
//	return true;
//}
//
//// 清空所有怪
//bool GmlineC::ClearAllMob()
//{
//	MonsterComponentMgr::getInstance().removeAllMonsters();
//	return true;
//}
//
//// 清空所有NPC
//bool GmlineC::ClearAllNpc()
//{
//	NpcComponentMgr::getInstance().removeAllNpcs();
//	return true;
//}
//
//// 停止自动生怪 清空所有怪 清空所有NPC
//bool GmlineC::ClearMobAndNpc()
//{
//	LocalServer_BirthMob::getInstance().SetDisableAutoBirthMob(true);
//	MonsterComponentMgr::getInstance().removeAllMonsters();
//	NpcComponentMgr::getInstance().removeAllNpcs();
//	return true;
//}
//
//// 暂停AI
//bool GmlineC::SetAllAIEnable()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//
//	int nEnable{};
//	if (!getGmPara(nEnable, 1)) return false;
//	if (nEnable == 1)
//	{
//		AIManager::getInstance().setEnable(true);
//	}
//	else
//	{
//		AIManager::getInstance().setEnable(false);
//	}
//
//	return true;
//}
//
//bool GmlineC::setPlayerModel()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//
//	size_t modelIdx{};
//	if (!getGmPara(modelIdx, 1)) return false;
//
//	static const std::vector<NS::tstring> s_modelList
//	{
//		"models/players/ingame/body/man/man_naked_01_hold_01/man_naked_01_hold_01.model",
//		"models/players/ingame/body/man/man_naked_01_hold_02/man_naked_01_hold_02.model",
//		"models/players/ingame/body/man/man_naked_01_axe/man_naked_01_axe.model",
//		"models/players/ingame/body/man/man_naked_01_bow/man_naked_01_bow.model",
//		"models/players/ingame/body/man/man_naked_01_cannon/man_naked_01_cannon.model",
//		"models/players/ingame/body/man/man_naked_01_gun/man_naked_01_gun.model",
//		"models/players/ingame/body/man/man_naked_01_knife/man_naked_01_knife.model",
//		"models/players/ingame/body/man/man_naked_01_magic/man_naked_01_magic.model",
//		"models/players/ingame/body/man/man_naked_01_staff/man_naked_01_staff.model",
//		"models/players/ingame/body/man/man_naked_01_sword/man_naked_01_sword.model",
//		"models/players/ingame/body/man/man_naked_01_throw/man_naked_01_throw.model",
//	};
//
//	if (modelIdx >= s_modelList.size())
//		return false;
//
//	auto&& pSelf = PlayerMgr::getInstance().getSelf();
//	if (pSelf)
//	{
//		pSelf->setNaked3DModel(s_modelList[modelIdx]);
//		Role::EquipMap equipMap{};
//		pSelf->refreshEquip(equipMap);
//
//		auto&& pObj = PlayerComponentMgr::getInstance().getSelf();
//		if (!pObj)
//			return false;
//		auto&& pPlayerCom = pObj->findComponent<PlayerComponent>();
//		if (!pPlayerCom)
//			return false;
//
//		CharacterAttributeComponent* pCharacterAttributeComp = pPlayerCom->findComponent<CharacterAttributeComponent>();
//		assert(pCharacterAttributeComp);
//		if (pCharacterAttributeComp)
//		{
//			int nMaxHp = pCharacterAttributeComp->getMaxHp();
//			int nCalculateMaxHp = pCharacterAttributeComp->CalculateMaxHp();
//			if (nCalculateMaxHp > 0 && nMaxHp != nCalculateMaxHp)
//			{
//				nMaxHp = nCalculateMaxHp;
//				pCharacterAttributeComp->SetMaxHp(nMaxHp, "GmlineC::setPlayerModel");
//			}
//			int nHp = pCharacterAttributeComp->getRealHp();
//			if (nHp > nMaxHp)
//			{
//				pCharacterAttributeComp->SetHp(nMaxHp, "GmlineC::setPlayerModel");
//			}
//		}
//	}
//
//	return true;
//}
//
//bool GmlineC::setachieveevent()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(2)) return false;
//
//	int eventId{};
//	if (!getGmPara(eventId, 1)) return false;
//	int val{};
//	if (!getGmPara(val, 2)) return false;
//
//	auto pSelf = PlayerMgr::getInstance().getSelf();
//	if (!pSelf)
//		return false;
//
//	LocalServer_Achieve::getInstance().TriggerAchieveEvent(static_cast<ACHIEVE_EVENT_TEMPLATE_ID>(eventId), val, AchieveEventArg::EM_MODE::EM_MODE_SET, 0);
//	return true;
//}
//
//bool GmlineC::teleport()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(2)) return false;
//
//	int posX{};
//	if (!getGmPara(posX, 1)) return false;
//	int posY{};
//	if (!getGmPara(posY, 2)) return false;
//
//	auto&& pPlayerCom = PlayerComponentMgr::getInstance().getSelf()->findComponent<PlayerComponent>();
//	if (pPlayerCom)
//	{
//		glm::vec3 birthWorldPos;
//		auto pMapData = MapDataMgr::getInstance().getCurrMapData();
//		pMapData->coordinateToWorldPos(birthWorldPos, glm::ivec2(posX, posY));
//		auto moveComp = pPlayerCom->findComponent<MovementComponent>();
//		birthWorldPos.z = -MapCfg::HALF_TILE_SIZE;
//		moveComp->teleport(birthWorldPos);
//	}
//
//	return true;
//}
//
//bool GmlineC::goprivatemap()
//{
//	// 弱联网转强联网转给 Server.
//	return Gmline::getInstance().run(NS::tstring("goprivatemap"));
//
//	return true;
//}
//
//bool GmlineC::addBuff()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//
//	int buffId{};
//	if (!getGmPara(buffId, 1)) return false;
//
//	int nAliveTime{ 0 };
//	getGmPara(nAliveTime, 2);
//
//	int nAddHeap{ 1 };
//	getGmPara(nAddHeap, 3);
//
//
//	auto&& self = PlayerMgr::getInstance().getSelf();
//	if (!self)
//		return false;
//
//	LocalServer_Buff::getInstance().addBuff(buffId, self->getId(), self->getId(), nAliveTime, nAddHeap);
//
//	return true;
//}
//
//bool GmlineC::addTargetBuff()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(2)) return false;
//
//	int buffId{};
//	if (!getGmPara(buffId, 1)) return false;
//	int targetId{};
//	if (!getGmPara(targetId, 2)) return false;
//
//	auto&& pTarget = findRole(targetId);
//	if (!pTarget) return false;
//
//	LocalServer_Buff::getInstance().addBuff(buffId, targetId);
//	return true;
//}
//
//
//bool GmlineC::setfood()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//
//	int newFood{};
//	if (!getGmPara(newFood, 1)) return false;
//
//	auto pSelf = PlayerMgr::getInstance().getSelf();
//	if (!pSelf)
//		return false;
//
//	int32 oldFoodReal = pSelf->getFood();
//	int32 newFoodReal = newFood * 100;
//
//	if(oldFoodReal > newFoodReal)
//		LocalServer_FoodMedicament::getInstance().DecFood(oldFoodReal - newFoodReal);
//	else
//		LocalServer_FoodMedicament::getInstance().IncFood(newFoodReal - oldFoodReal);
//
//	// Send Msg
//	/*MsgPB::MsgPlayerInfoGS2C msg;
//	msg.set_iggid(pSelf->getIggId());
//	msg.set_food(std::min(newFood, 100) * 100);
//
//	MsgMgr::getInstance().send_s2c(MsgPB::_MSG_PLAYER_INFO_GS2C, &msg);
//	*/
//	return true;
//}
//
//bool GmlineC::fullHpMp()
//{
//	auto pSelf = PlayerMgr::getInstance().getSelf();
//	if (!pSelf)
//		return false;
//	bzbee::GameObject* pSelfGameObject = PlayerComponentMgr::getInstance().getSelf();
//	if (!pSelfGameObject) return false;
//	auto pSelfRoleComp = pSelfGameObject->findComponent<RoleComponent>();
//	if (!pSelfRoleComp) return false;
//	CharacterAttributeComponent* pSelfCharacterAttributeComp = pSelfRoleComp->findComponent<CharacterAttributeComponent>();
//	if (!pSelfCharacterAttributeComp) return false;
//
//	// Send Msg
//	MsgPB::MsgPlayerInfoGS2C msg;
//	msg.set_iggid(pSelf->getIggId());
//	msg.set_hp(pSelfCharacterAttributeComp->getMaxHp());
//	msg.set_mp(pSelf->getMaxMp());
//
//	MsgMgr::getInstance().send_s2c(MsgPB::_MSG_PLAYER_INFO_GS2C, &msg);
//	return true;
//}
//
//bool GmlineC::setplantgrowtime()
//{
//	int t;
//	if (!getGmPara(t, 1))
//		return false;
//
//	auto pself = PlayerMgr::getInstance().getSelf();
//
//	int32 x = pself->getCoord().x;
//	int32 y = pself->getCoord().y;
//
//	LocalServer_BuildLogicCli::getInstance().GetPlantLogic().PlantSetGrowTime(x, y, t);
//	return true;
//}
//
//bool GmlineC::settbdebug()
//{
//	int bDebug;
//	if (!getGmPara(bDebug, 1))
//		return false;
//
//	auto pself = PlayerMgr::getInstance().getSelf();
//
//	int32 x = -1;// pself->getCoord().x;
//	int32 y = -1;// pself->getCoord().y;
//
//	if (!getGmPara(x, 2))
//		x = pself->getCoord().x;
//	if (!getGmPara(y, 3))
//		y = pself->getCoord().y;
//
//	auto pTower = LocalServer_BuildLogicCli::getInstance().GetTowerBuildLogic().GetTowerBuild(x, y, -1);
//	if(pTower)
//		pTower->SetDebug(bDebug);
//	return true;
//}
//
//bool GmlineC::showfog()
//{
//	if (m_gmline.empty()) return false;
//	NS::uint visible{};
//	if (!getGmPara(visible, 1)) return false;
//
//	auto&& pWorld = NS::SceneModule::getCurrentWorld();
//	auto&& pMapComponent = pWorld->getCurrentLevel()->findComponent<MapComponent>();
//	if (pMapComponent)
//	{
//		WarFogRenderComponent* fogRenderComp = pMapComponent->findComponent<WarFogRenderComponent>();
//		if (fogRenderComp)
//		{
//			fogRenderComp->setDiableWarFog(visible == 1 ? false : true);
//		}
//	}
//
//	return true;
//}
//
//bool GmlineC::scaleRT()
//{
//	if (m_gmline.empty()) return false;
//	float fScale{ 1.0f };
//	if (!getGmPara(fScale, 1)) return false;
//
//	auto pSceneModule = NS::Framework::getInstance().findComponent<NS::SceneModule>();
//	if (!pSceneModule) return false;
//
//	pSceneModule->resizeSceneRT(fScale);
//
//	return true;
//}
//
//bool GmlineC::uploadlogfile()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//
//	int uuid{};
//	if (!getGmPara(uuid, 1)) return false;
//	HandlerPlatform::getInstance().uploadLogFile(uuid);
//	return true;
//}
//
//bool GmlineC::createDummyPlayer()
//{
//	if (m_gmline.empty()) return false;
//	auto player = PlayerMgr::getInstance().getSelf();
//	if (!player)
//	{
//		return false;
//	}
//	int nMapObjId = 0;
//	int nTemplateId = 0;
//	return LocalServer_Dummy::getInstance().createDummy(player->getPos(), nMapObjId, nTemplateId);
//}
//
//bool GmlineC::skipNewbie()
//{
//	LocalServer_ItemPacket::getInstance().AddItemByDataID(10000, 1, MsgPB::PACKET_TYPE::BODY);
//	HandlerLogin::getInstance().setNewbieFlag((int)HandlerLogin::NewbieFlag::ForcedGuidEnd, true);
//	return true;
//}
//
//bool GmlineC::getAdventureLog()
//{
//	HandlerAdventureTask::getInstance().sendMsgAdventureLogTaskGetC2S(); // 请求冒险日志
//	return true;
//}
//
//bool GmlineC::jumpAchieve()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//
//	NS::int32 achieveUuid{};
//	if (!getGmPara(achieveUuid, 1)) return false;
//
//	auto&& scriptFun = AchieveCfg::getInstance().getScriptFunction(achieveUuid);
//	if (!scriptFun.empty())
//	{
//		LuaCallback(scriptFun.c_str())(fmt::format("{0}", achieveUuid));
//	}
//	return true;
//}
//
//bool GmlineC::fieldAdventrue()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//
//	NS::int32 id{};
//	if (!getGmPara(id, 1)) return false;
//
//	NS::Window* pWindow = UiMgr::getInstance().findDlgByClassName("UiHangup_savenpc");
//	if (!pWindow && m_vecFieldAdventrueNpcDefID.empty())
//	{
//		return false;
//	}
//
//	if (pWindow)
//	{
//		UiHangup_savenpc* pUiHangup_savenpc = dynamic_cast<UiHangup_savenpc*>(pWindow);
//		if (!pUiHangup_savenpc)
//		{
//			return false;
//		}
//		pUiHangup_savenpc->GetStationNpcDefID(m_vecFieldAdventrueNpcDefID);
//	}
//
//	if (m_vecFieldAdventrueNpcDefID.size() <= 0)
//	{
//		UiMsgBoxToast::getInstance().showText(_LC("CLI_NpcAutoIncomeExplore_GoFight_NOT_NPC"));
//		return true;
//	}
//
//	HandlerFieldAdventure::getInstance().GoFieldAdventureFight(id, m_vecFieldAdventrueNpcDefID);
//	return true;
//}
//
//bool GmlineC::openLC()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//
//	int lcType{};
//	if (!getGmPara(lcType, 1)) return false;
//
//	//com::igg::bzbee::app_sandbox::platform::network::MsgPB::MsgLocLiveChatReq msg;
//	//msg.set_type(com::igg::bzbee::app_sandbox::platform::network::MsgPB::MsgLocLiveChatReq_LiveChatType(lcType));
//	//NetworkModule::getInstance().sendLocalMessage(&msg, com::igg::bzbee::app_sandbox::platform::network::MsgPB::MSG_LIVE_CHAT_REQ);
//	HandlerPlatform::getInstance().doCustomerService(com::igg::bzbee::app_sandbox::platform::network::MsgPB::MsgLocLiveChatReq_LiveChatType(lcType));
//	//HandlerPlatform::getInstance().sendAppsFlyerEvent(HandlerPlatform::APPSFLYER_EVENT(lcType));
//	return true;
//}
//
//bool GmlineC::reqAgreementStatus()
//{
//	if (m_gmline.empty()) return false;
//	com::igg::bzbee::app_sandbox::platform::network::MsgPB::MsgAgreementStatusReq msg;
//	NetworkModule::getInstance().sendLocalMessage(&msg, com::igg::bzbee::app_sandbox::platform::network::MsgPB::MSG_LOC_AGREEMENT_STATUS_REQ);
//
//	return true;
//}
//
//bool GmlineC::reqAgreementList()
//{
//	if (m_gmline.empty()) return false;
//	com::igg::bzbee::app_sandbox::platform::network::MsgPB::MsgAgreementListReq msg;
//	NetworkModule::getInstance().sendLocalMessage(&msg, com::igg::bzbee::app_sandbox::platform::network::MsgPB::MSG_LOC_AGREEMENT_LIST_REQ);
//
//	return true;
//}
//
//bool GmlineC::agreeAgreement()
//{
//	if (m_gmline.empty()) return false;
//	com::igg::bzbee::app_sandbox::platform::network::MsgPB::MsgAgreeAgreement msg;
//	NetworkModule::getInstance().sendLocalMessage(&msg, com::igg::bzbee::app_sandbox::platform::network::MsgPB::MSG_LOC_AGREE_AGREEMENT);
//
//	return true;
//}
//
//bool GmlineC::afevent()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//
//	tstring afeventId{};
//	if (!getGmPara(afeventId, 1)) return false;
//	int tpy = 0;
//	if (!getGmPara(tpy, 2)) return false;
//	int count = 0;
//	if (!getGmPara(count, 3)) return false;
//
//	HandlerAppsFlyer::getInstance().sendAppsFlyerLog(afeventId , (HandlerAppsFlyer::AppsFlyer_LogTpy)(tpy), count);
//	//HandlerPlatform::getInstance().sendAppsFlyerEvent(HandlerPlatform::APPSFLYER_EVENT(afeventId));
//	return true;
//}
//
//// 小队PVP开始战斗
//bool GmlineC::SquadPvpGoFight()
//{
//	// 添加玩家自己的数据
//	NS::I64 myIggid = PlayerMgr::getInstance().getSelf()->getIggId();
//	auto&& gameObjPlayer = PlayerComponentMgr::getInstance().getSelf();
//	auto&& playerComp = gameObjPlayer->findComponent<PlayerComponent>();
//	auto&& pPlayerData = playerComp->getPlayerData();
//
//	//添加玩家自己的数据
//	{
//		MsgPB::MsgPvpSquadData_PlayerData* player_data = m_snapshot.mutable_player_data();
//		player_data->set_iggid(myIggid);
//		player_data->set_lev(pPlayerData->getLevel());
//		player_data->set_nickname(pPlayerData->getNickname());
//		player_data->set_gender(pPlayerData->getGender());
//		player_data->set_skincolor(pPlayerData->getPlayerModelData().m_skincolor);
//		player_data->set_hair(pPlayerData->getPlayerModelData().m_hair);
//		player_data->set_haircolor(pPlayerData->getPlayerModelData().m_haircolor);
//		auto&& mapEquip = pPlayerData->getEquipMap();
//		for (auto&& iterEquip = mapEquip.begin(); iterEquip != mapEquip.end(); ++iterEquip)
//		{
//			auto&& player_equip = player_data->add_equip();
////			int nEquipPos = iterEquip->first;
//			auto&& dataEquip = iterEquip->second;
//			player_equip->set_uuid(dataEquip->m_uuid);
//			player_equip->set_dataid(dataEquip->getItemId());
//			player_equip->set_pos((EQUIP_PACKET_POS)dataEquip->getEquipPos());
//			player_equip->set_hide(dataEquip->m_hide);
//			const AttrVec& vecEffect = dataEquip->getAttrEffectVec(AttrType::ATTR_EFFECT);
//			for (auto&& iterEffect = vecEffect.begin(); iterEffect != vecEffect.end(); ++iterEffect)
//			{
//				auto&& equip_effect = player_equip->add_effects();
//				equip_effect->set_type(iterEffect->type);
//				std::vector<AttrEffectValue> ref_values = iterEffect->ref_values;
//				for (auto&& iterEffectValue = ref_values.begin(); iterEffectValue != ref_values.end(); ++iterEffectValue)
//				{
//					auto&& equip_effect_values = equip_effect->add_values();
//					equip_effect_values->set_val((*iterEffectValue).value);
//					equip_effect_values->set_key((*iterEffectValue).attrref);
//				}
//			}
//			const AttrVec& vecEffectBase = dataEquip->getAttrEffectVec(AttrType::ATTR_BASE);
//			for (auto&& iterEffectBase = vecEffectBase.begin(); iterEffectBase != vecEffectBase.end(); ++iterEffectBase)
//			{
//				auto&& equip_effect = player_equip->add_effects();
//				equip_effect->set_type(iterEffectBase->type);
//				std::vector<AttrEffectValue> ref_values = iterEffectBase->ref_values;
//				for (auto&& iterEffectValue = ref_values.begin(); iterEffectValue != ref_values.end(); ++iterEffectValue)
//				{
//					auto&& equip_effect_values = equip_effect->add_values();
//					equip_effect_values->set_val((*iterEffectValue).value);
//					equip_effect_values->set_key((*iterEffectValue).attrref);
//				}
//			}
////			const AttrVec& vecEffectStaticval = dataEquip->getAttrEffectVec(AttrType::ATTR_STATICVAL);
//			for (auto&& iterEffectStaticval = vecEffectBase.begin(); iterEffectStaticval != vecEffectBase.end(); ++iterEffectStaticval)
//			{
//				auto&& equip_effect = player_equip->add_effects();
//				equip_effect->set_type(iterEffectStaticval->type);
//				std::vector<AttrEffectValue> ref_values = iterEffectStaticval->ref_values;
//				for (auto&& iterEffectValue = ref_values.begin(); iterEffectValue != ref_values.end(); ++iterEffectValue)
//				{
//					auto&& equip_effect_values = equip_effect->add_values();
//					equip_effect_values->set_val((*iterEffectValue).value);
//					equip_effect_values->set_key((*iterEffectValue).attrref);
//				}
//			}
//		}
//	}
//
//	//添加玩家自己的NPC数据
//	/*
//	{
//		m_snapshot.clear_npc_data();
//		auto&& mapUuid2NpcProtectorData = NpcProtectorDataMgr::getInstance()->getProtectorDataMap();
//		int nIndex = 0;
//		int nNpcCount = 3; // NPC上3个
//		if (!mapUuid2NpcProtectorData.empty())
//		{
//			for (auto&& iter = mapUuid2NpcProtectorData.begin(); iter != mapUuid2NpcProtectorData.end() && nIndex < nNpcCount; ++iter)
//			{
//				NS::I64 uuid = iter->first;
//				std::shared_ptr<NpcProtectorData> data = iter->second;
//				if (data->mapObjectID <= 0 || data->templateid <= 0)
//				{
//					continue;
//				}
//				auto&& npc_data = m_snapshot.add_npc_data();
//				npc_data->set_uuid(data->uuid);
//				npc_data->set_mapobjectid(data->mapObjectID);
//				npc_data->set_templateid(data->templateid);
//				npc_data->set_status(data->status);
//				npc_data->set_pos_x(data->pos_x);
//				npc_data->set_pos_y(data->pos_y);
//				npc_data->set_required(data->roomConditionUnmatchedFlag);
//				for (int i = 0; i < 5; ++i)
//				{
//					npc_data->add_param(data->param[i]);
//				}
//				npc_data->set_wait_time(static_cast<int>(data->wait_time));
//				npc_data->set_level(data->level);
//				npc_data->set_comfort_level(data->comfort_level);
//				npc_data->set_mapid(data->mapid);
//				npc_data->set_comfort(data->comfort);
//				for (auto&& iterQualityItem = data->vecQualityItemNum.begin(); iterQualityItem != data->vecQualityItemNum.end(); ++iterQualityItem)
//				{
//					npc_data->add_param(*iterQualityItem);
//				}
//				npc_data->set_hp(data->hp);
//				const std::map<int, int> mapAtterUseTimer = data->getAtterUseTimerMap();
//				for (auto&& iterAtterUseTimer = mapAtterUseTimer.begin(); iterAtterUseTimer != mapAtterUseTimer.end(); ++iterAtterUseTimer)
//				{
//					auto dataCount = npc_data->add_count_data();
//					dataCount->set_dataid(iterAtterUseTimer->first);
//					dataCount->set_count(iterAtterUseTimer->second);
//				}
//				const std::map<int, NS::I64> mapEquip = data->getEquipMap();
//				for (auto&& iterEquip = mapEquip.begin(); iterEquip != mapEquip.end(); ++iterEquip)
//				{
//					auto dataEquip = npc_data->add_equip_data();
//					dataEquip->set_pos((MsgPB::EQUIP_PACKET_POS)iterEquip->first);
//					dataEquip->set_item_uuid(iterEquip->second);
//				}
//
//				nIndex++;
//			}
//		}
//	}
//	*/
//
//	//添加玩家自己的Pet数据
//	/*
//	{
//		m_snapshot.clear_pet_data();
//		auto&& mapAllPets = PetData::getInstance().getPetDataMap();
//		int nIndex = 0;
//		int nPetCount = 2; // Pet上2个
//		if (!mapAllPets.empty())
//		{
//			for (auto&& iter = mapAllPets.begin(); iter != mapAllPets.end() && nIndex < nPetCount; ++iter)
//			{
//
//				NS::I64 uuid = iter->first;
//				auto&& petData = iter->second;
//				auto&& pet_data = m_snapshot.add_pet_data();
//				pet_data->set_uuid(uuid);
//				pet_data->set_dataid(petData.dataid());
//				pet_data->set_color(petData.color());
//				pet_data->set_newlookface(petData.newlookface());
//				pet_data->set_lev(petData.lev());
//				pet_data->set_activate(petData.activate());
//				pet_data->set_hp(petData.hp());
//				pet_data->set_mp(petData.mp());
//				pet_data->set_nickname(petData.nickname());
//				pet_data->clear_skillid();
//				//pet_data->set_allocated_skillid((MsgPB::SetObjID*)&(pPetData->skillid()));
//				pet_data->set_exp(petData.exp());
//				pet_data->set_maxhp(petData.maxhp());
//				pet_data->set_maxmp(petData.maxmp());
//				//pet_data->set_allocated_equipid((MsgPB::SetObj64*)&(pPetData->equipid()));
//				pet_data->set_atkstar(petData.atkstar());
//				pet_data->set_defstar(petData.defstar());
//				pet_data->set_reborntime(petData.reborntime());
//				pet_data->set_awardtime(petData.awardtime());
//				pet_data->set_dirty(petData.dirty());
//				pet_data->set_owneriggid(petData.owneriggid());
//				pet_data->clear_itemusecount();
//
//				MsgPB::PetItemUseCount* p_itemusecount = pet_data->mutable_itemusecount();
//				const ::MsgPB::PetItemUseCount& dataPetItemUseCount = petData.itemusecount();
//				int nPetItemUseCount = dataPetItemUseCount.data_size();
//				for (int i = 0; i < nPetItemUseCount; ++i)
//				{
//					MsgPB::PetItemUseCount_CountData* p_CountData = p_itemusecount->add_data();
//					const MsgPB::PetItemUseCount_CountData dataCount = dataPetItemUseCount.data(i);
//					p_CountData->set_count(dataCount.count());
//					p_CountData->set_dataid((int)dataCount.dataid());
//				}
//
//				pet_data->set_quality(petData.quality());
//				pet_data->set_reset_count(petData.reset_count());
//
//
//				nIndex++;
//			}
//		}
//	}
//	*/
//
//	SquadPvpDataMgr::getInstance().AddSquadPvpData(m_snapshot, myIggid, 999999);
//
//
//	//添加敌对玩家的数据
//	{
//		MsgPB::MsgPvpSquadData_PlayerData* enemy_player_data = m_snapshotEnemy.mutable_player_data();
//		enemy_player_data->set_iggid(999999);
//		enemy_player_data->set_lev(30);
//		enemy_player_data->set_nickname("Enemy_Player");
//		enemy_player_data->set_gender(Role::Gender::Female);	// 性别
//		// 装备数据
//		{
//			// 丛林套装 —— 30
//			// 头盔
//			auto&& player_equip_head = enemy_player_data->add_equip();
//			player_equip_head->set_uuid(1330052);
//			player_equip_head->set_dataid(1330052);
//			player_equip_head->set_pos(EQUIP_PACKET_POS::EQUIP_PACKET_HEAD);
//			player_equip_head->set_hide(false);
//			// 战甲
//			auto&& player_equip_clothes = enemy_player_data->add_equip();
//			player_equip_clothes->set_uuid(1330053);
//			player_equip_clothes->set_dataid(1330053);
//			player_equip_clothes->set_pos(EQUIP_PACKET_POS::EQUIP_PACKET_CLOTHES);
//			player_equip_clothes->set_hide(false);
//			// 战裤
//			auto&& player_equip_pants = enemy_player_data->add_equip();
//			player_equip_pants->set_uuid(1330054);
//			player_equip_pants->set_dataid(1330054);
//			player_equip_pants->set_pos(EQUIP_PACKET_POS::EQUIP_PACKET_PANTS);
//			player_equip_pants->set_hide(false);
//			// 战靴
//			auto&& player_equip_shoe = enemy_player_data->add_equip();
//			player_equip_shoe->set_uuid(1330055);
//			player_equip_shoe->set_dataid(1330055);
//			player_equip_shoe->set_pos(EQUIP_PACKET_POS::EQUIP_PACKET_SHOE);
//			player_equip_shoe->set_hide(false);
//			// 武器 —— 投掷炸弹
//			auto&& player_equip_weapon = enemy_player_data->add_equip();
//			player_equip_weapon->set_uuid(1341001);
//			player_equip_weapon->set_dataid(1341001);
//			player_equip_weapon->set_pos(EQUIP_PACKET_POS::EQUIP_PACKET_WEAPON);
//			player_equip_weapon->set_hide(false);
//		}
//	}
//
//
//	//添加敌对玩家的NPC数据
//	/*
//	{
//		m_snapshotEnemy.clear_npc_data();
//		auto&& mapUuid2NpcProtectorData = NpcProtectorDataMgr::getInstance()->getProtectorDataMap();
//		int nIndex = 0;
//		int nNpcCount = 3; // NPC上3个
//		auto&& npc_data = m_snapshotEnemy.add_npc_data();
//		npc_data->set_uuid(20087);
//		npc_data->set_mapobjectid(20087);
//		npc_data->set_templateid(20087);		//	阿努比斯的徽章
//		npc_data->set_status(MsgPB::PROTECTOR_STATUS::BADGE_STATUS_PATROL);
//// 		npc_data->set_pos_x(data->pos_x);
//// 		npc_data->set_pos_y(data->pos_y);
//		npc_data->set_required(MsgPB::PROTECTOR_LESS_THAN_REQUIRED::PROTECTOR_LESS_THAN_REQUIRED_NONE);
//// 		for (int i = 0; i < 5; ++i)
//// 		{
//// 			npc_data->add_param(data->param[i]);
//// 		}
////		npc_data->set_wait_time(data->wait_time);
//		npc_data->set_level(30);
//		npc_data->set_comfort_level(5);
////		npc_data->set_mapid(data->mapid);
////		npc_data->set_comfort(data->comfort);
//// 		for (auto&& iterQualityItem = data->vecQualityItemNum.begin(); iterQualityItem != data->vecQualityItemNum.end(); ++iterQualityItem)
//// 		{
//// 			npc_data->add_param(*iterQualityItem);
//// 		}
//		npc_data->set_hp(1000);
//// 		const std::map<int, int> mapAtterUseTimer = data->getAtterUseTimerMap();
//// 		for (auto&& iterAtterUseTimer = mapAtterUseTimer.begin(); iterAtterUseTimer != mapAtterUseTimer.end(); ++iterAtterUseTimer)
//// 		{
//// 			auto dataCount = npc_data->add_count_data();
//// 			dataCount->set_dataid(iterAtterUseTimer->first);
//// 			dataCount->set_count(iterAtterUseTimer->second);
//// 		}
//// 		const std::map<int, NS::I64> mapEquip = data->getEquipMap();
//// 		for (auto&& iterEquip = mapEquip.begin(); iterEquip != mapEquip.end(); ++iterEquip)
//// 		{
//// 			auto dataEquip = npc_data->add_equip_data();
//// 			dataEquip->set_pos((MsgPB::EQUIP_PACKET_POS)iterEquip->first);
//// 			dataEquip->set_item_uuid(iterEquip->second);
//// 		}
//
//// 		if (!mapUuid2NpcProtectorData.empty())
//// 		{
//// 			for (auto&& iter = mapUuid2NpcProtectorData.begin(); iter != mapUuid2NpcProtectorData.end() && nIndex < nNpcCount; ++iter)
//// 			{
//// 				NS::I64 uuid = iter->first;
//// 				std::shared_ptr<NpcProtectorData> data = iter->second;
//// 				if (data->mapObjectID <= 0 || data->templateid <= 0)
//// 				{
//// 					continue;
//// 				}
////
////
//// 				nIndex++;
//// 			}
//// 		}
//	}
//	*/
//
//	//添加敌对玩家的Pet数据
//	/*
//	{
//		m_snapshotEnemy.clear_pet_data();
//		// 熊猫法师
//		auto&& pet_data_01 = m_snapshotEnemy.add_pet_data();
//		pet_data_01->set_uuid(157);
//		pet_data_01->set_dataid(157);
//// 		pet_data->set_color(petData.color());
//// 		pet_data->set_newlookface(petData.newlookface());
//		pet_data_01->set_lev(30);
////		pet_data->set_activate(petData.activate());
//		pet_data_01->set_hp(1000);
//		pet_data_01->set_mp(1000);
////		pet_data->set_nickname(petData.nickname());
//		pet_data_01->clear_skillid();
//		pet_data_01->mutable_skillid()->add_objid(31007);
////		pet_data->set_exp(petData.exp());
//		pet_data_01->set_maxhp(9999);
//		pet_data_01->set_maxmp(9999);
//		//pet_data->set_allocated_equipid((MsgPB::SetObj64*)&(pPetData->equipid()));
//		pet_data_01->set_atkstar(3);
//		pet_data_01->set_defstar(3);
//// 		pet_data->set_reborntime(petData.reborntime());
//// 		pet_data->set_awardtime(petData.awardtime());
//// 		pet_data->set_dirty(petData.dirty());
//		pet_data_01->set_owneriggid(999999);
//		pet_data_01->clear_itemusecount();
//
//// 		MsgPB::PetItemUseCount* p_itemusecount = pet_data->mutable_itemusecount();
//// 		const ::MsgPB::PetItemUseCount& dataPetItemUseCount = petData.itemusecount();
//// 		int nPetItemUseCount = dataPetItemUseCount.data_size();
//// 		for (int i = 0; i < nPetItemUseCount; ++i)
//// 		{
//// 			MsgPB::PetItemUseCount_CountData* p_CountData = p_itemusecount->add_data();
//// 			const MsgPB::PetItemUseCount_CountData dataCount = dataPetItemUseCount.data(i);
//// 			p_CountData->set_count(dataCount.count());
//// 			p_CountData->set_dataid((int)dataCount.dataid());
//// 		}
////
//// 		pet_data->set_quality(petData.quality());
//// 		pet_data->set_reset_count(petData.reset_count());
//
//		// 萝卜小兔
//		auto&& pet_data_02 = m_snapshotEnemy.add_pet_data();
//		pet_data_02->set_uuid(338);
//		pet_data_02->set_dataid(338);
//		pet_data_02->set_lev(30);
//		pet_data_02->set_hp(1000);
//		pet_data_02->set_mp(1000);
//		pet_data_02->clear_skillid();
//		pet_data_02->mutable_skillid()->add_objid(74601);
//		pet_data_02->set_maxhp(9999);
//		pet_data_02->set_maxmp(9999);
//		pet_data_02->set_atkstar(3);
//		pet_data_02->set_defstar(3);
//		pet_data_02->set_owneriggid(999999);
//		pet_data_02->clear_itemusecount();
//
//	}
//	*/
//
//	SquadPvpDataMgr::getInstance().AddSquadPvpData(m_snapshotEnemy, myIggid, 999999);
//
//
//	LocalServer_SquadPvp::getInstance().GoFight(false);
//	return true;
//}
//
//// 小队PVP离开
//bool GmlineC::SquadPvpQuitMap()
//{
//	LocalServer_SquadPvp::getInstance().QuitMap();
//	return true;
//}
//
//// 小队PVP测试
//bool GmlineC::SquadPvpCheck()
//{
//	// 获取己方队伍HP
////	int nPlayerSquadHP = LocalServer_SquadPvp::getInstance().GetPlayerSquadHP();
////	int nEnemySquadHP = LocalServer_SquadPvp::getInstance().GetEnemySquadHP();
////	bool bFightOver = LocalServer_SquadPvp::getInstance().CheckFightOver();
//	LocalServer_SquadPvp::getInstance().SetAllAgentEnable(false);
//	return true;
//}
//
//// NPC委托 挂机 公共地图 进入地图
//bool GmlineC::HangupCommonEnterMap()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(0))
//	{
//		return false;
//	}
//
//	HandlerFieldAdventure::getInstance().reqExploreData();
//
//	return true;
//}
//
//// NPC委托 挂机 公共地图 退出地图
//bool GmlineC::HangupCommonQuitMap()
//{
//	HangupCommonMapMgr::getInstance().QuitMap();
//
//	return true;
//}
//
//// NPC委托 挂机 公共地图 切换挂机类型
//bool GmlineC::HangupCommonSwitchHangupType()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1))
//	{
//		return false;
//	}
//	int nHangupType{};
//	if (!getGmPara(nHangupType, 1))
//	{
//		return false;
//	}
//
//	int nBiomeType{};
//	if (!getGmPara(nBiomeType, 2))
//	{
//		return false;
//	}
//
//	HangupCommonMapMgr::getInstance().SwitchHangupType((HangupCommonMapData::HangupType)nHangupType, (HangupCommonMapData::BiomeType)nBiomeType);
//
//	return true;
//}
//
//// 为方便测试而添加供GM使用的
//bool GmlineC::SetFieldAdventureDisplayTestMonsterDefId()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1))
//	{
//		return false;
//	}
//	int nMonsterDefId{};
//	if (!getGmPara(nMonsterDefId, 1))
//	{
//		return false;
//	}
//	bzbee::LocalServer_FieldAdventureDisplay::getInstance().setTestMonsterDefId(nMonsterDefId);
//
//    return true;
//}
//
//// 为方便测试而添加供GM使用的
//bool GmlineC::SetFieldAdventureDisplayTestTaskId()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1))
//	{
//		return false;
//	}
//	int nTaskId{};
//	if (!getGmPara(nTaskId, 1))
//	{
//		return false;
//	}
//	bzbee::LocalServer_FieldAdventureDisplay::getInstance().setTestTaskId(nTaskId);
//
//    return true;
//}
//
//bool GmlineC::moveTributeLastAwardTime()
//{
//	int time;
//	if (!getGmPara(time, 1))
//	{
//		return false;
//	}
//	HandlerGoblinTribute::getInstance().moveLastAwardTime(time);
//	return true;
//}
//
//bool GmlineC::invalidAccount()
//{
//	com::igg::bzbee::app_sandbox::platform::network::MsgPB::MsgLocInvalidAccount msg;
//	NetworkModule::getInstance().sendLocalMessage(&msg, com::igg::bzbee::app_sandbox::platform::network::MsgPB::MSG_LOC_GAME_INVALID_ACCOUNT);
//	return true;
//}
//
//bool GmlineC::reloadServerCfg()
//{
//	HandlerPlatform::getInstance().reloadServerConfig();
//	return true;
//}
//
//bool GmlineC::tornadoplay()
//{
//	PlotMgr::getInstance().playPlot(1010);
//	return true;
//}
//
//bool GmlineC::togglePlainLightmap()
//{
//	bzbee::SceneModule::setUsingPlainLightMap(!bzbee::SceneModule::getUsingPlainLightMap());
//	return true;
//}
//
//bool GmlineC::clientquitmap()
//{
//	return HandlerLogin::getInstance().clientQuitMap();
//}
//
//bool GmlineC::fbevent()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//
//	int fbeventId{};
//	if (!getGmPara(fbeventId, 1)) return false;
//	tstring parValue1{};
//	getGmPara(parValue1, 2);
//	tstring parValue2{};
//	getGmPara(parValue2, 3);
//	HandlerPlatform::getInstance().sendFacebookEvent(HandlerPlatform::FACEBOOK_EVENT(fbeventId), parValue1,parValue2);
//	return true;
//}
//
//bool GmlineC::ratingGame()
//{
//	HandlerPlatform::getInstance().openScoreReq(HandlerPlatform::OpenScoreEnum::rechare);
//	return true;
//}
//
//bool GmlineC::setForbidChangeZoom()
//{
//	int nForbidChangeZoom = 0;
//	if (!getGmPara(nForbidChangeZoom, 1))
//	{
//		return false;
//	}
//	SandboxGamePlay* pSandboxGamePlay = bzbee::SceneModule::getGamePlay<SandboxGamePlay>();
//	if (!pSandboxGamePlay)
//	{
//		return false;
//	}
//	if (nForbidChangeZoom == 0)
//	{
//		pSandboxGamePlay->setForbidChangeZoom(false);
//	}
//	else
//	{
//		pSandboxGamePlay->setForbidChangeZoom(true);
//	}
//	return true;
//}
//
//bool GmlineC::setmp()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//
//	int newMp{};
//	if (!getGmPara(newMp, 1)) return false;
//
//	auto pSelf = PlayerMgr::getInstance().getSelf();
//	if (!pSelf)
//		return false;
//
//	// Send Msg
//	MsgPB::MsgPlayerInfoGS2C msg;
//	msg.set_iggid(pSelf->getIggId());
//	msg.set_mp(newMp < pSelf->getMaxMp() ? newMp : pSelf->getMaxMp());
//
//	MsgMgr::getInstance().send_s2c(MsgPB::_MSG_PLAYER_INFO_GS2C, &msg);
//	return true;
//}
//
//bool GmlineC::survivaltrigger()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//
//	int nEvent{};
//	if (!getGmPara(nEvent, 1)) return false;
//
//	gLocalServer_Survival.TriggerEventByActivet(nEvent, false);
//
//	return true;
//}
//
//bool GmlineC::showAirBrick()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//
//	bool bIsShow{};
//	if (!getGmPara(bIsShow, 1)) return false;
//
//	auto&& pWorld = NS::SceneModule::getCurrentWorld();
//	auto&& pMapComponent = pWorld->getCurrentLevel()->findComponent<MapComponent>();
//	if (pMapComponent)
//	{
//		auto&& pMapRenderComponent = pMapComponent->findComponent<MapRenderComponent>();
//		if (pMapRenderComponent)
//		{
//			auto&& pAirBrickRenderComponent = pMapRenderComponent->findComponent<AirBrickRenderComponent>();
//			if (pAirBrickRenderComponent)
//			{
//				pAirBrickRenderComponent->setVisible(bIsShow);
//			}
//		}
//	}
//
//
//	return true;
//}
//
//bool GmlineC::changeShipGunType()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//
//	int eGunType{};
//	if (!getGmPara(eGunType, 1)) return false;
//
//	auto&& pShipComponent = ShipComponent::getShipComponent();
//	if (pShipComponent)
//	{
//		//pShipComponent->setShipGunType(eGunType);
//	}
//
//	return true;
//}
//
//bool GmlineC::setShipPosition()
//{
//    if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//
//    float fPosX, fPosY;
//    if (!getGmPara(fPosX, 1)) return false;
//    if (!getGmPara(fPosY, 2)) return false;
//
//    glm::vec3 position{ fPosX, fPosY, 0 };
//
//    auto&& pShipComponent = ShipComponent::getShipComponent();
//    if (pShipComponent)
//    {
//        pShipComponent->setPosition(position);
//    }
//
//    return true;
//}
//
//bool GmlineC::setCameraPos()
//{
//	bzbee::GameWorld* pWorld = bzbee::SceneModule::getCurrentWorld();
//	NULL_RETURN_V(pWorld, false);
//
//	TargetCameraComponent* pCameraComponent = pWorld->findComponent<TargetCameraComponent>();
//	NULL_RETURN_V(pCameraComponent, false);
//
//	int nIndex = 0;
//	glm::vec3 vec3CameraPos;
//	if (!bzbee::StringUtil::stringToType(vec3CameraPos.x, m_gmParaList[++nIndex])) { return false; }
//	if (!bzbee::StringUtil::stringToType(vec3CameraPos.y, m_gmParaList[++nIndex])) { return false; }
//	if (!bzbee::StringUtil::stringToType(vec3CameraPos.z, m_gmParaList[++nIndex])) { return false; }
//
//	glm::vec3 vec3CameraLookAtPos;
//	if (!bzbee::StringUtil::stringToType(vec3CameraLookAtPos.x, m_gmParaList[++nIndex])) { return false; }
//	if (!bzbee::StringUtil::stringToType(vec3CameraLookAtPos.y, m_gmParaList[++nIndex])) { return false; }
//	if (!bzbee::StringUtil::stringToType(vec3CameraLookAtPos.z, m_gmParaList[++nIndex])) { return false; }
//
//	pCameraComponent->setCameraParam(vec3CameraPos, vec3CameraLookAtPos);
//
//	return true;
//}
//
//bool GmlineC::setCameraParam()
//{
//    bzbee::GameWorld* pWorld = bzbee::SceneModule::getCurrentWorld();
//    NULL_RETURN_V(pWorld, false);
//
//    TargetCameraComponent* pCameraComponent = pWorld->findComponent<TargetCameraComponent>();
//    NULL_RETURN_V(pCameraComponent, false);
//
//    int nIndex = 0;
//
//    glm::vec3 targetPos;
//    if (!bzbee::StringUtil::stringToType(targetPos.x, m_gmParaList[++nIndex])) { return false; }
//    if (!bzbee::StringUtil::stringToType(targetPos.y, m_gmParaList[++nIndex])) { return false; }
//    if (!bzbee::StringUtil::stringToType(targetPos.z, m_gmParaList[++nIndex])) { return false; }
//	pCameraComponent->setTargetCameraPos(targetPos);
//
//    glm::vec3 rotateAngle;
//    if (!bzbee::StringUtil::stringToType(rotateAngle.x, m_gmParaList[++nIndex])) { return false; }
//    if (!bzbee::StringUtil::stringToType(rotateAngle.y, m_gmParaList[++nIndex])) { return false; }
//    if (!bzbee::StringUtil::stringToType(rotateAngle.z, m_gmParaList[++nIndex])) { return false; }
//    pCameraComponent->setRotateAngle(rotateAngle);
//
//    float zoom = 0.0f;
//    if (!bzbee::StringUtil::stringToType(zoom, m_gmParaList[++nIndex])) { return false; }
//    pCameraComponent->setZoom(zoom);
//
//    return true;
//}
//
//bool GmlineC::changeShipCrystalStatus()
//{
//	if (m_gmline.empty() || !this->requestGmParaCount(1)) return false;
//
//	int status{};
//	if (!getGmPara(status, 1)) return false;
//
//	auto&& towerBuildings = StaticObjMgr::getInstance().getCoordId2TowerBuildings();
//	for (auto&& pir : towerBuildings)
//	{
//		auto&& pTowerBuildingData = pir.second.lock();
//		if (pTowerBuildingData && pTowerBuildingData->getTypeId() == 1484801)
//		{
//			auto&& pGameObject = BuildingMgr::getInstance().getBuilding(pir.first);
//			if (pGameObject)
//			{
//				auto&& pAnimController = pGameObject->findComponent<NS::AnimController>();
//				auto&& pAttachController = pGameObject->findComponent<NS::ModelRenderAttachmentStateController>();
//				if (pAnimController && pAttachController)
//				{
//					if (status == 0)
//					{
//						pAnimController->setCurrState("power_off");
//						pAttachController->setState("power_off");
//					}
//					else if (status == 1)
//					{
//						pAnimController->setCurrState("activate");
//						pAttachController->setState("idle");
//					}
//					else if (status == 2)
//					{
//						pAnimController->setCurrState("die");
//						pAttachController->setState("die");
//					}
//					else if (status == 3)
//					{
//						pAnimController->setCurrState("resurgenceToIdle");
//						pAttachController->setState("idle");
//					}
//					else if (status == 4)
//					{
//						pAnimController->setCurrState("hurt");
//					}
//					else if (status == 5)
//					{
//						pAnimController->setCurrState("overload");
//						pAttachController->setState("overload");
//					}
//				}
//			}
//		}
//	}
//
//	return true;
//}
//
//bool GmlineC::uploadLogs()
//{
//	if (m_gmParaList.size() < 2) { return false; }
////	NS::I64 uuid = atoi(m_gmParaList[m_gmParaList.size() - 1].c_str());
//	//LogModule::getInstance().UploadLog(uuid);
//	return true;
//}
//
//bool GmlineC::cancelUploadLog()
//{
//	//LogModule::getInstance().BreakUpload();
//	return true;
//}
//
////---------------------------------------------------------------------------------------------------
//bool GmlineC::iggQueryServerInfo()
//{
//	MsgPB::MsgQueryProxyServer msg1;
//	MsgMgr::getInstance().sendMessage(_MSG_QUERY_PROXY_SERVER, &msg1);
//	MsgPB::MsgQueryGameServer msg2;
//	MsgMgr::getInstance().sendMessage(_MSG_QUERY_GAME_SERVER, &msg2);
//	MsgPB::MsgQueryMapServer msg3;
//	MsgMgr::getInstance().sendMessage(_MSG_QUERY_MAP_SERVER, &msg3);
//
//	return true;
//}
//
//---------------------------------------------------------------------------------------------------
Gmline& Gmline::getInstance()
{
	static Gmline s_gmline;
	return s_gmline;
}


bool Gmline::run(const tstring& gmline)
{
	//if (Config::DEFAULT_ENVIRONMENT == Config::ENV_PRODUCTION) return true;

	bool bReresut = false;
	NS::tstring lowerGmline;
	NS::tstring trimGmline;
	// 转为为小写.;
	StringUtil::tolower(lowerGmline, gmline);
	// 除去两端的空格.;
	StringUtil::trim(trimGmline, lowerGmline);

	tstring gmName = trimGmline.substr(0, trimGmline.find_first_of(" "));

	switch (GmCfg::getInstance().getGmType(gmName))
	{
	case GmCfg::GmType::ClientGm:
		bReresut = m_gmlineC.run(trimGmline);
		break;
	case GmCfg::GmType::ServerGm:
	default:
		bReresut = m_gmlineS.run(trimGmline);
		break;
	}

	return bReresut;
}

// Lua 脚本执行.
void Gmline::runLua(const NS::tstring& lua)
{
	if (!ScriptVM::getState())
		return;

	// 除去两端的空格.
	NS::tstring trimLua;
	StringUtil::trim(trimLua, lua);
	if (trimLua.empty())
		return;

	//LOGI("runLua: {0}", trimLua.substr(1));
	return ScriptVM::getInstance().execString(trimLua.substr(1));
}

// 服务器 普通查询.
bool Gmline::runQuery(const NS::tstring& text)
{
	bool bReresut = false;
	NS::tstring lowerQuery;
	NS::tstring trimQuery;
	// 转为为小写.;
	StringUtil::tolower(lowerQuery, text);
	// 除去两端的空格.;
	StringUtil::trim(trimQuery, lowerQuery);

	bReresut = m_gmlineC.runQuery(trimQuery);

	return bReresut;
}
//
////////////////////////////////////////////////////////////////////////////
//bool GmlineC::NotifyNpcDakEvent()
//{
//	int nCountry = 0;
//	if (!getGmPara(nCountry, 1))
//		return false;
//	int nEvent = 0;
//	if (!getGmPara(nEvent, 2))
//		return false;
//	int nNotifyType = 0;
//	if (!getGmPara(nNotifyType, 3))
//		return false;
//
//	gLocalServer_NpcDak.GM_NotifyEvent(nCountry, nEvent, nNotifyType);
//
//	return true;
//}
//
//bool GmlineC::showUpgradeNote()
//{
//	ComponentPtr<UiUpdateNote> pDialog = UiUpdateNote::create();
//	pDialog->setForce(true);
//	pDialog->setUrl("" , "");
//	UiMgr::getInstance().showDialog(pDialog);
//	return true;
//}

///////////////////////////////////////////////////////////////
//Lua

void LUA(Gmline_Run)(tstring strGmOrder)
{
	Gmline::getInstance().run(strGmOrder);
}



