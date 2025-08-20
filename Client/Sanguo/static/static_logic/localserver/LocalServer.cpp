/*!
* \file LocalServer.cpp
*
* \author zhichenxia
* \date 2018/5/10
*
*
*/
#include "stdafx.h"
#include "LocalServer.h"


NS_BEGIN


void LocalServer::setEnable(bool enable)
{
	if (m_enable == enable) return;
	m_enable = enable;
	onEnableChanged(enable);
}


void LocalServer::onEnableChanged(bool b) {}


NS_END
