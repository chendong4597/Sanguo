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


class BehaviorTreeTypeCfg
{
	struct BehaviorTreeTypeCfgInfo
	{
		int n_id{};
		int n_Type{};
		std::string s_path;
	};

	typedef std::map<int, BehaviorTreeTypeCfgInfo> MAP_BEHAVIOR;

public:

public:

	BehaviorTreeTypeCfg() {}
	static BehaviorTreeTypeCfg& getInstance();

	bool initializeCfg();
	void terminate();
	bool parseConfig(tinyxml2::XMLElement* pXmlRoot);

	const BehaviorTreeTypeCfgInfo* GetBehaviorType(int id);

private:
	MAP_BEHAVIOR m_mapTreeType;
	
};
