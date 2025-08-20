/*!
 * \file MsgMgr_Impl_Android.h
 * \date 12-8-2012 16:45:30
 * 
 * 
 * \author zjhlogo (zjhlogo@gmail.com)
 */
#pragma once

#include "../../Server.h">

class MsgMgr_Impl_Html5 : public NetworkModule
{
public:
	RTTI_DEF(MsgMgr_Impl_Html5, NetworkModule);
    
	MsgMgr_Impl_Html5(MsgDecoder* pDecoder);
    virtual ~MsgMgr_Impl_Html5();

	//virtual bool sendLocalMessage(IMsg* pProtoMsg, ushort nMsgId) override;

protected:
	virtual bool doConnectToServer(const char* ip, ushort port);
	virtual bool doConnectToServer(uint ip, ushort port);
	virtual void doDisconnect();
	virtual bool doFlushSentBuffer(StreamWriter& buffer);

	static void* netThread(void* pData);

private:
	int m_socket;

	uint m_nRequestIp;
	ushort m_nRequestPort;

};


