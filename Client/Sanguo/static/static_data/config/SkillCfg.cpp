/*!
 * \file SkillCfg.cpp
 *
 * \author zhijieli
 * \date 2016/08/30
 *
 *
 */
#include "stdafx.h"
#include "SkillCfg.h"
#include "../common/calculate.h"
#include "XmlUtil.h"
#include "BuffCfg.h"
#include <StringUtil.h>

using namespace NS;

const float SkillCfg::AURA_FOV_RANGE{ 20.0f };

SkillType::~SkillType()
{

}

const SkillType::Effect* SkillType::findEffect(int index) const
{
    if (index < 0 || index >= MAX_EFFECTS) return nullptr;
    return &m_effects[index];
}

bool SkillType::IsPassivesSkill() const
{	
	return m_skillCastType == 2 ? true : false;
}

bool SkillType::IsActivesSkill() const
{
	if (m_skillCastType == 1)
	{
		return true;
	}
	if (IsAiSpecialSkill())
	{
		return true;
	}
	return false;
}

// AI使用的特殊技能
bool SkillType::IsAiSpecialSkill() const
{
	return m_skillCastType == 3 ? true : false;
}

bool SkillType::isTriggerBeforeDmg() const
{
    return m_triggerType == SkillType::TriggerType::TriggerTypeBeforeHurt;
}

bool SkillType::isTriggerAfterDmg() const
{
    return ((SkillType::TriggerType::TriggerTypeUnderAttackRemoveBuff
        | SkillType::TriggerType::TriggerTypeUnderAttackCastSkill
        | SkillType::TriggerType::TriggerTypeUnderAttackFightBack
        | SkillType::TriggerType::TriggerTypeUnderSkillRemoveBuff
        | SkillType::TriggerType::TriggerTypeUnderSkillCastSkill
        | SkillType::TriggerType::TriggerTypeUnderSkillFightBack
        | SkillType::TriggerType::TriggerTypeAttackBloodSucking
        | SkillType::TriggerType::TriggerTypeAttackCastSkill
        | SkillType::TriggerType::TriggerTypeCastingBloodSucking
        | SkillType::TriggerType::TriggerTypeCastingCastSkill) & m_triggerType) != 0;
}

bool SkillType::IsHealthRecovery() const
{
	for (const auto& effect : m_effects)
	{
		if (effect.m_type != Effect::Type::HealthRecovery)
		{
			return true;
		}
	}

	return false;
}

bool SkillType::IsRestoreHP()const
{
	for (const auto& effect : m_effects)
	{
		if (Effect::Type::None == effect.m_type) continue;
		if (effect.m_type != Effect::Type::HealthRecovery && effect.m_type != Effect::Type::AddBuff) return false;

		if (effect.m_type == Effect::Type::AddBuff)
		{
            auto& attrRef = effect.m_effectAttrRefs[0];
			auto pBuffCfg = BuffCfg::getInstance().findBuffType(attrRef.m_effectValue);
			if (!pBuffCfg) return false;

			for (const auto& buffEffect : pBuffCfg->effects)
			{
				auto buffEffectType = buffEffect.type;

				if (BuffType::CLI_BUFF_EFFECT_NULL == (BuffType::BUFF_EFFECT_TYPE)buffEffectType) continue;
				if ((BuffType::BUFF_EFFECT_TYPE)buffEffectType != BuffType::CLI_BUFF_EFFECT_HP_RESTORE) return false;
			}
		}
	}

	return true;
}

bool SkillType::isDamageSkill() const
{
	for (const auto& effect : m_effects)
	{
		if (Effect::Type::None == effect.m_type) continue;
		if (effect.m_type == Effect::Type::TrueDamage
			|| effect.m_type == Effect::Type::PhysicalDamage
			|| effect.m_type == Effect::Type::FireDamage
			|| effect.m_type == Effect::Type::IceDamage
			|| effect.m_type == Effect::Type::ThunderDamage
			|| effect.m_type == Effect::Type::EarthDamage
			|| effect.m_type == Effect::Type::PoisonDamage
			|| effect.m_type == Effect::Type::LightDamage
			|| effect.m_type == Effect::Type::HurtAndSucking
			)
		{
			return true;
		}
	}

	return false;
}


