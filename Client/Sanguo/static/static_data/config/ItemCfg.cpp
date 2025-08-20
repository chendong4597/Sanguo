/*!
* \file ItemCfg.cpp
*
* \author ChenBk
* \date 12/01/2016
*
*
*/
#include "stdafx.h"
#include "ItemCfg.h"
#include "GlobalCfg.h"
//#include "ConfigLoaderUtil.h"
//#include <StyleModule.h>
#include <Framework.h>
#include <msg/MsgGameDef.pb.h>
//#include <Item.pb.h>
//#include <Cooldown.pb.h>
//#include <ItemTag.pb.h>
//#include <ItemClassname.pb.h>
//#include <msg/MsgGameDef.pb.h>
#include <StringUtil.h>
#include <list>
#include <sstream>

#include "ArmorCfg.h"
#include "NpcTypeCfg.h"
#include "WeaponInfoTypeCfg.h"

ItemCfg& ItemCfg::getInstance()
{
	static ItemCfg s_itemCfg;
	return s_itemCfg;
}

bool ItemCfg::initializeCfg()
{
 //   if (!parseItemClassnameBinary()) return false;
 //   if (!parseItemTagBinary()) return false;
 //   if (!parseItemCfgBinary()) return false;
 //   if (!parseCooldownTypeBinary()) return false;
	//reloadMultiLanguage();
	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/Item.bin")) return false;

	if (!parseConfig(doc.RootElement())) {
		return false;
	}

	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/Material.bin")) return false;

	if (!parseMatConfig(doc.RootElement())) {
		return false;
	}

	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/Cooldown.bin")) return false;

	if (!parseCooldownTypeXml(doc.RootElement())) {
		return false;
	}

    return true;
}

void ItemCfg::terminate()
{
    m_itemMap.clear();

   // m_itemClassNameMap.clear();
    //SAFE_DELETE(m_itemClassNameList);

   // m_itemTagMap.clear();
   // SAFE_DELETE(m_itemTagList);

   m_cooldownMap.clear();
   // SAFE_DELETE(m_cooldownList);
}

