/*!
 * \file CraftCfg.cpp
 *
 * \author xiaobowang;
 * \date 2017/02/17
 *
 *
 */
#include "stdafx.h"
#include "CraftCfg.h"
#include <XmlUtil.h>
#include <LogUtil.h>
#include <StringUtil.h>
#include <XmlUtil.h>

CraftCfg& CraftCfg::getInstance()
{
	static CraftCfg s_CraftCfg;
	return s_CraftCfg;
}

bool CraftCfg::initialize()
{

	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/Craft.bin")) return false;
	if (!parseCraftConfig(doc.RootElement())) return false;
	reloadMultiLanguage();

	return true;
}

void CraftCfg::terminate()
{
	m_craftMap.clear();
}

bool CraftCfg::parseCraftConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto pAttrXml = p->FirstChildElement("Attribute");
	while (pAttrXml)
	{
		std::string strKey;
		NS::XmlUtil::getAttribute(strKey, pAttrXml, "Name");
		if (strKey == "f_CashCompleteMulti")
		{
			NS::XmlUtil::getAttribute(m_cashCompleteMulti, pAttrXml, "Value");
		}
		else if (strKey == "f_CashCompletePow")
		{
			NS::XmlUtil::getAttribute(m_cashCompletePow, pAttrXml, "Value");
		}
		pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	}

	auto xmlItem = p->FirstChildElement("Record");
	while (xmlItem != nullptr)
	{
		//auto& proto = dataList.achieveevents(i);
		Craft craft;

		NS::XmlUtil::getAttribute(craft.m_id, xmlItem, "n_ID");// = proto.n_id();
		NS::XmlUtil::getAttribute(craft.m_lockId, xmlItem, "n_LockID");// = proto.n_lockid();
		NS::XmlUtil::getAttribute(craft.m_toolId, xmlItem, "n_ToolID");//= proto.n_toolid();
		NS::XmlUtil::getAttribute(craft.m_time, xmlItem, "n_Time");// = proto.n_time();
		NS::XmlUtil::getAttribute(craft.n_MaxCraftNum, xmlItem, "n_MaxCraftNum");
		NS::XmlUtil::getAttribute(craft.m_bigTypeId, xmlItem, "s_Icon");//= proto.n_typeid();
		NS::XmlUtil::getAttribute(craft.m_subTypeId, xmlItem, "n_SubTypeId");// = proto.n_subtypeid();
		NS::XmlUtil::getAttribute(craft.m_reviewItemId, xmlItem, "n_ReviewItemId");// = proto.n_reviewitemid();
		NS::XmlUtil::getAttribute(craft.m_resultType, xmlItem, "n_ResultType");// = (Craft::ResultType)proto.n_resulttype();
		NS::XmlUtil::getAttribute(craft.m_resultParam, xmlItem, "s_ResultParam");// = proto.s_resultparam();
		NS::XmlUtil::getAttribute(craft.m_resultNum, xmlItem, "n_ResultNum");// = proto.n_resultnum();

		std::string strCrafts;
		NS::XmlUtil::getAttribute(strCrafts, xmlItem, "s_Tools");
		NS::StringUtil::splitString(craft.m_vecCrafts, strCrafts, "|");

		for (int m = 0; m < 3; m++)
		{
			for (int j = 0; j < 3; j++)
			{
				std::string itemTpy = fmt::format("n_ItemType{0}{1}", m, j);
				std::string itemNum = fmt::format("n_ItemNum{0}{1}", m, j);
				std::string param = fmt::format("s_ItemParam{0}{1}", m, j);
				int nTpy = 0;
				int nNum = 0;
				std::string strParam;
				NS::XmlUtil::getAttribute(nTpy, xmlItem, itemTpy.c_str());
				NS::XmlUtil::getAttribute(nNum, xmlItem, itemNum.c_str());
				NS::XmlUtil::getAttribute(strParam, xmlItem, param.c_str());
				parseCraftStuff(craft, m*3 + j, (Craft::ItemType)nTpy, nNum, strParam);
				m_craftMap[craft.m_id] = craft;
			}
		}
		xmlItem = xmlItem->NextSiblingElement("Record");
	}

    return true;
}


