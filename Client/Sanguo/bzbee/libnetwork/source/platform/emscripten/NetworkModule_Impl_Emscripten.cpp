/*!
 * \file MsgMgr_Impl_Html5.cpp
 * \date 12-8-2012 16:46:04
 * 
 * 
 * \author Honny Chen (honnychen@gmail.com)
 */
#include "MsgMgr_Impl_Html5.h"
#include <utils/IPlatformUtil.h>
#include <utils/StringUtil.h>
#include <utils/ILogUtil.h>
#include <utils/StreamReader.h>
#include "network/MsgDefine.h"
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>


static bool g_bDisconnectByHost = false;
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
		s_pNetworkModule = new MsgMgr_Impl_Html5(new MsgDecoder());
	}

	return (s_pNetworkModule != nullptr);
}

MsgMgr_Impl_Html5::MsgMgr_Impl_Html5(MsgDecoder* pDecoder)
	: m_pMsgDecoder(pDecoder)
{
	m_socket = 0;
	m_nRequestIp = 0;
	m_nRequestPort = 0;
}

MsgMgr_Impl_Html5::~MsgMgr_Impl_Html5()
{
	// TODO: 
}

bool MsgMgr_Impl_Html5::doConnectToServer(const char* ip, ushort port)
{
	uint nIP = 0;
	inet_pton(AF_INET, ip, &nIP);
	return connectToServer(nIP, port);
}

bool MsgMgr_Impl_Html5::doConnectToServer(uint ip, ushort port)
{
	//disconnect from server
	disconnect();

	m_nRequestIp = ip;
	m_nRequestPort = port;

	// create socket
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket < 0)
	{
		LOGE(_SO("create socket failed, socket: %d"), m_socket);
		m_socket = 0;
		return false;
	}

	setConnectionState(CONNECTION_STATE_CONNECTING, true);

	// create thread to connect to server
    bool bOk = IPlatformUtil::getInstance().createThread((THREAD_FUNC)&MsgMgr_Impl_Html5::netThread, this);
	if (!bOk)
	{
		setConnectionState(CONNECTION_STATE_CONNECTING, false);
		LOGE(_SO("create network thread failed"));
		return false;
	}

	return true;
}

void MsgMgr_Impl_Html5::doDisconnect()
{
	if (m_socket != 0)
	{
		close(m_socket);
		m_socket = 0;
		LOGD(_SO("disconnect from server"));
	}

	// waiting for thread end
	while (g_bThreadRunning)
	{
		LOGD(_SO("waiting for thread end"));
		IPlatformUtil::getInstance().sleep(50);
	}
}

