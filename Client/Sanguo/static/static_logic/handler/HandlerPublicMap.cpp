/*!
 * \file HandlerBuilding.cpp
 *
 * \author xiaobowang;
 * \date 2017/3/17
 *
 */
#include "stdafx.h"
#include "../common/MsgMgr.h"
#include "../GameAppExtern.h"
#include "HandlerPublicMap.h"

using namespace NS;

static HandlerPublicMap* g_HandlerPublicMap = nullptr;
HandlerPublicMap& HandlerPublicMap::getInstance()
{
    assert(g_HandlerPublicMap);
    return *g_HandlerPublicMap;
}

HandlerPublicMap::HandlerPublicMap()
{
    assert(!g_HandlerPublicMap);
	g_HandlerPublicMap = this;
    reset();
}

HandlerPublicMap::~HandlerPublicMap()
{
	g_HandlerPublicMap = nullptr;
}

bool HandlerPublicMap::initialize()
{
	REGISTER_S2C(onMsgPublicMapChangeMapRs_S2C);
    return true;
}

void HandlerPublicMap::terminate()
{

    reset();
}

bool HandlerPublicMap::reset()
{
    return true;
}

bool HandlerPublicMap::requestJoinToPubllic()
{
	MsgPublicMapChangeMap_C2S msg;
	msg.set_type(CHANGE_MAP_TYPE_PRIVATE_TO_PUBLIC);
	MsgMgr::getInstance().sendMessageDirect(msg);
	return true;
}

bool HandlerPublicMap::requestJoinToPrivate()
{
	MsgPublicMapChangeMap_C2S msg;
	msg.set_type(CHANGE_MAP_TYPE_PUBLIC_TO_PRIVATE);
	MsgMgr::getInstance().sendMessageDirect(msg);
	return true;
}

void HandlerPublicMap::onMsgPublicMapChangeMapRs_S2C(const MsgPB::MsgPublicMapChangeMapRs_S2C& msg)
{

}





