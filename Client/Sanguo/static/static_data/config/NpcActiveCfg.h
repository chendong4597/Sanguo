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

class NpcActiveCfg
{
public:
	class NpcActiveEffect {
	public:
		int n_effect_id{ 0 };
		int n_dialog_id{ 0 };
		NS::tstring desc;
		NS::tstring desc_untrans;
	};

	class NpcActiveDef
	{
	public:
		int n_ID{};	//可互动的mob(NPC) ID
		std::vector<NpcActiveEffect> m_npcActiveEffects;
		NS::tstring npcStatement;
		NS::tstring npcStatement_untrans;
		float f_Icon_OffsetY{0};
		int isPopCommonWin{ 1 };
	};

	class NpcComfort
	{
	public:
		int m_level;
		bzbee::tstring m_softIcon;
		bzbee::tstring m_des;
		bzbee::tstring m_des_untrans;
		bzbee::tstring m_gift;
		bzbee::tstring m_gift_untrans;
	};

	class ComfortConditions
	{
	public:
		int id;
		int level;
		int needComfort;
		int rent;
		float giftProb;
		int gift;
		int buffTarget;
		int buff;
		int needQuqlity;
		int needCount;
	};

	class BuildRoomActive
	{
	public:
		int id{ 0 };
		int npcId{ 0 };
		int level{ 0 };
		int activeId{ 0 };
		//int buff{ 0 };
		std::vector<int> vecBuff{};
		int skill{ 0 };
		int plantGrowSpeedup{ 0 };
		int enchantingProportion{ 0 };
		int synthesisMinus{ 0 };
		int forgingMinus{ 0 };
		int sellAwardCoinProportion{ 0 };
		int buyCostProportion{ 0 };
		int animal_pro_small{ 0 };
		int animal_hug_add{ 0 };
		int follow_dis{};
		int follow_max_dis{};
		int check_time{};
		int taskProportion{ 0 };
		int n_overRangeIsNeedPop{ 0 };
		NS::tstring headIcon{};
		NS::tstring halfIcon{};
		NS::tstring s_how_get_key{};//如何获得徽章的方法key
	};

	class NpcDialogActive
	{
	public:
		struct btnInfo
		{
			NS::tstring des{};
			NS::tstring des_untrans{};
			int textId{};
		};

		int id{};
		int npcActiveId{};
		std::vector<btnInfo> vecBtn{};
	};

	class GuildDialogActive
	{
	public:
		int id{};
		int type{};
		std::vector<NS::tstring> vecContent{};
		std::vector<NS::tstring> vecContent_untrans{};
	};

	typedef std::unordered_map<int, NpcActiveDef> NpcActiveDefMap;
	typedef std::unordered_map<int, NpcComfort> NpcComfortMap;
	typedef std::vector<ComfortConditions> ComfortConditionsVec;
	typedef std::vector<BuildRoomActive> BuildRoomActiveVec;
	typedef std::unordered_map<int, NpcDialogActive> NpcDialogActiveMap;
	typedef std::unordered_map<int, GuildDialogActive> GuildDialogMap;

public:
	static NpcActiveCfg& getInstance();

	bool initialize();
	void terminate();

	const NpcActiveCfg::NpcActiveDef* findNpcActiveDef(int id);
	bool isAMobActorAsANpc(int mobId);
	const NpcActiveCfg::NpcComfort *getComfortCfg(int level) const;
	NpcActiveCfg::ComfortConditions *getComfortCondtionsCfg(int id, int level);
	int getActiveId(int npcId, int badgeLevel);
	const NpcActiveCfg::NpcActiveDef* findNpcActive(int npcId, int badgeLevel);
	const BuildRoomActive* findBuildRoomActive(int npcId, int badgeLevel);
	NpcActiveCfg::NpcDialogActive* getNpcDialogActiveInfo(int npcActiveId);
	NpcActiveCfg::GuildDialogActive* getGuildDialogInfo(int id);

	bool reloadMultiLanguage();

private:
	bool parseNpcActiveConfig(tinyxml2::XMLElement* pXmlRoot);
	bool parseBuildRoomComfortRewardConfig(tinyxml2::XMLElement* pXmlRoot);
	bool parseBuildRoomComfortConditionsConfig(tinyxml2::XMLElement* pXmlRoot);
	bool parseBuildRoomActiveConfig(tinyxml2::XMLElement* pXmlRoot);
	//bool parseNpcDialogActiveBinary();
	//bool parseGuildDialogBinary();

private:
	NpcActiveDefMap m_npcActiveDefMap{};
	NpcComfortMap m_npcComfortMap{};
	ComfortConditionsVec m_comfortVec{};
	BuildRoomActiveVec m_buildRoomActiveVec{};
	NpcDialogActiveMap m_npcDialogActiveMap{};
	GuildDialogMap m_guildDialogMap{};
};
