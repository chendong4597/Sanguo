/*!
 * \file GlobalCfg.cpp
 *
 * \author cd
 * \date 2020/04/07
 *
 *
 */
#include "stdafx.h"
#include "WeaponInfoTypeCfg.h"
#include "XmlUtil.h"
#include "StringUtil.h"




WeaponInfoTypeCfg& WeaponInfoTypeCfg::getInstance()
{
	static WeaponInfoTypeCfg s_WeaponInfoTypeCfg;
	return s_WeaponInfoTypeCfg;
}

bool WeaponInfoTypeCfg::initializeCfg()
{

	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/WeaponInfoType.bin")) return false;

	return parseConfig(doc.RootElement());
}

void WeaponInfoTypeCfg::terminate()
{
	m_mapWeaponInfoType.clear();
}

bool WeaponInfoTypeCfg::parseConfig(tinyxml2::XMLElement* pXmlRoot)
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
		} else if (strKey == "s_SoundPath")
		{
			NS::XmlUtil::getAttribute(m_strSoundBase, pAttrXml, "Value");
		}
		pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	}

	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{
		WeaponInfoTypeCfgInfo info;

		NS::XmlUtil::getAttribute(info.n_id, xmlItem , "n_ID");
		NS::XmlUtil::getAttribute(info.n_Type , xmlItem, "n_Type");
		NS::XmlUtil::getAttribute(info.n_isCombo , xmlItem , "n_isCombo");
		NS::XmlUtil::getAttribute(info.n_Flip, xmlItem, "n_Flip");
		NS::XmlUtil::getAttribute(info.n_PreSkillId, xmlItem, "n_PreSkillId");
		NS::XmlUtil::getAttribute(info.n_SkillId, xmlItem, "n_SkillId");
		NS::XmlUtil::getAttribute(info.n_ComboSkill, xmlItem, "n_ComboSkill");
		NS::XmlUtil::getAttribute(info.n_exSkillId, xmlItem, "n_exSkillId");
		NS::XmlUtil::getAttribute(info.n_Speed, xmlItem, "n_Speed");
		NS::XmlUtil::getAttribute(info.n_runStrenth, xmlItem, "n_runStrenth");
		NS::XmlUtil::getAttribute(info.n_restStrenth, xmlItem, "n_restStrenth");
		NS::XmlUtil::getAttribute(info.n_normalStrenth, xmlItem, "n_normalStrenth");
		NS::XmlUtil::getAttribute(info.n_HurtRangeTpy, xmlItem, "n_HurtRangeTpy");
		NS::XmlUtil::getAttribute(info.n_RangeParam1, xmlItem, "n_RangeParam1");
		NS::XmlUtil::getAttribute(info.n_RangeParam2, xmlItem, "n_RangeParam2");
		NS::XmlUtil::getAttribute(info.n_RangeAngle, xmlItem, "n_RangeAngle");
		NS::XmlUtil::getAttribute(info.s_Path, xmlItem, "s_Path");
		NS::XmlUtil::getAttribute(info.s_MainPath, xmlItem, "s_MainPath");
		NS::XmlUtil::getAttribute(info.s_atkPath, xmlItem, "s_atkPath");
		NS::XmlUtil::getAttribute(info.s_RunSound, xmlItem, "s_RunSound");
		NS::XmlUtil::getAttribute(info.s_IdleSound, xmlItem, "s_IdleSound");
		NS::XmlUtil::getAttribute(info.s_Desc, xmlItem, "s_Desc");

		std::string strTexture;
		NS::XmlUtil::getAttribute(strTexture, xmlItem, "s_txture1");
		if (!strTexture.empty())
		{
			info.m_vecTextures.push_back(strTexture);
		}
		NS::XmlUtil::getAttribute(strTexture, xmlItem, "s_txture2");
		if (!strTexture.empty())
		{
			info.m_vecTextures.push_back(strTexture);
		}
		NS::XmlUtil::getAttribute(strTexture, xmlItem, "s_txture3");
		if (!strTexture.empty())
		{
			info.m_vecTextures.push_back(strTexture);
		}
		NS::XmlUtil::getAttribute(strTexture, xmlItem, "s_txture4");
		if (!strTexture.empty())
		{
			info.m_vecTextures.push_back(strTexture);
		}

		xmlItem = xmlItem->NextSiblingElement("Record");
		m_mapWeaponInfoType[info.n_id] = info;
	}
	return true;
}

