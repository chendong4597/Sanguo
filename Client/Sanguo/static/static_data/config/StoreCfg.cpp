/*!
 * \file StoreCfg.cpp
 *
 * \author zjy
 * \date 10/18/2017
 *
 *
 */
#include "stdafx.h"
#include "StoreCfg.h"
#include <utility>
#include "ItemCfg.h"
#include "XmlUtil.h"
#include "StringUtil.h"

StoreCfg::~StoreCfg()
{}

StoreCfg& StoreCfg::getInstance()
{
	static StoreCfg s_StoreCfg;
	return s_StoreCfg;
}

bool StoreCfg::initialize()
{
	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/Store.bin")) return false;

	parseStoreConfig(doc.RootElement());
    //reloadMultiLanguage();

    return true;
}

bool StoreCfg::parseStoreConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto pAttrXml = p->FirstChildElement("Attribute");
	while (pAttrXml)
	{
		std::string strKey;
		NS::XmlUtil::getAttribute(strKey, pAttrXml, "Name");
		if (strKey == "n_SellLimit")
		{
			NS::XmlUtil::getAttribute(m_sellMax, pAttrXml, "Value");
		}
		else if (strKey == "n_GoodsNum")
		{
			NS::XmlUtil::getAttribute(m_storeColNum, pAttrXml, "Value");
		}
		pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	}


	m_typeData.clear();
	m_data.clear();

	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{

		int id = 0;
		NS::XmlUtil::getAttribute(id, xmlItem, "n_ID");

		NS::XmlUtil::getAttribute(m_shopName[id], xmlItem, "s_Name");

        VecStore& vecInfo = m_data[id];

		for (int i = 0; i < m_storeColNum; i++)
		{
			StoreCfg::StoreInfo tmpData;
			std::string strKey = fmt::format("n_BuyItemID_{0}", i);
			NS::XmlUtil::getAttribute(tmpData.m_id, xmlItem, strKey.c_str());
			if (tmpData.m_id == 0)
			{
				continue;
			}
			tmpData.m_buyId = tmpData.m_id;
			tmpData.m_index = i;
			strKey = fmt::format("n_itemlist_{0}", i);
			NS::XmlUtil::getAttribute(tmpData.m_menuId, xmlItem, strKey.c_str());
			strKey = fmt::format("n_BuyItemCount_{0}", i);
			NS::XmlUtil::getAttribute(tmpData.m_buyNum, xmlItem, strKey.c_str());
			strKey = fmt::format("n_CostItemID_{0}", i);
			NS::XmlUtil::getAttribute(tmpData.m_costId, xmlItem, strKey.c_str());
			strKey = fmt::format("n_CostItemCount_{0}", i);
			NS::XmlUtil::getAttribute(tmpData.m_costNum, xmlItem, strKey.c_str());
			strKey = fmt::format("n_Inventory_{0}", i);
			NS::XmlUtil::getAttribute(tmpData.m_inventory, xmlItem, strKey.c_str());
			tmpData.m_totalInventory = tmpData.m_inventory;
			vecInfo.push_back(tmpData);
		}

		int nType = 0;
		NS::XmlUtil::getAttribute(nType, xmlItem, "n_Type");
        m_typeData.insert(MapId::value_type(id, nType));

        std::vector<int> vecMoneyTpy;
		std::string s_show_item;
		NS::XmlUtil::getAttribute(s_show_item, xmlItem, "s_Show_Item");
        bzbee::StringUtil::splitString(vecMoneyTpy, s_show_item, ",");
		m_showItems[id] = vecMoneyTpy;

		int nDisCopunt = 0;
		NS::XmlUtil::getAttribute(nDisCopunt, xmlItem, "n_Discount");
		m_discountData[id] = nDisCopunt;

		auto cfg = ItemCfg::getInstance().getItemById(nType);
        if (cfg)
        {
            m_mapItemCfg.insert(std::make_pair(nType, cfg));
        }

		std::vector<MenuIdInfo>& tmpVec = m_mapMenuId[id];

		for (int i = 0; i < 5; i++)
		{
			MenuIdInfo tmpData;
			std::string strKey = fmt::format("n_list_{0}", i + 1);
			NS::XmlUtil::getAttribute(tmpData.m_id, xmlItem, strKey.c_str());
			if (tmpData.m_id == 0)
			{
				continue;
			}
			strKey = fmt::format("s_list_{0}_icon", i + 1);
			NS::XmlUtil::getAttribute(tmpData.m_icon, xmlItem, strKey.c_str());
			strKey = fmt::format("s_list_{0}_name", i + 1);
			NS::XmlUtil::getAttribute(tmpData.m_name, xmlItem, strKey.c_str());
			tmpVec.push_back(tmpData);
		}
		xmlItem = xmlItem->NextSiblingElement("Record");
	}
	return true;
}

void StoreCfg::terminate()
{
	m_data.clear();
	m_typeData.clear();
	m_mapItemCfg.clear();
	m_mapMenuId.clear();

	m_shopName.clear();
	m_shopName_untrans.clear();
}

int StoreCfg::findDiscount(int shopId)
{
	return m_discountData[shopId];
}

//获取主菜单
const std::vector<StoreCfg::MenuIdInfo>& StoreCfg::getMainMenu(int shopId)
{
	static std::vector<StoreCfg::MenuIdInfo> s_emptyVec{};

	auto meuIter = m_mapMenuId.find(shopId);
	if (meuIter != m_mapMenuId.end())
	{
		return meuIter->second;
	}
	return s_emptyVec;
}

const ItemCfg::Item *StoreCfg::getItemCfg(int id)
{
	auto cfg = m_mapItemCfg.find(id);
	if (cfg != m_mapItemCfg.end())
	{
		return cfg->second;
	}
	return nullptr;
}

std::vector<int> StoreCfg::findMoneyTypeById(bzbee::I64 id)
{
	std::vector<int> vec;
	auto iter = m_showItems.find((int)id);
	if (iter != m_showItems.end()) return iter->second;
	return vec;
}

int StoreCfg::findTypeById(bzbee::I64 id)
{
	auto iter = m_typeData.find((int)id);
	if (iter != m_typeData.end()) return iter->second;
	return -1;
}

int StoreCfg::findIdByType(int type)
{
	for (auto it = m_typeData.begin(); it != m_typeData.end(); ++it)
	{
		if (it->second == type) return it->first;
	}
	return -1;
}

bool StoreCfg::findDataById(int id, VecStore &outVec)
{
	auto iter = m_data.find(id);
	if (iter == m_data.end()) return false;
	outVec = iter->second;
	return true;
}

bool StoreCfg::findDataByIdAndBuyItemId(NS::I64 id, int itemId, const StoreCfg::StoreInfo* &out)
{
	auto iter = m_data.find(id);
	if (iter == m_data.end()) return false;
	for (auto&& ite : iter->second)
	{
		if (ite.m_buyId == itemId)
		{
			out = &ite;
			return true;
		}
	}
	return false;
}

//获取商店名字
const bzbee::tstring& StoreCfg::getShopName(int shopId)
{
	auto iter = m_shopName.find(shopId);
	if (iter != m_shopName.end())
	{
		return iter->second;
	}

	return NS::EMPTY_STRING;
}

bool StoreCfg::reloadMultiLanguage()
{
	m_shopName.clear();
	for (auto iter = m_shopName_untrans.begin(); iter != m_shopName_untrans.end(); iter++)
	{
		m_shopName[iter->first] = _LC(iter->second);
	}
	return true;
}
