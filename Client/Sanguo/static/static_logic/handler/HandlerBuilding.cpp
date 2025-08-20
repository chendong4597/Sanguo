/*!
 * \file HandlerBuilding.cpp
 *
 * \author xiaobowang;
 * \date 2017/3/17
 *
 */
#include "stdafx.h"
#include "HandlerBuilding.h"
#include <msg/MsgType.pb.h>
#include "../common/MsgMgr.h"
//#include <SceneModule.h>
//#include "../player/PlayerComponent.h"
//#include "../map/MapComponent.h"
//#include "../common/PlaceComponent.h"
//#include "../player/PlayerComponentMgr.h"
//#include "../common/DigComponent.h"
#include <data/MapData.h>
//#include "../map/building/BuildingMgr.h"
//#include "data/DropData.h"
//#include "HandlerDrop.h"
//#include <msg/MsgBuild.pb.h>
//#include "msg/MsgPacketDrop.pb.h"
//#include "ui/npc/UiMsgBoxNpcRoomCondition.h"
//#include "ui/UiMgr.h"
//#include "ui/common/UiMsgBoxToast.h"
//#include "data/DigData.h"
//#include <set>
//#include "map/building/BuildingBehavior.h"
//#include "map/building/BuildingFlashBehavior.h"
//#include "utils/MapDataUtil.h"
//#include "map/building/BuildingRoleComponent.h"
//#include "HandlerFeed.h"
//#include "config/MapSettingCfg.h"
//#include "map/building/InteractiveBuildingBehavior.h"
//#include "ui/npc/UiNpcBadgePlaceTip.h"
//#include <data/ItemData.h>
//#include "HandlerLogin.h"
////#include "map/effects/RoomEffectComponent.h"
//#include "map/building/RoomTipComponent.h"
//#include "config/BuildRoomCfg.h"
//#include "config/TowerBuildingCfg.h"
//#include "map/building/NpcBadgeRoomCheck.h"
//#include "localserver/LocalServer_Achieve.h"
//#include "GameLog.h"
//#include "config/FeedAnimalCfg.h"
//#include "localserver/LocalServer_BirthMob.h"
//#include "cli_build/BuildLogicCli.h"
//#include "item/ItemDropMgr.h"
//#include "map/building/NpcBadgeAutoPlaceComponent.h"
//#include "common/TaskEventComponent.h"
//#include "data/NpcProtectorData.h"
//#include "localserver/LocalServer_Npc.h"
#include "msg/MsgBuildTower.pb.h"
//#include "HandlerSurvivalTower.h"
//#include "localserver/LocalServer_ItemPacket.h"
//#include "HandlerLoginUtils.h"
#include "HandlerItem.h"

using namespace NS;

static HandlerBuilding* g_HandlerBuilding = nullptr;
HandlerBuilding& HandlerBuilding::getInstance()
{
    assert(g_HandlerBuilding);
    return *g_HandlerBuilding;
}

HandlerBuilding::HandlerBuilding()
{
    assert(!g_HandlerBuilding);
    g_HandlerBuilding = this;
    reset();
}

HandlerBuilding::~HandlerBuilding()
{
    g_HandlerBuilding = nullptr;
}

bool HandlerBuilding::initialize()
{
	//REGISTER_S2C(onMsgDig);
	//REGISTER_S2C(onMsgPlace);
	REGISTER_S2C(onMsgBuildReplace);
	REGISTER_S2C(onMsgBuildingInfo);
	//REGISTER_S2C(onMsgDead);
	//REGISTER_S2C(onMsgBuildRoomInfo);
	//REGISTER_S2C(onMsgBuildRoomAddNTF_S2C);
	//REGISTER_S2C(onMsgBuildRoomDelNTF_S2C);
	//REGISTER_S2C(onMsgBuildRoomError);

	//REGISTER_S2C(onMsgValuableBuildPutS2C);
	//REGISTER_S2C(onMsgValuableBuildDigS2C);

	//REGISTER_S2C(onMsgValuableBuildGetS2C);
	//REGISTER_S2C(onMsgValuableBuildExceptDigS2C);


	//REGISTER_S2C(onMsgBuildTowerAddTowerData_S2C);
	//REGISTER_S2C(onMsgBuildTowerDelTowerData_S2C);

	// 地图切换完毕 监听;
	//REG_EVENT(&HandlerLogin::getInstance())[this](const Event_MapLoaded& evt)
	//{
	//	if (!MapDataMgr::getInstance().isValuableBuildingMsg())
	//	{
	//		StaticObjMgr::getInstance().clearValuableBuildings();
	//		return;
	//	}

	//	requestValuableBuildGet();
	//};

    return true;
}

void HandlerBuilding::terminate()
{
	//UNREG_S2C(MsgPB::MsgBuildingRemoveMult);
	//UNREG_S2C(MsgPB::MsgBuildPlaceMult);
	UNREG_S2C(MsgPB::MsgBuildReplace_MS2C);
	//UNREG_S2C(MsgPB::MsgPrivateDropBuildDisappearS2C);
	//UNREG_S2C(MsgPB::MsgBuildRoomInfo_S2C);
	//UNREG_S2C(MsgPB::MsgBuildPutBadgeCondition_S2C);

	//UNREG_S2C(MsgPB::MsgBuildTowerAddTowerData_S2C);
	//UNREG_S2C(MsgPB::MsgBuildTowerDelTowerData_S2C);

    reset();
}

bool HandlerBuilding::reset()
{
    return true;
}

//bool HandlerBuilding::requestDig(const glm::ivec2& globalCoord, NS::I64 digId)
//{
//	if (!HandlerLogin::getInstance().isConnect())
//	{
//		UiMsgBoxToast::getInstance().showText(_LC("CLI_Common-network is diconnect."));
//		return false;
//	}
//
//	MsgPB::MsgBuildingRemoveMultReq msg;
//	msg.set_pos_x(globalCoord.x);
//	msg.set_pos_y(globalCoord.y);
//	msg.set_dig_id(digId);
//	return MsgMgr::getInstance().sendMessage(MsgPB::_MSG_BUILDING_REMOVE_MULT_REQ, &msg);
//}
//
//bool HandlerBuilding::requestValuableBuildDig(NS::I64 uuid, NS::I64 digId)
//{
//	auto&& pPlayerGameObj = PlayerComponentMgr::getInstance().getSelf();
//	if (!pPlayerGameObj)
//	{
//		return false;
//	}
//
//	auto&& pPlayerComponent = pPlayerGameObj->findComponent<PlayerComponent>();
//	if (!pPlayerComponent)
//	{
//		return false;
//	}
//
//	pPlayerComponent->cancelAllDigCmd();
//
//	if (!MsgMgr::getInstance().getClientMode() && !HandlerLogin::getInstance().isConnect())
//	{
//		UiMsgBoxToast::getInstance().showText(_LC("CLI_Common-network is diconnect."));
//		return false;
//	}
//
//	const auto&& pValuableBuilding = StaticObjMgr::getInstance().findValuableBuilding(uuid);
//	if (!pValuableBuilding)
//	{
//		return false;
//	}
//
//	auto&& pCfgBuildingType = BuildingCfg::getInstance().findBuildingTypeByTemplateId(pValuableBuilding->m_buildingTemplateId);
//	if (!pCfgBuildingType)
//	{
//		return false;
//	}
//
//	if (!ItemPacketMgr::getInstance().isCanGetInDrop(pCfgBuildingType->m_typeId, 1, HandlerItem::getInstance().getCurrPacketType()))
//	{
//		return false;
//	}
//
//	MsgPB::MsgValuableBuildDigC2S msg;
//	msg.set_uuid(uuid);
//	msg.set_dig_id(digId);
//	return MsgMgr::getInstance().sendMessage(MsgPB::_MSG_VALUABLE_BUILD_DIG_C2S, &msg);
//}

