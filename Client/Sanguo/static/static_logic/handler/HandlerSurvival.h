/*!
 * \file HandlerBuilding.h
 *
 * \author xiaobowang;
 * \date 2017/3/17
 *
 */

#pragma once

#include <MsgHandler.h>
#include "BaseTypeDefine.h"
#include <memory>
#include "msg/MsgSurvival.pb.h"
#include "common/Timer.h"

namespace MsgPB
{
	class MsgSurvivalNotifyEventS2C;
	class MsgSurvivalNotifyWaveS2C;
	class MsgSurvivalNotifyTargetInfoS2C;
}

//class DropInfoItemData;

class HandlerSurvival : public bzbee::MsgHandler
{
	DECLARE_EVENT_HOST();
	DECLARE_TIMER_HOST();

public:

	RTTI_DEF(HandlerSurvival);

public:
	static HandlerSurvival& getInstance();

	HandlerSurvival();
	virtual ~HandlerSurvival();

	virtual bool initialize() override;
	virtual void terminate() override;
	virtual bool reset() override;

private:
	void onMsgSurvivalNotifyEventS2C(const MsgPB::MsgSurvivalNotifyEventS2C& msg);
	void onMsgSurvivalNotifyWaveS2C(const MsgPB::MsgSurvivalNotifyWaveS2C& msg);
	void onMsgSurvivalNotifyTargetInfoS2C(const MsgPB::MsgSurvivalNotifyTargetInfoS2C& msg);
	void onMsgSurvivalNotifyMobBirthPointS2C(const MsgPB::MsgSurvivalNotifyMobBirthPointS2C& msg);
	void onMsgSurvivalNotifyClearMobS2C(const MsgPB::MsgSurvivalNotifyClearMobS2C& msg);
	void onMsgSurvivalNotifyGameRewardS2C(const MsgPB::MsgSurvivalNotifyGameRewardS2C& msg);
};


