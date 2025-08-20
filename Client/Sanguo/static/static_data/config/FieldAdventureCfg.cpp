#include "stdafx.h"
#include "FieldAdventureCfg.h"
//#include "DropCfg.h"
#include "ItemCfg.h"
#include <StringUtil.h>
#include "RandomUtil.h"
#include "XmlUtil.h"

//////////////////////////////////////////////////////////////////////////
FieldAdventureCfg& FieldAdventureCfg::getInstance()
{
	static FieldAdventureCfg s_FieldAdventureCfg;
	return s_FieldAdventureCfg;
}

bool FieldAdventureCfg::initialize()
{
	// 解析 NpcAutomaticCfg.xml 文件
    //if (!parseNpcAutomaticCfgBinary()) return false;

    // 解析 NpcAutomaticMob.xml 文件
    //if (!parseNpcAutomaticMobBinary()) return false;

    // 解析 NpcAutomaticMaps.xml 文件
    //if (!parseNpcAutomaticMapsBinary()) return false;

	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/NpcAutomaticCfg.bin")) return false;

	return parseNpcAutomaticConfig(doc.RootElement());

    return true;
}

void FieldAdventureCfg::terminate()
{
	m_mapFieldAdventureInfo.clear();
	m_mapFieldAdventureInfoSortBiome.clear();
	m_mapFieldAdventureMobInfo.clear();
	m_mapFieldAdventureMapInfo.clear();
	m_biome2MiningRewardId.clear();
	m_atkDropInfo.clear();
}

const FieldAdventureInfo* FieldAdventureCfg::FindFieldAdventureInfo(int idTask)
{
	const FieldAdventureInfo* pTaskData = nullptr;
	auto it = m_mapFieldAdventureInfo.find(idTask);
	if (it != m_mapFieldAdventureInfo.end())
	{
		pTaskData = &(it->second);
	}

	return pTaskData;
}

const FieldAdventureInfo* FieldAdventureCfg::FindFieldAdventureInfo(int nBiome, int nStage)
{
	auto itBiome = m_mapFieldAdventureInfoSortBiome.find(nBiome);
	if (itBiome != m_mapFieldAdventureInfoSortBiome.end())
	{
		auto itStage = itBiome->second.find(nStage);
		if (itStage != itBiome->second.end())
		{
			return itStage->second;

		}
	}

	return nullptr;
}

const std::map<int, const FieldAdventureInfo*>* FieldAdventureCfg::FindFieldAdventureInfoByBiome(int nBiome)
{
	auto itBiome = m_mapFieldAdventureInfoSortBiome.find(nBiome);
	if (itBiome != m_mapFieldAdventureInfoSortBiome.end())
	{
		return &itBiome->second;
	}

	return nullptr;
}

void FieldAdventureCfg::getRandomAtkDropInfo(std::unordered_map<int, int> & out)
{
	for (auto & itemDropData : m_atkDropInfo)
	{
		int itemNum = NS::RandomUtil::getRandomInt(std::get<1>(itemDropData), std::get<2>(itemDropData));
		out[std::get<0>(itemDropData)] = itemNum;
	}
}

const glm::vec3 FieldAdventureCfg::getDropOffset()
{
	return m_dropOffset;
}

const int FieldAdventureCfg::getDropTime()
{
	return m_dropTime;
}

//LuaTable FieldAdventureCfg::getFieldAdventureItem_lua(int idTask)
//{
//	LuaTable RewardTable{};
////	const FieldAdventureInfo* pTaskData = nullptr;
//	auto it = m_mapFieldAdventureInfo.find(idTask);
//	if (it != m_mapFieldAdventureInfo.end())
//	{
//		auto pDropData = DropCfg::getInstance().findDropType(it->second.n_rewardID);
//		int RewardIndex = 1;
//
//		//经验
//		if (it->second.n_addExp > 0)
//		{
//			LuaTable dropItemData{};
//			auto pItemName = ItemCfg::getInstance().getItemById(99)->m_name;
//			dropItemData.set("name", pItemName);
//			dropItemData.set("itemId", 99);
//			dropItemData.set("icon", "item.ps_exp_icon");
//			dropItemData.set("num", it->second.n_addExp);
//			RewardTable.seti(RewardIndex, dropItemData);
//			++RewardIndex;
//		}
//
//		if (pDropData)
//		{
//			for (auto iterDropString = pDropData->tv_param.begin(); iterDropString != pDropData->tv_param.end(); ++iterDropString) {
//				std::vector<NS::tstring> v1;
//				NS::StringUtil::splitString(v1, *iterDropString, "[");
//				if ((int)v1.size() >= 2) {
//					bzbee::StringList vecDropItemListString;
//					int nDropItemCount = bzbee::StringUtil::splitString(vecDropItemListString, v1[1], ",");
//					if (nDropItemCount >= 3) {
//						auto pItemId = std::atoi(vecDropItemListString[0].c_str());
//						auto pItemIcon = ItemCfg::getInstance().getItemById(pItemId)->m_icon;
//						auto pItemNum = std::atoi(vecDropItemListString[2].c_str());
//						LuaTable dropItemData{};
//						if (pItemId <= 100) {
//							auto pItemName = ItemCfg::getInstance().getItemById(pItemId)->m_name;
//							dropItemData.set("name", pItemName);
//						}
//						dropItemData.set("itemId", pItemId);
//						dropItemData.set("icon", pItemIcon);
//						dropItemData.set("num", pItemNum);
//						RewardTable.seti(RewardIndex, dropItemData);
//						++RewardIndex;
//					}
//				}
//			}
//		}
//	}
//
//	return RewardTable;
//}

