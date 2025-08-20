/*!
 * \file MapData.h
 *
 * \author laizhisheng
 * \date 2016/11/22 10:40
 *
 */
#pragma once
#include <baseutil.h>
#include <functional>

struct MapDataShared
{
	using Coord2DropId = std::map<glm::ivec2, unsigned>;

	int id{};
	std::string mapName;
	NS::I64 lastSaveTime{ -1 };
	Coord2DropId dropIds;
};


class MapData : public std::enable_shared_from_this<MapData>
{
	MapData            (const MapData&) = delete;
	MapData& operator= (const MapData&) = delete;

public:
	//static const glm::ivec2 COORD_UP;
	//static const glm::ivec2 COORD_DOWN;
	//static const glm::ivec2 COORD_LEFT;
	//static const glm::ivec2 COORD_RIGHT;
	//static const glm::ivec2 COORD_UP_RIGHT;
	//static const glm::ivec2 COORD_UP_LEFT;
	//static const glm::ivec2 COORD_DOWN_RIGHT;
	//static const glm::ivec2 COORD_DOWN_LEFT;

public:
	explicit MapData(int id);
	MapData(std::shared_ptr<MapDataShared> pInfo, int id);
	virtual ~MapData(void);

public:
	//virtual std::shared_ptr<MapData> getLevel(int index) = 0;
	//virtual int getLevelCount(void) const noexcept = 0;

	//void setLevelIndex(int index) noexcept { m_levelIndex = index; }
	//int getLevelIndex(void) const noexcept { return m_levelIndex; }

	//virtual bool copyMapData(int& widthOut, int& heightOut, unsigned intList& bufferOut) const = 0;
	//virtual bool updateSubMapData(const unsigned int32* pMapData, const glm::ivec2& leftTop, const glm::ivec2& size) = 0;

	//virtual bool METHOD(setCoordOrigin)(const glm::ivec2& coord) { m_coordOrigin = coord; return true; }
	//const glm::ivec2& METHOD(getCoordOrigin)() const { return m_coordOrigin; }

	////inline void setOffset(const glm::vec3& offset, glm::vec2 origin) { setOffset(glm::vec3(offset.x - origin.x, offset.y - origin.y, offset.z)); }
 //   virtual void METHOD(setOffset)(const glm::vec3& offset) = 0;
 //   virtual glm::vec3 METHOD(getOffset)(void) const = 0;

	//virtual MapCfg::TileValue& at(int index       )       = 0;
	//virtual MapCfg::TileValue  at(int index       ) const = 0;
	//virtual MapCfg::TileValue& at(int x, int y    )       = 0;
	//virtual MapCfg::TileValue  at(int x, int y    ) const = 0;
	//inline  MapCfg::TileValue& at(glm::ivec2 coord)       { return at(coord.x, coord.y); }
	//inline  MapCfg::TileValue  at(glm::ivec2 coord) const { return at(coord.x, coord.y); }

public:
	int getId(void) { return m_pInfo->id; }
	//bool METHOD(setMapName)(const std::string& mapName) { m_pInfo->mapName = mapName; return true; }
	//const std::string& METHOD(getMapName)() const { return m_pInfo->mapName; }

	//// resize map and do allocate memory.
	//bool resizeMap(const glm::ivec2& mapDim);
	//// resize map do not allocate memory.
	//bool resizeEmptyMap(const glm::ivec2& mapDim);
	//const glm::ivec2& METHOD(getMapDim)() const { return m_mapDim; }
	//glm::vec2 METHOD(getWorldSize)() const { return { m_mapDim.x * MapCfg::TILE_SIZE, m_mapDim.y * MapCfg::TILE_SIZE }; }

	//bool getCoordOriginWorldPos(glm::vec3& out);

	//void getWorldRect(bzbee::FloatRect& rectOut);

	//unsigned int METHOD(getBasicTile)(const glm::ivec2& coord) const;
	//unsigned int METHOD(getMineralTile)(const glm::ivec2& coord) const;
	//unsigned int METHOD(getBackTile)(const glm::ivec2& coord) const;
	//unsigned int METHOD(getBuildingTile)(const glm::ivec2& coord) const;
	//unsigned int METHOD(getPlantTile)(const glm::ivec2& coord) const;
	//unsigned int getLiquidTileType(const glm::ivec2& coord) const;
	//unsigned int getLiquidTileType(const glm::vec3& worldPos) const;
	//unsigned int METHOD(getLiquidTileLevel)(const glm::ivec2& coord) const;
	//bool getLiquidTopSurfaceWorldPos(glm::vec3& worldPos, const glm::ivec2& coord);
	//bool getStepOffsetWorldPos(const glm::vec3& worldPos, float roleWidth, float roleHeight, float& stepOffset);

