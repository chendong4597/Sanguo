/*!
 * \file NetworkModule_Impl_Android.cpp
 * \date 12-8-2012 16:46:04
 *
 *
 * \author zjhlogo (zjhlogo@gmail.com)
 */
#include "NetworkModule_Impl_Android.h"
#include "./platform/android/JniHelper.h"
#include <baseutil.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <thread>
#include <chrono>
#include "../../MsgNetWork.pb.h"
#include "../../NetworkModule.h"
#include "../../../../libbaseutil/source/utils/BufferStream.h"
#include "../../DispatchMsg.h"

NS_BEGIN

//static bool g_bDisconnectByHost = false;
static bool g_bThreadRunning = false;

bool NetworkModule::initializeInstance(NetworkModule* pNetworkModule)
{
	if (s_pNetworkModule) return false;

	if (pNetworkModule)
	{
		s_pNetworkModule = pNetworkModule;
	}
	else
	{
		s_pNetworkModule = new NetworkModule_Impl_Android(new MsgDecoder());
	}

	return (s_pNetworkModule != nullptr);
}

NetworkModule_Impl_Android::NetworkModule_Impl_Android(MsgDecoder* pDecoder)
	: NetworkModule(pDecoder)
{}

bool NetworkModule_Impl_Android::connectToServer(const char* ip, uint16 port)
{
	m_isV4Ip = true;

	// 检测字符串是不是IPV4的格式
	for (const char *pc = ip; *pc != 0; pc++)
	{
		if ((*pc < '0' || *pc > '9') && *pc != '.')
		{
			m_isV4Ip = false;
			break;
		}
	}
	if (m_isV4Ip)
	{
		uint nIP = 0;
		inet_pton(AF_INET, ip, &nIP);
		return connectToServer(nIP, port);
	}
	else
	{
		m_strServerAddress = ip;
		return connectToServer(uint(0), port);
	}
}

bool NetworkModule_Impl_Android::connectToServer(uint ip, uint16 port)
{
	++m_serial;

	// disconnect from server
	disconnect();
	m_connectState = ConnectState::Connecting;

	if (m_isV4Ip)
	{
		m_nRequestIp = ip;
		m_nRequestPort = port;

		// create socket
		m_socket = socket(AF_INET, SOCK_STREAM, 0);
	}
	else
	{
		struct addrinfo hints;
		memset(&hints, 0, sizeof(hints));
		hints.ai_flags = 0;
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		m_aip = NULL;
		char szPort[10];
		sprintf(szPort, "%d", port);

		int error = getaddrinfo(m_strServerAddress.c_str(), szPort, &hints, &m_aip);
		if (error != 0)
		{
			if (m_aip)
			{
				freeaddrinfo(m_aip);
				m_aip = {};
			}
			return false;
		}
		// create socket
		m_socket = socket(m_aip->ai_family, m_aip->ai_socktype, m_aip->ai_protocol);
	}
	if (m_socket < 0)
	{
		LOGE("create socket failed, socket: {0} ip = {1} port = {2}", m_socket, ip, port);
		m_socket = 0;
		return false;
	}

	setConnectionState(CONNECTION_STATE_CONNECTING, true);

	// create thread to connect to server
	std::thread t{ &NetworkModule_Impl_Android::netThread, this };
	if (!t.joinable())
	{
		setConnectionState(CONNECTION_STATE_CONNECTING, false);
		LOGE("create network thread failed");
		return false;
	}
	t.detach();

	return true;
}

void NetworkModule_Impl_Android::doDisconnect()
{
	g_bThreadRunning = false;
	m_isConnected = false;

	if (m_socket)
	{
		close(m_socket);
		m_socket = 0;
		if (m_aip)
		{
			freeaddrinfo(m_aip);
			m_aip = {};
		}

		LOGD("disconnect from server");
	}
}

bool NetworkModule_Impl_Android::doFlushSentBuffer(BufferStreamWriter& buffer)
{
	if (buffer.getSize() <= 0) return true;

	bool bSendSuccess = false;

	BufferStreamReader reader(buffer.getBuffData());
	while (true)
	{
		if (checkConnectionState(CONNECTION_STATE_SIMULATE_SEND_ERROR))
		{
			reader.skip(reader.getSize());
			setLastErrorNo(-999999);
			setLastErrorString("client simulate error send message");
			break;
		}

		int result = send(m_socket, (const char*)reader.getData(), reader.getSize(), 0);

		if (result <= 0)
		{
			setLastErrorNo(result);
			break;
		}

		reader.skip(result);
		if (reader.getSize() <= 0)
		{
			bSendSuccess = true;
			break;
		}
	}

	if (checkConnectionState(CONNECTION_STATE_BREAK_CONNECTION))
	{
		close(m_socket);
		setLastErrorNo(-999999);
		setLastErrorString("client break the connection");
	}

	buffer.reset();
	// warring: copy the same memory block can be losing data
	buffer.write(reader.getData(), reader.getSize());

	return bSendSuccess;
}

