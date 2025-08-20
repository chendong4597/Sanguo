/*!
 * \file HandlerMgrC.cpp
 *
 * \author laizhisheng
 * \date 2016/10/31 11:28
 *
 */
#include "HandlerMgr.h"
#include "stdafx.h"
#include "HandlerKeepAlive.h"
#include "HandlerLogin.h"
#include "HandlerItem.h"
#include "HandlerBuilding.h"
#include "HandlerTowerBuild.h"
#include "HandlerPlant.h"
#include "HandlerSurvival.h"
#include "HandlerMonster.h"
#include "HandlerRole.h"
#include "HandlerNpc.h"
#include "HandlerDrop.h"
#include "HandlerCraft.h"
#include "HandlerHouse.h"
#include "HandlerInteraction.h"
#include "HandlerTeam.h"
#include "HandlerDungeon.h"
#include "HandlerPublicMap.h"
#include "HandlerStore.h"
#include "HandlerAchieve.h"
#include "data/MapObj.h"
#include "LogUtil.h"
#include "../localserver/LocalServer_Battle.h"
#include "../localserver/LocalServer_Buff.h"
#include "../localserver/LocalServer_Action.h"
#include "../common/MsgMgr.h"

using namespace NS;

HandlerMgr& HandlerMgr::getInstance()
{
	//assert(g_HandlerMgr);
	static HandlerMgr s_HandlerMgr;
	return s_HandlerMgr;
}

HandlerMgr::HandlerMgr()
{
	//assert(!g_HandlerMgr);
	//g_HandlerMgr = this;
}

HandlerMgr::~HandlerMgr()
{
}

bool HandlerMgr::initialize(Component* pEntity)
{
	initializeHandlers();
	MsgMgr::getInstance().initialize();
	initializeLocalServers();
	return true;
}

void HandlerMgr::terminate()
{

	// each handler register event from some other handlers, so release it by
	// first terminate them all, then delete them all
	for (auto it = m_vMsgHanders.rbegin(); it != m_vMsgHanders.rend(); ++it)
	{
		auto pHandler = *it;
		pHandler->terminate();
	}
	for (auto it = m_vMsgHanders.rbegin(); it != m_vMsgHanders.rend(); ++it)
	{
		auto pHandler = *it;
		SAFE_DELETE(pHandler);
	}
	m_vMsgHanders.clear();

	MsgMgr::getInstance().terminate();

	m_vecClientMsgIds.clear();
}

void HandlerMgr::regAllMsgId()
{
}

void HandlerMgr::regClientMsgId(int sendId, int recvId)
{
	m_vecClientMsgIds.push_back(std::make_pair(sendId, recvId));
}

void HandlerMgr::openClientTransfer(int msgId)
{
#ifndef WIN32
#endif
}

void HandlerMgr::closeClientTransfer(int msgId)
{
#ifndef WIN32
#endif
}

void HandlerMgr::update(float dt)
{
	Timer::getInstance().update(dt);
	MsgMgr::getInstance().update(dt);
	MapObjMgr::getInstance().update(dt);
	for (auto pHandler : m_vMsgHanders)
		pHandler->update(dt);
}

void HandlerMgr::addHandler(MsgHandler* handler)
{
	m_vMsgHanders.push_back(handler);
}

void HandlerMgr::initializeLocalServers()
{
	MsgMgr::getInstance().addLocalServer(&(LocalServer_Battle::getInstance()));
	MsgMgr::getInstance().addLocalServer(&(LocalServer_Buff::getInstance()));
	MsgMgr::getInstance().addLocalServer(&(LocalServer_Action::getInstance()));
}


bool HandlerMgr::initializeHandlers()
{
	addHandler(new HandlerKeepAlive());
	addHandler(new HandlerLogin());
	addHandler(new HandlerItem());
	addHandler(new HandlerBuilding());
	addHandler(new HandlerTowerBuild());
	addHandler(new HandlerPlant());
	addHandler(new HandlerSurvival());
	addHandler(new HandlerMonster());
	addHandler(new HandlerRole());
	addHandler(new HandlerNpc());
	addHandler(new HandlerDrop());
	addHandler(new HandlerCraft());
	addHandler(new HandlerHouse());
	addHandler(new HandlerInteraction());
	addHandler(new HandlerTeam());
	addHandler(new HandlerDungeon());
	addHandler(new HandlerPublicMap());
	addHandler(new HandlerStore());
	addHandler(new HandlerAchieve());

	for (auto pHandler : m_vMsgHanders)
	{
		if (!pHandler->initialize())
		{
			LOGE("initialize handler failed: {0}", pHandler->getRtti()->getClassName());
			return false;
		}
	}

	return true;
}

