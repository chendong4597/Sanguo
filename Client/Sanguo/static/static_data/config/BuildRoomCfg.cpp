/*!
* \file NpcBadgeCfg.cpp
* \date 08-30-2017 15:29:43
*
* \author LiQiangqiang
*/
#include "stdafx.h"
#include "BuildRoomCfg.h"
#include "StringUtil.h"
#include "format.h"
#include "ItemCfg.h"
#include "StringUtil.h"
#include "../data/NpcProtectorData.h"
#include <XmlUtil.h>

using namespace NS;


BuildRoomCfg& BuildRoomCfg::getInstance()
{
	static BuildRoomCfg s_buildRoomCfg;
	return s_buildRoomCfg;
}

bool BuildRoomCfg::initialize()
{
	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/BuildRoomCfg.bin")) return false;

	return true;
}

void BuildRoomCfg::terminate()
{
	m_builRoomDefMap.clear();
}

const BuildRoomCfg::BuilRoomDef* BuildRoomCfg::findBuilRoomDef(int id)
{
	auto it = m_builRoomDefMap.find(id);
	if (it == m_builRoomDefMap.end()) return nullptr;
	return &it->second;
}

bool BuildRoomCfg::isBadge(int badgeId)
{
	return findBuilRoomDef(badgeId) != nullptr ? true : false;
}

int BuildRoomCfg::getNpcId(int badgeId)
{
	auto cfg = findBuilRoomDef(badgeId);
	if (!cfg) return -1;
	return cfg->n_npcId;
}

bool BuildRoomCfg::parseBuilRoomConfig(tinyxml2::XMLElement* pXmlRoot)
{
	//config::BuildRoomCfgSet dataList;
	//if (!loadProtoConfig(dataList, "config/BuildRoomCfg.bin")) return false;

	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto pAttrXml = p->FirstChildElement("Attribute");
	while (pAttrXml)
	{
		std::string strKey;
		NS::XmlUtil::getAttribute(strKey, pAttrXml, "Name");
		if (strKey == "n_room_delete_time")
		{
			NS::XmlUtil::getAttribute(m_delTime, pAttrXml, "Value");
		}
		else if (strKey == "n_RefreshRentalTime")
		{
			NS::XmlUtil::getAttribute(m_RefreshRentalTime, pAttrXml, "Value");
		}
		else if (strKey == "n_Reborn_Time")
		{
			NS::XmlUtil::getAttribute(m_npcRebornTime, pAttrXml, "Value");
		}
		else if (strKey == "f_NpcRebornCost_a")
		{
			NS::XmlUtil::getAttribute(m_paramA, pAttrXml, "Value");
		}
		else if (strKey == "f_NpcRebornCost_b")
		{
			NS::XmlUtil::getAttribute(m_paramB, pAttrXml, "Value");
		}
		else if (strKey == "f_NpcRebornCost_c")
		{
			NS::XmlUtil::getAttribute(m_paramC, pAttrXml, "Value");
		}
		pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	}
	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		BuilRoomDef info;

		NS::XmlUtil::getAttribute(info.n_ID, xmlItem, "n_ID");// = proto.n_id();
		NS::XmlUtil::getAttribute(info.s_name, xmlItem, "s_name");// = proto.s_name();
		NS::XmlUtil::getAttribute(info.s_test_note, xmlItem, "s_note");// = proto.s_note();
		NS::XmlUtil::getAttribute(info.n_min_grid, xmlItem, "n_min_grid");// = proto.n_min_grid();
		NS::XmlUtil::getAttribute(info.n_max_grid, xmlItem, "n_max_grid");// = proto.n_max_grid();
		NS::XmlUtil::getAttribute(info.n_back_ground, xmlItem, "n_back_ground");// = proto.n_back_ground();
		NS::XmlUtil::getAttribute(info.n_furnitures[0], xmlItem, "n_build_type_1");// = proto.n_build_type_1(); 
		NS::XmlUtil::getAttribute(info.n_furnitures[1], xmlItem, "n_build_type_2");// = proto.n_build_type_2(); 
		NS::XmlUtil::getAttribute(info.n_furnitures[2], xmlItem, "n_build_type_3");// = proto.n_build_type_3(); 
		NS::XmlUtil::getAttribute(info.n_furnitures[3], xmlItem, "n_build_type_4");// = proto.n_build_type_4(); 
		NS::XmlUtil::getAttribute(info.n_furnitures[4], xmlItem, "n_build_type_5");// = proto.n_build_type_5();
		NS::XmlUtil::getAttribute(info.n_comfortable_oper, xmlItem, "n_comfortable_oper");// = proto.n_comfortable_oper();
		NS::XmlUtil::getAttribute(info.n_comfortable_need, xmlItem, "n_comfortable_need");// = proto.n_comfortable_need();
		NS::XmlUtil::getAttribute(info.n_height_oper, xmlItem, "n_height_oper");// = proto.n_height_oper();
		NS::XmlUtil::getAttribute(info.n_height_need, xmlItem, "n_height_need");// = proto.n_height_need();
		NS::XmlUtil::getAttribute(info.n_resurrection_cost, xmlItem, "n_resurrection_cost");// = proto.n_resurrection_cost();
		NS::XmlUtil::getAttribute(info.n_npcId, xmlItem, "n_npcid");// = proto.n_npcid();
		NS::XmlUtil::getAttribute(info.f_watiTimeSec, xmlItem, "n_npc_wait_sec");// = (float)proto.n_npc_wait_sec();
		NS::XmlUtil::getAttribute(info.n_equipSubType, xmlItem, "n_Sub_EquipType");// = proto.n_sub_equiptype();
		NS::XmlUtil::getAttribute(info.n_Check_Priority, xmlItem, "n_Check_Priority");// = proto.n_check_priority();
		NS::XmlUtil::getAttribute(info.s_Npc_Video, xmlItem, "s_Npc_Video");// = proto.s_npc_video();
		NS::XmlUtil::getAttribute(info.n_replace_Npc, xmlItem, "n_Replace_Npc");// = proto.n_replace_npc();
		NS::XmlUtil::getAttribute(info.s_Sub_EquipType_Icon, xmlItem, "s_Sub_EquipType_Icon");// = proto.s_sub_equiptype_icon();

		xmlItem = xmlItem->NextSiblingElement("Record");

		m_builRoomDefMap[info.n_ID] = info;
	}

	return true;
}

int BuildRoomCfg::getBadgeId(int npcId)
{
	for (const auto& pr: m_builRoomDefMap)
	{
		auto data = pr.second;
		if (data.n_npcId == npcId)
		{
			return data.n_ID;
		}
	}
	return -1;
}

int BuildRoomCfg::getRebornCastMoney(int leftTime, int badgeLevel)
{
	if (leftTime <= 0 || badgeLevel == 0) return 0;

	int32 nNeedCash = int32(std::pow(m_paramA * leftTime/60 , m_paramB)*std::pow(badgeLevel , m_paramC) + 0.5f);
	nNeedCash = std::max(1, nNeedCash);
	return nNeedCash;
}

int BuildRoomCfg::getReplaceNpcId(int npcId)
{
	for (const auto& pr : m_builRoomDefMap)
	{
		auto data = pr.second;
		if (data.n_npcId == npcId)
		{
			return data.n_replace_Npc;
		}
	}
	return -1;
}

