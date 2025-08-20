/*!
 * \file LogUtil_Ios.cpp
 * \date 2018/09/10
 *
 *
 * \author lzj
 */
#include "../../LogUtil.h"
#include <sstream>
#include <iostream>
#import <Foundation/Foundation.h>

NS_BEGIN

static std::mutex s_mutexException;
StringList LogUtil::m_exceptions;

void LogUtil::print(const char* location, int line, const char* func_name, LogPriority prio, const tstring& strMsg)
{
    static const char* PRIORITY_MAP[static_cast<int>(LogPriority::NumPriority)] =
    {
        "Info",
        "Debug",
        "Warning",
        "Error",
        "Behavior"
    };

    std::ostringstream oss;
#ifndef NDEBUG
    oss << location << "(" << line << "): " << PRIORITY_MAP[static_cast<int>(prio)] << ": " << strMsg << std::endl;
#else
    oss << PRIORITY_MAP[static_cast<int>(prio)] << ": " << strMsg << std::endl;
#endif

    NSLog(@"%s", oss.str().c_str());
    NSLog(@"\n");
}

void LogUtil::logException(const tstring& message)
{
    std::lock_guard<std::mutex> guard{ s_mutexException };
    m_exceptions.push_back(message);
}

void LogUtil::reportException(const tstring& tag)
{
    assert(false);

    std::lock_guard<std::mutex> guard{ s_mutexException };
    m_exceptions.clear();
}

bool LogUtil::openLogFile(const tstring& logFile)
{
    return true;
}

void LogUtil::closeLogFile()
{
}

NS_END
