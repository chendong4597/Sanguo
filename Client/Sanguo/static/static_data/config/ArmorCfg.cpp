/*!
 * \file ArmorCfg.cpp
 *
 * \author ChenBK
 * \date 12/30/2016
 *
 */
#include "stdafx.h"
#include "ArmorCfg.h"
#include "PlayerAttrCfg.h"
#include "../data/ItemData.h"
#include "../common/calculate.h"
#include <RandomUtil.h>
#include <StringUtil.h>
#include <XmlUtil.h>

using namespace NS;


ArmorCfg& ArmorCfg::getInstance()
{
	static ArmorCfg s_ArmorCfg;
	return s_ArmorCfg;
}

bool ArmorCfg::initialize()
{

	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/Armor.bin")) return false;
	if (!parseArmorConfig(doc.RootElement())) return false;

	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/ArmorUpGradeCfg.bin")) return false;
	if (!parseArmorUpGradeConfig(doc.RootElement())) return false;

	//if (!parseArmorBinary()) return false;
	//if (!parseArmorCfgTemplateBinary()) return false;
 //   if (!parseArmorMagicAddBinary()) return false;
 //   if (!parseArmorTypeGroupBinary()) return false;
 //   if (!parseArmorAttrExtBinary()) return false;
 //   if (!parseArmorSuitCfgBinary()) return false;
 //   if (!parseArmorUpGradeCfgBinary()) return false;
 //   if (!parseArmorFogUnlockBinary()) return false;
 //   if (!parseArmorSpecialEffectCfgBinary()) return false;

	return true;
}

const ArmorCfg::ArmorUpGrade* ArmorCfg::getUpLevelCfg(int id)
{
	auto it = m_mapUpLevel.find(id);
	if (it == m_mapUpLevel.end())
	{
		return nullptr;
	}
	return &it->second;
}

const ArmorCfg::ArmorType * ArmorCfg::findCanDigArmorType(NS::int32 digLevel, ArmorCfg::TargetLayer layer)
{
	NS::int32 minLevel{ INT32_MAX };
	const ArmorType* ret{nullptr};
	for (auto&& pir : m_armorTypeMap)
	{
		auto&& val = pir.second;
		if (val.n_target_layer == static_cast<int>(layer) && val.n_gatherLv >= digLevel && val.n_gatherLv < minLevel)
		{
			minLevel = val.n_gatherLv;
			ret = &val;
		}
	}

	return ret;
}

bool ArmorCfg::parseArmorUpGradeConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		ArmorUpGrade upgradeType;

		NS::XmlUtil::getAttribute(upgradeType.id, xmlItem, "n_ID");//
		NS::XmlUtil::getAttribute(upgradeType.nextId, xmlItem, "n_Next_Id");// = proto.n_next_id();

        NS::StringList strArray;
		std::string s_need;
		NS::XmlUtil::getAttribute(s_need, xmlItem, "s_Need");
        NS::StringUtil::splitString(strArray, s_need, ";",true);
        for (const auto& strValue : strArray)
		{
            std::vector<int> intValues;
            NS::StringUtil::splitString(intValues, strValue, ",",true);

            if (!intValues.empty())
			{
				ArmorCfg::ArmorUpGrade::needInfo info{};
				info.id = intValues[0];
				info.num = intValues[1];
				upgradeType.vecItem.push_back(info);
			}
		}
		m_mapUpLevel.insert(std::make_pair(upgradeType.id, upgradeType));
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

