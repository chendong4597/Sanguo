/*!
* \file MessageQueue.cpp
*
* \author zhichenxia
* \date 2018/8/9
*
*
*/

#include "MessageQueue.h"
#include "SyncCache.h"
#include "CacheQueue.h"
#include "Cache.h"
#include <msg/MsgLogin.pb.h>
#include <msg/MsgPing.pb.h>
#include <limits>
#include "LogUtil.h"
#include "MsgMgr.h"
#include "stdafx.h"

using namespace MsgPB;
using namespace NS;


#define check(expr) if(expr) {} else { auto str = fmt::format("Check failed: {0}", #expr); LOGE("{0}", str); throw std::runtime_error(str); }
std::map<MsgPB::EMsgType, MessageQueue::RegInfo> MessageQueue::m_reg;


static const float RECONNECT_DELAY = 1.5f;	// 断线重连间隔。
static const float OFFLINE_BLOCK = 120.0f;	// 离线游玩时限。
static const float PING_INTERVAL = 8.0f;	// Ping 间隔。
static const float SEND_TIMEOUT = 1.5f;		// 强联网消息限时发送。
static const float AUTO_CACHE = 0.7f;		// 自动保存缓存数据间隔。
static const float AUTO_SYNC = 15.0f;		// 连接正常时，自动同步间隔。
static const float HEALTHY = 0.7f;			// 网络健康度指标。
enum : size_t { PPS_LIMIT = 45u };			// 每秒发包数上限。


enum BlockType : int
{
	Offline,
	Send,
	Resp,
};


MessageQueue::MessageQueue(NS::I64 iggId, tstring checkText, const tstring& loginServer, const tstring& gameServerIp, int port)
	: m_iggId(iggId)
	, m_checkText(checkText)
	, m_ip(gameServerIp)
	, m_port(port)
{
	// 创建新的缓存队列。
	m_pCacheQueue.reset(new CacheQueue(m_iggId, loginServer));
}


MessageQueue::~MessageQueue(void)
{
	// 保存。
	if (SyncCache::getInstance().isChanged())
	{
		//MsgSyncClientLocalDataC2S msg;
		//SyncCache::getInstance().fill(msg);
		//SyncCache::getInstance().clear();
		//m_tickCache = 0.0f;
		//msg.set_serial(m_syncSerial++);
		//msg.set_sessionid(0);
		//msg.set_flag(SYNC_CLIENT_LOCAL_DATA_FLAG_SINGLE);

		//// 加入持久型缓存中。
		//tstring str;
		//msg.SerializeToString(&str);
		//m_pCacheQueue->push(str.data(), static_cast<uint32>(str.length()), true);
	}
	SyncCache::getInstance().clear();

	// 释放内存并清空消息队列。
	for (const auto& v : m_buffer)
		delete v.pMsg;
}


void MessageQueue::loadSync(NS::I64 lastSerial) try
{
	// 重复设置的情况下只有一次生效。
	if (m_syncSerial < 0) m_syncSerial = lastSerial;

	// 从文件中读取数据。
	NS::I64 timeStamp{};
	m_pCacheQueue->load([this, timeStamp](const BufferData& buffer) mutable -> bool
	{
		// 解析消息。
		//auto pSyncMsg = MsgSyncClientLocalDataC2S::default_instance().New();
		//if (!pSyncMsg->ParseFromArray(buffer.data(), static_cast<int>(buffer.size())))
		//	throw std::runtime_error("Parse message failed.");

		//// 忽略已经同步成功的。
		//if (m_syncSerial >= pSyncMsg->serial())
		//{
		//	LOGW("Ignore cache serial: {0}, server serial: {1}.", pSyncMsg->serial(), m_syncSerial);
		//	return false;
		//}
		//// 校验序列号严格递增。
		//check(++m_syncSerial == pSyncMsg->serial());
		//// 校验时间戳。
		//check(timeStamp <= pSyncMsg->time_stamp());
		//timeStamp = pSyncMsg->time_stamp();

		//sendImpl(_MSG_SYNC_CLIENT_LOCALDATA_C2S, *pSyncMsg);
		//delete pSyncMsg;

		return true;
	});
	++m_syncSerial;

	if (timeStamp != 0 && timeStamp > SyncCache::getInstance().getOriginLoginTime())
		throw std::runtime_error(fmt::format("Invalid time stamp from server: {0}, last is {1}.", SyncCache::getInstance().getOriginLoginTime(), timeStamp));
}
catch (const std::exception& ex)
{
	onError(ex.what());
}


