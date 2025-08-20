/*!
 * \file SurvivalEventCfg.cpp
 *
 * \author ChenBK;
 * \date 2017/08/30
 *
 * 
 */
#include "stdafx.h"
#include "SurvivalEventCfg.h"
#include <StringUtil.h>
#include <FileUtil.h>
#include <XmlUtil.h>

using namespace NS;

#define CalcDefenseDamageKey(templateid, lev) (templateid * 100 + lev)

SurvivalEventCfg& SurvivalEventCfg::getInstance()
{
	static SurvivalEventCfg s_survivalEventCfg;
	return s_survivalEventCfg;
}

bool SurvivalEventCfg::initialize()
{
	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/SurvivalEvent.bin")) return false;
	if (!parseSurvivalEventConfig(doc.RootElement())) return false;

	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/SurvivalEventMobPool.bin")) return false;
	if (!parseSurvivalEventMobPoolConfig(doc.RootElement())) return false;

	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/SurvivalEventMobList.bin")) return false;
	if (!parseSurvivalEventMobListConfig(doc.RootElement())) return false;

	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/SurvivalEventRateup.bin")) return false;
	if (!parseSurvivalEventRateupConfig(doc.RootElement())) return false;

	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/SurvivalEventMobReplace.bin")) return false;
	if (!parseSurvivalEventMobReplaceConfig(doc.RootElement())) return false;

	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/SurvivalEventDefenseDamage.bin")) return false;
	if (!parseSurvivalEventDefenseDamageConfig(doc.RootElement())) return false;

	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/SurvivalEventMobDamage.bin")) return false;
	if (!parseSurvivalEventMobDamageConfig(doc.RootElement())) return false;

    reloadMultiLanguage();

	return true;
}

void SurvivalEventCfg::terminate()
{
	m_eventMap.clear();
	m_eventMobPoolMap.clear();
	m_eventMobListMap.clear();
	m_eventGuaranteeRewardsMap.clear();
	m_eventNpcMap.clear();
	m_eventRateupMap.clear();
	m_eventMobReplaceMap.clear();
	m_mapDefenseDamage.clear();
	m_mapMobDamage.clear();
}

