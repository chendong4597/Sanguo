/*!
 * \file NetworkModule_Impl_Win32.cpp
 *
 * \author zhijieli
 * \date July 2016
 *
 * 
 */

#include "NetworkModule_Impl_Win32.h"
#include <LogUtil.h>
#include <thread>
#include <chrono>
#include <ws2tcpip.h>
#include "../../MsgNetWork.pb.h"

NS_BEGIN
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
		s_pNetworkModule = new NetworkModule_Impl_Win32(new MsgDecoder());
	}

	return (s_pNetworkModule != nullptr);
}

NetworkModule_Impl_Win32::NetworkModule_Impl_Win32(MsgDecoder* pDecoder)
	: NetworkModule(pDecoder)
{
	WSADATA wsaData;
	if (WSAStartup(0x0202, &wsaData) != 0)
	{
		LOGE("WSAStartup failed");
	}
}

NetworkModule_Impl_Win32::~NetworkModule_Impl_Win32()
{
	WSACleanup();
}

bool NetworkModule_Impl_Win32::connectToServer(const char* ip, uint16 port)
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
		return connectToServer(inet_addr(ip), port);
	}
	else
	{
		m_strServerAddress = ip;
		return connectToServer(uint(0), port);
	}
}

bool NetworkModule_Impl_Win32::connectToServer(uint ip, uint16 port)
{
	// disconnect from server
	disconnect();
	m_connectState = ConnectState::Connecting;

	if (m_isV4Ip)
	{
		m_nRequestIp = ip;
		m_nRequestPort = port;

		// create socket
		m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	}
	else
	{
		struct addrinfo hints;
		memset(&hints, 0, sizeof(hints));
		hints.ai_flags = 0;
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		m_aip = {};
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

	if (m_socket == NULL)
	{
		LOGE("create socket failed, socket: {0}", m_socket);
		m_socket = NULL;
		return false;
	}

	setConnectionState(CONNECTION_STATE_CONNECTING, true);

	// create thread to connect to server
	std::thread t{ &NetworkModule_Impl_Win32::netThread, this };
	if (!t.joinable())
	{
		setConnectionState(CONNECTION_STATE_CONNECTING, false);
		LOGE("create network thread failed");
		return false;
	}
	t.detach();

	return true;
}

void NetworkModule_Impl_Win32::doDisconnect()
{
	if (m_socket)
	{
		closesocket(m_socket);
		m_socket = NULL;
		if (m_aip)
		{
			freeaddrinfo(m_aip);
			m_aip = {};
		}

// 		LOGD("disconnect from server");
	}

	// 等待线程退出（最多等 3 秒，如果没有退出则直接放弃线程）。
	for (int i = 0; i < 60 && g_bThreadRunning; ++i)
	{
// 		LOGD("waiting for thread end");
		std::this_thread::sleep_for(std::chrono::milliseconds{50});
	}
	g_bThreadRunning = false;
}

bool NetworkModule_Impl_Win32::doFlushSentBuffer(BufferStreamWriter& buffer)
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
			int err = WSAGetLastError();
			setLastErrorNo(err);
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
		closesocket(m_socket);
		setLastErrorNo(-999999);
		setLastErrorString("client break the connection");
	}

	buffer.reset();
	// warring: copy the same memory block can be losing data
	buffer.write(reader.getData(), reader.getSize());

	return bSendSuccess;
}