bool ItemCfg::parseConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		int nId = 0;
		NS::XmlUtil::getAttribute(nId, xmlItem, "n_ID");
		if (nId == 0)
		{
			continue;
		}
		auto& item = m_itemMap[nId];
		NS::XmlUtil::getAttribute(item.m_itemId, xmlItem, "n_id");
		NS::XmlUtil::getAttribute(item.s_name, xmlItem, "s_name");
		NS::XmlUtil::getAttribute(item.s_icon, xmlItem, "s_icon");
		NS::XmlUtil::getAttribute(item.m_qualityGrade, xmlItem, "n_quality_grade");
		NS::XmlUtil::getAttribute(item.s_drop_model, xmlItem, "s_drop_model");
		NS::XmlUtil::getAttribute(item.s_hand_model, xmlItem, "s_hand_model");
		NS::XmlUtil::getAttribute(item.s_use_model, xmlItem, "s_use_model");
		NS::XmlUtil::getAttribute(item.n_use_model_trigger, xmlItem, "n_use_model_trigger");
		NS::XmlUtil::getAttribute(item.n_use_model_follow, xmlItem, "n_use_model_follow");
		NS::XmlUtil::getAttribute(item.s_use_play_animation, xmlItem, "s_use_play_animation");
		NS::XmlUtil::getAttribute(item.f_model_scale, xmlItem, "f_model_scale");
		NS::XmlUtil::getAttribute(item.n_use_lv, xmlItem, "n_use_lv");
		NS::XmlUtil::getAttribute(item.n_sub_type, xmlItem, "n_sub_type");
		NS::XmlUtil::getAttribute(item.m_maxHeap, xmlItem, "n_maxHeap");
		NS::XmlUtil::getAttribute(item.n_bag_class, xmlItem, "n_bag_class");
		NS::XmlUtil::getAttribute(item.n_class_name, xmlItem, "n_class_name");
		NS::XmlUtil::getAttribute(item.n_drop_id, xmlItem, "n_drop_id");
		NS::XmlUtil::getAttribute(item.n_cost, xmlItem, "n_cost");
		NS::XmlUtil::getAttribute(item.n_recyclingpricetype, xmlItem, "n_recyclingpricetype");
		NS::XmlUtil::getAttribute(item.n_recyclingpricevalue, xmlItem, "n_recyclingpricevalue");
		NS::XmlUtil::getAttribute(item.n_menu_id, xmlItem, "n_menu_id");
		NS::XmlUtil::getAttribute(item.n_canauction, xmlItem, "n_canauction");
		NS::XmlUtil::getAttribute(item.n_can_repair, xmlItem, "n_can_repair");
		NS::XmlUtil::getAttribute(item.n_useful, xmlItem, "n_useful");
		NS::XmlUtil::getAttribute(item.n_max_heap, xmlItem, "n_max_heap");
		NS::XmlUtil::getAttribute(item.n_cooldown_type, xmlItem, "n_cooldown_type");
		NS::XmlUtil::getAttribute(item.n_cooldown, xmlItem, "n_cooldown");
		NS::XmlUtil::getAttribute(item.n_show_info_type, xmlItem, "n_show_info_type");
		NS::XmlUtil::getAttribute(item.s_expried_time, xmlItem, "s_expried_time");
		NS::XmlUtil::getAttribute(item.n_countdown_time, xmlItem, "n_countdown_time");
		NS::XmlUtil::getAttribute(item.n_shortcut_type, xmlItem, "n_shortcut_type");
		NS::XmlUtil::getAttribute(item.n_can_trade, xmlItem, "n_can_trade");
		NS::XmlUtil::getAttribute(item.n_can_drop, xmlItem, "n_can_drop");
		NS::XmlUtil::getAttribute(item.n_consume, xmlItem, "n_consume");
		NS::XmlUtil::getAttribute(item.n_gift, xmlItem, "n_gift");
		NS::XmlUtil::getAttribute(item.m_tags, xmlItem, "m_tags");
		NS::XmlUtil::getAttribute(item.n_gift_max, xmlItem, "n_gift_max");
		NS::XmlUtil::getAttribute(item.s_tags, xmlItem, "s_tags");
		NS::XmlUtil::getAttribute(item.n_magic_injection_score, xmlItem, "n_magic_injection_score");
		NS::XmlUtil::getAttribute(item.n_selllimit, xmlItem, "n_selllimit");
		NS::XmlUtil::getAttribute(item.s_use_toast, xmlItem, "s_use_toast");
		NS::XmlUtil::getAttribute(item.n_use_effect_1, xmlItem, "n_use_effect_1");
		NS::XmlUtil::getAttribute(item.n_item_num, xmlItem, "n_item_num");
		NS::XmlUtil::getAttribute(item.n_item_tipnum, xmlItem, "n_item_tipnum");
		NS::XmlUtil::getAttribute(item.s_from_where, xmlItem, "s_from_where");
		NS::XmlUtil::getAttribute(item.n_is_npc_addition, xmlItem, "n_is_npc_addition") == 1 ? true : false;
        //if (!pStyleModule->findPieceInfo(item.m_icon))
        //{
        //    item.m_icon = "item.ps_no_pic";
        //}

        // 使用 效果, 目标, 值.
        item.m_vecUse.resize(3);
		NS::XmlUtil::getAttribute(item.m_vecUse[0].nEffect, xmlItem, "n_use_effect_1");// = proto.n_use_effect_1, xmlItem, "n_use_effect_1");//
		NS::XmlUtil::getAttribute(item.m_vecUse[0].nTarget , xmlItem, "n_use_target_1");//
		NS::XmlUtil::getAttribute(item.m_vecUse[0].nValue , xmlItem, "n_use_value_1");//

		NS::XmlUtil::getAttribute(item.m_vecUse[1].nEffect , xmlItem, "n_use_effect_2");//
		NS::XmlUtil::getAttribute(item.m_vecUse[1].nTarget , xmlItem, "n_use_target_2");//
		NS::XmlUtil::getAttribute(item.m_vecUse[1].nValue , xmlItem, "n_use_value_2");//

		NS::XmlUtil::getAttribute(item.m_vecUse[2].nEffect, xmlItem, "n_use_effect_3");//
		NS::XmlUtil::getAttribute(item.m_vecUse[2].nTarget , xmlItem, "n_use_target_3");//
		NS::XmlUtil::getAttribute(item.m_vecUse[2].nValue , xmlItem, "n_use_value_3");//

        // 可使用地图Id列表.
		std::string strUseMap;
		NS::XmlUtil::getAttribute(strUseMap, xmlItem, "s_useful_mapid");
		NS::StringUtil::splitString(item.m_usefulMapIds , strUseMap, ",");

        if (item.s_drop_model == "0")
        {
            item.s_drop_model = NS::EMPTY_STRING;
        }

		if (!item.s_icon.empty())
		{
			std::size_t pos = item.s_icon.find_first_of('.');
			if (pos != -1)
			{
				item.s_foder = item.s_icon.substr(0 , pos);
				item.s_icon = item.s_icon.substr(pos + 1);
			}
		}

		std::string s_desc_1;
		std::string s_desc_2;
		std::string s_desc_3;
		NS::XmlUtil::getAttribute(s_desc_1, xmlItem, "s_desc_1");
		NS::XmlUtil::getAttribute(s_desc_2, xmlItem, "s_desc_2");
		NS::XmlUtil::getAttribute(s_desc_3, xmlItem, "s_desc_3");

        if (s_desc_1 != NS::EMPTY_STRING)
        {
            item.m_vDes.push_back(s_desc_1);
        }

        if (s_desc_2 != NS::EMPTY_STRING)
        {
			item.m_vDes.push_back(s_desc_2);
        }

        if (s_desc_3 != NS::EMPTY_STRING)
        {
            item.m_vDes.push_back(s_desc_3);
        }

        //item.m_itemClassName_untrans = getItemClassName(item.m_itemClassNameId);

		//if (item.m_subType == MsgPB::ITEM_SUBTYPE::ITEM_SUBTYPE_NORMAL || item.m_subType == MsgPB::ITEM_SUBTYPE::ITEM_SUBTYPE_PLANT_SEED
		//	|| item.m_subType == MsgPB::ITEM_SUBTYPE::ITEM_SUBTYPE_PET_EGG)
		//{
		//	m_itemRefiningVec.push_back(item.m_itemId);
		//}
		//if (item.m_subType == MsgPB::ITEM_SUBTYPE::ITEM_SUBTYPE_FOOD || item.m_subType == MsgPB::ITEM_SUBTYPE::ITEM_SUBTYPE_NORMAL
		//	|| item.m_subType == MsgPB::ITEM_SUBTYPE::ITEM_SUBTYPE_PLANT_SEED)
		//{
		//	m_itemRefineFoodVec.push_back(item.m_itemId);
		//}
		
		xmlItem = xmlItem->NextSiblingElement("Record");
    }

    return true;
}

