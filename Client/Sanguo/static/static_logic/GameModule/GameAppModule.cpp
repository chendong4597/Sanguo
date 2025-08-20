/*!
 * \file GameAppModule.h
 *
 * \author mattho
 * \date 2017/12/20
 *
 *
 */
#include "GameAppModule.h"
#include <Framework.h>
#include <DispatchMsg.h>
#include "MacroUtility.h"
#include "SystemUtil.h"
#include "NetworkModule.h"
#include "../handler/HandlerMgr.h"
#include "config/LocalizationCfg.h"
#include "config/GlobalCfg.h"
//#include "../localserver/LocalServer_Battle.h"
#include "../common/ConfigInitializer.h"
#include "../common/MyLocalization.h"
#include "../common/MsgMgr.h"
#include "common/Timer.h"
#include "stdafx.h"



GameAppModule::GameAppModule()
	: NS::Component()
{
}

GameAppModule::~GameAppModule()
{
}

bool GameAppModule::initialize(NS::Component* pEntity)
{
	if (!Super::initialize(pEntity)) return false;
	if (!LocalizationCfg::getInstance().initialize()) return false;
	if (!ConfigInitializer::loadLanguage()) return false;
	if (!ConfigInitializer::initialize())return false;
	if (!addComponent(&HandlerMgr::getInstance())) return false;
	//if (!addComponent(&LocalServer_Battle::getInstance())) return false;
	HandlerMgr::getInstance().regAllMsgId();
	NS::MsgMgr::getInstance().SetServerMode(GlobalCfg::getInstance().IsServer());
	return true;
}

void GameAppModule::terminate()
{
	ConfigInitializer::terminate();
	Super::terminate();
}

void GameAppModule::update(float dt)
{
	Super::update(dt);
}

void GameAppModule::StartModule()
{
	//ConfigInitializer::LoadConfig();
}

void GameAppModule::StopModule()
{
	//副本编辑时用上
	//ConfigInitializer::UnloadConfig();
}



