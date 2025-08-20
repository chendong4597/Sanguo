
/*!
 * \file DispatchMsg.h
 *
 * \author laizhisheng
 * \date 2016/12/26 10:25
 *
 */

#pragma once

#include <BaseType.h>
#include <google/protobuf/message.h>
#include <mutex>
#include <LogUtil.h>
#include "MsgHandler.h"
#include <list>

class DispatchMsg : public bzbee::EventHandler
{
public:
	enum class MsgFlag 
	{
		None = 0,
		C2L,
		C2L2S,

		S2C,
		S2L,
		S2L2C,
		C2C_JAVA_CPP
	};

	class SubscribeInfo
	{
	public:
		bzbee::EventHandler* m_pHandler{};
		bzbee::BaseMsgCallback* m_pCallback{};
		MsgFlag m_msgFlag{ MsgFlag::None };
	};
	typedef std::multimap<bzbee::uint16/*nMsgId*/, SubscribeInfo> SubscribeInfoMap;

public:
	DispatchMsg();
	~DispatchMsg();

public:
	template<typename T> bool registerMessage(bzbee::uint16 nMsgId, bzbee::EventHandler* pHandler, const std::function<void(const T& msg)>& callback, DispatchMsg::MsgFlag flag)
	{
		// check existing
		SubscribeInfoMap::iterator it = findMessageInfo(nMsgId, pHandler);
		if (it != m_SubscribeMsgMap.end())
		{
			LOGE("duplicate subscribe message {0}", nMsgId);
			return false;
		}

		SubscribeInfo subscribeInfo;
		subscribeInfo.m_pHandler = pHandler;
		subscribeInfo.m_pCallback = new bzbee::MsgCallback<T>(callback);
		subscribeInfo.m_msgFlag = flag;
		m_SubscribeMsgMap.insert(std::make_pair(nMsgId, subscribeInfo));

		return true;
	}

	bool unregisterMessage(bzbee::uint16 nMsgId, bzbee::EventHandler* pHandler);
	bool dispatchOneMessage(bzbee::uint16 msgId, const bzbee::Msg* pProtoMsg);

private:
	SubscribeInfoMap::iterator findMessageInfo(bzbee::uint16 nMsgId, bzbee::EventHandler* pHandler);

private:
	SubscribeInfoMap m_SubscribeMsgMap;
};


typedef google::protobuf::Message IMsg;
class ClientMsgJavaToCpp
{
	ClientMsgJavaToCpp() {};

public:
	enum CONST_DEFINE
	{
		DEFAULT_BUFFER = 1024,
		INVALID_MSG_ID = 0,
		MAX_SENDING_LENGTH = 2048,
		MSG_HEAD_LENGTH = (sizeof(bzbee::uint16) + sizeof(bzbee::uint16)),
		SOCKET_MAX_RETRY = 5,
	};

	typedef struct MESSAGE_INFO_tag
	{
		bzbee::uint16 nMsgId;
		IMsg* pProtoMsg;
		bzbee::tstring strMd5;
	} MESSAGE_INFO;

//public:
	//static ClientMsgJavaToCpp* self;
	static ClientMsgJavaToCpp& getInstance();

	template<typename T>
	bool registerMessage(bzbee::uint16 nMsgId, bzbee::EventHandler* pHandler, const std::function<void(const T& msg)>& callback)
	{
		std::lock_guard<std::mutex> guard{ m_mutexLocalQueue };

		return m_dispatchMessage.registerMessage(nMsgId, pHandler, callback, DispatchMsg::MsgFlag::C2C_JAVA_CPP);
	}

	void addDispatchMessage(IMsg* pProtoMsg, bzbee::uint16 nMsgId, const bzbee::tstring& strMd5);
	void dispatchMessageQueue();

	bool unregisterMessage(bzbee::uint16 nMsgId, bzbee::EventHandler* pHandler);
	bool dispatchOneMessage(bzbee::uint16 msgId, const bzbee::Msg* pProtoMsg);

private:
	std::mutex m_mutexLocalQueue;

	DispatchMsg m_dispatchMessage;
	std::list<MESSAGE_INFO> m_localMsgQueue;
};

//client
#define REGISTER_MSG_JAVA2CPP(returnMsgId, msgCls, callback, ...) \
bzbee::ProtobufMsgMgr::getInstance().registerMsgCreator(returnMsgId, std::bind([]()->bzbee::Msg* { return new msgCls(); })); \
ClientMsgJavaToCpp::getInstance().registerMessage<msgCls>(returnMsgId, this, std::bind(callback, this, std::placeholders::_1, ##__VA_ARGS__));

#define UNREGISTER_MSG_JAVA2CPP(returnMsgId, msgCls, callback, ...) \
bzbee::ProtobufMsgMgr::getInstance().unregisterMsgCreator(returnMsgId); \
ClientMsgJavaToCpp::getInstance().unregisterMessage(returnMsgId, this);

