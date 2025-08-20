/*!
* \file NpcBadgeCfg.cpp
* \date 08-30-2017 15:29:43
*
* \author LiQiangqiang
*/
#include "stdafx.h"
#include "NpcBadgeCfg.h"
#include "ItemCfg.h"
//#include "ConfigLoaderUtil.h"
#include <StringUtil.h>
#include "XmlUtil.h"
//#include <NpcBadge.pb.h>

NpcBadgeCfg& NpcBadgeCfg::getInstance()
{
	static NpcBadgeCfg s_NpcBadgeCfg;
	return s_NpcBadgeCfg;
}

bool NpcBadgeCfg::initialize()
{
	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/NpcBadge.bin")) return false;

	return parseNpcBadgeDefConfig(doc.RootElement());

    reloadMultiLanguage();
    return true;
}

void NpcBadgeCfg::terminate()
{
	m_npcBadgeDefMap.clear();
}

const NpcBadgeCfg::NpcBadgeDef* NpcBadgeCfg::findNpcBadgeDef(int id)
{
	auto it = m_npcBadgeDefMap.find(id);
	if (it == m_npcBadgeDefMap.end()) return nullptr;
	return &it->second;
}

const NpcBadgeCfg::NpcBadgeDef* NpcBadgeCfg::findNpcBadgeCfg(int badgeId, int level)
{
	if (level == 0) level = 1;

	for (auto& pr: m_npcBadgeDefMap)
	{
		auto& data = pr.second;
		if (data.n_NpcBaseID == badgeId && data.n_Level == level)
		{
			return &pr.second;
		}
	}

	return nullptr;
}

void NpcBadgeCfg::getGiftDescAllLevel(int nNpcBadgeId, std::vector<bzbee::tstring>& vecDescs)
{
	auto BaseNpcBadgeDef = NpcBadgeCfg::getInstance().findNpcBadgeDef(nNpcBadgeId);
	if (!BaseNpcBadgeDef)return;
	int nextId = BaseNpcBadgeDef->n_NpcBaseID;
	int level = BaseNpcBadgeDef->n_Level;

	while (level != 0)
	{
		auto npcBadgeDef = NpcBadgeCfg::getInstance().findNpcBadgeDef(nextId);
		if (!npcBadgeDef) break;
		bzbee::tstring sDesc = fmt::format(_LC("CLI_Npc-Availible Gift For LV{0}:"), npcBadgeDef->n_Level);
		for ( decltype(npcBadgeDef->m_availibleItemGifts.size()) i = 0; i < npcBadgeDef->m_availibleItemGifts.size(); i++)
		{
			auto item = ItemCfg::getInstance().getItemById(npcBadgeDef->m_availibleItemGifts[i]);
			if (item)
			{
				//sDesc = fmt::format("{0},{1}", sDesc,item->m_name);
			}
		}
		vecDescs.push_back(sDesc);
	}
}


void NpcBadgeCfg::getConfortAdditionDescAllLevel(int nNpcBadgeId, std::vector<bzbee::tstring>& vecDescs)
{
	auto BaseNpcBadgeDef = NpcBadgeCfg::getInstance().findNpcBadgeDef(nNpcBadgeId);
	if (!BaseNpcBadgeDef)return;
	int nextId = BaseNpcBadgeDef->n_NpcBaseID;
	int level = BaseNpcBadgeDef->n_Level;

	while (level != 0)
	{
		auto npcBadgeDef = NpcBadgeCfg::getInstance().findNpcBadgeCfg(nextId, level++);
		if (!npcBadgeDef) break;
		bzbee::tstring sDesc = fmt::format("LV{0} {1}", npcBadgeDef->n_Level, npcBadgeDef->s_ConfortAdditionDesc);
		vecDescs.push_back(sDesc);
	}
}

void NpcBadgeCfg::getLevelAdditiontDescAllLevel(int nNpcBadgeId, std::vector<bzbee::tstring>& vecDescs)
{
	auto BaseNpcBadgeDef = NpcBadgeCfg::getInstance().findNpcBadgeDef(nNpcBadgeId);
	if (!BaseNpcBadgeDef)return;
	int nextId = BaseNpcBadgeDef->n_NpcBaseID;
	int level = BaseNpcBadgeDef->n_Level;

	while (level != 0)
	{
		auto npcBadgeDef = NpcBadgeCfg::getInstance().findNpcBadgeCfg(nextId, level++);
		if (!npcBadgeDef) break;
		bzbee::tstring sDesc = fmt::format("LV{0}{1}", npcBadgeDef->n_Level, npcBadgeDef->s_functionDesc);
		vecDescs.push_back(sDesc);
	}
}

