#include "HandlerLogin.h"
#include <stdafx.h>
#include "../common/MsgMgr.h"
#include "HandlerMgr.h"
#include "NetworkModule.h"
#include "LogUtil.h"
#include "data/role/Player.h"
#include "config/MapSettingCfg.h"
#include "config/GlobalCfg.h"

#define IGG_ID 12345678
#define GAME_ID 2078902





//#include <gm/GmLine.h>

#if defined(WIN32)
#else
#endif


using namespace NS;
using namespace MsgPB;

static HandlerLogin* g_HandlerLogin = nullptr;

HandlerLogin::HandlerLogin()
{
    assert(!g_HandlerLogin);
    g_HandlerLogin = this;
}

HandlerLogin::~HandlerLogin()
{
    g_HandlerLogin = nullptr;
}

HandlerLogin& HandlerLogin::getInstance()
{
    assert(g_HandlerLogin);
    return *g_HandlerLogin;
}

bool HandlerLogin::initialize()
{
	REGISTER_S2C(onMsgConnectionState);
	REGISTER_S2C(onMsgLoginResp);
	REGISTER_S2C(onMsgLoginValidate);
	REGISTER_S2C(onMsgPlayerInfoGS2C);
	REGISTER_S2C(onMsgJoinMapG2C);
	REGISTER_S2C(onMsgPublicDropListS2C);
	REGISTER_S2C(onMsgJoinMapRs);
	REGISTER_S2C(onMsgMapSrvLoadDone);
	REGISTER_S2C(onMsgPlayerSetAvatorResultS2C);
	REGISTER_S2C(onMsgPlatformAccountCheckLS2C);
	REGISTER_S2C(onMsgRegisterAccountLS2C);
	REGISTER_S2C(onMsgDeleteAccountS2C);

	m_bLoginToGs = false;
	//NetworkModule::getInstance().connectToServer("192.168.0.107", 5999);
	NetworkModule::getInstance().connectToServer(GlobalCfg::getInstance().GetLoginIp(), GlobalCfg::getInstance().GetLoginPort());
	return true;
}

void HandlerLogin::loginToLS(NS::I64 nIggId)
{
	MsgClientLoginToLS msg;
	msg.set_iggid(nIggId);
	MsgMgr::getInstance().sendMessageDirect(msg);
	notifyLoginState(HandlerLoginStateEvent::EventType::loginLs);
}

void HandlerLogin::checkPlatformAccount(std::string strAccount, std::string strPwd, int platform, int client_ver)
{
	MsgPlatformAccountCheckC2LS msg;
	msg.set_account(strAccount);
	msg.set_pwd(strPwd);
	msg.set_platform(platform);
	msg.set_client_ver(client_ver);
	MsgMgr::getInstance().sendMessageDirect(msg);
}

void HandlerLogin::registerAccount(std::string strAccount, std::string strPwd, int platform)
{
	MsgRegisterAccountC2LS msg;
	msg.set_account(strAccount);
	msg.set_pwd(strPwd);
	msg.set_platform(platform);
	MsgMgr::getInstance().sendMessageDirect(msg);
}

void HandlerLogin::loginToGS()
{
	MsgClientLoginToGS msg;

	msg.set_iggid(m_nIggId);
	msg.set_checktext(m_checkText);
	msg.set_nickname("guest");
	msg.set_gameid(GAME_ID);
	msg.set_deviceid(1);
	msg.set_device("pc");
	msg.set_versioncode(1);
	msg.set_client_svn_ver(1);
	msg.set_cliendmode(true);
	msg.set_is_adult(false);
	//msg.set_uploaddata();
	MsgMgr::getInstance().sendMessageDirect(msg);
	notifyLoginState(HandlerLoginStateEvent::EventType::loginGs);
}

