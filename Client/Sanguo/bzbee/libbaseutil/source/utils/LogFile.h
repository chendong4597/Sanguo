/*!
 * \file LogFile.h
 * \date 9-24-2019
 *
 *
 * \author zjhlogo (zjhlogo@gmail.com)
 */
#pragma once
#include "../file/File.h"
#include <memory>
#include <mutex>

NS_BEGIN

class LogFile
{
public:
    enum CONST_DEFINE
    {
        MAX_CACHE_COUNT = 32,
    };

public:
    LogFile();
    ~LogFile();

    bool isReady() const;

    bool openLogFile(const tstring& logFile);
    void closeLogFile();
    void flush();

    const tstring& getLogFilePath() const;
    bool logString(const tstring& strLog);

private:
    std::shared_ptr<File> m_file;
    tstring m_logFilePath;

    tstring m_logCaches[MAX_CACHE_COUNT];
    int m_currentCacheCount{};

    std::mutex m_mutex;

};

NS_END
