/*!
 * \file LogUtil.h
 * \date 10-11-2014 16:13:32
 *
 *
 * \author zjhlogo (zjhlogo@gmail.com)
 */
#pragma once

#include "BaseType.h"
#include "format.h"
#include "Cpp11EventSystem.h"

NS_BEGIN

class LogUtil
{
public:
	enum class LogPriority
	{
		Info = 0,
		Debug,
		Warning,
		Error,
        Behavior,
		NumPriority,
	};

    class LogEvent : public Event
    {
        RTTI_DEF(LogEvent);

    public:
        LogEvent(const tstring& logInfo, LogPriority prio) : m_logInfo(logInfo), m_priority(prio) {};

    public:
        tstring m_logInfo;
        LogPriority m_priority{ LogPriority::Info };

    };

public:
	static void print(const char* location, int line, const char* func_name, LogPriority prio, const tstring& strMsg);
    static void logWithEvent(const char* location, int line, const char* func_name, LogPriority prio, const tstring& strMsg);

	static void logException(const tstring& message);
	static void reportException(const tstring& tag);

	static bool openLogFile(const tstring& logFile);
	static void closeLogFile();

	static bool getCachedLogs(tstring& strLogOut);

    static EventHandler& getEventInstance() { return m_evtHandler; };

private:
    static StringList m_exceptions;
    static EventHandler m_evtHandler;

};

NS_END

#ifndef __FUNCTION_NAME__
#ifdef WIN32   //WINDOWS
#define __FUNCTION_NAME__   __FUNCTION__
#else          //*NIX
#define __FUNCTION_NAME__   __func__
#endif
#endif

#define LOGI(...) (NS::LogUtil::print(__FILE__, __LINE__, __FUNCTION_NAME__, NS::LogUtil::LogPriority::Info, fmt::format(__VA_ARGS__)))
#define LOGD(...) (NS::LogUtil::print(__FILE__, __LINE__, __FUNCTION_NAME__, NS::LogUtil::LogPriority::Debug, fmt::format(__VA_ARGS__)))
#define LOGW(...) (NS::LogUtil::print(__FILE__, __LINE__, __FUNCTION_NAME__, NS::LogUtil::LogPriority::Warning, fmt::format(__VA_ARGS__)))
#define LOGE(...) (NS::LogUtil::print(__FILE__, __LINE__, __FUNCTION_NAME__, NS::LogUtil::LogPriority::Error, fmt::format(__VA_ARGS__)))
#define LOGB(...) (NS::LogUtil::print(__FILE__, __LINE__, __FUNCTION_NAME__, NS::LogUtil::LogPriority::Behavior, fmt::format(__VA_ARGS__)))

#define LOG_EVENT_I(...) (NS::LogUtil::logWithEvent(__FILE__, __LINE__, __FUNCTION_NAME__, NS::LogUtil::LogPriority::Info, fmt::format(__VA_ARGS__)))
#define LOG_EVENT_D(...) (NS::LogUtil::logWithEvent(__FILE__, __LINE__, __FUNCTION_NAME__, NS::LogUtil::LogPriority::Debug, fmt::format(__VA_ARGS__)))
#define LOG_EVENT_W(...) (NS::LogUtil::logWithEvent(__FILE__, __LINE__, __FUNCTION_NAME__, NS::LogUtil::LogPriority::Warning, fmt::format(__VA_ARGS__)))
#define LOG_EVENT_E(...) (NS::LogUtil::logWithEvent(__FILE__, __LINE__, __FUNCTION_NAME__, NS::LogUtil::LogPriority::Error, fmt::format(__VA_ARGS__)))
