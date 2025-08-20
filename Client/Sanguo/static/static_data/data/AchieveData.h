/*!
 * \file AchieveData.h
 *
 * \author ChenBK;
 * \date 2017/10/11
 *
 * 
 */
#pragma once
#include "role/Role.h"
#include "../config/AchieveCfg.h"
#include "../config/ItemCfg.h"
#include <msg/MsgAchieve.pb.h>
#include <list>


class AchieveData 
{
public:
	enum MainTypeOpt : int
	{
		Sync,
		AddUpdate,
		Remove
	};

	enum RewardType : int
	{
		None,
		Item,
	};

	enum MainType : int
	{
		Top = 0,
		Stepup,
		Collect,
		Challenge,
		Social,
		Event,
		Food,
		MainTypeMax,
	};

	enum SubType : int
	{
		SubTop = 0,
		SubStepup,
		SubCollect,
		SubChallenge,
		SubSocial,
		SubEvent,
		SubExplore,
		SubTypeMax,
	};

	enum class SortType : int
	{
		None					= 0,
		UuidAsc					= 1u << 0,
		ProgressDes				= 1u << 1,
		CompleteDateDsc			= 1u << 2,
	};

	// 完成进度, 从 大 -> 小
	bool sortProgressDes(const std::shared_ptr<MsgPB::MsgAchieveData> lhs, const std::shared_ptr<MsgPB::MsgAchieveData> rhs)
	{
		int lhsP = AchieveData::getInstance().getAchieveProgress(lhs->uuid());
		int rhsP = AchieveData::getInstance().getAchieveProgress(rhs->uuid());

		return lhsP > rhsP;
	}
	// 完成 日期, 从 大 -> 小
	bool sortCompleteDateDsc(const std::shared_ptr<MsgPB::MsgAchieveData> lhs, const std::shared_ptr<MsgPB::MsgAchieveData> rhs)
	{
		return lhs->complete_time() > rhs->complete_time();
	}
	// UUID , 从 小 -> 大
	bool sortUuidAsc(const std::shared_ptr<MsgPB::MsgAchieveData> lhs, const std::shared_ptr<MsgPB::MsgAchieveData> rhs)
	{
		return lhs->uuid() < rhs->uuid();
	}

	bool sortLobbyShow(const std::shared_ptr<MsgPB::MsgAchieveData> lhs, const std::shared_ptr<MsgPB::MsgAchieveData> rhs);

	typedef std::array<Effect, 3> AchieveEffects;
	typedef std::list<AchieveEffects> AchieveEffectsList;

	typedef std::vector<std::shared_ptr<MsgPB::MsgAchieveData>> AchieveDataVec;										// 成就数据
	typedef std::map<NS::I64, std::shared_ptr<MsgPB::MsgAchieveData>> AchieveDataMap;								// 所有成就数据。 eventId = 10000 设置为 总成就
	typedef std::map<MainType, AchieveDataMap> AchieveDataMainTypeMap;												// 分类成就数据。
	typedef AchieveDataMap TotalAchieveDataMap;																		// 总成就

	typedef std::map<NS::I64, std::shared_ptr<MsgPB::MsgAchieveData>> FinishAchieveDataMap;						// 完成的成就
	typedef std::map<NS::I64, std::shared_ptr<MsgPB::MsgAchieveData>> UndoneAchieveDataMap;						// 未完成的成就

	typedef std::pair<int /*itemid*/, int/*count*/>	RewardNotifyItem;												// 奖品Item
	typedef std::list<RewardNotifyItem> RewardNotifyItemList;														// 缓存领取Item奖品列表

	typedef std::map<NS::I64, std::shared_ptr<MsgPB::MsgAchieveData>> UndoneRecommendMap;							// 推荐的未完成成就.
	typedef std::map<int, std::tuple<bool, NS::I64>> UndoneLobbyRecommendMap;										// Lobby 推荐的未完成成就.

public:
	~AchieveData();

	static AchieveData& getInstance();
	void clearData();

	// 添加
	bool syncAchieveData(const MsgPB::MsgAchieveData& data);
	bool addUpdateAchieveData(const MsgPB::MsgAchieveData& data);
	// 删除
	bool removeAchieveDataById(NS::I64 detailId);
	// 更新
	void updateHasRewardTag();
	void updateAchieveStatusMap(const std::shared_ptr<MsgPB::MsgAchieveData> dataPtr, MainTypeOpt opt);
	void updateMainTypeMap(const std::shared_ptr<MsgPB::MsgAchieveData> dataPtr, MainTypeOpt opt);
	void updateTotalMap(const std::shared_ptr<MsgPB::MsgAchieveData> dataPtr, MainTypeOpt opt);
	void updateRecommendMap(const std::shared_ptr<MsgPB::MsgAchieveData> dataPtr);
	// 查找
	const std::shared_ptr<MsgPB::MsgAchieveData> findAchieveDataById(NS::I64 detailId);
	const AchieveDataMap& findAchieveDataByType(MainType type);
	// 获取
	int getAchieveCurrValue(NS::I64 detailId);				// 成就己经完成数量
	int getAchieveGroupDoneNum(NS::I64 detailId);				// 成就集当前阶段
	int getAchieveGroupAccomplishNum(NS::I64 detailId);		// 成就集己完成，未领取奖品
	int getAchieveTotalNum(NS::I64 detailId);					// 成就总数量
	int getAchieveSatge(NS::I64 detailId);					// 成就当前阶段
	int getTotalAchieveProgress();								// 获取当前 总成进度.
	int getAchieveGroupCurrStage(NS::I64 detailId);			// 成就集当前阶段
	int getAchieveProgress(NS::I64 detailId);					// 成就进度值
	NS::I64 getAchieveGroupNextSubId(NS::I64 detailId, NS::I64 currSubId); // 获取 成就集 下一个 Id.
	const NS::tstring getStringCompleteTime(NS::I64 detailId);
	
