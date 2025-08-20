/*!
 * \file AchieveData.cpp
 *
 * \author ChenBK;
 * \date 2017/10/11
 *
 *
 */
#include "stdafx.h"
#include "AchieveData.h"
#include "../config/PlayerAttrNameCfg.h"
#include "role/Player.h"
#include <TimeUtil.h>
#include <StringUtil.h>

using namespace NS;


AchieveData::~AchieveData()
{

}

AchieveData& AchieveData::getInstance()
{
	static AchieveData s_achieveData;
	return s_achieveData;
}

void AchieveData::clearData()
{
	m_achieveDataMap.clear();
	m_returnAchieveDataVec.clear();
	m_undoneAchieveMap.clear();
	m_finishAchieveMap.clear();
	m_totalAchieveDataMap.clear();
	m_achieveDataMainTypeMap.clear();
	m_undoneRecommendMap.clear();
	m_undoneLobbyRecommendMap.clear();

	m_achieveNotifyList.clear();
	m_rewardNotifyItemList.clear();
}

bool AchieveData::syncAchieveData(const MsgPB::MsgAchieveData& data)
{
	std::shared_ptr<MsgPB::MsgAchieveData> dataPtr = std::make_shared<MsgPB::MsgAchieveData>(data);

	m_achieveDataMap.emplace(dataPtr->uuid(), dataPtr);

	if (AchieveCfg::getInstance().checkedIsTotalAchieve(dataPtr->uuid()))
	{
		updateTotalMap(dataPtr, MainTypeOpt::Sync);
	}
	else if (!checkedAchieveLock(dataPtr->uuid()))
	{
		updateRecommendMap(dataPtr);
		updateAchieveStatusMap(dataPtr, MainTypeOpt::Sync);
		updateMainTypeMap(dataPtr, MainTypeOpt::Sync);
	}

	return true;
}

bool AchieveData::addUpdateAchieveData(const MsgPB::MsgAchieveData& data)
{
	std::shared_ptr<MsgPB::MsgAchieveData> dataPtr;
	if (m_achieveDataMap.find(data.uuid()) == m_achieveDataMap.end())
	{
		dataPtr = std::make_shared<MsgPB::MsgAchieveData>(data);
		m_achieveDataMap[dataPtr->uuid()] = dataPtr;
	}
	else
	{
		m_achieveDataMap[data.uuid()]->MergeFrom(data);
		dataPtr = m_achieveDataMap[data.uuid()];
	}

	if (AchieveCfg::getInstance().checkedIsTotalAchieve(data.uuid()))
	{
		updateTotalMap(dataPtr, MainTypeOpt::AddUpdate);
	}
	else if(!checkedAchieveLock(dataPtr->uuid()))
	{
		updateRecommendMap(dataPtr);
		updateAchieveStatusMap(dataPtr, MainTypeOpt::AddUpdate);
		updateMainTypeMap(dataPtr, MainTypeOpt::AddUpdate);
	}

	return true;
}

bool AchieveData::removeAchieveDataById(NS::I64 detailId)
{
	// 未完成 Map
	removeUndoneAchieve(detailId);
	// 己完成 Map
	{
		auto&& findIt = m_finishAchieveMap.find(detailId);
		if (findIt != m_finishAchieveMap.end())
			m_finishAchieveMap.erase(findIt);
	}
	// 总成就 Map
	{
		auto&& findIt = m_totalAchieveDataMap.find(detailId);
		if (findIt != m_totalAchieveDataMap.end())
			m_totalAchieveDataMap.erase(findIt);
	}
	// 推荐 Map
	{
		auto&& findIt = m_undoneRecommendMap.find(detailId);
		if (findIt != m_undoneRecommendMap.end())
			m_undoneRecommendMap.erase(findIt);
	}
	// 成就类型 Map
	{
		auto&& detailInfo = AchieveCfg::getInstance().findDetailInfo(detailId);
		if (!detailInfo)
			return false;
		auto&& eventInfo = AchieveCfg::getInstance().findEventInfo(detailInfo->n_EventID);
		if (!eventInfo)
			return false;

		auto&& findIt = m_achieveDataMainTypeMap[static_cast<MainType>(eventInfo->n_Type)].find(detailId);
		if (findIt != m_achieveDataMainTypeMap[static_cast<MainType>(eventInfo->n_Type)].end())
			m_achieveDataMainTypeMap[static_cast<MainType>(eventInfo->n_Type)].erase(findIt);
	}
	// 所有 Map
	{
		auto&& findIt = m_achieveDataMap.find(detailId);
		return (findIt == m_achieveDataMap.end()) ? false : (m_achieveDataMap.erase(findIt), true);
	}
}

