/*!
* \file BuffGroupRuleCfg.cpp
*
* \author zhichenxia
* \date 2018/8/27
*
*
*/
#include "stdafx.h"
#include "BuffGroupRuleCfg.h"
#include "XmlUtil.h"
#include "StringUtil.h"

using namespace NS;

BuffGroupRuleCfg& BuffGroupRuleCfg::getInstance(void)
{
	static BuffGroupRuleCfg s;
	return s;
}

bool BuffGroupRuleCfg::initialize(void)
{
	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/BuffGroupRule.bin")) return false;

	return parseBuffGroupRule(doc.RootElement());

	return true;

    return true;
}

void BuffGroupRuleCfg::terminate(void)
{
	m_exclusionMap.clear();
	m_replaceMap.clear();
}

bool BuffGroupRuleCfg::checkExclusion(uint32 groupID, uint32 exclusionID) const
{
	auto range = m_exclusionMap.equal_range(groupID);
    for (auto it = range.first; it != range.second; ++it)
    {
        if (it->second == exclusionID) return true;
    }

	return false;
}

bool BuffGroupRuleCfg::checkReplace(uint32 groupID, uint32 replaceID) const
{
	auto range = m_replaceMap.equal_range(groupID);
	for (auto it = range.first; it != range.second; ++it)
		if (it->second == replaceID) return true;
	return false;
}

bool BuffGroupRuleCfg::parseBuffGroupRule(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{
        //auto& proto = dataList.buffgrouprules(i);
		
		uint32 n_GroupID{};// = proto.n_groupid();
		NS::XmlUtil::getAttribute(n_GroupID, xmlItem, "n_GroupID");

		uint32 n_ExclusionGroupID{};// = proto.n_exclusiongroupid();
		NS::XmlUtil::getAttribute(n_ExclusionGroupID, xmlItem, "n_ExclusionGroupID");
        if (n_ExclusionGroupID > 0u) m_exclusionMap.emplace(n_GroupID, n_ExclusionGroupID);

		uint32 n_ReplaceGroupID{};// = proto.n_replacegroupid();
		NS::XmlUtil::getAttribute(n_ReplaceGroupID, xmlItem, "n_ReplaceGroupID");
        if (n_ReplaceGroupID > 0u) m_replaceMap.emplace(n_GroupID, n_ReplaceGroupID);
		xmlItem = xmlItem->NextSiblingElement("Record");
    }

    return true;
}