void* NetworkModule_Impl_Android::netThread(void* pData)
{
	NetworkModule_Impl_Android* pNetworkModule = (NetworkModule_Impl_Android*)pData;
	if (!pNetworkModule) { return nullptr; }

	auto serial = pNetworkModule->m_serial;

	pNetworkModule->setConnectionState(NetworkModule::CONNECTION_STATE_DISCONNECT_BY_HOST, false);
	g_bThreadRunning = true;

	MsgPB::MsgConnectionState* pMsg = new MsgPB::MsgConnectionState();
	pMsg->set_estate(MsgPB::CS_CONNECTING);
	pNetworkModule->addDispatchMessage(pMsg, MsgPB::MCI_CONNECTION_STATE, EMPTY_STRING);

	int result = -1;
	tstring ip;
	if (pNetworkModule->m_isV4Ip)
	{
		// init address
		sockaddr_in addrServer;
		memset(&addrServer, 0, sizeof(addrServer));
		addrServer.sin_family = AF_INET;
		addrServer.sin_port = htons(pNetworkModule->m_nRequestPort);
		addrServer.sin_addr.s_addr = pNetworkModule->m_nRequestIp;

		// connect to server
		result = connect(pNetworkModule->m_socket, (struct sockaddr*)&addrServer, sizeof(addrServer));
		if (char* szIp = inet_ntoa(addrServer.sin_addr))
		{
			ip = szIp;
		}
	}
	else
	{
		result = connect(pNetworkModule->m_socket, pNetworkModule->m_aip->ai_addr, pNetworkModule->m_aip->ai_addrlen);
		ip = pNetworkModule->m_strServerAddress;
	}
	if (result < 0 && errno != EINPROGRESS)
	{
		// connect to server failed
		LOGE("connect to server {0}:{1} failed", ip, pNetworkModule->m_nRequestPort);

		MsgPB::MsgConnectionState* pMsgFailed = new MsgPB::MsgConnectionState();
		pMsgFailed->set_estate(MsgPB::CS_FAILED);
		pNetworkModule->addDispatchMessage(pMsgFailed, MsgPB::MCI_CONNECTION_STATE, EMPTY_STRING);

		g_bThreadRunning = false;
		pNetworkModule->m_connectState = ConnectState::Disconnect;
		return nullptr;
	}

	pNetworkModule->setConnectionState(CONNECTION_STATE_CONNECTING, false);
	pNetworkModule->setConnectionState(CONNECTION_STATE_CONNECTED, true);
	pNetworkModule->m_isConnected = true;
	pNetworkModule->m_connectState = ConnectState::Connected;

	LOGD("connect to server {0}:{1} success", ip, pNetworkModule->m_nRequestPort);

	MsgPB::MsgConnectionState* pMsgConnected = new MsgPB::MsgConnectionState();
	pMsgConnected->set_estate(MsgPB::CS_CONNECTED);
	pNetworkModule->addDispatchMessage(pMsgConnected, MsgPB::MCI_CONNECTION_STATE, EMPTY_STRING);

	char buf[DEFAULT_BUFFER];
	while(true)
	{
		if (pNetworkModule->m_socket == 0)
		{
			LOGD("null socket detected");
			break;
		}

		int recvLength = recv(pNetworkModule->m_socket, buf, DEFAULT_BUFFER, MSG_DONTWAIT);
		if (serial != pNetworkModule->m_serial) return nullptr;

		if (recvLength < 0)
		{
			int errorNo = errno;
			if (errorNo == EAGAIN)		// try again
			{
				std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
				continue;
			}

			// recv failed
			pNetworkModule->setLastErrorNo(errorNo);
			pNetworkModule->setLastErrorString(strerror(errorNo));
			LOGE("receive message failed: recvLength={0}, errno={1}, errno string={2}", recvLength, pNetworkModule->getLastErrorNo(), pNetworkModule->getLastErrorString());
			break;
		}
		else if (recvLength == 0)
		{
			// connection close by peer
			LOGD("connection close by peer");
			break;
		}
		else
		{
// 			tstring strBufferData;
// 			StringUtil::getBufferString(strBufferData, (const uchar*)buf, recvLength);
// 			LOGD("receive data size={0}: {1}", recvLength, strBufferData);

			pNetworkModule->decodeMsg(buf, recvLength);
		}
	}
	pNetworkModule->m_isConnected = false;
	pNetworkModule->m_connectState = ConnectState::Disconnect;

	if (!pNetworkModule->checkConnectionState(NetworkModule::CONNECTION_STATE_DISCONNECT_BY_HOST))
	{
		MsgPB::MsgConnectionState* pMsgConnectionBreak = new MsgPB::MsgConnectionState();
		pMsgConnectionBreak->set_estate(MsgPB::CS_BREAK);
		pNetworkModule->addDispatchMessage(pMsgConnectionBreak, MsgPB::MCI_CONNECTION_STATE, EMPTY_STRING);
	}

	g_bThreadRunning = false;
	return nullptr;
}

