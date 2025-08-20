/*!
 * \file AchieveCfg.cpp
 *
 * \author ChenBK;
 * \date 2017/10/10
 *
 * 
 */
#include "stdafx.h"
#include "AchieveCfg.h"
//#include "MapSettingCfg.h"
#include "../common/calculate.h"
#include "../data/AchieveData.h"
//#include "ConfigLoaderUtil.h"
//#include <Achieve/AchieveDefine.h>
#include <XmlUtil.h>
//#include <AchieveEvent.pb.h>
//#include <AchieveTitle.pb.h>
//#include <AchieveDetail.pb.h>
//#include <AchieveGroup.pb.h>
//#include <AchieveReward.pb.h>
//#include <AchieveAccumulateCfg.pb.h>

const float AchieveCfg::ANT_TIME_1_GET_BUTTON = 0.5f;
const float AchieveCfg::ANT_TIME_1_ICON_GLOW = 0.5f;
const float AchieveCfg::ANT_TIME_1_POINT_PARTICLE = 0.6f;
const float AchieveCfg::ANT_TIME_1_PROGRESS_GLOW = 0.5f;
const float AchieveCfg::ANT_TIME_1_STAR_SPINE = 0.5f;

#define ACCU_COMMON_PARAM 0

AchieveCfg& AchieveCfg::getInstance()
{
	static AchieveCfg s_AchieveCfg;
	return s_AchieveCfg;
}

bool AchieveCfg::initialize()
{
	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/AchieveEvent.bin")) return false;
	if (!parseEventConfig(doc.RootElement())) return false;

	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/AchieveTitle.bin")) return false;
	if (!parseTitleConfig(doc.RootElement())) return false;

	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/AchieveDetail.bin")) return false;
	if (!parseDetailConfig(doc.RootElement())) return false;

	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/AchieveGroup.bin")) return false;
	if (!parseGroupConfig(doc.RootElement())) return false;

	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/AchieveReward.bin")) return false;
	if (!parseRewardConfig(doc.RootElement())) return false;

	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/AchieveAccumulateCfg.bin")) return false;
	if (!parseAchieveAccumulateConfig(doc.RootElement())) return false;

	//if (!parseEventBinary()) return false;
	//if (!parseTitleBinary()) return false;
	//if (!parseDetailBinary()) return false;
	//if (!parseGroupBinary()) return false;
	//if (!parseRewardBinary()) return false;
	//if (!parseAchieveAccumulateBinary()) return false;

    //reloadMultiLanguage();

	return true;
}

void AchieveCfg::terminate()
{
	m_eventMap.clear();
	m_titleMap.clear();
	m_detailMap.clear();
	m_groupMap.clear();
	m_rewardMap.clear();
}

void AchieveCfg::reloadMultiLanguage()
{
    for (auto& pr : m_detailMap)
    {
        auto& info = pr.second;

        // 替换描述里的V[0-9]字段.
        info.m_desc = calcluate::getMedianValueName(info.m_desc_untrans, [&, info](int index)
        {
            return static_cast<int>(info.m_value[index]);
        });

        info.m_name = _LC(info.m_name_untrans);
    }
}

bool AchieveCfg::parseEventConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
        //auto& proto = dataList.achieveevents(i);
		EventInfo info;

		NS::XmlUtil::getAttribute(info.m_id, xmlItem, "n_ID");// = proto.n_id();
		NS::XmlUtil::getAttribute(info.m_name, xmlItem, "s_Name");// = proto.s_name();
		NS::XmlUtil::getAttribute(info.n_Type, xmlItem, "n_Type");// = proto.n_type();
		NS::XmlUtil::getAttribute(info.n_BeginDetailID, xmlItem, "n_BeginDetailID");//= proto.n_begindetailid();
		NS::XmlUtil::getAttribute(info.n_Param, xmlItem, "n_Param");//= proto.n_param();
		NS::XmlUtil::getAttribute(info.n_Distinguish, xmlItem, "n_JudgeType");// = proto.n_distinguish();
		NS::XmlUtil::getAttribute(info.n_JudgeType, xmlItem, "n_Distinguish");// = proto.n_judgetype();
		m_eventMap[info.m_id] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