bool ArmorCfg::parseArmorConfig(tinyxml2::XMLElement* pXmlRoot)
{

	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		ArmorType armorType;

		NS::XmlUtil::getAttribute(armorType.m_typeId, xmlItem, "n_ID");//
        NS::XmlUtil::getAttribute(armorType.m_equipType, xmlItem, "n_EquipType");// = proto.n_equiptype();
        NS::XmlUtil::getAttribute(armorType.m_equipSubType, xmlItem, "n_Sub_EquipType");// = proto.n_sub_equiptype();
        NS::XmlUtil::getAttribute(armorType.m_equipPos, xmlItem, "n_EquipPos");// = proto.n_equippos();
        NS::XmlUtil::getAttribute(armorType.m_starRank, xmlItem, "n_StarRank");// = proto.n_starrank();
        NS::XmlUtil::getAttribute(armorType.n_Durable, xmlItem, "n_Durable");// = proto.n_durable();
        NS::XmlUtil::getAttribute(armorType.f_Pause_time, xmlItem, "f_Pause_time");// = proto.f_pause_time();
        NS::XmlUtil::getAttribute(armorType.m_atkRange, xmlItem, "f_AtkRange");// = proto.f_atkrange();
        NS::XmlUtil::getAttribute(armorType.m_atkSpeed, xmlItem, "f_AtkSpeed");// = proto.f_atkspeed();
        NS::XmlUtil::getAttribute(armorType.n_Atk_Limit, xmlItem, "n_Atk_Limit");// = proto.n_atk_limit();
        NS::XmlUtil::getAttribute(armorType.n_gatherLv, xmlItem, "n_GatherLv");// = proto.n_gatherlv();
        NS::XmlUtil::getAttribute(armorType.f_GatherSpeed, xmlItem, "f_GatherSpeed");// = proto.f_gatherspeed();
        NS::XmlUtil::getAttribute(armorType.m_ResourceAtkDmg, xmlItem, "n_ResourceAtkDmg");// = static_cast<float>(proto.n_resourceatkdmg());
        NS::XmlUtil::getAttribute(armorType.m_hurtEffectId, xmlItem, "n_hurtid");// = proto.n_hurtid();
        NS::XmlUtil::getAttribute(armorType.m_attrExt, xmlItem, "n_AttrExt");// = proto.n_attrext();
        NS::XmlUtil::getAttribute(armorType.n_pet_moveType, xmlItem, "n_pet_moveType");// = proto.n_pet_movetype();
        NS::XmlUtil::getAttribute(armorType.n_canUpLevel, xmlItem, "n_isupgrade");// = proto.n_isupgrade();
//        armorType.n_SoundBankType = proto.n_SoundBankType();
		
		std::string s_soundbankvec;
		NS::XmlUtil::getAttribute(s_soundbankvec, xmlItem, "s_SoundBankVec");
        bzbee::StringUtil::splitString(armorType.s_SoundBankVec, s_soundbankvec, ",");

		int n_skillid_1{};
		NS::XmlUtil::getAttribute(n_skillid_1, xmlItem, "n_SkillID_1");
        if (n_skillid_1 != 0) armorType.m_vSkill.push_back(n_skillid_1);

		int n_skillid_2{};
		NS::XmlUtil::getAttribute(n_skillid_2, xmlItem, "n_SkillID_2");
        if (n_skillid_2 != 0) armorType.m_vSkill.push_back(n_skillid_2);

		int n_skillid_3{};
		NS::XmlUtil::getAttribute(n_skillid_3, xmlItem, "n_SkillID_3");
        if (n_skillid_3 != 0) armorType.m_vSkill.push_back(n_skillid_3);

        std::vector<int32> skillIds{};
		std::string s_passive_skillid;
		NS::XmlUtil::getAttribute(s_passive_skillid, xmlItem, "s_Passive_SkillID");
        bzbee::StringUtil::splitString(skillIds, s_passive_skillid, ",");
        for (auto skillId : skillIds)
        {
            if (skillId != 0) armorType.m_vPassiveSkill.push_back(skillId);
        }

		int n_armorspcialeffectid;
		NS::XmlUtil::getAttribute(n_armorspcialeffectid, xmlItem, "n_ArmorSpcialEffectID");
        if (n_armorspcialeffectid != 0) armorType.n_vArmorSpcialEffectIDs.push_back(n_armorspcialeffectid);

        NS::XmlUtil::getAttribute(armorType.m_sMotion, xmlItem, "s_MotionID");// = proto.s_motionid();
        NS::XmlUtil::getAttribute(armorType.m_s3DModel, xmlItem, "s_3DModelID");// = proto.s_3dmodelid();
        NS::XmlUtil::getAttribute(armorType.m_s3DModelWoman, xmlItem, "s_3DModelID_Woman");// = proto.s_3dmodelid_woman();
        NS::XmlUtil::getAttribute(armorType.m_durable, xmlItem, "n_Durable");// = proto.n_durable();
        NS::XmlUtil::getAttribute(armorType.m_durableRate, xmlItem, "n_DurableRate");// = proto.n_durablerate();
        NS::XmlUtil::getAttribute(armorType.n_target_layer, xmlItem, "n_GatherLayer");// = proto.n_gatherlayer();
        NS::XmlUtil::getAttribute(armorType.m_sSub3DModel, xmlItem, "s_3DModelID2");// = proto.s_3dmodelid2();
        NS::XmlUtil::getAttribute(armorType.m_sSub3DModelWoman, xmlItem, "s_3DModelID2s_Woman");// = proto.s_3dmodelid2s_woman();
        NS::XmlUtil::getAttribute(armorType.m_sHigh3DModel, xmlItem, "s_High_3DModelID");// = proto.s_high_3dmodelid();
        NS::XmlUtil::getAttribute(armorType.m_sHigh3DModelWoman, xmlItem, "s_High_3DModelID_Woman");// = proto.s_high_3dmodelid_woman();
        NS::XmlUtil::getAttribute(armorType.m_sSubHigh3DModel, xmlItem, "s_High_3DModelID2");// = proto.s_high_3dmodelid2();
        NS::XmlUtil::getAttribute(armorType.m_sSubHigh3DModelWoman, xmlItem, "s_High_3DModelID2_Woman");// = proto.s_high_3dmodelid2_woman();
		
		// parse attack animation and effect
		auto&& motion = armorType.m_sMotion;
		NS::StringList motionList;
		int num = NS::StringUtil::splitString(motionList, motion, ",");
		for (int i = 0; i < num; ++i)
		{
			NS::StringList anieffect;
			if (NS::StringUtil::splitString(anieffect, motionList[i], "|") != 2) { continue; }
			armorType.m_atkAniEffect.emplace_back(anieffect[0], anieffect[1]);
		}

		// parse jump attack animation and effect
  //      motionList.clear();
  //      num = NS::StringUtil::splitString(motionList, proto.s_jumpmotionid(), ",");
		//for (int i = 0; i < num; ++i)
		//{
		//	NS::StringList anieffect;
		//	if (NS::StringUtil::splitString(anieffect, motionList[i], "|") != 2) { continue; }
		//	armorType.m_jumpAtkAniEffect.emplace_back(anieffect[0], anieffect[1]);
		//}

		std::string s_underattackeffectid;
		NS::XmlUtil::getAttribute(s_underattackeffectid, xmlItem, "s_UnderAttackEffectID");
        NS::StringUtil::splitString(armorType.m_hurtEffect, s_underattackeffectid, ",");

        armorType.m_vecEffectAttr.resize(7);

		auto addTemplate = [](ArmorType& armorType, int index, int n_effect_type,int n_effect_attrref1,int n_effect_value1, int n_effect_attrref2, int n_effect_value2)
		{
			auto& attaInfoType = armorType.m_vecEffectAttr[index];
			attaInfoType.n_effect_type = n_effect_type;
			attaInfoType.vecArmorAtta.resize(2);
			attaInfoType.vecArmorAtta[0].n_effect_attrref = n_effect_attrref1;
			attaInfoType.vecArmorAtta[0].n_effect_value = n_effect_value1;
			attaInfoType.vecArmorAtta[1].n_effect_attrref = n_effect_attrref2;
			attaInfoType.vecArmorAtta[1].n_effect_value = n_effect_value2;
		};
		for (int i = 0 ; i < 7 ; i++)
		{
			int n_effect_type = 0;
			int n_effect_attrref1 = 0;
			int n_effect_value1 = 0;
			int n_effect_attrref2 = 0;
			int n_effect_value2 = 0;
			NS::XmlUtil::getAttribute(n_effect_type, xmlItem, fmt::format("n_effect_type_{0}", i + 1).c_str());
			NS::XmlUtil::getAttribute(n_effect_attrref1, xmlItem, fmt::format("n_effect_attrref1_{0}", i + 1).c_str());
			NS::XmlUtil::getAttribute(n_effect_value1, xmlItem, fmt::format("n_effect_value1_{0}", i + 1).c_str());
			NS::XmlUtil::getAttribute(n_effect_attrref2, xmlItem, fmt::format("n_effect_attrref2_{0}", i + 1).c_str());
			NS::XmlUtil::getAttribute(n_effect_value2, xmlItem, fmt::format("n_effect_value2_{0}", i + 1).c_str());
			addTemplate(armorType, i, n_effect_type, n_effect_attrref1, n_effect_value1, n_effect_attrref2, n_effect_value2);
		}

        NS::XmlUtil::getAttribute(armorType.n_replaceNakedModel, xmlItem, "n_nakedModel");// = proto.n_nakedmodel();
		NS::XmlUtil::getAttribute(armorType.s_trailEffectModel, xmlItem, "s_SmokeEffect");// = proto.s_smokeeffect();
       // NS::StringUtil::splitString(armorType.m_vDisablePos, proto.s_disablepos(), ",");
       // NS::StringUtil::splitString(armorType.m_vHideNode, proto.s_hidenode(), ",");
		//NS::StringUtil::splitString(armorType.m_vFemaleHideNode, proto.s_woman_hidenode(), ",");
		//NS::StringUtil::splitString(armorType.m_vHideEquip, proto.s_hidearmor(), ",");
		NS::XmlUtil::getAttribute(armorType.s_attach_node, xmlItem, "s_attach_node");// = proto.s_attach_node();
		//NS::StringUtil::splitString(armorType.m_postionRegion, proto.s_positionregion(), ",");
		//NS::StringUtil::splitString(armorType.m_actionEffect, proto.s_actioneffect(), ",");
		NS::XmlUtil::getAttribute(armorType.s_AttackSound, xmlItem, "s_AttackSound");// = proto.s_attacksound();
		NS::XmlUtil::getAttribute(armorType.s_UnderAttackSound, xmlItem, "s_UnderAttackSound");// = proto.s_underattacksound();

		auto getAnimationFilePath = [=](const tstring& strAnimationFilePath, std::map<int, bzbee::tstring>& animationMap)
		{
			if (!strAnimationFilePath.empty())
			{
				std::vector<tstring> vOut;
				NS::StringUtil::splitString(vOut, strAnimationFilePath, ",");
				for (int i = 0; i < (int)vOut.size(); ++i)
				{
                    std::vector<tstring> vTemp;
					NS::StringUtil::splitString(vTemp, vOut[i], "|");
					if ((int)vTemp.size() != 2) continue;
					int nArmorTypeId = 0;
					NS::StringUtil::stringToType(nArmorTypeId, vTemp[0]);
					animationMap.emplace(nArmorTypeId, vTemp[1]);					
				}
			}
		};
		
		//getAnimationFilePath(proto.s_animationfilepath(), armorType.m_AnimationMapMale);
		//getAnimationFilePath(proto.s_high_animationfilepath(), armorType.m_HighAnimationMapMale);
		//getAnimationFilePath(proto.s_woman_animationfilepath(), armorType.m_AnimationMapFemale);
		//getAnimationFilePath(proto.s_high_woman_animationfilepath(), armorType.m_HighAnimationMapFemale);

        NS::XmlUtil::getAttribute(armorType.m_isDoubleHandler, xmlItem, "n_DoubleHand");// = (proto.n_doublehand() == 1);
        NS::XmlUtil::getAttribute(armorType.m_isContinuousAttackWeapon, xmlItem, "n_isContinuousAttackWeapon");// = (proto.n_iscontinuousattackweapon() != 0);
        NS::XmlUtil::getAttribute(armorType.m_enchantmentType, xmlItem, "n_EnchantingId");// = proto.n_enchantingid();
		m_armorTypeMap.insert(std::make_pair(armorType.m_typeId, armorType));
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;
}

