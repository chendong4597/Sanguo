/*!
 * \file TowerBuildingCfg.cpp
 *
 * \author xiaobowang;
 * \date 2017/07/07
 *
 *
 */
#include "stdafx.h"
#include "TowerBuildingCfg.h"
#include "MonsterCfg.h"
#include "BuildingCfg.h"
#include "XmlUtil.h"
#include <StringUtil.h>

//static const NS::uint UPGRADE_STUFF_NUM = 3;
//static const NS::uint EVOLVE_STUFF_NUM = 3;

TowerBuildingCfg::~TowerBuildingCfg()
{
	this->terminate();
}

bool TowerBuildingCfg::initialize()
{
	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/BuildTowerDefence.bin")) return false;
	parseBuildTowerDefence(doc.RootElement());

	//if (!bzbee::XmlUtil::loadXmlFile(doc, "config/BuildTowerRepair.bin")) return false;
	//parseBuildTowerDefence(doc.RootElement());

	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/BuildTowerUpdate.bin")) return false;
	parseBuildTowerUpdate(doc.RootElement());

	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/BuildTowerReform.bin")) return false;
	parseBuildTowerReform(doc.RootElement());

	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/BuildTowerTemplateAttack.bin")) return false;
	parseBuildTowerTemplateAttack(doc.RootElement());

	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/BuildTowerCrystalBuff.bin")) return false;
	parseBuildTowerCrystalBuff(doc.RootElement());

    reloadMultiLanguage();

	return true;
}
void TowerBuildingCfg::terminate()
{
	m_towerBuildingDefenceMap.clear();
	m_towerBuildingRepairMap.clear();
	m_towerBuildingUpdateMap.clear();
	m_towerBuildingReformMap.clear();
	m_towerBuildTemplateAttackMap.clear();
	//m_towerBuildingShipGunMap.clear();

}

TowerBuildingCfg& TowerBuildingCfg::getInstance()
{
	static TowerBuildingCfg s_cfg;
	return s_cfg;
}

