/*!
* \file MessageQueue.h
*
* \author zhichenxia
* \date 2018/8/9
*
*
*/
#pragma once
#include <MsgDecoder.h>
#include <Cpp11EventSystem.h>
#include <NetworkModule.h>
#include <msg/MsgType.pb.h>
#include "BlockManager.h"
#include <map>
#include <queue>

NS_BEGIN


class SyncEvent final : public Event
{
	RTTI_DEF(SyncEvent);

public:
	tstring description;
};


class CacheQueue;
class MessageQueue final : public EventHandler
{
public:
	MessageQueue(NS::I64 iggId, tstring checkText, const tstring& loginServer, const tstring& gameServerIp, int port);
	virtual ~MessageQueue(void);

	void loadSync(NS::I64 lastSerial);
	void discardFiles(void);
	void checkEmpty(void);

	void send(MsgPB::EMsgType type, const Msg& msg);
	void sync(void);
	void onReceive(MsgPB::EMsgType type, const Msg& msg);
	void update(float dt);

	enum class State
	{
		Error,
		Disconnect,
		Encryptor,
		Relogin,
		Normal,
	};
	inline State getState(void) const noexcept { return m_state; }
	inline bool getRetry(void) const { return m_blockManager.getRetry(); }
	inline bool isBlocking(void) const { return m_blockManager.getBlock(); }
	bool isHealthy(void) const;
	bool empty(void);

public:
	static inline void regResp(MsgPB::EMsgType type, const std::vector<MsgPB::EMsgType>& resps, float timeout = 0.0f, float skip = 0.0f) { assert(m_reg.emplace(type, RegInfo{ resps, timeout, skip }).second); }
	static inline void regResp(MsgPB::EMsgType type,                   MsgPB::EMsgType   resp , float timeout = 0.0f, float skip = 0.0f) { regResp(type, std::vector<MsgPB::EMsgType>{ resp }, timeout, skip); }
	static inline void regResp(MsgPB::EMsgType type,                                                                  float skip       ) { regResp(type, std::vector<MsgPB::EMsgType>(), 0.0f, skip); }

private:
	void sendImpl(MsgPB::EMsgType type, const Msg& msg);
	void flush(void);
	void confirm(int32 count);

	void onError(const tstring& ex);

private:
	std::unique_ptr<CacheQueue> m_pCacheQueue;
	BlockManager m_blockManager;

	struct ListItem
	{
		float time;
		MsgPB::EMsgType type;
		Msg* pMsg;
	};
	typedef std::list<ListItem> Buffer;
	Buffer m_buffer;				// 发送缓冲队列。
	Buffer::const_iterator m_it{ m_buffer.end() };
	std::queue<float> m_timeQueue;	// 用于限制发包率。

	State m_state{ State::Normal };	// 消息队列介入时一定是已登录状态。
	int32 m_count{ 1 };			// 初始时要算上登录的消息。
	NS::I64 m_syncSerial{ -1 };
	NS::I64 m_delay{};
	int m_pingCount{};
	bool m_synchronizing{ false };
	NetworkModule::ConnectState m_connect{ NetworkModule::ConnectState::Connected };
	float m_tickReconnect{};
	float m_tickPing{};
	float m_tickSync{};
	float m_tickCache{};
	float m_tickOffline{};
	float m_time{};

	// 断线重连的游戏服务器地址。
	tstring m_ip;
	int m_port{};
	NS::I64 m_iggId{};
	tstring m_checkText;

private:	// 限时回包相关。
	struct RegInfo
	{
		std::vector<MsgPB::EMsgType> resps;
		float timeout;
		float skip;
	};
	static std::map<MsgPB::EMsgType, RegInfo> m_reg;

	struct WaitInfo
	{
		MsgPB::EMsgType type;
		const std::vector<MsgPB::EMsgType>& resps;
		float timeout;
		float skip;
	};
	std::list<WaitInfo> m_wait;
};


NS_END
