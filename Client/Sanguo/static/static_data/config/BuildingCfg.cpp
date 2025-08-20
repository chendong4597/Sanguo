/*!
* \file ItemCfg.cpp
*
* \author ChenBk
* \date 12/01/2016
*
*
*/
#include "stdafx.h"
#include "BuildingCfg.h"
#include "GlobalCfg.h"
//#include "ConfigLoaderUtil.h"
//#include <StyleModule.h>
#include <Framework.h>
#include <msg/MsgGameDef.pb.h>
//#include <Item.pb.h>
//#include <Cooldown.pb.h>
//#include <ItemTag.pb.h>
//#include <ItemClassname.pb.h>
//#include <msg/MsgGameDef.pb.h>
#include <StringUtil.h>
#include <list>
#include <sstream>

BuildingCfg& BuildingCfg::getInstance()
{
	static BuildingCfg s_BuildingCfg;
	return s_BuildingCfg;
}

bool BuildingCfg::initializeCfg()
{
	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/BuildCfgTemplate.bin")) return false;

	return parseBuildTemplateConfig(doc.RootElement());

    return true;
}

void BuildingCfg::terminate()
{
	m_buildingTemplateMap.clear();
	//m_buildingTemplateByTypeMap.clear();
}

bool BuildingCfg::parseBuildTemplateConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		BuildingTemplate buildingTemplate;

		NS::XmlUtil::getAttribute(buildingTemplate.m_templateId, xmlItem, "n_ID");
		NS::XmlUtil::getAttribute(buildingTemplate.m_typeId, xmlItem, "n_build_type");

		xmlItem = xmlItem->NextSiblingElement("Record");

		m_buildingTemplateMap[buildingTemplate.m_templateId] = buildingTemplate;
    }

    return true;
}

const BuildingCfg::BuildingTemplate* BuildingCfg::findBuildingTemplate(int templateId)
{
	auto it = m_buildingTemplateMap.find(templateId);
	return (it == m_buildingTemplateMap.end()) ? nullptr : &it->second;
}

const BuildingCfg::BuildingTemplate* BuildingCfg::findBuildingTemplateByTypeId(int typeId)
{
	
	for (auto&& iter = m_buildingTemplateMap.begin() ; iter != m_buildingTemplateMap.end() ; iter++)
	{
		if (iter->second.m_typeId == typeId)
		{
			return &(iter->second);
		}
	}

	return nullptr;
}