void AchieveData::removeUndoneAchieve(NS::I64 detailId)
{
	auto&& findIt = m_undoneAchieveMap.find(detailId);
	if (findIt != m_undoneAchieveMap.end())
		m_undoneAchieveMap.erase(findIt);
}

void AchieveData::removeRecommendAchieve(NS::I64 detailId)
{
	auto&& findIt = m_undoneRecommendMap.find(detailId);
	if (findIt != m_undoneRecommendMap.end())
		m_undoneRecommendMap.erase(findIt);
}

void AchieveData::updateHasRewardTag()
{
	for (int type = MainType::Top; type < MainType::MainTypeMax; ++type)
	{
		if(type == MainType::Top)
		{
			m_hasReward[type] = false;
			for (auto it : m_totalAchieveDataMap)
			{
				if(it.second->uuid() % 5  == 0) // 总成就, 只显示 % 5 的奖励.
				{
					m_hasReward[type] = m_hasReward[type] || checkedAchieveAccomplish(it.second->uuid());
					if (m_hasReward[type])
					{
						break;
					}
				}
			}
		}
		else
		{
			m_hasReward[type] = false;
			for (auto it : m_achieveDataMainTypeMap[static_cast<MainType>(type)])
			{
				m_hasReward[type] = m_hasReward[type] || checkedAchieveAccomplish(it.second->uuid());
				if (m_hasReward[type])
				{
					break;
				}
			}
		}
	}
}

void AchieveData::updateAchieveStatusMap(const std::shared_ptr<MsgPB::MsgAchieveData> dataPtr, MainTypeOpt opt)
{
	if (!dataPtr)
		return;

	NS::I64 detailId = dataPtr->uuid();
	AchieveDataMap::iterator findIt;
	switch (opt)
	{
	case MainTypeOpt::Sync:
		if (checkedAchieveFinish(detailId))
		{
			removeUndoneAchieve(detailId);
			m_finishAchieveMap[detailId] = dataPtr;
		}
		else
		{
			m_undoneAchieveMap[detailId] = dataPtr;
		}
		break;
	case MainTypeOpt::AddUpdate:
		if (checkedAchieveFinish(detailId))
		{
			removeUndoneAchieve(detailId);
			m_finishAchieveMap[detailId] = dataPtr;
		}
		else
		{
			m_undoneAchieveMap[detailId] = dataPtr;
		}
		break;
	default:
		break;
	}
}

void AchieveData::updateRecommendMap(const std::shared_ptr<MsgPB::MsgAchieveData> dataPtr)
{
	if (!dataPtr)
		return;

	NS::I64 detailId = dataPtr->uuid();
	const AchieveCfg::DetailInfo* detailInfo = AchieveCfg::getInstance().findDetailInfo(detailId);
	if (!detailInfo)
		return;
	if (!detailInfo->m_recommend)
		return;

	// 推荐成就.
	if (checkedAchieveFinish(detailId))
		// 从推荐成就列表 删除
		removeRecommendAchieve(detailId);
	else
		// 推荐成就 添加
		m_undoneRecommendMap[detailId] = dataPtr;

	// Lobby推荐成就.
		if (detailInfo->m_lobbyRecommend)
	{
		// Lobby 推荐成就 己完成(并且己领取)
		if (checkedAchieveReceived(detailId))
			m_undoneLobbyRecommendMap[detailInfo->m_lobbyRecommend] = std::tuple<bool, NS::I64>(true, detailId);
		else
			m_undoneLobbyRecommendMap[detailInfo->m_lobbyRecommend] = std::tuple<bool, NS::I64>(false, detailId);
	}
}

void AchieveData::updateMainTypeMap(const std::shared_ptr<MsgPB::MsgAchieveData> dataPtr, MainTypeOpt opt)
{
	if (!dataPtr)
		return;

	NS::I64 detailId = dataPtr->uuid();
	const AchieveCfg::DetailInfo* detailInfo = AchieveCfg::getInstance().findDetailInfo(detailId);
	if (!detailInfo) return;
	const AchieveCfg::EventInfo* eventInfo = AchieveCfg::getInstance().findEventInfo(detailInfo->n_EventID);
	if (!eventInfo) return;

	AchieveDataMap::iterator findIt;
	switch (opt)
	{
	case MainTypeOpt::Sync:
		m_achieveDataMainTypeMap[static_cast<MainType>(eventInfo->n_Type)].emplace(detailId, dataPtr);
		break;
	case MainTypeOpt::AddUpdate:
		m_achieveDataMainTypeMap[static_cast<MainType>(eventInfo->n_Type)][detailId] = dataPtr;
		break;
	default:
		break;
	}
}

