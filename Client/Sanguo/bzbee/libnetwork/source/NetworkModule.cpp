/*!
 * \file NetworkModule.cpp
 *
 * \author laizhisheng
 * \date 2016/12/26 10:37
 *
 */
#include "NetworkModule.h"
#include "DispatchMsg.h"
#include <LogUtil.h>


NS_BEGIN


NetworkModule* NetworkModule::s_pNetworkModule = nullptr;
NetworkModule::NetworkModule(MsgDecoder* pDecoder)
	: m_pMsgDecoder(pDecoder)
{}


NetworkModule::~NetworkModule() {}


NetworkModule& NetworkModule::getInstance()
{
	if (!s_pNetworkModule) initializeInstance(nullptr);
	return *s_pNetworkModule;
}


void NetworkModule::update(float dt)
{
	if (m_waiting)
	{
		std::lock_guard<std::mutex> guard(m_mutex);
		m_health += dt;
	}

	m_mutex.lock();
	while (!m_MsgQueue.empty())
	{
		auto& msgInfo = m_MsgQueue.front();
		getInstance().dispatchOneMessage(msgInfo.m_msgId, msgInfo.m_pProtoMsg);
		SAFE_DELETE(msgInfo.m_pProtoMsg);
		m_MsgQueue.pop();
	}
	m_mutex.unlock();

	if (!m_pMsgDecoder) return;
	tstring strMd5;
	uint16 nMsgId = 0;
	Msg* pMsg{};

	m_mutex.lock();
	uint16 msg_type = 0;
	while(m_decoder(msg_type, &pMsg, m_RecvBuffer))
	{
		if (!pMsg) continue;
		getInstance().dispatchOneMessage(msg_type, pMsg);
		delete pMsg;
	}

	m_mutex.unlock();

	// LocalMsg
	ClientMsgJavaToCpp::getInstance().dispatchMessageQueue();
}


void NetworkModule::terminate()
{
	disconnect();

	// clear all message
	while (true)
	{
		MessageInfo msgInfo;
		m_mutex.lock();
		if (!m_MsgQueue.empty())
		{
			msgInfo = m_MsgQueue.front();
			m_MsgQueue.pop();
		}
		m_mutex.unlock();

		if (!msgInfo.m_pProtoMsg) break;
		SAFE_DELETE(msgInfo.m_pProtoMsg);
	}

	m_nConnectionState = 0;
	m_nLastError = 0;
	m_RecvBuffer.reset();

	SAFE_DELETE(m_pMsgDecoder);
	SAFE_DELETE(s_pNetworkModule);
}


void NetworkModule::disconnect()
{
	setConnectionState(CONNECTION_STATE_DISCONNECT_BY_HOST, true);
	m_isConnected = false;
	m_connectState = ConnectState::Disconnect;

	doDisconnect();

	m_RecvBuffer.reset();

	setConnectionState(CONNECTION_STATE_CONNECTED | CONNECTION_STATE_CONNECTING, false);
}


bool NetworkModule::setConnectionState(uint state, bool set)
{
	uint oldState = m_nConnectionState;
	if (set) m_nConnectionState |= state;
	else m_nConnectionState &= (~state);
	return (oldState != m_nConnectionState);
}


bool NetworkModule::decodeMsg(const void* pData, int nLength)
{
	// 收到任何消息都可以认为当前网络状态良好。
	{
		std::lock_guard<std::mutex> guard(m_mutex);
		m_health = 0.0f;
	}

	if (pData && nLength > 0)
	{
		m_mutex.lock();
		m_RecvBuffer.write(pData, nLength);
		m_mutex.unlock();
	}
	return true;
}


void NetworkModule::addDispatchMessage(Msg* pProtoMsg, uint16 nMsgId, const tstring& strMd5)
{
	if (!pProtoMsg) return;

	m_mutex.lock();
	MessageInfo msgInfo;
	msgInfo.m_msgId = nMsgId;
	msgInfo.m_pProtoMsg = pProtoMsg;
	msgInfo.m_strMd5 = strMd5;

	m_MsgQueue.push(msgInfo);
	m_mutex.unlock();
}


NS_END