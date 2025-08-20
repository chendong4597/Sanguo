/*!
 * \file AchieveCfg.cpp
 *
 * \author ChenBK;
 * \date 2017/10/10
 *
 * 
 */
#include "stdafx.h"
#include "HouseCfg.h"
#include "../common/calculate.h"
#include <XmlUtil.h>


HouseCfg& HouseCfg::getInstance()
{
	static HouseCfg s_HouseCfg;
	return s_HouseCfg;
}

bool HouseCfg::initialize()
{
	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/HouseCfg.bin")) return false;
	if (!parseHoueItemConfig(doc.RootElement())) return false;

	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/HouseTpyCfg.bin")) return false;
	if (!parseHoueTypeConfig(doc.RootElement())) return false;

    reloadMultiLanguage();

	return true;
}

void HouseCfg::terminate()
{
	m_houseItemMap.clear();
	m_houseTypeMap.clear();
}

void HouseCfg::reloadMultiLanguage()
{
}

bool HouseCfg::parseHoueItemConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
        //auto& proto = dataList.achieveevents(i);
		HouseItemInfo info;

		NS::XmlUtil::getAttribute(info.n_ID, xmlItem, "n_ID");// = proto.n_id();
		NS::XmlUtil::getAttribute(info.n_X, xmlItem, "n_X");// = proto.s_name();
		NS::XmlUtil::getAttribute(info.n_Y, xmlItem, "n_Y");// = proto.n_type();
		NS::XmlUtil::getAttribute(info.n_HouseTpy, xmlItem, "n_HouseTpy");//= proto.n_begindetailid();
		m_houseItemMap[info.n_ID] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

bool HouseCfg::parseHoueTypeConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
        //auto& proto = dataList.achievetitles(i);
		HouseTypeInfo info;// = m_titleMap[proto.n_id()];

		NS::XmlUtil::getAttribute(info.n_ID, xmlItem, "n_ID");//  = proto.n_id();
		NS::XmlUtil::getAttribute(info.n_Type, xmlItem, "n_Type");//  = proto.s_name();
		NS::XmlUtil::getAttribute(info.s_icon, xmlItem, "s_icon");
		NS::XmlUtil::getAttribute(info.n_Quality, xmlItem, "n_Quality");
		NS::XmlUtil::getAttribute(info.s_Name, xmlItem, "s_Name");
		NS::XmlUtil::getAttribute(info.s_Path, xmlItem, "s_Path");//  = proto.n_type();
		NS::XmlUtil::getAttribute(info.n_Honor, xmlItem, "n_Honor");//  = proto.n_needpoints();
		NS::XmlUtil::getAttribute(info.s_items, xmlItem, "s_items");//  = proto.n_eventid();
		NS::XmlUtil::getAttribute(info.n_Lev, xmlItem, "n_Lev");//  = proto.n_groupid();
		NS::XmlUtil::getAttribute(info.n_Innum, xmlItem, "n_Innum");//  = proto.n_achieverewardid();
		NS::XmlUtil::getAttribute(info.n_NextId, xmlItem, "n_NextId");//  = proto.s_desc();
		NS::XmlUtil::getAttribute(info.n_upgradeTm, xmlItem, "n_upgradeTm");//  = proto.s_desc();
		NS::XmlUtil::getAttribute(info.n_Prosperous, xmlItem, "n_Prosperous");
		NS::XmlUtil::getAttribute(info.n_RentRate, xmlItem, "n_RentRate");
		NS::XmlUtil::getAttribute(info.n_PlantRate, xmlItem, "n_PlantRate");
		NS::XmlUtil::getAttribute(info.n_FarmRate, xmlItem, "n_FarmRate");
		NS::XmlUtil::getAttribute(info.n_MineRate, xmlItem, "n_MineRate");
		NS::XmlUtil::getAttribute(info.n_ShopRate, xmlItem, "n_ShopRate");
		NS::XmlUtil::getAttribute(info.n_CraftRate, xmlItem, "n_CraftRate");
		NS::XmlUtil::getAttribute(info.n_buildRate, xmlItem, "n_buildRate");

		std::string s_items;
		NS::XmlUtil::getAttribute(s_items, xmlItem, "s_items");

		std::vector<std::string> v_str;
		NS::StringUtil::splitString(v_str, s_items, ",");
		for (size_t i = 0; i < v_str.size(); i++) {
			int nId, nNum = 0;
			int32 count = sscanf(v_str[i].c_str(), "[%d:%d]", &nId, &nNum);
			if (count < 2 || nId == 0) {
				break;
			}
			info.m_NeedItems[nId] = nNum;
		}

		m_houseTypeMap[info.n_ID] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

const HouseCfg::HouseItemInfo* HouseCfg::getHouseItemInfo(int id)
{
	auto it = m_houseItemMap.find(id);
	return (it == m_houseItemMap.end()) ? nullptr : &it->second;
}

const HouseCfg::HouseTypeInfo* HouseCfg::getHouseTypeInfo(int id)
{
	auto it = m_houseTypeMap.find(id);
	return (it == m_houseTypeMap.end()) ? nullptr : &it->second;
}

void HouseCfg::GetVecHouseTpy(int nHouseTpy, std::vector<int>& vecHouse)
{
	vecHouse.clear();
	for (auto&& iter = m_houseTypeMap.begin(); iter != m_houseTypeMap.end(); iter++)
	{
		if (nHouseTpy == HOUSE_SEL_TYP_H && (iter->second.n_Type != HOUSE_TYPE_Normal
			|| iter->second.n_ID == 1000))
		{
			continue;
		} else if (nHouseTpy == HOUSE_SEL_TYP_W && iter->second.n_Type != HOUSE_TYPE_Wall)
		{
			continue;
		}
		else if (nHouseTpy == HOUSE_SEL_TYP_C && iter->second.n_Type != HOUSE_TYPE_Charm)
		{
			continue;
		}
		else if (nHouseTpy == HOUSE_SEL_TYP_R && iter->second.n_Type != HOUSE_TYPE_Road)
		{
			continue;
		}
		vecHouse.push_back(iter->second.n_ID);
	}
	std::sort(vecHouse.begin(), vecHouse.end());
}