void MessageQueue::discardFiles(void)
{
	try
	{
		m_pCacheQueue->discardFiles();
	}
	catch (...) {}

	Cache::getInstance().removeCache();
}


void MessageQueue::checkEmpty(void)
{
	if (SyncCache::getInstance().empty()) return;

	//// 获取当前的同步消息。
	//MsgSyncClientLocalDataC2S msg;
	//msg.set_serial(0);
	//msg.set_sessionid(0);
	//msg.set_flag(SYNC_CLIENT_LOCAL_DATA_FLAG_SINGLE);
	//SyncCache::getInstance().fill(msg);
	//LOGW("Unexpected sync message: {0}", msg.DebugString());

	// 清除实时缓存。
	SyncCache::getInstance().clear();
}


void MessageQueue::update(float dt)
{
	if (m_state == State::Error) return;
	m_time += dt;

	if (m_connect != NetworkModule::getInstance().getConnectState())
	{
		m_connect = NetworkModule::getInstance().getConnectState();
		if (m_connect == NetworkModule::ConnectState::Connected)
		{
			m_state = State::Encryptor;
			m_timeQueue.push(m_time);

			// 重新请求加密信息。
			//MsgMgr::getInstance().setEncryptionEnabled(false);
			//MsgEncryptorCode msgEncryptorCode;
			//MsgMgr::getInstance().sendMessageDirect(msgEncryptorCode);
		}
		else if (m_connect == NetworkModule::ConnectState::Disconnect)
		{
			m_state = State::Disconnect;	// 进入或重新进入断线重连状态。

			if (m_tickOffline <= 0.0f) m_tickOffline = OFFLINE_BLOCK;
			m_tickReconnect = RECONNECT_DELAY;	// 断线后延迟一小段时间再连接。
		}
	}

	if (SyncCache::getInstance().empty())
	{
		m_tickSync = 0.0f;
		m_tickCache = 0.0f;
	}
	else
	{
		//	if (HandlerLogin::getInstance().isPlaying() && !SyncCache::getInstance().empty())	// 仅当正常游戏且缓冲区非空时进行同步计时。
		if (m_state != State::Normal || m_tickSync <= 0.0f)	// 仅在登录状态正常的情况下定时自动同步。
			m_tickSync = AUTO_SYNC;
		else if ((m_tickSync -= dt) <= 0.0f)
			m_tickSync = 0.0f;

		//MsgSyncClientLocalDataC2S msg;
		//SyncCache::getInstance().fill(msg);
		//msg.set_serial(m_syncSerial);
		//msg.set_sessionid(0);
		//msg.set_flag(SYNC_CLIENT_LOCAL_DATA_FLAG_SINGLE);
		//if (m_tickSync == 0.0f || msg.ByteSize() > 48 * 1024)	// 限制弱联网缓存大小（警戒值）。
		//{
		//	SyncCache::getInstance().clear();
		//	m_tickCache = 0.0f;
		//	++m_syncSerial;

		//	// 正式加入持久型缓存中。
		//	tstring str;
		//	msg.SerializeToString(&str);
		//	m_pCacheQueue->push(str.data(), static_cast<uint32>(str.length()), true);

		//	sendImpl(_MSG_SYNC_CLIENT_LOCALDATA_C2S, msg);
		//}
		//else if ((m_tickCache += dt) > AUTO_CACHE)
		//{
		//	m_tickCache = 0.0f;

		//	// 实时写入持久型缓存。
		//	tstring str;
		//	msg.SerializeToString(&str);
		//	m_pCacheQueue->push(str.data(), static_cast<uint32>(str.length()), false);
		//}
	}

	// 更新限速队列。
	auto t = m_time - 1.0f;
	while (!m_timeQueue.empty() && m_timeQueue.front() < t)
		m_timeQueue.pop();

	switch (m_state)
	{
	case State::Disconnect:
		if (m_blockManager.getRetry() && m_tickReconnect > 0.0f && (m_tickReconnect -= dt) <= 0.0f)
		{
			m_tickReconnect = 0.0f;
			NetworkModule::getInstance().connectToServer(m_ip.c_str(), m_port);
			m_connect = NetworkModule::getInstance().getConnectState();
		}
		break;

	case State::Encryptor:
	case State::Relogin:
		break;

	case State::Normal:
		flush();
		if ((m_tickPing += dt) > PING_INTERVAL)
		{
			m_tickPing = 0.0f;
			++m_pingCount;	// 开始等待回包。
			NetworkModule::getInstance().set_waiting(true);

			MsgPing msg;
			sendImpl(EMsgType::_MSG_PING, msg);
		}
		//if (NetworkModule::getInstance().get_health() > HEALTHY)
		//	//FpsLabel::setNetwork(fmt::format("Weak ({0}ms)", static_cast<int>(NetworkModule::getInstance().get_health() * 1000.0f)));
		//else if (m_delay > 0)
		//	//FpsLabel::setNetwork(fmt::format("OK ({0}ms)", m_delay));
		//else
			//FpsLabel::setNetwork(fmt::format("OK"));
		break;

	default:
		break;
	}

	// 限时发送。
	t = m_time - SEND_TIMEOUT;
	//std::ostringstream os;
	//for (auto it = m_it; it != m_buffer.end() && it->time < t; ++it)
	//{
	//	if (it->type != EMsgType::_MSG_PING)// && it->type != EMsgType::_MSG_SYNC_CLIENT_LOCALDATA_C2S)
	//		os << ' ' << it->type;
	//}
	//auto reason = os.str();
	//if (!reason.empty())
	//	m_blockManager.setBlock(Send, reason);
	//else
	//	m_blockManager.clearBlock(Send);

	//// 限时回包。
	//os.str("");
	//for (auto it = m_wait.begin(); it != m_wait.end();)
	//{
	//	if (m_time >= it->skip)
	//		it = m_wait.erase(it);
	//	else
	//	{
	//		if (m_time >= it->timeout)
	//			os << ' ' << it->type;
	//		++it;
	//	}
	//}
	//const auto str = os.str();
	//if (!str.empty())
	//	m_blockManager.setBlock(Resp, os.str());
	//else
	//	m_blockManager.clearBlock(Resp);

	//// 限制离线游戏时间。
	//if (m_tickOffline > 0.0f && (m_tickOffline -= dt) <= 0.0f)
	//{
	//	m_tickOffline = 0.0f;
	//	m_blockManager.setBlock(Offline);
	//}

	//m_blockManager.update(dt);
}