bool SurvivalEventCfg::parseSurvivalEventConfig(tinyxml2::XMLElement* pXmlRoot)
{


	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto pAttrXml = p->FirstChildElement("Attribute");
	while (pAttrXml)
	{
		std::string strKey;
		NS::XmlUtil::getAttribute(strKey, pAttrXml, "Name");
		if (strKey == "n_ExitTime")
		{
			NS::XmlUtil::getAttribute(m_eventAttr.m_exitTime, pAttrXml, "Value");
		}
		else if (strKey == "n_ReadyTime")
		{
			NS::XmlUtil::getAttribute(m_eventAttr.m_readyTime, pAttrXml, "Value");
		}
		else if (strKey == "n_RewardTime")
		{
			NS::XmlUtil::getAttribute(m_eventAttr.m_rewardTime, pAttrXml, "Value");
		}
		else if (strKey == "n_TrigCountLimit")
		{
			NS::XmlUtil::getAttribute(m_eventAttr.m_nTrigCountLimit, pAttrXml, "Value");
		}
		else if (strKey == "n_ExeuntBuff")
		{
			NS::XmlUtil::getAttribute(m_eventAttr.m_nExeuntBuff, pAttrXml, "Value");
		}
		pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	}

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		EventInfo info;

		NS::XmlUtil::getAttribute(info.m_id, xmlItem, "n_ID");// = proto.n_id();
        NS::XmlUtil::getAttribute(info.m_name_untrans, xmlItem, "s_Name");// = proto.s_name();
		NS::XmlUtil::getAttribute(info.m_useItem, xmlItem, "n_UseItem");// = proto.n_useitem();
		NS::XmlUtil::getAttribute(info.m_skipPassiveTrigger.Flag, xmlItem, "n_SkipPassiveTrigger");// = proto.n_skippassivetrigger();
		NS::XmlUtil::getAttribute(info.m_area, xmlItem, "n_Area");// = proto.n_area();
		NS::XmlUtil::getAttribute(info.m_specialKillNpc, xmlItem, "n_Special_KillNpc");// = proto.n_special_killnpc();
		NS::XmlUtil::getAttribute(info.m_specialKillMob, xmlItem, "n_Special_KillMob");// = proto.n_special_killmob();
		NS::XmlUtil::getAttribute(info.m_specialDigItem, xmlItem, "n_Special_DigItem");// = proto.n_special_digitem();
		NS::XmlUtil::getAttribute(info.m_passiveTrigger, xmlItem, "n_PassiveTrigger");// = proto.n_passivetrigger();
		NS::XmlUtil::getAttribute(info.m_moon, xmlItem, "n_Moon");// = proto.n_moon();
		NS::XmlUtil::getAttribute(info.m_weather, xmlItem, "n_Weather");// = proto.n_weather();
		NS::XmlUtil::getAttribute(info.m_alternateMomentBegin, xmlItem, "n_AlternateMomentBegin");// = proto.n_alternatemomentbegin();
		NS::XmlUtil::getAttribute(info.m_alternateMomentEnd, xmlItem, "n_AlternateMomentEnd");// = proto.n_alternatemomentend();
		NS::XmlUtil::getAttribute(info.m_playTime, xmlItem, "n_PlayTime");// = proto.n_playtime();
		NS::XmlUtil::getAttribute(info.m_mode, xmlItem, "n_Mode");// = proto.n_mode();
		NS::XmlUtil::getAttribute(info.m_probability, xmlItem, "f_Probability");// = proto.f_probability();
        NS::XmlUtil::getAttribute(info.m_firstFewDrop, xmlItem, "n_FirstFewDrop");// = proto.n_firstfewdrop();
		NS::XmlUtil::getAttribute(info.m_maxMob, xmlItem, "n_MaxMob");// = proto.n_maxmob();
		NS::XmlUtil::getAttribute(info.m_changeWaveType, xmlItem, "n_ChangeWaveType");// = proto.n_changewavetype();
		NS::XmlUtil::getAttribute(info.m_targetInfo, xmlItem, "n_TargetInfo");// = proto.n_targetinfo();
		NS::XmlUtil::getAttribute(info.m_winCondition, xmlItem, "n_WinCondition");// = proto.n_wincondition();
		NS::XmlUtil::getAttribute(info.m_loseCondition, xmlItem, "n_LoseCondition");// = proto.n_losecondition();
		NS::XmlUtil::getAttribute(info.m_dropProcessId, xmlItem, "n_DropProcessID");// = proto.n_dropprocessid();
		NS::XmlUtil::getAttribute(info.m_onlyBirthMob, xmlItem, "n_OnlyBirthMob");// = proto.n_onlybirthmob();
		NS::XmlUtil::getAttribute(info.m_nMapID, xmlItem, "n_MapID");// = proto.n_mapid();
		NS::XmlUtil::getAttribute(info.m_nMinLevel, xmlItem, "n_MinLevel");// = proto.n_minlevel();
		NS::XmlUtil::getAttribute(info.m_nMaxLevel, xmlItem, "n_MaxLevel");// = proto.n_maxlevel();
		NS::XmlUtil::getAttribute(info.m_headimgres, xmlItem, "s_headimgres");// = proto.s_headimgres();
		NS::XmlUtil::getAttribute(info.m_speak, xmlItem, "n_speak");// = proto.n_speak();
		NS::XmlUtil::getAttribute(info.m_endSpeak, xmlItem, "n_end_speak");// = proto.n_end_speak();
		NS::XmlUtil::getAttribute(info.m_bossSpeak, xmlItem, "n_boss_speak");// = proto.n_boss_speak();
		NS::XmlUtil::getAttribute(info.m_mobId, xmlItem, "n_mob_id");// = proto.n_mob_id();
		NS::XmlUtil::getAttribute(info.m_type, xmlItem, "n_type");// = proto.n_type();
		NS::XmlUtil::getAttribute(info.m_priority, xmlItem, "n_priority");// = proto.n_priority();
        NS::XmlUtil::getAttribute(info.m_target_untrans, xmlItem, "s_MissionTarget");// = proto.s_missiontarget();
        NS::XmlUtil::getAttribute(info.m_desWin_untrans, xmlItem, "s_VictoryText");// = proto.s_victorytext();
        NS::XmlUtil::getAttribute(info.m_desLose_untrans, xmlItem, "s_FailedText");// = proto.s_failedtext();
		std::string s_Bgm;
		NS::XmlUtil::getAttribute(s_Bgm, xmlItem, "s_Bgm");
        NS::StringUtil::splitString(info.m_bgms, s_Bgm, ",");
		NS::XmlUtil::getAttribute(info.m_nKillMobID, xmlItem, "n_KillMobID");// = proto.n_killmobid();

		std::string s_superimpose;
		NS::XmlUtil::getAttribute(s_superimpose, xmlItem, "s_Superimpose");
        if (!s_superimpose.empty())
        {
            NS::StringUtil::splitString(info.m_setSuperimpose, s_superimpose, ",");
        }

		std::string s_birthEffect;
		NS::XmlUtil::getAttribute(s_birthEffect, xmlItem, "s_birthEffect");
		StringUtil::splitString(info.m_birthEffect, s_birthEffect, ";");
		
		std::vector<NS::tstring> v;
		std::string s_birthgrids;
		NS::XmlUtil::getAttribute(s_birthgrids, xmlItem, "s_birthGrids");
		NS::StringUtil::splitString(v, s_birthgrids, ",");
		for (decltype(v.size()) i = 0; i < v.size(); ++i)
		{
			glm::ivec2 pos;
			NS::int32 count = sscanf(v[i].c_str(), "[%d;%d]", &pos.x, &pos.y);
			if (count < 2) continue;
			info.m_birthGrids.push_back(pos);
		}

		NS::XmlUtil::getAttribute(info.m_bIsAtmosphere, xmlItem, "n_atmosphere");// = proto.n_atmosphere();
		NS::XmlUtil::getAttribute(info.m_bChkCrystal, xmlItem, "n_ChkCrystal");// = proto.n_chkcrystal();
		NS::XmlUtil::getAttribute(info.m_bCanRepairCrystal, xmlItem, "n_repair_crystal");// = proto.n_repair_crystal();
		NS::XmlUtil::getAttribute(info.m_wave, xmlItem, "n_wave");// = proto.n_wave();
		NS::XmlUtil::getAttribute(info.m_lessMNum, xmlItem, "n_remaining_num");// = proto.n_remaining_num();

		m_eventMap.insert(std::make_pair(info.m_id, info));
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

bool SurvivalEventCfg::parseSurvivalEventMobPoolConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		EventMobPoolInfo info;

		NS::XmlUtil::getAttribute(info.m_id, xmlItem, "n_ID");//()];

		for (int i = 0 ; i < 7; i ++)
		{
			S_SurvivalEventMobPoolTemplate_Mob tmp;
			NS::XmlUtil::getAttribute(tmp.n_MobID, xmlItem, fmt::format("n_MobID_{0}", i).c_str());//()];
			NS::XmlUtil::getAttribute(tmp.n_Score, xmlItem, fmt::format("n_Score_{0}", i).c_str());//()];
			NS::XmlUtil::getAttribute(tmp.n_percent, xmlItem, fmt::format("n_percent_{0}", i).c_str());//()];
			if (!tmp.Empty()) info.m_mobs.push_back(tmp);
		}
		m_eventMobPoolMap.insert(std::make_pair(info.m_id, info));
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

bool SurvivalEventCfg::parseSurvivalEventMobListConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		EventMobListInfo info;//
       // auto& info = m_eventMobListMap[proto.n_id()];
		NS::XmlUtil::getAttribute(info.m_id, xmlItem, "n_ID");//()];
		NS::XmlUtil::getAttribute(info.m_survivalEventID, xmlItem, "n_SurvivalEventID");//()];
		NS::XmlUtil::getAttribute(info.m_wave, xmlItem, "n_Wave");// = proto.n_wave();
		NS::XmlUtil::getAttribute(info.m_mobCount, xmlItem, "n_MobCount");// = proto.n_mobcount();
		NS::XmlUtil::getAttribute(info.m_changeWaveScore, xmlItem, "n_ChangeWaveScore");// = proto.n_changewavescore();
		NS::XmlUtil::getAttribute(info.m_changeWaveTime, xmlItem, "n_ChangeWaveTime");// = proto.n_changewavetime();
		NS::XmlUtil::getAttribute(info.m_cycle, xmlItem, "n_Cycle");// = proto.n_cycle();
		NS::XmlUtil::getAttribute(info.m_bornType, xmlItem, "n_BornType");// = proto.n_borntype();
		NS::XmlUtil::getAttribute(info.m_minDistance, xmlItem, "n_MinDistance");//, xmlItem, "n_ID");// proto.n_mindistance();
		NS::XmlUtil::getAttribute(info.m_maxDistance, xmlItem, "n_MaxDistance");// proto.n_maxdistance();
		NS::XmlUtil::getAttribute(info.m_minDistanceY, xmlItem, "n_MinDistanceY");// proto.n_mindistancey();
		NS::XmlUtil::getAttribute(info.m_maxDistanceY, xmlItem, "n_MaxDistanceY");// proto.n_maxdistancey();
		NS::XmlUtil::getAttribute(info.m_occupancyX, xmlItem, "n_Occupancy_X");// proto.n_occupancy_x();
		NS::XmlUtil::getAttribute(info.m_occupancyY, xmlItem, "n_Occupancy_Y");// proto.n_occupancy_y();
		NS::XmlUtil::getAttribute(info.m_leftPercentage, xmlItem, "n_Left_Percentage");// proto.n_left_percentage();
		NS::XmlUtil::getAttribute(info.m_rightPercentage, xmlItem, "n_Right_Percentage");// proto.n_right_percentage();
		NS::XmlUtil::getAttribute(info.m_mobRectA, xmlItem, "n_MobRect_A");// proto.n_mobrect_a();
		NS::XmlUtil::getAttribute(info.m_mobRectB, xmlItem, "n_MobRect_B");// proto.n_mobrect_b();
		NS::XmlUtil::getAttribute(info.m_mobRectC, xmlItem, "n_MobRect_C");// proto.n_mobrect_c();
		NS::XmlUtil::getAttribute(info.m_mobRectD, xmlItem, "n_MobRect_D");// proto.n_mobrect_d();
		NS::XmlUtil::getAttribute(info.m_mobRectE, xmlItem, "n_MobRect_E");// proto.n_mobrect_e();
		NS::XmlUtil::getAttribute(info.m_mobRectF, xmlItem, "n_MobRect_F");// proto.n_mobrect_f();
		NS::XmlUtil::getAttribute(info.m_mobRectG, xmlItem, "n_MobRect_G");// proto.n_mobrect_g();
		NS::XmlUtil::getAttribute(info.m_mobRectH, xmlItem, "n_MobRect_H");// proto.n_mobrect_h();

		for (int i = 0; i < 10; i++)
		{
			S_SurvivalEventMobListTemplate_Mob tmpData;
			NS::XmlUtil::getAttribute(tmpData.n_PoolID, xmlItem, fmt::format("n_PoolID_{0}", i).c_str());//()];
			NS::XmlUtil::getAttribute(tmpData.n_Count, xmlItem, fmt::format("n_Count_{0}", i).c_str());//()];
			NS::XmlUtil::getAttribute(tmpData.n_Interval, xmlItem, fmt::format("n_Interval_{0}", i).c_str());//()];
			if (!tmpData.Empty()) info.m_mobs.push_back(tmpData);
		}

		for (int i = 0; i < 10; i++)
		{
			S_SurvivalEventMobListTemplate_Mob tmpData;
			NS::XmlUtil::getAttribute(tmpData.n_PoolID, xmlItem, fmt::format("n_CyclePoolID_{0}", i).c_str());//()];
			NS::XmlUtil::getAttribute(tmpData.n_Count, xmlItem, fmt::format("n_CycleCount_{0}", i).c_str());//()];
			NS::XmlUtil::getAttribute(tmpData.n_Interval, xmlItem, fmt::format("n_CycleInterval_{0}", i).c_str());//()];
			if (!tmpData.Empty()) info.m_cycleMobs.push_back(tmpData);
		}
		m_eventMobListMap.insert(std::make_pair(info.m_id, info));
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

//bool SurvivalEventCfg::parseSurvivalEventGuaranteeRewardsConfig(tinyxml2::XMLElement* pXmlRoot)
//{
//    config::SurvivalEventGuaranteeRewardsSet dataList;
//    if (!loadProtoConfig(dataList, "config/SurvivalEventGuaranteeRewards.bin")) return false;
//
//    int itemSize = dataList.survivaleventguaranteerewardss_size();
//    for (int i = 0; i < itemSize; ++i)
//    {
//        auto& proto = dataList.survivaleventguaranteerewardss(i);
//        auto& info = m_eventGuaranteeRewardsMap[proto.n_id()];
//
//        info.m_id = proto.n_id();
//
//        info.m_items.resize(DEF_SURVIVAL_EVENT_ITEM_ARRAY);
//        {
//            info.m_items[0].n_ItemID = proto.n_itemid_0();
//            info.m_items[0].n_ItemCount = proto.n_itemcount_0();
//        }
//
//        {
//            info.m_items[1].n_ItemID = proto.n_itemid_1();
//            info.m_items[1].n_ItemCount = proto.n_itemcount_1();
//        }
//
//        {
//            info.m_items[2].n_ItemID = proto.n_itemid_2();
//            info.m_items[2].n_ItemCount = proto.n_itemcount_2();
//        }
//	}
//
//	return true;
//}

//bool SurvivalEventCfg::parseSurvivalEventNpcConfig(tinyxml2::XMLElement* pXmlRoot)
//{
//	if (!pXmlRoot) { return false; }
//
//	auto p = pXmlRoot->FirstChildElement("Table");
//	if (!p) return false;
//
//	auto xmlItem = p->FirstChildElement("Record");
//	while (xmlItem != nullptr)
//	{
//        //auto& proto = dataList.survivaleventnpcs(i);
//        //auto& info = m_eventNpcMap[proto.n_id()];
//		EventNpcInfo info;
//
//		NS::XmlUtil::getAttribute(info.m_id, xmlItem, "n_ID");//()];
//        info.m_id = proto.n_id();
//
//        {
//            S_SurvivalEventNpcTemplate_Npc tmp;
//            tmp.n_MobID = proto.n_mobid_0();
//            tmp.n_BirthPointX = proto.n_birthpointx_0();
//            tmp.n_BirthPointY = proto.n_birthpointy_0();
//            tmp.n_ItemID = proto.n_itemid_0();
//            tmp.n_ItemCount = proto.n_itemcount_0();
//            info.m_npcs.push_back(tmp);
//        }
//
//        {
//            S_SurvivalEventNpcTemplate_Npc tmp;
//            tmp.n_MobID = proto.n_mobid_1();
//            tmp.n_BirthPointX = proto.n_birthpointx_1();
//            tmp.n_BirthPointY = proto.n_birthpointy_1();
//            tmp.n_ItemID = proto.n_itemid_1();
//            tmp.n_ItemCount = proto.n_itemcount_1();
//            info.m_npcs.push_back(tmp);
//        }
//
//        {
//            S_SurvivalEventNpcTemplate_Npc tmp;
//            tmp.n_MobID = proto.n_mobid_2();
//            tmp.n_BirthPointX = proto.n_birthpointx_2();
//            tmp.n_BirthPointY = proto.n_birthpointy_2();
//            tmp.n_ItemID = proto.n_itemid_2();
//            tmp.n_ItemCount = proto.n_itemcount_2();
//            info.m_npcs.push_back(tmp);
//        }
//	}
//
//	return true;
//}

bool SurvivalEventCfg::parseSurvivalEventRateupConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		EventRateupInfo info;
        //auto& proto = dataList.survivaleventrateups(i);
        //auto& info = m_eventRateupMap[proto.n_id()];

		NS::XmlUtil::getAttribute(info.m_id, xmlItem, "n_ID");//()];
		NS::XmlUtil::getAttribute(info.m_achievementID, xmlItem, "n_AchievementID");// = proto.n_achievementid();
		NS::XmlUtil::getAttribute(info.m_level, xmlItem, "n_Level");// = proto.n_level();
		NS::XmlUtil::getAttribute(info.m_correctionProbability, xmlItem, "f_CorrectionProbability");// = proto.f_correctionprobability();
		m_eventRateupMap.insert(std::make_pair(info.m_id, info));
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool SurvivalEventCfg::parseSurvivalEventMobReplaceConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		EventMobReplaceInfo info;
        //auto& proto = dataList.survivaleventmobreplaces(i);
       // auto& info = m_eventMobReplaceMap[proto.n_id()];

		NS::XmlUtil::getAttribute(info.m_id, xmlItem, "n_ID");//()];
		NS::XmlUtil::getAttribute(info.m_survivalEventID, xmlItem, "n_SurvivalEventID");// = proto.n_achievementid();
		NS::XmlUtil::getAttribute(info.m_mobID, xmlItem, "n_MobID");// = proto.n_level();
		NS::XmlUtil::getAttribute(info.m_replaceMobID, xmlItem, "n_ReplaceMobID");// = proto.f_correctionprobability();
		m_eventMobReplaceMap.insert(std::make_pair(info.m_id, info));
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool SurvivalEventCfg::parseSurvivalEventDefenseDamageConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
        //auto& proto = dataList.survivaleventdefensedamages(i);

		int n_defense_id = 0;//proto.n_defense_id();
		int n_defense_lvl1 = 0;//proto.n_defense_lvl1();

		NS::XmlUtil::getAttribute(n_defense_id, xmlItem, "n_defense_id");//()];
		NS::XmlUtil::getAttribute(n_defense_lvl1, xmlItem, "n_defense_lvl1");//()];
        auto& info = m_mapDefenseDamage[CalcDefenseDamageKey(n_defense_id, n_defense_lvl1)];
		NS::XmlUtil::getAttribute(info.n_defense_lvl2, xmlItem, "n_defense_lvl2");// = proto.n_defense_lvl2();
		NS::XmlUtil::getAttribute(info.n_defense_damage, xmlItem, "n_defense_damage");// = proto.n_defense_damage();
		NS::XmlUtil::getAttribute(info.n_defense_frequnce, xmlItem, "n_defense_frequnce");// = proto.n_defense_frequnce();
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool SurvivalEventCfg::parseSurvivalEventMobDamageConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		EventMobDamageInfo info;// = m_mapMobDamage[proto.n_mob_id()];

		NS::XmlUtil::getAttribute(info.n_mob_id, xmlItem, "n_mob_id");//()];
		NS::XmlUtil::getAttribute(info.n_mob_damage, xmlItem, "n_mob_lvl");//
		NS::XmlUtil::getAttribute(info.n_mob_frequnce, xmlItem, "n_mob_damage");// 
		NS::XmlUtil::getAttribute(info.n_mob_frequnce, xmlItem, "n_mob_frequnce");//
		m_mapMobDamage.insert(std::make_pair(info.n_mob_id, info));
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

const SurvivalEventCfg::EventInfo* SurvivalEventCfg::findEventInfo(int id)
{
	auto it = m_eventMap.find(id);
	return (it == m_eventMap.end()) ? nullptr : &it->second;
}

const SurvivalEventCfg::EventGuaranteeRewardsInfo* SurvivalEventCfg::findEventGuaranteeRewardsInfo(int id)
{
	auto it = m_eventGuaranteeRewardsMap.find(id);
	return (it == m_eventGuaranteeRewardsMap.end()) ? nullptr : &it->second;
}

const SurvivalEventCfg::EventNpcInfo* SurvivalEventCfg::findEventNpcInfo(int id)
{
	auto it = m_eventNpcMap.find(id);
	return (it == m_eventNpcMap.end()) ? nullptr : &it->second;
}

const SurvivalEventCfg::EventRateupInfo* SurvivalEventCfg::findEventRateupInfo(int id)
{
	auto it = m_eventRateupMap.find(id);
	return (it == m_eventRateupMap.end()) ? nullptr : &it->second;
}

//////////////////////////////////////////////////////////////////////////
void SurvivalEventCfg::forEachSurvivalEvent(const std::function<void(const EventInfo&)>& fun)
{
	if (!fun) return;

	for (const auto& iter : m_eventMap)
	{
		fun(iter.second);
	}
}

//////////////////////////////////////////////////////////////////////////
void SurvivalEventCfg::forEachSurvivalEventMobList(const std::function<void(const EventMobListInfo&)>& fun)
{
	if (!fun) return;

	for (const auto& iter : m_eventMobListMap)
	{
		fun(iter.second);
	}
}

//////////////////////////////////////////////////////////////////////////
bool SurvivalEventCfg::EventInfo::CheckMoonTrigger() const
{
	//if (m_moon >= DayNightTime::MOON::MOON_0 && m_moon < DayNightTime::MOON::MOON_NUM) return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////
const SurvivalEventCfg::EventMobListInfo* SurvivalEventCfg::findEventMobListInfo(int id)
{
	auto it = m_eventMobListMap.find(id);
	return (it == m_eventMobListMap.end()) ? nullptr : &it->second;
}

//////////////////////////////////////////////////////////////////////////
const SurvivalEventCfg::EventMobPoolInfo* SurvivalEventCfg::findEventMobPoolInfo(int id)
{
	auto it = m_eventMobPoolMap.find(id);
	return (it == m_eventMobPoolMap.end()) ? nullptr : &it->second;
}

//////////////////////////////////////////////////////////////////////////
void SurvivalEventCfg::forEachSurvivalEventMobReplace(const std::function<void(const EventMobReplaceInfo&)>& fun)
{
	if (!fun) return;

	for (const auto& iter : m_eventMobReplaceMap)
	{
		fun(iter.second);
	}
}

bool SurvivalEventCfg::reloadMultiLanguage()
{
	for (auto& item : m_eventMap)
	{
		item.second.m_name = _LC(item.second.m_name_untrans);
		item.second.m_target = _LC(item.second.m_target_untrans);
		item.second.m_desWin = _LC(item.second.m_desWin_untrans);
		item.second.m_desLose = _LC(item.second.m_desLose_untrans);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
const SurvivalEventCfg::EventDefenseDamageInfo* SurvivalEventCfg::findEventDefenseDamageInfo(int nBuildType, int lev)
{
	auto it = m_mapDefenseDamage.find(CalcDefenseDamageKey(nBuildType, lev));
	return (it == m_mapDefenseDamage.end()) ? nullptr : &it->second;
}

//////////////////////////////////////////////////////////////////////////
const SurvivalEventCfg::EventMobDamageInfo* SurvivalEventCfg::findEventMobDamageInfo(int nMobDef)
{
	auto it = m_mapMobDamage.find(nMobDef);
	return (it == m_mapMobDamage.end()) ? nullptr : &it->second;
}
