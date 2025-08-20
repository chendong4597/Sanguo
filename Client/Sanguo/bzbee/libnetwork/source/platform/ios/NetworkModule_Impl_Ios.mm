/*!
 *
 * \file NetworkModule_Impl_Ios.cpp
 * \date 2018/09/10
 * 
 * 
 * \author lzj
 */
#include "NetworkModule_Impl_Ios.h"
#include <baseutil.h>
#include <SystemUtil.h>
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
#include <MsgNetWork.pb.h>
#include <NetworkModule.h>
#include <utils/BufferStream.h>
#include <DispatchMsg.h>
#include <Config.h>
#include "SystemUtil_Impl_Ios.h"

#import <Foundation/Foundation.h>
#import <SystemConfiguration/SystemConfiguration.h>

namespace {
    struct sockaddr_in zeroAddressIpv4;
    struct sockaddr_in6 zeroAddressIpv6;

    BOOL addressReachable(const struct sockaddr *hostAddress)
    {
        const SCNetworkReachabilityRef target = SCNetworkReachabilityCreateWithAddress(kCFAllocatorDefault, hostAddress);
        if (!target) { return FALSE; }
        
        SCNetworkReachabilityFlags flags = 0;
        if (!SCNetworkReachabilityGetFlags(target, &flags)) { return FALSE; }
        CFRelease(target);
        
        return (flags & kSCNetworkFlagsReachable) && (!(flags & kSCNetworkFlagsConnectionRequired));
    }
}

extern bool g_appWillTerminate;

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
		s_pNetworkModule = new NetworkModule_Impl_Ios(new MsgDecoder());
	}

        bzero(&zeroAddressIpv4, sizeof(zeroAddressIpv4));
        zeroAddressIpv4.sin_len = sizeof(zeroAddressIpv4);
        zeroAddressIpv4.sin_family = AF_INET;

        bzero(&zeroAddressIpv6, sizeof(zeroAddressIpv6));
        zeroAddressIpv6.sin6_len = sizeof(zeroAddressIpv6);
        zeroAddressIpv6.sin6_family = AF_INET6;

	return (s_pNetworkModule != nullptr);
}

NetworkModule_Impl_Ios::NetworkModule_Impl_Ios(MsgDecoder* pDecoder)
	:NetworkModule(pDecoder)
{

}

NetworkModule_Impl_Ios::~NetworkModule_Impl_Ios()
{
    if(m_aip)
    {
        freeaddrinfo(m_aip);
        m_aip = {};
    }
}

bool NetworkModule_Impl_Ios::connectToServer(const char* ip, uint16 port)
{
    m_isV4Ip = true;
    // 检测字符串是不是IPV4的格式
    for(const char *pc = ip; *pc != 0; pc++)
    {
        if((*pc < '0' || *pc > '9') && *pc != '.')
        {
            m_isV4Ip = false;
            break;
        }
    }
    if (m_isV4Ip)
    {
        struct addrinfo hints,* pResAdd;
        memset(&hints, 0, sizeof(hints));
        pResAdd = NULL;
        hints.ai_flags = AI_DEFAULT;
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        
        int error = getaddrinfo("cgi.igg.com", "9000", &hints, &pResAdd);
        if (error == 0)
        {
            if (pResAdd->ai_addr->sa_family == AF_INET6 || Config::DEFAULT_ENVIRONMENT == Config::ENV_PRODUCTION)
            {
                m_isV4Ip = false;
            }
            
            if (!m_isV4Ip)
            {
                int iIp[4];
                sscanf(ip, "%d.%d.%d.%d", &iIp[0], &iIp[1], &iIp[2], &iIp[3]);
                m_strServerAddress = fmt::format("{0}-{1}-{2}-{3}.ip.igotgames.net", iIp[0], iIp[1], iIp[2], iIp[3]);
                return connectToServer((uint)0, port);
            }
        }
        if(pResAdd)
        {
            freeaddrinfo(pResAdd);
            pResAdd = NULL;
        }
        uint nIP = 0;
        inet_pton(AF_INET, ip, &nIP);
        return connectToServer(nIP, port);
    }
    else
    {
        m_strServerAddress = ip;
        return connectToServer((uint)0, port);
    }
}

