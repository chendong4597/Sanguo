/*!
 * \file AchieveCfg.cpp
 *
 * \author ChenBK;
 * \date 2017/10/10
 *
 * 
 */
#include "stdafx.h"
#include "HelpCfg.h"
#include "../common/calculate.h"
#include <XmlUtil.h>


HelpCfg& HelpCfg::getInstance()
{
	static HelpCfg s_HelpCfg;
	return s_HelpCfg;
}

bool HelpCfg::initialize()
{
	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/HelpCfg.bin")) return false;
	if (!parseHelpConfig(doc.RootElement())) return false;

    reloadMultiLanguage();

	return true;
}

void HelpCfg::terminate()
{
	m_helpItemMap.clear();
}

void HelpCfg::reloadMultiLanguage()
{
}

bool HelpCfg::parseHelpConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		HelpInfo info;

		NS::XmlUtil::getAttribute(info.n_ID, xmlItem, "n_ID");// = proto.n_id();
		NS::XmlUtil::getAttribute(info.s_title, xmlItem, "s_title");// = proto.s_name();
		NS::XmlUtil::getAttribute(info.s_desc, xmlItem, "s_desc");// = proto.n_type();
		NS::XmlUtil::getAttribute(info.s_icon, xmlItem, "s_icon");//= proto.n_begindetailid();
		m_helpItemMap[info.n_ID] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

const HelpCfg::HelpInfo* HelpCfg::getHelpItemInfo(int id)
{
	auto it = m_helpItemMap.find(id);
	return (it == m_helpItemMap.end()) ? nullptr : &it->second;
}

