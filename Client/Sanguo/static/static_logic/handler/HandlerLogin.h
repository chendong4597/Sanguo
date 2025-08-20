#pragma once
#include <MsgHandler.h>
#include <common/Timer.h>
#include "msg/MsgPing.pb.h"
#include "MsgNetWork.pb.h"
#include "msg/MsgLogin.pb.h"
#include "msg/MsgPlayer.pb.h"
#include "msg/MsgMapServer.pb.h"
#include "../GameAppExtern.h"



using namespace MsgPB;

class HandlerLogin : public NS::MsgHandler
{
	RTTI_DEF(HandlerLogin);
	DECLARE_EVENT_HOST();
	DECLARE_TIMER_HOST();
public:
	HandlerLogin();
	virtual ~HandlerLogin();

	static HandlerLogin& getInstance();

	bool isConnect() { return true; }

	bool initialize() override;
	void terminate() override;
	bool reset() override { return true; }
	void update(float dt) override;

	void checkPlatformAccount(std::string strAccount, std::string strPwd, int platform, int client_ver);
	void registerAccount(std::string strAccount, std::string strPwd, int platform);
	void loginToLS(NS::I64 nIggId);
	void loginToGS();
	void requesetMapDone();

	void SetAvatar(std::string strName , int nSex , int nHairStyle , int nHairColor , int nSkinColor);
	NS::I64 GetHeroIggId() { return m_nIggId; }

	void enterMap();
private:
	void onMsgConnectionState(const MsgPB::MsgConnectionState& msg);
	void onMsgLoginResp(const MsgPB::MsgLoginResp& msg);
	void onMsgLoginValidate(const MsgPB::MsgLoginValidate& msg);
	void onMsgPlatformAccountCheckLS2C(const MsgPB::MsgPlatformAccountCheckLS2C& msg);
	void onMsgRegisterAccountLS2C(const MsgPB::MsgRegisterAccountLS2C& msg);
	void onMsgPlayerSetAvatorResultS2C(const MsgPB::MsgPlayerSetAvatorResultS2C& msg);
	void onMsgPlayerInfoGS2C(const MsgPB::MsgPlayerInfoGS2C& msg);
	void onMsgJoinMapG2C(const MsgPB::MsgJoinMapG2C& msg);
	void onMsgPublicDropListS2C(const MsgPB::MsgPublicDropListS2C& msg);
	void onMsgJoinMapRs(const MsgPB::MsgJoinMapRs& msg);
	void onMsgMapSrvLoadDone(const MsgPB::MsgMapSrvLoadDone& msg);
	void onMsgDeleteAccountS2C(const MsgPB::MsgDeleteAccountS2C& msg);

	void notifyLoginState(HandlerLoginStateEvent::EventType evtState);
	void notifyTranster(HandlerTransterEvent::EventTranster evtTranster);

private:
	NS::tstring m_ip;
	int m_port{};
	NS::tstring m_checkText;

	bool m_bLoginToGs;
	NS::I64 m_nIggId{};
};