void NpcBadgeCfg::getBadgeIdsAllLevel(int nNpcBadgeId, std::vector<int>& vecIds)
{
	auto BaseNpcBadgeDef = NpcBadgeCfg::getInstance().findNpcBadgeDef(nNpcBadgeId);
	if (!BaseNpcBadgeDef)return;
	int nextId = BaseNpcBadgeDef->n_NpcBaseID;
	int level = BaseNpcBadgeDef->n_Level;

	while (level != 0)
	{
		auto npcBadgeDef = NpcBadgeCfg::getInstance().findNpcBadgeCfg(nextId,level);
		if (!npcBadgeDef) break;
		vecIds.push_back(nextId);
	}
}

bool NpcBadgeCfg::reloadMultiLanguage()
{
	for (auto& item : m_npcBadgeDefMap)
	{
		item.second.s_functionDesc = _LC(item.second.s_functionDesc_untrans);
		item.second.s_npcBadgeName = _LC(item.second.s_npcBadgeName_untrans);
	}

	return true;
}

bool NpcBadgeCfg::parseNpcBadgeDefConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		NpcBadgeCfg::NpcBadgeDef npcBadgeDef;

		NS::XmlUtil::getAttribute(npcBadgeDef.n_ID, xmlItem, "n_ID");// = proto.n_id();
		NS::XmlUtil::getAttribute(npcBadgeDef.n_NpcBaseID, xmlItem, "n_NpcBaseID");//  = proto.n_npcbaseid();
		NS::XmlUtil::getAttribute(npcBadgeDef.n_Level, xmlItem, "n_Level");//  = proto.n_level();
		NS::XmlUtil::getAttribute(npcBadgeDef.n_Mob, xmlItem, "n_Mob");
		NS::XmlUtil::getAttribute(npcBadgeDef.n_Speed, xmlItem, "n_Speed");
		NS::XmlUtil::getAttribute(npcBadgeDef.n_Prosperous, xmlItem, "n_Prosperous");
		NS::XmlUtil::getAttribute(npcBadgeDef.n_Rent, xmlItem, "n_Rent");
		NS::XmlUtil::getAttribute(npcBadgeDef.n_RatePlant, xmlItem, "n_RatePlant");
		NS::XmlUtil::getAttribute(npcBadgeDef.n_RateBuild, xmlItem, "n_RateBuild");
		NS::XmlUtil::getAttribute(npcBadgeDef.n_RateMine, xmlItem, "n_RateMine");
		NS::XmlUtil::getAttribute(npcBadgeDef.n_CraftID, xmlItem, "n_CraftID");
		NS::XmlUtil::getAttribute(npcBadgeDef.n_LimitWork, xmlItem, "n_LimitWork");
		NS::XmlUtil::getAttribute(npcBadgeDef.s_npcBadgeName_untrans, xmlItem, "s_npcBadgeName");//  = proto.s_npcbadgename();
		NS::XmlUtil::getAttribute(npcBadgeDef.s_functionDesc_untrans, xmlItem, "s_functionDesc");//  = proto.s_functiondesc();
		NS::XmlUtil::getAttribute(npcBadgeDef.s_ConfortAdditionDesc, xmlItem, "s_ConfortAdditionDesc");//  = proto.s_confortadditiondesc();
		NS::XmlUtil::getAttribute(npcBadgeDef.s_NpcBadgeAvataFrameResourceId, xmlItem, "s_NpcBadgeAvataResourceId");//  = proto.s_npcbadgeavataframeresourceid();
		NS::XmlUtil::getAttribute(npcBadgeDef.s_NpcBadgeAvataResourceId, xmlItem, "s_NpcBadgeAvataFrameResourceId");//  = proto.s_npcbadgeavataresourceid();

        {
			npcBadgeDef.m_availibleItemGifts.clear();
			std::string s_gift_list;
			NS::XmlUtil::getAttribute(s_gift_list, xmlItem, "s_gift_list");
			std::vector<NS::tstring> groupStrList;
			NS::StringUtil::splitString(groupStrList, s_gift_list, ",");

			for (decltype(groupStrList.size()) i = 0; i < groupStrList.size(); ++i)
			{
				NS::int32 rewardItemId{ 0 };
				NS::int32 count = sscanf(groupStrList[i].c_str(), "[%d]", &rewardItemId);
				if (count != 1) continue;

				npcBadgeDef.m_availibleItemGifts.push_back(rewardItemId);
			}
		}
		m_npcBadgeDefMap[npcBadgeDef.n_ID] = npcBadgeDef;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}



