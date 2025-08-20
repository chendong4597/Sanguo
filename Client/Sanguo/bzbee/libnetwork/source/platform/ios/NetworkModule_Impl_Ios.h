/*!
 * \file NetworkModule_Impl_Ios.h
 * \date 2018/09/10
 * 
 * 
 * \author lzj
 */
#pragma once

#include <NetworkModule.h>

struct addrinfo;

NS_BEGIN

class NetworkModule_Impl_Ios : public NetworkModule
{
public:
	NetworkModule_Impl_Ios(MsgDecoder* pDecoder);
	virtual ~NetworkModule_Impl_Ios() override;

	virtual bool sendLocalMessage(Msg* pProtoMsg, uint16 nMsgId) override;
	void decodeLocalMsg(const void* pData, int nLength);

protected:
	virtual bool connectToServer(const char* ip, uint16 port) override;
	virtual bool connectToServer(uint ip, uint16 port) override;
	virtual void doDisconnect() override;
	virtual bool doFlushSentBuffer(BufferStreamWriter& buffer) override;

	static void* netThread(void* pData);

private:
	int m_socket{};
    int m_serial{};

	uint m_nRequestIp{};
	uint16 m_nRequestPort{};
    
    struct addrinfo* m_aip{}, * m_resaddr{};
    bool m_isV4Ip{};
    tstring m_strServerAddress;
};

NS_END