bool NetworkModule_Impl_Ios::connectToServer(uint ip, uint16 port)
{
    ++ m_serial;
    
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
        hints.ai_flags = AI_DEFAULT;
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        m_aip = NULL;
        m_resaddr = NULL;
        char szPort[10];
        sprintf(szPort, "%d", port);
        
        int error = getaddrinfo(m_strServerAddress.c_str(), szPort, &hints, &m_aip);
        if(error != 0)
        {
            if (m_aip)
            {
                freeaddrinfo(m_aip);
                m_aip = {};
            }
            return false;
        }
        
        for (m_resaddr = m_aip; m_resaddr; m_resaddr = m_resaddr->ai_next)
        {
            m_socket = socket(m_resaddr->ai_family, m_resaddr->ai_socktype, m_resaddr->ai_protocol  );
            if (m_socket < 0) { continue; }
            switch(m_resaddr->ai_addr->sa_family)
            {
                case AF_INET:
                {
                    struct sockaddr_in *v4sa = (struct sockaddr_in *)m_resaddr->ai_addr;
                    v4sa->sin_port = htons(port);
                }
                    break;
                case AF_INET6:
                {
                    struct sockaddr_in6 *v6sa = (struct sockaddr_in6 *)m_resaddr->ai_addr;
                    v6sa->sin6_port = htons(port);
                }
                    break;
            }
            break;
        }
        
    }
	if (m_socket < 0)
	{
		LOGE("create socket failed, socket: {0} ip = {1} port = {2}", m_socket, ip, port);
		m_socket = 0;
		return false;
	}

	setConnectionState(CONNECTION_STATE_CONNECTING, true);

	// create thread to connect to server
	std::thread t{ &NetworkModule_Impl_Ios::netThread, this };
	if (!t.joinable())
	{
		setConnectionState(CONNECTION_STATE_CONNECTING, false);
		LOGE("create network thread failed");
		return false;
	}
	t.detach();

	return true;
}

void NetworkModule_Impl_Ios::doDisconnect()
{
    g_bThreadRunning = false;
    m_isConnected = false;
    
	if (m_socket != 0)
	{
		close(m_socket);
		m_socket = 0;
        if(m_aip)
        {
            freeaddrinfo(m_aip);
            m_aip = {};
        }
		LOGD("disconnect from server");
	}
}

bool NetworkModule_Impl_Ios::doFlushSentBuffer(BufferStreamWriter& buffer)
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

void* NetworkModule_Impl_Ios::netThread(void* pData)
{
	NetworkModule_Impl_Ios* pNetworkModule = (NetworkModule_Impl_Ios*)pData;
    if (!pNetworkModule) { return {}; }

    auto serial = pNetworkModule->m_serial;
	pNetworkModule->setConnectionState(NetworkModule::CONNECTION_STATE_DISCONNECT_BY_HOST, false);
	g_bThreadRunning = true;

	MsgPB::MsgConnectionState* pMsg = new MsgPB::MsgConnectionState();
	pMsg->set_estate(MsgPB::CS_CONNECTING);
	pNetworkModule->addDispatchMessage(pMsg, MsgPB::MCI_CONNECTION_STATE, EMPTY_STRING);
    
    int result{};
    tstring ip;
    struct sockaddr *hostAddress{};
    if (pNetworkModule->m_isV4Ip)
    {
        // init address
        sockaddr_in addrServer;
        memset(&addrServer, 0, sizeof(addrServer));
        addrServer.sin_family = AF_INET;
        addrServer.sin_port = htons(pNetworkModule->m_nRequestPort);
        addrServer.sin_addr.s_addr = pNetworkModule->m_nRequestIp;
        
        hostAddress = reinterpret_cast<sockaddr*>(&zeroAddressIpv4);
        // connect to server
        result = connect(pNetworkModule->m_socket, (struct sockaddr*)&addrServer, sizeof(addrServer));
        if (char* szIp = inet_ntoa(addrServer.sin_addr))
        {
            ip = szIp;
        }
    }
    else
    {
        hostAddress = reinterpret_cast<sockaddr*>(&zeroAddressIpv6);
        result = connect(pNetworkModule->m_socket, pNetworkModule->m_resaddr->ai_addr, pNetworkModule->m_resaddr->ai_addrlen);
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
		return NULL;
	}
    int value{ 1 };
    setsockopt(pNetworkModule->m_socket, SOL_SOCKET, SO_NOSIGPIPE, &value, sizeof(value));

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
        if (g_appWillTerminate) {
            g_bThreadRunning = false;
            return {};
        }
		if (pNetworkModule->m_socket == 0)
		{
			LOGD("null socket detected");
			break;
		}
        if (!addressReachable(hostAddress)) {
            LOGD("Network is not available.");
            break;
        }

		int recvLength = static_cast<int>(recv(pNetworkModule->m_socket, buf, DEFAULT_BUFFER, MSG_DONTWAIT));
        if (serial != pNetworkModule->m_serial) { return {}; }
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

void bzbee::NetworkModule_Impl_Ios::decodeLocalMsg(const void* pData, int nLength)
{

}

bool bzbee::NetworkModule_Impl_Ios::sendLocalMessage(Msg* pProtoMsg, uint16 nMsgId)
{
    return static_cast<SystemUtil_Impl_Ios&>(SystemUtil::getInstance()).processLocalMsg(pProtoMsg, nMsgId);
}

NS_END
