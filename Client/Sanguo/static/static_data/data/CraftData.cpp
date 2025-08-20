/*!
* \file CraftData.cpp
*
* \author xiaobowang;
* \date 2017/02/22
*
*
*/
#include "stdafx.h"
#include "CraftData.h"
#include "../config/CraftCfg.h"
#include "ItemData.h"
#include <map>
#include "msg/MsgItem.pb.h"
//#include "../data/StaticObj.h"
#include "../common/Heartbeat.h"
#include "role/Player.h"
#include "Cache.h"
#include <vector>
#include <LogUtil.h>
#include <StringUtil.h>
//#include "ChargeRewardData.h"

using namespace NS;

CraftDataMgr& CraftDataMgr::getInstance()
{
	static CraftDataMgr s_CraftMgr;
	return s_CraftMgr;
}

CraftDataMgr::CraftDataMgr()
{
}

void CraftDataMgr::addCraftTaskData(std::shared_ptr<CraftTaskData> pCraftTaskData)
{
	m_craftTaskDataMap[pCraftTaskData->m_protectorUUID] = pCraftTaskData;
}

void CraftDataMgr::updateCraftTaskData(std::shared_ptr<CraftTaskData> pCraftTaskData)
{
	m_craftTaskDataMap[pCraftTaskData->m_protectorUUID] = pCraftTaskData;
}

std::shared_ptr<CraftTaskData> CraftDataMgr::findCraftTaskData(NS::I64 protectorUUID)
{
	auto&& it = m_craftTaskDataMap.find(protectorUUID);
	if (it == m_craftTaskDataMap.end())
	{
		return nullptr;
	}

	return it->second;
}

void CraftDataMgr::removeCraftTaskData(NS::I64 protectorUUID)
{
	m_craftTaskDataMap.erase(protectorUUID);
}

void CraftDataMgr::clearCraftTaskData()
{
	m_craftTaskDataMap.clear();
}

int CraftTaskData::getImmediateNeedCash()
{
	auto serverTime = Heartbeat::getInstance().getServerTime();
	auto vipOffsetTime = 0;//ChargeRewardData::getInstance().getVipStatusTime(ChargeRewardData::Vip_Charge_State::Vip_Charge_State_Craft);
	auto surplusTime = std::max(this->m_finishTime - serverTime - vipOffsetTime, (NS::I64)0LL);

	//need cash
	auto n = std::pow(surplusTime / 60.0f * CraftCfg::getInstance().getCashCompleteMulti(), CraftCfg::getInstance().getCashCompletePow());

	return (int)(std::ceil(n));
}

CraftTaskData::State CraftTaskData::getStates()
{
	if (this->m_canelFinishNum > -1 && this->m_finishTime > 0)	//cancel
	{
		return State::CANCEL;
	}

	if (m_finishTime == 0)
	{
		if (0 == this->m_craftId)
		{
			return State::FREE;
		}
		else
		{
			return State::TO_CRAFT;
		}
	}

	auto serverTime = Heartbeat::getInstance().getServerTime();
	if (serverTime >= this->m_finishTime)
	{
		return State::FINISH;
	}

	return State::BUSY;
}


