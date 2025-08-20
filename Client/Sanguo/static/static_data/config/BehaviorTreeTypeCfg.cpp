/*!
 * \file GlobalCfg.cpp
 *
 * \author cd
 * \date 2020/04/07
 *
 *
 */
#include "stdafx.h"
#include "BehaviorTreeTypeCfg.h"
#include "XmlUtil.h"
#include "StringUtil.h"




BehaviorTreeTypeCfg& BehaviorTreeTypeCfg::getInstance()
{
	static BehaviorTreeTypeCfg s_HaviorTreeCfg;
	return s_HaviorTreeCfg;
}

bool BehaviorTreeTypeCfg::initializeCfg()
{
	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/BehaviorTreeType.bin")) return false;

	return parseConfig(doc.RootElement());
}

void BehaviorTreeTypeCfg::terminate()
{
	m_mapTreeType.clear();
}

bool BehaviorTreeTypeCfg::parseConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{
		BehaviorTreeTypeCfgInfo info;
		NS::XmlUtil::getAttribute(info.n_id, xmlItem , "n_ID");
		NS::XmlUtil::getAttribute(info.n_Type, xmlItem, "n_Type");
		NS::XmlUtil::getAttribute(info.s_path , xmlItem, "s_Path");
		m_mapTreeType[info.n_id] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}
	return true;
}

const BehaviorTreeTypeCfg::BehaviorTreeTypeCfgInfo* BehaviorTreeTypeCfg::GetBehaviorType(int id)
{
	if (m_mapTreeType.find(id) != m_mapTreeType.end())
	{
		return &(m_mapTreeType[id]);
	}
	return nullptr;
}


