/*!
* \file LocalServer.h
*
* \author zhichenxia
* \date 2018/5/10
*
*
*/
#pragma once
#include <component/Component.h>
#include "../common/MsgMgr.h"

NS_BEGIN

// LocalServer 基类。
class LocalServer : public Component
{
	RTTI_DEF(LocalServer);

	LocalServer            (const LocalServer&) = delete;
	LocalServer& operator= (const LocalServer&) = delete;

protected:
	LocalServer(void) = default;

public:
	virtual ~LocalServer(void) = default;

	void setEnable(bool enable);
	bool getEnable(void) const { return m_enable; }

protected:
	// 可由子类重写：服务器启用/禁用。
	virtual void onEnableChanged(bool enable);

	// 发送 S->C 消息。
	template<typename MsgT> void send          (const MsgT& message) { MsgMgr::getInstance().send_s2c(MsgT::default_instance().msgtype(), &message); }
	// 发送 C->S 消息。
	template<typename MsgT> void send_to_remote(const MsgT& message) { MsgMgr::getInstance().send_c2s(MsgT::default_instance().msgtype(), &message); }

protected:
	template<typename FunctorT> void operator<<= (const FunctorT& functor) { _hook_impl<typename Decl<decltype(&FunctorT::operator())>::ArgType>(functor); }
	template<typename ThisT, typename ArgT> void _hook(ThisT* pThis, void(ThisT::*method)(ArgT)) { _hook_impl<std::remove_cv_t<std::remove_reference_t<ArgT>>>(std::bind(method, pThis, std::placeholders::_1)); }
	template<typename ThisT, typename ArgT> void _unhook(void(ThisT::*)(ArgT)) { _unhook<std::remove_cv_t<std::remove_reference_t<ArgT>>>(); }
	template<typename MsgT> void _unhook(void) { MsgMgr::getInstance().unhook(this, MsgT::default_instance().msgtype()); }

private:
	template<typename> struct Decl;
	template<typename T, typename ArgT> struct Decl<void(T::*)(ArgT) const> { typedef std::remove_cv_t<std::remove_reference_t<ArgT>> ArgType; };
	template<typename T, typename ArgT> struct Decl<void(T::*)(ArgT)      > { typedef std::remove_cv_t<std::remove_reference_t<ArgT>> ArgType; };
	template<typename MsgT> void _hook_impl(const std::function<void(const MsgT&)>& callback) { MsgMgr::getInstance().hook(this, MsgT::default_instance().msgtype(), new MsgCallback<MsgT>(callback)); }

private:
	bool m_enable{ false };
};

// 监听 C->S 消息。
#define HOOK *this <<=
#define HOOK_MSG(method) _hook(this, &std::remove_pointer<decltype(this)>::type::method)
// 取消监听消息。（仅用于非持续监听的消息。服务器被禁用时或者析构时不需要显式调用此函数。）
#define UNHOOK(MsgT) _unhook<MsgT>()
#define UNHOOK_MSG(method) _unhook(&std::remove_pointer<decltype(this)>::type::method)

NS_END
