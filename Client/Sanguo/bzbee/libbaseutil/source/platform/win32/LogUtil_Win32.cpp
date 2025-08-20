/*!
 * \file LogUtil_Win32.cpp
 * \date 10-11-2014
 *
 *
 * \author zjhlogo (zjhlogo@gmail.com)
 */
#include "../../LogUtil.h"
#include "../../StringUtil.h"
#include "../../file/FileSystem.h"
#include "../../FileUtil.h"
#include "../../utils/LogFile.h"
#include <json/value.h>
#include <windows.h>
#include <sstream>
#include <iostream>
#include <mutex>
#include <iomanip>
#include <chrono>

NS_BEGIN

static LogFile g_logFile;
static std::mutex s_mutexException;
StringList LogUtil::m_exceptions;

void LogUtil::print(const char* location, int line, const char* func_name, LogPriority prio, const tstring& strMsg)
{
    static const char* PRIORITY_MAP[static_cast<int>(LogUtil::LogPriority::NumPriority)] =
    {
        "Info",
        "Debug",
        "Warning",
        "Error",
        "Behavior",
    };

	static std::mutex s_mutex;
    std::lock_guard<std::mutex> guard{ s_mutex };

    // ### ChenBK 添加时间.
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::ostringstream oss;
	oss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S\t") << location << "(" << line << "): " << PRIORITY_MAP[static_cast<int>(prio)] << ": " << strMsg << std::endl;
	g_logFile.logString(oss.str());

	OutputDebugString(oss.str().c_str());
}

void LogUtil::logException(const tstring& message)
{
	std::lock_guard<std::mutex> guard{ s_mutexException };
	m_exceptions.push_back(message);
}

void LogUtil::reportException(const tstring& tag)
{
	assert(false && "Non-Fatal Exception occur, please contact developers to fix it");

	std::lock_guard<std::mutex> guard{ s_mutexException };
	m_exceptions.clear();
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
