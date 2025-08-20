/*!
 * \file StoreCfg.h
 *
 * \author zjy
 * \date 10/18/2017
 *
 *
 */
#pragma once
#include <map>
#include <memory>
#include <unordered_map>
#include "ItemCfg.h"
#include "tinyxml2.h"

class StoreCfg
{
public:
	class StoreInfo
	{
	public:
		int m_buyNum{ 0 };
		int m_costNum{ 0 };
		int m_inventory{ 0 };
		int m_id{ 0 };
		int m_index{ 0 };
		int m_totalInventory;
		int m_menuId{ 0 };
		NS::I64 m_buyId{ 0 };
		NS::I64 m_costId{ 0 };
		void cloneData(const StoreCfg::StoreInfo* info, int inventory , int buy_item_count, int cost_itemid, int cost_item_count) {
			m_buyId = info->m_buyId;
			m_buyNum = buy_item_count;
			if (cost_itemid > 0)
			{
				m_costId = cost_itemid;
			}
			else
			{
				m_costId = info->m_costId;
			}
			if (cost_item_count > 0)
			{
				m_costNum = cost_item_count;
			}
			else
			{
				m_costNum = info->m_costNum;
			}

			m_inventory = inventory;
			m_id = info->m_id;
			m_index = info->m_index;
			m_menuId = info->m_menuId;
			m_totalInventory = info->m_totalInventory;
		}
	};
	class MenuIdInfo
	{
	public:
		int m_id;
		NS::tstring m_icon;
		NS::tstring m_name;
	};
	typedef std::shared_ptr<StoreCfg::StoreInfo> SharePtrStoreInfo;
	typedef std::vector <StoreCfg::StoreInfo> VecStore;
	typedef std::map<NS::I64, VecStore> MapStore;
	typedef std::map<int, int> MapId;
	typedef std::map<int, const ItemCfg::Item*> MapItemCfg;
	typedef std::map<int, std::vector<MenuIdInfo>> MapMenuId;
	typedef std::map<int, bzbee::tstring> MapName;
	typedef std::map<int, std::vector<int>> MapShowItems;

public:
	static StoreCfg& getInstance();
	~StoreCfg();

	bool initialize();
	void terminate();

	int findTypeById(bzbee::I64 id);
	std::vector<int> findMoneyTypeById(bzbee::I64 id);
	bool findDataById(int id, VecStore &outVec);
	bool findDataByIdAndBuyItemId(NS::I64 id , int itemId , const StoreCfg::StoreInfo *& out);
	int findIdByType(int type);
	int getSellMax() { return m_sellMax; };
	int findDiscount(int shopId);
	const ItemCfg::Item *getItemCfg(int id);
	const std::vector<StoreCfg::MenuIdInfo>& getMainMenu(int shopId);
	const bzbee::tstring& getShopName(int shopId);

	bool reloadMultiLanguage();

private:
	//bool parseStoreBinary();
	bool parseStoreConfig(tinyxml2::XMLElement* pXmlRoot);

private:
	int m_sellMax;
	int m_storeColNum{};
	MapId m_typeData;
	MapId m_discountData;
	MapStore m_data;
	MapItemCfg m_mapItemCfg{};
	MapMenuId m_mapMenuId{};
	MapName m_shopName{};
	MapName m_shopName_untrans{};
	MapShowItems m_showItems;
};




