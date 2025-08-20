/*!
 * \file NetworkModule.h
 *
 * \author laizhisheng
 * \date 2016/12/26 10:37
 *
 */
#pragma once
#include "MsgHandler.h"
#include <Rtti.h>
#include <utils/BufferStream.h>
#include <queue>
#include <mutex>


NS_BEGIN

class NetworkModuleEvent : public Event
{
public:
	uint16 m_msgId{};
};

class NetworkModule : public EventHandler
{
	RTTI_ROOT(NetworkModule);
	DECLARE_EVENT_HOST();

public:
	NetworkModule(MsgDecoder* pDecoder);

public:
	static NetworkModule& getInstance(void);
	virtual ~NetworkModule(void);

	typedef std::function<bool(uint16& msg_type, Msg** pp_message, BufferStreamWriter& buffer)> Decoder;
	void setDecoder(const Decoder& decoder) { m_decoder = decoder; }

	void update(float dt);
	void terminate(void);
	static bool isInstanceInitialized(void) { return (s_pNetworkModule != nullptr); };

	virtual bool connectToServer(const char* ip, uint16 port) = 0;
	virtual bool connectToServer(uint ip, uint16 port) = 0;
	void disconnect(void);
	bool isConnected(void) const { return m_isConnected; }

	enum class ConnectState
	{
		Connecting,
		Connected,
		Disconnect
	};
	inline ConnectState getConnectState(void) const noexcept { return m_connectState; }
	inline void set_waiting(bool b) noexcept { m_waiting = b; }
	inline float get_health(void) const noexcept { return m_health; }

	virtual bool sendLocalMessage(Msg* pProtoMsg, uint16 nMsgId) = 0;
	void dispatchOneMessage(uint16 msgId, const Msg* pProtoMsg) { m_funcMsgCallback(msgId, pProtoMsg); }
	virtual bool doFlushSentBuffer(BufferStreamWriter& buffer) = 0;

	int getLastErrorNo(void) const { return m_nLastError; };
	const tstring& getLastErrorString(void) const { return m_strLastErrorStr; };

	typedef std::function<void(uint16 msgId, const Msg* pProtoMsg)> FuncMsgCallback;
	void setMsgCallback(const FuncMsgCallback& funcCallback) { m_funcMsgCallback = funcCallback; }

protected:
	static bool initializeInstance(NetworkModule* pMsgMgr);

	void setLastErrorNo(int errorNo) { m_nLastError = errorNo; };

	void setLastErrorString(const tstring& strErrorString) { m_strLastErrorStr = strErrorString; };

	bool checkConnectionState(uint state) const { return ((m_nConnectionState & state) == state); }
	bool setConnectionState(uint state, bool set);

	bool decodeMsg(const void* pData, int nLength);
	void addDispatchMessage(Msg* pProtoMsg, uint16 nMsgId, const tstring& strMd5);

	virtual void doDisconnect(void) = 0;

protected:
	static const int DEFAULT_BUFFER = 1024;
	static const int SOCKET_MAX_RETRY = 5;

	static const uint CONNECTION_STATE_CONNECTED = 1 << 0;
	static const uint CONNECTION_STATE_CONNECTING = 1 << 1;
	static const uint CONNECTION_STATE_DISCONNECT_BY_HOST = 1 << 2;
	static const uint CONNECTION_STATE_SIMULATE_SEND_ERROR = 1 << 3;
	static const uint CONNECTION_STATE_BREAK_CONNECTION = 1 << 4;

	MsgDecoder* m_pMsgDecoder{};

	bool m_isConnected{ false };

private:
	uint m_nConnectionState{};
	int m_nLastError{};
	tstring m_strLastErrorStr;

	std::mutex m_mutex;
	BufferStreamWriter m_RecvBuffer;

	class MessageInfo
	{
	public:
		uint16 m_msgId{};
		Msg* m_pProtoMsg{};
		tstring m_strMd5;
	};
	std::queue<MessageInfo> m_MsgQueue;

	static NetworkModule* s_pNetworkModule;
	FuncMsgCallback m_funcMsgCallback;

	Decoder m_decoder;

protected:
	float m_health{};
	bool m_waiting{ false };
	ConnectState m_connectState{ ConnectState::Connected };
};


NS_END