bool ItemCfg::parseMatConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		int nId = 0;
		NS::XmlUtil::getAttribute(nId, xmlItem, "n_ID");
		if (nId == 0)
		{
			continue;
		}
		S_Material item;
		NS::XmlUtil::getAttribute(item.n_ID, xmlItem, "n_ID");
		NS::XmlUtil::getAttribute(item.n_Type, xmlItem, "n_Type");
		NS::XmlUtil::getAttribute(item.s_icon, xmlItem, "s_icon");
		NS::XmlUtil::getAttribute(item.s_name, xmlItem, "s_name");
		NS::XmlUtil::getAttribute(item.s_desc, xmlItem, "s_desc");
		if (m_mapItemMaterial.find(item.n_Type) == m_mapItemMaterial.end()) {
			VEC_MATERIAL vecMat;
			m_mapItemMaterial[item.n_Type] = vecMat;
		}
		m_mapItemMaterial[item.n_Type].push_back(item);
		xmlItem = xmlItem->NextSiblingElement("Record");
	}
		
	return true;
}

bool ItemCfg::parseCooldownTypeXml(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		int nId = 0;
		int nTm = 0;
		NS::XmlUtil::getAttribute(nId, xmlItem, "n_ID");
		NS::XmlUtil::getAttribute(nTm, xmlItem, "n_cooldown");
		if (nId == 0 || nTm == 0) {
			xmlItem = xmlItem->NextSiblingElement("Record");
			continue;
		}
		
		m_cooldownMap[nId] = nTm;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

	return true;

	return true;
}
//
//bool ItemCfg::parseItemTagBinary()
//{
//    if (!m_itemTagList) m_itemTagList = new config::ItemTagSet();
//    if (!loadProtoConfig(*m_itemTagList, "config/ItemTag.bin")) return false;
//
//    int itemSize = m_itemTagList->itemtags_size();
//    for (int i = 0; i < itemSize; ++i)
//    {
//        auto& itemTag = m_itemTagList->itemtags(i);
//        m_itemTagMap.emplace(itemTag.n_id(), &itemTag);
//    }
//
//    return true;
//}
//
//bool ItemCfg::parseItemClassnameBinary()
//{
//    if (!m_itemClassNameList) m_itemClassNameList = new config::ItemClassnameSet();
//    if (!loadProtoConfig(*m_itemClassNameList, "config/ItemClassname.bin")) return false;
//
//    int itemSize = m_itemClassNameList->itemclassnames_size();
//    for (int i = 0; i < itemSize; ++i)
//    {
//        auto& itemClassname = m_itemClassNameList->itemclassnames(i);
//        m_itemClassNameMap.emplace(itemClassname.n_id(), &itemClassname);
//    }
//
//    return true;
//}