	//int METHOD(getFrontTileHp)(const glm::ivec2& coord) const;
	//int METHOD(getBackTileHp)(const glm::ivec2& coord) const;

	//bool METHOD(isBasicTile)(const glm::ivec2& coord) const;
	//bool METHOD(isDontDigBasicTile)(const glm::ivec2& coord) const;
	//bool METHOD(isMineralTile)(const glm::ivec2& coord) const;
	//bool METHOD(isBlockingTile)(const glm::ivec2& coord, const glm::ivec2& dir, bool platformBlocking = true) const;
	//bool METHOD(isPlatformTile)(const glm::ivec2& coord) const;
	//bool METHOD(isPlantTile)(const glm::ivec2& coord) const;
	//bool METHOD(isTree)(const glm::ivec2& coord) const;
	//bool METHOD(isBuildingTile)(const glm::ivec2& coord) const;
	//bool METHOD(isAnyCoordBuildingTile)(const glm::ivec2& anyCoord) const;
	//bool METHOD(isFrontTileEmpty)(const glm::ivec2& coord) const;
	//bool METHOD(isFrontTileCompleteEmpty)(const glm::ivec2& coord) const;
	//bool METHOD(isBackTileEmpty)(const glm::ivec2& coord) const;
	//bool METHOD(isClimbableTile)(const glm::ivec2& coord) const;
	//bool isLiquidTile(const glm::ivec2& coord) const { return getLiquidTileType(coord) != MapCfg::LiquidType::None; }
	//bool METHOD(isTargetTypeTile)(const glm::ivec2& coord, unsigned int typyId) const;
	//// 获取安全的，不在土块中的，可以站立的位置
	//glm::vec2 METHOD(GetStandableNotInBlockingBasicTile)(const glm::ivec2& coordTarget, int nRoleWidth, int nRoleHeight, bool platformBlocking, int nRangeWidth, int nRangeHeight, int nRangeLoopNum);
	//glm::vec2 METHOD(GetNotInBlockingBasicTile)(const glm::ivec2& coordTarget, int nRoleWidth, int nRoleHeight, bool platformBlocking, int nRangeWidth, int nRangeHeight);
	////  判断 Role 在 coordRole 位置是否会进入土块中
	//bool isInBlockingBasicTile(const glm::ivec2& coordRole, float roleWidth, float roleHeight, bool platformBlocking = true) const;
	//// coord is at left bottom corner
	//bool isStandable(const glm::ivec2& coord, float roleWidth, float roleHeight, bool platformBlocking = true) const;
	//bool isStandable(const glm::vec3& pos, float roleWidth, float roleHeight, bool platformBlocking = true) const;
	//bool isHangUpsideDownAble(const glm::ivec2& coord);
	//bool METHOD(isStandablePlatformBlocking)(const glm::ivec2& coord, float roleWidth, float roleHeight) const;
	//// coord is at left bottom corner
	//bool isPassable(const glm::ivec2& coord, const glm::ivec2& dir, float roleWidth, float roleHeight, bool platformBlocking = true) const;
	//bool isPassable(const glm::vec3& pos, const glm::ivec2& dir, float roleWidth, float roleHeight) const;
	//// coord is at left bottom corner		
	//bool isClimbable(const glm::vec3& pos, const glm::ivec2& dir) const;
	//bool isClimbable(const glm::ivec2& coord, const glm::ivec2& dir) const;
	//	
	//bool METHOD(isConnected)(const glm::ivec2& src, const glm::ivec2& dst);
	//bool isRayThrough(const glm::vec3& src, const glm::vec3& dst);
	//bool isBlocked(const glm::vec3& src, float srcWidth, float srcHeight, const glm::vec3& dst, float dstWidth, float dstHeight);
	//bool METHOD(isStaticObjTile)(const glm::ivec2& coord) const;
	//bool METHOD(isWarFog)(const glm::ivec2& coord) const;
	//bool METHOD(isBluePrint)(const glm::ivec2& coord) const;
	//bool METHOD(isBluePrintBack)(const glm::ivec2& coord) const;
	//bool METHOD(hasLiquid)(const glm::vec3& worldPos) const { return getLiquidTileType(worldPos) != MapCfg::LiquidType::None; }

