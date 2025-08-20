/*!
 * \file LogUtil_Android.cpp
 * \date 10-11-2014
 *
 *
 * \author zjhlogo (zjhlogo@gmail.com)
 */
#include "../../LogUtil.h"
#include "../../FileUtil.h"
#include "../../utils/LogFile.h"
#include "JniHelper.h"
#include <android/log.h>
#include <sstream>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <chrono>

NS_BEGIN

static LogFile g_logFile;
static std::mutex s_mutexException;
StringList LogUtil::m_exceptions;

void invokeCrashlyticsLog(const tstring& message)
{
    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, "com/igg/bzbee/app_sandbox/BehaviorLog", "log", "(Ljava/lang/String;)V"))
    {
        jstring jstrMessage = methodInfo.env->NewStringUTF(message.c_str());
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jstrMessage);
        methodInfo.env->DeleteLocalRef(jstrMessage);
    }
}

void invokeCrashlyticsLogException(const tstring& tag, const StringList& messages)
{
	JniMethodInfo methodInfo;
	if (JniHelper::getStaticMethodInfo(methodInfo, "com/igg/bzbee/app_sandbox/BehaviorLog", "logException", "(Ljava/lang/String;[Ljava/lang/String;)V"))
	{
		jstring jstrTag = methodInfo.env->NewStringUTF(tag.c_str());

		jobjectArray jarrMessages = methodInfo.env->NewObjectArray(messages.size(), methodInfo.env->FindClass("java/lang/String"), 0);
		for (int i = 0; i < messages.size(); i++)
		{
			jstring str = methodInfo.env->NewStringUTF(messages[i].c_str());
			methodInfo.env->SetObjectArrayElement(jarrMessages, i, str);
            methodInfo.env->DeleteLocalRef(str);
        }

		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jstrTag, jarrMessages);

        methodInfo.env->DeleteLocalRef(jarrMessages);
        methodInfo.env->DeleteLocalRef(jstrTag);
	}
}

void LogUtil::print(const char* location, int line, const char* func_name, LogPriority prio, const tstring& strMsg)
{
	static const int PRIORITY_MAP[static_cast<int>(LogPriority::NumPriority)] =
	{
		ANDROID_LOG_INFO,
		ANDROID_LOG_DEBUG,
		ANDROID_LOG_WARN,
		ANDROID_LOG_ERROR,
		ANDROID_LOG_INFO,
	};

	std::ostringstream oss;
#ifndef NDEBUG
	oss << location << "(" << line << "): " << PRIORITY_MAP[static_cast<int>(prio)] << ": " << strMsg << std::endl;
#else
	oss << PRIORITY_MAP[static_cast<int>(prio)] << ": " << strMsg << std::endl;
#endif

	g_logFile.logString(oss.str());

	// invoke java code to log to crashlytics
	if (prio == LogPriority::Behavior)
    {
        invokeCrashlyticsLog(oss.str());
    }

	__android_log_write(PRIORITY_MAP[static_cast<int>(prio)], func_name, oss.str().c_str());
}

void LogUtil::logException(const tstring& message)
{
	std::lock_guard<std::mutex> guard{ s_mutexException };
	m_exceptions.push_back(message);
}

void LogUtil::reportException(const tstring& tag)
{
	StringList listCopy;

	{
		std::lock_guard<std::mutex> guard{ s_mutexException };
        if (m_exceptions.empty()) return;

		listCopy = m_exceptions;
		m_exceptions.clear();
	}

	invokeCrashlyticsLogException(tag, listCopy);
}

bool LogUtil::openLogFile(const tstring& logFile)
{
	return g_logFile.openLogFile(logFile);
}

void LogUtil::closeLogFile()
{
	g_logFile.closeLogFile();
}

bool LogUtil::getCachedLogs(tstring& strLogOut)
{
	g_logFile.flush();
	return FileUtil::readFileIntoString(strLogOut, g_logFile.getLogFilePath());
}

NS_END
