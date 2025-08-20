/*!
 * \file GlobalCfg.cpp
 *
 * \author cd
 * \date 2020/04/07
 *
 *
 */
#include "stdafx.h"
#include "WeaponTypeCfg.h"
#include "XmlUtil.h"
#include "StringUtil.h"


WeaponTypeCfg& WeaponTypeCfg::getInstance()
{
	static WeaponTypeCfg s_WeaponTypeCfg;
	return s_WeaponTypeCfg;
}

bool WeaponTypeCfg::initializeCfg()
{

	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/WeaponType.bin")) return false;

	return parseConfig(doc.RootElement());
}

void WeaponTypeCfg::terminate()
{
	m_mapWeaponType.clear();
}

bool WeaponTypeCfg::parseConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto pAttrXml = p->FirstChildElement("Attribute");
	while (pAttrXml)
	{
		std::string strKey;
		NS::XmlUtil::getAttribute(strKey, pAttrXml, "Name");
		if (strKey == "s_warriorPath")
		{
			NS::XmlUtil::getAttribute(m_strWarriorPath, pAttrXml, "Value");
		}
		else if (strKey == "s_mushaPath") {
			NS::XmlUtil::getAttribute(m_strMushaPath, pAttrXml, "Value");
		}
		else if (strKey == "s_magePath") {
			NS::XmlUtil::getAttribute(m_strMagePath, pAttrXml, "Value");
		}
		else if (strKey == "s_bravoPath") {
			NS::XmlUtil::getAttribute(m_strBravoPath, pAttrXml, "Value");
		}

		pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	}

	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{
		WeaponTypeCfgInfo info;
		NS::XmlUtil::getAttribute(info.n_id, xmlItem , "n_ID");
		NS::XmlUtil::getAttribute(info.n_Type , xmlItem, "n_Type");
		NS::XmlUtil::getAttribute(info.s_SocketName1 , xmlItem , "s_SocketName1");
		NS::XmlUtil::getAttribute(info.s_SocketName2, xmlItem, "s_SocketName2");
		NS::XmlUtil::getAttribute(info.s_DemoPath, xmlItem, "s_DemoPath");
		NS::XmlUtil::getAttribute(info.s_desc, xmlItem, "s_desc");
		NS::XmlUtil::getAttribute(info.n_Combo, xmlItem, "n_Combo");
		NS::XmlUtil::getAttribute(info.n_HitHurt, xmlItem, "n_HitHurt");
		for (int i = 0 ; i < 5 ; i++)
		{
			std::string str = fmt::format("s_Ani{0}" , i + 1);
			std::string strInfo;
			std::string strSound;
			NS::XmlUtil::getAttribute(strInfo , xmlItem, str.c_str());
			str = fmt::format("s_sound{0}", i + 1);
			NS::XmlUtil::getAttribute(strSound, xmlItem, str.c_str());
			if (!strInfo.empty())
			{
				info.vecAniMotoPaths.push_back(strInfo);
				info.vecAniSoundPaths.push_back(strSound);
			}
		}
		NS::XmlUtil::getAttribute(info.s_Foder, xmlItem, "s_Foder");
		NS::XmlUtil::getAttribute(info.s_AniS1, xmlItem, "s_Ani_S1");
		NS::XmlUtil::getAttribute(info.s_AniE1, xmlItem, "s_Ani_E1");
		NS::XmlUtil::getAttribute(info.s_AniS2, xmlItem, "s_Ani_S2");
		NS::XmlUtil::getAttribute(info.s_AniE2, xmlItem, "s_Ani_E2");
		NS::XmlUtil::getAttribute(info.s_Death, xmlItem, "s_Death");
		NS::XmlUtil::getAttribute(info.s_Hit, xmlItem, "s_Hit");
		NS::XmlUtil::getAttribute(info.n_Flip, xmlItem, "n_Flip");
		m_mapWeaponType[info.n_Type] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}
	return true;
}

const WeaponTypeCfg::WeaponTypeCfgInfo* WeaponTypeCfg::GetWeaponType(int id)
{
	if (m_mapWeaponType.find(id) != m_mapWeaponType.end())
	{
		return &(m_mapWeaponType[id]);
	}
	return nullptr;
}

std::string WeaponTypeCfg::GetProfessPath(int nProfessIdx)
{
	switch (nProfessIdx)
	{
	case 0:
		return m_strWarriorPath;
		break;
	case 1:
		return m_strMushaPath;
		break;
	case 2:
		return m_strMagePath;
		break;
	case 3:
		return m_strBravoPath;
		break;
	default:
		break;
	}
	return m_strWarriorPath;
}

std::string WeaponTypeCfg::GetAniPath(int nProfessIdx, int weaponTpy, int comboIdx)
{
	std::string strPath = GetProfessPath(nProfessIdx);
	auto&& pCfg = GetWeaponType(weaponTpy);
	if (pCfg == NULL){
		return "";
	}
	std::string strAni;
	if (comboIdx < pCfg->vecAniMotoPaths.size())
	{
		strAni = pCfg->vecAniMotoPaths[comboIdx];
	}
	else {
		strAni = pCfg->vecAniMotoPaths[0];
	}
	return fmt::format("{0}{1}/{2}.{3}", strPath, pCfg->s_Foder, strAni, strAni);
}

std::string WeaponTypeCfg::GetSoundPath(int nPrefessIdx, int weaponTpy, int comboIdx)
{
	auto&& pCfg = GetWeaponType(weaponTpy);
	if (pCfg == NULL) {
		return "";
	}
	std::string strAni;
	if (comboIdx < pCfg->vecAniSoundPaths.size())
	{
		strAni = pCfg->vecAniSoundPaths[comboIdx];
	}
	else {
		strAni = pCfg->vecAniSoundPaths[0];
	}
	return fmt::format("SoundWave'/Game/Sound/effect/{0}.{1}'", strAni, strAni);
}

//****************************************************************************************
//
//****************************************************************************************
std::string WeaponTypeCfg::GetDeathPath(int nProfessIdx, int weaponTpy)
{
	std::string strPath = GetProfessPath(nProfessIdx);
	auto&& pCfg = GetWeaponType(weaponTpy);
	if (pCfg == NULL) {
		return "";
	}
	return fmt::format("{0}{1}/{2}.{3}", strPath, pCfg->s_Foder, pCfg->s_Death , pCfg->s_Death);
}

//****************************************************************************************
//
//****************************************************************************************
std::string WeaponTypeCfg::GetContinueAniPath(int nProfessIdx, int weaponTpy, int nIdx)
{
	std::string strPath = GetProfessPath(nProfessIdx);
	
	auto&& pCfg = GetWeaponType(weaponTpy);
	if (pCfg == NULL) {
		return "";
	}
	std::string strAni = nIdx == 1 ? pCfg->s_AniS1 : pCfg->s_AniS2;
	return fmt::format("{0}{1}/{2}.{3}", strPath, pCfg->s_Foder, strAni, strAni);
}


