#include "stdafx.h"
#include "MapDataMgr.h"
#include "MapData.h"
#include "config/MapSettingCfg.h"
#include "common/Heartbeat.h"
//#include "MapData_Double.h"
//#include "../config/FieldAdventureCfg.h"
//#include "../../libsandbox/source/npc/NpcPetFight.h"

int MapDataMgr::generateMapId()
{
	static int s_mapDataId{};
	return ++s_mapDataId;
}

MapDataMgr& MapDataMgr::getInstance()
{
	static MapDataMgr s;
	return s;
}

void MapDataMgr::terminate()
{
    m_mapDataList.clear();
}

bool MapDataMgr::addMapData(const std::shared_ptr<MapData>& mapData)
{
	if (!mapData) return false;
	return m_mapDataList.emplace(mapData->getId(), mapData).second;
}

std::shared_ptr<MapData> MapDataMgr::findMapData(int id) const
{
	auto iter = m_mapDataList.find(id);
	return iter == m_mapDataList.end() ? nullptr : iter->second;
}

std::shared_ptr<MapData> MapDataMgr::getCurrMapData() const
{
	auto iter = m_mapDataList.find(m_currMapDataId);
	if (iter == m_mapDataList.end()) iter = m_mapDataList.begin();
	return iter == m_mapDataList.end() ? nullptr : iter->second;
}

bool MapDataMgr::removeMapData(int id)
{
	m_mapDataList.erase(id);
	return true;
}

bool MapDataMgr::setCurrMapDataId(int id)
{
	m_currMapDataId = id;
	return true;
}

void MapDataMgr::StartMapTime()
{
	m_n64MapBeginTm = Heartbeat::getInstance().getServerTimeMs();
}

NS::int32 MapDataMgr::GetDungeonLessTm()
{
	if (m_n64MapBeginTm == 0) {
		return 0;
	}

	auto&& pMapCfg = MapSettingCfg::getInstance().findMap(getMapId());
	if (!pMapCfg) {
		return 0;
	}

	NS::I64 n64CurTm = m_n64MapBeginTm + (NS::I64)(pMapCfg->passtime*1000);
	NS::I64 nCurTm = Heartbeat::getInstance().getServerTimeMs();
	if (n64CurTm <= nCurTm) {
		return 0;
	}
	return (NS::int32)((n64CurTm - nCurTm) / 1000);
}

///////////////////////////////////////

