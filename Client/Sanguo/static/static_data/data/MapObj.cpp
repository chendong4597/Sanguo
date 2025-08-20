/*!
 * \file MapObj.cpp
 *
 * \author zhijieli
 * \date 2016/12/05
 *
 *
 */
#include "stdafx.h"
#include "MapObj.h"
#include "MapData.h"
#include "MapDataMgr.h"
#include <LogUtil.h>
#include <algorithm>
#include <iterator>
#include "Framework.h"
#include "MacroUtility.h"
#include "role/Player.h"
#include "role/Monsters.h"
#include <memory>

int MapObj::s_mapObjId;

int MapObj::generateMapObjId()
{
	return ++s_mapObjId;
}

//bool MapObj::worldPosToCoordinate(glm::ivec2 & coordOut, const glm::vec3 & worldPos, float width)
//{
//	std::shared_ptr<MapData> mapData{ MapDataMgr::getInstance().getCurrMapData() };
//	if (!mapData) { return false; }
//
//	glm::vec3 pos{ worldPos };
//	pos.x = pos.x - round(width) / 2 + MapCfg::HALF_TILE_SIZE;
//	return mapData->worldPosToCoordinate(coordOut, pos);
//}
//
//bool MapObj::coordinateToWorldPos(glm::vec3 & worldPosOut, const glm::ivec2 & coord, float width)
//{
//	std::shared_ptr<MapData> mapData{ MapDataMgr::getInstance().getCurrMapData() };
//	if (!mapData) { return false; }
//	if (!mapData->coordinateToWorldPos(worldPosOut, coord)) { return false; }
//
//	worldPosOut.x = worldPosOut.x - MapCfg::HALF_TILE_SIZE + round(width) / 2;
//
//	return true;
//}


MapObj::~MapObj()
{
}


const glm::ivec2 MapObj::getCoord() const
{
	//std::shared_ptr<MapData> mapData{ MapDataMgr::getInstance().getCurrMapData() };
	//if (!mapData) { return glm::ivec2{}; }
	//glm::vec3 pos{ getPos() };
	//pos.x = pos.x - round(getWidth()) / 2 + MapCfg::HALF_TILE_SIZE;
	glm::ivec2 coord{0 , 0};
	//mapData->worldPosToCoordinate(coord, pos);

	return coord;
}

void MapObj::setPos(const glm::vec3& pos)
{
	if (m_pos != pos)
	{
		m_pos = pos;
		MapObjMgr::getInstance().updateVision(this);
	}
}

void MapObj::setCoord(const glm::ivec2& coord)
{
	//std::shared_ptr<MapData> mapData{ MapDataMgr::getInstance().getCurrMapData() };
	//if (!mapData) { return; }

	//mapData->coordinateToWorldPos(m_pos, coord);
}

//////////////////////////////////////////////////////////////////////////
MapObjMgr& MapObjMgr::getInstance()
{
	static MapObjMgr mgr;
	return mgr;
}

std::shared_ptr<MapObj> MapObjMgr::findObj(bzbee::int32 mapObjId)
{
	auto iter = m_mapObjs.find(mapObjId);
	/*if (iter != m_mapObjs.end())
	{
		auto data = iter->second.lock();
	}*/
	return iter == m_mapObjs.end() ? nullptr : iter->second;
}

bool MapObjMgr::addObj(const std::shared_ptr<MapObj>& obj)
{
	if (findObj(obj->getId()))
	{
		LOGE("duplicated map obj id: {0}, override.", obj->getId());
		removeObj(obj->getId());
	}

	m_mapObjs[obj->getId()] = obj;

	m_visionXObjs.emplace_front(obj.get());
	m_visionXObjsIt[obj.get()] = m_visionXObjs.begin();
	updateVision(obj.get());

	return true;
}

void MapObjMgr::updateVision(const MapObj* obj)
{
	if (!obj) return;
	auto foundObj = findObj(obj->getId());
	if (!foundObj || foundObj.get() != obj) return;

	auto it = m_visionXObjsIt[obj];
	assert(it != m_visionXObjs.end());

	auto itb = it;
	for (; std::next(itb) != m_visionXObjs.end(); ++itb)
	{
		if (obj->getPos().x <= (*std::next(itb))->getPos().x)
		{
			break;
		}
	}
	if (itb != it)
	{
		auto replaceObj = *it;
		m_visionXObjs.erase(it);
		m_visionXObjs.insert(std::next(itb), replaceObj);
		m_visionXObjsIt[replaceObj] = std::next(itb);
		return;
	}

	for (itb = it; itb != m_visionXObjs.begin(); --itb)
	{
		if (obj->getPos().x >= (*std::prev(itb))->getPos().x)
		{
			break;
		}
	}
	if (itb != it)
	{
		bool isBegin = itb == m_visionXObjs.begin();
		auto prev = isBegin ? m_visionXObjs.begin() : std::prev(itb);
		auto replaceObj = *it;
		m_visionXObjs.erase(it);
		m_visionXObjs.insert(itb, replaceObj);
		m_visionXObjsIt[replaceObj] = isBegin ? m_visionXObjs.begin() : std::next(prev);
		return;
	}
}


void MapObjMgr::update(float dt)
{
	for (auto it : m_mapObjs)
	{
		auto pRole = std::dynamic_pointer_cast<Role>(it.second);//std::make_shared<Role>(pGameObject);
		if (!pRole) {
			continue;;
		}
		pRole->update(dt);
	}
}

//void vector_weak_remove(std::list<std::weak_ptr<MapObj>> l, const MapObj* o)
//{
//	l.remove_if([o](std::weak_ptr<MapObj> t)
//	{
//		return t.lock().get() == o;
//	});
//}




bool MapObjMgr::removeObj(bzbee::int32 mapObjId)
{
	if (m_mapObjs.find(mapObjId) != m_mapObjs.end())
	{
		auto obj = m_mapObjs[mapObjId].get();
		m_visionXObjs.remove(obj);
		m_visionXObjsIt.erase(obj);
	}
	return m_mapObjs.erase(mapObjId) > 0;
}

bool MapObjMgr::removeObjByIggId(NS::I64 id)
{
	for (auto it : m_mapObjs)
	{
		auto id = it.first;
		auto obj = it.second;

		if (nullptr != obj && obj->getIggId() == id)
		{
			return removeObj(obj->getId());
		}
	}
	return false;
}


void MapObjMgr::visionColisionUpdate()
{
}

void MapObjMgr::clear()
{
	m_visionXObjs.clear();
	m_visionXObjsIt.clear();
	m_visionSets.clear();
	m_visionSetsRev.clear();
}

