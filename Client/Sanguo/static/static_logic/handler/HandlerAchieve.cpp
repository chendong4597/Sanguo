/*!
 * \file HandlerAchieve.cpp
 *
 * \author ChenBK
 * \date 12/08/2016
 *
 */
#include "stdafx.h"
#include "HandlerAchieve.h"
#include "config/PlayerAttrNameCfg.h"
#include "data/AchieveData.h"
#include "HandlerLogin.h"
#include "HandlerRole.h"
#include "data/MapData.h"

using namespace NS;
using namespace MsgPB;

static HandlerAchieve* g_HandlerAchieve = nullptr;
HandlerAchieve::HandlerAchieve()
{
    assert(!g_HandlerAchieve);
    g_HandlerAchieve = this;
    reset();
}

HandlerAchieve::~HandlerAchieve()
{
    g_HandlerAchieve = nullptr;
}

HandlerAchieve& HandlerAchieve::getInstance()
{
    assert(g_HandlerAchieve);
    return *g_HandlerAchieve;
}

bool HandlerAchieve::initialize()
{
	REGISTER_S2C(onMsgAchieveSync);
	REGISTER_S2C(onMsgAchieveUpdate);
	REGISTER_S2C(onMsgAchieveRemove);
	REGISTER_S2C(onMsgAchieveReceiveRewardResult);
	// REGISTER_S2C(onMsgAchieveClientTriggerResult);

	return true;
}

void HandlerAchieve::terminate()
{
	UNREGISTER_TIMER(m_rewardAntTimerId);
	UNREGISTER_TIMER(m_rewardNotifyTimerId);
}

bool HandlerAchieve::reset()
{
	return true;
}

bool HandlerAchieve::requestAchieveReceiveReward(NS::I64 detailId)
{
	MsgPB::MsgAchieveReceiveRewardC2S msg;
	msg.set_uuid(detailId);

	return MsgMgr::getInstance().sendMessageDirect(_MSG_ACHIEVE_RECEIVE_REWARD_C2S, &msg);
}

bool HandlerAchieve::requestAchieveClientTrigger(NS::I64 detailId, int val)
{
	MsgPB::MsgAchieveClientTriggerC2S msg;
	msg.set_uuid(detailId);
	msg.set_value(val);

	return MsgMgr::getInstance().sendMessageDirect(_MSG_ACHIEVE_CLIENT_TRIGGER_C2S, &msg);
}

void HandlerAchieve::onMsgAchieveSync(const MsgPB::MsgAchieveSyncS2C& msg)
{
	for (auto data : msg.data())
	{
		AchieveData::getInstance().syncAchieveData(data);

		if(data.status() != MsgPB::ACHIEVE_STATUS::ACHIEVE_STATUS_LOCK)	// 弱联网状态, 不更新到UI.
		{
			//Post Event
			HandlerAchieveEvent evt;
			evt.m_type = HandlerAchieveEvent::EventType::AddAchieve;
			evt.m_detailId = data.uuid();
			POST_DATA_EVENT(evt);
		}
	}

	//Post Event
	HandlerAchieveEvent evt;
	evt.m_type = HandlerAchieveEvent::EventType::SyncAchieve;
	POST_DATA_EVENT(evt);

	m_bInitAchieveDone = true;  // 初始化成就数据;

	/*static bool firstInit{ true };
	if (firstInit)
	{
		firstInit = false;
		unLockFunEvent evt;
		evt.m_type = unLockFunEvent::EventType::firstRecvFinishAchieve;
		auto&& map = AchieveData::getInstance().getFinishAchieveDataMap();
		for (auto it : map)
		{
			evt.m_vecId.push_back(it.first);
		}
		if (!evt.m_vecId.empty())
			HandlerRole::getInstance().postEvent(evt);
	}*/
}

void HandlerAchieve::onMsgAchieveUpdate(const MsgPB::MsgAchieveUpdateS2C& msg)
{
	bool bNewAdd = (AchieveData::getInstance().findAchieveDataById(msg.data().uuid()) == nullptr);	// 检查是否新添加.
	bool bUnfinish = AchieveData::getInstance().checkedAchieveUnfinish(msg.data().uuid());
	bool bLock = AchieveData::getInstance().checkedAchieveLock(msg.data().uuid());
	bool bIsTotal = AchieveCfg::getInstance().checkedIsTotalAchieve(msg.data().uuid());
	if (!bIsTotal && (bUnfinish || bLock) && m_bInitAchieveDone 
		&& (msg.data().status() == ACHIEVE_STATUS_ACCOMPLISH || msg.data().status() == ACHIEVE_STATUS_RECEIVED))		// 发送通知到 界面.
	{

	}

	AchieveData::getInstance().addUpdateAchieveData(msg.data());

	if (msg.data().status() != MsgPB::ACHIEVE_STATUS::ACHIEVE_STATUS_LOCK)	// 弱联网状态, 不更新到UI.
	{
		//Post Event
		HandlerAchieveEvent evt;
		evt.m_type = bNewAdd ? HandlerAchieveEvent::EventType::AddAchieve : HandlerAchieveEvent::EventType::UpdateAchieve;
		evt.m_detailId = msg.data().uuid();
		POST_DATA_EVENT(evt);
	}
	if (bUnfinish && msg.data().status() == MsgPB::ACHIEVE_STATUS::ACHIEVE_STATUS_ACCOMPLISH)
	{
	}
}

