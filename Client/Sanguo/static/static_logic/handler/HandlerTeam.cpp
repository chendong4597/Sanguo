/*!
 * \file HandlerBuilding.cpp
 *
 * \author xiaobowang;
 * \date 2017/3/17
 *
 */
#include "stdafx.h"
#include "../common/MsgMgr.h"
#include "msg/MsgTeam.pb.h"
#include "../GameAppExtern.h"
#include "data/role/Player.h"
#include "HandlerTeam.h"

using namespace NS;

static HandlerTeam* g_HandlerTeam = nullptr;
HandlerTeam& HandlerTeam::getInstance()
{
    assert(g_HandlerTeam);
    return *g_HandlerTeam;
}

HandlerTeam::HandlerTeam()
{
    assert(!g_HandlerTeam);
	g_HandlerTeam = this;
    reset();
}

HandlerTeam::~HandlerTeam()
{
	g_HandlerTeam = nullptr;
}

bool HandlerTeam::initialize()
{
	REGISTER_S2C(onMsgTeamCreateDungeonRecruitmentRs_S2C);
	REGISTER_S2C(onMsgTeamRequestJoinTeamRs_S2C);
	REGISTER_S2C(onMsgTeamRequestLeaveTeamRs_S2C);
	REGISTER_S2C(onMsgTeamSetLeaderRs_S2C);
	REGISTER_S2C(onMsgTeamRemovePlayerRs_S2C);
	REGISTER_S2C(onMsgTeamAutoJoinTeamRs_S2C);
	REGISTER_S2C(onMsgTeamNotifyTeamDisbanded_S2C);
	REGISTER_S2C(onMsgTeamInfo_S2C);
	REGISTER_S2C(onMsgTeamMember_S2C);
	REGISTER_S2C(onMsgTeamDungeonReadyToGo_S2C);
	REGISTER_S2C(onMsgTeamSelectDungeonRs_S2C);
	REGISTER_S2C(onMsgTeamError);
	REGISTER_S2C(onMsgTeamRemovePlayerNotify_MC2C);
	REGISTER_S2C(onMsgCancelAutoJoinRs_S2C);
	REGISTER_S2C(onMsgTeamStartDungeonRs_S2C);
	
    return true;
}

void HandlerTeam::terminate()
{
    reset();
}

bool HandlerTeam::reset()
{
    return true;
}


void HandlerTeam::onMsgTeamRequestJoinTeamRs_S2C(const MsgTeamRequestJoinTeamRs_S2C& msg)
{
	
}


void HandlerTeam::onMsgTeamNotifyTeamDisbanded_S2C(const MsgTeamNotifyTeamDisbanded_S2C& msg)
{
}


void HandlerTeam::onMsgTeamInfo_S2C(const MsgTeamInfo_S2C& msg)
{
	
}


void HandlerTeam::onMsgTeamMember_S2C(const MsgPB::MsgTeamMember_S2C& msg)
{
	

}

void HandlerTeam::memInfoChangeOldVersion(const MsgTeamMember_S2C &msg)
{

}

void HandlerTeam::onMsgTeamDungeonReadyToGo_S2C(const MsgTeamDungeonReadyToGo_S2C& msg)
{
	POST_DATA_EVENT(msg);
}

void HandlerTeam::onMsgTeamStartDungeonRs_S2C(const MsgPB::MsgTeamStartDungeonRs_S2C& msg)
{
	if (!msg.result() == TEAM_START_DUNGEON_SUCCESS)
	{
		return;
	}
	//MapDataMgr::getInstance().setSerialID(msg.serialid());
	//HandlerTransterEvent evt;
	//evt.m_type = HandlerTransterEvent::EventTranster::EventTranster_DungeonMap;
	//POST_DATA_EVENT(evt);
}


// TODO: 此方法顺带注册一次回调事件。（由外部调用者注册事件，但是仅响应一次。）
void HandlerTeam::createTeam()
{
}

void HandlerTeam::createTeamByDungeonId(NS::int32 dungeonId)
{
	if (dungeonId == 0)
	{
		return;
	}
	MsgTeamCreateDungeonRecruitment_C2S msg;
	msg.set_serialid(0);
	msg.set_mapdataid(dungeonId);
	MapDataMgr::getInstance().setMapId(dungeonId);
	MsgMgr::getInstance().sendMessageDirect(msg);
}

void HandlerTeam::onMsgTeamCreateDungeonRecruitmentRs_S2C(const MsgTeamCreateDungeonRecruitmentRs_S2C& msg)
{
	if (msg.result() == CREATE_TEAM_SUCCESS) {
		MsgTeamStartDungeon_C2S msgC2S;
		msgC2S.set_serialid(msg.serialid());
		msgC2S.set_iggid(msg.iggid());
		MsgMgr::getInstance().sendMessageDirect(msgC2S);
	}
}

void HandlerTeam::autoJoinTeam()
{
	autoJoinTeamById(1);
}