bool TowerBuildingCfg::parseBuildTowerDefence(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	//auto pAttrXml = p->FirstChildElement("Attribute");
	//while (pAttrXml)
	//{
	//	pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	//}

	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{

		TowerBuildingDefence towerBuildingDefence;

		NS::XmlUtil::getAttribute(towerBuildingDefence.m_id, xmlItem, "n_ID");// = proto.n_id();
		NS::XmlUtil::getAttribute(towerBuildingDefence.m_buildingTypeId, xmlItem, "n_build_cfg_type");// = proto.n_build_cfg_type();
		NS::XmlUtil::getAttribute(towerBuildingDefence.m_level, xmlItem, "n_level");// = proto.n_level();
        NS::XmlUtil::getAttribute(towerBuildingDefence.m_name_untrans, xmlItem, "s_name");// = proto.s_name();
        NS::XmlUtil::getAttribute(towerBuildingDefence.m_note_untrans, xmlItem, "s_note");// = proto.s_note();
        NS::XmlUtil::getAttribute(towerBuildingDefence.m_category, xmlItem, "n_category");// = (TowerBuildingCategory)proto.n_category();
        NS::XmlUtil::getAttribute(towerBuildingDefence.m_typeDes_untrans, xmlItem, "s_type_des");// = proto.s_type_des();
		NS::XmlUtil::getAttribute(towerBuildingDefence.m_energy, xmlItem, "n_energy");// = proto.n_energy();
		NS::XmlUtil::getAttribute(towerBuildingDefence.m_power, xmlItem, "n_power");// = proto.n_power();
		NS::XmlUtil::getAttribute(towerBuildingDefence.n_Prosperous, xmlItem, "n_Prosperous");// = proto.n_power();
		NS::XmlUtil::getAttribute(towerBuildingDefence.m_hp, xmlItem, "n_hp");// = proto.n_hp();
		NS::XmlUtil::getAttribute(towerBuildingDefence.m_canAttack, xmlItem, "n_can_attack");// = proto.n_can_attack();
		NS::XmlUtil::getAttribute(towerBuildingDefence.m_hpUiCheck, xmlItem, "f_HpUICheck");// = proto.f_hpuicheck();
		NS::XmlUtil::getAttribute(towerBuildingDefence.m_fov, xmlItem, "n_fov");// = proto.n_fov();
		NS::XmlUtil::getAttribute(towerBuildingDefence.m_forbidden, xmlItem, "n_forbidden");// = proto.n_forbidden();
		NS::XmlUtil::getAttribute(towerBuildingDefence.m_ad, xmlItem, "n_AD");// = proto.n_ad();
		NS::XmlUtil::getAttribute(towerBuildingDefence.m_ap, xmlItem, "n_AP");// = proto.n_ap();
		NS::XmlUtil::getAttribute(towerBuildingDefence.m_skillSchematic, xmlItem, "s_skill_schematic");// = proto.s_skill_schematic();
		NS::XmlUtil::getAttribute(towerBuildingDefence.m_roleLevelLimit, xmlItem, "n_min_role_level");// = proto.n_min_role_level();
		NS::XmlUtil::getAttribute(towerBuildingDefence.n_arm, xmlItem, "n_arm");// = proto.n_arm();
		NS::XmlUtil::getAttribute(towerBuildingDefence.n_marm, xmlItem, "n_marm");// = proto.n_marm();
		NS::XmlUtil::getAttribute(towerBuildingDefence.n_public_cd, xmlItem, "n_public_cd");// = proto.n_public_cd();
		NS::XmlUtil::getAttribute(towerBuildingDefence.n_disapear_die, xmlItem, "n_disapear_die");// = proto.n_disapear_die();
		NS::XmlUtil::getAttribute(towerBuildingDefence.n_BeAttackNumberLimit, xmlItem, "n_beattacknumberlimit");// = proto.n_beattacknumberlimit();

        {
            TowerBuildSkillInfo sku;
			NS::XmlUtil::getAttribute(sku.n_skill, xmlItem, "n_skill_1");// = proto.n_skill_1();
			if (sku.n_skill > 0)
			{
				NS::XmlUtil::getAttribute(sku.n_skill_target_choose_first, xmlItem, "n_skill_target_1");// = proto.n_skill_target_1();
				NS::XmlUtil::getAttribute(sku.n_skill_target_movetype, xmlItem, "n_skill_target_movetype_1");// = proto.n_skill_target_movetype_1();
				NS::XmlUtil::getAttribute(sku.n_skill_cast_event, xmlItem, "n_skill_cast_event_1");// = proto.n_skill_cast_event_1();
				NS::XmlUtil::getAttribute(sku.n_target_type, xmlItem, "n_target_type_1");// = proto.n_target_type_1();
				NS::XmlUtil::getAttribute(sku.n_target_camp, xmlItem, "n_target_camp_1");// = proto.n_target_camp_1();
			}
			
			towerBuildingDefence.m_vSkill.emplace_back(sku);
        }

        {
			TowerBuildSkillInfo sku;
			NS::XmlUtil::getAttribute(sku.n_skill, xmlItem, "n_skill_2");// = proto.n_skill_1();
			if (sku.n_skill > 0)
			{
				NS::XmlUtil::getAttribute(sku.n_skill_target_choose_first, xmlItem, "n_skill_target_2");// = proto.n_skill_target_1();
				NS::XmlUtil::getAttribute(sku.n_skill_target_movetype, xmlItem, "n_skill_target_movetype_2");// = proto.n_skill_target_movetype_1();
				NS::XmlUtil::getAttribute(sku.n_skill_cast_event, xmlItem, "n_skill_cast_event_2");// = proto.n_skill_cast_event_1();
				NS::XmlUtil::getAttribute(sku.n_target_type, xmlItem, "n_target_type_2");// = proto.n_target_type_1();
				NS::XmlUtil::getAttribute(sku.n_target_camp, xmlItem, "n_target_camp_2");// = proto.n_target_camp_1();
			}

			towerBuildingDefence.m_vSkill.emplace_back(sku);
        }

        {
			TowerBuildSkillInfo sku;
			NS::XmlUtil::getAttribute(sku.n_skill, xmlItem, "n_skill_3");// = proto.n_skill_1();
			if (sku.n_skill > 0)
			{
				NS::XmlUtil::getAttribute(sku.n_skill_target_choose_first, xmlItem, "n_skill_target_3");// = proto.n_skill_target_1();
				NS::XmlUtil::getAttribute(sku.n_skill_target_movetype, xmlItem, "n_skill_target_movetype_3");// = proto.n_skill_target_movetype_1();
				NS::XmlUtil::getAttribute(sku.n_skill_cast_event, xmlItem, "n_skill_cast_event_3");// = proto.n_skill_cast_event_1();
				NS::XmlUtil::getAttribute(sku.n_target_type, xmlItem, "n_target_type_3");// = proto.n_target_type_1();
				NS::XmlUtil::getAttribute(sku.n_target_camp, xmlItem, "n_target_camp_3");// = proto.n_target_camp_1();
			}

			towerBuildingDefence.m_vSkill.emplace_back(sku);
        }

        {
			TowerBuildSkillInfo sku;
			NS::XmlUtil::getAttribute(sku.n_skill, xmlItem, "n_skill_4");// = proto.n_skill_1();
			if (sku.n_skill > 0)
			{
				NS::XmlUtil::getAttribute(sku.n_skill_target_choose_first, xmlItem, "n_skill_target_4");// = proto.n_skill_target_1();
				NS::XmlUtil::getAttribute(sku.n_skill_target_movetype, xmlItem, "n_skill_target_movetype_4");// = proto.n_skill_target_movetype_1();
				NS::XmlUtil::getAttribute(sku.n_skill_cast_event, xmlItem, "n_skill_cast_event_4");// = proto.n_skill_cast_event_1();
				NS::XmlUtil::getAttribute(sku.n_target_type, xmlItem, "n_target_type_4");// = proto.n_target_type_1();
				NS::XmlUtil::getAttribute(sku.n_target_camp, xmlItem, "n_target_camp_4");// = proto.n_target_camp_1();
			}

			towerBuildingDefence.m_vSkill.emplace_back(sku);
        }

        {
			TowerBuildSkillInfo sku;
			NS::XmlUtil::getAttribute(sku.n_skill, xmlItem, "n_skill_5");// = proto.n_skill_1();
			if (sku.n_skill > 0)
			{
				NS::XmlUtil::getAttribute(sku.n_skill_target_choose_first, xmlItem, "n_skill_target_5");// = proto.n_skill_target_1();
				NS::XmlUtil::getAttribute(sku.n_skill_target_movetype, xmlItem, "n_skill_target_movetype_5");// = proto.n_skill_target_movetype_1();
				NS::XmlUtil::getAttribute(sku.n_skill_cast_event, xmlItem, "n_skill_cast_event_5");// = proto.n_skill_cast_event_1();
				NS::XmlUtil::getAttribute(sku.n_target_type, xmlItem, "n_target_type_5");// = proto.n_target_type_1();
				NS::XmlUtil::getAttribute(sku.n_target_camp, xmlItem, "n_target_camp_5");// = proto.n_target_camp_1();
			}

			towerBuildingDefence.m_vSkill.emplace_back(sku);
        }

        {
			TowerFunctionInfo tfi;
			NS::XmlUtil::getAttribute(tfi.n_function, xmlItem, "n_function_1");// = proto.n_function_1();
			if (tfi.n_function > 0)
            {
				NS::XmlUtil::getAttribute(tfi.n_function_param1, xmlItem, "n_function_1_param1");// = proto.n_function_1_param1();
				NS::XmlUtil::getAttribute(tfi.n_function_param2, xmlItem, "n_function_1_param2");// = proto.n_function_1_param2();
				NS::XmlUtil::getAttribute(tfi.n_function_param3, xmlItem, "n_function_1_param3");// = proto.n_function_1_param3();
				NS::XmlUtil::getAttribute(tfi.n_function_param4, xmlItem, "n_function_1_param4");// = proto.n_function_1_param4();
				NS::XmlUtil::getAttribute(tfi.s_function_param, xmlItem, "s_function_1_param");// = proto.s_function_1_param();
				towerBuildingDefence.m_vTowerFunction.emplace_back(tfi);
			}
		}

		// 水晶结界数据
		{
			// 结界影响怪物等级(包含及以下)
			NS::XmlUtil::getAttribute(towerBuildingDefence.m_CrystalWardsData.n_WardsMobLev, xmlItem, "n_WardsMobLev");// = proto.n_wardsmoblev();

			// 进入结界时给目标添加的Buff
			std::string s_WardsBuff;
			NS::XmlUtil::getAttribute(s_WardsBuff, xmlItem, "s_WardsBuff");//
			
			if (!s_WardsBuff.empty() && s_WardsBuff != "null")
			{
                bzbee::StringUtil::splitString(towerBuildingDefence.m_CrystalWardsData.vecWardsBuff, s_WardsBuff, ",");
			}

			// 免疫结界效果怪物ID
			std::string s_WardsImmunityMob;
			NS::XmlUtil::getAttribute(s_WardsImmunityMob, xmlItem, "s_WardsImmunityMob");//
			if (!s_WardsImmunityMob.empty() && s_WardsImmunityMob != "null")
			{
                bzbee::StringUtil::splitString(towerBuildingDefence.m_CrystalWardsData.vecWardsImmunityMob, s_WardsImmunityMob, ",");
			}
		}

  //      if (towerBuildingDefence.m_category == TowerBuildingCategory::SHIP_MAINGUN)
		//{
		//	if (towerBuildingDefence.m_level > m_towerBuildingShipGunMaxLev)
		//	{
		//		m_towerBuildingShipGunMaxLev = towerBuildingDefence.m_level;
		//	}
		//	m_towerBuildingShipGunMap.emplace(towerBuildingDefence.m_id, towerBuildingDefence);
		//}

		xmlItem = xmlItem->NextSiblingElement("Record");

		m_towerBuildingDefenceMap[towerBuildingDefence.m_id] = towerBuildingDefence;
	}

	return true;
}

