/*!
 * \file SurvivalEventCfg.h
 *
 * \author ChenBK;
 * \date 2017/08/30
 *
 * 
 */
#pragma once

#include <baseutil.h>
#include "common/SurvivalDefine.h"
#include <map>
#include <memory>
#include <functional>
#include <unordered_map>

class SurvivalEventCfg
{
public:
	struct EventInfo
	{
		//-------------------------------------------
		// data
		int m_id{};
		int m_useItem{};
		S_SkipPassiveTrigger m_skipPassiveTrigger{};
		int m_area{};
		int m_specialKillNpc{};
		int m_specialKillMob{};
		int m_specialDigItem{};
		int m_passiveTrigger{};
		int m_moon{};
		int m_weather{};
		int m_alternateMomentBegin{};
		int m_alternateMomentEnd{};
		int m_playTime{};
		int m_mode{};
		float m_probability{};
		int m_firstFewDrop{};
		int m_maxMob{};
		int m_changeWaveType{};
		int m_targetInfo{};
		int m_winCondition{};
		int m_loseCondition{};
		int m_dropProcessId{};
		int m_onlyBirthMob{};		// (0:生存事件，1:僅刷怪)
		int m_nMapID{};
		int m_nMinLevel = -1;		// 角色等級下限，-1表示不判斷
		int m_nMaxLevel = -1;		// 角色等級上限，-1表示不判斷
		int m_speak{};
		int m_endSpeak{};
		int m_bossSpeak{};
		int m_mobId{};
		int m_type{};
		int m_priority{};
		int m_nKillMobID{};
		bool m_bIsAtmosphere{};
		bool m_bChkCrystal{ false };
		bool m_bCanRepairCrystal{ false };
		int m_wave{0};
		int m_lessMNum{ 0 };

		NS::tstring m_headimgres{};
		NS::tstring m_name{};
		NS::tstring m_name_untrans{};
		NS::tstring m_target{};
		NS::tstring m_target_untrans{};
		NS::tstring m_desWin{};
		NS::tstring m_desWin_untrans{};
		NS::tstring m_desLose{};
		NS::tstring m_desLose_untrans{};
		NS::StringList m_bgms{};

		NS::Ivec2List m_birthGrids{};
		NS::StringList m_birthEffect{};

		std::set<int> m_setSuperimpose{};

		bool CheckMoonTrigger() const;
	};

    struct EventMobPoolInfo
	{
		//-------------------------------------------
		// data
		int m_id{};
		std::vector<S_SurvivalEventMobPoolTemplate_Mob> m_mobs{};
	};

	struct EventMobListInfo
	{
		//-------------------------------------------
		// data
		int m_id{};
		int m_survivalEventID{};
		int m_wave{};
		int m_mobCount{};
		int m_changeWaveScore{};
		int m_changeWaveTime{};
		int m_cycle{};
		int m_bornType{};
		int m_minDistance{};
		int m_maxDistance{};
		int m_minDistanceY{};
		int m_maxDistanceY{};
		int m_occupancyX{};
		int m_occupancyY{};
		int m_leftPercentage{};
		int m_rightPercentage{};
		int m_mobRectA{};
		int m_mobRectB{};
		int m_mobRectC{};
		int m_mobRectD{};
		int m_mobRectE{};
		int m_mobRectF{};
		int m_mobRectG{};
		int m_mobRectH{};
		std::vector<S_SurvivalEventMobListTemplate_Mob> m_mobs{};
		std::vector<S_SurvivalEventMobListTemplate_Mob> m_cycleMobs{};
	};

	struct EventGuaranteeRewardsInfo
	{
		//-------------------------------------------
		// data
		int m_id{};
		std::vector<S_SurvivalEventGuaranteeRewardsTemplate_Item> m_items;
	};

	struct EventNpcInfo
	{
		//-------------------------------------------
		// data
		int m_id{};
		std::vector<S_SurvivalEventNpcTemplate_Npc> m_npcs;
	};

	struct EventRateupInfo
	{
		//-------------------------------------------
		// data
		int m_id{};
		int m_achievementID{};
		int m_level{};
		float m_correctionProbability{};
	};

	struct EventAttr
	{
		//-------------------------------------------
		// Attribute
		int m_readyTime{};
		int m_rewardTime{};
		int m_exitTime{};
		int m_nTrigCountLimit{};
		int m_nExeuntBuff{};
	};

	struct EventMobReplaceInfo
	{
		int m_id{};
		int m_survivalEventID{};
		int m_mobID{};
		int m_replaceMobID{};
	};