void HandlerLogin::onMsgLoginResp(const MsgPB::MsgLoginResp& msg)
{
	LOGI("HandlerLogin::onMsgLoginResp res = {0}" , msg.loginresult());
	if (msg.loginresult() == LOGIN_RESULT::SUCCESS)
	{
		m_nIggId = msg.iggid();
	}
}


void HandlerLogin::onMsgLoginValidate(const MsgPB::MsgLoginValidate& msg)
{
	LOGI("HandlerLogin::onMsgLoginValidate m_ip = {0} , m_port = {1}" , msg.ip().c_str(), msg.port());
	m_ip = msg.ip();
	m_nIggId = msg.iggid();
	m_port = msg.port();
	m_checkText = msg.checktext();
	m_bLoginToGs = true;
	NetworkModule::getInstance().connectToServer(GlobalCfg::getInstance().GetLoginIp(), m_port);
	//NetworkModule::getInstance().connectToServer(m_ip, m_port);
}


void HandlerLogin::terminate()
{
}

void HandlerLogin::update(float dt)
{
	Super::update(dt);
}

void HandlerLogin::onMsgPlatformAccountCheckLS2C(const MsgPB::MsgPlatformAccountCheckLS2C& msg)
{
	if (msg.iggid() == 0 || msg.account().empty())
	{
		POST_DATA_EVENT(msg);
		return;
	}
	extern bool g_bShowVideo;
	g_bShowVideo = msg.nvideo() == 1;
	GameAppExtern::Login(msg.iggid());//1001152);//
}

void HandlerLogin::onMsgRegisterAccountLS2C(const MsgPB::MsgRegisterAccountLS2C& msg)
{
	POST_DATA_EVENT(msg);
}

void HandlerLogin::onMsgDeleteAccountS2C(const MsgPB::MsgDeleteAccountS2C& msg)
{
	POST_DATA_EVENT(msg);
}

void HandlerLogin::onMsgConnectionState(const MsgPB::MsgConnectionState& msg)
{
	CONNECTION_STATE state = (CONNECTION_STATE)(msg.estate());
	switch (state)
	{
	case CS_CONNECTING:	// Connecting.
		LOGI("HandlerLogin::onMsgConnectionState Connecting... ");
		break;

	case CS_CONNECTED:	// Connected.
	{
		LOGI("HandlerLogin::onMsgConnectionState Connect succeeded.");
		if (!m_bLoginToGs){
			//loginToLS();
		}
		else {
			loginToGS();
		}
	}
	break;

	case CS_BREAK:
	case CS_FAILED:
		// Fall through.
		{
			HandlerNetEvent evt;
			if (state == CS_BREAK) {
				evt.m_type = HandlerNetEvent::EventType::NET_BREAK;
			} else {
				evt.m_type = HandlerNetEvent::EventType::NET_FAILED;
			}
			POST_DATA_EVENT(evt);
			LOGI("HandlerLogin::onMsgConnectionState Connect failed.");
		}
		break;
	default:
		break;
	}
}


void HandlerLogin::notifyLoginState(HandlerLoginStateEvent::EventType evtState)
{
	HandlerLoginStateEvent evt;
	evt.m_type = evtState;
	POST_DATA_EVENT(evt);
}

void HandlerLogin::notifyTranster(HandlerTransterEvent::EventTranster evtTranster)
{
	HandlerTransterEvent evt;
	evt.m_type = evtTranster;
	POST_DATA_EVENT(evt);
}

void HandlerLogin::SetAvatar(std::string strName, int nSex, int nHairStyle, int nHairColor, int nSkinColor)
{
	MsgPlayerSetAvatorC2S msg;
	msg.set_firstlogin(true);
	msg.set_iggid(m_nIggId);
	msg.set_nickname(strName.c_str());
	msg.set_gender(nSex);
	msg.set_hair(nHairStyle);
	msg.set_haircolor(nHairColor);
	msg.set_skincolor(nSkinColor);

	MsgMgr::getInstance().sendMessageDirect(msg);
}

