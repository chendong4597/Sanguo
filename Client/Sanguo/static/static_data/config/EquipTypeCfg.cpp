/*!
 * \file GlobalCfg.cpp
 *
 * \author cd
 * \date 2020/04/07
 *
 *
 */
#include "stdafx.h"
#include "EquipTypeCfg.h"
#include "XmlUtil.h"
#include "StringUtil.h"




EquipTypeCfg& EquipTypeCfg::getInstance()
{
	static EquipTypeCfg s_EquipTypeCfg;
	return s_EquipTypeCfg;
}

bool EquipTypeCfg::initializeCfg()
{

	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/EquipTpy.bin")) return false;

	return parseConfig(doc.RootElement());
}

void EquipTypeCfg::terminate()
{
	m_mapEquipType.clear();
}

bool EquipTypeCfg::parseConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto pAttrXml = p->FirstChildElement("Attribute");
	while (pAttrXml)
	{
		std::string strKey;
		NS::XmlUtil::getAttribute(strKey, pAttrXml, "Name");
		if (strKey == "s_manPath")
		{
			NS::XmlUtil::getAttribute(m_strManPath , pAttrXml, "Value");
		}
		else if (strKey == "s_womanPath") {
			NS::XmlUtil::getAttribute(m_strWomanPath , pAttrXml, "Value");
		}
		else if (strKey == "s_manBodyNonePath")
		{
			NS::XmlUtil::getAttribute(m_strManBodyPath, pAttrXml, "Value");
		}
		else if (strKey == "s_womanBodyNonePath") {
			NS::XmlUtil::getAttribute(m_strWomanBodyPath, pAttrXml, "Value");
		}
		else if (strKey == "s_manLegsNonePath")
		{
			NS::XmlUtil::getAttribute(m_strManLegsPath, pAttrXml, "Value");
		}
		else if (strKey == "s_womanLegsNonePath") {
			NS::XmlUtil::getAttribute(m_strWomanLegsPath, pAttrXml, "Value");
		}
		else if (strKey == "s_DmanBodyNonePath")
		{
			NS::XmlUtil::getAttribute(m_strDifuseManBodyPath, pAttrXml, "Value");
		}
		else if (strKey == "s_DwomanBodyNonePath") {
			NS::XmlUtil::getAttribute(m_strDifuseWomanBodyPath, pAttrXml, "Value");
		}
		else if (strKey == "s_DmanLegsNonePath")
		{
			NS::XmlUtil::getAttribute(m_strDifuseManLegsPath, pAttrXml, "Value");
		}
		else if (strKey == "s_DwomanLegsNonePath") {
			NS::XmlUtil::getAttribute(m_strDifuseWomanLegsPath, pAttrXml, "Value");
		}
		pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	}

	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{
		EquipCfgInfo info;
		NS::XmlUtil::getAttribute(info.n_id, xmlItem , "n_ID");
		NS::XmlUtil::getAttribute(info.n_Type , xmlItem, "n_Type");
		NS::XmlUtil::getAttribute(info.n_Type, xmlItem, "n_Wrap");
		NS::XmlUtil::getAttribute(info.s_ManFoder, xmlItem , "s_ManFoder");
		NS::XmlUtil::getAttribute(info.s_WomanFoder, xmlItem, "s_WomanFoder");
		NS::XmlUtil::getAttribute(info.s_ManHead, xmlItem, "s_Head");
		NS::XmlUtil::getAttribute(info.s_ManBody, xmlItem, "s_Body");
		NS::XmlUtil::getAttribute(info.s_ManLegs, xmlItem, "s_Legs");
		NS::XmlUtil::getAttribute(info.s_ManCape, xmlItem, "s_Cape");
		NS::XmlUtil::getAttribute(info.s_ManWrap, xmlItem, "s_Wrap");
		NS::XmlUtil::getAttribute(info.s_WomanHead, xmlItem, "s_Head1");
		NS::XmlUtil::getAttribute(info.s_WomanBody, xmlItem, "s_Body1");
		NS::XmlUtil::getAttribute(info.s_WomanLegs, xmlItem, "s_Legs1");
		NS::XmlUtil::getAttribute(info.s_WomanCape, xmlItem, "s_Cape1");
		NS::XmlUtil::getAttribute(info.s_WomanWrap, xmlItem, "s_Wrap1");
		NS::XmlUtil::getAttribute(info.n_ShowHair, xmlItem, "n_ShowHair");
		m_mapEquipType[info.n_id] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}
	return true;
}

const EquipTypeCfg::EquipCfgInfo* EquipTypeCfg::GetEquipType(int id)
{
	if (m_mapEquipType.find(id) != m_mapEquipType.end())
	{
		return &(m_mapEquipType[id]);
	}
	return nullptr;
}

