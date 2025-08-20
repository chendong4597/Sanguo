/*!
 * \file SandboxConfigInitializer.cpp
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 05/10/2016
 *
 *
 */
#include "ConfigInitializer.h"
#include "config/GlobalCfg.h"
#include "config/WeaponTypeCfg.h"
#include "config/WeaponAniEffectCfg.h"
#include "config/NpcTypeCfg.h"
#include "config/RoleCreateCfg.h"
#include "config/EquipTypeCfg.h"
#include "config/WeaponInfoTypeCfg.h"
#include "config/SkillTypeCfg.h"
#include "config/BuffTypeCfg.h"
#include "config/DefenceInfoTypeCfg.h"
#include "config/BehaviorTreeTypeCfg.h"
#include "config/SkillFrontEffectCfg.h"
#include "config/DefencePosTpyCfg.h"
#include "config/WalkNpcCfg.h"
#include "config/ItemCfg.h"
#include "config/BuildingCfg.h"
#include "config/PlantCfg.h"
#include "config/MonsterCfg.h"
#include "config/BuffCfg.h"
#include "config/ArmorCfg.h"
#include "config/PlayerAttrCfg.h"
#include "config/PlayerAttrNameCfg.h"
#include "config/SurvivalEventCfg.h"
#include "config/SkillCfg.h"
#include "config/BuffGroupRuleCfg.h"
#include "config/BuildRoomCfg.h"
#include "config/FieldAdventureCfg.h"
#include "config/NpcActiveCfg.h"
#include "config/NpcBadgeCfg.h"
#include "config/TowerBuildingCfg.h"
#include "config/AchieveCfg.h"
#include "config/CraftCfg.h"
#include "config/LocalizationCfg.h"
#include "config/HouseCfg.h"
#include "config/MapSettingCfg.h"
#include "config/TroopsCfg.h"
#include "config/StoreCfg.h"
#include "config/DropCfg.h"
#include "config/MedicamentCfg.h"
#include "config/HelpCfg.h"
#include "i18n/Localization.h"
#include "../gm/GmCfg.h"
#include <string>
#include "stdafx.h"


using namespace NS;


bool ConfigInitializer::initialize()
{
	if (!LoadConfig()) return false;
	return true;
}


bool ConfigInitializer::initializeAfterRevisionUpdate()
{

    return true;
}

bool ConfigInitializer::terminate()
{
	
	return true;
}

bool ConfigInitializer::loadLanguage()
{
	const LocalizationCfg::LanguageAttr* languageAttr = LocalizationCfg::getInstance().getCurrLanguage();
	if (!languageAttr) return false;

	NS::tstring strFile = languageAttr->m_moPath;

	//LOGI("loadLanguage ");
	return Localization::getInstance().initialize(strFile);
}

bool ConfigInitializer::ReloadConfig()
{
	UnloadConfig();
	return LoadConfig();
}