bool MsgMgr_Impl_Html5::doFlushSentBuffer(StreamWriter& buffer)
{
	StreamReader reader(buffer.getBuffer(), buffer.getBufferSize());
	if (reader.getSize() <= 0) return true;

	bool bSendSuccess = false;

	while (true)
	{
		int result = send(m_socket, (const char*)reader.getBuffer(), reader.getSize(), 0);

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

	buffer.reset();
	// warring: copy the same memory block can be losing data
	buffer.write(reader.getBuffer(), reader.getSize());

	return bSendSuccess;
}

void* MsgMgr_Impl_Html5::netThread(void* pData)
{
    MsgMgr_Impl_Html5* pMsgMgr = (MsgMgr_Impl_Html5*)pData;

	pMsgMgr->setConnectionState(IMsgMgr::CONNECTION_STATE_DISCONNECT_BY_HOST, false);
	g_bThreadRunning = true;

	// init address
	sockaddr_in addrServer;
	memset(&addrServer, 0, sizeof(addrServer));
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(pMsgMgr->m_nRequestPort);
	addrServer.sin_addr.s_addr = pMsgMgr->m_nRequestIp;

    MsgPB::MsgConnectionState* pMsg = new MsgPB::MsgConnectionState();
    pMsg->set_estate(MsgPB::CS_CONNECTING);
    pMsgMgr->addDispatchMessage(pMsg, MCI_CONNECTION_STATE, EMPTY_STRING, IMsgMgr::MSG_SOURCE_CLIENT);
	// connect to server
	int result = connect(pMsgMgr->m_socket, (struct sockaddr*)&addrServer, sizeof(addrServer));
	if (result < 0 && errno != EINPROGRESS)
	{
		// connect to server failed
		LOGE(_SO("connect to server %s:%d failed"), inet_ntoa(addrServer.sin_addr), pMsgMgr->m_nRequestPort);
		MsgConnectionState* pMsgFailed = new MsgConnectionState();
        pMsgFailed->set_estate(MsgPB::CS_FAILED);
        pMsgMgr->addDispatchMessage(pMsgFailed, MCI_CONNECTION_STATE, EMPTY_STRING, IMsgMgr::MSG_SOURCE_CLIENT);
		g_bThreadRunning = false;
		return NULL;
	}

	pMsgMgr->setConnectionState(CONNECTION_STATE_CONNECTING, false);
	pMsgMgr->setConnectionState(CONNECTION_STATE_CONNECTED, true);

	LOGD(_SO("connect to server %s:%d success"), inet_ntoa(addrServer.sin_addr), pMsgMgr->m_nRequestPort);
	MsgConnectionState* pMsgConnected = new MsgConnectionState();
    pMsgConnected->set_estate(MsgPB::CS_CONNECTED);
    pMsgMgr->addDispatchMessage(pMsgConnected, MCI_CONNECTION_STATE, EMPTY_STRING, IMsgMgr::MSG_SOURCE_CLIENT);

	int maxTryCount = SOCKET_MAX_RETRY;
	char buf[DEFAULT_BUFFER];

	while(true)
	{
		if (pMsgMgr->m_socket == 0)
		{
			LOGD(_SO("null socket detected"));
			break;
		}

		int recvLength = recv(pMsgMgr->m_socket, buf, DEFAULT_BUFFER, MSG_DONTWAIT);
		if (recvLength < 0)
		{
			int errorNo = errno;
			if (errorNo == EAGAIN)		// try again
			{
				IPlatformUtil::getInstance().sleep(100);		// sleep for 100 ms
				continue;
			}

			// recv failed
			pMsgMgr->setLastErrorNo(errorNo);
			pMsgMgr->setLastErrorString(strerror(errorNo));
			LOGE(_SO("receive message failed: recvLength=%d, errno=%d, errno string=%s"), recvLength, pMsgMgr->getLastErrorNo(), pMsgMgr->getLastErrorString().c_str());
			if (errorNo != ECONNRESET) break;

			// ECONNRESET:Connection reset by peer
			--maxTryCount;
			if (maxTryCount <= 0) break;
		}
		else if (recvLength == 0)
		{
			// connection close by peer
			LOGD(_SO("connection close by peer"));
			break;
		}
		else
		{
// 			tstring strBufferData;
// 			StringUtil::getBufferString(strBufferData, (const uchar*)buf, recvLength);
// 			LOGD(_T("receive data size=%d: %s"), recvLength, strBufferData.c_str());

			pMsgMgr->decodeMsg(buf, recvLength, IMsgMgr::MSG_SOURCE_SERVER);
		}
	}

	if (!pMsgMgr->checkConnectionState(IMsgMgr::CONNECTION_STATE_DISCONNECT_BY_HOST))
	{
		 MsgConnectionState* pMsgConnectionBreak = new MsgConnectionState();
        pMsgConnectionBreak->set_estate(MsgPB::CS_BREAK);
        pMsgMgr->addDispatchMessage(pMsgConnectionBreak, MCI_CONNECTION_STATE, EMPTY_STRING, IMsgMgr::MSG_SOURCE_CLIENT);
	}

	g_bThreadRunning = false;

	return NULL;
}

//bool MsgMgr_Impl_Html5::sendLocalMessage(IMsg* pProtoMsg, ushort nMsgId)
//{
//	if (!pProtoMsg) return false;
//
//	// TODO: 
//	IMsgMgrEvent event(IMsgMgrEvent::SEND_MESSAGE, nMsgId);
//	invokeEvent(&event);
//
//	return true;
//}