void AchieveData::updateTotalMap(const std::shared_ptr<MsgPB::MsgAchieveData> dataPtr, MainTypeOpt opt)
{
	if (!dataPtr)
		return;

	NS::I64 detailId = dataPtr->uuid();
	AchieveDataMap::iterator findIt;
	switch (opt)
	{
	case MainTypeOpt::Sync:
		m_totalAchieveDataMap[detailId] = dataPtr;
		break;
	case MainTypeOpt::AddUpdate:
		m_totalAchieveDataMap[detailId] = dataPtr;
		break;
	default:
		break;
	}

	// 刷新角色 属性
	const auto& self = PlayerMgr::getInstance().getSelf();
	if (self) self->calcAttr();
}

const std::shared_ptr<MsgPB::MsgAchieveData> AchieveData::findAchieveDataById(NS::I64 detailId)
{
	auto it = m_achieveDataMap.find(detailId);
	return (it == m_achieveDataMap.end()) ? nullptr : it->second;
}

void AchieveData::addAchieveNotifyId(NS::I64 detailId)
{
	m_achieveNotifyList.push_back(detailId);
}

NS::I64 AchieveData::getNextAchieveNotifyId()
{
	NS::I64 retId{};
	if (m_achieveNotifyList.size() > 0)
	{
		retId = m_achieveNotifyList.front();
		m_achieveNotifyList.pop_front();
	}

	return retId;
}

void AchieveData::addRewardNotify(int rewardId)
{
	auto&& rewardInfo = AchieveCfg::getInstance().findRewardInfo(rewardId);
	if (rewardInfo)
	{
		for (size_t i = 0; i < rewardInfo->m_rewardType.size(); ++i)
		{
			if (rewardInfo->m_rewardType[i] == AchieveData::RewardType::Item)
			{
				addRewardNotifyItem(rewardInfo->m_rewardParam1[i], rewardInfo->m_rewardParam2[i]);
			}
		}
	}
}

void AchieveData::addRewardNotifyItem(int itemId, int count)
{
	if (ItemCfg::getInstance().getItemById(itemId) == nullptr)
	{
		return;
	}

	m_rewardNotifyItemList.emplace_back(itemId, count);
}

AchieveData::RewardNotifyItem AchieveData::getNextRewardNotifyItem()
{
	AchieveData::RewardNotifyItem retItem{};
	if (m_rewardNotifyItemList.size() > 0)
	{
		retItem = m_rewardNotifyItemList.front();
		m_rewardNotifyItemList.pop_front();
	}

	return retItem;
}

const std::shared_ptr<MsgPB::MsgAchieveData> AchieveData::getCurrTotalAchieveData()
{
	for (auto it : m_totalAchieveDataMap)
	{
		if (AchieveCfg::getInstance().checkedIsTotalAchieve(it.second->uuid()) && it.second->currently() == 1) // currently 是否為目前使用中成就(0:否，1:是)
		{
			return it.second;
		}
	}

	return nullptr;
}

const std::shared_ptr<MsgPB::MsgAchieveData> AchieveData::getFrontTotalAchieveData(NS::I64 detailId)
{
	TotalAchieveDataMap::iterator findIt = m_totalAchieveDataMap.find(detailId -1);
	return (findIt != m_totalAchieveDataMap.end()) ? findIt->second : nullptr;
}

const std::shared_ptr<MsgPB::MsgAchieveData> AchieveData::getBackTotalAchieveData(NS::I64 detailId)
{
	TotalAchieveDataMap::iterator findIt = m_totalAchieveDataMap.find(detailId +1);
	return (findIt != m_totalAchieveDataMap.end()) ? findIt->second : nullptr;
}

const std::shared_ptr<MsgPB::MsgAchieveData> AchieveData::getTotalAchieveReward()
{
	std::shared_ptr<MsgPB::MsgAchieveData> retData; // 总成就 未领取 奖品Id。
	for (auto it : m_totalAchieveDataMap)
	{
		if (checkedAchieveAccomplish(it.second->uuid()))
		{
			if (!retData || it.second->uuid() < retData->uuid()) // 最低 阶段奖品
			{
				retData = it.second;
			}
		}
	}

	return (retData) ? retData : nullptr;
}

