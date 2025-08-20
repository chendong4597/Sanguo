/*!
 * \file AchieveCfg.h
 *
 * \author ChenBK;
 * \date 2017/10/10
 *
 * 
 */
#pragma once

#include <functional>
#include <memory>
#include <array>
#include <map>
#include <unordered_map>
#include "BaseTypeDefine.h"
#include "tinyxml2.h"

using namespace NS;


class HelpCfg
{
public:
	struct HelpInfo
	{
		//-------------------------------------------
		// data
		NS::int32 n_ID;
		std::string s_title;
		std::string s_desc;
		std::string s_icon;
	};

    typedef std::unordered_map<NS::uint, HelpInfo>			HelpItemMap;

public:
	static HelpCfg& getInstance();

	bool initialize();
	void terminate();
    void reloadMultiLanguage();

	const HelpInfo* getHelpItemInfo(int id);
private:
	bool parseHelpConfig(tinyxml2::XMLElement* pXmlRoot);

private:
	HelpItemMap	m_helpItemMap;
};
