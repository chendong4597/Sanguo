/*!
 * \file Cpp11EventSystem.h
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 02/05/2016 11:21:19
 *
 *
 */
#pragma once
#include "BaseType.h"
#include "Rtti.h"
#include "lua/LuaPlus/LuaPlus.h"

#include <typeindex>
#include <functional>
#include <algorithm>
#include <memory>
#include <map>

class LuaCallback;

NS_BEGIN

class EventHandler;

class Event
{
	RTTI_ROOT(Event);
	friend class EventHandler;

public:
	virtual ~Event(void) = default;

	EventHandler* getSender(void) const { return m_pSender; }

private:
	EventHandler* m_pSender{};
};


class LuaEvent final : public Event
{
	RTTI_DEF(LuaEvent);

public:
	LuaEvent(const LuaPlus::LuaObject& obj) : m_obj(obj) {}

	const LuaPlus::LuaObject& get(void) const { return m_obj; }

private:
	LuaPlus::LuaObject m_obj;
};

class EventHandler
{
public:
	class EventTargetHost final
	{
		friend class EventHandler;

	public:
		EventTargetHost(void) = default;
        EventTargetHost(const EventTargetHost& host);
        EventTargetHost& operator= (const EventTargetHost& host);
        ~EventTargetHost();

        void unregisterAllEvent();

	private:
		std::set<EventHandler*> m_senderSet;
	};

private:
	class EventInfoBase
	{
	public:
        EventInfoBase(EventTargetHost* pHost);

		virtual ~EventInfoBase(void) = default;
		virtual void invoke(Event& evt) = 0;

        EventTargetHost* getHost(void) const;

	private:
		EventTargetHost* m_pHost{};
	};

	typedef std::multimap<std::type_index, EventInfoBase*> TypeMap;
	typedef std::multimap<tstring, EventInfoBase*> NameMap;

public:
	EventHandler() = default;
    EventHandler(const EventHandler& handler);
    EventHandler& operator= (const EventHandler& handler);
    virtual ~EventHandler();

    void unregisterAllEvent();

public:
	template<typename EventT> void postEvent(EventT& evt)
	{
		evt.m_pSender = this;

		auto range1 = m_typeMap.equal_range(typeid(EventT));
		auto range2 = m_nameMap.equal_range(EventT::__RttiData()->getClassName() + "$N");

		unsigned int n = 0;
		if (range1.first != range1.second) n |= 1u;
		if (range2.first != range2.second) n |= 2u;

		if ((n & 1u)) post(evt, m_typeMap, range1);
		if ((n & 2u)) post(evt, m_nameMap, range2);
	}

    void postEvent(const tstring& eventName, Event& evt);

private:
	template <typename EventT, typename ContainerT>
	static void post(EventT& evt, const ContainerT& container, std::pair<typename ContainerT::iterator, typename ContainerT::iterator> range)
	{
		if (range.first == range.second) return;

		std::vector<EventInfoBase*> li;
		for (auto it = range.first; it != range.second; ++it) li.push_back(it->second);

		for (auto pInfo : li)
		{
			if (std::find_if(container.begin(), container.end(), [pInfo](const typename ContainerT::value_type& value)
			{
				return pInfo == value.second;
			}) == container.end()) continue;

			pInfo->invoke(evt);
		}
	}

public:
    void __luaRegisterEvent(const tstring& eventName, EventTargetHost* pHost, const LuaCallback& callback);

	template<typename EventT> void registerEvent(EventTargetHost* pHost, const std::function<void(EventT& evt)>& callback)
	{
		class EventInfo final : public EventInfoBase
		{
		public:
			EventInfo(EventTargetHost* p, const std::function<void(EventT& evt)>& callback1) : EventInfoBase(p), m_callback(callback1) {}
			virtual void invoke(Event& evt) override { m_callback(static_cast<EventT&>(evt)); }

		private:
            std::function<void(EventT& evt)> m_callback;
		};
		m_typeMap.emplace(std::type_index(typeid(EventT)), new EventInfo(pHost, callback));

		registerEvent_Raw<EventT>(EventT::__RttiData()->getClassName() + "$T", pHost, callback);
	}

	template<typename EventT> void registerEvent(const tstring& eventName, EventTargetHost* pHost, const std::function<void(EventT& evt)>& callback)
	{
		registerEvent_Raw<EventT>(eventName + "$N", pHost, callback);
	}

private:
	template<typename EventT> void registerEvent_Raw(const tstring& eventName, EventTargetHost* pHost, const std::function<void(EventT& evt)>& callback)
	{
		class EventInfo final : public EventInfoBase
		{
		public:
			EventInfo(EventTargetHost* p, const std::function<void(EventT& evt)>& callback1) : EventInfoBase(p), m_callback(callback1) {}
			virtual void invoke(Event& evt) override { if (auto p = dynamic_cast<EventT*>(&evt)) m_callback(*p); }

		private:
            std::function<void(EventT& evt)> m_callback;
		};
		m_nameMap.emplace(eventName, new EventInfo(pHost, callback));

		pHost->m_senderSet.insert(this);
	}

public:
    void unregisterEvent(const std::type_index& typeIndex, const tstring& className, EventTargetHost* pHost);
    void unregisterEvent(const tstring& eventName, EventTargetHost* pHost);
    void unregisterEventByHost(const EventTargetHost* pHost);

private:
    static void unregister(const EventTargetHost* pHost, TypeMap& container, std::pair<typename TypeMap::iterator, typename TypeMap::iterator> range);
    static void unregister(const EventTargetHost* pHost, NameMap& container, std::pair<typename NameMap::iterator, typename NameMap::iterator> range);
    void removeHost(EventTargetHost* pHost);

private:
	TypeMap m_typeMap;
	NameMap m_nameMap;
};

class RegEvent final
{
	template<typename> struct Decl;
	template<typename T, typename ArgT> struct Decl<void(T::*)(ArgT) const> { typedef std::remove_cv_t<std::remove_reference_t<ArgT>> ArgType; };
	template<typename T, typename ArgT> struct Decl<void(T::*)(ArgT)      > { typedef std::remove_cv_t<std::remove_reference_t<ArgT>> ArgType; };

public:
	RegEvent(EventHandler::EventTargetHost* pHost, EventHandler* pSender)
		: m_pHost(pHost)
		, m_pSender(pSender)
	{}

	template<typename FunctorT> void operator<<= (const FunctorT& functor) { m_pSender->template registerEvent<typename Decl<decltype(&FunctorT::operator())>::ArgType>(m_pHost, functor); }

private:
	EventHandler::EventTargetHost* m_pHost;
	EventHandler* m_pSender;
};

NS_END

#define DECLARE_EVENT_HOST() private: NS::EventHandler::EventTargetHost __event_host__

#define UNREGISTER_ALL_EVENT() { __event_host__.unregisterAllEvent(); unregisterAllEvent(); }

#define REGISTER_EVENT(sender, eventType, callback, ...) (sender)->template registerEvent<eventType>(&__event_host__, std::bind(callback, this, std::placeholders::_1, ##__VA_ARGS__))
#define UNREGISTER_EVENT(sender, eventType) (sender)->unregisterEvent(typeid(eventType), eventType::__RttiData()->getClassName() , &__event_host__)

#define REG_EVENT(pSender, ...) NS::RegEvent(&__event_host__, pSender) <<=
#define UNREG_EVENT UNREGISTER_EVENT