//const NS::I64 MapDataMgr::ERROR_UUID = MapDataMgr::localCoord2CoordId(glm::ivec2(MapDataMgr::ERROR_POS), 0);
//
//NS::I64 MapDataMgr::generateCoordId(const glm::ivec2 & localCoord, int mapDataLevel)
//{
//	return localCoord2CoordId(localCoord, mapDataLevel);
//}
//
//NS::I64 MapDataMgr::globalCoord2CoordId(const glm::ivec2 & globalCoord)
//{
//	glm::ivec2 localCoord{};
//	int mapDataLevel{};
//	std::tie(mapDataLevel, localCoord) = globalCoord2LocalCoord(globalCoord);
//
//	return localCoord2CoordId(localCoord, mapDataLevel);
//}
//
//std::tuple<int, glm::ivec2> MapDataMgr::globalCoord2LocalCoord(const glm::ivec2 & globalCoord)
//{
//	auto pMapData = std::dynamic_pointer_cast<MapData_Double>(getCurrMapData());
//	if (!pMapData)
//	{
//		return std::make_tuple(0, globalCoord);
//	}
//
//	for (int i = 1; i >= 0; i--)
//	{
//		auto&& coord = i == 0 ? globalCoord : globalCoord - pMapData->getOffsetCoord();
//
//		auto&& pSubMapData = pMapData->getLevel(i);
//
//		if (!pSubMapData)
//		{
//			continue;
//		}
//
//		auto&& placeCoord = pSubMapData->findPlacePos(coord);
//
//		if (pSubMapData->isBluePrint(placeCoord) || pSubMapData->isBluePrintBack(placeCoord) || !pSubMapData->isFrontTileEmpty(placeCoord) || !pSubMapData->isBackTileEmpty(placeCoord))
//		{
//			return std::make_tuple(i, coord);
//		}
//	}
//
//	return std::make_tuple(0, globalCoord);
//}
//
//std::tuple<int, glm::ivec2> MapDataMgr::coordId2LocalCoord(NS::I64 coordId)
//{
//	static unsigned NS::I64 mask = ~unsigned NS::I64(0);
//	static unsigned NS::I64 xMask = mask << 33 >> 1;
//	static unsigned NS::I64 yMask = mask >> 32;
//	static unsigned NS::I64 mapDataLevelMask = mask << 63;
//
//	glm::ivec2 localCoord{};
//	localCoord.x = (coordId & xMask) >> 32;
//	localCoord.y = (int)(coordId & yMask);
//	int mapDataLevel = (coordId & mapDataLevelMask) >> 63;
//
//	return std::make_tuple(mapDataLevel, localCoord);
//}
//
//glm::ivec2 MapDataMgr::coordId2GlobalCoord(NS::I64 coordId)
//{
//	glm::ivec2 localCoord{};
//	int mapDataLevel{};
//	std::tie(mapDataLevel, localCoord) = coordId2LocalCoord(coordId);
//
//	return localCoord2GlobalCoord(localCoord, mapDataLevel);
//}
//
//glm::ivec2 MapDataMgr::localCoord2GlobalCoord(const glm::ivec2 & localCoord, int mapDataLevel)
//{
//	if (mapDataLevel == 1)
//	{
//		auto pMapData = std::dynamic_pointer_cast<MapData_Double>(getCurrMapData());
//		if (pMapData)
//		{
//			return localCoord + pMapData->getOffsetCoord();
//		}
//	}
//
//	return localCoord;
//}
//
//NS::I64 MapDataMgr::localCoord2CoordId(const glm::ivec2 & localCoord, int mapDataLevel)
//{
//	if (localCoord.x >= INT32_MAX) return ERROR_POS;
//	if (localCoord.y >= INT32_MAX) return ERROR_POS;
//
//	return (unsigned NS::I64)localCoord.x << 32 | localCoord.y | (unsigned NS::I64)mapDataLevel << 63;
//}
//
//static std::set<unsigned int> DIG_MINE_DUNGEON_MAP_IDS{ 306001, 306002, 306003 };
//
//////////////////////////////////////////////
////是否判断贵重家具
//bool MapDataMgr::isValuableBuildingMsg()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::Private:
//		return true;
//	default:
//		break;
//	}
//
//	return false;
//}
//
//bool MapDataMgr::isTowerBuildingMsg()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::Private:
//	case MapDataMgr::MapType::ShipEdit:
//		return true;
//	default:
//		break;
//	}
//
//	return false;
//}
//
////是否可以放置土块
//bool MapDataMgr::isCanPlaceBasic()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::Public:
//	case MapDataMgr::MapType::FriendVisit:
//	case MapDataMgr::MapType::SquadPvp:
//		return false;
//	default:
//		break;
//	}
//
//	return true;
//}
//
////是否可以挖掘土块
//bool MapDataMgr::isCanDigBasic()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::Public:
//	case MapDataMgr::MapType::FriendVisit:
//	case MapDataMgr::MapType::SquadPvp:
//		return false;
//	default:
//		break;
//	}
//
//	return true;
//}
//
////是否可以放置家具
//bool MapDataMgr::isCanPlaceBuilding()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::Public:
//	case MapDataMgr::MapType::FriendVisit:
//	case MapDataMgr::MapType::TowerChallenge:
//	case MapDataMgr::MapType::SquadPvp:
//		return false;
//	default:
//		break;
//	}
//
//	return true;
//}
//
////是否可以挖掘家具
//bool MapDataMgr::isCanDigBuilding()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::Public:
//	case MapDataMgr::MapType::FriendVisit:
//	case MapDataMgr::MapType::TowerChallenge:
//	case MapDataMgr::MapType::SquadPvp:
//		return false;
//	default:
//		break;
//	}
//
//	return true;
//}
//
////是否可以放置植物
//bool MapDataMgr::isCanPlacePlant()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::Public:
//	case MapDataMgr::MapType::FriendVisit:
//	case MapDataMgr::MapType::TowerChallenge:
//	case MapDataMgr::MapType::SquadPvp:
//		return false;
//	default:
//		break;
//	}
//
//	return true;
//}
//
////是否可以挖掘植物
//bool MapDataMgr::isCanDigPlant()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::Public:
//	case MapDataMgr::MapType::FriendVisit:
//	case MapDataMgr::MapType::TowerChallenge:
//	case MapDataMgr::MapType::SquadPvp:
//		return false;
//	default:
//		break;
//	}
//
//	return true;
//}
//
////挖掘是否有掉落
//bool MapDataMgr::isHaveDigDrop()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::Public:
//	case MapDataMgr::MapType::FriendVisit:
//	case MapDataMgr::MapType::TowerChallenge:
//	case MapDataMgr::MapType::TowerPVP:
//	case MapDataMgr::MapType::SquadPvp:
//		return false;
//	default:
//		break;
//	}
//
//	return true;
//}
//
////是否有互动家具按钮
//bool MapDataMgr::isHaveBuildingBtn()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::FriendVisit:
//	case MapDataMgr::MapType::TowerChallenge:
//	case MapDataMgr::MapType::TowerPVP:
//	case MapDataMgr::MapType::SquadPvp:
//		return false;
//	default:
//		break;
//	}
//
//	return true;
//}
//
////是否可以与npc互动
//bool MapDataMgr::isCanInteractNpc()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::FriendVisit:
//	case MapDataMgr::MapType::TowerChallenge:
//	case MapDataMgr::MapType::TowerPVP:
//	case MapDataMgr::MapType::SquadPvp:
//		return false;
//	default:
//		break;
//	}
//
//	return true;
//}
//
////是否有挖掘操作
//bool MapDataMgr::isHaveDigOperator()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::Public:
//	case MapDataMgr::MapType::FriendVisit:	
//	case MapDataMgr::MapType::Party:
//	case MapDataMgr::MapType::Dungeon:
//	{
//		if (DIG_MINE_DUNGEON_MAP_IDS.find(m_currMapDataId) != DIG_MINE_DUNGEON_MAP_IDS.end())
//		{
//			return true;
//		}
//		return false;
//	}
//	case MapDataMgr::MapType::SquadPvp:
//		return false;
//	default:
//		break;
//	}
//
//	return true;
//}
//
////是否有放置操作
//bool MapDataMgr::isHavePlaceOperator()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::Public:
//	case MapDataMgr::MapType::FriendVisit:	
//	case MapDataMgr::MapType::Party:
//	case MapDataMgr::MapType::Dungeon:
//	{
//		if (DIG_MINE_DUNGEON_MAP_IDS.find(m_currMapDataId) != DIG_MINE_DUNGEON_MAP_IDS.end())
//		{
//			return true;
//		}
//	}
//	case MapDataMgr::MapType::SquadPvp:
//		return false;
//	default:
//		break;
//	}
//
//	return true;
//}
//
////是否有攻击操作
//bool MapDataMgr::isHaveAtkOperator()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::Public:
//	case MapDataMgr::MapType::FriendVisit:
//		return false;
//	default:
//		break;
//	}
//
//	return true;
//}
//
////是否有传送门家具
//bool MapDataMgr::isHavePortalBuilding()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::TowerPVP:
//	case MapDataMgr::MapType::TowerChallenge:
//	case MapDataMgr::MapType::FriendVisit:
//		return false;
//	default:
//		break;
//	}
//
//	return true;
//}
//
//bool MapDataMgr::isForbidMapZoomChange()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::Dungeon:
//	{
//		if (DIG_MINE_DUNGEON_MAP_IDS.find(m_currMapDataId) != DIG_MINE_DUNGEON_MAP_IDS.end())
//		{
//			return false;
//		}
//	}
//	case MapDataMgr::MapType::NpcPetFight:
//	case MapDataMgr::MapType::FieldAdventure:
//	case MapDataMgr::MapType::HangupCommonMap:
//	case MapDataMgr::MapType::SquadPvp:
//	case MapDataMgr::MapType::Endless:
//		return true;
//	default:
//		break;
//	}
//
//	return false;
//}
//
//bool MapDataMgr::isHaveViewModeBtn()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::ShipEdit:
//		return false;
//	default:
//		break;
//	}
//
//	return true;
//}
//
//bool MapDataMgr::isForbidMapSyncCache()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::Dungeon:
//	case MapDataMgr::MapType::NpcPetFight:
//	case MapDataMgr::MapType::SquadPvp:
//	case MapDataMgr::MapType::Endless:
//	case MapDataMgr::MapType::FriendVisit:
//	case MapDataMgr::MapType::Public:
//	case MapDataMgr::MapType::TowerPVP:
//	case MapDataMgr::MapType::TowerChallenge:
//	case MapDataMgr::MapType::FieldAdventure:
//	case MapDataMgr::MapType::HangupCommonMap:
//		return true;
//	default:
//		break;
//	}
//
//	return false;
//}
//
//bool MapDataMgr::isPlantGrow()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::Private:
//		return true;
//	default:
//		break;
//	}
//
//	return false;
//}
//
////是否在受击时显示等级和血条
//bool MapDataMgr::isOnHurtShowHpAndLevel()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::NpcPetFight:
//	case MapDataMgr::MapType::SquadPvp:
//	case MapDataMgr::MapType::FieldAdventure:
//	case MapDataMgr::MapType::HangupCommonMap:
//		return false;
//	default:
//		break;
//	}
//
//	return true;
//}
//
////是否在受击时显示名字
//bool MapDataMgr::isOnHurtShowNicknameAndTitle()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::NpcPetFight:
//	case MapDataMgr::MapType::SquadPvp:
//	case MapDataMgr::MapType::FieldAdventure:
//	case MapDataMgr::MapType::HangupCommonMap:
//		return false;
//	default:
//		break;
//	}
//
//	return true;
//}
//
////是否始终显示等级和血条
//bool MapDataMgr::isAlwaysShowHpAndLevel()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::NpcPetFight:
//	case MapDataMgr::MapType::SquadPvp:
//	case MapDataMgr::MapType::FieldAdventure:
//		return true;
//	default:
//		break;
//	}
//
//	return false;
//}
//
////是否始终隐藏等级显示名字
//bool MapDataMgr::isAlwaysHideNicknameAndTitle()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::NpcPetFight:
//	case MapDataMgr::MapType::SquadPvp:
//	case MapDataMgr::MapType::FieldAdventure:
//	case MapDataMgr::MapType::HangupCommonMap:
//		return true;
//	default:
//		break;
//	}
//
//	return false;
//}
//
////是否始终隐藏跟随宠物
//bool MapDataMgr::isAlwaysHideFollowPet()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::NpcPetFight:
//	case MapDataMgr::MapType::SquadPvp:
//	case MapDataMgr::MapType::FieldAdventure:
//	case MapDataMgr::MapType::HangupCommonMap:
//		return true;
//	default:
//		break;
//	}
//
//	return false;
//}
//
//
////是否玩家自己被限制只能在地图边界范围内活动
//bool MapDataMgr::isLimitHeroInMapBoundary()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::SquadPvp:
//		return true;
//	default:
//		break;
//	}
//
//	return false;
//}
//
////是否使用临时物品(背包)数据
//bool MapDataMgr::isUseItemTemp()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::NpcPetFight:
//	case MapDataMgr::MapType::SquadPvp:
//	case MapDataMgr::MapType::FieldAdventure:
//	case MapDataMgr::MapType::HangupCommonMap:
//		return true;
//	default:
//		break;
//	}
//
//	return false;
//}
//
//bool MapDataMgr::isAdjustPlayerPos()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::NpcPetFight:
//	case MapDataMgr::MapType::SquadPvp:
//	case MapDataMgr::MapType::FieldAdventure:
//	case MapDataMgr::MapType::HangupCommonMap:
//		return false;
//	default:
//		break;
//	}
//	return true;
//}
//
//// 是否开启死亡定时器
//bool MapDataMgr::isRoleDeadTime()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::NpcPetFight:
//	case MapDataMgr::MapType::SquadPvp:
//	case MapDataMgr::MapType::TowerPVP:
//	case MapDataMgr::MapType::FieldAdventure:
//	case MapDataMgr::MapType::HangupCommonMap:
//	case MapDataMgr::MapType::TowerChallenge:
//		return false;
//	default:
//		break;
//	}
//	return true;
//}
//
//bool MapDataMgr::isClientSwitchMapCleanData()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::NpcPetFight:
//	case MapDataMgr::MapType::FieldAdventure:
//	case MapDataMgr::MapType::HangupCommonMap:
//		return true;
//	default:
//		break;
//	}
//
//	return false;
//}
//
//bool MapDataMgr::isPlayerSetAway()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::NpcPetFight:
//	case MapDataMgr::MapType::FieldAdventure:
//	case MapDataMgr::MapType::HangupCommonMap:
//		return true;
//	default:
//		break;
//	}
//	return false;
//}
//
//bool MapDataMgr::IsFieldAdventureFight()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::FieldAdventure:
//		return true;
//	default:
//		break;
//	}
//	return false;
//}
//
//// 是否为客户端自己切地图的类型
//bool MapDataMgr::IsClientMapType(MapDataMgr::MapType nMapType)
//{
//	switch (nMapType)
//	{
//	case MapDataMgr::MapType::NpcPetFight:
//	case MapDataMgr::MapType::SquadPvp:
//	case MapDataMgr::MapType::FieldAdventure:
//	case MapDataMgr::MapType::HangupCommonMap:
//		return true;
//	default:
//		break;
//	}
//	return false;
//}
//
//bool MapDataMgr::isHangupCommonMap()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::HangupCommonMap:
//		return true;
//	default:
//		break;
//	}
//	return false;
//}
//
//// 当玩家自己进入地图时是否设置摄像机跟踪玩家
//bool MapDataMgr::IsOnSelfEnterMapCameraFollowPlayer()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::HangupCommonMap:
//	case MapDataMgr::MapType::FieldAdventure:
//	case MapDataMgr::MapType::NpcPetFight:
//	case MapDataMgr::MapType::SquadPvp:
//		return false;
//	default:
//		break;
//	}
//	return true;
//}
//
//// 是否使用战斗悬赏(绝对优先攻击距离最近的目标)模式
//bool MapDataMgr::IsUseNpcPetFightAttackTargetMode()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::HangupCommonMap:
//	case MapDataMgr::MapType::FieldAdventure:
//	case MapDataMgr::MapType::NpcPetFight:
//		return true;
//	default:
//		break;
//	}
//	return false;
//}
//
//bool MapDataMgr::isEnableAtSceneTouchEvent()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::HangupCommonMap:
//		return false;
//	default:
//		break;
//	}
//	return true;
//}
//
//// 是否显示Lobby界面;
//bool MapDataMgr::isEnableLobby()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::NpcPetFight:
//	case MapDataMgr::MapType::SquadPvp:
//	case MapDataMgr::MapType::FieldAdventure:
//	case MapDataMgr::MapType::HangupCommonMap:
//		return false;
//	default:
//		break;
//	}
//
//	return true;
//}
//
//// 是否需要发送玩家死亡日志数据给服务端
//bool MapDataMgr::IsEnableSendMsgPlayerClientModeDead()
//{
//	switch (m_curMapType)
//	{
//	case MapDataMgr::MapType::Public:
//	case MapDataMgr::MapType::Dungeon:
//	case MapDataMgr::MapType::PVP:
//	case MapDataMgr::MapType::Party:
//	case MapDataMgr::MapType::Endless:
//	case MapDataMgr::MapType::TowerPVP:
//	case MapDataMgr::MapType::ShipEdit:
//	case MapDataMgr::MapType::NpcPetFight:
//	case MapDataMgr::MapType::FriendVisit:
//	case MapDataMgr::MapType::SquadPvp:
//	case MapDataMgr::MapType::FieldAdventure:
//	case MapDataMgr::MapType::HangupCommonMap:
//		return false;
//	default:
//		break;
//	}
//	return true;
//}
//
///////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////////////////
//MapData* LUA(getCurrMapData)(void)
//{
//	return MapDataMgr::getInstance().getCurrMapData().get();
//}
//
//MapDataMgr* LUA(getCurrMapMgr)(void)
//{
//	return &MapDataMgr::getInstance();
//}