std::string ItemCfg::getItemClassName(int nItemId)
{
	const Item* pItem = getItemById(nItemId);
	if (!pItem || pItem->n_class_name == ITEM_CLASS_TPY_NONE)
	{
		return NS::EMPTY_STRING;
	}
	std::string str = fmt::format("UI_Local_Class_Main_{0}", pItem->n_class_name);
	std::string strClassName = _LC(str.c_str());
	if (pItem->n_class_name == ITEM_CLASS_TPY_EQUIP) {
		auto&& pArmorCfg = ArmorCfg::getInstance().findArmorType(nItemId);
		if (!pArmorCfg)
		{
			return strClassName;
		}
		strClassName += ".";
		str = fmt::format("UI_Local_Class_EQUIP_{0}", pArmorCfg->m_equipPos);
		strClassName += _LC(str.c_str());
		auto&& pWeaponCfg = WeaponInfoTypeCfg::getInstance().GetWeaponById(nItemId);
		if (!pWeaponCfg || pWeaponCfg->n_Type == 19) return strClassName;
		strClassName += ".";
		str = fmt::format("UI_local_trans_weapon_t_{0}", pWeaponCfg->n_Type);
		strClassName += _LC(str.c_str());
	}
	else if (pItem->n_class_name == ITEM_CLASS_TPY_NPC_SOUL) {
		auto&& pNpcCfg = NpcTypeCfg::getInstance().GetNpcType(nItemId);
		if (!pNpcCfg)
		{
			return strClassName;
		}
		strClassName += ".";
		str = fmt::format("UI_local_trans_process_{0}_{1}", pNpcCfg->n_Type, pNpcCfg->n_SubType);
		strClassName += _LC(str.c_str());
	}
	return strClassName;
}

const ItemCfg::Item* ItemCfg::getItemById(int itemId)
{
	auto pr = m_itemMap.find(itemId);
    if (pr != m_itemMap.end())
    {
        return &pr->second;
    }

    return nullptr;
}

const std::string& ItemCfg::getUseToast(int itemId)
{
	auto pr = m_itemMap.find(itemId);
    if (pr != m_itemMap.end())
    {
        return pr->second.s_use_toast;
    }

    return NS::EMPTY_STRING;
}

int ItemCfg::getCooldownType(int cdType)
{
	auto it = m_cooldownMap.find(cdType);
	if (it != m_cooldownMap.end())
	{
		return it->second;
	}

	return 0;
}

void ItemCfg::findItemMapByTag(ItemPtrMap& mapOut, const std::string& strTag, int nSubType /* = 0 */)
{
    mapOut.clear();

	for (const auto& pr : m_itemMap)
	{
        const auto& item = pr.second;
		if (checkItemTag(item.s_tags, strTag))
		{
			if (nSubType == 0 || item.n_sub_type == nSubType)
			{
                mapOut.emplace(item.m_itemId, &item);
			}
		}
	}
}

bool ItemCfg::checkItemTag(const std::string& strTags, const std::string& strTag)
{
	if (strTags.empty() || strTag.empty()) return false;

	std::vector<std::string> tagList;
	NS::StringUtil::splitString<std::vector<std::string>>(tagList, strTags, ",");

	for (auto& tag : tagList)
	{
		if (strTag == tag) return true;
	}

	return false;
}

bool ItemCfg::checkCanUse(int itemId)
{
	auto&& itemCfg = getItemById(itemId);

	return (itemCfg == nullptr) ? false : itemCfg->n_useful == 1;
}

//const config::ItemTagSet_ItemTag* ItemCfg::getItemTag(int tagId)
//{
//    auto pr = m_itemTagMap.find(tagId);
//	if (pr != m_itemTagMap.end())
//	{
//        return pr->second;
//	}
//
//	return nullptr;
//}

//void ItemCfg::getVecItem(std::vector<int>& vecOut, const std::string &name)
//{
//    vecOut.clear();
//
//	for (const auto& pr: m_itemMap)
//	{
//		auto& itemCfg = pr.second;
//		auto pos = itemCfg.m_name.find(name);
//		if (pos != std::string::npos)
//		{
//            vecOut.push_back(itemCfg.m_itemId);
//		}
//	}
//}

