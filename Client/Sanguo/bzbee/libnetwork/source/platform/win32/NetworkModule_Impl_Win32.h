/*!
 * \file NetworkModule_Impl_Win32.h
 *
 * \author zhijieli
 * \date July 2016
 *
 * 
 */
#pragma once
#include "../../NetworkModule.h"
#include <WinSock2.h>

struct addrinfo;
NS_BEGIN

class NetworkModule_Impl_Win32 : public NetworkModule
{
public:
	NetworkModule_Impl_Win32(MsgDecoder* pDecoder);
	virtual ~NetworkModule_Impl_Win32() override;

	virtual bool sendLocalMessage(Msg* pProtoMsg, uint16 nMsgId) override;

protected:
	virtual bool connectToServer(const char* ip, uint16 port) override;
	virtual bool connectToServer(uint ip, uint16 port) override;
	virtual void doDisconnect() override;
	virtual bool doFlushSentBuffer(BufferStreamWriter& buffer) override;

	static void* netThread(void* pData);

private:
	SOCKET m_socket{};

	uint m_nRequestIp{};
	uint16 m_nRequestPort{};

	bool m_isV4Ip{};
	struct addrinfo* m_aip{};
	tstring m_strServerAddress;

};

NS_END