SkillCfg& SkillCfg::getInstance()
{
	static SkillCfg s_SkillCfg;
	return s_SkillCfg;
}


bool SkillCfg::initialize()
{
	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/Skill_config.bin")) return false;
	parseSkillConfigConfig(doc.RootElement());

	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/SkillReplaceMap.bin")) return false;
	parseSkillReplaceConfig(doc.RootElement());


	reloadMultiLanguage();
	return true;
}


void SkillCfg::terminate()
{
	m_skillTypeMap.clear();
	m_skillReplaceMap.clear();
}


void SkillCfg::reloadMultiLanguage()
{
    for (auto& pr : m_skillTypeMap)
    {
        auto& skillType = pr.second;

        skillType.m_name = _LC(skillType.m_name_untrans);
        skillType.m_desc = calcluate::getMedianValueName(skillType.m_desc_untrans, [&, skillType](int index)
        {
            return static_cast<int>(skillType.m_value[index]);
        });
    }
}

const SkillType* SkillCfg::findSkillType(int32 id) const
{
	auto it = m_skillTypeMap.find(id);
	return it == m_skillTypeMap.end() ? nullptr : &it->second;
}

const SkillType* SkillCfg::findSkillTypeByTempId(NS::int32 id) const
{
	auto iter = m_skillTempTypeMap.find(id);
	if (iter == m_skillTempTypeMap.end())
	{
		return nullptr;
	}
	return findSkillType(iter->second);
}