	//bool METHOD(setBasicTile)(unsigned int basicTileValue, const glm::ivec2& coord);
	//bool METHOD(setLiquidTile)(unsigned int liquidType, unsigned int liquidTileValue, const glm::ivec2& coord);
	//bool METHOD(setMineralTile)(unsigned int mineralTileValue, const glm::ivec2& coord);
	//bool METHOD(setExtraTile)(unsigned int extraTileValue, const glm::ivec2& coord);
	//bool METHOD(setBackTile)(unsigned int backTileValue, const glm::ivec2& coord);
	//bool METHOD(setBlockingTile)(const glm::ivec2& coord, bool bBlockLiquid = false);
	//bool METHOD(setPlacePosOffset)(const glm::ivec2& offset, const glm::ivec2& coord);
	//bool METHOD(setWarFog)(const glm::ivec2& coord, bool bVisble);
	//bool METHOD(setBlueprint)(const glm::ivec2& coord, bool bVisble);
	//bool METHOD(setBlueprintBack)(const glm::ivec2& coord, bool bVisble);

	//bool METHOD(clearFrontTile)(const glm::ivec2& coord);
	//bool METHOD(clearBasicTile)(const glm::ivec2& coord);
	//bool METHOD(clearBackTile)(const glm::ivec2& coord);
	//bool METHOD(clearBlockingTile)(const glm::ivec2& coord);
	//bool METHOD(clearBuildingTiles)(const glm::ivec2& coord, unsigned int templateId);
	//bool METHOD(clearPlantTiles)(const glm::ivec2& coord, unsigned int templateId);

	//glm::ivec2 METHOD(worldPosToCoord)(const glm::vec3& worldPos) const;
	//glm::vec3 METHOD(coordToWorldPos)(const glm::ivec2& coord) const;

	//void WorldPosToCoordinateNotOverflow(glm::ivec2& coordOut, const glm::vec3& worldPos) const;
	//bool worldPosToCoordinate(glm::ivec2& coordOut, const glm::vec3& worldPos) const;
	//int WorldToCoordX(const float WorldX);
	//int WorldToCoordY(const float WorldY);
	//bool coordinateToWorldPos(glm::vec3& worldPosOut, const glm::ivec2& coord) const;
	//int CoordToWorldX(const int CoordX);
	//int CoordToWorldY(const int CoordY);
	//bool findLandingCoordinate(glm::ivec2& coordOut, const glm::ivec2& coord) const;
	//glm::ivec2 METHOD(findPlacePos)(const glm::ivec2& coordIn, bool isFilterBluePrint = true) const;

	//bool METHOD(isDigHavePlant)(const glm::ivec2& coord);
	//glm::ivec2 METHOD(GetDigHavePlantCoord)(const glm::ivec2& coord);
	//bool METHOD(isDigHaveBuildingByBasic)(const glm::ivec2 & coord);

	//struct CanAttachCoordType
	//{
	//	MapCfg::MinorTileType m_tileType{};
	//	unsigned int m_templateId{};
	//};
	//struct CanAttachCoordData
	//{
	//	glm::ivec2 m_coord{};
	//	std::vector<CanAttachCoordType> m_typeList{};
	//};
	//using CanAttachCoordList = std::vector<std::vector<CanAttachCoordData>>;
	//void findBuildingCanAttachCoordList(const glm::ivec2& coord, unsigned int buildingTemplateId, CanAttachCoordList& canAttachCoordList, const std::function<unsigned int(const glm::ivec2&)>& getSignTypeIdFunc = nullptr);
	//void findBuildingBottomCoordList(NS::Ivec2List& outPlaceCoordList, const glm::ivec2& coord);
	//void findNearBuildingList(NS::Ivec2List& outPlaceCoordList, const glm::ivec2& coord, MapCfg::MinorTileType tileType);
	//void findAutoRemoveBuildingList(NS::Ivec2List& outPlaceCoordList, const glm::ivec2& coord, MapCfg::MinorTileType tileType);

	//bool METHOD(isDoor)(const glm::ivec2& coord);
	//void METHOD(SetThroughBuildingDoor)(bool bThrough);

	//MapCfg::MinorTileType getTileType(const glm::ivec2& coord);

	//bool METHOD(setDropIds)(const std::string& ids);
	//void METHOD(setDropId)(const glm::ivec2& coord, unsigned dropId);
	//std::string METHOD(getDropIdString)() const;
	//const MapDataShared::Coord2DropId& getDropIds() const { return m_pInfo->dropIds; }
	//unsigned METHOD(getDropId)(const glm::ivec2& coord) const;
	//
	//static glm::vec3 serverWorldPos2ClientWorldPos(float x, float y) { return glm::vec3(x / 100.f, y / 100.f, 0); }
	//static glm::vec3 serverWorldPos2ClientWorldPos(int x, int y) { return serverWorldPos2ClientWorldPos(static_cast<float>(x), static_cast<float>(y)); }
	//static glm::vec2 clientWorldPos2ServerWorldPos(glm::vec3 pos) { return glm::vec2(pos.x * 100.f, pos.y * 100.f); }

