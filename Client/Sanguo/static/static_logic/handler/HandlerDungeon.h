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
#include "msg/MsgDungeon.pb.h"
#include "common/Timer.h"

//class DropInfoItemData;

class HandlerDungeon : public bzbee::MsgHandler
{
	DECLARE_EVENT_HOST();
	DECLARE_TIMER_HOST();

public:

	RTTI_DEF(HandlerDungeon);

public:
	static HandlerDungeon& getInstance();

	HandlerDungeon();
	virtual ~HandlerDungeon();

	virtual bool initialize() override;
	virtual void terminate() override;
	virtual bool reset() override;

protected:
	void onMsgMsgDungeonInfo_MS2C(const MsgPB::MsgDungeonInfo_MS2C            & msg);
	void onMsgDungeonBossChest_MS2C(const MsgPB::MsgDungeonBossChest_MS2C& msg); //副本boss宝箱
	void onMsgDungeonBossOpenChestRs_MS2C(const MsgPB::MsgDungeonBossOpenChestRs_MS2C& msg); //开宝箱结果副本boss宝箱
	void onMsgDungeonBossDrop_MS2C(const MsgPB::MsgDungeonBossDrop_MS2C& msg);//boss掉落返回
	void onMsgDungeonBossComing_MS2C(const MsgPB::MsgDungeonBossComing_MS2C& msg);
	void onMsgDungeonBossKill_MS2C(const MsgPB::MsgDungeonBossKill_MS2C& msg);
	void onMsgDungeonNextMob_S2C(const MsgPB::MsgDungeonNextMob_S2C& msg);
	void onMsgDungeonSucExp_S2C(const MsgPB::MsgDungeonSucExp_S2C& msg);

};