bool AchieveCfg::parseTitleConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
        //auto& proto = dataList.achievetitles(i);
		TitleInfo info;// = m_titleMap[proto.n_id()];

		NS::XmlUtil::getAttribute(info.m_id, xmlItem, "n_ID");//  = proto.n_id();
		NS::XmlUtil::getAttribute(info.m_name, xmlItem, "s_Name");//  = proto.s_name();
		NS::XmlUtil::getAttribute(info.m_type, xmlItem, "n_Type");//  = proto.n_type();
		NS::XmlUtil::getAttribute(info.m_needPoints, xmlItem, "n_NeedPoints");//  = proto.n_needpoints();
		NS::XmlUtil::getAttribute(info.m_eventId, xmlItem, "n_EventID");//  = proto.n_eventid();
		NS::XmlUtil::getAttribute(info.m_groupId, xmlItem, "n_GroupID");//  = proto.n_groupid();
		NS::XmlUtil::getAttribute(info.m_achieveRewardId, xmlItem, "n_AchieveRewardID");//  = proto.n_achieverewardid();
		NS::XmlUtil::getAttribute(info.m_desc, xmlItem, "s_Desc");//  = proto.s_desc();

		m_titleMap[info.m_id] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

bool AchieveCfg::parseDetailConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto pAttrXml = p->FirstChildElement("Attribute");
	while (pAttrXml)
	{
		std::string strKey;
		//NS::XmlUtil::getAttribute(strKey, pAttrXml, "Name");
		//if (strKey == "s_BuffPath")
		//{
		//	NS::XmlUtil::getAttribute(m_strPath, pAttrXml, "Value");
		//}
		   // m_lobbyReminderInterval = dataList.n_lobbyreminderinterval();
   // m_newbieReminderLev = dataList.n_newbiereminderlev();
		pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	}

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
        //auto& proto = dataList.achievedetails(i);
		DetailInfo info;//m_detailMap[proto.n_id()];

		NS::XmlUtil::getAttribute(info.n_ID, xmlItem, "n_ID");// = proto.n_id();
		NS::XmlUtil::getAttribute(info.m_name, xmlItem, "s_Name");// = proto.s_name();
		NS::XmlUtil::getAttribute(info.m_icon, xmlItem, "s_icon");//  = proto.s_icon();
		NS::XmlUtil::getAttribute(info.m_difficulty, xmlItem, "n_Difficulty");// = proto.n_difficulty();
		NS::XmlUtil::getAttribute(info.m_level, xmlItem, "n_Level");// = proto.n_level();
		NS::XmlUtil::getAttribute(info.n_PreAchieve, xmlItem, "n_PreAchieve");// = proto.n_preachieve();
		NS::XmlUtil::getAttribute(info.n_ScriptFuncMap, xmlItem, "n_ScriptFuncMap");// = proto.n_scriptfuncmap();
		NS::XmlUtil::getAttribute(info.n_Point, xmlItem, "n_Point");//  = proto.n_point();
		NS::XmlUtil::getAttribute(info.n_Property, xmlItem, "n_Property");// = proto.n_property();
		NS::XmlUtil::getAttribute(info.n_EventID, xmlItem, "n_EventID");//  = proto.n_eventid();
		NS::XmlUtil::getAttribute(info.n_Condition, xmlItem, "n_Condition");//  = proto.n_condition();
		NS::XmlUtil::getAttribute(info.n_NextID, xmlItem, "n_NextID");// = proto.n_nextid();
		NS::XmlUtil::getAttribute(info.n_GroupID, xmlItem, "n_GroupID");// = proto.n_groupid();
		NS::XmlUtil::getAttribute(info.n_AchieveRewardID, xmlItem, "n_AchieveRewardID");// = proto.n_achieverewardid();
		NS::XmlUtil::getAttribute(info.m_desc, xmlItem, "s_Desc");//  = proto.s_desc();
		NS::XmlUtil::getAttribute(info.n_ClientTrigger, xmlItem, "n_ClientTrigger");//  = proto.n_clienttrigger();
		NS::XmlUtil::getAttribute(info.n_ClientReceiveReward, xmlItem, "n_ClientReceiveReward");// = proto.n_clientreceivereward();
		NS::XmlUtil::getAttribute(info.m_recommend, xmlItem, "n_Recommend");//  = proto.n_recommend();
		NS::XmlUtil::getAttribute(info.m_lobbyRecommend, xmlItem, "n_LobbyRecommend");//  = proto.n_lobbyrecommend();
		NS::XmlUtil::getAttribute(info.m_scriptFunction, xmlItem, "s_ScriptFunction");//  = proto.s_scriptfunction();
		NS::XmlUtil::getAttribute(info.n_AutoReward, xmlItem, "n_AutoReward");//  = proto.n_autoreward();

		NS::XmlUtil::getAttribute(info.m_value[0], xmlItem, "f_value_0");//  = proto.f_value_0();
		NS::XmlUtil::getAttribute(info.m_value[1], xmlItem, "f_value_1");// = proto.f_value_1();

		NS::XmlUtil::getAttribute(info.m_nNeedItem, xmlItem, "n_NeedItem");
		NS::XmlUtil::getAttribute(info.s_NeedTips, xmlItem, "s_NeedTips");

		{
			std::string strGuid;
			std::vector<tstring> vecString;
			NS::XmlUtil::getAttribute(strGuid, xmlItem, "s_Script");

			bzbee::StringUtil::splitString(vecString, strGuid, ";");
			for (auto strG : vecString)
			{
				std::vector<int> intString;
				bzbee::StringUtil::splitString(intString, strG, "|");
				if (intString.size() != 5)
				{
					continue;
				}
				GuidInfo ginfo;
				ginfo.nGuidTpy = intString[0];
				ginfo.nParam1 = intString[1];
				ginfo.nParam2 = intString[2];
				ginfo.nParam3 = intString[3];
				ginfo.nParam4 = intString[4];
				info.vecGuidInfo.push_back(ginfo);
			}
		}

		{
			std::string strGuid;
			std::vector<tstring> vecString;
			NS::XmlUtil::getAttribute(strGuid, xmlItem, "s_Script1");

			bzbee::StringUtil::splitString(vecString, strGuid, ";");
			for (auto strG : vecString)
			{
				std::vector<int> intString;
				bzbee::StringUtil::splitString(intString, strG, "|");
				if (intString.size() != 5)
				{
					continue;
				}
				GuidInfo ginfo;
				ginfo.nGuidTpy = intString[0];
				ginfo.nParam1 = intString[1];
				ginfo.nParam2 = intString[2];
				ginfo.nParam3 = intString[3];
				ginfo.nParam4 = intString[4];
				info.vecPreGuidInfo.push_back(ginfo);
			}
		}

        // total Achieve Set Member
		m_achieveSetNumMap[info.n_EventID]++;
		m_detailMap[info.n_ID] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}
	return true;
}

