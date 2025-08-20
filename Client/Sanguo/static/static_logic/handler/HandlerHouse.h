/*!
 * \file HandlerBuilding.h
 *
 * \author xiaobowang;
 * \date 2017/3/17
 *
 */

#pragma once

#include "BaseTypeDefine.h"
#include <BaseType.h>
#include <memory>
#include "msg/MsgHouse.pb.h"
#include "common/Timer.h"


//class DropInfoItemData;

class HandlerHouse : public bzbee::MsgHandler
{
	DECLARE_EVENT_HOST();
	DECLARE_TIMER_HOST();

public:

	RTTI_DEF(HandlerHouse);

public:
	static HandlerHouse& getInstance();

	HandlerHouse();
	virtual ~HandlerHouse();

	virtual bool initialize() override;
	virtual void terminate() override;
	virtual bool reset() override;

	bool requestUpgrade(int nHouseId, int nHouseTpy);
	bool requestHome(NS::int32 nHouseId, NS::I64 uuid, bool bCheckin);

private:
	void onMsgHouseSyncS2C(const MsgPB::MsgHouseSyncS2C& msg);
	void onMsgHouseUpdateS2C(const MsgPB::MsgHouseUpdateS2C& msg);
	void onMsgHouseUpgradeS2C(const MsgPB::MsgHouseUpgradeS2C& msg);

};