//bool ArmorCfg::parseArmorCfgTemplateBinary()
//{
//    config::ArmorCfgTemplateSet dataList;
//    if (!loadProtoConfig(dataList, "config/ArmorCfgTemplate.bin")) return false;
//
//    int itemSize = dataList.armorcfgtemplates_size();
//    for (int i = 0; i < itemSize; ++i)
//    {
//        auto& proto = dataList.armorcfgtemplates(i);
//        auto& armorTemplate = m_armorTemplateMap[proto.n_id()];
//
//        armorTemplate.m_templateId = proto.n_id();
//		armorTemplate.m_typeId = proto.n_armor_type();
//		armorTemplate.m_target_layer = proto.n_target_layer();
//
//		m_armorTemplateByTypeMap.insert(std::make_pair(armorTemplate.m_typeId, &armorTemplate));
//	}
//
//	return true;
//}
//
//bool ArmorCfg::parseArmorMagicAddBinary()
//{
//    config::ArmorMagicAddSet dataList;
//    if (!loadProtoConfig(dataList, "config/ArmorMagicAdd.bin")) return false;
//
//    int itemSize = dataList.armormagicadds_size();
//    for (int i = 0; i < itemSize; ++i)
//    {
//        auto& proto = dataList.armormagicadds(i);
//        auto& armorEnchantAttr = m_armorEnchantAttrMap[proto.n_id()];
//
//        armorEnchantAttr.m_id = proto.n_id();
//		armorEnchantAttr.m_enchantment_type = proto.n_magictype();
//		armorEnchantAttr.m_energy = proto.n_energy();
//		armorEnchantAttr.m_weight = proto.n_weight();
//		armorEnchantAttr.s_name = proto.s_name();
//		armorEnchantAttr.m_rate = proto.n_rate();
//
//		NS::StringList vOut;
//		NS::StringUtil::splitString(vOut, proto.s_attrext(), ";");
//		for (size_t i = 0; i < vOut.size(); ++i)
//		{
//			NS::IntegerList vTemp;
//			NS::StringUtil::splitString(vTemp, vOut[i], ",");
//
//            if (vTemp.size() == 2)
//            {
//                armorEnchantAttr.m_attr.insert(std::make_pair(vTemp[0], vTemp[1]));
//            }
//		}
//	}
//
//	return true;
//}
//
//bool ArmorCfg::parseArmorAttrExtBinary()
//{
//    config::ArmorAttrExtSet dataList;
//    if (!loadProtoConfig(dataList, "config/ArmorAttrExt.bin")) return false;
//
//    int itemSize = dataList.armorattrexts_size();
//    for (int i = 0; i < itemSize; ++i)
//    {
//        auto& proto = dataList.armorattrexts(i);
//        auto& armorAttrExt = m_armorAttrExtMap[proto.n_id()];
//
//        armorAttrExt.m_id = proto.n_id();
//		armorAttrExt.min = proto.n_countmin();
//		armorAttrExt.max = proto.n_countmax();
//		armorAttrExt.rateMin = proto.n_ratemin();
//		armorAttrExt.rateMax = proto.n_ratemax();
//
//		NS::StringList vOut;
//		NS::StringUtil::splitString(vOut, proto.s_attrext(), ";");
//		for (size_t i = 0; i < vOut.size(); ++i)
//		{
//			NS::IntegerList vTemp;
//			NS::StringUtil::splitString(vTemp, vOut[i], ",");
//            if (vTemp.size() == 2)
//            {
//                armorAttrExt.m_attr.insert(std::make_pair(vTemp[0], vTemp[1]));
//            }
//		}
//	}
//
//	return true;
//}
//
//bool ArmorCfg::parseArmorTypeGroupBinary()
//{
//    config::ArmorTypeGroupSet dataList;
//    if (!loadProtoConfig(dataList, "config/ArmorTypeGroup.bin")) return false;
//
//    int itemSize = dataList.armortypegroups_size();
//    m_armorTypeGroupList.resize(itemSize);
//    for (int i = 0; i < itemSize; ++i)
//    {
//        auto& proto = dataList.armortypegroups(i);
//
//        auto& typeGroup = m_armorTypeGroupList[i];
//        NS::StringUtil::splitString(typeGroup, proto.n_typegroup(), ",");
//	}
//
//	return true;
//}
//
//bool ArmorCfg::parseArmorSpecialEffectCfgBinary()
//{
//    config::ArmorSpecialEffectCfgSet dataList;
//    if (!loadProtoConfig(dataList, "config/ArmorSpecialEffectCfg.bin")) return false;
//
//    int itemSize = dataList.armorspecialeffectcfgs_size();
//    for (int i = 0; i < itemSize; ++i)
//    {
//        auto& proto = dataList.armorspecialeffectcfgs(i);
//        auto& spEffect = m_mapArmorSpecialEffects[proto.n_id()];
//
//		spEffect.n_ID = proto.n_id();
//		spEffect.n_type = (ArmorSpecialEffect::EFFECT_ARMOR_TYPE)proto.n_type();
//		spEffect.n_value = proto.n_value();
//		spEffect.n_replace = (ArmorSpecialEffect::EFFECT_ARMOR_RULE)proto.n_replace();
//        NS::StringUtil::splitString(spEffect.v_armor_sub_types, proto.s_armor_sub_type(), ",");
//		spEffect.n_buff_id = proto.n_buff_id();
//	}
//
//	return true;
//}

