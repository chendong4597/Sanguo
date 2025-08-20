#pragma once
#include "BaseTypeDefine.h"
#include "msg/MsgDungeon.pb.h"
#include <map>

class MapData;
class MapDataMgr final
{
	MapDataMgr(void) = default;

	MapDataMgr            (const MapDataMgr&) = delete;
	MapDataMgr& operator= (const MapDataMgr&) = delete;
public:

public:

	static const int ERROR_POS = -1;
	static const NS::I64 ERROR_UUID;

	static MapDataMgr& getInstance();
	static int generateMapId();
    void terminate();

	std::shared_ptr<MapData> getCurrMapData() const;
	std::shared_ptr<MapData> findMapData(int id) const;
	
	bool addMapData(const std::shared_ptr<MapData>& mapData);
	bool removeMapData(int id);
	bool setCurrMapDataId(int id);
	int getCurrMapDataId() { return m_currMapDataId; }

	void setMapId(int nMapId) { m_mapId = nMapId; }
	int getMapId() { return m_mapId; }

	void setSerialID(int nSerialID) { m_mapSerialID = nSerialID; }
	int getSerialID() { return m_mapSerialID; }

	void StartMapTime();
	NS::int32 GetDungeonLessTm();

	void SetDungeonExpData(const MsgPB::MsgDungeonSucExp_S2C& data) { m_expData = data; }
	const MsgPB::MsgDungeonSucExp_S2C& GetDungeonExpData() { return m_expData; }
	
public:
	enum class MapType
	{
		None = 0,

		Private = 1,
		Public = 2,
		Dungeon = 3,
		PVP = 4,

		Unknown,
	};

	//void setIsLoadingMap(bool isLoading) { m_isLoadingMap = isLoading; }
	//bool METHOD(isLoadingMap)() { return m_isLoadingMap; }
	//void setWillGoToMapType(MapType mapType) { m_willGoToMapType = mapType; }
	//MapType getWillGoToMapType() { return m_willGoToMapType; }

	void setCurMapType(MapType mapType) { m_curMapType = mapType; }
	MapType getCurMapType() { return m_curMapType; }

	///////////////////////////////////
	//NS::I64 generateCoordId(const glm::ivec2& localCoord, int mapDataLevel);
	//NS::I64 globalCoord2CoordId(const glm::ivec2& globalCoord);
	//std::tuple<int, glm::ivec2> globalCoord2LocalCoord(const glm::ivec2& globalCoord);
	//static std::tuple<int, glm::ivec2> coordId2LocalCoord(NS::I64 coordId);
	//glm::ivec2 coordId2GlobalCoord(NS::I64 coordId);
	//glm::ivec2 localCoord2GlobalCoord(const glm::ivec2& localCoord, int mapDataLevel);
	//static NS::I64 localCoord2CoordId(const glm::ivec2& localCoord, int mapDataLevel);
	///////////////////////////////////

	//一系列地图限制判断end
private:
	std::map<int, std::shared_ptr<MapData>> m_mapDataList;
	int m_currMapDataId{1};

	bool m_isLoadingMap{ false };
	MapType m_willGoToMapType;

	MapType m_curMapType{ MapType::None };
	int m_mapId{ 0 };
	int m_mapSerialID{ 0 };

	NS::I64 m_n64MapBeginTm{ 0 };

	MsgPB::MsgDungeonSucExp_S2C m_expData;
};
