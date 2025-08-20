/*!
 * \file Localization.cpp
 * \date 3-18-2013 14:50:04
 * 
 * 
 * \author zjhlogo (zjhlogo@gmail.com)
 */
#include "Localization.h"
#include "../FileUtil.h"
#include "../LogUtil.h"

Localization::Localization()
{

}

Localization::~Localization()
{

}

Localization& Localization::getInstance()
{
	static Localization s_Localization;
	return s_Localization;
}

bool Localization::initialize(const bzbee::tstring& strFile)
{
	LOGI("Localization::initialize ");
	return m_Translator.initialize(strFile);
}

void Localization::terminate()
{

}

