/*!
* \file PlayerAttrNameCfg.h
*
* \author jiangping huang
* \date 03/02/2017
*
*
*/
#include "stdafx.h"
#include "PlayerAttrNameCfg.h"
#include <StringUtil.h>
#include <XmlUtil.h>

PlayerAttrNameCfg& PlayerAttrNameCfg::getInstance()
{
	static PlayerAttrNameCfg s_PlayerAttrNameCfg;
	return s_PlayerAttrNameCfg;
}

bool PlayerAttrNameCfg::initialize()
{
	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/PlayerAttrName.bin")) return false;
	if (!parsePlayerAttrNameConfig(doc.RootElement())) return false;

	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/PowerCalAttr.bin")) return false;
	if (!parsePowerCalAttrConfig(doc.RootElement())) return false;

    reloadMultiLanguage();
    return true;
}

void PlayerAttrNameCfg::terminate()
{
	m_PlayerAttrNameMap.clear();
	m_atterPowerMap.clear();
}

const PlayerAttrNameCfg::PlayerAttrNameType* PlayerAttrNameCfg::findPlayerAttrNameType(int id) const
{
	auto it = m_PlayerAttrNameMap.find(id);
	if (it == m_PlayerAttrNameMap.end()) return nullptr;
	return &it->second;
}

bool PlayerAttrNameCfg::parsePlayerAttrNameConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;
	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{
		PlayerAttrNameType dorpType{};

		NS::XmlUtil::getAttribute(dorpType.n_Type, xmlItem, "n_ID");// = proto.n_id();
		NS::XmlUtil::getAttribute(dorpType.n_Class, xmlItem, "n_Class");// = proto.n_class();
		NS::XmlUtil::getAttribute(dorpType.m_Name_untrans, xmlItem, "s_name");// = proto.s_name();
		NS::XmlUtil::getAttribute(dorpType.m_Pic, xmlItem, "s_attri_icon");// = proto.s_attri_icon();
		NS::XmlUtil::getAttribute(dorpType.m_PicAchieve, xmlItem, "s_achieve_attri_icon");// = proto.s_achieve_attri_icon();

		m_PlayerAttrNameMap[dorpType.n_Type] = dorpType;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

bool PlayerAttrNameCfg::parsePowerCalAttrConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;
	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{
		AtterPower power{};

		NS::XmlUtil::getAttribute(power.id, xmlItem, "n_ID");// = proto.n_id();
		NS::XmlUtil::getAttribute(power.value, xmlItem, "n_calType");// = proto.n_caltype();
		NS::XmlUtil::getAttribute(power.power, xmlItem, "n_Power");// = proto.n_power();

		m_atterPowerMap[power.id] = power;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

const PlayerAttrNameCfg::AtterPower* PlayerAttrNameCfg::getPowerCfg(int id)
{
	auto it = m_atterPowerMap.find(id);
	if (it == m_atterPowerMap.end()) return nullptr;
	return &it->second;
}

bool PlayerAttrNameCfg::reloadMultiLanguage()
{
	for (auto& item : m_PlayerAttrNameMap)
	{
		item.second.m_Name = _LC(item.second.m_Name_untrans);
	}

	return true;
}
