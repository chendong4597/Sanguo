/*!
 * \file MonsterCfg.cpp
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 05/31/2016
 *
 *
 */
#include "stdafx.h"
#include "MonsterCfg.h"
#include <StringUtil.h>
#include "../config/GlobalCfg.h"
#include <XmlUtil.h>

using namespace NS;


MonsterCfg& MonsterCfg::getInstance()
{
	static MonsterCfg s_MonsterCfg;
	return s_MonsterCfg;
}

bool MonsterCfg::initializeCfg()
{
	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/Mob.bin")) return false;
	if (!parseMonsterConfig(doc.RootElement())) return false;

	return true;
}

void MonsterCfg::terminate()
{
	m_monsterTypeMap.clear();
}

const MonsterCfg::MonsterType* MonsterCfg::findMonsterType(int id) const
{
	auto it = m_monsterTypeMap.find(id);
	if (it == m_monsterTypeMap.end()) return nullptr;
	return &it->second;
}


bool MonsterCfg::parseMonsterConfig(tinyxml2::XMLElement* pXmlRoot)
{

	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto pAttrXml = p->FirstChildElement("Attribute");
	while (pAttrXml)
	{
		std::string strKey;
		NS::XmlUtil::getAttribute(strKey, pAttrXml, "Name");
		if (strKey == "f_SpawnerSpaceScale")
		{
			NS::XmlUtil::getAttribute(m_spawnerSpaceScale, pAttrXml, "Value");
		} 
		else if (strKey == "n_RecHpTime")
		{
			NS::XmlUtil::getAttribute(m_recHpTime, pAttrXml, "Value");
		}
		else if (strKey == "n_RecHpRare")
		{
			NS::XmlUtil::getAttribute(m_recHpRare, pAttrXml, "Value");
		}
		else if (strKey == "n_SrvDropRare")
		{
			NS::XmlUtil::getAttribute(m_srvDropRare, pAttrXml, "Value");
		}
		else if (strKey == "n_final_blow_coefficient")
		{
			NS::XmlUtil::getAttribute(m_finalBlowCoefficient, pAttrXml, "Value");
		}
		else if (strKey == "n_repel_distance_time")
		{
			NS::XmlUtil::getAttribute(m_repelDistanceTime, pAttrXml, "Value");
		}
		pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	}

	m_strPath = "/Game/Models/Monster/entity_s";
	m_strDeathName = "dead_Montage";
	m_strHitName = "hurt_Montage";


	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		MonsterType monsterType{};
		NS::XmlUtil::getAttribute(monsterType.n_ID, xmlItem, "n_ID");
		NS::XmlUtil::getAttribute(monsterType.s_name, xmlItem, "s_name");
		NS::XmlUtil::getAttribute(monsterType.s_title, xmlItem, "s_title");
		NS::XmlUtil::getAttribute(monsterType.n_HPM, xmlItem, "n_HPM");
		NS::XmlUtil::getAttribute(monsterType.n_AD, xmlItem, "n_AD");
		NS::XmlUtil::getAttribute(monsterType.n_AP, xmlItem, "n_AP");
		NS::XmlUtil::getAttribute(monsterType.n_ARM, xmlItem, "n_ARM");
		NS::XmlUtil::getAttribute(monsterType.n_MARM, xmlItem, "n_MARM");
		NS::XmlUtil::getAttribute(monsterType.n_AR, xmlItem, "n_AR");
		NS::XmlUtil::getAttribute(monsterType.n_MR_Fire, xmlItem, "n_MR_Fire");
		NS::XmlUtil::getAttribute(monsterType.n_MR_Forst, xmlItem, "n_MR_Forst");
		NS::XmlUtil::getAttribute(monsterType.n_MR_Thunder, xmlItem, "n_MR_Thunder");
		NS::XmlUtil::getAttribute(monsterType.n_MR_Earth, xmlItem, "n_MR_Earth");
		NS::XmlUtil::getAttribute(monsterType.n_MR_Poison, xmlItem, "n_MR_Poison");
		NS::XmlUtil::getAttribute(monsterType.n_MR_Light, xmlItem, "n_MR_Light");
		NS::XmlUtil::getAttribute(monsterType.n_MR_Dark, xmlItem, "n_MR_Dark");
		NS::XmlUtil::getAttribute(monsterType.f_RES, xmlItem, "f_RES");
		NS::XmlUtil::getAttribute(monsterType.f_MRES, xmlItem, "f_MRES");
		NS::XmlUtil::getAttribute(monsterType.f_atkRange, xmlItem, "f_atkRange");
		NS::XmlUtil::getAttribute(monsterType.f_CRI, xmlItem, "f_CRI");
		NS::XmlUtil::getAttribute(monsterType.f_MCRI, xmlItem, "f_MCRI");
		NS::XmlUtil::getAttribute(monsterType.f_ARP, xmlItem, "f_ARP");
		NS::XmlUtil::getAttribute(monsterType.f_MRP, xmlItem, "f_MRP");
		NS::XmlUtil::getAttribute(monsterType.f_CRID, xmlItem, "f_CRID");
		NS::XmlUtil::getAttribute(monsterType.f_MCRID, xmlItem, "f_MCRID");
		NS::XmlUtil::getAttribute(monsterType.n_CC_Hit, xmlItem, "n_CC_Hit");
		NS::XmlUtil::getAttribute(monsterType.n_CC_Res, xmlItem, "n_CC_Res");
		NS::XmlUtil::getAttribute(monsterType.n_HPR, xmlItem, "n_HPR");
		NS::XmlUtil::getAttribute(monsterType.n_HPRS, xmlItem, "n_HPRS");
		NS::XmlUtil::getAttribute(monsterType.n_MPR, xmlItem, "n_MPR");
		NS::XmlUtil::getAttribute(monsterType.n_MPRS, xmlItem, "n_MPRS");
		NS::XmlUtil::getAttribute(monsterType.f_atk_speed_modifiy, xmlItem, "f_atk_speed_modifiy");
		NS::XmlUtil::getAttribute(monsterType.f_Move, xmlItem, "f_Move");
		NS::XmlUtil::getAttribute(monsterType.f_Run, xmlItem, "f_Run");
		NS::XmlUtil::getAttribute(monsterType.f_scale, xmlItem, "f_scale");
		NS::XmlUtil::getAttribute(monsterType.f_uiModelScale, xmlItem, "f_uiModelScale");
		NS::XmlUtil::getAttribute(monsterType.f_Width, xmlItem, "f_Width");
		NS::XmlUtil::getAttribute(monsterType.f_Height, xmlItem, "f_Height");
		NS::XmlUtil::getAttribute(monsterType.f_depth, xmlItem, "f_depth");
		NS::XmlUtil::getAttribute(monsterType.n_Jump, xmlItem, "n_Jump");
		NS::XmlUtil::getAttribute(monsterType.n_gethit_cd, xmlItem, "n_gethit_cd");
		NS::XmlUtil::getAttribute(monsterType.s_UnderAttackEffectID, xmlItem, "s_UnderAttackEffectID");
		NS::XmlUtil::getAttribute(monsterType.s_model, xmlItem, "s_model");
		NS::XmlUtil::getAttribute(monsterType.n_rare, xmlItem, "n_Rare");
		NS::XmlUtil::getAttribute(monsterType.n_kind, xmlItem, "n_kind");
		NS::XmlUtil::getAttribute(monsterType.f_HpUICheck, xmlItem, "f_HpUICheck");
		NS::XmlUtil::getAttribute(monsterType.m_briefRes, xmlItem, "n_Jum_briefResmp");
		NS::XmlUtil::getAttribute(monsterType.s_mob_dead_sound, xmlItem, "s_mob_dead_sound");
		NS::XmlUtil::getAttribute(monsterType.n_noapplylight, xmlItem, "n_noapplylight");
		NS::XmlUtil::getAttribute(monsterType.n_shake_map_id, xmlItem, "n_shake_map_id");
		NS::XmlUtil::getAttribute(monsterType.n_hurtEffectId, xmlItem, "n_hurtEffectId");
		NS::XmlUtil::getAttribute(monsterType.s_hurtEffect, xmlItem, "s_hurtEffect");
		NS::XmlUtil::getAttribute(monsterType.n_Camp, xmlItem, "n_Camp");
		NS::XmlUtil::getAttribute(monsterType.n_Camp_Change, xmlItem, "n_Camp_Change");
		NS::XmlUtil::getAttribute(monsterType.n_LifeCycle, xmlItem, "n_LifeCycle");
		NS::XmlUtil::getAttribute(monsterType.n_life_disappear, xmlItem, "n_life_disappear");
		NS::XmlUtil::getAttribute(monsterType.n_FOV, xmlItem, "n_FOV");
		NS::XmlUtil::getAttribute(monsterType.n_Exp, xmlItem, "n_Exp");
		NS::XmlUtil::getAttribute(monsterType.n_DropId, xmlItem, "n_DropId");
		NS::XmlUtil::getAttribute(monsterType.n_DeadSkill, xmlItem, "n_DeadSkill");
		NS::XmlUtil::getAttribute(monsterType.n_is_capture, xmlItem, "n_is_capture");
		NS::XmlUtil::getAttribute(monsterType.n_egg_id, xmlItem, "n_egg_id");
		NS::XmlUtil::getAttribute(monsterType.n_MobType, xmlItem, "n_MobType");
		NS::XmlUtil::getAttribute(monsterType.f_SpeakUiCheck, xmlItem, "f_SpeakUiCheck");
		NS::XmlUtil::getAttribute(monsterType.n_MobAI, xmlItem, "n_MobAI");
		NS::XmlUtil::getAttribute(monsterType.f_CorpseTime, xmlItem, "f_CorpseTime");
		NS::XmlUtil::getAttribute(monsterType.n_catchBuffId, xmlItem, "n_catchBuffId");
		NS::XmlUtil::getAttribute(monsterType.n_AITypeMob, xmlItem, "n_AITypeMob");
		NS::XmlUtil::getAttribute(monsterType.n_DisappearTime, xmlItem, "n_DisappearTime");
		NS::XmlUtil::getAttribute(monsterType.n_Ignore_AttackObject, xmlItem, "n_Ignore_AttackObject");
		NS::XmlUtil::getAttribute(monsterType.n_PlotDistance, xmlItem, "n_PlotDistance");
		NS::XmlUtil::getAttribute(monsterType.n_PlotId, xmlItem, "n_PlotId");
		NS::XmlUtil::getAttribute(monsterType.n_BornPlotID, xmlItem, "n_BornPlotID");
		NS::XmlUtil::getAttribute(monsterType.n_summon_effect, xmlItem, "n_summon_effect");
		NS::XmlUtil::getAttribute(monsterType.n_isCanBeFly, xmlItem, "n_isCanBeFly");
		//NS::XmlUtil::extractValue(monsterType.s_SoundBankVec, xmlItem, "n_Jump");
		NS::XmlUtil::getAttribute(monsterType.n_hide_hp, xmlItem, "n_hide_hp");
		NS::XmlUtil::getAttribute(monsterType.n_mob_direction, xmlItem, "n_mob_direction");
		NS::XmlUtil::getAttribute(monsterType.n_ischange_direction, xmlItem, "n_ischange_direction");
		NS::XmlUtil::getAttribute(monsterType.n_isRemotely, xmlItem, "n_isRemotely");
		NS::XmlUtil::getAttribute(monsterType.n_npc_voice_type, xmlItem, "n_npc_voice_type");
		NS::XmlUtil::getAttribute(monsterType.n_repel_distance, xmlItem, "n_repel_distance");
		int nSolider = false;
		NS::XmlUtil::getAttribute(nSolider, xmlItem, "n_Solider");
		monsterType.m_bSolider = nSolider == 1;

		//容错：怪物追击距离不小于怪物视野 By YuBo
		if (monsterType.n_FollowRange < monsterType.n_FOV)
		{
			monsterType.n_FollowRange = monsterType.n_FOV;
		}

		NS::XmlUtil::getAttribute(monsterType.n_MobLv, xmlItem, "n_MobLv");
		NS::XmlUtil::getAttribute(monsterType.n_CanAttack, xmlItem, "n_CanAttack");
		NS::XmlUtil::getAttribute(monsterType.n_ForbidAtk, xmlItem, "n_ForbidAtk");
		NS::XmlUtil::getAttribute(monsterType.n_IsOnlyNormalAtk, xmlItem, "n_IsOnlyNormalAtk");
		NS::XmlUtil::getAttribute(monsterType.n_nTowerTpy, xmlItem, "n_nTowerTpy");
		NS::XmlUtil::getAttribute(monsterType.n_MoveType, xmlItem, "n_MoveType");
		NS::XmlUtil::getAttribute(monsterType.n_through_wall, xmlItem, "n_through_wall");

		// parse attack animation and effect
		//bzbee::StringList motionList;
		//int num = bzbee::StringUtil::splitString(motionList, mobProto.s_attackeffect(), ",");
		//for (int i = 0; i < num; ++i)
		//{
		//	bzbee::StringList anieffect;
		//	if (bzbee::StringUtil::splitString(anieffect, motionList[i], "|") != 2) { continue; }
		//	monsterType.m_atkAniEffect.emplace_back(anieffect[0], anieffect[1]);
		//}
		//// parse under attack animation and effect
		//motionList.clear();
		//num = bzbee::StringUtil::splitString(motionList, monsterType.s_UnderAttackEffectID, ",");
		//for (int i = 0; i < num; ++i)
		//{
		//	bzbee::StringList anieffect;
		//	if (bzbee::StringUtil::splitString(anieffect, motionList[i], "|") != 2) { continue; }
		//	monsterType.m_animToUnderAttackEffectId.emplace(anieffect[0], anieffect[1]);
		//}

		//// parse attack animation and effect
		//motionList.clear();
		//num = bzbee::StringUtil::splitString(motionList, mobProto.s_mob_attack_sound(), ",");
		//for (int i = 0; i < num; ++i) {
		//	bzbee::StringList anisound;
		//	if (bzbee::StringUtil::splitString(anisound, motionList[i], "|") == 2) {
		//		monsterType.m_atkAniSound.emplace_back(anisound[0], anisound[1]);
		//	}
		//	else if (num == 1)
		//	{
		//		monsterType.m_atkAniSound.emplace_back(NS::EMPTY_STRING, anisound[0]);
		//		break;
		//	}
		//}

		//bzbee::StringList attackList;
		//num = bzbee::StringUtil::splitString(attackList, mobProto.s_attackobjecttype(), ",");
		//for (int i = 0; i < num; ++i)
		//{
		//	monsterType.s_AttackObjectType.emplace_back(std::atoi(attackList[i].c_str()));
		//}

		//bzbee::StringList speakList{};
		//num = bzbee::StringUtil::splitString(speakList, mobProto.s_speak(), ",");
		//for (int i = 0; i < num; ++i)
		//{
		//	int nSpeakId = atoi(speakList[i].c_str());
		//	if (nSpeakId <= 0)
		//	{
		//		continue;
		//	}
		//	monsterType.speakList.emplace_back(nSpeakId);
		//}

		//// Parse event material texture
		//bzbee::StringList materialList;
		//num = bzbee::StringUtil::splitString(materialList, mobProto.s_matid_evenid(), ",");
		//for (int i = 0; i < num; ++i)
		//{
		//	bzbee::StringList eventToMaterial;
		//	if (bzbee::StringUtil::splitString(eventToMaterial, materialList[i], "|") != 2) { continue; }
		//	MonsterType::EventTypeToMaterial::iterator it;
		//	bool ret;
		//	int eventType;
		//	if (!StringUtil::stringToType(eventType, eventToMaterial[0])) { continue; }
		//	std::tie(it, ret) = monsterType.m_eventTypeToMaterial.emplace(static_cast<bzbee::uint8>(eventType), MonsterType::MaterialTextureVec{});
		//	if (!ret) { continue; }
		//	bzbee::StringList materialIdToTextureList;
		//	int materialNum = bzbee::StringUtil::splitString(materialIdToTextureList, eventToMaterial[1], ":");
		//	for (int j = 0; j < materialNum; ++j)
		//	{
		//		bzbee::StringList materialIdToTexture;
		//		if (bzbee::StringUtil::splitString(materialIdToTexture, materialIdToTextureList[j], "-") != 2 || materialIdToTexture[1].empty()) { continue; }
		//		int materialId;
		//		if (!StringUtil::stringToType(materialId, materialIdToTexture[0])) { continue; }
		//		it->second.emplace_back(static_cast<bzbee::uint8>(materialId), materialIdToTexture[1]);
		//	}
		//}

		NS::XmlUtil::getAttribute(monsterType.n_SpeakInterval, xmlItem, "n_SpeakInterval");
		NS::XmlUtil::getAttribute(monsterType.n_type, xmlItem, "n_type");
		NS::XmlUtil::getAttribute(monsterType.n_mob_place, xmlItem, "n_mob_place");
		//NS::XmlUtil::getAttribute(monsterType.m_bIgnoreSpeakDistance, xmlItem, "m_bIgnoreSpeakDistance");
		//NS::XmlUtil::getAttribute(monsterType.m_bIgnoreFlyElevate, xmlItem, "n_mob_place");
		//NS::XmlUtil::getAttribute(monsterType.m_bDieWithOwner, xmlItem, "n_mob_place");;
		NS::XmlUtil::getAttribute(monsterType.n_FocusSpeed, xmlItem, "n_FocusSpeed");
		NS::XmlUtil::getAttribute(monsterType.n_PartalSpeed, xmlItem, "n_PartalSpeed");
		NS::XmlUtil::getAttribute(monsterType.s_SubFoder, xmlItem, "s_SubFoder");
		NS::XmlUtil::getAttribute(monsterType.s_BPName, xmlItem, "s_BPName");
		NS::XmlUtil::getAttribute(monsterType.n_AttackNum, xmlItem, "n_AttackNum");
		NS::XmlUtil::getAttribute(monsterType.n_AttackSkill, xmlItem, "n_AttackSkill");
		NS::XmlUtil::getAttribute(monsterType.n_AttackDis, xmlItem, "n_AttackDis");
		NS::XmlUtil::getAttribute(monsterType.n_AtkSight, xmlItem, "n_AtkSight");
		NS::XmlUtil::getAttribute(monsterType.n_LoseSight, xmlItem, "n_LoseSight");
		NS::XmlUtil::getAttribute(monsterType.n_AtkAngle, xmlItem, "n_AtkAngle");
		for (int i = 0; i < 4; i++)
		{
			std::string str = fmt::format("n_SkillID{0}", i + 1);
			int nSkillId{};
			NS::XmlUtil::getAttribute(nSkillId, xmlItem, str.c_str());
			if (nSkillId == 0) {
				continue;
			}
			MonsterType::SKILL_CON con;
			con.nSkillId = nSkillId;
			str = fmt::format("n_ConTpy{0}", i + 1);
			NS::XmlUtil::getAttribute(con.nConTpy, xmlItem, str.c_str());
			str = fmt::format("n_ConValue{0}", i + 1);
			NS::XmlUtil::getAttribute(con.nConValue, xmlItem, str.c_str());
			str = fmt::format("n_ConRate{0}", i + 1);
			NS::XmlUtil::getAttribute(con.nConRate, xmlItem, str.c_str());
			str = fmt::format("n_AniIdx{0}", i + 1);
			NS::XmlUtil::getAttribute(con.nAniIdx, xmlItem, str.c_str());
			monsterType.vecSkillCon.push_back(con);
		}
		NS::XmlUtil::getAttribute(monsterType.n_AI, xmlItem, "n_AI");

		NS::XmlUtil::getAttribute(monsterType.strTxtFonder, xmlItem, "s_TxtFonder");
		for (int i = 0; i < 6; i++)
		{
			std::string str = fmt::format("s_Txt{0}", i + 1);
			std::string strInfo;
			NS::XmlUtil::getAttribute(strInfo, xmlItem, str.c_str());
			if (strInfo.empty()) {
				continue;
			}
			monsterType.vecTxtures.push_back(strInfo);
		}
		NS::XmlUtil::getAttribute(monsterType.s_desc, xmlItem, "s_desc");

		NS::XmlUtil::getAttribute(monsterType.s_atk_sound, xmlItem, "s_atk_sound");
		NS::XmlUtil::getAttribute(monsterType.n_atk_tri, xmlItem, "n_atk_tri");
		NS::XmlUtil::getAttribute(monsterType.s_hit_sound, xmlItem, "s_hit_sound");
		NS::XmlUtil::getAttribute(monsterType.n_hit_tri, xmlItem, "n_hit_tri");
		NS::XmlUtil::getAttribute(monsterType.s_dead_sound, xmlItem, "s_dead_sound");
		NS::XmlUtil::getAttribute(monsterType.n_dead_tri, xmlItem, "n_dead_tri");

		m_monsterTypeMap.insert(std::make_pair(monsterType.n_ID, monsterType));
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

void MonsterCfg::MonsterType::getAttr(MonsterCfgType cfgType, AttrMap& mapData, int level) const
{
	using namespace MsgPB;
	mapData.clear();
	mapData[ROLE_EFFECT_AD] = static_cast<float>(n_AD);
	mapData[ROLE_EFFECT_AP] = static_cast<float>(n_AP);
	mapData[ROLE_EFFECT_ARM] = static_cast<float>(n_ARM);
	mapData[ROLE_EFFECT_MARM] = static_cast<float>(n_MARM);
	mapData[ROLE_EFFECT_AR] = static_cast<float>(n_AR);
	mapData[ROLE_EFFECT_HPM] = static_cast<float>(n_HPM);
	mapData[ROLE_EFFECT_MPM] = static_cast<float>(n_MPM);
	mapData[ROLE_EFFECT_HPR] = static_cast<float>(n_HPR);
	mapData[ROLE_EFFECT_HPRS] = static_cast<float>(n_HPRS);
	mapData[ROLE_EFFECT_MPR] = static_cast<float>(n_MPR);
	mapData[ROLE_EFFECT_MPRS] = static_cast<float>(n_MPRS);
	mapData[ROLE_EFFECT_MR_FIRE] = static_cast<float>(n_MR_Fire);
	mapData[ROLE_EFFECT_MR_FORST] = static_cast<float>(n_MR_Forst);
	mapData[ROLE_EFFECT_MR_THUNDER] = static_cast<float>(n_MR_Thunder);
	mapData[ROLE_EFFECT_MR_EARTH] = static_cast<float>(n_MR_Earth);
	mapData[ROLE_EFFECT_MR_POSION] = static_cast<float>(n_MR_Poison);
	mapData[ROLE_EFFECT_MR_LIGHT] = static_cast<float>(n_MR_Light);
	mapData[ROLE_EFFECT_MR_DARK] = static_cast<float>(n_MR_Dark);
	mapData[ROLE_EFFECT_ARP] = f_ARP;
	mapData[ROLE_EFFECT_MRP] = f_MRP;
	mapData[ROLE_EFFECT_CRI] = f_CRI;
	mapData[ROLE_EFFECT_MCRI] = f_MCRI;
	mapData[ROLE_EFFECT_CRID] = f_CRID;
	mapData[ROLE_EFFECT_MCRID] = f_MCRID;
	mapData[ROLE_EFFECT_RES] = f_RES;
	mapData[ROLE_EFFECT_MRES] = f_MRES;
	mapData[ROLE_EFFECT_CC_HIT] = static_cast<float>(n_CC_Hit);
	mapData[ROLE_EFFECT_CC_RES] = static_cast<float>(n_CC_Res);
	mapData[ROLE_EFFECT_ATK_SP] = f_atk_speed_modifiy;
	mapData[ROLE_EFFECT_MV_SP] = f_Move;
	mapData[ROLE_EFFECT_CHASE] = f_Run;
	//if (cfgType == MonsterCfgType::MonsterCfgBadgeNpc)
	//{
	//	auto cfg = NpcLevelCfg::getInstance().getAttr(level);
	//	if (!cfg) return;
	//	mapData[ROLE_EFFECT_AD] = static_cast<float>(cfg->ad);
	//	mapData[ROLE_EFFECT_AP] = static_cast<float>(cfg->ap);
	//	mapData[ROLE_EFFECT_HPM] = static_cast<float>(cfg->hp);
	//	mapData[ROLE_EFFECT_MPM] = static_cast<float>(cfg->mp);
	//	mapData[ROLE_EFFECT_ARM] = static_cast<float>(cfg->def);
	//	mapData[ROLE_EFFECT_MARM] = static_cast<float>(cfg->mr);
	//	mapData[ROLE_EFFECT_HPR] = static_cast<float>(cfg->hpr);
	//	mapData[ROLE_EFFECT_MPR] = static_cast<float>(cfg->mpr);
	//}

	//if (cfgType == MonsterCfgType::MonsterCfgSoul)
	//{
	//	auto cfg = SoulLevCfg::getInstance().findDef(level);
	//	if (!cfg) return;
	//	mapData[ROLE_EFFECT_AD] = static_cast<float>(cfg->n_AD);
	//	mapData[ROLE_EFFECT_AP] = static_cast<float>(cfg->n_AP);
	//	mapData[ROLE_EFFECT_HPM] = static_cast<float>(cfg->n_HP);
	//	mapData[ROLE_EFFECT_MPM] = static_cast<float>(cfg->n_MP);
	//	mapData[ROLE_EFFECT_ARM] = static_cast<float>(cfg->n_DEF);
	//	mapData[ROLE_EFFECT_MARM] = static_cast<float>(cfg->n_MR);
	//	mapData[ROLE_EFFECT_HPR] = static_cast<float>(cfg->n_HPR);
	//	mapData[ROLE_EFFECT_MPR] = static_cast<float>(cfg->n_MPR);
	//	auto tCfg = SoulTypeCfg::getInstance().findDefTpy(n_ID);
	//	if (!tCfg) return;
	//	mapData[ROLE_EFFECT_ATK_SP] = tCfg->f_speed;
	//}

}


std::string MonsterCfg::GetBPMonsterPath(int id)
{
	auto&& pCfg = findMonsterType(id);
	if (!pCfg || pCfg->s_SubFoder.empty() || pCfg->s_BPName.empty())
	{
		return "";
	}
	return fmt::format("Blueprint'{0}/{1}/{2}.{3}_C'", m_strPath, pCfg->s_SubFoder, pCfg->s_BPName, pCfg->s_BPName);
}

std::string MonsterCfg::GetMonsterMatTxt(int id, std::string strTxt)
{
	auto&& pCfg = findMonsterType(id);
	if (!pCfg || pCfg->s_SubFoder.empty() || pCfg->s_BPName.empty())
	{
		return "";
	}
	if (pCfg->strTxtFonder.empty())
	{
		return fmt::format("Texture2D'{0}/{1}/{2}.{3}'", m_strPath, pCfg->s_SubFoder, strTxt, strTxt);
	}
	else {
		return fmt::format("Texture2D'{0}/{1}/{2}/{3}.{4}'", m_strPath, pCfg->s_SubFoder, pCfg->strTxtFonder, strTxt, strTxt);
	}
}

std::string MonsterCfg::GetAttackPath(int id, int nIdx)
{
	auto&& pCfg = findMonsterType(id);
	if (!pCfg || pCfg->s_SubFoder.empty())
	{
		return "";
	}
	std::string strAni = fmt::format("Attack_{0:02d}_Montage.Attack_{1:02d}_Montage", nIdx + 1, nIdx + 1);
	return fmt::format("AnimMontage'{0}/{1}/{2}'", m_strPath, pCfg->s_SubFoder, strAni);
}

std::string MonsterCfg::GetMagicAttackPath(int id, int nIdx)
{
	auto&& pCfg = findMonsterType(id);
	if (!pCfg || pCfg->s_SubFoder.empty())
	{
		return "";
	}
	std::string strAni = fmt::format("s_attack_{0:02d}_Montage.s_attack_{1:02d}_Montage", nIdx + 1, nIdx + 1);
	return fmt::format("AnimMontage'{0}/{1}/{2}'", m_strPath, pCfg->s_SubFoder, strAni);
}

std::string MonsterCfg::GetDeathPath(int id)
{
	auto&& pCfg = findMonsterType(id);
	if (!pCfg || pCfg->s_SubFoder.empty())
	{
		return "";
	}
	return fmt::format("AnimMontage'{0}/{1}/{2}.{3}'", m_strPath, pCfg->s_SubFoder, m_strDeathName, m_strDeathName);
}

std::string MonsterCfg::GetHitPath(int id)
{
	auto&& pCfg = findMonsterType(id);
	if (!pCfg || pCfg->s_SubFoder.empty())
	{
		return "";
	}
	return fmt::format("AnimMontage'{0}/{1}/{2}.{3}'", m_strPath, pCfg->s_SubFoder, m_strHitName, m_strHitName);
}