//bool TowerBuildingCfg::parseBuildTowerDefencePvp(tinyxml2::XMLElement* pXmlRoot);
//{
//    config::BuildTowerDefencePvPSet dataList;
//    if (!loadProtoConfig(dataList, "config/BuildTowerDefencePvP.bin")) return false;
//
//    int itemSize = dataList.buildtowerdefencepvps_size();
//    for (int i = 0; i < itemSize; ++i)
//    {
//        auto& proto = dataList.buildtowerdefencepvps(i);
//        auto& towerBuildingDefence = m_towerBuildingDefenceMapPvP[proto.n_id()];
//
//        towerBuildingDefence.m_id = proto.n_id();
//        towerBuildingDefence.m_buildingTypeId = proto.n_build_cfg_type();
//        towerBuildingDefence.m_level = proto.n_level();
//        towerBuildingDefence.m_name_untrans = proto.s_name();
//        towerBuildingDefence.m_note_untrans = proto.s_note();
//        towerBuildingDefence.m_category = (TowerBuildingCategory)proto.n_category();
//        towerBuildingDefence.m_typeDes_untrans = proto.s_type_des();
//        towerBuildingDefence.m_energy = proto.n_energy();
//        towerBuildingDefence.m_power = proto.n_power();
//        towerBuildingDefence.m_hp = proto.n_hp();
//        towerBuildingDefence.m_canAttack = proto.n_can_attack();
//        towerBuildingDefence.m_hpUiCheck = proto.f_hpuicheck();
//        towerBuildingDefence.m_fov = proto.n_fov();
//        towerBuildingDefence.m_forbidden = proto.n_forbidden();
//        towerBuildingDefence.m_ad = proto.n_ad();
//        towerBuildingDefence.m_ap = proto.n_ap();
//        towerBuildingDefence.m_skillSchematic = proto.s_skill_schematic();
//        towerBuildingDefence.m_roleLevelLimit = proto.n_min_role_level();
//        towerBuildingDefence.n_arm = proto.n_arm();
//        towerBuildingDefence.n_marm = proto.n_marm();
//        towerBuildingDefence.n_public_cd = proto.n_public_cd();
//        towerBuildingDefence.n_disapear_die = proto.n_disapear_die();
//        towerBuildingDefence.n_BeAttackNumberLimit = proto.n_beattacknumberlimit();
//        towerBuildingDefence.n_mob_id = proto.n_mob_id();
//
//        if (proto.n_skill_1() >= 0)
//        {
//            TowerBuildSkillInfo sku;
//            sku.n_skill = proto.n_skill_1();
//            sku.n_skill_target_choose_first = proto.n_skill_target_1();
//            sku.n_skill_target_movetype = proto.n_skill_target_movetype_1();
//            sku.n_skill_cast_event = proto.n_skill_cast_event_1();
//            sku.n_target_type = proto.n_target_type_1();
//            sku.n_target_camp = proto.n_target_camp_1();
//            towerBuildingDefence.m_vSkill.emplace_back(sku);
//        }
//
//        if (proto.n_skill_2() >= 0)
//        {
//            TowerBuildSkillInfo sku;
//            sku.n_skill = proto.n_skill_2();
//            sku.n_skill_target_choose_first = proto.n_skill_target_2();
//            sku.n_skill_target_movetype = proto.n_skill_target_movetype_2();
//            sku.n_skill_cast_event = proto.n_skill_cast_event_2();
//            sku.n_target_type = proto.n_target_type_2();
//            sku.n_target_camp = proto.n_target_camp_2();
//            towerBuildingDefence.m_vSkill.emplace_back(sku);
//        }
//
//        if (proto.n_skill_3() >= 0)
//        {
//            TowerBuildSkillInfo sku;
//            sku.n_skill = proto.n_skill_3();
//            sku.n_skill_target_choose_first = proto.n_skill_target_3();
//            sku.n_skill_target_movetype = proto.n_skill_target_movetype_3();
//            sku.n_skill_cast_event = proto.n_skill_cast_event_3();
//            sku.n_target_type = proto.n_target_type_3();
//            sku.n_target_camp = proto.n_target_camp_3();
//            towerBuildingDefence.m_vSkill.emplace_back(sku);
//        }
//
//        if (proto.n_skill_4() >= 0)
//        {
//            TowerBuildSkillInfo sku;
//            sku.n_skill = proto.n_skill_4();
//            sku.n_skill_target_choose_first = proto.n_skill_target_4();
//            sku.n_skill_target_movetype = proto.n_skill_target_movetype_4();
//            sku.n_skill_cast_event = proto.n_skill_cast_event_4();
//            sku.n_target_type = proto.n_target_type_4();
//            sku.n_target_camp = proto.n_target_camp_4();
//            towerBuildingDefence.m_vSkill.emplace_back(sku);
//        }
//
//        if (proto.n_skill_5() >= 0)
//        {
//            TowerBuildSkillInfo sku;
//            sku.n_skill = proto.n_skill_5();
//            sku.n_skill_target_choose_first = proto.n_skill_target_5();
//            sku.n_skill_target_movetype = proto.n_skill_target_movetype_5();
//            sku.n_skill_cast_event = proto.n_skill_cast_event_5();
//            sku.n_target_type = proto.n_target_type_5();
//            sku.n_target_camp = proto.n_target_camp_5();
//            towerBuildingDefence.m_vSkill.emplace_back(sku);
//        }
//
//        {
//            TowerFunctionInfo tfi;
//            tfi.n_function = proto.n_function_1();
//            if (tfi.n_function > 0)
//            {
//                tfi.n_function_param1 = proto.n_function_1_param1();
//                tfi.n_function_param2 = proto.n_function_1_param2();
//                tfi.n_function_param3 = proto.n_function_1_param3();
//                tfi.n_function_param4 = proto.n_function_1_param4();
//                tfi.s_function_param = proto.s_function_1_param();
//                towerBuildingDefence.m_vTowerFunction.emplace_back(tfi);
//            }
//        }
//
//        // 水晶结界数据
//        {
//            // 结界影响怪物等级(包含及以下)
//            towerBuildingDefence.m_CrystalWardsData.n_WardsMobLev = proto.n_wardsmoblev();
//
//            // 进入结界时给目标添加的Buff
//            auto& s_WardsBuff = proto.s_wardsbuff();
//            if (!s_WardsBuff.empty() && s_WardsBuff != "null")
//            {
//                bzbee::StringUtil::splitString(towerBuildingDefence.m_CrystalWardsData.vecWardsBuff, s_WardsBuff, ",");
//            }
//
//            // 免疫结界效果怪物ID
//            auto& s_WardsImmunityMob = proto.s_wardsimmunitymob();
//            if (!s_WardsImmunityMob.empty() && s_WardsImmunityMob != "null")
//            {
//                bzbee::StringUtil::splitString(towerBuildingDefence.m_CrystalWardsData.vecWardsImmunityMob, s_WardsImmunityMob, ",");
//            }
//        }
//
//        if (towerBuildingDefence.m_category == TowerBuildingCategory::SHIP_MAINGUN)
//        {
//            if (towerBuildingDefence.m_level > m_towerBuildingShipGunMaxLev)
//            {
//                m_towerBuildingShipGunMaxLev = towerBuildingDefence.m_level;
//            }
//            m_towerBuildingShipGunMap.emplace(towerBuildingDefence.m_id, towerBuildingDefence);
//        }
//    }
//
//    return true;
//}