bool ItemCfg::reloadMultiLanguage()
{
    //for (auto& pr : m_itemMap)
    //{
    //    auto& item = pr.second;

    //    for (size_t i = 0; i < item.m_vDes_untrans.size(); ++i)
    //    {
    //        item.m_vDes[i] = _LC(item.m_vDes_untrans[i]);
    //    }

    //    item.m_itemClassName = _LC(item.m_itemClassName_untrans);
    //    item.m_name = _LC(item.m_name_untrans);
    //    item.m_use_toast = _LC(item.m_use_toast_untrans);
    //    item.m_fromWhere = _LC(item.m_fromWhere_untrans);
    //}

    return true;
}

std::string ItemCfg::Item::getNameByQgStyle() const
{
    std::string qgName;
    //switch (static_cast<MsgPB::QUALITY_GRADE>(m_qualityGrade))
    //{
    //case MsgPB::QUALITY_GRADE::QUALITY_GRADE_GREEN:
    //    qgName = fmt::format(GlobalCfg::ITEM_NAME_FORMAT_TEXT_GREEN, m_name);
    //    break;
    //case MsgPB::QUALITY_GRADE::QUALITY_GRADE_BLUE:
    //    qgName = fmt::format(GlobalCfg::ITEM_NAME_FORMAT_TEXT_BLUE, m_name);
    //    break;
    //case MsgPB::QUALITY_GRADE::QUALITY_GRADE_PURPLE:
    //    qgName = fmt::format(GlobalCfg::ITEM_NAME_FORMAT_TEXT_PURPLE, m_name);
    //    break;
    //case MsgPB::QUALITY_GRADE::QUALITY_GRADE_ORANGE:
    //    qgName = fmt::format(GlobalCfg::ITEM_NAME_FORMAT_TEXT_ORANGE, m_name);
    //    break;
    //case MsgPB::QUALITY_GRADE::QUALITY_GRADE_RED:
    //    qgName = fmt::format(GlobalCfg::ITEM_NAME_FORMAT_TEXT_RED, m_name);
    //    break;
    //case MsgPB::QUALITY_GRADE::QUALITY_GRADE_GRAY:
    //case MsgPB::QUALITY_GRADE::QUALITY_GRADE_WHITE:
    //case MsgPB::QUALITY_GRADE::QUALITY_GRADE_NONE:
    //default:
    //    qgName = fmt::format(GlobalCfg::ITEM_NAME_FORMAT_TEXT_WHITE, m_name);
    //    break;
    //}
    return qgName;
}

std::string ItemCfg::Item::getBgQgStyle() const
{
	std::string strBgStyle;// = GlobalCfg::QUALITY_GRADE_WHITE_BG;
    //switch (m_qualityGrade)
    //{
    //case MsgPB::QUALITY_GRADE::QUALITY_GRADE_GREEN:
    //    strBgStyle = GlobalCfg::QUALITY_GRADE_GREEN_BG;
    //    break;
    //case MsgPB::QUALITY_GRADE::QUALITY_GRADE_BLUE:
    //    strBgStyle = GlobalCfg::QUALITY_GRADE_BLUE_BG;
    //    break;
    //case MsgPB::QUALITY_GRADE::QUALITY_GRADE_PURPLE:
    //    strBgStyle = GlobalCfg::QUALITY_GRADE_PURPLE_BG;
    //    break;
    //case MsgPB::QUALITY_GRADE::QUALITY_GRADE_ORANGE:
    //    strBgStyle = GlobalCfg::QUALITY_GRADE_ORANGE_BG;
    //    break;
    //case MsgPB::QUALITY_GRADE::QUALITY_GRADE_RED:
    //    strBgStyle = GlobalCfg::QUALITY_GRADE_RED_BG;
    //    break;
    //case MsgPB::QUALITY_GRADE::QUALITY_GRADE_GRAY:
    //case MsgPB::QUALITY_GRADE::QUALITY_GRADE_WHITE:
    //case MsgPB::QUALITY_GRADE::QUALITY_GRADE_NONE:
    //default:
    //    break;
    //}
    return strBgStyle;
}