bool HandlerBuilding::requestPlace(const glm::ivec2& globalCoord, uint templateId, NS::I64 uuid)
{
	//if (!HandlerLogin::getInstance().isConnect())
	//{
	//	UiMsgBoxToast::getInstance().showText(_LC("CLI_Common-network is diconnect."));
	//	return false;
	//}

	//auto buildingType = BuildingCfg::getInstance().findBuildingTypeByTemplateId(templateId);
	//if (!buildingType)
	//{
	//	return false;
	//}

	//int nMaxPlace = 0;
	//if (GlobalCfg::getInstance().isBuildingPlaceLimit(buildingType->m_typeId, nMaxPlace))
	//{
	//	uint32 buildingCount = 0;
	//	auto&& buildings =  StaticObjMgr::getInstance().getCoordId2InteractiveBuildings();
	//	for (auto it: buildings)
	//	{
	//		std::weak_ptr<InteractiveBuilding> building = it.second;
	//		if (building.lock())
	//		{
	//			auto tmpBuildingType = building.lock()->getType();
	//			if (buildingType->m_typeId == tmpBuildingType->m_typeId)
	//			{
	//				buildingCount++;
	//			}
	//		}
	//	}

	//	if (buildingCount >= nMaxPlace)
	//	{
	//		UiMsgBoxToast::getInstance().showText(_LC("CLI_PUT_BUILDING_LIMIT_MAX"), GlobalCfg::getInstance().getMineralDungeonTipTime());
	//		return false;
	//	}
	//}

	////贵重物品放置
	//if (BuildingMgr::getInstance().isValuableBuildingMsg(templateId))
	//{
	//	return requestValuableBuildPlace(globalCoord, templateId, uuid);
	//}
	//else if (BuildingMgr::getInstance().isTowerBuildingMsg(templateId))
	//{
	//	return requestTowerBuildPlace(globalCoord, templateId, uuid);
	//}
	//else
	//{
		MsgPB::MsgBuildPlaceSingle msg;
		msg.set_pos_x(globalCoord.x);
		msg.set_pos_y(globalCoord.y);
		msg.set_templateid(templateId);
		msg.set_uuid(uuid);
		return MsgMgr::getInstance().sendMessageDirect(MsgPB::_MSG_BUILD_PLACE_SINGLE, &msg);
	//}
}

