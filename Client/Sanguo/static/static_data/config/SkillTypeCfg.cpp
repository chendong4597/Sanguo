/*!
 * \file GlobalCfg.cpp
 *
 * \author cd
 * \date 2020/04/07
 *
 *
 */
#include "stdafx.h"
#include "SkillTypeCfg.h"
#include "XmlUtil.h"
#include "StringUtil.h"




SkillTypeCfg& SkillTypeCfg::getInstance()
{
	static SkillTypeCfg s_SkillTypeCfg;
	return s_SkillTypeCfg;
}

bool SkillTypeCfg::initializeCfg()
{

	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/SkillType.bin")) return false;

	return parseConfig(doc.RootElement());
}

void SkillTypeCfg::terminate()
{
	m_mapSkillType.clear();
}

bool SkillTypeCfg::parseConfig(tinyxml2::XMLElement* pXmlRoot)
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
		else if (strKey == "s_HorsePath")
		{
			NS::XmlUtil::getAttribute(m_strHorseIconPath, pAttrXml, "Value");
		}
		else if (strKey == "s_IconPath")
		{
			NS::XmlUtil::getAttribute(m_strIconPath, pAttrXml, "Value");
		}
		
		pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	}

	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{
		SkillTypeInfo info;
		NS::XmlUtil::getAttribute(info.n_id, xmlItem, "n_ID");
		NS::XmlUtil::getAttribute(info.n_SkillTpy, xmlItem, "n_Type");
		NS::XmlUtil::getAttribute(info.s_Name, xmlItem, "s_Name");
		NS::XmlUtil::getAttribute(info.s_icon, xmlItem, "s_icon");
		NS::XmlUtil::getAttribute(info.f_CoolTime, xmlItem, "f_CoolTime");
		NS::XmlUtil::getAttribute(info.s_AniIdx, xmlItem, "s_AniIdx");
		NS::XmlUtil::getAttribute(info.n_AniTpy, xmlItem, "n_AniTpy");
		NS::XmlUtil::getAttribute(info.n_AttackStopMove, xmlItem, "n_AttackStopMove");
		NS::XmlUtil::getAttribute(info.f_Mana, xmlItem, "f_Mana");
		NS::XmlUtil::getAttribute(info.f_Hp, xmlItem, "f_Hp");
		NS::XmlUtil::getAttribute(info.n_Sten, xmlItem, "n_Sten");
		NS::XmlUtil::getAttribute(info.n_Anger, xmlItem, "n_Anger");
		NS::XmlUtil::getAttribute(info.n_AtkType, xmlItem, "n_AtkType");
		NS::XmlUtil::getAttribute(info.n_ScrollHit, xmlItem, "n_ScrollHit");
		NS::XmlUtil::getAttribute(info.n_nCastDis, xmlItem, "n_nCastDis");
		NS::XmlUtil::getAttribute(info.n_atk, xmlItem, "n_atk");
		NS::XmlUtil::getAttribute(info.n_magic, xmlItem, "n_magic");
		NS::XmlUtil::getAttribute(info.s_sockName, xmlItem, "s_sockName");
		NS::XmlUtil::getAttribute(info.n_num, xmlItem, "n_num");
		NS::XmlUtil::getAttribute(info.n_flash, xmlItem, "n_flash");
		NS::XmlUtil::getAttribute(info.n_curve, xmlItem, "n_curve");
		NS::XmlUtil::getAttribute(info.n_trace, xmlItem, "n_trace");
		NS::XmlUtil::getAttribute(info.n_AttackTarget, xmlItem, "n_AttackTarget");
		NS::XmlUtil::getAttribute(info.n_needTarget, xmlItem, "n_needTarget");
		NS::XmlUtil::getAttribute(info.n_BomrangHei, xmlItem, "n_BomrangHei");
		NS::XmlUtil::getAttribute(info.n_speed, xmlItem, "n_speed");
		NS::XmlUtil::getAttribute(info.f_rate, xmlItem, "f_rate");
		NS::XmlUtil::getAttribute(info.n_length, xmlItem, "n_length");
		NS::XmlUtil::getAttribute(info.f_maxTm, xmlItem, "f_maxTm");
		NS::XmlUtil::getAttribute(info.f_interTm, xmlItem, "f_interTm");
		NS::XmlUtil::getAttribute(info.n_buffId, xmlItem, "n_buffId");
		NS::XmlUtil::getAttribute(info.s_demoPath, xmlItem, "s_demoPath");
		NS::XmlUtil::getAttribute(info.f_continueInterTm, xmlItem, "f_continueInterTm");
		NS::XmlUtil::getAttribute(info.n_hitdead, xmlItem, "n_hitdead");
		NS::XmlUtil::getAttribute(info.n_frontSkill, xmlItem, "n_frontSkill");
		NS::XmlUtil::getAttribute(info.f_dieTm, xmlItem, "f_dieTm");
		std::string strNxtSkills;
		NS::XmlUtil::getAttribute(strNxtSkills, xmlItem, "s_nxtSkill");
		NS::StringUtil::splitString(info.m_vecNxtSkills, strNxtSkills, ",");
		if (info.m_vecNxtSkills.size() > 0) {
			info.n_nxtSkill = info.m_vecNxtSkills[0];
		}

		//伤害范围计算
		NS::XmlUtil::getAttribute(info.n_DamageTpy, xmlItem, "n_DamageTpy");
		NS::XmlUtil::getAttribute(info.n_BpDamageTpy, xmlItem, "n_BpDamageTpy");
		NS::XmlUtil::getAttribute(info.n_DamageParam1, xmlItem, "n_DamageParam1");
		NS::XmlUtil::getAttribute(info.n_DamageParam2, xmlItem, "n_DamageParam2");
		NS::XmlUtil::getAttribute(info.n_DamageParam3, xmlItem, "n_DamageParam3");
		NS::XmlUtil::getAttribute(info.n_DamageParam4, xmlItem, "n_DamageParam4");

		NS::XmlUtil::getAttribute(info.m_nNxtMaxNum, xmlItem, "n_nxtNum");
		NS::XmlUtil::getAttribute(info.s_hitEffect, xmlItem, "s_hitEffect");
		NS::XmlUtil::getAttribute(info.s_beginEff, xmlItem, "s_beginEff");
		NS::XmlUtil::getAttribute(info.s_meshPath, xmlItem, "s_meshPath");
		NS::XmlUtil::getAttribute(info.s_sound, xmlItem, "s_sound");
		NS::XmlUtil::getAttribute(info.s_desc, xmlItem, "s_desc");

		NS::StringUtil::splitString(info.n_vecAniIdx, info.s_AniIdx, ",");
		m_mapSkillType[info.n_id] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}
	return true;
}

const SkillTypeCfg::SkillTypeInfo* SkillTypeCfg::GetSkillType(int id)
{
	if (m_mapSkillType.find(id) != m_mapSkillType.end())
	{
		return &(m_mapSkillType[id]);
	}
	return nullptr;
}

std::string SkillTypeCfg::GetHorseSkillIcon(int nSkillId)
{
	auto&& pCfg = GetSkillType(nSkillId);
	if (!pCfg)
	{
		return "";
	}
	return fmt::format("Texture2D'{0}/{1}.{2}'", m_strHorseIconPath, pCfg->s_icon, pCfg->s_icon);
}


std::string SkillTypeCfg::GetSkillIcon(int nSkillId)
{
	auto&& pCfg = GetSkillType(nSkillId);
	if (!pCfg)
	{
		return "";
	}
	return fmt::format("Texture2D'{0}/{1}.{2}'", m_strIconPath, pCfg->s_icon, pCfg->s_icon);
}