bool AchieveCfg::parseGroupConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto pAttrXml = p->FirstChildElement("Attribute");
	while (pAttrXml)
	{
		std::string strKey;
		//NS::XmlUtil::getAttribute(strKey, pAttrXml, "Name");
		//if (strKey == "s_BuffPath")
		//{
		//	NS::XmlUtil::getAttribute(m_strPath, pAttrXml, "Value");
		//}
		   // m_lobbyReminderInterval = dataList.n_lobbyreminderinterval();
   // m_newbieReminderLev = dataList.n_newbiereminderlev();
		pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	}

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
        //auto& proto = dataList.achievegroups(i);
		GroupInfo info;// = m_groupMap[proto.n_id()];

		NS::XmlUtil::getAttribute(info.m_id, xmlItem, "n_ID");// = proto.n_id();
		NS::XmlUtil::getAttribute(info.DetailIDs[0], xmlItem, "n_DetailID_0");// = proto.n_detailid_0();
		NS::XmlUtil::getAttribute(info.DetailIDs[1], xmlItem, "n_DetailID_1");//= proto.n_detailid_1();
		NS::XmlUtil::getAttribute(info.DetailIDs[2], xmlItem, "n_DetailID_2");// = proto.n_detailid_2();
		NS::XmlUtil::getAttribute(info.DetailIDs[3], xmlItem, "n_DetailID_3");// = proto.n_detailid_3();
		NS::XmlUtil::getAttribute(info.DetailIDs[4], xmlItem, "n_DetailID_4");// = proto.n_detailid_4();
		NS::XmlUtil::getAttribute(info.DetailIDs[5], xmlItem, "n_DetailID_5");// = proto.n_detailid_5();
		NS::XmlUtil::getAttribute(info.DetailIDs[6], xmlItem, "n_DetailID_6");//= proto.n_detailid_6();
		NS::XmlUtil::getAttribute(info.DetailIDs[7], xmlItem, "n_DetailID_7");// = proto.n_detailid_7();
		NS::XmlUtil::getAttribute(info.DetailIDs[8], xmlItem, "n_DetailID_8");// = proto.n_detailid_8();
		NS::XmlUtil::getAttribute(info.DetailIDs[9], xmlItem, "n_DetailID_9");// = proto.n_detailid_9();
		m_groupMap[info.m_id] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

