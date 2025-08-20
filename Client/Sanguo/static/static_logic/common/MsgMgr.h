/*!
 * \file MsgMgr.h
 *
 * \author laizhisheng
 * \date 2016/12/26 14:47
 *
 */
#pragma once
#include <MsgHandler.h>
//#include "MsgEncryptor.h"

#include <utils/BufferStream.h>
#include "component/Component.h"
#include <queue>


namespace MsgPB
{
	class MsgSyncClientLocalDataC2S;
}

class UiInternetConnection;
class LocalServer;

NS_BEGIN

class NetworkModuleEvent;
class LocalServer;
class MessageQueue;
extern MessageQueue* g_pMessageQueue;


class MsgMgr final : public EventHandler
{
	DECLARE_EVENT_HOST();
	MsgMgr(void) = default;

public:
	static MsgMgr& getInstance(void);

	void initialize(void);
	void terminate(void);
	void update(float dt);

public:
	Msg * createMessage(const tstring& name) const;
	uint16 findIdByName(const tstring& name) const;

	

	bool sendMessage(uint16 msgId, const Msg* pMsg);
	template<typename MsgT> bool sendMessage(const MsgT& msg) { return sendMessage(msg.msgtype(), &msg); }

	bool sendMessageLocal(uint16 msgId, const Msg* pMsg);
	template<typename MsgT> bool sendMessageLocal(const MsgT& msg) { return sendMessageLocal(msg.msgtype(), &msg); }

	bool sendMessageDirect(uint16 msgId, const Msg* pMsg);
	template<typename MsgT> bool sendMessageDirect(const MsgT& msg) { return sendMessageDirect(msg.msgtype(), &msg); }

	typedef std::function<bool(uint16 msgId, const Msg* pMsg)> FuncMsgCallback;
	void setDebugLogCallback(const FuncMsgCallback& debugLogCallback) { m_debugLogCallback = debugLogCallback; }

	template<typename MsgT, typename CallbackT> 
    void reg(EventHandler* pHandler, const CallbackT& callback) 
    { 
        reg(pHandler, MsgT::default_instance().msgtype(), new MsgCallback<MsgT>(callback)); 
    }
	void reg(EventHandler* pHandler, uint16 msgId, BaseMsgCallback* pCallback);
	void reg(EventHandler* pHandler, const tstring& msgName, BaseMsgCallback* pCallback);

	template<typename MsgT> void unreg(EventHandler* pHandler) { unreg(pHandler, MsgT::default_instance().msgtype()); }
	void unreg(EventHandler* pHandler, uint16 msgId);
	void unreg(EventHandler* pHandler, const tstring& msgName);

	void sendLocalMessage(uint16 msgId, Msg* pMsg);

	void addLocalServer(LocalServer* pHandler);
	void hook(LocalServer* pHandler, uint16 msgId, BaseMsgCallback* pCallback);
	void unhook(LocalServer* pHandler, uint16 msgId);

public:	// LocalServer.


	void send_s2c(uint16 msgId, const Msg* pMsg);
	void send_c2s(uint16 msgId, const Msg* pMsg);

	void SetClientMode(bool bClientMode) { m_clientMode = bClientMode; }
	bool IsClientMode() { return m_clientMode; }

	void SetServerMode(bool bServerMode) { m_serverMode = bServerMode; }
	bool IsServerMode() { return m_serverMode; }

private:
	bool decode(uint16& msg_type, Msg** pp_message, BufferStreamWriter& buffer);
	void dispatchOneMessage(uint16 msgId, const Msg* pMsg, bool hookable);

	void onDispatchOneLocalMsg(const NetworkModuleEvent& evt);

	struct SubscribeInfo
	{
		EventHandler * pHandler{};
		BaseMsgCallback* pCallback{};
	};
	typedef std::multimap<uint16, SubscribeInfo> SubscribeInfoMap;
	SubscribeInfoMap::iterator findMessageInfo(uint16 msgId, EventHandler* pHandler);

private:
	SubscribeInfoMap m_SubscribeMsgMap;
	std::unordered_map<tstring, uint16    > m_name_to_id;
	std::unordered_map<tstring, const Msg*> m_name_to_message;
	std::unordered_map<uint16 , const Msg*> m_id_to_message;

	BufferStreamWriter m_bigMsgRecvBuffer;

	struct HookInfo
	{
		LocalServer * pHandler{};
		BaseMsgCallback* pCallback{};
	};
	std::multimap<uint16, HookInfo> m_hookMap;
	std::vector<LocalServer*> m_hookSet;

	FuncMsgCallback m_debugLogCallback{};

	BufferData m_bufferTemp;

	bool m_clientMode{ false };
	bool m_serverMode{ false };

	float m_tickPing{ 0.f };

	bool m_encryptionEnabled{};
	//CMsgEncryptor<0x82, 0x23, 0x61, 0x33, 0x17, 0x33, 0xEE, 0x50> m_sendEncryptor;
	//CMsgEncryptor<0x51, 0x68, 0xAD, 0x55, 0x21, 0x23, 0x70, 0x66> m_recvEncryptor;
};


template <typename ThisT> class RegS2C
{
	template<typename> struct Decl;
	template<typename T, typename ArgT> struct Decl<void(T::*)(ArgT) const> { typedef std::remove_cv_t<std::remove_reference_t<ArgT>> ArgType; };
	template<typename T, typename ArgT> struct Decl<void(T::*)(ArgT)      > { typedef std::remove_cv_t<std::remove_reference_t<ArgT>> ArgType; };

public:
	RegS2C(ThisT* pThis) : m_pThis(pThis) {}
	template<typename FunctorT> inline void operator<<= (const FunctorT& functor) { MsgMgr::getInstance().reg<typename Decl<decltype(&FunctorT::operator())>::ArgType>(m_pThis, functor); }

private:
	ThisT* m_pThis;
};

NS_END


template <typename ThisT> NS::RegS2C<ThisT> __reg_s2c(ThisT* pThis) { return NS::RegS2C<ThisT>(pThis); }
#define REG_S2C __reg_s2c(this) <<=
#define UNREG_S2C(MsgT) NS::MsgMgr::getInstance().unreg<MsgT>(this)

template<typename ThisT, typename ArgT> void __register_s2c(ThisT* pThis, void(ThisT::*method)(ArgT)) { NS::MsgMgr::getInstance().reg<std::remove_cv_t<std::remove_reference_t<ArgT>>>(pThis, std::bind(method, pThis, std::placeholders::_1)); }
#define REGISTER_S2C(callback) __register_s2c(this, &std::remove_pointer<decltype(this)>::type::callback)
template<typename ThisT, typename ArgT> void __unregister_s2c(ThisT* pThis, void(ThisT::*)(ArgT)) { NS::MsgMgr::getInstance().unreg<std::remove_cv_t<std::remove_reference_t<ArgT>>>(pThis); }
#define UNREGISTER_S2C(method) __unregister_s2c(this, &std::remove_pointer<decltype(this)>::type::method)


