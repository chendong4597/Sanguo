/*!
 * \file MapSettingCfg.cpp
 *
 * \author zhichenxia
 * \date 2017/07/12
 *
 *
 */
#include "stdafx.h"
#include "MapSettingCfg.h"
#include "MonsterCfg.h"
#include <StringUtil.h>
#include "../common/Timer.h"
#include <XmlUtil.h>

using namespace NS;

MapSettingCfg::MapSettingCfg()
{}


MapSettingCfg& MapSettingCfg::getInstance()
{
	static MapSettingCfg s_Instance;
	return s_Instance;
}

void MapSettingCfg::terminate()
{
	m_settingMap.clear();
}

bool MapSettingCfg::initialize()
{
	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/MapSetting.bin")) return false;
	if (!parseMapSettingConfig(doc.RootElement())) return false;

	tinyxml2::XMLDocument docMob;
	if (!bzbee::XmlUtil::loadXmlFile(docMob, "config/forestmap_101001.cfg")) return false;
	if (!parseMapMobConfig(docMob.RootElement())) return false;

    //reloadMultiLanguage();

    return true;
}


const MapSettingCfg::MapSetting* MapSettingCfg::findMap(int32 id) const
{
	auto it = m_settingMap.find(id);
	return it != m_settingMap.end() ? &it->second : nullptr;
}

NS::int32 MapSettingCfg::GetNextDungeonMapId(NS::int32 id)
{
	if (id == 0) {
		return 301011;
	}
	auto&& curMap = findMap(id);
	if (curMap) {
		return curMap->nextMapId;
	}
	return 0;
}

void MapSettingCfg::GetInstanceTpy(int mapId, std::vector<MapSettingCfg::MapSetting>& vecMaps)
{
	Map::iterator iter = m_settingMap.begin();
	for (; iter != m_settingMap.end(); iter++)
	{
		if (mapId/10 == iter->first/10)
		{
			vecMaps.push_back(iter->second);
		}
	}
	std::sort(vecMaps.begin(), vecMaps.end(), [](const MapSettingCfg::MapSetting& a, const MapSettingCfg::MapSetting& b) {
		return a.id < b.id;
	});
}

void MapSettingCfg::GetGenInstance(std::vector<MapSetting>& vecMaps)
{
	Map::iterator iter = m_settingMap.begin();
	for (; iter != m_settingMap.end(); iter++)
	{
		if (iter->second.generaltype == MapSettingCfg::DunGeneral_Type_General)
		{
			vecMaps.push_back(iter->second);
		}
	}
	std::sort(vecMaps.begin(), vecMaps.end(), [](const MapSettingCfg::MapSetting& a, const MapSettingCfg::MapSetting& b) {
		return a.id < b.id;
	});
}