bool AchieveCfg::parseRewardConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto pAttrXml = p->FirstChildElement("Attribute");
	while (pAttrXml)
	{
		std::string strKey;
		//NS::XmlUtil::getAttribute(strKey, pAttrXml, "Name");
		//if (strKey == "s_BuffPath")
		//{
		//	NS::XmlUtil::getAttribute(m_strPath, pAttrXml, "Value");
		//}
		   // m_lobbyReminderInterval = dataList.n_lobbyreminderinterval();
   // m_newbieReminderLev = dataList.n_newbiereminderlev();
		pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	}

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
        //auto& proto = dataList.achieverewards(i);
		RewardInfo info;// = m_rewardMap[proto.n_id()];

		NS::XmlUtil::getAttribute(info.m_id, xmlItem, "n_ID");// = proto.n_id();

		NS::XmlUtil::getAttribute(info.m_rewardType[0], xmlItem, "n_reward_type_1");//= proto.n_reward_type_1();
		NS::XmlUtil::getAttribute(info.m_rewardParam1[0], xmlItem, "n_reward_param1_1");// = proto.n_reward_param1_1();
		NS::XmlUtil::getAttribute(info.m_rewardParam2[0], xmlItem, "n_reward_param2_1");// = proto.n_reward_param2_1();
		NS::XmlUtil::getAttribute(info.m_effectType[0], xmlItem, "n_effect_type_1");// = proto.n_effect_type_1();
		NS::XmlUtil::getAttribute(info.m_effectAttrref1[0], xmlItem, "n_effect_attrref1_1");// = proto.n_effect_attrref1_1();
		NS::XmlUtil::getAttribute(info.m_effectAttrref2[0], xmlItem, "n_effect_attrref2_1");// = proto.n_effect_attrref2_1();
		NS::XmlUtil::getAttribute(info.m_effectValue1[0], xmlItem, "n_effect_value1_1");// = proto.n_effect_value1_1();
		NS::XmlUtil::getAttribute(info.m_effectValue2[0], xmlItem, "n_effect_value2_1");// = proto.n_effect_value2_1();

		NS::XmlUtil::getAttribute(info.m_rewardType[1], xmlItem, "n_reward_type_2");//= proto.n_reward_type_1();
		NS::XmlUtil::getAttribute(info.m_rewardParam1[1], xmlItem, "n_reward_param1_2");// = proto.n_reward_param1_1();
		NS::XmlUtil::getAttribute(info.m_rewardParam2[1], xmlItem, "n_reward_param2_2");// = proto.n_reward_param2_1();
		NS::XmlUtil::getAttribute(info.m_effectType[1], xmlItem, "n_effect_type_2");// = proto.n_effect_type_1();
		NS::XmlUtil::getAttribute(info.m_effectAttrref1[1], xmlItem, "n_effect_attrref1_2");// = proto.n_effect_attrref1_1();
		NS::XmlUtil::getAttribute(info.m_effectAttrref2[1], xmlItem, "n_effect_attrref2_2");// = proto.n_effect_attrref2_1();
		NS::XmlUtil::getAttribute(info.m_effectValue1[1], xmlItem, "n_effect_value1_2");// = proto.n_effect_value1_1();
		NS::XmlUtil::getAttribute(info.m_effectValue2[1], xmlItem, "n_effect_value2_2");// = proto.n_effect_value2_1();

		NS::XmlUtil::getAttribute(info.m_rewardType[2], xmlItem, "n_reward_type_3");//= proto.n_reward_type_1();
		NS::XmlUtil::getAttribute(info.m_rewardParam1[2], xmlItem, "n_reward_param1_3");// = proto.n_reward_param1_1();
		NS::XmlUtil::getAttribute(info.m_rewardParam2[2], xmlItem, "n_reward_param2_3");// = proto.n_reward_param2_1();
		NS::XmlUtil::getAttribute(info.m_effectType[2], xmlItem, "n_effect_type_3");// = proto.n_effect_type_1();
		NS::XmlUtil::getAttribute(info.m_effectAttrref1[2], xmlItem, "n_effect_attrref1_3");// = proto.n_effect_attrref1_1();
		NS::XmlUtil::getAttribute(info.m_effectAttrref2[2], xmlItem, "n_effect_attrref2_3");// = proto.n_effect_attrref2_1();
		NS::XmlUtil::getAttribute(info.m_effectValue1[2], xmlItem, "n_effect_value1_3");// = proto.n_effect_value1_1();
		NS::XmlUtil::getAttribute(info.m_effectValue2[2], xmlItem, "n_effect_value2_3");// = proto.n_effect_value2_1();
		m_rewardMap[info.m_id] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

