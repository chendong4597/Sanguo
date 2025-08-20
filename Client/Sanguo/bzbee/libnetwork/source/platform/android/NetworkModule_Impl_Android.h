/*!
 * \file NetworkModule_Impl_Android.h
 * \date 12-8-2012 16:45:30
 * 
 * 
 * \author zjhlogo (zjhlogo@gmail.com)
 */
#pragma once

#include "../../NetworkModule.h"
#include <jni.h>

struct addrinfo;

NS_BEGIN

class NetworkModule_Impl_Android : public NetworkModule
{
public:
	NetworkModule_Impl_Android(MsgDecoder* pDecoder);

	virtual bool sendLocalMessage(Msg* pProtoMsg, uint16 nMsgId) override;
	void decodeLocalMsg(const void* pData, int nLength, bool dirct = false);

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

	bool m_isV4Ip{};
	struct addrinfo* m_aip{};
	tstring m_strServerAddress;

};

void sendMsgPbToJava(unsigned int nBufferSize, const void* pBuffer);
NS_END

#ifdef __cplusplus
extern "C" {
#endif

	/*
	* Class:     com_igg_bzbee_app_sandbox_platform_ToCppNative
	* Method:    sendMsgPbToCpp
	* Signature: ([B)V
	*/
	JNIEXPORT void JNICALL Java_com_igg_bzbee_app_1sandbox_platform_ToCppNative_sendMsgPbToCpp
		(JNIEnv *, jclass, jbyteArray, jboolean);

#ifdef __cplusplus
}
#endif
