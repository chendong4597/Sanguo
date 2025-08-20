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
#include "msg/MsgStore.pb.h"
#include "common/Timer.h"

namespace MsgPB
{
	class MsgStoreSellLimitDataS2C;
	class MsgStoreGetDataResultS2C;
	class MsgStoreBuyResultS2C;
	class MsgStoreSellResultS2C;
}

//class DropInfoItemData;

class HandlerStore : public bzbee::MsgHandler
{
	DECLARE_EVENT_HOST();
	DECLARE_TIMER_HOST();

public:

	RTTI_DEF(HandlerStore);

public:
	static HandlerStore& getInstance();

	HandlerStore();
	virtual ~HandlerStore();

	virtual bool initialize() override;
	virtual void terminate() override;
	virtual bool reset() override;
private:
	void onMsgStoreGetDataResultS2C(const MsgPB::MsgStoreGetDataResultS2C& msg);
	void onMsgStoreBuyResultS2C(const MsgPB::MsgStoreBuyResultS2C& msg);
	void onMsgStoreSellResultS2C(const MsgPB::MsgStoreSellResultS2C& msg);

};


