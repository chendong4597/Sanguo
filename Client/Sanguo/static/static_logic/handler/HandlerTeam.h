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
#include "msg/MsgTeam.pb.h"
#include "data/TeamMemberData.h"
#include "common/Timer.h"


//class DropInfoItemData;

class HandlerTeam : public bzbee::MsgHandler
{
	DECLARE_EVENT_HOST();
	DECLARE_TIMER_HOST();

public:

	RTTI_DEF(HandlerTeam);

public:
	static HandlerTeam& getInstance();

	HandlerTeam();
	virtual ~HandlerTeam();

	virtual bool initialize() override;
	virtual void terminate() override;
	virtual bool reset() override;

public:
	// 创建队伍。
	void createTeam();
	void createTeamByDungeonId(NS::int32 dungeonId);


	// 自动加入。
	void autoJoinTeam();

	// 自动加入特定Id的副本
	void autoJoinTeamById(NS::int32 dungeonId);

	void showMatchingTipDialog();

	// 离开队伍。
	void leaveTeam();
	// 选择副本。
	void selectDungeon(NS::int32 mapId);
	// 队员菜单命令。
	// 设为队长。
	void setLeader(int iggID);
	// 移出队伍。
	void kickMember(int iggID);
	NS::I64 getMemberIggIdBySeatIndex(int seatIndex);

	void startPvp();

	bool requestSetEnableJoin();

	bool requestCancelAutoJoin();

	//组队好友
	bool requestInviteMember(const std::set<NS::I64>& setIggIds);

	bool requestAcceptTeamInvatation(NS::int32 teamId, NS::int32 mapId);


public:
	inline NS::int32 getTeamId() const { return m_TeamId; }
	inline NS::int32 getMapId() const { return m_MapId; }
	inline NS::I64 getLeaderIggId() const { return m_LeaderIggId; }

	inline bool getCountdownState() const { return m_teamStatus == MsgPB::PB_TEAM_STATUS_READY_TO_GO; }
	inline void setCountdownState(bool state) { m_CountdownState = state; }

	inline NS::int32 getCountdown() const { return m_Countdown; }
	inline void setCountdown(NS::int32 countdown) { m_Countdown = countdown; }

	// 	inline const TeamMemberInfo* getMemberInfo() const { return m_MemberInfoArr; }
	inline int getMemberCount() const { return m_MemberCount; }

	inline bool getIsInTeam() const { return getTeamId() != 0; }
	inline bool getIsTeamOpenJoin() const { return m_bIsTeamOpenJoin; }
	inline bool getIsMatching() const { return m_bIsMatching; }

	std::map<NS::I64, TeamMemberInfo>  getMapTeamMembers() { return m_mapTeamMembers; }

	bool isPlayerInTeam(NS::I64 iggId) { return m_mapTeamMembers.find(iggId) != m_mapTeamMembers.end(); };

private:
	void onMsgTeamCreateDungeonRecruitmentRs_S2C(const MsgPB::MsgTeamCreateDungeonRecruitmentRs_S2C& msg);
	void onMsgTeamRequestJoinTeamRs_S2C(const MsgPB::MsgTeamRequestJoinTeamRs_S2C         & msg);
	void onMsgTeamRequestLeaveTeamRs_S2C(const MsgPB::MsgTeamRequestLeaveTeamRs_S2C        & msg);

	void onMsgTeamSetLeaderRs_S2C(const MsgPB::MsgTeamSetLeaderRs_S2C               & msg);
	void onMsgTeamRemovePlayerRs_S2C(const MsgPB::MsgTeamRemovePlayerRs_S2C            & msg);
	void onMsgTeamRemovePlayerNotify_MC2C(const MsgPB::MsgTeamRemovePlayerNotify_MC2C            & msg);
	void onMsgTeamAutoJoinTeamRs_S2C(const MsgPB::MsgTeamAutoJoinTeamRs_S2C            & msg);
	void onMsgTeamNotifyTeamDisbanded_S2C(const MsgPB::MsgTeamNotifyTeamDisbanded_S2C       & msg);

	void clearTeamInfo();

	void onMsgTeamInfo_S2C(const MsgPB::MsgTeamInfo_S2C                      & msg);
	void onMsgTeamMember_S2C(const MsgPB::MsgTeamMember_S2C                    & msg);

	void memInfoChangeOldVersion(const MsgPB::MsgTeamMember_S2C &msg);

	void onMsgTeamDungeonReadyToGo_S2C(const MsgPB::MsgTeamDungeonReadyToGo_S2C          & msg);
	void onMsgTeamSelectDungeonRs_S2C(const MsgPB::MsgTeamSelectDungeonRs_S2C           & msg);
	void onMsgTeamError(const MsgPB::MsgTeamError                         & msg);
	void onMsgCancelAutoJoinRs_S2C(const MsgPB::MsgTeamAutoJoinCancelRes_MC2C                         & msg);

	//组队好友回调
	void onMsgTeamInviteRs_S2C(const MsgPB::MsgTeamInviteRs_S2C& msg);
	void onMsgTeamInviteNotify_S2C(const MsgPB::MsgTeamInviteNotify_S2C& msg);
	void onMsgTeamAcceptRs_S2C(const MsgPB::MsgTeamAcceptRs_S2C& msg);
	void onMsgTeamRefuseInvite_S2C(const MsgPB::MsgTeamRefuseInvite_S2C& msg);
	void onMsgTeamStartDungeonRs_S2C(const MsgPB::MsgTeamStartDungeonRs_S2C& msg);

private:
	NS::int32 m_TeamId{};
	NS::int32 m_MapId{};
	bool m_bIsTeamOpenJoin{ false };
	NS::I64 m_LeaderIggId{};
	NS::I64 m_lastLeaderIggId{ 0 };

	// 因为服务端返回有一定的延迟，所以用一个额外的状态来标志客户端是否已经准备倒计时。
	bool m_CountdownState{ false };
	NS::int32 m_Countdown{};
	int m_timerCountdownTimeRef{ 0 };

	// 	TeamMemberInfo m_MemberInfoArr[5];
	std::map<NS::I64, TeamMemberInfo> m_mapTeamMembers;

	// 	std::
	int m_MemberCount{ 0 };
	bool m_bIsMatching{ false };
	NS::int32 m_matchingMapId{ 0 };
	MsgPB::PB_TEAM_STATUS m_teamStatus;

	//邀請列表倒計時計時器
	NS::uint m_invatationCDTimer{ 0 };
};