AttrMap TowerBuildingDefence::getAttr() const
{
	return
    {
		{ MsgPB::ROLE_EFFECT_AD, static_cast<float>(m_ad) },
		{ MsgPB::ROLE_EFFECT_AP, static_cast<float>(m_ap) },
		{ MsgPB::ROLE_EFFECT_ARM, static_cast<float>(n_arm) },
		{ MsgPB::ROLE_EFFECT_MARM, static_cast<float>(n_marm) },
	};
}

bool TowerBuildingCfg::parseBuildTowerRepair(tinyxml2::XMLElement* pXmlRoot)
{
    if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	//auto pAttrXml = p->FirstChildElement("Attribute");
	//while (pAttrXml)
	//{
	//	pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	//}

	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{

		TowerBuildingRepair buildTowerRepair;
        //auto& proto = dataList.buildtowerrepairs(i);
        //auto& buildTowerRepair = m_towerBuildingRepairMap[proto.n_id()];

		NS::XmlUtil::getAttribute(buildTowerRepair.m_id, xmlItem, "n_ID");// = proto.n_id();
        NS::XmlUtil::getAttribute(buildTowerRepair.m_buildingTypeId, xmlItem, "n_build_cfg_type");// = proto.n_build_cfg_type();
        NS::XmlUtil::getAttribute(buildTowerRepair.m_level, xmlItem, "n_level");// = proto.n_level();
        NS::XmlUtil::getAttribute(buildTowerRepair.m_cashSkip, xmlItem, "n_cash_skip");// = proto.n_cash_skip();
        NS::XmlUtil::getAttribute(buildTowerRepair.m_skipSecUnit, xmlItem, "n_skip_sec_unit");// = proto.n_skip_sec_unit();
        NS::XmlUtil::getAttribute(buildTowerRepair.m_repairGold, xmlItem, "n_repair_gold");// = proto.n_repair_gold();
        NS::XmlUtil::getAttribute(buildTowerRepair.m_repairTime, xmlItem, "n_repair_time");// = proto.n_repair_time();

		std::string s_repair_material;
		NS::XmlUtil::getAttribute(s_repair_material, xmlItem, "s_repair_material");
		NS::StringList strList;
		NS::StringUtil::splitString(strList, s_repair_material, ",");

		for (const auto& s : strList)
		{
			TowerBuildingRepair::RepairMaterial mat;
			int count = sscanf(s.c_str(), "[%d:%d]", &mat.m_itemId, &mat.m_num);
			if (count == 2)
			{
				buildTowerRepair.m_repairMaterialList.emplace_back(mat);
			}
		}
		m_towerBuildingRepairMap[buildTowerRepair.m_id] = buildTowerRepair;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

bool TowerBuildingCfg::parseBuildTowerUpdate(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	//auto pAttrXml = p->FirstChildElement("Attribute");
	//while (pAttrXml)
	//{
	//	pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	//}

	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{
       // auto& proto = dataList.buildtowerupdates(i);
        //auto& buildTowerUpdate = m_towerBuildingUpdateMap[proto.n_id()];

		TowerBuildingUpdate buildTowerUpdate;

		NS::XmlUtil::getAttribute(buildTowerUpdate.m_id, xmlItem, "n_ID");// = proto.n_id();
		NS::XmlUtil::getAttribute(buildTowerUpdate.m_buildingTypeId, xmlItem, "n_build_cfg_type");// = proto.n_build_cfg_type();
		NS::XmlUtil::getAttribute(buildTowerUpdate.m_level, xmlItem, "n_level");// = proto.n_level();
		NS::XmlUtil::getAttribute(buildTowerUpdate.m_needCoin, xmlItem, "n_coin");// = proto.n_coin();

		{
			NS::XmlUtil::getAttribute(buildTowerUpdate.m_upItem1.m_itemTypeId, xmlItem, "n_item_type_1");// = proto.n_item_type_1();
			NS::XmlUtil::getAttribute(buildTowerUpdate.m_upItem1.m_itemNum, xmlItem, "n_item_count_1");// = proto.n_item_count_1();
			buildTowerUpdate.m_upgradeStuffMap[buildTowerUpdate.m_upItem1.m_itemTypeId] = buildTowerUpdate.m_upItem1;
		}

        {
			NS::XmlUtil::getAttribute(buildTowerUpdate.m_upItem2.m_itemTypeId, xmlItem, "n_item_type_2");// = proto.n_item_type_2();
			NS::XmlUtil::getAttribute(buildTowerUpdate.m_upItem2.m_itemNum, xmlItem, "n_item_count_2");// = proto.n_item_count_2();
            buildTowerUpdate.m_upgradeStuffMap[buildTowerUpdate.m_upItem2.m_itemTypeId] = buildTowerUpdate.m_upItem2;
        }

		m_towerBuildingUpdateMap[buildTowerUpdate.m_id] = buildTowerUpdate;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

bool TowerBuildingCfg::parseBuildTowerReform(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	//auto pAttrXml = p->FirstChildElement("Attribute");
	//while (pAttrXml)
	//{
	//	pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	//}

	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{
		TowerBuildingReform buildTowerReform;
        //auto& proto = dataList.buildtowerreforms(i);
       // auto& buildTowerReform = m_towerBuildingReformMap[proto.n_id()];

		NS::XmlUtil::getAttribute(buildTowerReform.m_id, xmlItem, "n_ID");// = proto.n_id();
		NS::XmlUtil::getAttribute(buildTowerReform.m_buildingTypeId, xmlItem, "n_build_type");// = proto.n_build_type();
		NS::XmlUtil::getAttribute(buildTowerReform.m_reformBuildTypeId, xmlItem, "n_build_type_reform");// = proto.n_build_type_reform();
		NS::XmlUtil::getAttribute(buildTowerReform.m_needCoin, xmlItem, "n_coin");// = proto.n_coin();

		{
			TowerBuildingReform::EvolveStuff stuff;
			NS::XmlUtil::getAttribute(stuff.m_itemTypeId, xmlItem, "n_item_type_1");// = proto.n_item_type_1();
			NS::XmlUtil::getAttribute(stuff.m_itemNum, xmlItem, "n_item_count_1");// = proto.n_item_count_1();
			buildTowerReform.m_evolveStuffMap[stuff.m_itemTypeId] = stuff;
		}

        {
            TowerBuildingReform::EvolveStuff stuff;
			NS::XmlUtil::getAttribute(stuff.m_itemTypeId, xmlItem, "n_item_type_2");// = proto.n_item_type_1();
			NS::XmlUtil::getAttribute(stuff.m_itemNum, xmlItem, "n_item_count_2");// = proto.n_item_count_1();
            buildTowerReform.m_evolveStuffMap[stuff.m_itemTypeId] = stuff;
        }

        {
            TowerBuildingReform::EvolveStuff stuff;
			NS::XmlUtil::getAttribute(stuff.m_itemTypeId, xmlItem, "n_item_type_3");// = proto.n_item_type_1();
			NS::XmlUtil::getAttribute(stuff.m_itemNum, xmlItem, "n_item_count_3");// = proto.n_item_count_1();
            buildTowerReform.m_evolveStuffMap[stuff.m_itemTypeId] = stuff;
        }
		m_towerBuildingReformMap[buildTowerReform.m_id] = buildTowerReform;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

void TowerBuildingCfg::ParseShootFov(const std::string& str, const bzbee::int32 ShootFov, TowerBuildShootFov& Cfg)
{
	std::vector<bzbee::int32> v_int;
	bzbee::StringUtil::splitString(v_int, str, ",");
	if (v_int.size() >= 2) {
		Cfg.m_fShootingDirX = v_int[0];
		Cfg.m_fShootingDirY = v_int[1];

		double l = sqrt(Cfg.m_fShootingDirX * Cfg.m_fShootingDirX + Cfg.m_fShootingDirY * Cfg.m_fShootingDirY);
		if (l > 0.0000000001) {
			double inv_l = 1.0 / l; //normalise
			Cfg.m_fShootingDirX = Cfg.m_fShootingDirX * inv_l;
			Cfg.m_fShootingDirY = Cfg.m_fShootingDirY * inv_l;
		}
		else {
			Cfg.m_fShootingDirX = 0.0;
			Cfg.m_fShootingDirY = 1.0;
		}
	}
	else {
		Cfg.m_fShootingDirX = 0.0;
		Cfg.m_fShootingDirY = 1.0;
	}

	double rad = (ShootFov * 3.1415926) / 180.0;
	Cfg.m_fShootingCos = cos(rad);
	if (std::fabs(Cfg.m_fShootingCos) < 0.0000001)
		Cfg.m_fShootingCos = 0.0;
}

bool TowerBuildingCfg::parseBuildTowerTemplateAttack(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	//auto pAttrXml = p->FirstChildElement("Attribute");
	//while (pAttrXml)
	//{
	//	pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	//}

	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{
		TowerBuildTemplateAttack buildTowerTemplateAttack;
       // auto& proto = dataList.buildtowertemplateattacks(i);
        //auto& buildTowerTemplateAttack = m_towerBuildTemplateAttackMap[proto.n_id()];

		NS::XmlUtil::getAttribute(buildTowerTemplateAttack.n_ID, xmlItem, "n_ID");// = proto.n_id();
		std::string s_shooting_point;
		NS::XmlUtil::getAttribute(s_shooting_point, xmlItem, "s_shooting_point");
		std::vector<bzbee::int32> v_int;
        bzbee::StringUtil::splitString(v_int, s_shooting_point, ",");
		if (v_int.size() >= 2)
        {
			buildTowerTemplateAttack.m_iShootingPointX = v_int[0];
			buildTowerTemplateAttack.m_iShootingPointY = v_int[1];
		}
		else
        {
			buildTowerTemplateAttack.m_iShootingPointX = 0;
			buildTowerTemplateAttack.m_iShootingPointY = 0;
		}

		std::string s_shooting_direction;
		NS::XmlUtil::getAttribute(s_shooting_direction, xmlItem, "s_shooting_direction");
		bzbee::int32 n_shooting_fov = 0;//proto.n_shooting_fov();
		NS::XmlUtil::getAttribute(n_shooting_fov, xmlItem, "n_shooting_fov");

		if (!s_shooting_direction.empty() && n_shooting_fov != -1)
		{
			TowerBuildShootFov Cfg;
			ParseShootFov(s_shooting_direction, n_shooting_fov, Cfg);
			buildTowerTemplateAttack.m_vShootFovCfg.push_back(Cfg);
		}

		NS::XmlUtil::getAttribute(s_shooting_direction, xmlItem, "s_shooting_direction1");
		NS::XmlUtil::getAttribute(n_shooting_fov, xmlItem, "n_shooting_fov1");
		if (!s_shooting_direction.empty() && n_shooting_fov != -1)
		{
			TowerBuildShootFov Cfg;
			ParseShootFov(s_shooting_direction, n_shooting_fov, Cfg);
			buildTowerTemplateAttack.m_vShootFovCfg.push_back(Cfg);
		}
		m_towerBuildTemplateAttackMap[buildTowerTemplateAttack.n_ID] = buildTowerTemplateAttack;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

bool TowerBuildingCfg::parseBuildTowerCrystalBuff(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	//auto pAttrXml = p->FirstChildElement("Attribute");
	//while (pAttrXml)
	//{
	//	pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	//}

	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{
		TowerBuildCrystalBuff buildTowerCrystalBuf;
        //auto& proto = dataList.buildtowercrystalbuffs(i);
        //auto& buildTowerCrystalBuf = m_towerBuildCrystalBuffMap[proto.n_id()];

		NS::XmlUtil::getAttribute(buildTowerCrystalBuf.n_ID, xmlItem, "n_ID");// = proto.n_id();
		NS::XmlUtil::getAttribute(buildTowerCrystalBuf.n_need_level, xmlItem, "n_need_level");// = proto.n_need_level();
		NS::XmlUtil::getAttribute(buildTowerCrystalBuf.n_buff_id, xmlItem, "n_buff_id");// = proto.n_buff_id();
		NS::XmlUtil::getAttribute(buildTowerCrystalBuf.n_buff_duration, xmlItem, "n_buff_duration");// = proto.n_buff_duration();
		NS::XmlUtil::getAttribute(buildTowerCrystalBuf.s_name_untrans, xmlItem, "s_name");// = proto.s_name();

		{
			NS::StringList strList;
			std::string s_unlock_item;
			NS::XmlUtil::getAttribute(s_unlock_item, xmlItem, "s_unlock_item");

			for (const auto& s : strList)
			{
				TowerBuildCrystalBuffCostItem tcci;
				int count = sscanf(s.c_str(), "[%d:%d]", &tcci.m_ItemID, &tcci.m_ItemCount);
				if (count == 2)
				{
					buildTowerCrystalBuf.m_vCrystalBuffItemCost.emplace_back(tcci);
				}
			}
		}
		m_towerBuildCrystalBuffMap[buildTowerCrystalBuf.n_ID] = buildTowerCrystalBuf;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

const TowerBuildingDefence* TowerBuildingCfg::getTowerBuildingDefence(NS::uint nId)
{
	auto&& iter = m_towerBuildingDefenceMap.find(nId);
	if (iter == m_towerBuildingDefenceMap.end())
	{
		return nullptr;
	}
	return &(iter->second);
}

//const TowerBuildingDefence* TowerBuildingCfg::getTowerBuildingDefencePvP(NS::uint buildingTypeId, int level)
//{
//	for (auto&& p : m_towerBuildingDefenceMapPvP)
//	{
//		if (p.second.m_buildingTypeId == buildingTypeId && p.second.m_level == level)
//		{
//			return &p.second;
//		}
//	}
//
//	return nullptr;
//}

const TowerBuildingRepair* TowerBuildingCfg::getTowerBuildingRepair(NS::uint buildingTypeId, int level)
{
	for (auto&& p : m_towerBuildingRepairMap)
	{
		if (p.second.m_buildingTypeId == buildingTypeId && p.second.m_level == level)
		{
			return &p.second;
		}
	}

	return nullptr;
}

const TowerBuildingUpdate* TowerBuildingCfg::getTowerBuildingUpdate(NS::uint buildingTypeId, int level)
{
	for (auto&& p : m_towerBuildingUpdateMap)
	{
		if (p.second.m_buildingTypeId == buildingTypeId && p.second.m_level == level)
		{
			return &p.second;
		}
	}

	return nullptr;
}

int TowerBuildingCfg::getTowerBuildingEvoMaxLev(NS::uint buildingTypeId)
{
	int level = 0;
	for (auto&& p : m_towerBuildingUpdateMap)
	{
		if (p.second.m_buildingTypeId == buildingTypeId)
		{
			if (p.second.m_level > level)
			{
				level = p.second.m_level;
			}
		}
	}

	return level;
}

std::vector<const TowerBuildingReform*> TowerBuildingCfg::getTowerBuildingReformList(NS::uint buildingTypeId)
{
	std::vector<const TowerBuildingReform*> res;

	for (auto&& p : m_towerBuildingReformMap)
	{
		if (p.second.m_buildingTypeId == buildingTypeId)
		{
			res.push_back(&p.second);
		}
	}

	return res;
}

const TowerBuildingReform* TowerBuildingCfg::getTowerBuildingReform(NS::uint buildingTypeId, NS::uint toBuildingTypeId)
{
	for (auto&& p : m_towerBuildingReformMap)
	{
		if (p.second.m_buildingTypeId == buildingTypeId && p.second.m_reformBuildTypeId)
		{
			return &p.second;
		}
	}

	return nullptr;
}


const TowerBuildingReform* TowerBuildingCfg::getTowerBuildingReform(NS::uint toBuildingTypeId)
{
	for (auto&& p : m_towerBuildingReformMap)
	{
		if (p.second.m_reformBuildTypeId)
		{
			return &p.second;
		}
	}

	return nullptr;
}

NS::uint TowerBuildingCfg::getPrevEvolveBuildingTypeId(NS::uint buildingTypeId)
{
	for (const auto& p : m_towerBuildingReformMap)
	{
		if (p.second.m_reformBuildTypeId == (NS::uint)buildingTypeId)
		{
			return p.second.m_buildingTypeId;
		}
	}

	return 0;
}

bool TowerBuildingCfg::isCanEvolve(NS::uint buildingTypeId, NS::uint toBuildingTypeId, bool recursive)
{
	NS::uint temp{ toBuildingTypeId };

	do
	{
		NS::uint prevId{ getPrevEvolveBuildingTypeId(temp) };
		if (prevId == 0)
		{
			return false;
		}

		if (prevId == buildingTypeId)
		{
			return true;
		}

		temp = prevId;

	} while (recursive);

	return false;
}


// lua ------------------------------------------------------------------------------
NS::int32 TowerBuildingCfg::getTowerBuildingSoulMobNum(NS::uint towerBuildingDefenceId, int monsterTypeId)
{
	auto&& itor = m_towerBuildingDefenceMap.find(towerBuildingDefenceId);
	if (itor == m_towerBuildingDefenceMap.end())
	{
		return 0;
	}

	auto&& towerBuildingDefence = itor->second;

	auto&& pMonsterType = MonsterCfg::getInstance().findMonsterType(monsterTypeId);
	if (!pMonsterType)
	{
		return 0;
	}

	if (towerBuildingDefence.m_vTowerFunction.empty())
	{
		return 0;
	}

	auto&& towerFunction = towerBuildingDefence.m_vTowerFunction[0];

	if (pMonsterType->n_rare == MonsterCfg::MonsterTypeRare::ELITE)
	{
		return towerFunction.n_function_param2;
	}
	else if (pMonsterType->n_rare == MonsterCfg::MonsterTypeRare::NORMAL)
	{
		return towerFunction.n_function_param3;
	}

	return 0;
}


bool TowerBuildingCfg::reloadMultiLanguage()
{
	//for (auto& pr : m_towerBuildingDefenceMap)
	//{
 //       auto& defence = pr.second;
	//	defence.m_name = _LC(defence.m_name_untrans);
	//	defence.m_note = _LC(defence.m_note_untrans);
	//	defence.m_typeDes = _LC(defence.m_typeDes_untrans);
	//}

	//for (auto& pr : m_towerBuildingDefenceMapPvP)
	//{
 //       auto& pvp = pr.second;
 //       pvp.m_name = _LC(pvp.m_name_untrans);
	//	pvp.m_note = _LC(pvp.m_note_untrans);
	//	pvp.m_typeDes = _LC(pvp.m_typeDes_untrans);
	//}

	//for (auto& pr : m_towerBuildCrystalBuffMap)
	//{
 //       auto& buff = pr.second;
	//	buff.s_name = _LC(buff.s_name_untrans);
	//}

	//for (auto& pr : m_towerBuildingShipGunMap)
	//{
	//	auto& pvp = pr.second;
	//	pvp.m_name = _LC(pvp.m_name_untrans);
	//	pvp.m_note = _LC(pvp.m_note_untrans);
	//	pvp.m_typeDes = _LC(pvp.m_typeDes_untrans);
	//}

	return true;
}

//std::vector<LuaTable> TowerBuildingCfg::getTowerBuildingShipGunMap(int level)
//{
//	std::vector<LuaTable> vecTable;
//	for (auto&& tower : m_towerBuildingShipGunMap)
//	{
//		if (tower.second.m_level != level)continue;
//		LuaTable tab;
//		int tempId = 0;
//		const BuildingCfg::BuildingTemplate* pTemp = BuildingCfg::getInstance().findBuildingTemplateByTypeId(tower.second.m_buildingTypeId , BuildingCfg::Direction::LEFT);
//		if (pTemp) {
//			tempId = pTemp->m_templateId;
//		}
//		auto attrMap = tower.second.getAttr();
//		tab.set("m_id", tower.second.m_id);
//		tab.set("m_name", tower.second.m_name);
//		tab.set("m_typeId", tower.second.m_buildingTypeId);
//		tab.set("n_level", tower.second.m_level);
//		tab.set("m_hp", tower.second.m_hp);
//		tab.set("m_fov", tower.second.m_fov);
//		tab.set("m_hpm", attrMap[ROLE_EFFECT_HPM]);
//		tab.set("m_ad", attrMap[ROLE_EFFECT_AD]);
//		tab.set("m_ap", attrMap[ROLE_EFFECT_AP]);
//		tab.set("m_arm", attrMap[ROLE_EFFECT_ARM]);
//		tab.set("m_marm", attrMap[ROLE_EFFECT_MARM]);
//		tab.set("m_note", tower.second.m_note);
//		tab.set("roleLevel", tower.second.m_roleLevelLimit);
//		tab.set("m_public_cd", fmt::format("{0:.1f}", tower.second.n_public_cd*1.0 / 1000));
//		tab.set("m_skillSchematic" , tower.second.m_skillSchematic);
//		tab.set("tempId", tempId);
//
//		vecTable.push_back(tab);
//	}
//	return vecTable;
//}

//LuaTable TowerBuildingCfg::getTowerBuildingDefence_Lua(NS::uint buildingTypeId, int level)
//{
//	LuaTable table;
//	const TowerBuildingDefence* pTowerCfg = getTowerBuildingDefence(buildingTypeId , level);
//	if (pTowerCfg)
//	{
//		auto attrMap = pTowerCfg->getAttr();
//		table.set("m_name", pTowerCfg->m_name);
//		table.set("m_hp", pTowerCfg->m_hp);
//		table.set("m_ad", attrMap[ROLE_EFFECT_AD]);
//		table.set("m_ap", attrMap[ROLE_EFFECT_AP]);
//		table.set("m_arm", attrMap[ROLE_EFFECT_ARM]);
//		table.set("m_marm", attrMap[ROLE_EFFECT_MARM]);
//		table.set("m_fov", pTowerCfg->m_fov);
//		table.set("m_skillSchematic", pTowerCfg->m_skillSchematic);
//		table.set("m_note", pTowerCfg->m_note);
//		table.set("roleLevel", pTowerCfg->m_roleLevelLimit);
//		table.set("m_public_cd", fmt::format("{0:.1f}" , pTowerCfg->n_public_cd*1.0/1000));
//	}
//	return table;
//}
//
//LuaTable TowerBuildingCfg::getTowerBuildingUpdate_Lua(NS::uint buildingTypeId, int level)
//{
//	LuaTable table;
//	auto&& pUpdateCfg = getTowerBuildingUpdate(buildingTypeId, level);
//	if (pUpdateCfg)
//	{
//		table.set("m_id", pUpdateCfg->m_id);
//		table.set("m_level", pUpdateCfg->m_level);
//		table.set("m_buildingTypeId", pUpdateCfg->m_buildingTypeId);
//		table.set("m_needCoin", pUpdateCfg->m_needCoin);
//
//		// 升级需要的Item
//		std::vector<LuaTable> luaTabVec;
//		for (auto&& item : pUpdateCfg->m_upgradeStuffMap)
//		{
//			LuaTable itemTab;
//			itemTab.set("item_id", item.second.m_itemTypeId);
//			itemTab.set("count", item.second.m_itemNum);
//			luaTabVec.push_back(itemTab);
//		}
//		table.set("items", luaTabVec);
//	}
//	return table;
//}
//
//TowerBuildingCfg* LUA(get_towerBuildCfg)(void)
//{
//	return &TowerBuildingCfg::getInstance();
//}