std::string ItemCfg::Item::getQgStyleColor() const
{
    std::string colorId;
    //switch (static_cast<MsgPB::QUALITY_GRADE>(m_qualityGrade))
    //{
    //case MsgPB::QUALITY_GRADE::QUALITY_GRADE_GREEN:
    //    colorId = GlobalCfg::QUALITY_GRADE_GREEN_COLOR;
    //    break;
    //case MsgPB::QUALITY_GRADE::QUALITY_GRADE_BLUE:
    //    colorId = GlobalCfg::QUALITY_GRADE_BLUE_COLOR;
    //    break;
    //case MsgPB::QUALITY_GRADE::QUALITY_GRADE_PURPLE:
    //    colorId = GlobalCfg::QUALITY_GRADE_PURPLE_COLOR;
    //    break;
    //case MsgPB::QUALITY_GRADE::QUALITY_GRADE_ORANGE:
    //    colorId = GlobalCfg::QUALITY_GRADE_ORANGE_COLOR;
    //    break;
    //case MsgPB::QUALITY_GRADE::QUALITY_GRADE_RED:
    //    colorId = GlobalCfg::QUALITY_GRADE_RED_COLOR;
    //    break;
    //case MsgPB::QUALITY_GRADE::QUALITY_GRADE_WHITE:
    //case MsgPB::QUALITY_GRADE::QUALITY_GRADE_NONE:
    //default:
    //    colorId = GlobalCfg::QUALITY_GRADE_WHITE_COLOR;
    //    break;
    //}
    return colorId;
}

bool ItemCfg::Item::canUse() const
{
    //switch (n_sub_type) {
    //case MsgPB::ITEM_SUBTYPE_SURVIVAL:
    //case MsgPB::ITEM_SUBTYPE_FOOD:
    //case MsgPB::ITEM_SUBTYPE_MEDICAMENT:
    //case MsgPB::ITEM_SUBTYPE_INTERACTIVE:
    //case MsgPB::ITEM_SUBTYPE_ENDLESS_POTION:
    //case MsgPB::ITEM_SUBTYPE_PET_FOOD:
    //case MsgPB::ITEM_SUBTYPE_PET_CAPTURE:
    //case MsgPB::ITEM_SUBTYPE_EXP_BALL:
    //case MsgPB::ITEM_SUBTYPE_ESSENCE:
    //    return true;
    //default:
    //    break;
    //} // End switch

    return false;
}

bool ItemCfg::Item::checkUsefulMap(int mapId) const
{
	// 没有限制.
	if (!m_usefulMapIds.size())
		return true;

	// 0 = all
	for (auto&& id: m_usefulMapIds)
	{
		if (id == 0 || id == mapId)
			return true;
	}
	
	return false;
}

ItemCfg::ITEM_USE_BY_SYSTEM ItemCfg::Item::GetUseBySystem() const
{
    switch (n_sub_type) {
    case MsgPB::ITEM_SUBTYPE_SURVIVAL:
        return ITEM_USE_BY_SYSTEM_SURVIVAL;
    case MsgPB::ITEM_SUBTYPE_FOOD:
        return ITEM_USE_BY_SYSTEM_FOOD;
    case MsgPB::ITEM_SUBTYPE_MEDICAMENT:
        return ITEM_USE_BY_SYSTEM_MEDICAMENT;
    case MsgPB::ITEM_SUBTYPE_INTERACTIVE:
        return ITEM_USE_BY_SYSTEM_INTERACTIVE;
    case MsgPB::ITEM_SUBTYPE_ENDLESS_POTION:
        return ITEM_USE_BY_ENDLESS_POTION;
    case MsgPB::ITEM_SUBTYPE_PET_FOOD:
        return ITEM_USE_BY_SYSTEM_PET_FOOD;
    case MsgPB::ITEM_SUBTYPE_PET_EGG:
        return ITEM_USE_BY_SYSTEM_PET_EGG;
    case MsgPB::ITEM_SUBTYPE_PET_CAPTURE:
        return ITEM_USE_BY_SYSTEM_PET_CAPTURE;
    default:
        break;
    } // End switch

    return ITEM_USE_BY_SYSTEM_ITEM;
}

bool ItemCfg::Item::isHightQg() const
{
    //return (m_qualityGrade != MsgPB::QUALITY_GRADE::QUALITY_GRADE_GRAY
    //    && m_qualityGrade >= MsgPB::QUALITY_GRADE::QUALITY_GRADE_PURPLE);
	return false;
}

const ItemCfg::S_Material* ItemCfg::getMaterial(int nID)
{
	auto&& iter = m_mapItemMaterial.begin();
	for (; iter != m_mapItemMaterial.end(); iter++)
	{
		for (int i = 0; i < iter->second.size(); i++)
		{
			if (iter->second[i].n_ID == nID)
			{
				return &iter->second[i];
			}
		}
	}
	return nullptr;
}
