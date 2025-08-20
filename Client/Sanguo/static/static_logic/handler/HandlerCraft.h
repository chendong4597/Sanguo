/*!
* \file HandlerCraft.h
*
* \author xiaobowang;
* \date 3/1/2017
*
*/
#pragma once

#include "BaseTypeDefine.h"
#include <MsgHandler.h>
#include <memory>
#include "../common/MsgMgr.h"
#include "common/Timer.h"
class CraftData;
class CraftToolData;

namespace MsgPB
{
	class MsgCraftToolNotifyG2C;
	class MsgCraftG2C;
	class MsgCraftGetG2C;
	class MsgCraftCancelG2C;
	class MsgCraftToolUpgradeG2C;
	class MsgCraftToolRemoveG2C;
	class MsgCraftCompleteG2C;
	class MsgCraftAllUnlockNotifyG2C;
	class MsgCraftUnlockNotifyG2C;
	class MsgCraftToolUnitUnlockNotifyS2C;
	class MsgCraftToolUpdateG2C;
}

class HandlerCraft : public bzbee::MsgHandler
{
	DECLARE_EVENT_HOST();
	DECLARE_TIMER_HOST();

public:
	RTTI_DEF(HandlerCraft);

public:
	static HandlerCraft& getInstance();

	HandlerCraft();
	virtual ~HandlerCraft();

	virtual bool initialize() override;
	virtual void terminate() override;
	virtual bool reset() override;

	bool requestCraft(const std::shared_ptr<CraftData>& pCraftData, NS::I64 protectorUUID, int craftNum);
	
	bool requestCraftGet(NS::I64 protectorUUID);

	bool requestCraftCancel(NS::I64 protectorUUID);

	bool requestCraftComplete(NS::I64 protectorUUID);

	
private:
	void onMsgCraft(const MsgPB::MsgCraftG2C& msg);
	void onMsgCraftCancel(const MsgPB::MsgCraftCancelG2C& msg);
	void onMsgCraftComplete(const MsgPB::MsgCraftCompleteG2C& msg);
	void onMsgCraftToolNotify(const MsgPB::MsgCraftToolNotifyG2C& msg);
	void onMsgCraftGet(const MsgPB::MsgCraftGetG2C& msg);
	void onMsgCraftToolUpdateG2C(const MsgPB::MsgCraftToolUpdateG2C& msg);
	void onMsgCraftToolUpgrade(const MsgPB::MsgCraftToolUpgradeG2C& msg);
	void onMsgCraftToolRemove(const MsgPB::MsgCraftToolRemoveG2C& msg);
	void onMsgCraftAllUnlockNotify(const MsgPB::MsgCraftAllUnlockNotifyG2C& msg);
	void onMsgCraftUnlockNotify(const MsgPB::MsgCraftUnlockNotifyG2C& msg);
	void onMsgCraftToolUnitUnlockNotify(const MsgPB::MsgCraftToolUnitUnlockNotifyS2C& msg);

	int m_reqCraftLockTimer{0};
	int m_reqCraftGetLockTimer{0};
	int m_reqCraftCancelLockTimer{0};
	int m_reqCraftToolUpgradeLockTimer{0};
	int m_reqCraftCompleteLockTimer{0};
};
