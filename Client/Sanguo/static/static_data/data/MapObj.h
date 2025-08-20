/*!
 * \file MapObj.h
 *
 * \author zhijieli
 * \date 2016/12/05
 *
 * 
 */
#pragma once
#include "BaseTypeDefine.h"
#include "MacroUtility.h"
#include <functional>
#include <list>
#include <unordered_map>

class MapObj;
class MapObjSightDelegate
{
public:
	virtual ~MapObjSightDelegate() {};

	virtual void inSight(MapObj* obj, int index) = 0;
	virtual void outOfSight(MapObj* obj, int index) = 0;
};

class MapObj
{
public:
	enum class Category : unsigned short
	{
		Player,
		Monster,
		Npc,
		TowerBuilding,
		InteractiveBuilding,
		Vehicle,
		Pet,
		FollowPet,
		Creature,	// 技能创生体，法球/区域等

		None,
	};

	struct FOV {
		float x;
		float y;
	};

	enum FOV_INDEX : int
	{
		INDEX_NORMAL_AI = 0,
		INDEX_BUFF_RANGE = 99,
		INDEX_BOSS_HPBAR = 9999,

	};

public:
	explicit MapObj(bzbee::int32 id) : m_id{ id } {}
	virtual ~MapObj();
	
	virtual bzbee::int32 getId() const      { return m_id;       }
	virtual NS::I64 getIggId() const { return m_iggId; }
	Category getCategory() const    { return m_category; }
	virtual const glm::vec3 getPos() const { return m_pos; }
	// Return value - left bottom coordinate
	virtual const glm::ivec2 getCoord() const;

	virtual void setId(bzbee::int32 id) { m_id = id; }
	virtual void setIggId(NS::I64 id) { m_iggId = id; }
	virtual void setPos(const glm::vec3& pos);
	void setCoord(const glm::ivec2& coord);

	virtual float getWidth() const { return 0.0f; }
	virtual float getHeight() const { return 0.0f; }

	void setSightDelegate(MapObjSightDelegate* delegate) { m_sightDelegate = delegate; }
	void outOfSight(MapObj* obj, int index) { if (m_sightDelegate) m_sightDelegate->outOfSight(obj, index); }
	void inSight(MapObj* obj, int index) { if (m_sightDelegate) m_sightDelegate->inSight(obj, index); }
	void resetSight();
	std::set<MapObj*>* getAllObjsInSight();

	static int generateMapObjId();
	//static bool worldPosToCoordinate(glm::ivec2& coordOut, const glm::vec3& worldPos, float width);
	//static bool coordinateToWorldPos(glm::vec3& worldPosOut, const glm::ivec2& coord, float width);

	//inline FOV getFOV(int index = FOV_INDEX::INDEX_NORMAL_AI) const { return m_fovs.find(index) != m_fovs.end() ? m_fovs.at(index) : FOV{0.f, 0.f}; }
	//void setFOV(const FOV& fov, int index = FOV_INDEX::INDEX_NORMAL_AI) { m_fovs[index] = fov; }
	//void removeFOV(int index) { m_fovs.erase(index); }

	//const std::unordered_map<int, FOV>& getFOVs() { return m_fovs; }

    DECLARE_PROPERTY(std::string, Name);

protected:
	bzbee::int32 m_id{};
	Category m_category;
	glm::vec3 m_pos;
	NS::I64 m_iggId{};
	std::unordered_map<int, FOV> m_fovs;

	static int s_mapObjId;
	MapObjSightDelegate* m_sightDelegate{};
};

//////////////////////////////////////////////////////////////////////////
class MapObjMgr
{
public:
	static MapObjMgr& getInstance();
	
	std::shared_ptr<MapObj> findObj(bzbee::int32 mapObjId);
	const std::unordered_map<bzbee::uint, std::shared_ptr<MapObj>>& getAllObjs() const { return m_mapObjs; }
	bool addObj(const std::shared_ptr<MapObj>& obj);
	bool removeObj(bzbee::int32 mapObjId);
	bool removeObjByIggId(NS::I64 id);
	
	void update(float dt);

	//std::list<bzbee::int32> findNearestObjectsByPos(const glm::vec3 pos, const float maxDistance, bzbee::uint32 maxObjects, const std::function<bool(MapObj*)> filter = nullptr);
	//std::list<bzbee::int32> findNearestObjectsByIdAndPos(const std::shared_ptr<MapObj> casterObj, const glm::vec3 pos, const float maxDistance, bzbee::uint32 maxObjects, const std::function<bool(MapObj*)> filter = nullptr);
	//std::list<bzbee::int32> findNearestObjectsByPos(glm::vec3 pos, MapObj::Category category, float maxDistance, bzbee::int32 maxObjects);
	//std::vector<bzbee::int32>& findNearestObjectsByMapObj(MapObj* mapObj, float maxDistance, bzbee::uint32 maxObjects, const std::function<bool(MapObj*)> filter = nullptr);

	//void resetSight(MapObj* obj);
	//void RemoveSightFromTarget(MapObj* obj, bzbee::int32 targetMapObjId);
	void updateVision(const MapObj* obj);
	void visionColisionUpdate();

	//std::set<MapObj*>* getAllObjsInSight(MapObj* obj, int index = 0) { return m_visionSets.find(obj) != m_visionSets.end() ? (m_visionSets[obj].find(index) != m_visionSets[obj].end() ? &m_visionSets[obj][index] : nullptr) : nullptr; }
	void clear();

private:
	//void CheckSightOut();
	//void CheckSightIn();
	//void CheckInSightByFovIndex(MapObj* a, MapObj* b, int fovIndex);

private:
	using MapObjId2MapObj = std::unordered_map<bzbee::uint, std::shared_ptr<MapObj>>;
	MapObjId2MapObj m_mapObjs;
	// 所有 MapObj 的列表 按照X由小到大排序了
	std::list<MapObj*> m_visionXObjs{};
	// 用于保存 MapObj 在 m_visionXObjs 中所对应的iterator值
	std::unordered_map<const MapObj*, std::list<MapObj*>::iterator> m_visionXObjsIt{};
	// 用于保存 MapObj 的第int视野内的MapObj
	std::unordered_map<MapObj*, std::unordered_map<int, std::set<MapObj*>>> m_visionSets{};
	// 用于保存 某个 MapObj 的视野内的MapObj， int为计数
	std::unordered_map<MapObj*, std::unordered_map<MapObj*, int>> m_visionSetsRev{};
};