bool ConfigInitializer::LoadConfig()
{
	if (!GlobalCfg::getInstance().initializeCfg()) return false;
	if (!WeaponTypeCfg::getInstance().initializeCfg()) return false;
	if (!WeaponAniEffectCfg::getInstance().initializeCfg()) return false;
	if (!ItemCfg::getInstance().initializeCfg()) return false;
	if (!NpcTypeCfg::getInstance().initializeCfg()) return false;
	if (!RoleCreateCfg::getInstance().initializeCfg()) return false;
	if (!EquipTypeCfg::getInstance().initializeCfg()) return false;
	if (!WeaponInfoTypeCfg::getInstance().initializeCfg())return false;
	if (!SkillTypeCfg::getInstance().initializeCfg())return false;
	if (!BuffTypeCfg::getInstance().initializeCfg())return false;
	if (!DefenceInfoTypeCfg::getInstance().initializeCfg())return false;
	if (!BehaviorTreeTypeCfg::getInstance().initializeCfg()) return false;
	if (!SkillFrontEffectCfg::getInstance().initializeCfg()) return false;
	if (!DefencePosTpyCfg::getInstance().initializeCfg()) return false;
	if (!WalkNpcCfg::getInstance().initializeCfg()) return false;
	if (!GmCfg::getInstance().initialize()) return false;
	if (!BuildingCfg::getInstance().initializeCfg()) return false;
	if (!PlantCfg::getInstance().initializeCfg()) return false;
	if (!MonsterCfg::getInstance().initializeCfg()) return false;
	if (!ArmorCfg::getInstance().initialize()) return false;
	if (!BuffCfg::getInstance().initializeCfg()) return false;
	if (!PlayerAttrCfg::getInstance().initialize()) return false;
	if (!PlayerAttrNameCfg::getInstance().initialize()) return false;
	if (!SurvivalEventCfg::getInstance().initialize()) return false;
	if (!SkillCfg::getInstance().initialize()) return false;
	if (!BuffGroupRuleCfg::getInstance().initialize()) return false;
	if (!BuildRoomCfg::getInstance().initialize()) return false;
	if (!FieldAdventureCfg::getInstance().initialize()) return false;
	if (!NpcActiveCfg::getInstance().initialize()) return false;
	if (!NpcBadgeCfg::getInstance().initialize()) return false;
	if (!TowerBuildingCfg::getInstance().initialize()) return false;
	if (!AchieveCfg::getInstance().initialize()) return false;
	if (!CraftCfg::getInstance().initialize()) return false;
	if (!HouseCfg::getInstance().initialize()) return false;
	if (!MapSettingCfg::getInstance().initialize()) return false;
	if (!TroopsCfg::getInstance().initializeCfg()) return false;
	if (!StoreCfg::getInstance().initialize())return false;
	if (!DropCfg::getInstance().initialize())return false;
	if (!MedicamentCfg::getInstance().initialize()) return false;
	if (!HelpCfg::getInstance().initialize()) return false;
	return true;
}
bool ConfigInitializer::UnloadConfig()
{
	GlobalCfg::getInstance().terminate();
	WeaponTypeCfg::getInstance().terminate();
	WeaponAniEffectCfg::getInstance().terminate();
	ItemCfg::getInstance().terminate();
	NpcTypeCfg::getInstance().terminate();
	RoleCreateCfg::getInstance().terminate();
	EquipTypeCfg::getInstance().terminate();
	WeaponInfoTypeCfg::getInstance().terminate();
	SkillTypeCfg::getInstance().terminate();
	BuffTypeCfg::getInstance().terminate();
	DefenceInfoTypeCfg::getInstance().terminate();
	BehaviorTreeTypeCfg::getInstance().terminate();
	SkillFrontEffectCfg::getInstance().terminate();
	DefencePosTpyCfg::getInstance().terminate();
	WalkNpcCfg::getInstance().terminate();
	GmCfg::getInstance().terminate();
	BuildingCfg::getInstance().terminate();
	PlantCfg::getInstance().terminate();
	MonsterCfg::getInstance().terminate();
	ArmorCfg::getInstance().terminate();
	BuffCfg::getInstance().terminate();
	PlayerAttrCfg::getInstance().terminate();
	PlayerAttrNameCfg::getInstance().terminate();
	SurvivalEventCfg::getInstance().terminate();
	SkillCfg::getInstance().terminate();
	BuffGroupRuleCfg::getInstance().terminate();
	BuildRoomCfg::getInstance().terminate();
	FieldAdventureCfg::getInstance().terminate();
	NpcActiveCfg::getInstance().terminate();
	NpcBadgeCfg::getInstance().terminate();
	TowerBuildingCfg::getInstance().terminate();
	AchieveCfg::getInstance().terminate();
	CraftCfg::getInstance().terminate();
	HouseCfg::getInstance().terminate();
	MapSettingCfg::getInstance().terminate();
	GmCfg::getInstance().terminate();
	TroopsCfg::getInstance().terminate();
	DropCfg::getInstance().terminate();
	MedicamentCfg::getInstance().terminate();
	HelpCfg::getInstance().terminate();
	return true;
}

