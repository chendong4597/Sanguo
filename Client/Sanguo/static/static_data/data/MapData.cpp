/*!
 * \file MapData.cpp
 *
 * \author laizhisheng
 * \date 2016/11/22 10:45
 *
 */
#include "stdafx.h"
#include "MapData.h"
//#include "../config/BuildingCfg.h"
//#include "../config/PlantCfg.h"
//#include "../pathfinding/FlightPathfindingEx.h"
//#include "../pathfinding/PathfindingEx.h"
#include "base64.h"
//#include "StaticObj.h"
//#include "../config/MapSettingCfg.h"
//#include <LogUtil.h>

using namespace NS;


//const glm::ivec2 MapData::COORD_UP         {  0, -1 };
//const glm::ivec2 MapData::COORD_DOWN       {  0,  1 };
//const glm::ivec2 MapData::COORD_LEFT       { -1,  0 };
//const glm::ivec2 MapData::COORD_RIGHT      {  1,  0 };
//const glm::ivec2 MapData::COORD_UP_RIGHT   {  1, -1 };
//const glm::ivec2 MapData::COORD_UP_LEFT    { -1, -1 };
//const glm::ivec2 MapData::COORD_DOWN_RIGHT {  1,  1 };
//const glm::ivec2 MapData::COORD_DOWN_LEFT  { -1,  1 };

//const float STAND_BIAS_ON_STAIR = 0.135f;


MapData::MapData(int id)
	: MapData(std::make_shared<MapDataShared>(), id)
{

}

MapData::MapData(std::shared_ptr<MapDataShared> pInfo, int id)
	: m_pInfo(pInfo)
{
	m_pInfo->id = id;
}

MapData::~MapData(void)
{

}