std::string EquipTypeCfg::GetEquipPath(int nSex, int nId, EQUIP_SHOW_TPY nShowTpy)
{
	auto&& pCfg = GetEquipType(nId);
	if (!pCfg)
	{
		return "";
	}
	std::string strMeshPath;
	std::string strSubFoder;
	switch (nShowTpy)
	{
	case EQUIP_SHOW_HEAD: {strMeshPath = nSex == 0 ? pCfg->s_ManHead : pCfg->s_WomanHead;strSubFoder = "head";}break;
	case EQUIP_SHOW_BODY: {strMeshPath = nSex == 0 ? pCfg->s_ManBody : pCfg->s_WomanBody;strSubFoder = "body";}break;
	case EQUIP_SHOW_LEGS: {strMeshPath = nSex == 0 ? pCfg->s_ManLegs : pCfg->s_WomanLegs;strSubFoder = "legs";}break;
	case EQUIP_SHOW_CAPE: {strMeshPath = nSex == 0 ? pCfg->s_ManCape : pCfg->s_WomanCape;strSubFoder = "cape";}break;
	case EQUIP_SHOW_WRAP: {strMeshPath = nSex == 0 ? pCfg->s_ManWrap : pCfg->s_WomanWrap;}break;
	default:return "";break;
	}
	if (strMeshPath.empty())
	{
		return "";
	}
	if (EQUIP_SHOW_WRAP != nShowTpy)
	{
		return fmt::format("{0}/{1}/{2}/{3}.{4}", nSex == 0 ? m_strManPath : m_strWomanPath, nSex == 0 ? pCfg->s_ManFoder : pCfg->s_WomanFoder
			, strSubFoder , strMeshPath , strMeshPath);
	}
	return fmt::format("{0}/{1}/{2}.{3}", nSex == 0 ? m_strManPath : m_strWomanPath, nSex == 0 ? pCfg->s_ManFoder : pCfg->s_WomanFoder , strMeshPath, strMeshPath);
}

std::string EquipTypeCfg::GetDifusePath(int nSex, int nId, EQUIP_SHOW_TPY nShowTpy)
{
	auto&& pCfg = GetEquipType(nId);
	if (!pCfg)
	{
		return "";
	}
	std::string strMeshPath;
	std::string strSubFoder;
	switch (nShowTpy)
	{
	case EQUIP_SHOW_HEAD: {strSubFoder = "head";}break;
	case EQUIP_SHOW_BODY: {strSubFoder = "body";}break;
	case EQUIP_SHOW_LEGS: {strSubFoder = "legs";}break;
	case EQUIP_SHOW_CAPE: {strSubFoder = "cape";}break;
	default:return "";break;
	}
	if (EQUIP_SHOW_WRAP != nShowTpy)
	{
		return fmt::format("{0}/{1}/{2}/diffuse.diffuse", nSex == 0 ? m_strManPath : m_strWomanPath, nSex == 0 ? pCfg->s_ManFoder : pCfg->s_WomanFoder
			, strSubFoder);
	}
	return fmt::format("{0}/{1}/diffuse.diffuse", nSex == 0 ? m_strManPath : m_strWomanPath, nSex == 0 ? pCfg->s_ManFoder : pCfg->s_WomanFoder);
}

std::string EquipTypeCfg::GetHightstPath(int nSex, int nId, EQUIP_SHOW_TPY nShowTpy)
{
	auto&& pCfg = GetEquipType(nId);
	if (!pCfg)
	{
		return "";
	}
	std::string strMeshPath;
	std::string strSubFoder;
	switch (nShowTpy)
	{
	case EQUIP_SHOW_HEAD: {strSubFoder = "head";}break;
	case EQUIP_SHOW_BODY: {strSubFoder = "body";}break;
	case EQUIP_SHOW_LEGS: {strSubFoder = "legs";}break;
	case EQUIP_SHOW_CAPE: {strSubFoder = "cape";}break;
	default:return "";break;
	}
	if (EQUIP_SHOW_WRAP != nShowTpy)
	{
		return fmt::format("{0}/{1}/{2}/highest.highest", nSex == 0 ? m_strManPath : m_strWomanPath, nSex == 0 ? pCfg->s_ManFoder : pCfg->s_WomanFoder
			, strSubFoder);
	}
	return fmt::format("{0}/{1}/highest.highest", nSex == 0 ? m_strManPath : m_strWomanPath, nSex == 0 ? pCfg->s_ManFoder : pCfg->s_WomanFoder);
}

std::string EquipTypeCfg::GetNonePath(int nSex, bool bBody)
{
	if (nSex == 0)
	{
		if (bBody)
		{
			return m_strManBodyPath;
		}
		else {
			return m_strManLegsPath;
		}
	}
	else {
		if (bBody)
		{
			return m_strWomanBodyPath;
		}
		else {
			return m_strWomanLegsPath;
		}
	}
}


std::string EquipTypeCfg::GetNoneDifusePath(int nSex, bool bBody)
{
	if (nSex == 0)
	{
		if (bBody)
		{
			return m_strDifuseManBodyPath;
		}
		else {
			return m_strDifuseManLegsPath;
		}
	}
	else {
		if (bBody)
		{
			return m_strDifuseWomanBodyPath;
		}
		else {
			return m_strDifuseWomanLegsPath;
		}
	}
}



