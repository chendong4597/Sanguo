/*!
 * \file GlobalCfg.h
 *
 * \author cd
 * \date 2020/04/07
 *
 *
 */
#pragma once
#include <map>
#include <memory>
#include <unordered_map>
#include "tinyxml2.h"


class TroopsCfg
{
public:
	struct TroopsInfo
	{
		int n_ID{};
		std::string s_Name;
		std::string s_icon;
		int n_cop{};
		std::vector<int> vecPos;
		int n_needLv{};
		int n_max{};
		int n_mainPos{};
		int n_buffID{};
	};


	typedef std::map<int, TroopsInfo> MAP_TROOPINFO;

public:

	TroopsCfg() {}
	static TroopsCfg& getInstance();

	bool initializeCfg();
	void terminate();
	bool parseConfig(tinyxml2::XMLElement* pXmlRoot);

	const TroopsInfo* GetTroopsType(int id);
	const MAP_TROOPINFO& GetTroopsMap() const { return m_mapTroopsType; }
private:
	MAP_TROOPINFO m_mapTroopsType;
};