void CraftCfg::getCraftList(std::vector<const Craft *>& craftListOut, int typeId, int subId)
{
	for (auto& cip : this->m_craftMap)
	{
		if (cip.second.m_bigTypeId == typeId && cip.second.m_subTypeId == subId)
		{
            craftListOut.push_back(&cip.second);

		}
	}
}

void CraftCfg::getCraftList(std::vector<const Craft *>& craftListOut, int typeId)
{
	for (auto& cip : this->m_craftMap)
	{
		if (cip.second.m_bigTypeId == typeId)
		{
            craftListOut.push_back(&cip.second);

		}
	}
}

void CraftCfg::getNpcCragtList(std::vector<const Craft*>& craftListOut, int typeId, int nLev)
{
	for (auto& cip : this->m_craftMap)
	{
		if (std::find(cip.second.m_vecCrafts.begin(), cip.second.m_vecCrafts.end(), 
			(NS::uint)typeId) != cip.second.m_vecCrafts.end())
		{
			craftListOut.push_back(&cip.second);
		}
	}
}

const CraftCfg::Craft* CraftCfg::getCraft(int craftId)
{
	if (this->m_craftMap.find(craftId) != m_craftMap.end())
	{
		return &this->m_craftMap[craftId];
	}

	return nullptr;
}

const CraftCfg::Craft* CraftCfg::getCraftByItemId(int itemId)
{
	for (auto& cip : m_craftMap)
	{
		if (cip.second.m_resultType == Craft::ResultType::ITEM_ID)
		{
			NS::tstring str;

			NS::StringUtil::typeToString(str, itemId);

			if (str == cip.second.m_resultParam)
			{
				return &cip.second;
			}
		}
	}

	return nullptr;
}

const CraftCfg::Craft::CraftStuff* CraftCfg::getCraftStuff(const Craft* pCraft, int r, int c)
{
	auto index = r * CRAFT_STUFF_ROW_NUM + c;

	if (pCraft->m_craftStuffMap.find(index) != pCraft->m_craftStuffMap.end())
	{
		return &(pCraft->m_craftStuffMap.at(index));
	}

	return nullptr;
}

bool CraftCfg::reloadMultiLanguage()
{
	//for (auto& itemMap : m_craftTypeMap)
	//{
	//	for (auto& item : itemMap.second)
	//	{
	//		item.second.m_typeName = _LC(item.second.m_typeName_untrans);
	//		item.second.m_subName = _LC(item.second.m_subName_untrans);
	//	}
	//}
	return true;
}

void CraftCfg::parseCraftStuff(Craft& craft, int index, Craft::ItemType type, int itemNum, const NS::tstring& itemParams)
{
	if (itemParams.empty() || itemParams == "0")
	{
		return;
	}
	auto& craftStuff = craft.m_craftStuffMap[index];
	craftStuff.m_itemType = type;
	craftStuff.m_needNum = itemNum;
	NS::StringUtil::stringToType(craftStuff.m_itemId, itemParams);
}

const ItemCfg::Item * CraftCfg::getCraftViewItem(const Craft * pCraft)
{
	if (pCraft->m_resultType == Craft::ResultType::ITEM_ID)
	{
		NS::uint itemId{};
		NS::StringUtil::stringToType(itemId, pCraft->m_resultParam);

		return ItemCfg::getInstance().getItemById(itemId);
	}
	else if (pCraft->m_resultType == Craft::ResultType::DROP_ID)
	{
		return ItemCfg::getInstance().getItemById(pCraft->m_reviewItemId);
	}

	return nullptr;
}

const CraftCfg::Craft* CraftCfg::getCarftByItemID(int nItemId)
{
	for (CraftMap::iterator it = m_craftMap.begin();it != m_craftMap.end(); it++)
	{
		Craft& info = it->second;
		if (it->second.m_resultType == Craft::ResultType::ITEM_ID)
		{
			if(atoi(it->second.m_resultParam.c_str()) == nItemId)
				return &info;
		}
		else if(it->second.m_resultType == Craft::ResultType::DROP_ID)
		{
			if (it->second.m_reviewItemId == nItemId)
			{
				return &info;
			}
		}
	}

	return nullptr;
}

