/*!
* \file BuffGroupRuleCfg.h
*
* \author zhichenxia
* \date 2018/8/27
*
*
*/
#pragma once
#include <unordered_map>
#include "tinyxml2.h"

class BuffGroupRuleCfg final
{
	BuffGroupRuleCfg(void) = default;
    BuffGroupRuleCfg(const BuffGroupRuleCfg&) = delete;
    BuffGroupRuleCfg& operator= (const BuffGroupRuleCfg&) = delete;

public:
	static BuffGroupRuleCfg& getInstance(void);

	bool initialize(void);
	void terminate (void);

public:
	bool checkExclusion(NS::uint32 groupID, NS::uint32 exclusionID) const;
	bool checkReplace(NS::uint32 groupID, NS::uint32 replaceID) const;

private:
    bool parseBuffGroupRule(tinyxml2::XMLElement* pXmlRoot);

private:
	std::unordered_multimap<NS::uint32, NS::uint32> m_exclusionMap;
	std::unordered_multimap<NS::uint32, NS::uint32> m_replaceMap;

};
