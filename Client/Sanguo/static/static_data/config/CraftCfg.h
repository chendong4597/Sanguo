/*!
 * \file CraftCfg.h
 *
 * \author xiaobowang
 * \date 2017/02/16
 *
 * 
 */
#pragma once

#include <map>
#include <vector>
#include "ItemCfg.h"

class CraftCfg
{

public:

	enum EM_CONST
	{
		CONST_MATERIAL_NUM = 9,					//合成樣板素材格數
		CONST_TIME_MAX = (60 * 60 * 24 * 7),	//最大合成時間
		CONST_ITEM_NUM = 999,					//最大素材數量
		CONST_COMPLETE_MAX = 99,				//最大完成數量
	}; // End of EM_CONST

	static const NS::uint CRAFT_STUFF_ROW_NUM = 3;
	static const NS::uint CRAFT_STUFF_COLUMN_NUM = 3;
	static const NS::uint TOOL_UPGRADE_STUFF_ROW_NUM = 1;
	static const NS::uint TOOL_UPGRADE_STUFF_COLUMN_NUM = 3;

	struct Craft
	{
		enum class ResultType : int
		{
			ITEM_ID = 0,
			DROP_ID
		};

		enum class ItemType : int
		{
			NONE = 0,
			ITEM_ID = 1,
			TAG = 2
		};

		struct CraftStuff{
			ItemType m_itemType{};
			NS::uint m_itemId{};
			NS::tstring m_tag{};
			NS::uint m_needNum{};
		};

		NS::uint m_id{};
		NS::uint m_lockId{};
		NS::uint m_toolId{};
		NS::uint m_time{};
		NS::uint n_MaxCraftNum{};
		//NS::tstring m_icon{};
		NS::uint m_bigTypeId{};
		NS::uint m_subTypeId{};
		NS::uint m_reviewItemId{};
		ResultType m_resultType{};
		NS::tstring m_resultParam{};
		NS::uint m_resultNum{};

		std::map<NS::uint, CraftStuff> m_craftStuffMap{};
		std::vector<NS::uint> m_vecCrafts;
	};

	//{craftId:Craft}
	using CraftMap = std::map<int, Craft>;

	static CraftCfg& getInstance();

	bool initialize();
	void terminate();
    bool reloadMultiLanguage();

	const CraftMap& getCraftMap()
	{
		return m_craftMap;
	}

	void getCraftList(std::vector<const Craft*>& craftListOut, int typeId, int subId);
	void getCraftList(std::vector<const Craft*>& craftListOut, int typeId);
	void getNpcCragtList(std::vector<const Craft*>& craftListOut, int typeId, int nLev);

	const Craft* getCraft(int craftId);

	const Craft* getCraftByItemId(int itemId);
	
	const float& getCashCompleteMulti()
	{
		return this->m_cashCompleteMulti;
	}

	const float& getCashCompletePow()
	{
		return this->m_cashCompletePow;
	}

	const Craft::CraftStuff* getCraftStuff(const Craft* pCraft, int r, int c);


	const ItemCfg::Item* getCraftViewItem(const Craft* pCraft);

	const Craft* getCarftByItemID(int nItemI);

private:
    bool parseCraftConfig(tinyxml2::XMLElement* pXmlRoot);
	void parseCraftStuff(Craft& craft, int index, Craft::ItemType type, int itemNum, const NS::tstring& itemParams);
private:
	CraftMap m_craftMap;
	float m_cashCompleteMulti{};
	float m_cashCompletePow{};
};