void NetworkModule_Impl_Android::decodeLocalMsg(const void* pData, int nLength, bool dirct/* = false*/)
{
	BufferStreamWriter recvBuffer;
	if (pData && nLength > 0)
	{
		recvBuffer.write(pData, nLength);
	}

	IMsg* pMsg = NULL;
	do
	{
		tstring strMd5;
		uint16 nMsgId = 0;
		m_pMsgDecoder->decodeMsg(pMsg, strMd5, recvBuffer, nMsgId);
		// add message into queue
		if (dirct)
		{
			ClientMsgJavaToCpp::getInstance().dispatchOneMessage(nMsgId, pMsg);
		}
		else
		{
			ClientMsgJavaToCpp::getInstance().addDispatchMessage(pMsg, nMsgId, strMd5);
		}

		LOGD("ClientMsgJavaToCpp::getInstance().addDispatchMessage msgId = {0}",nMsgId);
	} while (pMsg);
}

bool NetworkModule_Impl_Android::sendLocalMessage(Msg* pProtoMsg, uint16 nMsgId)
{
	if (!pProtoMsg) return false;

	char szBuf[ClientMsgJavaToCpp::CONST_DEFINE::MAX_SENDING_LENGTH + ClientMsgJavaToCpp::CONST_DEFINE::MSG_HEAD_LENGTH] = { 0 };
	int nProtoMsgSize = pProtoMsg->ByteSize();

	if (nProtoMsgSize > ClientMsgJavaToCpp::CONST_DEFINE::MAX_SENDING_LENGTH)
	{
		LOGE(("{0}::Too much bytes"), pProtoMsg->GetTypeName().c_str());
		return false;
	}
	memset(szBuf, 0, sizeof(szBuf));
	if (!pProtoMsg->SerializeToArray(szBuf + ClientMsgJavaToCpp::CONST_DEFINE::MSG_HEAD_LENGTH, nProtoMsgSize))
	{
		LOGD(("sendLocalMessage {0}::toStream failed"), pProtoMsg->GetTypeName().c_str());
		return false;
	}
    uint16* pDataSize = reinterpret_cast<uint16*>(szBuf);
	*pDataSize = nProtoMsgSize + ClientMsgJavaToCpp::CONST_DEFINE::MSG_HEAD_LENGTH;

    uint16* pMsgId = reinterpret_cast<uint16*>(szBuf + sizeof(uint16));
	*pMsgId = nMsgId;

	sendMsgPbToJava(nProtoMsgSize + ClientMsgJavaToCpp::CONST_DEFINE::MSG_HEAD_LENGTH, szBuf);
	LOGD(("sendLocalMessage {0}"), pProtoMsg->GetTypeName().c_str());
    return  true;
//	MsgMgrEvent evt(MsgMgrEvent::EventType::PostSendMessage, nMsgId);
//	postEvent(evt);
}

void sendMsgPbToJava(unsigned int nBufferSize, const void* pBuffer)
{
	JniMethodInfo methodInfo;
	if (JniHelper::getStaticMethodInfo(methodInfo, "com/igg/bzbee/app_sandbox/platform/network/MsgMgr", "sendMsgPbToJava", "([B)V"))
	{
		jbyteArray jarray = methodInfo.env->NewByteArray(nBufferSize);
		methodInfo.env->SetByteArrayRegion(jarray, 0, nBufferSize, (jbyte*)pBuffer);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jarray);
		methodInfo.env->DeleteLocalRef(jarray);
	}
}

NS_END


/*
* Class:     com_igg_bzbee_app_sandbox_platform_ToCppNative
* Method:    sendDataToCpp
* Signature: ([B)V
*/
JNIEXPORT void JNICALL Java_com_igg_bzbee_app_1sandbox_platform_ToCppNative_sendMsgPbToCpp(JNIEnv * env, jclass cls, jbyteArray bytes, jboolean dirct)
{
	jbyte* pBuffer = env->GetByteArrayElements(bytes, 0);
	jsize nBufferSize = env->GetArrayLength(bytes);

	bzbee::NetworkModule_Impl_Android* pNetworkModule = dynamic_cast<bzbee::NetworkModule_Impl_Android*>(&bzbee::NetworkModule::getInstance());
	if (pNetworkModule)
	{
		pNetworkModule->decodeLocalMsg(pBuffer, nBufferSize, dirct);
	}

	env->ReleaseByteArrayElements(bytes, pBuffer, 0);
}
