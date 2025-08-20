/*!
* \file NpcBadgeCfg.h
* \date 08-30-2017 15:29:14
*
* \author LiQiangqiang
*/
#pragma once
#include <unordered_map>
#include <memory>
#include <vector>
#include <map>
#include "tinyxml2.h"

class NpcBadgeCfg
{
public:
	class NpcBadgeDef
	{
	public:
		int n_ID{};
		int n_NpcBaseID{};
		int n_BuildType{};
		int n_Mob{};
		int n_Level{};
		int n_Speed{};
		int n_CraftID{};
		int n_LimitWork{}; //最多种植地块
		int n_HP{};
		int n_Rare{};
		int n_ResCost{};
		int n_Gift{};
		int n_GiftProb{};
		int n_GiftProbMax{};
		int n_Time{};
		int n_CoinCost{};
		int n_Prosperous{}; //增加繁荣度
		int n_Rent{};// 提供租金
		int n_RatePlant{};// 增加种植效率
		int n_RateBuild{};// 增加建筑房屋效率
		int n_RateMine{};// 增加挖矿效率
		bzbee::tstring s_gameObjCfg;
		bzbee::tstring s_npcBadgeName;
		bzbee::tstring s_npcBadgeName_untrans;
		bzbee::tstring s_functionDesc;
		bzbee::tstring s_functionDesc_untrans;
		bzbee::tstring s_InterectUI;

		std::unordered_map<int, int> m_mapUpgradeComsumes;
		int n_ConditionForNpcCheckIn;

		int n_BaseNpcBadgeId;
		int n_UpgradeNpcBadgeId;
		bzbee::tstring s_ConfortAdditionDesc;
		bzbee::tstring s_NpcBadgeAvataFrameResourceId;
		bzbee::tstring s_NpcBadgeAvataResourceId;
		std::vector<NS::int32> m_availibleItemGifts;
	};

	typedef std::unordered_map<int, NpcBadgeDef> NpcBadgeDefMap;

	static const NS::uint TOOL_UPGRADE_STUFF_ROW_NUM = 1;
	static const NS::uint TOOL_UPGRADE_STUFF_COLUMN_NUM = 3;

public:
	static NpcBadgeCfg& getInstance();

	bool initialize();
	void terminate();

	const NpcBadgeCfg::NpcBadgeDef* findNpcBadgeDef(int id);
	const NpcBadgeDefMap& getNpcBadgeDefMap() const { return m_npcBadgeDefMap; };
	const NpcBadgeCfg::NpcBadgeDef* findNpcBadgeCfg(int badgeId,int level);

	//获取全等级Lv1-Lv5的礼物，舒适度，功能加持的描述
	void getGiftDescAllLevel(int nNpcBadgeId, std::vector<bzbee::tstring>& vecDescs);
	void getConfortAdditionDescAllLevel(int nNpcBadgeId,std::vector<bzbee::tstring>& vecDescs);
	void getLevelAdditiontDescAllLevel(int nNpcBadgeId,std::vector<bzbee::tstring>& vDescs);
	void getBadgeIdsAllLevel(int nNpcBadgeId,std::vector<int>& vecIds);

	bool reloadMultiLanguage();

private:
	bool parseNpcBadgeDefConfig(tinyxml2::XMLElement* pXmlRoot);

private:
	NpcBadgeDefMap m_npcBadgeDefMap;
};
