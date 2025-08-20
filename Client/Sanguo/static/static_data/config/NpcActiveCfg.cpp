/*!
* \file NpcActiveCfg.cpp
* \date 08-30-2017 15:29:43
*
* \author LiQiangqiang
*/
#include "stdafx.h"
#include "NpcActiveCfg.h"
#include "../common/calculate.h"
#include "MonsterCfg.h"
#include <XmlUtil.h>
////#include "BuildRoomComfortReward.pb.h"
//#include "ConfigLoaderUtil.h"
//#include "BuildRoomComfortConditions.pb.h"
//#include "NpcActive.pb.h"
//#include "BuildRoomActive.pb.h"
//#include "NpcDialogActive.pb.h"
//#include "GuildDialog.pb.h"

NpcActiveCfg& NpcActiveCfg::getInstance()
{
	static NpcActiveCfg s_NpcActiveCfg;
	return s_NpcActiveCfg;
}

bool NpcActiveCfg::initialize()
{
	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/BuildRoomComfortConditions.bin")) return false;

	return parseBuildRoomComfortConditionsConfig(doc.RootElement());
    //if (!parseNpcActiveBinary()) return false;
    //if (!parseBuildRoomComfortRewardBinary()) return false;
    //if (!parseBuildRoomComfortConditionsBinary()) return false;
    //if (!parseBuildRoomActiveBinary()) return false;
    //if (!parseNpcDialogActiveBinary()) return false;
    //if (!parseGuildDialogBinary()) return false;

    reloadMultiLanguage();

	return true;
}

void NpcActiveCfg::terminate()
{
	m_npcActiveDefMap.clear();
	m_npcComfortMap.clear();
	m_comfortVec.clear();
	m_buildRoomActiveVec.clear();
	m_npcDialogActiveMap.clear();
	m_guildDialogMap.clear();
}

const NpcActiveCfg::NpcActiveDef* NpcActiveCfg::findNpcActiveDef(int id)
{
	auto it = m_npcActiveDefMap.find(id);
	if (it == m_npcActiveDefMap.end()) return nullptr;
	return &(it->second);
}

bool NpcActiveCfg::isAMobActorAsANpc(int mobId)
{
	auto data = findBuildRoomActive(mobId, 1);
	if (data)
		return true;
	auto cfg = MonsterCfg::getInstance().findMonsterType(mobId);
	if (cfg && cfg->n_type >= MonsterFuncType::battleNpc && cfg->n_type <= MonsterFuncType::cavNpc)
		return true;
	return false;
}

bool NpcActiveCfg::parseBuildRoomComfortRewardConfig(tinyxml2::XMLElement* pXmlRoot)
{
	//config::BuildRoomComfortRewardSet dataList;
	//if (!loadProtoConfig(dataList, "config/BuildRoomComfortReward.bin")) return false;
	//int count = dataList.buildroomcomfortrewards_size();
	//for (int i = 0; i < count; ++i)
	//{
	//	auto &proto = dataList.buildroomcomfortrewards(i);
	//	auto &mapComfor = m_npcComfortMap[proto.n_id()];
	//	mapComfor.m_level = proto.n_id();
	//	mapComfor.m_softIcon = proto.s_feeling_icon();
	//	mapComfor.m_des_untrans = proto.s_feeling_key();
	//	mapComfor.m_gift_untrans = proto.s_comfortlv_present();
	//}

	return true;
}

bool NpcActiveCfg::parseBuildRoomActiveConfig(tinyxml2::XMLElement* pXmlRoot)
{
	//config::BuildRoomActiveSet dataList;
	//if (!loadProtoConfig(dataList, "config/BuildRoomActive.bin")) return false;

	//m_buildRoomActiveVec.clear();

	//for (int i = 0; i < dataList.buildroomactives_size(); ++i)
	//{
	//	auto &proto = dataList.buildroomactives(i);
	//	BuildRoomActive info{};
	//	info.id = proto.n_id();
	//	info.level = proto.n_level();
	//	info.npcId = proto.n_npc_id();
	//	info.activeId = proto.n_active_id();
	//	info.vecBuff.clear();
	//	auto addTemplate = [](BuildRoomActive& info, int buffid)
	//	{
	//		if (buffid != 0)
	//		{
	//			info.vecBuff.push_back(buffid);
	//		}
	//	};
	//	addTemplate(info, proto.n_buff_0());
	//	addTemplate(info, proto.n_buff_1());
	//	addTemplate(info, proto.n_buff_2());

	//	info.skill = atoi(proto.s_npc_skill().c_str());
	//	info.plantGrowSpeedup = proto.n_plant_grow_speedup();
	//	info.enchantingProportion = proto.n_enchanting_proportion();
	//	info.synthesisMinus = proto.n_synthesis_proportion01();
	//	info.forgingMinus = proto.n_synthesis_proportion02();
	//	info.sellAwardCoinProportion = proto.n_sell_award_coin_proportion();
	//	info.buyCostProportion = proto.n_buy_cost_proportion();
	//	info.follow_dis = proto.n_follow_dis();
	//	info.follow_max_dis = proto.n_follow_max_dis();
	//	info.check_time = proto.n_check_time();
	//	info.taskProportion = proto.n_npctask_proportion();
	//	info.animal_pro_small = proto.n_animal_output_time();
	//	info.animal_hug_add = proto.n_animal_hunger_time();
	//	info.headIcon = proto.s_npc_head();
	//	info.halfIcon = proto.s_headimgres();
	//	info.s_how_get_key = proto.s_source_key();
	//	info.n_overRangeIsNeedPop = proto.n_need_energy();
	//	m_buildRoomActiveVec.push_back(info);

	//}

	return true;
}

