#include "HandlerFriend.h"
/*!
 * \file HandlerFriend.cpp
 *
 * \author baomingcheng
 * \date 10/30/2017
 *
 */
#include "stdafx.h"
#include "HandlerFriend.h"
#include "../common/MsgMgr.h"
#include "msg/MsgType.pb.h"
//#include "HandlerChat.h"
//#include "msg/MsgFriend.pb.h"
#include "data/role/Player.h"
//#include "data/ChatData.h"

//#include "ui/common/UiMsgBoxToast.h"
//#include "handler/HandlerLogin.h"

using namespace NS;
using namespace MsgPB;

static HandlerFriend* g_HandlerFriend = nullptr;
HandlerFriend& HandlerFriend::getInstance()
{
    assert(g_HandlerFriend);
    return *g_HandlerFriend;
}

HandlerFriend::HandlerFriend()
{
    assert(!g_HandlerFriend);
    g_HandlerFriend = this;
}

HandlerFriend::~HandlerFriend()
{
    g_HandlerFriend = nullptr;
}

bool HandlerFriend::initialize()
{	
	//REGISTER_S2C(onMsgFriendSearchResultS2C);
	//REGISTER_S2C(onMsgFriendListQueryResultS2C);
	//REGISTER_S2C(onMsgFriendApplyResultS2C);
	//REGISTER_S2C(onMsgFriendApplyConfirmS2C);
	//REGISTER_S2C(onMsgFriendApplyConfirmResultS2C);
	//REGISTER_S2C(onMsgFriendApplyListS2C);
	//REGISTER_S2C(onMsgFriendDeleteResultS2C);
	//REGISTER_S2C(onMsgFriendBlacklistS2C);
	//REGISTER_S2C(onMsgFriendPushBlackResultS2C);
	//REGISTER_S2C(onMsgFriendMoveBlackResultS2C);
	//REGISTER_S2C(onMsgFriendGiftDataResultS2C);
	//REGISTER_S2C(onMsgFriendGiftReciveResultS2C);
	//REGISTER_S2C(onMsgFriendGiftSendResult);
	//
	//REGISTER_EVENT(&HandlerChat::getInstance(), HandlerChatEvent, &HandlerFriend::onHandlerChatEvent);
	return true;
}

void HandlerFriend::terminate()
{
}

