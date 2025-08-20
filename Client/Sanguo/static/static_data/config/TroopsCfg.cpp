/*!
 * \file GlobalCfg.cpp
 *
 * \author cd
 * \date 2020/04/07
 *
 *
 */
#include "stdafx.h"
#include "TroopsCfg.h"
#include "XmlUtil.h"
#include "StringUtil.h"




TroopsCfg& TroopsCfg::getInstance()
{
	static TroopsCfg s_TroopsCfg;
	return s_TroopsCfg;
}

bool TroopsCfg::initializeCfg()
{

	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/TroopsCfg.bin")) return false;
	if (!parseConfig(doc.RootElement())) {
		return false;
	}

	return true;
}

void TroopsCfg::terminate()
{
	m_mapTroopsType.clear();
}

bool TroopsCfg::parseConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	//auto pAttrXml = p->FirstChildElement("Attribute");
	//while (pAttrXml)
	//{
	//	std::string strKey;
	//	NS::XmlUtil::getAttribute(strKey, pAttrXml, "Name");
	//	if (strKey == "s_Path")
	//	{
	//		NS::XmlUtil::getAttribute(m_strPath, pAttrXml, "Value");
	//	}
	//	pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	//}

	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{
		TroopsInfo info;
		NS::XmlUtil::getAttribute(info.n_ID, xmlItem , "n_ID");
		NS::XmlUtil::getAttribute(info.s_Name, xmlItem, "s_Name");
		NS::XmlUtil::getAttribute(info.s_icon, xmlItem, "s_icon");
		NS::XmlUtil::getAttribute(info.n_cop, xmlItem, "n_cop");
		std::string s_pos;
		NS::XmlUtil::getAttribute(s_pos, xmlItem, "s_pos");
		NS::StringUtil::splitString(info.vecPos, s_pos, ",");
		NS::XmlUtil::getAttribute(info.n_needLv, xmlItem, "n_needLv");
		NS::XmlUtil::getAttribute(info.n_max, xmlItem, "n_max");
		NS::XmlUtil::getAttribute(info.n_mainPos, xmlItem, "n_mainPos");
		NS::XmlUtil::getAttribute(info.n_buffID, xmlItem, "n_buffID");
		m_mapTroopsType[info.n_ID] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}
	return true;
}

const TroopsCfg::TroopsInfo* TroopsCfg::GetTroopsType(int id)
{
	if (m_mapTroopsType.find(id) != m_mapTroopsType.end())
	{
		return &(m_mapTroopsType[id]);
	}
	return nullptr;
}



