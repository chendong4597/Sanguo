/*!
 * \file HandlerRole.h
 *
 * \author laizhisheng
 * \date 2016/11/08 15:31
 *
 */
#pragma once

//#include <MsgHandler.h>
#include "MsgHandler.h"
#include "BaseTypeDefine.h"
#include "msg/MsgPlayer.pb.h"
#include "msg/MsgLottery.pb.h"
#include "data/MapDataMgr.h"
#include "common/Timer.h"
#include "data/role/Role.h"
#include "data/MapObj.h"

namespace MsgPB
{
	class MsgPlayerInfoGS2C;
	class MsgMapObjectLeaveView;
	class MsgNotifyPlayerMovementG2C;
	class MsgPlayerNormalAttackResS2C;
	class MsgMapObjectUnderAttackG2C;
	class MsgSkillEffectDmgMS2C;
	class MsgCharacterDeadS2C;
	class MsgCharacterReviveS2C;
	class MsgMovePathG2C;
	class MsgMovePath;
	class MsgPlayerAppearance;
	class MsgBuffInfo_S2C;
	class MsgPlayerSysInfoGS2C;
	class MsgPlayerRenameFlagS2C;
	class MsgPlayerDailyResetS2C;
	class MsgCharacterBeBeatBackS2C;
	class MsgPlayerKickOutS2C;
	class MsgSkillEffectDotDmgMS2C;
	class MsgSkillEffectRestoreMS2C;
	
	//class MsgPlayerOnHandItemS2C;
}

class RoleBuffEvent
{
public:
	enum class EventType
	{
		AddBuff,
		UpdateBuff,
		RemoveBuff,
		LockBuff,
		UnLockBuff,
	};
};

class HandlerRole : public bzbee::MsgHandler
{
	DECLARE_EVENT_HOST();
	DECLARE_TIMER_HOST();
public:
	RTTI_DEF(HandlerRole);

	static HandlerRole& getInstance();
    static bool isInitialized();

	HandlerRole();
	virtual ~HandlerRole();

	bool initialize() override;
	void terminate() override;
	bool reset() override;

	bool touchTestRole(const glm::vec3& worldPos);

	bool requestPlayerOnHandItemC2S(MsgPB::EQUIP_PACKET_POS equipPos); // 切换 武器和工具时, 通知服务端同步;
	bool changeNameReq(const bzbee::tstring name, bool isFirst = false);
	const std::shared_ptr<Role> addOtherPlayer(int mapObjId, const glm::vec3& pos);
	static MapObj* findRole(bzbee::uint mapObjId);

private:
	void onMsgCharacterReviveS2C(const MsgPB::MsgCharacterReviveS2C& msg);
	void onMsgPlayerInfo(const MsgPB::MsgPlayerInfoGS2C& msg);
	void onMsgPlayerAppearance(const MsgPB::MsgPlayerAppearance& msg);

	// void onMsgMapObjPreAttackS2C(const MsgPB::MsgMapObjectPreAttack_S2C& msg);
	void onMsgMapObjectUnderAttackG2C(const MsgPB::MsgMapObjectUnderAttackG2C& msg);
	void onMsgSkillEffectDotDmgMS2C(const MsgPB::MsgSkillEffectDotDmgMS2C& msg);
	void onMsgPlayerNormalAttackRes(const MsgPB::MsgPlayerNormalAttackResS2C& msg);
	void onMsgSkillEffectDmgMS2C(const MsgPB::MsgSkillEffectDmgMS2C& msg);
	void onMsgCharacterDeadS2C(const MsgPB::MsgCharacterDeadS2C& msg);
	void onMsgBuffInfo_S2C(const MsgPB::MsgBuffInfo_S2C& msg);
	void onMsgPlayerDailyResetS2C(const MsgPB::MsgPlayerDailyResetS2C& msg);
	void onMsgUserBuffInfo_S2C(const MsgPB::MsgUserBuffDataS2C& msg);
	void onMsgSkillEffectRestoreMS2C(const MsgPB::MsgSkillEffectRestoreMS2C& msg);
	void onMsgLotteryS2C(const MsgPB::MsgLotteryS2C& msg);

	void onMsgItemPlayerEquipNotifyS2C(const MsgPB::MsgItemPlayerEquipNotifyS2C& msg);
	void onMsgPlayerDataSyncDoneG2C(const MsgPB::MsgPlayerDataSyncDoneG2C& msg);
	void onMsgPlayerDataContainerSyncDoneG2C(const MsgPB::MsgPlayerDataContainerSyncDoneG2C & msg);
	void onMsgPlayerSysInfoGS2C(const MsgPB::MsgPlayerSysInfoGS2C& msg);
	//void onMsgItemCurrencySyncS2C(const MsgPB::MsgItemCurrencySyncS2C& msg);
	//void onMsgPlayerOnHandItemS2C(const MsgPB::MsgPlayerOnHandItemS2C& msg);
	void onMsgPlayerRenameS2C(const MsgPB::MsgPlayerRenameS2C& msg);
	void onMsgPlayerRenameFlagS2C(const MsgPB::MsgPlayerRenameFlagS2C& msg);
	void onMsgOtherPlayerChangeEquipment(const MsgPB::MsgItemOtherPlayerChangeEquip_S2C& msg);
	void onMsgCharacterBeBeatBackS2C(const MsgPB::MsgCharacterBeBeatBackS2C& msg); // 击退消息
	void onMsgPlayerKickOutS2C(const MsgPB::MsgPlayerKickOutS2C& msg);
	void onMsgPlayerADNVideoAwardS2C(const MsgPB::MsgPlayerADNVideoAwardS2C& msg);
	void onTimeEvent(TimerEvent& evt);

private:
	bool m_isAdult{ false };
	bool m_needAgainLogin{ false };
	bool m_haveEnterMap{ false };
	bool m_haveSend{ false };
	int m_timeId{};
	int m_realname_state{};

	bool m_haveChangeName{ false };
	bool m_firstRecvPlayerLevel{ true };
	int m_TimeId{};
	int m_TimeDeadId{};
	NS::I64 m_id{};
};