const tstring AchieveData::getStringCompleteTime(NS::I64 detailId)
{
	const std::shared_ptr<MsgPB::MsgAchieveData>  findData = findAchieveDataById(detailId);
	if (!findData) return EMPTY_STRING;

	NS::tstring retStr;
	if (findData->complete_time() > 20000000000000)
	{
		NS::tstring tempStr;
		tempStr = fmt::format("{0}", findData->complete_time()); //complete_time = yyyyMMddhhmmss
		retStr = fmt::format("{0}-{1}-{2}", tempStr.substr(4, 2), tempStr.substr(6, 2), tempStr.substr(0, 4)); // MM-dd-YYYY
	}
	else
	{
		retStr = TimeUtil::timestampToStr(findData->complete_time(), true, "%m-%d-%Y");
	}

	return retStr;
}

const std::shared_ptr<MsgPB::MsgAchieveData> AchieveData::getTopAlmostGoData()
{
	auto&& sortUndone = getUndoneAchieveData(SortType::ProgressDes);

	return (sortUndone.size() > 0) ? sortUndone[0] : nullptr;
}

const std::shared_ptr<MsgPB::MsgAchieveData> AchieveData::getUndoneLobbyRecommend()
{
	std::shared_ptr<MsgPB::MsgAchieveData> retData;
	for (auto&& it : m_undoneLobbyRecommendMap)
	{
		if (!std::get<0>(it.second))	// 对己排过序的 Lobby推荐成就(序号 小->大), 取第一个.
		{
			retData = findAchieveDataById(std::get<1>(it.second));
			break;
		}
	}

	return (retData) ? retData : nullptr;
}

NS::I64 AchieveData::getAchieveGroupNextSubId(NS::I64 detailId, NS::I64 currSubId)
{
	const AchieveCfg::GroupInfo* groupInfo = AchieveCfg::getInstance().findGroupInfoByDetailId(detailId);
	if (!groupInfo) return 0;

	bool bfind = false;
	for (auto subId : groupInfo->DetailIDs)
	{
		if (bfind || currSubId == 0) return subId;
		if (subId > 0 && subId == currSubId) // 跳过己完成，并领取状品的成就。
		{
			bfind = true;
		}
	}

	return 0;
}

const AchieveData::AchieveDataMap& AchieveData::getAchieveDataMap()
{
	return m_achieveDataMap;
}

const AchieveData::UndoneAchieveDataMap& AchieveData::getUndoneAchieveDataMap()
{
	return m_undoneAchieveMap;
}

const AchieveData::FinishAchieveDataMap& AchieveData::getFinishAchieveDataMap()
{
	return m_finishAchieveMap;
}

const AchieveData::TotalAchieveDataMap& AchieveData::getTotalAchieveDataMap()
{
	return m_totalAchieveDataMap;
}

const AchieveData::UndoneRecommendMap& AchieveData::getUndoneRecommendMap()
{
	return m_undoneRecommendMap;
}

const AchieveData::AchieveDataMap& AchieveData::findAchieveDataByType(MainType type)
{
	return m_achieveDataMainTypeMap[type];
}

int AchieveData::getAchieveGroupCurrStage(NS::I64 detailId)
{
	if (!AchieveCfg::getInstance().checkedHasGroup(detailId)) return 0;

	int stage = 0;
	int doneSub = getAchieveGroupDoneNum(detailId);
	int accomSub = getAchieveGroupAccomplishNum(detailId);
	int totalSub = AchieveCfg::getInstance().getAchieveTotalSubNum(detailId);

	stage = doneSub - accomSub;
	return (stage < totalSub) ? stage : totalSub;
}

int AchieveData::getAchieveCurrValue(NS::I64 detailId)
{
	int currValue = 0;
	const AchieveCfg::DetailInfo* info = AchieveCfg::getInstance().findDetailInfo(detailId);
	const std::shared_ptr<MsgPB::MsgAchieveData>  data = findAchieveDataById(detailId);
	if (data && info)
	{
		currValue = (data->value() > info->n_Condition) ? info->n_Condition : data->value();  // 只显示达到的最大值
	}

	return currValue;
}

int AchieveData::getAchieveGroupDoneNum(NS::I64 detailId)
{
	int doneCount = 0;
	if (AchieveCfg::getInstance().checkedHasGroup(detailId))
	{
		const AchieveCfg::GroupInfo*  groupInfo = AchieveCfg::getInstance().findGroupInfoByDetailId(detailId);
		if (!groupInfo) return doneCount;

		for (auto subDetailId : groupInfo->DetailIDs)
		{
			if (checkedAchieveFinish(subDetailId))
			{
				++doneCount;
			}
		}
	}

	return doneCount;
}

