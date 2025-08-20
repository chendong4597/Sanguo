/*!
 * \file MsgHandler.h
 *
 * \author zhijieli
 * \date July 2016
 *
 * 
 */

#pragma once
#include <Rtti.h>
#include <Cpp11EventSystem.h>
#include <google/protobuf/message.h>
#include "MsgDecoder.h"
#include <memory>


NS_BEGIN

///////////////////////////////////////////////////////////////////////////
class MsgHandler : public EventHandler
{
	RTTI_ROOT(MsgHandler);

public:
	MsgHandler();
	virtual ~MsgHandler();

public:
	virtual bool initialize() = 0;
	virtual void terminate() = 0;
	virtual bool reset() = 0;
	virtual void update(float dt) {};
};


class BaseMsgCallback
{
public:
	BaseMsgCallback() {};
	virtual ~BaseMsgCallback() {};

	virtual void invoke(const Msg& msg) = 0;
};

template<typename T>
class MsgCallback : public BaseMsgCallback
{
public:
	MsgCallback(const std::function<void(const T& evt)>& callback) : m_callback(callback) {};
	virtual ~MsgCallback() {};

	virtual void invoke(const Msg& msg) override
	{
		m_callback(static_cast<const T&>(msg));
	}

private:
	std::function<void(const T& evt)> m_callback;

};

NS_END