void ArmorCfg::terminate()
{
	m_armorTypeMap.clear();
	m_armorTemplateMap.clear();
	m_armorTemplateByTypeMap.clear();
	m_armorEnchantAttrMap.clear();
	m_armorAttrExtMap.clear();
	m_armorTypeGroupList.clear();
	m_mapArmorSuit.clear();
	m_armorFogUnlockMap.clear();
	m_mapItemID2SuitID.clear();
	m_mapArmorSpecialEffects.clear();
	m_pos2ArmorTypes.clear();
}

const ArmorCfg::ArmorType* ArmorCfg::findArmorType(NS::uint typeId)
{
	auto it = m_armorTypeMap.find(typeId);
	return (it == m_armorTypeMap.end()) ? nullptr : &it->second;
}

const ArmorCfg::ArmorType* ArmorCfg::findFirstArmorByEquipSubType(bzbee::uint equipSubTpy)
{
	for (auto&& army : m_armorTypeMap)
	{
		if (army.second.m_equipSubType == equipSubTpy)
		{
			return &army.second;
		}
	}
	return nullptr;
}

const ArmorCfg::ArmorType* ArmorCfg::findArmorTypeByTemplateId(NS::uint templateId)
{
	const auto* armorTemplate = findArmorTemplate(templateId);
	if (armorTemplate) return findArmorType(armorTemplate->m_typeId);
	
	return nullptr;
}