const FieldAdventureMobInfo* FieldAdventureCfg::FindFieldAdventureMobInfo(int idMob)
{
	const FieldAdventureMobInfo* pMobData = nullptr;
	auto it = m_mapFieldAdventureMobInfo.find(idMob);
	if (it != m_mapFieldAdventureMobInfo.end())
	{
		pMobData = &(it->second);
	}
	return pMobData;
}

// 地图数据
const FieldAdventureMapInfo* FieldAdventureCfg::FindFieldAdventureMapInfo(int idMap)
{
	const FieldAdventureMapInfo* pMapData = nullptr;
	auto it = m_mapFieldAdventureMapInfo.find(idMap);
	if (it != m_mapFieldAdventureMapInfo.end())
	{
		pMapData = &(it->second);

	}
	return pMapData;
}

int FieldAdventureCfg::getBiomeMiningRewardId(NS::I64 biomeId)
{
	return m_biome2MiningRewardId[biomeId];
}

//是否是野外探险场景
bool FieldAdventureCfg::IsFieldAdventureFightMap(int idMap)
{
	if (m_mapFieldAdventureMapInfo.find(idMap) != m_mapFieldAdventureMapInfo.end())
	{
		return true;
	}
	return false;
}

bool FieldAdventureCfg::isCanUnLockNextBiome(int nBiome, int nStage)
{
	auto itBiome = m_mapFieldAdventureInfoSortBiome.find(nBiome);
	if (itBiome != m_mapFieldAdventureInfoSortBiome.end())
	{
		auto itStage = itBiome->second.find(nStage);
		if (itStage != itBiome->second.end())
		{
			if (itStage->second->n_unlockbiome > 0)
			{
				return true;
			}
		}
	}

	return false;
}

// 解析 NpcAutomaticCfg.xml 文件
bool FieldAdventureCfg::parseNpcAutomaticConfig(tinyxml2::XMLElement* pXmlRoot)
{
	m_mapFieldAdventureInfo.clear();
	m_mapFieldAdventureInfoSortBiome.clear();

	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		FieldAdventureInfo data;

		NS::XmlUtil::getAttribute(data.n_ID, xmlItem, "n_ID"); // = proto.n_id();
		NS::XmlUtil::getAttribute(data.s_namekey, xmlItem, "s_namekey"); // = proto.s_namekey();
		NS::XmlUtil::getAttribute(data.n_mapID, xmlItem, "n_mapID"); // = proto.n_mapid();
		NS::XmlUtil::getAttribute(data.n_rewardID, xmlItem, "n_rewardID"); // = proto.n_rewardid();
		NS::XmlUtil::getAttribute(data.s_Icon, xmlItem, "s_ICON"); // = proto.s_icon();
		NS::XmlUtil::getAttribute(data.s_missiontipskey, xmlItem, "s_missiontipskey"); // = proto.s_missiontipskey();

		NS::XmlUtil::getAttribute(data.n_addExp, xmlItem, "n_addExp"); // = proto.n_addexp();
		NS::XmlUtil::getAttribute(data.n_AdventureRe, xmlItem, "n_AdventureRe"); // = proto.n_adventurere();
		NS::XmlUtil::getAttribute(data.n_MineRe, xmlItem, "n_MineRe"); // = proto.n_minere();
		NS::XmlUtil::getAttribute(data.n_biome, xmlItem, "n_biome"); // = proto.n_biome();
		NS::XmlUtil::getAttribute(data.n_num, xmlItem, "n_num"); // = proto.n_num();
		NS::XmlUtil::getAttribute(data.n_unlockbiome, xmlItem, "n_unlockbiome"); // = proto.n_unlockbiome();

		std::string s_monsterid;

		bzbee::StringList strMobIDList;
		NS::XmlUtil::getAttribute(s_monsterid, xmlItem, "s_monsterID");
		int nMobIDCount = bzbee::StringUtil::splitString(strMobIDList, s_monsterid, ";");

		for (int i = 0; i < nMobIDCount; ++i)
		{
			const bzbee::tstring& s_MobData = strMobIDList[i];
			bzbee::StringList strMobDataList;
			int nMobDataCount = bzbee::StringUtil::splitString(strMobDataList, s_MobData, ":");
			if (nMobDataCount < 2)
			{
				continue;
			}
			int nMobID = std::atoi(strMobDataList[0].c_str());
			int nMobNum = std::atoi(strMobDataList[1].c_str());
			FieldAdventureInfo::FieldAdventureMobInfo dataMob{};
			dataMob.nMobID = nMobID;
			dataMob.nMobNum = nMobNum;

			data.vecMobData.push_back(dataMob);
		}

		if (m_biome2MiningRewardId[data.n_biome] <= 0)
		{
			m_biome2MiningRewardId[data.n_biome] = data.n_MineRe;
		}

		auto& mapBiome = m_mapFieldAdventureInfoSortBiome[data.n_biome];
		mapBiome.emplace(std::make_pair(data.n_num, &data));
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}


