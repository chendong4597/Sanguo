/*!
* \file HandlerDrop.h
*
* \author huangjiangping
* \date 3/17/2017
*
*
*/
#include "stdafx.h"
#include "HandlerMsg.h"


using namespace NS;
using namespace MsgPB;

static HandlerMsg* g_HandlerMsg = nullptr;
HandlerMsg& HandlerMsg::getInstance()
{
    assert(g_HandlerMsg);
    return *g_HandlerMsg;
}

HandlerMsg::HandlerMsg()
{
    assert(!g_HandlerMsg);
	g_HandlerMsg = this;
	reset();
}

HandlerMsg::~HandlerMsg()
{
	g_HandlerMsg = nullptr;
}

bool HandlerMsg::initialize()
{
	return true;
}

void HandlerMsg::terminate()
{
	reset();
}

bool HandlerMsg::reset()
{
	return true;
}