void* NetworkModule_Impl_Win32::netThread(void* pData)
{
 	NetworkModule_Impl_Win32* pNetworkModule = (NetworkModule_Impl_Win32*)pData;
	assert(pNetworkModule);
	if (!pNetworkModule) { return nullptr; }
 
 	pNetworkModule->setConnectionState(NetworkModule::CONNECTION_STATE_DISCONNECT_BY_HOST, false);
 	g_bThreadRunning = true;

	MsgPB::MsgConnectionState* pMsg = new MsgPB::MsgConnectionState();
	pMsg->set_estate(MsgPB::CS_CONNECTING);
	pNetworkModule->addDispatchMessage(pMsg, MsgPB::MCI_CONNECTION_STATE, EMPTY_STRING);
	
	int result = -1;
	tstring ip;
	if (pNetworkModule->m_isV4Ip)
	{
		SOCKADDR_IN addrServer;
		memset(&addrServer, 0, sizeof(addrServer));
		addrServer.sin_addr.S_un.S_addr = pNetworkModule->m_nRequestIp;
		addrServer.sin_family = AF_INET;
		addrServer.sin_port = htons(pNetworkModule->m_nRequestPort);
		result = connect(pNetworkModule->m_socket, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));
		pNetworkModule->setLastErrorNo(WSAGetLastError());
		if (char* szIp = inet_ntoa(addrServer.sin_addr))
		{
			ip = szIp;
		}
	}
	else
	{
		result = connect(pNetworkModule->m_socket, pNetworkModule->m_aip->ai_addr, pNetworkModule->m_aip->ai_addrlen);
		pNetworkModule->setLastErrorNo(WSAGetLastError());
		ip = pNetworkModule->m_strServerAddress;
	};

 	// connect to server
 	if (result == SOCKET_ERROR)
 	{
 		// connect to server failed
		LOGE("connect to server {0}:{1} failed, errorNo={2}", ip, pNetworkModule->m_nRequestPort, pNetworkModule->getLastErrorNo());

		MsgPB::MsgConnectionState* pMsgFailed = new MsgPB::MsgConnectionState();
		pMsgFailed->set_estate(MsgPB::CS_FAILED);
		pNetworkModule->addDispatchMessage(pMsgFailed, MsgPB::MCI_CONNECTION_STATE, EMPTY_STRING);

 		g_bThreadRunning = false;
		pNetworkModule->m_connectState = ConnectState::Disconnect;
 		return 0;
 	}
 
 	u_long nMode = 1;
 	result = ioctlsocket(pNetworkModule->m_socket, FIONBIO, &nMode);
 	if (result != NO_ERROR)
 	{
		LOGE("ioctlsocket failed with error: {0}", result);
 	}
 
 	pNetworkModule->setConnectionState(CONNECTION_STATE_CONNECTING, false);
	pNetworkModule->setConnectionState(CONNECTION_STATE_CONNECTED, true);
	pNetworkModule->m_isConnected = true;
	pNetworkModule->m_connectState = ConnectState::Connected;

// 	LOGD("connect to server {0}:{1} success", ip, pNetworkModule->m_nRequestPort);

	MsgPB::MsgConnectionState* pMsgConnected = new MsgPB::MsgConnectionState();
	pMsgConnected->set_estate(MsgPB::CS_CONNECTED);
	pNetworkModule->addDispatchMessage(pMsgConnected, MsgPB::MCI_CONNECTION_STATE, EMPTY_STRING);

	char buf[DEFAULT_BUFFER];
 	while(true)
 	{
 		if (pNetworkModule->m_socket == 0)
 		{
//  			LOGD("null socket detected");
 			break;
 		}

 		int recvLength = recv(pNetworkModule->m_socket, buf, DEFAULT_BUFFER, 0);
 		if(recvLength < 0)
 		{
 			int errorNo = WSAGetLastError();
 			if (errorNo == WSAEWOULDBLOCK)		// try again
 			{
				std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
 				continue;
 			}
 
 			pNetworkModule->setLastErrorNo(errorNo);
 			pNetworkModule->setLastErrorString(strerror(errorNo));
			LOGE("receive message failed: recvLength={0}, errno={1}, errno string={2}", recvLength, pNetworkModule->getLastErrorNo(), pNetworkModule->getLastErrorString());
 
			break;
 		}
 		else if (recvLength == 0)
 		{
 			// connection close by peer
//  			LOGD("connection close by peer");
 			break;
 		}
 		else
 		{
 			pNetworkModule->decodeMsg(buf, recvLength/*, NetworkModule::MsgSource::Server*/);
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

bool NetworkModule_Impl_Win32::sendLocalMessage(Msg* pProtoMsg, uint16 nMsgId)
{
	if (!pProtoMsg) return false;

	// TODO: 

	return true;
}

NS_END