// 解析 NpcAutomaticMob.xml 文件
//bool FieldAdventureCfg::parseNpcAutomaticMobBinary()
//{
//	m_mapFieldAdventureMobInfo.clear();
//
//    config::NpcAutomaticMobSet dataList;
//    if (!loadProtoConfig(dataList, "config/NpcAutomaticMob.bin")) return false;
//
//    int itemSize = dataList.npcautomaticmobs_size();
//    for (int i = 0; i < itemSize; ++i)
//	{
//        auto& proto = dataList.npcautomaticmobs(i);
//        auto& data = m_mapFieldAdventureMobInfo[proto.n_id()];
//
//		data.n_ID = proto.n_id();
//		data.s_MonsterIcon = proto.s_monstericon();
//		data.s_MonsterTips = proto.s_monstertips();
//		data.n_PowerBuff = proto.n_powerbuff();
//		data.n_AblityBuff = proto.n_ablitybuff();
//		data.n_AIType = proto.n_aitype();
//		data.f_BornPlaceX = proto.f_bornplacex();
//		data.f_BornPlaceY = proto.f_bornplacey();
//		data.f_BornPlaceZ = proto.f_bornplacez();
//	}
//
//	return true;
//}


// 解析 NpcAutomaticMaps.xml 文件
//bool FieldAdventureCfg::parseNpcAutomaticMapsBinary()
//{
//	m_mapFieldAdventureMapInfo.clear();
//
//    config::NpcAutomaticMapsSet dataList;
//    if (!loadProtoConfig(dataList, "config/NpcAutomaticMaps.bin")) return false;
//
//    int itemSize = dataList.npcautomaticmapss_size();
//    for (int i = 0; i < itemSize; ++i)
//	{
//        auto& proto = dataList.npcautomaticmapss(i);
//        auto& data = m_mapFieldAdventureMapInfo[proto.n_mapid()];
//
//		data.n_ID = proto.n_id();
//		data.n_mapID = proto.n_mapid();
//		data.f_FT_coordinateX1 = proto.f_ft_coordinatex1();
//		data.f_FT_coordinateY1 = proto.f_ft_coordinatey1();
//        data.f_FT_coordinateX2 = proto.f_ft_coordinatex2();
//        data.f_FT_coordinateY2 = proto.f_ft_coordinatey2();
//		data.f_FT_radiusX = proto.f_ft_radiusx();
//		data.f_FT_radiusZ = proto.f_ft_radiusz();
//		data.f_AT_coordinateX1 = proto.f_at_coordinatex1();
//		data.f_AT_coordinateY1 = proto.f_at_coordinatey1();
//        data.f_AT_coordinateX2 = proto.f_at_coordinatex2();
//        data.f_AT_coordinateY2 = proto.f_at_coordinatey2();
//		data.f_AT_radiusX = proto.f_at_radiusx();
//		data.f_AT_radiusZ = proto.f_at_radiusz();
//		data.s_MapNameKey = proto.s_mapnamekey();
//
//        bzbee::StringUtil::splitString(data.vec3CameraPos, proto.s_camerapos(), ";");
//		bzbee::StringUtil::splitString(data.vec3CameraLookAtPos, proto.s_cameralookatpos(), ";");
//        //data.f_CameraZoom = proto.f_camerazoom();
//
//        std::vector<int> intArray;
//		if (bzbee::StringUtil::splitString(intArray, proto.s_mapboundary(), ";") == 4)
//		{
//            data.m_vec2MapBoundaryLeftUpper.x = intArray[0];
//            data.m_vec2MapBoundaryLeftUpper.y = intArray[1];
//            data.m_vec2MapBoundaryLowerRight.x = intArray[2];
//            data.m_vec2MapBoundaryLowerRight.y = intArray[3];
//        }
//	}
//
//	return true;
//}

FieldAdventureCfg* LUA(get_FieldAdventureCfg)(void)
{
	return &FieldAdventureCfg::getInstance();
}