int AchieveData::getAchieveGroupAccomplishNum(NS::I64 detailId)
{
	int accomplishCount = 0;
	if (AchieveCfg::getInstance().checkedHasGroup(detailId))
	{
		const AchieveCfg::GroupInfo*  groupInfo = AchieveCfg::getInstance().findGroupInfoByDetailId(detailId);
		if (!groupInfo) return accomplishCount;

		for (auto subDetailId : groupInfo->DetailIDs)
		{
			if (checkedAchieveAccomplish(subDetailId))
			{
				++accomplishCount;
			}
		}
	}

	return accomplishCount;
}

int AchieveData::getAchieveTotalNum(NS::I64 detailId)
{
	int totalNum = 0;
	if (AchieveCfg::getInstance().checkedHasGroup(detailId))
	{
		totalNum = AchieveCfg::getInstance().getAchieveTotalSubNum(detailId);
	}
	else
	{
		const AchieveCfg::DetailInfo* info = AchieveCfg::getInstance().findDetailInfo(detailId);
		if (info)
		{
			totalNum = info->n_Condition;
		}
	}

	return totalNum;
}

int AchieveData::getAchieveSatge(NS::I64 detailId)
{
	int currStage = 0;
	if (AchieveCfg::getInstance().checkedHasGroup(detailId))
	{
		int stage = getAchieveCurrValue(detailId) +1;
		int total = getAchieveTotalNum(detailId);
		currStage = (stage < total) ? stage : total;
	}
	else if (AchieveCfg::getInstance().checkedHasAchieveStage(detailId))
	{
		currStage = AchieveCfg::getInstance().getAchieveCurrStage(detailId);
	}
	else
	{
		currStage = 1;
	}

	return currStage;
}

int AchieveData::getTotalAchieveProgress()
{
	// Curr
	const std::shared_ptr<MsgPB::MsgAchieveData> pCurData = getCurrTotalAchieveData();
	if (!pCurData) return 0;
	const AchieveCfg::DetailInfo* pCurInfo = AchieveCfg::getInstance().findDetailInfo(pCurData->uuid());
	if (!pCurInfo) return 0;
	// Old
	const std::shared_ptr<MsgPB::MsgAchieveData> pOldData = getFrontTotalAchieveData(pCurData->uuid());
	if (!pOldData) return (pCurInfo->n_Condition > 0) ? pCurData->value() * 100 / pCurInfo->n_Condition : 0;
	const AchieveCfg::DetailInfo* pOldInfo = AchieveCfg::getInstance().findDetailInfo(pOldData->uuid());
	if (!pOldInfo) return 0;
	if ((pCurInfo->n_Condition -pOldInfo->n_Condition <= 0)) return 0;

	return (pCurData->value() - pOldInfo->n_Condition) * 100 / (pCurInfo->n_Condition - pOldInfo->n_Condition);
}

int AchieveData::getAchieveProgress(NS::I64 detailId)
{
	int totalNum = 0;
	int doneNum = 0;
	if (AchieveCfg::getInstance().checkedHasGroup(detailId))
	{
		totalNum = getAchieveTotalNum(detailId);
		doneNum = getAchieveGroupCurrStage(detailId);
	}
	else
	{
//		const AchieveCfg::DetailInfo* info = AchieveCfg::getInstance().findDetailInfo(detailId);
		totalNum = getAchieveTotalNum(detailId);
		doneNum = getAchieveCurrValue(detailId);
	}

	return (totalNum > 0) ? doneNum * 100 / totalNum : 0;
}

bool AchieveData::checkedAchieveFinish(NS::I64 detailId)
{
	return checkedAchieveAccomplish(detailId) || checkedAchieveReceived(detailId);
}

bool AchieveData::checkedAchieveUnfinish(NS::I64 detailId)
{
	UndoneAchieveDataMap::iterator it = m_undoneAchieveMap.find(detailId);

	return it != m_undoneAchieveMap.end();
}

bool AchieveData::checkedAchieveAccomplish(NS::I64 detailId)
{
	AchieveDataMap::iterator it = m_achieveDataMap.find(detailId);
	if (it != m_achieveDataMap.end())
	{
		if (it->second->status() == MsgPB::ACHIEVE_STATUS::ACHIEVE_STATUS_ACCOMPLISH)
		{
			return true;
		}
	}

	return false;
}

