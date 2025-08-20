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
#include "msg/MsgPublicMap.pb.h"
#include "common/Timer.h"

//class DropInfoItemData;

class HandlerPublicMap : public bzbee::MsgHandler
{
	DECLARE_EVENT_HOST();
	DECLARE_TIMER_HOST();

public:

	RTTI_DEF(HandlerPublicMap);
public:
	static HandlerPublicMap& getInstance();

	HandlerPublicMap();
	virtual ~HandlerPublicMap();

	virtual bool initialize() override;
	virtual void terminate() override;
	virtual bool reset() override;

	bool requestJoinToPubllic();
	bool requestJoinToPrivate();
protected:
	void onMsgPublicMapChangeMapRs_S2C(const MsgPB::MsgPublicMapChangeMapRs_S2C& msg);
};


