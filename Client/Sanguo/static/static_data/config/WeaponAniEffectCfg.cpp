/*!
 * \file GlobalCfg.cpp
 *
 * \author cd
 * \date 2020/04/07
 *
 *
 */
#include "stdafx.h"
#include "WeaponAniEffectCfg.h"
#include "XmlUtil.h"
#include "StringUtil.h"




WeaponAniEffectCfg& WeaponAniEffectCfg::getInstance()
{
	static WeaponAniEffectCfg s_WeaponAniEffectCfg;
	return s_WeaponAniEffectCfg;
}

bool WeaponAniEffectCfg::initializeCfg()
{

	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/WeaponAniEffect.bin")) return false;

	return parseConfig(doc.RootElement());
}

void WeaponAniEffectCfg::terminate()
{
	m_mapWeaponAniEffect.clear();
}

bool WeaponAniEffectCfg::parseConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{
		WeaponAniEffectInfo info;
		NS::XmlUtil::getAttribute(info.n_ID, xmlItem , "n_ID");
		NS::XmlUtil::getAttribute(info.n_WeaponTpy, xmlItem, "n_WeaponTpy");
		NS::XmlUtil::getAttribute(info.n_ComboIdx, xmlItem , "n_ComboIdx");
		NS::XmlUtil::getAttribute(info.s_SockName, xmlItem, "s_SockName");
		NS::XmlUtil::getAttribute(info.s_AssertPath, xmlItem, "s_AssertPath");
		NS::XmlUtil::getAttribute(info.f_DisplayTm, xmlItem, "f_DisplayTm");
		NS::XmlUtil::getAttribute(info.n_AttachTpy, xmlItem, "n_AttachTpy");
		NS::XmlUtil::getAttribute(info.s_HitAssertPath, xmlItem, "s_HitAssertPath");
		m_mapWeaponAniEffect[info.n_ID] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}
	return true;
}

const WeaponAniEffectCfg::WeaponAniEffectInfo* WeaponAniEffectCfg::GetWeaponAniEffect(int weaponTpy, int comboIdx)
{
	MAP_WEAPONANIEFFECT::iterator iter = m_mapWeaponAniEffect.begin();
	for ( ; iter != m_mapWeaponAniEffect.end() ; iter++)
	{
		if (iter->second.n_WeaponTpy == weaponTpy && iter->second.n_ComboIdx == comboIdx)
		{
			return &(iter->second);
		}
	}
	return nullptr;
}