bool AchieveData::checkedAchieveLock(NS::I64 detailId)
{
	AchieveDataMap::iterator it = m_achieveDataMap.find(detailId);
	if (it != m_achieveDataMap.end())
	{
		if (it->second->status() == MsgPB::ACHIEVE_STATUS::ACHIEVE_STATUS_LOCK)
		{
			return true;
		}
	}

	return false;
}

bool AchieveData::checkedAchieveReceived(NS::I64 detailId)
{
	AchieveDataMap::iterator it = m_achieveDataMap.find(detailId);
	if (it != m_achieveDataMap.end())
	{
		if (it->second->status() == MsgPB::ACHIEVE_STATUS::ACHIEVE_STATUS_RECEIVED)
		{
			return true;
		}
	}

	return false;
}

bool AchieveData::checkedAchieveIsCurrent(NS::I64 detailId)
{
	AchieveDataMap::iterator it = m_achieveDataMap.find(detailId);
	if (it != m_achieveDataMap.end())
	{
		return it->second->currently() != 0;
	}

	return false;
}

bool AchieveData::checkedHasRewardTag(MainType type)
{
	return m_hasReward[type];
}

bool AchieveData::checkedAllHasRewardTag()
{
	bool bHasReward{ false };
	updateHasRewardTag();

	for (int type = AchieveData::Top; type < AchieveData::MainTypeMax; ++type)
	{
		bHasReward = checkedHasRewardTag(static_cast<AchieveData::MainType>(type)) || bHasReward;
		if(bHasReward)
		{
			break;
		}
	}

	return bHasReward;
}

void AchieveData::getAttrMap(AttrMap& attrs, AchieveEffectsList& effectList)
{
	auto&& self = PlayerMgr::getInstance().getSelf();
	if (!self)
		return;

	CEffectSum effectSum;
	for (const auto& effectArray : effectList)
	{
		for (const auto& effect : effectArray)
		{
			if (effect.type ==  MsgPB::ROLE_EFFECT_NULL)
				continue;

			effectSum.AddEffect(effect.type, effect.ref_values, 0);
		}
	}

	// 获取角色自身基础属性
	AttrMap calcBaseAttrs;
	if (auto && pSelf = PlayerMgr::getInstance().getSelf())
	{
		auto&& baseAttrs = pSelf->getPlayerAttrInfo();
		for (auto&& effect : effectSum.GetEffects())
		{
			auto&& eType = effect.first;
			auto&& findIt = baseAttrs.find((eType > 100) ? static_cast<MsgPB::ROLE_EFFECT_TYPE>(eType % 100) : eType);
			if (findIt !=  baseAttrs.end())
			{
				calcBaseAttrs.emplace(findIt->first, findIt->second);
			}
		}
	}

	recalculateByEffectSum(attrs, calcBaseAttrs, effectSum);

	// 扣除参与计算的基础值。
	for (auto&& base : calcBaseAttrs)
	{
		auto&& findIt = attrs.find(base.first);
		if (findIt != attrs.end())
		{
			findIt->second -= base.second;
		}
	}
}

void AchieveData::sumEffect(CEffectSum& effectSum)
{
	AchieveData::AchieveEffectsList effectList;
	// getFoodEffectList(effectList);
	// getTotalEffectList(effectList);
	getAchieveEffectList(effectList);

	// 统计
	for (const auto& effectArray : effectList)
	{
		for (const auto& effect : effectArray)
		{
			effectSum.AddEffect(effect.type, effect.ref_values, 0);
		}
	}
}

void AchieveData::getAchieveEffectList(AchieveData::AchieveEffectsList& effectList)
{
	// 与服务器同步成就属性计算
	for (auto&& data : m_achieveDataMap)
	{
		if (auto && achieveData = data.second)
		{
			if (achieveData->status() == ACHIEVE_STATUS_LOCK)
				continue;
			auto && achieveCfg = AchieveCfg::getInstance().findDetailInfo(achieveData->uuid());
			if (!achieveCfg)
				continue;
			if (0 == achieveCfg->n_AchieveRewardID)
				continue;
			if (achieveCfg->n_EventID != ACHIEVE_EVENT_TEMPLATE_ID_10000 && achieveData->status() != ACHIEVE_STATUS_RECEIVED)
				continue;
			auto && pRewardInfo = AchieveCfg::getInstance().findRewardInfoByDetailId(achieveData->uuid()); // 获取奖品列表
			if (!pRewardInfo)
				continue;

			effectList.push_back(getAttrRewardByRewardInfo(pRewardInfo));
		}
	}
}