//bool MapData::resizeMap(const glm::ivec2& mapDim)
//{
//	m_mapDim = mapDim;
//	m_pFlightPathfinding.reset(new FlightPathfindingEx(this));
//	m_pPathfinding      .reset(new       PathfindingEx(this));
//
//	return onResize(mapDim);
//}
//
//bool MapData::resizeEmptyMap(const glm::ivec2& mapDim)
//{
//	if (mapDim.x != m_mapDim.x || mapDim.y != m_mapDim.y)
//	{
//		m_mapDim = mapDim;
//		m_pFlightPathfinding.reset(new FlightPathfindingEx(this));
//		m_pPathfinding.reset(new       PathfindingEx(this));
//
//		return onResize(mapDim);
//	}
//
//	return true;
//}
//
//uint MapData::getBasicTile(const glm::ivec2& coord) const
//{
//	if (isOverflow(coord)) return MapCfg::TILE_EMPTY;
//	return MapCfg::decodeBasicTile(at(coord));
//}
//
//uint MapData::getMineralTile(const glm::ivec2& coord) const
//{
//	if (isOverflow(coord)) return MapCfg::TILE_EMPTY;
//	return MapCfg::decodeMineralTile(at(coord));
//}
//
//uint MapData::getBackTile(const glm::ivec2& coord) const
//{
//	if (isOverflow(coord)) return MapCfg::TILE_EMPTY;
//	return MapCfg::decodeBackTile(at(coord));
//}
//
//uint MapData::getBuildingTile(const glm::ivec2& coord) const
//{
//	if (isOverflow(coord)) return MapCfg::TILE_EMPTY;
//	return MapCfg::decodeBuildingTile(at(coord));
//}
//
//uint MapData::getPlantTile(const glm::ivec2& coord) const
//{
//	if (isOverflow(coord)) return MapCfg::TILE_EMPTY;
//	return MapCfg::decodePlantTile(at(coord));
//}
//
//uint MapData::getLiquidTileType(const glm::ivec2& coord) const
//{
//	if (isOverflow(coord)) return{};
//	return MapCfg::decodeLiquidTileType(at(coord));
//}
//
//uint MapData::getLiquidTileType(const glm::vec3& worldPos) const
//{
//	glm::ivec2 coord;
//	if (!worldPosToCoordinate(coord, worldPos)) return{};
//	return getLiquidTileType(coord);
//}
//
//uint MapData::getLiquidTileLevel(const glm::ivec2& coord) const
//{
//	if (isOverflow(coord)) return{};
//	return MapCfg::decodeLiquidTileLevel(at(coord));
//}
//
//bool MapData::getLiquidTopSurfaceWorldPos(glm::vec3& worldPos, const glm::ivec2& coord)
//{
//	uint liquidLevel = MapCfg::decodeLiquidTileLevel(at(coord));
//	if (liquidLevel <= 0) return false;
//
//	coordinateToWorldPos(worldPos, coord);
//	worldPos.y = float{ worldPos.y - MapCfg::HALF_TILE_SIZE + MapCfg::TILE_SIZE * liquidLevel / MapCfg::LIQUID_LEVEL_MAX };
//	return true;
//}
//
//bool MapData::getStepOffsetWorldPos(const glm::vec3& worldPos, float roleWidth, float roleHeight, float& stepOffset)
//{
//	stepOffset = 0.f;
//
//	glm::ivec2 coord{};
//	if (!worldPosToCoordinate(coord, worldPos)) return false;
//
//	for (size_t i = 0; i <= 1; ++i)
//	{
//		coord.y += i;
//		if (isOverflow(coord)) return false;
//
//		if (MapCfg::isBuildingTile(at(coord)))
//		{
//			auto buildingTemplateId = getBuildingTile(findPlacePos(coord));
//			auto pBuildingTemplate = BuildingCfg::getInstance().findBuildingTemplate(buildingTemplateId);
//			auto pBuilding = BuildingCfg::getInstance().findBuildingTypeByTemplateId(buildingTemplateId);
//			if (pBuilding && pBuildingTemplate && pBuilding->m_isClimb)
//			{
//				glm::vec3 leftPoint{};
//				if (!coordinateToWorldPos(leftPoint, coord)) return false;
//				leftPoint.x -= MapCfg::HALF_TILE_SIZE;
//
//				switch (pBuildingTemplate->m_direction)
//				{
//				case BuildingCfg::Direction::RIGHT:;
//					stepOffset = (leftPoint.x + pBuilding->m_wide - worldPos.x) * pBuilding->m_high / pBuilding->m_wide;
//					break;
//
//				case BuildingCfg::Direction::LEFT:
//					stepOffset = (worldPos.x - leftPoint.x) * pBuilding->m_high / pBuilding->m_wide;
//					break;
//				case BuildingCfg::Direction::NONE:
//				default:
//					break;
//				}
//
//				stepOffset = stepOffset + leftPoint.y - worldPos.y;
//
//				return true;
//			}
//		}
//	}
//
//	return false;
//}
//
//int MapData::getFrontTileHp(const glm::ivec2& coord) const
//{
//	if (isOverflow(coord)) return MapCfg::TILE_HP_MIN;
//
//	auto fullTileValue = at(coord);
//	if (MapCfg::isBasicTile(fullTileValue))
//	{
//		return MapCfg::getInstance().getBasicTileHp(fullTileValue);
//	}
//	else if (MapCfg::isMineralTile(fullTileValue))
//	{
//		return MapCfg::getInstance().getMineralTileHp(fullTileValue);
//	}
//	else if (MapCfg::isPlantTile(fullTileValue))
//	{
//		auto pPlantTemplate = PlantCfg::getInstance().findPlantTemplate(MapCfg::decodePlantTile(fullTileValue));
//		if (pPlantTemplate) return pPlantTemplate->m_hp;
//	}
//	else if (MapCfg::isBuildingTile(fullTileValue))
//	{
//		auto pBuildingType = BuildingCfg::getInstance().findBuildingTypeByTemplateId(MapCfg::decodeBuildingTile(fullTileValue));
//		if (pBuildingType) return pBuildingType->m_hp;
//	}
//
//	return MapCfg::TILE_HP_MIN;
//}
//
//int MapData::getBackTileHp(const glm::ivec2& coord) const
//{
//	if (isOverflow(coord)) return MapCfg::TILE_HP_MIN;
//	return MapCfg::getInstance().getBackTileHp(at(coord));
//}
//
//bool MapData::isBasicTile(const glm::ivec2& coord) const
//{
//	if (isOverflow(coord)) return false;
//	return MapCfg::isBasicTile(at(coord));
//}
//
//bool MapData::isDontDigBasicTile(const glm::ivec2& coord) const {
//	if (!isBasicTile(coord)) {
//		return false;
//	}
//	glm::ivec2 placeCoord = findPlacePos(coord);
//	auto templateId = getBasicTile(placeCoord);
//
//	auto& basicTileInfoMap = MapCfg::getInstance().getBasicTileInfoMap();
//	auto&& it = basicTileInfoMap.find(templateId);
//	if (it != basicTileInfoMap.end()) {
//		if (it->second.m_digLevel == 99)
//		{
//			return true;
//		}
//	}
//	return false;
//}
//
//bool MapData::isMineralTile(const glm::ivec2& coord) const
//{
//	if (isOverflow(coord)) return false;
//	return MapCfg::isMineralTile(at(coord));
//}
//
//bool MapData::isPlatformTile(const glm::ivec2& coord) const
//{
//	if (isOverflow(coord))
//	{
//		return false;
//	}
//	if (MapCfg::checkTileFlag(at(coord), MapCfg::IS_BLOCKING))
//	{
//		auto buildingTemplateId = getBuildingTile(findPlacePos(coord));
//		if (!buildingTemplateId)
//		{
//			return false;
//		}
//		auto pBuilding = BuildingCfg::getInstance().findBuildingTypeByTemplateId(buildingTemplateId);
//		if (!pBuilding)
//		{
//			return false;
//		}
//		if (pBuilding->m_isPlane)
//		{
//			return true;
//		}
//	}
//
//	return false;
//}
//
//bool MapData::isBlockingTile(const glm::ivec2& coord, const glm::ivec2& dir, bool platformBlocking) const
//{
//	if (isOverflow(coord)) return true;
//
//	if (MapCfg::checkTileFlag(at(coord), MapCfg::IS_BLOCKING))
//	{
//		auto buildingTemplateId = getBuildingTile(findPlacePos(coord));
//		if (!buildingTemplateId) return true;
//		auto pBuilding = BuildingCfg::getInstance().findBuildingTypeByTemplateId(buildingTemplateId);
//		if (!pBuilding) return true;
//		if (m_isForceFindingPath && pBuilding->n_door == 1)
//		{
//			return false;
//		}
//		auto pBuildingTemplate = BuildingCfg::getInstance().findBuildingTemplate(buildingTemplateId);
//		if (!pBuildingTemplate) {
//			return true;
//		}
//		if (pBuilding->m_isClimb)
//		{
//			if ((dir.x > 0.f && BuildingCfg::Direction::LEFT == pBuildingTemplate->m_direction)
//				|| (dir.x < 0.f && BuildingCfg::Direction::RIGHT == pBuildingTemplate->m_direction)) {
//				return true;
//			}
//		}
//		if (pBuilding->m_isPlane)
//			return !(dir.y <= 0 || !platformBlocking);
//		else if (m_throughBuilding)
//			return pBuilding->m_bigTypeId != BuildingCfg::BUILDING_BIG_TYPE_DOOR;
//		else
//			return true;
//	}
//	return false;
//}
//
//bool MapData::isPlantTile(const glm::ivec2& coord) const
//{
//	if (isOverflow(coord)) return false;
//	return MapCfg::isPlantTile(at(coord));
//}
//
//bool MapData::isTree(const glm::ivec2& coord) const
//{
//	if (!isPlantTile(coord)) return false;
//
//	auto pType = PlantCfg::getInstance().findPlantTypeByTemplateId(getPlantTile(coord));
//	if (!pType) return false;
//
//	return pType->m_growType == PlantCfg::GrowType::TREE;
//}
//
//bool MapData::isBuildingTile(const glm::ivec2& coord) const
//{
//	if (isOverflow(coord)) return false;
//	return MapCfg::isBuildingTile(at(coord));
//}
//
//bool MapData::isAnyCoordBuildingTile(const glm::ivec2& anyCoord) const
//{
//	return isBuildingTile(findPlacePos(anyCoord));
//}
//
//bool MapData::isFrontTileEmpty(const glm::ivec2& coord) const
//{
//	if (isOverflow(coord)) return true;
//
//	auto tile = at(coord);
//	return MapCfg::getFrontTile(tile) == MapCfg::TILE_EMPTY/* && MapCfg::decodeLiquidTileLevel(tile) == MapCfg::TILE_EMPTY*/;
//}
//
//bool MapData::isFrontTileCompleteEmpty(const glm::ivec2& coord) const
//{
//	if (isOverflow(coord)) return true;
//
//	auto tile = at(coord);
//	return MapCfg::getFrontTile(tile) == MapCfg::TILE_EMPTY && MapCfg::decodeLiquidTileLevel(tile) == MapCfg::TILE_EMPTY;
//}
//
//bool MapData::isBackTileEmpty(const glm::ivec2& coord) const
//{
//	if (isOverflow(coord)) return true;
//	return MapCfg::decodeBackTile(at(coord)) == MapCfg::TILE_EMPTY;
//}
//
//glm::vec2 MapData::GetStandableNotInBlockingBasicTile(const glm::ivec2& coordTarget, int nRoleWidth, int nRoleHeight, bool platformBlocking, int nRangeWidth, int nRangeHeight, int nRangeLoopNum)
//{
//	int nWidth = nRangeWidth;
//	int nHeight = nRangeHeight;
//	glm::ivec2 coordRole;
//	coordRole.x = coordTarget.x;
//	coordRole.y = coordTarget.y;
//	bool bFindStandable = false;
//	for (int i = 0; i < nRangeLoopNum && !bFindStandable; ++i)
//	{
//		for (int nX = 0; nX < nWidth && !bFindStandable; ++nX)
//		{
//			for (int nDirX = -1; nDirX < 2 && !bFindStandable; nDirX = nDirX + 2)
//			{
//				for (int nY = 0; nY < nHeight && !bFindStandable; ++nY)
//				{
//					coordRole.x = coordTarget.x + nX * nDirX + i * nWidth;
//					coordRole.y = coordTarget.y - nY - i * nHeight;
//					if (!isOverflow(coordRole) && !isInBlockingBasicTile(coordRole, nRoleWidth * 1.f, nRoleHeight * 1.f, true) && isStandable(coordRole, nRoleWidth * 1.f, nRoleHeight * 1.f, true))
//					{
//						bFindStandable = true;
//						break;
//					}
//				}
//			}
//		}
//	}
//	if (!bFindStandable)
//	{
//		coordRole.x = coordTarget.x;
//		coordRole.y = coordTarget.y;
//	}
//
//	return coordRole;
//}
//
//glm::vec2 MapData::GetNotInBlockingBasicTile(const glm::ivec2& coordTarget, int nRoleWidth, int nRoleHeight, bool platformBlocking, int nRangeWidth, int nRangeHeight)
//{
//	return GetStandableNotInBlockingBasicTile(coordTarget, nRoleWidth, nRoleHeight, platformBlocking, nRangeWidth, nRangeHeight, 10);
//}
//
//bool MapData::isInBlockingBasicTile(const glm::ivec2& coordRole, float roleWidth, float roleHeight, bool platformBlocking) const
//{
//	glm::ivec2 coordClient;
//	coordClient.x = coordRole.x;
//	coordClient.y = coordRole.y;
//	int nHalfRoleWidth = (int)ceil((float)roleWidth / 2.0f);
//	for (int nY = 0; nY < roleHeight; ++nY)
//	{
//		for (int nDirX = -1; nDirX < 2; nDirX = nDirX + 2)
//		{
//			for (int nX = 0; nX < nHalfRoleWidth; ++nX)
//			{
//				coordClient.x = coordRole.x + nX * nDirX;
//				coordClient.y = coordRole.y - nY;
//				if (isOverflow(coordClient) || isBlockingTile(coordClient, glm::ivec2{ 0, 0 }, platformBlocking) || isBasicTile(coordClient))
//				{
//					return true;
//				}
//			}
//		}
//	}
//	return false;
//}
//
//bool MapData::isStandable(const glm::ivec2& coord, float roleWidth, float roleHeight, bool platformBlocking) const
//{
//	auto width = static_cast<int>(round(roleWidth));
//	bool hasBlock{};
//	for (int w = 0; w < width; ++w)
//	{
//		hasBlock = isBlockingTile(coord + glm::ivec2{ w, 1 }, { 0, 1 }, platformBlocking);
//		if (hasBlock) { break; }
//	}
//	if (!hasBlock) return false;
//
//	return isPassable(coord, {}, roleWidth, roleHeight, platformBlocking);
//}
//
//bool MapData::isStandable(const glm::vec3 & pos, float roleWidth, float roleHeight, bool platformBlocking) const
//{
//	glm::ivec2 coord;
//	if (!MapObj::worldPosToCoordinate(coord, pos, roleWidth)) return false;
//	return isStandable(coord, roleWidth, roleHeight, platformBlocking);
//}
//
//bool MapData::isHangUpsideDownAble(const glm::ivec2& coord)
//{
//	return isFrontTileEmpty(coord) && isBlockingTile(coord + glm::ivec2(0, -1), { 0,1 });
//}
//
//bool MapData::isStandablePlatformBlocking(const glm::ivec2& coord, float roleWidth, float roleHeight) const
//{
//	return isStandable(coord, roleWidth, roleHeight, true);
//}
//
//bool MapData::isPassable(const glm::ivec2& coord, const glm::ivec2& dir, float roleWidth, float roleHeight, bool platformBlocking) const
//{
//	auto width = static_cast<int>(round(roleWidth));
////	auto height = static_cast<int>(std::ceil(roleHeight));
//	for (int w = 0; w < width; w++) for (int h = 0; h < roleHeight; ++h)
//	{
//		if (isBlockingTile(coord + glm::ivec2{ w, -h }, dir, platformBlocking)) return false;
//	}
//
//	return true;
//}
//
//
//bool MapData::isPassable(const glm::vec3& pos, const glm::ivec2& dir, float roleWidth, float roleHeight) const
//{
//	glm::ivec2 coord;
//	if (!worldPosToCoordinate(coord, pos)) return false;
//	return isPassable(coord, dir, roleWidth, roleHeight);
//}
//
//bool MapData::isClimbable(const glm::vec3& pos, const glm::ivec2& dir) const
//{
//	glm::ivec2 coord;
//	if (!worldPosToCoordinate(coord, pos)) return false;
//	return isClimbable(coord, dir);
//}
//
//bool MapData::isClimbable(const glm::ivec2& coord, const glm::ivec2& dir) const
//{
//	if (isClimbableTile(coord))
//	{
//		auto buildingTemplateId = getBuildingTile(findPlacePos(coord));
//		auto pBuilding = BuildingCfg::getInstance().findBuildingTypeByTemplateId(buildingTemplateId);
//		auto pBuildingTemplate = BuildingCfg::getInstance().findBuildingTemplate(buildingTemplateId);
//		if (pBuilding && pBuildingTemplate && pBuilding->m_isClimb)
//		{
//			// standable if move dir opposite building dir, otherwise passable
//			return (dir.x > 0.f && BuildingCfg::Direction::LEFT == pBuildingTemplate->m_direction)
//				|| (dir.x < 0.f && BuildingCfg::Direction::RIGHT == pBuildingTemplate->m_direction);
//		}
//	}
//
//	return false;
//}
//
//bool MapData::isClimbableTile(const glm::ivec2& coord) const
//{
//	if (isOverflow(coord)) return false;
//
//	if (MapCfg::isBuildingTile(at(coord)))
//	{
//		auto pBuilding = BuildingCfg::getInstance().findBuildingTypeByTemplateId(getBuildingTile(findPlacePos(coord)));
//		return (pBuilding && pBuilding->m_isClimb);
//	}
//	return false;
//}
//
//bool MapData::isTargetTypeTile(const glm::ivec2& coord, unsigned int typyId) const
//{
//	if (!isBasicTile(coord))
//	{
//		return false;
//	}
//
//	glm::ivec2 placeCoord = findPlacePos(coord);
//	auto templateId = getBasicTile(placeCoord);
//
//	auto& basicTileInfoMap = MapCfg::getInstance().getBasicTileInfoMap();
//	auto&& it = basicTileInfoMap.find(templateId);
//	if (it != basicTileInfoMap.end())
//	{
//		if (it->second.m_typeId == typyId)
//		{
//			return true;
//		}
//	}
//	return false;
//}
//
//bool MapData::isConnected(const glm::ivec2& src, const glm::ivec2& dst)
//{
//	if (src == dst) return true;
//
//	int srcX{ src.x }, srcY{ src.y }, dstX{ dst.x }, dstY{ dst.y };
//	if (srcY < dstY)	// bottom to top
//	{
//		std::swap(srcX, dstX);
//		std::swap(srcY, dstY);
//	}
//	int deltaX{ dstX - srcX }, deltaY{ dstY - srcY };
//	int loopCnt{std::max(std::abs(deltaX), -deltaY)};
//	int oldX{ srcX }, oldY{ srcY };
//	for (int i{}; i < loopCnt; ++i)
//	{
//		int x{ srcX + static_cast<int>(std::ceil(static_cast<float>(deltaX) * i / loopCnt)) };
//		int y{ srcY + static_cast<int>(std::ceil(static_cast<float>(deltaY) * i / loopCnt)) };
//		if (isBlockingTile({ x, y }, {})) return false;
//		if (x != oldX && y != oldY && isBlockingTile({ x, oldY }, {}) && isBlockingTile({ oldX, y }, {}))
//		{
//			return false;
//		}
//	}
//
//	return true;
//}
//
//bool MapData::isBlocked(const glm::vec3& src, float srcWidth, float srcHeight, const glm::vec3& dst, float dstWidth, float dstHeight)
//{
//	if (src == dst)
//	{
//		return false;
//	}
//
//	float ax = std::min(src.x, dst.x), ay = std::min(src.y, dst.y), bx = std::max(src.x + srcWidth, dst.x + dstWidth), by = std::max(src.y + srcHeight, dst.y + dstHeight);
//	if (static_cast<int>(by - ay) == 0 && static_cast<int>(bx - ax) == 0)
//	{
//		return false;
//	}
//	int loopCnt = static_cast<int>(by - ay);
//
//	bool bRayThroughX = true;
//	for (int i{}; i < loopCnt; ++i)
//	{
//		if (isRayThrough({ ax, ay + i, 0 }, { bx, ay + i, 0 }))
//		{
//			bRayThroughX = false;
//		}
//	}
//
//	loopCnt = static_cast<int>(bx - ax);
//	bool bRayThroughY = true;
//	for (int i{}; i < loopCnt; ++i)
//	{
//		if (isRayThrough({ ax+i, ay, 0 }, { ax + i, by, 0 }))
//		{
//			bRayThroughY = false;
//		}
//	}
//
//	if (!bRayThroughX && !bRayThroughY)
//	{
//		return false;
//	}
//
//	return true;
//}
//
//bool MapData::isRayThrough(const glm::vec3& src, const glm::vec3& dst)
//{
//	if (src == dst) return true;
//
//	float deltaX = dst.x - src.x, deltaY = dst.y - src.y;
//	int loopCnt = static_cast<int>(std::max(std::abs(deltaX), std::abs(deltaY)) / MapCfg::TILE_SIZE) + 1;
//	glm::ivec2 coord;
//	for (int i{}; i < loopCnt; ++i)
//	{
//		auto pre = coord;
//		if (worldPosToCoordinate(coord, { src.x + deltaX * i / loopCnt, src.y + deltaY * i / loopCnt, 0 }) && isBlockingTile(coord, {}))
//		{
//			return false;
//		}
//
//		if (i != 0 && coord.x != pre.x && coord.y != pre.y)
//		{
//			if (isBlockingTile(glm::ivec2(coord.x, pre.y), {}) && isBlockingTile(glm::ivec2(pre.x, coord.y), {}))
//			{
//				return false;
//			}
//		}
//	}
//
//	return true;
//}
//
//bool MapData::isStaticObjTile(const glm::ivec2& coord) const
//{
//	if (isOverflow(coord)) return false;
//	return MapCfg::isStaticTile(at(coord));
//}
//
//bool MapData::isWarFog(const glm::ivec2& coord) const
//{
//	if (isOverflow(coord)) return false;
//	return MapCfg::isWarFog(at(coord));
//}
//
//bool MapData::isBluePrint(const glm::ivec2& coord) const
//{
//	if (isOverflow(coord)) return false;
//	return MapCfg::isBluePrint(at(coord));
//}
//
//bool MapData::isBluePrintBack(const glm::ivec2& coord) const
//{
//	if (isOverflow(coord)) return false;
//	return MapCfg::isBluePrintBack(at(coord));
//}
//
//bool MapData::setBasicTile(uint basicTileValue, const glm::ivec2& coord)
//{
//	if (isOverflow(coord)) return false;
//
//	auto& tile = at(coord);
//	auto finalValue = MapCfg::encodeLiquidTile(0, 0, tile);
//	finalValue = MapCfg::encodeBasicTile(basicTileValue, finalValue);
//	if (basicTileValue == MapCfg::TILE_EMPTY) finalValue.flags &= (~MapCfg::IS_BLOCKING);
//	if (finalValue == tile) return false;
//
//	tile = finalValue;
//	return true;
//}
//
//bool MapData::setLiquidTile(uint liquidType, uint liquidTileLevel, const glm::ivec2& coord)
//{
//	if (isOverflow(coord)) return false;
//
//	auto& tile = at(coord);
//	if (MapCfg::isBlocking(tile)) return false;
//	auto finalValue = MapCfg::encodeLiquidTile(liquidType, liquidTileLevel, tile);
//	if (finalValue == tile) return false;
//
//	tile = finalValue;
//	return true;
//}
//
//bool MapData::setMineralTile(uint mineralTileValue, const glm::ivec2& coord)
//{
//	if (isOverflow(coord)) return false;
//
//	auto& tile = at(coord);
//	uint basicTileValue = MapCfg::decodeBasicTile(tile);
//	if (basicTileValue == MapCfg::TILE_EMPTY) return false;
//	auto finalValue = MapCfg::encodeMineralTile(basicTileValue, mineralTileValue, tile);
//	if (finalValue == tile) return false;
//
//	tile = finalValue;
//	return true;
//}
//
//bool MapData::setExtraTile(uint extraTileValue, const glm::ivec2& coord)
//{
//	if (isOverflow(coord)) return false;
//
//	auto& tile = at(coord);
//	auto finalValue = MapCfg::encodeExtraTile(extraTileValue, tile);
//	if (extraTileValue == MapCfg::TILE_EMPTY) finalValue.flags &= (~MapCfg::IS_BLOCKING);
//	if (finalValue == tile) return false;
//
//	tile = finalValue;
//	return true;
//}
//
//bool MapData::setBackTile(uint backTileValue, const glm::ivec2& coord)
//{
//	if (isOverflow(coord)) return false;
//
//	auto& tile = at(coord);
//	auto finalValue = MapCfg::encodeBackTile(backTileValue, tile);
//	if (finalValue == tile) return false;
//
//	tile = finalValue;
//	return true;
//}
//
//bool MapData::setBlockingTile(const glm::ivec2& coord, bool bBlockLiquid)
//{
//	if (isOverflow(coord)) return false;
//
//	auto& tile = at(coord);
//	if (bBlockLiquid)
//	{
//		auto finalValue = MapCfg::encodeLiquidTile(0, 0, tile);
//		finalValue = MapCfg::setTileFlag(finalValue, MapCfg::IS_BLOCKING);
//		if (finalValue == tile) return false;
//		tile = finalValue;
//		return true;
//	}
//	else
//	{
//		auto finalValue = MapCfg::setTileFlag(tile, MapCfg::IS_BLOCKING);
//		if (finalValue == tile) return false;
//		tile = finalValue;
//		return true;
//	}
//}
//
//bool MapData::setWarFog(const glm::ivec2& coord, bool bVisble)
//{
//	if (isOverflow(coord)) return false;
//
//	auto& tile = at(coord);
//	auto finalValue = MapCfg::encodeWarFogTile(bVisble ? 1 : 0, tile);
//	if (finalValue == tile) return false;
//
//	tile = finalValue;
//	return true;
//}
//
//bool MapData::setBlueprint(const glm::ivec2& coord, bool bVisble)
//{
//	if (isOverflow(coord)) return false;
//
//	auto& tile = at(coord);
//	auto finalValue = MapCfg::encodeBlueprintTile(bVisble ? 1 : 0, tile);
//	if (finalValue == tile) return false;
//
//	tile = finalValue;
//	return true;
//}
//
//bool MapData::setBlueprintBack(const glm::ivec2& coord, bool bVisble)
//{
//	if (isOverflow(coord)) return false;
//
//	auto& tile = at(coord);
//	auto finalValue = MapCfg::encodeBlueprintBackTile(bVisble ? 1 : 0, tile);
//	if (finalValue == tile) return false;
//
//	tile = finalValue;
//	return true;
//}
//
//bool MapData::setPlacePosOffset(const glm::ivec2& offset, const glm::ivec2& coord)
//{
//	if (isOverflow(coord)) return false;
//
//	auto& tile = at(coord);
//	auto finalValue = MapCfg::encodePlacePosOffset(offset, tile);
//	if (finalValue == tile) return false;
//
//	tile = finalValue;
//	return true;
//}
//
//bool MapData::clearFrontTile(const glm::ivec2& coord)
//{
//	if (isOverflow(coord)) return false;
//
//	auto& tile = at(coord);
//
//	auto extraValue = MapCfg::getInstance().decodeExtraFromTile(tile);
//	if (MapCfg::isBuildingTile(tile))
//	{
//		return clearBuildingTiles(coord, extraValue);
//	}
//	else if (MapCfg::isPlantTile(tile))
//	{
//		return clearPlantTiles(coord, extraValue);
//	}
//	else
//	{
//		auto finalValue = MapCfg::clearFrontTile(tile);
//		if (finalValue == tile) return false;
//
//		tile = finalValue;
//		return true;
//	}
//}
//
//bool MapData::clearBasicTile(const glm::ivec2& coord)
//{
//	if (!isBasicTile(coord)) return false;
//
//	auto& tile = at(coord);
//	tile = MapCfg::clearFrontTile(tile);
//
//	return true;
//}
//
//bool MapData::clearBackTile(const glm::ivec2& coord)
//{
//	if (isOverflow(coord)) return false;
//
//	auto& tile = at(coord);
//	auto finalValue = MapCfg::clearBackTile(tile);
//	if (finalValue == tile) return false;
//
//	tile = finalValue;
//	return true;
//}
//
//bool MapData::clearBlockingTile(const glm::ivec2& coord)
//{
//	if (isOverflow(coord)) return false;
//
//	auto& tile = at(coord);
//	auto finalValue = MapCfg::clearTileFlag(tile, MapCfg::IS_BLOCKING);
//	if (finalValue == tile) return false;
//
//	tile = finalValue;
//	return true;
//}
//
//bool MapData::clearBuildingTiles(const glm::ivec2& coord, uint templateId)
//{
//	if (!isBuildingTile(coord)) return false;
//
//	// clear coord
//	at(coord) = MapCfg::clearFrontTile(at(coord));
//
//	const auto* pBuildingTemplate = BuildingCfg::getInstance().findBuildingTemplate(templateId);
//	if (pBuildingTemplate)
//	{
//		// clear offsetCoord
//		for (const auto& offset : pBuildingTemplate->m_offsetList)
//		{
//			glm::ivec2 coordClear = coord + offset;
//
//			if (offset != IVEC2_ZERO)
//			{
//				uint otherTemplateId = MapCfg::decodeExtraFromTile(at(coordClear));
//				if (otherTemplateId != MapCfg::TILE_EMPTY)
//				{
//					LOG_EVENT_E("Other object place coord=({0},{1}) templateId={2}", coordClear.x, coordClear.y, otherTemplateId);
//					continue;
//				}
//			}
//
//			at(coordClear) = MapCfg::clearFrontTile(at(coordClear));
//		}
//	}
//	else
//	{
//		LOG_EVENT_E("Building templateId not found: coord=({0},{1}) templateId={2}", coord.x, coord.y, templateId);
//	}
//
//	return true;
//}
//
//bool MapData::clearPlantTiles(const glm::ivec2& coord, uint templateId)
//{
//	if (!isPlantTile(coord)) return false;
//
//	// clear coord
//	at(coord) = MapCfg::clearFrontTile(at(coord));
//
//	const auto* pPlantTemplate = PlantCfg::getInstance().findPlantTemplate(templateId);
//	if (pPlantTemplate)
//	{
//		// clear offsetCoord
//		for (const auto& offset : pPlantTemplate->m_offsetList)
//		{
//			glm::ivec2 coordClear = coord + offset;
//
//			if (offset != IVEC2_ZERO)
//			{
//				uint otherTemplateId = MapCfg::decodeExtraFromTile(at(coordClear));
//				if (otherTemplateId != MapCfg::TILE_EMPTY)
//				{
//					LOG_EVENT_E("Other object place coord=({0},{1}) templateId={2}", coordClear.x, coordClear.y, otherTemplateId);
//					continue;
//				}
//			}
//
//			at(coordClear) = MapCfg::clearFrontTile(at(coordClear));
//		}
//	}
//	else
//	{
//		LOG_EVENT_E("Plant templateId not found: coord=({0},{1}) templateId={2}", coord.x, coord.y, templateId);
//	}
//
//	return true;
//}
//
//void MapData::WorldPosToCoordinateNotOverflow(glm::ivec2& coordOut, const glm::vec3& worldPos) const
//{
//	float fx = (getCoordOrigin().x * MapCfg::TILE_SIZE + worldPos.x) / MapCfg::TILE_SIZE;
//	if (fx < 0.0f) fx -= 0.5f; else fx += 0.5f;
//
//	float fy = (getCoordOrigin().y * MapCfg::TILE_SIZE - worldPos.y) / MapCfg::TILE_SIZE;
//	if (fy < 0.0f) fy -= 0.5f; else fy += 0.5f;
//
//	coordOut.x = static_cast<int>(fx);
//	coordOut.y = static_cast<int>(fy);
//	if (coordOut.x < 0)
//	{
//		coordOut.x = 0;
//	}
//	if (coordOut.x >= m_mapDim.x)
//	{
//		coordOut.x = m_mapDim.x - 1;
//	}
//	if (coordOut.y < 0)
//	{
//		coordOut.y = 0;
//	}
//	if (coordOut.y >= m_mapDim.y)
//	{
//		coordOut.y = m_mapDim.y - 1;
//	}
//}
//
//bool MapData::worldPosToCoordinate(glm::ivec2& coordOut, const glm::vec3& worldPos) const
//{
//	float fx = (getCoordOrigin().x * MapCfg::TILE_SIZE + worldPos.x) / MapCfg::TILE_SIZE;
//	if (fx < 0.0f) fx -= 0.5f; else fx += 0.5f;
//
//	float fy = (getCoordOrigin().y * MapCfg::TILE_SIZE - worldPos.y) / MapCfg::TILE_SIZE;
//	if (fy < 0.0f) fy -= 0.5f; else fy += 0.5f;
//
//	coordOut.x = static_cast<int>(fx);
//	coordOut.y = static_cast<int>(fy);
//	return !isOverflow(coordOut);
//}
//
//int32 MapData::WorldToCoordX(const float WorldX)
//{
//	float fx = (getCoordOrigin().x * MapCfg::TILE_SIZE + WorldX) / MapCfg::TILE_SIZE;
//	if (fx < 0.0f) fx -= 0.5f; else fx += 0.5f;
//
//	return static_cast<int>(fx);
//}
//int32 MapData::WorldToCoordY(const float WorldY)
//{
//	float fy = (getCoordOrigin().y * MapCfg::TILE_SIZE - WorldY) / MapCfg::TILE_SIZE;
//	if (fy < 0.0f) fy -= 0.5f; else fy += 0.5f;
//
//	return static_cast<int>(fy);
//}
//
//bool MapData::coordinateToWorldPos(glm::vec3& worldPosOut, const glm::ivec2& coord) const
//{
//	if (isOverflow(coord))
//		return false;
//	worldPosOut.x = static_cast<float>( coord.x * MapCfg::TILE_SIZE - getCoordOrigin().x * MapCfg::TILE_SIZE);
//	worldPosOut.y = static_cast<float>(-coord.y * MapCfg::TILE_SIZE + getCoordOrigin().y * MapCfg::TILE_SIZE);
//
//	return true;
//}
//
//int32 MapData::CoordToWorldX(const int32 CoordX)
//{
//	return static_cast<int32>(CoordX * MapCfg::TILE_SIZE - getCoordOrigin().x * MapCfg::TILE_SIZE);
//}
//int32 MapData::CoordToWorldY(const int32 CoordY)
//{
//	return static_cast<int32>(-CoordY * MapCfg::TILE_SIZE + getCoordOrigin().y * MapCfg::TILE_SIZE);
//}
//
//glm::vec3 MapData::coordToWorldPos(const glm::ivec2& coord) const
//{
//	glm::vec3 ret;
//	coordinateToWorldPos(ret, coord);
//	return ret;
//}
//
//glm::ivec2 MapData::worldPosToCoord(const glm::vec3& worldPos) const
//{
//	glm::ivec2 ret;
//	worldPosToCoordinate(ret, worldPos);
//	return ret;
//}
//
//bool MapData::findLandingCoordinate(glm::ivec2& coordOut, const glm::ivec2& coord) const
//{
//	if (coord.x < 0 || coord.x >= m_mapDim.x) return false;
//
//	int coordY = coord.y;
//	for (; coordY < m_mapDim.y; ++coordY)
//	{
//		if (isOverflow(coord.x, coordY)) return false;
//		if (MapCfg::checkTileFlag(at(coord.x, coordY), MapCfg::IS_BLOCKING)) break;
//	}
//	if (--coordY < 0) return false;
//
//	coordOut.x = coord.x;
//	coordOut.y = coordY;
//	return true;
//}
//
//glm::ivec2 MapData::findPlacePos(const glm::ivec2& coordIn, bool isFilterBluePrint/* = true*/) const
//{
//	if (isOverflow(coordIn)) return coordIn;
//	return coordIn - MapCfg::decodePlacePosOffset(at(coordIn), isFilterBluePrint);
//}
//
//void MapData::findBuildingCanAttachCoordList(const glm::ivec2& coord, uint buildingTemplateId, std::vector<std::vector<MapData::CanAttachCoordData>>& canAttachCoordList, const std::function<uint(const glm::ivec2&)>& getSignTypeIdFunc)
//{
//	auto pBuildingTemplate = BuildingCfg::getInstance().findBuildingTemplate(buildingTemplateId);
//	auto pBuildingType = BuildingCfg::getInstance().findBuildingType(pBuildingTemplate->m_typeId);
//	if (!pBuildingTemplate || !pBuildingType) return;
//
//	for (const auto& group : pBuildingTemplate->m_attachList)
//	{
//		std::vector<MapData::CanAttachCoordData> list{};
//
//		for (const auto& attachPair : group.m_pointList)
//		{
//			glm::ivec2 attachPos = coord + attachPair.first;
//
//			if (auto pBuildingAttachmentPoint = BuildingCfg::getInstance().findBuildingAttachmentPoint(attachPair.second))
//			{
//				MapData::CanAttachCoordData cacData;
//				cacData.m_coord = attachPos;
//
//				auto basicTemplateId = getBasicTile(attachPos);
//				if (pBuildingAttachmentPoint->m_isAttachFrontBlock && basicTemplateId)
//				{
//					MapData::CanAttachCoordType cacType{};
//					cacType.m_tileType = MapCfg::MinorTileType::Basic;
//					cacType.m_templateId = basicTemplateId;
//					cacData.m_typeList.emplace_back(cacType);
//				}
//
//				auto backTemplateId = getBackTile(attachPos);
//				if (pBuildingAttachmentPoint->m_isAttachBackBlock && backTemplateId)
//				{
//					MapData::CanAttachCoordType cacType{};
//					cacType.m_tileType = MapCfg::MinorTileType::Back;
//					cacType.m_templateId = backTemplateId;
//					cacData.m_typeList.emplace_back(cacType);
//				}
//
//				auto attachPosBuildingTemplateId = getBuildingTile(findPlacePos(attachPos));
//
//				auto attachPosPBuildingType = BuildingCfg::getInstance().findBuildingTypeByTemplateId(attachPosBuildingTemplateId);
//
//				if (!attachPosPBuildingType && pBuildingType->m_isDragPlace && getSignTypeIdFunc)
//				{
//					auto signTypeId = getSignTypeIdFunc(attachPos);
//					attachPosPBuildingType = BuildingCfg::getInstance().findBuildingType(signTypeId);
//				}
//
//				auto end = pBuildingAttachmentPoint->m_attachBuildTypeIds.end();
//				if (attachPosPBuildingType && end != std::find(pBuildingAttachmentPoint->m_attachBuildTypeIds.begin(), end, attachPosPBuildingType->m_typeId))
//				{
//					MapData::CanAttachCoordType cacType{};
//					cacType.m_tileType = MapCfg::MinorTileType::Building;
//					cacType.m_templateId = attachPosBuildingTemplateId;
//					cacData.m_typeList.emplace_back(cacType);
//				}
//
//				if (!cacData.m_typeList.empty()) list.emplace_back(cacData);
//			}
//		}
//
//		if (list.size() == group.m_pointList.size()) canAttachCoordList.emplace_back(list);
//	}
//}
//
//void MapData::findBuildingBottomCoordList(Ivec2List& outCoordList, const glm::ivec2& coord)
//{
//	auto pBuildingTemplate = BuildingCfg::getInstance().findBuildingTemplate(getBuildingTile(coord));
//	if (!pBuildingTemplate) return;
//
//	decltype(pBuildingTemplate->m_offsetList) posList{ pBuildingTemplate->m_offsetList };
//	posList.emplace_back(IVEC2_ZERO);
//
//	for (int i = 0, offsetX = pBuildingTemplate->m_minOffsetX; offsetX <= pBuildingTemplate->m_maxOffsetX; ++offsetX, ++i)
//	{
//		int offsetY = pBuildingTemplate->m_minOffsetY;
//		for (const auto& off : posList)
//		{
//			if (off.x == offsetX) offsetY = std::max(offsetY, off.y);
//		}
//		outCoordList.emplace_back(coord + glm::ivec2(offsetX, offsetY));
//	}
//}
//
//void MapData::findBuildingCurrPosList(Ivec2List& outPosList, const glm::ivec2& coord)
//{
//	auto pBuildingTemplate = BuildingCfg::getInstance().findBuildingTemplate(getBuildingTile(coord));
//	if (!pBuildingTemplate) return;
//
//	outPosList.emplace_back(coord);
//	for (const auto& off : pBuildingTemplate->m_offsetList) outPosList.emplace_back(coord + off);
//}
//
//void MapData::findNearBuildingList(Ivec2List& outPlaceCoordList, const glm::ivec2& coord, MapCfg::MinorTileType tileType)
//{
//	std::vector<glm::ivec2> offsetList{};
//
//	auto&& placeCoord = findPlacePos(coord);
//
//	switch (tileType)
//	{
//	case MapCfg::MinorTileType::Unknown:
//		break;
//	case MapCfg::MinorTileType::Basic:
//	{
//		offsetList = { MapData::COORD_UP, MapData::COORD_DOWN, MapData::COORD_LEFT, MapData::COORD_RIGHT, MapData::COORD_UP_LEFT, MapData::COORD_UP_RIGHT, MapData::COORD_DOWN_LEFT, MapData::COORD_DOWN_RIGHT };
//	}
//		break;
//	case MapCfg::MinorTileType::Back:
//	{
//		offsetList = { NS::IVEC2_ZERO, MapData::COORD_UP, MapData::COORD_DOWN, MapData::COORD_LEFT, MapData::COORD_RIGHT, MapData::COORD_UP_LEFT, MapData::COORD_UP_RIGHT, MapData::COORD_DOWN_LEFT, MapData::COORD_DOWN_RIGHT };
//	}
//		break;
//	case MapCfg::MinorTileType::Building:
//	{
//		glm::ivec2 dirArr[] = { MapData::COORD_UP, MapData::COORD_DOWN, MapData::COORD_LEFT, MapData::COORD_RIGHT, MapData::COORD_UP_LEFT, MapData::COORD_UP_RIGHT, MapData::COORD_DOWN_LEFT, MapData::COORD_DOWN_RIGHT };
//
//		auto&& pBuilding = StaticObjMgr::getInstance().findBuildingByLocalCoord(placeCoord, getLevelIndex());
//		if (pBuilding)
//		{
//			auto&& pBuildingTemplate = pBuilding->getTemplate();
//			if (pBuildingTemplate)
//			{
//				std::vector<glm::ivec2> placeOffsetList{ pBuildingTemplate->m_offsetList };
//
//				placeOffsetList.push_back(NS::IVEC2_ZERO);
//
//				for (auto&& grid : placeOffsetList)
//				{
//					for (auto&& dir : dirArr)
//					{
//						auto&& offset = dir + grid;
//
//						if (placeCoord != findPlacePos(offset + placeCoord))
//						{
//							offsetList.push_back(offset);
//						}
//					}
//				}
//			}
//		}
//	}
//		break;
//	default:
//		break;
//	}
//
//	std::set<glm::ivec2> outSet{};
//
//	for (auto&& offset : offsetList)
//	{
//		auto&& offsetPlaceCoord = findPlacePos(offset + placeCoord);
//		if (isBuildingTile(offsetPlaceCoord))
//		{
//			outSet.insert(offsetPlaceCoord);
//		}
//	}
//
//	outPlaceCoordList.assign(outSet.begin(), outSet.end());
//
//	/*for (const auto& obj : StaticObjMgr::getInstance().getBuildings())
//	{
//		auto&& pBuildingData = obj.second;
//
//		if (pBuildingData->getMapDataLevel() != getLevelIndex())
//		{
//			continue;
//		}
//
//		auto objBuildingPlaceCoord = obj.second->getLocalCoord();
//		if (std::find(outPlaceCoordList.begin(), outPlaceCoordList.end(), objBuildingPlaceCoord) != outPlaceCoordList.end()) continue;
//		if (tileType == MapCfg::MinorTileType::Building && coord == objBuildingPlaceCoord) continue;
//
//		int maxOffsetX{};
//		int minOffsetX{};
//		int maxOffsetY{};
//		int minOffsetY{};
//
//		if (tileType == MapCfg::MinorTileType::Building)
//		{
//			if (auto pBuildingTemplate = BuildingCfg::getInstance().findBuildingTemplate(getBuildingTile(coord)))
//			{
//				maxOffsetX = pBuildingTemplate->m_maxOffsetX;
//				minOffsetX = pBuildingTemplate->m_minOffsetX;
//				maxOffsetY = pBuildingTemplate->m_maxOffsetY;
//				minOffsetY = pBuildingTemplate->m_minOffsetY;
//			}
//		}
//
//		auto pObjBuildingTemplate = obj.second->getTemplate();
//		if (!pObjBuildingTemplate) continue;
//
//		if (pObjBuildingTemplate->m_maxOffsetX + objBuildingPlaceCoord.x < minOffsetX + coord.x - 1 ||
//			pObjBuildingTemplate->m_minOffsetX + objBuildingPlaceCoord.x > maxOffsetX + coord.x + 1 ||
//			pObjBuildingTemplate->m_maxOffsetY + objBuildingPlaceCoord.y < minOffsetY + coord.y - 1 ||
//			pObjBuildingTemplate->m_minOffsetY + objBuildingPlaceCoord.y > maxOffsetY + coord.y + 1)
//		{
//			continue;
//		}
//
//		outPlaceCoordList.emplace_back(objBuildingPlaceCoord);
//	}*/
//}
//
//void MapData::findAutoRemoveBuildingList(Ivec2List& outPlaceCoordList, const glm::ivec2& coord, MapCfg::MinorTileType tileType)
//{
//	uint templateId{};
//
//	if (tileType == MapCfg::MinorTileType::Building)
//	{
//		templateId = getBuildingTile(coord);
//	}
//	else if (tileType == MapCfg::MinorTileType::Basic)
//	{
//		templateId = getBasicTile(coord);
//	}
//	else if (tileType == MapCfg::MinorTileType::Back)
//	{
//		templateId = getBackTile(coord);
//	}
//
//	Ivec2List nearBuildingPlaceCoordList{};
//	findNearBuildingList(nearBuildingPlaceCoordList, coord, tileType);
//
//	for (const auto& buildingPlaceCoord : nearBuildingPlaceCoordList)
//	{
//		auto buildingTemplateId = getBuildingTile(buildingPlaceCoord);
//
//		auto pBuildingTemplate = BuildingCfg::getInstance().findBuildingTemplate(buildingTemplateId);
//		if (!pBuildingTemplate || !pBuildingTemplate->m_isAutoRemove) continue;
//
//		MapData::CanAttachCoordList canAttachCoordList{};
//		findBuildingCanAttachCoordList(buildingPlaceCoord, buildingTemplateId, canAttachCoordList);
//
//		if (canAttachCoordList.empty()) continue;
//
//		for (auto groupIte = canAttachCoordList.begin(); groupIte != canAttachCoordList.end();)
//		{
//			auto& group = *groupIte;
//
//			for (auto attachCoordIte = group.begin(); attachCoordIte != group.end(); ++attachCoordIte)
//			{
//				auto& attachCoord = *attachCoordIte;
//
//				bool isSame = false;
//				if (tileType == MapCfg::MinorTileType::Basic || tileType == MapCfg::MinorTileType::Back)
//				{
//					isSame = coord == attachCoord.m_coord;
//				}
//				else if (tileType == MapCfg::MinorTileType::Building)
//				{
//					isSame = coord == findPlacePos(attachCoord.m_coord);
//				}
//
//				if (isSame)
//				{
//					auto end = attachCoord.m_typeList.end();
//					auto ite = std::find_if(attachCoord.m_typeList.begin(), end, [tileType, templateId](MapData::CanAttachCoordType& type)
//					{
//						if (type.m_tileType == tileType)
//						{
//							if (tileType == MapCfg::MinorTileType::Building)
//							{
//								if (type.m_templateId == templateId)
//								{
//									return true;
//								}
//							}
//							else
//							{
//								return true;
//							}
//						}
//						return false;
//					});
//
//					if (ite != end)
//					{
//						group.clear();
//						break;
//					}
//				}
//			}
//
//			if (group.empty())
//			{
//				groupIte = canAttachCoordList.erase(groupIte);
//				continue;
//			}
//
//			++groupIte;
//		}
//
//		if (canAttachCoordList.empty())
//		{
//			outPlaceCoordList.emplace_back(buildingPlaceCoord);
//		}
//	}
//}
//
//bool MapData::isDoor(const glm::ivec2 & coord)
//{
//	auto&& placeCoord = findPlacePos(coord);
//
//	auto&& buildingTemplateId = getBuildingTile(placeCoord);
//	if (buildingTemplateId != MapCfg::TILE_EMPTY)
//	{
//		auto&& pCfgBuildingType = BuildingCfg::getInstance().findBuildingTypeByTemplateId(buildingTemplateId);
//		if (pCfgBuildingType)
//		{
//			return pCfgBuildingType->n_door == 1;
//		}
//	}
//
//	return false;
//}
//
//void MapData::SetThroughBuildingDoor(bool bThrough)
//{
//	m_throughBuilding = bThrough;
//}
//
//MapCfg::MinorTileType MapData::getTileType(const glm::ivec2& coord)
//{
//	auto fullTileValue = isOverflow(coord) ? MapCfg::TileValue{} : at(coord);
//	if (fullTileValue.data == MapCfg::TILE_EMPTY) return MapCfg::MinorTileType::Unknown;
//
//	if (MapCfg::isMineralTile(fullTileValue)) return MapCfg::MinorTileType::Mineral;
//	else if (MapCfg::isBasicTile(fullTileValue)) return MapCfg::MinorTileType::Basic;
//	else if (MapCfg::isPlantTile(fullTileValue)) return MapCfg::MinorTileType::Plant;
//	else if (MapCfg::isBuildingTile(fullTileValue)) return MapCfg::MinorTileType::Building;
//	else return MapCfg::MinorTileType::Unknown;
//}
//
//bool MapData::setDropIds(const tstring& ids)
//{
//	tstring dropIds{ base64_decode(ids) };
//	std::istringstream iss{ dropIds };
//	uint32 size{};
//	iss.read(reinterpret_cast<char*>(&size), sizeof(size));
//	for (uint32 i = 0; i < size; ++i)
//	{
//		uint16 x, y;
//		uint32 id;
//		iss.read(reinterpret_cast<char*>(&x), sizeof(x));
//		iss.read(reinterpret_cast<char*>(&y), sizeof(y));
//		iss.read(reinterpret_cast<char*>(&id), sizeof(id));
//		setDropId(glm::ivec2{ x, y }, id);
//	}
//
//	return true;
//}
//
//void MapData::setDropId(const glm::ivec2& coord, unsigned dropId)
//{
//	if (dropId == 0)
//	{
//		m_pInfo->dropIds.erase(coord);
//	}
//	else
//	{
//		m_pInfo->dropIds[coord] = dropId;
//	}
//}
//
//tstring MapData::getDropIdString() const
//{
//	std::ostringstream oss;
//	uint32 size{ static_cast<uint32>(m_pInfo->dropIds.size()) };
//	oss.write(reinterpret_cast<const char*>(&size), sizeof(size));
//	for (const auto& idPair : m_pInfo->dropIds)
//	{
//		uint16 x{ static_cast<uint16>(idPair.first.x) };
//		uint16 y{ static_cast<uint16>(idPair.first.y) };
//		uint32 id{ idPair.second };
//		oss.write(reinterpret_cast<const char*>(&x), sizeof(x));
//		oss.write(reinterpret_cast<const char*>(&y), sizeof(y));
//		oss.write(reinterpret_cast<const char*>(&id), sizeof(id));
//	}
//
//	tstring dropIdString = oss.str();
//	dropIdString = base64_encode(reinterpret_cast<const unsigned char*>(dropIdString.data()), dropIdString.length());
//	return dropIdString;
//}
//
//unsigned MapData::getDropId(const glm::ivec2& coord) const
//{
//	auto iter = m_pInfo->dropIds.find(coord);
//	return iter == m_pInfo->dropIds.end() ? 0 : iter->second;
//}
//
//bool MapData::getCoordOriginWorldPos(glm::vec3& v3)
//{
//	return coordinateToWorldPos(v3, getCoordOrigin());
//}
//
//void MapData::getWorldRect(bzbee::FloatRect& rectOut)
//{
//	// left bottom
//	glm::vec3 leftBottomPos;
//	coordinateToWorldPos(leftBottomPos, glm::ivec2(0, m_mapDim.y-1));
//
//	// right top
//	glm::vec3 rightTopPos;
//	coordinateToWorldPos(rightTopPos, glm::ivec2(m_mapDim.x-1, 0));
//
//	rectOut.min.x = leftBottomPos.x;
//	rectOut.min.y = leftBottomPos.y;
//
//	rectOut.max.x = rightTopPos.x;
//	rectOut.max.y = rightTopPos.y;
//}
//
//void MapData::copyMetaData(const MapData& mapData)
//{
//	setMapName(mapData.getMapName());
//	setCoordOrigin(mapData.getCoordOrigin());
//}
//
//bool MapData::flightPathfinding(std::vector<glm::ivec2>& pathList, glm::ivec2 start, glm::ivec2 end, glm::ivec2 aabb, glm::ivec2 anchor)
//{
//	return m_pFlightPathfinding->find(pathList, start, end, aabb, anchor);
//}
//
//bool MapData::pathfinding(std::vector<glm::ivec2>& pathList, glm::ivec2 start, glm::ivec2 end, glm::ivec2 aabb, glm::ivec2 anchor, int advance)
//{
//	return m_pPathfinding->find(pathList, start, end, aabb, anchor, advance);
//}
//
//void MapData::setPathfindingDistFactor(int iFac)
//{
//	if (iFac >= 0)
//	{
//		m_pPathfinding->setDistFactor(iFac);
//	}
//	else
//	{
//		m_pPathfinding->resetDistFactor();
//	}
//}
//
//bool MapData::isOverflowLua(const glm::ivec2& coord)
//{
//	return isOverflow(coord);
//}
//
//unsigned int MapData::getPlaceTemplateId(const glm::ivec2& placeCoord, MapCfg::MinorTileType placeTileType, unsigned int placeTypeId, bool bIsDirection/* = false*/, const glm::ivec2& playerCoord/* = NS::IVEC2_ZERO*/)
//{
//	switch (placeTileType)
//	{
//	case MapCfg::MinorTileType::Unknown:
//		break;
//	case MapCfg::MinorTileType::Basic:
//	{
//		const MapCfg::TileInfo* info = MapCfg::getInstance().findBasicTileByType(placeTypeId);
//		if (info) return info->m_value;
//	}
//	break;
//	case MapCfg::MinorTileType::Mineral:
//	{
//		const MapCfg::MineInfo* info = MapCfg::getInstance().findMineTileByType(placeTypeId);
//		if (info) return info->m_value;
//	}
//	break;
//	case MapCfg::MinorTileType::Back:
//	{
//		const MapCfg::TileInfo* info = MapCfg::getInstance().findBackTileByType(placeTypeId);
//		if (info) return info->m_value;
//	}
//	break;
//	case MapCfg::MinorTileType::Plant:
//	{
//		auto pPantType = PlantCfg::getInstance().findPlantTemplate(placeTypeId, 1);
//		if (pPantType) return pPantType->m_templateId;
//	}
//	break;
//	case MapCfg::MinorTileType::Building:
//	{
//		//auto playerCoord = this->m_pPlayerComponent->getCoord();
//
//		unsigned int templateId{ 0 };
//
//		auto pBuilding = BuildingCfg::getInstance().findBuildingType(placeTypeId);
//		if (!pBuilding)
//		{
//			return 0;
//		}
//
//		if (pBuilding->m_isSideHanging)
//		{
//			auto buildingTemplateList = BuildingCfg::getInstance().findBuildingTemplateListByTypeId(placeTypeId);
//
//			for (auto pBuildingTemplate : buildingTemplateList)
//			{
//				MapData::CanAttachCoordList canAttachCoordList{};
//				findBuildingCanAttachCoordList(placeCoord, pBuildingTemplate->m_templateId, canAttachCoordList);
//
//				if (!canAttachCoordList.empty())
//				{
//					templateId = pBuildingTemplate->m_templateId;
//					break;
//				}
//			}
//
//			if (templateId == 0 && buildingTemplateList.size() > 0)
//			{
//				templateId = buildingTemplateList[0]->m_templateId;
//			}
//		}
//		else
//		{
//			BuildingCfg::Direction direction{ BuildingCfg::Direction::NONE };
//			if (bIsDirection)
//			{
//				if (playerCoord.x < placeCoord.x)
//				{
//					direction = BuildingCfg::Direction::LEFT;
//				}
//				else
//				{
//					direction = BuildingCfg::Direction::RIGHT;
//				}
//			}
//
//			const BuildingCfg::BuildingTemplate* pTemplateInfo = BuildingCfg::getInstance().findBuildingTemplateByTypeId(placeTypeId, direction);
//			if (pTemplateInfo)
//			{
//				templateId = pTemplateInfo->m_templateId;
//			}
//		}
//
//		return templateId;
//	}
//	break;
//	default:
//		break;
//	}
//
//	return 0;
//}
//
//
//int MapData::getCurUndergroundBorderUp() const
//{
//	int mapId = MapSettingCfg::getInstance().getCurSceneId();
//	auto mapInfo = MapSettingCfg::getInstance().findMap(mapId);
//	if (!mapInfo)
//	{
//		return 0;
//	}
//	return mapInfo->m_undergroundBorderUp;
//}
//
//int MapData::getCurMapMoleExistHeight() const
//{
//	int mapId = MapSettingCfg::getInstance().getCurSceneId();
//	auto mapInfo = MapSettingCfg::getInstance().findMap(mapId);
//	if (!mapInfo)
//	{
//		return 0;
//	}
//	return mapInfo->m_nMoleExistHeight;
//}
//
//glm::ivec2 MapData::GetDigHavePlantCoord(const glm::ivec2& coord)
//{
//	std::vector<glm::ivec2> dir{ MapData::COORD_UP, MapData::COORD_DOWN, MapData::COORD_LEFT, MapData::COORD_RIGHT, NS::IVEC2_ZERO };
//	for (size_t i = 0; i < dir.size(); ++i)
//	{
//		auto placeCoord = findPlacePos(coord + dir[i]);
//
//		if (isPlantTile(placeCoord))
//		{
//			auto plantTemplateId = getPlantTile(placeCoord);
//			auto pPlantTemplate = PlantCfg::getInstance().findPlantTemplate(plantTemplateId);
//
//			glm::ivec2 testCoord{};
//
//			switch (pPlantTemplate->m_attach)
//			{
//			case PlantCfg::Attach::Top:
//				testCoord = placeCoord + MapData::COORD_UP;
//				break;
//
//			case PlantCfg::Attach::Bottom:
//				testCoord = placeCoord + MapData::COORD_DOWN;
//				break;
//
//			case PlantCfg::Attach::Left:
//				testCoord = placeCoord + MapData::COORD_LEFT;
//				break;
//
//			case PlantCfg::Attach::Right:
//				testCoord = placeCoord + MapData::COORD_RIGHT;
//				break;
//
//			case PlantCfg::Attach::Back:
//				testCoord = placeCoord;
//				break;
//
//			case PlantCfg::Attach::None:
//				break;
//
//			default:
//				break;
//			}
//
//			if (testCoord == coord)
//			{
//				return placeCoord;
//			}
//		}
//	}
//
//	return NS::IVEC2_ZERO;
//}
//
//bool MapData::isDigHavePlant(const glm::ivec2& coord)
//{
//	std::vector<glm::ivec2> dir{ MapData::COORD_UP, MapData::COORD_DOWN, MapData::COORD_LEFT, MapData::COORD_RIGHT, NS::IVEC2_ZERO };
//	for (size_t i = 0; i < dir.size(); ++i)
//	{
//		auto placeCoord = findPlacePos(coord + dir[i]);
//
//		if (isPlantTile(placeCoord))
//		{
//			auto plantTemplateId = getPlantTile(placeCoord);
//			auto pPlantTemplate = PlantCfg::getInstance().findPlantTemplate(plantTemplateId);
//
//			glm::ivec2 testCoord{};
//
//			switch (pPlantTemplate->m_attach)
//			{
//			case PlantCfg::Attach::Top:
//				testCoord = placeCoord + MapData::COORD_UP;
//				break;
//
//			case PlantCfg::Attach::Bottom:
//				testCoord = placeCoord + MapData::COORD_DOWN;
//				break;
//
//			case PlantCfg::Attach::Left:
//				testCoord = placeCoord + MapData::COORD_LEFT;
//				break;
//
//			case PlantCfg::Attach::Right:
//				testCoord = placeCoord + MapData::COORD_RIGHT;
//				break;
//
//			case PlantCfg::Attach::Back:
//				testCoord = placeCoord;
//				break;
//
//			case PlantCfg::Attach::None:
//				break;
//
//			default:
//				break;
//			}
//
//			if (testCoord == coord)
//			{
//				return true;
//			}
//		}
//	}
//
//	return false;
//}
//
//bool MapData::isDigHaveBuildingByBasic(const glm::ivec2 & coord)
//{
//	bzbee::Ivec2List outPlaceCoordList{};
//	findAutoRemoveBuildingList(outPlaceCoordList, findPlacePos(coord), MapCfg::MinorTileType::Basic);
//
//	for (const auto& placeCoord : outPlaceCoordList)
//	{
//		auto&& coordId = MapDataMgr::getInstance().localCoord2CoordId(placeCoord, getLevelIndex());
//
//		auto&& pBuildingData = StaticObjMgr::getInstance().findBuilding(coordId);
//
//		if (pBuildingData)
//		{
//			return true;
//		}
//	}
//	return false;
//}
//
//// 是否在地图边界范围内
//bool MapData::IsInMapBoundaryByWorldPosAndMoveDir(const glm::vec3& posWorld, glm::ivec2 m_moveDir)
//{
//	// 是否额外设置了地图边界
//	if (!IsSetMapBoundary())
//	{
//		return true;
//	}
//	glm::ivec2 posCoord;
//	if (!worldPosToCoordinate(posCoord, posWorld))
//	{
//		return false;
//	}
//	if ((posCoord.x + m_moveDir.x * 0.25) < m_vec2MapBoundaryLeftUpper.x)
//	{
//		return false;
//	}
//	if ((posCoord.x + m_moveDir.x * 0.25) > m_vec2MapBoundaryLowerRight.x)
//	{
//		return false;
//	}
//	return true;
//}
//
//// 是否在地图边界范围内
//bool MapData::IsInMapBoundaryByWorldPos(const glm::vec3& posWorld)
//{
//	// 是否额外设置了地图边界
//	if (!IsSetMapBoundary())
//	{
//		return true;
//	}
//	glm::ivec2 posCoord;
//	if (!worldPosToCoordinate(posCoord, posWorld))
//	{
//		return false;
//	}
//	if (posCoord.x < m_vec2MapBoundaryLeftUpper.x)
//	{
//		return false;
//	}
//	if (posCoord.x > m_vec2MapBoundaryLowerRight.x)
//	{
//		return false;
//	}
//	if (posCoord.y < m_vec2MapBoundaryLeftUpper.y)
//	{
//		return false;
//	}
//	if (posCoord.y > m_vec2MapBoundaryLowerRight.y)
//	{
//		return false;
//	}
//
//	return true;
//}
//
//// 调整坐标到在地图边界范围内
//void MapData::AdjustInMapBoundaryByWorldPos(glm::vec3& posWorld, const glm::vec3& posWorldOld)
//{
//	// 是否额外设置了地图边界
//	if (!IsSetMapBoundary())
//	{
//		return;
//	}
//	glm::ivec2 posCoord;
//	if (!worldPosToCoordinate(posCoord, posWorld))
//	{
//		return;
//	}
//	glm::ivec2 posCoordOld;
//	if (!worldPosToCoordinate(posCoordOld, posWorldOld))
//	{
//		return;
//	}
//	bool bNeedAdjust = false;
//	if (posCoord.x < m_vec2MapBoundaryLeftUpper.x)
//	{
//		if (posCoordOld.x < m_vec2MapBoundaryLeftUpper.x)
//		{
//			posCoord.x = m_vec2MapBoundaryLeftUpper.x;
//			bNeedAdjust = true;
//		}
//		else
//		{
//			posWorld.x = posWorldOld.x;
//		}
//	}
//	if (posCoord.x > m_vec2MapBoundaryLowerRight.x)
//	{
//		if (posCoordOld.x > m_vec2MapBoundaryLowerRight.x)
//		{
//			posCoord.x = m_vec2MapBoundaryLowerRight.x;
//			bNeedAdjust = true;
//		}
//		else
//		{
//			posWorld.x = posWorldOld.x;
//		}
//	}
//	if (posCoord.y < m_vec2MapBoundaryLeftUpper.y)
//	{
//		posCoord.y = m_vec2MapBoundaryLeftUpper.y;
//		bNeedAdjust = true;
//	}
//	if (posCoord.y > m_vec2MapBoundaryLowerRight.y)
//	{
//		posCoord.y = m_vec2MapBoundaryLowerRight.y;
//		bNeedAdjust = true;
//	}
//	if (bNeedAdjust)
//	{
//		posWorld = coordToWorldPos(posCoord);
//		posWorld.z = posWorldOld.z;
//	}
//}
//
//// 额外的设置地图边界
//void MapData::SetMapBoundary(glm::ivec2 vec2MapBoundaryLeftUpper, glm::ivec2 vec2MapBoundaryLowerRight)
//{
//	m_bSetMapBoundary = true;
//	m_vec2MapBoundaryLeftUpper = vec2MapBoundaryLeftUpper;
//	m_vec2MapBoundaryLowerRight = vec2MapBoundaryLowerRight;
//}
//
//// 是否额外设置了地图边界
//bool MapData::IsSetMapBoundary()
//{
//	return m_bSetMapBoundary;
//}
//
//// 获取额外设置的地图边界的左上边界
//glm::ivec2 MapData::GetMapBoundaryLeftUpper()
//{
//	return m_vec2MapBoundaryLeftUpper;
//}
//
//// 获取额外设置的地图边界的右下边界
//glm::ivec2 MapData::GetMapBoundaryLowerRight()
//{
//	return m_vec2MapBoundaryLowerRight;
//}
//
//bool MapData::isBasicCanDig(const glm::ivec2& coord, bool bIsFront, int digLevel)
//{
//	int basicDigLevel{0};
//
//	if (bIsFront)
//	{
//		auto&& templateId = getBasicTile(coord);
//		auto&& pTileInfo = MapCfg::getInstance().findBasicTileByTemplateId(templateId);
//		if (!pTileInfo)
//		{
//			return false;
//		}
//
//		basicDigLevel = pTileInfo->m_digLevel;
//	}
//	else
//	{
//		auto&& templateId = getBackTile(coord);
//		auto&& pTileInfo = MapCfg::getInstance().findBackTileByTemplateId(templateId);
//		if (!pTileInfo)
//		{
//			return false;
//		}
//
//		basicDigLevel = pTileInfo->m_digLevel;
//	}
//
//	if (basicDigLevel < 99)
//	{
//		return digLevel >= basicDigLevel;
//	}
//	else
//	{
//		return digLevel > basicDigLevel;
//	}
//}