const ArmorCfg::ArmorTemplate* ArmorCfg::findArmorTemplate(NS::uint templateId)
{
	auto it = m_armorTemplateMap.find(templateId);
	return (it == m_armorTemplateMap.end()) ? nullptr : &it->second;
}

const ArmorCfg::ArmorTemplate* ArmorCfg::findBuildingTemplate(int typeId, TargetLayer targetLayer)
{
	auto range = m_armorTemplateByTypeMap.equal_range(typeId);
	for (auto it = range.first; it != range.second; ++it)
	{
		const ArmorCfg::ArmorTemplate* pTemplate = (it->second) ? it->second : nullptr;
		if (pTemplate && it->first == typeId && targetLayer == pTemplate->m_target_layer)
		{
			return pTemplate;
		}
	}

	return nullptr;
}

const ArmorCfg::ArmorEnchantAttr* ArmorCfg::findArmorEnchantAttr(NS::uint enchantId)
{
	auto it = m_armorEnchantAttrMap.find(enchantId);
	return (it == m_armorEnchantAttrMap.end()) ? nullptr : &it->second;
}

const ArmorCfg::ArmorAttrExt* ArmorCfg::findArmorAttrExt(NS::uint itemId)
{
	auto it = m_armorAttrExtMap.find(itemId);
	return (it == m_armorAttrExtMap.end()) ? nullptr : &it->second;
}

int ArmorCfg::getEnchantmentEnergyMax(NS::uint enchantType)
{
	int nRet{};
	for (ArmorEnchantAttrMap::const_iterator it = m_armorEnchantAttrMap.begin(); it != m_armorEnchantAttrMap.end(); ++it)
	{
        if (it->second.m_enchantment_type == enchantType)
        {
            nRet = nRet > it->second.m_energy ? nRet : it->second.m_energy;
        }
	}

	return nRet;
}

int ArmorCfg::getEnchantmentEnergyMin(NS::uint enchantType)
{
	int nRet{};
	for (ArmorEnchantAttrMap::const_iterator it = m_armorEnchantAttrMap.begin(); it != m_armorEnchantAttrMap.end(); ++it)
	{
        if (it->second.m_enchantment_type == enchantType)
        {
            nRet = (nRet == 0 || nRet > it->second.m_energy) ? it->second.m_energy : nRet;
        }
	}

	return nRet;
}

bool ArmorCfg::isSameTypeGroup(int nType0, int nType1)
{
	if (nType0 == nType1) return true;
	std::vector<NS::IntegerList>::iterator it = m_armorTypeGroupList.begin();
	for (; it != m_armorTypeGroupList.end(); ++it)
	{
		if (std::find(it->begin(), it->end(), nType0) != it->end() && std::find(it->begin(), it->end(), nType1) != it->end())
			return true;
	}
	return false;
}

