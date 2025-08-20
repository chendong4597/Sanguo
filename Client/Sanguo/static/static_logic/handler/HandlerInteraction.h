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
#include "msg/MsgInteraction.pb.h"
#include "common/Timer.h"


//class DropInfoItemData;

class HandlerInteraction : public bzbee::MsgHandler
{
	DECLARE_EVENT_HOST();
	DECLARE_TIMER_HOST();

public:

	RTTI_DEF(HandlerInteraction);

public:
	static HandlerInteraction& getInstance();

	HandlerInteraction();
	virtual ~HandlerInteraction();

	virtual bool initialize() override;
	virtual void terminate() override;
	virtual bool reset() override;

private:
	void onMsgInteractionInWorldS2C(const MsgPB::MsgInteractionInWorldS2C& msg);
	void onMsgInteractionWalkS2C(const MsgPB::MsgInteractionWalkS2C& msg);
	void onMsgInteractionS2C(const MsgPB::MsgInteractionS2C& msg);
	void onMsgMagicEffectAppenceS2C(const MsgPB::MsgMagicEffectAppenceS2C& msg);
	void onMsgInteractionTurnS2C(const MsgPB::MsgInteractionTurnS2C& msg);
};