void AchieveData::getFoodEffectList(AchieveData::AchieveEffectsList& effectList)
{
	// 己完成食物.
	auto&& pFoodAchieve = getAchieveDataByType(MainType::Food); // 当前总成就阶段.
	for (auto achieve : pFoodAchieve)	// 有带属于的奖品．
	{
		if (AchieveData::getInstance().checkedAchieveFinish(achieve->uuid()))
		{
			effectList.push_back(getAttrRewardByDetailId(achieve->uuid())); // 当前己完成的 食物成就.
		}
	}
}

void AchieveData::getTotalEffectList(AchieveData::AchieveEffectsList& effectList)
{
	// 己完成总成就.
	const AchieveData::TotalAchieveDataMap& pTotalAchieve = AchieveData::getInstance().getTotalAchieveDataMap(); // 当前总成就阶段.
	for (auto achieve : pTotalAchieve)	// 有带属于的奖品．
	{
		if (!achieve.second) continue;
		if (AchieveData::getInstance().checkedAchieveFinish(achieve.second->uuid()))
		{
			effectList.push_back(getAttrRewardByDetailId(achieve.second->uuid())); // 当前己完成的 总成就
		}
	}
}

AchieveData::AchieveEffects AchieveData::getAttrRewardByDetailId(NS::I64 uuid)
{
	return getAttrRewardByRewardInfo(AchieveCfg::getInstance().findRewardInfoByDetailId(uuid)); // 获取奖品列表
}

AchieveData::AchieveEffects AchieveData::getAttrRewardByRewardInfo(const AchieveCfg::RewardInfo* pRewardInfo)
{
	AchieveData::AchieveEffects achieveEffects;
	if (!pRewardInfo)
		return achieveEffects;

	for (int i = 0; i < 3; ++i)
	{
		Effect effect;
		effect.type = static_cast<MsgPB::ROLE_EFFECT_TYPE>(pRewardInfo->m_effectType[i]);
		AttrEffectValue attr;
		attr.attrref = static_cast<ATTR_REF>(pRewardInfo->m_effectAttrref1[i]);
		attr.value = pRewardInfo->m_effectValue1[i];
		effect.ref_values.push_back(attr);
		attr.attrref = static_cast<ATTR_REF>(pRewardInfo->m_effectAttrref2[i]);
		attr.value = pRewardInfo->m_effectValue2[i];
		effect.ref_values.push_back(attr);

		achieveEffects[i] = std::move(effect);
	}
	return achieveEffects;
}


bool AchieveData::sortLobbyShow(const std::shared_ptr<MsgPB::MsgAchieveData> lhs, const std::shared_ptr<MsgPB::MsgAchieveData> rhs)
{
	auto&& detailInfoL = AchieveCfg::getInstance().findDetailInfo(lhs->uuid());
	if (!detailInfoL)
		return false;
	auto&& detailInfoR = AchieveCfg::getInstance().findDetailInfo(rhs->uuid());
	if (!detailInfoR)
		return false;
	if (detailInfoL->m_lobbyRecommend != detailInfoR->m_lobbyRecommend)
		return detailInfoL->m_lobbyRecommend > detailInfoR->m_lobbyRecommend;

	return lhs->uuid() < rhs->uuid();
}

const AchieveData::AchieveDataVec& AchieveData::getLobbyShowAchieveData()
{
	m_returnAchieveDataVec.clear();

	for (auto&& it : m_undoneAchieveMap)
		m_returnAchieveDataVec.emplace_back(it.second);
	std::sort(m_returnAchieveDataVec.begin(), m_returnAchieveDataVec.end(), std::bind(&AchieveData::sortLobbyShow, this, std::placeholders::_1, std::placeholders::_2));

	return m_returnAchieveDataVec;
}

// Achieve Sort
const AchieveData::AchieveDataVec& AchieveData::getUndoneAchieveData(SortType sort)
{
	m_returnAchieveDataVec.clear();

	for (auto&& it : m_undoneAchieveMap)
		m_returnAchieveDataVec.emplace_back(it.second);

	// 排序
	sortAchieveDataVec(m_returnAchieveDataVec, sort);

	return m_returnAchieveDataVec;
}

const AchieveData::AchieveDataVec& AchieveData::getFinishAchieveData(SortType sort)
{
	m_returnAchieveDataVec.clear();

	for (auto&& it : m_finishAchieveMap)
		m_returnAchieveDataVec.emplace_back(it.second);

	// 排序
	sortAchieveDataVec(m_returnAchieveDataVec, sort);

	return m_returnAchieveDataVec;
}