	//void findBuildingCurrPosList(NS::Ivec2List& outPosList, const glm::ivec2& coord);

	//void copyMetaData(const MapData& mapData);

	//int xy2Index(int x, int y) const { return ((y + 1)*(m_mapDim.x + 2) + x + 1); }
	//int xy2Index(const glm::ivec2& xy) const { return xy2Index(xy.x, xy.y); }
	//glm::ivec2 index2xy(int index) const { return glm::ivec2((index % (m_mapDim.x + 2)) - 1, (index / (m_mapDim.x + 2)) - 1); }
	//
	//NS::I64 GetLastSaveTime() { return m_pInfo->lastSaveTime; }
	//void SetLastSaveTime(const NS::I64 t) { m_pInfo->lastSaveTime = t; }

	//bool flightPathfinding(std::vector<glm::ivec2>& pathList, glm::ivec2 start, glm::ivec2 end, glm::ivec2 aabb, glm::ivec2 anchor);
	//bool pathfinding(std::vector<glm::ivec2>& pathList, glm::ivec2 start, glm::ivec2 end, glm::ivec2 aabb, glm::ivec2 anchor, int advance);
	//void METHOD(setPathfindingDistFactor)(int iFac);

	//inline bool isOverflow(int x, int y) const { return x < 0 || x >= m_mapDim.x || y < 0 || y >= m_mapDim.y; }
	//inline bool isOverflow(glm::ivec2 coord) const { return isOverflow(coord.x, coord.y); }
	//void METHOD(SetIsForceFindingPath)(bool status) { m_isForceFindingPath = status; }
	//bool METHOD(isOverflowLua)(const glm::ivec2& coord);
	//unsigned int getPlaceTemplateId(const glm::ivec2& placeCoord, MapCfg::MinorTileType placeTileType, unsigned int placeTypeId, bool bIsDirection = false, const glm::ivec2& playerCoord = NS::IVEC2_ZERO);

	//int METHOD(getCurUndergroundBorderUp)() const;
	//int METHOD(getCurMapMoleExistHeight)() const;

	//// 额外的设置地图边界
	//void METHOD(SetMapBoundary)(glm::ivec2 vec2MapBoundaryLeftUpper, glm::ivec2 vec2MapBoundaryLowerRight);
	//// 是否额外设置了地图边界
	//bool METHOD(IsSetMapBoundary)();
	//// 获取额外设置的地图边界的左上边界
	//glm::ivec2 METHOD(GetMapBoundaryLeftUpper)();
	//// 获取额外设置的地图边界的右下边界
	//glm::ivec2 METHOD(GetMapBoundaryLowerRight)();
	//// 是否在地图边界范围内
	//bool IsInMapBoundaryByWorldPosAndMoveDir(const glm::vec3& posWorld, glm::ivec2 m_moveDir);
	//// 是否在地图边界范围内
	//bool IsInMapBoundaryByWorldPos(const glm::vec3& posWorld);
	//// 调整坐标到在地图边界范围内
	//void AdjustInMapBoundaryByWorldPos(glm::vec3& posWorld, const glm::vec3& posWorldOld);

	////地块挖掘等级是否可挖掘
	//bool METHOD(isBasicCanDig)(const glm::ivec2& coord, bool bIsFront, int digLevel);

protected:
	//virtual bool onResize(const glm::ivec2& mapDim) = 0;

protected:
	int m_levelIndex{};
	//glm::ivec2 m_mapDim;
	//glm::ivec2 m_coordOrigin{ 128, 12 };
	//std::unique_ptr<FlightPathfindingEx> m_pFlightPathfinding;
	//std::unique_ptr<      PathfindingEx> m_pPathfinding;
	std::shared_ptr<MapDataShared> m_pInfo;
	//bool m_isForceFindingPath{ false };
	//bool m_throughBuilding{ false };
	//bool m_bSetMapBoundary{ false }; // 是否又额外的设置了地图边界
	//glm::ivec2 m_vec2MapBoundaryLeftUpper; // 额外设置的地图边界的左上边界
	//glm::ivec2 m_vec2MapBoundaryLowerRight; // 额外设置的地图边界的右下边界
};