const AchieveCfg::EventInfo* AchieveCfg::findEventInfo(int id)
{
	auto it = m_eventMap.find(id);
	return (it == m_eventMap.end()) ? nullptr : &it->second;
}

const AchieveCfg::TitleInfo* AchieveCfg::findTitleInfo(int id)
{
	auto it = m_titleMap.find(id);
	return (it == m_titleMap.end()) ? nullptr : &it->second;
}

const AchieveCfg::DetailInfo* AchieveCfg::findDetailInfo(NS::I64 detailId)
{
	auto it = m_detailMap.find(detailId);
	return (it == m_detailMap.end()) ? nullptr : &it->second;
}

const AchieveCfg::GroupInfo* AchieveCfg::findGroupInfo(int id)
{
	auto it = m_groupMap.find(id);
	return (it == m_groupMap.end()) ? nullptr : &it->second;
}

const AchieveCfg::GroupInfo* AchieveCfg::findGroupInfoByDetailId(NS::I64 detailId)
{
	auto it = m_detailMap.find(detailId);
	if (it != m_detailMap.end())
	{
		auto git = m_groupMap.find(it->second.n_GroupID);
		if (git != m_groupMap.end())
		{
			return &git->second;
		}
	}

	return nullptr;
}

const AchieveCfg::RewardInfo* AchieveCfg::findRewardInfo(int id)
{
	auto it = m_rewardMap.find(id);
	return (it == m_rewardMap.end()) ? nullptr : &it->second;
}

const AchieveCfg::RewardInfo* AchieveCfg::findRewardInfoByDetailId(NS::I64 detailId)
{
	auto it = m_detailMap.find(detailId);
	if (it != m_detailMap.end())
	{
		auto rit = m_rewardMap.find(it->second.n_AchieveRewardID);
		if (rit != m_rewardMap.end())
		{
			return &rit->second;
		}
	}

	return nullptr;
}

const AchieveCfg::EventInfo* AchieveCfg::findEventInfoByDetailId(NS::I64 detailId)
{
	auto it = m_detailMap.find(detailId);
	if (it != m_detailMap.end())
	{
		auto rit = m_eventMap.find(it->second.n_EventID);
		if (rit != m_eventMap.end())
		{
			return &rit->second;
		}
	}

	return nullptr;
}

NS::I64 AchieveCfg::getBeginDetailIdByEventId(int id)
{
	auto&& eventInfo = findEventInfo(id);
	return (eventInfo) ? eventInfo->n_BeginDetailID : 0;
}