/*
bool HandlerBuilding::requestValuableBuildPlace(const glm::ivec2& globalCoord, bzbee::uint templateId, NS::I64 uuid)
{
	auto&& pPlayerGameObj = PlayerComponentMgr::getInstance().getSelf();
	if (!pPlayerGameObj)
	{
		return false;
	}

	auto&& pPlayerComponent = pPlayerGameObj->findComponent<PlayerComponent>();
	if (!pPlayerComponent)
	{
		return false;
	}

	pPlayerComponent->cancelAllPlaceCmd();

	MsgPB::MsgValuableBuildPutC2S msg;
	msg.set_pos_x(globalCoord.x);
	msg.set_pos_y(globalCoord.y);
	msg.set_item_uuid(uuid);
	msg.set_build_id(templateId);
	return MsgMgr::getInstance().sendMessageDirect(MsgPB::_MSG_VALUABLE_BUILD_PUT_C2S, &msg);
}

bool HandlerBuilding::requestTowerBuildDig(NS::I64 uuid, NS::I64 digId)
{
	auto&& pPlayerGameObj = PlayerComponentMgr::getInstance().getSelf();
	if (!pPlayerGameObj)
	{
		return false;
	}

	auto&& pPlayerComponent = pPlayerGameObj->findComponent<PlayerComponent>();
	if (!pPlayerComponent)
	{
		return false;
	}

	pPlayerComponent->cancelAllDigCmd();

	if (!MsgMgr::getInstance().getClientMode() && !HandlerLogin::getInstance().isConnect())
	{
		UiMsgBoxToast::getInstance().showText(_LC("CLI_Common-network is diconnect."));
		return false;
	}

	const auto&& pTowerBuildingPlaceInfo = StaticObjMgr::getInstance().findTowerBuildingPlaceInfo(uuid);
	if (!pTowerBuildingPlaceInfo)
	{
		return false;
	}

	auto&& pMapData = MapDataMgr::getInstance().getCurrMapData();
	if (!pMapData)
	{
		return false;
	}

	auto&& pCfgBuildingType = BuildingCfg::getInstance().findBuildingTypeByTemplateId(pMapData->getBuildingTile(pTowerBuildingPlaceInfo->m_coord));
	if (!pCfgBuildingType)
	{
		return false;
	}

	if (!ItemPacketMgr::getInstance().isCanGetInDrop(pCfgBuildingType->m_typeId, 1, HandlerItem::getInstance().getCurrPacketType()))
	{
		return false;
	}

	MsgPB::MsgBuildTowerDelTowerData_C2S msg;
	msg.set_uuid(uuid);
	msg.set_digid(digId);
	return MsgMgr::getInstance().sendMessage(MsgPB::_MSG_BUILDTOWER_DEL_TOWERDATA_C2S, &msg);
}

bool HandlerBuilding::requestTowerBuildPlace(const glm::ivec2& globalCoord, bzbee::uint templateId, NS::I64 uuid)
{
	auto&& pPlayerGameObj = PlayerComponentMgr::getInstance().getSelf();
	if (!pPlayerGameObj)
	{
		return false;
	}

	auto&& pPlayerComponent = pPlayerGameObj->findComponent<PlayerComponent>();
	if (!pPlayerComponent)
	{
		return false;
	}

	pPlayerComponent->cancelAllPlaceCmd();

	MsgPB::MsgBuildTowerAddTowerData_C2S msg;
	msg.set_item_uuid(uuid);
	msg.set_pos_x(globalCoord.x);
	msg.set_pos_y(globalCoord.y);
	msg.set_tower_templateid(templateId);
	return MsgMgr::getInstance().sendMessage(MsgPB::_MSG_BUILDTOWER_ADD_TOWERDATA_C2S, &msg);
}

//切换地图时获取，
bool HandlerBuilding::requestValuableBuildGet()
{
	MsgPB::MsgValuableBuildGetC2S msg;
	return MsgMgr::getInstance().sendMessage(MsgPB::_MSG_VALUABLE_BUILD_GET_C2S, &msg);
}

//放置贵重家具异常，应当退回
bool HandlerBuilding::requestValuableBuildExcept(NS::I64 uuid)
{
	MsgPB::MsgValuableBuildExceptDigC2S msg;
	msg.set_uuid(uuid);
	return MsgMgr::getInstance().sendMessage(MsgPB::_MSG_VALUABLE_BUILD_EXCEPT_DIG_C2S, &msg);
}

void HandlerBuilding::showPlaceText(::MsgPB::BUILDING_RESULT res)
{
	switch (res)
	{
	case MsgPB::BUILDING_RESULT_ERROR:
		UiMsgBoxToast::getInstance().showText(_LC("SRV_MsgBuildPlaceMult-BUILDING_RESULT_ERROR"), "");
		return;
	case MsgPB::BUILDING_RESULT_ERROR_BLOCK:
		UiMsgBoxToast::getInstance().showText(_LC("SRV_MsgBuildPlaceMult-BUILDING_RESULT_ERROR_BLOCK"), "");
		return;
	case MsgPB::BUILDING_RESULT_ERROR_GRID:
		UiMsgBoxToast::getInstance().showText(_LC("SRV_MsgBuildPlaceMult-BUILDING_RESULT_ERROR_GRID"), "");
		return;
	case MsgPB::BUILDING_RESULT_ERROR_TEMPLATE:
		UiMsgBoxToast::getInstance().showText(_LC("SRV_MsgBuildPlaceMult-BUILDING_RESULT_ERROR_TEMPLATE"), "");
		return;
	case MsgPB::BUILDING_RESULT_ERROR_NO_BUILDING:
		UiMsgBoxToast::getInstance().showText(_LC("SRV_MsgBuildPlaceMult-BUILDING_RESULT_ERROR_NO_BUILDING"), "");
		return;
	case MsgPB::BUILDING_RESULT_ERROR_NO_RESOURCE:
		UiMsgBoxToast::getInstance().showText(_LC("SRV_MsgBuildPlaceMult-BUILDING_RESULT_ERROR_NO_RESOURCE"), "");
		return;
	case MsgPB::BUILDING_REUSLT_ERROR_WRONG_TYPE:
		UiMsgBoxToast::getInstance().showText(_LC("SRV_MsgBuildPlaceMult-BUILDING_REUSLT_ERROR_WRONG_TYPE"), "");
		return;
	case MsgPB::BUILDING_RESULT_ERROR_FLAG_UPLEVEL_ALREADY:
		UiMsgBoxToast::getInstance().showText(_LC("SRV_MsgBuildPlaceMult-BUILDING_RESULT_ERROR_FLAG_UPLEVEL_ALREADY"), "");
		return;
	case MsgPB::BUILDING_RESULT_ERROR_NO_TOOL:
		UiMsgBoxToast::getInstance().showText(_LC("SRV_MsgBuildPlaceMult-BUILDING_RESULT_ERROR_NO_TOOL"), "");
		return;
	case MsgPB::BUILDING_RESULT_ERROR_CANCEL:
		UiMsgBoxToast::getInstance().showText(_LC("SRV_MsgBuildPlaceMult-BUILDING_RESULT_ERROR_CANCEL"), "");
		return;
	case MsgPB::BUILDING_RESULT_ERROR_NO_STICK:
		UiMsgBoxToast::getInstance().showText(_LC("SRV_MsgBuildPlaceMult-BUILDING_RESULT_ERROR_NO_STICK"), "");
		return;
	case MsgPB::BUILDING_RESULT_ERROR_WRONG_DIS:
		UiMsgBoxToast::getInstance().showText(_LC("SRV_MsgBuildPlaceMult-BUILDING_RESULT_ERROR_WRONG_DIS"), "");
		return;
	case MsgPB::BUILDING_RESULT_PLACECOUNT_LIMIT:
		UiMsgBoxToast::getInstance().showText(_LC("SRV_MsgBuildPlaceMult-BUILDING_RESULT_PLACECOUNT_LIMIT"), "");
		return;
	case MsgPB::BUILDING_RESULT_ERROR_WRONG_MAP:
		UiMsgBoxToast::getInstance().showText(_LC("SRV_MsgBuildPlaceMult-BUILDING_RESULT_ERROR_WRONG_MAP"), "");
		return;
	case MsgPB::BUILDING_RESULT_PLACECOUNT_RANGE:
		UiMsgBoxToast::getInstance().showText(_LC("SRV_MsgBuildPlaceMult-BUILDING_RESULT_PLACECOUNT_RANGE"), "");
		return;
	case MsgPB::BUILDING_RESULT_ERROR_FLAG_USED:
		UiMsgBoxToast::getInstance().showText(_LC("SRV_Badge-Have finish build,plaease click badge look pos"), "");
		return;
	default:
		break;
	}
}

//ship can't place
void HandlerBuilding::onMsgPlace(const MsgPB::MsgBuildPlaceMult& msg)
{
	glm::ivec2 globalCoord{ msg.pos_x(), msg.pos_y() };

	int mapDataLevel{};
	glm::ivec2 localCoord{};
	std::tie(mapDataLevel, localCoord) = MapDataMgr::getInstance().globalCoord2LocalCoord(globalCoord);
	auto&& coordId = MapDataMgr::getInstance().localCoord2CoordId(localCoord, mapDataLevel);

	auto templateId = msg.templateid();

	//pPlaceComponent->removeCheckSign(placeCoord);

	if (msg.res() != MsgPB::BUILDING_RESULT_OK)
	{
		showPlaceText(msg.res());
		return;
	}

	auto&& typeCfg = BuildingCfg::getInstance().findBuildingTypeByTemplateId(templateId);

	if (msg.changed())
	{
		bool isBadDoor{ false };
		auto oldData = StaticObjMgr::getInstance().findBuilding(coordId);
		if (oldData && oldData->getType())
			isBadDoor = oldData->getType()->m_isBadDoor;
		if (!isBadDoor)
		{
			//auto typeCfg = BuildingCfg::getInstance().findBuildingTypeByTemplateId(templateId);
			if (typeCfg && typeCfg->m_isBadDoor)
				isBadDoor = true;
		}
		if (isBadDoor)
		{
			HandlerBuildingEvent evt;
			NpcBadgeRoomCheck::getBadgeRoomComponent()->isBadge(globalCoord, evt.badgeId);
			evt.isBadge = false;
			evt.coordX = globalCoord.x;
			evt.coordY = globalCoord.y;
			evt.isPlace = true;
			HandlerBuilding::getInstance().postEvent(evt);
		}
		BuildingMgr::getInstance().replaceBuilding(coordId, templateId);


		// 57 升级互动家具 n_task_event_param_1 进阶后家具ID
		if (auto && pTowerBuilding = std::dynamic_pointer_cast<TowerBuilding>(StaticObjMgr::getInstance().findBuildingByGlobalCoord(globalCoord)))
		{
			CommonTaskEvent evt(EN_PLAYER_EVENT_TYPE_interactive_build_level_up);
			evt.m_taskEventParams.n_task_event_param_1 = pTowerBuilding->getTypeId();
			TaskEventComponent::getInstance().postEvent(evt);
		}

		return;
	}

	auto&& currmd = MapDataMgr::getInstance().getCurrMapData();
	if (!currmd)
	{
		return;
	}

	auto pMapData = currmd->getLevel(mapDataLevel);
	if (!pMapData)
	{
		return;
	}

	auto&& pMapComponent = MapComponent::getMapComponent();
	if (pMapComponent)
	{
		pMapComponent->removeDigRemoveTimer(coordId, MapCfg::Front);
	}

	if (pMapData->isBluePrint(localCoord))
	{
		BuildingMgr::getInstance().removeBuilding(coordId);
		MapDataUtil::setBlueprint(pMapData.get(), localCoord, false);
	}

	if (!PlayerComponentMgr::getInstance().getSelf()) return;
	PlayerComponent* pPlayerComponent{ PlayerComponentMgr::getInstance().getSelf()->findComponent<PlayerComponent>() };
	if (!pPlayerComponent)
	{
		return;
	}

	auto pPlaceComponent = pPlayerComponent->findComponent<PlaceComponent>();
	if (!pPlaceComponent)
	{
		return;
	}

	bool isSync{true};

	if (BuildingMgr::getInstance().isValuableBuildingMsg(templateId) || BuildingMgr::getInstance().isTowerBuildingMsg(templateId))
	{
		isSync = false;
	}

	if (LocalServer_BuildLogicCli::getInstance().isTowerDataErrorCorrect())
	{
		isSync = true;
	}


	auto&& pBuildingGameObj = BuildingMgr::getInstance().addBuildToMapData(localCoord, templateId, mapDataLevel, isSync);
	if (!pBuildingGameObj)
	{
		return;
	}

	pPlaceComponent->playPlaceEffect(globalCoord, MapCfg::MinorTileType::Building, templateId);

	Ivec2List outPlaceCoordList{};
	pMapData->findNearBuildingList(outPlaceCoordList, localCoord, MapCfg::MinorTileType::Building);

	for (decltype(outPlaceCoordList.size()) i = 0; i < outPlaceCoordList.size(); ++i)
	{
		auto&& perCoordId = MapDataMgr::getInstance().localCoord2CoordId(outPlaceCoordList[i], mapDataLevel);
		BuildingMgr::getInstance().refreshPattern(perCoordId);
	}

	auto mapId = MapSettingCfg::getInstance().getCurSceneId();
	HandlerFeed::getInstance().placeFenceReq(templateId, msg.pos_x(), msg.pos_y(),mapId, msg.isource());

	pPlaceComponent->playPlaceSound(templateId, MapCfg::MinorTileType::Building);

	//grass
	NS::Ivec2List grassCoordList{};
	auto&& pBuildingData = StaticObjMgr::getInstance().findBuilding(coordId);
	if (pBuildingData)
	{
		auto&& pBuildingTemplate = pBuildingData->getTemplate();

		if (pBuildingTemplate && BuildingCfg::getInstance().isHideGrassBuilding(pBuildingTemplate->m_templateId))
		{
			glm::ivec2 grassCoord = localCoord + MapData::COORD_DOWN;
			if (pMapData->isBasicTile(grassCoord))
			{
				if (StaticObjMgr::getInstance().removeGrassByLocalCoord(grassCoord, mapDataLevel))
				{
					grassCoordList.push_back(grassCoord);
				}
			}

			for (const auto& off : pBuildingTemplate->m_offsetList)
			{
				grassCoord = localCoord + off + MapData::COORD_DOWN;
				if (pMapData->isBasicTile(grassCoord))
				{
					if (StaticObjMgr::getInstance().removeGrassByLocalCoord(grassCoord, mapDataLevel))
					{
						grassCoordList.push_back(grassCoord);
					}
				}
			}
		}
		if (PlayerMgr::getInstance().getSelf() && msg.iggid() == PlayerMgr::getInstance().getSelf()->getIggId())
		{
			CommonTaskEvent ctevt(MsgPB::EN_PLAYER_EVENT_TYPE_put_build);
			ctevt.m_taskEventParams.vecTaskEventParam_1.push_back(pMapData->getId());
			ctevt.m_taskEventParams.vecTaskEventParam_2.push_back(pBuildingData->getTypeId());
			TaskEventComponent::getInstance().postEvent(ctevt);
		}
	}

	if (!grassCoordList.empty())
	{
		GameWorld* pWorld = SceneModule::getCurrentWorld();
		MapComponent* pMapComponent = pWorld->getCurrentLevel()->findComponent<MapComponent>();
		if (pMapComponent)
		{
			pMapComponent->refreshGrass(grassCoordList);
		}
	}

	HandlerBuildingEvent evt;
	evt.isBadge = NpcBadgeRoomCheck::getBadgeRoomComponent()->isBadge(globalCoord, evt.badgeId);
	evt.coordX = globalCoord.x;
	evt.coordY = globalCoord.y;
	evt.isPlace = true;
	if (evt.isBadge || (typeCfg && (typeCfg->m_RoomBound != 0 || typeCfg->n_ratecount_boole != 0)) )
	{
		HandlerBuilding::getInstance().postEvent(evt);
	}


	// 飞船准备-触发成就
	if (HandlerLogin::getInstance().getMapType() == MapDataMgr::MapType::ShipEdit)
	{
		if (evt.isBadge)
		{
			LocalServer_Achieve::getInstance().TriggerAchieveEvent(ACHIEVE_EVENT_TEMPLATE_ID_79054, 1, AchieveEventArg::EM_MODE::EM_MODE_SET, 0);
		}

		if (typeCfg)
		{
			if (auto && pTowerBuildingDefence = TowerBuildingCfg::getInstance().getTowerBuildingDefence(typeCfg->m_typeId, 1))
			{
				if (pTowerBuildingDefence->m_category == TowerBuildingCategory::SOUL_TOWER)
				{
					LocalServer_Achieve::getInstance().TriggerAchieveEvent(ACHIEVE_EVENT_TEMPLATE_ID_79055, 1, AchieveEventArg::EM_MODE::EM_MODE_SET, 0);
				}
			}
		}
	}
}
*/

