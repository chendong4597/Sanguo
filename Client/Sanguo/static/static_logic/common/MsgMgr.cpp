/*!
 * \file MsgMgr.cpp
 *
 * \author laizhisheng
 * \date 2016/12/26 14:47
 *
 */
#include "MsgMgr.h"
#include "../common/MessageQueue.h"
#include <LogUtil.h>
#include <NetworkModule.h>
//#include <handler/HandlerMgr.h>
#include "../handler/HandlerLogin.h"
#include "../localserver/LocalServer.h"
#include <msg/MsgPing.pb.h>
#include <deque>
#include "stdafx.h"

namespace
{
	const int MSG_HEADER_SIZE{ sizeof(NS::uint16) + sizeof(NS::uint16) };
}

using namespace MsgPB;
NS_BEGIN


MessageQueue* g_pMessageQueue{};


MsgMgr& MsgMgr::getInstance(void)
{
	static MsgMgr mgr;
	return mgr;
}


void MsgMgr::initialize(void)
{
	google::protobuf::MessageFactory::generated_factory()->GetMap(m_name_to_message);

	for (auto& v : m_name_to_message)
	{
		auto pFieldDescriptor = v.second->GetDescriptor()->FindFieldByName("msgType");
		if (!pFieldDescriptor || pFieldDescriptor->is_repeated() || pFieldDescriptor->type() != google::protobuf::FieldDescriptor::TYPE_ENUM) continue;

		auto message_id = static_cast<uint16>(v.second->GetReflection()->GetEnum(*v.second, pFieldDescriptor)->number());
		m_name_to_id.emplace(v.first, message_id);
		m_id_to_message.emplace(message_id, v.second);
	}

	NetworkModule::getInstance().setMsgCallback(std::bind(&MsgMgr::dispatchOneMessage, this, std::placeholders::_1, std::placeholders::_2, true));
	NetworkModule::getInstance().setDecoder(std::bind(&MsgMgr::decode, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	m_tickPing = 8.f;

	REGISTER_EVENT(&NetworkModule::getInstance(), NetworkModuleEvent, &MsgMgr::onDispatchOneLocalMsg);
}




void MsgMgr::terminate()
{
	NetworkModule::getInstance().terminate();

    for (auto it : m_SubscribeMsgMap)
    {
        SAFE_DELETE(it.second.pCallback);
    }
    m_SubscribeMsgMap.clear();

    for (auto it : m_hookMap)
    {
        SAFE_DELETE(it.second.pCallback);
    }
    m_hookMap.clear();
}


void MsgMgr::update(float dt)
{
	NetworkModule::getInstance().update(dt);
	//if (g_pMessageQueue) g_pMessageQueue->update(dt);
	if ((m_tickPing += dt) > 8 && 
		NetworkModule::getInstance().getConnectState() == NetworkModule::ConnectState::Connected)
	{
		m_tickPing = 0.0f;
		MsgPing msg;
		sendMessageDirect(EMsgType::_MSG_PING, &msg);
	}
}


bool MsgMgr::sendMessage(uint16 msgId, const Msg* pMsg)
{	
	LOGI("MsgMgr::sendMessage msg type = {0}", msgId);
	if (m_clientMode && sendMessageLocal(msgId, pMsg)) {
		return true;
	}

	sendMessageDirect(msgId, pMsg);
	//if (g_pMessageQueue) g_pMessageQueue->send(static_cast<EMsgType>(msgId), *pMsg);
	return true;
}

bool MsgMgr::sendMessageLocal(uint16 msgId, const Msg* pMsg)
{
	auto range = m_hookMap.equal_range(msgId);
	if (range.first != range.second) {
		// copy subscribe info into vector, because the invoker may erase the subscribtion from the map
		std::vector<HookInfo> vec;
		for (auto it = range.first; it != range.second; ++it)
			vec.push_back(it->second);

		// invoke the subscribe function
		for (const auto& v : vec)
			v.pCallback->invoke(*pMsg);

		return true;
	}
	return false;
}


void MsgMgr::send_c2s(uint16 msgId, const Msg* pMsg)
{
	if (!m_clientMode) return;
	if (g_pMessageQueue) g_pMessageQueue->send(static_cast<EMsgType>(msgId), *pMsg);
}


bool MsgMgr::sendMessageDirect(uint16 msgId, const Msg* pMsg)
{
	if (msgId == EMsgType::_MSG_PING)
	{
		const_cast<MsgPing&>(dynamic_cast<const MsgPing&>(*pMsg)).set_clienttimeinms(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
	}

	if (m_debugLogCallback) m_debugLogCallback(msgId, pMsg);

	static char szBuf[1024 * 64] = { 0 };
	int nProtoMsgSize = pMsg->ByteSize();

	assert(nProtoMsgSize < sizeof(szBuf));
	if (nProtoMsgSize > sizeof(szBuf))
	{
		LOGE("{0}::Too much bytes", pMsg->GetTypeName());
		return false;
	}

	memset(szBuf, 0, sizeof(szBuf));
	if (!pMsg->SerializeToArray(szBuf, nProtoMsgSize))
	{
		LOGE("{0}::SerializeToArray failed", pMsg->GetTypeName());
		return false;
	}

	//encrypt(reinterpret_cast<unsigned char*>(szBuf), nProtoMsgSize);

	BufferStreamWriter wirtebuffer;

	// Split message
	const int MAX_MSG_SIZE_WITHOUT_HEAD{ 2044 };
	if (nProtoMsgSize > MAX_MSG_SIZE_WITHOUT_HEAD)
	{
		int idx{};
		const char* srcBuffer{ szBuf };
		while (nProtoMsgSize > 0)
		{
			int sizeWritten{ std::min(nProtoMsgSize, idx == 0 ? MAX_MSG_SIZE_WITHOUT_HEAD - MSG_HEADER_SIZE : MAX_MSG_SIZE_WITHOUT_HEAD) };
			uint16 nDataSize = sizeWritten + MSG_HEADER_SIZE + (idx == 0 ? MSG_HEADER_SIZE : 0);
			wirtebuffer.write(&nDataSize, sizeof(nDataSize));
			uint16 msgType{ static_cast<uint16>(idx == 0 ? _MSG_PARTIAL_HEAD : _MSG_PARTIAL_BODY) };
			wirtebuffer.write(&msgType, sizeof(msgType));
			if (idx == 0)
			{
				nDataSize = nProtoMsgSize + MSG_HEADER_SIZE;
				wirtebuffer.write(&nDataSize, sizeof(nDataSize));
				wirtebuffer.write(&msgId, sizeof(msgId));
			}
			wirtebuffer.write(srcBuffer, sizeWritten);

			nProtoMsgSize -= sizeWritten;
			srcBuffer += sizeWritten;
			++idx;
		}
	}
	else
	{
		uint16 nDataSize = nProtoMsgSize + MSG_HEADER_SIZE;
		wirtebuffer.write(&nDataSize, sizeof(nDataSize));
		wirtebuffer.write(&msgId, sizeof(msgId));
		wirtebuffer.write(szBuf, nProtoMsgSize);
	}

	auto& net = NetworkModule::getInstance();
	if (!net.doFlushSentBuffer(wirtebuffer))
	{
		LOGE("Send message {0} failed, error: [{1}] {2}", pMsg->GetTypeName(), net.getLastErrorNo(), net.getLastErrorString());
		return false;
	}

	if (msgId == _MSG_PING)
	{
		LOGI("Ping");
	}
	else {
		LOGI("Sent message: {0} {1}", pMsg->GetTypeName(), pMsg->DebugString());
	}
 		

	return true;
}




Msg* MsgMgr::createMessage(const tstring& name) const
{
	auto it = m_name_to_message.find(name);
	return it == m_name_to_message.end() ? nullptr : it->second->New();
}


uint16 MsgMgr::findIdByName(const tstring& name) const
{
	auto it = m_name_to_id.find(name);
	return it == m_name_to_id.end() ? 0u : it->second;
}


bool MsgMgr::decode(uint16& msg_type, Msg** pp_message, BufferStreamWriter& buffer)
{
	msg_type = 0;
	Msg* pMsg = *pp_message = nullptr;
	BufferStreamReader bufferReader(buffer.getBuffData());

	// 所有消息的起始数据为 msgSize(uint16) + msgId(uint16)。
	// 读取消息长度。
	uint16 nMsgSize = 0;
	if (!bufferReader.read(&nMsgSize, sizeof(nMsgSize))) return false;
	if (bufferReader.getSize() + sizeof(nMsgSize) < nMsgSize) return false;

	try
	{
		if (!bufferReader.read(&msg_type, sizeof(msg_type)))
			throw std::runtime_error("Invalid message.");	// read message type.

		// Combine big msg
		switch (msg_type)
		{
		case _MSG_PARTIAL_HEAD:
		{
			m_bigMsgRecvBuffer.reset();
			m_bigMsgRecvBuffer.write(bufferReader.getData(), nMsgSize - MSG_HEADER_SIZE);
			throw std::runtime_error("Big message head.");
		}
		break;

		case _MSG_PARTIAL_BODY:
		{
			m_bigMsgRecvBuffer.write(bufferReader.getData(), nMsgSize - MSG_HEADER_SIZE);
			if (*reinterpret_cast<const uint16*>(m_bigMsgRecvBuffer.getData()) == m_bigMsgRecvBuffer.getSize())
			{
				if (decode(msg_type, pp_message, m_bigMsgRecvBuffer))
				{
					pMsg = *pp_message;
					throw std::runtime_error("Big message end.");
				}
			}
			throw std::runtime_error("Big message body.");
		}
		break;

		default:
			break;
		}

		auto it = m_id_to_message.find(msg_type);
		if (it == m_id_to_message.end())
		{
			LOGE("Unknown message: Id={0}, size={1}.", msg_type, nMsgSize);
			throw std::runtime_error("Unknown message.");
		}

		if (msg_type == 1005) {
			LOGI("Pong");
		}
		else {
			LOGI("Receive: [{0}] {1}, size={2}.", msg_type, it->second->GetTypeName(), nMsgSize);
		}
 			

		pMsg = it->second->New();
		if (!pMsg->ParseFromArray(bufferReader.getData(), nMsgSize - MSG_HEADER_SIZE))
		{
			SAFE_DELETE(pMsg);
			throw std::runtime_error("Parse message failed.");
		}

		*pp_message = pMsg;
	}
	catch (const std::exception& ex)
	{
		LOGE(ex.what());
	}

	// 指向下一条消息。
	bufferReader.reset();
	bufferReader.skip(nMsgSize);

	// adjust output buffer
	int remainSize = static_cast<int>(bufferReader.getSize());
	if (remainSize > 0)
	{
		// copy remain buffer into temp buffer
		if ((int)m_bufferTemp.size() < remainSize) m_bufferTemp.resize(remainSize);
		std::copy(bufferReader.getData(), bufferReader.getData() + remainSize, m_bufferTemp.begin());

		// copy temp buffer into buffer
		buffer.reset();
		buffer.write(&m_bufferTemp[0], remainSize);
	}
	else
	{
		buffer.reset();
	}

	return (pMsg || buffer.getSize() > 0);
}


// 所有注册方法最终都会调用此函数。
void MsgMgr::reg(EventHandler* pHandler, uint16 msgId, BaseMsgCallback* pCallback)
{
	// check existing
	if (findMessageInfo(msgId, pHandler) != m_SubscribeMsgMap.end())
	{
		LOGE("duplicate subscribe message {0}", msgId);
		return;
	}

	SubscribeInfo subscribeInfo;
	subscribeInfo.pHandler = pHandler;
	subscribeInfo.pCallback = pCallback;
	m_SubscribeMsgMap.insert(std::make_pair(msgId, subscribeInfo));

	// 当 Component 被销毁时，反注册所有相关的网络消息。
	if (auto pComponent = dynamic_cast<Component*>(pHandler))
	{
		REG_EVENT(pComponent)[this, pHandler](ComponentDestroyEvent&)
		{
			for (auto it = m_SubscribeMsgMap.begin(); it != m_SubscribeMsgMap.end();)
			{
				if (it->second.pHandler != pHandler)
					++it;
				else
				{
					SAFE_DELETE(it->second.pCallback);
					it = m_SubscribeMsgMap.erase(it);
				}
			}
		};
	}
}


void MsgMgr::reg(EventHandler* pHandler, const tstring& msgName, BaseMsgCallback* pCallback)
{
	auto it = m_name_to_id.find(msgName);
	if (it == m_name_to_id.end())
	{
		LOGE("Invalid message name: {0}", msgName);
		return;
	}
	reg(pHandler, it->second, pCallback);
}


// 所有反注册方法最终都会调用此函数。
void MsgMgr::unreg(EventHandler* pHandler, uint16 msgId)
{
	SubscribeInfoMap::iterator it = findMessageInfo(msgId, pHandler);
	if (it != m_SubscribeMsgMap.end())
	{
		SAFE_DELETE(it->second.pCallback);
		m_SubscribeMsgMap.erase(it);
	}
}


void MsgMgr::unreg(EventHandler* pHandler, const tstring& msgName)
{
	auto it = m_name_to_id.find(msgName);
	if (it == m_name_to_id.end())
	{
		LOGE("Invalid message name: {0}", msgName);
		return;
	}
	unreg(pHandler, it->second);
}


void MsgMgr::dispatchOneMessage(uint16 msgId, const Msg* pMsg, bool hookable)
{
	if (pMsg)
	{
		auto&& msgTypeName = pMsg->GetTypeName();
		if (msgTypeName != "MsgPB.MsgBuildActiveAppearance" &&
			msgTypeName != "MsgPB.MsgBuildPlaceMult")
		{
			LOGI("MsgMgr::dispatchOneMessage msg type = {0}", msgTypeName);
		}
	}

	if (g_pMessageQueue) g_pMessageQueue->onReceive(static_cast<EMsgType>(msgId), *pMsg);
	if (m_debugLogCallback) m_debugLogCallback(msgId, pMsg);

	if (m_serverMode && sendMessageLocal(msgId, pMsg)) {
		return;
	}

	// 弱联网模式下，也允许拦截服务器下发的消息。
	if (hookable && m_clientMode)
	{
		auto range = m_hookMap.equal_range(msgId);
		if (range.first != range.second)
		{
			// copy subscribe info into vector, because the invoker may erase the subscribtion from the map
			std::vector<HookInfo> vec;
			for (auto it = range.first; it != range.second; ++it)
				vec.push_back(it->second);

			// invoke the subscribe function
			for (const auto& v : vec)
				v.pCallback->invoke(*pMsg);

			return;
		}
	}

	// copy subscribe info into vector, because the invoker may erase the subscribtion from the map
	std::vector<SubscribeInfo> vSubscribeInfo;
	std::pair<SubscribeInfoMap::iterator, SubscribeInfoMap::iterator> range = m_SubscribeMsgMap.equal_range(msgId);
	for (SubscribeInfoMap::iterator it = range.first; it != range.second; ++it)
	{
		SubscribeInfo& subscribeInfo = it->second;
		vSubscribeInfo.push_back(subscribeInfo);
	}
	if (vSubscribeInfo.empty()) return;

	// invoke the subscribe function
	for (auto it = vSubscribeInfo.begin(); it != vSubscribeInfo.end(); ++it)
		it->pCallback->invoke(*pMsg);
}



MsgMgr::SubscribeInfoMap::iterator MsgMgr::findMessageInfo(uint16 msgId, EventHandler* pHandler)
{
	auto range = m_SubscribeMsgMap.equal_range(msgId);
	for (auto it = range.first; it != range.second; ++it)
		if (it->second.pHandler == pHandler) return it;
	return m_SubscribeMsgMap.end();
}

void MsgMgr::addLocalServer(LocalServer* pHandler)
{
	m_hookSet.push_back(pHandler);
	// 手动调用初始化函数。
	pHandler->initialize(nullptr);

	// 当 LocalServer 被销毁时，反注册所有相关的网络消息。
	REG_EVENT(pHandler)[this, pHandler](ComponentDestroyEvent&)
	{
		for (auto it = m_hookMap.begin(); it != m_hookMap.end();)
		{
			if (it->second.pHandler != pHandler)
				++it;
			else
			{
				SAFE_DELETE(it->second.pCallback);
				it = m_hookMap.erase(it);
			}
		}
	};
}

void MsgMgr::hook(LocalServer* pHandler, uint16 msgId, BaseMsgCallback* pCallback)
{
	// 检查重复注册。
	auto range = m_hookMap.equal_range(msgId);
	for (auto it = range.first; it != range.second; ++it)
	{
		if (it->second.pHandler == pHandler)
		{
			LOGE("duplicate subscribe message {0}", msgId);
			return;
		}
	}

	HookInfo hookInfo;
	hookInfo.pHandler = pHandler;
	hookInfo.pCallback = pCallback;
	m_hookMap.insert(std::make_pair(msgId, hookInfo));
}


void MsgMgr::unhook(LocalServer* pHandler, uint16 msgId)
{
	auto range = m_hookMap.equal_range(msgId);
	for (auto it = range.first; it != range.second;)
	{
		if (it->second.pHandler != pHandler)
			++it;
		else
		{
			delete it->second.pCallback;
			it = m_hookMap.erase(it);
		}
	}
}

void MsgMgr::send_s2c(uint16 msgId, const Msg* pMsg)
{
	dispatchOneMessage(msgId, pMsg, false);
	//if (m_serverMode) {
	//	sendMessageDirect(msgId, pMsg);
	//}
}


void MsgMgr::sendLocalMessage(uint16 msgId, Msg* pMsg)
{
	NetworkModule::getInstance().sendLocalMessage(pMsg, msgId);
}

void MsgMgr::onDispatchOneLocalMsg(const NS::NetworkModuleEvent& evt)
{
	// LOGE("MsgMgr::onDispatchOneLocalMsg, recieve msgid = {0}", evt.m_msgId);
	// 关闭loading层
	//HandlerMgr::getInstance().closeClientTransfer(evt.m_msgId);
}
NS_END