bool HandlerFriend::reset()
{
	return true;
}
/*
bool HandlerFriend::FriendListQuery(int iggid)
{
	MsgPB::MsgFriendQuery  msg{};
	msg.set_iggid(iggid);
	return MsgMgr::getInstance().sendMessage(_MSG_FRIEND_QUERY, &msg);
}

bool HandlerFriend::FriendApply(int iggId, int targetIggId, bzbee::tstring applyMsg)
{
	if (!checkNotInBlackList(targetIggId, BlockDir::AtHis)) {
		UiMsgBoxToast::getInstance().showText(_LC("CLI_Friend-you are in his(her) Blcaklist"), "");
		return false;
	}
	if (!checkNotInBlackList(targetIggId, BlockDir::AtMy)) {
		UiMsgBoxToast::getInstance().showText(_LC("CLI_Friend-he(she) are in your Blcaklist,please remove"), "");
		return false;
	}
	if (!HandlerLogin::getInstance().isConnect())
	{
		UiMsgBoxToast::getInstance().showText(_LC("CLI_Common-network is diconnect."));
		return false;
	}
	MsgPB::MsgFriendApply  msg{}; 
	msg.set_iggid(iggId);
	msg.set_targetiggid(targetIggId);
	msg.set_applymsg(applyMsg);
	return MsgMgr::getInstance().sendMessage(_MSG_FRIEND_APPLY, &msg);
}


bool HandlerFriend::sendGiftGet(NS::I64 friIggid)
{
	if (!HandlerLogin::getInstance().isConnect())
	{
		UiMsgBoxToast::getInstance().showText(_LC("CLI_Common-network is diconnect."));
		return false;
	}
	if (getGiftRecvCount() > getMaxGiftRecvCount())
	{
		UiMsgBoxToast::getInstance().showText(_LC("CLI-Friend-GIFT Max Recv"), "");
		return false;
	}
	const MsgPB::MsgFriendGiftData* pData = getFriendGift(friIggid);
	if (pData != nullptr && pData->iitemid() >= 100
		&& ItemPacketMgr::getInstance().isCanGetInDrop(pData->iitemid(), pData->iitemnum()))
	{
		UiMsgBoxToast::getInstance().showText(_LC("CLI-Friend-GIFT Bag Max"), "");
		return false;
	}

	MsgPB::MsgFriendGiftRecv msg{};
	msg.set_iggid(0);
	msg.set_friendiggid(friIggid);
	return MsgMgr::getInstance().sendMessage(_MSG_FRIEND_GIFTRECV, &msg);
}

void HandlerFriend::onMsgFriendListQueryResultS2C(const MsgPB::MsgFriendList& msg)
{
	m_friendsLst.clear();

	m_friendLimitNum = msg.limitfrinednum();
	if (msg.data_size() > 0)
	{
		for (MsgPB::BaseData data : msg.data())
		{
			m_friendsLst.insert(std::make_pair(data.iggid(), data));
			addCacheData(data);
		}
	}
	HandlerFriendEvent  evt{};
	evt.m_type = HandlerFriendEvent::EventType::refreshFriendList;
	evt.pBaseDataLst = &m_friendsLst;
	postEvent(evt);
}

void HandlerFriend::onMsgFriendApplyResultS2C(const MsgPB::MsgFriendApplyResult& msg)
{
	if (msg.result() == MsgPB::EFriendResult::FRIEND_RESULT_SUCCESS)
	{
		UiMsgBoxToast::getInstance().showText(_LC("CLI_Friend-SUCCESS_SEND_APPLY"), "");
	}
	else
	{
		NS::tstring errorStr = getErrorStr(msg.result());
		showErrorMsg(errorStr);
	}
}

void HandlerFriend::onMsgFriendApplyConfirmS2C(const MsgPB::MsgFriendConfirm & msg)
{
	//  直接进申请列表   无须弹窗确认
	if (msg.has_applydata())
	{
		const MsgPB::BaseData& data = msg.applydata();
		std::unordered_map<NS::I64, MsgPB::BaseData>::iterator ite = m_applyLst.find(data.iggid());
		if (ite != m_applyLst.end())
		{
			ite->second = data;
		}
		else
		{
			m_applyLst.insert(std::make_pair(data.iggid(), data));
		}
		addCacheData(data);
	}
	setFriendRedState(Friend_Red::Friend_Red_Apply , true);
	HandlerFriendEvent  evt{};
	evt.m_type = HandlerFriendEvent::EventType::refreshApplyList;
	evt.pBaseDataLst = &m_applyLst;
	postEvent(evt);
}

bool HandlerFriend::FriendApplyConfirmByIggId(NS::int32 confirmResult, NS::I64 iggId, NS::I64 targetIggId)
{
	if (!HandlerLogin::getInstance().isConnect())
	{
		UiMsgBoxToast::getInstance().showText(_LC("CLI_Common-network is diconnect."));
		return false;
	}
	m_applyLst.erase(targetIggId);

	MsgFriendConfirmResult msg{};
	msg.set_result(confirmResult);
	msg.set_iggid(iggId);
	msg.set_targetiggid(targetIggId);

	HandlerFriendEvent  evt{};
	evt.m_type = HandlerFriendEvent::EventType::refreshApplyList;
	evt.pBaseDataLst = &m_applyLst;
	postEvent(evt);

	return MsgMgr::getInstance().sendMessage(_MSG_FRIEND_CONFIRM_RESULT, &msg);
}

bool HandlerFriend::FriendApplyConfirm(NS::int32 confirmResult, NS::I64 iggid, const MsgPB::BaseData& data)
{
	return FriendApplyConfirmByIggId(confirmResult , iggid , data.iggid());
}


void HandlerFriend::ProcessFriendApplyRequest(const MsgPB::BaseData& data, bool isPass)
{
	if (isPass && static_cast<int32>(m_friendsLst.size()) >= FriendLimitNum()) 
	{
		showErrorMsg(_LC("CLI_Friend-FAILDED_FULL_COUNT"));
		return;
	}
	if (isPass)
	{
		std::unordered_map<NS::I64, MsgPB::BaseData>::iterator ite = m_friendsLst.find(data.iggid());
		if (ite != m_friendsLst.end())
		{
			ite->second = data;
		}
		else
		{
			m_friendsLst.insert(std::make_pair(data.iggid(), data));
		}
		addCacheData(data);
		FriendApplyConfirm(1, 0, data);
	}
	else
	{
		FriendApplyConfirm(MsgPB::FRIEND_RESULT_FAILDED_REFUSE_APPLY, 0, data);
	}

}

void HandlerFriend::onMsgFriendApplyConfirmResultS2C(const MsgPB::MsgFriendConfirmResult & msg)
{
	if (msg.result() == MsgPB::EFriendResult::FRIEND_RESULT_SUCCESS)
	{
		if (msg.has_confirmdata())
		{
			const MsgPB::BaseData& data = msg.confirmdata();
			std::unordered_map<NS::I64, MsgPB::BaseData>::iterator ite = m_friendsLst.find(data.iggid());
			if (ite != m_friendsLst.end())
			{
				ite->second = data;
			}
			else
			{
				m_friendsLst.insert(std::make_pair(data.iggid(), data));
			}
			addCacheData(data);
		}
		setFriendRedState(Friend_Red::Friend_Red_NewFriend, true);
		HandlerFriendEvent  evt{};
		evt.m_type = HandlerFriendEvent::EventType::refreshFriendList;
		evt.pBaseDataLst = &m_friendsLst;
		postEvent(evt);
	}
}

bool HandlerFriend::FriendSearch(NS::I64 searchIGGID, NS::tstring searchName)
{
	if (!HandlerLogin::getInstance().isConnect())
	{
		UiMsgBoxToast::getInstance().showText(_LC("CLI_Common-network is diconnect."));
		return false;
	}
	MsgFriendSearch msg{};
	msg.set_searchiggid(searchIGGID);
	msg.set_searchname(searchName);
	return MsgMgr::getInstance().sendMessage(_MSG_FRIEND_SEARCH, &msg);
}

void HandlerFriend::addCacheData(const MsgPB::BaseData& data)
{
	if (m_cacheLst.find(data.iggid()) == m_cacheLst.end()) {
		m_cacheLst.insert(std::make_pair(data.iggid(), data));
	}
	else {
		m_cacheLst[data.iggid()] = data;
	}
}


NS::tstring HandlerFriend::getErrorStr(int errrorCode)
{
	NS::tstring errorStr{};

	switch (errrorCode)
	{
	case MsgPB::FRIEND_RESULT_FAILED_ALREADY_APPLY:						//失败：己经申请
		errorStr = _LC("CLI_Friend-FAILED_ALREADY_APPLY");
		break;
	case MsgPB::FRIEND_RESULT_FAILED_NOT_FRIEND:						//失败：不是好友关系
		errorStr = _LC("CLI_Friend-FAILED_NOT_FRIEND");
		break;
	case MsgPB::FRIEND_RESULT_INVALID_FRIEND_USER_ID:					//失败：无效的好友ID
		errorStr = _LC("CLI_Friend-INVALID_FRIEND_USER_ID");
		break;
	case MsgPB::FRIEND_RESULT_FAILDED_FAKE_USER_ID:						//失败：无效的玩家ID
		errorStr = _LC("CLI_Friend-FAILDED_FAKE_USER_ID");
		break;
	case MsgPB::FRIEND_RESULT_FAILDED_FULL_COUNT:						//失败：人数达到上限
		errorStr = _LC("CLI_Friend-FAILDED_FULL_COUNT");
		break;
	case MsgPB::FRIEND_RESULT_FAILDED_REFUSE_APPLY:						//失败：拒绝请求
		errorStr = _LC("CLI_Friend-FAILDED_REFUSE_APPLY");
		break;
	default:
		break;
	}
	return errorStr;
}

void HandlerFriend::showErrorMsg(const NS::tstring& content)
{
	if (content.empty()) return;
	
	UiMsgBoxToast::getInstance().showText(content, "");
}

void HandlerFriend::onMsgFriendSearchResultS2C(const MsgPB::MsgFriendSearchResult& msg)
{
	if (msg.result() == 1)
	{
		auto data = msg.basedata();
		m_searchLst.clear();
		
		if (data.iggid()>0)
		{
			m_searchLst.insert(std::make_pair(data.iggid(), data));
			addCacheData(data);
		}
		else
		{
			UiMsgBoxToast::getInstance().showText(_LC("CLI-Friend-SearchNone"), "");
		}
		HandlerFriendEvent  evt{};
		evt.m_type = HandlerFriendEvent::EventType::refreshSearchList;
		evt.pBaseDataLst = &m_searchLst;
		postEvent(evt);
	}
	else
	{
		UiMsgBoxToast::getInstance().showText(_LC("CLI-Friend-SearchNone"), "");
		m_searchLst.clear();	
		HandlerFriendEvent  evt{};
		evt.m_type = HandlerFriendEvent::EventType::refreshSearchList;
		evt.pBaseDataLst = &m_searchLst;
		postEvent(evt);
	}
}

void HandlerFriend::onMsgFriendApplyListS2C(const MsgPB::MsgFriendApplyList & msg)
{
	m_applyLst.clear();
	if (msg.basedata_size() > 0)
	{
		for (const MsgPB::BaseData& data : msg.basedata())
		{
			m_applyLst.insert(std::make_pair(data.iggid(), data));
			addCacheData(data);
		}
	}

	HandlerFriendEvent  evt{};
	evt.m_type = HandlerFriendEvent::EventType::refreshApplyList;
	evt.pBaseDataLst = &m_applyLst;
	postEvent(evt);

}

bool HandlerFriend::FriendApplyQuery(NS::I64 iggid)
{
	MsgFriendApplyQuery msg{};
	msg.set_iggid(iggid);
	return MsgMgr::getInstance().sendMessage(_MSG_FRIEND_APPLY_QUERY, &msg);
}

bool HandlerFriend::FriendDel(NS::I64 iggId, NS::I64 friendIggId)
{
	if (!HandlerLogin::getInstance().isConnect())
	{
		UiMsgBoxToast::getInstance().showText(_LC("CLI_Common-network is diconnect."));
		return false;
	}
	MsgFriendDelete msg{};
	msg.set_friendiggid(friendIggId);
	msg.set_iggid(iggId);
	return MsgMgr::getInstance().sendMessage(_MSG_FRIEND_DELETE, &msg);
}

void HandlerFriend::onMsgFriendDeleteResultS2C(const MsgPB::MsgFriendDeleteResult& msg)
{
	if (msg.result() == FRIEND_RESULT_SUCCESS)
	{
		m_friendsLst.erase(msg.friendplayerid());

		HandlerFriendEvent evt{};
		evt.m_type = HandlerFriendEvent::EventType::refreshFriendList;
		evt.pBaseDataLst = &m_friendsLst;
		postEvent(evt);
	}
}

bool HandlerFriend::isMyFriend(NS::I64 iggId)
{
	std::unordered_map<NS::I64, MsgPB::BaseData>::const_iterator ito = m_friendsLst.find(iggId);
	if (ito != m_friendsLst.end())
	{
		return true;
	}
	return false;
}

void HandlerFriend::onHandlerChatEvent(const HandlerChatEvent& evt)
{
	if (evt.m_type == HandlerChatEvent::EventType::ChatNewUpdate)
	{
		if (ChatData::getInstance().checkedIsPause()) return;

		auto&& pMsgData = ChatData::getInstance().findMsgById(evt.m_id);
		if (!pMsgData) return;
		if (pMsgData->getChannel() < ChatData::Channel::GroupChannelMax) return;

		const auto& self = PlayerMgr::getInstance().getSelf();
		if (!self) return;
		if (self->getIggId() != pMsgData->getSendIggid() && self->getIggId() != pMsgData->getRevIggid()) return;
		if (pMsgData->getSendIggid() != self->getIggId())
			m_mapPlayerChats[pMsgData->getSendIggid()] = pMsgData->getSendTime();
		if ( 0 == m_lastChatIggId)
		{
			if (pMsgData->getSendIggid() != self->getIggId())
			{
				m_lastChatIggId =  pMsgData->getSendIggid();
				m_lastChatName = pMsgData->getSendName();
			}			
		}
		if (pMsgData->getSendIggid() != self->getIggId() && !isMyFriend(pMsgData->getSendIggid()))
		{			
			MsgPB::BaseData  baseData{};
			baseData.set_iggid(pMsgData->getSendIggid());
			baseData.set_name(pMsgData->getSendName());
			int level{};
			StringUtil::str2int(level, pMsgData->getSendRoleInfo().m_level.c_str());
			baseData.set_level(level);
			int gender{};
			StringUtil::str2int(gender, pMsgData->getSendRoleInfo().m_gender.c_str());
			baseData.set_gender(gender);

			if (m_strangerLst.find(baseData.iggid()) == m_strangerLst.end())
			{
				m_strangerLst.insert(std::make_pair(baseData.iggid(), baseData));
			}
		}

		if (self->getIggId() != pMsgData->getSendIggid() &&
			pMsgData->getSendIggid() > 0)
		{
			if (!isMyFriend(pMsgData->getSendIggid())) {
				setFriendRedState(Friend_Red::Friend_Red_Stranger, true);
			}
			else {
				setFriendRedState(Friend_Red::Friend_Red_NewFriend, true);
			}
			if (m_chatIggs.find(pMsgData->getSendIggid()) == m_chatIggs.end())
			{
				m_chatIggs.insert(std::make_pair<NS::I64, bool>(pMsgData->getSendIggid(), true));
			}
			//if (m_lastChatIggId == 0 || m_lastChatIggId == pMsgData->getSendIggid()) {
			//	//setFriendRedState(Friend_Red::Friend_Red_Chat, true);
			//}
			//else {
			//	
			//}
		}
		

		HandlerStrangerChatEvent  evt{};
		evt.m_type = HandlerStrangerChatEvent::EventType::NewStrangerChat;
		evt.m_pMsgData = pMsgData;
		postEvent(evt);
	}
}


std::vector<NS::I64> HandlerFriend::lua_getChatIggIds()
{
	std::vector<NS::I64> vecTable;
	for (auto&& it : m_chatIggs)
	{
		vecTable.push_back(it.first);
	}
	return vecTable;
}

void HandlerFriend::lua_clearChatIggId(NS::I64 iggId)
{
	auto iter = m_chatIggs.find(iggId);
	if (iter != m_chatIggs.end())
	{
		m_chatIggs.erase(iter);
	}
}

// 黑名单功能.
bool HandlerFriend::requitMsgFriendPushBlack(NS::I64 blackIggid)
{
	auto&& self = PlayerMgr::getInstance().getSelf();
	if (!self)
		return false;

	MsgFriendPushBlack msg;
	msg.set_iggid(self->getIggId());
	msg.set_blackiggid(blackIggid);

	return MsgMgr::getInstance().sendMessage(_MSG_FRIEND_PUSHBLACK, &msg);
}

bool HandlerFriend::requitMsgFriendMoveBlack(NS::I64 blackIggid, bool bToAdd)
{
	auto&& self = PlayerMgr::getInstance().getSelf();
	if (!self)
		return false;

	MsgFriendMoveBlack msg;
	msg.set_iggid(self->getIggId());
	msg.set_blackiggid(blackIggid);
	msg.set_add(bToAdd);

	return MsgMgr::getInstance().sendMessage(_MSG_FRIEND_MOVEBLACK, &msg);
}

bool HandlerFriend::checkNotInBlackList(NS::I64 blackIggid, BlockDir bDir)
{
	auto&& findIt = m_blackLst.find(blackIggid);
	bool retBool{ findIt == m_blackLst.end() };

	if(!retBool)
	{
		switch (bDir)
		{
		case HandlerFriend::BlockDir::AtMy:
			retBool = (static_cast<BlockDir>(findIt->second.idatatype()) == BlockDir::AtMy);
			break;
		case HandlerFriend::BlockDir::AtHis:
			retBool = (static_cast<BlockDir>(findIt->second.idatatype()) == BlockDir::AtHis);
			break;
		case HandlerFriend::BlockDir::AtBoth:
			retBool = (static_cast<BlockDir>(findIt->second.idatatype()) == BlockDir::AtBoth);
			break;
		case HandlerFriend::BlockDir::None:
		default:
			break;
		}
	}

	return  retBool;
}

void HandlerFriend::onMsgFriendBlacklistS2C(const MsgPB::MsgFriendBlacklist& msg)
{
	for(auto&& black : msg.blacklist())
	{
		if (black.has_iiblackiggid())
			m_blackLst[black.iiblackiggid()] = black;
	}
}

void HandlerFriend::onMsgFriendPushBlackResultS2C(const MsgPB::MsgFriendPushBlackResult& msg)
{
	if (msg.result() == FRIEND_RESULT_SUCCESS)
	{
		auto&& blackUser = msg.blackuser();
		if (blackUser.iiblackiggid() == 0)
			return;

		m_blackLst[blackUser.iiblackiggid()] = blackUser;

		HandlerFriendEvent evt{};
		evt.m_type = HandlerFriendEvent::EventType::refreshBlackList;
		postEvent(evt);
	}
	else
	{
		UiMsgBoxToast::getInstance().showText(_LC("CLI_HandlerFriend_MsgItemExchangeResultGS2C-Friend Push Black Faild."));
		LOGE("CLI_HandlerFriend_MsgItemExchangeResultGS2C-Friend Push Black Faild. {0}", msg.result());
	}
}

void HandlerFriend::onMsgFriendMoveBlackResultS2C(const MsgPB::MsgFriendMoveBlackResult & msg)
{
	if (msg.result() == FRIEND_RESULT_SUCCESS)
	{
		auto&& findIt = m_blackLst.find(msg.blackiggid());
		if(findIt != m_blackLst.end())
		{
			m_blackLst.erase(findIt);
		}

		HandlerFriendEvent evt{};
		evt.m_type = HandlerFriendEvent::EventType::refreshBlackList;
		postEvent(evt);
	}
	else
	{
		UiMsgBoxToast::getInstance().showText(_LC("CLI_HandlerFriend_MsgItemExchangeResultGS2C-Friend Move Black Faild."));
		LOGE("CLI_HandlerFriend_MsgItemExchangeResultGS2C-Friend Move Black Faild. {0}", msg.result());
	}
}


void HandlerFriend::onMsgFriendGiftDataResultS2C(const MsgPB::MsgFriendGiftInfo& msg)
{
	m_iGiftRecvCnt = msg.irecvcnt();
	m_iGiftSentCnt = msg.isendcnt();
	m_giftLst.clear();
	for (int i = 0 ; i < msg.gift_size() ; i++)
	{
		m_giftLst.insert(std::make_pair(msg.gift(i).friendiggid() , msg.gift(i)));
	}
	HandlerFriendEvent  evt{};
	evt.m_type = HandlerFriendEvent::EventType::refreshGiftList;
	postEvent(evt);
}

void HandlerFriend::onMsgFriendGiftReciveResultS2C(const MsgPB::MsgFriendGiftRecvResult& msg)
{
	if (msg.result() == FRIEND_RESULT_SUCCESS)
	{
		m_iGiftRecvCnt++;
		HandlerFriendEvent  evt{};
		evt.m_type = HandlerFriendEvent::EventType::getGiftResult;
		evt.m_friIggid = msg.friendiggid();
		postEvent(evt);
		UiMsgBoxToast::getInstance().showText(_LC("CLI-Friend Recive Gift Success") , "");
	}
	else {
		UiMsgBoxToast::getInstance().showText(fmt::format(_LC("Friend Recive Gift Faild Result = {0}") , msg.result()));
	}
}

void HandlerFriend::onMsgFriendGiftSendResult(const MsgPB::MsgFriendGiftSendResult& msg)
{
	if (msg.result() == FRIEND_RESULT_SUCCESS)
	{
		auto iter = m_friendsLst.find(msg.friendiggid());
		if (iter != m_friendsLst.end()) {
			iter->second.set_sendgift(1);
		}
		m_iGiftSentCnt++;

		HandlerFriendEvent  evt{};
		evt.m_type = HandlerFriendEvent::EventType::sendGiftResult;
		postEvent(evt);
	}
}


LuaTable HandlerFriend::getRecvAndSendCnt()
{
	LuaTable tableData;
	tableData.set("m_iGiftSentCnt", m_iGiftSentCnt);
	tableData.set("m_iGiftRecvCnt" , m_iGiftRecvCnt);
	return tableData;
}

void HandlerFriend::setFriendRedState(Friend_Red state, bool bState)
{
	if (bState) {
		m_firRedState |= state;
	}
	else {
		m_firRedState &= (~state);
	}
}

bool HandlerFriend::checkFriendRedState(Friend_Red state)
{
	return ((m_firRedState & state) == state);
}


const MsgPB::MsgFriendGiftData* HandlerFriend::getFriendGift(NS::I64 iggid)
{
	auto iter = m_giftLst.find(iggid);
	if (iter != m_giftLst.end())
	{
		return &(iter->second);
	}
	return nullptr;
}

LuaTable HandlerFriend::lua_getFriendGift(NS::I64 iggid)
{
	LuaTable table;
	auto&& pData = getFriendGift(iggid);
	if (pData)
	{
		table.set("isGift", true);
		table.set("itemId", pData->iitemid());
		table.set("itemNum" , pData->iitemnum());
	}
	else
	{
		table.set("isGift", false);
	}
	return table;
}


NS::I64 HandlerFriend::getSendChatTm(NS::I64 iggId)
{
	auto iter = m_mapPlayerChats.find(iggId);
	if (iter != m_mapPlayerChats.end()) {
		return iter->second;
	}
	return 0;
}

std::vector<LuaTable> HandlerFriend::lua_getFriendList(int tpy)
{
	auto mTpy = static_cast<Friends_tpy>(tpy);
	std::vector<LuaTable> vecTable;
	if (mTpy == Friends_tpy::Friends_tpy_frilist)
	{
		for (auto&& data : m_friendsLst)
		{
			LuaTable table;
			table_from_message(table, data.second);
			vecTable.push_back(table);
		}
	}
	else if(mTpy == Friends_tpy::Friends_tpy_search)
	{
		for (auto&& data : m_searchLst)
		{
			LuaTable table;
			table_from_message(table, data.second);
			vecTable.push_back(table);
		}
	}
	else if (mTpy == Friends_tpy::Friends_tpy_apply)
	{
		for (auto&& data : m_applyLst)
		{
			LuaTable table;
			table_from_message(table, data.second);
			vecTable.push_back(table);
		}
	}
	else if (mTpy == Friends_tpy::Friends_tpy_cache)
	{
		for (auto&& data : m_cacheLst)
		{
			LuaTable table;
			table_from_message(table, data.second);
			vecTable.push_back(table);
		}
	}
	else if (mTpy == Friends_tpy::Friends_tpy_stranger)
	{
		for (auto&& data : m_strangerLst)
		{
			LuaTable table;
			table_from_message(table, data.second);
			vecTable.push_back(table);
		}
	}
	else if (mTpy == Friends_tpy::Friends_tpy_black)
	{
		for (auto&& data : m_blackLst)
		{
			LuaTable table;
			table_from_message(table, data.second);
			vecTable.push_back(table);
		}
		return vecTable;
	}

	std::sort(vecTable.begin(), vecTable.end(), [&](const LuaTable& lhs, const LuaTable& rhs) {
		NS::I64 loutTim{};
		NS::I64 routTim{};
		lhs.get("LogoutTime", loutTim);
		rhs.get("LogoutTime", routTim);
		if (loutTim != routTim)
		{
			return loutTim < routTim;
		}

		NS::I64 lIggid{};
		NS::I64 rIggId{};
		lhs.get("iggid", lIggid);
		rhs.get("iggid", rIggId);

		return lIggid < rIggId;
	});
	return vecTable;
}

void HandlerFriend::lua_regChatEvent(const LuaCallback& invoker)
{
	REG_EVENT(this)[=](const HandlerStrangerChatEvent& evt) {
		invoker(static_cast<int>(evt.m_type), evt.m_pMsgData->getSendIggid() , evt.m_pMsgData->getRevIggid() , evt.m_pMsgData->getId());
	};
}

void HandlerFriend::lua_unregChatEvent()
{
	UNREG_EVENT(this, HandlerStrangerChatEvent);
}

void HandlerFriend::lua_regFriendEvent(const LuaCallback& invoker)
{
	REG_EVENT(this)[=](const HandlerFriendEvent& evt) {
		invoker(static_cast<int>(evt.m_type), evt.m_friIggid);
	};
}

void HandlerFriend::lua_unregFriendEvent()
{
	UNREG_EVENT(this, HandlerFriendEvent);
}

*/
