/*!
 * \file GlobalCfg.cpp
 *
 * \author cd
 * \date 2020/04/07
 *
 *
 */
#include "stdafx.h"
#include "SkillFrontEffectCfg.h"
#include "XmlUtil.h"
#include "StringUtil.h"




SkillFrontEffectCfg& SkillFrontEffectCfg::getInstance()
{
	static SkillFrontEffectCfg s_SkillFrontCfg;
	return s_SkillFrontCfg;
}

bool SkillFrontEffectCfg::initializeCfg()
{

	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/SkillFrontEffectCfg.bin")) return false;

	return parseConfig(doc.RootElement());
}

void SkillFrontEffectCfg::terminate()
{
	m_mapSkillType.clear();
}

bool SkillFrontEffectCfg::parseConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto pAttrXml = p->FirstChildElement("Attribute");
	while (pAttrXml)
	{
		std::string strKey;
		NS::XmlUtil::getAttribute(strKey, pAttrXml, "Name");
		if (strKey == "s_Path")
		{
			NS::XmlUtil::getAttribute(m_strPath, pAttrXml, "Value");
		}
		pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	}

	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{
		int n_Type{};
		std::string s_SockName{};
		std::string s_DemoPath{};
		std::string f_tm;
		std::string s_desc{};
		SkillFrontEffectInfo info;
		NS::XmlUtil::getAttribute(info.n_id, xmlItem, "n_ID");
		NS::XmlUtil::getAttribute(info.n_Type, xmlItem, "n_Type");
		NS::XmlUtil::getAttribute(info.s_SockName, xmlItem, "s_SockName");
		NS::XmlUtil::getAttribute(info.s_DemoPath, xmlItem, "s_DemoPath");
		NS::XmlUtil::getAttribute(info.f_tm, xmlItem, "f_tm");
		NS::XmlUtil::getAttribute(info.s_desc, xmlItem, "s_desc");

		m_mapSkillType[info.n_id] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}
	return true;
}

const SkillFrontEffectCfg::SkillFrontEffectInfo* SkillFrontEffectCfg::GetSkillFrontType(int id)
{
	if (m_mapSkillType.find(id) != m_mapSkillType.end())
	{
		return &(m_mapSkillType[id]);
	}
	return nullptr;
}