bool MapSettingCfg::parseMapSettingConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		MapSetting info{};

		NS::XmlUtil::getAttribute(info.id, xmlItem, "n_ID");
		NS::XmlUtil::getAttribute(info.nextMapId, xmlItem, "n_nextMapId");
		NS::XmlUtil::getAttribute(info.name, xmlItem, "s_name");
		NS::XmlUtil::getAttribute(info.type, xmlItem, "n_type");// = proto.n_type();
		NS::XmlUtil::getAttribute(info.biome, xmlItem, "n_biome");// = proto.n_biome();
		NS::XmlUtil::getAttribute(info.maptemplate, xmlItem, "s_maptemplate");// = proto.s_maptemplate();
		NS::XmlUtil::getAttribute(info.mapfilename, xmlItem, "s_mapfilename");// = proto.s_mapfilename();
		NS::XmlUtil::getAttribute(info.dungeontype, xmlItem, "n_dungeontype");// = proto.n_dungeontype();
		NS::XmlUtil::getAttribute(info.generaltype, xmlItem, "n_GeneralTpy");
		NS::XmlUtil::getAttribute(info.icon, xmlItem, "s_dungeon_icon");// = proto.s_dungeon_icon();
		NS::XmlUtil::getAttribute(info.level, xmlItem, "n_dungeonlevel");// = proto.n_level();
		NS::XmlUtil::getAttribute(info.suggestLv, xmlItem, "n_SuggestLv");// = proto.n_suggestlv();
		NS::XmlUtil::getAttribute(info.grouptype, xmlItem, "n_grouptype");//= proto.n_grouptype();
		NS::XmlUtil::getAttribute(info.hunger, xmlItem, "n_hunger");// = proto.n_hunger();
		NS::XmlUtil::getAttribute(info.passtime, xmlItem, "n_time");
		NS::XmlUtil::getAttribute(info.difficulty, xmlItem, "n_dungeonlevel");// = proto.n_dungeonlevel();
		NS::XmlUtil::getAttribute(info.m_mapPath, xmlItem, "s_mapPath");
		NS::XmlUtil::getAttribute(info.n_first_drop_Id, xmlItem, "n_first_drop_Id");
		NS::XmlUtil::getAttribute(info.n_drop_Id, xmlItem, "n_drop_Id");
		NS::XmlUtil::getAttribute(info.n_NeedDungeon, xmlItem, "n_NeedDungeon");
		
		std::string strItem;
		NS::XmlUtil::getAttribute(strItem, xmlItem, "s_item");//
        std::vector<NS::tstring> groupStrList;
		NS::StringUtil::splitString(groupStrList, strItem, ",");

		for (decltype(groupStrList.size()) i = 0; i < groupStrList.size(); ++i)
		{
			NS::int32 itemId{ 0 };
			NS::int32 itemCount{ 0 };
			NS::int32 count = sscanf(groupStrList[i].c_str(), "[%d;%d]", &itemId, &itemCount);
			if (count != 2) continue;

			info.mapEntranceItemCost.emplace(itemId, itemCount);
		}


		std::vector<NS::tstring> insMobList;
		std::string strMobs;
		NS::XmlUtil::getAttribute(strMobs, xmlItem, "s_dungeon_mob");
		NS::StringUtil::splitString(insMobList, strMobs, ",");

		for (decltype(insMobList.size()) i = 0; i < insMobList.size(); ++i)
		{
			S_DungeonMobInit mob;
			NS::int32 count = sscanf(insMobList[i].c_str(), "[%d;%d;%d;%d;%d;%d;%d;%d;%d;%d]", &mob.m_mobTpy, &mob.m_nMobStage, &mob.m_nId,
				&mob.m_iMobId, &mob.m_iSubMobId, &mob.m_iSubMobNum, &mob.m_X, &mob.m_Y, &mob.m_Z, &mob.m_Yaw);
			if (count != 10) {
				count = sscanf(insMobList[i].c_str(), "[%d;%d;%d;%d;%d;%d;%d;%d;%d]", &mob.m_mobTpy, &mob.m_nMobStage, &mob.m_nId,
					&mob.m_iMobId, &mob.m_iSubMobId, &mob.m_iSubMobNum, &mob.m_X, &mob.m_Y, &mob.m_Z);
				if (count != 9) continue;
			}

			info.m_vecInsMobs.push_back(mob);
		}

		m_settingMap[info.id] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

bool MapSettingCfg::parseMapMobConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("mapCfg");
	if (!p) return true;

	auto xmlItem = p->FirstChildElement("PopBox");
	while (xmlItem != nullptr)
	{
		auto xmlMob = xmlItem->FirstChildElement("Mob");
		if (xmlMob)
		{
			MainCityMob info{};
			NS::XmlUtil::getAttribute(info.id, xmlMob, "defID");
			NS::XmlUtil::getAttribute(info.popTime, xmlMob, "popTime");
			NS::XmlUtil::getAttribute(info.prob, xmlMob, "prob");
			m_mapMaincityMob[info.id] = info;
		}

		xmlItem = xmlItem->NextSiblingElement("PopBox");
	}

	return true;
}

void MapSettingCfg::GetMapMobs(int nMinLv, int nMaxLv, std::vector<MainCityMob>& vecMobs)
{
	MapMob::iterator iter = m_mapMaincityMob.begin();
	for (; iter != m_mapMaincityMob.end(); iter++)
	{
		auto&& pMonster = MonsterCfg::getInstance().findMonsterType(iter->first);
		if (pMonster && pMonster->n_MobLv >= nMinLv && pMonster->n_MobLv <= nMaxLv) {
			vecMobs.push_back(iter->second);
		}
	}
}