bool SkillCfg::parseSkillConfigConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		SkillType skillType{};

        NS::XmlUtil::getAttribute(skillType.m_id, xmlItem, "n_ID");// = proto.n_id();
		NS::XmlUtil::getAttribute(skillType.m_tempSkillId, xmlItem, "n_t_skillid");// = proto.n_id();
        NS::XmlUtil::getAttribute(skillType.m_lv, xmlItem, "n_lv");// = proto.n_lv();
        NS::XmlUtil::getAttribute(skillType.m_name_untrans, xmlItem, "s_name");// = proto.s_name();
        NS::XmlUtil::getAttribute(skillType.m_desc_untrans, xmlItem, "s_desc");// = proto.s_desc();
        NS::XmlUtil::getAttribute(skillType.m_skillIcon, xmlItem, "s_skill_icon");// = proto.s_skill_icon();
        NS::XmlUtil::getAttribute(skillType.m_skillAction, xmlItem, "s_skill_action");// = proto.s_skill_action();
        NS::XmlUtil::getAttribute(skillType.m_skillJumpAction, xmlItem, "s_skill_jumpaction");// = proto.s_skill_jumpaction();
        NS::XmlUtil::getAttribute(skillType.m_skillDirection, xmlItem, "s_skill_direction");// = proto.s_skill_direction();
        NS::XmlUtil::getAttribute(skillType.m_skillType, xmlItem, "n_skill_type");// = (SkillType::Type)proto.n_skill_type();
        NS::XmlUtil::getAttribute(skillType.m_castType, xmlItem, "n_click_type");// = proto.n_click_type();
        NS::XmlUtil::getAttribute(skillType.m_skillCastType, xmlItem, "n_skill_cast_type");// = proto.n_skill_cast_type();
        NS::XmlUtil::getAttribute(skillType.m_costItem, xmlItem, "n_cost_item");// = proto.n_cost_item();
        NS::XmlUtil::getAttribute(skillType.m_costMp, xmlItem, "n_costmp");// = proto.n_costmp();
		NS::XmlUtil::getAttribute(skillType.m_showcostmp, xmlItem, "n_showcostmp");// = proto.n_showcostmp();
        NS::XmlUtil::getAttribute(skillType.m_cooldown, xmlItem, "n_cooldown");// = proto.n_cooldown();
        NS::XmlUtil::getAttribute(skillType.m_targetType, xmlItem, "n_target_type");//= (SkillType::TargetType)proto.n_target_type();
        NS::XmlUtil::getAttribute(skillType.m_targetCategory, xmlItem, "n_target_category");//= (SkillType::TargetCategory)proto.n_target_category();
        NS::XmlUtil::getAttribute(skillType.m_targetCount, xmlItem, "n_target_count");// = proto.n_target_count();
        NS::XmlUtil::getAttribute(skillType.m_targetArrtType, xmlItem, "n_targetattr_type");// = proto.n_targetattr_type();
        NS::XmlUtil::getAttribute(skillType.m_logicOperation, xmlItem, "n_logic");// = proto.n_logic();
        NS::XmlUtil::getAttribute(skillType.m_logicValue, xmlItem, "n_logic_num");// = proto.n_logic_num();
        NS::XmlUtil::getAttribute(skillType.m_attkMinRange, xmlItem, "f_attk_min_range");// = proto.f_attk_min_range();
        NS::XmlUtil::getAttribute(skillType.m_attkRange, xmlItem, "f_attk_range");// = proto.f_attk_range();
        NS::XmlUtil::getAttribute(skillType.m_through, xmlItem, "n_through");//= (SkillType::ThroughMode)proto.n_through();
        NS::XmlUtil::getAttribute(skillType.m_effectUseBallSetting, xmlItem, "n_effect_use_ball_setting");// = proto.n_effect_use_ball_setting();

        NS::XmlUtil::getAttribute(skillType.m_ballSize, xmlItem, "f_ball_size");// = proto.f_ball_size();
        NS::XmlUtil::getAttribute(skillType.m_ballSpeed, xmlItem, "f_ball_speed");// = proto.f_ball_speed();
        NS::XmlUtil::getAttribute(skillType.m_ballTime, xmlItem, "f_ball_time");//= proto.f_ball_time();
        NS::XmlUtil::getAttribute(skillType.m_ballAction, xmlItem, "f_ball_action");// = proto.f_ball_action();
        NS::XmlUtil::getAttribute(skillType.m_ballSound, xmlItem, "s_ball_sound");// = proto.s_ball_sound();
		NS::XmlUtil::getAttribute(skillType.m_ball_num_max, xmlItem, "n_ball_num_max");// = proto.n_ball_num_max();

        NS::XmlUtil::getAttribute(skillType.m_areaWidth, xmlItem, "f_area_width");// = proto.f_area_width();
        NS::XmlUtil::getAttribute(skillType.m_areaHeight, xmlItem, "f_area_height");// = proto.f_area_height();
        NS::XmlUtil::getAttribute(skillType.m_areaAliveTime, xmlItem, "n_area_alive_time");// = proto.n_area_alive_time();
        NS::XmlUtil::getAttribute(skillType.m_areaIntervalTime, xmlItem, "n_area_interval_time");// = proto.n_area_interval_time();

        NS::XmlUtil::getAttribute(skillType.m_lineWidth, xmlItem, "f_line_width");// = proto.f_line_width();
        NS::XmlUtil::getAttribute(skillType.m_lineAliveTime, xmlItem, "n_line_alive_time");// = proto.n_line_alive_time();
        NS::XmlUtil::getAttribute(skillType.m_lineIntervalTime, xmlItem, "n_line_interval_time");//= proto.n_line_interval_time();

        NS::XmlUtil::getAttribute(skillType.m_buffEffectDelay, xmlItem, "n_buff_effect_delay");//= proto.n_buff_effect_delay();

        NS::XmlUtil::getAttribute(skillType.m_CastEffectId, xmlItem, "n_skill_cast_effect");//= proto.n_skill_cast_effect();
        NS::XmlUtil::getAttribute(skillType.m_CastEffectSound, xmlItem, "s_skill_cast_sound");// = proto.s_skill_cast_sound();
        NS::XmlUtil::getAttribute(skillType.m_FlyEffectId, xmlItem, "n_skill_effect");// = proto.n_skill_effect();

        NS::XmlUtil::getAttribute(skillType.m_scriptPath, xmlItem, "s_script_path");// = proto.s_script_path();
        NS::XmlUtil::getAttribute(skillType.m_scriptParam, xmlItem, "s_script_param");// = proto.s_script_param();
        NS::XmlUtil::getAttribute(skillType.m_display_atTip, xmlItem, "n_display_armor");// = proto.n_display_armor();
        NS::XmlUtil::getAttribute(skillType.m_singingTime, xmlItem, "n_singing_time");// = proto.n_singing_time();
        NS::XmlUtil::getAttribute(skillType.m_skillInCD, xmlItem, "n_skillincd");//= proto.n_skillincd();
        NS::XmlUtil::getAttribute(skillType.m_ballOffsetX, xmlItem, "f_ball_offset_x");// = proto.f_ball_offset_x();
        NS::XmlUtil::getAttribute(skillType.m_ballOffsetY, xmlItem, "f_ball_offset_y");// = proto.f_ball_offset_y();
        NS::XmlUtil::getAttribute(skillType.m_triggerType, xmlItem, "n_trigger_type");// = proto.n_trigger_type();
        NS::XmlUtil::getAttribute(skillType.m_triggerParam1, xmlItem, "n_trigger_param1");// = proto.n_trigger_param1();
        NS::XmlUtil::getAttribute(skillType.m_triggerParam2, xmlItem, "n_trigger_param2");//= proto.n_trigger_param2();
        NS::XmlUtil::getAttribute(skillType.m_triggerParam3, xmlItem, "n_trigger_param3");// = proto.n_trigger_param3();
        NS::XmlUtil::getAttribute(skillType.m_hitEffectOffsetX, xmlItem, "n_hiteffect_offsetx");// = static_cast<float>(proto.n_hiteffect_offsetx());
        NS::XmlUtil::getAttribute(skillType.m_hitEffectOffsetY, xmlItem, "n_hiteffect_offsety");// = static_cast<float>(proto.n_hiteffect_offsety());
        NS::XmlUtil::getAttribute(skillType.m_effect_env, xmlItem, "n_effect_env");//= proto.n_effect_env();

        NS::XmlUtil::getAttribute(skillType.m_value[0], xmlItem, "f_value_0");// = proto.f_value_0();
        NS::XmlUtil::getAttribute(skillType.m_value[1], xmlItem, "f_value_1");// = proto.f_value_1();
        NS::XmlUtil::getAttribute(skillType.m_value[2], xmlItem, "f_value_2");// = proto.f_value_2();
        NS::XmlUtil::getAttribute(skillType.m_value[3], xmlItem, "f_value_3");// = proto.f_value_3();
        NS::XmlUtil::getAttribute(skillType.m_value[4], xmlItem, "f_value_4");// = proto.f_value_4();

		//m_hitEffectOffsetX, m_hitEffectOffsetY 为世界坐标*100
		skillType.m_hitEffectOffsetX /= 100.0f;
		skillType.m_hitEffectOffsetY /= 100.0f;

		for (int i = 0; i < 3; i++)
		{
			NS::XmlUtil::getAttribute(skillType.m_effects[i].m_type, xmlItem, fmt::format("n_effect_type_{0}", i + 1).c_str());// = proto.f_value_0();
			if (skillType.m_effects[i].m_type == SkillType::Effect::Type::Summon || 
				skillType.m_effects[i].m_type == SkillType::Effect::Type::SummonTarget || 
				skillType.m_effects[i].m_type == SkillType::Effect::Type::SummonTargetPos)
		    {
				skillType.m_bSummon = true;
			}
			NS::XmlUtil::getAttribute(skillType.m_effects[i].m_range, xmlItem, fmt::format("n_effect_range_{0}", i + 1).c_str());// = proto.n_effect_range_1();
			NS::XmlUtil::getAttribute(skillType.m_effects[i].m_targetType, xmlItem, fmt::format("n_effect_target_{0}", i + 1).c_str());// = (SkillType::TargetType)proto.n_effect_target_1();
			NS::XmlUtil::getAttribute(skillType.m_effects[i].m_targetCategory, xmlItem, fmt::format("proto.n_effect_category_{0}", i + 1).c_str());// = (SkillType::TargetCategory)proto.n_effect_category_1();
			NS::XmlUtil::getAttribute(skillType.m_effects[i].m_targetCount, xmlItem, fmt::format("n_effect_targetcount_{0}", i + 1).c_str());// = proto.n_effect_targetcount_1();
			NS::XmlUtil::getAttribute(skillType.m_effects[i].m_HitEffectId, xmlItem, fmt::format("n_skill_hiteffect_{0}", i + 1).c_str());// = proto.n_skill_hiteffect_1();
			NS::XmlUtil::getAttribute(skillType.m_effects[i].m_hitpercent, xmlItem, fmt::format("n_skill_hitpercent_{0}", i + 1).c_str());// = proto.n_skill_hitpercent_1();
			NS::XmlUtil::getAttribute(skillType.m_effects[i].m_effectsound, xmlItem, fmt::format("s_effect_sound_{0}", i + 1).c_str());// = proto.s_effect_sound_1();

			std::string strMobs;
			NS::XmlUtil::getAttribute(strMobs, xmlItem, fmt::format("s_effect_mobid{0}", i + 1).c_str());
			StringUtil::splitString(skillType.m_effects[i].m_mobid, strMobs, ",");

			NS::XmlUtil::getAttribute(skillType.m_effects[i].m_effectAttrRefs[0].m_effectAttrref, xmlItem, fmt::format("n_effect_attrref1_{0}", i + 1).c_str());// = proto.n_effect_attrref1_1();
			NS::XmlUtil::getAttribute(skillType.m_effects[i].m_effectAttrRefs[0].m_effectValue, xmlItem, fmt::format("n_effect_value1_{0}", i + 1).c_str());// = proto.n_effect_value1_1();
			NS::XmlUtil::getAttribute(skillType.m_effects[i].m_effectAttrRefs[1].m_effectAttrref, xmlItem, fmt::format("n_effect_attrref2_{0}", i + 1).c_str());// = proto.n_effect_attrref2_1();
			NS::XmlUtil::getAttribute(skillType.m_effects[i].m_effectAttrRefs[1].m_effectValue, xmlItem, fmt::format("n_effect_value2_{0}", i + 1).c_str());// = proto.n_effect_value2_1();
		}

		skillType.m_effectWhenApplyBuffToTarget.clear();
		std::string s_effect_when_apply_buff;
		NS::XmlUtil::getAttribute(s_effect_when_apply_buff, xmlItem, "s_effect_when_apply_buff");// = proto.f_value_4();
		std::vector<NS::tstring> groupStrList;
		NS::StringUtil::splitString(groupStrList, s_effect_when_apply_buff, ",");

        for (decltype(groupStrList.size()) i = 0; i < groupStrList.size(); ++i)
		{
			NS::int32 buffId{};
			NS::int32 effectId{};
			NS::int32 count = sscanf(groupStrList[i].c_str(), "[%d;%d]", &buffId, &effectId);
			if (count != 2) continue;

            skillType.m_effectWhenApplyBuffToTarget.emplace(buffId, effectId);
		}

		NS::XmlUtil::getAttribute(skillType.m_unlock, xmlItem, "n_ID");// = proto.n_unlock();
		m_skillTypeMap[skillType.m_id] = skillType;
		if (skillType.m_tempSkillId != 0) {
			m_skillTempTypeMap[skillType.m_tempSkillId] = skillType.m_id;
		}
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