void HandlerBuilding::onMsgBuildReplace(const MsgPB::MsgBuildReplace_MS2C& msg)
{
}

void HandlerBuilding::onMsgBuildingInfo(const MsgPB::MsgBuildingInfo& msg)
{
	int a = 0;
	a++;
}

//void HandlerBuilding::onMsgDig(const MsgPB::MsgBuildingRemoveMult& msg)
//{
//	if (msg.res() != MsgPB::BUILDING_RESULT_OK)
//	{
//		return;
//	}
//
//	glm::ivec2 globalCoord (msg.pos_x(), msg.pos_y());
//
//	/*int mapDataLevel{};
//	glm::ivec2 placeCoord{};
//	std::tie(mapDataLevel, placeCoord) = MapDataMgr::getInstance().globalCoord2LocalCoord(globalCoord);*/
//	auto&& coordId = MapDataMgr::getInstance().globalCoord2CoordId(globalCoord);
//
//	NS::I64 digId{ msg.dig_id() };
//
//	auto pDigData = DigDataMgr::getInstance().getDigData(digId);
//	if (pDigData)
//	{
//		auto&& digSingleData = pDigData->m_frontMap[coordId];
//
//		digSingleData.m_maxHp = msg.max_hp();
//		digSingleData.m_lossHpList.push_back(msg.reduce_hp());
//		digSingleData.m_tileType = MapCfg::MinorTileType::Building;
//	}
//
//	auto obj = BuildingMgr::getInstance().getBuilding(coordId);
//	if (obj)
//	{
//		auto roleComp = obj->findComponent<BuildingRoleComponent>();
//		if (roleComp)
//		{
//			roleComp->onDig();
//		}
//	}
//
//	if (msg.curr_hp() <= 0)
//	{
//		HandlerBuildingEvent evt;
//		evt.isBadge = NpcBadgeRoomCheck::getBadgeRoomComponent()->isBadge(globalCoord, evt.badgeId);
//		evt.coordX = globalCoord.x;
//		evt.coordY = globalCoord.y;
//		evt.isPlace = false;
//		HandlerBuilding::getInstance().postEvent(evt);
//	}
//}
//
//void HandlerBuilding::onMsgDead(const MsgPB::MsgPrivateDropBuildDisappearS2C& msg)
//{
//	glm::ivec2 globalCoord{ msg.pos_x(), msg.pos_y() };
//
//	int mapDataLevel{};
//	glm::ivec2 localCoord{};
//	std::tie(mapDataLevel, localCoord) = MapDataMgr::getInstance().globalCoord2LocalCoord(globalCoord);
//	auto&& coordId = MapDataMgr::getInstance().localCoord2CoordId(localCoord, mapDataLevel);
//
//	auto&& currmd = MapDataMgr::getInstance().getCurrMapData();
//	if (!currmd)
//	{
//		return;
//	}
//
//	auto pMapData = currmd->getLevel(mapDataLevel);
//	if (!pMapData)
//	{
//		return;
//	}
//
//	auto templateId = pMapData->getBuildingTile(localCoord);
//
//	auto&& pRoomTipComponent = RoomTipComponent::getRoomTipComponent();
//	if (pRoomTipComponent)
//	{
//		pRoomTipComponent->addVirtualRoomBlocks(globalCoord, false, templateId);
//	}
//
//	DropInfoList dropList;
//
//	for (int i = 0; i < msg.drop_info_size(); ++i)
//	{
//		const MsgPB::PBDropInfo& info = msg.drop_info(i);
//
//		auto pInfoData = std::make_shared<DropInfoItemData>();
//
//		pInfoData->obj_id = info.obj_id();
//		pInfoData->data_id = info.data_id();
//		pInfoData->m_iggId = msg.iggid();
//		pInfoData->m_FromType = EDROP_FROM_BUILD;
//		pInfoData->m_nFromDataId = templateId;
//		pInfoData->m_state = EDROP_DROPING;
//		pInfoData->count = info.count();
//		pInfoData->m_endMap = glm::ivec2{ info.pos_x(), info.pos_y() };
//		pInfoData->m_startMap = glm::ivec2{ msg.pos_x(), msg.pos_y() };
//		pInfoData->m_isWeapon = ItemPacketMgr::getInstance().isWeaponByTempId(pInfoData->data_id);
//		pInfoData->m_isArm = ItemPacketMgr::getInstance().isArmByTempId(pInfoData->data_id);
//		dropList.emplace_back(pInfoData);
//	}
//
//	Ivec2List outPlaceCoordList{};
//	pMapData->findNearBuildingList(outPlaceCoordList, localCoord, MapCfg::MinorTileType::Building);
//
//	std::vector<NS::I64> outCoordIdList{};
//	for (auto&& val : outPlaceCoordList)
//	{
//		auto&& perCoordId = MapDataMgr::getInstance().localCoord2CoordId(val, mapDataLevel);
//		outCoordIdList.push_back(perCoordId);
//	}
//
//	GameLog::getInstance().mapDataSyncTag(fmt::format("BuildingRemove {0}", templateId));
//	bool bForceSync{ BuildingMgr::getInstance().isForceSync(templateId) };
//	MapDataUtil::clearBuildingTiles(pMapData.get(), localCoord, templateId, bForceSync);
//
//	if (msg.exdata() == 1)		//isRemoveByInteractive
//	{
//		/*for (auto pInfoData : dropList)
//		{
//			HandlerDrop::getInstance().playAnimation(*pInfoData);
//		}*/
//
//		removeBuilding({ coordId }, dropList, outCoordIdList, false, true);
//
//		/*BuildingMgr::getInstance().onBuildingDead(coordId, true);
//		DigDataMgr::getInstance().removeHpData(coordId, MapCfg::Front);*/
//	}
//	else
//	{
//		NS::I64 digId{ msg.dig_id() };
//
//		auto&& pDigData = DigDataMgr::getInstance().getDigData(digId);
//
//		if (pDigData)
//		{
//			auto&& pDigSingleData = pDigData->m_frontMap[coordId];
//			pDigSingleData.m_isDead = true;
//
//			pDigData->m_dropInfoList = dropList;
//			pDigData->m_refreshCoordIdList = outCoordIdList;
//
//			auto building = BuildingMgr::getInstance().getBuilding(coordId);
//			if (building)
//			{
//				auto buildingRoleComp = building->findComponent<BuildingRoleComponent>();
//				if (buildingRoleComp)
//				{
//					buildingRoleComp->onDigRemove();
//				}
//			}
//
//			//防止挖不掉
//			auto&& pMapComponent = MapComponent::getMapComponent();
//			if (pMapComponent)
//			{
//				pMapComponent->addDigRemoveTimer(coordId, MapCfg::Layer::Front, [this, digId]()
//				{
//					updateViewByDigData(digId);
//				});
//			}
//		}
//		else
//		{
//			removeBuilding({ coordId }, dropList, outCoordIdList);
//		}
//	}
//
//	if (HandlerLogin::getInstance().getMapType() == MapDataMgr::MapType::Private)
//	{
//		HandlerFeed::getInstance().onDigMsg(msg.pos_x(), msg.pos_y());
//	}
//}
//
//bool HandlerBuilding::replaceBadge(const glm::ivec2& globalCoord, int badgeId, int replaceBadgeId)
//{
//	/*MsgPB::MsgBuildingRemoveMultReq msg1;
//	msg1.set_pos_x(globalCoord.x);
//	msg1.set_pos_y(globalCoord.y);
//	msg1.set_dig_id(0);
//
//	MsgMgr::getInstance().sendMessage(MsgPB::_MSG_BUILDING_REMOVE_MULT_REQ, &msg1);*/
//	auto data = NpcProtectorDataMgr::getInstance()->getNpcProtectorDataByUuid(badgeId);
//	if (data)
//		NS::LocalServer_Npc::getInstance().removeBadge(data->uuid, data->pos_x, data->pos_y, 0);
//
//	UNREG_EVENT(NpcProtectorDataMgr::getInstance(), NpcBadgeStateChangedS2CEvent);
//	REG_EVENT(NpcProtectorDataMgr::getInstance())[this , globalCoord, badgeId, replaceBadgeId](const NpcBadgeStateChangedS2CEvent& evt) {
//		if (evt.m_updatedData.lock() && ((evt.m_updatedData.lock()->uuid == badgeId) && (evt.m_updatedData.lock()->pos_x == 0 && evt.m_updatedData.lock()->pos_y == 0)))
//		{
//			UNREG_EVENT(NpcProtectorDataMgr::getInstance(), NpcBadgeStateChangedS2CEvent);
//			auto&& currmd = MapDataMgr::getInstance().getCurrMapData();
//			if (!currmd)
//			{
//				return;
//			}
//
////			PlayerComponent* m_pPlayerComponent{};
//			PlayerComponent* pPlayerComponent = PlayerComponentMgr::getInstance().getSelf()->findComponent<PlayerComponent>();
//			if (!pPlayerComponent) return;
//
//			auto&& pMapData = currmd->getLevel(0);
//			auto tempId = pMapData->getPlaceTemplateId(globalCoord, MapCfg::MinorTileType::Building, replaceBadgeId, true, pPlayerComponent->getCoord());
//			MsgPB::MsgBuildPlaceMultReq msg;
//			msg.set_pos_x(globalCoord.x);
//			msg.set_pos_y(globalCoord.y);
//			msg.set_templateid(tempId);
//			msg.set_uuid(replaceBadgeId);
//			MsgMgr::getInstance().sendMessage(MsgPB::_MSG_BUILD_PLACE_MULT_REQ, &msg);
//		}
//	};
//	return true;
//}
//
//
//void HandlerBuilding::onMsgBuildRoomInfo(const MsgPB::MsgBuildRoomInfo_S2C & msg)
//{
//	GameWorld* pWorld = SceneModule::getCurrentWorld();
//	MapComponent* pMapComponent = pWorld->getCurrentLevel()->findComponent<MapComponent>();
//	if (!pMapComponent) return;
//
//	NS::Ivec2List coords{};
//
//	for (int i = 0; i < msg.room_inter_point_size(); i++)
//	{
//		auto&& point = msg.room_inter_point(i);
//
//		auto&& pGrassData = StaticObjMgr::getInstance().findGrassByLocalCoord(glm::ivec2(point.x(), point.y() + 1), 0);
//		if (pGrassData)
//		{
//			pGrassData->setIsInRoom(true);
//			coords.push_back(pGrassData->getLocalCoord());
//		}
//	}
//
//	if (!coords.empty())
//	{
//		pMapComponent->refreshGrass(coords);
//	}
//}
//
//void HandlerBuilding::onMsgBuildRoomAddNTF_S2C(const MsgPB::MsgBuildRoomAddNTF_S2C & msg)
//{
//	/*GameWorld* pWorld = SceneModule::getCurrentWorld();
//	MapComponent* pMapComponent = pWorld->getCurrentLevel()->findComponent<MapComponent>();
//	if (!pMapComponent) return;
//
//	NS::Ivec2List roomCoords{};
//
//	for (int i = 0; i < msg.room_inter_point_size(); i++)
//	{
//		auto&& point = msg.room_inter_point(i);
//		roomCoords.push_back(glm::ivec2(point.x(), point.y()));
//	}
//
//	if (!roomCoords.empty())
//	{
//		auto&& pRoomEffect = RoomEffectComponent::create();
//		if (pRoomEffect)
//		{
//			pMapComponent->addComponent(pRoomEffect);
//			pRoomEffect->playRoomCreateEffect(roomCoords);
//		}
//	}*/
//}
//
//
//void HandlerBuilding::onMsgBuildRoomDelNTF_S2C(const MsgPB::MsgBuildRoomDelNTF_S2C & msg)
//{
//	/*GameWorld* pWorld = SceneModule::getCurrentWorld();
//	MapComponent* pMapComponent = pWorld->getCurrentLevel()->findComponent<MapComponent>();
//	if (!pMapComponent) return;
//
//	NS::Ivec2List roomCoords{};
//
//	for (int i = 0; i < msg.room_inter_point_size(); i++)
//	{
//		auto&& point = msg.room_inter_point(i);
//		roomCoords.push_back(glm::ivec2(point.x(), point.y()));
//	}
//
//	if (!roomCoords.empty())
//	{
//		auto&& pRoomEffect = RoomEffectComponent::create();
//		if (pRoomEffect)
//		{
//			pMapComponent->addComponent(pRoomEffect);
//			pRoomEffect->playRoomDestoryEffect(roomCoords);
//		}
//	}
//
//	auto&& pRoomTipComponent = RoomTipComponent::getRoomTipComponent();
//	if (pRoomTipComponent)
//	{
//		pRoomTipComponent->onRemoveRoom(roomCoords);
//	}*/
//}
//
//void HandlerBuilding::onMsgValuableBuildPutS2C(const MsgPB::MsgValuableBuildPutS2C& msg)
//{
//	if (msg.res() != MsgPB::BUILDING_RESULT::BUILDING_RESULT_OK)
//	{
//		showPlaceText(msg.res());
//	}
//}
//
//void HandlerBuilding::onMsgValuableBuildDigS2C(const MsgPB::MsgValuableBuildDigS2C& msg)
//{
//	if (msg.res() != MsgPB::BUILDING_RESULT::BUILDING_RESULT_OK)
//	{
//		showPlaceText(msg.res());
//	}
//}
//
//void HandlerBuilding::onMsgValuableBuildGetS2C(const MsgPB::MsgValuableBuildGetS2C& msg)
//{
//	StaticObjMgr::getInstance().clearValuableBuildings();
//
//	auto&& currMapDataId = MapDataMgr::getInstance().getCurrMapDataId();
//
//	for (int i = 0; i < msg.datas_size(); i++)
//	{
//		auto&& data = msg.datas(i);
//
//		if (data.map_id() != currMapDataId)
//		{
//			continue;
//		}
//
//		auto pVal = std::make_shared<ValuableBuilding>();
//		pVal->m_uuid = data.uuid();
//		pVal->m_coord = glm::ivec2(data.pos_x(), data.pos_y());
//		pVal->m_mapId = data.map_id();
//		pVal->m_buildingTemplateId = data.build_id();
//
//		StaticObjMgr::getInstance().addValuableBuilding(pVal);
//	}
//
//	std::map<glm::ivec2, std::shared_ptr<Building>> withoutValuableDatas{};
//	std::map<glm::ivec2, std::shared_ptr<Building>> haveValuableDatas{};
//	std::vector<std::shared_ptr<ValuableBuilding>> withoutBuildingDatas{};
//
//	//验证服务端贵重家具数据
//	auto&& pBuildings = StaticObjMgr::getInstance().getBuildings();
//	for (auto&& pir : pBuildings)
//	{
//		auto&& pBuildingData = pir.second;
//		if (BuildingMgr::getInstance().isValuableBuildingMsgByType(pBuildingData->getType()))
//		{
//			if (!StaticObjMgr::getInstance().findValuableBuilding(pBuildingData->getCoord(), currMapDataId))
//			{
//				withoutValuableDatas[pBuildingData->getCoord()] = pBuildingData;
//			}
//			else
//			{
//				haveValuableDatas[pBuildingData->getCoord()] = pBuildingData;
//			}
//		}
//	}
//
//	auto&& valuableBuildings = StaticObjMgr::getInstance().getValuableBuildings();
//	for (auto&& pir : valuableBuildings)
//	{
//		auto&& pValuableBuilding = pir.second;
//		auto&& it = haveValuableDatas.find(pValuableBuilding->m_coord);
//		if (it == haveValuableDatas.end())
//		{
//			withoutBuildingDatas.emplace_back(pValuableBuilding);
//		}
//	}
//
//	//有地图数据却没有贵重家具数据
//	for (auto&& pir : withoutValuableDatas)
//	{
//		auto&& pBuildingData = pir.second;
//
//		//移除有地图数据却没有贵重家具数据的家具
//		bool flag;
//		LocalServer_BuildLogicCli::getInstance().destroyBuilding(pBuildingData->getCoord(), INT32_MAX, 0, flag, false);
//	}
//
//	//有贵重家具数据却没有地图数据
//	for (auto&& val : withoutBuildingDatas)
//	{
//		//向服务端发起退回
//		requestValuableBuildExcept(val->m_uuid);
//	}
//}
//
//void HandlerBuilding::onMsgValuableBuildExceptDigS2C(const MsgPB::MsgValuableBuildExceptDigS2C& msg)
//{
//	if (msg.res() != MsgPB::BUILDING_RESULT_OK)
//	{
//		showPlaceText(msg.res());
//	}
//}
//
////onTowerAdd
//void HandlerBuilding::onMsgBuildTowerAddTowerData_S2C(const MsgPB::MsgBuildTowerAddTowerData_S2C& msg)
//{
//	
//}
//
////onTowerDel
//void HandlerBuilding::onMsgBuildTowerDelTowerData_S2C(const MsgPB::MsgBuildTowerDelTowerData_S2C& msg)
//{
//	if (msg.res() != MsgPB::TOWERBUILDING_RESULT::TOWERBUILDING_RESULT_OK)
//	{
//		HandlerSurvivalTower::getInstance().showErrorMsg("HandlerBuilding::onMsgBuildTowerDelTowerData_S2C", msg.res());
//	}
//}
//
//bool HandlerBuilding::updateViewByDigData(NS::I64 digId, bool isPublicDrop/* = false*/, bool isDelay/* = false*/, std::function<void()>* delayFunc/* = nullptr*/)
//{
//	auto&& pDigData = DigDataMgr::getInstance().getDigData(digId);
//
//	if (!pDigData) return false;
//
//	std::vector<NS::I64> coordIdList;
//
//	for (auto&& p : pDigData->m_frontMap)
//	{
//		auto&& coordId = p.first;
//		auto&& singleData = p.second;
//		auto&& pHpData = DigDataMgr::getInstance().getHpData(coordId, MapCfg::Front);
//
//		if (singleData.m_tileType != MapCfg::MinorTileType::Building)
//		{
//			continue;
//		}
//
//		if (!pHpData)
//		{
//			pHpData = DigDataMgr::getInstance().addHpData(coordId, MapCfg::Front);
//
//			pHpData->m_maxHp = singleData.m_maxHp;
//			pHpData->m_currHp = singleData.m_maxHp;
//		}
//
//		while (!singleData.m_lossHpList.empty())
//		{
//			pHpData->m_currHp -= singleData.m_lossHpList.back();
//			singleData.m_lossHpList.pop_back();
//		}
//
//		if (/*pHpData->m_currHp <= 0 && */singleData.m_isDead)
//		{
//			coordIdList.emplace_back(coordId);
//		}
//	}
//
//
//	if (coordIdList.size() > 0)
//	{
//		if (isDelay)
//		{
//
//			auto dropInfoList = pDigData->m_dropInfoList;
//			auto refreshCoordUuidList = pDigData->m_refreshCoordIdList;
//			(*delayFunc) = [this, coordIdList, dropInfoList, refreshCoordUuidList, isPublicDrop]()
//			{
//				removeBuilding(coordIdList, dropInfoList, refreshCoordUuidList, isPublicDrop);
//			};
//		}
//		else
//		{
//			removeBuilding(coordIdList, pDigData->m_dropInfoList, pDigData->m_refreshCoordIdList, isPublicDrop);
//		}
//		return true;
//	}
//
//	return false;
//}
//
//void HandlerBuilding::removeBuilding(const std::vector<NS::I64>& coordIdList, const DropInfoList& dropInfoList, const std::vector<NS::I64>& refreshCoordIdList, bool isPublicDrop/* = false*/, bool isInteractive/* = false*/)
//{
//	if (!coordIdList.empty())
//	{
//		auto&& pBuildingData = StaticObjMgr::getInstance().findBuilding(coordIdList.at(0));
//		if (pBuildingData)
//		{
//			int badgeId{0};
//			auto removeCoord = pBuildingData->getCoord();
//			if (!NpcBadgeRoomCheck::getBadgeRoomComponent()->isHaveBeDel(removeCoord, badgeId))
//			{
//				HandlerBuildingEvent evt;
//				evt.badgeId = badgeId;
//				evt.isBadge = badgeId != 0 ? true : false;
//				evt.coordX = removeCoord.x;
//				evt.coordY = removeCoord.y;
//				evt.isPlace = false;
//				HandlerBuilding::getInstance().postEvent(evt);
//			}
//		}
//	}
//
//	GameWorld* pWorld = SceneModule::getCurrentWorld();
//	MapComponent* pMapComponent = pWorld->getCurrentLevel()->findComponent<MapComponent>();
//	if (!pMapComponent) return;
//
//	for (auto pInfoData : dropInfoList)
//	{
//		if (isPublicDrop)
//		{
//			HandlerDrop::getInstance().playAnimationAtPublicMap(*pInfoData);
//		}
//		else
//		{
//			HandlerDrop::getInstance().playAnimation(*pInfoData);
//		}
//
//	}
//
//	NS::Ivec2List grassCoordList{};
//	for (auto&& coordUuid : coordIdList)
//	{
//		auto&& pBuildingData = StaticObjMgr::getInstance().findBuilding(coordUuid);
//		if (pBuildingData)
//		{
//			auto&& pBuildingTemplate = pBuildingData->getTemplate();
//			if (!pBuildingTemplate)
//			{
//				continue;
//			}
//
//			if (!BuildingCfg::getInstance().isHideGrassBuilding(pBuildingTemplate->m_templateId))
//			{
//				continue;
//			}
//
//			auto&& currmd = MapDataMgr::getInstance().getCurrMapData();
//			if (!currmd)
//			{
//				continue;
//			}
//
//			auto pMapData = currmd->getLevel(pBuildingData->getMapDataLevel());
//			if (!pMapData)
//				continue;
//
//			auto&& placeCoord = pBuildingData->getLocalCoord();
//
//			glm::ivec2 grassCoord = placeCoord + MapData::COORD_DOWN;
//			if (pMapData->isBasicTile(grassCoord))
//			{
//				if (StaticObjMgr::getInstance().addGrass(grassCoord))
//				{
//					grassCoordList.push_back(grassCoord);
//				}
//			}
//
//			for (const auto& off : pBuildingTemplate->m_offsetList)
//			{
//				grassCoord = placeCoord + off + MapData::COORD_DOWN;
//				if (pMapData->isBasicTile(grassCoord))
//				{
//					if (StaticObjMgr::getInstance().addGrass(grassCoord))
//					{
//						grassCoordList.push_back(grassCoord);
//					}
//				}
//			}
//
//			if (pBuildingTemplate->m_templateId == GlobalCfg::getInstance().getRapidRocketInvisibleBuildingTemplateId() && currmd->getId() == GlobalCfg::getInstance().getAchieveGuideLimitMapId())
//			{
//				//急速火箭放进背包
//				LocalServer_ItemPacket::getInstance().AddItemByDataID(GlobalCfg::getInstance().getRapidRocketTowerBuildingTypeId(), 1, MsgPB::PACKET_TYPE::BODY);
//
//				auto&& pMonsterData = MonsterMgr::getInstance().getMonsterByTypeId(GlobalCfg::getInstance().getRapidRocketBuildingMobDefId());
//				if (pMonsterData && pMonsterData->getCoord() == pBuildingData->getCoord())
//				{
//					//移除建筑怪
//					LocalServer_BirthMob::getInstance().collectMob(pMonsterData->getId());
//				}
//			}
//		}
//
//		BuildingMgr::getInstance().onBuildingDead(coordUuid, isInteractive);
//		DigDataMgr::getInstance().removeHpData(coordUuid, MapCfg::Front);
//
//		if (pMapComponent)
//		{
//			pMapComponent->removeDigRemoveTimer(coordUuid, MapCfg::Front);
//		}
//	}
//
//	if (!grassCoordList.empty())
//	{
//		pMapComponent->refreshGrass(grassCoordList);
//	}
//
//	for (decltype(refreshCoordIdList.size()) i = 0; i < refreshCoordIdList.size(); ++i)
//	{
//		BuildingMgr::getInstance().refreshPattern(refreshCoordIdList[i]);
//	}
//}
//
//void HandlerBuilding::onMsgBuildRoomError(const MsgPB::MsgBuildPutBadgeCondition_S2C & msg)
//{
//	//std::set<int> condSet;
//	//condSet.insert(MsgPB::EN_ROOM_CONDITION_TYPE_SPACE_LESS);
//	//condSet.insert(MsgPB::EN_ROOM_CONDITION_TYPE_SPACE_BIG);
//	//condSet.insert(MsgPB::EN_ROOM_CONDITION_TYPE_WALL_LESS);
//	//condSet.insert(MsgPB::EN_ROOM_CONDITION_TYPE_COMFORTABLE_LESS);
//	//condSet.insert(MsgPB::EN_ROOM_CONDITION_TYPE_COMFORTABLE_BIG);
//	//condSet.insert(MsgPB::EN_ROOM_CONDITION_TYPE_HEIGHT_LESS);
//	//condSet.insert(MsgPB::EN_ROOM_CONDITION_TYPE_HEIGHT_BIG);
//	//condSet.insert(MsgPB::EN_ROOM_CONDITION_TYPE_NO_LIGHT);
//	//condSet.insert(MsgPB::EN_ROOM_CONDITION_TYPE_NO_ROOM);
//	//condSet.insert(MsgPB::EN_ROOM_CONDITION_TYPE_TOO_MANY_FLAG);
//
//	//bool isRoom{ true };
//	//for (int i = 0; i < msg.condition_type_size(); i++)
//	//{
//	//	const MsgPB::EN_ROOM_CONDITION_TYPE& pCondition = msg.condition_type(i);
//	//	if (pCondition == MsgPB::EN_ROOM_CONDITION_TYPE_NO_ROOM)
//	//	{
//	//		isRoom = false;
//	//		break;
//	//	}
//	//}
//
//	//// 如果前提条件不是个房间，就无所谓判断后面的条件，所以不插入;
//	//std::map<int, bool> condMap;
//	//if (!isRoom)
//	//{
//	//	condMap.insert(std::pair<int, bool>(MsgPB::EN_ROOM_CONDITION_TYPE_NO_ROOM, false));
//	//}
//	//else
//	//{
//	//	for (auto cond : condSet)
//	//	{
//	//		condMap.insert(std::pair<int, bool>(cond, true));
//	//	}
//
//	//	condMap[MsgPB::EN_ROOM_CONDITION_TYPE_NO_LIGHT] = msg.has_door();
//	//	for (int i = 0; i < msg.condition_type_size(); i++)
//	//	{
//	//		const MsgPB::EN_ROOM_CONDITION_TYPE& pCondition = msg.condition_type(i);
//	//		condMap[pCondition] = false;
//	//	}
//	//}
//
//	//auto pDialog = UiMgr::getInstance().showDialog<UiNpcBadgePlaceTip>(true);
//	//pDialog->showPutTip(condMap);
//}
//