const NS::tstring AchieveCfg::findTitleInfoByPoints(int point)
{
	int lastPoint = 0;
	NS::tstring retStr = NS::EMPTY_STRING;
	for (const auto& pr : m_titleMap)
	{
		if (lastPoint < pr.second.m_needPoints && pr.second.m_needPoints < point)
		{
			retStr = pr.second.m_name;
		}
		lastPoint = pr.second.m_needPoints;
	}

	return retStr;
}

const NS::tstring AchieveCfg::findAchieveRewardTitle(NS::I64 detailId, int points)
{
	const DetailInfo* detailInfo = findDetailInfo(detailId);
	if (!detailInfo) return NS::EMPTY_STRING;
	const EventInfo* eventInfo = findEventInfo(detailInfo->n_EventID);
	if (!eventInfo) return NS::EMPTY_STRING;

	for (auto& it : m_titleMap)
	{
		TitleInfo* info = &it.second;
		if (!info) assert(false);

		if (info->m_type == eventInfo->n_Type
			&& info->m_eventId == detailInfo->n_EventID
			&& info->m_groupId == detailInfo->n_GroupID
			&& info->m_achieveRewardId == detailInfo->n_AchieveRewardID
			&& info->m_needPoints <= points)
		{
			return info->m_name;
		}
	}

	return NS::EMPTY_STRING;
}

int AchieveCfg::getAchieveTotalStageNum(NS::I64 detailId)
{
	const DetailInfo* detailInfo = findDetailInfo(detailId);
	if (!detailInfo) return 0;

	AchieveSetNumMap::iterator it = m_achieveSetNumMap.find(detailInfo->n_EventID);

	return (it != m_achieveSetNumMap.end()) ? it->second : 0;
}

int AchieveCfg::getAchieveCurrStage(NS::I64 detailId)
{
	return detailId % 1000; // detailId = 10001003 = XXXX + 阶段号
}


int AchieveCfg::getAchieveTotalSubNum(NS::I64 detailId)
{
	const AchieveCfg::GroupInfo*  groupInfo = findGroupInfoByDetailId(detailId);
	if (!groupInfo) return 0;

	int count = 0;
	for (auto subId : groupInfo->DetailIDs)
	{
		if (subId > 0) ++count;
	}

	return count;
}

int AchieveCfg::getAchieveStagePercent(NS::I64 detailId)
{
	int totalStage = getAchieveTotalStageNum(detailId);
	int currStage = getAchieveCurrStage(detailId);

	return (totalStage > 0) ? currStage *100 / totalStage : 0;
}

int AchieveCfg::getAchieveStar(NS::I64 detailId)
{
	const DetailInfo* pInfo = findDetailInfo(detailId);
	if (!pInfo) return 0;

	return pInfo->m_difficulty;
}

NS::I64 AchieveCfg::getAchievePreviousId(NS::I64 currDetailId)
{
	if (!checkedHasAchieveStage(currDetailId)) return 0;

	NS::I64 previousId = --currDetailId;
	return (findDetailInfo(previousId) != nullptr) ? previousId : 0;
}

NS::I64 AchieveCfg::getAchieveNextId(NS::I64 currDetailId)
{
	if (!checkedHasAchieveStage(currDetailId)) return 0;

	NS::I64 nextId = ++currDetailId;
	return (findDetailInfo(nextId) != nullptr) ? nextId : 0;
}

const NS::tstring& AchieveCfg::getScriptFunction(NS::I64 detailId)
{
	auto&& info = findDetailInfo(detailId);
	return info ? info->m_scriptFunction : NS::EMPTY_STRING;
}

bool AchieveCfg::checkedHasAchieveStage(NS::I64 detailId)
{
	const DetailInfo* detailInfo = findDetailInfo(detailId);
	if (!detailInfo) return false;

	return m_achieveSetNumMap[detailInfo->n_EventID] > 1;
}

bool AchieveCfg::checkedHasGroup(NS::I64 detailId)
{
	const DetailInfo* detailInfo = findDetailInfo(detailId);
	if (!detailInfo) return false;

	return (detailInfo->n_GroupID > 0) ? true : false;
}

bool AchieveCfg::checkedIsTotalAchieve(NS::I64 detailId)
{
	auto it = m_detailMap.find(detailId);
	if (it != m_detailMap.end())
	{
		return (it->second.n_EventID == ACHIEVE_EVENT_TEMPLATE_ID_10000);
	}

	return false;
}