bool SkillCfg::parseSkillReplaceConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if(!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		int nCosSkillId;
		NS::XmlUtil::getAttribute(nCosSkillId, xmlItem, "n_CosSkillId");
		SkillReplaceMap::iterator it = m_skillReplaceMap.find(nCosSkillId);
		if (it == m_skillReplaceMap.end())
		{
			std::map<int, int> mValue;
			m_skillReplaceMap.insert(std::make_pair(nCosSkillId, mValue));
			it = m_skillReplaceMap.find(nCosSkillId);
		}

		int nOldSkill{};// = proto.n_oldskillid();
		int nNewSkill{};// = proto.n_newskillid();
		NS::XmlUtil::getAttribute(nOldSkill, xmlItem, "n_OldSkillId");
		NS::XmlUtil::getAttribute(nNewSkill, xmlItem, "n_NewSkillId");
		it->second.insert(std::make_pair(nOldSkill, nNewSkill));
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

bool SkillCfg::findReplaceSkill(int cosSkillId, int oldSkillId, int& newSkillId)
{
	SkillReplaceMap::const_iterator it = m_skillReplaceMap.find(cosSkillId);
	if (it == m_skillReplaceMap.end())
	{
		newSkillId = oldSkillId;
		return false;
	}
	std::map<int, int>::const_iterator cit = it->second.find(oldSkillId);
	if (cit == it->second.end())
	{
		newSkillId = oldSkillId;
		return false;
	}
	newSkillId = cit->second;
	return true;
}