bool NpcActiveCfg::parseNpcActiveConfig(tinyxml2::XMLElement* pXmlRoot)
{
	//if (!pXmlRoot) { return false; }

	//auto p = pXmlRoot->FirstChildElement("Table");
	//if (!p) return false;

	//auto xmlItem = p->FirstChildElement("Record");
	//while (xmlItem != nullptr)
	//{
	//	auto &proto = datalist.npcactives(i);
	//	auto & info = m_npcActiveDefMap[proto.n_id()];
	//	info.n_ID = proto.n_id();
	//	info.f_Icon_OffsetY = proto.f_icon_offsety();
	//	info.isPopCommonWin = proto.n_pop();
	//	info.npcStatement_untrans = proto.s_npc_statement();
	//	info.npcStatement = _LC(info.npcStatement_untrans);
	//	info.m_npcActiveEffects.clear();
	//	auto addTemplate = [](NpcActiveDef& info, int effectId,int dialogId,const NS::tstring &desc_untrans)
	//	{
	//		NpcActiveEffect data{};
	//		if (effectId > 0 || dialogId > 0)
	//		{
	//			data.n_dialog_id = dialogId;
	//			data.n_effect_id = effectId;
	//			data.desc_untrans = desc_untrans;
	//			data.desc = _LC(data.desc_untrans);
	//			info.m_npcActiveEffects.push_back(data);
	//		}
	//	};
	//	addTemplate(info, proto.n_effect_0(), proto.n_dialog_0(), proto.s_note_0());
	//	addTemplate(info, proto.n_effect_1(), proto.n_dialog_1(), proto.s_note_1());
	//	addTemplate(info, proto.n_effect_2(), proto.n_dialog_2(), proto.s_note_2());
	//	addTemplate(info, proto.n_effect_3(), proto.n_dialog_3(), proto.s_note_3());
	//}
	return true;
}

bool NpcActiveCfg::parseBuildRoomComfortConditionsConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		//auto &proto = datalist.buildroomcomfortconditionss(i);
		ComfortConditions info{};
		NS::XmlUtil::getAttribute(info.id, xmlItem, "n_ID");// = proto.n_itemid();
		NS::XmlUtil::getAttribute(info.level, xmlItem, "n_ItemID");// = proto.n_comfortlevel();
		NS::XmlUtil::getAttribute(info.needComfort, xmlItem, "n_ComfortLevel");// = proto.n_comfortable();
		NS::XmlUtil::getAttribute(info.rent, xmlItem, "n_Rent");// = proto.n_rent();
		NS::XmlUtil::getAttribute(info.giftProb, xmlItem, "f_GiftProb");// = proto.f_giftprob();
		NS::XmlUtil::getAttribute(info.gift, xmlItem, "n_Gift");// = proto.n_gift();
		NS::XmlUtil::getAttribute(info.buffTarget, xmlItem, "n_BuffTarget");// = proto.n_buff();
		NS::XmlUtil::getAttribute(info.needCount, xmlItem, "n_QualityGradeCount");// = proto.n_qualitygradecount();
		NS::XmlUtil::getAttribute(info.needQuqlity, xmlItem, "n_QualityGrade");// = proto.n_qualitygrade();
		m_comfortVec.push_back(info);
		xmlItem = xmlItem->NextSiblingElement("Record");
	}
	return true;
}

const NpcActiveCfg::NpcComfort* NpcActiveCfg::getComfortCfg(int level) const
{
	if (level == 0)
	{
		level = 10000;
	}
	
	auto iter = m_npcComfortMap.find(level);
	if (iter != m_npcComfortMap.end())
	{
		return &(iter->second);
	}
	return nullptr;
}

NpcActiveCfg::ComfortConditions *NpcActiveCfg::getComfortCondtionsCfg(int id, int level)
{
	for (auto&& it: m_comfortVec)
	{
		if (it.id == id && it.level == level)
		{
			return &it;
		}
	}
	return nullptr;
}

int NpcActiveCfg::getActiveId(int npcId, int badgeLevel)
{
	for (const auto& it: m_buildRoomActiveVec)
	{
		if (it.npcId == npcId && it.level == badgeLevel)
		{
			return it.activeId;
		}
	}
	return -1;
}