void HandlerLogin::onMsgPlayerSetAvatorResultS2C(const MsgPB::MsgPlayerSetAvatorResultS2C& msg)
{
	POST_DATA_EVENT(msg);
}


void HandlerLogin::onMsgPlayerInfoGS2C(const MsgPB::MsgPlayerInfoGS2C& msg)
{
	//PlayerMgr::getInstance().addPlayer(msg);
	LOGI("HandlerLogin::onMsgPlayerInfoGS2C name = {0}" , msg.nickname());
	if (msg.iggid() != m_nIggId)
	{
		return;
	}
	if (msg.gender() == -1)
	{
		notifyTranster(HandlerTransterEvent::EventTranster::EventTranster_CreateRole);
	}
	else {
		//notifyTranster(HandlerTransterEvent::EventTranster::EventTranster_PrivateMap);
		
	}
}

void HandlerLogin::onMsgJoinMapG2C(const MsgPB::MsgJoinMapG2C& msg)
{
	LOGI("HandlerLogin::onMsgJoinMapG2C join map");
	POST_DATA_EVENT(msg);
	//MapDataMgr::getInstance().setMapId(msg.mapid());
	//notifyTranster(HandlerTransterEvent::EventTranster::EventTranster_PrivateMap);
}

void HandlerLogin::onMsgPublicDropListS2C(const MsgPB::MsgPublicDropListS2C& msg)
{
	MsgMapClientCreateMapDoneC2G msgSend;  //创建地图完成
	MsgMgr::getInstance().sendMessageDirect(msgSend);
}

void HandlerLogin::requesetMapDone()
{
	MsgMapClientUploadDoneReq msg;   //上传缓存完成
	MsgMgr::getInstance().sendMessageDirect(msg);
}

void HandlerLogin::onMsgJoinMapRs(const MsgPB::MsgJoinMapRs& msg)
{
	LOGI("HandlerLogin::MsgJoinMapRs failed");
	HandlerEnterMapStateEvent evt;
	if (msg.result() == JOIN_MAP_SUCESS) {
		evt.m_type = HandlerEnterMapStateEvent::EnterMapState::EnterMapState_Suc;
	}
	else {
		evt.m_type = HandlerEnterMapStateEvent::EnterMapState::EnterMapState_Fail;
	}
	POST_DATA_EVENT(evt);
}

void HandlerLogin::enterMap()
{
	const std::shared_ptr<Player> pPlayer = PlayerMgr::getInstance().getSelf();
	if (!pPlayer || pPlayer->getGender() == -1) return;

	int nCurMapId = MapDataMgr::getInstance().getMapId();
	if (nCurMapId == 0)
	{
		return;
	}
	auto&& pMap = MapSettingCfg::getInstance().findMap(nCurMapId);

	if (pMap) {
		MapDataMgr::getInstance().setCurMapType((MapDataMgr::MapType)pMap->type);
		if (pMap->type == (int)MapDataMgr::MapType::Dungeon) {
			LOGI("HandlerLogin::Dungeon join done mapId = {0}", nCurMapId);
			notifyTranster(HandlerTransterEvent::EventTranster::EventTranster_DungeonMap);
		}
		else if (pMap->type == (int)MapDataMgr::MapType::Private) {
			LOGI("HandlerLogin::Private join done mapId = {0}", nCurMapId);
			if (MapDataMgr::getInstance().getMapId() != 0) {
				LOGI("HandlerLogin::Private join done mapId 1 = {0}", nCurMapId);
				notifyTranster(HandlerTransterEvent::EventTranster::EventTranster_PrivateMap);
			}
		}
	}
}

void HandlerLogin::onMsgMapSrvLoadDone(const MsgPB::MsgMapSrvLoadDone& msg)
{
	LOGI("HandlerLogin::onMsgMapSrvLoadDone join done mapId = {0}", msg.mapid());
	
	MapDataMgr::getInstance().setMapId(msg.mapid());

	enterMap();
}