	const std::shared_ptr<MsgPB::MsgAchieveData> getCurrTotalAchieveData();		// 当前 总成就
	const std::shared_ptr<MsgPB::MsgAchieveData> getFrontTotalAchieveData(NS::I64 detailId); // 获取指定 总成就 前一个.
	const std::shared_ptr<MsgPB::MsgAchieveData> getBackTotalAchieveData(NS::I64 detailId); // 获取指定 总成就 前一个.
	const std::shared_ptr<MsgPB::MsgAchieveData> getTotalAchieveReward();
	const std::shared_ptr<MsgPB::MsgAchieveData> getTopAlmostGoData();		// 获取最接近完成成就.
	const std::shared_ptr<MsgPB::MsgAchieveData> getUndoneLobbyRecommend();	// 获取一条 Lobby 推荐
	// 获取 Map数据.
	const UndoneAchieveDataMap&		getUndoneAchieveDataMap();
	const FinishAchieveDataMap&		getFinishAchieveDataMap();
	const TotalAchieveDataMap&		getTotalAchieveDataMap();
	const UndoneRecommendMap&		getUndoneRecommendMap();
	const AchieveDataMap&			getAchieveDataMap();
	// 获取 排序数据.、
	const AchieveDataVec& getLobbyShowAchieveData();
	const AchieveDataVec& getUndoneAchieveData(SortType sort = SortType::None);
	const AchieveDataVec& getFinishAchieveData(SortType sort = SortType::None);
	const AchieveDataVec& getUndoneRecommendAchieveData(SortType sort = SortType::None);
	const AchieveDataVec& getAchieveDataByType(MainType type, SortType sort = SortType::None);
	void sortAchieveDataVec(AchieveDataVec& dataVec, SortType sort);
	// 通知 己完成
	void addAchieveNotifyId(NS::I64 detailId);				// 添加一个 己完成的成就ID通知.
	NS::I64 getNextAchieveNotifyId();							// UiAchievementsNotify 获取下一个需要显示 通知成就Id.
	// 通知 奖品
	void addRewardNotify(int rewardId);							// 添加 奖品通知
	void addRewardNotifyItem(int itemId, int count);			// 添加一个 奖品 Item 通知.
	RewardNotifyItem getNextRewardNotifyItem();					// 获取下一个需要显示 奖品通知.
	void clearRewardNotifyItem() { m_rewardNotifyItemList.clear(); }	// 领取奖品失败时, 清除奖品 通知列表.
	AchieveEffects getAttrRewardByDetailId(NS::I64 uuid);				// 获取成就Item奖品 属性.
	AchieveEffects getAttrRewardByRewardInfo(const AchieveCfg::RewardInfo* pRewardInfo);

	// 检查
	bool checkedAchieveFinish(NS::I64 detailId);				// 己完成 和 己领取奖品
	bool checkedAchieveUnfinish(NS::I64 detailId);			// 查找 未完成的成就
	bool checkedAchieveAccomplish(NS::I64 detailId);			// 己完成
	bool checkedAchieveLock(NS::I64 detailId);				// Lock
	bool checkedAchieveReceived(NS::I64 detailId);			// 己领取奖品
	bool checkedAchieveIsCurrent(NS::I64 detailId);			// 当前成就
	bool checkedHasRewardTag(MainType type);					// 有未领取奖品，标识小红点
	bool checkedAllHasRewardTag();								// 所有未领取奖品.
	// 计算属性
	void sumEffect(CEffectSum& effectSum);						// 计算总成就和食物 己完成就属性.
	void getAchieveEffectList(AchieveData::AchieveEffectsList& effectList); // 获取所有完成成就属性
	void getFoodEffectList(AchieveEffectsList& effectList);		// 获取所有己完成食物成就属性.
	void attachFoodAttrDes(ItemCfg::VEC_DES& desVec, int itemId); // 附加未完成食物属性到 Tip描述
	void getTotalEffectList(AchieveEffectsList& effectList);	// 获取所有己完成成就属性值.
	void getAttrMap(AttrMap& attrs, AchieveEffectsList& effectList); // 获取EffectList AttrMap

	std::string GetAchieveDetailDesc(NS::I64 uuid);

private:				
	void removeUndoneAchieve(NS::I64 detailId);
	void removeRecommendAchieve(NS::I64 detailId);

private:
	AchieveDataMap m_achieveDataMap;							// 所有成就 数据。
	AchieveDataVec m_returnAchieveDataVec;						// 返回 数据(非多线程使用)

	UndoneAchieveDataMap m_undoneAchieveMap;					// 未完成
	FinishAchieveDataMap m_finishAchieveMap;					// 己完成
	TotalAchieveDataMap m_totalAchieveDataMap;					// 总成就
	AchieveDataMainTypeMap m_achieveDataMainTypeMap;			// 成就分类
	UndoneRecommendMap m_undoneRecommendMap;					// 推荐的未完成成就.
	UndoneLobbyRecommendMap m_undoneLobbyRecommendMap;			// Lobby 推荐的未完成成就.

	std::array<bool, MainType::MainTypeMax> m_hasReward{};		// 有奖品 标识。
	std::list<NS::I64> m_achieveNotifyList;					// 己完成的成就通知列表.
	RewardNotifyItemList m_rewardNotifyItemList;				// 领取Item奖品通知列表.
};