//void ArmorCfg::findHideEquipAndNode(const Role::EquipMap& equipMap, NS::IntegerList& hideEquip, NS::IntegerList& hideNode, NS::IntegerList& femaleHideNode, const Role::EquipMap& fullEquipMap)
//{
//	hideEquip.clear();
//	hideNode.clear();
//	femaleHideNode.clear();
//
//	// NOTE:造型武器与普通武器显隐单独控制
//	auto pWeapon = equipMap.find(MsgPB::EQUIP_PACKET_WEAPON) == equipMap.end() ? nullptr : equipMap.find(MsgPB::EQUIP_PACKET_WEAPON)->second;
//	if (pWeapon == nullptr)
//	{
//		pWeapon = fullEquipMap.find(MsgPB::EQUIP_PACKET_WEAPON) == fullEquipMap.end() ? nullptr : fullEquipMap.find(MsgPB::EQUIP_PACKET_WEAPON)->second;
//	}
//	auto pCosWeapon = equipMap.find(MsgPB::EQUIP_PACKET_COS_WEAPON) == equipMap.end() ? nullptr : equipMap.find(MsgPB::EQUIP_PACKET_COS_WEAPON)->second;
//	if (pWeapon == nullptr && pCosWeapon)
//	{
//		hideEquip.push_back(pCosWeapon->getEquipPos());
//	}
//	else if (pWeapon && pCosWeapon && pWeapon->getArmorCfg() && !pCosWeapon->isHide())
//	{
//		bool bSameType = isSameTypeGroup(pWeapon->getArmorCfg()->m_equipSubType, pCosWeapon->getArmorCfg()->m_equipSubType);
//		hideEquip.push_back(bSameType ? pWeapon->getEquipPos() : pCosWeapon->getEquipPos());
//	}
//
//	const PlayerAttrCfg::AttachInfoMap& pList = PlayerAttrCfg::getInstance().getAttachInfoList();
//	for (auto&& it : pList)
//	{
//		auto pItemData = equipMap.find(it.m_attachPos) == equipMap.end() ? nullptr : equipMap.find(it.m_attachPos)->second;
//		if (!pItemData || !pItemData->getArmorCfg() || std::find(hideEquip.begin(), hideEquip.end(), it.m_attachPos) != hideEquip.end()) continue;
//
//		if (pItemData->isHide())
//		{
//			hideEquip.push_back(pItemData->getEquipPos());
//			continue;
//		}
//
//		auto&& pArmor = pItemData->getArmorCfg();
//		if (!pArmor->m_vHideEquip.empty())
//			hideEquip.insert(hideEquip.end(), pArmor->m_vHideEquip.begin(), pArmor->m_vHideEquip.end());
//		else if (!it.m_vHideEquip.empty())
//			hideEquip.insert(hideEquip.end(), it.m_vHideEquip.begin(), it.m_vHideEquip.end());
//	}
//
//	for (auto&& it : pList)
//	{
//		if (std::find(hideEquip.begin(), hideEquip.end(), it.m_attachPos) != hideEquip.end()) continue;
//
//		auto pItemData = equipMap.find(it.m_attachPos) == equipMap.end() ? nullptr : equipMap.find(it.m_attachPos)->second;
//		if (!pItemData || !pItemData->getArmorCfg()) continue;
//
//		const ArmorCfg::ArmorType* pArmor = pItemData->getArmorCfg();
//		if (!pArmor->m_vHideNode.empty())
//			hideNode.insert(hideNode.end(), pArmor->m_vHideNode.begin(), pArmor->m_vHideNode.end());
//		else if (it.m_vHideNode.empty())
//			hideNode.insert(hideNode.end(), it.m_vHideNode.begin(), it.m_vHideNode.end());
//
//		if (!pArmor->m_vFemaleHideNode.empty())
//			femaleHideNode.insert(femaleHideNode.end(), pArmor->m_vFemaleHideNode.begin(), pArmor->m_vFemaleHideNode.end());
//		else if (it.m_vHideNode.empty())
//			femaleHideNode.insert(femaleHideNode.end(), it.m_vHideNode.begin(), it.m_vHideNode.end());
//	}
//}

int ArmorCfg::getCosEquipType(int equipType)
{
	if (equipType == MsgPB::EQUIP_TYPE::EQUIP_MELEE_WEQPON) return MsgPB::EQUIP_TYPE::EQUIP_MELEE_WEQPON_COS;
	else if (equipType == MsgPB::EQUIP_TYPE::EQUIP_RANGED_WEQPON) return MsgPB::EQUIP_TYPE::EQUIP_RANGED_WEQPON_COS;
	else if (equipType == MsgPB::EQUIP_TYPE::EQUIP_MAGIC_WEQPON) return MsgPB::EQUIP_TYPE::EQUIP_MAGIC_WEQPON_COS;
	else if (equipType == MsgPB::EQUIP_TYPE::EQUIP_THROWING_WEQPON) return MsgPB::EQUIP_TYPE::EQUIP_THROWING_WEQPON_COS;

	return MsgPB::EQUIP_TYPE::EQUIP_NULL;
}

