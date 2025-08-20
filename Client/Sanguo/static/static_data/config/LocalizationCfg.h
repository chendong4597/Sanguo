/*!
 * \file LocalizationCfg.h
 *
 * \author bingkunchen;
 * \date 2017/05/03
 *
 * 
 */

#pragma once
#include "BaseTypeDefine.h"
#include <memory>
#include <map>

class LocalizationCfg
{
public:
	struct LanguageAttr
	{
		NS::tstring m_name;
		NS::tstring m_moPath;
		NS::tstring m_translate;
		NS::tstring m_icon;
	};

	typedef std::map<NS::tstring, std::shared_ptr<LanguageAttr>> LanguageAttrMap;

public:
	static LocalizationCfg& getInstance();

	bool initialize();
	void terminate();

	const LanguageAttr* getCurrLanguage() { return m_currentLanguage.get(); }
	const LanguageAttrMap& getLanguageAttrMap() const{ return m_languageAttrMap; }
	const std::shared_ptr<LanguageAttr> getLanguageAttr(NS::tstring strTitle);

private:
	bool parseLocalizationCfg(tinyxml2::XMLElement* pXmlRoot);

private:
	LanguageAttrMap m_languageAttrMap{};
	std::shared_ptr<LanguageAttr> m_currentLanguage;
};
