/*!
 * \file LogUtil.cpp
 *
 * \author zjhlogo
 * \date 2019/10/22
 *
 * 
 */
#include "LogUtil.h"

NS_BEGIN

EventHandler LogUtil::m_evtHandler;

void LogUtil::logWithEvent(const char* location, int line, const char* func_name, LogUtil::LogPriority prio, const tstring& strMsg)
{
    print(location, line, func_name, prio, strMsg);

    LogEvent evt(strMsg, prio);
    m_evtHandler.postEvent(evt);
}

NS_END