int ArmorCfg::toEquipPacketPos(int cfgEquipPos)
{
	int equipPos = cfgEquipPos;
	if (equipPos > 6)
	{
		equipPos += 2;
	}

	return equipPos;
}

//是否是双手武器
bool ArmorCfg::isDoubleHand(int id)
{
	auto cfg = ArmorCfg::getInstance().findArmorType(id);
	if (cfg && cfg->m_isDoubleHandler)
	{
		return true;
	}
	return false;
}

void ArmorCfg::getRandomEquipments(std::unordered_map<int, std::shared_ptr<ItemData>>& equipments) const
{
	// classify equipments by position
	if (m_pos2ArmorTypes.empty())
	{
		for (const auto& typeId2ArmorType : m_armorTypeMap)
		{
			const auto& pos = typeId2ArmorType.second.m_equipPos;
			if ((pos < MsgPB::EQUIP_PACKET_HEAD || pos > MsgPB::EQUIP_PACKET_SHOE) && pos != MsgPB::EQUIP_PACKET_COS_WEAPON) { continue; }

			auto iter = m_pos2ArmorTypes.find(pos);
			if (iter == m_pos2ArmorTypes.end())
			{
				m_pos2ArmorTypes.emplace(pos, std::vector<const ArmorType*>{ &typeId2ArmorType.second });
			}
			else
			{
				iter->second.push_back(&typeId2ArmorType.second);
			}
		}
	}

	for (const auto& pos2ArmorTypes : m_pos2ArmorTypes)
	{
		if (pos2ArmorTypes.second.empty()) { continue; }
		auto idx = NS::RandomUtil::getRandomInt(0, pos2ArmorTypes.second.size() - 1);
        auto&& itemData = std::make_shared<ItemData>();
		if (!itemData->initWithItemId(pos2ArmorTypes.second[idx]->m_typeId)) { continue; }
		equipments.emplace(pos2ArmorTypes.first, itemData);
	}
}

const int ArmorCfg::getEquipPacketPos(const int emArmorPos)
{
	switch (emArmorPos)
	{
	case MsgPB::ARMOR_EQUIP_NULL: // 
		return MsgPB::EQUIP_PACKET_NULL;
	case MsgPB::ARMOR_EQUIP_HEAD: // 1帽
		return MsgPB::EQUIP_PACKET_HEAD;
	case MsgPB::ARMOR_EQUIP_CLOTHES:	// 2上衣
		return MsgPB::EQUIP_PACKET_CLOTHES;
	case MsgPB::ARMOR_EQUIP_PANTS:		// 3褲子
		return MsgPB::EQUIP_PACKET_PANTS;
	case MsgPB::ARMOR_EQUIP_BACK:		// 4背飾
		return MsgPB::EQUIP_PACKET_BACK;
	case MsgPB::ARMOR_EQUIP_SHOE:		// 5鞋
		return MsgPB::EQUIP_PACKET_SHOE;
	case MsgPB::ARMOR_EQUIP_AMULET:		// 6飾品1	
		return MsgPB::EQUIP_PACKET_AMULET1;
	case MsgPB::ARMOR_EQUIP_COS_HEAD:	// 7造型帽
		return MsgPB::EQUIP_PACKET_COS_HEAD;
	case MsgPB::ARMOR_EQUIP_COS_CLOTHES:// 8造型上衣
		return MsgPB::EQUIP_PACKET_COS_CLOTHES;
	case MsgPB::ARMOR_EQUIP_COS_PANTS:	// 9造型褲子
		return MsgPB::EQUIP_PACKET_COS_PANTS;
	case MsgPB::ARMOR_EQUIP_COS_BACK:	// 10造型背飾
		return MsgPB::EQUIP_PACKET_COS_BACK;
	case MsgPB::ARMOR_EQUIP_COS_SHOE:	// 11造型鞋
		return MsgPB::EQUIP_PACKET_COS_SHOE;
	case MsgPB::ARMOR_EQUIP_COS_WEAPON:	// 12造型武器
		return MsgPB::EQUIP_PACKET_COS_WEAPON;
	case MsgPB::ARMOR_EQUIP_COS_EFFECT:	// 13造型特效
		return MsgPB::EQUIP_PACKET_COS_EFFECT;
	case MsgPB::ARMOR_EQUIP_WEAPON:	//14武器
		return MsgPB::EQUIP_PACKET_WEAPON;
	case MsgPB::ARMOR_EQUIP_HORSE:	//15採集工具
		return MsgPB::EQUIP_PACKET_HORSE;
	//case MsgPB::ARMOR_EQUIP_FOLLOW_PET:	//16跟隨寵
		//return MsgPB::EQUIP_PACKET_FOLLOW_PET;
	}
	return MsgPB::EQUIP_PACKET_NULL;
}

