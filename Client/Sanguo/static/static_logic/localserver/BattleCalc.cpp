/*!
* \file BattleCalc.cpp
*
* \author zhichenxia
* \date 2018/6/23
*
*
*/
#include "stdafx.h"
#include "BattleCalc.h"
#include <config/SkillCfg.h>


// 取得目標的免復值
int SkillEffect_ElemtDmg::GetTargetElemetRemitValue(const SkillRoleValue & TargetRoleValue)
{
	auto pType = SkillCfg::getInstance().findSkillType(m_skill_id);
	if (!pType) return 0;

	for (auto i = 0; i < SkillType::MAX_EFFECTS; ++i)
	{
		switch (pType->findEffect(i)->m_type)
		{
		case SkillType::Effect::Type::PhysicalDamage:
			return TargetRoleValue.NowAR;

		case SkillType::Effect::Type::FireDamage:
			return TargetRoleValue.NowMR_FIRE;

		case SkillType::Effect::Type::IceDamage:
			return TargetRoleValue.NowMR_FORST;

		case SkillType::Effect::Type::ThunderDamage:
			return TargetRoleValue.NowMR_THUNDER;

		case SkillType::Effect::Type::EarthDamage:
			return TargetRoleValue.NowMR_EARTH;

		case SkillType::Effect::Type::PoisonDamage:
			return TargetRoleValue.NowMR_POSION;

		case SkillType::Effect::Type::LightDamage:
			return TargetRoleValue.NowMR_LIGHT;

		case SkillType::Effect::Type::DarkDamage:
			return TargetRoleValue.NowMR_DARK;

		default:
			break;
		}
	}
	return 0;
}
