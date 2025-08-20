/*!
 * \file DispatchMsg.cpp
 *
 * \author laizhisheng
 * \date 2016/12/26 10:25
 *
 */

#include "DispatchMsg.h"
#include <StringUtil.h>
#include "NetworkModule.h"


DispatchMsg::DispatchMsg()
{

}

DispatchMsg::~DispatchMsg()
{
	for (auto pair : m_SubscribeMsgMap)
	{
		bzbee::ProtobufMsgMgr::getInstance().unregisterMsgCreator(pair.first); 
		SAFE_DELETE(pair.second.m_pCallback);
	}

	m_SubscribeMsgMap.clear();
}

bool DispatchMsg::unregisterMessage(bzbee::uint16 nMsgId, bzbee::EventHandler* pHandler)
{
	SubscribeInfoMap::iterator iter = findMessageInfo(nMsgId, pHandler);
	if (iter == m_SubscribeMsgMap.end()) return false;

	SAFE_DELETE(iter->second.m_pCallback);
	m_SubscribeMsgMap.erase(iter);
	return true;
}

DispatchMsg::SubscribeInfoMap::iterator DispatchMsg::findMessageInfo(bzbee::uint16 nMsgId, bzbee::EventHandler* pHandler)
{
	std::pair<SubscribeInfoMap::iterator, SubscribeInfoMap::iterator> range = m_SubscribeMsgMap.equal_range(nMsgId);
	for (SubscribeInfoMap::iterator it = range.first; it != range.second; ++it)
	{
		SubscribeInfo& subscribeInfo = it->second;
		if (subscribeInfo.m_pHandler == pHandler) return it;
	}

	return m_SubscribeMsgMap.end();
}

bool DispatchMsg::dispatchOneMessage(bzbee::uint16 msgId, const bzbee::Msg* pProtoMsg)
{
	// copy subscribe info into vector, because the invoker may erase the subscribtion from the map
	std::vector<SubscribeInfo> vSubscribeInfo;
	std::pair<SubscribeInfoMap::iterator, SubscribeInfoMap::iterator> range = m_SubscribeMsgMap.equal_range(msgId);
	for (SubscribeInfoMap::iterator it = range.first; it != range.second; ++it)
	{
		SubscribeInfo& subscribeInfo = it->second;
		vSubscribeInfo.push_back(subscribeInfo);
	}
	if (vSubscribeInfo.empty()) return false;

	bzbee::NetworkModuleEvent evt{};
	evt.m_msgId = msgId;
	bzbee::NetworkModule::getInstance().postEvent(evt);

	bool bDone = true;
	// invoke the subscribe function
	for (auto it = vSubscribeInfo.begin(); it != vSubscribeInfo.end(); ++it)
	{
		const auto& subscribeInfo = (*it);
		subscribeInfo.m_pCallback->invoke(*pProtoMsg);

		if (subscribeInfo.m_msgFlag == MsgFlag::C2L2S || subscribeInfo.m_msgFlag == MsgFlag::S2L2C)
		{
			bDone = false;
		}
	}

	return bDone;
}

//ClientMsgJavaToCpp* ClientMsgJavaToCpp::self = nullptr;
ClientMsgJavaToCpp& ClientMsgJavaToCpp::getInstance(){
	//if (nullptr == self)
	//{
	//	self = new ClientMsgJavaToCpp();
	//}
	//return *self;

	static ClientMsgJavaToCpp self;
	return self;
};

void ClientMsgJavaToCpp::addDispatchMessage(IMsg* pProtoMsg, bzbee::uint16 nMsgId, const bzbee::tstring& strMd5)
{
	if (!pProtoMsg) return;

	std::lock_guard<std::mutex> guard{ m_mutexLocalQueue };

	MESSAGE_INFO msgInfo = { nMsgId, pProtoMsg, strMd5 };
	m_localMsgQueue.emplace_back(msgInfo);

}

bool ClientMsgJavaToCpp::unregisterMessage(bzbee::uint16 nMsgId, bzbee::EventHandler* pHandler)
{
	std::lock_guard<std::mutex> guard{ m_mutexLocalQueue };

	return m_dispatchMessage.unregisterMessage(nMsgId, pHandler);
}


bool ClientMsgJavaToCpp::dispatchOneMessage(bzbee::uint16 msgId, const bzbee::Msg* pProtoMsg)
{
	return m_dispatchMessage.dispatchOneMessage(msgId, pProtoMsg);
}

void ClientMsgJavaToCpp::dispatchMessageQueue()
{
	std::lock_guard<std::mutex> guard{ m_mutexLocalQueue };

	// dispatch message
	while (!m_localMsgQueue.empty())
	{
		MESSAGE_INFO& msgInfo = m_localMsgQueue.front();
		if (!msgInfo.pProtoMsg) break;

		dispatchOneMessage(msgInfo.nMsgId, msgInfo.pProtoMsg);

		SAFE_DELETE(msgInfo.pProtoMsg);
		m_localMsgQueue.pop_front();
	}
}

