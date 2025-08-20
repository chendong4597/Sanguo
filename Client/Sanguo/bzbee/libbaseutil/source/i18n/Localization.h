/*!
 * \file Localization.h
 * \date 3-18-2013 14:49:59
 * 
 * 
 * \author zjhlogo (zjhlogo@gmail.com)
 */
#pragma once
#include "../BaseType.h"
#include "ITranslator.h"
#include <map>

class Localization
{
public:

	Localization();
	virtual ~Localization();

	static Localization& getInstance();

	bool initialize(const bzbee::tstring& strFile);
	void terminate();

	inline const NS::tstring translate(const NS::tstring& key) const
	{
		return m_Translator.translateText(key);
	}
	const NS::Translator* getTranslatorForUiEditor() {
		if (nullptr == m_pTranslatorForUiEditor)
		{
			m_pTranslatorForUiEditor = &m_Translator;
		}
		return m_pTranslatorForUiEditor;
	}
	void setTranslatorForUiEditor(const bzbee::Translator* val) { m_pTranslatorForUiEditor = val; }
private:
	NS::Translator m_Translator;
	const NS::Translator* m_pTranslatorForUiEditor{ nullptr };
};
