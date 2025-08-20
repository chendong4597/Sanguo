/*!
 * \file AchieveCfg.h
 *
 * \author ChenBK;
 * \date 2017/10/10
 *
 * 
 */
#pragma once

#include "../common/AchieveDefine.h"
#include <functional>
#include <memory>
#include <array>
#include <map>
#include <unordered_map>
#include "tinyxml2.h"

class AchieveCfg
{
public:
	static const float ANT_TIME_1_GET_BUTTON;
	static const float ANT_TIME_1_ICON_GLOW;
	static const float ANT_TIME_1_POINT_PARTICLE;
	static const float ANT_TIME_1_PROGRESS_GLOW;
	static const float ANT_TIME_1_STAR_SPINE;

	struct EventInfo
	{
		//-------------------------------------------
		// data
		int m_id{};
		NS::tstring m_name{};
		int n_Type{};
		int n_BeginDetailID{};
		int n_Param{};
		int n_Distinguish{};
		int n_JudgeType{};
	};

	struct TitleInfo
	{
		//-------------------------------------------
		// data
		int m_id{};
		NS::tstring m_name{};
		int m_type{};
		int m_needPoints{};
		int m_eventId{};
		int m_groupId{};
		int m_achieveRewardId{};
		NS::tstring m_desc{};
	};

	enum GuidType
	{
		GuidType_None = 0,
		GuidType_Nav,
		GuidType_UI,
		GuidType_Npc,
		GuidType_Plant,
	};

	struct GuidInfo
	{
		int nGuidTpy{ 0 }; //1:寻路特定位置  2：打开界面  3：特定类型Npc 4:特定植物
		int nParam1{ 0 };
		int nParam2{ 0 };
		int nParam3{ 0 };
		int nParam4{ 0 };
	};

	struct DetailInfo
	{
		//-------------------------------------------
		// data
		int n_ID{};
		NS::tstring m_name;
        NS::tstring m_name_untrans;
        NS::tstring m_icon;
		int m_difficulty{};
		int m_level{};
		int n_PreAchieve{};
		int n_ScriptFuncMap{ 0 };
		int n_Point{};
		int n_Property{};
		int n_EventID{};
		int n_Condition{};
		int n_NextID{};
		int n_GroupID{};
		int n_AchieveRewardID{};
		int n_ClientTrigger{};
		int n_ClientReceiveReward{};
		int m_recommend{};
		int m_lobbyRecommend{};
		int n_AutoReward{};
		NS::tstring m_scriptFunction;
		NS::tstring m_desc;
        NS::tstring m_desc_untrans;

		int m_nNeedItem{ 0 };
		NS::tstring s_NeedTips;
		std::vector<GuidInfo> vecGuidInfo;
		std::vector<GuidInfo> vecPreGuidInfo;

        float m_value[2];
	};

	struct GroupInfo
	{
		//-------------------------------------------
		// data
		int m_id{};
		std::array<int, 10> DetailIDs{};
	};

	struct RewardInfo
	{
		//-------------------------------------------
		// data
		int m_id{};
		std::array<int, 3> m_rewardType{};
		std::array<int, 3> m_rewardParam1{};
		std::array<int, 3> m_rewardParam2{};
		std::array<int, 3> m_effectType{};
		std::array<int, 3> m_effectAttrref1{};
		std::array<int, 3> m_effectAttrref2{};
		std::array<int, 3> m_effectValue1{};
		std::array<int, 3> m_effectValue2{};
	};

	enum RewardType
	{
		None = 0,
		Item,
		Exp
	};

    typedef std::unordered_map<NS::uint, EventInfo>			EventMap;
	typedef std::unordered_map<NS::uint, TitleInfo>			TitleMap;
	typedef std::unordered_map<NS::I64, DetailInfo>		DetailMap;
	typedef std::unordered_map<NS::uint, GroupInfo>			GroupMap;
	typedef std::unordered_map<NS::uint, RewardInfo>		RewardMap;
	typedef std::unordered_map<NS::uint, NS::uint>			AchieveSetNumMap;	// 连续成就 Id容器