bool AchieveCfg::checkedIsEndAchieve(NS::I64 detailId)
{
	const AchieveCfg::DetailInfo* detailInfo = AchieveCfg::getInstance().findDetailInfo(detailId);
	if (!detailInfo) return false;

	return detailInfo->n_NextID <= 0;
}

bool AchieveCfg::checkedIsLobbyRecommend(NS::I64 detailId)
{
	auto&& info = findDetailInfo(detailId);
	
	return (info == nullptr) ? false : info->m_lobbyRecommend != 0;
}

//////////////////////////////////////////////////////////////////////////
void AchieveCfg::forEachAchieveEvent(const std::function<void(const EventInfo& data)>& fun)
{
	if (!fun) return;

	for (const auto iter : m_eventMap)
	{
		fun(iter.second);
	}
}

//////////////////////////////////////////////////////////////////////////
bool AchieveCfg::parseAchieveAccumulateConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto pAttrXml = p->FirstChildElement("Attribute");
	while (pAttrXml)
	{
		std::string strKey;
		//NS::XmlUtil::getAttribute(strKey, pAttrXml, "Name");
		//if (strKey == "s_BuffPath")
		//{
		//	NS::XmlUtil::getAttribute(m_strPath, pAttrXml, "Value");
		//}
		   // m_lobbyReminderInterval = dataList.n_lobbyreminderinterval();
   // m_newbieReminderLev = dataList.n_newbiereminderlev();
		pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	}

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		int n_Type{};
		NS::XmlUtil::getAttribute(n_Type, xmlItem, "n_Type");

		ACCU_ACHIEVE_TYPE eType = (ACCU_ACHIEVE_TYPE)n_Type;

        std::vector<NS::int32> vecParam;
		std::string s_param;
		NS::XmlUtil::getAttribute(s_param, xmlItem, "s_Param");// = proto.n_id();
        NS::StringUtil::splitString(vecParam, s_param, ",");

        std::vector<NS::int32> vecEvent;
		std::string s_achieveeventid;
		NS::XmlUtil::getAttribute(s_achieveeventid, xmlItem, "s_AchieveEventID");
        NS::StringUtil::splitString(vecEvent, s_achieveeventid, ",");

		AccuTypeDataMap mapData;// = m_mapAccu[eType];
		for (auto nParam : vecParam)
		{
			auto& setData = mapData[nParam];
			for (auto nEventID : vecEvent)
			{
				setData.insert(nEventID);
			}
		}
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
void AchieveCfg::getAccuAchieveEventSet(ACCU_ACHIEVE_TYPE eType, NS::int32 nParam, std::set<NS::int32>& setEvent)
{
	setEvent.clear();
	const auto iterMapData = m_mapAccu.find(eType);
	if (iterMapData == m_mapAccu.cend())
		return;

	//if (ACCU_PLAYER_GO_DEEP != eType)
	//{
	//	const auto iterData = iterMapData->second.find(nParam);
	//	if (iterData != iterMapData->second.cend())
	//	{
	//		setEvent = iterData->second;
	//	}
	//}
	//else
	//{
	//	auto nMapID = MapSettingCfg::getInstance().getCurSceneId();
	//	const auto iterData = iterMapData->second.find(nMapID);
	//	if (iterData != iterMapData->second.cend())
	//	{
	//		for (auto nEventID : iterData->second)
	//		{
	//			auto pCfg = findEventInfo(nEventID);
	//			if (pCfg == nullptr || pCfg->n_Param > nParam)
	//				continue;
	//			auto pData = AchieveData::getInstance().findAchieveDataById(pCfg->n_BeginDetailID);
	//			if (pData && pData->status() != MsgPB::ACHIEVE_STATUS_IN_PROGRESS)
	//				continue;
	//			setEvent.insert(nEventID);
	//		}
	//	}
	//}

	//const auto iterComData = iterMapData->second.find(ACCU_COMMON_PARAM);
	//if (iterComData != iterMapData->second.cend())
	//{
	//	setEvent.insert(iterComData->second.begin(), iterComData->second.end());
	//}
}