void MessageQueue::send(EMsgType type, const Msg& msg)
{
	if (m_state == State::Error) return;

	// 验证参数的正确性。
	check(type != 0);

	// 处理限时回包。
	auto it = m_reg.find(type);
	if (it != m_reg.end())
	{
		const auto& info = it->second;
		auto timeout = m_time + info.timeout;
		m_wait.push_back({ type, info.resps, timeout, info.skip != 0.0f ? timeout + info.skip : std::numeric_limits<float>::max() });
	}

	sync();
	sendImpl(type, msg);
}


void MessageQueue::sendImpl(EMsgType type, const Msg& msg) try
{
	check(m_state != State::Error);
	++m_count;

	auto pCopy = msg.New();
	check(pCopy);
	pCopy->CopyFrom(msg);
	m_buffer.push_back({ m_time, type, pCopy });

	switch (m_state)
	{
	case State::Disconnect:
	case State::Encryptor:
	case State::Relogin:
		break;

	case State::Normal:
		if (!m_synchronizing && m_timeQueue.size() < PPS_LIMIT)
		{
			// 一定不存在未发出的消息。
			check(m_it == m_buffer.end());
			// 限速发送。
			m_timeQueue.push(m_time);
			MsgMgr::getInstance().sendMessageDirect(type, &msg);

			//if (type == EMsgType::_MSG_SYNC_CLIENT_LOCALDATA_C2S)
				//m_synchronizing = true;

			return;
		}
		break;

	default:
		break;
	}

	// 逾尾迭代器不会自动变为当前位置，所以需要特殊处理。
	if (m_it == m_buffer.end()) --m_it;	
}
catch (const std::exception& ex)
{
	onError(ex.what());
}


