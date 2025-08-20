/*!
 * \file Cpp11EventSystem.cpp
 *
 * \author zjhlogo
 * \date 2019/10/30
 *
 * 
 */
#include "Cpp11EventSystem.h"
#include "GarbageCollection.h"
#include "lua/LuaCallback.h"

NS_BEGIN

EventHandler::EventTargetHost::EventTargetHost(const EventTargetHost& host)
{

}

EventHandler::EventTargetHost& EventHandler::EventTargetHost::operator=(const EventTargetHost& host)
{
    return *this;
}

EventHandler::EventTargetHost::~EventTargetHost()
{
    unregisterAllEvent();
}

void EventHandler::EventTargetHost::unregisterAllEvent()
{
    for (auto sender : m_senderSet)
    {
        sender->unregisterEventByHost(this);
    }

    m_senderSet.clear();
}

EventHandler::EventInfoBase::EventInfoBase(EventTargetHost* pHost)
    : m_pHost(pHost)
{

}

EventHandler::EventTargetHost* EventHandler::EventInfoBase::getHost() const
{
    return m_pHost;
}

EventHandler::EventHandler(const EventHandler& handler)
{

}

EventHandler& EventHandler::operator=(const EventHandler& handler)
{
    return *this;
}

EventHandler::~EventHandler()
{
    unregisterAllEvent();
}

void EventHandler::unregisterAllEvent()
{
    for (const auto& v : m_typeMap)
    {
        auto p = v.second;
        p->getHost()->m_senderSet.erase(this);
        _GC[p]{ delete p; };
    }

    for (const auto& v : m_nameMap)
    {
        auto p = v.second;
        p->getHost()->m_senderSet.erase(this);
        _GC[p]{ delete p; };
    }

    m_typeMap.clear();
    m_nameMap.clear();
}

void bzbee::EventHandler::postEvent(const tstring& eventName, Event& evt)
{
    evt.m_pSender = this;
    post(evt, m_nameMap, { m_nameMap.lower_bound(eventName + "$N"), m_nameMap.upper_bound(eventName + "$T") });
}

void EventHandler::__luaRegisterEvent(const tstring& eventName, EventTargetHost* pHost, const LuaCallback& callback)
{
    class EventInfo final : public EventInfoBase
    {
    public:
        EventInfo(EventTargetHost* p, const LuaCallback& callback) : EventInfoBase(p), m_callback(callback) {}
        virtual void invoke(Event& evt) override { m_callback(&evt); }

    private:
        LuaCallback m_callback;
    };

    m_nameMap.emplace(eventName + "$N", new EventInfo(pHost, callback));

    pHost->m_senderSet.insert(this);
}

void EventHandler::unregister(const EventTargetHost* pHost, TypeMap& container, std::pair<typename TypeMap::iterator, typename TypeMap::iterator> range)
{
    for (auto it = range.first; it != range.second; )
    {
        if (it->second->getHost() == pHost)
        {
            auto p = it->second;
            _GC[p]{ delete p; };

            it = container.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void EventHandler::unregister(const EventTargetHost* pHost, NameMap& container, std::pair<typename NameMap::iterator, typename NameMap::iterator> range)
{
    for (auto it = range.first; it != range.second; )
    {
        if (it->second->getHost() == pHost)
        {
            auto p = it->second;
            _GC[p]{ delete p; };

            it = container.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void EventHandler::removeHost(EventTargetHost* pHost)
{
    if (std::find_if(m_typeMap.begin(), m_typeMap.end(), [pHost](const TypeMap::value_type& value) { return value.second->getHost() == pHost; }) == m_typeMap.end())
    {
        if (std::find_if(m_nameMap.begin(), m_nameMap.end(), [pHost](const NameMap::value_type& value) { return value.second->getHost() == pHost; }) == m_nameMap.end())
        {
            pHost->m_senderSet.erase(this);
        }
    }
}

void EventHandler::unregisterEventByHost(const EventTargetHost* pHost)
{
    unregister(pHost, m_typeMap, { m_typeMap.begin(), m_typeMap.end() });
    unregister(pHost, m_nameMap, { m_nameMap.begin(), m_nameMap.end() });
}

void EventHandler::unregisterEvent(const std::type_index& typeIndex, const tstring& className, EventTargetHost* pHost)
{
    unregister(pHost, m_typeMap, m_typeMap.equal_range(typeIndex));
    unregister(pHost, m_nameMap, m_nameMap.equal_range(className + "$T"));
    removeHost(pHost);
}

void EventHandler::unregisterEvent(const tstring& eventName, EventTargetHost* pHost)
{
    unregister(pHost, m_nameMap, m_nameMap.equal_range(eventName + "$N"));
    removeHost(pHost);
}

NS_END
