/*!
 * \file HandlerKeepAlive.h
 *
 * \author zhijieli
 * \date 2016/12/20
 *
 * 
 */
#pragma once
#include <MsgHandler.h>


namespace MsgPB
{
	class MsgPong;
}


class HandlerKeepAlive final : public NS::MsgHandler
{
	RTTI_DEF(HandlerKeepAlive);
	DECLARE_EVENT_HOST();

public:
	static HandlerKeepAlive& getInstance();

	HandlerKeepAlive();
    virtual ~HandlerKeepAlive();

	virtual bool initialize() override;
	virtual void terminate() override {}
	virtual bool reset() override { return true; }

	NS::I64 getServerTime(void) const { return m_serverTime; }
	void setServerTime(NS::I64 serverTime) { m_serverTime = serverTime; }

private:
	void onMsgPong(const MsgPB::MsgPong& msg);

	NS::I64 m_serverTime{};
};


class HandlerKeepAliveEvent : public NS::Event
{
public:
	enum class EventType
	{
		Unknown = 0,
		Time_Beat,
		Time_Beat_Send,
		Time_Beat_Receive,
	};

public:
	HandlerKeepAliveEvent() {};
	virtual ~HandlerKeepAliveEvent(){};

public:
	EventType m_type{ EventType::Unknown };

	NS::I64 m_timeDelay{};
};
