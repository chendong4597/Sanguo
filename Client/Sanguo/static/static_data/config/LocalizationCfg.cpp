/*!
 * \file LocalizationCfg.cpp
 *
 * \author bingkunchen;
 * \date 2017/05/03
 *
 * 
 */
#include "stdafx.h"
#include "LocalizationCfg.h"
#include "StringUtil.h"
#include <XmlUtil.h>
#include <lua/LuaCallback.h>

LocalizationCfg& LocalizationCfg::getInstance()
{
	static LocalizationCfg s_LocalizationCfg;
	return s_LocalizationCfg;
}

bool LocalizationCfg::initialize()
{
	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/Localization.cfg")) return false;

	return parseLocalizationCfg(doc.RootElement());
}

void LocalizationCfg::terminate()
{
	m_languageAttrMap.clear();
}

const std::shared_ptr<LocalizationCfg::LanguageAttr> LocalizationCfg::getLanguageAttr(NS::tstring strTitle)
{
	LanguageAttrMap::iterator iter = m_languageAttrMap.find(strTitle);
	if (iter != m_languageAttrMap.end())
	{
		return iter->second;
	}
	return nullptr;
}

bool LocalizationCfg::parseLocalizationCfg(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	NS::tstring currLangName;

	if (!bzbee::XmlUtil::getAttribute(currLangName, pXmlRoot, "s_current_language")) { return false; }

	for (tinyxml2::XMLElement* xmlItem = pXmlRoot->FirstChildElement("language"); xmlItem != nullptr; xmlItem = xmlItem->NextSiblingElement("language"))
	{
		std::shared_ptr<LanguageAttr> language = std::make_shared<LanguageAttr>();

		if (!bzbee::XmlUtil::getAttribute(language->m_name, xmlItem, "s_name"))  { return false; }
		if (!bzbee::XmlUtil::getAttribute(language->m_moPath, xmlItem, "s_mo_path"))  { return false; }
		if (!bzbee::XmlUtil::getAttribute(language->m_translate, xmlItem, "s_tranlate")) { return false; }
		if (!bzbee::XmlUtil::getAttribute(language->m_icon, xmlItem, "icon")) { return false; }

		m_languageAttrMap.emplace(language->m_name, language);
	}

	LanguageAttrMap::iterator it = m_languageAttrMap.find(currLangName);
	if (it != m_languageAttrMap.end())
	{
		m_currentLanguage = it->second;
	}

	return true;
}