// lua;
HandlerAchieve* LUA(get_handlerAchieve)(void)
{
	return &HandlerAchieve::getInstance();
}


void HandlerAchieve::onMsgAchieveRemove(const MsgPB::MsgAchieveRemoveS2C& msg)
{
	AchieveData::getInstance().removeAchieveDataById(msg.uuid());

	//Post Event
	HandlerAchieveEvent evt;
	evt.m_type = HandlerAchieveEvent::EventType::DelAchieve;
	POST_DATA_EVENT(evt);
}

void HandlerAchieve::onMsgAchieveReceiveRewardResult(const MsgPB::MsgAchieveReceiveRewardResultS2C& msg)
{
	tstring strMsg = EMPTY_STRING;
	switch (msg.result())
	{
	case MsgPB::MsgAchieveReceiveRewardResultS2C_RESULT_SUCCESS:
		//playerRewardAnt(msg.uuid());		// 播放 领奖动画与提示;
		break;
	case MsgPB::MsgAchieveReceiveRewardResultS2C_RESULT_FAILURE:
		strMsg = _LC("CLI_HandlerAchieve-ACHIEVE_RESUT_FAILURE");
		break;
	case MsgPB::MsgAchieveReceiveRewardResultS2C_RESULT_NOT_ACCOMPLISH:	// 未完成，無法領取
		strMsg = _LC("CLI_HandlerAchieve-ACHIEVE_RESUT_NOT_ACCOMPLISH");
		break;
	case MsgPB::MsgAchieveReceiveRewardResultS2C_RESULT_RECEIVED:		// 已領取過，無法再領取	
		strMsg = _LC("CLI_HandlerAchieve-ACHIEVE_RESUT_RECEIVED");
		break;
	case MsgPB::MsgAchieveReceiveRewardResultS2C_RESULT_TEMPLATE_ERROR:	// 樣版錯誤，無法領取	
		strMsg = _LC("CLI_HandlerAchieve-ACHIEVE_RESUT_TEMPLATE_ERROR");
		break;
	case MsgPB::MsgAchieveReceiveRewardResultS2C_RESULT_ITEM_FULL:		// 背包空間已滿，無法領取
		strMsg = _LC("CLI_HandlerAchieve-ACHIEVE_RESUT_ITEM_FULL");
		break;
	default:
		break;
	}

	if (!strMsg.empty())
	{
		popupMsgBox(strMsg);
	}
}

void HandlerAchieve::onMsgAchieveClientTriggerResult(const MsgPB::MsgAchieveClientTriggerResultS2C & msg)
{
	// TODO:
}

void HandlerAchieve::playerAchieveNotify()
{
	// 未开始播放, 启动一个新的 Notify显示.
}

void HandlerAchieve::playerRewardAnt(NS::I64 uuid)
{
	if (uuid == 0)
		return;
}

bool HandlerAchieve::addLimitMoveBuilding(const glm::ivec2 & coord, NS::uint buildingTypeId)
{
	return false;
}

void HandlerAchieve::removeLimitMoveBuilding(const glm::ivec2 & coord)
{
	if (m_limitMoveBuildings.find(coord) != m_limitMoveBuildings.end())
	{
		m_limitMoveBuildings.erase(coord);
	}
}

void HandlerAchieve::playerRewardNotify()
{
	// 未开始播放, 启动一个新的 Timer Notify Reward Item.
	if (m_rewardNotifyTimerId == 0)
	{
		REGISTER_TIMER(m_rewardNotifyTimerId, static_cast<float>(UINT_MAX), 1.2f, &HandlerAchieve::onRewardNotifyTimerEvent);
	}
}

//解救艾伦成就是否达成
bool HandlerAchieve::isSaveAllenAchComplete()
{

	return true;
}

void HandlerAchieve::onRewardNotifyTimerEvent(const TimerEvent& evt)
{
	AchieveData::RewardNotifyItem itemPair = AchieveData::getInstance().getNextRewardNotifyItem();
	if (itemPair == AchieveData::RewardNotifyItem(0, 0))
	{
		UNREGISTER_TIMER(m_rewardNotifyTimerId);
		m_rewardNotifyTimerId = 0;
	}
	else
	{
		NS::tstring itemName;
		const ItemCfg::Item*  itemCfg = ItemCfg::getInstance().getItemById(itemPair.first);
		itemName = itemCfg ? itemCfg->getNameByQgStyle() : fmt::format("{0}",itemPair.first);

	}
}

void HandlerAchieve::popupMsgBox(const tstring& text)
{
}

void HandlerAchieve::putLimitMoveBuildings(bool isForcePut)
{


}

void HandlerAchieve::putRapidRocketAndOperator()
{
	
}

void HandlerAchieve::removeLimitMoveBuildings()
{
}