    typedef std::map<NS::int32, std::set<NS::int32>> AccuTypeDataMap;
    typedef std::map<ACCU_ACHIEVE_TYPE, AccuTypeDataMap> AccuMap;

public:
	static AchieveCfg& getInstance();

	bool initialize();
	void terminate();
    void reloadMultiLanguage();

	const EventInfo* findEventInfo(int id);
	const TitleInfo* findTitleInfo(int id);
	const DetailInfo* findDetailInfo(NS::I64 detailId);
	const GroupInfo* findGroupInfo(int id);
	const RewardInfo* findRewardInfo(int id);
	const GroupInfo* findGroupInfoByDetailId(NS::I64 detailId);
	const RewardInfo* findRewardInfoByDetailId(NS::I64 detailId);
	const EventInfo* findEventInfoByDetailId(NS::I64 detailId);
	void getAccuAchieveEventSet(ACCU_ACHIEVE_TYPE eType, NS::int32 nParam, std::set<NS::int32>& setEvent);

	const NS::tstring findTitleInfoByPoints(int point);
	const NS::tstring findAchieveRewardTitle(NS::I64 detailId, int points);

	int getAchieveTotalStageNum(NS::I64 detailId);		// 连续成就 总数量
	int getAchieveCurrStage(NS::I64 detailId);			// 连续成就 当前阶段
	int getAchieveTotalSubNum(NS::I64 detailId);			// 成就集 Sub成就数理
	int getAchieveStagePercent(NS::I64 detailId);			// 完成阶段 百分比。
	int getAchieveStar(NS::I64 detailId);					// 成就星级
	NS::I64 getAchievePreviousId(NS::I64 currDetailId);	// 获取当前Id 的上一级Id;
	NS::I64 getAchieveNextId(NS::I64 currDetailId);		// 获取当前Id 的下一级Id;
	NS::I64 getBeginDetailIdByEventId(int id);			// 获取成就事件Begin DetailId
	int getLobbyReminderInterval() const { return m_lobbyReminderInterval; }; //  Lobby 成就效率提醒.
	int getNewbieReminderLev() const { return m_newbieReminderLev; }; // 新手成就 限定等级, 默认10
	const NS::tstring& getScriptFunction(NS::I64 detailId);		// 获取成就Script Function

	bool checkedHasAchieveStage(NS::I64 detailId);		// 检查是否 连续成就.
	bool checkedHasGroup(NS::I64 detailId);				// 检查是否 成就组.
	bool checkedIsTotalAchieve(NS::I64 detailId);			// 检查是否 总成就.
	bool checkedIsEndAchieve(NS::I64 detailId);			// 检查是否 是最终成就.
	bool checkedIsLobbyRecommend(NS::I64 detailId);		// 检查是否 Lobby推荐.
	
	void forEachAchieveEvent(const std::function<void(const EventInfo& data)>& fun);

	const DetailMap& GetDetailMap() { return m_detailMap; }

private:
	bool parseEventConfig(tinyxml2::XMLElement* pXmlRoot);
	bool parseTitleConfig(tinyxml2::XMLElement* pXmlRoot);
	bool parseDetailConfig(tinyxml2::XMLElement* pXmlRoot);
	bool parseGroupConfig(tinyxml2::XMLElement* pXmlRoot);
	bool parseRewardConfig(tinyxml2::XMLElement* pXmlRoot);
	bool parseAchieveAccumulateConfig(tinyxml2::XMLElement* pXmlRoot);

private:
	EventMap	m_eventMap;
	TitleMap	m_titleMap;
	DetailMap	m_detailMap;
	GroupMap	m_groupMap;
	RewardMap	m_rewardMap;

	int m_lobbyReminderInterval{};
	int m_newbieReminderLev{};

	AchieveSetNumMap m_achieveSetNumMap;
	AccuMap m_mapAccu;
};