void MessageQueue::sync(void)
{
	if (m_state == State::Error) return;

	if (!SyncCache::getInstance().empty())
	{
		//MsgSyncClientLocalDataC2S msg;
		//SyncCache::getInstance().fill(msg);
		//SyncCache::getInstance().clear();
		//m_tickCache = 0.0f;
		//msg.set_serial(m_syncSerial++);
		//msg.set_sessionid(0);
		//msg.set_flag(SYNC_CLIENT_LOCAL_DATA_FLAG_SINGLE);

		//// 加入持久型缓存中。
		//tstring str;
		//msg.SerializeToString(&str);
		//m_pCacheQueue->push(str.data(), static_cast<uint32>(str.length()), true);

		//sendImpl(_MSG_SYNC_CLIENT_LOCALDATA_C2S, msg);
	}
}


void MessageQueue::onReceive(EMsgType type, const Msg& msg) try
{
	if (m_state == State::Error) return;

	switch (m_state)
	{
	case State::Disconnect:
		break;

	case State::Encryptor:
		if (type == EMsgType::_MSG_ENCRYPTOR_CODE_RESP)	// 只接受加密消息回包。
		{
			//const auto& msgEncryptorCodeResp = dynamic_cast<const MsgEncryptorCodeResp&>(msg);
			//MsgMgr::getInstance().changeCode(msgEncryptorCodeResp.code());	// 应用加密器。

			m_state = State::Relogin;
			m_timeQueue.push(m_time);

			MsgClientReConnectToGS msg;
			msg.set_iggid(m_iggId);
			msg.set_checktext(m_checkText);
			MsgMgr::getInstance().sendMessageDirect(msg);
		}
		break;

	case State::Relogin:
		if (type == EMsgType::_MSG_GS_RESP)	// 只接受重连消息回包。
		{
			const auto& msgGSResp = dynamic_cast<const MsgGSResp&>(msg);
			check(msgGSResp.loginresult() == LOGIN_RESULT::SUCCESS);
			//confirm(msgGSResp.client_history_msg());

			m_blockManager.clearBlock(Offline);
			NetworkModule::getInstance().set_waiting(false);
			m_state = State::Normal;
			m_synchronizing = false;
			m_tickOffline = 0.0f;
			m_tickPing = 0.0f;
			m_pingCount = 0;
			m_delay = 0;

			// 忽略 PingPong 消息。
			for (auto it = m_buffer.begin(); it != m_buffer.end();)
			{
				if (it->type == EMsgType::_MSG_PING)
				{
					delete it->pMsg;
					it = m_buffer.erase(it);
					--m_count;
				}
				else
					++it;
			}

			m_it = m_buffer.begin();
			flush();
		}
		break;

	case State::Normal:
		check(type != EMsgType::_MSG_CLIENT_RECONNECT_TO_GS);	// 不再接受登录。
		if (type == EMsgType::_MSG_PONG)
		{
			const auto& msgPong = dynamic_cast<const MsgPong&>(msg);
			check(--m_pingCount >= 0);
			if (m_pingCount == 0)
				NetworkModule::getInstance().set_waiting(false);

			// 计算网络延迟。
			m_delay = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - msgPong.clienttimeinms();
			check(m_delay >= 0);

			confirm(msgPong.client_history_msg());
		}
		//else if (type == EMsgType::_MSG_SYNC_CLIENT_LOCALDATA_S2C)
		//{
			//check(m_synchronizing);
			//m_synchronizing = false;

			//check(!m_buffer.empty());
			//auto it = m_it;
			//--it;
			//check(it->type == EMsgType::_MSG_SYNC_CLIENT_LOCALDATA_C2S);
			//const auto& mC2S = dynamic_cast<const MsgSyncClientLocalDataC2S&>(*it->pMsg);
			//const auto& mS2C = dynamic_cast<const MsgSyncClientLocalDataS2C&>(msg);
			//check(mS2C.flag() == SYNC_CLIENT_LOCAL_DATA_FLAG_SINGLE);
			//check(mS2C.serail() == mC2S.serial());

			////if (mS2C.res() == SYNC_CLIENT_LOCAL_DATA_MAPID)	// 忽略地图 ID 错误。
			////{
			////	LOGW("SYNC_CLIENT_LOCAL_DATA_MAPID: serail = {0}, flag = {1}", mS2C.serail(), mS2C.flag());
			////}
			////else
			//if (mS2C.res() != SYNC_CLIENT_LOCAL_DATA_OK)	// 本次同步失败，提示错误并退出游戏。
			//{
			//	auto text = SYNC_CLIENT_LOCAL_DATA_RES_Name(mS2C.res());
			//	if (text.empty()) text = fmt::format("Unknown result: {0}.", mS2C.res());
			//	throw std::runtime_error(text);
			//}

			//m_pCacheQueue->pop();
			//flush();
		//}
		break;

	default:
		break;
	}

	// 从限时回包队列中移除一项。
	for (auto it = m_wait.begin(); it != m_wait.end(); ++it)
	{
		for (const auto v : it->resps)
		{
			if (type == v)
			{
				m_wait.erase(it);
				return;
			}
		}
	}
}
catch (const std::exception& ex)
{
	onError(ex.what());
}


