/*!
 * \file HandlerDrop.h
 *
 * \author huangjiangping
 * \date 3/17/2017
 *
 * 
 */
#pragma once
#include <MsgHandler.h>
#include "BaseTypeDefine.h"



namespace MsgPB
{
	class MsgMobSpawnerG2C;
}


class HandlerMsg : public NS::MsgHandler
{
	RTTI_DEF(HandlerMsg);
	DECLARE_EVENT_HOST();

public:
	static HandlerMsg& getInstance();

	HandlerMsg();
	virtual ~HandlerMsg();

	virtual bool initialize() override;
	virtual void terminate() override;
	virtual bool reset() override;

};


