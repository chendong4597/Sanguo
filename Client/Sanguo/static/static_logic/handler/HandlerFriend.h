/*!
* \file HandlerFriend.h
*
* \author baomincheng
* \date 11/21/2017
*
*/
#pragma once

#include "BaseTypeDefine.h"
#include <MsgHandler.h>
#include "../common/MsgMgr.h"
#include <msg/MsgType.pb.h>
#include "Cpp11EventSystem.h"
//#include "msg/MsgFriend.pb.h"
//#include "data/ChatData.h"


class HandlerChatEvent;

class HandlerFriend : public bzbee::MsgHandler
{
	DECLARE_EVENT_HOST();
public:
	RTTI_DEF(HandlerFriend);

	enum class BlockDir : NS::uint
	{
		None	= 0, 
		AtMy	= 1,				// 被我添加到黑名单.
		AtHis	= 2,				// 被对方添加到黑名单.
		AtBoth	= 3,				// 双方多添加了黑名单.
	};

	enum Friend_Red
	{
		Friend_Red_Null = 0,
		Friend_Red_NewFriend = 0x000001,
		Friend_Red_Apply = 0x000002,
		Friend_Red_Chat = 0x000004,
		Friend_Red_Stranger = 0x000008,
	};

	enum Friends_tpy
	{
		Friends_tpy_search,
		Friends_tpy_frilist,
		Friends_tpy_apply,
		Friends_tpy_cache,
		Friends_tpy_stranger,
		Friends_tpy_black
	};

public:
	static HandlerFriend& getInstance();

	HandlerFriend();
	virtual ~HandlerFriend();