bool HandlerTeam::requestSetEnableJoin()
{
	MsgTeamSetOpenJoin_C2MC msg;
	msg.set_open_join(!m_bIsTeamOpenJoin ? 1 : 0);
	return MsgMgr::getInstance().sendMessage(msg);
}

void HandlerTeam::autoJoinTeamById(NS::int32 dungeonId)
{
}

void HandlerTeam::onMsgTeamAutoJoinTeamRs_S2C(const MsgTeamAutoJoinTeamRs_S2C& msg)
{

}

void HandlerTeam::leaveTeam()
{
	MsgMgr::getInstance().sendMessage(MsgTeamRequestLeaveTeam_C2S());
}

void HandlerTeam::onMsgTeamRequestLeaveTeamRs_S2C(const MsgTeamRequestLeaveTeamRs_S2C& msg)
{
	
}



void HandlerTeam::selectDungeon(int32 mapId)
{
	MsgTeamSelectDungeon_C2S m;
	m.set_mapdataid(mapId);
	MsgMgr::getInstance().sendMessage(m);
}

void HandlerTeam::onMsgTeamSelectDungeonRs_S2C(const MsgTeamSelectDungeonRs_S2C& msg)
{

}

void HandlerTeam::setLeader(int seatIndex)
{
	auto iggId = getMemberIggIdBySeatIndex(seatIndex);
	MsgTeamSetLeader_C2S m;
	m.set_serialid(0);
	m.set_new_leader_iggid(iggId);
	MsgMgr::getInstance().sendMessage(m);
}

void HandlerTeam::onMsgTeamSetLeaderRs_S2C(const MsgTeamSetLeaderRs_S2C& msg)
{
	
}

NS::I64 HandlerTeam::getMemberIggIdBySeatIndex(int seatIndex)
{
	auto it = m_mapTeamMembers.find(seatIndex);
	if (it != m_mapTeamMembers.end())
	{
		return it->second.iggId;
	}
	return 0;
}

void HandlerTeam::kickMember(int seatIndex)
{
	auto iggId = getMemberIggIdBySeatIndex(seatIndex);
	MsgTeamRemovePlayer_C2S m;
	m.set_serialid(0);
	m.set_member_iggid(iggId);
	MsgMgr::getInstance().sendMessage(m);
}

void HandlerTeam::onMsgTeamRemovePlayerRs_S2C(const MsgTeamRemovePlayerRs_S2C& msg)
{
}

void HandlerTeam::onMsgTeamRemovePlayerNotify_MC2C(const MsgPB::MsgTeamRemovePlayerNotify_MC2C & msg)
{
}

void HandlerTeam::onMsgTeamError(const MsgPB::MsgTeamError & msg)
{

}


void HandlerTeam::clearTeamInfo()
{
	m_TeamId = 0;
	m_MapId = 0;
	m_LeaderIggId = 0;
	m_CountdownState = false;
	m_Countdown = 0;
	m_MemberCount = 0;
	m_lastLeaderIggId = 0;
	m_bIsTeamOpenJoin = false;
	m_mapTeamMembers.clear();
	m_bIsMatching = false;
	m_teamStatus = MsgPB::PB_TEAM_STATUS_NULL;
}


bool HandlerTeam::requestCancelAutoJoin()
{
	//MsgTeamAutoJoinCancel_C2MC msg;
	//auto pPlayerData = PlayerMgr::getInstance().getSelf();
	//if (pPlayerData)
	//{
	//	msg.set_iggid(pPlayerData->getIggId());
	//}
	//return MsgMgr::getInstance().sendMessage(msg);
	return true;
}

void HandlerTeam::onMsgCancelAutoJoinRs_S2C(const MsgPB::MsgTeamAutoJoinCancelRes_MC2C & msg)
{

}


bool HandlerTeam::requestInviteMember(const std::set<NS::I64>& setIggIds)
{
	MsgTeamInvite_C2S msg;
	for (auto it : setIggIds)
	{
		msg.add_target_iggids(it);
	}
	return MsgMgr::getInstance().sendMessage(msg);
}

void HandlerTeam::onMsgTeamInviteRs_S2C(const MsgPB::MsgTeamInviteRs_S2C& msg)
{
}

bool HandlerTeam::requestAcceptTeamInvatation(int32 teamId, int32 mapId)
{
	MsgTeamAccept_C2S msg;
	msg.set_teamid(teamId);
	msg.set_mapdataid(mapId);
	return MsgMgr::getInstance().sendMessage(msg);
}

void HandlerTeam::onMsgTeamInviteNotify_S2C(const MsgPB::MsgTeamInviteNotify_S2C& msg)
{

}



void HandlerTeam::onMsgTeamAcceptRs_S2C(const MsgPB::MsgTeamAcceptRs_S2C& msg)
{
}

void HandlerTeam::onMsgTeamRefuseInvite_S2C(const MsgPB::MsgTeamRefuseInvite_S2C& msg)
{
}