const WeaponInfoTypeCfg::WeaponInfoTypeCfgInfo* WeaponInfoTypeCfg::GetWeaponById(int id)
{
	if (m_mapWeaponInfoType.find(id) != m_mapWeaponInfoType.end())
	{
		return &(m_mapWeaponInfoType[id]);
	}
	return nullptr;
}

std::string WeaponInfoTypeCfg::GetWeaponTexture(int nId)
{
	auto&& iter = m_mapWeaponInfoType.find(nId);
	if (iter == m_mapWeaponInfoType.end())
	{
		return "";
	}
	return fmt::format("{0}/{1}/diffuse.diffuse", m_strPath, iter->second.s_MainPath);
}

std::string WeaponInfoTypeCfg::GetBPWeaponPath(int id)
{
	auto&& pCfg = GetWeaponById(id);
	if (!pCfg)
	{
		return "";
	}
	return fmt::format("Class'{0}/{1}/{2}.{3}_C'", m_strPath, pCfg->s_MainPath, pCfg->s_Path, pCfg->s_Path);
}

std::string WeaponInfoTypeCfg::GetSkeshWeaponPath(int id)
{
	auto&& pCfg = GetWeaponById(id);
	if (!pCfg)
	{
		return "";
	}
	return fmt::format("SkeletalMesh'{0}/{1}/{2}.{3}'", m_strPath, pCfg->s_MainPath, pCfg->s_Path, pCfg->s_Path);
}

std::string WeaponInfoTypeCfg::GetAnimHorse(int nId)
{
	auto&& pCfg = GetWeaponById(nId);
	if (!pCfg)
	{
		return "";
	}
	return fmt::format("AnimBlueprint'{0}/{1}/ani.ani_C'", m_strPath, pCfg->s_MainPath);
}

std::string WeaponInfoTypeCfg::GetAnimSound(int nId, bool bRun)
{
	auto&& pCfg = GetWeaponById(nId);
	std::string str = bRun ? pCfg->s_RunSound : pCfg->s_IdleSound;
	if (!pCfg || str.empty())
	{
		return "";
	}
	return fmt::format("SoundWave'{0}/{1}.{2}'", m_strSoundBase, str, str);
}

std::string WeaponInfoTypeCfg::GetAttackAniPath(int nWeaponId)
{
	auto&& pCfg = GetWeaponById(nWeaponId);
	if (!pCfg || pCfg->s_atkPath.empty())
	{
		return "";
	}
	return fmt::format("{0}/{1}/{2}.{3}", m_strPath, pCfg->s_MainPath, pCfg->s_atkPath, pCfg->s_atkPath);
}

std::string WeaponInfoTypeCfg::GetTexture(int nWeaponId, int nIdx)
{
	auto&& pCfg = GetWeaponById(nWeaponId);
	if (!pCfg || nIdx >= pCfg->m_vecTextures.size())
	{
		return "";
	}
	return fmt::format("{0}/{1}/{2}.{3}", m_strPath, pCfg->s_MainPath, pCfg->m_vecTextures[nIdx], pCfg->m_vecTextures[nIdx]);
}

int WeaponInfoTypeCfg::GetNextWeaponId(int id)
{
	MAP_WEAPONINFOTYPE::iterator iter = m_mapWeaponInfoType.find(id);

	if (iter != m_mapWeaponInfoType.end())
	{
		iter++;
	}
	if (iter != m_mapWeaponInfoType.end())
	{
		return iter->first;
	}
	iter = m_mapWeaponInfoType.begin();
	return iter->first;
}