const AchieveData::AchieveDataVec& AchieveData::getUndoneRecommendAchieveData(SortType sort)
{
	m_returnAchieveDataVec.clear();

	for (auto&& it : m_undoneRecommendMap)
		m_returnAchieveDataVec.emplace_back(it.second);

	// 排序
	sortAchieveDataVec(m_returnAchieveDataVec, sort);

	return m_returnAchieveDataVec;
}

const AchieveData::AchieveDataVec& AchieveData::getAchieveDataByType(MainType type, SortType sort)
{
	m_returnAchieveDataVec.clear();

	auto&& typeMap = findAchieveDataByType(type);
	for (auto&& it : typeMap)
		m_returnAchieveDataVec.emplace_back(it.second);

	// 排序
	sortAchieveDataVec(m_returnAchieveDataVec, sort);

	return m_returnAchieveDataVec;
}

void AchieveData::sortAchieveDataVec(AchieveDataVec& dataVec, SortType sort)
{
	switch (sort)  // 排序.
	{
	case AchieveData::SortType::UuidAsc:
		std::sort(dataVec.begin(), dataVec.end(), std::bind(&AchieveData::sortUuidAsc, this, std::placeholders::_1, std::placeholders::_2));
		break;
	case AchieveData::SortType::ProgressDes:
		std::sort(dataVec.begin(), dataVec.end(), std::bind(&AchieveData::sortProgressDes, this, std::placeholders::_1, std::placeholders::_2));
		break;
	case AchieveData::SortType::CompleteDateDsc:
		std::sort(dataVec.begin(), dataVec.end(), std::bind(&AchieveData::sortCompleteDateDsc, this, std::placeholders::_1, std::placeholders::_2));
		break;
	case AchieveData::SortType::None:
	default:
		break;
	}
}

// 附加未完成食物属性到 Tip描述
void AchieveData::attachFoodAttrDes(ItemCfg::VEC_DES& desVec, int itemId)
{
	AttrMap attrs;
	AchieveData::AchieveEffectsList effectList;

	std::set<int32> setEventID;
	AchieveCfg::getInstance().getAccuAchieveEventSet(ACCU_USE_ITEM, itemId, setEventID);

	// 收集食物未完成属性
	for (auto nEventID : setEventID)
	{
		if (const auto pEventCfg = AchieveCfg::getInstance().findEventInfo(nEventID))
		{
			int uuid = pEventCfg->n_BeginDetailID;
			if (AchieveData::getInstance().checkedAchieveUnfinish(uuid))
			{
				effectList.push_back(getAttrRewardByDetailId(uuid));
			}
		}
	}

	// 附加属性
	if (!effectList.empty())
	{
		AchieveData::getInstance().getAttrMap(attrs, effectList);

		// 附加食物属性描述
		for (auto it : attrs)
		{
			int eType = it.first;
			int val = it.second;
			if (auto&& pAttrName = PlayerAttrNameCfg::getInstance().findPlayerAttrNameType(eType))
			{
				NS::tstring strText = fmt::format("<font color='ui.cs_green'>{0}</font>", _LC("CLI_ItemTipBox-First Eating food: {0}+{1}"));
				desVec.push_back(fmt::format(strText, pAttrName->m_Name, val));
			}
		}
	}
}

std::string AchieveData::GetAchieveDetailDesc(NS::I64 uuid)
{
	auto&& detailInfo = AchieveCfg::getInstance().findDetailInfo(uuid);
	if (!detailInfo) return "";

	const std::shared_ptr<MsgPB::MsgAchieveData> ptrData = findAchieveDataById(uuid);

	auto&& self = PlayerMgr::getInstance().getSelf();

	std::string strDesc;
	if (detailInfo->n_EventID < 30100)
	{
		if (ptrData)
		{
			strDesc = fmt::format(_LC(detailInfo->m_desc.c_str()), ptrData->value(), detailInfo->n_Condition);
		}
		else {
			strDesc = fmt::format(_LC(detailInfo->m_desc.c_str()), 0, detailInfo->n_Condition);
		}
	} 
	else if (detailInfo->n_EventID >= 30510 && detailInfo->n_EventID <= 30512)
	{
		strDesc = fmt::format(_LC(detailInfo->m_desc.c_str()), detailInfo->n_Condition);
	}
	else {
		strDesc = _LC(detailInfo->m_desc);
	}
	return strDesc;
}

NS::I64 LUA(get_achivedata_cur_id)()
{
	auto data = AchieveData::getInstance().getUndoneLobbyRecommend();
	if (data)
	{
		return data->uuid();
	}
	return 0;
}