void MessageQueue::flush(void) try
{
	check(m_state != State::Error);

	//for (; !m_synchronizing && m_timeQueue.size() < PPS_LIMIT && m_it != m_buffer.end(); ++m_it)	// 限速释放缓存。
	//{
	//	m_timeQueue.push(m_time);
	//	MsgMgr::getInstance().sendMessageDirect(m_it->type, m_it->pMsg);

	//	if (m_it->type == EMsgType::_MSG_SYNC_CLIENT_LOCALDATA_C2S)
	//		m_synchronizing = true;
	//}
}
catch (const std::exception& ex)
{
	onError(ex.what());
}


void MessageQueue::confirm(int32 count) try
{
	check(m_state != State::Error);
	check(m_count >= count);		// 检查消息数。
	count = m_count - count;
	check(static_cast<int32>(m_buffer.size()) >= count);	// 检查队列中的消息数是否正确。

	// 删除服务端已收到的消息。
	while (static_cast<int32>(m_buffer.size()) > count)
	{
		check(m_it != m_buffer.begin());	// 不能确认尚未发出的消息。
		delete m_buffer.front().pMsg;
		m_buffer.pop_front();
	}
}
catch (const std::exception& ex)
{
	onError(ex.what());
}


bool MessageQueue::isHealthy(void) const
{
	return m_state == State::Normal && NetworkModule::getInstance().get_health() <= HEALTHY;
}


bool MessageQueue::empty(void)
{
	return m_buffer.empty() || m_it == m_buffer.end();
}


void MessageQueue::onError(const tstring& ex)
{
	// 清除所有同步数据。
	discardFiles();
	SyncCache::getInstance().clear();

	// 释放内存并清空消息队列。
	for (const auto& v : m_buffer)
		delete v.pMsg;
	m_buffer.clear();

	m_state = State::Error;

	SyncEvent evt;
	evt.description = ex;
	postEvent(evt);
}