const NpcActiveCfg::NpcActiveDef* NpcActiveCfg::findNpcActive(int npcId, int badgeLevel)
{
	auto id = getActiveId(npcId, badgeLevel);
	return findNpcActiveDef(id);
}

const NpcActiveCfg::BuildRoomActive* NpcActiveCfg::findBuildRoomActive(int npcId, int badgeLevel)
{
	for (auto&& it : m_buildRoomActiveVec)
	{
		if (it.npcId == npcId && it.level == badgeLevel)
		{
			return &it; //todo vector resize
		}
	}
	return nullptr;
}

NpcActiveCfg::NpcDialogActive * NpcActiveCfg::getNpcDialogActiveInfo(int npcActiveId)
{
	for (auto &&it: m_npcDialogActiveMap)
	{
		if (it.second.npcActiveId == npcActiveId)
		{
			return &(it.second);
		}
	}
	return nullptr;
}

NpcActiveCfg::GuildDialogActive * NpcActiveCfg::getGuildDialogInfo(int id)
{
	auto it = m_guildDialogMap.find(id);
	if (it != m_guildDialogMap.end())
		return &(it->second);
	return nullptr;
}

bool NpcActiveCfg::reloadMultiLanguage()
{
	for (auto& item : m_npcComfortMap)
	{
		item.second.m_des = _LC(item.second.m_des_untrans);
		item.second.m_gift = _LC(item.second.m_gift_untrans);
	}

	for (auto& item : m_npcActiveDefMap)
	{
		item.second.npcStatement = _LC(item.second.npcStatement_untrans);
		for (auto& subItem : item.second.m_npcActiveEffects)
		{
			subItem.desc = _LC(subItem.desc_untrans);
		}
	}

	for (auto& item : m_npcDialogActiveMap)
	{
		for (auto& subItem : item.second.vecBtn)
		{
			subItem.des = _LC(subItem.des_untrans);
		}
	}

	for (auto& item : m_guildDialogMap)
	{
		for (size_t i = 0; i < item.second.vecContent.size(); ++i)
		{
			item.second.vecContent[i] = _LC(item.second.vecContent_untrans[i]);
		}
	}

	return true;
}

//bool NpcActiveCfg::parseNpcDialogActiveBinary()
//{
//	config::NpcDialogActiveSet dataList{};
//	if (!loadProtoConfig(dataList, "config/NpcDialogActive.bin")) return false;
//	for (int i = 0; i < dataList.npcdialogactives_size(); ++i)
//	{
//		auto &proto = dataList.npcdialogactives(i);
//		auto &info = m_npcDialogActiveMap[proto.n_id()];
//		info.id = proto.n_id();
//		info.npcActiveId = proto.n_npcactiveid();
//		info.vecBtn.clear();
//		auto addTemplate = [](NpcDialogActive& info, int btn, const NS::tstring& btnText, const NS::tstring &des_untrans)
//		{
//			NpcDialogActive::btnInfo data{};
//			if (btn != 0)
//			{
//				data.des_untrans = des_untrans;
//				data.textId = atoi(btnText.c_str());
//				data.des = _LC(data.des_untrans);
//				info.vecBtn.push_back(data);
//			}
//		};
//		addTemplate(info, proto.n_btn1(), proto.s_btn1text(), proto.s_btn1des());
//		addTemplate(info, proto.n_btn2(), proto.s_btn2text(), proto.s_btn2des());
//		addTemplate(info, proto.n_btn3(), proto.s_btn3text(), proto.s_btn3des());
//		addTemplate(info, proto.n_btn4(), proto.s_btn4text(), proto.s_btn4des());
//		addTemplate(info, proto.n_btn5(), proto.s_btn5text(), proto.s_btn5des());
//		addTemplate(info, proto.n_btn6(), proto.s_btn6text(), proto.s_btn6des());
//		addTemplate(info, proto.n_btn7(), proto.s_btn7text(), proto.s_btn7des());
//		addTemplate(info, proto.n_btn8(), proto.s_btn8text(), proto.s_btn8des());
//	}
//	return true;
//}
//
//bool NpcActiveCfg::parseGuildDialogBinary()
//{
//	config::GuildDialogSet dataList{};
//	if (!loadProtoConfig(dataList, "config/GuildDialog.bin")) return false;
//	for (int i = 0; i < dataList.guilddialogs_size(); ++i)
//	{
//		auto &proto = dataList.guilddialogs(i);
//		auto &info = m_guildDialogMap[proto.n_id()];
//		info.id = proto.n_id();
//		info.type = proto.n_type();
//		info.vecContent.clear();
//		info.vecContent_untrans.clear();
//		auto addTemplate = [](GuildDialogActive& info,const NS::tstring &content)
//		{
//			if (!content.empty() && content != "")
//			{
//				info.vecContent_untrans.push_back(content);
//				auto contentLan = _LC(content);
//				info.vecContent.push_back(contentLan);
//			}
//		};
//		addTemplate(info, proto.s_content_1());
//	}
//	return true;
//}