/////////////////////////////////////////////////////////////////////////////
//LUA开放接口
//ArmorCfg* LUA(get_armorcfg)(void)
//{
//	return &ArmorCfg::getInstance();
//}
//
//LuaTable ArmorCfg::find_armor_type(int nTypeid)
//{
//	LuaTable luaTab{};
//
//	auto&& armorCfg = findArmorType(nTypeid);
//	if (armorCfg)
//	{
//		luaTab.set("typeId", armorCfg->m_typeId);
//		luaTab.set("motion", armorCfg->m_sMotion);
//		luaTab.set("3d_model", armorCfg->m_s3DModel);
//		luaTab.set("sub_3d_model", armorCfg->m_sSub3DModel);
//		luaTab.set("equipType", armorCfg->m_equipType);
//		luaTab.set("equipPos", armorCfg->m_equipPos);
//		luaTab.set("starRank", armorCfg->m_starRank);
//		luaTab.set("durable", armorCfg->n_Durable);
//		luaTab.set("atkRange", armorCfg->m_atkRange);
//		luaTab.set("atkSpeed", armorCfg->m_atkSpeed);
//		luaTab.set("atk_limit", armorCfg->n_Atk_Limit);
//		luaTab.set("gatherSpeed", armorCfg->f_GatherSpeed);
//		luaTab.set("resource_atk_dmg", armorCfg->m_ResourceAtkDmg);
//		luaTab.set("durable", armorCfg->m_durable);
//		luaTab.set("durableRate", armorCfg->m_durableRate);
//		luaTab.set("target_layer", armorCfg->n_target_layer);
//		luaTab.set("gatherLv", armorCfg->n_gatherLv);
//		luaTab.set("isDoubleHandler", armorCfg->m_isDoubleHandler);
//	}
//
//	return luaTab;
//}
//
////////////////////////////////////////////////////////////////////////////
//bool ArmorCfg::parseArmorSuitCfgBinary()
//{
//    config::ArmorSuitCfgSet dataList;
//    if (!loadProtoConfig(dataList, "config/ArmorSuitCfg.bin")) return false;
//
//    int itemSize = dataList.armorsuitcfgs_size();
//    for (int i = 0; i < itemSize; ++i)
//    {
//        auto& proto = dataList.armorsuitcfgs(i);
//        ArmorSuitInfo suitCfg;
//
//        suitCfg.n_id = proto.n_id();
//
//		NS::StringUtil::splitString(suitCfg.setItemID, proto.s_armorset(), ",");
//        if (suitCfg.setItemID.empty()) continue;
//
//		for (auto nItemID : suitCfg.setItemID)
//		{
//			m_mapItemID2SuitID.insert(std::make_pair(nItemID, suitCfg.n_id));
//		}
//
//        m_mapArmorSuit.emplace(suitCfg.n_id, suitCfg);
//	}
//
//	return true;
//}
//
//bool ArmorCfg::parseArmorFogUnlockBinary()
//{
//    config::ArmorFogUnlockSet dataList;
//    if (!loadProtoConfig(dataList, "config/ArmorFogUnlock.bin")) return false;
//
//    int itemSize = dataList.armorfogunlocks_size();
//    for (int i = 0; i < itemSize; ++i)
//    {
//        auto& proto = dataList.armorfogunlocks(i);
//        auto& fogUnlockCfg = m_armorFogUnlockMap[proto.n_id()];
//
//        fogUnlockCfg.n_id = proto.n_id();
//        fogUnlockCfg.n_unlock_radius = proto.n_unlock_radius();
//        fogUnlockCfg.n_unlock_angle = proto.n_unlock_angle();
//	}
//
//	return true;
//}

//////////////////////////////////////////////////////////////////////////
const ArmorCfg::ArmorSuitInfo* ArmorCfg::findArmorSuitCfg(const NS::int32 nItemID)
{
	const auto iterID = m_mapItemID2SuitID.find(nItemID);
	if (iterID != m_mapItemID2SuitID.cend())
	{
		const auto it = m_mapArmorSuit.find(iterID->second);
		return (it == m_mapArmorSuit.cend()) ? nullptr : &it->second;
	}

	return nullptr;
}

const ArmorCfg::ArmorFogUnlockInfo* ArmorCfg::findArmorFogUnlockCfg(const NS::int32 nItemId)
{
	const auto item = m_armorFogUnlockMap.find(nItemId);
	if (item != m_armorFogUnlockMap.cend())
	{
		return &item->second;
	}

	return nullptr;
}

const ArmorCfg::ArmorSpecialEffect* ArmorCfg::findArmorSpecialEffect(const NS::int32 nItemId) const
{
	const auto iter = m_mapArmorSpecialEffects.find(nItemId);
	if (iter != m_mapArmorSpecialEffects.cend())
	{
		return &iter->second;
	}

	return nullptr;
}

bool ArmorCfg::ArmorSpecialEffect::isEffective(int armorType) const
{
    return std::find(v_armor_sub_types.begin(), v_armor_sub_types.end(), armorType) != v_armor_sub_types.end();
}

bool ArmorCfg::ArmorType::isAttrIsStar(int effect_type)
{
    for (auto&& dataAttr : m_vecEffectAttr)
    {
        if (dataAttr.n_effect_type == effect_type)
        {
            return true;
        }
    }
    return false;
}
