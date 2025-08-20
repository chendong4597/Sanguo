/*!
 * \file HandlerAchieve.h
 *
 * \author ChenBK;
 * \date 2017/10/11
 *
 * 
 */
#pragma once

#include "BaseTypeDefine.h"
#include <MsgHandler.h>
#include "../common/MsgMgr.h"
#include <msg/MsgType.pb.h>
#include <msg/MsgAchieve.pb.h>
#include "common/Timer.h"

namespace MsgPB
{
	class MsgAchieveSyncS2C;
	class MsgAchieveUpdateS2C;
	class MsgAchieveRemoveS2C;
}

class HandlerAchieve : public bzbee::MsgHandler
{
	DECLARE_EVENT_HOST();
	DECLARE_TIMER_HOST();

public:
	RTTI_DEF(HandlerAchieve);

public:
	static HandlerAchieve& getInstance();

	HandlerAchieve();
	virtual ~HandlerAchieve();

	bool initialize() override;
	void terminate() override;
	bool reset() override;

	bool requestAchieveReceiveReward(NS::I64 detailId);
	bool requestAchieveClientTrigger(NS::I64 detailId, int val = 0);

	//void METHOD(register_HandlerAchieveEvent)(const LuaCallback& invoker);
	
	void playerRewardAnt(NS::I64 uuid = 0);			// 播放  获取成就奖品动画.

	bool addLimitMoveBuilding(const glm::ivec2& coord, NS::uint buildingTypeId);
	void removeLimitMoveBuilding(const glm::ivec2& coord);
	void clearLimitMoveBuilding()
	{
		m_limitMoveBuildings.clear();
	}
	void removeLimitMoveBuildings();
	void playerRewardNotify();		// 播放　领取奖品　通知．

	bool isSaveAllenAchComplete(); //解救艾伦成就是否达成
	bool isInitAchieveDone() { return m_bInitAchieveDone; }
private:
	void onMsgAchieveSync(const MsgPB::MsgAchieveSyncS2C& msg);
	void onMsgAchieveUpdate(const MsgPB::MsgAchieveUpdateS2C& msg);
	void onMsgAchieveRemove(const MsgPB::MsgAchieveRemoveS2C& msg);
	void onMsgAchieveReceiveRewardResult(const MsgPB::MsgAchieveReceiveRewardResultS2C& msg);
	void onMsgAchieveClientTriggerResult(const MsgPB::MsgAchieveClientTriggerResultS2C& msg);
	
	void playerAchieveNotify();		// 播放　成就完成通知．

	void onRewardNotifyTimerEvent(const TimerEvent& evt); // 播放 获取成就奖品通知 Timer
	void popupMsgBox(const NS::tstring& text);

	void putLimitMoveBuildings(bool isForcePut);
	void putRapidRocketAndOperator();

private:
	bool m_bInitAchieveDone{ false };
//	bool m_bInPlayerRewardNotifyItem{ false };
	
	int m_rewardNotifyTimerId{ 0 };
	int m_rewardAntTimerId{ 0 };

	//int m_putLMBCount{ 20 }; // 防止无限循环的保护
	//bool m_hasPutLimitMoveBuilding{ false };
	std::set<glm::ivec2> m_limitMoveBuildings{};
};