	struct EventDefenseDamageInfo
	{
		int n_defense_lvl2{};		// 等级(伤害递减用)
		int n_defense_damage{};		// 伤害(血量百分比)
		int n_defense_frequnce{};	// 频率CD(毫秒)
	};

	struct EventMobDamageInfo
	{
		int n_mob_id;
		int n_mob_lvl{};		// 等级(伤害递减用)
		int n_mob_damage{};		// 伤害(血量百分比)
		int n_mob_frequnce{};	// 频率CD(毫秒)
	};

	typedef std::unordered_map<bzbee::uint, EventInfo>					EventMap;
	typedef std::unordered_map<bzbee::uint, EventMobPoolInfo>		    EventMobPoolMap;
	typedef std::unordered_map<bzbee::uint, EventMobListInfo>		    EventMobListMap;
	typedef std::unordered_map<bzbee::uint, EventGuaranteeRewardsInfo>	EventGuaranteeRewardsMap;
	typedef std::unordered_map<bzbee::uint, EventNpcInfo>			    EventNpcMap;
	typedef std::unordered_map<bzbee::uint, EventRateupInfo>			EventRateupMap;
	typedef std::unordered_map<bzbee::uint, EventMobReplaceInfo>		EventMobReplaceMap;
	typedef std::unordered_map<bzbee::uint, EventDefenseDamageInfo>		EventDefenseDamageMap;
	typedef std::unordered_map<bzbee::uint, EventMobDamageInfo>			EventMobDamageMap;

public:

	static SurvivalEventCfg& getInstance();

	bool initialize();
	void terminate();

	const EventInfo* findEventInfo(int id);
	const EventGuaranteeRewardsInfo* findEventGuaranteeRewardsInfo(int id);
	const EventNpcInfo* findEventNpcInfo(int id);
	const EventRateupInfo* findEventRateupInfo(int id);
	const EventMobListInfo* findEventMobListInfo(int id);
	const EventMobPoolInfo* findEventMobPoolInfo(int id);
	const SurvivalEventCfg::EventDefenseDamageInfo* findEventDefenseDamageInfo(int nBuildType, int lev);
	const SurvivalEventCfg::EventMobDamageInfo* findEventMobDamageInfo(int nMobDef);

	void forEachSurvivalEvent(const std::function<void(const EventInfo&)>& fun);
	void forEachSurvivalEventMobList(const std::function<void(const EventMobListInfo&)>& fun);
	void forEachSurvivalEventMobReplace(const std::function<void(const EventMobReplaceInfo&)>& fun);

	const EventAttr& getEventAttr() const { return m_eventAttr; }

	bool reloadMultiLanguage();

private:
	//bool parseSurvivalEventBinary();
	//bool parseSurvivalEventMobPoolBinary();
	//bool parseSurvivalEventMobListBinary();
	//bool parseSurvivalEventGuaranteeRewardsBinary();
	//bool parseSurvivalEventNpcBinary();
	//bool parseSurvivalEventRateupBinary();
	//bool parseSurvivalEventMobReplaceBinary();
 //   bool parseSurvivalEventDefenseDamageBinary();
 //   bool parseSurvivalEventMobDamageBinary();

	bool parseSurvivalEventConfig(tinyxml2::XMLElement* pXmlRoot);
	bool parseSurvivalEventMobPoolConfig(tinyxml2::XMLElement* pXmlRoot);
	bool parseSurvivalEventMobListConfig(tinyxml2::XMLElement* pXmlRoot);
	//bool parseSurvivalEventGuaranteeRewardsConfig(tinyxml2::XMLElement* pXmlRoot); //没有数据
	//bool parseSurvivalEventNpcConfig(tinyxml2::XMLElement* pXmlRoot); //没有数据
	bool parseSurvivalEventRateupConfig(tinyxml2::XMLElement* pXmlRoot);
	bool parseSurvivalEventMobReplaceConfig(tinyxml2::XMLElement* pXmlRoot);
	bool parseSurvivalEventDefenseDamageConfig(tinyxml2::XMLElement* pXmlRoot);
	bool parseSurvivalEventMobDamageConfig(tinyxml2::XMLElement* pXmlRoot);

private:
	EventAttr m_eventAttr;
	EventMap m_eventMap;
	EventMobPoolMap m_eventMobPoolMap;
	EventMobListMap m_eventMobListMap;
	EventGuaranteeRewardsMap m_eventGuaranteeRewardsMap;
	EventNpcMap m_eventNpcMap;
	EventRateupMap m_eventRateupMap;
	EventMobReplaceMap m_eventMobReplaceMap;
	EventDefenseDamageMap m_mapDefenseDamage;
	EventMobDamageMap m_mapMobDamage;

};