	bool initialize() override;
	void terminate() override;
	virtual bool reset() override;

//	bool METHOD(FriendDel)(NS::I64 iggId, NS::I64 friendIggId);
//	bool METHOD(FriendApplyQuery)(NS::I64 iggid);
//	bool METHOD(FriendSearch)(NS::I64 searchIGGID,NS::tstring searchName);
//	bool METHOD(FriendListQuery)(int  iggid);
//	bool METHOD(FriendApply)(int iggId, int targetIggId, bzbee::tstring ApplyMsg);
//	bool METHOD(FriendApplyConfirmByIggId)(NS::int32 confirmResult, NS::I64 iggId, NS::I64 targetIggId);
//	bool FriendApplyConfirm(NS::int32 confirmResult, NS::I64 iggid, const MsgPB::BaseData& data);
//	void ProcessFriendApplyRequest(const MsgPB::BaseData& data, bool isPass);
//	bool METHOD(isMyFriend)(NS::I64 iggId);
//	void onHandlerChatEvent(const HandlerChatEvent& evt);
//	// 好友黑名单功能.
//	bool requitMsgFriendPushBlack(NS::I64 blackIggid);									// 拉黑.
//	bool requitMsgFriendMoveBlack(NS::I64 blackIggid, bool bToAdd = false);				// 移出黑名单[并添加好友]
//	bool checkNotInBlackList(NS::I64 blackIggid, BlockDir bDir = BlockDir::None);			// 是否有在黑名单中
//
//	bool METHOD(sendGiftGet)(NS::I64 friIggid);
//
//	int METHOD(getGiftSendCount)() { return m_iGiftSentCnt; }
//	int METHOD(getMaxGiftSendCount)() { return m_iCfgSentCnt; }
//
//	int METHOD(getGiftRecvCount)() { return m_iGiftRecvCnt; }
//	int METHOD(getMaxGiftRecvCount)() { return m_iCfgRecvCnt; }
//
//	void METHOD(setRecvAndSendCnt)(int recvCnt, int SendCnt) { m_iCfgSentCnt = SendCnt; m_iCfgRecvCnt = recvCnt; }
//	LuaTable METHOD(getRecvAndSendCnt)();
//
//	void METHOD(setFriendRedState)(Friend_Red state, bool bState);
//	bool METHOD(checkFriendRedState)(Friend_Red state);
//	bool METHOD(isFriendRedState)() {return m_firRedState != 0;}
//	void METHOD(clearFriendRedStates)() { m_firRedState = 0; }
//private:
//	void onMsgFriendSearchResultS2C(const MsgPB::MsgFriendSearchResult& msg);
//	void onMsgFriendListQueryResultS2C(const MsgPB::MsgFriendList& msg);
//	void onMsgFriendApplyResultS2C(const MsgPB::MsgFriendApplyResult& msg);
//	void onMsgFriendApplyConfirmS2C(const MsgPB::MsgFriendConfirm & msg);
//	void onMsgFriendApplyConfirmResultS2C(const MsgPB::MsgFriendConfirmResult & msg);
//	void onMsgFriendApplyListS2C(const MsgPB::MsgFriendApplyList & msg);
//	void onMsgFriendDeleteResultS2C(const MsgPB::MsgFriendDeleteResult& msg);
//	// 好友黑名单功能.
//	void onMsgFriendBlacklistS2C(const MsgPB::MsgFriendBlacklist& msg);						// 黑名单列表.
//	void onMsgFriendPushBlackResultS2C(const MsgPB::MsgFriendPushBlackResult& msg);			// 拉黑返回结果.
//	void onMsgFriendMoveBlackResultS2C(const MsgPB::MsgFriendMoveBlackResult& msg);			// 移出黑名单结果.
//
//	void onMsgFriendGiftDataResultS2C(const MsgPB::MsgFriendGiftInfo& msg);			//获取好友礼物数据
//	void onMsgFriendGiftReciveResultS2C(const MsgPB::MsgFriendGiftRecvResult& msg); //领取好友礼物
//
//	void onMsgFriendGiftSendResult(const MsgPB::MsgFriendGiftSendResult& msg);
//	
//	NS::tstring getErrorStr(int errrorCode);
//	void showErrorMsg(const NS::tstring& content);
//
//	
//
//public:
//	const std::unordered_map<NS::I64, MsgPB::BaseData>& SearchLst() const { return m_searchLst; }
//	const std::unordered_map<NS::I64, MsgPB::BaseData>& FriendsLst() const { return m_friendsLst; }
//	const std::unordered_map<NS::I64, MsgPB::BaseData>& ApplyLst() const { return m_applyLst; }
//	const std::unordered_map<NS::I64, MsgPB::BaseData>& CacheLst() const { return m_cacheLst; }
//	std::map<NS::I64, MsgPB::BaseData>& StrangerLst() { return m_strangerLst; }
//	std::map<NS::I64/*BlackIggid*/, MsgPB::MsgBlackUser> getBlackList() const { return m_blackLst; };
//
//	std::vector<LuaTable> METHOD(lua_getFriendList)(int tpy);
//
//	NS::int32 METHOD(FriendLimitNum)() const { return m_friendLimitNum; }
//	NS::int32 METHOD(FriendNum)() const { return m_friendsLst.size(); }
//	NS::I64  m_lastChatIggId{ 0 };
//	bzbee::tstring m_lastChatName{};
//	bool METHOD(isFriendGift)(NS::I64 iggid) { return m_giftLst.find(iggid) != m_giftLst.end(); }
//	const MsgPB::MsgFriendGiftData* getFriendGift(NS::I64 iggid);
//	LuaTable METHOD(lua_getFriendGift)(NS::I64 iggid);
//
//	void addCacheData(const MsgPB::BaseData& data);
//
//	NS::I64 getSendChatTm(NS::I64 iggId);
//
//	void METHOD(lua_regChatEvent)(const LuaCallback& invoker);
//	void METHOD(lua_unregChatEvent)();
//	void METHOD(lua_regFriendEvent)(const LuaCallback& invoker);
//	void METHOD(lua_unregFriendEvent)();
//
//	std::vector<NS::I64> METHOD(lua_getChatIggIds)();
//	void METHOD(lua_clearChatIggId)(NS::I64 iggId);
//private:
//	std::unordered_map<NS::I64, MsgPB::BaseData>  m_searchLst{};
//	std::unordered_map<NS::I64, MsgPB::BaseData>  m_friendsLst{};
//	std::unordered_map<NS::I64, MsgPB::BaseData>  m_applyLst{};
//	std::unordered_map<NS::I64, MsgPB::BaseData>  m_cacheLst{};
//	std::unordered_map<NS::I64, MsgPB::MsgFriendGiftData>  m_giftLst{};
//	std::map<NS::I64, MsgPB::BaseData>  m_strangerLst{};
//	std::map<NS::I64/*BlackIggid*/ , MsgPB::MsgBlackUser>  m_blackLst{};
//
//	NS::int32 m_friendLimitNum{ 0 };
//
//	NS::int32 m_iGiftSentCnt{ 0 };
//	NS::int32 m_iGiftRecvCnt{ 0 };
//
//	NS::int32 m_iCfgSentCnt{ 0 };
//	NS::int32 m_iCfgRecvCnt{ 0 };
//
//	NS::uint m_firRedState{ 0 };
//
//	std::map<NS::I64, NS::I64> m_mapPlayerChats;  //私聊记录下时间顺序
//	std::map<NS::I64, bool> m_chatIggs;
};

class HandlerFriendEvent : public bzbee::Event
{
public:
	enum class EventType
	{
		Unknown,
		refreshSearchList,
		refreshFriendList,
		refreshApplyList,
		refreshBlackList,
		friendApplyConfimResult,
		getGiftResult,
		refreshGiftList,
		sendGiftResult,
	};

public:
	HandlerFriendEvent() {};
	virtual ~HandlerFriendEvent(){};

public:
	EventType m_type{ EventType::Unknown };
	const google::protobuf::Message *  pMsg{nullptr};
	//const std::unordered_map<NS::I64, MsgPB::BaseData> *  pBaseDataLst;
	NS::I64 m_friIggid{ 0 };
};

class HandlerStrangerChatEvent : public bzbee::Event
{
public:
	enum class EventType
	{
		Unknown,
		NewStrangerChat,
	};

public:
	HandlerStrangerChatEvent() {};
	virtual ~HandlerStrangerChatEvent() {};

public:
	EventType m_type{ EventType::Unknown };
	//std::shared_ptr<ChatData::MsgData> m_pMsgData{ nullptr };

};
