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
#include "data/role/Monsters.h"
#include "msg/MsgMob.pb.h"
#include "msg/MsgMapServer.pb.h"
#include "common/Timer.h"

namespace MsgPB
{
	class MsgMobAppearance;
}

//class DropInfoItemData;

class HandlerMonster : public bzbee::MsgHandler
{
	DECLARE_EVENT_HOST();
	DECLARE_TIMER_HOST();

public:

	RTTI_DEF(HandlerMonster);

public:
	static HandlerMonster& getInstance();

	HandlerMonster();
	virtual ~HandlerMonster();

	virtual bool initialize() override;
	virtual void terminate() override;
	virtual bool reset() override;

public:
	std::shared_ptr<Monster> addMonster(int mapObjId, int insId, int defId, const glm::vec3& pos, bool createWithLocal,
		NS::I64 serverMapObjId, bzbee::uint8 eventType, int nGroupId);
private:
	void onMsgMobAppearance(const MsgPB::MsgMobAppearance& msg);
	void onMsgMapObjectLeaveView(const MsgPB::MsgMapObjectLeaveView & msg);
};


