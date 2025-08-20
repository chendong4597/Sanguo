/*!
 * \file GlobalCfg.cpp
 *
 * \author cd
 * \date 2020/04/07
 *
 *
 */
#include "stdafx.h"
#include "BuffTypeCfg.h"
#include "XmlUtil.h"
#include "StringUtil.h"




BuffTypeCfg& BuffTypeCfg::getInstance()
{
	static BuffTypeCfg s_BuffTypeCfg;
	return s_BuffTypeCfg;
}

bool BuffTypeCfg::initializeCfg()
{

	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/BuffType.bin")) return false;

	return parseConfig(doc.RootElement());
}

void BuffTypeCfg::terminate()
{
	m_mapBuffType.clear();
}

bool BuffTypeCfg::parseConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto pAttrXml = p->FirstChildElement("Attribute");
	while (pAttrXml)
	{
		std::string strKey;
		NS::XmlUtil::getAttribute(strKey, pAttrXml, "Name");
		if (strKey == "s_BuffPath")
		{
			NS::XmlUtil::getAttribute(m_strPath, pAttrXml, "Value");
		}
		pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	}

	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{
		BuffTypeInfo info;
		NS::XmlUtil::getAttribute(info.n_id, xmlItem, "n_ID");//
		NS::XmlUtil::getAttribute(info.s_Name, xmlItem, "s_Name");
		NS::XmlUtil::getAttribute(info.s_icon, xmlItem, "s_icon");
		NS::XmlUtil::getAttribute(info.n_nOverlay, xmlItem, "n_nOverlay");
		NS::XmlUtil::getAttribute(info.f_tm, xmlItem, "f_tm");
		NS::XmlUtil::getAttribute(info.n_FollowRat, xmlItem, "n_FollowRot");
		NS::XmlUtil::getAttribute(info.s_MeshTag, xmlItem, "s_MeshTag");
		NS::XmlUtil::getAttribute(info.s_SockName, xmlItem, "s_SockName");
		NS::XmlUtil::getAttribute(info.s_path, xmlItem, "s_path");
		NS::XmlUtil::getAttribute(info.s_desc, xmlItem, "s_desc");
		m_mapBuffType[info.n_id] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}
	return true;
}

const BuffTypeCfg::BuffTypeInfo* BuffTypeCfg::GetBuffType(int id)
{
	if (m_mapBuffType.find(id) != m_mapBuffType.end())
	{
		return &(m_mapBuffType[id]);
	}
	return nullptr;
}




